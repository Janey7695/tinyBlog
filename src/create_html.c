#include "create_html.h"
#include "./utils/utils.h"
#include <locale.h>

#define HTML_MD_BEGIN "<html><head><meta charset=\"UTF-8\">"
#define HTML_MD_END "</body></html>"
#define HTML_EXCEPTMD_LENGTH (sizeof(HTML_MD_BEGIN) / sizeof(HTML_MD_BEGIN[0]) + sizeof(HTML_MD_END) / sizeof(HTML_MD_END[0]) - 2)

/// @brief 生成包含指明css样式表路径的标签字节流
/// @param configure 全局配置configure的指针
/// @param pageType 需要生成的页面的类型，有PAGE_TYPE_MARKDOWN，PAGE_TYPE_MAINPAGE，PAGE_TYPE_LISTPAGE
/// @return 包含指明css样式表路径的标签字节流的指针
char *create_style_tag(configures *configure, int pageType)
{
    char *contentStyleTag = (char *)malloc(sizeof(char) * (100));
    if (contentStyleTag == NULL)
    {
        DEBUG_WARN("alloc mem faild in %s", __func__)
        return NULL;
    }
    switch (pageType)
    {
    case PAGE_TYPE_MARKDOWN:
        sprintf(contentStyleTag, "<link rel=\"stylesheet\" href=\"../themes/%s/mkd-style.css\"><body>", configure->items[CONFIGURE_THEME]);
        break;
    case PAGE_TYPE_MAINPAGE:
        sprintf(contentStyleTag, "<link rel=\"stylesheet\" href=\"../themes/%s/home-style.css\"><body>", configure->items[CONFIGURE_THEME]);
        break;
    case PAGE_TYPE_LISTPAGE:
        sprintf(contentStyleTag, "<link rel=\"stylesheet\" href=\"../themes/%s/list-style.css\"><body>", configure->items[CONFIGURE_THEME]);
        break;
    default:
        sprintf(contentStyleTag, "<link rel=\"stylesheet\" href=\"../themes/%s/style.css\"><body>", configure->items[CONFIGURE_THEME]);
        break;
    }
    return contentStyleTag;
}

/// @brief 用html的头和尾包裹字节流
/// @param content
/// @param Length
/// @return
char *wrap_with_html_heads(char *content, int *Length, int pageType)
{
    int len=0;
    char *contentWithHtmlHeads = (char *)malloc(sizeof(char) * (*Length + HTML_EXCEPTMD_LENGTH + 101));
    DEBUG_WARN("alloc %d bytes in %s",*Length + HTML_EXCEPTMD_LENGTH + 101,__func__)
    char *contentStyleTag = create_style_tag(get_configures_point(), pageType);
    if (contentWithHtmlHeads == NULL)
    {
        DEBUG_WARN("alloc mem faild in %s .", __func__)
        return NULL;
    }
    if (contentStyleTag == NULL)
    {
        LOG_WARN("couldn't find specified theme")
        len = sprintf(contentWithHtmlHeads, "%s%s%s", HTML_MD_BEGIN, content, HTML_MD_END);
    }
    else
    {
        len = sprintf(contentWithHtmlHeads, "%s%s%s%s", HTML_MD_BEGIN, contentStyleTag, content, HTML_MD_END);
        free(contentStyleTag);
    }
    contentWithHtmlHeads[len] = '\0';
    DEBUG_WARN("write %d bytes in %s",len,__func__)
    *Length = len;
    // *Length = sprintf(contentWithHtmlHeads,"%s%s%s", HTML_MD_BEGIN, content, HTML_MD_END);
    return contentWithHtmlHeads;
}

#define HTML_NAV_TAG "<div class=\"container\"><nav><a href=\"/themes/pop-blue/index.html\">home</a><a href=\"/article-list\">archive</a><a href=\"/about-me\">about</a></nav></div>"
#define HTML_NAV_TAG_LENGTH (sizeof(HTML_NAV_TAG) / sizeof(HTML_NAV_TAG[0]) -1)

/// @brief 生成包含nav导航标签的字节流
/// @param length 
/// @return 包含nav导航标签的字节流的指针
char *create_nav_htmlBytesStream(int *length)
{
    int len=0;
    char *content = (char*)malloc(sizeof(char) * (HTML_NAV_TAG_LENGTH +1));
    if(content == NULL){
        DEBUG_WARN("alloc mem faild in %s",__func__)
        return NULL;
    }
    DEBUG_WARN("alloc %d bytes in %s",HTML_NAV_TAG_LENGTH +1,__func__)
    len = sprintf(content,"%s",HTML_NAV_TAG);
    content[len] = '\0';
    DEBUG_WARN("write %d bytes in %s",len,__func__)
    *length = len;
    return content;
}

