 /* $Id: lecmib_if.c,v 3.3.42.4 1996/08/28 12:55:12 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecmib_if.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 ATMF LEC MIB counter support functions
 * SNMP MIB method routines are in sr_lecmib.c
 * Only the SNMPR auto-generated functions are kept there
 * This file contains support for:
 *       * RFC 1573 IF-MIB support
 *
 * July 1995, Andy Bierman
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: lecmib_if.c,v $
 * Revision 3.3.42.4  1996/08/28  12:55:12  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.42.3  1996/07/11  20:34:47  cyoung
 * Deregister RcvAddresses when client shutsdown/tearsdown.
 * CSCdi61990:  LANE CLIENT: memory leak
 * Branch: California_branch
 *
 * Revision 3.3.42.2  1996/05/09  14:33:05  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.42.1.10.1  1996/04/27  07:06:56  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.42.1  1996/04/04  05:35:08  mordock
 * CSCdi51613:  incorrect locIfReason in linkUp trap
 * Branch: California_branch
 * avoid consulting mib cache if request serialNum == -1, else cache might
 * be used when it really shouldn't be.
 *
 * Revision 3.3.60.2  1996/04/08  14:52:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.60.1  1996/04/08  01:57:20  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.48.1  1996/03/22  22:55:20  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.32.1  1996/03/05  06:43:26  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.22.1  1996/02/27  21:10:44  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.2.1  1996/01/11  01:46:43  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1996/01/05  04:56:19  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.2.10.1  1995/12/29  01:07:00  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:51:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:12:43  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/07/09  02:23:01  abierman
 * Placeholder for ATMF LEC MIB -- RFC 1573 (IF-MIB) support
 *:
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "interface.h"
#include "packet.h"
#include "address.h"
#include "name.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */
#include "../ui/debug.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "../snmp/ifmib_registry.h"

#include "../if/atm.h"
#include "../atm/ilmi_api.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/network.h"
#include "../atm/atmsig_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"
#include "lane_debug.h"
#include "lane_ilmi.h"
#include "lane_registry.h"

#include "../snmp/snmp_api.h"
#include "../snmp/sr_ifmibdefs.h"
#include "sr_lecmib.h"
#include "lecmib_util.h"
#include "lecmib_if.h"

/* boolean values for the lecmib_get_vc_counter function */
#define   LECMIB_DEL_CNTR          FALSE
#define   LECMIB_CUR_CNTR          TRUE

#define  LEC_IF_MAX_SIZE_ADJUST    14

/* CPP flag to exclude IF-MIB counters that aren't supported by the
 * ATM layer yet 
 */
/* #define  FULL_IFMIB_SUPPORT   1 */

/*---------------------------------------------------------------------
 * get_lec_vc_cntrs
 *
 * get whatever IF-MIB counters possible kept by the ATM layer
 * for the specified VC
 * INPUTS:
 *    lec_info == LEC owning the VC
 *    lec_vc   == VC to get counts for
 *    cur_or_del == LECMIB_DEL_CNTR --> add stats to deleted counts
 *               == LECMIB_CUR_CNTR --> add stats to current PDU counts
 * RETURNS:
 *    none
 * SIDE EFFECTS:
 *    the appropriate IF-MIB counters in the lec_info->lec_snmp
 *    struct are updated
 *---------------------------------------------------------------------*/
