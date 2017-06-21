/* $Id: flash_dvr_les.h,v 3.2.60.1 1996/08/07 08:58:02 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_dvr_les.h,v $
 *------------------------------------------------------------------
 * flash_dvr_les.h - Low end systems Flash driver.
 *
 * Feb 1994, Ashwin Baindur
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: flash_dvr_les.h,v $
 * Revision 3.2.60.1  1996/08/07  08:58:02  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:00:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/24  19:15:38  vnguyen
 * CSCdi37804:  flash simms based on different chips can not be mixed
 * Changes made to support the mix of simms made from different chips.
 *
 * Revision 2.1  1995/06/07  20:24:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __FLASH_DVR_LES_H__
#define __FLASH_DVR_LES_H__

#define MAX_CMND_SETS 3

#define WRITE_TRIES 25
#define ZERO_WRITE_TRIES 2000
#define ERASE_TRIES 2000

#define READY_LOOP_COUNT_008 10000
#define READY_LOOP_COUNT_040 10000

enum chip_type {
    CHIP_TYPE_FLASH_ROM,
    CHIP_TYPE_RAM,
    CHIP_TYPE_ROM
    };

#define TYPE_FLASH_ROM 0
#define TYPE_RAM 1

enum flash_status {
    FR_NOT_QUERIED,
    FR_NOT_VALID,
    FR_READ_ONLY,
    FR_READ_WRITE,
    FR_RAM,
    FR_REMOVED
    };

/* some run-time option control */
#define BUG(a)  if (a & bugflags) {
#define BUGELSE } else {
#define BUGEND  }

/* control flags for use in above macros */
#define BF_CACHE   0x0001   /* disable data cache */
#define BF_MASK    0x0002   /* use command masking */ 
#define BF_WRTRIES 0x0004
#define BF_BOOT    0x0008
#define BF_PROBE   0x0010
#define BF_ERASE   0x0020
#define BF_VERIFY  0x0040

#define FLASH_GEOM struct flash_geom
typedef FLASH_GEOM {
    ushort  fg_status;       /* query status */
    ushort  fg_chips;        /* total number of chips in bank */
    ulong   fg_chipsize;     /* size of one flash rom (bytes) */
    ulong   fg_banksize;     /* size of each bank (in bytes)  */
    ulong   fg_bank_end;     /* bank end addr relative to device start */
    ulong   fg_sectorsize;   /* erase sector size in bytes */
    ushort  fg_sectors;      /* no of sectors in a chip */
    ushort  fg_cmnd_set;     /* command set to be used */
    ushort  fg_width;        /* Width of a bank in # of bytes */
    ushort  fg_chip_width;   /* Width of a chip in # of bytes */
    ulong   fg_wmask;        /* width mask  */
    ushort  fg_usec6;        /* count for 6 usec delay */
    ushort  fg_usec10;       /* count for 10 usec delay */
    ushort  fg_wtries;       /* write retries */
    ushort  fg_etries;       /* erase retries */
    ushort  fg_start_chip;   /* Starting physical chip number */
    ulong   fg_write_errors[MAX_CHIPS_IN_BANK]; /* count of write errors */
    ulong   fg_erase_errors[MAX_CHIPS_IN_BANK]; /* count of erase errors */
    ushort  fg_tbl_index[MAX_CHIPS_IN_BANK];    /* flash_info table index */
    ushort  fg_chip_code[MAX_CHIPS_IN_BANK];    /* manuf and chip code */
    uchar   *fg_chip_id[MAX_CHIPS_IN_BANK];     /* manuf and chip id as
						   in data sheets */
    char    *fg_chip_name[MAX_CHIPS_IN_BANK];   /* name of device chips */
    ushort  fg_rom_id[MAX_CHIPS_IN_BANK];       /* binary form of chip_id */
    ulong   fg_bank_no;	     /* bank number */
    char    *fg_card;        /* MC+, Clean, IGS */
    char    *fg_ctlr;        /* ENVM or MCI */
    /* chip specific write function */
    boolean (*fg_program)(dev_info_t *, ulong, volatile ulong *, ulong); 
    /* long aligned fast write */
    boolean (*fg_fast_program)(dev_info_t *, ulong, volatile ulong *); 
    /* chip specific erase function */
    ulong   (*fg_erase)(dev_info_t *, uchar *, ulong);   
    ulong   (*fg_read)(dev_info_t *, uchar *, ulong);
    FLASH_GEOM *next_geom;	       /* pointer to the next bank */
} flash_geom_t;

/*
 * Manufacturer codes table:
 */

typedef struct {
    ushort chip_code;     /* Manufacturer and device code(ordered logically) */
    ushort rev_chip_code; /* Manufacturer and device code(reversed) */
    char *manu_chip_code; /* as in the data sheets */
    ulong chip_size;	 /* Size of device (bytes) */
    ulong sector_size;	 /* size of an erase sector */
    ushort cmnd_set;	 /* command set type (0=old, 1=new) */
    boolean disp_flag;   /* whether entry should be displayed */
    char *chip_name;	 /* Name of chip */
} flash_info_t;

