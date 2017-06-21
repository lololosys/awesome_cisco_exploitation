/* $Id: flash_defines.h,v 3.6.20.2 1996/07/03 01:21:09 ramk Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_defines.h,v $
 *------------------------------------------------------------------
 * flash_defines.h -  defines for all Flash code.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: flash_defines.h,v $
 * Revision 3.6.20.2  1996/07/03  01:21:09  ramk
 * CSCdi58260:  agent returned out of range and non-enumeration for
 * ciscoFlashMIB
 * Branch: California_branch
 *
 * Revision 3.6.20.1  1996/05/07  00:37:45  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.6  1996/02/06  23:06:10  abaindur
 * CSCdi47558:  COPY MOP FLASH fails.
 *  Getting there.... one more try. Fix copy setup/cleanup for relocatable
 *  images. Increase buffer size to deal with the 1492 byte data blocks
 *  plus leftovers from previous block when crossing over from the reloc
 *  tables section to the text section.
 *
 * Revision 3.5  1995/12/27  07:43:18  abaindur
 * CSCdi46143:  Checksum generation broken for copy to flash on low-end
 *
 * Revision 3.4  1995/12/18  07:08:42  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.3  1995/12/08  19:25:34  smackie
 * Move memory size defines into a central place. (CSCdi45214)
 *
 * Revision 3.2  1995/11/17  09:00:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/24  19:15:34  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.2  1995/08/25  11:43:22  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.1  1995/06/07 20:24:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_DEFINES_H__
#define __FLASH_DEFINES_H__

#include "address.h"

/*
 * Global flash defines 
 */

/*
 * Device aliases
 */
#define SF "flash"
#define BF "bootflash"

/*
 * Minimum and Maximum number of flash partitions that will be
 * allowed (refer to CISCO-FLASH-MIB.my definition of 
 * ciscoFlashDeviceMaxPartitions)
 */
#define FLASH_MIN_PART_CNT 1
#define FLASH_MAX_PART_CNT 8

/*
 * Minimum device chip count
 * (refer to CISCO-FLASH-MIB.my definition of 
 * ciscoFlashDeviceChipCount)
 */
#define FLASH_MIN_CHIP_CNT 1

/*
 * Show device level info.
 */
#define  SHOW_FLASH_SIZE	1
#define  SHOW_FLASH_CHIPS	2
#define  SHOW_FLASH_ERR		4
#define  SHOW_FLASH_ALL		7
#define  SHOW_VER_FLASH_SIZE	8
#define  SHOW_DEV_INFO_MASK     0xFF

/*
 * Show file system level info.
 */
#define  SHOW_FILES_BRIEF	0x100
#define  SHOW_FILES_DETAILED	0x200
#define  SHOW_FILES_SUMMARY	0x400
#define  SHOW_FS_INFO_MASK      0xFF00

#ifndef MIN
#define MIN(x,y)  ((x)<(y)?(x):(y))
#endif 

#define FLASH_PHY_DEV_NAME_LEN (((sizeof("slavebootflash:0") + 3) / 4) * 4)
#define FLASH_LOG_DEV_NAME_LEN 32
#define FLASH_DEV_NAME_LEN     (FLASH_PHY_DEV_NAME_LEN + FLASH_LOG_DEV_NAME_LEN)
/*
 * Be careful when changing the file name length. This
 * should match the name length defined in the file system
 * module.
 */
#define FLASH_FILE_NAME_LEN    (64 - (2 * sizeof(ulong)) - \
                               (2 * sizeof(ushort)) - sizeof(uchar *))
#define FLASH_FILE_NAME_MALLOC_LEN  (FLASH_FILE_NAME_LEN + FLASH_DEV_NAME_LEN)
#define FLASH_PATH_MALLOC_LEN  (FLASH_DEV_NAME_LEN + \
                                FLASH_FILE_NAME_MALLOC_LEN)
#define FLASH_SERVER_NAME_LEN  64
#define FLASH_SERVER_NAME_MALLOC_LEN  80
#define FLASH_PATH_SEPARATOR    ':'

#define FLASH_MIN_FREE_SPACE (4 * 1024)
#define FLASH_MIN_HDR_SIZE  64

#define FLASH_PRIMARY       0
#define FLASH_FOR_BOOTSTRAP 1
#define FLASH_PCMCIA_CARD   2

#define MAX_CHIPS_IN_BANK   4
#define MAX_CHIPS	    64

#define BW_8BITS            1   /* Bus width */
#define BW_16BITS           2
#define BW_32BITS           4

