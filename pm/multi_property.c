#include <stdio.h>
#include <jansson.h>
#include <stdbool.h>

#include "property.h"
#include "pmhelper.h"

typedef struct multi_property
{
    property_t *property;
    struct multi_property *next;
} multi_property_t;

multi_property_t*
multi_property_init() {
    multi_property_t* mp = calloc(1, sizeof(multi_property_t));

    if(mp == NULL) { 
        write_log(__FILE__, __func__, "Failed to malloc..."); 
        return NULL;
    }
    mp->property = NULL;
    mp->next = NULL;
    return mp;
}

void
free_multi_property(multi_property_t *mp) 
{
    if(mp != NULL) {
        free_properties(mp->property);
    }
    free(mp);
}

void
free_multi_properties(multi_property_t *head) 
{
    multi_property_t  *tmp;

    while (head != NULL) {
       tmp = head;
       head = head->next;
       free_multi_property(tmp);
    } 
}

multi_property_t*
json_array_to_multi_property(multi_property_t* current,json_t *json)
{
    if(json == NULL || current == NULL) { return current; }

    size_t n = json_array_size(json);
    size_t index;
    json_t *value;
    
    json_array_foreach(json, index, value) {
        if(json_is_array(value)) {
            current = json_array_to_multi_property(current, value);      
        }
        else {
            current->property = json_to_property(value);
        }
        if(index < n - 1) {
            current->next =  multi_property_init();
            current = current->next;
        }      
    }
    current->next = NULL;

    return current;    
}

multi_property_t*
json_to_multi_property(json_t *json) 
{
    if(json == NULL) { return NULL; }

    multi_property_t *properties = NULL;
   
    if(json_is_object(json)) {
        properties = multi_property_init();
        properties->property = json_to_property(json);
        properties->next = NULL;
    }
    else if(json_is_array(json) && json_array_size(json) > 0) {
        properties = multi_property_init();
        json_array_to_multi_property(properties, json);
    }

    return properties;
}

bool
has_a_property(multi_property_t *head, const char *key)
{
    multi_property_t *current = head;
    while(current != NULL) {
        if(has_property(current->property, key)) {
            return true;
        }
        current= current->next;
    }    
    return false;
}

property_t*
get_a_property(multi_property_t *head, const char *key)
{
    multi_property_t *current = head;
    while(current != NULL) {
        property_t *p = get_property(current->property, key);
        if(p != NULL) {
            return p;
        }
        current = current->next;
    }    
    return NULL;
}

multi_property_t*
add_multi_property(multi_property_t *head, multi_property_t *p)
{
    if(p == NULL) { return head; }

    multi_property_t *new_head = p;
    new_head->next = head; 
    return new_head;
}

//testing
void 
print_multi_property(multi_property_t *head) 
{
    multi_property_t *current = head;
    while(current != NULL) {
        printf("---properties---\n");
        print_property(current->property);              
        current = current->next;
    }
}
