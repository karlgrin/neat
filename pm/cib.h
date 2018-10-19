#ifndef HEADER_CIB
#define HEADER_CIB

extern node_t* cib_head;

void cib_start();
void cib_close();

void generate_cib_from_ifaces();

#endif