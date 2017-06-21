/* $Id: lcp_test2.c,v 1.1.4.1 1996/05/09 14:54:58 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/lcp_test2.c,v $
 *------------------------------------------------------------------
 * lcp_test2.c -- Synergy ATM line-card LCP driver test support.
 *
 * March 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_test2.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:58  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:12  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:30  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:25  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:54  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.12  1995/10/13  06:33:20  rlowe
 * Cleaned up some ifdefs.
 *
 * Revision 1.11  1995/09/07  12:09:06  rlowe
 * LCP driver changes to 1) push driver data/state from Boot-2 into IOS
 * Kernel; 2) allow for NMP-driven system loopback diags during SCP
 * startup sequence.
 *
 * Revision 1.10  1995/07/26  04:15:47  rlowe
 * Change LCP driver init so that driver knows explicitly if it is being
 * started for the Kernel (vs. a Boot).  Supports LCP driver messaging
 * to applications.
 *
 * Revision 1.9  1995/06/09  08:18:02  rlowe
 * Integrate to latest LCP driver (message model, startup & HW handshakes).
 * Set local default MAC as 0:2:3:4:5:6 for testing.
 *
 * Revision 1.8  1995/05/19  21:51:06  rlowe
 * Add LCP_Ntwk_MacAddr to provide network-side MAC address block for
 * atm driver/lane startup.  Also futz around current syn_common instability.
 *
 * Revision 1.7  1995/05/13  22:39:26  rlowe
 * Support Rev 2 in-band downloading, and shift image start address to
 * $40004000.
 *
 * Revision 1.6  1995/05/04  10:38:31  rlowe
 * Integrate in-band ipc & download to Boot-2 for testing, using TEST_DOWNLOAD
 * flag.
 *
 * Revision 1.5  1995/05/04  07:20:21  rlowe
 * Add new APIs LCP_NMP_PhysSlotNbr and LCP_NMP_MacAddr.
 *
 * Revision 1.4  1995/04/27  01:52:41  rlowe
 * Cleanup LCP_ReadMacAddr, and LCP_PhysSlotNbr; added LCP_LogicalCardNbr.
 * Now provide a default MAC address (and logical card number) if LCP EEPROM
 * is not configured.
 *
 * Revision 1.3  1995/04/25  03:15:34  rlowe
 * Change from a static echo-test repeat count to a user-provided count.
 *
 * Revision 1.2  1995/04/03  04:27:44  rlowe
 * Sync up code, including LCP support, for IOS 10.3 merge.
 *
 * Revision 1.1  1995/03/29  08:16:48  rlowe
 * Archive current LCP driver as integration continues.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "syn_ser_eeprom.h"
#include "scp_defs.h"
#include "scp_msgs.h"
#include "lcp_if.h"
#include "lcp_defs.h"
#include "lo_wdog.h"
#include "mfp_hw_defs.h"
#include "acp_c_defs.h"


static u_int8	LCPT_MyId [] = "LCP Test:" ;

tLCP_DvrBufU	LCPT_TxBuf ;
tLCP_DvrBufU	LCPT_RxBuf ;
tLCP_DriverInfo	LCPT_DvrInfo ;



/* --------------------------------------------------------------------	*/
/*	Define this in case it doesn't exist in our libc.		*/
/* --------------------------------------------------------------------	*/
s_int32 memcmp (register s_int8 * pOne, register s_int8 * pTwo,
							register s_int32 Len)
{
	while (Len > 0)
	{
		if (*pOne > *pTwo)
			return (1) ;		/* String-1 > String-2 */

		else  if (*pOne < *pTwo)
			return (-1) ;		/* String-1 < String-2 */

		-- Len ;
		++ pOne ;
		++ pTwo ;
	}

	return (0) ;				/* Match !! */

}	/* memcmp */


