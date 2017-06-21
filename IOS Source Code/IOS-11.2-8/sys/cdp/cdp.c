/* $Id: cdp.c,v 3.7.8.12 1996/09/11 02:56:10 perryl Exp $
 * $Source: /release/112/cvs/Xsys/cdp/cdp.c,v $
 *------------------------------------------------------------------
 * Support for CDP (Cisco Discovery Protocol)
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp.c,v $
 * Revision 3.7.8.12  1996/09/11  02:56:10  perryl
 * CSCdi65039:  vtemplate loses cdp enable value after reload
 * Branch: California_branch
 * nvgen "cdp enable" for vtemplate if it is cdp enable.
 *
 * Revision 3.7.8.11  1996/08/28  12:38:02  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.8.10  1996/08/27  14:56:51  bcole
 * CSCdi67267:  Dialer profiles breaks CDP and ODR
 * Branch: California_branch
 *
 * Revision 3.7.8.9  1996/08/27  05:12:50  bcole
 * CSCdi67267:  Dialer profiles breaks CDP and ODR
 * Branch: California_branch
 *
 * Avoid encapsulating CDP packets out ISDN B channels directly, as nasty
 * things happen...
 *
 * Revision 3.7.8.8  1996/08/15  00:44:41  tylin
 * CSCdi66084:  cdp should send updates on lowest numbered vlan
 * Branch: California_branch
 *
 * Revision 3.7.8.7  1996/07/08  18:39:38  tylin
 * CSCdi61348:  CDP ALWAYS sends FULL MTU size datagram (feature/bug?)
 *    Allocate data buffer with smaller of interface MTU and Huge data
 *    size.
 * Branch: California_branch
 *
 * Revision 3.7.8.6  1996/07/02  23:10:25  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.7.8.5  1996/06/12  19:08:28  pst
 * CSCdi60173:  Eliminate cisco definition of ntohl in favor of
 * swap_xx_bits (partial commit part 1)
 * Branch: California_branch
 *
 * Revision 3.7.8.4  1996/04/19  17:17:55  bcole
 * CSCdi55160:  CDP: Runt packet check is incorrect
 * Branch: California_branch
 *
 * When packet len < encapsulation len, CDP's computed length was going
 * negative but promoted to an unsigned.  Fix length check.
 *
 * Revision 3.7.8.3  1996/04/15  21:12:59  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.7.8.2  1996/03/23  01:29:40  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.7.8.1  1996/03/18  19:01:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.2.4  1996/03/16  06:29:35  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.2.3  1996/03/13  01:12:23  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.2.2  1996/03/07  08:39:22  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.2.1  1996/02/20  00:20:12  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.7  1996/02/28  03:05:13  rbadri
 * CSCdi49394:  CDP can not be enabled on LANE interfaces
 * Enable CDP on ATM interfaces, when LANE is configured and
 * when the LANE client is operational.
 *
 * Revision 3.6  1996/02/13  08:10:56  dstine
 * CSCdi48797:  Extraneous definitions in interface_private.h
 *
 * Revision 3.5  1996/02/01  06:00:20  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1996/01/18  19:55:50  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.3  1995/11/17  08:49:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:11:44  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:05:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.11  1995/08/31  23:08:37  asastry
 * CSCdi39534:  CDP should wait for system_configured flag before running
 *
 * Revision 2.10  1995/07/17  23:15:55  hampton
 * Miscellaneous timer cleanups for CDP.  [CSCdi37215]
 *
 * Revision 2.9  1995/07/06 22:59:19  achopra
 * CSCdi32586:  dynamic frame relay map address for dec is incorrect.
 *
 * Revision 2.8  1995/06/29  01:21:48  asastry
 * CSCdi36442:  <cdp enable> on interface should not override <no cdp run>
 *
 * Revision 2.7  1995/06/28  20:47:13  asastry
 * CSCdi36099:  CDP forces hardware reset for every defined subinterface
 *
 * Revision 2.6  1995/06/27  07:21:13  smackie
 * Convert CDP to use platform_get_string(). Tidy up subsystem entry
 * points to use the right prototype. (CSCdi36461)
 *
 * Revision 2.5  1995/06/18  19:39:08  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.4  1995/06/16  17:06:11  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.3  1995/06/09  12:58:49  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/08 04:18:47  asastry
 * CSCdi35395:  CDP should have registry callback for interface
 * initialization
 *
 * Revision 2.1  1995/06/07  20:14:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */
#include <ciscolib.h>
#include "sys_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "ieee.h"
#include "packet.h"
#include "../ui/debug.h"
#include "../if/network.h"
#include "config.h"
#include "cdp_externs.h"
#include "cdp_debug.h"
#include "parser.h"
#include "name.h"
#include "parser_defs_cdp.h"
#include "../dev/monitor1.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "../wan/dialer.h"
#include "cdp_registry.h"
#include "../cdp/cdp_registry.regc"
#include "media_registry.h"

/*
 * Global variables
 */
boolean	cdp_ever_start;                /* TRUE if CDP process is ever invoked */
boolean cdp_running;                   /* TRUE if CDP running */
boolean cdp_explicitly_disarmed;       /* TRUE if CDP was explicitly turned
					  off by user */
ushort cdp_xmit_frequency;             /* frequency of packet xmits */
ushort cdp_holdtime;                   /* holdtime advertised in packets */
uchar cdp_multicast[] = {0x01, 0x00, 0x0c, 0xcc, 0xcc, 0xcc};
uchar cdp_tr_func_addr[] = {0xC0, 0x00, 0x08, 0x00, 0x00, 0x00};
ulong cdp_traffic[CDP_MAXTRAFFIC];     /* statistics */
ulong cdp_global_device_number;        /* each device (by name) will have a
					  unique device ID. Needed by SNMP
					  for getnext */
long cdp_regcode;			/* Our dynamically created registry */

/*
 * Local variables
 */
static watched_queue *cdpQ;               /* Queue of CDP packets */
static int cdpfork;                       /* Fork of CDP process */
static sys_timestamp cdp_next_activation; /* periodic CDP timer */
static char cdp_platform_string[CDP_MAX_PLATFORM_STR_LEN + 1];
static int  cdp_platform_string_size;

/*
 * CDP subsystem header
 */

#define CDP_MAJVERSION 1
#define CDP_MINVERSION 0
#define CDP_EDITVERSION  1

SUBSYS_HEADER(cdp, CDP_MAJVERSION, CDP_MINVERSION, CDP_EDITVERSION,
	      cdp_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);

/*
 * cdp_supported_int
 *
 * Returns TRUE if CDP is returned on the interface.
 *
 * CDP can theoretically be used over the following interface types.  In
 * reality, it tends to fail over NBMA media due to lack of a static map
 * table entry.  For DDR, CDP makes use of IP static map table entries to
 * avoid the need to configure a CDP static map table entry.
 */
