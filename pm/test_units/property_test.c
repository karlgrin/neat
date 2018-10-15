#include "../../../Unity/src/unity.h"
#include "../property.h"

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

    TEST_ASSERT_EQUAL(p3, head);
    TEST_ASSERT_EQUAL(p2, head->next);
    TEST_ASSERT_EQUAL(p1, head->next->next);
    
    free_property(p1); free_property(p2); free_property(p3); 
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

    free_property(p1); free_property(p2); free_property(p3);
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

     free_property(p1); free_property(p2); free_property(p3); free_property(p4);   
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
    p2->key = "test3";

    head = add_property(head,p1);
    head = add_property(head,p2);
    head = add_property(head,p3);

    property_t *p4 = property_init();
    p3->key = "test4";

    property_t *p5 = property_init();
    p5->key = "test5";

    property_t *p6 = property_init();
    p6->key = "test6";


    overwrite_property(&head, p1->key, p4);
    overwrite_property(&head, p2->key, p5);
    overwrite_property(&head, p3->key, p6);

    TEST_ASSERT_EQUAL(p6, head);
    TEST_ASSERT_EQUAL(p5, head->next);
    TEST_ASSERT_EQUAL(p4, head->next->next);

    free_property(p4); free_property(p4); free_property(p6);   
}
