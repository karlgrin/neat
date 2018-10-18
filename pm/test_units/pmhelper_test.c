#include "../../../Unity/src/unity.h"
#include "test_constant.h"
#include "../pmhelper.h"

void test_concat(void)
{
    char *s1 = "Hello ";
    char *s2 = "World";

    char *r = concat(s1,s2);
    TEST_ASSERT_EQUAL_STRING("Hello World", r);
    free(r);
}

void test_concat_3(void)
{
    char *s1 = "Hello ";
    char *s2 = "big ";
    char *s3 = "World";

    char *r = concat_3(s1, s2, s3);
    TEST_ASSERT_EQUAL_STRING("Hello big World", r);
    free(r);
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
