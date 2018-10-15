#ifndef HEADER_MULTI_PROPERTY
#define HEADER_MULTI_PROPERTY

#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>

#include "pmhelper.h"
#include "property.h"

typedef struct multi_property
{
    property_t *property;
    struct multi_property *next;
} multi_property_t;

multi_property_t* multi_property_init();      
void free_multi_property(multi_property_t *mp); 
void free_multi_properties(multi_property_t *head);

multi_property_t* json_array_to_multi_property(multi_property_t* current,json_t *json);
multi_property_t* json_to_multi_property(json_t *json);

bool has_a_property(multi_property_t *head, const char *key);
property_t* get_a_property(multi_property_t *head, const char *key);
multi_property_t* add_multi_property(multi_property_t *head, multi_property_t *p);
void print_multi_property(multi_property_t *head);

#endif