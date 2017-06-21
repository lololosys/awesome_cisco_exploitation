/* $Id: lane_util.c,v 3.3.42.9 1996/08/28 12:54:50 thille Exp $
 * $Source: /release/112/cvs/Xsys/lane/lane_util.c,v $
 *------------------------------------------------------------------
 * lane_util.c -- LAN Emulation utility functions
 *
 * October 1994  Brad Benson
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lane_util.c,v $
 * Revision 3.3.42.9  1996/08/28  12:54:50  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.42.8  1996/07/30  18:20:01  bbenson
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * First commit of this ddts omitted registry calls out of network.c into
 * the lane subsys.
 *
 * Revision 3.3.42.7  1996/07/30  14:42:49  michellw
 * CSCdi64226:  Support larger max_frame_size (>1516) for LANE
 * Branch: California_branch
 * - The max_frame_size will be set to 1516 for ethernet and 4544 for TR
 *   if mtu is not configured on the sub-interface; otherwise, it will be
 *   set to based on the configured mtu.
 * - The lane module will not come up if the configured mtu is not valid.
 * - Add one more parameter (swidb) to the reg_invoke_media_change_mtu
 *   registry function.
 * - 1577 will come up with the default AIP mtu size (4470) if mtu is not
 *   configured on the sub-interface.
 *
 * Revision 3.3.42.6  1996/05/31  23:24:10  cakyol
 * CSCdi58685:  router crashed when LS100 system call errors occured.
 *              this really had nothing to do with the atm switch.
 *              it was all a coincidence.  forgot locking the elements
 *              while printing them.  this is a much better fix.
 * Branch: California_branch
 *
 * Revision 3.3.42.5  1996/05/30  21:32:50  rbadri
 * CSCdi59101:  LE-ARP entry should use 6 byte MAC address instead of
 * hwaddrtype
 * Branch: California_branch
 * Changed the le_arp structure and all the associated functions to use
 * 6 byte MAC address instead of using 24 byte hwaddrtype to store 6
 * bytes.
 *
 * Revision 3.3.42.4  1996/05/25  07:16:26  cakyol
 * CSCdi58732:  automore processing produces spurious output.
 *
 *              "ttyprintf" in "atm_related_printf" should be
 *              "_ttyprintf" since the former turns automore
 *              processing off.
 *
 * Branch: California_branch
 *
 * Revision 3.3.42.3  1996/05/09  14:32:42  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.42.2.2.1  1996/04/27  07:06:35  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.42.2  1996/04/25  23:14:40  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.42.1.8.6  1996/04/16  02:44:33  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.5  1996/04/11  18:19:59  cakyol
 * amendments to the latest sync:
 * - take some unnecessary crashdump's out of lane server.
 * - correct possible input buffer loss at LECS input processing
 *   for token ring lan type.
 * - debugging messages needed tidying up.
 * - some but not all white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.4  1996/04/11  05:33:37  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.3.42.1.8.3  1996/04/09  02:16:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.3.42.1.8.2  1996/04/08  14:51:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.42.1.8.1  1996/04/08  01:57:00  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.42.1  1996/03/18  20:32:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.46.3  1996/04/05  21:29:01  cakyol
 * Combine all the special atm printing routines into a
 * single one (atm_related_printf), saving code space.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.46.2  1996/04/05  03:20:28  cakyol
 * started using the %Cx Cisco extensions to printf.  These are:
 *
 *   j: uchar*, atm RAW bytes.
 *
 *   h: hwaddrtype*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 *   w: MaskedAddress*, where type is either STATION_IEEE48 or
 *      STATION_ATMNSAP.
 *
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.46.1  1996/03/22  22:54:43  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.32.1  1996/03/05  06:43:04  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.22.1  1996/02/27  21:10:24  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3.2.6  1996/02/22  21:08:15  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.5  1996/02/06  22:19:23  cakyol
 * cleanup for the last code review:
 * - get rid of "#ifdef PARANOID...#endif".  Leave some in
 *   permanently, take some out completely.
 * - get rid of all "if 0....endif" type constructs.
 * - debug messages speeded up by using variable argument
 *   macro expansion.
 * - most fatal type errors are converted to crashdump with
 *   __FILE__ and __LINE__ printing.
 * - in the LECS, do *NOT* update the table of global LECS
 *   addresses, if any outstanding signalling requests remain.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.4  1996/01/30  23:56:23  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.3  1996/01/29  22:06:01  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_api_callId2CallingPartyAddress   and
 *     atmSig_api_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.2.2.2  1996/03/23  21:13:02  kjadams
 * Branch: TRlane_branch
 * LES/BUS support for token ring LANE has been added.
 * TR LEC now has a new state for registration - to register the virtual
 *    ring number and bridge (route descriptor) with the LES.
 * BLLI code points for token ring have also been added for TR BUS
 *    connections.
 *
 * Revision 3.3.2.2.2.1  1996/02/26  23:07:25  ddecapit
 * Branch: TRlane_branch
 *
 * Revision 3.3.2.2  1996/01/17  20:26:22  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3.2.1  1996/01/11  01:46:25  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.3  1996/01/05  04:56:08  hampton
 * Remove extraneous includes of h/timer.h.  [CSCdi46482]
 *
 * Revision 3.2.10.1  1995/12/29  01:06:44  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  17:50:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:23:13  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/09/01  00:59:34  shj
 * CSCdi39584:  set sdu sizes per connection - part 1
 * Mods for LES/BUS setting sdu size in SETUP's.
 *
 * Revision 2.4  1995/08/06  21:59:45  bbenson
 * CSCdi36725:  LEC needs to obtain LECS address from the switch.
 * - Locally implement LECS address determination (not using cfgacc).
 * - Use interface_admin_down instead of interface_up_simple.
 *
 * Revision 2.3  1995/06/30  22:02:13  bbenson
 * CSCdi36650:  LEC Join Failure error code could be more descriptive
 * - Removed coercion of mallocs in the bargain.
 *
 * Revision 2.2  1995/06/22  01:20:45  cakyol
 * CSCdi36146:  LECS warns of failed ILMI registration, when ILMI PVC
 * missing
 *
 * fixed.  The warnings now come out ONLY if an ILMI pvc exists.
 * Otherwise, they appear ONLY if debugging is turned on.
 *
 * As a by product of fixing this, a new function named
 * "atm_get_specific_vc" is offered.  This returns a vc info,
 * given an hwidb, a vpi and a vci.
 *
 * Revision 2.1  1995/06/07  21:19:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#undef   toupper			/* use library function call */
