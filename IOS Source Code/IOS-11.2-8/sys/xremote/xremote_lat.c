/* $Id: xremote_lat.c,v 3.3 1996/01/22 07:53:35 mdb Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/xremote/xremote_lat.c,v $
 *------------------------------------------------------------------
 * xremote_lat.c - Xremote LAT support
 *  
 * March 27, 1991, Kevin Paul Herbert
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: xremote_lat.c,v $
 * Revision 3.3  1996/01/22  07:53:35  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  19:27:01  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:35:33  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "../lat/mtcsys.h"
#include "../lat/latsys.h"
#include "../lat/latsb.h"
#include "../lat/latob.h"
#include "../lat/latnd.h"
#include "../lat/latcb.h"
#include "../lat/laths.h"
#include "../lat/latslt.h"
#include "../lat/latmsg.h"
#include "../lat/lathi.h"
#include "../lat/latfnc.h"
#include "../lat/laterr.h"
#include "../lat/lat.h"

#include "config.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "ttysrv.h"

#include "Xproto.h"
#include "os.h"
#include "Xdmcp.h"
#include "xremote.h"
#include "xremote_local.h"
#include "xremote_lat.h"
#include "lat_xremote.h"

#include "registry.h"
#include "../h/cs_registry.regh"
#include "xrlatfont.h"


/*
 * External references
 */
extern boolean lat_debug;
extern char *lat_sloterrors[];
extern boolean xrflow_debug;

/*
 * xrlat_openBLOCK - block for a LAT connection to open
 */

static boolean xrlat_openBLOCK (struct client *client)
{
    sb_ptr sb = (sb_ptr) client->tc;

    if (!sb)
	return(FALSE);		/* Error in connection */
    if (sb->sb_rmt_ses_id != 0)
	return(FALSE);
    return(TRUE);
}

/*
 * xremote_lat_session_connect - Try to connect to a remote node
 */

boolean xremote_lat_session_connect (char *keyword)
{
    struct client *client;
    char fontname[32];
    char *fontptr = &fontname[0];
    byte service[SV_STR+1];

    if (create_lat_name(keyword, service, SV_STR, "service"))
      return(FALSE);

    if (!ls_show(service, NULL, NULL, 0)) {
	printf("\nService %s not found", keyword);
	return(FALSE);
    }

    if (!cb) {
	cb = xremote_new_context(stdio, FALSE);
	if (!cb) {
	    printf(nomemory);
	    return(FALSE);
	}
    }

    client = xremote_connect(stdio);
    if (!client) {
	printf(nomemory);
	return(FALSE);
    }

    printf("Trying %*s...", LATSTRING(service));
    flush();
    client->tc = sb_open(service,
			 client,
			 NULL,
			 XREMOTE_BUFFERSIZE / hs->hs_sb_max_data,
			 NULL,
			 NULL,
			 NULL,
			 SVC_CLASS_XWIRE);
    if (!client->tc) {
	if ((lat_error & 0xf0) == BAD_CLASS)
	  printf(" DECwindows LAT service is not available");
	else
	  lat_perror();
	CloseClient((xrcontext *)stdio->xrcb, client);
	return(FALSE);
    }

    client->transport = TRANSPORT_LAT;
    if (client->tc)
      edisms((blockproc *)xrlat_openBLOCK, (ulong)client);
    if (!client->tc) {
	CloseClient((xrcontext *)stdio->xrcb, client);
	printf("\n%% %s", cb->lat_error ? cb->lat_error : "Unknown error");
	return(FALSE);
    }

    printf(" Open");

    client = xremote_local_connect(stdio);	/* Start the local client */
    if (client) {
	sprintf(fontname, "/lat/%*s\0", LATSTRING(service));
	XSetFontPath(client, &fontptr, 1);
    }

    return(TRUE);
}

/*
 * xremote_lat_client_writable - Indicate something was written to client
 */

void xremote_lat_client_writable (struct client *client)
{
    sb_ptr sb = (sb_ptr)client->tc;

    if (sb && client->buf->outbuf.count) {
	sb_signal(sb, SB_DATA_A, 0);
    }
}

/*
 * xremote_lat_client_connected - Determine if a client is connected
 */

boolean xremote_lat_client_connected (struct client *client)
{
    return((((sb_ptr)client->tc)->sb_rmt_ses_id) != 0);
}

/*
 * xremote_lat_get_data - Try to get more data from host
 */

void xremote_lat_get_data (struct client *client)
{
    sb_ptr sb = (sb_ptr) client->tc;
    int credits;

    if (sb) {
	credits = (BufferSlack(&client->buf->inbuf) / hs->hs_sb_max_data)
	    - (sb->sb_lcl_credits + sb->sb_ext_credits);
				/* Calculate credits to extend */
	if (credits) {
	    sb_signal(sb, 0, credits); /* Update the credit count */
	}
    }
}

/*
 * xremote_lat_close_client - Close a client connection
 */

