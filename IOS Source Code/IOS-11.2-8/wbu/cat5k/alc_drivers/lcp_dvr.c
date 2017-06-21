/* $Id: lcp_dvr.c,v 1.1.4.1 1996/05/09 14:54:54 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/lcp_dvr.c,v $
 *------------------------------------------------------------------
 * lcp_dvr.c -- ATM line card LCP (8051) driver.
 *
 * March 15, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_dvr.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:54  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.2  1996/05/02  06:46:29  rlowe
 * Tighten LCP handshake pulse to a more reasonable width (3.2 usec
 * typical use from ACP DRAM, 1.4 usec best case from I-cache) -- down
 * from approx 200 usec.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.40.1  1996/04/27  06:14:22  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.3  1996/04/26  05:51:21  rlowe
 * Treat compiler warnings as errors.  Eliminate such warnings from
 * current ACP source.  Revise LCP handshake pulse width to be closer
 * to 1.0 usec.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.2  1996/04/11  05:31:24  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:26  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:21  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:50  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.47.6.1.2.2  1996/03/05  02:32:23  rlowe
 * Fold in fixes from mfg_repair_branch.
 *
 * Revision 1.47.6.1.2.1  1996/02/29  07:00:13  rlowe
 * Changes needed for IOS-side sync to Arkansas V111_0_16
 * (Synalc3_Cal_V111_0_16_branch).
 *
 * Revision 1.47.6.1  1996/01/18  05:02:20  rlowe
 * Manufacturability changes to bootrom and flash boot code.
 *
 * Branch: ATM21_branch
 *
 * Revision 1.47  1995/10/25  00:16:38  rlowe
 * Fix to allow LCP driver to work with older Boots that do NOT release
 * LCP reset upon 68k startup.  Remain compatible with Kernel LCP driver
 * init, ie. after Boot-2 has done some LCP messaging.
 *
 * Revision 1.46  1995/10/20  05:04:47  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 * Revision 1.45  1995/10/17  05:47:53  rlowe
 * Fix unterminated comment that mangled an if statement in LCP_DriverInit_1.
 *
 * Revision 1.44  1995/10/17  01:27:26  rbau
 * link led is the bottom one(Not in the middle)
 *
 * Revision 1.43  1995/10/14  03:35:12  rlowe
 * Mark Restart-Ack/Module Ready SCP indication to NMP when doing PROM Boot
 * downloads to flash.  Flag that a PROM-based download is occurring, so
 * no ALC system diags are possible.
 *
 * Revision 1.42  1995/10/13  06:33:19  rlowe
 * Cleaned up some ifdefs.
 *
 * Revision 1.41  1995/10/13  06:06:51  rlowe
 * Enable and use BCS checksums in PROM and flash Boots.
 *
 * Revision 1.40  1995/10/10  01:01:41  rbau
 * return card led status to NMP polling
 *
 * Revision 1.39  1995/09/27  18:19:38  rbau
 * allow API function LCP_NMP_MacAddr to return NMP MAC before LCP driver
 * initialization completes.
 *
 * Revision 1.38  1995/09/09  04:38:05  rlowe
 * Take care of case where Rx LCP packets may have shown up between
 * Boot-2 and Kernel; have final LCP driver init step check for active
 * I/O line indicating pending Rx message.
 *
 * Revision 1.37  1995/09/08  21:00:38  rbau
 * use wrapper function to set PMD type in LCP driver
 *
 * Revision 1.36  1995/09/08  18:33:46  rbau
 * added support to read PMD type
 *
 * Revision 1.35  1995/09/07  12:09:05  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.34  1995/08/31  12:17:42  rlowe
 * Changes to work with IOS 11.0 and higher.
 *
 * Revision 1.33  1995/08/22  23:58:35  sakumar
 * For now, enabled IFDEF'd code for NMP IP address to be 127.0.0.2. Actually
 * the NMP should provide this.
 *
 * Revision 1.32  1995/08/09  02:32:21  rlowe
 * Allow override of NMP-supplied internal IP address, while
 * retaining mechanism of deriving ACP internal IP from NMP.
 *
 * Revision 1.31  1995/08/01  00:33:05  rbau
 * Fix to put extra diag results into exact location in SCP Module Ready
 * (Restart Ack) message to be sent on to NMP.
 *
 * Revision 1.30  1995/07/26  04:38:28  rlowe
 * Perform LANE upcall for Set Port State (spanning tree) messages.
 * Only do this if the LCP driver finished initialization AND
 * was setup for Kernel use (upcall NOT applicable to Boot use).
 *
 * Revision 1.29  1995/07/26  04:15:45  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.28  1995/07/13  21:29:40  sakumar
 * Modify Set Port Features to handle port enable as well as port unreset.
 * By RKL.
 *
 * Revision 1.27  1995/06/29  17:29:32  rlowe
 * Integrate LCP driver to IOS (kernel/run-time).  Setup proper NMP
 * Status Poll responses.  Clean-up LCP debug prints (on for Boot, off
 * for Kernel).
 *
 * Revision 1.26  1995/06/23  21:03:51  rlowe
 * Force instance number to be zero for old LCP case, and real value
 * (from NMP) in current LCP case.  Assume that NMP is now sending
 * us its proper internal IP address, and we can derive ours from
 * that one.  Clarify how ATM/LEC MACs are assigned.  Old case, from
 * EEPROM MAC.  Current case, from NMP.
 *
 * Revision 1.25  1995/06/23  20:29:49  rlowe
 * Use real ALC eeprom MAC instead of canned 0:2:3:4:5:6 MAC.
 * Setup own (internal) IP address until NMP code gives us its
 * internal IP address.
 *
 * Revision 1.24  1995/06/20  20:42:36  rlowe
 * Fix SCP header addressing (SAP) bugs.  Force NMP_CONFIG_SAP for Module
 * Ready message dest SAP; use (NMP) source SAP as dest SAP for all other
 * response messages.  Always LCP code driver SAPs in source SAP field.
 *
 * Revision 1.23  1995/06/20  08:31:36  rlowe
 * Add new APIs for own (ACP) internal IP address, SNMP ATM interface
 * start index, SNMP ATM interface index range.
 *
 * Revision 1.22  1995/06/15  09:25:52  rlowe
 * Move boot download functionality to boot-1.  Condition PMON to SARs
 * on MFG jumper instead of DBG jumper.  Condition boot download on
 * SPARE jumper until checksums are done.  Cleanup makefiles.
 * Lock s-bus when resetting BIGA/SARs to avoid hung frame buffer.
 * Explicitly hold/release reset on BIGA.  Force LCP reset whenever
 * LCP driver re-initialized.
 *
 * Revision 1.21  1995/06/15  00:42:47  rlowe
 * Allow the FCC-LCP prom code to use the ACP/LCP serial EEPROM MAC
 * address for LEC.  Stuff it into the HW IDB.  This is a workaround
 * for the FCC-LCP prom not being able to exchange MACs with the NMP.
 * The normal/default MAC used with the NMP, 0:2:3:4:5:6, is still in
 * effect (ie. for IPC).
 *
 * Revision 1.20  1995/06/13  23:58:17  rlowe
 * Change startup-sequence to include ATM instance message from NMP.
 *
 * Revision 1.19  1995/06/13  23:33:56  rlowe
 * SYNALC builds use "buginf" instead of "printf".
 * Also include some new SCP messages for FDDI compatibility.
 *
 * Revision 1.18  1995/06/09  10:46:31  rlowe
 * If statement needed parentheses around test expression.
 *
 * Revision 1.17  1995/06/09  08:17:58  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 * Set local default MAC as 0:2:3:4:5:6 for testing.
 *
 * Revision 1.16  1995/06/04  02:31:41  rlowe
 * Set (temporary) default MAC address to 0:2:3:4:5:6 instead of 1:2:3:4:5:6.
 * (ie. no multicast).
 *
 * Revision 1.15  1995/05/19  21:51:12  rlowe
 * Add LCP_Ntwk_MacAddr to provide network-side MAC address block for
 * atm driver/lane startup.  Also futz around current syn_common instability.
 *
 * Revision 1.14  1995/05/18  23:47:56  rlowe
 * Revisited actions when driver timeouts detected.  Can reboot board or
 * just loop forever (hang).  Try to put out informative debug prints.
 *
 * Revision 1.13  1995/05/13  22:39:26  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.12  1995/05/04  10:38:49  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.11  1995/05/04  10:38:31  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.10  1995/05/04  07:20:21  rlowe
 * Add new APIs LCP_NMP_PhysSlotNbr and LCP_NMP_MacAddr.
 *
 * Revision 1.9  1995/05/03  04:26:56  rlowe
 * Rev.1 and Rev.2 LCPs are now using the same address map, etc.  So now
 * should be able to use same LCP driver on any board.
 *
 * Revision 1.8  1995/04/29  08:25:27  rlowe
 * Fix MFP_ClrPendIntr calls to NOT require clearing MFP InService bits;
 * they are currently unused and this will be a touch more efficient.
 *
 * Revision 1.7  1995/04/27  02:45:13  rlowe
 * Support latest LCP handshake functionality, whereby interrupt status lines
 * TO ACP are held active until LCP gets responding handshake.  This is
 * (initially) only available on Rev.2 ATM line cards (Rev.1 use non-std
 * LCP boot code).  MFP driver now provides MFP I/O line polling routine
 * in support of this (MFP interrupt sense is based on I/O pin transitions
 * and may be missed).
 *
 * Revision 1.6  1995/04/27  01:52:41  rlowe
 * Cleanup LCP_ReadMacAddr, and LCP_PhysSlotNbr; added LCP_LogicalCardNbr.
 * Now provide a default MAC address (and logical card number) if LCP EEPROM
 * is not configured.
 *
 * Revision 1.5  1995/04/03  04:27:41  rlowe
 * Sync up code, including LCP support, for IOS 10.3 merge.
 *
 * Revision 1.4  1995/03/29  08:16:47  rlowe
 * Archive current LCP driver as integration continues.
 *
 * Revision 1.3  1995/03/24  06:28:07  rlowe
 * For LCP driver init to sync on Reset Ack message.  Allow mechanism for
 * deferred/late sync.  Provide APIs to request slot number/MAC address
 * from LCP driver, as reported by LCP at init time.  Provide utility
 * routine to format LCP message headers.
 *
 * Revision 1.2  1995/03/15  11:32:41  rlowe
 * Have LCP driver clear interrupt pending status from MFP when in polling mode.
 *
 * Revision 1.1  1995/03/15  10:38:50  rlowe
 * Created LCP driver.
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "lo_memblks.h"
#include "lo_error.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "mfp_hw_defs.h"
#include "mfp_acp_defs.h"
#include "syn_ser_eeprom.h"
#include "scp_defs.h"
#include "scp_msgs.h"
#include "lcp_if.h"
#include "lcp_defs.h"


#define	LCP_MFP_BITMASK	(MFP_IMSK_LCPRD | MFP_IMSK_LCPTP)

extern void	A_LCP_Rx_Isr () ;		/* Assembly ISR. */
extern void	A_LCP_Tx_Isr () ;		/* Assembly ISR. */

