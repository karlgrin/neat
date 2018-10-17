#ifndef HEADER_MODIFIED_FILE
#define HEADER_MODIFIED_FILE

#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <jansson.h>

#include "pmhelper.h"

typedef struct node {
    char *filename;
    time_t last_updated;
    json_t *json;
    struct node *next;
}node_t;

node_t* node_init(const char *file_path);
void free_node(node_t *node);
void free_nodes(node_t *head);

void update_node_content(node_t *node, json_t *json);
node_t* create_node(const char * file_path);

bool has_node(node_t *head, const char *file_path);
node_t* add_node(node_t *head, node_t *node);
void remove_node(node_t **head, const char *file_path);
node_t* get_node(node_t *head, const char *file_path);

void print_nodes(node_t* head);
void print_node(node_t *node);

#endif

