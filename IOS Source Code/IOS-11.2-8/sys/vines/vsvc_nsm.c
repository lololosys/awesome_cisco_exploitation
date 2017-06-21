/* $Id: vsvc_nsm.c,v 3.5.12.1 1996/03/18 22:32:56 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/vines/vsvc_nsm.c,v $
 *------------------------------------------------------------------
 * vsvc_nsm.c - Support for the `VINES Network and Systems 
 *				 Management' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_nsm.c,v $
 * Revision 3.5.12.1  1996/03/18  22:32:56  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.5  1996/03/14  01:36:57  gstovall
 * Branch: DeadKingOnAThrone_branch
 * The King is now ready for action.
 *
 * Revision 3.4.12.4  1996/03/13  02:05:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.4.12.3  1996/03/07  11:05:38  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.2  1996/02/20  21:47:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/12  18:18:48  abaindur
 * CSCdi48693:  show process mem does not include alternate mempool mallocs
 *
 * Revision 3.4.12.1  1996/01/24  23:30:08  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.4  1995/12/20  23:46:51  gstovall
 * CSCdi45909:  Item->handled field tested while uninitialized
 * Make sure that item->handled is initialized.  Bad programmer, no pizza.
 *
 * Revision 3.3  1995/11/17  17:59:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:48:31  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:46:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/07/27  07:31:15  hampton
 * Rename insque/remque to lw_insert/lw_remove. [CSCdi37745]
 *
 * Revision 2.3  1995/06/23 18:58:58  slin
 * CSCdi36177:  vines doesnt check malloc return code
 *
 * Revision 2.2  1995/06/15  18:25:16  slin
 * CSCdi35901:  Memory leak in Vines Network and Systems Management
 * - Free memory after sending summary statistics messages.
 *
 * Revision 2.1  1995/06/07  23:16:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *
 * The "Network and System Management" Service is the network
 * administrator's window into what is happening in a VINES network or
 * on a VINES server.  It can provide detailed configuration
 * information for a server, or it can provide dynamic information
 * captured in real time.  This information may be either information
 * about the server itself, information about the network topology and
 * routing tables for any of the supported network layer protocols, etc.
 *
 * In general, anything labelled "info" is a request for static
 * information, and anthing labelled "stats" is a request for dynamic
 * information.
 **********************************************************************/

#include "master.h"
#include <string.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "config.h"
#include "parser.h"
#include "ieee.h"
#include "../if/network.h"
#include "../h/media_registry.h"
#include "vines_private.h"
#include "vines_ipc.h"
#include "vines_netrpc.h"
#include "vines_services.h"

#include "vsvc_nsm.h"
#include "vsvc_nsm.rpch"
#include "vsvc_nsm.rpcc"
#include "vsvc_ss.h"

/*
 * The following are included so that the Network Management code can extract
 * information from other parts of the router. 
 */
#include "../if/fddi.h"
#include "../os/free.h"
#include "../os/region.h"
#include "../if/tring.h"
#include "../util/itemlist.h"

/*
 * Local variables
 */
static queuetype vnsm_traceQ;
static dqueue_t  vnsm_send_list;
static dqueue_t  vnsm_pstk_passQ;


/**************************************************
 *
 *	NSM - Routines Common Across Services
 *
 **************************************************/

/*
 * vnsm_insert_label
 *
 * Used by routines that can return TAGS or DATA in the same byte field.
 * This routine inserts a single field label and descriptions into the
 * message.
 */
static uchar *vnsm_insert_label (
    uchar *data,
    char *name,
    ushort data_type,
    ushort unknown,
    ushort length)
{
    data = vines_insert_string(data, name, VNSM_LEN_LABELMAX);
    data = vines_insert_short(data, data_type);
    data = vines_insert_short(data, unknown);
    data = vines_insert_short(data, length);
    return(data);
}

/*
 * vnsm_insert_rawdata
 *
 * Used by routines that can return TAGS or DATA in the same byte field.
 * This routine inserts a single data field into the message.  The type and
 * length of this data field was described in a previous TAGS message.
 */
static uchar *vnsm_insert_rawdata (
    uchar *data,
    ulong value,
    ushort length)
{
    switch (length) {
      case 1:  data = vines_insert_uchar(data, value);  break;
      case 2:  data = vines_insert_ushort(data, value); break;
      case 4:  data = vines_insert_ulong(data, value);  break;
      default: data += length; break;
    }
    return(data);
}

/*
 * vnsm_insert_rawdata
 *
 * Used by routines that can return TAGS or DATA in the same byte field.
 * This routine inserts a single MAC Address data field into the message.
 * The type and length of this data field was described in a previous TAGS
 * message. 
 */
static uchar *vnsm_insert_mac (
    uchar *data,
    uchar const *address)
{
    ieee_copy(address, data);
    return(data+IEEEBYTES);
}

/*
 * vnsm_insert_fmtdata
 *
 * This routine inserts a single self-defining data field into the message.
 */
static uchar *vnsm_insert_fmtddata (
    uchar *data,
    ushort data_type,
    ushort semantics,
    ushort length,
    ulong  value)
{
    data = vines_insert_short(data, data_type);
    data = vines_insert_short(data, semantics);
    data = vines_insert_short(data, length);
    switch (length) {
      case 1:  data = vines_insert_uchar(data, value);  break;
      case 2:  data = vines_insert_ushort(data, value); break;
      case 4:  data = vines_insert_ulong(data, value);  break;
      default: data += length; break;
    }
    return(data);
}

static inline uchar *vnsm_insert_counter16 (uchar *data, ushort value)
{
    return(vnsm_insert_fmtddata(data, VNSM_NMA_COUNTER16, 0, 2, value));
}

static inline uchar *vnsm_insert_counter32 (uchar *data, ulong value)
{
    return(vnsm_insert_fmtddata(data, VNSM_NMA_COUNTER32, 0, 4, value));
}

static inline uchar *vnsm_insert_ticks32 (uchar *data, ulong value)
{
    return(vnsm_insert_fmtddata(data, VNSM_NMA_TICKS32, 0, 4, value));
}

/*
 * vnsm_insert_fmtdmac
 *
 * This routine inserts a single self-defining MAC Address data field into
 * the message.
 */
static uchar *vnsm_insert_fmtdmac (
    uchar *data,
    uchar *address)
{
    data = vines_insert_short(data, VNSM_NMA_LLADDRESS);
    data = vines_insert_short(data, 0);
    data = vines_insert_short(data, IEEEBYTES);
    ieee_copy(address, data);
    return(data+IEEEBYTES);
}


/**************************************************
 *
 *              NSM - Miscellaneous
 *
 **************************************************/


/**************************************************
 *
 *         NSM - Get Server Info Summary (0)
 *
 **************************************************/

/*
 * vnsm_getsvrinfosumm_rcv_call
 *
 * Get general information about this server.  General means name, key
 * number, and version.
 */
ushort vnsm_getsvrinfosumm_rcv_call (
    vinesipcsocket *socket,
    vnsm_getsvrinfosumm_query *query,
    vnsm_getsvrinfosumm_reply *reply)
{
    ushort invalid;
    char version_string[VNSM_LEN_OSREVNAME];

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (invalid)
	return(invalid);

    /*
     * Build the reply packet, and we're done
     */
    reply->version      = VNSM_VERSION_1;
    vsvc_insert_hostname(reply->servername);
    reply->unknown1     = 0;
    reply->serial       = vines_network;
    reply->dupserial    = vines_network;
    reply->product_type = 0;
    reply->unknown2     = 0;
    sstrncpy(reply->product_name, "cisco", VNSM_LEN_PRODUCTNAME);
    reply->unknown3     = 0;
    reply->unknown4     = 0;
    reply->os_rev       = vsvc_nsm_info.version;
    sstrncpy(reply->os_rev_name, vsvc_nsm_info.version_string, VNSM_LEN_OSREVNAME);
    reply->os_type      = 0;
    sprintf(version_string, "IOS %u.%u", SYS_MAJORVERSION, SYS_MINORVERSION);
    sstrncpy(reply->os_type_string, version_string, VNSM_LEN_OSTYPENAME);
    reply->unknown5     = 0;
    reply->unknown6     = 0;
    reply->start_time   = vss_get_start_time();
    reply->real_memory  = region_get_size_by_class(REGION_CLASS_LOCAL) / 1024;
    reply->current_time = vss_get_time();
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *             NSM - Get Server Info (1)
 *
 **************************************************/

/*
 * vnsm_getsvrinfo_add_comminfo
 *
 * Get the static "comms" configuration information about this server.
 * The only response to this should be VINES/IP.  Banyan is migrating
 * their tools to be SNMP based, and it is silly to add support for
 * DOD IP, Appletalk, IPX, etc. to this section when it is already
 * available using SNMP.
 */
static void vnsm_getsvrinfo_add_comminfo (
    vnsm_getsvrinfo_reply *reply)
{
    vnsm_svrcomminfo *info;
    vnsm_prot_stack *stack;
    vnsm_protocol *protocol;

    info = &reply->info.comm.data;

    info->totalcommbuff = 0;
    info->socketscfg = 0;
    info->prot_stack_count = 1;

    stack = &info->prot_stack[0];
    stack->type = VNSM_FAMILY_VINES;
    sstrncpy(stack->name, "VINES", VNSM_LEN_STACKNAME);
    stack->protocol_count = 2;

    protocol = &stack->protocol[0];
    protocol->type = VNSM_PROT_VINESIP;
    sstrncpy(protocol->name, "IP", VNSM_LEN_PROTOCOLNAME);

    protocol = &stack->protocol[1];
    protocol->type = VNSM_PROT_VINESRTP;
    sstrncpy(protocol->name, "RTP", VNSM_LEN_PROTOCOLNAME);
}

/*
 * vnsm_getsvrinfo_rcv_call
 *
 * Get specific static configuration information about this server.
 * The query specifies which information should be returned.
 */
ushort vnsm_getsvrinfo_rcv_call (
    vinesipcsocket *socket,
    vnsm_getsvrinfo_query *query,
    vnsm_getsvrinfo_reply *reply)
{
    vnsm_svrosinfo *os;
    vnsm_osinfo *osinfo;
    ushort invalid;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (invalid)
	return(invalid);

    /*
     * Build the reply packet, and we're done
     */
    reply->version   = VNSM_VERSION_1;
    reply->info_type = query->info_type;
    switch (reply->info_type) {
      case VNSM_SVR_FSINFO:
	/* Leave this as all zeros since we don't have a file system. */
	break;
      case VNSM_SVR_OSINFO:
	os = &reply->info.os.data;
	os->osinfo_type = VNSM_SVR_OS_UNIX;
	osinfo = &os->osinfo;
	osinfo->unix_type.data.processmemsize =
	    mempool_get_total_bytes(MEMPOOL_CLASS_LOCAL,
				    MEMPOOL_ALTERNATE_INCLUDE) / 1024;
	/* Leave the rest as all zeros. */
	break;
      case VNSM_SVR_COMMINFO:
	vnsm_getsvrinfo_add_comminfo(reply);
	break;
      case VNSM_SVR_CPUINFO:
	reply->info.cpu.data.cpus = 1;
	break;
      default:
	if (vnsm_debug)
	    buginf("\nVNSM:  request for unknown info type %d", query->info_type);
	return(VNSM_ERR_INVPARM);
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *            NSM - Get Server Stats (4)
 *
 **************************************************/

/*
 * vnsm_getsvrstat_rcv_call
 *
 * Get dynamic runtime summary statistics from this server.
 */
static void vnsm_getsvrstat_enter_summstats (vnsm_svrsummstats2 *stats)
{
    vinesidbtype *idb;
    hwidbtype *hwidb;
    ulong framesin, framesout, drops;

    framesin = framesout = drops = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb == vinesnullidb)
	    continue;
	hwidb = idb->phys_idb->hwptr;
	framesin += vines_frames_input(hwidb);
	framesout += vines_frames_output(hwidb);
	drops += hwidb->counters.input_drops + hwidb->counters.output_total_drops;
    }

    stats->timestamp	 = 0;	/* not used, see next routine */
    stats->loadavg1      = last_busy_percent;
    stats->loadavg5      = avg_busy1_percent;
    stats->loadavg15     = avg_busy5_percent;
    stats->msgavg1       = 0;
    stats->msgavg5       = 0;
    stats->msgavg15      = 0;
    stats->swapavg15     = 0;
    stats->pctcachehits  = 0;
    stats->commbuffusage = 0;
    stats->msgin         = framesin;
    stats->msgout        = framesout;
    stats->drops         = drops;
    stats->pctusercpu	 = 0;
    stats->pctsyscpu	 = 0;
    stats->pctwiocpu	 = 0;
    stats->pctidlecpu	 = 0;
    stats->runqsz	 = 0;
    stats->pctrunocc	 = 0;
    stats->swapqsz	 = 0;
    stats->pctswapocc	 = 0;
    stats->bswpinrate	 = 0;
    stats->bswpoutrate	 = 0;
    stats->pswitchrate	 = 0;
    stats->syscallrate	 = 0;
    stats->pctrdcache	 = 0;
    stats->pctwrcache	 = 0;
    stats->brdrate	 = 0;
    stats->bwrrate	 = 0;
    stats->vfltsrate	 = 0;
    stats->freemem_left	 =
	mempool_get_free_bytes(MEMPOOL_CLASS_LOCAL,
			       MEMPOOL_ALTERNATE_INCLUDE) / 1024;
    stats->freemem_xxx   = 0;
    stats->freeswap	 = 0;
    stats->processes	 = 0;
    stats->inodes	 = 0;
    stats->files	 = 0;
}

/*
 * vnsm_getsvrstat_rcv_call
 *
 * Get dynamic runtime information about this server. The query
 * specifies which information should be returned.
 */
ushort vnsm_getsvrstat_rcv_call (
    vinesipcsocket *socket,
    vnsm_getsvrstat_query *query,
    vnsm_getsvrstat_reply *reply)
{
    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);

    /*
     * Build the reply packet, and we're done
     */
    reply->version   = VNSM_VERSION_1;
    reply->info_type = query->info_type;
    switch (reply->info_type) {
      case VNSM_SVR_SUMMSTAT:
	reply->info.summ.timestamp = vss_get_time();
	vnsm_getsvrstat_enter_summstats (&reply->info.summ.data);
	break;
      case VNSM_SVR_DISKSTAT:
	reply->info.disk.timestamp = vss_get_time();
	/* Leave this as all zero's. */
	break;
      case VNSM_SVR_FSSTAT:
	reply->info.fs.timestamp = vss_get_time();
	/* Leave the rest as all zero's. */
	break;
      default:
	if (vnsm_debug)
	    buginf("\nVNSM:  request for unknown info type %d", query->info_type);
	return(VNSM_ERR_INVPARM);
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *           NSM - Get Wan Information (5)
 *
 **************************************************/

/*
 * vnsm_getwaninfo_rcv_call
 *
 * Get general information about all WAN interfaces this server.  The
 * protocol is designed to only support 15 LAN interfaces.  Because of
 * this, the user must configure the interfaces for which they want to
 * ba able to retrieve statistics.  Once again, the router indicates
 * that it only supports the VINES protocol.  Any other protocol
 * information should be retrieved via SNMP.
 */
ushort vnsm_getwaninfo_rcv_call (
    vinesipcsocket *socket,
    vnsm_getwaninfo_query *query,
    vnsm_getwaninfo_reply *reply)
{
    vnsm_getwaninfo_ifinfo *ifinfo;
    vinesidbtype *idb;
    hwidbtype *hwidb;
    idbtype *swidb;
    ushort count;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);

    /*
     * Start the reply packet.
     */
    reply->version = VNSM_VERSION_1;

    count = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb->vines_metric == 0)
	    continue;
	if ((idb->svc_slot == 0) || (idb->svc_line == 0))
	    continue;
	if (!vines_is_wan(idb))
	    continue;
	swidb = idb->phys_idb;
	hwidb = swidb->hwptr;
	ifinfo = &reply->ifinfo[count];
	ifinfo->slot = idb->svc_slot;
	ifinfo->line = idb->svc_line;
	ifinfo->type = vsvc_c2b_wantype(idb);
	ifinfo->frames_rcvd = idb_inputs(hwidb);
	ifinfo->frames_xmit = idb_outputs(hwidb);
	ifinfo->frames_rexmit = 0;
	ifinfo->crc_errors = hwidb->counters.input_crc;
	ifinfo->drops = hwidb->counters.input_drops;
	ifinfo->underruns = hwidb->counters.runts;
	ifinfo->overruns = hwidb->counters.input_overrun;
	ifinfo->framing_errors = hwidb->counters.input_frame;
	if (++count >= VNSM_NUM_INTERFACES)
	    break;
    }
    reply->ifinfo_count = count;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *           NSM - Get X25 Information (6)
 *
 **************************************************/