/* --------------------------------------------------------------------	*/
/*	This routine formats, sends, and compares Echo messages with	*/
/*	the local LCP.  Provides a convenient way of checking on LCP	*/
/*	performance too.						*/
/* --------------------------------------------------------------------	*/
void LCPT_EchoMsgTest (u_int32 Nbr)
{
	s_int32		RC ;
	u_int8		MsgLen ;


	printf ("%s Echo Test for %u\n\t", LCPT_MyId, Nbr) ;

	memset (LCPT_TxBuf.BF, 0, sizeof (LCPT_TxBuf.BF)) ;
	memset (LCPT_RxBuf.BF, 0, sizeof (LCPT_RxBuf.BF)) ;

		/* Allocate room in front of string for a 4-byte counter. */
		/* Align counter, so need extra pad byte in front of it.  */
	sprintf (& LCPT_TxBuf.BU.msg.mdata [5], "Echo Test of %s %s",
		 __DATE__, __TIME__ ) ;

	MsgLen = strlen (& LCPT_TxBuf.BU.msg.mdata [5]) + 5 ;
	RC     = LCP_RC_Normal ;

	printf ("%s echo msg body len %d\n", LCPT_MyId, MsgLen) ;

	LCPP_FmtHeader (& LCPT_TxBuf.BU, TRUE, MsgLen, LCP_LCL_8051_SAP,
#ifdef	ACP_REV0_LCP
			(LCP_ACP_SAP_FLAG | LCP_ACP_OWN_RESET_SAP),
#else
			LCP_ACP_DRIVER_SAP,
#endif
			LCP_MSG_SCP_PING) ;

	while (Nbr > 0)
	{
		-- Nbr ;
		if ((Nbr % 1000) == 0)
			puts ("+ ") ;

		++ * (u_int32 *)(& LCPT_TxBuf.BU.msg.mdata [1]) ;
		RC = LCP_TransmitPoll (TRUE, TRUE, FALSE, & LCPT_TxBuf.BU) ;
		if (RC != LCP_RC_Normal)
			printf ("%s LCP_TransmitPoll rtn %d\n", LCPT_MyId, RC) ;

		LCP_ReceivePoll  (FALSE, & LCPT_RxBuf.BU) ;

			/* Check body and parts of the message header too. */
		if (LCPT_TxBuf.BU.msg.addr != LCPT_RxBuf.BU.msg.addr)
			printf ("%s msg hdr addr mismatch %02x %02x\n",
				LCPT_MyId, LCPT_TxBuf.BU.msg.addr,
				LCPT_RxBuf.BU.msg.addr) ;

		if (LCPT_TxBuf.BU.msg.length != LCPT_RxBuf.BU.msg.length)
			printf ("%s msg hdr len mismatch %02x %02x\n",
				LCPT_MyId, LCPT_TxBuf.BU.msg.length,
				LCPT_RxBuf.BU.msg.length) ;

		if (LCPT_TxBuf.BU.msg.opcode!= LCPT_RxBuf.BU.msg.opcode)
			printf ("%s msg hdr opcode mismatch %02x %02x\n",
				LCPT_MyId, LCPT_TxBuf.BU.msg.opcode,
				LCPT_RxBuf.BU.msg.opcode) ;

		RC = memcmp (LCPT_TxBuf.BU.msg.mdata, LCPT_RxBuf.BU.msg.mdata,
								MsgLen + 5) ;
		if (RC)
			printf ("%s body memcmp did not match = %d\n",
				LCPT_MyId, RC) ;
	}

	printf ("*\n%s Echo Test finished\n", LCPT_MyId) ;

}	/* LCPT_EchoMsgTest */


/* --------------------------------------------------------------------	*/
/*	This is a test-only routine to dump out MFP I/O pins for LCP.	*/
/* --------------------------------------------------------------------	*/
#define	LCPT_LCP_IMASK	(MFP_INTR_GPIP3 | MFP_INTR_GPIP2)
extern volatile tLCP_DriverInfo LDI ;
extern void LCP_TxTOutCallBack (u_int32 Arg) ;

void LCPT_CheckMFP ()
{
	register volatile u_int8 *	pMFP ;
	u_int8				Pnd  ;
	u_int8				OPnd ;
	u_int8				IOs  ;
	u_int8				OIOs ;
	u_int8				fGo  ;
	register volatile tLCP_DriverInfo * pDI ;


	pMFP = (u_int8 *) (ACP_HW_MFP_BASE) ;
	Pnd  = 0 ;
	OPnd = 0 ;
	IOs  = 0 ;
	OIOs = 0 ;
	fGo  = TRUE ;
	pDI  = & LDI ;
	pDI->fTxTimeout = FALSE ;


	MFP_HwtSetTimer (LCP_TxTOutCallBack, 0, pDI->HWTOutVal) ;
	while (! pDI->fTxTimeout)
	{
		MFP_HwtPollChk (0, TRUE) ;
	}
	MFP_HwtClrTimer (LCP_TxTOutCallBack) ;
	pDI->fTxTimeout = FALSE ;
	printf ("%s HW timer popped\n", LCPT_MyId) ;

	do {
		PUNCH_WD ;

		Pnd = ((pMFP [MFP_RO_IPRB]) & (LCPT_LCP_IMASK)) ;
		IOs = ((pMFP [MFP_RO_GPDR]) & (LCPT_LCP_IMASK)) ;

		if ((Pnd != OPnd) || (IOs != OIOs) || fGo)
		{
			printf ("%s MFP-b Pend %02x (%02x), IO %02x (%02x)\n",
				LCPT_MyId, Pnd, OPnd, IOs, OIOs) ;

			OPnd = Pnd ;
			OIOs = IOs ;
			fGo  = FALSE ;
		}
	} while (FALSE) ;

}	/* LCPT_CheckMFP */


