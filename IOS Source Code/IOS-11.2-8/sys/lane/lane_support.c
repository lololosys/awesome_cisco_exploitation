/* $Id: lane_support.c,v 3.7.18.5 1996/05/30 21:32:45 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_support.c,v $
 *------------------------------------------------------------------
 * lane_support.c -- LAN Emulation user interface routines
 *
 * October 1994  Chris Parrott
 *
 * Copyright (c) 1985-1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_support.c,v $
 * Revision 3.7.18.5  1996/05/30  21:32:45  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.7.18.4  1996/05/21  09:54:17  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.18.3  1996/05/09  14:32:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.18.2.2.1  1996/04/27  07:06:33  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.18.2  1996/04/25  23:14:37  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.18.1.8.5  1996/04/26  00:36:22  michellw
 * Branch: LE_Cal_V112_0_2_branch
 * Support "[no] lane client idle-time ..,." command.
 *
 * Revision 3.7.18.1.8.4  1996/04/16  02:44:30  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.18.1.8.3  1996/04/09  02:16:16  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.7.18.1.8.2  1996/04/08  14:51:50  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.18.1.8.1  1996/04/08  01:56:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.18.1  1996/03/18  20:32:46  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.20.1  1996/03/22  22:54:38  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.8.1  1996/03/05  06:43:02  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.7.2.3  1996/03/01  01:46:34  cakyol
 * convert "crashdump"s to "lane_router_crash" which can print
 * a last gasp message and call "logger_flush" before dying.
 * Helps debugging a lot.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7.2.2  1996/02/29  22:16:10  cakyol
 * correct some slight merge errors and clean up stuff related
 * to config access, which somehow must not have been totally
 * cleaned before.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.7.2.1  1996/02/27  21:10:22  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.2.2.2.2  1996/03/23  21:12:54  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.4.2.2.2.1  1996/02/26  23:07:15  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.4.2.2  1996/01/23  23:54:07  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.2.1  1996/01/11  01:46:23  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4  1996/01/05  04:56:07  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.3  1995/12/17  18:31:35  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2.10.1  1995/12/29  01:06:43  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.8  1995/11/08  21:12:24  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.7  1995/08/06  21:59:44  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.6  1995/07/26  00:19:05  cakyol
 * CSCdi37667:  show lane default-atm-addresses should show esi if no
 * prefix
 * done
 *
 * Revision 2.5  1995/07/19  00:24:36  cakyol
 * CSCdi37287:  show lane default-atm-addresses should handle no prfix case
 * - fixed
 *
 * Revision 2.4  1995/07/18  01:09:48  cakyol
 * CSCdi37230:  lane default-atm-address command should be a show command
 * - fixed.  Here is the summary:
 *
 *         show lane default-atm-address
 *                 prints the possible lane components on ALL atm
 *                 interfaces.
 *
 *         show lane default-atm-address interface atm1/0
 *                 prints the possible lane components on the
 *                 specified main-interface.
 *
 *         show lane default-atm-address interface atm1/0.1
 *                 prints the possible lane components on the
 *                 specified sub-interface.
 *
 * Revision 2.3  1995/07/17  22:34:55  cakyol
 * CSCdi37207:  auto addresses should be displayed without running the
 * components
 * - fixed.  The interface/subinterface command "lane
 * default-atm-addresses"
 *   now displays all would be auto addresses for all the lane components
 *   on the specific interface.
 *
 * Revision 2.2  1995/06/26  23:21:59  nfinn
 * CSCdi35235:  LANE config server access sequencing incorrect
 *
 * Revision 2.1  1995/06/07  21:19:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include "mgd_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "../if/ether.h"
#include "../if/tring.h"
#include "config.h"
#include "parser.h"
#include "../ui/debug.h"
#include "../ui/parse_util.h"
#include "../atm/parser_defs_atm.h"
#include "parser_defs_lane.h"
#include "address.h"
#include "logger.h"
#include "../ip/ip.h"
#include "../ip/ip_media.h"
#include "../ip/ip_registry.h"
#include "../hes/if_mci.h"
#include "../if/network.h"
#include "../if/atm.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"

#include "../ip/ip.h"
#include "../parser/parser_actions.h"

#include "lane.h"
#include "lane_debug.h"
#include "lane_private.h"
#include "lane_signal.h"
#include "lec_private.h"
#include "lsv_private.h"
#include "lane_ilmi.h"

/*
 * crash the router and if desired, print a last minute message
 */
