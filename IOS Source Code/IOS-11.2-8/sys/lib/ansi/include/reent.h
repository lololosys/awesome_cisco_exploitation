/* $Id: reent.h,v 3.1.10.1 1996/03/18 20:48:42 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/include/reent.h,v $
 *------------------------------------------------------------------
 * reent.h -- This header file provides the reentrancy structure.
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reent.h,v $
 * Revision 3.1.10.1  1996/03/18  20:48:42  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:16  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:04:08  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __REENT_H__
#define __REENT_H__

void *_reent_init(void);	/* malloc and initialize a reent */
void  _reent_cleanup(void *);	/* cleanup and free a reent */

#endif /* __REENT_H__ */
