/* $Id: mfp.c,v 1.1.4.1 1996/05/09 14:55:01 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/mfp.c,v $
 *-----------------------------------------------------------------
 * mfp.c -- 68901 MFP low-level driver and interrupt service routines.
 *		(Based on Catalyst code.)
 *
 * December 8, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mfp.c,v $
 * Revision 1.1.4.1  1996/05/09  14:55:01  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:29  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:34  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:28  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:57  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.16  1995/10/20  05:04:49  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_hw_defs.h"
#include "acp_c_defs.h"
#include "mfp_acp_defs.h"


extern void	_rbf_hdlr () ;
extern void	_re_hdlr  () ;
extern void	_tbe_hdlr () ;
extern void	_te_hdlr  () ;
extern void	_timer_a_hdlr () ;
extern void	_timer_b_hdlr () ;
extern void	_timer_c_hdlr () ;


typedef union MFP_ShortBytesU
{
	u_int16	W ;
	struct
	{
		u_int8	hi ;	/* Assume big-endian. */
		u_int8	lo ;
	} B ;
} tMFP_ShortBytesU ;



/* --------------------------------------------------------------------	*/
/*	This clears the Pending and In-Service bits at the MFP for a	*/
/*	set of interrupt sources.  A separate flag indicates whether or	*/
/*	not interrupt In-Service bits should be touched.		*/
/* --------------------------------------------------------------------	*/
void MFP_ClrPendIntr (u_int16 BitMask, u_int8 fClrInSrvc)
{
	register volatile u_int8 *	pReg ;
	tMFP_ShortBytesU		Mask ;
	u_int32				Old_SR ;


	pReg   = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Mask.W = ~ BitMask ;
	Old_SR = spl7 () ;

		/* Clear In-Service, then Pending, MFP bits. */
	if (fClrInSrvc)
	{
		pReg [MFP_RO_ISRA] = Mask.B.hi ;
		pReg [MFP_RO_ISRB] = Mask.B.lo ;
	}

	pReg [MFP_RO_IPRA] = Mask.B.hi ;
	pReg [MFP_RO_IPRB] = Mask.B.lo ;

	splx (Old_SR) ;

}	/* MFP_ClrPendIntr */


/* --------------------------------------------------------------------	*/
/*	This enables a set of interrupt sources at the MFP interrupt	*/
/*	controller.  These sources are identified with a bitmask that	*/
/*	follows the MFP interrupt register definitions.  A separate	*/
/*	flag indicates whether or not MFP Pending and In-Service bits	*/
/*	should be cleared.  This routine does NOT unmask the interrupt	*/
/*	sources at the MFP.						*/
/* --------------------------------------------------------------------	*/
void MFP_EnableIntrW (u_int16 BitMask, u_int8 fClrPending)
{
	register volatile u_int8 *	pReg ;
	tMFP_ShortBytesU		Mask ;
	u_int32				Old_SR ;


	pReg   = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Mask.W = BitMask ;
	Old_SR = spl7 () ;

	if (fClrPending)
	{
		MFP_ClrPendIntr (BitMask, TRUE) ;
	}

		/* Enable the interrupt sources, but do NOT unmask them. */
	pReg [MFP_RO_IERA] |= Mask.B.hi ;
	pReg [MFP_RO_IERB] |= Mask.B.lo ;

	splx (Old_SR) ;

}	/* MFP_EnableIntrW */


/* --------------------------------------------------------------------	*/
/*	This disables a set of interrupt sources at the MFP interrupt	*/
/*	controller.  These sources are identified with a bitmask that	*/
/*	follows the MFP interrupt register definitions.  This routine	*/
/*	does NOT mask the interrupt sources at the MFP.			*/
/* --------------------------------------------------------------------	*/
void MFP_DisableIntrW (u_int16 BitMask)
{
	register volatile u_int8 *	pReg ;
	tMFP_ShortBytesU		Mask ;
	u_int32				Old_SR ;


	pReg   = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Mask.W = ~ BitMask ;
	Old_SR = spl7 () ;

		/* Disable the interrupt sources, but do NOT mask them. */
	pReg [MFP_RO_IERA] &= Mask.B.hi ;
	pReg [MFP_RO_IERB] &= Mask.B.lo ;

	MFP_ClrPendIntr (BitMask, TRUE) ;

	splx (Old_SR) ;

}	/* MFP_DisableIntrW */


