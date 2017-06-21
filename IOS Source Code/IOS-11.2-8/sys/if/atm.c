/* $Id: atm.c,v 3.35.4.47 1996/09/12 21:09:39 wilber Exp $
 * $Source: /release/112/cvs/Xsys/if/atm.c,v $
 *------------------------------------------------------------------
 * A . T . M . C	- Generic ATM encapsulation routine
 *
 * November, 1993, Rick Li Fo Sjoe
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the generic ATM encapsulation routines.  It
 * performs necessary encapsulations for all 'ATM' type interfaces, both
 * present & future.  It takes into account per-vc encapsulations like,
 * LLC/SNAP, NLPID, MUX, etc...
 * It also supports the SVC functionality and automatically calls the
 * Q93B code for call setup & circuit establishment.  Absent Q93B, this
 * module supports the notion of PVCs, as in Frame-Relay.
 * It will allow connection with any device that is ATM capable, given
 * that a 'level-1' encapsulation routine exists for that device
 * (See hes/if_aip.c) for an example of such a module that supports the AIP.
 *------------------------------------------------------------------
 * $Log: atm.c,v $
 * Revision 3.35.4.47  1996/09/12  21:09:39  wilber
 * CSCdi68890:  IRB for Appletalk/IPX not working over ATM interface
 * Should adjust pak->info_start as well
 * Branch: California_branch
 *
 * Revision 3.35.4.46  1996/09/08  03:03:20  rzagst
 * CSCdi62883:  debug atm packet shows incorrect message for oam cells
 * Branch: California_branch
 * Prevent displaying of oam cells if debug atm packet is running, create
 * a separate atm_debug_swidb to hold the oam interface, and add a flag
 * to indicate whether inarp is running or not on the particular vc.
 *
 * Revision 3.35.4.45  1996/08/28  12:47:44  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.35.4.44  1996/08/28  04:53:09  lmercili
 * CSCdi66774:  Router crashed while excuting rfc1577 tests
 * Branch: California_branch
 *
 * Revision 3.35.4.43  1996/08/13  02:18:31  gstovall
 * CSCdi39484:  Giant packets output on ethernet interface
 * Branch: California_branch
 * Make sure we drop giants, even if they are just a wee bit too big.
 *
 * Revision 3.35.4.42  1996/08/08  05:04:09  lmercili
 * CSCdi63861:  rfc1577 stops working when router has more than one AIP
 * Branch: California_branch
 *
 * Revision 3.35.4.41  1996/08/06  18:56:04  awu
 * CSCdi62194:  %SYS-3-INVMEMINT:Invalid memory action; %SYS-2-MALLOCFAIL:
 * Memory al
 * Branch: California_branch
 * Change to the previous commit to ensure calling atm_vc_created() at
 * a safer place.
 *
 * Revision 3.35.4.40  1996/08/03  23:26:49  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.35.4.39  1996/08/01  20:20:40  awu
 * CSCdi62194:  %SYS-3-INVMEMINT:Invalid memory action; %SYS-2-MALLOCFAIL:
 * Memory al
 * Branch: California_branch
 *  -introduce reg_invoke_atm_vc_activated()/deactivated(), for the
 *   callback of activating/deactivating a PVC.
 *
 * Revision 3.35.4.38  1996/07/31  18:37:29  wilber
 * CSCdi63739:  Ping failed when IRB configured on top of LANE
 * Incoporate code-review comment
 * Branch: California_branch
 *
 * Revision 3.35.4.37  1996/07/31  07:57:31  wilber
 * CSCdi63739:  Ping failed when IRB configured on top of LANE
 * Should check for bridging on the received bridged interface instead of
 * the BVI before setting PAK_TBRIDGE_IN
 * Branch: California_branch
 *
 * Revision 3.35.4.36  1996/07/26  04:00:59  achopra
 * CSCdi62544:  Spurious memory access from atm_cpcs_smds()
 * Branch: California_branch
 *
 * Revision 3.35.4.35  1996/07/25  00:15:44  rbadri
 * CSCdi63919:  LANE Client can invalidate fast cache entries too often
 * Branch: California_branch
 * New flag to facilitate better cache invalidation when VCs go down.
 * When there is no change in line state, subif_line_statechange()
 * should just return.
 *
 * Revision 3.35.4.34  1996/07/16  23:39:53  rbadri
 * CSCdi62961:  RFC 1483 bridging fails for E->ATM->E
 * Branch: California_branch
 * In atm_bridge_broadcast(), use pak->atm_vcid only if the packet
 * came in on an ATM interface. Ignore left over values...
 *
 * Revision 3.35.4.33  1996/07/11  00:49:14  bbenson
 * CSCdi61915:  TR Lane doesnt check return value of pak_duplicate()
 * Branch: California_branch
 * Continuation of first commit.  Modified lane registries.
 *
 * Revision 3.35.4.32  1996/07/03  18:15:38  bbenson
 * CSCdi61915:  TR Lane doesnt check return value of pak_duplicate()
 * Branch: California_branch
 * Now checking return of pak_duplicate.  Also moved more the code into
 * the registered function(s) in srt/srb_lane.c, where it belongs.
 *
 * Revision 3.35.4.31  1996/07/03  16:45:39  rzagst
 * CSCdi61369:  Potential memory leak in atm create vc
 * Branch: California_branch
 *
 * Revision 3.35.4.30  1996/07/03  00:01:45  rzagst
 * CSCdi55228:  spurious messages when PLIM is oversubscribed
 * Branch: California_branch
 *
 * Revision 3.35.4.29  1996/06/29  22:04:36  rzagst
 * CSCdi59683:  OAM cells are not transmitted after ATM int was shut/noshut
 * Branch: California_branch
 *
 * Revision 3.35.4.28  1996/06/27  09:33:18  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.35.4.27  1996/06/26  20:35:42  rbadri
 * CSCdi61312:  Broadcast packets (1483 PVC) are not properly filtered
 * Branch: California_branch
 * In atm_bridge_vencap(), skip the VC on which the packets came in.
 *
 * Revision 3.35.4.26  1996/06/19  23:19:30  rzagst
 * CSCdi60596:  ATM ARP fix to CSCdi56362 arp init called w/o atm interface
 * Branch: California_branch
 *
 * Revision 3.35.4.25  1996/06/06  19:24:07  ddecapit
 * CSCdi54960:  SRB Explorer traffic causes ATM input queue wedge with TR
 * LANE
 * Branch: California_branch
 *
 * Revision 3.35.4.24  1996/06/05  21:11:31  ronnie
 * CSCdi39527:  wavl_walk calls avl_walk with wrong parameter
 * Branch: California_branch
 *
 * Revision 3.35.4.23  1996/06/04  21:45:28  ddecapit
 * CSCdi59496:  Code Review Comments for Token Ring LANE
 * Branch: California_branch
 *
 * Revision 3.35.4.22  1996/06/04  01:43:41  ddecapit
 * CSCdi59404:  Microcode incorrectly pads all LANE frames to minimum
 * ethernet
 *              size
 * Branch: California_branch
 *
 * Revision 3.35.4.21  1996/05/24  23:33:10  achopra
 * CSCdi56238:  Linear search for glommable VCs in the vc HASH table has
 *         been changed to a binary search in a WAVL tree maintained per
 *         swidb, which is keyed on NSAPs, AAL5 encapsulation and
 *         protocol type if encapsulation is AAL5MUX.
 * Branch: California_branch
 *
 * Revision 3.35.4.20  1996/05/24  21:09:36  schrupp
 * CSCdi58635:  ATM: ESI cleanup
 * Branch: California_branch
 *
 *  - Deregister addresses when a new prefix is added by ILMI
 *  - Deregister addresses when ESI/NSAP is changed from command line
 *  - Reregister addresses even if the new prefix is the same as the old
 *  - Deregister addresses when subinterface is shut
 *  - Reregister addresses when subinterface is no shutD
 *  - Don't allow qsaal and ilmi pvc's on subinterfaces
 *  - Init atm_db->peeruni_ifname
 *
 * Revision 3.35.4.19  1996/05/23  23:49:08  rzagst
 * CSCdi57889:  %ALIGN-3-SPURIOUS: Spurious memory access made at
 * atm_mapclass_comma
 * Branch: California_branch
 *
 * Revision 3.35.4.18  1996/05/21  16:39:42  jwjang
 * CSCdi57875:  show int atm display wrong uni version
 *              added ATMUNI_VERSION_DEFAULT == ATMUNI_VERSION_30
 * Branch: California_branch
 *
 * Revision 3.35.4.17  1996/05/19  22:43:00  rzagst
 * CSCdi57698:  AIP Burst parameter not rounded up when using SVCs
 * Branch: California_branch
 *
 * Revision 3.35.4.16  1996/05/09  14:29:31  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.35.4.15  1996/05/08  02:43:34  grobelch
 * - Create system list registry functions invoked by static-map software
 *   when:
 *     a static-map is deleted, a map-group is deleted, and when a
 *     static-map is updated. A provider of static-map service (such
 *     as ATM signalling) can register for this, check for one of it's
 *     map's being the subject of the call, and act appropriately.
 *   - Provide ATM signalling routines for the static-map and group
 *     registries. Remove ARP table entries for IP and free up
 *     ATM signalling-specific data structures.
 *   - Improve vc "glomming" for point-to-point VCs to take in
 *     AAL5MUX case.
 *   - Fix mistaken use of an enumeration as a bit-value.
 * CSCdi53027:  ATMSIG: SVC static maps not updated properly in all cases
 * Branch: California_branch
 *
 * Revision 3.35.4.14  1996/05/07  04:32:16  rzagst
 * CSCdi56362:  ATM ARP init can be called when no atm interface is present
 * backing out fix since it kills atm arp.
 * Branch: California_branch
 *
 * Revision 3.35.4.13  1996/05/07  00:37:50  rzagst
 * CSCdi56362:  ATM ARP init can be called when no atm interface is present
 * Branch: California_branch
 *
 * Revision 3.35.4.12  1996/05/05  17:08:47  rzagst
 * CSCdi54165:  Cant create pvcs during subinterface shutdown
 * Branch: California_branch
 *
 * Revision 3.35.4.11  1996/05/04  00:46:34  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Restore the input to the Bridge-group Virtual Interface if necessary
 * - Support user settable mtu for the BVI.
 * - Introduce global_irb_enable, SET_BRIDGING_RXTYPE and "show idb irb"
 *
 * Revision 3.35.4.10  1996/05/01  10:30:04  ronnie
 * CSCdi55904:  ATM multipoint VCs dont work
 * Branch: California_branch
 *
 * Revision 3.35.4.9  1996/04/30  23:16:47  sdurham
 * CSCdi46300:  Interface missing from MIB
 * Branch: California_branch
 *          add atm and aal5 layers to the ifmib
 *
 * Revision 3.35.4.8  1996/04/29  21:08:46  rzagst
 * CSCdi55300:  Invalid vcd of 65535 causes FAIL_CREATEVC error
 * Branch: California_branch
 *
 * Revision 3.35.4.7.2.2  1996/04/30  20:48:36  rbadri
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * IS-IS support for LANE.
 *
 * Revision 3.35.4.7.2.1  1996/04/27  07:03:17  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.35.4.7  1996/04/25  23:12:39  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.35.4.6  1996/04/23  06:05:21  weismann
 * CSCdi51054:  atm 1483 spanning tree bpdus not to spec
 * Branch: California_branch
 * Make to spec.
 *
 * Revision 3.35.4.5  1996/04/11  20:12:52  ronnie
 * CSCdi54406:  Potential null pointer dereference
 * Branch: California_branch
 *
 * Revision 3.35.4.4  1996/04/05  17:59:14  ronnie
 * CSCdi43183:  malloc and free called from at interrupt level running 1577
 * Branch: California_branch
 *
 * Revision 3.35.4.3.4.6  1996/04/17  23:13:29  rbadri
 * Branch: LE_Cal_V112_0_2_branch
 * Convert TokenRing LANE related functions to registries
 *
 * Revision 3.35.4.3.4.5  1996/04/16  02:43:30  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.35.4.3.4.4  1996/04/10  23:23:27  jwjang
 * CSCdi54325:  commit uni 3.1 to LE_Cal_V112_0_2_branch
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.35.4.3.4.3  1996/04/09  02:15:06  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Easter bunny had a field day last weekend through the code.  Non-cisco
 * coding styles squashed.  Dangling else's now attached to }'s, else's
 * now attached to if's, 10 character tabs removed...
 *
 * Revision 3.35.4.3.4.2  1996/04/08  14:47:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.35.4.3.4.1  1996/04/08  01:54:17  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.35.4.3  1996/03/29  19:26:09  grobelch
 *   Provide traffic shaping for SVCs when possible.
 *   Also:
 *   - Specify map-class rate parameters in kbps.
 *   - Provide a per-interface command to indicate
 *     whether SVC should be established if signalled
 *     parameters cannot be installed.
 *   - Don't attach ATM submaps to map-class block
 *   - Check for map-class params prior to using
 *     them (they can be null).
 *   - Fix help-strings for "clp1" parameters to
 *     indicate for clp0&1 flows.
 *   - Signal best effort if only pcr for clp0&1 are
 *     specified.
 *   - map-class parameters override map-list where
 *     they overlap.
 *   - Fix a couple of erroneous variable setting/usages
 *     in static-map handling.
 * CSCdi51550:  ATMSIG: SVCs should attempt to use traffic shaping
 * Branch: California_branch
 *
 * Revision 3.35.4.2  1996/03/23  01:32:11  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.35.4.1  1996/03/18  20:12:59  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.35.6.4  1996/03/28  17:25:10  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.35.6.3  1996/03/25  23:22:25  cakyol
 * fix a sync merge bug.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.35.6.2  1996/03/23  04:54:14  rlowe
 * More sync-time fun and games.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.35.6.1  1996/03/22  09:37:02  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.35  1996/03/08  23:31:26  shj
 * CSCdi50945:  LANE client does not handle data arriving on multicast-send
 * Treat packets coming in on mcast-send same as mcast-forward.
 * Also avoid raw_enqueue() after retbuffer().
 *
 * Revision 3.34  1996/02/27  19:27:24  achopra
 * CSCdi45189:  "show atm traffic" shows different (wrong) packet counts
 *
 * Revision 3.33  1996/02/27  00:34:52  mleelani
 * CSCdi49908:  Crash when configuring PIM RP on LEC
 * Info start incorrectly set for locally generated packets.
 *
 * Revision 3.32  1996/02/23  15:57:44  schrupp
 * CSCdi49684:  Init max_conn_pending for hotswap
 *
 * Revision 3.31  1996/02/23  00:48:52  rzagst
 * CSCdi43692:  ATM subinterfaces ignore subinterface shutdowns
 *
 * Revision 3.30  1996/02/15  07:29:48  smackie
 * Remove highly dubious use of the comma operator. (CSCdi49069)
 *
 * Revision 3.29.6.2  1996/03/05  06:13:34  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.29.6.1  1996/03/03  21:23:08  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.29.2.1  1996/02/27  21:08:05  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.29  1996/02/07  16:12:23  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.28  1996/02/03  22:17:42  ronnie
 * CSCdi46955:  ATM ARP Server table out of sync with map list
 *
 * Revision 3.27  1996/02/01  06:03:48  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.26  1996/01/30  20:01:50  rzagst
 * CSCdi46580:  debug atm packet needs to filter on an interface or vc
 * basis
 *
 * Revision 3.25  1996/01/29  19:18:02  lmercili
 * CSCdi47728:  removing a pvc can cause arp caches to be flushed during
 * interrupt
 *
 * Revision 3.24  1996/01/29  07:28:13  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.23  1996/01/26  03:08:26  awu
 * CSCdi43779:  Declaring or removing a PVC can cause loss of all SVCs.
 *  -change the way of setting up a rateq so that resetting the interf card
 *   is being bypassed.
 *
 * Revision 3.22  1996/01/25  11:18:47  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.21.2.1  1996/01/24  22:17:39  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.21  1996/01/18  23:00:45  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.20  1996/01/13  00:01:22  rzagst
 * CSCdi41346:  Output from sho atm vc shows VCs with negative numbers
 *
 * Revision 3.19  1996/01/09  16:40:52  lmercili
 * CSCdi40580:  Deleting an ATM PVC leaves ARP table populated
 *
 * Revision 3.18.4.1  1996/02/29  07:48:30  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.18.2.4  1996/02/15  19:13:44  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * Add an additional swidb pointer to the registry bump_cache_version
 * to invalidate caches based on swidb too.
 *
 * Revision 3.18.2.3  1996/02/13  00:18:26  rbadri
 * Branch: LE_Cal_V111_0_16_branch
 * DECnet routing for LANE
 *
 * Revision 3.18.2.2  1996/01/30  22:20:57  cakyol
 * - print the vcd in decimal AS WELL for god's sake,
 *   in "atm_show_packet", to simplify debugging.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.18.2.1  1996/01/11  01:45:14  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.18  1996/01/06  02:55:37  ronnie
 * CSCdi46517:  Cant turn off multiple arp server feature
 *
 * Revision 3.17  1995/12/28  23:24:12  rzagst
 * CSCdi45867:  statistics of atm oam cells are not always being displayed
 *
 * Revision 3.16  1995/12/28  00:27:49  rzagst
 * CSCdi46020:  sh atm vc truncates subinterface numbers
 *
 * Revision 3.15  1995/12/27  23:19:28  rzagst
 * CSCdi44587:  config point-to-point pvc does not work correctly when
 * interf is dow
 *
 * Revision 3.14  1995/12/19  21:55:53  rzagst
 * CSCdi45121:  AAL5MUX encap on a PVC allows INARP to be specified
 *
 * Revision 3.13  1995/12/17  18:28:38  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.12  1995/12/11  22:22:30  awu
 * CSCdi31527:  number of current VC in show atm vc differs from show int
 * atm.
 *
 * Revision 3.11  1995/12/05  22:20:26  kartik
 * CSCdi42922:  ILMI should do bookkeeping about registrations/deregs
 * ILMI address registration will now ignore the selector byte.
 *
 * Revision 3.10.6.1  1995/12/29  01:05:52  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.10  1995/11/29  02:38:15  lmercili
 * CSCdi39654:  Add idle time remaining to show atm vc command output
 *
 * Revision 3.9  1995/11/28  22:33:29  rzagst
 * CSCdi39434:  atm->vc_hash[] should be (vc_info_t *) not unsigned
 *
 * Revision 3.8  1995/11/23  19:58:03  bbenson
 * CSCdi42518:  vc creation fails because of wrong max_vpibits in ILMI
 * Fixes bits per VPI/VCI reported by the ILMI.
 *
 * Revision 3.7  1995/11/23  10:40:03  weismann
 * CSCdi34084:  peak/ave/burst is not always displayed properly in show
 *              atm vc. Fixed atm debug,show output. Handle atm vc
 *              defaults with flags.
 *
 * Revision 3.6  1995/11/22  20:44:56  ronnie
 * CSCdi44212:  Many fixes done at Telecom Finland
 *
 * Revision 3.5  1995/11/20  19:04:21  rbadri
 * CSCdi44136:  misleading sh int output after no lane client on subif
 * In atm_encaps_show_protocol_attr(), LANE should be displayed
 * only when a client is configured on a subif.
 *
 * Revision 3.4  1995/11/20  15:42:56  pitargue
 * CSCdi43833:  flooding over multiple atm vcs from fddi results in
 * bitswapped macs
 * only add aal5snap vc's to atm tbridge flood list
 *
 * Revision 3.3  1995/11/17  09:25:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:37:09  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:47:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.41.4.4.2.1  1996/02/03  07:06:44  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.41.4.4  1995/10/19  19:14:30  rnaderi
 * Branch: Arkansas_branch
 * Ark_Ibm_branch commit
 *
 * Revision 2.41.4.3  1995/10/18  02:22:06  ronnie
 * Add Classic IP over ATM enhancements (multiple ARP Servers)
 * Branch: Arkansas_branch
 *
 * Revision 2.41.4.2  1995/10/12  03:22:52  rbadri
 * Branch: Arkansas_branch
 * Commiting LE_Ark features to Arkansas_branch
 *
 * Revision 2.41.4.1  1995/10/01  23:58:11  henniger
 * Branch: Arkansas_branch
 * Sync Arizona Integration branch to V110_nightly_950929_2300
 *
 * Revision 2.33.6.1.6.2  1995/10/26  14:13:34  sakumar
 * Added ILMI support for the "atm vc-per-vp" command for the Catalyst 5000
 * ATM Module only.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.33.6.1.6.1  1995/09/29  18:45:39  sakumar
 * Added support for vc-per-vp command for the Catalyst 5000 ATM module..
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.33.6.1  1995/08/29  16:47:05  rbadri
 * Branch: LE2_V110_1_0_3_branch
 * Non-sync Sync for LE2_V110_0_13_branch from V110_0_13 to V110_1_0_3
 *
 * Revision 2.33  1995/08/18  14:23:39  schrupp
 * CSCdi38181:  Add ATMSIG throttling and SSCOP changes
 *
 * Revision 2.32  1995/08/14  23:23:21  wfried
 * CSCdi38140:  ATM NIM cant ping itself when a 2E NIM is in the box
 * Move setting of IDB_HEARSELF to platform dependent code.
 *
 * Revision 2.31  1995/08/11  20:58:44  ronnie
 * CSCdi38022:  ATM ARP client needs to check for valid NSAP before
 * creating map
 *
 * Revision 2.30  1995/08/10  23:30:37  wfried
 * CSCdi37608:  oam f5 cells generated by 4500 are not recognized by
 * analyzer
 * Changed ATM code to pass VC pointer for OAM cells and other related
 * changes
 *
 * Revision 2.29  1995/08/10  00:59:32  awu
 * CSCdi37887:  ATM OAM cells are transmitted and received incorrect.
 *
 * Revision 2.28  1995/08/10  00:06:27  shj
 * CSCdi38491:  traceroute through LANE interface fails
 * Packets incorrectly classified as to broadcast address.  Add
 * correct processing for LANE packets to atm_isbcast().  Non-LANE
 * still to be fixed under CSCdi35837.
 *
 * Revision 2.27  1995/08/08  16:47:43  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.26  1995/08/08 06:19:02  schrupp
 * CSCdi38024:  Add interface string to show atm map command
 *
 * Revision 2.25  1995/07/31  22:52:10  awu
 * CSCdi37938:  FAILCREATEVC uses VCD in hex, but show atm vc uses VCD in
 * decimal.
 *
 * Revision 2.24  1995/07/27  14:45:40  awu
 * CSCdi36054:  ATM driver calls printf.
 *
 * Revision 2.23  1995/07/26  17:36:22  percyk
 * CSCdi34916:  sscop interface commands cause SegV exception during
 * initialization
 *
 * Revision 2.22  1995/07/26  15:14:19  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.21  1995/07/19  23:16:30  awu
 * CSCdi35048:  part of CSCdi30966 is missing from 11.0.
 *
 * Revision 2.20  1995/07/19  07:30:50  awu
 * CSCdi36778:  <show atm vc nn> rejects valid vcd numbers.
 *  Fix a typo in the last commit.
 *
 * Revision 2.19  1995/07/19  07:11:26  awu
 * CSCdi36778:  <show atm vc nn> rejects valid vcd numbers.
 *
 * Revision 2.18  1995/07/17  21:55:37  shj
 * CSCdi37128:  Startup registry error message
 *
 * Move MAC assignment for AIP from common code (atm.c) to platform
 * specific (if_ags_c7000.c) so it doesn't fail cause registry error
 * on 4500.
 *
 * Revision 2.17  1995/07/10  22:34:55  bbenson
 * CSCdi36808:  LANE should not allow RFC 1577 to be configured
 *
 * Revision 2.16  1995/07/01  09:17:22  awu
 * CSCdi36472:  Misalignment in atm_show_packet().
 *
 * Revision 2.15  1995/06/30  20:24:00  kartik
 * CSCdi36522:  VCCChange ILMI trap should not be sent for SVCs.
 *
 * Revision 2.14  1995/06/28  09:26:21  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.13  1995/06/27  00:22:06  wfried
 * CSCdi36069:  Performance tuning and changes on controller screen for
 * new interfac
 * Included more interface status in the show controller output,
 * forced ATM receiving packets to start at 16 byte boundaries for
 * better DBUS utilization, and made some fastswitching optimizations.
 * Created new ATM interface loopback command options and renamed E3
 * framing modes to reflect current terminology
 *
 * Revision 2.12  1995/06/26  01:56:20  percyk
 * CSCdi34296:  No error message if no ATM signalling channel
 *
 * Revision 2.11  1995/06/24  00:45:40  awu
 * CSCdi35640:  ATM_BAD_VC macro is incorrect.
 *
 * Revision 2.10  1995/06/22  18:07:30  bbenson
 * CSCdi36308:  pak->datagramsize isnt accounting for the lecid in LANE
 *
 * Revision 2.9  1995/06/22  02:35:11  bbenson
 * CSCdi35017:  Client should disallow rfc1577 and vice versa on subint.
 *
 * Revision 2.8  1995/06/20  07:13:39  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.7  1995/06/19  01:05:41  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.6  1995/06/15  06:03:33  dbath
 * CSCdi33071:  IPX fast switching, over equal cost ATM paths is broken
 * Replaced PAK_FR_CACHED flag with a more generic one, PAK_ENCAPS_ONLY,
 * so that we could also use it in ATM-land.  Made IPX ATM fastswitch
 * cache population code cognisant of multiple equal cost paths,
 * like the frame relay case.
 *
 * Revision 2.5  1995/06/12  19:12:59  ronnie
 * CSCdi29773:  clear inter atm could lose atm pvc config command and the
 * PVC
 *
 * Revision 2.4  1995/06/09  18:22:00  rlfs
 * CSCdi34432:  AIP rejected the SETUP_VC command after switch reset
 *
 * Revision 2.3  1995/06/09  16:58:43  ronnie
 * CSCdi35021:  ATM idle-timeout not displayed
 * Also moved the ATM NSAP displays down to the individual subinterfaces
 *
 * Revision 2.2  1995/06/08  15:44:35  pitargue
 * CSCdi35314:  flood list for atm bridge links gets hammered
 * don't zero out the list on every atm_reset() call.
 *
 * Revision 2.1  1995/06/07  20:50:18  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "ether.h"
#include "tring.h"
#include "../llc2/llc2.h"
#include "config.h"
#include "parser.h"
#include "../ui/parse_util.h"
#include "../parser/parser_defs_exec.h"
#include "../atm/parser_defs_atm.h"
#include "../lane/parser_defs_lane.h"
#include "../wan/serial.h"
#include "logger.h"
#include "../ip/ip_registry.h"
#include "network.h"
#include "../srt/span.h"
#include "atm_debug.h"
#include "atm.h"
#include "static_map.h"
#include "../atm/atmsig_api.h"
#include "../atm/sscop_public.h"
#include "../atm/atmsig_private.h"
#include "../atm/atmsig_smap.h"
#include "../wan/smds.h"
#include "../lane/lane.h"
#include "../lane/lane_registry.h"
#include "../lane/lanefast_les.h"

#include "../lane/lec_srb.h"
#include "../lane/lane_private.h"
#include "../lane/lec_private.h"
#include "../srt/srt_registry.h"
#include "../srt/rsrb.h"
#include "../srt/srb.h"
#include "fastswitch_registry.h"
#include "fddi.h"
#include "msg_datalink.c"		/* Not a typo, see logger.h */
#include "../atm/msg_atm.c"		/* Not a typo, see logger.h */
#include "../atm/atm_oam.h"
#include "../atm/atm_arp.h"

#include "../atm/atm_registry.h"
#include "../atm/atm_registry.regc"


/*
 * Local storage stuff
 */
ulong atm_counters[ATM_MAX_COUNTERS];

long atm_regcode;		/* for dynamically allocated registry */
boolean atm_ilmi_enabled = FALSE;
static boolean atm_free_vc_flag = FALSE; /* flag whether we need to free some
					    vc's */

/*
 * Forward declarations
 */
static int atm_max_proto_mtu(idbtype *, enum LINK, boolean);
static void atm_map_nvgen(staticmap_type *);
static void atm_mapclass_nvgen(mapclass_type *);
static void atm_interface(hwidbtype *);


