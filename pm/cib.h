#ifndef HEADER_CIB
#define HEADER_CIB

#include <sys/queue.h>
#include <time.h>
#include <stdbool.h>

#include "multi_property.h"
#include "property.h"

#define CIB_DEFAULT_TIMEOUT 600;

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
    multi_property_t *properties;
    property_t *match;
    struct list linked_nodes;
    cib_node_t *next;
} cib_node_t;

/*----- END cib_node -----*/

typedef struct cib_s {
    cib_node_t *head;
} cib_t;

#endif