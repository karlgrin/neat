#include <jansson.h>
#include <string.h>

#include "parse_json.h"
#include "pm_helper.h"

#define PRECEDENCE_BASE 0

#define MAX(a,b) (((a)>(b))?(a):(b))
#define ARRAY_SIZE(array) (sizeof((array))/sizeof((array)[0]))

bool
pre_resolve(const json_t *requests)
{
    bool pre_resolve = false;
    json_t *request;
    size_t i;

    json_array_foreach(requests, i, request) {
        json_t *req_type = json_object_get(request, "__request_type");
        if (req_type) {
            const char *req_type_val = json_string_value(json_object_get(req_type, "value"));
            if (!strcmp(req_type_val, "pre-resolve")) {
                json_object_del(request, "__request_type");
                pre_resolve = true;
            }
        }

    }
    return pre_resolve;
}

void
add_default_values(json_t *request)
{
    json_t *property, *attr;
    const char *key;
    size_t n;
    unsigned int i;

    /* json values for default props */
    char *default_props[] = { "score", "evaluated", "precedence"};
    char *default_types[] = { "i", "b", "i"};
    int  default_values[] = { 0, 0, PRECEDENCE_OPTIONAL };

    json_object_foreach(request, key, property) {
        for (i = 0; i < ARRAY_SIZE(default_props); i++) {

            /* handle array of values */
            if (json_is_array(property)) {
                json_array_foreach(property, n, attr) {
                    json_t *tmp_prop = json_pack("{sO}", key, attr);
                    add_default_values(tmp_prop);
                    json_decref(tmp_prop); // RISKY seems that attr is not dereferenced?
                }
                break;
            }
            /* add default property if not found */
            if (json_object_get(property, default_props[i]) == NULL) {
                json_object_set(property, default_props[i], json_pack(default_types[i], default_values[i]));
            }
        }
    }
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
            write_log(__FILE__, __func__, LOG_ERROR, "Cannot remove array element in parsing..");
        }
    }
    return array;
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
parse_local_endpoint(json_t* local_endpoint, json_t* element)
{
    json_t* value = json_object_get(local_endpoint, "value");

    if(!json_is_null(value)) {
        char *le_value = strdup(json_string_value(value));
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
    }
   return element;
}

json_t*
create_json_array(json_t* json) {
    if(json_is_array(json)) { return json; }

    json_t *root;
    root = json_array();
    json_array_append(root, json);
    return root;
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
        if(local_endpoint && json_is_object(local_endpoint)) {
            append_json_arrays(my_return, parse_local_endpoint(local_endpoint, value));
        }
        else {
            json_array_append(my_return, value);
        }
    }
    return my_return;
}

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
        write_log(__FILE__, __func__, LOG_ERROR, "Failed to parse json.");
        if(debug_enabled) {
            char* json_string = json_dumps(new_value, 0);
            write_log(__FILE__, __func__, LOG_DEBUG, "Failed to parse: \n%s\n", json_string);
            free(json_string);
        }
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
    write_log(__FILE__, __func__, LOG_ERROR, "Unknown json structure");
    return result;
}


json_t*
expand_property(json_t* element, json_t* property_input, const char* key)
{
    size_t index1;
    json_t *value;
    json_t *my_return = json_array();
    json_t * property = json_deep_copy(property_input);

    //create a new element for every element in the property
    json_array_foreach(property, index1, value) {
        json_t* temp = json_deep_copy(element);
        json_object_set(temp, key, value);
        append_json_arrays(my_return, expand_element_property(temp));    //loop call
    }

    return my_return;
}

json_t*
expand_element_property(json_t* element)
{
    const char *key;
    json_t *property;
    json_t *my_return = json_array();

    json_object_foreach(element, key, property) {
        if(json_is_array(property)) {
            append_json_arrays(my_return, expand_property(element, json_deep_copy(property), key));
            break;   //break here due to loop call in expand_property
        }
    }

    if(json_array_size(my_return) == 0) {
        json_array_append(my_return, element);
    }
    return my_return;
}


json_t*
expand_properties(json_t* req)
{
    size_t index;
    json_t *element;
    json_t *root = create_json_array(req);
    json_t *my_return = json_array();

    json_array_foreach(root, index, element) {
        if(json_is_object(element)) {
            append_json_arrays(my_return, expand_element_property(element));
       }
       else {
            write_log(__FILE__, __func__, LOG_ERROR, "Wrong format on json, cant parse it!");
       }
    }
    return my_return;
}

json_t*
expand_value(json_t* element, json_t* property, json_t* value_input, const char* key)
{
    size_t index1;
    json_t* v;
    json_t* my_return = json_array();
    json_t* value = json_deep_copy(value_input);

    //create a new element for every element in the value
    json_array_foreach(value, index1, v) {
        json_t* temp_ele = json_deep_copy(element);
        json_t* temp_prop = json_object_get(temp_ele, key);
        json_object_set(temp_prop, "value", v);
        json_object_set(temp_ele, key, temp_prop);
        append_json_arrays(my_return, temp_ele);    //loop call
    }

    return my_return;
}

