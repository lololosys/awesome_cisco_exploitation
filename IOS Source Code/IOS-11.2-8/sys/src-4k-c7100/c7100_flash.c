/* $Id: c7100_flash.c,v 3.2.56.7 1996/08/14 00:12:02 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_flash.c,v $
 *------------------------------------------------------------------
 * c7100_flash.c - Predator flash support
 *
 * July 1995, Michael Beesley
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_flash.c,v $
 * Revision 3.2.56.7  1996/08/14  00:12:02  thille
 * CSCdi65933:  Remove more obsolete files
 * Branch: California_branch
 * Remove dead references to empty obsolete file fs_dev.h.
 *
 * Revision 3.2.56.6  1996/05/21  10:03:28  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.2.56.5  1996/05/07  00:39:51  ssangiah
 * CSCdi56357:  Misleading erase command behavior in the RSP file system.
 *   o Remove erase command from the RSP file system parse chain.
 *   o Clean up the RSP file system parse chain.
 *   o Fix problem with return of error codes by the flash MIB agent.
 * Branch: California_branch
 *
 * Revision 3.2.56.4  1996/04/22  18:58:43  ssangiah
 * CSCdi54920:  Need to add support for the flash MIB on c7200.
 * Branch: California_branch
 *
 * Revision 3.2.56.3  1996/04/15  21:22:58  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2.56.2  1996/04/03  15:44:04  wfried
 * CSCdi52833:  PCMCIA Flash File System not working
 * Branch: California_branch
 * Fix Predator PCMCIA flash file system support. Major issues are byte
 * swapping, and PCMCIA controller timing.
 *
 * Revision 3.2.56.1  1996/03/21  23:47:48  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.7  1996/03/18  23:33:11  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.6  1996/03/01  20:44:09  wfried
 * CSCdi50199:  Need to re-enabled flash user interface
 * Branch: ELC_branch
 *
 * Revision 3.2.2.5  1996/02/10  01:02:32  wfried
 * Branch: ELC_branch
 * Disable flash file system until HW works.
 *
 * Revision 3.2.2.4  1996/02/08  09:26:51  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.2.2.3  1996/01/16  02:16:47  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1996/01/03  06:06:49  wfried
 * Branch: ELC_branch
 * Fix PCMCIA controller configuration.
 *
 * Revision 3.2.2.1  1995/11/22  21:43:07  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.2  1995/11/17  18:41:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:23:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include files 
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include "config.h"
#include "sys_registry.h"
#include "subsys.h"
#include "parser.h"

#include "../dev/flash_defines.h"
#include "../dev/dev_io.h"
#include "../dev/flash_registry.h"

#include "../parser/parser_actions.h"
#include "../parser/macros.h"
#include "../parser/parser_defs_exec.h"
#include "../parser/parser_defs_flash.h"
#include "../parser/parser_defs_config.h"

#include "../filesys/ralib.h"
#include "../filesys/fslib_internals.h"
#include "../filesys/fs_internals.h"
#include "../filesys/flh_config.h"
#include "../filesys/fslib_dev_intel_series_2plus.h"
#include "../filesys/fslib_dev_rsp_internal_flash.h"
#include "../filesys/flash_command.h"
#include "../filesys/filesys_debug.h"

#include "c7100_pcmap.h"
#include "c7100_flashmib.h"
#include "c7100_fpga.h"

/*
 * Define flash device aliases
 */
#define FLASH_CARD0_NAME_STR    "Flash PCMCIA card at slot 0"
#define FLASH_CARD1_NAME_STR    "Flash PCMCIA card at slot 1"
#define FLASH_SIMM_NAME_STR     "Flash internal SIMM"

/*
 * Default flash device.
 */
#define DEFAULT_DEV_ID	"slot0"

typedef struct flash_chip_rec_
{
    char flag;		/* indicate if entry is valid */
    char mfg_id;	/* manufacture id */
    char dev_id;	/* device id */
    char erase_value;	/* erase value */
    int size;		/* chip size */
    int sector_size;	/* sector(block) size */
    int alg_num;	/* algorithm number */
} flash_chip_rec_t;

