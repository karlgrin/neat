#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "jansson.h"
#include <sys/queue.h>

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
} CIBNode_t;

CIBNode_t *CIBNode_constructor(){
    CIBNode_t *node = malloc(sizeof(CIBNode_t));
    node->root = false;
    node->link = false;
    node->priority = 0;
    node->expire = time(NULL);
    LIST_INIT(&node->linked_nodes.self);
    return node;
}

int main(int argc, char **argv) {
    // Testing the list with linked nodes
    CIBNode_t *node = CIBNode_constructor();

    linked_CIBNode *linked_node = &(linked_CIBNode) {
        .inner_node = malloc(sizeof(CIBNode_t))
    };

    linked_CIBNode *linked_node2 = malloc(sizeof(linked_CIBNode));
    LIST_INSERT_HEAD(&node->linked_nodes.self, linked_node, pointers);
    LIST_INSERT_HEAD(&node->linked_nodes.self, linked_node2, pointers);
    return 0;
}