/* $Id: lcp_lane.c,v 1.1.4.1 1996/05/09 14:54:56 rbadri Exp $
 * $Source: /release/112/cvs/Xwbu/cat5k/alc_drivers/lcp_lane.c,v $
 *------------------------------------------------------------------
 * lcp_lane.c -- C code to handle LCP driver/LANE application interactions.
 *
 * July 25, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lcp_lane.c,v $
 * Revision 1.1.4.1  1996/05/09  14:54:56  rbadri
 * Branch: California_branch
 * Synergy features to the mainline
 *
 * Revision 1.1.40.1  1996/04/27  06:14:23  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.28.3  1996/04/15  21:21:50  sakumar
 * Support for VTP on NMP creating/destroying LECs on Cat5K ATM Module.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.2  1996/04/11  05:31:25  rlowe
 * Add LCP driver and upcall support for VTP messages (over SCP).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 1.1.28.1  1996/04/08  01:39:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.16.1  1996/03/22  09:10:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.2.2  1996/03/20  10:52:31  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/03/13  07:41:22  rlowe
 * Add file content from Synergy repository.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1  1996/03/11  07:39:51  rlowe
 * Placeholders for California.
 *
 *------------------------------------------------------------------
 * Legacy comments from Synergy repository:
 *
 * Revision 1.3  1995/11/03  03:11:21  rlowe
 * Allow LANE to see all flavours of spanning-tree port state changes.
 *
 * Revision 1.2  1995/07/27  01:22:19  rlowe
 * Cleanup LANE Set Port State upcall from LCP driver so that
 * Boot code does not need to know about it.  Register the
 * upcall from ALC-specific startup.
 *
 * Revision 1.1  1995/07/26  03:10:29  rlowe
 * Changes to pass certain Set Port State messages to LANE for spanning
 * tree support.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "acp_basic_types.h"
#include "lo_wdog.h"
#include "acp_c_defs.h"
#include "lcp_api.h"
#include "lcp_if.h"
#include "lcp_defs.h"
#include "vlan_defs.h"
#include "lec_ipc.h"

#define	LCPl_DFLT_ALCPORTNBR	0


u_int32	(*pLCP_SPS_LaneUpcall)(u_int32, u_int32, u_int32) = NULL ;
u_int32	(*pLCP_SVP_VTP_Upcall)(u_int8 *) = NULL ;


/**/
/* --------------------------------------------------------------------	*/
/*	Registration routine so LCP driver can learn where to upcall	*/
/*	when Set Port State messages are received for spanning-tree.	*/
/* --------------------------------------------------------------------	*/
void LCPl_RegPortStateInd (u_int32 (* pFcn)(u_int32, u_int32, u_int32))
{
	pLCP_SPS_LaneUpcall = pFcn ;

}	/* LCPl_RegPortStateInd */

/**/
/* --------------------------------------------------------------------	*/
/*	Inform LANE of NMP spanning-tree port state changes.		*/
/*	Return TRUE on success, FALSE on any failure.			*/
/* --------------------------------------------------------------------	*/
u_int8 LCPl_LanePortStateInd (tLCP_DvrBufU * pInd)
{
	u_int32	VLanNbr ;
	u_int32	P_State ;
	u_int32	Result  ;


		/* Is upcall available ? */
	PUNCH_WD ;
	if (pLCP_SPS_LaneUpcall == NULL)
		return (TRUE) ;

		/* Suppress any internal Synergy VLan IDs. */
	VLanNbr = pInd->PS.portst [LCPl_DFLT_ALCPORTNBR].vlan_num ;
	if ((VLanNbr < SYNERGY_MIN_USR_VLAN) ||
	    (VLanNbr > SYNERGY_MAX_USR_VLAN))
		return (TRUE) ;

		/* Map the Spanning Tree port state for LANE. */
	switch (pInd->PS.portst [LCPl_DFLT_ALCPORTNBR].port_state)
	{
	    case PORT_DISABLED:
		P_State = LEC_PORTDISABLD ;
		break ;
	    case PORT_LISTENING:
		P_State = LEC_PORTLISTEN ;
		break ;
	    case PORT_LEARNING:
		P_State = LEC_PORTLEARN ;
		break ;
	    case PORT_FORWARDING:
		P_State = LEC_PORTUNBLOCK ;
		break ;
	    case PORT_BLOCKING:
		P_State = LEC_PORTBLOCK ;
		break ;
	    default:
		P_State = LEC_PORTUNKNOWN ;
		break ;
	}

		/* Expect 0 (zero) on success. */
	Result = (* pLCP_SPS_LaneUpcall)
			(VLanNbr,
			 P_State,
			 pInd->PS.portst [LCPl_DFLT_ALCPORTNBR].port_num) ;

	PUNCH_WD ;
	return (! Result) ;

}	/* LCPl_LanePortStateInd */

/**/
/* --------------------------------------------------------------------	*/
/*	Registration routine so LCP driver can learn where to upcall	*/
/*	when Set Vlan Parameters commands are received for VTP Helper.	*/
/* --------------------------------------------------------------------	*/
void LCPl_RegVlanParamCmd (u_int32 (* pFcn)(u_int8 *))
{
	pLCP_SVP_VTP_Upcall = pFcn ;

}	/* LCPl_RegVlanParamCmd */

/**/
/* --------------------------------------------------------------------	*/
/*	Inform VTP Helper of new Vlan Parameter Commands.		*/
/*	Return TRUE on success, FALSE on any failure.			*/
/* --------------------------------------------------------------------	*/
u_int8 LCPl_VTP_VlanParamCmd (tLCP_DvrBufU * pInd)
{
	u_int32	VLanNbr ;
	u_int32	Result  ;


		/* Is upcall available ? */
	PUNCH_WD ;
	if (pLCP_SVP_VTP_Upcall == NULL)
		return (FALSE) ;

		/* Suppress any internal Synergy VLan IDs. */
	VLanNbr = pInd->VC.vlanparams.vlan_num ;
	if ((VLanNbr < SYNERGY_MIN_USR_VLAN) ||
	    (VLanNbr > SYNERGY_MAX_USR_VLAN))
		return (FALSE) ;

		/* Expect 0 (zero) on success. */
	Result = (* pLCP_SVP_VTP_Upcall) ((u_int8 *) & pInd->VC.vlanparams) ;

	PUNCH_WD ;
	return (Result) ;

}	/* LCPl_VTP_VlanParamCmd */
