/* $Id: async_ppp.c,v 3.2.62.7 1996/09/07 02:08:24 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/async_ppp.c,v $
 *------------------------------------------------------------------
 * async_ppp.c - PPP framing support for asynchronous lines
 *
 * September 1990, Kevin Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: async_ppp.c,v $
 * Revision 3.2.62.7  1996/09/07  02:08:24  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.2.62.6  1996/08/07  09:03:05  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.62.5  1996/06/28  23:30:54  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.2.62.4  1996/06/18  01:49:27  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.62.3  1996/05/09  14:47:34  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.62.2.14.1  1996/04/27  07:37:36  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.62.2.4.1  1996/04/08  02:10:25  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.62.2  1996/03/27  18:26:27  sjackson
 * CSCdi41921:  sh async statistics is inaccurate
 * Branch: California_branch
 *
 * Revision 3.2.62.1  1996/03/18  22:19:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.66.1  1996/03/22  09:44:25  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.1  1996/03/05  05:37:00  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.40.1  1996/02/27  21:19:53  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.14.1  1996/02/15  19:21:19  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.2  1995/11/17  18:52:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:08:38  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#define APPP_INTERNAL
#include "master.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "../ts/if_async.h"
#include "../ts/async_ppp.h"
#include "../if/network.h"
#include "../ip/ip.h"
#include "../tcp/tcpcompress.h"
#include "../wan/ppp.h"

#ifdef PACKED_ESCAPE_MAP

static unsigned char BitMasks[8] = 
    { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

static unsigned char ppp_lcp_escapemap[256/8] = {
    0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, /* 00 - 3f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, /* 40 - 7f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 80 - bf */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* c0 - ff */
  };

#define BYTENUMBER(i) ((i)>>3)
#define BITNUMBER(i) ((i)&0x7)
#define ESCAPEMAP(map) ((map)->EscapeCharBitmap)
#define TURNON(map,i) (map)[BYTENUMBER(i)] \
  |= BitMasks[BITNUMBER(i)])
#define TURNOFF(map,i) (map)[BYTENUMBER(i)] \
			&= ~BitMasks[BITNUMBER(i)];
#define ISON(map,i) ((map)[BYTENUMBER(i)] \
		     & BitMasks[BITNUMBER(i)])

#else PACKED_ESCAPE_MAP

static const unsigned char ppp_lcp_escapemap[256] = {
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 00 - 07 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 08 - 0f */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 10 - 17 */
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, /* 18 - 1f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 20 - 27 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 28 - 2f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 30 - 37 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 38 - 3f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 40 - 47 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 48 - 4f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 50 - 57 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 58 - 5f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 60 - 67 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 68 - 6f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 70 - 77 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, /* 78 - 7f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 80 - 87 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 88 - 8f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 90 - 97 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* 98 - 9f */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a0 - a7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* a8 - af */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* b0 - b7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* b8 - bf */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* c0 - c7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* c8 - cf */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* d0 - d7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* d8 - df */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e0 - e7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* e8 - ef */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, /* f0 - f7 */
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00  /* f8 - ff */
};

#define ESCAPEMAP(map) ((map)->EscapeCharMap)
#define TURNON(map,i) (map)[(i)] = 1
#define TURNOFF(map,i) (map)[(i)] = 0
#define ISON(map,i) ((map)[(i)])

#endif PACKED_ESCAPE_MAP

/*
 * FCS lookup table as calculated by genfcstab.
 */
