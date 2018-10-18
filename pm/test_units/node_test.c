#include "../../../Unity/src/unity.h"
#include "test_constant.h"
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
test_node_update_property (void)
{
    node_t *node = NULL;

    /* read in node properties */
    node = update_ib_node(node, "../json_examples/pib/profile//low_latency.profile");

    TEST_ASSERT_NOT_NULL(node);
    TEST_ASSERT_NOT_NULL(node_has_property(node, "low_latency_interface"));

    /* test updating low_latency_interface property value */
    json_t *new_value = json_loads("{ \"value\" : false, \"precedence\": 7 }", 0, NULL);
    node_set_property(node, "low_latency_interface", new_value);

    json_t *prop = node_has_property(node, "low_latency_interface");

    TEST_ASSERT_NOT_NULL(prop);

    TEST_ASSERT_TRUE(json_is_false(json_object_get(prop, "value")));

    json_t *json_precedence_value = json_object_get(prop, "precedence");

    TEST_ASSERT_NOT_NULL(json_precedence_value);

    json_int_t precedence_int_value = json_integer_value(json_precedence_value);

    TEST_ASSERT_EQUAL(precedence_int_value, 7);

}
