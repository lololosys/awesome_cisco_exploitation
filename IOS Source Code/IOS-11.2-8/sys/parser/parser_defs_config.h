/*
 *	P A R S E R _ D E F S _ C O N F I G . H
 *
 * $Id: parser_defs_config.h,v 3.4.4.4 1996/09/05 01:18:12 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_config.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_config.h,v $
 * Revision 3.4.4.4  1996/09/05  01:18:12  sjackson
 * CSCdi67473:  big buffers created not trimmed (queued on TCP input with
 * rtelnet)
 * Enable coalescing of TCP input packets by default for reverse telnet
 * Branch: California_branch
 *
 * Revision 3.4.4.3  1996/07/04  00:29:00  irfan
 * CSCdi61001:  Autoselect throws away characters on startup
 * Branch: California_branch
 * Add "flush-at-activation" line subcommand to the ts system.
 *
 * Revision 3.4.4.2  1996/06/12  14:31:30  bjthomas
 * CSCdi38891:  need warning when line session-timeout nears
 * Branch: California_branch
 *
 * Revision 3.4.4.1  1996/03/27  10:44:13  irfan
 * CSCdi33144:  telnet should be configurable to refuse some options
 * Branch: California_branch
 * Add a bit in tty->capabilities to allow telnet/rlogin to silently
 * ignore remote host's flow control change requests.
 *
 * Revision 3.4  1996/03/09  22:57:07  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1996/01/12  20:04:28  sjackson
 * CSCdi44704:  modem busyout command is needed
 * Command added as an extension of the line modem autoconfigure
 *
 * Revision 3.2  1995/11/17  18:49:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:54:48  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:20:34  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/13  16:36:24  syiu
 * CSCdi35685:  Async callback rotary should be in reverse oder
 *
 * Revision 2.1  1995/06/07  22:29:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Banner defines */
#define CONF_BANNER		 0
#define BANNER_MOTD		 1
#define BANNER_INCOMING		 2
#define BANNER_EXEC		 3
#define BANNER_LOGIN		 4

/* Terminal and line parameter defines */
enum TERM_LINE_PARAMS {
    TERM_LENGTH,
    TERM_WIDTH,
    TERM_DNS,
    TERM_PADDING,
    TERM_MONITOR,
    TERM_ESCAPE,
    TERM_NOTIFY,
    TERM_TRANSPARENT,
    TERM_TYPE,
    LINE_ACCESS_CLASS,
    LINE_BANNER,
    LINE_MOTD_BANNER,
    LINE_EXECTIMEOUT,
    LINE_EXIT_CONFIG,
    LINE_LOC,
    LINE_LOGIN,
    LINE_NUMBER,
    LINE_PASSWORD,
    LINE_REFUSE,
    LINE_VACANT,
    LINE_EXTBAUD,
    LINE_INSECURE,
    LINE_LOCKABLE,
    LINE_SESSION_LIMIT,
    LINE_STOPBITS,
    LINE_AUTHEN,
    TERM_ESCAPE_BREAK,
    TERM_ESCAPE_NONE,
    TERM_ESCAPE_DEFAULT,
    TERM_IP_NETMASK_FORMAT,
    LINE_SESSIONTIMEOUT_WARNING,
    TERM_IP_TCP_QUEUE_SIZE,
};

