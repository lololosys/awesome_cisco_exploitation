/* $Id: vtp_pruning.c,v 1.20 1996/10/08 02:29:33 bpan Exp $
 * $Source: /wbu-sw/dev/synergy/syn_core/nmp/nmpflash/vtp/vtp_pruning.c,v $
 *------------------------------------------------------------------
 * vtp_pruning.c 
 *  Optimized flooding (pruning) for VTP (portable code module)
 * 
 * Notes:
 *  1. Even though this code is written portability as much as possible,
 *     some platform related trace functions are included for debugging 
 *     purpose.
 *  2. The worst case for vtp pruning is not a temporary
 *     occurrence, instead, it is permanently existing if happened. For 
 *     example, if user configured 1000 vlans, these vlans will permanently
 *     exist until user changes. Under this situation, these vlans will be
 *     handled at least every 6 seconds (received periodical Join msg), or
 *     sometimes even worse 1 second (received triggered Join msg). 
 *     In this implementation, the worst case is always put into concern
 *     that do not kill the system once it happens. 
 *     This will sacrifice performance for small amount of vlans, but make 
 *     it safe for the worst case
 *  3. "joining" means we're joining a vlan, so neighbor must send traffic for
 *		that vlan
 *     "joined" means neighbor's joining a vlan, so we must send traffic for
 *		that vlan
 *  4. vtp_process_reschedule are used in this code for c5000 platform, it may
 *     not need to be called so often in other platforms
 *
 * Jun 1996, Bider Pan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_pruning.c,v $
 * Revision 1.20  1996/10/08  02:29:33  bpan
 * Set management vlan to be always "vlan local assoc"
 *
 * Revision 1.19  1996/10/03  20:23:45  bpan
 * Removed redundant "\n" from syslog msg
 *
 * Revision 1.18  1996/10/01  19:24:10  bpan
 * Cleanup and reorganized trace & syslog msg for vtp pruning
 *
 * Revision 1.17  1996/09/27  01:46:08  bpan
 * Initially added syslog support
 *
 * Revision 1.16  1996/09/25  01:31:21  bpan
 * -Added error recovery support if pruning events dropped due to lack of mbuf
 * -Created VTP_PRUNING_LOG_xxx for vtp pruning
 *
 * Revision 1.15  1996/09/20  23:21:09  bpan
 * Fixes after testing "pruning mode" capability
 *
 * Revision 1.14  1996/09/18  00:34:20  bpan
 * Initially added "pruning en/disable" supports
 *
 * Revision 1.13  1996/09/14  04:54:07  bpan
 * -Changed vtp_add/change_domain prototypes to support "pruning dis/enable"
 * -Modified existing functions to support "pruning dis/enable"
 *
 * Revision 1.12  1996/09/09  23:16:58  bpan
 * Modifed tracing msgs to display more useful info
 *
 * Revision 1.11  1996/09/07  01:03:01  bpan
 * -Added more functions for engineering commands
 * -Modified function prototypes
 * -Modified tracing messages
 *
 * Revision 1.10  1996/08/31  01:06:48  bpan
 * Fixed wrong message when rx join from different domain
 * Modified some tracing messages
 *
 * Revision 1.9  1996/08/30  01:34:33  bpan
 * Modified tracing messages
 *
 * Revision 1.8  1996/08/28  02:04:52  bpan
 * Changed vtp pruning debugging messages
 *
 * Revision 1.7  1996/08/23  22:08:40  bpan
 * Changed core function prototypes for status/statistics
 * Changed trace category to PRUNING_TRACE
 *
 * Revision 1.6  1996/08/16  22:06:06  bpan
 * MAX_NUM_VLANS_BYTES/BITS changed to VLAN_BYTE/BIT_STR_SIZ (defined in vtp_pruning.h)
 * Changed vtp_pruning_add_vlan/_init/_refresh logic to allow being called after multiple vlans added.
 * Changed display format for "trunk not found" failures
 *
 * Revision 1.5  1996/08/11  06:03:40  bpan
 * Added changes for integration test
 *
 * Revision 1.4  1996/08/03  06:29:56  bpan
 * add changes after unit test
 *
 * Revision 1.3  1996/07/19  02:20:14  bpan
 * Add more functions
 *
 * Revision 1.2  1996/07/13  00:09:30  bpan
 * Add more functions
 *
 * Revision 1.1  1996/07/10  01:29:02  bpan
 * Initial check-in vtp pruning code
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-------------------------------------------------------------------------
 * Note: See vtp.c for pre-cautions
 *------------------------------------------------------------------------*/
#include "vtp_externs.h"
#include "vtp_debug.h"
#include "vtp_interface.h"
#include "vtp_pub.h"
#include "vtp_private.h"
#include "vtp_to_mib.h"
#include "vtp_pruning.h"
#include "vtp_local.h"    /* especially for pruning now */
#include "vtp_pub.h"

/*-------------------------------------------------------------------------
 * Definitions:
 *------------------------------------------------------------------------*/

/************************* FSM state table *************************/

/* action codes: used to dispatch functions */
/* some of the actions (prefix '-') combined from smaller actions as in spec */
typedef enum {
    FSM_A0_NULL = 0,		    /* a0:do nothing */
    FSM_A1_RST_T1,		    /* a1:Reset T1 */
    FSM_A2_RST_T2,		    /* a2:Reset T2 */
    FSM_A3_UPD_TICK,		    /* a3:Update T1,T2 if not expired */
    FSM_A4_NOPRUNE,		    /* a4:Set vlan joined, T1,T2=expired */
    FSM_A5_PRUNE,		    /* a5:set vlan to pruned */
    FSM_A6_JOIN_ON_VER1,	    /* a6:Set vlan to joined, reset T1 */
    FSM_A7_JOIN_ON_JOIN1,	    /* a7:Set vlan to joined, reset T2 */
    FSM_A8_JOIN_ON_JOIN0,	    /* a8:do nothing */
    
    FSM_ACTS_NUM
} FSM_ACTS;

/* action table: indexed by action code */
/* Note: this array must follow the order in the above enum: FSM_ACTS */
const void
(*vtp_pruning_fsm_act_table[FSM_ACTS_NUM])(vtp_trunk_itf_ptr trunk_p, 
					   unsigned short isl_vlan_id,
					   unsigned char event) = {
	vtp_pruning_fsm_act0_null,
	vtp_pruning_fsm_act1_reset_t1,
	vtp_pruning_fsm_act2_reset_t2,
	vtp_pruning_fsm_act3_update_t1t2,
	vtp_pruning_fsm_act4_goto_noprune,
	vtp_pruning_fsm_act5_prune,
	vtp_pruning_fsm_act6_set_join_on_ver1,
	vtp_pruning_fsm_act7_set_join_on_join1,
	vtp_pruning_fsm_act8_set_join_on_join0,
};

/* action code table: indexed by event code & current state */
const unsigned char
vtp_pruning_fsm_act_code_table[FSM_EVENTS_NUM][FSM_STATES_NUM] = {
	 /* Nul S1      S2		S3		S4 */
	 /*	dis/    no		normal		normal	 */
	 /*	down    prune		joined		pruned	 */
	 /* -------------------------------------------------	 */
/*Nul*/	    {0,	0,	0,		0,		0		},
/*E1=ver1*/ {0, 0,      FSM_A1_RST_T1,	FSM_A1_RST_T1,	FSM_A6_JOIN_ON_VER1 },
/*E2=Join1*/{0, 0,	FSM_A2_RST_T2,	FSM_A2_RST_T2,	FSM_A7_JOIN_ON_JOIN1},
/*E3=Join0*/{0, 0,	FSM_A5_PRUNE,	FSM_A8_JOIN_ON_JOIN0,	0	},
/*E4=fwd*/  {0, FSM_A4_NOPRUNE, 0,	0,		0		},
/*E5=nofwd*/{0, 0,	FSM_A5_PRUNE,	FSM_A5_PRUNE,	FSM_A0_NULL	},
/*E6=noexp*/{0, 0,	0,		FSM_A3_UPD_TICK,0		},
/*E7=exp*/  {0, 0,	0,		FSM_A5_PRUNE,	0		},
/*E8=down*/ {0, 0,	FSM_A5_PRUNE,	FSM_A5_PRUNE,	FSM_A0_NULL	},
/*E9=up*/   {0, FSM_A4_NOPRUNE, 0,	0,		0		},
/*E10expNJ*/{0, 0,	0,		FSM_A5_PRUNE,	0		}
};


/* next state table: indexed by event code & current code */
const unsigned char
vtp_pruning_fsm_next_state_table[FSM_EVENTS_NUM][FSM_STATES_NUM] = {
	  /* Nul     S1	     S2	     S3	     S4		 */
	  /*	     dis/    no	     normal  normal	 */
	  /*	     down    prune   joined  pruned	 */
	  /* ---------------------------------------	 */
/*Nul*/	    {FSM_S0, FSM_S1, FSM_S2, FSM_S3, FSM_S4},
/*E1=ver1*/ {FSM_S0, FSM_S1, FSM_S3, FSM_S3, FSM_S3},
/*E2=Join1*/{FSM_S0, FSM_S1, FSM_S3, FSM_S3, FSM_S3},
/*E3=Join0*/{FSM_S0, FSM_S1, FSM_S4, FSM_S3, FSM_S4},
/*E4=fwd*/  {FSM_S0, FSM_S2, FSM_S2, FSM_S3, FSM_S4},
/*E5=nofwd*/{FSM_S0, FSM_S1, FSM_S1, FSM_S1, FSM_S1},
/*E6=noexp*/{FSM_S0, FSM_S1, FSM_S2, FSM_S3, FSM_S4},
/*E7=exp*/  {FSM_S0, FSM_S1, FSM_S2, FSM_S4, FSM_S4},
/*E8=down*/ {FSM_S0, FSM_S1, FSM_S1, FSM_S1, FSM_S1},
/*E9=up*/   {FSM_S0, FSM_S2, FSM_S2, FSM_S3, FSM_S4},
/*E10expNJ*/{FSM_S0, FSM_S1, FSM_S2, FSM_S4, FSM_S4}
};


/*-------------------------------------------------------------------------
 * Globals:
 *------------------------------------------------------------------------*/

/* for trace & syslog */
unsigned char *vtp_pruning_prefix = "\nPRUNING:";

/* for debugging: used by trans mode & pruning ineligible */
unsigned char vtp_pruning_fsm_special_event = 0;

/* for FSM display */
unsigned char *vlan_state_str[ FSM_STATES_NUM ] = { 
    "Null", "Disabled", "NoPrune", "Joined ", "Pruned " };

unsigned char *vlan_event_str[ FSM_ALL_EVENTS_NUM ] = { 
    "Null", "RxVer1", "RxJ1", "RxJ0", "Fwd", "NonFwd",
    "Tick  ", "Tmo   ", "LnkDn ", "LnkUp ", "TmoNoJ",
    "Inelig", "Eligi ", "Trans ", "NoTran", "AddTrk", "Refrsh" };
    
/* for general display */
unsigned char *no_domain_str = "(no domain)";
unsigned char *no_pruning_str = "(no pruning)";
unsigned char *vtp_null_str = "";

/* for error trace */
unsigned long no_buf_cnt = 0;

/* for controlling display frequency on error rx events */
unsigned char	flag_join_domain_not_found, flag_join_trunk_not_found;
unsigned char	flag_join_at_transparent, flag_join_at_disable;
unsigned char	flag_join_no_domain, flag_join_no_buf;


/*-------------------------------------------------------------------------
 * Externals:
 *------------------------------------------------------------------------*/
extern unsigned char vtp_null_domain_name[]; /* for domain validation */

/*-------------------------------------------------------------------------
 * Macros:
 *------------------------------------------------------------------------*/
/*
 * Note: the following macro totally depends on the implementation of vtp.c
 */
#define is_no_domain_state(d)	( no_domain_state || (d)->transitioning )

/* use traditional vtp search routines */
#define vtp_pruning_search_domain	vtp_search_domain
#define vtp_pruning_search_trunk_link	vtp_find_trunk_link_in_domain


/**************************************************************************
 * 
 *
 *	******************** Pruning code starts here ********************
 *
 *
 **************************************************************************/


/*=========================================================================
 * 
 * PRUNING Event Dispatchers:	PART I- interface to dependent element
 *
 *	Generate pruning events and go for pruning FSM handling.
 *	Events categorized as: 
 *
 *	affect trunk		affect all vlans	affect one vlan
 *	------------------------------------------------------------		
 *	rx ver1 pkt		rx Join msg		*spt state change
 *	*link state change	timeout			*pruning-eligibility
 *							*local assoc change
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_core_change_trunk_link_state:
 *	The interface for dependent element to change trunk link state
 *
 * Note: it may be called back by vtp_dep_trunk_change_notification, so
 *	 check if during "add_trunk" session is necessary (refer to 
 *	 vtp_pruning_add_trunk_init())
 *------------------------------------------------------------------------*/
void
vtp_core_change_trunk_link_state (unsigned char dname_length, 
				  unsigned char *domain_name, void *trunk_id, 
				  unsigned char up_flag )
{	
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned char	*fmt = "%s chg trunk %s link st to %x %s\n";
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%schg link"
				" st:domain %s(%d) not found",
				domain_name, dname_length);  
	return;
    }
    
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%schg link"
				" st:trunk(%x) not found(%s)",
				trunk_id, domain_p->domain_name);  
	return;
    }
    
    /* ----- check parameters ----- */
    if ( up_flag != TRUNK_LINK_UP && up_flag != TRUNK_LINK_DOWN ) {
	VTP_PRUNING_LOG_WARNING(INVLNKST, "%schg link st:inv"
				" link st: %x", up_flag); 
	return;
    }
    
    /* ----- chk if change needed, & store link state ----- */
    if ( trunk_p->link_state != up_flag ) {
			
	trunk_p->link_state = up_flag;
    
	/* if no domain state, no FSM */
	if ( is_no_domain_state(domain_p) ) {
	    VTP_PRUNING_LOG_TRACE(NOTICE, fmt, trunk_p->trunk_name,
				  up_flag, no_domain_str); 
	    return;
	}
    
	/* if transparent mode, no FSM */
	if ( is_pruning_mode_stopped(domain_p) ) {
	    VTP_PRUNING_LOG_TRACE(NOTICE, fmt, trunk_p->trunk_name,
				  up_flag, no_pruning_str); 
	    return;
	}
    
	VTP_PRUNING_LOG_TRACE(NOTICE, fmt, trunk_p->trunk_name,
			      up_flag, vtp_null_str); 
	
	/* invoke FSM */
	vtp_pruning_fsm_linkst(trunk_p, up_flag);
    }

}   /*vtp_core_change_trunk_link_state*/

/*-------------------------------------------------------------------------
 * vtp_core_change_trunk_vlan_spt_state:
 *	The interface for dependent element to change vlan spt state
 *	It affects both the joining bit string & state variable (FSM)
 *
 * Note: it may be called back by vtp_dep_trunk_change_notification, so
 *	 check if during "add_trunk" session is necessary (refer to 
 *	 vtp_pruning_add_trunk_init())
 *------------------------------------------------------------------------*/
void
vtp_core_change_trunk_vlan_spt_state (unsigned char dname_length, 
				      unsigned char *domain_name, 
				      unsigned short isl_vlan_id, 
				      void *trunk_id, 
				      unsigned short new_spt_state) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short	offset;
    unsigned char	mask;
    unsigned char	*fmt = "%s chg trunk %s vlan %d SPT st to %x %s";

    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%schg SPT st:domain"
				" %s(%d) not found", domain_name,
				dname_length);  
	return;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%schg SPT"
				" st:trunk(%x) not found (domain %s)",
				trunk_id, domain_p->domain_name); 
	
	return;
    }
	
    /* ----- find vlan existence ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if ( !(domain_p->vlan_exist[offset] & mask) ) {
	
	/* Note: this msg may come out when deleting a vlan */
	VTP_PRUNING_LOG_WARNING(NOVLAN, "%schg SPT st:vlan %d"
				" not found (%s)", isl_vlan_id,
				domain_p->domain_name);  
	return;
    }
	
    /* ----- check parameter ----- */
    if (new_spt_state != VLAN_SPT_STATE_FWD && 
	new_spt_state != VLAN_SPT_STATE_NON_FWD ) {
	VTP_PRUNING_LOG_WARNING(INVSPTST, "%schg SPT st:inv"
				" SPT state: %x", new_spt_state); 
	return;
    }
      
    /* ----- chk if change needed, & store spt state ----- */
    if (((new_spt_state == VLAN_SPT_STATE_FWD) && 
	 !(trunk_p->spt_state[offset] & mask)) ||
	((new_spt_state == VLAN_SPT_STATE_NON_FWD) && 
	 (trunk_p->spt_state[offset] & mask))) {
	
	/* save new spt state */
	if (new_spt_state == VLAN_SPT_STATE_FWD) {
	  trunk_p->spt_state[ offset ] |= mask;
      }	else {
	  trunk_p->spt_state[ offset ] &= ~mask;
      }
    
	/* if no domain state, stop here */
	if ( is_no_domain_state(domain_p) ) {
	    VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->trunk_name,
				  isl_vlan_id, new_spt_state,
				  no_domain_str); 
	    return;
	}
    
	/* if transparent mode, no rebuild joining bit, no FSM */
	if ( is_pruning_mode_stopped(domain_p) ) {
	    VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->trunk_name,
				  isl_vlan_id, new_spt_state,
				  no_pruning_str); 
	    return;
	}
	
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->trunk_name,
			      isl_vlan_id, new_spt_state, vtp_null_str); 
	
	/* ----- (I) vlan joining bit: rebuild & chk triggered Join ----- */
	vtp_pruning_build_joining_bit( domain_p, trunk_p, isl_vlan_id );
	vtp_pruning_process_trig_join( trunk_p ); /* on this trunk only */
	  
	/* ----- (II) state variable: invoke FSM ----- */
	vtp_pruning_fsm_spt(trunk_p, isl_vlan_id, new_spt_state);
	
    }	/*if_spt-state-change)*/
	
} /*vtp_core_change_trunk_vlan_spt_state*/

/*-------------------------------------------------------------------------
 * vtp_core_change_trunk_all_vlan_spt_state:
 *	The interface for dependent element to change ALL vlan spt state
 *	It affects both the joining bit string & state variable (FSM)
 *
 * Note: it may be called back by vtp_dep_trunk_change_notification, so
 *	 check if during "add_trunk" session is necessary (refer to 
 *	 vtp_pruning_add_trunk_init())
 * Note: *spt_state must be large enough to contain ALL vlans, one bit per
 *	 vlan's spt state
 *------------------------------------------------------------------------*/
