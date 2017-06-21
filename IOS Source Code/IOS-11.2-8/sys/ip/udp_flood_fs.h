/* $Id: udp_flood_fs.h,v 3.1.8.1 1996/07/08 22:34:28 mwu Exp $
 * $Source: /release/112/cvs/Xsys/ip/udp_flood_fs.h,v $
 *------------------------------------------------------------------
 * support for udp flood fastswitching
 *
 * December 1995, Bob O'Grady
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: udp_flood_fs.h,v $
 * Revision 3.1.8.1  1996/07/08  22:34:28  mwu
 * CSCdi60922:  11.2 Core Features Commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/03/12  11:10:04  ogrady
 * Placeholder
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void platform_fs_pak_data_copy(paktype *srcpak, paktype *dstpak,
				      uchar *srcptr, uchar *dstptr, int size);
extern boolean udp_flood_fs(idbtype *swinput, paktype *pak);
