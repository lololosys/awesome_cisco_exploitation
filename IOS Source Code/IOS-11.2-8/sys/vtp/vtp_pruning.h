/* $Id: vtp_pruning.h,v 1.12 1996/09/27 01:46:09 bpan Exp $
 * $Source: /wbu-sw/dev/synergy/syn_core/nmp/nmpflash/vtp/vtp_pruning.h,v $
 *------------------------------------------------------------------
 * vtp_pruning.h - Header file for VTP PRUNING features.
 *
 * Jun 1996, Bider Pan
 *
 * PRUNING related definitions added 
 * 
 * --Changes include:
 *	1. Modify structures in "vtp.h": vtp_trunk_itf, vlan_domain
 *	2. Add new structures: vtp_trunk_stat, vtp_vlan_status, join_msg
 *	3. Add new constants: MAX_NUM_VLANS (1024)
 *	4. Add new function prototype definitions
 *	5. "VTP_PRUNING" must be defined when compiling
 * --Search on keyword "VTP_PRUNING" for above details
 * --The following restrictions apply:
 *	1. The byte/bit string fields in domain & trunk structures are fixed-
 *	   length. This requires additional DRAM space about 0.25K per
 *	   domain and 3.875K per trunk link. For systems w/ memory constraint,
 *	   a variable-length scheme may be needed to dynamically adjust the 
 *	   size of these strings, but it is out of the scope of this 
 *	   implementation.
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_pruning.h,v $
 * Revision 1.12  1996/09/27  01:46:09  bpan
 * Initially added syslog support
 *
 * Revision 1.11  1996/09/25  01:31:26  bpan
 * -Added error recovery support if pruning events dropped due to lack of mbuf
 * -Created VTP_PRUNING_LOG_xxx for vtp pruning
 *
 * Revision 1.10  1996/09/20  23:21:11  bpan
 * Fixes after testing "pruning mode" capability
 *
 * Revision 1.9  1996/09/07  01:03:03  bpan
 * -Added more functions for engineering commands
 * -Modified function prototypes
 * -Modified tracing messages
 *
 * Revision 1.8  1996/08/30  01:34:34  bpan
 * Modified tracing messages
 *
 * Revision 1.7  1996/08/23  22:08:41  bpan
 * Changed core function prototypes for status/statistics
 * Changed trace category to PRUNING_TRACE
 *
 * Revision 1.6  1996/08/16  22:07:06  bpan
 * MAX_NUM_VLANS_BYTES/BITS changed to VLAN_BYTE/BIT_STR_SIZ
 *
 * Revision 1.5  1996/08/11  06:09:15  bpan
 * added more trace flags
 *
 * Revision 1.4  1996/08/03  06:30:33  bpan
 * add changes after unit test
 *
 * Revision 1.3  1996/07/19  02:21:12  bpan
 * Reorganized
 *
 * Revision 1.2  1996/07/13  01:00:41  bpan
 * minor changes
 *
 * Revision 1.1  1996/07/10  01:26:36  bpan
 * Initial check in
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VTP_PRUNING_H__
#define __VTP_PRUNING_H__

/*===================== New Defs for Pruning =============================*/

/* ----- pruning timer ticks ----- */
#define VTP_PRUNING_MAX_RANDOM_MS  100	 /* and up to 100ms jittered */
#define VTP_PRUNING_TICK_SEC	   6	 /* 6 seconds periodical tick */
#define VTP_PRUNING_TRIG_TICK_SEC  1	 /* 1 second for triggered Join */

#define NO_PRUNE_SUPPORT_TICK	160 /* T1=160 ticks */
#define PRUNE_SUPPORTED_TICK	3   /* T2=3 ticks */

