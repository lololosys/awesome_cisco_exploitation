/* $Id: hub_parser.c,v 3.5.18.3 1996/08/28 12:56:52 thille Exp $
 * $Source: /release/112/cvs/Xsys/les/hub_parser.c,v $
 *------------------------------------------------------------------
 * Hub parser commands
 *
 * March 1994, Robert Widmer
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Gurus: Steve Garcia, Viet Nguyen
 *------------------------------------------------------------------
 * $Log: hub_parser.c,v $
 * Revision 3.5.18.3  1996/08/28  12:56:52  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.2  1996/06/17  23:34:55  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  20:41:01  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/02/20  01:08:08  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/07  16:13:53  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.4  1996/02/01  06:06:42  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3.26.1  1996/01/24  22:22:48  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/11/17  17:37:55  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:57:13  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:30:41  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:14:02  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/09  00:24:17  hampton
 * Remove include of deleted file.  [CSCdi35520]
 *
 * Revision 2.1  1995/06/07 21:32:09  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../les/msg_hub.c"		/* Not a typo, see logger.h */
#include "ttysrv.h"
#include "config.h"
#include "interface_private.h"
#include "registry.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "subsys.h"
#include "packet.h"
#include "parser.h"
#include "ieee.h"
#include "../les/hub.h"
#include "../les/hub_ether.h"
#include "../les/hub_show_ether.h"
#include "../les/hub_registry.h"
#include "../les/if_les.h"
#include "../les/if_les_lance.h"
#include "../les/parser_defs_hub.h"
#include "../../boot/src-68-c3000/pcmap.h"
#include "../ui/command.h"
#include "../if/msg_clear.c"		/* Not a typo, see logger.h */


/*
 * =====================================================================
 * show_hub()
 *
 * Description:
 * Handler for show hub user commands.
 * Called by show_hub_command() to show needed data for a single hub.
 *
 * Parameters:
 * csb     - pointer to struct of parser info for current user command
 * cur_hub - pointer to hub to show
 *
 * Returns:
 * None
 * =====================================================================
 */
void show_hub (parseinfo *csb, hub_t *hub)
{
    /*
     * show hub [{ether|token} <hub-num> [<start-port> [<end-port>]]]
     *
     * OBJ(int,1) = HUB_ETHER == Ether hub
     * OBJ(int,2) = <hub-num>
     * OBJ(int,3) = <start-port>
     * OBJ(int,4) = <end-port>
     * OBJ(int,5) = TRUE iff a hub-num was entered
     * OBJ(int,6) = TRUE iff a start-port was entered
     */

    int start_port_entered = GETOBJ(int,6);
    int start_port = GETOBJ(int,3);
    int end_port = GETOBJ(int,4);
    int port;
    int port_count;
    hub_access_t access;
    boolean show_aui_port = FALSE;

    port_count = hub_count_ports(hub->hub_type, hub->hub_number);
    access.hub_number = hub->hub_number;
    access.hub_type = hub->hub_type;

    /*
     * Figure out which hub ports to show.
     */
    if (!(start_port_entered)) {
        /* 
         * No start_port was entered (or end_port either)
         * Setup to show every port, including aui port.
         */
        start_port = 1;
        end_port = port_count;
        show_aui_port = TRUE;
    }

    /*
     * show the needed ports
     */
    if (access.hub_type == HUB_ETHER) {
        for (port = start_port; port <= end_port; port++) {
            if (port <= port_count) {
                hub_show_tp_port(port, &access);
            } else {
                errmsg(&msgsym(BADUNIT, HUB), port);;
                break;
            }
        }
        hub_show_ether_general(&access);
        if (show_aui_port) {
            hub_show_aui_port(&access);
        }
    } 

}  /* show_hub */


/*
 * =====================================================================
 * hub_show_int()
 *
 * Description:
 * If hub exists, display what hub this hwidb is supporting.
 *
 * Note that this function will be called for every interface in
 * any box that has registered this function -- ie. any box
 * containing a hub.
 *
 * Parameters:
 * idb - pointer to hwidbtype
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_show_int (hwidbtype *idb)
{
    if (idb->hub != NULL) {
	printf(", using hub %d", idb->hub->hub_number);
    }

}   /* hub_show_int */


/*
 * =====================================================================
 * hub_get_hub_type_str()
 * 
 * Description:
 * Helper function for writing nvram.  Given a hub_type, returns the
 * right character string to be written to nvram.
 *
 * Parameters:
 * hub_type 
 * 
 * Returns:
 * string to describe hub_type
 * =====================================================================
 */
char *hub_get_hub_type_str (int hub_type)
{
    if (hub_type == HUB_ETHER) {
        return("ether");
    } else
        return(NULL);
}  /* hub_get_hub_type_str */


