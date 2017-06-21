/* $Id: compress.h,v 3.3 1995/11/21 20:44:37 turadek Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/x25/compress.h,v $
 *------------------------------------------------------------------
 * COMPRESS.H
 *
 * July 29, 1993, Bill Thomas
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: compress.h,v $
 * Revision 3.3  1995/11/21  20:44:37  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.2  1995/11/17  18:05:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:52:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __COMPRESS_H__
#define __COMPRESS_H__

#include "../wan/comp_sys.h"

/* defined here */
extern paktype *rand_compress (hwidbtype *, paktype *);
extern paktype *stac_compress (hwidbtype *, paktype *);
extern paktype *default_compress (hwidbtype *, paktype *);
extern paktype *rand_decompress (hwidbtype *, paktype *);
extern paktype *stac_decompress (hwidbtype *, paktype *);
extern paktype *default_decompress (hwidbtype *, paktype *);
extern void frame_relay_setup_encaps_info (hwidbtype *hwidb, paktype *pak);
extern void lapb_setup_encaps_info (hwidbtype *hwidb, paktype *pak);

/* defined elsewhere */

#endif __COMPRESS_H__
