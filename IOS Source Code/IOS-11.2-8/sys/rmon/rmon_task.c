/* $Id: rmon_task.c,v 3.14.8.6 1996/08/28 13:07:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/rmon/rmon_task.c,v $
 *------------------------------------------------------------------
 * rmon_task.c:  Remote Monitoring MIB Packet Processing Task
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_task.c,v $
 * Revision 3.14.8.6  1996/08/28  13:07:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.14.8.5  1996/07/04  19:26:53  sdurham
 * CSCdi62114:  Process Thrashing on RMON Reprioritize
 * Branch: California_branch
 * clear watched boolean after processing
 *
 * Revision 3.14.8.4  1996/06/27  16:36:35  hampton
 * Keep your mitts off the scheduler private data structures.  [CSCdi61518]
 * Branch: California_branch
 *
 * Revision 3.14.8.3  1996/06/05  21:56:24  sdurham
 * CSCdi59292:  RMON process continously thrashing its own queue
 * Branch: California_branch - handle all packets when process called
 *
 * Revision 3.14.8.2  1996/05/24  20:43:41  sdurham
 * CSCdi55847:  IOS RMON agent only monitors incoming traffic
 * Branch: California_branch
 *          add service point for passing xmit packets to rmon
 *
 * Revision 3.14.8.1  1996/03/18  21:45:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.2.3  1996/03/16  07:20:22  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.7.2.2  1996/03/13  01:52:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.7.2.1  1996/02/20  17:01:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.14  1996/02/28  09:33:34  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.13  1996/02/19  06:05:55  jjohnson
 * CSCdi48853:  rmon can consume all available memory
 * As an interim measure, don't allow rmon to consume memory once the
 * heap's low-water mark has been reached.  While we're at it, name
 * all the malloc'ed memory so that we can tell what is used where
 *
 * Revision 3.12  1996/02/12  06:58:05  jjohnson
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 * There isn't much demand for network management during a meltdown
 *
 * Revision 3.11  1996/02/10  01:21:43  jjohnson
 * CSCdi48455:  RMON captured packet timestamp are not accurate
 * packet inputtime is not valid when rmon sees the packet.  make it so.
 *
 * Revision 3.10  1996/02/10  00:01:58  jjohnson
 * CSCdi48508:  rmon failure in low memory condition
 * Avoid malloc failure by creating ifIndex OID on the stack
 *
 * Revision 3.9  1996/02/09  07:57:21  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.8  1996/01/25  06:05:50  jjohnson
 * CSCdi47480:  bad config nvgen for rmon priority command
 *
 * Revision 3.7  1996/01/18  23:23:00  jjohnson
 * CSCdi45754:  snmp subsystem should not contain non-snmp mibs
 * the snmp timer task is now part of the snmp subsystem
 *
 * Revision 3.6  1996/01/18  01:14:18  jjohnson
 * CSCdi47080:  Enable limited RMON packet capture
 *
 * Revision 3.5  1996/01/02  23:27:42  jjohnson
 * CSCdi46285:  Need to clean up the RMON user interface
 *
 * Revision 3.4  1995/12/20  23:46:01  gstovall
 * CSCdi45909:  Item->handled field tested while uninitialized
 * Make sure that item->handled is initialized.  Bad programmer, no pizza.
 *
 * Revision 3.3  1995/11/17  18:39:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:22:16  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:03:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "fastswitch_registry.h"
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/ether_inline.h"
#include "config.h"
#include "parser_defs_rmon.h"
#include "rmon_task.h"
#include "sr_rmonmib.h"
#include "rmon_sanitize.h"
#include "rmon_registry.h"
#include "../snmp/sr_ifmib.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include "../util/itemlist.h"

watched_queue *RMONpacketQ;
watched_boolean *rmon_priority_change;

int rmon_input_packets;
int rmon_filtered_packets;
int rmon_passed_packets;
int rmon_dropped_packets;
int rmon_enqueued_packets;
int rmon_dequeued_packets;
int rmon_queue_highwater;

static pid_t rmon_pid;
static process_priority_t current_rmon_priority;
static process_priority_t desired_rmon_priority;
static int rmon_refcount;
static int rmon_serialnum;
static boolean rmon_sanitize_packets;

static void rmon_task(void);
extern void lance_setup_rmon(hwidbtype *idb);

/*
 * rmon_enqueue
 * Queues a packet for rmon processing if the queue isn't full
 * This is called at interrupt level from rmon_input() or rmon_output()
 */
