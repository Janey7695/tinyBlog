
#include "utils.h"

/// @brief 初始化markdown_files 的根节点
/// @return 返回mkd_files指针 若初始化失败则返回NULL
mkd_files *init_mkdroot()
{
    mkd_files *mkdroot = (mkd_files*)malloc(sizeof(struct markdown_files));
    if (mkdroot == NULL)
    {
        printf("create mkdroot faild.\n");
        exit(1);
    }
    mkdroot->head = NULL;
    mkdroot->tail = NULL;
    mkdroot->total_mkd_file_number = 0;
    return mkdroot;
}

char *remove_suffix(char *strings,int stringLength)
{
    if (strings == NULL)
        return NULL;
    char *temp = NULL;
    temp = (char*)malloc((stringLength + 1) * sizeof(char));
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
mkd_file *create_new_mkd_file(const char *dirpath, const char *filename,int filenameLength)
{
    mkd_file *temp_mkd = NULL;
    temp_mkd = (mkd_file*)malloc(sizeof(struct markdown_file));
    char temp_path[256];
    FILE *fp;
    int fd;
    struct stat buf;

    if (temp_mkd == NULL)
    {
        printf("create mkd node faild.\n");
        return temp_mkd;
    }

    temp_mkd->filename = (char*)malloc(sizeof(char) * (filenameLength + 1));
    strcpy(temp_mkd->filename, filename);
    temp_mkd->filename[filenameLength] = '\0';
    temp_mkd->filename_without_suffix = remove_suffix(filename,filenameLength);
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
        printf("open file to get time faild.\n");
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
            printf("mkdir %s error!\n", dirpath);
            return -1;
        }
    }
    printf("create dir\n");
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
        printf("target markdown file dir don't exist\n try to create it.\n");
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
            addMkd_to_mkdroot(mkds, create_new_mkd_file(dirpath, ptr->d_name,ptr->d_reclen));
        }

        else if (ptr->d_type == 10) /// link file
        {
            total_file_number++;
            addMkd_to_mkdroot(mkds, create_new_mkd_file(dirpath, ptr->d_name,ptr->d_reclen));
        }
        else if (ptr->d_type == 4) /// dir
        {
            // memset(base, '\0', sizeof(base));
            // strcpy(base, dirpath);
            // strcat(base, "/");
            // strcat(base, ptr->d_name);
            // readFileList(base);
            printf("seems 3\n");
            continue;
        }
    }
    closedir(dir);
    return total_file_number;
}
