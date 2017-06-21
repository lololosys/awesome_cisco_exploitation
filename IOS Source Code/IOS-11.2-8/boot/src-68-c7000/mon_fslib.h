/* $Id: mon_fslib.h,v 3.1 1995/11/09 09:10:20 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/mon_fslib.h,v $
 *------------------------------------------------------------------
 * MONLIB file system vector table MACRO definition
 *
 * November 1994, Szewei Ju
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mon_fslib.h,v $
 * Revision 3.1  1995/11/09  09:10:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/06/07 09:05:12  smackie
 * Fold back Arizona_branch into mainline.
 *
 * Revision 1.1.4.1  1995/05/02  23:22:33  sthormod
 * Branch: Arizona_branch
 * Rom monitor support for RP1 flash credit card
 *
 * Revision 1.1  1995/04/23  00:22:44  sthormod
 * Placeholders for Arizona branch.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern struct monlib_iface ml_iface;

#define FS_init		(*ml_iface.init)
#define FS_open		(*ml_iface.open)
#define FS_close	(*ml_iface.close)
#define FS_read		(*ml_iface.read)
#define FS_write	(*ml_iface.write)
#define FS_lseek	(*ml_iface.lseek)
#define FS_delete	(*ml_iface.delete)
#define FS_undelete	(*ml_iface.undelete)
#define FS_squeeze	(*ml_iface.squeeze)
#define FS_stat		(*ml_iface.stat)
#define FS_fstat	(*ml_iface.fstat)
#define FS_istat	(*ml_iface.istat)
#define FS_fsstat	(*ml_iface.fsstat)
#define FS_getdents	(*ml_iface.getdents)

