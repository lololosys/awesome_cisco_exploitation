/* $Id: xremote.c,v 3.13.4.5 1996/08/07 09:05:48 snyder Exp $
 * $Source: /release/112/cvs/Xsys/xremote/xremote.c,v $
 *------------------------------------------------------------------
 * xremote.c - Many X windows sessions over one asynchronous line
 *
 * September 1990, Kevin Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: xremote.c,v $
 * Revision 3.13.4.5  1996/08/07  09:05:48  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.13.4.4  1996/06/28  23:34:14  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.13.4.3  1996/05/04  02:19:25  wilber
 * Branch: California_branch
 *
 * Name Access List
 *
 * Revision 3.13.4.2  1996/03/27  08:47:26  irfan
 * CSCdi50567:  Xremote Idle counter not displayed correctly.
 * Branch: California_branch
 * Add the missing space.
 *
 * Revision 3.13.4.1  1996/03/18  22:56:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/16  08:01:56  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.2  1996/03/07  11:23:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.7.2.1  1996/02/20  22:02:40  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/03/07  12:10:15  billw
 * CSCdi46565:  <uses> in  <show line> doesnt include PPP, Xremote, etc
 *
 * Revision 3.12  1996/02/26  06:47:40  irfan
 * CSCdi49862:  forward xremote not working over telnet vtys
 * fix nvector of a vty for the duration of xremote command.
 *
 * Revision 3.11  1996/02/15  08:12:40  irfan
 * CSCdi48518:  XRemote idle timer shows 5TE instead of a time
 * Fix typo in format string. Fix, the direct reference to tty->location.
 *
 * Revision 3.10  1996/02/07  16:18:35  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.9  1996/02/01  06:13:35  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.8  1996/01/24  20:43:00  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.7  1996/01/22  07:53:33  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.6  1996/01/16  03:38:19  ahh
 * CSCdi46921:  TCP: remove unused second parameter to tcp_close
 *
 * Revision 3.5  1995/12/31  12:24:01  irfan
 * CSCdi46266:  unused field in tt_soc structure
 * xremote doesn't need a compression struct ptr in tt_soc. It uses
 * the one in serial context.
 *
 * Revision 3.4  1995/12/28  18:31:20  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.3  1995/11/17  19:26:46  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:52  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:01:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/08/08  16:51:41  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.7  1995/07/21 01:47:32  billw
 * CSCdi36156:  No AAA accounting messages for ARAP or Xremote
 *
 * Revision 2.6  1995/06/21  09:01:58  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.5  1995/06/20  21:54:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/20  03:45:26  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.3  1995/06/17  06:27:07  ahh
 * CSCdi35734:  TCP: Long overdue code reorganization
 * Separate tcp.h into public and private header files.  Shuffle functions
 * around into more logical groupings.  Rename functions to meet naming
 * conventions (say goodbye to StudlyCaps).  Delete unneeded externs.
 * Trim off old log messages.
 *
 * Revision 2.2  1995/06/09  13:26:32  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:32:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "registry.h"
#include "sys_registry.h"
#include "../h/cs_registry.regh"
#include "../h/auth_registry.regh"
#include "packet.h"
#include "subsys.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../tcp/tcpinternal.h"
#include "access.h"
#include "../ip/ipaccess.h"
#include "ttysrv.h"
#include "aaa.h"
#include "file.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_xremote.h"
#include "name.h"

#define htoXRl(l) h2nl(l)
#define XRtohl(l) n2hl(l)
#define htoXRs(l) h2n(l)

#define NEED_REPLIES
#include "Xproto.h"
#include "os.h"
#include "Xdmcp.h"
#include "xremote.h"
#include "xremote_local.h"
#include "lat_xremote.h"
#include "xremote_debug.h"

#define MAXCHRS 40

#define MAXREPLYLENGTH (1 << 24)

boolean deltaEnable = TRUE;

queuetype xrfonthostQ;
int xr_font_memerr = 0;
boolean xremote_enable = TRUE;

extern boolean xdmBLOCK(xrcontext *);

struct delta tempDeltaBuf[MAXBYTESDIFF];
uchar tempReqBuf[MAXDELTASIZE];
uchar tempReplyBuf[MAXDELTASIZE];

/* forwards */
static struct buffer  *NewBuffer(xrcontext *, int, int);
static void DisposeBuffer(xrcontext *, struct buffer *);
static void CloseCachedFile(xrcontext *);
static void ClientWrite(struct client *);
static boolean DoSpoofedReply(struct client *, xGenericReply *, int);
static void DoRequests(void);
static void DoReplies(void);
static int DialClient(struct client *);
static int SendRequest(struct client *);
static int HangupClient(struct client *);
static void StuffDelta(struct client *, int, uchar *);
static boolean xremote_xtrek_client(ipaddrtype);
static int SerialRead(struct buffer *, int);
static int MaybeChangeClient(struct client *);
static void logout_xremote_shutdown(tt_soc *);
static boolean xremote_show_systat(tt_soc *); 

int xr_tftp_buffersize = XR_TFTP_BUFFERSIZE;
static int xr_tftp_retries = XR_TFTP_RETRIES;

/*
 * Xremote subsystem header
 */

#define XREM_MAJVERSION 1
#define XREM_MINVERSION 0
#define XREM_EDITVERSION  1

SUBSYS_HEADER(xremote, XREM_MAJVERSION, XREM_MINVERSION, XREM_EDITVERSION,
	      xremote_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: iphost, lat",
	      NULL);

static char *const clientStates[] = {
    "CS_NEW",
    "CS_ACTIVE",
    "CS_HANGUP",
    "CS_AWAIT_HANGUP_REPLY"
  };
     

void xremote_init (subsystype *subsys)
{
    xremote_parser_init();
    xr_debug_init();
    reg_add_alllines_setup(xremote_setup_global, "xremote_setup_global");
    reg_add_death(logout_xremote_shutdown, "logout_xremote_shutdown");
    reg_add_xremote_show_systat(xremote_show_systat, "xremote_show_systat");
    reg_add_xremote_show_tcp(xremote_show_tcp, "xremote_show_tcp");
    reg_add_xremote_tcp_conn(xremote_tcp_connect, "xremote_tcp_connect");
    reg_add_xremote_tcp_check_access(
		      xremote_tcp_check_access, "xremote_tcp_check_access");
    reg_add_xremote_create_tty_proc(create_xremote_tty_process,
		      "create_xremote_tty_process");
    xremote_lat_init();
}

char *client_state (struct client *client)
{
    return(client->state <= CS_AWAIT_HANGUP_REPLY ?
	   clientStates[client->state] : "UNKNOWN");
}

xrcontext *xremote_new_context (tt_soc *tty, boolean reverseMode)
{
    xrcontext *ctx;

    ctx = malloc_named(sizeof(xrcontext), "Xremote context");
    if (!ctx)
      return(NULL);
	
    ctx->tty = tty;
    ctx->lineProtocol = XR_XWIRE;
    ctx->lineBuf = NewBuffer(ctx, XREMOTE_BUFFERSIZE,
			     max(sizeof (struct shortcommand),
				 MAX_LZW_PLAIN));
    if (!ctx->lineBuf) {
	free(ctx);
	return(NULL);
    }

    ctx->serialContext = BSInit(tty, reverseMode);
    if (!ctx->serialContext) {
	DisposeBuffer(ctx, ctx->lineBuf);
	free(ctx);
	return(NULL);
    }
    ctx->xdmState = XDM_OFF;		/* Default to disable XDM */
    ctx->reverseMode = reverseMode; /* Set the reverse mode */

    return(ctx);
}

void xremote_shutdown (tt_soc *tty)
{
    int i;

    xrcontext *ctx = (xrcontext *)tty->xrcb;
    tty->xrcb = NULL;			/* Ensure no more connections */
    reg_invoke_aaa_acct_end(tty->ustruct, 0, ctx->acctinfo, 0,0,0,0);

    if (DEBUG_FLOW)
      buginf("\nXRemote%t: Shutting down; numClients = %d, someClients = %d",
	     tty->ttynum, ctx->numClients, ctx->someClients);

    BSShutdown(ctx->serialContext);	/* Shut down the bytestream driver */

    for (i = 0; i < MAXSOCKS; i++) {
	if (ctx->clientTable[i]) {
	    CloseClient(ctx, ctx->clientTable[i]);
	}
    }

    while (ctx->bufferList) {		/* Dump all the I/O buffers */
	DisposeBuffer(ctx, ctx->bufferList);
    }

    CloseCachedFile(ctx);		/* Dispose of any open file */

    if (ctx->fileReplyBuffer)		/* Dump any file reply packet */
      free(ctx->fileReplyBuffer);

    if (ctx->fileRequestedName)		/* And saved filename */
      free(ctx->fileRequestedName);

    BSFree(ctx->serialContext);		/* Free the serial resources */

    xdm_shutdown(ctx);			/* Free the XDM resources */

    process_sleep_for(ONESEC);			/* Wait a second for the NCD */
    clearinput(tty);			/* And clear any garbage */

    if (ctx->stopReason && !ctx->reverseMode) /* Display stop reason */
      printf("\nXRemote exited: %s", ctx->stopReason);

    free(ctx);				/* And free the context structure */
}

void logout_xremote_shutdown (tt_soc *tty)
{
    if (tty->xrcb) {
	xremote_shutdown(tty);
    }

}

