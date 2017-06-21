/* $Id: mc.h,v 3.1.58.1 1996/05/09 13:54:27 rbadri Exp $
 * $Source: /release/112/cvs/Xboot/src-68-alc/mc.h,v $
 *------------------------------------------------------------------
 * mc.h -- Synergy ATM line card defines needed for flash support
 *	   (based on c3000)
 *
 * April 13, 1995, Rick Lowe
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mc.h,v $
 * Revision 3.1.58.1  1996/05/09  13:54:27  rbadri
 * Branch: California_branch
 * Synergy features to the mainline.
 *
 * Revision 3.1.88.1  1996/04/27  06:13:25  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.78.1  1996/04/08  01:34:56  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.64.1  1996/03/22  09:07:21  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.50.1  1996/03/03  21:19:21  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.48.1  1996/03/02  01:07:20  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.32.1  1996/02/29  07:42:21  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.1  1995/11/09  09:08:53  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1.62.1  1996/02/03  06:11:58  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.1.44.1  1995/09/11  01:39:53  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.1.28.1  1995/08/22  18:18:48  lcheung
 * move files from synergy_atmlc_branch to synalc2_branch
 * Branch: Synalc2_branch
 *
 * Revision 2.1  1995/06/07  19:15:16  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.1  1995/04/19  02:28:52  rlowe
 * Merge Synergy ATM line card code (from 10.2) into IOS 10.3.
 *
 * Branch: Synergy_ATMlc_branch
 *
 * Revision 1.1  1995/04/14 05:22:07  rlowe
 * Placeholders for IOS port to Synergy ATM line card.
 *
 *------------------------------------------------------------------
 * Revision 1.1  1995/02/01  10:59:56  rlowe
 * Ram's initial merge of ALC IOS-Boot code (from older 10.2 tree).
 *
 * Revision 1.1  1995/01/11  23:04:37  rlowe
 * Initial port of IOS Boot to ACP.  Got Monitor up.
 *
 * Revision 1.1.1.1  1994/08/31  22:16:55  rahuja
 * IPS ver 10.2 snapshot
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

















