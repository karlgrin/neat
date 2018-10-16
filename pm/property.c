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

property_t*
property_init() {
    property_t *p = (property_t*) calloc(1, sizeof(property_t));

    if(p == NULL) { 
        write_log(__FILE__, __func__, "Failed to malloc..."); 
        return NULL;
    }
    
    p->key = NULL;
    p->precedence = 0;
    p->score = 0;
    p->value = NULL;
    p->type = NULL_VALUE;
    p-> next= NULL;

    return p;
}

void
free_property(property_t *p) 
{
    if(p != NULL) { 
        free(p->value);
        p->value = NULL; 
    }
    free(p);   
}

void 
free_properties(property_t  *head)
{
   property_t  *tmp;

   while (head != NULL) {
       tmp = head;
       head = head->next;
       free_property(tmp);
    }
}

type_t
json_to_type(json_t *json) 
{
    if(json_is_null(json))
        return NULL_VALUE;
    else if(json_is_integer(json)) 
        return INTEGER;
        else if(json_is_real(json)) 
        return DOUBLE;
    else if(json_is_boolean(json))
        return BOOLEAN;
    else if(json_is_string(json))
        return STRING;
    else if(json_is_object(json))
         return RANGE;      //assume range?

    return NULL_VALUE;  //unknown type, error handle?
}

value_t*
json_to_value(json_t *json) 
{
    value_t *my_value = malloc(sizeof(value_t));
    if(json_is_null(json)) {
        my_value = NULL;
    }
    else if(json_is_integer(json)) {
        my_value->integer = json_integer_value(json);
    }
    else if(json_is_real(json)) {
        my_value->double_t = json_real_value(json);
    }
    else if(json_is_boolean(json)) {
        my_value->boolean = json_boolean_value(json);
    }
    else if(json_is_string(json)) {
        my_value->string = json_string_value(json);
    }
    else if(json_is_object(json)) { //assume range
        range_t r;
        r.low_thresh = json_integer_value(json_object_get(json, "start"));
        r.high_thresh = json_integer_value(json_object_get(json, "end"));
        my_value->range = r;
    }
    //no error handling, just return null
    return my_value;
}

property_t*
json_to_property(json_t * json)
{
    if(json == NULL || !json_is_object(json)) { return NULL; }

    property_t *head = property_init();
    property_t *current = head;
    
    void *iter = json_object_iter(json);
    while(iter)
    {
            
        current->key = json_object_iter_key(iter);
        current->precedence = json_integer_value(json_object_get(json_object_iter_value(iter), "precedence"));
        current->score = json_integer_value(json_object_get(json_object_iter_value(iter), "score"));
        current->type = json_to_type(json_object_get(json_object_iter_value(iter), "value"));
        current->value = json_to_value(json_object_get(json_object_iter_value(iter), "value"));            

        if(json_object_iter_next(json,iter) != NULL) {    
            current-> next =  property_init();
            current = current->next;    
        }    
        iter = json_object_iter_next(json, iter);       
    }
    current->next = NULL; //last item in list is null

    return head;
}

bool
has_property(property_t *head, const char *key)
{
    property_t *current = head;
    while(current != NULL) {
        if(strcmp(current->key, key) == 0) {  //key unique?
            return true;
        }
        current = current->next;
    }
    return false;
}

property_t*
get_property(property_t *head, const char *key)
{
    property_t *current = head;
    while(current != NULL) {
        if(strcmp(current->key, key)== 0) {  //key unique?
            return current;
        }
        current = current->next;
    }
    return NULL;
}

property_t*
add_property(property_t *head, property_t *p)
{
    if(p == NULL) { return head; }

    property_t *new_head = p;
    p->next = head;
    return new_head;
}

bool
remove_property(property_t **head, const char *key)
{
    if(head == NULL || key == NULL) { return false; }

    property_t *current = *head;
    property_t *previous = NULL;

    while(current != NULL) {
        if(strcmp(current->key,key) == 0) {  //key unique?
            if(previous == NULL) {
                *head = current->next;               
            } 
            else {
                previous->next = current->next;
            }   
            free_property(current);
            return true;
        }
        previous = current;
        current = current->next;
    }
    return false;
}

void
overwrite_property(property_t **head, const char *key, property_t *p)
{
    if(head == NULL) { return; }
    
    property_t *current = *head;
    property_t *previous = NULL;
    while(current != NULL) {
        if(strcmp(current->key,key)  == 0) {  //key unique?
            if(previous == NULL) { //first element
                p->next = current->next;
                *head = p;
            }
            else {
                previous->next = p;
                p->next = current->next;
            }
            free_property(current);
            return;
        }
        previous = current;
        current = current->next;
    }
}

//testing
void 
print_property(property_t *head) 
{
    property_t *current = head;
    while(current != NULL) {
        printf("\t%s:\n", current->key);
        printf("\t\tprecedence: %d, ", current-> precedence);
        printf("score: %d, ", current-> score);
        printf("type: %d, ", current-> type);

        if(current->type == INTEGER) printf("value: %d", current->value->integer);    
        if(current->type == DOUBLE) printf("value: %f", current->value->double_t);
        if(current->type == STRING) printf("value: %s", current->value->string);
        if(current->type == BOOLEAN) printf("value: %s", current->value->boolean ? "true": "false");
        if(current->type == RANGE) printf("value: range(%d,%d)", current->value->range.low_thresh, current->value->range.high_thresh); 
        if(current->type == NULL_VALUE) printf("value: %s", "null"); 

        printf("\n");
        current = current->next;
    }
}