void
vtp_core_change_trunk_all_vlan_spt_state (unsigned char dname_length, 
					  unsigned char *domain_name, 
					  void *trunk_id, 
					  unsigned char *spt_state) { 
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned char	*fmt = "%s trunk %s chg ALL SPT st %s";

    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%schg all SPT"
				" st:domain %s(%d) not found",
				domain_name, dname_length);  
	return;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%schg ALL SPT"
				" st:trunk(%x) not found (%s)",
				trunk_p, domain_p->domain_name);  
	return;
    }
	
    /* domain and trunk determined */
    
    /* save new spt state */
    memcpy( trunk_p->spt_state, spt_state, VLAN_BIT_STR_SIZ ); 
	
    /* ----- if no domain state, stop here ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->trunk_name, no_domain_str);
	return;
    }
    
    /* ----- if transparent mode, no rebuild join bit & FSM, just save ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->trunk_name, no_pruning_str);
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->trunk_name, vtp_null_str);
   
    /* ----- (I) vlan joining bit: rebuild & chk triggered Join ----- */
    vtp_pruning_build_all_joining_bits( domain_p, trunk_p );
    vtp_pruning_process_trig_join( trunk_p ); /* on this trunk only */
	  
    /* ---- (II) FSM for all vlans & rebuild other trunk's joining bit ---- */
    vtp_pruning_fsm_spt_all(trunk_p);
	
} /*vtp_core_change_trunk_all_vlan_spt_state*/

/*-------------------------------------------------------------------------
 * vtp_core_change_vlan_local_assoc:
 *	The interface for dependent element to change vlan local association
 *	It affects joining bit only
 *
 * Note: it may be called back by vtp_vlan_change_notification, so
 *	 check if during "add_vlan" session is necessary (refer to 
 *	 vtp_pruning_add_vlan(_init))
 *------------------------------------------------------------------------*/
void
vtp_core_change_vlan_local_assoc (unsigned char dname_length, 
				  unsigned char *domain_name, 
				  unsigned short isl_vlan_id, 
				  unsigned char flag) {
    vlan_domain_ptr	domain_p;
    unsigned short	offset;
    unsigned char	mask;
    unsigned char	*fmt = "%s domain %s vlan %d chg local assoc to %x %s";
	
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%schg local"
				" assoc:domain %s(%d) not found",
				domain_name, dname_length);  
	return;
    }

    /* ----- find vlan existence ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if ( !(domain_p->vlan_exist[offset] & mask) ) {
	VTP_PRUNING_LOG_WARNING(NOVLAN, "%schg local"
				" assoc:vlan %d not found (%s)",
				isl_vlan_id, domain_name);  
	return;
    }
    
    /* ----- check paramater ----- */
    if (flag != VLAN_LOCAL_ASSOC && flag != VLAN_LOCAL_ASSOC_NONE) {
	VTP_PRUNING_LOG_WARNING(INVASSOC, "%schg local"
				" assoc:inv vlan local assoc: %x"
				" (%s)", flag, domain_name); 
	return;
    }
    
    /* ----- chk if change needed, & store local assoc into bit string ----- */
    if (((flag == VLAN_LOCAL_ASSOC) && 
	 !(domain_p->vlan_local_assoc[offset] & mask)) ||
	((flag == VLAN_LOCAL_ASSOC_NONE) &&
	 (domain_p->vlan_local_assoc[offset] & mask))) {
    	
	/* save new vlan local assoc */
	if (flag == VLAN_LOCAL_ASSOC)
	  domain_p->vlan_local_assoc[ offset ] |= mask;
	else 
	  domain_p->vlan_local_assoc[ offset ] &= ~mask;

    
	/* if no domain state, stop here */
	if ( is_no_domain_state(domain_p) ) {
	    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
				  isl_vlan_id, flag, no_domain_str); 
	    return;
	}
    
	/* if transparent mode, no rebuild joining bit, no FSM */
	if ( is_pruning_mode_stopped(domain_p) ) {
	    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
				  isl_vlan_id, flag, vtp_null_str); 
	    return;
	}
	
	VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
			      isl_vlan_id, flag, vtp_null_str); 
	
	/* ----- rebuild this vlan joining bit on all trunks ----- */
	vtp_pruning_build_all_other_joining_bits( domain_p, NULL,
						  isl_vlan_id );  
	/* all trunk, this vlan affected */
	
	/* ----- chk triggered Join conditions on all trunks ----- */
	vtp_pruning_process_all_other_trig_joins( domain_p, NULL ); 
	
    }	/*if(vlan-local-assoc-change)*/

} /*vtp_core_change_vlan_local_assoc*/

/*-------------------------------------------------------------------------
 * vtp_core_change_all_vlan_local_assoc:
 *	The interface for dependent element to change ALL vlan local
 *      association it affects joining bit only
 *
 * Note: it may be called back by vtp_domain_change_notification, so
 *	 check if during "add_vlan" session is necessary (refer to 
 *	 vtp_pruning_add_vlan(_init))
 * Note: *local_assoc must be large enough to contain ALL vlans, one bit per
 *	 vlan's local assoc
 *------------------------------------------------------------------------*/
void
vtp_core_change_all_vlan_local_assoc (unsigned char dname_length, 
				      unsigned char *domain_name, 
				      unsigned char *vlan_local_assoc) {
    vlan_domain_ptr	domain_p;
    unsigned char	*fmt = "%s chg %s ALL vlan local assoc %s";
	
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%schg ALL local"
				" assoc:domain %s(%d) not found",
				domain_name, dname_length);  
	return;
    }
	    
    /* save new local assoc */
    memcpy(domain_p->vlan_local_assoc, vlan_local_assoc,
	   VLAN_BIT_STR_SIZ );
	
    /* ----- if no domain state, stop here ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
			      no_domain_str); 
	return;
    }
    
    /* ----- if transparent mode, no rebuild joining bit, no FSM ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
			      no_pruning_str); 
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name, vtp_null_str);
    
    /* ----- rebuild all vlan joining bits on all trunks ----- */
    vtp_pruning_build_all_other_joining_bits( domain_p, NULL, 0 );
    /* all trunk, all vlan affected */
	
    /* ----- chk triggered Join condition on all trunks ----- */
    vtp_pruning_process_all_other_trig_joins( domain_p, NULL );

} /*vtp_core_change_all_vlan_local_assoc*/

/*-------------------------------------------------------------------------
 * vtp_core_change_vlan_pruning_eligible:
 *	The interface for dependent element to change pruning-eligibility
 *	This event will not go thru FSM, but may trigger Join msg
 *
 * Note: it may be called back by vtp_pruning_domain_change_notification, so
 *	 check if during "add_domain" session is necessary (refer to 
 *	 vtp_pruning_add_domain_init())
 *------------------------------------------------------------------------*/
void
vtp_core_change_vlan_pruning_eligible (void* trunk_id,
				       unsigned char *vlan_prune_eligibility)
{
    vtp_trunk_itf_ptr   trunk_p;
    unsigned char	*new, *old;
    unsigned char	*fmt = "%s change %s pruning eligibility %s";
    vtp_boolean         need_fsm;
    vtp_trunk_info      old_vals, new_vals;

    need_fsm = TRUE;
    trunk_p = vtp_find_trunk_link(trunk_id);
    if (!trunk_p) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%schg pruning"
				" eligible:trunk %x not found. (%s)",
				trunk_id, "(no domain given)");  
	return;
    }

    retrieve_trunk_info(trunk_p, &old_vals);
    /* ----- mask off for default vlans ----- */
    old = trunk_p->vlan_pruning_eligibility;
    new = vlan_prune_eligibility;
	
    /* default vlans always NON-pruning-eligible. 
     * overwrite them, a '1' means non-pruning-eligible 
     */
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL,VTP_VLAN_BASE)] &= 
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL,VTP_VLAN_BASE); 
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_TR_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_TR_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL,VTP_VLAN_BASE);
	
    /* ----- if no domain state, stop here ----- */
    if ( is_no_domain_state(trunk_p->domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt,
			      trunk_p->domain_p->domain_name,
			      no_domain_str);  
	need_fsm = FALSE;
    }
    
    /* ----- if transparent mode, no rebuild join bit & FSM, just save ----- */
    if ( is_pruning_mode_stopped(trunk_p->domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->domain_p->domain_name,
			      no_pruning_str); 
	need_fsm = FALSE;
    }
	
    /* ----- if during add_domain, just save ----- */
    if ( trunk_p->domain_p->status & DOMAIN_STS_ADD_DOMAIN ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->domain_p->domain_name,
			      "during adding domain"); 
	need_fsm = FALSE;
    }

    if (need_fsm) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->domain_p->domain_name, 
			      vtp_null_str); 
    
	/* ----- FSM handling firs	t (don't save yet) ----- */
	vtp_pruning_fsm_eligible(trunk_p->domain_p, trunk_p, new);
    }
	
    /* ----- save new settings ----- */
    memcpy( old, new, VLAN_BIT_STR_SIZ );
    retrieve_trunk_info(trunk_p, &new_vals);
    vtp_trunk_change_notification(VTP_MODIFY, &old_vals, &new_vals);

    if (need_fsm) {
	
	/* ----- rebuil	d all vlan joining bits on all trunks ----- */
	vtp_pruning_build_all_other_joining_bits( trunk_p->domain_p, NULL, 0 );
	/* all trunks all vlans affected */
	
	/* -	---- chk triggered Join condition on all trunks ----- */
	vtp_pruning_process_all_other_trig_joins( trunk_p->domain_p, NULL ); 
	/* all trunk aff	ected */
    }
	
} /*vtp_core_change_vlan_pruning_eligible*/

/*-------------------------------------------------------------------------
 *	The interface for dependent element to change pruning-eligibility
 *	This event will not go thru FSM, but may trigger Join msg
 *
 * Note: it may be called back by vtp_pruning_domain_change_notification, so
 *	 check if during "add_domain" session is necessary (refer to 
 *	 vtp_pruning_add_domain_init())
 *------------------------------------------------------------------------*/
void
vtp_core_set_vlan_eligible_on_trunk (void* trunk_id,
				     unsigned short vlan_id, 
				     vtp_boolean is_eligible) {
    vtp_trunk_itf_ptr  trunk_p;
    unsigned char	*fmt = "%s change %s pruning eligibility %s";
    unsigned char      new[VLANS_BITMASK_SIZE];
    vtp_boolean        need_fsm;
    vtp_trunk_info     old_vals, new_vals;

    need_fsm = TRUE;
    trunk_p = vtp_find_trunk_link(trunk_id);
    if (!trunk_p) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%schg pruning"
				" eligible:trunk %x not found. (%s)",
				trunk_id, "(no domain given)");  
	return;
    }

    if (vtp_is_default_vlan_id(vlan_id)) {
	VTP_PRUNING_LOG_WARNING(ISDEFAULT, "%schg pruning"
				" eligible:cannot change eligibility"
				" of defaul VLAN %d", vlan_id); 
	return;
    }

    retrieve_trunk_info(trunk_p, &old_vals);

    memcpy(new, trunk_p->vlan_pruning_eligibility, VLANS_BITMASK_SIZE);
    if (is_eligible) {
	new[vlan_bit_offset(vlan_id, 0)] |= vlan_bit_mask(vlan_id, 0);
    } else {
	new[vlan_bit_offset(vlan_id, 0)] &= ~vlan_bit_mask(vlan_id, 0);	
    }
	
    /* ----- if no domain state, stop here ----- */
    if ( is_no_domain_state(trunk_p->domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt,
			      trunk_p->domain_p->domain_name,
			      no_domain_str); 
	need_fsm = FALSE;
    }
    
    /* ----- if transparent mode, no rebuild join bit & FSM, just save ----- */
    if ( is_pruning_mode_stopped(trunk_p->domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->domain_p->domain_name,
			      no_pruning_str); 
	need_fsm = FALSE;
    }
	
    /* ----- if during add_domain, just save ----- */
    if ( trunk_p->domain_p->status & DOMAIN_STS_ADD_DOMAIN ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->domain_p->domain_name,
			      "during adding domain"); 
	need_fsm = FALSE;
    }

    if (need_fsm) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, trunk_p->domain_p->domain_name, 
			      vtp_null_str); 

	/* ----- FSM handling first --	--- */
	vtp_pruning_fsm_eligible(trunk_p->domain_p, trunk_p, new);
    }

    trunk_p->vlan_pruning_eligibility[vlan_bit_offset(vlan_id, 0)] = 
	new[vlan_bit_offset(vlan_id, 0)];
    retrieve_trunk_info(trunk_p, &new_vals);
    vtp_trunk_change_notification(VTP_MODIFY, &old_vals, &new_vals);

    if (need_fsm) {
	/* ----- rebuil	d all vlan joining bits on all trunks ----- */
	vtp_pruning_build_all_other_joining_bits( trunk_p->domain_p, NULL, 0 );
	/* all trunks all vlans affected */
	
	/* ----- chk triggered Join condition on all trunks ----- */
	vtp_pruning_process_all_other_trig_joins( trunk_p->domain_p, NULL ); 
	/* all trunk affected */
    }
	
} /*vtp_core_change_vlan_pruning_eligible*/






/*=========================================================================
 * 
 *		----- interface to dependent element -----
 *			get vlan joining/pruning status
 *			get general status (local assoc/spt state)
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_core_get_trunk_pruning_status:
 *  Get vlan pruning status for a trunk
 *
 * Note: *pruning_status must be >= 128 bytes long
 *------------------------------------------------------------------------*/
vtp_boolean
vtp_core_get_trunk_pruning_status (unsigned char dname_length, 
				   unsigned char *domain_name, void *trunk_id,
				   unsigned char *pruning_status) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sget pruning"
				" status: domain %s(%d) not found",
				domain_name, dname_length);  
	return FALSE;
    }
    
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sget pruning"
				" status:trunk %x not found (%s)",
				trunk_id, domain_p->domain_name);
	return FALSE;
    }
    
    /* ----- convert vlan fsm state to bit string ----- */
    vtp_pruning_build_joined_bits(trunk_p, pruning_status);
    
    return TRUE;
    
} /*vtp_core_get_trunk_pruning_status*/

/*-------------------------------------------------------------------------
 * vtp_core_get_vlan_pruning_status:
 *  Get vlan pruning status for a particular vlan on a trunk
 *
 *------------------------------------------------------------------------*/
Vtp_Err_t
vtp_core_get_vlan_pruning_status (unsigned char dname_length, 
				  unsigned char *domain_name, 
				  void *trunk_id,
				  unsigned short vlan_id, 
				  vtp_boolean *status) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sget pruning status:"
				" domain %s(%d) not found",
				domain_name, dname_length); 
	return VTP_ERROR_NO_DOMAIN;
    }
    
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sget joining"
				" status:trunk %x not found (%s)",
				trunk_id, domain_p->domain_name);  
	return VTP_ERROR_NO_TRUNK;
    }

    /* non-pruning-eligibility should be joined: even no_domain_state */
    /* ??? must be in spt forwarding state */
    if ((!(trunk_p->vlan_pruning_eligibility[vlan_bit_offset(vlan_id,
							     0)] &
	   vlan_bit_mask(vlan_id, 0))) ||
	trunk_p->vlan_fsm_state[vlan_id] == FSM_ST_NO_PRUNE || 
	trunk_p->vlan_fsm_state[vlan_id] == FSM_ST_NORM_JOIN ) {
	*status = TRUE;
    } else {
	*status = FALSE;
    }
    
    return VTP_OK;
    
} /*vtp_core_get_trunk_pruning_status*/

/*-------------------------------------------------------------------------
 * vtp_core_get_trunk_joining_status:
 *  Get vlan joining status for a trunk
 *
 * Note: *joining_status must be >= 128 bytes long
 *------------------------------------------------------------------------*/
vtp_boolean 
vtp_core_get_trunk_joining_status (unsigned char dname_length, 
				   unsigned char *domain_name,
				   void *trunk_id, 
				   unsigned char *joining_status) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sget joining"
				" status:domain %s(%d) not found",
				domain_name, dname_length);  
	return FALSE;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sget joining"
				" status:trunk %x not found (%s)",
				trunk_id, domain_p->domain_name	);  
	return FALSE;
    }
    
    /* ----- copy joining bit string ----- */
    memcpy( joining_status, trunk_p->cur_join_bits, VLAN_BIT_STR_SIZ ); 
    
    return TRUE;
    
} /*vtp_core_get_trunk_pruning_status*/

/*-------------------------------------------------------------------------
 * vtp_core_get_trunk_eligibility:
 *  Get vlan pruning eligibility for a trunk
 *
 * Note: *eligibility must be >= 128 bytes long
 *------------------------------------------------------------------------*/
vtp_boolean vtp_core_get_trunk_eligibility (void *trunk_id, 
					    unsigned char *eligibility) {
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- find trunk link ----- */
    trunk_p = vtp_find_trunk_link(trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sget joining"
				" status:trunk %x not found (%s)",
				trunk_id, "(no domain provided)");  
	return FALSE;
    }
    
    /* ----- copy joining bit string ----- */
    memcpy( eligibility, trunk_p->vlan_pruning_eligibility,
	   VLAN_BIT_STR_SIZ );  
    
    return TRUE;
    
} /*vtp_core_get_trunk_pruning_status*/

/*-------------------------------------------------------------------------
 * vtp_core_get_trunk_vlan_spt_state:
 *------------------------------------------------------------------------*/
unsigned short vtp_core_get_trunk_vlan_spt_state (unsigned char dname_length, 
						  unsigned char *domain_name,
						  unsigned short isl_vlan_id, 
						  void *trunk_id) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short	offset;
    unsigned char	mask;
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	return VLAN_SPT_STATE_NA;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	return VLAN_SPT_STATE_NA;
    }
	
    /* ----- also check vlan existence ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if (domain_p->vlan_exist[offset] &  mask) {
	return ((trunk_p->spt_state[offset] & mask)? 
		VLAN_SPT_STATE_FWD : VLAN_SPT_STATE_NON_FWD);
    } else {
	return VLAN_SPT_STATE_NA;
    }

}   /*vtp_core_get_trunk_vlan_spt_state*/


/*-------------------------------------------------------------------------
 * vtp_core_get_vlan_local_assoc:
 *------------------------------------------------------------------------*/
