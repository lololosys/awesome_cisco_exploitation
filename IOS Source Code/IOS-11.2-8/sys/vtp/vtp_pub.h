/* $Id: vtp_pub.h,v 1.10 1996/10/08 02:29:32 bpan Exp $
 * $Source: /wbu-sw/dev/synergy/syn_core/nmp/nmpflash/vtp/vtp_pub.h,v $
 *------------------------------------------------------------------
 * vtp_pub.h:
 *   Header file for global VTP function prototypes
 *   It declares all interface routines for VTP core & dependent functions, 
 *   Contains only vtp pruning related now
 *
 *   For those routines internally invoked by pruning functions, see
 *   vtp_local.h
 *
 * Jun 1996, Bider Pan
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_pub.h,v $
 * Revision 1.10  1996/10/08  02:29:32  bpan
 * Set management vlan to be always "vlan local assoc"
 *
 * Revision 1.9  1996/10/03  23:37:25  bpan
 * Added more comments and rearrange function prototypes
 *
 * Revision 1.8  1996/10/03  20:23:48  bpan
 * Removed redundant "\n" from syslog msg
 *
 * Revision 1.7  1996/09/13  21:08:55  bpan
 * Changed engineering function prototypes for pruning
 *
 * Revision 1.6  1996/09/07  01:03:04  bpan
 * -Added more functions for engineering commands
 * -Modified function prototypes
 * -Modified tracing messages
 *
 * Revision 1.5  1996/08/23  22:08:43  bpan
 * Changed core function prototypes for status/statistics
 * Changed trace category to PRUNING_TRACE
 *
 * Revision 1.4  1996/08/16  22:11:09  bpan
 * Added more core function prototypes
 *
 * Revision 1.3  1996/08/11  06:07:21  bpan
 * added more routines
 *
 * Revision 1.2  1996/08/03  06:31:55  bpan
 * add changes after unit test
 *
 * Revision 1.1  1996/07/19  02:22:45  bpan
 * Initially checked in
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __VTP_PUB_H__
#define __VTP_PUB_H__

/* ----- global constants ----- */
#define TRUNK_LINK_DOWN		0
#define TRUNK_LINK_UP		1
#define VLAN_SPT_STATE_FWD	1
#define VLAN_SPT_STATE_NON_FWD	0
#define VLAN_SPT_STATE_NA	0xff /* not available */
#define VLAN_LOCAL_ASSOC	1
#define VLAN_LOCAL_ASSOC_NONE	0
#define VLAN_LOCAL_ASSOC_NA	0xff /* not available */

/*====================== Pruning Function Prototypes =====================*/

/*=========================================================================
 *
 * Public for all:
 * Note: in a multitasking environment, appropriate "semaphore" scheme
 *	 may be needed to provide mutual exclusively access to vtp database
 *	 (issue left for dependen code)
 *
 *========================================================================*/

/* ----- VTP PRUNING Trunk Statistics structure ----- */
typedef struct {
	unsigned long	tx_join;
	unsigned long	rx_join;
	unsigned long	rx_no_pruning_support_msg;
} vtp_trunk_stat;

/* ----- VTP PRUNING vlan pruned/joined status change structure ----- */
typedef struct {
	unsigned short	isl_vlan_id;
	unsigned short	isl_vlan_status;
#define VLAN_STS_JOINED		0x01
#define VLAN_STS_PRUNED		0x00
} vtp_vlan_status;

/* ----- interface to dependent code ----- */
void vtp_core_change_trunk_link_state(unsigned char dname_length, 
    unsigned char *domain_name, void *trunk_id, unsigned char up_flag);

void vtp_core_change_trunk_vlan_spt_state(unsigned char dname_length, 
    unsigned char *domain_name, unsigned short isl_vlan_id, 
    void *trunk_id, unsigned short new_spt_state);

void vtp_core_change_trunk_all_vlan_spt_state(unsigned char dname_length, 
    unsigned char *domain_name, void *trunk_id, unsigned char *spt_state);

