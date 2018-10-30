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
update_node(node_t *head, char * file_path)
{
    node_t *node = get_node(head, file_path);

    if(node != NULL) {
        if(node->last_updated <= file_edit_time(file_path)) {
            printf("Updating node content: %s \n", file_path);
            json_t *json = load_json_file(file_path);
            update_node_content(node, json);
        }
    }
    else {  //node does not exist, create it
        printf("Create node: %s \n", file_path);
        head = add_node(head, create_node(file_path));
    }
    return head;
}

node_t *
read_modified_files(node_t *head, const char *dir_path)
{
    DIR *dir;
    struct dirent *ent;
    int file_type = 8;

    if ((dir = opendir (dir_path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == file_type) {
                char *file_path = new_string("%s%s", dir_path, ent->d_name);
                head = update_node(head, file_path);
                free(file_path);
            }
        }
        closedir (dir);
        return head;
    } else {
        write_log(__FILE__, __func__, "Error: Can't read the directory %s", dir_path);
    }
    return NULL;
}

json_t *
get_node_properties (node_t *node)
{
    if(node != NULL) {
        return json_object_get(node->json, "properties");
    }
    return NULL;
}

json_t *
node_has_property (node_t *node, const char *prop)
{
    return json_object_get(get_node_properties(node), prop);
}

void
node_set_property (node_t *node, const char *prop, json_t *new_value)
{
    if(json_object_set(get_node_properties(node), prop, new_value) == -1) {
        write_log(__FILE__,__func__, "Error: was unable to update property %s", prop);
    }
}

/* check if a is a subset of b */
int
subset(json_t *prop_a, json_t *prop_b)
{
    const char *key_a;
    json_t *value_prop_a;
    json_t *value_prop_b;
    json_t *value_a;
    json_t *value_b;

    printf("\nSUBSET check between\n%s\nand\n%s\n\n", json_dumps(prop_a,0), json_dumps(prop_b,0));

    json_object_foreach(prop_a, key_a, value_prop_a) {
        printf("checking if %s is a member of candidate... ", key_a);

        value_prop_b = json_object_get(prop_b, key_a);

        if (value_prop_b == NULL) {
            printf("it's not, this is therefore not a subset (FAIL)\n");
            return 0;
        }
        value_b = json_object_get(value_prop_b, "value");
        value_a = json_object_get(value_prop_a, "value");

        if (json_equal(value_a, value_b)) {
            printf("it is (same key and value)\n");
        }
        else {
            printf("it's not (same key, but different value)\n");
            return 0;
        }
    }
    printf("all properties are members. this is a subset (OK).\n");
    return 1;
}

/* merge properties in prop_a into prop_b */
void
merge_properties(json_t *prop_a, json_t *prop_b, int should_overwrite)
{
    const char *key;
    json_t *value;

    json_object_foreach(prop_a, key, value) {
        json_t *found_prop = json_object_get(prop_b, key);

        if (found_prop) {
            printf("found prop %s", key);
            if (!should_overwrite) {
                printf(", but it's write protected\n");
                continue;
            }
            printf(", setting { %s : %s } to %s\n", key, json_dumps(found_prop, 0), json_dumps(value, 0));
            json_object_set(prop_b, key, value);
        }
        else {
            printf("didn't find property %s adding it as a new value\n", key);
            json_object_set(prop_b, key, value);
        }
    }
}

