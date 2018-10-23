#include "../../../Unity/src/unity.h"
#include "test_constants.h"
#include "../node.h"


void test_node_init(void)
{
    node_t *node = node_init(TEST_FILE_PATH);
    TEST_ASSERT_NOT_NULL(node);
    free_node(node);
}

void test_update_node_content(void)
{
    json_t *json  = load_json_file(TEST_FILE_PATH);
    node_t *node = node_init(TEST_FILE_PATH);
    if(json != NULL) {
        update_node_content(node, json);
        TEST_ASSERT_EQUAL(json, node->json);
        TEST_ASSERT_GREATER_THAN_INT(0, node->last_updated);
    }
    free_node(node);
}

void test_create_node(void)
{
    node_t *node = create_node(TEST_FILE_PATH);
    if(node != NULL) {
        TEST_ASSERT_EQUAL_STRING(TEST_FILE_PATH, node->filename);
        TEST_ASSERT_GREATER_THAN_INT(0, node->last_updated);
        TEST_ASSERT_NOT_NULL(node->json)
    }
    else {
        TEST_FAIL();
    }
    free_node(node);
}

void test_add_node(void)
{
    node_t *head = NULL;
    node_t *node = node_init("file");

    head = add_node(head, node);
    TEST_ASSERT_EQUAL_STRING(node,head);

    free_node(head);
}


void test_add_node_2(void)
{
    node_t *head = NULL;

    node_t *node1 = node_init("file1");
    node_t *node2 = node_init("file2");
    node_t *node3 = node_init("file3");
    node_t *node4 = node_init("file4");

    head = add_node(head, node1);
    head = add_node(head, node2);
    head = add_node(head, node3);
    head = add_node(head, node4);

    TEST_ASSERT_EQUAL(node1,head);
    TEST_ASSERT_EQUAL(node2,head->next);
    TEST_ASSERT_EQUAL(node3,head->next->next);
    TEST_ASSERT_EQUAL(node4,head->next->next->next);
    TEST_ASSERT_NULL(head->next->next->next->next);

    TEST_ASSERT_EQUAL_STRING(node1->filename,head->filename);
    TEST_ASSERT_EQUAL_STRING(node2->filename,head->next->filename);
    TEST_ASSERT_EQUAL_STRING(node3->filename,head->next->next->filename);
    TEST_ASSERT_EQUAL_STRING(node4->filename,head->next->next->next->filename);

    free_nodes(head);
}

void test_has_node(void) {
    node_t *head = NULL;
    node_t *node = node_init("file");
    head = add_node(head, node);

    TEST_ASSERT_EQUAL(true, has_node(head,node->filename));
    free_node(head);
}

void test_has_node_2(void) {
    node_t *head = NULL;

    node_t *node1 = node_init("file1");
    node_t *node2 = node_init("file2");
    node_t *node3 = node_init("file3");
    node_t *node4 = node_init("file4");
    node_t *node_no = node_init("file_no");

    head = add_node(head, node1);
    head = add_node(head, node2);
    head = add_node(head, node3);
    head = add_node(head, node4);

    TEST_ASSERT_EQUAL(true, has_node(head,node1->filename));
    TEST_ASSERT_EQUAL(true, has_node(head,node2->filename));
    TEST_ASSERT_EQUAL(true, has_node(head,node3->filename));
    TEST_ASSERT_EQUAL(true, has_node(head,node4->filename));
    TEST_ASSERT_EQUAL(false, has_node(head,node_no->filename));

    free_nodes(head);
    free_node(node_no);
}

void test_get_node(void)
{
    node_t *head = NULL;
    node_t *node = node_init("file");
    head = add_node(head, node);
    TEST_ASSERT_EQUAL(node, get_node(head,node->filename));
    free_node(head);
}

void test_get_node_2(void)
{
    node_t *head = NULL;

    node_t *node1 = node_init("file1");
    node_t *node2 = node_init("file2");
    node_t *node3 = node_init("file3");
    node_t *node4 = node_init("file4");
    node_t *node_no = node_init("file_no");

    head = add_node(head, node1);
    head = add_node(head, node2);
    head = add_node(head, node3);
    head = add_node(head, node4);

    TEST_ASSERT_EQUAL(node1, get_node(head,node1->filename));
    TEST_ASSERT_EQUAL(node2, get_node(head,node2->filename));
    TEST_ASSERT_EQUAL(node3, get_node(head,node3->filename));
    TEST_ASSERT_EQUAL(node4, get_node(head,node4->filename));
    TEST_ASSERT_NULL(get_node(head,node_no->filename));

    free_nodes(head);
    free_node(node_no);
}

