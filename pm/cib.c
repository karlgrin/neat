#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jansson.h"
#include <sys/queue.h>

/*----- START CIBNode -----*/

typedef struct CIBNode_s CIBNode_t;

typedef struct linked_CIBNode {
    CIBNode_t *inner_node;
    LIST_ENTRY(linked_CIBNode) pointers;
} linked_CIBNode;

struct list {
    linked_CIBNode node;
    LIST_HEAD(linked_nodes, linked_CIBNode) self;
};

typedef struct CIBNode_s {
    char *uid;
    bool root;
    bool link;
    int priority;
    time_t expire;
    char *filename;
    char *description;
    json_t *properties;
    json_t *match;
    struct list linked_nodes;

    bool (*match_entry)(CIBNode_t *, json_t);
    bool (*update_links_from_match)(CIBNode_t *);
    //TODO
} CIBNode_t;

bool
match_entry(CIBNode_t *self, json_t entry){
    //TODO
    return false;
}

bool
update_links_from_match(CIBNode_t *self){
    //TODO
    return false;
}

CIBNode_t *CIBNode_init(){
    CIBNode_t *node = malloc(sizeof(CIBNode_t));
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

/*----- END CIBNode -----*/

int
main(int argc, char **argv) {
    // Testing the list with linked nodes
    CIBNode_t *node = CIBNode_init();

    linked_CIBNode *linked_node = &(linked_CIBNode) {
        .inner_node = malloc(sizeof(CIBNode_t))
    };

    linked_CIBNode *linked_node2 = malloc(sizeof(linked_CIBNode));
    LIST_INSERT_HEAD(&node->linked_nodes.self, linked_node, pointers);
    LIST_INSERT_HEAD(&node->linked_nodes.self, linked_node2, pointers);
    return 0;
}
