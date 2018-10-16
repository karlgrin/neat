#include "../../../Unity/src/unity.h"
#include "test_constant.h"
#include "../property.h"

void test_property_init(void) 
{
    property_t *p = property_init();
    TEST_ASSERT_NOT_NULL(p);
}

void test_add_property(void)
{
    property_t *head = NULL;

    property_t *p = property_init();
    p->key = "test";

    head = add_property(head,p);

    TEST_ASSERT_EQUAL(p, head);

    free_property(p);
}

void test_add_property_2(void)
{
    property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    head = add_property(head,p1);
    head = add_property(head,p2);
    head = add_property(head,p3);

    TEST_ASSERT_EQUAL(p1, head);
    TEST_ASSERT_EQUAL(p2, head->next);
    TEST_ASSERT_EQUAL(p3, head->next->next);
    
    free_properties(head);
}

void test_get_property(void)
{
    property_t *head = NULL;

    property_t *p = property_init();
    p->key = "test";

    head = add_property(head,p);

    TEST_ASSERT_EQUAL(p, get_property(head, "test"));

    free_property(p);
}

void test_get_property_2(void)
{
    property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    head = add_property(head,p1);
    head = add_property(head,p2);
    head = add_property(head,p3);

    TEST_ASSERT_EQUAL(p1, get_property(head,"test1"));
    TEST_ASSERT_EQUAL(p2, get_property(head,"test2"));
    TEST_ASSERT_EQUAL(p3, get_property(head,"test3"));

    free_properties(head);
}

void test_remove_property(void)
{
    property_t *head = NULL;

    property_t *p = property_init();
    p->key = "test";

    head = add_property(head, p);

    TEST_ASSERT_NOT_EQUAL(NULL,head);

    remove_property(&head, p->key);

    TEST_ASSERT_EQUAL(NULL, head);
}

void test_remove_property_2(void)
{
    property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    property_t *p4 = property_init();
    p4->key = "test4";

    head = add_property(head,p1);
    head = add_property(head,p2);
    head = add_property(head,p3);
    head = add_property(head,p4);

    if(!remove_property(&head, "test1"))
        TEST_FAIL();
    if(!remove_property(&head, "test4"))
        TEST_FAIL();
    if(remove_property(&head, "does not exist"))
        TEST_FAIL();
    if(!remove_property(&head, "test2"))
        TEST_FAIL();
    if(!remove_property(&head, "test3"))
        TEST_FAIL();
    if(remove_property(&head, "head is null"))
        TEST_FAIL();

    TEST_ASSERT_EQUAL(NULL, head);
}

void test_has_property(void)
{
    property_t *head = NULL;

    property_t *p = property_init();
    p->key = "test";


    head = add_property(head,p);

    if(!has_property(head, "test"))
        TEST_FAIL();

    free_property(p);
}

void test_has_property_2(void)
{
    property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    property_t *p4 = property_init();
    p4->key = "test4";

    head = add_property(head,p1);
    head = add_property(head,p2);
    head = add_property(head,p3);
    head = add_property(head,p4);

    if(!has_property(head, "test1"))
        TEST_FAIL();
    if(!has_property(head, "test4"))
        TEST_FAIL();
    if(!has_property(head, "test3"))
        TEST_FAIL();
    if(!has_property(head, "test2"))
        TEST_FAIL();
    if(has_property(head, "does not exist"))
        TEST_FAIL();

     free_properties(head); 
}


void test_overwrite_property(void)
{
    property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p2 = property_init();
    p1->key = "test2";

    head = add_property(head,p1);
    
    overwrite_property(&head, p1->key, p2);
    
    TEST_ASSERT_EQUAL(head, p2);

    free_property(p2);
}

void test_overwrite_property_2(void)
{
    property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    head = add_property(head,p1);
    head = add_property(head,p2);
    head = add_property(head,p3);

    property_t *p4 = property_init();
    p4->key = "test4";

    property_t *p5 = property_init();
    p5->key = "test5";

    property_t *p6 = property_init();
    p6->key = "test6";

    overwrite_property(&head, p1->key, p4);
    overwrite_property(&head, p2->key, p5);
    overwrite_property(&head, p3->key, p6);

    TEST_ASSERT_EQUAL(p4, head);
    TEST_ASSERT_EQUAL(p5, head->next);
    TEST_ASSERT_EQUAL(p6, head->next->next);

    free_properties(head); 
}

void test_json_to_property(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH);

    if(json != NULL) {
        property_t * p = json_to_property(json_object_get(json, "properties"));
        if(p != NULL) {
            TEST_ASSERT_EQUAL_STRING("RTT", p->key);
            TEST_ASSERT_EQUAL_INT(1, p->precedence);
            TEST_ASSERT_EQUAL_INT(5, p->score);
            TEST_ASSERT_EQUAL_UINT(0, p->value->range.low_thresh);
            TEST_ASSERT_EQUAL_UINT(50, p->value->range.high_thresh);
            TEST_ASSERT_EQUAL_STRING("low_latency_interface", p->next->key); 
            TEST_ASSERT_EQUAL(true, p->next->value->boolean);
            TEST_ASSERT_EQUAL_STRING("is_wired_interface", p->next->next->key);
            TEST_ASSERT_EQUAL_INT(1, p->next->next->precedence);
        }
        else {
            TEST_FAIL();
        }
    }
}


void test_json_to_value(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH);

    if(json != NULL) {
        json_t *json1 = json_object_get(json, "priority");
        value_t *t1 = json_to_value(json1);
        TEST_ASSERT_EQUAL_INT(2,t1->integer);

        json_t *json2 = json_object_get(json, "uid");
        value_t *t2 = json_to_value(json2);
        TEST_ASSERT_EQUAL_STRING("low_latency",t2->string);

        json_t *json3 = json_object_get(json, "replace_matched");
        value_t *t3 = json_to_value(json3);
        TEST_ASSERT_EQUAL(true,t3->boolean);
    }
}

void test_json_to_type(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH);

    if(json != NULL) {
        json_t *json1 = json_object_get(json, "priority");
        type_t t1 = json_to_type(json1);
        TEST_ASSERT_EQUAL_INT(INTEGER,t1);

        json_t *json2 = json_object_get(json, "uid");
        type_t t2 = json_to_type(json2);
        TEST_ASSERT_EQUAL_INT(STRING,t2);

        json_t *json3 = json_object_get(json, "replace_matched");
        type_t t3 = json_to_type(json3);
        TEST_ASSERT_EQUAL_INT(BOOLEAN,t3);
    }
}
