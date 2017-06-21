/* $Id: mfp_hw_defs.h,v 1.1.4.1 1996/05/09 14:55:42 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_include/mfp_hw_defs.h,v $
 *-----------------------------------------------------------------
 * mfp_hw_defs.h -- ACP 68901 (MFP) HW address map and definitions.
 *			Describe various MFP registers here.
 *			This file is used with both assembly and C.
 *
 * December 7, 1994, Rick Lowe
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mfp_hw_defs.h,v $
 * Revision 1.1.4.1  1996/05/09  14:55:42  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:15:12  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:40:09  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:11:21  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:42:13  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:40:41  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.15  1995/10/20  05:04:53  rlowe
 * Fixes for early LCP reset release, so that ACP does not generate
 * unintended interrupts to the LCP.  Also clean up any stray LCP
 * RxDone interrupts to the ACP once the Module Ready startup message
 * is found.  Fix the LCP driver behaviour if HW guard timeout occurs
 * during Tx to LCP.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__MFP_HW_DEFS_H
#define	__MFP_HW_DEFS_H

	/* ====================================================	*/
	/* 	MFP Register Offsets.  I'd prefer to use an	*/
	/* 	enum but assembly would not understand it.	*/
	/* ====================================================	*/

#define	MFP_RO_GPDR		(0x00)
#define	MFP_RO_AER		(MFP_RO_GPDR + 1)
#define	MFP_RO_DDR		(MFP_RO_AER  + 1)
#define	MFP_RO_IERA		(MFP_RO_DDR  + 1)
#define	MFP_RO_IERB		(MFP_RO_IERA + 1)
#define	MFP_RO_IPRA		(MFP_RO_IERB + 1)
#define	MFP_RO_IPRB		(MFP_RO_IPRA + 1)
#define	MFP_RO_ISRA		(MFP_RO_IPRB + 1)
#define	MFP_RO_ISRB		(MFP_RO_ISRA + 1)
#define	MFP_RO_IMRA		(MFP_RO_ISRB + 1)
#define	MFP_RO_IMRB		(MFP_RO_IMRA + 1)
#define	MFP_RO_VR		(MFP_RO_IMRB + 1)
#define	MFP_RO_TACR		(MFP_RO_VR   + 1)
#define	MFP_RO_TBCR		(MFP_RO_TACR + 1)
#define	MFP_RO_TCDCR		(MFP_RO_TBCR + 1)
#define	MFP_RO_TADR		(MFP_RO_TCDCR+ 1)
#define	MFP_RO_TBDR		(MFP_RO_TADR + 1)
#define	MFP_RO_TCDR		(MFP_RO_TBDR + 1)
#define	MFP_RO_TDDR		(MFP_RO_TCDR + 1)
#define	MFP_RO_SCR		(MFP_RO_TDDR + 1)
#define	MFP_RO_UCR		(MFP_RO_SCR  + 1)
#define	MFP_RO_RSR		(MFP_RO_UCR  + 1)
#define	MFP_RO_TSR		(MFP_RO_RSR  + 1)
#define	MFP_RO_UDR		(MFP_RO_TSR  + 1)

	/* ============================================================	*/
	/*	I/O pin bit definitions for General Purpose I/O Data	*/
	/*	Register (GPDR), Active Edge Register (AER), and Data	*/
	/*	Direction Register (DDR).				*/
	/* ============================================================	*/

#define	MFP_GPIP0_BIT		(0x01)
#define	MFP_GPIP1_BIT		(MFP_GPIP0_BIT << 1)
#define	MFP_GPIP2_BIT		(MFP_GPIP1_BIT << 1)
#define	MFP_GPIP3_BIT		(MFP_GPIP2_BIT << 1)
#define	MFP_GPIP4_BIT		(MFP_GPIP3_BIT << 1)
#define	MFP_GPIP5_BIT		(MFP_GPIP4_BIT << 1)
#define	MFP_GPIP6_BIT		(MFP_GPIP5_BIT << 1)
#define	MFP_GPIP7_BIT		(MFP_GPIP6_BIT << 1)

	/* ============================================================	*/
	/*	Register bit definitions for Interrupt Enable Register	*/
	/*	A/B (IERA/B), Interrupt Mask Register A/B (IMRA/B),	*/
	/*	Interrupt Pending Register A/B (IPRA/B), Interrupt	*/
	/*	In-Service Register A/B (ISRA/B).			*/
	/* ============================================================	*/

