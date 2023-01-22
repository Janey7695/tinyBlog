#ifndef _UTILS_H_
#define _UTILS_H_


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

#include <sys/stat.h>
#include <sys/types.h>

typedef struct 
{
    int year;
    int mon;
    int day;
    int hour;
    int min;
}timestamp;


typedef struct markdown_file
{
    char* filename;
    char* filename_without_suffix;
    long  modify_time;
    struct markdown_file* next;
}mkd_file;

typedef struct markdown_files
{
    struct markdown_file* head;
    struct markdown_file* tail;
    int total_mkd_file_number;
}mkd_files;

mkd_files* init_mkdroot();
int get_mkd_files_name(const char *dirpath, mkd_files* mkds);
void free_mkds(mkd_files* mkdroot);
int create_dir(const char *dirpath);

#endif