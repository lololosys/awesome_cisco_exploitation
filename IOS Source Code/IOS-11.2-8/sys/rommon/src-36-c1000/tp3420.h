/* $Id: tp3420.h,v 3.2 1995/11/17 18:44:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src-36-c1000/tp3420.h,v $
 *------------------------------------------------------------------
 * tp3420.h
 *
 * January 1995, Rob Clevenger
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the National TP3420 ISDN chip
 *------------------------------------------------------------------
 * $Log: tp3420.h,v $
 * Revision 3.2  1995/11/17  18:44:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:07:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:39:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __TP3420_H__
#define __TP3420_H__

/* Register Commands */

/* Activation/Deactivation */
#define TP3420_CRF_NOP		0xff	/* NOP */
#define TP3420_CRF_PDN		0x00	/* Power-Down */
#define TP3420_CRF_PUP		0x20	/* Power-Up */
#define TP3420_CRF_DR		0x01	/* Deactivation request */
#define TP3420_CRF_FI2		0x02	/* Force INFO2 */
#define TP3420_CRF_MMA		0x1f	/* Monitor Mode Activation */
#define TP3420_CRF_AR		0x03	/* Activation Request */

/* Device Modes */
#define TP3420_CRF_NTA		0x04	/* NT Mode, Adaptive Sampling */
#define TP3420_CRF_NTF		0x05	/* NT Mode, Fixed Sampling */
#define TP3420_CRF_TES		0x06	/* TE Mode, DSI Slave */
#define TP3420_CRF_TEM		0x07	/* TE Mode, DSI Master */

/* Digital Interface Formats */
#define TP3420_CRF_DIF1		0x08	/* DSI Format 1 */
#define TP3420_CRF_DIF2		0x09	/* DSI Format 2 */
#define TP3420_CRF_DIF3		0x0a	/* DSI Format 3 */
#define TP3420_CRF_DIF4		0x0b	/* DSI Format 4 */

/* BCLK Frequency Settings */
#define TP3420_CRF_BCLK4	0x9b	/* BCLK 2.56 MHz */

/* B Channel Exchange */
#define TP3420_CRF_BDIR		0x0c	/* B Chan Mapped Direct, B1-B1 B2-B2 */
#define TP3420_CRF_BEX		0x0d	/* B Chan Exchanged, B1->B2 B2->B1 */

/* D Channel Access */
#define TP3420_CRF_DREQ1	0x0e	/* D Chan Request, Class 1 Message */
#define TP3420_CRF_DREQ2	0x0f	/* D Chan Request, Class 2 Message */

/* D Channel Access Control */
#define TP3420_CRF_DACCE	0x90	/* Enable D TE mode */
#define TP3420_CRF_DCKE		0xf1	/* D Chan Clock Enable */

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
#define TP3420_CRF_LBS		0x1a	/* Loopback 2B+D Towards Digtal IF */
#define TP3420_CRF_LBB1		0x1c	/* Loopback B1 Towards Digital IF */
#define TP3420_CRF_LBB2		0x1d	/* Loopback B2 Towards Digital IF */
#define TP3420_CRF_CAL		0x1b	/* Clear All Loopbacks */


/* Control Device State Reading */
#define TP3420_CRF_ENST     	0x92    /* Enable device state on the NOCST */
#define TP3420_CRF_DISST    	0x93    /* Disable device state on the NOCST */
#define TP3420_F6_STATE     	0xd0    /* State F6 */
#define TP3420_F7_STATE     	0xe0    /* State F7 */
#define TP3420_F8_STATE     	0xf0    /* State F8 */

/* Pin Signal Selection */
#define TP3420_CRF_PINDEF(n)	(0xe0 + (n & 0x03)) /* redefine pin signals */

/* Status Register Functions */
#define TP3420_SRF_NOC		0x00	/* No Change */
#define TP3420_SRF_LSD		0x02	/* Line Signal Detected from Far-End */
#define TP3420_SRF_AP		0x03	/* Activation Pending */
#define TP3420_SRF_EOM		0x06	/* End of Message */
#define TP3420_SRF_CON		0x07	/* Lost Contention */
#define TP3420_SRF_MFR		0x30	/* Multiframe RCV Buffer Full */
#define TP3420_SRF_AI		0x0c	/* Activation Indication */
#define TP3420_SRF_EI		0x0e	/* Error Indication */
#define TP3420_SRF_DI		0x0f	/* Deactivation Indication */


/* Q-Channel and S1-Channel Messages */

/* Receive messages - S1-Chan */
#define TP3420_S1CHAN_IDLE	0x0	/* Idle (Normal) */
#define TP3420_S1CHAN_LOP	0xf	/* Loss of Power */
#define TP3420_S1CHAN_STPP	0x2	/* STP Pass */
#define TP3420_S1CHAN_STPF	0x1	/* STP Fail */
#define TP3420_S1CHAN_STII	0x7	/* STI Indication */
#define TP3420_S1CHAN_DTSE_IN	0x8	/* DTSE-IN */
#define TP3420_S1CHAN_DTSE_OUT	0x4	/* DTSE-OUT */
#define TP3420_S1CHAN_DTSE_BOTH	0xC	/* DTSE-IN&OUT */
#define TP3420_S1CHAN_LB1I	0xd	/* LB1 Indication */
#define TP3420_S1CHAN_LB2I	0xb	/* LB2 Indication */
#define TP3420_S1CHAN_LBI	0x9	/* LB1/2 Indication */
#define TP3420_S1CHAN_LORSI	0xa	/* Loss-of-Received-Signal */

/* Send messages - Q-Chan */
#define TP3420_QCHAN_IDLE	0xf	/* Idle (Normal) */
#define TP3420_QCHAN_LOP	0x0	/* Loss of Power */
#define TP3420_QCHAN_STR	0x1	/* ST request */
#define TP3420_QCHAN_LB1R	0x7	/* LB1 request */
#define TP3420_QCHAN_LB2R	0xb	/* LB2 request */
#define TP3420_QCHAN_LBR	0x3	/* LB1/2 request */

#endif /* __TP3420_H__ */
