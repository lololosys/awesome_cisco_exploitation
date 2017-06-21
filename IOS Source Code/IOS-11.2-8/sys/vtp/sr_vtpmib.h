/* $Id: sr_vtpmib.h,v 1.2.58.3 1996/05/21 22:29:17 cyoung Exp $
 * $Source: /release/112/cvs/Xsys/vtp/sr_vtpmib.h,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * October 1995, Christopher Young (via the SNMP Research MIB Compiler)
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_vtpmib.h,v $
 * Revision 1.2.58.3  1996/05/21  22:29:17  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 1.2.58.2  1996/05/21  10:08:54  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 1.2.58.1  1996/05/09  14:48:23  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.2.84.1  1996/04/27  07:38:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.2.74.1  1996/04/08  02:42:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.2.64.3  1996/04/03  22:36:55  cyoung
 * New objects for TFTP support, ETR vlan type and trunk table creation
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MIB - TFTP support, ETR type allowed, still need work on Trunk creation
 * dep - use temp buf to determine add/mod mode instead of real database
 * core - add tooBig hook for trap generation
 *
 * Revision 1.2.64.2  1996/03/25  02:21:42  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.64.1  1996/03/22  09:44:54  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.2.36.2  1996/03/22  16:22:11  cyoung
 * MIB: allow TR types to be created, check range on newly created vlan
 *      ids, add trap cli commands
 * VTP core: replace REV_LESS macro with function, include notification
 *           for deleted domain
 * VTP dep: clean up after deleted trunks and domains (including startup
 *          time), add some code for vlan modification notification.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.36.1  1996/02/27  21:20:19  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 1.2.4.2  1996/02/22  03:09:59  cyoung
 * Move include files around to make code more easily ported to Cat5000.
 * TruthValue false is 2 and not 0.
 * Roll function declarations from vtp_agent into vtp_interface and
 * vtp_externs (cut down on number of include files).
 * Add IOS_SPECIFIC compile flag to mark non-portable code.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2.4.1  1996/01/24  01:37:49  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 1.2  1996/01/02  04:50:05  shankar
 * Branch: FR_SVC_branch
 * Sync to mainline
 *
 * Revision 1.1  1995/12/26  19:10:30  cyoung
 * placeholder for VTP MIB support files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

#ifndef _vtpmib_h_
#define _vtpmib_h_

/* standard header files, should be platform independant */
#include "vtp_externs.h"
#include "vtp_interface.h"
#include "vtp_pub.h"

/* platforms that do not support the IOS should remove
   the following line to avoid including the IOS specific header
   files. Equivalent functionality will have to be included instead */

#define IOS_SPECIFIC
#ifdef IOS_SPECIFIC
#include "../ui/common_strings.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"
#include "registry.h"

#include "../parser/parser_defs_snmp.h" /* TRAP_VTP */
#include "../parser/parser_actions.h"
#include "../parser/macros.h"  /* trap cli commands */
#include "../parser/parser_defs_parser.h"
#include "../parser/parser_defs_exec.h"

#include "../snmp/sr_tmq.h" /* TimeOutDescriptor in vtp_editbuf.h */
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.regh"

#include "../util/avl.h" /* avl_node_type in vtp_editbuf.h */

#include "vtp_dep.h" 

#endif IOS_SPECIFIC

/* standard header files, should be platform independant */
#include "sr_vtpmibdefs.h"
#include "sr_vtpmibsupp.h"
#include "sr_vtpmibtype.h"
#include "vtp_editbuf.h"

#ifdef IOS_SPECIFIC
#include "sr_vtpmibpart.h"
#endif IOS_SPECIFIC

#define VTP_SAID_LENGTH 4

#define VTP_INITIAL_REVISION_NUMBER 1
#define NO_VLANS 0
#define VTP_TLV_UNDEFINED 5000 /* a number larger than any valid in
				  the objects' range */

#define VTP_TRAP_INTERVAL 30000 /* hundreths of seconds */

#define D_vtpConfigRevNumberError 1
#define D_vtpConfigDigestError 2
#define D_vtpServerDisabled 3
#define D_vtpMtuTooBig 4

extern unsigned char k_get_vtp_domain(vtp_domain_info*, unsigned char);

extern long vtp_dep_get_notifications_enabled(void);

extern void vtp_dep_notification_enabled(long new_value);

extern vtp_boolean vtp_dep_validate_managementDomainRowStatus(long value);

extern vtp_boolean vtp_dep_validate_vlanTrunkPortRowStatus(long value);

extern long vtp_dep_validate_vlanTrunkPortEncaps(long ifIndex, 
						 long encaps);
extern void  vtp_dep_add_trap_functions(void);

extern void* 
vtp_dep_get_trunk_from_ifIndex(long vlanTrunkPortIfIndex);

extern long  vtp_dep_get_ifIndex_from_trunk(void* trunk_id);

extern unsigned long vtp_dep_get_local_ipaddr(void);

#ifdef IOS_SPECIFIC
extern const ObjectInfo vtpmib_OidList[];
extern const uint       vtpmib_OidListNum;
#endif IOS_SPECIFIC

#endif  /* _vtpmib_h_ */
      
