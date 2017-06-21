/* $Id: fldvr.h,v 3.1 1995/11/09 09:10:15 shaker Exp $
 * $Source: /swtal/cherf.111/ios/boot/src-68-c7000/fldvr.h,v $
 *------------------------------------------------------------------
 * fldvr.h -- Definitions for the MC and MC+ cards.
 *
 *	18-December-1990, Chris Shaker
 *      April 1991, Steve Elias
 *
 *	Copyright (c) 1990-1993 by cisco Systems, Inc.
 *	All rights reserved.
 *------------------------------------------------------------------
 * $Log: fldvr.h,v $
 * Revision 3.1  1995/11/09  09:10:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  19:17:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1  1993/09/24 16:34:23  widmer
 * Rename boot obj directories to include CPU type and product family
 *
 * Revision 1.1  1993/01/30  01:11:50  whp4
 * Add RP1 support.
 *
 *
 * $Endlog$
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
#define READ_MEMORY     0
#define READ_ID_CODES	0x90
#define ERASE		0x20
#define ERASE_VERIFY    0xA0
#define PROGRAM		0x40
#define PROGRAM_VERIFY	0xC0
#define RESET		0xFF

/* eprom command codes -- reversed for merlot */
#define notREAD_MEMORY	0
#define notREAD_ID_CODES	0x9
#define notERASE		0x4
#define notERASE_VERIFY	0x5
#define notPROGRAM		0x2
#define notPROGRAM_VERIFY	0x3
#define notRESET		0xFF



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

















