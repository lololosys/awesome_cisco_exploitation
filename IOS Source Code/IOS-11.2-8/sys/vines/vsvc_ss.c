/* $Id: vsvc_ss.c,v 3.4.18.4 1996/09/11 15:52:29 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vsvc_ss.c,v $
 *------------------------------------------------------------------
 * vsvc_ss.c - Support for the `Server' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_ss.c,v $
 * Revision 3.4.18.4  1996/09/11  15:52:29  snyder
 * CSCdi68773:  more things const
 *              536 out of data section
 *              100 out of image
 * Branch: California_branch
 *
 * Revision 3.4.18.3  1996/07/16  10:34:32  ehk
 * CSCdi58105:  Vines time sync not updating starting epoch
 * Branch: California_branch
 * the original time sync problem reported does not exist in this
 * release. added new vines time sync related debug trace. fixed
 * minor vines time epoch display problem.
 *
 * Revision 3.4.18.2  1996/06/17  23:39:01  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.4.18.1  1996/03/18  22:33:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/13  02:05:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.2.26.2  1996/03/07  11:05:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  21:47:57  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/07  16:16:53  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.3  1996/02/01  06:11:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  17:59:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/08/25  12:49:04  hampton
 * Fix to VINES miscellaneous timer cleanup  [CSCdi39258]
 *
 * Revision 2.4  1995/08/25 11:45:42  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi39258]
 *
 * Revision 2.3  1995/08/03 17:27:27  slin
 * CSCdi37669: Vines Server Service Get Local Time returns wrong timezone
 *             offset
 *
 * Revision 2.2  1995/06/16  04:30:21  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  23:16:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#include "../os/time.h"
#include "master.h"
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "../os/clock.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"
#include "parser_defs_vines.h"
#include "../dev/monitor1.h"		/* for system version info */

#include "vsvc_ss.h"
#include "vsvc_ss.rpch"
#include "vsvc_ss.rpcc"

/*
 * Local variables set by configuration commands.
 */
static boolean vss_time_to_system;
static boolean vss_time_use_system;
static boolean vss_time_participate;
static int     vss_time_list;
static int	vss_time_dest_count;
static simplevinesaddr vss_time_dest[VSS_TIME_MAX_DEST];

/*
 * Local variables used to track the VINES time base
 */
       ulong   vss_time_base;		/* actually the delta from msclock */
static ulong   vss_time_adjustment;	/* adtl offset - used by test code */
static long    vss_time_epoch_age;
static ushort  vss_time_epoch_seq;
static char    vss_time_epoch_name[VST_TOTALLEN];
static ulong   vss_time_set_adjustment;
static sys_timestamp vss_time_set_when;
static ulong   vss_time_set_net;
static ushort  vss_time_set_host;
static ushort  vss_time_local_seq;

static const char vss_msg_time_from[] =
    "\nVSS: %s %s from %#z, time: %s\nVSS:     epoch %s-%u, age: %s";
static const char vss_msg_time_to[] =
    "\nVSS: %s %s to %#z, time: %s\nVSS:     epoch %s-%u, age: %s";
static const char vss_msg_time_to_many[] =
    "\nVSS: Sending Time Sync messages, time: %s\nVSS:     epoch %s-%u, age: %s";

/**************************************************
 *
 *		  SS/NTP Interaction
 *
 **************************************************/

/*
 * vss_update_time_from_ntp
 *
 * Import NTP time into Vines time.  Set up all the state information to
 * say that this time epoch started _now_, and it started at this server.
 */
static void vss_update_from_local_time (void)
{
    ulong clock, time;

    if (!clock_is_probably_valid())
	return;
    time = unix_time();
    clock = system_uptime_seconds();

    sstrncpy(vss_time_epoch_name, vsvc_ss_info.svc_name, VST_TOTALLEN);
    vss_time_epoch_seq = ++vss_time_local_seq;
    vss_time_epoch_age = 0 - clock;
    vss_time_base = time - clock + vss_time_adjustment;
    GET_TIMESTAMP(vss_time_set_when);
    vss_time_set_net = vines_network;
    vss_time_set_host = vines_host;
    vss_time_set_adjustment = 0;
}

/**************************************************
 *
 *		  Miscellaneous
 *
 **************************************************/

/*
 * vss_get_time
 *
 * Get the vines version of the time.
 */
ulong vss_get_time (void)
{
    if (vss_time_base == 0)
	return(0);
    return(vss_time_base + system_uptime_seconds());
}

/*
 * vss_get_start_time
 *
 * Get the vines version of the time.
 */
ulong vss_get_start_time (void)
{
    return(vss_time_base);
}


/**************************************************
 *
 *		  Test Interface
 *
 **************************************************/

void vss_set_time_adjustment (ulong offset)
{
    vss_time_adjustment += offset;
    vss_time_base += offset;
}

void vss_clear_time_adjustment (void)
{
    vss_time_base -= vss_time_adjustment;
    vss_time_adjustment = 0;
}


/**************************************************
 *
 *	   SS - Installed Services v3.x (4)
 *
 **************************************************/