boolean cdp_supported_int (idbtype *swidb)
{
    hwidbtype *idb;

    if (swidb == NULL)
	return(FALSE);
    idb = swidb->hwptr;
    return(is_atm_lane(swidb) ||
	   is_hdlc(idb) ||
	   is_smds(idb) || 
	   is_x25(idb) ||
	   is_ppp(idb) || 
	   is_frame_relay(idb) ||
	   is_ethernet(idb) || 
	   is_tokenring(idb) ||
	   is_fddi(idb));
}


/*
 * cdp_init_idb
 *
 * Initialize interface-related stuff for CDP.
 */

void cdp_init_idb (idbtype *idb, boolean setup_hwidb)
{
    /*
     * Don't anything if we are still in reading system configuration state,
     * because the enctype may not be correct.
     */
    if (!cdp_ever_start)
	return;

    /*
     * Enable CDP on CDP supported interfaces 
     * (on which CDP is enabled by default).
     */
    if (CDP_DEFAULT_ENABLE_INT(idb)) 
	cdp_intf_enable(idb, FALSE); 
}

/*
 * cdp_init
 *
 * Initialize all CDP-related stuff.
 */
void cdp_init (subsystype *subsys)
{
    int i;
    idbtype *idb;

    cdp_media_registry();
    cdp_add_registry_calls();
    cdp_debug_init();
    cdp_parser_init();
    GET_TIMESTAMP(cdp_next_activation);
    cdp_xmit_frequency = CDP_DEF_XMIT_INTERVAL;
    cdp_holdtime = CDP_DEF_TTL;
    cdpQ = NULL;
    cdp_global_device_number = 1;
    cdp_ever_start = FALSE;

    FOR_ALL_SWIDBS(idb) {
	idb->cdp_enabled = FALSE;
	TIMER_STOP(idb->cdp_update_timer);
    }

    for (i = 0; i < CDP_MAXTRAFFIC; i++)
	cdp_traffic[i] = 0;

    cdp_init_platform_strings();

    /*
     * Start CDP by default.
     */
    cdp_explicitly_disarmed = FALSE;
    cdp_start_process();
}

/*
 * cdp_running_test
 *
 * Return TRUE if CDP is running on this system.
 */
static boolean cdp_running_test (void) 
{
    return(cdp_running);
}

/*
 * cdp_on_idb 
 *
 * Return TRUE if CDP is running on the particular interface.
 */
static boolean cdp_on_idb (idbtype *idb)
{
    if ((!cdp_running) || (!idb->cdp_enabled)) {
	return(FALSE);
    } else {
	return(TRUE);
    }    
}

/*
 * cdp_start_process
 *
 * Start the CDP process
 */
void cdp_start_process (void)
{
    idbtype *idb;

    if (!cdpfork) {
	cdpfork = process_create(cdp_prot, "CDP Protocol", NORMAL_STACK,
				 PRIO_NORMAL);
    }

    cdp_running = TRUE;
    cdp_global_device_number = 1;

    /*
     * Register CDP notification functions
     */
    reg_add_will_reload(cdp_reload_notify, "cdp_reload_notify");
    reg_add_swif_goingdown(cdp_if_down_notify, "cdp_if_down_notify");
    reg_add_swif_comingup(cdp_if_up_notify, "cdp_if_up_notify");

    if (!cdp_explicitly_disarmed) {
	FOR_ALL_SWIDBS(idb) {
	    if (idb->cdp_enabled) {
		cdp_macaddrs_update(idb);
		if (!is_atm(idb->hwptr))
	            (*idb->hwptr->reset)(idb->hwptr);
	    }
	}
    }
    cdp_explicitly_disarmed = FALSE;
}

/*
 * cdp_enqueue
 * Called from interrupt level to put a packet on the CDP input queue
 */

void cdp_enqueue (paktype *pak)
{
    if (pak->if_input->cdp_enabled && cdp_running) {
	process_enqueue_pak(cdpQ, pak);
    } else
	protocol_discard(pak, FALSE);
}


/*
 * cdp_prot
 * 
 * Main CDP (Cisco Discovery Protocol) process
 */

process cdp_prot (void)
{
    int count;
    paktype *pak;
    ulong major, minor;
    idbtype *idb;
    boolean enable;

    process_wait_on_system_init();

    /*
     * If this is the first time to start CDP, scan each swidb to enable
     * cdp for supported interface. In starting mode, the cdp_update_timer
     * is used to detect if any configuration is read in. If non-zero
     * then enable/disable cdp per cdp_enabled field, otherwise use
     * default setting to disable/enable cdp.
     */
    if (!cdp_ever_start) {
	cdp_ever_start = TRUE;
	FOR_ALL_SWIDBS(idb) {
	    enable = TIMER_RUNNING(idb->cdp_update_timer) ?
		     idb->cdp_enabled : CDP_DEFAULT_ENABLE_INT(idb);
	    idb->cdp_enabled = FALSE;
	    if (enable) {
		cdp_intf_enable(idb, FALSE);
	    }
	}
    }

    if (!system_loading)
	process_watch_timer(&cdp_next_activation, ENABLE);
    cdpQ = create_watched_queue("CDP packets", 0, 0);
    process_watch_queue(cdpQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_CDP, cdp_enqueue, "cdp_enqueue");

    while (cdp_running) {
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case QUEUE_EVENT:
		/* Process a limited number of packets */
		count = PAK_SWITCHCOUNT;
		while (--count > 0) {	
		    pak = process_dequeue(cdpQ); 
		    if (!pak)
			break;

		    if (!interface_up(pak->if_input)) {
			datagram_done(pak);
			continue;
		    }

		    idb_increment_rx_proto_counts(pak, ACCT_PROTO_CDP);

		    cdp_handle_packet(pak);

		    datagram_done(pak);
		}
		break;

	      case TIMER_EVENT:
		TIMER_START(cdp_next_activation, (CDP_NEXT_ACTIVATION * ONESEC));
		cdp_cache_ager();
		process_may_suspend();
		cdp_send_periodic_updates();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }

    /* Clean up ... */
    reg_delete_raw_enqueue(LINK_CDP);
    process_watch_queue(cdpQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(cdpQ)) != NULL)
	retbuffer(pak);
    delete_watched_queue(&cdpQ);
    process_watch_timer(&cdp_next_activation, DISABLE);
    cdp_clean_up();

    /* Die ... */
    cdpfork = 0;
    process_kill(THIS_PROCESS);
}

/*
 * cdp_clean_up
 *
 * Clean up after myself
 */
void cdp_clean_up(void)
{
    reg_invoke_bridge_entry_global_remove(cdp_multicast);
    cdp_clear_table();
}

/*
 * cdp_checksum
 * 
 * Apply checksum for the specified data.
 */
