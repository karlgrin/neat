#include <jansson.h>
#include <string.h>

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

/* return the sum of all property scores */
/* TODO sum evaluated and not yet evaluated into separate sums (see def score in policy.py) */
int
property_score_sum(json_t *candidate)
{
    int sum = 0;
    int score;
    const char *key;
    json_t *property;
    json_t *score_obj;
    json_object_foreach(candidate, key, property) {
        score_obj = json_object_get(property, "score");
        if (score_obj) {
            score = json_integer_value(score_obj);
            sum += score;
        }
    }
    return sum;
}

int
cmp_score(const void *json1, const void *json2)
{
    int score1 = property_score_sum(*((json_t **) json1));
    int score2 = property_score_sum(*((json_t **) json2));

    return score2 - score1;
}

json_t *
sort_json_array(json_t *array)
{
    size_t arr_size = json_array_size(array);
    json_t *to_sort[arr_size];
    json_t *result = json_array();
    json_t *item;
    size_t i;

    json_array_foreach(array, i, item) {
        to_sort[i] = item;
    }

    qsort(to_sort, arr_size, sizeof(json_t *), cmp_score);

    for (i = 0; i < arr_size; i++) {
        json_array_append_new(result, to_sort[i]);
    }

    return result;
}

/* return the first n elements of array defined by limit */
json_t *
limit_json_array(json_t *array, const unsigned int limit)
{
    size_t arr_size = json_array_size(array);
    size_t i;

    if(arr_size == 0) { return array; }
    
    for (i = arr_size - 1; i >= limit; i--) {
        if (json_array_remove(array, i) == -1) {
            fprintf(stderr, "Error: cannot remove array element\n");
        }
    }
    return array;
}

json_t* 
parse_local_endpoint(json_t* local_endpoint, json_t* element)
{   
    char *le_value = strdup(json_string_value(json_object_get(local_endpoint, "value")));

    if(strchr(le_value, '@')) {
        char * ip_value = strtok(le_value, "@");
        char * interface_value = strtok(NULL, "@");
            
        json_t *local_ip = json_deep_copy(local_endpoint);
        json_object_set(local_ip, "value", json_string(ip_value));
        
        json_t *interface = json_deep_copy(local_endpoint);
        json_object_set(interface, "value", json_string(interface_value));

        json_object_set(element, "interface", interface);
        json_object_set(element, "local_ip", local_ip);
        json_object_del(element, "local_endpoint");
   }    
   return element;
}

json_t* 
parse_local_endpoint_array(json_t* local_endpoint, json_t* element)
{   
    size_t index, index2;
    json_t *value, *value2;
    json_t * my_return = json_array();

    json_t * le = json_deep_copy(local_endpoint);
    json_object_del(element, "local_endpoint");

    //create a new element for every interface/local_ip
    json_array_foreach(le, index, value) {
        json_object_set(element, "local_endpoint", value);
        json_t * temp = json_deep_copy(element);     
        json_array_append(my_return, temp);              
    } 

    //parse the requests, local_endpoint is no longer a arrays
    json_array_foreach(my_return, index2, value2) {
        json_t *le_temp = json_object_get(value2, "local_endpoint");
        if(le_temp) {
            value2 = parse_local_endpoint(le_temp, value2);
        }
    }

    return my_return;
}

json_t* 
create_json_array(json_t* json) {   
    if(json_is_array(json)) { return json; }
    
    json_t *root;
    root = json_array();
    json_array_append(root, json);
    return root;
}

void
append_json_arrays(json_t* root_array, json_t* array)
{
    size_t index;
    json_t *value;

    json_t * new_array = create_json_array(array);

    json_array_foreach(new_array, index, value) { 
        json_array_append(root_array, value);
    }
}

json_t* 
process_special_properties(json_t* req) 
{
    size_t index;
    json_t *value;
    json_t *root = create_json_array(req);
    json_t *my_return = json_array();

    json_array_foreach(root, index, value) {
        json_t * local_endpoint = json_object_get(value, "local_endpoint");
        if(local_endpoint) {
            if(json_is_array(local_endpoint)) {
                append_json_arrays(my_return, parse_local_endpoint_array(local_endpoint, value));  
            }             
            else {
                append_json_arrays(my_return, parse_local_endpoint(local_endpoint, value)); 
            }       
        }
        else {
            json_array_append(my_return, value);
        }
    }    
    return my_return;
}
