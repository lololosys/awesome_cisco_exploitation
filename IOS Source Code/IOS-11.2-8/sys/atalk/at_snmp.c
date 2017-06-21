/* $Id: at_snmp.c,v 3.5.8.7 1996/08/03 23:19:12 fox Exp $
 * $Source: /release/112/cvs/Xsys/atalk/at_snmp.c,v $
 *------------------------------------------------------------------
 * AppleTalk SNMP Protocol support
 *
 * August 1990, David Edwards
 * September 1991, Renamed from AMP, David Edwards
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_snmp.c,v $
 * Revision 3.5.8.7  1996/08/03  23:19:12  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.5.8.6  1996/07/10  22:18:57  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.5.8.5  1996/06/07  16:24:18  kao
 * CSCdi59777:  no snmp-server crashes router
 * Need to reset proc id after process is killed
 * Branch: California_branch
 *
 * Revision 3.5.8.4  1996/04/24  14:12:25  jjohnson
 * CSCdi54272:  AT SNMP SCHED-3-THRASHING
 * Don't watch for managed events until after waiting for system init
 * Branch: California_branch
 *
 * Revision 3.5.8.3  1996/03/28  16:44:48  jjohnson
 * CSCdi50399:  SNMP can drive CPU utilization to over 90%
 * Make SNMP processes default to low priority, and add a hidden config
 * command to allow customers to change it
 * Branch: California_branch
 *
 * Revision 3.5.8.2  1996/03/23  01:27:38  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.5.8.1  1996/03/18  18:53:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.2.3  1996/03/16  06:27:02  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.2.2  1996/03/07  08:29:31  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.2.1  1996/02/20  00:17:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/28  09:32:12  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.4  1996/02/12  06:57:11  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * There isn't much demand for network management during a meltdown
 *
 * Revision 3.3  1996/01/20  02:35:09  jjohnson
 * CSCdi47220:  need snmp packet debugging
 *
 * Revision 3.2  1995/11/17  08:43:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:50:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/06/18  06:17:47  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.2  1995/06/09 12:58:45  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 20:09:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <interface.h>
#include "packet.h"
#include "config.h"
#include "../os/signal.h"
#include "atalk_private.h"
#include "macip_util.h"
#include "atalk_nbp.h"
#include "sys_registry.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../snmp/snmp_debug.h"

/*
 * Local Storage
 */
static int at_snmp_pid;
static watched_queue *atalksnmpQ;
static watched_boolean *at_snmp_priority_changed;
static process_priority_t at_snmp_priority;

static void
atalk_snmp_priority (process_priority_t priority)
{
    /*
     * a process can only change its own priority, so tell the
     * atalk snmp process to do it
     */
    at_snmp_priority = priority;
    if (at_snmp_priority_changed)
	process_set_boolean(at_snmp_priority_changed, TRUE);
  }

