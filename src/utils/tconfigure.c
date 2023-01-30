#include "utils.h"
configures globalConfigure;

const char *configure_items[] = {
    "port",
    "mkd_path",
    "threads",
    "theme"
};

/// @brief 初始化configure
/// @param configure 
void init_configure(configures* configure){
    configure->items[CONFIGURE_MKDPATH] = "articles";
    configure->items[CONFIGURE_PORT] = "8000";
    configure->items[CONFIGURE_THREADS] = "0";
    configure->items[CONFIGURE_THEME] = "default";
}

int get_configre_x(configures* configure,cJSON* configure_json,int whichConfigure){
    cJSON* pj;
    pj = cJSON_GetObjectItemCaseSensitive(configure_json,configure_items[whichConfigure]);
    if(pj == NULL){
        LOG_WARN("don't find configure %s in the json file.",configure_items[whichConfigure])
        return 1;
    }
    int string_length = 0;

    string_length = strlen(cJSON_GetStringValue(pj));
    char *temp  = (char*)malloc(sizeof(char)* (string_length+1));
    strcpy(temp,cJSON_GetStringValue(pj));
    configure->items[whichConfigure] = temp;

    LOG_SUCCESS("read configure %s succeed.",configure_items[whichConfigure])
    return 0;
}

int get_configure(configures* configure,cJSON* configure_json){
    int i = 0;
    int total_get = 0;
    for(i;i<TOTAL_CONFIGURES;i++){
        total_get += !(get_configre_x(configure,configure_json,i));
    }
    return total_get;
}

/// @brief 读取配置文件
/// @param config_file_path  配置文件所在位置
/// @return 配置结构体的指针
configures *read_configure_json(const char *config_file_path)
{
    configures *pconfigures = NULL;
    cJSON *configures_json;
    cJSON *obj_json;
    char *json_string;
    FILE *fp;

    fp = fopen(config_file_path, "r");
    if (fp == NULL)
    {
        LOG_ERROR("can't open configure file %s\n", config_file_path)
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

    configures_json = cJSON_Parse(json_string);
    if (configures_json == NULL)
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

    init_configure(&globalConfigure);
    int total_get = get_configure(&globalConfigure,configures_json);
    LOG_NORMAL("get %d of %d configures.",total_get,TOTAL_CONFIGURES)
    pconfigures = &globalConfigure;
    cJSON_Delete(configures_json);

    return pconfigures;
}

configures *get_configures_point(){
    return &globalConfigure;
}

/// @brief 输出读取的配置信息
/// @param configure 
void print_configure(configures *configure){
    int i = 0;
    for(i;i<TOTAL_CONFIGURES;i++){
        LOG_NORMAL("%s : %s",configure_items[i],configure->items[i])
    }
}