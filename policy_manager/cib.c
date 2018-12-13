#define _GNU_SOURCE

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <netinet/in.h>
#include <netdb.h>
#include <jansson.h>

#include "pm_helper.h"
#include "node.h"
#include "parse_json.h"

node_t* cib_nodes = NULL;

void
get_cib_list_aux(json_t *cib_array, node_t *head)
{
    if(head != NULL) {
        json_array_append(cib_array, json_object_get(head->json, "uid"));
        get_cib_list_aux(cib_array, head->next);
    }
}

json_t *
get_cib_list()
{
    json_t *cib_array = json_array();
    get_cib_list_aux(cib_array, cib_nodes);
    return cib_array;
}

json_t *
get_cibnode_by_uid(const char *uid)
{
    node_t *cib;
    cib = get_node_by_uid(cib_nodes, uid);
    if (cib) {
        return cib->json;
    }
    return NULL;
}

void
add_cib_node(json_t *json_for_node)
{
    const char *uid = json_string_value(json_object_get(json_for_node, "uid"));
    if(uid == NULL){
        uid = get_hash();
        json_object_set(json_for_node, "uid", json_string(uid));
    }

    const char *filename = json_string_value(json_object_get(json_for_node, "filename"));
    if(filename == NULL){
        filename = new_string("%s.cib", uid);
        json_object_set(json_for_node, "filename", json_string(filename));
    }
    char *path = new_string("%s%s", CIB_DIR, filename);
    node_t *node = node_init(path);
    write_log(__FILE__, __func__, LOG_DEBUG, "Path to add cib node: %s\n", path);

    if(json_object_get(json_for_node, "description") == NULL){
        json_object_set(json_for_node, "description", json_string(""));
    }
    if(json_object_get(json_for_node, "priority") == NULL){
        json_object_set(json_for_node, "priority", json_integer(0));
    }
    if(json_object_get(json_for_node, "root") == NULL){
        json_object_set(json_for_node, "root", json_boolean(false));
    }
    if(json_object_get(json_for_node, "expire") == NULL){
        double expiry = time(NULL) + CIB_DEFAULT_TIMEOUT;
        json_object_set(json_for_node, "expire", json_real(expiry));
    }
    node->json = json_for_node;
    add_node(cib_nodes, node);
    write_json_file(path, node->json);
    free(path);
}