unsigned char
vtp_core_get_vlan_local_assoc (unsigned char dname_length, 
			       unsigned char *domain_name,
			       unsigned short isl_vlan_id) {
    vlan_domain_ptr	domain_p;
    unsigned short	offset;
    unsigned char	mask;

    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	return VLAN_LOCAL_ASSOC_NA;
    }
	
    /* ----- also check vlan existence ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if (domain_p->vlan_exist[offset] &  mask) {
	return ((domain_p->vlan_local_assoc[offset] & mask)?
		VLAN_LOCAL_ASSOC : VLAN_LOCAL_ASSOC_NONE);
    } else {
	return VLAN_LOCAL_ASSOC_NA;
    }

}   /*vtp_core_get_vlan_local_assoc*/


/*-------------------------------------------------------------------------
 * vtp_core_is_trunk_link_exist:
 *  check if the specified trunk exists
 *------------------------------------------------------------------------*/
vtp_boolean
vtp_core_is_trunk_link_exist (unsigned char dname_length, 
			      unsigned char *domain_name,
			      void *trunk_id) {
    vlan_domain_ptr	domain_p;
    
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if (domain_p) {
	if (vtp_pruning_search_trunk_link(domain_p, trunk_id))
	    return TRUE;
	else
	    return FALSE;
    } 
	
    return FALSE;
} 

/*-------------------------------------------------------------------------
 * vtp_core_get_vlan_exist:
 *  Return a bit map indicating the existing vlans, a '1' means the
 *  corresponding vlan exist, result stored in **vlan_bitmap
 *------------------------------------------------------------------------*/
void
vtp_core_get_vlan_exist (unsigned char dname_length, 
			 unsigned char *domain_name,  
			 unsigned char **vlan_bitmap) {
    vlan_domain_ptr	domain_p;
	
    /* ----- find domain ----- */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sget vlan exist:domain"
				" %s(%d) not found", domain_name,
				dname_length);  
	vlan_bitmap = NULL;
	return;
    }
	
    *vlan_bitmap = domain_p->vlan_exist;
      
} /*vtp_core_get_vlan_exist*/

/*-------------------------------------------------------------------------
 * vtp_core_is_vlan_exist:
 *  Check and return if the specified vlan existing, result stored in *exist
 *  (TRUE means exist, FLASE otherwise)
 *------------------------------------------------------------------------*/
vtp_boolean
vtp_core_is_vlan_exist (unsigned char dname_length, 
			unsigned char *domain_name,  
			unsigned short isl_vlan_id) {
    vlan_domain_ptr	domain_p;
    unsigned short	offset;
    unsigned char	mask;
	
    /* ----- find domain ----- */
    domain_p = vtp_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sis vlan exist:domain"
				" %s(%d) not found", domain_name,
				dname_length);   
	return FALSE;
    }
	
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if ( domain_p->vlan_exist[offset] & mask )
	return TRUE;

    return FALSE;
	
} /*vtp_core_is_vlan_exist*/


/*=========================================================================
 * 
 *		----- interface to dependent element -----
 *			get/clear statistics
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_core_get_trunk_statistics:
 *	Get statistics for a trunk (now only pruning related)
 *------------------------------------------------------------------------*/
vtp_boolean
vtp_core_get_trunk_statistics (unsigned char dname_length, 
			       unsigned char *domain_name, 
			       void *trunk_id,
			       vtp_trunk_stat *statistics) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;

    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sget trunk stat:domain"
				" %s(%d) not found", domain_name,
				dname_length); 	  
	return FALSE;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	return FALSE;	
    }
    
    /* ----- copy statistics ----- */
    memcpy(statistics, &trunk_p->pruning_stat, sizeof(vtp_trunk_stat) );
    return TRUE;
    
} /*vtp_core_get_trunk_statistics*/

/*-------------------------------------------------------------------------
 * vtp_core_clear_trunk_statistics:
 *  Clear statistics for a trunk (now only pruning related)
 *
 * input    clear_all: TRUE to clear all trunks	
 *		       FALSE to clear the specified trunk
 *------------------------------------------------------------------------*/
vtp_boolean 
vtp_core_clear_trunk_statistics (unsigned char clear_all, 
				 unsigned char dname_length, 
				 unsigned char *domain_name, void
				 *trunk_id) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sclr trunk stat:domain"
				" %s(%d) not found", domain_name,
				dname_length);   
	return FALSE;
    }
	
    /* domain determined */
    
    /* ----- clear all trunks ----- */
    if ( clear_all == TRUE ) {
	for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	     trunk_p->trunk_next) {
	    memset(&trunk_p->pruning_stat, 0, sizeof(vtp_trunk_stat));
	} 
	return TRUE;
    } else {
	/* find trunk link */
	trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
	if ( !trunk_p ) {
	    VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sclr stat:trunk %x not"
				    " found (%s)", trunk_id,
				    domain_p->domain_name);  
	    return FALSE;
	}
	memset(&trunk_p->pruning_stat,0,sizeof(vtp_trunk_stat));
    } 

    return TRUE;
	
} /*vtp_core_clear_trunk_statistics*/


/*=========================================================================
 * 
 *		--- interface to dependent element ---
 *			set pruning eligibility
 *			set local assoc
 *			set spt state
 *			set link state
 *			refresh (error recovery)
 *
 *	    Note: mainly called by dependent: vtp_xxx_change_notification()
 *		  (no database protection, so necessary protection 
 *		   should be done before calling)
 *
 *========================================================================*/
/*-------------------------------------------------------------------------
 * vtp_core_set_trunk_link_state:
 *	The interface for dependent element to setup trunk link state
 *
 * Note: it can be called only by dependent: vtp_trunk_change_notification()
 *------------------------------------------------------------------------*/
void
vtp_core_set_trunk_link_state (unsigned char dname_length, 
			       unsigned char *domain_name, void *trunk_id, 
			       unsigned char up_flag) {	
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned char	*fmt = "%s set domain %s trunk %s link st to %x";
    
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sset link st:domain %s(%d)"
				" not found", domain_name,
				dname_length);  
	return;
    }
    
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sset link st:trunk %x not"
				" found (%s)", trunk_id,
				domain_p->domain_name);  
	return;
    }
    
    /* domain and trunk determined */
    
    /* ----- check parameters ----- */
    if ( up_flag != TRUNK_LINK_UP && up_flag != TRUNK_LINK_DOWN ) {
	VTP_PRUNING_LOG_WARNING(INVLNKST, "%sset link st:inv trunk"
				" link st: %x", up_flag); 	
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
			  trunk_p->trunk_name, up_flag); 

    /* ----- save link state ----- */
    trunk_p->link_state = up_flag;
    
}   /*vtp_core_set_trunk_link_state*/

/*-------------------------------------------------------------------------
 * vtp_core_set_trunk_vlan_spt_state:
 *	The interface for dependent element to set vlan spt state
 *	It affects both the joining bit string & state variable (FSM)
 *
 * Note: it can be called only by dependent: vtp_trunk_change_notification()
 *	 or vtp_vlan_change_notification()
 *------------------------------------------------------------------------*/
void
vtp_core_set_trunk_vlan_spt_state (unsigned char dname_length, 
				   unsigned char *domain_name, 
				   unsigned short isl_vlan_id, 
				   void *trunk_id, 
				   unsigned short new_spt_state) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short	offset;
    unsigned char	mask;
    unsigned char	*fmt = "%s set domain %s trunk %s vlan %d SPT"
	" st to %x"; 	

    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sset SPT st:domain %s(%d)"
				" not found", domain_name,
				dname_length);  
	return;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sset SPT st:trunk %x not"
				" found (%s)", trunk_id,
				domain_p->domain_name);  
	return;
    }
	
    /* domain and trunk determined */
    
    /* ----- find vlan existence ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if ( !(domain_p->vlan_exist[offset] & mask) ) {
	VTP_PRUNING_LOG_WARNING(NOVLAN, "%sset SPT st:vlan (%d) not"
				" found (%s)", isl_vlan_id,
				domain_p->domain_name);
	return;
    }
	
    /* ----- check parameter ----- */
    if (new_spt_state != VLAN_SPT_STATE_FWD && 
	new_spt_state != VLAN_SPT_STATE_NON_FWD) {
	VTP_PRUNING_LOG_WARNING(INVSPTST, "%sset SPT st:inv SPT st:"
				" %x",new_spt_state);	 
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
			  trunk_p->trunk_name, isl_vlan_id,
			  new_spt_state); 
    
    /* ----- store spt state ----- */
    if ( new_spt_state == VLAN_SPT_STATE_FWD )
      trunk_p->spt_state[ offset ] |= mask;
    else
      trunk_p->spt_state[ offset ] &= ~mask;

} /*vtp_core_set_trunk_vlan_spt_state*/

/*-------------------------------------------------------------------------
 * vtp_core_set_trunk_all_vlan_spt_state:
 *	The interface for dependent element to set ALL vlan spt state
 *	It affects both the joining bit string & state variable (FSM)
 *
 * Note: it can be called only by dependent: vtp_trunk_change_notification()
 *------------------------------------------------------------------------*/
void
vtp_core_set_trunk_all_vlan_spt_state (unsigned char dname_length, 
				       unsigned char *domain_name, 
				       void *trunk_id, 
				       unsigned char *spt_state) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    unsigned char	*fmt = "%s set domain %s trunk %s ALL SPT st";

    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sset ALL SPT st:domain"
				" %s(%d) not found", domain_name,
				dname_length);  
	return;
    }
	
    /* ----- find trunk link ----- */
    trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id);
    if ( !trunk_p ) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sset ALL SPT st:trunk %x"
				" not found (%s)", trunk_p,
				domain_p->domain_name);  
	return;
    }
	
    /* domain and trunk determined */
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name,
			  trunk_p->trunk_name); 
    
    /* save new spt state */
    memcpy( trunk_p->spt_state, spt_state, VLAN_BIT_STR_SIZ ); 
	
} /*vtp_core_set_trunk_all_vlan_spt_state*/

/*-------------------------------------------------------------------------
 * vtp_core_set_vlan_local_assoc:
 *	The interface for dependent element to set vlan local association
 *	It affects joining bit only
 *
 * Note: it can be called only by dependent: vtp_vlan_change_notification()
 *------------------------------------------------------------------------*/
void
vtp_core_set_vlan_local_assoc (unsigned char dname_length, 
			       unsigned char *domain_name, 
			       unsigned short isl_vlan_id, 
			       unsigned char flag) {
    vlan_domain_ptr	domain_p;
    unsigned short	offset;
    unsigned char	mask;
    unsigned char	*fmt = "%s set domain %s vlan %d local assoc to %x";
	
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sset local assoc:domain"
				" %s(%d) not found", domain_name,
				dname_length);  
	return;
    }

    /* ----- find vlan existence ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if ( !(domain_p->vlan_exist[offset] & mask) ) {
	VTP_PRUNING_LOG_WARNING(NOVLAN, "%sset local assoc:vlan %d not"
				" found (%s)", isl_vlan_id,
				domain_name);  
	return;
    }
    
    /* ----- check parameters ----- */
    if (flag != VLAN_LOCAL_ASSOC && flag != VLAN_LOCAL_ASSOC_NONE) {
	VTP_PRUNING_LOG_WARNING(INVASSOC, "%sset local assoc:inv local"
				" assoc: %x (%s)", flag, domain_name);
	return;	
    }
	
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name, isl_vlan_id, flag);
    
    /* ----- chk if change needed, & store local assoc into bit string ----- */
    if ( flag == VLAN_LOCAL_ASSOC )
      domain_p->vlan_local_assoc[ offset ] |= mask;
    else
      domain_p->vlan_local_assoc[ offset ] &= ~mask;

} /*vtp_core_set_vlan_local_assoc*/

/*-------------------------------------------------------------------------
 * vtp_core_set_all_vlan_local_assoc:
 *	The interface for dependent element to set ALL vlan local association
 *	It affects joining bit only
 *
 * Note: it can be called only by dependent: vtp_domain_change_notification()
 *	 or vtp_vlan_change_notification()
 *------------------------------------------------------------------------*/
void
vtp_core_set_all_vlan_local_assoc (unsigned char dname_length, 
				   unsigned char *domain_name, 
				   unsigned char *vlan_local_assoc) {
    vlan_domain_ptr	domain_p;
    unsigned char	*fmt = "%s set domain %s ALL vlan local assoc";
	
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sset ALL local"
				" assoc:domain %s(%d) not found",
				domain_name, dname_length);  
	return;
    }
	
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name);

    /* save new local assoc */
    memcpy( domain_p->vlan_local_assoc, vlan_local_assoc, VLAN_BIT_STR_SIZ ); 

} /*vtp_core_set_all_vlan_local_assoc*/

/*-------------------------------------------------------------------------
 * vtp_core_set_vlan_pruning_eligible:
 *	The interface for dependent element to set pruning-eligibility
 *	This event will not go thru FSM, but may trigger Join msg
 *
 * Note: it can be called only by dependent: vtp_domain_change_notification()
 *------------------------------------------------------------------------*/
void
vtp_core_set_vlan_pruning_eligible (unsigned char dname_length, 
				    unsigned char *domain_name, 
				    void* trunk_id, 
				    unsigned char *vlan_prune_eligibility ) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr   trunk_p;
    unsigned char	*new, *old;
    unsigned char	*fmt = "%s set domain %s pruning eligibility";
    vtp_trunk_info      old_vals, new_vals;
	
    /* ----- find domain ----- */
    domain_p = vtp_pruning_search_domain(dname_length, domain_name);
    if ( !domain_p ) {
	VTP_PRUNING_LOG_WARNING(NODOMAIN, "%sset pruning"
				" eligible:domain %s(%d) not found",
				domain_name, dname_length);  
	return;
    }		

    trunk_p = vtp_find_trunk_link_in_domain(domain_p, trunk_id);
    if (!trunk_p) {
	VTP_PRUNING_LOG_WARNING(NOTRUNK, "%sset pruning eligible:"
				" trunk %x not found. (%s)", trunk_id,
				domain_name);
	return;
    }

    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name);
    retrieve_trunk_info(trunk_p, &old_vals);

    /* ----- mask off for default vlans ----- */
    old = trunk_p->vlan_pruning_eligibility;
    new = vlan_prune_eligibility;
	
    /* default vlans always NON-pruning-eligible */
    /* overwrite them, a '1' means non-pruning-eligible */
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_TR_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_TR_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL,VTP_VLAN_BASE);
	
    new[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL,VTP_VLAN_BASE)] &=
      ~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL,VTP_VLAN_BASE);
	
    memcpy( old, new, VLAN_BIT_STR_SIZ ); /* save new settings */
    retrieve_trunk_info(trunk_p, &new_vals);
    vtp_trunk_change_notification(VTP_MODIFY, &old_vals, &new_vals);
    
} /*vtp_core_set_vlan_pruning_eligible*/


/*=========================================================================
 * 
 *		----- Other interface to vtp.c/vtp_util.c -----
 *			change vtp modes
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_change_to_non_transparent:
 *	VTP mode changed from transparent to non-transparent
 *------------------------------------------------------------------------*/
void
vtp_pruning_change_to_non_transparent (vlan_domain_ptr domain_p) {
    unsigned char	*fmt = "%s chg domain %s to non-transparent %s";
      
    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name, no_domain_str);
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name, vtp_null_str);
    
    /* ----- init all existing vlan FSM states in the domain ----- */
    vtp_pruning_fsm_non_transparent(domain_p);
	
} /*vtp_pruning_change_to_non_transparent*/


/*-------------------------------------------------------------------------
 * vtp_pruning_change_to_transparent:
 *------------------------------------------------------------------------*/
void
vtp_pruning_change_to_transparent (vlan_domain_ptr domain_p) {
    unsigned char	*fmt = "%s chg domain %s to transparent %s";
      
    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name, no_domain_str);
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(INFO, fmt, domain_p->domain_name, vtp_null_str);
    
    /* ----- enable all existing vlan FSM states in the domain ----- */
    
    /* NOTE: should restore all databse by getting info from dependent
     *	     (one way to recover from error) 
     */
       
    vtp_pruning_fsm_transparent(domain_p);
	
} /*vtp_pruning_change_to_transparent*/


/*=========================================================================
 * 
 * PRUNING Event Dispatchers:	PART II- invoked by vtp routines internally
 *
 *	Generate pruning events and go for FSM handling.
 *	Events categorized as: 
 *
 *	affect trunk		affect all vlans	affect one vlan
 *	----------------------------------------------------------------
 *	*rx ver1 pkt		*rx Join msg		spt state change
 *	link state change	timeout 		pruning-eligibility
 *							local assoc change
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_receive_join_packet:
 *	Validate the rcv Join msg, then invoke FSM
 *------------------------------------------------------------------------*/
void
vtp_receive_join_packet (void *trunk_id, void *buff_p, unsigned long length, 
			 unsigned char *src_mac) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
    join_msg_ptr	join_p;
    unsigned long	len1, len2;
    unsigned short      first_vlan, last_vlan;

    /* NOTE: if no_domain_state, do not handle this Join */
    if ( no_domain_state ) {
	return;
    }
    
    /* ----- locate the receiving trunk first ----- */
    join_p = vtp_buffer_payload( trunk_id, buff_p);
    
    /* if no domain name in Join */
    if ( !join_p->dname_length ) {
		
	if (!flag_join_no_domain) {
	    VTP_PRUNING_LOG_NOTICE(JOINNONAME, "%sno domain name in rx"
				   " Join on trunk %x", trunk_id);
	    flag_join_no_domain++; /* do not display for a while */
	}
	return;
    }
	
    /* ----- domain name present in Join, find domain & trunk ----- */
    domain_p=vtp_pruning_search_domain(join_p->dname_length,
				       join_p->domain_name);
    
    /* no management domain found? */
    if ( !domain_p ) {
	
	if (!flag_join_domain_not_found) {
	    VTP_PRUNING_LOG_NOTICE(JOINDIFFDOMAIN, "%sdomain %s not"
				   " found in join rx on trunk %x",
				   join_p->domain_name, trunk_id);  
	    flag_join_domain_not_found++; /* do not display for a while */
	}
	return;
    }
    
    /* domain found, locate trunk */
    if ( !(trunk_p = vtp_pruning_search_trunk_link(domain_p, trunk_id)) ) {
	
	if (!flag_join_trunk_not_found) {
	    VTP_PRUNING_LOG_NOTICE(JOINNOTRUNK, "%strunk %x for rcv"
				   " Join not found", trunk_id);
	    flag_join_trunk_not_found++; /* do not display for a while */
	}
	return;
    }
    
    /* domain and trunk determined */
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s trunk %s rx Join, len=%d (domain"
			  " %s)", trunk_p->trunk_name, length,
			  domain_p->domain_name);  
    
    /* ----- count the pkt ----- */
    trunk_p->pruning_stat.rx_join++;
    
    /* ----- transparent mode: relay Join ----- */
    if ( is_pruning_mode_transparent(domain_p) ) {
	vtp_relay_packet(trunk_p, buff_p, length);
	return;
    }
    
    /* ----- disable: discard rcv Join ----- */
    if ( is_pruning_mode_disable(domain_p) ) {
	if (!flag_join_at_disable) {
	    VTP_PRUNING_LOG_NOTICE(JOINDISCARD, "%sdiscard Join rx on"
				   " trunk %x", trunk_id);
	    flag_join_at_disable++; /* do not display for a while */
	}
	return;
    }
	
    /* ----- Note: validate first/last vlan number ----- */
    first_vlan = vtp_format_2_bytes(join_p->first_vlan);
    last_vlan = vtp_format_2_bytes(join_p->last_vlan);
    if ( (first_vlan % 8) != FIRST_VLAN_MOD ) {
	VTP_PRUNING_LOG_ALERT(JOININVFSTV, "%sJoin rx on trunk %s-inv"
			      " first vlan: %d", trunk_p->trunk_name,
			      first_vlan);   
	return;
    }
    
    if ( (last_vlan % 8) != LAST_VLAN_MOD ) {
	VTP_PRUNING_LOG_ALERT(JOININVLSTV, "%sJoin rx on trunk %s-inv"
			      " last vlan: %d", trunk_p->trunk_name,
			      join_p->last_vlan);  
	return;
    }
    
    /* ----- Note: validate length ----- */

    /* actual length for join msg */
    len1 = length; 

    /* valid length for this Join */
    len2 = ((last_vlan - first_vlan +1) / 8) +sizeof(join_msg);

    if ( len1 < len2 ) {
	VTP_PRUNING_LOG_ALERT(JOININVLEN, "%sJoin rx on trunk %s-inv"
			      " len: %d (%d)", trunk_p->trunk_name,
			      len1, len2);  
	return;		
    }
    
    /* ----- process Join bits: entry to FSM ----- */
    vtp_pruning_fsm_join( trunk_p, join_p );

}   /*vtp_receive_join_packet*/


