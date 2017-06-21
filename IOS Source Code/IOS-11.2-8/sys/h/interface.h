/* $Id: interface.h,v 3.8.12.19 1996/08/26 15:23:55 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/h/interface.h,v $
 *------------------------------------------------------------------
 * interface.h -- device independent network interface definitions
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface.h,v $
 * Revision 3.8.12.19  1996/08/26  15:23:55  ppearce
 * CSCdi66582:  Implement SRB vector table to cut size of SRB_CORE subsys
 * Branch: California_branch
 *
 * Revision 3.8.12.18  1996/08/15  02:31:59  hampton
 * Change some function arguments to be 'const'.  [CSCdi66102]
 * Branch: California_branch
 *
 * Revision 3.8.12.17  1996/07/26  00:07:22  hampton
 * Add some accessor routines for various IDB values, and make some
 * existing routines use const values.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.8.12.16  1996/07/09  05:49:54  ppearce
 * CSCdi59527:  ascii string in inline wastes code space
 * Branch: California_branch
 *   Just say "no" to inline abuse - SRB subblock inlines removed
 *
 * Revision 3.8.12.15  1996/07/02  23:10:50  wmay
 * CSCdi34302:  Protocol accounting not done correctly for dialer ints
 * do receive accounting for dialer ints on the actual input interface
 * Branch: California_branch
 *
 * Revision 3.8.12.14  1996/06/28  06:03:09  myeung
 * CSCdi61229:  Trying to turn on OSPF on IRB BVI interfaces crashes router
 * Branch: California_branch
 * - Make OSPF recognize BVI
 * - Improve OSPF interface detection so that it will not crash again
 *   when new type of virtual interface is added
 *
 * Revision 3.8.12.13  1996/06/17  08:31:02  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.8.12.12  1996/06/16  21:11:48  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.8.12.11  1996/06/01  01:30:59  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.8.12.10  1996/05/21  22:28:43  cyoung
 * Now it returns a code
 * CSCdi56442:  vtp_download_info needs return code
 * Branch: California_branch
 *
 * Revision 3.8.12.9  1996/05/17  11:15:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.2.7  1996/05/05  23:05:09  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.8.2.6  1996/05/02  22:01:55  hampton
 * Remove support for old Multibus token ring cards.
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.5  1996/04/26  15:17:46  ppearce
 * IBU modularity - SRB subblock (part 1)
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.2.4  1996/04/26  07:52:47  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.8.2.3  1996/04/03  14:25:32  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.8.2.2  1996/03/21  06:07:01  ppearce
 * IBU modularity: Channel subblock
 * Branch: IbuMod_Calif_branch
 *   Move Channel fields from hwidbtype/idbtype structs to Channel subblock
 *
 * Revision 3.8.2.1  1996/03/16  12:08:52  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.8.12.8  1996/05/15  05:31:33  fowler
 * Frame relay VC queuing and rate enforcement
 * Branch: California_branch
 *
 * Revision 3.8.12.7  1996/05/09  14:13:14  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.12.6  1996/05/04  00:42:17  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.8.12.5  1996/04/27  05:46:55  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.8.12.4.4.1  1996/04/27  06:35:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.12.4  1996/04/22  03:34:37  rnardin
 * CSCdi53602:  SNMP nits on new platform
 * Branch: California_branch
 *
 * Revision 3.8.12.3.6.2  1996/04/25  20:05:08  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Incorporate gstovall's comments:
 * - filesys/flash_config.c
 *         - Don't include globs.h, master.h already includes it.
 * - h/interface.h
 *         - Extra "," after IDBTYPE_SYNALC_BIGA.
 * - if/interface.c
 *         - Why did you include network.h?
 *
 * Revision 3.8.12.3.6.1  1996/04/08  01:45:50  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.12.3  1996/03/23  01:30:01  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.8.12.2  1996/03/21  22:37:55  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.8.12.1  1996/03/18  19:37:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.8.16.2  1996/03/25  02:19:02  cakyol
 * sync LE_Syn_Cal_V111_1_3_branch to LE_Cal_V111_1_0_3_branch
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8.16.1  1996/03/22  09:35:39  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.4.2  1996/03/13  17:45:30  cyoung
 * Commit swidb_sb_ info needed by 'vtp configured' changes.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.4.1  1996/02/27  20:41:14  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6  1996/01/25  20:46:30  ccpoe
 * CSCdi47341:  remove subif_tmp_state from idb. It's no longer used.
 *
 * Revision 3.5  1996/01/17  11:49:53  dcalia
 * Update port config to disallow illegal configuration (CSCdi46447)
 *
 * Revision 3.4.2.1  1996/01/24  01:33:57  cyoung
 * VTP MIB support and dependant auto-config base. Part I
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4  1995/12/20  23:45:07  gstovall
 * CSCdi45909:  Item->handled field tested while uninitialized
 * Make sure that item->handled is initialized.  Bad programmer, no pizza.
 *
 * Revision 3.3  1995/11/17  09:12:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:03  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:35:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:58:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/18  17:14:03  wmay
 * CSCdi39845:  show interface displays serials in a non-intuitive manner
 *
 * Revision 2.4  1995/06/30  06:41:24  dstine
 * CSCdi36619:  IDB nits
 *         - sort the SB and list ID enum's
 *         - tidy up some nits
 *
 * Revision 2.3  1995/06/29  23:29:54  jchlin
 * Currently, c4000 and c7000 are reusing hwidbs and swidbs because
 * of dynamic changes of interface configuration. Due to the lack of
 * way to clean up the idbs when they are freed, both platforms will
 * try to release as many known buffer pointers of idb as possible
 * before they are put in the private queue for reuse.
 * When reusing idbs, idb init routines called inside idb_create()
 * are called to reinitialized idbs. In 11.0, there is a new
 * function call added in idb_create() which is not picked up by
 * both platform init_idb(). And therefore, it caused a crash
 * because a null pointer is not initialized. This fix is trying to
 * separate the malloc of idbs and initialization of idbs into
 * two different functions so that the initialization of
 * both hwidb and swidb can be done in that new rourtine as well
 * as new addition function calls in the future. Then we can avoid
 * another addition of function calls in the idb_create() to crash
 * both platform again.
 * CSCdi36076:  Configuring PRI on E1 crashes box w/ALIGN-1-FATAL
 *
 * Revision 2.2  1995/06/14  22:52:56  slin
 * CSCdi35860:  Misc. SMRP cleanups
 * - Pull SMRP fields out of idb and put them into idb sub-block!
 * - Add static idb sub-block type for SMRP.
 *
 * Revision 2.1  1995/06/07  20:35:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#ifndef	__INTERFACE_H__
#define	__INTERFACE_H__

#include "types.h"

/*
 * We assume that master.h was included ahead of this file.
 * master.h includes ../os/list.h, which is needed for this
 * file to compile.
 */