/*
 * Wired ATM descriptor blocks to cover the OIR case where we can't alloc
 * these puppies in the atm_setup() routine.  This routine may be called
 * from interrupt side where malloc() is not allowed!
 */
atm_db_t atm_db[MAX_ATM_INTFC];

static atm_multicast_t *atm_multic = NULL; /* Ptr. to all MULTICAST addrs. */
idbtype *atm_debug_swidb[MAX_DEBUG_FILTER_TYPES];
vcd_t atm_debug_vc;
void (*atm_arp_start_func)(void) = NULL;

/* forward declarations */
static void atm_flush_ar_cache(hwidbtype *);


/**************************Subfunctions***********************************/
/*
 * Section: Subroutines
 *	This is the subroutines section.  Contains routines for parsing
 *	protocol addresses, movement of strings, searching for static
 *	mapping entries, adding static maps, etc...
 */

static void
atm_hashin_vc (atm_db_t *atm, vc_info_t *vc_info)
{
    vcd_t vcnum;
    vc_info_t *hash_cur;
    leveltype level;

    level = raise_interrupt_level(NETS_DISABLE);
    vcnum = vc_info->vc;
    hash_cur = atm->vc_hash_tbl[HASHVC(atm, vcnum)];
    vc_info->next_vc = hash_cur;
    atm->vc_hash_tbl[HASHVC(atm, vcnum)] = vc_info;
    /*
     * link the VC into the doubl-linked InARP chain if necessary
     */
    if (vc_info->inarp) {
	vc_info->next_inarp = atm->inarp_vc;
	atm->inarp_vc = vc_info;
	/*
	 * set up backward links
	 */
	if (vc_info->next_inarp)
	    vc_info->next_inarp->prev_inarp = vc_info;
	vc_info->prev_inarp = NULL;
    }
    reset_interrupt_level(level);
}
static void
atm_unlink_inarp_vc(atm_db_t * atm,vc_info_t * cur)
{
	if (cur->prev_inarp) {
	    cur->prev_inarp->next_inarp = cur->next_inarp;
	} else {
	    atm->inarp_vc = cur->next_inarp;
	}
	if (cur->next_inarp)
	    cur->next_inarp->prev_inarp = cur->prev_inarp;
	cur->prev_inarp = cur->next_inarp = NULL;
}

static vc_info_t *
atm_hashout_vc (atm_db_t *atm, vcd_t vcnum)
{
    vc_info_t *cur, *prev;
    leveltype level;

    prev = NULL;
    cur = atm->vc_hash_tbl[HASHVC(atm,vcnum)];
    while (cur) {
	if (cur->vc == vcnum)
	    break;
	prev = cur;
	cur = cur->next_vc;
    }

    if (!cur) 
	return (NULL);					/* Not found */

    level = raise_interrupt_level(NETS_DISABLE);
    if(!prev) {
	atm->vc_hash_tbl[HASHVC(atm, vcnum)] = cur->next_vc;
    } else {
    	prev->next_vc = cur->next_vc;		/* Unlink from list */
    }
    /*
     * Unlink it from the InARP chain if necessary
     */
    if (cur->inarp) {
	atm_unlink_inarp_vc(atm,cur);
    }
    reset_interrupt_level(level);
    
    return(cur);
}

/*
 * atm_link2nlpid
 * Given a link type, return a serial type code
 * NOTE: If changing this, see also link2nlpid() in wan/frame_relay.c
 *	for similar changes.
 */
static ushort 
atm_link2nlpid (ulong link)
{
    switch (link) {
	case LINK_IP:		 return(ATM_NLPID_IETF_IP);

	case LINK_CLNS_ALL_ES:
	case LINK_CLNS_ALL_IS:
	case LINK_ISIS_ALL_L1_IS: 
	case LINK_ISIS_ALL_L2_IS: 
	case LINK_CLNS:
	case LINK_CLNS_BCAST:	 return(ATM_NLPID_CTL);
    }
    if (atm_errors_debug)
	buginf("\natm_link2nlpid():Illegal serial link type code %d\n",
	       link);
    return(0);
}
#ifdef	ATM_FUTURES
/*
 * atm_nlpid2link
 * Given a link type, return a serial type code
 * NOTE: If changing this, see also link2nlpid() in wan/frame_relay.c
 *	for similar changes.
 */
static ulong 
atm_nlpid2link (ushort nlpid)
{
    switch (nlpid) {
      case FR_IETF_IP:	return(LINK_IP);
      case FR_CLNP:	return(LINK_CLNS);
      case FR_ESIS:	return(LINK_CLNS);
      case FR_ISIS:	return(LINK_CLNS);
      default:		return(LINK_ILLEGAL);
    }
}
#endif /*ATM_FUTURES*/

/*
 * atm_smdsequal
 * Given two 64 bit addresses, return equality predicate.
 *
 * This will also have to change if all 60 bits of address are used.
 * NOTE: If changing this, see also link2nlpid() in wan/frame_relay.c
 *	for similar changes.
 */

static boolean
atm_smdsequal (uchar *addr1, uchar *addr2)
{
    if ((addr1 == NULL) || (addr2 == NULL))
        return(FALSE);
    if (bcmp(addr1, addr2, STATIONLEN_SMDS64))
	return(FALSE);
    return(TRUE);
}

/*
 * atm_smds_l3_check: Called to check the sanity of an SMDS header/trailer.
 * NOTE: If changing this, see also link2nlpid() in wan/frame_relay.c
 *	for similar changes.
 * OUTPUT/RETURN:
 *	TRUE - Header & Trailer are correct
 *	FALSE - Header and/or Trailer are incorrect
 * INPUT:
 *	smdstype ptr.
 */
static boolean
atm_smds_l3_check (smdstype *header, idbtype *idb)
{
    smdshetype  *smdshe;
    
    smdshe = (smdshetype *) header->smds_hel;
    /*
     * Qualify the packet header before processing
     */
    if (header->smds_rsvd != 0) {
        if (atm_errors_debug)
          buginf("\n SMDS(%s): Invalid packet, Reserved NOT ZERO, %d ",
		 idb->namestring, header->smds_rsvd);
	return(FALSE);
    }
    if (header->smds_basize < SMDS_MINPAKLEN 
        || header->smds_basize > SMDS_MAXPAKLEN) {
        if (atm_errors_debug)
          buginf("\n SMDS(%s): Invalid packet, Bad BA length %d ",
		 idb->namestring, header->smds_basize);
	return(FALSE);
    }
    if ((header->smds_xhel & SMDS_XHEL_HEL) != XHEL_SIZE) {
        if (atm_errors_debug)
          buginf("\n SMDS(%s): Invalid packet, Bad header extension length %d ",
		 idb->namestring, header->smds_xhel);
	return(FALSE);
    }

    /*
     * Verify the Header extension 'Version' values only
     */
    if (smdshe->smdshe_length != SMDSHE_VERLEN) {
        if (atm_errors_debug)
          buginf("\n SMDS(%s): Invalid packet, Bad header extension length %d ",
		 idb->namestring, smdshe->smdshe_length);
	return(FALSE);
    }
    if (smdshe->smdshe_type != SMDSHE_VERTYP) {
        if (atm_errors_debug)
          buginf("\n SMDS(%s): Invalid packet, Bad header extension type %d ",
		 idb->namestring, smdshe->smdshe_type);
	return(FALSE);
    }
    if (smdshe->smdshe_value != SMDSHE_VERVAL) {
        if (atm_errors_debug)
          buginf("\n SMDS(%s): Invalid packet, Bad header extension value %d ",
		 idb->namestring, smdshe->smdshe_value);
	return(FALSE);
    }

    if (!SMDS_ISINDIV(header->smdsdest[0]) && 
      !SMDS_ISGROUP(header->smdsdest[0])) {
	if (atm_errors_debug)
	    buginf("\n SMDS(%s): Invalid Destination E.164 address format %E",
		idb->namestring, header->smdsdest);
	return(FALSE);
    }
    if (!SMDS_ISINDIV(header->smdssrc[0]) && 
      !SMDS_ISGROUP(header->smdssrc[0])) {
	if (atm_errors_debug)
	    buginf("\n SMDS(%s): Invalid Source E.164 address format %E",
		idb->namestring, header->smdssrc);
	return(FALSE);
    }
    return(TRUE);
}

/**************************Subfunctions***********************************/

/*
 * Below are the 3 routines needed to manage the multicast addresses
 *	Add, Delete & Get.
 */
boolean
atm_add_multicast (idbtype *idb, hwaddrtype *hwaddr)
{
    atm_multicast_t *multic = atm_get_multicast(idb);

    if(multic) {
	bcopy(hwaddr, &multic->mca, sizeof(hwaddrtype));
	return(TRUE);
    } else {
	multic = malloc(sizeof(atm_multicast_t));
	if (multic) {
	    bcopy(hwaddr, &multic->mca, sizeof(hwaddrtype));
	    multic->idb = idb;
	    multic->next = atm_multic;
	    multic->prev = (atm_multicast_t *)&atm_multic;
	    if (atm_multic) {
		atm_multic->prev = multic;
	    }
	    atm_multic = multic;
	    return(TRUE);
	}
    }
    return(FALSE);
}


atm_multicast_t *
atm_get_multicast (idbtype *idb)
{
    atm_multicast_t *multic = atm_multic;

    for(multic = atm_multic; multic; multic = multic->next) {
	if (multic->idb == idb) {
	    break;
	}
    }
    return(multic);
}

void
atm_delete_multicast (idbtype *idb)
{
    atm_multicast_t *multic = atm_multic;

    multic = atm_get_multicast(idb);
    if(!multic) {
	return;
    }

    if (multic->prev == atm_multic) {
	atm_multic = multic->next;
	if (multic->next) {
	    multic->next->prev = atm_multic;
	}
    } else {
	multic->prev->next = multic->next;
	if (multic->next) {
	    multic->next->prev = multic->prev;
	}
    }
    free(multic);
}

/*
 * atm_free_db:	Let go of the sucker
 *	1) Release all VC information & memory
 *	2) Release the HASH table.
 *	3) Clear the FLAGS to indicate !DB_INUSE
 */
static void
atm_free_db (hwidbtype *hwidb)
{
    atm_db_t *atm_db = hwidb->atm_db;
    ushort ndx;
    vc_info_t **savhash = atm_db->vc_hash_tbl;
    unsigned savbucket = atm_db->vchash_buckets;

    if(atm_db->vc_hash_tbl) {
	for(ndx = 0; ndx < ATM_HASH_DEFAULT; ndx++) {
	    vc_info_t *vc;
    	    vc = atm_db->vc_hash_tbl[ndx];
	    for(; vc; vc = vc->next_vc) {
		vc->flags &= ~VC_FLAG_ACTIVE;	/* No active anymore */
	    }
	}
    }
    memset(atm_db, 0, sizeof(atm_db_t));
    atm_db->vc_hash_tbl = savhash;
    atm_db->vchash_buckets = savbucket;

    /* initialize signalling vc number */
    atm_db->sig_vcnum = 0;
 
    /* fill in timer values */
    atm_db->timer_t303   = ATMSIG_T303_DEFAULT;
    atm_db->timer_t308   = ATMSIG_T308_DEFAULT;
    atm_db->timer_t310   = ATMSIG_T310_DEFAULT;
    atm_db->timer_t313   = ATMSIG_T313_DEFAULT;
    atm_db->timer_t316   = ATMSIG_T316_DEFAULT;
    atm_db->timer_t322   = ATMSIG_T322_DEFAULT;
    atm_db->timer_t398   = ATMSIG_T398_DEFAULT;
    atm_db->timer_t399   = ATMSIG_T399_DEFAULT;
 
    /* default max retry count */
    atm_db->atmsig_retry_cnt = ATMSIG_RETRY_DEFAULT;

    /* default to user side on the router */
    atm_db->uni_side = ATMSIG_SIDE_USER;
 
    /* default to UNI 3.0 */
    atm_db->uni_version = ATMUNI_VERSION_DEFAULT;
    atm_db->ignore_peer_uni = FALSE;
    atm_db->peeruni_version = ATMUNI_VERSION_INVALID;

    /* default max connections pending */
    atm_db->max_conn_pending = ATMSIG_MAX_CONN_PENDING_DEFAULT;

    /* init ilmi fields */
    atm_db->peeruni_ifname = NULL;
    atm_db->disable_ilmi_traps = FALSE;

    /* set default mapclass to null */
    if (atm_db->def_class) {
        atm_db->def_class->ref_count--;
        atm_db->def_class = NULL;
    }

}

/*
 * atm_get_db:	Called to get a Descriptor block for it
 *	Try to use the same info. block that was attached to this interface
 *	if one already exists.  If not, then just grab a free one.
 * NOTE: We can't do allocation here since we may be called from INT.
 *	code.  Look up these dudes in a static list of free blocks.
 */
static atm_db_t *
atm_get_db (hwidbtype *hwidb)
{
    atm_db_t **atmdb_ptr, *atmdb;
    ushort ndx;
    int free_ndx, old_ndx;

    atmdb_ptr = &hwidb->atm_db;
    atmdb = *atmdb_ptr;

    ndx = free_ndx = old_ndx = -1;
    if (!atmdb) {
	for(ndx = 0; ndx < MAX_ATM_INTFC; ndx++) {
	    if(!(atm_db[ndx].flags & ATM_FLAG_DB_INUSE) && free_ndx < 0)
		free_ndx = ndx;
	    if(atm_db[ndx].hwidb == hwidb)
		old_ndx = ndx;
	}
	if(free_ndx < 0 && old_ndx < 0)
	    return(NULL);
	ndx = (old_ndx >= 0) ? old_ndx : free_ndx;
	atmdb = &atm_db[ndx];
	atmdb->hwidb = hwidb;	/* It's ours now! */
	*atmdb_ptr = atmdb;
	atm_free_db(hwidb);	/* Call to clean up the struct */
	atmdb->hwidb = hwidb;	/* It's ours now! */
    	/*
     	 * Now setup the ATM interface specific config stuff.
     	 */
	if(!atmdb->vc_hash_tbl)
    	    atmdb->vchash_buckets = ATM_HASH_DEFAULT;
    	atmdb->num_vc = 0;			/* nothing allocated yet */
	atmdb->txclock = ATM_CLOCK_LINE;	/* Default */
	atmdb->idle_time = ATM_IDLETIME_DEFAULT;
    }
    atmdb->flags |= ATM_FLAG_DB_INUSE;
    atmdb->address_reg = TRUE; /* Default: Enable*/
    return(atmdb);
}

/*
 * atm_free_inactive_vc
 *
 * Called periodically to clean up inactive SVC's.
 * It simply returns if atm_free_vc_flag is not set. Otherwose,
 * it walks whole VC table "row" by "row" and w/in each row
 * it walks through the VC list to find out any inactive SVC. Hash it out
 * and free it when find one during the walk-through.
 *
 */
static void atm_free_inactive_vc (atm_db_t *atm)
{
    vc_info_t *vc, *next_vc;
    ushort ndx;
    boolean removed;
    boolean re_run = FALSE;

    /*
     * if atm_free_vc_flag is NOT set, we don't need to proceed
     */
    if (atm_free_vc_flag && atm->vc_hash_tbl) {
	/*
	 * we must reset the flag here at the very beginning to avoid
	 * locking out interrupts for the whole walk-through. The worst
	 * case is that we'll have chance that we still walk through 
	 * the whole VC table next time (due to the atm_free_vc_flag being 
	 * set) when there is no inactive svc at all(due to that all those
	 * inactive SVC's happen to have been taken care by this round). 
	 * The win is that we don't need to lock out interrupts for the whole 
	 * walk-through.
	 */
	atm_free_vc_flag = FALSE;
	    
	/*
	 * we now walk through the VC table "row by row"
	 */
	for (ndx = 0; ndx < atm->vchash_buckets; ndx++) {
    	    vc = atm->vc_hash_tbl[ndx];
	    /*
             * w/in each row, we walk through the VC list
	     */
	    while (vc) {
		removed = FALSE;
		next_vc = vc->next_vc;
		/* if a vc was removed but hardware failed to remove it*/
		if (IS_VC_DELETED(vc)) {
		    removed = (*atm->funcs.teardownvc)(atm->hwidb, vc);
		    if (!removed) {
			re_run = TRUE;
		    }
		} 
		if (IS_PVC_DOWN(vc)) {
		    removed = (*atm->funcs.teardownvc)(atm->hwidb, vc);
		    if (removed) {
			/* now pvc is removed in hardware */
			vc->flags &= ~VC_FLAG_PVC_DOWN;
			/* don't hash out pvc */
			removed = FALSE;
		    } else {
			re_run = TRUE;
		    }
		}

		if ((!ISVCACTIVE(vc) && IS_ATMVC_SVC(vc) &&
		     (!(IS_VC_DELETED(vc)))) || removed) {
		    vc = atm_hashout_vc(atm, vc->vc);
		    vc -> next_vc = NULL;
		    if ((vc->swidb->atm_glommable_vc_tree != NULL) && 
			atm_is_glommable_vc(vc))
			    wavl_delete(vc->swidb->atm_glommable_vc_tree, &vc->wavl);
		    reg_invoke_atm_vc_removed(vc);
		    free(vc);
		}
		vc = next_vc;
	    }
	}
    }
    if (re_run)
	atm_free_vc_flag = TRUE;
}


/*
 * atm_update_rateq:	called to change the status of (i.e. enable/disable)
 *			a rateq.
 * INPUT:
 *	ptr to atm_db
 *	ptr to rq_param, which contains the necessary info for the change.
 *		
 * OUTPUT/RETURN:
 *	void
 */
void atm_update_rateq (atm_db_t *atm, rateq_param_t *rq_param)
{
    if (atm==NULL || rq_param==NULL || rq_param->rq_id >= atm->ratequeue_no)
	return;
    
    atm->rateq[rq_param->rq_id].rate = rq_param->rq_rate;
    atm->rateq[rq_param->rq_id].state = rq_param->rq_state;
}

/*
 * atm_rateq_ratesum:	called to calculate the total aggregated bandwith
 * 			currently being allocated to all rateq's
 * INPUT:
 *        ptr to atm_db.
 * OUTPUT/RETURN:
 *	total bandwith in Kbps
 */
unsigned atm_rateq_ratesum (atm_db_t *atm)
{
    ushort ndx;
    unsigned trate = 0;
    
    for (ndx = 0; ndx < atm->ratequeue_no; ndx++) {
	if (atm->rateq[ndx].state != RATE_QUEUE_DISABLED)
	    trate += atm->rateq[ndx].rate;
    }
    return (trate);
}
	

/*
 * NOTE: This call only returns ACTIVE vcs.
 * atm_vpivci_to_vc:	Given a VPI/VCI & ATM_DB, return the vcinfo
 *	structure, if any.
 * OUTPUT/RETURN:
 *	Returns ptr. to vc_info_t or NULL if non match the VPI/VCI.
 * INPUT:
 *	atm_db == ptr. to ATM descriptor block
 *	vpi == VPI
 *	vci == VCI....Duh!
 */
vc_info_t *
atm_vpivci_to_vc (atm_db_t *atm, unsigned vpi, unsigned vci)
{
    vcd_t vcnum;
    vc_info_t *vc_info;

    if(!atm) {
	return(NULL);
    }
    for(vcnum = 1; vcnum < atm->config.max_intf_vcs; vcnum++) {
	vc_info = atm_getvc(atm, vcnum);
	if (!vc_info) {
	    continue;
	}
	if (vc_info->vpi == vpi && vc_info->vci == vci) {
	    return(vc_info);
	}
    }
    return(NULL);
}

/*
 * NOTE: This call returns both ACTIVE and INACTIVE vcs.
 * atm_vpivci_to_vc_all: Given a VPI/VCI & ATM_DB, return the vcinfo
 *      structure, if any. 
 * OUTPUT/RETURN:
 *      Returns ptr. to vc_info_t or NULL if non match the VPI/VCI.
 * INPUT:
 *      atm_db == ptr. to ATM descriptor block
 *      vpi == VPI
 *      vci == VCI....Duh!
 */
vc_info_t *
atm_vpivci_to_vc_all (atm_db_t *atm, unsigned vpi, unsigned vci)
{
    vcd_t vcnum;
    vc_info_t *vc_info;

    if(!atm) {
        return(NULL);
    }
    for(vcnum = 1; vcnum < atm->config.max_intf_vcs; vcnum++) {
        vc_info = atm_getvc_all(atm, vcnum);
        if (!vc_info) {
            continue;
        }
        if (vc_info->vpi == vpi && vc_info->vci == vci) {
            return(vc_info);
        }
    }
    return(NULL);
}

/*
 * NOTE: This call only returns ACTIVE vcs
 * atm_getvc:	Find & return the VC struct. associated with the vc no.
 *	specified.
 * OUTPUT/RETURN:
 *	Returns ptr. to the VC infor struct associated with a
 *	particular VC.  NULL is returned for a nonexistent VC.
 * INPUT:
 *	atm == Ptr. to the ATM struct. to use.
 *	vc == Int. value of the VC to look up in the ATM hash table.
 */
vc_info_t *
atm_getvc (atm_db_t *atm, vcd_t vcd)
{
    return(atm_getvc_inline(atm,vcd));
}

/*
 * NOTE: This call returns both ACTIVE and INACTIVE vcs
 * atm_getvc_all: Find & return the VC struct. associated with the vc no.
 *      specified.  return up and down vc's...
 * OUTPUT/RETURN:
 *      Returns ptr. to the VC infor struct associated with a
 *      particular VC.  NULL is returned for a nonexistent VC.
 * INPUT:
 *      atm == Ptr. to the ATM struct. to use.
 *      vc == Int. value of the VC to look up in the ATM hash table.
 */
vc_info_t *
atm_getvc_all (atm_db_t *atm, vcd_t vc)
{
    vc_info_t *cur;
 
    if(!atm->vc_hash_tbl)
        return(NULL);

    cur = atm->vc_hash_tbl[HASHVC(atm, vc)];
 
    while (cur) {
        if (cur->vc == vc) {
	    /* vc is deleted but hardware failed to perform */
	    if (IS_VC_DELETED(cur))
		return(NULL);
            return(cur);
	}
 
        cur = cur->next_vc;
    }
    return(NULL);
}
/*
 * NOTE: This call returns any pvcs including deleted ones
 * atm_getvc_deleted: Find & return the VC struct. associated with the vc no.
 *      specified.  
 * OUTPUT/RETURN:
 *      Returns ptr. to the VC infor struct associated with a
 *      particular VC.  NULL is returned for a nonexistent VC.
 * INPUT:
 *      atm == Ptr. to the ATM struct. to use.
 *      vc == Int. value of the VC to look up in the ATM hash table.
 */
vc_info_t *
atm_getvc_deleted (atm_db_t *atm, vcd_t vc)
{
    vc_info_t *cur;
 
    if(!atm->vc_hash_tbl)
        return(NULL);

    cur = atm->vc_hash_tbl[HASHVC(atm, vc)];
 
    while (cur) {
        if (cur->vc == vc) {
            return(cur);
	}
 
        cur = cur->next_vc;
    }
    return(NULL);
}

/*
 * atm_getvcaddr: Retrieve called party address from vc structure
 * associated with vc number in ARP's hardware address
 *
 * OUTPUT/RETURN:
 *	Returns ptr. to called party address in VC structure. 
 *  	NULL if VC cannot be found or if VC is a PVC.
 * INPUT:
 *	idb == Ptr. to software IDB
 *	hardware == Ptr to hardware address in ARP entry
 */
static hwaddrtype *
atm_getvcaddr (idbtype *idb, uchar *hardware)
{
    vc_info_t *vc_info;

    vc_info = atm_getvc_all(idb->hwptr->atm_db, GETSHORT(hardware));
    if (!vc_info)
	return(NULL);
    if (IS_ATMVC_PVC(vc_info)) 
	return(NULL);

    return(&(vc_info->calledparty));
}


/*
 * Here are the point-2-point interfac functions to deal with the
 * sole VCC on a p2p interface.
 */
 
/*
 * atm_get_p2pvc: Called to fetch *THE* VCC assigned to this subint.
 *	if it is a point-to-point interface.  In this case, there can
 *	only be 1 VCC assigned to it and we use it for all outbound
 *	traffic.
 * OUTPUT/RETURN:
 *	Returns ptr. to the vc_info struct., if a VCC exists.
 *	Else, returns NULL
 * INPUT:
 *	atm -> ptr. to the ATM descriptor
 *	idb -> ptr. to the swidb in question
 */
static vc_info_t *
atm_get_p2pvc (idbtype *idb, boolean want_actives_only)
{
    vc_info_t *vc_info;
    vcd_t 	vcnum = 0;
    
    if (!idb->p2pmac) {
	return(NULL);
    }
    bcopy(idb->p2pmac->addr, (char *)&vcnum, STATIONLEN_ATMVC);
    if (want_actives_only)	{
	 vc_info = atm_getvc_inline(idb->hwptr->atm_db, vcnum);
    }
    else {
         vc_info = atm_getvc_all(idb->hwptr->atm_db, vcnum);
    }
    return(vc_info);
}

/*
 * atm_set_p2pvc: Called to set the current VCC as the p2p VCC on
 * 	the specified ATM interface.
 */
static boolean
atm_set_p2pvc (idbtype *idb, vc_info_t *vc_info)
{
    if (idb->p2pmac) {
	return(FALSE);
    }
    idb->p2pmac = malloc(sizeof(hwaddrtype));
    if (!idb->p2pmac) {
	return(FALSE);
    }
    idb->p2pmac->type = STATION_ATMVC;
    idb->p2pmac->length = STATIONLEN_ATMVC;
    bcopy(&vc_info->vc, idb->p2pmac->addr, STATIONLEN_ATMVC);
    return (TRUE);
}

/*
 * atm_clear_p2pvc:	Called to remove a VCC from a p2p interface
 */
static void
atm_clear_p2pvc (vc_info_t *vc_info) 
{
    idbtype *idb;

    idb = vc_info->swidb;
    if(!idb->p2pmac) {
	return;
    }
    free(idb->p2pmac);
    idb->p2pmac = NULL;
}

/*
 * atm_get_aal34vcnum:	Fetch the AAL3/4 VC associated with a specific
 *		IDB.
 * OUTPUT/RETURN:
 *	vcnum - An int. to the VCNUM associated with this IDB, or ZERO
 * INPUT:
 *	idb - Ptr. to the Virtual IDB to look for.
 */
uint
atm_get_aal34vcnum (idbtype *idb)
{
    atm_db_t *atm = idb->hwptr->atm_db;
    vc_info_t *vc_info;
    vcd_t vcd;

    for(vcd = 1; vcd < atm->config.max_intf_vcs; vcd++) {
	vc_info = atm_getvc(atm, vcd);
	if (vc_info && (vc_info->swidb == idb) && ISVCAAL34(vc_info)) {
	    return(vc_info->vc);
	}
    }
    return(0);
}

/*
 * atm_getvc_counts: Retrieve the latest vc counters associated with a
 *	particular hwidb/vcd.
 * OUTPUT/RETURN:
 *	in_pkts, out_pkts - The latest statistics off of the ATM interface.
 *	Obtained by calling the getvc_stats vector.
 * INPUT:
 *	hwidb, vcd - The specific vcd to look up stats.
 */
void
atm_getvc_counts (hwidbtype *hwidb, 
		  vcd_t vcd,
		  unsigned *in_pkts,
		  unsigned *out_pkts)
{
    vc_info_t *vc_info;
    atm_db_t *atm;

    *in_pkts	= 0;
    *out_pkts	= 0;

    if (hwidb) {
	vc_info = atm_getvc(hwidb->atm_db, vcd);
	if (vc_info) {
	    atm = hwidb->atm_db;
	    hwidb->atm_db->funcs.getvc_stats(hwidb, vc_info);
	    ATM_INAS(atm, vc_info);
	    ATM_OUTAS(atm, vc_info);
	    *in_pkts	= vc_info->total_in_pkts;
	    *out_pkts	= vc_info->total_out_pkts;
	}
    }
}

/* atm_getvc_mib_id 
 * for retrieving the port, VPI, VCI for the LEC MIB 
 * INPUTS:
 *   vc_vcd  == VCD for the VC to find
 *   swidb == idb for the interface 'owning' the vcd
 * OUTPUTS:
 *   *vc_port, vpi, vci == VC identifiers
 * RETURNS:
 *   TRUE if VC found, FALSE, if not found or param error 
 */
