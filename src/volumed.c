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

/*
 * PLAN:
 *   - help/version args?
 *   - valgrind makefile target
 *   - read config file
 *   - lint?
 *   - unit tests
 *   - create basic websocket service
 *   - man page?
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>


/**
 * @brief The invocation name by which we were launched, used in
 * error and usage messages
 */
static char *progname = NULL;

/**
 * @brief Structure for containing configuration options read from the 
 * config file or command line.
 */
typedef struct s_options {
    int port;
    int verbosity;
} options_t;

static options_t options = {8888, 0};

static int verbose_flag = 0;

/**
 * @brief Safely close down \ref index, returning \p exitcode.
 *
 * @param exitcode (integer) code to be returned on \ref index exit.
 *
 * Close down and free all resources used by \ref index before exitting
 * with exitcode.
 */
void
closedown(int exitcode)
{
    free(progname);
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
	    "usage: %s [--verbose] [--port port-number]\n"
	    "       port-number: the port on which the websocket "
	    "is to be created.\n\n" , progname);
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
 * @brief Record the port number of the websocket to be opened.
 *
 * @param optarg (char *) The string representing the port number.
 */
static void
record_port(char *optarg)
{
    options.port = atoi(optarg);
    if ((options.port <= 0) || (options.port > 65535)) {
	fprintf(stderr,
		"%s: port must be a number in the range 1 .. 65535\n",
		progname);
	usage(2);
    }
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
    static struct option option_defs[] = {
	{"port", required_argument, NULL, 0},
	{"verbose", no_argument, NULL, 0},
	{NULL, 0, NULL, 0}
    };
    int c;
    int oidx = 0;
    
    record_progname(argv);

    while ((c = getopt_long(
		argc, argv, "p:v", option_defs, &oidx)) != -1)
    {
	switch (c) {
	case 0:
	    if (strcmp(option_defs[oidx].name, "port") == 0) {
		record_port(optarg);
	    }
	    else if (strcmp(option_defs[oidx].name, "verbose") == 0) {
		options.verbosity++;
	    }
	    break;
	case 'p':
	    record_port(optarg);
	    break;
	case 'v':
	    options.verbosity++;
	    break;
	case '?':
	    break;
	}
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
    printf("Port: %d, Verbosity: %d\n", options.port, options.verbosity);
    closedown(0);
}
