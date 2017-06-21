/* $Id: lapb.h,v 3.5.30.1 1996/06/27 09:47:43 fox Exp $
 * $Source: /release/112/cvs/Xsys/x25/lapb.h,v $
 *------------------------------------------------------------------
 * lapb.h -- Link Access Protocol, Balanced
 *
 * December 1986, Greg Satz
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lapb.h,v $
 * Revision 3.5.30.1  1996/06/27  09:47:43  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.5  1996/01/15  04:47:59  atsao
 * CSCdi46647:  Update N1 when MTU, X25 modulo or LAPB compression is
 * changed
 * Dynamically update N1 value whenever there is configuration change on
 * hardware MTU, LAPB/X25 modulo, or lapb compression.
 *
 * Revision 3.4  1996/01/12  23:29:24  atsao
 * CSCdi44422:  LAPB N1 command not accepted, N1 falls back to default
 * If user has ever configured LAPB N1 value, make sure this N1 is shown
 * in the show interface output, otherwise, update this N1 value to
 * reflect the current configuration.
 *
 * Revision 3.3  1995/11/17  18:06:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:51:40  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:52:32  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/09  00:55:16  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.2  1995/06/14  20:39:22  gglick
 * CSCdi30355:  Unexpected LAPB reset under severe traffic condition
 *         Implement receiver busy (RNR) when interface is congested.
 *
 * Revision 2.1  1995/06/07  23:20:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LAPB_H__
#define __LAPB_H__

/*
 * logical interface types
 */
#define LAPBX25_DTE	0
#define LAPBX25_DXE	1
#define LAPBX25_DCE	2

#define	IS_DCE(var, type)	(var->type == LAPBX25_DCE)
#define	IS_DTE(var, type)	(var->type == LAPBX25_DTE)
#define	IS_DXE(var, type)	(var->type == LAPBX25_DXE)

/*
 * protocol values
 */

#define LAPB_MODULO_8	8
#define LAPB_MODULO_128	128

#define LAPB_OVERHEAD_8		2
#define LAPB_OVERHEAD_128	3
#define LAPB_OVERHEAD(idb)	(idb->lapb_modulo == LAPB_MODULO_128 ?	\
				 LAPB_OVERHEAD_128 : LAPB_OVERHEAD_8)

/*
 * Linked list of those idbs running LAPB
 */

struct lapbidbtype_ {
    lapbidbtype *next;
    hwidbtype *hwidb;
};


#define LAPB_CRC_BITS	16

/*
 * multiprotocol encapsulation Protocol ID field
 */
#define	LAPB_PID_BYTES	sizeof(ushort)

/*
 *  define default parameters
 */
#define	LAPB_DEFAULT_MODULO	LAPB_MODULO_8
#define	LAPB_DEFAULT_T1	(3 * ONESEC)
#define	LAPB_DEFAULT_T3	0
#define	LAPB_DEFAULT_T4	0
#define	LAPB_DEFAULT_N1(idb) lapb_default_n1(idb)
#define	LAPB_DEFAULT_N2	20
#define	LAPB_DEFAULT_K	(LAPB_MODULO_8 - 1)
#define	LAPB_HOLDQ	(LAPB_MODULO_8 + (LAPB_MODULO_8 / 2))

/*
 *  define N1 when lapb modulo is changed
 */
#define LAPB_ADMIN_N1(idb) lapb_admin_n1(idb)

/*
 * Address field
 */

#define LAPB_ADDRESS_INX	0

#define LAPB_ADDRESSA 0x03
#define LAPB_ADDRESSB 0x01

#define LAPB_CMDADDR(idb)(IS_DCE(idb, lapb_dce) ? LAPB_ADDRESSA : LAPB_ADDRESSB)
#define	LAPB_RSPADDR(idb)(IS_DCE(idb, lapb_dce) ? LAPB_ADDRESSB : LAPB_ADDRESSA)


/*
 * Control Field values
 */

#define LAPB_CONTROL_INX	1

#define	LAPB_ISIFRAME(pak) (((pak)->datagramstart[LAPB_CONTROL_INX] &	\
			     LAPB_IFRAME_M) == LAPB_IFRAME_V)