void xremote_exec_command (parseinfo *csb)
{
    ipaddrtype xdmserver;
    boolean xdm_broadcast;
    struct vectortype_ *save_vector = NULL;

    switch (GETOBJ(int,1)) {

      case PARSER_XREMOTE_DISABLE:
	if (cb) {
	    xremote_shutdown(stdio);
	    printf("\nXRemote disabled");
	}
	return;

      case PARSER_XREMOTE_FONT:
#ifdef FONTTEST
	xremote_lat_font_test();
#endif FONTTEST
	return;

      case PARSER_XREMOTE_LAT:
	if (!xremote_lat_session_connect(GETOBJ(string, 1))) {
	    if (cb) {
		xremote_shutdown(stdio);
	    }
	    return;
	}
	break;

      case PARSER_XREMOTE_XDM:

	cb = xremote_new_context(stdio, FALSE);
	if (!cb) {
	    printf(nomemory);
	    return;
	}
	xdmserver = GETOBJ(paddr,1)->ip_addr;
	xdm_broadcast = FALSE;
	if (!xdmserver) {
	    xdmserver = -1L;
	    xdm_broadcast = TRUE;
	}
	printf("Trying XDM server at %i... ", xdmserver);
	flush();
	psion(stdio);
	xdm_init(cb, xdmserver, xdm_broadcast ? XDM_BROADCAST : XDM_QUERY);
	while (cb->xdmState != XDM_RUN_SESSION) {
	    edisms((blockproc *)xdmBLOCK, (ulong)cb);
	    if (cb->stopReason) {		/* Error? */
		printf("\nXDM access failed: %s", cb->stopReason);
		flush();
		cb->stopReason = NULL;	/* Don't say this again */
		psioff(stdio);
		xremote_shutdown(stdio); /* Clean things up */
		return;
	    }
	    if (psiset(stdio)) {
		printf("aborted");
		flush();
		psioff(stdio);
		xremote_shutdown(stdio);
		return;
	    }
	    if (modemcheck(stdio)) {
		xremote_shutdown(stdio);
		return;
	    }
	    xdm_wakeup(cb);
	}
	printf("\nEntering XRemote; display server is %s\n", 
	       name_addr2string(&cb->xdmSocket->faddress));
	flush();
	psioff(stdio);
	break;

      case PARSER_XREMOTE_XTREK:

	cb = xremote_new_context(stdio, FALSE);
	if (!cb) {
	    printf(nomemory);
	    return;
	}
	if (!xremote_xtrek_client(GETOBJ(paddr,1)->ip_addr)) {
	    xremote_shutdown(stdio);
	    return;
	}
	break;

      case PARSER_XREMOTE_DEFAULT:

	if (!cb) {
	    cb = xremote_new_context(stdio, FALSE);
	    if (!cb) {
		printf(nomemory);
		return;
	    }
	    printf("XRemote enabled; your display is %s:%d\n", hostname,
		   2000+stdio->ttynum);
	    printf("Start your clients and type XRemote again");
	    if (DEBUG_FLOW)
	      buginf("\nXRemote%t: passive enable", stdio->ttynum);
	    return;
	} else {
	    if (!cb->numClients) {
		printf("No X clients waiting - "
		       "check that your display is %s:%d",
		       hostname, 2000+stdio->ttynum);
		return;
	    }
	}
	break;
    }

    if (DEBUG_FLOW)
      buginf("\nXRemote%t: Entering XRemote", stdio->ttynum);

    GET_TIMESTAMP(cb->idletime);

    if (VTY_LINE == stdio->type) {
	/*
	 * xremote uses tty macros that use nvector. This is
	 * usually set to noVector on vty's. Temporary swap to
	 * allow xremote to function on vty's.
	 */
	save_vector = stdio->nvector;
	stdio->nvector = stdio->ivector;
    }

    BSStartup(cb->serialContext);

    cb->acctinfo = reg_invoke_aaa_acct_conn(stdio->ustruct, csb, NULL);

    stdio->usagecount++;
    while (TRUE) {
	edisms((blockproc *)xremoteBLOCK, (ulong)stdio);
	if (modemcheck(stdio)) {
	    if (DEBUG_FLOW)
	      buginf("\nXRemote%t: Modem disconnect", stdio->ttynum);
	    break;
	}
	if (cb->stopReason)
	  break;

	if (cb->someClients && !cb->numClients)
	  break;
	  
	BSWakeup(cb->serialContext);
	xdm_wakeup(cb);		/* See if XDM has any work to do */
	DoIO();
	DoRequests();
	DoReplies();
    }
    xremote_shutdown(stdio);
    if (VTY_LINE == stdio->type) {
	stdio->nvector = save_vector; /* Restore vty nativity */
    }
}

/*
 * Called if a fatal error occurs
 */

void xremote_reset (xrcontext *ctx, char *reason)
{
    if (DEBUG_FLOW)
      buginf("\nXRemote%t: got call to xremote_reset, reason %s", 
	     ctx->tty->ttynum, reason);
    if (!ctx->stopReason)
      ctx->stopReason = reason;
}

/*
 * Send a file reply packet
 */

static void
DoFileReply (void)
{
    FileReadData ReadReply;
    FileReadData *pReadReply = &ReadReply;

    if (!cb->fileErrorCode) {
	if (!cb->fileReplyBuffer) {
	    buginf("\nXRemote%t: DoFileReply() with no reply buffer",
		   stdio->ttynum);
	    return;
	}
	pReadReply = cb->fileReplyBuffer;
    } else {
	cb->fileReplyLength = 0;
    }      

    pReadReply->opcode = REQ_FILE_DATA;
    pReadReply->transaction_unique = cb->file_transaction_unique;
    pReadReply->xlength = htoXRs((sizeof(ReadReply)+
				  pad(cb->fileReplyLength))>>2);
    pReadReply->error = cb->fileErrorCode;
    pReadReply->offset = htoXRl(cb->fileRequestedOffset);
    pReadReply->lengthOfData = htoXRl(cb->fileReplyLength);
    if (DEBUG_FILES)
      buginf("\nXRemote%t: Sending %d bytes of %s, error code %d",
	     stdio->ttynum, cb->fileReplyLength, cb->fileRequestedName,
	     cb->fileErrorCode);
    {
	int todo;
	int try;
	uchar * next = (uchar *)pReadReply;
	todo = sizeof(ReadReply)+pad(cb->fileReplyLength);
	while (todo || cb->lineBuf->outbuf.count) {
	    if (BSPollWrite(cb->serialContext)) {
		if (cb->lineBuf->outbuf.count)
		  SerialWrite(cb->lineBuf);

		try = min(todo, BufferSlack(&cb->lineBuf->outbuf));
		if (try) {
		    BufferStuff(try, next, &cb->lineBuf->outbuf);
		    next += try;
		    todo -= try;
		}
	    }

	    edisms((blockproc *)xremoteBLOCK, (ulong)stdio);
	    if (modemcheck(stdio))	/* Hang up? */
	      break;			/* Yes, get out */
	    BSWakeup(cb->serialContext);
	}
    }
    if (pReadReply != &ReadReply) {
	free(pReadReply);
	cb->fileReplyBuffer = NULL;
    }
    if (cb->fileRequestedName) {
	free(cb->fileRequestedName);
	cb->fileRequestedName = NULL;
    }

    cb->fileErrorCode = 0;
}

static void
DoRequests (void)
{
    struct client  *curClient;
    struct client  *tempClient;
    int             i;
    int count;
    uchar *bufptr;
    boolean switchClient;

    if (!cb->firstRequestClient)
	cb->firstRequestClient = cb->anyClient;
    if (!cb->firstRequestClient)
	return;			/* no clients -- no work */
    curClient = cb->firstRequestClient;
    do {
	while (TRUE) {

	    switchClient = FALSE;	/* Assume we won't switch clients */

	    /* If we've started processing a request, just
	     * move bytes until we've finished the request.
	     */

	    while (cb->requestBytesLeft > 0) {
		
		switchClient = TRUE;	/* Ensure we switch after request */

		/* If there is no room left in the output buffer, write
		 * the buffer out.
		 */

		if (!BufferSlack(&cb->lineBuf->outbuf)) {
		    if (!SerialWrite(cb->lineBuf))
		      goto blocked;
		}

		/* If we have data bufferred in the client input
		 * buffer, we need to move it to the line output
		 * buffer.
		 */

		if (curClient->buf->inbuf.count) {
		    cb->requestBytesLeft -= BufferMove(cb->requestBytesLeft,
						       &curClient->buf->inbuf,
						       &cb->lineBuf->outbuf);
		    continue;
		}

		/* If there is data in the output buffer, read enough into
		 * the output buffer to fill it.
		 */

		if (cb->lineBuf->outbuf.count) {
		    count = ClientGetData(curClient,
					  &bufptr,
					  min(cb->requestBytesLeft,
					   BufferSlack(&cb->lineBuf->outbuf)));
		    if (count == 0)
		      goto blocked;
		    BufferStuff(count, bufptr, &cb->lineBuf->outbuf);
		    cb->requestBytesLeft -= count;
		    continue;
		}

		/* At this point, we have guaranteed that the client input
		 * buffer and the line output buffer is empty, so we can
		 * read data directly from the client transport connection
		 * and pass full blocks directly to the serial driver.
		 */
		 
		if (!BSPollWrite(cb->serialContext))
		  goto blocked;			/* Get out if we can't write */

		count = ClientGetData(curClient,
				      &bufptr, min(MAX_LZW_PLAIN,
						   cb->requestBytesLeft));
		if (count == 0) {
		    if (curClient->state != CS_HANGUP)
		      goto blocked;

		    /*
		     * We have no more data from the disconnected client,
		     * so we must pad the request with zeroes.
		     */

		    count = min(MAX_LZW_PLAIN, 
				min(cb->requestBytesLeft, 
				    curClient->buf->inbuf.bufsize));
		    memset(curClient->buf->inbuf.buf, 0, count);
		    bufptr = curClient->buf->inbuf.buf;
		}

		if (count == MAX_LZW_PLAIN) {
		    i = BSWrite(cb->serialContext, bufptr, count);
		    cb->requestBytesLeft -= i; /* Adjust by bytes moved */
		    count -= i;	/* In both places */
		    bufptr += i; /* Point past data moved */
		}

		if (count) {	/* Any residual bytes? */
		    BufferStuff(count, bufptr, &cb->lineBuf->outbuf);
		    cb->requestBytesLeft -= count;
		}
	    }

	    /*
	     * At each request boundary, consider what to do next.
	     *
	     * First off, check out a pending file read reply
	     */

	    if (cb->fileErrorCode || cb->fileReplyBuffer)
	      DoFileReply();

	    /*
	     * Get out if in the middle of a file transfer */
	    if (cb->lineProtocol != XR_XWIRE)
	      return;

	    /*
	     * Check out the current client
	     */

	    switch (curClient->state) {
	    case CS_NEW:
		i = DialClient(curClient);
		if (i == BLOCKED)
		    goto blocked;
		if (i == NOTREADY)
		    goto nextclient;
		if (i == DIAL_ERROR) {
		    tempClient = curClient;
		    curClient = NextClient(curClient);
		    CloseClient(cb, tempClient);
		    goto nextwhile;
		}
		else
		    continue;
	    case CS_AWAIT_HANGUP_REPLY:
		goto nextclient;
	    case CS_HANGUP:
		i = SendRequest(curClient);
		if (i == BLOCKED)
		    goto blocked;
		if (i == OUT_READY)
		    continue;
		if (HangupClient(curClient) == BLOCKED)
		    goto blocked;
		if (DEBUG_FLOW)
		    buginf("\nXRemote%t: "
			   "client %d change state from %s "
			   "to CS_AWAIT_HANGUP_REPLY",
			   stdio->ttynum,
			   curClient->XRemId, client_state(curClient));
		curClient->state = CS_AWAIT_HANGUP_REPLY;
		continue;
	    case CS_ACTIVE:
		if (switchClient ||
		    ((i = SendRequest(curClient)) == SWITCH_CLIENT)) {
		    cb->firstRequestClient = curClient;
		    goto nextclient;
		}
		if (i == BLOCKED)
		    goto blocked;
		if (i == NOTREADY)
		    goto nextclient;
		else
		    continue;
	    default:
		buginf("\nXRemote%t: DoRequest client %d bad state %x (%s)",
		       stdio->ttynum, curClient->XRemId,
		       curClient->state,
		       client_state(curClient));
		tempClient = curClient;
		curClient = NextClient(curClient);
		CloseClient(cb, tempClient);
		goto nextwhile;
	    }
	}
nextclient:
	curClient = NextClient(curClient);
nextwhile:
	;
    } while (curClient != cb->firstRequestClient);
blocked:
    cb->firstRequestClient = curClient;
}

