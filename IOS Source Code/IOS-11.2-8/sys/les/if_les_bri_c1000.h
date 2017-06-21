/* $Id: if_les_bri_c1000.h,v 3.4.12.2 1996/06/12 16:50:56 wmay Exp $
 * $Source: /release/112/cvs/Xsys/les/if_les_bri_c1000.h,v $
 *------------------------------------------------------------------
 * if_les_bri_c1000.h -- Definitions for BRI/TP3420 common to c1000. 
 *
 * NOTE: This file is a temporary hack for the 10.3 release, and will 
 * go away in Arizona.
 *
 * Feb 1995, Greg Stovall
 *	
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_les_bri_c1000.h,v $
 * Revision 3.4.12.2  1996/06/12  16:50:56  wmay
 * CSCdi39956:  BRI: show int stat and show int accounting only show D
 * channel info - put b channel idbs in the idb list.  This removes a
 * lot of special initialization code for BRI.  Also, make the names
 * of the b-channels more consisted with PRI (ie: BRI0:1 and BRI0:2).
 * Branch: California_branch
 *
 * Revision 3.4.12.1  1996/03/18  20:42:11  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/16  07:12:09  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.26.1  1996/01/24  22:24:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/02/15  18:02:30  gtaylor
 * CSCdi45779:  Leased Line ISDN work.
 *
 * Revision 3.3  1995/11/17  17:40:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:59:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:32:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:35:48  hampton
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

#define DCHAN_INSTANCE		dchan_idb->devctl

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

/* BCLK Frequency Settings */
#define TP3420_CRF_BCLK1	0x98	/* Set BCLK to 2.048 MHz */
#define TP3420_CRF_BCLK2	0x99	/* Set BCLK to 256 kHz */
#define TP3420_CRF_BCLK3	0x9A	/* Set BCLK to 512 kHz */
#define TP3420_CRF_BCLK4	0x9B	/* Set BCLK to 2.56 MHz */

/* B Channel Exchange */
#define TP3420_CRF_BDIR		0x0C	/* B Chan Mapped Direct, B1-B1 B2-B2 */
#define TP3420_CRF_BEX		0x0D	/* B Chan Exchanged, B1->B2 B2->B1 */

/* D Channel Access */
#define TP3420_CRF_DREQ1	0x0E	/* D Chan Request, Class 1 Message */
#define TP3420_CRF_DREQ2	0x0F	/* D Chan Request, Class 2 Message */

/* D Channel Access Control */
#define TP3420_CRF_DACCE	0x90	/* Enable D-chan Access, TE Mode */
#define TP3420_CRF_DACCD	0x91	/* Enable D-chan Access, TE Mode */
#define TP3420_CRF_EBIT0	0x96	/* Force Echo Bit to 0 */
#define TP3420_CRF_EBITI	0x97	/* Force Echo Bit to Inverted RX */ 
#define TP3420_CRF_EBITNRM	0x9C	/* Reset EBIT0 & EBITI to Normal */  
#define TP3420_CRF_DCKE		0xF1	/* D Channel Clock Enable */

/* End of Message Interrupt */
#define TP3420_CRF_EIE		0x10	/* EOM Interrupt Enabled */
#define TP3420_CRF_EID		0x11	/* EOM Interrupt Disabled */

/* Multiframe Circuit and Interrupt */
#define TP3420_CRF_MIE1		0x12	/* Enable SC1/Q Mess & MFR1 Int */
#define TP3420_CRF_MID1		0x13	/* Disable SC1/Q Mess and Int */
#define TP3420_CRF_MFC1E	0x22	/* Enable 5ms Int (Every Multiframe) */
#define TP3420_CRF_MFC1D	0x23	/* Disable 5ms Int */
#define TP3420_CRF_MFC6E	0x24	/* Enable 30ms Int (6 Multiframes) */
#define TP3420_CRF_MFC6D	0x25	/* Disable 30ms Int */
#define TP3420_CRF_MIE2		0x26	/* Enable SC2 Mess & MFR2 Int */
#define TP3420_CRF_MID2		0x27	/* Disable SC2 Mess and Int */

/* Multiframe Receive Message Validation */
#define TP3420_CRF_ENV		0x28	/* Enable 3X & 1X Validation */
#define TP3420_CRF_DISV		0x29	/* Disable 3X & 1X Validation */

/* Multiframe Transmit register */
#define TP3420_CRF_MFT1L	0x30	/* Write to Multiframe SC1 Tx Reg */
#define TP3420_CRF_MFT1H	0x40	/* Write to Multiframe SC1 Tx Reg */
#define TP3420_CRF_MFT2		0x50	/* Write to Multiframe SC2 Tx Reg */

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
#define TP3420_CRF_ENST     	0x92    /* Enable device state on the NOCST */
#define TP3420_CRF_DISST    	0x93    /* Disable device state on the NOCST */
#define TP3420_F1_STATE     	0x80    /* State F1 */
#define TP3420_F2_STATE     	0x90    /* State F2 */
#define TP3420_F3_STATE     	0xA0    /* State F3 */
#define TP3420_F4_STATE     	0xB0    /* State F4 */
#define TP3420_F5_STATE     	0xC0    /* State F5 */
#define TP3420_F6_STATE     	0xD0    /* State F6 */
#define TP3420_F7_STATE     	0xE0    /* State F7 */
#define TP3420_F8_STATE     	0xF0    /* State F8 */

