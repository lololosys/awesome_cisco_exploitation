/* $Id: xremote_reverse.c,v 3.5.28.1 1996/03/18 22:56:48 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xremote/xremote_reverse.c,v $
 *------------------------------------------------------------------
 * xremote_reverse.c - Handle outbound XRemote support
 *  
 * December 24, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_reverse.c,v $
 * Revision 3.5.28.1  1996/03/18  22:56:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.1  1996/03/07  11:23:27  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5  1996/01/22  07:53:39  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/28  18:31:23  jenny
 * CSCdi46011:  TCP/IP need to do code clean up.
 * This is the first patch of code changes for TCP/IP code cleanup.
 * . removed all references to TUBA_IP and TUBA_CLNS.
 * . renamed open_tuba_socket() to ip_open_socket().
 * . renamed tcp_tuba_open() to tcp_open().
 * . pass addrtype * instead of ipaddrtype for local and foreight addresses
 *   to tcp_open() for feature portability.
 *
 * Revision 3.3  1995/11/17  19:27:10  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:54:55  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  14:01:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:40:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/20  03:45:30  ahh
 * CSCdi36095:  Connection routines use msclock
 * Remove more msclock references.
 *
 * Revision 2.2  1995/06/09  13:26:35  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:35:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "packet.h"
#include "../ui/debug.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "ttysrv.h"
#include "../ts/async_ppp.h"
#include "config.h"
#include "logger.h"
#include "connect.h"
#include "../os/async.h"
#include "../os/os_debug.h"

#define NEED_REPLIES
#include "Xproto.h"
#include "os.h"
#include "Xdmcp.h"
#include "xremote.h"
#include "xremote_debug.h"

#define firstReplyClient firstRequestClient
#define lastReplyClient lastRequestClient
#define activeReplyDeltas activeReqDeltas
#define reqBytesLeft replyBytesLeft
#define repBytesLeft requestBytesLeft

#define reply_length(client, rep) ((rep)->type==X_Reply ? \
	32 + (HostUnswapLong(client,(rep)->length) << 2) \
	: 32)

static int RevHangupReply(struct client *);
static int RevSendReply(struct client *);
static int RevHangupClient(struct client *);
static int RevMaybeChangeClient(struct client *);
static void RevStuffDelta(struct client *, int, unsigned char *);
static struct client *xremote_reverse_connect(int);

/*
 * This routine walks through all of our server connections, and looks for
 * any replies to send over the serial line.
 */