#include "mgd_timers.h"
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "config.h"
#include "parser.h"
#include "address.h"
#include "stdarg.h"
#include "../ui/debug.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */
#include "../if/ether.h"
#include "../if/tring.h"
#include "../if/atm.h"
#include "../if/network.h"
#include "../if/atm_actions.h"
#include "../atm/atmsig_api.h"
#include "../atm/ilmi_api.h"

#include "lane.h"
#include "lane_private.h"
#include "lane_debug.h"
#include "lane_signal.h"
#include "lec_private.h"
#include "lec_fsm.h"
#include "lec_pdu.h"

/* static sprintf buffer */
static char lane_msg_buf [256];

/*
** generic debug routine
*/
void lane_debug (int type, char *ifname, char *fmt, ...)
{
    va_list args;
    uint p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;
    char *typeString;

    va_start(args, fmt);
    p0 = va_arg(args, uint);
    p1 = va_arg(args, uint);
    p2 = va_arg(args, uint);
    p3 = va_arg(args, uint);
    p4 = va_arg(args, uint);
    p5 = va_arg(args, uint);
    p6 = va_arg(args, uint);
    p7 = va_arg(args, uint);
    p8 = va_arg(args, uint);
    p9 = va_arg(args, uint);
    switch (type) {
    case _LECS_EVENT_DBG: 
	typeString = "LECS EVENT"; break;
    case _LECS_PACKET_DBG: 
	typeString = "LECS PACKET"; break;
    case _LECS_FINDER_DBG: 
	typeString = "LECS FINDER"; break;
    case _LES_EVENT_DBG:
	typeString = "LES"; break;
    case _BUS_EVENT_DBG:
	typeString = "BUS"; break;
    case _LEC_EVENT_DBG:
	typeString = "LEC EVENT"; break;
    default: 
	typeString = "UNKNOWN"; break;
    }
    buginf("\n%s", typeString);
    if (ifname != NULL) buginf(" %s", ifname);
    buginf(": ");
    sprintf(lane_msg_buf, fmt, p0, p1, p2, p3, p4, 
	p5, p6, p7, p8, p9);
    buginf("%s", lane_msg_buf);
    va_end(args);
}

/*
** generic more serious errors routine
*/
void lane_error (const message *msg, const char *fmt, ...)
{ 
    va_list args;
    uint p0, p1, p2, p3, p4, p5, p6, p7, p8, p9;

    va_start(args, fmt);
    p0 = va_arg(args, uint);
    p1 = va_arg(args, uint);
    p2 = va_arg(args, uint);
    p3 = va_arg(args, uint);
    p4 = va_arg(args, uint);
    p5 = va_arg(args, uint);
    p6 = va_arg(args, uint);
    p7 = va_arg(args, uint);
    p8 = va_arg(args, uint);
    p9 = va_arg(args, uint);
    sprintf(lane_msg_buf, fmt, p0, p1, p2, p3, p4, 
	p5, p6, p7, p8, p9);
    errmsg(msg, lane_msg_buf);
    va_end(args);
}