/*
 * vss_inst_svcs_v3_rcv_call
 *
 * List all services installed on this server.  Make sure to completely
 * count the number of services on each pass.  Its only imporant the
 * first time, but its easiest to do it every time.
 */
ushort vss_inst_svcs_v3_rcv_call (
    vinesipcsocket *socket,
    vss_inst_svcs_v3_query *query,
    vss_inst_svcs_v3_reply *reply)
{
    vss_inst_svcs_v3_entry *entry;
    vsvc_info *service;
    int svc_count, entry_count;

    svc_count = entry_count = 0;
    FOR_ALL_SERVICES(service) {
	if (svc_count++ < query->pass)
	    continue;
	entry = &reply->entry[entry_count++];
	sstrncpy(entry->service_name, service->svc_name, VST_TOTALLEN);
	if (entry_count >= VSS_MAX_SVC_STATS)
	    break;
    }
    
    if (service != NULL)
	reply->pass = svc_count;
    reply->entry_count = entry_count;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	     SS - Get Statistics (6)
 *
 **************************************************/

/*
 * vss_get_stats_rcv_call
 *
 * Someone sent a Get Statistics query to the router.
 */
ushort vss_get_stats_rcv_call (
    vinesipcsocket *socket,
    vss_get_stats_query *query,
    vss_get_stats_reply *reply)
{
    vsvc_insert_hostname(reply->server_name);
    reply->users_count = 0;
    reply->service_count = 0;
    reply->time_booted = vss_time_base ? vss_time_base : 0;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	     SS - Get Local Time (12)
 *
 **************************************************/

/*
 * vss_get_local_time_rcv_call
 *
 * Get the local time from this server.
 */
ushort vss_get_local_time_rcv_call (
    vinesipcsocket *socket,
    vss_get_local_time_query *query,
    vss_get_local_time_reply *reply)
{
    timeval tv;
    ulong time;
    char  *tz_name;

    if (!vss_time_participate)
	return(VSVC_ERR_INVALID);
    
    time = vss_time_base + system_uptime_seconds();
    unix_time_to_timeval(time, &tv, &tz_name);

    reply->second       = tv.second;
    reply->minute       = tv.minute;
    reply->hour	        = tv.hour;
    reply->day_of_month = tv.day;
    reply->month	= tv.month;
    reply->year	        = tv.year;
    reply->day_of_week  = tv.day_of_week;
    reply->day_of_year  = tv.day_of_year;
    reply->dst	        = unix_time_is_in_summer(time);
    reply->tz_offset    = -tv.tz_offset / (long) SECSPERMIN;
    reply->raw_time     = time;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	      SS - Format Time (13)
 *
 **************************************************/

/*
 * vss_format_time_rcv_call
 *
 * Get the local time from this server.  The buffer for the formatted
 * string must be at least VSS_TIME_STRING_LENGTH bytes in size including
 * the NULL terminator.
 */
ushort vss_format_time_rcv_call (
    vinesipcsocket *socket,
    vss_format_time_query *query,
    vss_format_time_reply *reply)
{
    timeval tv;
    ulong time, hour;
    char *in, *out, *am_pm, *tz_name;
    char *tmpbuf;
    int NumCharsLeft, NumCharsProcessed = 0;
    
    time = query->raw_time;
    if (!time) {
	if (!vss_time_participate)
	    return(VSVC_ERR_INVALID);
	time = vss_time_base + system_uptime_seconds();
    }
    unix_time_to_timeval(time, &tv, &tz_name);

    in = query->format_string;
    out = reply->formatted_text;
    tmpbuf = string_getnext();
    NumCharsLeft = VSS_TIME_STRING_LENGTH - 1; /* Number of bytes left in */
                                               /* formatted string */
    
    while (*in && NumCharsLeft > 0) {
	if (*in != '%') {
	    *out++ = *in++;
	    NumCharsLeft--;
	    continue;
	}
	
	switch (in[1]) {
	  case 'w':
	    if (in[2] == '2') {
		NumCharsProcessed = ansi_sprintf(out, "%.*s", NumCharsLeft,
						 day_name[tv.day_of_week]);
	    } else {
		NumCharsProcessed = ansi_sprintf(out, "%.*s", NumCharsLeft,
						 long_day_name[tv.day_of_week]);
	    }
	    break;
	  case 'd':
	    sprintf(tmpbuf, "%s %d, %d", long_month_name[tv.month - 1],
		    tv.day, tv.year);
	    NumCharsProcessed = ansi_sprintf(out, "%.*s", NumCharsLeft,
					     tmpbuf);
	    break;
	  case 't':
	    hour = tv.hour;
	    am_pm = (hour >= 12) ? "pm" : "am";
	    hour = hour % 12;
	    if (hour == 0) hour = 12;
	    sprintf(tmpbuf, "%d:%02d:%02d %s", hour, tv.minute, tv.second,
		    am_pm);
	    NumCharsProcessed = ansi_sprintf(out, "%.*s", NumCharsLeft,
					     tmpbuf);
	    break;
	  case 'z':
	    NumCharsProcessed = ansi_sprintf(out, "%.*s", NumCharsLeft,
					     tz_name);
	    break;
	}
	in += 3;
	out += NumCharsProcessed;
	NumCharsLeft -= NumCharsProcessed;
    }
    *out = '\0';		/* Terminate string */
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	     SS - Get Time Info (16)
 *
 **************************************************/

/*
 * vss_get_time_info_send_call
 *
 * Send a Get Time Info query to a Banyan server in order to get the
 * current time epoch.  This is done shortly after boot time so that the
 * router does not have to wait for a thirty minute synchronization
 * message from a neighbor.  It seems safer to send this to a server who
 * will always respond, than to send it to a neighbor who might be a
 * cisco router that doesn't support this query.  ***This should be
 * changed to look for a neighbor sometime in the future after cisco has
 * supported the time service for a couple of releases.***
 */
static void vss_get_time_info_send_call (
    ulong net,
    ushort host)
{
    vinesipcsocket *socket;

    socket = vipc_create_conn(net, host, 0, 0, TRUE);
    if (!socket) {
	buginf(vsvc_nomem);
	return;
    }
    vss_get_time_info_send_call_(socket, NULL);
}

/*
 * vss_get_time_info_rcv_reply
 *
 * This is the response to a Get Time Info query.  Save this information
 * as the current Banyan time and time epoch.
 */
void vss_get_time_info_rcv_reply (
    vinesipcsocket *socket,
    vss_get_time_info_reply *reply)
{
    char text[20];
    char timestring[UNIX_TIME_STRING_MINBUF];
    ulong clock;

    clock = system_uptime_seconds();
    
    if (vss_debug) {
	vss_sprint_secs_in_dhms(text, reply->epoch_age);
	unix_time_string(timestring, reply->current_time);
	buginf(vss_msg_time_from, "Get Time Info", "response",
	       socket->remote_net, socket->remote_host,
	       timestring, reply->epoch_name,
	       reply->epoch_sequence, text);
    }

    if (!vss_time_participate)
	return;

    /*
     * Check access list
     */
    if (!vines_short_access_check(socket->remote_net,
				  socket->remote_host, vss_time_list)) {
	buginf("\nVSS: Failed input time list.");
	return;
    }

    /*
     * Save the information
     */
    sstrncpy(vss_time_epoch_name, reply->epoch_name, VST_TOTALLEN);
    vss_time_epoch_seq = reply->epoch_sequence;
    vss_time_epoch_age = reply->epoch_age - clock;
    vss_time_base = reply->current_time - clock;
    GET_TIMESTAMP(vss_time_set_when);
    vss_time_set_net = socket->remote_net;
    vss_time_set_host = socket->remote_host;

    /*
     * Should this be exported to the system?
     */
    if (vss_time_to_system) {
	if (clock_set_unix(reply->current_time, CLOCK_SOURCE_VINES))
	    clock_is_now_valid();
    }
}

/*
 * vss_get_time_info_rcv_call
 *
 * Someone sent a Get Time Info query to the router.  If the current time
 * epoch is known, build a response frame and send it back to the source.
 * If not, send an abort message back to the source.
 */
ushort vss_get_time_info_rcv_call (
    vinesipcsocket *socket,
    vss_get_time_info_query *query,
    vss_get_time_info_reply *reply)
{
    ulong clock, now, age;
    char text[20];
    char timestring[UNIX_TIME_STRING_MINBUF];

    if (!vss_time_participate || !vss_time_base) {
	return(VSS_ERR_SERVER_INIT);
    }

    clock = system_uptime_seconds();
    now = vss_time_base + clock;
    age = vss_time_epoch_age + clock;

    reply->current_time = now;
    sstrncpy(reply->epoch_name, vss_time_epoch_name, VST_TOTALLEN);
    reply->epoch_sequence = vss_time_epoch_seq;
    reply->epoch_age = age;

    if (vss_debug) {
	vss_sprint_secs_in_dhms(text, age);
	unix_time_string(timestring, now);
	buginf(vss_msg_time_to, "Get Time Info", "response", 
	       socket->remote_net, socket->remote_host,
	       timestring, vss_time_epoch_name,
	       vss_time_epoch_seq, text);
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *		SS - Time Set (17)
 *
 **************************************************/

/*
 * vss_time_set_rcv_call
 *
 * Set the local time to the time in the message.
 */
ushort vss_time_set_rcv_call (
    vinesipcsocket *socket,
    vss_time_set_query *query,
    vss_time_set_reply *reply)
{
    boolean duplicate;
    ulong clock;
    char timestring[UNIX_TIME_STRING_MINBUF];

    duplicate = ((strcmp(query->node_name, vss_time_epoch_name) == 0) &&
		 (query->node_sequence == vss_time_epoch_seq));
    if (vss_debug) {
	unix_time_string(timestring, query->current_time);
	buginf(vss_msg_time_from, "Time Set", "message",
	       socket->remote_net, socket->remote_host,
	       timestring, query->node_name,
	       query->node_sequence, 0);
    }

    if (!vss_time_participate || duplicate)
	return(VSVC_NO_ERROR);

    /*
     * Check access list
     */
    if (!vines_short_access_check(socket->remote_net,
				  socket->remote_host, vss_time_list)) {
	buginf("\nVSS: %#z failed input time list.",
	       socket->remote_net, socket->remote_host);
	return(VSVC_NO_ERROR);
    }

    /*
     * Save the information
     */
    clock = system_uptime_seconds();
    sstrncpy(vss_time_epoch_name, query->node_name, VST_TOTALLEN);
    vss_time_epoch_seq = query->node_sequence;
    vss_time_epoch_age = 0 - clock;
    vss_time_base = query->current_time - clock;
    GET_TIMESTAMP(vss_time_set_when);
    vss_time_set_net = socket->remote_net;
    vss_time_set_host = socket->remote_host;
    vss_time_set_adjustment = 0;

    /*
     * Should this be exported to the system?
     */
    if (vss_time_to_system) {
	if (clock_set_unix(query->current_time, CLOCK_SOURCE_VINES))
	    clock_is_now_valid();
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	       SS - Time Sync (18)
 *
 **************************************************/

/*
 * vss_time_sync_send_call
 *
 * Broadcast a time synchronization message on all interfaces.  This
 * routine is called every thirty minutes from the server service
 * periodic routine.
 */
static void vss_time_sync_send_call (void)
{
    vss_time_sync_query *sync;
    vinesipcsocket *socket;
    ulong clock, now, age;
    char text[20];
    char timestring[UNIX_TIME_STRING_MINBUF];
    int i;

    if (!vss_time_base) {
	if (vss_debug)
	    buginf("\nVSS: Time sync not sent because time has not been set.");
	return;
    }

    sync = malloc(sizeof(vss_time_sync_query));
    if (!sync)
	return;
    clock = system_uptime_seconds();
    now = vss_time_base + clock;
    age = vss_time_epoch_age + clock;

    sync->current_time = now;
    sync->epoch_age = age;
    sstrncpy(sync->epoch_name, vss_time_epoch_name, VST_TOTALLEN);
    sync->epoch_sequence = vss_time_epoch_seq;

    /*
     * Broadcast the time sync?
     */
    vss_sprint_secs_in_dhms(text, age);
    if (vss_time_dest_count == 0) {
	if (vss_debug) {
	    unix_time_string(timestring, now);
	    buginf(vss_msg_time_to, "Time Sync", "message", VINES_ALLNETS,
		   VINES_ALLHOSTS, timestring, vss_time_epoch_name,
		   vss_time_epoch_seq, text);
	}
	vss_time_sync_send_call_(NULL, sync);
	return;
    }

    /*
     * Send it to a list of specific stations
     */
    if (vss_debug) {
	unix_time_string(timestring, now);
	buginf(vss_msg_time_to_many, timestring,
	       vss_time_epoch_name, vss_time_epoch_seq, text);
    }
    for (i = 0; i < vss_time_dest_count; i++) {
	socket = vipc_create_conn(vss_time_dest[i].net,
				  vss_time_dest[i].host, 0, 0, TRUE);
	if (!socket) {
	    buginf(vsvc_nomem);
	    return;
	}
	vss_time_sync_send_call_(NULL, sync);
	if (vss_debug)
	    buginf("\nVSS: Time Sync sent to %#z",
		       vss_time_dest[i].net, vss_time_dest[i].host);
    }
    free(sync);
}

/*
 * vss_time_sync_rcv_call
 *
 * Check the incoming time message to see if this is a new 
 *
 * VTS2, p62.  Check to see if this is a time sync message carrying time
 * based upon the last known established starting time epoch.  If so and
 * the received time is greater then the local time, then adjust the time
 * forward.  If less or equal to the current time, then ignore it.
 *
 * If the lastname and lastseq are NOT the same as the currently stored
 * values then this message represents time based upon a different starting
 * time epoch.  Given this case, the lastsettime values are compared.  If
 * the received lastsettime is less than the local lastsettime, then the
 * new time is accepted.
 */
ushort vss_time_sync_rcv_call (
    vinesipcsocket *socket,
    vss_time_sync_query *query,
    vss_time_sync_reply *reply)
{
    ulong proposed_time, age, now, clock;
    long local_age;
    char text[20];
    char timestring[UNIX_TIME_STRING_MINBUF];

    clock = system_uptime_seconds();
    now = vss_time_base + clock;
    local_age = vss_time_epoch_age + clock;

    proposed_time = query->current_time;
    age = query->epoch_age;

    if (vss_debug) {
	vss_sprint_secs_in_dhms(text, age);
	unix_time_string(timestring, proposed_time);
	buginf(vss_msg_time_from, "Time Sync", "message",
	       socket->remote_net, socket->remote_host,
	       timestring, query->epoch_name,
	       query->epoch_sequence, text);
    }

    if (!vss_time_participate)
	return(VSVC_NO_ERROR);

    /*
     * Check access list
     */
    if (!vines_short_access_check(socket->remote_net,
				  socket->remote_host, vss_time_list)) {
	buginf("\nVSS: Failed input time list.");
	return(VSVC_NO_ERROR);
    }

    /*
     * First time message?
     */
    if (!vss_time_base) {
	if (vss_debug)
	    buginf("\nVSS: Accepting first time epoch");
	sstrncpy(vss_time_epoch_name, query->epoch_name, VST_TOTALLEN);
	vss_time_epoch_seq = query->epoch_sequence;
	vss_time_epoch_age = age - clock;
	vss_time_base = query->current_time - clock;
	GET_TIMESTAMP(vss_time_set_when);
	vss_time_set_net = socket->remote_net;
	vss_time_set_host = socket->remote_host;
	vss_time_set_adjustment = 0;
    } else if ((strcmp(query->epoch_name, vss_time_epoch_name) == 0) &&
	(query->epoch_sequence == vss_time_epoch_seq)) {
	/*
	 * Same Epoch?
	 */
	if (proposed_time > now) {
	    if (vss_debug)
		buginf("\nVSS: Adjusting time forward %u seconds",
		       proposed_time - now);
	    vss_time_base = proposed_time - clock;
	    GET_TIMESTAMP(vss_time_set_when);
	    vss_time_set_net = socket->remote_net;
	    vss_time_set_host = socket->remote_host;
	    vss_time_set_adjustment = proposed_time - now;
	}
    } else if ((long)age < local_age) {
	/*
	 * Newer epoch?
	 */
	if (vss_debug)
	    buginf("\nVSS: Accepting new time epoch");
	sstrncpy(vss_time_epoch_name, query->epoch_name, VST_TOTALLEN);
	vss_time_epoch_seq = query->epoch_sequence;
	vss_time_epoch_age = age - clock;
	vss_time_base = proposed_time - clock;
	GET_TIMESTAMP(vss_time_set_when);
	vss_time_set_net = socket->remote_net;
	vss_time_set_host = socket->remote_host;
	vss_time_set_adjustment = 0;
    } else {
	/*
	 * Old or slow time source.  Ignore it.
	 */
	if (vss_debug) {
	    char age_text[20];

	    vss_sprint_secs_in_dhms(text, local_age);
	    vss_sprint_secs_in_dhms(age_text, age);
	    buginf("\nVSS: Time Sync call ignored due to slow time");
	    buginf("\nVSS: local epoch %s(%x) proposed epoch %s(%x)", text, local_age,
		   age_text, age);
	    buginf("\nVSS: clock(%x) vss_time_base(%x) proposed_time(%x)",
		   clock, vss_time_base, proposed_time);
	}
	return(VSVC_NO_ERROR);
    }

    /*
     * Should this be exported to the system?
     */
    if (vss_time_to_system) {
	if (clock_set_unix(proposed_time, CLOCK_SOURCE_VINES))
	    clock_is_now_valid();
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	     SS - Get Server Info V3.x (19)
 *
 **************************************************/

/*
 * vss_getsvrinfov3_rcv_call
 *
 * Get general information about this server.  General means name, key
 * number, and version.
 */
ushort vss_getsvrinfov3_rcv_call (
    vinesipcsocket *socket,
    vss_getsvrinfov3_query *query,
    vss_getsvrinfov3_reply *reply)
{
    vsvc_insert_hostname(reply->server_name);
    reply->protocol_revision = 33272;
    sstrncpy(reply->version_string, vsvc_ss_info.version_string, VSS_VERSION_LENGTH);
    reply->serial_number = vines_network;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	         SS - Read File (25)
 *
 **************************************************/

/*
 * vss_read_file_rcv_call
 *
 * Get information about the LAN cards attached to this server.
 */
ushort vss_read_file_rcv_call (
    vinesipcsocket *socket,
    vss_read_file_query *query,
    vss_read_file_reply *reply)
{
    ushort invalid;
    char *src, *dst;

    /*
     * Validate the user.  Bail if it fails.
     */
    invalid = vsvc_validate_nonce(&vsvc_ss_info, &query->for_whom);
    if (invalid)
	return(invalid);
    if (vss_debug)
	buginf("\nVSS:   Request for file %s.", query->filename);

    /*
     * Bail if any of the unknowns in non-zero.
     */
    if (query->unknown1 || query->unknown2) {
	if (vss_debug)
	    buginf("\nVSS:   Unknown values are %d and %d.",
		   query->unknown1, query->unknown2);
	return(VSS_ERR_READ_FAILED);
    }

    /*
     * Build the appropriate response for the file name requested.
     */
    if (strcasecmp(query->filename, "/MACHINE") == 0) {
	strcpy(reply->contents, "V386\n");
    } else if ((strcasecmp(query->filename, "/disk1/banyan/ss/patches/pathist") == 0) ||
	       (strcasecmp(query->filename, "/disk1/banyan/ss/patches/namehist") == 0)) {
	strcpy(reply->contents, version);
    } else {
	return(VSS_ERR_READ_FAILED);
    }

    /*
     * Strip any carriage returns.
     */
    src = reply->contents;
    dst = reply->contents;
    while (*src) {
	if (*src == '\r')
	    continue;
	*dst++ = *src++;
	}
    *dst = '\0';

    /*
     * Strip out anything at the front that has already been sent.
     */
    if (query->offset > 499)
	query->offset = 499;
    strcpy(reply->contents, &reply->contents[query->offset]);

    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	      SS - Get Lan Info (27)
 *
 **************************************************/

/*
 * vss_get_lan_info_rcv_call
 *
 * Get information about the LAN cards attached to this server.
 */
ushort vss_get_lan_info_rcv_call (
    vinesipcsocket *socket,
    vss_get_lan_info_query *query,
    vss_get_lan_info_reply *reply)
{
    vss_get_lan_info_entry *entry;
    vinesidbtype *idb;
    idbtype *swidb;
    int index = 0;

    FOR_ALL_VIDBS(idb) {
	swidb = idb->phys_idb;
	if (idb == vinesnullidb)
	    continue;
	if (!vines_interface_up(idb))
	    continue;
	if (!(idb_is_ieeelike(swidb)))
	    continue;
	if (idb->svc_slot == 0)
	    continue;

	entry = &reply->entry[index++];
	entry->slot = idb->svc_slot;
	entry->line = idb->svc_line;
	if (swidb->description)
	    vsvc_insert_description(entry->network_name, swidb->description);
	entry->interface_type = vsvc_c2b_iftype(idb);
	vsvc_insert_description(entry->interface_name, swidb->namestring);
	entry->macaddr_length = IEEEBYTES;
    }
    reply->entry_count = index;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	      SS - Get Mux Info (28)
 *
 **************************************************/

/*
 * vss_get_mux_info_rcv_call
 *
 * Get information about the WAN cards attached to this server.
 */
ushort vss_get_mux_info_rcv_call (
    vinesipcsocket *socket,
    vss_get_mux_info_query *query,
    vss_get_mux_info_reply *reply)
{
    vss_get_mux_info_entry *entry;
    vinesidbtype *idb;
    hwidbtype *hwidb;
    idbtype *swidb;
    int index = 0;

    FOR_ALL_VIDBS(idb) {
	swidb = idb->phys_idb;
	hwidb = swidb->hwptr;
	if (idb == vinesnullidb)
	    continue;
	if (!vines_interface_up(idb))
	    continue;
	if (!(hwidb->status & IDB_SERIAL))
	    continue;
	if ((idb->svc_slot == 0) || (idb->svc_line == 0))
	    continue;

	entry = &reply->entry[index++];
	entry->slot = idb->svc_slot;
	entry->line = idb->svc_line;
	vsvc_insert_description(entry->network_name, swidb->description);
	entry->interface_type = vsvc_c2b_iftype(idb);
	if (is_x25(hwidb))
	    vsvc_insert_description(entry->interface_name, "<X.25>");
	else if (is_frame_relay(hwidb))
	    vsvc_insert_description(entry->interface_name, "<FRAME>");
	else if (is_smds(hwidb))
	    vsvc_insert_description(entry->interface_name, "<SMDS>");
	else if (is_atm(hwidb))
	    vsvc_insert_description(entry->interface_name, "<ATM>");
	else
	    vsvc_insert_description(entry->interface_name, "<Serial>");
    }
    reply->entry_count = index;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	     SS - Installed Services (33)
 *
 **************************************************/

/*
 * vss_inst_svcs_rcv_call
 *
 * List all services installed on this server.  Make sure to completely
 * count the number of services on each pass.  Its only imporant the
 * first time, but its easiest to do it every time.
 */
ushort vss_inst_svcs_rcv_call (
    vinesipcsocket *socket,
    vss_inst_svcs_query *query,
    vss_inst_svcs_reply *reply)
{
    vss_inst_svcs_entry *entry;
    vsvc_info *service;
    int svc_count, entry_count;

    svc_count = entry_count = 0;
    FOR_ALL_SERVICES(service) {
	if (svc_count++ < query->pass)
	    continue;
	entry = &reply->entry[entry_count++];
	sstrncpy(entry->service_name, service->svc_name, VST_TOTALLEN);
	if (entry_count >= VSS_MAX_SVC_STATS)
	    break;
    }
    
    if (service != NULL)
	reply->pass = svc_count;
    reply->entry_count = entry_count;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	       SS - Get Server Info (39)
 *
 **************************************************/

/*
 * vss_getsvrinfo_rcv_call
 *
 * Get general information about this server.  General means name, key
 * number, and version.
 */
ushort vss_getsvrinfo_rcv_call (
    vinesipcsocket *socket,
    vss_getsvrinfo_query *query,
    vss_getsvrinfo_reply *reply)
{
    vsvc_insert_hostname(reply->server_name);
    reply->version_number = vsvc_ss_info.version;
    sstrncpy(reply->version_string, vsvc_ss_info.version_string, VSS_VERSION_LENGTH);
    reply->serial_number = vines_network;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *	      SS - Get Service Info (46)
 *
 **************************************************/

/*
 * vss_get_svc_summ_stats_rcv_call
 *
 * Get information about the services running on this server.  Make sure
 * to completely count the number of services on each pass.  Its only
 * imporant the first time, but its easiest to do it every time.
 */
ushort vss_get_svc_summ_stats_rcv_call (
    vinesipcsocket *socket,
    vss_get_svc_summ_stats_query *query,
    vss_get_svc_summ_stats_reply *reply)
{
    vss_get_svc_summ_stats_entry *entry;
    vsvc_info *service;
    int svc_count, entry_count;

    svc_count = entry_count = 0;
    FOR_ALL_SERVICES(service) {
	if (svc_count++ < query->pass)
	    continue;
	entry = &reply->entry[entry_count++];
	sstrncpy(entry->service_name, service->svc_name, VST_TOTALLEN);
	entry->msgsin = service->msgs_in;
	entry->msgsout = service->msgs_out;
	entry->locin = 0;
	entry->locout = 0;
	entry->cpuusg = 0;
	entry->size = 0;
	entry->sockinuse = 0;
	entry->sppconninuse = 0;
	entry->time_started = vss_time_base;
	if (entry_count >= VSS_MAX_SVC_STATS)
	    break;
    }
    
    if (service != NULL)
	reply->pass = svc_count;
    reply->entry_count = entry_count;
    return(VSVC_NO_ERROR);
}


/**************************************************
 *
 *            SS - Search / ReturnAddress
 *
 **************************************************/

/*
 * vss_rcv_search
 *
 * A NetRPC search message has been received on the Server Service port.
 */
void vss_rcv_search (
    vinesipcsocket *socket,
    ushort user_num,
    uchar *user_string,
    uchar *info1,
    uchar *info2,
    uchar *info3)
{
    vsvc_send_returnaddress(socket, &vsvc_ss_info);
}

/*
 * vss_rcv_returnaddress
 *
 * A NetRPC returnaddress message has been received on the Server Service
 * port.  
 */
void vss_rcv_returnaddress (
    vinesipcsocket *socket,
    ushort proto_rev,
    ulong  service_rev,
    vinesaddress *service_address)
{
    /* Do nothing */
}

/**************************************************
 *
 *	SS - Routines Common Across Services
 *
 **************************************************/

/*
 * vss_periodic
 *
 * Perform any periodic processing for the server service.  This routine
 * is called once every thirty seconds.
 */
void vss_periodic (void)
{
    ulong net;
    int i;

    if (vss_time_use_system)
	vss_update_from_local_time();

    if (!vss_time_participate)
	return;

    if (vss_time_base) {
	vss_time_sync_send_call();
    } else if (vss_time_dest_count) {
	for (i = 0; i < vss_time_dest_count; i++) {
	    vss_get_time_info_send_call(vss_time_dest[i].net,
					vss_time_dest[i].host);
	}
    } else {
	/* Pull in the timer in case we need to try again. */
	mgd_timer_start(&vsvc_ss_info.timer, VSS_PERIOD_INIT);
	net = vines_nearest_server_net();
	if (net != VINES_UNKNOWNNET) {
	    vss_get_time_info_send_call(net, VINES_SERVER);
	}
    }
}

/*
 * This function does exactly same thing that sprint_ticks_in_dhms()
 * in os/util.c does except it expects interval in seconds instead of ticks. 
 * If the time we want to convert is a big number and multiplied by ONESEC,
 * it will overflow and sprint_ticks_in_dhms() may give bad time. In order 
 * to avoid this overflow, receive the interval in seconds and call 
 * time_break1 directly.
 */
int vss_sprint_secs_in_dhms (char *text, ulong seconds)
{
    timetype t;
    timetype *time;

    if (text == NULL)
	return(0);

    time = &t;
    memset(time, 0, sizeof(timetype));
    if ((int) seconds >= 0)
	time->seconds = seconds;
    else
	time->seconds = 0;

    time->minutes += time->seconds/60; time->seconds %= 60;
    time->hours += time->minutes/60; time->minutes %= 60;
    time->days += time->hours/24; time->hours %= 24;
    time->weeks += time->days/7; time->days %= 7;

    if (time->weeks)
	return(sprintf(text, "%dw%dd", time->weeks, time->days));
    else if (time->days > 0)
	return(sprintf(text, "%dd%-02d", time->days, time->hours));
    else
	return(sprintf(text, "%-02d:%-02d:%-02d", time->hours, 
		    time->minutes, time->seconds));
}

/*
 * vss_command
 *
 * Configuration for the server service.
 */
void vss_command (parseinfo *csb)
{
    ulong net;
    ushort host;
    int i, j;

    if (csb->nvgen) {
	if (!vines_running) {
	    return;
	}
	switch (csb->which) {
	  case VINES_TIME_DEST:
	    for (i = 0; i < vss_time_dest_count; i++)
		nv_write(TRUE, "%s %z", csb->nv_command,
			 vss_time_dest[i].net, vss_time_dest[i].host);
	    break;
	  case VINES_TIME_LIST:
	    nv_write(vss_time_list, "%s %d", csb->nv_command, vss_time_list);
	    break;
	  case VINES_TIME_PARTICIPATE:
	    nv_write(!vss_time_participate, "no %s", csb->nv_command);
	    break;
	  case VINES_TIME_SET:
	    nv_write(vss_time_to_system, "%s", csb->nv_command);
	    break;
	  case VINES_TIME_USE:
	    nv_write(vss_time_use_system, "%s", csb->nv_command);
	    break;
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case VINES_TIME_LIST:
	vss_time_list = csb->sense ? GETOBJ(int,1) : 0;
	break;
      case VINES_TIME_DEST:
	if (csb->sense) {
	    if (vss_time_dest_count == VSS_TIME_MAX_DEST) {
		printf("\nToo many time destinations.");
		return;
	    }
	    i = vss_time_dest_count++;
	    vss_time_dest[i].net = GETOBJ(paddr,1)->vines_addr.net;
	    vss_time_dest[i].host = GETOBJ(paddr,1)->vines_addr.host;
	    net = GETOBJ(paddr,1)->vines_addr.net;
	    host = GETOBJ(paddr,1)->vines_addr.host;
	    for (i = 0; i < vss_time_dest_count; i++) {
		if ((net == vss_time_dest[i].net) &&
		    (host == vss_time_dest[i].host)) {
		    j = --vss_time_dest_count;
		    vss_time_dest[i].net = vss_time_dest[j].net;
		    vss_time_dest[i].host = vss_time_dest[j].host;
		    break;
		}
	    }
	}
	break;

      case VINES_TIME_PARTICIPATE:
	vss_time_participate = csb->set_to_default || csb->sense;
	break;

      case VINES_TIME_SET:
	vss_time_to_system = csb->sense;
	if (csb->sense) {
	    vss_time_use_system = FALSE;
	    if (vss_time_base) {
		if (clock_set_unix(vss_time_base + system_uptime_seconds(),
			      CLOCK_SOURCE_VINES)) 
		    clock_is_now_valid();
	    }
	}
	break;

      case VINES_TIME_USE:
	vss_time_use_system = csb->sense;
	if (csb->sense) {
	    vss_time_to_system = FALSE;
	    vss_update_from_local_time();
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * vss_init
 *
 * Boot time initializion of information that is maintained by the Server
 * Service.  This routine will only ever be called once by the router.
 */
void vss_init (void)
{
    int i;

    vsvc_install_svc(&vsvc_ss_info);

    /*
     * Initialize configuration variables
     */
    vss_time_list = 0;
    vss_time_to_system = FALSE;
    vss_time_use_system = FALSE;
    vss_time_participate = TRUE;
    for (i = 0; i < VSS_TIME_MAX_DEST; i++) {
	vss_time_dest[i].net = VINES_UNKNOWNNET;
	vss_time_dest[i].host = VINES_UNKNOWNHOST;
    }
}

/*
 * vss_start
 *
 * Run time initialize of information that is maintained by the Server
 * Service.  This routine will be called each time the 'vines routing'
 * command is issued.
 */
void vss_start (void)
{
    /*
     * Initialize private variables
     */
    vss_time_base = 0;
    vss_time_epoch_age = 0;
    vss_time_epoch_seq = 0;
    strcpy(vss_time_epoch_name, "none");
    vss_time_set_adjustment = 0;
    TIMER_STOP(vss_time_set_when);
    vss_time_set_net = 0L;
    vss_time_set_host = 0;
    vss_time_local_seq = 0;
}

/*
 * vss_stop
 *
 * Purge all information that is maintained by the Server Service.  This
 * routine will be called each time the 'no vines routing' command is
 * issued.
 */
void vss_stop (void)
{
}

/*
 * vss_display
 *
 * Display all information related to the Server Service.
 */
void vss_display (void)
{
    int i;
    char text[20];
    char timestring[UNIX_TIME_STRING_MINBUF];
    ulong clock = system_uptime_seconds();

    if (vss_time_base) {
	unix_time_string(timestring, vss_time_base + clock);
	printf("\n  Time is %s", timestring);
	sprint_dhms(text, vss_time_set_when);
	if (vss_time_set_adjustment) {
	    printf("\n  Time last adjusted (+%d sec) by %#z, %s ago.",
		   vss_time_set_adjustment, vss_time_set_net, vss_time_set_host,
		   text);
	} else {
	    printf("\n  Time last set by %#z, %s ago.",
		   vss_time_set_net, vss_time_set_host, text);
	}
	vss_sprint_secs_in_dhms(text, (vss_time_epoch_age + clock));
	printf("\n  Time epoch is %s-%u, started %s ago.",
	       vss_time_epoch_name, vss_time_epoch_seq, text);
    } else {
	printf("\n  VINES time is not set.");
    }

    printf("\n  Configuration:");
    if (vss_time_participate)
	printf("\n    Participating in vines time of day synchronization.");
    if (vss_time_dest_count) {
	printf("\n    Sending time messages to %d specific destinations.",
	       vss_time_dest_count);
	for (i = 0; i < vss_time_dest_count; i++)
	    printf("%s%#15z", (i % 4 == 0) ? "\n      " : " ",
		   vss_time_dest[i].net, vss_time_dest[i].host);
    } else {
	printf("\n    Sending time messages to the broadcast address.");
    }
    if (vss_time_list)
	printf("\n    Input time access list is %d", vss_time_list);
    if (vss_time_to_system)
	printf("\n    Synchronizing system time with vines time.");
    if (vss_time_use_system)
	printf("\n    Synchronizing vines time with system time.");
}
