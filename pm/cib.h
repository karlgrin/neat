#ifndef HEADER_CIB
#define HEADER_CIB

extern node_t* cib_nodes;

json_t *get_cibnode_by_uid(const char *uid);

void cib_start();
void cib_close();

void generate_cib_from_ifaces();
json_t *cib_lookup(json_t *input_props);

#endif
