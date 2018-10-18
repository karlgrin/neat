#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pib.h"
#include "node.h"
#include "pmhelper.h"

node_t *pib_head_node = NULL;


/*
int
main ()
{
    pib_head_node = read_modified_ib_files(pib_head_node, PROFILE_DIR);
    // pib_head_node = read_modified_ib_files(pib_head_node, CIB_DIR);
    print_nodes(pib_head_node);
    free_nodes(pib_head_node);

}
*/
