/* $Id: vtp_registry.c,v 1.1.16.1 1996/05/09 14:48:39 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/vtp/vtp_registry.c,v $
 *------------------------------------------------------------------
 * vtp_registry.c
 *
 * February 1996, Gee Akyol
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vtp_registry.c,v $
 * Revision 1.1.16.1  1996/05/09  14:48:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 1.1.54.1  1996/04/27  07:38:42  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 1.1.42.1  1996/04/08  02:42:34  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 1.1.30.1  1996/03/22  09:45:09  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 1.1.10.1  1996/03/08  01:47:06  rlowe
 * This file was new with LE_Cal_V111_1_0_3_branch, and did not have
 * a V111_1_0_3 trunk tag.  Thus it was missed on the previous LANE
 * merge step into this branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 1.1.2.1  1996/02/27  21:20:35  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "registry.h"
#include "interface_private.h"
#include "address.h"
#include "../if/atm.h"
#include "vtp_externs.h"
#include "vtp_interface.h"
#include "vtp_registry.h"
#include "../vtp/vtp_registry.regc"

/*
 * vtp_registry_init
 *
 * Initialize VTP Registry
 */
 
void vtp_registry_init (subsystype *subsys)
{
    registry_create(REG_VTP, SERVICE_VTP_MAX, "LAN Emulation");
    create_registry_vtp();
}
 
/*
 * VTP Registry subsystem header
 */
 
#define VTP_REGISTRY_MAJVERSION		1
#define VTP_REGISTRY_MINVERSION 	0
#define VTP_REGISTRY_EDITVERSION  	1
 
SUBSYS_HEADER(vtp_registry,
              VTP_REGISTRY_MAJVERSION, VTP_REGISTRY_MINVERSION,
              VTP_REGISTRY_EDITVERSION,
              vtp_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
