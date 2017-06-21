/* $Id: vtp_to_mib.h,v 1.2.58.1 1996/05/09 14:48:43 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_to_mib.h,v $
 *------------------------------------------------------------------
 * API between VTP platform independant code and VTP MIB subsystem
 *
 * December, 1995 - Chris Young
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_to_mib.h,v $
 * Revision 1.2.58.1  1996/05/09  14:48:43  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:39  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.1  1996/03/22  09:45:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.48.1  1996/03/05  05:37:43  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:39  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.5  1996/02/22  21:51:27  cyoung
 * Report apply status of succeeded and now someOtherError where
 * appropriate.  Put back in fix for spurious memory accesses.
 * Minor code cleanup.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.4  1996/01/31  23:15:53  cyoung
 * vtpVlanEditTranslationalVlan1 and 2 now work.  vtpVlanApplyStatus
 * reports succeeded now after inProgress.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.3  1996/01/31  02:34:10  cyoung
 * Provide function to allow variance in managementDomainEntry row
 * creation.
 * Remove redundant consistency checking.  Add hooks to correctly report
 * result of 'apply' operation.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.2  1996/01/31  00:44:11  cyoung
 * MIB fixes and beginning code for sub-interface creation
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/24  01:38:03  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2  1996/01/02  04:50:11  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 1.1  1995/12/26  19:10:37  cyoung
 * placeholder for VTP MIB support files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef VTP_TO_MIB_H
#define VTP_TO_MIB_H

enum { 
    vtp_inProgress = 1, 
    vtp_succeeded = 2,
    vtp_configNumberError = 3,
    vtp_inconsistentEdit = 4,
    vtp_tooBig = 5,
    vtp_localNVStoreFail = 6,
    vtp_remoteNVStoreFail = 7,
    vtp_editBufferEmpty = 8,
    vtp_someOtherError = 9
};

extern void vtpAPI_revnum_error_detected(unsigned char* domain_name,
					 unsigned char dname_length);
extern void vtpAPI_digest_error_detected(unsigned char* domain_name,
					 unsigned char dname_length);
extern void vtpAPI_storage_exceeded(unsigned char* domain_name,
				    unsigned char dname_length);

/* If the MTU for the VLAN is too big for the whole device and not
   just one trunk then the trunk_info parameter should be passed in as
   NULL */
extern void vtpAPI_mtu_too_big(vtp_domain_info* dom_info,
			       vtp_vlan_info* vlan_info,
			       vtp_trunk_info* trunk_info);
extern void vtpAPI_report_apply_status(vtp_domain_info* info, long
				       vtpVlanApplyStatus);
#endif
