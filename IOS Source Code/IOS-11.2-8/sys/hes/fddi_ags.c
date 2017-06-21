/* $Id: fddi_ags.c,v 3.3.60.1 1996/03/18 19:40:30 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/fddi_ags.c,v $
 *------------------------------------------------------------------
 * fddi_ags.c -- fddi routines specific to ags
 *
 * june, 1992       steve elias
 * September, 1989  Robert Fletcher
 *
 * Copyright (c) 1986-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fddi_ags.c,v $
 * Revision 3.3.60.1  1996/03/18  19:40:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/01/24  21:45:29  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  09:16:19  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:26:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:38:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:39:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "linktype.h"
#include "interface.h"
#include "../if/fddi.h"
#include "../if/fddi_private.h"

/*
 * hes_get_phy
 * Platform specific routine to get the appropriate fddi phy structure
 * for this interface
 */

phytype *hes_get_phy (hwidbtype *hwidb, int index)
{
    return(malloc(sizeof(phytype)));
}
