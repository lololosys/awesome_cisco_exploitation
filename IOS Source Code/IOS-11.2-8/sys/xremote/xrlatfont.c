/* $Id: xrlatfont.c,v 3.3.28.2 1996/06/28 23:34:17 hampton Exp $
 * $Source: /release/112/cvs/Xsys/xremote/xrlatfont.c,v $
 *------------------------------------------------------------------
 * xrlatfont.c - XRemote LAT font handling
 *  
 * May 17, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xrlatfont.c,v $
 * Revision 3.3.28.2  1996/06/28  23:34:17  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.3.28.1  1996/03/18  22:56:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.1  1996/03/07  11:23:32  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3  1996/01/22  07:53:40  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  19:27:14  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:02:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/20  21:55:00  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:35:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"	/* includes ctype.h string.h ciscolib.h */
#undef   toupper			/* use library function call */
#include "../lat/latsys.h"
#include "../lat/latsb.h"
#include "../lat/latob.h"
#include "../lat/latls.h"
#include "../lat/latnd.h"
#include "../lat/latcb.h"
#include "../lat/laths.h"
#include "../lat/latslt.h"
#include "../lat/latmsg.h"
#include "../lat/lathi.h"
#include "../lat/latfnc.h"
#include "../lat/sysfnc.h"
#include "../lat/lat.h"

#include "../ip/ip.h"
#include "../tcp/tcp.h"

#include "Xproto.h"
#include "os.h"
#include "dwf.h"
#include "Xdmcp.h"
#include "xremote.h"
#include "xrlatfont.h"

queuetype lat_font_ctxQ;

static void xremote_lat_newstate(lat_font_context *, enum font_state);
static void xremote_lat_font_poll(lat_font_context *);
static void xremote_lat_font_version_recv(lat_font_context *, int);
static void xremote_lat_font_ready_recv(lat_font_context *, int);
static void xremote_lat_font_directory_recv(lat_font_context *, int);
static void xremote_lat_font_attributes_recv(lat_font_context *, int);
static void xremote_lat_font_properties_recv(lat_font_context *, int);
static void xremote_lat_font_glyphs_recv(lat_font_context *, int);

static void xremote_lat_font_list_reply(lat_font_context *, int, int);
static void xremote_lat_font_list_end(lat_font_context *);
static void xremote_lat_font_attributes_reply(lat_font_context *, int);
static void xremote_lat_font_properties_reply(lat_font_context *, 
					      int, int, int, int);
static void xremote_lat_font_properties_end(lat_font_context *);
static void xremote_lat_font_glyphs_reply(lat_font_context *, int, int);
static void xremote_lat_font_glyphs_end(lat_font_context *);

static boolean xremote_lat_font_glyphs(lat_font_context *, int);
static boolean xremote_lat_get_directory(lat_font_context *, char *);

extern boolean xrfiles_debug;
extern boolean lat_debug;
extern uchar *lat_sloterrors[];

/*
 * LAT font access support - data
 */

static uchar xremote_lat_font_version_send[] = { 0x6c, 0x00, 0x08, 0x00, 0x01, 0x00,
				   0x00, 0x00 };
static uchar xremote_lat_font_ready_msg[] = { 0x02, 0x00, 0x04, 0x00 };

/*
 * LAT font access support - LAT engine interface routines
 */

void xremote_lat_font_session (sb_ptr sb, int reason, boolean *ret)
{
    lat_font_context *fc = (lat_font_context *)sb->sb_sid;

    switch (reason) {
      case SX_REJECT:
	if (xrfiles_debug || lat_debug)
	  buginf("\nXRemote: LAT font session to %*s rejected, "
		 "reason = %s (%d)",
		 LATSTRING(fc->service),
		 lat_sloterrors[sb->sb_stop_reason], 
		 sb->sb_stop_reason);
	fc->sb = NULL;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	break;

      case SX_CONNECT:
      case SX_CONNECT_HIC:
	if (xrfiles_debug || lat_debug)
	  buginf("\nXRemote: LAT font session to %*s started",
		 LATSTRING(fc->service));
	BufferStuff(sizeof(xremote_lat_font_version_send), 
		    xremote_lat_font_version_send,
		    &fc->buf->outbuf);
	xremote_lat_newstate(fc, FONT_VERSION_WAIT);
	break;

      case SX_CIRCUIT:
	if (xrfiles_debug || lat_debug)
	  buginf("\nXRemote: LAT font session to %*s virtual circuit "
		 "disconnected, reason = %s (%d)",
		 LATSTRING(fc->service),
		 lat_circuit_error(sb->sb_cb->cb_halt_reason),
		 sb->sb_cb->cb_halt_reason);
	fc->sb = NULL;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	break;

      case SX_STOP:
	if (xrfiles_debug || lat_debug)
	  buginf("\nXRemote: LAT font session to %*s stopped, "
		 "reason = %s (%d)",
		 LATSTRING(fc->service),
		 lat_sloterrors[sb->sb_stop_reason],
		 sb->sb_stop_reason);
	fc->sb = NULL;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	break;

      case SX_TIMEOUT:
	if (xrfiles_debug || lat_debug)
	  buginf("\nXRemote: LAT font session to %*s solicit timeout",
		 LATSTRING(fc->service));
	fc->sb = NULL;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	break;

      case SX_RESOURCE:
	if (xrfiles_debug || lat_debug)
	  buginf("\nXRemote: LAT font session to %*s insufficient resources",
		 LATSTRING(fc->service));
	fc->sb = NULL;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	break;

      default:
	*ret = FALSE;		/* Not a font server */
	return;
    }
    *ret = TRUE;
}		 

void xremote_lat_font_get_char (sb_ptr sb, of_byte ptr, int max, int *n)
{
    int bc;
    lat_font_context *fc = (lat_font_context *)sb->sb_sid;

    bc = BufferYank(max, &fc->buf->outbuf, ptr);

    if (fc->buf->outbuf.count) {
	sb->sb_flags |= SB_DATA_A;
    }

    *n = bc;
    return;
}

void xremote_lat_font_put_char (sb_ptr sb, if_byte ptr, int len)
{
    lat_font_context *fc = (lat_font_context *)sb->sb_sid;

    BufferStuff(len, ptr, &fc->buf->inbuf);
    fc->newdata = TRUE;
}

/*
 * LAT font access support - Internal routines to manage font contexts
 */

/*
 * xremote_lat_font_find - Find (or create) a font context
 *
 * This routine is used to find a font context for a given service,
 * and create one if one does not already exist.
 *
 * We will only create a font context if we can look up the LAT service,
 * and if it offers the font service class.
 */

