/* $Id: dec_ppb.c,v 3.7.10.5 1996/06/04 00:31:09 mbeesley Exp $
 * $Source: /release/112/cvs/Xsys/pas/dec_ppb.c,v $
 *------------------------------------------------------------------
 * dec_ppb.c - Config dec pci to pci bridge
 *
 * March 14 1995, Paulina Tran
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dec_ppb.c,v $
 * Revision 3.7.10.5  1996/06/04  00:31:09  mbeesley
 * CSCdi59364:  Need bridge specific control of bridge_control register
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/05/22  14:13:52  getchell
 * CSCdi57682:  PA0 not seen on some boards (when PA1 present)
 * Branch: California_branch
 *
 * Revision 3.7.10.3  1996/05/21  09:59:55  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.10.2  1996/04/02  22:55:08  mbeesley
 * CSCdi53300:  PA power-on sequence can leave bus interface off
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/21  23:22:38  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.5  1996/03/18  23:28:32  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.4  1996/01/19  20:03:42  mbeesley
 * Branch: ELC_branch
 * Make "show bridge" more usefull. Remove unused static inline
 * for AMDP2 driver, fix process level packet accounting.
 *
 * Revision 3.2.2.3  1996/01/16  01:42:06  dcarroll
 * Branch: ELC_branch
 * Sync ELC_branch between ELC_baseline_120995 and ELC_baseline_960110.
 *
 * Revision 3.2.2.2  1995/12/08  05:33:23  mbeesley
 * Branch: ELC_branch
 * Mini-sync ... get correct ID fields, and parse chains.
 *
 * Revision 3.2.2.1  1995/11/22  21:41:26  mbeesley
 * Initial commit of c7100 support.
 * Branch: ELC_branch
 * Revision 3.7  1996/02/22  14:37:09  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.6  1995/12/19  20:24:10  gstovall
 * CSCdi45859:  Need to remove unused cruft from pas drivers
 * Nuke extra includes, tidy up comments, remove useless code.
 *
 * Revision 3.5  1995/12/17  18:35:52  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.4  1995/12/09  21:57:11  mbeesley
 * CSCdi45202:  VIP/PCI parse chains need slight reorganization
 *
 * Revision 3.3  1995/12/01  01:59:55  mbeesley
 * CSCdi44698:  PCI subsystem needs improvement
 *
 * Revision 3.2  1995/11/17  18:53:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:57:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:21:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "interface_private.h"
#include "subsys.h"
#include "config.h"
#include "parser.h"

#include "../if/pci.h"
#include "../parser/parser_actions.h"
#include "../parser/macros.h"

#include "if_pas.h"
#include "dec_ppb.h"

#define	ALTERNATE	NONE
#include "dec_ppb_show_chain.h"
LINK_POINT (show_bridge_commands, ALTERNATE);
#undef	ALTERNATE

/*
 * Parser chains
 */
static parser_extension_request bridge_chain_init_table[] = {
    { PARSE_ADD_SHOW_PCI_CMD, &pname(show_bridge_commands) },
    { PARSE_LIST_END, NULL }
};

/*
 * dec21050_reg_wr:
 * Write a value to a config register on the DEC21050
 */
static void dec21050_reg_wr (pci_bridge_config_t *b,
			     int reg_offset,
			     ulong value)
{
    volatile ulong *ptr;

    if (b->config_ptr) {
	/*
	 * This platform supports direct access to the bridge
	 * chips registers. So do the cycle.
	 */
	ptr = (volatile ulong *)(b->config_ptr + reg_offset);
	*ptr = value;
    } else {
	/*
	 * We must go thru a PCI host bridge
	 */
	pci_config_write(b->primary_bus, b->device_num, 0, reg_offset, value);
    }
}

/*
 * dec21050_reg_rd:
 * Write a value to a config register on the DEC21050
 */
static ulong dec21050_reg_rd (pci_bridge_config_t *b,
			      int reg_offset)
{
    volatile ulong *ptr, value;

    if (b->config_ptr) {
	/*
	 * This platform supports direct access to the bridge
	 * chips registers. So do the cycle.
	 */
	ptr = (volatile ulong *)(b->config_ptr + reg_offset);
	value = *ptr;
    } else {
	/*
	 * We must go thru a PCI host bridge
	 */
	value = pci_config_read(b->primary_bus, b->device_num, 0, reg_offset);
    }
    return(value);
}

/*
 * dec21050_bridge_config:
 * Configure a DEC 21050 bridge
 */