static inline void ascii_to_banyan (
    char *ascii_num,
    uchar *banyan_num,
    int ascii_len)
{
    int i;

    for (i = 0; i < ascii_len; i++)
	banyan_num[i] = ascii_num[i] - '0';
}

/*
 * vnsm_getx25info_rcv_call
 *
 * Get general information about all X25 interfaces this server.  The
 * protocol is designed to only support 15 LAN interfaces.  Because of
 * this, the user must configure the interfaces for which they want to
 * ba able to retrieve statistics.  Once again, the router indicates
 * that it only supports the VINES protocol.  Any other protocol
 * information should be retrieved via SNMP.
 */
ushort vnsm_getx25info_rcv_call (
    vinesipcsocket *socket,
    vnsm_getx25info_query *query,
    vnsm_getx25info_reply *reply)
{
    vnsm_getx25info_ifinfo *ifinfo;
    vinesidbtype *idb;
    hwaddrtype hwaddr;
    hwidbtype *hwidb;
    idbtype *swidb;
    lcitype *lci;
    ushort count;
    boolean success;
    int vc;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);

    /*
     * Start the reply packet.
     */
    reply->version = VNSM_VERSION_1;

    count = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb->vines_metric == 0)
	    continue;
	if ((idb->svc_slot == 0) || (idb->svc_line == 0))
	    continue;
	swidb = idb->phys_idb;
	hwidb = swidb->hwptr;
	if (!is_x25(hwidb))
	    continue;
	vc = 0;
	while ((lci=reg_invoke_media_x25_get_next_lci(hwidb,vc,NULL)) != NULL) {
	    ifinfo = &reply->ifinfo[count];
	    ifinfo->slot = idb->svc_slot;
	    ifinfo->line = idb->svc_line;

	    success = reg_invoke_media_x25_get_lci_info(lci, &vc, &hwaddr,
							&ifinfo->pktsneedack);
	    if (!success)
		continue;
	    ifinfo->lsn = vc;
	    ifinfo->lcn = vc;
	    ifinfo->remx121sig = min(VNSM_MAX_X25LENGTH, hwaddr.length);
	    ascii_to_banyan(hwaddr.addr, ifinfo->remx121addr, ifinfo->remx121sig);

	    success = reg_invoke_media_x25_get_lci_stats(
		 lci, &ifinfo->outbytes, &ifinfo->inbytes, &ifinfo->outpkts,
		 &ifinfo->inpkts, &ifinfo->outresets, &ifinfo->inresets); 
	    if (!success)
		continue;

	    ifinfo->totalin = 0; /* not counted separately */
	    ifinfo->totalout = 0; /* not counted separately */
	    ifinfo->inerrors = 0; /* not counted separately */
	    ifinfo->outerrors = 0; /* not counted separately */
	    ifinfo->bytesneedack = 0; /* not counted separately */
	    if (++count >= VNSM_NUM_INTERFACES)
		break;
	}
    }
    reply->ifinfo_count = count;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *           NSM - Get Lan Information (7)
 *
 **************************************************/

/*
 * vnsm_getlaninfo_rcv_call
 *
 * Get general information about all LAN interfaces this server.  The
 * protocol is designed to only support 15 LAN interfaces.  Because of
 * this, the user must configure the interfaces for which they want to
 * ba able to retrieve statistics.  Once again, the router indicates
 * that it only supports the VINES protocol.  Any other protocol
 * information should be retrieved via SNMP.
 */
ushort vnsm_getlaninfo_rcv_call (
    vinesipcsocket *socket,
    vnsm_getlaninfo_query *query,
    vnsm_getlaninfo_reply *reply)
{
    vnsm_getlaninfo_ifinfo *ifinfo;
    vinesidbtype *idb;
    hwaddrtype hwaddr;
    hwidbtype *hwidb;
    idbtype *swidb;
    ushort count;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);

    /*
     * Start the reply packet.
     */
    reply->version = VNSM_VERSION_1;
    reply->timestamp = vss_get_time();

    count = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb->vines_metric == 0)
	    continue;
	if (idb->svc_slot == 0)
	    continue;
	if (!vines_is_lan(idb))
	    continue;
	swidb = idb->phys_idb;
	hwidb = swidb->hwptr;
	ifinfo = &reply->ifinfo[count];
	ifinfo->line_slot = idb->svc_slot;
	ifinfo->type = vsvc_c2b_iftype(idb);
	ifinfo->protocols_supported = VNSM_LIF_SUPP_VINES;
	sstrncpy(ifinfo->name, swidb->namestring, VNSM_LEN_IFNAME);
	if ((hwidb->extract_hwaddr)(hwidb, &hwaddr)) {
	    /* Copy 6 bytes, indicate how many are actually valid */
	    ieee_copy(hwaddr.addr, ifinfo->interface_address);
	    ifinfo->sig_bytes = min(hwaddr.length, VNSM_MAX_MACLENGTH);
	} else {
	    ifinfo->sig_bytes = 0;
	}
	if (++count >= VNSM_NUM_INTERFACES)
	    break;
    }
    reply->ifinfo_count = count;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *        NSM - Get Lan Interface Stats (10)
 *
 **************************************************/

/*
 * vnsm_count_arps
 *
 * Count up all the received and transmitted VINES ARPs for an
 * interface.
 */
static void vnsm_count_arps (
    vinesidbtype *idb,
    ulong *rx_arp,
    ulong *tx_arp)
{
    ulong tx, rx;
    uchar i;

    tx = 0;
    rx = idb->rx_arpv0_illegal + idb->rx_arpv1_illegal;
    for (i = 0; i < VARP_LAST; i++) {
	tx += idb->tx_arpv0_type[i] + idb->tx_arpv1_type[i];
	rx += idb->rx_arpv0_type[i] + idb->rx_arpv1_type[i];
    }
    *rx_arp = rx;
    *tx_arp = tx;
}

/*
 * vnsm_getlanifstat_ether
 *
 * Insert ethernet specific MAC layer information into the current
 * message.  This information field has the separate TAGS/DATA format.
 */
static ushort vnsm_getlanifstat_ether (
    ushort tags_data,
    vinesidbtype *idb,
    vnsm_getlanifstat_reply *reply)
{
    ulong rx_arp, tx_arp;
    idbtype *swidb = idb->phys_idb;
    hwidbtype *hwidb = swidb->hwptr;
    uchar *data;

    strcpy(reply->help_section, "nmether");
    reply->rows = 8;
    data = reply->data;
    
    if (tags_data == VNSM_TAGS) {
	data = vines_insert_string(data, "Total in           ", 20);
	data = vines_insert_string(data, "Input errors       ", 20);
	data = vines_insert_string(data, "Total out          ", 20);
	data = vines_insert_string(data, "Collisions         ", 20);
	data = vines_insert_string(data, "Runts              ", 20);
	data = vines_insert_string(data, "Unknowns           ", 20);
	data = vines_insert_string(data, "ARP packets in     ", 20);
	data = vines_insert_string(data, "ARP packets out    ", 20);
    } else if (tags_data == VNSM_DATA) {
	vnsm_count_arps(idb, &tx_arp, &rx_arp);
	data = vnsm_insert_counter32(data, idb_inputs(hwidb));
	data = vnsm_insert_counter32(data, hwidb->counters.input_error);
	data = vnsm_insert_counter32(data, idb_outputs(hwidb));
	data = vnsm_insert_counter32(data, hwidb->counters.output_collision);
	data = vnsm_insert_counter32(data, hwidb->counters.runts);
	data = vnsm_insert_counter32(data, 0);
	data = vnsm_insert_counter32(data, rx_arp);
	data = vnsm_insert_counter32(data, tx_arp);
    } else {
	return(VNSM_ERR_INVPARM);
    }

    reply->data_count = data - reply->data;
    return(VSVC_NO_ERROR);
}

/*
 * vnsm_getlanifstat_token
 *
 * Insert token ring specific MAC layer information into the current
 * message.  This information field has the separate TAGS/DATA format.
 */