/*
 * returns (if any) a vc with the specified vpi & vci on the
 * requested interface.  If none exists, returns NULL
 */
vc_info_t *atm_get_specific_vc (hwidbtype *hwidb, unsigned vpi, 
    unsigned vci)
{
    if (hwidb == NULL) return NULL;
    if (hwidb->atm_db == NULL) return NULL;
    return atm_vpivci_to_vc(hwidb->atm_db, vpi, vci);
}


/*
 * lane_addr_valid
 *
 * Returns TRUE if address is a valid LANE address
 */
boolean lane_addr_valid (hwaddrtype *addr)
{
    return((addr->type    == STATION_ATMNSAP)     &&
	   (addr->length  == STATIONLEN_ATMNSAP)  &&
	   (addr->addr[0] != 0)			     );
}


/*
 * lane_addr_hash
 *
 * Returns the hash value given a hwaddrtype.
 */
unsigned lane_addr_hash (hwaddrtype *addr)
{
    unsigned hash;
    int length;

    for (hash = 0, length = addr->length-1; 
	 length >= 0; 
	 length--) {
	hash ^= addr->addr[length];
    }
    return(hash & 0xFF);
}

unsigned lane_mac_hash (uchar *mac)
{
  return (mac[IEEEBYTES-1] ^ mac[IEEEBYTES-2]);
}

/*
 * lane_addr_equal
 *
 * Compares two hwaddrtype structs and returns the result.
 */
boolean lane_addr_equal (hwaddrtype *addr1, hwaddrtype *addr2)
{

    if ((addr1->type != addr2->type) ||
	(addr1->length != addr2->length)) {
	return(FALSE);
    }

    if (memcmp(addr1->addr, addr2->addr, addr1->length)) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}



/*
 * lane_hdr_check
 *
 * Checks the LANE control header for consistency.
 */
boolean lane_hdr_check (lane_ctl_hdr_t *hdr)
{
    if ((hdr->protocol != LANE_PROTOCOL_ID) || 
	(hdr->version != LANE_PROTOCOL_VERSION) ||
	((hdr->marker & LANE_MARKER_MASK) != LANE_MARKER_MASK)) {
	return(FALSE);
    } else {
	return(TRUE);
    }
}

/*
 * lane_pr_opcode
 *
 * Give a pointer to the lane control header, return a string
 * representing the packet type.
 */
char *lane_pr_opcode (lane_ctl_hdr_t *hdr)
{
    switch (hdr->opcode) {
    case LANE_CONFIG_REQ:	return("LANE_CONFIG_REQ");
    case LANE_CONFIG_RSP:	return("LANE_CONFIG_RSP");
    case LANE_JOIN_REQ:		return("LANE_JOIN_REQ");
    case LANE_JOIN_RSP:		return("LANE_JOIN_RSP");
    case LANE_READY_QUERY:	return("LANE_READY_QUERY");
    case LANE_READY_IND:	return("LANE_READY_IND");
    case LANE_REG_REQ:		return("LANE_REG_REQ");
    case LANE_REG_RSP:		return("LANE_REG_RSP");
    case LANE_UNREG_REQ:	return("LANE_UNREG_REQ");
    case LANE_UNREG_RSP:	return("LANE_UNREG_RSP");
    case LANE_ARP_REQ:		return("LANE_ARP_REQ");
    case LANE_ARP_RSP:		return("LANE_ARP_RSP");
    case LANE_FLUSH_REQ:	return("LANE_FLUSH_REQ");
    case LANE_FLUSH_RSP:	return("LANE_FLUSH_RSP");
    case LANE_NARP_REQ:		return("LANE_NARP_REQ");
    case LANE_TOPO_CHANGE:	return("LANE_TOPO_CHANGE");
    default:			return("UNKNOWN");
    }
}

/*
 * lane_pr_lan_type
 *
 * Returns string associated with the lan type.
 */
char *lane_pr_lan_type (uchar type)
{
    switch (type) {
    case LANE_TYPE_UNSPECIFIED:	return("unspecified");
    case LANE_TYPE_802_3:	return("ethernet");
    case LANE_TYPE_802_5:	return("token ring");
    default:			return("unknown");
    }
}

/*
 * lane_pr_max_frame_size
 *
 * Returns a string representing the max frame size of the elan.
 */
