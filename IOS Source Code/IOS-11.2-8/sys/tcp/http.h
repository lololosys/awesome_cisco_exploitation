/* $Id: http.h,v 3.2.4.6 1996/07/12 17:37:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/http.h,v $
 *------------------------------------------------------------------
 * http.h -- Definitions for HTTP
 *
 * July 1995 Tony Li
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: http.h,v $
 * Revision 3.2.4.6  1996/07/12  17:37:18  widmer
 * CSCdi61607:  HTTP server uses non-optimal subsystem lookup
 * Branch: California_branch
 *
 * Revision 3.2.4.5  1996/06/19  18:30:41  widmer
 * CSCdi56931:  Length of the text fields are high
 * Branch: California_branch
 * Add maxlength to ezsetup form variables
 *
 * Revision 3.2.4.4  1996/06/09  22:15:13  rpratt
 * CSCdi59969:  Adding /- token to user URL, if missing, gives weird
 * context
 * Branch: California_branch
 *
 * Revision 3.2.4.3  1996/05/23  19:53:45  nkwok
 * CSCdi57859:  HTTP data verification
 * Branch: California_branch
 *
 * Revision 3.2.4.2  1996/05/22  04:45:47  snijsure
 * CSCdi58178:  HTTP problem with flash access
 * Branch: California_branch
 * CSCdi58178: HTTP problem with flash access
 *
 * Revision 3.2.4.1  1996/05/06  17:18:03  widmer
 * CSCdi56491:  long http tokens can crash router
 * Branch: California_branch
 * Don't write off end of array if token is too long.
 *
 * Revision 3.2  1996/03/09  22:57:38  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.1  1996/02/11  04:25:51  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

typedef enum httptoktype_ {
    TOK_NULL,				/* No token present */
    TOK_EOI,				/* End of input */
    TOK_STRING,				/* String token in body */
    TOK_SPACE,				/* Whitespace */
    TOK_CRLF,				/* Line terminator */
    TOK_OTHER,				/* Any other junk */
} httptoktype;

#define	HTTP_MAX_URI_LEN	256
#define HTTP_MAX_TOKEN_LEN 	50
#define	HTTP_IDLE_TIMEOUT	(3 * ONEMIN)
#define HTTP_UNSAFE_CHARS       "<>\"#%{}|\\^~[]"

#define HTTP_MAJOR_VERSION      1
#define HTTP_MINOR_VERSION      0

#define HTTP_DALLYSECS		2


/*
 * Macros for indicies into the http_status array.
 * These MUST be kept in order.
 */

#define HTTP_STATUS_OK 			1
#define HTTP_STATUS_CREATED 		HTTP_STATUS_OK+1
#define HTTP_STATUS_ACCEPTED 		HTTP_STATUS_CREATED+1
#define HTTP_STATUS_PROVISIONAL_INFORMATION HTTP_STATUS_ACCEPTED+1
#define HTTP_STATUS_NO_CONTENT 		HTTP_STATUS_PROVISIONAL_INFORMATION+1
#define HTTP_STATUS_MULTIPLE_CHOICES 	HTTP_STATUS_NO_CONTENT+1
#define HTTP_STATUS_MOVED_PERMANENTLY 	HTTP_STATUS_MULTIPLE_CHOICES+1
#define HTTP_STATUS_MOVED_TEMPORARILY 	HTTP_STATUS_MOVED_PERMANENTLY+1
#define HTTP_STATUS_METHOD 		HTTP_STATUS_MOVED_TEMPORARILY+1
#define HTTP_STATUS_NOT_MODIFIED 	HTTP_STATUS_METHOD+1
#define HTTP_STATUS_BAD_REQUEST 	HTTP_STATUS_NOT_MODIFIED+1
#define HTTP_STATUS_UNAUTHORIZED 	HTTP_STATUS_BAD_REQUEST+1
#define HTTP_STATUS_PAYMENT_REQUIRED 	HTTP_STATUS_UNAUTHORIZED+1
#define HTTP_STATUS_FORBIDDEN 		HTTP_STATUS_PAYMENT_REQUIRED+1
#define HTTP_STATUS_NOT_FOUND 		HTTP_STATUS_FORBIDDEN+1
#define HTTP_STATUS_METHOD_NOT_ALLOWED 	HTTP_STATUS_NOT_FOUND+1
#define HTTP_STATUS_NONE_ACCEPTABLE 	HTTP_STATUS_METHOD_NOT_ALLOWED+1
#define HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED HTTP_STATUS_NONE_ACCEPTABLE+1
#define HTTP_STATUS_REQUEST_TIMEOUT HTTP_STATUS_PROXY_AUTHENTICATION_REQUIRED+1
#define HTTP_STATUS_CONFLICT 		HTTP_STATUS_REQUEST_TIMEOUT+1
#define HTTP_STATUS_GONE 		HTTP_STATUS_CONFLICT+1
#define HTTP_STATUS_INTERNAL_SERVER_ERROR HTTP_STATUS_GONE+1
#define HTTP_STATUS_NOT_IMPLEMENTED 	HTTP_STATUS_INTERNAL_SERVER_ERROR+1
#define HTTP_STATUS_BAD_GATEWAY 	HTTP_STATUS_NOT_IMPLEMENTED+1
#define HTTP_STATUS_SERVICE_UNAVAILABLE HTTP_STATUS_BAD_GATEWAY+1
#define HTTP_STATUS_GATEWAY_TIMEOUT 	HTTP_STATUS_SERVICE_UNAVAILABLE+1

