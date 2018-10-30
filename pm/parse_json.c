#include <jansson.h>

#include "parse_json.h"
#include "pmhelper.h"

void
append_value(json_t *json, json_t *new_value)
{
    if(json_is_array(new_value)) {
        size_t index;
        json_t *value;
        json_array_foreach(new_value, index, value) {
            json_array_append(json, value);
        }
    }
    else if(json_is_object(new_value)) {
        json_object_update(json, new_value);
    }
    else {
        write_log(__FILE__, __func__, "Failed to parse: \n%s", json_dumps(new_value, JSON_INDENT(3)));
    }
}


//json array in och returnerar en json array
json_t*
expand_json_arrays(json_t* in_properties)
{
    json_t *result, *temp1, *temp2;
    result = json_array();
    temp1 = json_array();

    if(in_properties == NULL || json_is_null(in_properties)) { return result; }

    json_array_append_new(result, temp1);
    size_t index1, index2, index3;
    json_t *value1, *value2, *value3;

    json_array_foreach(in_properties, index1, value1) {
        temp1 = json_array();
        json_array_foreach(value1, index2, value2) {
            json_array_foreach(result, index3, value3) {
                temp2 = json_object();
                append_value(temp2, value3);
                append_value(temp2, value2);
                json_array_append(temp1, temp2);
            }
        }
        result = temp1;
    }
    return result;
}

//any json in och returnerar en json array (2D eller 1D)
json_t*
expand_json(json_t* in_properties)
{
    json_t *result;
    result = json_array();

    if(in_properties == NULL || json_is_null(in_properties)) {
        return result;
    }
    else if(json_is_object(in_properties)) {
        json_array_append(result, in_properties);
        return result;
    }
    else if(json_is_array(in_properties)) {
        if( json_array_size(in_properties) > 0 && json_is_array(json_array_get(in_properties, 0))) {
            json_decref(result);
            return expand_json_arrays(in_properties);
        }
        else {
            json_decref(result);
            return in_properties;
        }
    }
    write_log(__FILE__, __func__, "Error: unknown json structure");
    return result;
}

json_t *
sort_json_array(json_t* array){

    return array;
}