char *lane_pr_max_frame_size (uchar max_frame_size)
{
    switch (max_frame_size) {
    case LANE_MAX_FRAME_UNSPECIFIED:	return("unspecified");
    case LANE_MAX_FRAME_1516:		return("1516");
    case LANE_MAX_FRAME_4544:		return("4544");
    case LANE_MAX_FRAME_9234:		return("9234");
    case LANE_MAX_FRAME_18190:		return("18190");
    default:				return("unknown");
    }
}

/*
 * lane_frame_to_sdu_size
 *
 * Return aal6 sdu size to use for a given max frame size.
 */
unsigned
lane_frame_to_sdu_size(uchar max_frame_size)
{
    switch (max_frame_size) {
    case LANE_MAX_FRAME_1516:
    default:
        return(LANE_SDU_SIZE_1516);
    case LANE_MAX_FRAME_4544:
        return(LANE_SDU_SIZE_4544);
    case LANE_MAX_FRAME_9234:
        return(LANE_SDU_SIZE_9234);
    case LANE_MAX_FRAME_18190:
        return(LANE_SDU_SIZE_18190);
    }
}

/*
 * lane_pr_status
 * Return a string associated with the LANE control frame status
 * value.
 */
char *lane_pr_status (int status)
{
    switch (status) {
    case LANE_STATUS_SUCCESS:		return("success");
    case LANE_STATUS_VERSION_NOT_SUP:	return("version not supported");
    case LANE_STATUS_INVALID_REQ_PARMS:	return("invalid request parameters");
    case LANE_STATUS_DUP_LAN_DEST:	return("duplicate lan destination");
    case LANE_STATUS_DUP_ATM_ADDR:	return("duplicate atm address");
    case LANE_STATUS_INSUFF_RESOURCE:	return("insufficient resources");
    case LANE_STATUS_ACCESS_DENIED:	return("access denied");
    case LANE_STATUS_INVALID_REQ_LECID:	return("invalid lecid");
    case LANE_STATUS_INVALID_LAN_DEST:	return("invalid lan destination");
    case LANE_STATUS_INVALID_ATM_ADDR:	return("invalid atm address");
    case LANE_STATUS_NO_CONFIG:		return("no configuration");
    case LANE_STATUS_LE_CONFIG_ERROR:	return("configuration error");
    case LANE_STATUS_INSUFF_INFO:	return("insufficient information");
    default:				return("unknown");
    }
}

/*
 * printable string for "lane_addr_recycle_reason"
 */
char *lane_pr_addr_recycle_reason (lane_addr_recycle_reason reason)
{
    switch (reason) {
    case PREFIX_ADD_EVENT:		return "prefix add";
    case PREFIX_DELETE_EVENT:		return "prefix delete";
    case MAC_ADDRESS_CHANGE_EVENT:	return "mac address change";
    default:				return "UNKNOWN";
    }
}

/*
 * increments the least significant 3 bytes of a mac address
 * by the specified increment and returns the new value in
 * dstmac.  Does NOT touch the company id part.  Overflows
 * will wrap around within the three bytes.
 */
void increment_macaddress_by (uchar *srcmac, uchar *dstmac, int increment)
{
    int value;

    /* copy the oui part */
    dstmac [0] = srcmac [0];
    dstmac [1] = srcmac [1];
    dstmac [2] = srcmac [2];

    /* extraxt out the last 3 bytes to increment */
    value = srcmac [5];
    value |= (srcmac [4] << 8);
    value |= (srcmac [3] << 16);
    value += increment;

    /* now put them back in */
    dstmac [5] = value & 0xff;
    dstmac [4] = (value>>8) & 0xff;
    dstmac [3] = (value>>16) & 0xff;
}

/******************************************************************
**		Masked nsap address parsing and
**		helper routines
**
** 	this USED to be taken from clns routines
**	but is NOW home grown.  It is not as sophisticated
**	as the clns ones, but allows "..." and wildcarding
**	a nibble with a "*".  More importantly, it removes
**	our dependency on clns AND is more reliable in parsing
**	odd number of nibbles.
******************************************************************/

static char hexChars [] = "0123456789ABCDEF";

/*
** converts binary bytes into printable form in string hexString.
** Note that the capacity of hexString must be at least 2*binaryCount.
** The string will be NULL terminated.
*/
void binaryBytes2hexString (uchar *binaryBytes, int binaryCount, 
    char *hexString)
{
    uchar value;

    while (binaryCount-- > 0)
    {
	value = *binaryBytes++;
	*hexString++ = hexChars [value >> 4];
	*hexString++ = hexChars [value & 0xF];
    }
    *hexString = (uchar) 0;
}