#define	PARSER_LINE_TELNET	       100
#define	PARSER_LINE_PARITY	       101
#define	PARSER_LINE_DATABITS	       102
#define	PARSER_LINE_STOPBITS	       103
#define	PARSER_LINE_FLOW	       104
#define	PARSER_LINE_MODEM	       105
#define	PARSER_LINE_AUTOBAUD	       106
#define	PARSER_LINE_STOPCHAR	       107
#define	PARSER_LINE_STARTCHAR	       108
#define	PARSER_LINE_ROTARY	       109
#define	PARSER_LINE_ACTIVATECHAR       110
#define	PARSER_LINE_AUTOHANGUP	       111
#define	PARSER_LINE_CTSREQUIRED        112
#define	PARSER_LINE_DISCONNECTCHAR     113
#define	PARSER_LINE_DISPATCH_TIMEOUT   114
#define	PARSER_LINE_PRIVATE	       115
#define	PARSER_LINE_LOCKABLE	       116
#define	PARSER_LINE_SESSION_TIMEOUT    117
#define	PARSER_LINE_SPEED	       118
#define	PARSER_LINE_RXSPEED	       119
#define	PARSER_LINE_TXSPEED	       120
#define	PARSER_LINE_DISPATCHCHAR       121
#define	PARSER_LINE_HOLDCHAR	       122
#define	PARSER_LINE_SESSION_LIMIT      123
#define	PARSER_LINE_TRANSPORT	       124
#define	PARSER_LINE_EXEC	       125
#define	PARSER_LINE_TERM_TYPE	       126
#define	PARSER_LINE_BUSYSTRING	       127
#define	PARSER_LINE_LOGINSTRING	       128
#define	PARSER_LINE_USERNAME	       129
#define	PARSER_LINE_AUTOCOMMAND	       130
#define	PARSER_LINE_AUTOHOST	       131
#define	PARSER_LINE_DATAMASK	       132
#define	PARSER_LINE_SPECIALMASK	       133
#define	PARSER_LINE_EXEC_MASK	       134
#define	PARSER_LINE_HISTORY	       135
#define	PARSER_LINE_HISTORY_SIZE       136
#define	PARSER_LINE_EDITING	       137
#define PARSER_LINE_DOWNLOAD	       138
#define	PARSER_LINE_HELP	       139
#define PARSER_LINE_ABSOLUTE_TIMEOUT   140
#define PARSER_LINE_LOGOUT_WARNING     141
#define PARSER_LINE_AUTOSELECT	       142
#define PARSER_LINE_AUTOSELECTDEF      143

#define PARSER_LINE_SCRIPT_STARTUP      151  /* chat script: sys-startup */
#define PARSER_LINE_SCRIPT_CONNECTION   152  /* chat script: connection */
#define PARSER_LINE_SCRIPT_DIALER       154  /* chat script: dialer */
#define PARSER_LINE_SCRIPT_RESET        153  /* chat script: line off */
#define PARSER_LINE_SCRIPT_ACTIVATION   155  /* chat script: line off */
#define PARSER_LINE_SCRIPT_LINE_CHANGE  156  /* chat script: param change */
#define PARSER_LINE_SCRIPT_CALLBACK     157  /* chat script: callback */
#define PARSER_LINE_SCRIPT_ARAPCALLBACK 158  /* chat script: arap-callback */
#define PARSER_LINE_SCRIPT_OFFHOOK      159  /* chat script: offhook */

#define	PARSER_LINE_PRIVILEGE		160
#define	PARSER_LINE_MODEM_AUTOCONFIGURE	161

#define PARSER_LINE_MODEM_BUSYOUT	162
#define PARSER_LINE_FLOWPERMANENT       163  /* Flow control locked */
#define PARSER_LINE_FLUSH_ACTIVATE      164  /* clearinput at activation */

/* Modem autoconfigure */

#define MODEM_AUTOCONFIGURE_TYPE	0
#define MODEM_AUTOCONFIGURE_DISCOVERY	1

/* Modem busyout */
#define MODEM_BUSYOUT_TYPE		0

/* Configuration sources */
#define PARSER_CONF_NONE                 0      /* No source identified */
#define PARSER_CONF_TERM                 1      /* Config from terminal */
#define PARSER_CONF_MEM                  2      /* Config from NV memory */
#define PARSER_CONF_NET                  3      /* Config from a TFTP host */
#define PARSER_CONF_OVERWRITE_NET        4      /* Config from a TFTP host and o
verwrite config */
#define PARSER_CONF_BATCH                5      /* Not from terminal */
#define	PARSER_CONF_HTTP		 6	/* Config from HTTP server */


/* Enable defines */
#define ENABLE_USETACACS	 0
#define ENABLE_PASSWORD		 1
#define ENABLE_LASTRESORT	 2
#define ENABLE_SECRET		 3

/*
 *  Default value command
 */
#define	PARSER_DEFAULTCMD_DATAMASK		1
#define	PARSER_DEFAULTCMD_SPECIALMASK		2
#define	PARSER_DEFAULTCMD_EXECMASK		3
#define PARSER_DEFAULTCMD_MODINT		4

/* How many characters to save at the end of config buffers to guarantee
 * that "\nend" gets written.
 */
#define ENDSTRLEN   5

