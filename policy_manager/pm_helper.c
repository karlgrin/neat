#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>

#include "pm_helper.h"

#define LOG_FILENAME "Log.txt"

char* neat_dir;
char* SOCKET_PATH;
char* ib_dir;
char* cib_dir;
char* pib_dir;
char* profile_dir;
char* policy_dir;

char* pm_socket_path;
char* cib_socket_path;
char* pib_socket_path;

int CIB_DEFAULT_TIMEOUT = 60*10;

bool debug_enabled = false;
bool log_file_enabled = false;

char*
get_home_dir()
{
    char *homedir;

    homedir = getenv("HOME");

    if (homedir == NULL) {
        homedir = getpwuid(getuid())->pw_dir;
    }

    return homedir;
}

char*
get_current_dir(){
    char buf[1014];

    if (getcwd(buf, sizeof(buf)) != NULL) {
        char *dir = new_string(buf);
        return dir;
    } else {
        write_log(__FILE__ , __func__, LOG_ERROR, "Unable to get current directory..");
        return "";
    }
}

int
create_folder(char* path)
{
    struct stat st = {0};

    if (stat(path, &st) == -1) {
        if(mkdir(path, 0700) == -1) {
            write_log(__FILE__, __func__,LOG_ERROR, "Failed to create directory: %s, error message: %s", path, strerror(errno));
            return -1;
        }
    }
    return 0;
}

void
create_folders()
{
    neat_dir    = new_string("%s/%s", get_home_dir(), ".neat/");
    ib_dir      = new_string("%s%s", neat_dir, "infobase/");
    cib_dir     = new_string("%s%s", ib_dir, "cib/");
    pib_dir     = new_string("%s%s", ib_dir, "pib/");
    profile_dir = new_string("%s%s", pib_dir, "profile/");
    policy_dir  = new_string("%s%s", pib_dir, "policy/");

    create_folder(neat_dir);
    create_folder(ib_dir);
    create_folder(cib_dir);
    create_folder(pib_dir);
    create_folder(profile_dir);
    create_folder(policy_dir);
}

void pm_helper_close()
{
    free(neat_dir);
    free(SOCKET_PATH);
    free(ib_dir);
    free(cib_dir);
    free(pib_dir);
    free(profile_dir);
    free(policy_dir);
}

void create_socket_paths() {
    pm_socket_path  = new_string("%s%s", neat_dir, "neat_pm_socket");
    cib_socket_path = new_string("%s%s", neat_dir, "neat_cib_socket");
    pib_socket_path = new_string("%s%s", neat_dir, "neat_pib_socket");

    write_log(__FILE__, __func__, LOG_EVENT, "Socket created in %s", pm_socket_path);
    write_log(__FILE__, __func__, LOG_EVENT, "Socket created in %s", cib_socket_path);
    write_log(__FILE__, __func__, LOG_EVENT, "Socket created in %s", pib_socket_path);
    write_log(__FILE__, __func__, LOG_NEW_LINE, "\n");
}

void start_pm_helper() 
{
    create_folders();
    create_socket_paths();
}


char*
new_string(char *string, ...)
{
    char buffer[1000];
	va_list arglist;

	va_start(arglist, string);
	vsprintf(buffer, string, arglist);
	va_end(arglist);

    const size_t len = strlen(buffer);
    char *result = calloc(1, len + 1); // +1 for the null-terminator

    if(result == NULL) {
         write_log(__FILE__, __func__, LOG_ERROR, "Failed to allocate memory");
         return NULL;
    }

    memcpy(result, buffer, len);
    return result;
}

int
file_exist(const char * file_path)
{
   return access(file_path, F_OK) != -1;
}

void
enable_log_file(bool enable)
{
    if(enable) {
        char* current_dir = get_current_dir();
        write_log(__FILE__, __func__, LOG_EVENT, "Log messages: %s/%s\n", current_dir, LOG_FILENAME);
        free(current_dir);
    }
    log_file_enabled = enable;
}

void
enable_debug_message(bool enable)
{
    debug_enabled = enable;
    if(enable)
        write_log(__FILE__, __func__, LOG_EVENT, "Debug mode: ON\n");
}

void
write_log(const char* module, const char* func, LOG_LEVEL log_level, const char* format, ...)
{
    char* log_type;
    switch(log_level) {
        case LOG_EVENT:
            log_type = "EVENT";
            break;
        case LOG_ERROR:
            log_type = "ERROR";
            printf("\n[ERROR] ");
            break;
        case LOG_DEBUG:
            log_type = "DEBUG";
            if(debug_enabled) {
                printf("[DEBUG] ");
            }
            break;
        case LOG_NEW_LINE:
            printf("\n");
            return;
    }

    //write to console
    if(log_level != LOG_DEBUG || debug_enabled) {
        va_list argptr;
        va_start(argptr, format);

        vprintf(format, argptr);
        printf("\n");
        if(log_level == LOG_ERROR) { printf("\n"); }

        va_end(argptr);
    }

    //write to log file
    if(log_file_enabled) {
        FILE *fp = fopen(LOG_FILENAME, "a");
        if(fp != NULL) {
            va_list argptr;
            va_start(argptr, format);

            char time_buffer[100];
            time_t now = time (0);
            strftime (time_buffer, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));

            fprintf(fp, "Log Type: %s\nTime: %s  \nModule: %s\nFunction: %s\nDescription: ", log_type, time_buffer, module, func);
            vfprintf(fp, format, argptr);
            fprintf(fp, "\n\n");

            fclose(fp);
            va_end(argptr);
        }
        else {
            write_log(__FILE__ , __func__, LOG_ERROR, "Cannot access log file");
        }
    }
}

time_t
file_edit_time(const char *file_path)
{
    struct stat attr;
    stat(file_path, &attr);
    return attr.st_mtime;
}

void
clear_log_file()
{
    FILE *fp = fopen(LOG_FILENAME, "w");
    if(fp != NULL) {
        fclose(fp);
    }
}

//Returns 0 if file is not found
int
file_is_modified(const char *path, time_t old_time)
{
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        write_log(__FILE__ , __func__, LOG_ERROR, "Failure when reading: %s", path);
    }
    return file_stat.st_mtime > old_time;
}


json_t*
load_json_file(const char *file_path)
{
    if(file_path == NULL) { return NULL; }

    json_error_t error;
    json_t *json = json_load_file(file_path, 0, &error);

    if(!json) {
        write_log(__FILE__, __func__, LOG_ERROR, "Failed to read json file: %s",file_path);
    }
    return json;
}

void
write_json_file(const char* file_path, json_t *json)
{
    if(json_dump_file(json, file_path, JSON_INDENT(4)) == -1) {
        write_log(__FILE__, __func__, LOG_ERROR, "Unable to generate JSON file %s", file_path);
    }
}

bool
array_contains_value(json_t *array, json_t *value)
{
    size_t i;
    json_t *elem;
    json_array_foreach(array, i, elem) {
        if (json_equal(elem, value)) {
            return true;
        }
    }
    return false;
}

char *
get_hash()
{
    int hash_length = 30;
    char *hash = malloc((hash_length) * sizeof(char));
    time_t t;
    srandom((unsigned) time(&t));

    const char *charset = "0123456789abcdef";

    for (int i = 0; i < hash_length; i++) {
        *hash = *(charset + (random() % 16));  // Following chars in range 0..15
        hash++;
    }
    hash -= hash_length;
    return hash;
}
