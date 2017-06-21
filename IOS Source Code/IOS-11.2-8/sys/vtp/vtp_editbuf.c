/* $Id: vtp_editbuf.c,v 1.2.58.4 1996/06/29 02:08:18 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_editbuf.c,v $
 *------------------------------------------------------------------
 * VTP MIB edit buffer implementation
 *
 * December 1995, Chris Young
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_editbuf.c,v $
 * Revision 1.2.58.4  1996/06/29  02:08:18  cyoung
 * Commit the right fix for the right bug (gah).
 * CSCdi61715:  VTP MIB walk skips VLAN entries
 * Branch: California_branch
 *
 * Revision 1.2.58.3  1996/06/29  02:04:38  cyoung
 * Update VTP MIB code to correctly use VLAN retrieval routines
 * CSCdi61715: VTP MIB walk skips VLAN entries
 * Branch: California_branch
 *
 * Revision 1.2.58.2  1996/05/21  22:29:30  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.1  1996/05/09  14:48:34  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:28  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.3  1996/03/27  23:41:38  cyoung
 * MIB - implement new trap formats (correct varbinds)
 * core - vtp_delete_domain crashes with trunk defined
 * depend - TFTP read needs its own process, implement VLAN state
 *          modifications, some cleanup
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.2  1996/03/25  02:21:51  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:45:04  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.2  1996/03/13  17:28:03  cyoung
 * Get rid of vtp_edit_vlan, use download instead.
 * Add vtp_store_domain_info, one stop database storing.
 * Put in semaphore to avoid saving database being loaded in.
 * More TFTP support.
 * Add initial code for 'vtp configured' action routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:29  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.7  1996/02/22  03:10:02  cyoung
 * Move include files around to make code more easily ported to Cat5000.
 * TruthValue false is 2 and not 0.
 * Roll function declarations from vtp_agent into vtp_interface and
 * vtp_externs (cut down on number of include files).
 * Add IOS_SPECIFIC compile flag to mark non-portable code.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.6  1996/02/08  04:12:35  cyoung
 * Set default time interval when creating or modifying management domain.
 * Add support for setting vlan filter on trunks and the native vlan.
 * Add support for transmitting adverts over LANE.
 * Add code for removing trunks from idblist.
 * Add interface for core code to notify dependant code when a mod has
 *  been made to the trunk list.
 * More code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.5  1996/02/01  19:17:37  cyoung
 * Start work on LANE encaps for VTP adverts.  Reset timer to zero when
 * freed.  Fix 'imaginative' cast for SAID value.  More basic cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.4  1996/01/31  23:15:51  cyoung
 * vtpVlanEditTranslationalVlan1 and 2 now work.  vtpVlanApplyStatus
 * reports succeeded now after inProgress.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.3  1996/01/31  02:34:07  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/31  00:44:06  cyoung
 * MIB fixes and beginning code for sub-interface creation
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/24  01:37:55  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2  1996/01/02  04:50:07  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 1.1  1995/12/26  19:10:32  cyoung
 * placeholder for VTP MIB support files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "sr_vtpmib.h"

static vtpEditDomainType* vtp_first_domain = NULL;
static vtpEditDomainType* vtp_last_domain = NULL;
static OctetString empty_string = { NULL, 0 };

/* This function is called by the AVL code to make decisions about
 * whether to branch left or right in the tree.  It is used for
 * inserting, deleting and finding nodes in the AVL tree. 
 */
enum avl_compare vtp_edit_vlan_compare (vtpEditVlanType* a,
					vtpEditVlanType* b) {

    /* I'm not going to check for a and b == NULL as I'd rather crash
       than return a uninformative return value */

    if (a->info.isl_vlan_id > b->info.isl_vlan_id)
	return AVL_GT;
    else if (a->info.isl_vlan_id < b->info.isl_vlan_id)
	return AVL_LT;
    return AVL_EQ;
}

/* Called when the edit buffer is copied or a new VLAN is added to it.
 */ 
static inline 
vtpEditVlanType* vtp_edit_vlan_insert (vtpEditVlanType** top,
				       vtpEditVlanType* new, vtp_boolean*
				       balancing_needed) { 
    return ((vtpEditVlanType*)avl_insert((avl_node_type**)top,
					 (avl_node_type*)new,
					 balancing_needed,
					 (avl_compare_type)vtp_edit_vlan_compare));
}

/* Called when the edit buffer is released or a VLAN is removed from
 * the edit buffer. */