boolean 
atm_getvc_mib_id (vcd_t    vc_vcd,
		  hwidbtype  *hwidb,
		  long     *vc_port,
		  long     *vc_vpi,
		  long     *vc_vci)
{
    vc_info_t  *vc;

    /* make sure pointers are valid */
    if (hwidb && hwidb->atm_db) {
	/* try to retrieve the vc_info struct */
	vc = atm_getvc(hwidb->atm_db, vc_vcd);
	if (vc) {
	    *vc_port = (long)vc->port_index;
	    *vc_vpi = (long)vc->vpi;
	    *vc_vci = (long)vc->vci;
	    return TRUE;
	}
    }
    return FALSE;
}  /* atm_getvc_mib_id */

/*
 * atm_getvc_full_counts: Retrieve the latest vc counters associated with a
 *	particular hwidb/vcd.
 * OUTPUT/RETURN:
 *	MIB_stats struct filled in with counts...only
 *      a few IF-MIB counters are supported at this time
 *      The latest statistics off of the ATM interface.
 *	Obtained by calling the getvc_stats vector.
 * INPUT:
 *	hwidb, vcd - The specific vcd to look up stats.
 */
boolean
atm_getvc_full_counts (hwidbtype *hwidb, 
		       vcd_t vcd,
		       atm_real_stats_t  *stats)
{
    vc_info_t *vc_info;
    atm_db_t *atm;

    if (hwidb) {
	vc_info = atm_getvc(hwidb->atm_db, vcd);
	if (vc_info) {
	    atm = hwidb->atm_db;
	    hwidb->atm_db->funcs.getvc_stats(hwidb, vc_info);
	    ATM_INAS(atm, vc_info);
	    ATM_OUTAS(atm, vc_info);

	    stats->inOctets = vc_info->in_bytes;
	    stats->outOctets = vc_info->out_bytes;
	    stats->inPkts = vc_info->total_in_pkts;
	    stats->outUcastPkts = vc_info->total_out_pkts -
		vc_info->broadcasts;
	    stats->outNUcastPkts = vc_info->broadcasts;
	    return TRUE;
	}
    }
    return FALSE;
}


/*
 * atm_getvcnum:	Returns the next available VC number
 * OUTPUT/RETURN:
 *	vcd_t value of the next VC number available or all ones if all used up.
 * INPUT:
 *	hwidb -> ptr. of the ATM to use for allocation
 */
vcd_t
atm_getvcnum (hwidbtype *hwidb)
{
    register vcd_t vcnum;
    vc_info_t *vc_info = NULL;
    atm_db_t *atm = hwidb->atm_db;

    /*
     * If it's the first time on this int., allocate the hash table.
     * DON'T do this at atm_setup() time since that routine can get
     * called from interrupt and we can't malloc() in interrupt level.
     */
    if (!atm->vc_hash_tbl) {
	atm->vc_hash_tbl = malloc(sizeof(vc_info_t *) * atm->vchash_buckets);
	if (!atm->vc_hash_tbl) {
	    return(ATM_OUT_OF_VCDS);
	}
    }
    vcnum = atm->last_vcd + 1;
    if (vcnum == 0 || vcnum >= atm->config.max_intf_vcs) {
	vcnum = 1;
    }
    while (vcnum != atm->last_vcd) {
	/* even deleted vcs aren't available until removed from hash table*/
        vc_info = atm_getvc_deleted(atm, vcnum);
	if (!vc_info) {
	    atm->last_vcd = vcnum;
	    return(vcnum);
	}
        vcnum++;
        if (vcnum >= atm->config.max_intf_vcs) {
            vcnum = 1;
        }
    }
    return(ATM_OUT_OF_VCDS);
}

/*
 * compare_glom_vcs
 *
 * This is the comparison routine for the WAVL tree to call to compare each 
 * node with goal node. 
 * The keys here are (NSAP address + ATM Encap + protocol if encap is AAL5MUX).
 */
static enum avl_compare
compare_glom_vcs (avl_node_type *n1, avl_node_type *n2)
{
    vc_info_t *node1, *node2;
    int result, encap1, encap2;

    node1 = wavl_to_vcinfo(wavl_normalize(n1, 0));
    node2 = wavl_to_vcinfo(wavl_normalize(n2, 0));

    /* 
     * First compare NSAP address lengths of the nodes, if equal then 
     * compare the NSAP addresses of the nodes 
     */
    if (node1->calledparty.length < node2->calledparty.length)
	return(AVL_LT);
    if (node1->calledparty.length > node2->calledparty.length)
	return(AVL_GT);
    /*
     * Compare NSAP addresses of nodes, if equal, then compare the AAL5
     * encapsulation.
     */
    result = memcmp(node1->calledparty.addr, node2->calledparty.addr, 
	node1->calledparty.length);
    if (result < 0)
        return(AVL_LT);
    if (result > 0)
        return(AVL_GT);

    /*
     * If the AAL5 encapsulation is same for the two nodes, then return equal
     * if encapsulation is ATM_VC_AAL5SNAP, else go ahead and compare the 
     * protocol type under ATM_VC_AAL5MUX encapsulation.
     */
    encap1 = node1->flags & VC_FLAG_AAL_MASK;
    encap2 = node2->flags & VC_FLAG_AAL_MASK;

    if (encap1 < encap2)
        return(AVL_LT);
    if (encap1 > encap2)
        return(AVL_GT);

    /* Comparing protocol type under ATM_VC_AAL5MUX encapsulation */
    if (encap1 == encap2 && encap1 == ATM_VC_AAL5MUX) {
	if (node1->linktype < node2->linktype)
	    return(AVL_LT);
	if (node1->linktype > node2->linktype)
            return(AVL_GT);
    }

    return(AVL_EQ);
}

/*
 * atm_wavl_to_idb
 *
 * Given a pointer to the glommable VC WAVL tree node, return a pointer to the
 * idb structure holding it.
 */
static void *
atm_wavl_to_idb (wavl_node_type *node)
{
    return (void *)&((idbtype *)0)->atm_glommable_vc_tree;
}

/*
 * atm_init_wavl_tree
 *
 * Function to allocate WAVL handle per swidb
 * This is always safe to call, no checking needed.
 */
static void
atm_init_wavl_tree (idbtype *swidb)
{
    if (swidb->atm_glommable_vc_tree == NULL) {
        swidb->atm_glommable_vc_tree = malloc(sizeof(wavl_handle));
        if (swidb->atm_glommable_vc_tree != NULL) {
            if (wavl_init(swidb->atm_glommable_vc_tree, 1, atm_wavl_to_idb, 
			  compare_glom_vcs))
		free(swidb->atm_glommable_vc_tree);
        }
    }
}
/*
 * atm_activate_pvc: bring a pvc that has been de-activated by a shut
 *                   or a line protocol going down
 *   OUTPUT/RETURN: none
 *
 *   INPUT: hwidb -> ptr to the atm interface
 *          vc -> ptr to the vc
 */
void
atm_activate_pvc (hwidbtype *hwidb,vc_info_t *vc)
{
    atm_db_t *atm = hwidb->atm_db;

    /* should never happen */
    if (atm == NULL)
	return;
    /* only activate pvc */
    if (!(vc->flags & VC_FLAG_PVC))
	return;
    
    if ((*atm->funcs.setupvc)(hwidb,vc)) {
	reg_invoke_atm_vc_activated(vc);
	atm_oam_enable_timer(vc, vc->oam);
 	if (ISVCACTIVE(vc))
 	    atm->num_vc++;
    }
}

/*
 * atm_create_vc: Create a VC struct, initialize the struct & hash it
 *	into the ATM hash structure.
 * OUTPUT/RETURN:
 *	Returns a ptr., of vc_info_t type, or NULL if unsuccessful.
 * INPUT:
 *	hwidb == ptr. to the ATM in question.
 *	call_addr == NSAP address of the called party address
 *	qos == Quality of Service requested on this VC
 *	rateq == Bandwith requirement for this VC
 *	mtu == Maximum MTU size for this VC(<= ATM Max MTU)
 *	flags == One of the VC flag types for AAL & ENCAP stuff
 *	protocol == LinkType for protocols on MUX VCs
 *	user_vc_type == user identifier for this VC...used by LANE.
 *	idb == ptr. to the SWIDB associated with this VC.
 *	inarp == (0 == Disabled), (> 0 minutes between InARP packets)
 *	oam  == (0 == Disabled), (> 0 seconds between OAM cells)
 */
vc_info_t *
atm_create_vc (hwidbtype *hwidb, vcd_t vcnum, uint vpi, uint vci, 
	       hwaddrtype call_addr, uint qos, uint rateq, uint mtu, 
	       uint flags, uint protocol, user_vc_type_t user_vc_type,
	       idbtype *idb, uint peak, uint avg, uint burst, 
	       uint midlow, uint midhigh,
	       uint inarp, uint oam)
{
    vc_info_t *vc = NULL, *ovc = NULL;
    atm_db_t *atm = hwidb->atm_db;
    unsigned ndx, numidbvc;
    ushort max_vpivci;

    if ((flags & VC_FLAG_SVC) && !atm_interface_up(idb)) {
	errmsg(&msgsym(FAILCREATEVC,ATM), vcnum,vpi,vci,
	       idb->namestring,"Cannot create SVC on shutdown interface");
	return(NULL);    /* sub-interface is shutdown */
    }
    
    /* are the vpi/vci values in a valid range */
    max_vpivci = atm_calc_max_vpivci(atm->config.max_vpi_bits);
    if (vpi > max_vpivci) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring, "VPI value too large.");
	    return(NULL);
    }
    max_vpivci = atm_calc_max_vpivci(atm->config.max_vci_bits);
    if (vci > max_vpivci) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring, "VCI value too large.");
	    return(NULL);
    }
	
    /*
     * If it's the first time on this int., allocate the hash table.
     * DON'T do this at atm_setup() time since that routine can get
     * called from interrupt and we can't malloc() in interrupt level.
     */
    if (!atm->vc_hash_tbl) {
	atm->vc_hash_tbl = malloc(sizeof(vc_info_t *) * atm->vchash_buckets);
	if (!atm->vc_hash_tbl) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring, "No memory available");
	    return(NULL);
	}
    }
    /* 
     * Allocate the VC WAVL tree handle for each swidb if not done yet. 
     * This needs to be moved in a registry per swidb.
     */
    atm_init_wavl_tree(idb);

    if (vcnum >= atm->config.max_intf_vcs) {
	errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
	       hwidb->hw_namestring, "VCD exceeds maximum");
	return(NULL);			/* Exceeded maxvc limit */
    }

    if (inarp && ((flags & VC_FLAG_AAL_MASK) != ATM_VC_AAL5SNAP)) {
	errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
	       hwidb->hw_namestring, "InARP allowed on AAL5-SNAP VCs only");
 	return(NULL);
    }

/*
 * Search ALL VCs for this interface & ensure that the same VPI/VCI pair
 * isn't being reused.
 */
    numidbvc = 0;
    for(ndx = 0; ndx < atm->vchash_buckets; ndx++) {
        uint vcmask = atm->config.rt_vc_per_vp - 1;
	vc = atm->vc_hash_tbl[ndx];
	while(vc) {
	    if(ISVCACTIVE(vc) || (IS_ATMVC_PVC(vc) &&
				  (!(IS_VC_DELETED(vc))))) {
		if(idb == vc->swidb) {
		    numidbvc++;
		    if ((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL34SMDS) {
			errmsg(&msgsym(FAILCREATEVC,ATM),vcnum,vpi,vci,
			       hwidb->hw_namestring,
			       "AAL3/4-SMDS VC already configured on interface"
			       ". It must be the only VC on an interface.");
			return(NULL);
		    }
		    
		}
		if (vc->vpi == vpi && (vc->vci & vcmask) == (vci & vcmask)) {
		    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
			   hwidb->hw_namestring, "ACTIVE vpi/vci pair already in use");
		    if (atm_errors_debug)
			buginf("\n%%Create VC failed, ACTIVE vpi/vci pair already in use by VCD=%d, VPI=%d, VCI=%d", vc->vc, vc->vpi, vc->vci);
		    return(NULL);
		}
		if(vc->vc == vcnum) {
		    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
			   hwidb->hw_namestring, "ACTIVE VCD is already in use");
		    return(NULL);
		}
	    } else {
	        if((atm->flags & ATM_FLAG_UP) && 
		    atm->config_status == ATM_CONFIG_OK) {
		    if(vc->vc == vcnum) 
		        ovc = vc;
	        } else {
		    if (vc->vpi == vpi && (vc->vci & vcmask) == (vci & vcmask)) {
			errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
			       hwidb->hw_namestring,
			       "vpi/vci pair already in use");
			if (atm_errors_debug)
			   buginf("\n%%Create VC failed, vpi/vci pair already in use by VCD=%d, VPI=%d, VCI=%d", vc->vc, vc->vpi, vc->vci); 
		        return(NULL);
		    }
		    if (vc->vc == vcnum) {
			errmsg(&msgsym(FAILCREATEVC,ATM), vcnum,
			       vpi, vci, hwidb->hw_namestring,
			       "VCD is already in use");
		        return(NULL);
		    }
		}
	    }
	    vc = vc->next_vc;
	}
    }

    if (is_p2p(idb)) {
	if (atm_get_p2pvc(idb,FALSE)) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring,
		   "Only one VCC allowed on a Point-to-Point interface");
	    return(NULL);
	}
    }

    if ((flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL34SMDS) {
	if (numidbvc > 0) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring,
		   "AAL3/4-SMDS VC must be the only VC on an interface");
	    return(NULL);
	}
	
	if (midhigh < midlow) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring,
		   "AAL3/4-SMDS MID-Low must be LESS than MID-High");
	    return(NULL);
	}
	if ((midhigh || midlow)&&(midhigh-midlow > atm->config.mid_per_vc-1)) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   hwidb->hw_namestring,
		   "AAL3/4-SMDS MID exceeds maximum");
	    return(NULL);
	}
    }
/*
 * Allocate space for the VC info struct. 
 */
    if(!ovc) {
    	if (!(vc = malloc(sizeof(vc_info_t))))
	    return(NULL);
    } else {
	vc = ovc->next_vc;
	memset(ovc, 0, sizeof(vc_info_t));
	ovc->next_vc = vc;
	vc = ovc;
    }
/*
 * At this point, we assume we have an allocated VC struct that has
 * been ZERO initialized!
 */
    vc->vpi = vpi;
    vc->vci = vci;
    vc->vc = vcnum;
    vc->qos = qos;
    vc->calledparty = call_addr;
    vc->flags = flags;			/* AAL, Encaps, MUX? */
    vc->swidb = idb;
    vc->peakrate = peak;
    vc->avgrate = avg;
    vc->burstcells = burst;
    vc->mid_start = midlow;
    vc->mid_end = midhigh;
    vc->oam_status = ATM_OAM_DISABLED;
    vc->inarp = inarp * ONEMIN;

    /* svcs are still in actual cell amounts which may need to
     * rounded up to the resolution of the platform dependent
     * burst parameter
     */
    if (IS_ATMVC_SVC(vc)) {
	vc->burstcells = burst / atm->burstcells_factor;
	if ((burst % atm->burstcells_factor) != 0) {
	    vc->burstcells++;
	}
	vc->burstcells *= atm->burstcells_factor;
    }
	
    /*
     *  SVCs have an idle timeout timer, unless they were created with the
     *  NOIDLETIMER flag set, or the whole ATM interface has idle timers
     *  disabled.
     */
    if (IS_ATMVC_SVC(vc) && !(flags & VC_FLAG_NOIDLETIME) &&
	    atm->idle_time != 0) {
	TIMER_START(vc->idle_timer, atm->idle_time);
    } else {
	TIMER_STOP(vc->idle_timer);
    }

    switch(vc->flags & VC_FLAG_AAL_MASK) {
    case ATM_VC_AAL5SNAP:
	vc->encapsize = ATM_SNAP_ENCAPBYTES;
	vc->etype = ATM_ENCTYPE_SNAP;
	break;
    case ATM_VC_AAL5NLPID:
	vc->encapsize = ATM_NLPID_ENCAPBYTES;
	vc->etype = ATM_ENCTYPE_NLPID;
	break;
    case ATM_VC_AAL5FRNLPID:
	vc->encapsize = ATM_FRNLPID_ENCAPBYTES;
	vc->etype = ATM_ENCTYPE_FRNLPID;
	break;
    case ATM_VC_AAL34SMDS:
	if (!(atm->config.mode & ATM_CFG_MODE_AAL34)) {
	    errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
		   idb->namestring, "SMDS not enabled");
	    free(vc);
	    return(NULL);
	}
	vc->encapsize = ATM_SMDS_ENCAPBYTES;
	vc->etype = ATM_ENCTYPE_SMDS;

	/*
	 * force process level bridging for aal3/4.
	 * unfortunately, it'll force process level bridging
	 * for the entire hwidb!!!
	 * hosed for aal5...
	 */
	hwidb->span_bridge_process_force |= SPAN_BR_PROC_ATM_BRIDGE;
	break;
    case ATM_VC_AALQSAAL:
        vc->encapsize = ATM_MUX_ENCAPBYTES;
        vc->etype = ATM_ENCTYPE_SAAL;
        atm->sig_vcnum = vcnum;
        break;
    case ATM_VC_AAL5MUX:
	vc->encapsize = ATM_MUX_ENCAPBYTES;
	vc->linktype = protocol;
    	switch(protocol) {
    	case LINK_IP:
	    vc->etype = TYPE_IP10MB;
	    break;
    	case LINK_XNS:
	    vc->etype = TYPE_XNS;
	    break;
    	case LINK_APPLETALK:
	    vc->etype = TYPE_ETHERTALK;
	    break;
    	case LINK_CLNS:
    	case LINK_CLNS_ALL_ES:
    	case LINK_CLNS_ALL_IS:
        case LINK_CLNS_BCAST:
    	case LINK_ISIS_ALL_L1_IS:
    	case LINK_ISIS_ALL_L2_IS:
	    vc->etype = TYPE_CLNS;
	    break;
    	case LINK_DECNET:
	case LINK_DECNET_NODE:
	case LINK_DECNET_ROUTER_L1:
	case LINK_DECNET_ROUTER_L2:
        case LINK_DECNET_PRIME_ROUTER:
	    vc->etype = TYPE_DECNET;
	    break;
    	case LINK_PAD:
	    vc->etype = 0;
	    break;
    	case LINK_NOVELL:
	    vc->etype = TYPE_NOVELL1;
	    break;
    	case LINK_APOLLO:
	    vc->etype = TYPE_APOLLO;
	    break;
    	case LINK_VINES:
	    vc->etype = TYPE_VINES;
	    break;
    	case LINK_ILLEGAL:
    	default:
	    vc->etype = 0;
	    break;
    	}
	break;
    case ATM_VC_AAL5LANE:
	vc->user_vc_type = user_vc_type;

	switch (user_vc_type) {
	case LANE_LEC_DIRECT:
	case LANE_LEC_DISTRIBUTE:
	case LANE_LEC_CONFIGURE:
	case LANE_LES_DIRECT:
	case LANE_LES_DISTRIBUTE:
	case LANE_CONFIGURE:
	    vc->encapsize = ATM_LANE_CONTROL_ENCAPBYTES;
	    vc->etype = ATM_ENCTYPE_LANE;
	    break;
	case LANE_LEC_DATA:
	    vc->encapsize = ATM_LANE_DATA_ENCAPBYTES;
	    vc->etype = ATM_ENCTYPE_LANE;
	    break;
	case LANE_LEC_TR_DATA:
	    vc->encapsize = ATM_LANE_DATA_ENCAPBYTES;

            /* Microcode pads all ATM_ENCTYPE_LANE VCs to the minimum ethernet size */
	    vc->etype = ATM_ENCTYPE_LANE_BUS;
	    break;
        /*
         * Vcc for Packets arriving at the BUS for reflection out the
         * bus-forward vcc are set to "ATM_ENCTYPE_RAW" to make sure the SP
         * doesn't try to do anything to the packets (like decrement TTL).
         */
	case LANE_BUS_SEND:
	case LANE_BUS_TR_SEND:
            vc->encapsize = ATM_LANE_DATA_ENCAPBYTES;
            vc->etype     = ATM_ENCTYPE_RAW;
            break;
	case LANE_LEC_SEND:
	case LANE_LEC_TR_SEND:
	case LANE_LEC_FORWARD:
	case LANE_LEC_TR_FORWARD:
	case LANE_BUS_FORWARD:
	case LANE_BUS_TR_FORWARD:
	    vc->encapsize = ATM_LANE_DATA_ENCAPBYTES;
	    vc->etype = ATM_ENCTYPE_LANE_BUS;
	    break;
	default:
	    if (atm_errors_debug) {
		buginf("\n%%: Create VC failed, unknown LANE vc_type");
	    }
	    free(vc);
	    return(NULL);
	    break;
	}

	break;
    case ATM_VC_AAL5ILMI:	
    /*
     * Looks just like a MUX, but the etype is UNKNOWN
     */
	vc->etype = 0;
	if (atm->ilmi_vc) {
	    free(vc);
	    return(NULL);	/* Already have an ILMI VCD */
	}
	atm->ilmi_vc = vcnum;
	break;
    default:
	if (atm_errors_debug) {
	    buginf("\n%%: Create VC failed, unknown type");
	}
	free(vc);
	return(NULL);
    }

/*
 * Pass commands to ATM interface to activate a VC connection to VPI/VCI
 * with the service parameters supplied
 */
    if(!(*atm->funcs.setupvc)(hwidb, vc)) {
	errmsg(&msgsym(FAILCREATEVC,ATM), vcnum, vpi, vci,
	       hwidb->hw_namestring,"Failed to have the driver to accept the VC"
);
	vc->flags &= ~VC_FLAG_ACTIVE;
	free(vc);
	return(NULL);
    }

    if (ISVCACTIVE(vc))
	atm->num_vc++;			/* Count it as valid */

    /*
     * Hash this sucker into our list of VCs
     */
    if(!ovc) {
	atm_hashin_vc(atm, vc);
    }
    /*
     * If it is a candidate for glomming and we do not have an entry for this 
     * NSAP address already, we insert it into the idb's WAVL tree
     */
    if (atm_is_glommable_vc(vc) && (idb->atm_glommable_vc_tree != NULL)) {
	if (!wavl_search(idb->atm_glommable_vc_tree, &vc->wavl, 0))
	    wavl_insert(idb->atm_glommable_vc_tree, &vc->wavl);
    }

    if (is_p2p(idb)) {
	    atm_set_p2pvc(idb, vc);
    }

    
    /* 
     *  Send a trap to the adjacent ILMI. (If ILMI enterprise traps are not
     *  disabled.)
     */
	if (vc->flags & VC_FLAG_PVC) {
	    if (atm->disable_ilmi_traps == FALSE) {
		reg_invoke_atm_ilmi_send_trap (hwidb->hw_if_index,
					       ilmiVCCChange,vpi,vci,
					       ilmiLocalUpEnd2endUnknown);
	    }
	}

    
    if (idb->tbridge_on_swidb)
	if ((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5SNAP)
	    reg_invoke_media_tbridge_swidb_vc_enqueue(ET_ATM, idb, vc);

    reg_invoke_atm_vc_created(vc);
    atm_oam_enable_timer(vc, oam);
    reg_invoke_atm_vc_activated(vc);


    return(vc);
}

/*
 * atm_inactivate_pvc: used to take down a pvc but preserve it's data
 *                     structures for re-activation
 *  OUTPUT/RETURN: none
 *
 *  INPUT: hwidb -> ptr. to atm interface
 *         vc -> ptr. to vc struct
 */
void
atm_inactivate_pvc (hwidbtype *hwidb,vc_info_t *vc)
{
    boolean removed;
    
    atm_db_t *atm = hwidb->atm_db;
    /* should never happen */
    if (atm == NULL)
	return;

    /* only remove pvc */
    if (!(vc->flags & VC_FLAG_PVC))
	return;

    removed = (*atm->funcs.teardownvc)(hwidb,vc);
    
    /* did vc actually get removed in interface? */
    if (!removed) {
	/* no set it to down state */
	vc->flags |= VC_FLAG_PVC_DOWN;
	
	/* set it to inactive as well */
	vc->flags &= ~VC_FLAG_ACTIVE; 

        /* enable search algorithm in atm_periodic */
	atm_free_vc_flag = TRUE;
    }

	reg_invoke_atm_vc_deactivated(vc);
	atm_oam_disable_timer(vc);
}

/*
 * atm_remove_vc(hwidb, vc)
 *	Used to destroy a VC and remove config. information
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	hwidb -> ptr. to the ATM interface 
 *	vc == VC number to remove
 */
void
atm_remove_vc (hwidbtype *hwidb, unsigned vcnum)
{
    vc_info_t *vc_info;
    atm_db_t *atm = hwidb->atm_db;
    idbtype *swidb;
    uint in_interrupt;
    boolean removed = TRUE;
    leveltype level;

/*
 * Scan list for this VC.  Return if not found, else unlink it.
 */
    if(!atm->vc_hash_tbl)
	return;

    in_interrupt = onintstack();

    vc_info = atm_getvc_all(atm, vcnum);

    if (!vc_info) {
        if (atm_errors_debug)
            buginf("\nATM failed to remove VC(VCD=%d) on interface %s. The VC does not exist.", vcnum, hwidb->hw_namestring);
        return;
    }

    if ((vc_info->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5ILMI) {
	atm->ilmi_vc = 0; /* Deconfig. ILMI VCD */
    }

      /*
       * Flush out caches for this interface...... only if the 
       * VC_FLAG_NOBUMP_CACHE flag is not set to prevent thrashing when
       * control VCs go down.
       * Do it now before deconfiguring the VC.  This shuts off A/S to
       * a VC while we're deconfiguring it. 
       */
      if (!(vc_info->flags & VC_FLAG_NOBUMP_CACHE)) {
	  reg_invoke_bump_cache_version(hwidb, vc_info->swidb, TRUE);
      }
  
    /* remove VC from tbridge linked list if bridging configured */
    swidb = vc_info->swidb;
    if (swidb->tbridge_on_swidb)
	reg_invoke_media_tbridge_swidb_vc_unqueue(ET_ATM, swidb, vc_info);

    /*
     * Call to disable OAM processing.
     */
    atm_oam_disable_timer(vc_info);

    /*
     *  Send ATM interface commands to disable this VC.
     */
    if (ISVCACTIVE(vc_info)) {
	removed = (*atm->funcs.teardownvc)(hwidb, vc_info);
	atm->num_vc--;			/* Mark it down */
    }


    /* remove signalling vc */
    if (vc_info->etype == ATM_ENCTYPE_SAAL)
       atm->sig_vcnum = 0;

    if (is_p2p(vc_info->swidb)) {
    	atm_clear_p2pvc(vc_info);
    }

    /* Send a trap to the adjacent ILMI. (If ILMI enterprise traps are not
     *  disabled.)
     */

    if (vc_info->flags & VC_FLAG_PVC) {
	if (atm->disable_ilmi_traps == FALSE) {
	    reg_invoke_atm_ilmi_send_trap (hwidb->hw_if_index,ilmiVCCChange,
					   vc_info->vpi,vc_info->vci,
					   ilmiLocalDown);
	}
    }

    /* did vc actually get removed in interface? */
    if (!removed) {
	level = raise_interrupt_level(NETS_DISABLE);

	/* no set it to hold state */
	vc_info->flags |= VC_FLAG_DELETED;
	
	/* set it to inactive as well */
	vc_info->flags &= ~VC_FLAG_ACTIVE; 

        /* enable search algorithm in atm_periodic */
	atm_free_vc_flag = TRUE;

	/*
	 * Unlink it from the InARP chain if necessary
	 */
	if (vc_info->inarp) {
	    atm_unlink_inarp_vc(atm,vc_info);
	}
	reset_interrupt_level(level);

	return;
    }

    /* hardware succeeded in removing vc */
    if (in_interrupt)			/* at interrupt level */
	atm_free_vc_flag = TRUE;
    else {				/* at process level */
        vc_info = atm_hashout_vc(atm, vc_info->vc);

	if ((vc_info->swidb->atm_glommable_vc_tree != NULL) &&
	    atm_is_glommable_vc(vc_info))
		wavl_delete(vc_info->swidb->atm_glommable_vc_tree, &vc_info->wavl);
	reg_invoke_atm_vc_removed(vc_info);
	free(vc_info);
    }
}

/*
 * atm_params_init
 * Initialize ATM static map parameters
 */
void
atm_params_init (atm_params_type *params)
{
    params->broadcast = FALSE;
    params->aal5mux = FALSE;
    params->fwpkrate0 = ATM_FWPKRATE0;
    params->bkpkrate0 = ATM_BKPKRATE0;
    params->fwpkrate1 = ATM_FWPKRATE1;
    params->bkpkrate1 = ATM_BKPKRATE1;
    params->fwstrate0 = ATM_FWSTRATE0;
    params->bkstrate0 = ATM_BKSTRATE0;
    params->fwstrate1 = ATM_FWSTRATE1;
    params->bkstrate1 = ATM_BKSTRATE1;
    params->fwbsize0 = ATM_FWBSIZE0;
    params->bkbsize0 = ATM_BKBSIZE0;
    params->fwbsize1 = ATM_FWBSIZE1;
    params->bkbsize1 = ATM_BKBSIZE1;
    params->trafmgmt = ATM_TRAFMGMT;
    params->layer2id = ATM_BLLIE;
}

/*
 * is_atm_map_bcast
 * Returns TRUE if static map is defined as pseudo-broadcast
 */
static boolean
is_atm_map_bcast (staticmap_type *map_ptr)
{
    atm_params_type *class_params;
    atm_params_type *map_params = map_ptr->params;

    if ((map_ptr->class) && (map_ptr->class->params)) {
	class_params = (atm_params_type *)map_ptr->class->params;
	return(class_params->broadcast || map_params->broadcast);
    } else
        return(map_params ? map_params->broadcast : FALSE);
}

/*
 * atm_staticmap_add
 * Add an entry into the static mapping table
 */
static void
atm_staticmap_add (maplist_type *list_ptr, idbtype *idb, hwaddrtype *hwconf, 
		   addrtype *addr, uint protocol, char *class, 
		   atm_params_type *params)
{
    if (!staticmap_enter(list_ptr, addr, hwconf, protocol, ET_ATM, 
				MAP_STATIC, class, params))
        printf("\n%% Cannot add map to list");
}

/*
 * atm_smds_check:	Called to verify the DA of the SMDS packet.
 *	Sets pak->if_input == to the appropriate interface for this DA.
 *
 * OUTPUT/RETURN:
 *	TRUE/FALSE if it's valid.
 * INPUT:
 *	Ptr. to PAK type for interrogation.
 */
static boolean
atm_smds_check (paktype *pak)
{
    smdstype *smds;
    idbtype *idb;
    hwidbtype *hwidb;

    smds = (smdstype *)pak->info_start;
    if (pak->if_input) {
	hwidb = pak->if_input->hwptr;
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (SMDS_ISINDIV(smds->smdsdest[0])) {
		if (idb->vaddr &&
		    atm_smdsequal(smds->smdsdest, idb->vaddr->addr)){
		    pak->if_input = idb;
		    return(TRUE);
		}
	    } else {
		atm_multicast_t *multic;

		multic = atm_get_multicast(idb);
		if (multic &&
		    atm_smdsequal(smds->smdsdest, multic->mca.addr)) {
		    pak->if_input = idb;
		    return(TRUE);
		}
	    }
	}
    }
    return(FALSE);
}

