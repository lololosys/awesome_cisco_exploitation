/* $Id: c7100_macaddr.c,v 3.1.66.5 1996/08/28 13:11:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-4k-c7100/c7100_macaddr.c,v $
 *------------------------------------------------------------------
 * c7100_macaddr.c - Manage ID EEPROM on midplane (holds MAC addr)
 *
 * October 1995, Michael Beesley
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: c7100_macaddr.c,v $
 * Revision 3.1.66.5  1996/08/28  13:11:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.66.4  1996/06/26  18:40:13  mbeesley
 * CSCdi61260:  Make output of show c7200 more usefull to manufacturing
 * Branch: California_branch
 *
 * Revision 3.1.66.3  1996/06/11  14:27:42  dcarroll
 * CSCdi59664:  enable power supply fail interrupts
 * Branch: California_branch
 * Turn on power supply interrupts.
 *
 * Revision 3.1.66.2  1996/04/06  02:05:41  ssangiah
 * CSCdi53822:  Need to deal with product name change.
 * Make changes to deal with the change in the product name for Predator
 * from c7100 to c7200.
 * Branch: California_branch
 *
 * Revision 3.1.66.1  1996/03/21  23:48:02  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.1.2.4  1996/01/23  02:18:12  mbeesley
 * Branch: ELC_branch
 * Add support for the CPU ID EEPROM. Postpone PCI SRAM initialization
 * until the rev of the CPU card has been read and verified.
 *
 * Revision 3.1.2.3  1996/01/09  21:27:46  mbeesley
 * Branch: ELC_branch
 * Enhance/fix Midplane EEPROM utility. Protect against left over
 * interrupts from boot helper image. Fix some gt64010 register naming
 * mistakes.
 *
 * Revision 3.1.2.2  1995/12/09  22:52:14  mbeesley
 * Branch: ELC_branch
 * Defeat OIR, ENV processes until hw is verified. Fix initial bugs:
 *         o NVRAM access must be 32 bit cycle, only using bits 7:0
 *         o Always create DRAM particle pool
 *         o Read 32 bytes, not 64 bytes from MP eeprom
 *         o Correct some misdefined hw addresses
 *         o Make packet SRAM usage on Rev 1 optional, with default as
 *           no until hw is verified (define C7100_CPU_SRAM to use it)
 *
 * Revision 3.1.2.1  1995/11/22  21:43:14  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 *
 * Revision 3.1  1995/11/21  01:37:44  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "logger.h"
#include "config.h"
#include "ieee.h"

#include "../pas/msg_pas.c"		/* Not a typo, see logger.h */
#include "../pas/if_pas.h"

#include "msg_c7100.c"			/* Not a typo, see logger.h */
#include "c7100_macaddr.h"
#include "c7100_fpga.h"
#include "../snmp/sr_old_lchassismib.h"

/*
 * public data
 */
uchar c7100_chassis_model;
uint  c7100_chassis_serial;

/*
 * MAC addr info
 */
static uint mac_count;
static uchar mac_base[IEEEBYTES];
static c7100_mpeeprom_copy_t mp_copy;

static pas_management_t c7100_mp_eeprom = {
    -1,						/* Not a bay */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    SK2_CLOCK_MIDPLANE,				/* Clock mask */
    &C7100_IO_FPGA->serial_eprom_ctrl,		/* Clock control register */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    CS2_CHIP_SEL_MIDPLANE,			/* Select mask */
    &C7100_IO_FPGA->serial_eprom_ctrl,		/* Select control register */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),   /* 8 bit register */
    DI2_DATA_IN_MIDPLANE,			/* DATAIN mask */
    &C7100_IO_FPGA->serial_eprom_ctrl,		/* DATAIN register */

    sizeof(C7100_IO_FPGA->serial_eprom_ctrl),	/* 8 bit register */
    DO2_DATA_OUT_MIDPLANE,			/* DATAOUT mask */
    &C7100_IO_FPGA->serial_eprom_ctrl		/* DATAOUT register */
};

