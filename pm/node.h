#ifndef HEADER_MODIFIED_FILE
#define HEADER_MODIFIED_FILE

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>

#include "multi_property.h"
#include "pmhelper.h"

typedef struct node {
    const char *filename;
    time_t last_updated;

    const char *uid;
    int priority;
    bool replace_matched;        
    property_t *match;              //PropertyList
    multi_property_t *properties;   //PropertyMultiList

    struct node *next;
}node_t;

void update_node_content(node_t *node, json_t *json);
node_t* create_node(const char * file_path);
bool contain(node_t *head, const char *file_path);
node_t* add_node(node_t *head, node_t *node);
void remove_node(node_t **head, const char *file_path);
node_t* get_node(node_t *head, const char *file_path);
void print_node(node_t* head);

#endif

