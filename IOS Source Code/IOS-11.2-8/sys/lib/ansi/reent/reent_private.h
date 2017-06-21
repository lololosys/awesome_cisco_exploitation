/* $Id: reent_private.h,v 3.1.10.1 1996/03/18 20:48:53 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/reent/reent_private.h,v $
 *------------------------------------------------------------------
 * reent_private.h -- This header file provides the reentrancy structure.
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: reent_private.h,v $
 * Revision 3.1.10.1  1996/03/18  20:48:53  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:21  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:15:54  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __REENT_PRIVATE_H__
#define __REENT_PRIVATE_H__

typedef struct _reent_ {
    struct _reent_ *_rp;        /* chained list of reent blocks */
    int _errno;                 /* local copy of errno */
    char *_scanpoint;           /* used by strtok */
    long _next;                 /* used by rand/srand */
    char _asctime[26];          /* used by asctime */
} _reent_t;

void *stack_reent_ptr(unsigned int i);
unsigned int onintstack(void);

#define _REENT ((_reent_t *) stack_reent_ptr(onintstack()))

#endif /* __REENT_PRIVATE_H__ */
