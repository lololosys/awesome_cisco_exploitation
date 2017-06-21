/* $Id: flh_config.h,v 3.2 1995/11/17 09:07:42 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/flh_config.h,v $
 *------------------------------------------------------------------
 * Flash config header file.
 *
 * July 1995, Yan Ke
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flh_config.h,v $
 * Revision 3.2  1995/11/17  09:07:42  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  01:13:12  yanke
 * Placeholder file.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __flash_config_h__
#define	__flash_config_h__

#define	SET_WRITE_FLASH_CONFIG	1	/* write access type */
#define	SET_READ_FLASH_CONFIG	2	/* read access type */
#define	SET_ERASE_FLASH_CONFIG	3	/* erase access type */

#define	FLASH_CONFIG_IN_USE	0x0001	/* exclusive use flash config rec */
#define	FLASH_CONFIG_ON		0x0002	/* turn on the flash config */
#define	FLASH_CONFIG_ONLY	0x0004	/* indicate only save to flash */

/*
 * Max length config file name
 */
#define	MAX_CONFIG_FILE_ID	128

extern char config_file_id[];	/* ptr to startup config file name */

typedef struct flash_config_ 
{
    uint flag;		/* bitmask, see above defined flags */
    int access;		/* access type of operation (rd/wr/erase) */
    int fd;		/* file descriptor of config file opened */
    char *cfg_file_id;	/* pointer to config file id */
    char *buffer;	/* allocated buffer for read/write */
    nvtype *nvp;	/* save the real nvtype pointer */
} flash_config_t;

extern int flash_config_on;

/*
 * Function prototypes.
 */
extern void flash_config_init(void);

extern int distilled_config_nv(void);
extern boolean distilled_config(char *);

extern boolean open_flash_config(nvtype **);
extern boolean close_flash_config(void);

#endif /* __flash_config_h__ */
