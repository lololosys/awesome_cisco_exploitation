/* $Id: ieee.c,v 3.7.10.6 1996/08/03 23:26:58 fox Exp $
 * $Source: /release/112/cvs/Xsys/if/ieee.c,v $
 *------------------------------------------------------------------
 * ieee.c  --  IEEE 802.x definitions
 *
 * May 1993, David R. Hampton
 * Mostly taken from modules dated October 1986 and January 1988.
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ieee.c,v $
 * Revision 3.7.10.6  1996/08/03  23:26:58  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.7.10.5  1996/05/17  11:22:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.7.2.4  1996/05/05  23:09:35  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.7.2.3  1996/04/24  06:30:29  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.7.2.2  1996/04/03  14:37:32  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.7.2.1  1996/03/01  16:31:03  ppearce
 * CSCdi50498:  IBU modularity - Implement LLC subblock
 * Branch: IbuMod_Calif_branch
 *   Move LLC fields from idbtype struct to LLC subblock
 *
 * Revision 3.7.10.4  1996/05/10  01:38:24  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.7.10.3  1996/05/09  14:29:55  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.10.2.10.1  1996/04/27  07:03:45  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.10.2  1996/04/03  23:19:24  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.7.10.1.8.1  1996/04/08  01:54:35  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.10.1  1996/03/18  20:13:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.14.1  1996/03/22  09:37:14  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7  1996/02/26  06:53:00  dmccowan
 * CSCdi45288:  need appn configuration changes to support multilink ppp
 * Add configuration option for APPN over a dialer interface
 *
 * Revision 3.6.16.1  1996/03/05  06:13:47  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.12.1  1996/02/27  21:08:17  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/01/22  03:55:11  dmccowan
 * CSCdi45007:  LLC2 LCP negotiated over PPP when not wanted
 *
 * Revision 3.5.6.1  1996/01/11  01:45:24  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.5  1995/12/08  02:01:50  bcole
 * CSCdi45234:  NHRP: Dont refresh unused cache entries
 *
 * Keep track of which cache entries are used, and avoid refreshing unused
 * entries.  Fix holddown time for implicit cache entries.
 *
 * Use IANA defined SNAP OUI for NHRP.
 *
 * Revision 3.4  1995/12/07  22:41:48  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3.10.1  1995/12/23  03:59:22  dcheng
 * Branch: LE_Calif_branch
 * Added support for VTP.
 *
 * Revision 3.3  1995/11/17  09:27:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:50:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.7  1995/11/08  21:02:59  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.6  1995/10/19  19:23:52  ppearce
 * CSCdi40639:  CMNS uses wrong MAC address to open LLC2 session
 *   llc_clear_rii() not clearing RII bit correctly
 *
 * Revision 2.5  1995/10/18  05:32:11  pmorton
 * CSCdi40639:  CMNS uses wrong MAC address to open LLC2 session
 * Add idb_is_tokenring_like, and llc_clear_rii.
 *
 * Revision 2.4  1995/09/02  07:27:33  pmorton
 * CSCdi36509:  CMNS broken - using wrong MAC address and spurious XID
 * sent on llc2
 * Don't evaluate RII bit on source address, or manipulate RIF (if input
 * is ethernet).
 *
 * Revision 2.3  1995/06/20  19:58:26  pmorton
 * CSCdi35117:  X.25 restarts ignored over CMNS connection
 * Make sure to remove that pesky XID from input
 *
 * Revision 2.2  1995/06/13  03:28:14  alam
 * CSCdi35570:  Remove extraneous functions
 * Use llc_process_test and llc_process_xid for FDDI as well as ethernet
 * and FR.
 * Remove FDDI packet debug.
 *
 * Revision 2.1  1995/06/07  20:52:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "ieee.h"
#include "media_registry.h"
#include "fastswitch_registry.h"
#include "packet.h"
#include "interface_private.h"
#include "tring.h"
#include "rif.h"
#include "fddi.h"
#include "network.h"
#include "address.h"
#include "ether.h"
#include "../llc2/llc2.h"
#include "../llc2/llc2_sb.h"
#include "../wan/smds.h"


uchar const baddr[IEEEBYTES] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};
uchar const zeromac[IEEEBYTES] = {0x00,0x00,0x00,0x00,0x00,0x00};
uchar const zerosnap[SNAP_OUIBYTES] = {0x00,0x00,0x00};
uchar const cisco_snap[SNAP_OUIBYTES] = {0x00,0x00,0x0c};
uchar const tb1490_snap[SNAP_OUIBYTES] = {0x00,0x80,0xc2};
uchar const iana_snap[SNAP_OUIBYTES] = {0x00,0x00,0x5e};
    

/*
 *
 */
