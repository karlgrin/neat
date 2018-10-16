#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "jansson.h"
#include "multi_property.h"
#include "property.h"
#include "cib.h"

bool
match_entry(cib_node_t *self, json_t entry)
{
    //TODO
    return false;
}

bool
update_links_from_match(cib_node_t *self)
{
    //TODO
    return false;
}

cib_node_t *
cib_node_init()
{
    cib_node_t *node = malloc(sizeof(cib_node_t));
    node->uid = "";
    node->root = false;
    node->link = false;
    node->priority = 0;
    node->expire = time(NULL) + CIB_DEFAULT_TIMEOUT;
    node->filename = "";
    node->description = "";
    node->properties = malloc(sizeof(multi_property_t));
    node->match = malloc(sizeof(property_t));
    LIST_INIT(&node->linked_nodes.self);
    return node;
}

/*----- END cib_node -----*/
/*
int
main(int argc, char **argv)
{
    // Testing the list with linked nodes
    cib_node_t *node = cib_node_init();

    linked_cib_node *linked_node = &(linked_cib_node) {
        .inner_node = malloc(sizeof(cib_node_t))
    };

    linked_cib_node *linked_node2 = malloc(sizeof(linked_cib_node));
    LIST_INSERT_HEAD(&node->linked_nodes.self, linked_node, pointers);
    LIST_INSERT_HEAD(&node->linked_nodes.self, linked_node2, pointers);
    LIST_REMOVE(linked_node, pointers);
    LIST_REMOVE(linked_node2, pointers);
    return 0;
}
*/
