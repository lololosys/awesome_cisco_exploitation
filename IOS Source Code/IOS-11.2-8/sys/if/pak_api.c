/* $Id: pak_api.c,v 3.2 1995/11/17 09:30:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/if/pak_api.c,v $
 *------------------------------------------------------------------
 * Binary API to the paktype structure
 *
 * April 1995, Steve Berl
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pak_api.c,v $
 * Revision 3.2  1995/11/17  09:30:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:51:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:54:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "packet.h"
#include "pak_api.h"
#include "../os/buffers.h"

uchar* pak_get_datagramstart(const paktype* pak)
{
    if (pak != NULL) {
	return(pak->datagramstart);
    } else {
	return(NULL);
    }
}

void pak_set_datagramstart(paktype* pak, uchar* start)
{
    if (pak != NULL) {
	pak->datagramstart = start;
    }
}

void pak_set_datagramsize(paktype* pak, ushort size)
{
    if (pak != NULL) {
	pak->datagramsize = size;
    }
}

ushort pak_get_datagramsize(const paktype* pak)
{
    if (pak != NULL) {
	return(pak->datagramsize);
    } else {
	return(0);
    }
}

int pak_get_overhead(void) {
    return(sizeof(paktype) + 
	   ENCAPBYTES + 
	   TRAILBYTES +
	   sizeof(pakdata));
}
