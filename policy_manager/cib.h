#ifndef HEADER_CIB
#define HEADER_CIB

extern node_t* cib_nodes;

void cib_start();
void cib_close();

void generate_cib_from_ifaces();
json_t *cib_lookup(json_t *input_props);

#endif
