/* $Id: hub.c,v 3.6.20.2 1996/08/28 12:56:44 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/hub.c,v $
 *------------------------------------------------------------------
 * hub.c -- generalized hub functions
 *
 * March 1994, Steve Garcia
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub.c,v $
 * Revision 3.6.20.2  1996/08/28  12:56:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.6.20.1  1996/03/18  20:40:54  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/09  05:07:16  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.1  1996/02/20  01:07:59  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/01  06:06:33  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.5  1996/01/25  11:19:45  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.4  1995/12/16  00:14:14  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.3  1995/11/17  17:37:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:10  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:13:51  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/18  06:47:05  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 21:31:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "sys_registry.h"
#include "subsys.h"
#include "ttysrv.h"
#include "interface_private.h"
#include "logger.h"
#include "../les/msg_hub.c"		/* Not a typo, see logger.h */
#include "stacks.h"
#include "exception.h"
#include "../les/hub.h"
#include "../les/hub_ether.h"
#include "../les/hub_parser.h"
#include "../les/hub_registry.h"
#include "../les/hub_registry.regc"
#include "../les/if_les.h"
#include "../les/sr_rptrmib.h"
#include "../les/parser_defs_hub.h"

long hub_regcode;			/* hub service registration
					   code */

/*
 * Prototypes
 */
static void hub_init(subsystype *);
static hub_t *hub_create (int, int);

/*
 * hub subsystem header
 */

#define	HUB_MAJVERSION	1
#define	HUB_MINVERSION	0
#define	HUB_EDITVERSION	1

SUBSYS_HEADER(hub, HUB_MAJVERSION, HUB_MINVERSION, HUB_EDITVERSION,
	      hub_init, SUBSYS_CLASS_DRIVER,
	      NULL,
	      NULL);

/*
 * =====================================================================
 * hub_platform()
 *
 * Description:
 * Determines if the platform is a hublet
 *
 * Parameters:
 * None
 *
 * Returns:
 * TRUE if the platform is a hub, FALSE otherwise
 * =====================================================================
 */
boolean hub_platform (void)
{
    if ((cpu_type != CPU_CANCUN) ||
	((cookie->interfaces != INTERFACE_1E2Thub) &&
         (cookie->interfaces != INTERFACE_1E2T1Bhub))) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * =====================================================================
 * hub_init()
 *
 * Description:
 * Hub subsytem initialization entry point.
 *
 * Parameters:
 * subsys - subsystype ptr
 *
 * Returns:
 * Nothing
 * =====================================================================
 */
static void hub_init (subsystype *subsys)
{
    hub_t *hub = NULL;
    ushort *hub_misc_reg_addr;
    ushort hub_misc_reg;

    /* Set up the hub private function registry */
    hub_regcode = registry_create(REG_UNKNOWN, SERVICE_HUB_MAX, "Hub");
    if (hub_regcode != REG_ILLEGAL)
	create_registry_hub();

    /* system wide list of hubs in the box */
    hub_list = NULL;

    /* bail out if not cancun hub */
    if (!hub_platform())
	return;

    /* read Hub Miscellaneous Register */
    hub_misc_reg_addr = (ushort *)ADRSPC_GAL_HUB_MISC_REG;
    hub_misc_reg = *hub_misc_reg_addr;

    /*
     * create the parent hub struct, if cannot create struct,
     * things are real bad, crashdump the system.
     */
    hub = hub_create(HUB_ETHER, 0);
    if (!hub) {
        errmsg(&msgsym(NOMEMORY, HUB), 0, "hub structure");
        crashdump(0);
    }
    /*
     * create the device struct, if cannot create struct,
     * things are real bad, crashdump the system.
     */
    hub->hub_device = hub_create_ether_device(FALSE, hub);
    if (hub->hub_device == NULL) {
        errmsg(&msgsym(NOMEMORY, HUB), 0, "hub device structure");
        crashdump(0);
    }
    /*
     * create daughterboard device struct if needed, 
     * if cannot create struct, things are real bad, crashdump
     * the system.
     */
    if (~hub_misc_reg & ETHER_DAUGHTER_CARD) {
        hub->hub_device->next_hub_device = hub_create_ether_device(TRUE, hub);
        if (hub->hub_device->next_hub_device == NULL) {
            errmsg(&msgsym(NOMEMORY, HUB), 0, "hub device structure");
            crashdump(0);
        }
    }

    /*
     * Fill in more of the hub struct.
     */
    if (cookie->interfaces == INTERFACE_1E2Thub) {
        if (hub_count_ports(hub->hub_type, hub->hub_number) ==
                PORTS_PER_DEVICE_8) {
          hub->management->grp_description = GROUPDESCR_8PORT;
        } else {
          hub->management->grp_description = GROUPDESCR_16PORT;
        }
    } else {
        hub->management->grp_description = GROUPDESCR_14PORT;
    }

    /*
     * Do etherhub specific modular magic.
     */
    /* hub setup registry */
    reg_add_setup_hub(hub_ether_setup_int, "hub_ether_setup_int");

    /*
     * Find end of global list of hubs (hub_list) and
     * insert new hub there.
     * (Since this is a single hub box, no need to follow a list.)
     */
    hub_list = hub;

    /* initialize hub portion of parser and debug facility */
    hub_parser_init();
    hub_debug_init();

    /* register some hub functions */
    reg_add_show_int(hub_show_int, "hub_show_int");
    reg_add_if_final_init(hub_if_final_init, "hub_if_final_init");

}   /* hub_init */


/*
 * =====================================================================
 * hub_count_ports()
 * 
 * Description:
 * Count the number of ports on a hub.
 *
 * Parameters:
 * target_hub_type   - hub type to look for 
 * target_hub_number - hub number to look for
 *
 * Returns:
 * port count of a specified hub.
 * =====================================================================
 */
int hub_count_ports (int target_hub_type, int target_hub_number)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;
    int port_count = 0;

    cur_hub = hub_list;
    while (cur_hub != NULL) {
        if (cur_hub->hub_type == target_hub_type) {
            if (cur_hub->hub_number == target_hub_number) {
                /* 
                 * Found the hub we want.  Sum the
                 * ports on each device on this hub. 
                 */
                cur_device = cur_hub->hub_device;
                while (cur_device != NULL) {
                    port_count += cur_device->num_ports;
                    cur_device = cur_device->next_hub_device;
                }
                return(port_count);
            }
        }
        cur_hub = cur_hub->next_hub;
    }

    /* couldn't find target hub */
    return (0);
}   /* hub_count_ports */