/*
 * atm_getlink:	Get the link type for the ATM packet
 * OUTPUT/RETURN:
 *	Returns an int. value of type LINK to the raw_enqueue routine.
 * INPUT:
 *	pak->flags == flags for this packet, i.e. BRIDGING info.
 *	pak->enctype == Encapsulation type being used.
 *	pak->snaptype == ETYPE of protocol encapsulated
 */
static long
atm_getlink (paktype *pak)
{
    snap_hdr   	*snap;
    ulong 	link = LINK_ILLEGAL;
    ushort	*etype = (ushort *)(pak->datagramstart + sizeof(ushort));
    vc_info_t	*vc_info;
    smdstype	*smds;

    if (global_irb_enable && is_bvi(pak->if_input->hwptr)) {
	vc_info = pak->irb_rcvd_input->hwptr->atm_db->fast_vc_info;
    } else {
	vc_info = pak->if_input->hwptr->atm_db->fast_vc_info;
    }

    /*
     * Locally generated packets may not have info_start set.
     * Dont crash and burn.
     */
    if (pak->info_start == NULL) {
	pak->info_start = pak->datagramstart + ATM_ENCAP_HDRBYTES;
    }

    snap = (snap_hdr *)pak->info_start;	/* Default to AAL5-LLC/SNAP */
    switch(vc_info->etype) {
    case ATM_ENCTYPE_SMDS:
	smds = (smdstype *)pak->info_start;
	snap = (snap_hdr *)smds->data;
	if (!atm_smds_check(pak)) {
	    if (atm_errors_debug) {
		buginf("\n(%s) input: Unknown SMDS address %E", 
		pak->if_input->namestring, smds->smdsdest);
	    }
	    return(LINK_ILLEGAL);
	}

	if (ieee_equal_oui(snap->oui, atm_tbridge_oui)) {
	    switch(GETSHORT(&snap->type)) {
	    case TBRIDGE_PID_BPDU:
		/*
		 * force IEEE bpdu, span_process will sort that out later
		 */
		link = LINK_IEEE_SPANNING;
		break;
	    case TBRIDGE_PID_8023:
	    case TBRIDGE_PID_8023FCS:
		{
		    ether_hdr *ether;
		    ether = (ether_hdr *)pak->mac_start;
		    link = serial2link(GETSHORT(&ether->type_or_len) & 0xFFFF);
		}
		break;
	    }
	} else if (pak->enctype == ET_SAP)  {
	    link = LINK_CLNS;
	} else if (GETSHORT(etype) < 0x0100) { /* Look for special encodings */
	    link = serial2link(GETSHORT(&snap->type) & 0xFFFF);
	} else {
	    link = LINK_ILLEGAL;
	}
	break;

    case ATM_ENCTYPE_SNAP:
	if (ieee_equal_oui(snap->oui, atm_tbridge_oui)) {
	    switch(GETSHORT(&snap->type)) {

	    /*
	     * keep this case TBRIDGE_PID_BPDU for backwards
	     * compatibility.  all 10.3 based code which is slow
	     * switched sends bpdu's this way.
	     */
	    case TBRIDGE_PID_BPDU:
		/*
		 * force IEEE bpdu, span_process will sort that out later
		 */
		link = LINK_IEEE_SPANNING;
		break;

	    /*
	     * all bpdu's were changed to ethernet type
	     */
	    case TBRIDGE_PID_8023:
	    case TBRIDGE_PID_8023FCS: {
    		ether_hdr *ether;

		/*
		 * strip off the 1483 hdr
		 */
		pak->datagramstart += ATM_BRIDGE_ETHER_ENCAPBYTES;
		pak->datagramsize -= ATM_BRIDGE_ETHER_ENCAPBYTES;
		pak->mac_start = pak->datagramstart;
		pak->addr_start = pak->datagramstart;

	    	ether = (ether_hdr *)pak->mac_start;

		/*
		 * figure out what kind of packet it is depending
		 * on packet type.  you see, these packets are going
		 * to process level.  so set pak->network_start
		 * and forget about it.
		 */
		switch (pak->enctype) {
		case ET_ARPA:
		    link = serial2link(GETSHORT(&ether->type_or_len) & 0xFFFF);
		    /*
		     * make it look like a damn ethernet arpa frame
		     */
		    pak->network_start = pak->datagramstart +
			ETHER_ARPA_ENCAPBYTES;
		    break;

		case ET_SNAP: {
		    /*
		     * make it look like a damn ethernet snap frame
		     */
		    snap_hdr *snap = (snap_hdr *)ether->data;
		    link = serial2link(GETSHORT(&snap->type));
		    pak->info_start = pak->datagramstart + 
			ETHER_8023_ENCAPBYTES;
		    pak->network_start = pak->datagramstart +
			ETHER_SNAP_ENCAPBYTES;
		}
		    break;

		case ET_SAP: {
		    sap_hdr *sap = (sap_hdr *)ether->data;
		    pak->llc2_cb = NULL;	/* indicate UI for LLC2 */
		    link = sap2link(sap->dsap);
		    /*
		     * make it look like a damn ethernet sap frame
		     */
		    pak->info_start = pak->datagramstart + 
			ETHER_8023_ENCAPBYTES;
		    pak->network_start = pak->datagramstart +
			ETHER_SAP_ENCAPBYTES;
		}
		    break;

		default:
		    /*
		     * totally hopeless.  throw the packet away.
		     */
		    link = LINK_ILLEGAL;
		    break;
		}

		/*
		 * set PAK_TBRIDGE_IN in case it needs to get forwarded
		 * onwards to other bridges...
		 */
		pak->flags |= PAK_TBRIDGE_IN;
	    }
		break;

	    default:
		break;
	    }
        } else if (pak->enctype == ET_SAP)  {
            link = LINK_CLNS;
        } else if (GETSHORT(etype) < 0x0100) { /* Look for special encodings */
            link = serial2link(GETSHORT(&snap->type) & 0xFFFF);
        } else {
            link = LINK_ILLEGAL;
        }
        break;
    case ATM_ENCTYPE_NLPID:
	if (pak->enctype == ET_LLC2) {
	    link = LINK_LLC2;
	    break;
	}
        switch(GETSHORT((ushort *)pak->info_start)) {
        case ATM_NLPID_SNAP1PAD:
            {
                snap_hdr *snap = (snap_hdr *)pak->info_start;
                link = serial2link(GETSHORT(&snap->type));
            }
            break;
        case ATM_NLPID_SNAP0PAD:
            {
                snap_hdr *snap = (snap_hdr *)((uchar *)pak->info_start - 1);
                link = serial2link(GETSHORT(&snap->type));
            }
            break;
        default:
            link = serial2link(GETSHORT((ushort *)pak->info_start));
            break;
        }
        break;
    case ATM_ENCTYPE_LANE:
    case ATM_ENCTYPE_LANE_BUS:
	/*
	 * atm_parse_packet left datagramstart pointing at the VCD for
	 * the copy into memory.  Now we can put it somewhere useful and
	 * treat it like an ethernet frame.
         */

         if (vc_info->swidb->lane_info->type == LANE_TYPE_802_3) {
	     link = ether_getlink(pak);
         } else {
	     link = reg_invoke_tr_lane_tring_getlink(pak);
	     break;
         }

	if ((link != LINK_DEC_SPANNING) && (link !=
					    LINK_IEEE_SPANNING)) {
	    if (vc_info->swidb->tbridge_on_swidb) { 
		pak->flags |= PAK_TBRIDGE_IN;
	    }
	}
	break;
    case ATM_ENCTYPE_FRNLPID:
    case ATM_ENCTYPE_SAAL:
    case ATM_ENCTYPE_RAW:
    /* case ATM_ENCTYPE_ILMI:     Same as ATM_ENCTYPE_RAW */
        link = LINK_ILLEGAL;
        break;
    default:
        /*
         * Must be a MUX type packet
         */
        link = serial2link(GETSHORT(etype) & 0xFFFF);
    }
    return(link);
}
/*
 * atm_extract_addr:
 *
 * Given an ATM packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean
atm_extract_addr (paktype *pak,
		  hwaddrtype *address,
		  enum SRCDST which)
{
    vcd_t vcnum;
    vc_info_t *vc = NULL;
    uchar *srcaddr, *dstaddr;

    vcnum = pak->atm_vcid;
    vc = atm_getvc(pak->if_input->hwptr->atm_db, vcnum);
    if (!vc) {
	return(FALSE);
    }

    if (is_atm_lane(vc->swidb)) {
	/*
	 * Lane follows ethernet model. 
	 */
	ether_extract_addr(pak, address,which);
	return(TRUE);
    }
    dstaddr = srcaddr = pak->datagramstart;
    if (ISVCAAL34(vc)) {
	smdstype *smds;

	smds = (smdstype *)(pak->mac_start + ATM_ENCAP_HDRBYTES);
    	address->type = STATION_SMDS64;
    	address->length = STATIONLEN_SMDS64;
	srcaddr = smds->smdssrc;
	dstaddr = smds->smdsdest;
    } else {
    	address->type = STATION_ATMVC;
    	address->length = STATIONLEN_ATMVC;
    }

    address->target_type = TARGET_UNICAST;
    switch (which) {
      case SOURCE:
	bcopy(srcaddr, address->addr, address->length);
	break;
      case DESTINATION:
	bcopy(dstaddr, address->addr, address->length);
	break;
    }

    return(TRUE);
}

/*
 * atm_parse_vc_stats:
 *	Called to parse a block of memory that contains per-VC information
 *	regarding total packets/bytes transmitted & received.
 * OUTPUT/RETURN:
 *	Updates per-VC counters.
 * INPUT:
 *	*hwidb -> Ptr. to HWIDB in question
 *	tx_rx_flag -> Which side to update: TRUE => Tx, Rx otherwise
 *	*start -> Ptr. to memory containing per-vc counts.
 *	length -> Length of memory to parse.
 * NOTE:
 *	Split from atm_vc_stats so that it could be used to parse the per-VC
 *	information which comes in as love letters on the RSP.
 */
#define	VCD_MASK	0x3FFF		/* Mask out all other bits */

void
atm_parse_vc_stats (hwidbtype *hwidb, boolean tx_rx_flag, ulong *start,
		    ulong length)
{
    atm_db_t *atm = hwidb->atm_db;
    vc_info_t *vc;
    struct vccnt {
	ulong vcd;
	ulong pkt_cnt;
	ulong byte_cnt;
    } *vc_cnt;
    ulong limit = (ulong)start + length;

    vc_cnt = (struct vccnt *)start;
    atm_counters[ATM_INPUT]++;
    atm->inpkts++;
    for(; vc_cnt->vcd && (ulong)vc_cnt < limit; vc_cnt++) {
	vc = atm_getvc(atm, (vc_cnt->vcd & VCD_MASK));
	if (vc) {
	    if (tx_rx_flag) {		
	    /* 
	     * Handle the TX side info.  
	     */
	    	vc->out_pkts = vc_cnt->pkt_cnt;
		vc->out_bytes += vc_cnt->byte_cnt;
		if (!(ISVCAAL34(vc))) {
		    atm->aal5.ifOutOctets += vc_cnt->byte_cnt;
		}
		ATM_OUTAS(atm, vc);
		if (atm_event_debug) {
		    buginf("\nVC_STAT(%s): VCD# %lu: TX Packets: %lu, Bytes: %lu",
			hwidb->hw_namestring, vc_cnt->vcd, vc_cnt->pkt_cnt,
			vc_cnt->byte_cnt);
		}
	    } else {
	    /*
	     * Handle the RX side info.
	     */
		vc->in_pkts = vc_cnt->pkt_cnt;
		vc->in_bytes += vc_cnt->byte_cnt;
		if (!(ISVCAAL34(vc))) {
		    atm->aal5.ifInOctets += vc_cnt->byte_cnt;
		}
		ATM_INAS(atm, vc);
		if (atm_event_debug) {
		    buginf("\nVC_STAT(%s): VCD# %lu: RX Packets: %lu, Bytes: %lu",
			hwidb->hw_namestring, vc_cnt->vcd, vc_cnt->pkt_cnt,
			vc_cnt->byte_cnt);
		}
	    }
	}
    }
}

/*
 * atm_vc_stats:
 *	Called to process one of the special packets from the IP.
 *	The packet contains per-VC information regarding total 
 *	packets/bytes transmitted & received.
 * OUTPUT/RETURN:
 *	Updates per-VC counters.
 * INPUT:
 *	*hwidb -> Ptr. to HWIDB in question
 *	*pak -> Ptr. to packet containing per-vc counts.
 */
static void
atm_vc_stats (hwidbtype *hwidb, paktype *pak)
{
    atm_parse_vc_stats(hwidb, (((short) GETSHORT(pak->datagramstart)) >= 0),
	(ulong *)pak->network_start, pak->datagramsize -
	((ulong)pak->network_start - (ulong)pak->datagramstart));
    retbuffer(pak);
}

/*
 * atm_parse_packet: 
 *
 *      Set up a packet buffer so that the network level protocol fields
 *      are initialized.
 *
 *	The packet arrives with only the VC as the 'OVERHEAD'(encapsulation).
 *	This routine checks the VC config. to determine the type of encaps.
 *	configured for it to compute the actual ENCAP size.
 *	On exit NULL is returned if the VC is not config'ed, else a pointer
 *	to the input queue processing routine to call.
 * OUTPUT/RETURN:
 *	pak->datagramstart == beginning of the PAK buffer (if data != NULL)
 *	space for the mci2buffer() move operation to align the PDU
 *	at the 'LINE'.
 * INPUT:
 *
 *      data == ptr to Sniff Area
 *             If data is NULL, pak->datagramstart is assumed to point to the
 *             start of the frame to parse. No attempt will be made to align
 *             the frame to The Line.
 *	idb == Ptr. to the hardware IDB for the ATM.  From there we find the
 *		mciregtype pointers.  We position the 'read' window to
 *		the word prior to the LINE and read that word.  If 
 *		mci_rxoffset is proper, there should be a VC at that
 *		location which is then used to determine the encapsulation
 *		type.
 */

static iqueue_t 
atm_parse_packet (hwidbtype *hwidb,
		  paktype *pak,
		  uchar *data)
{
    atm_db_t	*atm = hwidb->atm_db;
    vc_info_t	*vc_info;
    ushort	vcnum, atmtype;
    iqueue_t	iq;
    charlong	firstlong;

    /*
     * Check to see if building a frame in a virgin packet, or whether
     * we're just setting up media pointers into the frame
     */
    if (data) {
	/*
	 * We've got a virgin packet. 
	 */
	firstlong.d.lword = GETLONG(data);
    } else {
	/*
	 * Soiled packet. Start from datagramstart
	 */
	firstlong.d.lword = GETLONG(pak->datagramstart);
    }

    vcnum = firstlong.d.sword[0];
    atmtype = firstlong.d.sword[1];

    if (atmtype == ATM_ENCTYPE_RAW) {
	iq = atm->funcs.rawinput;

	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) - hwidb->encsize;
	}
	pak->network_start = pak->datagramstart + hwidb->encsize;
	pak->encsize = ATM_ENCAP_HDRBYTES;
    } else if (atmtype == ATM_ENCTYPE_VCSTAT) {
	iq = atm->funcs.rawinput;
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) - hwidb->encsize;
	}
	pak->network_start = pak->datagramstart + hwidb->encsize;
	pak->encsize = ATM_ENCAP_HDRBYTES;
	iq = atm_vc_stats;
    } else {
	if (!vcnum || vcnum >= atm->config.max_intf_vcs) {
	    if (atm_errors_debug)
		buginf("\natm_parse_packet(%s):"
		       "Invalid VC(%d) received, type=%x",
		       hwidb->hw_namestring,
		       vcnum, atmtype);
	    return(NULL);
	}
	if (!(vc_info = atm->fast_vc_info)) {
	    if (atm_errors_debug)
		buginf("\natm_parse_packet(%s):"
		       "VC(%d) NOT configured, type=%x",
		       hwidb->hw_namestring,
		       vcnum, atmtype);
	    return(NULL);
	}
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) - hwidb->encsize;
	}
	iq = hwidb->iqueue;
	switch (atmtype) {
	case ATM_ENCTYPE_NLPID:
	    {
	    ushort 	snaplen = 0;
	    ushort	nlpidtype = 0;
	    ushort      l2pid;
            ushort      sniff_buffer[3];
            ushort      *sniff_ptr;

	    pak->encsize = vc_info->encapsize;
	    if (data)
		nlpidtype = reg_invoke_atm_get_nlpidtype(hwidb, data);
	    else
		nlpidtype = reg_invoke_atm_get_nlpidtype(hwidb,
							 pak->datagramstart);
	    switch(nlpidtype) {
	    case ATM_NLPID_Q2931:

#define LLC_ENCAP_SIZE 6                     /* ctl, nlpid, l2pid, l3pid */
#define PIDS_ENCAP_SIZE (LLC_ENCAP_SIZE - 2) /* l2pid, l3pid */
#define MAX_LLC2_HDRSZ 4                     
#define USER_DEFINED_L3PID_FOLLOWS 0x70

		/* 
		 * Sniff 1 short words pick up type and possibly SNAP 
		 * before passing to ether_parse_packet.
		 */
		sniff_ptr = (*atm->funcs.sniff)(hwidb, 
                                                pak,
                                                data,
				                sniff_buffer,
				                3,

				                1);
		l2pid = *sniff_ptr; 
		if (l2pid == TYPE_L2_8022_PAD) {
		    pak->enctype = ET_LLC2;
		    pak->mac_start = pak->datagramstart;
		    pak->addr_start = NULL;
		    pak->rif_start = NULL;
		    pak->riflen = 0;
		    pak->info_start = pak->datagramstart +
			vc_info->encapsize + PIDS_ENCAP_SIZE;
		    pak->network_start = pak->info_start + MAX_LLC2_HDRSZ;
		    pak->encsize = pak->network_start -
			pak->datagramstart;
		    return(iq);
		}
		break;
	    case ATM_NLPID_SNAP0PAD:
		snaplen = ATM_NLPID_SNAP_HDRBYTES - 1;
            case ATM_NLPID_SNAP1PAD:
                snaplen = ATM_NLPID_SNAP_HDRBYTES;
                break;
            }
            pak->network_start = (uchar *)pak->datagramstart +
                vc_info->encapsize + snaplen;
	    pak->encsize += snaplen;
            }
            break;

        case ATM_ENCTYPE_LANE:
	case ATM_ENCTYPE_LANE_BUS:
	    /*
	     * Some of the LANE control processes need to know which VC
	     * the packet came in on to determine what's on the other
	     * end.  So we save it away in the pak.
	     */
	    pak->vcd = vcnum;
            switch (vc_info->user_vc_type) {
	    case LANE_LEC_DATA:
            case LANE_LEC_TR_DATA:
	    case LANE_LEC_SEND:
	    case LANE_LEC_TR_SEND:
	    case LANE_LEC_FORWARD:
            case LANE_LEC_TR_FORWARD:
                /*
                 * Pkts on LANE data vc's are either LANE control or data
                 * with ethernet encaps following ATM/LANE encaps.  We
                 * depend on rxtype to tell us if it's LANE control.
                 *
                 * If not LANE control, use ether_parse_packet to decode.
                 */
                if (pak->rxtype == RXTYPE_LANE_CONTROL) {
                    pak->encsize = ATM_ENCAP_HDRBYTES;
                } else {
                    if (data) {
                        /*
                         * "Virgin" packet - data not yet in processor memory,
                         * must sniff to get ether_parse or tring_parse data.
                         */
                        ushort       sniff_data[btow(SNAP_HDRBYTES + 2)];
                        ushort      *sniff_ptr;

                        sniff_ptr = (*atm->funcs.sniff)(hwidb,
                                                        pak,
                                                        data,
                                                        sniff_data,
                                              btow(ATM_AAL5_LANE_ENCAPBYTES + 
                                                   2 * IEEEBYTES),
                                              btow(SNAP_HDRBYTES + 2));
                        if (vc_info->swidb->lane_info->type == LANE_TYPE_802_3) {
			    ether_parse_packet(hwidb, pak, (uchar *)sniff_ptr);
                        } else {
			    reg_invoke_tr_lane_tring_parse_packet(hwidb, 
								  pak, 
								  (uchar *)sniff_ptr);
                            if (data) {
                               return(iq);
                            }
			}
                    } else {
                        /*
                         * "Soiled" packet - data at pak->datagramstart is
                         * valid.  Advance past ATM LANE encap so ether_parse
                         * sees embedded ethernet packet.
                         */
                        pak_delta_adjust_start(pak, ATM_AAL5_LANE_ENCAPBYTES);
                        ether_parse_packet(hwidb, pak, NULL);
                    }
                    /*
                     * After ether_parse, adjust start, size, encsize to
                     * account for ATM/LANE encap.  Then return rather
                     * than fall through so we leave enctype, mac_start,
                     * & info_start as ether_parse left them.
                     */
                    pak_delta_adjust_start(pak, -ATM_AAL5_LANE_ENCAPBYTES);
                    pak->encsize = pak->network_start - pak->datagramstart;
                    return(iq);
                }
                break;

	    case LANE_LEC_DIRECT:
	    case LANE_LEC_DISTRIBUTE:
	    case LANE_LEC_CONFIGURE:
	    case LANE_LES_DIRECT:
	    case LANE_LES_DISTRIBUTE:
	    case LANE_BUS_SEND:
	    case LANE_BUS_FORWARD:
	    case LANE_CONFIGURE:
		break;
	    }
	    break;
	case ATM_ENCTYPE_SMDS:
	case ATM_ENCTYPE_SNAP:
	default:		/* AAL5 MUX types come here */
	    pak->network_start = pak->datagramstart + vc_info->encapsize;;
	    pak->encsize = vc_info->encapsize;
	    break;
    	}

    }

    pak->enctype       = ET_ATM;
    pak->mac_start     = pak->datagramstart;
    pak->addr_start    = NULL;
    pak->rif_start     = NULL;
    pak->riflen        = 0;
    pak->info_start    = pak->datagramstart + ATM_ENCAP_HDRBYTES;

    /*
     * Record just how big this encapsulation was rather than every protocol
     * have it's own fun and wacky way to work it out.
     */
    pak->encsize = pak->network_start - pak->datagramstart;
    return(iq);
}

/*
 * Section: Encapsulation subroutines
 * 	This section contains the individual encapsulation routines
 *	that we must support for ATM.  Each is designed to work similar
 *	to the 'vencaps' routine in that they simply encapsulate the
 *	datagram starting with pak->datagramstart.
 */

/*
 * atm_cpcs_snap: Called to do SNAP encapsulation.
 */
static void
atm_cpcs_snap (paktype *pak, uchar const *snap_info)
{
    snap_hdr *snap;
    sap_hdr *sap;

    switch(pak->linktype) {
    case LINK_CLNS:
    case LINK_CLNS_BCAST:
    case LINK_CLNS_ALL_ES:
    case LINK_CLNS_ALL_IS:
    case LINK_ISIS_ALL_L1_IS:
    case LINK_ISIS_ALL_L2_IS:
	pak->enctype = ET_SAP;
	pak->datagramstart -= ATM_SAP_HDRBYTES;
	pak->datagramsize += ATM_SAP_HDRBYTES;
    	sap = (sap_hdr *)pak->datagramstart;
	sap->dsap = sap->ssap = SAP_CLNS;
	sap->control = LLC1_UI;
    	pak->encsize += ATM_SAP_HDRBYTES;
   	break;
    case LINK_DEC_SPANNING:
    case LINK_IEEE_SPANNING:
        pak->datagramstart -= (ATM_SNAP_HDRBYTES);
        pak->datagramsize += (ATM_SNAP_HDRBYTES);
        snap = (snap_hdr *)pak->datagramstart;
        bzero(pak->datagramstart, SNAP_HDRBYTES);
        snap->dsap = snap->ssap = SAP_SNAP;
        snap->control = LLC1_UI;
        ieee_copy_oui(snap_info, snap->oui);
        PUTSHORT(&snap->type, TBRIDGE_PID_BPDU);
        pak->encsize += ATM_SNAP_HDRBYTES;
        break;
    case LINK_BRIDGE:
	pak->enctype = ET_BRIDGE;
    default:
	if ((pak->enctype == ET_BRIDGE) || (pak->enctype == ET_BRIDGE_ENCAPS)) {
    	    pak->datagramstart -= (ATM_SNAP_HDRBYTES + ATM_BRIDGE_PAD_SIZE);
	    pak->datagramsize += (ATM_SNAP_HDRBYTES + ATM_BRIDGE_PAD_SIZE);
	    snap = (snap_hdr *)pak->datagramstart;
	    memset(pak->datagramstart, 0, SNAP_HDRBYTES+ATM_BRIDGE_PAD_SIZE);
	    snap->dsap = snap->ssap = SAP_SNAP;
	    snap->control = LLC1_UI;
	    ieee_copy_oui(snap_info, snap->oui);
	    PUTSHORT(&snap->type, TBRIDGE_PID_8023);
    	    pak->encsize += ATM_SNAP_HDRBYTES + ATM_BRIDGE_PAD_SIZE;
	    break;
	}
	pak->enctype = ET_SNAP;
    	pak->datagramstart -= (ATM_SNAP_HDRBYTES);
	pak->datagramsize += (ATM_SNAP_HDRBYTES);
	snap = (snap_hdr *)pak->datagramstart;
	memset(pak->datagramstart, 0, SNAP_HDRBYTES);
	snap->dsap = snap->ssap = SAP_SNAP;
	snap->control = LLC1_UI;
	ieee_copy_oui(snap_info, snap->oui);
	PUTSHORT(&snap->type, link2serial(pak->linktype));
    	pak->encsize += ATM_SNAP_HDRBYTES;
	break;
    }
    pak->mac_start  = pak->datagramstart;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen     = 0;
    pak->info_start = pak->datagramstart;
}

