/* $Id: sd.c,v 3.6.22.8 1996/08/02 17:37:44 dino Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/sd.c,v $
 *------------------------------------------------------------------
 * sd.c - Session Directory 
 *
 * February, 1995.
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file implement SDPv2.  SDPv1 is obsolete.  You'll see references to 
 * sdr which is a well-known unix tool that implements SDPv2.
 *
 *------------------------------------------------------------------
 * $Log: sd.c,v $
 * Revision 3.6.22.8  1996/08/02  17:37:44  dino
 * CSCdi64689:  SDP Process leaks memory again. Fix code review comment.
 * Branch: California_branch
 *
 * Revision 3.6.22.7  1996/08/01  21:07:50  dino
 * CSCdi64689:  SDP Process leaks memory again.
 * Branch: California_branch
 *
 * Revision 3.6.22.6  1996/06/20  17:59:28  dino
 * CSCdi60555:  Memory leak in sdr
 * Branch: California_branch
 *
 * Revision 3.6.22.5  1996/05/30  08:33:46  dino
 * CSCdi59053:  "show ip mroute active" doesn't display session for media
 *              based groups
 * Branch: California_branch
 *
 * Revision 3.6.22.4  1996/05/21  09:52:22  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.6.22.3  1996/05/06  16:07:23  dino
 * CSCdi56516:  The sd proecss doesnt process sdr annoucements
 * Branch: California_branch
 *
 * Revision 3.6.22.2  1996/04/06  00:06:05  mleelani
 * CSCdi53650:  igmp_add_group() should send unsol Report for
 * IGMP_USER_FLAG calls
 * Branch: California_branch
 * Make user processes send unsolicited reports consistently
 *
 * Revision 3.6.22.1  1996/03/18  20:19:08  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.5.2.2  1996/03/07  09:48:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.5.2.1  1996/02/20  01:01:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/01/24  20:16:03  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.5  1996/01/19  01:22:17  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.4  1995/12/17  18:30:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.3  1995/11/30  04:42:17  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.2  1995/11/17  17:30:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:01:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/09/26  01:05:29  dino
 * CSCdi41034:  SD Process goes away when ip sd listen
 *
 * Revision 2.10  1995/08/03  07:42:15  hampton
 * EIGRP stops processing input.  Convert the IP socket input queue into a
 * watched_queue.  This allows the scheduler to see if there is input
 * waiting when the process dismisses.  [CSCdi38156]
 *
 * Revision 2.9  1995/07/27 07:04:08  dino
 * CSCdi37750:  Detailed sd display has junk in Description field.
 *
 * Revision 2.8  1995/07/24  07:33:53  hampton
 * Transition IP Multicast support to use the passive timer macros for all
 * its timers.  It no longer references the system clock directly.
 * [CSCdi37539]
 *
 * Revision 2.7  1995/07/18 22:02:50  dino
 * CSCdi37275:  ustrcmp() says <abc> is greater than <abcde>
 *
 * Revision 2.6  1995/07/18  10:20:58  dino
 * CSCdi37233:  Local membership for sd not established when connected
 * member exists
 *
 * Revision 2.5  1995/07/17  08:29:00  dino
 * CSCdi37181:  Need to show active multicast sources - show ip mroute
 * active.
 *
 * Revision 2.4  1995/07/12  21:55:06  dino
 * CSCdi36995:  Timestamp cached sd advertisments.
 *
 * Revision 2.3  1995/06/16  04:28:20  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/09  13:05:51  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:04:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <master.h>
#include "../ui/common_strings.h"
#include <ciscolib.h>
#include <stdlib.h>
#include <config.h>
#include "interface_generic.h"
#include "../os/clock.h"
#include "../os/signal.h"
#include "../parser/parser_actions.h"
#include "../ip/ip.h"
#include "ipmulticast_debug.h"
#include "sd.h"
#include "igmp.h"
#include "parser_defs_igmp.h"

static pid_t sd_pid;
static int sdr_running;
static queuetype sdr_cache;
static ipsocktype *sdr_socket;
static sys_timestamp cache_ager_timer;
static longlong sd_cache_timeout;
char const * const sd_media_strings[SD_MEDIA_MAX] = { "video", "whiteboard" };

static uchar *sd_strcpy(uchar *, uchar **, int);
static ipaddrtype sd_get_ipaddr(uchar *, int *);
static void sd_clear_cache(queuetype *);
static void sd_parse_media_string(sdtype *, int);
static void sd_free_cache(sdtype *);
static void sd_process_input(paktype *, udptype *, sdhdrtype *);
static void sd_show_session(sdtype *);
static void sd_cache_ager(void);

static void sdp_process_teardown (int signal, int dummy1, void *dummy2,
				  char *dummy3)
{
    process_watch_timer(&cache_ager_timer, DISABLE);
    sd_clear_cache(&sdr_cache);
}


/*
 * sdp_process
 *
 * Process sdr received session announcements.
 */
