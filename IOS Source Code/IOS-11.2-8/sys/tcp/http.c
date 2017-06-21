/* $Id: http.c,v 3.2.4.22 1996/08/13 18:47:02 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http.c,v $
 *------------------------------------------------------------------
 * http.c -- HyperText Transfer Protocol
 *
 * July 1995 Tony Li and Greg Christy
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http.c,v $
 * Revision 3.2.4.22  1996/08/13  18:47:02  widmer
 * CSCdi65503:  security hole when using httpd and no enable passwd is set
 * Branch: California_branch
 * Make http authentication work with "enable use-tacacs"
 *
 * Revision 3.2.4.21  1996/08/07  09:02:49  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.4.20  1996/08/02  01:01:20  rpratt
 * CSCdi64878:  Platform link on home page twice
 * Branch: California_branch
 *
 * Revision 3.2.4.19  1996/07/12  17:37:17  widmer
 * CSCdi61607:  HTTP server uses non-optimal subsystem lookup
 * Branch: California_branch
 *
 * Revision 3.2.4.18  1996/07/06  05:56:58  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.2.4.17  1996/06/19  18:30:38  widmer
 * CSCdi56931:  Length of the text fields are high
 * Branch: California_branch
 * Add maxlength to ezsetup form variables
 *
 * Revision 3.2.4.16  1996/06/18  16:56:42  ahh
 * CSCdi60283:  HTTP: closing a connection prematurely crashes the router
 * Remove the unnecessary tcp_close(), the connection will be closed
 * when the tty gets closed down.
 * Branch: California_branch
 *
 * Revision 3.2.4.15  1996/06/18  14:47:57  nkwok
 * CSCdi57950:  Show dialer should be removed from non-ISDN router
 * Branch: California_branch
 * Move 'Show dialer status' from http_send_root_page() to isdn_http_root()
 *
 * Revision 3.2.4.14  1996/06/18  00:29:08  nkwok
 * CSCdi57945:  EZSetup name need changing
 * Branch: California_branch
 * Add sub-system setup registry
 *
 * Revision 3.2.4.13  1996/06/09  22:15:11  rpratt
 * CSCdi59969:  Adding /- token to user URL, if missing, gives weird
 * context
 * Branch: California_branch
 *
 * Revision 3.2.4.12  1996/05/23  19:53:44  nkwok
 * CSCdi57859:  HTTP data verification
 * Branch: California_branch
 *
 * Revision 3.2.4.11  1996/05/22  04:45:46  snijsure
 * CSCdi58178:  HTTP problem with flash access
 * Branch: California_branch
 * CSCdi58178: HTTP problem with flash access
 *
 * Revision 3.2.4.10  1996/05/21  10:06:16  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.4.9  1996/05/19  00:24:54  rpratt
 * CSCdi57306:  HTML config has problems in new platform
 * Branch: California_branch
 *
 * Revision 3.2.4.8  1996/05/13  15:03:43  widmer
 * CSCdi57075:  http access-group should deny tcp connection
 * Branch: California_branch
 * Move access check to http_access_check were it belongs
 *
 * Revision 3.2.4.7  1996/05/06  23:02:13  jjohnson
 * CSCdi56492:  Need to limit access to http server with access-list
 * code review comment
 * Branch: California_branch
 *
 * Revision 3.2.4.6  1996/05/06  18:00:15  widmer
 * CSCdi56662:  connect from web browser crashed router
 * Branch: California_branch
 * treat \n the same as \r\n in http header
 *
 * Revision 3.2.4.5  1996/05/06  17:39:56  widmer
 * CSCdi56492:  Need to limit access to http server with access-list
 * Branch: California_branch
 * Add "ip http access-class" command
 *
 * Revision 3.2.4.4  1996/05/06  17:18:01  widmer
 * CSCdi56491:  long http tokens can crash router
 * Branch: California_branch
 * Don't write off end of array if token is too long.
 *
 * Revision 3.2.4.3  1996/04/29  15:23:26  widmer
 * CSCdi55741:  Change router home page reference from CIO to CCO
 * Branch: California_branch
 *
 * Revision 3.2.4.2  1996/04/22  16:11:20  widmer
 * CSCdi53936:  IOS HTML Home page has wrong phone number for TAC
 * Branch: California_branch
 *
 * Revision 3.2.4.1  1996/03/18  22:13:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/16  07:40:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  22:57:37  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:49  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "sched.h"
#include "ctype.h"
#include "subsys.h"
#include "login.h"
#include "name.h"
#include "stdarg.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../ip/ip_registry.h"
#include "../os/clock.h"
#include "../ui/service.h"
#include "ttysrv.h"
#include "tcp.h"
#include "connect.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "http.h"
#include "http_debug.h"
#include "../util/md5_crypt.h"
#include "../ip/ipautoaddr.h"
#include "sys_registry.h"
#include "../os/async.h"
#include "http_parser_defs.h"
#include "../os/nv.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "aaa.h"
#include "../h/auth_registry.regh"
#include <stdlib.h>
#include "logger.h"
#include "../os/msg_sched.c"
#include "../os/os_debug.h"

struct http_status_string {
    uint code;
    const char *string;
};

static const struct http_status_string http_status[] = {
    { HTTP_STATUS_OK, "200 OK" },
    { HTTP_STATUS_CREATED, "201 Created" },
    { HTTP_STATUS_ACCEPTED, "202 Accepted" },
    { HTTP_STATUS_PROVISIONAL_INFORMATION, "203 Provisional Information" },
    { HTTP_STATUS_NO_CONTENT, "204 No Content" },
    { HTTP_STATUS_MULTIPLE_CHOICES, "300 Multiple Choices" },
    { HTTP_STATUS_MOVED_PERMANENTLY, "301 Moved Permanently" },
    { HTTP_STATUS_MOVED_TEMPORARILY, "302 Moved Temporarily" },
    { HTTP_STATUS_METHOD, "303 Method" },
    { HTTP_STATUS_NOT_MODIFIED, "304 Not Modified" },
    { HTTP_STATUS_BAD_REQUEST, "400 Bad Request"},
    { HTTP_STATUS_UNAUTHORIZED, "401 Unauthorized" },
    { HTTP_STATUS_PAYMENT_REQUIRED, "402 Payment Required" },
    { HTTP_STATUS_FORBIDDEN, "403 Forbidden" },
    { HTTP_STATUS_NOT_FOUND, "404 Not Found" },
    { HTTP_STATUS_METHOD_NOT_ALLOWED, "405 Method Not Allowed"},
    { HTTP_STATUS_NONE_ACCEPTABLE, "406 None Acceptable" },
    { HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED,
	  "407 Proxy Authentication Required" },
    { HTTP_STATUS_REQUEST_TIMEOUT, "408 Request Timeout" },
    { HTTP_STATUS_CONFLICT, "409 Conflict" },
    { HTTP_STATUS_GONE, "410 Gone" },
    { HTTP_STATUS_INTERNAL_SERVER_ERROR, "500 Internal Server Error" },
    { HTTP_STATUS_NOT_IMPLEMENTED, "501 Not Implemented" },
    { HTTP_STATUS_BAD_GATEWAY, "502 Bad Gateway" },
    { HTTP_STATUS_SERVICE_UNAVAILABLE, "503 Service Unavailable" },
    { HTTP_STATUS_GATEWAY_TIMEOUT, "504 Gateway Timeout" },
    { 0, NULL }
};

struct http_mime_header {
    const char *extension;	/* Filename extension */
    const char *type;		/* Content-type */
};


