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

const char *argv[] = {"/path/to/myapp.exe",
                      "-a",
                      "key1",
                      "-akey2",
                      "-a=key3=value",
                      "--append",
                      "key1",
                      "--append=key2",
                      "--append=key3=value",
                      "xxx",
                      "-r",
                      "123",
                      "yyy",
                      "--remove",
                      "456",
                      "zzz",
                      "-v",
                      "-vvv",
                      "-vhv",
                      "--debug",
                      "-a",
                      "123",
                      "--",
                      "-a",
                      "456",
                      "-v",
                      "-vhv"};
int argc = sizeof(argv) / sizeof(argv[0]);

int main() {
    option options[] = {
        {'a', "append", "<file>", "append file."},    // -a, --append
        {'r', "remove", "<file>", "remove file."},    // -r, --remove
        {'h', "help", nil, "show help."},             // -h, --help
        {nil, "debug", nil, "enable debug."},         //     --debug
        {'v', nil, nil,
         "show version with"                          // -v
         "<br>comment line 2."}};
    const int optsum = sizeof(options) / sizeof(options[0]);

    if (miniopt.init(argc, (char **)argv, options, optsum) != 0) {
        printf("error: %s\n", miniopt.what());
        return 0;
    }

    int status;
    while ((status = miniopt.getopt()) > 0) {
        int id = miniopt.optind();
        switch (id) {
            case 0:    // -a, --append
            case 1:    // -r, --remove
            case 2:    // -h, --help
            case 3:    //     --debug
            case 4:    // -v
                printf("[option]\n  ");
                printf("optin-index = [%d] ", id);
                if (options[id].sname) {
                    printf("short-name = [%c]  ", options[id].sname);
                }
                if (options[id].lname) {
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

    if (status < 0) printf("error: %s\n", miniopt.what());

    printf("\nOptions:\n");
    miniopt.printopts(printf, 2);

    if (status == 0) printf("\n--test pass--\n");

    return status;
}