static process sdp_process (void)
{

    paktype   *pak;
    udptype   *udp;
    sdhdrtype *sd;
    
    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, sdp_process_teardown);
    if (sdr_socket)
	process_watch_queue(sdr_socket->inq, ENABLE, RECURRING);
    else
	process_kill(THIS_PROCESS);
    process_watch_timer(&cache_ager_timer, ENABLE);
    if (sd_cache_timeout) {
	TIMER_START(cache_ager_timer, ONEMIN);
    } else {
	TIMER_STOP(cache_ager_timer);
    }
    
    /*
     * Loop looking for input.
     */
    while (TRUE) {
	ulong major, minor;
	
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	    case QUEUE_EVENT:
		while ((pak = read_ipsocket(sdr_socket, 0, 0, &udp, &sd))) {
		    sd_process_input(pak, udp, sd);
		    datagram_done(pak);
		}
		break;
	    case TIMER_EVENT:
		TIMER_STOP(cache_ager_timer);
		sd_cache_ager();
		if (sd_cache_timeout)
		    TIMER_START(cache_ager_timer, ONEMIN);
		break;
	    }
	}
    }
}

/*
 * sd_process_input
 *
 * Process input packet received.
 */
void sd_process_input (paktype *pak, udptype *udp, sdhdrtype *sd)
{

    uchar      *pkt, *start;
    sdtype     *cache, *cache2, *prev;
    queuetype  *head;
    ushort     *ttl_ptr;
    ipaddrtype *group_ptr;
    boolean    truncate, media_seen;
    int        length, media_count, attr_count, offset, diff;

    length = udp->length - UDPHEADERBYTES - sizeof(sdhdrtype);
    pkt = sd->data;
    start = pkt;

    SD_BUGINF("\nSDR: Announcement from %i on %s, %d bytes", 
	      sd->source, idb_get_namestring(pak->if_input), length);

    cache = malloc(sizeof(sdtype));
    if (!cache) return;

    cache->source = sd->source;
    media_count = 0;
    media_seen = FALSE;
    attr_count = 0;
    while (length > 0) {
	if (*(pkt+1) != '=') {
	    SD_BUGINF("\nSD: Bad format, can't find =");
	    break;
	}
	switch (*pkt) {
	case 's':

	    /*
	     * Session name.
	     */
	    pkt = sd_strcpy(pkt, cache->name ? NULL : &cache->name, length);
	    break;

	case 'i':
	    
	    /*
	     * Description.
	     */
	    pkt = sd_strcpy(pkt, cache->descrip ? NULL : &cache->descrip,
			    length);
	    break;

	case 'o':
	    
	    /*
	     * Creator.
	     */
	    pkt = sd_strcpy(pkt, cache->person ? NULL : &cache->person,
			    length);
	    break;

	case 'c':
	    pkt = sd_strcpy(pkt, NULL, length);

	    /*
	     * Group, ttl, and lifetime for sd.
	     */
	    offset = 0;

	    /*
	     * sdr format is: "c=IN IP4 <group>/<ttl>/<num addresses>".
	     * Can have a "c=" per media. <num addresses> is optional.
	     */
	    if (media_seen) {
		if (media_count >= SD_MAX_MEDIA) break;
		group_ptr = &cache->media[media_count].group;
		ttl_ptr = &cache->media[media_count].ttl;
	    } else {
		group_ptr = &cache->group;
		ttl_ptr = &cache->ttl;
	    }

	    /*
	     * Get group address.
	     */
	    if (strncmp(start+2, SD_IN_IP4, 7)) {
		SD_BUGINF("\nSDR: Non-supported connection data");
		break;
	    }
	    *group_ptr = sd_get_ipaddr(start+9, &offset);
	    if (strncmp(start+9+offset, "/", 1)) {
		SD_BUGINF("\n    Expecting /, then ttl");
		break;
	    }
	    
	    /*
	     * Get ttl.
	     */
	    offset++;
	    match_decimal(start+9, 0, 255, &offset, ttl_ptr);

	    /*
	     * Get <num addresses> only when not a media specific "c=".
	     */
	    if (!media_count) {
		if (*(start+9+offset)) {
		    if (strncmp(start+9+offset, "/", 1)) {
			SD_BUGINF("\n    Expecting /, then <num addresses>");
			break;
		    }
		    offset++;
		    match_decimal(start+9, 1, 255, &offset, 
				  &cache->num_groups);
		} else {
		    cache->num_groups = 1;
		}
	    }
	    break;

	case 'm':
	    if (media_seen) media_count++;
	    media_seen = TRUE;
	    if (media_count >= SD_MAX_MEDIA) {
		pkt = sd_strcpy(pkt, NULL, length);
		break;
	    }
	    
	    /*
	     * Media.
	     */
	    attr_count = 0;
	    pkt = sd_strcpy(pkt, &cache->media[media_count].media_string,
			    length);
	    sd_parse_media_string(cache, media_count);
	    break;

	case 'a':

	    /*
	     * Media format, if any.
	     */
	    if (media_count >= SD_MAX_MEDIA || attr_count >= SD_MAX_ATTR || 
		!media_seen) {
		pkt = sd_strcpy(pkt, NULL, length);
		break;
	    }
	    pkt = sd_strcpy(pkt, &cache->media[media_count].format[attr_count],
			    length);
	    attr_count++;
	    break;

	case 'v':

	    /*
	     * SDP version number. This starts sdr specific strings case
	     * statements. If found in sd packets, fall through to default 
	     * case and complain.
	     */
	    pkt = sd_strcpy(pkt, NULL, length);
	    break;

	case 'u':
	    
	    /*
	     * URL.
	     */
	    pkt = sd_strcpy(pkt, cache->url ? NULL : &cache->url, length);
	    break;

	case 'e':
	    
	    /*
	     * Email address.
	     */
	    pkt = sd_strcpy(pkt, cache->email ? NULL : &cache->email,
			    length);
	    break;

	case 'p':
	    
	    /*
	     * Phone number.
	     */
	    pkt = sd_strcpy(pkt, cache->phone ? NULL : &cache->phone,
			    length);
	    break;
	    
	case 'b':
	    
	    /*
	     * Bandwidth. "b=<modifier>:<bandwidth-value>".
	     */
	    pkt = sd_strcpy(pkt, NULL, length);
	    break;

	case 't':
	    
	    /*
	     * Times.
	     */
	    pkt = sd_strcpy(pkt, NULL, length);
	    offset = 0;
	    match_ulong_decimal(start+2, 0, -1, &offset, 
				&cache->lifetime_start);
	    offset++;
	    match_ulong_decimal(start+2, 0, -1, &offset, 
				&cache->lifetime_stop);
	    break;

	case 'r':
	    
	    /*
	     * Repeat Times.
	     */

	case 'z':
	    
	    /*
	     * Time Zones.
	     */

	case 'k':
	    
	    /*
	     * Encryption Keys.
	     */

	default:
	    pkt = sd_strcpy(pkt, NULL, length);
	    if (strlen(start) > MEDDATA) *(start+MEDDATA-1) = 0;
	    SD_BUGINF("\n    Unknown option: %c, data: %s", *start, start+2);
	    break;
	}
	if (!pkt) break;

	/*
	 * If record is too large for buginf buffer, truncate it and tell
	 * the user.
	 */
	if (strlen(start) > MEDDATA) {
	    truncate = TRUE;
	    *(start + MEDDATA-1) = 0;
	} else {
	    truncate = FALSE;
	}
	SD_BUGINF("\n    %s", start);
	if (truncate) SD_BUGINF(" (truncated)");

	length -= (pkt - start);
	start = pkt;
    }

    /*
     * For existing entry, if found, replace it. Otherwise, insert in
     * alphabetic order.
     */
    head = &sdr_cache;

    prev = NULL;
    diff = 1;
    for (cache2 = head->qhead; cache2; cache2 = cache2->next) {
	diff = strcasecmp(cache2->name, cache->name);
	if (diff >= 0) break;
	prev = cache2;
    }

    /*
     * Link in.
     */
    insqueue(head, cache, prev);
    if (diff) {
	GET_TIMESTAMP(cache->uptime);
    } else {
	/* Remove existing entry */
	remqueue(head, cache2, cache);
	COPY_TIMESTAMP(cache2->uptime, cache->uptime);
	sd_free_cache(cache2);
    }
    
    /*
     * Timestamp last heard.
     */
    GET_TIMESTAMP(cache->last_heard);
}

