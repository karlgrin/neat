#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <pwd.h>

#include "pmhelper.h"

#define FILENAME "ErrorLog.txt"

char*
new_string(char *string, ...)
{
    char buffer[100];
	va_list arglist;

	va_start(arglist, string);
	vsprintf(buffer, string, arglist);
	va_end(arglist);

    const size_t len = strlen(buffer);
    char *result = calloc(1, len + 1); // +1 for the null-terminator

    if(result == NULL) {
         write_log(__FILE__, __func__, "Error: failed to callloc..");
         return NULL;
    }

    memcpy(result, buffer, len);
    return result;
}

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

// TODO currently not used
char*
get_exec_path(){
    char buf[1024], *ptr;
    ssize_t len;
    if ((len = readlink("/proc/self/exe", buf, sizeof(buf)-1)) != -1){
        buf[len] = '\0';
    }
    ptr = strstr(buf, "/pm");
    strcpy(ptr, "/");
    char *ret_ptr = buf;
    printf("exec path: %s\n", buf);
    return ret_ptr;
}

int
file_exist(const char * file_path)
{
   return access(file_path, F_OK) != -1;
}

void ps(const char *str, ...)
{
    va_list arg;

    va_start(arg, str);

    while (str) {
        puts(str);
        str = va_arg(arg, const char *);
    }

    va_end(arg);
}

void
write_log(const char* module, const char* func, const char *desc, ...)
{
    char desc_buffer[100];
	va_list arglist;

	if(!desc)
	  return;

	va_start(arglist, desc);
	vsprintf(desc_buffer, desc, arglist);
	va_end(arglist);

    char time_buffer[100];
    time_t now = time (0);
    strftime (time_buffer, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));

    FILE *fp = fopen(FILENAME, "a");
    if(fp != NULL) {
        fprintf(fp, "Time: %s  \nModule: %s\nFunction: %s\nDescription:%s\n\n", time_buffer, module, func, desc_buffer);
        fclose(fp);
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
clear_log()
{
    FILE *fp = fopen(FILENAME, "w");
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
        write_log(__FILE__ , __func__, "Error when reading file: %s", path);
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
        write_log(__FILE__, __func__, "Error: failed to read json file: %s",file_path);
    }
    return json;
}

void
write_json_file(const char* file_path, json_t *json)
{
    if(json_dump_file(json, file_path, JSON_INDENT(4)) == -1) {
        write_log(__FILE__, __func__, "Error: Unable to generate JSON file %s", file_path);
    }
    else {
        printf("Write file: %s\n", file_path);
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
