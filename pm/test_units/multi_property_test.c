#include "../../../Unity/src/unity.h"
#include "test_constant.h"
#include "../multi_property.h"


void test_multi_property_init(void) 
{
    multi_property_t *mp = multi_property_init();
    TEST_ASSERT_NOT_NULL(mp);
}

void test_add_multi_property(void)
{
    multi_property_t *head = NULL;

    property_t *p = property_init();
    p->key = "test";

    multi_property_t *mp = multi_property_init();
    mp->property = p;

    head = add_multi_property(head,mp);

    TEST_ASSERT_EQUAL(mp, head);

    free_multi_property(mp); 
}

void test_add_multi_property_2(void)
{

    multi_property_t *head = NULL;


    property_t *p1 = property_init();
    p1->key = "test";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    multi_property_t *mp1 = multi_property_init();
    mp1->property = p1;

    multi_property_t *mp2 = multi_property_init();
    mp2->property = p2;

    multi_property_t *mp3 = multi_property_init();
    mp3->property = p3;

    head = add_multi_property(head,mp1);
    head = add_multi_property(head,mp2);
    head = add_multi_property(head,mp3);

    TEST_ASSERT_EQUAL(mp3, head);
    TEST_ASSERT_EQUAL(mp2, head->next);
    TEST_ASSERT_EQUAL(mp1, head->next->next);

    free_multi_properties(head);
}

void test_has_a_property(void)
{
    multi_property_t *head = NULL;

    property_t *p = property_init();
    p->key = "test";

    multi_property_t *mp = multi_property_init();
    mp->property = p;

    head = add_multi_property(head,mp);

    if(!has_a_property(head, p->key))
        TEST_FAIL();

    if(has_a_property(head, "does not exist"))
        TEST_FAIL();

    free_multi_property(mp);
}

void test_has_a_property_2(void)
{
    multi_property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p12 = property_init();
    p12->key = "test12";

    property_t *p13 = property_init();
    p13->key = "test13";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    p1 = add_property(p1, p12);
    p1 = add_property(p1, p13);

    multi_property_t *mp1 = multi_property_init();
    mp1->property = p1;

    multi_property_t *mp2 = multi_property_init();
    mp2->property = p2;

    multi_property_t *mp3= multi_property_init();
    mp3->property = p3;

    head = add_multi_property(head,mp1);
    head = add_multi_property(head,mp2);
    head = add_multi_property(head,mp3);

    if(!has_a_property(head, p1->key))
        TEST_FAIL();
    if(!has_a_property(head, p12->key))
        TEST_FAIL();
    if(!has_a_property(head, p13->key))
        TEST_FAIL();
    if(!has_a_property(head, p2->key))
        TEST_FAIL();
    if(!has_a_property(head, p3->key))
        TEST_FAIL();
    if(has_a_property(head, "does not eixst"))
        TEST_FAIL();

    free_multi_properties(head);
}


void test_get_a_property(void)
{
    property_t *p = property_init();
    p->key = "test";

    multi_property_t *mp = multi_property_init();
    mp->property = p;

    TEST_ASSERT_EQUAL(p, get_a_property(mp, p->key));

    free_multi_property(mp);
}


void test_get_a_property_2(void)
{
    multi_property_t *head = NULL;

    property_t *p1 = property_init();
    p1->key = "test1";

    property_t *p12 = property_init();
    p12->key = "test12";

    property_t *p13 = property_init();
    p13->key = "test13";

    property_t *p2 = property_init();
    p2->key = "test2";

    property_t *p3 = property_init();
    p3->key = "test3";

    p1 = add_property(p1, p12);
    p1 = add_property(p1, p13);

    multi_property_t *mp1 = multi_property_init();
    mp1->property = p1;

    multi_property_t *mp2 = multi_property_init();
    mp2->property = p2;

    multi_property_t *mp3= multi_property_init();
    mp3->property = p3;

    head = add_multi_property(head,mp1);
    head = add_multi_property(head,mp2);
    head = add_multi_property(head,mp3);

    TEST_ASSERT_EQUAL(p1, get_a_property(head, p1->key));
    TEST_ASSERT_EQUAL(p12, get_a_property(head, p12->key));
    TEST_ASSERT_EQUAL(p13, get_a_property(head, p13->key));
    TEST_ASSERT_EQUAL(p2, get_a_property(head, p2->key));
    TEST_ASSERT_EQUAL(p3, get_a_property(head, p3->key));
    TEST_ASSERT_EQUAL(NULL, get_a_property(head, "does not exist"));    

    free_multi_properties(head);
}