/*
 * Hardware IDB sub-block ID fields. Add to this as necessary.
 */
typedef enum {
    HWIDB_SB_MIN,

    HWIDB_SB_ATM = HWIDB_SB_MIN,
    HWIDB_SB_CMNS,
    HWIDB_SB_CHANNEL,
    HWIDB_SB_DIAL_BACKUP,
    HWIDB_SB_DIAL_ON_DEMAND,
    HWIDB_SB_ETHER,
    HWIDB_SB_FDDI,
    HWIDB_SB_FR,
    HWIDB_SB_HDLC,
    HWIDB_SB_LAPB,
    HWIDB_SB_MOP,
    HWIDB_SB_NETBIOS,
    HWIDB_SB_PPP,
    HWIDB_SB_SDLC,
    HWIDB_SB_SERIAL,
    HWIDB_SB_SMDS,
    HWIDB_SB_STUN,
    HWIDB_SB_TOKEN,
    HWIDB_SB_X25,
    HWIDB_SB_MODEM_INFO,

    HWIDB_SB_MAX,			/* must be before DYNAMIC */
    HWIDB_SB_DYNAMIC
} hwidb_sb_t;

typedef enum {
    SWIDB_SB_MIN,

    SWIDB_SB_APOLLO = SWIDB_SB_MIN,
    SWIDB_SB_APPLE,
    SWIDB_SB_ARP,
    SWIDB_SB_BRIDGING,			/* bridging */
    SWIDB_SB_CDP,
    SWIDB_SB_CLNS,
    SWIDB_SB_DECNET,
    SWIDB_SB_HOTSTANDBY,
    SWIDB_SB_IP,			/* ip routing */
    SWIDB_SB_IPHOST,			/* IP host variables */
    SWIDB_SB_IPROUTING,
    SWIDB_SB_IP_MULTICAST,
    SWIDB_SB_IGMP,
    SWIDB_SB_ISIS,
    SWIDB_SB_ISO_IGRP,
    SWIDB_SB_LAT,
    SWIDB_SB_LEX,
    SWIDB_SB_LLC2,
    SWIDB_SB_NETBIOS,
    SWIDB_SB_NOVELL,
    SWIDB_SB_NTP,
    SWIDB_SB_OSPF,
    SWIDB_SB_SMRP,
    SWIDB_SB_SPAN,			/* spanning tree stuff per port */
    SWIDB_SB_SRB,
    SWIDB_SB_SSE,
    SWIDB_SB_VINES,
    SWIDB_SB_XNS,
    SWIDB_SB_VTP_SUB,                   /* per sub-interface on trunk */

    SWIDB_SB_MAX,			/* must be before SWIDB_SB_DYNAMIC */
    SWIDB_SB_DYNAMIC
} swidb_sb_t;

/*
 * Private IDB list keys.
 * These are used as a reference # to the idb list functions.
 * For new "applications" which want to keep their own lists of IDB's,
 * you can either add a new enum, or you can pass *IDBLIST_DYNAMIC
 * down to the function and keep the indentifier that it returns to you.
 */
typedef enum {
    HWIDBLIST_MIN,			/* generic types of interfaces */

    HWIDBLIST_APOLLO = HWIDBLIST_MIN,	/* protocols and subsys which use */
    HWIDBLIST_APPLETALK,		/* ...hwidb's */
    HWIDBLIST_ASYNC,
    HWIDBLIST_ATM,
    HWIDBLIST_BOOTP,
    HWIDBLIST_BRI,
    HWIDBLIST_BSI,
    HWIDBLIST_CBUS,
    HWIDBLIST_CDP,
    HWIDBLIST_CIP,
    HWIDBLIST_CLNS,
    HWIDBLIST_DECNET,
    HWIDBLIST_ETHER,
    HWIDBLIST_FDDI,
    HWIDBLIST_FR,
    HWIDBLIST_HSSI,
    HWIDBLIST_IP,
    HWIDBLIST_IP_ARP,
    HWIDBLIST_IP_HOST,
    HWIDBLIST_IP_MCAST,
    HWIDBLIST_LANCE,
    HWIDBLIST_LANMGR,
    HWIDBLIST_LAT,
    HWIDBLIST_MCI,			/* particular classes of cards */
    HWIDBLIST_MIP,
    HWIDBLIST_MOP,
    HWIDBLIST_NOVELL,
    HWIDBLIST_NTP,
    HWIDBLIST_RSRB,
    HWIDBLIST_SDLC,
    HWIDBLIST_SERIAL,
    HWIDBLIST_SPAN,
    HWIDBLIST_SRB,
    HWIDBLIST_TMS380,
    HWIDBLIST_TOKEN,
    HWIDBLIST_XNS,
    HWIDBLIST_THROTTLED,		/* currently throttled interfaces */
    HWIDBLIST_TUNNEL,

    HWIDBLIST_MAX = HWIDBLIST_TUNNEL,

    /* SW idb list users */
    SWIDBLIST_MIN,

    SWIDBLIST_APOLLO = SWIDBLIST_MIN,
    SWIDBLIST_APPLETALK,
    SWIDBLIST_BOOTP,
    SWIDBLIST_CDP,
    SWIDBLIST_CLNS,
    SWIDBLIST_DECNET,
    SWIDBLIST_IP,
    SWIDBLIST_IP_ARP,
    SWIDBLIST_IP_HOST,
    SWIDBLIST_IP_MCAST,
    SWIDBLIST_LAT,
    SWIDBLIST_NOVELL,
    SWIDBLIST_NTP,
    SWIDBLIST_RSRB,
    SWIDBLIST_SPAN,
    SWIDBLIST_XNS,
    SWIDBLIST_VTP,

    SWIDBLIST_MAX = SWIDBLIST_VTP,

    /* This should always be before the "dynamic" list enum */
    IDBLIST_MAX,

    /* This should always be last in the list */
    IDBLIST_DYNAMIC
} idblist_t;