static const struct http_mime_header mime_header[] = {
	/* Some of the popular image formats */
        { "html", "text/html" },
        { "htm", "text/html" },
	{ "gif",  "image/gif" },
	{ "jpeg", "image/jpeg" },
	{ "jpg", "image/jpeg" },
	{ "jpe", "image/jpeg" },
	/* Some of the popular audio formats */
	{ "au", "audio/basic" },
	{ "snd", "audio/basic" },
	{ "wav", "audio/x-wav" },
	{ "class", "application/octet-stream" },
	{ NULL, NULL }
};


/* const char *logo_url = "<img SRC=\"//wwwin.cisco.com/CustAdv/InfoSys/Tools/imagelib/cisco-logo.gif\" ALT=\"CiscoWeb\" align=\"center\" width=150>"; */

const char logo_url[] = "<H1>Cisco Systems</H1>";

typedef enum {
    HTTP_SERVER_DISABLED,
    HTTP_SERVER_AUTO_ON,
    HTTP_SERVER_ENABLED
} server_status;

static server_status http_server_status;
static ushort http_port;
static int http_access_class;
static boolean subsys_exists_http_platform;


/*
 * http_get_status
 * Given http status code, return http status string
 */
static const char *http_get_status (uint status)
{
    uint i;

    for (i=0; http_status[i].string; i++) {
	if (status == http_status[i].code) {
	    return(http_status[i].string);
	}
    }
    return("UNKNOWN");
}

/*
 * http_advance
 * Add one byte to the accumulated token and advance pointers.
 */

static inline void http_advance (httpstate *state, uchar c)
{
    if (state->lexlen <= HTTP_MAX_TOKEN_LEN) {
	*state->lexoutptr++ = c;
    }
    state->lexlen++;
    state->lexinptr++;
    state->lexcount--;
}

/*
 * http_get_token
 * Get a token from the input stream, classify it and return.
 */

static void http_get_token (httpstate *state)
{
    uchar *ptr;
    int status;
    int count;
    uchar c;
    ulong major;
    ulong minor;

    if (state->token_type == TOK_EOI)
	return;
    state->token_type = TOK_NULL;	/* Note that we have nothing. */
    state->lexoutptr = state->token;	/* Point to start of token. */
    state->lexlen = 0;			/* Zero length token */

    while (TRUE) {
	/*
	 * Before we can get a token, we need to be able to get a byte from
	 * the input stream.
	 */
	while (state->lexcount == 0) {
	    if (tcp_unread(state->tcb)) {
		status = tcp_getstring(state->tcb, &ptr, &count, MAXINT);
		if (status != OK)
		    break;
		if (count) {
		    state->lexcount = count;
		    state->lexinptr = ptr;
		    break;
		}
	    }
	    if (tcp_closing(state->tcb) || !SLEEPING(state->tcb->dallytime)) {
		state->token_type = TOK_EOI;
		return;
	    }
	    TIMER_START(state->tcb->dallytime, HTTP_IDLE_TIMEOUT);
	    process_wait_for_event();
	    while (process_get_wakeup(&major, &minor)) {
		switch (major) {
		  case DIRECT_EVENT:
		  case TIMER_EVENT:
		    /* We have input or timed out */
		    break;
		  default:
		    errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		    state->token_type = TOK_EOI;
		    return;
		}
	    }
	}

	/*
	 * We now have useful data in the input buffer.  Scan it and
	 * possibly add it to the current token.
	 */
	c = *state->lexinptr;
	if (isalnum(c)) {

	    /*
	     * Alphanumerics are part of a string.
	     */
	    switch (state->token_type) {
	    case TOK_NULL:
	    case TOK_STRING:
		state->token_type = TOK_STRING;
		http_advance(state, c);
		break;
	    case TOK_CRLF:
		state->token_type = TOK_OTHER; /* Wierd, fall through */
	    default:
		goto emit;
	    }
	} else if (c == '\r') {

	    /*
	     * CR is the start of a CRLF.
	     */
	    switch (state->token_type) {
	    case TOK_NULL:
		state->token_type = TOK_CRLF;
		http_advance(state, c);
		break;
	    case TOK_CRLF:
		state->token_type = TOK_OTHER; /* Wierd, fall through */
	    default:
		goto emit;
	    }
	} else if (c == '\n') {

	    /*
	     * LF is the end of a CRLF.
	     */
	    switch (state->token_type) {
	    case TOK_CRLF:
		http_advance(state, c);
		goto emit;
	    case TOK_NULL:
		/* Treat \n the same as \r\n */
		state->token_type = TOK_CRLF;
		http_advance(state, c);
		goto emit;
	    default:
		goto emit;
	    }
	} else if (isspace(c)) {
	    switch (state->token_type) {
	    case TOK_NULL:
	    case TOK_SPACE:
		state->token_type = TOK_SPACE;
		http_advance(state, c);
		break;
	    case TOK_CRLF:
		state->token_type = TOK_OTHER; /* Wierd, fall through */
	    default:
		goto emit;
	    }
	} else {
	    switch (state->token_type) {
	    case TOK_NULL:
		state->token_type = TOK_OTHER;
		http_advance(state, c);
		goto emit;
	    default:
		goto emit;
	    }
	}
    }	

 emit:
    *state->lexoutptr = '\0';
    state->lexoutptr = state->token;

    /*
     * This is generally overkill.
     */
    if (http_token_debug) {
	buginf("\nHTTP: token len %d: '", state->lexlen);
	for (c = *state->lexoutptr++; c; c = *state->lexoutptr++) {
	    if (isprint(c)) {
		buginf("%c", c);
	    } else {
		buginf("\\%o", c);
	    }
	}
	buginf("'");
    }
}


/*
 * http_token_count - Get token count
 */

uint http_token_count (const char *pszName)
{
  uint i;
  const char *pszWorkName = pszName;

  if ((NULL == pszName) || ('\0' == *pszName))
    return 0;

  while (isspace(*pszWorkName))
    pszWorkName++;

  if ('\0' == *pszWorkName)
    return 0;

  for (i = 1;; i++) {
    while (*pszWorkName && !isspace(*pszWorkName))
      pszWorkName++;

    if ('\0' == *pszWorkName)
      break;

    while (isspace(*pszWorkName))
      pszWorkName++;

    if ('\0' == *pszWorkName)
      break;
  }

  return i;
}