/*
 * sd_strcpy
 *
 * Assumes following format of variable from:
 *     "x=abc abc" which is terminated with a newline.
 *
 * Copies to variable to in null-terminated string format.
 */
static uchar *sd_strcpy (uchar *from, uchar **to, int pkt_len)
{

    uchar *start;
    ulong length;

    start = from + 2;
    while (pkt_len > 0) {
	if (*from == 0x0a) break;
	from++;
	pkt_len--;
    }

    if (!pkt_len) {
	SD_BUGINF("\nSD: Couldn't find NL in packet");
	return(NULL);
    }

    *from++ = 0;
    length = from - start;

    if (to) {
	*to = malloc(length+1);
	if (*to) strcpy(*to, start);
    }
    return(from);
}

static void sd_parse_media_string (sdtype *sd, int media_count)
{
    char *media_type, *port_ptr;
    int i;
    
    /* Set pointers to first and second sub-field */
    media_type = sd->media[media_count].media_string;
    port_ptr = findarg(media_type);
    
    /* Identify media type */
    for (i = 0; i < SD_MEDIA_MAX; i++) {
	if (strstr(media_type, sd_media_strings[i]) == media_type) {
	    sd->media[media_count].media_type = i;
	    break;
	}
    }
    if (i == SD_MEDIA_MAX) {
	sd->media[media_count].media_type = SD_MEDIA_UNDEFINED;
	return;
    }

    /* Get transport port */
    sd->media[media_count].port = strtoul(port_ptr, NULL, 10);
}