static void 
get_lec_vc_cntrs (lec_info_t *lec_info,
		  lec_vc_t   *lec_vc,
		  boolean    cur_or_del)
{
    /* note...caller must make sure that lec_info->lec_snmp is valid */

    atm_real_stats_t   stats;
    lec_snmp_info_t    *lec_snmp = lec_info->lec_snmp;

    if (!atm_getvc_full_counts(lec_info->swidb->hwptr, 
			  lec_vc->vcd,
			  &stats))
	return;     /* get counters failed */
	
    /* !!! need the rest of the IF-MIB counters supported !!! */
    if (cur_or_del)  {
	/* cur: add these counts to the current counters */
	lec_snmp->in_octets += stats.inOctets;
	/*???*/
	/***** NO UCAST/NUCAST split for inPkts ****/
	lec_snmp->in_ucast_pkts += stats.inPkts;
	lec_snmp->out_octets += stats.outOctets;
	lec_snmp->out_ucast_pkts += stats.outUcastPkts;
	/***** NO MCAST/BCAST split for outNUcastPkts *****/
	lec_snmp->out_bcast_pkts += stats.outNUcastPkts;
    }
    else  {
	/* del: add these counters to the deleted counters */
	lec_snmp->deleted_in_octets += stats.inOctets;
	/*???*/
	/***** NO UCAST/NUCAST split for inPkts ****/
	lec_snmp->deleted_in_ucast_pkts += stats.inPkts;
	lec_snmp->deleted_out_octets += stats.outOctets;
	lec_snmp->deleted_out_ucast_pkts += stats.outUcastPkts;
	/***** NO MCAST/BCAST split for outNUcastPkts *****/
	lec_snmp->deleted_out_bcast_pkts += stats.outNUcastPkts;
    }
}  /* END get_lec_vc_cntrs */


/*---------------------------------------------------------------------
 * get_lec_ifmib_cntrs
 *
 * get whatever IF-MIB counters possible kept by the ATM layer
 * for all VCs used by the specified LEC
 * INPUTS:
 *    lec_info == LEC to get stats for
 * RETURNS:
 *    none
 * SIDE EFFECTS:
 *    all IF-MIB counters
 *    the appropriate IF-MIB counters in the lec_info->lec_snmp
 *    struct are cleared then updated, cache tag is not changed
 *---------------------------------------------------------------------*/
static void 
get_lec_ifmib_cntrs ( lec_info_t  *lec_info)
{
    int	bucket;
    lec_nsap_info_t *nsap_info;

    if (lec_info->lec_snmp==NULL)
	return;

    lecmib_clear_cur_if_cntrs(lec_info->lec_snmp);

    /* configure VC */
    if (lec_info->configure.call_state==CALL_STATE_UP)
	get_lec_vc_cntrs(lec_info, &lec_info->configure,
			 LECMIB_CUR_CNTR);

    /* control direct VC */
    if (lec_info->ctl_direct.call_state==CALL_STATE_UP)
	get_lec_vc_cntrs(lec_info, &lec_info->ctl_direct,
			 LECMIB_CUR_CNTR);


    /* control distribute */
    if (lec_info->ctl_distribute.call_state==CALL_STATE_UP) 
	get_lec_vc_cntrs(lec_info, &lec_info->ctl_distribute,
			 LECMIB_CUR_CNTR);


    /* multicast send */
    if (lec_info->mcast_send.call_state==CALL_STATE_UP) 
	get_lec_vc_cntrs(lec_info, &lec_info->mcast_send,
			 LECMIB_CUR_CNTR);

    /* multicast forward */
    if (lec_info->mcast_forward.call_state==CALL_STATE_UP) 
	get_lec_vc_cntrs(lec_info, &lec_info->mcast_forward,
			 LECMIB_CUR_CNTR);

    /* add up all the data-direct SVCs ... borrowed from lec.c */
    for (bucket = 0; bucket < LANE_NETHASHLEN; bucket++)  {
	for (nsap_info = lec_info->nsap_table[bucket]; 
	     nsap_info; nsap_info = nsap_info->next)  {
	    if (nsap_info->vc.call_state==CALL_READY)
		get_lec_vc_cntrs(lec_info, &nsap_info->vc,
				 LECMIB_CUR_CNTR);
	}
    }
}  /* END get_lec_ifmib_cntrs */



/*---------------------------------------------------------------------
 * ifmib_err_t lecmib_if_cntr32
 *
 * IF-MIB ETHERNET counter32 callback function
 * get the specified ifEntry or ifXEntry counter 
 * INPUTS:
 *	ifCntr == enum of specific counter requested
 *	if_ptr == r/o pointer to IDB for this interface
 *      serialNum == SNMPR PDU serial ID -- used for caching
 * OUTPUTS:
 *	*retval == the returned counter value
 * RETURNS:
 *	return status
 *---------------------------------------------------------------------*/
