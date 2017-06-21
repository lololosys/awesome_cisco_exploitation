/* $Id: plb.h,v 3.2 1995/11/17 07:41:33 hampton Exp $
 * $Source: /swtal/cherf.111/ios/boot/plb.h,v $
 *------------------------------------------------------------------
 * plb structure file
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: plb.h,v $
 * Revision 3.2  1995/11/17  07:41:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:12:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__PLB_H__)
#define __PLB_H__
/*
 * process load block constructed by rom monitor
 */
typedef struct plb_t {
    char *plb_str;		/* file to boot or error message */
    uchar *plb_addr;		/* address of loaded image */
    long plb_size;		/* size of loaded image */
    ipaddrtype plb_ipaddr;	/* address to boot image from */
    long plb_magic;		/* magic number for backward compatiblity */
    long plb_option;		/* options below */
    long plb_result;		/* result of load operation */
} plbtype;

/*
 * plb_magic is used so that we can confirm from a rxboot image whether 
 * or not the RXBOOT supports flh.  An image capable of running flh will
 * look for the magic number as part of quailfying the plb request.
 */

#define PLBMAGIC 0xDAD2BABE

/*
 * plb_option
 */

#define BOOTERASEFLASH		0x01
#define BOOTCOPYTFTPFLASH 	0x02
#define BOOTCOPYMOPFLASH	0x04
#define BOOTCOPYRCPFLASH	0x08
#define BOOTIGNORECONFIGBITS    0x20000000
#define BOOTRUNIMAGE		0x40000000
#define BOOTLOADNETBOOTIMAGE	0x80000000

/*
 * plb_result codes
 */

#define PLB_OK			0x00 /* everything's fine and dandy */
#define PLB_ERROR_RETRY		0x01 /* error - retry */
#define PLB_ERROR_NORETRY	0x02 /* error - don't retry */

#endif /* !defined(__PLB_H__) */
