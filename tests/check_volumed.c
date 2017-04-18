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
#include <unistd.h>
#include <check.h>
#include "../src/volumed.h"

#define PROGNAME "./volumed"
#define CFGNAME  "wibble"
#define CFGNAME2 "wibble2"

FILE *
redirect(FILE *stream, char *path)
{
    return(freopen(path, "a+", stream));
}


START_TEST(param_progname)
{
    char *argv[] = {PROGNAME};
    process_args(1, argv);
    ck_assert(strcmp(PROGNAME, progname) == 0);
}
END_TEST

/* Test the handling of the --config option. */
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

/* Test the handling of the --port option. */
START_TEST(param_port)
{
    char *argv[] = {PROGNAME};
    char *argv2[] = {PROGNAME, "-p", "8887"};
    char *argv3[] = {PROGNAME, "--port", "8886"};
    
    process_args(1, argv);
    ck_assert_int_eq(options.port, 8888);

    process_args(3, argv2);
    ck_assert_int_eq(options.port, 8887);

    process_args(3, argv3);
    ck_assert_int_eq(options.port, 8886);
}
END_TEST

typedef void (redirect_checker_fn_t)(void);
static FILE *my_stderr;
static redirect_checker_fn_t *chk_redirect;
static expected_exitcode = 0;

static void
redirect_setup(void)
{
    my_stderr = redirect(stderr, "stderr.log");
    chk_redirect = NULL;
    expected_exitcode = 0;
}

static void
redirect_teardown(void)
{
    unlink("stderr.log");
    unlink("stdout.log");
}

extern void
closedown(int exitcode)
{
    if (chk_redirect) {
	chk_redirect();
    }
    ck_assert_int_eq(exitcode, expected_exitcode);
}

static void
check_usage()
{
    int r;
    fflush(my_stderr);
    r = system("grep usage: stderr.log >/dev/null") >> 8;

    if (r != 0) {
	ck_abort_msg("Usage message not found.");
    }
}

static void
check_no_arg_for_option()
{
    int r;
    fflush(my_stderr);
    //r = system("cat stderr.log");
    r = system(
	"grep \"option.*requires an argument\" stderr.log >/dev/null") >> 8;

    if (r != 0) {
	ck_abort_msg("Error message not found.");
    }
    check_usage();
}

static void
check_unexpected()
{
    int r;
    fflush(my_stderr);
    //r = system("cat stderr.log");
    r = system("grep \"unrecognized option\" stderr.log >/dev/null") >> 8;

    if (r != 0) {
	ck_abort_msg("Error message not found.");
    }
    check_usage();
}

START_TEST(param_missing_port)
{
    char *argv[] = {PROGNAME, "-p"};

    /* This will result in a usage message and exit with a failure
     * code. */
    expected_exitcode = 2;
    chk_redirect = check_no_arg_for_option;
    process_args(2, argv);
}
END_TEST

START_TEST(param_missing_config)
{
    char *argv[] = {PROGNAME, "--config"};

    /* This will result in a usage message and exit with a failure
     * code. */
    expected_exitcode = 2;
    chk_redirect = check_no_arg_for_option;
    process_args(2, argv);
}
END_TEST

START_TEST(param_version)
{
    int stdout_save = dup(STDOUT_FILENO);
    char *argv[] = {PROGNAME, "--version"};
    FILE *my_stdout;
    int r;

    fflush(stdout); 
    my_stdout = redirect(stdout, "stdout.log");

    process_args(2, argv);

    fflush(my_stdout); 
    dup2(stdout_save, STDOUT_FILENO); //restore the previous state of stdout

    r = system(
	"grep \"" PROGNAME ".*volume control daemon.*"
	VERSION "\" stdout.log >/dev/null") >> 8;

    if (r != 0) {
	ck_abort_msg("version message not found.");
    }
    
    r = system("grep -i Copyright stdout.log >/dev/null") >> 8;

    if (r != 0) {
	ck_abort_msg("copyright message not found.");
    }
}
END_TEST

START_TEST(param_verbose)
{
    char *argv[] = {PROGNAME, "-v", "--verbose", "--verb"};

    process_args(4, argv);
    ck_assert_int_eq(options.verbosity, 3);
}
END_TEST

START_TEST(param_unexpected)
{
    char *argv[] = {PROGNAME, "--wibble"};

    /* This will result in a usage message and exit with a failure
     * code. */
    expected_exitcode = 2;
    chk_redirect = check_unexpected;
    process_args(2, argv);
}
END_TEST

static TCase *
tcase_params()
{
    TCase *tc_params = tcase_create("params");
    tcase_add_checked_fixture(tc_params, redirect_setup, redirect_teardown);

    tcase_add_test(tc_params, param_progname);
    tcase_add_test(tc_params, param_configfile);
    tcase_add_test(tc_params, param_port);
    tcase_add_test(tc_params, param_missing_port);
    tcase_add_test(tc_params, param_missing_config);
    tcase_add_test(tc_params, param_version);
    tcase_add_test(tc_params, param_verbose);
    tcase_add_test(tc_params, param_unexpected);

    return tc_params;
}

