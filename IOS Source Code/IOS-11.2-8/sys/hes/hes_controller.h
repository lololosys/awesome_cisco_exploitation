/* $Id: hes_controller.h,v 3.3 1995/11/17 09:16:27 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/hes/hes_controller.h,v $
 *------------------------------------------------------------------
 * hes_controller.h - Platform specific controller definitions for hes.
 *                 (based on hes/controller.c by Ronnie Kon)
 *
 * Jan 30 1995, Manoj Leelanivas
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: hes_controller.h,v $
 * Revision 3.3  1995/11/17  09:16:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:24  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:39:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:39:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __HES_CONTROLLER_H__
#define __HES_CONTROLLER_H__


#ifdef RSP
typedef slottype platform_hware_handle_type;
#else
typedef mcitype platform_hware_handle_type;
#endif    

extern void load_idb_buffer_info(hwidbtype *, cdbtype *);
extern void default_love_note_handler(cdbtype *, ushort);

cdbtype *controller_find_by_unit(int unit);
cdbtype *controller_create(int, enum pif_types_, int,
			   platform_hware_handle_type *, boolean);

#endif	/* __HES_CONTROLLER_H__ */

	