static void
atalk_snmp_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_queue(atalksnmpQ, DISABLE, RECURRING);
    process_watch_boolean(at_snmp_priority_changed, DISABLE, RECURRING);
    while ((pak = process_dequeue(atalksnmpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&atalksnmpQ);
    delete_watched_boolean(&at_snmp_priority_changed);
    at_snmp_pid = NO_PROCESS;
}

/*
 * atalk_snmp
 */
static process
atalk_snmp (void)
{
    paktype    *inpak;
    paktype    *outpak;
    uint	pktsize;
    ataddrtype	atsrc_addr;
    addrtype	source_addr;
    ulong	major, minor;
    int		pak_count;

    process_wait_on_system_init();

    process_set_crashblock(THIS_PROCESS, TRUE);

    /*
     * Set up this process' world.
     */
    signal_permanent(SIGEXIT, atalk_snmp_teardown);
    atalksnmpQ = create_watched_queue("AT SNMP packets", 0, 0);
    process_watch_queue(atalksnmpQ, ENABLE, RECURRING);
    at_snmp_priority_changed = create_watched_boolean("SNMP reprioritize", 0);
    process_watch_boolean(at_snmp_priority_changed, ENABLE, RECURRING);

    atalk_BindSocket(atalksnmpQ, "SNMP", ATALK_SOCK_SNMP, NULL);

    while (TRUE) {
	process_wait_for_event();
	process_get_wakeup(&major, &minor);
	if (major == BOOLEAN_EVENT) {
	    process_set_priority(at_snmp_priority);
	    process_set_boolean(at_snmp_priority_changed, FALSE);
	    continue;
	}
	pak_count = PAK_SWITCHCOUNT;
	while (TRUE) {
	    /*
	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }
	    inpak = process_dequeue(atalksnmpQ);
	    if (inpak == NULL) {
		break;
	    }
	    if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
		buginf("\nSNMP: Packet received via Appletalk from %a on %s",
		       atalk_pak_srcaddr(inpak),
		       idb_get_namestring(inpak->if_input));

	    /* dequeue then checksum and DDP type check */
	    if (!inpak ||
		!atalk_check(inpak, DDP_TYPE_SNMP, ATALK_UNKNOWN, FALSE))
		continue;

	    atalk_pak_printprocess(inpak, atalktrue_debug, "SNMP packet");

	    /* get a buffer for the reply */
	    pktsize = min(max(snmp_size, inpak->atalk_datalen),
			  DDP_MAX_DATABYTES);
	    outpak = atalk_pak_reply(inpak, atalk_pak_getpt(inpak), pktsize);
	    if (outpak == NULL) {
		/*
		 * we can't attach a buffer, so we can't process the message
		 * we should probably bump an error counter here, but chances
		 * are we're gonna die Real Soon Now(tm), so why bother?
		 */
	    } else {
		atsrc_addr = atalk_pak_srcaddr(inpak);
		source_addr.type = ADDR_APPLE;
		source_addr.length = ADDRLEN_APPLE;
		source_addr.apple_addr.net = atalk_netonly(atsrc_addr);
		source_addr.apple_addr.host = atalk_nodeonly(atsrc_addr);

		reg_invoke_snmp_engine(&source_addr,
				       inpak->transport_start,
				       inpak->atalk_datalen,
				       outpak->transport_start,
				       &pktsize);

		if (pktsize) {
		    /* we have a response, so send it */
		    if (reg_invoke_snmp_debug(DEBUG_SNMP_PACKETS))
			buginf("\nSNMP: Packet sent via Appletalk to %a",
			       atalk_pak_dstaddr(outpak));

		    atalk_pak_setlength(outpak, pktsize);
		    atalk_DDPcksum(outpak);
		    atalk_send(outpak);
		} else {
		    /* no response, so deallocate the response buffer */
		    retbuffer(outpak);
		}
	    }
	    datagram_done(inpak);
            (void) process_may_suspend();
	}
    }
}

/*
 * atalk_snmp_process
 * Create or delete a process to handle SNMP for Appletalk
 * Invoked as SERVICE_SNMP_PROCESS
 */

static void atalk_snmp_process (boolean createflag)
{
    if (!createflag) {
	if (at_snmp_pid != NO_PROCESS) {
	    process_kill(at_snmp_pid);
	    at_snmp_pid = NO_PROCESS;
	}
    } else {
	if (at_snmp_pid == NO_PROCESS && RUNNING(LINK_APPLETALK)) {
	    at_snmp_pid = process_create(atalk_snmp, "AT SNMP", NORMAL_STACK,
					 at_snmp_priority);
	}
    }
}

/*
 * at_snmp_register
 * Register SNMP in each unique primary zone we are connected.
 */

void atalk_snmp_register (void)
{
    atalkidbtype	*idb;
    ziptabletype *ztab;
    nbpEntity	nve;
    str32	ifzone;
    ataddress	src;

    if (!snmp_serving)
	return;
    FOR_ALL_ATIDBS(idb) {
	if (!idb->atalk_enabled || !atalkif_operational(idb))
	    continue;
	ztab = atalkif_primary_zone(idb);
	str32ninit(&ifzone, (char *)ztab->name, ztab->length);
	atnbp_mkname(&nve, hostname, "SNMP Agent", STR32PTR(&ifzone));
	if (atnbp_isregistered(&nve) == NBP_OK)
	    continue;
	src.net  = idb->atalknet;
	src.node = idb->atalknode;
	src.spec = ATALK_SOCK_SNMP;
	if ((atnbp_register(&nve, &src, TRUE) == NBP_REG_PENDING)
	    && at_debug(atalk_events, ATALK_NULL_MASK))
	    errmsg(AT_ERR_REGPENDING, atnbp_entityStr(&nve), ddpAddrStr(&src));
    }
}


/*
 * at_access_check
 */

static boolean at_access_check (int access_list, addrtype *address)
{
    return(TRUE);
}

void atsnmp_Initialize (void)
{
    at_snmp_pid = NO_PROCESS;
    at_snmp_priority = SNMP_PRIORITY_DEF;
    atalksnmpQ = NULL;
    at_snmp_priority_changed = NULL;
    reg_add_snmp_access_check(ADDR_APPLE, at_access_check, "at_access_check");
    reg_add_snmp_process(atalk_snmp_process, "atalk_snmp_process");
    reg_add_snmp_priority(atalk_snmp_priority, "atalk_snmp_priority");
}
      