/*
 * Function prototypes for the IDB sub-block functions.
 *
 * NB In-line functions are still in interface_private.h for now.
 */

/*
 * Add a sub-block pointer passed by client and set usage to 0.
 */
boolean idb_add_swsb(struct idbtype_ *, swidb_sb_t *, void *);
boolean idb_add_hwsb(struct hwidbtype_ *, hwidb_sb_t *, void *);

/*
 * Remove (ie, nil out) the sub-block pointer and set usage to 0.
 */
boolean idb_delete_swsb(struct idbtype_ *, swidb_sb_t);
boolean idb_delete_hwsb(struct hwidbtype_ *, hwidb_sb_t);

/*
 * Obtain a pointer to the sub-block without incrementing usage count.
 */
void *idb_get_swsb(struct idbtype_ const *, swidb_sb_t);
void *idb_get_hwsb(struct hwidbtype_ *, hwidb_sb_t);

/*
 * Obtain or release a pointer to a sub-block, adjusting usage count.
 */
void *idb_use_swsb(struct idbtype_ *, swidb_sb_t);
void *idb_use_hwsb(struct hwidbtype_ *, hwidb_sb_t);
boolean idb_release_swsb(struct idbtype_ *, swidb_sb_t);
boolean idb_release_hwsb(struct hwidbtype_ *, hwidb_sb_t);

/*
 * IDB list functions. There are no in-lines for these functions.
 */

/*
 * Add a list header for a private IDB list
 */
boolean idb_create_list(idblist_t, ulong *);
void idb_report_create_error (char *str);

/*
 * Destroy a list of IDB's. Free's all list elements.
 */
boolean idb_destroy_list(idblist_t);

/*
 * Add an IDB to the list. The IDB is added in unit/interface-number
 * order.
 */
boolean idb_add_swidb_to_list(struct idbtype_ *, idblist_t);
boolean idb_add_hwidb_to_list(struct hwidbtype_ *, idblist_t);
void idb_report_add_error (char *str);

boolean idb_remove_from_list(void *, idblist_t);
void idb_report_removal_error (char *str);

/*
 * Remove an IDB from a list.
 * Since the function doens't care about the actual type of
 * pointer it is being handed (it just looks for the matching
 * address in the list), we use these inline jackets to get
 * type-checking.
 */
static inline boolean 
idb_remove_swidb_from_list (struct idbtype_ *idb, idblist_t list)
{
    return (idb_remove_from_list(idb, list));
}

static inline boolean 
idb_remove_hwidb_from_list (struct hwidbtype_ *hwidb, idblist_t list)
{
    return (idb_remove_from_list(hwidb, list));
}

/*
 * The following functions are a way of "applying" (in the lisp sense)
 * a specified function to all IDB's on a list.
 */
typedef void (*swidbfunc_t)(struct idbtype_ *, void *);
typedef void (*hwidbfunc_t)(struct hwidbtype_ *, void *);

boolean idb_for_all_on_swlist(idblist_t, swidbfunc_t, void *);
boolean idb_for_all_on_hwlist(idblist_t, hwidbfunc_t, void *);

/*
 * For those who want to live dangerously, this is how you get to the head
 * of the list of IDB's.
 */
list_header     *idb_get_listhead(idblist_t);

/*
 * In-lines and macros.
 *
 * Use these for speed where you can honestly justify not using a safety
 * net. There is no range-checking in these; after all, you're using
 * these in-lines and macro's because you wanted speed, right?
 */

#define FOR_ALL_SWIDBS_IN_LIST(__swlisttype, __element, __idb)             \
    for (__element = LIST_HEAD_ELEMENT(idb_get_listhead(__swlisttype)),    \
         __idb = LIST_GET_DATA(__element);				   \
	 __element != NULL;						   \
	 __element = LIST_NEXT_ELEMENT(__element), __idb = LIST_GET_DATA(__element))

#define FOR_ALL_HWIDBS_IN_LIST(__hwlisttype, __element, __idb)             \
    for (__element = LIST_HEAD_ELEMENT(idb_get_listhead(__hwlisttype)),    \
         __idb = LIST_GET_DATA(__element);				   \
	 __element != NULL;						   \
	 __element = LIST_NEXT_ELEMENT(__element), __idb = LIST_GET_DATA(__element))

/*
 * These will be used to hide the walking of the global list of IDB's,
 * both hardware and software.
 */
#define	IDB_NEXT(__idb) (__idb)->next
#define IDB_PREV(__idb)


/*
 * Driver interface through the IDB.
 */

/*
 * An item is described with one of these descriptors.
 *
 * "item" refers to the item code in use. This will be defined for the 
 * driver in question.
 *
 * "length" will describe the input or output length of the buffer
 * to which "buffer" will point.
 *
 * For items which are 4 bytes or less, use "buffer" as a unsigned long,
 * not as a pointer to an unsigned long; do this by using the union
 * overlays "l_value", "s_value" and "b_value" as appropriate.
 *
 * When an item has been handled (either read or written, according to
 * the operation)
 */
struct itemdesc_ {
    ushort	item;
    ushort	length;

    ulong	handled:1;
    ulong	reserved0:1;
    ulong	reserved1:1;
    ulong	reserved2:1;
    ulong	reserved:24;

    union {
	void		*buffer;
	ulong		l_value;
	ushort		s_value[2];
	uchar		b_value[4];
    } u;
};

/*
 * Inline to setup the item descriptor when using the N_ITEM_LIST macro.
 */
static inline void item_desc_init (struct itemdesc_ *item_desc, ushort item,
				   ushort length)
{
    item_desc->item = item;
    item_desc->length = length;
    item_desc->handled = FALSE;
}

/*
 * A whole list of item descriptors is then described by a itemlist
 * header as follows:
 *
 * item_count -- a count of how many items are being passed in the list.
 * current_item -- points to the first item to be processed or read
 *		   by the function receiving the itemlist.
 * items -- the first item descriptor (above) in a contiguous list of
 *	    such descriptors.
 *
 * item_count *must* reflect the total number of items in the list.
 * If you mis-count, especially with a itemlist allocated on the stack,
 * you might get some very interesting crashes as you overwrite your
 * stack frame.
 *
 * If "relative" is set, this means that item addresses are relative
 * to the start of the itemlist. ie, the item storage and the itemlist
 * are allocated in the same block of memory, with the itemlist at the
 * start of the memory block and the item stored starting after the
 * end of area used for the itemlist. By using item storage which is
 * addressed relative to the beginning of the itemlist block, it is
 * possible to copy the itemlist and item storage as one across an IPC
 * or into another memory space and not have to re-compute the item
 * addresses in the itemlist.
 *
 */
