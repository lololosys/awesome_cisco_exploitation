/* $Id: atmdvr_gdata.c,v 1.1.4.1 1996/05/09 14:54:42 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/atmdvr_gdata.c,v $
 *------------------------------------------------------------------
 * atmdvr_gdata.c -- Definitions of ATM driver global data structures.
 *
 * April 28, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: atmdvr_gdata.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:42  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:10  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:38:53  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:08  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:38  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.4  1995/05/26  08:42:58  sakumar
 * Adjust platform specific startup/init for driver and LANE integration.
 *
 * Revision 1.3  1995/05/26  01:25:29  sakumar
 * Added functionality for LANE Support. (1) Upcall to initiate LE-ARP etc.
 *
 * Revision 1.2  1995/05/02  05:01:17  rlowe
 * Move some global counter definitions from datatest.c to atmdvr_gdata.c/h.
 * Eliminate some atm driver startup code from atmdvr_poll.c; all startup now
 * in atmdriver.c.
 *
 * Revision 1.1  1995/05/02  01:06:58  rlowe
 * Re-structure code to handling polling (boot case) as well as interrupt
 * mode (kernel case).  Isolate IOS-isms (such as edisms) a bit more.
 * Provide polling mode APIs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef LANGUAGE_C
#define     LANGUAGE_C
#endif

#include "basic_types.h"
#include "param.h"
#include "sar_if.h"
#include "atmdrv_if.h"
#include "atmdvr_gdata.h"

/*
**	PROTOTYPES
*/


/*
**	GLOBALS
*/

tATMDRVGCB	AtmdrvGcb;
tpATMDRVGCB	pAtmdrvGcb;				    /* atmdrv_if.h */
UINT16		vcd_vpc_map [MAX_PORT][MAX_DEFAULT_VPC_C];  /* atmdrv_if.h */

volatile UINT32	sar_intr_status = 0;

UINT32	crc_err = 0;
UINT32	rto_err = 0;
UINT32	buf_ovfl_err = 0;
UINT32	len_err = 0;


UINT32 num_active_vci_bits;
UINT32 num_active_vpi_bits;
UINT32 num_active_vpi_vci_bits;

UINT8	fADVR_ForceEvPoll ;

void     (* ADVR_Std_TSar_Upcall) (UINT8 *, UINT32) ;
void     (* ADVR_Std_TSar_LeArpUep) (tpLEARP_PARAM pMsg);


/*
**
** Structure to keep track of which VCR Info structs are in
** VCR memory (as opposed to Secondary Memory)
**
*/

t_vcr_ivci_map_entry_info vcr_imap_ptr[MAX_VCR_IVCI_MAP_ENTRIES];

int head_avail_list;
int head_empty_list;

