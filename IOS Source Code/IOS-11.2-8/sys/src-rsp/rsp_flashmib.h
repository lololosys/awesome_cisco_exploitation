/* $Id: rsp_flashmib.h,v 3.2 1995/11/17 18:48:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/src-rsp/rsp_flashmib.h,v $
 * ------------------------------------------------------------------
 * Flash MIB support definitions
 *
 * October 1995, Carl Schaefer
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * ------------------------------------------------------------------
 * $Log: rsp_flashmib.h,v $
 * Revision 3.2  1995/11/17  18:48:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:28:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:28:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/18  01:22:50  schaefer
 * placeholders for 7500 flash MIB
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

/*
 * hardwired devices
 */
#define RSPDEV_SLOT0		0
#define RSPDEV_SLOT1		1
#define RSPDEV_BOOTFLASH	2

#define RSPDEVNAME_SLOT0	"slot0"
#define RSPDEVNAME_SLOT1	"slot1"
#define RSPDEVNAME_BOOTFLASH	"bootflash"

#define RSP_FLASHMIB_PORT	"Flash MIB"

extern void rsp_flashmib_init(void);