inline static void
rmon_enqueue (watched_queue *qptr, paktype *rmonpak, rmon_t *rmondata)
{
    GET_TIMESTAMP(rmonpak->inputtime);
    if (process_enqueue(qptr, rmonpak)) {
	rmon_enqueued_packets++;
	rmon_queue_highwater =
	    MAX(process_queue_size(qptr), rmon_queue_highwater);
    } else {
	rmondata->rmon_drops++;
	rmon_dropped_packets++;
	datagram_done(rmonpak);
    }
}

static boolean
rmon_input (hwidbtype *idb, paktype *pak)
{
    paktype *pakcopy;

    if (idb->rmon) {

	rmon_input_packets++;

	/*
	 * need to determine if this is a packet that the router would
	 * have normally seen, or if we are only seeing it because rmon
	 * has placed the interface in promiscous mode.  we will consume
	 * the packet iff 1) we are not bridging on this interface, 2)
	 * rmon is in segment (promiscous) mode, and 3) the MAC address
	 * of the packet does not indicate a local destination
	 */

	if ((!RUNNING_IF(LINK_BRIDGE, idb->firstsw)) &&
	    (idb->rmon->rmon_mode == RMON_PROMISCUOUS)) {

	    /*
	     * we are not bridging and we are in promiscuous mode.
	     * if the packet is not a broadcast, not a multicast, and not
	     * addressed to this interface, then consume it
	     *
	     * NOTE WELL: we should probably enhance this code to consume
	     * multicasts that we are not interested in
	     */

	    ether_hdr *ehdr = (ether_hdr *)pak->datagramstart;
	    if (!((ehdr->daddr[0] & 1) || 
		  ieee_equal(ehdr->daddr, &idb->hardware[0]))) {
		rmon_enqueue(RMONpacketQ, pak, idb->rmon);
		rmon_filtered_packets++;
		return (TRUE);
	    }
	}
	
	/*
	 * the packet was not received just because of rmon.
	 * make a copy for rmon and return the original for processing
	 */
	pakcopy = pak_duplicate(pak);
	if (pakcopy)
	    rmon_enqueue(RMONpacketQ, pakcopy, idb->rmon);
    }
    rmon_passed_packets++;
    return (FALSE);
}

static void
rmon_output (hwidbtype *idb, paktype *pak)
{
    paktype *pakcopy;

    if (idb->rmon && (idb->firstsw->hwptr == idb)) {

	rmon_input_packets++;
	/*
	 * make a copy for rmon and return the original for processing
	 */
	pakcopy = pak_duplicate(pak);
	if (pakcopy) {
	    /*
	     * make sure all the buffer pointers are set
	     */
	    pakcopy->rxtype = ether_decode_encapsulation_inline(
                                  pakcopy,
			          (ether_hdr *)pak->datagramstart);
	    pakcopy->if_output = idb->firstsw;
	    pakcopy->if_input = NULL;
	    rmon_enqueue(RMONpacketQ, pakcopy, idb->rmon);
	}
    }
    rmon_passed_packets++;
}


static ulong
rmon_errors (hwidbtype *hwidb)
{
    return (hwidb->counters.input_frame + hwidb->counters.input_crc);
}