static void
CloseCachedFile (
    xrcontext *ctx)
{
    if (ctx->currentFileName != NULL) {
	free(ctx->currentFileName);
	ctx->currentFileName = NULL;
	if (ctx->currentFileData) {
	    free(ctx->currentFileData);
	    ctx->currentFileData = NULL;
	}
    }
}

static int
openFile (
    char *filename)
{
    int status;
    xrfonthost *host;
    filetype file;
    int unixerror = EIO;

    cb->currentFileData = malloc(xr_tftp_buffersize);
    if (!cb->currentFileData) {
	xr_font_memerr++;
	return(ENOMEM);
    }

    cb->currentFileDataLen = xr_tftp_buffersize;

    /*
     * Look for a path beginning with /lat/ - this gets handles by LAT
     */

    if (strlen(filename) > 6 &&
	bcmp(filename, "/lat/", 5) == 0) {
	if (!xremote_lat_file_read(cb, &filename[5]))
	  return(EIO);
	return(0);
    }

    file.proto = FILE_ACCESS_TFTP;
    file.ps.tuba.boothost.type = ADDR_IP;
    file.filename = filename;
    for (host = (xrfonthost *)xrfonthostQ.qhead; host; host = host->next) {
	cb->currentFileDataLen = xr_tftp_buffersize; /* Restore buffersize */
	file.ps.tuba.boothost.ip_addr = host->addr;
	status = reg_invoke_file_read(file.proto, (filetype *)&file,
				 (uchar *)cb->currentFileData,
				 &cb->currentFileDataLen,
				 FALSE, NULL, NULL);
	if (DEBUG_FILES)
	 buginf("\nXRemote%t: openFile: read %s via tftp from %s, status = %d",
		stdio->ttynum, filename,
		name_addr2string(&file.ps.tuba.respondent),
		status);

	if (status == ERROR_OK) {
	    host->bytesread += cb->currentFileDataLen;
	    host->filesread++;
	    return(0);
	} else {
	    host->neterrors++;
	}
    }
    return(unixerror);
}

/*
 * Read the given file name (after 'seeking' to offset),
 * into the buf, and store the amount read through the pLength pointer.
 * The return value is the error number indicator.
 */
static int
ReadFileData (
    char * filename,
    int offset,
    char * buf,
    int * pLength)
{
    int rval = 0;
    int len;

    if (DEBUG_FILES)
      buginf("\nXRemote%t: ReadFileData name=%s offset=%d, length=%d",
	     stdio->ttynum, filename, offset, *pLength);
    if ((cb->currentFileName == NULL) ||
	strcmp(filename, cb->currentFileName)) {
	if (cb->currentFileName != NULL) {
	    CloseCachedFile(cb);
	}
	cb->currentFileName = strdup(filename);
	if (cb->currentFileName == NULL) {
	    xr_font_memerr++;
	    *pLength = 0;
	    return(ENOMEM);
	}
	rval = openFile(filename);
	if (rval) {
	    *pLength = 0;
	    CloseCachedFile(cb);
	    return(rval);
	}
    }
    len = *pLength;			/* Assume we can read whole thing */
    if (len > cb->currentFileDataLen - offset) {
	len = cb->currentFileDataLen - offset;
    }

    bcopy(cb->currentFileData+offset, buf, len); /* Copy the data */
    *pLength = len;
    return(0);
}

static inline ulong
xremote_reply_length (
     struct client *client,
     xGenericReply *rh)
{
    if (rh->type != X_Reply)
      return(8);		/* Events are always 8 longwords */

    return(HostUnswapLong(client, rh->length) + 8);
}

boolean DoSpoofedReply (struct client *client,
			xGenericReply *gotreply,
			int bytes)
{
    xListHostsReply reply;

    if (!client->spoofreq)
      return(FALSE);		/* Get out if not spoofing */

    if (client->spoofnum != HostUnswapWord(client, gotreply->sequenceNumber))
      return(FALSE);

    if (bytes != sz_xReply) {
	xremote_reset(cb, "Invalid byte count in spoofed reply");
	return(FALSE);
    }

    reply.type = X_Reply;
    reply.enabled = FALSE;
    reply.sequenceNumber = HostUnswapWord(client, client->spoofnum);
    reply.length = 0;
    reply.nHosts = 0;
    BufferStuff(sz_xListHostsReply, &reply, &client->buf->outbuf);

    client->spoofreq = FALSE;

    return(TRUE);
}