static inline 
vtpEditVlanType* vtp_edit_vlan_delete (vtpEditVlanType** top,
				       vtpEditVlanType* target,
				       vtp_boolean* balancing_needed) {
    return ((vtpEditVlanType*)avl_delete((avl_node_type**)top,
					 (avl_node_type*)target,
					 balancing_needed,
					 (avl_compare_type)vtp_edit_vlan_compare)); 
}

/* Called by vtp_edit_apply_buffer to get the head of the AVL tree. */ 
static inline 
vtpEditVlanType* vtp_edit_get_first (vtpEditVlanType* top) {
    return ((vtpEditVlanType*)avl_get_first((avl_node_type*)top));
}

/* Called by vtp_edit_apply_buffer and vtp_edit_get_vlan_info to get
 * the VLAN immediately proceeding the current VLAN.
 */
static inline
vtpEditVlanType* vtp_edit_get_next (vtpEditVlanType* top,
				    vtpEditVlanType* current) {
    return ((vtpEditVlanType*)avl_get_next((avl_node_type*)top,
					   (avl_node_type*)current,
					   (avl_compare_type)vtp_edit_vlan_compare)); 
}

/* Called by vtp_edit_get_vlan_info to find an exact match for a
 *  'goal' VLAN.
 */
static inline
vtpEditVlanType* vtp_edit_search (vtpEditVlanType* top,
				  vtpEditVlanType* goal) {
    return ((vtpEditVlanType*)avl_search((avl_node_type*)top,
					 (avl_node_type*)goal,
					 (avl_compare_type)vtp_edit_vlan_compare));
}

/* yes, this is expected to be a small list (<10) */
static vtpEditDomainType* 
vtp_edit_find_domain (long managementDomainIndex) { 
    vtpEditDomainType* domPtr;
    
    domPtr = vtp_first_domain;
    while (domPtr) {
	if (domPtr->managementDomainIndex == managementDomainIndex)
	    return domPtr;
	domPtr = domPtr->next;
    }
    
    return NULL;
}

/* Called because the edit buffer is to be released.  This could be
 * due to the expiry of a timer, the deletion of a domain, by explicit
 * request (from SNMP) or due to an error condition when trying to
 * create a new buffer.
 */
static void vtp_free_edit_buffer (vtpEditVlanType** top) {
    vtpEditVlanType* vlan;
    vtp_boolean balancing_needed;

    balancing_needed = FALSE;
    while (*top) {
	vlan = vtp_edit_vlan_delete(top, *top, &balancing_needed);
	free(vlan);
    }
}

static void vtp_edit_timer_expired (TimeOutDescriptor* Td) {
    vtpEditDomainType* edit_domain;

    edit_domain = vtp_edit_find_domain(Td->TimerId);
    if (!edit_domain)
	return;
    if (edit_domain->owner != &empty_string)
	FreeOctetString(edit_domain->owner);
    edit_domain->owner = &empty_string;
    vtp_free_edit_buffer(&edit_domain->head);
    edit_domain->num_vlans = NO_VLANS;
    edit_domain->timer_id = 0;
}

/* Called when a new domain is created to set up values for the 
 * buffer timer. 
 */
static void vtp_edit_init_timer (vtpEditDomainType* domain) {

    domain->buffer_timer.TimeOutType = VTP_BUFFER_TIMER_TYPE;
    domain->buffer_timer.TimerId = domain->managementDomainIndex;
    domain->buffer_timer.CallBack = vtp_edit_timer_expired;
    domain->buffer_timer.UserData1 = 0;
    domain->buffer_timer.UserData2 = NULL;
}

/* Called when either the edit buffer is applied or by explicit
 * request from SNMP.
 */
static vtp_boolean vtp_edit_restart_timer (vtpEditDomainType* domain) {
    if (domain->timer_id)
	CancelTimeout(domain->timer_id);
    domain->timer_id = SetOneshotTimeout(snmp_sysUpTime() +
					 VTP_BUFFER_TIMER_PERIOD, 
					 &domain->buffer_timer);
    if (!domain->timer_id)
	return FALSE;
    return TRUE; /* no error */
}

