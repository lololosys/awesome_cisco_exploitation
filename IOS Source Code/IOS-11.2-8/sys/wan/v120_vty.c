/* $Id: v120_vty.c,v 3.4.6.2 1996/07/11 19:24:17 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/wan/v120_vty.c,v $
 *------------------------------------------------------------------
 * v120_vty.c - routines for v.120 interface with vty drivers
 *
 * July-1995, Bill May
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: v120_vty.c,v $
 * Revision 3.4.6.2  1996/07/11  19:24:17  sjackson
 * CSCdi60440:  Unable to Allocate IP Address From Local (Non-Default) Pool
 * Branch: California_branch
 * Configuration IDB containing pool information determined from physical
 * interface when required
 *
 * Revision 3.4.6.1  1996/03/18  22:49:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.22.3  1996/03/16  07:56:36  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.22.2  1996/03/07  11:08:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.22.1  1996/02/20  21:54:06  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/01  17:22:12  wmay
 * CSCdi50480:  ARAP session established by v.120 call dropped - put a
 * limit on the number of packets we will store to send out V.120.
 * Have the voutputblock vector reflect this limit.
 *
 * Revision 3.3  1995/11/20  23:28:18  wmay
 * CSCdi42911:  slip or ppp session not closed after first v120 call on
 * ISDN - call terminate_line, not death
 *
 * Revision 3.2  1995/11/17  18:05:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:37:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  23:30:35  wmay
 * Placeholder for CS_Arkansas_branch
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "config.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "connect.h"
#include "../os/async.h"
#include "../ui/command.h"
#include "packet.h"
#include "../if/network.h"
#include "../les/isdn_bri.h"
#include "v120.h"
#include "../ui/debug.h"
#include "v120_debug.h"
#include "dialer.h"
#include <ciscolib.h>
#include <string.h>
#include "../h/cs_registry.regh"

/*
 * Could have pointer here and allocate them when we need them
 */
static vectortype V120vector;
static connvector V120connvector;

/*
 * TTY_TO_V120
 * get v120 pointer from tty pointer
 */
static inline v120_info_type *TTY_TO_V120 (tt_soc *vty)
{
    return(vty->v120_info);
}

/*
 * v120_data_to_vty
 * Get input data packet from v120 code.  Process it so that the
 * vty can get the code.
 */
void v120_data_to_vty (v120_info_type *v120, paktype *pak)
{
    tt_soc *vty;
    uchar *dest, *src;
    ushort count;
    int c;
    boolean have_break;

    have_break = FALSE;
    if (v120->header_extension) {
	c = *pak->datagramstart;
	if (c & V120_HEAD_BREAK)
	    have_break = TRUE;
	pak->datagramstart++;
	pak->datagramsize--;
	if (c & (V120_HEAD_C1 | V120_HEAD_C2)) {
	    /* parity or stop bit error - get rid of last data byte */
	    pak->datagramsize--;
	}
	if ((c & V120_HEAD_EXT) == 0) {
	    /*
	     * hardware flow control here ?
	     * Make sure that we are talking to the remote side the same
	     */
	    pak->datagramstart++;
	    pak->datagramsize--;
	    if (v120->header_bytes == 1) {
		v120->header_bytes = 2;
		if (v120_debug)
		    buginf("\n%s - changing to 2 V.120 header bytes",
			   v120->hwptr->hw_short_namestring);
	    }
	} else {
	    /*
	     * 1 header byte received - just send 1
	     */
	    v120->header_bytes = 1;
	}
    }
		
    vty = v120->v120_vty;
    if ((vty == NULL) ||
	(pak->datagramsize == 0 && have_break == FALSE)) {
	datagram_done(pak);
	return;
    }

    /*
     * we might have to remove the parity bit on 7 databit data here
     */
    if (vty->rxservice) {
	(*(vty->rxservice))(vty, pak->datagramstart, pak->datagramsize);
	datagram_done(pak);
    } else {
	/*
	 * Indicate break at the end of the data we are sending.
	 * Do this by setting pak->enc_flags to 1.  When we hit
	 * 0 with the datagramsize, we check this flag in v120_vgetc.
	 */
	pak->enc_flags = 0;
	if (have_break) {
	    pak->enc_flags = 1; /* indicate this for v120_vgetc */
	    vty->incount++;
	    vty->totalin++;
	}

	src = dest = pak->datagramstart;
	count = pak->datagramsize;

	/*
	 * Convert the input characters - might have to convert
	 * parity and less data bits here.
	 */
	while (count > 0) {
	    count--;
	    c = *src++;
	    if (special_chars(vty, c) >= 0) {
		*dest++ = c;
	    }
	}

	pak->datagramsize = dest - pak->datagramstart;
	if (pak->datagramsize > 0 || pak->enc_flags != 0) {
	    /*
	     * We will either put this in the inheadpak field of the
	     * vty, or in the rxpakq for later processing
	     */
	    if (vty->inheadpak == NULL) {
		vty->inheadpak = pak;
	    } else {
		p_enqueue(&vty->rxpakq, pak);
	    }
	    /*
	     * Increment the incount
	     */
	    vty->incount += pak->datagramsize;
	    vty->totalin += pak->datagramsize;
	}

    }
}

