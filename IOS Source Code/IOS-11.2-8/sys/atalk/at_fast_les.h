/* $Id: at_fast_les.h,v 3.2.62.1 1996/05/02 08:39:38 dwong Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_fast_les.h,v $
 *------------------------------------------------------------------
 * SMRP AppleTalk fast switching definitions
 *
 * August 1995, Kent Leung
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_fast_les.h,v $
 * Revision 3.2.62.1  1996/05/02  08:39:38  dwong
 * Branch: California_branch
 * Commit SMRP-7kFastSwitch, Load Balancing, and AURP Modularity
 *
 * Revision 3.2  1995/11/17  08:42:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/05  02:31:32  kleung
 * Placeholder.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_FAST_LES_H__
#define __AT_FAST_LES_H__

#define FAST_SERIAL
#undef FAST_OLD_SERIAL


/*
 * Forward declarations.
 */
extern inline void at_write_ether_header(ulong*, atcachetype*, ushort, uchar);
extern inline void at_write_etalk_header(ulong*, atcachetype*, uchar);
extern inline void at_write_fddi_header(ulong*, atcachetype*, uchar);
extern inline void at_write_serial_header(ulong*, atcachetype*, uchar);

#endif __AT_FAST_LES_H__