/* reverse of the above */
void hexString2binaryBytes (char *hexString, int hexCount, 
    uchar *binaryBytes)
{
    while (hexCount-- > 0)
    {
	*binaryBytes = 0;
	*binaryBytes = hexCharValue(*hexString++) << 4;
	*binaryBytes |= hexCharValue(*hexString++);
	binaryBytes++;
    }
}

/* is this mask ALL zeros ? */
boolean allZeroMask (uchar *mask, int count)
{
    /* a NO mask means EVERYTHING is significant */
    if (mask == NULL) return FALSE;

    /* do long words as much as you can */
    while (count >= sizeof (ulong))
    {
	if (GETLONG (mask) != (ulong) 0) 
	    return FALSE;
	mask += sizeof (ulong);
	count -= sizeof (ulong);
    }

    /* do the remainder byte by byte */
    while (count-- > 0)
	if (*mask++ != (uchar) 0)
	    return FALSE;
    
    return TRUE;
}

/* is this mask ALL ones ? */
boolean allOnesMask (uchar *mask, int count)
{
    /* a NO mask means EVERYTHING is significant */
    if (mask == NULL) return TRUE;

    /* do long words as much as you can */
    while (count >= sizeof (ulong))
    {
	if (GETLONG (mask) != (ulong) 0xFFFFFFFF) 
	    return FALSE;
	mask += sizeof (ulong);
	count -= sizeof (ulong);
    }

    /* do the remainder byte by byte */
    while (count-- > 0)
	if (*mask++ != (uchar) 0xFF)
	    return FALSE;

    return TRUE;
}

#define LEN	(STATIONLEN_ATMNSAP * 2)
#define SZ	(LEN + 3)

typedef struct storage
{
    char data [SZ];
    char mask [SZ];
    int count;
} Storage;

static int insertData (Storage *sto, char dataChar, char maskChar, int *total)
{
    if (*total >= LEN) return TRUE;
    sto->data [sto->count] = dataChar;
    sto->mask [sto->count] = maskChar;
    sto->count++;
    (*total)++;
    return FALSE;
}

char *atm_masked_nsap_short_help (parseinfo *csb, uint flag, idbtype *idb)
{ return (""); }

boolean match_masked_atm_nsap (parseinfo *csb, char *buf, int *pi, 
    void *addr_pointer, int in_help, uint flag, idbtype *idb)
{
    MaskedAddress *maddr = (MaskedAddress*) addr_pointer;
    char c;
    int totalChars;
    int incharIndex;
    boolean threeDotsParsed;
    int origIncharIndex;
    Storage frontend, backend;
    Storage *current;
    int consecutiveDots;
    int i, j;
    int errorOccured = 0;
    char finalData [SZ];
    char finalMask [SZ];

    /* initialize everything */
    for (i=0; i<SZ; i++) {
	frontend.data [i] = backend.data [i] = '0';
	frontend.mask [i] = backend.mask [i] = 'F';
	finalData [i] = '0';
	finalMask [i] = 'F';
    }
    frontend.count = backend.count = 0;
    origIncharIndex = incharIndex = *pi;
    threeDotsParsed = FALSE;
    current = &frontend;
    totalChars = 0;
    consecutiveDots = 0;

    /****************** MORE TO DO *********************/
    /* handle 0x45 E.164 expansion */
    /*
    if ((buf [incharIndex] == '4') && (buf [incharIndex+1] == '5')) {
	special parsing for E.164 address
    }
    */

    /* start parsing */
    do {
	c = toupper(buf [incharIndex]);
	if (c == '*') {
	    /* ".*" and "...*" are ok but "..*" is NOT */
	    if (consecutiveDots == 2) goto ERROR;
	    if (insertData(current, '0', '0', &totalChars)) goto ERROR;
	    incharIndex++;
	    consecutiveDots = 0;
	} else if (isxdigit(c))	{
	    /* ".F" and "...F" are ok but "..F" is NOT */
	    if (consecutiveDots == 2) goto ERROR;
	    if (insertData(current, c, 'F', &totalChars)) goto ERROR;
	    incharIndex++;
	    consecutiveDots = 0;
	} else if (c == '.') {
	    /* either three dots or a nibble separator */
	    /* if three dots is already seen, this is not acceptable */
	    if ((consecutiveDots >= 1) && threeDotsParsed) goto ERROR;
	    consecutiveDots++;
	    incharIndex++;

	    /*
	    ** three consecutive dots seen, switch to filling the
	    ** characters from the back end and note it down
	    */
	    if (consecutiveDots >= 3) {
		threeDotsParsed = TRUE;
		current = &backend;
	    }
	} else if (isspace(c) || (c==0) || (c=='\n') || (c=='\r')) {
	    /* a terminating character encountered, we should exit */
	    /* we cannot exit empty handed EXCEPT for "..." only */
	    if ((totalChars <= 0) && !threeDotsParsed) goto ERROR;

	    /*
	    ** we cannot leave dangling dots at the end
	    ** UNLESS it was the threeDotsParsed we just parsed
	    */
	    if ((consecutiveDots > 0) && (consecutiveDots <= 2)) {
		incharIndex -= consecutiveDots;
		goto ERROR;
	    }
	    goto FINISH;
	} else {
	    goto ERROR;
	}
    } while (1);

ERROR:
    errorOccured = 1;

FINISH:
    /* update consumed character count */
    (*pi) += (incharIndex - origIncharIndex);

    /* if an error occured, no need to proceed more */
    if (errorOccured) return FALSE;

    /* construct the final data & mask strings */
    if (threeDotsParsed) {
	for (i=0; i<SZ; i++) finalMask [i] = '0';
	j = LEN - backend.count;
	for (i=0, j=LEN-backend.count; i<backend.count; i++, j++) {
	    finalData [j] = backend.data [i];
	    finalMask [j] = backend.mask [i];
	}
    }
    for (i=0; i<frontend.count; i++) {
	finalData [i] = frontend.data [i];
	finalMask [i] = frontend.mask [i];
    }

    if (lane_nsap_debug) {
	printf("\n****** internal final nsap char string ******\n");
	finalData [LEN] = 0;
	finalMask [LEN] = 0;
	printf("data: %s\n", finalData);
	printf("mask: %s\n", finalMask);
	printf("****** end internal final nsap char string ******\n");
    }

    /* convert to hex numbers and insert into the MaskedAddress */
    hexString2binaryBytes(finalData, LEN, maddr->dataBytes);
    hexString2binaryBytes(finalMask, LEN, maddr->maskBytes);
    maddr->type = STATION_ATMNSAP;
    maddr->length = STATIONLEN_ATMNSAP;

    return TRUE;
}

