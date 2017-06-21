| $Id: bt_vector.S,v 1.1.4.1 1996/05/09 15:00:01 rbadri Exp $
| $Source: /release/112/cvs/Xwbu/cat5k/alc_romboot/bt_vector.S,v $
|-----------------------------------------------------------------
| bt_vector.S -- ACP Boot PROM exception vector table.
|                (Based on Catalyst Boot code.)
|
| November 24, 1994, Rick Lowe
|
| Copyright (c) 1994-1996 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: bt_vector.S,v $
| Revision 1.1.4.1  1996/05/09  15:00:01  rbadri
| Branch: California_branch
| Synergy features to the mainline
|
| Revision 1.1.40.1  1996/04/27  06:15:57  cakyol
| non sync sync from V112_0_2 to ....
| Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
|
| Revision 1.1.28.1  1996/04/08  01:41:06  bbenson
| Branch: LE_Cal_V112_0_2_branch
| Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
|
| Revision 1.1.16.1  1996/03/22  09:12:06  rlowe
| Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
| yielding LE_Syn_Cal_V111_1_3_branch.
| Branch: LE_Syn_Cal_V111_1_3_branch
|
| Revision 1.1.2.1  1996/03/13  07:42:30  rlowe
| Add file content from Synergy repository.
| Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
|
| Revision 1.1  1996/03/11  07:43:10  rlowe
| Placeholders for California.
|
|------------------------------------------------------------------
| Legacy comments from Synergy repository:
|
| Revision 1.3.8.1  1996/01/18  05:02:14  rlowe
| Manufacturability changes to bootrom and flash boot code.
|
| Branch: ATM21_branch
|
|------------------------------------------------------------------
| $Endlog$
|

#include "asm_mode_defs.h"
#include "boot_defs.h"
#include "bt_globvar.h"


| ----------------------------------------------------------------------------
|	Exception vector table for Boot code.
|	For PROM, need to make sure that this table starts at abs $0,
|	to support CPU reset vector !
| ----------------------------------------------------------------------------
		.text
		.globl	__bt_VectorTbl
__bt_VectorTbl:					| Table of 256 exception vects
|
|	After reset assign the system stack pointer to the end of
|	the Boot area in main memory (DRAM).
|
		.long	BOOT_STACK_START	| V 0 Reset stack pointer
		.long	__Boot_main		| V 1 Reset entry point
|
|	Assign the default exception handler for vectors 2 - 30.
|
		.long	__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr
|
|	Assign default exception handler for vec 31, Level 7 (NMI) autovector.
|
		.long	__error_hdlr		| V31 Level 7 (NMI) autovector
|
|	Assign the default exception handler for vectors 32 - 63.
|
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
|
|	Assign interrupt handlers for the MFP interrupt controller;
|	some of these will be the default exception handler.
|
		.long	__error_hdlr		| V64 TxSar intr (use default)
		.long	__error_hdlr		| V65 RxSar intr (use default)
		.long	__error_hdlr		| V66 LCP/MP intr (use default)
		.long	__error_hdlr		| V67 LCP/MR intr (use default)
		.long	__error_hdlr		| V68 MFP Timer D (use default)
		.long	__timer_c_hdlr		| V69 MFP Timer C interrupt
		.long	__error_hdlr		| V70 NTC intr (use default)
		.long	__error_hdlr		| V71 MFP I5 intr (use default)
		.long	__timer_b_hdlr		| V72 MFP Timer B interrupt
		.long	__te_hdlr		| V73 MFP UART Tx Error intr
		.long	__tbe_hdlr		| V74 MFP UART Tx Buffer Empty
		.long	__re_hdlr		| V75 MFP UART Rx Error intr
		.long	__rbf_hdlr		| V76 MFP UART Rx Buffer Full
		.long	__timer_a_hdlr		| V77 MFP Timer A interrupt
		.long	__error_hdlr		| V78 MFP I6 intr (use default)
		.long	__error_hdlr		| V79 Mfg. jumper (use default)
|
|	Assign the default exception handler for vectors 80 - 255.
|	MFP interrupt vectors are set to come here when the MFP has
|	not (yet) been initialized.
|
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr
		.long	__error_hdlr,__error_hdlr,__error_hdlr,__error_hdlr

| ----------------------------------------------------------------------------
|	Vector table is well-known, so put jump right after it to Boot entry.
| ----------------------------------------------------------------------------

		jra	__Boot_main	| Get to main Boot entry point.

