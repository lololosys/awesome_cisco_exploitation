/* $Id: http_parser.c,v 3.2.4.6 1996/07/12 17:17:21 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http_parser.c,v $
 *------------------------------------------------------------------
 * HTTP parser functions
 *
 * July 1995, Robert Widmer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http_parser.c,v $
 * Revision 3.2.4.6  1996/07/12  17:17:21  widmer
 * CSCdi62752:  Missing HR at top of http parser pages
 * Branch: California_branch
 *
 * Revision 3.2.4.5  1996/07/01  21:03:07  snyder
 * CSCdi61803:  html code uses char * in places it should use char []
 * bRanch: California_branch
 *              Only 88 bytes for this one.
 *
 * Revision 3.2.4.4  1996/06/09  22:15:14  rpratt
 * CSCdi59969:  Adding /- token to user URL, if missing, gives weird
 * context
 * Branch: California_branch
 *
 * Revision 3.2.4.3  1996/05/21  10:06:18  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.4.2  1996/05/19  00:24:57  rpratt
 * CSCdi57306:  HTML config has problems in new platform
 * Branch: California_branch
 *
 * Revision 3.2.4.1  1996/03/18  22:13:27  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/16  07:40:43  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  22:57:42  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:59  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include <ciscolib.h>
#include "config.h"
#include "parser.h"
#include "http.h"
#include "../parser/parser_actions.h"
#include "../tcp/tcp.h"
#include "ttysrv.h"

static const char form_command[] = "command";
static const char form_command_url[] = "command_url";
static const char form_new_command_url[] = "new_command_url";
static const char form_new_base_url[] = "new_base_url";
static const char uri_cr[] = "/CR";
static const char uri_base[] = "/-";
static const char exit_cr[] = "-/EXIT/CR";

/*
 * This is the privilege level the http parser runs at
 */
#define	HTTP_VTY_PRIV	PRIV_ROOT

/*
 * Guess how many expansion characters will be needed - win the
 * ability to not malloc and strcpy into bigger space. 
 */
#define	EXPANSION	5


/*
 * http_parser
 * Send a page that's the result of a (possibly incomplete) exec command.
 */
