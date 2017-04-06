/**
 * \code
 *     Copyright (c) 2017 Marc Munro
 *     Author:  Marc Munro
 *     License: GPL V3
 *
 * \endcode
 * @brief  
 * The volumed volume management daemon.
 *
 * PLAN:
 *   - doxygen headers for all functions
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
//#include <libgen.h>
//#include <stdbool.h>

static char *progname = NULL;

typedef struct s_options {
    int port;
    int verbosity;
} options_t;

static options_t options = {8888, 0};

static int verbose_flag = 0;

void
closedown(int exitcode)
{
    free(progname);
    exit(exitcode);
}

void
usage(int exitcode)
{
    fprintf(stderr,
	    "usage: %s [--verbose] [--port port-number]\n"
	    "       port-number: the port on which the websocket "
	    "is to be created.\n\n" , progname);
    closedown(exitcode);
}

void record_progname(char **argv)
{
    char *pname = (char *) malloc(strlen(argv[0]) + 1);
    strcpy(pname, argv[0]);
    progname = pname;
}

void
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

void
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

int
main(int argc, char **argv)
{
    process_args(argc, argv);
    printf("Port: %d, Verbosity: %d\n", options.port, options.verbosity);
    closedown(0);
}