void xremote_lat_close_client (struct client *client)
{
    sb_stop((sb_ptr)client->tc, SLOT_STOP_USER);
}

/*
 * show_xremote_lat_client - Display source of LAT connection
 */

void show_xremote_lat_client (struct client *client)
{
    printf("LAT connection ");
    if (client->tc) {
	printf("from %*s", LATSTRING(((sb_ptr)client->tc)->
				     sb_cb->cb_nd->nd_node));
    } else {
	printf("closing");
    }
}

/*
 * xremote_lat_get_char - Supply data to the LAT engine
 *
 * This routine copies data from our output buffer directly into
 * LAT slot buffers.
 */

void xremote_lat_get_char (
     sb_ptr sb,				/* Pointer to session block */
     of_byte ptr,			/* Pointer to slot space */
     int max,				/* Maximum slot space available */
     int *n)
{
    int bc;

    struct client *client = (struct client *)sb->sb_sid;

    bc = BufferYank(max, &client->buf->outbuf, ptr); /* Assemble the slot */

    if (client->buf->outbuf.count) {
	sb->sb_flags |= SB_DATA_A;	/* Signal we have more data */
    }
    *n = bc;
    return;
}

/*
 * xremote_lat_put_char - Take data from the LAT engine
 *
 * This routine will take data from the LAT engine and store it in the
 * input buffer for a line. We allocate credits based on our ability
 * to buffer the data, so we will always be able to take anything sent
 * to us.
 */

void xremote_lat_put_char (
     sb_ptr sb,
     if_byte ptr,
     int len)
{
    struct client *client = (struct client *)sb->sb_sid;

    client->newdata = TRUE;	/* Mark that we have new data */
    BufferStuff(len, ptr, &client->buf->inbuf);
}

/*
 * xremote_lat_connect (hi)
 *
 * This routine is called by the LAT engine when a new connection
 * comes in. We will attempt to allocate a client context structure.
 */

boolean xremote_lat_connect (hi_ptr hi)
{
    struct client *client;
    int ttynum;
    sb_ptr sb;
    tt_soc *tty;

    hi->hi_port[hi->hi_port[0] < (OB_STR - 1) ? hi->hi_port[0] + 1 :
		OB_STR - 1 ] = 0; /* Zero terminate the string */

    ttynum = get_ttyline((char *)&hi->hi_port[1]);	

    if (ttynum >= 0) {
	tty = MODEMS[ttynum];
	client = xremote_connect(tty);
	if (client) {
	    sb = hi_open(hi, client, XREMOTE_BUFFERSIZE / hs->hs_sb_max_data);
	    if (sb) {
		if (xrflow_debug || lat_debug) {
		    buginf("\nXRemote%t: Sending LAT start for new client %d",
			   tty->ttynum, client->XRemId);
		}
		client->transport = TRANSPORT_LAT;
		client->tc = sb;
		return(TRUE);
	    }
	    CloseClient((xrcontext *)tty->xrcb, client);
	}
    }
    if (xrflow_debug || lat_debug) {
	buginf("\nXRemote: Rejecting LAT incoming connection");
    }
    hi_reject(hi, SLOT_STOP_NO_RESOURCE);
    return(TRUE);
}

