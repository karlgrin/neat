#ifndef HEADER_PMHELPER
#define HEADER_PMHELPER

#include <time.h>
#include <jansson.h>
#include<stdbool.h>

#define FILENAME "ErrorLog.txt"

#define IB_DIR "infobase/"
#define PROFILE_DIR IB_DIR "pib/profile/"
#define POLICY_DIR  IB_DIR "pib/policy/"
#define CIB_DIR     IB_DIR "cib/"

char* new_string(char *string, ...);
char* get_home_dir();
char* get_exec_path();
int file_exist(const char * file_path);
void write_log(const char* module, const char* func, const char* desc, ...);
time_t file_edit_time(const char *file_path);
void clear_log();
int file_is_modified(const char *path, time_t old_time);
json_t* load_json_file(const char *file_path);
void write_json_file(const char* file_path, json_t *json);
bool array_contains_value(json_t *array, json_t *value);
char *get_hash();

#endif
