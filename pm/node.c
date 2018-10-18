#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <jansson.h>

#include "pmhelper.h"
#include "node.h"

void
free_node(node_t *node) {
    if(node != NULL) {
        free(node->filename);
        json_decref(node->json);
    }
    free(node); 
}

void
free_nodes(node_t *head) 
{
    node_t  *tmp;

    while (head != NULL) {
       tmp = head;
       head = head->next;
       free_node(tmp);
    } 
}

void
update_node_content(node_t *node, json_t *json) 
{
    if(json != NULL && node != NULL) {
        if(node->json != NULL) {
            json_decref(node->json);
        }
        node->json = json;
        node->last_updated = time(0);
    }
}

node_t*
node_init(const char *file_path) 
{
    node_t *node = calloc(1,sizeof(node_t));

    if(node == NULL) { 
        write_log(__FILE__, __func__, "Failed to calloc..."); 
        return NULL;
    }

    node->filename = strdup(file_path);
    node->last_updated = 0;
    return node;
}

node_t*
create_node(const char * file_path) 
{
    json_t *json = load_json_file(file_path);

    if(json == NULL) { return NULL; }

    node_t* node = node_init(file_path);
    update_node_content(node, json);

    return node;
}


bool
has_node(node_t *head, const char *file_path)
{
    if(file_path == NULL) { return false; }

    node_t *current = head;
    while(current != NULL) {
        if(strcmp(current->filename,file_path) == 0) { 
            return true; 
        }
        current = current->next;
    }
    return false;
}

node_t*
add_node(node_t *head, node_t *node)
{
    if(node == NULL) { return head; }
    if(head == NULL) { return node; }

    node_t *current = head;
    while(current->next != NULL) { current = current->next; }

    current->next = node;
    return head;
}

void
remove_node(node_t **head, const char *file_path)
{
    if(head == NULL || file_path == NULL) { return; }

    node_t *current = *head;
    node_t *previous = NULL;

    while(current != NULL) {
        if(strcmp(current->filename,file_path) == 0) {
            if(previous == NULL) {
                *head = current->next;               
            } 
            else {
                previous->next = current->next;
            }   
            free_node(current);     
            return;
        }
        previous = current;
        current = current->next;
    }
}

node_t*
get_node(node_t *head, const char *file_path)
{
    if(file_path == NULL) { return NULL; }

    node_t *current = head;
    while(current != NULL) {
        if(strcmp(current->filename, file_path) == 0) { 
            return current;
        }
        current = current->next;
    }
    return NULL;
}

//testing
void 
print_nodes(node_t* head) 
{
    node_t *current = head;
    while(current != NULL) {   
        print_node(current);
        current= current->next;
    }
}

void 
print_node(node_t *node) 
{
    if(node != NULL) {
     printf("--------NODE----------\n");
        if(node->filename != NULL) { 
            printf("filename: %s\n", node->filename); 
        }
        printf("last_updated: %s",ctime(&node->last_updated));
        char *json_string = json_dumps(node->json, JSON_INDENT(4));
        printf("json: %s\n" , json_string);
        free(json_string);
    }
}