/*
 * atm_cpcs_nlpid: Called to do NLPID encapsulation.
 *      Follows RFC1483/1294 for encapsulations.  ISO & IP have a
 *      straight encapsulation scheme.  Simple 2 byte NLPID is all that
 *      is necessary.  
 *
 *      LLC2 encapsulation uses NLPID value 0x09 to identify Q.2931,
 *      followed by L2 and L3 protocol identifiers.
 *
 *      All other protocols require a SNAP frame after
 *      the NLPID with a real protocol PID as defined with SNAP.
 * OUTPUT/RETURN:
 *      The PACKET is adjusted properly and all encapsulations are slapped
 *      onto the beginning.
 *          VC+DM+NLPID+SNAP then PDU.  SNAP is optional depending on
 *      the protocol being transmitted.
 * INPUT:
 *      PAK is all that is needed.
 * ASSUMPTIONS:
 */

static void
atm_cpcs_nlpid (paktype *pak, uchar const *snap_info, uchar l3pid)
{
    ushort *nlpid;
    atm_nlpid_snap_t *snap;

    pak->encsize = ATM_NLPID_HDRBYTES;
    if (pak->linktype == LINK_LLC2) {
        pak->datagramstart -= LLC_ENCAP_SIZE;
        pak->datagramsize += LLC_ENCAP_SIZE;
        pak->datagramstart[0] = ATM_NLPID_CTL;
        pak->datagramstart[1] = ATM_NLPID_PID_Q2931;
        *(ushort *)(pak->datagramstart+2) = TYPE_L2_8022_PAD;
        pak->datagramstart[4] = USER_DEFINED_L3PID_FOLLOWS;
        pak->datagramstart[5] = l3pid;
        pak->encsize = LLC_ENCAP_SIZE;
    } else if(ATM_NLPID_DEF(pak->linktype)) {
        pak->datagramstart -= (ATM_NLPID_HDRBYTES);
        pak->datagramsize += (ATM_NLPID_HDRBYTES);
	nlpid = (ushort *)pak->datagramstart;
        *nlpid = atm_link2nlpid(pak->linktype);
	if (*nlpid == ATM_NLPID_CTL) {
	/*
	 * For NLPID_DEF, the PID was already put in by CLNS.  
	 * We didn't add it, so we must not count the PID byte in the
	 * Start/Size values of this datagram.
	 */
	    pak->datagramstart += 1;
	    pak->datagramsize -= 1;
	    pak->encsize -= 1;
	}
    } else {
        pak->datagramstart -= ((ATM_NLPID_HDRBYTES) +
                ATM_NLPID_SNAP_ENCAPBYTES);
        pak->datagramsize += (ATM_NLPID_HDRBYTES +
                ATM_NLPID_SNAP_ENCAPBYTES);
	pak->encsize += ATM_NLPID_SNAP_HDRBYTES;

	nlpid = (ushort *)pak->datagramstart;
        PUTSHORT (nlpid, ATM_NLPID_SNAP1PAD);  /* ALWAYS use PAD1....! */

        pak->enctype = ET_SNAP;
        snap = (atm_nlpid_snap_t *)((uchar *)pak->datagramstart + 
		ATM_NLPID_HDRBYTES);
        memset(snap, 0, ATM_NLPID_SNAP_ENCAPBYTES);
        snap->control = 0x80;
        ieee_copy_oui(snap_info, snap->oui);
        PUTSHORT(&snap->type, link2serial(pak->linktype));
    }
}

/*
 * atm_cpcs_smds: Called to do SMDS(AAL3/4) encapsulation.
 */
static void
atm_cpcs_smds (paktype *pak, 
	       uchar const *snap_info, 
	       vc_info_t *vc,
	       hwaddrtype *daddr)
{
    int pad = 0;
    smdstype *header;
    idbtype *idb = pak->if_output;
    hwidbtype *hwidb;
    unsigned len;

    if (idb) {
	hwidb = idb->hwptr;

	/*
	 * First slam on a SNAP as required for RFC1209
	 */
	atm_cpcs_snap(pak, snap_info);

	/*
	 * Now do the L3PDU dirty work.  header first.
	 */
	len = pak->datagramsize;
	pak->datagramstart -= ATM_SMDS_L3_HDRBYTES;
	header = (smdstype *)pak->datagramstart;
	memset(header, 0, ATM_SMDS_L3_HDRBYTES);

	len += ATM_SMDS_L3_HDRBYTES;
	header->smds_betag = hwidb->smds_begin_tag++;
	bcopy(daddr->addr, header->smdsdest, STATIONLEN_SMDS64);
	bcopy(idb->vaddr->addr, header->smdssrc, STATIONLEN_SMDS64);
	header->smds_xhel = XHEL_SIZE;
	header->smds_betag = (vc->outpkts % 256);

	pak->enctype = ET_SMDS;
	/*
	 * Calculate the pad size. remember to subtract the psdeo hdlc
	 * bytes off first (sizeof 2).
	 * RLFS: Although the aip performs this task for us, I leave this
	 * code here, commented, to remind future IP implementors that this
	 * is a required field in SMDS.
	   pad = 3 - ((len + 3) % 4);
	   header->smds_basize = len - (sizeof(smdstype)) + SMDS_HEADER + pad;
	   ptr = pak->datagramstart + len;
	 */
	/*
	 *the lower two bits of the higher level protocol
	 * identifier field is used as a count of pad bytes
	 *
	 * all higher level protocols use llc/snap headers
	 */
	header->smds_xhlpi = pad | SMDS_LLC;
	header->smds_hel[0] = 3;
	header->smds_hel[1] = 0;
	header->smds_hel[2] = 1;

	pak->datagramsize = len;
	/*
	 * Now for the L3PDU trailer.
	 * Code remains commented in case other ATM IPs require trailer
	 * I leave it here to remind future SMDS engineers to check this.
	 * RLFS: removed... ATM interface puts PAD & trailer in for me.
	 *
	    for (i = 0; i < pad; i++)
	        *ptr++ = 0;
	    bcopy(&header->smds_rsvd, ptr, SMDS_TRAILER);
	    pak->datagramsize += pad + SMDS_TRAILER;
	 */
	pak->encsize += ATM_SMDS_L3_HDRBYTES;
    }
}


/*
 * atm_cpcs_encap: Used to do final encapsulation of the datagram.
 * OUTPUT/RETURN:
 *	pak->datagramstart == ptr. to encapsulated data ready for transmission.
 *	pak->datagramsize == size of encapsulated datagram.
 * INPUT:
 *	pak == ptr. to the packet struct.
 *	atm == ATM interface in question.
 *	vc == ptr. to the VC for this encapsulation.
 *      l3pid == value for 1483 llc traffic
 */
void
atm_cpcs_encap (paktype *pak, 
		atm_db_t *atm, 
		vc_info_t *vc_info,
		uchar const *snap_info,
		hwaddrtype *hwaddr,
		uchar l3pid)
{
    switch(vc_info->flags & VC_FLAG_AAL_MASK) {
    case ATM_VC_AAL5SNAP:
	pak->encsize = 0;
	atm_cpcs_snap(pak, snap_info);
	break;
    case ATM_VC_AAL5NLPID:
	pak->encsize = 0;
	atm_cpcs_nlpid(pak, snap_info, l3pid);
	break;
    case ATM_VC_AAL5FRNLPID:
	pak->datagramstart -= ATM_FRNLPID_ENCAPBYTES;
	pak->datagramsize += ATM_FRNLPID_ENCAPBYTES;
    	pak->encsize = (vc_info->encapsize - ATM_ENCAP_HDRBYTES);
	break;
    case ATM_VC_AAL5ILMI:	/* Encaps ILMI frame.... */
    case ATM_VC_AAL5MUX:
    case ATM_VC_AALQSAAL:
    	pak->encsize = (vc_info->encapsize - ATM_ENCAP_HDRBYTES);
	break;
    case ATM_VC_AAL34SMDS:
	pak->encsize = 0;
	atm_cpcs_smds(pak, snap_info, vc_info, hwaddr);
	break;
    case ATM_VC_AAL5LANE:
	/*
	 * Note that this fragment of code could be moved out into
	 * lane.c.  lane_data_vencap calls atm_cpcs_encap, which gets
	 * here, and then lec_cpcs_lane calls back to the lane code.
	 * Left it here for clarity.
	 */
	switch(vc_info->user_vc_type) {
	case LANE_LEC_SEND:
	case LANE_LEC_TR_SEND:
	case LANE_LEC_DATA:
	case LANE_LEC_TR_DATA:
	    reg_invoke_lec_cpcs_lane(pak);
	    break;
	    /*
	     * The following encaps should be handled 
	     * by the external interface lane_control_encap defined in
	     * lane.c.
	     * This path should NOT be used.
	     */
	case LANE_LEC_DIRECT:
	case LANE_LEC_DISTRIBUTE:
	case LANE_LEC_CONFIGURE:
	case LANE_LES_DIRECT:
	case LANE_LES_DISTRIBUTE:
	case LANE_BUS_SEND:
	case LANE_BUS_TR_SEND:
	case LANE_BUS_FORWARD:
	case LANE_BUS_TR_FORWARD:
	case LANE_LEC_FORWARD:
	case LANE_LEC_TR_FORWARD:
	case LANE_CONFIGURE:
	    if (atm_errors_debug) {
		buginf("\nATM interface(%s): CPCS LANE encaps failure, VCD# %d", 
		       pak->if_output->namestring, 
		       vc_info->vc);
	    }
	    return;
	}
	break;
    default:
	if (atm_errors_debug) {
	    buginf("\nATM interface(%s): CPCS encaps failure, VCD# %d", 
		   pak->if_output->namestring, 
		   vc_info->vc);
	}
	return;
    }
    /*
     * The above routines should have already initialized encsize.
     * Now add in the size of the VCD/ENCTYPE.
     */
    pak->encsize += (*atm->funcs.encapsulate)(atm, pak, vc_info, 0);
}

/*
 * atm_multicast:	Called to Multicast an AAL3/4 packet out.
 */
static vc_info_t *
atm_multicast (idbtype *idb, 
	       paktype *pak, 
	       uchar const *snap_info, 
	       hwaddrtype **hwaddr)
{
    atm_multicast_t *multic;
    vcd_t vcnum;
    vc_info_t *vc;

    multic = atm_get_multicast(idb);
    if (!multic) {
	return(NULL);
    }
    /* If no SMDS address configured on the interface, return */
    if (idb->vaddr == NULL) {
	return(NULL);
    }
    vcnum = atm_get_aal34vcnum(idb);

    if (idb->hwptr) {
	paktype *newpak;
	hwidbtype *hwidb;
	atm_db_t *atm;

	newpak = NULL;
	hwidb = idb->hwptr;
	atm = (atm_db_t *)hwidb->atm_db;
	vc = atm_getvc(atm, vcnum);
	if (!vc) {
	    return(NULL);
	}
	if (hwaddr) {
	    *hwaddr = &multic->mca;
	    return(vc);
	} 
	newpak = pak_duplicate(pak);
	if (!newpak) {
	    return(NULL);
	}
	atm_cpcs_encap(newpak, atm, vc, snap_info, &multic->mca, 0);
	if (atm_packet_debug)
	    atm_show_packet(hwidb, newpak, TRUE);
	datagram_out(newpak);		/* Send it on it's way */

	vc->broadcasts++;
	atm_counters[ATM_BROADCAST]++;
	atm_counters[ATM_OUTPUT]++;	/* Count as output packets */
	atm->outpkts++;
    }
    return(NULL);
}

/*
 * atm_broadcast: Called to setup to broadcast a packet to the BROADCAST
 *	VCs.  All VCs, configured for BROADCAST, will get a copy of this
 *	datagram.
 *	In a PVC world, any PVC can be a BROADCAST VC.
 *	In the signalling world, we'll likely have only 1 BROADCAST VC
 *	per SubInterface/Protocol.
 * SVC's
 *      Handle SVC's separately from PVC's.
 *      Build svc map chain, fill in bc info in first_svcmap entry.
 *      Process SVC's after PVC's, need to return correct valid_vc
 *      for PVC or SVC. 0 = no map, -1 = SVC opening. 
 *
 * OUTPUT/RETURN:
 *	Returns a ptr. to the last VC for the datagram to be sent to.
 *	This func. will send a dup. to all but the last BROADCAST VC.
 * INPUT:
 */
static vc_info_t *
atm_broadcast (idbtype *idb, 
	       paktype *pak, 
	       staticmap_type *map,
	       uchar const *snap_info)
{
    vc_info_t *vc;
    paktype *dup;
    atm_db_t *atm;
    hwidbtype *hwidb;
    boolean all_tagged;
    staticmap_type *start_map;
    char addrstr[64];
    int conn_pending;

    vc_info_t *bc_vc;
    vc_info_t *valid_vc;
    int bc_vcd;
    staticmap_type *first_svcmap = NULL;
    staticmap_type *cur_map = NULL;
    atm_map_type *first_atm_map = NULL;
    atm_params_type *ptr_to_smap_params = NULL;
    atmSvcType *bc_call_id = NULL;
    atmSig_smap *bc_smap;

    atm = NULL;
    bc_vc = NULL;
    valid_vc = NULL;
    bc_vcd = 0;
    bc_smap = NULL;
    vc = NULL;
    all_tagged = TRUE;
    start_map = map;
    conn_pending = 0;

    if (idb) {
      hwidb = idb->hwptr;
      if (hwidb) {
	atm = hwidb->atm_db;

	if (!atm)
	  return(NULL);

	if (!map)
	  goto tackle_inarps;

	while (map) {
	  
	  vc = atm_addr_lookup(idb, atm, map);

	  if (!vc)
	    goto do_next;


	  /*
	   *  If we are capable of doing multipoint signalling, then we will
	   *  just build up a list of maps to attach to the multipoint VC.  If
	   *  we are not, then we must do pseudo-broadcast.
	   */
	  if (idb->atmsig_mtp_flag == TRUE) {

	    if ( (IS_ATMVC_SVC(vc)) || (vc->vc == atm->sig_vcnum) ) {

	      if (map->atm_map == NULL) 
		goto do_next;

	      /* check for first svc map entry */
	      if (!first_atm_map) {
		first_svcmap = cur_map = map;
		first_atm_map = first_svcmap->atm_map;
		if (!first_atm_map)
		  goto do_next;
		
		first_atm_map->next_svc = NULL;   
		first_atm_map->all_maps_active = TRUE;
		first_atm_map->bc_vc = NULL;   
		first_atm_map->bc_linkstate = LINK_NULL;   
		first_atm_map->bc_call_id = NULL;   

		if (first_atm_map->map_linkstate == LINK_ACTIVE) {
		  TIMER_STOP(first_atm_map->map_timestamp);
		  TIMER_STOP(first_atm_map->bc_timestamp);
		}
		else {
		  COPY_TIMESTAMP(first_atm_map->map_timestamp,
				 first_atm_map->bc_timestamp);
		}
	      }
	      /* already have first_svcmap */
	      else {
		cur_map->atm_map->next_svc = map;
		cur_map = map;
		cur_map->atm_map->next_svc = NULL;   

		if (!TIMER_RUNNING(first_atm_map->bc_timestamp)) {
		  COPY_TIMESTAMP(cur_map->atm_map->map_timestamp,
				 first_atm_map->bc_timestamp);
		}
	      }

	      /* check this entry for party active */
	      if (map->atm_map->map_partystate != PARTY_ACTIVE) {
		first_atm_map->all_maps_active = FALSE;
	      }
	      
	      /* if bc_linkstate not already active, check this entry
	       * if this entry active, stuff bc_call_id and bc_vc 
	       * into first_svcmap.
	       * only open parties later if call active 
	       */
	      if (first_atm_map->bc_linkstate != LINK_ACTIVE) {

		if (map->atm_map->map_linkstate == LINK_ACTIVE) {

		  ptr_to_smap_params = map->params;

                  if (ptr_to_smap_params)
		    bc_smap = (atmSig_smap *)ptr_to_smap_params->bc_smap;
                  else
                    bc_smap = NULL;
	  
		  if (bc_smap)
		    bc_call_id = (atmSvcType *)bc_smap->call_id;
                  else /* !!check this change 3/23!! */
                    bc_call_id = NULL;

		  if (bc_call_id) {
		    first_atm_map->bc_call_id = bc_call_id;
		    first_atm_map->bc_linkstate = LINK_ACTIVE;

		    bc_vcd = (map->atm_map->bc_hwaddr.addr[0] << 8) +
		             (map->atm_map->bc_hwaddr.addr[1]);
		    bc_vc = atm_getvc(atm, bc_vcd);
		    
		    if (!bc_vc) {
		      if (atm_errors_debug)
			buginf("\natm_broadcast, no vc in map");
		      goto tackle_inarps;
		    }

		    first_atm_map->bc_vc = bc_vc;
		    
		    /* if no valid vc yet, save it here */
		    if (!valid_vc) {
		      valid_vc = bc_vc;
		    }

		    /* this map active, clear the timestamp */
		    TIMER_STOP(map->atm_map->map_timestamp);
		  }
		}

		/* link not active, check if this entry is opening */
		if (map->atm_map->map_linkstate == LINK_OPENING) {
		  first_atm_map->bc_linkstate = LINK_OPENING;
		}
	      }
	    }
	    goto do_next;
	  } /* end mtp_flag = TRUE */

	  /* get here when MTP disabled or map is for PVC */

	  /** throttle here **/
	  /* if signalling vc, open connection and move on */
	  if (vc->vc == atm->sig_vcnum) {
	    if (map->conn_tag == FALSE) {
	      if (conn_pending < 6) {

		if (atm_event_debug) {
		  print_proto_addr(NULL, &map->addr, map->protocol, addrstr);
		  buginf("\nopening SVC %s", addrstr);
		}

		(void)atmSmap_openSvcConnection(idb, map, 0);
		map->conn_tag = TRUE;
		++conn_pending;
	      }
	      else {
		all_tagged = FALSE;
		if (atm_event_debug) {
		  print_proto_addr(NULL, &map->addr, map->protocol, addrstr);
		  buginf("\nTHROTTLE %s - SVC", addrstr);
		}
	      }
	    }
	    goto do_next;
	  }
	  else
	    map->conn_tag = TRUE; /* tag all active connections */     

	  /* this is a PVC or no MTP, dupe the packet */

	  /* check for valid_vc, if we don't have one yet, hang on to
	   * this one and continue processing maps.
	   */
	  if (!valid_vc) {
	    valid_vc = vc;
	    goto do_next;
	  }

	  /* already have valid_vc, dupe this one */
	  dup = pak_duplicate(pak);
	  if (dup) {
	    atm_cpcs_encap(dup, atm, vc, snap_info, &map->hwconf, 0);
	    if (atm_packet_debug)
	      atm_show_packet(hwidb, dup, TRUE);
	    datagram_out(dup); /* Send it on it's way */

	    vc->broadcasts++;
	    atm_counters[ATM_BROADCAST]++;
	    atm_counters[ATM_OUTPUT]++;	/* Count as output packets */
	    atm->outpkts++;
	  }
do_next:
	  map = map->next_bc;
	} /* end while (map) */


	/* fall through here to svc maps, separate chain built above, 
	 * bc info in first_svcmap 
	 * if no svc map, jump out
	 */
	if (!first_atm_map)
	  goto tackle_inarps;


	/* if MTP SVC opening, let it finish 
	 * if no valid_vc from PVC, 
	 *  return -1 to indicate open in progress
	 */
	if (first_atm_map->bc_linkstate == LINK_OPENING) { 

	  if (!valid_vc)
	    valid_vc = (vc_info_t *) -1;

	  goto tackle_inarps;

	}

	/* verify link active */
	if (first_atm_map->bc_linkstate == LINK_ACTIVE) {

	  /* if MTP link active and valid_vc == PVC
	   * dupe the PVC here and forget about it.
	   * set valid_vc = MTP bc_vc
	   */
	  if (valid_vc != first_atm_map->bc_vc) {

	    dup = pak_duplicate(pak);
	    if (dup) {
	      atm_cpcs_encap(dup, atm, valid_vc, snap_info,
			     &map->hwconf, 0);
	      if (atm_packet_debug)
		atm_show_packet(hwidb, dup, TRUE);
	      datagram_out(dup); /* Send it on it's way */

	      valid_vc->broadcasts++;
	      atm_counters[ATM_BROADCAST]++;
	      atm_counters[ATM_OUTPUT]++;	/* Count as output packets */
	      atm->outpkts++;
	    }

	    valid_vc = first_atm_map->bc_vc;
	  }

	  /* verify all parties active, get out*/
	  if (first_atm_map->all_maps_active) {
	    goto tackle_inarps;
	  }

	  /* not all parties active, if timestamp set, check if OK to 
	   * add parties now, if not, get out 
	   */
	  if (TIMER_RUNNING(first_atm_map->bc_timestamp)) {
	    if ( CLOCK_IN_INTERVAL(first_atm_map->bc_timestamp, 
					  atm->bc_interval) ) {
	      goto tackle_inarps;
	    }
	  }

	  /** throttle here **/
	  /* link up, parties to be added, find bc_call_id in first_svcmap */
	  bc_call_id = first_atm_map->bc_call_id;
	  if (!bc_call_id) {
	    if (atm_errors_debug)
	      buginf("\natm_broadcast: no bc_call_id");
	    goto tackle_inarps;
	  }

	  /* partystate either NULL or ACTIVE, won't attempt multiple opens
	   * before T399 timeout, 
	   * set ACTIVE in atmSmap_partyUpdate, NULL in atmSmap_partyDelete
	   * if active, clear the timestamp for this map
	   */
	  map = first_svcmap;
	  while (map) {               
	    if (map->atm_map->map_partystate == PARTY_ACTIVE) {
	      map->conn_tag = TRUE;  /* tag all active connections */
	      TIMER_STOP(map->atm_map->map_timestamp);

	    }

	    if ( (map->atm_map->map_partystate == PARTY_NULL) && 
		(map->conn_tag == FALSE) ) {
	      
	      if (conn_pending < 6) {

		if (atm_event_debug) {
		  print_proto_addr(NULL, &map->addr, map->protocol, addrstr);
		  buginf("\nopening Party %s", addrstr);
		}
		
		/* set timestamp when opening, clear it when active */
		GET_TIMESTAMP(map->atm_map->map_timestamp);
		if (atmSmap_openPartyConnection(idb, map, bc_call_id)) {
		    map->atm_map->map_partystate = PARTY_OPENING; 
		    map->conn_tag = TRUE;
		    ++conn_pending;
		} else {
		    map->atm_map->map_partystate = PARTY_NULL;
		}
	      }
	      else {
		all_tagged = FALSE;
		if (atm_event_debug) {
		  print_proto_addr(NULL, &map->addr, map->protocol, addrstr);
		  buginf("\nTHROTTLE %s - Party", addrstr);
		}
	      }
	    }
	    map = map->atm_map->next_svc;
	  }
	  
	  /* parties opening, return VC */
	  goto tackle_inarps;

	/* link not active, bring it up */
        } else {
    
	  /* search map entry for link to open, only open one,
	   * if no valid_vc from PVC, return -1 to indicate opening
	   */
	  if (first_atm_map->bc_linkstate == LINK_NULL) { 
	    map = first_svcmap;
	    while (map) {
	      if (map->atm_map->map_linkstate == LINK_NULL) {
		map->atm_map->map_linkstate = LINK_OPENING;
		first_atm_map->bc_linkstate = LINK_OPENING;

		if (atm_event_debug) {
		  print_proto_addr(NULL, &map->addr, map->protocol, addrstr);
		  buginf("\nopening MTP SVC %s", addrstr);
		}

		/* set timestamp when opening, clear it when active */
		GET_TIMESTAMP(first_atm_map->map_timestamp);
		if (!atmSmap_openSvcMtpConnection(idb, map)) {
		    /* failed to place call */
		    map->atm_map->map_linkstate = LINK_NULL;
		    first_atm_map->bc_linkstate = LINK_NULL;
		    goto tackle_inarps;
		}

		if (!valid_vc)
		  valid_vc = (vc_info_t *) -1;

		goto tackle_inarps;
	      }
	      map = map->atm_map->next_svc;
	    }
	    /* get here when all maps have failed, clear them out, 
	     * try next time 
	     */
	    map = first_svcmap;
	    while (map) {
	      map->atm_map->map_linkstate = LINK_NULL;
	      map = map->atm_map->next_svc;
	    }
	  }
	}


	/*
	 * Tackle all the InARP VCCs.  We need to fire off 
	 * route updates to all these differently
	 */
tackle_inarps:
	for(vc = atm->inarp_vc; vc; vc = vc->next_inarp) {
	  if (vc->swidb != idb || !ISVCACTIVE(vc)) {
	    continue;
	  }
	  if (!valid_vc) {
	      valid_vc = vc;
	  } else {
	      /*
	       * We've got a live one.  Have to broadcast to it.
	       */
	      dup = pak_duplicate(pak);
	      if (dup) {
	          atm_cpcs_encap(dup, atm, vc, snap_info, &map->hwconf, 0);
	          if (atm_packet_debug)
	              atm_show_packet(hwidb, dup, TRUE);
	          datagram_out(dup); /* Send it on it's way */
	    
	          vc->broadcasts++;
	          atm_counters[ATM_BROADCAST]++;
	          atm_counters[ATM_OUTPUT]++;	/* Count as output packets */
	          atm->outpkts++;
	      }
	  }   
	}

      }  /* matches if hwidb */
    }    /* matches if idb */

    /* untag everything if all entries are tagged */
    if (all_tagged) {
       while (start_map) {
          start_map->conn_tag = FALSE;
          start_map = start_map->next_bc;
       }
    }

    return(valid_vc);

  }



/*
 * atm_get_bridge_entry:
 * Function to determine if a mac level address exists in the bridge database.
 * OUTPUT/RETURN:
 * INPUT:
 */
static btetype *
atm_get_bridge_entry (idbtype *idb, uchar *mac_addr)
{
    ulong ultemp;
    spantype *span;
    btetype *mac;
    leveltype level;

    ultemp = mac_addr[4];
    ultemp ^= mac_addr[5];
    span = idb->span_ptr;
    if (span) {
	level = raise_interrupt_level(NETS_DISABLE);
	for (mac = span->bridge_tbl[ultemp]; mac; mac = mac->bte_next) {
	    if (ieee_equal(mac_addr, (uchar *) &mac->bte_mac_addr.sword[0])) {
		reset_interrupt_level(level);
		if ((mac->bte_action == BRIDGE_RECEIVE) && 
		    !(mac->bte_attributes & BTE_LAT))
		  return(NULL);
		return(mac);
	    }
	}
	reset_interrupt_level(level);
    }
    return(NULL);
}

/*
 * atm_bridge_broad:	Called from atm_bridge_vencap to broadcast a
 *	bridge frame to all AAL5SNAP VCs on a particular subinterface.
 * OUTPUT/RETURN:
 * INPUT:
 */