/*
 * Subsystem definitons
 */
#define HTTP_MAJVERSION 	1
#define HTTP_MINVERSION 	0
#define HTTP_EDITVERSION  	1

#define	HTTP_MAX_FORM_VARIABLES	100

#define	HTTP_REQUEST_UNKNOWN	0
#define	HTTP_REQUEST_GET	1
#define	HTTP_REQUEST_POST	2

#define	HTTP_INPUT_STRING	1
#define	HTTP_INPUT_PASSWORD	2

/*
 * httpstate_.flags definitions
 */
#define	HTTP_NO_FLAGS		0
#define	HTTP_SUPRESS_CLI_FORM	1
#define HTTP_REFRESH_HOME_PAGE  2

#define HTTP_CHAP_PASSWD_SET    "password already set"

typedef struct http_form_ {
    char name[HTTP_MAX_TOKEN_LEN+1];
    char value[HTTP_MAX_TOKEN_LEN+1];
    const char *error;
} http_form;

struct httpstate_ {
    httptoktype token_type;
    char	token[HTTP_MAX_TOKEN_LEN+1];
    char	*lexoutptr;		/* Next byte in token to write */
    char	*lexinptr;		/* Next byte in stream to read */
    ushort	lexcount;		/* Bytes left to be read */
    uint	lexlen;			/* Current length of token */
    char	*uri;
    char	*uri_index;		/* Current parse position in uri */
    char        *authorization;         /* Holds authorization string */
    tcbtype	*tcb;
    http_form	**form;
    uint	form_length;
    uint	flags;
};

extern const char logo_url[];

extern void http_parser_init(void);
extern char *http_get_form_variable(httpstate *, const char *);
extern void http_set_form_variable(httpstate *, const char *, const char *, ...);
extern boolean http_parser_post(httpstate *, parser_mode *);
extern void http_parser(httpstate *, parser_mode *, boolean);
extern void http_send_prologue(httpstate *, int, boolean, boolean);
extern void http_page_complete(httpstate *);
extern char *http_get_uri_token(httpstate *);
extern void http_send_not_found(httpstate *, const char *);
extern void http_free_form(http_form **);
extern void http_form_print_variable(httpstate *, uint, uint,
				     const char *, const char *);
extern void http_form_print_error(httpstate *, const char *msg);
extern void http_form_add(httpstate *, const char *, const char *, ...);
extern uint http_token_count(const char *);

extern void http_send_flash_file(httpstate *state);
extern boolean http_send_home_page(httpstate *state);
extern void http_error(httpstate *state, int error);
extern void http_send_prefix(httpstate *state, const char *type, uint len);

/*
 * tcp/http_ezsetup.c
 * EZ Setup functions
 */
extern void http_ezsetup_error(httpstate *, boolean *,
			       const char *, const char *);
