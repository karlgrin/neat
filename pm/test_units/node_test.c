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
        TEST_ASSERT_EQUAL_INT(2, node->priority);
        TEST_ASSERT_EQUAL_STRING("low_latency", node->uid);
        TEST_ASSERT_EQUAL(true,node->replace_matched);
        TEST_ASSERT_NOT_NULL(node->match);
        TEST_ASSERT_NOT_NULL(node->properties);
    }
    free(json);free_node(node);
}

void test_create_node(void) 
{
    node_t *node = create_node(TEST_FILE_PATH);
    if(node != NULL) {
        TEST_ASSERT_EQUAL_STRING("low_latency", node->uid);
        TEST_ASSERT_EQUAL_INT(2, node->priority);     
        TEST_ASSERT_EQUAL(true, node->replace_matched);
        
        if(node->match != NULL) {
            TEST_ASSERT_EQUAL_STRING("low_latency", node->match->key);
            TEST_ASSERT_EQUAL(true, node->match->value->boolean);
        
            if(node->properties != NULL) {
                TEST_ASSERT_EQUAL_STRING("RTT", node->properties->property->key);
                TEST_ASSERT_EQUAL_INT(1, node->properties->property->precedence);
                TEST_ASSERT_EQUAL_INT(0, node->properties->property->value->range.low_thresh);
                TEST_ASSERT_EQUAL_INT(50, node->properties->property->value->range.high_thresh);
                TEST_ASSERT_EQUAL_INT(RANGE, node->properties->property->type);
                TEST_ASSERT_EQUAL_INT(5, node->properties->property->score);

                if(node->properties->property->next != NULL) {
                    TEST_ASSERT_EQUAL_STRING("low_latency_interface", node->properties->property->next->key);
                    TEST_ASSERT_EQUAL_INT(1, node->properties->property->next->precedence);
                    TEST_ASSERT_EQUAL(true, node->properties->property->next->value->boolean);
                    TEST_ASSERT_EQUAL_INT(BOOLEAN, node->properties->property->next->type);

                    if(node->properties->property->next != NULL) {
                        TEST_ASSERT_EQUAL_STRING("is_wired_interface", node->properties->property->next->next->key);
                        TEST_ASSERT_EQUAL_INT(1, node->properties->property->next->next->precedence);
                        TEST_ASSERT_EQUAL(true, node->properties->property->next->next->value->boolean);
                        TEST_ASSERT_EQUAL_INT(BOOLEAN, node->properties->property->next->next->type);

                        if(node->properties->property->next->next->next == NULL) {
                                free_node(node);
                                return;
                        }
                    }
                }
            }
        }
    }
    free_node(node);
    TEST_FAIL();
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