void test_remove_node(void)
{
    node_t *head = NULL;
    node_t *node = node_init("file");

    head = add_node(head, node);
    remove_node(&head, node->filename);

    TEST_ASSERT_NULL(head);
}

void test_remove_node_2(void)
{
    node_t *head = NULL;

    node_t *node1 = node_init("file1");
    node_t *node2 = node_init("file2");
    node_t *node3 = node_init("file3");
    node_t *node4 = node_init("file4");
    node_t *node5 = node_init("file5");
    node_t *node_no = node_init("file_no");

    head = add_node(head, node1);
    head = add_node(head, node2);
    head = add_node(head, node3);
    head = add_node(head, node4);
    head = add_node(head, node5);

    remove_node(&head,node1->filename);
    remove_node(&head,node5->filename);
    remove_node(&head,node_no->filename);
    remove_node(&head,node3->filename);
    remove_node(&head,node2->filename);
    remove_node(&head,node_no->filename);
    remove_node(&head,node4->filename);

    TEST_ASSERT_NULL(head);
    free_node(node_no);
}

void
test_get_node_properties(void)
{
    node_t *node = create_node(TEST_FILE_PATH);
    json_t* prop = json_object_get(node->json, "properties");

    node_t *node_null = NULL;

    TEST_ASSERT_EQUAL(prop, get_node_properties(node));
    TEST_ASSERT_NULL(get_node_properties(node_null));

    free_node(node);
}

void
test_node_has_property(void)
{
    node_t *node = create_node(TEST_FILE_PATH);
    json_t *RTT = node_has_property(node, "RTT");
    json_t *low_latency_interface = node_has_property(node, "low_latency_interface");
    json_t *is_wired_interface = node_has_property(node, "is_wired_interface");

    TEST_ASSERT_EQUAL(RTT, node_has_property(node, "RTT"));
    TEST_ASSERT_EQUAL(low_latency_interface, node_has_property(node, "low_latency_interface"));
    TEST_ASSERT_EQUAL(is_wired_interface, node_has_property(node, "is_wired_interface"));
    TEST_ASSERT_NULL(node_has_property(node, "does not exist"));

    free_node(node);
}

void
test_node_set_property(void)
{
    node_t *node = create_node(TEST_FILE_PATH);
    json_t *RTT = node_has_property(node, "RTT");
    json_t* new_value = json_integer(2);

    json_object_set(RTT, "precedence", new_value);
    
    TEST_ASSERT_EQUAL_INT(2, (int)json_number_value(json_object_get(json_object_get(get_node_properties(node), "RTT"), "precedence")));
    free_node(node); free(new_value); 
}

void 
test_node_set_property_2(void) 
{
    node_t *node = create_node(TEST_FILE_PATH);

    json_t* value_v = json_boolean(false);
    json_t* precedence_v = json_integer(2);
    json_t* o;

    o = json_object();
    json_object_set(o, "value", value_v);
    json_object_set(o, "precedence", precedence_v);
    json_object_set(get_node_properties(node), "transport", o);
   
    TEST_ASSERT_NOT_NULL(json_object_get(get_node_properties(node), "transport")); 
    
    TEST_ASSERT_NOT_NULL(json_object_get(json_object_get(get_node_properties(node), "transport"), "value"));
    TEST_ASSERT_EQUAL(false, json_boolean_value(json_object_get(json_object_get(get_node_properties(node), "transport"), "value")));        
    
    TEST_ASSERT_NOT_NULL(json_object_get(json_object_get(get_node_properties(node), "transport"), "precedence"));
    TEST_ASSERT_EQUAL_INT(2, (int)json_number_value(json_object_get(json_object_get(get_node_properties(node), "transport"), "precedence")));      
    
    json_decref(value_v); json_decref(precedence_v); json_decref(o); free_node(node);
}