#include "../../../Unity/src/unity.h"
#include "pmhelper_test.h"
#include "property_test.h"
#include "multi_property_test.h"

int main(void)
{
    UNITY_BEGIN();
    printf("   Start Unit Tests!\n");

    printf("\n-----pmhelper.c------\n");
    RUN_TEST(test_concat);
    RUN_TEST(test_file_exit);
    RUN_TEST(test_load_json_file);
    RUN_TEST(test_write_json_file);
    RUN_TEST(test_file_is_modified);
    RUN_TEST(test_file_edit_time);

    printf("\n-----property.c------\n");
    RUN_TEST(test_add_property);
    RUN_TEST(test_add_property_2);
    RUN_TEST(test_get_property);
    RUN_TEST(test_get_property_2);
    RUN_TEST(test_remove_property);
    RUN_TEST(test_remove_property_2);
    RUN_TEST(test_has_property);
    RUN_TEST(test_has_property_2);
    RUN_TEST(test_overwrite_property);
    RUN_TEST(test_overwrite_property_2);

    printf("\n-----multi_property.c------\n");
    RUN_TEST(test_add_multi_property);
    RUN_TEST(test_add_multi_property_2);
    RUN_TEST(test_has_a_property);
    RUN_TEST(test_has_a_property_2);
    RUN_TEST(test_get_a_property);
    RUN_TEST(test_get_a_property_2);

    return UNITY_END();
}