ushort cdp_checksum (ushort *p, int count)
{
    register long crc;
    ushort i, *data;

    data = p;
    crc = 0;
    while (count > 1) {
	crc += *data++;
	count -= 2;
    }

    if (count > 0)
	crc += *(char *)data;

    /*
     * Fold 32-bit sum into 16 bits
     */
    while (crc >> 16) {
        crc = (crc & 0xFFFF) + (crc >> 16);
    }

    i = (short) crc;
    return (~i);
}

/*
 * cdp_handle_packet
 * 
 * Process incoming CDP packet. Packet is disposed of by caller.
 */
void cdp_handle_packet (paktype *pak)
{
    cdp_hdrtype *cdphdr;
    cdp_info_type *item, *device_id_item;
    ushort bytes_left, encap_len;
    long pak_len;
    cdp_cache_type *cdp_cache_entry;
    boolean new_entry;
    ulong device_num;
    ushort item_length, item_code;

    /*
     * Validity checks
     */
    cdphdr = (cdp_hdrtype *) cdpheadstart(pak);
    encap_len = pak->network_start - pak->datagramstart;
    pak_len = pak->datagramsize - encap_len;
    if (pak_len < CDP_MIN_HEADER_LENGTH) {
	cdp_evbuginf("\nCDP-EV: %s CDP packet len (%d) less than min"
		     " header len (4)", pak->if_input->namestring, pak_len);
	cdp_traffic[CDP_HDRSYNTAX]++;
	return;
    }

    if (cdphdr->version != CDP_VERSION) {
	cdp_evbuginf("\nCDP-EV: Bad version number in header");
	cdp_traffic[CDP_HDRSYNTAX]++;
	return;
    }

    /*
     * Packet checksum
     */
    if (cdp_checksum((ushort *)cdphdr, pak_len)) {
	cdp_evbuginf("\nCDP-EV: Bad checksum in header");
	cdp_traffic[CDP_CHECKSUM]++;
	return;
    }

    /*
     * Validate all TLV's. While we are scanning, look for the item that
     * has the device identifier, since we need that to process the information
     * that we've received.
     */
    device_id_item = NULL;
    bytes_left = pak_len - CDP_HEADER_OVERHEAD;
    item = (cdp_info_type *)cdphdr->item;
    while (bytes_left > 0) {
	item_length = GETSHORT(&item->length);
	item_code = GETSHORT(&item->code);

        if (item_length == 0) {
            cdp_evbuginf("\nCDP-EV: Received item (type : %x) with length of "
			 "zero", item_code);
            cdp_traffic[CDP_BADPACKET]++;
            return;
        }

	if (item_length > bytes_left) {
	    cdp_evbuginf("\nCDP-EV: Invalid TLV portion");
	    cdp_traffic[CDP_BADPACKET]++;
	    return;
	}

	if (!(CDP_TYPE_KNOWN(item_code))) {
	    /*
	     * Print debug message if we see an unknown TYPE.
	     */
	    cdp_evbuginf("\nCDP-EV: Unrecognized type (%x) seen in TLV",
			 item_code);
	}
	if (item_code == CDP_DEVICE_NAME)
	    device_id_item = item;

	bytes_left -= item_length;
        item = (cdp_info_type *)((char *)item + item_length);
    }

    if (bytes_left != 0) {		/* Partial TLV at end */
        cdp_evbuginf("\nCDP-EV: Partial TLV at end of packet");
	cdp_traffic[CDP_BADPACKET]++;
	return;
    }
    	    
    /*
     * If we did not find a TLV containing device ID info, we can't process
     * the packet, since we don't know the identity of the sender.
     */
    if (!device_id_item) {
	cdp_evbuginf("\nCDP-EV: Device ID information is missing in packet");
	cdp_traffic[CDP_BADPACKET]++;
	return;
    }

    new_entry = FALSE;         /* assume entry is already in hash table */
    cdp_cache_entry = cdp_hash_lookup((char *)device_id_item->value,
				     GETSHORT(&device_id_item->length)
				                            - CDP_TLV_OVERHEAD,
				     pak->if_input, &device_num);
    if (!cdp_cache_entry) {
	cdp_cache_entry = malloc(sizeof(cdp_cache_type));
	if (!cdp_cache_entry) {
	    cdp_traffic[CDP_NOMEM]++;
	    return;	    
	}
	/*
	 * If 'device_num' is non-zero, it means we found a hash table entry
	 * with the same name (but not the same idb - in this case,
	 * 'cdp_cache_entry' would have been non-NULL). Otherwise, it means
	 * we have a new entry, and so assign a new device number from the
	 * "global device number space".
	 */
	if (device_num != 0)
	    cdp_cache_entry->device_number = device_num;
	else
	    cdp_cache_entry->device_number = cdp_global_device_number++;

	new_entry = TRUE;
    }
    if (cdp_pkt_debug) {
        buginf("\nCDP-PA: Packet received from ");
        buginf("%s", device_id_item->value);
	buginf(" on interface %s", pak->if_input->namestring); 
        buginf("\n**Entry %s found in cache**", new_entry?"NOT":"");
    }
    cdp_insert_packet_info(cdphdr, pak_len, pak->if_input, cdp_cache_entry,
			   new_entry);
    cdp_traffic[CDP_INPUT]++;

    return;
}


/*
 * cdp_send_periodic_updates_on_swidb
 * 
 * Send periodic CDP updates on the given swidb.
 */
void cdp_send_periodic_updates_on_swidb (idbtype *idb)
{
    if (SLEEPING(idb->cdp_update_timer)) {
	cdp_next_activation = TIMER_SOONEST(cdp_next_activation,
					    idb->cdp_update_timer);
    } else {

	/*
	 *Timer has expired - send update 
	 */
	TIMER_START(idb->cdp_update_timer, (cdp_xmit_frequency * ONESEC));
	cdp_next_activation = TIMER_SOONEST(cdp_next_activation,
					    idb->cdp_update_timer);
	    
	if (interface_up(idb) && cdp_supported_int(idb))
	    cdp_send_update(idb, cdp_holdtime);

	process_may_suspend();
    }
}

/*
 * cdp_broadcast_if
 * Should we send cdp broadcasts out this interface?
 * Return TRUE if so.
 */

static inline boolean cdp_broadcast_if (idbtype *swidb)
{
    /*
     * For dialer interfaces, special case check needed to avoid
     * redundantly broadcasting out interface.
     */
    if (get_netcfg_idb(swidb) != swidb)
	return(FALSE);
    return(swidb->cdp_enabled);
}

/*
 * cdp_send_periodic_updates
 * 
 * Send periodic CDP updates.
 */
