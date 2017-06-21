/* $Id: hw_csp.h,v 3.1.60.1 1996/03/22 00:16:00 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/xdi/hw_csp.h,v $
 *------------------------------------------------------------------
 * hw_csp.h - 1F MIF68840 based PCI port adaptor support
 *             ( Port adapter platforms common PHY definitions for XDI )
 *
 * December 1995, Farhad Sunavala
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hw_csp.h,v $
 * Revision 3.1.60.1  1996/03/22  00:16:00  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.1  1995/12/23  04:42:24  fsunaval
 * Branch: ELC_branch
 * 1.First pass at FDDI port adapter.
 *
 * Revision 3.1  1995/12/19  20:36:11  fsunaval
 * Placeholder files for new development.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __HW_CSP_H__
#define __HW_CSP_H__

/*
@(#) xm7_hw_csp.h xm7_v3.1.1:cset.0166:6:6 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		Connection Services Process Header File

		PCM State Machine Support
		****	FOR MOTOROLA, DEC, and AMD PHYs WITH BUILT-IN
		****	PCM SUPPORT.

		File:			xm7_hw_csp.h
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
*	Default SMT Timer Values
*/
#define	DEFAULT_C_MIN_VAL		(TPC_VALUE (C_Min))
#define	DEFAULT_A_MAX_VAL		DEFAULT_C_MIN_VAL	/* older SMT name */
#define	DEFAULT_TL_MIN_VAL		(TPC_VALUE (TL_Min))
#define	DEFAULT_LS_MAX_VAL		DEFAULT_TL_MIN_VAL	/* older SMT name */
#define	DEFAULT_TB_MIN_VAL		(TPC_VALUE (TB_Min))
#define	DEFAULT_T_OUT_VAL		(TPC_VALUE (T_Out))
#define	DEFAULT_LC_SHORT_VAL	(TPC_VALUE (LC_Short))
#define	DEFAULT_T_SCRUB_VAL		(TPC_VALUE (T_Scrub))
#define	DEFAULT_NS_MAX_VAL		(TNE_VALUE (NS_Max))

/*
*	XLNT Manager(tm) Signals
*/
#define	SIG_PCM_BREAK			(PCM_SIGNALS | 0x81)	/* PCM_BREAK intr */
#define	SIG_PCM_CODE			(PCM_SIGNALS | 0x82)	/* PCM_CODE intr */
#define	SIG_PCM_ENABLED			(PCM_SIGNALS | 0x83)	/* PCM_ENABLED intr */
#define	SIG_PCM_TRACE_PROP		(PCM_SIGNALS | 0x84)	/* TRACE_PROP intr */
#define	SIG_PCM_SELF_TEST		(PCM_SIGNALS | 0x85)	/* SELF_TEST intr */
#define	SIG_PC_Restart			(PCM_SIGNALS | 0x8A)	/* handling PC_Start */

#endif __HW_CSP_H__
