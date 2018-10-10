
#include<stdio.h>
#include<string.h>
#include<jansson.h>
#include<stdbool.h>

enum type {
    integer, boolean, string, range, list
};

typedef struct range
{
    unsigned int low_thresh;
    unsigned int high_thresh;
} range_t;

typedef struct property_value
{
    union Value { int integer; bool boolean; char *string; } value;
    enum type type;
    struct property_value *next;
} property_value_t;

// TODO hash table of properties
typedef struct property
{
    char *key;
    int precedence; // TODO what is this?
    int score;
    property_value_t *value;
    struct property *next;
} property_t;


property_t *
json_prop_to_propt_t(json_t *json_prop)
{
    return NULL;
}

property_t *
json_to_property_array(json_t *json_file)
{
    // TODO
    property_t *list = malloc(sizeof(property_t));

    // for each in json
    //             add_property(list, convert_prop)
    return list;
}

bool
has_property(property_t *list, property_t check)
{
    return true;
}

property_t *
add_property(property_t *list, property_t to_add)
{
    return NULL;
}

property_t *
overwrite_property_value(property_t *list, property_t check)
{
    return NULL;
}

/* testing */
int
main (int argc, char **agrv) {

    char *test_prop = "{\"transport\" : \"TPC\"}";
    json_t *test_json_prop = NULL; // = load_json(test_prop);

    printf("test properties: %s\n", test_prop);

    if (!test_json_prop) {
        fprintf(stderr, "json load error");
        return 1;
    }

	return 0;
}
