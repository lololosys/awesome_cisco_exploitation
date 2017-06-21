/* $Id: flash_dvr_private.h,v 3.2.60.2 1996/08/28 12:40:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/dev/flash_dvr_private.h,v $
 *------------------------------------------------------------------
 * flash_dvr_private.h -- private definitions for common Flash driver
 *
 * 18-December-1990, Chris Shaker
 * April 1991, Steve Elias
 * Octoberfest-1991, Darren Kerr
 * March 1992, Dit Morse
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_dvr_private.h,v $
 * Revision 3.2.60.2  1996/08/28  12:40:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.60.1  1996/08/02  22:52:51  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:00:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:19:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:24:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 *------------------------------------------------------------------
 */

/* Define max no of command sets */

#define MAX_CMND_SETS 3

#define WRITE_TRIES 25
#define ZERO_WRITE_TRIES 2000
#define MAX_ERASE_TRIES 2000

#define MAX_RETRIES 20 

/* typedef ulong flash_offset; */

#define FLASH_ADDR(a) (flash_ptr)(FLASH_BASE + a)

/* these are chips "0", "1", ... in internal counts and tables */
/* correspond to bits 31..24, 23..16, 15..8, 7..0 */

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
 INTEL_008            /* INTEL 8mbit with status return, sector erasable */
 };

struct flash_info flash_info_tbl[] = {
  /* Chip-id      Rev. Chip-id    man-dev  Chip-sz    Erase-sect  Cmnd-set  
     Name  */
  {(ushort)0x0   , (ushort)0x0   , "0000", 0x000000L, 0x1L    , INVALID_SET, 
   "Unknown Chip"},
  {(ushort)0x89B4, (ushort)0x912D, "89B4", 0x020000L, 0x20000L, INTEL_010, 
   "INTEL 28F010"},
  {(ushort)0x89BD, (ushort)0x91BD, "89BD", 0x040000L, 0x40000L, INTEL_010, 
   "INTEL 28F020"},
  {(ushort)0x01A7, (ushort)0x80E5, "01A7", 0x020000L, 0x20000L, INTEL_010, 
   "AMD   28F010"},
  {(ushort)0x012A, (ushort)0x8054, "012A", 0x040000L, 0x40000L, INTEL_010, 
   "AMD   28F020"},
  {(ushort)0x1CD0, (ushort)0x380D, "1CD0", 0x040000L, 0x40000L, INTEL_010, 
   "M5M   28F101P"},
  {(ushort)0x89A2, (ushort)0x9145, "89A2", 0x100000L, 0x10000L, INTEL_008, 
   "INTEL 28F008SA"},
 };

#define MAX_ROM_MFGS (sizeof(flash_info_tbl)/sizeof(struct flash_info))
/*------------ Defines for commands ---------------------------------------*/


/*
 * Flash EPROM command codes:
 * as found in the data sheets, bit reversed as per low-end systems.
 * INTEL_010 command set found in platform dependent files.
 */
#ifndef __mips
#define INTEL008_READ_CMD        0xFF
#define INTEL008_ERASE_CMD	 0x04
#define INTEL008_ERASE_CNFRM	 0x0B
/*efine INTEL008_ERASE_VERIFY    0x05 */
#define INTEL008_PROGRAM	 0x02
/*efine INTEL008_PROGRAM_VERIFY  0x03 */
#define INTEL008_READ_STS        0x0E
#define INTEL008_CLEAR_STS       0x0A
#define INTEL008_ERASE_SUSP      0x0D
#define INTEL008_ERASE_RESUME    0x0B 
#define INTEL008_READ_ID	 0x09
#define INTEL008_RESET_CMD	 0xFF
#else
#define INTEL008_READ_CMD        0xFF
#define INTEL008_ERASE_CMD	 0x20
#define INTEL008_ERASE_CNFRM	 0xD0
/*efine INTEL008_ERASE_VERIFY    0xA0 */
#define INTEL008_PROGRAM	 0x40
/*efine INTEL008_PROGRAM_VERIFY  0xC0 */
#define INTEL008_READ_STS        0x70
#define INTEL008_CLEAR_STS       0x50
#define INTEL008_ERASE_SUSP      0xB0
#define INTEL008_ERASE_RESUME    0xD0
#define INTEL008_READ_ID	 0x90
#define INTEL008_RESET_CMD	 0xFF
#endif