long sap2link (
    uchar sap)
{
    long value;

    switch (sap) {
      case SAP_IP:       return(LINK_IP);
      case SAP_EXTENDED: return(LINK_PROBE);
      case SAP_CLNS:     return(LINK_CLNS);		
      case SAP_SPAN:     return(LINK_IEEE_SPANNING);
      case SAP_NOVELL:   return(LINK_NOVELL);
      case SAP_3COM:     return(LINK_XNS);
      case SAP_IBMNM:    return(LINK_IBMNM);
      case SAP_CONS:     return(LINK_CMNS);
      case SAP_SDE:      return(LINK_SDE);
      case SAP_NETBIOS:  return(LINK_NBF);	  
    }
    value = reg_invoke_media_sap_to_link(sap);
    return(value ? value : LINK_ILLEGAL);
}

/*
 *
 */
long type2link (
    ushort type)
{
    long value;

    switch (type) {
      case TYPE_IP10MB:		return(LINK_IP);
      case TYPE_RFC826_ARP:	return(LINK_ARP);
      case TYPE_LAT:		return(LINK_LAT);
      case TYPE_LOOP:		return(LINK_LOOP);
      case TYPE_MOP_BOOT:	return(LINK_MOP_BOOT);
      case TYPE_MOP_CONSOLE:	return(LINK_MOP_CONSOLE);
      case TYPE_XEROX_ARP:	return(LINK_XARP);
      case TYPE_REVERSE_ARP:	return(LINK_RARP);
      case TYPE_DECNET:		return(LINK_DECNET);
      case TYPE_XNS:		return(LINK_XNS);
      case TYPE_NOVELL1:	return(LINK_NOVELL);
      case TYPE_DEC_SPANNING:	return(LINK_DEC_SPANNING);
      case TYPE_ETHERTALK:	return(LINK_APPLETALK);
      case TYPE_AARP:		return(LINK_AARP);
      case TYPE_APOLLO:		return(LINK_APOLLO);
      case TYPE_VINES:
      case TYPE_VINES2:		return(LINK_VINES);
      case TYPE_VINES_ECHO:
      case TYPE_VINES_ECHO2:	return(LINK_VINES_ECHO);
      case TYPE_VINES_LOOP:	return(LINK_VINES_LOOP);
      case TYPE_HP_PROBE:	return(LINK_PROBE);
      case TYPE_RSRB:		return(LINK_RSRB);
      case TYPE_L2_8022_PAD:	return(LINK_LLC2);
      case TYPE_CDP:		return(LINK_CDP);
      case TYPE_VTP:		return(LINK_VTP);
      case TYPE_CPP:		return(LINK_CPP);
    }
    value = reg_invoke_media_type_to_link(type);
    return(value ? value : LINK_ILLEGAL);
}

/*
 * link2snaptype
 *
 * Given a link type, return the SNAP type code
 */