void xremote_lat_session (
     sb_ptr sb,
     int reason,
     boolean *ret)
{
    struct client *client = (struct client *)sb->sb_sid;

    switch (reason) {
      case SX_REJECT:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote%t: LAT Client %d session rejected, "
		   "state = %s, reason = %s (%d)",
		   client->ctx->tty->ttynum, client->XRemId,
		   client_state(client),
		   lat_sloterrors[sb->sb_stop_reason], sb->sb_stop_reason);
	}
	if (client->state != CS_NEW) {
	    buginf("\nXRemote%t: Rejected client in wrong state",
		   client->ctx->tty->ttynum);
	    client->state = CS_HANGUP;		/* Hope for the best */
	}
	client->ctx->lat_error = lat_sloterrors[sb->sb_stop_reason];
	client->tc = NULL;
	break;

      case SX_CIRCUIT:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote%t: LAT client %d virtual circuit "
		   "disconnected, state = %s, reason = %s (%d)",
		   client->ctx->tty->ttynum, client->XRemId,
		   client_state(client),
		   lat_circuit_error(sb->sb_cb->cb_halt_reason),
		   sb->sb_cb->cb_halt_reason);
	}
	client->state = CS_HANGUP;		/* Set to hang up soon */
	client->tc = NULL;			/* Dump session pointer */
	client->ctx->lat_error = lat_circuit_error(sb->sb_cb->cb_halt_reason);
	break;

      case SX_STOP:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote%t: LAT client %d session stopped, "
		   "state = %s, reason = %s (%d)",
		   client->ctx->tty->ttynum, client->XRemId,
		   client_state(client),
		   lat_sloterrors[sb->sb_stop_reason],
		   sb->sb_stop_reason);
	}
	client->state = CS_HANGUP;		/* Set to hang up soon */
	client->tc = NULL;			/* Don't touch connection */
	client->ctx->lat_error = lat_sloterrors[sb->sb_stop_reason];
	break;

      case SX_CONNECT:
      case SX_CONNECT_HIC:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote%t: LAT client %d connection started, state = %s",
		   client->ctx->tty->ttynum, client->XRemId,
		   client_state(client));
	}
	break;

      case SX_AS_ACCESS:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote: Rejecting incoming connection attempt");
	}
	sb_reject(sb, SLOT_STOP_SRV_NOT_OFFER);
	break;

      case SX_RESOURCE:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote%t: LAT Client %d resource allocation failure, "
		   "state = %s",
		   client->ctx->tty->ttynum, client->XRemId,
		   client_state(client));
	}
	if (client->state != CS_NEW) {
	    buginf("\nXRemote%t: Rejected client in wrong state",
		   client->ctx->tty->ttynum);
	    client->state = CS_HANGUP;		/* Hope for the best */
	}
	client->ctx->lat_error = lat_sloterrors[SLOT_STOP_NO_RESOURCE];
	client->tc = NULL;
	break;

      case SX_TIMEOUT:
	if (xrflow_debug || lat_debug) {
	    buginf("\nXRemote%t: LAT Client %d solicit timeout, state = %s",
		   client->ctx->tty->ttynum, client->XRemId,
		   client_state(client));
	}
	if (client->state != CS_NEW) {
	    buginf("\nXRemote%t: Rejected client in wrong state",
		   client->ctx->tty->ttynum);
	    client->state = CS_HANGUP;		/* Hope for the best */
	}
	client->ctx->lat_error = "Solicit timeout";
	client->tc = NULL;
	break;

      default:
	*ret = FALSE;				/* We don't handle this */
	return;
    }
    *ret = TRUE;				/* Indicate we handled it */
}

void xremote_lat_ox_session (
     sb_ptr sb,
     ob_ptr ob,
     int reason,
     boolean *ret)
{
    struct client *client = (struct client *)sb->sb_sid;
    xrcontext *ctx = client->ctx;
    tt_soc *tty = ctx->tty;

    switch (reason) {
      case OX_START:
	ob->ob_name[0] = sprintf((char *)(&ob->ob_name[1]), "%t", tty->ttynum);
	ob->ob_idl[0] = 0;
	*ret = TRUE;
	return;
    }
    *ret = FALSE;
}

static void xremote_lat_font_ox_session (sb_ptr sb, ob_ptr ob,
				  int reason, boolean *ret)
{
    *ret = TRUE;
}

void xremote_show_lat (tt_soc *tty)
{
    xrcontext *ctx = (xrcontext *)tty->xrcb;
    sb_ptr sb;
    nd_ptr nd;
    int i;
    struct client *client;

    if (!ctx)
      return;

    for (i = 0; i < MAXSOCKS; i++) {
	client = ctx->clientTable[i];
	if (client && client->transport == TRANSPORT_LAT) {
	    printf("\n\ntty%t, Connection %d, XRemote LAT connection ",
		   tty->ttynum, client->XRemId);
	    sb = (sb_ptr)client->tc;
	    nd = sb->sb_cb->cb_nd;
	    if (sb) {
		printf("from %*s", LATSTRING(nd->nd_node));
		lat_status(sb, nd);
	    } else {
	      printf("closing");
	  }
	}
    }
}

void xremote_lat_init (void)
{
    reg_add_lat_sx_get_char(SVC_CLASS_XWIRE,
		      xremote_lat_get_char, "xremote_lat_get_char");
    reg_add_lat_sx_get_char(SVC_CLASS_XFONT,
		      xremote_lat_font_get_char, "xremote_lat_font_get_char");
    reg_add_lat_sx_put_char(SVC_CLASS_XWIRE,
		      xremote_lat_put_char, "xremote_lat_put_char");
    reg_add_lat_sx_put_char(SVC_CLASS_XFONT,
		      xremote_lat_font_put_char, "xremote_lat_font_put_char");
    reg_add_lat_sx_session(SVC_CLASS_XWIRE,
		      xremote_lat_session, "xremote_lat_session");
    reg_add_lat_sx_session(SVC_CLASS_XFONT,
		      xremote_lat_font_session, "xremote_lat_font_session");
    reg_add_xremote_hx_session(xremote_lat_connect, "xremote_lat_connect");
    reg_add_xremote_show_lat(xremote_show_lat, "xremote_show_lat");
    reg_add_xremote_show_lat_font(xremote_lat_font_show_sessions,
		      "xremote_lat_font_show_sessions");
    reg_add_lat_ox_session(SVC_CLASS_XWIRE,
		      xremote_lat_ox_session, "xremote_lat_ox_session");
    reg_add_lat_ox_session(SVC_CLASS_XFONT,
		      xremote_lat_font_ox_session, "xremote_lat_ox_session");
}