/* ----- Join message structure ---- */
typedef struct {
    unsigned char version;		 /* ion number */
    unsigned char code;			 /* the type */
#define VTP_JOIN_MSG	0x04
    unsigned char reserved;		 /* reserved */
    unsigned char dname_length;		 /* the domain name length in bytes */
    unsigned char domain_name[32];	 /* the domain name, padded by zeros */
    unsigned short first_vlan;		 /* first isl vlan id (aligned) */
    unsigned short last_vlan;		 /* last isl vlan id (aligned) */
	
    /* followed by vlan joining bit string */
	
} join_msg, *join_msg_ptr;
#define MAX_JOIN_MSG_LEN	(sizeof(join_msg) + VLAN_BIT_STR_SIZ)

/*====================== End of New Defs for Pruning =====================*/




/*====================== FSM definitions ================*/

/*-------------------------------------------------------------------------
 * FSM:
 *   action code table, action table, next state table
 *------------------------------------------------------------------------*/

/* FSM = VTP_PRUNING_FSM */

typedef enum {
    FSM_S0 = 0,			    /* S0:null */
    FSM_S1,								 
    FSM_ST_DIS = FSM_S1,	    /* S1:non-forwarding or link down
				     * (pruned)*/  
    FSM_S2,								 
    FSM_ST_NO_PRUNE = FSM_S2,	    /* S2:No pruning (joined) */
    FSM_S3,
    FSM_ST_NORM_JOIN = FSM_S3,	    /* S3:Normal joined (joined) */
    FSM_S4,
    FSM_ST_NORM_PRUNE = FSM_S4,	    /* S4:Normal pruned (pruned) */
    
    FSM_STATES_NUM		    /* 5 */
} FSM_STATES;

typedef enum {
    FSM_E0 = 0,			    /* E0:null */
    FSM_E1,
    FSM_EVT_RX_VER1 = FSM_E1,	    /* E1:Rx a vtp version 1 msg */
    FSM_E2,
    FSM_EVT_RX_JOIN1 = FSM_E2,	    /* E2:Rx Join(vlan bit=1) */
    FSM_E3,
    FSM_EVT_RX_JOIN0 = FSM_E3,	    /* E3:Rx Join(vlan bit=0) */
    FSM_E4,
    FSM_EVT_FWD = FSM_E4,	    /* E4:SPT forwarding when Link up */
    FSM_E5,
    FSM_EVT_NON_FWD = FSM_E5,	    /* E5:SPT non forwarding */
    FSM_E6,
    FSM_EVT_TMR_NOEXP = FSM_E6,	    /* E6:T1,T2 not expired */
    FSM_E7,
    FSM_EVT_TMR_EXP = FSM_E7,	    /* E7:T1,T2 expired with rx Join(0)/Ver1*/
    FSM_E8,
    FSM_EVT_LNKDN = FSM_E8,	    /* E8:Link down */
    FSM_E9,
    FSM_EVT_LNKUP = FSM_E9,	    /* E9:Link up when SPT forwarding */
    FSM_E10,
    FSM_EVT_TMR_EXP_NOJ = FSM_E10,  /* E10:T1,T2 expired w/o rx any Join */
    
    FSM_EVENTS_NUM		    /* 10 */
} FSM_EVENTS;

/* additional events NOT in FSM state table: for debugging purpose */
#define FSM_ADDI_EVT_INELIGIBLE	 (FSM_EVENTS_NUM) /* pruning ineligible */
#define FSM_ADDI_EVT_ELIGIBLE	 (FSM_EVENTS_NUM+1) /* pruning eligible */
#define FSM_ADDI_EVT_TRANSPARENT (FSM_EVENTS_NUM+2) /* transparent mode */
#define FSM_ADDI_EVT_NONTRANSPARENT (FSM_EVENTS_NUM+3) /* non-trans mode */
#define FSM_ADDI_EVT_ADDTRUNK    (FSM_EVENTS_NUM+4) /* add trunk */
#define FSM_ADDI_EVT_VLANREFRESH (FSM_EVENTS_NUM+5) /* vlan refresh */
#define FSM_ALL_EVENTS_NUM	 (FSM_EVENTS_NUM+6)


/*====================== End of FSM definitions ================*/




/*======================     Macros    ================*/
/*====================== End of Macros ================*/


#endif	/*__VTP_PRUNING_H__*/