/*
 * =====================================================================
 * clear_hub_command()
 *
 * Description:
 * Parse the "clear hub" command and call appropriate routines to 
 * either clear the hub counters or reset the hub.
 *
 * clear hub {ether|token} <hub-num>
 * clear hub counters [{ether|token} <hub-num> [<start-port> [<end-port>]]]
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * OBJ(int,1) = HUB_ETHER == Ether hub
 * OBJ(int,2) = <hub-num>
 * OBJ(int,3) = <start-port>
 * OBJ(int,4) = <end-port>
 * OBJ(int,5) = TRUE iff a start-port was entered
 * OBJ(int,10) = TRUE == clear counters
 *               FALSE == don't clear counters
 *
 * Returns:
 * None
 * =====================================================================
 */
void clear_hub_command (parseinfo *csb)
{
    hub_t *cur_hub;
    int hub_type = GETOBJ(int,1);
    int hub_num = GETOBJ(int,2);
    int start_port = GETOBJ(int,3);
    int end_port = GETOBJ(int,4);
    int port_num_entered = GETOBJ(int,5);
    int port = 0;
    char buffer[80];

    if (GETOBJ(int,10)) {
        switch (hub_type) {
        case HUB_UNSPECIFIED:
            /* clear all counters on all hubs */
            if (confirm("Clear \"show hub\" counters on all ports ")) {
                cur_hub = hub_list;
                while (cur_hub != NULL) {
       	          /* test for ether -- token ain't got counters */
                  if (cur_hub->hub_type == HUB_ETHER) {
                      reset_himib_counters(cur_hub->hub_type, 
                                           cur_hub->hub_number, 0);
                  }
                  cur_hub = cur_hub->next_hub;
                }
                errmsg(&msgsym(COUNTERS, CLEAR), "all", "hub ports", 
                    user_on_stdio_str());
            }
            break;
        case HUB_ETHER:
            /* clear only the specified ether hub counters */
            if (port_num_entered) {
                if (start_port == end_port) {
                    sprintf(buffer, "port %d", start_port);
                    printf("Clear \"show hub\" counters on %s ", buffer);
                    if (confirm("")) {
	              reset_himib_counters(hub_type, hub_num, start_port);
                    }
                    errmsg(&msgsym(COUNTERS, CLEAR), "hub", buffer, 
                       user_on_stdio_str());
                } else {
                    sprintf(buffer, "ports %d to %d", start_port, end_port);
                    printf("Clear \"show hub\" counters on %s ", buffer);
                    if (confirm("")) {
                        for (port = start_port; port <= end_port; port++) {
	                    reset_himib_counters(hub_type, hub_num, port);
                        }
                    }
                    errmsg(&msgsym(COUNTERS, CLEAR), "hub", buffer, 
                       user_on_stdio_str());
                }
            } else {
                /* reset counters for all ports */
                if (confirm("Clear \"show hub\" counters on all ports ")) {
                    reset_himib_counters(hub_type, hub_num, 0);
                }
                errmsg(&msgsym(COUNTERS, CLEAR), "all", "hub ports", 
                    user_on_stdio_str());
            }
            break;
        default:
            errmsg(&msgsym(BADHUB, HUB), hub_type, hub_num);
            break;
        }
    } else {
        /* clear hub */
	cur_hub = hub_get_hub_ptr(hub_type, hub_num);
	hub_reset(cur_hub);
	reg_invoke_hub_reset(hub_type);
    }
}   /* clear_hub_command */


/*
 * =====================================================================
 * show_hub_command()
 *
 * Description:
 * Parse the "show hub" command to call appropriate routines to show
 * hub statistics.
 *
 * show hub [{ether|token} <hub-num> [<start-port> [<end-port>]]]
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * OBJ(int,1) = HUB_ETHER == Ether hub
 * OBJ(int,2) = <hub-num>
 * OBJ(int,3) = <start-port>
 * OBJ(int,4) = <end-port>
 * OBJ(int,5) = TRUE iff a hub-num was entered
 * OBJ(int,6) = TRUE iff a start-port was entered
 *
 * Returns:
 * None
 * =====================================================================
 */
void show_hub_command (parseinfo *csb)
{
    hub_t *cur_hub = NULL;
    int hub_type = GETOBJ(int,1);
    int hub_num = GETOBJ(int,2);

    /* do the automore thing */
    automore_enable(NULL);

    switch (hub_type) {
    case HUB_UNSPECIFIED:
        /* display all hubs -- ether */
        cur_hub = hub_list;
        while (cur_hub != NULL) {
            show_hub(csb, cur_hub);
            cur_hub = cur_hub->next_hub;
        }
        break;
    case HUB_ETHER:
        /* display only the specified ether hub */
        cur_hub = hub_get_hub_ptr(hub_type, hub_num);
        if (cur_hub == NULL) {
            errmsg(&msgsym(BADHUB, HUB), hub_type, hub_num);
            break;
        }
        show_hub(csb, cur_hub);
        break;
    default:
        errmsg(&msgsym(BADHUB, HUB), hub_type, hub_num);
    }

    /* do the automore thing */
    automore_disable();

}   /* show_hub_command */