static ifmib_err_t 
lecmib_if_cntr32 (
	  ifmib_cntr_t	       ifCntr,
	  const	subiabtype     *subiab,
          ulong		       *retval,
	  int                  serialNum )
{
    lec_info_t         *lec_info;
    lec_snmp_info_t    *lec_snmp;
    const snmpifptrtype *if_ptr;

    if_ptr = &subiab->master_ptr;
    *retval = 0;

    /* check the pointer chain and make sure the LEC is enabled */
    if (!(if_ptr && if_ptr->ifptr.swidb->lane_info && 
	  if_ptr->ifptr.swidb->lane_info->lec_enabled))
	return IF_CNTR_NOT_AVAIL;

    /* make sure SNMP info was malloced on this LEC */
    lec_info = if_ptr->ifptr.swidb->lane_info->lec_info;
    if (lec_info->lec_snmp==NULL)
	return IF_CNTR_NOT_AVAIL;
    else
	lec_snmp = lec_info->lec_snmp;

    /* get serialNum passed in to check against cache id */
    if ((serialNum == -1) || (lec_snmp->cache_serial_num != serialNum))   {
	get_lec_ifmib_cntrs(lec_info);
	lec_snmp->cache_serial_num = serialNum;
    }

    switch (ifCntr) {
    case IN_OCTETS_CNTR:
	*retval = lec_snmp->deleted_in_octets + 
	    lec_snmp->in_octets;
	break;
    case IN_UCAST_CNTR:
	/* can't really support this yet...all inPkts counted here */
	*retval = lec_snmp->deleted_in_ucast_pkts +
	    lec_snmp->in_ucast_pkts;
	break;
    case IN_NUCAST_CNTR:
#ifdef FULL_IFMIB_SUPPORT
	*retval = lec_snmp->deleted_in_mcast_pkts +
	       lec_snmp->deleted_in_bcast_pkts +
	       lec_snmp->in_mcast_pkts +
	       lec_snmp->in_bcast_pkts;
	break;
#else
	return IF_CNTR_NOT_AVAIL;
#endif
    case IN_DISCARDS_CNTR:
	*retval = lec_snmp->deleted_in_discards +
	    lec_snmp->in_discards;
	break;
    case IN_ERRORS_CNTR:
	*retval = lec_snmp->deleted_in_errors + 
	    lec_snmp->in_errors;
	break;
    case IN_UNK_PROTOS_CNTR:
	*retval = lec_snmp->deleted_in_unknown_protos + 
	    lec_snmp->in_unknown_protos;
	break;
    case OUT_OCTETS_CNTR:
	*retval = lec_snmp->deleted_out_octets + 
	    lec_snmp->out_octets;
	break;
    case OUT_UCAST_CNTR:
	*retval = lec_snmp->deleted_out_ucast_pkts +
	    lec_snmp->out_ucast_pkts;
	break;
    case OUT_NUCAST_CNTR:
	*retval = lec_snmp->deleted_out_mcast_pkts +
	       lec_snmp->deleted_out_bcast_pkts +
	       lec_snmp->out_mcast_pkts +
	       lec_snmp->out_bcast_pkts;
	break;
    case OUT_DISCARDS_CNTR:
	*retval = lec_snmp->deleted_out_discards +
	    lec_snmp->out_discards;
	break;
    case OUT_ERRORS_CNTR:
	*retval = lec_snmp->deleted_out_errors +
	    lec_snmp->out_errors;
	break;
    case OUT_QLEN_CNTR:
	return IF_CNTR_NOT_AVAIL;    /* QLEN not supported */
    case IN_BCAST_CNTR:			/* ifXTable */
#ifdef FULL_IFMIB_SUPPORT
	*retval = lec_snmp->deleted_in_bcast_pkts +
	    lec_snmp->in_bcast_pkts;
	break;
#else
	return IF_CNTR_NOT_AVAIL;
#endif
    case IN_MCAST_CNTR:
#ifdef FULL_IFMIB_SUPPORT
	*retval = lec_snmp->deleted_in_mcast_pkts +
	    lec_snmp->in_mcast_pkts;
	break;
#else
	return IF_CNTR_NOT_AVAIL;
#endif
    case OUT_BCAST_CNTR:
#ifdef FULL_IFMIB_SUPPORT
	*retval = lec_snmp->deleted_out_bcast_pkts +
	    lec_snmp->out_bcast_pkts;
	break;
#else
	return IF_CNTR_NOT_AVAIL;
#endif
    case OUT_MCAST_CNTR:
#ifdef FULL_IFMIB_SUPPORT
	*retval = lec_snmp->deleted_out_mcast_pkts +
	    lec_snmp->out_mcast_pkts;
	break;
#else
	return IF_CNTR_NOT_AVAIL;
#endif
    default:
	return IF_CNTR_NOT_AVAIL;
    }

    return IF_NO_ERR;
}  /* END lecmib_if_cntr32 */


