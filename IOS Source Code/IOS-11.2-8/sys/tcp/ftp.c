/* $Id: ftp.c,v 3.9.10.7 1996/09/06 21:01:08 snyder Exp $
 * $Source: /release/112/cvs/Xsys/tcp/ftp.c,v $
 *------------------------------------------------------------------
 * ftp.c -- FTP client code
 *
 * May 1995, Andy Heffernan
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ftp.c,v $
 * Revision 3.9.10.7  1996/09/06  21:01:08  snyder
 * CSCdi68334:  dump unused spinner support, save memory
 *              412 image, 4 data
 * Branch: California_branch
 *
 * Revision 3.9.10.6  1996/07/06  05:56:56  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.9.10.5  1996/06/17  23:38:19  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.9.10.4  1996/06/08  19:44:01  che
 * CSCdi57850:  password encrytion not working correctly for vty
 * Branch: California_branch
 * Implement proper handling of line password structures when
 * password encryption is turned on.
 *
 * Revision 3.9.10.3  1996/05/21  10:06:14  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.9.10.2  1996/04/15  15:00:07  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.9.10.1  1996/03/18  22:13:23  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/16  07:40:33  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.2  1996/03/07  10:52:46  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  21:20:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.9  1996/02/23  23:38:49  ahh
 * CSCdi47841:  Running some telnet/pad sessions over serial causes rout...
 * Allow tcp_uniqueport() to fail, and clean up some debug messages.
 *
 * Revision 3.8  1996/02/22  14:38:55  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.7  1996/02/07  16:16:02  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.6  1996/01/29  00:30:35  billw
 * CSCdi13190:  Increase password characters encryted
 * Make sure that the maximum length password that can be encrypted is
 * the maximum length that will be accepted.
 *
 * Revision 3.5  1996/01/24  20:41:00  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.4  1996/01/16  03:37:49  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.3  1995/12/28  18:29:02  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.2  1995/12/17  18:39:15  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.1  1995/11/09  13:35:14  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/25  08:35:20  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ctype.h>
#include "interface_private.h"
#include "sys_registry.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "ttysrv.h"
#include "connect.h"
#include "logger.h"
#include "login.h"
#include "aaa.h"
#include "config.h"
#include "parser.h"
#include "subsys.h"
#include "file.h"
#include "../os/boot.h"
#include "../ui/command.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../os/nv.h"
#include "../ip/ip_registry.h"
#include "../dev/flash_registry.h"
#include "../tcp/tcp_debug.h"
#include "../tcp/telnet.h"
#include "../dev/monitor1.h"
#include "exception.h"
#include "../os/free.h"
#include "../parser/macros.h"
#include "../parser/parser_actions.h"
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_tcpip.h"
#include "../if/network.h"

static char *ftp_username;
static password_struct ftp_password;
static boolean ftp_passive_mode = TRUE;
static idbtype *ftp_source_idb;

#define FTP_BLOCKS_PER_BANG	10
#define FTP_WINDOW		(8 * 1024)

static boolean
ftp_move_data (tcbtype *tcb, char *buf, uint bufsize, boolean display)
{
    ulong bytes_left, bytes_written, writes;
    int error;

    /*
     * Transfer the actual file data.
     */
    writes = 0;

    error = ERROR_OK;
    bytes_left = bufsize;
    while (bytes_left) {
	bytes_written = tcp_putstring(tcb, buf, bytes_left);
	if (display)
	    file_xfer_progress(++writes, 10);
	if (bytes_written < bytes_left)
	    edisms((blockproc *) tcpputBLOCK, (ulong) tcb);
	if (tcp_closing(tcb)) {
	    error = ERROR_ABORT;
	    break;
	}
	buf += bytes_written;
	bytes_left -= bytes_written;
    }

    return(error == ERROR_OK);
}


#define FTP_EOF	-1
static int
ftp_getc (tcbtype *tcb)
{
    char ch;
    int status;

    tcp_read(tcb, &ch, 1, &status);
    if (status != OK)
	return(FTP_EOF);
    else
	return(ch);
}

