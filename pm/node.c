#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <jansson.h>

#include "pmhelper.h"
#include "property.h"
#include "multi_property.h"
#include "node.h"

static void
free_node(node_t *node) {
    if(node->match != NULL) { 
        free_properties(node->match); 
        }
    if(node->properties != NULL) { 
        free_multi_properties(node->properties); 
    }
    free(node); 
}

void
update_node_content(node_t *node, json_t *json) 
{
    if(json != NULL && node != NULL) {
        node-> uid = json_string_value(json_object_get(json, "uid"));    
        node->priority = json_real_value(json_object_get(json, "priority"));
        node->replace_matched = json_boolean_value(json_object_get(json, "replace_matched"));
        node-> match = json_to_property(json_object_get(json, "match")); 
        node-> properties = json_to_multi_property(json_object_get(json, "properties"));
    }
}

static node_t*
node_init(const char *file_path) 
{
    node_t *node = malloc(sizeof(node_t));
    node->filename = file_path;
    node->last_updated = time(0);
    node->priority = 0;
    node->replace_matched = false;
    node->uid = NULL;
    node->match = NULL;
    node->next = NULL;
    
    return node;
}

node_t*
create_node(const char * file_path) 
{
    json_t *json = load_json_file(file_path);

    if(json == NULL) { free(json); return NULL; }

    node_t* node = node_init(file_path);
    update_node_content(node, json);
    free(json);

    return node;
}


bool
contain(node_t *head, const char *file_path)
{
    if(file_path == NULL) { return NULL; }

    node_t *current = head;
    while(current != NULL) {
        if(strcmp(current->filename,file_path) == 0) { 
            return true; 
        }
        current = current->next;
    }
    return false;
}

//add at the start of the list
node_t*
add_node(node_t *head, node_t *node)
{
    if(node == NULL) { return head; }

    node_t *new_head = node; 
    new_head->next = head;
    return new_head;
}

void
remove_node(node_t **head, const char *file_path)
{
    if(head == NULL || file_path == NULL) { return ; }

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
            break;
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
void print_node(node_t* head) 
{
    node_t *current = head;
    while(current != NULL) {   
        printf("--------NODE----------\n");
        if(current->uid != NULL) { printf("uid: %s\n", current->uid); }
        printf("priority: %d\n", current->priority);
        printf("replace_matched: %d\n", current->replace_matched);
        printf("last_updated: %s",ctime(&current->last_updated));
        if(current->filename != NULL) { printf("filename: %s\n", current->filename); }
        if(current->match != NULL) {
            printf("match: \n");
            print_property(current->match);
        }
        if(current->properties != NULL) {
            printf("properties: \n");
            print_multi_property(current->properties);
        }
        printf("\n");
        current= current->next;
    }
}