struct itemlist_ {
    ushort		relative:1;	/* 1 == relative item addressing */
    ushort		reserved0:1;
    ushort		reserved1:1;
    ushort		reserved2:1;
    ushort		reserved:12;	/* reserved for future use */
    short		item_count;
    short		current_count;	/* what # item we're working on */
    struct itemdesc_	*current_item;	/* ptr to what item we're on */
    struct itemdesc_	*items;		/* Items start here */
};

/*
 * Function to malloc some memory and set it up for use as an item list
 */
static inline void item_list_malloc (struct itemlist_ *item_list,
				     struct itemdesc_ *item_desc,
				     ulong item_count)
{
    item_list = malloc(sizeof(struct itemlist_) +
		       (item_count * sizeof(struct itemdesc_)));

    if (item_list == NULL) {
	return;
    }

    item_desc = (struct itemdesc_ *)(&(item_list->items) +
				     sizeof(item_list->items));

    item_list->relative = 0;
    item_list->item_count = item_count;
    item_list->current_count = -1;
    item_list->current_item = NULL;
    item_list->items = item_desc;
}


extern boolean hwidb_generic_state_config(hwidbtype *, ulong,
					  struct itemlist_ *);
#ifdef NOBODY_CALLS_ME
extern boolean swidb_generic_state_config(idbtype *, ulong,
					  struct itemlist_ *);
#endif /* NOBODY_CALLS_ME */
typedef boolean (*hwidb_state_config_t)(struct hwidbtype_ *, ulong,
					struct itemlist_ *);
typedef boolean (*swidb_state_config_t)(struct hwidbtype_ *, ulong,
					struct itemlist_ *);


/*
 * Operation codes for the hwidb_control and swidb_control vector.
 */
enum {
    IDB_CONTROL_SET_CONFIG=1,
    IDB_CONTROL_READ_CONFIG,
    IDB_CONTROL_SET_STATE,
    IDB_CONTROL_READ_STATE,

    IDB_CONTROL_CLEAR_STATISTICS,
    IDB_CONTROL_READ_STATISTICS,

    IDB_CONTROL_PEEK_FUNCTION,		/* read contents of function vector */
    IDB_CONTROL_POKE_FUNCTION,		/* set contents of function vector */
    IDB_CONTROL_EXEC_FUNCTION		/* exec function in function vector */
};


/*
 * Base codes for various types of interfaces.
 * These are used to inflate a various IDB type's values into non-conflicting
 * ranges.
 */
enum {
    IDB_TYPE_BASE_GENERIC,
    IDB_TYPE_BASE_NULL=1000,
    IDB_TYPE_BASE_ETHER=2000,
    IDB_TYPE_BASE_TOKEN=3000,
    IDB_TYPE_BASE_FDDI=4000,
    IDB_TYPE_BASE_ATM=5000,
    IDB_TYPE_BASE_SERIAL=6000,
    IDB_TYPE_BASE_STUN=7000,
    IDB_TYPE_BASE_CIP=8000,
    IDB_TYPE_BASE_ISDN=9000,
    IDB_TYPE_BASE_MIP=10000
};

enum IDBTYPE {
    IDBTYPE_UNKNOWN = 0,	/* hardware type is unknown, or is fully */
				/*  defined by the status bits */
    /*
     * Obsolete OEMed interface
     */
    IDBTYPE_SBE16 = 1,		/* hdw type is SBE 16MB TR */
    IDBTYPE_STR = 2,		/* hdw type is SBE STR Dual TR */

    /*
     * cisco built interface boards
     */
    IDBTYPE_MCIETHER = 3,	/* MCI Ethernet */
    IDBTYPE_MCISERIAL = 4,	/* MCI HDLC Interface */
    IDBTYPE_CBUSETHER = 5,	/* CBus Ethernet */
    IDBTYPE_CBUSSERIAL = 6,	/* CBus Serial */
    IDBTYPE_FDDI = 7,		/* CBus FCI (v1) */
    IDBTYPE_FDDIT = 8,		/* CBus FCI-T  */
    IDBTYPE_CTR = 9,		/* CBus Token Ring */
    IDBTYPE_ASYNC = 10,		/* random async line */
    IDBTYPE_HSSI = 11,		/* hardware is High speed serial (T3) */
    IDBTYPE_T1 = 12,	        /* a card with a T1 interface */
    IDBTYPE_E1 = 13,	        /* a card with an E1 interface */
    IDBTYPE_BRI = 14,	        /* a card with a BRI interface */
    IDBTYPE_AIP = 15,		/* hardware is ATM Interface Processor */
    IDBTYPE_CIP = 16,           /* hardware is Channel Interface Processor */
    IDBTYPE_VTY_ASYNC = 17,     /* random virtual async line */
    IDBTYPE_FEIP = 18,          /* Fast Ethernet Interface Processor */
    IDBTYPE_ASYNC_GROUP = 19,	/* master async line */
    IDBTYPE_VTEMPLATE = 20,	/* virtual template interface */
    IDBTYPE_VACCESS = 21,	/* virtual access interface */
    /*
     * The following are chipsets in shared memory.  A board using such a
     * chip, but requiring special drivers (eg the SBE16 board uses
     * the TMS380) should have it's own interface type.
     */
    IDBTYPE_LANCE,		/* hardware is AMD Lance Ethernet */
    IDBTYPE_MK5025,		/* hardware is MK5025 Serial */
    IDBTYPE_TMS380,		/* hardware is TMS380 Tokenring */
    IDBTYPE_BSI,                /* hardware be National BSI DP83200 fddi */
    IDBTYPE_ATMZR,		/* hardware is LSI Logic ATMizer */
    IDBTYPE_HD64570,            /* hardware is HD64570 Serial */
    IDBTYPE_PCBUS,              /* hardware is PC (ISA) Bus */
    IDBTYPE_QUICC_ETHER,	/* hardware is QUICC Ethernet */
    IDBTYPE_QUICC_SERIAL,	/* hardware is QUICC Serial */
    IDBTYPE_AMDP2,		/* hardware is AMD Presidio2 Ethernet */
    IDBTYPE_MIF68840,   	/* hardware is Motorola MIF68840 Fddi */
    IDBTYPE_CYLEPA,		/* hardware is Cylink Encryption Card */
    IDBTYPE_S4T68360,		/* hardware is 68360 Serial */
    IDBTYPE_IBM2692,		/* hardware is ibm2692 token ring */
    IDBTYPE_CD2430,             /* hardware is CD2430 serial */

