/* $Id: pak_api.h,v 3.3 1995/11/17 09:13:10 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/pak_api.h,v $
 *------------------------------------------------------------------
 * Binary API to the paktype structure
 *
 * April 1995, Steve Berl
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pak_api.h,v $
 * Revision 3.3  1995/11/17  09:13:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:25  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:36:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __PAK_API_H__
#define __PAK_API_H__

#include "types.h"

ushort pak_get_datagramsize(const paktype* pak);
void pak_set_datagramsize(paktype* pak, ushort size);

uchar* pak_get_datagramstart(const paktype* pak);
void pak_set_datagramstart(paktype* pak, uchar* start);

int pak_get_overhead(void);

paktype *getbuffer(int size);
int pak_get_pool_size(paktype *pak);
void datagram_done(paktype *pak);
int get_available_buf_mem (void);
void pool_grow_perm (pooltype *pool, int num);
void appn_init_pool(int *, int *);

#endif __PAK_API_H__