/*
 * v120_vclearinput
 * Get rid off all the input packets/counts
 */
static void v120_vclearinput (tt_soc *vty)
{
    paktype *pak;

    if (vty->inheadpak) {
	datagram_done(vty->inheadpak);
	vty->inheadpak = NULL;
    }
    while ((pak = p_dequeue(&vty->rxpakq)) != NULL) {
	datagram_done(pak);
    }
    vty->incount = 0;
    vty->isavedchar = -1;
}

/*
 * v120_vgetc
 * vgetc vector from vty.  Get next character - return packets
 * when we are done
 */
static int v120_vgetc (tt_soc *vty)
{
    int ret_char;
    paktype *pak;

    /*
     * First check the saved character
     */
    if (vty->isavedchar != -1) {
	ret_char = vty->isavedchar;
	vty->isavedchar = -1;
	return(ret_char);
    }

    if (vty->statbits & (CARDROP + HANGUP + IDLE)) {
	/* dump all input packets - sure, why not ? */
	v120_vclearinput(vty);
	return(-1);
    }
	
    if (vty->incount > 0) {
	/*
	 * read characters from the pak on inheadpak
	 */
	pak = vty->inheadpak;
	if (pak != NULL) {
	    /* If datagramsize of packet is 0, and enc_flags = 1,
	     * we were saving a break character
	     */
	    if (pak->datagramsize == 0 && pak->enc_flags == 1) {
		ret_char = BREAK_CHAR;
		pak->enc_flags = 0;
	    } else {
		/*
		 * Normal character.  Get one from datagramstart,
		 * decrement datagramsize, set ESCAPED_FLAG if needed
		 */
		ret_char = *pak->datagramstart++;
		pak->datagramsize--;
		if (vty->statbits & LASTWASSPEC) {
		    vty->statbits &= ~LASTWASSPEC;
		    ret_char |= ESCAPED_FLAG;
		}
	    }
	    /*
	     * Decrement 1 from number of character.  Free the
	     * drained packet, and get the next one off the rxpakq
	     */
	    vty->incount--;
	    if (pak->datagramsize == 0 && pak->enc_flags == 0) {
		datagram_done(pak);
		vty->inheadpak = p_dequeue(&vty->rxpakq);
	    }
	    return(ret_char);
	} else {
	    /* error condition - try to recover */
	    v120_vclearinput(vty);
	    if (v120_debug)
		buginf("\nV120:incount is not 0, inheadpak is NULL");
	}
    }
    return(-1);
}

/*
 * v120_vinputpending
 * return the number of input characters
 */
static int v120_vinputpending (tt_soc *vty)
{
    int count;

    count = vty->incount;
    if (vty->isavedchar != -1)
	count++;
    return(count);
}

/*
 * v120_check_vty_txq
 * This routine is called by the vty output routines and is also
 * called when we come out of a remote busy state on the protocol code
 * or when we have acknowledgements of packets.  It will see if
 * we have any more packets on the txq of the vty to send.
 */
