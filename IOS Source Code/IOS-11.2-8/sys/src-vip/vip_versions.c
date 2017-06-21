/* $Id: vip_versions.c,v 3.2.62.1 1996/03/18 22:03:54 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/src-vip/vip_versions.c,v $
 *------------------------------------------------------------------
 * vip_versions.c : VIP RSP support routines (rvip/svip independent)
 *
 * June 1995, David Getchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_versions.c,v $
 * Revision 3.2.62.1  1996/03/18  22:03:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  10:46:00  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  18:04:56  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:52:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:30:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:50  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/19  22:01:14  getchell
 * Placeholder file for vip development
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
 

#include "master.h"

/*
 * init_vip_versions:
 */
ushort vip_sw_version (void)
{
    /*
     * Setup the sw/hw version numbers
     */
    return(VIP_SW_VERSION);
}

/*
 * The following two routines will be moved once we have rom monitor support
 *  for them 
 */

ushort vip_vpld_version (void)
{
    /* FIXME - once we have rom monitor support we need to do a call */
    return(VIP_VPLD_VERSION);
}

ushort vip_rom_version (void)
{
    return(0xffff);
}
/* end of file */