static void
DoReplies (void)
{
    struct shortcommand temp;
    int             i;
    struct client *curReplyClient;

    while (cb->lineBuf->inbuf.count) {

	GET_TIMESTAMP(cb->idletime);		/* Set new idle time */

	/* Get out if a pending file read reply answer */

	if (cb->fileErrorCode || cb->fileReplyBuffer)
	  return;

	curReplyClient = cb->clientTable[cb->selectedClient];

	if (cb->replyBytesLeft > 0) {
	    if (curReplyClient) {
		cb->replyBytesLeft -=
		  BufferMove(cb->replyBytesLeft,
			     &cb->lineBuf->inbuf,
			     (curReplyClient->state == CS_AWAIT_HANGUP_REPLY
			      || curReplyClient->state == CS_HANGUP) ? NULL :
			     &curReplyClient->buf->outbuf);
		if (cb->replyBytesLeft > 0)
		  return;
		else
		  continue;
	    } else {
		buginf("XRemote%t: Client %d not found with %d bytes to move",
		       stdio->ttynum, cb->selectedClient, cb->replyBytesLeft);
		xremote_reset(cb, "Client not found when moving reply");
		return;
	    }
	}
	i = *(uchar *) (cb->lineBuf->inbuf.tail);
	if (DEBUG_REPLIES)
	    buginf("\nXRemote%t: Got reply type %x", stdio->ttynum, i);
	switch (i) {
	case REPLY_CHANGE_CLIENT:
	    if (cb->lineBuf->inbuf.count < sizeof(temp))
		return;
	    BufferYank(sizeof(temp), &cb->lineBuf->inbuf, &temp);
	    if (temp.client >= MAXSOCKS) {
		buginf("\nXRemote%t: REPLY_CHANGE_CLIENT, client %d invalid",
		       stdio->ttynum, temp.client);
		xremote_reset(cb, "Invalid client ID in CHANGE_CLIENT");
		return;
	    }

	    cb->selectedClient = temp.client;
	    curReplyClient = cb->clientTable[cb->selectedClient];
	    if (!curReplyClient) {
		buginf("\nXRemote%t: "
		       "REPLY_CHANGE_CLIENT client %d nonexistent",
		       stdio->ttynum, temp.client);
		xremote_reset(cb, "CHANGE_CLIENT to nonexistent client");
		return;
	    }
	    break;
	case REPLY_CLOSE_CLIENT: {
	    struct client  *cClient;
	    if (cb->lineBuf->inbuf.count < sizeof(temp))
		return;
	    BufferYank(sizeof(temp), &cb->lineBuf->inbuf, &temp);
	    if (temp.client >= MAXSOCKS) {
		buginf("\nXRemote%t: REPLY_CLOSE_CLIENT, client %d invalid",
		       stdio->ttynum, temp.client);
		xremote_reset(cb, "Invalid client ID in CLOSE_CLIENT");
		return;
	    }

	    if (!(cClient = cb->clientTable[temp.client])) {
		buginf("\nXRemote%t: "
		       "REPLY_CLOSE_CLIENT client %d already gone",
		    stdio->ttynum, temp.client);
		xremote_reset(cb, "CLOSE_CLIENT to nonexistent client");
		return;
	    }
	    if (cClient->NeedConnectionInfo)
		CloseClient(cb, cClient);
	    else {
		if (DEBUG_REPLIES)
		    buginf("\nXRemote%t: DoReplies: HANGUP %d",
			   stdio->ttynum,
			   temp.client);
		if (DEBUG_FLOW)
		    buginf("\nXRemote%t: client %d change state from %s to CS_HANGUP (REPLY_CLOSE_CLIENT)",
			   stdio->ttynum,
			   cClient->XRemId,
			   client_state(cClient));
		cClient->state = CS_HANGUP;
	    }
	    break;
	    }
	case REPLY_HANGUP_REPLY:
	    if (cb->lineBuf->inbuf.count < sizeof(temp))
		return;
	    BufferYank(sizeof(temp), &cb->lineBuf->inbuf, &temp);
	    if (temp.client >= MAXSOCKS) {
		buginf("\nXRemote%t: REPLY_HANGUP_REPLY, client %d invalid",
		       stdio->ttynum, temp.client);
		xremote_reset(cb, "Invalid client ID in HANGUP_REPLY");
		return;
	    }

	    if (!cb->clientTable[temp.client]) {
		buginf("\nXRemote%t: REPLY_HANGUP_REPLY client %d nonexistent",
		    stdio->ttynum, temp.client);
		xremote_reset(cb, "HANGUP_REPLY to nonexistent client");
		return;
	    } else
		CloseClient(cb, cb->clientTable[temp.client]);
	    break;
	case REPLY_DELTA:
	    {
		struct deltacommand dc;
		int             deltaIndex, deltaBytesDiff, deltabytes;
		ulong	replybytes, replylongs;
		xGenericReply  *drprefix = (xGenericReply *) tempReplyBuf;
		char debug_buf[128];
		char *debug_ptr = debug_buf;

		if (!curReplyClient) {
		    buginf("\nXRemote%t: REPLY_DELTA client %d does not exist",
			   stdio->ttynum, cb->selectedClient);
		    xremote_reset(cb, "Client not found for delta reply");
		    return;
		}

		if (BufferSlack(&curReplyClient->buf->outbuf) < MAXDELTASIZE)
		    return;
		if (cb->lineBuf->inbuf.count < sizeof(dc.sc))
		    return;
		BufferPreview(sizeof(struct shortcommand), &cb->lineBuf->inbuf, &dc.sc);
		deltaIndex = dc.sc.client & 0xf;
		deltaBytesDiff = (dc.sc.client >> 4) & 0xf;
		deltabytes = sizeof(dc.sc) + deltaBytesDiff * sizeof(struct delta);
		if (cb->lineBuf->inbuf.count < deltabytes)
		    return;

		if (deltabytes > sizeof(dc)) {
		    buginf("\nXRemote%t: REPLY_DELTA client %d length %d "
			   "invalid",
			   stdio->ttynum, cb->selectedClient,
			   deltabytes);
		    xremote_reset(cb, "REPLY_DELTA with invalid length");
		    return;
		}

		BufferYank(deltabytes, &cb->lineBuf->inbuf, &dc);
		bcopy(cb->ReplyDeltas[deltaIndex].buf, tempReplyBuf, MAXDELTASIZE);
		for (i = 0; i < deltaBytesDiff; i++) {
		    if (DEBUG_DELTAS) {
			if (i == 0)
			  debug_ptr += sprintf(debug_ptr,
					       "\nXRemote%t: REP(%d)",
					       stdio->ttynum, deltaIndex);
			debug_ptr += sprintf(debug_ptr, "b%d:%02x>%02x",
					     dc.deltas[i].byte,
			tempReplyBuf[dc.deltas[i].byte], dc.deltas[i].data);
			if (i + 1 == deltaBytesDiff)
			  buginf(debug_buf);
			else
			  debug_ptr += sprintf(debug_ptr, ", ");
		    }
		    if (dc.deltas[i].byte >= MAXDELTASIZE) {
			buginf("\nXRemote%t: "
			       "Client %d invalid delta offset %d",
			       stdio->ttynum, cb->selectedClient,
			       dc.deltas[i].byte);
			xremote_reset(cb, "Invalid offset in delta reply");
			return;
		    }
		    tempReplyBuf[dc.deltas[i].byte] += dc.deltas[i].data;
		}
		replylongs = xremote_reply_length(curReplyClient, drprefix);
		replybytes = replylongs << 2; /* Convert to bytes */
		if (replylongs > (MAXDELTASIZE >> 2)) {
		    buginf("\nXRemote%t: "
			   "Client %d decoded delta reply length %d",
			   stdio->ttynum, cb->selectedClient,
			   replybytes);
		    xremote_reset(cb, "Invalid decoded length in delta reply");
		    return;
		}

		if (!(curReplyClient->state == CS_AWAIT_HANGUP_REPLY)
		    && !(curReplyClient->state == CS_HANGUP))
		  if (!DoSpoofedReply(curReplyClient, drprefix,
				      replybytes))
		    BufferStuff(replybytes, tempReplyBuf, 
				&curReplyClient->buf->outbuf);
		bcopy(tempReplyBuf, cb->ReplyDeltas[cb->nextReplyDelta].buf,
		      replybytes);
		cb->ReplyDeltas[cb->nextReplyDelta].length = replybytes;
		cb->nextReplyDelta = (cb->nextReplyDelta + 1) % NDELTAS;
		/*
		 * buginf("incrementing nextReplyDelta:
		 * %d\n",cb->nextReplyDelta); fflush(stdout);
		 */
	    }
	    break;
	case REPLY_READ_FILE: {
	    FileReadRequest ReadReq;
	    uint fileNameLength;

	    uint requestLength;
	    uint requestedReadLength;

	    if (cb->lineBuf->inbuf.count < sizeof(ReadReq))
		return;
	    BufferPreview(sizeof(ReadReq), &cb->lineBuf->inbuf, &ReadReq);
	    requestLength = XRtohl(ReadReq.requestlength)<<2;
	    cb->fileRequestedOffset = XRtohl(ReadReq.offset);
	    requestedReadLength = XRtohl(ReadReq.length);
	    fileNameLength = XRtohl(ReadReq.fileNameLength);
	    cb->file_transaction_unique = ReadReq.transaction_unique;
	    if (cb->lineBuf->inbuf.count < requestLength)
		return;
	    if (DEBUG_FILES)
		buginf("\nXRemote%t: read file request",stdio->ttynum);
	    requestLength -= sizeof(ReadReq);
	    if (fileNameLength > requestLength) {
		buginf("\nXRemote%t: REPLY_READ_FILE bad packet %d(%d)",
		    stdio->ttynum, fileNameLength, requestLength);
	    }
	    BufferYank(sizeof(ReadReq), &cb->lineBuf->inbuf, &ReadReq);
	    cb->lineProtocol = XR_FILE_ACCESS;
	    if (!(cb->fileRequestedName=malloc(requestLength+1))) {
		cb->fileErrorCode = ENOMEM;
		xr_font_memerr++;
		break;
	    } else {
		BufferYank(requestLength, &cb->lineBuf->inbuf,
			   cb->fileRequestedName);
		cb->fileRequestedName[fileNameLength] = '\0';
	    }
	    if (!(cb->fileReplyBuffer = malloc(sizeof (FileReadData)+pad(requestedReadLength)))) {
		cb->fileErrorCode = ENOMEM;
		xr_font_memerr++;
	    } else {
		cb->fileReplyLength = requestedReadLength;
		cb->fileErrorCode = ReadFileData(cb->fileRequestedName,
						 cb->fileRequestedOffset,
						 (char *)
						 (cb->fileReplyBuffer + 1),
						 &cb->fileReplyLength);
		if (cb->fileErrorCode) {
		    free(cb->fileReplyBuffer);
		    cb->fileReplyBuffer = NULL;
		}
	    }
	    break;
	}
	case REPLY_READ_FILE_DONE: {
	    FileReadDone ReadDoneReq;
	    if (cb->lineBuf->inbuf.count < sizeof(FileReadDone))
		return;
	    CloseCachedFile(cb);
	    BufferYank(sizeof(FileReadDone), &cb->lineBuf->inbuf, &ReadDoneReq);
	    if (DEBUG_FILES)
		buginf("\nXRemote%t: returning to XWIRE protocol",
		       stdio->ttynum);
	    cb->lineProtocol = XR_XWIRE;
	    break;
	}
	default:
	    if (!curReplyClient) {
		buginf("\nXRemote%t: Generic reply %x client %d does not exist",
		       stdio->ttynum, i, cb->selectedClient);
		xremote_reset(cb, "Client not found for reply");
		return;
	    } else {
		if (DEBUG_REPLIES)
		  buginf("\nXRemote%t: DoReplies: state=%s(%d)",
			 stdio->ttynum, client_state(curReplyClient),
			 curReplyClient->NeedConnectionInfo);
	    }
	    if (curReplyClient->NeedConnectionInfo) {
		xConnSetupPrefix prefix;
		if (cb->lineBuf->inbuf.count < sizeof(prefix))
		    return;
		BufferPreview(sizeof(prefix), &cb->lineBuf->inbuf, &prefix);
		cb->replyBytesLeft = sizeof(xConnSetupPrefix)
		    + 4 * HostUnswapWord(curReplyClient, prefix.length);
		if (DEBUG_REPLIES)
		    buginf("\nXRemote%t: "
			   "DoReplies: new client %d prefix len=%d",
			   stdio->ttynum,
			   curReplyClient->XRemId,
			   cb->replyBytesLeft);
		curReplyClient->NeedConnectionInfo = FALSE;
		continue;
	    } else {
		xGenericReply   prefix;
		ulong replylongs;

		if (cb->lineBuf->inbuf.count < sizeof(prefix))
		    return;
		if (BufferSlack(&curReplyClient->buf->outbuf) < sizeof(prefix))
		  return;
		BufferPreview(sizeof(prefix), &cb->lineBuf->inbuf, &prefix);
		replylongs = xremote_reply_length(curReplyClient, &prefix);
		if (replylongs > MAXREPLYLENGTH) {
		    buginf("\nXRemote%t: Reply length %d too long",
			   stdio->ttynum, replylongs);
		    xremote_reset(cb, "Reply length too long");
		    return;
		}
		cb->replyBytesLeft = replylongs << 2;
		if (DEBUG_REPLIES)
		    buginf("\nXRemote%t: DoReplies: generic len=%d",
			stdio->ttynum, cb->replyBytesLeft);
		if (cb->replyBytesLeft <= MAXDELTASIZE) {
		    if (cb->lineBuf->inbuf.count < cb->replyBytesLeft) {
			cb->replyBytesLeft = 0;
			return;
		    }
		    BufferPreview(cb->replyBytesLeft, &cb->lineBuf->inbuf, cb->ReplyDeltas[cb->nextReplyDelta].buf);
		    cb->ReplyDeltas[cb->nextReplyDelta].length = cb->replyBytesLeft;
		    cb->nextReplyDelta = (cb->nextReplyDelta + 1) % NDELTAS;
		    /*
		     * printf("incrementing nextReplyDelta:
		     * %d\n",cb->nextReplyDelta); fflush(stdout);
		     */
		}
		if (DEBUG_REPLIES)
		    buginf("\nXRemote%t: reply: %d, size %d",
			   stdio->ttynum, prefix.type, cb->replyBytesLeft);
		if (DoSpoofedReply(curReplyClient, &prefix,
				   cb->replyBytesLeft)) {
		    BufferMove(cb->replyBytesLeft, &cb->lineBuf->inbuf,
			       NULL);
		    cb->replyBytesLeft = 0;
		}
		continue;
	    }
	}
    }
}

/*
 * Determine if we should wake up because we can write from our I/O buffer
 * into a network buffer.
 */

static boolean ClientWritable (
     struct client *client)
{
    tcbtype *tcb;

    if (client->buf->outbuf.count == 0)
      return(FALSE);			/* Not writable if buffer empty */

    switch (client->transport) {
      case TRANSPORT_TCP:
	tcb = (tcbtype *)client->tc;

	if (tcb->curoutputpak && tcb->curoutputpak->length)
	  return(TRUE);
	if (tcp_putsetup(tcb))
	  return(TRUE);
	break;

      case TRANSPORT_LAT:
	xremote_lat_client_writable(client);
	break;

      case TRANSPORT_LOCAL:
	return(xremote_local_client_writable(client));
    }

    return(FALSE);			/* By now, not writable */
}

