/* $Id: vtp_local.h,v 1.10 1996/10/08 02:29:31 bpan Exp $
 * $Source: /wbu-sw/dev/synergy/syn_core/nmp/nmpflash/vtp/vtp_local.h,v $
 *------------------------------------------------------------------
 * vtp_local.h
 *   Function prototypes for VTP internal use only
 *   Currently only pruning related functions included
 *
 *   For those routines internally invoked by pruning functions, see
 *   vtp_pub.h
 *
 * Jul 1996, Bider Pan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_local.h,v $
 * Revision 1.10  1996/10/08  02:29:31  bpan
 * Set management vlan to be always "vlan local assoc"
 *
 * Revision 1.9  1996/10/01  19:24:09  bpan
 * Cleanup and reorganized trace & syslog msg for vtp pruning
 *
 * Revision 1.8  1996/09/18  00:34:18  bpan
 * Initially added "pruning en/disable" supports
 *
 * Revision 1.7  1996/09/13  21:08:54  bpan
 * Changed engineering function prototypes for pruning
 *
 * Revision 1.6  1996/08/28  02:04:55  bpan
 * Changed vtp pruning debugging messages
 *
 * Revision 1.5  1996/08/23  22:08:39  bpan
 * Changed core function prototypes for status/statistics
 * Changed trace category to PRUNING_TRACE
 *
 * Revision 1.4  1996/08/16  22:01:56  bpan
 * add more debugging utility prototypes
 *
 * Revision 1.3  1996/08/11  06:07:49  bpan
 * added more routines
 *
 * Revision 1.2  1996/08/03  06:29:04  bpan
 * add minor changes
 *
 * Revision 1.1  1996/07/19  02:23:15  bpan
 * Initially checked in
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VTP_LOCAL_H__
#define __VTP_LOCAL_H__


/*================== Pruning Internal Function Prototypes =================*/


/* ----- interface to vtp.c ----- */
void vtp_pruning_change_to_non_transparent(vlan_domain_ptr domain_p);

void vtp_pruning_change_to_transparent(vlan_domain_ptr domain_p);

void vtp_pruning_add_domain_init(vlan_domain_ptr domain_p);
void vtp_pruning_add_domain(vlan_domain_ptr domain_p);

void vtp_pruning_add_trunk_init(vlan_domain_ptr domain_p, 
				vtp_trunk_itf_ptr trunk_p);
void vtp_pruning_add_trunk(vlan_domain_ptr domain_p, 
			   vtp_trunk_itf_ptr trunk_p);

void vtp_pruning_add_vlan_init(vlan_domain_ptr domain_p, 
			       vlan_info_ent_ptr vlan_p);
void vtp_pruning_add_vlan_refresh(vlan_domain_ptr domain_p, 
				  vlan_info_ent_ptr vlan_p);
void vtp_pruning_add_vlan(vlan_domain_ptr domain_p, 
			  vlan_info_ent_ptr vlan_p);

void vtp_pruning_delete_domain(vlan_domain_ptr domain_p);

void vtp_pruning_delete_trunk(vlan_domain_ptr domain_p,
			      vtp_trunk_itf_ptr trunk_p);

void vtp_pruning_delete_vlan(vlan_domain_ptr domain_p,
			     vlan_info_ent_ptr vlan_p); 
void vtp_pruning_flush_database(vlan_domain_ptr domain_p);

void vtp_pruning_test_and_set_database_busy(vlan_domain_ptr domain_p);
void vtp_pruning_reset_database_busy(vlan_domain_ptr domain_p);

void vtp_pruning_init( void );

void vtp_receive_join_packet(void *trunk_id, void *buff_p, 
			     unsigned long length, unsigned char *src_mac);

void vtp_pruning_check_version(vtp_trunk_itf_ptr trunk_p, 
			       summary_adv_ptr pkt_p, unsigned long length );
vtp_boolean vtp_pruning_check_summary_tlv(vlan_domain_ptr domain_p, 
				      summary_adv_ptr pkt_p, 
				      unsigned long length, 
				      unsigned char *mode);

void vtp_pruning_timeout( void *context );
void vtp_pruning_trig_timeout(vlan_domain_ptr domain_p, 
			      vtp_trunk_itf_ptr trunk_p);
void vtp_pruning_tick_timeout(vlan_domain_ptr domain_p, 
			      vtp_trunk_itf_ptr trunk_p);