static ulong
rmon_collisions (hwidbtype *hwidb)
{
    N_ITEM_LIST(tmp, 2);

    item_desc_init(&tmp_item[0], ETHER_HW_OUTPUT_ONE_COLLISION, sizeof(ulong));

    item_desc_init(&tmp_item[1], ETHER_HW_OUTPUT_MORE_COLLISION,
		   sizeof(ulong));

    (*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE,
				       &tmp_list);

    return ((hwidb->type == IDBTYPE_LANCE ?
	     tmp_item[0].u.l_value :     /* one_coll */
	     hwidb->counters.output_collision) +
	    tmp_item[1].u.l_value);      /* more_coll */
}

static const char *ifIndex = "1.3.6.1.2.1.2.2.1.1";

static void
rmon_enable_etherstats (hwidbtype *idb)
{
    etherStatsEntry_t esp;
    doList_t        de;
    OctetString    *etherStatsOwner;
    OID            *etherStatsDataSource = 0;
    char	    buf[80];

    etherStatsOwner = MakeOctetStringFromText("config");
    sprintf(buf, "%s.%ld", ifIndex, idb->snmp_if_index);
    etherStatsDataSource = MakeOIDFromDot(buf);

    /* set the table entry */
    memset(&de, 0, sizeof(doList_t));
    memset(&esp, 0, sizeof(etherStatsEntry_t));
    de.data = (void *) &esp;
    if (k_etherStatsEntry_set_defaults(&de) != NO_ERROR) {
        DPRINTF((APWARN, "%s: can't set etherStats defaults\n", fname));
        goto done;
    }
    FreeOID(esp.etherStatsDataSource);
    FreeOctetString(esp.etherStatsOwner);
    esp.etherStatsOwner = etherStatsOwner;
    esp.etherStatsDataSource = etherStatsDataSource;
    esp.etherStatsIndex = idb->snmp_if_index;
    esp.etherStatsStatus = D_etherStatsStatus_valid;

    /* clear our parameters */
    etherStatsOwner = 0;
    etherStatsDataSource = 0;

    /* do the set */
    k_etherStatsEntry_set(&esp, (ContextInfo *) 0, ADD_MODIFY);

done:
    FreeOID(etherStatsDataSource);
    FreeOctetString(etherStatsOwner);

}

static void
rmon_disable_etherstats (hwidbtype *idb)
{
}

static const char *
rmon_priority_string (int priority)
{
    switch (priority) {
    case PRIO_HIGH: return "high";
    case PRIO_NORMAL: return "normal";
    default: return "low";
    }
}

void
rmon_global_command (parseinfo *csb)
{
    queuetype overflow;
    paktype *pak;
    ulong rmon_queue_size;

    if (csb->nvgen) {
	switch (csb->which) {
	case RMON_QUEUESIZE:
	    rmon_queue_size = process_queue_max_size(RMONpacketQ);
	    nv_write(rmon_queue_size != RMON_QUEUE_SIZE_DEF,
		     "%s %d", csb->nv_command, rmon_queue_size);
	    break;
	case RMON_PRIORITY:
	    nv_write(desired_rmon_priority != RMON_PRIORITY_DEF,
		     "%s %s", csb->nv_command,
		     rmon_priority_string(desired_rmon_priority));
	    break;
	}
	return;
    }

    switch (csb->which) {
    case RMON_QUEUESIZE:
	queue_init(&overflow, 0);
	rmon_queue_size = csb->sense ? GETOBJ(int,1) : RMON_QUEUE_SIZE_DEF;
	process_resize_queue(RMONpacketQ, rmon_queue_size, &overflow);
	while ((pak = dequeue(&overflow)) != NULL)
	    retbuffer(pak);
	break;
    case RMON_PRIORITY:
	if (csb->sense) {
	    desired_rmon_priority = GETOBJ(int,1);
	} else {
	    desired_rmon_priority = RMON_PRIORITY_DEF;
	}
	process_set_boolean(rmon_priority_change, TRUE);
	break;
    }
}

