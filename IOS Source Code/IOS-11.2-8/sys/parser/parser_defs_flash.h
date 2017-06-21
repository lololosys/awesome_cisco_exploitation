/*
 * $Id: parser_defs_flash.h,v 3.2 1995/11/17 18:50:40 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/parser/parser_defs_flash.h,v $
 *------------------------------------------------------------------
 *	P A R S E R _ D E F S _ F L A S H . H
 *
 * 
 * Copyright (c) 1985-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: parser_defs_flash.h,v $
 * Revision 3.2  1995/11/17  18:50:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:55:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:29:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Copy command defines */
/* Source and destinations for copy command */
#define COPY_UNUSED                     0
#define COPY_FLASH_TFTP                 1
#define COPY_TFTP_FLASH                 2
#define COPY_TFTP_BOOTFLASH   	        3
#define COPY_BOOTFLASH_TFTP   	        4
#define COPY_RUNNING_CONFIG_TFTP        5
#define COPY_STARTUP_CONFIG_TFTP        6
#define COPY_TFTP_RUNNING_CONFIG        7
#define COPY_TFTP_STARTUP_CONFIG        8
#define COPY_FFFFFFFF_FLASH             9	/* OBSOLETE */
#define COPY_VERIFY_FLASH               10
#define COPY_SSE_MEMORY		        11
#define COPY_ERASE_FLASH      	        12
#define COPY_ERASE_BOOTFLASH  	        13
#define COPY_VERIFY_BOOTFLASH 	        14
#define COPY_MOP_FLASH                  15
#define COPY_MOP_BOOTFLASH    	        16
#define COPY_FLASH_RCP                  17
#define COPY_RCP_FLASH                  18
#define COPY_RCP_BOOTFLASH              19
#define COPY_BOOTFLASH_RCP              20
#define COPY_RUNNING_CONFIG_RCP         21
#define COPY_STARTUP_CONFIG_RCP         22
#define COPY_RCP_RUNNING_CONFIG         23
#define COPY_RCP_STARTUP_CONFIG         24
#define COPY_RUNNING_CONFIG             25
#define COPY_STARTUP_CONFIG             26
#define COPY_TFTP			27
#define COPY_RCP			28
#define COPY_FLHLOG			29
