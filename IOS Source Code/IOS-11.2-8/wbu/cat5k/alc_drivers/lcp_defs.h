/* $Id: lcp_defs.h,v 1.1.4.1 1996/05/09 14:54:54 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/lcp_defs.h,v $
 *-----------------------------------------------------------------
 * lcp_defs.h -- LCP driver internal definitions.
 *
 * March 15, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:54  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:21  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.3  1996/04/11  05:31:23  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.2  1996/04/11  01:50:19  sakumar
 * Header file changes to support LEC creation via VTP messages.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:07  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:25  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:20  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:49  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.20  1995/10/20  05:04:47  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 * Revision 1.19  1995/09/08  18:32:33  rbau
 * added PMD type in LCP_DriverInfo structure.
 *
 * Revision 1.18  1995/09/07  12:09:04  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.17  1995/07/26  04:15:42  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.16  1995/07/26  03:10:28  rlowe
 * Changes to pass certain Set Port State messages to LANE for spanning
 * tree support.
 *
 * Revision 1.15  1995/06/29  17:29:29  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.14  1995/06/20  08:31:34  rlowe
 * Add new APIs for own (ACP) internal IP address, SNMP ATM interface
 * start index, SNMP ATM interface index range.
 *
 * Revision 1.13  1995/06/15  00:42:34  rlowe
 * Allow the FCC-LCP prom code to use the ACP/LCP serial EEPROM MAC
 * address for LEC.  Stuff it into the HW IDB.  This is a workaround
 * for the FCC-LCP prom not being able to exchange MACs with the NMP.
 * The normal/default MAC used with the NMP, 0:2:3:4:5:6, is still in
 * effect (ie. for IPC).
 *
 * Revision 1.12  1995/06/13  23:58:10  rlowe
 * Change startup-sequence to include ATM instance message from NMP.
 *
 * Revision 1.11  1995/06/13  23:33:41  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 * Also include some new SCP messages for FDDI compatibility.
 *
 * Revision 1.10  1995/06/09  20:41:43  rlowe
 * Allow LCP handshakes to be correct for appropriate setting of
 * ACP_REV0_LCP switch.
 *
 * Revision 1.9  1995/06/09  08:17:56  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 * Set local default MAC as 0:2:3:4:5:6 for testing.
 *
 * Revision 1.8  1995/05/19  21:51:00  rlowe
 * Add LCP_Ntwk_MacAddr to provide network-side MAC address block for
 * atm driver/lane startup.  Also futz around current syn_common instability.
 *
 * Revision 1.7  1995/05/13  22:39:25  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.6  1995/05/04  07:20:20  rlowe
 * Add new APIs LCP_NMP_PhysSlotNbr and LCP_NMP_MacAddr.
 *
 * Revision 1.5  1995/04/27  01:52:40  rlowe
 * Cleanup LCP_ReadMacAddr, and LCP_PhysSlotNbr; added LCP_LogicalCardNbr.
 * Now provide a default MAC address (and logical card number) if LCP EEPROM
 * is not configured.
 *
 * Revision 1.4  1995/04/03  04:27:37  rlowe
 * Sync up code, including LCP support, for IOS 10.3 merge.
 *
 * Revision 1.3  1995/03/29  08:16:47  rlowe
 * Archive current LCP driver as integration continues.
 *
 * Revision 1.2  1995/03/24  06:28:08  rlowe
 * For LCP driver init to sync on Reset Ack message.  Allow mechanism for
 * deferred/late sync.  Provide APIs to request slot number/MAC address
 * from LCP driver, as reported by LCP at init time.  Provide utility
 * routine to format LCP message headers.
 *
 * Revision 1.1  1995/03/15  10:38:50  rlowe
 * Created LCP driver.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _LCP_DEFS_H
#define _LCP_DEFS_H

#ifndef	__syn_ser_eeprom_H__
#include "syn_ser_eeprom.h"
#endif

#ifndef	_SCP_MSGS_H
#include "scp_msgs.h"
#endif

#ifndef	_SCP_DEFS_H
#include "scp_defs.h"
#endif

#ifndef	_LCP_IF_H
#include "lcp_if.h"
#endif

#ifndef	__ACP_HW_DEFS_H
#include "acp_hw_defs.h"
#endif

/* --------------------------------------------	*/
/*	Some nice constants, NOT part of API.	*/
/* --------------------------------------------	*/