/*-------------------------------------------------------------------------
 * vtp_pruning_check_version:
 *  This function check a version 1 summary pkt to see if pruning supported.
 *  TLV is used to distinguish the pruning support capability.
 *
 *  Note that this function must be called after all basic checks have been
 *  done, see routines in "vtp.c/vtp_util.c".
 *	
 *  Note: if transparent mode, it won't come here 
 *  (see vtp.c: vtp_receive...)
 *------------------------------------------------------------------------*/
void
vtp_pruning_check_version (vtp_trunk_itf_ptr trunk_p, 
			   summary_adv_ptr pkt_p, unsigned long length) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    unsigned char	mode;	    /* pruning mode */
    unsigned char	*fmt = "%s trunk %s rx summary from"
	" non-pruning-support device %s"; 
    
    /* ----- check TLV ----- */
    if ( vtp_pruning_check_summary_tlv(domain_p, pkt_p, length, &mode) ) {
	/* summary pkt from pruning-capable device */
	set_vtp_tlv_pruning_ok(domain_p); /* for later use */
	set_vtp_tlv_pruning_mode(domain_p,mode); /* for later use */
	return;			    /* no further processing */
    }
    
    clr_vtp_tlv_pruning_ok(domain_p); /* for later use */
    
    /* ----- pkt from non-pruning-support-device event ----- */
    
    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name, no_domain_str);
	return;
    }
    
    /* ----- if transparent mode, do nothing ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name, no_pruning_str);
	return;
    }
    
    VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name, vtp_null_str);
    
    /* update counters */
    trunk_p->pruning_stat.rx_no_pruning_support_msg++;
    
    /* entry to FSM */
    vtp_pruning_fsm_ver1( trunk_p );
    
}   /*vtp_pruning_check_version*/

/*-------------------------------------------------------------------------
 * vtp_pruning_check_summary_tlv:
 *  It checks the TLV (indicating pruning support) appended at a summary pkt
 *  If shown, it checks the value field (pruning mode)
 *
 *  It returns TRUE if TLV ok, *mode contains pruning mode set in TLV
 *  It returns FALSE if no (valid) TLV
 *
 * Note: the first TLV, if shown, must be pruning tlv
 *------------------------------------------------------------------------*/
vtp_boolean    
vtp_pruning_check_summary_tlv (vlan_domain_ptr domain_p, 
			       summary_adv_ptr pkt_p, 
			       unsigned long length, 
			       unsigned char *mode) {
    unsigned long		len;
    summary_pruning_tlv_ptr	tlv_p;
    unsigned char		*chr;
    
    /* ----- validate length: must long enough to contain TLV ----- */
    len = length - sizeof(summary_adv);	/* TLV length */
    chr = ((unsigned char *)pkt_p) + sizeof(summary_adv); /* point to TLV */
    tlv_p = (summary_pruning_tlv_ptr)chr;
    
    if (len >= SUMMARY_PRUNING_TLV_SIZ) { /* must long enough */
	if ( tlv_p->tag == TAG_PRUNING_SUPPORT && 
	     tlv_p->length == TAG_PRUNING_SUPPORT_LEN ) {
	    
	    if ( is_valid_pruning_mode(tlv_p->value) ) {
		/* rx a summary pkt from pruning support device */
		*mode = tlv_p->value;
		return TRUE;
	    } else {
		VTP_PRUNING_LOG_ALERT(INVTLV, "%srx summary in domain"
				      " %s: inv TLV value(%x)",
				      domain_p->domain_name,
				      tlv_p->value);  
	    }
	}
    }
    
    return FALSE;		    /* no valid TLV found */
    
}   /*vtp_pruning_check_summary_tlv*/
    

/*=========================================================================
 * 
 *			timeout function
 *
 *========================================================================*/
/*-------------------------------------------------------------------------
 * vtp_pruning_timeout:
 *	Called when timeout occurs
 *	Two reasons: triggered Join period ends, or periodical sending Join
 *	
 * Note: T1,T2 is down-counting
 * Note: Two time granularities are possible: 1-sec and 6-sec
 * Note: For performance concern, we use 6-sec tick for periodical sending
 *	 Join, rather than 6 times of a 1-sec tick, therefore, the real T1 &
 *	 T2 may be greater than what defined in spec (960-sec, 19-sec
 *	 respecitvely), in worst case T1 up to 24-sec, T2 up to 965-sec.
 *	 However, this should be tolerable (joined->pruned can be delayed).
 * Note: if no domain state, the timer should not be started at all
 * Note: be sure routines called by it do NOT yield_cpu (it's timing
 *	 critical	
 *------------------------------------------------------------------------*/
void vtp_pruning_timeout (void *context) {
    vlan_domain_ptr	domain_p;
    vtp_trunk_itf_ptr	trunk_p;
	
    trunk_p = (vtp_trunk_itf_ptr)context;
    domain_p = trunk_p->domain_p;

    /* ----- if no domain state, should not happen ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(WARNING, "%strunk %s timeout(%s)",
			      trunk_p->trunk_name, no_domain_str); 
	return;
    }
	
    /* ----- if transparent mode, should not happen ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	
	/* Note: may b/c previous pending timeout */
	VTP_PRUNING_LOG_TRACE(WARNING, "%strunk %s timeout(%s)",
			      trunk_p->trunk_name, no_pruning_str); 
	return;
    }

    if (vtp_timer_active(trunk_p->pruning_timer_p)) {
	vtp_stop_timer(trunk_p->pruning_timer_p);
    }
    
    /* ----- trigger Join (1-sec) period ends ----- */
    if ( trunk_p->status & TRUNK_STS_JOIN_TRIG_PERIOD ) {
	vtp_pruning_trig_timeout(domain_p, trunk_p);
    } else {
	vtp_pruning_tick_timeout(domain_p, trunk_p);
    } 
	
} /*vtp_pruning_timeout*/


/*-------------------------------------------------------------------------
 * vtp_pruning_trig_timeout:
 *
 * Note: be sure routines called by it do NOT yield_cpu (it's timing
 *	 critical	
 *------------------------------------------------------------------------*/
void
vtp_pruning_trig_timeout (vlan_domain_ptr domain_p, 
			  vtp_trunk_itf_ptr trunk_p) {
    /* clear flag first */
    trunk_p->status &= ~TRUNK_STS_JOIN_TRIG_PERIOD;
			
    /* if any joining bit changing to 1 during this period, we must
     * start the trig period again */
    if ( vtp_pruning_is_joining_bit_chg_1(trunk_p) ) {
	vtp_pruning_start_trig_join(trunk_p); /* will start 1-sec tmr */
    } else {
	VTP_PRUNING_LOG_TRACE(DEBUG, "%s trunk %s stop trig Join",
			      trunk_p->trunk_name); 
	    
	/* still need one more Join sent */
	vtp_pruning_tx_join_msg(trunk_p);
	    
	/* start another 6-sec tick */
	trunk_p->join_jittered_value = vtp_pruning_jittered_value();
	vtp_start_timer(trunk_p->pruning_timer_p, 
			VTP_PRUNING_TICK_SEC*VTP_ONE_SECOND +
			trunk_p->join_jittered_value ); 
	
    }   	
} /*vtp_pruning_trig_timeout*/


/*-------------------------------------------------------------------------
 * vtp_pruning_tick_timeout:
 *  It is to handle "periodical pruning timeout"
 *  If many vlans exist, the handling may be time consuming
 *
 * Note: T1,T2 is down-counting
 * Note: For performance concern, we use 6-sec tick for periodical sending
 *	 Join, rather than 6 times of a 1-sec tick, therefore, the real T1 &
 *	 T2 may be greater than what defined in spec (960-sec, 19-sec
 *	 respecitvely), in worst case T1 up to 24-sec, T2 up to 965-sec.
 *	 However, this should be tolerable (joined->pruned can be delayed).
 * Note: if no domain state, the timer should not be started at all
 *------------------------------------------------------------------------*/
void
vtp_pruning_tick_timeout (vlan_domain_ptr domain_p, 
			  vtp_trunk_itf_ptr trunk_p) {
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s trunk %s timeout",
			  trunk_p->trunk_name); 

    /* ----- send a Join  ----- */
    vtp_pruning_tx_join_msg(trunk_p);
      
    /* ----- timeout event: entry to FSM ----- */
    /* Note: the timeout at this trunk can only cause the vlan state at this 
     *	     trunk goes from joined to pruned, and in turns may cause other 
     *	     trunk's joining bit changing to a 0, which no triggered Join
     *	     needed so that we can invoke FSM AFTER sending Join msg
     */
    vtp_pruning_fsm_tmo(trunk_p);
	
    /* ----- start another 6-sec tick ----- */
    /* Note: start timer after finishing all vlans' fsm */
    trunk_p->join_jittered_value = vtp_pruning_jittered_value();
    vtp_start_timer(trunk_p->pruning_timer_p, 
		    VTP_PRUNING_TICK_SEC*VTP_ONE_SECOND +
		    trunk_p->join_jittered_value ); 

}   /*vtp_pruning_tick_timeout*/



			 




/*=========================================================================
 * 
 *		----- Other interface to vtp.c/vtp_util.c -----
 *		  pruning operations for (add) domain/trunk/vlan...
 *		  pruning operations for (delete) domain/trunk/vlan...
 *
 *	Note: no pruning database protection done in these routines
 *	      (protect it before calling if necessary)
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_domain_init:
 *	Called by vtp_add_domain()
 *
 * scenario:
 *	vtp_add_domain => vtp_pruning_add_domain_init
 *			  (init pruning related fields in domain struc)
 *		       => vtp_domain_change_notification (dependent)
 *		          (call pruning functions to setup	
 *		           pruning-eligibility)
 *		       => vtp_pruning_add_domain
 *			  (process info gathered from above steps)
 *
 * Note: one exception: null domain overwritten by a real domain (still
 *			treated as 'add_domain')
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_domain_init (vlan_domain_ptr domain_p) {

    /* ----- if null domain will be overwritten by a real domain ----- */
    if ( is_no_domain_state(domain_p) && domain_p->dname_length ) {
	/* pruning database must have been init already, do nothing now */
	VTP_PRUNING_LOG_TRACE(DEBUG, "%s will quit no domain state soon");
    } else {
	VTP_PRUNING_LOG_TRACE(DEBUG, "%s start to add domain %s",
			      domain_p->domain_name); 
	
	/* ----- defaults: vlan local association: none ----- */
	memset( domain_p->vlan_local_assoc, 0, VLAN_BIT_STR_SIZ );
	
	/* ----- defaults: no vlan exists ----- */
	memset( domain_p->vlan_exist, 0, VLAN_BIT_STR_SIZ );
	
	/* ----- max/min vlan no. ----- */
	domain_p->min_vlan_no = domain_p->max_vlan_no = 0;
	
	domain_p->status = 0;
    }
	
    /* ----- set operational flag ----- */
    domain_p->status |= DOMAIN_STS_ADD_DOMAIN;
      
} /*vtp_pruning_add_domain_init*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_domain:
 *  Called by vtp_add_domain() after all pruning related status info has been
 *  got
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_domain (vlan_domain_ptr domain_p) {
    unsigned char	*fmt = "%s end of add domain %s %s";
      
    /* ----- remove operational flag ----- */
    domain_p->status &= ~DOMAIN_STS_ADD_DOMAIN;
    
    /*
     * Note: init FSM states & build joining bits ONLY when 
     *	     real domain state & non-transparent mode
     */
    
    /* ----- if no domain state ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, domain_p->domain_name,
			      no_domain_str); /* Note: init: add nul
					       * domain */  
	return;
    }
    
    /* ----- if transparent/disable mode, no rebuild join bit & FSM ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, domain_p->domain_name,
			      no_pruning_str); 
	return;
    }
	
    /* ----- pruning handling for this added domain ----- */
    vtp_pruning_fsm_non_transparent( domain_p );

    VTP_PRUNING_LOG_TRACE(DEBUG, fmt, domain_p->domain_name, vtp_null_str);
    
} /*vtp_pruning_add_domain*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_trunk_init:
 *  Called by vtp_add_trunk_link()
 *
 * scenario:
 *	vtp_add_trunk_link => vtp_pruning_add_trunk_init
 *			      (init pruning related fields in trunk struc)
 *			   => vtp_pruning_change_notification (dependent)
 *		              (call pruning functions to setup spt-state &
 *			       link-state)
 *		           => vtp_pruning_add_trunk
 *			      (process info gathered from above steps)
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_trunk_init (vlan_domain_ptr domain_p, 
			    vtp_trunk_itf_ptr trunk_p) {
    char* chr;

    /* ----- defaults: pruning-eligibility ----- */
    chr = trunk_p->vlan_pruning_eligibility;
	
    /* non-default vlans: pruning-eligible (1) */
    memset( chr, 0xff, VLAN_BIT_STR_SIZ );
    
    /* default vlans: non-pruning-eligible (0) */
    chr[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_ETHERNET_ISL,VTP_VLAN_BASE);
    
    chr[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_FDDI_ISL,VTP_VLAN_BASE);
    
    chr[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_TR_ISL,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_TR_ISL,VTP_VLAN_BASE);
    
    chr[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_FDDINET_ISL,VTP_VLAN_BASE);
    
    chr[vlan_bit_offset(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(VTP_FACTORY_DEFAULT_VLAN_TRNET_ISL,VTP_VLAN_BASE);
    
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s start to add trunk %s at domain"
			  " %s", trunk_p->trunk_name, domain_p->domain_name); 
    
    /* ----- defaults: SPT state: non-forwarding ----- */
    memset( trunk_p->spt_state, 0, VLAN_BIT_STR_SIZ );
	  
    /* ----- defaults: vlan FSM event: none	 ----- */
    memset( trunk_p->vlan_fsm_event, 0, VLAN_BYTE_STR_SIZ );
	  
    /* ----- defaults: status: none ----- */	  
    trunk_p->status = 0;
	
    /* ----- defaults: statistics: none ----- */	  
    memset(&trunk_p->pruning_stat, 0, sizeof(vtp_trunk_stat));
	
    /* ----- defaults: timer: stopped ----- */	  
    trunk_p->pruning_timer_p = vtp_create_timer(vtp_pruning_timeout,
						trunk_p); 
    trunk_p->join_jittered_value = 0;
	
    /* ----- defaults: link state: UP ----- */
    trunk_p->link_state = TRUNK_LINK_UP;
	
    /* ----- defaults: vlan_status array ----- */
    trunk_p->vlan_status_num = 0;
    trunk_p->vlan_status_win = DEF_VLAN_STATUS_SIZ;
		  
    /* ----- defaults: cur/prev joining bits: none ----- */
    memset( trunk_p->cur_join_bits, 0, VLAN_BIT_STR_SIZ );
	
    /* ----- defaults: vlan FSM state,T1,T2 ----- */
    memset( trunk_p->t2, 0, VLAN_BYTE_STR_SIZ );
    trunk_p->t1 = 0;		    /* 0 means expired */
    memset( trunk_p->vlan_fsm_state, 0, VLAN_BYTE_STR_SIZ );

    /* added to distinguish exact reason causing timeout */
    memset( trunk_p->rx_join_flag, 0, VLAN_BIT_STR_SIZ );
    
    /* ----- set operational flag ----- */
    domain_p->status |= DOMAIN_STS_ADD_TRUNK;
      
} /*vtp_pruning_add_trunk_init*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_trunk:
 *  Called by vtp_add_trunk_link()
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_trunk (vlan_domain_ptr domain_p, 
		       vtp_trunk_itf_ptr trunk_p) {
    unsigned char	*fmt = "%s end of add trunk %s %s";
    
    /* ----- remove operational flag ----- */
    domain_p->status &= ~DOMAIN_STS_ADD_TRUNK;
    
    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name, no_domain_str);
	return;
    }
    
    /* ----- if transparent mode, do nothing ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name, no_pruning_str);
	return;
    }
    
    /* ----- (I) state variable ----- */
    vtp_pruning_fsm_special_event = FSM_ADDI_EVT_ADDTRUNK;
    vtp_pruning_init_trunk_vlan_state(domain_p, trunk_p);
    vtp_pruning_process_state_change( domain_p, trunk_p ); /* for safety */
    
    /* ----- (II) build Joining bits for this trunk & send Join ----- */
    vtp_pruning_build_all_joining_bits( domain_p, trunk_p);
    vtp_pruning_flooding(domain_p, trunk_p);
    
    /* ----- (III) rebuild & chk triggered Join for other trunks ----- */
    vtp_pruning_build_all_other_joining_bits( domain_p, trunk_p, 0 );
    vtp_pruning_process_all_other_trig_joins( domain_p, trunk_p );
	  
    VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name, vtp_null_str);

} /*vtp_pruning_add_trunk*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_vlan_init:
 *  Called by vtp_add_vlan_in_database() & vtp_validate_database()
 *
 * Note: some of fields in trunk structure should have been init'ed or cleaned
 *	 up (refer to vtp_pruning_delete_vlan)
 *
 * scenario:
 *	vtp_add_vlan => vtp_pruning_add_vlan_init
 *			  (init pruning related fields in domain struc)
 *		     => vtp_vlan_change_notification (dependent)
 *		        (call pruning functions to setup spt-state &
 *		         local-assoc)
 *		     => vtp_pruning_add_vlan_refresh (update database)
 *		     => vtp_pruning_add_vlan 
 *			(process vlan state changes & joining bit changes)
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_vlan_init (vlan_domain_ptr domain_p, 
			   vlan_info_ent_ptr vlan_p) {
    unsigned short	isl_vlan_id = vlan_p->isl_vlan_id;
    
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s start to add vlan %d at domain"
			  " %s", vlan_p->isl_vlan_id,
			  domain_p->domain_name);  

    /* ----- domain ----- */
    /* defaults: vlan local association: none (vtp_pruning_delete_vlan) */
    
    /* adjust min/max_vlan_no */
    vtp_pruning_setup_vlan_range(domain_p);
    
    /* this vlan become existing */
    domain_p->vlan_exist[ vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE) ] |=
				vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);

    /* defaults: spt state: not-fwd (see vtp_pruning_delete_vlan) */
    /* defaults: vlan FSM event: none (see vtp_pruning_delete_vlan) */
    /* defaults: cur joining bit: 0 (see vtp_pruning_delete_vlan) */
    /* defaults: prev joining bit: 0 (see vtp_pruning_delete_vlan) */
    /* defaults: vlan FSM state, T2: later */
    
    /* ----- set operational flag ----- */
    domain_p->status |= DOMAIN_STS_ADD_VLAN;
      
}   /*vtp_pruning_add_vlan_init*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_vlan_refresh:
 *  Called by vtp_validate_database() to refresh pruning database for a vlan
 *
 * iinput	vlan_p = specified vlan
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_vlan_refresh (vlan_domain_ptr domain_p,
			      vlan_info_ent_ptr vlan_p) {
    unsigned short	isl_vlan_id = vlan_p->isl_vlan_id;
    unsigned char	*fmt = "%s refresh vlan %d (%s) %s";
      	    
    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	return;
    }
    
    /* ----- if transparent mode, do nothing ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	return;
    }
    
    /* ----- (I) init state variable ----- */
    vtp_pruning_fsm_special_event = FSM_ADDI_EVT_VLANREFRESH;
    vtp_pruning_init_domain_vlan_state(domain_p, isl_vlan_id);
    
    /* ----- (II) vlan joining bit: build Join bit for this vlan ----- */
    vtp_pruning_build_all_other_joining_bits( domain_p, NULL, isl_vlan_id );
	  
    VTP_PRUNING_LOG_TRACE(DEBUG, fmt, isl_vlan_id,
			  domain_p->domain_name, vtp_null_str); 
    
}   /*vtp_pruning_add_vlan_refresh*/