void cdp_send_periodic_updates (void)
{
    hwidbtype *hwidb;
    idbtype *swidb;

    FOR_ALL_HWIDBS(hwidb) {
	/*
	 * If any ISL vLan configured, send on the lowest ISL # only.
	 */
	if (hwidb->isl_vlan_on_hwidb) {
	    swidb = reg_invoke_isl_vlan_lowest_colour(hwidb);
	    if (swidb) {
		cdp_send_periodic_updates_on_swidb(swidb);
		continue;
	    }
	}

	/*
	 * Otherwise, send on each enabled swidb.
	 */
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	    if (!cdp_broadcast_if(swidb))
		continue;
	    cdp_send_periodic_updates_on_swidb(swidb);
	}
    }
}

/*
 * cdp_tlv_namestring
 * Map TLV type codes to names
 */

static uchar *cdp_tlv_namestring (ushort tlv_code)
{
    switch (tlv_code) {
    case CDP_DEVICE_NAME:
	return("device");
    case CDP_ADDRESS_TYPE:
    case CDP_PORT_ID_TYPE:
	return("port ID");
    case CDP_CAPABILITIES_TYPE:
	return("capability");
    case CDP_VERSION_TYPE:
	return("version");
    case CDP_PLATFORM_TYPE:
	return("hardware");
    case CDP_IPPREFIX_TYPE:
	return("IP network prefix");
    default:
	return("unknown");
    }
}
/*
 * cdp_insert_dummy
 * NOOP the insertion of a TLV.
 */

static boolean cdp_insert_dummy (cdp_info_type *item, idbtype *idb,
			  ushort *bytes_left)
{
    PUTSHORT(&item->length, 0);
    return(TRUE);
}

/*
 * Define all the CDP TLV insertion functions, indexed by TLV type code
 */
cdp_add_func_t add_tlv_func[] = {
    NULL,
    cdp_insert_device_name,
    cdp_insert_address,
    cdp_insert_port_id,
    cdp_insert_capabilities,
    cdp_insert_version,
    cdp_insert_hardware,
    cdp_insert_dummy
};

/*
 * cdp_tlv_to_send
 * Determine if we still have TLVs to insert into the packet that is being
 * built.
 */

static inline boolean cdp_tlv_to_send (boolean *add_tlv)
{
    ushort tlv_code;
    
    for (tlv_code = CDP_DEVICE_NAME; tlv_code <= CDP_MAX_TYPE; tlv_code++)
	if (add_tlv[tlv_code])
	    return(TRUE);
    return(FALSE);
}

/*
 * cdp_send_update
 *
 * Send a CDP update on a particular interface. The 'ttl' argument specifies
 * the TTL that is sent in the packet. If the ttl is zero, send only
 * device ID information.
 *
 * We allocate a buffer of interface MTU size. If, while filling in the
 * buffer, we see that we will overrun the buffer, we just return from the
 * particular function (with FALSE). If this function sees a return value
 * of FALSE, it will send the packet with the information that has been put in
 * up to that point, and then we iterate again, basically picking up from
 * where we left off. In the case of addresses, we send whatever
 * information fits in the buffer. This makes it easy to handle address info
 * that we receive (since we don't have to deal with address info
 * arriving in multiple fragments.
 */
void cdp_send_update (idbtype *idb, uchar ttl)
{
    paktype *pak;
    cdp_info_type *item;
    cdp_hdrtype *cdphdr;
    ushort pak_length, pak_alloc_size, bytes_left, item_length;
    boolean status;
    boolean add_tlv[CDP_MAX_TYPE + 1];
    ushort tlv_code;

    boolean more_frags, second_try;

    /*
     * Flags for sending the various types of information. Once we have
     * sent the particular type of information, we set the corresponding
     * flag to FALSE.
     */
    for (tlv_code = CDP_DEVICE_NAME; tlv_code <= CDP_MAX_TYPE; tlv_code++)
	add_tlv[tlv_code] = TRUE;
    more_frags = FALSE;
    second_try = FALSE;

    while (TRUE) {
	/*
	 * Ensure that we will not need to fragment by allocating a buffer
	 * of interface MTU size.
	 */
	pak_alloc_size = min(idb->hwptr->maxmtu, CDP_MAX_DATA_SIZE);
        pak = getbuffer(pak_alloc_size);
        if (!pak) {
	    cdp_traffic[CDP_NOMEM]++;
	    return;
        }
        /*
         * Find pointer to CDP header, and fill in some information.
	 */
	cdphdr = (cdp_hdrtype *) cdpheadstart(pak);
	cdphdr->version = CDP_VERSION;
	cdphdr->ttl = ttl;
	item = (cdp_info_type *)cdphdr->item;
	pak_length = 0;
	bytes_left = pak_alloc_size - CDP_HEADER_OVERHEAD;

	/*
	 * Start filling in TLV's ...
	 */

	while (cdp_tlv_to_send(add_tlv)) {
	    /*
	     * Add device name information. If the function returns TRUE,
	     * it means we added the information successfully, and so we
	     * set the flag "add_device_info" to FALSE. If the function
	     * returned FALSE, it means we did not have enough space, 
	     * which means we should just quit. We need to be able to have
	     * at least enough space to contain device ID information.
	     * 
	     * We also need to send device ID info when 'more_frags' is TRUE,
	     * since each fragment needs to have the sender's ID in it.
	     */
	    tlv_code = CDP_DEVICE_NAME;
	    if (add_tlv[tlv_code] || more_frags) {
		status = (*add_tlv_func[tlv_code])(item, idb, &bytes_left);
	        if (!status) {
		    /*
		     * There is an optimization we can make. Since device info
		     * is the very first thing that is added in the packet
		     * (and subsequent fragments), we can just quit if we
		     * don't have enough space to fit in this information.
		     */
		    cdp_evbuginf("\nCDP-EV: No space to add device information"
				 " - aborting");
		    retbuffer(pak);
		    return;
		}
	        add_tlv[tlv_code] = FALSE;
		item_length = GETSHORT(&item->length);
		if (item_length) {
		    item = (cdp_info_type *)((char *)item + item_length);
		}
	    }

	    /*
	     * If TTL is zero (which means the recipients should age out
	     * our entry), don't bother sending rest of the information. 
	     */
	    if (ttl == 0) {
		for (tlv_code = CDP_ADDRESS_TYPE; tlv_code <= CDP_MAX_TYPE;
		     tlv_code++)
		    add_tlv[tlv_code] = FALSE;
		break;
	    }
	    status = TRUE;
	    for (tlv_code = CDP_ADDRESS_TYPE;
		 status && (tlv_code <= CDP_MAX_TYPE); tlv_code++) {
		if (!add_tlv[tlv_code])
		    continue;
		/*
		 * Add the TLV info
		 */
	        status = (*add_tlv_func[tlv_code])(item, idb, &bytes_left);
		add_tlv[tlv_code] = (status) ? FALSE: TRUE;
		if (!status) {
		    if (tlv_code == CDP_ADDRESS_TYPE) {
			/*
			 * JUDGEMENT CALL HERE.  We decided that we'd overwrite
			 * address information when we receive it. For that
			 * to work, we need to make the assumption that all
			 * address info will fit in one packet. If it will
			 * not fit in one packet, we send whatever fits. We
			 * make sure that the first protocol to be sent is IP,
			 * followed by Novell, Appletalk, and then the other
			 * ones.
			 *
			 * Because of this, we set the add flag FALSE
			 * regardless of whether or not info for all protocols
			 * was filled in. This way, we won't try to
			 * insert addresses into the second packet.
			 */
			add_tlv[tlv_code] = FALSE;
			item_length = GETSHORT(&item->length);
			if (item_length) {
			    item = (cdp_info_type *)((char *)item +
						     item_length);
			}
			break;
		    }
		    /*
		     * Allow only one more attempt to add info to buffer.
		     * This will prevent infinite loops if we just can't
		     * allocate enough space.
		     */
		    if (!second_try)
			second_try = TRUE;
		    else {
			cdp_evbuginf("\nCDP-EV: Second attempt to add %s"
				     " info failed (%d free) - aborting",
				     cdp_tlv_namestring(tlv_code), bytes_left);
			add_tlv[tlv_code] = FALSE;
			second_try = FALSE;
			break;
		    }
		    cdp_evbuginf("\nCDP-EV: No space (%d) for insertion of %s"
                                 " information. Creating new fragment",
				 bytes_left,
				 cdp_tlv_namestring(tlv_code));
		    break;
		}
		item_length = GETSHORT(&item->length);
		if (item_length) {
		    item = (cdp_info_type *)((char *)item + item_length);
	        }
	    }
	} /* inner while */

	pak_length = (char *)item - (char *)cdpheadstart(pak);
	
	if (pak_length) {
	    pak->if_output = idb;
	    pak->linktype = LINK_CDP;
	    pak->acct_proto = ACCT_PROTO_CDP;

	    /*
	     * Fill in CDP checksum.
	     */
	    cdphdr->checksum = 0;
	    cdphdr->checksum = cdp_checksum((ushort *)cdphdr, pak_length);
    
	    /*
	     * Send packet out. NOTE...since we allocated a buffer whose size
	     * is = interface MTU, we don't need to worry about fragmentation.
	     */
	    if (pak_encap(idb->hwptr, pak, pak_length, 0)) {
		cdp_pabuginf("\nCDP-PA: Packet sent out on %s",
			     idb->namestring);
		if (more_frags)
		    cdp_traffic[CDP_FRAG]++;
		cdp_traffic[CDP_OUTPUT]++;
		datagram_out(pak);
	    }
	    else {
		cdp_evbuginf("\nCDP-EV: Encapsulation on interface %s failed",
			     idb->namestring);
		cdp_traffic[CDP_ENCAPSFAILED]++;
		datagram_done(pak);
		break;
	    }
	    
	    /*
	     * If we are done sending all information, just get outta here
	     */
	    if (!cdp_tlv_to_send(add_tlv)) {
		break;
	    } else {
		/*
		 * We are here because we need to send information in a
		 * second packet. Set the 'more_frags' to TRUE if it has
		 * not been set previously.
		 */
		if (!more_frags)
		    more_frags = TRUE;
	    }
	} else {
	    /*
	     * We have nothing to send...free the packet buffer if
	     * there was one. Actually, we should not even come here...
	     */
	    if (pak)
		retbuffer(pak);

	    break;
	}
    } /* outer while */
    
    return;
}