static vc_info_t *
atm_bridge_broadcast (idbtype *idb, paktype *pak, long dhost)
{
    atm_db_t *atm;						
    paktype *dup;						
    vc_info_t *vc = NULL, *sav_vc = NULL;				
    ushort ndx;							
    atm = idb->hwptr->atm_db;					
    for(ndx = 1; ndx < atm->config.max_intf_vcs; ndx++) {			
	vc = atm_getvc(atm, ndx);				
	if (!vc || vc->etype != ATM_ENCTYPE_SNAP || vc->swidb != idb) {
	    continue;
	}
	if ( pak->if_input && is_atm(pak->if_input->hwptr) && 
	    (vc->vc == pak->atm_vcid)) {
	    continue;
	}
	if(sav_vc == NULL) {		/* Save first one */
	    sav_vc = vc;
	    continue;
	}

	/*
	 * We've got a live one.  Have to broadcast to it.
	 */
	dup = pak_duplicate(pak);
	if (dup) {
	    atm_cpcs_encap(dup, atm, vc, atm_tbridge_oui, NULL, 0);
	    if(atm_packet_debug)
    		atm_show_packet(idb->hwptr, dup, TRUE);
	    datagram_out(dup);		/* Send it on it's way */

	    vc->broadcasts++;
	    atm_counters[ATM_BROADCAST]++;
	    atm_counters[ATM_OUTPUT]++;		/* Count as output packets */
	    atm->outpkts++;
	}
    }
    return(sav_vc);
}

/*
 * atm_bridge_vencap:	Encapsulate a bridged ATM packet
 * OUTPUT/RETURN:
 * INPUT:
 */
static bridge_vencap_t
atm_bridge_vencap (paktype *pak, long dhost)
{
    btetype *mac;
    uchar *mac_dest;
    vc_info_t *vc;
    staticmap_type *map;
    hwaddrtype *hwaddr = NULL;

    vc = NULL;
    switch(pak->linktype) {
    case LINK_IP:
	mac_dest = reg_invoke_ip_transit_bridging(dhost, pak->if_output);
	if (!mac_dest)
	    return(NULL);
	break;
    case LINK_BRIDGE:
	if ((dhost != 0L) && (dhost != -1L)) {
    	    mac = atm_get_bridge_entry(pak->if_output, (uchar *)dhost);
	    if (mac) {
		return((bridge_vencap_t)&mac->bte_proxy_addr);
	    } else {
		return(NULL);
	    }
	}
    /*
     * Fall Through
     */
    case LINK_DEC_SPANNING:
    case LINK_IEEE_SPANNING:
	map = staticmap_broadcast(pak->if_output, LINK_BRIDGE, ET_ATM);
	if (map) {
	    vc = atm_broadcast(pak->if_output, pak, map, atm_tbridge_oui);
	} else {
	    pak->enctype = ET_BRIDGE_ENCAPS;
	    vc = atm_bridge_broadcast(pak->if_output, pak, dhost);
	}
	if (!vc) {
	    return(NULL);
	} 
	if (vc == (vc_info_t *) -1) {
	  return(NULL);
	}

	return((bridge_vencap_t)&vc->vc);
	break;
    default:
        if ((pak->enctype == ET_BRIDGE_ENCAPS) &&
	    is_ieee_bcast((uchar *)dhost)) {
           /*
            * ET_BRIDGE_ENCAPS will only be set by the arp routines when we
            * wish to force transit bridge encapsulation no matter if
            * mac_dest is in the bridge table or not.  The call to
            * ieee_equal() is only a sanity check.  The only difference
            * between this encapsulation and the following is the use of
            * the bridge multicast address instead of something from the
            * bridge table.
            */
	    pak->enctype = ET_ATM;
	    if (!ether_vencap(pak, dhost))
		return(NULL);
	    map = staticmap_broadcast(pak->if_output, LINK_BRIDGE, ET_ATM);
	    vc = atm_broadcast(pak->if_output, pak, map, atm_tbridge_oui);
	    if (vc == (vc_info_t *) -1) {
	      return(NULL);
	    }
	    if(!vc) {
	        vc = atm_multicast(pak->if_output, pak, atm_tbridge_oui,
				   &hwaddr);
		pak->enctype = ET_BRIDGE_ENCAPS;
		if (!vc) {
		    vc = atm_bridge_broadcast(pak->if_output, pak, dhost);
		}
	    } else {
	        atm_multicast(pak->if_output, pak, atm_tbridge_oui, NULL);
	    }
	    return((bridge_vencap_t)&vc->vc);
	}
        mac_dest = (uchar *) dhost;
    }

    /*
     * IP datagram AND we know the MAC level address of where to send it.
     * If the datagram is in the bridging database -- encapsulate it as
     * a transit bridging datagram and return the vc number which
     * binds to the mac, else flood the datagram out every vc of the
     * sub-interface.
     */
    pak->enctype = ET_ATM;
    if (!ether_vencap(pak, dhost))
        return(NULL);
    
    if ((mac = (btetype *) atm_get_bridge_entry(pak->if_output, mac_dest))){
        return((bridge_vencap_t)&mac->bte_proxy_addr);
    } else {
        pak->enctype = ET_BRIDGE_ENCAPS;
        vc = atm_bridge_broadcast(pak->if_output, pak, dhost);
        return((bridge_vencap_t)&vc->vc);
    }
}

/*
 * atm_interface_up:  check if the ATM hw is really ready.  It is possible
 * that the hw is up but the vc's are not fully initialized.  This routine
 * returns TRUE only if both the hw is up and the vc's are initialized.
 */
boolean
atm_interface_up (idbtype *idb)
{
   hwidbtype   *hwout;
   atm_db_t    *atm;

   if (idb == NULL)
       return(FALSE);

   if (!interface_up_simple(idb)) {
       return(FALSE);
   }
   
   if (idb->hwptr) {
       hwout = idb->hwptr;
       if (!hw_interface_up(hwout)) {	/* If interface down, err. out */
	   if (atm_errors_debug)
	       buginf("\nATM(%s): interface not up", hwout->hw_namestring);
	   return(FALSE);
       }

       atm = hwout->atm_db;
       if (! (atm && (atm->flags & ATM_FLAG_UP)) ) {
	   if (atm_errors_debug)
	       buginf("\nATM(%s): ATM not active yet", hwout->hw_namestring);
	   return(FALSE);
       }
   } else {
       return(FALSE);			/* No hwidb. Not up */
   }
   return(TRUE);
}  

/*
 * atm_saal_encap:      Called by SVC code to encapsulate a signal packet.
 * OUTPUT/
 */
boolean
atm_saal_encap (paktype *pak, vc_info_t *vc)
{
    idbtype     *idb;

    idb = pak->if_output;
    if (idb && atm_interface_up(idb)) {
	hwidbtype   *hwout;
	atm_db_t    *atm;
	ushort      *enclen, encsize;

	hwout = idb->hwptr;
	atm = hwout->atm_db;
	if ((vc->flags & VC_FLAG_AAL_MASK) == ATM_VC_AALQSAAL) {
	    encsize = vc->encapsize - ATM_ENCAP_HDRBYTES;

	    encsize += (*atm->funcs.encapsulate)(atm, pak, vc, 0);
	    enclen = (ushort *)(pak->datagramstart - sizeof(ushort));
	    *enclen = encsize;
	    atm_show_packet(hwout, pak, TRUE);
	    atm_out_inc(atm, vc);

	    return(TRUE);
	} else {
            if (atm_errors_debug)
                buginf("\nATM(%s): saal_encap - vc %d not config'd for saal",
                       hwout->hw_namestring, vc->vc);
        }
    }
    return(FALSE);
}

/* 
 * atm_vencap:	Variable encap size entry.  Mostly for ATALK
 * OUTPUT/RETURN:
 *	Return TRUE if successful in doing the encaps, else FALSE.
 * INPUT:
 *	pak->datagramstart == beginning of the protocol to encapsulate
 *	pak->datagramsize == size of the PDU to encapsulate
 *	pak->linktype == Type of protocol being encapsulated
 *	dhost == protocol address to send to.
 */
static boolean
atm_vencap (paktype *pak, long dhost)
{
    idbtype 	*idb;
    hwidbtype 	*hwout;
    vc_info_t 	*vc_info = NULL;
    atm_db_t 	*atm = NULL;
    staticmap_type *map = NULL;
    boolean	broadcast;
    vcd_t	vcnum = -1;
    uchar const *snap_info = zerosnap;
    hwaddrtype 	*hwaddr = NULL, atbl;
    uchar       l3pid = 0;
    
    idb = pak->if_output;
    if (!idb) {
	return(FALSE);
    }

    if (!atm_interface_up(idb)) {
	return(FALSE);
    }

    /*
     * If the LANE Client is active on the subinterface,
     * use the lane specific vencap routine and bail.
     */
    if (lec_enabled(idb)) {
	return(reg_invoke_lane_data_vencap(pak, dhost));
    }

    hwout = idb->hwptr;
    atm = hwout->atm_db;
    /*
     * If this interface is a p2p connection, then send all packets to
     * the VCC on this idb.
     * No static-map lookup is required.
     */
    if (is_p2p(idb)) {
	vc_info = atm_get_p2pvc(idb,TRUE);
	if (!vc_info) {
	    if (atm_errors_debug) {
		buginf("\nATM(%s):point-to-point interface does not have a VCD",
			idb->namestring);
	    }
	    return(FALSE);
	}
	/* If it is AAL34 encap, we need to determine the destination address */
	if ((vc_info->flags & VC_FLAG_AAL_MASK) != ATM_VC_AAL34SMDS) {
	    if (((vc_info->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL5SNAP) &&
                (vc_info->swidb->tbridge_on_swidb)) {
                /*
                 * A point-to-point AAL5SNAP VC which is bridging the
                 * traffic. Write the correct 1483 bridged encaps on this
                 */
                snap_info = atm_tbridge_oui;
                if (pak->linktype != LINK_BRIDGE && 
                    pak->linktype != LINK_IEEE_SPANNING &&
                    pak->linktype != LINK_DEC_SPANNING) {

                    /* Add ethernet mac header to the packet. */
                    pak->enctype = ET_ATM;
                    if (ether_vencap(pak, dhost)) {
                        pak->enctype = ET_BRIDGE_ENCAPS;
                    } else {
                        return(FALSE);
                    }
                }
	    }
            goto atm_p2p_ok;
	}
    }

    if (!reg_invoke_media_atm_macaddr(pak->linktype, 
				      pak, 
				      dhost, 
				      &atbl, 
				      &snap_info,
				      &map, 
				      &broadcast)) {
	if (!broadcast) {
	    if(atm_errors_debug) {
	        buginf("\nATM(%s): Encapsulation error1, link=%x, host=%x",
		    hwout->hw_namestring, pak->linktype, dhost);
	    }
	    if (!registry_case_used(REG_MEDIA, SERVICE_MEDIA_ATM_MACADDR,
		pak->linktype))
	       errmsg(&msgsym(BADMACREG, LINK), idb->namestring,
		      pak->linktype);
	    return(FALSE);
	}
    }
    /*
     * On return from the registry call, the following may have occurred
     *
     * 	snap_info == ptr. to 3 byte OUI to use for this packet
     *		atm_tbridge_oui(00 80 c2) for bridged frames
     *		zerosnap(00 00 00) for normal routed(non-atalk) frames
     *		applesnap(?? ?? ??) for atalk frames
     *
     * If ROUTING the particular protocol:
     *	map == ptr. to a STATIC MAP entry, for STATIC maps
     *	atbl == E.164 SMDS address if found in the ARP table
     *	broadcast == indication of whether to flood packet
     *		TRUE - Send packet to all BROADCAST Static-MAP entries
     *		FALSE - Send packet only to VC returned
     *
     * if BRIDGING the particular protocol:
     *	atbl - Contains the address in question.
     *		Could be a VCD# of length 2 or E.164 of length 8
     *	broadcast == FALSE
     *	map == NULL
     *	pak->linktype == LINK_BRIDGE
     *	pak->enctype == ET_BRIDGE_ENCAPS
     *
     * For LLC traffic:
     * atbl - Contains VCD# of length 2
     * map == NULL
     * broadcast == FALSE
     * pak->linktype == LINK_LLC2
     * pak->enctype == ET_LLC2
     * dhost - dhost is really a pointer to the dte string for the
     *         connection and L3_PID values can be found at 
     *         ((uchar*) dhost) + ATM_L3_PID_DTE. This should be
     *         cleaned up in the "real" implementation.
     * 
     *	RULE: ARP table entries will always take precedence of STATIC
     *		MAP table entries.
     */

    /*
     * Deal with entries from the ARP table....
     */
    if (atbl.length) {
	/*
	 * Check to see if we got an E.164 address.  If so, go fetch
	 * the AAL3/4 VC for this interface
	 */
	if ((atbl.length > STATIONLEN_ATMVC) && 
	    (SMDS_ISINDIV(atbl.addr[0]) || 
	     SMDS_ISGROUP(atbl.addr[0]))) {
	    vcnum = atm_get_aal34vcnum(idb);
	} else {
	/*
	 * OOPS! We got a VCNUM back.  Must be an AAL5 connection
	 */
	    vcnum = *(ushort *)atbl.addr;
	}
	/*
	 * Make sure that the hwaddr gets passed out of the arp table.
	 */
	hwaddr = &atbl;
    }

    if (map && map->hwconf.type == STATION_SMDS64) { /* STATIC SMDS MAP entry */
	    vcnum = atm_get_aal34vcnum(idb);
    }

    if(!broadcast && ATM_BAD_VC(atm, vcnum) &&
	   !(vc_info = atm_addr_lookup(pak->if_output, atm, map))) {
	if (atm_errors_debug)
   	    buginf("\nATM(%s): Encapsulation error2, link=%x, host=%x",
		hwout->hw_namestring, pak->linktype, dhost);
	if (!registry_case_used(REG_MEDIA, SERVICE_MEDIA_ATM_MACADDR,
		pak->linktype))
		errmsg(&msgsym(BADMACREG, LINK), idb->namestring,
		       pak->linktype);
	return(FALSE);
    }

    hwaddr = (map) ? &map->hwconf : hwaddr;

    /*
     * Now we know if its broadcast or not.  If broadcast, look in the
     * map for the broadcast address.  If unicast, look first in the
     * static map.  If not found, and the protocol supports arp, the
     * call arp.  If we emerge without an address, complain.
     * One slight exception: arp can send mac level stuff.
     */
    if (broadcast)  {
        vc_info = atm_broadcast(idb, pak, map, snap_info);

	if (vc_info == (vc_info_t *) -1) {
	  if (atm_event_debug)
	    buginf("\nATM(%s): VC opening",idb->namestring);
	  return(FALSE);
	}

	if(!vc_info) {
	    vc_info = atm_multicast(idb, pak, snap_info, &hwaddr);
	} else {
	    atm_multicast(idb, pak, snap_info, NULL);
	}
    }

    if (!vc_info && !broadcast) {
	if (!(vc_info = atm_getvc(atm, vcnum))) {
	    if (atm_errors_debug)
	    	buginf("\nATM(%s): Encapsulation error, vc=%d not connected",
		    idb->namestring, vcnum);
	    atm_counters[ATM_OUT_NOVC]++;
	    return(FALSE);
        }
    }

    if (!vc_info) {
        if (atm_errors_debug)
	  buginf("\nATM(%s) Send: Error in encapsulation, No VC for address 0x%x",
	      idb->namestring, dhost);
	atm_counters[ATM_OUT_NOVC]++;
        return(FALSE);
    }

atm_p2p_ok:
    /* if signalling vc chosen, open svc connection, drop packet */
    if (vc_info->vc == atm->sig_vcnum) {
       (void)atmSmap_openSvcConnection(idb, map, 0);
       return(FALSE);
    }

    /* If SMDS address not configured, drop the packet */
    if ((vc_info->flags & VC_FLAG_AAL_MASK) == ATM_VC_AAL34SMDS) {
        if (idb->vaddr == NULL || hwaddr == NULL) {
            if (atm_errors_debug)
                buginf("\nATM(%s): AAL34 encaps error, Address not configured",
                idb->namestring);
            return(FALSE);
        }
    }

    /*
     * Sanity check.  If VC is in HOLD state then we can only send
     * ARP packets.  Wait for ARP processing before enabling this
     * VC again. 
     */
    if (IS_VC_HOLD(vc_info)) {
	return(FALSE);
    }

    if (pak->linktype == LINK_LLC2) {
	l3pid = *(((uchar*) dhost) + ATM_L3_PID_DTE);
    }

    atm_cpcs_encap(pak, atm, vc_info, snap_info, hwaddr, l3pid);

    /*
     * All that remains to be done now is to add this guy to the fastswitch
     * cache, and to account for it.  However, if this is a dummy ENCAPS_ONLY
     * packet, then we don't want to do any of this, so bail now.
     */
    if (pak->flags & PAK_ENCAPS_ONLY) {
	return(TRUE);
    }

    atm_show_packet(hwout, pak, TRUE);

    /*
     * Make sure routing cache is populated by transit traffic.
     */
    if (pak->enctype != ET_BRIDGE_ENCAPS) {
	pak->enctype = ET_ATM;
    }

    reg_invoke_fs_cache_update(pak->linktype, pak, 0L);

    if(broadcast) {
	atm_counters[ATM_BROADCAST]++;
	vc_info->broadcasts++;
    } else {
    	vc_info->outpkts++;
    }
    atm_counters[ATM_OUTPUT]++;
    atm->outpkts++;

    return(TRUE);
}

/*
 * atm_bridge_it:	An inline to do generic BRIDGING stuff.
 * OUTPUT/RETURN:
 * Returns with TRUE if tbridge was not able to process the packet.
 * Returns FALSE if tbridge has already forwarded the packet and no
 *	more processing is required on our part.
 * We know that tbridge did it's work, because it initialized pak->if_output
 * with the IDB of the next hop interface the packet was sent to.
 */
static boolean
atm_bridge_it (vc_info_t *vc, paktype *pak)
{
    ether_hdr *ether;

    if (!vc->swidb->tbridge_on_swidb) {			
	retbuffer(pak);				
	return(FALSE);					
    }						
    pak->flags |= PAK_TBRIDGE_IN;			

    /*
     * assume arpa encaps, bad assumption, BTW...
     */
    pak->enctype = ET_ARPA;				

    pak->encsize += ATM_BRIDGE_PAD_SIZE;
    pak->datagramstart += pak->encsize;
    pak->datagramsize -= pak->encsize;
    pak->mac_start = pak->datagramstart;
    ether = (ether_hdr *)pak->mac_start;
    pak->addr_start = ether->daddr;
    pak->network_start = ether->data;
    net_bridge_copy(pak);				
    if (pak->if_output) { /* If !0 tbridge worked */ 
	retbuffer(pak);				
	return(FALSE);
    }						
    pak->datagramstart -= pak->encsize;
    pak->datagramsize += pak->encsize;
    return(TRUE);
}

/*
 * figure out what kind of ethernet mac type it is.  remember
 * all packets were changed to ethernet pid in atm_input()
 */
static void
atm_aal5_tbridge_classify_enctype (paktype *pak)
{
    ether_hdr *ether;
    uchar *datagramstart;

    pak->encsize = ATM_BRIDGE_ETHER_ENCAPBYTES;

    datagramstart = pak->datagramstart + ATM_BRIDGE_ETHER_ENCAPBYTES;

    /*
     * set these bad boys up for process level...
     */
    ether = (ether_hdr *)datagramstart;

    if (GETSHORT(&ether->type_or_len) <= MAXETHERSIZE) {
	sap_hdr *sap = (sap_hdr *)ether->data;

	if (GETSHORT(&sap->dsap) == SNAPSNAP)
	    pak->enctype = ET_SNAP;
	else
	    pak->enctype = ET_SAP;
    } else
	pak->enctype = ET_ARPA;
}
/*
 * atm_bridge_tr_fddi_prep: An inline to convert tr and fddi frames ethernet
 * OUTPUT/RETURN:
 * no return
 */
static void
atm_bridge_tr_fddi_prep (paktype *pak)
{
    int i;
    uchar *dgstart = pak->datagramstart + ATM_BRIDGE_ETHER_ENCAPBYTES;
    uchar *dgend   = pak->datagramstart + ATM_BRIDGE_ETHER_ENCAPBYTES +
	    TR_ACFC_SIZE;
    ether_hdr *ether;
    snap_hdr *snap;

    /*
     * slide the dst and src address to the left 2 bytes.
     */
    for (i = 0; i < (IEEEBYTES * 2) ; i++)
	*dgstart++ = *dgend++;

    /*
     * reset 1483 type to pid ether
     */
    snap = (snap_hdr *)(pak->datagramstart + ATM_ENCAP_HDRBYTES);
    PUTSHORT(&snap->type, TBRIDGE_PID_8023);

    /*
     * add length to make it a sap mac header
     */
    ether = (ether_hdr *)(pak->datagramstart + ATM_BRIDGE_ETHER_ENCAPBYTES);
    PUTSHORT(&ether->type_or_len,
	     (pak->datagramsize - ATM_BRIDGE_ETHER_ENCAPBYTES -
	      ETHER_ARPA_ENCAPBYTES));
}

/*
 * atm_raw_enqueue:	Called to do quick link checking to see if
 *	ARP specific stuff needs to be done for RFC1577
 * OUTPUT/RETURN:
 *	Packet is queued to ATM_ARP process Q for further processing if 
 *	it's an ARP packet.  Else, we simply enqueue it to the regular
 *	raw_enqueue.
 * INPUT:
 *	idbtype -- Obvious
 *	paktype -- Obvious
 */
static void
atm_raw_enqueue (hwidbtype *hwidb, paktype *pak)
{
    atm_db_t *atm;

    if (hwidb) {
	atm = hwidb->atm_db;
    }
    
    /*
     * Snap the frame to alignment if required
     */
    ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));

    pak->linktype = (*hwidb->getlink)(pak);

    GET_TIMESTAMP(hwidb->lastinput);	/* remember time of last input */
    
    if (pak->linktype == LINK_ARP) {
        if (!reg_invoke_atm_arp_enqueue(pak)) {
            datagram_done(pak);
        }
        return;
    }
    reg_invoke_raw_enqueue(pak->linktype, pak);
}

/*
 * atm_input:	Called to handle incoming ATM frames
 *	Qualifies the contents of the packet.
 * OUTPUT/RETURN:
 *	If correct, the packet is enqueued to the RAW input queue for
 *	further processing.
 *
 * INPUT:
 *	pak->datagramstart == Beginning of the Encapsulated area.  The VC
 *		is the first word of encapsulation.
 *	pak->datagramsize == size of the entire datagram.
 */
