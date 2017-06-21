/* $Id: termcapglo.c,v 3.2.60.1 1996/03/18 22:18:31 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/termcap/termcapglo.c,v $
 *------------------------------------------------------------------
 * TERMCAPGLO.C - routines to allocate and free termcap globals structure.
 * 
 * June 1991, Robert Snyder
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: termcapglo.c,v $
 * Revision 3.2.60.1  1996/03/18  22:18:31  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.24.2  1996/03/07  10:53:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.24.1  1996/02/20  21:21:54  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  18:50:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:36:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "ttysrv.h"
#include "interface_private.h"
#include "packet.h"
#include "connect.h"
#include "termcapglo.h"
#include "termcap_public.h"

termcap_globals_struct *memallocTermcap()
{
     termcap_globals_struct *temp;

     if ((temp = malloc(sizeof(termcap_globals_struct))) != NULL) {
         memset(temp, 0, sizeof(termcap_globals_struct));
     }
     return(temp);
}

void freeTermcap(tg)
termcap_globals_struct *tg;
{
    if (tg) {
        free(tg);
    }
}
