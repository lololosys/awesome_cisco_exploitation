/* $Id: flh.h,v 3.3 1996/02/19 17:28:18 abaindur Exp $
 * $Source: /release/111/cvs/Xboot/flh.h,v $
 *------------------------------------------------------------------
 * flh support header file
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flh.h,v $
 * Revision 3.3  1996/02/19  17:28:18  abaindur
 * CSCdi30593:  SHOW VERSION underreports processor memory by 4 kilobytes
 *
 * Revision 3.2  1995/11/17  07:41:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  09:04:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:11:59  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__FLH_H__)
#define __FLH_H__

/* 
 * flash load helper 
 * This structure is passed from system code to the flash load helper
 * emt call.  It contains enough info to instruct the rxboot image to
 * perform the copy operation
 */

struct flash_load_helper {
    uint flags;
    char *src_filename;
    char *dst_filename;
    char *host;
};

/* flags for tftpboot_helper above */
#define HELPER_TFTP_COPY_FLASH        0x01
#define HELPER_FLASH_ERASE            0x02
#define HELPER_MOP_COPY_FLASH         0x04
#define HELPER_RCP_COPY_FLASH         0x10
#define HELPER_IGNORE_CONFIG_BITS     0x08

/* version returned when flh is invoked */

#define FLH_VERSION_INITIAL	0x00 /* Initial flh only version */
#define FLH_VERSION_WITH_DFB	0x01 /* flh + dfb support */
#define FLH_VERSION_WITH_RCP    0x02 /* RCP support */

/* loggin data structures */

struct _flhlog_begin {
    ulong magic2;		/* magic numbers to confirm buffer existance */
    uchar *start;		/* pointer to first data element */
    uchar *end;			/* pointer to last data element */
    uchar *next_input;          /* next location to put data */
    uchar *output_ptr;         /* location to start printing from */
};

struct _flhlog_end {
    ulong dummy;                /* location of zero */
    ulong length;		/* length of data buffer (structs included) */
    ulong magic1;		/* magic number to confirm buffer existance */
};

#define FLHLOGMAGIC1 		0xadad2bee
#define FLHLOGMAGIC2 		0xabad1dea
#define DEFAULT_FLHLOG_SIZE  	0x1000
#define FLH_DISPLAY_BUFSIZE 	(DEFAULT_FLHLOG_SIZE * 3)

#define FLHASCIIMASK 		0x7f
#define FLHREPEAT_FLAG 		0x80

struct _flh_log_routines {
    void (* flh_write_char)(char);
    void (* flh_print_output)(char *,int);
    void (* flh_clear_buff)(void);
};

#define FLH_GET_VERSION 	0x00
#define FLH_DO_IT		0x01
#define FLH_GET_ROUTINES 	0x02
#define FLH_GET_PARMBUFF_LEN	0x03
#define FLH_GET_LOGBUFF_LEN	0x04

#endif /* !defined(__FLH_H__) */