/*
 * =====================================================================
 * hub_get_hub_ptr()
 *
 * Description:
 * Find the hub specified by type and number.
 *
 * Parameters:
 * target_hub_type   - hub type to look for
 * target_hub_number - hub number to look for
 *
 * Returns:
 * ptr to hub structure
 * =====================================================================
 */
hub_t *hub_get_hub_ptr (int target_hub_type, int target_hub_number)
{
    hub_t *cur_hub;

    /* 
     * traverse the list of hubs, looking for right type and number 
     */
    cur_hub = hub_list;
    while (cur_hub != NULL) {
        if (cur_hub->hub_type == target_hub_type) {
            if (cur_hub->hub_number == target_hub_number) {
                return (cur_hub);
            }
        }
        cur_hub = cur_hub->next_hub;
    }
    return (NULL);

}   /* hub_get_hub_ptr */


/*
 * =====================================================================
 * hub_map_port()
 *
 * Description:
 * Map a given hub number and logical port combination onto a
 * device and physical port pair.
 *
 * Parameters:
 * logical_port  - logical hub port to be mapped (one based)
 * access        - pointer to Hub access structure with hub_type 
 *                 and hub_number fields complete.
 *
 * Returns:
 * True iff able to complete mapping
 * phys_port (zero based) and device fields filled in access struct
 * =====================================================================
 */
boolean hub_map_port (int logical_port, hub_access_t *access)
{
    hub_t *cur_hub;
    hub_device_t *cur_device;
    int count;

    /* locate target hub */
    cur_hub = hub_get_hub_ptr(access->hub_type, access->hub_number);
    if (cur_hub == NULL) {
        errmsg(&msgsym(BADHUB, HUB), access->hub_type, access->hub_number);
        return(FALSE);
    }

    /* map to device and port within that hub */
    cur_device = cur_hub->hub_device;
    count = 1;
    while (cur_device != NULL) {
        if (logical_port < (cur_device->num_ports + count)) {
            /* found right device */
            access->phys_port = logical_port - count;
            access->device = cur_device;
            return(TRUE);
        } else {
            count += cur_device->num_ports;
            cur_device = cur_device->next_hub_device;
        }
    }

    return(FALSE);

}   /* hub_map_port */


/*
 * =====================================================================
 * hub_create()
 *
 * Description:
 * Create a new hub data structure.
 *
 * Parameters:
 * type   - type of hub
 * number - number to assign to hub
 *
 * Returns: 
 * pointer to newly formed hub data structure
 * =====================================================================
 */