void vtp_core_change_vlan_local_assoc(unsigned char dname_length, 
    unsigned char *domain_name, unsigned short isl_vlan_id, 
    unsigned char flag); 

void vtp_core_change_all_vlan_local_assoc(unsigned char dname_length, 
    unsigned char *domain_name, unsigned char *local_assoc);

void 
vtp_core_change_vlan_pruning_eligible(void* trunk_id,
				      unsigned char *vlan_prune_eligibility);

void vtp_core_set_vlan_eligible_on_trunk(void* trunk_id, 
					 unsigned short vlan_id, 
					 vtp_boolean is_eligible);

/* ----- interface to get status ----- */
vtp_boolean vtp_core_get_trunk_pruning_status(unsigned char dname_length, 
					  unsigned char *domain_name,
					  void *trunk_id,
					  unsigned char *pruning_status); 

Vtp_Err_t vtp_core_get_vlan_pruning_status(unsigned char dname_length,
					   unsigned char *domain_name,
					   void* trunk_id, 
					   unsigned short vlan_id,
					   vtp_boolean* status);

vtp_boolean vtp_core_get_trunk_joining_status(unsigned char dname_length, 
					  unsigned char *domain_name,
					  void *trunk_id, 
					  unsigned char *joining_status); 

vtp_boolean vtp_core_get_trunk_eligibility(void* trunk_id, 
				       unsigned char* eligibility);

unsigned short vtp_core_get_trunk_vlan_spt_state(unsigned char dname_length, 
						 unsigned char *domain_name,
						 unsigned short isl_vlan_id, 
						 void *trunk_id); 

unsigned char vtp_core_get_vlan_local_assoc(unsigned char dname_length, 
					    unsigned char *domain_name, 
					    unsigned short isl_vlan_id); 

void vtp_core_get_vlan_exist(unsigned char dname_length, 
			     unsigned char *domain_name, 
			     unsigned char **vlan_bitmap);
vtp_boolean vtp_core_is_vlan_exist(unsigned char dname_length, 
			    unsigned char *domain_name, 
			    unsigned short isl_vlan_id);

/* ----- interface to statistics ----- */
vtp_boolean vtp_core_get_trunk_statistics(unsigned char dname_length, 
				      unsigned char *domain_name, 
				      void *trunk_id, 
				      vtp_trunk_stat *statistics);

vtp_boolean vtp_core_clear_trunk_statistics(unsigned char clear_all, 
					unsigned char dname_length, 
					unsigned char *domain_name, 
					void *trunk_id);

/*=========================================================================
 *
 * ONLY for dependent code: vtp_xxx_change_notification()
 * Other dependent portion should NOT call these routines
 *
 *========================================================================*/
void vtp_core_set_trunk_link_state(unsigned char dname_length, 
				   unsigned char *domain_name, void *trunk_id, 
				   unsigned char up_flag );

void vtp_core_set_trunk_vlan_spt_state(unsigned char dname_length, 
				       unsigned char *domain_name, 
				       unsigned short isl_vlan_id, 
				       void *trunk_id, 
				       unsigned short new_spt_state );

void vtp_core_set_trunk_all_vlan_spt_state(unsigned char dname_length, 
					   unsigned char *domain_name, 
					   void *trunk_id, 
					   unsigned char *spt_state );

void vtp_core_set_vlan_local_assoc(unsigned char dname_length, 
				   unsigned char *domain_name, 
				   unsigned short isl_vlan_id, 
				   unsigned char flag );

void vtp_core_set_all_vlan_local_assoc(unsigned char dname_length, 
				       unsigned char *domain_name, 
				       unsigned char *vlan_local_assoc );

void vtp_core_set_vlan_pruning_eligible(unsigned char dname_length, 
					unsigned char *domain_name, 
					void* trunk_id,
					unsigned char *vlan_prune_eligibility );

/*========================================================================
 *
 * Dependent functions: needed by vtp core code
 *
 *========================================================================*/

