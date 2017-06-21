/* $Id: flash_command.h,v 3.4 1996/02/16 15:25:12 banderse Exp $
 * $Source: /release/111/cvs/Xsys/filesys/flash_command.h,v $
 *------------------------------------------------------------------
 * Flash command header file
 *
 * August 1995, Yan Ke
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_command.h,v $
 * Revision 3.4  1996/02/16  15:25:12  banderse
 * CSCdi47098:  CIP ucode sneaks into slot0:   NOT !!
 * extern build_whole_file_id()
 *
 * Revision 3.3  1995/11/25  22:51:26  yanke
 * Add show device list. Fix a bug in copy command.
 *
 * CSCdi42314:  slave sync config command doesnt update slave config
 *
 * Revision 3.2  1995/11/17  09:07:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  00:42:14  yanke
 * Initial version.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __flash_command_h__
#define	__flash_command_h__

/*
 * For dir command optional parameters.
 */
#define	DIR_NORMAL		0
#define	DIR_LONG		1
#define	DIR_DELETED		2
#define	DIR_ALL			3

/*
 * For show flash command optional parameters.
 */
#define	SHOW_FLASH_FILE_NORMAL	0
#define	SHOW_FLASH_FILE_FILESYS	1
#define	SHOW_FLASH_FILE_CHIPS	2
#define	SHOW_FLASH_FILE_ALL	3
#define	SHOW_FLASH_FILE_DEV	4

#define IP_IMAGE_HEADER_SIZE 512
#define IP_IMAGE_SUFFIX_LEN 12

extern boolean flash_config_print;

#if defined(C7000)
extern void fs_init_rp_flash(void);
#endif

/*
 * Function prototypes.
 */
extern void show_file_command(parseinfo *);
extern void copy_file_command(parseinfo *);
extern void dir_command(parseinfo *);
extern void squeeze_command(parseinfo *);
extern void format_command(parseinfo *);
extern void boot_config_file_command(parseinfo *);
extern void show_flash_file_command(parseinfo *);
extern const char *get_default_dev_id(void);
extern void init_flash_command(void);
extern char *startup_config_to_buf(int *);
extern char *startup_private_config_to_buf(int *);
extern int get_whole_file_id(const char *, char *, int);
extern void build_whole_file_id(char *, const char *, const char *);

extern void rsp_flash_copy_command (parseinfo *);
extern boolean rsp_flash_tftp_server(ipsocktype *, const char *);
extern void rsp_flash_file_parse (char *, parseinfo *);
extern char *rsp_flash_first_bootable_file (void);
extern boolean rsp_file_boot_in_place(const char *, uchar **, ulong *);
extern int rsp_flash_file_read(const char *, char *, int);
extern boolean rsp_flash_file_exists(const char *);
typedef boolean (* callback_func_t)(uchar **, long *, long, void *);
extern boolean rsp_flash_block_server(const char *, char *, int, callback_func_t,
					void *);
#if !defined(C7000)
extern void rsp_flash_file_source (char *, uchar *);
#endif

#endif /* __flash_command_h__ */