void v120_check_vty_txq (v120_info_type *v120)
{
    tt_soc *vty;
    paktype *pak;

    vty = v120->v120_vty;

    if (vty == NULL)
	return;
    /*
     * flow control out ?
     */
    if (vty->statbits & WANTXON) return;
    /*
     * If we have packets on the txpakq, try to send them.  If they
     * don't send, put them back on the head of the queue.  If we
     * do send the packet, decrement the outcount
     *
     * An optional way of coding here would be to call the p_dequeue
     * instead of checking the qhead.  This would slow it down, but
     * then we wouldn't be looking inside the txpakq structure
     */
    while (vty->txpakq.qhead != NULL) {
	ushort outchars;
	pak = p_dequeue(&vty->txpakq);
	outchars = pak->datagramsize - pak->encsize;
	if (v120_send_data(pak, v120) == FALSE) {
	    p_requeue(&vty->txpakq, pak);
	    return;
	} else {
	    vty->outcount -= outchars;
	    vty->totalout += outchars;
	}
    }
}

/*
 * v120_send_data_packet
 * This will actually send the data packet, either when we have a full
 * packet, or the vstartoutput tells us to.  It will put the header
 * on, first.  If there are already elements on the queue, it
 * will put the packet on the txpakq.  Note that the lapd headers
 * are not added until the v120 code actually sends the packet.
 */
static void v120_send_data_packet (tt_soc *vty,
				   v120_info_type *v120,
				   uchar header, uchar header_ext)
{
    ushort size;
    paktype *pak;

    pak = vty->outpak;
    vty->outpak = NULL;
    /*
     * add in the V120 header bytes
     */
    pak->encsize = v120->header_bytes;
    pak->datagramstart = pak->network_start - v120->header_bytes;
    size = pak->datagramsize;
    pak->datagramsize += v120->header_bytes;
    header |= V120_HEAD_BF;
    if (v120->header_extension) {
	if (v120->header_bytes == 1)
	    header |= V120_HEAD_EXT;
	*pak->datagramstart = header;
	if (v120->header_bytes == 2)
	    pak->datagramstart[1] = V120_HEAD_EXT_ALL;
    }
    /*
     * we are encapsulated
     * Put the element on the queue if we have already items on
     * the queue, or send the packet.  If we send the packet,
     * decrement the outcount.
     */
    if ((vty->txpakq.qhead) ||
	(vty->statbits & (WANTXON | HOLDING)) ||
	(v120_send_data(pak, v120) == FALSE)) {
	p_enqueue(&vty->txpakq, pak);
    } else {
	vty->outcount -= size;
	vty->totalout += size;
    }
}

/*
 * v120_clearoutput
 * Get rid of everything that we are sending
 */
static void v120_vclearoutput (tt_soc *vty)
{
    paktype *pak;

    if (vty->outpak) {
	retbuffer(vty->outpak);
	vty->outpak = NULL;
    }
    while ((pak = p_dequeue(&vty->txpakq)) != NULL) {
	retbuffer(pak);
    }
    vty->outcount = 0;
}

/*
 * v120_get_packet
 * get a packet to store transmit characters
 */
static paktype *v120_get_packet (void)
{
    paktype *pak;

    pak = getbuffer(V120_MAX_IFRAME_SIZE + V120_ENCAPBYTES);
    if (pak) {
	pak->dataptr = pak->network_start;
	pak->datagramsize = 0;
    }
    return(pak);
}
/*
 * v120_vputc
 * Put a data byte.  We will put the output data in a pak structure
 */