const unsigned short ppp_fcstab[256] = {
        0x0000, 0x1189, 0x2312, 0x329b, 0x4624, 0x57ad, 0x6536, 0x74bf, 
        0x8c48, 0x9dc1, 0xaf5a, 0xbed3, 0xca6c, 0xdbe5, 0xe97e, 0xf8f7, 
        0x1081, 0x0108, 0x3393, 0x221a, 0x56a5, 0x472c, 0x75b7, 0x643e, 
        0x9cc9, 0x8d40, 0xbfdb, 0xae52, 0xdaed, 0xcb64, 0xf9ff, 0xe876, 
        0x2102, 0x308b, 0x0210, 0x1399, 0x6726, 0x76af, 0x4434, 0x55bd, 
        0xad4a, 0xbcc3, 0x8e58, 0x9fd1, 0xeb6e, 0xfae7, 0xc87c, 0xd9f5, 
        0x3183, 0x200a, 0x1291, 0x0318, 0x77a7, 0x662e, 0x54b5, 0x453c, 
        0xbdcb, 0xac42, 0x9ed9, 0x8f50, 0xfbef, 0xea66, 0xd8fd, 0xc974, 
        0x4204, 0x538d, 0x6116, 0x709f, 0x0420, 0x15a9, 0x2732, 0x36bb, 
        0xce4c, 0xdfc5, 0xed5e, 0xfcd7, 0x8868, 0x99e1, 0xab7a, 0xbaf3, 
        0x5285, 0x430c, 0x7197, 0x601e, 0x14a1, 0x0528, 0x37b3, 0x263a, 
        0xdecd, 0xcf44, 0xfddf, 0xec56, 0x98e9, 0x8960, 0xbbfb, 0xaa72, 
        0x6306, 0x728f, 0x4014, 0x519d, 0x2522, 0x34ab, 0x0630, 0x17b9, 
        0xef4e, 0xfec7, 0xcc5c, 0xddd5, 0xa96a, 0xb8e3, 0x8a78, 0x9bf1, 
        0x7387, 0x620e, 0x5095, 0x411c, 0x35a3, 0x242a, 0x16b1, 0x0738, 
        0xffcf, 0xee46, 0xdcdd, 0xcd54, 0xb9eb, 0xa862, 0x9af9, 0x8b70, 
        0x8408, 0x9581, 0xa71a, 0xb693, 0xc22c, 0xd3a5, 0xe13e, 0xf0b7, 
        0x0840, 0x19c9, 0x2b52, 0x3adb, 0x4e64, 0x5fed, 0x6d76, 0x7cff, 
        0x9489, 0x8500, 0xb79b, 0xa612, 0xd2ad, 0xc324, 0xf1bf, 0xe036, 
        0x18c1, 0x0948, 0x3bd3, 0x2a5a, 0x5ee5, 0x4f6c, 0x7df7, 0x6c7e, 
        0xa50a, 0xb483, 0x8618, 0x9791, 0xe32e, 0xf2a7, 0xc03c, 0xd1b5, 
        0x2942, 0x38cb, 0x0a50, 0x1bd9, 0x6f66, 0x7eef, 0x4c74, 0x5dfd, 
        0xb58b, 0xa402, 0x9699, 0x8710, 0xf3af, 0xe226, 0xd0bd, 0xc134, 
        0x39c3, 0x284a, 0x1ad1, 0x0b58, 0x7fe7, 0x6e6e, 0x5cf5, 0x4d7c, 
        0xc60c, 0xd785, 0xe51e, 0xf497, 0x8028, 0x91a1, 0xa33a, 0xb2b3, 
        0x4a44, 0x5bcd, 0x6956, 0x78df, 0x0c60, 0x1de9, 0x2f72, 0x3efb, 
        0xd68d, 0xc704, 0xf59f, 0xe416, 0x90a9, 0x8120, 0xb3bb, 0xa232, 
        0x5ac5, 0x4b4c, 0x79d7, 0x685e, 0x1ce1, 0x0d68, 0x3ff3, 0x2e7a, 
        0xe70e, 0xf687, 0xc41c, 0xd595, 0xa12a, 0xb0a3, 0x8238, 0x93b1, 
        0x6b46, 0x7acf, 0x4854, 0x59dd, 0x2d62, 0x3ceb, 0x0e70, 0x1ff9, 
        0xf78f, 0xe606, 0xd49d, 0xc514, 0xb1ab, 0xa022, 0x92b9, 0x8330, 
        0x7bc7, 0x6a4e, 0x58d5, 0x495c, 0x3de3, 0x2c6a, 0x1ef1, 0x0f78
};