char *http_get_uri_token (httpstate *state)
{
    char *slash;
    char *out;

    while (*state->uri_index == '/') {
	state->uri_index++;
    }

    slash = state->uri_index;
    while (1) {
	slash = strchr(slash, '/');
	if  (!slash ||
	     (slash == state->uri_index) ||
	     (*(slash-1) != '\\')) {
	    break;
	}
	slash++;
    }

    if (slash) {
	out = string_getnext();
	sstrncpy(out, state->uri_index, (slash - state->uri_index)+1);
	state->uri_index = slash;
    } else {
	out = state->uri_index;
	while (*state->uri_index) {
	    state->uri_index++;
	}
    }
    return(out);
}

/*
 *
 */
int http_strcasecmp (const char *s, const char *hs)
{
    if ((!s && hs) || (s && !hs)) /* no match if only one ptr is NULL */
        return((int)(s - hs));
    if (s == hs)               /* match if both ptrs the same (e.g. NULL) */
        return(0);
    while (*s && *hs) {
	if (toupper(*s) == toupper(*hs)) {
	    s++;
	    hs++;
	} else {
	    if ((*hs == '\\') &&
		(toupper(*s) == toupper(*(hs+1)))) {
		s++;
		hs += 2;
	    } else {
		break;
	    }
	}
    }
    return(toupper(*s) - toupper(*hs));
}

/*
 * http_page_complete
 * Finish off any page.
 */

void http_page_complete (httpstate *state)
{
    printf("\n\n");
    tcp_putpush(state->tcb, TRUE, FALSE);
}

/*
 * http_send_status
 * Send a status response
 */
static void http_send_status (httpstate *state, int status)
{
    clock_epoch curtime;

    clock_get_time(&curtime);
    printf("HTTP/%d.%d %s\n"
	   "Date: %CC\n",
	   HTTP_MAJOR_VERSION, HTTP_MINOR_VERSION,
	   http_get_status(status),
	   "%tw, %d %tn %Y %T %tZ", &curtime);
    if (status == HTTP_STATUS_OK) {
	printf("Server: cisco-IOS/%d.%d HTTP-server/%d.%d(%d)\n",
	       SYS_MAJORVERSION, SYS_MINORVERSION, HTTP_MAJVERSION,
	       HTTP_MINVERSION, HTTP_EDITVERSION);
    }
}

/*
 * http_error
 * Generate an error message.
 */

void http_error (httpstate *state, int error)
{
    http_send_prologue(state, error, TRUE, TRUE);
    printf("<H1>%s</H1>\n", http_get_status(error));
    http_page_complete(state);
    tcp_putpush(state->tcb, TRUE, FALSE);
}

/*
 * http_send_prologue
 * Send a prologue to real data.
 */

void http_send_prologue (httpstate *state, int status,
			 boolean complete, boolean expired)
{
    http_send_status(state, status);
    printf("Content-type: text/html\n");

    if (expired)
        printf("Expires: Thu, 16 Feb 1989 00:00:00 GMT\n");

    if (complete)
	printf("\n");
}


/* 
 * Send the appropriate mime prefix based on type of the file. 
 * The prefix includes following
 *        Content-type:
 *        Content-length:
 *        Content-Transfer-Encoding
 * If the specified extension is not found in the static lookup table
 * mime_header, Content-type of text/plain is sent. 
 * Input :
 *       char *type : Filename extension.
 *       int  len   : Length of the file.
 * Return:
 *        Return TRUE if encoding should be binary else returns false.
 */
void http_send_prefix (httpstate *state, const char *extension, uint len)
{
    int n = 0;
    boolean found = FALSE;

    http_send_status(state, HTTP_STATUS_OK);

    while (extension && mime_header[n].extension) {
        if (strcmp(mime_header[n].extension,extension) == 0) {
             printf("Content-type: %s\n"
		    "Content-length: %d\n"
		    "Content-Transfer-Encoding: binary\n",
		    mime_header[n].type, len);
	     found = TRUE;
	     break;
	}
	n++;
    }

    if (found == FALSE) {
        printf("Content-type: text/plain\n");
        printf("Content-length: %d\n",len);
    }
    printf("\n");
}


/*
 * http_send_root_page
 * Send the proper HTML incantations to display the root page.  This is the
 * "top" of the entire hierarchy.
 */

static void http_send_root_page (httpstate *state)
{
    ipaddrtype addr = state->tcb->localhost.ip_addr;

    /* Check if we have html file with name home.html in flash */
    if ( http_send_home_page(state) == TRUE ) {
        http_page_complete(state);
        return;
    }
    http_send_prologue(state, HTTP_STATUS_OK, TRUE, FALSE);
    printf("<HTML><HEAD><TITLE>%s Home Page</TITLE></HEAD>"
	   "\n<BODY>%s<H2>Accessing Cisco %s \"%s\"</H2>"
	   "\n<MENU><DL>",
	   hostname, logo_url,
	   platform_get_string(PLATFORM_STRING_NOM_DU_JOUR), hostname);

    reg_invoke_http_setup_startup(state);

    printf("\n<P>"
	   "\n<DT><A HREF=telnet://%i>Telnet</A>"
	   " - to the router.<P>"
	   "\n<DT><A HREF=http:/exec/show/interfaces/CR>Show interfaces</A>"
	   " - display the status of the interfaces.",
	   addr, addr);

    printf("\n<DT><A HREF=http:/exec/show/log/CR>Show diagnostic log</A>"
	   " - display the diagnostic log."
	   "\n<DT><A HREF=http:/exec/->Monitor the router</A>"
	   " - HTML access to the command line interface.");

    if (domain_server[0]) {
	printf("<DT><A HREF=http:/exec/ping/%i/CR>"
	       "Connectivity test</A> - ping the nameserver.",
	       domain_server[0]);
    }

    if (subsys_exists_http_platform) {
	printf("\n<DT><A HREF=http:/platform>Platform</A>"
	       " - platform utilities."
 	       " Send comments to cs-html (below).");
    }

    printf("\n<P>"
	   "\n<DT><A HREF=http:/exec/show/tech-support/cr>Show tech-support</A>"
	   " - display information commonly needed by tech support.");

    reg_invoke_http_root(state);


    printf("\n</DL></MENU>"
	   "\n<HR>"
	   "\n<H3>Help resources</H3>"
	   "\n<OL>"
	   "\n<LI><A HREF=http://www.cisco.com>CCO at www.cisco.com</A>"
	   " - Cisco Connection Online, including the Technical "
	   "Assistance Center (TAC)."
	   "\n<LI><A HREF=mailto:tac@cisco.com>tac@cisco.com</A>"
	   " - e-mail the TAC."
	   "\n<LI><B>1-800-553-2447 or +1-408-526-7209</B> - phone the TAC."
	   "\n<LI><A HREF=mailto:cs-html@cisco.com>cs-html@cisco.com</A>"
	   " - e-mail the HTML interface development group."
	   "\n</OL>"
	   "\n</BODY></HTML>"
	   ,hostname, logo_url,
	   platform_get_string(PLATFORM_STRING_NOM_DU_JOUR),
	   hostname, addr, addr, addr, addr, addr, addr, addr,addr);
    http_page_complete(state);
}

/*
 * http_send_not_found
 * Reject request because the URL is invalid
 */