/*-------------------------------------------------------------------------
 * vtp_pruning_add_vlan:
 *  Called by vtp_add_vlan_in_database() & vtp_validate_database() to process
 *  fsm state & joining bit changes for the added vlan
 *
 * input	*domain
 *		*vlan: not used
 *------------------------------------------------------------------------*/
void
vtp_pruning_add_vlan (vlan_domain_ptr domain_p, 
		      vlan_info_ent_ptr vlan_p) {
    unsigned short	isl_vlan_id = (vlan_p)? vlan_p->isl_vlan_id:0;
    unsigned char	*fmt = "%s end of add vlan %d (%s) %s";
     	    
    /* ----- remove operational flag ----- */
    domain_p->status &= ~DOMAIN_STS_ADD_VLAN;
    
    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, isl_vlan_id,
			      domain_p->domain_name, no_domain_str); 
	return;
    }
    
    /* ----- if transparent mode, do nothing ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, isl_vlan_id,
			      domain_p->domain_name, no_pruning_str); 
	return;
    }
    
    /* ----- (I) process state change ----- */
    vtp_pruning_process_state_change( domain_p, NULL );	/* all trunks */
    
    /* ----- (II) process vlan joining bit change ----- */
    vtp_pruning_process_all_other_trig_joins( domain_p, NULL );	
	  
    if (isl_vlan_id) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, isl_vlan_id,
			      domain_p->domain_name, vtp_null_str); 
    } else {
	VTP_PRUNING_LOG_TRACE(DEBUG, "%s end of add multi vlans at"
			      " domain %s", domain_p->domain_name); 
    }
    
} /*vtp_pruning_add_vlan*/

/*-------------------------------------------------------------------------
 * vtp_pruning_delete_domain:
 *  Called by vtp_delete_domain()
 *------------------------------------------------------------------------*/
void
vtp_pruning_delete_domain (vlan_domain_ptr domain_p) {
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s delete domain %s", domain_p->domain_name);

    /* ----- free allocated memory, if any ----- */
    /* none */
    
}   /*vtp_pruning_delete_domain*/
	
/*-------------------------------------------------------------------------
 * vtp_pruning_delete_trunk:
 *	Called by vtp_delete_trunk_link()
 *
 * Note: it should be called after this trunk removed from list b/c it'll
 *	 rebuild all other trunks' joining bit
 *------------------------------------------------------------------------*/
void vtp_pruning_delete_trunk (vlan_domain_ptr domain_p, 
			       vtp_trunk_itf_ptr trunk_p) {
    unsigned char	*fmt = "%s end of del trunk %s %s";
    
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s delete trunk %s at domain %s",
			  trunk_p->trunk_name, domain_p->domain_name);

    /* ----- delete timer ----- */
    if (vtp_timer_active(trunk_p->pruning_timer_p)) {
	vtp_stop_timer(trunk_p->pruning_timer_p);
    }

    vtp_delete_timer(trunk_p->pruning_timer_p);

    /* ----- if no domain state, do nothing ----- */
    if ( is_no_domain_state(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name,
			      no_domain_str); 
	return;
    }
    
    /* ----- if transparent mode, do nothing ----- */
    if ( is_pruning_mode_stopped(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(DEBUG, fmt, trunk_p->trunk_name,
			      no_pruning_str); 
	return;
    }
	
    /* ----- rebuild all other trunk's joining bits ----- */
    vtp_pruning_build_all_other_joining_bits( domain_p, NULL, 0 );
    vtp_pruning_process_all_other_trig_joins( domain_p, NULL );
    
}   /*vtp_pruning_delete_trunk*/

/*-------------------------------------------------------------------------
 * vtp_pruning_delete_vlan:
 *	Called by vtp_delete_vlan_in_database()
 *	Clean up some fields in domain & trunk structure
 *------------------------------------------------------------------------*/
void
vtp_pruning_delete_vlan (vlan_domain_ptr domain_p, 
			 vlan_info_ent_ptr vlan_p) {
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short	isl_vlan_id = vlan_p->isl_vlan_id;
    unsigned short	offset;
    unsigned char	mask;
    
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s delete vlan %d at domain %s",
			  vlan_p->isl_vlan_id, domain_p->domain_name);

    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    
    /* ----- domain ----- */
    /* cleanup: vlan local association: none */
    domain_p->vlan_local_assoc[ offset ] &= ~mask;
    
    /* adjust min/max_vlan_no */
    vtp_pruning_setup_vlan_range(domain_p);
    
    /* this vlan become non-existing */
    domain_p->vlan_exist[ offset ] &= ~mask;

    /* ----- affected all trunks in the domain ----- */
    for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	
	/* cleanup: spt state: not-fwd */
	trunk_p->spt_state[ offset ] &= ~mask;
	
	/* cleanup: vlan FSM state/event, T2: none */
	trunk_p->vlan_fsm_state[ isl_vlan_id ] = 0;
	trunk_p->vlan_fsm_event[ isl_vlan_id ] = 0;
	trunk_p->t2[ isl_vlan_id ] = 0;
	
	/* cleanup: cur joining bit: 0 */
	/* cleanup: prev joining bit: 0 */
	trunk_p->cur_join_bits[ offset ] &= ~mask;
	
    } /*for(trunks)*/
    
}   /*vtp_pruning_delete_vlan*/

/*-------------------------------------------------------------------------
 * vtp_pruning_flush_database:
 *  Called by vtp_flush_database() after deleting all vlan structures
 *  from a domain. 
 *
 * Note: It just clean up the structures, w/o notifying pruning status to
 *	 dep, it may need to rewrite
 *------------------------------------------------------------------------*/
void vtp_pruning_flush_database (vlan_domain_ptr domain_p) {
    vtp_trunk_itf_ptr	trunk_p;
    
    VTP_PRUNING_LOG_TRACE(DEBUG, "%s flush domain %s database",
			  domain_p->domain_name); 

    /* ----- domain structure: vlan local association/vlan_exist: none ----- */
    memset( domain_p->vlan_local_assoc, 0, VLAN_BIT_STR_SIZ );
    memset( domain_p->vlan_exist, 0, VLAN_BIT_STR_SIZ );
    
    /* ----- domain structure: max/min vlan no. ----- */
    domain_p->min_vlan_no = domain_p->max_vlan_no = 0;

    /* ----- domain structure: status ----- */
    domain_p->status = 0;         /* Note: should not in any vtp operations */
    
    /* ----- trunk structure: ----- */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	
	/* cleanup: spt state: not-fwd */
	memset( trunk_p->spt_state, 0, VLAN_BIT_STR_SIZ );
	  
	/* cleanup: vlan FSM state/event, T2: none */
	memset( trunk_p->vlan_fsm_state, 0, VLAN_BYTE_STR_SIZ );
	memset( trunk_p->vlan_fsm_event, 0, VLAN_BYTE_STR_SIZ );
	memset( trunk_p->t2, 0, VLAN_BYTE_STR_SIZ );
	
	/* cleanup: cur/prev joining bit: 0 */
	memset( trunk_p->cur_join_bits, 0, VLAN_BIT_STR_SIZ );
	
	/* cleanup: trunk status */
	trunk_p->status = 0;
	
	/* vlan_status[] */
	trunk_p->vlan_status_num = 0;
	
	/* no need to stop pruning timer here (vtp.c should take care) */
	/* do not touch statistics, jitter value, link state, timer ptr */
	
    } /*for(trunks)*/
    
}   /*vtp_pruning_flush_database*/

/*=========================================================================
 * 
 *			----- Utility for Pruning Interface -----
 *				set memory block
 *				adjust vlan range	
 *				jittered value	
 *				check joining bit changes
 *				pruning database protection
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_setup_vlan_range:
 *  Setup min_vlan_no & max_vlan_no (both aligned) in domain structure
 *------------------------------------------------------------------------*/
void
vtp_pruning_setup_vlan_range (vlan_domain_ptr domain_p) {
    vlan_info_ent_ptr	vlan_first_p, vlan_last_p, vlan_info_p;
    
    if ( !domain_p->vlan_count ) {
	return;
    }
    
    /* get first & last vlans */
    vlan_first_p = domain_p->vlan_head;
    vlan_last_p = domain_p->vlan_head;
    for (vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p =
	 vlan_info_p->isl_next) {
	vlan_last_p = vlan_info_p;
    }
    /* set up min vlan no (integral of 8) */
    domain_p->min_vlan_no = 
	vlan_bit_offset(vlan_first_p->isl_vlan_id,VTP_VLAN_BASE) *8;
    
    /* set up max vlan no (one less than integral of 8) */
    domain_p->max_vlan_no = 
      vlan_bit_offset(vlan_last_p->isl_vlan_id,VTP_VLAN_BASE) *8 +7;
	    
}   /*vtp_pruning_setup_vlan_range*/

/*-------------------------------------------------------------------------
 * vtp_pruning_jittered_value:
 *	Get a random number between 0 and 100 ms
 *------------------------------------------------------------------------*/
unsigned int 
vtp_pruning_jittered_value (void) {
    unsigned long random_number;

    random_number = vtp_get_random_number();
    return (random_number % VTP_PRUNING_MAX_RANDOM_MS) ;
    
} /*vtp_pruning_jittered_value*/

/*-------------------------------------------------------------------------
 * vtp_pruning_is_joining_bit_chg_1:
 *  check if any joining bit changing to a 1 by comparing cur_join_bits &
 *  prev_join_bits 
 *  
 *  Note that we can not depend on the flag TRUNK_STS_JOIN_BIT_CHG_1 becasue
 *  it does not detect any joining bit 0->1->0, but it MUST be no change to
 *  1 if this flag not set. 
 *  Note also that it does not clean up that flag
 *------------------------------------------------------------------------*/
vtp_boolean
vtp_pruning_is_joining_bit_chg_1 (vtp_trunk_itf_ptr trunk_p) {
	
    if ( !(trunk_p->status & TRUNK_STS_JOIN_BIT_CHG_1) )
      return FALSE;		    /* if flag not set, no change to 1 */
	
    /* ----- determine if any joining bit REALLY changing to 1 ----- */
    /*
     * Note: the simplest algorithm is just return TRUE
     *		 OR
     *		 check all joining bits
     */
    return TRUE;		    /* be grace for joining */
	
} /*vtp_pruning_is_joining_bit_chg_1*/





/*=========================================================================
 * 
 *			----- Pruning Initialization -----
 *
 *		These init routines are written for original vtp core code
 *		to add pruning support
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_init:
 *	Called by vtp_init to add init for pruning stuff
 *------------------------------------------------------------------------*/
void
vtp_pruning_init () {
    flag_join_domain_not_found = 0;
    flag_join_trunk_not_found = 0;
    flag_join_at_transparent = 0;
    flag_join_at_disable = 0;
    flag_join_no_domain = 0;
    flag_join_no_buf = 0;
    
}   /*vtp_pruning_init*/






/**************************************************************************
 * 
 *
 *      ******* Pruning Finite State Machine (FSM) Implementation *******
 *      ******* See pruning spec for state diagram		  *******
 *
 *
 * From Now On ===>
 *
 * NOTE: the following routines will NOT check no domain state & vtp mode,
 *	 all necesary checking must be donw before calling	
 *
 * NOTE: the pruning database must be protected (exclusively accessed)
 *	 BEFORE calling the following routines
 *
 **************************************************************************/


/*=========================================================================
 * 
 *			----- PRUNING FSM Utility -----
 *
 *			Utilities for pruning FSM init
 *			 & for vlan_status[] full
 *			
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_init:
 *  Called before starting FSM on vlans to init FSM related structures
 *
 * input:	*domain ptr	
 *		*trunk ptr: NULL=all trunks
 *
 * Note: this routine must be called before any FSM operation going to be done
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_init (vlan_domain_ptr domain_p, 
		      vtp_trunk_itf_ptr trunk_ptr) {
	
} /*vtp_pruning_fsm_init*/

/*-------------------------------------------------------------------------
 * vtp_pruning_sts_array_full:
 *	If vlan_status (in trunk structure) rnu out of space, process current
 *	vlan state changes before proceeding
 *	In most caases, this is rarely happened
 *------------------------------------------------------------------------*/
void
vtp_pruning_sts_array_full (vtp_trunk_itf_ptr trunk_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
	
    VTP_PRUNING_LOG_TRACE(NOTICE,"%s trunk %s vlan_status overflow =>"
			  " flush!(%s)", trunk_p->trunk_name,
			  domain_p->domain_name);  
	
	
    /* ----- notify dependent code to handle current vlan changes ----- */
    /*
     * Note: the following call may cause task suspended in dep code
     */
    vtp_dep_vlan_state_change_notification(     
	domain_p->dname_length, domain_p->domain_name,	
	trunk_p->trunk_id, trunk_p->vlan_status_num, trunk_p->vlan_status);
	
    
    trunk_p->vlan_status_num =0;    /* for reuse */
	
} /*vtp_pruning_sts_array_full*/





/*=========================================================================
 * 
 *			----- PRUNING FSM Utility -----
 *
 *			Utilities for pruning FSM pre-processor
 *			for pruning-eligibility
 *			for change to transparent mode
 *			
 * Note: vtp_pruning_fsm_init() must be called before all utilities to init
 *	 ----------------------
 *	 "vlan_status" related fields in trunk structure, and after calling 
 *	 these utilities, an appropriate vtp_pruning_process_state_change()
 *	 to pass vlan state change array (accumulated in these utilities) to
 *	 dependent code (program pruning hardware)
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_enable_domain_vlan_state:
 *  Enable vlan FSM state(s) in a domain (all trunks) (becoming joined)
 *
 *  Typically it is called
 *  after the "pruning capability" is temporarily disabled on the vlan due to
 *  pruning-ineligible or vtp transparent mode
 *
 *  All specified existing vlans will be set to 'joined' state (enabled)
 *
 * input	*domain ptr
 *		isl_vlan_id= specified vlan, 0 = all vlans
 *
 * Note: vtp_pruning_fsm_init() must have been called
 *------------------------------------------------------------------------*/
void
vtp_pruning_enable_domain_vlan_state (vlan_domain_ptr domain_p,
				      unsigned short isl_vlan_id) {
    vtp_trunk_itf_ptr	trunk_p;
    vlan_info_ent_ptr	vlan_info_p;
	
    /* ----- affected all trunks in the domain ----- */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
		
	/* ----- vlan specified ----- */
	if ( isl_vlan_id ) {
		
	    /* enable this vlan state */
	    vtp_pruning_enable_vlan_state(trunk_p, isl_vlan_id);
		
	} else {
	    for (vlan_info_p = domain_p->vlan_head; vlan_info_p;
		 vlan_info_p = vlan_info_p->isl_next) {
		vtp_pruning_enable_vlan_state(trunk_p,
					      vlan_info_p->isl_vlan_id);
	    } 			
	} 

	vtp_process_reschedule();   /* care of too many trunks */
		
    } /*for(trunks)*/
	
} /*vtp_pruning_enable_domain_vlan_state*/

/*-------------------------------------------------------------------------
 * vtp_pruning_enable_vlan_state:
 *   Enable vlan FSM state(s) to always joined on a specified trunk 
 *
 * input	*trunk ptr = specified trunk
 *		isl_vlan_id= specified vlan
 *
 * Note: vtp_pruning_fsm_init() must have been called
 * Note: the specified trunk & vlan must exist
 *------------------------------------------------------------------------*/