void
rmon_int_command (parseinfo *csb)
{
    hwidbtype *csbidb;
    ulong oldmode, newmode;
    rmon_t *rmon_p;

    csbidb = csb->interface->hwptr;

    if (csb->nvgen) {
	if (csbidb->rmon) {
	    nv_write(csbidb->rmon->rmon_mode == csb->which,
		     csb->nv_command);
	}
	return;
    }

    if (csbidb->rmon) {
	oldmode = csbidb->rmon->rmon_mode;
    } else {
	oldmode = RMON_DISABLED;
    }

    if (csb->sense) {
	newmode = csb->which;
    } else {
	newmode = RMON_DISABLED;
    }

    if (newmode != oldmode) {
	if (oldmode == RMON_DISABLED) {
	    rmon_p = malloc_named(sizeof(rmon_t), "RMON IDB");
	    if (rmon_p == NULL) {
		return;
	    }
	    rmon_p->rmon_mode = newmode;
	    rmon_p->rmon_errors = rmon_errors(csbidb);
	    rmon_p->rmon_collisions = rmon_collisions(csbidb);
	    rmon_p->rmon_drops = 0;
	    rmon_p->rmon_serialnum = 0;
	    if (rmon_pid == NO_PROCESS) {
		rmon_pid = process_create(rmon_task, "RMON",
					  NORMAL_STACK, desired_rmon_priority);
		if (rmon_pid == NO_PROCESS) {
		    free(rmon_p);
		    return;
		}
		process_set_crashblock(rmon_pid, TRUE);
		current_rmon_priority = desired_rmon_priority;
	    }
	    rmon_refcount++;
	    csbidb->rmon = rmon_p;
	    rmon_enable_etherstats(csbidb);
	} else if (newmode == RMON_DISABLED) {
	    /* we are disabling rmon on this interface */
	    rmon_disable_etherstats(csbidb);
	    free(csbidb->rmon);
	    csbidb->rmon = NULL;
	    if (--rmon_refcount == 0) {
		/* no interfaces have rmon enabled so kill the process */
		process_kill(rmon_pid);
		rmon_pid = NO_PROCESS;
	    }
	} else {
	    /* simply changing the rmon mode */
	    csbidb->rmon->rmon_mode = newmode;
	}
	lance_setup_rmon(csbidb);
    }
}

static void
show_rmon_task (void)
{
    automore_enable(NULL);
    printf("\nRMON task statistics:");
    printf("\n%d packets input (%d promiscuous), %d drops",
	   rmon_input_packets, rmon_filtered_packets, rmon_dropped_packets);
    printf("\n%d packets processed, %d on queue, queue utilization %d/%d",
	   rmon_dequeued_packets,
	   rmon_enqueued_packets - rmon_dequeued_packets,
	   rmon_queue_highwater, process_queue_max_size(RMONpacketQ));
    automore_disable();
}


static void
rmon_show_command (uint flagword)
{
    if (flagword & (1<<RMON_SHOW_STATISTICS))
        show_rmon_etherstats();
    if (flagword & (1<<RMON_SHOW_HISTORY))
        show_rmon_history();
    if (flagword & (1<<RMON_SHOW_HOSTS))
        show_rmon_hosts();
    if (flagword & (1<<RMON_SHOW_TOPN))
        show_rmon_topn();
    if (flagword & (1<<RMON_SHOW_MATRIX))
        show_rmon_matrix();
    if (flagword & (1<<RMON_SHOW_FILTER))
        show_rmon_filters();
    if (flagword & (1<<RMON_SHOW_CAPTURE))
        show_rmon_capture();
    if ((flagword & (1<<RMON_SHOW_TASK)) || (flagword == 0))
        show_rmon_task();

}

void
rmon_task_init (subsystype *subsys)
{
    RMONpacketQ = create_watched_queue("RMON queue",
				       RMON_QUEUE_SIZE_DEF, 0);
    rmon_priority_change = create_watched_boolean("RMON reprioritize", 0);
    rmon_parser_init();
    rmon_pid = NO_PROCESS;
    desired_rmon_priority = RMON_PRIORITY_DEF;
    rmon_serialnum = 0;
    rmon_input_packets = 0;
    rmon_filtered_packets = 0;
    rmon_passed_packets = 0;
    rmon_dropped_packets = 0;
    rmon_enqueued_packets = 0;
    rmon_dequeued_packets = 0;
    rmon_queue_highwater = 0;
    rmon_sanitize_packets = TRUE;
    reg_add_rmon_input(rmon_input, "rmon_input");
    reg_add_rmon_output(rmon_output, "rmon_output");
    reg_add_rmon_show_command(rmon_show_command, "rmon_show_command");
}

