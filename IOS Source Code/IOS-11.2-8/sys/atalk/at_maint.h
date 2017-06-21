/* $Id: at_maint.h,v 3.2.60.1 1996/03/23 01:26:59 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/at_maint.h,v $
 *------------------------------------------------------------------
 * at_maint.h -- definitions for Appletalk maintenance module
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_maint.h,v $
 * Revision 3.2.60.1  1996/03/23  01:26:59  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:43:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:08:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_MAINT_H__
#define __AT_MAINT_H__


/*
 * AppleTalk private prototypes
 */
char * 		atmaint_PeerType(neighborentry *);
void 		atalkif_NetAcquired(atalkidbtype*);
void		atalk_start_aging(idbtype *, paktype *);
neighborentry * maint_GetFirstNeighbor(void);
neighborentry *	maint_GetNextNeighbor(neighborentry*);

#endif __AT_MAINT_H__