/*
 * appp_add_quoted_char - Add a character from the quoted set
 *
 * If it is reasonable to add the given char to the set of escaped
 * chars: do so, and yield true; otherwise, simply return false.
 */

boolean appp_add_quoted_char (tt_soc *tty, uchar ch)
{
    pppContext *ctx = tty->appp_context;

    if (!ctx)
	return(FALSE);		/* No async context */
    if (ISON(ESCAPEMAP(ctx),ch))
	return(TRUE);			/* Already quoted - this is a NOP */
    else if (ISON(ESCAPEMAP(ctx),PPP_ESCAPE_MECHANISM(ch)))
	return(FALSE);			/* Quoted character already quoted */
    else {
	TURNON(ESCAPEMAP(ctx),ch);			/* Turn on quoting */
	return(TRUE);			/* And we're done */
    }
}

/*
 * appp_remove_quoted_char - Remove a character from the quoted set
 */

void appp_remove_quoted_char (tt_soc *tty, uchar ch)
{
    if (!tty->appp_context)
	return;			/* No async context */

    TURNOFF(ESCAPEMAP(tty->appp_context), ch);	/* Clear the bit */
}

boolean appp_tty_startup (
     tt_soc *tty,		/* Terminal pointer */
     void (*iqueue)(paktype *, tt_soc *),	/* Input queue handler */
     int ioffset,		/* Receive offset into packet buffer */
     int imax,			/* Receive maximum */
     hwidbtype *idb)		/* IDB pointer, if any */
{
    pppContext *ctx;
    leveltype status;

    if (tty->appp_context)
	appp_tty_shutdown(tty);	/* Shut down if started up once */

    ctx = malloc_named(sizeof(pppContext), "ASYPPP context");
    if (!ctx)
      return(FALSE);		/* Can't get a context block */

    clearoutput(tty);

    ctx->idb = idb;		/* Save IDB pointer */

    ctx->outchar = -1;		/* No output character yet */
    ctx->iqueue = iqueue;
    ctx->ioffset = ioffset;
    ctx->imax = imax;

    TURNON(ESCAPEMAP(ctx),PPP_FRAME_END); /* avoid frame control chars */
    TURNON(ESCAPEMAP(ctx),PPP_FRAME_ESC);

    ctx->outstate = APPP_FRAME_END; /* Initially, send an end of frame */

    tty->appp_context = ctx;	/* Set the context pointer */
    if (!tty_servicehook(tty, appp_txservice, appp_rxservice)) {
	tty->appp_context = NULL; /* Dump context pointer */
	free(ctx);		/* Free context */
	if (async_framing_debug || async_state_debug)
	    buginf("\n%s: Unable to set PPP service hook on TTY%t",
		   idb->hw_namestring, tty->ttynum);
	return(FALSE);		/* No such luck */
    }
    tty_pppmode(tty, TRUE);	/* Tell driver about PPP mode */

    status = raise_interrupt_level(TTY_DISABLE); /* Lock out tty interrupts */
    appp_new_inpak(ctx);	/* Get an initial input buffer */
    reset_interrupt_level(status);

    startoutput(tty);		/* Start up the output process */
    return(TRUE);
}

void appp_tty_shutdown (tt_soc *tty)
{
    pppContext *ctx = tty->appp_context;
    paktype *pak;

    if (!ctx)
	return;

    tty_pppmode(tty, FALSE);	/* Tell driver PPP done */
    tty_noservicehook(tty);	/* Disable the service hook */

    tty->appp_context = NULL;

    /*
     * If fast switching is on, obsolete the route cache
     */
    if ((ctx->idb) && (ctx->idb->ip_fastokay)) 
	reg_invoke_bump_cache_version(NULL, NULL, TRUE);

    while ((pak = dequeue(&ctx->oqueue))) {
	datagram_done(pak);
    }

    pak = ctx->outpak;
    if (pak)
	datagram_done(pak);

    pak = ctx->inpak;
    if (pak)
	datagram_done(pak);

    free(ctx);
}

/*
 * appp_oqueue - Queue an output packet for a line
 *
 * This routine is used to queue an output packet. It handles waking up
 * idle lines as necessary.
 */

