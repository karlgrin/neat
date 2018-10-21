#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pib.h"
#include "node.h"
#include "pmhelper.h"

node_t *pib_profiles = NULL;
node_t *pib_policies = NULL;

/* TODO move subset & merge to node.c */

/* check if a is a subset of b */
int
subset(json_t *prop_a, json_t *prop_b)
{
    const char *key_a;
    json_t *value_a;

    const char *key_b;
    json_t *value_b;

    int found = 0;

    json_object_foreach(prop_a, key_a, value_a) {
        if (json_object_get(prop_b, key_a) == NULL) {
            return 0;
        }
    }
    return 1;
}

/* merge properties in prop_b into prop_a */
void
merge_properties(json_t *prop_a, json_t *prop_b, int should_overwrite)
{
    const char *key;
    json_t *value;

    json_object_foreach(prop_b, key, value) {
        json_t *found_prop = json_object_get(prop_a, key);
        if (found_prop && should_overwrite) {
            // TODO overwrite value
            printf("setting { %s : %s } to a value of %s\n", key, "something", value);
        }
        else {
            json_object_set(prop_a, key, value);
        }
    }
}

json_t *
pib_lookup(node_t *pib_list, json_t *input_props)
{
    node_t *current_policy  = NULL;
    json_t *candidate_array = json_array();

    /* array variables */
    size_t index;
    json_t *candidate;

    json_array_append(candidate_array, input_props);

    for (current_policy = pib_list; current_policy; current_policy = current_policy->next) {
        json_array_foreach(candidate_array, index, candidate) {
            // TODO if match is empty (wildcard) then the properties should be added
            json_t *policy_match = json_object_get(current_policy->json, "match");

            if (!policy_match) {
                printf("No match field found = default profile\n");
            }

            /* no match field becomes a match by default */
            if (!policy_match || subset(policy_match, json_object_get(candidate, "properties"))) {
                // TODO expand properties
                // json_t *properties_expanded = expand(properties);

                printf("subset found\n");

                // TODO add to candidate
                // copy json cand to cand_updated
                // merge cand_updated with props
                // add to cand array

                // json_t *cand_updated = json_deep_copy(candidate);
                // merge_properties(cand_updated,
                // json_array_append(candidate_array, cand_updated);
            }
            else {
                printf("subset not found\n");
            }
        }
    }
    return candidate_array; /* TODO free */
}

json_t *
policy_lookup(json_t *input_props)
{
    return pib_lookup(pib_policies, input_props);
}

json_t *
profile_lookup(json_t *input_props)
{
    return pib_lookup(pib_profiles, input_props);
}

void
pib_start()
{
    char *path;

    path = new_string("%s/%s/%s", get_home_dir(), ".neat", PROFILE_DIR);
    pib_profiles = read_modified_files(pib_profiles, path);

    path = new_string("%s/%s/%s", get_home_dir(), ".neat", POLICY_DIR);
    pib_policies = read_modified_files(pib_policies, path);

    free(path);
}

void
pib_close()
{
    free_nodes(pib_profiles);
    free_nodes(pib_policies);
}
