/* $Id: arap_v42bis.h,v 3.2 1995/11/17 08:39:01 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/arap/arap_v42bis.h,v $
 *------------------------------------------------------------------
 * v42bis.h
 *
 * October 1993, Tim Kolar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: arap_v42bis.h,v $
 * Revision 3.2  1995/11/17  08:39:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:53:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:05:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef V42BIS_H
#define V42BIS_H

#include "arap_fcrbis.h"

typedef struct v42struct {
    v42bis_connection_t *comp;
    paktype *bufpak;
    long input_inchar;
    long input_outchar;
    long output_inchar;
    long output_outchar;
} v42type;

v42type *init_v42(void);
void destroy_v42(tt_soc *);
int v42_input(tt_soc *, queuetype *, queuetype *);
int v42_output(tt_soc *, queuetype *, queuetype *);

extern v42type *get_tty_v42ptr(tt_soc *);

#endif	/* V42BIS_H */
