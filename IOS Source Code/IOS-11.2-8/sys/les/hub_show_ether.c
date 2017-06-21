/* $Id: hub_show_ether.c,v 3.5.20.3 1996/08/28 12:56:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/hub_show_ether.c,v $
 *------------------------------------------------------------------
 * hub_show_ether.c -- functions to implement ether hub show commands
 *
 * May 1994, Steve Garcia
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_show_ether.c,v $
 * Revision 3.5.20.3  1996/08/28  12:56:54  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.20.2  1996/05/21  09:56:06  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.5.20.1  1996/03/18  20:41:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.14.2  1996/02/20  01:08:10  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/01  06:06:44  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4.14.1  1996/01/24  22:22:51  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/17  18:32:45  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/17  17:38:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:15  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:14:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  21:32:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "../ui/common_strings.h"
#include "logger.h"
#include "../les/msg_hub.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"
#include "config.h"
#include "interface_private.h"
#include "ieee.h"
#include "../les/if_les.h"
#include "../les/hub.h"
#include "../les/hub_ether.h"
#include "../les/hub_show_ether.h"
#include "../les/parser_defs_hub.h"
#include "../parser/parser_actions.h"


/*
 * =====================================================================
 * hub_show_ether_general()
 *
 * Description:
 * Display general repeater statistics and aui port data.
 *
 * Parameters:
 * access - pointer to hub access structure
 * 
 * Returns:
 * None
 * =====================================================================
 */
void hub_show_ether_general (hub_access_t *access)
{
    uint total_octets;
    int port;
    boolean okay;
    uint uint_data;
    int i;
    uint num_bytes;
    hub_t *hub;
    hub_ether_t *hub_ether;
    hub_device_t *tmp_device = NULL;
    himib_port_info_t *port_info = NULL;
    ushort *hub_misc_reg_addr;
    ushort hub_misc_reg;

    /* 
     * For a detailed explanation of why I don't merely use the
     * HIMB's TotalOctets counter for total octets, see
     * les/snmp_repeatermib.c, CASE RPTR_MONITORGROUPTOTALOCTETS:
     */
    hub = hub_get_hub_ptr(access->hub_type, access->hub_number);
    access->access_code = READABLE_OCTETS_CODE;
    total_octets = 0;  /* initialize */
    tmp_device = hub->hub_device;
    while (tmp_device != NULL) {
        for (port = 0; port < tmp_device->num_ports; port++) {
            access->device = tmp_device;
            access->phys_port = port;
 
	    uint_data = hub_get_val_uint(access);
	    port_info =
	        &(access->device->hub_ether->port_info[access->phys_port]);
	    total_octets +=
	        zero_base_counter(uint_data, port_info->octets_count_base);
        }
        tmp_device = tmp_device->next_hub_device;
    }

    /*
     * map any logical port onto physical port/device combination to
     * fill out hub access structure to retrieve the remaining data
     */
    okay = hub_map_port(0, access);
    if (!okay) {
        errmsg(&msgsym(BADUNIT, HUB), 0);
        return;
    }

    printf("\nRepeater information (Connected to %s)\n",
        hub->host_hwidb->firstsw->namestring);

    /* total transmit collisions */
    access->access_code = TRANSMIT_COLLISIONS_CODE;
    uint_data = hub_get_val_uint(access);

    hub_ether = access->device->hub_ether;
    printf("  %u bytes seen with %u collisions, %u hub resets",
	   total_octets,
	   zero_base_counter(uint_data,
			     hub_ether->gen_info->tx_collisions_count_base),
	   hub->resets);

    /* 
     * read Hub Misc Register to retrieve 
     * status of MDI/MDI-X switch for Cabrillo
     */
    if (cookie->interfaces == INTERFACE_1E2T1Bhub) {
        hub_misc_reg_addr = (ushort *)ADRSPC_GAL_HUB_MISC_REG;
        hub_misc_reg = *hub_misc_reg_addr;
        printf("\n  MDI/MDI-X switch set to MDI%s mode for port %d", 
            hub_misc_reg & ETHER_MDI_MODE?"":"-X", MDI_SWITCHABLE_PORT);
    } 

    /* device codes */
    access->access_code = VERSION_DEVICE_ID_CODE;
    num_bytes = hub_read_himib( access );
    printf("\n  Version/device ID ");
    for (i = 0; i < num_bytes; i++) {
        printf("%d/%d", access->buffer[i] >> 4, access->buffer[i] & 0x0f);
    }
    /*
     * Retrieve version id from daughter device if it exists.
     * Have to bump access struct device pointer, so retain current.
     */
    if (access->device->next_hub_device) {
        tmp_device = access->device;
        access->device = access->device->next_hub_device;
        num_bytes = hub_read_himib( access );
        for (i = 0; i < num_bytes; i++) {
            printf(" (%d/%d)", access->buffer[i] >> 4,access->buffer[i] & 0x0f);
        }
        access->device = tmp_device;
    }

    /* config register and jabber status, but only if in enabled mode */
    if (CUR_PRIV(stdio->privilege) > PRIV_USER) {
	access->access_code = CONFIG_REG_CODE;
	num_bytes = hub_read_himib( access );
	printf("\n  Repeater config register is");
	for (i = 0; i < num_bytes; i++) {
	    printf(" %#2x", access->buffer[i]);
	}

        access->access_code = MJLP_STATUS_CODE;
        num_bytes = hub_read_himib( access );
        if (access->buffer[0]) {
            printf("\n  MAU Jabber Lock Up Protection was triggered for mother board.");
        }
        /*
         * retrieve MJLP status from daughter device if it exists
         */
        if (access->device->next_hub_device) {
            tmp_device = access->device;
            access->device = access->device->next_hub_device;
            num_bytes = hub_read_himib( access );
            if (access->buffer[0]) {
                printf("\n  MAU Jabber Lock Up Protection was triggered for daughter board.");
            }
            access->device = tmp_device;
        }
    }

    /* last counter clearing */
    printf("\n  Last clearing of \"show hub\" counters ");
    print_dhms(access->device->parent_hub->last_clear_time);
    printf("\n");

}   /* hub_show_ether_general */