extern u_int16	MFP_HwtPollChk () ;		/* Polling-mode HW timeouts. */


	/* Default/test local MAC address. */
const tLCP_MacAddrU LCP_LclMacAdrDflt =	{ 0x0002, 0x0304, 0x0506, } ;

	/* Driver global data structure. */
volatile tLCP_DriverInfo	LDI ;


/* --------------------------------------------------------------------	*/
/*    Debug trace log support for LCP driver.				*/
/* --------------------------------------------------------------------	*/
#define       LTB_RX_FLAG     (0)
#define       LTB_TX_FLAG     (1)
#define       LTB_NBR_ITEMS   (128)                   /* Uses > 32 K. */

typedef struct LTB_Item
{
	u_int8          T_Valid ;               /* Valid trace entry */
	u_int8          T_fTx ;                 /* Tx (1) or Rx (0) */
	u_int32         T_SeqNbr ;              /* Sequence nbr. */
	u_int32         T_Time ;                /* Whole time (sw) */
	u_int16         T_PTime ;               /* Partial time (hw) */
	tLCP_DvrBufU    T_Msg ;                 /* Real message trace buf */
} tLTB_Item ;

typedef struct LTB_G
{
        tLTB_Item *     pFirst ;
        tLTB_Item *     pLast ;
        tLTB_Item *     pCur ;
        u_int32         M_Count ;
        tLTB_Item       LogTbl [LTB_NBR_ITEMS] ;
} tLTB_G ;

tLTB_G                LTB_Glob ;              /* Real thing. */

/* --------------------------------------------------------------------	*/
/*    Initialize trace log data structures.				*/
/* --------------------------------------------------------------------	*/
void LCP_InitTrace ()
{
        register tLTB_G *pT = & LTB_Glob ;
 
 
        (void) A_BlockFill ((u_int8 *) pT, sizeof (* pT), 0, AR_SvcWatchdog) ;

        pT->pFirst  = & pT->LogTbl [0] ;
        pT->pLast   = & pT->LogTbl [LTB_NBR_ITEMS - 1] ;
        pT->pCur    = pT->pFirst ;
        pT->M_Count = 0 ;

        buginf ("LCP trace structure @ $%08x\n", pT) ;

}     /* LCP_InitTrace */

/* --------------------------------------------------------------------	*/
/*    Copy a message (buffer) to the trace log.				*/
/*    This code may need tuning if logging can occur outside of ISRs.	*/
/* --------------------------------------------------------------------	*/
void LCP_CopyTrace (u_int8 fTx, tLCP_DvrBufU * pMsg)
{
        register tLTB_G    *pT = & LTB_Glob ;
        register tLTB_Item *pR ;


        pR = pT->pCur ;
        ++ (pT->M_Count) ;

        pR->T_Valid  = TRUE ;
        pR->T_fTx    = fTx ;
        pR->T_SeqNbr = pT->M_Count ;
        pR->T_Time   = pT->M_Count ;    /* TBD */
        pR->T_PTime  = 0 ;              /* TBD */

        (void) A_BlockCopy ((u_int8 *) pMsg, pR->T_Msg.BF,
                                pMsg->MH.length + LCP_MSGHDR_SIZ,
                                AR_SvcWatchdog) ;

        if (++ pR > pT->pLast)
                pR = pT->pFirst ;

        pT->pCur = pR ;

}     /* LCP_CopyTrace */

/* --------------------------------------------------------------------	*/
/*    Dump the message trace log.					*/
/*    This code may need tuning if dumping can occur from kernel.	*/
/* --------------------------------------------------------------------	*/
void LCP_DumpTrace ()
{
        register tLTB_G    *pT = & LTB_Glob ;
        register tLTB_Item *pR ;
        register tLTB_Item *pE ;


        pE = pT->pCur ;
        buginf ("\nLCP Msg Log Dump:  %d msgs traced\n\n", pT->M_Count) ;

        pR = pE ;
        do {
                if (-- pR < pT->pFirst)
                        pR = pT->pLast ;

                if (pR->T_Valid)
                {
                        buginf (
"%4d  %4d.%d  %s  %02x-%02x-%02x-%02x-%02x  %02x  %02x %02x %02x %02x %02x %02x %02x %02x %02x %02x\n",
                                pR->T_SeqNbr, pR->T_Time, pR->T_PTime,
                                (pR->T_fTx ? "T " : " R"),
                                pR->T_Msg.BF [ 0], pR->T_Msg.BF [ 1],
                                pR->T_Msg.BF [ 2], pR->T_Msg.BF [ 3],
                                pR->T_Msg.BF [ 4], pR->T_Msg.BF [ 5],
                                pR->T_Msg.BF [ 6], pR->T_Msg.BF [ 7],
                                pR->T_Msg.BF [ 8], pR->T_Msg.BF [ 9],
                                pR->T_Msg.BF [10], pR->T_Msg.BF [11],
                                pR->T_Msg.BF [12], pR->T_Msg.BF [13],
                                pR->T_Msg.BF [14], pR->T_Msg.BF [15]) ;
                }

        } while (pR != pE) ;

        buginf ("\nLCP Log Dump Complete\n\n") ;

}     /* LCP_DumpTrace */


/* --------------------------------------------------------------------	*/
/*	This routine generates the appropriate HW-level handshakes	*/
/*	(interrupts) to the LCP.  (Tx posted, Rx done)			*/
/*	It trusts that the caller has presented valid arguments.	*/
/* --------------------------------------------------------------------	*/
void LCP_IntrHandshake (register volatile u_int8 * pReg)
{
	register s_int8	 Buzz ;		/* No need to access data cache. */
/* RKL:  is this a critical section ? */

		/* Be careful whether interrupt to LCP generated   */
		/* on rising or falling edge of handshake signal.  */
		/* Also need to be sure that pulse has appropriate */
		/* width, even if we are running fast from cache.  */

		/* LCP handshake pulse width should be >= 1.0 usec. */

		/* Using a loop constant of "2":                     */
		/* The buzz loops have been checked out with the ACP */
		/* (68ec030 @ 25 MHz) with I/D caches enabled.       */
		/* This code has been tested to give 1.0 usec pulse  */
		/* even with ALL instruction fetches from cache.     */
		/* In a more normal environment, ie. caches enabled  */
		/* but no cache hits, and running from DRAM, we get  */
		/* a 2.6 usec pulse on the ACP.                      */

		/* We use a loop constant of "3" to provide a safety */
		/* margin:  this gives 1.4 usec for all cache hits,  */
		/* 3.2 usec more typical DRAM usage.                 */

	*pReg = LCP_INTR_INACTIVE_ ;
	for (Buzz = 3 ; Buzz > 0 ; -- Buzz) { }

	*pReg = LCP_INTR_ACTIVE_   ;
	for (Buzz = 3 ; Buzz > 0 ; -- Buzz) { }

	*pReg = LCP_INTR_INACTIVE_ ;

}	/* LCP_IntrHandshake */


/* --------------------------------------------------------------------	*/
/*	This routine copies data from the LCP DPRAM to a buffer.	*/
/*	It trusts that the caller has presented valid arguments.	*/
/* --------------------------------------------------------------------	*/
u_int8 * LCP_ReadDPR (volatile tLCP_DriverInfo * pDI, u_int8 * pBuf)
{
	(void) A_BlockCopy ((u_int8 *) (LCP_DPR_BUF_TO_ACP), pBuf,
				LCP_MAX_MSG_SIZ, AR_SvcWatchdog) ;
	++ (pDI->Rx.MsgCount) ;

	LCP_IntrHandshake (pDI->Rx.pRxDoneIntr) ;
	++ (pDI->Rx.AckCount) ;

	return (pBuf) ;

}	/* LCP_ReadDPR */


