#include <stdio.h>
#include <string.h>
#include <time.h>
#include <errno.h>
#include <git2.h>
#include "aur.h"
#include "util.h"
#include "alpm-query.h"

static alpm_list_t *targets = NULL;
aq_config config;

int main(int argc, char *argv[]){

    /* 
     * AUR url and pacman config file location defined at compile time
     * ~/.config/sah is hardcoded
     */
    config.format_out = getenv("HOME");
    strcat(config.format_out, "/.config/sah/");
    if((mkdir(config.format_out, 0755)) && errno != EEXIST)
        fatal("Could not create file in ~.config", NULL);

    config.aur_url = strdup(AUR_BASE_URL);
    config.configfile = strdup(CONFFILE);
    git_libgit2_init();
    
    /*satisfy package-query*/
    config.aur = 1;
    init_db_sync();
	config.colors = isatty(1);
    
    if(argc > 1 && argv[1][0] == '-' && argv[1][1] == 'S'){
        int n = 2;
        while(argv[1][n] != '\0'){
                switch(argv[1][n]){
                    case 's':
                        config.op = OP_SEARCH;
                        break;
                    /* 
                     * following pacman's style, -Sy should not be doing
                     * the same as -Su, but there is no database being kept and there is no
                     * harm in keeping it like this over -Sy doing nothing
                     */
                    case 'u':
                    case 'y':
                        config.filter |= F_SYNC;  
                        break;
                        
                }
                argv[1]++;
        }
    }
    else{
        fprintf(stderr, "usage:\n");
        exit(1);
    }
    
    //conflict between s and y/u flag
    if(config.op == OP_SEARCH && config.filter & F_SYNC){
        printf("error\n");
        return -1;
    }
    /*
     * if it has F_SYNC flag, upgrade
     * if it doesn't AND no package was passed as argument, exit
     * if it has OP_SEARCH flag, take arguments as search parameters
     * otherwise, take arguments as install requests
     */
    if(config.filter & F_SYNC){
        alpm_search_local(F_FOREIGN | (config.filter & ~F_SYNC), "%n>%v", &targets);
        aur_request(&targets, AUR_INFO);
        config.filter ^= F_SYNC;
    }
    else if(argc <= 2){
        printf("error\n");
        return -1;
    }
    if(config.op == OP_SEARCH){
        if(argc > 2){
            while(argc > 2){
                targets = alpm_list_add(targets, strdup(argv[2]));
                argv++;
                argc--;
            }
        }
        else{
            printf("error message");
        }
        aur_request(&targets, AUR_SEARCH);
    }
    else{ 
        if(argc > 2){
            while(argc > 2){
                targets = alpm_list_add(targets, strdup(argv[2]));
                argv++;
                argc--;
            }
            aur_request(&targets, AUR_INSTALL);
        }
    }
    
    free(config.aur_url);
    free(config.configfile);
    
    curl_cleanup();
    git_libgit2_shutdown();
    return 0;
}
