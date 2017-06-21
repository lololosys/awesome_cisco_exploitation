/* $Id: flash.h,v 3.2.62.1 1996/06/01 02:45:37 clev Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/flash.h,v $
 *------------------------------------------------------------------
 * flash.h
 *
 * March 1993, Rob Clevenger
 *
 * Copyright (c) 1994 - 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Defines for the universal flash routines.
 *------------------------------------------------------------------
 * $Log: flash.h,v $
 * Revision 3.2.62.1  1996/06/01  02:45:37  clev
 * CSCdi55487:  Add support for AMD 008 and 016 flash SIMMs
 * Branch: California_branch
 *
 * Fujitsu chips too.
 *
 * Revision 3.2  1995/11/17  18:41:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:05:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:37:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FLASH_H__
#define __FLASH_H__

/*
** Flash id structure.
** Used in the flash device table and the flash driver structure.
*/

struct flash_id {
    unsigned short dev_code;    /* manufacturer and device code as in manual */
    unsigned short r_dev_code;  /* man and dev code as we see it */
    unsigned int dev_size;      /* the size of a single device */
    unsigned int sector_size;   /* the size of an erase sector */
    char *dev_string;           /* the device name string */
    unsigned int flags;         /* flag field (see below) */
};

/* Flags for above flag field */
#define FI_STATREG 1            /* this flash part has a status register */
#define AMD_STATREG 0x80        /* this flash part is an AMD with a status register */

/*
** Flash driver structure.
** Used by all of the flash driver routines.
*/

struct flash_dvr_info {
    struct flash_id *fiptr;      /* pointer to flash_id structure */
    long flash_base;             /* base address of flash on the platform */
    unsigned int flash_width;    /* width of the flash bus */
    unsigned int flash_size;     /* total flash size in bank */
    unsigned int flash_banks;    /* number of flash banks on the platform */
    unsigned int flash_sector;   /* size of flash sectors in this group */
    unsigned long flash_mask;    /* flash width mask */
    unsigned int flash_flags;    /* flag field (for future use) */
    struct flash_dvr_info *next; /* for hardware designs with noncontiguous banks */
};

/*
** Flash command codes.
** These commands are bit reversed for cisco 68k products. On R4k based
** products, the flash memory is wired up correctly.
*/
#define R_READ_MEMORY	  0
#define R_READ_ID_CODES	  0x9
#define R_READ_STAT_REG	  0xE
#define R_CLEAR_STAT_REG  0xA
#define R_ERASE		  0x4
#define R_ERASE_CONFIRM	  0xB
#define R_ERASE_VERIFY	  0x5
#define R_ERASE_SUSPEND   0xD
#define R_ERASE_RESUME    0xB
#define R_PROGRAM	  0x2
#define R_BYTE_WRITE	  0x2
#define R_ALT_BYTE_WRITE  0x8
#define R_PROGRAM_VERIFY  0x3
#define R_RESET		  0xFF

/*
** Status bit defines for the new Intel 28F008SA chips
** These are reverse bitswapped.
*/
#define R_WR_STAT_RDY     0x1  /* actually 0x80 */
#define R_ERA_SUSP_STAT   0x2  /* actually 0x40 */
#define R_ERASE_STAT      0x4  /* actually 0x20 */
#define R_BYTE_WR_STAT    0x8  /* actually 0x10 */
#define R_VPP_STAT        0x10 /* actually 0x08 */

/*
** Flash command codes (correct bit ordering for R4k based products
**/
#define F_READ_MEMORY	  0x00
#define F_READ_ID_CODES	  0x90
#define F_READ_STAT_REG	  0x70
#define F_CLEAR_STAT_REG  0x50
#define F_ERASE		  0x20
#define F_ERASE_CONFIRM	  0xD0
#define F_ERASE_VERIFY	  0xA0
#define F_ERASE_SUSPEND   0xB0
#define F_ERASE_RESUME    0xD0
#define F_PROGRAM	  0x40
#define F_BYTE_WRITE	  0x40
#define F_ALT_BYTE_WRITE  0x10
#define F_PROGRAM_VERIFY  0xC0
#define F_RESET		  0xFF

/*
** Status bit defines for the new Intel 28F008SA chips
** These are ordered correctly for R4k systems
*/
#define F_WR_STAT_RDY     0x80
#define F_ERA_SUSP_STAT   0x40
#define F_ERASE_STAT      0x20
#define F_BYTE_WR_STAT    0x10
#define F_VPP_STAT        0x08

#ifndef __mips

/*
 * Ordered backwards
 */
