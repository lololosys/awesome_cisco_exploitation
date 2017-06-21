/* $Id: config_history.h,v 3.2 1995/11/17 18:58:21 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/snmp/config_history.h,v $
 *------------------------------------------------------------------
 * Configuration History Database
 *
 * May 1995, Bob Stewart
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: config_history.h,v $
 * Revision 3.2  1995/11/17  18:58:21  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:16:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:25:31  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/15  22:40:10  thille
 * Add placeholders for Bob's config history feature and MIB.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Parameters */

#define MAX_HISTORY 10          /* maximum table entries */
#define MAX_SERIAL 0x7FFFFFFF   /* maximum event serial number */


/* Enumerated Integers */

/* Note:  the values for these are the same as the enumerations in the
 * Configuration Management MIB.  They must be kept the same or that
 * assumption must be removed from the ConMan MIB code.
 */

#define CONFIG_COMMAND_SOURCE_COMMANDLINE 1
#define CONFIG_COMMAND_SOURCE_SNMP        2
#define CONFIG_COMMAND_SOURCE_MAX         2   /* used for range check */

#define CONFIG_MEDIUM_UNKNOWN      -1   /* unknown not for SNMP */
#define CONFIG_MEDIUM_ERASE         1
#define CONFIG_MEDIUM_COMMANDSOURCE 2
#define CONFIG_MEDIUM_RUNNING       3
#define CONFIG_MEDIUM_STARTUP       4
#define CONFIG_MEDIUM_LOCAL         5
#define CONFIG_MEDIUM_NETWORKTFTP   6
#define CONFIG_MEDIUM_NETWORKRCP    7
#define CONFIG_MEDIUM_MAX           7   /* used for range check */

#define CONFIG_TERMINAL_NOTAPPLICABLE 1
#define CONFIG_TERMINAL_UNKNOWN       2
#define CONFIG_TERMINAL_CONSOLE       3
#define CONFIG_TERMINAL_TERMINAL      4
#define CONFIG_TERMINAL_VIRTUAL       5
#define CONFIG_TERMINAL_AUXILIARY     6


/* Type Definitions */

typedef
struct _confighistoryentry_t {
        long            serial_number;
        unsigned long   timestamp;
        long            command_source;
        long            data_source;
        long            data_destination;
        long            terminal_type;
        long            terminal_number;
        char            *terminal_user;
        char            *terminal_location;
        unsigned long   command_source_address;
        char            *virtual_host_name;
        unsigned long   server_address;
        char            *file_name;
        char            *rcp_user;
} confighistoryentry_t;


/* Globals */

extern unsigned long running_last_changed;
extern unsigned long running_last_saved;
extern unsigned long startup_last_changed;
extern unsigned long entries_bumped;

extern confighistoryentry_t config_history[MAX_HISTORY];


/* Procedure Prototypes */

/*
 * config_history.c
 */ 
extern confighistoryentry_t *config_history_find(long want_serial_number,
						 boolean want_equal);