/*
 * Only support intel flash chip for now.
 */
flash_chip_rec_t flash_chip_table[] =
{
	{1, 0x89, 0xA0, 0xff, 0x100000, 0x10000, 5},
	{1, 0x89, 0xA2, 0xff, 0x100000, 0x10000, 5},
	{1, 0x89, 0xA7, 0xff, 0x80000, 0x10000, 5},
	{1, 0x89, 0xA6, 0xff, 0x100000, 0x10000, 5},
	{1, 0x89, 0xAA, 0xff, 0x200000, 0x10000, 5},
	{0, 0, 0, 0, 0, 0, 0}
};

#define	CIS_ACTUAL_SIZE		0xaf	/* exact size of CIS block */
#define	CIS_MFG_ID_OFFSET	0xa6	/* offset of mfg record offset */
#define	CIS_MFG_ID_LEN		0x3	/* bytes in mfg record */

typedef struct format_dev_rec_
{
    uint alg_num;			/* algorithm number */
    uint sector_size;			/* sector size */
    uint device_size;			/* device size */
    uint erase_state;			/* erase state */
    char mfg_rec[CIS_MFG_ID_LEN];	/* 3-byte manufacture id */
    char reserved;			/* not used */
} format_dev_rec_t;

/*
 * Type of flash cards we support.
 */
format_dev_rec_t format_dev_table[] =
{
    /* 4MB/150ns */
    {4, 0x20000, 0x400000, 0xffffffff, {0x89, 0x00, 0x12}, 0},

    /* 8MB/150ns? */
    {4, 0x20000, 0x800000, 0xffffffff, {0x89, 0x00, 0x22}, 0},

    /* 20MB/150ns */
    {4, 0x20000, 0x1400000, 0xffffffff, {0x89, 0x00, 0x42}, 0},

    /* 4MB/200ns */
    {4, 0x20000, 0x400000, 0xffffffff, {0x89, 0x00, 0x11}, 0},

    /* 8MB/200ns? */
    {4, 0x20000, 0x800000, 0xffffffff, {0x89, 0x00, 0x21}, 0},

    /* 20MB/200ns */
    {4, 0x20000, 0x1400000, 0xffffffff, {0x89, 0x00, 0x41}, 0},

    {-1, 0, 0, 0, {0, 0, 0}, 0}
};

/*
 * get_flash_device_info:
 */
boolean get_flash_device_info (int devnum, int *size, int *sector, char *name)
{
    format_dev_parm_t fp_rec;
    format_dev_parm_t *fpp;
    int value;

    /*
     * Make sure flash devices are initialized.
     */
    value = ralib_device_init(devnum);
    if (value < 0)
	return FALSE;
    fpp = &fp_rec;
    memset(fpp, 0, sizeof(format_dev_parm_t));
    fpp->devnum = devnum;

    if (analyze_flash_device(fpp) >= 0) {
	*size = fpp->device_size;
	*sector = fpp->sector_size;
	sstrncpy(name, fpp->flash_name, FLASH_NAME_LEN_MAX);
	return TRUE;
    }
    return FALSE;
}

/*
 * flash_print_hardware:
 */
static void flash_print_hardware (void)
{
    int devnum;
    int size;
    int sector;
    char name[FLASH_NAME_LEN_MAX + 1];
    int retcode;

    /*
     * Eventually we'll ask fs to return a list
     * of devices.
     */
    for (devnum = C7100DEV_SLOT0; devnum <= C7100DEV_BOOTFLASH; devnum++) {
	memset(name, 0, FLASH_NAME_LEN_MAX + 1);
	retcode = get_flash_device_info(devnum, &size, &sector, name);
	if (retcode == TRUE) {
	    printf("\n%dK bytes of %s (Sector size %dK).",
		size/1024, name, sector/1024);
	    flush();
	}
    }
}

/* 
 * add_to_cksum(new_word, old_cksum);
 * Accumulates a checksum by adding the new word the old checksum, with
 * end-around carry.  
 * Returns the new checksum value.
 */
