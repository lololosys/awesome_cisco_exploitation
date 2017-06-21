/* $Id: if_groups.c,v 3.6.18.8 1996/09/03 22:37:14 sjackson Exp $
 * $Source: /release/112/cvs/Xsys/if/if_groups.c,v $
 *------------------------------------------------------------------
 * Group interface abstraction
 *
 * April 1995, Tim Kolar
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_groups.c,v $
 * Revision 3.6.18.8  1996/09/03  22:37:14  sjackson
 * CSCdi66297:  All interfaces reset (all calls dropped) when group-range
 * applied
 * Avoid reconfiguring existing interfaces within the group range
 * Branch: California_branch
 *
 * Revision 3.6.18.7  1996/08/07  08:59:49  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.6.18.6  1996/07/02  19:58:12  tkolar
 * CSCdi48408:  check for enough room before writing into a buffer
 * Branch: California_branch
 * Handle excessively large interface configurations gracefully.
 *
 * Revision 3.6.18.5  1996/06/24  22:54:19  perryl
 * CSCdi59423:  Reconfiguring virtual template causes configuration errors
 * Branch: California_branch
 * Use "default" instead of "no" to unconfigure commands in
 * add_unconfigured_command().
 *
 * Revision 3.6.18.4  1996/05/21  09:49:51  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.18.3  1996/04/27  05:49:41  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.10.2  1996/04/26  01:06:25  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.10.1  1996/04/17  01:59:28  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.6.18.2  1996/04/15  14:58:06  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.6.18.1  1996/03/18  20:13:51  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.3  1996/03/07  09:44:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.2  1996/02/20  00:57:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/02/08  03:21:26  anke
 * CSCdi48405:  Grammar are bad in output
 *
 * Revision 3.5  1996/01/29  22:04:04  tkolar
 * CSCdi47712:  Group interfaces can fail to find interface configurations
 * Look for "<newline>interface asyncN" instead of just "interface asyncN"
 *
 * Revision 3.4.2.1  1996/01/24  22:18:21  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1996/01/18  23:00:51  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/12/17  18:28:49  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.2  1995/11/17  09:28:00  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:50:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/22  06:05:15  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Fix over-zealousness in limiting output of "Building Configuration..."
 *
 * Revision 2.3  1995/06/23  18:47:49  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.2  1995/06/19  01:05:49  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  20:52:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "config.h"
#include "../os/nv.h"
#include "if_groups.h"
#include "../parser/parser_actions.h"

/*
 * These are the command prefixes which are used *only* by
 * master interfaces.  We use this to recognize them when
 * moving configs from master -> member.
 */
static const char *const master_cmds[] = { "group-range", "member", NULL};

/*
 * is_master_cmd(uchar *)
 */

static boolean is_master_cmd (uchar *command)
{
    int count = 0;

    while ((NULL != master_cmds[count])) {
	if (!strncmp(command, master_cmds[count], strlen(master_cmds[count])))
		return(TRUE);
	count++;
    }
    return(FALSE);
}


/* 
 * generate_command_list 
 *    chop up a buffer full of interface configuration commands into an array 
 */
int generate_command_list (uchar *inbuf, uchar *outbuf, int inbuf_size)
{
    int count, command_count = 0, offset = 0;
    uchar recording = FALSE;

    for (count = 0; count < inbuf_size; count++) {
	/* 
	 * new line followed by a space marks an entry we want to record.
	 *   Don't copy master commands.
	 */
	if (inbuf[count] == NEW_LINE) {
	    count++;
	    if ((inbuf[count] == ' ') &&
			!is_master_cmd(&inbuf[count+1])) {
	        outbuf[offset] = NEW_LINE;
	        offset++;
		recording = TRUE;
	        command_count++;
	    } else {
		recording = FALSE;
   	    }
	} else {
	    if (recording) {
		outbuf[offset] = inbuf[count];
		offset++;
		if (offset > MAX_COMMAND_BUFSIZE)
		    return(0);
	    }
	}
    }
    outbuf[offset] = NEW_LINE;
    outbuf[offset+1] = 0;
    return(command_count);
}

/*
 * add_unconfigured_command
 *
 * This is the tough bit.  This function takes a string representing a
 *  cisco configuration command and replaces it with the the string 
 *  necessary to reset the command back to its default value, ie. it
 *  unconfigures the command.
 * This function use the default command. When the keyword, default, 
 * prefix with any cisco configuration command. The default value of
 * the command is applied.
 * 
 */