    /*
     * "fake" interfaces
     */
    IDBTYPE_LB,			/* hardware is loopback */
    IDBTYPE_LEX,                /* Lan Extension interface */
    IDBTYPE_DIALER,		/* Dialer interface */
    IDBTYPE_CIP_LAN_ADAPTER,    /* CIP internal LAN adapter */
    IDBTYPE_BVI,		/* Bridge-group Virtual Interface */
    IDBTYPE_OSPF_VL,		/* OSPF virtual link interface */
/*
 * Interface type(s) for the Catalyst 5000 ATM Module
 */
    IDBTYPE_SYNALC_ATM,		/* Synergy ATM Line Card ATM interface */
    IDBTYPE_SYNALC_BIGA,	/* Synergy ATM Line Card in-band IPC interface*/

    IDBTYPE_MAX                 /* Maximum IDBTYPE (always defined last) */

};

/*
 * Applique types.
 */
enum APPTYPE {
    APPTYPE_NONE,               /* no applique */
    APPTYPE_5IN1,               /* 5 in 1 applique */
    APPTYPE_G703_BAL,           /* G.703 Balanced */
    APPTYPE_G703_UNBAL          /* G.703 Unbalanced */
};


/*
 * Subinterface link types
 */
#define SUBIF_TYPE_UNSPEC	0	/* Unspecified */
#define SUBIF_TYPE_P2P		1	/* Point-to-point */
#define SUBIF_TYPE_MULTI	2	/* Multipoint */


/*
 * Item codes for setting the generic configuration of a HW IDB.
 */
enum {
    ITM_HW_NAME,
    ITM_HW_TYPESTRING,
    ITM_HW_HW_NAMESTRING,
    ITM_HW_HW_SHORT_NAMESTRING,
    ITM_HW_BIA,
    ITM_HW_MAC_ADDRESS,
    ITM_HW_FORCED_MAC,
    ITM_HW_MTU_MAX,
    ITM_HW_MTU_DEF,
    ITM_HW_BUFFERSIZE,
    ITM_HW_CARRIER_TIMER,
    ITM_HW_CARRIER_DELAY,
    ITM_HW_ENCTYPE,
    ITM_HW_ENCTYPE_DEF,
    ITM_HW_ENCTYPE_CFG,
    ITM_HW_ENCSIZE,
    ITM_HW_START_OFFSET,
    ITM_HW_NOKEEPALIVE,
    ITM_HW_EXT_LOOP_TYPE,
    ITM_HW_KEEP_PERIOD,
    ITM_HW_KEEP_COUNT,
    ITM_HW_QUEUE_ALGORITHM
};

/*
 * Item codes for setting/reading the state of a HW IDB.
 */
enum {
    ITM_HW_HW_ENABLED,
    ITM_HW_STATUS,
    ITM_HW_STATE,
    ITM_HW_STATE_TIME,
    ITM_HW_OLDSTATE,
    ITM_HW_NEWPROTO,
    ITM_HW_NEWFDDISTAT,
    ITM_HW_CARRIERTIMER,
    ITM_HW_LASTINPUT,
    ITM_HW_LASTOUTPUT,
    ITM_HW_LASTRESET,
    ITM_HW_REASON,
    ITM_HW_LOOPBACK,
    ITM_HW_IN_LOOPBACK
};

/*
 * Item codes for reading/setting the configuration of a SW IDB.
 */
enum {
    ITM_SW_IP_MTU,
    ITM_SW_CLNS_MTU,
    ITM_SW_TBRIDGE_ON_SWIDB,
    ITM_SW_SUBIF_LINK_TYPE,
    ITM_SW_NAMESTRING,
    ITM_SW_SHORT_NAMESTRING,
    ITM_SW_DESCRIPTION,
    ITM_SW_LAN_NAME,
    ITM_SW_STATIC_ROUTING,
    ITM_SW_RESOLVEMETHOD,
    ITM_SW_DELAY,
    ITM_SW_DELAY_DEF,
    ITM_SW_VISIBLE_BANDWIDTH_DEF,
    ITM_SW_VISIBLE_BANDWIDTH,
    ITM_SW_MANUAL_DELAY,
    ITM_SW_MANUAL_BANDWIDTH
};

/*
 * Interface state codes
 */
enum IDB_STATES {
    IDBS_DOWN,				/* IDB or link proto down */
    IDBS_GOINGDOWN,			/*  */
    IDBS_INIT,				/* in initializing state */
    IDBS_TESTING,			/* link proto is coming up */
    IDBS_UP,				/* IDB up and running */
    IDBS_RESET,				/* user cleared interface */
    IDBS_ADMINDOWN,			/* user shut IDB down */
    IDBS_DELETED			/* IDB marked for being free'd */
};

/*
 * Subinterface states
 */
enum SUBIDB_STATES {
    SUBIF_STATE_DOWN=0,			/* Subinterface is down */
    SUBIF_STATE_UP,			/* Subinterface is up */
    SUBIF_STATE_DELETED,		/* Subinterface has been deleted */
    SUBIF_STATE_ADMINDOWN		/* subinterface admin down */
};


/*
 * Item codes for setting the state of a SW IDB.
 */
enum {
    ITM_SW_SUBIF_STATE
};
/*
 * Enumeration of the different priority levels that can be used by
 * the output priority queuing feature. 
 */
enum PRIORITY {
    PRIORITY_HIGH,
    PRIORITY_MEDIUM,
    PRIORITY_NORMAL,
    PRIORITY_LOW
};

/*
 * Items codes for calling functions based off the IDB.
 */
