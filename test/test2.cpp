/**
 * The MIT License
 *
 * Copyright 2022 Krishna sssky307@163.com
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#define USING_MINIOPT_ASSERT
#include "miniopt.h"
#include <iostream>

const char *argv[] = {"-a", "key1", "-akey2", "-a=key3=value", 
    "--append", "key1", "--append=key2", "--append=key3=value", "xxx",
    "-r", "123", "yyy", "--remove", "456", "zzz", "-v", "-vvv",
    "-vhv", "--debug", "-a", "123", "--", "-a", "456", "-v", "-vhv"
};
int argc = sizeof(argv)/sizeof(argv[0]);

int main() {
    option options[] = {
        {'a',  "append", "<file>  append file." , opt_has_arg}, // -a, --append
        {'r',  "remove", "<file>  remove file." , opt_has_arg}, // -r, --remove
        {'\0', "debug" , "        enable debug.", opt_no_arg},  //     --debug
        {'h',  "help"  , "        show help."   , opt_no_arg},  // -h, --help
        {'v',  ""      , "        show version.", opt_no_arg}   // -v
    };

    const int optsum = sizeof(options) / sizeof(options[0]);
    miniopt.init(argc, (char **)argv, options, optsum);

    int status;
    while ((status = miniopt.getopt()) > 0) {
        int id = miniopt.optind();
        switch (id) {
            case 0:    // -a, --append
            case 1:    // -r, --remove
            case 2:    //     --debug
            case 3:    // -h, --help
            case 4:    // -v
                printf("[option]\n  ");
                printf("optin-index = [%d] ", id);
                if(options[id].sname != '\0'){
                    printf("short-name = [%c]  ", options[id].sname);
                }
                if(options[id].lname[0] != '\0'){
                    printf("long-name = [%s]  ", options[id].lname);
                }
                if (miniopt.optarg()) {
                    printf("opt-has-arg = [%s]", miniopt.optarg());
                }
                printf("\n");
                break;
            default:
                printf("[non-option-arg]\n  arg = [%s]\n", miniopt.optarg());
                break;
        }
    }

    if(status < 0) printf("error: %s\n", miniopt.what());

    printf("\n[print-options]\n");
    miniopt.printopts(printf, 2);

    if(status == 0) printf("\n--test pass--\n");

    return status;
}