#include <string.h>

#include "../../../Unity/src/unity.h"
#include "test_constants.h"
#include "../parse_json.h"
#include "../pmhelper.h"

char *expand_result = "[[{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 4, \"local_ip\": \"10.0.2.15\"}, {\"test1\": 4, \"local_ip\": \"10.0.2.15\"}], [{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}, {\"test1\": 4, \"local_ip\": \"10.0.2.15\"}], [{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 4, \"local_ip\": \"10.0.2.15\"}, {\"test2\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}], [{\"interface\": \"enp0s3\", \"local_interface\": true}, {\"ip_version\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}, {\"test2\": 6, \"local_ip\": \"fe80::9096:12f:5a38:d228%enp0s3\"}]]";

void remove_symbol(char *str, char symbol) 
{  
    int count = 0; 
 
    for (int i = 0; str[i]; i++) {
        if (str[i] != symbol) {
            str[count++] = str[i]; 
        }
    } 
                                   
    str[count] = '\0'; 
}

void test_expand(void)
{
    json_t *json = load_json_file(TEST_FILE_PATH_CIB);
    
    json_t *properties = json_object_get(json, "properties");

    if(json_is_array(properties)) {
        json_t *ex = expand(properties);
        char *s = json_dumps(ex, 0);
        remove_symbol(s, '\n');
        TEST_ASSERT_EQUAL_STRING(expand_result, s);
        json_decref(ex); json_decref(json); free(s);
    } else {
        TEST_FAIL();
    }
}
 