/* --------------------------------------------------------------------	*/
/*	Routine to cleanup after Rx Posted (from LCP) detected.		*/
/* RKL:	in future need to fix for proper intr level buffer strategy.	*/
/* --------------------------------------------------------------------	*/
void LCP_CleanupRx (register volatile tLCP_DriverInfo * pDI,
				tLCP_MsgBufU * pBuf, u_int8 fPoll)
{
/* RKL:  is this a critical section ? */

	if (pBuf == NULL)
		pBuf = (tLCP_MsgBufU *) & pDI->RxHoldBuf.BU ;

	if (fPoll)
		MFP_ClrPendIntr (MFP_IMSK_LCPTP, FALSE) ;

	(void) LCP_ReadDPR (pDI, pBuf->buffer) ;

	LCP_CopyTrace (LTB_RX_FLAG, (tLCP_DvrBufU *) pBuf) ;

#ifdef	DEBUG
	if (pDI->fDbgPrts)
		buginf ("\r\nLCP_TakeRxPoll:  %x-%x-%02x-%02x-%02x-%02x-%02x\n",
			pBuf->hdr.src_addr, pBuf->hdr.dest_addr,
			pBuf->hdr.length, pBuf->hdr.dsap, pBuf->hdr.ssap,
			pBuf->hdr.opcode, pBuf->msg.mdata [0]) ;
#endif

}	/* LCP_CleanupRx */


/* --------------------------------------------------------------------	*/
/*	Callback routine for handling LCP Tx timeouts.			*/
/*	This may be called from MFP/HW timer interrupt-level.		*/
/* --------------------------------------------------------------------	*/
void LCP_TxTOutCallBack (u_int32 Arg)
{
	LDI.fTxTimeout = TRUE ;

#ifdef	DEBUG
	if (LDI.fDbgPrts)
	{
		buginf ("\r\nLCP timeout detected !\n") ;
	}
#endif

	if (LDI.fKernel)
	{
			/* Kernel mode, board reset for driver HW timeout. */
		MFP_HwtStdTimeout () ;
	}
	else		/* Boot mode, fatal error for driver HW timeout. */
	{
		A_FailLeds (LOER_STD_LED_FLASH, PUD_8051_RAM,
						0xffffffff, AR_SvcWatchdog) ;
	}


}	/* LCP_TxTOutCallBack */


/* --------------------------------------------------------------------	*/
/*	Routine to cleanup after LCP Tx Done or Tx timeout.		*/
/* --------------------------------------------------------------------	*/
void LCP_CleanupTx (register volatile tLCP_DriverInfo * pDI, u_int8 fTOut, u_int8 fPoll)
{
/* RKL:  is this a critical section ? */
	MFP_HwtClrTimer (LCP_TxTOutCallBack) ;

	if (fPoll)
		MFP_ClrPendIntr (MFP_IMSK_LCPRD, FALSE) ;

	pDI->fTxPosted  = FALSE ;
	pDI->fTxTimeout = FALSE ;

		/* Timeouts are a hard error, reset the board ! */
	if (fTOut)
		++ (LDI.Tx.TxTimeoutCount) ;
	else
		++ (LDI.Tx.AckCount) ;

}	/* LCP_CleanupTx */


/* --------------------------------------------------------------------	*/
/*	This routine copies data from a buffer to the LCP DPRAM.	*/
/*	It trusts that the caller has presented valid arguments.	*/
/* --------------------------------------------------------------------	*/
u_int8 * LCP_WriteDPR (register volatile tLCP_DriverInfo * pDI,
						u_int8 * pBuf, u_int8 Len)
{
	u_int32		Old_SR ;


	Old_SR = spl7 () ;

		/* Setup everything BEFORE asserting Tx interrupt to LCP. */
	(void) A_BlockCopy (pBuf, (u_int8 *) (LCP_DPR_BUF_FR_ACP),
						Len, AR_SvcWatchdog) ;

	pDI->fTxPosted = TRUE  ;
	LDI.fTxTimeout = FALSE ;
	++ (pDI->Tx.MsgCount)  ;

		/* Setup recovery timer in case of hung LCP. */
	MFP_HwtSetTimer (LCP_TxTOutCallBack, 0, pDI->HWTOutVal) ;

	LCP_IntrHandshake (pDI->Tx.pTxPostIntr) ;

	splx (Old_SR) ;

	return (pBuf) ;

}	/* LCP_WriteDPR */


/* --------------------------------------------------------------------	*/
/*	Check/poll LCP driver for TxAck handshake.  Used to synchronize	*/
/*		with driver if asynch (non-blocking) transmits used.	*/
/*		A flag indicates whether to poll forever or just once.	*/
/*	Returns:							*/
/*		LCP_RC_Normal	TxAck seen or no Tx pending to LCP.	*/
/*		LCP_RC_Tx_Busy	LCP busy with previous Tx msg.		*/
/*		LCP_RC_Tx_TOut	driver timed out waiting for LCP TxAck.	*/
/*	NB.  This is a POLLING mode routine.				*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_TxBusyPoll2 (volatile tLCP_DriverInfo * pDI, u_int8 fOnce)
{
	s_int32		RC = LCP_RC_Normal ;


		/* Is a Tx to LCP outstanding at this point ? */
	if (! pDI->fTxPosted)
	{
		RC = LCP_RC_Normal ;
	}
		/* Is our Tx to LCP done ?   LCP Tx Done has precedence    */
		/* over driver HW timeouts.  This call may block depending */
		/* on fOnce.  Note that the MFP timeout polling routine    */
		/* returns every HW timeout tick, so must loop here until  */
		/* we get either given driver HW timeout or Tx done.       */
	else  while (TRUE)
	{
			/* Check for Tx done, driver HW timer tick. */
			/* If Tx done, ignore any pending HW touts. */
		if (MFP_HwtPollChk (MFP_IMSK_LCPRD, fOnce))
		{
			LCP_CleanupTx (pDI, FALSE, TRUE) ;
			RC = LCP_RC_Normal ;
			break ;
		}
			/* Timeout flag set by driver HW timer callback fcn. */
		else  if (pDI->fTxTimeout)
		{
			LCP_CleanupTx (pDI, TRUE, TRUE) ;
			RC = LCP_RC_Tx_TOut ;
			break ;
		}
			/* Tx to LCP is still in progress; fOnce is TRUE */
			/* or single HW timer tick occurred above.       */
		else  if (fOnce)
		{
			RC = LCP_RC_Tx_Busy ;
			break ;
		}
	}


	return (RC) ;

}	/* LCP_TxBusyPoll2 */


/* --------------------------------------------------------------------	*/
/*	API:  User entry point for Polling LCP driver for TxAck		*/
/*		handshake.  Used to synchronize with driver if asynch	*/
/*		(non-blocking) transmits used.				*/
/*		A flag indicates whether to poll forever or just once.	*/
/*	Returns:							*/
/*		See LCP_TxBusyPoll2.					*/
/*	NB.  This is a POLLING mode API.				*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_TxBusyPoll (u_int8 fOnce)
{
	return (LCP_TxBusyPoll2 (& LDI, fOnce)) ;

}	/* LCP_TxBusyPoll */


/* --------------------------------------------------------------------	*/
/*	API:  Transmit message through LCP.  No fragmentation is done	*/
/*		for oversize packets, and they are not sent out.	*/
/*		Flags indicate whether to check on Tx-Free or Tx-Ack,	*/
/*		and if polling should be forever or just once.		*/
/*	Returns:							*/
/*		LCP_RC_Normal	msg sent to LCP.			*/
/*		LCP_RC_Bad_Ptr	Bad message buffer pointer.		*/
/*		LCP_RC_Msg_Len	Bad message length field.		*/
/*		LCP_RC_Tx_Busy	LCP busy with previous Tx msg.		*/
/*		LCP_RC_Tx_TOut	driver timed out waiting for LCP TxAck.	*/
/*	NB.  This is a POLLING mode API.				*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_TransmitPoll (u_int8 fChkFree, u_int8 fChkAck, u_int8 fOnce,
							tLCP_MsgBufU * pTx)
{
	volatile tLCP_DriverInfo *	pDI ;
	s_int32				RC ;


	pDI = & LDI ;
	RC  = LCP_RC_Normal ;

	do {
		if (pTx == NULL)
			{ ++ (pDI->Tx.DrpCount) ; RC = LCP_RC_Bad_Ptr ; break; }

		if (pTx->msg.length > LCP_MAX_DATA_SIZ)
			{ ++ (pDI->Tx.DrpCount) ; RC = LCP_RC_Msg_Len ; break; }

			/* Want to wait until LCP Tx is clear ? */
		if (fChkFree)
			if (LCP_TxBusyPoll2 (pDI, fOnce) == LCP_RC_Tx_Busy)
				{ RC = LCP_RC_Tx_Busy ; break ; }

#ifdef	ACP_REV0_LCP
			/* Ensure that message is marked as coming from ACP. */
		pTx->msg.ssap |= (LCP_ACP_SAP_FLAG) ;
#else
		if (pTx->msg.ssap == LCP_LCL_8051_SAP)
			pTx->msg.ssap = LCP_ACP_DRIVER_SAP ;
#endif

#ifdef	DEBUG
	if (pDI->fDbgPrts)
		buginf ("\r\nLCP_TransmitPoll:  %x-%x-%02x-%02x-%02x-%02x-%02x",
			pTx->hdr.src_addr, pTx->hdr.dest_addr, pTx->hdr.length,
			pTx->hdr.dsap, pTx->hdr.ssap, pTx->hdr.opcode,
			pTx->msg.mdata [0]) ;
#endif

		(void) LCP_WriteDPR (pDI, pTx->buffer,
					pTx->msg.length + LCP_MSGHDR_SIZ) ;

		LCP_CopyTrace (LTB_TX_FLAG, (tLCP_DvrBufU *) pTx) ;

			/* Want to wait until Tx is Acked ? */
		if (fChkAck)
			if (LCP_TxBusyPoll2 (pDI, fOnce) == LCP_RC_Tx_TOut)
				{ RC = LCP_RC_Tx_TOut ; break ; }

	} while (FALSE) ;

