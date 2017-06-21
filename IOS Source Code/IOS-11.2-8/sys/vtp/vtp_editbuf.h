/* $Id: vtp_editbuf.h,v 1.2.58.1 1996/05/09 14:48:35 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_editbuf.h,v $
 *------------------------------------------------------------------
 * VTP MIB edit buffer header file
 *
 * December, 1995 - Chris Young
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_editbuf.h,v $
 * Revision 1.2.58.1  1996/05/09  14:48:35  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:38  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:29  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.1  1996/03/22  09:45:05  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.48.1  1996/03/05  05:37:36  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:31  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.1  1996/01/24  01:37:56  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2  1996/01/02  04:50:08  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 1.1  1995/12/26  19:10:33  cyoung
 * placeholder for VTP MIB support files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef VTP_EDITBUF_H
#define VTP_EDITBUF_H

typedef struct vtpEditVlanType_ {
    avl_node_type avlnode;
    vtp_vlan_info info;
} vtpEditVlanType;

#define VTP_BUFFER_TIMER_TYPE 1
#define VTP_REV_NUM_TRAP_TIMER_TYPE 2
#define VTP_DIGEST_TRAP_TIMER_TYPE 3

/* period in hundredths of a second */
#define VTP_BUFFER_TIMER_PERIOD 30000

typedef struct vtpEditDomainType_ {
    struct vtpEditDomainType_ *next;
    long managementDomainIndex;
    long status;
    OctetString* owner;
    unsigned long rev_num;
    vtpEditVlanType* head;
    unsigned int num_vlans;
    TimeOutDescriptor buffer_timer;
    unsigned long last_rev_num_trap;
    unsigned long last_digest_trap;
    int timer_id;
} vtpEditDomainType;

extern vtpEditDomainType* 
vtp_edit_add_domain_info(long managementDomainIndex); 

extern void vtp_edit_delete_domain_info(long managementDomainIndex);

extern vtpEditDomainType* 
vtp_edit_get_domain_info(int searchType, long managementDomainIndex); 

extern void vtp_edit_set_revision_number(long managementDomainIndex,
					 unsigned long revision);
extern void vtp_edit_set_owner(long managementDomainIndex,
			       OctetString* owner);

extern vtp_boolean vtp_edit_do_operation(long managementDomainIndex, 
				 long operation);

extern vtpEditVlanType* 
vtp_edit_get_vlan_info(int searchType, vtpEditDomainType* e_domain, 
		       long vlanIndex); 

extern vtp_boolean vtp_edit_update_vlan_info(vtpVlanEditEntry_t* data);
extern vtp_boolean vtp_edit_create_vlan_info(vtpVlanEditEntry_t* data);
extern vtp_boolean vtp_edit_delete_vlan_info(long managementDomainIndex, 
				     long vlanIndex); 

#endif
