#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <errno.h>
#include <pwd.h>

#include "pm_helper.h"

#define LOG_FILENAME "Log.txt"

char* NEAT_DIR;
char* SOCKET_PATH;
char* IB_DIR;
char* CIB_DIR;
char* PIB_DIR;
char* PROFILE_DIR;
char* POLICY_DIR;

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
    NEAT_DIR    = new_string("%s/%s", get_home_dir(), ".neat/");
    SOCKET_PATH = new_string("%s%s", NEAT_DIR, "neat_pm_socket");
    IB_DIR      = new_string("%s%s", NEAT_DIR, "infobase/");
    CIB_DIR     = new_string("%s%s", IB_DIR, "cib/");
    PIB_DIR     = new_string("%s%s", IB_DIR, "pib/");
    PROFILE_DIR = new_string("%s%s", PIB_DIR, "profile/");
    POLICY_DIR  = new_string("%s%s", PIB_DIR, "policy/");

    create_folder(NEAT_DIR);
    create_folder(IB_DIR);
    create_folder(CIB_DIR);
    create_folder(PIB_DIR);
    create_folder(PROFILE_DIR);
    create_folder(POLICY_DIR);
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
