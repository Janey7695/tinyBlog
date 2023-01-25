
#include "utils.h"

/// @brief 初始化markdown_files 的根节点
/// @return 返回mkd_files指针 若初始化失败则返回NULL
mkd_files *init_mkdroot()
{
    mkd_files *mkdroot = (mkd_files *)malloc(sizeof(struct markdown_files));
    if (mkdroot == NULL)
    {
        LOG_ERROR("alloc mem faild in %s",__LINE__)
        // printf("create mkdroot faild.\n");
        exit(1);
    }
    mkdroot->head = NULL;
    mkdroot->tail = NULL;
    mkdroot->total_mkd_file_number = 0;
    return mkdroot;
}

/// @brief 移除文件名后缀
/// @param strings 
/// @param stringLength 
/// @return 
char *remove_suffix(char *strings, int stringLength)
{
    if (strings == NULL)
        return NULL;
    char *temp = NULL;
    temp = (char *)malloc((stringLength + 1) * sizeof(char));
    int i = 0;
    while (strings[i] != '.')
    {
        temp[i] = strings[i];
        i++;
    }
    temp[i] = '\0';
    return temp;
}

/// @brief 创建一个新的mkdfile节点，并填充数据
/// @param filename 文件名称
/// @return 返回指向该节点的指针
mkd_file *create_new_mkd_file(const char *dirpath, const char *filename, int filenameLength)
{
    mkd_file *temp_mkd = NULL;
    temp_mkd = (mkd_file *)malloc(sizeof(struct markdown_file));
    char temp_path[256];
    FILE *fp;
    int fd;
    struct stat buf;

    if (temp_mkd == NULL)
    {
        LOG_WARN("alloc mem faild in %s",__LINE__)
        return temp_mkd;
    }

    temp_mkd->filename = (char *)malloc(sizeof(char) * (filenameLength + 1));
    strcpy(temp_mkd->filename, filename);
    temp_mkd->filename[filenameLength] = '\0';
    temp_mkd->filename_without_suffix = remove_suffix(filename, filenameLength);
    sprintf(temp_path, "%s/%s", dirpath, filename);

    fp = fopen(temp_path, "r");
    if (fp != NULL)
    {
        fd = fileno(fp);
        fstat(fd, &buf);
        int size = buf.st_size;
        temp_mkd->modify_time = buf.st_mtime;
        fclose(fp);
    }
    else
    {
        LOG_WARN("open file to get time faild.",__LINE__)
        // printf("open file to get time faild.\n");
        temp_mkd->modify_time = 0;
    }

    temp_mkd->next = NULL;
    return temp_mkd;
}

/// @brief 添加节点到mkd链表上
/// @param mkdroot 根节点
/// @param newnode 要添加的节点
/// @return 返回添加新节点后的链表长度
int addMkd_to_mkdroot(mkd_files *mkdroot, mkd_file *newnode)
{
    if (mkdroot->head == NULL && mkdroot->tail == NULL)
    {
        mkdroot->head = newnode;
        mkdroot->tail = newnode;
        mkdroot->total_mkd_file_number += 1;
        return mkdroot->total_mkd_file_number;
    }

    mkdroot->tail->next = newnode;
    mkdroot->tail = newnode;
    mkdroot->total_mkd_file_number += 1;
    return mkdroot->total_mkd_file_number;
}

/// @brief 释放mkdroot申请的内存
/// @param mkdroot mkd根节点
void free_mkds(mkd_files *mkdroot)
{
    if (mkdroot == NULL)
        return;
    if (mkdroot->head == NULL && mkdroot->tail == NULL)
    {
        free(mkdroot);
        return;
    }
    mkd_file *t1 = NULL;
    mkd_file *t2;
    t1 = mkdroot->head;
    if (t1->next != NULL)
        t2 = mkdroot->head->next;
    else
        t2 = NULL;
    while (t1)
    {
        free(t1);
        t1 = t2;
        if (t2 != NULL)
            t2 = t2->next;
    }
    free(mkdroot);
}

/// @brief 在当前目录下创建一个新文件夹
/// @param dirpath 目录名称
/// @return -1失败 0成功
int create_dir(const char *dirpath)
{
    if (access(dirpath, NULL) != 0)
    {
        if (mkdir(dirpath, 0755) == -1)
        {
            LOG_ERROR("mkdir %s error!",dirpath)
            // printf("mkdir %s error!\n", dirpath);
            return -1;
        }
    }
    LOG_SUCCESS("mkdir %s successed!")
    // printf("create dir\n");
    return 0;
}

/// @brief 获取目标文件夹下的markdown文件数量
/// @param dirpath 目标文件夹名称
/// @param mkds mkd根节点
/// @return 读取到的文件总数量
int get_mkd_files_name(const char *dirpath, mkd_files *mkds)
{
    DIR *dir;
    struct dirent *ptr;
    int total_file_number = 0;

    if ((dir = opendir(dirpath)) == NULL)
    {
        LOG_WARN("target markdown file dir don't exist\n try to create it.")
        // printf("target markdown file dir don't exist\n try to create it.\n");
        if (create_dir(dirpath) != 0)
        {
            exit(1);
        }
        else
        {
            dir = opendir(dirpath);
        }
    }
    perror("opendir : ");
    while ((ptr = readdir(dir)) != NULL)
    {
        if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0) /// current dir OR parrent dir
        {
            continue;
        }

        else if (ptr->d_type == 8) /// file
        {
            total_file_number++;
            addMkd_to_mkdroot(mkds, create_new_mkd_file(dirpath, ptr->d_name, ptr->d_reclen));
        }

        else if (ptr->d_type == 10) /// link file
        {
            total_file_number++;
            addMkd_to_mkdroot(mkds, create_new_mkd_file(dirpath, ptr->d_name, ptr->d_reclen));
        }
        else if (ptr->d_type == 4) /// dir
        {
            // memset(base, '\0', sizeof(base));
            // strcpy(base, dirpath);
            // strcat(base, "/");
            // strcat(base, ptr->d_name);
            // readFileList(base);
            continue;
        }
    }
    closedir(dir);
    return total_file_number;
}

