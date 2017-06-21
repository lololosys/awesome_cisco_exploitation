/* $Id: netbios_srb.h,v 3.1.2.1 1996/05/17 12:13:05 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/netbios_srb.h,v $
 *------------------------------------------------------------------
 * netbios_srb.h  Decoupled glue between netbios_acl and srb
 *
 * April 1996, Gordon Cox
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_srb.h,v $
 * Revision 3.1.2.1  1996/05/17  12:13:05  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.1  1996/04/26  07:58:16  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1  1996/04/26  05:51:50  dstine
 * Placeholders for IbuMob_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__NETBIOS_SRB_H__
#define	__NETBIOS_SRB_H__

#include "types.h"   /* just for boolean, FALSE */

/* 
 * Here we declare just a few function pointers 
 * These are initialised by the linker, so can always be called
 * They may be re-initialised by subsystem init's, which are all on 
 *   one thread, and before any interrupt threads ( and hence 
 *   asynchronous calls) can occur, so no int-disable reqd
 *
 * The decoupled nature of this means that support is provided for 
 * either or both of netbios_acl and srb in any image.
 *
 * This file does not describe the whole interface; the plan is to 
 * gather all aspects of this interface here, and then understand 
 * how to remove the interface. 
 */

/* 
 * Following is 
 *    set by srb_sb.c from srb subsys init if srb present;
 *    called by 
 *        ibm/netbios_acl.c
 *        ibm/netbios_sb.c
 */
extern void (*tr_set_access_flags_p) (struct hwidbtype_ *hwidb);
extern boolean (*srb_access_in_p) (struct hwidbtype_ *hwidb);
extern boolean (*srb_access_out_p) (struct hwidbtype_ *hwidb);

/* 
 * Following are
 *    set by netbios_acl.c subsys init if netbios_acl present;
 *    called by 
 *        srt/srb_sb.c ( in tr_set_access_flags )
 *        hes/srb_sse.c
 */
extern boolean (*netbios_sb_ibm_iacf_is_enabled_p) (struct idbtype_ *swidb);
extern boolean (*netbios_sb_ibm_oacf_is_enabled_p) (struct idbtype_ *swidb);

#endif	__NETBIOS_SRB_H__
