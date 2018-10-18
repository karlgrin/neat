#include <stdbool.h>
#include <time.h>
#include <string.h>
#include <jansson.h>
#include <dirent.h>

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


node_t *
update_ib_node(node_t *ib_node_head, char * file_path)
{
    node_t *node = get_node(ib_node_head, file_path);

    if(node != NULL) {
        if(node->last_updated <= file_edit_time(file_path)) {
            json_t *json = load_json_file(file_path);
            update_node_content(node, json);
        }
    }
    else {  //node does not exist, create it
        ib_node_head = add_node(ib_node_head, create_node(file_path));
    }
    return ib_node_head;
}

node_t *
read_modified_ib_files(node_t *ib_node_head, const char *ib_dir)
{
    DIR *dir;
    struct dirent *ent;
    int file_type = 8;

    if ((dir = opendir (ib_dir)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == file_type) {
                char *file_path = new_string("%s/%s", dir_path, ent->d_name);
                ib_node_head = update_ib_node(ib_node_head, file_path);
                free(file_path);
            }
        }
        closedir (dir);
        return ib_node_head;
    } else {
        write_log(__FILE__, __func__, "Error: Can't read the directory %s", ib_dir);
    }
    return NULL;
}

json_t *
get_node_properties (node_t *node)
{
    return json_object_get(node->json, "properties");
}

json_t *
node_has_property (node_t *node, const char *text_prop)
{
    return json_object_get(get_node_properties(node), text_prop);
}

void
node_set_property (node_t *node, const char *text_prop, json_t *new_value)
{
    if( json_object_set(get_node_properties(node), text_prop, new_value) == -1) { /* -1 on error, 0 on success */
        // TODO log error
    }
}
