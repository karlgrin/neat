#include "../../../Unity/src/unity.h"
#include "pmhelper_test.h"
#include "property_test.h"
#include "multi_property_test.h"
#include "node_test.h"

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
    RUN_TEST(test_property_init);
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
    RUN_TEST(test_json_to_property);
    RUN_TEST(test_json_to_value);
    RUN_TEST(test_json_to_type);

    printf("\n-----multi_property.c------\n");
    RUN_TEST(test_multi_property_init);
    RUN_TEST(test_add_multi_property);
    RUN_TEST(test_add_multi_property_2);
    RUN_TEST(test_has_a_property);
    RUN_TEST(test_has_a_property_2);
    RUN_TEST(test_get_a_property);
    RUN_TEST(test_get_a_property_2);

    printf("\n-----node.c------\n");
    RUN_TEST(test_node_init);
    RUN_TEST(test_update_node_content);
    RUN_TEST(test_create_node);
    RUN_TEST(test_add_node);
    RUN_TEST(test_add_node_2);
    RUN_TEST(test_has_node);
    RUN_TEST(test_has_node_2);
    RUN_TEST(test_get_node);
    RUN_TEST(test_get_node_2);
    RUN_TEST(test_remove_node);
    RUN_TEST(test_remove_node_2);

    return UNITY_END();
}