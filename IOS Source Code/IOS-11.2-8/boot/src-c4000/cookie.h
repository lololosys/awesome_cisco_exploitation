/* $Id: cookie.h,v 3.3.50.1 1996/04/05 02:42:02 yoko Exp $
 * $Source: /release/112/cvs/Xboot/src-c4000/cookie.h,v $
 *------------------------------------------------------------------
 * cookie.h -- C4000 Cookie definition
 *
 * September 4, 1990, Chris Shaker
 * November 1993, Lele Nardin
 *
 * Copyright (c) 1992-1996,97 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cookie.h,v $
 * Revision 3.3.50.1  1996/04/05  02:42:02  yoko
 * CSCdi53681:  add new cpu types
 * Branch: California_branch
 * define new cpu types and emt calls
 *
 * Revision 3.3  1995/12/03  17:55:33  thille
 * CSCdi44905:  ciscopro targets not in 11.1
 * Put them in.
 *
 * Revision 3.2  1995/11/17  07:42:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:10:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __COOKIE_H__
#define __COOKIE_H__

typedef struct cookie cookie_t;
struct cookie {							/* byte #  */
    uchar interfaces;		/* Which interfaces does it have?      0   */
    uchar vendor;		/* Which vendors version of the IGS    1   */
    uchar ether0_hw_addr[6];	/* Increment for ethernet 1           2-7  */
    uchar processor;		/* Which processor is this?           8    */
    uchar new_xx;               /* 1=4000-M 0=old XX                   9   */	
    uchar UNUSED_1[6];          /* Unused (padding)                  10-15 */
    uchar serial[4];		/* BCD packed 8 digit serial number  16-19 */
    uchar UNUSED_2[7];          /* Unused                            20-26 */
    uchar magic[2];             /* Magic number for old cookies      27-28 */
    uchar capabilities[2];	/* capabilities (future stuff)       29-30 */
    uchar version;		/* cookie version numba              31    */
};

extern struct cookie *get_cookie(void);

/*
 * Vendor codes: Now defined in cpu.h
 */


/*
 * Interface codes:
 */
#define INTERFACE_1E1T		1
#define INTERFACE_2E		2
#define INTERFACE_1R1T		3
#define INTERFACE_1R1E		4
#define INTERFACE_2R		5
#define INTERFACE_1E1B		6
#define INTERFACE_1E2T		7
#define INTERFACE_1R2T		8
#define INTERFACE_1E1T1B	9
#define INTERFACE_1R1T1B	10
#define INTERFACE_1E2T1B	11
#define INTERFACE_1R2T1B	12
#define INTERFACE_1R1B		13
#define INTERFACE_1E2T8A	14
#define INTERFACE_1R2T8A	15

/*
 * Processor codes:
 * When adding a new processor code, please make sure to also add it
 * to boot/src-68-c3000/cookie.h
 */
#define PROCESSOR_PAN		0 /* IGS processor */
#define PROCESSOR_MERLOT	1 /* Token ring IGS processor */
#define PROCESSOR_XX		2 /* DosXX */
#define PROCESSOR_BRUT          3
#define PROCESSOR_BASS          4
#define PROCESSOR_CRISTAL       5
#define PROCESSOR_CANCUN        6
#define PROCESSOR_SIERRA	7
#define PROCESSOR_SAPPHIRE      8
#define PROCESSOR_VOLCANO       9
#define PROCESSOR_C3500         10
#define PROCESSOR_JANEIRO	11
#define PROCESSOR_MAX		12
#define CISCOPRO_FLAG		0x80 /* Bit is set if platform is branded
				        for CiscoPro images only */

/*
 * capability bits
 */
#define CAP_PAN_BRIDGE	0x0001		/* SYSCAP_1_BRIDGE */
#define CAP_PAN_X25	0x0002		/* SYSCAP_1_DDN | SYSCAP_1_X25 */
#define CAP_PAN_LAT	0x0004		/* SYSCAP_1_LAT */
#define CAP_PAN_PROTO	0x0008		/* SYSCAP_1_PROTO */

/*
 * Magic numbers necessary to honor capability[1] bits
 */
#define COOKIE_MAGIC_0          0xDE
#define COOKIE_MAGIC_1          0xAD

/*
 * cookie->capabilities[1] bits
 * Features are encoded bitwise in cababilites,
 * In decoding, 0 = include feature; 1 exclude feature:
 *      Bit             Feature
 *      ---             -------
 *       0              Unused
 *       1              Unused
 *       2              Unused
 *       3              Unused
 *       4              Unused
 *       5              Unused
 *       6              Unused
 *       7              CiscoPro Platform
 */
#define CISCOPRO_PLATFORM	0x80 /* Platform branded for CiscoPro images only */

/*
 * Kludge here:  CiscoPro boxes come with the PROCESSOR_CISCOPRO bit set in 
 *	cookie.processor and the CISCOPRO_PLATFORM bit set in
 *	cookie.capabilities[1].  The PROCESSOR_CISCOPRO bit disallows pre-
 *	CiscoPro images from running on CiscoPro boxes, even if old ROMs
 *	are installed.  This bit is turned off by the ROM monitor right
 *	after reading the cookie, so all other ROM monitor code will run
 *	as always (only post-CiscoPro ROM monitors do this).  The ciscopro_
 *	flag allows the CPU_CISCOPRO bit to be turned on appropriately
 *	in the CPU type returned from the EMT_PROCTYPE trap.  This is
 *	intended to confuse old system images that have no business running
 *	on a CiscoPro platform (ie. unknown CPU type).  Newer system images
 *	mask the CPU_CISCOPRO bit back off, but only if the image is branded
 *	for use on a CiscoPro platform (ie. CISCOPRO_IMAGE is defined).
 *	(whew!!, but it works).
 */
#endif __COOKIE_H__