static lat_font_context *xremote_lat_font_find (
     byte *service)
{
    lat_font_context *fc;
    ls_ptr ls;

    /*
     * Search for a matching font context
     */

    for (fc = (lat_font_context *)lat_font_ctxQ.qhead; fc; fc = fc->next)
      if (str_equal(fc->service, service))
	return(fc);

    /*
     * We couldn't find a font context, so look for a LAT service
     */

    ls = ls_show(service, NULL, NULL, 0);
    if (!ls) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s not found by LAT",
		 LATSTRING(service));
	return(NULL);
    }
	
    if (!(ls->ls_nd->nd_classes[1] & 0x10)) {
	if (xrfiles_debug)
	  buginf("\nXremote: Service %*s on node %*s does not offer font "
		 "service", LATSTRING(service), LATSTRING(ls->ls_nd->nd_node));
	return(NULL);
    }

    /*
     * By now, we have verified that the service exists and offers LAT font
     * service. Create a new font context for it.
     */

    fc = malloc(sizeof(lat_font_context));

    if (!fc) {
	return(NULL);
    }

    fc->buf = xremote_io_buffer(XRLAT_INBUFSIZE, XRLAT_OUTBUFSIZE);
    if (!fc->buf) {
	free(fc);
	if (xrfiles_debug)
	  buginf("\nXRemote: No memory for font I/O buffer for %*s",
		 LATSTRING(service));
	return(NULL);
    }

    str_move(fc->service, service);	/* Save the service name */
    enqueue(&lat_font_ctxQ, fc);

    if (xrfiles_debug)
      buginf("\nXRemote: Created new font server "
	     "context %*s", LATSTRING(service));

    return(fc);
}

static boolean xremote_lat_waitBLOCK (
     lat_font_context *fc)
{
    if (fc->newdata)
      return(FALSE);		/* New data on the LAT connection*/

    if (fc->state == FONT_DISCONNECT)
      return(FALSE);		/* Font session disconnected */

    return(TRUE);		/* By now, nothing to do */
}

/*
 * xremote_lat_font_wait - Wait for the current request to finish
 *
 * This routine is used to wait for the current request to finish.
 * It polls the LAT connection until we reach DISCONNECT or READY
 * states. It returns TRUE if the operation was successful (no errors
 * and state as READY), FALSE otherwise.
 */

static boolean xremote_lat_font_wait (
     lat_font_context *fc)
{
    while ((fc->state != FONT_READY) && (fc->state != FONT_DISCONNECT)) {
	edisms((blockproc *)xremote_lat_waitBLOCK, (ulong)fc);
	xremote_lat_font_poll(fc);
    }

    return(!fc->error && fc->state == FONT_READY);
}

static boolean xrlf_freeBLOCK (
     lat_font_context *fc)
{
    return(fc->xrctx != NULL);
}

static void xremote_lat_font_detach (
     lat_font_context *fc)
{
    if (fc->xrctx) {
	if (xrfiles_debug)
	  buginf("\nXRemote%t: Font server %*s detached",
		 fc->xrctx->tty->ttynum, LATSTRING(fc->service));
	fc->xrctx = NULL;
    }
    fc->error = FALSE;
}

/*
 * xremote_lat_font_attach - Attach to a font server process
 *
 * This routine is used to attach an XRemote process to a font server
 * process. We ensure that no other process is using the font server.
 * We then create a LAT connection to the font server if necessary.
 * If we don't have a directory context, we establish one and create a new
 * incarnation number.
 */

static lat_font_context *xremote_lat_font_attach (
     xrcontext *ctx,
     byte *service)
{
    lat_font_context *fc;

    fc = xremote_lat_font_find(service);
    if (!fc)
      return(NULL);

    /*
     * Wait until the font server context is available
     */

    while (fc->xrctx)
	edisms((blockproc *)xrlf_freeBLOCK, (ulong) fc);

    fc->xrctx = ctx;		/* Grab the font server context */

    if (xrfiles_debug)
      buginf("\nXRemote%t: Font server %*s attached",
	     ctx->tty->ttynum, LATSTRING(fc->service));

    /*
     * If we have not yet connected to this font server, start up the
     * connection process now.
     */

    if (!fc->sb) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Connecting to font server %*s",
		 LATSTRING(fc->service));

	fc->sb = sb_open(service,
			 fc,
			 NULL,
			 XRLAT_INBUFSIZE / hs->hs_sb_max_data,
			 NULL,
			 NULL,
			 NULL,
			 SVC_CLASS_XFONT);

	if (!fc->sb) {
	    xremote_lat_font_detach(fc);
	    return(NULL);		/* Unable to open LAT connection */
	}

	fc->state = FONT_OPEN_WAIT;	/* Set as open wait */
	if (!xremote_lat_font_wait(fc)) {
	    xremote_lat_font_detach(fc);
	    return(NULL);
	}

	/*
	 * We have established a connection to the font server.
	 * We must now request a directory from the server.
	 */

	fc->incarnation++;	/* Born again! */

	if (!xremote_lat_get_directory(fc, "*") ||
	    !xremote_lat_font_wait(fc)) {
	    xremote_lat_newstate(fc, FONT_DISCONNECT);
	    xremote_lat_font_detach(fc);
	    return(NULL);
	}
    }

    return(fc);			/* It worked */
}

/*
 * LAT font access support - state machine
 */

/*
 * xremote_lat_newstate - Change to a new state
 *
 * This routine is used to handle changes in the font server connection
 * state. If the state is changed to FONT_DISCONNECT, we will abort the
 * underlying LAT connection. For any other state changes, we ensure that
 * enough LAT credits are sent to the remote system.
 */

static void
xremote_lat_newstate (lat_font_context *fc, enum font_state state)
{
    sb_ptr sb = (sb_ptr)fc->sb;
    int credits;

    if (state != fc->state) {
	if (xrfiles_debug)
	  buginf("\nXRemote: LAT font session %*s "
		 "changed state from %d to %d",
		 LATSTRING(fc->service),
		 fc->state, state);
	fc->state = state;
    }

    if (state == FONT_DISCONNECT) {
	if (lat_debug || xrfiles_debug)
	  buginf("\nXRemote: Disconnecting LAT font server session "
		 "to %*s",
		 LATSTRING(fc->service));
	if (sb)
	  sb_stop((sb_ptr)fc->sb, SLOT_STOP_USER);
	fc->sb = NULL;
	if (fc->directory) {
	    free(fc->directory);
	    fc->directory = NULL;
	}

	fc->buf->inbuf.head = fc->buf->inbuf.buf;
	fc->buf->inbuf.tail = fc->buf->inbuf.buf;
	fc->buf->inbuf.count = 0;

	fc->buf->outbuf.head = fc->buf->outbuf.buf;
	fc->buf->outbuf.tail = fc->buf->outbuf.buf;
	fc->buf->outbuf.count = 0;
	
    } else {
	if (sb) {
	    credits = (BufferSlack(&fc->buf->inbuf) /
		       hs->hs_sb_max_data) -
			 (sb->sb_lcl_credits + sb->sb_ext_credits);
	    if (credits)
	      sb_signal(sb, 0, credits);
	    if (fc->buf->outbuf.count)
	      sb_signal(sb, SB_DATA_A, 0);
	}
    }
}