/*---------------------------------------------------------------------
 * ifmib_err_t lecmib_if_cntr64
 *
 * IF-MIB ETHERNET counter64 callback function
 * get the specified ifEntry or ifXEntry counter 
 * INPUTS:
 *	ifCntr == enum of specific counter requested
 *	if_ptr == r/o pointer to IDB for this interface
 *      serialNum == SNMPR PDU serial ID -- used for caching
 * OUTPUTS:
 *      *lobits == the returned counter value -- bits  0..31
 *	*hibits == the returned counter value -- bits 32..63
 * RETURNS:
 *	return status
 *---------------------------------------------------------------------*/
/*ARGSUSED*/
static ifmib_err_t 
lecmib_if_cntr64 (
	  ifmib_cntr_t	       ifCntr,
	  const	subiabtype     *subiab,
	  ulong                *hibits,
          ulong		       *lobits,
	  int                  serialNum )
{
    lec_info_t         *lec_info;
    lec_snmp_info_t    *lec_snmp;
    const snmpifptrtype *if_ptr;

    if_ptr = &subiab->master_ptr;
    *hibits = 0;
    *lobits = 0;

    /* check the pointer chain and make sure the LEC is enabled */
    if (!(if_ptr && if_ptr->ifptr.swidb->lane_info && 
	  if_ptr->ifptr.swidb->lane_info->lec_enabled))
	return IF_CNTR_NOT_AVAIL;

    /* make sure SNMP info was malloced on this LEC */
    lec_info = if_ptr->ifptr.swidb->lane_info->lec_info;
    if (lec_info->lec_snmp==NULL)
	return IF_CNTR_NOT_AVAIL;
    else
	lec_snmp = lec_info->lec_snmp;

    /* check the ifStack...see if a sonet interface is below,
     * otherwise there are no high speed counters
     * TEMP -- no high counters --
     */
    return IF_CNTR_NOT_AVAIL;

#ifdef FULL_IFMIB_SUPPORT
    /* get serialNum passed in to check against cache id */
    if ((serialNum != -1) && (lec_snmp->cache_serial_num != serialNum))  {
	get_lec_ifmib_cntrs(lec_info);
	lec_snmp->cache_serial_num = serialNum;
    }

    switch (ifCntr) {
    case IN_OCTETS_CNTR:
	*lobits = lec_snmp->deleted_in_octets + 
	    lec_snmp->in_octets;
	*hibits = lec_snmp->deleted_in_octets_HC + 
	    lec_snmp->in_octets_HC;
	break;
    case IN_UCAST_CNTR:
    case IN_NCAST_CNTR:
    case IN_DISCARDS_CNTR:
    case IN_ERRORS_CNTR:
    case IN_UNK_PROTOS_CNTR:
	return IF_CNTR_NOT_AVAIL;
    case OUT_OCTETS_CNTR:
	*lobits = lec_snmp->deleted_out_octets + 
	    lec_snmp->out_octets;
	*hibits = lec_snmp->deleted_out_octets_HC + 
	    lec_snmp->out_octets_HC;
	break;
    case OUT_UCAST_CNTR:
    case OUT_NUCAST_CNTR:
    case OUT_DISCARDS_CNTR:
    case OUT_ERRORS_CNTR:
    case OUT_QLEN_CNTR:
    case IN_BCAST_CNTR:	
    case IN_MCAST_CNTR:
    case OUT_BCAST_CNTR:
    case OUT_MCAST_CNTR:
    default:
	return IF_CNTR_NOT_AVAIL;
    }
    return IF_NO_ERR;
#endif  /* FULL_IFMIB_SUPPORT */
}  /* END lecmib_if_cntr64 */