/// @brief 初始化configure
/// @param configure 
void init_configure(configures* configure){
    configure->markdown_floder = "articles";
    configure->port = 8000;
}

int get_configure_mkdpath(configures* configure,cJSON* configure_json){
    cJSON* pj;
    pj = cJSON_GetObjectItemCaseSensitive(configure_json,"mkd_path");
    if(pj == NULL){
        LOG_WARN("don't find configure %s in the json file.\n","mkd_path")
        // printf("don't find configure %s in the json file.\n","mkd_path");
        return 1;
    }
    int path_length = 0;
    path_length = strlen(cJSON_GetStringValue(pj));
    configure->markdown_floder = (char*)malloc(sizeof(char)* (path_length+1));
    strcpy(configure->markdown_floder,cJSON_GetStringValue(pj));
    return 0;
}

int get_configure_port(configures* configure,cJSON* configure_json){
    cJSON* pj;
    pj = cJSON_GetObjectItemCaseSensitive(configure_json,"port");
    if(pj == NULL){
        LOG_WARN("don't find configure %s in the json file.\n","port")

        return 1;
    }
    configure->port = (int)cJSON_GetNumberValue(pj);
    return 0;
}

int get_configure(configures* configure,cJSON* configure_json){
    get_configure_mkdpath(configure,configure_json);
    get_configure_port(configure,configure_json);
}

/// @brief 读取配置文件
/// @param config_file_path  配置文件所在位置
/// @return 配置结构体的指针
configures *read_configure_json(const char *config_file_path)
{
    configures *temp_configure;
    cJSON *temp_json;
    cJSON *obj_json;
    char *json_string;
    FILE *fp;

    fp = fopen(config_file_path, "r");
    if (fp == NULL)
    {
        LOG_ERROR("can't open configure file %s\n", config_file_path)
        // printf("can't open configure file %s\n", config_file_path);
        perror("open configure file : ");
        exit(1);
    }

    fseek(fp, 0, SEEK_END);
    int filesize = 0;
    filesize = ftell(fp);
    json_string = (char *)malloc(sizeof(char) * (filesize + 1));

    rewind(fp);
    fread(json_string, filesize, sizeof(char), fp);
    json_string[filesize] = '\0';
    fclose(fp);
    LOG_NORMAL("read configure file : %s\n",json_string)
    // printf("read configure file : %s\n",json_string);

    temp_json = cJSON_Parse(json_string);
    if (temp_json == NULL)
    {
        const char *error_ptr = cJSON_GetErrorPtr();
        if (error_ptr != NULL)
        {
            fprintf(stderr, "Error before: %s\n", error_ptr);
        }
        free(json_string);
        exit(1);
    }
    free(json_string);

    temp_configure = malloc(sizeof(configures));
    if(temp_configure == NULL){
        LOG_WARN("configures alloc faild.\n")
        //printf("configures alloc faild.\n");
        cJSON_Delete(temp_json);
        exit(1);
    }

    init_configure(temp_configure);
    get_configure(temp_configure,temp_json);

    cJSON_Delete(temp_json);

    return temp_configure;
}

void print_configure(configures *configure){
    LOG_NORMAL("Port : %d \r\n markdown file store path : %s \r\n",configure->port,configure->markdown_floder)
    //printf("Port : %d \r\n markdown file store path : %s \r\n",configure->port,configure->markdown_floder);
}

int hex2dec(char c)
{
    if ('0' <= c && c <= '9')
    {
        return c - '0';
    }
    else if ('a' <= c && c <= 'f')
    {
        return c - 'a' + 10;
    }
    else if ('A' <= c && c <= 'F')
    {
        return c - 'A' + 10;
    }
    else
    {
        return -1;
    }
}

void urldecode(char url[])
{
    int i = 0;
    int len = strlen(url);
    int res_len = 0;
    char res[2048];
    for (i = 0; i < len; ++i)
    {
        char c = url[i];
        if (c != '%')
        {
            res[res_len++] = c;
        }
        else
        {
            char c1 = url[++i];
            char c0 = url[++i];
            int num = 0;
            num = hex2dec(c1) * 16 + hex2dec(c0);
            res[res_len++] = num;
        }
    }
    res[res_len] = '\0';
    strcpy(url, res);
}

void get_current_timestamp(char* tss){
    time_t timep;
	time(&timep);
	struct tm *p;
	p = gmtime(&timep);
    sprintf(tss,"%d/%d/%d-%d:%d:%d",p->tm_year - 100,1+p->tm_mon,p->tm_mday,8+p->tm_hour,p->tm_min,p->tm_sec);
}