/*
 * xremote_lat_font_poll - Check for new messages from the font server
 *
 * This routine is used to poll for new messages from the font server,
 * and dispatch the message to the appropriate handler routine.
 */

static void
xremote_lat_font_poll (lat_font_context *fc)
{
    int len;
    xReq rh;
    int credits;

    fc->newdata = FALSE;

    while (fc->buf->inbuf.count >= sizeof(rh)) {


	if (!fc->sb || !(((sb_ptr)fc->sb)->sb_rmt_ses_id))
	  break;		/* Nothing to do if no session */

	if (fc->state == FONT_READY)
	  break;

	/*
	 * Pick up the request header. Pick up and validate the
	 * length. Ensure that we have the entire request.
	 */

	BufferPreview(sizeof(rh), &fc->buf->inbuf, &rh);
	len = lit2h(rh.length);

	if (len > XRLAT_INBUFSIZE) {
	    if (xrfiles_debug)
	      buginf("\nXRemote: Font server %*s sent invalid length %d",
		     LATSTRING(fc->service), len);
	    fc->protoerrors++;
	    xremote_lat_newstate(fc, FONT_DISCONNECT);
	    break;
	}

	if (fc->buf->inbuf.count < len) {
	    credits = (BufferSlack(&fc->buf->inbuf) /
		       hs->hs_sb_max_data) -
			 (fc->sb->sb_lcl_credits + fc->sb->sb_ext_credits);
	    if (credits)
	      sb_signal(fc->sb, 0, credits);
	    break;
	}

	switch (rh.reqType) {
	  case LAT_FONT_VERSION:
	    xremote_lat_font_version_recv(fc, len);
	    break;

	  case LAT_FONT_READY:
	    xremote_lat_font_ready_recv(fc, len);
	    break;

	  case LAT_FONT_DIRECTORY:
	    xremote_lat_font_directory_recv(fc, len);
	    break;

	  case LAT_FONT_ATTRIBUTES:
	    xremote_lat_font_attributes_recv(fc, len);
	    break;

	  case LAT_FONT_PROPERTIES:
	    xremote_lat_font_properties_recv(fc, len);
	    break;

	  case LAT_FONT_GLYPHS:
	    xremote_lat_font_glyphs_recv(fc, len);
	    break;

	  default:
	    if (xrfiles_debug)
	      buginf("\nXRemote: Font server %*s sent unknown reply %x, "
		     "state %d",
		     LATSTRING(fc->service), rh.reqType, fc->state);
	    BufferMove(len, &fc->buf->inbuf, NULL);
	    break;
	}
    }
}

/*
 * Protocol handlers for messages received from the font server
 */

/*
 * xremote_lat_font_version_recv - Handle a received version message
 *
 * This routine is used to handle a received version message. The
 * message is only valid in FONT_VERSION_WAIT state. We reply with
 * a ready message, and go into FONT_READY_WAIT state.
 */

static void
xremote_lat_font_version_recv (lat_font_context *fc, int len)
{
    if (fc->state != FONT_VERSION_WAIT) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent version message in state %d",
		 LATSTRING(fc->service),
		 fc->state);
	fc->protoerrors++;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	return;
    } else {
	if (BufferSlack(&fc->buf->outbuf) <
	    sizeof(xremote_lat_font_ready_msg))
	  return;

	BufferStuff(sizeof(xremote_lat_font_ready_msg), 
		    xremote_lat_font_ready_msg,
		    &fc->buf->outbuf);
	xremote_lat_newstate(fc, FONT_READY_WAIT);
    }
    BufferMove(len, &fc->buf->inbuf, NULL);
}

/*
 * xremote_lat_font_ready_recv - Handle a received ready message
 *
 * This routine handles received ready messages. Depending on the state,
 * we will call an appropriate "end" callback handler, and change into
 * FONT_READY state.
 */

static void
xremote_lat_font_ready_recv (lat_font_context *fc, int len)
{
    enum font_state old_state = fc->state;

    BufferMove(len, &fc->buf->inbuf, NULL);

    xremote_lat_newstate(fc, FONT_READY);

    switch (old_state) {
      case FONT_READY_WAIT:
	break;

      case FONT_LIST:
	xremote_lat_font_list_end(fc);
	break;

      case FONT_GET_PROPERTIES:
	xremote_lat_font_properties_end(fc);
	break;

      case FONT_GET_GLYPHS:
	xremote_lat_font_glyphs_end(fc);
	break;

      default:
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent ready message, state = %d",
		 LATSTRING(fc->service), old_state);
	fc->protoerrors++;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	return;
    }
}

/*
 * xremote_lat_font_directory_recv - Handle a received directory message
 *
 * This is the handler for received directory messages. We ensure that
 * we are in the proper state, and that the message is valid, and then
 * pass it off to the directory callback handler.
 * 
 * We remain in FONT_LIST state, as the server will send us a directory
 * message for each file.
 */

static void
xremote_lat_font_directory_recv (lat_font_context *fc, int len)
{
    int filenum;
    int namelen;
    lat_font_list_rep list_reply;

    if (fc->state != FONT_LIST) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent directory message, "
		 "state = %d",
		 LATSTRING(fc->service), fc->state);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    if (len < sizeof(list_reply)) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent short directory message, "
		 "len = %d", LATSTRING(fc->service), len);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    BufferYank(sizeof(list_reply), &fc->buf->inbuf, &list_reply);
    filenum = h2lit(list_reply.filenum);
    namelen = h2lit(list_reply.namelen);

    if ((namelen + sizeof(list_reply)) > len) {
	if (xrfiles_debug)
	  buginf("\nXremote: Font server %*s directory len = %d, namelen = %d",
		 LATSTRING(fc->service), len, namelen);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    xremote_lat_newstate(fc, FONT_LIST);
    xremote_lat_font_list_reply(fc, filenum, namelen);
    BufferMove(len - (sizeof(list_reply) + namelen), &fc->buf->inbuf, NULL);
}

/*
 * xremote_lat_font_attributes_recv - Handle a received attributes message
 *
 * This is the handler for received attribute messages. We ensure that
 * we are in the proper state, and then pass the message contents off
 * to the handler. We then go into FONT_GET_PROPERTIES state, awaiting
 * the start of the properties messages.
 */