/*
** analyze the mask of an address, and print it out taking
** this into account. buf must have at least ATM_MAX_NSAP_STRING
** bytes capacity.
*/
static void sprintfNsapAsUserTypedIt (char buf [], uchar *binaryData, 
    uchar *binaryMask)
{
    int longest, longestStart;
    int currentLongest, currentLongestStart;
    int i, j;
    char data [SZ];
    char mask [SZ];

/* this HAS to be > LEN */
#define UNSET	LEN+10

    /*
    ** if all ones mask (this includes NULL pointer), 
    ** then use the standard routine.
    */
    if (allOnesMask(binaryMask, STATIONLEN_ATMNSAP))
    {
	atm_printnsap(binaryData, buf);
	return;
    }

    /* convert to hex characters first */
    binaryBytes2hexString(binaryData, STATIONLEN_ATMNSAP, data);
    binaryBytes2hexString(binaryMask, STATIONLEN_ATMNSAP, mask);

    /* find the longest insignificant stretch start and its length */
    longest = currentLongest = 0;
    longestStart = currentLongestStart = UNSET;
    for (i=0; i<LEN; i++) {
	if (mask [i] == '0') {
	    if (currentLongestStart == UNSET) {
		currentLongestStart = i;
		currentLongest = 1;
	    } else {
		currentLongest++;
	    }
	} else {
	    if (currentLongest > longest) {
		longest = currentLongest;
		longestStart = currentLongestStart;
	    }
	    currentLongest = 0;
	    currentLongestStart = UNSET;
	}
    }
    /* do this again for the last byte */
    if (currentLongest > longest)
    {
	longest = currentLongest;
	longestStart = currentLongestStart;
    }

    /* sprintf it */
    i = j = 0;
    while (i<LEN) {
	if (i==longestStart) { 
	    buf [j++] = '.'; buf [j++] = '.'; buf [j++] = '.';
	    i += longest;
	} else if (mask [i] == '0') { 
	    buf [j++] = '*'; i++;
	} else { 
	    buf [j++] = data [i]; i++;
	}
    }
    /* null terminate for printing */
    buf [j] = 0;
}

/***************************************************************
****************************************************************
**		atm related printf formats
****************************************************************
***************************************************************/