enum IDB_FUNCTION {
    IDB_FUNC_VENCAP,
    IDB_FUNC_BRIDGE_VENCAP,
    IDB_FUNC_BOARD_ENCAP,
    IDB_FUNC_CACHE_BOARD_ENCAP,
    IDB_FUNC_SOUTPUT,
    IDB_FUNC_SINPUT,
    IDB_FUNC_OQUEUE,
    IDB_FUNC_OQUEUE_DEQUEUE,
    IDB_FUNC_IQUEUE,
    IDB_FUNC_PARSE_PACKET,
    IDB_FUNC_RESET,
    IDB_FUNC_SHUTDOWN,
    IDB_FUNC_LINKTYPE,
    IDB_FUNC_EXTRACT_HWADDR,
    IDB_FUNC_EXTRACT_SNPA,
    IDB_FUNC_MAKE_SNPA_NATIVE,
    IDB_FUNC_BROADCAST,
    IDB_FUNC_CAN_MCAST,
    IDB_FUNC_SETENCAP,
    IDB_FUNC_DEENCAP,
    IDB_FUNC_LINEACTION,
    IDB_FUNC_LINESTATE,
    IDB_FUNC_PERIODIC,
    IDB_FUNC_DEVICE_PERIODIC,
    IDB_FUNC_ENABLE,
    IDB_FUNC_LISTEN,
    IDB_FUNC_LISTEN_RANGE,
    IDB_FUNC_SETUP_BRIDGING,
    IDB_FUNC_SETUP_SRB,
    IDB_FUNC_MODIFY_SRB,
    IDB_FUNC_SET_EXTRA_ADDRS,
    IDB_FUNC_SET_MAXDGRAM,
    IDB_FUNC_TXQLENGTH,
    IDB_FUNC_MAX_PROTO_MTU,
    IDB_FUNC_SYSTEM_CONFIGURED,
    IDB_FUNC_RX_CHAR_INT,
    IDB_FUNC_TX_COMP_NOTIFY,
    IDB_FUNC_RESTART_RX,
    IDB_FUNC_DISABLE_RX,
    IDB_FUNC_GETSYSBUF,
    IDB_FUNC_SHOW_SUPPORT,
    IDB_FUNC_SHOW_CONTROLLER_SUPPORT,
    IDB_FUNC_CLEAR_SUPPORT,
    IDB_FUNC_MAXIMUM			/* must be last in enum list */
};

/*
 * Opcodes used to return interface-specific counter information.
 */
enum IDB_COUNTER_INFO {
    IDB_COUNTER_SI_CNT_PTR=1,
    IDB_COUNTER_CNT_PTR,
    IDB_COUNTER_SI_CNT_ACTIVE,
    IDB_COUNTER_MAXIMUM
};


/*
 * IDB creation and manipulation function prototypes.
 * These are used by drivers and low-level subsystems when creating
 * and ordering IDB's.
 */
extern hwidbtype *idb_create(void);
extern void idb_init_all(hwidbtype *, idbtype *);
extern void idb_unlink(hwidbtype *);
extern void media_idb_enqueue(hwidbtype *);
extern void idb_enqueue(hwidbtype *);
extern void idb_free(hwidbtype *);
extern void idb_init_common(hwidbtype *);
extern void idb_init_names(idbtype *, boolean);
extern int idb_get_next_hw_index(void);
extern int idb_get_current_hw_index(void);
extern boolean idb_can_create_more(void);
extern void idb_first_init(void);
extern void idb_final_init(void);
extern void idb_final_hw_init(hwidbtype *);
extern void idb_final_sw_init(idbtype *);
extern idbtype *idb_create_subif(idbtype *, int);
extern void idb_print_summary(void);
extern void idb_link_hw_sw(hwidbtype *, idbtype *);
extern int idb_name_format(void);
extern boolean idb_subif_allowed_here(hwidbtype *);
extern boolean idb_hw_state_config(hwidbtype *, ulong, struct itemlist_ *);
#ifdef NOBODY_CALLS_ME
extern boolean idb_sw_state_config(idbtype *, ulong, struct itemlist_ *);
#endif /* NOBODY_CALLS_ME */
extern void *idb_get_counter_info(hwidbtype *, enum IDB_COUNTER_INFO);
extern boolean idb_default_ordering(hwidbtype *, hwidbtype *);
extern boolean idb_slotted_ordering(hwidbtype *, hwidbtype *);

/*
 * The following functions access data from within the IDB.
 * These are used mostly by application-level software, but
 * may be used anywhere in the system.
 */
const char* idb_get_namestring(const idbtype*);
void idb_get_hwaddr(const idbtype*, hwaddrtype*);

enum	IDB_ATR_TESTS {
    IDB_ATR_VIRTUAL=0,
    IDB_ATR_TUNNEL,

    IDB_ATR_ONLY_SWIDB,
    IDB_ATR_SUBIF,			/* is this interface a sub-if? */
    IDB_ATR_FIRST_IF,			/* is this interface the first if? */
    IDB_ATR_SUBIF_ALLOWED,		/* is a sub-interface allowed here? */

    /*
     * These are high-level interface characteristics.
     */
    IDB_ATR_POINT_TO_POINT,		/* tunnels and serials */
    IDB_ATR_TOKENRING_LIKE,		/* ring, can do group addresses */
    IDB_ATR_SRB_HW,			/* hardware support SRB */
    IDB_ATR_IEEE_802,			/* Ether, TR, FDDI and 802.10 */
    IDB_ATR_NBMA,			/* FR or FR-like interfaces */
    IDB_ATR_WAN,			/*  */
    IDB_ATR_VLAN_CAPABLE,
    IDB_ATR_SDA_CAPABLE,
    IDB_ATR_EPHEMERAL,
    IDB_ATR_C703,
    IDB_ATR_DDR,

    IDB_ATR_ETHERNET,			/* 10mbit or 100mbit ethernet */
    IDB_ATR_TOKENRING,			/* generic TR */
    IDB_ATR_FDDI,			/* FDDI, son of TR */
    IDB_ATR_ASYNC,			/* async serial */
    IDB_ATR_SERIAL,			/* sync serial */
    IDB_ATR_ISDN,			/* another sync serial */
    IDB_ATR_HSSI,			/* T3 sync serial */
    IDB_ATR_SYNC,			/* a generic sync serial */
    IDB_NOHARDWARE,			/* no hardware backing this up */

    /*
     * These are all sync serial encapsulations or characteristics.
     */
    IDB_ATR_FRAME_RELAY,		/* sync serial encapsulations */
    IDB_ATR_SMDS,
    IDB_ATR_HDLC,
    IDB_ATR_X25,
    IDB_ATR_LAPB,
    IDB_ATR_SLAPB,
    IDB_ATR_PPP,

