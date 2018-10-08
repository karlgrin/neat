#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jansson.h"
#include <sys/queue.h>

/*----- START cib_node -----*/

typedef struct cib_node_s cib_node_t;

typedef struct linked_cib_node {
    cib_node_t *inner_node;
    LIST_ENTRY(linked_cib_node) pointers;
} linked_cib_node;

struct list {
    linked_cib_node node;
    LIST_HEAD(linked_nodes, linked_cib_node) self;
};

typedef struct cib_node_s {
    char *uid;
    bool root;
    bool link;
    int priority;
    time_t expire;
    char *filename;
    char *description;
    json_t *properties; //PropertyMultiArray
    json_t *match;      //List of PropertyArrays
    struct list linked_nodes;

    bool (*match_entry)(cib_node_t *, json_t);
    bool (*update_links_from_match)(cib_node_t *);
    //TODO
} cib_node_t;

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
    node->expire = time(NULL);
    node->filename = "";
    node->description = "";
    node->properties = malloc(sizeof(json_array));
    node->match = malloc(sizeof(json_array));
    LIST_INIT(&node->linked_nodes.self);

    node->match_entry = match_entry;
    node->update_links_from_match = update_links_from_match;
    return node;
}

/*----- END cib_node -----*/

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
    return 0;
}
