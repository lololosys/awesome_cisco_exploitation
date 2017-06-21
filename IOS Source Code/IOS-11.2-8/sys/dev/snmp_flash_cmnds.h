/* $Id: snmp_flash_cmnds.h,v 3.2 1995/11/17 09:01:40 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/snmp_flash_cmnds.h,v $
 *------------------------------------------------------------------
 * snmp_flash_cmnds.h - High level flash routines for SNMP.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: snmp_flash_cmnds.h,v $
 * Revision 3.2  1995/11/17  09:01:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:25:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SNMP_FLASH_CMNDS_H__
#define __SNMP_FLASH_CMNDS_H__

/*
 * Prototypes
 */
extern boolean snmp_flash_exist(void);
extern boolean snmp_flash_busy(void);
extern uint snmp_flash_size(void);
extern uint snmp_flash_free(void);
extern boolean snmp_flash_vpp(void);
extern int snmp_flash_filecnt(void);
extern uchar *snmp_flash_card(void);
extern uchar *snmp_flash_ctlr(void);
extern int snmp_flash_erase_all(void);
extern int snmp_flash2tftp(addrtype *, char *);
extern int snmp_tftp2flash(addrtype *, char *);
extern uchar *snmp_flash_dir_filename(int);
extern boolean snmp_flash_filedeletestatus(int);
extern int snmp_flash_filesize(int);

#endif /* __SNMP_FLASH_CMNDS_H__ */
