
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <dirent.h>
#include "plugin_api.h"

typedef int (*ppf_func_t)(const char*, struct option*, size_t); //for plugin_process_file
typedef int (*pgi_func_t)(struct plugin_info*); // for plugin_get_info;

struct longopt {
    struct option *all_opt ; // все опции любого плагина
    size_t all_opt_len ; // количество опции любого
    struct option *opts_to_pass ; // опции которые удовлетроряют условия любого плагина
    size_t opts_to_pass_len ; // количество опции которые удовлетроряют условия любого плагина
    ppf_func_t func;
    pgi_func_t info;
};

int count_so (const char* dirname, int* len) { // найти и считать количество все файлы  .so
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        fprintf(stderr, " ERROR: No directory %s\n", dirname);
        return -1;
    }
    struct dirent* entity;
    entity = readdir(dir);
    *len = 0;
    while (entity != NULL) {
        int flen = strlen(entity->d_name);
        if ((entity->d_type == DT_REG) && (entity->d_name[flen-1] == 'o') && (entity->d_name[flen-2] == 's') && (entity->d_name[flen-3] == '.')) {
            (*len)++;
            fprintf(stdout,"lib_name: %s\n",entity->d_name);
        }
        entity = readdir(dir);
    }
    closedir(dir);
    return 0;
}

int option_p (const char* dirname, void* dl[], int len) { // подлючиться к всем файлам .so
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        fprintf(stderr, " ERROR: No directory %s\n", dirname);
        return -1;
    }
    struct dirent* entity;
    entity = readdir(dir);
    int index = 0;
    while (entity != NULL && index < len) {
        int flen = strlen(entity->d_name);
        if ((entity->d_type == DT_REG) && (flen > 3) && (entity->d_name[flen-1] == 'o') && (entity->d_name[flen-2] == 's') && (entity->d_name[flen-3] == '.')) {
            size_t file_name_len = strlen(dirname) + strlen(entity->d_name) + 2 ;
            char* file_name = malloc(file_name_len);
            sprintf(file_name, "%s/%s", dirname, entity->d_name);
            dl[index] = dlopen(file_name, RTLD_LAZY);
            if (dl[index] == NULL) {
                fprintf(stderr, "ERROR: Failed to dlopen %s\n%s\n", entity->d_name, dlerror());
                return -1;
            }
            else {
                index++;
            }
            free(file_name);

        }
        entity = readdir(dir);
    }
    closedir(dir);
    return 0;
}

//fun file recursive search;
int res_file (const char* dirname, int tlen, struct longopt sup_all[], int is_or, int is_not) {
    DIR* dir = opendir(dirname);
    if (dir == NULL) {
        fprintf(stderr, " ERROR: No directory %s\n", dirname);
        return -1;
    }
    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL) {
    
        if(strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0){
             // printf("lol : %s\n",entity->d_name);
            size_t path_len = strlen(dirname) + strlen(entity->d_name) + 2;
            char* path = malloc(path_len);
            snprintf(path, path_len, "%s/%s", dirname, entity->d_name);
	    // если это  entity -> каталог
            if(entity->d_type == DT_DIR){ 
                int res = res_file(path, tlen, sup_all, is_or, is_not);
                if (res){
                    free(path);
                    return -1;
                }
            }
	    // если это entity -> файл 
            if(entity->d_type == DT_REG) {
                int ret_true = 0; // if plugin retrun true ret++;
                int plugins_call = 0; //_count the number of plugins called
                // для каждого файла мы передадим все динамические библиотеки
                for (int i=0; i < tlen; i++){
                    if(sup_all[i].opts_to_pass_len > 0) {
                        plugins_call++ ;
                        int ret_fun = sup_all[i].func(path, sup_all[i].opts_to_pass, sup_all[i].opts_to_pass_len);
                     //   fprintf(stdout, "%d %d \n", i , ret_fun);
                        if (ret_fun == 0) ret_true++;
                        if (ret_fun < 0){
                            free(path);
                            errno = 0;
                            fprintf(stdout, "Error information: %s\n", strerror(errno));
                            return -1;
                        }
                    }
                }
                
                if(plugins_call){
                    // short_opt A and no opt; 
                    if ( ret_true == plugins_call && is_or == 0 && is_not == 0) fprintf(stdout, "The file meets the requirements: %s\n", entity->d_name);
                    //short_opt O;
                    else if (ret_true > 0 && is_or == 1 && is_not == 0) fprintf(stdout, "The file meets the requirements: %s\n", entity->d_name);
                    //short_opt NA;
                    else if (ret_true < plugins_call && is_or ==0 && is_not ==1) fprintf(stdout, "The file meets the requirements: %s\n", entity->d_name);
                    //short_opt NO;
                    else if (ret_true == 0 && is_or == 1 && is_not == 1) fprintf(stdout, "The file meets the requirements: %s\n", entity->d_name);

                }

                
            }

            free(path);
        }  
        entity = readdir(dir);
    }

    closedir(dir);
    return 0;
}

