#ifndef HEADER_PARSE_JSON
#define HEADER_PARSE_JSON

#include <jansson.h>

json_t* expand_json(json_t* in_properties);
json_t* expand_json_arrays(json_t* in_properties);

json_t* sort_json_array(json_t* array);
json_t* limit_json_array(json_t *array, const unsigned limit);

json_t* process_special_properties(json_t* req);
json_t* parse_local_endpoint(json_t* local_endpoint, json_t* element);

json_t* create_json_array(json_t* json);
void append_json_arrays(json_t* root_array, json_t* array);

json_t* expand_property(json_t* element, json_t* property_input, const char* key);
json_t* expand_element(json_t* element);
json_t* expand_properties(json_t* req);

#endif