static void
process_rmon_queue (void)
{
    ulong ifIndexInst[LNifIndex+1];
    OID ifIndexInstOID;

    paktype *pak;
    hwidbtype *idb;
    PacketState packet;
    rmon_t *rmon_p;
    ulong count;
    
    idb = NULL;
    while ((pak = process_dequeue(RMONpacketQ))) {
	rmon_dequeued_packets++;
#ifdef DEBUG
	int i;
	for (i = 0; i < 64; i++)
	    printf("%x", pak->datagramstart[i]);
	printf("\n");
#endif
	if (pak->if_input != NULL) {
	    idb = pak->if_input->hwptr;
	} else if (pak->if_output != NULL) {
	    idb = pak->if_output->hwptr;
	}
	if (idb) {
	    rmon_p = idb->rmon;

	    /*
	     * apply security posture to the packet
	     */
	    if (rmon_sanitize_packets) {
		rmon_sanitize(pak);
	    }
	    
	    /* create ifIndex instance associated with the interface */
	    memcpy(ifIndexInst, IDifIndex, LNifIndex * sizeof(ulong));
	    ifIndexInst[LNifIndex] = idb->snmp_if_index;
	    ifIndexInstOID.length = LNifIndex+1;
	    ifIndexInstOID.oid_ptr = ifIndexInst;
	    
	    packet.source = &ifIndexInstOID;
	    packet.p = pak->datagramstart;
	    packet.size = pak->datagramsize + 4;
	    count = rmon_errors(idb);
	    packet.error = count - rmon_p->rmon_errors;
	    rmon_p->rmon_errors = count;
	    count = rmon_collisions(idb);
	    packet.collisions = count - rmon_p->rmon_collisions;
	    rmon_p->rmon_collisions = count;
	    packet.drops = rmon_p->rmon_drops;
	    rmon_p->rmon_drops = 0;

	    if (pak->if_input != NULL) {
		packet.orderapprox = 0;
	    } else {

		/*
		 * output packets have latency between being queued
		 * and when they are sent
		 */
		packet.orderapprox = TRUE;
	    }

	    packet.id = rmon_p->rmon_serialnum++;
	    COPY_TIMESTAMP(pak->inputtime, packet.timestamp);
	    UpdateTables(&packet);
	}
	datagram_done(pak);
	process_may_suspend();
    }
}


static process
rmon_task (void)
{
    ulong major, minor;

    process_watch_queue(RMONpacketQ, ENABLE, RECURRING);
    process_watch_boolean(rmon_priority_change, ENABLE, RECURRING);
    while (TRUE) {
        process_wait_for_event();
        while (process_get_wakeup(&major, &minor)) {
            switch (major) {
	    case QUEUE_EVENT:
		process_rmon_queue();
		break;
	    case BOOLEAN_EVENT:
		if (current_rmon_priority != desired_rmon_priority) {
		    process_set_priority(desired_rmon_priority);
		    current_rmon_priority = desired_rmon_priority;
		}
		process_set_boolean(rmon_priority_change, FALSE);
		break;
	    default:
                errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
                break;
            }
	}
    }
}
/*
 * RMON subsystem header
 */

#define RMON_MAJVERSION 1
#define RMON_MINVERSION 0
#define RMON_EDITVERSION 0

SUBSYS_HEADER(rmon, RMON_MAJVERSION, RMON_MINVERSION, RMON_EDITVERSION,
	      rmon_task_init, SUBSYS_CLASS_MANAGEMENT,
	      "req: rmonmib rmonlite", "seq: rmonlite");