void* vtp_pruning_generate_join(vlan_domain_ptr domain_p, 
				    vtp_trunk_itf_ptr trunk_p,
				    unsigned int *len);

/* ----- utilities for pruning interface ----- */
void		vtp_pruning_setup_vlan_range(vlan_domain_ptr domain_p);
unsigned int	vtp_pruning_jittered_value( void );
vtp_boolean		vtp_pruning_is_joining_bit_chg_1( vtp_trunk_itf_ptr trunk_p );

/* misc */
void	vtp_pruning_fsm_init(vlan_domain_ptr domain_p,
			     vtp_trunk_itf_ptr trunk_ptr); 
void	vtp_pruning_sts_array_full(vtp_trunk_itf_ptr trunk_p);

/* ----- pruning FSM utilities ----- */
void vtp_pruning_enable_domain_vlan_state(vlan_domain_ptr domain_p,
					  unsigned short isl_vlan_id);
void vtp_pruning_enable_trunk_vlan_state(vtp_trunk_itf_ptr trunk_p,
					 unsigned short isl_vlan_id);
void vtp_pruning_enable_vlan_state(vtp_trunk_itf_ptr trunk_p, 
				   unsigned short isl_vlan_id);
void vtp_pruning_init_domain_vlan_state(vlan_domain_ptr domain_p,
					unsigned short isl_vlan_id);
void vtp_pruning_init_trunk_vlan_state(vlan_domain_ptr domain_p, 
				       vtp_trunk_itf_ptr trunk_p);
void vtp_pruning_init_vlan_state(vlan_domain_ptr domain_p, 
				 vtp_trunk_itf_ptr trunk_p, 
				 unsigned short isl_vlan_id);
void vtp_pruning_chg_eligible(vlan_domain_ptr domain_p, 
			      unsigned short isl_vlan_id, unsigned char flag);

/* ----- pruning FSM Pre-Processors ----- */
void vtp_pruning_fsm_join(vtp_trunk_itf_ptr trunk_p, join_msg_ptr join_p);
void vtp_pruning_fsm_tmo( vtp_trunk_itf_ptr trunk_p );
void vtp_pruning_fsm_spt( vtp_trunk_itf_ptr trunk_p, 
			 unsigned short isl_vlan_id,
			 unsigned short spt_state );
void vtp_pruning_fsm_spt_all( vtp_trunk_itf_ptr trunk_p );
void vtp_pruning_fsm_linkst(vtp_trunk_itf_ptr trunk_p, 
			    unsigned char link_state); 
void vtp_pruning_fsm_ver1( vtp_trunk_itf_ptr trunk_p );
void vtp_pruning_fsm_eligible(vlan_domain_ptr domain_p,
			      vtp_trunk_itf_ptr trunk_p,
			      unsigned char *eligible);
void vtp_pruning_fsm_non_transparent(vlan_domain_ptr domain_p);
void vtp_pruning_fsm_transparent(vlan_domain_ptr domain_p);

/* ----- pruning FSM Processor ----- */
void vtp_pruning_fsm(vtp_trunk_itf_ptr trunk_p, unsigned short isl_vlan_id,
		     unsigned char event );

/* ----- pruning FSM action utilities ----- */
void vtp_pruning_fsm_util_set_joined(vtp_trunk_itf_ptr trunk_p, 
				     unsigned short isl_vlan_id);
void vtp_pruning_fsm_util_set_pruneed(vtp_trunk_itf_ptr trunk_p,
				      unsigned short isl_vlan_id);

/* ----- pruning FSM action routines ----- */
const void vtp_pruning_fsm_act0_null(vtp_trunk_itf_ptr trunk_p, 
				     unsigned short isl_vlan_id,
				     unsigned char event);
const void vtp_pruning_fsm_act1_reset_t1(vtp_trunk_itf_ptr trunk_p, 
					 unsigned short isl_vlan_id,
					 unsigned char event);
const void vtp_pruning_fsm_act2_reset_t2(vtp_trunk_itf_ptr trunk_p, 
					 unsigned short isl_vlan_id,
					 unsigned char event);
const void vtp_pruning_fsm_act3_update_t1t2(vtp_trunk_itf_ptr trunk_p, 
					    unsigned short isl_vlan_id,
					    unsigned char event);
const void vtp_pruning_fsm_act4_goto_noprune(vtp_trunk_itf_ptr trunk_p, 
					     unsigned short isl_vlan_id,
					     unsigned char event);