ushort link2snaptype (
    long link)
{
    ulong value;

    switch (link) {
      case LINK_IP:             return(TYPE_IP10MB);
      case LINK_ARP:		return(TYPE_RFC826_ARP);
      case LINK_XARP:		return(TYPE_XEROX_ARP); 
      case LINK_RARP:		return(TYPE_REVERSE_ARP);
      case LINK_DECNET:	 	return(TYPE_DECNET);
      case LINK_DECNET_ROUTER_L1:  return(TYPE_DECNET);
      case LINK_DECNET_ROUTER_L2:  return(TYPE_DECNET);
      case LINK_DECNET_PRIME_ROUTER:  return(TYPE_DECNET);
      case LINK_DECNET_NODE:	return(TYPE_DECNET);
      case LINK_LAT:		return(TYPE_LAT);
      case LINK_LOOP:		return(TYPE_LOOP);
      case LINK_MOP_BOOT:	return(TYPE_MOP_BOOT);
      case LINK_MOP_CONSOLE:	return(TYPE_MOP_CONSOLE);
      case LINK_XNS:		return(TYPE_XNS);
      case LINK_NOVELL:	 	return(TYPE_NOVELL1);
      case LINK_BRIDGE:	 	return(TYPE_BRIDGE);
      case LINK_DEC_SPANNING:	return(TYPE_DEC_SPANNING);
      case LINK_AARP:		return(TYPE_AARP);
      case LINK_APPLETALK:	return(TYPE_ETHERTALK);
      case LINK_APOLLO:	 	return(TYPE_APOLLO);
      case LINK_VINES:	 	return(TYPE_VINES2);
      case LINK_VINES_ECHO:	return(TYPE_VINES_ECHO2);
      case LINK_VINES_LOOP:	return(TYPE_VINES_LOOP);
      case LINK_PROBE:	 	return(TYPE_HP_PROBE);
      case LINK_RSRB:		return(TYPE_RSRB);
      case LINK_CDP:		return(TYPE_CDP);
      case LINK_CGMP:		return(TYPE_CGMP);
      case LINK_VTP:		return(TYPE_VTP);
      default:
	value = reg_invoke_media_link_to_type(link);
	return(value ? value : 0L);
    }
}

/* ----------------------------------------------------------------
 *
 *			SAP REGISTRY
 *
 * Protoytypes
 */

static void sap_preregister(sap_registry_t *sapregistry);

/* 
 * Default SAP handling for well-known non-SNA SAPs
 *
 * Historical SAPs
 *
 * Kludgy to include IBM here, but it help the protocol go through 
 */
static const uchar predefined_saps[] = { 
    SAP_IBM_SNA,
    SAP_IP, 
    SAP_SPAN,
    SAP_CONS,
    SAP_3COM, 
    SAP_SNAP, 
    SAP_NOVELL,
    SAP_NETBIOS, 
    SAP_IBMNM,
    SAP_CLNS
};

static const predef_sap_count = sizeof(predefined_saps);

#define SAP_PREDEF_FLAGS  		\
	(SAP_TEST_AUTORESPONSE |	\
	 SAP_IEEE_XID_AUTORESPONSE | 	\
	 SAP_SNA_XID_AUTORESPONSE  |    \
         SAP_PRE_REG_SAP)

#define SAP_NULL_FLAGS			\
	(SAP_TEST_AUTORESPONSE |	\
	 SAP_IEEE_XID_AUTORESPONSE |    \
	 SAP_PRE_REG_SAP |              \
	 SAP_CLS_ACT_SAP)

static sap_entry_t sap_predef_entry =
{ 
    SAP_PREDEF_FLAGS,			/* sap_flags */
    NULL,				/* usapid */
    NULL,				/* llc_list */
    NULL,				/* sap_handle_test */
    NULL,				/* sap_handle_ieee_xid */
    NULL,				/* sap_handle_sna_xid */
    XID_CLASS_TYPE1,			/* xid_class */
    XID_WIN_TYPE1			/* xid_window */
};

static sap_entry_t sap_null_entry =
{ 
    SAP_NULL_FLAGS,			/* sap_flags */
    NULL,				/* usapid */
    NULL,				/* llc_list */
    NULL,				/* sap_handle_test */
    NULL,				/* sap_handle_ieee_xid */
    NULL,				/* sap_handle_sna_xid */
    XID_CLASS_TYPE1,			/* xid_class */
    XID_WIN_TYPE1			/* xid_window */
};

/*
 * sap_create_registry
 *
 * build the SAP registry and preregister all well-known SAPs
 */

sap_registry_t *sap_create_registry (idbtype *idb)
{
    sap_registry_t *sapregistry;

    sapregistry = malloc_named(sizeof(sap_registry_t), "IEEE SAP Registry");
    if (sapregistry != NULL) {
	sap_preregister(sapregistry);
	llc_sb_set_sap_registry(idb, sapregistry);
    }
    return(sapregistry);
}

/* sap_preregister
 *
 * Some users won't register SAPs for a number of years.
 * This will help them get by for the time being.
 */
 
static void sap_preregister (sap_registry_t *sap_registry)
{
    uint sap;
	
    for (sap = 0; sap <  predef_sap_count; sap++)
	sap_register(sap_registry, predefined_saps[sap], &sap_predef_entry);

    /* special handling for null SAP */
    sap_register(sap_registry, SAP_NULL, &sap_null_entry);
}