/*
 * sd_free_cache
 */
static void sd_free_cache (sdtype *cache)
{

    int i,j;

    free(cache->name);
    free(cache->person);
    free(cache->descrip);
    for (i = 0; i < SD_MAX_MEDIA; i++) {
	if (cache->media[i].media_string) free(cache->media[i].media_string);
	for (j = 0; j < SD_MAX_ATTR; j++) {
	    if (cache->media[i].format[j]) free(cache->media[i].format[j]);
	}
    }

    /*
     * Free some sdr specific fields.
     */
    if (cache->url) free(cache->url);
    if (cache->email) free(cache->email);
    if (cache->phone) free(cache->phone);

    cache->next = NULL;
    free(cache);
}

/*
 * sd_is_group_in_entry
 *
 * Check if group address is in this cache entry.
 */
static boolean sd_is_group_in_entry (const sdtype *sd, ipaddrtype group)
{
    int i;

    if (group == sd->group)
	return (TRUE);

    for (i = 0; i < SD_MAX_MEDIA; i++) {
	if (!sd->media[i].media_string)
	    return (FALSE);
	if (group == sd->media[i].group)
	    return (TRUE);
    }

    return (FALSE);
}

/*
 * sd_parse_listen_cmd
 *
 * Enable/Disable reception of sd multicast packets on interface.
 */
static void sd_parse_listen_cmd (parseinfo *csb)
{
    igmp_cachetype *igmp;
    idbtype        *idb;
    ipsocktype     **socket;
    ushort         port;
    addrtype       laddr;
    ipaddrtype     group;

    idb = csb->interface;

    group = SDRSYSTEMS;
    port = SDR_PORT;
    socket = &sdr_socket;

    igmp = igmp_find_group(group, idb);
    if (igmp && !(igmp->flags & (IGMP_LOCAL_FLAG | IGMP_USER_FLAG))) {
	igmp = NULL;
    }
	
    if (csb->nvgen) {
	if (igmp) nv_write(TRUE, "%s", csb->nv_command);
	return;
    }

    /*
     * Enable sd/sdr on an interface. If already enabled, just return.
     * Otherwise, decide if process should be started and UDP socket
     * should be opened.
     */
    if (csb->sense) {
	if (igmp) return;
	igmp_enable_idb(idb);
	igmp_add_group(group, idb, 0, IGMP_LOCAL_FLAG | IGMP_USER_FLAG, 0);
	
	if (!*socket) {
	    
	    /*
	     * Open sd UDP listening socket.
	     */
	    laddr.type = ADDR_IP;
	    laddr.ip_addr = group;
	    *socket = open_ipsocket(UDP_PROT, &laddr, NULL, port, 0, 
				    SOCK_ANYSRC);
	    if (!*socket) {
		SD_BUGINF("\nSD: Can't open sd UDP socket");
		return;
	    }
	}
	/*
	 * Start process, if not started.
	 */
	if (!sdr_running) {
	    sd_pid = process_create(sdp_process, "SDP Process", NORMAL_STACK, 
				    PRIO_NORMAL);
	}
	
	sdr_running++;
    } else {
	if (!igmp) return;
	
	/*
	 * Disable sdr on an interface.
	 */
	igmp_send_leave(group, idb);
	igmp_delete_group(group, idb);
	sdr_running--;
	
	/*
	 * If no more sdr's on any interface, close sdr socket.
	 */
	if (!sdr_running) {
	    close_ipsocket(sdr_socket);
	    sdr_socket = NULL;
	    process_kill(sd_pid);
	}
    }
}