static void v120_vputc (tt_soc *vty, int data)
{
    paktype *pak;
    v120_info_type *v120;

    v120 = TTY_TO_V120(vty);
    if (v120 == NULL) {
	v120_vclearoutput(vty);
	return;
    }
    
    if (vty->statbits & CARDROP) {
	v120_vclearoutput(vty);
	return;
    }

    if (vty->outpak == NULL) {
	vty->outpak = v120_get_packet();
    } 

    pak = vty->outpak;

    if (pak == NULL) {
	if (v120_debug) {
	    buginf("\nVTY %d: couldn't get output packet", vty->ttynum);
	}
	return;
    }


    if (data == BREAK_CHAR) {
	v120_send_data_packet(vty, v120, V120_HEAD_BREAK, 0);
	return;
    }

    /*
     * Put the data in the pak - increment dataptr, size and outcount
     */
    *pak->dataptr++ = data;
    pak->datagramsize++;
    vty->outcount++;
    tty_dallytimer(vty);
    /*
     * If we hit the max size for a v120 packet, transmit it.
     * Note: should be just MAX_IFRAME - header bytes, but the
     * bitsurfer messes up, and includes the encapbyte in its N201
     * So, we will only send N201 - 5 bytes.
     */
    if (pak->datagramsize >=
	(V120_MAX_IFRAME_SIZE - V120_ENCAPBYTES - v120->header_bytes)) {
	v120_send_data_packet(vty,v120, 0, 0);
    }
}

/*
 * v120_vstartoutput
 * See if we can output any packets.  If we have an outstanding
 * packet, send it.
 */
static boolean v120_vstartoutput (tt_soc *vty)
{
    v120_info_type *v120;

    v120 = TTY_TO_V120(vty);
    if (v120 == NULL) {
	v120_vclearoutput(vty);
	return(TRUE);
    }
	
    v120_check_vty_txq(v120);
    
    if (vty->outpak && vty->outcount) {
	v120_send_data_packet(vty, v120, 0, 0);
    } 
    return(TRUE);
}

/*
 * v120_voutputpending
 * return the number of output characters we have queued up
 */
static int v120_voutputpending (tt_soc *vty)
{
    return(vty->outcount);
}

#define V120_MAX_TX_PAKQ 24

/*
 * v120_voutputblock
 * see if output would block - If we have an outpak, we can
 * store at least 1 data byte.  If we don't, get an outpak.  If
 * that is NULL, return TRUE.
 */
static boolean v120_voutputblock (tt_soc *vty)
{
    if (vty->outpak != NULL)
	return(FALSE);
    if (queryqueuedepth(&vty->txpakq) > V120_MAX_TX_PAKQ)
	return(FALSE);
    vty->outpak = v120_get_packet();
    return(vty->outpak == NULL);
}

/*
 * v120_voutputxoffed
 * If the v120 is in RNR, return TRUE.  If we implement header extension
 * with modem signals, we might want to handle that also
 */
static boolean v120_voutputxoffed (tt_soc *vty)
{
    v120_info_type *v120;

    v120 = TTY_TO_V120(vty);
    if (v120 == NULL)
	return(FALSE);
    
    return(v120->remote_busy |
	   ((vty->statbits & (WANTXON | HOLDING)) ? TRUE : FALSE));
}

/*
 * v120_vputpkt
 * Just vputc all the characters we see.  start the output when we
 * are done.
 *
 * Note: could do some pretty big optimization here - shouldn't
 * have to call vputc for each character.  Might be able to write
 * an inline that does the guts for each.
 */
static int v120_vputpkt (tt_soc *vty, paktype *pak, uchar *buf, int len)
{
    int count;

    count = 0;
    while (len--) {
	v120_vputc(vty, vty->tty_datamask & (*buf++));
	count++;
    }
    v120_vstartoutput(vty);
    return(count);
}

/*
 * v120_vsendbreak
 * send the break character.  This might not be used.
 */
static void v120_vsendbreak (tt_soc *vty)
{
    v120_vputc(vty, BREAK_CHAR);
}


static void v120_vdtrstate (tt_soc *vty, boolean flag)
{
}

static modem_status v120_vmodemsignal (tt_soc *vty)
{
    modem_status m;

    m.cts = 1;
    m.ring = 1;
    m.dtr = 1;
    m.rts = 1;
    return(m);
}
static modem_status v120_vmodemchange (tt_soc *vty)
{
    modem_status m;

    m.cts = 0;
    m.ring = 0;
    m.dtr = 0;
    m.rts = 0;
    return(m);
}

