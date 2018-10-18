#ifndef HEADER_PIB
#define HEADER_PIB

#include "node.h"

extern node_t *pib_head_node;

void update_pib_node(char * file_path);
void ead_modified_pib_files(const char *dir_path);


#endif