ushort add_to_cksum (ushort new_word, ushort old_cksum)
{
   u_long work;			/* work value for end-around add */
 
   work = old_cksum + new_word;	/* add new word to checksum */
    if (work & ~0xFFFF) 
    	work = ++work & 0xFFFF;		/* end-around carry */   
    return(work);
}

/*
 * update_cksum(ushort cksum, uchar *wp, ulong i, char v)
 * cksum a block, adding it to the old checksum.
 * the char v will print if provided to show Checksum
 * progress.
 */
ushort update_cksum (ushort cksum, uchar *wp, ulong i)
{
    if ( (ulong)wp & 0x01 ) {
	cksum = add_to_cksum(*wp++, cksum);
        i--;
    } 

    for ( ; i > 1; i -= sizeof(ushort)) 
	cksum = add_to_cksum(*((ushort *)wp)++, cksum);
    
    if (i == 1) 
	cksum = add_to_cksum(*wp << 8, cksum);
    return(cksum);
}

static boolean flash_alias_match (char *alias)
{
    if (strcmp(alias, SF) == 0)
	return TRUE;
    else
	return FALSE;
}

/*
 * Read CIS structure from device into given buffer,
 * filter out odd bytes.
 */
static int read_dev_cis (int devnum, char *buffer, int len)
{
    char *bufp;
    int size;
    int retcode;
    char *ptr;

    size = 2 * len;	/* need buffer twice big */
    bufp = malloc(size);
    if (bufp == NULL) {
	return FS_DESP_NO_MEM;
    }

    /*
     * Read raw CIS structure from device at 0 offset.
     */
    retcode = ralib_attrib_read(devnum, 0, bufp, size);
    if (retcode < 0) {
	if (filesys_debug)
	    printf("\nRead cis from dev %d error (%d)", devnum, retcode);
	return retcode;
    } else if (retcode != size) {
	if (filesys_debug)
	    printf("\nRead cis %d bytes instead of %d", retcode, size);
	free(bufp);
	return FS_DESP_CIS_RD_ERR;
    }

    /*
     * Filter out even bytes so pack structure
     * into contiguous block.
     */
    ptr = bufp;
    while (len--) {
	ptr++;
	*buffer++ = *ptr++;
    }
    free(bufp);
    return 0;
}

/*
 * Search supported flash chip table to find matching entry.
 */
static flash_chip_rec_t *find_flash_chip (char mfg_id, char dev_id)
{
    flash_chip_rec_t *fcp;

    for (fcp=&flash_chip_table[0]; fcp->flag; fcp++) {
	if (fcp->mfg_id == mfg_id && fcp->dev_id == dev_id)
	    return fcp;
    }
    return NULL;
}

/*
 * Set flash chip to read mode.
 */
static int set_flash_read_mode (int devnum, int offset, int width)
{
    int cnt;
    char flash_cmd[4];
    int value;

    /*
     * We only handle 1,2,4-byte case
     */
    if (width != 1 && width != 2 && width != 4)
	return FS_DESP_INT_ERR;

    /*
     * Construct read-mode command
     */
    for (cnt=0; cnt<width; cnt++)
	flash_cmd[cnt] = IFLASH_CMD_READ_ARRAY;

    /*
     * Issue the command.
     */
    value = FSLIB_ralib->write(devnum, &flash_cmd[0], offset, width);
    if (value < 0) {
	if (filesys_debug)
	    printf("\nPlace dev %d in read mode failed (%d)", devnum, value);
	return FS_DESP_FLSH_OP_ERR;
    } else if (value != width) {
	if (filesys_debug)
	    printf("\nPlace dev %d in read mode error (%d)", devnum, value);
	return FS_DESP_FLSH_OP_ERR;
    }
    return 0;
}

/*
 * Read mfg/dev id from the given device at offset.
 * The data width is also given. Caller needs to provide
 * enouch space to hold mfg/dev id according to width.
 */