/// @brief 解析markdown文件生成toc的html字节流
/// @param filepath markdown文件路径
/// @param length 
/// @return 字节流指针
char *parse_mdtoc_to_htmlBytesStream(const char *filepath, int *length){
    struct buf *ib, *ob;
    int ret;
    int len=0;
    FILE *in = stdin;

    struct sd_callbacks callbacks;
    struct html_renderopt options;
    struct sd_markdown *markdown;

    char *content = NULL;

    // LOG_NORMAL("will open : %s.",filepath)

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

    sdhtml_toc_renderer(&callbacks, &options);
    markdown = sd_markdown_new(0, 16, &callbacks, &options);
    sd_markdown_render(ob, ib->data, ib->size, markdown);
    sd_markdown_free(markdown);

    sdhtml_renderer(&callbacks, &options, HTML_TOC);
    markdown = sd_markdown_new(MKDEXT_FENCED_CODE | MKDEXT_TABLES | MKDEXT_STRIKETHROUGH, 16, &callbacks, &options);
    sd_markdown_render(ob, ib->data, ib->size, markdown);
    sd_markdown_free(markdown);

    content = (char *)malloc(sizeof(char) * (strlen(ob->data) + 2));

    if (content != NULL)
    {
        DEBUG_SUCCESS("alloc mem for markdown2html buffer succed.")
        DEBUG_WARN("alloc %d bytes in %s",strlen(ob->data) + 2,__func__)
        // printf("alloc mem succ\n");
    }
    else
    {
        free(content);
        bufrelease(ib);
        bufrelease(ob);

        DEBUG_WARN("alloc mem for markdown2html buffer faild in %s", __func__)

        return NULL;
    }
    len = sprintf(content, "%s", ob->data);
    content[len] = '\0';
    DEBUG_WARN("write %d bytes in %s",len,__func__)
    *length = len;
    bufrelease(ib);
    bufrelease(ob);

    return content;
}

/// @brief 解析markdown 为 包含html标签的字节流
/// @param filepath markdown文件路径
/// @param length 转换后的字节长度
/// @return 返回转换后的字节流指针
char *parse_md_to_htmlBytesStream(const char *filepath, int *length)
{
    struct buf *ib, *ob;
    int ret;
    int len=0;
    FILE *in = stdin;

    struct sd_callbacks callbacks;
    struct html_renderopt options;
    struct sd_markdown *markdown;

    char *content = NULL;

    // LOG_NORMAL("will open : %s.",filepath)

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

    // sdhtml_renderer(&callbacks, &options, HTML_TOC);
    // markdown = sd_markdown_new(MKDEXT_FENCED_CODE | MKDEXT_TABLES | MKDEXT_STRIKETHROUGH, 16, &callbacks, &options);

    // sd_markdown_render(ob, ib->data, ib->size, markdown);

    sdhtml_toc_renderer(&callbacks, &options);
    markdown = sd_markdown_new(MKDEXT_FENCED_CODE | MKDEXT_TABLES | MKDEXT_STRIKETHROUGH, 16, &callbacks, &options);
    sd_markdown_render(ob, ib->data, ib->size, markdown);
    sd_markdown_free(markdown);

    sdhtml_renderer(&callbacks, &options, HTML_TOC);
    markdown = sd_markdown_new(MKDEXT_FENCED_CODE | MKDEXT_TABLES | MKDEXT_STRIKETHROUGH, 16, &callbacks, &options);
    sd_markdown_render(ob, ib->data, ib->size, markdown);
    sd_markdown_free(markdown);

    content = (char *)malloc(sizeof(char) * (strlen(ob->data) + 2));

    if (content != NULL)
    {
        DEBUG_SUCCESS("alloc mem for markdown2html buffer succed.")
        DEBUG_WARN("alloc %d bytes in %s",strlen(ob->data) + 2,__func__)
        // printf("alloc mem succ\n");
    }
    else
    {
        free(content);
        bufrelease(ib);
        bufrelease(ob);

        DEBUG_WARN("alloc mem for markdown2html buffer faild in %s", __func__)

        return NULL;
    }
    len = sprintf(content, "%s", ob->data);
    content[len] = '\0';
    DEBUG_WARN("write %d bytes in %s",len,__func__)
    *length = len;
    bufrelease(ib);
    bufrelease(ob);

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
char *parse_articlesList_to_htmlBytesStream(const char *mkd_floder_path, int *length)
{
    mkd_files *mkds;
    mkd_file *mkd;
    char *content;
    int files_number = 0, files_number1;
    int filename_total_length = 0;
    int offset = 0;

    mkds = init_mkdroot();
    files_number = get_mkd_files_name(mkd_floder_path, mkds);
    mkd = mkds->head;
    files_number1 = files_number;
    if (files_number == 0)
    {
        *length = 14;
        content = (char *)malloc(sizeof(char) * 14);
        strcpy(content, "Not articles!");
        return content;
    }

    while (files_number--)
    {
        filename_total_length += strlen(mkd->filename_without_suffix);
        mkd = mkd->next;
    }
    mkd = mkds->head;
    content = (char *)malloc(sizeof(char) * (HTML_a_TAG_EXCEPT_LENGTH * files_number1 + filename_total_length * 2 + 10));
    while (mkd)
    {
        offset += sprintf(content + offset, "%s%s%s%s%s", HTML_a_TAG_BEGIN, mkd->filename_without_suffix, HTML_a_TAG_MIDDLE, mkd->filename_without_suffix, HTML_a_TAG_END);
        mkd = mkd->next;
    }
    content[offset] = '\0';
    *length += offset;

    DEBUG_NORMAL("offset=%d while alloc length=%d\n", offset, HTML_a_TAG_EXCEPT_LENGTH * files_number1 + filename_total_length * 2 + 10);
    free_mkds(mkds);
    return content;
}

// int create_static_html(const char *save_path, char *content, int length)
// {
//     FILE *fp;
//     content[length] = '\0';
//     fp = fopen(save_path, "w+");
//     if (fp == NULL)
//     {
//         printf("open file faild.\n");
//         return 1;
//     }
//     fputs(content, fp);
//     fclose(fp);
//     return 0;
// }