/*
 * sap_register()
 *
 * Add a sap to the registry. Fail, if there's already one there.
 * Returns 0 if all is well, -1 if in use or other error.
 */

int sap_register (sap_registry_t *sap_registry, 
		  uchar sap, 
		  sap_entry_t *entry)
{
    if (sap_registry == NULL) {
        buginf("\nsap_register(): sap_registry is NULL");
        return(-1);
    }

    if (sap_registry->dlc_saps[sap/2] != NULL) {
	/* Someone is already registered for this SAP. */
	return (-1);
    }

    sap_registry->dlc_saps[sap/2] = entry;
    return (0);	
}

/*
 * sap_register_bridging()
 *
 * Add the bridging sap to the registry.
 * Returns 0 if all is well, -1 if in use or other error.
 */

int sap_register_bridging (sap_registry_t *sap_registry, 
		  sap_entry_t *entry)
{
    if (sap_registry == NULL) {
        buginf("\nsap_register(): sap_registry is NULL");
        return(-1);
    }

    if (sap_registry->bridging_sap != NULL) {
	/* Someone is already registered for this SAP. */
	return (-1);
    }

    sap_registry->bridging_sap = entry;
    return (0);	
}

/*
 * sap_deregister()
 *
 * Remove a sap from the registry. Fail, if it's wasn't there.
 * Returns the pointer that was in the table.
 */

sap_entry_t *sap_deregister (sap_registry_t *sap_registry, uchar sap)
{
    sap_entry_t *sap_entry;
    int index;

    sap_entry = sap_registry->dlc_saps[sap/2];

    /*
     * If we are trying to unregister a predefined SAP, then go ahead
     * and just set the pointer to NULL.
     */

    if (sap_entry->sap_flags & SAP_PRE_REG_SAP) {
	sap_registry->dlc_saps[sap/2] = NULL;
	return(sap_entry);
    }

    /*
     * If this is a predefined sap, then restore predefined value,
     * otherwise disable the sap by setting the pointer to NULL.
     */
    for (index = 0; index < predef_sap_count; index++) {
	if (sap == predefined_saps[index]) {
	    sap_registry->dlc_saps[sap/2] = &sap_predef_entry;
	    return(sap_entry);
	}
    }

    sap_registry->dlc_saps[sap/2] = NULL;
    return(sap_entry);
}


/*
 * sap_deregister_bridging()
 *
 * Remove the bridging sap from the registry.
 * Returns the pointer that was in the table.
 */

sap_entry_t *sap_deregister_bridging (sap_registry_t *sap_registry)
{
    sap_entry_t *sap_entry;

    sap_entry = sap_registry->bridging_sap;
    sap_registry->bridging_sap = NULL;
    return(sap_entry);
}

/*
 * sap_get_entry
 *
 * given hwidb, sap get the sap entry
 *
 * allow for well-know SAPs,
 * return NULL if no sap entry
 */

sap_entry_t *sap_get_entry(idbtype *idb, uchar sap)
{
    sap_registry_t *sap_registry;
    int index;

    sap_registry = sap_get_registry(idb);
    if (sap_registry == NULL) {
	if (sap == SAP_NULL)
	    return(&sap_null_entry);

	for (index = 0; index < predef_sap_count; index++) {
	    if (predefined_saps[index] == sap)
		return(&sap_predef_entry);
	}
	return(NULL);
    }
    else
    {
	return(sap_registry->dlc_saps[sap/2]);
    }
}

/*
 * sap_get_bridging_entry
 *
 * given hwidb, sap get the bridging sap entry
 *
 * return NULL if no sap entry, or no sap registry
 */

sap_entry_t *sap_get_bridging_entry (idbtype *idb)
{
    sap_registry_t *sap_registry;

    sap_registry = sap_get_registry(idb);
    if (sap_registry == NULL) {
	return(NULL);
    }
    else
    {
	return(sap_registry->bridging_sap);
    }
}

/*
 * sap_get_usap_id
 *
 * Given a SAP, see return ptr to record if SAP has been registered,
 * else return NULL
 */

