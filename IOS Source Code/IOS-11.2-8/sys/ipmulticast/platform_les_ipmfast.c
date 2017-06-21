/* $Id:$
 * $Source:$
 *------------------------------------------------------------------
 * Routines used by all low-end platforms (except RSP) for
 * ipmfast support.
 *
 * November 1996, Don Lobete
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <ciscolib.h> 
#include "master.h"
#include "interface_private.h"
#include "packet.h"
 
paktype *platform_fastsend_getbuffer (int size)
{
    return(getbuffer(size));
}
 
void platform_fs_pak_data_copy (paktype *srcpak,
                                paktype *dstpak,
                                uchar *srcptr,
                                uchar *dstptr,
                                int size)
{
    pak_set_dirty(srcpak);
    pak_set_dirty(dstpak);
    bcopy(srcpak->network_start, dstpak->network_start, size);
}

