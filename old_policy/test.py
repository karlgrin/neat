#!/usr/bin/env python3.5

import socket
import pmdefaults as PM
import time


if __name__ == "__main__":
    mylist = []
    amount = 10
    sum = 0

    for i in range(amount):
        test_request_str = '[{"remote_ip": {"precedence": 2,"value": "10.54.1.23"}, "port": {"precedence": 2, "value": 8080}, "__pre_resolve": {"value": true}, "transport": {"value": "reliable"}, "MTU": {"value": [1500, 9000]}, "low_latency": {"precedence": 1, "value": true}}]'

        s = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        s.settimeout(10)  # 1 second timeout, after all it should be instant because its local
        try:
            s.connect(PM.DOMAIN_SOCK)
        except (FileNotFoundError, ConnectionRefusedError) as e:
            print("PM: " + e.args[1])
            s.close()
            exit()


        start = time.time()

        s.send(test_request_str.encode())
        s.shutdown(socket.SHUT_WR)
        resp = s.recv(8192)

        end = time.time() 
        elapsed = end - start
        mylist.append(elapsed)
        s.close()

    for i in range(amount):
        print(i, "  :   ", mylist[i]*1000)
        sum += mylist[i]

    print("\nAverage: ", (sum/amount)*1000, "ms\n")
 

