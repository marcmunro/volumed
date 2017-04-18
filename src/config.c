/*
 *     Copyright (c) 2017 Marc Munro
 *     Author:  Marc Munro
 *     License: GPL V3
 *
 */

#ifdef DOCONLY
/*
 * Put any mainpage documentation sections that we might need, in here.
 */
#else

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>
#include <ctype.h>
#include "volumed.h"


/**
 * @brief Open our config file, failing if we have an explicit config
 * file option and it cannot be opened.
 * 
 * @param options (option_t *) Pointer to our options struct.
 * @return (FILE *) the opened config file.
 */
static FILE *
open_config_file(options_t *options, char **p_filename)
{
    FILE *f = NULL;
    char *name;
    
    if (options->config_filename) {
	/* Explicit config file was specified.  We *must* open this. */
	f = fopen(name = options->config_filename, "r");
	if (!f) {
	    dofail(2, "unable to open file: \"%s\"", name);
	}
    }
    else {
	/* Try each of the standard config file locations in turn.  No
	 * problem if we cannot open any of them. */
		
	if (!(f = fopen(name = "./" LOCAL_CONFIG_FILE, "r"))) {
	    /* Nothing in the cur directory; try our home directory. */
	    glob_t paths;
	    int res;
	    if ((res = glob("~/" LOCAL_CONFIG_FILE,
			    GLOB_TILDE, NULL, &paths)) == 0)
	    {
		name = paths.gl_pathv[0];
		STRCPY((*p_filename), name);
	        f = fopen(name, "r");
		globfree(&paths);
		if (!f) {
		    FREE(*p_filename);
		    *p_filename = NULL;
		}

	    }
	    else {
		/* Nothing in our home directory; try the default config
		 * file path. */ 
		f = fopen(name = CONFIG_FILE, "r");
	    }
	}
    }
    if (f && !*p_filename) {
	STRCPY((*p_filename), name);
    }
    return f;
}


/**
 * @brief Read the next token and value pair from the config file.
 * 
 * Keep calling this until it returns NULL at which time \p f will have
 * been closed.
 *
 * @param f (FILE *) The file pointer for the configuration file.
 * @param p_token (char **) Pointer to a (char *) string variable
 *        into which a dynamically allocated string will be placed for a
 *        configuration token.  The caller must free this.
 * @param p_value (char **) Pointer to a (char *) string variable
 *        into which a dynamically allocated string will be placed for a
 *        configuration value.  The caller must free this.
 * 
 * @return (FILE *) the file pointer for the config file until the last
 *         config setting has been returned, and then NULL.
 */
extern FILE *
next_config_setting(
    FILE *f, char **p_token, char **p_value, int *p_line_no, char *filename)
{
    char c;
    char *idx;
    char *trailing_space = NULL;
    bool in_comment = false;
    bool before_text = true;
    int len = 0;
    int size = 100;
    
    fflush(stdout);

    *p_token = idx = (char *) MALLOC(size);
    *p_value = NULL;

    while ((c = fgetc(f)) != EOF) {
	if (in_comment) {
	    if (c != '\n') {
		continue;
	    }
	}
	switch (c) {
	case '#':
	    in_comment = true;
	    continue;
	case '=':
	    *idx = '\0';
	    *p_value = idx = (char *) malloc(size);
	    before_text = true;
	    if (trailing_space) {
		*trailing_space = '\0';
		trailing_space = NULL;
	    }
	    continue;
	case '\n':
	    in_comment = false;
	    (*p_line_no)++;
	    break;
	default:
	    if (before_text && isspace(c)) {
		continue;
	    }
	    before_text = false;
	    if (isspace(c)) {
		if (!trailing_space) {
		    trailing_space = idx;
		}
	    }
	    else {
		trailing_space = NULL;
	    }
	    *idx = c;
	    idx++;
	    len++;
	    if (len >= size) {
		size = size + 200;
		if (*p_value) {
		    *p_value = realloc((void *) *p_value, size);
		    idx = *p_value + len;
		}
		else {
		    *p_token = realloc((void *) *p_token, size);
		    idx = *p_token + len;
		}
	    }
	    continue;
	}
	/* Only get here at the end of a line. */
	*idx = '\0';
	if (trailing_space) {
	    *trailing_space = '\0';
	}
	if (idx == *p_token) {
	    /* We read nothing, so go on to the next line. */
	    continue;
	}
	if (!*p_value) {
	    fflush(stdout);
	    /* We have no value. */
	    fprintf(stderr,
		    "Warning: Invalid configuration entry \"%s\" "
		    "(entry ignored) at %s:%d\n",
		    *p_token, filename, *p_line_no);
	    idx = *p_token;
	    continue;
	}
	return f;
    }

    /* We reach this point only when eof has been reached, and there is
     * nothing to return to the caller. */
    fclose(f);
    return NULL;
}