void
generate_cib_from_ifaces()
{
    write_log(__FILE__, __func__, LOG_EVENT, "Generate CIB from interfaces:");

    struct ifaddrs *ifaddr, *interface;
    struct if_nameindex *if_nidxs, *iface;
    int family, s, n;
    char address[NI_MAXHOST];
    void *iter;

    json_t *root = json_object();

    if ((if_nidxs = if_nameindex()) == NULL )
    {
        write_log(__FILE__, __func__, LOG_ERROR, "if_nameindex() failed");
    }
    if (getifaddrs(&ifaddr) == -1) {
        write_log(__FILE__, __func__, LOG_ERROR, "getifaddrs() failed");
    }
    for (iface = if_nidxs; iface->if_index != 0 || iface->if_name != NULL; iface++)
    {
        char* desc = new_string("%s%s", "autogenerated CIB node for local interface ", iface->if_name);
        char* filename = new_string("%s%s", iface->if_name, ".cib");

        json_object_set_new(root, iface->if_name, json_object());
        json_object_set_new(json_object_get(root, iface->if_name), "description", json_string(desc));
        json_object_set_new(json_object_get(root, iface->if_name), "filename", json_string(filename));
        json_object_set_new(json_object_get(root, iface->if_name), "expire", json_integer(-1.0));
        json_object_set_new(json_object_get(root, iface->if_name), "link", json_boolean(false));
        json_object_set_new(json_object_get(root, iface->if_name), "priority", json_integer(0));
        json_object_set_new(json_object_get(root, iface->if_name), "properties", json_array());
        json_array_append_new(json_object_get(json_object_get(root, iface->if_name), "properties"), json_array());
        json_array_append_new(json_object_get(json_object_get(root, iface->if_name), "properties"), json_array());

        json_t *interface_array = json_array_get(json_object_get(json_object_get(root, iface->if_name), "properties"), 0);
        json_array_append_new(interface_array, json_object());

        json_object_set_new(json_array_get(interface_array, 0), "interface", json_object());
        json_object_set_new(json_object_get(json_array_get(interface_array, 0), "interface"), "precedence", json_integer(2));
        json_object_set_new(json_object_get(json_array_get(interface_array, 0), "interface"), "value", json_string(iface->if_name));
        json_object_set_new(json_array_get(interface_array, 0), "local_interface", json_object());
        json_object_set_new(json_object_get(json_array_get(interface_array, 0), "local_interface"), "precedence", json_integer(2));
        json_object_set_new(json_object_get(json_array_get(interface_array, 0), "local_interface"), "value", json_boolean(true));

        free(filename); free(desc);
    }

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
            write_log(__FILE__, __func__, LOG_ERROR, "getnameinfo() failed: %s", gai_strerror(s));
        }

        iter = json_object_iter(root);
        while(iter){
            if(strcmp(json_object_iter_key(iter), interface->ifa_name) == 0){
                json_t *ip_array = json_array_get(json_object_get(json_object_get(root, interface->ifa_name), "properties"), 1);

                if(family == AF_INET){
                    json_array_append_new(ip_array, json_object());
                    json_object_set_new(json_array_get(ip_array, 0), "ip_version", json_object());
                    json_object_set_new(json_object_get(json_array_get(ip_array, 0), "ip_version"), "precedence", json_integer(2));
                    json_object_set_new(json_object_get(json_array_get(ip_array, 0), "ip_version"), "value", json_integer(4));
                    json_object_set_new(json_array_get(ip_array, 0), "local_ip", json_object());
                    json_object_set_new(json_object_get(json_array_get(ip_array, 0), "local_ip"), "precedence", json_integer(2));
                    json_object_set_new(json_object_get(json_array_get(ip_array, 0), "local_ip"), "value", json_string(address));
                } else if (family == AF_INET6){
                    json_array_append_new(ip_array, json_object());
                    json_object_set_new(json_array_get(ip_array, 1), "ip_version", json_object());
                    json_object_set_new(json_object_get(json_array_get(ip_array, 1), "ip_version"), "precedence", json_integer(2));
                    json_object_set_new(json_object_get(json_array_get(ip_array, 1), "ip_version"), "value", json_integer(6));
                    json_object_set_new(json_array_get(ip_array, 1), "local_ip", json_object());
                    json_object_set_new(json_object_get(json_array_get(ip_array, 1), "local_ip"), "precedence", json_integer(2));
                    json_object_set_new(json_object_get(json_array_get(ip_array, 1), "local_ip"), "value", json_string(address));
                }
            }
            iter = json_object_iter_next(root, iter);
        }
    }

    iter = json_object_iter(root);
    while(iter){
        json_object_set_new(json_object_get(root, json_object_iter_key(iter)), "root", json_boolean(true));
        json_object_set_new(json_object_get(root, json_object_iter_key(iter)), "uid", json_string(json_object_iter_key(iter)));

        char* path = new_string("%s%s%s", CIB_DIR, json_object_iter_key(iter), ".cib");
        write_json_file(path, json_object_get(root, json_object_iter_key(iter)));
        write_log(__FILE__, __func__, LOG_EVENT, "%s", path);
        free(path);

        iter = json_object_iter_next(root, iter);
    }
    write_log(__FILE__, __func__, LOG_NEW_LINE, "\n");  //just to make console look nice

    freeifaddrs(ifaddr);
    freeifaddrs(interface);
    if_freenameindex(if_nidxs);
    json_decref(root);
}

json_t *
cib_lookup(json_t *input_props)
{
    node_t *current_node = NULL;
    json_t *candidate_array = json_array();
    json_t *node_props;
    json_t *immutable_input_props = json_array();

    const char *key;
    json_t *property;

    json_array_append(candidate_array, input_props);

    for(current_node = cib_nodes; current_node; current_node = current_node->next){
        //If every IMMUTABLE value in input_props are in current_node->properties, continue
        node_props = json_object_get(current_node->json, "properties");

        json_array_clear(immutable_input_props);
        json_object_foreach(input_props, key, property){
            if(json_integer_value(json_object_get(property, "precedence")) == 2){
                json_array_append(immutable_input_props, property);
            }
        }

        if(subset(node_props, immutable_input_props)){
            continue;
        }
    }
    return candidate_array;
}

void
cib_start()
{
    cib_nodes = read_modified_files(cib_nodes, CIB_DIR);
}

void
cib_close()
{
    free_nodes(cib_nodes);
}
