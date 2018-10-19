#ifndef HEADER_PMHELPER
#define HEADER_PMHELPER

#include <time.h>
#include <jansson.h>

#define FILENAME "ErrorLog.txt"

#define PROFILE_DIR "json_examples/pib/profile/"
#define POLICY_DIR  "json_examples/pib/policy/"
#define CIB_DIR     "json_examples/cib/"

char* new_string(char *string, ...);
char* get_exec_path();
int file_exist(const char * file_path);
void write_log(const char* module, const char* func, const char* desc, ...);
time_t file_edit_time(const char *file_path);
void clear_log();
int file_is_modified(const char *path, time_t old_time);
json_t* load_json_file(const char *file_path);
void write_json_file(const char* file_path, json_t *json);

#endif