/*
 * cdp_insert_device_name
 *
 * Insert device name information into the item. If item was successfully
 * added, return TRUE. If there is no space to add this information, return
 * FALSE.
 */
boolean cdp_insert_device_name (cdp_info_type *item, idbtype *swidb,
				ushort *bytes_left)
{
    char *src, *dst;
    ushort bytes_needed;

    /*
     * Check if there is enough space before filling in input buffer.
     */
    bytes_needed = CDP_TLV_OVERHEAD + strlen(hostname) + 
	           ((default_domain) ? strlen(default_domain) : 0);

    if (*bytes_left < bytes_needed)
	return(FALSE);

    src = hostname;

    /*
     * Copy host name first
     */
    bcopy(src, item->value, strlen(hostname));
    PUTSHORT(&item->length, strlen(hostname));

    if (default_domain) {
	src = &item->value[strlen(hostname)];
        /*
         * Insert the  '.' before domain name
         */
        *src++ = '.';
        PUTSHORT(&item->length, GETSHORT(&item->length) + 1);

	src = default_domain;
	dst = &item->value[GETSHORT(&item->length)];
	bcopy(src, dst, strlen(default_domain));
	PUTSHORT(&item->length, GETSHORT(&item->length)+
		 strlen(default_domain));
    }

    PUTSHORT(&item->code, CDP_DEVICE_NAME);
    PUTSHORT(&item->length, GETSHORT(&item->length)+ CDP_TLV_OVERHEAD);

    *bytes_left -= GETSHORT(&item->length);
    return (TRUE);
}

/*
 * cdp_insert_address
 *
 * Insert address information into the item. If item was successfully
 * added, return TRUE. If there is no space to add this information, return
 * FALSE. Since we need to fill in addresses from various protocols, we
 * call routines for each protocol. If any of these routines returns
 * FALSE, we return a FALSE, but we set item type, code and length (if
 * length is non-zero), so that all protocols for which we have successfully
 * added info will be advertised in the outgoing packet. We send only one
 * packetful of address info; this makes it easy at the receiving end, since
 * we just need to overwrite the address info buffer, instead of having to
 * deal with address info arriving in multiple fragments.
 * 
 * The first long word in the 'value' field is the total number of addresses
 * that have been configured on this box. For the general packet format,
 * please see 'cdp.h'.
 */
