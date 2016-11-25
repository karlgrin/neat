#!/usr/bin/env python3.5
import argparse
import asyncio
import hashlib
import json
import logging
import os
from copy import deepcopy
from operator import attrgetter

import pmrest
import policy
from cib import CIB
from pib import PIB
from policy import PropertyMultiArray

parser = argparse.ArgumentParser(description='NEAT Policy Manager')
parser.add_argument('--cib', type=str, default='cib/example/', help='specify directory in which to look for CIB files')
parser.add_argument('--pib', type=str, default='pib/example/', help='specify directory in which to look for PIB files')
parser.add_argument('--sock', type=str, default=None, help='set Unix domain socket')
parser.add_argument('--debug', type=bool, default=True, help='enable debugging')
parser.add_argument('--rest', type=bool, default=True, help='enable REST API')

args = parser.parse_args()

if args.sock:
    DOMAIN_SOCK = args.sock
else:
    DOMAIN_SOCK = os.environ['HOME'] + '/.neat/neat_pm_socket'

PIB_SOCK = os.environ['HOME'] + '/.neat/neat_pib_socket'
CIB_SOCK = os.environ['HOME'] + '/.neat/neat_cib_socket'
REST_PORT = 45888

PIB_DIR = args.pib
CIB_DIR = args.cib

# Make sure the socket does not already exist
try:
    os.unlink(DOMAIN_SOCK)
    os.unlink(PIB_SOCK)
    os.unlink(CIB_SOCK)
except OSError:
    if os.path.exists(DOMAIN_SOCK):
        raise


def process_request(json_str, num_candidates=10):
    """Process JSON requests from NEAT logic"""
    logging.debug(json_str)

    # list which will hold all requests
    requests = []
    try:
        properties_list = policy.json_to_properties(json_str)
    except policy.InvalidPropertyError:
        return

    try:
        for request in properties_list:
            # we create a PropertyMultiArray first and then expand it to get a list of PropertyArrays
            # with all permutations of requested properties
            pma = PropertyMultiArray()
            for p in request:
                pma.add(p)
            requests.extend(pma.expand())


    except policy.NEATPropertyError as e:
        print(e)
        return

    # local_endpoint handling
    # let's try to avoid any other special handling of properties!
    for r in requests:
        if 'local_endpoint' in r:
            # the local_endpoint property has the format a.b.c.d@eth0 so we need to split it
            local_endpoint = r.get('local_endpoint')
            ip, eth = local_endpoint.value.split('@')

            # create two new NEATProperties for the ip and interfaces
            local_ip = deepcopy(local_endpoint)
            local_ip.key = 'local_ip'
            local_ip.value = ip
            r.add(local_ip)

            interface = deepcopy(local_endpoint)
            interface.key = 'interface'
            interface.value = eth
            r.add(interface)

            del r['local_endpoint']

        # FIXME
        p_cached = policy.NEATProperty(('cached', False), precedence=policy.NEATProperty.OPTIONAL, score=1.0)

        r.add(p_cached)

    print('Received %d NEAT requests' % len(requests))
    # for i, request in enumerate(requests):
    #    print("%d: " % i, request)

    candidates = []

    # main lookup sequence
    for i, request in enumerate(requests):
        logging.info("\n")
        logging.info("------ processing request %d/%d -------" % (i + 1, len(requests)))
        logging.info("    %s" % request)

        print('Profile lookup...')
        updated_requests = profiles.lookup(request)
        for ur in updated_requests:
            logging.debug("updated request %s" % (ur))

        cib_candidates = []
        print('CIB lookup...')
        for ur in updated_requests:
            cib_candidates.extend(cib.lookup(ur))

        cib_candidates.sort(key=attrgetter('score'), reverse=True)
        logging.debug('CIB lookup returned %d candidates:' % len(cib_candidates))
        for c in cib_candidates:
            logging.debug('   %s %.1f %.1f' % (c, *c.score))

        print('PIB lookup...')
        for j, candidate in enumerate(cib_candidates):
            candidates.extend(pib.lookup(candidate, cand_id=i + 1))

        for c in candidates:  # XXXX
            print(' -~>   ', c)

    candidates.sort(key=attrgetter('score'), reverse=True)
    logging.info("%d candidates generated in total. Top %d:" % (len(candidates), num_candidates))

    for candidate in candidates[:num_candidates]:
        print(candidate, candidate.score)
    # TODO check if candidates contain the minimum src/dst/transport tuple

    return candidates[:num_candidates]


class PIBProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        peername = transport.get_extra_info('sockname')
        self.transport = transport
        self.slim = ''

    def data_received(self, data):
        self.slim += data.decode()

    def eof_received(self):
        logging.info("New PIB object received (%dB)." % len(self.slim))
        # TODO do a sanity check
        try:
            json_slim = json.loads(self.slim)
        except json.decoder.JSONDecodeError:
            logging.warning('invalid PIB file format')
            return

        filename = json_slim.get('uid')
        if not filename:
            # generate hash policy filename
            filename = hashlib.md5('json_slim'.encode('utf-8')).hexdigest()

        filename = filename.lower()

        filename = os.path.join(PIB_DIR, '%s.policy' % filename)

        f = open(filename, 'w')
        f.write(self.slim)
        f.close()
        logging.info("Policy saved as \"%s\"." % filename)

        self.transport.close()
        pib.reload()


class CIBProtocol(asyncio.Protocol):
    def __init__(self):
        self.transport = None
        self.slim = ''

    def connection_made(self, transport):
        self.transport = transport

    def data_received(self, data):
        self.slim += data.decode()

    def eof_received(self):
        logging.info("New CIB object received (%dB)" % len(self.slim))

        cib.import_json(self.slim)

        self.transport.close()


class PMProtocol(asyncio.Protocol):
    def connection_made(self, transport):
        self.transport = transport
        self.request = ''

    def data_received(self, data):
        message = data.decode()
        self.request += message

    def eof_received(self):
        logging.info("New JSON request received (%dB)" % len(self.request))
        candidates = process_request(self.request.strip())

        # create JSON string for NEAT logic reply
        try:
            j = [policy.properties_to_json(c) for c in candidates]
            candidates_json = '[' + ', '.join(j) + ']\n'
        except TypeError:
            return

        data = candidates_json.encode(encoding='utf-8')

        self.transport.write(data)
        self.transport.close()


def no_loop_test():
    """
    Dummy JSON request for testing
    """
    # test_request_str = '{"remote_ip": {"precedence": 2, "value": "10:54:1.23"}, "transport": [{"value": "TCP", "banned": ["UDP", "UDPLite"]}, {"value": "UDP"}], "MTU": {"value": [1500, 9000]}, "low_latency": {"precedence": 2, "value": true}, "foo": {"banned": ["baz"]}}'

    test_request_str = '{"remote_ip": {"value": "192.168.113.24", "precedence": 2}, "transport": {"value": "reliable", "precedence": 2}}'

    # SDN
    test_request_str = '{"remote_ip": {"value": "203.0.113.23", "precedence": 2}, "transport": {"value": "reliable", "precedence": 2}, "remote_port": {"value": 80}}'
    process_request(test_request_str)

    import code
    code.interact(local=locals(), banner='debug')


if __name__ == "__main__":
    logging.debug("PIB directory is %s" % PIB_DIR)
    logging.debug("CIB directory is %s" % CIB_DIR)

    cib = CIB(CIB_DIR)
    profiles = PIB(PIB_DIR, file_extension='.profile')
    pib = PIB(PIB_DIR, file_extension='.policy')

    # FIXME: REMOVE only for local testing
    # no_loop_test()

    loop = asyncio.get_event_loop()

    # Each client connection creates a new protocol instance
    coro = loop.create_unix_server(PMProtocol, DOMAIN_SOCK)
    server = loop.run_until_complete(coro)

    coro_pib = loop.create_unix_server(PIBProtocol, PIB_SOCK)
    pib_server = loop.run_until_complete(coro_pib)

    coro_cib = loop.create_unix_server(CIBProtocol, CIB_SOCK)
    cib_server = loop.run_until_complete(coro_cib)

    pmrest.init_rest_server(loop, pib, cib, rest_port=REST_PORT)

    print('Waiting for PM requests on {} ...'.format(server.sockets[0].getsockname()))
    try:
        loop.run_forever()
    except KeyboardInterrupt:
        print("Quitting policy manager.")
        pass
    # TODO implement http://aiohttp.readthedocs.io/en/stable/web.html#graceful-shutdown

    # Close the server
    server.close()
    pib_server.close()
    cib_server.close()
    pmrest.close()

    loop.run_until_complete(server.wait_closed())
    loop.close()
    exit(0)