/*
 * =====================================================================
 * hub_show_port()
 *
 * Description:
 * Display all statistics for a single given repeater port.
 * Called by hub_show_tp_port and hub_show_aui_port. 
 *
 * Parameters:
 * phys_port - physical port number to display
 * access    - pointer to hub access structure
 *
 * Returns:
 * None
 * =====================================================================
 */
static void hub_show_port (int phys_port, hub_access_t *access)
{
    int num_bytes;
    uint uint_data;
    uint fcs_err, align_err, too_long_err, short_err, runt_err;
    uint collision_err, late_event_err, very_long_err, data_rate_err;
    uint total_errors = 0;
    himib_port_info_t *port_info;
    hub_ether_t *hub_ether;
    uchar port_mask = 0;

    port_info = &(access->device->hub_ether->port_info[phys_port]);

    /* fetch each value needed, and print it */
    access->access_code = READABLE_FRAMES_CODE;
    uint_data = hub_get_val_uint(access);
    printf("  %u packets input", 
        zero_base_counter(uint_data, port_info->frames_count_base));

    access->access_code = READABLE_OCTETS_CODE;
    uint_data = hub_get_val_uint(access);
    printf(", %u bytes\n", 
        zero_base_counter(uint_data, port_info->octets_count_base));

    access->access_code = COLLISIONS_CODE;
    uint_data = hub_get_val_uint(access);
    collision_err = 
        zero_base_counter(uint_data, port_info->collisions_count_base);

    access->access_code = FCS_ERR_CODE;
    uint_data = hub_get_val_uint(access);
    fcs_err = zero_base_counter(uint_data, port_info->fcs_count_base);
    total_errors += fcs_err;

    access->access_code = ALIGNMENT_ERR_CODE;
    uint_data = hub_get_val_uint(access);
    align_err = zero_base_counter(uint_data, port_info->align_err_count_base);
    total_errors += align_err;

    access->access_code = FRAMES_TOO_LONG_CODE;
    uint_data = hub_get_val_uint(access);
    too_long_err = zero_base_counter(uint_data, port_info->long_count_base);
    total_errors += too_long_err;

    access->access_code = SHORT_EVENTS_CODE;
    uint_data = hub_get_val_uint(access);
    short_err = zero_base_counter(uint_data, port_info->short_count_base);
    total_errors += short_err;

    access->access_code = RUNTS_CODE;
    uint_data = hub_get_val_uint(access);
    runt_err = zero_base_counter(uint_data, port_info->runts_count_base);
    total_errors += runt_err;

    access->access_code = LATE_EVENTS_CODE;
    uint_data = hub_get_val_uint(access);
    late_event_err = zero_base_counter(uint_data, port_info->lates_count_base);
    total_errors += late_event_err;

    access->access_code = VERY_LONG_EVENTS_CODE;
    uint_data = hub_get_val_uint(access);
    very_long_err = 
        zero_base_counter(uint_data, port_info->long_events_count_base);
    total_errors += very_long_err;

    access->access_code = DATA_RATE_MISMATCHES_CODE;
    uint_data = hub_get_val_uint(access);
    data_rate_err = 
        zero_base_counter(uint_data, port_info->mismatch_count_base);
    total_errors += data_rate_err;

    printf("  %u errors with %u collisions\n",
	   total_errors, collision_err);
    printf("     (%u FCS, %u alignment, %u too long,\n",
	   fcs_err, align_err, too_long_err);
    printf("      %u short, %u runts, %u late,\n",
	   short_err, runt_err, late_event_err);
    printf("      %u very long, %u rate mismatches)\n",
	   very_long_err, data_rate_err);

    access->access_code = AUTO_PARTITIONS_CODE;
    uint_data = hub_get_val_uint(access);
    printf("  %u auto partitions,", 
        zero_base_counter(uint_data, port_info->partitions_count_base));

    /* if port hasn't yet received anything, then HIMIB will return
     * lame LSA information.  Don't display it.
     */
    hub_ether = access->device->hub_ether;
    port_mask = GET_PORT_MASK(access->phys_port);
    if (hub_ether->LSA_correct & port_mask) {
	access->access_code = LAST_SOURCE_ADDR_CODE;
	num_bytes = hub_read_himib( access );
	ieee_copy(access->buffer, port_info->learned_addr);
	printf(" last source address was %e", access->buffer);
    }
    else {
	printf(" last source address (none)");
    }

    if (CUR_PRIV(stdio->privilege) > PRIV_USER) {
         if ((hub_ether->source_addr_config & port_mask) ||
             (hub_ether->source_addr_learn & port_mask)) {
	     printf("\n  Target source address is %e", 
                    port_info->source_addr);
             if (hub_ether->source_addr_illegal & port_mask) {
	         printf("\n  Last illegal source address was %e", 
                        port_info->current_illegal_source_addr);
             }
         }
    }

    printf("\n  Last clearing of \"show hub\" counters ");
    print_dhms(port_info->last_clear_time);
    printf("\n");

}   /* hub_show_port */


