/* $Id: xremote_local.c,v 3.3.28.1 1996/03/18 22:56:45 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xremote/xremote_local.c,v $
 *------------------------------------------------------------------
 * xremote_local.c - XRemote local client support
 *  
 * September 21, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_local.c,v $
 * Revision 3.3.28.1  1996/03/18  22:56:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  11:23:25  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  07:53:37  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  19:27:06  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  21:54:58  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:35:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#define NEED_REPLIES
#define NEED_EVENTS
#include "Xproto.h"
#include "xremote.h"
#include "xremote_local.h"

/*
 * External references
 */

extern boolean xrflow_debug;
extern boolean xrfiles_debug;

/*
 * Transport-specific hooks for the scheduler block routines
 */

/*
 * xremote_local_client_writable - See if there is something to write to
 * the local client.
 *
 * This routine determines if we have enough of a response in order
 * to pass it to the local client.
 */

boolean xremote_local_client_writable (struct client *client)
{
    xrcontext *ctx = (xrcontext *)client->tc;

    return(client->buf->outbuf.count > ctx->localReadThresh);
}

/*
 * xremote_local_client_readable - See if there is anything to read
 *
 * This routine checks to see if there is anything to read from the
 * local client. It checks the output request queue for this.
 */

boolean xremote_local_client_readable (struct client *client)
{
    xrcontext *ctx = (xrcontext *)client->tc;

    return(BufferSlack(&client->buf->inbuf) > ctx->localWriteThresh &&
	   ctx->localReqQ.qhead);
}

/*
 * Client I/O handler
 */

/*
 * xremote_local_client_write - Write to the local client
 *
 * This is where the real work of the local client occurs.
 */

void xremote_local_client_write (struct client *client)
{
    xConnSetupPrefix prefix;
    xReply reply;
    xReq *xreq;
    int len;
    local_req *req;

    if (!cb->localConnected) {		/* Not connected yet */
	if (client->buf->outbuf.count < sizeof(prefix)) {
	    cb->localReadThresh = sizeof(prefix);
	    return;			/* Not enough of a prefix */
	}

	BufferPreview(sizeof(prefix), &client->buf->outbuf, &prefix);
	len = sizeof(prefix) + (prefix.length << 2);
	if (client->buf->outbuf.count < len) {
	    cb->localReadThresh = len;
	    return;
	}

	/*
	 * We have the entire connection reply. Flush it and ensure
	 * that the X terminal accepted the request.
	 */

	BufferMove(len, &client->buf->outbuf, NULL);

	if (!prefix.success) {
	    if (xrflow_debug)
	      buginf("\nXRemote%t: Local client rejected", stdio->ttynum);
	    CloseClient(cb, client);
	    return;
	}
	cb->localConnected = TRUE;	/* Mark as connected */
	cb->localReadThresh = sizeof(reply);
    }

    /*
     * At this point, we are guaranteed to be connected to the client,
     * so anything that we get here is a reply, an error, or an event.
     */

    while (client->buf->outbuf.count >= cb->localReadThresh) {
	BufferPreview(sizeof(reply), &client->buf->outbuf, &reply);

	if (reply.generic.type == X_Error) {

	    /*
	     * Handle an error event received from the server
	     */

	    if (xrflow_debug)
	      buginf("\nXRemote%t: Local client got error %d, seq %d",
		     stdio->ttynum,
		     reply.error.errorCode, reply.error.sequenceNumber);
	    BufferMove(sizeof(xError), &client->buf->outbuf, NULL);
	} else if (reply.generic.type == X_Reply) {

	    /*
	     * Handle a reply received from the server
	     */

	    len = sizeof(reply) + (reply.generic.length << 2);
	    if (client->buf->outbuf.count < len) {
		cb->localReadThresh = len;
	    } else {
		if (xrflow_debug)
		  buginf("\nXRemote%t: Local client got reply for seq %d",
			 stdio->ttynum, reply.generic.sequenceNumber);
		BufferMove(len, &client->buf->outbuf, NULL);
		cb->localReadThresh = sizeof(reply);
	    }
	} else {

	    /*
	     * Handle an event received from the server
	     */

	    if (xrflow_debug)
	      buginf("\nXRemote%t: Local client got event %d",
		     stdio->ttynum, reply.generic.type);
	    BufferMove(sizeof(xEvent), &client->buf->outbuf, NULL);
	}
    }

    /*
     * We have handled all of the replies to the local client. Now
     * we will check on requests to send.
     */

    while (TRUE) {
	req = (local_req *)cb->localReqQ.qhead;
	if (!req) {
	    cb->localWriteThresh = 0;
	    break;				/* Nothing more to do */
	}

	xreq = (xReq *)(req + 1);	/* Point to actual request */
	len = xreq->length << 2;	/* Get length */
	if (BufferSlack(&client->buf->inbuf) < len) {
	    cb->localWriteThresh = len;
	    break;
	}
	    
	BufferStuff(len, (char *)xreq, &client->buf->inbuf);
	(void)dequeue(&cb->localReqQ);
	free(req);
    }
}	
/*
 * Client creation and destruction
 */