static void RevDoReplies (void)
{
    struct client  *curClient;
    struct client  *tempClient;
    int             i;
    int count;
    uchar *bufptr;
    boolean switchClient;

    if (!cb->firstReplyClient)
	cb->firstReplyClient = cb->anyClient;
    if (!cb->firstReplyClient)
	return;			/* no clients -- no work */
    curClient = cb->firstReplyClient;
    do {
	while (TRUE) {

	    switchClient = FALSE;	/* Assume we won't switch clients */

	    /* If we've started processing a reply, just
	     * move bytes until we've finished the reply.
	     */

	    while (cb->repBytesLeft > 0) {
		
		switchClient = TRUE;	/* Ensure we switch after reply */

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
		    cb->repBytesLeft -= BufferMove(cb->repBytesLeft,
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
					  min(cb->repBytesLeft,
					   BufferSlack(&cb->lineBuf->outbuf)));
		    if (count == 0)
		      goto blocked;
		    BufferStuff(count, bufptr, &cb->lineBuf->outbuf);
		    cb->repBytesLeft -= count;
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
						   cb->repBytesLeft));
		if (count == 0)
		  goto blocked;
		if (count == MAX_LZW_PLAIN) {
		    BSWrite(cb->serialContext, bufptr, count);
		} else {
		    BufferStuff(count, bufptr, &cb->lineBuf->outbuf);
		}
		cb->repBytesLeft -= count;
	    }

	    /*
	     * At each reply boundary, consider what to do next.
	     */

	    switch (curClient->state) {
	      case CS_NEW:
		/*
		 * New client, wait for the initial message
		 */

		goto nextclient;

	      case CS_AWAIT_HANGUP_REPLY:
		/*
		 * Sent a hangup, waiting for a reply. Just skip this client.
		 */
		goto nextclient;

	      case CS_HANGUP_REPLY:
		/*
		 * Respond to a request to close a client from the
		 * helper process
		 */

		i = RevHangupReply(curClient);
		if (i == BLOCKED)
		  goto blocked;

		goto closeclient;

	      case CS_HANGUP:
		/*
		 * Tell the helper to close a client; the remote server
		 * just closed our transport connection.
		 */
		i = RevSendReply(curClient);
		if (i == BLOCKED)
		    goto blocked;
		if (i == OUT_READY)
		    continue;
		if (RevHangupClient(curClient) == BLOCKED)
		    goto blocked;
		if (curClient->NeedConnectionInfo) {
		    if (DEBUG_FLOW) 
		      buginf("\nXRemote%t: "
			     "client %d hangup before any I/O",
			     stdio->ttynum, curClient->XRemId);
		    goto closeclient;
		}
		if (DEBUG_FLOW)
		  buginf("\nXRemote%t: "
			 "client %d change state from %s "
			 "to CS_AWAIT_HANGUP_REPLY",
			 stdio->ttynum,
			 curClient->XRemId, client_state(curClient));
		curClient->state = CS_AWAIT_HANGUP_REPLY;
		goto nextclient;

	    case CS_ACTIVE:
		if (switchClient ||
		    ((i = RevSendReply(curClient)) == SWITCH_CLIENT)) {
		    cb->firstReplyClient = curClient;
		    goto nextclient;
		}
		if (i == BLOCKED)
		    goto blocked;
		if (i == NOTREADY)
		    goto nextclient;
		else
		    continue;
	    default:
		buginf("\nXremote%t: RevDoReply client %d bad state %x (%s)",
		       stdio->ttynum, curClient->XRemId,
		       curClient->state,
		       client_state(curClient));
		goto closeclient;
	    }
	}
nextclient:
	curClient = NextClient(curClient);
	goto nextwhile;

closeclient:
	tempClient = curClient;
	curClient = NextClient(curClient);
	CloseClient(cb, tempClient);
	if (!cb->firstReplyClient)
	  cb->firstReplyClient = cb->anyClient;
	if (!cb->firstReplyClient)
	  return;
nextwhile:
	;
    } while (curClient != cb->firstReplyClient);
blocked:
    cb->firstReplyClient = curClient;
}

/*
 * Read a reply from the X server, and send it down the serial line.
 */

static int 
RevSendReply (struct client *client)
{
    xGenericReply rh;
    int             bytes;

    if (cb->grabbingClient && cb->grabbingClient != client)
	return (NOTREADY);

    /*
     * If we are sending the server's initial response, we need to figure
     * out the length differently.
     */

    if (client->NeedConnectionInfo) {
	xConnSetupPrefix prefix;

	ClientRead(client, sizeof(prefix));
	if (client->buf->inbuf.count < sizeof(prefix))
	  return(NOTREADY);

	if (RevMaybeChangeClient(client) == BLOCKED)
	  return(BLOCKED);

	BufferPreview(sizeof(prefix), &client->buf->inbuf, &prefix);
	cb->repBytesLeft = sizeof(xConnSetupPrefix)
	  + 4 * HostUnswapWord(client, prefix.length);
	if (DEBUG_REPLIES)
	  buginf("\nXRemote%t: "
		 "DoReplies: new client %d prefix len=%d",
		 stdio->ttynum,
		 client->XRemId,
		 cb->repBytesLeft);
	client->NeedConnectionInfo = FALSE;
	return(OUT_READY);
    }

    ClientRead(client, sizeof(rh));

    if (client->buf->inbuf.count < sizeof(rh))
      return(NOTREADY);

    if (client->bytesSent > MAXBYTESPERCLIENT) {
	client->bytesSent = 0;
	return(SWITCH_CLIENT);
    }

    BufferPreview(sizeof(rh), &client->buf->inbuf, &rh);
    bytes = reply_length(client, &rh);

    /*
     * If this reply could be compressed, ensure that we have read the
     * entire reply before proceeding.
     */

    if ((bytes <= MAXDELTASIZE) && deltaEnable) {
	ClientRead(client, bytes);
	if (client->buf->inbuf.count < bytes)
	  return(NOTREADY);
    }

    if (RevMaybeChangeClient(client) == BLOCKED)
	return (BLOCKED);

    if (DEBUG_REPLIES) {
	buginf("\nXremote%t: sending reply for sequence %d, fd=%d, size %d",
	       stdio->ttynum, HostUnswapWord(client, rh.sequenceNumber),
	       client->XRemId, bytes);
    }

    if ((bytes <= MAXDELTASIZE) && (deltaEnable)) {
	if (BufferSlack(&cb->lineBuf->outbuf) < bytes)
	    return (BLOCKED);
	BufferYank(bytes, &client->buf->inbuf, tempReplyBuf);
	RevStuffDelta(client, bytes, tempReplyBuf);
	client->bytesSent += bytes;
    } else {
	cb->repBytesLeft = bytes;
	client->bytesSent += cb->repBytesLeft;
    }
    return (OUT_READY);
}