/* AMD/INTEL 010,020 commands */

#define FLASH_COMMAND(a)     a | a << 8 | a << 16 | a << 24

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
/* #define erase_verify_008    FLASH_COMMAND(INTEL008_ERASE_VERIFY) */
#define read_mem_008        FLASH_COMMAND(INTEL008_READ_CMD)
#define program_mem_008     FLASH_COMMAND(INTEL008_PROGRAM)
/* #define program_verify_008  FLASH_COMMAND(INTEL008_PROGRAM_VERIFY) */
#define read_status_008     FLASH_COMMAND(INTEL008_READ_STS)
#define clear_status_008    FLASH_COMMAND(INTEL008_CLEAR_STS)
#define erase_susp_008      FLASH_COMMAND(INTEL008_ERASE_SUSP)
#define erase_resm_008      FLASH_COMMAND(INTEL008_ERASE_RESUME)


#define COMMAND(a,b,m) ((a&m) | (b&(~m)))

/* 
 * Generic commands.  work on both chip sets, and are needed
 * before the chip set type is known to read ids.
 */
#define reset_cmd       FLASH_COMMAND(FLASH_RESET_CMD)
#define read_id         FLASH_COMMAND(FLASH_READ_ID)

/*
 * Flash status values for INTEL 008 command set d bit reverse()
 */
#ifndef __mips
#define  FLASH_READY		0x01010101UL
#define  FLASH_ERASE_ERR	0x04040404UL
#define  FLASH_WRITE_ERR 	0x08080808UL
#define  FLASH_VPP_LOW		0x10101010UL
#else
#define  FLASH_READY		0x80808080UL
#define  FLASH_ERASE_ERR	0x20202020UL
#define  FLASH_WRITE_ERR 	0x10101010UL
#define  FLASH_VPP_LOW		0x08080808UL
#endif

/*
 * This is [written] to the header of flash the first
 * time it's programmed.  This enables software to
 * read the size and types of flash roms installed
 * after the security jumper is installed.
 * (The security jumper prevents VPP from being asserted,
 * so the ROM ID flash command would not work).
 */

#define FLASH_OPEN (DEV_FLAGS_EXCL | DEV_FLAGS_NDLY)

/*
 * flash_dvr.c
 */

ulong flash_align(ulong addr);
boolean run_from_flash(void);
ushort flash_query_roms(flash_dev_t *dev);
void query_bank(int bankaddr, ulong *manu, ulong *device);
boolean flash_erase_bank(flash_dev_t *dev, int bank);
ulong flash_read(uchar *buf, uchar *addr, uint len);
ulong flash_write(uchar *buf, uchar *addr, ulong len, boolean verbose);
boolean zero_bank(int bank);
boolean verify_zeroed(int bank);
void prt_gotexp(ulong got, ulong exp, ulong mask, ulong *addr);
ulong got_exp(ulong got, ulong exp, ulong mask);
void inc_ep(ulong mask, int bank);
void inc_wp(ulong mask, ulong *addr);
void prt_errors(int bank);
boolean prom_err(void);
void flash_print_chips_known(void);
boolean prom_quad_008(ulong data, volatile ulong *ptr);
boolean prom_008(ulong data, volatile ulong *ptr, ulong mask, int write_tries);
boolean flash_erase_008(flash_dev_t *dev, int bank);
boolean verify_erased_008(int bank);
boolean verify_flash_bank_008(int bank, ulong value);
boolean verify_flash_sector_008(int bank, int sector, ulong value, boolean verb);
boolean flash_erase_bank_008(flash_dev_t *dev, int bank);
boolean flash_erase_sector_008(flash_dev_t *dev, int bank, int sector);
boolean prom_quad_010(ulong data, volatile ulong *ptr);
boolean prom_010(ulong data, volatile ulong *ptr, ulong mask, int write_tries);
boolean flash_erase_bank_010(flash_dev_t *dev, int bank);
int flash_identify(ushort code);
char *flash_manu_code(uint i);
int flash_cmnd_set(uint i);
char *flash_name(uint i);
ulong flash_size(uint i);
int flash_sectors(void);
int flash_banks(void);
ulong flash_sector_size(uint i);