int main(int argc, char *argv[]) {

    struct longopt *sup_all = 0;
    char *f_name = 0;
    opterr = 0;
 
    // Minimum number of arguments is 2: 
    // $ program_name --opts file to ch
    if (argc < 2) {        
        fprintf(stdout, "Usage: ./main -short_opt --[options_for_lib] /path/to/file\n");
        fprintf(stdout, "Short_options:\n");
                fprintf(stdout, "\t\t-P: Plugin directory\n");
                fprintf(stdout, "\t\t-A: Combine plugin options using the 'AND' operation\n");
                fprintf(stdout, "\t\t-O: Combine plugin options using the 'OR' operation.\n");
                fprintf(stdout, "\t\t-N: Inverting the search term (after combining options plugins with -A or -O)\n");
                fprintf(stdout, "\t\t-v: Displaying performer's full name, group number,lab version number o\n");
                fprintf(stdout, "\t\t-h: Display help for options.\n");
        fprintf(stdout, "Long_options in plugin 1:\n");
                fprintf(stdout,"\t\t --mac-addr\t\t Target id of mac and check mac-addr\n");
        return 0;
    }
    
    int len = 0;
   // if (count_so(".", &len)) {
    //    fprintf (stderr, "ERROR: unable to count in curren dir\n");
     //   return -1;
    //}
     
    void** dl = 0;
   // if (option_p(".", dl, len)){
    //    fprintf (stderr,"ERROR: unable to dlopen libs in a current dir\n");
    //}

    char **new_argv = (char**) malloc (argc * sizeof(char*));
    if(!new_argv){
        fprintf(stdout,"ERROR: could not allocate for argv copy\n");
    }

    int  is_o = 0, is_n = 0 , is_v = 0, is_h = 0 ,is_P =0;
    // short_option A is_a = 1 (if is_o == 0 and is_n == 0)


    memcpy(new_argv, argv, argc * sizeof(char*));
    
    int ret_shrt = 0;
    while((ret_shrt = getopt(argc,new_argv, "P:vhAON"))!=-1)
           
    {
        switch(ret_shrt){

            case 'P':
                fprintf(stdout, "Short option -%c is detected!\n", ret_shrt);
                is_P = 1;
                if (count_so(optarg, &len)){
                    fprintf(stderr, "ERROR: unable to count file.so\n");
                    goto END;
                }

                dl = calloc (len, sizeof(void*));
                if (option_p(optarg, dl, len)){
                    fprintf(stderr, "ERROR: unable to open file.so\n");
                    goto END;
                }

                if( optind == argc ) {
                    is_h = 1;
                    goto START;
                }
                if(argv[optind][1] ==  '-') goto START;
               
                break;
            case 'v':
                fprintf(stdout, "Short option -%c is detected!\n", ret_shrt);
                is_v = 1 ;
                if( optind == argc ) goto START;
                if( argv[optind][1] ==  '-' ) goto START;
                break;
            case 'h':
                fprintf(stdout, "Short option -%c is detected!\n", ret_shrt);
                is_h = 1;
                if( optind == argc ) goto START;
                if( argv[optind][1] ==  '-' ) goto START;
                break;
            case 'A':
                fprintf(stdout, "Short option -%c is detected!\n", ret_shrt);

                if(argv[optind][1] ==  '-') goto START;
                break;
            case 'O':
                fprintf(stdout, "Short option -%c is detected!\n", ret_shrt);
                is_o = 1;
                if(argv[optind][1] ==  '-') goto START;
                break;
            case 'N':
                fprintf(stdout, "Short option -%c is detected!\n", ret_shrt);
                is_n = 1;
                if(argv[optind][1] ==  '-') goto START;
                break;
            default:
                fprintf(stdout, "No short options\n");
                goto START;          
        }
    
    

    }
    

    START: ;

    if(!is_P) {
        if (count_so(".", &len)) {
            fprintf (stderr, "ERROR: unable to count in curren dir\n");
            goto END;
        }

        dl = calloc (len, sizeof(void*));
        if (option_p(".", dl, len)){
            fprintf (stderr,"ERROR: unable to dlopen libs in a current dir\n");
            goto END;
        }
    }
    
    sup_all = calloc (len, sizeof(struct longopt));
	


    for (int i = 0; i < len; i++){

        // Check for plugin_get_info()
        sup_all[i].info = dlsym(dl[i], "plugin_get_info");
        if(!sup_all[i].info){
            fprintf(stderr, "ERROR: dlsym() failed: %s\n", dlerror());
            goto END;
        }
        struct plugin_info pi = {0};
        if (sup_all[i].info(&pi) < 0){
            fprintf(stderr, "ERROR: plugin_get_info() failed\n");
            goto END;
        }
        if (pi.sup_opts_len == 0){
            fprintf(stderr, "ERROR: library supports no options! How so?\n");
            goto END;
        }
         
        // Plugin info and printf short option v , h;

        if(is_v == 1){
            fprintf(stdout, "Plugin purpose:\t\t%s\n", pi.plugin_purpose);
            fprintf(stdout, "Plugin author:\t\t%s\n", pi.plugin_author);
            if (i == len -1)goto END;
        }

        if(is_h == 1){
            fprintf(stdout, "Supported options: ");
            if (pi.sup_opts_len > 0) {
                fprintf(stdout, "\n");
                for (size_t j = 0; j < pi.sup_opts_len; j++) {
                    fprintf(stdout, "\t--%s\t\t%s\n", pi.sup_opts[j].opt.name, pi.sup_opts[j].opt_descr);
                    }
                }
            else{
                fprintf(stdout, "none (!?)\n");
            }
            fprintf(stdout, "\n");
            if (i == len - 1) goto END;
        }

        // Get pointer to plugin_process_file()

        sup_all[i].func = dlsym(dl[i], "plugin_process_file");
        if(!sup_all[i].func) {
            fprintf(stderr, "ERROR: no plugin_process_file() function found\n");
            goto END;
        }

        // Prepare array of options for getopt_long

        sup_all[i].all_opt_len = pi.sup_opts_len;
        sup_all[i].all_opt = calloc(pi.sup_opts_len + 1, sizeof(struct option));

        if (!sup_all[i].all_opt){
            fprintf(stderr, "ERROR: calloc() failed:%s\n", strerror(errno));
            goto END;
        }

        // copy option information

        for (size_t j = 0; j < pi.sup_opts_len; j++) {
            memcpy(&sup_all[i].all_opt[j], &pi.sup_opts[j].opt, sizeof(struct option));
        }

        // Prepare array of actually used options that will be passed to 
        // plugin_process_file() (Maximum pi.sup_opts_len options)

        sup_all[i].opts_to_pass_len = 0;
        sup_all[i].opts_to_pass = calloc(pi.sup_opts_len, sizeof(struct option));
        if(!sup_all[i].opts_to_pass) {
            fprintf(stderr, "ERROR: calloc() failed: %s\n", strerror(errno));
            goto END;
        }
    }


    // Now process options for the lib

    for (int i = 0; i < len; i++) {
        optind = 1;
        memcpy(new_argv, argv, argc * sizeof(char*));
        while (1){
            int opt_ind = 0;
            int ret = getopt_long_only(argc, new_argv, " ", sup_all[i].all_opt, &opt_ind);
            if (ret == -1) break;

            if(ret != 0) continue;

            // Check how many options we got up to this moment
            if ((size_t) sup_all[i].opts_to_pass_len == sup_all[i].all_opt_len){
                fprintf(stderr, "ERROR: too many options!\n");
                goto END;
            }

            // Add this option to array of options actually passed to plugin_process_file()
            memcpy(sup_all[i].opts_to_pass + sup_all[i].opts_to_pass_len, sup_all[i].all_opt + opt_ind, sizeof(struct option));

            // Argument (if any) is passed in flag
            if ((sup_all[i].all_opt + opt_ind)->has_arg) {
                // Mind this!
                // flag is of type int*, but we are passing char* here (it's ok to do so). 
                (sup_all[i].opts_to_pass + sup_all[i].opts_to_pass_len)->flag = (int*)strdup(optarg);
            }

            sup_all[i].opts_to_pass_len++;
        }

    }

    if (getenv("LAB1DEBUG")) {
        for (int i = 0; i < len; i++){
            fprintf(stderr, "DEBUG: opts_to_pass_len = %ld\n", sup_all[i].opts_to_pass_len);
            for (size_t j = 0; j < sup_all[i].opts_to_pass_len; j++) {
            fprintf(stderr, "DEBUG: passing option '%s' with arg '%s'\n",
                (sup_all[i].opts_to_pass[j]).name,
                (char*)(sup_all[i].opts_to_pass[j]).flag);
            }
        }
    }

    fprintf(stdout, "The options are passed to libs!!! \n");
    fprintf(stdout, "------------------------------------- \n");

    // Call fun recursive search and plugin_process_file()


    errno = 0;
    f_name = strdup(argv[argc-1]);
    int ret_main = res_file(f_name, len, sup_all, is_o, is_n);
    fprintf (stdout, "------------------------------------- \n");
    fprintf (stdout, "fun res_file() returned %d\n",ret_main);
    if(ret_main < 0){
        fprintf(stderr, "error infomation: %s\n", strerror(errno));
    }

    END:
    if (sup_all){
        for (int i = 0; i < len; i++){
            for(size_t j = 0; j < sup_all[i].opts_to_pass_len; j++){
                if (sup_all[i].opts_to_pass[j].flag) free(sup_all[i].opts_to_pass[j].flag);
            }
            if (sup_all[i].opts_to_pass) free(sup_all[i].opts_to_pass);
            if (sup_all[i].all_opt) free(sup_all[i].all_opt);
        }
        free(sup_all);
    }

    if(new_argv) free(new_argv);
    if(f_name) free(f_name);
    if (dl){
        for (int i = 0; i < len; i++){
            if(dl[i]) dlclose(dl[i]);
        }
        free (dl);
    }

    return 0;
}

        



   

