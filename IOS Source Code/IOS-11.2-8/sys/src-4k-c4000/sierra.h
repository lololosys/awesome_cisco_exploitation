/* $Id: sierra.h,v 3.3 1995/11/17 18:40:56 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-4k-c4000/sierra.h,v $
 *------------------------------------------------------------------
 * Header file for Sierra specific routine
 *
 * Oct 1993, Lele Nardin
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: sierra.h,v $
 * Revision 3.3  1995/11/17  18:40:56  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:30:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:22:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:53:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
 * Shared externals:
 */
extern cookie_t *cookie;	/* Address of cookie PROM */

/*
 * Prototypes
 */
void 	check_for_sierra_overtemp(void);
void 	set_nevada_regs(void);
void 	test_leds(parseinfo *);
boolean platform_is_c4700(void);
boolean platform_is_c4500(void);