/*---------------------------------------------------------------------
 * lecmib_if_register
 *
 * register the specified LEC with the IF-MIB library
 * INPUTS:
 *	lec_info == ptr to LEC to add to the IF-MIB
 *         It is assumed that little info is known; 
 *         Info will be updated when JOIN completes
 *      init_lec_up ==
 *         TRUE if initial LEC ifAdminStatus should be up
 *         FALSE if initial LEC ifAdminStatus should be down
 *---------------------------------------------------------------------*/
void 
lecmib_if_register (lec_info_t  *lec_info, boolean init_lec_up)
{
    subiabtype    *subiab;
    snmpifptrtype if_ptr;

    /* check SNMP enabled for this LEC */
    if ((lec_info->lec_snmp == NULL) || (lec_info->lane_info == NULL))
	return;

    subiab = malloc(sizeof(subiabtype));
    if (!subiab)
	return;

    if_ptr.ifptr.subiab = subiab;

    subiab->master_ptr.ifptr.swidb = lec_info->swidb;
    subiab->master_type = SNMP_IF_SWIDB;

    subiab->data_ptr = NULL;
    subiab->cleaner = NULL;

    /* this field should get overwritten by the IF-MIB code */    
    subiab->snmp_if_index = 0;

    /* the following fields are not known at the time LEC is created 
     * they need to be reset when the JOIN is completed
     * ??? the following temp settings will have to do until the
     * ??? JOIN phase is complete
     */

    /* set the ifName to LEC/ATM<slot>/<hw-if>.0 */
    (void)sprintf(lec_info->lec_snmp->if_name, "LEC/%s",
		  lec_info->swidb->namestring);
    subiab->if_name = lec_info->lec_snmp->if_name;

    /* set the ifDescr to LEC.<lecIndex>.<lecid> */
    (void)sprintf(lec_info->lec_snmp->if_descr, "LEC.%d.%d",
		  get_lec_index(lec_info),
                  lec_info->lecid);
    subiab->if_descrstring = lec_info->lec_snmp->if_descr;

    /* set ifPhysAddr to the lec_info MAC address */
    subiab->ifPhysAddr = lec_info->normalMac;

    /* set ifPhysAddrLen pointer */
    lec_info->lec_snmp->if_phys_addr_len = STATIONLEN_IEEE48;
    subiab->ifPhysAddrLen = &lec_info->lec_snmp->if_phys_addr_len;

    /* set the MTU based on the LANE frame size, as defined 
     * in the LEC MIB
     */
    switch (lec_info->lane_info->max_data_frame) {
    case LANE_MAX_FRAME_1516:
	lec_info->lec_snmp->if_max_mtu = LANE_MAX_FRAME_SIZE_1516;
	break;
    case LANE_MAX_FRAME_4544:
	lec_info->lec_snmp->if_max_mtu = LANE_MAX_FRAME_SIZE_4544;
	break;
    case LANE_MAX_FRAME_9234:
	lec_info->lec_snmp->if_max_mtu = LANE_MAX_FRAME_SIZE_9234;
	break;
    case LANE_MAX_FRAME_18190:
	lec_info->lec_snmp->if_max_mtu = LANE_MAX_FRAME_SIZE_18190;
	break;
    case LANE_MAX_FRAME_UNSPECIFIED:
    default:
	lec_info->lec_snmp->if_max_mtu =
	    LANE_MAX_FRAME_SIZE_UNSPECIFIED;
    }

    subiab->maxmtu = &lec_info->lec_snmp->if_max_mtu;

    /* set the ifType to aflane_8023
     * this may need to change, after the JOIN, to aflane_8025 
     * when token ring is implemented 
     */
    subiab->ifType = D_ifType_aflane8023;

    /* finally try to register the interface with the IF-MIB 
     * the CLI and VTP should set the initiial adminStatus to
     * UP, and SNMP should set it to DOWN
     */
    lec_info->lec_snmp->ifmib_enabled =
	reg_invoke_ifmib_register_subif(
              &if_ptr, SNMP_IF_SUBIAB,
              (init_lec_up) ? SNMP_ADMIN_UP : SNMP_ADMIN_DOWN);
    if (!lec_info->lec_snmp->ifmib_enabled)
	free(subiab);
    lec_info->lec_snmp->if_index = subiab->snmp_if_index;

}  /* lecmib_if_register */
			    


