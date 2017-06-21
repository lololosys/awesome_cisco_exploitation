/* $Id: config_history.c,v 3.3 1995/11/17 18:58:20 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/config_history.c,v $
 *------------------------------------------------------------------
 * Configuration History Database
 *
 * May 1995, Bob Stewart
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config_history.c,v $
 * Revision 3.3  1995/11/17  18:58:20  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:28:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:16:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:30  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/15  22:40:07  thille
 * Add placeholders for Bob's config history feature and MIB.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "ttysrv.h"
#include "connect.h"
#include "address.h"
#include "sys_registry.h"
#include "../snmp/config_history.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/sr_confmanmib.h"

/*
 * The history data base came into being as a result of a new SNMP
 *  Configuration Management MIB.  Although modeled for those needs it is
 * implemented to be separate from SNMP.
 *
 * Although intended to be fully separate, for short-term convenience while
 * its proper home is determined, the data base and its support procedures
 * are attached to the SNMP module.
 *
 * The history data base is kept updated by the various places that change
 * configuration status calling config_history_event and initialized here with
 * config_history_init.
 *
 * The history table is circular, that is, when it is full, indexing wraps
 * back to the beginning and reuses the oldest entries.  The most recent
 * entry in the table is the one circularly before next_entry.  The
 * next entry to record into is the one at next_entry.  Entries that
 * contain valid information have a non-zero serialNumber.
 *
 * The event serial number is  monotonically increasing and wraps, skipping 
 * zero.  The next value for it is next_serial.
 *
 */

/* History Scalars */

unsigned long running_last_changed;
unsigned long running_last_saved;
unsigned long startup_last_changed;
unsigned long entries_bumped;

/* History Table */

confighistoryentry_t config_history[MAX_HISTORY];

/* Internal State */

static int next_entry = 0;

static unsigned long next_serial;   /* leave space to recognize overflow */


/* External Procedures */

/* config_history_find
 * 
 * Find the configuration history entry with a serial number equal to or
 * greater than the one supplied, as controlled by an input option)
 *
 */

confighistoryentry_t *config_history_find (long want_serial_number,
					   boolean want_equal) 
{
    long least_serial;
    int best_entry;

    /* find the entry with the least serial number */
    best_entry = next_entry;
    least_serial = config_history[best_entry].serial_number;

    /* check for table that hasn't wrapped or is empty */
    if (least_serial == 0) {
	if (best_entry == 0) {
	    return(NULL);
	} else {
	    least_serial =
		config_history[0].serial_number;
	    best_entry = 0;

	    if (want_serial_number >= (least_serial + next_entry))
		return(NULL);
	}
    } else {
	if (want_serial_number >= (least_serial + MAX_HISTORY))
	    return(NULL);
    }

    /* check for at or before the least value */
    if (want_serial_number < least_serial) {
	if (want_equal) {
	    return(NULL);
	} else {
	    return(&config_history[best_entry]);
	}
    }

    /* must be in what we have */
    best_entry = want_serial_number - least_serial + best_entry;
    if (best_entry >= MAX_HISTORY)
	best_entry -= MAX_HISTORY;
    return(&config_history[best_entry]);
}


/* config_history_event
 *
 * Record a configuration event in the history table and send a notification.
 *
 */