static ushort vnsm_getlanifstat_token (
    ushort tags_data,
    vinesidbtype *idb,
    vnsm_getlanifstat_reply *reply)
{
    ulong rx_arp, tx_arp;
    idbtype *swidb = idb->phys_idb;
    hwidbtype *hwidb = swidb->hwptr;
    uchar *data;

    /*
     * The following looks strange, but it will get all the information
     * we need from the private data structures on the individual
     * interface cards, and put it into common location on the idb.
     */
    if (hwidb->dot5_get_smt_stats) {
	(*hwidb->dot5_get_smt_stats)(hwidb);
	(*hwidb->dot5_add_counters)(hwidb);
    }

    strcpy(reply->help_section, "nmtoken");
    reply->rows = 22;
    data = reply->data;
    
    if (tags_data == VNSM_TAGS) {
	data = vines_insert_string(data, "Total in            ", 25);
	data = vines_insert_string(data, "Input errors        ", 25);
	data = vines_insert_string(data, "Total out           ", 25);
	data = vines_insert_string(data, "Collisions          ", 25);
	data = vines_insert_string(data, "Runts               ", 25);
	data = vines_insert_string(data, "Unknowns            ", 25);
	data = vines_insert_string(data, "ARP packets in      ", 25);
	data = vines_insert_string(data, "ARP packets out     ", 25);
	data = vines_insert_string(data, "Line errors         ", 25);
	data = vines_insert_string(data, "Internal errors     ", 25);
	data = vines_insert_string(data, "Burst errors        ", 25);
	data = vines_insert_string(data, "A/C errors          ", 25);
	data = vines_insert_string(data, "Abort delimiters    ", 25);
	data = vines_insert_string(data, "Lost frames         ", 25);
	data = vines_insert_string(data, "Congestion count    ", 25);
	data = vines_insert_string(data, "Frame copy errors   ", 25);
	data = vines_insert_string(data, "Frequency errors    ", 25);
	data = vines_insert_string(data, "Token errors        ", 25);
	data = vines_insert_string(data, "Last Beacon Type    ", 25);
	data = vines_insert_string(data, "Beacon NAUN         ", 25);
	data = vines_insert_string(data, "Ring status         ", 25);
	data = vines_insert_string(data, "Active Upstream NAUN", 25);
    } else if (tags_data == VNSM_DATA) {
	itemlist *tmp_list = NULL;
	itemdesc *tmp_item = NULL;

	item_list_malloc(tmp_list, tmp_item, 14);
	if (tmp_list == NULL) {
	    crashdump(0);
	}

	item_desc_init(&tmp_item[0], TOKEN_HW_PREV_ERR_LINE, sizeof(ulong));
	item_desc_init(&tmp_item[1], TOKEN_HW_PREV_ERR_INTERNAL,
		       sizeof(ulong));
	item_desc_init(&tmp_item[2], TOKEN_HW_PREV_ERR_BURST, sizeof(ulong));
	item_desc_init(&tmp_item[3], TOKEN_HW_PREV_ERR_ARIFCI, sizeof(ulong));
	item_desc_init(&tmp_item[4], TOKEN_HW_PREV_ERR_ABORT, sizeof(ulong));
	item_desc_init(&tmp_item[5], TOKEN_HW_PREV_ERR_LOSTFRAME,
		       sizeof(ulong));
	item_desc_init(&tmp_item[6], TOKEN_HW_PREV_ERR_RCVCONGESTION,
		       sizeof(ulong));
	item_desc_init(&tmp_item[7], TOKEN_HW_PREV_ERR_COPY, sizeof(ulong));
	item_desc_init(&tmp_item[8], TOKEN_HW_PREV_ERR_FREQ, sizeof(ulong));
	item_desc_init(&tmp_item[9], TOKEN_HW_PREV_ERR_TOKEN, sizeof(ulong));
	item_desc_init(&tmp_item[10], TOKEN_HW_LAST_BEACON_TYPE,
		       sizeof(ushort));
	item_desc_init(&tmp_item[11], TOKEN_HW_BEACON_NAUN, sizeof(ulong));
	item_desc_init(&tmp_item[12], TOKEN_HW_RING_STATUS, sizeof(ushort));
	item_desc_init(&tmp_item[13], TOKEN_HW_NAUN, sizeof(ulong));

	(*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE, tmp_list);
	
	vnsm_count_arps(idb, &tx_arp, &rx_arp);
	data = vnsm_insert_counter32(data, idb_inputs(hwidb));
	data = vnsm_insert_counter32(data, hwidb->counters.input_error);
	data = vnsm_insert_counter32(data, idb_outputs(hwidb));
	data = vnsm_insert_counter32(data, hwidb->counters.output_collision);
	data = vnsm_insert_counter32(data, hwidb->counters.runts);
	data = vnsm_insert_counter32(data, 0);
	data = vnsm_insert_counter32(data, rx_arp);
	data = vnsm_insert_counter32(data, tx_arp);
	data = vnsm_insert_counter32(data, tmp_item[0].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[1].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[2].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[3].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[4].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[5].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[6].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[7].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[8].u.l_value);
	data = vnsm_insert_counter32(data, tmp_item[9].u.l_value);
	data = vnsm_insert_counter16(data, tmp_item[10].u.s_value[0]);
	data = vnsm_insert_fmtdmac(data,   tmp_item[11].u.buffer);
	data = vnsm_insert_counter16(data, tmp_item[12].u.s_value[0]);
	data = vnsm_insert_fmtdmac(data,   tmp_item[13].u.buffer);

	free(tmp_list);
    } else {
	return(VNSM_ERR_INVPARM);
    }

    reply->data_count = data - reply->data;
    return(VSVC_NO_ERROR);
}

/*
 * vnsm_getlanifstat_fddi
 *
 * Insert fddi specific MAC layer information into the current
 * message.  This information field has the separate TAGS/DATA format.
 */
static ushort vnsm_getlanifstat_fddi (
    ushort tags_data,
    vinesidbtype *idb,
    vnsm_getlanifstat_reply *reply)
{
    ulong rx_arp, tx_arp;
    idbtype *swidb = idb->phys_idb;
    hwidbtype *hwidb = swidb->hwptr;
    uchar *data;
    sys_timestamp time_storage;

    strcpy(reply->help_section, "nmfddi");
    reply->rows = 14;
    data = reply->data;
    
    if (tags_data == VNSM_TAGS) {
	data = vines_insert_string(data, "Total in             ",25);
	data = vines_insert_string(data, "Input errors         ",25);
	data = vines_insert_string(data, "Total out            ",25);
	data = vines_insert_string(data, "Collisions           ",25);
	data = vines_insert_string(data, "Runts                ",25);
	data = vines_insert_string(data, "Unknowns             ",25);
	data = vines_insert_string(data, "ARP packets in       ",25);
	data = vines_insert_string(data, "ARP packets out      ",25);
	data = vines_insert_string(data, "Active Upstream NAUN ",25);
	data = vines_insert_string(data, "Active Dnstream NAUN ",25);
	data = vines_insert_string(data, "CFM State            ",25);  
	data = vines_insert_string(data, "Token Rotation (Req) ",25);
	data = vines_insert_string(data, "Token Rotation (Act) ",25);
	data = vines_insert_string(data, "Time Operational     ",25);
    } else if (tags_data == VNSM_DATA) {
	N_ITEM_LIST(tmp_config, 2);
	N_ITEM_LIST(tmp_state, 2);
	
	item_desc_init(&tmp_config_item[0], FDDI_HW_TRT, sizeof(ulong));

	item_desc_init(&tmp_config_item[1], FDDI_HW_TRT_NEG, sizeof(ulong));

	item_desc_init(&tmp_state_item[0], FDDI_HW_CFM_STATE, sizeof(ushort));

	item_desc_init(&tmp_state_item[1], FDDI_HW_OPERATIONAL,
		       sizeof(sys_timestamp)); 
	tmp_state_item[1].u.buffer = &time_storage;

	(*hwidb->state_config)(hwidb, IDB_CONTROL_READ_CONFIG,
			       &tmp_config_list);

	(*hwidb->state_config)(hwidb, IDB_CONTROL_READ_STATE,
			       &tmp_state_list);

	vnsm_count_arps(idb, &tx_arp, &rx_arp);
	data = vnsm_insert_counter32(data, idb_inputs(hwidb));
	data = vnsm_insert_counter32(data, hwidb->counters.input_error);
	data = vnsm_insert_counter32(data, idb_outputs(hwidb));
	data = vnsm_insert_counter32(data, hwidb->counters.output_collision);
	data = vnsm_insert_counter32(data, hwidb->counters.runts);
	data = vnsm_insert_counter32(data, 0);
	data = vnsm_insert_counter32(data, rx_arp);
	data = vnsm_insert_counter32(data, tx_arp);
	data = vnsm_insert_fmtdmac(data, 0);
	data = vnsm_insert_fmtdmac(data, 0);

	/*
	 * Get cfm_state
	 */
	data = vnsm_insert_counter16(data, tmp_state_item[0].u.s_value[0]);

	/*
	 * Get TRT
	 */
	data = vnsm_insert_counter32(data,
				     (tmp_config_item[0].u.l_value * 10) /
				     ((FDDI_BYTES_SECOND) / 100000));

	/*
	 * Get TRT Neg.
	 */
	data = vnsm_insert_counter32(data,
				     (tmp_config_item[1].u.l_value * 10) /
				     ((FDDI_BYTES_SECOND) / 100000));

	/*
	 * Get fddi_operational
	 */
	data = vnsm_insert_ticks32(data, TS_VALUE32(time_storage));
    } else {
	return(VNSM_ERR_INVPARM);
    }

    reply->data_count = data - reply->data;
    return(VSVC_NO_ERROR);
}

/*
 * vnsm_getlanifstat_rcv_call
 *
 * Get specific runtime information about a single LAN interface on this
 * router.
 */
ushort vnsm_getlanifstat_rcv_call (
    vinesipcsocket *socket,
    vnsm_getlanifstat_query *query,
    vnsm_getlanifstat_reply *reply)
{
    vinesidbtype *idb;
    ushort invalid;

    /*
     * Validate both the version number and user.  Bail if either it fails.  The
     * version number has been checked by the calling routine.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (invalid)
	return(invalid);
    if (query->slot == 0)
	return(VNSM_ERR_INVPARM);

    /*
     * Start building the reply, and begin determine the actual table
     * requested.  It is easiest to do this at the same time, so that we
     * only need to do the three way switch once instead of twice.
     */
    reply->version = VNSM_VERSION_1;
    strcpy(reply->help_file, "snm.txt");
    strcpy(reply->language, "USA");

    FOR_ALL_VIDBS(idb) {
	if (idb->svc_slot != query->slot)
	    continue;
	if (!vines_is_lan(idb))
	    continue;
	if (idb_is_ether_like(idb->phys_idb)) {
	    invalid = vnsm_getlanifstat_ether(query->tags_data, idb, reply);
	} else if (idb->phys_idb->hwptr->status & IDB_TR) {
	    invalid = vnsm_getlanifstat_token(query->tags_data, idb, reply);
	} else if (idb->phys_idb->hwptr->status & IDB_FDDI) {
	    invalid = vnsm_getlanifstat_fddi(query->tags_data, idb, reply);
	} else {
	    invalid = VNSM_ERR_NOTLAN;
	}
	break;
    }
    if (invalid)
	return(invalid);
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *        NSM - Get Lan IF Summary Stats (11)
 *
 **************************************************/

/*
 * vnsm_getlanifsummstat_ifinfo
 *
 * Get summary runtime information for all of the LAN interfaces on
 * this router.  Interfaces must be configured for vines statistic
 * reporting, and must be LAN interfaces.
 */
static void vnsm_getlan_ifinfo (
    vnsm_getlanifsummstat_reply *reply)
{
    vnsm_getlanifsummstat_ifinfo *ifinfo;
    vnsm_getlanifsummstat_ifs *ifs;
    vinesidbtype *idb;
    hwidbtype *hwidb;
    idbtype *swidb;
    ushort count;

    ifs = &reply->info.ifs;
    count = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb->vines_metric == 0)
	    continue;
	if (idb->svc_slot == 0)
	    continue;
	if (!vines_is_lan(idb))
	    continue;
	swidb = idb->phys_idb;
	hwidb = swidb->hwptr;
	ifinfo = &ifs->ifinfo[count];
	ifinfo->line_slot = idb->svc_slot;
	ifinfo->type = vsvc_c2b_iftype(idb);
	sstrncpy(ifinfo->name, swidb->namestring, VNSM_LEN_IFNAME);
	ifinfo->total_in = vines_frames_input(hwidb);
	ifinfo->total_in_valid = TRUE;
	ifinfo->total_out = vines_frames_output(hwidb);
	ifinfo->total_out_valid = TRUE;
	ifinfo->errs_in = idb->rx_too_small + idb->rx_too_large +
	    idb->rx_bad_size + idb->rx_not_enabled +
	    idb->rx_checksum_error;
	ifinfo->errs_in_valid = TRUE;
	ifinfo->errs_out = idb->tx_failed_encaps + idb->tx_failed_down;
	ifinfo->errs_out_valid = TRUE;
	if (++count >= VNSM_NUM_INTERFACES)
	    break;
    }
    ifs->ifinfo_count = count;
}

/*
 * vnsm_getlanifsummstat_rcv_call
 *
 * Get specific runtime information related to the LAN interfaces on
 * this router.  Some of the information types seem weird for this
 * call, but here they are.
 */