#define	LAPB_ISSFRAME(pak) (((pak)->datagramstart[LAPB_CONTROL_INX] &	\
			     LAPB_SFRAME_M) == LAPB_SFRAME_V)
#define	LAPB_ISUFRAME(pak) (((pak)->datagramstart[LAPB_CONTROL_INX] &	\
			     LAPB_UFRAME_M) == LAPB_UFRAME_V)

#define	LAPB_NOFRAME	0xFF

#define LAPB_IFRAME_M	0x01
#define LAPB_IFRAME_V	0x00
#define LAPB_IFRAME	0x00

#define LAPB_SFRAME_M	0x03
#define LAPB_SFRAME_V	0x01
#define LAPB_RR		0x01
#define LAPB_RNR	0x05
#define LAPB_REJ	0x09

#define LAPB_UFRAME_M	0x03
#define LAPB_UFRAME_V	0x03
#define LAPB_U_CONTROL_M 0xEF
#define LAPB_DM		0x0F
#define LAPB_SABM	0x2F
#define LAPB_DISC	0x43
#define LAPB_UA		0x63
#define LAPB_SABME	0x6F
#define LAPB_FRMR	0x87
#define LAPB_XID	0xAF


/*
 * FRMR reason bits
 */

#define LAPB_FRMR_W	0x01	/* undefined control field */
#define LAPB_FRMR_X	0x02	/* information field not allowed (with W) */
#define LAPB_FRMR_Y	0x04	/* information field too long */
#define LAPB_FRMR_Z	0x08	/* invalid N(R) */


/*
 *  enumerate the protocol service states, transmit machine states
 *  and receive machine states
 *
 *  Why aren't lapb_state, lapb_tx_state and lapb_rx_state typed to these
 *  enumerations?  Because the compiler won't take an unenumerated enum
 *  type and I don't want to clutter h/interface.h with them. *sigh*.
 */
enum LAPB_STATE {
    LAPB_DISCONNECTED,
    LAPB_SABM_SENT,
    LAPB_CONNECTED,
    LAPB_FRMR_SENT,
    LAPB_DISC_SENT
};

/*
 *  the transmit machine is responsible for ensuring that what it has
 *  sent is received by the other side--it does this by using the T1
 *  timer, the Polling mechanism and the transmission count (N2)
 */
enum LAPB_TX_STATE {
    LAPB_TX_INACTIVE,		/* protocol is not connected */
    LAPB_TX_NORMAL,		/* able to send */
    LAPB_TX_REMOTE_BUSY		/* received an RNR */
};

enum LAPB_RX_STATE {
    LAPB_RX_INACTIVE,		/* protocol is not connected */
    LAPB_RX_NORMAL,		/* able to receive */
    LAPB_RX_REJ_SENT,		/* recovering lost I-frame */
    LAPB_RX_CONGESTED		/* RNR sent */
};


/*
 *  RFC 1381 lapbFlowCurrentMode
 */
#define LAPB_MIBMODE_DISCON	1	/* initial state or DISC received */
#define LAPB_MIBMODE_SETUP	2	/* SABM/E sent */
#define LAPB_MIBMODE_FRMR	3	/* invalid frame rx, FRMR sent */
#define LAPB_MIBMODE_DISC_SENT	4	/* DISC sent */
#define LAPB_MIBMODE_CON	5	/* connected */
#define LAPB_MIBMODE_REJ	6	/* REJ sent */
#define LAPB_MIBMODE_T1		7	/* polling for acknowledgement */
#define LAPB_MIBMODE_RNR_TX	8	/* RNR sent */
#define LAPB_MIBMODE_RNR_RX	9	/* RNR received */
#define LAPB_MIBMODE_RNR_BOTH	10	/* both stations are RNR */
#define LAPB_MIBMODE_T1_RNR_TX	11	/* T1 expired, RNR sent */
#define LAPB_MIBMODE_T1_RNR_RX	12	/* T1 expired, RNR received */
#define LAPB_MIBMODE_T1_RNR_B	13	/* T1 expired, both RNR */
#define LAPB_MIBMODE_REJ_RNR	14	/* REJ sent, RNR received */
#define LAPB_MIBMODE_XID	15	/* XID frame sent */
#define LAPB_MIBMODE_ERROR	16	/* other error state not listed above */
#define LAPB_MIBMODE_OTHER	17	/* other state not listed above */

