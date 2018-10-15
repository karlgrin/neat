#include "../../../Unity/src/unity.h"
#include "property_test.h"
#include "multi_property_test.h"

int main(void)
{
    UNITY_BEGIN();
    
    printf("\n-----property.c------\n");
    RUN_TEST(test_add_property);
    RUN_TEST(test_add_property_2);
    RUN_TEST(test_get_property);
    RUN_TEST(test_get_property_2);
    RUN_TEST(test_remove_property);
    RUN_TEST(test_remove_property_2);
    RUN_TEST(test_has_property);
    RUN_TEST(test_has_property_2);
    //RUN_TEST(test_overwrite_property);
   // RUN_TEST(test_overwrite_property_2);

    printf("\n-----multi_property.c------\n");
    RUN_TEST(test_add_multi_property);
    RUN_TEST(test_add_multi_property_2);
    RUN_TEST(test_has_a_property);
    RUN_TEST(test_has_a_property_2);

    return UNITY_END();
}