void appp_oqueue (tt_soc *tty, paktype *pak)
{
    leveltype status;
    pppContext *ctx = tty->appp_context;
    boolean start = FALSE;

    status = raise_interrupt_level(TTY_DISABLE);
    enqueue(&ctx->oqueue, pak);
    if (ctx->outstate == APPP_IDLE) {
	ctx->outstate = APPP_FRAME_END;
	start = TRUE;
    }
    reset_interrupt_level(status);

    if (start)
      startoutput(tty);
}

/*
 * appp_startoutput - Start output routine when using IDB interface
 */

void appp_startoutput (tt_soc *tty)
{
    leveltype status;
    boolean start = FALSE;
    pppContext *ctx = tty->appp_context;

    status = raise_interrupt_level(TTY_DISABLE);
    if (ctx->outstate == APPP_IDLE) {
	ctx->outstate = APPP_FRAME_END;
	start = TRUE;
    }
    reset_interrupt_level(status);

    if (start)
      startoutput(tty);
}

void appp_rcv_error (tt_soc *tty)
{
    pppContext *ctx = tty->appp_context; /* Get context pointer */
    hwidbtype *idb = ctx->idb; 	/* And IDB pointer */
    paktype *pak = ctx->inpak;	/* And packet pointer */

    ctx->rcv_errors++;			/* Count an error */
    if (pak) {
	pak->datagramsize = 0;		/* Reset the length */
	pak->dataptr = pak->datagramstart; /* Reset fill pointer */
	pak->unspecified[0] = PPPINITFCS;	/* And CRC accumulation */
    }
    ctx->inescape = FALSE;		/* No receive escape */
    MAYBE_DROPPED(idb);
}

boolean appp_new_inpak (pppContext *ctx)
{
    paktype *pak;

    if (ctx->inpak)
      return(TRUE);			/* Nothing to do if we a buffer */

    if (ctx->idb)
      pak = input_getbuffer((ctx->imax - ctx->ioffset),
			    ctx->idb);
    else
      pak = getbuffer(ctx->imax - ctx->ioffset);
					/* Get a new input buffer */
    if (pak) {
	ctx->inpak = pak;		/* Save the input buffer */
	pak->datagramsize = 0;		/* Reset the length */
	pak->dataptr = pak->network_start - ctx->ioffset; /* Set input pointer */
	pak->datagramstart = pak->dataptr; /* Save it for later */
	pak->unspecified[0] = PPPINITFCS; /* And CRC accumulation */
	pak->enctype = ET_PPP;		/* Set encapsulation */
	return(TRUE);
    }
    if (ctx->idb) {
      slip_traffic.nobuf++;
      ctx->idb->counters.input_error++;
    }
    return(FALSE);
}
/*
 * Process a single input character for a terminal line running async HDLC
 *
 * This routine is called by the terminal port driver for each received
 * character.
 */
