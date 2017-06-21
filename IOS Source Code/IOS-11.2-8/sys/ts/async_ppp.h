/* $Id: async_ppp.h,v 3.2.60.4 1996/09/07 02:08:26 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ts/async_ppp.h,v $
 *------------------------------------------------------------------
 * async_ppp.h - Definitions for asynchronous point-to-point framing
 *
 * September 1990, Kevin Herbert
 *
 * Copyright (c) 1990-1996 by cisco Systems, Inc.
 *
 * Portions of this module are based on public domain software and/or
 * software copyright (c) 1990 by Network Computing Devices, Inc., and
 * used with permission.
 *------------------------------------------------------------------
 * $Log: async_ppp.h,v $
 * Revision 3.2.60.4  1996/09/07  02:08:26  snyder
 * CSCdi68409:  declare more things const
 *              572 out of data, 12 image
 * Branch: California_branch
 *
 * Revision 3.2.60.3  1996/08/07  09:03:07  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2.60.2  1996/06/18  01:49:31  hampton
 * Split the monolithic traffic array into smaller per-protocol traffic
 * arrays.  [CSCdi59224]
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/03/18  22:19:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/13  02:03:41  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.24.1  1996/02/20  21:23:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:52:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:38:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/28  23:19:45  syiu
 * CSCdi37843:  Fair Queuing with async fastswitch crashes box
 *
 * Revision 2.1  1995/06/07  23:08:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ASYNC_PPP_H__
#define __ASYNC_PPP_H__

/*
 * Manifest constants particular to the Point-To-Point encapsulation
 * of packets.
 */

#define PPP_FRAME_END 0x7E
#define PPP_FRAME_ESC 0x7D
#define PPP_ESCAPE_MECHANISM(x) ((x)^0x20)
#define PPP_XON 0x11
#define PPP_XOFF 0x13

#define PPPINITFCS      0xffff  /* Initial FCS value */
#define PPPGOODFCS      0xf0b8  /* Good final FCS value */

extern const unsigned short ppp_fcstab[256];

#define FCS_ACCUMULATE(fcs, ch) \
        (fcs) = ((fcs) >> 8) ^ ppp_fcstab[((fcs) ^ (ch)) & 0xff]

typedef unsigned short FrameCheckField;
/* number of bytes in the frame checksum field */
#define PPP_FCS_SIZE 2

/*
 * Expansion function which yields the maximum length of the encapsulated
 * version of a plain-text packet of a given length.  Includes room
 * for a leading FRAME_END (which serves to clear any accumulated trash
 * on the line), and a trailing FRAME_END, and a worst-case scenario
 * wherein each of the plain-text bytes and the frame check characters
 * requires escaping.
 */
#define MAX_PPP_ENCAPSULATED_LENGTH(len) (1+(((len)+PPP_FCS_SIZE)*2)+1)

/*
 * Define PPP context block
 */

enum appp_outstate {
    APPP_DATA,
    APPP_FCS_LSB,
    APPP_FCS_MSB,
    APPP_FRAME_END,
    APPP_IDLE
  };

struct pppContext_ {
    /* Fields common to input and output */
#ifdef PACKED_ESCAPE_MAP
    unsigned char EscapeCharBitmap[256/8];
#else /* PACKED_ESCAPE_MAP */
    unsigned char EscapeCharMap[256];
#endif /* PACKED_ESCAPE_MAP */
    const unsigned char *ActiveEscapeMap; /* Active escape map */
    hwidbtype *idb;		/* Pointer to async IDB, if any */

    /* Input processing */

    paktype *inpak;		/* Current packet being assembled */
    int inpaks;			/* Total packets received */
    int rcv_errors;		/* Receive errors */
    void (*iqueue)(paktype *, tt_soc *); /* Input queue service routine */
    int ioffset;		/* Receive offset */
    int imax;			/* Receive maximum */
    boolean inescape;		/* True if next input character escaped */
    boolean ingiant;            /* True if waiting for end-of-packet */
                                /*  after detecting giant */

    /* Output processing */

    int outchar;		/* Saved output character */
    enum appp_outstate outstate; /* Output state machine state */
    paktype *outpak;		/* Current output packet */
    uchar *outptr;		/* Pointer into current output packet */
    int outlen;			/* Bytes remaining in current output packet */
    ushort outfcs;		/* Accumulated FCS for output packet */
    queuetype oqueue;		/* Output packet queue */
};

#ifdef APPP_INTERNAL
/*
 * appp_get_oqueue()
 *	Get next packet from a PPP context output queue
 */
inline static paktype *
appp_get_oqueue(pppContext *ctx)
{
    paktype *pak;
    leveltype status;

    pak = dequeue(&ctx->oqueue);
    if (!pak) {
	hwidbtype *idb;

	idb = ctx->idb;
	if (idb) {
            status = raise_interrupt_level(NETS_DISABLE);
	    pak = (*idb->oqueue_dequeue)(idb);
            reset_interrupt_level(status);
	    if (pak) {
		GET_TIMESTAMP(idb->lastoutput);
                if (pak->acct_proto != ACCT_PROTO_LAST){
		   idb->counters.tx_cumbytes += pak->datagramsize;
                   idb->counters.outputs++;
		}
		slip_traffic.outpacks++;
		slip_traffic.outbytes += pak->datagramsize;
	    }
	}
    }
    return(pak);
}
#endif


extern boolean appp_tty_startup (tt_soc *, void (*)(paktype *, tt_soc *), 
				 int, int, hwidbtype *);
extern void appp_tty_shutdown(tt_soc *);
extern boolean appp_add_quoted_char (tt_soc *, uchar);
extern void appp_remove_quoted_char (tt_soc *, uchar);
extern void appp_startoutput(tt_soc *);
extern void appp_oqueue(tt_soc *, paktype *);
extern boolean appp_new_inpak(pppContext *);
extern void appp_rcv_error(tt_soc *);
/*
 * IETF PPP encapsulation module
 */

uint appp_txservice(tt_soc *, uchar *, int);
void appp_rxservice(tt_soc *, uchar *, int);
void appp_rcv_error(tt_soc *);
boolean appp_new_inpak(pppContext *);

#endif __ASYNC_PPP_H__