void
vtp_pruning_enable_vlan_state (vtp_trunk_itf_ptr trunk_p, 
			       unsigned short isl_vlan_id) {
    /* NOTE: if DISABLE state, either link not up or SPT NONFWD
     *	     it's no sense to set it to joined 
     */
    
    /* set to joined if pruned originally */
    if (trunk_p->vlan_fsm_state[isl_vlan_id] == FSM_ST_NORM_PRUNE) {
		
	/* Note: may suspend inside this call */
	vtp_pruning_fsm_util_set_joined(trunk_p, isl_vlan_id);
    }
	
    /* change to joined state unconditionally */
    if (trunk_p->vlan_fsm_state[ isl_vlan_id ] != FSM_ST_NO_PRUNE) {
	VTP_PRUNING_LOG_INFO(FSMSTCHG, "%sT%s,V%d: st %s,event %s=>new"
			     " st %s", trunk_p->trunk_name,
			     isl_vlan_id,
			     vlan_state_str[trunk_p->vlan_fsm_state[isl_vlan_id]], 
			     vlan_event_str[vtp_pruning_fsm_special_event], 
			     vlan_state_str[FSM_ST_NO_PRUNE]);  
	trunk_p->vlan_fsm_state[ isl_vlan_id ] = FSM_ST_NO_PRUNE;
	trunk_p->vlan_fsm_event[ isl_vlan_id ] = vtp_pruning_fsm_special_event;
    }
	
} /*vtp_pruning_enable_vlan_state*/


/*-------------------------------------------------------------------------
 * vtp_pruning_init_domain_vlan_state:
 *  Init vlan FSM state(s) in a domain (all trunks), typically it is called
 *  after the "pruning capability" is enabled on the vlan due to
 *  pruning-eligible or vtp non-transparent mode
 *
 *  All specified existing vlans will be init to 'joined' state if trunk link
 *  up AND vlan spt state forwarding, or 'pruned' otherwise. In both cases,
 *  NO vlan state change notification sent to user 
 *
 * input	*domain ptr
 *		isl_vlan_id= specified vlan, 0 = all vlans
 *------------------------------------------------------------------------*/
void
vtp_pruning_init_domain_vlan_state (vlan_domain_ptr domain_p,
				    unsigned short isl_vlan_id) {
    vtp_trunk_itf_ptr	trunk_p;
	
    /* ----- affected all trunks in the domain ----- */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
		
	/* ----- vlan specified ----- */
	if ( isl_vlan_id ) {
		
	    /* init this vlan state */
	    vtp_pruning_init_vlan_state(domain_p, trunk_p, isl_vlan_id);

	    vtp_process_reschedule();   /* care of too many trunks */
	} else {
	    vtp_pruning_init_trunk_vlan_state(domain_p, trunk_p);
	} /*else(all-vlans)*/

	vtp_process_reschedule();   /* care of too many trunks */		
    } /*for(trunks)*/
	
} /*vtp_pruning_init_domain_vlan_state*/

/*-------------------------------------------------------------------------
 * vtp_pruning_init_trunk_vlan_state:
 *  Init all vlan FSM states in a trunk
 *
 *  All specified existing vlans will be init to 'joined' state if trunk link
 *  up AND vlan spt state forwarding, or 'pruned' otherwise. In both cases,
 *  NO vlan state change notification sent to user 
 *
 * Note: the specified domain & trunk must exist
 * Note: it will make T1 expired since all vlans on this trunk affected
 *		 (refer to FSM in design spec)
 *------------------------------------------------------------------------*/
void
vtp_pruning_init_trunk_vlan_state (vlan_domain_ptr domain_p, 
				   vtp_trunk_itf_ptr trunk_p) {
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	isl_vlan_id;
	
    for ( vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p
	 = vlan_info_p->isl_next) {
	isl_vlan_id = vlan_info_p->isl_vlan_id;
				
	/* make T1 expired */
	trunk_p->t1 = 0;
		
	/* init this vlan state */
	vtp_pruning_init_vlan_state(domain_p, trunk_p, isl_vlan_id);

	vtp_process_reschedule();   /* care of too many vlans */
	
    } /*for(vlans)*/
	
} /*vtp_pruning_init_trunk_vlan_state*/

/*-------------------------------------------------------------------------
 * vtp_pruning_init_vlan_state:
 *  Init vlan FSM state on a specified trunk, only S1, S2 possible
 *  (for non-pruning-eligible vlans, S2 always)
 *
 * input	*trunk ptr = specified trunk
 *		isl_vlan_id= specified vlan
 *
 * Note: the specified trunk & vlan must exist
 * Note: no need to set joined or pruned b/c we never touch it before
 *	 calling this routine
 *------------------------------------------------------------------------*/
void
vtp_pruning_init_vlan_state (vlan_domain_ptr domain_p, 
			     vtp_trunk_itf_ptr trunk_p, 
			     unsigned short isl_vlan_id) {
    unsigned short	offset;
    unsigned char	mask;
	
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
	
    /* ----- if non-pruning-eligible, always S2 ----- */
    if ( !(trunk_p->vlan_pruning_eligibility[offset] & mask) ) {
	
	if (trunk_p->vlan_fsm_state[isl_vlan_id] != FSM_ST_NO_PRUNE) {
	    VTP_PRUNING_LOG_INFO(FSMSTCHG, "%sT%s,V%d: st %s,event"
				 " %s=>new st %s",
				 trunk_p->trunk_name, isl_vlan_id, 
				 vlan_state_str[trunk_p->vlan_fsm_state[isl_vlan_id]], 
				 vlan_event_str[vtp_pruning_fsm_special_event],
				 vlan_state_str[FSM_ST_NO_PRUNE]); 
	    trunk_p->vlan_fsm_state[isl_vlan_id] = FSM_ST_NO_PRUNE; 
	    trunk_p->vlan_fsm_event[isl_vlan_id] =
		vtp_pruning_fsm_special_event; 
	}
    } else {
	if ( (trunk_p->link_state == TRUNK_LINK_UP) &&
	     (trunk_p->spt_state[ offset ] & mask) ) {
	
	    /* ---- Note: refer to vtp_pruning_fsm_act4_goto_noprune() ---- */
	    
	    /* stay in joined state */
	    if(trunk_p->vlan_fsm_state[isl_vlan_id] !=
	       FSM_ST_NO_PRUNE) {
		VTP_PRUNING_LOG_INFO(FSMSTCHG, "%sT%s,V%d: st %s,event"
				   " %s=>new st %s",
				     trunk_p->trunk_name, isl_vlan_id,
				     vlan_state_str[trunk_p->vlan_fsm_state[isl_vlan_id]], 
				     vlan_event_str[vtp_pruning_fsm_special_event], 
				     vlan_state_str[FSM_ST_NO_PRUNE]); 
		trunk_p->vlan_fsm_state[isl_vlan_id] = FSM_ST_NO_PRUNE;
		trunk_p->vlan_fsm_event[isl_vlan_id]=vtp_pruning_fsm_special_event;
	    }
	    
	    /* init T2 */
	    trunk_p->t2[ isl_vlan_id ] = 0; /* 0 means expired */
	} else {			    /* link not up or spt not fwd */
	    
	    /* Note: not to do action: 'set vlan to pruned' */
	    
	    if(trunk_p->vlan_fsm_state[isl_vlan_id] != FSM_ST_DIS) {
	      VTP_PRUNING_LOG_INFO(FSMSTCHG, "%sT%s,V%d: st %s,event"
				   " %s=>new st %s",
				   trunk_p->trunk_name, isl_vlan_id, 
				   vlan_state_str[trunk_p->vlan_fsm_state[isl_vlan_id]], 
				   vlan_event_str[vtp_pruning_fsm_special_event], 
				   vlan_state_str[FSM_ST_DIS]); 
		trunk_p->vlan_fsm_state[isl_vlan_id] = FSM_ST_DIS;
		trunk_p->vlan_fsm_event[isl_vlan_id]=vtp_pruning_fsm_special_event;
	    }
	    
	} /*else(link-not-up-or-spt-not-fwd)*/
    }   /*if-else(pruning-eligible)*/
	
} /*vtp_pruning_init_vlan_state*/


/*-------------------------------------------------------------------------
 * vtp_pruning_chg_eligible:
 *  Process "pruning-eligibility change" event
 *  This event is not part of the FSM actually, however, for a non-pruning-
 *  eligible vlan it still has a state variable, with state only in S1 or S2
 *	
 *  All trunks are affected
 *  Only existing vlans will be affected
 *  The classical FSM actions will not be called. 
 *
 * input:	*domain ptr
 *		isl_vlan_id
 *		flag: 0=become ineligible: always joined
 *		      non-0=become eligible:   init FSM state
 *
 * Note: the area to store the changed vlan status (for notifying dep code)
 *	 must have been init'ed correctly (in vtp_pruning_fsm_init())
 * Note: isl_vlan_id (vlan) must exists
 *------------------------------------------------------------------------*/
void
vtp_pruning_chg_eligible (vlan_domain_ptr domain_p, 
			  unsigned short isl_vlan_id,
			  unsigned char flag) {
    /* ----- a vlan pruning-eligibility change affects the domain ----- */
    if ( !flag ) {		    /* 1->0=non eligible */
	/* enable this vlan state for all trunks in the domain */
	vtp_pruning_fsm_special_event = FSM_ADDI_EVT_INELIGIBLE;
	vtp_pruning_enable_domain_vlan_state( domain_p, isl_vlan_id );
    } else {			    /* 0->1=eligible */
	/* init this vlan state for all trunks in the domain */
	vtp_pruning_fsm_special_event = FSM_ADDI_EVT_ELIGIBLE;
	vtp_pruning_init_domain_vlan_state( domain_p, isl_vlan_id );
    }
    
}   /*vtp_pruning_chg_eligible*/
	

/*=========================================================================
 * 
 *			----- PRUNING FSM Pre-Processor -----
 *
 *    Pre-process the following events: Join(x), SPT state change, Link state
 *    change, version 1 msg, and possibly some others, on per trunk basis.
 *    They'll perform FSM operations for each of the vlans individually.
 *
 *
 *    Note: if vtp transparent mode, these FSM routines should NOT be called
 *    Note: if no domain state, these FSM routines should NOT be called
 *    Note: any operations touching "vlan_fsm_state" in trunk structure should
 *	    go through FSM pre-processors, which deal with the join/prune
 *	    status change, to achieve the easy maintenance. (because status
 *	    needs to notify dependent code which may suspend itself)
 *    Note: These routines may be called during the midway of some operations
 *	    such as add_trunk, add_vlanm, or add_domain, in which no Join 
 *	    msg will be sent.
 *    Note: When calling any FSM pre-processor, the pruning database should
 *	    be exclusively accessed
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_join:
 *  Process joining bit in a rcv Join msg.
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_join (vtp_trunk_itf_ptr trunk_p, join_msg_ptr join_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	isl_vlan_id;
    unsigned char	event, *bits;
    unsigned short      first_vlan, last_vlan;

    first_vlan = vtp_format_2_bytes(join_p->first_vlan);
    last_vlan = vtp_format_2_bytes(join_p->last_vlan);

    /* ----- FSM: init related fields before start FSM for vlans ----- */
    vtp_pruning_fsm_init( domain_p, trunk_p );
		
    /* ----- bits = *joining-bit-string ----- */
    bits = ((unsigned char *)join_p) + sizeof(join_msg);
			
    /* ----- FSM: process for each vlan ----- */
    /* ----- locate the matching vlans, loop over all joining matching bits */
    for ( vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p
	 = vlan_info_p->isl_next) {
	isl_vlan_id = vlan_info_p->isl_vlan_id;
		
	if ( isl_vlan_id >= first_vlan &&
	     isl_vlan_id <= last_vlan ) { /* found one */
			
	    /* speed up: only meaningful for S2 or above */
	    if ( trunk_p->vlan_fsm_state[isl_vlan_id] <= FSM_S1 )
	      continue;
	       
	    /* determine event */
	    event = ( bits[ vlan_bit_offset(isl_vlan_id,first_vlan) ]
		     & vlan_bit_mask(isl_vlan_id,first_vlan) )?
		       FSM_EVT_RX_JOIN1 : FSM_EVT_RX_JOIN0 ;
					 
	    /* invoke FSM handling */
	    vtp_pruning_fsm( trunk_p, isl_vlan_id, event );
			
	} /*if(vlan_match)*/
	
	/*vtp_process_reschedule();*/
	
    } /*for(vlans)*/
	
    /* ----- FSM: post process ----- */
    vtp_pruning_fsm_post_proc( trunk_p );
	
} /*vtp_pruning_fsm_join*/


/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_tmo:
 *  Process periodical timeout
 *
 * input	*trunk_ptr
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_tmo (vtp_trunk_itf_ptr trunk_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	isl_vlan_id;
    unsigned char	event;
	
    /* ----- FSM: init	related fields before start FSM for vlans ----- */
    vtp_pruning_fsm_init( domain_p, trunk_p );
	
    /* ----- update T1 timer count first ----- */
    /* Note: because T1 is for per trunk, we can not decrement it when
     *	     each vlan going thru FSM */
    if ( trunk_p->t1 )
      --trunk_p->t1;
	
    /* ----- FSM: timeout event for all vlans ----- */
    for ( vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p
	 = vlan_info_p->isl_next) {
		
	isl_vlan_id = vlan_info_p->isl_vlan_id;
		
	/* SPEEDUP: invoke fsm only for states needs it */
	if ( trunk_p->vlan_fsm_state[isl_vlan_id] != FSM_ST_NORM_JOIN )
	  continue;
	
	/* check T2 timer count */
	if ( trunk_p->t2[ isl_vlan_id ] )
	  --trunk_p->t2[ isl_vlan_id ];
	    
	if ( trunk_p->t2[ isl_vlan_id ] || trunk_p->t1 ) {
	  event = FSM_EVT_TMR_NOEXP; /* not expired yet */
	} else {
	    /* distinguish exact reason of timeout (ever rx Join or not) */
	    if (trunk_p->rx_join_flag[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)]
		& vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE)) { 
		event = FSM_EVT_TMR_EXP_NOJ;
		
		/* for debug statistics: abnormal timeout: no Join ever rx */
		if (trunk_p->vlan_fsm_state[isl_vlan_id] ==
		    FSM_ST_NORM_JOIN) {
		    trunk_p->tmo_no_rx_join++;
		}
	    } else {
		event = FSM_EVT_TMR_EXP; /* 0: normal timeout */
		
		/* for debug statistics */
		if (trunk_p->vlan_fsm_state[isl_vlan_id] == FSM_ST_NORM_JOIN)
		    trunk_p->tmo_cnt++;
	    }
	}
	
	/* invoke FSM handling */
	vtp_pruning_fsm( trunk_p, isl_vlan_id, event );
				
	vtp_process_reschedule();
	
    } /*for(all_vlans)*/

    /* ----- FSM: post process ----- */
    vtp_pruning_fsm_post_proc( trunk_p );
	  
} /*vtp_pruning_fsm_tmo*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_spt:
 *  Process SPT state change for a vlan
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_spt (vtp_trunk_itf_ptr trunk_p, 
		     unsigned short isl_vlan_id, 
		     unsigned short spt_state) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
	
    /* ----- FSM: init related fields before start FSM for vlans ----- */
    vtp_pruning_fsm_init( domain_p, trunk_p );
	
    /* ----- FSM: process for this vlan ----- */
    if ( spt_state == VLAN_SPT_STATE_FWD ) {
		
	/* invoke FSM only when both spt & link ok */
	if ( trunk_p->link_state == TRUNK_LINK_UP ) {
	    vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_FWD );
	}
		
	/* Note: if link down => no invocation */
    } else {
	vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_NON_FWD );
    }
	
    /* ----- FSM: post process ----- */
    vtp_pruning_fsm_post_proc( trunk_p );
		
} /*vtp_pruning_fsm_spt*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_spt_all:
 *  Process SPT state change for all vlans
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_spt_all (vtp_trunk_itf_ptr trunk_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	isl_vlan_id, offset;
    unsigned char	mask;
	
	
    /* ----- FSM: init related fields before start FSM for vlans ----- */
    vtp_pruning_fsm_init( domain_p, trunk_p );
	
    /* ----- FSM: process for all vlans ----- */
    for ( vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p
	 = vlan_info_p->isl_next) {
	isl_vlan_id = vlan_info_p->isl_vlan_id;
	offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
	mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
	
	/* determine event */
	if ( (trunk_p->spt_state[offset] & mask) ) { /* forwarding state */
		
	    /* invoke FSM only when both spt & link ok */
	    if ( trunk_p->link_state == TRUNK_LINK_UP ) {
		vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_FWD );
	    }
		
	    /* Note: if link down => no invocation */
	} else {
	    vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_NON_FWD );
	}
	
	vtp_process_reschedule();
	
    } /*for(vlans)*/
	
    /* ----- FSM: post process ----- */
    vtp_pruning_fsm_post_proc( trunk_p );
		
} /*vtp_pruning_fsm_spt_all*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_linkst:
 *  Process link state change
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_linkst (vtp_trunk_itf_ptr trunk_p, 
			unsigned char link_state) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	isl_vlan_id;
	
    /* ----- FSM: init related fields before start FSM for vlans ----- */
    vtp_pruning_fsm_init( domain_p, trunk_p );
	
    /* save link state */
    trunk_p->link_state = link_state;
	
    /* ----- FSM: process for this trunk (all vlans) ----- */
    for ( vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p
	 = vlan_info_p->isl_next) {
	isl_vlan_id = vlan_info_p->isl_vlan_id;
		
	/* determine event: (1) link up when spt fwd or (2) link down */
	if ( link_state == TRUNK_LINK_UP ) {
			
	    /* invoke FSM only when both spt & link ok */
	    if ( trunk_p->spt_state[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)]
		& vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE) ) {
		vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_LNKUP );
	    }
				
	    /* Note: if spt not forwarding => no invocation */
	} else {
	    vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_LNKDN );
	}
	
	vtp_process_reschedule();
	
    } /*for(all_vlans)*/
	
    /* ----- FSM: post process ----- */
    vtp_pruning_fsm_post_proc( trunk_p );
		
} /*vtp_pruning_fsm_linkst*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_ver1:
 *  Process "rx vtp msg from non-pruning supporting device" event
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_ver1 (vtp_trunk_itf_ptr trunk_p) {	
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    vlan_info_ent_ptr	vlan_info_p;
    unsigned short	isl_vlan_id;
	
    /* ----- FSM: init related fields before start FSM for vlans ----- */
    vtp_pruning_fsm_init( domain_p, trunk_p );
	
    /* ----- FSM: process for this trunk ----- */
    for ( vlan_info_p = domain_p->vlan_head; vlan_info_p; vlan_info_p
	 = vlan_info_p->isl_next) {
	isl_vlan_id = vlan_info_p->isl_vlan_id;
		
	/* invoke FSM */
	vtp_pruning_fsm( trunk_p, isl_vlan_id, FSM_EVT_RX_VER1 );
		
	vtp_process_reschedule();
	
    } /*for(all_vlans)*/
	
    /* ----- FSM: post process ----- */
    vtp_pruning_fsm_post_proc( trunk_p );
	
} /*vtp_pruning_fsm_ver1*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_eligible:
 *  Process "pruning-eligibility change" event, update vlan state if necessary
 *	
 *  All trunks are affected by any changing bit
 *
 * Note: domain_p->vlan_pruning_eligibility[] should not be updated yet when
 *	 calling this routine
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_eligible (vlan_domain_ptr domain_p, 
			  vtp_trunk_itf_ptr trunk_p, 
			  unsigned char *eligible) {
    unsigned short	isl_vlan_id, i, j;
    unsigned char	mask, *new, *old, chg;
	
    /* ----- init FSM related fields for all trunks ----- */
    vtp_pruning_fsm_init( domain_p, NULL );
	
    /*
     * Note: two way to implement: 1-navigate bit string, 2-navigate sorted
     *	     vlan link list
     *	     (worst case should be major concern for pruning support)
     */

    /* ----- navigate vlan pruning-eligibility bit string */
    old = trunk_p->vlan_pruning_eligibility;
    new = eligible;
    
    /* below: i = offset, j = bit location, mask = bit mask */
    for( i = 0; i < VLAN_BIT_STR_SIZ;	i++, old++, new++ ) {
		
	/* compared in byte first */
	if ( (chg = (*old ^ *new)) ) { /* any bit changed in this byte*/
			
	    /* check each bit in this byte */
	    for( j = 0, mask = 0x80; j < 8; j++, mask >>= 1 ) {
		if ( chg & mask ) { /* if any bit changed */
		    isl_vlan_id = i*8+j+VTP_VLAN_BASE;
		    
		    /* ----- only existing vlan need processed  ----- */
		    if ( !(domain_p->vlan_exist[i] & mask) ) {
			continue;
		    }
		    
		    VTP_PRUNING_LOG_TRACE(DEBUG, "%s vlan %d pruning"
					  " eligibility change:"
					  " %x->%x", isl_vlan_id, *old
					  & mask, *new & mask); 
		    
		    /* Note: update this bit before init state for this vlan */
		    trunk_p->vlan_pruning_eligibility[i] ^= mask;
		    
		    vtp_pruning_chg_eligible(domain_p, isl_vlan_id,
					     *new & mask); 
		    
		} /*if(this-bit-chg)*/
	    } /*for(j-bit)*/
	    
	    vtp_process_reschedule();
	    
	} /*if(chg-at-byte)*/
    } /*for(i-byte)*/
	
    /* ----- check if vlan pruning status changed on any trunk ----- */
    vtp_pruning_process_state_change( domain_p, NULL ); /* check all trunks */
	
} /*vtp_pruning_fsm_eligible*/


