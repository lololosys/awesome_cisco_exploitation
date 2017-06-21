/* $Id: nv.h,v 3.2.62.6 1996/09/10 02:15:54 ahh Exp $
 * $Source: /release/112/cvs/Xsys/os/nv.h,v $
 *------------------------------------------------------------------
 * Declarations for nvram
 *
 * January 1993, Dave Katz
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: nv.h,v $
 * Revision 3.2.62.6  1996/09/10  02:15:54  ahh
 * CSCdi68092:  NVRAM in use errors with service compress-config
 * NVRAM access code fails to clean itself up properly when another user
 * tries to look at the config simultaneously.
 * Branch: California_branch
 *
 * Revision 3.2.62.5  1996/08/03  04:06:10  che
 * CSCdi65047:  Kerberos SRVTAB keys not encrypted
 * Branch: California_branch
 * Clean up Private NVRAM code.
 *
 * Revision 3.2.62.4  1996/05/29  14:07:08  bew
 * CSCdi56794:  IOS private configuration not always recovered from NVRAM
 * Branch: California_branch
 * Added nv_private_getptr() to find the beginning of private NVRAM. This
 * is called by some platform-specific code as well as
 * platform-independant code.
 *
 * Revision 3.2.62.3  1996/04/19  15:24:36  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 2.1.6.4  1996/04/09  21:28:38  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 2.1.6.3  1996/04/02  10:31:48  carrel
 * cvs fixes
 *
 * Revision 2.1.6.2  1996/02/29  19:04:01  bew
 * Branch: IosSec_branch
 * Commit of the private/encrypted NVRAM feature. This is in support
 * of the crypto project and allows portions of NVRAM to be
 * encrypted, and a portion to be private key storage for the IOS.
 *
 * Revision 2.1.6.1  1995/11/17  17:22:54  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 3.2.62.2  1996/04/15  14:59:05  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.2.62.1  1996/03/18  21:30:30  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.1  1996/01/24  22:36:42  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:49:59  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:47:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:19:04  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:57:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#if !defined(__NV_H__)
#define __NV_H__

extern hwidbtype *nv_idbp;
extern hwidbtype *flash_nv_idbp;
extern char *nv_line_buffer;

extern nvtype *nvptr;

extern boolean nv_valid;
extern boolean nv_writeflag;

#define NV_V1_TEXTBASE(p)        ((uchar *)&(p->v1_ptr))
#define NV_V2_TEXTBASE(p)        (((uchar *)p) + sizeof(nvtype))

extern void nv_init(void);
extern boolean nv_check_nvptr(void);
extern void nv_set_nvptr(void *);
extern void nv_clear_nvptr(void);
extern uint nv_get_config_size(void);
extern int  nv_check_getptr(nvtype *);
extern int  nv_check_done(nvtype *);
extern void nv_restore_nvptr(nvtype *);
extern boolean nv_rom_supports_nvcompress(void);
extern boolean nv_badptr(nvtype *ptr);
extern nvtype *nv_getptr(void);
extern nvtype *nv_getptr_raw(void);
extern void nv_done(nvtype *ptr);
extern nvtype *nv_review_buffer(void);
extern void nv_writedisable(nvtype *ptr);
extern void nv_writeenable(nvtype *ptr);
extern void nv_write_summary(boolean eraseflag);
extern void nv_bcopy(void const *, void *, int);
extern void nv_setconfig(ushort);
extern void nv_set_master_key(int, char *, boolean);
extern void nv_store_master_keys(void);
extern void nv_write_private(boolean, char *, ...);
extern void nv_add_private(boolean, char *, ...);
extern int nv_current_to_buffer(char *, uint, boolean, uchar, boolean);
extern boolean ok_to_write_nv(void);
extern int nv_current_to_nv(boolean);
extern int nv_buffer_to_nv(char *, uint, boolean);
extern int nv_erase(boolean);
extern void nv_configure(uchar);
extern void nv_line(const char *, const int);
extern void nv_line_end(void);
extern boolean nv_write_open(void);
extern int nv_write_close(boolean);
extern nvtype_private *nv_private_getptr(nvtype *);
extern boolean nv_there_are_keys(void);
extern boolean nv_encrypt(char *, int);
extern boolean nv_decrypt(char *, int *);
extern void nv_build_private_config_header(char *, nvtype_private *);

#endif /* !defined(__NV_H__) */