/*---------------------------------------------------------------------
 * lecmib_if_deregister
 *
 * de-register the specified LEC with the IF-MIB library
 * INPUTS:
 *	lec_info == ptr to LEC to remove from the IF-MIB
 *---------------------------------------------------------------------*/
void 
lecmib_if_deregister (lec_info_t  *lec_info)
{
   if (lec_info->lec_snmp->ifmib_enabled)  {
       reg_invoke_ifmib_deregister_subif(
	    get_lec_ifIndex(lec_info));
       lec_info->lec_snmp->ifmib_enabled = FALSE;
   }
}  /* END lecmib_if_deregister */


/*---------------------------------------------------------------------
 * lecmib_count_deleted_SVC
 *
 * registry callback function to add the IF-MIB stats for the specified
 * SVC to the lec_snmp.deleted* counters.
 * Called just before it is deleted.
 * INPUTS:
 *    lec_info == LEC that owns the SVC
 *    lec_svc == specific SVC that is about to be torn down
 * RETURNS:
 *    none
 *---------------------------------------------------------------------*/
void 
lecmib_count_deleted_SVC (void *lec_info_mask,
			  void *lec_svc_mask)
{
    lec_info_t *lec_info = (lec_info_t *)lec_info_mask;
    lec_vc_t   *lec_svc = (lec_vc_t *)lec_svc_mask;

    /* make sure SNMP structure was allocated for this LEC */
    if (lec_info->lec_snmp == NULL)
	return;

    get_lec_vc_cntrs(lec_info, lec_svc, LECMIB_DEL_CNTR);
}  /* lecmib_count_deleted_SVC */



/*---------------------------------------------------------------------
 * convert_admin_status
 *
 * convert internal LANE admin status to LEC MIB value
 * INPUTS:
 *    lane_status == internal status value
 * RETURNS:
 *    LEC MIB status value
 *---------------------------------------------------------------------*/
static inline uint  
convert_admin_status (lane_status_t lane_status)
{
    switch (lane_status)   {
    case LANE_ACTIVE:
	return SNMP_ADMIN_UP;
    case LANE_INACTIVE:
	return SNMP_ADMIN_DOWN;
    default:
	return SNMP_ADMIN_UNKNOWN;
    }
    /*NOTREACHED*/
}  /* convert_admin_status */



/*---------------------------------------------------------------------
 * lecmib_if_get_adminstatus
 *
 * IF-MIB get_admin_status callback
 * retrieve the ifAdminStatus for the LEC referenced in the
 * specified IDB
 * INPUTS:
 *    if_ptr == ptr to IDB (should have a lec_info_t attached to it)
 * RETURNS:
 *    LEC MIB status value, or UNKNOWN if wrong IDB type passed
 *---------------------------------------------------------------------*/
static uint 
lecmib_if_get_adminstatus (const subiabtype *subiab)
{
    lec_info_t  *lec_info;

    lec_info = lec_info_from_swidb(subiab->master_ptr.ifptr.swidb);
    if (lec_info)
	return convert_admin_status(lec_info->admin_status);
    else
	return SNMP_ADMIN_UNKNOWN;
}  /* lecmib_if_get_adminstatus */


/*---------------------------------------------------------------------
 * lecmib_if_admin_change 
 *
 * IF-MIB check_admin_status callback
 * test the proposedifAdminStatus for the LEC referenced in the
 * specified IDB, this may be test-phase or commit-phase of PDU 
 * processing.
 * INPUTS:
 *    newstatus == proposed ifAdminStatus for this LEC
 *    testphase == TRUE if this is the test-phase; no action by agent yet
 *              == FALSE if commit phase; new admin status should be applied
 *    if_ptr == ptr to IDB (should have a lec_info_t attached to it)
 * RETURNS:
 *    TRUE if proposed change should be allowed, FALSE otherwise
 *---------------------------------------------------------------------*/