#define	MFP_IINDX_GPI7		(0x0F)
#define	MFP_IINDX_GPI6		(0x0E)
#define	MFP_IINDX_TMRA		(0x0D)
#define	MFP_IINDX_RXFL		(0x0C)
#define	MFP_IINDX_RXER		(0x0B)
#define	MFP_IINDX_TXBE		(0x0A)
#define	MFP_IINDX_TXER		(0x09)
#define	MFP_IINDX_TMRB		(0x08)
#define	MFP_IINDX_GPI5		(0x07)
#define	MFP_IINDX_GPI4		(0x06)
#define	MFP_IINDX_TMRC		(0x05)
#define	MFP_IINDX_TMRD		(0x04)
#define	MFP_IINDX_GPI3		(0x03)
#define	MFP_IINDX_GPI2		(0x02)
#define	MFP_IINDX_GPI1		(0x01)
#define	MFP_IINDX_GPI0		(0x00)

#define	MFP_IMSK_ALL		(0xffff)
#define	MFP_IMSK_RXFL		(1 << MFP_IINDX_RXFL)
#define	MFP_IMSK_RXER		(1 << MFP_IINDX_RXER)
#define	MFP_IMSK_TXBE		(1 << MFP_IINDX_TXBE)
#define	MFP_IMSK_TXER		(1 << MFP_IINDX_TXER)
#define	MFP_IMSK_NONE		(0x0000)

#define	MFP_INTR_TMR_B		(0x01)			/* For A registers. */
#define	MFP_INTR_T_ERR		(MFP_INTR_TMR_B << 1)
#define	MFP_INTR_T_EMP		(MFP_INTR_T_ERR << 1)
#define	MFP_INTR_R_ERR		(MFP_INTR_T_EMP << 1)
#define	MFP_INTR_R_FUL		(MFP_INTR_R_ERR << 1)
#define	MFP_INTR_TMR_A		(MFP_INTR_R_FUL << 1)
#define	MFP_INTR_GPIP6		(MFP_INTR_TMR_A << 1)
#define	MFP_INTR_GPIP7		(MFP_INTR_GPIP6 << 1)

#define	MFP_INTR_GPIP0		(0x01)			/* For B registers. */
#define	MFP_INTR_GPIP1		(MFP_INTR_GPIP0 << 1)
#define	MFP_INTR_GPIP2		(MFP_INTR_GPIP1 << 1)
#define	MFP_INTR_GPIP3		(MFP_INTR_GPIP2 << 1)
#define	MFP_INTR_TMR_D		(MFP_INTR_GPIP3 << 1)
#define	MFP_INTR_TMR_C		(MFP_INTR_TMR_D << 1)
#define	MFP_INTR_GPIP4		(MFP_INTR_TMR_C << 1)
#define	MFP_INTR_GPIP5		(MFP_INTR_GPIP4 << 1)

#define	MFP_NBR_UART_IVECS	(4)

	/* ============================================================	*/
	/*	Useful definitions for the MFP Vector Register.		*/
	/* ============================================================	*/

#define	MFP_VECREG_INSVC_BIT	(0x08)

#define	MFP_VECREG_SET(basevec,fInService) \
				(((basevec) & 0x00f0) + \
				 ((fInService) * MFP_VECREG_INSVC_BIT))

	/* ============================================================	*/
	/*	Register bit(-field) definitions for Timer Control	*/
	/*	Registers.  TACR/TBCR are identical but different	*/
	/*	from TCDCR.  However, the delay timer prescaler bit	*/
	/*	patterns are common.  MFP data sheet has more details.	*/
	/* ============================================================	*/

