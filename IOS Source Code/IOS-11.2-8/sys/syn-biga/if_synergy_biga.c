/* $Id: if_synergy_biga.c,v 3.2.62.2 1996/08/28 13:16:27 thille Exp $
 * $Source: /release/112/cvs/Xsys/syn-biga/if_synergy_biga.c,v $
 *------------------------------------------------------------------
 * if_synergy_biga.c - Synergy In-band I/F (BIGA) initialization.
 *
 * May 26 1995, Jin-Jang Chen
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_synergy_biga.c,v $
 * Revision 3.2.62.2  1996/08/28  13:16:27  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.62.1  1996/05/09  14:47:01  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.2.92.1  1996/04/27  07:26:43  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.2.82.1  1996/04/08  02:09:58  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.2.68.2  1996/04/02  04:59:54  rlowe
 * Fix up more include file paths.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.68.1  1996/03/22  09:44:12  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.2.52.2  1996/03/20  10:48:53  rlowe
 * Integrate with ios/wbu sub-tree and makefiles.
 * Also fold old Synergy atmtest.c content into vcdtest.c
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.52.1  1996/03/03  21:25:05  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.2.50.1  1996/03/02  01:21:21  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.2.34.1  1996/02/29  07:50:26  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.2  1995/11/17  18:57:50  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:32:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:02:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "packet.h"
#include "../../boot/src-68-c3000/console.h"
#include "../if/arp.h"
#include "../if/ether.h"
#include "config.h"
#include "ttysrv.h"
#include "logger.h"
#include "../if/msg_datalink.c"		/* Not a typo, see logger.h */
#include "../syn-biga/if_les_biga.h"  
#include "../les/if_les.h"
#include "../if/network.h"
#include "../ip/ip_media.h"
#include "exception.h"

#include "../if/ether_inline.h"

#include "../../wbu/cat5k/sysif/lec_ipc.h"

hwidbtype * biga_idb = 0;

extern uchar my_mac_address[6];
extern void ipc_init(void);


/*
 * Initialize Synergy in-band IP interface.
 */
void syn_ib_ip_init(void)
{
    idbtype *swidb;

    if (biga_idb) {
	swidb = biga_idb->firstsw;

#ifdef SAKUMAR_COMMENTING_OUT_FOR_NOW
	/*
	 * Hard codeing for now !!!
	 */

	swidb->ip_majornet = 0x7f000000;
	swidb->ip_majormask = 0xff000000;
	swidb->ip_address = 0x7f000006;
	swidb->ip_nets_mask = 0xff000000;
	swidb->resolvemethod = 5;
	swidb->ip_enabled = 1;

	/*
	 * Set arp entry for this interface.
	 */
	ip_arp_table_add(swidb, swidb->ip_address, my_mac_address, ET_ARPA, ARP_INTERFACE);
#endif /* SAKUMAR_COMMENTING_OUT_FOR_NOW */
    }
}

/*
 * BIGA subsystem initialisation entry point
 */

void biga_subsys_init (subsystype *subsys)
{

    /*
     * Discover synergy BIGA interface.
     */
    nethernets += 1;
    init_biga_idb(0);
    syn_ib_ip_init();
}

/*
 * This is set up to be called by biga_init() in order to get these codes
 * be linked into the image, since image only includes functions being
 * used from bigatest.a. !!! discard it later.
 */

char phony_flag = FALSE;

void biga_phony_init(void)
{
#if 0
    subsystype * sp;
    if (phony_flag) /* never be TRUE */
        biga_subsys_init(sp);
#endif
}


/*
 * Reset BIGA:
 */
void
biga_if_reset (hwidbtype *idb)
{

}

/*
 * init_biga_idb -- set up an idb for synergy BIGA I/F
 */

void
init_biga_idb (int which)
{
    hwidbtype *idb;

    ipc_init();
    
    /*
     * Set up storage, add IDB to interface queue
     */
    idb = idb_create();
    if (!idb) {
	crashdump(0);
    }

    /*
     * Save our unit number
     */
    idb->unit = which;    /* unit number */

    init_biga_idb_common(idb);

    /*
     * Initialize the hardware:
     */
    biga_if_init(idb, FALSE);
    idb->counters.resets = 0;

    biga_idb = idb;	
}

/*
 * biga_get_default_hw_address
 */

void
biga_get_default_hw_address (hwidbtype *idb, uchar addr[IEEEBYTES])
{
    uchar *ptr = my_mac_address;
    int i;

    /*
     * Copy the hardware address into addr:
     */
    for (i = 0; i < IEEEBYTES; i++)
	addr[i] = *ptr++;
}

/*
 * biga_idb_rx
 */

void biga_idb_rx(paktype *pak)
{
    ushort rxtype;
    idbtype *swidb;

    if (biga_idb) {
	/*
	 * Get pointer to the start of the packet.  The hardware
	 * always assumes that packets are arpa encapsulated and
	 * positions them appropriately.  Thus the magic number
	 * here.
	 */
	rxtype = pak->rxtype = ether_decode_encapsulation_inline(pak,
					(ether_hdr *)pak->datagramstart);

	swidb = biga_idb->firstsw;
	set_if_input(pak, swidb);

	(biga_idb->parse_packet)(biga_idb, pak, NULL);

	biga_idb->counters.rx_cumbytes += pak->datagramsize;
	biga_idb->counters.inputs++;

	(biga_idb->iqueue)(biga_idb, pak);
    }
    else
	datagram_done(pak);
}