void
appp_rxservice(tt_soc *tty, uchar *p, int len)
{
    paktype *pak;
    pppContext *ctx = tty->appp_context;
    ulong firstlong;
    uchar ch;

    pak = ctx->inpak;
    while (len-- > 0) {
	ch = *p++;
	if (!pak) {
	    if (!appp_new_inpak(ctx)) {
		if (ctx->idb) {
		    ctx->idb->counters.output_nobuffers++; /* Count as no buffers */
		    MAYBE_DROPPED(ctx->idb);
		}
		return;
	    }
	    pak = ctx->inpak;
	}

	if (ctx->inescape) {	/* Should we escape this character */
	    ctx->inescape = FALSE;	/* Yes, get out of escape state */
	    if (ch == PPP_FRAME_END) {
		if (async_framing_debug)
		  buginf("\nAPPP%t: Escaped FRAME_END received", tty->ttynum);
		if (ctx->idb) {
		    ctx->idb->counters.input_abort++; /* Count as an abort */
		    ctx->idb->counters.input_error++;
		    slip_traffic.fmterrors++; /* Count this as a format error */
		}
		appp_rcv_error(tty);
		continue;
	    }
	    ch = PPP_ESCAPE_MECHANISM(ch);	/* Unescape the character */
	} else {
	    switch (ch) {
	      case PPP_FRAME_END:

		if (pak->datagramsize == 0)
		  continue;		/* Ignore multiple end characters */

		if ((pak->unspecified[0] & 0xffff) != PPPGOODFCS) {
		    if (async_framing_debug)
		      buginf("\nAPPP%t: CRC error", tty->ttynum);
		    if (ctx->idb) {
			ctx->idb->counters.input_crc++; /* Count as a CRC error */
			ctx->idb->counters.input_error++;
			slip_traffic.crcerr++;	/* Count in SLIP counters */
		    }
		    appp_rcv_error(tty);
		    continue;
		}

		if (pak->datagramsize < PPP_FCS_SIZE) {
		    if (async_framing_debug)
		      buginf("\nAPPP%t: Packet too small", tty->ttynum);
		    if (ctx->idb) {
			ctx->idb->counters.input_frame++; /* Count as framing error */
			ctx->idb->counters.input_error++;
			slip_traffic.badlen++;	/* Count as length error */
		    }
		    appp_rcv_error(tty);
		    continue;
		}

		pak->datagramsize -= PPP_FCS_SIZE;

		/*
		 * If this packet is less that four bytes long, it will
		 * not be properly aligned. Check for this here, and
		 * handle realignment.
		 */

		if ((pak->datagramsize < 4) && ctx->idb) {
		    firstlong = GETLONG(pak->datagramstart);
		    ppp_parse_packet(ctx->idb, pak, pak->datagramstart);
		    if (ctx->idb->encsize > pak->datagramsize) { /* Too small? */
			ctx->idb->counters.runts++; /* Count this as a runt */
			appp_rcv_error(tty);
			continue;
		    }
		    PUTLONG(pak->datagramstart, firstlong);
				    /* And the data */
		}

		ctx->inpak = NULL;	/* Dump input pointer */

		if (async_packet_debug)
		  buginf("\nAPPP%t: Input packet, len = %d, header = %x %x %x %x",
			 tty->ttynum,
			 pak->datagramsize,
			 pak->datagramstart[0],
			 pak->datagramstart[1],
			 pak->datagramstart[2],
			 pak->datagramstart[3]);

		(*ctx->iqueue)(pak, tty); /* Handle the input buffer */
		ctx->inpaks++;
		appp_new_inpak(ctx);
		pak = ctx->inpak;
		if (pak == NULL) {
		    return;
		}
		continue;

	      case PPP_FRAME_ESC:
		ctx->inescape = TRUE;
		continue;
	    }
	}

	/*
	 * By now, we have an input character in ch to stuff in the buffer
	 */

	if (pak->datagramsize >= PPP_FCS_SIZE + ctx->imax) {
	    if (async_framing_debug)
	      buginf("\nAPPP%t: Input packet too large", tty->ttynum);
	    if (ctx->idb) {
		ctx->idb->counters.input_overrun++; /* Count as an overrun */
		ctx->idb->counters.input_error++;
		slip_traffic.badlen++;
	    }
	    appp_rcv_error(tty);
	    continue;
	}

	pak->datagramsize++;	/* Count another byte */
	*pak->dataptr++ = ch;	/* Store the character */
	FCS_ACCUMULATE(pak->unspecified[0], ch);	/* Accumulate CRC */

	/*
	 * If this is the fourth byte received, and we are running as an
	 * async interface, then we must check for proper packet alignment.
	 */

	if ((pak->datagramsize == 4) && ctx->idb) {
	    firstlong = GETLONG(pak->datagramstart);
					    /* Get first four bytes */
	    ppp_parse_packet(ctx->idb, pak, pak->datagramstart);
	    pak->dataptr = pak->datagramstart + 4; /* And next byte pointer */
	    PUTLONG(pak->datagramstart, firstlong);
					    /* Move the first four bytes */
	}
    }
}

/*
 * Send a input characters for a terminal line running async HDLC
 */
