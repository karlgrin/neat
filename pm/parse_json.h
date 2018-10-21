#ifndef HEADER_PARSE_JSON
#define HEADER_PARSE_JSON

#include <jansson.h>

json_t* expand_json(json_t* in_properties);
json_t* expand_json_arrays(json_t* in_properties);

#endif