static int read_flash_mfg_dev_id (int devnum, int offset, int width,
				  char *mfg_id, char *dev_id)
{
    int cnt;
    char flash_cmd[4];
    int value;

    /*
     * We only handle 1,2,4-byte case
     */
    if (width != 1 && width != 2 && width != 4)
	return FS_DESP_INT_ERR;

    /*
     * Construct id command
     */
    for (cnt=0; cnt<width; cnt++)
	flash_cmd[cnt] = IFLASH_CMD_INTELLIGENT_ID;

    /*
     * Issue the command to the chip.
     */
    value = FSLIB_ralib->write(devnum, &flash_cmd[0], offset, width);
    if (value < 0) {
	if (filesys_debug)
	    printf("\nIssue id cmd to dev %d failed (%d)", devnum, value);
	return value;
    } else if (value != width) {
	if (filesys_debug)
	    printf("\nIssue id cmd to dev %d error (%d)", devnum, value);
	return FS_DESP_FLSH_OP_ERR;
    }

    /*
     * Read mfg id and device id.
     */
    value = FSLIB_ralib->read(devnum, offset, mfg_id, width);
    if (value < 0) {
	if (filesys_debug)
	    printf("\nRead mfg id from dev %d failed (%d)", devnum, value);
	return value;
    } if (value != width) {
	if (filesys_debug)
	    printf("\nRead mfg id from dev %d error (%d)", devnum, value);
	return FS_DESP_FLSH_OP_ERR;
    }

    value = FSLIB_ralib->read(devnum, offset + width, dev_id, width);
    if (value < 0) {
	if (filesys_debug)
	    printf("\nRead dev id from dev %d failed (%d)", devnum, value);
	return value;
    } else if (value != width) {
	if (filesys_debug)
	    printf("\nRead dev id from dev %d error (%d)", devnum, value);
	return FS_DESP_FLSH_OP_ERR;
    }

    /*
     * Restore flash chip to read mode.
     */
    return set_flash_read_mode(devnum, offset, width);
}

/*
 * Identify flash chip by reading
 * its manufacture id/device id.
 */
static int identify_flash_chip (int devnum, int offset, int width,
    flash_chip_rec_t **fcp)
{
    int cnt;
    int value;
    char mfg_ids[4];
    char dev_ids[4];
    char mfg_id;
    char dev_id;
    flash_chip_rec_t *fcpp;

    /*
     * We only handle 1,2,4-byte case
     */
    if (width != 1 && width != 2 && width != 4)
	return (FS_DESP_INT_ERR);

    value = read_flash_mfg_dev_id(devnum, offset, width,
	&mfg_ids[0], &dev_ids[0]);
    if (value < 0)
	return (value);

    if (filesys_debug) {
	printf("\nmfg_id = %x.%x.%x.%x",
	    mfg_ids[0], mfg_ids[1], mfg_ids[2], mfg_ids[3]);
	printf("\ndev_id = %x.%x.%x.%x",
	    dev_ids[0], dev_ids[1], dev_ids[2], dev_ids[3]);
    }

    /*
     * For now we only support identical chip-group.
     */
    mfg_id = mfg_ids[0];
    for (cnt=0; cnt<width; cnt++) {
	if (mfg_id != mfg_ids[cnt]) {
	    if (filesys_debug) {
		printf("\nFlash chips's mfg id not identical (%x,%x)",
		    mfg_id, mfg_ids[cnt]);
	    }
	    return (0);
	}
    }
    dev_id = dev_ids[0];
    for (cnt=0; cnt<width; cnt++) {
	if (dev_id != dev_ids[cnt]) {
	    if (filesys_debug) {
		printf("\nFlash chips's dev id not identical (%x,%x)",
		    dev_id, dev_ids[cnt]);
	    }
	    return (0);
	}
    }

    /*
     * Found chip record and return pointer to it.
     */
    fcpp = find_flash_chip(mfg_id, dev_id);
    if (fcpp == NULL) {
	if (filesys_debug)
	    printf("\nFlash chip (%x,%x) not supported", mfg_id, dev_id);
	*fcp = NULL;
	return (FS_DESP_UNK_MFG_ID);
    }
    *fcp = fcpp;
    return (0);
}

/*
 * Check if the card is the one we support.
 * And if ok, update format param structures.
 */
