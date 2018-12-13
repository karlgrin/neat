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
    return pib_array;
}

void
add_pib_node(json_t *json_for_node)
{
    //Check uid, filename, time
    const char *uid = json_string_value(json_object_get(json_for_node, "uid"));
    if(uid == NULL){
        uid = get_hash();
        json_object_set(json_for_node, "uid", json_string(uid));
    }

    char* path = new_string("%s%s%s", pib_dir, uid, ".policy");
    write_log(__FILE__, __func__, LOG_DEBUG, "PIB node created in %s\n", path);
 
    node_t *node = node_init(path);


    if(json_object_get(json_for_node, "filename") == NULL){
        json_object_set(json_for_node, "filename", json_string(new_string("%s.policy", uid)));
    }

    if(json_object_get(json_for_node, "time") == NULL){
        json_object_set(json_for_node, "time", json_integer((int)time(NULL)));
    }
    node->json = json_for_node;
    add_node(pib_policies, node);
    write_json_file(path, node->json);
    free(path);
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
        write_log(__FILE__, __func__, LOG_DEBUG,"\n---------- POLICY %s ---------\n", current_policy->filename);
        candidate_updated_array = json_array();
        policy_match = json_object_get(current_policy->json, "match");

        json_array_foreach(candidate_array, index, candidate) {

            /* no match field becomes a match by default */
            if (!policy_match || subset(policy_match, candidate)) {
                write_log(__FILE__, __func__, LOG_DEBUG,"subset found for %s", current_policy->filename);

                properties_expanded = expand_json(json_object_get(current_policy->json, "properties"));
                replace = replace_matched(current_policy->json);

                json_array_foreach(properties_expanded, index_2, expanded_prop) {
                    write_log(__FILE__, __func__, LOG_DEBUG, "\n    ------ EXPANDED PROP #%ld of %s\n", index_2, current_policy->filename);

                    candidate_updated = json_deep_copy(candidate); // TODO free

                    /* add merged copy to updated array */
                    merge_properties(expanded_prop, candidate_updated, replace);
                    json_array_append_new(candidate_updated_array, candidate_updated);
                }

            }
            else {
                write_log(__FILE__, __func__, LOG_DEBUG, "subset not found for %s\n", current_policy->filename);
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
    pib_profiles = read_modified_files(pib_profiles, profile_dir);
    pib_policies = read_modified_files(pib_policies, policy_dir);
}

void
pib_close()
{
    free_nodes(pib_profiles);
    free_nodes(pib_policies);
}
