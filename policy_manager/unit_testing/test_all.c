#include "../../../Unity/src/unity.h"
#include "pm_helper_test.h"
#include "node_test.h"
#include "parse_json_test.h"

int main(void)
{
    UNITY_BEGIN();
    //printf("\n   Start Unit Tests!\n");

    //printf("\n-----pm_helper.c------\n");
    RUN_TEST(test_new_string);
    RUN_TEST(test_file_exist);
    RUN_TEST(test_load_json_file);
    RUN_TEST(test_write_json_file);
    RUN_TEST(test_file_is_modified);
    RUN_TEST(test_file_edit_time);
    RUN_TEST(test_array_contains_value);

    //printf("\n-----node.c------\n");
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
    RUN_TEST(test_get_node_properties);
    RUN_TEST(test_node_has_property);
    RUN_TEST(test_node_set_property);
    RUN_TEST(test_node_set_property_2);

    //printf("\n-----parse_json.c------\n");
    RUN_TEST(test_expand_json_array);
    RUN_TEST(test_expand_json);
    RUN_TEST(test_sort_json_array);
    RUN_TEST(test_limit_json_array);
    RUN_TEST(test_process_special_properties);
    RUN_TEST(test_expand_properties);
    RUN_TEST(test_expand_value);

    return UNITY_END();
}
