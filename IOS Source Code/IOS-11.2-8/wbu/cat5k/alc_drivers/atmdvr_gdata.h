/* $Id: atmdvr_gdata.h,v 1.1.4.1 1996/05/09 14:54:43 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/atmdvr_gdata.h,v $
 *------------------------------------------------------------------
 * atmdvr_gdata.h -- Declarations of ATM driver global data structures.
 *
 * April 28, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdvr_gdata.h,v $
 * Revision 1.1.4.1  1996/05/09  14:54:43  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:11  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:54  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.2  1996/04/02  05:08:07  rlowe
 * Fix up include file paths.  Cleanup main ALC makefile regarding
 * sar images and tools.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.16.1  1996/03/22  09:10:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:09  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:39  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.8.10.1  1996/02/29  07:00:07  rlowe
 * Changes needed for IOS-side sync to Arkansas V111_0_16
 * (Synalc3_Cal_V111_0_16_branch).
 *
 * Revision 1.8  1995/05/26  08:42:59  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.7  1995/05/26  01:25:30  sakumar
 * Added functionality for LANE Support. (1) Upcall to initiate LE-ARP etc.
 *
 * Revision 1.6  1995/05/19  00:01:17  sakumar
 * Detect and handle SAR timeouts.
 *
 * Revision 1.5  1995/05/03  04:21:42  rlowe
 * Replace "acp_memcpy" with "memcpy", which is available in
 * services/libc/libc.a and libc_ios.a.
 *
 * Revision 1.4  1995/05/02  05:01:17  rlowe
 * Move some global counter definitions from datatest.c to atmdvr_gdata.c/h.
 * Eliminate some atm driver startup code from atmdvr_poll.c; all startup now
 * in atmdriver.c.
 *
 * Revision 1.3  1995/05/02  04:28:18  rlowe
 * Fix upcall registration and use, so that ATM driver will work even if
 * NO upcall is current (fcn pointer is NULL).  Add upcall registration
 * to Syn_ipc.c test file.
 *
 * Revision 1.2  1995/05/02  01:28:46  rlowe
 * Cleanup files a bit; ensure polling mode api's available in atm driver
 * interface file.
 *
 * Revision 1.1  1995/05/02  01:06:58  rlowe
 * Re-structure code to handling polling (boot case) as well as interrupt
 * mode (kernel case).  Isolate IOS-isms (such as edisms) a bit more.
 * Provide polling mode APIs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	_ATMDVR_GDATA_H
#define	_ATMDVR_GDATA_H


#include "../alc_cominc/basic_types.h"
#include "../alc_cominc/param.h"
#include "../alc_cominc/sar_if.h"
#include "../alc_cominc/atmdrv_if.h"


/*
**	PROTOTYPES
*/

typedef void (tADVR_PktUpCall) (UINT8 *, UINT32) ;
typedef void (tADVR_LearpUpCall) (tpLEARP_PARAM);

extern void		ADVR_CmdTimeOut (UINT8 WhichSar, tpSARCMD pCmd) ;
extern void		ADVR_DispatchInPktUpCall (UINT8 CmdCode, UINT8 * pData,
							UINT32 length) ;
extern void 	ADVR_Dispatch_lec_arpevent (tpLEARP_PARAM  pMsg);
extern UINT8	ADVR_RxSar_Handler (UINT8 fOnce) ;
extern UINT8	ADVR_TxSar_Handler (UINT8 fOnce) ;
extern void		ADVR_DismissOnRxSar (void * pFunc, tpSARCMD pCmd) ;
extern void		ADVR_DismissOnTxSar (void * pFunc, tpSARCMD pCmd) ;
extern void		ADVR_DismissOnRxSar2 (void * pFunc, tpSARCMD pCmd) ;
extern void		ADVR_DismissOnTxSar2 (void * pFunc, tpSARCMD pCmd) ;
extern void		ADVR_RxSarPollEvWait (void * pFunc, tpSARCMD pCmd) ;
extern void		ADVR_TxSarPollEvWait (void * pFunc, tpSARCMD pCmd) ;

extern tBOOLEAN	atmdrv_BLOCK (tpSARCMD pCmd) ;


/*
**	GLOBALS
*/

extern tATMDRVGCB AtmdrvGcb;

#define	 RXSAR_INTR	1
#define	 TXSAR_INTR	2
extern volatile UINT32	sar_intr_status ;

extern	UINT32	crc_err;
extern	UINT32	rto_err;
extern	UINT32	buf_ovfl_err;
extern	UINT32	len_err;

extern UINT32 num_active_vci_bits;
extern UINT32 num_active_vpi_bits;
extern UINT32 num_active_vpi_vci_bits;

extern UINT8	fADVR_ForceEvPoll ;

extern void	(* ADVR_Std_TSar_Upcall) (UINT8 *, UINT32) ;
extern void	(* ADVR_Std_TSar_LeArpUep) (tpLEARP_PARAM);

/*
**
** Structure to keep track of which VCR Info structs are in
** VCR memory (as opposed to Secondary Memory)
**
*/
typedef struct vcr_ivci_map_entry_info_ {
	tpIMAPENTRY	pMapEntry;
	int	next;
} t_vcr_ivci_map_entry_info, *tp_vcr_ivci_map_entry_info;

extern t_vcr_ivci_map_entry_info vcr_imap_ptr[MAX_VCR_IVCI_MAP_ENTRIES];

extern int head_avail_list;
extern int head_empty_list;

#endif	/* _ATMDVR_GDATA_H */