void *sap_get_usap_id (sap_registry_t *sap_registry, uchar sap)
{
    sap_entry_t		*sap_entry;

    if (sap_registry == NULL)
	return(NULL);
    
    sap_entry = sap_registry->dlc_saps[sap/2];
    /*
     * Could be NULL if SAP not activated.
     */
    return ((sap_entry != NULL) ? sap_entry->usapid : NULL);
}

/*
 * sap_registry_t *sap_get_registry(idbtype *idb)
 *
 * Accessor method
 */

sap_registry_t *sap_get_registry(idbtype *idb)
{
    return (llc_sb_get_sap_registry(idb));
}

/*
 * If the sap is a "well known sap", we will respond whether the
 * registry exists or not. This routine provides a quick test for
 * whether a SAP is in the well known list.
 */
boolean is_well_known_sap(uchar sap)
{

    /*
     * Give the code optimizer a chance. 
     * Keep these in sorted order.
     */
    switch (sap) {
    case SAP_NULL:			/* 0x00 */
    case SAP_IBM_SNA:			/* 0x04 */
    case SAP_IP:			/* 0x06 */
    case SAP_SPAN:			/* 0x42 */
    case SAP_CONS:			/* 0x7E */
    case SAP_3COM:			/* 0x80 */
    case SAP_SNAP:			/* 0xAA */
    case SAP_NOVELL:			/* 0xE0 */
    case SAP_NETBIOS:                   /* 0xF0 */
    case SAP_IBMNM:			/* 0xF4 */
    case SAP_CLNS:			/* 0xFE */
	return TRUE;
    default:
	return FALSE;
    }
}

/*
 * llc_clear_rii
 *
 * Clear the RII.  This is different depending on whether the interface
 * supports SRB or not, and which bit order the address is.
 */

void llc_clear_rii (hwaddrtype *src, hwaddrtype *dest, idbtype *idb)
{
    uchar	ieee48[IEEEBYTES];
    hwidbtype	*hwptr;
    
    hwptr = hwidb_or_null(idb);
    
    /* eliminate the cases where there is no source-route bridging */
    if (hwptr == NULL || !is_srb_hw(hwptr) ) {
	
 	snpa_copy(src, dest);
	
    } else {				/* SRB capable interface */
 	ieee_copy(src->addr, ieee48);	/* make a temporary copy of the address */
	if (idb_is_tokenring_like(hwptr)) {
            /*
             * TOKEN RING, FDDI, and Channel that looks like TR/FDDI.
             * Clear the high-order bit (0x80).
             * FDDI_RII is the same as TR_RII.
             */
            ieee48[0] &= ~TR_RII;
        } else {
            /* FDDIT, and Channel that looks like FDDIT.
             * Clear the low-order bit (0x01).
             */
 	    ieee48[0] &= ~FDDIT_RII;
 	}
 	snpa_init(dest, snpa_get_type(src), snpa_get_target_type(src),
 		  ieee48);
    }
    return;
}

/*
 * Convert a received TEST or XID command packet into a response back
 * to the sender. Turns around the RIF if there is one. Works for all
 * media that support llc (TR, EN, FDDI, FR, ATM). 
 * Since the response is built in place, in the same pak, that pak
 * can't be reused for anything else.
 */
