/* $Id: rsp_macaddr.h,v 3.2 1995/11/17 18:48:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_macaddr.h,v $
 *------------------------------------------------------------------
 * Definitions for backplane NVRAM (DS1201) containing MAC addresses
 * and saved environmental measurements.
 *
 * January 1995, Carl Schaefer
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_macaddr.h,v $
 * Revision 3.2  1995/11/17  18:48:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:58:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {
    TEST_RSP_DS1201_READ = 1,
    TEST_RSP_DS1201_WRITE,
};

extern void macaddr_init(void);
extern void test_rsp_ds1201(parseinfo *csb);

extern uchar ds1201_read_byte(uchar addr);
extern void ds1201_write_byte(uchar addr, uchar value);

/*
 * The NVRAM is DS1201_BYTES in size.
 * The region up to and including DS1201_MFG_TOP is reserved for
 * manufacturing (but is "protected" by software only).
 * The rest is available for any system software use.
 * The chassis interface code is allocated the region between
 * DS1201_CI and DS1201_TOP.
 */
#define DS1201_BYTES	128
#define DS1201_MFG_TOP	63
#define DS1201_CI	64
#define DS1201_CI_TOP	95

#define NO_CHASSIS_MODEL	((uchar) -1)

extern uchar chassis_model;
extern uint chassis_serial;