#define	MFP_TMR_STOPPED		(0x0)
#define	MFP_TMR_PSCALE_4	(MFP_TMR_STOPPED    + 1)
#define	MFP_TMR_PSCALE_10	(MFP_TMR_PSCALE_4   + 1)
#define	MFP_TMR_PSCALE_16	(MFP_TMR_PSCALE_10  + 1)
#define	MFP_TMR_PSCALE_50	(MFP_TMR_PSCALE_16  + 1)
#define	MFP_TMR_PSCALE_64	(MFP_TMR_PSCALE_50  + 1)
#define	MFP_TMR_PSCALE_100	(MFP_TMR_PSCALE_64  + 1)
#define	MFP_TMR_PSCALE_200	(MFP_TMR_PSCALE_100 + 1)

#define	MFP_TMR_EV_PULSE_FLAG	(0x08)
#define	MFP_TMR_RESET_OUT_FLG	(0x10)

#define	MFP_TMR_CLEAN_PSCALE(v)	((v) & 0x07)

#define	MFP_TMR_CTL_AB(nibble,fEvPulse,fResetOut) \
				((MFP_TMR_CLEAN_PSCALE (nibble)) + \
				 ((fEvPulse) * MFP_TMR_EV_PULSE_FLAG) + \
				 ((fResetOut) * MFP_TMR_RESET_OUT_FLG))

#define	MFP_TMR_CTL_C(nibble)	((MFP_TMR_CLEAN_PSCALE (nibble)) << 4)
#define	MFP_TMR_CTL_D(nibble)	(MFP_TMR_CLEAN_PSCALE (nibble))

#define	MFP_TMR_MAX_COUNTER	(0x00ff)	/* max timer count */

	/* ============================================================	*/
	/*	UART Control Register (UCR) bit definitions.		*/
	/* ============================================================	*/

#define	MFP_UCR_EVEN_PARITY	(1 << 1)
#define	MFP_UCR_PARITY_ENB	(1 << 2)

#define	MFP_UCR_CH_SYNCH	(0 << 3)	/* synch */
#define	MFP_UCR_CHSTOP_1	(1 << 3)	/* async, 1 stop bit */
#define	MFP_UCR_CHSTOP_1_5	(2 << 3)	/* async, 1.5 stop bits */
#define	MFP_UCR_CHSTOP_2	(3 << 3)	/* async, 2 stop bits */

#define	MFP_UCR_CHLEN_8		(0 << 5)	/* 8 bits */
#define	MFP_UCR_CHLEN_7		(1 << 5)	/* 7 bits */
#define	MFP_UCR_CHLEN_6		(2 << 5)	/* 6 bits */
#define	MFP_UCR_CHLEN_5		(3 << 5)	/* 5 bits */

#define	MFP_UCR_CLK_16X		(1 << 7)
#define	MFP_UART_CLK_DIV_16	16

#define	MFP_UCR_SETVAL(f16xClk,ChLen,StopBits,fParity,fEvenPar) \
				(((f16xClk) * MFP_UCR_CLK_16X) + \
				 ((ChLen) & 0x60) + ((StopBits) & 0x18) + \
				 ((fParity) * MFP_UCR_PARITY_ENB) + \
				 ((fEvenPar) * MFP_UCR_EVEN_PARITY))

	/* ============================================================	*/
	/*	UART Receiver Status Register (RSR) bit definitions.	*/
	/* ============================================================	*/

#define	MFP_RSR_DEPTH		(2)

#define	MFP_RSR_RX_ENABLE	(0x01)
#define	MFP_RSR_SYNC_STRIP	(MFP_RSR_RX_ENABLE    << 1)
#define	MFP_RSR_CHAR_IN_PROG	(MFP_RSR_SYNC_STRIP   << 1)
#define	MFP_RSR_BREAK_DETECT	(MFP_RSR_CHAR_IN_PROG << 1)
#define	MFP_RSR_FRAME_ERR	(MFP_RSR_BREAK_DETECT << 1)
#define	MFP_RSR_PARITY_ERR	(MFP_RSR_FRAME_ERR    << 1)
#define	MFP_RSR_OVERRUN_ERR	(MFP_RSR_PARITY_ERR   << 1)
#define	MFP_RSR_BUFFER_FULL	(MFP_RSR_OVERRUN_ERR  << 1)