const void vtp_pruning_fsm_act5_prune(vtp_trunk_itf_ptr trunk_p, 
				      unsigned short isl_vlan_id,
				      unsigned char event);
const void vtp_pruning_fsm_act6_set_join_on_ver1(vtp_trunk_itf_ptr trunk_p, 
						 unsigned short isl_vlan_id,
						 unsigned char event);
const void vtp_pruning_fsm_act7_set_join_on_join1(vtp_trunk_itf_ptr trunk_p, 
						  unsigned short isl_vlan_id,
						  unsigned char event);
const void vtp_pruning_fsm_act8_set_join_on_join0(vtp_trunk_itf_ptr trunk_p, 
						  unsigned short isl_vlan_id,
						  unsigned char event);

/* ----- pruning FSM Post-Processor ----- */
void vtp_pruning_fsm_post_proc( vtp_trunk_itf_ptr trunk_p );

/* ----- utility to notify dependent the vlan state change ----- */
void vtp_pruning_process_state_change(vlan_domain_ptr domain_p, 
				      vtp_trunk_itf_ptr trunk_ptr);

/* ----- utilities to handle triggered Join conditions ----- */
void vtp_pruning_process_all_other_trig_joins(vlan_domain_ptr domain_p, 
					  vtp_trunk_itf_ptr trunk_ptr);
void vtp_pruning_process_trig_join(vtp_trunk_itf_ptr trunk_p);
void vtp_pruning_start_trig_join(vtp_trunk_itf_ptr trunk_p);

/* ----- utilities to send Join msg ----- */
void vtp_pruning_flooding(vlan_domain_ptr domain_p,vtp_trunk_itf_ptr trunk_ptr);
void vtp_pruning_tx_join_msg(vtp_trunk_itf_ptr trunk_p);
/*void vtp_pruning_tx_join_msg(vtp_trunk_itf_ptr trunk_p, unsigned long ms,
			     unsigned long jitter);*/

/* ----- utilities to deal with joining bits ----- */
void vtp_pruning_build_all_other_joining_bits(vlan_domain_ptr domain_p,
					vtp_trunk_itf_ptr trunk_ptr, 
					unsigned short isl_vlan_id);
void vtp_pruning_build_all_joining_bits(vlan_domain_ptr domain_p, 
				     vtp_trunk_itf_ptr trunk_p);
unsigned char vtp_pruning_are_other_joined(vlan_domain_ptr domain_p,
					   vtp_trunk_itf_ptr trunk_ptr, 
					   unsigned short offset);
void vtp_pruning_build_joining_bit(vlan_domain_ptr domain_p,
				vtp_trunk_itf_ptr trunk_p, 
				unsigned short isl_vlan_id);

vtp_boolean vtp_pruning_is_other_joined(vlan_domain_ptr domain_p,
				    vtp_trunk_itf_ptr trunk_ptr, 
				    unsigned short isl_vlan_id,
				    unsigned short offset, 
				    unsigned char mask);

/* ----- utilities to deal with vlan state ----- */
void vtp_pruning_build_joined_bits(vtp_trunk_itf_ptr trunk_p, 
				   unsigned char *buf);

/* ----- debugging utilities ----- */
void vtp_pruning_print_join(vtp_trunk_itf_ptr trunk_p, join_msg_ptr join_p);
void vtp_pruning_cmp_joining_bits(unsigned char old, unsigned new,
			       unsigned short vlanid);

/* ----- utilities to search ----- */
vlan_domain_ptr vtp_search_domain(unsigned char dname_length, 
					  unsigned char *domain_name);
vtp_trunk_itf_ptr vtp_search_trunk_link(vlan_domain_ptr domain_p,
					   void *trunk_id);

/* ----- Utilities for debugging/engineering functions ----- */
void show_vtp_pruning_trunk_detail(vlan_domain_ptr domain_p, 
				   vtp_trunk_itf_ptr trunk_p,
				   unsigned short start_vlan, 
				   unsigned short vlan_num);
void show_vtp_pruning_vlan_detail(vlan_domain_ptr domain_p, 
				  vlan_info_ent_ptr vlan_info_p);
void show_vtp_pruning_trunk_vlan_detail(vtp_trunk_itf_ptr trunk_p, 
					vlan_info_ent_ptr vlan_info_p,
					unsigned char flag);
void show_vtp_pruning_status_detail(vtp_trunk_itf_ptr trunk_p);


/*================ End of Pruning Internal Function Prototypes ===============*/

#endif  /*__VTP_LOCAL_H__*/
