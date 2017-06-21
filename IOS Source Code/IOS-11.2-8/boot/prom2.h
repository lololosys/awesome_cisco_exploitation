/* $Id: prom2.h,v 3.2 1995/11/17 07:41:37 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/prom2.h,v $
 *------------------------------------------------------------------
 * prom2.h -- definitions for linkage to upper ROM software
 *
 * August 1986, Greg Satz
 *
 * Copyright (c) 1986-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: prom2.h,v $
 * Revision 3.2  1995/11/17  07:41:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Boot file name formats:
 */
#define CISCO_FORMAT		"cisco%o-%s"
#define OEM_FORMAT		"system%o-%s"
#define SYNOPTICS_IGS_FORMAT	"rt338%o.img"

#define	PROM2_MAGIC1	0x4E56		/* link a6,... */
#define	PROM2_MAGIC2	0xC0BE		/* compressed structure */
#define	PROM2_MAGIC3	0xABCD		/* uncompressed structure */

#define PROM3_MAGIC	0xDEAD		/* bootstrap */

#define RELOC_MAGIC     0xB0EE          /* magic for reloc header */

#define	PROM2_VERSION	3		/* request prom2 version */
#define	PROM2_BOOTLOAD	2		/* request boot loading */
#define PROM2_BOOTDEBUG 4		/* Boot and enter debug mode */
#define PROM2_DEBUG	5		/* Set up debugger and return */

/*
 * Compressed data structure
 */

struct cload {
	u_long addr;			/* absolute physical address */
	u_long len;			/* length of data */
};

struct cdata {
	u_short magic;			/* compressed data magic number */
	short number;			/* number of load structures */
	u_short cksum;			/* cksum of entire data */
};
 
struct bootheader {
    	u_short magic;			/* Magic number */
	short reserved;			/* Reserved */
	u_long size;			/* Size of the image that follows */
    };

/*
 * Header for relocating system code load address for XX
 * See code in init.c in the XX subdirectory for use
 */
struct relochdr {
    u_short magic;           /* RELOC_MAGIC above */
    short size;              /* size of this structure (for future expansion) */
    u_long loadaddr;         /* new load point for system code (was 0x1000) */
    u_long reserved[6];      /* pad out to 8 longs for future expansion */
};

/*
 * process load block now defined in plb.h
 */
typedef ulong ipaddrtype;	/* for plb struct */

#include "plb.h"

/*
 * We only use the bootload functions
 */

#define	ram_version()		ram_dispatch(PROM2_VERSION, 0)
#define	ram_bootload(name)	ram_dispatch(PROM2_BOOTLOAD, name)
#define	prom2_version()		prom2_dispatch(PROM2_VERSION, 0)
#define	prom2_bootload(name)	prom2_dispatch(PROM2_BOOTLOAD, name)

extern char *makebfile();
extern int ram_dispatch();
extern int prom2_dispatch();
