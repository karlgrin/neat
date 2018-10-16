#include <stdio.h>
#include <time.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>

#include "node.h"

node_t *head_node = NULL;


void
update_pib_node(const char * file_path)
{
    node_t *node = get_node(head_node, file_path);

    if(node != NULL) {
        if(node->last_updated <= file_edit_time(file_path)) {
            json_t *json = load_json_file(file_path);
            update_node_content(node, json);
            node->last_updated = time(0);
            free(json);
        }
    }
    else {  //node does not exist, create it
        head_node = add_node(head_node, create_node(file_path));
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
                update_pib_node(concat(concat(dir_path, "/"), ent->d_name));
            }
        }
        closedir (dir);
    } else {
        write_log(__FILE__, __func__, concat("Error: Can't read the directory ", dir_path));
    }
}

/*
int main()
{

    read_modified_pib_files("/home/free/Downloads/neat/pm/json_examples/pib");
    print_node(head_node);

    return 0;
}
*/