/*
 * RevHangupClient - Send message to helper to close a specified client
 *
 * This routine is used to get the helper process to initiate the client
 * shutdown procedure.
 */

static int 
RevHangupClient (struct client *client)
{
    struct shortcommand hm;
    if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(hm))
	return (BLOCKED);
    if (DEBUG_FLOW)
	buginf("\nXremote%t: sending REPLY_CLOSE_CLIENT for client %d", 
	       stdio->ttynum, client->XRemId);
    hm.opcode = REPLY_CLOSE_CLIENT;
    hm.client = client->XRemId;
    BufferStuff(sizeof(hm), &hm, &cb->lineBuf->outbuf);
    return (OUT_READY);
}

/*
 * RevHangupReply - Respond to the helper's shutdown request
 *
 * This routine sends the response when the helper sends a shutdown
 * request. No further messages will be sent on this connection.
 */

static int 
RevHangupReply (struct client *client)
{
    struct shortcommand hm;

    if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(hm))
	return (BLOCKED);
    if (DEBUG_FLOW)
	buginf("\nXremote%t: sending REPLY_HANGUP_REPLY for client %d", 
	       stdio->ttynum, client->XRemId);
    hm.opcode = REPLY_HANGUP_REPLY;
    hm.client = client->XRemId;
    BufferStuff(sizeof(hm), &hm, &cb->lineBuf->outbuf);
    return (OUT_READY);
}

static int 
RevMaybeChangeClient (struct client *client)
{
    struct shortcommand cm;

    if (client == cb->lastReplyClient)
	return (OUT_READY);
    if (BufferSlack(&cb->lineBuf->outbuf) < sizeof(cm))
	return (BLOCKED);
    cm.opcode = REPLY_CHANGE_CLIENT;
    cm.client = client->XRemId;
    BufferStuff(sizeof(cm), &cm, &cb->lineBuf->outbuf);
    cb->lastReplyClient = client;
    if (DEBUG_REPLIES || DEBUG_FLOW) {
	buginf("\nXremote%t: requesting switching to client %d",
	    stdio->ttynum, cb->lastReplyClient->XRemId);
    }
    return (OUT_READY);
}

