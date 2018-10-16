#include "../../../Unity/src/unity.h"
#include "../pmhelper.h"

char *JSON_FILE_PATH = "json_files/test.profile";

void test_concat(void)
{
    char *s1 = "Hello ";
    char *s2 = "World";

    TEST_ASSERT_EQUAL_STRING("Hello World", concat(s1,s2));
}

void test_file_exit(void)
{
    if(file_exist("pmhelper_test.c") == 0)
        TEST_FAIL();

    if(file_exist("Does not exist") != 0)
        TEST_FAIL();
}

void test_load_json_file(void) 
{
    json_t *json1 = load_json_file(JSON_FILE_PATH);

    if(json1 == NULL)
        TEST_FAIL();

    char *s = NULL;
    json_t *json2 = load_json_file(s);

    if(json2 != NULL)   
        TEST_FAIL();
}

void test_write_json_file(void)
{
    json_t *json = load_json_file(JSON_FILE_PATH);

    if(json == NULL)
        TEST_FAIL();

    int removed = remove(JSON_FILE_PATH);

    if(removed == 0 && file_exist(JSON_FILE_PATH) != 0)
        TEST_FAIL();

    write_json_file(JSON_FILE_PATH, json);

    if(file_exist(JSON_FILE_PATH) != 1)
        TEST_FAIL();
}

void test_file_is_modified(void)
{
    if(file_is_modified(JSON_FILE_PATH, time(0)) != 0)    
        TEST_FAIL();

    if(file_is_modified(JSON_FILE_PATH, 0) == 0)    
        TEST_FAIL();
}

void test_file_edit_time(void)
{
    if(file_edit_time(JSON_FILE_PATH) == 0)
        TEST_FAIL();
}