/*
 * =====================================================================
 * hub_show_aui_port()
 *
 * Description:
 * Display statistics for internal AUI port.
 * Called by show_hub to display internal AUI port. 
 *
 * Parameters:
 * access - pointer to hub access structure
 * 
 * Returns:
 * None
 * =====================================================================
 */
void hub_show_aui_port (hub_access_t *access)
{
    boolean okay;
    int aui_index;

    /* map any logical port just to get hub_access struct filled */
    okay = hub_map_port(1, access);
    if (!okay) {
        errmsg(&msgsym(BADUNIT, HUB), 1);
        return;
    }

    /* change access struct's physical port to HIMIB's aui representation */
    access->phys_port = HIMIB_AUI_PORT;
    /* get index to aui port info */
    aui_index = access->device->num_ports;

    /* retrieve values for AUI port, and print it */
    printf("\nInternal Port (Connected to %s)\n",
        access->device->parent_hub->host_hwidb->firstsw->namestring);
    hub_show_port(aui_index, access); 

}   /* hub_show_aui_port */


/*
 * =====================================================================
 * hub_show_tp_port()
 *
 * Description:
 * Display statistics for a single himib TP port.
 * Called by show_hub once per port to be displayed. 
 * 
 * Parameters:
 * logical_port - logic TP port number to show
 * access       - pointer to hub access structure
 *
 * Returns:
 * None 
 * =====================================================================
 */
void hub_show_tp_port (int logical_port, hub_access_t *access)
{
    boolean okay;
    int port_mask;
    hub_ether_t *hub_ether;
    boolean admin_enabled, partitioned, mismatch;
    boolean link_enabled, link_state;
    ushort *hub_misc_reg_addr;
    ushort hub_misc_reg;


    /* map logical port onto physical port/device combination */
    okay = hub_map_port(logical_port, access);
    if (!okay) {
        errmsg(&msgsym(BADUNIT, HUB), logical_port);
        return;
    }

    port_mask = GET_PORT_MASK(access->phys_port);
    hub_ether = access->device->hub_ether;

    /*
     * Gather some status to decide what/how to display port state.
     */
    /* is port administratively enabled? */
    admin_enabled = access->device->enabled & port_mask;
    /* is port autopartitioned by IMR+? */
    access->access_code = TP_PORT_PARTITIONING_STATUS_CODE;
    hub_read_himib(access);
    partitioned = (~(access->buffer[0]) & port_mask);
    /* is there a source addr mismatch */
    mismatch = hub_ether->source_addr_mismatch & port_mask;
    /* is link state test enabled */
    link_enabled = hub_ether->link_test & port_mask;
    /* get link status */
    access->access_code = TP_PORT_LINK_TEST_STATUS_CODE;
    hub_read_himib(access);
    link_state = access->buffer[0] & port_mask;

    printf("\nPort %d of %d is ", logical_port, netherhubports);
    if (!admin_enabled)
	printf("administratively down");
    else if (link_enabled && !link_state)
	printf("down");
    else if (partitioned || mismatch)
	printf("down");
    else
	printf("up");

    if (link_enabled) {
	if (link_state)
	    printf(", link state is up");
	else
	    printf(", link state is down");
    }

    /* retrieve receiver polarity status of TP port */
    access->access_code = TP_PORT_RECEIVE_POLARITY_STATUS_CODE;
    hub_read_himib(access);
    if (access->buffer[0] & port_mask) {
        printf(", polarity reversed");
    } 

    /*
     * read Hub Miscellaneous Register to check
     * status of MDI/MDI-X switch for Cabrillo
     */
    if ((cookie->interfaces == INTERFACE_1E2T1Bhub) && 
        (logical_port == MDI_SWITCHABLE_PORT)) {
        hub_misc_reg_addr = (ushort *)ADRSPC_GAL_HUB_MISC_REG;
        hub_misc_reg = *hub_misc_reg_addr;
        printf(", MDI%s mode\n", hub_misc_reg & ETHER_MDI_MODE?"":"-X");
    } else {
        printf("\n");
    }        

    hub_show_port(access->phys_port, access);

}   /* hub_show_tp_port */
