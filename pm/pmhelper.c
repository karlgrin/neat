<<<<<<< HEAD
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

#include "pmhelper.h"

char*
concat(const char *s1, const char *s2)
{
    if(s1 == NULL && s2 == NULL) { return strdup(""); }
    if(s1 == NULL) { return strdup(s2); }
    if(s2 == NULL) { return strdup(s1); }

    const size_t len1 = strlen(s1);
    const size_t len2 = strlen(s2);
    char *result = calloc(1, len1 + len2 + 1); // +1 for the null-terminator

    if(result == NULL) {
         write_log(__FILE__, __func__, "Error: failed to callloc..");
         return NULL;
    }

    memcpy(result, s1, len1);
    memcpy(result + len1, s2, len2 + 1);
    return result;
}

char*
concat_3(const char *s1, const char *s2, const char *s3)
{
    char *tmp = concat(s1,s2);
    char *result = concat(tmp,s3);
    free(tmp);
    return result;
}

int
file_exist(const char * file_path)
{
   return access(file_path, F_OK) != -1;
}

void
write_log(const char* module, const char* func, const char* desc)
{
    char time_buffer[100];
    time_t now = time (0);
    strftime (time_buffer, 100, "%Y-%m-%d %H:%M:%S.000", localtime (&now));

    FILE *fp = fopen(FILENAME, "a");
    if(fp != NULL) {
        fprintf(fp, "Time: %s  \nModule: %s\nFunction: %s\nDescription: %s\n\n", time_buffer, module, func, desc);
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
file_is_modified(const char *path, time_t oldTime)
{
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        write_log(__FILE__ , __func__, concat("Error when reading file: ", path));
    }
    return file_stat.st_mtime > oldTime;
}


json_t*
load_json_file(const char *file_path)
{
    if(file_path == NULL) { return NULL; }

    json_error_t error;
    json_t *json = json_load_file(file_path, 0, &error);

    if(!json) {
        write_log(__FILE__, __func__, concat("Error: failed to read json file: ", file_path));
    }
    return json;
}

void
write_json_file(const char* filePath, json_t *json)
{
    if(json_dump_file(json, filePath, JSON_INDENT(4)) == -1) {
        write_log(__FILE__, __func__, concat("Error: Unable to generate a Json file, ", filePath));
    }
}
=======
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>

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
        fprintf(fp, "Time: %s  \nModule: %s\nFunction: %s\nDescription: %s\n\n", time_buffer, module, func, desc_buffer);  
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
file_is_modified(const char *path, time_t oldTime) 
{
    struct stat file_stat;
    int err = stat(path, &file_stat);
    if (err != 0) {
        write_log(__FILE__ , __func__, "Error when reading file: %s", path);
    }
    return file_stat.st_mtime > oldTime;
}


json_t* 
load_json_file(const char *file_path) 
{
    if(file_path == NULL) { return NULL; }

    json_error_t error;
    json_t *json = json_load_file(file_path, 0, &error);

    if(!json) {
        write_log(__FILE__, __func__, "Error: failed to read json file: %s", file_path);
    }
    return json;
}

void 
write_json_file(const char* filePath, json_t *json) 
{
    if(json_dump_file(json, filePath, JSON_INDENT(4)) == -1) {
        write_log(__FILE__, __func__, "Error: Unable to generate a Json file, ", filePath);
    }
}                                                                                                                                                                              
>>>>>>> 0a81998ce9377541b45b5b7d162dee5ff746369b
