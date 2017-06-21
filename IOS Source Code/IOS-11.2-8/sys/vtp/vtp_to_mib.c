/* $Id: vtp_to_mib.c,v 1.2.58.1 1996/05/09 14:48:42 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_to_mib.c,v $
 *------------------------------------------------------------------
 * abstraction of VTP MIB registry calls
 *
 * December, 1995 - Chris Young
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_to_mib.c,v $
 * Revision 1.2.58.1  1996/05/09  14:48:42  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:44  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:38  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.2  1996/03/25  02:21:57  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:45:11  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.2  1996/03/22  16:22:19  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:38  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.3  1996/01/31  02:34:09  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/31  00:44:10  cyoung
 * MIB fixes and beginning code for sub-interface creation
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/24  01:38:03  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2  1996/01/02  04:50:10  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 1.1  1995/12/26  19:10:36  cyoung
 * placeholder for VTP MIB support files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "vtp_externs.h"
#include "vtp_interface.h"
#include "vtp_registry.h"
#include "vtp_to_mib.h"

void vtpAPI_revnum_error_detected (unsigned char* domain_name,
				   unsigned char dname_length) {
    reg_invoke_vtp_revnum_error_detected(domain_name, dname_length);
}

void vtpAPI_digest_error_detected (unsigned char* domain_name,
				   unsigned char dname_length) {
    reg_invoke_vtp_digest_error_detected(domain_name, dname_length);
}

void vtpAPI_storage_exceeded (unsigned char* domain_name, unsigned
			      char dname_length) {
    reg_invoke_vtp_storage_exceeded(domain_name, dname_length);
}

void vtpAPI_mtu_too_big (vtp_domain_info* dom_info, vtp_vlan_info*
			 vlan_info, vtp_trunk_info* trunk_info) {
    reg_invoke_vtp_mtu_too_big(dom_info, vlan_info, trunk_info);
}
    
void vtpAPI_report_apply_status (vtp_domain_info* info, long
				 vtpVlanApplyStatus) { 
    reg_invoke_vtp_report_apply_status(info, vtpVlanApplyStatus);
}

