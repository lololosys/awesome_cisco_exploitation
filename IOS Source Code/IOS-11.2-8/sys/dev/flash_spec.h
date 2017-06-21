/* $Id: flash_spec.h,v 3.2 1995/11/17 09:01:23 hampton Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/dev/flash_spec.h,v $
 *------------------------------------------------------------------
 * flash_spec.h -- public definitions for Flash file support
 * 
 * common interface spec for
 *    flash.c      - ags+ flash support
 *    flash_fmgr.c - high level support for xx/igs/rp1
 *
 * February 1993, Dit Morse
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_spec.h,v $
 * Revision 3.2  1995/11/17  09:01:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:25:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

/* Maximum partition count in flash.  For high end systems, the value is
 * 0.  For low end, it's defined in sys/les/flash_fs_les.h.
 */
#define FLASH_MAX_PART_CNT    0

extern void flash_parser_init(void);
extern long flash_file_offset(const char *filename);
extern long flash_file_length(const char *filename);
extern int flash_read_file(const char *filename, uchar *bufp,
			   ulong *sizep, boolean netboot);
extern int flash_read_block(int offset, uchar *buff, int len);
extern ushort update_cksum(ushort cksum, uchar *wp, ulong i);
extern ushort update_cksum_may_suspend(ushort cksum, uchar *wp, ulong i);
extern ushort flash_file_cks(const char *filename);
extern const char *flash_bootfile(void);
extern boolean flash_file_exists(const char *filename);
extern boolean write_buffer_to_flash(void *, const char *, uchar *, ulong);
extern void copy_command (parseinfo *);
extern void flash_print_avail(void);
extern void flash_print_files(boolean);
extern void verify_checksum(void);
extern boolean flash_tftp_server(ipsocktype *, const char *, boolean);
extern boolean flash_block_server(const char *, uchar *, ulong *, boolean,
                          boolean (*func)(uchar **, long *, long, void *),
                          void *);

/* snmp support */
extern boolean snmp_flash_exist(void);
extern boolean snmp_flash_busy(void);
extern uint snmp_flash_size(void);
extern uint snmp_flash_free(void);
extern boolean snmp_flash_vpp(void);
extern int snmp_flash_filecnt(void);
extern uchar *snmp_flash_card(void);
extern uchar *snmp_flash_ctlr(void);
extern int snmp_flash_erase_all(void);
extern int snmp_flash2tftp(addrtype *address, const char *filename);
extern int snmp_tftp2flash(addrtype *address, const char *filename);
extern uchar *snmp_flash_dir_filename(int i);
extern boolean snmp_flash_filedeletestatus(int i);
extern int snmp_flash_filesize(int i);
