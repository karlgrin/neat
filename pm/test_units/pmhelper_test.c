#include "../../../Unity/src/unity.h"
#include "test_constants.h"
#include "../pmhelper.h"

void test_new_string(void)
{
    char *s1 = "Hello";
    char *s2 = "big";
    char *s3 = "World";
    char *s_null = NULL;

    char *r1 = new_string("%s %s %s", s1, s2, s3);
    char *r2 = new_string("test %s %s test %s test", s1, s2, s3);
    char *r3 = new_string(s_null);
    char *r4 = new_string("%s %s %s", s1, s2, s_null);
    char *r5 = new_string("%s %s %s", s_null, s2, s3);
    char *r6 = new_string("%s", s1);
    char *r7 = new_string("");



    TEST_ASSERT_EQUAL_STRING("Hello big World", r1);
    TEST_ASSERT_EQUAL_STRING("test Hello big test World test", r2);
    TEST_ASSERT_EQUAL_STRING("", r3);
    TEST_ASSERT_EQUAL_STRING("Hello big (null)", r4);
    TEST_ASSERT_EQUAL_STRING("(null) big World", r5);
    TEST_ASSERT_EQUAL_STRING(s1, r6);
    TEST_ASSERT_EQUAL_STRING("", r7);

    free(r1); free(r2); free(r3); free(r4); free(r5); free(r6); free(r7);
}


void test_file_exist(void)
{
    if(file_exist(__FILE__) == 0)
        TEST_FAIL();

    if(file_exist("Does not exist") != 0)
        TEST_FAIL();
}

void test_load_json_file(void)
{
    json_t *json1 = load_json_file(TEST_FILE_PATH);

    if(json1 == NULL)
        TEST_FAIL();

    char *s = NULL;
    json_t *json2 = load_json_file(s);

    if(json2 != NULL)
        TEST_FAIL();

    json_decref(json1); json_decref(json2);
}

void test_write_json_file(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH);

    if(json == NULL)
        TEST_FAIL();

    int removed = remove(TEST_FILE_PATH);

    if(removed == 0 && file_exist(TEST_FILE_PATH) != 0)
        TEST_FAIL();

    write_json_file(TEST_FILE_PATH, json);

    if(file_exist(TEST_FILE_PATH) != 1)
        TEST_FAIL();

    json_decref(json);
}

void test_file_is_modified(void)
{
    if(file_is_modified(TEST_FILE_PATH, time(0)) != 0)
        TEST_FAIL();

    if(file_is_modified(TEST_FILE_PATH, 0) == 0)
        TEST_FAIL();
}

void test_file_edit_time(void)
{
    if(file_edit_time(TEST_FILE_PATH) == 0)
        TEST_FAIL();
}

void test_array_contains_value(void)
{
    json_t *array = json_loads("[ { \"a\" : true, \"b\" : { \"ba\" : true, \"bb\" : false } }, { \"c\" : { \"ca\" : true, \"cb\" : true }, \"d\" : false } ]", 0, NULL);

    json_t *testval1 = json_loads("{ \"foo\" : \"bar\" }", 0, NULL);
    json_t *testval2 = json_loads("{ \"c\" : { \"ca\" : true, \"cb\" : true }, \"d\" : false }", 0, NULL);
    json_t *testval3 = json_loads("{ \"x\" : { \"ca\" : true, \"cb\" : true }, \"d\" : false }", 9, NULL);

    if (array_contains_value(array, testval1))
        TEST_FAIL();
    if (!array_contains_value(array, testval2))
        TEST_FAIL();
    if (array_contains_value(array, testval3))
        TEST_FAIL();
}