#define BW_8BITS_MASK       0xFF   
#define BW_16BITS_MASK      0xFFFF
#define BW_32BITS_MASK      0xFFFFFFFF

#define VPP_OFF		0
#define VPP_ON 		1
#define VPP_UNKNOWN	2
#define CACHE_OFF	0
#define CACHE_ON	1

/* vpp ramp times, in milliseconds */
#define VPP_RAMP_UP     100
#define VPP_RAMP_DOWN   100

enum flash_cache {
    CACHE_NOT_INIT,     /* cache control not yet set up */
    CACHE_NONE,         /* no cache control needed - xx */
    CACHE_OLD_CACHE,    /* old emts must be used */
    CACHE_NEW_CACHE     /* new emts - rom mon 4.6-0.11 or later */
};

/*
 * Flash MIB specified file system copy modes
 */
#define FS_COPY_UNKNOWN   1
#define FS_COPY_FLH       2
#define FS_COPY_DIRECT    3

/*
 * Flash MIB specified device status values
 */
#define SNMP_DEV_STATE_RDONLY    1
#define SNMP_DEV_STATE_RFF       2
#define SNMP_DEV_STATE_RW        3

/*
 * Flash MIB specified file status values
 */
#define FLASH_FILE_DELETED            1
#define FLASH_FILE_INVALID_CHKSUM     2
#define FLASH_FILE_VALID              3

#define TWO_FIFTY_K             (250 * ONE_K)
#define ONE_MB                  ONE_MEG
#define FS_MALLOC_BLOCK         TWO_K

/*
 * Error snmp status defines for Copy object
 */
#define SNMP_NO_ERROR			0
#define SNMP_INVALID_OP			1
#define SNMP_INVALID_PROTO		2
#define SNMP_INVALID_SRC_FILE		3
#define SNMP_INVALID_DEST_FILE		4
#define SNMP_INVALID_SRVR_ADDR		5
#define SNMP_DEVICE_BUSY		6
#define SNMP_DEVICE_OPEN_ERR		7
#define SNMP_DEVICE_ERR			8
#define SNMP_DEVICE_READONLY		9
#define SNMP_DEVICE_FULL		10
#define SNMP_FILE_OPEN_ERR		11
#define SNMP_FILE_TRANSFER_ERR		12
#define SNMP_FILE_CHECKSUM_ERR		13
#define SNMP_FILE_DELETE_FAIL		14
#define SNMP_FILE_UNDELETE_FAIL		15
#define SNMP_LOW_RESOURCES		16
#define SNMP_UNKNOWN_ERROR		17
#define SNMP_DEVICE_SQUEEZE_FAIL	18
#define SNMP_DEVICE_FORMAT_FAIL		19

/*
 * FLH and DFB feature support. These values
 * are based on the #defines in boot/flh.h.
 */
#define FLASH_BASIC_FLH_SUPPORT		01
#define FLASH_FLH_SUPPORT_WITH_DFB	02
#define FLASH_FLH_SUPPORT_WITH_RCP	03

/*
 * General purpose data structure used in most of the flash
 * services (copy commands). Will help avoid mallocing
 * multiple buffers.
 */
typedef struct flash_copy_parms {
    /* NOTE : Keep buff as the first field */
    uchar buff[4*ONE_K];                 /* data buffer */
    ulong buflen;                        /* length of buffer to be used */
    int datalen;                         /* length of data in buff */
    addrtype addr;                       /* IP address */
    int   sts;                           /* status  of operation */
    int   snmp_sts;			 /* status of SNMP operation */
    char *rcp_username;
    int fh;                              /* file handle */
    int dh;                              /* device handle */
    long option;                         /* options for operation */
    sys_timestamp start_time;            /* start time of the operation */
    ulong elapsed_time;                  /* time taken for operation */
    ulong erase;                         /* whether to erase */
    ulong prompt;                        /* whether to prompt */
    ulong verbose;                       /* whether to be eloquent */
    ulong copy_option;                   /* TFTP or MOP copy */
    char devnm[FLASH_DEV_NAME_LEN];      /* device name */
    char sfname[FLASH_FILE_NAME_MALLOC_LEN];  /* source file name */
    char dfname[FLASH_FILE_NAME_MALLOC_LEN];  /* dest file name */
    char path[FLASH_PATH_MALLOC_LEN];    /* file path */
    char host[FLASH_SERVER_NAME_MALLOC_LEN];    /* host name or MAC address */
} flash_copy_parms_t;

#endif /* __FLASH_DEFINES_H__ */
