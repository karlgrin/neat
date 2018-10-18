#include "../../../Unity/src/unity.h"
#include "test_constant.h"
#include "../pmhelper.h"

void test_concat(void)
{
    char *s1 = "Hello ";
    char *s2 = "World";
    char *s_null = NULL;

    char *r1 = concat(s1, s2);
    char *r2 = concat(s1, s_null);
    char *r3 = concat(s_null, s2);
    char *r4 = concat(s_null, s_null);

    TEST_ASSERT_EQUAL_STRING("Hello World", r1);
    TEST_ASSERT_EQUAL_STRING(s1, r2);   
    TEST_ASSERT_EQUAL_STRING(s2, r3); 
    TEST_ASSERT_EQUAL_STRING("", r4);

    free(r1); free(r2); free(r3); free(r4);
}

void test_concat_3(void)
{
    char *s1 = "Hello ";
    char *s2 = "big ";
    char *s3 = "World";
    char* s_null = NULL;

    char *r1 = concat_3(s1, s2, s3);
    char *r2 = concat_3(s_null,s2, s3);
    char *r3 = concat_3(s1, s_null, s3);
    char *r4 = concat_3(s1, s2, s_null);

    TEST_ASSERT_EQUAL_STRING("Hello big World", r1);
    TEST_ASSERT_EQUAL_STRING("big World", r2);
    TEST_ASSERT_EQUAL_STRING("Hello World", r3);
    TEST_ASSERT_EQUAL_STRING("Hello big ", r4);

    free(r1); free(r2); free(r3); free(r4);
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
