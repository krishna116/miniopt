/**
 * The MIT License
 *
 * Copyright 2022 Krishna sssky307@163.com
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#pragma once

//
// If you need assert, define USING_MINIOPT_ASSERT before include this file.
//
#ifdef USING_MINIOPT_ASSERT
#include <assert.h>
#define miniopt_assert(x) assert(x)
#else
#define miniopt_assert(x)
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Option type.
 * 
 */
#define opt_no_arg 0     ///< Option has no argument.
#define opt_has_arg 1    ///< Option has an argument.

/**
 * @brief Option.
 * 
 * Some rules to make things clear:
 * 
 * - If option has no short name, make it empty(option.sname = '\0').
 * - If option has no long name, make it empty(option.lname = "").
 * - Either option.sname or option.lname cannot be empty in an option.
 * 
 * - If option.sname used, it cannot be character '-' or '='.
 * - If option.lname used, its length ought to no less than 2.
 * - If option.lname used, it cannot begin with character '-'.
 * - If option.lname used, it cannot use character '='.
 * 
 * - It is user's responsibility to make sure no duplicated options 
 *   in an option array.
 */
typedef struct option_ {
    const char sname;     ///< Option short name;
    const char *lname;    ///< Option long name;
    const char *desc;     ///< Option description;
    int type;             ///< Option type.
} option;

/**
 * @brief Initialize miniopt object.
 *
 * @param[in] argc      Argument array size.
 * @param[in] argv      Argument array.
 * @param[in] opts      Option array.
 * @param[in] optsum    Option sum of the option array.
 *
 * @return 0            Init pass.
 * @return other        Init error.
 */
typedef int (*miniopt_init)(int argc, char **argv, option *opts, int optsum);

#define MINIOPT_PASS 1
#define MINIOPT_FINISHED 0
#define MINIOPT_ERROR -1
/**
 * @brief Get next option.
 * 
 * It should be used after miniopt.init();
 *
 * @return MINIOPT_PASS         Get next option pass.
 * @return MINIOPT_FINISHED     Work finished ok(no more option to get).
 * @return MINIOPT_ERROR        Work finished or stopped with error.
 */
typedef int (*miniopt_getopt)();

/**
 * @brief Get current option index to the option array.
 *
 * It should be used after (miniopt.getopt() > 0);
 *
 * Here optsum means sum-of-the-option-array or option-array-size.
 *
 * @return [0, optsum-1]    An option index to the option array.
 * @return optsum           It means miniopt.optarg() is non option argument.
 */
typedef int (*miniopt_optind)();

/**
 * @brief Get current option-argument or non-option-argument.
 *
 * It should be used after (miniopt.getopt() > 0);
 *
 * @return not NULL     An argument.
 * @return NULL         Current option has no argument.
 */
typedef const char *(*miniopt_optarg)();

/**
 * @brief printf function declaration.
 */
typedef int (*printf_fn)(char const* const _Format, ...);

/**
 * @brief Print the option array.
 * 
 * @param[in] printf_       User provide printf function.
 * @param[in] indention     Print indention at the line beginning.
 */
typedef void (*miniopt_printopts)(printf_fn printf_, int indention);

/**
 * @brief Get current error str.
 *
 * @return not NULL     An erro str.
 * @return NULL         No error.
 */
typedef const char *(*miniopt_what)();

/**
 * @brief Miniopt class.
 */
typedef struct Miniopt_ {
//
// Public api:
//
    miniopt_init        init;       ///< Initialize miniopt object.
    miniopt_getopt      getopt;     ///< Get next option.
    miniopt_optind      optind;     ///< Get current option index.
    miniopt_optarg      optarg;     ///< Get current opt-arg or non-opt-arg.
    miniopt_printopts   printopts;  ///< Print options.
    miniopt_what        what;       ///< Print any error.
} Miniopt;

/**
 * @brief Miniopt singleton object declaration.
 */
extern Miniopt miniopt;

#ifdef __cplusplus
}
#endif