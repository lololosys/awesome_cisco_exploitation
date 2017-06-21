/* $Id: c7100_macaddr.h,v 3.1.68.3 1996/07/24 18:02:27 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_macaddr.h,v $
 *------------------------------------------------------------------
 * c7100_macaddr.h - Header for midplane EEPROM (contains MAC
 *		     MAC addr, Serial No, and envm stuff)
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_macaddr.h,v $
 * Revision 3.1.68.3  1996/07/24  18:02:27  mbeesley
 * CSCdi63873:  Top two port adaptor slots are optional
 * Branch: California_branch
 *
 * Revision 3.1.68.2  1996/06/11  14:27:43  dcarroll
 * CSCdi59664:  enable power supply fail interrupts
 * Branch: California_branch
 * Turn on power supply interrupts.
 *
 * Revision 3.1.68.1  1996/03/21  23:48:04  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/02/15  23:37:52  mbeesley
 * CSCdi49115:  MP EEPROM should conform to new spec
 * Branch: ELC_branch
 *
 * Revision 3.1.2.3  1996/01/23  02:18:13  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.1.2.2  1996/01/09  21:27:47  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.1.2.1  1995/11/22  21:43:16  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:45  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Chassis definitions
 */
#define C7100_NO_CHASSIS_MODEL	((uchar) -1)
#define C7100_FOUR_SLOT		4
#define C7100_SIX_SLOT		6

/*
 * Cookie defintions
 */
#define C7100_COOKIE_VERSION   	1

/*
 * DRAM copy of Midplane NMC93C46 contents
 */
typedef struct c7100_mpeeprom_copy_t_ {
    uchar eeprom_version;
    uchar model;
    uchar hardware_version_high;
    uchar hardware_version_low;
    uchar serial_number[4];
    uchar part_number[4];
    uchar mac_base[IEEEBYTES];
    uchar mac_block_size[2];
    uchar test_history;
    uchar RMA_number[3];
    uchar mfg_date_yy;
    uchar mfg_date_mm;
    uchar mfg_date_dd;
    uchar board_revision;
    uchar board_deviation[2];
    uchar board_config;
    uchar reserved[1];
} c7100_mpeeprom_copy_t;

/*
 * Externals
 */
extern uchar 	c7100_chassis_model;
extern uint 	c7100_chassis_serial;

/*
 * Prototypes
 */
extern void 	c7100_macaddr_init(void);
extern boolean	c7100_assign_mac_addr(hwidbtype *, uchar addr[IEEEBYTES]);
extern void 	test_c7100_mpeeprom(parseinfo *csb);
extern void	c7100_show_mpeeprom(void);
extern void     c7100_read_mpeeprom(c7100_mpeeprom_copy_t *);
extern boolean  c7100_read_mpeeprom_partial(void *, ushort, ushort);
extern boolean  c7100_write_mpeeprom_partial(void *, ushort, ushort);

/* end of file */