/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_non_transparent:
 *  Process "change to non vtp transparent mode" event
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_non_transparent (vlan_domain_ptr domain_p) {
    /* ----- FSM: init related fields before change for vlan states ----- */
    /* not necessary, but for safety */
    vtp_pruning_fsm_init( domain_p, NULL ); /* all trunks */

    /* ----- init all vlan FSM states on all trunks in the domain ----- */
    vtp_pruning_fsm_special_event = FSM_ADDI_EVT_NONTRANSPARENT;
    vtp_pruning_init_domain_vlan_state( domain_p, 0 ); /* all vlans */

    /* ----- check if any vlan state change ----- */
    /* not necessary, but for safety */
    vtp_pruning_process_state_change( domain_p, NULL ); /* all trunks */
         
    /* ----- Rebuild all vlan joining bits for all trunks ----- */
    vtp_pruning_build_all_other_joining_bits( domain_p, NULL, 0 );

    /* ----- send Join on all trunks ----- */
    vtp_pruning_flooding( domain_p, NULL ); /* all trunks */

} /*vtp_pruning_fsm_non_transparent*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_transparent:
 *  Process "change to vtp transparent mode" event
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_transparent (vlan_domain_ptr domain_p) {
    vtp_trunk_itf_ptr	trunk_p;
	
    /* ----- FSM: init related fields before change for vlan states ----- */
    vtp_pruning_fsm_init( domain_p, NULL );
	
    /* ----- stop timers ----- */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	if (vtp_timer_active(trunk_p->pruning_timer_p)) {
	    vtp_stop_timer(trunk_p->pruning_timer_p);
	}
    }
    
    /* ----- enable all vlan FSM states on all trunks in the domain ----- */
    vtp_pruning_fsm_special_event = FSM_ADDI_EVT_TRANSPARENT;
    vtp_pruning_enable_domain_vlan_state( domain_p, 0 );
	
    /* ----- check if any vlan state change (pruned->joined) ----- */
    vtp_pruning_process_state_change( domain_p, NULL );
	  
} /*vtp_pruning_fsm_transparent*/

/*=========================================================================
 * 
 *			----- PRUNING FSM Processor -----
 *
 *		The entry point of vtp pruning FSM on a per-vlan basis.
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm:
 *  FSM entry point (per trunk per vlan per event basis)
 *  		     --------- -------- ---------
 *  Note that the programming of hw for vlan pruning status change will NOT
 *  be done here. (it is done after all vlans on this trunk have completed FSM)
 *
 * Input:	*trunk_p
 *		isl_vlan_id
 *		event number
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm (vtp_trunk_itf_ptr trunk_p, 
		 unsigned short isl_vlan_id, unsigned char event) {
    unsigned short	offset;
    unsigned char	act, state, new_state, mask;
	
    /* ----- Note: FSM only applied to pruning-eligible vlans ----- */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask   = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
    if ( !(trunk_p->vlan_pruning_eligibility[offset] & mask)) 
      return;
    
    /* current state  */
    state = trunk_p->vlan_fsm_state[ isl_vlan_id ];
	
    /* get action code according to event & current state */
    act = vtp_pruning_fsm_act_code_table[ event ] [ state ];
	    
    /* ----- invoke appropriate action routine ----- */
    (*vtp_pruning_fsm_act_table[act])(trunk_p, isl_vlan_id, event);
	
    /* ----- determine next state ----- */
    trunk_p->vlan_fsm_state[ isl_vlan_id ] = new_state = 
      vtp_pruning_fsm_next_state_table[ event ][ state ];
	
    if ( state != new_state ) {	    /* save event causing state
				     * transition */
	VTP_PRUNING_LOG_INFO(FSMSTCHG, "%sT%s,V%d: st %s,event %s=>new"
			     " st %s", trunk_p->trunk_name,
			     isl_vlan_id, vlan_state_str[state], 
			     vlan_event_str[event],
			     vlan_state_str[new_state]); 

	/* ----- save event ----- */
	/* o	nly record those causing state transition or any action */
	trunk_p->vlan_fsm_event[ isl_vlan_id ] = event;
    }
    
    /* ----- if any pruned/joined state change, update other trunks' ----- */
    /* ----- corresponding vlan joining bit */
    if (((state == FSM_ST_NO_PRUNE || state == FSM_ST_NORM_JOIN) &&
	 (new_state == FSM_ST_DIS || new_state == FSM_ST_NORM_PRUNE))
	||  
	((state == FSM_ST_DIS || state == FSM_ST_NORM_PRUNE) && 
	 (new_state == FSM_ST_NO_PRUNE || new_state ==
	  FSM_ST_NORM_JOIN))) {  
	    
	vtp_pruning_build_all_other_joining_bits(trunk_p->domain_p, trunk_p, 
						 isl_vlan_id ); 
	/* all other trunks, this vlan affected */
    }
    
} /*vtp_pruning_fsm*/





/*=========================================================================
 * 
 *			----- PRUNING FSM Utilities -----
 *			mainly for FSM action routines
 *
 *========================================================================*/

/* 
 * Note: set join/prune routines must also check the vlan_status array size 
 *		 and do adjustment if not long enough
 */
void
vtp_pruning_fsm_util_set_joined (vtp_trunk_itf_ptr trunk_p, 
				 unsigned short isl_vlan_id) {
    /* setup next entry in *vlan_status */
    trunk_p->vlan_status[ trunk_p->vlan_status_num ].isl_vlan_id =
	isl_vlan_id;
    trunk_p->vlan_status[ trunk_p->vlan_status_num ].isl_vlan_status =
	VLAN_STS_JOINED;
    if ( ++trunk_p->vlan_status_num >= trunk_p->vlan_status_win ) {
	vtp_pruning_sts_array_full(trunk_p);
    }
}

void
vtp_pruning_fsm_util_set_pruned (vtp_trunk_itf_ptr trunk_p,
				 unsigned short isl_vlan_id) {
    /* setup next entry in *vlan_status */
    trunk_p->vlan_status[ trunk_p->vlan_status_num ].isl_vlan_id =
	isl_vlan_id;
    trunk_p->vlan_status[ trunk_p->vlan_status_num ].isl_vlan_status =
	VLAN_STS_PRUNED;
    if ( ++trunk_p->vlan_status_num >= trunk_p->vlan_status_win ) {
	vtp_pruning_sts_array_full(trunk_p);
    }
}

/*=========================================================================
 * 
 *			----- PRUNING FSM Action Routines -----
 *			(see design spec & pruning spec for details)
 *
 *========================================================================*/

/* action 0: do nothing */
const void
vtp_pruning_fsm_act0_null (vtp_trunk_itf_ptr trunk_p, 
			   unsigned short isl_vlan_id,
			   unsigned char event) {
}

/* action 1: Reset T1 */
const void
vtp_pruning_fsm_act1_reset_t1 (vtp_trunk_itf_ptr trunk_p, 
			       unsigned short isl_vlan_id,
			       unsigned char event) {
    trunk_p->t1 = NO_PRUNE_SUPPORT_TICK;
}

/* action 2:  Reset T2 */
const void
vtp_pruning_fsm_act2_reset_t2 (vtp_trunk_itf_ptr trunk_p, 
			       unsigned short isl_vlan_id,
			       unsigned char event) {
    trunk_p->t2[ isl_vlan_id ] = PRUNE_SUPPORTED_TICK;

    /* added to distinguish exact reason causing timeout 
     * set to 1 by rx Join(1) 
     */
    trunk_p->rx_join_flag[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] |=
	vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE); 
}

/* action 3: Update T1,T2 if not expired */
/* Note: becasue T1 is for per trunk (all vlans), we can not update T1 for per
 *		 vlan going thru this FSM */
const void
vtp_pruning_fsm_act3_update_t1t2 (vtp_trunk_itf_ptr trunk_p, 
				  unsigned short isl_vlan_id,
				  unsigned char event) {
    /* ----- do nothing (timer count updated before calling) ----- */
}

/* action 4: Set vlan to joined when link up and spt forwarding */
const void
vtp_pruning_fsm_act4_goto_noprune (vtp_trunk_itf_ptr trunk_p, 
				   unsigned short isl_vlan_id,
				   unsigned char event) {	  
    /* stop T1 if 'link up' event */
    if ( event == FSM_EVT_LNKUP )
      trunk_p->t1 = 0;		    /* T1 only reset when link up */
	
    /* stop T2 */
    trunk_p->t2[ isl_vlan_id ] = 0;

    /* set this vlan to joined state */
    vtp_pruning_fsm_util_set_joined(trunk_p, isl_vlan_id);

}   /*vtp_pruning_fsm_act4_goto_noprune*/

/* action 5: Set vlan to pruned */
const void
vtp_pruning_fsm_act5_prune (vtp_trunk_itf_ptr trunk_p, 
			    unsigned short isl_vlan_id,
			    unsigned char event) {
    /* set this vlan to pruned state */
    vtp_pruning_fsm_util_set_pruned(trunk_p, isl_vlan_id);

    /* added to distinguish exact reason causing timeout */
    trunk_p->rx_join_flag[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
}

/* action 6: Set vlan to joined, reset T1 */
const void
vtp_pruning_fsm_act6_set_join_on_ver1 (vtp_trunk_itf_ptr trunk_p, 
				       unsigned short isl_vlan_id,
				       unsigned char event) {
    /* set this vlan to joined state */
    vtp_pruning_fsm_util_set_joined(trunk_p, isl_vlan_id);
	
    /* reset T1 */
    trunk_p->t1 = NO_PRUNE_SUPPORT_TICK;
}

/* action 7: Set vlan to joined, reset T2 */
const void
vtp_pruning_fsm_act7_set_join_on_join1 (vtp_trunk_itf_ptr trunk_p, 
					unsigned short isl_vlan_id,
					unsigned char event) {
    /* set this vlan to joined state */
    vtp_pruning_fsm_util_set_joined(trunk_p, isl_vlan_id);
	
    /* reset T2 */
    trunk_p->t2[ isl_vlan_id ] = PRUNE_SUPPORTED_TICK;

    /* added to distinguish exact reason causing timeout 
     * set to 1 by rx Join(1) 
     */
    trunk_p->rx_join_flag[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] |=
	vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE); 
}

/* action 8: do nothing */
const void
vtp_pruning_fsm_act8_set_join_on_join0 (vtp_trunk_itf_ptr trunk_p, 
					unsigned short isl_vlan_id,
					unsigned char event) {
    /* added to distinguish exact reason causing timeout 
     * set to 0 by rx Join(0) 
     */
    trunk_p->rx_join_flag[vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE)] &=
	~vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE); 
}


/*=========================================================================
 * 
 *			------ PRUNING FSM Post Processor ------
 *
 *	In considering of the conditions after FSM performed on vlan(s):
 *	(1) more than one vlan pruning status may be changed by FSM operations
 *	(2) more than one vlan joining bits for other trunks may be changed
 *	    to 1 due to FSM operations on vlans of this trunk
 *	
 *	Therefore we need a post-processor to handle all the changes caused by
 *	FSM. (To handle "in batch" is for performance concern)
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_fsm_post_proc:
 *  Post-processing for vtp pruning FSM on per domain basis.
 *
 *  After applying FSM to individual vlans, the following conditions may exist:
 *    -vlans w/ pruning status changed => notify dep code to program hw
 *    -any other trunk's joining bit(s) changed to 1 => send Join on that trunk
 *
 * Note 1:
 *  The running task may sleep when calling dependent function to
 *  change hardware join/prune settings or sending Join msg. (the pruning
 *  database must be exclusively accessed for this reason)
 * Nore 2:
 *  If during the midway of some operations such as add_trunk, add_domain,
 *  or add_vlan, don't perform any functions here (performed after the 
 *  operation completes)
 *------------------------------------------------------------------------*/
void
vtp_pruning_fsm_post_proc (vtp_trunk_itf_ptr trunk_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;

    /* Note: for system performance, it's better to send trigger Join
     *	     before changing vlan state at dependent portion */

    /* ----- check if a Join msg should be triggered on other trunk(s) ----- */
    vtp_pruning_process_all_other_trig_joins(domain_p, trunk_p);
    
    /* ----- check if vlan state changed on this trunk ----- */
    /* and notify dependent code */
    vtp_pruning_process_state_change(domain_p, trunk_p);
    
}	/*vtp_pruning_fsm_post_proc*/

/*=========================================================================
 * 
 *		------ Utility for vlan state change notification  ------
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_process_state_change:
 *  Check if any vlan state change on trunks
 *  If yes, notify dependent element
 *
 * input	*domain_p
 *		*trunk_p - chk for this trunk
 *			   (NULL means all trunks will be checked)
 *
 * Note: this routine will not proceed if vtp operations "add domain/vlan/
 *	 trunk" has not been completed
 *------------------------------------------------------------------------*/
void
vtp_pruning_process_state_change (vlan_domain_ptr domain_p, 
				  vtp_trunk_itf_ptr trunk_ptr) {
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- if during any vtp operations, stop ----- */
    if ( vtp_pruning_not_complete(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, "%s domain %s- no st chg handle"
			      " during vtp op", domain_p->domain_name); 
	return;
    }
    
    /* if specified */
    if ( trunk_ptr ) {
	trunk_p = trunk_ptr;
	if ( trunk_p->vlan_status_num ) { /* some vlan change its state */
	    
	    /*
	     * Note: the following call may cause task suspended in dep code
	     */
	    vtp_dep_vlan_state_change_notification(     
			domain_p->dname_length, domain_p->domain_name,	
			trunk_p->trunk_id, trunk_p->vlan_status_num, 
			trunk_p->vlan_status);
	    
	    trunk_p->vlan_status_num = 0;
	}
	return;
    }
    
    /* if not specified */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	if ( trunk_p->vlan_status_num ) { /* some vlan change its state */

	    /*
	     * Note: the following call may cause task suspended in dep code
	     */
	    vtp_dep_vlan_state_change_notification(
			domain_p->dname_length, domain_p->domain_name,
			trunk_p->trunk_id, trunk_p->vlan_status_num, 
			trunk_p->vlan_status);
	    
	    trunk_p->vlan_status_num = 0;
	    
	}   /*if(num)*/

	vtp_process_reschedule();

    } /*for(trunks)*/
    
}   /*vtp_pruning_process_state_change*/


/*=========================================================================
 * 
 *		------ Utility for triggered Join ------
 *
 *		Utilities called AFTER building joining bits
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_process_all_other_trig_joins:
 *  Check if a triggered Join should be sent on all trunks, except the 
 *  specified trunk, in a domain
 *
 * input	*domain_p
 *		*trunk_p - do not chk triggered Join condition for this trunk
 *			   (NULL means all trunks will be checked)
 *------------------------------------------------------------------------*/
void
vtp_pruning_process_all_other_trig_joins (vlan_domain_ptr domain_p, 
					  vtp_trunk_itf_ptr trunk_ptr) {
    vtp_trunk_itf_ptr	trunk_p;
    
    /* ----- check if a Join msg should be triggered on other trunk(s) ----- */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	if ( trunk_p == trunk_ptr ) /* trunk excluded */
	  continue;
	
	vtp_pruning_process_trig_join(trunk_p);

	vtp_process_reschedule();
	
    } /*for(trunks)*/
    
} /*vtp_pruning_process_all_other_trig_joins*/

/*-------------------------------------------------------------------------
 * vtp_pruning_process_trig_join:
 *  Check if a triggered Join should be sent on a specified trunk.
 *  The entry point to check & start a triggered Join period (1-sec) on a trunk
 *
 *  The entry point to process triggered Join conditions.
 *
 * Note: this routine will not proceed if vtp operations "add domain/vlan/
 *	 trunk" has not been completed
 *------------------------------------------------------------------------*/
