#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "cJSON.h"

#include "tversion.h"

// #define DEBUG_LOG

/*loglevel to print 
 *
 * 0 = all level to print
 * 1 = don't print Normal level
 * 2 = only print Warn and Error level
 * 3 = only print Error level
 *
 * */

#ifdef DEBUG_LOG
#define DEBUG_PLAIN(color, level, message, ...)                       \
    {                                                               \
        char tss[80] = "";                                          \
        get_current_timestamp(tss);                                 \
        printf("%s - \033[%dm [%s] \033[0m : ", tss, color, level); \
        printf(message, ##__VA_ARGS__);                             \
        printf("\r\n");                                             \
    }
#else
#define DEBUG_PLAIN(color, level, message, ...)          {char* tss = NULL;}            
#endif
#define DEBUG_WARN(message, ...) DEBUG_PLAIN(COLOR_YELLOW_NUM, "debug-warning", message, ##__VA_ARGS__)
#define DEBUG_ERROR(message, ...) DEBUG_PLAIN(COLOR_RED_NUM, "debug-error", message, ##__VA_ARGS__)
#define DEBUG_NORMAL(message, ...) DEBUG_PLAIN(COLOR_NORMAL_NUM, "debug-log", message, ##__VA_ARGS__)
#define DEBUG_SUCCESS(message, ...) DEBUG_PLAIN(COLOR_GREEN_NUM, "debug-ok", message, ##__VA_ARGS__)

#define TMALLOC(type,size) 		(type *)malloc(sizeof(type) * size)


enum configureItems_index {
    CONFIGURE_PORT = 0,
    CONFIGURE_MKDPATH,
    CONFIGURE_THREADS,
    CONFIGURE_THEME,
    TOTAL_CONFIGURES
};

typedef struct
{
    char* items[TOTAL_CONFIGURES];

} configures;

extern configures globalConfigure;

typedef struct markdown_file
{
    char *filename;
    char *filename_without_suffix;
    long modify_time;
    struct markdown_file *next;
} mkd_file;

typedef struct markdown_files
{
    struct markdown_file *head;
    struct markdown_file *tail;
    int total_mkd_file_number;
} mkd_files;

mkd_files *init_mkdroot();
int get_mkd_files_name(const char *dirpath, mkd_files *mkds);
void free_mkds(mkd_files *mkdroot);
char *remove_suffix(const char *filename, int stringLength);
char *get_suffix(const char *filename);
int create_dir(const char *dirpath);
configures *read_configure_json(const char *config_file_path);
void print_configure(configures *configure);
void urldecode(char url[]);
void get_current_timestamp(char *tss);
configures *get_configures_point();
int targs_deal_args(int argc, char **argv);


char *skip_char(char *s1,char skipchar);
char *seek_until(char *s1,char overchar);

#endif