static void
xremote_lat_font_attributes_recv (lat_font_context *fc, int len)
{
    if (fc->state != FONT_GET_ATTRIBUTES) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent attributes message, "
		 "state = %d",
		 LATSTRING(fc->service), fc->state);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    if (len < sizeof(dwfInfo) + sizeof(lat_font_attr_rep)) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent attributes message, "
		 "len = %d",
		 LATSTRING(fc->service), len);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    BufferMove(sizeof(lat_font_attr_rep), &fc->buf->inbuf, NULL);

    xremote_lat_newstate(fc, FONT_GET_PROPERTIES);
    xremote_lat_font_attributes_reply(fc, len - sizeof(lat_font_attr_rep));
}

/*
 * xremote_lat_font_properties_recv - Handle a received property message
 *
 * This routine handles received property messages. It ensure that we
 * are in FONT_GET_PROPERTIES state, and ensures that the lengths
 * in the header are all valid. We remain in FONT_GET_PROPERTIES.
 */

static void
xremote_lat_font_properties_recv (lat_font_context *fc, int len)
{
    CARD16 atom;
    CARD16 namelen;
    CARD16 value;
    CARD16 valuelen;
    lat_font_property_rep prop_reply;;

    if (fc->state != FONT_GET_PROPERTIES) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent property message, "
		 "state = %d",
		 LATSTRING(fc->service), fc->state);
	return;
    }

    if (len < sizeof(prop_reply)) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent property message, "
		 "len = %d",
		 LATSTRING(fc->service), len);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    BufferYank(sizeof(prop_reply), &fc->buf->inbuf, &prop_reply);
    atom = lit2h(prop_reply.atom);
    namelen = lit2h(prop_reply.namelen);
    value = lit2h(prop_reply.value);
    valuelen = lit2h(prop_reply.valuelen);

    if ((sizeof(prop_reply) + namelen + valuelen) > len) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %s size mismatch: "
		 "total = %d, name len = %d, value len = %d",
		 LATSTRING(fc->service), len, namelen, valuelen);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    xremote_lat_newstate(fc, FONT_GET_PROPERTIES);
    xremote_lat_font_properties_reply(fc, atom, namelen, value, valuelen);
    BufferMove(len - (namelen + valuelen + sizeof(prop_reply)),
	       &fc->buf->inbuf, NULL);
}

/*
 * xremote_lat_font_glyphs_recv - Handle a received glyphs message
 *
 * This routine handles received glyph messages. The glyph message does
 * not include an actual total length, so we must calculate it by looking
 * into the dwfbCharInfo record at the start of the glyph. We remain
 * in FONT_GET_GLYPHS state.
 */

static void
xremote_lat_font_glyphs_recv (lat_font_context *fc, int len)
{
    lat_font_glyph_rep glyph_reply;
    dwfbCharInfoNoAttr info;
    int glyphlen;
    int boxH;
    int boxW;
    int charnum;

    if (fc->state != FONT_GET_GLYPHS) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent glyph message, state = %d",
		 LATSTRING(fc->service), fc->state);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    if (len < sizeof(glyph_reply)) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent glyph message, len = %d",
		 LATSTRING(fc->service), len);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    BufferYank(sizeof(glyph_reply), &fc->buf->inbuf, &glyph_reply);
    charnum = h2lit(glyph_reply.charnum);
    
    if (charnum < fc->min_glyph || charnum > fc->max_glyph) {
	if (xrfiles_debug)
	  buginf("\nXremote: Font server %*s send glyph %d, "
		 "min = %d, max = %d",
		 LATSTRING(fc->service),
		 charnum, fc->min_glyph, fc->max_glyph);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    /*
     * Calculate the size of the glyph data itself. This requires us
     * to look at the dwfbCharInfo record to calculate the number of
     * bits in the character.
     */

    BufferPreview(sizeof(info), &fc->buf->inbuf, &info);
    boxH = info.ascent + info.descent;
    boxW = info.rightSideBearing - info.leftSideBearing;

    if (boxH < 0 || boxW < 0) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font reply from %*s for character %d "
		 "has boxH = %d and boxW = %d",
		 LATSTRING(fc->service), charnum, boxH, boxW);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    glyphlen = (((boxW * boxH) + 7) / 8) + fc->charinfosize;

    if (glyphlen + sizeof(glyph_reply) > len) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent glyph %d, "
		 "total len = %d, glyph len = %d",
		 LATSTRING(fc->service), charnum, len, glyphlen);
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	fc->protoerrors++;
	return;
    }

    xremote_lat_newstate(fc, FONT_GET_GLYPHS);
    xremote_lat_font_glyphs_reply(fc, charnum, glyphlen);
    BufferMove(len - (glyphlen + sizeof(glyph_reply)), &fc->buf->inbuf,
	       NULL);
}

/*
 * Callback routines. These routines are called by the receive handlers
 * when a message is received which is of interest to the font reading
 * process. These routines actually construct the font file.
 */

/*
 * xremote_lat_font_list_reply - Handle a callback for a file name entry
 *
 * For each file entry, we create a lat_font_dir_hdr header, followed
 * by the actual file data.
 */

static void
xremote_lat_font_list_reply (
    lat_font_context *fc,
    int filenum,
    int namelen)
{
    lat_font_dir_hdr entry;

    /*
     * If we had an error, just ignore the message
     */

    if (fc->error) {
	BufferMove(namelen, &fc->buf->inbuf, NULL);
	return;
    }

    /*
     * Ensure that we can fit this entry plus the list terminator
     */

    if (((fc->dirfill - fc->directory) + (sizeof(entry) * 2) + namelen) >
	xr_tftp_buffersize) {
	fc->error = TRUE;		/* Buffer overflow */
	fc->memerrors++;
	if (xrfiles_debug)
	  buginf("\nXRemote: LAT font directory buffer too small for "
		 "server %*s",
		 LATSTRING(fc->service));
	BufferMove(namelen, &fc->buf->inbuf, NULL);
	return;
    }

    /*
     * Construct an entry and add it to the buffer
     */

    entry.len = namelen;
    entry.num = filenum;

    bcopy((char *)&entry, fc->dirfill, sizeof(entry));
    BufferYank(namelen, &fc->buf->inbuf, fc->dirfill + sizeof(entry));

    fc->dirfill += sizeof(entry) + namelen;
    fc->dirfiles++;
}

/*
 * xremote_lat_font_list_end - Mark the end of the font list
 *
 * This routine is called at the end of the received directory messages.
 * We put a zero marker at the end of the directory area.
 */

static void
xremote_lat_font_list_end (lat_font_context *fc)
{
    if (!fc->dirfiles) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s sent empty directory",
		 LATSTRING(fc->service));
	fc->protoerrors++;
	xremote_lat_newstate(fc, FONT_DISCONNECT);
	return;
    }

    if (!fc->error)
      memset(fc->dirfill, 0, sizeof(lat_font_dir_hdr));
}

