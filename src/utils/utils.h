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

#define DEBUG_LOG

/*loglevel to print 
 *
 * 0 = all level to print
 * 1 = don't print Normal level
 * 2 = only print Warn and Error level
 * 3 = only print Error level
 *
 * */
#define LOGLEVEL_TO_PRINT 2 

#define COLOR_RED_NUM 31
#define COLOR_GREEN_NUM 32
#define COLOR_YELLOW_NUM 33
#define COLOR_NORMAL_NUM 0

#define LOG_PLAIN(color, level, message, ...)                       \
    {                                                               \
        char tss[80] = "";                                          \
        get_current_timestamp(tss);                                 \
        printf("%s - \033[%dm [%s] \033[0m : ", tss, color, level); \
        printf(message, ##__VA_ARGS__);                             \
        printf("\r\n");                                             \
    }
#if LOGLEVEL_TO_PRINT == 0
#define LOG_WARN(message, ...) LOG_PLAIN(COLOR_YELLOW_NUM, "warning", message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) LOG_PLAIN(COLOR_RED_NUM, "error", message, ##__VA_ARGS__)
#define LOG_NORMAL(message, ...) LOG_PLAIN(COLOR_NORMAL_NUM, "log", message, ##__VA_ARGS__)
#define LOG_SUCCESS(message, ...) LOG_PLAIN(COLOR_GREEN_NUM, "ok", message, ##__VA_ARGS__)
#elif  LOGLEVEL_TO_PRINT == 1
#define LOG_WARN(message, ...) LOG_PLAIN(COLOR_YELLOW_NUM, "warning", message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) LOG_PLAIN(COLOR_RED_NUM, "error", message, ##__VA_ARGS__)
#define LOG_NORMAL(message, ...) 
#define LOG_SUCCESS(message, ...) LOG_PLAIN(COLOR_GREEN_NUM, "ok", message, ##__VA_ARGS__)
#elif LOGLEVEL_TO_PRINT == 2
#define LOG_WARN(message, ...) LOG_PLAIN(COLOR_YELLOW_NUM, "warning", message, ##__VA_ARGS__)
#define LOG_ERROR(message, ...) LOG_PLAIN(COLOR_RED_NUM, "error", message, ##__VA_ARGS__)
#define LOG_NORMAL(message, ...) 
#define LOG_SUCCESS(message, ...) 
#else 
#define LOG_WARN(message, ...) 
#define LOG_ERROR(message, ...) LOG_PLAIN(COLOR_RED_NUM, "error", message, ##__VA_ARGS__)
#define LOG_NORMAL(message, ...) 
#define LOG_SUCCESS(message, ...) 
#endif

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
char *remove_suffix(char *filename, int stringLength);
char *get_suffix(char *filename);
int create_dir(const char *dirpath);
configures *read_configure_json(const char *config_file_path);
void print_configure(configures *configure);
void urldecode(char url[]);
void get_current_timestamp(char *tss);
configures *get_configures_point();
int targs_deal_args(int argc, char **argv);

#endif