static boolean ClientReadable (
     struct client *client)
{
    tcbtype *tcb;

    switch (client->transport) {
      case TRANSPORT_TCP:
	tcb = (tcbtype *)client->tc;

	if (tcp_unread(tcb) || tcp_closing(tcb))
	  return(TRUE);
	break;

      case TRANSPORT_LAT:
	if (client->newdata)
	  return(TRUE);

      case TRANSPORT_LOCAL:
	return(xremote_local_client_readable(client));
    }

    return(FALSE);
}

boolean xremoteBLOCK (tt_soc *tty)
{
    xrcontext *ctx = (xrcontext *)(tty->xrcb);

    if (modemcheck(tty))		/* Get out on carrier drop */
      return(FALSE);

    if (ctx->stopReason)		/* Get out if we have a stop reason */
      return(FALSE);

    if (ctx->someClients && !ctx->numClients)
      return(FALSE);

    if ((ctx->lineBuf->outbuf.count || ctx->fileReplyBuffer ||
	 ctx->fileErrorCode) && BSPollWrite(ctx->serialContext))
      return(FALSE);			/* Wake up if pending output */

    if (BufferSlack(&ctx->lineBuf->inbuf) && BSPollRead(ctx->serialContext))
      return(FALSE);		/* Wake up if pending input */

    if (ctx->lineBuf->inbuf.count)
      return(FALSE);		/* Wake up if unread input */

    if (!xdmBLOCK(ctx))
      return(FALSE);			/* Wake up if XDM is ready */

    if (ctx->anyClient) {		/* If we have a client */

	if (ClientWritable(ctx->anyClient))
	  return(FALSE);		/* Return if we can write data */

	/* Poll for clients that have disconnected or have data to read
	 * for which we have buffer space */

	if (ctx->lineProtocol == XR_XWIRE &&
	    (ctx->anyClient == ctx->firstRequestClient ||
	     ctx->requestBytesLeft == 0) &&
	    ctx->anyClient->state != CS_HANGUP &&
	    ctx->anyClient->state != CS_AWAIT_HANGUP_REPLY &&
	    (ctx->anyClient->buf->inbuf.count != 0 ||
				/* Note: Should have threshold for check
				 * above, instead of non-zero */
	    ClientReadable (ctx->anyClient)))
	  return(FALSE);

	/* Nothing to do with this client; set up to check another one */

	ctx->anyClient = ctx->anyClient->flink;
    }
    return(BSBlock(ctx->serialContext));
}

void DoIO (void)
{
    struct client *client = cb->anyClient;
    struct client *nextClient;

    if (BufferSlack(&cb->lineBuf->inbuf) && BSPollRead(cb->serialContext)) {
	SerialRead(cb->lineBuf, cb->lineBuf->inbuf.bufsize);
    }

    if (cb->lineBuf->outbuf.count && BSPollWrite(cb->serialContext)) {
	SerialWrite(cb->lineBuf);
    }
    
    while (client) {
	nextClient = client->flink;
	ClientWrite(client);
	client = nextClient;
	if (client == cb->anyClient)
	  break;
    }
}

struct client *
xremote_create_client (xrcontext *ctx, int index)
{
    struct client *result;

    result = malloc_named(sizeof(struct client), "Xremote client");
    if (result == NULL)
	return(result);
    result->buf = NewBuffer(ctx, XREMOTE_BUFFERSIZE, XREMOTE_BUFFERSIZE);

    if (!result->buf) {
	free(result);
	return(NULL);
    }

    if (!ctx->anyClient) {
	result->blink = result;
	result->flink = result;
	ctx->anyClient = result;
    } else {
	result->flink = ctx->anyClient->flink;
	result->blink = ctx->anyClient;
	ctx->anyClient->flink = result;
	result->flink->blink = result;
    }

    result->ctx = ctx;

    result->XRemId = index;
    result->NeedConnectionInfo = TRUE;
    if (DEBUG_FLOW)
	buginf("\nXRemote%t: new client %d state is CS_NEW",
	    ctx->tty->ttynum, result->XRemId);
    result->state = CS_NEW;
    ctx->clientTable[index] = result;
    ctx->numClients++;
    ctx->someClients++;
    return(result);
}

struct client *xremote_connect (tt_soc *tty)
{
    XRemoteClientId	index;
    Bool	found = FALSE;
    xrcontext *ctx = (xrcontext *)(tty->xrcb);

    if (!ctx)				/* Reject if Xremote not enabled */
      return(NULL);

    if (ctx->reverseMode)
      return(NULL);		/* Reject if in reverse mode */

    for (index = 0 ; index < MAXSOCKS ; index++) {
	if (!ctx->clientTable[index]) {
	    found = TRUE;
	    break;
	}
    }
    if (!found)
	return(NULL);

    return(xremote_create_client(ctx, index));
}

boolean xremote_tcp_check_access (tt_soc *tty, ipaddrtype fhost, ulong
				  lport, ulong dport)
{
    xrcontext *ctx = (xrcontext *)tty->xrcb;

    if (!ctx)
      return(FALSE);		/* XRemote not enabled */

    if (ctx->reverseMode)
      return(FALSE);		/* Not allowed in reverse mode */

    return(check_access(tty, fhost, lport, dport, FALSE));
}

boolean xremote_tcp_connect (tcbtype *tcb, tt_soc *tty)
{
    struct client *client;

    client = xremote_connect(tty);
    if (client) {
	client->transport = TRANSPORT_TCP;
	client->tc = tcb;
	xdm_tcp_open(tty->xrcb, client);
	return TRUE;
    }
    return FALSE;
}

struct buffer *
xremote_io_buffer (int insize, int outsize) 
{
    struct buffer *buf;

    buf = malloc_named(sizeof(struct buffer), "Xremote IO Buf");
    if (buf) {
	buf->inbuf.buf = malloc_named(insize, "Xremote InBuf");
	if (buf->inbuf.buf) {
	    buf->inbuf.buftop = buf->inbuf.buf + insize;
	    buf->inbuf.bufsize = insize;
	    buf->inbuf.head = buf->inbuf.buf;
	    buf->inbuf.tail = buf->inbuf.buf;

	    buf->outbuf.buf = malloc_named(outsize, "Xremote OutBuf");
	    if (buf->outbuf.buf) {
		buf->outbuf.buftop = buf->outbuf.buf + outsize;
		buf->outbuf.bufsize = outsize;
		buf->outbuf.head = buf->outbuf.buf;
		buf->outbuf.tail = buf->outbuf.buf;
		buf->link = NULL;
		return(buf);
	    }
	    free(buf->inbuf.buf);
	}
	free(buf);
    }
    return(NULL);
}

static struct buffer  *NewBuffer (xrcontext *ctx, int insize, int outsize)
{
    struct buffer *buf = ctx->bufferList;
    struct buffer **lastLink = &ctx->bufferList;

    while (buf) {
	lastLink = &buf->link;
	buf = buf->link;
    }

    buf = xremote_io_buffer(insize, outsize);
    if (buf)
      *lastLink = buf;

    return(buf);
}

struct client *NextClient (struct client  *client)
{
    return(client->flink);
}

static void DisposeBuffer (xrcontext *ctx, struct buffer *bufptr)
{
    struct buffer *bufprv = NULL;	/* Previous buffer in list */
    struct buffer *bufnxt;		/* Next buffer to check */
    
    for (bufnxt = ctx->bufferList; bufnxt; bufnxt = bufnxt->link) {
	if (bufnxt == bufptr)		/* If we found a match */
	  break;			/* Get out */
	bufprv = bufnxt;		/* Save previous pointer */
    }

    if (!bufnxt) {			/* If we didn't find buffer */
	buginf("\nXRemote%t: DisposeBuffer: Buffer at %x not found in list",
	       ctx->tty->ttynum, bufptr);
	return;
    }

    if (bufprv) { 			/* If we found an entry */
	bufprv->link = bufptr->link;	/* Unlink it */
    } else {
	ctx->bufferList = bufptr->link;	/* Not found, set the new head */
    }

    if (bufptr->inbuf.buf)		/* Free the input buffer */
      free(bufptr->inbuf.buf);

    if (bufptr->outbuf.buf)		/* Free the output buffer */
      free(bufptr->outbuf.buf);

    free(bufptr);			/* Free the buffer structure */
}

static void
DisposeClient (xrcontext *ctx, struct client *client)
{
    DisposeBuffer(ctx,client->buf); /* Return the I/O buffers */

    if (client->flink == client) {
	free(client);
	ctx->anyClient = NULL;
    } else {
	client->flink->blink = client->blink;
	client->blink->flink = client->flink;
	if (ctx->anyClient == client)
	    ctx->anyClient = client->blink;
	free(client);
    }
    ctx->numClients--;
}

static int 
SendRequest (struct client *client)
{
    xReq            rh;
    int             bytes;
    if (cb->grabbingClient && cb->grabbingClient != client)
	return(NOTREADY);

    if (client->spoofreq)
      return(NOTREADY);		/* Get out if already spoofing a request */

    ClientRead(client, sizeof(rh));

    if (client->buf->inbuf.count < sizeof(rh))
      return(NOTREADY);

    if (client->bytesSent > MAXBYTESPERCLIENT) {
	client->bytesSent = 0;
	return(SWITCH_CLIENT);
    }

    BufferPreview(sizeof(rh), &client->buf->inbuf, &rh);
    bytes = 4 * HostUnswapWord(client, rh.length);

    /*
     * If this request could be compressed, or it is one of the requests
     * that we spoof, ensure that we have read the entire request before
     * proceeding.
     */

    if (((bytes <= MAXDELTASIZE) && deltaEnable) ||
	(rh.reqType >= X_ChangeHosts && rh.reqType <= X_SetAccessControl)) {
	ClientRead(client, bytes);
	if (client->buf->inbuf.count < bytes)
	  return(NOTREADY);
    }

    if (MaybeChangeClient(client) == BLOCKED)
	return(BLOCKED);

    if (DEBUG_REQUESTS) {
	buginf("\nXRemote%t: sending request: %d, fd=%d, size %d",
	    stdio->ttynum, rh.reqType, client->XRemId, bytes);
    }
    if (bytes < sz_xReq) {
	buginf("\nXRemote%t: "
	       "bad request length %d request code=%d. terminating client",
	       stdio->ttynum, bytes, rh.reqType);
	client->buf->inbuf.count = 0;
	client->state = CS_HANGUP;
	return(OUT_READY);
    }

    /*
     * Actually spoof the requests here
     */

    if (rh.reqType >= X_ChangeHosts && rh.reqType <= X_SetAccessControl) {

	if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(rh))
	  return(BLOCKED);
	BufferMove(bytes, &client->buf->inbuf, NULL);
	client->seqnum++;

	/*
	 * If this is a request that needs a reply, turn this into a
	 * X_GetInputFocus request so that we can later spoof the reply;
	 * otherwise, make this a NoOperation request so that the sequence
	 * numbers are properly preserved.
	 */

	if (rh.reqType == X_ListHosts) {
	    rh.reqType = X_GetInputFocus;
	    client->spoofreq = TRUE;
	    client->spoofnum = client->seqnum;
	} else
	  rh.reqType = X_NoOperation;

	rh.length = HostUnswapWord(client, 1);

	StuffDelta(client, sizeof(rh), (uchar *)&rh);
	client->bytesSent += sizeof(rh);

	return(OUT_READY);
    }

    if ((bytes <= MAXDELTASIZE) && (deltaEnable)) {
	if (BufferSlack(&cb->lineBuf->outbuf) < bytes)
	    return(BLOCKED);
	BufferYank(bytes, &client->buf->inbuf, tempReqBuf);
	client->seqnum++;
	StuffDelta(client, bytes, tempReqBuf);
	client->bytesSent += bytes;
    } else {
	cb->requestBytesLeft = bytes;
	client->bytesSent += cb->requestBytesLeft;
	client->seqnum++;		/* Update sequence number */
    }
    if (rh.reqType == X_GrabServer)
	cb->grabbingClient = client;
    if (rh.reqType == X_UngrabServer)
	cb->grabbingClient = NULL;
    return(OUT_READY);
}