static void
RevStuffDelta (
    struct client *client,
    int bytes,
    unsigned char *buf)
{
    int	i = MAXBYTESDIFF + 1;
    int	j = MAXBYTESDIFF + 1;
    int	k = MAXBYTESDIFF + 1;
    int	l = MAXBYTESDIFF + 1;
    char debug_buf[128];
    char *debug_ptr = debug_buf;

    int		m;

    for (m = cb->nextReplyDelta, i = 0; i < cb->activeReplyDeltas; i++) {
	if (m==0)
	    m = NDELTAS;
	m--;
	if (cb->ReplyDeltas[m].length == bytes)
	    j = BytesDiff(buf, cb->ReplyDeltas[m].buf, bytes);
	else
	    j = MAXBYTESDIFF + 1;	/* don't use this slot */
	if (j < l) {
	    k = m;
	    l = j;
	}
    }
    if (l > MAXBYTESDIFF || sizeof(struct shortcommand) + l * 
	sizeof(struct delta) >= bytes)
      BufferStuff(bytes, buf, &cb->lineBuf->outbuf);
    else {
	struct shortcommand sc;
	sc.opcode = REPLY_DELTA;
	sc.client = ((l << 4) & 0xf0) | (k & 0xf);
	BufferStuff(sizeof(sc), &sc, &cb->lineBuf->outbuf);
	j = 0;
	for (i = 0; i < bytes; i++)
	    if (buf[i] != cb->ReplyDeltas[k].buf[i]) {
		if (DEBUG_DELTAS) {
		    if (j == 0)
		      debug_ptr += sprintf(debug_ptr, "\nXremote%t: REPLY(%d)",
					   stdio->ttynum, k);
		    debug_ptr += sprintf(debug_ptr, "b%d:%02x>%02x",
					i, cb->ReplyDeltas[k].buf[i],
					buf[i]);
		    if (j + 1 == l)
			buginf(debug_buf);
		    else
		      debug_ptr += sprintf(debug_ptr, ", ");
		}
		tempDeltaBuf[j].byte = i;
		tempDeltaBuf[j++].data = buf[i] - cb->ReplyDeltas[k].buf[i];
	    }
	BufferStuff(j * sizeof(struct delta), tempDeltaBuf, &cb->lineBuf->outbuf);
	/*
	 * printf("sent delta'd reply: "); for(i=0;i<bytes;i++)printf("%d
	 * ",*(unsigned char *)(buf+i)); printf("\n"); fflush(stdout);
	 */
    }
    bcopy(buf, cb->ReplyDeltas[cb->nextReplyDelta].buf, bytes);
    cb->ReplyDeltas[cb->nextReplyDelta].length = bytes;
    cb->nextReplyDelta = (cb->nextReplyDelta + 1) % NDELTAS;
    if (cb->activeReplyDeltas < NDELTAS)
	cb->activeReplyDeltas++;
}

/*
 * Process requests from the serial line, and distribute them on the
 * appropriate X connection.
 */