void config_history_event (int command_source,
			   int data_source,
			   int data_destination,
			   ipaddrtype server_ip,
			   char *config_file, 
			   char *rcp_user)
{
   confighistoryentry_t *entry;
    int i;
    addrtype *snmp_source;


    /* Make sure we've been called with valid enumerated values.  If not,
     * ignore the event. */

    if (command_source < 1 || command_source > CONFIG_COMMAND_SOURCE_MAX
	|| data_source < 1 || data_source > CONFIG_MEDIUM_MAX
	|| data_destination < 1 || data_destination > CONFIG_MEDIUM_MAX)
	return;

    /* start a new entry, check for wrapping stuff */

    entry = &config_history[next_entry];
    next_entry++;
    if (next_entry >= MAX_HISTORY)
	/* wrap in the circular table */
	next_entry = 0;

    if (entry->serial_number != 0)
	/* bumping an old entry */
	entries_bumped++;

    entry->serial_number = next_serial;
    next_serial++;
    if (next_serial > MAX_SERIAL) {
	/* wrap the BIG index space and flush because its way easier
	 * than trying to deal with gaps and bignums and stuff
	 *
	 * if this ever actually happens, send salt for my hat
	 */
	for (i=0; i < MAX_HISTORY; i++)
	    config_history[i].serial_number = 0;

	entry = &config_history[0];
	entry->serial_number = 1;

	next_entry = 1;
	next_serial = 2;
	entries_bumped += MAX_HISTORY - 1;
    }

    /* first, set entry values from event procedure call */

    entry->timestamp = snmp_sysUpTime();

    if (data_destination == CONFIG_MEDIUM_RUNNING)
	running_last_changed = entry->timestamp;

    if (data_source == CONFIG_MEDIUM_RUNNING)
	running_last_saved = entry->timestamp;

    if (data_destination == CONFIG_MEDIUM_STARTUP)
	startup_last_changed = entry->timestamp;

    entry->command_source = command_source;

    entry->data_source = data_source;

    entry->data_destination = data_destination;

    entry->server_address = server_ip;

    setstring((char **)&(entry->file_name), config_file);

    setstring((char **)&(entry->rcp_user), rcp_user);

    /* next, set remainder of entry to defaults */

    entry->terminal_type = CONFIG_TERMINAL_NOTAPPLICABLE;

    entry->terminal_number = -1;

    if (entry->terminal_user) {
	free(entry->terminal_user);
	entry->terminal_user = NULL;
    }

    if (entry->terminal_location) {
	free(entry->terminal_location);
	entry->terminal_location = NULL;
    }

    entry->command_source_address = 0;

    if (entry->virtual_host_name) {
	free(entry->virtual_host_name);
	entry->virtual_host_name = NULL;
    }

    /* finally, set remainder of values according to command source */

    if (command_source == CONFIG_COMMAND_SOURCE_COMMANDLINE) {
	if (stdio) {
	    switch (stdio->type) {
	    case CTY_LINE:
		entry->terminal_type = CONFIG_TERMINAL_CONSOLE;
		break;
	    case TTY_LINE:
		entry->terminal_type = CONFIG_TERMINAL_TERMINAL;
		break;
	    case VTY_LINE:
		entry->terminal_type = CONFIG_TERMINAL_VIRTUAL;
		break;
	    case AUX_LINE:
		entry->terminal_type = CONFIG_TERMINAL_AUXILIARY;
		break;
	    default:
		entry->terminal_type = CONFIG_TERMINAL_UNKNOWN;
	    }
	    entry->terminal_number = stdio->ttynum;
	    setstring((char **)&(entry->terminal_user), stdio->user);
	    setstring((char **)&(entry->terminal_location), stdio->location);
	}
	if (stdio && stdio->conntop) {
	    if (stdio->conntop->address.type == ADDR_IP) 
		entry->command_source_address = stdio->conntop->address.ip_addr;
	    setstring((char **)&(entry->virtual_host_name),
		      stdio->conntop->hostname);
	}
    } else if (command_source == CONFIG_COMMAND_SOURCE_SNMP) {
	snmp_source = reg_invoke_snmp_packet_source();
	if (snmp_source->type == ADDR_IP)
	    entry->command_source_address = snmp_source->ip_addr;
    }

    /* generate the notification */
    reg_invoke_ciscoConfigManEvent(entry->serial_number);
}


/* config_history_init
 *
 * Initialize configuration history data structures and register handler.
 *
 */

static void init_config_history (subsystype *subsys)
{
    int i;

    running_last_changed = 0;
    running_last_saved = 0;
    startup_last_changed = 0;
    entries_bumped = 0;

    for (i=0; i < MAX_HISTORY; i++) {
	config_history[i].terminal_user = NULL;
	config_history[i].terminal_location = NULL;
	config_history[i].virtual_host_name = NULL;
	config_history[i].file_name = NULL;
	config_history[i].rcp_user = NULL;
	config_history[i].serial_number = 0;
    }

    next_entry = 0;
    next_serial = 1;

    reg_add_config_history_event(config_history_event, "config_history_event");
}

/*
 * config_history subsystem header
 */

#define MAJVERSION_config_history 1
#define MINVERSION_config_history 0
#define EDITVERSION_config_history 0

SUBSYS_HEADER(config_history,
              MAJVERSION_config_history,
              MINVERSION_config_history,
              EDITVERSION_config_history,
              init_config_history,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
