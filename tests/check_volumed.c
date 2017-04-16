/*
 *     Copyright (c) 2017 Marc Munro
 *     Author:  Marc Munro
 *     License: GPL V3
 *
 * This provides unit tests for volumed.  Unit testing is managed using
 * "check", the unit testing framework for C.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <check.h>
#include "../src/volumed.h"

#define PROGNAME "./volumed"
#define CFGNAME  "wibble"
#define CFGNAME2 "wibble2"

START_TEST(param_progname)
{
    char *argv[] = {PROGNAME};
    process_args(1, argv);
    ck_assert(strcmp(PROGNAME, progname) == 0);
}
END_TEST

START_TEST(param_configfile)
{
    char *argv[] = {PROGNAME, "-c", CFGNAME};
    char *argv2[] = {PROGNAME, "-c", CFGNAME2};
    char *argv3[] = {PROGNAME, "--config=" CFGNAME};
    
    process_args(3, argv);
    ck_assert(strcmp(CFGNAME, options.config_filename) == 0);

    process_args(3, argv2);
    ck_assert(strcmp(CFGNAME2, options.config_filename) == 0);

    process_args(3, argv3);
    ck_assert(strcmp(CFGNAME, options.config_filename) == 0);
}
END_TEST

static Suite *
params_suite(void)
{
    Suite *s;
    TCase *tc_params;

    s = suite_create("Volumed");
    tc_params = tcase_create("params");
    suite_add_tcase (s, tc_params);

    tcase_add_test(tc_params, param_progname);
    tcase_add_test(tc_params, param_configfile);

    return s;
}

    
int
main(int argc, char *argv[])
{
    int number_failed;
    Suite *s;
    SRunner *sr;

    s = params_suite();
    sr = srunner_create(s);
    
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