/* list is unsorted.  Expected to be very small */
vtpEditDomainType* 
vtp_edit_add_domain_info (long managementDomainIndex) {
    vtpEditDomainType* new_element;

    new_element = malloc(sizeof(vtpEditDomainType));
    if (!new_element)
	return NULL; /* no memory */
    
    /* initial values */
    new_element->next = NULL;
    new_element->managementDomainIndex = managementDomainIndex;
    new_element->status = D_vtpVlanApplyStatus_succeeded;
    new_element->owner = &empty_string;
    new_element->head = NULL;
    new_element->num_vlans = NO_VLANS;
    vtp_edit_init_timer(new_element);
    new_element->last_rev_num_trap = 0;
    new_element->last_digest_trap = 0;
    new_element->timer_id = 0;

    if (!vtp_first_domain) {
	/* first case, new list */

	vtp_first_domain = new_element;
	vtp_last_domain = vtp_first_domain;
    } else {
	/* adding to tail */

	vtp_last_domain->next = new_element;
	vtp_last_domain = new_element;
    }
    return (new_element);
}

void vtp_edit_delete_domain_info (long managementDomainIndex) {
    vtpEditDomainType* prev, *ptr;
    ptr = vtp_first_domain;
    prev = vtp_first_domain;
    while (ptr) {
	if (ptr->managementDomainIndex == managementDomainIndex) {
	    /* delete from head */
	    if (ptr == vtp_first_domain) 
		vtp_first_domain = ptr->next;
	    else /* delete from end */
		if (ptr == vtp_last_domain) {
		    vtp_last_domain = prev;
		    prev->next = NULL;
		} else  /* delete from middle */
		    prev->next = ptr->next;
	    
	    /* delete the node */
	    vtp_free_edit_buffer(&ptr->head);
	    if (ptr->owner != &empty_string)
		FreeOctetString(ptr->owner);
	    free(ptr);
	    return;
	}
	prev = ptr;
	ptr = ptr->next;
    }
}

void vtp_edit_set_revision_number(long managementDomainIndex, unsigned
				  long revision) {
    vtpEditDomainType* element;
    
    element = vtp_edit_find_domain(managementDomainIndex);
    element->rev_num = revision;
}

void vtp_edit_set_owner (long managementDomainIndex, OctetString*
			 owner) {
    vtpEditDomainType* element;

    if (!owner)
	return;

    element = vtp_edit_find_domain(managementDomainIndex);
    if (element->owner != &empty_string)
	FreeOctetString(element->owner);
    if (owner->length == 0)
	element->owner = &empty_string;
    else 
	element->owner = CloneOctetString(owner);
}

static vtp_boolean vtp_edit_copy_buffer (long managementDomainIndex) {
    vtpEditDomainType* edit_domain;
    vtpEditVlanType* new_element;
    vtp_domain_info domain;
    vtp_vlan_info vlan;
    vtp_boolean balancing_needed;
    unsigned char error;

    edit_domain = vtp_edit_find_domain(managementDomainIndex);

    if (!edit_domain)
	return FALSE;  /* should never happen */
    
    domain.index = managementDomainIndex;
    if (k_get_vtp_domain(&domain, EXACT))
	return FALSE; /* should never happen */

    error = vtp_get_vlan_info(domain.domain_name_length, domain.domain_name,
			      &vlan, VTP_FIRST); 

    while (!error) {
	new_element = malloc(sizeof(vtpEditVlanType));

	/* we're out of memory, clean up and return */
	if (!new_element) { 
	    vtp_free_edit_buffer(&edit_domain->head);
	    return FALSE; 
	}
	memcpy(&new_element->info, &vlan, sizeof(vtp_vlan_info));
	vtp_edit_vlan_insert(&edit_domain->head, new_element,
			     &balancing_needed);
	edit_domain->num_vlans++;
	error = vtp_get_vlan_info(domain.domain_name_length,
				  domain.domain_name, &vlan, VTP_NEXT);
    }

    if (edit_domain->timer_id) 
	CancelTimeout(edit_domain->timer_id);
	
    edit_domain->timer_id = SetOneshotTimeout(snmp_sysUpTime() +
					      VTP_BUFFER_TIMER_PERIOD,
					      &edit_domain->buffer_timer);
    if (!edit_domain->timer_id) {
	vtp_free_edit_buffer(&edit_domain->head);
	return FALSE; /* couldn't start timer */
    }
    
    /* bump proposed revision number by one */
    edit_domain->rev_num = domain.revision + 1;
    return TRUE;
}

