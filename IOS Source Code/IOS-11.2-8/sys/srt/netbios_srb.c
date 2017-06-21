/* $Id: netbios_srb.c,v 3.1.2.2 1996/07/09 06:05:24 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/srt/netbios_srb.c,v $
 *------------------------------------------------------------------
 * netbios_srb.c  implementation of netbios_acl <-> srb glue
 *
 * April 1996, Gordon Cox
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: netbios_srb.c,v $
 * Revision 3.1.2.2  1996/07/09  06:05:24  ppearce
 * CSCdi59527: inlines waste code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.1.2.1  1996/05/17  12:13:03  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.2  1996/05/02  20:04:47  gcox
 * Branch: IbuMod_Calif_branch
 * IBU Modularity - add ibu_int subsystem
 *
 * Revision 3.1.4.1  1996/04/26  07:58:15  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1  1996/04/26  05:51:49  dstine
 * Placeholders for IbuMob_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_generic.h"
#include "subsys.h"
#include "netbios_srb.h"

/*
 * Here we define the function ptrs declared in the .h and
 * give them sensible initial values, so they can always be called
 */

static void tr_set_access_flags_default ( struct hwidbtype_ *hwidb)
{
  hwidb->srb_access_in = (*netbios_sb_ibm_iacf_is_enabled_p) (hwidb->firstsw);
  hwidb->srb_access_out= (*netbios_sb_ibm_oacf_is_enabled_p) (hwidb->firstsw);
}

static boolean default_idb_srb_access( struct hwidbtype_ *hwidb)
{
  return (0);
}

void (*tr_set_access_flags_p) ( struct hwidbtype_ *hwidb)
                             = tr_set_access_flags_default;
boolean (*srb_access_in_p) (struct hwidbtype_ *hwidb)
                             = default_idb_srb_access;
boolean (*srb_access_out_p) (struct hwidbtype_ *hwidb)
                             = default_idb_srb_access;



static boolean default_acf_enabled ( struct idbtype_ *swidb)
{
  return (FALSE);
}

boolean (*netbios_sb_ibm_iacf_is_enabled_p) (struct idbtype_ *swidb)
                             = default_acf_enabled;

boolean (*netbios_sb_ibm_oacf_is_enabled_p) (struct idbtype_ *swidb)
                             = default_acf_enabled;

/* 
 * This subsystem exists only for the packaging system's view of 
 * the req: dependencies; so that it will be pulled into the right images
 * It contains decoupled interfaces between ibu components
 */

static void ibu_int_init (subsystype *subsys)
{
    ;
}

#define IBU_INT_MAJVER 1
#define IBU_INT_MINVER 0
#define IBU_INT_EDITVER 1

SUBSYS_HEADER(ibu_int, IBU_INT_MAJVER, IBU_INT_MINVER, IBU_INT_EDITVER,
	      ibu_int_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);