json_t*
expand_element_value(json_t* element)
{
    const char *key;
    json_t *property;
    json_t *my_return = json_array();

    json_object_foreach(element, key, property) {
        if(json_is_object(property)) {
            json_t* value = json_object_get(property, "value");
            if(value != 0 && json_is_array(value)) {
                append_json_arrays(my_return, expand_value(element, property, value, key));
                break;   //break here due to loop call in expand_value
            }
        }
    }

    if(json_array_size(my_return) == 0) {
        json_array_append(my_return, element);
    }
    return my_return;
}


json_t*
expand_values(json_t* req)
{
    size_t index;
    json_t *element;
    json_t *root = create_json_array(req);
    json_t *my_return = json_array();

    json_array_foreach(root, index, element) {
        if(json_is_object(element)) {
            append_json_arrays(my_return, expand_element_value(element));
       }
       else {
            write_log(__FILE__, __func__, LOG_ERROR, "PM: Wrong format on json, cant parse it!");
       }
    }
    return my_return;
}

/* check if a is a subset of b */
int
subset(json_t *prop_a, json_t *prop_b)
{
    const char *key_a;
    json_t *value_prop_a;
    json_t *value_prop_b;
    json_t *value_a;
    json_t *value_b;


    json_object_foreach(prop_a, key_a, value_prop_a) {
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
    //printf("all properties are members. this is a subset (OK).\n");
    return 1;
}

/* update prop_a with properties from prop_b */
void
merge_update_properties(json_t *prop_a, json_t *prop_b, bool evaluated)
{
    json_object_set(prop_a, "evaluated", json_pack("b", evaluated));

    json_t *value_a = json_object_get(prop_a, "value");
    json_t *value_b = json_object_get(prop_b, "value");

    if (!value_b) {
        printf("ERRRO: undefined: value_b should not be null\n");
        return;
    }

    /* null = match all */
    if (value_a == NULL || json_equal(value_a, value_b)) {

        json_t *score_value_a_obj = json_object_get(prop_a, "score");
        int score_value_a = score_value_a_obj ? json_integer_value(score_value_a_obj) : 0;

        json_t *score_value_b_obj = json_object_get(prop_b, "score");
        int score_value_b = score_value_b_obj ? json_integer_value(score_value_b_obj) : 0;

        json_object_set(prop_a, "score", json_pack("i", score_value_a + score_value_b));

        json_t *precedence_value_a_obj = json_object_get(prop_a, "precedence");
        int precedence_value_a = precedence_value_a_obj ? json_integer_value(precedence_value_a_obj) : 0;

        json_t *precedence_value_b_obj = json_object_get(prop_b, "precedence");
        int precedence_value_b = precedence_value_b_obj ? json_integer_value(precedence_value_b_obj) : 0;

        json_object_set(prop_a, "precedence", json_integer((int)MAX(precedence_value_a, precedence_value_b)));

        /* value = value_match? */

    }
    else {
        json_t *precedence_value_a_obj = json_object_get(prop_a, "precedence");
        int precedence_value_a = precedence_value_a_obj ? json_integer_value(precedence_value_a_obj) : 0;

        json_t *precedence_value_b_obj = json_object_get(prop_b, "precedence");
        int precedence_value_b = precedence_value_b_obj ? json_integer_value(precedence_value_b_obj) : 0;

        if (precedence_value_b >= precedence_value_a) {
            json_t *score_prop_b = json_object_get(prop_b, "score");
            json_object_set(value_a, "score", score_prop_b ? score_prop_b : json_pack("i", 0));

            json_object_set(prop_a, "value", value_b);

            json_object_set(prop_a, "precedence", precedence_value_b_obj);
        }
    }
}

/* decide if reverse comparision is to be used */
void
merge_add_properties(json_t *prop_a, json_t *prop_b)
{
    json_t *precedence_prop_b_json = json_object_get(prop_b, "precedence");

    if (!precedence_prop_b_json) {
        fprintf(stderr, "Error: no precedence attribute\n");
        return;
    }
    int precedence_prop_b = json_integer_value(precedence_prop_b_json);

    if (precedence_prop_b == PRECEDENCE_BASE) {
        json_t *prop_b_copy = json_deep_copy(prop_b);
        merge_update_properties(prop_b_copy, prop_a, false);
        json_object_update(prop_a, prop_b_copy);
        json_decref(prop_b_copy);
        return;
    }

    merge_update_properties(prop_a, prop_b, true);
}

/* merge properties in prop_a into prop_b */
void
merge_properties(json_t *prop_a, json_t *prop_b, int should_overwrite)
{
    const char *key;
    json_t *value;

    printf("\nMERGING: \n%s%\n\n AND \n\n%s\n\n", json_dumps(prop_a, 0), json_dumps(prop_b, 0));

    json_object_foreach(prop_a, key, value) {

        json_t *found_prop = json_object_get(prop_b, key);

        if (!found_prop || should_overwrite) {
            json_object_set(prop_b, key, value);
        }
        else {
            /* Update the property values */
            merge_add_properties(found_prop, value);
        }
    }
}
