/* $Id: if_les_bri.h,v 3.4.12.2 1996/06/12 16:50:53 wmay Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_bri.h,v $
 *------------------------------------------------------------------
 * if_les_bri.h -- Definitions for BRI/TP3420 common to XX/low end. 
 *
 * March 1994, Manoj Leelanivas
 * Based on "if_les_bri.c", November 1991, Greg Stovall
 *	
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_bri.h,v $
 * Revision 3.4.12.2  1996/06/12  16:50:53  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  20:42:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/16  07:12:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.1  1996/01/24  22:24:03  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/15  18:02:27  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.3  1995/11/17  17:40:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:06  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:35:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/* General defines */
#define VISIBLE_BRI_BANDWIDTH_64 64 /* BRI bandwidth for IGRP display 64kb   */
#define VISIBLE_BRI_BANDWIDTH_56 56 /* BRI bandwidth for IGRP display 56kb   */
#define VISIBLE_BRI_BANDWIDTH_16 16 /* BRI bandwidth for IGRP display 16kb   */
#define BRI_SERIAL_DELAY 2000      /* BRI Serial delay */

#define BRI_D_CHAN 0               /* Index for D chan */
#define BRI_B1_CHAN 1              /* Index for B1 chan */
#define BRI_B2_CHAN 2              /* Index for B2 chan */
#define BRI_MAX_CHAN 3             /* Max number of Chans (2b+d) */

/* Defines for the National TP3420 ISDN chip */

/* Register Commands */

/* Activation/Deactivation */
#define TP3420_CRF_NOP		0xFF	/* NOP */
#define TP3420_CRF_PDN		0x00	/* Power-Down */
#define TP3420_CRF_PUP		0x20	/* Power-Up */
#define TP3420_CRF_DR		0x01	/* Deactivation request */
#define TP3420_CRF_FI2		0x02	/* Force INFO2 */
#define TP3420_CRF_MMA		0x1F	/* Monitor Mode Activation */
#define TP3420_CRF_AR		0x03	/* Activation Request */

/* Device Modes */
#define TP3420_CRF_NTA		0x04	/* NT Mode, Adaptive Sampling */
#define TP3420_CRF_NTF		0x05	/* NT Mode, Fixed Sampling */
#define TP3420_CRF_TES		0x06	/* TE Mode, DSI Slave */
#define TP3420_CRF_TEM		0x07	/* TE Mode, DSI Master */

/* Digital Interface Formats */
#define TP3420_CRF_DIF1		0x08	/* DSI Format 1 */
#define TP3420_CRF_DIF2		0x09	/* DSI Format 2 */
#define TP3420_CRF_DIF3		0x0A	/* DSI Format 3 */
#define TP3420_CRF_DIF4		0x0B	/* DSI Format 4 */

/* B Channel Exchange */
#define TP3420_CRF_BDIR		0x0C	/* B Chan Mapped Direct, B1-B1 B2-B2 */
#define TP3420_CRF_BEX		0x0D	/* B Chan Exchanged, B1->B2 B2->B1 */

/* D Channel Access */
#define TP3420_CRF_DREQ1	0x0E	/* D Chan Request, Class 1 Message */
#define TP3420_CRF_DREQ2	0x0F	/* D Chan Request, Class 2 Message */

/* End of Message Interrupt */
#define TP3420_CRF_EIE		0x10	/* EOM Interrupt Enabled */
#define TP3420_CRF_EID		0x11	/* EOM Interrupt Disabled */

/* Multiframe Circuit and Interrupt */
#define TP3420_CRF_MIE		0x12	/* Multiframe Circuit & Int Enabled */
#define TP3420_CRF_MID		0x13	/* Multiframe Circuit & Int Disabled */

/* Multiframe Receive Message 3X Checking */
#define TP3420_CRF_EN3X		0x28	/* Enable 3X Checking */
#define TP3420_CRF_DIS3X	0x29	/* Disable #x Checking */

/* Multiframe Transmit register */
#define TP3420_CRF_MFT		0x30	/* Write to Multiframe XMT Reg */

/* B1 Channel Enable/Disable */
#define TP3420_CRF_B1E		0x14	/* B1 Channel Enabled */
#define TP3420_CRF_B1D		0x15	/* B1 Channel Disabled */

/* B2 Channel Enable/Disable */
#define TP3420_CRF_B2E		0x16	/* B2 Channel Enabled */
#define TP3420_CRF_B2D		0x17	/* B2 Channel Disabled */