/*ARGSUSED*/
static boolean 
lecmib_if_admin_change (enum SNMP_ADMIN_STATE newstatus,
			     boolean testphase,
			     const subiabtype *subiab)
{
    /* when LEC control via SNMP is done, this callback should
     * be supported
     */
    return FALSE;    /* don't allow any changes */
}  /* lecmib_if_admin_change */



/*---------------------------------------------------------------------
 * lecmib_if_get_operstatus
 *
 * IF-MIB get_oper_status callback
 * retrieve the ifOperStatus for the LEC referenced in the
 * specified IDB
 * INPUTS:
 *    if_ptr == ptr to IDB (should have a lec_info_t attached to it)
 * RETURNS:
 *    IF-MIB enum for current LEC ifOperstatus
 *---------------------------------------------------------------------*/
static uint 
lecmib_if_get_operstatus (const subiabtype *subiab)
{
    lec_info_t  *lec_info;

    lec_info = lec_info_from_swidb(subiab->master_ptr.ifptr.swidb);
    if (lec_info)
	return convert_admin_status(lec_info->oper_status);
    else
	return SNMP_ADMIN_UNKNOWN;
}  /* lecmib_if_get_operstatus */


/*---------------------------------------------------------------------
 * lecmib_if_rcvaddr_screen
 *
 * IF-MIB check_rcv_address callback
 * test the proposed ifRcvAddress for the LEC referenced in the
 * specified IDB, this may be test-phase or commit-phase of PDU 
 * processing.
 * INPUTS:
 *    rcvaddr   == pointer to proposed address
 *    rcvaddrlen == length of rcvaddr in bytes
 *    changeIsAdd == TRUE if adding the rcv address, FALSE if deleting it
 *    testphase == TRUE if this is the test-phase; no action by agent yet
 *              == FALSE if commit phase; address change should be applied
 *    if_ptr == ptr to IDB (should have a lec_info_t attached to it)
 * RETURNS:
 *    TRUE if proposed change should be allowed, FALSE otherwise
 *---------------------------------------------------------------------*/
/*ARGSUSED*/
static boolean 
lecmib_if_rcvaddr_screen (const uchar *rcvaddr,
			  uint rcvaddrlen,
			  boolean changeIsAdd,
			  boolean testPhase,
			  const snmpidbtype *snmpidb)
{
    return FALSE;   /* don't allow any changes */
}


/*---------------------------------------------------------------------
 * lecmib_if_stack_screen
 *
 * IF-MIB check_if_stack callback
 * test the proposed ifStack binding for the specified IDBs
 * this may be test-phase or commit-phase of PDU processing.
 * INPUTS:
 *    higherif_ptr  == pointer to IDB of proposed upper layer
 *    lowerif_ptr  == pointer to IDB of proposed lower layer
 *    higherIfType == ifType of proposed upper layer
 *    changeIsAdd == TRUE if adding the ifStackEntry, FALSE if deleting it
 *    testPhase == TRUE if this is the test-phase; no action by agent yet
 *              == FALSE if commit phase; ifStack change should be applied
 * RETURNS:
 *    TRUE if proposed change should be allowed, FALSE otherwise
 *---------------------------------------------------------------------*/
/*ARGSUSED*/
static boolean 
lecmib_if_stack_screen (snmpidbtype *higheridb,
			snmpidbtype *loweridb,
			uint higherIfType,
			boolean changIsAdd,
			boolean testPhase)
{
    return FALSE;   /* don't allow any changes */
}


/*---------------------------------------------------------------------
 * lecmib_if_complete_entry
 *
 * complete fields that may have been unknown when the LEC ifEntry
 * was created. Called after the JOIN completes from the if-complete
 * registry callback.
 * INPUTS:
 *    lec_info == pointer to specific LEC to complete the IF-MIB for
 *---------------------------------------------------------------------*/