hub_t *hub_create (int type, int number)
{
    hub_t *hub;

    /* malloc hub struct */
    hub = malloc(sizeof(hub_t));
    if (!hub) {
        return(NULL);
    }
    hub->next_hub = NULL;
    hub->hub_type = type;
    hub->hub_number = number;

    /* malloc hub management struct */
    hub->management = malloc(sizeof(hub_mgmt_t));
    if (!hub->management) {
	free(hub);
	return(NULL);
    }

    /*
     * Bidirectional links between hub struct and the hosting
     * interface's idb (hub->host_hwidb, and hwidb->hub) will be
     * initialized in hub_if_final_init().  Can't do them now, since
     * the link order determines whether the idb's even exist now.
     */
    hub->host_hwidb = NULL;

    return(hub);

}   /* hub_create */


/*
 * =====================================================================
 * hub_if_final_init()
 *
 * Description:
 * Final initialization of the hub after setting up all hub structures.
 * Depending on hub type, these include:
 *   - Setup bidirectional links between each hub struct and its hosting
 *     interface's idb.  The links are hub->host_hwidb and hwidb->hub.
 *   - Clearing hub counters and setting up interrupts handling for 
 *     ethernet hub.
 *
 * Called via if_final_init service point, to guarantee all interfaces
 * have already been discovered and initialized.
 *
 * The algorithm used below is okay for products with a single hub,
 * but scrap it when we have a product with multiple hubs.  Come up
 * with an algorithm that will traverse the hwidb list one single
 * time (rather than one time per hub).  Also come up with one that
 * works with multiple hwidbs of the same type as the hub.  :-)
 * Below would tie a hub to every hwidb of the hub's type.
 *
 * Parameters:
 * None
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_if_final_init (void)
{
    hub_t *cur_hub;
    hwidbtype *idb;
    uchar *hub_ivr;
    list_element *l_elt;

    hub_ivr = (uchar *) ADRSPC_GAL_HUB_IVR;
    /* 
     * Traverse the list of hubs.
     * For each hub, find the hosting interface and setup pointers.
     */
    cur_hub = hub_list;
    while (cur_hub != NULL) {
	hub_reset(cur_hub);
        if (cur_hub->hub_type == HUB_ETHER) {
            FOR_ALL_HWIDBS_IN_LIST(HWIDBLIST_LANCE, l_elt, idb) {
		/* point hwidb to hub, and hub to hwidb */
		idb->hub = cur_hub;
		cur_hub->host_hwidb = idb;
		break;
            }
            /* clear all counters on this hub */
            reset_himib_counters(HUB_ETHER, cur_hub->hub_number, 0);

            /* initialize port_info parameters related to mac security */
            hub_init_mac_security(cur_hub);
	    
	    /* snmp hack: rfc1516 (ether repeater mib) wants
	     * GroupLastOperStatusLastChange to be 0 if the group's
	     * oper status has not changed since the agent last
	     * restarted.  I'll consider hub_if_final_init to be the
	     * starting point of the hub agent, so zero that field
	     * here.
	     */

	    TIMER_STOP(cur_hub->management->grp_last_oper_stat_change);

            /*
             * Set up Interrupt Vector Register, assign the address
             * of interrupt handler to the vector offset, and enable
             * all ether hub interrupts in himib.
             */
            *hub_ivr = GAL_ETHER_HUB_VECTOR;
            createlevel(LEVEL2, NULL,"HUB interrupt");
	    exceptionHandler(GAL_ETHER_HUB_VECTOR, level2_hub);
	    pan_level2_int.function = (ulong) hub_ether_interrupt;
	    pan_level2_int.idb1 = NULL;
	    pan_level2_int.idb2 = NULL;
            hub_ether_enable_interrupts(cur_hub);
        } 

        cur_hub = cur_hub->next_hub;
    }
}  /* hub_if_final_init */


/*
 * =====================================================================
 * hub_reset()
 *
 * Descriptions:
 * Resets a given hub.
 *
 * Parameters:
 * hub -- ptr to hub struct for hub to be reset
 *
 * Returns:
 * void
 * =====================================================================
 */
void hub_reset (hub_t *hub)
{
    hub_access_t access;

    switch(hub->hub_type) {
    case HUB_ETHER:
        access.hub_number = hub->hub_number;
        access.hub_type = hub->hub_type;
        hub_ether_reset(&access);
        break;
    default:
        errmsg(&msgsym(BADHUB, HUB), hub->hub_type, hub->hub_number);
        break;
    }
}  /* hub_reset */

