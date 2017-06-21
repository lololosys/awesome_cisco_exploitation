/* $Id: flash_fmgr_private.h,v 3.2.60.2 1996/08/02 22:52:57 smackie Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_fmgr_private.h,v $
 *------------------------------------------------------------------
 * flash_fmgr_private.h -- private definitions for Flash file support
 *
 * 18-December-1990, Chris Shaker
 * April 1991, Steve Elias
 * Octoberfest-1991, Darren Kerr
 * March 1992, Dit Morse
 * February 1993, Dit Morse
 *          - renamed in the restructuring of flash support
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_fmgr_private.h,v $
 * Revision 3.2.60.2  1996/08/02  22:52:57  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/07/12  02:00:47  widmer
 * CSCdi52582:  parser should remove the trailing spaces in the filename
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:01:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:24:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define MAX_RETRIES          20
#define FFH_FLAGS_DELETED    0x0001
#define FFH_FLAGS_SYS_CONFIG            0x0002
#define FFH_FLAGS_SYS_IMAGE             0x0004

#ifndef MIN
#define MIN(x,y)  ((x)<(y)?(x):(y))
#endif 

/*
 * Structure used by callback routine tftp2flash_write_ok
 */
struct flash_write_args {
    flash_fhdr *ffp;
    uchar *buf;
    uchar *flash_addr;
    boolean verbose;
};

void show_flash(parseinfo *csb);
void flash_print_hardware(void);
void flash_print_chips(int flag);
void flash_print_avail(void);
void flash_print_files(boolean all);
void copy_command(parseinfo *csb);
flash_fhdr *flash_init_fhdr(void);
int write_tftp_file(flash_fhdr *ffp, uchar *buff, uchar *flash_addr,
		    ulong len);
int write_mop_file(ulong *ffp, uchar *buff, uchar *flash_addr, ulong len);
int net2flash (int mode, flash_dev_t *dev, addrtype *host, const char *remname,
	       const char *locname, uchar *buff, boolean allowerase, 
	       const char *host_namep);
extern int flash2tftp(flash_dev_t *dev, addrtype *host, const char *remname,
		      const char *locname, uchar *buff);
extern void flash2rcp(flash_dev_t *dev, addrtype *host, const char *remname,
		      const char *locname);
int boot_flash_file(const char *filename, uchar **bufp, ulong *sizep);
int read_flash_boot_file(flash_dev_t *dev, flash_fhdr *fh, uchar *bufp,
			 ulong *sizep, boolean netboot);
int flash_delete_file(flash_dev_t *dev, const char *filename);
ushort flash_verify(flash_dev_t *dev, flash_fhdr *ffp, uchar *bp, uchar *addr,
		    boolean verbose);
boolean flash_read_fhdrs(flash_dev_t *dev);
flash_fhdr *flash_get_fhdr(flash_dev_t *dev, const char *filename);
int flash_get_fn(flash_dev_t *dev, const char *filename);
flash_dev_t *flash_open(flash_dev_t *dev, int flags);
void flash_close(void);
ushort add_to_cksum(ushort new_word, ushort old_cksum);
void verify_checksum(void);
int erase_command(void);
int flash_erase_all(flash_dev_t *dev);
boolean snmp_flash_exist(void);
boolean snmp_flash_busy(void);
uint snmp_flash_size(void);
uint snmp_flash_free(void);
boolean snmp_flash_vpp(void);
int snmp_flash_filecnt(void);
uchar *snmp_flash_card(void);
uchar *snmp_flash_ctlr(void);
int snmp_flash_erase_all(void);
int snmp_flash2tftp(addrtype *address, const char *filename);
int snmp_tftp2flash(addrtype *address, const char *filename);
uchar *snmp_flash_dir_filename(int i);
boolean snmp_flash_filedeletestatus(int i);
int snmp_flash_filesize(int i);