#define C_READ_MEMORY	  R_READ_MEMORY	  
#define C_READ_ID_CODES	  R_READ_ID_CODES	  
#define C_READ_STAT_REG	  R_READ_STAT_REG	  
#define C_CLEAR_STAT_REG  R_CLEAR_STAT_REG  
#define C_ERASE		  R_ERASE		  
#define C_ERASE_CONFIRM	  R_ERASE_CONFIRM	  
#define C_ERASE_VERIFY	  R_ERASE_VERIFY	  
#define C_ERASE_SUSPEND   R_ERASE_SUSPEND   
#define C_ERASE_RESUME    R_ERASE_RESUME    
#define C_PROGRAM	  R_PROGRAM	  
#define C_BYTE_WRITE	  R_BYTE_WRITE	  
#define C_ALT_BYTE_WRITE  R_ALT_BYTE_WRITE  
#define C_PROGRAM_VERIFY  R_PROGRAM_VERIFY  
#define C_RESET		  R_RESET		  
#define C_WR_STAT_RDY     R_WR_STAT_RDY     
#define C_ERA_SUSP_STAT   R_ERA_SUSP_STAT   
#define C_ERASE_STAT      R_ERASE_STAT      
#define C_BYTE_WR_STAT    R_BYTE_WR_STAT    
#define C_VPP_STAT        R_VPP_STAT        

#else /* __mips */

/*
 * Ordered the correct way
 */
#define C_READ_MEMORY	  F_READ_MEMORY	  
#define C_READ_ID_CODES	  F_READ_ID_CODES	  
#define C_READ_STAT_REG	  F_READ_STAT_REG	  
#define C_CLEAR_STAT_REG  F_CLEAR_STAT_REG  
#define C_ERASE		  F_ERASE		  
#define C_ERASE_CONFIRM	  F_ERASE_CONFIRM	  
#define C_ERASE_VERIFY	  F_ERASE_VERIFY	  
#define C_ERASE_SUSPEND   F_ERASE_SUSPEND   
#define C_ERASE_RESUME    F_ERASE_RESUME    
#define C_PROGRAM	  F_PROGRAM	  
#define C_BYTE_WRITE	  F_BYTE_WRITE	  
#define C_ALT_BYTE_WRITE  F_ALT_BYTE_WRITE  
#define C_PROGRAM_VERIFY  F_PROGRAM_VERIFY  
#define C_RESET		  F_RESET		  
#define C_WR_STAT_RDY     F_WR_STAT_RDY     
#define C_ERA_SUSP_STAT   F_ERA_SUSP_STAT   
#define C_ERASE_STAT      F_ERASE_STAT      
#define C_BYTE_WR_STAT    F_BYTE_WR_STAT    
#define C_VPP_STAT        F_VPP_STAT        

#endif /* __mips */

/*
** Commands for the new AMD 29F016 flash chips.
** Both reversed and normal commands are represented here for
** platforms with flash chips wired backwards.
** THESE ONLY WORK ON 32 BIT WIDE FLASH
*/
#define AMD_FLASH_CMD_ADDR1  (0x15554 / 4)
#define AMD_FLASH_CMD_ADDR2  (0xAAA8 / 4)
#ifdef __mips
#define AMD_FLASH_CMD_CODE1  0xaaaaaaaa
#define AMD_FLASH_CMD_CODE2  0x55555555
#define AMD_FLASH_RESET      0xf0f0f0f0
#define AMD_FLASH_AUTOSELECT 0x90909090
#define AMD_FLASH_PROGRAM    0xa0a0a0a0
#define AMD_FLASH_ERASE1     0x80808080
#define AMD_FLASH_ERASE_CHIP 0x10101010
#define AMD_FLASH_ERASE_SCTR 0x30303030
#define AMD_DQ7_MASK         0x80808080
#else  /* reverse order */
#define AMD_FLASH_CMD_CODE1  0x55555555
#define AMD_FLASH_CMD_CODE2  0xaaaaaaaa
#define AMD_FLASH_RESET      0x0f0f0f0f
#define AMD_FLASH_AUTOSELECT 0x09090909
#define AMD_FLASH_PROGRAM    0x05050505
#define AMD_FLASH_ERASE1     0x01010101
#define AMD_FLASH_ERASE_CHIP 0x08080808
#define AMD_FLASH_ERASE_SCTR 0x0c0c0c0c
#define AMD_DQ7_MASK         0x01010101
#endif

#define MAXFLASHSETS  8
#define MAXFLASHBANKS 16
#define MAXFLASHWRITE 25

#ifndef UNIX
/* flash.c */
extern int get_flash_info(struct flash_dvr_info *fdiptr);
extern unsigned long poll_flash_stat(volatile unsigned char *addr, unsigned long stat, int size, unsigned long mask);
extern int program_flash(volatile unsigned char *location, long data, struct flash_dvr_info *fdiptr);
extern struct flash_dvr_info *get_fdi_struct(struct flash_dvr_info *fdiptr, unsigned start);
extern int flash_cleanup();
extern int erase_flash(struct flash_dvr_info *fdiptr, unsigned start, int size);
extern int flash_test(int testlength);
extern int ftest(struct flash_dvr_info *fdiptr, unsigned long *valptr, int numvals, char *nameptr);
extern int flash(int argc, char *argv[]);
extern int bootflash_addr(long *addrptr, int *fcc_install);
extern unsigned get_flash_base(void);
extern long get_flash_size(int addr_set);
extern struct bitdef flash_stat[];
#endif

#endif  /* __FLASH_H__ */