#ifdef	DEBUG
	if (pDI->fDbgPrts)
		buginf ("\r\nLCP_TransmitPoll RC %d\n", RC) ;
#endif

	return (RC) ;

}	/* LCP_TransmitPoll */


/* --------------------------------------------------------------------	*/
/*	Poll for a message to show up in LCP DPRAM.			*/
/* --------------------------------------------------------------------	*/
u_int8 LCP_PollInpData (u_int16 Offset, u_int8 Value, u_int8 fOnce)
{
	register volatile u_int8 *	p ;
	u_int16				Delay ;
	u_int8				fMatch ;


	p = (u_int8 *)(LCP_DPR_BUF_TO_ACP + Offset) ;
	fMatch = FALSE ;

		/* Wait for indicated byte to show up. */
	do {
		PUNCH_WD ;
		for (Delay = 0 ; Delay < 10000 ; ++ Delay) { }
		fMatch = (*p == Value) ;
	} while ((! fOnce) && (! fMatch)) ;

		/* Wait for rest of message to show up. */
	if (fMatch)
	{
		PUNCH_WD ;
		for (Delay = 0 ; Delay < 10000 ; ++ Delay) { }
	}

	PUNCH_WD ;
	return (fMatch) ;

}	/* LCP_PollInpData */


/* --------------------------------------------------------------------	*/
/*	Provide uniform means of handling LCP Rx Poll completions.	*/
/* --------------------------------------------------------------------	*/
tLCP_MsgBufU * LCP_TakeRxPoll (u_int16 fWhat, tLCP_MsgBufU * pRx,
				volatile tLCP_DriverInfo * pDI)
{
	if (fWhat)
	{
		LCP_CleanupRx (pDI, pRx, TRUE) ;
		++ (pDI->Rx.MsgCount) ;
	}
	else	/* No pending Rx, can only get here */
	{	/* if NOT waiting forever.          */

		pRx = NULL ;
	}

	return (pRx) ;

}	/* LCP_TakeRxPoll */


/* --------------------------------------------------------------------	*/
/*	API:  Receive message from LCP.  User must provide a msg	*/
/*		buffer that can handle a max sized message.		*/
/*		A flag indicates whether to poll forever or just once.	*/
/*	Returns:							*/
/*		ptr	to given buffer if message was read from LCP.	*/
/*		NULL	if no message was read from LCP or pointer bad.	*/
/*	NB.  This is a POLLING mode API.				*/
/* --------------------------------------------------------------------	*/
tLCP_MsgBufU * LCP_ReceivePoll (u_int8 fOnce, tLCP_MsgBufU * pRx)
{
	u_int16	fResult = FALSE ;


	if (pRx != NULL)
	{
		fResult = MFP_PollForIntr (MFP_IMSK_LCPTP, FALSE, fOnce, TRUE) ;

		pRx = LCP_TakeRxPoll (fResult, pRx, & LDI) ;
	}

	return (pRx) ;

}	/* LCP_ReceivePoll */


/* --------------------------------------------------------------------	*/
/*	Receive initial message from LCP.  User must provide a msg	*/
/*		buffer that can handle a max sized message.		*/
/*		A flag indicates whether to poll forever or just once.	*/
/*		This routine is used to check for the FIRST handshake	*/
/*		message from the LCP at startup time.  The standard Rx	*/
/*		polling routine (above) should be used thereafter.	*/
/*	Returns:							*/
/*		ptr	to given buffer if message was read from LCP.	*/
/*		NULL	if no message was read from LCP or pointer bad.	*/
/*	NB.  This is a POLLING mode routine.				*/
/* --------------------------------------------------------------------	*/
tLCP_MsgBufU * LCP_ReceivePoll0 (u_int8 fOnce, tLCP_MsgBufU * pRx)
{
	u_int8	fResult = FALSE ;


	if (pRx != NULL)
	{
#ifdef	ACP_REV0_LCP
		fResult = LCP_PollInpData (LCP_MSG_OPCODE_OFST,
					   LCP_MSG_RESET_ACK_TO_MP, fOnce) ;
#else
		fResult = MFP_Poll_IOs (~ MFP_GPI_LCP_MP_, MFP_GPI_LCP_MP_,
					fOnce, FALSE) ;

			/* LCP activity bits are active low. */
		fResult = ! (fResult & MFP_GPI_LCP_MP_) ;
#endif

		pRx = LCP_TakeRxPoll ((u_int16) fResult, pRx, & LDI) ;
	}

	return (pRx) ;

}	/* LCP_ReceivePoll0 */


/* --------------------------------------------------------------------	*/
/*	API:  Returns size of LCP Driver's info structure.		*/
/* --------------------------------------------------------------------	*/
u_int32 LCP_GetDrvrInfoSiz (void)
{
	return (sizeof (LDI)) ;

}	/* LCP_GetDrvrInfoSiz */


/* --------------------------------------------------------------------	*/
/*	API:  This routine copies back the LCP Driver info structure.	*/
/* --------------------------------------------------------------------	*/
p_void LCP_GetDriverInfo (p_void pBuf)
{
	u_int32		Old_SR ;


	if (pBuf != NULL)
	{
			/* Protect against LCP interrupts during copy. */
		Old_SR = spl7 () ;
		(void) A_BlockCopy ((u_int8 *) & LDI, (u_int8 *) pBuf,
				    sizeof (LDI), AR_SvcWatchdog) ;
		splx (Old_SR) ;
	}

	return (pBuf) ;

}	/* LCP_GetDriverInfo */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns the physical slot number received	*/
/*		during driver initialization (LCP Reset Ack message).	*/
/*		Since slot numbers are 1-based, returns 0 if driver	*/
/*		is not initialized.					*/
/* --------------------------------------------------------------------	*/
u_int8 LCP_PhysSlotNbr (void)
{
		/* Is LCP driver unaware of our slot number ? */
#ifdef	ACP_REV0_LCP
	if (LDI.fInitialz < LCP_DVRINIT_DONE)
#else
	if (LDI.fInitialz <= LCP_DVRINIT_TWO)
#endif
		return (0) ;

		/* Return value from LCP init, should never be zero !! */
	else	return (LDI.MySlotNbr) ;

}	/* LCP_PhysSlotNbr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns the NMP's physical slot number.	*/
/* --------------------------------------------------------------------	*/
u_int8 LCP_NMP_PhysSlotNbr (void)
{
	return (LCP_NMP_PHYS_SLOT) ;

}	/* LCP_PhysSlotNbr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns the card instance number received	*/
/*		during driver initialization (LCP Reset Ack message).	*/
/*		Since card numbers are 1-based, returns 0 if driver	*/
/*		is not initialized.					*/
/* --------------------------------------------------------------------	*/
u_int16 LCP_CardInstanceNbr (void)
{
		/* Is LCP driver NOT fully initialized ? */
	if (LDI.fInitialz < LCP_DVRINIT_DONE)
		return (0) ;

		/* Return value from LCP init. */
	else	return (LDI.MyInstNbr) ;

}	/* LCP_CardInstanceNbr */

/* -------------------------------------------------------------------- */
/* API:  This routine returns the PMD type                              */
/*   returns 0 if PMD eeprom checksum error                             */
/*           else, pmd type                                             */
/* -------------------------------------------------------------------- */
u_int16 LCP_PmdType (void)
{
	/* Is LCP driver still in INIT_1 */
	if (LDI.fInitialz == LCP_DVRINIT_ONE)
		return (0) ;

	else  return (LDI.PmdType) ;

}  /* LCP_PmdType() */

/* -------------------------------------------------------------------- */
/* API:  This routine sets the PMD type                                 */
/*   returns 0 if PMD eeprom checksum error                             */
/*           else, pmd type                                             */
/* -------------------------------------------------------------------- */
void LCP_SetPmdType (u_int16 type)
{
	LDI.PmdType = type; 
}  /* LCP_SetPmdType() */


