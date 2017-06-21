/* $Id: boot.h,v 3.3.62.1 1996/05/17 11:34:08 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/os/boot.h,v $
 *------------------------------------------------------------------
 * boot.h -- network configuration and loading support
 * 
 * Bill Palmer, September 1988
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: boot.h,v $
 * Revision 3.3.62.1  1996/05/17  11:34:08  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.1  1996/05/02  22:06:25  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3  1995/11/17  18:44:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:58  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/16  21:35:32  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Merde, not my day.  Missed file in last commit.
 *
 * Revision 2.2  1995/07/15  02:37:03  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.1  1995/06/07  21:53:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BOOT_H__
#define __BOOT_H__

/* Neither BOOT_TFTP nor BOOT_MOP should be zero.  See cfg_boot.h */
#define BOOT_NONE 0
#define BOOT_NET  1
#define BOOT_SYS  2
#define BOOT_HOST 3
#define BOOT_BOOT 5
#define BOOT_BUFFERSIZE 6
#define BOOT_TFTP   7
#define BOOT_MOP    8
#define BOOT_FILE   9
#define	BOOT_FLASH 10
#define BOOT_MODULE 11
#define BOOT_RCP    12

#define TFTP_MAXDATA 512		/* standard amount of data */
#define MAXRETRIES 3			/* retransmit up to this often */
#define BOOTRETRIES 12			/* send bootload requests this often*/
#define TFTPD_FAILURE_SIZE 0x1000000	/* tftpd fails on this size */

#define MAXCHRS_BOOT_FILENAME 64
#define MAXCHRS_BOOT_PROTOCOL 20
#define MAXCHRS_BOOT_HOSTNAME 64
#define MAXCHRS_BOOT_ARGUMENT 20

#define DEFAULT_CONFIG_BYTES 32768	/* Default configuration buffer */

/*
 * Note : Size of this struct should not exceed the size of
 * boot_information[BOOTINFOSIZE] defined in boot/extern.c.
 * BOOTINFOSIZE is currently defined as 128.
 * Size of this bootinfo_t structure is 114.
 */
struct bootinfo_t_ {
    int	protocol;		/* Protocol identifier */
    char filename[MAXCHRS_BOOT_FILENAME]; /* Boot filename */
    ipaddrtype ipaddr;		/* If booted from IP, host address */
    char mopaddr[IEEEBYTES];	/* If booted from MOP, boot address */
    char mopif[MAXCHRS_BOOT_ARGUMENT]; /* If booted from MOP, interface name */
    char mophost[16];		/* If booted from MOP, host name */
};

/*
 * Process load block constructed by the ROM monitor.
 * Now defined in just one place - it's own .h file.
 */
#include "../../boot/plb.h"

/***********************************************************************
 *
 *                      Externs and Prototypes
 *
 ***********************************************************************/
extern queuetype bootconfgQ;
extern queuetype sysconfgQ;
extern boolean some_errmsgs_have_printed;

extern boolean configure (queuetype *, filetype *, boolean);
extern boolean read_config_file(filetype *, boolean, uchar);
extern process bootload(void);
extern void boot_init(void);
extern void boot_bootinfo(void);
extern boolean boot_fromflash(char *);
extern boolean bootflash_exists(void);

#endif __BOOT_H__