/*
 * c7100_read_mpeeprom:
 * Read the EEPROM data
 */
void c7100_read_mpeeprom (c7100_mpeeprom_copy_t *eeprom)
{
    pas_access_sys_eeprom(&c7100_mp_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_READ, 
			  (sizeof(c7100_mpeeprom_copy_t) >> 1));
}

/*
 * c7100_write_mpeeprom:
 * Write the EEPROM data
 */
static void c7100_write_mpeeprom (c7100_mpeeprom_copy_t *eeprom)
{
    pas_access_sys_eeprom(&c7100_mp_eeprom, eeprom, PAS_EEPROM_NMC93C46,
			  PAS_EEPROM_CMD_WRITE, 0);

}

/*
 * c7100_read_mpeeprom_partial
 *
 * Read part of the midplane eeprom.
 *
 * The midplane eeprom is currently allocated like this:
 *
 *   0 --------------
 *     |            | 
 *     |  sys data  | c7100_mpeeprom_copy_t: MAC address, serial number, etc. 
 *     |            | 
 *  32 |------------|   
 *     |            | 
 *     |  env mon   | envm_stats_t:  "show env last" stats.
 *     |            | 
 *     |------------| 
 *  64 |            | 
 *           . 
 *           . 
 *           . 
 *
 *     |            |
 *     --------------
 */
boolean c7100_read_mpeeprom_partial (void * buf, ushort count, ushort offset)
{
    return pas_access_sys_eeprom_partial(&c7100_mp_eeprom,
                                         buf,
                                         PAS_EEPROM_NMC93C46,
			                 PAS_EEPROM_CMD_READ,
                                         count, 
                                         offset);
}

/*
 * c7100_write_mpeeprom_partial
 *
 * Write part of the midplane eeprom.
 *
 * The midplane eeprom is currently allocated like this:
 *
 *   0 --------------
 *     |            | 
 *     |  sys data  | c7100_mpeeprom_copy_t: MAC address, serial number, etc. 
 *     |            | 
 *  32 |------------|   
 *     |            | 
 *     |  env mon   | envm_stats_t:  "show env last" stats.
 *     |            | 
 *     |------------| 
 *  64 |            | 
 *           . 
 *           . 
 *           . 
 *     |            |
 *     --------------
 */
boolean c7100_write_mpeeprom_partial (void *buf, ushort count, ushort offset)
{
    return pas_access_sys_eeprom_partial(&c7100_mp_eeprom,
                                         buf,
                                         PAS_EEPROM_NMC93C46,
                                         PAS_EEPROM_CMD_WRITE,
                                         count, 
                                         offset);
}

/*
 * test_c7100_mpeeprom:
 * read & write the NMC93 Mid plane eeprom
 */
void test_c7100_mpeeprom (parseinfo *csb)
{
    uchar addr, value, *eeprom_ptr;
    c7100_mpeeprom_copy_t eeprom;

    /*
     * Read the complete eeprom
     */
    c7100_read_mpeeprom(&eeprom);
    eeprom_ptr = (uchar *)&eeprom;

    addr = GETOBJ(int,2);

    switch (GETOBJ(int,1)) {

    case TEST_C7100_EEPROM_READ:
	value = eeprom_ptr[addr];
	printf("Value: %02x", value);
	break;

    case TEST_C7100_EEPROM_WRITE:
	eeprom_ptr[addr] = GETOBJ(int,3);
	c7100_write_mpeeprom(&eeprom);
	break;
    }
}

/*
 * c7100_show_mpeeprom:
 * Print out the Midplane cookie
 */