/* --------------------------------------------------------------------	*/
/*	Internal:  This function checks a byte string to see if it	*/
/*			contains the same byte value throughout.	*/
/*			Returns TRUE if so, else returns FALSE.		*/
/* --------------------------------------------------------------------	*/
u_int8 LCP_ChkByteStr (register u_int8 * pSrc, u_int32 Len, u_int8 Pat)
{
	if ((pSrc == NULL) || (Len == 0))
		return (FALSE) ;

	for (; Len > 0 ; -- Len)
		if (*pSrc++ != Pat)
			return (FALSE) ;

	return (TRUE) ;

}	/* LCP_ChkByteStr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns the MAC address read from the LCP	*/
/*		serial EEPROM during LCP startup.			*/
/*		Returns NULL if driver not fully initialized.		*/
/* --------------------------------------------------------------------	*/
tLCP_MacAddrU * LCP_ReadMacAddr (tLCP_MacAddrU * pRtn)
{
	register tLCP_MacAddrU *	pMacU ;


#ifdef	ACP_REV0_LCP
		/* Hack, force MAC as 0:2:3:4:5:6 for now. */
	pMacU = & LCP_LclMacAdrDflt ;
#else
	pMacU = & LDI.pLcpEEprom->mac_addr ;
#endif

		/* Is LCP driver unaware of our local MAC address ? */
#ifdef	ACP_REV0_LCP
	if ((LDI.fInitialz < LCP_DVRINIT_DONE) || (pRtn == NULL))
#else
	if ((LDI.fInitialz <= LCP_DVRINIT_TWO) || (pRtn == NULL))
#endif
		pRtn = NULL ;

	else	/* Provide local MAC address. */
		memcpy (pRtn->byte, pMacU->byte, sizeof (pRtn->byte)) ;


	return (pRtn) ;

}	/* LCP_ReadMacAddr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns the MAC address received from NMP	*/
/*		during LCP startup.					*/
/*		Returns NULL if driver not fully initialized.		*/
/* --------------------------------------------------------------------	*/
tLCP_MacAddrU * LCP_NMP_MacAddr (tLCP_MacAddrU * pRtn)
{
	register tLCP_MacAddrU *	pMacU ;


	pMacU = (tLCP_MacAddrU *) & LDI.NMP_Mac ;

	/* Is LCP driver NOT fully initialized ? */
	if ((LDI.fInitialz < LCP_DVRINIT_FOUR) || (pRtn == NULL))
		pRtn = NULL ;

	else	/* Provide NMP MAC address. */
		memcpy (pRtn->byte, pMacU->byte, sizeof (pRtn->byte)) ;


	return (pRtn) ;

}	/* LCP_NMP_MacAddr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns our network-side MAC address as	*/
/*		assigned by the NMP during LCP startup.			*/
/*		Returns NULL if driver not fully initialized		*/
/*		or bad arguments.					*/
/* --------------------------------------------------------------------	*/
tLCP_MacAddrU * LCP_Ntwk_MacAddr (tLCP_MacAddrU * pRtn, u_int32 * pNbr)
{
	register tLCP_MacAddrU *	pMacU ;


	pMacU = (tLCP_MacAddrU *) & LDI.NtwkMac ;


		/* Is LCP driver NOT fully initialized ? */
	if ((LDI.fInitialz < LCP_DVRINIT_DONE) ||
					(pRtn == NULL) || (pNbr == NULL))
		pRtn = NULL ;

	else	/* Provide network-side MAC address from NMP. */
	{	/* Also give number of MACs in address block. */
		memcpy (pRtn->byte, pMacU->byte, sizeof (pRtn->byte)) ;
		*pNbr = LDI.NbrNtwkMacs ;
	}


	return (pRtn) ;

}	/* LCP_Ntwk_MacAddr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns NMP IP address received from NMP	*/
/*		during LCP startup.					*/
/*		Returns NULL if driver not fully initialized.		*/
/* --------------------------------------------------------------------	*/
tLCP_IPAddrU * LCP_NMP_IP_Addr (tLCP_IPAddrU * pRtn)
{
	register tLCP_IPAddrU *	pIP_U ;


	pIP_U = (tLCP_IPAddrU *) & LDI.NMP_IP ;

		/* Is LCP driver NOT fully initialized ? */
	if ((LDI.fInitialz < LCP_DVRINIT_DONE) || (pRtn == NULL))
		pRtn = NULL ;

	else	/* Provide NMP IP address. */
		memcpy (pRtn->ch, pIP_U->ch, sizeof (pRtn->ch)) ;


	return (pRtn) ;

}	/* LCP_NMP_IP_Addr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns our own IP address.			*/
/*		Returns NULL if driver not fully initialized.		*/
/* --------------------------------------------------------------------	*/
tLCP_IPAddrU * LCP_ACP_IP_Addr (tLCP_IPAddrU * pRtn)
{
	register tLCP_IPAddrU *	pIP_U ;


	pIP_U = (tLCP_IPAddrU *) & LDI.Own_IP ;

		/* Is LCP driver NOT fully initialized ? */
	if ((LDI.fInitialz < LCP_DVRINIT_DONE) || (pRtn == NULL))
		pRtn = NULL ;

	else	/* Provide NMP IP address. */
		memcpy (pRtn->ch, pIP_U->ch, sizeof (pRtn->ch)) ;


	return (pRtn) ;

}	/* LCP_ACP_IP_Addr */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns ATM interface starting index rcvd	*/
/*		during driver initialization (ATM_INSTANCE_MSG).	*/
/*		Will return 0 if LCP driver is not initialized.		*/
/* --------------------------------------------------------------------	*/
u_int32 LCP_AtmIf_StartIndex (void)
{
		/* Is LCP driver NOT fully initialized ? */
	if (LDI.fInitialz < LCP_DVRINIT_DONE)
		return (0) ;

		/* Return value from LCP init. */
	else	return (LDI.IF_Start) ;

}	/* LCP_AtmIf_StartIndex */


/* --------------------------------------------------------------------	*/
/*	API:  This routine returns ATM interface index range received	*/
/*		during driver initialization (ATM_INSTANCE_MSG).	*/
/*		Will return 0 if LCP driver is not initialized.		*/
/* --------------------------------------------------------------------	*/
u_int32 LCP_AtmIf_IndexRange (void)
{
		/* Is LCP driver NOT fully initialized ? */
	if (LDI.fInitialz < LCP_DVRINIT_DONE)
		return (0) ;

		/* Return value from LCP init. */
	else	return (LDI.IF_Range) ;

}	/* LCP_AtmIf_IndexRange */


/* --------------------------------------------------------------------	*/
/*	API:  This routine sets the LCP driver internal debug print	*/
/*		flag.  It returns to the previous value of that flag.	*/
/*		The input value is interpreted as follows:		*/
/*			 2 :	read existing value, no change.		*/
/*			 1 :	force debug prints on.			*/
/*			 0 :	force debug prints off.			*/
/*			-1 :	use debug jumper.			*/
/* --------------------------------------------------------------------	*/
u_int8 LCP_SetDbgPrt (s_int8 NewVal)
{
	register volatile tLCP_DriverInfo *	pDI ;
	u_int8					OldVal ;


	pDI = & LDI ;
	OldVal = pDI->fDbgPrts ;


	if (NewVal == 2)		/* Read, no change. */
		{ }

	else  if (NewVal > 0)		/* Force on. */
		pDI->fDbgPrts = TRUE ;

	else  if (NewVal < 0)		/* Use debug jumper. */
		pDI->fDbgPrts = C_CHK_JMPR (ACP_RRB_JMP_DEBUG_) ;

	else	pDI->fDbgPrts = FALSE ;	/* Force off. */


	return (OldVal) ;

}	/* LCP_SetDbgPrt */


/* --------------------------------------------------------------------	*/
/*	API:  This routine initializes the LCP Driver.			*/
/*		Initialization completes when the LCP itself is done;	*/
/*		this must be checked for later.				*/
/*		Assumes that MFP HW recovery timers are available.	*/
/*		Returns 0 on success, non-zero on failure.		*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_DriverInit_1 (void (** VecTbl)(void), u_int8 fReset,
			  u_int8 fMFPIntr, u_int8 fKernelMode,
			  p_void pPreviousLDI)
{
	register volatile tLCP_DriverInfo *	pDI ;
	register volatile u_int8 *		pReg ;
	s_int16					Count ;


		/* Need to groom MFP here, both interrupt lines and timer. */
	pDI  = & LDI ;
	pReg = ((volatile u_int8 *) (ACP_HW_IOREG_BASE)) ;
	Count = 0 ;

	MFP_HwtClrTimer (LCP_TxTOutCallBack) ;
	(void) A_BlockFill ((u_int8 *) pDI, sizeof (* pDI), 0, AR_SvcWatchdog) ;

		/* Copy data from previous LCP driver instance, ie.      */
		/* in Boot, IF it is known and we are not resetting LCP. */
#ifdef	DEBUG
	buginf ("Previous LDI @ $%08x\n", pPreviousLDI) ;
#endif
	if ((pPreviousLDI) && (! fReset))
	{
		memcpy ((tLCP_DriverInfo *) pDI, pPreviousLDI, sizeof (* pDI)) ;
	}
	else	/* Not copying previous LDI data (ie. from Boot). */
	{
		pDI->fInitialz      = LCP_DVRINIT_ONE ;
		pDI->fTxPosted      = FALSE ;
		pDI->fTxTimeout     = FALSE ;
		pDI->ModStat        = 0 ;
		pDI->PortStat       = 0 ;
	}

		/* Always set these fields anyway. */
	pDI->fDbgPrts       = FALSE ;
	pDI->fKernel        = fKernelMode ;
	pDI->HWTOutVal      = LCP_STD_TX_TOUT ;
	pDI->Rx.pRxDoneIntr = & pReg [ACP_WRRG_RD_FR_LCP_] ;
	pDI->Tx.pTxPostIntr = & pReg [ACP_WRRG_TP_TO_LCP_] ;

	LCP_InitTrace () ;

		/* Whenever the LCP is reset, ensure that the DPRAM gets  */
		/* put into a known (zero) state as well.  This allows us */
		/* to poll DPRAM for the LCP Reset Ack, if necessary.     */
		/* Also, we can clean/force inactive the interrupt lines  */
		/* from the ACP TO the LCP.                               */
	if (fReset)
	{
		pReg [ACP_WRRG_LCP_RESET_] = ON_ ;

		for (Count = 0 ; Count < 10 ; )	/* Pause to take effect. */
			{ ++ Count ; }

		(void) A_BlockFill ((u_int8 *) (ACP_HW_LCP_RAM_BASE),
				     ACP_HW_LCP_RAM_SIZE, 0, AR_SvcWatchdog) ;
	}

		/* Setup MFP IO lines for LCP status. */
	if (fMFPIntr)
	{
		MFP_BindEnb (VecTbl, A_LCP_Rx_Isr,
					MFP_BASE_VECTOR, MFP_IINDX_LCPTP) ;
		MFP_BindEnb (VecTbl, A_LCP_Tx_Isr,
					MFP_BASE_VECTOR, MFP_IINDX_LCPRD) ;
	}

		/* At this point LCP interrupts are enabled but masked off   */
		/* at the MFP.  We will unmask them (unless in polling mode) */
		/* during the second half of initialization, after the LCP   */
		/* has given the Reset Ack message.                          */
		/* Assume that the caller has dropped BIGA from reset.       */
		/* LCP reset also controls SAGE, and SAGE uses BIGA for DMA. */

		/* Do NOT touch ACP interrupt lines TO LCP unless we just    */
		/* reset the LCP.  Assume that PROM Boot groomed these lines */
		/* properly after HW Reset, when LCP was dropped from its    */
		/* reset.  Otherwise, we might lose any message posted to us */
		/* BEFORE this point by the LCP.                             */
		/* This is because on HW reset, those interrupt lines are in */
		/* a low/active condition, and making them inactive causes a */
		/* transition (edge).  Since the edge is seen by LCP, this   */
		/* can ONLY be done safely when the LCP itself is in reset.  */

		/* NB. Older Boots do NOT release LCP from reset at startup. */
		/* Assume (to support older Boots) that no LCP handshakes    */
		/* occur BEFORE calling here, for Boot-1 or Boot-2.  This    */
		/* is NOT the case during Kernel startup, since Boot-2 will  */
		/* have done some LCP messages.  Thus we can groom our HW    */
		/* interrupt lines to the LCP without worry in Boot cases.   */
		/* For the Kernel we SHOULD be able to do this, but I do not */
		/* want to risk it.  Failure mode would be if Boot-2 was in  */
		/* the middle of LCP handshaking and then jumped to Kernel.  */
	A_LCP_Release (! fKernelMode) ;

	if (pDI->fInitialz == LCP_DVRINIT_ONE)
	{
		pDI->fInitialz |= LCP_DVRINIT_XITFLG ;
	}

	return (LCP_RC_Normal) ;			/* Show success. */

}	/* LCP_DriverInit_1 */