static int analyze_dev_cis (format_dev_parm_t *fpp)
{
    int devnum;
    char cis_rec[CIS_ACTUAL_SIZE];
    char *ptr;
    format_dev_rec_t *fdp;
    int found;
    int value;
    ushort mfg_id;
    ushort dev_id;

    /*
     * Read CIS block into buffer.
     */
    devnum = fpp->devnum;
    ptr = &cis_rec[0];
    memset(ptr, 0, CIS_ACTUAL_SIZE);
    value = read_dev_cis(devnum, ptr, CIS_ACTUAL_SIZE);
    if (value < 0) {
	if (filesys_debug)
	    printf("\nRead cis from dev %d failed (%d)", devnum, value);
	return value;
    }

    /*
     * Check if we support the type.
     */
    ptr = &cis_rec[CIS_MFG_ID_OFFSET];
    for (fdp=&format_dev_table[0], found=0; fdp->alg_num>0; fdp++) {
	if (memcmp(fdp->mfg_rec, ptr, CIS_MFG_ID_LEN) == 0) {
	    found = 1;
	    break;
	}
    }
    if (found == 0) {
	if (filesys_debug) {
	    printf("\nUnknown device mfg id (%02x.%02x.%02x)",
		*ptr++, *ptr++, *ptr);
	}
	return FS_DESP_UNK_MFG_ID;
    }

    fpp->alg_num = fdp->alg_num;
    fpp->sector_size = fdp->sector_size;
    fpp->erase_state = fdp->erase_state;

    /*
     * Device with 20MB may show up as 16MB
     * and we need to detect existence of 5th
     * chip to tell which is which.
     */
    if (fdp->device_size == 0x1400000) {
	value = read_flash_mfg_dev_id(devnum, 0x1000000, 2,
			(char *)&mfg_id, (char *)&dev_id);
	if (value < 0)
	    return(value);

	if (mfg_id == 0x8989 && dev_id == 0xa0a0)
	    fpp->device_size = fdp->device_size;
	else
	    fpp->device_size = 0x1000000;
    } else
	fpp->device_size = fdp->device_size;

    if (devnum == 0)
	strcpy(&fpp->flash_name[0], FLASH_CARD0_NAME_STR);
    else
	strcpy(&fpp->flash_name[0], FLASH_CARD1_NAME_STR);

    return(0);
}

/*
 * flash_simm_size: 
 * Read asic flash simm register to determin flash simm size.
 */
static int flash_simm_size (void)
{
    ushort value;
    int bank_size, size;
    int bank_cnt;
    int total_size = 0;

    value = C7100_IO_FPGA->flash_simm_reg;
    if (value & FSIMMA) {
	/*
	 * Bit 12 is 1, IE no flash simm
	 */
	return(total_size);
    }

    size = (value & FLSH_SZ_MASK) >> FLSH_SZ_SHIFT;
    switch (size) {
	case FLSH_SZ_1MB:  bank_size = 0x100000; break;
	case FLSH_SZ_2MB:  bank_size = 0x200000; break;
	case FLSH_SZ_4MB:  bank_size = 0x400000; break;
	case FLSH_SZ_8MB:  bank_size = 0x800000; break;
	case FLSH_SZ_16MB: bank_size = 0x1000000; break;
	case FLSH_SZ_32MB: bank_size = 0x2000000; break;
	default :          bank_size = 0; break;
    }

    if (value & FBANK2A) {
	bank_cnt = 1;
    } else {
	bank_cnt = 2;
    }
    total_size = bank_size * bank_cnt;
    return(total_size);
}

/*
 * Figure out device type, size, sector size, etc.
 * Return 0 if all ok.
 * or native value if errors.
 */
