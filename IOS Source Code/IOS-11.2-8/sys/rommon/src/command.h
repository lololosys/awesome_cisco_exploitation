/* $Id: command.h,v 3.2 1995/11/17 18:41:08 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/rommon/src/command.h,v $
 *------------------------------------------------------------------
 * command.h
 *
 * Oct, 95, Steve J. Zhang
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: command.h,v $
 * Revision 3.2  1995/11/17  18:41:08  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:04:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:23:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/10/06  23:11:10  szhang
 * placeholder for DBUS handlers in RSP2
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __COMMAND_H__
#define __COMMAND_H__

#include "mon_elf.h"

#ifdef ASM          /* ASM version of definitions */

#else              /* C version of definitions */

/* WCS opcodes */
#define WCS_WR_CMD_REG            0x01
#define WCS_WR_DAT_REG            0x02
#define WCS_LOADED                0x04
#define WCS_RESET_ROM_ADDRESS     0x07  /* no action taken */
#define WCS_INCREMENT_ROM_ADDRESS 0x08  /* no action taken */
#define WCS_READ_ROM              0x0b  /* no action taken */
#define WCS_RD_CMD_REG            0x0d
#define WCS_RD_DAT_REG            0x0e

/* WCS commands */
#define WCS_INIT_CNT 0x10
#define WCS_WR_RAM   0x30
#define WCS_WR_PAR   0x31
#define WCS_RD_RAM   0x20
#define WCS_RD_PAR   0x21
#define WCS_IMAGE_TRANSFER 0x25
#define WCS_DIAG     0x26



#define UCODE_TYPE       0
#define VPLD_TYPE        1
#define DBUS_TYPE        2

#define UNIX_COMPRESSION 1
#define NOT_COMPRESSED   0
#define TRANSFER_FROM_RSP 0
#define TRANSFER_TO_RSP   1


/* data is sent between dummy and cksum.  Cksum is sent after
   the data is sent */

/*
 * wcs_image_header:
 * This structure outlines the format of the header of a WCS image
 * download/upload.  The fields, addr, length, start_addr, direction,
 * comp, code_type, dummy, and version make up the header to be
 * downloaded.  The cksum field follows the entire image.
 * The data portion follows the version field and is equal
 * in length to the value of the length field.  The data 
 * portion may be compressed or not compressed according to the
 * value of the comp field.
 */
typedef struct wcs_image_header_ {
    uint  addr;       /* Address of image in CIP memory */
    uint  length;     /* Length of image */
    uint  start_addr; /* For download, address to call to start
			 kernel_init() */
    uchar direction;  /* 0: from RP
			 1: to RP from CIP */
    uchar comp;       /* compression, 0: no comp 
		       *              1: compressed
		       */
    uchar code_type;  /* 0: firmware code download
		       * 1: vpld download 
		       * 2: secondary dbus handler
		       */ 
                      /* Do not change location
			 of this byte.  image_transfer depends on this
			 byte being at least 3 bytes before the end of
			 the header */
    uchar dummy[1];
    ushort version;   /* Header version */
           /* Data goes here */
    ushort cksum;     /* checksum of data portion of the image */
} wcs_image_header;

typedef enum wcs_state_ {awaiting_wcs_opcode, loading_cmd_reg, awaiting_data,
	     reading_data, image_mode} wcs_state;

#define OFFSETOF(type,memb)  ((size_t)&((type *)0)->memb)

#endif

#endif __COMMAND_H__