void http_parser (httpstate *state, parser_mode *mode, boolean print_title)
{
    parseinfo *csb;
    const char *s;
    char *old_uri = NULL;
    char *new_uri = NULL;
    char *baseurl = NULL;
    char *d;
    char *rev_s;
    uint old_priv;
    int error;
    boolean rm_question = FALSE;
    boolean exit_mode = FALSE;
    boolean no_baseurl = TRUE;

    if (parser_http_debug) {
	buginf("\nhttp_parser mode '%s'  command '%s'",
	       get_mode_name(mode), state->uri);
    }

    state->uri_index = state->uri;
    d = http_get_uri_token(state);	/* skip past mode */

    csb = get_csb("http");
    if (csb) {
	stdio->csb = csb;
	csb->http_state = state;
	csb->flags = CONFIG_HTTP;
	csb->mode = mode;
	csb->in_help = PARSER_HELP;
	csb->lastchar = '?';

	old_priv = CUR_PRIV(stdio->privilege);
	csb->priv = HTTP_VTY_PRIV;
	SET_PRIV(stdio->privilege, HTTP_VTY_PRIV);

	if (!mode_http_init(csb)) {
	    stdio->csb = NULL;
	    SET_PRIV(stdio->privilege, old_priv);
	    free_csb(&csb);
	    return;
	}

	s = state->uri_index;
	d = csb->line;

	while (*s && (*s == '/')) {
	    s++;
	}

	while (*s) {
	    switch (toupper(*s)) {
	      case '\\':
		if (*(s+1)) {
		    /* Escaped character */
		    s++;
		}
		*d++ = *s++;
		break;
	      case '-':
		if (state->flags & HTTP_SUPRESS_CLI_FORM) {
		    *d++ = *s++;
		} else {
		    if (no_baseurl && (s > state->uri) && (*(s-1) == '/') &&
				    ((*(s+1) == '/') || (*(s+1)== '\0'))) {
		        baseurl = malloc(((s + 1) + 1 - state->uri) + 1);
		        if (!baseurl) {
			    stdio->csb = NULL;
			    SET_PRIV(stdio->privilege, old_priv);
			    free_csb(&csb);
			    return;
		        } else {
			    *baseurl = '\0';
		        }
		        sstrncpy(baseurl, state->uri, (s + 1) + 1 - state->uri);
		        no_baseurl = FALSE;
		        if (!(strncasecmp(s, exit_cr, strlen(exit_cr)))) {
			    exit_mode = TRUE;
		        }
		        s++;
		    } else {
		        *d++ = *s++;
		    }
		}
		break;
	      case '/':
		if (strcasecmp(s, uri_cr) != 0) {
		    /*
		     * Not EOL, so replace '/' with a space
		     */
		    *d++ = ' ';
		    s++;
		} else {
		    csb->in_help = PARSER_NO_HELP;
		    csb->lastchar = ' ';
		    if (parser_http_debug) {
			*d = '\0';
			buginf("\nComplete command '%s'", csb->line);
		    }
		    while (*s) {
			s++;	/* skip to end and leave while loop */
		    }
		}
		break;
              case '?':
		rm_question = TRUE;
                *d++ = *s++;	/* '?' used to determine last space req. */
                while (*s) {
                    s++;        /* skip to end and leave while loop */
                }
                break;
	      default:
		*d++ = *s++;
		break;
	    }
	}

	if ((no_baseurl && !(state->flags & HTTP_SUPRESS_CLI_FORM)) 
							|| rm_question) {
            new_uri = malloc(strlen(state->uri) + strlen(uri_base) + 1);
            if (!new_uri) {
		if (baseurl) {
		    free(baseurl);
		}
		stdio->csb = NULL;
		SET_PRIV(stdio->privilege, old_priv);
		free_csb(&csb);
                return;
            } else {
		*new_uri = '\0';
	    }
            strcpy(new_uri, state->uri);
	    if (rm_question) {
		rev_s = new_uri;
		while (*rev_s && (*rev_s != '?')) {
		    rev_s++;
		}
		if (*(rev_s-1) == '/') {
		    rev_s--;
		}
		*rev_s = '\0';
	    }
	    if (no_baseurl && !(state->flags & HTTP_SUPRESS_CLI_FORM)) {
		strcat(new_uri,uri_base);
		baseurl = strdup(new_uri);
                if (!baseurl) {
		    if (new_uri) {
		        free(new_uri);
		    }
		    stdio->csb = NULL;
		    SET_PRIV(stdio->privilege, old_priv);
		    free_csb(&csb);
                    return;
                }
	    }
	    
            old_uri = state->uri;
            state->uri = new_uri;
            state->uri_index = state->uri;
            rev_s = http_get_uri_token(state); /* skip past mode */
            if (parser_http_debug) {
                buginf("\nhttp_parser mode '%s'  reworked command '%s'",
                       get_mode_name(mode), state->uri);
            }
        }

        if ((d != csb->line) && (csb->in_help == PARSER_HELP)) {
            if (*(d-1) == '?') {
                d--; /* back up to point at '?' before placing the null */
            } else {
                if (*(d-1) != ' ') {
                    *d++ = ' ';
                }
            }
        }
	*d = '\0';

	if (parser_http_debug) {
	    buginf("\nProcessing http command\n  '%s'\n  '%s'",
		   csb->line, csb->nv_command);
	}

	if (print_title) {
	    http_send_prologue(state, HTTP_STATUS_OK, TRUE, TRUE);
	    printf("\n<HTML><HEAD><TITLE>%s %s</TITLE></HEAD>"
		   "\n<BODY><H1>%s</H1><PRE><HR>",
		   hostname, state->uri, hostname);
	    printf("\n<FORM METHOD=POST ACTION=\"%s\">", csb->http_state->uri);
	    if (!(state->flags & HTTP_SUPRESS_CLI_FORM)) {
	        printf("\n<DT>%s<INPUT TYPE=TEXT NAME=%s SIZE=60 VALUE=\"%s\">",
				"Command ", form_command, rm_question?
				(*csb->line == ' '?(csb->line+1):csb->line):"");
                printf("\n<INPUT TYPE=HIDDEN NAME=%s VALUE=\"%s\">",
				form_command_url, baseurl);
	        if (parser_http_debug) {
                    buginf("\nform_command_url: %s", baseurl);
		}
                printf("\n<H4>Output</H4><H5>Command base-URL was: %s",
				baseurl);
                printf("\nComplete URL was: %s %s%s%s</H5><PRE><HR>", 
				state->uri, 
				(*csb->line)?"\nCommand was: ":"", csb->line,
				((*csb->line)&&rm_question)?"?":"");
	    }
	    if (csb->in_help == PARSER_HELP) {
		printf("<DL>");
	    }
	} else {
            printf("\n<INPUT TYPE=HIDDEN NAME=%s VALUE=\"%s\">",
                        form_new_command_url, baseurl);
	    if (parser_http_debug) {
                buginf("\nform_new_command_url: %s", baseurl);
	    }
        }

	if ((error = parse_cmd(csb)) != 0) {
            report_error(csb, error, TRUE);
	} else {
	    if (print_title) {
		if (!(state->flags & HTTP_SUPRESS_CLI_FORM)) {
	            printf("</DL><HR><PRE>command completed.");
		}
	    }
	    if ((csb->mode == config_mode) ||
		test_mode_config_subcommand(csb)) {
	    }
	}
	if (print_title) {
	    if (exit_mode && !(state->flags & HTTP_SUPRESS_CLI_FORM)) {
		if (!(csb->http_exited_name && *csb->http_exited_name)) {
		    csb->http_exited_name = "exec";
		}
		printf("\nCommand base-URL is: /%s/-", csb->http_exited_name);
		printf("\n<INPUT TYPE=HIDDEN NAME=%s VALUE=\"/%s/-\">",
				form_new_base_url, csb->http_exited_name);
	    }
	    /* Watch out if you are dinking with </FORM>.
	     * This may either end The Real Form, or an Empty Form that 
             * follows the single field entry Form generated in the 
	     * parse_cmd. It's always The Real Form if exit_mode.
	     */
            printf("\n</FORM>");
	    if (csb->in_help == PARSER_HELP) {
		printf("</DL>");
	    }
	    printf("<HR>\n</PRE></BODY></HTML>");
	    http_page_complete(state);
	}
	stdio->csb = NULL;
	SET_PRIV(stdio->privilege, old_priv);
	free_csb(&csb);
    }

    if (baseurl) {
	free(baseurl);
    }

    if (new_uri) {
	state->uri = old_uri;
	free(new_uri);
    }

    if (parser_http_debug) {
	buginf("\nFinished processing command");
    }
}