/*
 *  RFC 1381 lapbFlowChangeReason
 */
#define LAPB_MIBREASON_INITIAL	1	/* initial reason */
#define LAPB_MIBREASON_ABM	2	/* asynch. balanced mode entered */
#define LAPB_MIBREASON_ABME	3	/* modulo 128 ABM entered */
#define LAPB_MIBREASON_RESET	4	/* service reset */
#define LAPB_MIBREASON_E_RESET	5	/* modulo 128 service reset */
#define LAPB_MIBREASON_DM_RX	6	/* DM received */
#define LAPB_MIBREASON_DM_TX	7	/* DM sent */
#define LAPB_MIBREASON_DISC_RX	8	/* DISC received */
#define LAPB_MIBREASON_DISC_TX	9	/* DISC sent */
#define LAPB_MIBREASON_FRMR_RX	10	/* FRMR received */
#define LAPB_MIBREASON_FRMR_TX	11	/* FRMR sent */
#define LAPB_MIBREASON_N2	12	/* N2 unsuccessful tries made */
#define LAPB_MIBREASON_OTHER	13	/* unspecified reason */

#define NEW_MIB_STATE(idb, state, reason)		\
	if ((reason == LAPB_MIBREASON_RESET) ||		\
	    (reason == LAPB_MIBREASON_E_RESET) ||	\
	    (state != idb->lapb_mib_state)) {		\
	    idb->lapb_mib_state_changes++;		\
	    idb->lapb_mib_state = state;		\
	    idb->lapb_state_reason = reason;		\
	}

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/

extern void lapb_init(void);
extern boolean lapb_vencapsulate(paktype *pak, long address);
extern boolean lapb_output(hwidbtype *idb, paktype *pak, enum HEADTAIL which);
extern void lapb_u_frame(hwidbtype *idb, paktype *pak, uchar frame_id,
			boolean poll_final);
extern void lapb_s_frame(hwidbtype *idb, paktype *pak,
			boolean command, boolean poll_final);
extern paktype *lapb_pull_frame (hwidbtype *idb);
extern char *lapb_prstate(hwidbtype *idb);
extern void lapb_prframe(hwidbtype *idb, paktype *pak, char io);
extern boolean lapb_validcmdframe(hwidbtype *idb, paktype *pak);
extern void lapb_start(hwidbtype *idb, paktype *pak, uchar ack, uchar poll,
			uchar mib_reason);
extern void lapb_input(hwidbtype *idb, paktype *pak);
extern void lapb_frmr(hwidbtype *idb, paktype *epak, uchar reason,
			uchar poll_final, uchar cmd);
extern boolean lapb_ack(hwidbtype *idb, paktype *pak, uchar poll,
			uchar cmd, int nr);
extern boolean lapbBLOCK(void);
extern forktype lapb_timer(void);
extern void lapb_reset(hwidbtype *idb, char *why);
extern long lapb_getlink(paktype *pak);
extern fuzzy lapb_bcast(hwidbtype *, paktype *pak);
extern int lapb_default_n1(hwidbtype *idb);
extern int lapb_admin_n1(hwidbtype *idb);
extern int lapb_min_n1(hwidbtype *idb);
extern void lapb_update_n1(hwidbtype *idb);
extern void lapb_deencapsulate(hwidbtype *idb);
extern boolean lapb_setup(parseinfo *csb, hwidbtype *idb);
extern boolean lapb_x25_oqueue(hwidbtype *idb, paktype *pak,
				enum HEADTAIL which);
extern paktype *lapb_x25_oqueue_dequeue(hwidbtype *idb);
extern void lapb_check_output_vectors (hwidbtype *idb);
extern void lapb_cstate(hwidbtype *idb);
extern void lapb_interface(hwidbtype *idb);
extern void lapb_clearcounters(hwidbtype *idb);
extern int lapb_payload(hwidbtype *idb);
extern void copy_lapb_fields(hwidbtype *idb1, hwidbtype *idb2);
extern void lapb_ppp_setup(hwidbtype *idb, uchar address, uchar window,
                           int maxhdr);
extern void lapb_ppp_close(hwidbtype *idb);

/*
 * lapb_chain.c
 */
extern void lapb_parser_init(void);

#endif __LAPB_H__