static vtp_boolean vtp_edit_apply_buffer (long managementDomainIndex) {
    vtpEditDomainType* edit_domain;
    vtpEditVlanType* vlan;
    vtp_domain_info domain;
    unsigned char* buffer, *ptr;
    
    edit_domain = vtp_edit_find_domain(managementDomainIndex);

    if (!edit_domain)
	return FALSE; /* should never happen */

    domain.index = edit_domain->managementDomainIndex;
    if (k_get_vtp_domain(&domain, EXACT))
	return FALSE; /* should never happen */

    buffer = malloc(sizeof(vtp_domain_info) + 
		    (edit_domain->num_vlans * sizeof(vtp_vlan_info))); 
    if (!buffer)
	return FALSE; /*out of memory */

    ptr = buffer;
    domain.mode = VTP_SERVER;
    domain.vlan_count = edit_domain->num_vlans;
    domain.revision = edit_domain->rev_num;
    domain.updater = vtp_dep_get_local_ipaddr();
    vtp_get_timestamp(domain.timestamp);
    memcpy(ptr, &domain, sizeof(vtp_domain_info));
    ptr += sizeof(vtp_domain_info);

    vlan = vtp_edit_get_first(edit_domain->head);
    if (!vlan)
	return FALSE; /* should never happen */
    while (vlan) {
	memcpy(ptr, &vlan->info, sizeof(vtp_vlan_info));
	ptr += sizeof(vtp_vlan_info);
	vlan = vtp_edit_get_next(edit_domain->head, vlan);
    }

    edit_domain->status = D_vtpVlanApplyStatus_inProgress;

    vtp_post_download_config(domain.domain_name,
			     domain.domain_name_length, TRUE, FALSE,
			     buffer);

    if (vtp_edit_restart_timer(edit_domain))
	return FALSE; /* No recovery from this anyway */

    return TRUE;
}

/* This function is called (perhaps indirectly) by the VTP core code
 * to report the result of a download request.  The result is stored
 * for retrieval by SNMP. 
 */
void vtp_report_apply_status (vtp_domain_info* info, 
			      long vtpVlanApplyStatus) {
    vtpEditDomainType* edit_domain;

    if (!info)
	return; /* Zot! */

    edit_domain = vtp_edit_find_domain(info->index);

    if (!edit_domain)
	return; /* no recovery */

    if (edit_domain->status != D_vtpVlanApplyStatus_inProgress)
	return; /* wasn't related to a MIB action */

    edit_domain->status = vtpVlanApplyStatus;
    return;
}

vtp_boolean vtp_edit_release_buffer (long managementDomainIndex) {
    vtpEditDomainType* edit_domain;

    edit_domain = vtp_edit_find_domain(managementDomainIndex);
    if (!edit_domain)
	return FALSE; 

    if (edit_domain->timer_id) {
	CancelTimeout(edit_domain->timer_id);
	edit_domain->timer_id = 0;
    }
    if (edit_domain->owner != &empty_string)
	FreeOctetString(edit_domain->owner);
    edit_domain->owner = &empty_string;
    vtp_free_edit_buffer(&edit_domain->head);
    edit_domain->num_vlans = NO_VLANS;
    return TRUE; 
}

vtp_boolean vtp_edit_do_operation (long managementDomainIndex, 
			   long operation) {
    vtpEditDomainType* edit_domain;

    switch (operation) {
    case D_vtpVlanEditOperation_none:
	return TRUE; /* no error */
    case D_vtpVlanEditOperation_copy:
	return(vtp_edit_copy_buffer(managementDomainIndex));
    case D_vtpVlanEditOperation_apply:
	return(vtp_edit_apply_buffer(managementDomainIndex));
    case D_vtpVlanEditOperation_release:
	return(vtp_edit_release_buffer(managementDomainIndex));
    case D_vtpVlanEditOperation_restartTimer:
	edit_domain = vtp_edit_find_domain(managementDomainIndex);
	if (!edit_domain)
	    return FALSE; /* shouldn't happen */
	if (vtp_edit_restart_timer(edit_domain))
	    return FALSE;
	return TRUE;
    default:
	return FALSE; /* shouldn't happen */
    }
}

vtpEditDomainType*
vtp_edit_get_domain_info (int searchType, long managementDomainIndex)
{
    vtpEditDomainType* domPtr;
    vtp_domain_info info;

    info.index = managementDomainIndex;
    if (k_get_vtp_domain(&info, searchType))
	return NULL;
    
    domPtr = vtp_edit_find_domain(info.index);
    if (domPtr)
	return domPtr;

    /* We found one in the core data so we should have one here.
       Create one. */

    domPtr = vtp_edit_add_domain_info(managementDomainIndex);
    if (!domPtr)
	return NULL; /* no memory */

    vtp_edit_set_revision_number(managementDomainIndex, info.revision);
    return (domPtr);
}

