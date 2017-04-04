/**
 * @file   volumed.c
 * \code
 *     Copyright (c) 2017 Marc Munro
 *     Author:  Marc Munro
 *     License: GPL V3
 *
 * \endcode
 * @brief  
 * The volumed volume management daemon.
 */

#include <stdio.h>
#include <stdlib.h>
#include <libgen.h>
#include <string.h>

static char *progname = NULL;

void
usage(int exitcode)
{
    fprintf(stderr,
	    "usage: %s [--port port-number]\n"
	    "       port-number: the port on which the websocket "
	    "is to be created.\n\n" , progname);
    exit(exitcode);
}

void record_progname(char **argv)
{
    char *pname = (char *) malloc(strlen(argv[0]) + 1);
    strcpy(pname, argv[0]);
    progname = basename(pname);
}

int
main(int argc, char **argv)
{
    record_progname(argv);
    usage(2);
}