/* --------------------------------------------------------------------	*/
/*	API:  This routine checks for LCP to do initial handshake	*/
/*		with ACP using LCP Ready (Reset Ack) message.		*/
/*		Once LCP Ready msg is found copy it into driver global	*/
/*		data and return so that local diags can be run.		*/
/*		Flags indicate whether to wait forever on LCP Ready.	*/
/*		LCP interrupts should still be masked at the MFP upon	*/
/*		entry here.  They may be unmasked once LCP init is	*/
/*		finished, if appropriate.				*/
/*	Returns:							*/
/*		0 (LCP_RC_Normal) on success, non-0 on LCP busy.	*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_DriverInit_2 (u_int8 fOnce, u_int8 fFwdRsp, u_int8 fPolling)
#ifdef	ACP_REV0_LCP
{
	register volatile tLCP_DriverInfo *	pDI = & LDI ;


		/* Physical slot numbers are 1-based, so 0 shows */
		/* still uninitialized situation.                */
	while (pDI->MySlotNbr == 0)
	{
			/* Wait for the Reset Ack message. */
		if (LCP_ReceivePoll0 (fOnce, & pDI->ResetAck.BU) != NULL)
		{
				/* Our phys slot number must NEVER be 0 ! */
			pDI->MySlotNbr  = pDI->ResetAck.MH.src_addr + 1 ;

			pDI->MyInstNbr	= 0 ;	/* Hack until msg defined. */
			pDI->pLcpEEprom = (tLCP_EEpromData *)
					  & pDI->ResetAck.RA.serEProm [0] ;
			pDI->fInitialz  = LCP_DVRINIT_DONE ;

				/* Copy NMP's MAC address. */
			memcpy (pDI->NMP_Mac.byte,
				pDI->NMP_Mac.byte,	/* Hack til defined. */
				sizeof (pDI->NMP_Mac.byte)) ;

				/* Hack to use EEPROM MAC, which is unique, */
				/* for ATM/LEC MACs.  Since our own MAC is  */
				/* canned (EEPROM MAC not used for IPC) and */
				/* we do NOT learn ATM/LEC MACs from NMP yet.*/
			memcpy (pDI->NtwkMac.byte,
				& pDI->ResetAck.BF [LCP_MSG_MACADDR_OFST],
				sizeof (pDI->NtwkMac.byte)) ;
			pDI->NbrNtwkMacs = 16 ;	/* Hack until msg defined. */

				/* Format Reset Ack to NMP even if we */
				/* do not send it at this time.  Thus */
				/* it will be available when needed.  */
			pDI->ResetAck.MH.dest_addr =
						LCP_SLOTNUM (LCP_NMP_PHYS_SLOT);
			pDI->ResetAck.MH.dsap = LCP_NMP_CFG_SAP ;
			pDI->ResetAck.MH.ssap = (LCP_ACP_SAP_FLAG  |
						 LCP_ACP_OWN_RESET_SAP) ;

				/* Reset Ack is now ready to be sent to NMP. */
				/* Wait for LCP Tx to be free, and wait for  */
				/* LCP TxAck too.  Wait forever (until HW    */
				/* timeout), as last check that LCP works.   */
			if (fFwdRsp)
				LCP_TransmitPoll (TRUE, TRUE, FALSE,
							& pDI->ResetAck.BU) ;
		}
		else  if (fOnce)
		{
			return (LCP_RC_InitBusy) ;
		}
	}

	if (! fPolling)
	{
		MFP_UnmaskIntrW (LCP_MFP_BITMASK, FALSE, FALSE) ;
	}

	return (LCP_RC_Normal) ;

}	/* LCP_DriverInit_2 */
#else	/* ACP_REV0_LCP */
{
	register volatile tLCP_DriverInfo *	pDI = & LDI ;


		/* Look out for warm driver restarts (ie. kernel startup). */
	if (pDI->fInitialz > LCP_DVRINIT_TWO)
		return (LCP_RC_Normal) ;


		/* The very first message to wait for is LCP_MSG_LCP_READY. */
		/* This is the initial handshake, so use a special polling  */
		/* routine.  After this message is found, normal polling    */
		/* can be used (we'll be in sync) with the LCP.             */
	pDI->fInitialz = LCP_DVRINIT_TWO ;

	while (TRUE)
	{
			/* Do NOT adjust driver state to show exit from */
			/* this state unless we get the LCP Ready msg.  */
		if (LCP_ReceivePoll0 (fOnce,
				      (tLCP_MsgBufU *) & pDI->ModulRdy.BU)
				== NULL)
		{
			if (fOnce)
				return (LCP_RC_InitBusy) ;
			else
				continue ;
		}

		if (pDI->ModulRdy.MH.opcode == LCP_MSG_LCP_READY)
			break ;
	}

		/* Done hunting for initial LCP handshake message so */
		/* groom/clean any spurious LCP RxDone interrupts.   */
	MFP_ClrPendIntr (MFP_IMSK_LCPRD, TRUE) ;

		/* Our physical slot number must NEVER be 0 ! */
	pDI->MySlotNbr = pDI->ModulRdy.MH.dest_addr + 1 ;

	pDI->pLcpEEprom = (tLCP_EEpromData *) & (pDI->ModulRdy.RA.serEProm [0]);


		/* Re-format LCP Ready message header to do as our own   */
		/* Module Ready message header, so that it can be sent   */
		/* on to the NMP.  We will send this later, after diags. */
		/* We force header address, as the LCP used its own and  */
		/* this message now goes to the NMP.                     */
	pDI->ModulRdy.MH.dest_addr = LCP_SLOTNUM (LCP_NMP_PHYS_SLOT) ;
	pDI->ModulRdy.MH.src_addr  = LCP_SLOTNUM (pDI->MySlotNbr) ;
	pDI->ModulRdy.MH.dsap      = (LCP_NMP_CFG_SAP) ;
	pDI->ModulRdy.MH.ssap      = (LCP_ACP_OWN_RESET_SAP) ;

		/* Leave length as is, change op-code. */
	pDI->ModulRdy.MH.opcode    = LCP_MSG_MODULE_READY ;

	buginf ("LcpRx: Lcp Rdy\n") ;

	if (pDI->fInitialz == LCP_DVRINIT_TWO)
		pDI->fInitialz |= LCP_DVRINIT_XITFLG ;

	return (LCP_RC_Normal) ;			/* Show success. */

}	/* LCP_DriverInit_2 */
#endif	/* ACP_REV0_LCP */

#ifndef	ACP_REV0_LCP

/* --------------------------------------------------------------------	*/
/*	Parse SCP Set Port Features messages.				*/
/*	Can be called from Rx ISR.					*/
/* --------------------------------------------------------------------	*/
void LCP_SetPortFeatures (register volatile tLCP_DriverInfo * pDI,
			  register tLCP_DvrBufU * pRx, u_int8 fIntr)
{
	u_int16		CountBits ;
	u_int16		CountFeat ;
	u_int32		FeatMask  ;


		/* Indicate that many features are unsupported. */
		/* However, we return a good response if any    */
		/* supported feature is given to us.            */
	pRx->PF.cmdStatus = SCPSTAT_NOTSUPPORTED ;


		/* Feature data bytes are ONLY defined for those  */
		/* features that are given in the feature mask !! */
	for (CountBits = 0, CountFeat = 0, FeatMask = 1 ;
		CountBits < MAX_PORT_FEATURES ; ++ CountBits, FeatMask <<= 1)
	{
	    if (pRx->PF.feature_mask & FeatMask)
	    {
		switch (CountBits)
		{
		    case port_enable:
			if (pRx->PF.feature_data [CountFeat] &
			    (PE_ENABLE | PE_UNRESET))
			    pDI->PortStat = (SCP_PSTAT_ENABLED) ;
			pRx->PF.cmdStatus = SCPSTAT_OK ;
			break ;
		    case port_loopback:
			if ((pRx->PF.feature_data [CountFeat] &
			     (PL_EXT_LOOPBACK_ON | PL_INT_LOOPBACK_ON)) == 0)
			{
					/* All loops off. */
				pDI->fInitialz = LCP_DVRINIT_FIVE ;
			}
			pRx->PF.cmdStatus = SCPSTAT_OK ;
			break ;
		    default:
			break ;
		}

		++ CountFeat ;
	    }
	}

}	/* LCP_SetPortFeatures */

