#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pib.h"
#include "node.h"
#include "pmhelper.h"

node_t *pib_profiles = NULL;
node_t *pib_policies = NULL;

void
pib_start() 
{
    pib_profiles = read_modified_ib_files(pib_profiles,PROFILE_DIR);
    pib_policies = read_modified_ib_files(pib_policies,POLICY_DIR);
}

void
pib_close() 
{
    free_nodes(pib_profiles);
    free_nodes(pib_policies);
}



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