uint
appp_txservice(tt_soc *tty, uchar *p, int max)
{
    int ch, len = 0;
    pppContext *ctx = tty->appp_context;
    paktype *pak = ctx->outpak;
    hwidbtype *idb;

    while (len < max) {
	ch = ctx->outchar;		/* Fetch any saved character */
	if (ch >= 0) {		/* Saved output character? */	
	    ctx->outchar = -1;	/* Yes, clear it for next time */
	} else {
	    switch (ctx->outstate) {
	    case APPP_DATA:	/* Output a byte of data */

		/*
		 * There should always be a packet here!
		 */
		if (!pak) {
		    buginf("\nAPPP%t: APPP_DATA state with no pak",
			   tty->ttynum);
		    ctx->outstate = APPP_IDLE;
		    return(len);
		}
		 
		ch = *ctx->outptr++;	/* Fetch the character */
		FCS_ACCUMULATE(ctx->outfcs, ch); /* Accumulate the FCS */

		if (--ctx->outlen == 0) {	/* End of the packet? */
		    ctx->outstate = APPP_FCS_LSB; /* Yes, change state */
		    ctx->outpak = NULL; /* Dump input packet pointer */
		    datagram_done(pak);	/* And flush the packet */
		    pak = NULL;
		}
		break;
		
	    case APPP_FCS_LSB:		/* Handle LSB of FCS */
		ch = (~ctx->outfcs) & 0xFF; /* Pick up the LSB */
		ctx->outstate = APPP_FCS_MSB;
		break;
		
	    case APPP_FCS_MSB:		/* Handle MSB of FCS */
		ch = ((~ctx->outfcs) >> 8) & 0xFF; /* Pick up the MSB */
		ctx->outstate = APPP_FRAME_END; /* Send frame mark next */
		break;

	    case APPP_FRAME_END:		/* Handle end of frame */
		/*
		 * Get next packet to send
		 */
		pak = appp_get_oqueue(ctx);

		/*
		 * Set escape map to either global LCP one or the
		 * negotiated value.
		 */
		if (pak && (pak->flags & PAK_CONTROL)) {
		    ctx->ActiveEscapeMap = ppp_lcp_escapemap;
		} else {
		    ctx->ActiveEscapeMap = ESCAPEMAP(ctx);
		}

		/*
		 * If have a packet, set it up as the next buffer to
		 * be sent via txservice.
		 */
		if (pak) {
		    ctx->outpak = pak;	/* Make this the input buffer */
		    ctx->outptr = pak->datagramstart; /* Set pointer */
		    ctx->outlen = pak->datagramsize; /* And length */
		    ctx->outfcs = PPPINITFCS; /* Set initial FCS */
		    ctx->outstate = APPP_DATA; /* Change the state */
		    if (async_packet_debug)
		      buginf("\nAPPP%t: Output packet, len = %d, "
			     "header = %x %x %x %x",
			     tty->ttynum,
			     pak->datagramsize,
			     pak->datagramstart[0],
			     pak->datagramstart[1],
			     pak->datagramstart[2],
			     pak->datagramstart[3]);
		} else {
		    ctx->outstate = APPP_IDLE; /* Mark line as idle */
		}
		*p++ = PPP_FRAME_END;
		len += 1;
		continue;

	    case APPP_IDLE:		/* Idle state, nothing to do */
		idb = ctx->idb;
		if ((idb) && (idb->output_qcount != 0)){
		    ctx->outstate = APPP_FRAME_END;
		    *p++ = PPP_FRAME_END;
		    len += 1;
		    continue;
		}
		return(len);
	    }

	    /*
	     * At this point, we have a character to be output, with
	     * possible quoting. If the character is to be quoted, we
	     * save the escaped character for next time, and send the
	     * escape now.
	     */
	    if (ISON(ctx->ActiveEscapeMap, ch)) {
		ctx->outchar = PPP_ESCAPE_MECHANISM(ch);
		ch = PPP_FRAME_ESC;
	    }
	}

	/*
	 * Add character to the output buffer
	 */
	*p++ = ch;
	len += 1;
    }
    return(len);
}