static void
atm_input (hwidbtype *hwidb, paktype *pak)
{
    atm_db_t	*atm;
    vc_info_t	*vc_info;
    atm_encap_t	*inpkt;
    snap_hdr	*snap;
    sap_hdr	*sap;
    smdstype	*smds;
    idbtype	*idb;
    ushort	lecid;
    trrif_t	*trh;
    int		riflen;
    sap_hdr	*llc1;		/* pointer to llc-1 header */
    uchar	*frameptr;
    uchar	conch;
    lec_info_t	*lec_info;

    atm_show_packet(hwidb, pak, FALSE);
    atm = hwidb->atm_db;
    inpkt = (atm_encap_t *)pak->datagramstart;
    vc_info = atm->fast_vc_info;
    /*
     * If we don't know about this VC, flame all over this & toss.
     */
    if (!vc_info) {
	if (atm_errors_debug)
	    buginf("\nATM(%s): vc %d packet received and not configured",
		   hwidb->hw_namestring, inpkt->vcnum);
	retbuffer(pak);
	atm_counters[ATM_INPUT_NOVC]++;		/* Count it. */
	atm_counters[ATM_INPUT]++;		/* Count as input packets */
	return;
    }
    idb = vc_info->swidb;
    pak->flags &= ~PAK_TBRIDGE_IN;

    /*
     * If IRB(Integrated Routing and Bridging) is enabled, 
     * the input interface *may* have been overwritten to the BVI
     * (Bridge-group Virtual Interface) in tbridging fast-switching
     * code where it makes route/bridge decision. Don't overwrite it!
     * Instead, record the received interface.
     */
    if (global_irb_enable && is_bvi(pak->if_input->hwptr)) {
	pak->irb_rcvd_input = vc_info->swidb;
    } else {
	pak->if_input = vc_info->swidb;
    }

    atm_in_inc(atm, vc_info);

    switch(vc_info->flags & VC_FLAG_AAL_MASK) {
    case ATM_VC_AAL5SNAP:
	sap = (sap_hdr *)inpkt->data;
	snap = (snap_hdr *)inpkt->data;

	if (GETSHORT(&snap->dsap) == SNAPSNAP) {
	   /*
	    * Do normal SNAP(SNAP) encapsulation stuff
	    */
	    if (ieee_equal_oui(snap->oui, atm_tbridge_oui)) {
		/*
		 * these packets somehow didn't get transparently bridged
		 * at fast level.  they are going to process level
		 * for some reason.  figure out what kind of ethernet
		 * mac encap it is...
		 */
		switch(GETSHORT(&snap->type)) {
		case TBRIDGE_PID_8023FCS:
		    pak->datagramsize -= FCS8023_SIZE;	/* Strip FCS */
		    /*
		     * Fall Through
		     */
		case TBRIDGE_PID_8023:
		    atm_aal5_tbridge_classify_enctype(pak);
		    break;

		case TBRIDGE_PID_FDDIFCS:
		    pak->datagramsize -= FDDI_FCS;
		    /*
		     * Fall Through
		     */
		case TBRIDGE_PID_FDDI:
		    atm_bridge_tr_fddi_prep(pak);
		    atm_aal5_tbridge_classify_enctype(pak);
		    break;

		case TBRIDGE_PID_8025FCS:
		    pak->datagramsize -= TR_FCS_SIZE;
		    /*
		     * Fall Through
		     */
		case TBRIDGE_PID_8025:
		    atm_bridge_tr_fddi_prep(pak);
		    atm_aal5_tbridge_classify_enctype(pak);
		    break;

		/*
		 * for backwards compatibility.  sigh...
		 */
		case TBRIDGE_PID_BPDU:
		    break;

		default:
		    if (atm_errors_debug)
			buginf("\nATM(%s): encap type %d not supported",
			       hwidb->hw_namestring, GETSHORT(&snap->type));
		    retbuffer(pak);
		    return;
		}
	        raw_enqueue(hwidb, pak);		/* Check specials */
	        return;
	    }
	    atm_raw_enqueue(hwidb, pak);		/* Check specials */
	    return;
	} else if (GETSHORT(&sap->dsap) == SAPSAP_CLNS) {
	/*
	 * Got a CLNS frame to deal with.  Have to shift the packet around
	 * for realignment with the 'LINE'.
	 */
	    pak->enctype = ET_SAP;
	    memmove(pak->datagramstart + ATM_SNAP_SAP_ADJBYTES,
		    pak->datagramstart, pak->datagramsize);
	    pak->datagramstart += ATM_SNAP_SAP_ADJBYTES;
	    pak->info_start += ATM_SNAP_SAP_ADJBYTES;
	    pak->mac_start += ATM_SNAP_SAP_ADJBYTES;
	} else {
	    if (atm_errors_debug)
	    	buginf("\nATM(%s): VC(%d) Bad SAP received %04x", 
		    idb->namestring,
		    vc_info->vc, (snap->dsap << 8 | snap->ssap));
	    retbuffer(pak);
	    return;
	}
	break;
    case ATM_VC_AAL5MUX:
    /*
     * Nothing to do here for MUX case.  Let atm_getlink() handle it.
     */
	switch(vc_info->etype) {
	case TYPE_BRIDGE:
	    if(!(atm_bridge_it(vc_info, pak))) {
		return;
	    }
	    break;
	}
	break;
    case ATM_VC_AALQSAAL:
	pak->datagramstart += ATM_MUX_ENCAPBYTES;
	pak->datagramsize -= ATM_MUX_ENCAPBYTES;
        saal_enqueue(pak, vc_info);  /* signalling packet - give to SSCOP */
        return;
	break;
    case ATM_VC_AAL5NLPID:
	switch(GETSHORT((uchar *)pak->datagramstart + ATM_ENCAP_HDRBYTES)) {
	case ATM_NLPID_IETF_IP:
	    PUTSHORT(((uchar *)pak->datagramstart + ATM_ENCAP_HDRBYTES),
		TYPE_IP10MB);
	    break;
	case ATM_NLPID_ISO:
	case ATM_NLPID_ISIS:
	case ATM_NLPID_ESIS:
	case ATM_NLPID_CLNP:
	    pak->info_start -= 1;
	    pak->network_start -= 1;
	    PUTSHORT(((uchar *)pak->info_start), TYPE_CLNS);
	    break;
	case ATM_NLPID_SNAP0PAD:
	case ATM_NLPID_SNAP1PAD:
	case ATM_NLPID_Q2931:
	/*
	 * Do nothing here. 
	 */
	    break;
	default:
	    if(atm_errors_debug) {
	        buginf("\nATM(%s): Bad VC(%d) encapsulation configured %x", 
		    idb->namestring, vc_info->vc, vc_info->flags);
	    }
	    retbuffer(pak);
	    return;
	}
	break;
    case ATM_VC_AAL34SMDS:
	smds = (smdstype *)inpkt->data;
	sap = (sap_hdr *)smds->data;
	snap = (snap_hdr *)smds->data;

	if (!atm_smds_l3_check(smds, idb)) {
	    retbuffer(pak);
	    return;
	}

	if (GETSHORT(&snap->dsap) == SNAPSNAP) {
	   /*
	    * Do normal SNAP(SNAP) encapsulation stuff
	    */
	    if (ieee_equal_oui(snap->oui, atm_tbridge_oui) 
	      && (GETSHORT(&snap->type) == TBRIDGE_PID_8023)) {

		if(!(atm_bridge_it(vc_info, pak))) {
		    return;
		}
	    }
	} else if (GETSHORT(&sap->dsap) == SAPSAP_CLNS) {
	/*
	 * Got a CLNS frame to deal with.  Have to shift the packet around
	 * for realignment with the 'LINE'.
	 */
	    pak->enctype = ET_SAP;
	    memmove(pak->datagramstart + ATM_SNAP_SAP_ADJBYTES,
		    pak->datagramstart, pak->datagramsize);
	    pak->datagramstart += ATM_SNAP_SAP_ADJBYTES;
	    pak->info_start += ATM_SNAP_SAP_ADJBYTES;
	    pak->mac_start += ATM_SNAP_SAP_ADJBYTES;
	} else {
	    if (atm_errors_debug) {
	    	buginf("\nATM(%s): VC(%d) Bad SAP received %04x", 
		    idb->namestring,
		    vc_info->vc, (snap->dsap << 8 | snap->ssap));
	    }
	    retbuffer(pak);
	    return;
	}
	break;
    case ATM_VC_AAL5LANE:
	switch(vc_info->user_vc_type) {
	case LANE_LEC_DIRECT:
	case LANE_LEC_DISTRIBUTE:
	case LANE_LEC_CONFIGURE:
	    pak->datagramstart += ATM_LANE_CONTROL_ENCAPBYTES;
	    pak->datagramsize -= ATM_LANE_CONTROL_ENCAPBYTES;
	    if (!reg_invoke_lec_enqueue(pak)) {
		retbuffer(pak);
	    }
	    return;
	case LANE_LES_DIRECT:
	case LANE_LES_DISTRIBUTE:
	    pak->datagramstart += ATM_LANE_CONTROL_ENCAPBYTES;
	    pak->datagramsize -= ATM_LANE_CONTROL_ENCAPBYTES;
	    if (!reg_invoke_lsv_enqueue(pak)) {
		retbuffer(pak);
	    }
	    return;
	case LANE_CONFIGURE:
	    pak->datagramstart += ATM_LANE_CONTROL_ENCAPBYTES;
	    pak->datagramsize -= ATM_LANE_CONTROL_ENCAPBYTES;
	    if (!reg_invoke_lecs_enqueue(pak)) {
		retbuffer(pak);
	    }
	    return;
	case LANE_LEC_FORWARD:
	case LANE_LEC_DATA:
	case LANE_LEC_SEND:
	    /*
             * Packet on data vc - from BUS or another LEC.
	     * If marker is set, this is a flush packet which must be
	     * handled by the LEC.  For real data, step over the LECID
	     * and fall through to raw_enqueue.
	     */
	    pak->datagramstart += ATM_LANE_CONTROL_ENCAPBYTES;
	    pak->datagramsize -= ATM_LANE_CONTROL_ENCAPBYTES;

	    lecid = GETSHORT((uchar *)pak->datagramstart);
	    if ((lecid & LANE_MARKER_MASK) == LANE_MARKER) {
		if (!reg_invoke_lec_enqueue(pak)) {
		    retbuffer(pak);
		}
		return;
	    }

	    pak->datagramstart += ATM_LANE_LECID_ENCAPBYTES;
            pak->datagramsize -= ATM_LANE_LECID_ENCAPBYTES;
	    break;
  	case LANE_LEC_TR_FORWARD:
	case LANE_LEC_TR_SEND:
  	case LANE_LEC_TR_DATA:
 
	    pak->datagramstart += ATM_LANE_CONTROL_ENCAPBYTES;
	    pak->datagramsize -= ATM_LANE_CONTROL_ENCAPBYTES;
 
 	    lecid = GETSHORT((uchar *)pak->datagramstart);
 	    if ((lecid & LANE_MARKER_MASK) == LANE_MARKER) {
 		if (!reg_invoke_lec_enqueue(pak)) {
 		    retbuffer(pak);
 		}
 		return;
 	    }
 
 	    pak->datagramstart += ATM_LANE_LECID_ENCAPBYTES;
            pak->datagramsize -= ATM_LANE_LECID_ENCAPBYTES;
 
            if (vc_info->swidb->lane_info->type == LANE_TYPE_802_5) {
		if (lec_enabled(vc_info->swidb)) {
		    lec_info = lec_info_from_swidb(vc_info->swidb);    
		    if (!lec_info)
			return;
		    if (lec_info->lec_is_srb){
			trh = (trrif_t *)pak->datagramstart;
			riflen = (trh->saddr[0] & TR_RII) ? 
			    trh->rc_blth & RC_LTHMASK : 0;

			pak->llc2_sapoffset = TRING_ENCAPBYTES + riflen;
			frameptr = pak->datagramstart + pak->llc2_sapoffset;
 
			llc1 = (sap_hdr *)(pak->datagramstart + 
					   TRING_ENCAPBYTES + 
					   riflen);
			conch = llc1->control & ~P_BIT;     /* snuff out p/f bit */
 
			if (((GETSHORT(&trh->daddr[0]) == TRFUNC_UPPER) &&
			     (GETLONG(&trh->daddr[2]) == TRFUNC_NETBIOS) && 
			     (llc1->dsap == SAP_NETBIOS)) ||
                             ((trh->saddr[0] & TR_RII) && (trh->rc_blth & RC_BROADBIT))){
			    reg_invoke_tr_lane_srb_enq(lec_info, pak);
			    retbuffer(pak);
			    return;
			} else {
			    if (trh->saddr[0] & TR_RII){
				reg_invoke_tr_lane_vrforward_pak(lec_info, pak);
                                retbuffer(pak);
                                return;
			    } else {
				/* Do protocol processing */ 
				/* We assume LLC/SNAP encapsulation at this point */
				pak->enctype = ET_SNAP;
				pak->info_start = (uchar *)llc1;
				pak->network_start = pak->info_start +
				    (TRING_SNAP_ENCAPBYTES - TRING_ENCAPBYTES);
				raw_enqueue(hwidb, pak);	
				return;
			    }
			}
		    }
		}
 	    }
 
	    break;
	case LANE_BUS_SEND:
	case LANE_BUS_FORWARD:
	default:
	    /* 
	     * BUS packets shouldn't even get here, since they should
	     * have been handled at fastswitch level.
	     */ 
	    if (atm_errors_debug) {
		buginf("\nATM(%s): Unexpected LANE packet on VC(%d)",
		       idb->namestring, vc_info->vc);
	    }
	    retbuffer(pak);
	    return;
	}
	break;
    case ATM_VC_AAL5ILMI:	/* Got an ILMI frame */
	if (atm_ilmi_enabled) {
	    reg_invoke_atm_ilmi_pak_enqueue(pak);
	} else {
	    retbuffer(pak);
	}
	return;
	/*
	 * Below we let fall the unsupported/illegal VC encaps types..
	 */
    case ATM_VC_AAL5FRNLPID:
    default:
	if(atm_errors_debug)  {
	    buginf("\nATM(%s): Bad VC(%d) encapsulation configured %x", 
		   idb->namestring, vc_info->vc, vc_info->flags);
	}
	retbuffer(pak);
	return;
    }

    /*
     * Got a clean packet.  All adjustments made for LLC encapsulation.
     * Now call the protocol input routine to handle the PDU.
     */
    raw_enqueue(hwidb, pak);		/* Do protocol processing */
    return;
}

/*
 *  atm_hangup_vc
 *
 *  Cause the passed VC to get hung up.  This goes through the Signalling
 *  API, which is sort of odd, as we are a driver, not an application from
 *  a strict point of view, but it works.
 */
void
atm_hangup_vc (vc_info_t *vc)
{
    sig_api_msg *msg;
    sig_api_release *release;
    atmSvcType *svc;

    /*
     *  The svc structure address is the call id used by the API
     */
    svc = atmSig_findSvcBlockByVC(vc->swidb, vc->vc); 
    if (svc == NULL)
	return;
    
    msg = malloc(sizeof(sig_api_msg));
    if (msg == NULL)
	return;
   
    msg->opcode = ATM_RELEASE;
    release = &msg->u.release;
    release->call_id = svc;
    release->cause = ATM_NORMAL_UNSPECIFIED;

    /* send to signalling code */
    process_enqueue(atmSig_outputQ, msg);
}

/*
 * atm_periodic: Called periodically to do simple time management chores.
 *	(a) Invoke Hardware specific periodic function.
 *	(b) Collect stats as necessary to avoid overflow in the ATM interface.
 *	(c) Issue InARPs to VCs so configured.
 */
static void
atm_periodic (hwidbtype *hwidb)
{
    atm_db_t *atm;
    vc_info_t *vc;

    atm = hwidb->atm_db;
    if (atm->flags & ATM_FLAG_FLUSH_CACHE)
	atm_flush_ar_cache(hwidb);
    (*atm->funcs.periodic)(hwidb);

    if (!hw_interface_up(hwidb)) {
	return;
    }
    atm_free_inactive_vc(atm);
    /*
     * Every VCD_UPDATE_INTERVAL seconds, we query the ATM IP to return
     * the per-VC statistics so that we may update the local counters.
     * We only initiate the request here. The IP will return the results
     * in special packets that are processed in a special function.
     */
    if (CLOCK_OUTSIDE_INTERVAL(atm->vcd_update, VCD_UPDATE_INTERVAL)) {
	if (atm->funcs.getvc_stats != NULL) {
	    (*atm->funcs.getvc_stats)(hwidb, NULL); /* Initiate request */
	    GET_TIMESTAMP(atm->vcd_update);
	}

	/*
	 * Check for an overly idle SVCs.  Involves checking all SVCs
	 * on the atm.  At present we only do this every VCD_UPDATE_INTERVAL
	 * time because that is the only time they can change.  When we are
	 * supporting more than just the 7000 implementation, this may need
	 * to change.
	 */
	if(atm_db->vc_hash_tbl) {
	    int ndx;
	
	    for(ndx = 0; ndx < ATM_HASH_DEFAULT; ndx++) {
		vc = atm_db->vc_hash_tbl[ndx];
		for(; vc; vc = vc->next_vc) {
		    /*
		     *  We only worry about SVCs here, as there is nothing to
		     *  be done with an idle PVC.
		     */
		    if (!IS_ATMVC_SVC(vc) || !ISVCACTIVE(vc) ||
			(vc->flags & VC_FLAG_NOIDLETIME))
			continue;
			
		    /*
		     *  Check this VC's counters to see if they've changed
		     *  since last time
		     */
		    if (vc->total_in_pkts != vc->last_in_pkts ||
			vc->total_out_pkts != vc->last_out_pkts) {
		        /*
			 * Something was sent or received.  Restart the timer
			 */
		        if (atm->idle_time != 0) {
			    TIMER_START(vc->idle_timer, atm->idle_time);
			} else {
			    TIMER_STOP(vc->idle_timer);
			}
			vc->last_in_pkts = vc->total_in_pkts;
			vc->last_out_pkts = vc->total_out_pkts;
		    } else {
			/*
			 * This was an idle period.  Does that mean the timer
			 * has expired?
			 */
			if (TIMER_RUNNING(vc->idle_timer)) {
			    if (AWAKE(vc->idle_timer)) {
				/* we timed out */
				if (atm_event_debug)
				    buginf("\nVC Idle Timeout: %s vc %d",
					   vc->swidb->namestring, vc->vc);
				atm_hangup_vc(vc);
			    }
			} else {
			    /*
			     *  this can never happen, so better plan for it.
			     */
			    if (atm->idle_time != 0) {
				TIMER_START(vc->idle_timer, atm->idle_time);
			    } else {
				TIMER_STOP(vc->idle_timer);
			    }
			}
		    }
		}
	    }
	}
    }
}

/*
 * atm_isbcast:
 */
static fuzzy
atm_isbcast (hwidbtype *hwidb, paktype *pak)
{
    return(FUZ_NOTSURE);
}

/*
 * atm_clear_counter_support:
 *	Called to flush the per-VC counters as part of the 'clear counter'
 *	command.
 *	Also clears the per-interface ATM counters.
 */
static void
atm_clear_counter_support (hwidbtype *hwidb)
{
    atm_db_t *atm;
    uint ndx;
    vc_info_t *vc_info;

    atm = hwidb->atm_db;
    if (!atm) {
	return;
    }
    for (ndx = 1; ndx < atm->config.max_intf_vcs; ndx++) {
	vc_info = atm_getvc_inline(atm, ndx);
	if (!vc_info) {
	    continue;
	}
	/*
	 * ZERO out all the per-VC counters
	 */
	vc_info->inpkts = vc_info->outpkts = 0;
	vc_info->infast = vc_info->outfast = 0;
	vc_info->in_as = vc_info->out_as = 0;
	vc_info->in_pkts = vc_info->out_pkts = 0;
	vc_info->total_in_pkts = vc_info->total_out_pkts = 0;
	vc_info->in_bytes = vc_info->out_bytes = 0;
	vc_info->in_oam = vc_info->out_oam = 0;
	vc_info->broadcasts = 0;
    }
    atm->inpkts = atm->outpkts = 0;
    atm->infast = atm->outfast = 0;
    if (atm->out_ts_drop != UNSUPPORTED_COUNT) 
	atm->out_ts_drop = 0;

    for (ndx = 0; ndx < ATM_MAX_COUNTERS; ndx++) {
	atm_counters[ndx] = 0;
    }
}

/*
 * atm_setup_swidb
 *
 * Called to initialize the swidb fields.  The hwidb has already been
 * initialized.
 */
static void
atm_setup_swidb (idbtype *swidb, boolean init_hw)
{
    if (is_atm(swidb->hwptr)) {
	swidb->ip_mtu = swidb->sub_mtu;
	swidb->arp_timeout = ATMARP_CLIENTAGE;
	swidb->ip_split_disabled = swidb->ip_split_disabled_def = TRUE;
	swidb->ip_enctype = lec_enabled(swidb) ? ET_ARPA : ET_ATM;
	reg_invoke_media_enctype_setup(swidb, ET_ATM);
	swidb->rfc1577_flavor = RFC1577_EXT_NONE;
    }
}

/*
 * Section: Config/command parsing routines
 *	Routines to handle configuration of the ATM interface from user
 *      commands. Called to enable/disable the ATM interface, create PVCs,
 *      enable specific ATM options set max. VCs supported, etc...
 */

/*
 * atm_setup:	Called to setup the ATM interface.
 */
static boolean 
atm_setup (hwidbtype *hwidb)
{
    idbtype *swidb;
    atm_db_t *atm;

    /*
     * First setup the ATM interface struct.
     */
    swidb = hwidb->firstsw;
    atm = atm_get_db(hwidb);			/* Get a DB */
    if(!atm)
	return(FALSE);
    
    /*
     * Set the fastswitch flavor
     */
    hwidb->fast_switch_type = FS_AIP;

    /*
     * Do the remainder of the IDB
     */
    hwidb->enctype = ET_ATM;
    hwidb->vencap = atm_vencap;
    hwidb->bridge_vencap = (bridge_vencap_t)atm_bridge_vencap;
    hwidb->getlink = atm_getlink;
    hwidb->oqueue = holdq_enqueue;
    hwidb->oqueue_dequeue = holdq_dequeue;
    hwidb->iqueue = atm_input;
    hwidb->periodic = atm_periodic;
    hwidb->extract_hwaddr = invalidate_hwaddr;
    hwidb->extract_snpa = atm_extract_addr;
    hwidb->broadcast = atm_isbcast;
    hwidb->show_support = atm_interface;
    hwidb->atm_lineup = TRUE;		/* Always for ATM */
    hwidb->encsize = ATM_ENCAP_HDRBYTES;
    hwidb->parse_packet = atm_parse_packet;
    hwidb->max_proto_mtu = atm_max_proto_mtu;
    hwidb->clear_support = atm_clear_counter_support;
    holdq_reset(hwidb);

    /*
     * Set the max size
     */
    hwidb->max_pak_size = hwidb->max_pak_size_def = ATM_DEFAULT_MTU;

    GET_TIMESTAMP(atm->vcd_update);
    atm->bc_interval = ATM_MTP_INTERVAL_DEFAULT;   /* default for now */
    atm->out_ts_drop = UNSUPPORTED_COUNT;  /* default to unsupported
					      for now */
    
    /* probably just one swidb now, but we might as well be complete */
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	atm_setup_swidb(swidb, FALSE);
    }


    atm_oam_init();
    if (atm_arp_start_func != NULL) {
	(*atm_arp_start_func)();
    }
    return(TRUE);
}

/*
 * enqueue element if it doesn't exist
 * this will come in handy when dealing w/ SVC's
 * lock out interrupts..
 */
static void
media_atm_tbridge_swidb_vc_enqueue (idbtype *swidb, vc_info_t *vc)
{
    queuetype *p = &swidb->tbridge_circuitQ;
    vc_info_t *last_vc;
    leveltype status;

    if (vc == NULL)
	return;

    /*
     * only add aal5 snap vc's
     */
    if (ATM_VC_AAL5SNAP != (vc->flags & VC_FLAG_AAL_MASK))
	return;

    status = raise_interrupt_level(ALL_DISABLE);

    /*
     * Make sure the element isn't already queued or the last
     * element in this list
     */
    if ((vc->next_vc_in_bridge_group != NULL) ||
	(p->qtail == vc)) {
	reset_interrupt_level(status);
	return;
    }
    if (!p->qhead) {   /* q empty */
	p->qhead = vc;
	p->qtail = vc;
    }
    else {                                  	/* not empty */
	last_vc = (vc_info_t *) p->qtail;
	last_vc->next_vc_in_bridge_group = vc; /* update link */
    }

    p->qtail = vc;   /* tail points to new element */
    p->count++;
    vc->next_vc_in_bridge_group = NULL;

    reset_interrupt_level(status);
}

/*
 * to make interrupt level flooding easier, link all the VC's
 * associated w/ this VC into a linked list.
 *
 * CAUTION!!!  this only works w/ PVC's.  SVC's are a entirely
 * different matter, and will have to be worked in later...
 */
static void
media_atm_tbridge_swidb_vc_queue_init (idbtype *swidb)
{
    atm_db_t *atm = NULL;
    vc_info_t *vc;
    ushort index;

    if (swidb->tbridge_on_swidb == FALSE)
	return;

    queue_init(&swidb->tbridge_circuitQ, 0);

    atm = swidb->hwptr->atm_db;
    if (!atm)
	return;

    for (index = 1; index < atm->config.max_intf_vcs; index++) {
	vc = atm_getvc_all(atm, index);
	if (!vc || (vc->etype != ATM_ENCTYPE_SNAP) || vc->swidb != swidb)
	    continue;

	media_atm_tbridge_swidb_vc_enqueue(swidb, vc);
    }
}

/*
 * dequeue element if it exists
 * this will come in handy when dealing w/ SVC's
 * lock out interrupts..
 */
static void
media_atm_tbridge_swidb_vc_unqueue (idbtype *swidb, vc_info_t *vc)
{
    leveltype status;
    vc_info_t *p, *prev;
    queuetype *q = &swidb->tbridge_circuitQ;

    status = raise_interrupt_level(ALL_DISABLE);

    prev = p = (vc_info_t *) q->qhead;

    while (p) {
	if (p == vc) {
	    prev->next_vc_in_bridge_group =
		p->next_vc_in_bridge_group;

	    if (p == prev)
		q->qhead = p->next_vc_in_bridge_group;

	    if (!p->next_vc_in_bridge_group) {
		if (q->qhead == NULL)
		    prev = NULL;
		q->qtail = prev;
	    }

	    vc->next_vc_in_bridge_group = NULL;

	    if ((--q->count < 0) && (q->maximum)) {
		/*
		 * bad news...
		 */
		q->count = 0;
	    }

	    break;

	} else {
	    prev = p;
	    p = p->next_vc_in_bridge_group;
	}
    }
    reset_interrupt_level(status);
}

/*
 * display VC's in the bridge group
 */
static void
media_tbridge_atm_display_VCs (idbtype *idb)
{
    vc_info_t *vc;
    int i;

    vc = idb->tbridge_circuitQ.qhead;
    if (vc == NULL)
	return;

    i = 1;

    printf("\n      VC's:");

    while (vc) {
	if (!(i++ % 7))
	    printf("\n      ");
	printf("\t %u", vc->vc);
	vc = vc->next_vc_in_bridge_group;
    }
}

/*
 * clear out linked list...
 */
static void
media_atm_tbridge_swidb_vc_queue_clear (idbtype *swidb)
{
    atm_db_t *atm = NULL;
    vc_info_t *vc = NULL;
    queuetype *q;

    atm = swidb->hwptr->atm_db;
    if (!atm)
	return;

    q = &swidb->tbridge_circuitQ;
    vc = (vc_info_t *) q->qhead;
    while (vc) {
	media_atm_tbridge_swidb_vc_unqueue(swidb, vc);
	vc = (vc_info_t *) q->qhead;
    }
}

/*
 * atm_max_proto_mtu():
 *	Called during interface init. to set the maximum protocol MTU
 *	for each swidb on this interface.
 */
static int
atm_max_proto_mtu (idbtype *swidb,
		   enum LINK linktype,
		   boolean get_default)
{
    int value;
    value = reg_invoke_media_atm_mtu(linktype, swidb, get_default);

    return (value ? value: swidb->sub_mtu);
}

/*
 * atm_reset(): Called from the low level driver to indicate a device has
 *	been reset and to give the higher layers a chance to clean what
 *	should be cleaned up.
 */
static void
atm_reset (hwidbtype *hwidb)
{
    atm_db_t *atm;
    vc_info_t *vc;
    uint ndx, rateq;

    if (atm_event_debug)
	buginf("\nResetting %s", hwidb->hw_namestring);
    
    atm = hwidb->atm_db;
    for(ndx = 1; ndx < atm->config.max_intf_vcs; ndx++) {
	vc = atm_getvc(atm, ndx);
	if (!vc) {
	    continue;
	}
	if (!ISVCACTIVE(vc)) { 	/* Ignore INACTIVE VCCs */
	    continue;
	}
	/*
	 * A CD state change.  The ATM interface detects that CD has gone
	 * down, the interface is declared down.
	 *
	 * Basically, we teardown *ALL* SVCs.
	 * We keep the PVCs configured on the ATM interface.
	 */
	if (IS_ATMVC_SVC(vc)) {
	    /*
	     * Handle SVCs.  Can them....!
	     */
	    atm_hangup_vc(vc);
	} else {
	    /*
	     * Handle PVCs
	     * Remove them from the dynamic rate-queue and mark the INACTIVE.
	     * We'll reconfig them later, when the line comes back up.
	     */
	    if (vc->vcmode) {
    	    	rateq = (vc->vcmode >> ATM_VC_RATEQ_SHIFT) 
	      	    & (ATM_RATEQUEUE_MAX - 1);
    	    	atm->rateq[rateq].num_vc--;
		if (atm->rateq[rateq].num_vc == 0 && 
		    atm->rateq[rateq].state == RATE_QUEUE_DYNAMIC) {
		    atm->rateq[rateq].state = RATE_QUEUE_DISABLED;
		}

	    	vc->vcmode = 0;
	    }
	    
	    vc->flags &= ~VC_FLAG_ACTIVE;
	    atm->num_vc--;
	    atm->config_status = ATM_VCS_NOT_UP;
	    atm_oam_disable_timer(vc);
	    reg_invoke_atm_vc_deactivated(vc);
	}
    }
    GET_TIMESTAMP(atm->vcd_update);

    /*
     * Flush out all cached entries, if any
     */

    atm_flush_ar_cache(hwidb);
}

/*
 * atm_flush_protocol_cache
 *
 * called to clear out the protocol address resolution caches.  The problem is
 * that it may be called from interrupt routines, but the registry needs to
 * malloc() and free(), so we can't just call the registry.
 */
static void
atm_flush_ar_cache (hwidbtype *hwidb)
{
    idbtype *swidb;
    
    if (onintstack()) {
	/* we can't clear it now.  just flag process level */
	hwidb->atm_db->flags |= ATM_FLAG_FLUSH_CACHE;
	return;
    }
    
    /*
     *  If we are not running from an interrupt, just call the registry and
     *  bail.  Note that there is a critical region here which we are
     *  sidestepping: it is possible that we get an interrupt which will cause
     *  us to set the cache flush bit after we've cleared it, while we are
     *  processing the flushing, which will cause us to flush a second time.
     *  We can live with that.  The alternative is to disable interrupts around
     *  the cache clearing, which is overkill.  Moving the bit clear after the
     *  cache clear is dead wrong, for obvious reasons.
     */
    hwidb->atm_db->flags &= ~ATM_FLAG_FLUSH_CACHE;
    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	reg_invoke_ar_cache_clear(swidb);
    }
}

/*
 * 
 * atm_if_statechange_complete
 *              :called from shutdown interface 
 * OUTPUT/RETURN: Restore all PVCs or teardown all PVCs
 * INPUT: *swidb, *hwidb
 *
 */