/* --------------------------------------------------------------------	*/
/*	Get Card & Port Status from the LED driver                      */
/*	Can be called from Rx ISR.					*/
/* --------------------------------------------------------------------	*/
void LCP_SetCardLedStatus (register volatile tLCP_DriverInfo * pDI,
			  u_int8 fIntr)
{
	int  led_color;

	led_color = led_status(ACP_WRRG_PMD_STAT_LED);
	switch(led_color)
	{
		case  ACP_PMD_STAT_LED_OFF:
		   pDI->ModStat = SCP_MSTAT_CARDDEAD;    /* 0x04 */
		   break;
		case  ACP_PMD_STAT_LED_RED: 
		   pDI->ModStat = SCP_MSTAT_CARDBAD;     /* 0x02 */
		   break;
		case  ACP_PMD_STAT_LED_GRN:
		   pDI->ModStat = SCP_MSTAT_ENABLED;     /* 0x01 */ 
		   break;
		case  ACP_PMD_STAT_LED_YEL:
		   pDI->ModStat =  SCP_MSTAT_DISABLED;   /* 0x00 */
		   break;
		default:
		   pDI->ModStat =  SCP_MSTAT_DISABLED;   /* 0x00 */
		   break;
	}

	led_color = led_status(ACP_WRRG_PMD_BOT_LED);
	switch(led_color)
	{
		case  ACP_PMD_LEVL_LED_OFF:
		   pDI->PortStat = SCP_PSTAT_LINKBAD;    /* 0x02 */
		   break;
		case  ACP_PMD_LEVL_LED_GRN:
		   pDI->PortStat = SCP_PSTAT_ENABLED;     /* 0x01 */ 
		   break;
		case  ACP_PMD_LEVL_LED_YEL:
		   pDI->PortStat =  SCP_PSTAT_DISABLED;   /* 0x00 */
		   break;
		default:
		   pDI->PortStat =  SCP_PSTAT_DISABLED;   /* 0x00 */
		   break;
	}

}	/* LCP_SetCardLedStatus() */

#endif	/* ACP_REV0_LCP */

#ifndef	ACP_REV0_LCP

/* --------------------------------------------------------------------	*/
/*	Handle specific inbound LCP messages.				*/
/*	Can be called from Rx ISR.					*/
/* --------------------------------------------------------------------	*/
void LCP_TakeRxMsgs (register volatile tLCP_DriverInfo * pDI,
			register tLCP_DvrBufU * pRx, u_int8 fIntr)
{
	switch (pRx->MH.opcode)
	{
	    case LCP_MSG_MODULE_ENABLE:
		switch (pRx->ED.cmdStatus)
		{
		    case COMMAND_MODULE_ENABLE:
			pDI->ModStat |= (SCP_MSTAT_ENABLED) ;
			pRx->ED.cmdStatus = STATUS_MODULE_ENABLED ;
			break ;
		    case COMMAND_MODULE_DISABLE:
#if	0	/* Not supported. */
			pDI->ModStat &= ~(SCP_MSTAT_ENABLED) ;
			pRx->ED.cmdStatus = STATUS_MODULE_DISABLED ;
#else
			pRx->ED.cmdStatus = SCPSTAT_NOTSUPPORTED ;
#endif
			break ;
		    case COMMAND_MODULE_ON_LINE:
			pDI->ModStat |= (SCP_MSTAT_ON_LINE) ;
			pDI->fInitialz = LCP_DVRINIT_DONE ;
						/* LCP/NMP handshakes done ! */
			pRx->ED.cmdStatus = STATUS_MODULE_ON_LINE ;
			break ;
		    case COMMAND_MODULE_OFF_LINE:
#if	0	/* Not supported. */
			pDI->ModStat &= ~(SCP_MSTAT_ON_LINE) ;
			pRx->ED.cmdStatus = STATUS_MODULE_OFF_LINE ;
#else
			pRx->ED.cmdStatus = SCPSTAT_NOTSUPPORTED ;
#endif
			break ;
		    default:
			pRx->ED.cmdStatus = SCPSTAT_NOTSUPPORTED ;
			break ;
		}
		break ;
	    case LCP_MSG_SET_PORT_FEATR:
		LCP_SetPortFeatures (pDI, pRx, fIntr) ;
		break ;
	    case LCP_MSG_SET_PORT_STATE:
			/* Pass these up to LANE for spanning-tree support  */
			/* if we are in Kernel mode (LANE might exist) AND  */
			/* we have finished LCP driver init.  Messages seen */
			/* during init are NOT passed up (LANE not there).  */
			/* Assume that up-call is fast enough for interrupt */
			/* service, and meeting LCP/SCP message ack timing. */

		if ((pDI->fKernel) && (pDI->fInitialz >= LCP_DVRINIT_DONE))
			(void) LCPl_LanePortStateInd (pRx) ;

		pRx->PR.status = STATUS_SET_PORT_STATE_GOOD ;
		break ;
	    case LCP_MSG_NMP_MAC_IP_ADDR:
		memcpy ((tLCP_MacAddrU *) pDI->NMP_Mac.byte, pRx->NA.MAC_addr,
					sizeof (pRx->NA.MAC_addr)) ;
		memcpy ((tLCP_IPAddrU *) pDI->NMP_IP.ch, pRx->NA.IP_addr,
					sizeof (pRx->NA.IP_addr)) ;

			/* ACP internal IP address differs from NMP's */
			/* by using our physical slot number with LSB.*/
		memcpy ((tLCP_IPAddrU *) pDI->Own_IP.ch,
			(tLCP_IPAddrU *) pDI->NMP_IP.ch,
			sizeof (pDI->NMP_IP.ch)) ;
		pDI->Own_IP.ch [3] += LCP_SLOTNUM (pDI->MySlotNbr) ;

			/* Once we have this startup msg we can */
			/* prepare for system loopback tests.   */
		pDI->fInitialz = LCP_DVRINIT_FOUR ;
		break ;
	    case LCP_MSG_ATM_INSTANCE:
		pDI->MyInstNbr   = pRx->AI.ATMCardInstanceNumber ;
		pDI->NbrNtwkMacs = 16 ;	/* RKL:  Hack until msg defined. */
		memcpy ((tLCP_MacAddrU *) pDI->NtwkMac.byte,
					pRx->AI.ATMMACAddresses,
					sizeof (pRx->AI.ATMMACAddresses)) ;
		pDI->IF_Start = pRx->AI.IfStartIndex ;
		pDI->IF_Range = pRx->AI.IfIndexRange ;
		break ;
	    case LCP_MSG_DIAG_INFO:
		memcpy (pRx->BU.msg.mdata,
					(u_int8 *) pDI->ModulRdy.BU.msg.mdata,
					pDI->ModulRdy.BU.msg.length) ;
		pRx->BU.msg.length = pDI->ModulRdy.BU.msg.length ;
		break ;
	    case LCP_MSG_STATUS_POLL:
		LCP_SetCardLedStatus(pDI,fIntr);
		pRx->GS.modStat = pDI->ModStat ;
		pRx->GS.portStat [0] = pDI->PortStat ;
		pRx->GS.hdr.length = 2 ;
		break ;
	    case LCP_MSG_SET_CDP_MATCH_ADDR:
		pRx->MR.status = STATUS_SET_CDP_MATCH_ADDR_GOOD ;
		pRx->MR.hdr.length = SET_CDP_MATCH_ADDR_RESP_LEN ;
		break ;
	    case LCP_MSG_SET_VLAN_PARAMETERS:
			/* Pass these up to VTP Helper for VTP support if   */
			/* we are in Kernel mode (task might exist) AND we  */
			/* have finished LCP driver init.  Messages seen at */
			/* init are NOT passed up (Helper not there).       */
			/* Assume that up-call is fast enough for interrupt */
			/* service, and meeting LCP/SCP message ack timing. */

		if ((! pDI->fKernel) || (pDI->fInitialz < LCP_DVRINIT_DONE))
			pRx->VR.status = SET_VLAN_PARAMS_FAILURE ;

		else  if (LCPl_VTP_VlanParamCmd (pRx))
			pRx->VR.status = SET_VLAN_PARAMS_SUCCESS ;

		else	pRx->VR.status = SET_VLAN_PARAMS_FAILURE ;

		pRx->VR.hdr.length = sizeof (pRx->VR) - sizeof (pRx->VR.hdr) ;
		pRx->VR.msg_status = SET_VLAN_PARAMS_MSG_REPLY ;
		break ;
	    case LCP_MSG_CLR_COUNTERS:
		break ;
	    case LCP_MSG_SCP_PING:
		break ;
	    default:
		break ;
#if	0			/* What if LCP takes msg, but ACP does NOT ? */
	    default:		/* What the 8051 (LCP) does. */
		pRx->LM.mdata [0] = SCPSTAT_NOTSUPPORTED ;
		pRx->LM.length    = 1 ;
		break ;
#endif
	}


		/* Send response back to the source SAP on the NMP.         */
		/* Assume header op-code and length fields are already set. */
	pRx->MH.dest_addr = LCP_SLOTNUM (LCP_NMP_PHYS_SLOT) ;
	pRx->MH.src_addr  = LCP_SLOTNUM (pDI->MySlotNbr) ;
	pRx->MH.dsap      = pRx->MH.ssap ;
	pRx->MH.ssap      = (LCP_ACP_DRIVER_SAP) ;

	LCP_TransmitPoll (TRUE, TRUE, FALSE, & pRx->BU) ;

}	/* LCP_TakeRxMsgs */
#endif	/* ACP_REV0_LCP */