/* --------------------------------------------------------------------	*/
/*	This unmasks a set of interrupt sources at the MFP interrupt	*/
/*	controller.  These sources are identified with a bitmask that	*/
/*	follows the MFP interrupt register definitions.  Separate	*/
/*	flags indicate if the sources should be enabled at the MFP,	*/
/*	and whether or not MFP Pending and In-Service bits should be	*/
/*	cleared.							*/
/* --------------------------------------------------------------------	*/
void MFP_UnmaskIntrW (u_int16 BitMask, u_int8 fEnableIntr, u_int8 fClrPending)
{
	register volatile u_int8 *	pReg ;
	tMFP_ShortBytesU		Mask ;
	u_int32				Old_SR ;


	pReg   = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Mask.W = BitMask ;
	Old_SR = spl7 () ;

	if (fEnableIntr)
	{
		MFP_EnableIntrW (BitMask, fClrPending) ;
	}

		/* Unmask the interrupt sources. */
	pReg [MFP_RO_IMRA] |= Mask.B.hi ;
	pReg [MFP_RO_IMRB] |= Mask.B.lo ;

	splx (Old_SR) ;

}	/* MFP_UnmaskIntrW */


/* --------------------------------------------------------------------	*/
/*	This masks a set of interrupt sources at the MFP interrupt	*/
/*	controller.  These sources are identified with a bitmask that	*/
/*	follows the MFP interrupt register definitions.  A separate	*/
/*	flag indicates if the sources should be disabled at the MFP,	*/
/*	and whether or not MFP Pending and In-Service bits should be	*/
/*	cleared.							*/
/* --------------------------------------------------------------------	*/
void MFP_MaskIntrW (u_int16 BitMask, u_int8 fDisableIntr)
{
	register volatile u_int8 *	pReg ;
	tMFP_ShortBytesU		Mask ;
	u_int32				Old_SR ;


	pReg   = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Mask.W = ~ BitMask ;
	Old_SR = spl7 () ;

		/* Mask the interrupt sources. */
	pReg [MFP_RO_IMRA] &= Mask.B.hi ;
	pReg [MFP_RO_IMRB] &= Mask.B.lo ;

	if (fDisableIntr)
	{
		MFP_DisableIntrW (BitMask) ;
	}

	splx (Old_SR) ;

}	/* MFP_MaskIntrW */