/*
 * xremote_lat_font_attributes_reply - Handle an attribute reply
 *
 * This routine is used to handle a received attribute reply. We will
 * construct a dwfPrefix header, move in the dwfInfo record from the
 * reply, and then set up for the reception of the properties.
 */

static void
xremote_lat_font_attributes_reply (lat_font_context *fc, int infolen)
{
    dwfPrefix *dwfpP = (dwfPrefix *)fc->xrctx->currentFileData;
    dwfInfo *dwfiP = (dwfInfo *)(dwfpP + 1);
    char *hdrP = fc->header;
    lat_font_dir_hdr hdr;

    bcopy(hdrP, (char *)&hdr, sizeof(hdr));	/* Fetch header */
    hdrP += sizeof(hdr);			/* Point to the string */

    if ((sizeof(dwfPrefix) + infolen + hdr.len) > 
	fc->xrctx->currentFileDataLen) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s, dwfInfo overflow, "
		 "infolen = %d, hdrlen = %d",
		 LATSTRING(fc->service), infolen, hdr.len);
	fc->error = TRUE;
	fc->memerrors++;
	BufferMove(infolen, &fc->buf->inbuf, NULL);
	return;
    }

    dwfpP->headerLength = h2lit(infolen);	/* Set the header length */
    dwfpP->nameLength = h2lit(hdr.len);		/* And the file name length */

    BufferYank(infolen, &fc->buf->inbuf, (char *)dwfiP);
    bcopy(hdrP, ((char *)dwfiP) + infolen, hdr.len); /* Move name */

    /*
     * Now calculate the size of the properties area and set the fill
     * pointer to the beginning of the property string area
     */

    fc->propnum = 0;
    fc->propoff = 0;
    fc->property = ((char *)(dwfpP + 1)) + infolen + hdr.len;
    fc->propstr = fc->property +
      (sizeof(PropInfoRec) * h2lit(dwfiP->numProps));
    dwfpP->propLength = h2lit(sizeof(PropInfoRec) * h2lit(dwfiP->numProps));

    /*
     * Ensure that there is enough room to hold all of the property
     * pointers.
     */

    if (((sizeof(PropInfoRec) * h2lit(dwfiP->numProps)) + 
	 (sizeof(dwfPrefix) + infolen + hdr.len)) >
	fc->xrctx->currentFileDataLen) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s, PropInfoRec overflow",
		 LATSTRING(fc->service));
	fc->error = TRUE;
	fc->memerrors++;
    }
}

/*
 * xremote_lat_font_properties_reply - Handle a received property
 *
 * This is the handler for a received property message. A property
 * message consists of an atom and a value. Either of these may be
 * a string or an integer, but not both.
 *
 * We produce a PropInfoRec in the output file.
 */

static void
xremote_lat_font_properties_reply (
    lat_font_context *fc,
    int atom,
    int namelen,
    int value,
    int valuelen)
{
    dwfPrefix *dwfpP = (dwfPrefix *)fc->xrctx->currentFileData;
    PropInfoRec prop;

    if (fc->error) {
	BufferMove(namelen + valuelen, &fc->buf->inbuf, NULL);
	return;
    }
    /*
     * Check for buffer overflow in the string area. We have already
     * checked the property area.
     */

    if ((namelen + valuelen + fc->propoff + (fc->propstr - (char *)dwfpP)) >
	fc->xrctx->currentFileDataLen) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Server %*s, font string overflow",
		 LATSTRING(fc->service));
	fc->error = TRUE;
	fc->memerrors++;
	BufferMove(namelen + valuelen, &fc->buf->inbuf, NULL);
	return;
    }

    /*
     * Handle the atom name itself
     */

    if (namelen) {
	prop.propAtom.offset = vaxorder(fc->propoff);
	BufferYank(namelen, &fc->buf->inbuf, fc->propstr + fc->propoff);
	fc->propoff += namelen;
	fc->propstr[fc->propoff++] = '\0';
	prop.indirectAtom = h2lit(1);
    } else {
	prop.propAtom.value = vaxorder(atom);
	prop.indirectAtom = 0;
    }

    /*
     * Now handle the atom value
     */

    if (valuelen) {
	prop.v.offset = vaxorder(fc->propoff);
	BufferYank(valuelen, &fc->buf->inbuf, fc->propstr + fc->propoff);
	fc->propoff += valuelen;
	fc->propstr[fc->propoff++] = '\0';
	prop.indirectValue = h2lit(1);
    } else {
	prop.v.value = vaxorder(value);
	prop.indirectValue = 0;
    }

    /*
     * We have moved the strings, if any, and built a PropInfoRec record
     * for this property. Now move it into the output area.
     */

    bcopy((char *)&prop, fc->property +
	  (sizeof(PropInfoRec) * fc->propnum++), sizeof(prop));
}

/*
 * xremote_lat_font_properties_end - Handle the end of the properties
 *
 * This routine is called at the end of the reception of properties.
 * We verify that the number of properties received corresponds to
 * the number of properties specified in the dwfInfo record, and set up
 * for the reception of the glyph data.
 *
 * Note that we currently do not support reception of two-dimensional
 * fonts. The font server doesn't appear to send any of these. If it did,
 * the received glyph handler will have to change as well.
 */

