/* $Id: mc.h,v 3.1 1995/11/09 09:09:15 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c3000/mc.h,v $
 *------------------------------------------------------------------
 * mc.h - Defines needed for flash support
 *
 * April 1992, Rob Clevenger
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mc.h,v $
 * Revision 3.1  1995/11/09  09:09:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:15:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.2  1995/01/16 18:43:00  shj
 * CSCdi28716:  Eliminate duplicate copyright notices
 *
 * Revision 1.1  1993/09/24  16:33:30  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.2  1992/08/31  20:38:02  dcoli
 * Changes for Brut.
 *
 * Revision 1.1  1992/05/12  22:36:01  clev
 * - Flash support for Pan Monitor.
 *
 * Revision 1.1  1992/03/09  08:38:08  hampton
 * Initial conversion from RCS to CVS.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * mc.h -- Definitions for the MC and MC+ cards.
 *
 *	18-December-1990, Chris Shaker
 *      April 1991, Steve Elias
 *
 */

typedef struct flash_header {

    ulong   magic;
    ulong   len;
    ushort  checksum;
/*    char    fn[64];*/
} flash_header;

/*
 * Manufacturer codes table:
 */

struct prom_table {
    ushort manu_dev_code;	/* Manufacturer and device code */
    long device_size;		/* Size of device (bytes) */
    char *name;			/* Name string */
};

struct prom_data {
    ushort manu_dev;		/* Manufacturer/Device code */
    char *location;		/* Socket location name string */
};

#define FLASHMAGIC 0xABBADABA /* ...DOO!!! */

#define FLASH_MAGIC                     0x12345679L

#define MAX_RETRIES 20 

#define	COMPRESSED_FLASH_IMAGE_MAGIC    0xDEAF
#define UNCOMPRESSED_FLASH_IMAGE_MAGIC  0xBABE

#define MC_TEST_PATTERN		0x89ABCDEFL

#define MC_NVRAM_ADDRESS	0x0		/* NVRAM base address */
#define MC_NVRAM_SIZE		32768		/* 32K bytes NV memory */ 
#define MC_VPP_CONTROL		0x7fe000	/* MC+ VPP Control register */
#define MC_ROM_ADDRESS		0x800000
#define MC_ROM0			MC_ROM_ADDRESS
#define MC_ROM8			0xC00000	/* ROM8 */
#define MAX_ROMS		16
#define MAX_ROM_MFGS	3

/*
 * Only one bit is defined in the CPP control register on the MC+:
 */
#define MC_VPP_ON		0x0001
#define MC_VPP_OFF		0x0000
#define MC_VPP_MASK		0x0001

/*
 * Definitions for MC tests:
 */
#define MC_MAXIN		10
#define MC_DEF_TIMES		1
#define MC_DEF_START		0x0
#define MC_DEF_LEN		0xE00000
#define MC_DEF_DATA		0x1234


/*
 * Intel Flash EPROM command codes:  reverse em for Merlot!
 */
#define notREAD_MEMORY	        0
#define notREAD_ID_CODES	0x90
#define notREAD_STAT_REG	0x70
#define notCLEAR_STAT_REG	0x50
#define notERASE		0x20
#define notERASE_CONFIRM	0xD0
#define notERASE_VERIFY	        0xA0
#define notERASE_SUSPEND        0xB0
#define notERASE_RESUME         0xD0
#define notPROGRAM		0x40
#define notBYTE_WRITE		0x40
#define notALT_BYTE_WRITE	0x10
#define notPROGRAM_VERIFY	0xC0
#define notRESET		0xFF

/* eprom command codes -- reversed for merlot */
#define READ_MEMORY	0
#define READ_ID_CODES	0x9
#define READ_STAT_REG	0xE
#define CLEAR_STAT_REG	0xA
#define ERASE		0x4
#define ERASE_CONFIRM	0xB
#define ERASE_VERIFY	0x5
#define ERASE_SUSPEND   0xD
#define ERASE_RESUME    0xB
#define PROGRAM		0x2
#define BYTE_WRITE	0x2
#define ALT_BYTE_WRITE	0x8
#define PROGRAM_VERIFY	0x3
#define RESET		0xFF

/*
** Status bit defines for the new Intel 28F008SA chips
** These are reverse bitswapped for Pan
*/
#define WR_STAT_RDY     0x1
                        /* actually 0x80 */
#define ERA_SUSP_STAT   0x2
                        /* actually 0x40 */
#define ERASE_STAT      0x4
                        /* actually 0x20 */
#define BYTE_WR_STAT    0x8
                        /* actually 0x10 */
#define VOLTAGE_STAT    0x10
                        /* actually 0x08 */

/*
 * Prototypes:
 */
#ifdef GEORGE
ushort mc_read_short(uint);
void mc_write_short(ushort, uint);
uint mc_read_long(uint);
void mc_write_long(uint, uint);
long mc_read(uint, mempointer, long);
long mc_write(mempointer, uint, long);
char *identify_mc_card(void);
long mc_get_rom_size(ushort);
char *mc_get_rom_name(ushort);
void mc_query_roms(void);
boolean mc_zero_roms(void);
boolean mc_erase_roms(void);
boolean mc_program_rom(ushort, uint);
void mc_init(void);
int flash_to_mem(uchar *bufp, ulong *sizep);
void mc_assert_vpp(void);
void mc_deassert_vpp(void);
void mc_show(void);
void mc_nvram_test(void);
void mc_flash_test(void);
void mc_read_test(void);
void mc_write_test(void);
void mc_vpp_test(void);
void tftp2flash(void);
void flash2tftp(void);
void tftp2nvram(void);
void nvram2tftp(void);
void usecdelay(ushort);
boolean tftp_wflash(ipaddrtype);
boolean tftp_wnvram(ipaddrtype);
ulong mc_nvsize(void);
ushort add_to_cksum(ushort new_word, ushort old_cksum);
int badpart(int address, ushort expected);

int nv_dvr_install();
int nv_dvr_init();
int nv_dvr_open();
int nv_dvr_close();
int nv_dvr_ioctl(int, int, char *);
int nv_dvr_read(int, char *, int);
int nv_dvr_write(int, char *, int);

int fl_dvr_install();
int fl_dvr_init();
int fl_dvr_open();
int fl_dvr_close();
int fl_dvr_ioctl(int, int, char *);
int fl_dvr_read(int, char *, int);
int fl_dvr_write(int, char *, int);


#endif GEORGE

