void lane_router_crash (char *lastGaspMessage)
{
    if (lastGaspMessage)
    {
	buginf("\n********************************");
	buginf("\nLANE initiated deliberate crash:");
	buginf("\n%s", lastGaspMessage);
	buginf("\n********************************\n");
    }
    crashdump(5);
}


/*
 * lane_interface_commands
 *
 * Called to do all LANE interface specific command processing.
 *
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	csb = Ptr. to the Parse block with command & arguments.
 *	csb->which = LANE Command type
 *	csb->sense = [NO] value(0 = NO), (1 = !NO)
 */
void
lane_interface_commands (parseinfo *csb)
{
    idbtype		*swidb;
    hwidbtype 		*hwidb;
    lane_info_t		*lane_info;
    lec_info_t		*lec_info;
    int			sense, nvgen;

    swidb	= csb->interface;
    nvgen	= csb->nvgen;
    sense	= csb->sense;
    lane_info	= swidb->lane_info;
    hwidb 	= swidb->hwptr;
    lec_info	= NULL;

    /*
     * csb->nvgen == TRUE when the 'write [term || mem || net]' command is
     * entered.  Otherwise, it's false to indicate that a command is being
     * specified by the user.
     */
    if (nvgen) {

	/*
	 * Nothing is enabled on this interface, so punt.
	 */
	if (lane_info == NULL) {
	    return;
	}

	switch(csb->which) {
	case LANE_CMD_LEC:
	    lec_gen_lec(csb);
	    break;
	case LANE_CMD_LES_BUS:
	    lsv_gen_les_bus(csb);
	    break;
	case LANE_CMD_LECS_NSAP:
            break;
	case LANE_CMD_LEC_NSAP:
	    lec_gen_lec_nsap(csb);
            break;
	case LANE_CMD_BUS_NSAP:
	case LANE_CMD_LES_NSAP:
	    lsv_gen_atm_address(csb);
	    break;
	case LANE_CMD_LE_ARP:
	    lec_gen_le_arp(csb);
	    break;
	case LANE_CMD_IDLE_TIME:
	   lec_gen_idle_time(csb);
	    break;
	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

/*
 * csb->nvgen == FALSE.  Go process user specific command
 */

    switch (csb->which) {

    case LANE_CMD_LEC:
	lec_cfg_lec(csb);
	break;

    case LANE_CMD_LES_BUS:
	lsv_cmd_les_bus (csb);
	break;

    case LANE_CMD_LECS_NSAP:
	break;

    case LANE_CMD_LEC_NSAP:
	lec_cfg_lec_nsap(csb);
	break;

    case LANE_CMD_BUS_NSAP:
	lsv_cmd_bus_nsap(csb);
	break;

    case LANE_CMD_LES_NSAP:
	lsv_cmd_les_nsap(csb);
	break;

    case LANE_CMD_LE_ARP:
	lec_cfg_le_arp(csb);
	break;

    case LANE_CMD_IDLE_TIME:
	lec_cfg_idle_time(csb);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

} /* lane_interface_commands () */

void lane_mac_address_plan (parseinfo *csb)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    lane_info_t	*lane_info;
    int sense, nvgen;
    hwaddrtype *newAddr;

    swidb	= csb->interface;
    nvgen	= csb->nvgen;
    sense	= csb->sense;
    lane_info	= swidb->lane_info;
    hwidb 	= swidb->hwptr;

    if (nvgen && lane_info)
    {
	switch (lane_info->mac_plan_type) {
	    /* default */
	case LANE_MACPLAN_SAME:
	    break;

	case LANE_MACPLAN_SAME_NEWADDR:
	    nv_write(TRUE, "%s same %e", 
		     csb->nv_command, lane_info->mac_plan_base);
	    break;

	case LANE_MACPLAN_DIFF:
	    nv_write(TRUE, "%s different", csb->nv_command);
	    break;

	case LANE_MACPLAN_DIFF_NEWADDR:
	    nv_write(TRUE, "%s different %e", 
		     csb->nv_command, lane_info->mac_plan_base);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /* create the lane info */
    if (lane_info == NULL) {
	swidb->lane_info = lane_info = lane_init_idb(swidb);

	/* failed to create lane_info */
	if (lane_info == NULL) return;
    }

    /* the [no] form ALWAYS forces default */
    if (!sense) {
	set_default_mac_plan(lane_info);
	return;
    }

    /* record the type */
    lane_info->mac_plan_type = csb->which;

    /* in case we need it */
    newAddr = GETOBJ(hwaddr, 1);

    switch (csb->which) {
	/* default */
    case LANE_MACPLAN_SAME:
	set_default_mac_plan(swidb->lane_info);
	break;

	/*
	** on this one, we have to set the mac address back
	** to default first in case someone may have changed
	** it previously.  Then set the type back to what we want.
	*/
    case LANE_MACPLAN_DIFF:
	set_default_mac_plan(lane_info);
	lane_info->mac_plan_type = LANE_MACPLAN_DIFF;
	break;

    case LANE_MACPLAN_SAME_NEWADDR:
    case LANE_MACPLAN_DIFF_NEWADDR:
	bcopy(newAddr->addr, lane_info->mac_plan_base, 
	      STATIONLEN_IEEE48);
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}


/*
 * lane_show_one_all
 *
 * helper for lane_show_xxx_commands.  Calls "show_one" routines for
 * each lane component that might be attached to a given lane_info.
 */
static void
lane_show_one_all (lane_info_t *lane_info, boolean brief)
{
    if (lane_info == NULL)
        return;
    if (!is_subinterface(lane_info->swidb))
        lecs_interface_show(lane_info->swidb, brief);
    lane_show_one_server(lane_info, brief);
    lane_show_one_bus(lane_info, brief);
    lane_show_one_lec(lane_info, brief);
}

/*
 * show lane [interface INTERFACE | name NAME]
 * show lane server [interface INTERFACE | name NAME]
 * show lane bus [interface INTERFACE | name NAME]
 */

void
lane_show_xxx_commands (parseinfo *csb)
{
    idbtype             *swidb;
    lane_info_t         *lane_info;
    char                *name;
    boolean             brief;
    void                (*show_one_routine)(lane_info_t *, boolean);

    switch(GETOBJ(int, 1)) {
    case SHOW_LANE_SERVER:
        show_one_routine = lane_show_one_server;
        break;
    case SHOW_LANE_BUS:
        show_one_routine = lane_show_one_bus;
        break;
    case SHOW_LANE_ALL:
    default:
        show_one_routine = lane_show_one_all;
        break;
    }

    brief = GETOBJ(int, 2) & SHOW_LANE_BRIEF;

    automore_enable(NULL);
        
    if (GETOBJ(int, 2) & SHOW_LANE_INT) {

        swidb = GETOBJ(idb, 1);
        if (csb->subif_specified) {
            if (swidb->lane_info)
                show_one_routine(swidb->lane_info, brief);
        } else {
            FOR_ALL_SWIDBS_ON_HW(swidb->hwptr, swidb)
                if (swidb->lane_info)
                    show_one_routine(swidb->lane_info, brief);
        }

    } else if (GETOBJ(int, 2) & SHOW_LANE_NAME) {

        name = GETOBJ(string, 1);
        FOR_ALL_LANE_INFOS(lane_info) {
            if ((strlen(name) == lane_info->name_size) &&
                !bcmp(name, lane_info->name, lane_info->name_size))
                show_one_routine(lane_info, brief);
        }
        
    } else {

        FOR_ALL_LANE_INFOS(lane_info)
            show_one_routine(lane_info, brief);

    }

    automore_disable();
    
}


/*
** show what each lane component's auto address would
** normally be on this sub/main interface
*/
static void show_auto_address (idbtype *swidb, 
    boolean supressSelectorPrinting)
{
    MaskedAddress maddr;
    hwaddrtype hwaddr;
    lane_ilmi_entity_t lane_entity;
    char buffer [ATM_MAX_NSAP_STRING];
    char *buf;
    int len;
    lane_ilmi_compback_t err;
    boolean wildcardPrinted = FALSE;
    boolean noPrefix;
    
    for (lane_entity = LANE_ENTITY_LOW;
	 lane_entity <= LANE_ENTITY_HIGH;
	 lane_entity++) {
	/* write header */
	switch (lane_entity) {
	    case LANE_ILMI_CONFIG_SERVER:
		printf("LANE Config Server: ");
		break;
	    case LANE_ILMI_LE_SERVER:
		printf("LANE Server:        ");
		break;
	    case LANE_ILMI_BUS:
		printf("LANE Bus:           ");
		break;
	    case LANE_ILMI_LE_CLIENT:
		printf("LANE Client:        ");
		break;
	    default:
		printf("UNKNOWN LANE ENTITY %d", lane_entity);
		goto skipped;
	}

	/* generate the auto address and print it */
	noPrefix = FALSE;
	memset(&maddr, 0, sizeof(MaskedAddress));
	maddr.type = STATION_ATMNSAP;
	maddr.length = STATIONLEN_ATMNSAP;
	err = lane_ilmi_compute_atmsap(swidb, lane_entity, &maddr, &hwaddr);

	/* if no prefix available, just try and display the esi */
	if (err == LANE_ILMI_COMPUTE_NO_PREFIX)	{
	    noPrefix = TRUE;
	    memset(maddr.maskBytes, 0xff, PREFIX_SIZE);
	    err = lane_ilmi_compute_atmsap
		    (swidb, lane_entity, &maddr, &hwaddr);
	}

	/* if still an error not much we can do */
	if (err == LANE_ILMI_COMPUTE_SUCCESS) {
	    buf = &buffer [0];
	    atm_printnsap(hwaddr.addr, buffer);
	    if (noPrefix) {
		buf += 25;
		buf [0] = buf [1] = buf [2] = '.';
	    }
	    if (supressSelectorPrinting && 
		(lane_entity != LANE_ILMI_CONFIG_SERVER)) {
		len = strlen(buf);
		buf [--len] = '*';
		buf [--len] = '*';
		wildcardPrinted = TRUE;
	    }
	    printf("%s", buf);
	} else {
	    printf("%s", lane_ilmi_error_text(err));
	}

	skipped:
	    printf("\n");
    }
    if (wildcardPrinted)
	printf ("note: ** is the subinterface number byte in hex\n");
}

void lane_show_auto_addresses (parseinfo *csb)
{
    hwidbtype *hwidb;
    idbtype *swidb;

    if (csb->which == SHOW_LANE_ADDR_INT) {
	swidb = GETOBJ(idb, 1);
	if (csb->subif_specified) {
	    show_auto_address(swidb, FALSE);
	} else {
	    show_auto_address(swidb, TRUE);
	}
    } else if (csb->which == SHOW_LANE_ADDRESSES) {
	FOR_ALL_HWIDBS(hwidb) {
	    if (is_atm(hwidb)) {
		printf("\ninterface %s:\n", hwidb->hw_namestring);
		show_auto_address(hwidb->firstsw, TRUE);
	    }
	}
    } else {
	printf("%% internal router error in lane_show_auto_addresses\n");
    }
}