#define FLASH_TABLE_END	0xFFFF

extern const flash_info_t flash_info_tbl[];

/*----------- Declaration of command sets ---------------------------------*/
/*  Each command set has provision for upto 10 commands.
 *  First command set is for old flash chips (upto 4Mbits)
 *  Second command set is for new flash chips (8Mbits)
 *  Reset and Read-ID commands have been excluded from the
 *    command set because these are assumed to be common to
 *    all chips (without a Read-ID, how do we know what type
 *    of flash chips we have ??)
 * The sequence of commands is :
 *    Read, Erase-setup, Erase-cnfrm, Erase-verify, Write, Write-verify, 
 *    Read-sts, Clear-sts, Erase-suspend, Erase-resume
 *  Note that values are stored in bit reversed form for low-end platforms.
 */

enum COMMAND_SET {
 INVALID_SET,
 INTEL_010,           /* AMD/INTEL 28F010, 28F020 */ 
 INTEL_008,            /* INTEL 8mbit with status return, sector erasable */
 AMD_040,              /* AMD 28F040 */
 ANY_TYPE_OF_RAM
 };

/*------------ Defines for commands ---------------------------------------*/

/*
 * Flash EPROM command codes:
 * moved to platform specific files (sierra_flash.h, xx_flash.h, c3000_flash.h
 */

#define FLASH_COMMAND(a)     (a | (a << 8) | (a << 16) | (a << 24))

#define reset_cmd_010       FLASH_COMMAND(INTEL010_RESET_CMD)
#define read_id_010         FLASH_COMMAND(INTEL010_READ_ID)
#define erase_cmd_010       FLASH_COMMAND(INTEL010_ERASE_CMD)
#define erase_verify_010    FLASH_COMMAND(INTEL010_ERASE_VERIFY)
#define read_mem_010        FLASH_COMMAND(INTEL010_READ_CMD)
#define program_mem_010     FLASH_COMMAND(INTEL010_PROGRAM)
#define program_verify_010  FLASH_COMMAND(INTEL010_PROGRAM_VERIFY)

/* INTEL 008 commands */

#define reset_cmd_008       FLASH_COMMAND(INTEL008_RESET_CMD)
#define read_id_008         FLASH_COMMAND(INTEL008_READ_ID)
#define erase_mem_008       FLASH_COMMAND(INTEL008_ERASE_CMD)
#define erase_cnf_008       FLASH_COMMAND(INTEL008_ERASE_CNFRM)
#define read_mem_008        FLASH_COMMAND(INTEL008_READ_CMD)
#define program_mem_008     FLASH_COMMAND(INTEL008_PROGRAM)
#define read_status_008     FLASH_COMMAND(INTEL008_READ_STS)
#define clear_status_008    FLASH_COMMAND(INTEL008_CLEAR_STS)
#define erase_susp_008      FLASH_COMMAND(INTEL008_ERASE_SUSP)
#define erase_resm_008      FLASH_COMMAND(INTEL008_ERASE_RESUME)

/* AMD 040 commands */
#define reset_cmd_040       FLASH_COMMAND(AMD040_RESET_CMD)
#define read_id_040         FLASH_COMMAND(AMD040_READ_ID)
#define erase_chip_040      FLASH_COMMAND(AMD040_CHIP_ERASE_CMD)
#define erase_chip_cnf_040  FLASH_COMMAND(AMD040_CHIP_ERASE_CNFM)
#define erase_sect_040      FLASH_COMMAND(AMD040_SECT_ERASE_CMD)
#define erase_sect_cnf_040  FLASH_COMMAND(AMD040_SECT_ERASE_CNFRM)
#define read_mem_040        FLASH_COMMAND(AMD040_READ_CMD)
#define program_mem_040     FLASH_COMMAND(AMD040_PROGRAM)
#define ADR_040_5555        (0x15554 / 4)
#define ADR_040_2AAA        (0xAAA8 / 4)
#define ADR_040_5555_VAL    FLASH_COMMAND(AMD040_UNLOCK_5555)
#define ADR_040_2AAA_VAL    FLASH_COMMAND(AMD040_UNLOCK_AAAA)

/* 
 * Generic commands.  Work on both chip sets, and are needed
 * before the chip set type is known to read ids.
 */
#define reset_cmd       FLASH_COMMAND(FLASH_RESET_CMD)
#define read_id         FLASH_COMMAND(FLASH_READ_ID)

extern void cache_flush(void);
extern void flash_bank_query_wrapper (dev_info_t *, ulong, ulong *,ulong *);
extern int flash_chip_identify (ushort);
extern ushort flash_probe_memory_type (dev_info_t *);
extern ulong flash_chip_size (int);



#endif /* __FLASH_DVR_LES_H__ */