void c7100_show_mpeeprom (void)
{
    uchar revision;
    ulong serial_number;
    uint part_number_mid;
    c7100_mpeeprom_copy_t c;

    /*
     * Get the data from the EEPROM
     */
    c7100_read_mpeeprom(&c);
    printf("\nC7200 Midplane EEPROM:");

    /*
     * Parse the data programmed into the EEPROM.
     */
    revision = c.board_revision;
    part_number_mid = (c.part_number[1] << 8) | c.part_number[2];
    serial_number = c.serial_number[0] << 24 |
                    c.serial_number[1] << 16 |
                    c.serial_number[2] << 8  |
                    c.serial_number[3];

    /*
     * Print out the fields
     */
    printf("\n\tHardware revision %d.%10d\t",
	   c.hardware_version_high, c.hardware_version_low);
    if (revision < 0x50 || revision == 0xff) {
        printf("Board revision UNKNOWN");
    } else {
        printf("Board revision %c%d", 'A' + ((revision - 0x50) / 8),
	       revision & 0x7);
    }
    printf("\n\tSerial number     %10lu\tPart number    %02u-%04u-%02u",
	   serial_number, c.part_number[0], part_number_mid,
	   c.part_number[3]);
    printf("\n\tTest history      0x%10x\tRMA number     %02d-%02d-%02d",
	   c.test_history, c.RMA_number[0], c.RMA_number[1], 
	   c.RMA_number[2]);
    printf("\n\tMAC=%e, MAC Size=%d", 
	   c.mac_base, (c.mac_block_size[0] << 8) + c.mac_block_size[1]);
    printf("\n\tEEPROM format version %d, Model=%#x", 
	   c.eeprom_version, c.model);

    /*
     * Dump the EEPROM contents in hex
     */
    c7100_dump_ideeprom((uchar *)&c);
}

/*
 * c7100_macaddr_init:
 * read in data (e.g. MAC addresses) from backplane EEPROM
 */
void c7100_macaddr_init (void)
{
    /*
     * Read the complete eeprom
     */
    c7100_read_mpeeprom(&mp_copy);

    if ((mp_copy.eeprom_version == C7100_COOKIE_VERSION) &&
	((mp_copy.mac_base[0] & 0x01) == 0)) {
	
	/*
	 * We have a valid EEPROM content
	 */
	ieee_copy(mp_copy.mac_base, mac_base);
	mac_count = GETSHORT(mp_copy.mac_block_size);
	c7100_chassis_model = mp_copy.model;
	c7100_chassis_serial = GETLONG(mp_copy.serial_number);

    } else {
	
	/*
	 * The EEPROM content was not valid. Make up a random
	 * MAC address and let the user know
	 */
	ulong base;

	do
	    base = getcp0_count_inline();
	while (base == 0);

	mac_count = MAX_PA_BAYS * MAX_INTERFACES_PER_PA;
	mac_base[0] = 0x00;
	PUTLONG(&mac_base[1], base);
	errmsg(&msgsym(COOKIE, C7200), mac_base);
	c7100_chassis_model = C7100_NO_CHASSIS_MODEL;
	c7100_chassis_serial = 0;
    }
}

/*
 * get_mac_addr:
 * assign a MAC address to a port in a slot
 */
static void get_mac_addr (uint slot, uint port, uchar *addr)
{
    ulong value;
    uchar byte2;
    uint offset;

    offset = (slot * MAX_INTERFACES_PER_PA) + port;
    if (offset >= mac_count) {
	errmsg(&msgsym(NOMAC, C7200), slot, port);
	return;
    }

    value = GETLONG(&mac_base[2]);
    byte2 = value >> 24;
    value += offset;
    if (byte2 != (value >> 24)) {
	errmsg(&msgsym(NOMAC, C7200), slot, port);
	return;
    }
    ieee_copy(mac_base, addr);
    PUTLONG(&addr[2], value);
}

/*
 * c7100_assign_mac_addr:
 * Assign a MAC address to an interface
 */
boolean c7100_assign_mac_addr (hwidbtype *hwidb, uchar addr[IEEEBYTES])
{
    get_mac_addr(hwidb->slot, hwidb->slotunit, addr);
    return(TRUE);
}
/* end of file */