/*
 * =====================================================================
 * hub_config_command()
 *
 * Description:
 * Parse the "config hub" command and subcommands to (en/dis)able
 * hub features.  Also, this routine is called to show config.
 *
 * hub {ether|token} <hub-num> <start-port> [<end-port>]
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * OBJ(int,1) = HUB_ETHER == Ether hub
 * OBJ(int,2) = <hub-num>
 * OBJ(int,3) = <start-port>
 * OBJ(int,4) = <end-port>
 * OBJ(int,5) = <ring-speed>   
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_config_command (parseinfo *csb)
{
    int hub_type = GETOBJ(int,1);
    int hub_num = GETOBJ(int,2);
    hub_t *cur_hub;
    hub_device_t *cur_device;
    hub_ether_t *hub_ether;
    int port;
    int log_port;
    int port_mask = 0;
    hub_access_t access;
    boolean okay;
    uchar *macaddr = NULL;
    boolean macaddr_config = FALSE;
    uchar *trap_name = "illegal-address";

    
    if (csb->nvgen) {
        switch (csb->which) {
        case HUB_CONFIG:
            for (cur_hub = hub_list; cur_hub != NULL; 
                 cur_hub = cur_hub->next_hub) {
                log_port = 1;
                for (cur_device = cur_hub->hub_device; cur_device != NULL;
                     cur_device = cur_device->next_hub_device) {
                    for (port = 0; port < cur_device->num_ports; port++) {
                        port_mask = GET_PORT_MASK(port);
               	        nv_write(TRUE, "!\n%s %s %d %d", csb->nv_command,
                            hub_get_hub_type_str(cur_hub->hub_type),
               	            cur_hub->hub_number, log_port);
                        set_mode_byname(&csb->mode, "hub", MODE_VERBOSE);
                        if (cur_hub->hub_type == HUB_ETHER) {
                            hub_ether = cur_device->hub_ether;
       	                    if (hub_ether->link_test & port_mask) {
                                /* link test function is enabled */
                                nv_write(TRUE, "link-test");
			    } else {
                                nv_write(TRUE, "no link-test");
                            }
                            if (hub_ether->automatic_polarity & port_mask) {
                                nv_write(TRUE, "auto-polarity");
                            } else {
                                nv_write(TRUE, "no auto-polarity");
                            }
                            if (hub_ether->source_addr_change_int & port_mask) {
                                if (hub_ether->source_addr_config & port_mask) {
                                    macaddr = 
                                        hub_ether->port_info[port].source_addr;
                                    nv_write(TRUE,"source-address %e", macaddr);
                                } else {
                                    nv_write(TRUE,"source-address");
                                }
                            }
			    if (hub_ether->port_info[port].illegal_addr_trap_enabled
				== TRAP_ENABLED_TRUE) {
                                nv_write(TRUE,"snmp trap %s", trap_name);
                                hub_illegal_addr_trap_process_creation();
                            }
                        } 
                        if (!(cur_device->enabled & port_mask)) {
                            /* this port is disabled */
                            nv_write(TRUE, "shutdown");
                        }
                        csb->mode = config_mode;
                        log_port++;
		    }   /* for each port in device */
                }       /* for each device in hub */
	    }           /* for each hub in box */
            csb->nv_command[0] = '\0';
	    break;
        case HUB_SHUTDOWN:
        case HUB_LINK_TEST:
        case HUB_AUTOMATIC_POLARITY:
        case HUB_SOURCE_ADDRESS:
        case HUB_SNMP_TRAP:
	    break;
        case HUB_INVALID:
        default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /* not nvgen */

    switch (csb->which) {
    case HUB_CONFIG:
        /*
         * Set csb->hub to the selected hub for future config subcommands.
         */
        csb->hub = hub_get_hub_ptr(hub_type, hub_num);
        if (csb->hub == NULL) {
            errmsg(&msgsym(BADHUB, HUB), hub_type, hub_num);
            break;
        }

        csb->hub_port1 = GETOBJ(int,3);
        csb->hub_port2 = GETOBJ(int,4);
	set_mode_byname(&csb->mode, "hub", MODE_VERBOSE);
        break;

    case HUB_LINK_TEST:
    case HUB_AUTOMATIC_POLARITY:
	if (csb->set_to_default)
	    csb->sense = TRUE;
	/* fall through */
    case HUB_SHUTDOWN:
        cur_hub = csb->hub;

        /* some error checking */
        if (cur_hub == NULL) {
            errmsg(&msgsym(BADHUB, HUB), 0, 0);
            break;
        }
        if (cur_hub->hub_type == HUB_UNSPECIFIED) {
            errmsg(&msgsym(BADHUB, HUB), cur_hub->hub_type,cur_hub->hub_number);
            break;
        }

        /*
         * No need to figure out which hub ports to enable/disable
         * since hub_config_command already setup csb->hub_port1
	 * and csb->hub_port2.
         */
    
        /*
         * Set up access structure for use in accessing the hardware.
         */
        access.hub_number = cur_hub->hub_number;
        access.hub_type = cur_hub->hub_type;

        /*
         * For each port to be enabled/disabled, map that port onto a
         * physical port/device pair, then call to enable/disable.
         */
        for (port = csb->hub_port1; port <= csb->hub_port2; port++) {
            okay = hub_map_port(port, &access);
            if (!okay) {
                errmsg(&msgsym(BADUNIT, HUB), port);;
                return;
            }

            if (access.hub_type == HUB_ETHER) {
    	        hub_ether_config_port(csb->sense, &access, csb->which);
            } 
        }
        break;

    case HUB_SOURCE_ADDRESS:
        cur_hub = csb->hub;

        /* some error checking */
        if (cur_hub == NULL) {
            errmsg(&msgsym(BADHUB, HUB), 0, 0);
            break;
        }
        if (cur_hub->hub_type != HUB_ETHER) {
            errmsg(&msgsym(BADHUB, HUB), cur_hub->hub_type,cur_hub->hub_number);
            break;
        }

        /*
         * No need to figure out which hub ports to enable/disable
         * since hub_config_command already setup csb->hub_port1
	 * and csb->hub_port2.
         */
    
        /*
         * Set up access structure for use in accessing the hardware.
         */
        access.hub_number = cur_hub->hub_number;
        access.hub_type = cur_hub->hub_type;

        /*
         * For each port to be enabled/disabled, map that port onto a
         * physical port/device pair, retrieve mac-address if one was
         * entered, then (en/dis)/able the source address control.
         */
        for (port = csb->hub_port1; port <= csb->hub_port2; port++) {
            okay = hub_map_port(port, &access);
            if (!okay) {
                errmsg(&msgsym(BADUNIT, HUB), port);;
                return;
            }

            macaddr_config = GETOBJ(int,1);
            if (macaddr_config) {
                macaddr = GETOBJ(hwaddr,1)->addr;
                ieee_copy(macaddr, access.buffer);
            }
            hub_ether_src_addr_ctrl(csb->sense, &access, macaddr_config);
        }
        break;

    case HUB_SNMP_TRAP:
        cur_hub = csb->hub;

        /* some error checking */
        if (cur_hub == NULL) {
            errmsg(&msgsym(BADHUB, HUB), 0, 0);
            break;
        }
        if (cur_hub->hub_type != HUB_ETHER) {
            errmsg(&msgsym(BADHUB, HUB), cur_hub->hub_type,cur_hub->hub_number);
            break;
        }

        /*
         * No need to figure out which hub ports to enable/disable
         * since hub_config_command already setup csb->hub_port1
	 * and csb->hub_port2.
         */
    
        /*
         * Set up access structure for use in accessing the hardware.
         */
        access.hub_number = cur_hub->hub_number;
        access.hub_type = cur_hub->hub_type;

        /*
         * For each port to be enabled/disabled, map that port onto a
         * physical port/device pair, retrieve mac-address if one was
         * entered, then (en/dis)/able the source address control.
         */
        for (port = csb->hub_port1; port <= csb->hub_port2; port++) {
            okay = hub_map_port(port, &access);
            if (!okay) {
                errmsg(&msgsym(BADUNIT, HUB), port);
                return;
            }
            if (csb->sense && GETOBJ(int,1)) {
                access.device->hub_ether->port_info[access.phys_port]
		    .illegal_addr_trap_enabled = TRAP_ENABLED_TRUE;
                hub_illegal_addr_trap_process_creation();
            } else {
		  access.device->hub_ether->port_info[access.phys_port]
		      .illegal_addr_trap_enabled = TRAP_ENABLED_FALSE;
              }		  
        }
        break;

    case HUB_INVALID:
    default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }

}   /* hub_config_command */


/*
 * =====================================================================
 * hub_exit_command()
 *
 * Description:
 * Set hub fields in parseinfo to unused state when exiting "config hub".
 *
 * Parameters:
 * csb - pointer to parseinfo
 *
 * Returns:
 * None
 * =====================================================================
 */
void hub_exit_command (parseinfo *csb)
{
    exit_config_submode(csb);

    /* return csb->hub values to unused state */
    csb->hub = NULL;
    csb->hub_port1 = 0;
    csb->hub_port2 = 0;
}   /* hub_exit_command */