#ifndef	ACP_REV0_LCP

/* --------------------------------------------------------------------	*/
/*	API:	This is called after diags are completed to return	*/
/*		results back to the NMP.  We also complete all other	*/
/*		LCP handshakes at this point, up to setting loopbacks.	*/	
/*									*/
/*		Flags indicate whether to poll forever on pending msgs	*/
/*		and if the Module Ready msg should be sent to the NMP.	*/
/*		LCP interrupts should still be masked at the MFP upon	*/
/*		entry here.  They will be unmasked once LCP init is	*/
/*		finished, if appropriate.				*/
/*		Diags results are given by pointer and length of array	*/
/*		of bytes containing the results.                        */
/*	Returns:							*/
/*		0 (LCP_RC_Normal) on success, non-0 on LCP busy.	*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_DriverInit_3 (u_int8 fOnce, u_int8 fFwdRsp, u_int8 fPolling,
			  u_int8 DiagsLen, u_int8 * pDiagRes)
{
	register volatile tLCP_DriverInfo *	pDI = & LDI ;


		/* Watch out for warm driver startup, ie. */
		/* using Boot LCP driver instance data.   */
	if (pDI->fInitialz < LCP_DVRINIT_FOUR)
	{
	    pDI->fInitialz = LCP_DVRINIT_THREE ;


		/* Should we send Module Ready to NMP ? */
	    if (fFwdRsp)
	    {
			/* Module Ready is ready to be sent to NMP.  */
			/* Wait for LCP Tx to be free, and wait for  */
			/* LCP TxAck too.  Wait forever (until HW    */
			/* timeout), as last check that LCP works.   */

		if ((DiagsLen > 0) && (pDiagRes != NULL))
		{
				/* Truncate the diags results if too big. */
			if (DiagsLen > LCP_DIAG_SIZ)
				DiagsLen = LCP_DIAG_SIZ ;

			memcpy ((u_int8 *)pDI->ModulRdy.RA.mpu_xtra_diag_result,
				pDiagRes, DiagsLen) ;
		}
		LCP_TransmitPoll (TRUE, TRUE, FALSE,
				  (tLCP_MsgBufU *) & pDI->ModulRdy.BU) ;

		buginf ("LcpTx: Mod Rdy\n") ;
	    }
	}


		/* Complete remaining LCP handshakes. */
	while (pDI->fInitialz < LCP_DVRINIT_FOUR)
	{
			/* Do NOT adjust driver state to show exit from  */
			/* this state unless we complete all handshakes. */
		if (LCP_ReceivePoll (fOnce,
				(tLCP_MsgBufU *) & pDI->RxHoldBuf.BU) == NULL)
		{
			if (fOnce)
				return (LCP_RC_InitBusy) ;
			else
				continue ;
		}

		LCP_TakeRxMsgs (pDI, (tLCP_DvrBufU *) & pDI->RxHoldBuf, FALSE) ;
	}


	return (LCP_RC_Normal) ;			/* Show success. */

}	/* LCP_DriverInit_3 */
#endif	/* ACP_REV0_LCP */

#ifndef	ACP_REV0_LCP

/* --------------------------------------------------------------------	*/
/*	API:	This is called while loopbacks are in effect to check	*/
/*		for SCP messages that are part of the loop test.	*/	
/*									*/
/*		Flags indicate whether to poll forever on pending msgs.	*/
/*		LCP interrupts should still be masked at the MFP upon	*/
/*		entry here.  They will be unmasked once LCP init is	*/
/*		finished, if appropriate.				*/
/*	Returns:							*/
/*		TRUE when loopbacks done, else FALSE.			*/
/* --------------------------------------------------------------------	*/
u_int8 LCP_DriverInit_4 (u_int8 fOnce)
{
	register volatile tLCP_DriverInfo *	pDI = & LDI ;


		/* Watch out for warm driver startup, ie. */
		/* using Boot LCP driver instance data.   */

		/* Check and handle SCP messages. */
	if (pDI->fInitialz < LCP_DVRINIT_FIVE)
	{
			/* Do NOT adjust driver state to show exit from  */
			/* this state unless we complete all handshakes. */
		if (LCP_ReceivePoll (fOnce,
					(tLCP_MsgBufU *) & pDI->RxHoldBuf.BU))
			LCP_TakeRxMsgs (pDI, (tLCP_DvrBufU *) & pDI->RxHoldBuf,
					FALSE) ;
	}


	return (pDI->fInitialz >= LCP_DVRINIT_FIVE) ;

}	/* LCP_DriverInit_4 */
#endif	/* ACP_REV0_LCP */

#ifndef	ACP_REV0_LCP

/* --------------------------------------------------------------------	*/
/*	API:	This is called after loops are completed to finish	*/
/*		all other LCP handshakes, up to going on-line.		*/	
/*									*/
/*		Flags indicate whether to poll forever on pending msgs.	*/
/*		LCP interrupts should still be masked at the MFP upon	*/
/*		entry here.  They will be unmasked once LCP init is	*/
/*		finished, if appropriate.				*/
/*	Returns:							*/
/*		0 (LCP_RC_Normal) on success, non-0 on LCP busy.	*/
/* --------------------------------------------------------------------	*/
s_int32 LCP_DriverInit_5 (u_int8 fOnce, u_int8 fPolling)
{
	register volatile tLCP_DriverInfo *	pDI = & LDI ;


		/* Watch out for warm driver startup, ie. */
		/* using Boot LCP driver instance data.   */

		/* Complete remaining LCP handshakes. */
	while (pDI->fInitialz < LCP_DVRINIT_DONE)
	{
			/* Do NOT adjust driver state to show exit from  */
			/* this state unless we complete all handshakes. */
		if (LCP_ReceivePoll (fOnce,
				(tLCP_MsgBufU *) & pDI->RxHoldBuf.BU) == NULL)
		{
			if (fOnce)
				return (LCP_RC_InitBusy) ;
			else
				continue ;
		}

		LCP_TakeRxMsgs (pDI, (tLCP_DvrBufU *) & pDI->RxHoldBuf, FALSE) ;
	}


	if (! fPolling)
	{
		MFP_UnmaskIntrW (LCP_MFP_BITMASK, FALSE, FALSE) ;
	}

	/* Need to check MFP/LCP status lines, in case Kernel driver */
	/* is being warm started and Rx packets are already pending. */
	if (!(MFP_Poll_IOs (0, MFP_GPI_LCP_MP_, TRUE, FALSE) & MFP_GPI_LCP_MP_))
	{
		LCP_Rx_Isr () ;				/* Rx a packet. */
	}

#if	0						/* Dump uses 6 sec. */
	if (fPolling)
		LCP_DumpTrace () ;			/* Not in kernel. */
#endif

	return (LCP_RC_Normal) ;			/* Show success. */

}	/* LCP_DriverInit_5 */
#endif	/* ACP_REV0_LCP */


/* --------------------------------------------------------------------	*/
/*	LCP-Receive C-code interrupt service routine.			*/
/*	Called from interrupt-level by the Assembly ISR.		*/
/* --------------------------------------------------------------------	*/
void LCP_Rx_Isr ()
{
	LCP_CleanupRx (& LDI, NULL, FALSE) ;

#ifndef	ACP_REV0_LCP
		/* Need to unmask interrupts at MFP since LCP-Rx msgs     */
		/* usually force an immediate response/ack, ie. LCP-Tx.   */
		/* This LCP-Tx ack is driven from here in polled fashion. */
		/* Thus the MFP may mask for LCP polling operation.       */
	LCP_TakeRxMsgs (& LDI, (tLCP_DvrBufU *) & LDI.RxHoldBuf, TRUE) ;
	MFP_UnmaskIntrW (LCP_MFP_BITMASK, FALSE, FALSE) ;
#endif

}	/* LCP_Rx_Isr */


/* --------------------------------------------------------------------	*/
/*	LCP-Transmit Done C-code interrupt service routine.		*/
/*	Called from interrupt-level by the Assembly ISR.		*/
/* --------------------------------------------------------------------	*/
void LCP_Tx_Isr ()
{
	LCP_CleanupTx (& LDI, FALSE, FALSE) ;

}	/* LCP_Tx_Isr */


/* --------------------------------------------------------------------	*/
/*	Mark internal state for flash code unavailability (download	*/
/*	needed) for SCP handshake messages.  Meant for PROM Boot use.	*/
/* --------------------------------------------------------------------	*/
void LCP_MarkFlashCodeState (u_int8 fBad)
{
	ALCSTATUS *	pStat ;


	pStat = (ALCSTATUS *) (LDI.ModulRdy.RA.mpu_xtra_diag_result) ;

	if (fBad)
		pStat->AcpErrStat1 |= (ACP_FLASH_CODE) ;
	else
		pStat->AcpErrStat1 &= ~(ACP_FLASH_CODE) ;

}	/* LCP_MarkFlashCodeState */
