#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netdb.h>

#include "jansson.h"
#include "pmhelper.h"

int CIB_DEFAULT_TIMEOUT = 10*60;

void
generate_cib_from_ifaces()
{
    struct ifaddrs *ifaddr, *interface;
    struct if_nameindex *if_nidxs, *iface;
    int family, s, n;
    char address[NI_MAXHOST];
    void *iter;

    json_t *root = json_object();

    if ((if_nidxs = if_nameindex()) == NULL )
    {
        //TODO: Write to error log
    }
    if (getifaddrs(&ifaddr) == -1) {
        //TODO: Write to error log
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    //For every iface name
    for (iface = if_nidxs; iface->if_index != 0 || iface->if_name != NULL; iface++)
    {
        json_object_set(root, iface->if_name, json_object());
        json_object_set(json_object_get(root, iface->if_name), "description", json_string(new_string("autogenerated CIB node for local interface", iface->if_name)));
        json_object_set(json_object_get(root, iface->if_name), "filename", json_string(new_string(iface->if_name, ".cib")));
        json_object_set(json_object_get(root, iface->if_name), "expire", json_integer(-1.0));
        json_object_set(json_object_get(root, iface->if_name), "link", json_boolean(false));
        json_object_set(json_object_get(root, iface->if_name), "priority", json_integer(0));
        json_object_set(json_object_get(root, iface->if_name), "properties", json_array());
        json_array_append(json_object_get(json_object_get(root, iface->if_name), "properties"), json_array());
        json_array_append(json_object_get(json_object_get(root, iface->if_name), "properties"), json_array());

        json_t *interface_array = json_array_get(json_object_get(json_object_get(root, iface->if_name), "properties"), 0);
        json_array_append(interface_array, json_object());
        json_object_set(json_array_get(interface_array, 0), "interface", json_object());
        json_object_set(json_object_get(json_array_get(interface_array, 0), "interface"), "precedence", json_integer(2));
        json_object_set(json_object_get(json_array_get(interface_array, 0), "interface"), "value", json_string(iface->if_name));
        json_object_set(json_array_get(interface_array, 0), "local_interface", json_object());
        json_object_set(json_object_get(json_array_get(interface_array, 0), "local_interface"), "precedence", json_integer(2));
        json_object_set(json_object_get(json_array_get(interface_array, 0), "local_interface"), "value", json_boolean(true));
    }

    //For every iface name + ip version combination
    for (interface = ifaddr, n = 0; interface != NULL; interface = interface->ifa_next, n++) {
        if (interface->ifa_addr == NULL || interface->ifa_addr->sa_family == AF_PACKET)
            continue;

        family = interface->ifa_addr->sa_family;

        s = getnameinfo(interface->ifa_addr,
                    (family == AF_INET) ? sizeof(struct sockaddr_in) :
                                            sizeof(struct sockaddr_in6),
                    address, NI_MAXHOST,
                    NULL, 0, NI_NUMERICHOST);
        if (s != 0) {
            printf("getnameinfo() failed: %s\n", gai_strerror(s));
            exit(EXIT_FAILURE);
        }

        iter = json_object_iter(root);
        while(iter){
            if(strcmp(json_object_iter_key(iter), interface->ifa_name) == 0){
                json_t *ip_array = json_array_get(json_object_get(json_object_get(root, interface->ifa_name), "properties"), 1);

                if(family == AF_INET){
                    json_array_append(ip_array, json_object());
                    json_object_set(json_array_get(ip_array, 0), "ip_version", json_object());
                    json_object_set(json_object_get(json_array_get(ip_array, 0), "ip_version"), "precedence", json_integer(2));
                    json_object_set(json_object_get(json_array_get(ip_array, 0), "ip_version"), "value", json_integer(4));
                    json_object_set(json_array_get(ip_array, 0), "local_ip", json_object());
                    json_object_set(json_object_get(json_array_get(ip_array, 0), "local_ip"), "precedence", json_integer(2));
                    json_object_set(json_object_get(json_array_get(ip_array, 0), "local_ip"), "value", json_string(address));
                } else if (family == AF_INET6){
                    json_array_append(ip_array, json_object());
                    json_object_set(json_array_get(ip_array, 1), "ip_version", json_object());
                    json_object_set(json_object_get(json_array_get(ip_array, 1), "ip_version"), "precedence", json_integer(2));
                    json_object_set(json_object_get(json_array_get(ip_array, 1), "ip_version"), "value", json_integer(6));
                    json_object_set(json_array_get(ip_array, 1), "local_ip", json_object());
                    json_object_set(json_object_get(json_array_get(ip_array, 1), "local_ip"), "precedence", json_integer(2));
                    json_object_set(json_object_get(json_array_get(ip_array, 1), "local_ip"), "value", json_string(address));
                }
            }
            iter = json_object_iter_next(root, iter);
        }
    }
    iter = json_object_iter(root);
    while(iter){
        json_object_set(json_object_get(root, json_object_iter_key(iter)), "root", json_boolean(true));
        json_object_set(json_object_get(root, json_object_iter_key(iter)), "uid", json_string(json_object_iter_key(iter)));
        write_json_file(new_string(new_string("/home/samulars/", json_object_iter_key(iter)), ".cib"), root); //TODO: CHange to working folder
        iter = json_object_iter_next(root, iter);
    }
    json_decref(root);
    freeifaddrs(ifaddr);
    return ;
}

/*
int
main(int argc, char **argv)
{
    generate_cib_from_ifaces();
    return 0;
}
*/
