
# miniopt

miniopt is a command line argument parser library for C/C++ language.

## Features.
- Support C99 standard.
- Support GCC/MSVC/CLANG compiler.
- Can be no C library dependency.
- No third party dependencies.

# Example

Here is a test example.

```C
// test.c
#include "miniopt.h"
#include <stdio.h>

int main(int argc, char *argv[]) {
    option options[] = {
        {'a', "append", "<file>", "append file."},      // -a, --append
        {'r', "remove", "<file>", "remove file."},      // -r, --remove
        {'h', "help",   nil,      "show help."},        // -h, --help
        {nil, "debug",  nil,      "enable debug."},     //     --debug
        {'v', nil,      nil,      "show version with"   // -v
                                  "<br>comment line 2."
        }
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

    return status;
}
```

After compile the test, you can run the test by follow example commands.

```bash
> test -a key1 -akey2 -a=key3=value 
> test --append key1 --append=key2 --append=key3=value
> test xxx -r 123 yyy --remove 456 zzz
> test -v -vvv -vhv --debug
> test -a 123 -- -a 456 -v -vhv
```

# How to build the library.
It is just a pair of file([miniopt.h](src/miniopt.h) and [miniopt.c](src/miniopt.c)), and any C99 compiler ought to build it pass.

## The library public APIs.
```C
// Initialize miniopt.
int miniopt.init(int argc, char **argv, option *opts, int optsum);

// Get next option.
int miniopt.getopt();

// Get current option index.
int miniopt.optind();

// Get current option-argument or non-option-argument.     
const char* miniopt.optarg();

// Print options.
void miniopt.printopts(printf_fn printf_, int indention);

// Print any error.
const char* miniopt.what();
```

## Support option features.
1. Short options that all have no arguments can concatenate with each other. For example:  
    ```
    foo -a -b -c  
    foo -abc  
    ```
2. A short option that have an argument can concatenate with its argument. For example:  
    ```
    foo -D _DEBUG
    foo -D=_DEBUG
    foo -D_DEBUG    ## If this option have both means of clause 1 and clause 2, 
                    ## this clause 2 will take effect first, so it won't be 
                    ## seen as "foo -D -_ -D -E -B -U -G".
    ```
3. A long option that have an argument can concatenate with its argument. For example:  
    ```
    foo --define USING_DEBUG  
    foo --define=USING_DEBUG  
    ```
4. The token "--" is reserved as **non option argument marker**. it means all string(s) after which will be seen as **non option argument**(s). For example:  
    ```bash
    $> rm -f       ## You can not delete a file name "-f" by this command,
    $> rm -- -f    ## but you can delete the file by this command.
    ```

# The code generator
There is an experimental **code generator** can be used to generate the code automatically. You can run follow commands to build it, and the document is [here](./tool/readme.md).
```bash
> cmake -S . -B build
> cmake --build build
``` 


# References  
 
https://pubs.opengroup.org/onlinepubs/9699919799/basedefs/V1_chap12.html  
https://www.gnu.org/software/libc/manual/html_node/Argument-Syntax.html  
https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Options.html  
https://linux.die.net/man/3/getopt_long  
https://github.com/rpm-software-management/popt  