static void llc_respond_pak (paktype* pak)
{
    idbtype *swidb;
    hwidbtype *hwidb;
    sap_hdr *llc;			/* pointer to llc header */
    uchar sap;				/* src SAP holding pen */
    srbrif_t *pak_hdr;
    hwaddrtype hwaddr;

    swidb = pak->if_input;
    hwidb = swidb->hwptr;
    llc = (sap_hdr *) pak->info_start;
    pak_hdr = (srbrif_t *)pak->addr_start;

    /* 
     * Swap the SAP addresses 
     */
    sap = llc->ssap & ~SAP_RESPONSE;
    llc->ssap = llc->dsap | SAP_RESPONSE;
    llc->dsap = sap;
    pak->if_output = pak->if_input;

    if (is_ieee802(hwidb)) {
  	/* 
 	 * Need to swap source and dest mac addresses
         * copy source to dest and pull source from interface
 	 */
 
        ieee_copy(pak_hdr->saddr, pak_hdr->daddr);
 	(*hwidb->extract_hwaddr)(hwidb, &hwaddr);
 	if (is_fddi(hwidb) && (hwidb->type != IDBTYPE_FDDIT)) 
 	  ieee_swap(hwaddr.addr, pak_hdr->saddr);
 	else
 	  ieee_copy(hwaddr.addr, pak_hdr->saddr);
 
         /*
          * If there is a rif, set the rii in the source and
          * remove it from the destination.
          */
	if (pak->rif_start != NULL) {
	  if (hwidb->type == IDBTYPE_FDDIT) {   
 	    pak_hdr->saddr[0] |= FDDIT_RII;
            pak_hdr->daddr[0] &= ~FDDIT_RII;
	  } else {
 	    /* Token ring and non-FDDIT FDDI interfaces */
 	    pak_hdr->saddr[0] |= FDDI_RII;
            pak_hdr->daddr[0] &= ~FDDI_RII;
	  }
	  pak_hdr->rc_blth &= ~RC_RTMASK;
 	  pak_hdr->rc_dlf ^= RC_D;
 	}
     } else if (is_smds(hwidb)) {
 	uchar temp_dte_string[LLC_MAX_DTE_SIZE];
  	hwaddrtype temp_hwaddr;
  
  	memset(temp_dte_string, 0, LLC_MAX_DTE_SIZE);
  	(*hwidb->extract_snpa)(pak, &temp_hwaddr, SOURCE);
 	snpa_copy_to_buffer(&temp_hwaddr, &temp_dte_string[SMDS_RMAC_DTE]);
 	(*hwidb->extract_hwaddr)(hwidb, &temp_hwaddr);
 	snpa_copy_to_buffer(&temp_hwaddr, &temp_dte_string[SMDS_LMAC_DTE]);
 	temp_dte_string[SMDS_LSAP_DTE] = llc->ssap;
 	temp_dte_string[SMDS_RSAP_DTE] = llc->dsap;
 
         pak->enctype = ET_LLC2;
 	pak->datagramsize -= (pak->info_start - pak->datagramstart);
  	pak->datagramstart = pak->info_start;
  	(*swidb->hwptr->vencap)(pak, (long) temp_dte_string);
    } else if (is_atm(hwidb)) {
	uchar temp_dte[LLC_MAX_DTE_SIZE];
	hwaddrtype temp_addr;
	sap_hdr	*llc1;

	memset(temp_dte, 0, LLC_MAX_DTE_SIZE);
	(*hwidb->extract_snpa)(pak, &temp_addr, SOURCE);
	llc1 = (sap_hdr *)pak->info_start;

	snpa_copy_to_buffer(&temp_addr, &temp_dte[ATM_VCD_DTE]);
	temp_dte[ATM_LSAP_DTE] = llc1->dsap;
	temp_dte[ATM_RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
	temp_dte[ATM_L3_PID_DTE] = *(pak->info_start - 1);
	
	pak->datagramsize -= (pak->info_start - pak->datagramstart);
	pak->datagramstart = pak->info_start;
	(*swidb->hwptr->vencap)(pak, (long) temp_dte);
    } else if (is_ddr(hwidb)) {
	uchar temp_dte[LLC_MAX_DTE_SIZE];
	sap_hdr	*llc1;

	memset(temp_dte, 0, LLC_MAX_DTE_SIZE);
	llc1 = (sap_hdr *)pak->info_start;

	temp_dte[PPP_LSAP_DTE] = llc1->dsap;
	temp_dte[PPP_RSAP_DTE] = llc1->ssap & ~SAP_RESPONSE;
	
	pak->datagramsize -= (pak->info_start - pak->datagramstart);
	pak->datagramstart = pak->info_start;
	(*swidb->hwptr->vencap)(pak, (long) temp_dte);
    }
    
    /* 
     * Send it back where it came from
     */
    datagram_out(pak);
}

static void llc_process_test (paktype *pak)
{
    sap_hdr		*llc1;		/* pointer to llc-1 header */
    sap_entry_t		*sap_entry;
    sap_handle_test_t	handle_test;
    idbtype             *idb;
    sap_registry_t      *sap_registry;
    ushort              type_or_len;

    idb = pak->if_input;
    
    llc1 = (sap_hdr *) pak->info_start;
    
    /*
     * If there is no sap registry, go ahead and respond to any TEST
     * request that we receive. Probably want to revisit this someday.
     */
    sap_registry = sap_get_registry(idb);
    if (sap_registry == NULL) {
	if (((llc1->ssap & SAP_RESPONSE) == 0) &&
	    is_well_known_sap(llc1->dsap)) {
	    llc_respond_pak(pak);
	} else {
	    datagram_done(pak);
	}
	return;
    }
	
    sap_entry = sap_get_entry(idb, llc1->dsap);
    if (sap_entry == NULL) {
	/*
	 * Destination sap not registered. 
	 * Do not respond.
	 */
	datagram_done(pak);
  	return;
    }
    
    /*
     * Pass up to the higher layer. We still remain responsible for
     * the pak. If the higher layer needs to use the memory, it saves a
     * copy. 
     */
    if (sap_entry->sap_flags & SAP_TEST_FORWARD) {
	handle_test = sap_entry->sap_handle_test;
	if (handle_test != NULL) {
	    (*handle_test)(idb, pak, FALSE);
	}
    }

    /*
     * If this is a request and we are set up to autorespond, then
     * send the response. Otherwise, dispose of the pak.
     */
    if (((llc1->ssap & SAP_RESPONSE) == 0) && 
	(sap_entry->sap_flags & SAP_TEST_AUTORESPONSE)) {
        /*
         * Do not respond to ethernet 80D5 test frames for non-bridging llc frames.
         */
        if ( is_ethernet(idb->hwptr) ) {
           type_or_len = GETSHORT( &((ether_hdr *)pak->datagramstart)->type_or_len );
           if (type_or_len == ETHER_80D5_TYPE) {
              datagram_done(pak);
              return;
           }  
	}
	llc_respond_pak(pak);
    } else {
	datagram_done(pak);
    }
    return;
}

static void llc_respond_xid (paktype *pak)
{
    uchar *xid_data;

    if ((pak->datagramstart + pak->datagramsize) >
	(pak->info_start + SAP_HDRBYTES)) {
	xid_data = pak->info_start + SAP_HDRBYTES;
	if (*xid_data == XID_FMT_STD) {
	    /*
	     * Eventually want to get this stuff out of the SAP
	     * registry. For now it is hard coded. [sberl] 9/28/94
	     */
	    *(xid_data + 1) = XID_CLASS_TYPE1 | XID_CLASS_TYPE2;
	    *(xid_data + 2) = XID_WIN_TYPE2;
	    llc_respond_pak(pak);
	} else
	    datagram_done(pak);
    } else
	datagram_done(pak);
}

/* 
 * Need to trace all paths to here so we can figure out that
 * the packet is always discarded. [sberl] 9/28/94
 */
static void llc_process_xid (paktype *pak)
{
    sap_registry_t              *sap_registry;
    sap_entry_t			*sap_entry;
    sap_hdr			*llc1;
    boolean			isresponse;
    idbtype                     *idb;
    sap_handle_sna_xid_t	handle_sna_xid;
    sap_handle_ieee_xid_t	handle_ieee_xid;
    ushort                      type_or_len;

    /* common code for both kinds of XID */

    idb = pak->if_input;
    llc1 = (sap_hdr *) pak->info_start;
    isresponse = (llc1->ssap & SAP_RESPONSE);

    sap_registry = sap_get_registry(idb);
    if (sap_registry == NULL) {
	sap_entry = NULL;		/* Never forward, always */
					/* respond if IEEE XID */
    } else {
	sap_entry = sap_get_entry(idb, llc1->dsap);
    }
	
    /* 
     * Decide whether it's an IEEE XID or not.
     */

    /* 
     * Length test for NULL XID (0xBF is last byte)
     * If it's longer than that, see if next byte is
     * IEEE XID (0x81)
     */
    if ((pak->datagramstart + pak->datagramsize > ((uchar *)llc1 +
						   SAP_HDRBYTES) ) &&
	(llc1->data[0] == XID_FMT_STD)) {
	/*
	 * Pass it up to a higher layer. We maintain responsibility
	 * for disposing of the packet. If the upper layer needs to, it
	 * can make a copy.
	 */
	if ((sap_entry != NULL) &&
	    (sap_entry->sap_flags & SAP_IEEE_XID_FORWARD) &&
	    (handle_ieee_xid = sap_entry->sap_handle_ieee_xid)) {
	    (*handle_ieee_xid)(idb, pak, FALSE);
	}

	/*
	 * Decide whether we are to respond to this XID here. If there
	 * is a sap registry entry, then do what it says. If there is
	 * not an entry, then always respond for now. 
	 */
	if (!isresponse) {		/* Is it a request? */
	    if (sap_entry != NULL) {
		if (sap_entry->sap_flags & SAP_IEEE_XID_AUTORESPONSE) {
		    llc_respond_xid(pak);
		    pak = NULL;
		}
	    } else if (is_well_known_sap(llc1->dsap)){
		llc_respond_xid(pak);
		pak = NULL;
	    }
	}
    } else {				/* HANDLE OTHER XID TYPES */
	/*
	 * Pass it up to a higher layer. We maintain responsibility
	 * for disposing of the packet. If the upper layer needs to, it
	 * can make a copy.
	 */
	if ((sap_entry != NULL) &&
	    (sap_entry->sap_flags & SAP_SNA_XID_FORWARD) &&
	    (handle_sna_xid = sap_entry->sap_handle_sna_xid) ) {
            /*
             * Do not respond to ethernet 80D5 xid frames for non-bridging llc frames.
             */
             if ( is_ethernet(idb->hwptr) ) {
                type_or_len = GETSHORT( &((ether_hdr *)pak->datagramstart)->type_or_len );
                if (type_or_len == ETHER_80D5_TYPE) {
                   datagram_done(pak);
                   return;
                }  
            }
	    (*handle_sna_xid)(idb, pak, sap_entry->usapid, isresponse );
       } else {
           /*
            * It is valid to have a llctype w/o a valid sap_entry.
            * If a llctype exists, present the XID to upper layer.
            * If not, returns.
            */
            reg_invoke_SNA_Handle_SNA_XID (idb, pak, NULL, isresponse);
       }

	/*
	 * If we were to handle autoresponse to non-IEEE XIDs this is
	 * where the code would go. I don't think we will ever need
	 * it. Just dispose of the packet.
	 */
    }
    if (pak != NULL) {
	datagram_done(pak);
    }
}

void llc_input(paktype *pak)
{
    sap_hdr* llcheader;
    idbtype   *idb;
    
    idb = pak->if_input;

#ifdef notdef
    /*
     * This is a temporary check here to make sure that we found all
     * the code paths into here. The new general purpose llc stack
     * requires that the packet be parsed before it gets here. This
     * allows the stack to utilize the various pointers in the paktype
     * to find the llc header, mac addresses, etc. Once we have proven
     * to ourselves that this is true, this code can be deleted.
     * [sberl]
     */

    if ((pak->data_area == NULL) ||
	(pak->datagramstart == NULL) ||
	(pak->datagramstart < pak->data_area) ||
	(pak->info_start == NULL) ||
	(pak->info_start < pak->datagramstart)) {
	buginf("\nllc_input: Got a packet that isn't parsed");
	datagram_done(pak);
	return;
    }
    
    /* Additional tests for IEEE 802 LAN packets */
    if (is_ieee802(idb->hwptr) &&
	((pak->mac_start == NULL) ||
	 (pak->mac_start < pak->datagramstart) ||
	 (pak->addr_start == NULL) ||
	 (pak->addr_start < pak->mac_start))) {
	buginf("\nllc_input: Got a LAN packet that isn't parsed");
	datagram_done(pak);
	return;
    }
    
#endif

    /*
     * This is copied from raw_enqueue() since we are bypassing
     * it unless we end up with an I frame.
     */
    GET_TIMESTAMP(idb->hwptr->lastinput); /* remember time of last input */
    GET_TIMESTAMP(pak->inputtime);	/* save it in packet too */

    if (!interface_up_simple(idb)) {
	protocol_discard(pak, TRUE);
	return;				/* and return */
    }

    /*
     * Token ring takes a different path. 
     * Frame relay and other LANS come this way. 
     */
    llcheader = (sap_hdr*) pak->info_start;
    
    switch (llcheader->control & ~LLC1_P) {
    case LLC1_TEST:
	llc_process_test(pak);
	break;
    case LLC1_XID:
	llc_process_xid(pak);
	break;
    default:
	reg_invoke_llc2_input(pak);
	break;
    }
    return;
}

