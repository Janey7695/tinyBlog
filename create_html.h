#ifndef _CREATE_HTML_H_
#define _CREATE_HTML_H_

#include "markdown.h"
#include "html.h"
#include "buffer.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define READ_UNIT 1024
#define OUTPUT_UNIT 64

char* pmd(const char* filepath,int* length);
char *articles_html(const char *mkd_floder_path,int* length);
int create_static_html(const char* save_path,char* content,int length);


#endif