#ifndef HEADER_PIB
#define HEADER_PIB

#include "node.h"

extern node_t *pib_policies;
extern node_t *pib_profiles;

void pib_start();
void pib_close();

#endif