/*
 * sd_commands
 *
 * Parse configuration commands
 */
void sd_commands (parseinfo *csb)
{
    switch (csb->which) {
    case SDR_LISTEN_COMMAND:
	sd_parse_listen_cmd(csb);
	break;

    case SDR_CACHETIMEOUT_COMMAND:
	if (csb->nvgen) {
	    /* Must cast sd_cache_timeout to int because varargs can't
	     * handle longlongs
	     */
	    nv_write(sd_cache_timeout, "%s %d", csb->nv_command,
		     (int)(sd_cache_timeout/ONEMIN));
	    return;
	}
	if (csb->sense) {
	    sd_cache_timeout = (longlong) GETOBJ(int, 1) * ONEMIN;
	    TIMER_START(cache_ager_timer, ONEMIN);
	} else
	    sd_cache_timeout = 0;
	break;
    }
}

/*
 * sd_show_command
 *
 * Process command:
 *
 * show ip sdr [<group-address> | "<session-name>" | detail]
 */
void sd_show_command (parseinfo *csb)
{

    sdtype     *cache, *next;
    uchar      *name;
    queuetype  *head;
    ipaddrtype group;
    boolean    detail;

    /*
     * Get input command options.
     */
    group = GETOBJ(paddr,1)->ip_addr;
    name = GETOBJ(string,1);
    detail = GETOBJ(int,1);
    if (group || *name) detail = TRUE;

    /*
     * Setup correct cache pointer and determine if respective protocol is
     * running.
     */
    if (!sdr_running) {
	printf("\nSDR not enabled");
	return;
    }
    head = &sdr_cache;

    automore_enable(NULL);
    printf("SDR Cache - %d entries", head->count);

    for (cache = (sdtype *) head->qhead; cache; cache = next) {
	next = cache->next;
	if (group && !sd_is_group_in_entry(cache, group))
	    continue;
	if (*name && strcasecmp(name, cache->name)) continue;
	
	/*
	 * Display entry.
	 */
	mem_lock(cache);
	if (detail) {
	    sd_show_session(cache);
	} else {
	    printf("\n  %s", cache->name);
	}
	if (group || *name) {
	    free(cache);
	    break;
	}
	if (detail && cache->next) {
	    automore_conditional(0);
	    printf("\n");
	}
	next = cache->next;
	free(cache);
    }
    automore_disable();
}

/*
 * sd_show_session
 *
 * Display one sd cache entry.
 */
