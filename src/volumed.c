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
  - providing a direct interface to the alsa mixer controls rather
    than invoking amixer as a shell command.
*/

#define DOCONLY
#include "config.c"
#undef DOCONLY

/*
 * PLAN:
 *   - create basic websocket service
 *   - create volumec
 *   - create volume-config-moode
 *   - man page?
 *   - debianize
 */

#include <stdio.h>
#include <stdlib.h>
#include "volumed.h"


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
    read_config_file();
    printf("Port: %d, Verbosity: %d\n", options.port, options.verbosity);
    printf("volcurve: %d, max_pct: %d\n", options.volcurve, options.max_pct);
    printf("alsa_mixer: %s, mpd_mixer: %s\n",
	   options.alsa_mixer_name, options.mpd_mixer);
    printf("alsa_card: %s\n", options.alsa_card);
    closedown(0);
    return 0;
}