vtpEditVlanType* 
vtp_edit_get_vlan_info (int searchType, vtpEditDomainType* e_domain,
			long vlanIndex) { 
    vtpEditVlanType current, *vlan;
    if ((!e_domain) || (!e_domain->head))
	return NULL;

    current.info.isl_vlan_id = vlanIndex;
    vlan = vtp_edit_search(e_domain->head, &current);
    if ((!vlan) && (searchType == NEXT))
	vlan = vtp_edit_get_next(e_domain->head, &current);

    return vlan;
}
	
vtp_boolean vtp_edit_update_vlan_info (vtpVlanEditEntry_t* data) {
    vtpEditDomainType* e_domain;
    vtpEditVlanType* vlan;

    e_domain = vtp_edit_get_domain_info(EXACT,
					data->managementDomainIndex);
    if (!e_domain)
	return FALSE; /* should never happen, already checked in _ready */
    vlan = vtp_edit_get_vlan_info(EXACT, e_domain,
				  data->vtpVlanEditIndex);
    if (!vlan)
	return FALSE; /* should never happen, already checked in _ready
		    */
    vlan->info.state = data->vtpVlanEditState;
    vlan->info.type = data->vtpVlanEditType;
    vlan->info.vlan_name_length = data->vtpVlanEditName->length;
    memcpy(vlan->info.vlan_name, data->vtpVlanEditName->octet_ptr,
	   vlan->info.vlan_name_length); 
    vlan->info.mtu = data->vtpVlanEditMtu;
    memcpy(&vlan->info.said, data->vtpVlanEditDot10Said->octet_ptr,
	   VTP_SAID_LENGTH); 
    vlan->info.ring_number = data->vtpVlanEditRingNumber;
    vlan->info.bridge_number = data->vtpVlanEditBridgeNumber;
    vlan->info.parent_vlan = data->vtpVlanEditParentVlan;
    vlan->info.tb_vlan_1 = data->vtpVlanEditTranslationalVlan1;
    vlan->info.tb_vlan_2 = data->vtpVlanEditTranslationalVlan2;
    return TRUE; /* no error */
}

vtp_boolean vtp_edit_create_vlan_info (vtpVlanEditEntry_t* data) {
    vtpEditDomainType* e_domain;
    vtpEditVlanType* vlan;
    vtp_boolean balancing_needed;

    e_domain = vtp_edit_get_domain_info(EXACT,
					data->managementDomainIndex);
    if (!e_domain)
	return FALSE; /* should never happen, already checked in _ready
		    */

    vlan = malloc(sizeof(vtpEditVlanType));
    
    /* we're out of memory */
    if (!vlan)  
	return FALSE; 
    
    vlan->info.isl_vlan_id = data->vtpVlanEditIndex;
    vlan->info.state = data->vtpVlanEditState;
    vlan->info.type = data->vtpVlanEditType;
    vlan->info.vlan_name_length = data->vtpVlanEditName->length;
    memcpy(vlan->info.vlan_name, data->vtpVlanEditName->octet_ptr,
	   vlan->info.vlan_name_length); 
    vlan->info.mtu = data->vtpVlanEditMtu;
    memcpy(&vlan->info.said, data->vtpVlanEditDot10Said->octet_ptr,
	   VTP_SAID_LENGTH); 
    vlan->info.ring_number = data->vtpVlanEditRingNumber;
    vlan->info.bridge_number = data->vtpVlanEditBridgeNumber;
    vlan->info.stp_type = data->vtpVlanEditBridgeType;
    vlan->info.parent_vlan = data->vtpVlanEditParentVlan;
    vlan->info.tb_vlan_1 = data->vtpVlanEditTranslationalVlan1;
    vlan->info.tb_vlan_2 = data->vtpVlanEditTranslationalVlan2;

    vtp_edit_vlan_insert(&e_domain->head, vlan, &balancing_needed);
    e_domain->num_vlans++;
    return TRUE; /* no error */
}

vtp_boolean vtp_edit_delete_vlan_info (long managementDomainIndex, long
				     vlanIndex) {
    vtpEditDomainType* e_domain;
    vtpEditVlanType *vlan, target;
    vtp_boolean balancing_needed;

    e_domain = vtp_edit_get_domain_info(EXACT, managementDomainIndex);
    if (!e_domain)
	return FALSE; /* bogus domain */
    
    target.info.isl_vlan_id = vlanIndex;
    vlan = vtp_edit_vlan_delete(&e_domain->head, &target,
				&balancing_needed); 
    if (!vlan)
	return FALSE; /* wasn't there */
    e_domain->num_vlans--;
    free(vlan);
    return TRUE;  /* no error */
}
 
