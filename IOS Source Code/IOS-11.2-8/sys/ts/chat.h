/* $Id: chat.h,v 3.2.62.1 1996/08/15 21:54:48 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/chat.h,v $
 *------------------------------------------------------------------
 * chat.h - Chat script definitions
 *
 * April 28, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: chat.h,v $
 * Revision 3.2.62.1  1996/08/15  21:54:48  irfan
 * CSCdi62960:  chat scripts mishandle control chars in nvgen
 * Branch: California_branch
 * translate non-printable characters correctly when nvgen'ing.
 * use same translations for debug output.
 * don't trim non-blank characters while looking for delimiters.
 * use malloc_named instead of malloc.
 *
 * Revision 3.2  1995/11/17  18:54:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/28  09:32:05  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.2  1995/06/13  16:37:28  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.1  1995/06/07  23:09:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* Some constants */

#define CHAT_OUTPUT_TIMEOUT 60
#define CHAT_DEFAULT_TIMEOUT 5
#define CHAT_MAXEXPECT 128
#define CHAT_MAXSEND 128
#define CHAT_MAX_STR 256

/* Chat Script type-code..purpose of current activation */

enum CHAT_TYPE {
    SCRIPT_STARTUP,
    SCRIPT_CONNECTION,
    SCRIPT_DIALER_MODEM,
    SCRIPT_DIALER_SYSTEM,
    SCRIPT_RESET,
    SCRIPT_ACTIVATION,
    SCRIPT_CALLBACK,
    SCRIPT_ARAPCALLBACK,
    SCRIPT_OFFHOOK,
    SCRIPT_LINE_CHANGE,
    SCRIPT_USER
    };

/*
 * Layout of chat script entry
 */

struct chat_script_ {
    struct chat_script_ *next;	/* Link to next defined chat script */
    char *name;			/* Pointer to name of chat script */
    char *script;		/* Pointer to chat script itself */
    char data[0];		/* Data (name of script and script) */
};

/*
 * Layout of entry in a match list
 */

typedef struct chat_match_ {
    struct chat_match_ *next;	/* Link to next in match list */
    int len;			/* Length of match string */
    int match;			/* Characters matched so far */
    char pattern[0];		/* Match string itself */
} chat_match;

/*
 * Layout of a chat session block
 */

typedef struct chat_session_ {
    tt_soc *tty;		/* Pointer to associated tty */
    enum CHAT_TYPE chat_type;
    chat_script *modem_script;  /* Pointer to script running */
    chat_script *system_script; /* Next script if any */
    addrtype addr;              /* protocol address of remote node */

} chat_session;

/*
 * Macro to traverse all chat scripts defined in the system
 */

#define FOR_ALL_CHAT_SCRIPTS(chat_ptr) \
    for ((chat_ptr) = chat_script_head; (chat_ptr); \
	 (chat_ptr) = (chat_ptr)->next)

extern boolean chat_dial(hwidbtype *, char *, dialermaptype *);
extern boolean do_chat_script(tt_soc *, char *, regexp *, int, char *);
extern boolean get_script(int, char *, char **, regexp **);
extern void script_line_commands (parseinfo *);
extern void chat_scripts_init(subsystype *);
extern chat_script *find_chat_script(regexp *);
extern chat_script_add_or_delete(boolean, boolean, char *, char *);
extern void callback_init(void);