/*
** %C (Cisco special characters) followed by:
**
**    j: raw atm bytes	(uchar*)
**    h: hwaddrtype	(hwaddrtype*)
**    w: masked address	(MaskedAddress*)
*/
int atm_related_printf (const char **fmt, char **cp, tt_soc *tty, 
    va_list *argp, boolean rjust, int width, int size, 
    int hash, int precision)
{
    void *argument;
    uchar *atmaddr;
    hwaddrtype *hwaddr;
    MaskedAddress *maddr;
    int charcount, length, d;
    char buf [64];
    char *fmtPtr, fmtChar;

    charcount = 0;
    argument = va_arg(*argp, void*);
    if (argument == NULL)
    {
	sprintf(buf, "NULL");
	goto CONTINUE;
    }
    atmaddr = argument;
    hwaddr = argument;
    maddr = argument;

    /*
    ** Format according to the character.
    ** Note that the actual format character is one 
    ** before the current format pointer
    */
    fmtPtr = *((char**) fmt);
    fmtPtr--;
    fmtChar = *fmtPtr;
    switch (fmtChar) {

    /* raw atm bytes */
    case 'j':
	atm_printnsap(atmaddr, buf);
	break;

    /* hwaddrtype */
    case 'h':
	if (hwaddr->length <= 0) {
	    sprintf(buf, "UNKNOWN/UNSET");
	} else {
	    if (hwaddr->type == STATION_IEEE48)
		sprintf(buf, "%e", hwaddr->addr);
	    else if (hwaddr->type == STATION_ATMNSAP)
		atm_printnsap(hwaddr->addr, buf);
	    else
		sprintf(buf, "UNKNOWN/UNSET");
	}
	break;

    /* masked address */
    case 'w':
	if (maddr->length <= 0) {
	    sprintf(buf, "UNKNOWN/UNSET");
	} else {
	    if (maddr->type == STATION_IEEE48) {
		sprintf(buf, "%e", maddr->dataBytes);
	    } else if (maddr->type == STATION_ATMNSAP) {
		sprintfNsapAsUserTypedIt
		    (buf, maddr->dataBytes, maddr->maskBytes);
	    } else if (maddr->type == STATION_ROUTEDESC) {
		uint segment;
		uint bridge; 

		segment = (((uint)(maddr->dataBytes[0]) << 4) |
		    (((uint)(maddr->dataBytes[1] & 0xF0)) >> 4));

		/* low order 4 bits */
		bridge = 0x0F & (maddr->dataBytes [1]);

		sprintf(buf, "segment: %d  bridge: %d", 
		    segment, bridge);
	    } else {
		sprintf(buf, "UNKNOWN/UNSET");
	    }
	}
	break;

    default:
	sprintf(buf, "unknown fmt char %d", (int) fmtChar);
    }

CONTINUE:

    length = strlen(buf);

    /* right justify spaces */
    if (rjust && (width != -1) && (length < width)) {
	for (d=0; d<(width-length); d++) {
	    doprintc(cp, tty, ' ');
	    charcount++;
	}
    }

    /* actual stuff */
    if (*cp)
	*cp += sprintf (*cp, "%s", buf);
    else
	_ttyprintf(tty, "%s", buf);
    charcount += length;

    /* left justify spaces */
    if (!rjust && (width != -1) && (length < width)) {
	for (d=0; d<(width-length); d++) {
	    doprintc(cp, tty, ' ');
	    charcount++;
	}
    }

    return charcount;
}

/********************************************************
**	generic point to point LANE connect routine
********************************************************/

/*
** generic connect to target atm address routine.
** returns the call id if successfull otherwise NULL.
*/
void *connect_to_target (idbtype *swidb, hwaddrtype *src,
    hwaddrtype *target, lane_vc_type_t vcType, 
    void (*callback)(void *))
{
    void *callId;
    sig_api_msg msg;
    int n;

    /* sanity */
    if (!swidb || !target || !callback) return NULL;

    callId = atmSig_api_allocate_call_id();
    if (callId == NULL) {
	LANE_ERR("connectToTarget: allocate call id failed");
	return NULL;
    }
    msg.link = NULL;
    msg.opcode = ATM_SETUP;
    msg.idb = swidb;
    msg.u.setup.multipoint = FALSE;
    msg.u.setup.endpoint_id = NULL;
    msg.u.setup.call_id = callId;
    msg.u.setup.sig_user_callback = callback;
    atm_params_init(&msg.u.setup.params);
    msg.u.setup.called_party = *target;
    msg.u.setup.flags = (ATM_VC_AAL5LANE | VC_FLAG_NOIDLETIME);
    msg.u.setup.user_vc_type = vcType;
    msg.u.setup.fwd_sdu_size = LANE_SDU_SIZE_1516;
    msg.u.setup.bwd_sdu_size = LANE_SDU_SIZE_1516;
 
    lane_sig_make_blli(&msg.u.setup.blli [0], vcType);
    for (n=1; n<ATM_MAX_BLLI_NUM; n++)
	msg.u.setup.blli [n].length = 0;
    if (src) {
	msg.u.setup.calling_party = *src;
    } else {
	msg.u.setup.calling_party.length = 0;
	msg.u.setup.calling_party.type = STATION_ILLEGAL;
    }

    /* pass the request to signalling */
    atmSig_handleUserRequest(&msg);

    return callId;
}

