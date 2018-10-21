#include <string.h>

#include "../../../Unity/src/unity.h"
#include "test_constants.h"
#include "../parse_json.h"
#include "../pmhelper.h"

char *expand_result = "[[{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 4, \"local_ip\": \"10.0.2.15\"}, {\"test1\": 4, \"local_ip\": \"10.0.2.15\"}], [{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}, {\"test1\": 4, \"local_ip\": \"10.0.2.15\"}], [{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 4, \"local_ip\": \"10.0.2.15\"}, {\"test2\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}], [{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}, {\"test2\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}]]";

void test_expand_json_array(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH_CIB);
    
    json_t *properties = json_object_get(json, "properties");

    if(json_is_array(properties)) {
        json_t *ex = expand_json_arrays(properties);
        char *s = json_dumps(ex, 0);
        TEST_ASSERT_EQUAL_STRING(expand_result, s);
        json_decref(json); json_decref(ex); free(s);
    } else {
        TEST_FAIL();
    }
}

void test_expand_json(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH);
    json_t *ex = expand_json(json_object_get(json, "properties"));
    
    if(json_is_array(ex)) {
        size_t index;
        json_t *value;

        json_array_foreach(ex, index, value) {
            if(index == 0) { TEST_ASSERT_NOT_NULL(json_object_get(value, "RTT")); }
            if(index == 1) { TEST_ASSERT_NOT_NULL(json_object_get(value, "low_latency_interface")); }
            if(index == 2) { TEST_ASSERT_NOT_NULL(json_object_get(value, "is_wired_interface")); }
        }

        json_t *json_null = expand_json(NULL);
        if(json_is_array(ex) && json_array_size(ex)) {
            json_decref(json); json_decref(ex); json_decref(json_null);
            return;
        }
    }
    TEST_FAIL();
}
 