static void
xremote_lat_font_properties_end (lat_font_context *fc)
{
    lat_font_dir_hdr hdr;
    dwfPrefix *dwfpP = (dwfPrefix *)fc->xrctx->currentFileData;
    dwfInfo *dwfiP = (dwfInfo *)(dwfpP + 1);
    int glyphs;

    if (fc->error)
      return;				/* Get out if an error occured */

    if ((fc->property + (sizeof(PropInfoRec) *
				   fc->propnum)) !=
	fc->propstr) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Server %*s, font properties %x, calculated %x",
		 LATSTRING(fc->service),
		 fc->property + (sizeof(PropInfoRec) * fc->propnum),
		 fc->propstr);
	fc->error = TRUE;
	fc->protoerrors++;
	return;
    }

    /*
     * We have validated that the property value and string areas were
     * built properly. Finish up the initialization of the dwfPrefix
     * record, and set up to read the glyph area.
     */

    dwfpP->propStringsLength = h2lit(fc->propoff);

    /*
     * We have validated that the property value and string area was
     * built properly. Now we will allocate the glyph pointers and
     * start reading the glyphs.
     */

    fc->glyphs = fc->propstr + fc->propoff + sizeof(GlyphHeader);
    fc->charinfosize = ((h2lit(dwfiP->charInfosHaveAttrs) & 1) != 0) ?
      sz_dwfbCharInfo : sz_dwfbCharInfoNoAttr;

    if ((dwfiP->minrow != dwfiP->maxrow) && (dwfiP->mincol != dwfiP->maxcol)) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Server %*s sent two-dimensional font",
		 LATSTRING(fc->service));
	fc->error = TRUE;
	fc->protoerrors++;
	return;
    }

    if (dwfiP->minrow != dwfiP->maxrow) {
	fc->min_glyph = vaxorder(dwfiP->minrow);
	fc->max_glyph = vaxorder(dwfiP->maxrow);
    } else {
	fc->min_glyph = vaxorder(dwfiP->mincol);
	fc->max_glyph = vaxorder(dwfiP->maxcol);
    }

    if (fc->min_glyph > fc->max_glyph) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Server %*s sent font, min = %d, max = %d",
		 LATSTRING(fc->service), fc->min_glyph, fc->max_glyph);
	fc->error = TRUE;
	fc->protoerrors++;
	return;
    }

    glyphs = fc->max_glyph - fc->min_glyph + 1;

    /*
     * Ensure that we have enough space for the glyph pointer area
     */

    fc->glyphoff = glyphs * sizeof(ShortGlyphPointer);
    if ((fc->glyphoff + (fc->glyphs - (char *)dwfpP)) >
	fc->xrctx->currentFileDataLen) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s, glyph pointer overflow",
		 LATSTRING(fc->service));
	fc->error = TRUE;
	fc->memerrors++;
	return;
    }

    /*
     * Pre-fill the pointers to -1
     */

    memset(fc->glyphs, 0xff, glyphs * sizeof(ShortGlyphPointer));

    /*
     * We are all set up, now get the glyphs from the server
     */

    bcopy(fc->header, (char *)&hdr, sizeof(hdr));
    xremote_lat_font_glyphs(fc, hdr.num);	/* Fetch glyphs */
}

/*
 * xremote_lat_font_glyphs_reply - Handle a received glyph message
 *
 * This routine handles received glyphs. We move the glyph into the
 * appropriate part of the output file, and set up a pointer to it.
 *
 * Note that we always produce font files with short pointers. This
 * should probably change to always producing font files with long
 * pointers.
 */

static void
xremote_lat_font_glyphs_reply (
    lat_font_context *fc,
    int charnum,
    int glyphlen)
{
    ShortGlyphPointer offset;

    if (fc->error) {
	BufferMove(glyphlen, &fc->buf->inbuf, NULL);
	return;
    }

    /*
     * Check for an overflow on the glyph data. Note that we don't
     * need to check the pointer area, as that was checked at the
     * end of the properties handling.
     */

    if ((fc->glyphoff + glyphlen + 
	 (fc->glyphs - fc->xrctx->currentFileData)) >
	fc->xrctx->currentFileDataLen) {
	if (xrfiles_debug)
	  buginf("\nXRemote: Font server %*s, glyph overflow",
		 LATSTRING(fc->service));
	fc->error = TRUE;
	fc->memerrors++;
	BufferMove(glyphlen, &fc->buf->inbuf, NULL);
	return;
    }

    /*
     * Move the glyph into the output area
     */

    BufferYank(glyphlen, &fc->buf->inbuf, fc->glyphs + fc->glyphoff);

    offset.offset = h2lit(fc->glyphoff);
    bcopy(&offset, fc->glyphs +
	  (sizeof(ShortGlyphPointer) * (charnum - fc->min_glyph)),
	  sizeof(offset));

    fc->glyphoff += glyphlen;
}

/*
 * xremote_lat_font_glyphs_end - Handle the end of the glyphs
 *
 * This routine handles reception of the end of the font glyphs. We
 * construct a GlyphHeader, fill in the magic number in the dwfPrefix,
 * and set the total file size.
 */

static void
xremote_lat_font_glyphs_end (lat_font_context *fc)
{
    GlyphHeader ghdr;
    xrcontext *ctx = fc->xrctx;
    dwfPrefix *dwfpP = (dwfPrefix *)ctx->currentFileData;

    ghdr.FirstGlyphAddress = vaxorder(fc->glyphs - 
				      ctx->currentFileData);
    ghdr.BytesOfPointersAndGlyph = vaxorder(fc->glyphoff);

    bcopy((char *)&ghdr, fc->glyphs - sizeof(GlyphHeader),
	  sizeof(GlyphHeader));

    dwfpP->magic = vaxorder(DWF_MAGIC);
    ctx->currentFileDataLen = (fc->glyphs + fc->glyphoff) -
      ctx->currentFileData;
}

/*
 * Protocol generation routines. These routines send requests to the remote
 * font server.
 */

/*
 * xremote_lat_font_list - Send a font list request
 *
 * This routine initiates the reading of a font directory. The fontname
 * argument is a X LFD (logical font description).
 */

static boolean xremote_lat_font_list (
     lat_font_context *fc,
     char *fontname)
{
    lat_font_list_req req;
    int len;

    if (fc->state != FONT_READY)
      return(FALSE);

    len = strlen(fontname);

    req.opcode = LAT_FONT_LIST_OPCODE;
    req.reserved1 = 0;
    req.reqlen = h2lit(sizeof(lat_font_list_req) + len);
    req.strlen = h2lit(len);
    req.reserved2 = 0;

    BufferStuff(sizeof(req), &req, &fc->buf->outbuf);
    BufferStuff(len, fontname, &fc->buf->outbuf);

    xremote_lat_newstate(fc, FONT_LIST);
    return(TRUE);
}

/*
 * xremote_lat_font_properties - Initiate reading of font properties
 *
 * This routine initiates the reading of font properties. It requires
 * a font number, returned from the font directory.
 */

static boolean xremote_lat_font_properties (
     lat_font_context *fc,
     ushort fontnum)
{
    lat_font_get_prop_req req;

    if (fc->state != FONT_READY)
      return(FALSE);

    req.opcode = LAT_FONT_GET_PROPERTIES;
    req.reserved1 = 0;
    req.reqlen = h2lit(sizeof(req));
    req.filenum = h2lit(fontnum);
    req.reserved2 = 0;

    BufferStuff(sizeof(req), &req, &fc->buf->outbuf);

    xremote_lat_newstate(fc, FONT_GET_ATTRIBUTES);
    return(TRUE);
}

/*
 * xremote_lat_font_glyphs - Initiate reading of font glyphs
 * 
 * This routine is used to initiate the reading of font glyphs. It is
 * passed a font file number.
 */

