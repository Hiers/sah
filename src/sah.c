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
     * some actions (like strcat, for reasons that escape me)
     * will change the LANG environment variable which will change
     * the intended language of makepkg and break compilation on some
     * programs
     *
     * this is saved now so it can be reset later and makepkg can be called
     */
    config.lang = (char*)strdup((char*)getenv("LANG"));
    
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

    CURL *curl = (strncmp (config.aur_url, "https", strlen ("https")) == 0) ?
			curl_init (CURL_GLOBAL_SSL) : curl_init (CURL_GLOBAL_NOTHING);
	if (!curl) {
		return 0;
	}
    
    alpm_list_t *update = NULL, *install = NULL, *to_upgrade = NULL; 
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
                    default:
                        fatal("Unkown flag -%c", NULL, argv[1][n]);
                        
                }
                argv[1]++;
        }
    }
    else{
        fprintf(stderr, "usage: sah -S [-s] -[y/u] [package(s)]\n");
        exit(1);
    }
    
    //conflict between s and y/u flag
    if(config.op == OP_SEARCH && config.filter & F_SYNC)
        fatal("Search and sync/upgrade flags can not be used together", NULL);
    
    /*
     * if it has F_SYNC flag, upgrade
     * if it doesn't AND no package was passed as argument, exit
     * if it has OP_SEARCH flag, take arguments as search parameters
     * otherwise, take arguments as install requests
     */
    if(config.filter ^ F_SYNC && argc <= 2)
        fatal("No target defined", NULL);

    printf("\033[1m++ Fetching information from the AUR...\033[22m\n");
    if(config.filter & F_SYNC){
		alpm_search_local (F_FOREIGN | (config.filter & ~F_SYNC), "%n>%v", &targets);
        update = aur_request(&targets, AUR_INFO, curl);
        config.filter ^= F_SYNC;
    }
    
    if(config.op == OP_SEARCH){
        while(argc > 2){
            targets = alpm_list_add(targets, strdup(argv[2]));
            argv++;
            argc--;
        }
        aur_request(&targets, AUR_SEARCH, curl);
    }
    else{ 
        if(argc > 2){
            while(argc > 2){
                targets = alpm_list_add(targets, strdup(argv[2]));
                argv++;
                argc--;
            }
            install = aur_request(&targets, AUR_INSTALL, curl);
        }
    }

    if(update)
        to_upgrade = alpm_list_join(update, install);
    else
        to_upgrade = alpm_list_join(install, update);
    

    if(to_upgrade)
        aur_upgrade(to_upgrade, curl);
    else
        printf(" there is nothing to do.\n");

    free(config.lang);
    free(config.aur_url);
    free(config.configfile);
    alpm_list_free_inner (to_upgrade, (alpm_list_fn_free) aur_pkg_free);
	alpm_list_free (to_upgrade);
    
    curl_cleanup();
    git_libgit2_shutdown();
    return 0;
}