int analyze_flash_device (format_dev_parm_t *fpp)
{
    int devnum;
    flash_chip_rec_t *fcp;
    int value;
    int i_width = 4;	/* internal flash is 32bit */
    int flash_size;

    devnum = fpp->devnum;
    if (devnum == C7100DEV_SLOT0 || devnum == C7100DEV_SLOT1)
	return analyze_dev_cis(fpp);  /* pcmcia flash card */

    if (devnum != C7100DEV_BOOTFLASH) {
	if (filesys_debug)
	    buginf("\nInvalid device number (%d)", devnum);
	return FS_DESP_INV_DEV_ID;
    }

    /*
     * This is internal system flash, see if we
     * support it.
     */
    value = identify_flash_chip(devnum, 0, i_width, &fcp);
    if (value < 0)
	return value;

    flash_size = flash_simm_size();

    fpp->alg_num = fcp->alg_num;
    /*
     * The size of flash simm can also be determined by
     * probing chip's dev/mfg id then multiplied by data-width.
     * Currently we read the size from async flash simm register.
     */
    fpp->device_size = flash_size;
    fpp->sector_size = i_width * fcp->sector_size;
    fpp->erase_state = fcp->erase_value;
    strcpy(&fpp->flash_name[0], FLASH_SIMM_NAME_STR);
    return 0;
}

/*
 * Parse chains for Flash copy commands
 */