/* --------------------------------------------------------------------	*/
/*	This polls the MFP interrupt controller for a set of interrupt	*/
/*	sources.  When done it returns a bitmask for the MFP pending	*/
/*	register contents.  Separate flags indicate if the interrupt	*/
/*	sources should be disabled when done, if the poll is continuous	*/
/*	or single-pass, and if a match is needed on any or all of the	*/
/*	given interrupt sources.					*/
/*									*/
/*	In any case, this routine always feeds the HW watchdog.		*/
/* --------------------------------------------------------------------	*/
u_int16 MFP_PollForIntr (register u_int16 BitMask,    u_int8 fDisableDone,
			 register u_int8 fSingleLoop, register u_int8 fAny)
{
	register volatile u_int8 *	pReg ;
	tMFP_ShortBytesU		Read ;
	tMFP_ShortBytesU		Seen ;
	u_int8				fMatch ;


	pReg   = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Read.W = MFP_IMSK_NONE ;		/* 0x0000 */
	Seen.W = MFP_IMSK_NONE ;		/* 0x0000 */
	fMatch = FALSE ;

		/* Watch for no interrupt sources selected. */
	if (BitMask == MFP_IMSK_NONE)		/* 0x0000 */
	{
		PUNCH_WD ;			/* Always feed watchdog. */
		return (BitMask) ;
	}

		/* Mask the interrupt sources, then enable them.  If we	*/
		/* are in disable-done mode, disable and clear sources	*/
		/* before enabling them.  This gives a clean window in	*/
		/* which new interrupts can fire.  Otherwise, we do NOT	*/
		/* clear any pending interrupts and will accept any may	*/
		/* have occurred in the past.				*/
	MFP_MaskIntrW   (BitMask, fDisableDone) ;
	MFP_EnableIntrW (BitMask, fDisableDone) ;


		/* Inner loop polls for any relevant MFP activity.     */
		/* Outer loop checks net results against requirements. */
	do {
		do {
			Read.B.hi = pReg [MFP_RO_IPRA] ;
			Read.B.lo = pReg [MFP_RO_IPRB] ;
			Read.W   &= BitMask ;	/* Isolate relevant bits. */
			PUNCH_WD ;		/* Keep a happy watchdog. */
		} while ((! fSingleLoop) && (! Read.W)) ;

		Seen.W |= Read.W ;		/* Acquire partial results. */

		fMatch = ((fAny) ? (Seen.W != MFP_IMSK_NONE) :
					(Seen.W == BitMask)) ;

	} while ((! fSingleLoop) && (! fMatch)) ;


		/* If we are in disable-done mode, disable and clear	*/
		/* the interrupts we saw.  Otherwise, just clear them.	*/
	if (Seen.W != MFP_IMSK_NONE)
	{
		if (fDisableDone)
			MFP_DisableIntrW (Seen.W) ;
		else
			MFP_ClrPendIntr (Seen.W, TRUE) ;
	}


	return (Seen.W) ;

}	/* MFP_PollForIntr */


/* --------------------------------------------------------------------	*/
/*	This binds a service routine to a MFP interrupt vector.		*/
/*	The corresponding interrupt source is enabled at the MFP.	*/
/*	This allows polling routines to check Interrupt Pending bits.	*/
/*	However, the interrupt source is still masked off at the MFP.	*/
/* --------------------------------------------------------------------	*/
void MFP_BindEnb (void (** VecTbl)(), void (* pISR)(), u_int8 BaseVec, u_int8 Which)
{
	register volatile u_int8 *	pReg ;
	register u_int16		Mask ;
	u_int32				Old_SR ;


	PUNCH_WD ;

	pReg = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	Old_SR = spl7 () ;

	if ((VecTbl != NULL) && (pISR != NULL))
	{
		VecTbl [ BaseVec + Which ] = pISR ;
	}

	Mask = (1 << Which) ;			/* Set selected bit to 1. */
	MFP_MaskIntrW (Mask, FALSE) ;
	MFP_EnableIntrW (Mask, TRUE) ;

	splx (Old_SR) ;

}	/* MFP_BindEnb */


/* --------------------------------------------------------------------	*/
/*	This does a re-init of the MFP I/O pins and interrupt control.	*/
/* --------------------------------------------------------------------	*/
void MFP_InitIntr (u_int8 fResetMFP, u_int8 BaseVec, u_int8 fInSvc)
{
	register volatile u_int8 *	pReg ;
	u_int32				Old_SR ;
	s_int16				Count ;

		/* Disable interrupt sources while we adjust MFP. */
	PUNCH_WD ;
	Old_SR = spl7 () ;

		/* MFP Reset needs about 2 usec to take effect. */
	pReg = (volatile u_int8 *)(ACP_HW_IOREG_BASE) ;
	if (fResetMFP)
	{
		pReg [ACP_WRRG_MFP_RESET_] = ON_ ;	/* Reset active low. */

		for (Count = 0 ; Count < 10 ; )
			{ ++ Count ; }

	}
	pReg [ACP_WRRG_MFP_RESET_] = OFF_ ;		/* Reset active low. */

		/* Now get access to MFP registers. */
	pReg = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;

	pReg [MFP_RO_VR  ] = MFP_VECREG_SET (BaseVec, fInSvc) ;
	MFP_MaskIntrW (MFP_IMSK_ALL, TRUE) ;

	pReg [MFP_RO_DDR ] = MFP_TGT_DDR  ;
	pReg [MFP_RO_AER ] = MFP_TGT_AER  ;
	pReg [MFP_RO_GPDR] = MFP_TGT_GPDR ;		/* Do GPIO write   */
	Count = pReg [MFP_RO_GPDR] ;			/* then GPIO read. */

	splx (Old_SR) ;
	PUNCH_WD ;

}	/* MFP_InitIntr */


