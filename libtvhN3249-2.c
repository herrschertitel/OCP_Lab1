
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>
#include "plugin_api.h"
static char *g_lib_name = "libtvhN3249-2.so";

static char *g_plugin_purpose = "Find all file with size smaller than input size";

static char *g_plugin_author = "Tran Van Hoang - N3249";

// #define OPT_file_txt "txt"
#define OPT_size "size"

static struct plugin_option g_po_arr[] = {
/*
    struct plugin_option {
        struct option {
           const char *name;
           int         has_arg;
           int        *flag;
           int         val;
        } opt,
        char *opt_descr
    }
*/
    {
        {
            OPT_size,
            required_argument,
            0, 0,
        },
        "Find file with size smaller than input size"
    },   
};

static int g_po_arr_len = sizeof(g_po_arr)/sizeof(g_po_arr[0]);

int plugin_get_info(struct plugin_info* ppi) {
    if (!ppi) {
        fprintf(stderr, "ERROR: invalid argument\n");
        return -1;
    }
    
    ppi->plugin_purpose = g_plugin_purpose;
    ppi->plugin_author = g_plugin_author;
    ppi->sup_opts_len = g_po_arr_len;
    ppi->sup_opts = g_po_arr;
    
    return 0;
}
static int input_size = 0;

int plugin_process_file(const char *fname,
        struct option in_opts[],
        size_t in_opts_len) {
    int ret = -1;

    char *DEBUG = getenv("LAB1DEBUG");

    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }
   /* if (DEBUG) {
        for (size_t i = 0; i < in_opts_len; i++) {
            fprintf(stderr, "DEBUG: %s: Got option '%s' with arg '%s'\n",
                g_lib_name, in_opts[i].name, (char*)in_opts[i].flag);
        }
    }*/
    g_lib_name="libtvhN3249-2.so";
    // if (strcmp(in_opts[0].name, OPT_file_txt)!=0) {   
    //     errno = EINVAL; 
    //     return -1; 
    // }
    int got_input_size = 0;

    #define OPT_CHECK(opt_var) \
    if (got_##opt_var) { \
        if (DEBUG) { \
            fprintf(stderr, "DEBUG: %s: Option '%s' was already supplied\n", \
                g_lib_name, in_opts[i].name); \
        } \
        errno = EINVAL; \
        return -1; \
    } \
    else { \
        char *endptr = NULL; \
        opt_var = strtol((char*)in_opts[i].flag, &endptr, 10); \
        if (strcmp(endptr,"")!=0) { \
            	if (DEBUG) { \
                	fprintf(stderr, "DEBUG: %s: Failed to convert '%s'\n", \
                  	g_lib_name, (char*)in_opts[i].flag); \
            	} \
            	errno = EINVAL; \
            	return -1; \
       } \
        got_##opt_var = 1; \
    }

    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_size)) {
            OPT_CHECK(input_size)
        }
	else {
            errno = EINVAL;
            return -1;
        }
    }
    
    if (!got_input_size) {
        if (DEBUG) {
            fprintf(stderr, "DEBUG: %s: The input size value was not supplied.\n",
                g_lib_name);
        }
        errno = EINVAL;
        return -1;
    }

    // if (DEBUG) {
    //     fprintf(stderr, "DEBUG: %s: Inputs: Size file must smaller than %d bytes\n", g_lib_name, input_size);
    // }

    FILE *fp;
    fp = fopen(fname,  "rb");
    if (fp == NULL) {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            return -1;
          
    }
    fseek(fp, 0L, SEEK_END);
    int res = ftell(fp);
    fclose(fp);
    if (res < input_size) 
        return ret = 0;
    else return ret = 1;

    // char dirname_tmp[500];
    // strcpy(dirname_tmp,dirname);
       
    // int len_file=strlen(dirname_tmp);
    // if( (dirname_tmp[len_file-1]==116) && (dirname_tmp[len_file-2]==120) && (dirname_tmp[len_file-3]==116) && (dirname_tmp[len_file-4]==46) ){
    // 	return ret=0;
    // }
    // else{
    // 	return ret=1;
    // }
  
}


