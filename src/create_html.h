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

enum {
    PAGE_TYPE_MARKDOWN = 0,
    PAGE_TYPE_MAINPAGE,
    PAGE_TYPE_LISTPAGE
};

char *wrap_with_html_heads(char* content,int *Length,int pageType);
char *parse_md_to_htmlBytesStream(const char *filepath, int *length);
char *parse_articlesList_to_htmlBytesStream(const char *mkd_floder_path,int* length);
char* pmd(const char* filepath,int* length);
char *articles_html(const char *mkd_floder_path,int* length);
int create_static_html(const char* save_path,char* content,int length);
char *create_nav_htmlBytesStream(int *length);
char *parse_mdtoc_to_htmlBytesStream(const char *filepath, int *length);


#endif