boolean cdp_insert_address (cdp_info_type *item, idbtype *idb,
			    ushort *bytes_left)
{
    int num_addrs, len;
    uchar *temp;
    boolean status;

    /*
     * Check if there is enough space before filling in input buffer..the
     * first 4 bytes are needed to store the number of addresses.
     */
    if (*bytes_left < CDP_TLV_OVERHEAD + CDP_ADDRESS_OVERHEAD) {
	PUTSHORT(&item->length, 0);
	return(FALSE);
    }

    /*
     * Leave first 4 bytes in value part of TLV to contain the number of
     * addresses
     */
    temp = (char *)item->value + CDP_ADDRESS_OVERHEAD;
    len = CDP_TLV_OVERHEAD + CDP_ADDRESS_OVERHEAD;
    num_addrs = 0;

    if (idb->ip_address) {
        status = cdp_insert_ip_info(&temp, idb, &len, &num_addrs, bytes_left);
        if (!status) {
	    cdp_evbuginf("\nCDP-EV: No space for insertion of IP address "
                         "information. ");
	    PUTSHORT(&item->length, 0);
	    return(FALSE);
        }
    }

    status = cdp_insert_novell_info(&temp, idb, &len, &num_addrs, bytes_left);
    if (!status) {
	/*
         * NOTE!! If we came to this point, it means that we successfully
         * filled in the information for the previous protocol
         * (IP, in this case), so we need to fill in all relevant
         * fields before returning. This way, we will send out all
         * information about previous protocol(s) which we filled in
         * successfully.
         */
        cdp_evbuginf("\nCDP-EV: No space for insertion of Novell address "
                     "information.  ");
        PUTLONG(item->value, num_addrs);
        PUTSHORT(&item->code, CDP_ADDRESS_TYPE);
        PUTSHORT(&item->length, len);
	return(FALSE);
    }

    status = cdp_insert_clns_info(&temp, idb, &len, &num_addrs, bytes_left);
    if (!status) {
        /*
         * Same comments as for Novell...
         */
        cdp_evbuginf("\nCDP-EV: No space for insertion of CLNS address "
                     "information.  ");
        PUTLONG(item->value, num_addrs);
        PUTSHORT(&item->code, CDP_ADDRESS_TYPE);
        PUTSHORT(&item->length, len);
	return(FALSE);
    }

   status = cdp_insert_decnet_info(&temp, idb, &len, &num_addrs, bytes_left);
   if (!status) {
        /*
         * Same comments as for Novell...
         */
        cdp_evbuginf("\nCDP-EV: No space for insertion of DECnet address "
                     "information. ");
        PUTLONG(item->value, num_addrs);
        PUTSHORT(&item->code, CDP_ADDRESS_TYPE);
        PUTSHORT(&item->length, len);
        return(FALSE);
    }

   status = cdp_insert_apple_info(&temp, idb, &len, &num_addrs, bytes_left);
   if (!status) {
        /*
         * Same comments as for Novell...
         */
        cdp_evbuginf("\nCDP-EV: No space for insertion of Appletalk address "
                     "information. ");
        PUTLONG(item->value, num_addrs);
        PUTSHORT(&item->code, CDP_ADDRESS_TYPE);
        PUTSHORT(&item->length, len);
        return(FALSE);
    }

    /*
     * Fill in number of addresses
     */
    PUTLONG(item->value, num_addrs);
    PUTSHORT(&item->code, CDP_ADDRESS_TYPE);
    PUTSHORT(&item->length, len);

    return (TRUE);
}

/*
 * cdp_insert_ip_info
 *
 * Insert IP info into the item. If there is no space to add the IP address
 * information, return FALSE. 
 * 
 * Also, if packet was successfully filled in with the information, we
 * update the input buffer pointer to point past the end of the item.
 */
boolean cdp_insert_ip_info (uchar **buf, idbtype *idb, int *len, int *num_addrs,
			    ushort *bytes_left)
{
    uchar *temp;

    temp = *buf;

    /*
     * Check if there is enough space before filling in input buffer..
     */
    if (*bytes_left < (CDP_NLPID_INFO_OVERHEAD + 2 + ADDRLEN_IP)) {
        return(FALSE);
    }

    *temp++ = CDP_NLPID_FORMAT;
    *temp++ = CDP_NLPID_LEN;
    *temp++ = CDP_IP_NLPID;
    PUTSHORT(temp, ADDRLEN_IP);
    temp += 2;
    (*len) += (CDP_NLPID_INFO_OVERHEAD + 2);

    /* insert IP address */
    PUTLONG(temp, GETLONG(&idb->ip_address));
    temp += ADDRLEN_IP;
    (*len) += ADDRLEN_IP;

    (*bytes_left) -= (CDP_NLPID_INFO_OVERHEAD + 2 + ADDRLEN_IP);
    (*num_addrs)++;

    /*
     * Move 'buf' past the item we have just filled.
     */

    *buf = temp;
    return(TRUE);
}

/*
 * cdp_insert_clns_info
 *
 * Insert CLNS info into the item. Same behavior as the IP routine above.
 */
boolean cdp_insert_clns_info (uchar **buf, idbtype *idb, int *len,
			      int *num_addrs, ushort *bytes_left)
{
    uchar nsap_len, *temp;
    addrtype addr;

    if (!RUNNING(LINK_CLNS))
	return (TRUE);

    reg_invoke_proto_address(LINK_CLNS, idb, NULL, &addr);
    if (addr.length != 0) {
	temp = *buf;
	
	/* nsap_len will include length of NSAP length byte */
	nsap_len = addr.length;

	/*
         * Check if there is enough space before filling in input buffer..
         */
        if (*bytes_left < (CDP_NLPID_INFO_OVERHEAD + 2 + nsap_len)) {
	    return(FALSE);
	}

	*temp++ = CDP_NLPID_FORMAT;
	*temp++ = CDP_NLPID_LEN;
	*temp++ = CDP_CLNS_NLPID;
	PUTSHORT(temp, nsap_len);
	temp += 2;
        (*len) += (CDP_NLPID_INFO_OVERHEAD + 2);

        /* insert CLNS NSAP */
        bcopy(addr.clns_addr, temp, nsap_len);
	temp += nsap_len;
	(*len) += nsap_len;

	(*bytes_left) -= (CDP_NLPID_INFO_OVERHEAD + 2 + nsap_len);
	(*num_addrs)++;

	/*
	 * Move 'buf' past the item we have just filled.
	 */
	*buf = temp;
    }

    return(TRUE);
}

/*
 * cdp_insert_novell_info
 *
 * Insert Novell info into the item. Return TRUE if we successfully
 * entered the Novell info; else return FALSE.
 */
boolean cdp_insert_novell_info (uchar **buf, idbtype *idb, int *len,
				int *num_addrs, ushort *bytes_left)
{
    uchar *temp;
    snap_hdr *snap;
    addrtype addr;

    if (!RUNNING(LINK_NOVELL))
	return (TRUE);

    /*
     * Check if there is enough space before filling in input buffer..
     */
    if (*bytes_left < (CDP_8022_INFO_OVERHEAD + 2 + ADDRLEN_NOVELL)) {
        return(FALSE);
    }
    
    temp = *buf;

    reg_invoke_proto_address(LINK_NOVELL, idb, NULL, &addr);
    if (addr.length != 0) {
	*temp++ = CDP_8022_FORMAT;
        *temp++ = CDP_8022_FORMAT_LEN;

        /* Use SNAP formats */
        snap = (snap_hdr *)temp;
        snap->dsap = SAP_SNAP;
        snap->ssap = SAP_SNAP;
        snap->control = LLC1_CLASS;
        bcopy(zerosnap, snap->oui, SNAP_OUIBYTES);
        PUTSHORT(&snap->type, TYPE_NOVELL1);
        temp += SNAP_HDRBYTES;
        PUTSHORT(temp, ADDRLEN_NOVELL);
        temp += 2;	
        (*len) += (CDP_8022_INFO_OVERHEAD + 2);

        /* insert Novell address */

        bcopy((uchar *)&addr.nov_addr, temp, ADDRLEN_NOVELL);
        temp += ADDRLEN_NOVELL;
        (*len) += ADDRLEN_NOVELL;
	    
        (*bytes_left) -= (CDP_8022_INFO_OVERHEAD + 2 + ADDRLEN_NOVELL);
        (*num_addrs)++;

	/*
	 * Move 'buf' past the item we have just filled.
	 */
	*buf = temp;
    }

    return(TRUE);
}