    /*
     * ATM, spawn of X.25, has a host of flavors.
     */
    IDB_ATR_ATM,			/* generic ATM */
    IDB_ATR_ATM_DXI,			/* ATM in DXI encapsulation */
    IDB_ATR_ATM_DS3,			/* ATM on a DS3 link */
    IDB_ATR_ATM_E3,			/* ATM on a E3 link */
    IDB_ATR_ATM_SONET,			/* ATM on SONET link */
    IDB_ATR_ATM_LANE,			/* ATM encapsulations */
    IDB_ATR_ATM_NOLANE,

    /*
     * The following are IDB's which are really "remote" devices or
     * interfaces.
     */
    IDB_ATR_LEX,			/* Lan EXtender, PPP on sync serial */
    IDB_ATR_VIP,			/*  */
    IDB_ATR_CIP,
    IDB_ATR_CIP_LAN,
    IDB_ATR_CIP_VIRTUAL,

    IDB_ATR_SDLC,			/* encap is set for sdlc */
    IDB_ATR_SDLCP,
    IDB_ATR_SDLCS,
    IDB_ATR_STUN,
    IDB_ATR_BSTUN,			/* encap is set for bstun */
    IDB_ATR_MAXIMUM			/* always must be last */
};

boolean idb_test_atr(const idbtype*, enum IDB_ATR_TESTS);
    
const boolean idb_is_virtual(const idbtype*);
const boolean idb_is_ieee(const idbtype*);
const boolean idb_is_ieeelike(const idbtype*);
const boolean idb_is_tokenring(const idbtype*);
const boolean idb_is_fddi(const idbtype*);
const boolean idb_is_ethernet(const idbtype*);
const boolean idb_is_ether_like(const idbtype*);
const boolean idb_is_fastethernet(const idbtype*);
const boolean idb_is_serial(const idbtype*);
const boolean idb_is_hdlc(const idbtype*);
const boolean idb_is_framerelay(const idbtype*);
const boolean idb_is_sdlc(const idbtype*);
const boolean idb_is_x25(const idbtype*);
const boolean idb_is_smds(const idbtype* idb);
const boolean is_p2p(const idbtype*);
const boolean is_p2p_hw(const hwidbtype*);
const boolean idb_is_ppp(const idbtype* idb);
const boolean idb_is_tunnel(const idbtype* idb);
const boolean idb_is_atmhw(const idbtype* idb);
const boolean idb_is_atm(const idbtype* idb);
const boolean idb_is_atm_lane(const idbtype* idb);
const boolean idb_is_bvi(const idbtype* idb);
const boolean idb_is_lan(const idbtype* idb);
const boolean idb_is_tokenring_like(hwidbtype *idb);

const int		idb_get_unit(const idbtype* idb);
const ipaddrtype	idb_sw_get_ipaddress(const idbtype *);
const boolean idb_validate_sdlc_address(idbtype* idb,int sec_addr);
const ushort            idb_sw_get_atalknet(const idbtype *);
const boolean           idb_sw_get_atalkclientmode(const idbtype *);
const uchar *           idb_sw_get_clnsnet(const idbtype *);
const ulong             idb_sw_get_dnnet(const idbtype *);
const ulong             idb_sw_get_ipxnet(const idbtype *);
const uchar *           idb_sw_get_ipxnode(const idbtype *);
const ulong             idb_sw_get_spanindex(const idbtype *);
const ulong             idb_sw_get_xnsnet(const idbtype *);
const uchar *           idb_sw_get_xnsnode(const idbtype *);

/*
 * Encapsulation functions.
 */
boolean	idb_pak_vencap(paktype *, long);
boolean	idb_board_encap(paktype *, hwidbtype *);
uchar * idb_bridge_vencap(paktype *, long);
void	idb_deencap(hwidbtype *);

/*
 * IO functions.
 */
void		idb_start_output(hwidbtype *);
void		idb_start_input(hwidbtype *);
void		idb_queue_for_output(hwidbtype *, paktype *, enum HEADTAIL);
paktype	*	idb_dequeue_from_output(hwidbtype *);
void		idb_queue_for_input(hwidbtype *, paktype *);
void		idb_rx_char_int(hwidbtype *, uchar, boolean);


/*
 * Protocol types for per-interface accounting.
 */
enum ACCT_PROTOS {
    ACCT_PROTO_OTHER,
    ACCT_PROTO_IP,
    ACCT_PROTO_DECNET,
    ACCT_PROTO_XNS,
    ACCT_PROTO_CLNS,
    ACCT_PROTO_APPLE,
    ACCT_PROTO_NOVELL,
    ACCT_PROTO_APOLLO,
    ACCT_PROTO_VINES,
    ACCT_PROTO_BRIDGE,
    ACCT_PROTO_SRB,
    ACCT_PROTO_MOP,
    ACCT_PROTO_LANMAN,
    ACCT_PROTO_STUN,
    ACCT_PROTO_SPAN,
    ACCT_PROTO_ARP,
    ACCT_PROTO_PROBE,
    ACCT_PROTO_FRS,
    ACCT_PROTO_CDP,
    ACCT_PROTO_BSTUN,    
    ACCT_PROTO_VTP,
    ACCT_PROTO_LAST
};

#define PRIORITY_MAXTYPE		17   /* must be 2^n + 1 */