static void RevDoRequests (void)
{
    struct shortcommand temp;
    int             i;
    struct client *curReqClient;
    xReq	rh;

    while (cb->lineBuf->inbuf.count) {

	curReqClient = cb->clientTable[cb->selectedClient];

	if (cb->reqBytesLeft > 0) {
	    if (curReqClient) {
		cb->reqBytesLeft -=
		  BufferMove(cb->reqBytesLeft,
			     &cb->lineBuf->inbuf,
			     (curReqClient->state == CS_AWAIT_HANGUP_REPLY
			      || curReqClient->state == CS_HANGUP) ? NULL :
			     &curReqClient->buf->outbuf);
		if (cb->reqBytesLeft > 0)
		  return;
		else
		  continue;
	    } else {
		buginf("\nXRemote%t: "
		       "Client %d not found with %d bytes to move",
		       stdio->ttynum, cb->selectedClient, 
		       cb->reqBytesLeft);
		xremote_reset(cb, "Client not found when moving request");
		return;
	    }
	}
	i = *(unsigned char *) (cb->lineBuf->inbuf.tail);
	if (DEBUG_REQUESTS)
	    buginf("\nXremote%t: Got request type %x", stdio->ttynum, i);
	switch (i) {
	case REQ_CHANGE_CLIENT:
	    if (cb->lineBuf->inbuf.count < sizeof(temp))
		return;
	    BufferYank(sizeof(temp), &cb->lineBuf->inbuf, &temp);
	    cb->selectedClient = temp.client;
	    curReqClient = cb->clientTable[cb->selectedClient];
	    if (!curReqClient) {
		buginf("\nXremote%t: "
		       "REQ_CHANGE_CLIENT client %d nonexistent",
		       stdio->ttynum, temp.client);
	    }
	    if (DEBUG_FLOW)
	      buginf("\nXRemote%t: "
		     "New requesting client is %d(%s)",
		     stdio->ttynum, cb->selectedClient,
		     client_state(curReqClient));
	    break;

	case REQ_DIAL:
	    if (cb->lineBuf->inbuf.count < sizeof(temp))
	      return;

	    BufferYank(sizeof(temp), &cb->lineBuf->inbuf, &temp);

	    if (cb->clientTable[temp.client]) {
		buginf("\nXremote%t: Already have client %d and got REQ_DIAL",
		       temp.client);
		xremote_reset(cb, "Duplicate client");
		return;
	    }

	    curReqClient = xremote_reverse_connect(temp.client);
	    if (!curReqClient) {
		/* Come up with a way to do a graceful failure */
		xremote_reset(cb, "Failed to connect to X server");
		return;
	    }
	    cb->selectedClient = temp.client; /* Set new client number */
	    break;

	case REQ_HANGUP: {
	    struct client  *cClient;

	    /* Ensure the entire request is here */

	    if (cb->lineBuf->inbuf.count < sizeof(temp))
	      return;

	    BufferYank(sizeof(temp), &cb->lineBuf->inbuf, &temp);
	    
	    if (!(cClient = cb->clientTable[temp.client])) {
		buginf("\nXremote%t: "
		       "REQ_HANGUP client %d already gone",
		    stdio->ttynum, temp.client);
		break;
	    }

	    if (DEBUG_REQUESTS || DEBUG_FLOW)
	      buginf("\nXremote%t: RevDoRequests: CS_HANGUP_REPLY %d from %s",
		     stdio->ttynum,
		     temp.client,
		     client_state(cClient));

	    cClient->state = CS_HANGUP_REPLY;
	    break;
	}

	case REQ_DELTA:
	    {
		struct deltacommand dc;
		int             deltaIndex, deltaBytesDiff, deltabytes;
		int reqbytes;
		xReq  *drprefix = (xReq *) tempReqBuf;
		char debug_buf[128];
		char *debug_ptr = debug_buf;

		if (!curReqClient) {
		    buginf("\nXremote%t: REQ_DELTA client %d does not exist",
			   stdio->ttynum, cb->selectedClient);
		    xremote_reset(cb,
				      "Client not found for delta request");
		    return;
		}

		if (BufferSlack(&curReqClient->buf->outbuf) < MAXDELTASIZE)
		    return;
		if (cb->lineBuf->inbuf.count < sizeof(dc.sc))
		    return;
		BufferPreview(sizeof(struct shortcommand), 
			      &cb->lineBuf->inbuf, &dc.sc);
		deltaIndex = dc.sc.client & 0xf;
		deltaBytesDiff = (dc.sc.client >> 4) & 0xf;
		deltabytes = sizeof(dc.sc) + 
		  deltaBytesDiff * sizeof(struct delta);
		if (cb->lineBuf->inbuf.count < deltabytes)
		    return;
		BufferYank(deltabytes, &cb->lineBuf->inbuf, &dc);
		bcopy(cb->ReqDeltas[deltaIndex].buf, tempReqBuf, MAXDELTASIZE);
		for (i = 0; i < deltaBytesDiff; i++) {
		    if (DEBUG_DELTAS) {
			if (i == 0)
			  debug_ptr += sprintf(debug_ptr,
					       "\nXremote%t: REQ(%d)",
					       stdio->ttynum, deltaIndex);
			debug_ptr += sprintf(debug_ptr, "b%d:%02x>%02x",
					     dc.deltas[i].byte,
			tempReqBuf[dc.deltas[i].byte], dc.deltas[i].data);
			if (i + 1 == deltaBytesDiff)
			  buginf(debug_buf);
			else
			  debug_ptr += sprintf(debug_ptr, ", ");
		    }
		    tempReqBuf[dc.deltas[i].byte] += dc.deltas[i].data;
		}
		reqbytes = 4 * HostUnswapWord(curReqClient, 
						  drprefix->length);
		if (!(curReqClient->state == CS_AWAIT_HANGUP_REPLY)
		    && !(curReqClient->state == CS_HANGUP))
		  BufferStuff(reqbytes, tempReqBuf, 
				&curReqClient->buf->outbuf);
		bcopy(tempReqBuf, cb->ReqDeltas[cb->nextReqDelta].buf,
		      reqbytes);
		cb->ReqDeltas[cb->nextReqDelta].length = reqbytes;
		cb->nextReqDelta = (cb->nextReqDelta + 1) % NDELTAS;
	    }
	    break;

	default:
	    if (!curReqClient) {
		buginf("\nXremote%t: Generic request %x client %d does "
		       "not exist",
		       stdio->ttynum, i, cb->selectedClient);
		xremote_reset(cb, "Client not found for request");
		return;
	    } else {
		if (DEBUG_REQUESTS)
		  buginf("\nXremote%t: DoRevRequests state=%s(%d)",
			 stdio->ttynum, client_state(curReqClient),
			 curReqClient->NeedConnectionInfo);
	    }

	    if (curReqClient->state == CS_NEW) {
		xConnClientPrefix prefix;

		if (cb->lineBuf->inbuf.count < sizeof(xConnClientPrefix))
		  return;

		BufferPreview(sizeof(xConnClientPrefix),
			      &cb->lineBuf->inbuf, &prefix);
		switch (prefix.byteOrder) {
		  case 'B':
		    curReqClient->LSBfirst = FALSE;
		    break;
		    
		  case 'l':
		    curReqClient->LSBfirst = TRUE;
		    break;

		  default:
		    buginf("\nXremote%t: Bad byte order specifier for client %d",
			   stdio->ttynum, curReqClient->XRemId);
		    xremote_reset(cb, "Bad byte order specifier for new client");
		    return;
		}

		cb->reqBytesLeft = sizeof(xConnClientPrefix)
		  + pad(HostUnswapWord(curReqClient, prefix.nbytesAuthProto))
		  + pad(HostUnswapWord(curReqClient, prefix.nbytesAuthString));
		if (DEBUG_REQUESTS | DEBUG_FLOW)
		  buginf("\nXRemote%t: DoRevRequests client %d state change"
			 " from CS_NEW to CS_ACTIVE", stdio->ttynum,
			 cb->selectedClient);
		curReqClient->state = CS_ACTIVE;

		break;
	    }

	    if (cb->lineBuf->inbuf.count < sizeof(rh))
	      return;
	    if (BufferSlack(&curReqClient->buf->outbuf) < sizeof(rh))
	      return;

	    BufferPreview(sizeof(rh), &cb->lineBuf->inbuf, &rh);
	    cb->reqBytesLeft = 4 * HostUnswapWord(curReqClient,
						      rh.length);

	    if (cb->reqBytesLeft <= MAXDELTASIZE) {
		if (cb->lineBuf->inbuf.count < cb->reqBytesLeft) {
		    cb->reqBytesLeft = 0;
		    return;
		}
		BufferPreview(cb->reqBytesLeft, &cb->lineBuf->inbuf, 
			      cb->ReqDeltas[cb->nextReqDelta].buf);
		cb->ReqDeltas[cb->nextReqDelta].length = cb->reqBytesLeft;
		cb->nextReqDelta = (cb->nextReqDelta + 1) % NDELTAS;
	    }
	    if (DEBUG_REQUESTS)
	      buginf("\nXremote%t: client %d request: %d, size %d",
		     stdio->ttynum, cb->selectedClient,
		     rh.reqType, cb->reqBytesLeft);
	    break;
	}
    }
}