static char ftp_last_reply[80];

static int
ftp_read_reply (tcbtype *tcb, int *statusx)
{
    int ch, status;
    int dig, code;
    boolean continuation;
    static char tn_reply[3] = { TN_IAC, TN_DONT, 0 };
    int reply_index;

    continuation = FALSE;
    while (1) {
	reply_index = 0;
	dig = code = 0;
	while ((ch = ftp_getc(tcb)) != '\n' && ch != -1) {
	    if (ch == TN_IAC) {
		ch = ftp_getc(tcb);
		switch (ch) {
		case TN_WILL:
		case TN_WONT:
		    tn_reply[1] = TN_DONT;
		    tn_reply[2] = ftp_getc(tcb);
		    tcp_write(tcb, tn_reply, 3, &status); 
		    break;
		case TN_DO:
		case TN_DONT:
		    tn_reply[1] = TN_WONT;
		    tn_reply[2] = ftp_getc(tcb);
		    tcp_write(tcb, tn_reply, 3, &status); 
		    break;
		case -1:
		    return(600);	/* XXX */
		}
		continue;
	    }

	    /* One more letter in the reply. */
	    dig++;
	    if (reply_index < sizeof(ftp_last_reply) - 1) {
		ftp_last_reply[reply_index++] = ch;
		ftp_last_reply[reply_index] = '\0';
	    }
	    
	    if (dig <= 3)
		code = code * 10 + (ch - '0');
	    else if (dig == 4)
		continuation = (ch == '-');
	}
	if (ch == -1)
	    return(600);	/* XXX */
	if (reply_index < sizeof(ftp_last_reply) - 1) {
	    ftp_last_reply[reply_index++] = ch;
	    ftp_last_reply[reply_index] = '\0';
	}
	if (!continuation)
	    return(code);
    }
}

static boolean
ftp_command (tcbtype *tcb, char *cmd, int *code)
{
    int status;

    if (tcpftp_debug)
	buginf("\nFTP: ---> %*s", strlen(cmd) - 2, cmd);
    if (tcp_write(tcb, cmd, strlen(cmd), &status) == -1)
	return(FALSE);
    *code = ftp_read_reply(tcb, &status);
    if (tcpftp_debug)
	buginf("\nFTP: %*s", strlen(ftp_last_reply) - 2, ftp_last_reply);
    return(*code < 400);
}