static boolean xremote_lat_font_glyphs (lat_font_context *fc, int fontnum)
{
    lat_font_get_glyph_req req;

    if (fc->state != FONT_READY)
      return(FALSE);

    req.opcode = LAT_FONT_GET_GLYPHS;
    req.reserved1 = 0;
    req.reqlen = h2lit(sizeof(req));
    req.filenum = h2lit(fontnum);
    req.reserved2 = 0;
    req.reserved3 = 0;
    req.reserved4 = 0;
    req.mustbe1 = 1;

    BufferStuff(sizeof(req), &req, &fc->buf->outbuf);

    xremote_lat_newstate(fc, FONT_GET_GLYPHS);
    return(TRUE);
}

/*
 * Higher level internal interface. These routines actually initiate
 * transactions with the font server. They use the protocol routines
 * defined above to do their work.
 */

/*
 * xremote_lat_get_directory
 *
 * This routine is called to initiate reading of the font directory from
 * the font server.
 *
 * Callbacks are made to xremote_lat_font_list_reply() with each file
 * in the list.
 */

static boolean xremote_lat_get_directory (lat_font_context *fc, char *fontmask)
{
    fc->directory = malloc(xr_tftp_buffersize);
    if (!fc->directory) {
	xr_font_memerr++;
	return(FALSE);
    }

    fc->error = FALSE;			/* No error initially */
    fc->dirfill = fc->directory;	/* Set directory fill pointer */
    fc->dirfiles = 0;			/* Start file counter */

    return(xremote_lat_font_list(fc, fontmask));
}

/*
 * xremote_lat_get_file - Read a font file
 *
 * This routine is used to initiate the reading of a font file from
 * a font server.
 */

static boolean xremote_lat_get_file (
     lat_font_context *fc,
     int filenum)
{
    lat_font_dir_hdr hdr;
    char *hdrP;

    hdrP = fc->directory;
    if (!hdrP) {
	if (xrfiles_debug)
	  buginf("\nXRemote: No directory found in xremote_lat_get_file "
		 "for %*s", LATSTRING(fc->service));
	return(FALSE);
    }

    while (TRUE) {
	bcopy(hdrP, &hdr, sizeof(hdr));
	if (!hdr.len) {
	    if (xrfiles_debug)
	      buginf("\nXRemote: File %d not found in xremote_lat_get_file "
		     "for %*s", filenum, LATSTRING(fc->service));
	    return(FALSE);
	}
	if (hdr.num == filenum)
	  break;
	hdrP += hdr.len + sizeof(hdr);
    }
	    
    fc->header = hdrP;		/* Save header pointer */
    fc->error = FALSE;		/* No errors have occured yet */

    if (xrfiles_debug)
      buginf("\nXRemote: Reading font %*s from %*s",
	     hdr.len, hdrP + sizeof(hdr),
	     LATSTRING(fc->service));

    return(xremote_lat_font_properties(fc, filenum));
}

/*
 * File oriented interface
 *
 * In order to be compatible with the NCD, which knows nothing of LAT font
 * loading, we dynamically create "files" to pass back to the NCD.
 *
 * If the NCD requests a file called fonts.dir, we get a directory from the
 * font server, and produce a file in the following format:
 *	line-count
 *	fontname.dwf fontname
 *
 * If the NCD requests a file called fonts.alias, we return a zero length
 * file.
 *
 * If the NCD requests a file called fontname.dwf, we request the associated
 * font from the font server, and turn it back into a dwf file.
 */

static const char fonts_dir[] = "fonts.dir";
static const char fonts_alias[] = "fonts.alias";

boolean xremote_lat_file_read (
     xrcontext *ctx,
     char *filename)
{
    lat_font_context *fc;
    int i;
    char *buff;
    lat_font_dir_hdr hdr;
    char *hdrP;
    char *fileptr;
    byte service[SV_STR];
    int namelen;

    /*
     * First, parse the "directory" portion of the filename. This
     * will be used as the service name.
     */

    i = 0;
    while (TRUE) {
	if (!filename[i])
	  return(FALSE);		/* Can't terminate here */
	if (filename[i] == '/')
	  break;			/* Get out if terminated */
	if (++i >= SV_STR)
	  return(FALSE);		/* Too long */
	service[i] = (byte)toupper(filename[i - 1]);
    }

    service[0] = i;			/* Set length */

    fileptr = &filename[i + 1];		/* Point to the filename */
    namelen = strlen(fileptr);

    if (namelen < 5)
      return(FALSE);			/* Ensure we have ?.dwf */

    /*
     * We have now parsed the service name. Look for a matching
     * font server context.
     */

    fc = xremote_lat_font_attach(ctx, service); /* Attach to the font server */
    if (!fc)
      return(FALSE);			/* We couldn't find it */

    /*
     * We have successfully attached the font server.
     */

    if (strcmp(fileptr, fonts_dir) == 0) {

	/*
	 * Reading the fonts.dir file
	 */

	hdrP = fc->directory;
	if (!hdrP) {
	    xremote_lat_font_detach(fc);
	    return(FALSE);		/* Something wrong */
	}
	buff = ctx->currentFileData;

	buff += sprintf(buff, "%d\n", fc->dirfiles);
	while (TRUE) {
	    bcopy(hdrP, (char *)&hdr, sizeof(hdr));
	    if (!hdr.len)
	      break;

	    /*
	     * Ensure that we have enough memory to add this entry.
	     * The "16" below is padding space for the newline, space,
	     * .dwf, and trailing null. The value 7 would work, but
	     * a little breathing room never hurts.
	     */

	    if (((buff - ctx->currentFileData) + (2 * hdr.len) + 16) >
		ctx->currentFileDataLen) {
		if (xrfiles_debug)
		  buginf("\nXRemote: Buffer overflow building fonts.dir "
			 "from %*s",
			 LATSTRING(fc->service));
		xremote_lat_font_detach(fc);
		return(FALSE);
	    }

	    /* Copy the font name, converting space characters to # */

	    for (i = 0; i < hdr.len; i++)
	      *buff++ = hdrP[sizeof(hdr) + i] == ' ' ? '#' :
		hdrP[sizeof(hdr) + i];

	    buff += sprintf(buff, ".dwf %*s\n",
			    hdr.len, hdrP + sizeof(hdr));
	    hdrP += hdr.len + sizeof(hdr);
	}
	ctx->currentFileDataLen = buff - ctx->currentFileData;

	fc->bytesread += ctx->currentFileDataLen;
	fc->filesread++;

	xremote_lat_font_detach(fc);		/* Detach server */
	return(TRUE);
    }

    if (strcmp(fileptr, fonts_alias) == 0) {

	/*
	 * Reading the fonts.alias file
	 */

	ctx->currentFileDataLen = 0;
	xremote_lat_font_detach(fc);		/* Detach server */
	return(TRUE);
    }

    /*
     * Ok, now see if this is for a font file. Ensure that the name
     * ends in .dwf.
     */


    namelen -= 4;		/* Get length of font name without .dwf */

    if (strcmp(fileptr + namelen, ".dwf") == 0) {

	hdrP = fc->directory;
	if (!hdrP) {		/* Shouldn't ever happen */
	    xremote_lat_font_detach(fc);
	    return(FALSE);
	}
	
	while (TRUE) {
	    bcopy(hdrP, (char *)&hdr, sizeof(hdr));
	    if (!hdr.len)	/* File not found */
	      break;

	    /*
	     * Check for filename match
	     */

	    if (hdr.len == namelen) {
		for (i = 0; i < namelen; i++) {
		    if ((fileptr[i] == '#' ? ' ' : fileptr[i]) !=
			hdrP[sizeof(hdr) + i])
		      break;
		}
		if (namelen == i) {
		    if (!xremote_lat_get_file(fc, hdr.num) ||
			!xremote_lat_font_wait(fc)) {
			xremote_lat_font_detach(fc);
			return(FALSE);
		    }
	
		    fc->bytesread += ctx->currentFileDataLen;
		    fc->filesread++;
	    
		    xremote_lat_font_detach(fc);
		    return(TRUE);
		}
	    }
	    hdrP += hdr.len + sizeof(hdr);
	}
    }
    xremote_lat_font_detach(fc);
    return(FALSE);			/* By now, no such file */
}