void lecmib_if_complete_entry (lec_info_t  *lec_info)
{
    /* register the MAC address in the ifRcvAddressTable */
    /* if (verify_mac_addr(&lec_info->mac))   { */
    if (TRUE)
    {
        reg_invoke_ifmib_create_rcvaddr(
	  get_lec_ifIndex(lec_info),
	  lec_info->normalMac,
	  STATIONLEN_IEEE48);
    }

    /* set correct ifName-- LEC/ATM<slot>/<hw-if>.<sub-if> */
    (void)sprintf(lec_info->lec_snmp->if_name, "LEC/%s",
		  lec_info->swidb->namestring);

    /* set correct ifDescr-- LEC.<lecIndex>.<lecid> */
    (void)sprintf(lec_info->lec_snmp->if_descr, "LEC.%d.%d",
		  get_lec_index(lec_info),
                  lec_info->lecid);

    /* set correct ifPhysAddr length */
    lec_info->lec_snmp->if_phys_addr_len = STATIONLEN_IEEE48;

    /* check completed ifStackTable */
    /*???*/

}  /* lecmib_if_complete_entry */

/*---------------------------------------------------------------------
 * lecmib_if_deregister_rcv_addr
 *
 * free's memory for registered MAC addresses of a client.  Called
 * when MAC addresses are unregistered (which at this point is only
 * during client termination).
 *
 * INPUTS:
 *    lec_info == pointer to specific LEC 
 *---------------------------------------------------------------------*/
void lecmib_if_deregister_rcv_addr (lec_info_t *lec_info)
{
    reg_invoke_ifmib_destroy_rcvaddr(get_lec_ifIndex(lec_info),
				     lec_info->normalMac,
				     STATIONLEN_IEEE48); 
}

/*---------------------------------------------------------------------
 * lecmib_if_init
 *
 * IF-MIB for LEC-MIB initialization.
 * reg_add's all the IF-MIB callbacks for AFLANE_8023
 * INPUTS:
 *    none
 * RETURNS:
 *    none
 *---------------------------------------------------------------------*/
void lecmib_if_init (void)
{
    reg_add_ifmib_get_operstatus(D_ifType_aflane8023,
			   lecmib_if_get_operstatus,
			   "lecmib_if_get_operstatus");

    reg_add_ifmib_get_adminstatus(D_ifType_aflane8023,
			   lecmib_if_get_adminstatus,
			   "lecmib_if_get_adminstatus");

    reg_add_ifmib_admin_change(D_ifType_aflane8023,
			   lecmib_if_admin_change,
			   "lecmib_if_admin_change");

    reg_add_ifmib_cntr32(D_ifType_aflane8023,
			 lecmib_if_cntr32,
			 "lecmib_if_cntr32");

    reg_add_ifmib_cntr64(D_ifType_aflane8023,
			 lecmib_if_cntr64,
			 "lecmib_if_cntr64");

    reg_add_ifmib_rcvaddr_screen(D_ifType_aflane8023,
			   lecmib_if_rcvaddr_screen,
			   "lecmib_if_rcvaddr_screen");

    reg_add_ifmib_stack_screen(D_ifType_aflane8023,
			   lecmib_if_stack_screen,
			   "lecmib_if_stack_screen");


#ifdef AFLANE_8025                             /* AFLANE_8025 support */
    /* enable these calls for token ring support 
     * first update the callback functions to support TR
     */
    reg_add_ifmib_get_operstatus(D_ifType_aflane8025,
			   lecmib_if_get_operstatus,
			   "lecmib_if_get_operstatus");

    reg_add_ifmib_get_adminstatus(D_ifType_aflane8025,
			   lecmib_if_get_adminstatus,
			   "lecmib_if_get_adminstatus");

    reg_add_ifmib_admin_change(D_ifType_aflane8025,
			   lecmib_if_admin_change,
			   "lecmib_if_admin_change");

    reg_add_ifmib_cntr32(D_ifType_aflane8025,
			 lecmib_if_cntr32,
			 "lecmib_if_cntr32");

    reg_add_ifmib_cntr64(D_ifType_aflane8025,
			 lecmib_if_cntr64,
			 "lecmib_if_cntr64");

    reg_add_ifmib_rcvaddr_screen(D_ifType_aflane8025,
			   lecmib_if_rcvaddr_screen,
			   "lecmib_if_rcvaddr_screen");

    reg_add_ifmib_stack_screen(D_ifType_aflane8025,
			   lecmib_if_stack_screen,
			   "lecmib_if_stack_screen");

#endif  /* AFLANE_8025 support */

}  /* lecmib_if_init */


/* END lecmib_if.c */
      
