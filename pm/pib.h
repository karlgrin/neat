#ifndef HEADER_PIB
#define HEADER_PIB

#include "node.h"

extern node_t *pib_policies;
extern node_t *pib_profiles;

json_t *profile_lookup(json_t *);
json_t *policy_lookup(json_t *);
void pib_start();
void pib_close();
json_t *get_pibnode_by_uid (const char *uid);

#endif