boolean dec21050_bridge_config (pci_bridge_config_t *b, boolean reset_ok)
{
    ulong temp;
    int i;

    /*
     * If reset_ok is TRUE, reset the bridge
     */
    if (reset_ok) {
	/* Read is a workaround for VIP hardware bug (address stepping) */
	temp = dec21050_reg_rd(b, REG16_OFFSET);

	dec21050_reg_wr(b, REG16_OFFSET, REG16_CFG_RESET);
	for (i=0; i<DEC21150_MAX_INIT_LOOP; i++) {
	    temp = dec21050_reg_rd(b, REG16_OFFSET);
	    if (!(temp & REG16_CFG_RESET)) {
		break;
	    }
	}
    }
    temp = dec21050_reg_rd(b, REG16_OFFSET);
    if (temp & REG16_CFG_RESET) {		/* did not come out of reset */
	return(FALSE);
    }

    /*
     * Configure reg 1 (status/cmd), reg3 (latency), 
     */
    dec21050_reg_wr(b, REG1_OFFSET, b->stat_cmd);
    dec21050_reg_wr(b, REG3_OFFSET, b->primary_latency);

    /*
     * Configure reg 6, primary, secondary buses and secondary latency
     */
    temp = ((b->primary_bus) | 
	    (b->secondary_bus << 8) |
	    (b->subordinate_bus << 16) |
	    (b->secondary_latency << 24));
    dec21050_reg_wr(b, REG6_OFFSET, temp);

    /*
     * Configure reg 7, io limit, io base
     */
    temp = ((0xffff0000) |
	    (b->io_limit & 0xff00) |
	    ((b->io_base & 0xff00) >> 8));
    dec21050_reg_wr(b, REG7_OFFSET, temp);
    
    /*
     * Configure reg 8, memory mapped io limit and base
     */
    temp = ((b->memio_limit & 0xfff00000) |
	    ((b->memio_base  & 0xfff00000) >> 16));
    dec21050_reg_wr(b, REG8_OFFSET, temp);

    /*
     * Configure reg 9, prefetchable memory base, limit
     */
    temp = ((b->mem_limit & 0xfff00000) |
	    ((b->mem_base  & 0xfff00000) >> 16));
    dec21050_reg_wr(b, REG9_OFFSET, temp);

    /*
     * Configure reg 15, bridge control
     */
    dec21050_reg_wr(b, REG15_OFFSET, b->bridge_control);

    /*
     * Configure reg 16, burst limit etc
     */
    temp = b->burst_limit << 16;
    dec21050_reg_wr(b, REG16_OFFSET, temp);

    /*
     * Configure reg 17, primary, secondary timer
     */
    temp = ((b->secondary_timer << 8) |
	    (b->primary_timer));
    dec21050_reg_wr(b, REG17_OFFSET, temp);

    /*
     * Return TRUE
     */
    return(TRUE);
}

/*
 * dec21050_print_regs:
 * Printout the registers from a bridge chip
 */
void dec21050_print_regs (pci_bridge_config_t *ptr, uint handle)
{
    ulong vendor_id, temp;

    /*
     * Make sure it is a DEC21050 bridge chip
     */
    vendor_id = dec21050_reg_rd(ptr, PCI_VENDOR_ID_OFFSET);
    if (vendor_id != DECPPB_VENDOR_DEVICE_ID) {
	printf("\nHandle %d, bridge chip is not DEC21050, vendor/id=0x%x\n",
	       handle, vendor_id);
	return;
    }

    /*
     * Print out the registers
     */
    printf("\n%s, Handle=%d\n", ptr->desc, handle);
    printf("DEC21050 bridge chip, config=%#x\n", ptr->config_ptr);

    printf("(0x%02x): cfid   = 0x%-08x\n", PCI_VENDOR_ID_OFFSET, vendor_id);
    temp = dec21050_reg_rd(ptr, REG1_OFFSET);
    printf("(0x%02x): cfcs   = 0x%-08x\n", REG1_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG2_OFFSET);
    printf("(0x%02x): cfccid = 0x%-08x\n", REG2_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG3_OFFSET);
    printf("(0x%02x): cfpmlt = 0x%-08x\n\n", REG3_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG6_OFFSET);
    printf("(0x%02x): cfsmlt = 0x%-08x\n", REG6_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG7_OFFSET);
    printf("(0x%02x): cfsis  = 0x%-08x\n", REG7_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG8_OFFSET);
    printf("(0x%02x): cfmla  = 0x%-08x\n", REG8_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG9_OFFSET);
    printf("(0x%02x): cfpmla = 0x%-08x\n\n", REG9_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG15_OFFSET);
    printf("(0x%02x): cfbc   = 0x%-08x\n", REG15_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG16_OFFSET);
    printf("(0x%02x): cfseed = 0x%-08x\n", REG16_OFFSET, temp);
    temp = dec21050_reg_rd(ptr, REG17_OFFSET);
    printf("(0x%02x): cfstwt = 0x%-08x\n", REG17_OFFSET, temp);
}

/*
 * show_pci_bridge_regs:  
 * Print the PCI registers for this DEC bridge
 */
void show_pci_bridge_regs (parseinfo *csb)
{
    pci_bridge_config_t *ptr;
    uint handle;

    /*
     * Get parameters
     */
    handle = GETOBJ(int,1);
    ptr = get_bridge_config(handle);
    if (!ptr) {
	printf("\nNo config data structure for handle %d\n", handle);
	return;
    }
    if (pas_pa_present(handle)) {
        dec21050_print_regs(ptr, handle);
    } else {
        printf("\nPort Adapter %d is not present",handle);
    }
}

/*
 * dec_ppb_subsys_init:
 * DEC PCI to PCI Bridge subsystem initalization entry point
 */
static void dec_ppb_subsys_init (subsystype *subsys)
{
    /*
     * Add the parse chains
     */
    parser_add_command_list(bridge_chain_init_table, "pci-pci bridge");
}

/*
 * PCI to PCI bridge subsystem header
 */
#define DECPPB_MAJVERSION	1
#define DECPPB_MINVERSION	0
#define DECPPB_EDITVERSION	1

SUBSYS_HEADER (dec_bridge, DECPPB_MAJVERSION, 
	      DECPPB_MINVERSION, DECPPB_EDITVERSION,
	      dec_ppb_subsys_init, SUBSYS_CLASS_DRIVER,
	      NULL, NULL);

/* end of file */