/*
 * xremote_reverse_connect - Connect to the X server
 */

static struct client *xremote_reverse_connect (int id)
{
    struct client *client;
    tcbtype *tcb;
    addrtype faddr;
    int status;

    if (cb->clientTable[id]) {
	xremote_reset(cb, "Client already exists and got a dial message");
	return(NULL);
    }

    faddr.type = ADDR_IP;
    faddr.ip_addr =  cb->reverse_ipaddr;
    tcb = tcp_open(NULL, cb->reverse_display, &faddr, 0, NULL, &status,NULL);
    if (!tcb) 
      return(NULL);

    client = xremote_create_client(cb, id);
    if (!client) {
	tcp_abort(tcb);
	return(NULL);
    }

    client->transport = TRANSPORT_TCP;
    client->tc = tcb;

    tcb->tty = stdio;
    tcb->ttynum = stdio->ttynum;

    return(client);
}

/*
 * RevScanInput - Scan the input for the start sequence
 */

static boolean RevScanInput (
     xrcontext *ctx,
     int ch)
{
    ch &= 0xff;			/* Trim any garbage */

    if (ctx->reverse_inescape) {	/* Should we escape this character */
	ctx->reverse_inescape = FALSE;	/* Yes, get out of escape state */
	if (ch == PPP_FRAME_END) {
	    ctx->reverse_scanptr = 0;	/* Reset scan pointer */
	    ctx->reverse_crc = PPPINITFCS; /* Reset FCS accumulation */
	    return(FALSE);	/* Not a valid packet */
	}
	ch = PPP_ESCAPE_MECHANISM(ch);	/* Unescape the character */
    } else {
	switch (ch) {
	  case PPP_FRAME_END:

	    if ((ctx->reverse_scanptr == 0) || /* Ignore multiple end */
		(ctx->reverse_crc != PPPGOODFCS) || /* Got a CRC error */
		(ctx->reverse_scanptr < PPP_FCS_SIZE)) {
		ctx->reverse_scanptr = 0; /* Reset scan pointer */
		ctx->reverse_crc = PPPINITFCS; /* And FCS accumulator */
		return(FALSE);
	    }

	    return(TRUE);	/* Got a good packet! */

	  case PPP_FRAME_ESC:
	    ctx->reverse_inescape = TRUE;
	    return(FALSE);
	}
    }

    /*
     * By now, we have an input character to accumulate in the FCS
     */

    FCS_ACCUMULATE(ctx->reverse_crc, ch);	/* Accumulate CRC */
    ctx->reverse_scanptr++;	/* Cound another character */
    return(FALSE);		/* Not a packet yet */
}

