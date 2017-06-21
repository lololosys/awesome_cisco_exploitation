/* $Id: les_controller.h,v 3.3.12.1 1996/06/16 21:16:09 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/les/les_controller.h,v $
 *------------------------------------------------------------------
 * les_controller.h - controller specific defines for low-end.
 *
 * Nov 27 1994, Manoj Leelanivas
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: les_controller.h,v $
 * Revision 3.3.12.1  1996/06/16  21:16:09  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.3  1996/02/13  08:14:56  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.2  1995/11/17  17:41:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:33:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/17  00:54:51  jliu
 * CSCdi35899:  Controller didnt come back up after shut/no shut
 *
 * Revision 2.1  1995/06/07  21:38:04  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LES_CONTROLLER_H__
#define __LES_CONTROLLER_H__

extern cdbtype *controller_create(int, enum pif_types_, int);
extern void controller_state_change(cdbtype  *, enum IDB_STATES);


#endif	/* __LES_CONTROLLER_H__ */


