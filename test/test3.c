/**
 * The MIT License
 *
 * Copyright 2022 Krishna sssky307@163.com
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#include "miniopt.h"
#include <stdio.h>

int ParseArgs(int argc, char **argv) {
    option options[] = {
        {'a', "append", "<key=value>", "append key and value."},
        {'r', "remove", "<key>", "remove key."},
        {'q', "query", "<key>", "query key."},
        {'m', "modify", "<key=value>", "modify key."},
        {'l', "list", nil, "list keys."},
        {nil, "offset", "<n>", "list offset number."},
        {nil, "limit", "<n>", "list size."},
        {'d', nil, "<val>", "define something."},
        {'e', nil, "<val>", "any description1."},
        {'f', nil, "<val>", "any description2."},
        {'g', nil, nil, "any description3."}
    };
    const int optsum = sizeof(options) / sizeof(options[0]);

    if (miniopt.init(argc, (char **)argv, options, optsum) != 0) {
        printf("error: %s\n", miniopt.what());
        return 0;
    }

    int status;
    while ((status = miniopt.getopt()) > 0) {
        int id = miniopt.optind();
        switch (id) {
            case 0: // -a --append <key=value>
                // <key=value> = miniopt.optarg()
            break;
            case 1: // -r --remove <key>
                // <key> = miniopt.optarg()
            break;
            case 2: // -q --query <key>
                // <key> = miniopt.optarg()
            break;
            case 3: // -m --modify <key=value>
                // <key=value> = miniopt.optarg()
            break;
            case 4: // -l --list
            break;
            case 5: //    --offset <n>
                // <n> = miniopt.optarg()
            break;
            case 6: //    --limit <n>
                // <n> = miniopt.optarg()
            break;
            case 7: // -d <val>
                // <val> = miniopt.optarg()
            break;
            case 8: // -e <val>
                // <val> = miniopt.optarg()
            break;
            case 9: // -f <val>
                // <val> = miniopt.optarg()
            break;
            case 10: // -g
            break;
            default:
            printf("[non-opt-arg] arg = [%s]\n", miniopt.optarg());
            break;
        }
    }

    if (status < 0) printf("error: %s\n", miniopt.what());

    printf("\nOptions:\n");
    miniopt.printopts(printf, 2);

    return status;
}

int main(int argc, char* argv[]){
    return ParseArgs(argc, argv);
}