void http_send_not_found (httpstate *state, const char *reason)
{
    printf("<HEAD><TITLE>404 Not Found</TITLE></HEAD>"
	   "<BODY><H1>404 Not Found</H1>"
	   "The requested URL %s was not found on this server. %s</BODY>",
	   state->uri, (reason && reason[0]) ? reason : NULL);
    http_page_complete(state);
}

/*
 * http_get_action
 * Respond to a GET request.
 */

static void http_get_action (httpstate *state)
{
    parser_mode *mode;
    char *token;
    char *base_url_mark;

    token = http_get_uri_token(state);

    if (http_transaction_debug) {
	buginf("\nHTTP: received GET '%s'", token);
    }

    /* If base_url wasn't specified, supress the CLI Form */
    base_url_mark = strstr(state->uri_index, "/-");
    if ((!base_url_mark) || ((*(base_url_mark+2) != '/') && 
					((*(base_url_mark+2) != '\0')))) {
	state->flags |= HTTP_SUPRESS_CLI_FORM;
    }

    /*
     * Send the appropriate page based on the URI.
     */
    if (!token[0]) {
	http_send_root_page(state);
    } else if (!strcasecmp(token, "platform")) {
        if (!reg_invoke_platform_http_page_send(state)) {
	    http_error(state, HTTP_STATUS_NOT_IMPLEMENTED);
	}
    } else if (!strcasecmp(token, "ezsetup")) {
	if (!reg_invoke_http_ezsetup_page_send(state)) {
	    http_error(state, HTTP_STATUS_NOT_IMPLEMENTED);
	}
    } else if (!strcasecmp(token,"flash")) {
        http_send_flash_file(state);
    } else if ((mode = get_mode_byname(token, MODE_SILENT))) {
	http_parser(state, mode, TRUE);
    } else if (!reg_invoke_http_get(state, token)) {
	http_send_not_found(state, NULL);
    }
}

/*
 * decode base-64 ASCII encoding as described in RFC1521
 */
static const int pr2six[256]={
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,62,64,64,64,63,
  52,53,54,55,56,57,58,59,60,61,64,64,64,64,64,64,64,0,1,2,3,4,5,6,7,8,9,
  10,11,12,13,14,15,16,17,18,19,20,21,22,23,24,25,64,64,64,64,64,64,26,27,
  28,29,30,31,32,33,34,35,36,37,38,39,40,41,42,43,44,45,46,47,48,49,50,51,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,64,
  64,64,64,64,64,64,64,64,64,64,64,64,64
};

/*
 * Table used to covert a base64 encoded character to its appropriate
 * 6-bit value.  -1 is returned on illegal input values.
 */
static const char base64_conv[256] = {
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,62, -1,-1,-1,63,
    52,53,54,55, 56,57,58,59, 60,61,-1,-1, -1,-1,-1,-1,
    -1, 0, 1, 2,  3, 4, 5, 6,  7, 8, 9,10, 11,12,13,14,
    15,16,17,18, 19,20,21,22, 23,24,25,-1, -1,-1,-1,-1,
    -1,26,27,28, 29,30,31,32, 33,34,35,36, 37,38,39,40,
    41,42,43,44, 45,46,47,48, 49,50,51,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1,
    -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1, -1,-1,-1,-1
};

/*
 * http_uudecode
 * Decode a buffer of base 64 data per RFC1521.
 */
