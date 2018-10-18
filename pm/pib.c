#include <stdio.h>
#include <time.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>

#include "node.h"

node_t *pib_head_node = NULL;


void
update_pib_node(char * file_path)
{
    node_t *node = get_node(pib_head_node, file_path);

    if(node != NULL) {
        if(node->last_updated <= file_edit_time(file_path)) {
            json_t *json = load_json_file(file_path);
            update_node_content(node, json);
            json_decref(json);
        }
    }
    else {  //node does not exist, create it
        pib_head_node = add_node(pib_head_node, create_node(file_path));
    }
}

void
read_modified_pib_files(const char *dir_path)
{
    DIR *dir;
    struct dirent *ent;
    int file_type = 8;

    if ((dir = opendir (dir_path)) != NULL) {
        while ((ent = readdir (dir)) != NULL) {
            if (ent->d_type == file_type) {
                char *s = concat_3(dir_path, "/", ent->d_name);
                update_pib_node(s);
                free(s);
            }
        }
        closedir (dir);
    } else {
        write_log(__FILE__, __func__, concat("Error: Can't read the directory ", dir_path));
    }
}

int 
main () 
{
    read_modified_pib_files("/home/free/Downloads/neat/pm/json_examples/pib");
    print_nodes(pib_head_node);
    free_nodes(pib_head_node);
}