/* Loopback Test Modes */
#define TP3420_CRF_LBL1		0x18	/* Loopback B1 Towards Line IF */
#define TP3420_CRF_LBL2		0x19	/* Loopback B2 Towards Line IF */
#define TP3420_CRF_LBS		0x1A	/* Loopback 2B+D Towards Digtal IF */
#define TP3420_CRF_LBB1		0x1C	/* Loopback B1 Towards Digital IF */
#define TP3420_CRF_LBB2		0x1D	/* Loopback B2 Towards Digital IF */
#define TP3420_CRF_CAL		0x1B	/* Clear All Loopbacks */

/* Control Device State Reading */
#define TP3420_CRF_ENST     0x92    /* Enable device state on the NOCST */
#define TP3420_CRF_DISST    0x93    /* Disable device state on the NOCST */
#define TP3420_F1_STATE     0x80    /* State F1 */
#define TP3420_F2_STATE     0x90    /* State F2 */
#define TP3420_F3_STATE     0xA0    /* State F3 */
#define TP3420_F4_STATE     0xB0    /* State F4 */
#define TP3420_F5_STATE     0xC0    /* State F5 */
#define TP3420_F6_STATE     0xD0    /* State F6 */
#define TP3420_F7_STATE     0xE0    /* State F7 */
#define TP3420_F8_STATE     0xF0    /* State F8 */


/* Status Register Functions */
#define TP3420_SRF_NOC		0x00	/* No Change */
#define TP3420_SRF_LSD		0x02	/* Line Signal Detected from Far-End */
#define TP3420_SRF_AP		0x03	/* Activation Pending */
#define TP3420_SRF_EOM		0x06	/* End of Message */
#define TP3420_SRF_CON		0x07	/* Lost Contention */
#define TP3420_SRF_MFR		0x30	/* Multiframe RCV Buffer Full */
#define TP3420_SRF_AI		0x0C	/* Activation Indication */
#define TP3420_SRF_EI		0x0E	/* Error Indication */
#define TP3420_SRF_DI		0x0F	/* Deactivation Indication */


/* Q-Channel and S1-Channel Messages */

/* Receive messages - S1-Chan */
#define TP3420_S1CHAN_IDLE	0x0	/* Idle (Normal) */
#define TP3420_S1CHAN_LOP	0xF	/* Loss of Power */
#define TP3420_S1CHAN_STPP	0x2	/* STP Pass */
#define TP3420_S1CHAN_STPF	0x1	/* STP Fail */
#define TP3420_S1CHAN_STII	0x7	/* STI Indication */
#define TP3420_S1CHAN_DTSE_IN	0x8	/* DTSE-IN */
#define TP3420_S1CHAN_DTSE_OUT	0x4	/* DTSE-OUT */
#define TP3420_S1CHAN_DTSE_BOTH	0xC	/* DTSE-IN&OUT */
#define TP3420_S1CHAN_LB1I	0xD	/* LB1 Indication */
#define TP3420_S1CHAN_LB2I	0xB	/* LB2 Indication */
#define TP3420_S1CHAN_LBI	0x9	/* LB1/2 Indication */
#define TP3420_S1CHAN_LORSI	0xA	/* Loss-of-Received-Signal */

/* Send messages - Q-Chan */
#define TP3420_QCHAN_IDLE	0xF	/* Idle (Normal) */
#define TP3420_QCHAN_LOP	0x0	/* Loss of Power */
#define TP3420_QCHAN_STR	0x1	/* ST request */
#define TP3420_QCHAN_LB1R	0x7	/* LB1 request */
#define TP3420_QCHAN_LB2R	0xB	/* LB2 request */
#define TP3420_QCHAN_LBR	0x3	/* LB1/2 request */

/*
 * Prototypes and externs.
 */
void bri_enq_prim(uint, uint);
void bri_init_isdn_queue(void);
hwidbtype *init_bri_idb_common(uint);

extern void isdn_bri_enable_layer1_Q(void); 
extern void bri_init(hwidbtype *, boolean);

extern void bri_init_idb_vectors(hwidbtype *);

extern void bri_restart (hwidbtype *idb);
extern boolean is_layer1_current_state_activated(hwidbtype *hwidb);
extern void init_bri_leased_line(hwidbtype *hwidb);

extern queuetype *IsdnL1Q;		/* L1 Q to ISDN */	
extern queuetype *IsdnL1PrimPoolQ;	/* L1 primitives pool */

/*
 * if_les_bri_chain.c
 */
void bri_parser_init(void);