ushort vnsm_getlanifsummstat_rcv_call (
    vinesipcsocket *socket,
    vnsm_getlanifsummstat_query *query,
    vnsm_getlanifsummstat_reply *reply)
{
    ushort invalid;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (invalid)
	return(invalid);

    /*
     * Build the reply packet, and we're done
     */
    reply->version = VNSM_VERSION_1;
    reply->info_type = query->info_type;
    switch (query->info_type) {
      case VNSM_LANIF_COMMSTATS:
#ifdef DRH_ORIGINAL
	/* Just leave everything at zero. */
	break;
#else
	return(VNSM_ERR_NOTSUPPORTED);
#endif
      case VNSM_LANIF_IFSTATS:
	vnsm_getlan_ifinfo(reply);
	break;
      default:
	return(VNSM_ERR_NOTSUPPORTED);
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *     NSM - Get Protocol Stack Table Info (14)
 *
 **************************************************/

/*
 * vnsm_pstk_i_add_protocol
 *
 * Helper routine for adding protocol name/id table name/id sets to a
 * outgoing message.
 */
static inline void vnsm_pstk_i_add_protocol (
    vnsm_getpstktblinfo_table *entry,
    ushort protocol_id,
    char *protocol_name,
    ushort table_id,
    char *table_name)
{
    entry->protocol_id = protocol_id;
    strcpy(entry->protocol_name, protocol_name);
    entry->table_id = table_id;
    strcpy(entry->table_name, table_name);
}

/*
 * vnsm_getpstktblinfo_rcv_call
 *
 * Get static configuration information about which protocols are
 * suppoered by this router, and which tables are available for
 * extracting information.  The actual data queries use a different
 * call number, #15.
 */
ushort vnsm_getpstktblinfo_rcv_call (
    vinesipcsocket *socket,
    vnsm_getpstktblinfo_query *query,
    vnsm_getpstktblinfo_reply *reply)
{
    ushort invalid;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (invalid)
	return(invalid);

    /*
     * Build the reply.
     */
    reply->version = VNSM_VERSION_1;
    reply->table_count = 2;
    switch (query->protocol_family) {
      case VNSM_FAMILY_VINES:
	vnsm_pstk_i_add_protocol(&reply->table[0],
				 VNSM_PROT_VINESRTP, "RTP",
				 VNSM_TBL_VRTP_NBRS, "Neighbors");
	vnsm_pstk_i_add_protocol(&reply->table[1],
				 VNSM_PROT_VINESRTP, "RTP",
				 VNSM_TBL_VRTP_ROUTES, "Routes");
	break;
      default:
	return(VNSM_ERR_NOTIMPLEMENTED);
    }
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *        NSM - Get Protocol Stack Info (15)
 *
 **************************************************/

/*
 * Data structure for remembering where to pick up in the router's
 * tables for the second and subsequent messages a multi-message
 * response.  The *only* reason that this message does not use the
 * normal "pass" variable mechanism, is that it needs to keep track of
 * a six byte address from the neighbor's table, but the pass variable
 * is only four bytes.
 */

typedef struct vnsm_pstk_info_ {
    vnsm_getpstkinfo_reply *reply;
    vnsm_pstk_pass_info *pass;
} vnsm_pstk_info;

static vnsm_pstk_pass_info *vnsm_find_pass (
    ulong  req_net,
    ushort req_host,
    ushort family,
    ushort protocol,
    ushort table,
    ulong  pass)
{
    vnsm_pstk_pass_info *info;
    dqueue_t *p;

    RUN_DQUEUE(vnsm_pstk_passQ, p) {
	info = p->parent;
	if ((info->req_net == req_net) && (info->req_host == req_host) &&
	    (info->family == family) && (info->protocol == protocol) &&
	    (info->table == table) && (info->iteration == pass)) {
	    lw_remove(p);
	    return(info);
	}
    }
    return(NULL);
}

static void vnsm_remember_pass (vnsm_pstk_pass_info *pass_info)
{
    o_init(&pass_info->p, pass_info);
    lw_insert(&pass_info->p, &vnsm_pstk_passQ);
}

/**************************************************/

/*
 * vnsm_insert_one_neighbor
 * 
 * Copy the relevent information from one entry in the neighbor table
 * into the NSM message currently being built.  All MAC address are
 * stored in a six byte field.  Another firld indicates how many bytes
 * are actually valid.
 */
static boolean vnsm_insert_one_neighbor (treeLink *link, void *dummy)
{
    vnsm_pstk_pass_info *pass_info;
    vnsm_getpstkinfo_reply *reply;
    vinesneighbortype *neighbor;
    vnsm_pstk_info *info;
    vinespathtype *path;
    vinesidbtype *idb;
    hwaddrtype *hwaddr, ifhwaddr;
    idbtype *swidb;
    uchar *data;

    neighbor = vneighCast(link);
    if (neighbor->newflags & V_N_ME)
	return(TRUE);

    info = dummy;
    pass_info = info->pass;
    pass_info->last_net = neighbor->net;
    pass_info->last_host = neighbor->host;

    reply = info->reply;
    data = reply->data + reply->data_count;
    FOR_ALL_PATHS(neighbor, path) {
	idb = path->idb;
	if ((idb->svc_slot == 0) && (idb->svc_line == 0))
	    continue;
	swidb = idb->phys_idb;
	hwaddr = &path->dlc_addr;
	if (vines_is_lan(idb)) {
	    data = vnsm_insert_mac(data, hwaddr->addr);
	    data = vnsm_insert_rawdata(data, IEEEBYTES, 1);
	} else {
	    data = vnsm_insert_mac(data, zeromac);
	    if (is_x25(swidb->hwptr)) {
		data = vnsm_insert_rawdata(data, 1, 1);
	    } else {
		data = vnsm_insert_rawdata(data, 0, 1);
	    }
	}
	data = vnsm_insert_rawdata(data, neighbor->net, 4);
	data = vnsm_insert_rawdata(data, neighbor->host, 2);
	data = vnsm_insert_rawdata(data, VSRTP_NODE_ROUTER - neighbor->node, 1);
	data = vnsm_insert_rawdata(data, path->idb->svc_slot, 1);
	data = vnsm_insert_rawdata(data, path->idb->svc_line, 1);
	data = vnsm_insert_rawdata(data, vsvc_c2b_iftype(idb), 1);
	if ((swidb->hwptr->extract_hwaddr)(swidb->hwptr, &ifhwaddr)) {
	    if (vines_is_lan(idb)) {
		data = vnsm_insert_mac(data, ifhwaddr.addr);
	    } else if (is_x25(swidb->hwptr)) {
		int vc;
		uchar temp[6];
		lcitype *lci;
		
		*((ulong *)&temp[0]) = 0;
		*((ushort *)&temp[4]) = 0;
		lci = reg_invoke_media_x25_lookup_lci(swidb->hwptr, hwaddr->addr, hwaddr->length);
		if (lci) {
		    boolean success;
		    success = reg_invoke_media_x25_get_lci_info(lci, &vc, NULL, NULL);
		    if (success)
			temp[0] = vc;
		}
		data = vnsm_insert_mac(data, temp);
	    } else {
		data = vnsm_insert_mac(data, zeromac);
	    }
	} else {
	    data = vnsm_insert_mac(data, zeromac);
	}
	reply->data_count = data - reply->data;
	reply->rows++;
    }
    if (reply->data_count >= VNSM_DATA_LIMIT)
	return(FALSE);
    return(TRUE);
}

/*
 * vnsm_getpstkinfo_vines_neighbors
 *
 * Dump as much of the vines neighbor table as will fit into a NSM
 * message.  This routine will be invoked repetitively (at the clients
 * behest) until the entire table has been dumped.
 */
static ushort vnsm_getpstkinfo_vines_neighbors (
    vinesipcsocket *socket,
    vnsm_getpstkinfo_query *query,
    vnsm_getpstkinfo_reply *reply)
{
    vnsm_pstk_pass_info *pass_info;
    vnsm_pstk_info fn_data;
    vinesneighbortype *n;
    boolean finished;
    ushort tags_data;
    uchar *data;

    if ((query->version != VNSM_VERSION_1) &&
	(query->version != VNSM_VERSION_2))
	return(VNSM_ERR_UNSUPREV);
    if (query->runtime != VNSM_RUNTIME)
	return(VNSM_ERR_INVPARM);

    reply->columns = 9;
    strcpy(reply->table_name, "Neighbors");
    strcpy(reply->help_section, "nmvnbtab");
    
    tags_data = query->tags_data;
    if (tags_data == VNSM_TAGS) {
	data = reply->data;
	data = vnsm_insert_label(data, "LinkAdr", VNSM_NMA_LLADDRESS, 0, IEEEBYTES);
	data = vnsm_insert_label(data, "Sig ",    VNSM_NMA_BYTE, 0, 1);
	data = vnsm_insert_label(data, "NetId     ", VNSM_NMA_COUNTER32, 0, 4);
	data = vnsm_insert_label(data, "SNetId ", VNSM_NMA_COUNTER16, 0, 2);
	data = vnsm_insert_label(data, "NbrTyp",  VNSM_NMA_BYTE, 0, 1);
	data = vnsm_insert_label(data, "Slot   ", VNSM_NMA_BYTE, 0, 1);
	data = vnsm_insert_label(data, "Line   ", VNSM_NMA_BYTE, 0, 1);
	data = vnsm_insert_label(data, "IfcTyp",  VNSM_NMA_BYTE, 0, 1);
	data = vnsm_insert_label(data, "IfcAdr",  VNSM_NMA_LLADDRESS, 0, IEEEBYTES);
	reply->data_count = data - reply->data;
	return(VSVC_NO_ERROR);
    } else if (tags_data != VNSM_DATA) {
	return(VNSM_ERR_INVPARM);
    }

    /*
     * If the pass number indicates that this is not the first pass through
     * this routine, look up the status information recorded during the
     * previous pass.
     */
    if (query->pass == 0) {
	pass_info = malloc(sizeof(vnsm_pstk_pass_info));
	if (pass_info == NULL)
	    return (VNSM_ERR_NOMEM);
	pass_info->req_net   = socket->remote_net;
	pass_info->req_host  = socket->remote_host;
	pass_info->family    = query->family;
	pass_info->protocol  = query->protocol;
	pass_info->table     = query->table;
	pass_info->iteration = 1;
	reply->timestamp     = vss_get_time();
    } else {
	pass_info = vnsm_find_pass(socket->remote_net, socket->remote_host,
				   query->family, query->protocol,
				   query->table, query->pass);
	if (pass_info == NULL) {
	    return(VNSM_ERR_INVPARM);
	}
	pass_info->iteration++;
	reply->timestamp = query->timestamp;
    }

    /*
     * Find out where we left off at the lass pass through this table and
     * start again there.  This code looks up the neighbor fresh instead of
     * keeping a pointer to the old neighbor structure.  This makes it safe
     * for neighbor deletions.  If we can't find the neighbor from the
     * previous pass, assume that the local table has changed and sucessfully
     * return no data instead of aborting the transaction.
     */
    if (query->pass == 0) {
	n = vneigh_GetFirstNeighbor();
    } else {
	n = vneigh_FindEntry(pass_info->last_net, pass_info->last_host);
	if (n == NULL)
	    return(VSVC_NO_ERROR);
	n = vneigh_GetNextNeighbor(n);
    }
    if (n == NULL)
	return(VSVC_NO_ERROR);

    fn_data.reply = reply;
    fn_data.pass = pass_info;
    finished = vneigh_WalkPartialTree(vnsm_insert_one_neighbor, n, &fn_data);
    if (!finished) {
	reply->pass = pass_info->iteration;
	vnsm_remember_pass(pass_info);
    } else {
	free(pass_info);
    }
    return(VSVC_NO_ERROR);
}

/*
 * v_c2b_flags
 *
 * Convert cisco internal flags word to a banyan flags word, or
 * approximate it as close as possible.
 */
static inline ushort v_c2b_flags (
    vinesnettype *server,
    vinesroutetype *route)
{
    ushort banyan;

    banyan = VNSM_FLAG_SERVER | VNSM_FLAG_USABLE;
    if (TIMER_RUNNING_AND_SLEEPING(server->suppresstime))
	banyan |= VNSM_FLAG_SUPPRESSION2;
    if (TIMER_RUNNING_AND_SLEEPING(server->changetime))
	banyan |= VNSM_FLAG_MODIFIED;
    if (route->flags & V_NET_REDIRECT)
	banyan |= VNSM_FLAG_REDIRECT_M;
    if (route->rtp_flags & VSRTP_ENTRY_LAN)
	banyan |= VNSM_FLAG_LAN;
    else if (route->rtp_flags & VSRTP_ENTRY_WAN)
	banyan |= VNSM_FLAG_WAN;
    if (route->rtp_flags & VSRTP_ENTRY_NO_SEQ)
	banyan |= VNSM_FLAG_PRE_550;
    return(banyan);
}

/*
 * vnsm_insert_one_route
 * 
 * Copy the relevent information from one entry in the routing table
 * into the NSM message currently being built.
 */
static boolean vnsm_insert_one_route (treeLink *link, void *dummy)
{
    vnsm_getpstkinfo_reply *reply;
    vinesroutetype *route;
    vinesnettype *server;
    uchar *data;
    int ttl, interval;

    server = vrouteCast(link);
    if (server->flags & V_NET_ME)
	return(TRUE);

    reply = dummy;
    data = reply->data + reply->data_count;
    FOR_ALL_ROUTES(server, route) {
	data = vnsm_insert_rawdata(data, server->net, 4);
	data = vnsm_insert_rawdata(data, vmetric_to_srtp(server->adv_metric), 2);
	if (route->gw->version == VRTP_VERSION) {
	    interval = vines_get_slowest_update_interval(route->gw);
	    if (interval != -1) {
		ttl = VNSM_RTP_MAXTTL - (ELAPSED_TIME(route->timer)/interval);
		ttl = max(0, ttl);
	    } else {
		ttl = VNSM_RTP_NOTAGED;
	    }
	} else {
	    ttl = VNSM_RTP_NOTAGED;
	}
	data = vnsm_insert_rawdata(data, ttl, 2);
	data = vnsm_insert_rawdata(data, v_c2b_flags(server, route), 2);
	data = vnsm_insert_rawdata(data, route->gw->net, 4);
	data = vnsm_insert_rawdata(data, 0 /* no pref gw */, 4);
	reply->data_count = data - reply->data;
	reply->rows++;
    }
    if (reply->data_count >= VNSM_DATA_LIMIT) {
	reply->pass = server->net;
	return(FALSE);
    }
    return(TRUE);
}

/*
 * vnsm_getpstkinfo_vines_routes
 *
 * Dump as much of the vines routing table as will fit into a NSM
 * message.  This routine will be invoked repetitively (at the clients
 * behest) until the entire table has been dumped.
 */
static ushort vnsm_getpstkinfo_vines_routes (
    vnsm_getpstkinfo_query *query,
    vnsm_getpstkinfo_reply *reply)
{
    vinesnettype *svr;
    boolean finished;
    uchar *data;

    if ((query->version != VNSM_VERSION_1) &&
	(query->version != VNSM_VERSION_2))
	return(VNSM_ERR_UNSUPREV);

    reply->columns = 6;
    strcpy(reply->table_name, "Routes");
    strcpy(reply->help_section, "nmvrttab");
    data = reply->data;

    if (query->tags_data == VNSM_TAGS) {
	data = vnsm_insert_label(data, "NetId     ", VNSM_NMA_COUNTER32, 0, 4);
	data = vnsm_insert_label(data, "Metric ",    VNSM_NMA_COUNTER16, 0, 2);
	data = vnsm_insert_label(data, "TTL",        VNSM_NMA_COUNTER16, 0, 2);
	data = vnsm_insert_label(data, "Flags ",     VNSM_NMA_STATUS16, 0, 2);
	data = vnsm_insert_label(data, "GateNetId ", VNSM_NMA_COUNTER32, 0, 4);
	data = vnsm_insert_label(data, "PrefNetId ", VNSM_NMA_COUNTER32, 0, 4);
	reply->data_count = data - reply->data;
    } else if (query->tags_data == VNSM_DATA) {
	if (query->pass == 0) {
	    svr = vroute_GetFirstNetwork();
	} else {
	    svr = vroute_FindEntry(query->pass);
	    if (svr == NULL)
		return(VSVC_NO_ERROR);
	    svr = vroute_GetNextNetwork(svr);
	}
	if (svr == NULL)
	    return(VSVC_NO_ERROR);
	
	finished = vroute_WalkPartialTree(vnsm_insert_one_route, svr, reply);
	if (finished)
	    reply->pass = 0;
    } else {
	return(VNSM_ERR_INVPARM);
    }
    return(VSVC_NO_ERROR);
}

/*
 * vnsm_getpstkinfo_rcv_call
 *
 * Get runtime information about the network layers on this
 * router.
 */
ushort vnsm_getpstkinfo_rcv_call (
    vinesipcsocket *socket,
    vnsm_getpstkinfo_query *query,
    vnsm_getpstkinfo_reply *reply)
{
    ushort invalid;

    /*
     * Validate the user.  Bail if either it fails.  The version number has
     * been checked by the calling routine.
     */
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (invalid)
	return(invalid);

    /*
     * Start building the reply, and begin determine the actual table
     * requested.  It is easiest to do this at the same time, so that we
     * only need to do the three way switch once instead of twice.
     */
    reply->version = query->version;
    reply->pass = 0;
    reply->rows = 0;
    strcpy(reply->help_file, "snm.txt");
    strcpy(reply->language, "USA");

    switch (query->family) {
      case VNSM_FAMILY_VINES:
	strcpy(reply->family_name, "VINES");
	switch (query->protocol) {
	  case VNSM_PROT_VINESRTP:
	    strcpy(reply->protocol_name, "RTP");
	    switch (query->table) {
	      case VNSM_TBL_VRTP_NBRS:
		invalid = vnsm_getpstkinfo_vines_neighbors(socket, query, reply);
		break;
	      case VNSM_TBL_VRTP_ROUTES:
		invalid = vnsm_getpstkinfo_vines_routes(query, reply);
		break;
	      default:
		invalid = VNSM_ERR_NOTSUPPORTED;
		break;
	    }
	    break;
	  default:
	    invalid = VNSM_ERR_NOTSUPPORTED;
	    break;
	}
	break;
      default:
	invalid = VNSM_ERR_NOTSUPPORTED;
	break;
    }
    if (invalid)
	return(invalid);
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *     NSM - Get Protocol Stack Summary Stats (19)
 *
 **************************************************/

/*
 * vnsm_pstk_s_add_protocol
 *
 * Helper routine for adding protocol name/id table name/id sets to a
 * outgoing message.
 */
static inline void vnsm_pstk_s_add_protocol (
    vnsm_getpstksummstat_protocol *entry,
    ushort protocol_id,
    ushort unknown,
    char *protocol_name,
    ulong total_in,
    ulong total_out,
    ulong errors_in,
    ulong errors_out,
    boolean total_in_v,
    boolean total_out_v,
    boolean errors_in_v,
    boolean errors_out_v)
{
    entry->protocol_id = protocol_id;
    strcpy(entry->protocol_name, protocol_name);
    entry->unknown = unknown;
    entry->total_in   = total_in;
    entry->total_out  = total_out;
    entry->errors_in  = errors_in;
    entry->errors_out = errors_out;
    entry->total_in_valid   = total_in_v;
    entry->total_out_valid  = total_out_v;
    entry->errors_in_valid  = errors_in_v;
    entry->errors_out_valid = errors_out_v;
}

/*
 * vnsm_getpstksummstat_rcv_call
 *
 * Get summary statistics for all protocols in a given protocol
 * "family".  The only family that we support here is the VINES
 * family.  If the user want's anything else, its already available
 * via SNMP.
 */
ushort vnsm_getpstksummstat_rcv_call (
    vinesipcsocket *socket,
    vnsm_getpstksummstat_query *query,
    vnsm_getpstksummstat_reply *reply)
{
    ulong frames_in, frames_out, errors;
    ulong rtp_in, rtp_out, rtp_errors;
    vinesidbtype *idb;
    hwidbtype *hwidb;
    int i;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_2)
	return(VNSM_ERR_UNSUPREV);
    if (query->family != VNSM_FAMILY_VINES)
	return(VNSM_ERR_UNSUPREV);

    /*
     * Accumulate all of the necessary information.
     */
    frames_in = frames_out = errors = 0;
    rtp_in = rtp_out = rtp_errors = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb == vinesnullidb)
	    continue;
	hwidb = idb->phys_idb->hwptr;
	frames_in += vines_frames_input(hwidb);
	frames_out += vines_frames_output(hwidb);
	errors += idb->rx_too_small + idb->rx_too_large +
	    idb->rx_bad_size + idb->rx_checksum_error;
	rtp_errors += idb->rx_rtpv0_illegal + idb->rx_rtpv1_illegal;
	for (i = 0; i < VRTP_LAST; i++) {
	    rtp_in += idb->rx_rtpv0_type[i];
	    rtp_out += idb->tx_rtpv0_type[i];
	}
	for (i = 0; i < VRTP55_LAST; i++) {
	    rtp_in += idb->rx_rtpv1_type[i];
	    rtp_out += idb->tx_rtpv1_type[i];
	}
    }

    /*
     * Build the reply.
     */
    reply->version = VNSM_VERSION_2;
    strcpy(reply->family_name, "VINES");
    reply->protocol_count = 2;
    vnsm_pstk_s_add_protocol(&reply->protocol[0],
			     VNSM_PROT_VINESIP, 0 /*unknown*/, "IP",
			     frames_in, frames_out, errors, 0,
			     TRUE, TRUE, TRUE, TRUE);
    vnsm_pstk_s_add_protocol(&reply->protocol[1],
			     VNSM_PROT_VINESRTP, 0 /*unknown*/, "RTP",
			     rtp_in, rtp_out, rtp_errors, 0,
			     TRUE, TRUE, TRUE, TRUE);
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *     NSM - Get Protocol Stack Statistics (20)
 *
 **************************************************/

/*
 * vnsm_getpstkstats_vip
 *
 * Insert the VINES IP specific statistics into the current message.
 * This information field has the separate TAGS/DATA format.
 */
static ushort vnsm_getpstkstats_vip (
    ushort tags_data,
    vnsm_getpstkstats_reply *reply)
{
    ulong frames_in, frames_out, cksum_errors, too_small;
    ulong bad_length, routed, broadcasts;
    vinesidbtype *idb;
    hwidbtype *hwidb;
    uchar *data;

    strcpy(reply->help_section, "nminter");
    reply->rows = 8;
    data = reply->data;
    
    if (tags_data == VNSM_TAGS) {
	data = vines_insert_string(data, "Total in            ", 25);
	data = vines_insert_string(data, "Total out           ", 25);
	data = vines_insert_string(data, "Bad Checksum        ", 25);
	data = vines_insert_string(data, "Too Small           ", 25);
	data = vines_insert_string(data, "Bad Length          ", 25);
	data = vines_insert_string(data, "No Buffers Avail    ", 25);
	data = vines_insert_string(data, "Routed              ", 25);
	data = vines_insert_string(data, "Broadcasts          ", 25);
    } else if (tags_data == VNSM_DATA) {
	/*
	 * Accumulate all of the necessary information.
	 */
	frames_in = frames_out = cksum_errors = too_small = 0;
	bad_length = routed = broadcasts = 0;
	FOR_ALL_VIDBS(idb) {
	    if (idb == vinesnullidb)
		continue;
	    hwidb = idb->phys_idb->hwptr;
	    frames_in += vines_frames_input(hwidb);
	    frames_out += vines_frames_output(hwidb);
	    cksum_errors += idb->rx_checksum_error;
	    too_small += idb->rx_too_small;
	    bad_length += idb->rx_too_large + idb->rx_bad_size;
	    routed += idb->rx_forwarded + vines_frames_fastin(hwidb);
	    broadcasts += idb->tx_bcast + idb->tx_bcast_forwarded +
		idb->tx_bcast_helpered;
	}
	/*
	 * Write it down.
	 */
	data = vnsm_insert_counter32(data, frames_in);
	data = vnsm_insert_counter32(data, frames_out);
	data = vnsm_insert_counter32(data, cksum_errors);
	data = vnsm_insert_counter32(data, too_small);
	data = vnsm_insert_counter32(data, bad_length);
	data = vnsm_insert_counter32(data, 0); /* No Buff */
	data = vnsm_insert_counter32(data, routed);
	data = vnsm_insert_counter32(data, broadcasts);
    } else {
	return(VNSM_ERR_INVPARM);
    }

    reply->data_count = data - reply->data;
    return(VSVC_NO_ERROR);
}

static uchar *vnsm_getpstkstats_vrtp_tags (uchar *data)
{
    data = vines_insert_string(data, "Total in", 40);
    data = vines_insert_string(data, "Total out", 40);
    data = vines_insert_string(data, "Input Errors", 40);
    data = vines_insert_string(data, "No Buffers Avail", 40);
    data = vines_insert_string(data, "Updates Rcvd", 40);
    data = vines_insert_string(data, "Responses rcvd", 40);
    data = vines_insert_string(data, "Requests in", 40);
    data = vines_insert_string(data, "Request Lookups rcvd", 40);
    data = vines_insert_string(data, "Request SLR info rcvd", 40);
    data = vines_insert_string(data, "Reinit rcvd", 40);
    data = vines_insert_string(data, "Resync rcvd", 40);
    data = vines_insert_string(data, "Redirects rcvd", 40);
    data = vines_insert_string(data, "Fragments in", 40);
    data = vines_insert_string(data, "Fragments Dropped", 40);
    data = vines_insert_string(data, "Invalid Fragments", 40);
    data = vines_insert_string(data, "Duplicate Fragments", 40);
    data = vines_insert_string(data, "Reassemblies done", 40);
    data = vines_insert_string(data, "Reassemblies failed", 40);
    data = vines_insert_string(data, "Outdated Packets", 40);
    data = vines_insert_string(data, "Old Net Info", 40);
    data = vines_insert_string(data, "Invalid Redirects", 40);
    data = vines_insert_string(data, "Incoming broadcasts", 40);
    data = vines_insert_string(data, "No Buffers Available (out)", 40);
    data = vines_insert_string(data, "Updates sent", 40);
    data = vines_insert_string(data, "Responses sent", 40);
    data = vines_insert_string(data, "Response Lookups sent", 40);
    data = vines_insert_string(data, "Response SLR Metric Info sent", 40);
    data = vines_insert_string(data, "Requests out", 40);
    data = vines_insert_string(data, "Reinit sent", 40);
    data = vines_insert_string(data, "Resync sent", 40);
    data = vines_insert_string(data, "Redirects sent", 40);
    data = vines_insert_string(data, "Fragments sent", 40);
    data = vines_insert_string(data, "Fragmentations done", 40);
    data = vines_insert_string(data, "Fragmentations failed", 40);
    data = vines_insert_string(data, "Fragments Dropped (no buffers)", 40);
    data = vines_insert_string(data, "Outgoing broadcasts", 40);
    data = vines_insert_string(data, "Routes created by redirects", 40);
    data = vines_insert_string(data, "Routes modified by redirects", 40);
    data = vines_insert_string(data, "Neighbor Anchor entries", 40);
    data = vines_insert_string(data, "Neighbor Host entries", 40);
    data = vines_insert_string(data, "Network Entries", 40);
    data = vines_insert_string(data, "Routing lookup calls", 40);
    data = vines_insert_string(data, "Failed Routing lookups", 40);
    data = vines_insert_string(data, "Non-sequenced Total in", 40);
    data = vines_insert_string(data, "Non-sequenced Total out", 40);
    data = vines_insert_string(data, "Non-sequenced Total in errs", 40);
    data = vines_insert_string(data, "Non-sequenced Updates rcvd", 40);
    data = vines_insert_string(data, "Non-sequenced Responses rcvd", 40);
    data = vines_insert_string(data, "Non-sequenced Requests rcvd", 40);
    data = vines_insert_string(data, "Non-sequenced Redirects rcvd", 40);
    data = vines_insert_string(data, "Invalid Non-sequenced Redirects rcvd",40);
    data = vines_insert_string(data, "Non-sequenced broadcasts rcvd", 40);
    data = vines_insert_string(data, "Non-sequenced Updates sent", 40);
    data = vines_insert_string(data, "Non-sequenced Responses sent", 40);
    data = vines_insert_string(data, "Non-sequenced Requests send", 40);
    data = vines_insert_string(data, "Non-sequenced Redirects sent", 40);
    data = vines_insert_string(data, "Non-sequenced Broadcasts sent", 40);
    return(data);
}

static uchar *vnsm_getpstkstats_vrtp_data (uchar *data)
{
    ulong rx_rtpv0_illegal, rx_rtpv0_update, rx_rtpv0_response;
    ulong rx_rtpv0_request, rx_rtpv0_redirect, rx_rtpv0_bcast;
    ulong tx_rtpv0_illegal, tx_rtpv0_update, tx_rtpv0_response;
    ulong tx_rtpv0_request, tx_rtpv0_redirect, tx_rtpv0_bcast;
    ulong rx_rtpv1_illegal, rx_rtpv1_update, rx_rtpv1_request;
    ulong rx_rtpv1_reinit, rx_rtpv1_redirect, rx_rtpv1_bcast;
    ulong tx_rtpv1_update, tx_rtpv1_request;
    ulong tx_rtpv1_reinit, tx_rtpv1_redirect, tx_rtpv1_bcast;
    ulong total_in, total_out;
    vinesidbtype *idb;

    rx_rtpv0_illegal = rx_rtpv0_update = rx_rtpv0_response = 0;
    rx_rtpv0_request = rx_rtpv0_redirect = rx_rtpv0_bcast = 0;
    tx_rtpv0_illegal = tx_rtpv0_update = tx_rtpv0_response = 0;
    tx_rtpv0_request = tx_rtpv0_redirect = tx_rtpv0_bcast = 0;
    rx_rtpv1_illegal = rx_rtpv1_update = rx_rtpv1_request = 0;
    rx_rtpv1_reinit = rx_rtpv1_redirect = rx_rtpv1_bcast = 0;
    rx_rtpv1_illegal = tx_rtpv1_update = tx_rtpv1_request = 0;
    tx_rtpv1_reinit = tx_rtpv1_redirect = tx_rtpv1_bcast = 0;
    total_in = total_out = 0;

    FOR_ALL_VIDBS(idb) {
	if (idb == vinesnullidb)
	    continue;
	rx_rtpv1_illegal  += idb->rx_rtpv1_illegal;
	rx_rtpv1_bcast    += idb->rx_rtpv1_bcast;
	tx_rtpv1_bcast    += idb->tx_rtpv1_bcast;
	rx_rtpv1_request  += idb->rx_rtpv1_type[VSRTP_REQUEST];
	rx_rtpv1_update   += idb->rx_rtpv1_type[VSRTP_UPDATE];
	rx_rtpv1_reinit   += idb->rx_rtpv1_type[VSRTP_REINIT];
	rx_rtpv1_redirect += idb->rx_rtpv1_type[VSRTP_REDIRECT];
	tx_rtpv1_request  += idb->tx_rtpv1_type[VSRTP_REQUEST];
	tx_rtpv1_update   += idb->tx_rtpv1_type[VSRTP_UPDATE];
	tx_rtpv1_reinit   += idb->tx_rtpv1_type[VSRTP_REINIT];
	tx_rtpv1_redirect += idb->tx_rtpv1_type[VSRTP_REDIRECT];

	rx_rtpv0_illegal  += idb->rx_rtpv0_illegal +
	    idb->rx_rtpv0_type[2] + idb->rx_rtpv0_type[3];
	rx_rtpv0_bcast    += idb->rx_rtpv0_bcast;
	tx_rtpv0_bcast    += idb->tx_rtpv0_bcast;
	rx_rtpv0_request  += idb->rx_rtpv0_type[VRTP_REQUEST];
	rx_rtpv0_update   += idb->rx_rtpv0_type[VRTP_UPDATE];
	rx_rtpv0_response += idb->rx_rtpv0_type[VRTP_RESPONSE];
	rx_rtpv0_redirect += idb->rx_rtpv0_type[VRTP_REDIRECT];
	tx_rtpv0_illegal  += idb->rx_rtpv0_type[2] + idb->rx_rtpv0_type[3];
	tx_rtpv0_request  += idb->tx_rtpv0_type[VRTP_REQUEST];
	tx_rtpv0_update   += idb->tx_rtpv0_type[VRTP_UPDATE];
	tx_rtpv0_response += idb->tx_rtpv0_type[VRTP_RESPONSE];
	tx_rtpv0_redirect += idb->tx_rtpv0_type[VRTP_REDIRECT];
    }

    total_in = rx_rtpv0_illegal + rx_rtpv0_request + rx_rtpv0_update +
	rx_rtpv0_response + rx_rtpv0_redirect;
    total_in += rx_rtpv1_illegal + rx_rtpv1_request + rx_rtpv1_update +
	rx_rtpv1_reinit + rx_rtpv1_redirect;
    total_out = tx_rtpv0_illegal + tx_rtpv0_request + tx_rtpv0_update +
	tx_rtpv0_response + tx_rtpv0_redirect;
    total_out += tx_rtpv1_request + tx_rtpv1_update + tx_rtpv1_reinit +
	tx_rtpv1_redirect;

    if (vinestest_debug) {
    	buginf("\n%10d  Total in", total_in);
    	buginf("\n%10d  Total out", total_out);
    	buginf("\n%10d  Input Errors", rx_rtpv0_illegal + rx_rtpv1_illegal);
    	buginf("\n%10d  No Buffers Avail", vroute_no_buffer_in);
    	buginf("\n%10d  Updates Rcvd", rx_rtpv1_update - vsrtp_rx_response_count);
    	buginf("\n%10d  Responses rcvd", vsrtp_rx_response_count);
    	buginf("\n%10d  Requests in", rx_rtpv1_request);
    	buginf("\n%10d  Request Lookups rcvd", vsrtp_rx_request_lookup_count);
    	buginf("\n%10d  Request SLR info rcvd", vsrtp_rx_request_slr_count);
    	buginf("\n%10d  Reinit rcvd", rx_rtpv1_reinit);
    	buginf("\n%10d  Resync rcvd", vsrtp_rx_resync_count);
    	buginf("\n%10d  Redirects rcvd", rx_rtpv1_redirect);
    	buginf("\n%10d  Fragments in", vsrtp_rx_frag_rcvd);
    	buginf("\n%10d  Fragments Dropped", vsrtp_rx_frag_drop_old);
    	buginf("\n%10d  Invalid Fragments", vsrtp_rx_frag_drop_oos);
    	buginf("\n%10d  Duplicate Fragments", vsrtp_rx_frag_drop_dup);
    	buginf("\n%10d  Reassemblies done", vsrtp_reass_complete);
    	buginf("\n%10d  Reassemblies failed", vsrtp_reass_failed);
    	buginf("\n%10d  Outdated Packets", vsrtp_rx_outdated);
    	buginf("\n%10d  Old Net Info", vsrtp_rx_oldnetinfo);
    	buginf("\n%10d  Invalid Redirects", 0);
    	buginf("\n%10d  Incoming broadcasts", vroute_bcast_in);
    	buginf("\n%10d  No Buffers Available (out)", vroute_no_buffer_out);
    	buginf("\n%10d  Updates sent", tx_rtpv1_update - vsrtp_tx_response_count);
    	buginf("\n%10d  Responses sent", vsrtp_tx_response_count);
    	buginf("\n%10d  Response Lookups sent", vsrtp_tx_request_lookup_count);
    	buginf("\n%10d  Response SLR Metric Info sent", vsrtp_tx_request_slr_count);
    	buginf("\n%10d  Requests out", tx_rtpv1_request);
    	buginf("\n%10d  Reinit sent", tx_rtpv1_reinit);
    	buginf("\n%10d  Resync sent", vsrtp_tx_resync_count);
    	buginf("\n%10d  Redirects sent", tx_rtpv1_redirect);
    	buginf("\n%10d  Fragments sent", vsrtp_tx_frag_sent);
    	buginf("\n%10d  Fragmentations done", vsrtp_frag_complete);
    	buginf("\n%10d  Fragmentations failed", vsrtp_frag_failed);
    	buginf("\n%10d  Fragments Dropped (no buffers)", vsrtp_tx_frag_drop);
    	buginf("\n%10d  Outgoing broadcasts", vroute_bcast_out);
    	buginf("\n%10d  Routes created by redirects", vroute_redirect_created);
    	buginf("\n%10d  Routes modified by redirects", vroute_redirect_modified);
    	buginf("\n%10d  Neighbor Anchor entries", vneigh_get_path_count());
    	buginf("\n%10d  Neighbor Host entries", vneigh_get_neighbor_count());
    	buginf("\n%10d  Network Entries", vroute_get_route_count());
    	buginf("\n%10d  Routing lookup calls", vroute_get_lookup_success());
    	buginf("\n%10d  Failed Routing lookups", vroute_get_lookup_failed());
    	buginf("\n%10d  Non-sequenced Total in", rx_rtpv0_illegal +
				     rx_rtpv0_update + rx_rtpv0_response +
				     rx_rtpv0_request + rx_rtpv0_redirect);
    	buginf("\n%10d  Non-sequenced Total out", tx_rtpv0_update + tx_rtpv0_response +
				     tx_rtpv0_request + tx_rtpv0_redirect);
    	buginf("\n%10d  Non-sequenced Total in errs", rx_rtpv0_illegal);
    	buginf("\n%10d  Non-sequenced Updates rcvd", rx_rtpv0_update);
    	buginf("\n%10d  Non-sequenced Responses rcvd", rx_rtpv0_response);
    	buginf("\n%10d  Non-sequenced Requests rcvd", rx_rtpv0_request);
    	buginf("\n%10d  Non-sequenced Redirects rcvd", rx_rtpv0_redirect);
    	buginf("\n%10d  Invalid Non-sequenced Redirects rcvd", 0);
    	buginf("\n%10d  Non-sequenced broadcasts rcvd", rx_rtpv0_bcast);
    	buginf("\n%10d  Non-sequenced Updates sent", tx_rtpv0_update);
    	buginf("\n%10d  Non-sequenced Responses sent", tx_rtpv0_response);
    	buginf("\n%10d  Non-sequenced Requests send", tx_rtpv0_request);
    	buginf("\n%10d  Non-sequenced Redirects sent", tx_rtpv0_redirect);
    	buginf("\n%10d  Non-sequenced Broadcasts sent", tx_rtpv0_bcast);
    }

    data = vnsm_insert_counter32(data, total_in);
    data = vnsm_insert_counter32(data, total_out);
    data = vnsm_insert_counter32(data, rx_rtpv0_illegal + rx_rtpv1_illegal);
    data = vnsm_insert_counter32(data, vroute_no_buffer_in);
    data = vnsm_insert_counter32(data, rx_rtpv1_update - vsrtp_rx_response_count);
    data = vnsm_insert_counter32(data, vsrtp_rx_response_count);
    data = vnsm_insert_counter32(data, rx_rtpv1_request);
    data = vnsm_insert_counter32(data, vsrtp_rx_request_lookup_count);
    data = vnsm_insert_counter32(data, vsrtp_rx_request_slr_count);
    data = vnsm_insert_counter32(data, rx_rtpv1_reinit);
    data = vnsm_insert_counter32(data, vsrtp_rx_resync_count);
    data = vnsm_insert_counter32(data, rx_rtpv1_redirect);
    data = vnsm_insert_counter32(data, vsrtp_rx_frag_rcvd);
    data = vnsm_insert_counter32(data, vsrtp_rx_frag_drop_old);
    data = vnsm_insert_counter32(data, vsrtp_rx_frag_drop_oos);
    data = vnsm_insert_counter32(data, vsrtp_rx_frag_drop_dup);
    data = vnsm_insert_counter32(data, vsrtp_reass_complete);
    data = vnsm_insert_counter32(data, vsrtp_reass_failed);
    data = vnsm_insert_counter32(data, vsrtp_rx_outdated);
    data = vnsm_insert_counter32(data, vsrtp_rx_oldnetinfo);
    data = vnsm_insert_counter32(data, 0); /* "Invalid Redirects " */
    data = vnsm_insert_counter32(data, vroute_bcast_in);
    data = vnsm_insert_counter32(data, vroute_no_buffer_out);
    data = vnsm_insert_counter32(data, tx_rtpv1_update - vsrtp_tx_response_count);
    data = vnsm_insert_counter32(data, vsrtp_tx_response_count);
    data = vnsm_insert_counter32(data, vsrtp_tx_request_lookup_count);
    data = vnsm_insert_counter32(data, vsrtp_tx_request_slr_count);
    data = vnsm_insert_counter32(data, tx_rtpv1_request);
    data = vnsm_insert_counter32(data, tx_rtpv1_reinit);
    data = vnsm_insert_counter32(data, vsrtp_tx_resync_count);
    data = vnsm_insert_counter32(data, tx_rtpv1_redirect);
    data = vnsm_insert_counter32(data, vsrtp_tx_frag_sent);
    data = vnsm_insert_counter32(data, vsrtp_frag_complete);
    data = vnsm_insert_counter32(data, vsrtp_frag_failed);
    data = vnsm_insert_counter32(data, vsrtp_tx_frag_drop);
    data = vnsm_insert_counter32(data, vroute_bcast_out);
    data = vnsm_insert_counter32(data, vroute_redirect_created);
    data = vnsm_insert_counter32(data, vroute_redirect_modified);
    data = vnsm_insert_counter32(data, vneigh_get_path_count());
    data = vnsm_insert_counter32(data, vneigh_get_neighbor_count());
    data = vnsm_insert_counter32(data, vroute_get_route_count());
    data = vnsm_insert_counter32(data, vroute_get_lookup_success());
    data = vnsm_insert_counter32(data, vroute_get_lookup_failed());

    data = vnsm_insert_counter32(data, rx_rtpv0_illegal +
				 rx_rtpv0_update + rx_rtpv0_response +
				 rx_rtpv0_request + rx_rtpv0_redirect);
    data = vnsm_insert_counter32(data, tx_rtpv0_update + tx_rtpv0_response +
				 tx_rtpv0_request + tx_rtpv0_redirect);
    data = vnsm_insert_counter32(data, rx_rtpv0_illegal);
    data = vnsm_insert_counter32(data, rx_rtpv0_update);
    data = vnsm_insert_counter32(data, rx_rtpv0_response);
    data = vnsm_insert_counter32(data, rx_rtpv0_request);
    data = vnsm_insert_counter32(data, rx_rtpv0_redirect);
    data = vnsm_insert_counter32(data, 0); /* "Invalid Non-sequenced Redirects rcvd " */
    data = vnsm_insert_counter32(data, rx_rtpv0_bcast);
    data = vnsm_insert_counter32(data, tx_rtpv0_update);
    data = vnsm_insert_counter32(data, tx_rtpv0_response);
    data = vnsm_insert_counter32(data, tx_rtpv0_request);
    data = vnsm_insert_counter32(data, tx_rtpv0_redirect);
    data = vnsm_insert_counter32(data, tx_rtpv0_bcast);
    return(data);
}

/*
 * vnsm_getpstkstats_vrtp
 *
 * Insert the VINES RTP specific statistics into the current message.
 * This information field has the separate TAGS/DATA format.
 */
static ushort vnsm_getpstkstats_vrtp (
    ushort tags_data,
    vnsm_getpstkstats_reply *reply)
{
    uchar *data;

    strcpy(reply->help_section, "nmrtp");
    reply->rows = 57;
    data = reply->data;
    
    if (tags_data == VNSM_TAGS) {
	data = vnsm_getpstkstats_vrtp_tags(data);
    } else if (tags_data == VNSM_DATA) {
	data = vnsm_getpstkstats_vrtp_data(data);
    } else {
	return(VNSM_ERR_INVPARM);
    }

    reply->data_count = data - reply->data;
    return(VSVC_NO_ERROR);
}

/*
 * vnsm_getpstkstats_vrtp_rcv_call
 *
 * Get specific runtime information about a single protocol from a
 * given protocol family on this router.
 */
ushort vnsm_getpstkstats_rcv_call (
    vinesipcsocket *socket,
    vnsm_getpstkstats_query *query,
    vnsm_getpstkstats_reply *reply)
{
    ushort invalid;

    /*
     * Validate both the version number and user.  Bail if either it fails.  The
     * version number has been checked by the calling routine.
     */
    if (query->version != VNSM_VERSION_2)
	return(VNSM_ERR_UNSUPREV);
    if (query->family != VNSM_FAMILY_VINES)
	return(VNSM_ERR_INVPARM);

    /*
     * Start building the reply, and begin determine the actual table
     * requested.  It is easiest to do this at the same time, so that we
     * only need to do the three way switch once instead of twice.
     */
    reply->version = VNSM_VERSION_2;
    strcpy(reply->language, "USA");
    strcpy(reply->help_file, "snm.txt");

    if (query->protocol == VNSM_PROT_VINESIP) {
	invalid = vnsm_getpstkstats_vip(query->tags_data, reply);
    } else if (query->protocol == VNSM_PROT_VINESRTP) {
	invalid = vnsm_getpstkstats_vrtp(query->tags_data, reply);
    } else {
	invalid = VNSM_ERR_INVPARM;
    }
    if (invalid)
	return(invalid);
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *               NSM - Start Send (22)
 *
 **************************************************/

/*
 * vnsm_svrsummstats_send_data
 *
 * Builld a single summary statistics message, and then send copies of
 * it to all clients that are currently monitoring this router.  This
 * routine is called from a periodic task.
 */
static void vnsm_svrsummstats_send_data (void)
{
    vnsm_start_send_other *info;
    vnsm_start_send_svrsummstats *stats;
    vnsm_send_addr *dest;
    vinesidbtype *idb;
    hwidbtype *hwidb;
    ulong framesin, framesout, drops;
    dqueue_t *ptr, *next;

    info = malloc(sizeof(vnsm_start_send_other));
    if (info == NULL)
	return;

    framesin = framesout = drops = 0;
    FOR_ALL_VIDBS(idb) {
	if (idb == vinesnullidb)
	    continue;
	hwidb = idb->phys_idb->hwptr;
	framesin += vines_frames_deltain(hwidb);
	framesout += vines_frames_deltaout(hwidb);
	drops += hwidb->counters.input_drops + hwidb->counters.output_total_drops;
    }

    vsvc_insert_hostname(info->servername);
    info->timestamp = vss_get_time();
    info->unknown = 0;
    stats = &info->svrsummstats;
    stats->loadavg1      = last_busy_percent;
    stats->loadavg5      = avg_busy1_percent;
    stats->loadavg15     = avg_busy5_percent;
    stats->msgavg1       = 0;
    stats->msgavg5       = 0;
    stats->msgavg15      = 0;
    stats->swapavg15     = 0;
    stats->pctcachehits  = 0;
    stats->commbuffusage = 0;
    stats->msgin         = framesin;
    stats->msgout        = framesout;
    stats->drops         = drops;

    mem_lock(info);
    RUN_DQUEUE_W_NEXT(vnsm_send_list, ptr, next) {
	dest = ptr->parent;
	vnsm_start_send_send_other_(&dest->address, info);
    }
    free(info);			/* Unlock info block */
    free(info);			/* Free info block */
}

/*
 * vnsm_enqueue_for_sends
 *
 * Given a vines address, add this address to the list of stations
 * that receive the periodic summary statistics message.
 */
static ushort vnsm_enqueue_for_sends (vinesaddress *address)
{
    sys_timestamp wakeup;
    vnsm_send_addr *addr;
    dqueue_t *p;
    ulong net;

    /*
     * First, prevent broadcast addresses.
     */
    net = GETLONG(address->f.vip.nnet);
    if (!vines_valid_net(net) || !vines_valid_host(address->f.vip.host) ||
	(address->f.vip.port == VIPC_PORT_TRANSIENT))
	return(VNSM_ERR_BCASTPORT);

    /*
     * Next, see if this address is already on the queue.
     */
    RUN_DQUEUE(vnsm_send_list, p) {
	addr = p->parent;
	if (bcmp(&addr->address, address, sizeof(vinesaddress)) == 0) {
	    break;
	}
    }
    if (!DQUEUE_HEAD(vnsm_send_list, p))
	return(VNSM_ERR_ALREADYREG);
    
    /*
     * It's a new address.  Build the data structure and enqueue it.
     */
    addr = malloc(sizeof(vnsm_send_addr));
    if (addr == NULL)
	return(FALSE);
    bcopy(address, &addr->address, sizeof(vinesaddress));
    addr->q.parent = addr;
    lw_insert(&addr->q, vnsm_send_list.blink);

    /*
     * Yank the timer so that a info message will be sent to this port
     * soon.  We don't send one now, because it will mess up the ports
     * for the reply message that is about to be sent.
     */
    TIMER_START(wakeup, 0);
    mgd_timer_set_soonest(&vsvc_nsm_info.timer, wakeup);
    return(TRUE);
}

/*
 * vnsm_start_send_rcv_call
 *
 * The caller would like to receive periodic information about this
 * server.  Stick it in the queue of stations that are receiving
 * periodic traffic totals.
 */
ushort vnsm_start_send_rcv_call  (
    vinesipcsocket *socket,
    vnsm_start_send_query *query,
    vnsm_start_send_reply *reply)
{
    char port_string[20];
    ushort invalid;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (vnsm_debug) {
	vines_printf_address(port_string, &query->data_port);
	buginf(", port %s", port_string);
    }
    if (invalid)
	return(invalid);

    /*
     * Call a routine to send the initial batch of data, and to queue
     * up the destination port for subsequent periodic sends.
     */
    if (!vnsm_enqueue_for_sends(&query->data_port))
	return(VNSM_ERR_NOMEM);

    /*
     * Tell the caller that we are finished.
     */
    reply->version = VNSM_VERSION_1;
    return(VSVC_NO_ERROR);
}

/**************************************************
 *
 *               NSM - Stop Send (23)
 *
 **************************************************/

/*
 * vnsm_stop_send_rcv_call
 *
 * Cancel the periodic sending of server load information to this
 * particular station.
 */
ushort vnsm_stop_send_rcv_call  (
    vinesipcsocket *socket,
    vnsm_stop_send_query *query,
    vnsm_stop_send_reply *reply)
{
    vnsm_send_addr *addr;
    char port_string[20];
    ushort invalid;
    dqueue_t *p;

    /*
     * Validate the version and the user.  Bail if either one fails.
     */
    if (query->version != VNSM_VERSION_1)
	return(VNSM_ERR_UNSUPREV);
    invalid = vsvc_validate_nonce(&vsvc_nsm_info, &query->for_whom);
    if (vnsm_debug) {
	vines_printf_address(port_string, &query->data_port);
	buginf(", port %s", port_string);
    }
    if (invalid)
	return(invalid);

    /*
     * Find the record of this data port, dequeue it, and delete it.
     */
    RUN_DQUEUE(vnsm_send_list, p) {
	addr = p->parent;
	if (bcmp(&addr->address, &query->data_port, sizeof(vinesaddress)) == 0) {
	    lw_remove(p);
	    free(p->parent);
	    return(VSVC_NO_ERROR);
	}
    }
    return(VNSM_ERR_NOTREG);
}

/**************************************************
 *
 *               NSM - Traceroute (27)
 *
 **************************************************/

/*
 *
 */
static void vnsm_rcv_trace_data (
    vinesipcsocket *socket,
    uchar *ipc_data,
    int ipc_length)
{
    vsvc_rtn_info *routine;
    vnsm_trace_other *trace;

    routine = vsvc_find_routine(&vsvc_nsm_info, VNSM_TRACE);
    if (!routine) {
        vsvc_oops_buginf("NSM", "vnsm_trace", "receiving data");
        return;
    }

    trace = vsvc_unpack_other(socket, &vsvc_nsm_info, routine,
			      ipc_data, ipc_length);
    vtrace_rcv_data(socket->last_local_port, trace);
}

/*
 * vnsm_trace_send_call
 *
 * Get general information about this server.  General means name, key
 * number, and version.
 */
ushort vnsm_trace_send_call (
    ulong source,
    ulong destination)
{
    vinesaddress *address, space;
    vinesipcsocket *socket;
    vnsm_trace_query *query;
    vipc_port_info *port;

    query = malloc(sizeof(vnsm_trace_query));
    if (query == 0)
	return(0);
    socket = vipc_create_conn(source, VINES_SERVER, 0, 0, TRUE);
    if (socket == NULL)
	return(0);
    port = vipc_create_port(0, "Exec Trace Command", FALSE,
			    vnsm_rcv_trace_data);
    if (port == NULL)
	return(0);
    address = vsvc_create_address(&space, port->number);

    query->version = VNSM_VERSION_1;
    memset(&query->for_whom, 0, sizeof(vinesnonce));
    query->destination = destination;
    query->timestamp = vss_get_time();
    query->sequence = 0;
    bcopy(address, &query->response_port, sizeof(vinesaddress));
    vnsm_trace_send_call_(socket, query);
    return(port->number);
}

/*
 * vnsm_trace_rcv_call
 *
 * Get general information about this server.  General means name, key
 * number, and version.
 */
ushort vnsm_trace_rcv_call  (
    vinesipcsocket *socket,
    vnsm_trace_query *query,
    vnsm_trace_reply *reply)
{
    sys_timestamp wakeup;

    if (vnsm_debug) {
	buginf("\nVNSM:  user %s, dest %#Z, timestamp %08x, index %d",
	       vsvc_print_nonce(&query->for_whom), query->destination,
				query->timestamp, query->sequence);
    }

    /*
     * Queue the request for later processing.  This is necessary because
     * the router's application layer support is designed as a
     * query/response model, but the returned information must arrive in
     * a separate packet *after* the response.  This is only an issue
     * when this router is the first station in the trace, because that's
     * when the response and the data are being sent to the same place.
     */
    mem_lock(query);
    if (data_enqueue(&vnsm_traceQ, query) == NULL) {
	free(query);
	return(VNSM_ERR_NOMEM);
    }
    TIMER_START(wakeup, ONESEC);
    mgd_timer_set_soonest(&vsvc_nsm_info.timer, wakeup);

    /*
     * Finally, tell the caller that we are finished.
     */
    reply->version = VNSM_VERSION_1;
    reply->timestamp = query->timestamp;
    return(VSVC_NO_ERROR);
}

/*
 * vnsm_trace_rcv_reply
 *
 * Get general information about this server.  General means name, key
 * number, and version.
 */
void vnsm_trace_rcv_reply  (
    vinesipcsocket *socket,
    vnsm_trace_reply *reply)
{
}

/*
 * vnsm_trace_do_request
 *
 * The VINES code requires that the response message to a "Trace" call
 * be received before the actual information bearing message is
 * received.  This routine handles generating those delayed
 * information messages.
 */
static void vnsm_trace_do_request (void)
{
    vnsm_trace_query *query;
    vnsm_trace_query *query_fwd;
    vnsm_trace_other *info;
    vnsm_trace_route *info_route;
    vnsm_trace_neighbor *info_neighbor;
    vinespathtype *path;
    vinesneighbortype *neighbor;
    vinesroutetype *route;
    vinesnettype *server;
    routingdecision ihatethis;
    boolean result;
    vinesipcsocket *socket_fwd;
    idbtype *swidb;
    hwaddrtype hwaddr;

    while ((query = data_dequeue(&vnsm_traceQ)) != NULL) {
	/*
	 * In order to complete this request, we need to look up a route and
	 * get a buffer in which the data will be returned.  This particular
	 * query is unusual in that the information requested is non
	 * necessarily returned to the station that sent the call.  This is
	 * because the call is cascased through the network, but all of the
	 * data needs ro be sent to the originator.
	 */
	result = vines_first_hop(query->destination, VINES_SERVER,
				 &ihatethis);
	if (result == FALSE)
	    continue;
	server   = ihatethis.server;
	route    = ihatethis.route;
	neighbor = ihatethis.neighbor;
	path     = ihatethis.path;
	
	info = malloc(sizeof(vnsm_trace_other));
	if (info == NULL)
	    continue;
	info_route = &info->route;
	info_neighbor = &info->neighbor;
	
	/*
	 * O.K.  We have everything necessary to send our information to
	 * the originator of the query.  Do that much now.
	 */
	query->sequence++;
	if (neighbor->newflags & V_NET_ME) {
	    info_route->destination = neighbor->net;
	    info_route->metric = 0;
	    info_route->idle = 0;
	    info_route->flags = VNSM_TRACE_GNET_VALID;
	    info_route->gateway_net = 0;
	    info_route->preferred_net = 0;
	} else {
	    if (route && server) {
		info_route->destination = server->net;
		info_route->metric = vmetric_to_srtp(server->adv_metric);
		info_route->idle = 0;
		info_route->flags = VNSM_TRACE_GNET_VALID;
		info_route->gateway_net = route->gw->net;
		info_route->preferred_net = 0;
	    } else {
		info_route->destination = neighbor->net;
		info_route->metric = vmetric_to_srtp(neighbor->metric);
		info_route->idle = 0;
		info_route->flags = VNSM_TRACE_GNET_VALID;
		info_route->gateway_net = neighbor->net;
		info_route->preferred_net = 0;
	    }
	    
	    info_neighbor->lnetid = vines_network;
	    swidb = path->idb->phys_idb;
	    if ((swidb->hwptr->extract_hwaddr)(swidb->hwptr, &hwaddr)) {
		/* Copy 6 bytes, indicate how many are actually valid */
		ieee_copy(path->dlc_addr.addr, info_neighbor->neighbor_addr);
		ieee_copy(hwaddr.addr, info_neighbor->interface_addr);
		info_neighbor->sig_bytes = min(hwaddr.length, IEEEBYTES);
	    } else {
		info_neighbor->sig_bytes = 0;
	    }
	    info_neighbor->network        = neighbor->net;
	    info_neighbor->host           = neighbor->host;
	    info_neighbor->neighbor_type  = VSRTP_NODE_ROUTER - neighbor->node;
	    info_neighbor->interface_type = vsvc_c2b_iftype(path->idb);
	    info_neighbor->slot           = path->idb->svc_slot;
	}
	info->timestamp = query->timestamp;
	info->sequence = query->sequence;
	vnsm_trace_send_other_(&query->response_port, info);
	
	/*
	 * Now we need to send the query on to the next router.
	 */
	if (query->destination != vines_network) {
	    socket_fwd = vipc_create_conn(neighbor->net, neighbor->host, 0, 0, TRUE);
	    if (!socket_fwd)
		continue;
	    query_fwd = malloc(sizeof(vnsm_trace_query));
	    if (query_fwd == NULL)
		continue;
	    bcopy(query, query_fwd, sizeof(vnsm_trace_query));
	    vnsm_trace_send_call_(socket_fwd, query_fwd);
	}
	free(query);
    }
}

/**************************************************
 *
 *            NSM - Search / ReturnAddress
 *
 **************************************************/

/*
 * vnsm_rcv_search
 *
 * A NetRPC search message has been received on the Network and Systems
 * Management port.
 */
void vnsm_rcv_search (
    vinesipcsocket *socket,
    ushort user_num,
    uchar *user_string,
    uchar *info1,
    uchar *info2,
    uchar *info3)
{
    /* Do nothing */
}

/*
 * vnsm_rcv_returnaddress
 *
 * A NetRPC returnaddress message has been received on the Network and
 * Systems Management port.
 */
void vnsm_rcv_returnaddress (
    vinesipcsocket *socket,
    ushort proto_rev,
    ulong  service_rev,
    vinesaddress *service_address)
{
    /* Do nothing */
}


/**********************************************************************
 *
 *
 *
 **********************************************************************/

void vnsm_report_error (vinesipcsocket *socket)
{
    char port_string[20];
    vinesaddress address;
    vnsm_send_addr *addr;
    dqueue_t *p;

    memset(&address, 0, sizeof(vinesaddress));
    address.family = VINES_ADDR_NATIVE;
    PUTLONG(address.f.vip.nnet, socket->remote_net);
    address.f.vip.host = socket->remote_host;
    address.f.vip.port = socket->last_remote_port;

   /*
     * Find the record of this data port, dequeue it, and delete it.
     */
    RUN_DQUEUE(vnsm_send_list, p) {
	addr = p->parent;
	if (bcmp(&addr->address, &address, sizeof(vinesaddress)) == 0) {
	    if (vnsm_debug) {
		vines_printf_address(port_string, &address);
		buginf("\nVNSM: Removing port %s due to IPC message.", port_string);
	    }
	    lw_remove(p);
	    free(p->parent);
	    break;
	}
    }
}

/*
 * vnsm_init
 *
 * Boot time initializion of information that is maintained by the
 * Network and Systems Management Service.  This routine will only ever
 * be called once by the router.
 */
void vnsm_init (void)
{
    vsvc_install_svc(&vsvc_nsm_info);
    queue_init(&vnsm_traceQ, 0);
    o_init(&vnsm_send_list, NULL);
    o_init(&vnsm_pstk_passQ, NULL);
}

/*
 * vnsm_start
 *
 * Run time initialize of information that is maintained by the Network
 * and Systems Management Service.  This routine will be called each time
 * the 'vines routing' command is issued.
 */
void vnsm_start (void)
{
}

/*
 * vnsm_stop
 *
 * Purge all information that is maintained by the Network and Systems
 * Management Service.  This routine will be called each time the 'no
 * vines routing' command is issued.
 */
void vnsm_stop (void)
{
    vnsm_trace_query *query;
    dqueue_t *ptr, *next;

    while ((query = data_dequeue(&vnsm_traceQ)) != NULL)
	free(query);
    RUN_DQUEUE_W_NEXT(vnsm_send_list, ptr, next) {
	lw_remove(ptr);
	free(ptr->parent);
    }
    RUN_DQUEUE_W_NEXT(vnsm_pstk_passQ, ptr, next) {
	lw_remove(ptr);
	free(ptr->parent);
    }
}

void vnsm_periodic (void)
{
    if (vnsm_traceQ.count)
	vnsm_trace_do_request();
    vnsm_svrsummstats_send_data();
}

void vnsm_display (void)
{
    vnsm_pstk_pass_info *pass;
    vnsm_send_addr *addr;
    char space[20];
    dqueue_t *p;
    boolean first;

    first = TRUE;
    RUN_DQUEUE(vnsm_send_list, p) {
	if (first) {
	    printf("\n  Sending periodic information to:");
	    first = FALSE;
	}
	addr = p->parent;
	printf("\n    %s", vsvc_sprintf_address(space, &addr->address));
    }

    first = TRUE;
    RUN_DQUEUE(vnsm_pstk_passQ, p) {
	if (first) {
	    printf("\n  Pass Information:");
	    first = FALSE;
	}
	pass = p->parent;
	printf("\n    %#15z %d %d %d %d %#15z", pass->req_net, pass->req_host,
	       pass->family, pass->protocol, pass->table, pass->iteration,
	       pass->last_net, pass->last_host);
    }
}