/* --------------------------------------------------------------------	*/
/*	This does a re-init of the MFP Timer functionality.		*/
/*	Timer D is used as a Baud Rate Generator and is not		*/
/*	accessed here.							*/
/* --------------------------------------------------------------------	*/
void Timer_HW_Init (void (** VecTbl)(), u_int8 BaseVec, u_int8 fPoll_A, u_int8 fPoll_B, u_int8 fPoll_C)
{
	register volatile u_int8 *	pReg ;
	u_int32				Old_SR ;
	u_int8				Temp ;

		/* ---------------------------------------------- */
		/* Disable interrupt sources while we adjust MFP. */
		/* ---------------------------------------------- */
	PUNCH_WD ;
	Old_SR = spl7 () ;

	pReg = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;

		/* --------------------------------- */
		/* Disable all available MFP timers. */
		/* --------------------------------- */
	pReg [MFP_RO_TACR ] = MFP_TMR_STOPPED ;
	pReg [MFP_RO_TBCR ] = MFP_TMR_STOPPED ;
	pReg [MFP_RO_TCDCR] = ((pReg [MFP_RO_TCDCR] & 0x0f) |
				(MFP_TMR_CTL_C (MFP_TMR_STOPPED))) ;

		/* Disable and mask these MFP interrupt sources. */
	MFP_MaskIntrW ((MFP_4MS_TMR_BITMASK  |
			MFP_10MS_TMR_BITMASK |
			MFP_HWTOUT_TMR_BITMASK), TRUE) ;

		/* ----------------------------------------------------	*/
		/*	Timer A is used for IOS 4 ms tick interrupt.	*/
		/*	Timer B is used for Boot 10 ms tick interrupt	*/
		/*	Timer C is used for HW (driver) timeout checks.	*/
		/*	(Timer D is used below as Baud Rate Generator.)	*/
		/* ----------------------------------------------------	*/
	pReg [MFP_RO_TADR] = MFP_4MS_TMR_COUNTER  ;
	pReg [MFP_RO_TBDR] = MFP_10MS_TMR_COUNTER ;
	pReg [MFP_RO_TCDR] = MFP_10MS_TMR_COUNTER ;	/* Poll HW timeouts. */

	MFP_BindEnb (VecTbl, _timer_a_hdlr, BaseVec, MFP_IINDX_TMRA) ;
	MFP_BindEnb (VecTbl, _timer_b_hdlr, BaseVec, MFP_IINDX_TMRB) ;
	MFP_BindEnb (VecTbl, _timer_c_hdlr, BaseVec, MFP_IINDX_TMRC) ;

	if (! fPoll_A)
	{
		MFP_UnmaskIntrW (MFP_4MS_TMR_BITMASK, TRUE, TRUE) ;
	}
	if (! fPoll_B)
	{
		MFP_UnmaskIntrW (MFP_10MS_TMR_BITMASK, TRUE, TRUE) ;
	}
	if (! fPoll_C)
	{
		MFP_UnmaskIntrW (MFP_HWTOUT_TMR_BITMASK, TRUE, TRUE) ;
	}

	pReg [MFP_RO_TACR ] = MFP_TMR_CTL_AB (MFP_4MS_TMR_PSCALE,  0, 0) ;
	pReg [MFP_RO_TBCR ] = MFP_TMR_CTL_AB (MFP_10MS_TMR_PSCALE, 0, 0) ;

#if	0
		/* Timer C, for HW (driver) timeout polling, is only	*/
		/* activated when needed.				*/
	pReg [MFP_RO_TCDCR] = ((pReg [MFP_RO_TCDCR] & 0x0f) |
				(MFP_TMR_CTL_C (MFP_10MS_TMR_PSCALE))) ;
#endif

	splx (Old_SR) ;

}	/* Timer_HW_Init */