/* --------------------------------------------------------------------	*/
/*	This routine performs/completes LCP and LCP driver init, then	*/
/*	tests with some echo-back messages.  Due to the nature of LCP	*/
/*	driver init (requiring Reset Ack msg from LCP), do NOT do this	*/
/*	in a production system !! (IOS kernel will startup its LCP	*/
/*	driver, which will expect a Reset Ack !!)			*/
/* --------------------------------------------------------------------	*/
void LCPT_TestIt (u_int32 RepeatCount)
{
	tLCP_MacAddrU	MyMac ;
	u_int32		Val = 0 ;


	printf ("%s LCP_DriverInit_1 rtn %d\n", LCPT_MyId,
		LCP_DriverInit_1 (NULL, TRUE, TRUE, FALSE, NULL)) ;

	LCPT_CheckMFP () ;

	printf ("%s LCP_DriverInit_2 rtn %d\n", LCPT_MyId,
		LCP_DriverInit_2 (FALSE, FALSE /* TRUE */, TRUE)) ;

#ifndef	ACP_REV0_LCP
	printf ("%s LCP_DriverInit_3 rtn %d\n", LCPT_MyId,
		LCP_DriverInit_3 (FALSE, TRUE, TRUE, 0, NULL)) ;
#endif

	printf ("%s LCP_GetDrvrInfoSiz rtn %u\n", LCPT_MyId,
		LCP_GetDrvrInfoSiz ()) ;

	printf ("%s LCP_PhysSlotNbr rtn %u\n", LCPT_MyId,
		LCP_PhysSlotNbr ()) ;

	printf ("%s LCP_NMP_PhysSlotNbr rtn %u\n", LCPT_MyId,
		LCP_NMP_PhysSlotNbr ()) ;

	printf ("%s LCP_CardInstanceNbr rtn %u\n", LCPT_MyId,
		LCP_CardInstanceNbr ()) ;

	printf ("%s LCP_ReadMacAddr rtn $%8x (given $%8x)\n", LCPT_MyId,
		LCP_ReadMacAddr (MyMac.byte), MyMac.byte) ;

	printf ("%s LCP MAC addr %02x:%02x:%02x:%02x:%02x:%02x\n", LCPT_MyId,
		MyMac.byte [0], MyMac.byte [1], MyMac.byte [2],
		MyMac.byte [3], MyMac.byte [4], MyMac.byte [5]) ;

	printf ("%s LCP_NMP_MacAddr rtn $%8x (given $%8x)\n", LCPT_MyId,
		LCP_NMP_MacAddr (MyMac.byte), MyMac.byte) ;

	printf ("%s NMP MAC addr %02x:%02x:%02x:%02x:%02x:%02x\n", LCPT_MyId,
		MyMac.byte [0], MyMac.byte [1], MyMac.byte [2],
		MyMac.byte [3], MyMac.byte [4], MyMac.byte [5]) ;

	printf ("%s LCP_Ntwk_MacAddr rtn $%8x (given $%8x)\n", LCPT_MyId,
		LCP_Ntwk_MacAddr (MyMac.byte, & Val), MyMac.byte) ;

	printf ("%s Ntwk MACs block of %u at %02x:%02x:%02x:%02x:%02x:%02x\n",
		LCPT_MyId, Val,
		MyMac.byte [0], MyMac.byte [1], MyMac.byte [2],
		MyMac.byte [3], MyMac.byte [4], MyMac.byte [5]) ;

	LCPT_EchoMsgTest (RepeatCount) ;

	memset (& LCPT_DvrInfo, 0, sizeof (LCPT_DvrInfo)) ;
	printf ("%s LCP_GetDriverInfo rtn $%8x (given $%8x)\n", LCPT_MyId,
		LCP_GetDriverInfo (& LCPT_DvrInfo), & LCPT_DvrInfo) ;

	printf ("%s Dvr Tx Counts Msg = %u Ack = %u Drp = %u Err = %u\n",
		LCPT_MyId, LCPT_DvrInfo.Tx.MsgCount, LCPT_DvrInfo.Tx.AckCount,
		LCPT_DvrInfo.Tx.DrpCount, LCPT_DvrInfo.Tx.ErrCount) ;

	printf ("%s Dvr Rx Counts Msg = %u Ack = %u Drp = %u Err = %u\n",
		LCPT_MyId, LCPT_DvrInfo.Rx.MsgCount, LCPT_DvrInfo.Rx.AckCount,
		LCPT_DvrInfo.Rx.DrpCount, LCPT_DvrInfo.Rx.ErrCount) ;

	printf ("%s All done !!\n\n", LCPT_MyId) ;

}	/* LCPT_TestIt */