boolean http_parser_post (httpstate *state, parser_mode *mode)
{
    char *old_uri = state->uri;
    char *new_uri;
    char *expanded_uri;
    char *value;
    char *token;
    char *cp;
    char *command_url = NULL;
    char *base_url_mark;
    int expand;

    if (parser_http_debug) {
	buginf("\nhttp_parser_post URI = '%s'", state->uri);
    }

    if (!(command_url = http_get_form_variable(state, form_new_base_url))) {
	if (!(command_url = http_get_form_variable(state, 
						form_new_command_url))) {
	    command_url = http_get_form_variable(state, form_command_url);
	}
    }
    if (!command_url && parser_http_debug) {
        buginf("\nhttp_parser_post no command url in form");
    }
    if (command_url) {
        value = http_get_form_variable(state, form_command);
        if (!value) {
	    value = "";
        }
        if (parser_http_debug) {
	    buginf(" command variable = '%s'", value);
        }
        /* space for command_url/value */
        new_uri = malloc(strlen(command_url) + 1 + 
			strlen(value) + strlen(uri_cr) + 1 + EXPANSION); 
        if (!new_uri) {
	    return(FALSE);
        } else {
	    *new_uri = '\0';
	}
	strcpy(new_uri, command_url);
	strcat(new_uri, "/");
    } else {
        token = strrchr(state->uri, '/');
        if (!token || !*(token+1)) {
	    return(FALSE);
        }
        token++;

        value = http_get_form_variable(state, token);
        if (!value) {
	    return(FALSE);
        }

        if (parser_http_debug) {
	    buginf(" token variable = '%s'", value);
        }

        new_uri = malloc(strlen(state->uri) + strlen(value) + 1 + EXPANSION);

        if (!new_uri) {
	    return(FALSE);
        } else {
	    *new_uri = '\0';
        }

        sstrncpy(new_uri, state->uri, (token - state->uri) + 1);
    }

    /* Handle expansion space requirements */
    cp = value;
    expand = 0;
    while (*cp) {
	/* 
	 * Don't check %, it shrinks or breaks even during translation.
         */
	if (*cp == '/') {
	    expand++;
	}
	cp++;
    }
    if (expand > EXPANSION) { /* you lose... */
        if (parser_http_debug) {
	    buginf("\nexpand = '%d'", expand);
        }
        expanded_uri = malloc(strlen(new_uri) + strlen(value) + 
				strlen(uri_cr) + 1 + expand);
	if (!expanded_uri) {
	    free(new_uri);
	    return(FALSE);
        } else {
	    *expanded_uri = '\0';
	}
	strcpy(expanded_uri,new_uri);
	free(new_uri);
	new_uri = expanded_uri;
    }

    cp = new_uri;
    while (*cp) {
	cp++;
    }
    while (*value) {
	switch (*value) {
	  case ' ':
	  case '+':
	    *cp = '/';
	    break;
	  case '%':
	    if (*(value+1) && *(value+2)) {
		value++;
		*cp = atohex(value);
		value++;
		if (*cp == '/') {
		    *cp++ = '\\';
		    *cp = '/';
		}
	    } else {
		*cp = *value;
	    }
	    break;
	  case '/':
	    *cp++ = '\\';
	    *cp = '/';
	    break;
	  default:
	    *cp = *value;
	    break;
	}
	cp++;
	value++;
    }
    *cp = '\0';

    state->uri = new_uri;

    if (command_url) {
        strcat(new_uri, uri_cr);
	state->uri_index = state->uri;
        token = http_get_uri_token(state); /* get mode */
	if (token) {
	    mode = get_mode_byname(token, MODE_SILENT);
	    if (mode == NULL) {
    		state->uri = old_uri;
		free(new_uri);
		return(FALSE);
	    }
	}
    }

    /* If base_url wasn't specified, supress the CLI Form */
    base_url_mark = strstr(state->uri_index, "/-");
    if ((!base_url_mark) || ((*(base_url_mark+2) != '/') &&
                                        ((*(base_url_mark+2) != '\0')))) {
        state->flags |= HTTP_SUPRESS_CLI_FORM;
    }

    http_parser(state, mode, TRUE);
    state->uri = old_uri;
    free(new_uri);

    return(TRUE);
}
