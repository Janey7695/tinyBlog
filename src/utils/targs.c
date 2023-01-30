#include "utils.h"

enum{
    FLAG_C = 0,
    FLAG_H,
    FLAG_V,
    TOTAL_FLASG,
    FLAG_ERROR
};

const char *argFlagsList[][2] = {
    /*flag          description*/
    {"-c",          "-c <configure file path> : Specifies the path to the configuration file"},
    {"-h",          "-h : show help message"},
    {"-v",          "-v : show version information"}
};

void tagrs_print_version(){
    printf("current version is: %s .\r\n",VERSION);
}

void targs_print_flags_description(){
    int i = 0;
    for(i;i<TOTAL_FLASG;i++){
        printf("  %s  |  %s .\r\n",argFlagsList[i][0],argFlagsList[i][1]);
    }
}

void targs_print_usage(){
    tagrs_print_version();
    printf("Usage : tinyblog [-c <configure file path>] [-flag <flag args>] ...\r\n");
    printf("type \"tinyblog -h\" to show more help message. \r\n");
}

void targs_print_help(){
    tagrs_print_version();
    printf("Usage : tinyblog [-c <configure file path>] [-flag <flag args>] ...\r\n");
    printf("\r\n flags\t |\t description \r\n");
    targs_print_flags_description();

}

int targs_map_flag_to_number(char *flag){
    switch (flag[1])
    {
    case 'c':
        return FLAG_C;
    case 'h':
        return FLAG_H;
    case 'v':
        return FLAG_V;
    
    default:
        printf("unknown flag \"%s\"\r\n",flag);
        return FLAG_ERROR;
    }
}

int targs_deal_flag(char *flag,char *flag_arg){
    switch (targs_map_flag_to_number(flag))
    {
    case FLAG_C:
        configures *g_gloablConfigures = read_configure_json(flag_arg);
        print_configure(g_gloablConfigures);
        return 0;
    case FLAG_H:
        targs_print_help();
        exit(0);
    case FLAG_V:
        tagrs_print_version();
        exit(0);
    default:
        targs_print_usage();
        exit(0);
    }
}

int targs_deal_args(int argc, char **argv){
    if(argc < 2){
        targs_print_usage();
        exit(1);
    }

    int i = 1;
    for(i;i<argc;i++){
        if(argv[i][0] == '-'){
            targs_deal_flag(argv[i],argv[i+1]);
        }
    }
}