START_TEST(config_base)
{
    char *argv[] = {PROGNAME};

    process_args(1, argv);
    ck_assert(options.config_filename == NULL);
    ck_assert(options.port == 8888);
    ck_assert(options.volcurve == true);
    ck_assert(options.max_pct == 100);
    ck_assert(strcmp(options.alsa_mixer_name, "Digital") == 0);
    ck_assert(strcmp(options.mpd_mixer, "hardware") == 0);
    ck_assert(options.alsa_card == NULL);   
}
END_TEST

START_TEST(config_tst1)
{
    char *argv[] = {PROGNAME,  "-c", "configfile.tst1"};
    int r1;
    int r2;
    int r3;
    int r4;
    
    redirect(stderr, "stderr.log");
    process_args(3, argv);
    read_config_file();
    fflush(stderr);
    r1 = system("grep \"Warning: Invalid configuration entry.*VOLCURVE\" "
		" stderr.log >/dev/null");
    r2 = system("grep \"Warning: Unrecognized token.*wibble\" "
		" stderr.log >/dev/null");
    r3 = system("grep \"invalid value (hellyes)\" "
		" stderr.log >/dev/null");
    r4 = system("grep \"nvalid value (s) for integer\" "
		" stderr.log >/dev/null");
    unlink("stderr.log");
    ck_assert(strcmp(options.config_filename, "configfile.tst1") == 0);
    ck_assert(options.port == 8888);
    ck_assert(options.volcurve == true);
    ck_assert(options.max_pct == 99);
    ck_assert(strcmp(options.alsa_mixer_name, "Digital") == 0);
    ck_assert(strcmp(options.mpd_mixer, "hardware") == 0);
    ck_assert(strcmp(options.alsa_card, "Bloodnok") == 0);
    ck_assert_int_eq(r1, 0);  // Check for expected warning no. 1
    ck_assert_int_eq(r2, 0);  // Check for expected warning no. 2
    ck_assert_int_eq(r3, 0);  // Check for expected warning no. 3
    ck_assert_int_eq(r4, 0);  // Check for expected warning no. 4
}
END_TEST

START_TEST(config_tst2)
{
    char *argv[] = {PROGNAME,  "--config=configfile.tst2"};
    
    process_args(2, argv);
    read_config_file();

    ck_assert(strcmp(options.config_filename, "configfile.tst2") == 0);
    ck_assert(options.port == 8889);
    ck_assert(options.volcurve == false);
    ck_assert(options.max_pct == 96);
    ck_assert(strcmp(options.alsa_mixer_name, "Analog") == 0);
    ck_assert(strcmp(options.mpd_mixer, "software") == 0);
    ck_assert(options.alsa_card == NULL);
}
END_TEST

START_TEST(config_tst3)
{
    char *argv[] = {PROGNAME};

    system("cp configfile.tst2 .volumed.conf");
    process_args(1, argv);
    read_config_file();

    ck_assert(options.port == 8889);
    ck_assert(options.volcurve == false);
    ck_assert(options.max_pct == 96);
    ck_assert(strcmp(options.alsa_mixer_name, "Analog") == 0);
    ck_assert(strcmp(options.mpd_mixer, "software") == 0);
    ck_assert(options.alsa_card == NULL);

    unlink(".volumed.conf");
}
END_TEST

START_TEST(config_tst4)
{
    char *argv[] = {PROGNAME,  "--config=configfile.tst99"};
    int r;
    
    redirect(stderr, "stderr.log");
    process_args(2, argv);
    expected_exitcode = 2;
    read_config_file();
    fflush(stderr);
    r = system("grep \"unable to open file:.*configfile.tst99\""
	       " stderr.log >/dev/null");
    unlink("stderr.log");
    
    ck_assert(strcmp(options.config_filename, "configfile.tst99") == 0);
    ck_assert(options.port == 8888);
    ck_assert(options.volcurve == true);
    ck_assert(options.max_pct == 100);
    ck_assert(strcmp(options.alsa_mixer_name, "Digital") == 0);
    ck_assert(strcmp(options.mpd_mixer, "hardware") == 0);
    ck_assert(options.alsa_card == NULL);
    ck_assert_int_eq(r, 0);  // Check for expected warning
}
END_TEST

static TCase *
tcase_config()
{
    TCase *tc_config = tcase_create("config");

    tcase_add_test(tc_config, config_base);
    tcase_add_test(tc_config, config_tst1);
    tcase_add_test(tc_config, config_tst2);
    tcase_add_test(tc_config, config_tst3);
    tcase_add_test(tc_config, config_tst4);

    return tc_config;
}

static Suite *
volumed_suite(void)
{
    Suite *s;

    s = suite_create("Volumed");
 
    suite_add_tcase (s, tcase_params());
    suite_add_tcase (s, tcase_config());
    return s;
}

int
main(int argc, char *argv[])
{
    int number_failed;
    Suite *s;
    SRunner *sr;
    
    s = volumed_suite();
    sr = srunner_create(s);
    
    srunner_run_all(sr, CK_NORMAL);
    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);
    return (number_failed == 0)? EXIT_SUCCESS: EXIT_FAILURE;
}
