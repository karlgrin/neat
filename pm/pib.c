#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pib.h"
#include "node.h"
#include "pmhelper.h"

#include "parse_json.h"

node_t *pib_profiles = NULL;
node_t *pib_policies = NULL;

/* TODO move subset & merge to pmhelper.c */

/* check if a is a subset of b */
int
subset(json_t *prop_a, json_t *prop_b)
{
    const char *key_a;
    json_t *value_prop_a;
    json_t *value_prop_b;
    json_t *value_a;
    json_t *value_b;

    printf("\nSUBSET check between\n%s\nand\n%s\n\n", json_dumps(prop_a,0), json_dumps(prop_b,0));

    json_object_foreach(prop_a, key_a, value_prop_a) {
        printf("checking if %s is a member of candidate... ", key_a);

        value_prop_b = json_object_get(prop_b, key_a);

        if (value_prop_b == NULL) {
            printf("it's not, this is therefore not a subset (FAIL)\n");
            return 0;
        }
        value_b = json_object_get(value_prop_b, "value");
        value_a = json_object_get(value_prop_a, "value");

        if (json_equal(value_a, value_b)) {
            printf("it is (same key and value)\n");
        }
        else {
            printf("it's not (same key, but different value)\n");
            return 0;
        }
    }
    printf("all properties are members. this is a subset (OK).\n");
    return 1;
}

/* merge properties in prop_a into prop_b */
void
merge_properties(json_t *prop_a, json_t *prop_b, int should_overwrite)
{
    const char *key;
    json_t *value;

    json_object_foreach(prop_a, key, value) {
        json_t *found_prop = json_object_get(prop_b, key);

        if (found_prop) {
            printf("found prop %s", key);
            if (!should_overwrite) {
                printf(", but it's write protected\n");
                continue;
            }
            printf(", setting { %s : %s } to %s\n", key, json_dumps(found_prop, 0), json_dumps(value, 0));
            json_object_set(prop_b, key, value);
        }
        else {
            printf("didn't find property %s adding it as a new value\n", key);
            json_object_set(prop_b, key, value);
        }
    }
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
                printf("subset found for %s\n", current_policy->filename);
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