void
vtp_pruning_process_trig_join (vtp_trunk_itf_ptr trunk_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    
    /* ----- if during any vtp operations, quit ----- */
    if ( vtp_pruning_not_complete(domain_p) ) {
	VTP_PRUNING_LOG_TRACE(INFO, "%s trunk %s: no trig join sent"
			      " during vtp op", trunk_p->trunk_name); 
	return;
    }
    
    /* ----- if any vlan joining bit changing to 1 ----- */
    if ( vtp_pruning_is_joining_bit_chg_1(trunk_p) ) {
		
	/* if the previous 1-sec not timeout yet, don't send Join */
	if ( trunk_p->status & TRUNK_STS_JOIN_TRIG_PERIOD ) {
	    VTP_PRUNING_LOG_TRACE(INFO, "%s trunk %s- another trig"
				  " join in 1-sec", trunk_p->trunk_name); 
	    return;
	}

	/* start new triggered join period (1-sec) */
	vtp_pruning_start_trig_join(trunk_p);
		  
    } /*if(join_bit_chg_1)*/
    
} /*vtp_pruning_process_trig_join*/



/*-------------------------------------------------------------------------
 * vtp_pruning_start_trig_join:
 *  Start a triggered Join period, it sends a Join & starts 1-sec timer
 *  It sets the "triggered Join" flag, clears the "joining bit chg to 1" flag
 *------------------------------------------------------------------------*/
void
vtp_pruning_start_trig_join (vtp_trunk_itf_ptr trunk_p) {
    VTP_PRUNING_LOG_TRACE(INFO, "%s trunk %s- send trig Join",
			  trunk_p->trunk_name); 
    
    /* ----- stop pruning timer if it is running ----- */
    if (vtp_timer_active(trunk_p->pruning_timer_p)) {
	vtp_stop_timer(trunk_p->pruning_timer_p);
    }
    
    /* ----- tx a Join ----- */
    vtp_pruning_tx_join_msg(trunk_p);
    trunk_p->status |= TRUNK_STS_JOIN_TRIG_PERIOD;
    
    /* ----- start 1-sec timer ----- */
    vtp_start_timer(trunk_p->pruning_timer_p,
		    VTP_PRUNING_TRIG_TICK_SEC*VTP_ONE_SECOND); 
    
}   /*vtp_pruning_start_trig_join*/

/*=========================================================================
 * 
 *			------ Utility to send Join ------
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_flooding:
 *  Send Join on all trunks, or specified trunk, in a domain
 *
 * input	*domain_p
 *		*trunk_p - send Join for this trunk
 *			   (NULL means all trunks)
 *------------------------------------------------------------------------*/	
void vtp_pruning_flooding (vlan_domain_ptr domain_p, 
			   vtp_trunk_itf_ptr trunk_ptr) {
    vtp_trunk_itf_ptr trunk_p;
    
    /* ----- do not flood if no vlan ----- */
    if ( !domain_p->vlan_count ) {
	return;
    }
      
    /* ----- flooding Join on desired trunk ----- */
    if ( trunk_ptr ) {
	VTP_PRUNING_LOG_TRACE(INFO,"%s flood join on trunk %s",
			      trunk_ptr->trunk_name); 

	/* tx a Join & start 6-sec timer */
	vtp_pruning_tx_join_msg(trunk_ptr);
	trunk_ptr->join_jittered_value = vtp_pruning_jittered_value();
	vtp_start_timer(trunk_ptr->pruning_timer_p, 
		VTP_PRUNING_TICK_SEC*VTP_ONE_SECOND +
			trunk_ptr->join_jittered_value ); 
    } else {
	for (trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	     trunk_p->trunk_next) {

	    VTP_PRUNING_LOG_TRACE(INFO,"%s flood join on trunk %s",
				  trunk_p->trunk_name); 

	    /* tx a Join & start 6-sec timer */
	    vtp_pruning_tx_join_msg(trunk_p);
	    trunk_p->join_jittered_value = vtp_pruning_jittered_value();
	    vtp_start_timer(trunk_p->pruning_timer_p, 
		    VTP_PRUNING_TICK_SEC*VTP_ONE_SECOND +
			    trunk_p->join_jittered_value ); 
	    vtp_process_reschedule();
	    
	}   /*for(trunks)*/
    }
    
}   /*vtp_pruning_flooding*/

/*-------------------------------------------------------------------------
 * vtp_pruning_tx_join_msg:
 *  Packetize a Join msg and tx
 *
 *  Note that this routine does not check the vtp mode & timer , they should
 *   be checked by the caller
 *  It does check the link state???
 *------------------------------------------------------------------------*/
void
vtp_pruning_tx_join_msg (vtp_trunk_itf_ptr trunk_p) {
    vlan_domain_ptr	domain_p = trunk_p->domain_p;
    void		*buff_p;
    unsigned int	len;
    
    /* ----- generate Join ----- */
    /* NOTE: if no buffer to tx a triggered Join, the trig_join_bit in 
     * trunk_p->status always on => cause every 1 second to try to send a 
     * trig join hereafter 
     */
    buff_p = vtp_pruning_generate_join(domain_p, trunk_p, &len);
    if ( buff_p == NULL )
      return;
    
    /* ----- transmit Join ----- */
    
    vtp_transmit_packet( trunk_p->trunk_id, buff_p, sizeof(join_msg)+len );
    
    /* ----- statistics ----- */
    trunk_p->pruning_stat.tx_join++;
    
    /* ----- clear flag for joining bit changing to 1 ----- */
    trunk_p->status &= ~TRUNK_STS_JOIN_BIT_CHG_1;
    
}   /*vtp_pruning_tx_join_msg*/

/*-------------------------------------------------------------------------
 * vtp_pruning_generate_join:
 *------------------------------------------------------------------------*/
void* vtp_pruning_generate_join (vlan_domain_ptr domain_p,
				 vtp_trunk_itf_ptr trunk_p, 
				 unsigned int *len) {
    void		*buff_p;
    unsigned char	*bits;
    join_msg*           join_p;

    /* ----- no vlan, no join msg ----- */
    if ( !domain_p->vlan_count ) {
        return NULL;
    }
    
    /* ----- get buffer space ----- */
    buff_p = vtp_get_buffer( MAX_JOIN_MSG_LEN);
    if ( !buff_p ) {
	if (!flag_join_no_buf) {    /* won't continuous log */
	    VTP_PRUNING_LOG_WARNING(NOBUF, "%sno buf to build Join for"
				    " trunk %s",trunk_p->trunk_name); 
	    flag_join_no_buf++;
	}
	no_buf_cnt++;
	return NULL;
    } else {
	flag_join_no_buf = 0;
    }
    
    /* ----- build a join msg ----- */
    join_p = vtp_buffer_payload(trunk_p->trunk_id, buff_p);
    join_p->version = VTP_VERSION;
    join_p->code = VTP_JOIN_MSG;
    join_p->reserved = 0;
    join_p->dname_length = domain_p->dname_length;

    memset(join_p->domain_name, 0, VTP_MAX_DOMAIN_NAME_LENGTH);
    memcpy(join_p->domain_name, domain_p->domain_name,
	   domain_p->dname_length); 
    
    join_p->first_vlan = vtp_format_2_bytes(domain_p->min_vlan_no);
    join_p->last_vlan  = vtp_format_2_bytes(domain_p->max_vlan_no);
    
    /* ----- points to vlan joining string in Join msg ----- */
    bits = ((unsigned char *)join_p) + sizeof(join_msg);
    *len = (domain_p->max_vlan_no - domain_p->min_vlan_no + 1) /8;
    memcpy( bits, &(trunk_p->cur_join_bits[domain_p->min_vlan_no/8]),
	   *len );

    return buff_p;
    
}   /* vtp_pruning_generate_join*/   

/*=========================================================================
 * 
 *		----- Utility to deal with joining bits -----
 *
 *========================================================================*/

/*-------------------------------------------------------------------------
 * vtp_pruning_build_all_other_joining_bits:
 *  update the vlan joining bit(s) for all trunks, except the specified
 *  trunk, in a domain
 *
 * input	*domain_p
 *		*trunk_p - do not update the joining bit for this trunk
 *			   (NULL means all trunks will be updated)
 *		isl_vlan_id - the vlan joining bit to be updated (0 means ALL)
 *------------------------------------------------------------------------*/
void
vtp_pruning_build_all_other_joining_bits (vlan_domain_ptr domain_p,
					  vtp_trunk_itf_ptr trunk_ptr,
					  unsigned short isl_vlan_id) {
    vtp_trunk_itf_ptr	trunk_p;
    
    /* go thru all trunks in this domain */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	if ( trunk_p == trunk_ptr ) /* trunk excluded */
	    continue;
	
	/* ----- update vlan joining bit(s) on this trunk ----- */
	if ( !isl_vlan_id ) {	    /* all join bits need updated */
			
	    /* ----- loop over all vlans ----- */
	    vtp_pruning_build_all_joining_bits(domain_p, trunk_p);
	    
	} else {	    /* only this vlan join bit updated */
	    vtp_pruning_build_joining_bit(domain_p, trunk_p, isl_vlan_id);
	}
	
    } /*for(all_trunks)*/
    
}   /*vtp_pruning_build_all_other_joining_bits*/

/*-------------------------------------------------------------------------
 * vtp_pruning_build_all_joining_bits:
 *  This routine builds all joining bits for a specified trunk.
 *  Instead of transversing all vlan_info_ent, it uses "byte" (8 vlans a time)
 *  arithmatic operations on pruning database items to get quick result,
 *  it assumes that the bits/bytes in pruning database
 *  for non-existing vlans are 0
 *
 *  It also sets the "joining bit changing to 1" flag (if happening)
 *------------------------------------------------------------------------*/
void
vtp_pruning_build_all_joining_bits (vlan_domain_ptr domain_p, 
				    vtp_trunk_itf_ptr trunk_p) {  
    unsigned short	i, len, offset;
    unsigned char	old_join_bits, new_join_bits;

    len = (domain_p->max_vlan_no -domain_p->min_vlan_no +1)/8;
    /* how many bytes(8 vlans) to loop */
    offset = vlan_bit_offset(domain_p->min_vlan_no,VTP_VLAN_BASE);
    /* offset for first vlan in bit string */
	
    /* one loop for 8 vlans, see "vtp_pruning_build_joining_bit() for 
     * joining bit formula */
    for( i = 0; i < len; i++, offset++ ) { /* total 'len' bytes */
				
	old_join_bits = trunk_p->cur_join_bits[ offset ];
		
	/* build joining bits for up to 8 vlans a time, use arithmatic op */
	new_join_bits = trunk_p->cur_join_bits[ offset ] =
	  trunk_p->spt_state[offset] &				/* --(I) */
	    ( 
	     (~trunk_p->vlan_pruning_eligibility[offset]) |
	     domain_p->vlan_local_assoc[offset] |		/* --(II) */
	     vtp_pruning_are_other_joined(domain_p, trunk_p, offset) 
								/* --(III) */
	    );
		
	/* check if any bit changing to 1 */
	if ( ~old_join_bits & new_join_bits ) {
	    trunk_p->status |= TRUNK_STS_JOIN_BIT_CHG_1;
	}

    } /*for(len-bytes)*/
			
} /*vtp_pruning_build_all_joining_bits*/		

/*-------------------------------------------------------------------------
 * vtp_pruning_are_other_joined:
 *  Determine if specific vlans are joined at any other trunk
 *  It returns 8 joining bits, a '1' in a bit means joined, '0' pruned. 
 *  (the first vlan for bit/byte string = 1)
 *	
 * input:	*domain ptr
 *		*trunk ptr
 *		offset = byte offset in a bit string (representing vlans
 *			 (offset*8+1) to (offset*8+8) )
 *------------------------------------------------------------------------*/
unsigned char
vtp_pruning_are_other_joined (vlan_domain_ptr domain_p,
			      vtp_trunk_itf_ptr trunk_ptr,
			      unsigned short offset) {
    vtp_trunk_itf_ptr	trunk_p;
    unsigned short	i, index; /* for vlan_fsm_state[] byte array */
    /* the index in byte array */
    unsigned char	mask, join, all = 0;
	
    /* check all other trunks in this domain */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	if ( trunk_p == trunk_ptr ) /* exclude itself */
	  continue;
		
	/* ----- convert 8 vlan fsm states (byte) to a bit string ----- */
	for( i = 0, mask = 0x80, join = 0, index = offset*8; 
	    i < 8; i++, index++, mask >>= 1 ) {
	    if ( trunk_p->vlan_fsm_state[index] == FSM_ST_NO_PRUNE ||
		 trunk_p->vlan_fsm_state[index] == FSM_ST_NORM_JOIN )
	      join |= mask;	    /* fsm state  for these 8 vlans */
	}
		
	/* ----- determine & return joining bits ----- */
	all |= ( join & trunk_p->spt_state[offset] );
	
	vtp_process_reschedule();

    } /*for(trunks)*/
	
    return all;

}   /*vtp_pruning_are_other_joined*/


/*-------------------------------------------------------------------------
 * vtp_pruning_build_joining_bit:
 *  Setup the specific vlan joining bit on a specific trunk
 *  It also sets the "joining bit changing to 1" flag (if happening)
 *------------------------------------------------------------------------*/
void
vtp_pruning_build_joining_bit (vlan_domain_ptr domain_p,
			       vtp_trunk_itf_ptr trunk_p,
			       unsigned short isl_vlan_id) {
    unsigned short	offset;
    unsigned char	mask, old_join_bit;
	  
    /* determine offset & mask in a bit string for isl_vlan_id */
    offset = vlan_bit_offset(isl_vlan_id,VTP_VLAN_BASE);
    mask = vlan_bit_mask(isl_vlan_id,VTP_VLAN_BASE);
	
    /*
     * Formula for a vlan joining bit:
     *		spt_forwarding &&				--(I)
     *		(
     *		  pruning-ineligible || has-local-assoc ||	--(II)
     *		  ( joined-state-at-any-other-trunk && 
     *		    spt-forwarding-at-that-trunk )		--(III)
     *		)
     */
	
    old_join_bit = trunk_p->cur_join_bits[offset] & mask;
    if ( (trunk_p->spt_state[offset] & mask) &&		/* --(I) */
	( 
	 (!(trunk_p->vlan_pruning_eligibility[offset] & mask)) ||
	 (domain_p->vlan_local_assoc[offset] & mask) ||	/* --(II) */
	 vtp_pruning_is_other_joined(domain_p, trunk_p, isl_vlan_id,
				     offset, mask)	/* --(III) */
	)
       ) {
		
	trunk_p->cur_join_bits[offset] |= mask; /* new joining bit = 1 */
		
	/* set status indicator: used later by other routines */
	if ( !old_join_bit ) {
	    
	    VTP_PRUNING_LOG_TRACE(INFO, "%s trunk %s vlan %d: J0->1"
				  " (trig Join)", trunk_p->trunk_name,
				  isl_vlan_id);  
	    trunk_p->status |= TRUNK_STS_JOIN_BIT_CHG_1; 
	}
    } else {
	trunk_p->cur_join_bits[offset] &= ~mask; /* joining bit = 0 */
	
	if ( old_join_bit ) {
	    VTP_PRUNING_LOG_TRACE(INFO, "%s trunk %s vlan %d: J1->0",
				  trunk_p->trunk_name, isl_vlan_id); 
	}
	
    }   /*if(bit=1)-else(0)*/
	
} /*vtp_pruning_build_joining_bit*/

/*-------------------------------------------------------------------------
 * vtp_pruning_is_other_joined:
 *  Determine if a specific vlan is joined at any other trunk
 *------------------------------------------------------------------------*/
vtp_boolean
vtp_pruning_is_other_joined (vlan_domain_ptr domain_p,
			     vtp_trunk_itf_ptr trunk_ptr,
			     unsigned short isl_vlan_id,
			     unsigned short offset, unsigned char mask) {
    vtp_trunk_itf_ptr	trunk_p;
    
    /* check all other trunks in this domain */
    for ( trunk_p = domain_p->trunk_head; trunk_p; trunk_p =
	 trunk_p->trunk_next) {
	if ( trunk_p == trunk_ptr ) /* exclude itself */
	    continue;
	
	if ( (trunk_p->spt_state[offset] & mask) &&
	    (trunk_p->vlan_fsm_state[isl_vlan_id] == FSM_ST_NO_PRUNE ||
	     trunk_p->vlan_fsm_state[isl_vlan_id] == FSM_ST_NORM_JOIN) ) {
	    
	    return TRUE;	    /* vlan in this trunk joined */
	}
	
    } /*for(trunks)*/ 
	
    return(FALSE);		    /* no trunk joined this vlan */
	
} /*vtp_pruning_is_other_joined*/

/**************************************************************************
 * 
 *			Utilities to deal with vlan state	
 * 
 **************************************************************************/

/*-------------------------------------------------------------------------
 * vtp_pruning_build_joined_bits:
 *  build vlan "joined/pruned" status bit
 *------------------------------------------------------------------------*/
void
vtp_pruning_build_joined_bits (vtp_trunk_itf_ptr trunk_p, 
			       unsigned char *buf) {
    unsigned short	i,j,isl_vlan_id;
    unsigned char	*sts = buf;
    unsigned char	mask;
    
    memset(buf, 0, VLAN_BIT_STR_SIZ); /* assumed pruned first */
    for( i = 0; i < VLAN_BIT_STR_SIZ; i++, sts++ ) {
		
	for( j = 0, mask = 0x80; j < 8; j++, mask >>= 1 ) {
			
	    isl_vlan_id = i*8+j+VTP_VLAN_BASE;

	    /* to be joined: must be in link up and spt forwarding state */
	    if ((trunk_p->spt_state[i] & mask) &&		/*FWD*/
		(trunk_p->link_state == TRUNK_LINK_UP))	{	/*LINK_UP*/ 
	    
		/* pruning ineligible */
		if ( !(trunk_p->vlan_pruning_eligibility[i] & mask) ) {
		    *sts |= mask;	    /* joined */
		} else {
		    /* if pruning disable or transparent: joined */
		    if (is_pruning_mode_stopped(trunk_p->domain_p) ||
			trunk_p->vlan_fsm_state[isl_vlan_id]==FSM_ST_NO_PRUNE ||
			trunk_p->vlan_fsm_state[isl_vlan_id]==FSM_ST_NORM_JOIN)
			*sts |= mask;	    /* joined */
		}
	    }	/*if(FWD && LINK_UP)*/
	}
    }
}   /*vtp_pruning_build_joined_bits*/    