/*
 * Show command support
 */

void xrlatfont_show_server (void)
{
    lat_font_context *fc;

    for (fc = (lat_font_context *)lat_font_ctxQ.qhead; fc; fc = fc->next) {
	printf("\n"
	       "\n"
	       "\nLAT font load statistics for service %*s, incarnation %d:"
	       "\n  Bytes read:          %9d          Files read: %d"
	       "\n  Protocol errors:     %9d          Insufficient memory: %d",
	       LATSTRING(fc->service), fc->incarnation,
	       fc->bytesread, fc->filesread,
	       fc->protoerrors, fc->memerrors);
    }
}

void xremote_lat_font_show_sessions (void)
{
    lat_font_context *fc;
    sb_ptr sb;
    nd_ptr nd;

    for (fc = (lat_font_context *)lat_font_ctxQ.qhead; fc; fc = fc->next) {
	sb = fc->sb;
	if (sb) {
	    nd = sb->sb_cb->cb_nd;
	    printf("\n\nXRemote LAT font connection to service %*s",
		   LATSTRING(fc->service));
	    lat_status(sb, nd);
	}
    }
}

/*
 * Test routines from here on in
 */

#ifdef FONTTEST

void xremote_lat_font_test (xrcontext *ctx)
{
    char buff[80];
    byte service[SV_STR];
    int i;
    lat_font_context *fc;
    lat_font_dir_hdr hdr;
    char *hdrP;
    boolean parsed;

    while (TRUE) {

	printf(
	       "\n1 = Read a file"
	       "\n2 = Read all files quietly"
	       "\n3 = Display directory"
	       "\n4 = Display file (ascii)"
	       "\n"
	       "\nEnter selection: ");
	if (!rdtty(buff, 80))
	    return;

	if (!buff[0])
	  break;

	i = parse_unsigned(buff, &parsed);
	switch (i) {
	  case 1:
	  case 4:

	    printf("\nEnter filename: ");
	    if (!rdtty(buff, 80))
		return;
	
	    if (!buff[0])
	      return;

	    if (!ctx->currentFileData)
	      ctx->currentFileData = malloc(xr_tftp_buffersize);
	    ctx->currentFileDataLen = xr_tftp_buffersize;

	    if (xremote_lat_file_read(ctx, buff)) {

		printf("\nFile read successfully, length = %d\n",
		       ctx->currentFileDataLen);
		if (i == 4)
		  printf("%*s", ctx->currentFileDataLen,
			 ctx->currentFileData);
		else {
		    for (i = 0; i < ctx->currentFileDataLen; i++) {
			if ((i & 0xf) == 0)
			  printf("\n%04x: ", i);
			if ((i & 1) == 0)
			  printf(" ");
			printf("%02x", ((uchar *)ctx->currentFileData)[i]);
		    }
		}
		free(ctx->currentFileData);
		ctx->currentFileData = NULL;
	    } else {
		printf("\nError reading file");
	    }
	    break;

	  case 2:

	    printf("\nEnter host: ");
	    if (!rdtty(buff, 80))
		return;

	    if (!buff[0])
	      return;

	    if (!str_convert(service, (byte *)buff, SV_STR, TRUE)) {
		printf("\nInvalid service name");
		break;
	    }

	    if (!ctx->currentFileData)
	      ctx->currentFileData = malloc(xr_tftp_buffersize);
	    ctx->currentFileDataLen = xr_tftp_buffersize;

	    fc = xremote_lat_font_attach(ctx, service);
	    if (!fc) {
		printf("\nUnable to attach to service");
		break;
	    }

	    hdrP = fc->directory;
	    i = 0;
	    while (TRUE) {
		bcopy(hdrP, (char *)&hdr, sizeof(hdr));
		if (!hdr.len)
		  break;
		ctx->currentFileDataLen = xr_tftp_buffersize;

		printf("\nReading %d: %*s...",
		       hdr.num, hdr.len, hdrP + sizeof(hdr));
		if (xremote_lat_get_file(fc, hdr.num)) {
		    if (xremote_lat_font_wait(fc)) {
			printf(" OK");
		    } else {
			printf(" Failed in font_wait");
			i++;
		    }
		} else {
		    printf("Failed in get_file");
		    i++;
		}
		hdrP += hdr.len + sizeof(hdr);
	    }
	    xremote_lat_font_detach(fc);
	    printf("\nDone with %d errors", i);

	    free(ctx->currentFileData);
	    ctx->currentFileData = NULL;
	    break;

	  case 3:

	    printf("\nEnter host: ");
	    if (!rdtty(buff, 80))
		return;

	    if (!buff[0])
	      return;

	    if (!str_convert(service, (byte *)buff, SV_STR, TRUE)) {
		printf("\nInvalid service name");
		break;
	    }

	    fc = xremote_lat_font_attach(ctx, service);
	    if (!fc) {
		printf("\nUnable to attach to service");
		break;
	    }

	    hdrP = fc->directory;
	    i = 0;
	    while (TRUE) {
		bcopy(hdrP, (char *)&hdr, sizeof(hdr));
		if (!hdr.len)
		  break;
		printf("\nFile %d is %*s",
		       hdr.num, hdr.len, hdrP + sizeof(hdr));
		hdrP += hdr.len + sizeof(hdr);
	    }
	    xremote_lat_font_detach(fc);
	    break;

	  default:
	    printf("\nUnrecognized function code");
	    break;
	}
    }
}

#endif FONTTEST