#define NOSTRING "no "
#define DEFAULTSTRING "default "
boolean add_unconfigured_command (uchar *config_string, uchar *command)
{
    int len = 0;
    int nostring_len = strlen(NOSTRING);

    if (command == NULL) 
	return(FALSE);
    /* 
     *  Figure out the length of this command
     */
    while ((command[len] != '\n') && (command[len] != 0))
	len++;
    len += 1;

    if ((len + strlen(DEFAULTSTRING) + strlen(config_string) + 2) 
			> MAX_COMMAND_BUFSIZE) {
	/* there's no room for the "default" string and the ending
		new-line and NULL */
	printf("Defaulted buffer is too big\n");
	return(FALSE);
    }

    /*
     * if this is a default command, we do not need to
     * unconfigure, simply return
     */
    if (!strncmp(DEFAULTSTRING, command, strlen(DEFAULTSTRING))) 
	return(TRUE);
    /* 
     *  if there isn't a "no " in front the command, 
     *  put a "default " in front the command.
     */
    if (strncmp(NOSTRING, command, nostring_len)) {
	strcat(config_string, DEFAULTSTRING);
	sstrncat(config_string, command, len);
    } else {
	/*
	 * if there's a no in front the command
	 * replace it with a "default "
	 */
	strcat(config_string, DEFAULTSTRING);
	sstrncat(config_string, command+nostring_len, len-nostring_len);
    }

    len = strlen(config_string);
    config_string[len++] = NEW_LINE;
    config_string[len]   = '\0';
    return(TRUE);
}

/* 
 * end_of_interface
 *
 *  Take a pointer to the beginning of an configuration interface 
 * specification return a pointer to the end of it.  
 *
 *  The end of an interface is defined as "The first line that doesn't start
 * with a space or an exclamation mark."
 *
 */

uchar *end_of_interface (uchar *config) 
{
    uchar c;
    int state = STATE_NORMAL;

    do {
	c = *config++;
	switch(state) {
	  case STATE_NORMAL:
	      if (c == NEW_LINE)
		  state = STATE_FOUND_NEW_LINE;
	      break;
	  case STATE_FOUND_NEW_LINE:
	      if ((c != ' ') && (c != '!'))
		  return(--config);
	      state = STATE_NORMAL;
	      break;
	  default:
	      buginf("\nBogus case %d in end_of_interface", state);
	}
    } while ( c != '\0');

    return(NULL);
	
}

/*
 *  Take a list of commands and return a ptr to the start of the n'th one.
 */

uchar *get_command_entry (uchar *command_list, int n) 
{
    int count;
    uchar *ptr = command_list;

    for (count = 0; count < n; count++) {
	/* look for new lines */
	while(*ptr++ != '\n') { 
	    if (ptr-command_list > MAX_COMMAND_BUFSIZE) {
		return(NULL);
	    }
	}
    }
    return(ptr);
}



/*
 * unconfigure_interfaces(configuration, config_size, 
 *		interface_type, low_unit, high_unit, begin_skip, end_skip)
 *
 * Do a pretty good job of undoing what someone did to a set of interfaces, 
 * and try to delete them from the config.
 *
 * The interface_type string should be the name that the parser uses.  See
 * the IFNAME_* strings in parser_defs_parser.h .
 *
 * No one said life was pretty.
 */