static boolean
ftp_write (char *name, addrtype *dest, uchar *buf, ulong bufsize,
	   void *junk1, void *junk2, void *junk3)
{
    boolean display = TRUE;
    int status, code;
    tcbtype *ctrl_tcb, *data_tcb, *listen_tcb;
    addrtype laddr, *lhost;
    uchar cmdbuf[80];
    boolean ok = TRUE;
    ushort fport, lport;
    ulong winsize;

    if (display) {
	printf("\nWriting %s ", name);
	flush();
    }

    if (ftp_source_idb && ftp_source_idb->ip_address &&
	interface_up(ftp_source_idb)) {
	laddr.type = ADDR_IP;
	laddr.ip_addr = ftp_source_idb->ip_address;
	lhost = &laddr;
    } else
	lhost = NULL;

    /*
     * Open control connection.
     */
    ctrl_tcb = tcp_open(NULL, FTP_PORT, dest, 0, lhost, &status, NULL);
    if (ctrl_tcb == NULL) {
	if (tcpftp_debug)
	    buginf("\nFTP: open failed -- %s", tcperror2string(status));
	return(FALSE);
    }
    code = ftp_read_reply(ctrl_tcb, &status);
    if (tcpftp_debug)
	buginf("\nFTP: %*s", strlen(ftp_last_reply) - 2, ftp_last_reply);
    if (code >= 400) {
	ok = FALSE;
	goto ftp_done_no_quit;
    }

    /*
     * Send "USER" command.
     */
    sprintf(cmdbuf, "USER %s\r\n", ftp_username ? ftp_username : "anonymous");
    if (!ftp_command(ctrl_tcb, cmdbuf, &code)) {
	ok = FALSE;
	goto ftp_done;
    }

    /*
     * Send "PASS" command.
     */
    if (ftp_username && ftp_password.pw) {
	char clear_password[PASSWORDLEN+1];
	boolean encrypted = FALSE;

	if (ftp_password.encrypt_type == ENCRYPT_TEMP) {
	    encrypted = TRUE;
	    decrypt(ftp_password.pw, clear_password, PASSWORDLEN);
	}
	sprintf(cmdbuf, "PASS %s\r\n", encrypted ? clear_password :
		ftp_password.pw);
    } else {
	sprintf(cmdbuf, "PASS %s@%s%s%s\r\n",
		stdio->user ? stdio->user : "root",
		hostname,
		default_domain ? "." : "",
		default_domain ? default_domain : "");
    }
    if (!ftp_command(ctrl_tcb, cmdbuf, &code)) {
	ok = FALSE;
	goto ftp_done;
    }

    /*
     * Send "TYPE I" command.
     */
    if (!ftp_command(ctrl_tcb, "TYPE I\r\n", &code)) {
	ok = FALSE;
	goto ftp_done;
    }

    /*
     * If passive mode (default), send a "PASV" command to
     * request an IP address and port for the data connection,
     * and connect to it.
     * If not in passive mode, create an end point and send
     * a "PORT" command to the server.
     */
    data_tcb = listen_tcb = NULL;
    winsize = FTP_WINDOW;
    if (ftp_passive_mode) {
	int i;
	char *addrport;
	addrtype faddr;

	if (!ftp_command(ctrl_tcb, "PASV\r\n", &code)) {
	    ok = FALSE;
	    goto ftp_done;
	}

	addrport = NULL;
	for (i = 3; i < strlen(ftp_last_reply); i++) {
	    if (isdigit(ftp_last_reply[i])) {
		addrport = &ftp_last_reply[i];
		break;
	    }
	}

	if (!addrport) {
	    tcp_close(ctrl_tcb);
	    return(FALSE);
	}

	faddr.type = ADDR_IP;
	faddr.ip_addr = 0;
	fport = 0;

	/* Get the address */
	for (i = 0; i < 4; i++) {
	    int octet;

	    octet = 0;
	    while (isdigit(*addrport)) {
		octet = octet * 10 + (*addrport - '0');
		addrport++;
	    }
	    faddr.ip_addr = (faddr.ip_addr << 8) + octet;

	    /* Skip comma. */
	    addrport++;
	}

	/* Get the port */
	for (i = 0; i < 2; i++) {
	    int octet;

	    octet = 0;
	    while (isdigit(*addrport)) {
		octet = octet * 10 + (*addrport - '0');
		addrport++;
	    }
	    fport = (fport << 8) + octet;

	    /* Skip comma or paren. */
	    addrport++;
	}

	data_tcb = tcp_open(NULL, fport, &faddr, 0, NULL, &status, NULL);
	if (data_tcb == NULL) {
	    if (tcpftp_debug)
		buginf("\nFTP: open failed -- %s", tcperror2string(status));
	    tcp_close(ctrl_tcb);
	    return(FALSE);
	}
	tcp_set_property(data_tcb, TCP_WINDOW_SIZE, &winsize, sizeof(winsize));
    } else {
	/*
	 * Create and TCB and listen on it.
	 */
	listen_tcb = tcp_create(NULL, &status, NULL);
	lport = tcp_uniqueport(0);
	if (lport == 0) {
	    if (tcpftp_debug)
		buginf("\nFTP: no local ports");
	    tcp_close(listen_tcb);
	    return(FALSE);
	}
	tcp_set_property(listen_tcb, TCP_WINDOW_SIZE, &winsize,
			 sizeof(winsize));
	tcp_bind(listen_tcb, NULL, lport);
	tcp_listen(listen_tcb, 1, NULL);
	sprintf(cmdbuf, "PORT %u,%u,%u,%u,%u,%u\r\n",
		((ctrl_tcb->localhost.ip_addr >> 24) & 0xff),
		((ctrl_tcb->localhost.ip_addr >> 16) & 0xff),
		((ctrl_tcb->localhost.ip_addr >> 8) & 0xff),
		((ctrl_tcb->localhost.ip_addr >> 0) & 0xff),
		(lport >> 8) & 0xff,
		(lport >> 0) & 0xff);
	if (!ftp_command(ctrl_tcb, cmdbuf, &code)) {
	    tcp_close(listen_tcb);
	    return(FALSE);
	}
    }

    /*
     * Send "STOR" command.
     */
    sprintf(cmdbuf, "STOR %s\r\n", name);
    if (!ftp_command(ctrl_tcb, cmdbuf, &code)) {
	if (data_tcb)
	    tcp_close(data_tcb);
	ok = FALSE;
	goto ftp_done;
    }

    /*
     * If not in passive mode, wait for the data connection.
     */
    if (!ftp_passive_mode && listen_tcb) {
	data_tcb = tcp_accept(listen_tcb, &status);
	if (data_tcb == NULL) {
	    if (tcpftp_debug)
		buginf("\nFTP: open failed -- %s", tcperror2string(status));
	    tcp_close(listen_tcb);
	    tcp_close(ctrl_tcb);
	    return(FALSE);
	}
	tcp_close(listen_tcb);
    }

    /*
     * Send the data.
     */
    ftp_move_data(data_tcb, buf, bufsize, display);

    /*
     * Close the data channel.
     */
    tcp_close(data_tcb);
    code = ftp_read_reply(ctrl_tcb, &status);
    if (tcpftp_debug)
	buginf("\nFTP: %*s", strlen(ftp_last_reply) - 2, ftp_last_reply);
    if (code >= 400) {
	ok = FALSE;
	goto ftp_done;
    }

ftp_done:
    /*
     * Send "QUIT" command.
     */
    ftp_command(ctrl_tcb, "QUIT\r\n", &code);

ftp_done_no_quit:
    /*
     * All done.
     */
    tcp_close(ctrl_tcb);
    return(ok);
}