/* --------------------------------------------------------------------	*/
/*	This does a re-init of the MFP UART functionality.		*/
/*	Since Timer D is used as a Baud Rate Generator it is		*/
/*	also initialized here.						*/
/* --------------------------------------------------------------------	*/
void Uart_HW_Init (void (** VecTbl)(), u_int8 BaseVec, u_int8 fPoll)
{
	register volatile u_int8 *	pReg ;
	u_int32				Old_SR ;
	u_int8				Temp ;

		/* ---------------------------------------------- */
		/* Disable interrupt sources while we adjust MFP. */
		/* ---------------------------------------------- */
	PUNCH_WD ;
	Old_SR = spl7 () ;

	pReg = (volatile u_int8 *)(ACP_HW_MFP_BASE) ;

		/* ------------------------------------------------------ */
		/* Disable UART, then drain any pending UART status/data. */
		/* ------------------------------------------------------ */
	pReg [MFP_RO_RSR] = OFF ;
	pReg [MFP_RO_TSR] = OFF ;

	Temp = pReg [MFP_RO_RSR] ;
	Temp = pReg [MFP_RO_UDR] ;
	Temp = pReg [MFP_RO_TSR] ;

		/* ----------------------------------------------------- */
		/* Disable and program BRG (Timer D).  Note that clock   */
		/* prescaler is 4, resulting clock output is toggled on  */
		/* countdown.  Thus countdown period is one-HALF desired */
		/* full clock period.  Set BRG for 9600 Baud.            */
		/* Do not allow interrupts on the BRG.                   */
		/* ----------------------------------------------------- */
	pReg [MFP_RO_TCDCR] = ((pReg [MFP_RO_TCDCR] & 0xf0) |
				(MFP_TMR_CTL_D (MFP_TMR_STOPPED))) ;
	pReg [MFP_RO_TDDR ] = (ACP_HW_MFP_CLK_HZ / MFP_BAUD_9600 /
				MFP_UART_CLK_DIV_16 / 4 / 2) ;

		/* Disable and mask this MFP interrupt source. */
	MFP_MaskIntrW (MFP_BRG_TMR_BITMASK, TRUE) ;

	pReg [MFP_RO_TCDCR] = ((pReg [MFP_RO_TCDCR] & 0xf0) |
				(MFP_TMR_CTL_D (MFP_TMR_PSCALE_4))) ;

		/* -------------------------------------------------------- */
		/* Enable UART.  Use 8-bit data, 1 stop bit, BRG div-by-16. */
		/* Bind and unmask UART interrupt handlers as indicated.    */
		/* Do NOT use the MFP separate Rx/Tx error capability.      */
		/* -------------------------------------------------------- */
	pReg [MFP_RO_SCR] = 0x00 ;
	pReg [MFP_RO_UCR] = MFP_UCR_SETVAL (1, MFP_UCR_CHLEN_8,
						MFP_UCR_CHSTOP_1, 0, 0) ;

	MFP_BindEnb (VecTbl, _rbf_hdlr, BaseVec, MFP_IINDX_RXFL) ;
	MFP_BindEnb (VecTbl, _tbe_hdlr, BaseVec, MFP_IINDX_TXBE) ;
#if	0
	MFP_BindEnb (VecTbl, _re_hdlr,  BaseVec, MFP_IINDX_RXER) ;
	MFP_BindEnb (VecTbl, _te_hdlr,  BaseVec, MFP_IINDX_TXER) ;
#endif

	if (! fPoll)
	{
		MFP_UnmaskIntrW (((1 << MFP_IINDX_RXFL) |
#if	0
				  (1 << MFP_IINDX_RXER) |
				  (1 << MFP_IINDX_TXER) |
#endif
				  (1 << MFP_IINDX_TXBE)), TRUE, TRUE) ;
	}

		/* ------------------------------------------ */
		/* RKL:  Could add a loopback test in future. */
		/* ------------------------------------------ */
	pReg [MFP_RO_TSR] = (MFP_TSR_TX_ENABLE | MFP_TSR_TXSO_HIGH) ;
	pReg [MFP_RO_RSR] = MFP_RSR_RX_ENABLE ;

	splx (Old_SR) ;

}	/* Uart_HW_Init */