static void
StuffDelta (struct client *client, int bytes, uchar *buf)
{
    int	i = MAXBYTESDIFF + 1;
    int	j = MAXBYTESDIFF + 1;
    int	k = MAXBYTESDIFF + 1;
    int	l = MAXBYTESDIFF + 1;
    char debug_buf[128];
    char *debug_ptr = debug_buf;

    int		m;

    for (m = cb->nextReqDelta, i = 0; i < cb->activeReqDeltas; i++) {
	if (m==0)
	    m = NDELTAS;
	m--;
	if (cb->ReqDeltas[m].length == bytes)
	    j = BytesDiff(buf, cb->ReqDeltas[m].buf, bytes);
	else
	    j = MAXBYTESDIFF + 1;	/* don't use this slot */
	if (j < l) {
	    k = m;
	    l = j;
	}
    }
    if (l > MAXBYTESDIFF || sizeof(struct shortcommand) + l * sizeof(struct delta) >= bytes)
	BufferStuff(bytes, buf, &cb->lineBuf->outbuf);
    else {
	struct shortcommand sc;
	sc.opcode = REQ_DELTA;
	sc.client = ((l << 4) & 0xf0) | (k & 0xf);
	BufferStuff(sizeof(sc), &sc, &cb->lineBuf->outbuf);
	j = 0;
	for (i = 0; i < bytes; i++)
	    if (buf[i] != cb->ReqDeltas[k].buf[i]) {
		if (DEBUG_DELTAS) {
		    if (j == 0)
		      debug_ptr += sprintf(debug_ptr, "\nXRemote%t: REQ(%d)",
					   stdio->ttynum, k);
		    debug_ptr += sprintf(debug_ptr, "b%d:%02x>%02x",
					i, cb->ReqDeltas[k].buf[i],
					buf[i]);
		    if (j + 1 == l)
			buginf(debug_buf);
		    else
		      debug_ptr += sprintf(debug_ptr, ", ");
		}
		tempDeltaBuf[j].byte = i;
		tempDeltaBuf[j++].data = buf[i] - cb->ReqDeltas[k].buf[i];
	    }
	BufferStuff(j * sizeof(struct delta), tempDeltaBuf, &cb->lineBuf->outbuf);
	/*
	 * printf("sent delta'd request: "); for (i=0;i<bytes;i++)printf("%d
	 * ",*(uchar *)(buf+i)); printf("\n"); fflush(stdout);
	 */
    }
    bcopy(buf, cb->ReqDeltas[cb->nextReqDelta].buf, bytes);
    cb->ReqDeltas[cb->nextReqDelta].length = bytes;
    cb->nextReqDelta = (cb->nextReqDelta + 1) % NDELTAS;
    if (cb->activeReqDeltas < NDELTAS)
	cb->activeReqDeltas++;
}

int 
BytesDiff (uchar *ptr1, uchar *ptr2, int n)
{
    int result = 0;
    while (n--) {
	if (*(ptr1++) != *(ptr2++)) {
	    result++;
	}
    }
    return(result);
}


static int 
DialClient (struct client *client)
{
    struct shortcommand dm;
    xConnClientPrefix prefix;
    int             setupbytes;

    if (!client->tc)
      return(DIAL_ERROR);

    if (cb->grabbingClient && cb->grabbingClient != client)
	return(NOTREADY);
    ClientRead(client, sizeof(xConnClientPrefix));
    if (client->buf->inbuf.count < sizeof(xConnClientPrefix))
	return(NOTREADY);

    if (client->transport == TRANSPORT_LAT && 
	!xremote_lat_client_connected(client))
      return(NOTREADY);			/* Connection not up yet */
	  
    if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(dm))
	return(BLOCKED);
    if (DEBUG_FLOW)
	buginf("\nXRemote%t: new requesting client %d", 
	       stdio->ttynum, client->XRemId);
    BufferPreview(sizeof(xConnClientPrefix), &client->buf->inbuf, &prefix);
    switch (prefix.byteOrder) {
    case 'B':
	client->LSBfirst = FALSE;
	break;
    case 'l':
	client->LSBfirst = TRUE;
	break;
    default:
	buginf("\nXRemote%t: DialClient: bad byte order specifier",
	       stdio->ttynum);
	return(DIAL_ERROR);
    }
    dm.opcode = REQ_DIAL;
    dm.client = client->XRemId;
    BufferStuff(sizeof(dm), &dm, &cb->lineBuf->outbuf);
    cb->lastRequestClient = client;
    setupbytes = sizeof(xConnClientPrefix)
	+ pad(HostUnswapWord(client, prefix.nbytesAuthProto))
	+ pad(HostUnswapWord(client, prefix.nbytesAuthString));
    cb->requestBytesLeft = setupbytes;
    if (DEBUG_FLOW)
	buginf("\nXRemote%t: client %d change state from %s to CS_ACTIVE",
	    stdio->ttynum, client->XRemId, client_state(client));
    client->state = CS_ACTIVE;
    return(OUT_READY);
}

static int 
HangupClient (struct client *client)
{
    struct shortcommand hm;
    if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(hm))
	return(BLOCKED);
    if (DEBUG_FLOW)
	buginf("\nXRemote%t: sending REQ_HANGUP for client %d", 
	       stdio->ttynum, client->XRemId);
    hm.opcode = REQ_HANGUP;
    hm.client = client->XRemId;
    BufferStuff(sizeof(hm), &hm, &cb->lineBuf->outbuf);
    return(OUT_READY);
}


static int 
MaybeChangeClient (struct client *client)
{
    struct shortcommand cm;
    if (client == cb->lastRequestClient)
	return(OUT_READY);
    if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(cm))
	return(BLOCKED);
    cm.opcode = REQ_CHANGE_CLIENT;
    cm.client = client->XRemId;
    BufferStuff(sizeof(cm), &cm, &cb->lineBuf->outbuf);
    cb->lastRequestClient = client;
    if (DEBUG_REQUESTS) {
	buginf("\nXRemote%t: requesting switching to client %d",
	    stdio->ttynum, cb->lastRequestClient->XRemId);
    }
    return(OUT_READY);
}

int 
BufferMove (int bc, struct halfbuf *buf1, struct halfbuf *buf2)
{
    int             total = 0;
    while (total < bc && buf1->count > 0 &&
	   (!buf2 || BufferSlack(buf2) > 0)) {
	int             bytes = bc - total;
	if (bytes > buf1->count)
	    bytes = buf1->count;
	if (buf2 && bytes > BufferSlack(buf2))
	    bytes = BufferSlack(buf2);
	if (bytes > buf1->buftop - buf1->tail)
	    bytes = buf1->buftop - buf1->tail;
	if (buf2 && bytes > buf2->buftop - buf2->head)
	    bytes = buf2->buftop - buf2->head;
	if (buf2)
	    bcopy(buf1->tail, buf2->head, bytes);
	buf1->tail += bytes;
	if (buf1->tail == buf1->buftop)
	    buf1->tail = buf1->buf;
	if (buf2) {
	    buf2->head += bytes;
	    if (buf2->head == buf2->buftop)
		buf2->head = buf2->buf;
	    buf2->count += bytes;
	}
	total += bytes;
	buf1->count -= bytes;
    }
    return(total);
}


int 
BufferStuff (int bc, void *src, struct halfbuf *buf2)
{
    int             total = 0;
    while (total < bc && BufferSlack(buf2) > 0) {
	int             bytes = bc - total;
	if (bytes > BufferSlack(buf2))
	    bytes = BufferSlack(buf2);
	if (bytes > buf2->buftop - buf2->head)
	    bytes = buf2->buftop - buf2->head;
	bcopy(src, buf2->head, bytes);
	buf2->head += bytes;
	if (buf2->head == buf2->buftop)
	    buf2->head = buf2->buf;
	total += bytes;
	(uchar *)src += bytes;
	buf2->count += bytes;
    }
    if (total != bc)
	buginf("XRemote%t: BufferStuff: failed: count=%d, total=%d", 
	       stdio->ttynum, bc, total);
    return(total);
}


/*
 * Preview 'bc' bytes from 'buf1' into 'dst'.
 * Handles buffer wrap-around.
 * No modification to the buffer is made.
 * Return value: number of bytes previewed (which might be
 * less than the number requested.)
 */
int 
BufferPreview (int bc, struct halfbuf *buf1, void *dst)
{
    int total = 0, bcount = buf1->count;
    uchar *btail = buf1->tail;

    while (total < bc && bcount > 0) {
	int bytes = bc - total;

	if (bytes > buf1->count)
	    bytes = buf1->count;
	if (bytes > buf1->buftop - btail)
	    bytes = buf1->buftop - btail;
	bcopy(btail, dst, bytes);
	btail += bytes;
	if (btail == buf1->buftop)
	    btail = buf1->buf;
	(uchar *)dst += bytes;
	total += bytes;
	bcount -= bytes;
    }
    return(total);
}