static cfg_option_t cfg_options[] = {
    {CFG_NAME_VOLCURVE,  BOOLEAN},
    {CFG_NAME_MAX_PCT,  INTEGER},
    {CFG_NAME_ALSA_MIXER_NAME,  STRING},
    {CFG_NAME_MPD_MIXER,  STRING},
    {CFG_NAME_ALSA_CARD,  STRING},
    {CFG_NAME_PORT,  INTEGER},
    {NULL, NONE}
};

/**
 * @brief Identify the #cfg_options entry for \p name.
 * 
 * @param name (char *) A token, returned from next_config_setting() for
 * which we want the matching #cfg_options entry.
 *
 * @return (int) The index into #cfg_options for \p name, or -1 if no
 * matching entry is found.
 */
static int
get_option(const char *name)
{
    cfg_option_t *opt = cfg_options;
    int idx = 0;
    while (opt->option_name) {
	if (strcmp(name, opt->option_name) == 0) {
	    return idx;
	}
	opt++;
	idx++;
    }
    return -1;
}

/**
 * @brief Convert a string to lower case
 * 
 * @param str (char *) The string to downcase.
 */
static void
downcase(char *str)
{
    int i;
    for (i = strlen(str); i >= 0; i--) {
	str[i] = tolower(str[i]);
    }
}

/**
 * @brief Read our config file, updating the #options variable.
 * 
 * @param options (option_t *) Pointer to our options struct.
 */
extern void
read_config_file()
{
    char *filename = NULL;
    FILE *f = open_config_file(&options, &filename);
    char *token;
    char *value;
    char *ptr;
    char  c;
    bool  bval = false;
    int   ival = 0;
    int   line_no;
    int   opt_id;
    
    while (f &&
	   (f = next_config_setting(f, &token, &value, &line_no, filename)))
    {
        downcase(token);
	opt_id = get_option(token);
	if (opt_id >= 0) {
	    switch (cfg_options[opt_id].option_type) {
	    case BOOLEAN:
		downcase(value);
		bval = ((strcmp(value, "yes") == 0) ||
			(strcmp(value, "true") == 0));
		if (!bval) {
		    if (!((strcmp(value, "no") == 0) ||
			  (strcmp(value, "false") == 0)))
		    {
			fprintf(stderr,
				"Warning: invalid value (%s) for boolean "
				"\"%s\" (entry ignored) at %s:%d\n",
				value, token, filename, line_no);
		    }
		}
		break;
	    case INTEGER:
		ival = 0;
		ptr = value;
		while ((c = *ptr)) {
		    ptr++;
		    if ((c >= '0') && (c <= '9')) {
			ival *= 10;
			ival += (c - '0');
		    }
		    else {
			fprintf(stderr,
				"Warning: Invalid value (%s) for integer "
				"\"%s\" (entry ignored) at %s:%d\n",
				value, token, filename, line_no);
		    }
		}
	    default:
		break;
	    }
	    switch (opt_id) {
	    case 0:
		options.volcurve = bval;
		FREE(value);
		break;
	    case 1:
		options.max_pct = ival;
		FREE(value);
		break;
	    case 2:
		options.alsa_mixer_name = value;
		break;
	    case 3:
		options.mpd_mixer = value;
		break;
	    case 4:
		options.alsa_card = value;
		break;
	    case 5:
		options.port = ival;
		break;
	    }
	}
	else {
	    fprintf(stderr,
		    "Warning: Unrecognized token \"%s\" "
		    "(entry ignored) at %s:%d\n", token, filename, line_no);
	}
    }
    FREE(filename);
}



#endif