void unconfigure_interfaces (uchar *config_buf, uint nv_size, 
			     uchar *interface_type, int low_unit,
			     int high_unit, int begin_skip, int end_skip)
{
    uchar *inter;
    uchar *my_buf;
    uchar *command_list;
    uchar *out_buf;
    uchar interface_name[INTERFACE_NAME_SIZE];
    uint num_commands;
    int count;
    int unit;
    uint my_bufsize;
    uint config_bufsize;
    boolean local_config = FALSE;
    int outbuf_len;

    /* 
     * if there wasn't a config passed to us, make one locally 
     */
#ifdef DEBUGGROUP
    buginf("\nUnconfigure %s %d %d", interface_type, low_unit, high_unit);
#endif
    if (config_buf == NULL) {
	config_bufsize = nv_get_config_size();
	nv_size = config_bufsize;
	while (((config_buf = malloc(nv_size)) == NULL) && (nv_size > 0x3fff)){
	    nv_size = nv_size>>1;
	}
	if (config_buf == NULL) {
	    return;
	}
        if (nv_current_to_buffer(config_buf, nv_size, TRUE,
				 PRIV_ROOT, FALSE) > 0) {
	    local_config = TRUE;
	} else {
	    free(config_buf);
	    return;
	}
    }


    /* 
     * get a set of buffers to save interface configurations and 
     *   command lists in.
     */
    my_buf = malloc(MAX_COMMAND_BUFSIZE);
    command_list = malloc(MAX_COMMAND_BUFSIZE);
    out_buf = malloc(MAX_COMMAND_BUFSIZE);

    if ((my_buf == NULL) || (command_list == NULL) || (out_buf == NULL)) {
	free(my_buf);
	free(command_list);
	free(out_buf);
	if (local_config) {
	    free(config_buf);
	}
	return;
    }

    for (unit = low_unit; unit <= high_unit; unit++) {
        if ((unit >=  begin_skip) && (unit <= end_skip)) {
            continue;
        }
	sprintf(interface_name, "\ninterface %s%d\n", interface_type, unit);

	/* 
	 * Step 1
	 * 
	 * Grab the configuration for this interface, and store it in
	 * my_buf
	 */
	inter = strstr(config_buf, interface_name);
	if (inter == NULL) {
	     /* 
	      * The interface exists, but it's not in the configuration.
	      *    This means that it is already a member of this async
	      *    group, and thus needs no new configuration 
	      */
	    continue;
	}
	inter++;  /* skip the initial new line */
	my_bufsize = (int) end_of_interface(inter) - (int) inter;
	if (my_bufsize > MAX_COMMAND_BUFSIZE) {
	    printf("Configuration on %s was too big to copy\n");
	    continue;
	}
	bcopy(inter, my_buf, my_bufsize);
	my_buf[my_bufsize] = '\0';

	/*
	 * Step 2
	 *
	 * Parse my_buf into an array of individual commands, command_list
	 */

	memset(command_list, 0, MAX_COMMAND_BUFSIZE);
	num_commands = generate_command_list(my_buf, command_list, 
				    my_bufsize);

        /*
	 * Step 3
	 *
	 * Create the final command string out_buf, by
	 * 	a) inserting the interface name at the front
	 *      b) stepping through the command_list backwards, inserting the
	 *		negated command in out_buf
	 *	c) putting an "end" on to reset the parser context
	 */

	memset(out_buf, 0, MAX_COMMAND_BUFSIZE);
	strcat(out_buf, interface_name);

	for (count = num_commands; count > 0; count--) {
	    if (!add_unconfigured_command(out_buf, 
		   get_command_entry(command_list, count))) {
		break;
	    }
	}

	if (strlen(out_buf) + strlen("end") + 2 > MAX_COMMAND_BUFSIZE) {
	    /*  not enough room for the end of the buffer */
	    printf("Defaulted buffer is too big for %s\n", interface_name);
	} else {
	    strcat(out_buf, "end");
	    outbuf_len = strlen(out_buf);
	    out_buf[outbuf_len++] = NEW_LINE;
	    out_buf[outbuf_len] = '\0';

	    /*
	     * Step 4
	     *
	     * Execute out_buf as a set of configuration commands.
	     */
	    parse_configure((char *)out_buf, TRUE, RES_CONFIG, PRIV_ROOT);
	}

    }

    free(my_buf);
    free(command_list);
    free(out_buf);
    /* if we made the config buffer, destroy it on the way out */
    if (local_config)
	free(config_buf);
}

/* 
 * sync_to_master_interface
 *
 * This causes all of the commands that have been executed against a master
 *  or group leader interface to be applied to all of the interfaces 
 *  specified in the fucntion call.
 */

void sync_to_master_interface (uchar *config_buf, uint nv_size, 
			       uchar *master_interface_type, int master_unit, 
			       uchar *interface_type, 
			       int low_unit, int high_unit,
			       int begin_skip, int end_skip)
{
    uchar *inter, *my_buf, *out_buf;
    uchar interface_name[INTERFACE_NAME_SIZE];
    int my_bufsize, count, ncmd;

#ifdef DEBUGGROUP
    buginf("\nSync_to_master %s %d-%d", interface_type, low_unit, high_unit);
#endif
    /*
     * Pass me a real config...
     */
    if (config_buf == NULL)
       return;

    my_buf = malloc(MAX_COMMAND_BUFSIZE);
    out_buf = malloc(MAX_COMMAND_BUFSIZE);
    if ((my_buf == NULL) || (out_buf == NULL)) {
	free(my_buf);
	free(out_buf);
	return;
    }

    /* 
     * 1) Grab the configuration off of the master interface.
     * 2) Weed out master-only config commands
     * 3) Apply it to all of the normal interfaces specified.
     */
    sprintf(interface_name, "\ninterface %s%d\n", master_interface_type, 
				master_unit);
    inter = strstr(config_buf, interface_name);
    inter++;   /* skip the initial newline */
    my_bufsize = end_of_interface(inter) - inter;
    if (my_bufsize > MAX_COMMAND_BUFSIZE) {
	printf("Configuration on %s was too big to copy.\n");
	free(my_buf);
	free(out_buf);
	return;
    }
    ncmd = generate_command_list(inter, my_buf, my_bufsize);

    for (count = low_unit; count <= high_unit; count++) {
        if ((count >= begin_skip) && (count <= end_skip)) {
            continue;
        }
	sprintf(out_buf, "interface %s%d\n%send\n", interface_type, count,
			my_buf);
	parse_configure((char *)out_buf, TRUE, RES_CONFIG, PRIV_ROOT);
    }

    free(my_buf);
    free(out_buf);
}
