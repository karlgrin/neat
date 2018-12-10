#ifndef HEADER_PARSE_JSON
#define HEADER_PARSE_JSON

#define PRECEDENCE_OPTIONAL 1

#include <jansson.h>
#include "pm_helper.h"

json_t* expand_json(json_t* in_properties);
json_t* expand_json_arrays(json_t* in_properties);

json_t* sort_json_array(json_t* array);
json_t* limit_json_array(json_t *array, const unsigned limit);

json_t* process_special_properties(json_t* req);
json_t* parse_local_endpoint(json_t* local_endpoint, json_t* element);

json_t* create_json_array(json_t* json);
void append_json_arrays(json_t* root_array, json_t* array);

bool pre_resolve(const json_t *requests);
void add_default_values(json_t *request);

json_t* expand_property(json_t* element, json_t* property_input, const char* key);
json_t* expand_element_property(json_t* element);
json_t* expand_properties(json_t* req);

json_t* expand_value(json_t* element, json_t* property, json_t* value_input, const char* key);
json_t* expand_element_value(json_t* element);
json_t* expand_values(json_t* req);

int subset(json_t *prop_a, json_t *prop_b);
void merge_properties(json_t *prop_a, json_t *prop_b, int should_overwrite);

#endif