#define	LCP_NBR_DPR_BUFS	((ACP_HW_LCP_RAM_SIZE) / LCP_DPR_BUF_SIZ)
								/* 004 */
#define	LCP_DPR_BUF_OFST(n)	(((n) % LCP_NBR_DPR_BUFS) * LCP_DPR_BUF_SIZ)
#define	LCP_DPR_BUF_ADDR(n)	((volatile u_int8 *)		\
				 ((LCP_DPR_BUF_OFST (n))	\
				 + (ACP_HW_LCP_RAM_BASE)))
#define	LCP_DPR_BUF_FR_ACP	(LCP_DPR_BUF_ADDR (0))		/* 28000000 */
#define	LCP_DPR_BUF_TO_ACP	(LCP_DPR_BUF_ADDR (2))		/* 28000200 */

#define	LCP_PKT_TX_TIME		(50)	/* LCP millisecs used per Tx packet. */
#define	LCP_DVR_TX_TOUT		(4 * LCP_PKT_TX_TIME)
					/* Driver allows up to 4 Tx packet */
					/* periods as a HW sanity timeout. */
#define	LCP_STD_TX_TOUT		(LCP_DVR_TX_TOUT / MFP_STD_10MS_TICK)
					/* HW timer is in units of 10 msec. */

#define	LCP_NMP_PHYS_SLOT	(1)	/* Standard slot number for NMP. */
#define	LCP_SLOTNUM(phys)	(((phys) - 1) & 0x0f)
					/* Chg 1-based to 0-based; 4 bits. */

#define	LCP_DVRINIT_NONE	(0x00)		/* Driver init states.   */
#define	LCP_DVRINIT_ONE		(0x10)		/* -- set these vals on  */
#define	LCP_DVRINIT_TWO		(0x20)		/*    init code entry.   */
#define	LCP_DVRINIT_THREE	(0x30)
#define	LCP_DVRINIT_FOUR	(0x40)
#define	LCP_DVRINIT_FIVE	(0x50)
#define	LCP_DVRINIT_DONE	(0x60)		/* All driver init done. */
#define	LCP_DVRINIT_XITFLG	(0x01)		/* -- set LSB on exit.   */

#ifdef	ACP_REV0_LCP
#define	LCP_INTR_INACTIVE_	(OFF)		/* Interrupt pulses to   */
#define	LCP_INTR_ACTIVE_	(ON)		/*   LCP are active low. */
#else
#define	LCP_INTR_INACTIVE_	(OFF_)		/* Interrupt pulses to   */
#define	LCP_INTR_ACTIVE_	(ON_)		/*   LCP are active low. */
#endif

/* --------------------------------------------	*/
/*	LCP/SCP Message types.			*/
/* --------------------------------------------	*/