static boolean http_uudecode (uchar *inbuf, uchar *outbuf, int outbufsize)
{
    int c[4];
    boolean good = TRUE;
    boolean done = FALSE;

    *outbuf = '\0';

    /*
     * Leave room for the terminating null
     */
    outbufsize--;

    while (!done) {
	c[0] = *inbuf++;
	if (c[0] == '\0') {
	    done = TRUE;
	    continue;
	}

	/* skip preliminary garbage */
	if ((c[0] != '=') && (base64_conv[c[0]] == -1))
	    continue;
	/*
	 * Grab four characters from the input buffer.  Base-64
	 * streams should always come in blocks of four chars and not
	 * have illegal values.
	 */
	c[1] = *inbuf++;
	if (c[1] == '\0') {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	c[2] = *inbuf++;
	if (c[2] == '\0') {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	c[3] = *inbuf++;
	if (c[3] == '\0') {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	/*
	 * Only the third and forth character should ever be
	 * padding ('=').
	 */
	if ((c[0] == '=') || (c[1] == '=')) {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}
	
	c[0] = base64_conv[c[0]];
	c[1] = base64_conv[c[1]];

	if (c[0] == -1 || c[1] == -1) {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	if (outbufsize) {
	    *outbuf++ = (c[0] << 2) | (c[1] >> 4);
	    outbufsize--;
	} else {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	if (c[2] == '=') {
	    done = TRUE;
	    continue;
	}

	c[2] = base64_conv[c[2]];

	if (c[2] == -1) {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	if (outbufsize) {
	    *outbuf++ = (c[1] << 4) | (c[2] >> 2);
	    outbufsize--;
	} else {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	if (c[3] == '=') {
	    done = TRUE;
	    continue;
	}

	c[3] = base64_conv[c[3]];

	if (c[3] == -1) {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}

	if (outbufsize) {
	    *outbuf++ = (c[2] << 6) | c[3];
	    outbufsize--;
	} else {
	    good = FALSE;
	    done = TRUE;
	    continue;
	}
    }

    *outbuf = '\0';
    return(good);
}

/*
 * http_send_unauthorized
 * Reject request and ask for authorization info.
 */

static void http_send_unauthorized (httpstate *state)
{
    http_send_prologue(state, HTTP_STATUS_UNAUTHORIZED, FALSE, TRUE);
    printf("WWW-Authenticate: Basic realm=\"access\"\n\n");
    printf("<HEAD><TITLE>Authorization Required</TITLE></HEAD>"
	   "<BODY><H1>Authorization Required</H1>"
	   "Browser not authentication-capable or"
	   " authentication failed.</BODY>");
    http_page_complete(state);
}

/*
 * http_authorize
 * Validate authorization, if necessary.
 *
 * XXX - We need to deal with ALL enable password methods (md5,
 *       tacacs, etc)
 *
 */


static boolean http_authorize (httpstate *state)
{
    char tmp[(HTTP_MAX_TOKEN_LEN*2)+2];
    char username[HTTP_MAX_TOKEN_LEN+1], password[HTTP_MAX_TOKEN_LEN+1];
    char *cptr;
    int i;
    char *reason;

    /*
     * No enable password set, let it through.
     */
    if (old_access_control &&
	!enable_use_login_server &&
	!enable_password[PRIV_MAX].pw &&
	!enable_secret[PRIV_MAX].pw) {
	return(TRUE);
    }

    /*
     * They should have sent authorization info, but didn't.
     */
    if (!state->authorization) {
	http_send_unauthorized(state);
	if (http_transaction_debug)
	    buginf("\nHTTP: requesting authorization");
	return(FALSE);
    }

    /*
     * Decode username:password
     */
    if (!http_uudecode(state->authorization, tmp,
		       (HTTP_MAX_TOKEN_LEN*2)+2)) {
	http_error(state, HTTP_STATUS_BAD_REQUEST);
	return(FALSE);
    }

    /*
     * Get username and ignore it for now.
     */
    cptr = tmp;
    i = 0;

    while ((i <= HTTP_MAX_TOKEN_LEN) && (*cptr != ':')) {
	username[i++] = *cptr++;
    }
    username[i] = '\0';

    /*
     * Get password;
     */
    cptr++;

    password[0] = '\0';
    i = 0;
    while ((i <= HTTP_MAX_TOKEN_LEN) && *cptr) {
	password[i++] = *cptr++;
    }
    password[i] = '\0';

    if (old_access_control) {
	if (enable_use_login_server) {
	    reason = NULL;
	    if (reg_invoke_login_enable_pw(stdio, username, password,
					   &reason, PRIV_MAX)) {
		return(TRUE);
	    }
	} else {
	    if (enable_secret[PRIV_MAX].pw) {
		if (md5_crypt_compare(enable_secret[PRIV_MAX].pw,
				      password, ENCRYPT_NONE)) {
		    return(TRUE);
		}
	    } else {
		if (passwdcheck(password, enable_password[PRIV_MAX].pw,
				enable_password[PRIV_MAX].encrypt_type)) {
		    return(TRUE);
		}
	    }
	}
    } else {
	authenstruct authen;
	boolean ret;
	char port[16];
	char *rem = tty_get_remote_addr(stdio);

	memset(&authen, 0, sizeof(authen));
	authen.tty = stdio;
	sprintf(&port[0], "tty%d", stdio->ttynum);

	ret = aaa_create_user(&stdio->ustruct, NULL, NULL, port, rem,
			      AAA_AUTHEN_TYPE_ASCII, AAA_SVC_LOGIN, 0);
	if (ret) {
	    ret = aaa_start_login(stdio->ustruct, &authen, ACC_LIST_DEFAULT,
				  AAA_AUTHEN_ACT_LOGIN, AAA_SVC_LOGIN);
	    if (!ret) {
		/* Can't find any authen mechanism, fail right away */
		authen.status = AAA_AUTHEN_STATUS_FAIL;
	    }
	    i = 0;
	    while ((authen.status == AAA_AUTHEN_STATUS_GETUSER) ||
		   (authen.status == AAA_AUTHEN_STATUS_GETPASS)) {
		if (authen.status == AAA_AUTHEN_STATUS_GETUSER) {
		    if (username && username[0]) {
			authen.user_reply = username;
		    } else {
			authen.status = AAA_AUTHEN_STATUS_ERROR;
			if (aaa_authen_debug) {
			    buginf("\nAAA/AUTHEN/HTTP (%u): "
				   "username required, none given",
				   authen.session_id);
			}
		    }
		} else {
		    authen.user_reply = password;
		}
		aaa_cont_login(stdio->ustruct, &authen);
		authen.user_reply = NULL;
		i++;
		if (i > 2) {
		    authen.status = AAA_AUTHEN_STATUS_ERROR;
		    if (aaa_authen_debug) {
			buginf("\nAAA/AUTHEN/HTTP (%u): "
			       "possible loop in login request",
			       authen.session_id);
		    }
		}
	    }
	    ret = (authen.status == AAA_AUTHEN_STATUS_PASS);
	    aaa_cleanup_login(stdio->ustruct, &authen);
	} else {
	    http_error(state, HTTP_STATUS_INTERNAL_SERVER_ERROR);
	    return(FALSE);
	}

	if (ret &&
	    aaa_author_needed(AAA_AUTHOR_TYPE_SHELL, 0, NULL, stdio)) {

	    authorstruct author;
	    int avail = 0;
	    char *attr;
	    char *val;
	    enum AV_TYPE type;
	    int tmp_priv = -1;

	    bzero(&author, sizeof(authorstruct));
	    author.auth_tty = stdio;

	    /* Build minimal AV list */
	    ret = (aaa_add_AV(&author.out_args, &avail, AAA_ATTR_service,
			      AAA_VAL_SVC_shell, AAA_AV_MAND) &&
		   aaa_add_AV(&author.out_args, &avail, AAA_ATTR_cmd,
			      AAA_VAL_null, AAA_AV_OPT));

	    if (!ret) {
		http_error(state, HTTP_STATUS_INTERNAL_SERVER_ERROR);
		return(FALSE);
	    }
	    
	    /* We've been authenticated, now check authorization */
	    ret = aaa_do_author(stdio->ustruct, &author,
				AAA_AUTHOR_TYPE_SHELL, 0);
	    if (!ret) {
		http_error(state, HTTP_STATUS_INTERNAL_SERVER_ERROR);
		return(FALSE);
	    }
	    switch (author.status) {
	      case AAA_AUTHOR_STATUS_PASS_ADD:
		if (!author.in_args) {
		    break;
		}
		aaa_merge_attrs(author.out_args, author.in_args, &author.in_args);
		/* FALL THROUGH */
	      case AAA_AUTHOR_STATUS_PASS_REPL:
		if (!author.in_args) {
		    break;
		}
		for (i = 0; ret && author.in_args[i]; i++) {
		    if (aaa_split_AV(author.in_args[i], &attr, &val, &type)) {
			if (!strcmp(attr, AAA_ATTR_priv_lvl)) {
			    tmp_priv = atoi(val);
			    if ((tmp_priv < PRIV_MIN) || (tmp_priv > PRIV_MAX)) {
				if (type == AAA_AV_MAND) {
				    ret = FALSE;
				}
				tmp_priv = -1;
			    }
			}
		    }
		}
		break;
	      default:
		ret = FALSE;
		break;
	    }
	    if (ret && ((tmp_priv == -1) || (tmp_priv < PRIV_MAX))) {
		ret = FALSE;
	    }
	    aaa_cleanup_author(&author);
	} else {
	    if (ret) {
		/* authorization needed, but authorization type not set */
		ret = FALSE;
	    }
	}

	if (ret) {
	    return(TRUE);
	}
    }

    http_send_unauthorized(state);
    return(FALSE);
}

void http_form_print_variable (httpstate *state, uint type, uint max_length,
			       const char *prompt, const char *name)
{
    const char *type_string;
    char *form_value = NULL;
    const char **error_string = NULL;
    int i;

    if (state->form) {
	for (i=0; i<HTTP_MAX_FORM_VARIABLES; i++) {
	    if (!state->form[i]) {
		break;
	    }
	    if (strcasecmp(state->form[i]->name, name) == 0) {
		if (state->form[i]->error) {
		    error_string = &(state->form[i]->error);
		}
		form_value = state->form[i]->value;
		break;
	    }
	}
    }

    switch (type) {
      case HTTP_INPUT_STRING:
      default:
	type_string = "STRING";
	break;
      case HTTP_INPUT_PASSWORD:
	type_string = "PASSWORD";
	break;
    }

    if (error_string) {
	printf("\n<A NAME=%s_tag><HR>", name);
	http_form_print_error(state, *error_string);
	printf("\n</A><HR>");
	*error_string = NULL;
    }
    printf("\n<DT>%s <INPUT TYPE=%s NAME=%s VALUE=\"%s\"",
	   prompt, type_string, name, form_value);
    if (max_length) {
	printf(" MAXLENGTH=%u", max_length);
    }
    printf(">");
}

void http_form_print_error (httpstate *state, const char *msg)
{
    printf("\n<H1><I>- %s</I></H1>", msg);
}

void http_form_add (httpstate *state, const char *name,
		    const char *fmt, ...)
{
    va_list args;

    va_start(args, fmt);
    state->form[state->form_length] = malloc(sizeof(http_form));
    if (state->form[state->form_length]) {
	strcpy(state->form[state->form_length]->name, name);
	vsprintf(state->form[state->form_length]->value, fmt, args);
	state->form_length++;
    }
    va_end(args);
}

void http_free_form (http_form **form)
{
    uint i;

    if (form) {
	for (i=0; i<HTTP_MAX_FORM_VARIABLES && form[i]; i++) {
	    free(form[i]);
	}
	free(form);
    }
}

/*
 * http_post_action
 * Respond to a POST request.
 */

static boolean http_post_action (httpstate *state)
{
    char *name, *value;
    int len;
    parser_mode *mode;
    char *src;
    char *dst;
    boolean char_escape;

    if (http_transaction_debug)
	buginf("\nHTTP: received POST '%s' %d",
	       state->uri, state->token_type);

    /*
     * Grab and try to parse the data block.
     *
     * Format:      <varname> = <value> & ... CRLF
     */

    state->form = malloc(sizeof(http_form)*HTTP_MAX_FORM_VARIABLES);
    if (!state->form) {
	return(FALSE);
    }

    /*
     * Skip initial CR/LF
     */
    if (state->token_type == TOK_CRLF) {
	http_get_token(state);
    }

    while (TRUE) {
	if (state->token_type == TOK_EOI) {
	    return(FALSE);
	}

	state->form[state->form_length] = malloc(sizeof(http_form));
	if (!state->form[state->form_length]) {
	    return(FALSE);
	}
	name = state->form[state->form_length]->name;
	value = state->form[state->form_length]->value;

	/*
	 * Parse the name of the variable.
	 */
	name[0] = '\0';
	len = HTTP_MAX_TOKEN_LEN;
	while ((state->token_type != TOK_OTHER) ||
	       (state->token[0] != '=')) {
	    if (state->token_type == TOK_EOI) {
		return(FALSE);
	    }

	    len -= strlen(state->token);
	    if (len >= 0)
		strcat(name, state->token);
	    http_get_token(state);

	}
	if (http_transaction_debug)
	    buginf("\nHTTP: parsed variable '%s'", name);
	state->form_length++;
	http_get_token(state);

	/*
	 * Parse its value.
	 */
	value[0] = '\0';
	len = HTTP_MAX_TOKEN_LEN;
	char_escape = FALSE;
	while ((state->token_type != TOK_OTHER) ||
	       (state->token[0] != '&')) {
	    if (state->token_type == TOK_EOI) {
		return(FALSE);
	    }

	    if (state->token_type == TOK_CRLF)
		goto done;

	    len -= strlen(state->token);
	    if (len >= 0) {
		dst = value;
		while (*dst) {
		    dst++;
		}
		src = state->token;
		while (*src) {
		    switch (*src)  {
		      case '+':
			*dst = ' ';
			char_escape = FALSE;
			break;
		      case '%':
			if (*(src+1) && *(src+2)) {
			    src++;
			    *dst = atohex(src);
			    src++;
			    char_escape = FALSE;
			} else {
			    char_escape = TRUE;
			}
			break;
		      default:
			if (char_escape && *src && *(src+1)) {
			    *dst = atohex(src);
			    src++;
			} else {
			    *dst = *src;
			}
			char_escape = FALSE;
			break;
		    }
		    src++;
		    dst++;
		}
		*dst = '\0';
	    }
	    http_get_token(state);
	}
	if (http_transaction_debug)
	    buginf("\nHTTP: parsed value '%s'", value);

	http_get_token(state);
    }

  done:
    if (!strcasecmp(state->uri, "/ezsetup/set")) {
	if (!reg_invoke_http_ezsetup_page_post(state)) {
	    http_error(state, HTTP_STATUS_NOT_IMPLEMENTED);
	}
	return(TRUE);
    }
    if (!reg_invoke_http_post(state)) {
	char *mode_name = http_get_uri_token(state);

	mode = get_mode_byname(mode_name, MODE_SILENT);
	if (mode) {
	    boolean ret = http_parser_post(state, mode);

	    return(ret);
	} else {
	    if (parser_http_debug) {
		buginf("\nCan't find mode '%s' for '%s'",
		       mode_name, state->uri);
	    }
	}
    }
    return(FALSE);
}

char *http_get_form_variable (httpstate *state, const char *variable)
{
    int i;

    if (state->form) {
	for (i=0; i<HTTP_MAX_FORM_VARIABLES; i++) {
	    if (!state->form[i]) {
		return(NULL);
	    }
	    if (strcasecmp(state->form[i]->name, variable) == 0) {
		return(state->form[i]->value);
	    }
	}
    }
    return(NULL);
}

void http_set_form_variable (httpstate *state,
			     const char *variable,
			     const char *fmt, ...)
{
    va_list args;
    int i;

    if (state->form) {
	for (i=0; i<HTTP_MAX_FORM_VARIABLES; i++) {
	    if (!state->form[i]) {
		return;
	    }
	    if (strcasecmp(state->form[i]->name, variable) == 0) {
		va_start(args, fmt);
		vsprintf(state->form[i]->value, fmt, args);
		va_end(args);
		return;
	    }
	}
    }
}

/*
 * http_parse_uri
 * Parse up through and including "HTTP".  Return FALSE on error.
 */

static boolean http_parse_uri (httpstate *state)
{
    char tmp[HTTP_MAX_URI_LEN+1];
    int len = HTTP_MAX_URI_LEN;
    char *src;
    int src_len;

    if (state->token_type == TOK_EOI)
	return(FALSE);

    /*
     * We're at the start of a line.  If we get a CRLF, then something
     * is wrong.
     */
    if (state->token_type == TOK_CRLF)
	return(FALSE);

    /*
     * Parse the uri.
     */
    tmp[0] = '\0';
    while (state->token_type != TOK_SPACE) {
	if (state->token_type == TOK_EOI)
	    return(FALSE);
	len -= strlen(state->token);
	if (len >= 0)
	    strcat(tmp, state->token);
	http_get_token(state);
    }

    /*
     * decode and %xx's
     */
    src = strchr(tmp, '%');
    while (src != NULL) {
        if ((*src == '%') && *(src+1) && *(src+2)) {
            *src = atohex(src+1);
            src++;
            src_len = strlen(src+2);
            memmove(src, src+2, src_len+1); /* src_len+1 includes the '\0' */
        }
        src = strchr(src, '%');
    }

    state->uri = strdup(tmp);
    state->uri_index = state->uri;
    if (http_transaction_debug) {
	buginf("\nHTTP: parsed uri '%s'", state->uri);
    }

    return(TRUE);
}

/*
 * http_parse_extensions
 * Parse extensions that we don't grok.  Bail out if we get just a CRLF.
 */

static boolean http_parse_extensions (httpstate *state)
{
    char tmp[(HTTP_MAX_TOKEN_LEN*2)+2];
    int len;

    while (TRUE) {
	if (state->token_type == TOK_EOI)
	    return(FALSE);

	/*
	 * We're at the start of a line.  If we get a CRLF, then we're
	 * done.
	 */
	if (state->token_type == TOK_CRLF) {
	    return(TRUE);
	}

	/*
	 * Parse the name of the extension.
	 */
	tmp[0] = '\0';
	len = HTTP_MAX_TOKEN_LEN;
	while ((state->token_type != TOK_OTHER) ||
	       (state->token[0] != ':')) {
	    if (state->token_type == TOK_EOI)
		return(FALSE);
	    len -= strlen(state->token);
	    if (len >= 0)
		strcat(tmp, state->token);
	    http_get_token(state);
	}
	if (http_transaction_debug)
	    buginf("\nHTTP: parsed extension %s", tmp);

	http_get_token(state);

	if (!strcasecmp(tmp, "authorization")) {
	    /*
	     * We have an authorization extension.
	     *
	     * Format:   Authorization: <scheme> <param>
	     */

	    while (state->token_type == TOK_SPACE)
		http_get_token(state);

	    /*
	     * Grab authorization type.
	     */
	    if (state->token_type != TOK_STRING)
		return(FALSE);

	    if (http_transaction_debug)
		buginf("\nHTTP: parsed authorization type %s", state->token);

	    /*
	     * Since we asked for "Basic" we should have gotten it.
	     */

	    if (strcasecmp(state->token, "basic")) {
		return(FALSE);
	    }

	    http_get_token(state);

	    while (state->token_type == TOK_SPACE) {
		http_get_token(state);
	    }

	    /*
	     * Parse up through the CRLF.
	     */
	    tmp[0] = '\0';
	    len = HTTP_MAX_TOKEN_LEN;
	    while (state->token_type != TOK_CRLF) {
		if (state->token_type == TOK_EOI)
		    return(FALSE);
		len -= strlen(state->token);
		if (len >= 0)
		    strcat(tmp, state->token);
		http_get_token(state);
	    }
	    state->authorization = strdup(tmp);
	} else {
	    /*
	     * Parse up through the CRLF.
	     */
	    tmp[0] = '\0';
	    len = HTTP_MAX_TOKEN_LEN;
	    while (state->token_type != TOK_CRLF) {
		if (state->token_type == TOK_EOI)
		    return(FALSE);
		len -= strlen(state->token);
		if (len >= 0)
		    strcat(tmp, state->token);
		http_get_token(state);
	    }
	    if (http_transaction_debug)
		buginf("\nHTTP: parsed line %s", tmp);
	}
	http_get_token(state);
    }
}

/*
 * http_parse_request_body
 * Parse the body of a request.  Return FALSE on error.
 */

static boolean http_parse_request_body (httpstate *state)
{
    uint major, minor;
    boolean ok;

    /*
     * Syntax: SP URI SP "HTTP" "/" DIGIT "." DIGIT CRLF <fancy>
     *					       CRLF CRLF <fancy>
     *         SP URI CRLF
     */
    if (state->token_type != TOK_SPACE)
	return(FALSE);

    /*
     * Go parse the URI.
     */
    http_get_token(state);
    if (!http_parse_uri(state))
	return(FALSE);

    if (http_uri_debug) {
	buginf("\nHTTP: processing URL '%s' from host %i",
	       state->uri, state->tcb->foreignhost.ip_addr);
    }

    /*
     * If we got a CRLF this is a simple request.
     */
    if (state->token_type == TOK_CRLF)
	return(TRUE);
    if (state->token_type != TOK_SPACE)
	return(FALSE);
    http_get_token(state);

    /*
     * Parse "HTTP".
     */
    if ((state->token_type != TOK_STRING) ||
	(strcasecmp(state->token, "http")))
	return(FALSE);
    http_get_token(state);

    /*
     * Parse the '/'.
     */
    if ((state->token_type != TOK_OTHER) ||
	(state->token[0] != '/'))
	return(FALSE);
    http_get_token(state);

    /*
     * Parse the version number.
     */
    if (state->token_type != TOK_STRING)
	return(FALSE);
    major = parse_unsigned(state->token, &ok);
    http_get_token(state);
    if ((state->token_type != TOK_OTHER) ||
	(state->token[0] != '.'))
	return(FALSE);
    http_get_token(state);
    if (state->token_type != TOK_STRING)
	return(FALSE);
    minor = parse_unsigned(state->token, &ok);
    http_get_token(state);
    if (http_transaction_debug)
	buginf("\nHTTP: client version %d.%d", major, minor);

    /*
     * Parse the CRLF.
     */
    if (state->token_type != TOK_CRLF)
	return(FALSE);
    http_get_token(state);

    /*
     * Now, parse the extensions.
     */
    if (!http_parse_extensions(state))
	return(FALSE);

    return(TRUE);
}

/*
 * http_parse_get_request
 * Parse a GET request.  Note that we've already parsed the "get" keyword,
 * and we should be looking at a space.
 */

static boolean http_parse_get_request (httpstate *state)
{
    /*
     * Syntax: "GET" <body>
     */

    if (!http_parse_request_body(state)) {
	http_error(state, HTTP_STATUS_BAD_REQUEST);
	return(FALSE);
    }
    if (!http_authorize(state)) {
	if (http_transaction_debug)
	    buginf("\nHTTP: authorization rejected");
	return(FALSE);
    }

    /*
     * Successful request.  Do something interesting.
     */
    http_get_action(state);
    return(TRUE);
}

/*
 * http_parse_post_request
 * Parse a POST request.  Note that we've already parsed the "post" keyword,
 * and we should be looking at a space.
 */

static boolean http_parse_post_request (httpstate *state)
{
    /*
     * Syntax: "POST" <body>
     */
    if (!http_parse_request_body(state)) {
	http_error(state, HTTP_STATUS_BAD_REQUEST);
	return(FALSE);
    }
    if (!http_authorize(state)) {
	if (http_transaction_debug)
	    buginf("\nHTTP: authorization rejected");
	return(FALSE);
    }
    if (!http_post_action(state)) {
	http_error(state, HTTP_STATUS_BAD_REQUEST);
	return(FALSE);
    }
    return(TRUE);
}

/*
 * http_parse_request
 * Parse an entire request.  Currently very stupid.
 */

static void http_parse_request (httpstate *state)
{
    /*
     * If we don't see a string with "GET" or "POST", we don't know about it.
     */
    while ((state->token_type == TOK_SPACE) ||
	   (state->token_type == TOK_CRLF))
	http_get_token(state);
    if (state->token_type != TOK_STRING) {
	http_error(state, HTTP_STATUS_BAD_REQUEST);
	return;
    }

    if (!strcasecmp(state->token, "get")) {
	http_get_token(state);
	(void) http_parse_get_request(state);
    } else  if (!strcasecmp(state->token, "post")) {
	http_get_token(state);
	(void) http_parse_post_request(state);
	http_free_form(state->form);
    } else {
	http_error(state, HTTP_STATUS_NOT_IMPLEMENTED);
    }
}

/*
 * http_daemon
 * The HTTP server daemon process
 */

static process http_daemon (void)
{
    tcbtype *tcb;
    httpstate state;			/* State for this server */

    if (!process_get_arg_ptr((void**)&tcb)) {
	process_kill(THIS_PROCESS);
    }

    /*
     * HTTP client has apparently completed its address resolution attempt
     * for us.
     */
    reg_invoke_autoaddr_stop(TRUE);
    process_watch_timer(&tcb->dallytime, ENABLE);
    TIMER_START(tcb->dallytime, HTTP_IDLE_TIMEOUT);

    /*
     * State initialization.  Because this is a _stack_ variable, not
     * malloc'ed, it is not zeroed automagically.
     */
    memset(&state, 0, sizeof(state));
    state.token_type = TOK_NULL;
    state.lexoutptr = state.token;
    state.tcb = tcb;
    state.flags = HTTP_NO_FLAGS;

    /*
     * Go get the first token.  Note that this parser always assumes
     * that a routine is trying to process the _current_ token.
     */
    http_get_token(&state);
    if (state.token_type != TOK_EOI) {
	http_parse_request(&state);
    }

    if (state.uri) {
	free(state.uri);
    }
    if (state.authorization) {
	free(state.authorization);
    }
    death(tcb->tty);
}

/*
 * http_listen
 * Start a new HTTP task when someone opens a connection to us.  This is
 * really gross because we also set up all of the IO state here.
 */

static void http_listen (tcbtype *tcb)
{
    int line;
    tt_soc *tty;
    conntype *conn;		/* connection pointer */

    line = get_vtyline(WWW_PORT, &tcp_connvector);

    if (line <= 0) {
	if (http_transaction_debug)
	    buginf("\nHTTP: out of lines");
	tcp_close(tcb);
	return;
    }
    tty = tty_allocate(line, TRUE);
    if (!tty) {
	if (http_transaction_debug)
	    buginf("\nHTTP: out of ttys");
	tcp_close(tcb);
	return;
    }
    conn = getconnection(tty, &tcp_connvector);
    if (conn == NULL) {
	if (http_transaction_debug)
	    buginf("\nHTTP: out of connections");
	tcp_close(tcb);
	tty_deallocate(tty);
	return;
    }	
    conn->tcb = tcb;
    tcb->tty = tty;
    tcb->ttynum = tty->ttynum;		/* set line number */
    conn->proto.tn.hostaddr = tcb->foreignhost;	/* remember host address */
    address_copy(&conn->address, &tcb->foreignhost);
    if (tcp_keepalivesin) {
	tcb->flags |= TCB_KEEPALIVE;
    }
    unqueue(&tty->connhead, conn);	/* remove from general q */
    tty->statbits &= ~(CARDROP+HANGUP+IDLE); /* reincarnated again! */
    tty->statbits |= READY;
    tty->conntop = conn;		/* remember the driver conn */
    tty->conncount++;			/* another use */
    tty->ovector = &TCPvector;
    tty_savedefaults(tty);		/* Turn off automore */
    tty->tty_length = 0;
    tty->capabilities &= ~TRANSPARENT;        /* Turn off telnet transparent */
    setdatabits(tty, 8);              /* Set everything to 8-bit */
    tty->tty_datamask = 0xff;
    tty->tty_execmask = 0xff;

    tcb->pid = process_create(http_daemon, "HTTP Server",
			      LARGE_STACK, PRIO_LOW);
    if (tcb->pid != NO_PROCESS) {
	process_set_arg_ptr(tcb->pid, tcb);
	process_set_ttysoc(tcb->pid, tty);
	tty->tiptop = tcb->pid;
    } else {
	tty_deallocate(tty);
	tcp_abort(tcb);
    }
}

/*
 * http_access_check
 * Return true if we should talk to this client.
 */

static boolean http_access_check (ulong sport, ulong dport, ipaddrtype
				  address)
{
    return((http_server_status != HTTP_SERVER_DISABLED) &&
	   fast_check(http_access_class, address));
}

void http_config_command (parseinfo *csb)
{
    ushort new_http_port;

    if (csb->nvgen) {
	switch (csb->which) {
	  case HTTP_SERVER:
	    nv_write(http_server_status == HTTP_SERVER_ENABLED,
		     "%s", csb->nv_command);
	    break;
	  case HTTP_PORT:
	    nv_write(http_port != WWW_PORT, "%s %d",
		     csb->nv_command, http_port);
	    break;
	  case HTTP_ACCESS:
	    nv_write(http_access_class, "%s %d",
		     csb->nv_command, http_access_class);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case HTTP_SERVER:
	http_server_status =
	    csb->sense ? HTTP_SERVER_ENABLED : HTTP_SERVER_DISABLED;
	break;
      case HTTP_PORT:
	new_http_port = csb->sense ? GETOBJ(int,1) : WWW_PORT;
	if (new_http_port != http_port) {
	    reg_delete_tcp_listen(http_port);
	    reg_delete_tcp_access_check(http_port);
	    http_port = new_http_port;
	    reg_add_tcp_access_check(http_port, http_access_check,
				     "http_access_check");
	    reg_add_tcp_listen(http_port, http_listen, "http_listen");
	}
	break;
      case HTTP_ACCESS:
	http_access_class = csb->sense ? GETOBJ(int,1) : 0;
	break;
    }
}


/*
 * http_init
 *
 * Initialize HTTP subsystem
 */
static void http_init (subsystype *subsys)
{
    nvtype *nv;

    http_server_status = HTTP_SERVER_DISABLED;
    if (subsys_find_by_name("http_ezsetup") && nvsize && (nv = nv_getptr())) {
	if (nv->magic != NVMAGIC) {
	    http_server_status = HTTP_SERVER_AUTO_ON;
	}
	nv_done(nv);
    }
    if (subsys_find_by_name("platform_http")) {
	subsys_exists_http_platform = TRUE;
    } else {
	subsys_exists_http_platform = FALSE;
    }
    http_port = WWW_PORT;
    http_access_class = 0;

    http_parser_init();
    http_debug_init();

    reg_add_tcp_access_check(http_port, http_access_check, "http_access_check");
    reg_add_tcp_listen(http_port, http_listen, "http_listen");

    reg_add_http_get_uri_token(http_get_uri_token, "http_get_uri_token");
    reg_add_http_get_form_variable(http_get_form_variable,
				   "http_get_form_variable");
    reg_add_http_send_not_found(http_send_not_found, "http_send_not_found");
    reg_add_http_parser(http_parser, "http_parser");
    reg_add_http_strcasecmp(http_strcasecmp, "http_strcasecmp");
}

/*
 * HTTP subsystem header
 */

SUBSYS_HEADER(http, HTTP_MAJVERSION, HTTP_MINVERSION,
	      HTTP_EDITVERSION, http_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost, subsys_req_iphost);
