
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
static char *g_lib_name = "libtvhN3249.so";

static char *g_plugin_purpose = "Find type of file";

static char *g_plugin_author = "Tran Van Hoang - N3249";

#define OPT_exe "exe"
// #define OPT_lines_count "lines-count"
// #define OPT_lines_count_comp "lines-count-comp" 
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
            OPT_exe,
            required_argument,
            0, 0,
        },
        "File file with type elf, pe32, coff, a.out "
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

static char *input_type = NULL;


int plugin_process_file(const char *fname,
        struct option in_opts[],
        size_t in_opts_len) {

    int ret = -1;
    
    
    char *DEBUG = getenv("LAB1DEBUG");
    
    if (!fname || !in_opts || !in_opts_len) {
        errno = EINVAL;
        return -1;
    }
    // int got_lines_count = 0;
    // int got_lines_count_comp = 0;
    int got_input_type = 0;
    // int tmp_comp = 0;
    int tmp_type = 0;

//check value op (type input)
#define OPT_CHECK(opt_var, tmp_type) \
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
        tmp_type = strtol((char*)in_opts[i].flag, &endptr, 10); \
        if(tmp_type!=0){\
        	if (DEBUG) { \
                	fprintf(stderr, "DEBUG: %s: Failed to convert '%s'\n", \
                    	g_lib_name, (char*)in_opts[i].flag); \
            	} \
            	errno = EINVAL; \
            	return -1; \
        }\
        opt_var=endptr; \
        got_##opt_var = 1; \
    }
    
    for (size_t i = 0; i < in_opts_len; i++) {
        if (!strcmp(in_opts[i].name, OPT_exe)) {
            OPT_CHECK(input_type, tmp_type)
        }
        else {
            errno = EINVAL;
            return -1;
        }
    }
    // if(input_type!=NULL){     
    // 	if (DEBUG) {
    //        	 fprintf(stderr, "DEBUG: %s: input type value was not supplied.\n",
    //             	g_lib_name);
    //    	}
    //     	errno = EINVAL;
    //     	return -1;
   	// }
    // if (DEBUG) {
    //     fprintf(stderr, "DEBUG: %s: Inputs: Input type is %s\n", g_lib_name, input_type);
    // }
    FILE *fp;
    // Get file name from user. The file should be
    // either in current folder or complete path should be provided
    // Open the file
    fp = fopen(fname, "rb");
    static unsigned char magic[4];
    // Check if file exists
    if (fp == NULL) {
            fprintf(stderr, "Error : Failed to open entry file - %s\n", strerror(errno));
            return -1;
          
    }
    
    size_t k = fread(magic,1,4,fp);
    k++;
    fclose(fp);
    // Extract characters from file and store in character c
    int loop;
    int i; 
    
    i=0;
    loop=0;
    static char output[9];
    while(magic[loop] != '\0')
    {
        sprintf((char*)(output+i),"%02x", magic[loop]);
        
        loop+=1;
        i+=2;
    }
    //insert NULL at the end of the output string
    output[i++] = '\0';
    char c[100];
    strcpy(c,input_type);
    // fprintf(stderr,"Magic number of file: %s\n",output);
    // fprintf(stderr,"%s\n",c);
    if (strcmp(output,"7f454c46")==0 && strstr(c,"elf")!=NULL) {
            return ret = 0;
    }
    if (strstr(output,"ffd8ff")!=NULL && strstr(c,"jpeg")!= NULL) {
        return ret = 0;
    }
    if (strstr(output,"424d") != NULL && strstr(c,"bmp")!= NULL) {
        return ret = 0;
    }
    if (strstr(output,"47494638")!=NULL && strstr(c,"gif")!=NULL) {
        return ret = 0;
    } 
    if (strstr(output, "89504e47") != NULL && strstr(c,"png") != NULL) {
        return ret = 0;
    }
    else if (strstr(output,"4d5a")!=NULL && strstr(c,"pe32")!= NULL) {
            return ret = 0;
    }
    // else if (strstr(output,"4c01")> 0) {
    //     if (strstr(input_type, "coff")!=NULL)
    //         return ret = 0;
    //     else return ret = 1;
    // }
    else if ((strstr(output,"0410")!=NULL || strstr(output,"0413")!=NULL) && strstr(c,"a.out")!= NULL) {
            return ret = 0;
    }
    else return ret = 1;
    
}        