/*
 * Fetch bc bytes from buf1 into dst.
 * Update buffer pointers.
 * Return value is the number of bytes transferred.
 * See BufferPreview if you can't deal with short reads.
 */
int 
BufferYank (int bc, struct halfbuf *buf1, void *dst)
{
    int             total = 0;
    while (total < bc && buf1->count > 0) {
	int             bytes = bc - total;
	if (bytes > buf1->count)
	    bytes = buf1->count;
	if (bytes > buf1->buftop - buf1->tail)
	    bytes = buf1->buftop - buf1->tail;
	bcopy(buf1->tail, dst, bytes);
	(uchar *)dst += bytes;
	buf1->tail += bytes;
	if (buf1->tail == buf1->buftop)
	    buf1->tail = buf1->buf;
	total += bytes;
	buf1->count -= bytes;
    }
    return(total);
}

int ClientGetData (struct client *client, uchar **bufptr, int bytes)
{
    int status;
    int gotbytes;
    tcbtype *tcb;

    switch (client->transport) {
      case TRANSPORT_TCP:

	tcb = (tcbtype *)client->tc;

	status = tcp_getstring(tcb, (uchar **)bufptr, &gotbytes, bytes);
	switch (status) {
	  case OK:
	  case URGENT_DATA:
	    break;
	  case NO_DATA:
	    gotbytes = 0;
	    break;
	  case END_OF_FILE:
	  default:
	    if (client->state != CS_HANGUP &&
		client->state != CS_AWAIT_HANGUP_REPLY) {
		if (DEBUG_FLOW) {
		    buginf("\nXRemote%t: client %d(%d) change state "
			   "from %s to CS_HANGUP (ClientGetData)",
			   stdio->ttynum, client->XRemId, tcb,
			   client_state(client));
		}
		client->state = CS_HANGUP;
	    }
	    gotbytes = 0;
	}
	break;
      case TRANSPORT_LAT:
	xremote_lat_get_data(client);
	gotbytes = 0;			/* No I/O done here */
	break;

      case TRANSPORT_LOCAL:
	gotbytes = 0;			/* Never do real I/O */
	break;
    }

    return(gotbytes);
}

/*
 * ClientRead - Read from a client connection into our I/O buffer
 */

void ClientRead (struct client *client, int bc)
{
    int bytes;
    uchar *bufptr;
    int read_count;
    struct halfbuf *buf = &client->buf->inbuf;

    client->newdata = FALSE;	/* Avoid race conditions by clearing now */

    if (bc > buf->bufsize)
      bc = buf->bufsize;

    while (buf->count < bc) {

	/* Set bytes to be the number of bytes we can move this pass */

	bytes = bc - buf->count;	/* Get bytes left to move */
	if (bytes > buf->buftop - buf->head) /* If we'd wrap the buffer */
	  bytes = buf->buftop - buf->head; /* Adjust the count down */

	read_count = ClientGetData(client, &bufptr, bytes);
	if (!read_count)
	  break;

	bcopy(bufptr, buf->head, read_count);
	  
	if (DEBUG_REQUESTS || DEBUG_PACKETS) {
	    int i;
	    uchar *p = (uchar *)buf->head;
	    char debug_buf[128];
	    char *debug_ptr = debug_buf;

	    buginf("\nXRemote%t: read %d bytes from client fd %d",
		   stdio->ttynum, read_count, client->XRemId);
	    if (DEBUG_PACKETS) {
		for (i = 0; i < read_count; i++) {
		    if (debug_ptr == debug_buf)
		      debug_ptr += sprintf(debug_ptr,
					   "\nXRemote%t: ", stdio->ttynum);
		    debug_ptr += sprintf(debug_ptr,
					 "%02x ", *p++);
		    if ((i % 20 == 19) || (i + 1 == read_count)) {
			buginf(debug_buf);
			debug_ptr = debug_buf;
		    }
		}
	    }
	}
	buf->head += read_count;
	if (buf->head == buf->buftop)
	  buf->head = buf->buf;
	buf->count += read_count;
    }
}
      
/*
 * Read bytes from the serial line into the serial buffer
 */

static int 
SerialRead (struct buffer *buf, int bc)
{
    int             total = 0;
    register struct halfbuf *buf2 = &buf->inbuf;
    int gotbytes;
    int bytes;

    if (bc > buf2->bufsize)
      bc = buf2->bufsize;

    while (buf2->count < bc) {

	/* Set bytes to be the number of bytes we can move this pass */

	bytes = bc - buf2->count;	/* Get bytes left to move */
	if (bytes > buf2->buftop - buf2->head) /* If we'd wrap the buffer */
	  bytes = buf2->buftop - buf2->head; /* Adjust the count down */

	gotbytes = BSRead(cb->serialContext, buf2->head, bytes);
	if (!gotbytes)
	  break;

	if (DEBUG_REPLIES) {
	    buginf ("\nXRemote%t: received %d bytes from serial line",
		    stdio->ttynum, gotbytes);
	}
	buf2->head += gotbytes;
	if (buf2->head == buf2->buftop)
	    buf2->head = buf2->buf;
	total += gotbytes;
	buf2->count += gotbytes;
    }
    return(total);
}

static void ClientWrite (struct client *client)
{
    int             total = 0;
    register struct halfbuf *buf1 = &client->buf->outbuf;

    switch (client->transport) {
      case TRANSPORT_TCP:

	while (buf1->count > 0) {
	    int             bytes = buf1->count, gotbytes;
	    if (bytes > buf1->buftop - buf1->tail)
	      bytes = buf1->buftop - buf1->tail;
	    if (DEBUG_REPLIES || DEBUG_PACKETS) {
		int i;
		uchar * p = (uchar *)buf1->tail;
		char debug_buf[128];
		char *debug_ptr = debug_buf;
		
		buginf("\nXRemote%t: writing %d bytes to client fd %d",
		       stdio->ttynum, bytes, client->XRemId);
		if (DEBUG_PACKETS) {
		    for (i = 0; i < bytes; i++) {
			if (debug_ptr == debug_buf)
			  debug_ptr += sprintf(debug_ptr,
					       "\nXRemote%t: ", stdio->ttynum);
			debug_ptr += sprintf(debug_ptr,
					     "%02x ", *p++);
			if ((i % 20 == 19) || (i + 1 == bytes)) {
			    buginf(debug_buf);
			    debug_ptr = debug_buf;
			}
		    }
		}
	    }
	    
	    gotbytes = tcp_putstring((tcbtype *)client->tc, 
				     (uchar *)buf1->tail, bytes);

	    buf1->tail += gotbytes;
	    if (buf1->tail == buf1->buftop)
	      buf1->tail = buf1->buf;
	    total += gotbytes;
	    buf1->count -= gotbytes;
	    if (gotbytes == 0)
	      break;
	}
	tcp_putpush((tcbtype *)client->tc, FALSE, FALSE);
	break;
      case TRANSPORT_LAT:
	xremote_lat_client_writable(client);
	break;
      case TRANSPORT_LOCAL:
	xremote_local_client_write(client);
	break;
    }
}

int 
SerialWrite (struct buffer *buf)
{
    int             total = 0;
    register struct halfbuf *buf1 = &buf->outbuf;
    if (buf1->count > 0) {
	int             bytes = buf1->count, gotbytes;
	if (bytes > buf1->buftop - buf1->tail)
	    bytes = buf1->buftop - buf1->tail;

	gotbytes = BSWrite(cb->serialContext, buf1->tail, bytes);

	buf1->tail += gotbytes;
	if (buf1->tail == buf1->buftop)
	    buf1->tail = buf1->buf;
	total += gotbytes;
	buf1->count -= gotbytes;
	if (gotbytes < bytes)
	    return(total);
    }
    return(total);
}

void CloseClient (xrcontext *ctx, struct client *client)
{
    if (!client) {
	buginf ("\nXRemote%t: CloseClient(Null)", ctx->tty->ttynum);
	return;
    }

    /* Close the transport connection */

    if (client->tc) {
	switch (client->transport) {
	  case TRANSPORT_TCP:
	    xdm_tcp_close(ctx, client);		/* Tell XDM about the close */
	    tcp_close((tcbtype *)client->tc);
	    break;
	  case TRANSPORT_LAT:
	    xremote_lat_close_client(client);
	    break;
	  case TRANSPORT_LOCAL:
	    xremote_local_client_close(client);
	    break;
	}
    }

    if (ctx->clientTable[client->XRemId] != client) {
	buginf("\nXRemote%t: CloseClient: mismatch: "
	       "clientTable[%d]=0x%x, client=0x%x",
	       ctx->tty->ttynum, client->XRemId, 
	       ctx->clientTable[client->XRemId], client);
    } else
	ctx->clientTable[client->XRemId] = NULL;
    if (DEBUG_FLOW)
	buginf("\nXRemote%t: closing client %d", 
	       ctx->tty->ttynum, client->XRemId);
    DisposeClient(ctx,client);
    if (ctx->firstRequestClient == client)
	ctx->firstRequestClient = 0;
    if (ctx->lastRequestClient == client)
	ctx->lastRequestClient = 0;
    if (ctx->grabbingClient == client)
	ctx->grabbingClient = NULL;
}

int 
HostUnswapWord (struct client *client, int val)
{
    if (!client->LSBfirst)
	return(val);
    return(((val & 0xff00) >> 8) | ((val & 0xff) << 8));
}

int 
HostUnswapLong (struct client *client, int val)
{
    if (!client->LSBfirst)
	return(val);
    return(((val >> 24) & 0xff) | ((val >> 8) & 0xff00) |
	   ((val << 8) & 0xff0000) | ((val << 24) & 0xff000000));
}


int 
pad (int i)
{
    if (i % 4 == 0)
	return(i);
    return(i - i % 4 + 4);
}