unsigned char vtp_dep_vlan_state_change_notification(unsigned char dname_length,
    unsigned char *domain_name, void *trunk_id, unsigned short num_changed_vlan,
    vtp_vlan_status *info);

/*=========================================================================
 *
 * ONLY for debugging:
 * Dependent code should NOT call these routines
 *
 *========================================================================*/
/* ----- Debugging/Engineering Functions ----- */
void show_vtp_core_pruning_domain(unsigned char dname_length, 
				  unsigned char *domain_name);
void show_vtp_core_pruning_trunk(unsigned char dname_length, 
				 unsigned char *domain_name, void *trunk_id,
				 unsigned short start_vlan, 
				 unsigned short vlan_num);
void show_vtp_core_pruning_vlan(unsigned char dname_length, 
				unsigned char *domain_name, 
				unsigned short start_vlan,
				unsigned short vlan_num);
void show_vtp_core_pruning_status(unsigned char dname_length, 
				  unsigned char *domain_name, void *trunk_id);
void set_pruning_join_msg(unsigned char dname_length, 
			  unsigned char *domain_name, void *trunk_id,
			  unsigned short min_vlan_no, unsigned short max_vlan_no,
			  unsigned char *join_bits, 
			  void **buf, unsigned int *data_len);
void set_pruning_ver1_msg(unsigned char dname_length, 
			  unsigned char *domain_name, void *trunk_id,
			  unsigned char join_support_flag,
			  void **buf, unsigned int *data_len);

/* functions to update internal structure fields */
void set_pruning_eligible(unsigned char dname_length, 
			  unsigned char *domain_name, 
			  void* trunk_id,
			  unsigned short isl_vlan_id, unsigned char flag);
void set_pruning_local_assoc(unsigned char dname_length, 
			     unsigned char *domain_name, 
			     unsigned short isl_vlan_id, unsigned char flag);
void set_pruning_vlan_exist(unsigned char dname_length, 
			    unsigned char *domain_name, 
			    unsigned short isl_vlan_id, unsigned char flag);
void set_pruning_min_max_vlan(unsigned char dname_length, 
			      unsigned char *domain_name, 
			      unsigned short vlan_no, unsigned char min_flag);
void set_pruning_operation_status(unsigned char dname_length, 
				  unsigned char *domain_name, 
				  unsigned char status);
void set_pruning_mode(unsigned char dname_length, 
		      unsigned char *domain_name, unsigned mode);
void set_pruning_joining_bit(unsigned char dname_length, 
			     unsigned char *domain_name, void *trunk_id,
			     unsigned short isl_vlan_id, unsigned char flag);
void set_pruning_spt_state(unsigned char dname_length, 
			   unsigned char *domain_name, void *trunk_id,
			   unsigned short isl_vlan_id, unsigned char flag);
void set_pruning_fsm_state(unsigned char dname_length, 
			   unsigned char *domain_name, void *trunk_id,
			   unsigned short isl_vlan_id, unsigned char state);
void set_pruning_t2(unsigned char dname_length, 
		    unsigned char *domain_name, void *trunk_id,
		    unsigned short isl_vlan_id, unsigned char t2_value);
void set_pruning_link_state(unsigned char dname_length, 
			    unsigned char *domain_name, void *trunk_id,
			    unsigned char flag);
void set_pruning_t1(unsigned char dname_length, 
		    unsigned char *domain_name, void *trunk_id,
		    unsigned short t1_value);
void set_pruning_status_array_num(unsigned char dname_length, 
				  unsigned char *domain_name, void *trunk_id,
				  unsigned short num);
void set_pruning_busy(unsigned char dname_length, 
		      unsigned char *domain_name, unsigned int flag);
void set_pruning_T3(unsigned char T3_value);

/*=========================================================================
 *
 * for functions internally referenced only, see "vtp_local.h"
 *
 *========================================================================*/



/*====================== End of Pruning Function Prototypes ==============*/

#endif	/*__VTP_PUB_H__*/