/*
 * xremote_reverse_daemon - Process which watches reverse XRemote
 */

#define MAXLEN 80

static forktype xremote_reverse_daemon (
     tt_soc *tty)
{
    vectortype *tmpvector;
    boolean entered_xremote = FALSE;
    int count;
    int ch;
    char buff[MAXLEN];

    /*
     * Attempt to establish the XRemote context
     */

    cb = xremote_new_context(tty, TRUE); /* Create a new context */
    if (!cb) {
	tty->ovector = tty->ivector; /* Do I/O to network connection */
	printf(nomemory);	/* Give the bad news */
	async_daemon_kill(tty);	/* And kill ourselves */
    }
    cb->reverse_crc = PPPINITFCS; /* Init FCS accumulation */

    /*
     * Set up the connection to the user
     */

    async_daemon_attach(tty);	/* Attach to the line and check password */

    /*
     * Get the X display location
     */

    tmpvector = tty->ovector;	/* Save the output vector */
    tty->ovector = tty->ivector; /* Do I/O to the network connection */

    printf("\n\n --- Outbound XRemote service ---\n\n");

    while ((!modemcheck(tty)) && (!cb->reverse_ipaddr)) {
	printf("Enter X server name or IP address: ");
	if (!rdtty(buff, MAXLEN) || 
	    !parse_hostname(buff, &cb->reverse_ipaddr)) {
	    printf("\n%% Bad IP address");
	    continue;
	}

	cb->reverse_display = 
	  get_num_in_range("Enter display number",
			   10, 0,
			   65536 - X_TCP_PORT, 0)
	    + X_TCP_PORT;

    }

    printf("\n\n\n"
	   " --- Connecting to tty%t... please start up XRemote on the "
	   "remote system ---\n\n",
	   tty->ttynum);
    flush();

    tty->ovector = tmpvector;	/* Restore vector */

    if (modemcheck(tty)) {
	xremote_shutdown(tty);
	async_daemon_kill(tty);
    }

    /*
     * Move data back and forth until a disconnect or until we
     * see an XRemote start sequence.
     */

    while ((!entered_xremote) && (!modemcheck(tty))) {
	tty_dallytimer(tty);
	edisms((blockproc *)ttyBLOCK,(long)(tty));
	GET_TIMESTAMP(tty->conntop->idletime);
	if (tty->statbits & (IDLETIMEOUT + CARDROP)) {
	    if (modem_debug && (tty->statbits & IDLETIMEOUT))
	      buginf("\nTTY%t: Inactivity timeout", tty->ttynum);
	    break;
	}

	/* Check for port changes on the connection */

	if (tty->conntop->status & CONN_CHANGED 
	    && conn_changed(tty->conntop))
	  conn_report(tty->conntop);

	/*
	 * Move data from the network connection to the async line.
	 * Note that the tty block input vector is actually the network
	 * input vector, and the output vector is the real tty vector.
	 */

	if (inputpending(tty)) {
	    count = TTYDAEMON_OUTLIMIT(tty);	/* set a limit */
	    while (--count) {
		ch = getc(tty);	/* Get byte from protocol */
		if (ch == -1)
		  break;	/* No bytes just now */

		putc(tty, ch);	/* Send a byte to the async line */
	    }
	}
	(void)startoutput(tty);	/* Start up the output */

	/*
	 * Move data from the async line to the network connection.
	 */

	tmpvector = tty->ivector; /* Save input vector */
	tty->ivector = tty->ovector; /* Input becomes old output */
	tty->ovector = tmpvector; /* Output becomes old input */

	if (inputpending(tty)) {
	    count = TTYDAEMON_INLIMIT(tty);
	    while (--count) {
		ch = getc(tty);	/* Get byte from async line */
		if (ch == -1)
		  break;	/* No bytes just now */

		if (RevScanInput(cb, ch)) { /* Check for switch */
		    entered_xremote = TRUE;
		    break;
		}
		putc(tty, ch);	/* Send a byte to the protocol */
	    }
	}
	(void)startoutput(tty);	/* Make sure output is running */

	/*
	 * Reset the I/O vectors and check if connection is still active
	 */

	tmpvector = tty->ivector; /* Save input vector */
	tty->ivector = tty->ovector; /* Input becomes old output */
	tty->ovector = tmpvector; /* Output becomes old input */

    }

    if (!entered_xremote)	/* Get out if we didn't connect */
      async_daemon_kill(tty);

    /*
     * Set up to send a final message to the network connection
     */

    tmpvector = tty->ovector;	/* Save the output vector */
    tty->ovector = tty->ivector; /* Do I/O to the network */
    printf("\n\n\n --- Entering XRemote on tty%t ---\n\n", tty->ttynum);
    flush();
    while (outputpending(tty))
      process_sleep_for(ONESEC/2);

    /*
     * Set up to do I/O to the async line and close the network connection
     */

    tty->ovector = tmpvector;	/* Restore the output vector */
    tty->ivector = tmpvector;	/* Set the input vector */
    if (tty->conntop) {
	conn_close(tty->conntop);
	tty->conntop = NULL;
    }

    /*
     * Run the XRemote session
     */

    BSStartup(cb->serialContext);

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
	DoIO();
	RevDoReplies();
	RevDoRequests();
    }
    xremote_shutdown(tty);	/* Shut down */
    async_daemon_kill(tty);	/* Kill ourselves */
}

/*
 * create_xremote_tty_process - Create the tty daemon for reverse XRemote
 */

boolean create_xremote_tty_process (tt_soc *tty)
{
    switch (tty->type) {
      case AUX_LINE:
      case TTY_LINE:
	tty->tiptop = cfork((forkproc *)xremote_reverse_daemon, (long)tty, 0,
			    "Reverse XRemote", tty->ttynum);
	break;

      default:
	tty->tiptop = -1;
	break;
    }
    return(TRUE);
}