static void v120_dummy (tt_soc *vty)
{
}

/*************************************************************************
 *  Routines for the connection vector
 *************************************************************************/
/*
 * v120_alive
 */
static boolean v120_alive (conntype *conn)
{
    tt_soc *vty;
    v120_info_type *v120;

    if (conn == NULL)
	return(FALSE);
    
    vty = conn->tty;
    if (vty == NULL)
	return(FALSE);
    
    v120 = TTY_TO_V120(vty);
    if (v120 == NULL)
	return(FALSE);
    return(TRUE);
}

/*
 * v120_vclose
 * return the connection structure
 */
static void v120_vclose (conntype *conn)
{
    if (conn == NULL)
	return;
    
    freeconnection(conn);
}


/*
 * v120_vunread
 * return the number of bytes outstanding - just get the vty, and
 * look there
 */
static int v120_vunread (conntype *conn)
{
    tt_soc *vty;

    if (!conn)
	return(0);
    vty = conn->tty;
    if (!vty)
	return(0);

    return(inputpending(vty));
}

/*
 * v120_vopen
 * Cannot open a v120 connection
 */
static conntype *v120_vopen (parseinfo *csb, boolean *abortflag)
{
    return(NULL);
}


/****************************************************************************
 * V120 VTY Initialization and Take down
 ****************************************************************************/
/*
 * The below are translations from the data in the LLC to how it is
 * used for the vty
 */
static const int v120_to_stopbits[] = { 0, STOPBITS1, STOPBITS15, STOPBITS2 };
static const int v120_to_databits[] = { 0, 5, 7, 8};
static const int v120_to_parity[] = {
    PARITY_ODD,
    PARITY_NO,
    PARITY_EVEN,
    PARITY_NO,
    PARITY_SPACE,
    PARITY_MARK
};
static const int v120_to_speed[] =
{
    0, 600, 1200, 2400, 3600, 4800, 7200, 8000, 9600, 14400, 16000,
    19200, 32000, 48000, 0, 56000, 64000
};

/*
 * v120_stop_vty
 * Disconnect the vty.  Clear the input and the output, then terminate
 * the line and clear out the vty variables.  This is called when
 * isdn disconnects the line.
 */
void v120_stop_vty (v120_info_type *v120)
{
    tt_soc *vty;

    vty = v120->v120_vty;
    if (vty == NULL)
	return;

    v120_vclearinput(vty);
    v120_vclearoutput(vty);
    vty->v120_info = NULL;
    vty->cfgidb = NULL;
    v120->I_called_death = TRUE;
    terminate_line(vty);
    v120->v120_vty = NULL;
}

/*
 * v120_cfg_idb
 *
 * Return the ISDN configuration idb
 */
static idbtype *v120_cfg_idb (tt_soc *vty)
{
    if (vty->v120_info && vty->v120_info->hwptr) {
	return (vty->v120_info->hwptr->firstsw);
    }
    return (NULL);
}

/*
 * v120_init_vty
 * Bring up the vty when we get a v120 connection.
 */
