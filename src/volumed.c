/*
 *     Copyright (c) 2017 Marc Munro
 *     Author:  Marc Munro
 *     License: GPL V3
 *
 */

/*! @mainpage volumed
@brief  
The volumed volume management daemon.

@copyright
(c) 2017 Marc Munro \n

@par License
GPL version 3

@version 0.1.0

@section overview Overview
volumed is a websocket-based server providing a responsive volume
control for music players such as runeaudio, volumio and moode
audio.  It allows for:
- a more responsive web UI;
- more responsive lirc control through the companion volumec
   executable.

It does this by:
  - providing a fully asynchronous interface;
  - accumulating/batching new commands when a command is already
    running;
  - providing a direct interface to the alsa mixer controls instead
    of invoking amixer as a shell command.
*/

#define DOCONLY
#include "config.c"
#undef DOCONLY

/*
 * PLAN:
 *   - check docs
 *   - change some errors to warnings
 *   - unit tests
 *   - valgrind
 *   - lint?
 *   - create basic websocket service
 *   - create volumec
 *   - create volume-config-moode
 *   - man page?
 *   - debianize
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <stdarg.h>
#include "volumed.h"


/**
 * @brief The invocation name by which we were launched, used in
 * error and usage messages
 */
static char *progname = NULL;

static options_t options = {
    8888, 			/* default websocket port */
    0,    			/* default verbosity */
    NULL,			/* config file name */
    CONFIG_VOLCURVE,
    CONFIG_MAX_PCT,
    CONFIG_ALSA_MIXER_NAME,
    CONFIG_MPD_MIXER,
    CONFIG_ALSA_CARD
};


/**
 * @brief Safely close down \ref index, returning \p exitcode.
 *
 * @param exitcode (integer) code to be returned on \ref index exit.
 *
 * Close down and free all resources used by \ref index before exitting
 * with exitcode.
 */
extern void
closedown(int exitcode)
{
    free(progname);
    FREE(options.config_filename);
    exit(exitcode);
}

/**
 * @brief Show usage message and exit with \p exitcode
 *
 * @param exitcode (integer) code to be returned on \ref index exit.
 *
 */
static void
usage(int exitcode)
{
    fprintf(stderr,
	    "usage: %s [-v | --verbose] [(-p | --port) port-number]\n"
	    "        [(-c | --config) config-file] [-V | --version]\n"
	    "    port-number: the port on which the websocket "
	    "is to be created\n"
	    "                 (default - %d);\n"
	    "    config-file: the name of a configuration file to use\n"
	    "                 (default - \"%s\").\n"
	    "\n" , progname, DEFAULT_PORT, CONFIG_FILE);
    closedown(exitcode);
}

/**
 * @brief Record our invocation name for use in error and usage messages.
 *
 * @param argv (char **) Argv as passed in to main()  We get the program
 * name from argv[0] and store it in  #progname.
 */
static void
record_progname(char **argv)
{
    char *pname = (char *) malloc(strlen(argv[0]) + 1);
    strcpy(pname, argv[0]);
    progname = pname;
}

/**
 * @brief Show out name and version and gracefully exit.
 *
 */
static void
show_version_and_exit()
{
    printf("%s - volume control daemon, version: %s\n%s\n%s\n\n",
	   progname, VERSION, COPYRIGHT, WARRANTY);
    closedown(0);
}

/**
 * @brief Validate and record our command line arguments.
 *
 * @param argc (int) The number of arguments passed to us.
 * @param argv (char **) The array of command line arguments.
 */
static void
process_args(int argc, char **argv)
{
    struct option option_defs[] = {
	{"port", required_argument, NULL, 0},
	{"verbose", no_argument, NULL, 0},
	{"version", no_argument, NULL, 0},
	{"config", required_argument, NULL, 0},
	{NULL, 0, NULL, 0}
    };
    char option_map[] = {'p', 'v', 'V', 'c'};
    int c;
    int oidx = 0;
    
    record_progname(argv);

    while ((c = getopt_long(
		argc, argv, "c:p:vV", option_defs, &oidx)) != -1)
    {
	if (c == 0) {
	    /* Get the shortcode that matches the long option. */
	    c = option_map[oidx];
	}	    
	switch (c) {
	case 'c':
	    FREE(options.config_filename);
	    STRCPY(options.config_filename, optarg);
	    break;
	case 'p':
	    options.port = atoi(optarg);
	    if ((options.port <= 0) || (options.port > 65535)) {
		fail(2, "port must be a number in the range 1 .. 65535");
	    }
	    break;
	case 'V':
	    show_version_and_exit();
	    break;
	case 'v':
	    options.verbosity++;
	    break;
	case '?':
	    usage(2);
	    break;
	}
    }
}

void *
checked_malloc(size_t size, const char *file, int line)
{
    void *res = malloc(size);
    if (!res) {
	fail(2, "Unable to allocate memory of size %d at %s:%d",
	     size, file, line);
    }
}

/**
 * @brief Print an error message and, possibly, exit with a failure code.
 *
 * If \p code is non-zero, we will exit with that as an error code.

 * @param code (int) The errorcode with which to exit, if non-zero.
 * @param fmt (char *)... Formatting parameters just as with printf and
 *                        friends.
 */
void
fail(int code, const char *fmt, ...)
{
    va_list argp;
    fprintf(stderr, "%s: ", progname);
    va_start(argp, fmt);
    vfprintf(stderr, fmt, argp);
    va_end(argp);
    fprintf(stderr, "\n");

    if (code) {
	closedown(code);
    }
}


/**
 * @brief Main entry point to \ref index.
 *
 * @param argc (int) The number of arguments passed to us.
 * @param argv (char **) The array of command line arguments.
 */
int
main(int argc, char **argv)
{
    process_args(argc, argv);
    read_config_file(&options);
    printf("Port: %d, Verbosity: %d\n", options.port, options.verbosity);
    printf("volcurve: %d, max_pct: %d\n", options.volcurve, options.max_pct);
    printf("alsa_mixer: %s, mpd_mixer: %s\n",
	   options.alsa_mixer_name, options.mpd_mixer);
    printf("alsa_card: %s\n", options.alsa_card);
    closedown(0);
}