#define	MFP_RSR_RX_ERR_MSK	(MFP_RSR_OVERRUN_ERR + MFP_RSR_PARITY_ERR + \
				 MFP_RSR_FRAME_ERR   + MFP_RSR_BREAK_DETECT)

#define	MFP_RSR_RX_EVNTMSK	(MFP_RSR_BUFFER_FULL + MFP_RSR_RX_ERR_MSK)

	/* ============================================================	*/
	/*	UART Transmitter Status Register (TSR) bit definitions.	*/
	/* ============================================================	*/

#define	MFP_TSR_TX_ENABLE	(0x01)

#define	MFP_TSR_TXSO_HI_IMP	(0 << 1)
#define	MFP_TSR_TXSO_LOW	(1 << 1)
#define	MFP_TSR_TXSO_HIGH	(2 << 1)
#define	MFP_TSR_TXSO_LOOP	(3 << 1)

#define	MFP_TSR_TX_BREAK	(MFP_TSR_TX_ENABLE  << 3)
#define	MFP_TSR_TX_OFF_EOT	(MFP_TSR_TX_BREAK   << 1)
#define	MFP_TSR_AUTO_TURN	(MFP_TSR_TX_OFF_EOT << 1)
#define	MFP_TSR_UNDERRUN	(MFP_TSR_AUTO_TURN  << 1)
#define	MFP_TSR_BUF_EMPTY	(MFP_TSR_UNDERRUN   << 1)

#define	MFP_TSR_TX_EXC_MSK	(MFP_TSR_UNDERRUN  + MFP_TSR_TX_OFF_EOT)
#define	MFP_TSR_TX_EVNTMSK	(MFP_TSR_BUF_EMPTY + MFP_TSR_TX_EXC_MSK)


#ifndef	ASM_MODE_FLG		/* Prototypes for C code only. */
#include "acp_basic_types.h"

#define MFPHWT_Cback(n) void (* n) (u_int32)

extern void MFP_ClrPendIntr (u_int16 BitMask, u_int8 fClrInSrvc) ;
extern void MFP_EnableIntrW (u_int16 BitMask, u_int8 fClrPending) ;
extern void MFP_DisableIntrW (u_int16 BitMask) ;
extern void MFP_UnmaskIntrW (u_int16 BitMask, u_int8 fEnableIntr,
							u_int8 fClrPending) ;
extern void MFP_MaskIntrW (u_int16 BitMask, u_int8 fDisableIntr) ;
extern u_int16 MFP_PollForIntr (register u_int16 BitMask, u_int8 fDisableDone,
			register u_int8 fSingleLoop, register u_int8 fAny) ;
extern void MFP_BindEnb (void (** VecTbl)(void), void (* pISR)(void),
			 u_int8 BaseVec, u_int8 Which) ;
extern void MFP_Poll_uSec   (u_int32 Nbr_uSec) ;
extern void MFP_Poll_10uSec (void) ;
extern void MFP_Poll_mSec   (u_int32 Nbr_mSec) ;
extern u_int8 MFP_Poll_IOs (u_int8 PatBits, u_int8 MskBits,
				u_int8 fOnce, u_int8 fAny) ;

extern void MFP_HwtStdTimeout (void) ;
extern u_int16 MFP_HwtPollChk (u_int16 DevMask, u_int8 fOnce) ;
extern s_int32 MFP_HwtClrTimer (MFPHWT_Cback (pHdlr)) ;
extern s_int32 MFP_HwtSetTimer (MFPHWT_Cback (pHdlr), u_int32 Arg,
							u_int32 NewCount) ;
extern void MFP_HwtChgState (u_int8 fPolling) ;
extern void MFP_HwtInit (u_int8 fPolling) ;
#endif

#endif	/* __MFP_HW_DEFS_H */