/*
 * cdp_insert_decnet_info
 *
 * Insert DECnet info into the item. Return TRUE if we successfully
 * entered the DECnet info; else return FALSE.
 */
boolean cdp_insert_decnet_info (uchar **buf, idbtype *idb, int *len,
				int *num_addrs, ushort *bytes_left)
{
    uchar *temp;
    addrtype addr;
    snap_hdr *snap;

    temp = *buf;

    if (!RUNNING(LINK_DECNET))
	return (TRUE);

    /*
     * Check if there is enough space before filling in input buffer..
     */
    if (*bytes_left < (CDP_8022_INFO_OVERHEAD + 2 + ADDRLEN_DECNET)) {
        return(FALSE);
    }

    reg_invoke_proto_address(LINK_DECNET, idb, NULL, &addr);
    if (addr.length != 0) {
	temp = *buf;
	*temp++ = CDP_8022_FORMAT;
        *temp++ = CDP_8022_FORMAT_LEN;

	/* Use SNAP formats */
	snap = (snap_hdr *)temp;
	snap->dsap = SAP_SNAP;
	snap->ssap = SAP_SNAP;
	snap->control = LLC1_CLASS;
	bcopy(zerosnap, snap->oui, SNAP_OUIBYTES);
	PUTSHORT(&snap->type, TYPE_DECNET);
	temp += SNAP_HDRBYTES;
        PUTSHORT(temp, ADDRLEN_DECNET);
        temp += 2;	
        (*len) += (CDP_8022_INFO_OVERHEAD + 2);

	/* insert DECnet address, swap it as it is returned as swapped by
	   decnet_build_address routine */
	/* XXX this is 68k dependant and broken */
	PUTSHORT(temp, swap_16bit_word(addr.decnet_addr));
	temp += ADDRLEN_DECNET;
        (*len) += ADDRLEN_DECNET;

        (*bytes_left) -= (CDP_8022_INFO_OVERHEAD + 2 + ADDRLEN_DECNET);
        (*num_addrs)++;

	/*
	 * Move 'buf' past the item we have just filled.
	 */
	*buf = temp;
    }

    return(TRUE);
}

/*
 * cdp_insert_apple_info
 *
 * Insert Appletalk info into the item. Return TRUE if we successfully
 * entered the Appletalk info; else return FALSE.
 */
boolean cdp_insert_apple_info (uchar **buf, idbtype *idb, int *len,
			       int *num_addrs, ushort *bytes_left)
{
    uchar *temp;
    addrtype addr;
    snap_hdr *snap;

    temp = *buf;

    if (!RUNNING_IF(LINK_APPLETALK, idb))
	return (TRUE);

    /*
     * Check if there is enough space before filling in input buffer..
     */
    if (*bytes_left < (CDP_8022_INFO_OVERHEAD + 2 + ADDRLEN_APPLE)) {
        return(FALSE);
    }

    reg_invoke_proto_address(LINK_APPLETALK, idb, NULL, &addr);
    if (addr.length != 0) {
	temp = *buf;
	*temp++ = CDP_8022_FORMAT;
        *temp++ = CDP_8022_FORMAT_LEN;

	/* Use SNAP formats */
	snap = (snap_hdr *)temp;
	snap->dsap = SAP_SNAP;
	snap->ssap = SAP_SNAP;
	snap->control = LLC1_CLASS;
	bcopy(zerosnap, snap->oui, SNAP_OUIBYTES);
	PUTSHORT(&snap->type, TYPE_ETHERTALK);
	temp += SNAP_HDRBYTES;
        PUTSHORT(temp, ADDRLEN_APPLE);
        temp += 2;	
        (*len) += (CDP_8022_INFO_OVERHEAD + 2);

	/* insert Appletalk address */
	bcopy((uchar *)&addr.apple_addr, temp, ADDRLEN_APPLE);
	temp += ADDRLEN_APPLE;
        (*len) += ADDRLEN_APPLE;

        (*bytes_left) -= (CDP_8022_INFO_OVERHEAD + 2 + ADDRLEN_APPLE);
        (*num_addrs)++;

	/*
	 * Move 'buf' past the item we have just filled.
	 */
	*buf = temp;
    }

    return(TRUE);
}

/*
 * cdp_insert_port_id
 *
 * Insert information on outgoing port information into the item. Return
 * TRUE if we successfully entered the information; else return FALSE.
 */
boolean cdp_insert_port_id (cdp_info_type *item, idbtype *idb,
			    ushort *bytes_left)
{
    char *str;
    ushort string_len;

    /*
     * Allow maximum of CDP_MAX_PORT_ID_STR_LEN bytes for port ID info
     */

    str = idb->namestring;
    string_len = strlen(str);

    string_len = (string_len >= CDP_MAX_PORT_ID_STR_LEN ?
	          (CDP_MAX_PORT_ID_STR_LEN) : string_len);
    
    /*
     * Check if there is enough space before filling in input buffer..
     */
    if (*bytes_left < CDP_TLV_OVERHEAD + string_len)
	return(FALSE);


    PUTSHORT(&item->code, CDP_PORT_ID_TYPE);
    bcopy(str, item->value, string_len);

    PUTSHORT(&item->length, CDP_TLV_OVERHEAD + string_len);

    (*bytes_left) -= GETSHORT(&item->length);

    return(TRUE);
}

/*
 * cdp_insert_capabilities
 *
 * Insert capabilities information into the item. Return TRUE if we
 * successfully entered the information; else return FALSE.
 */