boolean v120_init_vty (v120_info_type *v120, v120_parameters_type *params)
{
    int line;
    tt_soc *vty;
    conntype *conn;
   
    line = get_vtyline(0, &V120connvector); 
    if (line == 0) {
	if (v120_debug)
	    buginf("\nV120:No vty's found - disconnecting");
	return(FALSE);
    }
    vty = v120->v120_vty = tty_allocate(line, TRUE);
    if (vty == NULL) {
	if (v120_debug)
	    buginf("\nV120:Couldn't allocate tty - disconnecting");
	return(FALSE);
    }

    /*
     * save the defaults, save the v120 pointer in the ttynum_arr,
     * then init all the variables that we use to pass data
     */
    tty_savedefaults(vty);
    vty->v120_info = v120;
    queue_init(&vty->txpakq, 0);
    queue_init(&vty->rxpakq, 0);
    vty->inheadpak = NULL;
    vty->incount = 0;
    vty->outpak = NULL;
    vty->outcount = 0;
    vty->statbits &= ~(CARDROP + HANGUP+ IDLE);
    vty->ivector = &V120vector;
    vty->cfgidb = v120_cfg_idb;
    /*
     * We might want to do this after the v120 goes into established state
     */
    vty->statbits |= READY;
    tty_create_vty_exec(vty);
    if (params != NULL) {
	vty->tty_stopbits = v120_to_stopbits[params->stopbits];
	vty->tty_databits = v120_to_databits[params->databits];
	vty->tty_parity = v120_to_parity[params->parity];
	if (params->speed <= 0x10) 
	    vty->tty_rxspeed = vty->tty_txspeed = v120_to_speed[params->speed];
    }
    /*
     * Get the connection vector that we need for ppp/slip mode
     * When you get the connection, it puts it on the connection
     * queue, rather than checking if it needs to be at conntop
     * Put it at vty->conntop, and copy the name of the interface
     * to the connection name
     */
    conn = getconnection(vty, &V120connvector);
    if (conn == NULL) {
	/* No connection - disconnect the vty, and exit */
	v120_stop_vty(v120);
	return(FALSE);
    }
    unqueue(&vty->connhead, conn);
    vty->conntop = conn;
    vty->conncount++;
    vty->statbits |= CONNECTED;
    sstrncpy(conn->hostname, v120->hwptr->hw_namestring, CONNLEN+1);
    return(TRUE);
}


/*
 * v120_death
 * called when the death registry is called.  If we still have a
 * v.120, and we didn't call death because of a disconnect, disconnect
 * the dialer
 */
static void v120_death (tt_soc *vty)
{
    v120_info_type *v120;

    v120 = TTY_TO_V120(vty);

    if (v120 == NULL || v120->I_called_death)
	return;
    
    dialer_disconnect(v120->hwptr);
}

/*
 * v120_get_channelidb
 * return the hwidb controlling the input side of this tty, if it's a
 * V.120 user
 */
static boolean
v120_get_channelidb (tt_soc * tty, hwidbtype **hwidb)
{
    if (tty->type != VTY_LINE)
	return(FALSE);
    if (tty->v120_info) {
	*hwidb = tty->v120_info->hwptr;
	return(TRUE);
    }
    return(FALSE);
}

/*
 * v120_init_vty_vector
 * Initialize the vectortype structure for V120
 */
void v120_init_vty_vector (void)
{
    vectortype *vector;

    reg_add_death(v120_death, "v120_death");
    reg_add_channel_hwidb(v120_get_channelidb, "v120_get_channelidb");
    
    vector = &V120vector;

    vector_init(vector);
    vector->vgetc		= v120_vgetc;
    vector->vclearinput		= v120_vclearinput;
    vector->vinputpending	= v120_vinputpending;
    vector->vputc		= v120_vputc;
    vector->vstartoutput	= v120_vstartoutput;
    vector->vclearoutput	= v120_vclearoutput;
    vector->voutputpending	= v120_voutputpending;
    vector->voutputblock	= v120_voutputblock;
    vector->voutputxoffed	= v120_voutputxoffed;
    vector->vputpkt		= v120_vputpkt;
    vector->vsendbreak		= v120_vsendbreak;
    vector->vdtrstate		= v120_vdtrstate;
    vector->vsetsflow		= v120_dummy;
    vector->vsetflow		= v120_dummy;
    vector->vservicehook	= generic_servicehook;
    vector->vnoservicehook	= generic_noservicehook;
    vector->vportenable		= v120_dummy;
    vector->vportdisable	= v120_dummy;
    vector->vmodemsignal	= v120_vmodemsignal;
    vector->vmodemchange	= v120_vmodemchange;

    /*
     * Initialize connection vectors.  Might want a NULL name
     * here - they appear in the show line displays
     */
    conn_vector_init(&V120connvector, "v120", "Async over ISDN");
    V120connvector.valive = v120_alive;
    V120connvector.vclose = v120_vclose;
    V120connvector.vunread = v120_vunread;
    V120connvector.vopen = v120_vopen;
    /*
     * Don't need the vsettty, vresume or vaccounting vectors
     */
}

/* end file v120_vty.c */