/* Control of Additional Interrupts */
#define TP3420_CRF_ENINT	0x94	/* Enable Slip & RMFE Interrupts */
#define TP3420_CRF_DISINT	0x95	/* Disable Slip & RMFE Interrupts */

/* Control Polarity of B Channel Data */
#define TP3420_CRF_INVB1	0x9D	/* Invert B1 Channel Data */
#define TP3420_CRF_INVB2	0x8E	/* Invert B2 Channel Data */
#define TP3420_CRF_NRMB12	0x9F	/* Normal B1, B2 Data */

/* Pin Signal Selection */
#define TP3420_CRF_PINDEF	0xE0	/* Redfine Pin Signals */
#define TP3420_P8_DENX		0x00	/* Pin 8 is DENx */
#define TP3420_P8_SCLK		0x04	/* Pin 8 is SCLK */
#define TP3420_P18_LSD		0x00	/* Pin 18 is LSD */
#define TP3420_P18_DENR		0x01	/* Pin 18 is DENr */
#define TP3420_P18_SCLK		0x02	/* Pin 18 is SCLK */
#define TP3420_P18_DENX		0x03	/* Pin 18 is DENx */

/* Status Register Functions */
#define TP3420_SRF_NOC		0x00	/* No Change */
#define TP3420_SRF_LSD		0x02	/* Line Signal Detected from Far-End */
#define TP3420_SRF_AP		0x03	/* Activation Pending */
#define TP3420_SRF_EOM		0x06	/* End of Message */
#define TP3420_SRF_CON		0x07	/* Lost Contention */
#define TP3420_SRF_MFR1		0x30	/* Multiframe RCV Buffer 1 Full */
#define TP3420_SRF_MFR2		0x50	/* Multiframe RCV Buffer 2 Full */
#define TP3420_SRF_AI		0x0C	/* Activation Indication */
#define TP3420_SRF_EI		0x0E	/* Error Indication */
#define TP3420_SRF_DI		0x0F	/* Deactivation Indication */
#define TP3420_SRF_MFC		0x04	/* Multiframe Clock (5ms or 30ms) */

/* Additional Interrupts after ENINT Command */
#define TP3420_SRF_RMFE		0x05	/* Receive Multiframe Error */
#define TP3420_SRF_SLIP_TX	0x09	/* Phase Slip in Buffer for Bx Data */
#define TP3420_SRF_SLIP_RX	0x0A	/* Phase Slip in Buffer for Br Data */
#define TP3420_SRF_SLIP_TXRX	0x0B	/* Phase Slip for Both Bx and Br */

/* NO Change Return Status */
#define TP3420_SRF_NOC		0x00	/* NOC Status after DISST Command */
#define TP3420_SRF_NOCST	0x80	/* NOC Status after ENST Command */


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
 * Bri test interface specific defines.
 */
#define BRI_BOTH_LOOP_SKIPPED 2
#define BRI_ONE_LOOP_SKIPPED  1
#define BRI_NO_LOOP_SKIPPED   0

#define BRI_TEST_DATA_SIZE       100
#define BRI_TEST_DATA_COUNT        5
#define BRI_TEST_DATA_GAP          2
#define BRI_TEST_DATA_MAX_SIZE  1500



/*
 * Prototypes and externs.
 */
void bri_enq_prim(uint, uint);
void bri_init_isdn_queue(void);
hwidbtype *init_bri_idb_common(uint);
void bri_drop_pak(hwidbtype *idb, paktype *pak);
void bri_setup_bridging(hwidbtype *idb);
uchar bri_tint_state_handler(hwidbtype *dchan_idb);
void bri_display_layer1(hwidbtype *dchan_idb);
void test_bri_init(hwidbtype *dchan_idb);
void test_bri_interface(hwidbtype *idb, int *nfailed, int *npassed,
			int *nskipped);

extern void isdn_bri_enable_layer1_Q(void); 
extern void bri_init(hwidbtype *, boolean);
extern void bri_disable(hwidbtype *);
extern void tp3420_init(hwidbtype *);
extern void tp3420_enable_line(hwidbtype *);
extern void bri_start_t3(hwidbtype *);
extern void bri_stop_t3(hwidbtype *);
extern void bri_start_t4(hwidbtype *);
extern void bri_stop_t4(hwidbtype *);
extern void write_sid(hwidbtype *, uchar);
extern uchar read_sid(hwidbtype *);
extern void bri_dchan_coll(hwidbtype *);
extern void bri_chan_init(hwidbtype *);
extern void bri_up_channels(hwidbtype *);
extern void bri_down_channels(hwidbtype *);
extern void bri_restart (hwidbtype *idb);
extern boolean is_layer1_current_state_activated(hwidbtype *hwidb);
extern void init_bri_leased_line (hwidbtype *hwidb);

extern void bri_init_idb_vectors(hwidbtype *);

extern queuetype *IsdnL1Q;		/* L1 Q to ISDN */	
extern queuetype *IsdnL1PrimPoolQ;	/* L1 primitives pool */
extern queuetype test_bri_loopbackQ; /* Queue for to store packets which were
                                        looped back on the BRI under test */
extern queuetype test_bri_echoQ;     /* ICMP echo queue used when testing bri
                                        interface */

/*
 * if_les_bri_chain.c
 */
void bri_parser_init(void);