boolean cdp_insert_capabilities (cdp_info_type *item, idbtype *idb,
				 ushort *bytes_left)
{
    boolean bridging_enabled;
    hwidbtype *hwidb;
    ulong value;

    /*
     * Check if there is enough space before filling in input buffer..
     *
     */
    if (*bytes_left < CDP_TLV_OVERHEAD + CDP_CAPAB_FIELD_LEN)
	return(FALSE);

    value = 0L;
    bridging_enabled = FALSE;

    if (RUNNING(LINK_IP) || RUNNING(LINK_NOVELL) || RUNNING(LINK_CLNS) ||
	RUNNING(LINK_XNS) || RUNNING(LINK_APPLETALK) || RUNNING(LINK_APOLLO) ||
	RUNNING(LINK_VINES) || RUNNING(LINK_DECNET)) {
	value |= CDP_ROUTER_DEVICE;
    }
    FOR_ALL_HWIDBS(hwidb) {
	if (hwidb->tbridge_on_hwidb) {
	    bridging_enabled = TRUE;
	    break;
	}
    }
    if (bridging_enabled)
	value |= CDP_TB_DEVICE;
    if (reg_invoke_srb_enabled())
	value |= CDP_SRB_DEVICE;

    PUTSHORT(&item->code, CDP_CAPABILITIES_TYPE);
    PUTLONG(item->value, value);
    PUTSHORT(&item->length, CDP_TLV_OVERHEAD + CDP_CAPAB_FIELD_LEN);
    *bytes_left -= GETSHORT(&item->length);

    return(TRUE);
}

/*
 * cdp_insert_version
 *
 * Insert version information into the item. Return TRUE if we
 * successfully entered the information; else return FALSE.
 */
boolean cdp_insert_version (cdp_info_type *item, idbtype *idb,
			    ushort *bytes_left)
{
    char *temp;
    uint len;

    len = strlen(version);

    /*
     * Check if there is enough space before filling in input buffer
     */
    if (*bytes_left < CDP_TLV_OVERHEAD + len)
	return(FALSE);

    temp = version;

    bcopy(temp, item->value, len);

    PUTSHORT(&item->code, CDP_VERSION_TYPE);
    
    PUTSHORT(&item->length, CDP_TLV_OVERHEAD + len);
    *bytes_left -= GETSHORT(&item->length);

    return(TRUE);
}

/*
 * cdp_init_platform_strings
 *
 * Intialize strings used to describe hardware
 */

void cdp_init_platform_strings (void)
{
    int len;
    char *str, *platform;

    /*
     * Set up the platform string
     */
    platform = cdp_platform_string;
    str = platform_get_string(PLATFORM_STRING_VENDOR);
    len = min(strlen(str), CDP_MAX_PLATFORM_STR_LEN);
    strncpy(platform, str, len);
    platform += len;
    if (len < CDP_MAX_PLATFORM_STR_LEN) {
	*platform++ = ' ';
	len++;
	str = platform_get_string(PLATFORM_STRING_NOM_DU_JOUR);
	len = min(strlen(str), CDP_MAX_PLATFORM_STR_LEN - len);
	if (len > 0) {
	    strncpy(platform, str, len);
	    platform += len;
	}
    }
    *platform = '\0';
    cdp_platform_string_size = (int) (platform - cdp_platform_string);
}


/*
 * cdp_insert_hardware
 *
 * Insert hardware information into the item. Return TRUE if we
 * successfully entered the information; else return FALSE.
 */
boolean cdp_insert_hardware (cdp_info_type *item, idbtype *idb,
			     ushort *bytes_left)
{
    int size;

    size = CDP_TLV_OVERHEAD + cdp_platform_string_size;

    /*
     * Check if there is enough space before filling in input buffer..
     */
    if (*bytes_left < size)
	return(FALSE);


    PUTSHORT(&item->code, CDP_PLATFORM_TYPE);
    bcopy(cdp_platform_string, item->value, cdp_platform_string_size);

    PUTSHORT(&item->length, size);

    (*bytes_left) -= GETSHORT(&item->length);

    return(TRUE);
}

/*
 * cdp_reload_notify
 *
 * Notify all interfaces that we are going down by sending out a
 * packet with TTL 0.
 */
void cdp_reload_notify (void)
{
    idbtype *idb;

    FOR_ALL_SWIDBS(idb) {
	if (!cdp_broadcast_if(idb))
	    continue;
	cdp_send_update(idb, 0);
    }
}

/*
 * cdp_if_down_notify
 *
 * Notify neighbors on this interfaces when the interface goes down. This is
 * done by sending a packet with TTL of 0.
 */
void cdp_if_down_notify (idbtype *idb)
{
    idb = get_netcfg_idb(idb);
    if (idb->cdp_enabled) {
	cdp_adjbuginf("\nCDP-AD: Interface %s going down", idb->namestring);
        cdp_send_update(idb, 0);
    }
}


/*
 * cdp_if_up_notify
 *
 * Notify neighbors on this interfaces when the interface comes up. This is
 * done by sending a packet with TTL of non-zero.
 */
void cdp_if_up_notify (idbtype *idb)
{
    idbtype *tmpidb;

    idb = get_netcfg_idb(idb);
    if (idb->cdp_enabled) {
	cdp_adjbuginf("\nCDP-AD: Interface %s coming up", idb->namestring);

	/*
	 * If any ISL configured on the hwidb, make sure this is the
	 * lowest ISL number configured.
	 */
	if (idb->hwptr->isl_vlan_on_hwidb) {
	    tmpidb = reg_invoke_isl_vlan_lowest_colour(idb->hwptr);
	    if (idb != tmpidb)	/* We already send one w/ the lowest ISL # */
		return;
	}
        cdp_send_update(idb, cdp_holdtime);
    }
}

/*
 * cdp_reset
 *
 * Called whenever encapsulation changed or p2p<->p2mp changed.
 */

static void cdp_reset (hwidbtype *hwidb, idbtype *swidb)
{
    idbtype *idb;

    if (swidb) {
	if (CDP_DEFAULT_ENABLE_INT(swidb)) {
	    cdp_intf_enable(swidb, FALSE);
	} else {
	    cdp_intf_disable(swidb);
	}
    } else {
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (CDP_DEFAULT_ENABLE_INT(idb)) {
		cdp_intf_enable(idb, FALSE);
	    } else {
		cdp_intf_disable(idb);
	    }
	}
    }
}

/*
 * cdp_add_registry_calls
 *
 * Do all registry-related stuff here. CDP notification functions get
 * registered upon process startup (and correspondingly deleted upon
 * process shutdown).
 */
void cdp_add_registry_calls (void)
{
    reg_add_proto_running(LINK_CDP, cdp_running_test, "cdp_running_test");
    reg_add_proto_on_swidb(LINK_CDP, cdp_on_idb, "cdp_on_idb");
    reg_add_sw_idb_init(cdp_init_idb, "cdp_init_idb");
    reg_add_cdp_reset(cdp_reset, "cdp_reset");

    /*
     * Create our dynamic registry
     */
    cdp_regcode = registry_create(REG_UNKNOWN, SERVICE_CDP_MAX, "CDP");
    if (cdp_regcode != REG_ILLEGAL)
	create_registry_cdp();
}

