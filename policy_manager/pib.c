#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pib.h"
#include "node.h"
#include "pm_helper.h"

#include "parse_json.h"

node_t *pib_profiles = NULL;
node_t *pib_policies = NULL;

void
get_pib_list_aux (json_t *pib_array, node_t *head)
{
    if(head != NULL) {
        json_array_append(pib_array, json_object_get(head->json, "uid"));
        get_pib_list_aux(pib_array, head->next);
    }
}

json_t *
get_pib_list ()
{
    json_t *pib_array = json_array();
    get_pib_list_aux(pib_array, pib_policies);
    printf("\n%s\n", json_dumps(pib_array, 2));
    return pib_array;
}

void
add_pib_node(node_t *node, char *path)
{
    //Check uid, filename, time
    add_node(pib_policies, node);
    //write_json_file(path, node->json);
    //free(path);
}

json_t *
get_pibnode_by_uid(const char *uid)
{
    node_t *pib;
    pib = get_node_by_uid(pib_policies, uid);
    if (pib) {
        return pib->json;
    }
    return NULL;
}
/* TODO move subset & merge to pm_helper.c */

int
replace_matched(json_t *policy)
{
    json_t *replace_matched = json_object_get(policy, "replace_matched");
    if (replace_matched) {
        return json_is_true(replace_matched);
    }
    return 0; /* TODO what is the default value? */
}

json_t *
pib_lookup(node_t *pib_list, json_t *input_props)
{
    node_t *current_policy  = NULL;
    json_t *candidate_array = json_array();
    json_t *candidate_updated_array;
    json_t *policy_match;

    /* expand */
    json_t *properties_expanded;
    json_t *expanded_prop;
    json_t *candidate_updated;
    size_t index_2;
    int replace;

    /* array variables */
    size_t index;
    json_t *candidate;

    json_array_append(candidate_array, input_props);

    for (current_policy = pib_list; current_policy; current_policy = current_policy->next) {
        printf("\n---------- POLICY %s ---------\n", current_policy->filename);
        candidate_updated_array = json_array();
        policy_match = json_object_get(current_policy->json, "match");

        if (policy_match) { printf("MATCH: %s\n", json_dumps(policy_match, 0)); }

        printf("size of candidate_array is %ld\n", json_array_size(candidate_array));

        json_array_foreach(candidate_array, index, candidate) {

            /* just for the debug print */
            if (!policy_match) {
                printf("no match field found = default profile\n");
            }

            /* no match field becomes a match by default */
            if (!policy_match || subset(policy_match, candidate)) {
                printf("subset found for %s", current_policy->filename);
                printf("merging...\n");

                properties_expanded = expand_json(json_object_get(current_policy->json, "properties"));
                replace = replace_matched(current_policy->json);

                json_array_foreach(properties_expanded, index_2, expanded_prop) {
                    printf("\n    ------ EXPANDED PROP #%ld\n    >> VALUE: %s\n", index_2, json_dumps(expanded_prop, 0));

                    candidate_updated = json_deep_copy(candidate); // TODO free

                    /* add merged copy to updated array */
                    merge_properties(expanded_prop, candidate_updated, replace);
                    json_array_append_new(candidate_updated_array, candidate_updated);
                    printf("size of candidate_updated_array is %ld\n", json_array_size(candidate_updated_array));
                }

            }
            else {
                printf("subset not found for %s\n", current_policy->filename);
                json_array_append_new(candidate_updated_array, candidate);
            }
        }
        candidate_array = json_deep_copy(candidate_updated_array);
        json_decref(candidate_updated_array);

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
    pib_profiles = read_modified_files(pib_profiles, PROFILE_DIR);
    pib_policies = read_modified_files(pib_policies, POLICY_DIR);
}

void
pib_close()
{
    free_nodes(pib_profiles);
    free_nodes(pib_policies);
}
