/* $Id: hw_phy.h,v 3.1.60.1 1996/03/22 00:16:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/hw_phy.h,v $
 *------------------------------------------------------------------
 * hw_phy.h - 1F MIF68840 based PCI port adaptor support
 *             ( Port adapter platforms common PHY definitions for XDI )
 *
 * December 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hw_phy.h,v $
 * Revision 3.1.60.1  1996/03/22  00:16:05  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1995/12/23  04:42:26  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/12/19  20:36:12  fsunaval
 * Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __HW_PHY_H__
#define __HW_PHY_H__

/*
@(#) xm7_hw_phy.h xm7_v3.1.1:cset.0166:7:7 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Hardware Interface Header File

		PHY Register Access
		****	FOR MOTOROLA, DEC, and AMD PHYs WITH BUILT-IN
		****	PCM SUPPORT.

		File:			xm7_hw_phy.h
		Created:		02/13/91

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0166		01/10/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED

		This file was generated based on the data sheets for
		the Motorola ELM (MC68837) Revision 1.7 and the AMD
		AmPLC (Am79C864) Preliminary documentation.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*
*	Register Definitions
*/
#define	CNTRL_A_REG				(0x00)
#define	CNTRL_B_REG				(0x01)
#define	INTR_MASK_REG			(0x02)
#define	XMIT_VECTOR_REG			(0x03)
#define	VECTOR_LENGTH_REG		(0x04)
#define	LE_THRESHOLD_REG		(0x05)
#define	C_MIN_REG				(0x06)
#define	A_MAX_REG				C_MIN_REG	/* older SMT name */
#define	TL_MIN_REG				(0x07)
#define	LS_MAX_REG				TL_MIN_REG	/* older SMT name */
#define	TB_MIN_REG				(0x08)
#define	T_OUT_REG				(0x09)
#define	LC_SHORT_REG			(0x0B)
#define	T_SCRUB_REG				(0x0C)
#define	NS_MAX_REG				(0x0D)
#define	TPC_LOAD_VALUE_REG		(0x0E)
#define	TNE_LOAD_VALUE_REG		(0x0F)
#define	STATUS_A_REG			(0x10)
#define	STATUS_B_REG			(0x11)
#define	TPC_REG					(0x12)
#define	TNE_REG					(0x13)
#define	CLK_DIV_REG				(0x14)
#define	BIST_SIGNATURE_REG		(0x15)
#define	RCV_VECTOR_REG			(0x16)
#define	INTR_EVENT_REG			(0x17)
#define	VIOL_SYM_CTR_REG		(0x18)
#define	MIN_IDLE_CTR_REG		(0x19)
#define	LINK_ERR_CTR_REG		(0x1A)
#define FOTOFF_ASSERT_REG		(0x1E)
#define FOTOFF_DEASSERT_REG		(0x1F)

/*
*	Control A Register Field Masks
*/
#define	NOISE_TIMER_MASK		(1 << 14)
#define	TNE_16BIT_MASK			(1 << 13)
#define	TPC_16BIT_MASK			(1 << 12)
#define	REQ_SCRUB_MASK			(1 << 11)
#define	VSYM_CTR_INTRS_MASK		(1 << 9)
#define	MINI_CTR_INTRS_MASK		(1 << 8)
#define	FCG_LOOP_CNTRL_MASK		(1 << 7)/* Motorola name */
#define	CD_LOOP_CNTRL_MASK		(1 << 7)/* AMD name */
#define	FOT_OFF_MASK			(1 << 6)
#define	EB_LOC_LOOP_MASK		(1 << 5)
#define	ELM_LOC_LOOP_MASK		(1 << 4)/* Motorola name */
#define	LM_LOC_LOOP_MASK		(1 << 4)/* AMD name */
#define	SC_BYPASS_MASK			(1 << 3)
#define	SC_REM_LOOP_MASK		(1 << 2)
#define	RF_DISABLE_MASK			(1 << 1)
#define	RUN_BIST_MASK			(1)

/*
*	Control B Register Field Masks
*/
#define	CONFIG_CNTRL_MASK		(1 << 15)	/* 1000 0000 0000 0000 */
#define	MATCH_LS_MASK			(0xF << 11)	/* 0111 1000 0000 0000 */
#define	MAINT_LS_MASK			(7 << 8)/* 0000 0111 0000 0000 */
#define	CLASS_S_MASK			(1 << 7)/* 0000 0000 1000 0000 */
#define	PC_LOOP_MASK			(3 << 5)/* 0000 0000 0110 0000 */
#define	PC_JOIN_MASK			(1 << 4)/* 0000 0000 0001 0000 */
#define	LONG_MASK				(1 << 3)/* 0000 0000 0000 1000 */
#define	PC_MAINT_MASK			(1 << 2)/* 0000 0000 0000 0100 */
#define	PCM_CNTRL_MASK			(3)		/* 0000 0000 0000 0011 */

/*
*	Status A Register Field Masks
*/
#define	SIGNAL_DETECT_MASK		(1 << 10)
#define	PREV_LINE_ST_MASK		(3 << 8)
#define	LINE_ST_MASK			(7 << 5)
#define	LSM_STATE_MASK			(1 << 4)
#define	UNKN_LINE_ST_MASK		(1 << 3)
#define	SYM_PR_CTR_MASK			(7)

