/* $Id: login.h,v 3.3.20.4 1996/07/17 19:11:13 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/h/login.h,v $
 *------------------------------------------------------------------
 * login.h -- 
 *
 * December 1990, Bill Westfield
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: login.h,v $
 * Revision 3.3.20.4  1996/07/17  19:11:13  sjackson
 * CSCdi49571:  AAA user name is not passed in login get address requests
 * User name determined from the authentication method in use
 * Branch: California_branch
 *
 * Revision 3.3.20.3  1996/06/08  19:41:41  che
 * CSCdi57850:  password encrytion not working correctly for vty
 * Branch: California_branch
 * Implement proper handling of line password structures when
 * password encryption is turned on.
 *
 * Revision 3.3.20.2  1996/05/10  01:37:34  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.3.20.1  1996/04/15  14:57:42  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.3  1996/01/29  00:30:12  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.2  1995/11/17  09:12:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/09/25  08:34:20  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.3  1995/08/30  19:56:29  vandys
 * CSCdi38886:  Longer usernames needed
 * Extend, while maintaining compat with named things
 *
 * Revision 2.2  1995/08/04  23:13:48  irfan
 * CSCdi38157:  memory leaks in PPP PAP, CHAP, Callback
 * fix memory leaks in pap_rrequest and chap_rresponse path towards
 * callback. change as many malloc's to malloc_named, as possible.
 *
 * Revision 2.1  1995/06/07  20:35:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "login_public.h"

struct password_struct_ {
    char *pw;
    uint encrypt_type;
};

/*
 * A username, and all associated attributes
 *
 * NB, the name[] is used in this structure compatibly with namedthingtype.
 * However, customers require more than the 20 bytes allowed by the
 * default definition NAMEDTHINGMACRO.
 */

#define UNAMELEN (64)			/* Max length of a local username */
struct usernametype_ {
    struct usernametype_ *next;
    char name[UNAMELEN+1];
    char password[PASSWORDLEN_ENCRYPTED+1];
    ulong flags;
    ulong accesslist;
    char *autocommand;
    hwidbtype *callback_incoming_idb;
    int callback_rotary_or_line;
    char *dial_string;
};

#define USER_ENCRYPTION  0x07	/* up to 8 types of encryption for passwd */
#define USER_NOPASSWORD  0x08	/* User does not require a password */
#define USER_AUTOCOMMAND 0x10	/* execute the command provided */
#define USER_NOESCAPE    0x20	/* undefine escape character, set autohangup */
#define USER_ACCESSLIST	 0x40	/* special access list for this user */
#define USER_NOHANGUP    0x80	/* restart login instead of hanging up */
#define USER_NOCALLBACK_VERIFY 0x100	/* no callback verification req'd */
#define USER_CALLBACK_DIALSTRING 0x200	/* callback dial string */
#define USER_CALLBACK_ROTARY 0x400	/* callback rotary number */
#define USER_CALLBACK_LINE 0x800	/* callback TTY line */
#define	USER_PRIV	 0xf0000000	/* User privilege level */

/*
 * types of encryption
 */
#define ENCRYPT_NONE 0
#define ENCRYPT_UNIX 1
#define ENCRYPT_MD5  5
#define ENCRYPT_TEMP 7

/*
 * Last resort codes.  What to do if no servers answer.
 */
#define TACACS_LAST_FAIL     0
#define TACACS_LAST_PASSWORD 1
#define TACACS_LAST_SUCCEED  2

/*
 * Number of times to retry the login server (TACACS)
 */
#define LOGIN_DEF_ATTEMPTS	3

/*
 * Number of seconds to wait for a login name or password
 */
#define LOGIN_DEF_TIMEOUT       30

/*
 * Local username check responses
 */
typedef enum password_check_type {
    PASSWORD_NO_USER,
    PASSWORD_NO_PASSWORD,
    PASSWORD_NOT_CORRECT,
    PASSWORD_CORRECT,
} password_check_type;


/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

/*
 * login.c
 */

extern boolean password_encryption_enable;
extern boolean enable_use_login_server;
extern boolean login_have_servers;
extern int login_server_attempts;
extern int login_input_timeout;
extern password_struct enable_password[];
extern password_struct enable_secret[];
extern int enable_lastresort;
extern queuetype userQ;

extern void login_init(void);
extern boolean check_login(tt_soc *);
extern boolean passwdcheck(char *, char *, uint);
extern void login_parser_init(void);
extern usernametype *find_passwd(char *, char *);
extern usernametype *duplicate_usernametype(usernametype *);
extern enum password_check_type check_local_password(char *, char *,
						     usernametype **);
extern void free_usernametype(usernametype *);
extern void clear_usernametype(usernametype *);
extern char *get_user_name(tt_soc *);
extern void nv_password(parseinfo *, char *, int, tt_soc *, password_struct *,
			char *, uint);
extern boolean do_exec_author(tt_soc *, boolean *);

#endif /* __LOGIN_H__ */
