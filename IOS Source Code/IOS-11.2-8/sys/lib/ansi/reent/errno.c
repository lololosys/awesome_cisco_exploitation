/* $Id: errno.c,v 3.1.10.1 1996/03/18 20:48:50 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/lib/ansi/reent/errno.c,v $
 *------------------------------------------------------------------
 * errno.c -- function used to provide errno
 *
 * January 1996, Mark D. Baushke
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: errno.c,v $
 * Revision 3.1.10.1  1996/03/18  20:48:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.2.1  1996/03/07  08:19:18  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1  1996/03/05  03:15:52  mdb
 * placeholder for DeadKingOnAThrone_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* The errno variable is stored in the reentrancy structure.  This
   function returns its address for use by the macro errno defined in
   errno.h.  */

#include <errno.h>
#include <reent.h>
#include "reent_private.h"

int *
__errno (void)
{
    return &(_REENT->_errno);
}