static void atm_if_statechange_complete (idbtype *swidb,hwidbtype *hwidb)
{
  ulong ndx;
  atm_db_t *atm;
  vc_info_t *vc;
  atmSig_nsap_info *nsap_ptr;
  
  /* hw interface state change? */
  if (swidb == NULL && hwidb)
    return;
  
  /* should never happen but playing safe*/
  if (!swidb)
    return;

  hwidb = swidb->hwptr;
  /* again should never happen*/
  if (!hwidb)
    return;
  
  atm = hwidb->atm_db;

  /* should never happen */
  if (!is_atm(hwidb)) {
    return;
  }

  if (swidb->subif_state == SUBIF_STATE_UP) {

    /*
     * Restore all inactive PVCs 
     */
    
    /* go through all possible VC's and turn on all PVCs */
    for (ndx = 1; ndx < atm->config.max_intf_vcs; ndx++) {

      /* atm_getvc_all returns active and inactive vc's */
      vc = atm_getvc_all(atm, ndx);

      /* should never be an SVC */
      if (vc && !IS_ATMVC_SVC(vc)) {
      
	/* found a PVC on this interface
	 *  Send commands platform to re-enable this VC.
	 */
	if (vc->swidb == swidb) {

	  /* redundant check since all vc's should be inactive*/
	  if (!ISVCACTIVE(vc)) {
	      atm_activate_pvc(hwidb,vc);
	  }
	}
      }
    }
    /* tell signalling to resume listening on this nsap */
    /* 
     * If this is ESI generated NSAP, then the nsap_ptr should still
     * be in the list. Find it and reregister with ILMI. ILMI callback
     * will reregister with signalling.
     * Else, just reregister with signalling.
     */
    if (swidb->nsap) {

      /* search the nsap_info list */    
      nsap_ptr = atm->nsap_list_head;
      while (nsap_ptr) {
	if (nsap_ptr->idb == swidb) {
	  if (nsap_ptr->useIlmiPrefix) {
	    if (atmSmap_getIlmiPrefix(atm, nsap_ptr)) {
	      atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressValid);
	    }
	  }
	  return;
	}
	nsap_ptr = nsap_ptr->next;
      }

      atmSmap_registerAtmAddress(swidb);
    }
  }
 else {
   /* tear down all VCs while only letting PVCs be flagged as inactive
    * so a no shutdown command can restore them 
    */
  
   /* index through all possible vc's teardown all active pvc's */
   for (ndx = 1; ndx < atm->config.max_intf_vcs; ndx++) {

     /* atm_getvc returns all active vc's */
     vc = atm_getvc(atm, ndx);

     /* only teardown pvc's here */
     if (vc && !IS_ATMVC_SVC(vc)) {

       /* found a PVC on this interface 
	*  Send ATM interface commands to disable this VC.
	*/
       if (vc->swidb == swidb) {
	 if (ISVCACTIVE(vc)) {
	     atm_inactivate_pvc(hwidb,vc);
	     atm->num_vc--;
	 }
       }
     }
   }

   /* Point to note
        1. Nsap ptr is deleted when the sub-interface is removed.
        2. We retain the Nsap ptr for both static and ESI generated
           nsaps when the sub-interface is shutdown.
   */

   /* tell signalling to stop listening on this nsap */
   if (swidb->nsap) {
     /* 
      * If this is ESI generated NSAP, then need to keep the 
      * nsap_ptr. Find nsap_ptr in atm_db, clear out prefix, 
      * set flags, but keep ESI. Don't call cancelAtmAddress.
      * Deregister with ILMI if already registered.
      * Will get prefix and reregister with "no shut".
      */

     /* search the nsap_info list */    
     nsap_ptr = atm->nsap_list_head;
     while (nsap_ptr) {
       if (nsap_ptr->idb == swidb) {

         /* If this is an ESI generated address */
	 if (nsap_ptr->useIlmiPrefix) {
	   if (nsap_ptr->ilmiAddrRegistered) {
	     atmSmap_sendNsapToIlmi(nsap_ptr, reqAddressInValid);
	   }
	   nsap_ptr->ilmiPrefixSet = FALSE;
	   nsap_ptr->ilmiAddrRegistered = FALSE;
	   nsap_ptr->idb->atm_nsap_valid = FALSE;
           /* If this is an ESI generated address and the interface is
           * is being shutdown then we need not have to delete the
           * address. Will retain the ESI portions alone and then
           * reregister when the interface is active again.
           */
          if (swidb->subif_state != SUBIF_STATE_DELETED) {
	     bzero(nsap_ptr->idb->nsap->addr, ILMI_PREFIX_SIZE);
	     bzero(&nsap_ptr->nsap.addr, ILMI_PREFIX_SIZE);
          }
         }
          /* In any case if the interface is being deleted lets cancel the
          * address from the signaling driver.
          */
          if (swidb->subif_state == SUBIF_STATE_DELETED) {
           /* If the interface is deleted go ahead and cancel the
            * NSAP address from the signalling driver.Release the
            * nsap memory. */
            atmSmap_cancelAtmAddress(swidb);
            free (swidb->nsap);
            swidb->nsap = NULL;
          }
          break;
       }
       nsap_ptr = nsap_ptr->next;
     }
   }
   
  /* remove all svc's associated with this subinterface */
  atmSig_subinterfaceReset(swidb);
 }

}

/*
 * atm_deleteidb: Called from 'no int atm ?/0.x' to delete a
 *	subinterface.
 *	After tearing down all PVCs on this subinterface, we bump the
 *	cache version nos. & flush the ARP table(s) to clean up.
 * OUTPUT/RETURN:
 *	Removes all PVCs associated with this IDB.
 * INPUT:
 *	*idb -> ptr. to swidb in question.
 */
static void
atm_deleteidb (idbtype *idb) 
{
    hwidbtype *hwidb;
    atm_db_t *atm;
    ushort ndx;
    boolean bump_cache = FALSE;
    vc_info_t *vc;

    hwidb = idb->hwptr;
    if (!is_atm(hwidb)) {
	return;
    }
    atm = hwidb->atm_db;

    for(ndx = 1; ndx < atm->config.max_intf_vcs; ndx++) {
	vc = atm_getvc_all(atm, ndx);
	if (vc && !IS_ATMVC_SVC(vc)) {
	    if (vc->swidb == idb) {
	    /*
	     * This VC is one to remove.  Tear it down!
	     */
		atm_remove_vc(hwidb, ndx);
		bump_cache = TRUE;
	    }
	}
    }

    /* remove all svc's associated with this subinterface */
    atmSig_subinterfaceReset(idb);

    /*
     * For AAL3/4, we need to clean up the E.164 stuff
     */
    atm_delete_multicast(idb);
    if(idb->vaddr) {
	free(idb->vaddr);
	idb->vaddr = NULL;
    }

    if (bump_cache) {	/* Only if we really have to! */
	reg_invoke_bump_cache_version(hwidb, NULL, TRUE);
    	reg_invoke_ar_cache_clear(idb);    
    }
    /*
     * Free resources used by WAVL tree.
     */
    if (idb->atm_glommable_vc_tree != NULL) {
	wavl_finish(idb->atm_glommable_vc_tree);
	free(idb->atm_glommable_vc_tree);    
	idb->atm_glommable_vc_tree = NULL;
    }
}

/*
 * atm_init:	Called to init locals & register
 *	Sets up hash tables, etc...  Registers ET_ATM encapsulation.
 * OUTPUT/RETURN:
 *	None.
 * INPUT:
 *	None.
 *
 */
static void
atm_init (subsystype *subsys)
{
    ushort ndx;

    atm_debug_init();

    atm_parser_init();
    reg_add_media_atm_setup(atm_setup, "atm_setup");
    reg_add_sw_idb_init(atm_setup_swidb, "atm_setup_swidb");
    
    reg_add_media_tbridge_swidb_vc_queue_init(ET_ATM,
					      media_atm_tbridge_swidb_vc_queue_init,
					      "media_atm_tbridge_swidb_vc_queue_init");
    reg_add_media_tbridge_swidb_vc_queue_clear(ET_ATM,
					       media_atm_tbridge_swidb_vc_queue_clear,
					       "media_atm_tbridge_swidb_vc_queue_clear");
    reg_add_media_tbridge_swidb_vc_enqueue(ET_ATM,
					   media_atm_tbridge_swidb_vc_enqueue,
					   "media_atm_tbridge_swidb_vc_enqueue");
    reg_add_media_tbridge_swidb_vc_unqueue(ET_ATM,
					   media_atm_tbridge_swidb_vc_unqueue,
					   "media_atm_tbridge_swidb_vc_unqueue");
    reg_add_media_tbridge_display_VCs(ET_ATM, media_tbridge_atm_display_VCs,
				      "media_tbridge_atm_display_VCs");
				      
    reg_add_staticmap_nvgen(ET_ATM, atm_map_nvgen, "atm_map_nvgen");
    reg_add_mapclass_nvgen(ET_ATM, atm_mapclass_nvgen, "atm_mapclass_nvgen");
    reg_add_is_map_broadcast(ET_ATM, is_atm_map_bcast, "is_atm_map_bcast");
    reg_add_media_hw_reset(atm_reset, "atm_reset");
    reg_add_swif_erase(atm_deleteidb, "atm_deleteidb");
    reg_add_encaps_show_protocol_attr(ET_ATM,
				      atm_encaps_show_protocol_attr,
				      "atm_encaps_show_protocol_attr");
    reg_add_if_statechange_complete(atm_if_statechange_complete,"atm_if_statechange_complete");
    reg_add_arp_getphysaddr(atm_getvcaddr, "atm_getvcaddr");

   /*
    * init ATM static maps
    */
    atmSmap_Init();

    /*
     * Nuke the ATM descriptor blocks
     */
    for(ndx = 0; ndx < MAX_ATM_INTFC; ndx++)
      atm_db[ndx].flags = 0;		/* Clear the IN-USE flags */
    /*
     * create the ATM registry.  This will set up the stubs.
     */
    atm_regcode = registry_create(REG_UNKNOWN, SERVICE_ATM_MAX, "ATM");
    if (atm_regcode != REG_ILLEGAL)
      create_registry_atm();
}



/*
 * atm_calc_vci_bits: called during platform configuration or
 * when the vc-per-vp command is requested
 * Returns the number of bits required to align with the vc-per-vp
 * value.
 */
ushort
atm_calc_vci_bits (ushort vc_per_vp)
{
    ushort max_vci_bits = 0;
    ushort i;

    for (i = vc_per_vp;i != 1;i = i >> 1) {
	max_vci_bits++;
    }
    return (max_vci_bits);
}

/*
 * atm_calc_max_vpivci: called whenever the max vpi or vci allowed is
 * needed for a validity check such as when the vc-per-vp
 * commands is invoked.
 * Returns the maximum vpi allowed by the number of bits input.
 */
ushort
atm_calc_max_vpivci (ushort max_bits)
{
    ushort max_vpivci;
    
    max_vpivci = (1 << max_bits);
    max_vpivci -= 1;

    return (max_vpivci);
}


/*
 * atm_calc_vpi_bits: called during platform configuration 
 * Returns the vpi bits required to align with the total vp/vc bits
 * and the vci bits input   
 */
ushort
atm_calc_vpi_bits (ushort total_vpi_vci_bits,ushort max_vci_bits)
{
    ushort max_vpi_bits;

    max_vpi_bits = total_vpi_vci_bits - max_vci_bits;

    return (ATM_MAX_VPI_BITS < max_vpi_bits ? ATM_MAX_VPI_BITS : max_vpi_bits);

}
		   

inline void 
atm_newconfig (atm_db_t *atm) 
{
    atm->config_status  = ATM_CONFIG_CHANGED;
    if (atm_event_debug) {
	buginf("\natm_newconfig(%s): ATM config. status"
	       " set to ATM_CONFIG_CHANGED",
	       atm->hwidb->hw_namestring);
    }
    atm->configs_pending++;  
    GET_TIMESTAMP(atm->config_time);
}

/*
 * atm_aal34_remove_ok:  A simple check function.  Determines if it's
 *	save to 'no atm aal aal3/4' on the interface.
 *
 *	FALSE: Means the user still has AAL3/4 specific config.s on the
 *		interface, or subinterfaces to remove.
 *	TRUE: Means that it's save to disable AAL3/4 processing on the
 *		interface.
 */
static inline boolean
atm_aal34_remove_ok (hwidbtype *hwidb)
{
    idbtype *swidb;

    FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {
	if (atm_get_aal34vcnum(swidb)) {
	    return(FALSE);
	}
	if (swidb->vaddr) {
	    return(FALSE);
	}
	if (atm_get_multicast(swidb)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * atm_inarp_on_aal: called from atm pvc command to check whether
 *                   inarp is supported on the aal encapsulation.
 *    
 */
boolean
atm_inarp_on_aal (uint pvc_encap)
{
    switch (pvc_encap) {
    case ATM_VC_AAL34SMDS:
    case ATM_VC_AALQSAAL:
    case ATM_VC_AAL5ILMI:  
    case ATM_VC_AAL5MUX:
    case ATM_VC_AAL5NLPID:
    case ATM_VC_AAL5FRNLPID:
    case ATM_VC_AAL5LANE:
        return(FALSE);

    case ATM_VC_AAL5SNAP:
        return(TRUE);

    default: 
        return(FALSE);
    }
}

/*
 * atm_supported_protocol
 * Verify that given protocol is supported
 */
static boolean 
atm_supported_protocol (uint protocol)
{
    return(
	(protocol == LINK_IP) ||
	(protocol == LINK_NOVELL) ||
	(protocol == LINK_APPLETALK) ||
	(protocol == LINK_VINES) ||
	(protocol == LINK_DECNET) ||
	(protocol == LINK_CLNS) ||
	(protocol == LINK_XNS) ||
	(protocol == LINK_PAD) ||
	(protocol == LINK_BRIDGE) ||
	(protocol == LINK_CDP) ||
	(protocol == LINK_APOLLO));
}

/*
 * atm_map_command
 * Parse a static map command
 */
void
atm_map_command (parseinfo *csb)
{
    staticmap_type *map_ptr;
    maplist_type *list_ptr;
    atm_params_type *params;
    addrtype addr;
    hwaddrtype hwconf, *hwconf_ptr;
    uint protocol;
    vcd_t vcnum;
    boolean new_map;

    list_ptr = csb->maplist;
    protocol = GETOBJ(int,1);
    vcnum = GETOBJ(int,3);
    if (!atm_supported_protocol(protocol)) {
	printf("\n%% Protocol not supported - %s", print_linktype(protocol));
	return;
    }

    memset(&addr, 0, sizeof(addrtype));
    if(!parse_proto_addr(csb, NONULL, &addr, GETOBJ(paddr,1), protocol)){
        return;
    }

    map_ptr = staticmap_lookup_list(list_ptr, &addr, protocol, ET_ATM);
    if (csb->sense) {
        /*
         * First update parameters given on the command line
         */
        if (!map_ptr) {
            params = malloc(sizeof(atm_params_type));
            if (!params) {
                printf(nomemory);
                return;
            }
            atm_params_init(params);
            new_map = TRUE;
        } else {
            params = (atm_params_type *)map_ptr->params;
            new_map = FALSE;
        }
        params->broadcast = GETOBJ(int,4);
        params->aal5mux   = GETOBJ(int,5);

	hwconf_ptr = &hwconf;
        memset(hwconf_ptr, 0, sizeof(hwaddrtype));
        if (GETOBJ(int,2) == ATM_MAP_VC) {
            hwconf_ptr->type = STATION_ATMVC;
            hwconf_ptr->length = STATIONLEN_ATMVC;
	    hwconf_ptr->target_type = TARGET_UNICAST;
            bcopy(&vcnum, hwconf_ptr->addr, STATIONLEN_ATMVC);
        } else {
	    bcopy(GETOBJ(hwaddr,1), hwconf_ptr, sizeof(hwaddrtype));
        }
        if (new_map) {
            atm_staticmap_add(list_ptr, NULL, &hwconf, &addr, protocol,
                                                GETOBJ(string,2), params);
        } else {
	    /*
	     * Replace existing entry & rehash as necessary
	     * Update CLASS info. if needed
	     */
            staticmap_update(list_ptr, map_ptr, hwconf_ptr, GETOBJ(string,2));
	}
    } else {
        if (map_ptr)
            staticmap_delete(list_ptr, map_ptr);
        else
            printf("\n%% Cannot find address map");
    }
}

/*
 * atm_htoa
 */
static uchar
atm_htoa (char c)
{
   if ( (c >= 0) && (c <= 9) )
      return (c + '0');
   else
      return (c - 10 + 'A');
}



/*
 * atm_printnsap
 * Return a printable ATM NSAP with '.' from a 20-octet character NSAP
 */
void atm_printnsap (uchar *innsap, uchar *outnsap)
{
   uint i, j, k;

   if (!innsap) return;
   i = j = 0;

   /* print AFI */
   outnsap [j] = atm_htoa ((innsap [i] & 0xf0) >> 4);
   outnsap [++j] = atm_htoa (innsap[i] & 0x0f);
   outnsap [++j] = '.';

   /* rest of the prefix */
   for (k=0; k<12; ++k)
   { 
      outnsap [++j] = atm_htoa ((innsap [++i] &0xf0) >> 4);
      outnsap [++j] = atm_htoa (innsap[i] & 0x0f);
   } 
   outnsap [++j] = '.';

   /* the esi */
   for (k=0; k<6; ++k)
   {
      outnsap [++j] = atm_htoa ((innsap [++i] &0xf0) >> 4);
      outnsap [++j] = atm_htoa (innsap [i] & 0x0f);
   }
   outnsap[++j] = '.';

   /* the selector */
   outnsap [++j] = atm_htoa ((innsap [++i] & 0xf0) >> 4);
   outnsap [++j] = atm_htoa (innsap[i] & 0x0f); 

   /* NULL terminate */
   outnsap [++j] = '\0';
}

/*
 * atm_printprefix
 * Return a printable ATM PREFIX with '.' from a 13-octet character Prefix
 */
void atm_printprefix (uchar *innsap, uchar *outnsap)
{
   uint i, j, k;

   if (!innsap) return;
   i = j = 0;

   /* print AFI */
   outnsap [j] = atm_htoa ((innsap [i] & 0xf0) >> 4);
   outnsap [++j] = atm_htoa (innsap[i] & 0x0f);
   outnsap [++j] = '.';

   /* rest of the prefix */
   for (k=0; k< (ILMI_PREFIX_SIZE - 1); ++k)
   { 
      outnsap [++j] = atm_htoa ((innsap [++i] &0xf0) >> 4);
      outnsap [++j] = atm_htoa (innsap[i] & 0x0f);
   } 

   /* NULL terminate */
   outnsap [++j] = '\0';
}

/*
 * atm_printesi
 * Return a printable ATM ESI with '.' from a 7-octet character ESI
 */
void atm_printesi (uchar *inesi, uchar *outesi)
{
   uint i, j, k;

   if (!inesi) return;
   i = j = 0;

   /* the esi */
   for (k=0; k< (ILMI_ESI_SIZE - 1); ++k)
   {
      outesi [j++] = atm_htoa ((inesi [i] &0xf0) >> 4);
      outesi [j++] = atm_htoa (inesi [i++] & 0x0f);
   }
   outesi[j++] = '.';

   /* the selector */
   outesi [j++] = atm_htoa ((inesi [i] & 0xf0) >> 4);
   outesi [j++] = atm_htoa (inesi[i] & 0x0f); 

   /* NULL terminate */
   outesi [j] = '\0';
}

/*
 * atm_map_nvgen
 */
static void
atm_map_nvgen (staticmap_type *map_ptr)
{
    char addrstr[64];
    char nsapstr[MAX_ATM_NSAP_ASCII_LEN];

    print_proto_addr(NULL, &map_ptr->addr, map_ptr->protocol, addrstr);
    nv_write(TRUE, "%s", addrstr);
    switch(map_ptr->hwconf.type) {
    case STATION_ATMVC:
        nv_add(TRUE, " atm-vc %d", *(ushort *)map_ptr->hwconf.addr);
	break;
    case STATION_ATMNSAP:
	atm_printnsap(map_ptr->hwconf.addr, nsapstr);
        nv_add(TRUE, " atm-nsap %s", nsapstr);
	break;
    case STATION_SMDS64:
	nv_add(TRUE, " smds %E", map_ptr->hwconf.addr);
	break;
    default:				/* Should never happen -SHB 7/13/94 */
	break;
    }
    if (map_ptr->class)
        nv_add(TRUE, " class %s", map_ptr->class->name);
    /* broadcast keyword must be written after aal5mux keyword */
    nv_add(((atm_params_type *)map_ptr->params)->aal5mux, " aal5mux");
    nv_add(((atm_params_type *)map_ptr->params)->broadcast, " broadcast");
}

/*
 * atm_mapclass_command
 * Parse ATM mapclass commands
 */
void
atm_mapclass_command (parseinfo *csb)
{
    mapclass_type *class_ptr = csb->mapclass;
    atm_params_type *params = class_ptr->params;

    if (class_ptr->encap != ET_ATM) {
        printf("\n%% Cannot configure atm parameters in non atm map class");
        return;
    }

    /*
     * if no parameters configured yet, initialiaze parameter list
     */
    if (!params) {
        params = malloc(sizeof(atm_params_type));
        if (!params) {
            printf(nomemory);
            return;
        }
        atm_params_init(params);
        class_ptr->params = params;
    }
    switch (csb->which) {
      case ATM_MAP_BROAD:
        params->broadcast = csb->sense;
        break;
      case ATM_MAP_FWPK0:
        params->fwpkrate0 = csb->sense ? GETOBJ(int,1) : ATM_FWPKRATE0;
        break;
      case ATM_MAP_BKPK0:
        params->bkpkrate0 = csb->sense ? GETOBJ(int,1) : ATM_BKPKRATE0;
        break;
      case ATM_MAP_FWPK1:
        params->fwpkrate1 = csb->sense ? GETOBJ(int,1) : ATM_FWPKRATE1;
        break;
      case ATM_MAP_BKPK1:
        params->bkpkrate1 = csb->sense ? GETOBJ(int,1) : ATM_BKPKRATE1;
        break;
      case ATM_MAP_FWST0:
        params->fwstrate0 = csb->sense ? GETOBJ(int,1) : ATM_FWSTRATE0;
        break;
      case ATM_MAP_BKST0:
        params->bkstrate0 = csb->sense ? GETOBJ(int,1) : ATM_BKSTRATE0;
        break;
      case ATM_MAP_FWST1:
        params->fwstrate1 = csb->sense ? GETOBJ(int,1) : ATM_FWSTRATE1;
        break;
      case ATM_MAP_BKST1:
        params->bkstrate1 = csb->sense ? GETOBJ(int,1) : ATM_BKSTRATE1;
        break;
      case ATM_MAP_FWMX0:
        params->fwbsize0 = csb->sense ? GETOBJ(int,1) : ATM_FWBSIZE0;
        break;
      case ATM_MAP_BKMX0:
        params->bkbsize0 = csb->sense ? GETOBJ(int,1) : ATM_BKBSIZE0;
        break;
      case ATM_MAP_FWMX1:
        params->fwbsize1 = csb->sense ? GETOBJ(int,1) : ATM_FWBSIZE1;
        break;
      case ATM_MAP_BKMX1:
        params->bkbsize1 = csb->sense ? GETOBJ(int,1) : ATM_BKBSIZE1;
        break;
      case ATM_MAP_TRFMGT:
        params->trafmgmt = csb->sense ? GETOBJ(int,1) : ATM_TRAFMGMT;
        break;
      case ATM_MAP_AAL5MUX:
        params->aal5mux = csb->sense;
        break;
      case ATM_MAP_LAYER2ID:
        params->layer2id = csb->sense ? GETOBJ(int,1) : ATM_BLLIE;
        break;
      default:
        bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * atm_mapclass_nvgen
 * Does map-class nvgen
 */
static void
atm_mapclass_nvgen (mapclass_type *class_ptr)
{
    atm_params_type *params;

    /* if no class parms are specified (just atm map-class), */
    /* class_ptr->params is null. */
    if ( (!class_ptr) || (!(class_ptr->params)) )
        return;

    params = class_ptr->params;

    nv_write(params && params->broadcast, "atm broadcast");
    nv_write(params && params->aal5mux, "atm aal5mux");
    nv_write(params && (params->fwpkrate0 != ATM_FWPKRATE0), "%s %d",
                "atm forward-peak-cell-rate-clp0", params->fwpkrate0);
    nv_write(params && (params->bkpkrate0 != ATM_BKPKRATE0), "%s %d",
                "atm backward-peak-cell-rate-clp0", params->bkpkrate0);
    nv_write(params && (params->fwpkrate1 != ATM_FWPKRATE1), "%s %d",
                "atm forward-peak-cell-rate-clp1", params->fwpkrate1);
    nv_write(params && (params->bkpkrate1 != ATM_BKPKRATE1), "%s %d",
                "atm backward-peak-cell-rate-clp1", params->bkpkrate1);
    nv_write(params && (params->fwstrate0 != ATM_FWSTRATE0), "%s %d",
                "atm forward-sustainable-cell-rate-clp0", params->fwstrate0);
    nv_write(params && (params->bkstrate0 != ATM_BKSTRATE0), "%s %d",
                "atm backward-sustainable-cell-rate-clp0", params->bkstrate0);
    nv_write(params && (params->fwstrate1 != ATM_FWSTRATE1), "%s %d",
                "atm forward-sustainable-cell-rate-clp1", params->fwstrate1);
    nv_write(params && (params->bkstrate1 != ATM_BKSTRATE1), "%s %d",
                "atm backward-sustainable-cell-rate-clp1", params->bkstrate1);
    nv_write(params && (params->fwbsize0 != ATM_FWBSIZE0), "%s %d",
                "atm forward-max-burst-size-clp0", params->fwbsize0);
    nv_write(params && (params->bkbsize0 != ATM_BKBSIZE0), "%s %d",
                "atm backward-max-burst-size-clp0", params->bkbsize0);
    nv_write(params && (params->fwbsize1 != ATM_FWBSIZE1), "%s %d",
                "atm forward-max-burst-size-clp1", params->fwbsize1);
    nv_write(params && (params->bkbsize1 != ATM_BKBSIZE1), "%s %d",
                "atm backward-max-burst-size-clp1", params->bkbsize1);
    nv_write(params && (params->trafmgmt != ATM_TRAFMGMT), "%s %d",
                "atm traffic-mgmt-options", params->trafmgmt);
    nv_write(params && (params->layer2id != ATM_BLLIE), "%s %x",
                "atm layer2-protocol-id", params->layer2id);
}

/*
 * atm_interface: Called from the 'Show int' command to display ATM 
 *	specific information.
 */
static void
atm_interface (hwidbtype *hwidb)
{
    atm_db_t *atm;
    vc_info_t *vc = NULL;

    atm = hwidb->atm_db;
    if (!atm) {
	printf("\n%%ATM: (%s) not configured for ATM", hwidb->hw_namestring);
	return;
    }
    printf("\n  Encapsulation(s):");
    if(atm->config.mode & ATM_CFG_MODE_AAL5)
	printf(" AAL5");
    if(atm->config.mode & ATM_CFG_MODE_AAL34)
	printf(" AAL3/4");
    if(atm->flags & ATM_FLAG_PVC)
	printf(", PVC mode");
    if (hwidb->type == IDBTYPE_AIP)
	printf("\n  %u TX buffers, %u RX buffers,",
	    atm->config.tx_buff, atm->config.rx_buff);
    printf ("\n  %u maximum active VCs, %u VCs per VP, %u current VCCs",
	atm->config.max_intf_vcs, atm->config.rt_vc_per_vp, atm->num_vc);

    printf("\n  VC idle disconnect time: ");
    if (atm->idle_time == 0)
	printf("never");
    else
	printf("%u second%s",
	       atm->idle_time / ONESEC, atm->idle_time > 1 ? "s" : "");

    /* signalling details */
    if (atm->sig_vcnum) {
	/* need to call atm_getvc_all to get inactive vc for shutdown case */
	vc = atm_getvc_all(atm, atm->sig_vcnum);
	/* should never happen*/
	if (vc != NULL) {
	    printf("\n  Signalling vc = %u, vpi = %u, vci = %u",
		  atm->sig_vcnum,vc->vpi, vc->vci);
	    printf("\n  UNI Version = %s, Link Side = %s",
		   (atm->uni_version == ATMUNI_VERSION_30) ? "3.0":"3.1",
		   (atm->uni_side == ATMSIG_SIDE_USER) ? "user":"network");
	}
    }
/*
    printf("\n  %u Input, %u Output, %u InFast, %u OutFast",
	atm->inpkts, atm->outpkts, atm->infast, atm->outfast);
*/
}

/*
 * atm_framing_string: returns a string with the ATM framing mode
 */
char *
atm_framing_string (hwidbtype *hwidb)
{
    switch(hwidb->atm_db->plimtype) {
    case ATM_PLIM_INVALID:
        return("INVALID");
        break;
    case ATM_PLIM_DS3:
        switch(hwidb->atm_db->ds3_framing) {
        case ATM_FRAMING_DS3CBITADM:
            return("C-bit ADM");
            break;
        case ATM_FRAMING_DS3CBITPLCP:
            return("C-bit PLCP");
            break;
        case ATM_FRAMING_DS3M23ADM:
            return("M23 ADM");
            break;
        case ATM_FRAMING_DS3M23PLCP:
            return("M23 PLCP");
        }
        break;
    case ATM_PLIM_E3:
        switch(atm_db->e3_framing) {
        case ATM_FRAMING_E3G751PLCP:
            return("G.751 PLCP");
            break;
        case ATM_FRAMING_E3G751ADM:
            return("G.751 ADM");
            break;
        case ATM_FRAMING_E3G832ADM:
            return("G.832/G.804");
	}
        break;
    }

    return("Unknown");
}



/*
 * ATM subsystem header
 */

#define ATM_MAJVERSION 1
#define ATM_MINVERSION 0
#define ATM_EDITVERSION  1

/*
 * Changed this registration to KERNEL from PROTOCOL to ensure it gets
 * initialized for use by the ATM driver (if present).
 */
SUBSYS_HEADER(atm, ATM_MAJVERSION, ATM_MINVERSION, ATM_EDITVERSION,
	      atm_init, SUBSYS_CLASS_KERNEL,
	      NULL,
	      NULL);