/* --------------------------------------------------------------------	*/
/*	This does a re-init of all MFP functional areas.		*/
/* --------------------------------------------------------------------	*/
void MFP_InitAll (u_int8 fResetMFP, void (** VecTbl)(), u_int8 BaseVec,
		  u_int8 fInSvc, u_int8 fPoll_A, u_int8 fPoll_B,
		  u_int8 fPoll_C, u_int8 fPoll_U)
{
	PUNCH_WD ;

	MFP_InitIntr  (fResetMFP, BaseVec, fInSvc) ;
	Timer_HW_Init (VecTbl, BaseVec, fPoll_A, fPoll_B, fPoll_C) ;
	Uart_HW_Init  (VecTbl, BaseVec, fPoll_U) ;
	MFP_HwtInit   (fPoll_C) ;

}	/* MFP_InitAll */


/* --------------------------------------------------------------------	*/
/*	ISRs for timer interrupts.					*/
/* --------------------------------------------------------------------	*/
void c_timer_A_hdlr ()
{
}

void c_timer_B_hdlr ()
{
}


/* --------------------------------------------------------------------	*/
/*	This polls the MFP I/O lines for a set pattern.  A bitmask is	*/
/*	used to filter out "don't care" bits.  (A bit must be set in	*/
/*	the bitmask for the corresponding data bit to be checked.)	*/
/*	Once a pattern match occurs (don't care bits should be 0 in	*/
/*	the pattern bitmask) the entire, unmasked set of MFP I/O data	*/
/*	bits is returned.  Separate flags indicate whether to poll	*/
/*	just once, and whether a pattern match must be exact.		*/
/*									*/
/*	This routine always feeds the HW watchdog.			*/
/* --------------------------------------------------------------------	*/
u_int8 MFP_Poll_IOs (u_int8 PatBits, u_int8 MskBits, u_int8 fOnce, u_int8 fAny)
{
	register volatile u_int8 *	pReg ;
	u_int8				fMatch ;
	u_int8				DatRead ;
	u_int8				DatSeen ;


	pReg	= (volatile u_int8 *)(ACP_HW_MFP_BASE) ;
	fMatch	= FALSE ;
	DatRead	= 0 ;
	DatSeen	= 0 ;

	PatBits &= MskBits ;			/* Groom input pattern. */


		/* Loop to poll and check results against requirements. */
	do {
		DatRead = pReg [MFP_RO_GPDR] ;
		PUNCH_WD ;			/* Keep a happy watchdog. */

		DatSeen = (DatRead & MskBits) ;	/* Isolate relevant bits. */

		fMatch = ((fAny) ? ((~ (DatSeen ^ PatBits)) & MskBits) :
					(DatSeen == PatBits)) ;

	} while ((! fOnce) && (! fMatch)) ;


	return (DatRead) ;

}	/* MFP_Poll_IOs */