static void sd_show_session (sdtype *cache)
{

    uchar *ptr;
    int   i, j;
    clock_epoch epoch;
    
    printf("\nSession Name: %s", cache->name);
    printf("\n  Description: ");
    ptr = cache->descrip;
    while (*ptr) {
	for (i = 0; (i < 62) && *ptr; i++) printf("%c", *ptr++);
	if (*ptr) printf("\n%15s", " ");
    }
    printf("\n  Group: %i, ttl: %d", cache->group, cache->ttl);
    printf(", Contiguous allocation: %d", cache->num_groups);

    if (cache->lifetime_start) {
	epoch.epoch_secs = cache->lifetime_start;
	printf("\n  Lifetime: from %CC", "%T %tZ %E", &epoch);
	epoch.epoch_secs = cache->lifetime_stop;
	printf(" until %CC", "%T %tZ %E", &epoch);
    }
    printf("\n  Uptime: %TE, Last Heard: %TE", cache->uptime,
	   cache->last_heard);
    printf("\n  Announcement source: %i", cache->source);
    printf("\n  Created by: %s", cache->person);
    printf("\n  Phone number: %s", cache->phone);
    printf("\n  Email: %s", cache->email);
    printf("\n  URL: %s", cache->url);
    for (i = 0; i < SD_MAX_MEDIA; i++) {
	if (!cache->media[i].media_string) continue;
	printf("\n  Media: %s", cache->media[i].media_string);
	if (cache->media[i].group) {
	    printf("\n    Media group: %i, ttl: %d", 
		   cache->media[i].group, cache->media[i].ttl);
	}
	for (j = 0; j < SD_MAX_ATTR; j++) {
	    if (!cache->media[i].format[j]) break;
	    printf("\n    Attribute: %s", cache->media[i].format[j]);
	}
    }
}

/*
 * sd_clear_command
 *
 * Process command:
 *
 * clear ip sdr [<group-address> | "<session-name>"]
 */
void sd_clear_command (parseinfo *csb)
{
    sdtype     *cache, *prev, *next;
    uchar      *name;
    queuetype  *head;
    ipaddrtype group;

    group = GETOBJ(paddr,1)->ip_addr;
    name = GETOBJ(string,1);

    head = &sdr_cache;
    prev = NULL;
    
    for (cache = head->qhead; cache; cache = next) {
	next = cache->next;
	if ((*name && strcasecmp(name, cache->name)) ||
	    (group && !sd_is_group_in_entry(cache, group))) {
	    prev = cache;
	    continue;
	}

	/*
	 * Relink and free memory.
	 */
	remqueue(head, cache, prev);
	sd_free_cache(cache);
    }
}

/*
 * sd_clear_cache
 */
static void sd_clear_cache (queuetype *head)
{

    sdtype *cache, *next;

    /* This for loop is faster than calling dequeue several times */
    for (cache = head->qhead; cache; cache = next) {
	next = cache->next;
	sd_free_cache(cache);
    }
    queue_init(head, 0);
}

/*
 * sd_get_ipaddr
 */
static ipaddrtype sd_get_ipaddr (uchar *buf, int *offset)
{

    ushort     num[4];
    ipaddrtype addr;
    
    addr = 0;
    if (match_decimal(buf, 0, 255, offset, &num[0]) &&
        match_char(buf, offset, '.') &&
        match_decimal(buf, 0, 255, offset, &num[1]) &&
        match_char(buf, offset, '.') &&
        match_decimal(buf, 0, 255, offset, &num[2]) &&
        match_char(buf, offset, '.') &&
        match_decimal(buf, 0, 255, offset, &num[3])) {
        addr = ((((ulong) num[0]) << 24) |
		(((ulong) num[1]) << 16) |
		(((ulong) num[2]) << 8)  |
		((ulong) num[3]));
    }
    return(addr);
}

static void sd_cache_ager (void)
{
    sdtype *cache, *prev, *next;
    
    prev = NULL;
    for (cache = sdr_cache.qhead; cache; cache = next) {
	next = cache->next;
	if (ELAPSED_TIME64(cache->last_heard) < sd_cache_timeout) {
	    prev = cache;
	    continue;
	}
	remqueue(&sdr_cache, cache, prev);
	sd_free_cache(cache);
    }
}

/*
 * sd_get_session
 *
 * Get session for group address.  Check the media specific groups too.
 */
sdtype *sd_get_session (ipaddrtype group)
{

    sdtype *cache;

    for (cache = (sdtype *) sdr_cache.qhead; cache; cache = cache->next) {
	if (sd_is_group_in_entry(cache, group))
	    return (cache);
    }
    return(NULL);
}

boolean sd_get_media_port (ipaddrtype group, ushort media_type,
			   ulong *port_ptr)
{
    sdtype *sd;
    int i;

    sd = sd_get_session(group);

    if (!sd)
	return (FALSE);
    
    for (i = 0; i < SD_MAX_MEDIA; i++) {
	if (!sd->media[i].media_string)
	    break;
	if (sd->media[i].media_type == media_type) {
	    *port_ptr = (ulong) sd->media[i].port;
	    return (TRUE);
	}
    }
    return (FALSE);
}
