#ifndef HEADER_PROPERTY
#define HEADER_PROPERTY

#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <jansson.h>

#include "pmhelper.h"

typedef enum type 
{
     INTEGER, DOUBLE, BOOLEAN, STRING, RANGE, NULL_VALUE,
}type_t;

typedef struct range
{
    unsigned int low_thresh;
    unsigned int high_thresh;
}range_t;

typedef union value 
{ 
    int integer; double double_t; bool boolean; const char *string; range_t range;
}value_t;

typedef struct property
{
    const char *key;
    int precedence;
    int score;
    value_t *value;
    type_t type;
    struct property *next;
} property_t;

property_t* property_init();      
void free_property(property_t * p);
void free_properties(property_t  *head);

type_t json_to_type_t(json_t *json);
value_t* json_to_value_t(json_t *json);
property_t* json_to_property_t(json_t * json);

bool has_property(property_t *head, const char *key);
property_t* get_property(property_t *head, const char *key);
property_t* add_property(property_t *head, property_t *p);
bool remove_property(property_t **head, const char *key);
void overwrite_property(property_t **head, const char *key, property_t *p);
void print_property(property_t *head);

#endif