typedef struct idb_current_counters_ {
    ulong inputs_fast[ACCT_PROTO_LAST]; /* # packets in fast path */
    ulong inputs_auto[ACCT_PROTO_LAST]; /* # packets in autonomous path */
    ulong outputs_fast[ACCT_PROTO_LAST]; /* # packets in fast path */
    ulong outputs_auto[ACCT_PROTO_LAST]; /* # packets in autonomous path */

    ulong output_broad;			/* # broadcasts transmitted */

    ulong tx_cumbytes_fast[ACCT_PROTO_LAST]; /* total bytes sent fast */
    ulong tx_cumbytes_auto[ACCT_PROTO_LAST]; /* total bytes sent auto */
    ulong tx_intbytes;			/* tx_cumbytes last interval */
    ulong tx_datarate;			/* average data rate */

    ulong rx_cumbytes_fast[ACCT_PROTO_LAST]; /* total bytes rcvd fast */
    ulong rx_cumbytes_auto[ACCT_PROTO_LAST]; /* total bytes rcvd auto */
    ulong rx_intbytes;			/* value of rx_cumbytes last time */
    ulong rx_datarate;			/* average data rate */

    /* per-interface-accounting counters */
    ulong cache_miss[ACCT_PROTO_LAST];	/* num fast cache misses */

    /*
     * From here on down, these are counters which are in common
     * with the "show interface counters" in the struct below.
     */
    /*
     * These three fields are grouped together to improve MIPS cache
     * performance.
     */
    ulong inputs;			/* no. all  packets received */
    ulong rx_cumbytes;			/* total all bytes received */
    ulong input_broad;			/* # broadcasts received */

    ulong input_error;			/* # damaged packets received */
    ulong input_crc;			/* # received with CRC errors */
    ulong input_frame;			/* # rcvd with framing/alignment err*/
    ulong input_dribble;		/* # good pkts rcvd w/dribble present*/
    ulong input_overrun;		/* # overruns and resource errors */
    ulong input_abort;			/* # aborts received */
    ulong input_resource;		/* # insuff. input resources events */
    ulong input_drops;			/* # dropped for input congestion */

    ulong outputs;			/* # all  packets transmitted */
    ulong output_error;			/* # packets erred on output */
    ulong output_collision;		/* # of packets collided on output */
    ulong output_underrun;		/* # of underruns on output */
    ulong output_resource;		/* # of no resource errors on output */
    ulong output_nobuffers;		/* # dropped because no buffers */
    ulong output_total_drops;		/* # output drops (inc. fast-switch) */
    ulong output_drops[PRIORITY_MAXTYPE]; /* # dropped output, this q*/
    ulong output_dequeued[PRIORITY_MAXTYPE]; /* # dequeued, this q */

    ulong resets;			/* # of board inits */
    ulong runts;			/* # too small packets received */
    ulong giants;			/* # too large packets received */
    ulong transitions;			/* # of carrier transitions */
    ulong parity;			/* # of parity errors (link ctrlr) */

    ulong tx_cumbytes;			/* total all bytes sent */

    ulong tx_intoutputs;		/* value of outputs last interval */
    ulong tx_packetrate;		/* average output packet rate */
    ulong rx_intinputs;			/* value of inputs last time */
    ulong rx_packetrate;		/* average input packet rate */
    ulong rxtx_interrors;		/* input & output errs last interval */

    /*
     * per-interface-accounting counters 
     * Please use the idb_increment_rx_proto_counts routine to add to the 
     * rx_frames and rx_bytes fields.  This will take into account
     * dialer interfaces, so the correct input interface gets credit
     */
    ulong rx_frames[ACCT_PROTO_LAST];	/* no. pkts; slow,fast,auto */
    ulong rx_bytes[ACCT_PROTO_LAST];	/* no. bytes; slow,fast,auto */
    ulong tx_frames[ACCT_PROTO_LAST];	/* no. pkts; slow,fast,auto */
    ulong tx_bytes[ACCT_PROTO_LAST];	/* no. bytes; slow,fast,auto */

    ulong throttle_count;
} idb_current_counters_t;

/*
 * This struct is used in conjunction with the idb_counters struct to
 * preserve "show interface" counters after a "clear counters"
 * command is performed.
 */
typedef struct idb_si_counters_ {
    ulong slow_in;			/* no. slow packets received */
    ulong fast_in;			/* no. fast packets received */
    ulong auto_in;			/* no. autonomous packets received */

    ulong slow_out;			/* # slow packets output */
    ulong fast_out;			/* no. fast packets output */
    ulong auto_out;			/* no. autonomous packets output */

    ulong tx_slow_cumbytes;		/* total slow bytes sent */
    ulong tx_fast_cumbytes;		/* total fast bytes sent */
    ulong tx_auto_cumbytes;		/* total auto bytes sent */
    ulong rx_slow_cumbytes;		/* total slow bytes received */
    ulong rx_fast_cumbytes;		/* total fast bytes received */
    ulong rx_auto_cumbytes;		/* total auto bytes received */

    /*
     * From here on down, these are counters which are in common
     * with the "current counters" in the struct above.
     */
    ulong inputs;			/* no. all  packets received */
    ulong input_broad;			/* # broadcasts received */
    ulong input_error;			/* # damaged packets received */
    ulong input_crc;			/* # received with CRC errors */
    ulong input_frame;			/* # rcvd with framing/alignment err*/
    ulong input_dribble;		/* # good pkts rcvd w/dribble present*/
    ulong input_overrun;		/* # overruns and resource errors */
    ulong input_abort;			/* # aborts received */
    ulong input_resource;		/* # insuff. input resources events */
    ulong input_drops;			/* # dropped for input congestion */

    ulong outputs;			/* # all  packets transmitted */
    ulong output_error;			/* # packets erred on output */
    ulong output_collision;		/* # of packets collided on output */
    ulong output_underrun;		/* # of underruns on output */
    ulong output_resource;		/* # of no resource errors on output */
    ulong output_nobuffers;		/* # dropped because no buffers */
    ulong output_total_drops;		/* # output drops (inc. fast-switch) */
    ulong output_drops[PRIORITY_MAXTYPE]; /* # dropped output, this q*/
    ulong output_dequeued[PRIORITY_MAXTYPE]; /* # dequeued, this q */

    ulong resets;			/* # of board inits */
    ulong runts;			/* # too small packets received */
    ulong giants;			/* # too large packets received */
    ulong transitions;			/* # of carrier transitions */
    ulong parity;			/* # of parity errors (link ctrlr) */

    ulong rx_cumbytes;			/* total all bytes received */
    ulong tx_cumbytes;			/* total all bytes sent */

    ulong tx_intoutputs;		/* value of outputs last interval */
    ulong tx_packetrate;		/* average output packet rate */
    ulong rx_intinputs;			/* value of inputs last time */
    ulong rx_packetrate;		/* average input packet rate */
    ulong rxtx_interrors;		/* input & output errs last interval */

    /* per-interface-accounting counters */
    ulong rx_frames[ACCT_PROTO_LAST];	/* no. pkts; slow,fast,auto */
    ulong rx_bytes[ACCT_PROTO_LAST];	/* no. bytes; slow,fast,auto */
    ulong tx_frames[ACCT_PROTO_LAST];	/* no. pkts; slow,fast,auto */
    ulong tx_bytes[ACCT_PROTO_LAST];	/* no. bytes; slow,fast,auto */

    ulong throttle_count;
} idb_si_counters_t;


/*
 * Pre-existing garbage that should be obsoleted sooner than is
 * humanly possible.
 */
extern void idb_clear_dangerous(hwidbtype *hwidb);

extern void idb_increment_rx_proto_counts(paktype *, int);

#endif __INTERFACE_H__