/*
 * lane_mtu_to_frame 
 *
 * This routine is a generic routine to be shared by
 * all the LANE modules to figure out the max data frame size
 * based on the swidb->sub_mtu.
 */
unsigned lane_mtu_to_frame (idbtype *swidb)
{
    switch (swidb->lane_info->type) {
    case LANE_TYPE_802_3:
	switch (swidb->sub_mtu) {
	case LANE_MTU_ETHER_1516:	return(LANE_MAX_FRAME_1516);
	case LANE_MTU_ETHER_4544:	return(LANE_MAX_FRAME_4544);
	case LANE_MTU_ETHER_9234:	return(LANE_MAX_FRAME_9234);
	case LANE_MTU_ETHER_18190:	return(LANE_MAX_FRAME_18190);
	default:			return(LANE_MAX_FRAME_UNSPECIFIED);
	}
	break;
    case LANE_TYPE_802_5:
	switch (swidb->sub_mtu) {
	case LANE_MTU_TR_1516:		return(LANE_MAX_FRAME_1516);
	case LANE_MTU_TR_4544:		return(LANE_MAX_FRAME_4544);
	case LANE_MTU_TR_9234:		return(LANE_MAX_FRAME_9234);
	case LANE_MTU_TR_18190:		return(LANE_MAX_FRAME_18190);
	default:			return(LANE_MAX_FRAME_UNSPECIFIED);
	}
	break;
    default:
	break;
    }
    return(LANE_MAX_FRAME_UNSPECIFIED);
}

/*
 * is_valid_lane_mtu
 *
 * Checks whether the mtu is valid for LANE.  The routine will be
 * invoked when mtu is changed, in mtu_command(), or LEC/LES/BUS 
 * is created, in lec_create_client() and lsv_create_lesbus().
 *
 * Currently, we can not support the max frame size 18190 bytes,
 * since, the aip driver only supports mtu up to 9188 bytes.
 */
boolean is_valid_lane_mtu (idbtype *swidb, ushort mtu)
{
    if (swidb->lane_info->type == LANE_TYPE_802_3) {
	if (mtu == LANE_MTU_ETHER_1516 ||
	    mtu == LANE_MTU_ETHER_4544 ||
	    mtu == LANE_MTU_ETHER_9234)
	    return(TRUE);

    } else if (swidb->lane_info->type == LANE_TYPE_802_5) {
	if (mtu == LANE_MTU_TR_4544 ||
	    mtu == LANE_MTU_TR_9234)
	    return(TRUE);
    }
    return(FALSE);
}	

/*
 * lane_default_frame_size
 *
 * The default frame size is 1516 for ethenet and
 * 4544 for token ring.
 */
unsigned lane_default_frame_size (idbtype *swidb)
{
    if (swidb->lane_info->type == LANE_TYPE_802_3)
	return(LANE_MAX_FRAME_1516);
    else
	return(LANE_MAX_FRAME_4544);
}


/*
 * lane_default_mtu
 *
 * The default mtu for LANE is 1500 for ethernet and 
 * 4464 for token ring.
 */
unsigned lane_default_mtu (idbtype *swidb)
{
    if (swidb->lane_info->type == LANE_TYPE_802_3)
	return(LANE_MTU_ETHER_1516);
    else
	return(LANE_MTU_TR_4544);
}


/*
 * lane_set_mtu
 *
 * Change the max MTU of the AIP to that of the emulated LAN.  This
 * should probably be collapsed into if/network.c, since there is a
 * command function called max_mtu_command and pt_set_vtyasync_mtu in
 * vtyasync.c which perform pretty much the same thing.
 */
boolean lane_set_maxmtu (idbtype *idb, int mtu_size)
{
    hwidbtype *hwidb;
    long      delta;
  
    hwidb = idb->hwptr;
    delta = mtu_size - idb->sub_mtu;
    idb->sub_mtu = mtu_size;

    /*
     * Set the maximum mtu on the hwidb, based on all the sub-mtu
     * values in the sub-interfaces.
     */
    set_max_interface_mtu(idb, delta);
    return(TRUE);
}




