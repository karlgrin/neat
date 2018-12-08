#ifndef HEADER_PM_HELPER
#define HEADER_PM_HELPER

#include <time.h>
#include <jansson.h>
#include<stdbool.h>

#define FILENAME "ErrorLog.txt"

extern char* NEAT_DIR;
extern char* SOCKET_PATH;
extern char* IB_DIR;
extern char* CIB_DIR;
extern char* PIB_DIR;
extern char* PROFILE_DIR;
extern char* POLICY_DIR;

typedef enum {
    LOG_NORMAL, LOG_ERROR, LOG_DEBUG, LOG_NEW_LINE
} LOG_LEVEL;

void enable_log_file(bool enable);
void enable_debug_message(bool enable);
void write_log(const char* module, const char* func, LOG_LEVEL log_level, const char *desc, ...);

int create_folder(char* path);
void create_folders();
char* new_string(char *string, ...);
int file_exist(const char * file_path);

time_t file_edit_time(const char *file_path);
void clear_log_file();
int file_is_modified(const char *path, time_t old_time);
json_t* load_json_file(const char *file_path);
void write_json_file(const char* file_path, json_t *json);
bool array_contains_value(json_t *array, json_t *value);

#endif