#ifdef	ACP_REV0_LCP			/* legacy stuff. */
#define	LCP_MSG_SCP_PING	(0x03)	/* Crib LCP message types. */
#define	LCP_MSG_RESET_ACK_TO_MP	(0x08)
#define	LCP_MSG_OPCODE_OFST	(3)	/* Match old LCP "fcc" test prom. */
#define	LCP_MSG_MACADDR_OFST	(0x10)	/* Old format Reset Ack MAC Addr. */
#else					/* Crib LCP message types. */
#define	LCP_MSG_OPCODE_OFST	(SCP_HDR_OPCODE_OFFSET)		/* 0x04 */
#define	LCP_MSG_LCP_READY	(LCP_READY)			/* 0x04 */
#define	LCP_MSG_MODULE_READY	(MODULE_READY)			/* 0x05 */
#define	LCP_MSG_SCP_PING	(SCP_PING)			/* 0x11 */
#define	LCP_MSG_CLR_COUNTERS	(SCP_CLR_COUNTERS)		/* 0x1a */
#define	LCP_MSG_DIAG_INFO	(SCP_GET_DIAG_STAT)		/* 0x1b */
#define	LCP_MSG_MODULE_ENABLE	(SCP_ENADIS_MODPORT)		/* 0x1d */
#define	LCP_MSG_STATUS_POLL	(SCP_GET_STATUS)		/* 0x1e */
#define	LCP_MSG_NMP_MAC_IP_ADDR	(SCP_SUPERVISOR_MAC_IP_ADDR)	/* 0x1f */
#define	LCP_MSG_SET_PORT_STATE	(SCP_SET_PORT_STATE)		/* 0x2b */
#define	LCP_MSG_SET_PORT_FEATR	(SCP_SET_PORT_FEATURES)		/* 0x2c */
#define	LCP_MSG_ATM_INSTANCE	(SCP_SET_ATM_INSTANCE)		/* 0x2f */
#define	LCP_MSG_SET_CDP_MATCH_ADDR (SET_CDP_MATCH_ADDR_CMD)	/* 0x37 */
#define	LCP_MSG_SET_VLAN_PARAMETERS (SCP_SET_VLAN_PARAMETERS)	/* 0x3C */
#endif

typedef	SCPMSG				tLCPm_SCPMSG ;
typedef	RESET_ACK			tLCPm_RESET_ACK ;
typedef	SET_PORT_FEATURES		tLCPm_SET_PORT_FEATURES ;
typedef	ENABLE_DISABLE_MSG		tLCPm_ENABLE_DISABLE_MSG ;
typedef	SET_PORT_STATE			tLCPm_SET_PORT_STATE ;
typedef	SET_PORT_STATE_RESP		tLCPm_SET_PORT_STATE_RESP ;
typedef	SET_VLAN_PARAMETERS_CMD		tLCPm_SET_VLAN_PARAMETERS_CMD ;
typedef	SET_VLAN_PARAMETERS_RESP	tLCPm_SET_VLAN_PARAMETERS_RESP ;
typedef	SUPV_MAC_IP_ADDR		tLCPm_NMP_MAC_IP_ADDR ;
typedef	GETSTAT				tLCPm_GETSTAT ;
typedef	SET_CDP_MATCH_ADDR		tLCPm_CDP_MATCH_ADDR_CMD ;
typedef	SET_CDP_MATCH_ADDR_RESP		tLCPm_CDP_MATCH_ADDR_RESP ;
typedef	SAINTCLR			tLCPm_CLR_COUNTERS ;
typedef	ATM_INSTANCE_MSG		tLCPm_ATM_INSTANCE ;

/* --------------------------------------------	*/
/*	Driver view of LCP message buffers.	*/
/* --------------------------------------------	*/

typedef union LCP_DvrBufU
{
	u_int8				AL [LCP_MALLOC_SIZ] ;	/* align */
	u_int8				BF [LCP_MAX_MSG_SIZ] ;
	tLCP_MsgHdr			MH ;
	tLCP_MsgBufU			BU ;
	tLCPm_SCPMSG			LM ;
	tLCPm_RESET_ACK			RA ;
	tLCPm_ENABLE_DISABLE_MSG	ED ;
	tLCPm_SET_PORT_FEATURES 	PF ;
	tLCPm_SET_PORT_STATE		PS ;
	tLCPm_SET_PORT_STATE_RESP	PR ;
	tLCPm_SET_VLAN_PARAMETERS_CMD   VC ;
	tLCPm_SET_VLAN_PARAMETERS_RESP  VR ;
	tLCPm_NMP_MAC_IP_ADDR		NA ;
	tLCPm_GETSTAT			GS ;
	tLCPm_CDP_MATCH_ADDR_CMD	CM ;
	tLCPm_CDP_MATCH_ADDR_RESP	MR ;
	tLCPm_CLR_COUNTERS		CC ;
	tLCPm_ATM_INSTANCE		AI ;
} tLCP_DvrBufU ;