#define	ALTERNATE	NONE
#include "../dev/exec_copy_flash.h"
LINK_POINT(flash_exec_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for Flash show commands
 */
#define	ALTERNATE	NONE
#include "../src-rsp/exec_show_flash_file.h"
LINK_POINT(flash_show_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chains for verify and erase exec commands. 
 */
#define	ALTERNATE	NONE
#include "../dev/exec_erase_cmnds.h"
LINK_POINT(exec_erase_commands, ALTERNATE);
#undef	ALTERNATE

#define	ALTERNATE	NONE
#include "../dev/exec_verify_cmnds.h"
LINK_POINT(exec_verify_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parse chain registration array for Flash
 */
static parser_extension_request system_flash_chain_table[] = {
    { PARSE_ADD_COPY_CMD, &pname(flash_exec_commands) },
    { PARSE_ADD_SHOW_CMD, &pname(flash_show_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_erase_commands) },
    { PARSE_ADD_EXEC_CMD, &pname(exec_verify_commands) },
    { PARSE_LIST_END, NULL }
};


/*
 * c7100_flash_parser_init - Initialize Flash parser support
 */
static void c7100_flash_parser_init (void)
{
    if (!system_loading) {
	/* verify file-id */
	parser_add_link_point(PARSE_ADD_VERIFY_FILE_CMD,
			      "verify file",
			      &pname(exec_verify_file_extend_here));

	/* copy flash ... */
	parser_add_link_point(PARSE_ADD_COPY_FLASH_CMD,
			      "copy flash ...",
			      &pname(exec_copy_flash_extend_here));
	parser_add_link_point(PARSE_ADD_COPY_FLASH_FILE_CMD,
			      "copy flash ...",
			      &pname(exec_copy_flash_file_extend_here));
    }

    parser_add_command_list(system_flash_chain_table, "system flash");
}

static char *default_flash_device (void)
{
    return DEFAULT_DEV_ID;
}

static void flash_init (subsystype *subsys)
{
    c7100_flash_parser_init();

    reg_add_default_flash_device(default_flash_device, "default flash device");

    if (!system_loading) {
	reg_add_flash_print_hardware(flash_print_hardware,
	    "flash_print_hardware");

	c7100_flashmib_init();
    }

    flash_config_init();	/* Init flash config */
    init_flash_command();	/* Init flash file system commands */

    reg_add_flash_alias_match(flash_alias_match, "flash_alias_match");
}

/****************************************************************************
 * Name: rsp_internal_flash_show_info_fn
 *
 * Description: show info function for C7100 internal flash
 * 
 * Input:
 *   dev_num -- the number of the device
 *
 * Return Value:
 *   rc -- return code: FSLIB_OK if successful, otherwise an error code
 *
 ****************************************************************************/
int rsp_internal_flash_show_info_fn (int dev_num)
{
    int rc;
    ushort flash_simm_reg, bank_size;
    ulong cmd;
    int cmd_actual;
    ulong status;
    int status_actual;
    struct {
	ulong mfg_ids;
	ulong model_nums;
    } id_codes;
    int id_actual;
    int bank;
    ulong bank_base;
    int num_banks;

    rc = FSLIB_OK;

    /*
     * Initialize the device (may not be necessary, but won't hurt)
     */
    FSLIB_ralib->device_init(dev_num);

    /*
     * Read the RSP flash SIMM register and interpret the value
     */
    printf("\n******** C7100 Internal Flash Bank -- Intel Chips ********\n");
    flash_simm_reg = C7100_IO_FPGA->flash_simm_reg;
    printf("Flash SIMM Reg: %x\n", flash_simm_reg);

    if (flash_simm_reg & FSIMMA) {
	printf("  Flash SIMM NOT PRESENT\n");
	return(rc);
    }

    printf("  Flash SIMM PRESENT\n");
    if (flash_simm_reg & FBANK2A) {
	printf("  1 Bank\n");
	num_banks = 1;
    } else {
	printf("  2 Banks\n");
	num_banks = 2;
    }
    printf("  Bank Size = ");
    bank_size = (flash_simm_reg & FLSH_SZ_MASK) >> FLSH_SZ_SHIFT;
    switch (bank_size) {
	case FLSH_SZ_1MB:  printf("1M\n");  break;
	case FLSH_SZ_2MB:  printf("2M\n");  break;
	case FLSH_SZ_4MB:  printf("4M\n");  break;
	case FLSH_SZ_8MB:  printf("8M\n");  break;
	case FLSH_SZ_16MB: printf("16M\n"); break;
	case FLSH_SZ_32MB: printf("32M\n"); break;
	default:           printf("UNKNOWN\n"); break;
    }
    printf("  HW Rev = %d\n", flash_simm_reg & HWREV_MASK);

    /* 
     * Now get the info from the chips for each bank
     */
    for (bank = 0; bank < num_banks; bank++) {

	bank_base = RSPFLASH_BANK_SIZE * bank;

	/*
	 * Common memory status registers
	 */
	printf("\nFlash Status Registers: Bank %d\n",bank);
	
	/*
	 * read the intelligent identifier codes
	 */
	printf("  Intelligent ID Code  : ");
	cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_INTELLIGENT_ID);
	cmd_actual = FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
	if (cmd_actual != sizeof(cmd)) {
	    printf("ERROR WRITING ID COMMAND\n");
	    rc = FSLIB_ERROR;
	} else {
	    id_actual = FSLIB_ralib->read(dev_num,bank_base,&id_codes,
					  sizeof(id_codes));
	    if (id_actual != sizeof(id_codes)) {
		printf("ERROR READING IID CODE\n");
		rc = FSLIB_ERROR;
	    } else {
		printf("%lx %lx\n",id_codes.mfg_ids, id_codes.model_nums);
		if (id_codes.mfg_ids != 0x89898989) {
		    printf("    IID Not Intel\n");
		    continue;
		}
	    }
	}
	
	printf("  Status Reg: ");
	cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_READ_STATUS_REG);
	cmd_actual = FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
	if (cmd_actual != sizeof(cmd)) {
	    printf("ERROR WRITING READ STATUS REG COMMAND\n");
	    rc = FSLIB_ERROR;
	} else {
	    status_actual = FSLIB_ralib->read(dev_num,bank_base,
					      &status,sizeof(status));
	    if (status_actual != sizeof(status)) {
		printf("ERROR READING STATUS\n");
		rc = FSLIB_ERROR;
	    } else {
		printf("%lx\n",status);
	    }
	}
	cmd = RSPFLASH_CMD_32(RSPFLASH_CMD_READ_ARRAY);
	FSLIB_ralib->write(dev_num,&cmd,bank_base,sizeof(cmd));
    }
    return(rc);
}

/*
 * Flash hardware subsystem header
 */
#define FLASH_MAJVERSION	1
#define FLASH_MINVERSION	0
#define FLASH_EDITVERSION	1

SUBSYS_HEADER(flash_devices, FLASH_MAJVERSION, FLASH_MINVERSION,
              FLASH_EDITVERSION, flash_init, SUBSYS_CLASS_DRIVER,
              "seq: les_flash_driver, les_flash_file_system, flash_file_system",
              NULL);

/* end of file */
