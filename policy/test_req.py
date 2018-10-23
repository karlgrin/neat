#!/usr/bin/env python3.5

import socket
import pmdefaults as PM


if __name__ == "__main__":
    test_request_str = '[{"remote_ip": {"precedence": 2,"value": "10.54.1.23"}, "port": {"precedence": 2, "value": 8080}, "__pre_resolve": {"value": true}, "transport": {"value": "reliable"}, "MTU": {"value": [1500, 9000]}, "low_latency": {"precedence": 1, "value": true}}]'

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.settimeout(10)  # 1 second timeout, after all it should be instant because its local
    try:
        s.connect(PM.DOMAIN_SOCK)
    except (FileNotFoundError, ConnectionRefusedError) as e:
        print("PM: " + e.args[1])
        s.close()
        exit()

    print('Connected to PM on %s' % PM.DOMAIN_SOCK)
    print("REQUEST:")
    print(test_request_str)
    s.send(test_request_str.encode())
    s.shutdown(socket.SHUT_WR)

    resp = s.recv(8192)
    s.close()
    print("REPLY:")
    jresp = json.loads(resp.decode())
    for r in jresp:
        print(PropertyArray.from_dict(r))
    print("\n")
#!/usr/bin/env python3.5

import socket
import pmdefaults as PM


if __name__ == "__main__":
    test_request_str = '[{"remote_ip": {"precedence": 2,"value": "10.54.1.23"}, "port": {"precedence": 2, "value": 8080}, "__pre_resolve": {"value": true}, "transport": {"value": "reliable"}, "MTU": {"value": [1500, 9000]}, "low_latency": {"precedence": 1, "value": true}}]'

    s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    s.settimeout(10)  # 1 second timeout, after all it should be instant because its local
    try:
        s.connect(PM.DOMAIN_SOCK)
    except (FileNotFoundError, ConnectionRefusedError) as e:
        print("PM: " + e.args[1])
        s.close()
        exit()

    print('Connected to PM on %s' % PM.DOMAIN_SOCK)
    print("REQUEST:")
    print(test_request_str)
    s.send(test_request_str.encode())
    s.shutdown(socket.SHUT_WR)

    resp = s.recv(8192)
    s.close()
    print("REPLY:")
    jresp = json.loads(resp.decode())
    for r in jresp:
        print(PropertyArray.from_dict(r))
    print("\n")