/*
 * xremote_local_connect - Open the local client connection
 *
 * This routine is used to initiate the connection to the X terminal.
 */


static xConnClientPrefix local_connect = {
#ifdef BIGENDIAN
    'B',		/* Big endian byte ordering */
#else
    'l',		/* Little endian byte ordering */
#endif BIGENDIAN
    0,
    X_PROTOCOL,
    X_PROTOCOL_REVISION,
    0,
    0,
    0
  };

struct client *xremote_local_connect (tt_soc *tty)
{
    struct client *client;
    xrcontext *ctx = (xrcontext *)tty->xrcb;

    client = xremote_connect(tty);
    if (client) {
	client->transport = TRANSPORT_LOCAL;
	client->tc = ctx;
	BufferStuff(sz_xConnClientPrefix, &local_connect, &client->buf->inbuf);
	
	ctx->localReadThresh = sizeof(xConnSetupPrefix);
	ctx->localWriteThresh = 0;
	ctx->localConnected = FALSE;
    }
    return(client);
}

/*
 * xremote_local_client_close - Close the local client
 *
 * This routine is called when the local client is being closed. We
 * dump the queue of pending requests.
 */

void xremote_local_client_close (struct client *client)
{
    local_req *req;
    xrcontext *ctx = (xrcontext *)client->tc;

    while (TRUE) {
	req = dequeue(&ctx->localReqQ);
	if (!req)
	  break;

	free(req);
    }
}

/*
 * External routines which emulate various Xlib functions. Whenever
 * possible, create routines which behave like the real Xlib.
 *
 * At the present time, there is no support for replies, so all requests
 * must be of the type that do not generate replies.
 */

void XSetFontPath (
    struct client *client,
    char **directories,
    int ndirs)
{
    xrcontext *ctx = (xrcontext *)client->tc;
    int i;
    int len = 0;
    local_req *req;
    xSetFontPathReq *path;
    char *dirptr;

    /*
     * Calculate the length of the request
     */

    for (i = 0; i < ndirs; i++)
      len += 1 + strlen(directories[i]);

    len = pad(len + sizeof(xSetFontPathReq));

    /*
     * Allocate a buffer for the request
     */

    req = malloc(len + sizeof(local_req));
    if (!req) {
	return;
    }

    /*
     * Build the request header
     */

    path = (xSetFontPathReq *)(req + 1);
    path->reqType = X_SetFontPath;
    path->pad = 0;
    path->length = len >> 2;
    path->nFonts = ndirs;
    path->pad1 = 0;
    path->pad2 = 0;

    /*
     * Now build the request parameters
     */

    dirptr = (char *)(path + 1);
    for (i = 0; i < ndirs; i++) {
	len = strlen(directories[i]);
	*dirptr++ = len;
	bcopy(directories[i], dirptr, len);
	dirptr += len;
    }

    /*
     * Now put the request on the queue
     */

    enqueue(&ctx->localReqQ, req);
}