void xremote_command (parseinfo *csb)
{
    xrfonthost *fonthost;

    if (csb->nvgen) {
	switch (csb->which) {
	    
          case XREMOTE_BUFFERSIZE_CMD:
            nv_write(xr_tftp_buffersize != XR_TFTP_BUFFERSIZE,
		     "%s %d", csb->nv_command,
		     xr_tftp_buffersize);
            break;

	  case XREMOTE_TFTP_BUFFERSIZE:
            /* Obsolete command. Don't need NV generation */
            break;

	  case XREMOTE_TFTP_RETRIES:
            nv_write(xr_tftp_retries != XR_TFTP_RETRIES,
		     "%s %d", csb->nv_command,
		     xr_tftp_retries);
            break;

          case XREMOTE_TFTP_HOST:
            for (fonthost = (xrfonthost *)xrfonthostQ.qhead; fonthost;
		fonthost = fonthost->next) {
                nv_write(TRUE, "%s %i", csb->nv_command, fonthost->addr);
	    }  
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
    switch (csb->which) {

      case XREMOTE_BUFFERSIZE_CMD:
      case XREMOTE_TFTP_BUFFERSIZE:
	if (csb->sense) {
	    xr_tftp_buffersize = GETOBJ(int,1);
	} else {
	    xr_tftp_buffersize = XR_TFTP_BUFFERSIZE;
	}
	break;

      case XREMOTE_TFTP_RETRIES:
	if (csb->sense) {
	    xr_tftp_retries = GETOBJ(int,1);
	} else {
	    xr_tftp_retries = XR_TFTP_RETRIES;
	}
	break;

      case XREMOTE_TFTP_HOST:
	for (fonthost = (xrfonthost *)xrfonthostQ.qhead; fonthost;
	     fonthost = fonthost->next) {
	    if (fonthost->addr == GETOBJ(paddr,1)->ip_addr) {
		if (!csb->sense) {
		    unqueue(&xrfonthostQ, fonthost);
		    free(fonthost);
		}
		return;
	    }
	}
	if (!csb->sense) {
	    printf("\nHost %i is not a font server", GETOBJ(paddr,1)->ip_addr);
	    return;
	}

	fonthost = malloc(sizeof(xrfonthost));
	if (fonthost) {
	    fonthost->addr = GETOBJ(paddr,1)->ip_addr;
	    enqueue(&xrfonthostQ, fonthost);
	}else{
	    printf(nomemory);
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

static void xremote_show_server (void)
{
    xrfonthost *host;

    printf("\nXRemote server-wide parameters:"
	   "\n  Font buffersize:     %9d          Font retries: %d"
	   "\n  Font memory errors:  %9d",
	   xr_tftp_buffersize, xr_tftp_retries, xr_font_memerr);

    for (host = (xrfonthost *)xrfonthostQ.qhead; host; host = host->next) {
	printf("\n"
	       "\n"
	       "\nTFTP font load statistics for host %i:"
	       "\n  Bytes read:          %9d          Files read: %d"
	       "\n  Network errors:      %9d          File errors: %d",
	       host->addr, host->bytesread, host->filesread,
	       host->neterrors, host->fileerrors);
    }
    xrlatfont_show_server();
}

static boolean xremote_show_tty (int line)
{
    tt_soc *tty;
    xrcontext *ctx;
    int j;
    struct client *client;

    tty = MODEMS[line];
    if (!tty)
      return(FALSE);

    ctx = (xrcontext *)tty->xrcb;
    if (!ctx)
      return(FALSE);

    mem_lock (ctx);
    printf("\n"
	   "\n"
	   "\nXRemote statistics for tty%t:",
	   tty->ttynum);

    if (ctx->reverseMode)
      printf(" (reverse connection to %i:%d)",
	     ctx->reverse_ipaddr,
	     ctx->reverse_display - X_TCP_PORT);

    printf("\n  Current clients:     %9d          Total clients: %d",
	   ctx->numClients, ctx->someClients);
    
    if (ctx->firstRequestClient) {
	printf("\n  Requesting client:   %9d          Current request size: %d",
	       ctx->firstRequestClient->XRemId, ctx->requestBytesLeft);
    }
	
    printf("\n  Replying client:     %9d          Current reply size: %d",
	   ctx->selectedClient, ctx->replyBytesLeft);

    if (ctx->grabbingClient)
      printf("\n  Grabbing client:     %9d",
	     ctx->grabbingClient->XRemId);

    if (ctx->xdmState != XDM_OFF)
      printf("\n  XDM state:           %9d          Next timeout: %d"
	     "\n  Retransmit counter:  %9d          Local UDP port: %d"
	     "\n  Keepalive dormancy:  %9d          Session id: %d",
	     ctx->xdmState, TIME_LEFT_SLEEPING(ctx->xdmTimeOutTime), 
	     ctx->xdmTimeOutRtx, ctx->xdmSocket->lport, 
	     ctx->xdmKeepaliveDormancy, ctx->xdmSessionID);

    printf("\n  Unread input:        %9d          Unwritten output: %d"
	   "\n  Input buffer size:   %9d          Output buffer size: %d",
	   ctx->lineBuf->inbuf.count,
	   ctx->lineBuf->outbuf.count,
	   ctx->lineBuf->inbuf.bufsize,
	   ctx->lineBuf->outbuf.bufsize);

    XremoteStatsDisplay(ctx->serialContext);
    
    for (j = 0; j < MAXSOCKS; j++) {
	client = ctx->clientTable[j];
	if (client) {
	    mem_lock(client);
	    printf("\n\n  Connection %d - ", j);
	    switch (client->transport) {
	      case TRANSPORT_TCP:
		printf("TCP connection ");
		if (client->tc)
		    printf("from %s", 
			   name_addr2string(&((tcbtype *)client->tc)->
					    foreignhost));
		else
		  printf("closing");
		if (ctx->xdmSessionClient == client)
		  printf("  [Display Manager]");
		break;
	      case TRANSPORT_LAT:
		show_xremote_lat_client(client);
		break;
	      case TRANSPORT_LOCAL:
		printf("local connection");
		break;
	    }
	    printf("\n    Client state:       %9s         Byte order: %sSBfirst"
		   "\n    Unread input:       %9d         Unwritten output: %d"
		   "\n    Input buffer size:  %9d         Output buffer size: %d",
		   client_state(client),
		   client->LSBfirst ? "L" : "M",
		   client->buf->inbuf.count,
		   client->buf->outbuf.count,
		   client->buf->inbuf.bufsize,
		   client->buf->outbuf.bufsize);
	    if (client->NeedConnectionInfo)
	      printf("\n    Awaiting connection information");
	}
	free(client);
    }
    free(ctx);
    return(TRUE);
}

void show_xremote (parseinfo *csb)
{
    int i;

    if (GETOBJ(int,1)) {
	automore_enable(NULL);
	if (!xremote_show_tty(GETOBJ(int,2))) {
	    printf("\nLine is not using XRemote");
	}
    } else {
	automore_enable(NULL);		/* Enable more processing */
	xremote_show_server();

	for (i = 0; i < ALLlines; i++) {
	    xremote_show_tty(i);
	}
    }
    automore_disable();
}

void xremote_show_tcp (tt_soc *tty)
{
    xrcontext *ctx = (xrcontext *)tty->xrcb;
    tcbtype *tcb;
    struct client *client;
    int i;

    if (!ctx)
      return;

    for (i = 0; i < MAXSOCKS; i++) {
	client = ctx->clientTable[i];
	if (client && client->transport == TRANSPORT_TCP) {
	    tcb = (tcbtype *)client->tc;
	    printf("\n\ntty%t, Connection %d, "
		   "XRemote TCP connection from host %s",
		   tty->ttynum, client->XRemId,
		   name_addr2string(&tcb->foreignhost));
	    tcp_status(tcb);
	}
    }
}

/*
 * xremote_clients - Return the current number of Xremote clients
 */

int xremote_clients (tt_soc *tty)
{
    xrcontext *ctx = (xrcontext *)tty->xrcb;

    return(ctx ? ctx->numClients : 0);
}

/*
 * xremote_lastinput - Return the time of last Xremote input
 */

static sys_timestamp xremote_lastinput (tt_soc *tty)
{
    xrcontext *ctx = (xrcontext *)tty->xrcb;

    return(ctx ? ctx->idletime : timer_no_time);
}

static boolean xtrekBLOCK (tcbtype *tcb)
{
    tt_soc *tty = (tt_soc *)tcb->tty;
    xrcontext *ctx = (xrcontext *)tty->xrcb;

    if (psiset(tty) || tcp_unread(tcb) || ctx->numClients)
      return(FALSE);
    return(TRUE);
}

/*
 * xremote_xtrek_client - Provoke the xtrek daemon to talk to us
 */

#define XTREK_DAEMON_PORT 5701

boolean xremote_xtrek_client (ipaddrtype host)
{
    tcbtype *tcb;
    addrtype faddr;
    char buf[256], *luser;
    int status;
    int count;
    int i;
    int ch;

    printf("\nTrying %i... ", host);
    faddr.type = ADDR_IP;
    faddr.ip_addr = host;
    tcb = tcp_open(stdio, XTREK_DAEMON_PORT, &faddr, 0, NULL, &status, NULL);

    if (!tcb) {
	printf("%s", tcperror2string(status));
	return(FALSE);
    }

    count = sprintf(buf, "Display: %i:%d Login: ",
		    tcb->localhost, stdio->ttynum + 2000);

    if (old_access_control) {
	if (stdio->user)
	    count += sprintf(&buf[count], "%s", stdio->user);
	else
	    count += sprintf(&buf[count], "tty%t", stdio->ttynum);
    } else {
	if (aaa_get_user_p(stdio->ustruct, &luser))
	    count += sprintf(&buf[count], "%s", luser);
	else
	    count += sprintf(&buf[count], "tty%t", stdio->ttynum);
    }

    count += sprintf(&buf[count], " Name: ");

    if (stdio->location)
      count += sprintf(&buf[count], "%s\015\012", stdio->location);
    else
      count += sprintf(&buf[count], "tty%t\015\012", stdio->ttynum);

    for (i = 0; i < count; i++) {
	if (!tcp_putbyte(tcb, buf[i], TRUE)) {
	    printf("Error sending player request message");
	    tcp_abort(tcb);
	    return(FALSE);
	}
    }
    tcp_putpush(tcb, FALSE, FALSE);

    /*
     * We have sent the request, now await a response
     */

    psion(stdio);

    while (TRUE) {
	edisms((blockproc *)xtrekBLOCK, (ulong)tcb);

	while ((ch = tcp_getbyte(tcb, &status)) != -1)
	  printc(ch);
	flush();

	if (psiset(stdio)) {
	    psioff(stdio);
	    tcp_abort(tcb);
	    printf("aborted");
	    return(FALSE);
	}

	if (cb->numClients) {
	    psioff(stdio);
	    tcp_close(tcb);
	    return(TRUE);		
	}

	if (tcp_closing(tcb)) {
	    psioff(stdio);
	    tcp_abort(tcb);
	    return(FALSE);
	}
    }
}

static boolean xremote_show_systat (tt_soc *tty)
{
    int j;

    j = xremote_clients(tty);
    if (j) {
	printf("XRemote: %-2d clients  %-#5TE %s\n",
	       j, xremote_lastinput(tty), get_tty_location(tty));
	return(TRUE);
    }
    return(FALSE);
}