/* ----------------------------	*/
/*	Driver state info.	*/
/* ----------------------------	*/

typedef struct LCP_MACInfo
{
	u_int32		MsgCount ;	/* Nbr all msgs posted/received. */
	u_int32		AckCount ;	/* Nbr ack interrupts made/seen. */
	u_int32		DrpCount ;	/* Nbr rx/tx msgs dropped.       */
	u_int32		ErrCount ;	/* Nbr rx errors or tx timeouts. */
	volatile u_int8 *pIntr2LCP ;	/* Ptr to HW to cause LCP intrs. */
} tLCP_Counters ;

#define	TxTimeoutCount	ErrCount
#define	pRxDoneIntr	pIntr2LCP
#define	pTxPostIntr	pIntr2LCP

typedef struct LCP_DriverInfo
{
	u_int8		fInitialz ;	/* Driver was init.*/
	u_int8		MySlotNbr ;	/* Own phys slot nbr, 1-based. */
	u_int8		fTxPosted ;	/* Tx ack pending. */
	u_int8		fTxTimeout;	/* Tx timed out.   */
	u_int8		fDbgPrts  ;	/* Allow debug prints. */
	u_int8		fKernel   ;	/* Driver in kernel (vs. boot) mode. */
	u_int8		ModStat   ;	/* Module status for NMP Status Poll. */
	u_int8		PortStat  ;	/* Port status for NMP Status Poll. */
	u_int32		HWTOutVal ;	/* Tx HW tmr period*/
	tLCP_EEpromData* pLcpEEprom ;	/* EEprom data from LCP Reset Ack. */
	u_int16		MyInstNbr ;	/* Own card instance nbr, 1-based. */
	u_int16     PmdType ;   /* PMD type number, 1-based. */
	tLCP_IPAddrU	NMP_IP    ;	/* NMP internal IP address. */
	tLCP_IPAddrU	Own_IP    ;	/* ACP internal IP address. */
	tLCP_MacAddrU	NMP_Mac   ;	/* NMP MAC address. */
	tLCP_MacAddrU	NtwkMac   ;	/* Network MAC addr block from NMP. */
	u_int32		NbrNtwkMacs ;	/* Nbr of network MACs in block. */
	u_int32		IF_Start  ;	/* Starting (SNMP) I/F index value. */
	u_int32		IF_Range  ;	/* Size of (SNMP) I/F index block.  */
	tLCP_Counters	Rx ;		/* Rx driver info. */
	tLCP_Counters	Tx ;		/* Tx driver info. */
	tLCP_DvrBufU	ModulRdy  ;	/* Module Ready from LCP startup. */
	tLCP_DvrBufU	RxHoldBuf ;	/* Used if no Rx buf. */
	tLCP_DvrBufU	TxHoldBuf ;	/* Used if no Rx buf. */
	/* RKL:  Queue management pointers ?   */
	/* RKL:  Tx recovery timer management? */
	/* RKL:  SAP registry table ? */
} tLCP_DriverInfo ;

#ifdef	ACP_REV0_LCP
#define	ResetAck	ModulRdy
#endif

extern volatile tLCP_DriverInfo	LDI ;


	/* Function prototypes for routines supporting LANE and VTP. */
extern u_int8 LCPl_LanePortStateInd (tLCP_DvrBufU * pInd) ;
extern u_int8 LCPl_VTP_VlanParamCmd (tLCP_DvrBufU * pInd) ;


#endif	/* _LCP_DEFS_H */
