
#include "utils.h"
#include <stddef.h>


/// @brief 移除文件名后缀
/// @param filename 
/// @param stringLength 
/// @return 
char *remove_suffix(char *filename, int stringLength)
{
    if (filename == NULL)
        return NULL;
    char *filenameWithoutSuffix = NULL;
    filenameWithoutSuffix = (char *)malloc((stringLength + 1) * sizeof(char));
    int i = 0;
    while (filename[i] != '.')
    {
        filenameWithoutSuffix[i] = filename[i];
        i++;
    }
    filenameWithoutSuffix[i] = '\0';
    return filenameWithoutSuffix;
}


char *get_suffix(char *filename)
{
	if (filename == NULL) {
		return NULL;
	}
	char *suffix = NULL;
	int filenameLength = strlen(filename);
	int i = 0;
	for (i=filenameLength-1; i>=0; i--) {
		if (filename[i]!='.') {
			continue;
		}	
		suffix = TMALLOC(char, filenameLength - i + 1 +1);
		strcpy(suffix, filename+i+1);
		suffix[filenameLength -i +1+1]='\0';	
		DEBUG_NORMAL("file %s suffix is %s",filename,suffix)
		return suffix;
	}
	DEBUG_NORMAL("don't find suffix for file %s in %s",filename,__func__);
	return NULL;
}

char *find_next_char(char *s1,char tag)
{
	char *s1p = s1;
	while(*s1p == tag && *s1p != '\0'){
			s1p++;
	}
	return s1p;
}

char *slice_with_char(char *s1,char tag,int sliceLength){
	char *fragment = NULL;	
	char *s_temp = s1;	
	int fragmentLength = 0;
	if (*s1 == tag) {
		s_temp = find_next_char(fragment, tag);
	}
	while(s_temp[fragmentLength] != tag && s_temp[fragmentLength] != '\0'){
		fragmentLength++;
	}
	fragment = TMALLOC(char, fragmentLength);
	fragment[fragmentLength] = '\0';
	for (--fragmentLength; fragmentLength>=0;fragmentLength-- ) {
		fragment[fragmentLength] = s_temp[fragmentLength];
	}
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
            return -1;
        }
    }
    LOG_SUCCESS("create new floder %s .",dirpath)
    return 0;
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
