#include "create_html.h"
#include "utils.h"
#include <locale.h>
#define HTML_MD_BEGIN "<html><head><meta charset=\"UTF-8\"></head><body><link rel=\"stylesheet\" href=\"../style.css\">"
#define HTML_MD_END "</body></html>"
#define HTML_EXCEPTMD_LENGTH (sizeof(HTML_MD_BEGIN) / sizeof(HTML_MD_BEGIN[0]) + sizeof(HTML_MD_END) / sizeof(HTML_MD_END[0]) - 2)


/// @brief 解析markdown 为 html
/// @param filepath markdown文件路径
/// @param length 转换后的字节长度
/// @return 返回转换后的html内容指针
char *pmd(const char *filepath, int *length)
{
    struct buf *ib, *ob;
    int ret;
    FILE *in = stdin;

    struct sd_callbacks callbacks;
    struct html_renderopt options;
    struct sd_markdown *markdown;

    char *content = NULL;
    char *old_locale = NULL;
    /* opening the file if given from the command line */
    printf("will open:%s \n",filepath);
    in = fopen(filepath, "r");
    if (!in)
    {
        fprintf(stderr, "Unable to open input file \"%s\": %s\n", filepath, strerror(errno));
        return NULL;
    }
    /* reading everything */
    ib = bufnew(READ_UNIT);
    bufgrow(ib, READ_UNIT);
    while ((ret = fread(ib->data + ib->size, 1, ib->asize - ib->size, in)) > 0)
    {
        ib->size += ret;
        bufgrow(ib, ib->size + READ_UNIT);
    }

    if (in != stdin)
        fclose(in);

    /* performing markdown parsing */
    ob = bufnew(OUTPUT_UNIT);

    sdhtml_renderer(&callbacks, &options, 0);
    markdown = sd_markdown_new(MKDEXT_FENCED_CODE | MKDEXT_TABLES | MKDEXT_STRIKETHROUGH, 16, &callbacks, &options);

    sd_markdown_render(ob, ib->data, ib->size, markdown);
    content = (char *)malloc(sizeof(char) * (ob->size + HTML_EXCEPTMD_LENGTH + 1));
    if (content != NULL)
    {
        printf("alloc mem succ\n");
    }
    else
    {
        free(content);
        bufrelease(ib);
        bufrelease(ob);
        return NULL;
    }
    // printf("%s%s%s",HTML_MD_BEGIN,ob->data,HTML_MD_END);
    // printf("ok,length \n");
    *length = sprintf(content, "%s%s%s", HTML_MD_BEGIN, ob->data, HTML_MD_END);
    content[*length] = '\0';
    // strcpy(content,ob->data);
    // *length = ob->size;
    sd_markdown_free(markdown);
    bufrelease(ib);
    bufrelease(ob);
    printf("ok,length is %d \n", *length);
    return content;
}

#define HTML_a_TAG_BEGIN "<a href=\"articles/"
#define HTML_a_TAG_MIDDLE "\">"
#define HTML_a_TAG_END "</a><br>"
#define HTML_a_TAG_EXCEPT_LENGTH (sizeof(HTML_a_TAG_BEGIN) / sizeof(HTML_a_TAG_BEGIN[0]) + sizeof(HTML_a_TAG_MIDDLE) / sizeof(HTML_a_TAG_MIDDLE[0]) + sizeof(HTML_a_TAG_END) / sizeof(HTML_a_TAG_END[0]) - 3)

/// @brief 生成包含文章链接的页面
/// @param mkd_floder_path markdown文件的存放位置
/// @param length 返回的内容的长度
/// @return 返回指向包含html内容的指针
char *articles_html(const char *mkd_floder_path,int* length)
{
    mkd_files *mkds;
    mkd_file *mkd;
    char *content;
    int files_number = 0,files_number1;
    int filename_total_length = 0;
    int offset = 0;
    
    mkds = init_mkdroot();
    files_number = get_mkd_files_name(mkd_floder_path, mkds);
    mkd = mkds->head;
    files_number1 = files_number;
    if(files_number == 0){
        *length = 14;
        content = (char*)malloc(sizeof(char) * 14);
        strcpy(content,"Not articles!");
        return content;
    }

    while (files_number--)
    {
        filename_total_length += strlen(mkd->filename_without_suffix);
        mkd = mkd->next;
        printf("seems count\n");
    }

    mkd = mkds->head;
    content = (char*)malloc(sizeof(char) * (HTML_a_TAG_EXCEPT_LENGTH*files_number1 + HTML_EXCEPTMD_LENGTH + filename_total_length * 2 + 10));
    offset = sprintf(content, "%s", HTML_MD_BEGIN);
    printf("%d :: %d",offset,sizeof(HTML_MD_BEGIN) / sizeof(HTML_MD_BEGIN[0]));
    while (mkd)
    {
        offset += sprintf(content + offset, "%s%s%s%s%s", HTML_a_TAG_BEGIN, mkd->filename_without_suffix, HTML_a_TAG_MIDDLE,mkd->filename_without_suffix,HTML_a_TAG_END);
        printf("%s\n",mkd->filename_without_suffix);
        mkd = mkd->next;
    }
    offset += sprintf(content + offset,"%s",HTML_MD_END);
    content[offset] = '\0';
    *length = offset;
    printf("offset=%d while alloc length=%d\n",offset,HTML_a_TAG_EXCEPT_LENGTH*files_number1 + HTML_EXCEPTMD_LENGTH + filename_total_length * 2 + 10);
    free_mkds(mkds);
    return content;
}

int create_static_html(const char* save_path,char* content,int length){
    FILE* fp;
    content[length] = '\0';
    fp = fopen(save_path,"w+");
    if (fp == NULL){
        printf("open file faild.\n");
        return 1;
    }
    fputs(content,fp);
    fclose(fp);
    return 0;
}