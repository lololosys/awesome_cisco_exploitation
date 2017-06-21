/* $Id: latpad.h,v 3.2 1995/11/17 18:59:09 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/latpad.h,v $
 *------------------------------------------------------------------
 * LAT/PAD conversion specific definitions and prototypes
 *
 * December 1993, Eman Schaffer, Scott Mackie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: latpad.h,v $
 * Revision 3.2  1995/11/17  18:59:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:34:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Prototypes
 */

/*
 * padlat.c
 */

int x25_transto_lat(translatetype *trans, tt_soc *tty);

/*
 * latpad.c
 */
void pt_latpad_init(subsystype *subsys);
void latpadhic(translatetype *trans);
int lat_transto_x25(translatetype *trans, tt_soc *tty);




