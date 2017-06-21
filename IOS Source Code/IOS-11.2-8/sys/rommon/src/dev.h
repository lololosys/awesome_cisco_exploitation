/* $Id: dev.h,v 3.2 1995/11/17 18:41:29 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/dev.h,v $
 *------------------------------------------------------------------
 * dev.h
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994 - 1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the device concept.
 *------------------------------------------------------------------
 * $Log: dev.h,v $
 * Revision 3.2  1995/11/17  18:41:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:36:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DEV_H__
#define __DEV_H__
/*
** Defines for the device concept
*/

struct devstruct {
    char *devid;            /* device identifier  (i.e. flash:) */
    char *devname;          /* the device name (i.e. flash) */
    char *devstart;         /* physical starting address of this resource */
    int devsize;            /* size of this device in bytes */
    char *devend;           /* the physical address of the end of this device */
    unsigned flags;         /* see defines below */
    int (*devsizroutine)(); /* routine to call to size the device */
    int devfap;             /* file access protocol (for bootinfo) */
};

/* defines for above flags field */
#define DEV_IS_SIZED      0x00000001
#define DEV_LIB_LOADED    0x00000002
#define DEV_FS_CAPABLE    0x00000004
#define DEV_IS_MEMORY     0x00000008
#define DEV_IS_BITSWAPPED 0x80000000

/* define the character used to separate the device specifier from a filename */
#define DEVSEPARATOR ':'

/* device id's - must contain device separator character */
#define DEV_KMEMU_ID     "mkuseg:"
#define DEV_KMEM0_ID     "mkseg0:"
#define DEV_KMEM1_ID     "mkseg1:"
#define DEV_MEM_ID       "mem:"
#define DEV_NVRAM_ID     "nvram:"
#define DEV_EPROM_ID     "eprom:"
#define DEV_FLASH_ID     "flash:"
#define DEV_BOOTFLASH_ID "bootflash:"
#define DEV_NETWORK_ID   "net:"
#define DEV_PCMCIA1_ID   "p1:"
#define DEV_SLOT0        "slot0:"
#define DEV_SLOT1        "slot1:"

extern struct devstruct devtblinit[];
extern struct devstruct devtbl[];
extern struct devstruct *memdevp;
extern int numdevstructs;
extern char mon_main_mem_id[];
extern char default_bootdev_id[];
extern char default_boothelper_id[];

/* related prototypes */
extern int isdevname(char *name);
extern int getdevnum(char *name);
extern void clr_dev_lib();
extern void initdevtbl();
extern int memdevnum;
extern unsigned long phymemmask;
extern int devs(int argc, char *argv[]);

#endif  /* __DEV_H__ */