/*
 * ftp_dump_core_file - Write out a core file using FTP
 * This is a separate call (the caller could be doing
 * reg_invoke_file_write()) because TFTP needs the extra
 * ipsocktype parameter.
 */

static boolean 
ftp_dump_core_file (uchar *mem, ulong size, const char *filename,
		    ipaddrtype iphost, ipsocktype *soc_unused_have_a_nice_day)
{
    addrtype host;

    host.type = ADDR_IP;
    host.ip_addr = iphost;

    return(ftp_write((char *) filename, &host, mem, size, NULL, NULL, NULL));
}

/*
 * ip_ftp_command
 *
 * Handle parsing chores.
 */

void
ip_ftp_command (parseinfo *csb)
{
    char temppassword[PASSWORDLEN_ENCRYPTED+1];

    if (csb->nvgen) {
	switch (csb->which) {
	case FTP_PASSIVE:
	    nv_write(!ftp_passive_mode, "no %s", csb->nv_command);
	    break;

	case FTP_SOURCE_INTERFACE:
	    if (ftp_source_idb)
		nv_write(TRUE, "%s %s", csb->nv_command,
			 ftp_source_idb->namestring);
	    break;

	case FTP_USERNAME:
	    if (ftp_username)
		nv_write(TRUE, "%s %s", csb->nv_command,
			 ftp_username);
	    break;

	case FTP_PASSWORD:
	    nv_password(csb, csb->nv_command, PRIV_MAX, NULL, &ftp_password,
			temppassword, PASSWORDLEN_ENCRYPTED);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
    case FTP_PASSIVE:
	ftp_passive_mode = csb->set_to_default || csb->sense;
	break;

    case FTP_SOURCE_INTERFACE:
	if (csb->sense)
	    ftp_source_idb = GETOBJ(idb,1);
	else
	    ftp_source_idb = NULL;
	break;

    case FTP_USERNAME:
	if (csb->sense)
	    setstring(&ftp_username, GETOBJ(string,1));
	else {
	    if (ftp_username) {
		free(ftp_username);
		ftp_username = NULL;
	    }
	}
	    
	break;

    case FTP_PASSWORD:
	if (csb->sense) {
	    char *pass;

	    ftp_password.encrypt_type = GETOBJ(int,1);
	    pass = GETOBJ(string,1);

	    /*
	     * If we are encrypting
	     * and we got a non encrypted password
	     * encrypt it.
	     */
	    if ((ftp_password.encrypt_type == ENCRYPT_NONE) &&
		password_encryption_enable) {
		if (strlen(pass) > PASSWORDLEN) {
		    printf("\n%% Overly long Password truncated");
		    pass[PASSWORDLEN] = 0;
		}
		memset(temppassword, 0, sizeof(temppassword));
		ftp_password.encrypt_type =
		    encrypt(pass, temppassword, 0, PASSWORDLEN_ENCRYPTED);
		pass = temppassword;
	    }
	    setstring(&ftp_password.pw, pass);
	} else {
	    setstring(&ftp_password.pw, NULL);
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Parse chains for top-level IP configuration commands
 */
#define ALTERNATE	NONE
#include "cfg_ip_ftp.h"
LINK_POINT(ftp_cfg_ip, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for exception protocol configuration command
 */
#define ALTERNATE	NONE
LINK_TRANS(exception_proto_ftp_exit, no_alt);
KEYWORD_ID(exception_proto_ftp, exception_proto_ftp_exit, ALTERNATE,
	 OBJ(int,1), FILE_ACCESS_FTP,
	 "ftp", "FTP protocol", PRIV_CONF);
#undef ALTERNATE
#define ALTERNATE	exception_proto_ftp
LINK_POINT(ftp_exception_proto, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chains for "debug ip" debug command
 */
#define ALTERNATE	NONE
EOLS  (debug_ip_ftp_eol, tcp_debug_command, DEBUG_TCPFTP);
KEYWORD (debug_ip_ftp, debug_ip_ftp_eol, ALTERNATE,
	 "ftp", "FTP dialogue", PRIV_OPR);
#undef ALTERNATE
#define ALTERNATE	debug_ip_ftp
LINK_POINT(ftp_debug, ALTERNATE);
#undef ALTERNATE

/*
 * Parse chain registration array for ftp
 */
static parser_extension_request ftp_chain_table[] = {
    { PARSE_ADD_CFG_TOP_IP_CMD, &pname(ftp_cfg_ip) },
    { PARSE_ADD_CFG_EXCEP_PROTO_CMD, &pname(ftp_exception_proto) },
    { PARSE_ADD_CFG_EXCEP_PROTO_EXIT,
	  (dynamic_transition *) &pname(exception_proto_ftp_exit) },
    { PARSE_ADD_DEBUG_IP_CMD, &pname(ftp_debug) },
    { PARSE_LIST_END, NULL }
};

/*
 * ftp_init
 * Initialize FTP support 
 */
static void
ftp_init (subsystype *subsys)
{
    ftp_username = NULL;
    memset(&ftp_password, 0, sizeof(ftp_password));
    ftp_passive_mode = TRUE;
    ftp_source_idb = NULL;

    parser_add_command_list(ftp_chain_table, "ftp");

    /*
     * Register some functions.
     */
    reg_add_core_dump(FILE_ACCESS_FTP, ftp_dump_core_file,
		      "ftp_dump_core_file");
}

/*
 * FTP subsystem header
 */

#define FTP_MAJVERSION	1
#define FTP_MINVERSION	0
#define FTP_EDITVERSION	1

SUBSYS_HEADER(ftp, FTP_MAJVERSION, FTP_MINVERSION,
	      FTP_EDITVERSION, ftp_init, SUBSYS_CLASS_PROTOCOL,
	      subsys_seq_iphost,
	      subsys_req_iphost);