/*
*	Status B Register Field Masks
*/
#define	RF_STATE_MASK			(3 << 14)
#define	PCI_STATE_MASK			(3 << 12)
#define	PCI_SCRUB_MASK			(1 << 11)
#define	PCM_STATE_MASK			(0xF << 7)
#define	PCM_SIGNALING_MASK		(1 << 6)
#define	LSF_MASK				(1 << 5)
#define	RCF_MASK				(1 << 4)
#define	TCF_MASK				(1 << 3)
#define	BREAK_REASON_MASK		(7)

/*
*	Interrupt Register Masks
*/
#define	INTR_NP_ERR				0x8000
#define	INTR_LSD				0x4000
#define	INTR_LE_CTR				0x2000
#define	INTR_MINI_CTR			0x1000
#define	INTR_VSYM_CTR			0x0800
#define	INTR_PHYINV				0x0400
#define	INTR_EBUF_ERR			0x0200
#define	INTR_TNE_EXPIRED		0x0100
#define	INTR_TPC_EXPIRED		0x0080
#define	INTR_PCM_ENABLED		0x0040
#define	INTR_PCM_BREAK			0x0020
#define	INTR_SELF_TEST			0x0010
#define	INTR_TRACE_PROP			0x0008
#define	INTR_PCM_CODE			0x0004
#define	INTR_LS_MATCH			0x0002
#define	INTR_PARITY_ERR			0x0001

/*
*	Multibit Field Values
*	These values are shifted to match their position in the
*	registers.
*/
#define	MATCH_LS_ANY			0
#define	MATCH_LS_QLS			(8 << 11)
#define	MATCH_LS_MLS			(4 << 11)
#define	MATCH_LS_HLS			(2 << 11)
#define	MATCH_LS_ILS			(1 << 11)

#define	MAINT_LS_ILS			(1 << 8)
#define	MAINT_LS_HLS			(2 << 8)
#define	MAINT_LS_MLS			(3 << 8)
#define	MAINT_LS_QLS			(4 << 8)/* 0 doesn't change the LS */
#define	MAINT_LS_PDR			(6 << 8)

#define	PC_LOOP_NO_LCT			0
#define	PC_LOOP_PDR_PDU			(1 << 5)
#define	PC_LOOP_IDLE			(2 << 5)
#define	PC_LOOP_PDR_LOOPBACK	(3 << 5)

#define	PCM_CNTRL_NONE			0
#define	PCM_CNTRL_BREAK			1
#define	PCM_CNTRL_TRACE			2
#define	PCM_CNTRL_OFF			3

#define	PREV_LINE_ST_QLS		0
#define	PREV_LINE_ST_MLS		(1 << 8)
#define	PREV_LINE_ST_HLS		(2 << 8)
#define	PREV_LINE_ST_ILS		(3 << 8)/* 16 Idle symbols */

#define	LINE_ST_NLS				0
#define	LINE_ST_ALS				(1 << 5)
#define	LINE_ST_ILS4			(3 << 5)
#define	LINE_ST_QLS				(4 << 5)
#define	LINE_ST_MLS				(5 << 5)
#define	LINE_ST_HLS				(6 << 5)
#define	LINE_ST_ILS16			(7 << 5)

#define	RF_STATE_REPEAT			0
#define	RF_STATE_IDLE			(1 << 14)
#define	RF_STATE_HALT1			(2 << 14)
#define	RF_STATE_HALT2			(3 << 14)

#define	PCI_STATE_REMOVED		0
#define	PCI_STATE_INSERT_SCRUB	(1 << 12)
#define	PCI_STATE_REMOVE_SCRUB	(2 << 12)
#define	PCI_STATE_INSERTED		(3 << 12)

#define	PCM_STATE_OFF			(PC_OFF << 7)
#define	PCM_STATE_BREAK			(PC_BREAK << 7)
#define	PCM_STATE_TRACE			(PC_TRACE << 7)
#define	PCM_STATE_CONNECT		(PC_CONNECT << 7)
#define	PCM_STATE_NEXT			(PC_NEXT << 7)
#define	PCM_STATE_SIGNAL		(PC_SIGNAL << 7)
#define	PCM_STATE_JOIN			(PC_JOIN << 7)
#define	PCM_STATE_VERIFY		(PC_VERIFY << 7)
#define	PCM_STATE_ACTIVE		(PC_ACTIVE << 7)
#define	PCM_STATE_MAINT			(PC_MAINT << 7)

#define	BREAK_NONE				0
#define	BREAK_PC_START			1
#define	BREAK_T_OUT				2
#define	BREAK_NS_MAX			3
#define	BREAK_QLS				4
#define	BREAK_ILS				5
#define	BREAK_HLS				6

/*
*	Timer Register Macros
*	The macros convert microsecond time values into TPC and
*	TNE register values.
*/
#define	TPC_VALUE(usecs)		-((((usecs) * 100) / 2048) + 1)
#define	TNE_VALUE(usecs)		-((((usecs) * 100) / 32) + 1)

#endif __HW_PHY_H__
