#ifndef HEADER_PM_HELPER
#define HEADER_PM_HELPER

#include <time.h>
#include <jansson.h>
#include <stdbool.h>

#define PRECEDENCE_BASE 0
#define PRECEDENCE_OPTIONAL 1
#define PRECEDENCE_IMMUTABLE 2

extern char* neat_dir;
extern char* ib_dir;
extern char* cib_dir;
extern char* pib_dir;
extern char* profile_dir;
extern char* policy_dir;

extern char *pm_socket_path;
extern char *cib_socket_path;
extern char *pib_socket_path;

extern bool debug_enabled;

extern int CIB_DEFAULT_TIMEOUT;

typedef enum {
    LOG_EVENT, LOG_ERROR, LOG_DEBUG, LOG_NEW_LINE
} LOG_LEVEL;

void enable_log_file(bool enable);
void enable_debug_message(bool enable);
void write_log(const char* module, const char* func, LOG_LEVEL log_level, const char *desc, ...);

void pm_helper_close();
int create_folder(char* path);
void start_pm_helper();
char* new_string(char *string, ...);
int file_exist(const char * file_path);
char *get_home_dir();

time_t file_edit_time(const char *file_path);
void clear_log_file();
int file_is_modified(const char *path, time_t old_time);
json_t* load_json_file(const char *file_path);
void write_json_file(const char* file_path, json_t *json);
bool array_contains_value(json_t *array, json_t *value);
char *get_hash();

#endif
