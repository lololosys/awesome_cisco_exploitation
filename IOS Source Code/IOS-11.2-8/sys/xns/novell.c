/* $Id: novell.c,v 3.51.4.71 1996/09/15 05:35:32 john Exp $
 * $Source: /release/112/cvs/Xsys/xns/novell.c,v $
 *------------------------------------------------------------------
 * novell.c -- Novell/IPX protocol suite.
 *
 *     FOR GENERAL DOCUMENTATION, SEE THE BIG COMMENT SECTION AT THE END
 *     OF THE RCS/CVS LOG!
 *
 * Spring 1989, By Bill Westfield
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This module contains RIP/SAP/Echo input packet processing, RIP/SAP/Echo
 * packet output, and all SAP maintenance functions.
 *------------------------------------------------------------------
 * $Log: novell.c,v $
 * Revision 3.51.4.71  1996/09/15  05:35:32  john
 * CSCdi68150:  Watchdog timeout in novspx_ager
 * Branch: California_branch
 *
 * Revision 3.51.4.70  1996/09/15  02:48:05  dko
 * CSCdi61575:  wrong value of hop count for connected serial i/f somtimes
 * Branch: California_branch
 * Replaced last_delay with last_hopcount in bringup state.
 *
 * Revision 3.51.4.69  1996/09/13  20:29:28  sseal
 * CSCdi65771:  ipx show commands incorrectly display encap
 * Branch: California_branch
 *
 * Revision 3.51.4.68  1996/09/11  17:39:31  snyder
 * CSCdi68797:  char pointers to char arrays and make const
 *              120 data, 152 image
 * Branch: California_branch
 *
 * Revision 3.51.4.67  1996/09/09  15:38:01  mschaef
 * CSCdi68274:  IPX SAP table may be incomplete when NLSP in use
 * Branch: California_branch
 *
 * Revision 3.51.4.66  1996/09/05  22:26:26  john
 * CSCdi67901:  SPX ager timer might not be triggered
 * Branch: California_branch
 *
 * Revision 3.51.4.65  1996/09/04  23:49:31  sluong
 * CSCdi67655:  IPX network deletion freezes
 * Branch: California_branch
 *
 * Revision 3.51.4.64  1996/08/28  13:24:43  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.51.4.63  1996/08/28  00:52:16  hampton
 * Migrate Desktop Protocols away from the one minute and one second
 * registries.  [CSCdi67383]
 * Branch: California_branch
 *
 * Revision 3.51.4.62  1996/08/27  20:24:50  sluong
 * CSCdi66169:  IPX-EIGRP can eventually crash router when there is a
 * flapping link.
 * Branch: California_branch
 *
 * Revision 3.51.4.61  1996/08/22  20:56:57  sluong
 * CSCdi66719:  IPX-EIGRP: Cannot remove SAPs learned from wrong interface
 * Branch: California_branch
 *
 * Revision 3.51.4.60  1996/08/21  20:38:15  hampton
 * Further isolate VLAN code from IPX internals.  [CSCdi66726]
 * Branch: California_branch
 *
 * Revision 3.51.4.59  1996/08/14  23:03:21  sluong
 * CSCdi64783:  ipx routing failed in fr inverse arp test
 * Branch: California_branch
 *
 * Revision 3.51.4.58  1996/08/14  18:09:25  john
 * CSCdi64010:  IPX SPX spoofing stops when spoof table gets large -
 *              need to age out
 * Branch: California_branch
 *
 * Revision 3.51.4.57  1996/08/14  02:37:06  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.51.4.56  1996/08/10  00:19:15  mschaef
 * CSCdi65656:  CPUHOG Process = IPX SAP Out
 * Branch: California_branch
 *
 * Revision 3.51.4.55  1996/08/09  06:06:04  sluong
 * CSCdi65345:  Directly connected IPX network lost from EIGRP topology
 * table
 * Branch: California_branch
 *
 * Revision 3.51.4.54  1996/08/08  05:18:48  mschaef
 * CSCdi31506:  NLSP Ager process remains when NLSP is disabled
 * Branch: California_branch
 *
 * Also:
 *
 *   - Convert to use new scheduler primitives
 *   - Remove cruft including unused variables/params
 *
 * Revision 3.51.4.53  1996/08/07  09:05:14  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.51.4.52  1996/08/06  22:24:49  hampton
 * Fix IPX encapsulation problems.  [CSCdi65213]
 * Branch: California_branch
 *
 * Revision 3.51.4.51  1996/08/03  23:44:10  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.51.4.50  1996/08/01  06:05:10  hampton
 * Put back accidentally removed check for HDLC interfaces.  [CSCdi64821]
 * Branch: California_branch
 *
 * Revision 3.51.4.49  1996/07/30  21:59:55  jserpa
 * CSCdi63900:  IPX does not accept  SAP type 0 advertisements
 * Branch: California_branch
 * Corrected to accept SAP type 0 advertisements, as well as get nearest
 * server requests.
 *
 * Revision 3.51.4.48  1996/07/26  00:08:50  hampton
 * Various minor IPX cleanups.  [CSCdi64087]
 * Branch: California_branch
 *
 * Revision 3.51.4.47  1996/07/25  22:27:56  jserpa
 * CSCdi58345:  SAP poison sent counter stays at zero
 * Branch: California_branch
 * Removed displaying unused counters.
 *
 * Revision 3.51.4.46  1996/07/23  13:33:03  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.51.4.45  1996/07/20  00:23:57  sluong
 * CSCdi63576:  Interface changed indicator for SAP never goes away
 * Branch: California_branch
 *
 * Revision 3.51.4.44  1996/07/09  23:38:56  amcrae
 * CSCdi60939:  show ipx interface has incorrect protocol state
 * Branch: California_branch
 *
 * Revision 3.51.4.43  1996/07/08  16:31:27  boyle
 * CSCdi47813:  Turning off&on RIP doesnot redistribute rip into ipx-EIGRP
 * cloud
 * Branch: California_branch
 *
 * Revision 3.51.4.42  1996/07/07  19:33:08  mschaef
 * CSCdi61877:  IPX Net Mgr process has memory leak
 * Branch: California_branch
 *
 * Revision 3.51.4.41  1996/07/04  20:49:12  john
 * CSCdi61993:  Need to remove Multicast check for IPXWAN links
 * Branch: California_branch
 *
 * Revision 3.51.4.40  1996/06/28  23:33:31  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.51.4.39  1996/06/27  01:10:17  mschaef
 * CSCdi61334:  Alignment error, building IPX route cache entry
 * Branch: California_branch
 *
 * Revision 3.51.4.38  1996/06/24  21:52:59  sluong
 * CSCdi61173:  NLSP does not kick in pessimal SAPs
 * Branch: California_branch
 *
 * Revision 3.51.4.37  1996/06/21  23:30:09  mschaef
 * CSCdi37519:  CLI command sh ipx int brief doesnt reflect
 *              orrect encap enabled
 * Branch: California_branch
 *
 * Revision 3.51.4.36  1996/06/21  22:50:15  sluong
 * CSCdi61045:  Hop count not incremented while sending the sap updates
 * Branch: California_branch
 *
 * Revision 3.51.4.35  1996/06/21  18:41:57  akr
 * CSCdi59116:  Router caches greater than 4 equal cost sap entries in sh
 * ipx sap d
 * Branch: California_branch
 *
 * Revision 3.51.4.34  1996/06/19  22:40:14  john
 * CSCdi60499:  Non connected nearby Services show as connected
 * Branch: California_branch
 *
 * Revision 3.51.4.33  1996/06/18  17:39:59  akr
 * CSCdi56186:  %SYS-3-INTPRINT: Illegal printing attempt from interrupt
 * level
 * Branch: California_branch
 * Changed unsolicited printf to errmsg.
 *
 * Revision 3.51.4.32  1996/06/17  23:40:01  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.51.4.31  1996/06/12  17:37:15  akr
 * CSCdi60228:  ipx route-cache can grow without limit
 * Branch: California_branch
 *
 * Revision 3.51.4.30  1996/06/10  19:24:44  kleung
 * CSCdi57965:  Misc. cosmetic command bugs.
 * Branch: California_branch
 *
 * Revision 3.51.4.29  1996/06/05  14:56:30  john
 * CSCdi58363:  ipx rip updates are not sent when nlsp is disabled
 * Branch: California_branch
 *
 * Revision 3.51.4.28  1996/06/01  01:33:17  hampton
 * Remove unnecessary cross module references.  [CSCdi59221]
 * Branch: California_branch
 *
 * Revision 3.51.4.27  1996/05/30  23:48:53  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.51.4.26  1996/05/30  00:36:35  widmer
 * CSCdi58426:  Invalid range for ipx gns-response-delay command
 * Branch: California_branch
 * Fix parser ranges to fit variable size
 * Change %d to %u for nv generation of uint's
 *
 * Revision 3.51.4.25  1996/05/29  00:48:40  akr
 * CSCdi58823:  enabling ipx route-cache cbus causes crash
 * Branch: California_branch
 *
 * Revision 3.51.4.24  1996/05/29  00:42:08  akr
 * CSCdi58840:  ipx fast-switching can be enabled with IPX/SPX spoofing
 * Branch: California_branch
 * Added check for spoofing, before enabling fast-switching.
 *
 * Revision 3.51.4.22  1996/05/21  10:11:32  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.51.4.21  1996/05/17  12:17:56  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.42.2.4  1996/05/06  00:12:08  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.51.4.20  1996/05/13  17:55:11  john
 * CSCdi35609:  IPX ping to local low end ethernet does not work
 *              make IPX ping to local interface do what IP does.
 * Branch: California_branch
 *
 * Revision 3.51.4.19  1996/05/09  14:49:48  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.51.4.18  1996/05/08  05:22:22  mschaef
 * CSCdi57038:  Spurious memory access made at
 * ipx_distribute_sap_list_check+0x60
 * Branch: California_branch
 *
 * Revision 3.51.4.17  1996/05/07  20:03:11  sluong
 * CSCdi56588:  IPX SAP table accepted EIGRP entry over locally learned SAP
 * Branch: California_branch
 *
 * Revision 3.51.4.16  1996/05/07  16:42:39  mschaef
 * CSCdi56910:  Unreachable services not removed from SAP table
 * Branch: California_branch
 *
 * Revision 3.51.4.15  1996/05/05  05:24:03  sluong
 * CSCdi55681:  NLSP: Distribute-list Out Rip causes Bus Error in
 * chunk_malloc
 * Branch: California_branch
 *
 * Revision 3.51.4.14  1996/05/04  02:18:35  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Enqueue to routable packet received on bridged interface to the
 *   appropriate queue instead of bridgeQ.
 * - Restore input to the BVI if necessary when copying packet for
 *   process-level.
 *
 * Name Access List
 *
 * Revision 3.51.4.13  1996/05/03  02:17:35  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.51.4.12.4.1  1996/04/27  07:39:40  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.51.4.12  1996/04/22  14:50:36  john
 * CSCdi54677:  Debug statement left in novell static route command
 * Branch: California_branch
 *
 * Revision 3.51.4.11  1996/04/10  16:43:53  john
 * CSCdi40886:  NLSP mib returns wrong values for objects
 * Branch: California_branch
 *
 * Revision 3.51.4.10  1996/04/09  22:32:37  john
 * CSCdi52606:  IPX static routes can cause error reading from NETWORK on
 *              client. Allow configurable metrics.
 * Branch: California_branch
 *
 * Revision 3.51.4.9  1996/04/09  16:46:42  sluong
 * CSCdi39057:  SYS-3-CPUHOG console message on 4000, also on 7000
 * Branch: California_branch
 *
 * Revision 3.51.4.8  1996/04/08  22:07:42  john
 * CSCdi53685:  Allow encoded characters in parser string macro
 * Branch: California_branch
 *
 * Revision 3.51.4.7  1996/04/02  07:18:29  dkatz
 * CSCdi51966:  EIGRP SAP split horizon causes problems
 * Branch: California_branch
 * Default split horizon off until we get to the bottom of this.
 *
 * Revision 3.51.4.6  1996/04/02  05:38:11  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.51.4.5  1996/04/01  04:23:38  john
 * CSCdi53167:  RIP format errors display twice in show ipx traffic
 * Branch: California_branch
 *
 * Revision 3.51.4.4.4.1  1996/04/08  02:43:52  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.51.4.4  1996/03/26  23:44:14  sluong
 * CSCdi52234:  ipx update-time cmd may generate traceback error msg
 * Branch: California_branch
 *
 * Revision 3.51.4.3  1996/03/26  23:14:14  sluong
 * CSCdi52523:  poisoned saps violate split horizon rule
 * Branch: California_branch
 *
 * Revision 3.51.4.2  1996/03/26  18:36:17  john
 * CSCdi52030:  IPX interface startup sends extra or unwanted RIP/SAP
 *              packets. In particular RIP is sent on IPXWAN NLSP links.
 * Branch: California_branch
 *
 * Revision 3.51.4.1  1996/03/18  22:53:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.51.6.1  1996/03/28  17:37:17  rbadri
 * Branch: LE_Syn_Cal_V111_1_3_branch
 * MTU support for sub-interfaces (used by ATM interfaces)
 *
 * Revision 3.51  1996/03/09  18:12:56  mschaef
 * CSCdi46978:  IPX fastswitch cache can grow very large
 *
 * Revision 3.50  1996/03/09  02:47:05  john
 * CSCdi51038:  IPX RIP/SAP triggered delays change upon reload
 *
 * Revision 3.49  1996/03/08  22:27:51  sluong
 * CSCdi50745:  SAP poisons for down interface sent twice
 *
 * Revision 3.48  1996/03/07  23:12:39  mschaef
 * CSCdi49527:  %SYS-3-CPUHOG from NLSP Update or IPX EIGRP process
 *
 * Revision 3.47  1996/03/07  06:47:12  shj
 * CSCdi50313:  IPX packets not evenized when sent via LANE
 * Treat ethernet LANE subinterfaces same as real ethernet.
 *
 * Revision 3.46  1996/03/05  23:12:14  sluong
 * CSCdi50715:  NLSP route aggregation conformance (phase 2)
 *
 * Revision 3.45  1996/03/02  17:19:51  john
 * CSCdi50505:  IPX ppp-client asynch interface delay value wrong
 *
 * Revision 3.44  1996/02/28  23:23:08  john
 * CSCdi50223:  Empty Service name causes NLSP warning on NetWare Servers
 *
 * Revision 3.43  1996/02/28  19:49:26  bbenson
 * CSCdi49729:  Using the same IPX encap on LANE subinterfaces failes
 * Config checks in novell.c would not allow same encaps on different
 * interfaces of the AIP.
 *
 * Revision 3.42  1996/02/25  03:53:17  mmcneali
 * CSCdi49686:  IPX non-functional in gs7 images on ISL encapsulating
 *              subinterfaces.
 *
 * Revision 3.41  1996/02/23  23:59:13  sluong
 * CSCdi49577:  Crash in nov_sapcheck running EIGRP
 *
 * Revision 3.40  1996/02/23  21:22:56  john
 * CSCdi49369:  IPX SAPs poisoned out too many interfaces
 *
 * Revision 3.39  1996/02/15  01:20:16  sluong
 * CSCdi46488:  IPX hop count 0
 *
 * Revision 3.38  1996/02/14  23:01:14  john
 * CSCdi47314:  IPX network FFFFFFFE ignored when default handling is
 *              disabled. Only treat -2 special if default route enabled.
 *
 * Revision 3.37  1996/02/08  20:15:53  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.36  1996/02/07  16:18:20  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.35  1996/02/05  17:27:28  john
 * CSCdi47276:  Parallel equal bandwidth links get different ipxwan nlsp
 *              metrics
 *
 * Revision 3.34  1996/01/29  07:31:53  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.33  1996/01/26  06:14:21  john
 * CSCdi46812:  no ipx sap-uses-routing-info causes sap output to stop
 *
 * Revision 3.32  1996/01/24  22:40:40  john
 * CSCdi47413:  debug ipx sap event shows fail to forward for non SAP
 *              packets
 *
 * Revision 3.31  1996/01/24  17:49:26  mschaef
 * CSCdi45840:  ipx parser seems a bit confused
 *
 * Revision 3.30  1996/01/23  23:33:38  dko
 * CSCdi47086:  Maximum value for ipx delay cmd is set too high for 2 bytes
 * Fixed the type of novell_delay to ushort and reduced Max. tick to 65535.
 *
 * Revision 3.29  1996/01/22  07:52:54  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.28  1996/01/17  03:32:37  boyle
 * CSCdi41676:  EIGRP not propagating sap when socket number changes
 *
 * Revision 3.27  1996/01/12  06:35:47  wilber
 * CSCdi46798:  Bridging ipx doesnt work if ipx routing had ever been
 * turned on
 * - Should update ipx routing indicator BEFORE updating ipx rxtypes
 * - Add back the missing call to update rxtype when configing ipx routing
 *
 * Revision 3.26  1996/01/09  21:04:50  fred
 * CSCdi43225:  ipx ping fails with autonomous switching enabled on mip
 *              need to disable fair queuing on any interface that is
 *              autonomous switched, as SP/SSE don't do fancy queuin
 *
 * Revision 3.25  1996/01/09  02:20:38  mmcneali
 * CSCdi46184:  Multiple IPX networks with same encap allowed on LAN
 *              interface.
 *
 * Only allow this for Virtual LAN encapsulations.
 *
 * Revision 3.24  1996/01/04  07:12:52  sluong
 * CSCdi46224:  SAPs missing using RSUP when interface flaps
 *
 * Revision 3.23  1996/01/04  01:13:45  john
 * CSCdi38803:  Subinterface ordering of ipx network and ipx encapsulation
 *              issue
 *
 * Revision 3.22  1996/01/01  01:51:33  john
 * CSCdi46268:  IPX Echo doesnt complain about illegal ipx address
 *
 * Revision 3.21  1995/12/22  20:38:16  boyle
 * CSCdi31767:  ipx router rip does not add networks once configured
 *
 * Revision 3.20  1995/12/20  18:04:25  wmay
 * CSCdi45455:  cannot configure ipx fast switching over smds - remove ALL
 * the checks for smds.
 *
 * Revision 3.19  1995/12/17  18:42:31  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.18  1995/12/15  18:26:08  john
 * CSCdi45662:  Static SAP doesnt allow embedded spaces in Service name
 *
 * Revision 3.17  1995/12/15  01:38:10  mschaef
 * CSCdi45394:  IPX AS and input access list restrictions not properly
 * enforced
 *
 * Revision 3.16  1995/12/13  23:37:48  jenny
 * CSCdi45241:  show ip int brief does not fit in 80 columns
 * Use namestring instead of short_namestring to display subinterface
 * names.
 *
 * Revision 3.15  1995/12/12  20:41:12  wmay
 * CSCdi45455:  cannot configure ipx fast switching over smds
 *
 * Revision 3.14  1995/12/07  22:34:52  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.13  1995/12/02  08:26:11  mschaef
 * CSCdi37520:  mac-address command not taken upon router reload or
 * initial config
 *
 * Revision 3.12  1995/12/01  06:25:22  mschaef
 * CSCdi37234:  Allow fastswitching of certain directed broadcast packets
 *
 * Revision 3.11  1995/11/30  21:48:28  sluong
 * CSCdi44536:  isis/nlsp does not generate the non-pseudonode
 * Fix the spurious memory access.
 *
 * Revision 3.10  1995/11/26  08:24:05  mmcneali
 * CSCdi43630:  ISL VLAN Multicast changed--IOS gs7 images are DEAF
 *
 *              If subinterface configured as a vLAN do not permit another
 *              encapsulation.
 *
 *              Correct counters of vLAN packets switched.
 *
 * Revision 3.9  1995/11/22  09:18:27  hampton
 * Fix the mips routine to read/write unaligned longlong words.  While
 * there, rework the entire family of routines to reduce overhead.  This
 * necessitates a change to code calling these routines.  [CSCdi44257]
 *
 * Revision 3.8  1995/11/21  23:43:06  john
 * CSCdi44009:  Test ipx echo command doesnt use ipx internal network
 *              address
 *
 * Revision 3.7  1995/11/20  22:27:12  snyder
 * CSCdi44202:  spellink errors
 *              inconsistant -> inconsistent
 *
 * Revision 3.6  1995/11/20  17:55:00  sluong
 * CSCdi43294:  filtering saps between protocols are not feasible
 *
 * Revision 3.5  1995/11/20  06:44:50  john
 * CSCdi43211:  Removing internal net writes ipx router rip & no ntework x.
 *
 * Revision 3.4  1995/11/19  06:02:42  john
 * CSCdi43213:  IPXCP w/. ppp shouldnt use internal net when IPXWAN is
 *              present
 *
 * Revision 3.3  1995/11/17  19:22:43  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:53:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:58:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.49  1995/11/08  21:39:00  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.48  1995/10/26  00:52:12  mschaef
 * CSCdi42340:  ipx maximum-path should allow more than 4 if NLSP is not on
 *
 * Revision 2.47  1995/10/25  15:53:42  boyle
 * CSCdi41675:  SAP entries not always marked as poisened when poison is
 * recieved
 *
 * Revision 2.46  1995/10/24  23:32:44  hampton
 * Remove the old inline s_kill_self_with_no_analysis by pushing it into
 * the routines that call it.  [CSCdi42780]
 *
 * Revision 2.45  1995/10/22  03:27:28  sluong
 * CSCdi42363:  novell_common_cleanup can kill Exec task
 * (comment from tkolar)
 *
 * Revision 2.44  1995/10/20  23:44:48  john
 * CSCdi42278:  Inconsistencies in rip/sap output/triggered-output delay
 *              config
 *
 * Revision 2.43  1995/10/19  00:46:25  sluong
 * CSCdi42363:  novell_common_cleanup can kill Exec task
 *
 * Revision 2.42  1995/10/13  01:04:26  shj
 * CSCdi41571:  atm interface uses wrong ipx node address
 * Backout CSCdi38984, which caused wrong ipx address on non-LANE ATM
 * interfaces.  Instead, LANE now changes ipx address if ipx is already
 * configured when LANE Client configured.
 *
 * Revision 2.41  1995/10/12  21:10:25  sluong
 * CSCdi41898:  clear ipx route <network> clears config as well as route
 *
 * Revision 2.40  1995/10/11  05:29:46  sluong
 * CSCdi37380:  no ipx encap command doesnt work.
 *
 * Revision 2.39  1995/10/04  23:23:40  sluong
 * CSCdi40568:  no ipx router rip causes IPX RIP process to run
 * continuously
 *
 * Revision 2.38  1995/10/04  22:06:51  sluong
 * CSCdi40901:  sh ipx traffic does not honor term length setting
 *
 * Revision 2.37  1995/10/04  03:59:36  sluong
 * CSCdi41319:  no ipx routing dont kill connected routes sometimes
 *
 * Revision 2.36  1995/10/04  01:25:41  john
 * CSCdi41272:  SegV exception in IPX fast switch cache update
 *              Part 2: code review suggested better fix.
 *
 * Revision 2.35  1995/09/29  18:35:48  sluong
 * CSCdi41016:  IPX Routing doesnt clean up cleanly
 *
 * Revision 2.34  1995/09/29  06:25:05  mschaef
 * CSCdi37830:  Gratuitous poisons generated via shut and config net
 *
 * Revision 2.33  1995/09/28  18:01:27  mschaef
 * CSCdi41028:  Memory leak when configuring/Deconfiguring IPX
 *
 * Revision 2.32  1995/09/23  21:43:08  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Generalize NLSP's interface to the ISIS LSP structure so code and
 * structures can be shared with ISIS-IP.
 *
 * Revision 2.31  1995/09/11  23:58:45  hampton
 * Convert XNS and Apollo to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi39966]
 *
 * Revision 2.30  1995/09/09 00:51:15  bbenson
 * CSCdi38984:  IPX uses Ethernet0 MAC instead of LEC MAC on LANE
 * subinterface
 * Load the AIP's MAC instead of the default in all cases...not just
 * LANE.
 * Yes, the AIP does have a MAC.
 *
 * Revision 2.29  1995/09/08  15:44:07  john
 * CSCdi38381:  IPX SAP holding onto memory when no longer in use
 *
 * Revision 2.28  1995/08/25  14:11:38  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi39328]
 *
 * Revision 2.27  1995/08/10 19:12:59  john
 * CSCdi38293:  SAP send may fail but counter still increments
 *
 * Revision 2.26  1995/08/09  00:22:18  kramling
 * CSCdi38439:  FEIP product is now supported in 11.0.  Update support to
 * 10.3.
 *
 * Revision 2.25  1995/07/31  21:03:30  john
 * CSCdi36543:  Rconsole is time-out with 2 netware clients (SPX-Spoof)
 *
 * Revision 2.24  1995/07/30  22:00:26  john
 * CSCdi37833:  IPX triggered rip/sap delays need a global default
 *              pt.2 10.3/11.0 have different defaults missed spots
 *
 * Revision 2.23  1995/07/28  23:56:28  john
 * CSCdi37833:  IPX triggered rip/sap delays need a global default
 *
 * Revision 2.22  1995/07/20  20:22:00  john
 * CSCdi35867:  %ALIGN-3-SPURIOUS: Spurious memory access w/show align
 *              too! Align errors when a SAP queue max is configured.
 *
 * Revision 2.21  1995/07/19  06:04:39  mschaef
 * CSCdi37298:  RIP-only IPXWAN subinterface XROUTES not included in LSP
 *
 * Revision 2.20  1995/07/19  05:44:52  mschaef
 * CSCdi37285:  IPX input-sap-filter incorrectly applied to NLSP services
 *
 * Revision 2.19  1995/07/18  23:00:41  john
 * CSCdi34414:  ipx maximimum-output-processes can be exceeded without
 *              notice. Add a show novell traffic counter.
 *
 * Revision 2.18  1995/07/18  22:33:27  john
 * CSCdi34411:  Large ipx output-rip/sap-delays may delay routing from
 *              running
 *
 * Revision 2.17  1995/07/01  03:38:47  hampton
 * Minor Novell IPX Cleanups.  Remove some unused variables and routines.
 * Eliminate direct references to the system clock.  [CSCdi36668]
 *
 * Revision 2.16  1995/06/28 18:03:43  john
 * CSCdi35568:  ipx route-cache cbus should not be permitted on ATM
 *              interfaces
 *
 * Revision 2.15  1995/06/27  18:08:20  john
 * CSCdi36397:  SPX spoof table remains when all spx-spoof is removed
 *
 * Revision 2.14  1995/06/23  00:07:45  john
 * CSCdi36324:  no ipx spx-spoof causes spurious memory event
 *
 * Revision 2.13  1995/06/22  16:15:48  sluong
 * CSCdi36015:  no network <nn> should not disable sap
 *
 * Revision 2.12  1995/06/21  09:01:40  smackie
 * Rename malloc memory pool derivatives so that they're prefixed with
 * malloc_ for consistency. (CSCdi36222)
 *
 * Revision 2.11  1995/06/20  22:07:04  bbenson
 * CSCdi36185:  LEC router disallows more than one ELAN running IPX
 * - Don't treat LANE ATM physical interface like an ethernet in the case
 *   of checking for a previously existing encap.
 *
 * Revision 2.10  1995/06/20  21:53:38  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.9  1995/06/20  01:18:25  bbenson
 * CSCdi36051:  IPX ping fails after initial success
 * CSCdi36052:  IPX ping returns 0 success rate (Serial/HDLC -> LANE/ARPA)
 *
 *         - Closed a hole where default router MAC address was being
 *         substituted for the client when the subint gets cycled down/up
 *
 * Revision 2.8  1995/06/19  19:01:15  mschaef
 * CSCdi36047:  IPX routing broken on unnumbered links
 *
 * Revision 2.7  1995/06/18  06:20:00  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.6  1995/06/16 17:10:22  dkatz
 * CSCdi35956:  Timer declarations should be in a separate file
 * Eliminate obsolete macro references.
 *
 * Revision 2.5  1995/06/15  18:03:36  john
 * CSCdi35604:  IPX ping to local interface fails if Internal Network
 *              is configured
 *
 * Revision 2.4  1995/06/13  22:14:57  mschaef
 * CSCdi35794:  Problems with IPX default route
 *
 * Revision 2.3  1995/06/10  07:07:56  mschaef
 * CSCdi35418:  IPX not working in FrankenStorm & Sapphire
 *
 * "ipx routing" command incorrectly places deleted interface
 * in "[down]" state.
 *
 * Revision 2.2  1995/06/09  13:26:00  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:28:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * DESCRIPTION OF ROUTINES IN NOVELL.C AND NOVINPUT.C
 *
 *
 *	nov_input --
 *
 *		Receives all Novell frames.
 *
 *		If a SAP GET NEAREST SERVER request, place the request on the
 *		novell_gnsQ for handling by 'novell_rcv_nearquery'.	
 *
 *		Puts all other SAP frames on 'novbgq' for handling by
 *		novell_fork
 *
 *		Gives all IGRP frames to 'novigrp_process'
 *
 *		Gives all RIP frames to 'novrip_pdb.inputq' for handling by
 *		novell_router
 *
 *		Puts all the other frames on the 'novell_genericQ' for handling
 *		by 'novell_forwarder' 
 *
 *	novell_router --
 *
 *	   Performs the following, in order:
 *
 *		1) Process all outstanding frames on novrip_pdb.inputq
 *			- If a RIP REPLY, process new RIP info; if flash
 *			  updates need to be sent, they will be posited
 *			  as a side effect of route creation or modification.
 *			  (See novell_mark_changed() for details.)
 *			  
 *			- If a RIP REQUEST --
 *				a) If request for a specific network, process
 *				   and immediately reply
 *				b) If request for ALL NETS, 'posit' a full
 *				   RIP (unicast) update to be given to the
 *				   sender of the request.
 *
 *		2)  If the 'novell_ripagertimer' goes off, this means
 *		    the RIP table needs aging.
 *
 *	novell_fork --
 *
 *          BECAUSE OF (4), BELOW, NEVER EVER STOP THIS PROCESS ONCE
 *          IT IS STARTED.
 *
 *		1) Process all outstanding frames on novbgq
 *			- If a SAP REPLY, process new SAP info; if flash
 *			  updates need to be sent, they will be posited
 *			  as a side effect of modifying the sap table.
 *			  (See novell_mark_sap_changed() for details.)
 *			  
 *			- If a SAP REQUEST --
 *				a) If request for a specific server, process
 *				   and immediately reply
 *				b) If request for ALL SERVERS, 'posit' a full
 *				   SAP (unicast) update to be given to the
 *				   sender of the request.
 *			- If a NEAR REPLY, ignore it
 *
 *		2) Clean up the 'freed NDB queue' - an queue of all NDBs that
 *		   have been deleted. For each, before deleting it, check if
 *		   any route processes have any information about the network
 *		   whose NDB is being deleted (for example, does IGRP have
 *		   any 'backup' info about a RIP route that is being deleted).
 *		   Must be done here because IGRP is not re-entrant, and this
 *		   code is guaranteed to run only when IGRP is *not* running.
 *
 *		3) If a request for IGRP 'backup servers' has been made, have
 *		   IGRP provide them. (That is, a SAP learned server is lost, 
 *		   does IGRP have any entries for this server in its table 
 *		   that it can provide.)
 *		
 *		4)  If the 'novell_sapagertimer' goes off, this means that
 *		    the SAP table needs aging.
 *
 *novell_rcv_nearquery --
 *
 *	 Receives SAP get nearest server queueries off 'novell_gnsQ'
 *	 Happily, that is all this process does.
 *
 *novell_forwarder --
 *	 Takes packets off of 'novell_genericQ' and processes them (echoes
 *	 or other (oddball) frames to forward (type 20 and helpered)).
 *
 * novell_supervisor
 *
 *	 Coordinates miscellaneous protocol activites including:
 *
 *	 - Advertise to lost route (ATLR) updates
 *	 - Scheduling of periodic full RIP and SAP updates
 *	 - Scheduling of partial (flash) RIP and SAP updates
 *
 *	 ATLR works as follows. All ATLR eligible routes are placed on
 *	 a special queue whenever we receive a poison for a route that
 *	 we continue to treat as reachable. If the ATLR timer is not
 *	 running it is set to expire with the ATLR interval (currently
 *	 hardwired at 1/2 second, per the Novell spec). Note that new
 *	 entries are added to the end of the list such that the list
 *	 is ordered from oldest to newest. Entries already on the list
 *	 (possible if known via multiple paths) are dequeued and added
 *	 back at the end of the list.
 *
 *	 When the ATLR timer expires this process examines the ATLR queue.
 *	 All entries at the beginning of the list which are still valid
 *	 (reachable) and have been on the list for more than 1/2 second
 *	 old are scheduled for a partial update (out all interfaces, not
 *	 just the interfaces from which poisons were received).
 *
 *	 If any entries are left on the list it is because they are not
 *	 yet 1/2 old. The timer is reset (with a granularity of 100 mSec)
 *	 such that it will expire when the first entry (or entries) in
 *	 the list are 1/2 second in age.
 *
 * novell_rip_update_daemon
 *
 *	 Coordinates all RIP update activity
 *
 *	 - Watches the parent timer for all Update State Variables (usv).
 *	   When a usv timer expires, this process wakes up and extracts
 *	   the usv from context of the expired timer. The process then
 *	   dispatches to process_next_rip_update() which builds and sends
 *	   the next packet (if any) for the update described by the usv.
 *	   If after sending an update packet, the update is incomplete,
 *	   the timer is set to the value of the correct interpacket gap
 *	   in use on the interface.
 *
 *	   Though not directly visible to the process, it may be thought
 *	   of as having a queue of usv update requests in order of
 *	   expiration. The process waits for the first timer on the
 *	   list to expire, then dequeues and processes it. If the
 *	   update is not complete it then sorts the usv back into the
 *	   active list and waits for the next usv to expire.
 *	 
 * novell_sap_update_daemon
 *
 *	 This process is exactly analagous to the rip_update_daemon,
 *	 except for the fact that it handles SAP updates. The two
 *	 processes could easily be combined but have been kept
 *	 seperate in order to allow RIP and SAP activity to be
 *	 more easily distinguished. This is especially important for
 *	 SAP, which is well known as a resource hog in large networks
 *
 * novell_net_down_handler
 *
 *	 Handles the details of networks declared down by users (via
 *	 "ipx down <net>" or "shut") and interfaces which flap down
 *	 spontaneously. In both cases the processe is signalled via
 *	 one of two booleans which it watches.
 *
 *	 The process has two basic states:
 *
 *		- Waiting for linkdown
 *		- Waiting for linkdown completions
 *
 *	 One or more linkdown events may be signalled at any time. All up
 *	 networks on interfaces going down are marked with
 *
 *		nidb->doing_linkdown_poisons
 *
 *	 If this is a user directed down or delete action, the nidb state
 *	 is changed to down_executing or delete_executing, as appropriate.
 *	 All up networks not marked as "doing_linkdown_poisons" are marked as
 *
 *		nidb->doing_reverse_poisons
 *
 *	 The list of newest networks and newest servers is traversed in
 *	 order (i.e., from newest to oldest). All entries reachable only
 *	 via down interfaces and not already marked by a linkdown event
 *	 are promoted to newest.
 *
 *	 A full update is then posited for all up interfaces. Do to the
 *	 presence of the flags the updates become either "down" or "revp"
 *	 updates (not "full", periodic updates). As these updates run they
 *	 select formerly "reachable" networks to poison or reverse poison
 *	 out each network, as appropriate.
 *
 *	 Each time a "down" or "revp" (linkdown or reverse poison) update
 *	 completes, the appropriate daemon process signals via the completion
 *	 boolean. At each completion event, the net_down process polls the
 *	 network list to determine if all poisons updates have completed.
 *	 (This probably should be optimized at some point.) When all poinson
 *	 updates are complete, igrp route adjustment is performed, connected
 *	 networks are deleted, and all RIP and SAP entries learned via the
 *	 downed networks are pruned from the routing/service tables.
 *
 *	 Some additional notes (from a previous release) follow. This
 *	 functionality is still present in the completion phase of
 *	 linkdown processing:
 *	 
 *      V: Finish off 'down_executing' or 'delete_executing' NIDBs...cleanup
 *	  directly connected information from these things if interface down
 *	10) For all NIDBs...
 *		a) If we had to just send out LINKDOWN_POISONS, then
 *		   tell IGRP the NIDB is dead, and kill the connected route
 *		   information associated with the interface
 *		b) If we were in 'delete executing' for the NIDB, 
 *		   delete it fully and remove the NIDB from the NIDB queue
 *		c) If we were in 'down executing' for the NIDB, mark it
 *		   fully DOWN.
 *		d) for all IDBs, if we just deleted an NIDB from it, have
 *		   the fast switching support clean up.
 *	 
 *		
 */

/*
 * DESIGN OF THE DATA STRUCTURES ASSOCIATED WITH STORING INFORMATION
 * ABOUT SERVERS.
 *
 *   Servers require a whole boatload of complex work to be done with
 *   them, and as such, there are multiple data structures that wind
 *   their way through server lists. This documentation attempts to
 *   describe all of these.
 *
 *   First, you must understand the core information that needs to be stored
 *   about each server. This is the 'novell_sapentrytype,' and it includes
 *   the following. Information about cross-data-structure pointers is not
 *   given here, but later. This list is primarily for reference, and has
 *   no real bearing on the cross-data-structure pointers described later.
 *	nidb: The nidb we learned the server from
 *	flags: Flags about the server's source (route flags, etc.)
 *	timer: 'msclock' of when an update was last received.
 *	poisoned: If TRUE, the server is poisoned.
 *	deleted: If TRUE, this entry is deleted, and should be ignored
 *		   in updates.
 *	route_delay, route_hops, route_flags, ext_hop: Information 'cached'
 *		   from the routing table about the true distance to the
 *		   server's network. This is the *real* distance determination
 *		   mechanism used when novell_algorithms is enabled.
 *	changed: The time, in msclock, when information about this server
 *		 was last changed.
 *	deleted_static: If TRUE, this server is a static server that is
 *		 being deleted because the user is deconfiguring it. This
 *		 allows deleted static servers to be advertised out as
 *		 poisoned, and be deleted only after the advertisements
 *		 have happened, just like other servers.
 *	srchost: host advertising server.
 *	snet:	 Network of host advertising server.
 *	server: A novell_serveridtype structure that is in the exact format
 *		used for SAP updates. This allows a simple 'copy' of
 *		information into the packet. It includes the following. The
 *		first two elements (server_type, server_name) are common
 *		server data, that define who the server is. The remaining
 *		items identify information about the server that could
 *		be different from each advertising source of a server.
 *		Applications, such as EIGRP, that must remember the information
 *		from each advertising source need to keep copies of this
 *		information from each source. The 'common' information need
 *		only be kept once (this saves on memory.)
 *                        server_type: The server 'type'
 *                        server_name: The server 'name'
 *                               dnet: The destination network of the server.
 *                              dsock: The destination socket of the server.
 *                            nethops: SAP distance server was advertised as.
 *
 *  Now that you know what a 'novell_sapentrytype' contains, it is time to
 *  learn how they are attached to each other and to other data structures.
 *
 *  The first structure to know about is the queue of 'novell_sapqueuetype'
 *  structures. This is pointed to by 'novell_sapQ'.
 *
 *  Each novell_sapqueue structure contains information needed to retrieve
 *  a set of servers of a SINGLE GIVEN SERVER TYPE. It has the following
 *  structure:
 *      server_type: The server type number of servers pointed to by this
 *		     data structure.
 *	next: Points to the next novell_sapqueue structure. This list is
 *	      kept in ascending numerical order, so when trying to find
 *	      a server of type 'N', if scanning the novell_sapQ list gets
 *	      you to an entry whose server_type > N, then you know 'N' is
 *	      nowhere in the list.
 *      sorted: TRUE if the following queue is sorted, with nearest server
 *		first.
 *      saps: A queue of servers of the given server_type type. Will be
 *	      guaranteed to be in a sorted order if 'sorted' is true; this
 *	      means the first entry is the nearest server. If not sorted, the
 *	      entire 'saps' list must be scanned to find the nearest server.
 *	next_sorted_time: The closest time from now when we will be allowed
 *	      to sort the 'saps' list in response to a Get Nearest Server
 *	      query. This throttle is placed in the code to ensure that a
 *	      'SAP' storm which affects server order does not create a CPU 
 *	      nightmare through excessive sorting requests. Note that when
 *	      new information is learned, and the list is currently sorted,
 *	      the new information is placed in the data structure in a way
 *	      to retain the SORTED order, so that sorting need not happen
 *	      too often. Of course, this implies the initial creation of this
 *	      list takes more time.
 *	sap_hashtable: This is a hashtable providing another pointer to all
 *	      the servers of this server_type. This is hashed by name, and
 *	      allows for quick lookup of servers when they are re-advertised
 *	      to us.
 *
 *      The following pointers in the 'novell_sapentrytype' structure support
 *	this data structure:
 *		next: The next entry in the sorted 'saps' server list for this
 *		      server type.
 *		hash_next: The next entry in the 'sap_hashtable' collision
 *		      server list for this server type.
 *
 *  The next structure to know about is the global hashtable called
 *  'nov_saproutetable'. This is a hash table by network number, which
 *  points via a collision list to servers which share the same network
 *  number. This allows us to quickly change the information known about
 *  a server's network route whenever a network route itself changes.
 *
 *  This is a simple hashtable, and it is supported by the
 *  'novell_sapentrytype' with the following fields:
 *      net_hash_next: Pointer to the next server with the same network
 *		       number hash value.
 *
 *   Note that if we learn a new network number about a server, we must
 *   obviously remove it from the current 'net_hash' list and place it
 *   on a new one.
 *
 *   The final server datastructure to know about is the 'newest' servers
 *   datastructure. Actually, RIP has one for networks as well. This
 *   list is pointed to by 'novell_newest_servers'. Every time a server
 *   changes information, it is removed from its current location, and placed
 *   at the front of this list. In this way, a partial update need not scan
 *   the entire server list, but only needs to scan the 'newest' entries,
 *   in order. Since server entry removal and reinsertion is so common, this
 *   is a doubly linked list, and is supported with the following two
 *   pointers in the 'novell_sapentrytype':
 *	next_newest: Pointer to the next 'newest' server on the
 *		     'novell_newest_servers' list. That is, it points
 *		     to the server who most recently changed before this
 *                   current server changed.
 *	prev_newest: Pointer to the previous 'newest' server on the
 *                   'novell_newest_servers' list.
 *
 *  Obviously, with all these data structures, consistency is a must.
 *  Therefore, well defined entry points are defined:
 *       novell_place_in_sap_list: Called whenever a new server
 *					  is created.
 *       novell_mark_sap_changed: Called whenever information stored in
 *				  the 'novell_sapentrytype' entry is changed.
 *       nov_sap_free: Called whenever a server is to be deleted.
 *       nov_sap_set_new_dnet: Set a new 'destination network' for a server.
 *			       Call this BEFORE calling the common change
 *			       routine (novell_mark_sap_changed).
 *       novell_update_sap_route_info: Called whenever information about
 *			       a route's path (NDB) has changed.
 *
 */

/******************************************************************************
 *
 * CHANGES FOR NOVELL COMPLIANCE:
 *
 * "ipx novell-algorithms" was never documented, formally tested, or visible.
 * It is now deprecated with the exception of changing next hop selection
 * from 9.21+ Novell compliant "ticks/hops" to the 9.1 and earlier behavior
 * of "hops".
 *
 * This continues to be unsupported and is for emergency upgrade use only
 * from 9.1 (and earlier) to 11.2.
 *   
 *  Novell/IPX		     Behavior when 'novell algorithms'
 * Property                 Enabled                   Disabled
 *|-----------------------|-------------------------|------------------------|
 *|Computing the 'best    | If the 'delay' (ticks)  | Only the hopcount is   |
 *|choice' next-hop       | field is different      | used in comparison.    |
 *|router for a given     | between A and B, then it| Parallel paths are made|
 *|network between routers| is used as the metric of| only when hopcounts are|
 *|'A' and 'B'.           | comparison. If it is    | equal.                 |
 *|                       | the same, then the hop  |                        |
 *|                       | count is used.          |                        |
 *|                       | Parallel paths made only|                        |
 *|                       | when delays are equal.  |                        |
 *|-----------------------|-------------------------|------------------------|
 ****************************************************************************/

#include "master.h"
#include "../ui/common_strings.h"
#include <stdarg.h>
#include <string.h>
#include <ciscolib.h>
#include "fastswitch_registry.h"
#include "sys_registry.h"
#include "../hes/sse_registry.h"
#include "mgd_timers.h"
#include "interface_private.h"
#include "packet.h"
#include "ttysrv.h"
#include "logger.h"
#include "../xns/msg_ipx.c"		/* Not a type, see logger.h */
#include "../os/free.h"
#include "../if/ether.h"
#include "xns.h"
#include "novell.h"
#include "novell_public.h"
#include "xnsfast.h"
#include "novfast.h"
#include "novigrp.h"
#include "novisis.h"
#include "novnlsp.h"
#include "config.h"
#include "access.h"
#include "../if/network.h"
#include "parser.h"
#include "../ibm/netbios.h"
#include "../ibm/netbios_sb.h"
#include "../os/buffers.h"
#include "../parser/parser_actions.h"
#include "../clns/clns.h"
#include "../clns/isis_externs.h"
#include "../clns/clns_externs.h"
#include "../clns/isis_ipx.h"
#include "../wan/snapshot.h"
#include "../if/if_vidb.h"
#include "../if/atm.h"
#include "../lane/lane.h"
#include "ipx_registry.h"
#include "../clns/isis_registry.h"
#include "../if/priority.h"
#include "../ui/common_strings.h"

#include "../vlan/vlan.h"
#include "../util/regexp.h"


/*
 * Local Variables
 */
novpdbtype *nov_proctable[MAX_NOVPROCS];
int novell_igrp_running = 0;
boolean ipx_eigrp_sap_split_horizon;  /* EIGRP SAP has split horizon */
struct radix_node_head *nov_rnhead;         /* patricia tree */
novpdbtype *nov_proctable[MAX_NOVPROCS];
queuetype novbgQ;			/* Novell services queue */
queuetype novell_gnsQ;			/* packet queue of GNS requests */
queuetype novell_sapQ;			/* List of Novell SAP received */
queuetype novell_genericQ;              /* echo reply, and helpered packets */
queuetype novechoQ;			/* Novell echo matching queue */
queuetype novell_ipxwanQ;		/* Novell IPXWAN queue */
queuetype nov_freed_ndbQ;		/* Queue of pending free ndb's */
queuetype nov_free_nidbQ;		/* Queue of free nidb's */
ulong	  ipx_checkpointvalue;		/* IPX accounting checkpoint value */
ulong	novell_route_count;		/* Total IPX routes */
ulong	novell_server_count;		/* Total IPX servers */
nidbtype  *novell_next_output_nidb;	/* Last NIDB in queue, for output */
ulong novell_backup_server_interval; 	/* How often to query backup SAPs */
ulong novell_freed_ndbq_wakeup;		/* Time to process the freed NDBQ? */
sys_timestamp novell_timestamp;         /* 64 bits timestamp counter */
sys_timestamp novell_sapagertimer;	/* SAP ager timer */
sys_timestamp novell_ripagertimer;	/* RIP ager timer */
sys_timestamp novell_onemin_timer;	/* per-minute task timer */
sys_timestamp novell_freed_ndb_timer;	/* Freed NDB deletion timer */
sys_timestamp novell_backup_servers_timer; /* Get backups? */
ulong novell_update_proc_throttle;      /* Throttle update proc creation */
ulong novell_update_sched_throttle;     /* Throttle update scheduling */
ulong novell_def_output_rip_dly;        /* default inter packet RIP gap */
ulong novell_def_output_sap_dly;        /* default inter packet SAP gap */
ulong novell_def_trig_rip_dly;          /* default gap triggered RIP packets */
ulong novell_def_trig_sap_dly;          /* default gap triggered SAP packets */
novell_sapentrytype *novell_newest_servers; /* List of newest servers */
novndbtype *novell_newest_routes; 	/* List of newest routes */
/*
 * "atlr" = Advertise To Lost Routers --- routes lost by other routers that
 * we should advertise to them. Note that this is a STRUCTURE, not
 * a pointer! novell_atlr_head->next_atlr is the front of the list.
 * novell_atlr_head->prev_atlr is the tail of list.
 */
novndbtype novell_atlr_head; 		/* Head of lost route dbly lnklist */
ulong novell_max_hops;			/* Configurable maximum hops */
ulong novell_infinity;
pid_t novell_inp_pid;			/* Input */
pid_t novell_sap_pid;			/* SAP Input*/
pid_t novell_spv_pid;      		/* Novell Supervisor */
pid_t novell_rup_pid;      		/* Novell Update Daemon */
pid_t novell_sup_pid;      		/* Novell Update Daemon */
pid_t novell_net_pid;      		/* RIP/SAP DOWN Handler */
pid_t novell_gns_pid;			/* GNS */
pid_t novell_fwd_pid;			/* Forwarder */
pid_t novell_ipxwan_pid;       		/* IPXWAN */
pid_t novell_ipxwanbg_pid;		/* IPXWAN Background */
int nov_static_route_count;
int nov_static_server_count;
enum IPX_ECHO_TYPE novell_ping_deflt;   /* Which ping to default to */
boolean ipx_kludge_enabled_flag; 	/* If test specific kludges enabled */
boolean ipx_server_split_on_server_paths; /* Split SAP on server paths? */
nidbtype *ipx_internal_network_nidb;
boolean novell_backup_servers_needed;
boolean ipxwan_enabled;		        /* ipxwan subsys present/initialized */
boolean novell_nlsp_enabled;		/* NLSP subsys present/initialized */
boolean novigrp_enabled;		/* EIGRP subsys present/initialized */
int novell_nlsp_running;		/* NLSP configured/running */
boolean novell_default_route;           /* Use default route (net -2) */
novndbtype *novell_default_ndb;         /* Default route, if known */
boolean novell_running;			/* TRUE if running Novell */
boolean novell_shutting_down;		/* TRUE if "no ipx routing" pending */
boolean novell_started;			/* TRUE if Novell processes */
boolean nov_gns_roundrobin;             /* TRUE if reporting all services */
boolean nov_load_share_per_host;        /* TRUE if using only one path */
boolean novell_algorithms;		/* If using Novell compatible algos. */
boolean novell_sap_uses_routing; 	/* SAP table affected by routes? */
boolean novell_potential_pseudonode;    /* Keep potential pseudonode data? */
uchar nov_defaddr[IEEEBYTES];		/* addr used by non-LANs */
ushort novell_maxpaths;			/* Active max path count */
ushort novell_cfg_maxpaths;		/* Configured max path count */
nov_routetable *novell_table;		/* Routing table for Novell */
nov_backup_routetable *novell_table2;	/* Backup rt table for isis/nlsp */
nov_statictable *static_table;		/* Floating static table */
nov_backup_saptable *novell_table3;	/* Backup sap table for isis/nlsp */
nov_urd_table_type *nov_urd_table;	/* Unreachable sap routes */
ulong novell_urd_insert_count;          /* URD entry added */
ulong novell_urd_callback_count;        /* URD entry consumed */
ulong novell_urd_scrub_count;           /* URD entry invalid and deleted */
novroutetype *nov_pessimal_route_list;  /* Hold pessimal backup routes */
nov_svc_type *nov_pessimal_sap_list;    /* Hold pessimal backup services */
ulong novell_stat[NOVELL_MAXTRAFFIC];	/* Novell statistics */
ulong nov_last_bad_sap_net;
uchar nov_last_bad_sap_host[IEEEBYTES];
ulong nov_last_bad_rip_net;
uchar nov_last_bad_rip_host[IEEEBYTES];

/*
 * New RIP/SAP wakeup timers and booleans:
 */
watched_boolean *novell_rip_part_wb; /* Partial RIP update (novell_changed) */
watched_boolean *novell_sap_part_wb; /* Partial SAP update (novell_changed) */
watched_boolean *novell_link_down_wb; /* Signal link going down */
watched_boolean *novell_sent_down_wb; /* Signal down/reverse poison sent */

mgd_timer novell_rip_delay_timer;
mgd_timer novell_sap_delay_timer;
mgd_timer novell_super_root_timer;
mgd_timer novell_atlr_timer;

ulong novell_routes;     /* Current route count */
novndbtype *ipx_pak_ndb_for_cache;  /* route ndb to pass to cache update */
/* Support "novell encap" command before "novell network" command issued */
static idbtype  *implicit_idb  = NULL;
static int implicit_enctype    = ET_NULL;
ulong novell_sapqueuemax;		/* SAP queue Length max. 0 == inf */
ulong novell_gns_response_delay;
boolean novell_rip_running;
boolean novell_type20_in_checks;  /* Special checks before accepting type 20 */
boolean novell_type20_out_checks; /* Special checks before flooding type 20 */
boolean novell_netbios_in_checks; /* Special checks for other netbios bc */
boolean novell_type20_helpered;   /* Old-style type 20 forwarding */
boolean novell_fastswitch_bcast;  /* Fastswitch selected helpered packets */
boolean novell_global_acl_checks; /* True if acl check required for fs path */
novpdbtype novconnected_pdb;
novpdbtype novstatic_pdb;
novpdbtype novfstatic_pdb;
novpdbtype novrip_pdb;
ulong novell_debug1 = 0, novell_debug2 = 0;
ulong novell_debug3 = 0, novell_debug4 = 0;
ulong novell_debug5 = 0, novell_debug6 = 0;
ulong novell_debug7 = 0, novell_debug8 = 0;
ulong novell_debug_net1 = 0;
uchar novell_debug_host1[IEEEBYTES] = {0,0,0,0,0,0};
ulong novell_debug_net2 = 0;
uchar novell_debug_host2[IEEEBYTES] = {0,0,0,0,0,0};
uchar ipx_internal_hostid[IEEEBYTES]  = {0,0,0,0,0,1};
uchar ipx_internal_name[NOVELL_SERVER_NAME_LEN];  /* System Name */
nidbtype *nullnidb;
#ifdef DEBUG_SORT
ulong max_qsort_depth = 0;
#endif
nov_saproutetable *novell_sap_route_hashtable;
boolean novell_auto_sw_supported;
ulong ipx_internal_net;
static uchar capacity_host[IEEEBYTES];
static ulong capacity_fork;
static int capacity_startroute;
static int capacity_nexthop_net;
static int capacity_serversleft;
static int capacity_routesleft;
static char capacity_servername[50];
static char capacity_ourhost[28];

/*
 * External declarations
 */
static void nov_default_sap_delay_command(parseinfo *);
static void nov_default_rip_delay_command(parseinfo *);
static void nov_default_trig_sap_delay_command(parseinfo *);
static void nov_default_trig_rip_delay_command(parseinfo *);

void test_novell(parseinfo *);
static void test_novell_gns(parseinfo *);
static void test_novell_name(parseinfo *);
static void test_novell_watchdog(parseinfo *);
static void test_novell_netbios(parseinfo *);
static void test_novell_debug(parseinfo *);
static void test_novell_ripreq(parseinfo *);

/* Forward declarations */

static nov_usv *nov_allocate_and_init_usv (enum IPX_UPD_TYPE type,
				    nidbtype *nidb,
				    uchar const *host,
				    ushort sock,
				    enum IPX_FULL fullupdate,
				    enum IPX_DESTINATION destination,
				    ushort servertype,
				    ushort query_type,
				    uchar *server_name, /* %%% */
				    ulong dnet,
				    ulong dmask,
				    boolean reverse_poison,
				    boolean linkdown_poison);

static void nov_deallocate_usv (nov_usv *usv);

static void novell_nidb_free (nidbtype *nidb);

#undef NEVER
#undef TESTCMD
#undef NOVELL_DEBUG

static const char nov_badaddr[] = "\n%%Illegal IPX address - %s";
static const char nov_notenabled[] = "\n%%IPX not running";
static const char nov_remove_cipx[] = "\n%%IPX header compression must be removed%s";
static const char nov_no_autosw[] = "\n%%IPX autonomous switching not supported";
static const char nov_no_sse[] = "\n%%IPX SSE switching not supported";
static const char nov_on_itf[] = " on interface %s";
static const char nov_on_xxx_itf[] = " on %s interface %s";
queuetype nov_router_initQ;
static boolean nov_router_init_running = FALSE;

#define NOV_LINKDOWN_POISONS_NEEDED(thisnidb) \
  (NOVELL_NIDB_ISUP(thisnidb) && (!interface_up((thisnidb)->idb)) && \
   (!(thisnidb)->nov_sent_linkdown_poisons))

#ifdef DEBUG_NIDB
void
dump_nidb (nidbtype *nidb)
{
    if (!novell_debug)
	return;

    buginf("\nnidb@%8x: %x (enctype %d=%s), idb=%s, novellnets=%d",
	   nidb,
	   nidb->novellnet,
	   nidb->enctype,
	   encstring(nidb->enctype),
	   nidb->idb->namestring,
	   nidb->idb->novellnets);
}
void
dump_novidbQ (void)
{
    nidbtype *nidb;

    if (!novell_debug)
	return;

    buginf("\n----->novidbQ----->");
    FOR_ALL_NIDBS(nidb)
	dump_nidb(nidb);
    buginf("\n<-----novidbQ<-----");
}
#endif

/*
 * nov_free_nidb - place nidb on free queue pending reuse.
 */
static void nov_free_nidb (nidbtype *nidb)
{
    enqueue(&nov_free_nidbQ, nidb);
    nidb->state_version++;
}


/*
 * nov_get_nidb - allocate an nidb either from the free queue or via malloc()
 */
static nidbtype *nov_get_nidb (void)
{
    nidbtype *nidb;

    nidb = dequeue(&nov_free_nidbQ);
    if (nidb == NULL) {
	nidb = malloc_named(sizeof(nidbtype), "IPX NIDB");
    } else {
      /* reset the used nidb to a known state as if it were from malloc */
      memset(nidb, 0, sizeof(nidbtype));
    }
    return(nidb);
}

    
/*
 * novell_set_nidb_{rip,sap}_mode
 * Set the configured mode for an interface, and play with the appropriate
 * status bits.
 */
static char *novell_name_auto_mode (enum IPX_RIPSAP_CONFIG status)
{
    switch (status) {
      case IPX_RIPSAP_CONFIG_OFF:	return("OFF");
      case IPX_RIPSAP_CONFIG_AUTO:	return("AUTO");
      case IPX_RIPSAP_CONFIG_ON:	return("ON");
      default: 				return("");
    }
    return("");
}

void novell_start_nidb_rip_auto (nidbtype *nidb)
{
    if (nidb->lsp_rip_cfg != IPX_RIPSAP_CONFIG_AUTO)
      return;
    nidb->lsp_rip_on = TRUE;
    GET_TIMESTAMP(nidb->lsp_rip_timestamp);
}
void novell_stop_nidb_rip_auto (nidbtype *nidb)
{
    nidb->lsp_rip_on = FALSE;
    TIMER_STOP(nidb->lsp_rip_timestamp);
}
void novell_set_nidb_rip_mode (nidbtype *nidb, enum IPX_RIPSAP_CONFIG status)
{
    nidb->lsp_rip_cfg = status;
    switch (status) {
      case IPX_RIPSAP_CONFIG_OFF:
	nidb->lsp_rip_on = FALSE;
	TIMER_STOP(nidb->lsp_rip_timestamp);
	break;
      case IPX_RIPSAP_CONFIG_ON:
	nidb->lsp_rip_on = TRUE;
	TIMER_STOP(nidb->lsp_rip_timestamp);
	break;
      case IPX_RIPSAP_CONFIG_AUTO:
	novell_start_nidb_rip_auto(nidb);
	break;
    }
}

void novell_start_nidb_sap_auto (nidbtype *nidb)
{
    if (nidb->lsp_sap_cfg != IPX_RIPSAP_CONFIG_AUTO)
      return;
    nidb->lsp_sap_on = TRUE;
    GET_TIMESTAMP(nidb->lsp_sap_timestamp);
}
void novell_stop_nidb_sap_auto (nidbtype *nidb)
{
    nidb->lsp_sap_on = FALSE;
    TIMER_STOP(nidb->lsp_sap_timestamp);
}
void novell_set_nidb_sap_mode (nidbtype *nidb, enum IPX_RIPSAP_CONFIG status)
{
    nidb->lsp_sap_cfg = status;
    switch (status) {
      case IPX_RIPSAP_CONFIG_OFF:
	nidb->lsp_sap_on = FALSE;
	TIMER_STOP(nidb->lsp_sap_timestamp);
	break;
      case IPX_RIPSAP_CONFIG_ON:
	nidb->lsp_sap_on = TRUE;
	TIMER_STOP(nidb->lsp_sap_timestamp);
	break;
      case IPX_RIPSAP_CONFIG_AUTO:
	novell_start_nidb_sap_auto(nidb);
	break;
    }
}
/*
 * nov_get_rip_metrics
 *
 * get the RIP metric to be sent in a RIP response
 * called by both normal RIP update processing and RIP request handling
 * making this a routine used by both ensures the metrics will be set
 * the same in both cases.
 */
static void nov_get_rip_metrics(novndbtype *entry , novellripinfotype *rip_ptr,
			 nidbtype *nidb)
{

    if (entry->flags & NET_IGRP) {
	if (novrip_pdb.redist_hops) {
	    rip_ptr->metric = novrip_pdb.redist_hops;
	    rip_ptr->delay = novrip_pdb.redist_delay;
	} else {
	    rip_ptr->metric = min(entry->ext_hop + 1,NOVELL_RIP_ADV_INFINITY);
	    rip_ptr->delay = (entry->ext_delay ? entry->ext_delay :
			      entry->ext_hop) + NOVELL_IDB_TICKS(nidb);
	}
    } else { /* Non-IGRP, including static/conn/internal */
	rip_ptr->metric =
	    min(entry->hopcount + 1, NOVELL_RIP_ADV_INFINITY);
    }
}

/*
 * multicast_addr - Determine the multicast status of a given host address
 * given the hw idb info and the "low order" byte of the host address.
 */
boolean nov_multicast_addr (hwidbtype *hwidb, uchar lsb)
{
    if (hwidb->status & (IDB_ETHER | IDB_SERIAL | IDB_LEX))
	return(lsb & NOVELL_MULTICAST_ETHER);
    
    if (hwidb->status & IDB_TR)
	return(lsb & NOVELL_MULTICAST_TR);
    
    if (hwidb->status & IDB_FDDI)
	if (hwidb->type == IDBTYPE_FDDIT)
	    return(lsb & NOVELL_MULTICAST_ETHER);
	else
	    return(lsb & NOVELL_MULTICAST_TR);

    return(lsb & NOVELL_MULTICAST_ETHER);
}

/*
 * novell_auto_sw_active
 *
 * Scan the configuration and determine whether IPX autonomous switching is
 * present on any active interface.
 */
boolean novell_auto_sw_active (void)
{
    nidbtype *nidb;

    if (novell_running) {
	FOR_ALL_NIDBS(nidb) {
	    if ((nidb->idb->hwptr->novell_routecache & NOVELL_RCACHE_CBUS) &&
		(interface_up(nidb->idb))) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * novell_on_idb
 *
 * Return TRUE if Novell/IPX is enabled on the given interface
 */
boolean
novell_on_idb (idbtype *idb)
{
    if ((!novell_running || novell_shutting_down) || (!idb->novellnets))
      return(FALSE);
    else
      return(TRUE);
}

/*
 * novell_on_hwidb
 *
 * Return TRUE if Novell/IPX is enabled on any swidb on this hwidb
 */
boolean novell_on_hwidb (hwidbtype *hwidb)
{
    idbtype *idb;

    if (novell_running) {
	FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {
	    if (novell_on_idb(idb)) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * rcache_cbus_sse_on_this_hwidb
 *
 * Return TRUE if autonomous/SSE switching is enabled on this hwidb
 */
static boolean rcache_cbus_sse_on_this_hwidb (hwidbtype *hwidb)
{
    
    if (novell_running) {
	if (hwidb->novell_routecache &
	    (NOVELL_RCACHE_CBUS | NOVELL_RCACHE_SSE)) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * rcache_cbus_sse_on_any_hwidb
 *
 * Return TRUE if autonomous/SSE switching is enabled on any hwidb
 */
static boolean rcache_cbus_sse_on_any_hwidb (void)
{
    hwidbtype *hwidb;
    
    if (novell_running) {
	FOR_ALL_HWIDBS(hwidb) {
	    if (novell_on_hwidb(hwidb) &&
		rcache_cbus_sse_on_this_hwidb(hwidb)) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * novell_sse_sw_active
 *
 * Scan the configuration and determine whether IPX SSE switching is
 * present on any active interface.
 */
boolean novell_sse_sw_active (void)
{
    nidbtype *nidb;

    if (novell_running) {
	FOR_ALL_NIDBS(nidb) {
	    if ((nidb->idb->hwptr->novell_routecache & NOVELL_RCACHE_SSE) &&
		(interface_up(nidb->idb))) {
		return(TRUE);
	    }
	}
    }
    return(FALSE);
}

/*
 * novell_findsapqueue
 *
 * Find the SAP queue for a given type. Returns a pointer to the queue itself.
 * WILL CREATE ONE IF NONE EXISTS IF 'CREATE' IS SET, SO DON'T CALL
 * THIS PUPPY AT INTERRUPT LEVEL!!!
 */
static novell_sapqueuetype *novell_findsapqueue (ushort saptype,
					  enum IPX_SAP_QUEUE_LOOKUP
					  lookup_type)
{
    novell_sapqueuetype *current, *prev, *new;
    queuetype *sapque;
    int i;

    /* Try to find the SAP queue in the current list */
    for (prev = NULL, current = (novell_sapqueuetype *) novell_sapQ.qhead;
	 current;
	 prev = current,
	 current = current->next) {
	/* Found? */
	if (current->server_type == saptype) {
	    return(current);
	}
	/* Are we past where it could be? */    
	else if (current->server_type > saptype) {
	    break;
	}
    }
    /* If at this point, we didn't find the desired entry. */
    if (lookup_type == IPX_SAP_QUEUE_LOOKUP_MUST_EXIST)
	return(NULL);
    new = malloc_named(sizeof(novell_sapqueuetype), "IPX SAP List");
    if (new == NULL) {
	return(NULL);
    }
    new->server_type = saptype;
    new->saps.flink = &new->saps;
    new->saps.blink = &new->saps;
    new->next = NULL;
    new->sorted = TRUE;
    TIMER_STOP(new->next_sorted_time);
    for (i = 0; i < NOVELL_SAP_HASHLEN; i++)
      new->sap_hashtable[i] = NULL;
    /* Place it in the queue */
    insqueue(&(novell_sapQ), new, prev);
    sapque = &(novell_sapQ);
    if (novellsap_debug || novellsap_event_debug)
      buginf("\nIPX: SAP queue-hash added for type %x, count %d",
	     saptype, sapque->count);
    return(new);
}

/*
 * nov_nlsp_router_command
 * do the work that either parser or snmp (or elsewhere) has started
 * to enable NLSP routing.
 *
 * interactive = TRUE if we come from the parser
 * sense       = are we enabling or disabling (csb->sense)
 * csb         = csb block if interactive, NULL if not interactive
 * pdb         = the novell pdb or NULL if we are enabling
 * router_tag  = the pdb's router tag, or NULL if no tag.
 */

void nov_nlsp_router_command (boolean interactive, boolean sense,
			      parseinfo *csb, novpdbtype *pdb,
			      char *router_tag)
{

  if (sense) {
    if (pdb == NULL) {
      if (!novell_nlsp_enabled) {
	if (interactive)
	  printf("\n%% Error: NLSP is not included in this IOS build");
      } else {
	pdb = reg_invoke_ipx_nlsp_start(router_tag);
      }
    }
    if (pdb == NULL) {
      if (interactive)
	printf("\n%% Error creating NLSP routing process");
    } else {
      if (!pdb->running) {
	if (pdb->restart) {
	  (*pdb->restart)(pdb);
	}
      }
      if (interactive) {
	if (set_mode_byname(&csb->mode, "ipx-router",
			    MODE_VERBOSE)) {
	  csb->protocol = pdb;
	} else {
	  return;
	}
      }
      pdb->entered = TRUE;
    }
  } else {
    if (pdb == NULL) {
      if (interactive)
	printf("\n%% NLSP not configured");
    } else {
      nov_router_erase(pdb);
    }
  }
}

/* csb->prototype contains the AS number... */
pdecl(confignovrouter_start);

void novell_router_command (parseinfo *csb)
{
    nidbtype *nidb;
    int i;
    novpdbtype *pdb, *loop_pdb;
    idbtype *idb;
    char *router_tag = NULL;

    if (!novell_running || novell_shutting_down) {
        if (!csb->nvgen) {
            printf("\n%%Must give \"ipx routing\" command first");
	}
        return;
    }

    if (csb->nvgen) {
        pdb = (novpdbtype *) GETOBJ(pdb, 1); 
	if (pdb == (novpdbtype *) &novrip_pdb) {
	    /*
	     * Yes, there is a goto here! The non_default_behavior flag
	     * was constantly creating bugs; the simplest thing to do is
	     * if there is a reason to generate config commands, just jump
	     * there rather than constantly making sure this variable is
	     * set correctly throughout all of the other checks. There
	     * *are* valid reasons for a goto...
	     */
	    if (!novell_rip_running) {
		goto generate_config_commands;
	    }
	    if (novrip_pdb.ofltr_all || novrip_pdb.ifltr_all ||
		novrip_pdb.sap_ofltr_all || novrip_pdb.sap_ifltr_all) {
	        goto generate_config_commands;
	    }
	    /*
	     * Check to see if any non-default behavior exists before
	     * doing ANY NV-writes for 'router rip...' sigh...
	     */

	    /*
	     * Ensure all nidbs are enabled for rip routing (default behavior)
	     *
	     * IPXWAN interfaces never cause ipx router rip commands
	     *        may happen if IPXWAN is down and novellnet == 0.
	     */
	    FOR_ALL_NIDBS(nidb) {
		for (i = 0; i < novrip_pdb.netcount; i++) {
		    if ((novrip_pdb.networks[i] == nidb->novellnet) ||
			nov_ipxwan_on_nidb(nidb) || (nidb->idb == NULL) ||
			(NOVELL_NIDB_DELETED_MODE(nidb)))
			break;
		}
		if ((i == novrip_pdb.netcount) && 
		    (nidb->novellnet && !(NOVELL_NIDB_DELETED_MODE(nidb)))) {
		    goto generate_config_commands;
		}
	    }
	    /*
	     * Ensure RIP redistributes everything except floating
	     * static (default behavior)
	     */
	    for (loop_pdb = (novpdbtype *) novpdbQ.qhead;
		 (loop_pdb != NULL);
		 loop_pdb = loop_pdb->next) {
		if (!(novrip_pdb.distflag & loop_pdb->mask)) {
		    if ((loop_pdb->proctype == NET_NLSP) &&
			!loop_pdb->running) {
			continue;
		    }
		    if (loop_pdb->proctype != NET_FLOATING_STATIC) {
			goto generate_config_commands;
		    }
		} else if (loop_pdb->proctype == NET_FLOATING_STATIC) {
		    goto generate_config_commands;
		}
	    }
	    /*
	     * See if there are any distribute-lists or distribute-sap-lists
	     */
	    for (i = 0; i < MAX_NOVPROCS; i++) {
		if (novrip_pdb.ofltr_proto[i] || 
		    novrip_pdb.sap_ofltr_proto[i]) {
		    goto generate_config_commands;
		}
	    }
            FOR_ALL_SWIDBS(idb) {
                if (IPX_PRIMARY_NIDB(idb)) {
                    if (IPX_PRIMARY_NIDB(idb)->oprotofltr[pdb->index] ||
                	IPX_PRIMARY_NIDB(idb)->iprotofltr[pdb->index] ||
			IPX_PRIMARY_NIDB(idb)->sap_oprotofltr[pdb->index] ||
                	IPX_PRIMARY_NIDB(idb)->sap_iprotofltr[pdb->index]) {
			goto generate_config_commands;
		    }
		}
	    }
	    /*
	     * If no non-default behavior, just return & write nothing...
	     */
	    return;
	} else {
	    /*
	     * Interface subcommands such as "ipx hello-int eigrp 100 30" 
	     * must create the pdb even if the user hasn't entered 
	     * "ipx router eigrp 100" yet. Don't display the process
	     * until the "ipx router" command is entered.
	     */ 
	    if (!pdb->entered)
		return;
	}
      generate_config_commands:
	if ((pdb == (novpdbtype *) &novrip_pdb) && !novell_rip_running) {
	  nv_write(TRUE,"!");
	  nv_write(TRUE, "no %s", csb->nv_command);
	  return;
	} else {
	  if (!pdb->running) {
	    return;
	  }
	  nv_write(TRUE,"!");
	  nv_write(TRUE, "%s", csb->nv_command);
	}
	csb->protocol = pdb;
        *csb->nv_command = '\0';
	if (set_mode_byname(&csb->mode, "ipx-router", MODE_SILENT)) {
	    push_node(csb, get_mode_nv(csb));
	}
        return;
    }

    csb->protocol = NULL;
    switch (csb->router_name.protocol) {
      case PROC_IGRP2:
	pdb = find_novigrp_pdb(csb->router_name.as);
	if (csb->sense) {
	    if (pdb == NULL) {
		if (!novigrp_enabled) {
		    printf("\n%% Error: IPX EIGRP is not included in this IOS build");
		} else {
		    pdb = reg_invoke_novigrp_start(csb->router_name.as);
		}
	    }
	    if (pdb == NULL) {
		printf("\n%% Error creating EIGRP routing process");
	    } else {
		if (set_mode_byname(&csb->mode, "ipx-router", MODE_VERBOSE)) {
		    csb->protocol = pdb;
		} else {
		    return;
		}
	    }
	    pdb->entered = TRUE;
	} else {
	    if (pdb == NULL) {
		printf("\n%% EIGRP not configured for AS %d",
		       csb->router_name.as);
	    } else {
		nov_router_erase(pdb);
	    }
	}
	break;
      case PROC_RIP:
	if (csb->set_to_default || csb->sense) {
	    if (!novell_rip_running) {
		csb->protocol = &novrip_pdb;
		novrip_pdbinit();
		(*novrip_pdb.restart)(&novrip_pdb);
	    }
	    if (set_mode_byname(&csb->mode, "ipx-router", MODE_VERBOSE)) {
		csb->protocol = &novrip_pdb;
	    } else {
		return;
	    }
	} else {
	    if (novell_rip_running) {
	        nov_router_erase(&novrip_pdb);
	    }
	}
	break;
      case PROC_NLSP:

	if (novell_default_route) {
	  if (strlen(csb->router_name.tag)) 
	    router_tag = csb->router_name.tag;
	  pdb = novnlsp_find_pdb(router_tag);

	  nov_nlsp_router_command(TRUE, csb->sense, csb, pdb, router_tag);

	} else {
	  printf("\n%%IPX NLSP commands not allowed, default route is disabled");
	}
	break;
      default:
	printf("\n%% Unknown protocol '%s'", csb->router_name.myname);
	break;
    }
}

/*
 * nov_get_timestamp
 *
 * ipx internal 64 bit counter. Bumps it up by 1 each time it's called.
 */
static void nov_get_timestamp (sys_timestamp *timestamp)
{
    novell_timestamp.u.value++;
    *timestamp = novell_timestamp;
}

/*
 * novell_nidb_new
 *
 * Create a new nidb; fill in common structures
 */
nidbtype *novell_nidb_new (idbtype *idb, ulong net, int enctype,
			   boolean primary, boolean old_style,
			   boolean internal_net, boolean finish,
			   boolean enctype_given)
{
    nidbtype *nidb;

    nidb = nov_get_nidb();
    if (nidb == NULL)
	return(NULL);
    if (novell_next_output_nidb == NULL)
	novell_next_output_nidb = nidb;
    if (idb != NULL) {
	idb->novellnets++;
	novell_rxtypes_update(idb);
    }
    nidb->internal_network = internal_net;
    if (internal_net) {
	/* An IDB has to be created to go with this thing... Avoids crashes */
	idb = vidb_malloc();
	if (idb == NULL) {
	    printf(nomemory);
	    free(nidb);
	    return(NULL);
	}
	/* Bring the interface UP */
	idb->hwptr->state = IDBS_UP;
	/* Node is '1' */
	ieee_zero(idb->novell_node);
	idb->novell_node[5] = 0x01;
	idb->novellnets++;
	novell_rxtypes_update(idb);
	idb->short_namestring = "Int";
    }
    nidb->idb = idb;
    nidb->up_pending = FALSE;
    nidb->node_pending = FALSE;
    nidb->novellnet = 0;
    nidb->novell_interface_up = FALSE;
    nidb->enctype = enctype;
    nidb->explicit_encap = enctype_given;

    GET_TIMESTAMP(nidb->rip_lastupdate);
    GET_TIMESTAMP(nidb->rip_revp_lastupdate);
    nov_get_timestamp(&nidb->sap_lastupdate);
    nidb->sap_revp_lastupdate = nidb->sap_lastupdate;

    nidb->adv_default_route_only = FALSE;
    nidb->trig_sap_delay = 
      ((novell_def_trig_sap_dly != NOV_OUTPUT_DELAY_DEFAULT) ? 
	novell_def_trig_sap_dly : NOV_OUTPUT_DELAY_DEFAULT);
    nidb->trig_rip_delay = 
      ((novell_def_trig_rip_dly != NOV_OUTPUT_DELAY_DEFAULT) ? 
	novell_def_trig_rip_dly : NOV_OUTPUT_DELAY_DEFAULT);
    nidb->novell_spxidletime = NOVELL_SPX_IDLE_DEFLT;
    nidb->nov_ipxwan_enabled = FALSE;
    nidb->nov_ipxwan_delay = 0;
    nidb->nov_ipxwan_internal_net =
	nidb->nov_ipxwan_external_net = 0;
    nidb->nov_ipxwan_client = FALSE;
    nidb->nov_ipxwan_static = FALSE;
    nidb->nov_ipxwan_error = IPX_IPXWAN_ERROR_CONFIG_RESET;
    nidb->novell_link_delay = NOVELL_LINK_DELAY_DEFLT;
    nidb->novell_link_throughput = NOVELL_THROUGHPUT_DEFLT;
    nidb->nov_sent_linkdown_poisons = TRUE;
    if (idb_is_lan(idb)) {
	nidb->igrp_sap_updates = FALSE;
    } else {
	nidb->igrp_sap_updates = TRUE;
    }
    nidb->novell_spoof_flag = 0;
    /*
     * Set defaults so that RIP/SAP enabled on all networks:
     */
    nidb->nov_lsp_enabled = FALSE;
    nidb->nov_lsp_isis = FALSE;
    nidb->administratively_shutdown = FALSE;
    novell_set_nidb_rip_mode(nidb, IPX_RIPSAP_CONFIG_ON);
    novell_set_nidb_sap_mode(nidb, IPX_RIPSAP_CONFIG_ON);

    /*
     * Set compression defaults
     */
    nidb->compression_enabled = FALSE;
    nidb->compression_uncompressed_bytes_out = 0;
    nidb->compression_bytes_saved = 0;
    nidb->compression_negotiated_enabled = FALSE;
    nidb->compression_slots = 0;
    nidb->compression_negotiated_type = 0;
    nidb->compression_negotiated_slots = 0;
    nidb->compression_negotiated_options = 0;
    nidb->compression_most_recently_used_slot = 0;
    nidb->compression_least_recently_used_slot = 0;
    nidb->inbound_compression_slots = 0;
    nidb->outbound_compression_slots = 0;
    nidb->compression_sent_cipx_regular = 0;
    nidb->compression_sent_cipx_compressed = 0;
    nidb->compression_sent_cipx_cinitial = 0;
    nidb->compression_sent_cipx_uinitial = 0;
    nidb->compression_sent_cipx_confirm = 0; 
    nidb->compression_sent_cipx_reject = 0;
    nidb->compression_rx_cipx_regular = 0;
    nidb->compression_rx_cipx_compressed = 0;
    nidb->compression_rx_cipx_cinitial = 0;
    nidb->compression_rx_cipx_uinitial = 0;
    nidb->compression_rx_cipx_confirm = 0;
    nidb->compression_rx_cipx_reject = 0;

    /*
     * Remember if the primary network was given with or
     * without explicit encapsulation. This information
     * will be used when the configuration is written to
     * NVM in order to keep things backwards compatible
     * with 9.1 and earlier.
     */
    nidb->old_style = old_style;
    if (primary) {
	IPX_SET_PRIMARY_NIDB(idb, nidb);
    }
    enqueue(&novidbQ, nidb);
    if (finish) {
	novell_nidb_set_network_number(nidb, net);
	novell_nidb_newstate(nidb, NOVELL_NIDB_NEW);
	if (!internal_net)
	    reg_invoke_novigrp_address_command(idb, net, TRUE);
	reg_invoke_ipx_nidb_network_changed(nidb, IPX_ROUTE_EVENT_ADD);
    }
    mgd_timer_init_leaf(&nidb->rip_update_timer, &novell_super_root_timer,
			IPX_TMR_RIP_UPD, nidb, FALSE);

    mgd_timer_init_leaf(&nidb->sap_update_timer, &novell_super_root_timer,
			IPX_TMR_SAP_UPD, nidb, FALSE);

    nidb->rip_full =
	nov_allocate_and_init_usv(IPX_UPD_RIP, nidb, baddr, 0, IPX_FULL_NEEDED,
				  IPX_DESTINATION_BROADCAST, 0, 0, NULL, 0, 0,
				  FALSE, FALSE);
    nidb->sap_full =
	nov_allocate_and_init_usv(IPX_UPD_SAP, nidb, baddr, 0, IPX_FULL_NEEDED,
				  IPX_DESTINATION_BROADCAST, NOVELL_EVERYONE,
				  0, NULL, 0, 0, FALSE, FALSE);
    nidb->rip_part =
	nov_allocate_and_init_usv(IPX_UPD_RIP, nidb, baddr, 0,
				  IPX_FULL_NOT_NEEDED,
				  IPX_DESTINATION_BROADCAST,
				  0, 0, NULL, 0, 0, FALSE, FALSE);
    
    nidb->sap_part =
	nov_allocate_and_init_usv(IPX_UPD_SAP, nidb, baddr, 0,
				  IPX_FULL_NOT_NEEDED,
				  IPX_DESTINATION_BROADCAST, NOVELL_EVERYONE,
				  0, NULL, 0, 0, FALSE, FALSE);
    
    /*
     * Note the kludge to force linkdown and reverse usv creation.
     * At the present time, the nidb flag is our only indicator.
     */

    /*nidb->doing_reverse_poisons = TRUE;*/
    nidb->rip_revp =
	nov_allocate_and_init_usv(IPX_UPD_RIP, nidb, baddr, 0, IPX_FULL_NEEDED,
				  IPX_DESTINATION_BROADCAST, 0, 0, NULL, 0, 0,
				  TRUE, FALSE);

    nidb->sap_revp =
	nov_allocate_and_init_usv(IPX_UPD_SAP, nidb, baddr, 0, IPX_FULL_NEEDED,
				  IPX_DESTINATION_BROADCAST, NOVELL_EVERYONE,
				  0, NULL, 0, 0, TRUE, FALSE);
    /*nidb->doing_reverse_poisons = FALSE;*/
    
    /*nidb->doing_linkdown_poisons = TRUE;*/
    nidb->rip_down =
	nov_allocate_and_init_usv(IPX_UPD_RIP, nidb, baddr, 0, IPX_FULL_NEEDED,
				  IPX_DESTINATION_BROADCAST, 0, 0, NULL, 0, 0,
				  FALSE, TRUE);
    nidb->sap_down =
	nov_allocate_and_init_usv(IPX_UPD_SAP, nidb, baddr, 0, IPX_FULL_NEEDED,
				  IPX_DESTINATION_BROADCAST, NOVELL_EVERYONE,
				  0, NULL, 0, 0, FALSE, TRUE);
    /*nidb->doing_linkdown_poisons = FALSE;*/

    /*
     * Any alloc failure aborts the whole deal:
     */
    if ((nidb->rip_full == NULL) || (nidb->sap_full == NULL) ||
	(nidb->rip_part == NULL) || (nidb->sap_part == NULL) ||
	(nidb->rip_down == NULL) || (nidb->sap_down == NULL) ||
	(nidb->rip_revp == NULL) || (nidb->sap_revp == NULL))
	{
	    novell_nidb_free(nidb);
	    return(NULL);
	}

    return(nidb);
}


static void novell_algorithms_command (boolean enabling)
{
    if (!novell_running || novell_shutting_down)
	return;
    novell_algorithms = enabling;
}

void novell_internal_net_command (boolean interactive, boolean enabling,
				  ulong cnet)
{
    ulong net, old_net;
    nidbtype *nidb;

    if (!novell_running || novell_shutting_down)
	return;
    net = cnet;
    if (NOV_ILLEGAL_NET(net)) {
      if (interactive)
	    printf("\n%%IPX network %x is illegal", net);
      return;
    }
    if (enabling == FALSE) {
        FOR_ALL_NIDBS(nidb) {
	  if (nov_ipxwan_on_nidb(nidb) && 
	      (nidb->nov_ipxwan_internal_net == 0L)) {
	    if (interactive)
	      printf("\nIPX: Internal Network %x currently in use on IPXWAN interface %s", net, nidb->idb->namestring);
	    return;
	  }
	}
	FOR_ALL_NIDBS(nidb) {
	    if (IPX_NIDB_IS_INTERNAL(nidb)) {
		if (nidb->novellnet == net) {
		    novell_delete_network(nidb);
		    ipx_internal_net = 0;
		    ipx_internal_network_nidb = NULL;
		    reg_invoke_ipx_internal_net_changed();
		    return;
		}
	    }
	}
	if (interactive)
	    printf("\nIPX: No internal network numbered %x", net);
	return;
    } else {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->novellnet == net) {
		if (IPX_NIDB_IS_INTERNAL(nidb)) {
		    /* Same net as before. Do nothing. */
		    return;
		} else {
		    if (interactive)
			printf("%%IPX network %x already exists on interface %s",
			       nidb->novellnet, nidb->idb->namestring);
		    return;
		}
	    }
	}
	if (novrip_find_rt(net, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_EXACT) 
	    != NULL) {
	  if (interactive)
	    printf("%%IPX network %x already exists in routing table", net);
	  return;
	}
	/* We have a new Int Net! Force lines with no internal net to reset */
	FOR_ALL_NIDBS(nidb) {
	  if (nov_ipxwan_on_nidb(nidb) &&
	      (nidb->nov_ipxwan_internal_net == 0L)) {
	    /* Force world to know it's gone and bring IPXWAN down... */
	    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	  }
	}
	FOR_ALL_NIDBS(nidb) {
	    if (IPX_NIDB_IS_INTERNAL(nidb)) {
		if (!NOVELL_NIDB_CONFIGURABLE(nidb)) {
		    if (interactive)
			printf("\nNetwork %x is being deleted, and cannot be configured at this time",
			       nidb->novellnet);
		    return;
		}
		if (nidb->novellnet != net) {
		    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
		    nov_network_finish(&novrip_pdb, nidb->novellnet,
				       FALSE);
		    old_net = nidb->novellnet;
		    novell_nidb_set_network_number(nidb, net);
		    nidb->enctype = ET_HDLC;
		    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
		    nov_network_finish(&novrip_pdb, nidb->novellnet, TRUE);
		    ipx_internal_net = net;
		    ipx_internal_network_nidb = nidb;
		}
		reg_invoke_ipx_internal_net_changed();
		return;
	    }
	}
	/*
	 * The specified primary or secondary network is new.
	 * Allocate and initialize (virtual) network descriptor:
	 */
	nidb = novell_nidb_new(NULL, net, ET_HDLC, TRUE, TRUE, TRUE,
			       TRUE, FALSE);
	if (nidb == NULL) {
	    if (interactive)
		printf("\n%%no memory to create new IPX network block");
	    return;
	}
	nov_network_finish(&novrip_pdb, nidb->novellnet, TRUE);
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	ipx_internal_net = nidb->novellnet;
	ipx_internal_network_nidb = nidb;
	reg_invoke_ipx_internal_net_changed();
    }
}

static void novell_acctthreshold_command (boolean enabling, ulong thresh)
{
    if (!novell_running || novell_shutting_down)
	return;
    ipx_acctg_info.threshold = enabling ? thresh : IPXACCTGDEFAULTLIMIT;
}

static void novell_accttransits_command (boolean enabling, ulong transits)
{
    if (!novell_running || novell_shutting_down)
	return;
    if (ipx_acctg_acc == NULL)
	return;
    ipx_acctg_acc->transits_def = ipx_acctg_acc->transits = 
	enabling ? transits : 0;
}

static void novell_sap_uses_routing_command (boolean enabling)
{
    novell_sapqueuetype *sapq;
    
    if (!novell_running || novell_shutting_down)
	return;
    if (novell_sap_uses_routing != enabling) {
	/* The world has changed. FORCE a SAP sort... */
	novell_sap_uses_routing = enabling;
	FOR_ALL_SAP_TYPES(sapq) {
	    sapq->sorted = FALSE;
	}
    }
}

static void novell_potential_pseudonode_command (boolean enabling)
{
    
    if (!novell_running || novell_shutting_down)
	return;
    if (novell_potential_pseudonode && !enabling) {
	novrip_delete_all_backup_data(0, 0);
    }
    novell_potential_pseudonode = enabling;
}

static void novell_nsqrr_command (boolean enabling)
{
    if (!novell_running || novell_shutting_down)
	return;
    nov_gns_roundrobin = enabling;
}

static void novell_perhost_command (boolean enabling)
{
    if (!novell_running || novell_shutting_down)
	return;
    nov_load_share_per_host = enabling;
    novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0, NOV_NETMASK_ALL);
}

static void novell_type20_in_command (boolean enabling)
{
    if (!novell_running || novell_shutting_down)
	return;
    novell_type20_in_checks = enabling;
}

static void novell_type20_out_command (boolean enabling)
{
    if (!novell_running || novell_shutting_down)
	return;
    novell_type20_out_checks = enabling;
}

static void novell_netbios_in_command (boolean enabling)
{
    if (!novell_running || novell_shutting_down)
	return;
    novell_netbios_in_checks = enabling;
}

static void novell_rip_default_route_command (boolean enabling)
{
    boolean old_value_enabled = novell_default_route;

    if (!novell_running || novell_shutting_down)
	return;

    if (novell_nlsp_running && !enabling) {
      printf("\n%%IPX NLSP is running, can not disable default route");
      return;
    }

    novell_default_route = enabling;
    /* If default route was off we need to remove any entry from the table
     * if it exists
     */
    if (novell_default_route && !old_value_enabled) {
      if (novrip_find_rt(NOV_RIP_DEFAULT_ROUTE, 
			 NOV_NETMASK_ALL, IPX_ROUTE_MATCH_EXACT) 
	  != NULL) {
	novrip_del_rt(NOV_RIP_DEFAULT_ROUTE, NOV_NETMASK_ALL,
		      IPX_ROUTE_DELETE_REMOVE_CONNECTED);
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				   NOV_NETMASK_ALL);
      }
    }
    /*
     * Take care of some details if disabling default route handling.
     * Invalidate the entire cache since there may be many entries
     * dependent upon the default route.
     */
    if (!novell_default_route && old_value_enabled) {
	novrip_del_rt(NOV_RIP_DEFAULT_ROUTE, NOV_NETMASK_ALL,
		      IPX_ROUTE_DELETE_REMOVE_CONNECTED);
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				   NOV_NETMASK_ALL);
	novell_default_ndb = NULL;
    }
}

/*
 * novell_sap_check_backup_queue
 * Check the backup queue of potential alternates to this server that we
 * heard about before, but ignored.
 */
void novell_sap_check_backup_queue (novell_sapentrytype *entry)
{
    backuptype *bi, *binext;
    novpdbtype *loop_pdb;
    
    /* Somebody may have better info for us */
    for (bi = entry->backup_info; (bi != NULL); bi = binext) {
	binext = bi->next;
	loop_pdb = NOV_INDEX2PDB(bi->pdbindex);
	if ((loop_pdb != NULL) && (loop_pdb->add_route)) {
	    (*loop_pdb->add_route)(loop_pdb, 0L, 0L,
				   bi->backup_context_info,
				   bi->route_type);
	}
	free(bi);
    }
    entry->backup_info = NULL;
}

/*
 * nov_insert_sap_placeholder
 *
 */
static void nov_insert_sap_placeholder (novell_sapentrytype *sap,
					novell_sapentrytype *ph)
{
    if ((sap == NULL) || ( ph == NULL) ||
	(sap->type != IPX_SAP_ACTIVE) ||
	( ph->type != IPX_SAP_PLACEHOLDER) ||
	(ph->next_newest != NULL) ||
	(ph->prev_newest != NULL) ||
	(ph == novell_newest_servers))
	{
	    return;
	}

    ph->next_newest = sap;
    ph->prev_newest = sap->prev_newest;
    if (sap->prev_newest != NULL) {
	sap->prev_newest->next_newest = ph;
    } else {
	novell_newest_servers = ph;
    }
    sap->prev_newest = ph;
}

/*
 * nov_delete_sap_placeholder
 *
 */
static novell_sapentrytype
    *nov_delete_sap_placeholder (novell_sapentrytype *ph)
{
    novell_sapentrytype *sap;

    if (ph->type != IPX_SAP_PLACEHOLDER) {
	return(NULL);
    }

    if ((ph->next_newest == NULL) && (ph->prev_newest == NULL)) {
	return(ph);
    }

    /*
     * Prepare to return next active node:
     */
    sap = ph->next_newest;

    /*
     * Delete placeholder:
     */
    if (ph->next_newest != NULL) {
	ph->next_newest->prev_newest = ph->prev_newest;
    }

    if (ph->prev_newest != NULL) {
	ph->prev_newest->next_newest = ph->next_newest;
    }

    /*
     * Update listhead if appropriate:
     */
    if (novell_newest_servers == ph) {
	novell_newest_servers = ph->next_newest;
    }

    ph->next_newest = NULL;
    ph->prev_newest = NULL;

    /*
     * Slide along past possible placeholders to return next active sap:
     */
    while ((sap != NULL) && (sap->type == IPX_SAP_PLACEHOLDER)) {

	if ((sap->type != IPX_SAP_ACTIVE) &&
	    (sap->type != IPX_SAP_PLACEHOLDER))
	    {
		return(NULL);
	    }

	sap = sap->next_newest;
    }

    /*
     * Return next active sap (if any):
     */
    return(sap);
}

/*
 * nov_get_first_sap
 *
 */
static novell_sapentrytype *nov_get_first_sap (void)
{
    novell_sapentrytype *sap;

    /*
     * Slide along past possible placeholders to return first active sap:
     */

    sap = novell_newest_servers;

    while ((sap != NULL) && (sap->type != IPX_SAP_ACTIVE)) {

	if (sap->type != IPX_SAP_PLACEHOLDER) {
	    return(NULL);
	}

	sap = sap->next_newest;
    }

    return(sap);
}

/*
 * nov_get_next_sap
 *
 */
static novell_sapentrytype *nov_get_next_sap (novell_sapentrytype *current)
{
    novell_sapentrytype *sap;

    if (current == NULL) {
	return(NULL);
    }

    /*
     * Slide along past possible placeholders to return next active sap:
     */
    sap = current->next_newest;

    while ((sap != NULL) && (sap->type == IPX_SAP_PLACEHOLDER)) {
	sap = sap->next_newest;
    }

    if ((sap != NULL) && (sap->type != IPX_SAP_ACTIVE)) {
	return(NULL);
    }

    return(sap);
}

/*
 * nov_common_sap_entry_init
 *
 * Common, consistent SAP entry initialization for all allocators.
 */
static void nov_common_sap_entry_init (novell_sapentrytype *newsap)
{
    newsap->options = 0;
    newsap->nidb_changed = FALSE;
    newsap->deleted_static = FALSE;
    newsap->type = IPX_SAP_ACTIVE;
}
/*
 * allocate_sap_entry
 *
 */
static novell_sapentrytype *nov_allocate_sap_entry (void)
{
    novell_sapentrytype *newsap;

    newsap = malloc_named(sizeof(novell_sapentrytype), "IPX SAP");
    if (newsap == NULL) {
	return(NULL);
    }
    nov_common_sap_entry_init(newsap);
    return(newsap);
}

/*
 * nov_sap_malloc_placeholder
 *
 */
static novell_sapentrytype *nov_sap_malloc_placeholder (void)
{
    novell_sapentrytype *newsap;

    newsap = malloc_named(sizeof(novell_sapentrytype), "IPX SAP PH");
    if (newsap == NULL) {
	return(NULL);
    }
    nov_common_sap_entry_init(newsap);
    newsap->type = IPX_SAP_PLACEHOLDER;

    return(newsap);
}


/*
 * nov_mark_sap_newest
 *
 * Mark a SAP entry as newest, with no other side effects.
 */
static void nov_mark_sap_newest (novell_sapentrytype *entry)
{
    if (entry == NULL) {
	return;
    }

    nov_get_timestamp(&entry->sap_changed);

    if (entry != novell_newest_servers) {
	/* Move this entry out of the current list */
	if (entry->prev_newest != NULL) {
	    entry->prev_newest->next_newest = entry->next_newest;
	}
	if (entry->next_newest != NULL) {
	    entry->next_newest->prev_newest = entry->prev_newest;
	}
	/* Mark this as the newest server -- no prev, next is old newest */
	entry->prev_newest = NULL;
	entry->next_newest = novell_newest_servers;
	/* The old newest, if any, points to us */
	if (novell_newest_servers != NULL) {
	    novell_newest_servers->prev_newest = entry;
	}
	/* Finally, the newest_servers pointer should point to this */
	novell_newest_servers = entry;
    }
}

/*
 * mark_sap_changed
 *
 * Mark a SAP entry as changed. Call with entry == NULL if all you want to
 * do is indicate a flash update should be sent...
 */
static void mark_sap_changed (novell_sapentrytype *entry)
{

    if (entry != NULL) {
	
	/*
	 * Mark entry as newest and move to head of list:
	 */
	nov_mark_sap_newest(entry);
	
    }

    /*
     * Wake somebody up to posit a flash update:
     */
    process_set_boolean(novell_sap_part_wb, TRUE);
    
}

/*
 * novell_mark_sap_changed
 *
 * Mark a SAP entry as changed.
 *
 */
static void novell_mark_sap_changed (novell_sapentrytype *entry)
{
    novpdbtype *pdb;
    int event;

    if (entry != NULL) {
	/*
	 * Do not force flash if this entry has already been reverse
	 * poisoned. This is to avoid sending unreachable twice.
	 */
	if (!entry->reverse_poisoned) {
	    mark_sap_changed(entry);
	}
	if (entry->poisoned)
	    event = ROUTE_DOWN;
	else
	    event = ROUTE_UP;
	pdb = nov_proctable[entry->novpdbindex];
	nov_service_redist(pdb, entry, event);
    }
}

/*
 * novell_mark_srdb_changed
 * Mark an SRDB block as having changed. Call all PDBs that have a redist
 * back into this, and report this event
 */
void novell_mark_srdb_changed (novell_sapentrytype *entry,
			      novell_sapentry_rdb *srdb,
			      ulong flags,
			      enum IPX_ROUTE_EVENT event)
{
    novpdbtype *pdb;
    backuptype backup;
    novell_sapentry_rdb *srdb_loop;

    /*
     * See if the nidb_changed field needs to be set... This is true if the
     * SRDB being added/deleted is on an nidb that is not on the entry's new
     * SRDB list (ignoring ourselves, if any). We don't need to do this if
     * this is the last entry being deleted.
     */
    if ((entry->paths != NULL) &&
	((event == IPX_ROUTE_EVENT_ADD) ||
	 (event == IPX_ROUTE_EVENT_DELETED) ||
	 (event == IPX_ROUTE_EVENT_PUSHED_OUT))) {
	for (srdb_loop = entry->paths; srdb_loop != NULL;
	     srdb_loop = srdb_loop->next) {
	    if ((srdb_loop != srdb) && (srdb_loop->nidb == srdb->nidb))
	      break;
	}
	if (srdb_loop == NULL) {
	    entry->nidb_changed = TRUE;
	    novell_mark_sap_changed(entry);
	}
    }

    /* Build backup info if appropriate */
    if (event == IPX_ROUTE_EVENT_PUSHED_OUT) {
	backup.pdbindex = entry->novpdbindex;
	backup.backup_context_info = ((srdb->lsp_linkage.lsp_index << 16) |
				      srdb->lsp_linkage.lsp_version);
	backup.route_type = RTYPE_L1;
	ipx_insert_backup_info(&(entry->backup_info), &backup,
			       NOV_INDEX2PDB(entry->novpdbindex), FALSE);
    }
    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->srdb_changed_callback) {
	    (*pdb->srdb_changed_callback)(pdb, entry, srdb, event, flags);
	}
    }
}

/*
 * novell_posit_backup_servers_needed
 *
 * Ask for any backup servers that are available... Do it now if not done in
 * past interval
 *
 * This is for EIGRP. NLSP does not need this, because on any change
 * of route/server topology which would cause this to be called, the
 * full list of servers and routes are re-fed to the routing and server
 * tables, with the servers only coming in after all of the routes
 * come in. If there are any changes in routes that would make services
 * visible that were previously not visible, the only time this rule will
 * NOT work is if a route NOT LEARNED VIA NLSP brought us in a service that
 * IS learned via NLSP. Not bloody likely, and not worth the CPU overload
 * that would occur by trying to take it in. But, if we see it in the field,
 * the easy fix is to just make this sucker make NLSP think an SPF run is
 * needed...
 */
void novell_posit_backup_servers_needed (void)
{

    novell_backup_servers_needed = TRUE;
    if (!TIMER_RUNNING(novell_backup_servers_timer)) {
	GET_TIMESTAMP(novell_backup_servers_timer);
    }
}

void novell_paths_command (boolean interactive, boolean enabling, int maxpaths)
{
    int novell_new_maxpaths;
    int i;
    novndbtype *ndb;
    novrdbtype *rdb;
    novpdbtype *pdb;
    novell_sapqueuetype *sapq;
    novell_sapentrytype *entry;
    novell_sapentry_rdb *srdb;
    
    if (!novell_running || novell_shutting_down)
	return;
    if (enabling) {
	novell_new_maxpaths = novell_cfg_maxpaths = maxpaths;
    } else {
	novell_new_maxpaths = novell_cfg_maxpaths = RIP_MAXPATHS;
    }
    /*
     * Do not exceed NLSP max if configured:
     */
    if (novell_nlsp_running > 0) {
	novell_new_maxpaths = min(novell_new_maxpaths, NOVELL_NLSP_MAXPATHS);
	if (enabling && (novell_new_maxpaths < maxpaths))
	    if (interactive)
		printf("\n%%Active path count reduced to %d because "
		       "NLSP is configured", novell_new_maxpaths);
    }
    /*
     * Invalidate cbus cache if going from 1 path to many ...
     * this because arcane details of cbus cache management depend
     * on the value of maxpaths.
     */
    if ((novell_new_maxpaths > 1) && (novell_maxpaths == 1)) {
	if (novell_auto_sw_active())
	    reg_invoke_auto_sw_cache_init(LINK_NOVELL);
    }
    /*
     * If the new maximum number of paths is less than the old,
     * trim the number of IGRP paths in the routing table. RIP
     * routes are trimmed in novrip_ager(). NLSP routes are not
     * trimmed. If the new number  of paths is greater, DUAL will
     * give us more paths. DUAL is called in either case so it can
     * adjust it's succesor count.
     */
    if (novell_new_maxpaths != novell_maxpaths) {
	/*
	 * Blow away the cache whenever maxpaths changes ...
	 * this so rare as to not merit any optimization.
	 */
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				   NOV_NETMASK_ALL);
	novell_maxpaths = novell_new_maxpaths;
	for (i = 0; i < NOV_NETHASHLEN; i++) {
	    for (ndb = (*novell_table)[i]; ndb; ndb = ndb->next) {
		if (ndb->flags & NET_IGRP) {
		    if (ndb->paths > novell_maxpaths) {
			while (ndb->paths > novell_new_maxpaths) {
			    ndb->paths--;
			    rdb = ndb->path_list;
			    ndb->path_list = rdb->next;
			    ndb->nextpath = ndb->path_list;
			    novell_mark_rdb_changed(ndb, rdb, ndb->flags,
						    IPX_ROUTE_EVENT_PUSHED_OUT);
			    novrdb_free(rdb);
			}
		    }
		    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb;
			 pdb = pdb->next) {
			if (pdb->add_route)
			    (*pdb->add_route)(pdb, ndb->dstnet, ndb->rt_mask, 
					      0L, 0L);
		    }
		}
	    }
	}

	/* Do the same thing for the patricia tree */
	rn_walktree_blocking(nov_rnhead->rnh_treetop,
			     patricia_paths_command,
			     novell_new_maxpaths);

 	FOR_ALL_SAPS(entry, sapq) {
	    if (entry->flags & NET_IGRP) {
		if (entry->pathcount > novell_maxpaths) {
		    while (entry->pathcount > novell_new_maxpaths) {
			entry->pathcount--;
			srdb = entry->paths;
			entry->paths = srdb->next;
			novell_mark_srdb_changed(entry, srdb, entry->flags,
						 IPX_ROUTE_EVENT_PUSHED_OUT);
			novsrdb_free(srdb);
		    }
		}
	    }
	}
	novell_posit_backup_servers_needed();
    }
}    

static void novell_sapqueuemax_command (boolean enabling, int sapqmax)
{
    if (!novell_running || novell_shutting_down)
	return;
    if (enabling) {
	novell_sapqueuemax = sapqmax;
    } else {
	novell_sapqueuemax = 0;
    }
}

static void nov_spx_session_command (boolean enabling, int spxentry)
{
    if (!novell_running || novell_shutting_down)
	return;
    if (enabling) {
	nov_spx_idle_time = spxentry * ONEMIN;
    } else {
	nov_spx_idle_time = SPX_DEFAULT_IDLE_TIME * ONEMIN;
    }
}
static void nov_spx_clear_command (boolean enabling, int spxclear)
{
    if (!novell_running || novell_shutting_down)
	return;
    if (enabling) {
	nov_spx_clear_time = spxclear * ONEHOUR;
    } else {
	nov_spx_clear_time = SPX_DEFAULT_CLEAR_TIME * ONEHOUR;
    }
}

static void novell_gns_response_delay_command (boolean enabling, int grd)
{
    idbtype *idb;
    int old_gns_delay;
    
    /* If novell not running or no change to the defaul GNS delay, exit */
    if (!novell_running || novell_shutting_down ||
	(grd == novell_gns_response_delay))
	return;

    /* save the old delay so we may compare all configured interfaces */
    old_gns_delay = novell_gns_response_delay;  

    novell_gns_response_delay = enabling ? grd :
	NOVELL_DEFAULT_GNS_RESPONSE_DELAY;
    FOR_ALL_SWIDBS(idb) {
      if (idb->nov_gns_delay == old_gns_delay)
	idb->nov_gns_delay = novell_gns_response_delay;
    }
}

static void novell_rcache_inactive_age_rate_command (boolean enabling, ulong limit)
{
    /* If novell not running or no change, exit */
    if (!novell_running || novell_shutting_down)
	return;
    
    if (enabling) {
	novcache_inactive_age_rate = limit;
    } else {
	novcache_inactive_age_rate = NOVRCACHE_DEFAULT_INACTIVE_AGE_RATE;
    }
}

static void novell_rcache_max_size_command (boolean enabling, ulong maxsize)
{
    /* If novell not running or no change, exit */
    if (!novell_running || novell_shutting_down)
	return;

    if (enabling) {
	novcache_max_size = maxsize;
    } else {
	novcache_max_size = NOVRCACHE_DEFAULT_MAX_SIZE;
    }
}

static void novell_rcache_max_inactive_age_command (boolean enabling, ulong agelimit)
{
    /* If novell not running or no change, exit */
    if (!novell_running || novell_shutting_down)
	return;

    if (enabling) {
	novcache_max_inactive_age = agelimit;
    } else {
	novcache_max_inactive_age = NOVRCACHE_DEFAULT_MAX_INACTIVE_AGE;
    }
}

static void novell_rcache_update_age_rate_command (boolean enabling, ulong limit)
{
    /* If novell not running or no change, exit */
    if (!novell_running || novell_shutting_down)
	return;
    
    if (enabling) {
	novcache_update_age_rate = limit;
    } else {
	novcache_update_age_rate = NOVRCACHE_DEFAULT_UPDATE_AGE_RATE;
    }
}

static void novell_rcache_max_update_age_command (boolean enabling, ulong agelimit)
{
    /* If novell not running or no change, exit */
    if (!novell_running || novell_shutting_down)
	return;

    if (enabling) {
	novcache_max_update_age = (agelimit * ONEMIN);
    } else {
	novcache_max_update_age = (NOVRCACHE_DEFAULT_MAX_UPDATE_AGE * ONEMIN);
    }
}

static void novell_backup_server_interval_command (boolean enabling, int bsi)
{
    if (!novell_running || novell_shutting_down)
	return;
    novell_backup_server_interval = enabling ? bsi :
	NOVELL_DEFAULT_BACKUP_SERVER_INTERVAL;
}

/*
 * novell_ping_default_command
 * set the default ping type to use as either cisco or novell
 */
static void
novell_ping_default_command (boolean enabling, enum IPX_ECHO_TYPE echo_type)
{

if (!novell_running || novell_shutting_down)
  return;
  if (enabling) {
    novell_ping_deflt = echo_type;
  } else {
    novell_ping_deflt = IPX_ECHO_TYPE_CISCO;
  }
}


/*
 * nov_route_command
 * Add a static route
 */

void
nov_route_command (boolean interactive,
		   boolean enabling, int cnet, int cnet2, uchar *chost2,
		   int ticks, int hops,
		   int is_idb, idbtype *route_idb,
		   boolean floating, ulong netmask, int is_summary,
		   enum IPX_STATIC_DEFINE_TYPE nvgen_option)
{
    register nidbtype *nidb;
    uchar host[IEEEBYTES], *rinp;
    ulong dstnet, net, i;
    backuptype *binfo, *old_backup_info;
    novndbtype *old_route;

    binfo = old_backup_info = NULL;
    if (!is_summary)
	netmask = NOV_NETMASK_ALL;
    
    if (hops < NOVELL_DEFAULT_CONNECTED_HOPS) {
      hops = NOVELL_DEFAULT_CONNECTED_HOPS;
    }
    if (hops > novell_max_hops) {
	if (interactive) {
	    printf("\n%u Exceeds maximum-hop reachable value of %u",
		   hops, novell_max_hops);
	}
    }
    /*
     * No implicit encap unless "novell encap" issued
     */
    implicit_idb = NULL;
    implicit_enctype = ET_NULL;
    /* Zero, All Ones are illegal always */
    if (NOV_ILLEGAL_NET(cnet) && !ZERO_PREFIX(cnet, netmask)) {
      if (interactive)
	printf("\n%%IPX network %x is illegal", cnet);
      return;
    }
    if ((cnet & netmask) != cnet) {
	if (interactive)
	    printf("\n%% IPX network %x is inconsistent with mask %x", cnet,
		   netmask);
	return;
    }
    if (!ipx_validate_contiguous_mask(netmask)) {
	printf("\n%% Non-contiguous mask %8x is not supported", netmask);
	return;
    }

    dstnet = cnet;
    if (!enabling) {
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				   NOV_NETMASK_ALL);
	novrip_del_rt(dstnet, netmask, IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	nov_del_floating(dstnet, netmask);
	return;
    }
    net = cnet2;
    if (chost2 != NULL)
      ieee_copy(chost2, host);

    if (!is_idb) {
	/*
	 * Disallow if local interface (internal net is OK):
	 */
	FOR_ALL_NIDBS(nidb) {
	    /*
	     * Skip over internal net:
	     */
	    if (nidb->novellnet == ipx_internal_net) {
		continue;
	    }
	    if (nidb->novellnet == net) {
		if (ieee_equal(nidb->idb->novell_node, host)) {
		    if (interactive)
			printf(nov_badaddr,  "can't be local host");
		    return;
		}
		break;
	    }
	}
	/*
	 * Not local host ... make sure it's a local network:
	 */
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->novellnet == net) {
		break;
	    }
	}
	if (!nidb) {
	    if (interactive)
	      printf(nov_badaddr, "must be on connected network");
	    return;
	}
	/* IPXWAN derived MAC address might have multicast bit set */
	if (!nov_ipxwan_on_nidb(nidb) &&
	    nov_multicast_addr(nidb->idb->hwptr, host[0])) {
	    if (interactive)
	      printf(nov_badaddr, "multicast not permitted");
	    return;
	}
    } else {
	nidb = IPX_PRIMARY_NIDB(route_idb);
	if (!nidb) {
	    if (interactive) {
		printf("Interface %s is not running IPX",
		       route_idb->namestring);
	    }
	    return;
	}
	if (!nidb->nov_ipxwan_enabled) {
	    if (interactive) {
		printf("Interface %s is not enabled for IPXWAN",
		       route_idb->namestring);
	    }
	    return;
	}
	if (NOVELL_NIDB_IDB_UP(nidb)) {
	    rinp = (uchar *) &(nidb->nov_ipxwan_remote_internal_net);
	    net = nidb->novellnet;
	    for (i = 0; i < 4; i++) {
	        host[i] = *rinp++;
	    }
	    host[4] = host[5] = 0;
	}
    }
    novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
			       NOV_NETMASK_ALL);
    if (ticks < NOVELL_DEFAULT_CONNECTED_TICKS) {
      ticks = nidb->idb->novell_delay;
    }
    if (floating) {
	/* Delete existing floating static from floating table... */
	nov_del_floating(dstnet, netmask);
	/* Replace any old FLOATING STATIC routes with new one... */
        if ((old_route = novrip_find_rt(dstnet, netmask, 
					IPX_ROUTE_MATCH_EXACT)) != NULL) {
	    if (old_route->flags & NET_FLOATING_STATIC) {
		novrip_del_rt(dstnet, netmask, 
			      IPX_ROUTE_DELETE_RETAIN_CONNECTED);
	    }
	}
        novrip_add_rt(&novfstatic_pdb, nidb, dstnet, netmask, net, host,
		      ticks, hops,
		      (NET_FLOATING_STATIC | NET_STATIC),
		      IPX_ROUTE_ADD_NORMAL,  nvgen_option,
		      (is_idb ? IPX_STATIC_TYPE_INTERFACE :
		       IPX_STATIC_TYPE_HOST),
		      IPX_FLOATING_STATIC_EXISTS_NO);
    } else {
        if ((old_route = novrip_find_rt(dstnet, netmask, 
					IPX_ROUTE_MATCH_EXACT)) != NULL) {
	    ipx_ndb_self_backup(old_route);
	    old_backup_info = old_route->backup_info;
	    old_route->backup_info = NULL;
        }
        novrip_del_rt(dstnet, netmask, IPX_ROUTE_DELETE_RETAIN_CONNECTED);
        if ((old_route = novrip_find_rt(dstnet, netmask, 
					IPX_ROUTE_MATCH_EXACT)) != NULL) {
	    old_route->backup_info = old_backup_info;
	    return;
        }
        novrip_add_rt(&novstatic_pdb, nidb, dstnet, netmask, net, host,
		      ticks, hops,
		      NET_STATIC, IPX_ROUTE_ADD_NORMAL, nvgen_option,
		      (is_idb ? IPX_STATIC_TYPE_INTERFACE :
		       IPX_STATIC_TYPE_HOST), 
		      IPX_FLOATING_STATIC_EXISTS_NO);
        if ((old_route = novrip_find_rt(dstnet, netmask, 
					IPX_ROUTE_MATCH_EXACT)) != NULL) {
	    nov_static_route_count++;
	    old_route->backup_info = old_backup_info;
        } else {
	    ipx_delete_backup_list(old_backup_info);
	}
    }
}

/*
 * nov_sap_distance_compare
 * Return TRUE if sap1 is in the 'howto' comparison relation for distance to
 * sap2.
 * 'howto' is one of NOV_SAP_CMP_{GT, GE, EQ, NE, LE, LT}
 *
 * IF LOGIC IN THIS FUNCTION IS CHANGED, MAKE SURE THAT THE LOGIC IS
 * APPROPRIATELY CHANGED IN novell_sap_compare_lt AND novell_sap_compare_eq
 * AS WELL!!
 */
static boolean nov_sap_distance_compare (novell_serveridtype *sap1,
				  ulong rtdelay1, ushort rthops1,
				  ulong rtflags1, ulong sap_flags1,
				  novell_serveridtype *sap2,
				  ulong rtdelay2, ushort rthops2,
				  ulong rtflags2, ulong sap_flags2,
				  int howto)
{
    ulong metric1, metric2, delay1, delay2, hops1, hops2;
    
    metric1 = metric2 = 0;
    if ((!sap1) || (!sap2)) {
	return(FALSE);
    }
    /* If one of the SAP distances is INFINITY, it is always worse... */
    if ((sap1->nethops >= novell_infinity) ||
	(sap2->nethops >= novell_infinity)) {
	delay1 = delay2 = 0;
	hops1 = sap1->nethops;
	hops2 = sap2->nethops;
    /* Different protocols compare route hops */
    } else if ((rtflags1 & (NET_IGRP | NET_RIP)) !=
	       (rtflags2 & (NET_IGRP | NET_RIP))) {
	delay1 = delay2 = 0;
	hops1 = rthops1;
	hops2 = rthops2;
    /* If Novell Algorithms, use the complex method. If not, use SAP hops */
    } else if (!novell_sap_uses_routing) {
	delay1 = delay2 = 0;
	hops1 = sap1->nethops;
	hops2 = sap2->nethops;
    } else {
	/* Find the various delay and hopcount values */
	delay1 = rtdelay1;
	hops1 = rthops1;
	delay2 = rtdelay2;
	hops2 = rthops2;
    }
    /*
     * Choose delay as metric unless equal, then choose hopcount of route
     * If these are both equal, use 'nethops' in the server entry itself
     */
    if (delay1 != delay2) {
	metric1 = delay1;
	metric2 = delay2;
    } else if (hops1 != hops2) {
	metric1 = hops1;
	metric2 = hops2;
    } else {
	metric1 = sap1->nethops;
	metric2 = sap2->nethops;
    }
    /* Here is where we favor IGRP over RIP for equal cost SAPs */
    /*
     * Making RIP superior to NLSP allows us to accept equal cost
     * services from SAP to become NLSP XSERVICE's.
     */
    if (metric1 == metric2) {
	if (sap_flags1 != sap_flags2) {
	    /* A simple comparison table... */
	    switch (NOVELL_SAP_SOURCE(sap_flags1)) {
	      case NET_STATIC:	metric1 = 0;	break;
	      case NET_IGRP:	metric1 = 1;	break;
	      case NET_RIP:	metric1 = 2;    break;
	      case NET_NLSP:	metric1 = 3;    break;
	      default:		metric1 = 4;	break;
	    }
	    switch (NOVELL_SAP_SOURCE(sap_flags2)) {
	      case NET_STATIC:	metric2 = 0;	break;
	      case NET_IGRP:	metric2 = 1;	break;
	      case NET_RIP:	metric2 = 2;    break;
	      case NET_NLSP:	metric2 = 3;    break;
	      default:		metric2 = 4;	break;
	    }
	}
    }

    /* Now compare according to 'howto' and return result */
    switch (howto) {
      case NOV_SAP_CMP_GT:
	return(metric1 > metric2);
	break;
      case NOV_SAP_CMP_GE:
	return(metric1 >= metric2);
	break;
      case NOV_SAP_CMP_EQ:
	return(metric1 == metric2);
	break;
      case NOV_SAP_CMP_NE:
	return(metric1 != metric2);
	break;
      case NOV_SAP_CMP_LE:
	return(metric1 <= metric2);
	break;
      case NOV_SAP_CMP_LT:
	return(metric1 < metric2);
	break;
      default:
	return(FALSE);
    }
}

/*
 * nov_sap_hash
 *
 * Compute a hashtable entry based on the name of the server
 */
int nov_sap_hash (novell_serveridtype *server)
{
    int i, result;
    
    for (i = result = 0; i < NOVELL_SERVER_NAME_LEN; i++) {
	if (server->server_name[i] == '\0')
	    break;
	result = (result << 6) + (uchar) server->server_name[i];
	result = result % NOVELL_SAP_HASHLEN;
    }
    return(result);
}
/*
 * nov_find_sap_name_entry
 *
 * Find a SAP with the name given in the sapq passed into this function
 */
static novell_sapentrytype *nov_find_sap_name_entry (novell_sapqueuetype *sapq,
					     novell_serveridtype *server)
{
    int i;
    novell_sapentrytype *entry;
    
    i = nov_sap_hash(server);
    for (entry = sapq->sap_hashtable[i];
	 entry;
	 entry = entry->hash_next) {
	if (strcmp(entry->server.server_name, server->server_name) == 0)
	  break;
    }
    return(entry);
}

/*
 * nov_sap_delete_net_hash
 *
 * Delete the current server from the per-network hashtable of servers.
 */
static void nov_sap_delete_net_hash (novell_sapentrytype *entry)
{
    int i;
    novell_sapentrytype *hash, *prev;

    i = NOV_NETHASH(entry->server.dnet);
    for (prev = NULL, hash = (*novell_sap_route_hashtable)[i];
	 hash;
	 prev = hash, hash = hash->net_hash_next) {
	if (hash == entry) {
	    if (prev)
	      prev->net_hash_next = hash->net_hash_next;
	    else
	      (*novell_sap_route_hashtable)[i] = hash->net_hash_next;
	    break;
	}
    }
    /* It's going away... take it out of the newest list */
    if (novell_newest_servers == entry) {
	novell_newest_servers = entry->next_newest;
    }
    if (entry->prev_newest != NULL) {
	entry->prev_newest->next_newest = entry->next_newest;
    }
    if (entry->next_newest != NULL) {
	entry->next_newest->prev_newest = entry->prev_newest;
    }
    if (novell_server_count > 0) {
	novell_server_count--;
    }
}

/*
 * nov_sap_free
 *
 * Free a Novell SAP entry. This will set the 'deleted' bit in the structure
 * indicating it is invalid. This is so any simultaneous reader of the entry
 * using 'mem_lock' will ignore it.
 */
static void nov_sap_free (novell_sapqueuetype *sapq, novell_sapentrytype *entry,
		   enum IPX_SAP_FREE free_action)
{
    int i;
    novell_sapentrytype *hash, *prev;
    novell_sapentry_rdb *srdb, *srdb_next;
    backuptype *bi, *binext;
    
    /* Delete the entry from the sapq's hash table */
    if (sapq != NULL) {
	i = nov_sap_hash(&(entry->server));
	for (prev = NULL, hash = sapq->sap_hashtable[i];
	     hash;
	     prev = hash, hash = hash->hash_next) {
	    if (hash == entry) {
		if (sapq->sap_count > 0)
		    sapq->sap_count--;
		if (prev)
		  prev->hash_next = hash->hash_next;
		else
		  sapq->sap_hashtable[i] = hash->hash_next;
		break;
	    }
	}
    }
    nov_sap_delete_net_hash(entry);
    /*
     * The backup server stuff should have been gone through before, but
     * why not...
     */
    srdb = entry->paths;
    entry->paths = NULL;
    /*
     * Note that the service is removed off of all paths before SRDB_CHANGED
     * is called. This avoids needless redistribution of the other paths we
     * are just going to delete anyway.
     */
    entry->pathcount = 0;
    if (free_action == IPX_SAP_FREE_LOOKUP_BACKUPS) {
	novell_posit_backup_servers_needed();
	novell_sap_check_backup_queue(entry);
    }
    for (; srdb != NULL; srdb = srdb_next) {
	srdb_next = srdb->next;
	novell_mark_srdb_changed(entry, srdb, entry->flags,
				 IPX_ROUTE_EVENT_DELETED);
	novsrdb_free(srdb);
    }
    
    /* Free this stuff. We don't want it now, as we asked above */
    for (bi = entry->backup_info; (bi != NULL); bi = binext) {
	binext = bi->next;
	free(bi);
    }
    entry->backup_info = NULL;
    free(entry);
}

/*
 * nov_sap_insert_net_hash
 *
 * Place the server given by 'entry' into the hash table list associated
 * with the network the entry is located on. Also, keeps track of the
 * total server count. 
 */
static void nov_sap_insert_net_hash (novell_sapentrytype *entry)
{
    int i;

    i = NOV_NETHASH(entry->server.dnet);
    entry->net_hash_next = (*novell_sap_route_hashtable)[i];
    (*novell_sap_route_hashtable)[i] = entry;
    novell_server_count++;
    /* Only 'mark_changed' can insert into the changed list... */
    entry->next_newest = entry->prev_newest = NULL;
}

/*
 * nov_sap_set_new_dnet
 *
 * Set a new destination network on the server given as 'entry'. This
 * requires us to rehash it by removing it from the chain of servers
 * associated with the old network, and inserting it into the chain
 * of servers associated with the new network.
 */
static void nov_sap_set_new_dnet (novell_sapentrytype *entry, ulong new_dnet)
{
    nov_sap_delete_net_hash(entry);
    entry->server.dnet = new_dnet;
    nov_sap_insert_net_hash(entry);
}

/*
 * novell_place_in_sap_list
 *
 * Place the entry in appropriate the sapq list. Do not bother with
 * sorting other than to mark the sapq as unsorted.
 *
 */
static void novell_place_in_sap_list (novell_sapqueuetype *sapq,
  				      novell_sapentrytype *entry,
 				      enum IPX_SAP_INSERT hashtable_action,
 				      boolean sort_requested)
{
    novell_sapentrytype *cur;
    int i;
    
    /*
     * Place it in the queue. If a sort was requested AND
     * the queue is sorted, sort it in appropriately. If
     * not, just add it in the quickest way and make sure
     * to mark the queue appropriately.
     */
    cur = (novell_sapentrytype *) sapq->saps.flink;
    if (sort_requested && sapq->sorted) {
 	while (cur != (novell_sapentrytype *)&sapq->saps) {
 	    if (nov_sap_distance_compare(&(entry->server), entry->route_delay,
					 entry->route_hops, entry->route_flags,
 					 entry->flags,
 					 &(cur->server), cur->route_delay,
 					 cur->route_hops, cur->route_flags,
 					 cur->flags, NOV_SAP_CMP_LE))
 		break;
 	    cur = (novell_sapentrytype *) cur->saps.flink;
	}
    } else {
 	sapq->sorted = FALSE;
    }
    lw_insert(&entry->saps, cur->saps.blink);
    
    /* Now, place it in the SAP name hash table for this type */
    if (hashtable_action == IPX_SAP_INSERT_HASH_NEEDED) {	
	i = nov_sap_hash(&(entry->server));
	sapq->sap_count++;
	entry->hash_next = sapq->sap_hashtable[i];
	sapq->sap_hashtable[i] = entry;
	nov_sap_insert_net_hash(entry);
    }
    entry->sapq = sapq;
    /* Now, mark it as changed */
    novell_mark_sap_changed(entry);
}

/*
 * nov_sap_command
 * Add a static sap.  Format of command is:
 * novell sap <type> <service-name> <net.address> <socket> <hop count>
 *
 * THIS FUNCTION MUST RUN TO COMPLETION WITHOUT ANY PROCESS BREAKS!!!
 */

void
nov_sap_command (boolean interactive, boolean enabling, uchar *string1,
		 int int1, int net1, uchar *host1, int int2, int int3,
		 enum IPX_STATIC_DEFINE_TYPE nvgen_option)
{
    novell_sapentrytype *newsap;
    novndbtype *rip_entry;
    register nidbtype *nidb;
    ulong net, route_delay;
    ushort route_hops;
    ulong route_flags;
    uchar host[IEEEBYTES];
    novell_sapqueuetype *sapq;
    novell_sapentrytype *match;
    uchar new_server_name[NOVELL_SERVER_NAME_LEN];
    backuptype *binfo, *old_backup_info;
    novell_sapentry_rdb *newsrdb;

    binfo = old_backup_info = NULL;

    memset(new_server_name, 0, NOVELL_SERVER_NAME_LEN);
    sstrncpy(new_server_name, string1, NOVELL_SERVER_NAME_LEN);

    implicit_idb = NULL;
    implicit_enctype = ET_NULL;

    /*
     * If "no" form of the command was entered, see if its in the table,
     * if so, delete it and return, otherwise just return quietly. 
     */
    if (!enabling) {
	sapq = novell_findsapqueue(int1,
				   IPX_SAP_QUEUE_LOOKUP_MUST_EXIST);
	if (sapq) {
	    FOR_ALL_SAPS_OF_TYPE(match, sapq) {
		if (strcmp(new_server_name, match->server.server_name) == 0) {
		    novell_sap_poison(match);
		    match->deleted_static = TRUE;
		    break;
		}
	    }
	}
	return;
    }

    /* Get destination address */
    net = net1;
    ieee_copy(host1, (char *)host);

    /*
     * Always disallow the broadcast address:
     */
    if (ieee_equal(host, novell_allhosts)) {
	printf(nov_badaddr, "broadcast not permitted");
	return;
    }

    /*
     * Disallow if local interface (internal net is OK):
     */
    FOR_ALL_NIDBS(nidb) {
	/*
	 * Skip over internal net:
	 */
	if (nidb->novellnet == ipx_internal_net) {
	    continue;
	}
        if (nidb->novellnet == net) {
	    if (ieee_equal(nidb->idb->novell_node, host)) {
		if (interactive)
		    printf(nov_badaddr, "can't be local host");
		return;
	    }
	    break;
	}
    }
    /*
     * Not local host ... make sure it's a local network:
     */
    FOR_ALL_NIDBS(nidb) {
        if (nidb->novellnet == net)
            break;
    }

    /*
     * Screen out multicast addresses if connected net:
     */
    if (nidb != NULL) {
	if (nov_multicast_addr(nidb->idb->hwptr, host[0])) {
	    if (interactive) {
		printf(nov_badaddr, "multicast not permitted");
	    } else {
		buginf(nov_badaddr, "multicast not permitted");
	    }
	    return;
	}
    }

    if (int3 > novell_max_hops) {
	if (interactive) {
	    printf("IPX: Hopcount cannot be greater than %d", novell_max_hops);
	} else {
	    buginf("IPX: Hopcount cannot be greater than %d", novell_max_hops);
	}
	return;
    }

    newsap = nov_allocate_sap_entry();
    if (newsap == NULL) {
	if (interactive) {
	    printf(nomemory);
	} else {
	    buginf(nomemory);
	}
	return;
    }
    newsrdb = malloc_named(sizeof(novell_sapentry_rdb), "IPX service RDB");
    if (newsrdb == NULL) {
	if (interactive) {
	    printf(nomemory);
	} else {
	    buginf(nomemory);
	}
	free(newsap);
	return;
    }

    /* Get type code */
    newsap->server.server_type = int1;
    
    /* Get service name */
    memset(newsap->server.server_name, 0, NOVELL_SERVER_NAME_LEN);
    /* Record name, truncating at max legal size if necessary */
    sstrncpy(newsap->server.server_name, new_server_name,
	     NOVELL_SERVER_NAME_LEN);


    newsap->server.dnet = net;
    ieee_copy(host, newsap->server.dhost);

    /* Get socket number */
    newsap->server.dsock = int2;

    /*
     * Get hop count.
     */
    newsap->server.nethops = int3;
    newsap->pathcount = 1;
    /*
     * Get an nidb to associate with this sap, if none found, print warning.
     * We can't announce the SAP until the route is learned because we won't
     * be able to implement split horizon otherwise. A side effect is also
     * black hole prevention. If we find a route, fill in the nidb as if the
     * SAP was learned via that interface. Fudge the srchost field with our 
     * own mac address for safety (a packet erroneously sent to this address 
     * will be ignored and its better than leaving it zero).
     */
    rip_entry = novrip_find_rt(net, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_FUZZ);
    if (!rip_entry) {
	if (interactive) {
	    printf("\n%%no route to %x, %s won't be announced until"
		   " route is learned", net, newsap->server.server_name);
	} 
	newsrdb->nidb = NULL;
	route_delay = METRIC_INACCESSIBLE;
	route_hops = NOVELL_MAX_HOPS_ALLOWED;
	route_flags = NET_RIP;
    } else {
	newsrdb->nidb = rip_entry->path_list->nidb;
	route_delay = rip_entry->delay;
	route_hops = rip_entry->hopcount;
	route_flags = rip_entry->flags;
	if (route_flags & NET_STATIC) {
	    route_flags = NET_RIP;
	} else if (route_flags & NET_IGRP) {
	    route_delay = rip_entry->path_metric;
	    route_hops = rip_entry->ext_hop;
	} else if (route_flags & NET_RIP) {
	    if ((route_hops >= novell_infinity) ||
		TIMER_RUNNING(rip_entry->holddownuntil) ||
		(route_delay == NOVELL_INFDELAY)) {
		route_delay = METRIC_INACCESSIBLE;
		route_hops = NOVELL_MAX_HOPS_ALLOWED;
	    }
	}
    }
    /*
     * See if we already have one in the table, if so delete it. Static
     * SAPs will always override dynamically learned ones.
     */
    sapq = novell_findsapqueue(newsap->server.server_type,
			       IPX_SAP_QUEUE_LOOKUP_MUST_EXIST);
    if (sapq) {
 	match = nov_find_sap_name_entry(sapq, &newsap->server);
	if (match != NULL) {
	    lw_remove(&match->saps);
	    ipx_sap_self_backup(match, FALSE);
	    old_backup_info = match->backup_info;
	    match->backup_info = NULL;
	    nov_sap_free(sapq, match, IPX_SAP_FREE_IGNORE_BACKUPS);
	}
    }
    /*
     * Find a place for it in the table.
     */ 
    if (!sapq) {
	sapq = novell_findsapqueue(newsap->server.server_type,
				   IPX_SAP_QUEUE_LOOKUP_MAY_CREATE);
	if (!sapq) {
	    if (interactive)
		printf("\nIPX: No memory for new sap entry!");
	    nov_sap_free(NULL, newsap, IPX_SAP_FREE_LOOKUP_BACKUPS);
	    return;
	}
    }
    newsap->flags |= NET_STATIC;
    GET_TIMESTAMP(newsrdb->timer);
    newsap->paths = newsrdb;
    newsap->novpdbindex = novstatic_pdb.index;
    newsap->poisoned = newsap->reverse_poisoned = FALSE;
    newsap->route_delay = route_delay;
    newsap->route_hops = route_hops;
    newsap->route_flags = route_flags;
    if (nvgen_option == IPX_STATIC_DEFINE_TYPE_NO_NVGEN) {
	newsap->options |= IPX_SERVER_OPTION_STATIC_NO_NVGEN;
    }
    newsap->backup_info = old_backup_info;
    novell_place_in_sap_list(sapq, newsap, IPX_SAP_INSERT_HASH_NEEDED, TRUE);
    nov_static_server_count++;
}


/*
 * novell_enable_full_rip_updates
 *
 * Enable periodic RIP updates on a network
 *
 * NOTE: This routine is a NOP of full updates are already enabled
 *
 */
static void novell_enable_full_rip_updates (nidbtype *nidb)
{
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);
    
    if (nidb == NULL) {
	return;
    }
    
    if (debug) buginf("\nIPXRIP:  Enable full updates, net %x:  ",
		      nidb->novellnet);
    
    if (!mgd_timer_running(&nidb->rip_update_timer)) {
	
	if (debug) buginf("ENABLE");

	/*
	 * Treat as reset ... that is, as if the network was down and
	 * is now back up. If we don't do this a reverse poison which
	 * was aborted due to a link flap may still appear to be active,
	 * and certain routes may be stuck in holddown as a result.
	 */
	GET_TIMESTAMP(nidb->rip_lastupdate); 
	GET_TIMESTAMP(nidb->rip_revp_lastupdate); 

	/*
	 * Fire up the periodic update timer with NO holddown.
	 */
	mgd_timer_start(&nidb->rip_update_timer, NOV_UPDATE_HOLDDOWN);

    } else {
	if (debug) buginf("RUNNING");
    }
}


/*
 * novell_enable_full_sap_updates
 *
 * Enable periodic SAP updates on a network
 *
 * NOTE: This routine is a NOP if full updates are already enabled
 *
 */
static void novell_enable_full_sap_updates (nidbtype *nidb)
{
    boolean debug = (novell_sapupd_debug && novell_verbose_debug);

    if (nidb == NULL) {
	return;
    }

    if (debug) buginf("\nIPXSAP:  Enable full updates, net %x ",
		      nidb->novellnet);

    /*
     * Only pretend if SAP after RIP is configured. The RIP timer is
     * effectively in control of SAP periodic updates in this mode.
     */
    if (nidb->novell_sap_after_rip) {
	return;
    }

    if (!mgd_timer_running(&nidb->sap_update_timer)) {

	if (debug) buginf("(ENABLE)");

	/*
	 * Treat as reset ... that is, as if the network was down and
	 * is now back up. If we don't do this a reverse poison which
	 * was aborted due to a link flap may still appear to be active,
	 * and certain services may be stuck in holddown as a result.
	 */
	nov_get_timestamp(&nidb->sap_lastupdate);
	nidb->sap_revp_lastupdate = nidb->sap_lastupdate;

	/*
	 * Fire up the periodic update timer (with a short holddown)
	 * EVEN IF the SAP interval has been set to ZERO. This is
	 * because one full update should be sent when a circuit
	 * comes up, EVEN IF SAP incremental is configured. Note that
	 * the first update is set to fire two seconds after the first
	 * rip update, increasing the chance that routes to services
	 * will be available when the SAP update is running. This
	 * could be made more intelligent by linking the initial sap
	 * update to completion of the initial rip update (see
	 * "ipx sap-after-rip").
	 */
	mgd_timer_start(&nidb->sap_update_timer, ONESEC*2);

    } else {
	if (debug) buginf("(RUNNING)");
    }
}


/*
 * novell_disable_full_rip_updates
 *
 * Disable periodic RIP updates on a network
 *
 * %%% TBS: POSSIBLE ABORT OF ANY UPDATE IN PROGRESS %%%
 *
 */
static void novell_disable_full_rip_updates (nidbtype *nidb)
{
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);
    
    if (nidb == NULL) {
	return;
    }
    
    if (debug) buginf("\nIPXRIP: Disable full updates, net %x ",
		      nidb->novellnet);
    
    if (mgd_timer_running(&nidb->rip_update_timer)) {
	
	if (debug) buginf("(DISABLE)");
	
	mgd_timer_stop(&nidb->rip_update_timer);

    } else {
	if (debug) buginf("(DISABLED)");
    }
}


/*
 * novell_disable_full_sap_updates
 *
 * Disable periodic SAP updates on a network
 *
 */
static void novell_disable_full_sap_updates (nidbtype *nidb)
{
    boolean debug = (novell_sapupd_debug && novell_verbose_debug);
    
    if (nidb == NULL) {
	return;
    }
    
    if (debug) buginf("\nIPXSAP: Disable full updates, net %x:  ",
		      nidb->novellnet);

    if (mgd_timer_running(&nidb->sap_update_timer)) {
	
	if (debug) buginf("(DISABLE)");

	mgd_timer_stop(&nidb->sap_update_timer);

    } else {
	if (debug) buginf("(DISABLED)");
    }
}


/*
 * novell_posit_full_rip_update
 *
 * Trigger a full RIP update to the broadcast address
 *
 */
static boolean novell_posit_full_rip_update (nidbtype *nidb)
{
    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_rip_update(nidb,
				   nidb->novellnet,
				   baddr,
				   0,
				   IPX_FULL_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   FALSE, FALSE));
}


/*
 * novell_posit_full_sap_update
 *
 * Trigger a full SAP update to the broadcast address
 *
 */
static boolean novell_posit_full_sap_update (nidbtype *nidb)
{

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_sap_update(nidb,
				   nidb->novellnet,
				   baddr,
				   0,
				   IPX_FULL_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   NOVELL_EVERYONE,
				   FALSE, FALSE));
}


/*
 * novell_posit_partial_rip_update
 *
 * Trigger a partial RIP update to the broadcast address
 *
 */
static boolean novell_posit_partial_rip_update (nidbtype *nidb)
{
    
    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }
    
    return(novell_posit_rip_update(nidb,
				   nidb->novellnet,
				   baddr,
				   0,
				   IPX_FULL_NOT_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   FALSE, FALSE));
}


/*
 * novell_posit_partial_sap_update
 *
 * Trigger a partial SAP update to the broadcast address
 *
 */
static boolean novell_posit_partial_sap_update (nidbtype *nidb)
{

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_sap_update(nidb,
				   nidb->novellnet,
				   baddr,
				   0,
				   IPX_FULL_NOT_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   NOVELL_EVERYONE,
				   FALSE, FALSE));
}


/*
 * novell_posit_reverse_rip_update
 *
 * Trigger a reverse poison RIP update to the broadcast address
 *
 */
static boolean novell_posit_reverse_rip_update (nidbtype *nidb)
{
    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_rip_update(nidb, nidb->novellnet, baddr, 0,
				   IPX_FULL_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   TRUE, FALSE));
}


/*
 * novell_posit_reverse_sap_update
 *
 * Trigger a reverse poisons SAP update to the broadcast address
 *
 */
static boolean novell_posit_reverse_sap_update (nidbtype *nidb)
{

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_sap_update(nidb, nidb->novellnet, baddr, 0,
				   IPX_FULL_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   NOVELL_EVERYONE,
				   TRUE, FALSE));
}


/*
 * novell_posit_linkdown_rip_update
 *
 * Trigger a linkdown poison RIP update to the broadcast address
 *
 */
static boolean novell_posit_linkdown_rip_update (nidbtype *nidb)
{
    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_rip_update(nidb, nidb->novellnet, baddr, 0,
				   IPX_FULL_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   FALSE, TRUE));
}


/*
 * novell_posit_linkdown_sap_update
 *
 * Trigger a linkdown poison SAP update to the broadcast address
 *
 */
static boolean novell_posit_linkdown_sap_update (nidbtype *nidb)
{

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
	return(FALSE);
    }

    return(novell_posit_sap_update(nidb, nidb->novellnet, baddr, 0,
				   IPX_FULL_NEEDED,
				   IPX_DESTINATION_BROADCAST,
				   NOVELL_EVERYONE,
				   FALSE, TRUE));
}


/*
 * novell_nidb_bringup
 *
 * Bring a novell network 'up' Delete current routes to us; create new
 * connected route to us; fixup RIP/SAP queues
 *
 * NOTE: This routine is called by route_adjust with process suspends
 *       prohibited. Ergo: DO NOT SUSPEND.
 */
void novell_nidb_bringup (nidbtype *nidb)
{
    novndbtype *p;
    backuptype *binfo, *old_backup_info;

    binfo = old_backup_info = NULL;

    /*
     * Has startup work been completed on the line? If not, do not bring up,
     * but ask for startup work to start. If so, then allow the line to come
     * fully up
     */
    if (interface_up(nidb->idb) &&
	nov_ipxwan_on_nidb(nidb) && (!nov_ipxwan_up_nidb(nidb))) {
        reg_invoke_nov_ipxwan_start_nidb(nidb);
	return;
    }
    /* Only bring up if FULLY up */
    if (!NOVELL_NIDB_IDB_UP(nidb)) {
	/* We aren't up, and won't do 'bringup' work; therefore, we are DOWN */
	novell_nidb_bringdown(nidb);
	return;
    }
    nidb->nov_sent_linkdown_poisons = TRUE;
    novcache_increment_version(nidb->idb->hwptr, NOVCACHE_BRINGUP_NET, 
			       nidb->novellnet, NOV_NETMASK_ALL);
    nov_adjust_fastflags(nidb->idb);
    /* Don't do the following for unnumbered interfaces... */
    if (!NOVELL_NIDB_UNNUMBERED(nidb)) {
	p = novrip_find_rt(nidb->novellnet, NOV_NETMASK_ALL, 
			   IPX_ROUTE_MATCH_EXACT);
    } else {
	p = NULL;
    }
    if (p != NULL) {
	if (!(p->flags & NET_CONNECTED)) {
	    ipx_ndb_self_backup(p);
	    old_backup_info = p->backup_info;
	    p->backup_info = NULL;
	    novrip_del_rt(nidb->novellnet, NOV_NETMASK_ALL, 
			  IPX_ROUTE_DELETE_REMOVE_CONNECTED);
	    novrip_prune_nidb(nidb);
	    novell_prune_nidb_sapq(nidb);
	    /*
	     * Note that adding the connected network triggers
	     * a request for a partial update:
	     */
	    novrip_add_rt(&novconnected_pdb, nidb, nidb->novellnet, 
			  NOV_NETMASK_ALL, 0L, NULL, 
			  NOVELL_DEFAULT_CONNECTED_TICKS,
			  NOVELL_DEFAULT_CONNECTED_HOPS,
			  NET_CONNECTED, IPX_ROUTE_ADD_NORMAL,
			  IPX_STATIC_DEFINE_TYPE_NVGEN, IPX_STATIC_TYPE_HOST,
			  IPX_FLOATING_STATIC_EXISTS_NO);
	    p = novrip_find_rt(nidb->novellnet, NOV_NETMASK_ALL, 
			       IPX_ROUTE_MATCH_EXACT);
	    if (p != NULL) {
		p->backup_info = old_backup_info;
	    } else {
		ipx_delete_backup_list(old_backup_info);
	    }
	} else {
	    p->hopcount = p->ext_hop = p->last_hopcount;
	    p->delay = p->ext_delay = p->last_delay;
	    novell_mark_rip_changed(p);
	}
    } else {
	if (!NOVELL_NIDB_UNNUMBERED(nidb)) {
	    novrip_add_rt(&novconnected_pdb, nidb, nidb->novellnet, 
			  NOV_NETMASK_ALL, 0L, NULL,
			  NOVELL_DEFAULT_CONNECTED_TICKS,
			  NOVELL_DEFAULT_CONNECTED_HOPS,
			  ((nidb->internal_network ? NET_INTERNAL : 0) |
			   NET_CONNECTED), IPX_ROUTE_ADD_NORMAL,
			  IPX_STATIC_DEFINE_TYPE_NVGEN, IPX_STATIC_TYPE_HOST,
			  IPX_FLOATING_STATIC_EXISTS_NO);
	}
    }	
    reg_invoke_novigrp_route_adjust(NULL, nidb->idb);
    novrip_static_routes(nidb, ROUTE_UP);
    /* On coming up we need to send out general queries */
    /* but first check if we are supposed to send RIP and SAP */
    if (nidb->novell_interface_up && 
	(!nov_ipxwan_on_nidb(nidb) ||
	 (nov_ipxwan_up_nidb(nidb) && nidb->nov_ipxwan_need_ripsap))) {
	    /*
	     * Now send the general queries:
	     */
	    novell_send_gen_rip_query(nidb);
	    novell_send_gen_sap_query(nidb, NOVELL_EVERYONE);
	    /*
	     * Now post initial full broadcast updates. The rip update
	     * goes out ASAP while the sap update is delayed slightly.
	     */
	    novell_enable_full_rip_updates(nidb);
	    novell_enable_full_sap_updates(nidb);
	}
}

/*
 * nov_kill_connected_route
 *
 * Kill a directly connected route. Flush the cache; delete the route;
 * delete routes and  servers learned via that connected route. Delete
 * static routes pointing through it. Mark the nidb associated with it
 * as 'down' as far as IPX is concerned.
 */
static void nov_kill_connected_route (nidbtype *nidb)
{
    novndbtype *p = novrip_find_rt(nidb->novellnet, NOV_NETMASK_ALL, 
				   IPX_ROUTE_MATCH_EXACT);

    /* Handle numbered connected, or unnumbered case... */
    if (((p != NULL) && (p->flags & NET_CONNECTED)) || (p == NULL)) {
	nov_adjust_fastflags(nidb->idb);
	/* Don't try to delete unnumbered... */
	if (!NOVELL_NIDB_UNNUMBERED(nidb)) {
	    novrip_del_rt(nidb->novellnet, NOV_NETMASK_ALL,
			  IPX_ROUTE_DELETE_REMOVE_CONNECTED);
	}
	novrip_prune_nidb(nidb);
	novell_prune_nidb_sapq(nidb);
	novrip_static_routes(nidb, ROUTE_DOWN);
	novcache_increment_version(nidb->idb->hwptr, NOVCACHE_KILL_CONN_RT, 
				   nidb->novellnet, NOV_NETMASK_ALL);
    }
    /* Reset IPXWAN state machine */
    if (nov_ipxwan_on_nidb(nidb)) {
        reg_invoke_nov_ipxwan_stop_nidb(nidb);
    }
}

/*
 * novell_nidb_bringdown
 *
 * Bring a novell network 'down' Flush the cache; set the route as being
 * infinite distance away (we have to retain it in the table so we can
 * advertise it as 'poisoned'.)
 */
void novell_nidb_bringdown (nidbtype *nidb)
{
    novndbtype *p = novrip_find_rt(nidb->novellnet, NOV_NETMASK_ALL,
				   IPX_ROUTE_MATCH_EXACT);

    nov_adjust_fastflags(nidb->idb);
    /* Handle connected route case, or unnumbered case (no route) */
    if (((p != NULL) && (p->flags & NET_CONNECTED)) || (p == NULL)) {
	novcache_increment_version(nidb->idb->hwptr, NOVCACHE_BRINGDOWN_NET, 
				   nidb->novellnet, NOV_NETMASK_ALL);
	reg_invoke_novigrp_route_adjust(NULL, nidb->idb);
	if (p != NULL) {
	    p->hopcount = NOVELL_MAX_HOPS_ALLOWED;
	    novell_mark_rip_changed(p);
	}
    }
    /* We need to delay deletion action until poisons are sent */
    nidb->nov_sent_linkdown_poisons = FALSE;

    novell_disable_full_rip_updates(nidb);
    novell_disable_full_sap_updates(nidb);

    process_set_boolean(novell_link_down_wb, TRUE);

}

/*
 * novell_nidb_print_state
 *
 * Print the state for the NIDB given
 */
static char *novell_nidb_print_state (
    nidbtype *nidb)
{
    if (!nidb)
	return("");

    switch (nidb->state) {
      case NOVELL_NIDB_UP:
	return("[up]");
      case NOVELL_NIDB_DELETE_PENDING:
	return("[deletion pending]");
      case NOVELL_NIDB_DOWN_PENDING:
	return("[down pending]");
      case NOVELL_NIDB_DELETE_EXECUTING:
	return("[being deleted]");
      case NOVELL_NIDB_DOWN_EXECUTING:
	return("[being brought down]");
      case NOVELL_NIDB_DELETED:
	return("[deleted]");
      case NOVELL_NIDB_DOWN:
	return("[down]");
      case NOVELL_NIDB_IPXWAN_STARTING:
	return("[IPXWAN starting]");
      case NOVELL_NIDB_NEW:
	return("[new]");
      default:
	return("[unknown]");
    }
}

/*
 * novell_nidb_free
 *
 * Free an NIDB and its associated structures. Do common work that may
 * be required when an NIDB is freed (such as clearing SAP/RIP update
 * queues).
 */
static void novell_nidb_free (nidbtype *nidb)
{
    nidbtype *next_nidb;
    novell_client *client;
    int count, Qsize;

    if (!nidb)
        return;

    if (novellrip_debug || novellrip_event_debug)
        buginf("\nIPX: Deleting network %x from interface %s",
	       nidb->novellnet, nidb->idb->namestring);
    reg_invoke_ipx_nidb_network_changed(nidb, IPX_ROUTE_EVENT_DELETED);

    /* Remember 'next' in chain for 'next output' pointer fixup (if needed) */
    next_nidb = nidb->next;

    /* Now, unqueue the nidb from the nidb chain */
    unqueue(&novidbQ, nidb);

    /* Cleanup the 'next output' pointer to no longer be here */
    if (novell_next_output_nidb == nidb) {
	novell_next_output_nidb = next_nidb;
	if (novell_next_output_nidb == NULL)
	  novell_next_output_nidb = (nidbtype *) novidbQ.qhead;
    }

    if (nidb->pname)
	free(nidb->pname);
    /* Finally, free the nidb */
    if (IPX_NIDB_IS_INTERNAL(nidb)) {
	vidb_free(nidb->idb);
	nidb->idb = NULL;
    }
    if (nidb->inbound_compression_slots != NULL)
	free(nidb->inbound_compression_slots);
    if (nidb->outbound_compression_slots != NULL)
	free(nidb->outbound_compression_slots);

    nov_deallocate_usv(nidb->rip_full);
    nov_deallocate_usv(nidb->sap_full);
    nov_deallocate_usv(nidb->rip_part);
    nov_deallocate_usv(nidb->sap_part);
    nov_deallocate_usv(nidb->rip_down);
    nov_deallocate_usv(nidb->sap_down);
    nov_deallocate_usv(nidb->rip_revp);
    nov_deallocate_usv(nidb->sap_revp);
    mgd_timer_stop(&nidb->rip_update_timer);
    mgd_timer_stop(&nidb->sap_update_timer);

    Qsize = QUEUESIZE(&nidb->clientQ);
    for (count = 0; count < Qsize; count++) {
	client = dequeue(&nidb->clientQ);
	if (client->idb) {
	    client->idb->owner_nidb = NULL;
	    DECREMENT_NOVELLNETS(client->idb);
	}
	free(client);
    }

    nov_free_nidb(nidb);
}

/*
 * fiddle_node_address - Set ipx node address each time a
 *                       network is (re)configured.
 */
static void fiddle_node_address (idbtype *swidb)
{
    hwidbtype *hwidb;

    if (swidb == NULL)
	return;
    hwidb = swidb->hwptr;
    if (hwidb == NULL)
	return;

    /*
     * On non-IEEE, non-LANE interfaces, use the Novell default
     * address, unless "mac-address" has been specified. An explicit
     * "mac-address" will always override the default.
     *
     * On all IEEE and LANE interfaces, simply use the current hw address.
     * Note that if an explicit "mac-address" has been configured, this
     * will be reflected in the current hw address.
     */
    if (is_ieee_zero(hwidb->forced_mac) && !idb_is_lan(swidb)) {
	ieee_copy(nov_defaddr, swidb->novell_node);
    } else {
	ieee_copy(hwidb->hardware, swidb->novell_node);
    }
}

/*
 * novell_address_command - implements "no ipx routing" (shutdown)
 * and "ipx routing" (startup).
 *
 * A word about the shutdown/startup strategy. The basic idea is to set
 * up a mechanism to allow some semblance of a graceful shutdown, including
 * route poisoning, process deletion and data deallocation. At startup, the
 * reverse takes place. Note that at the present time, RIP and NLSP will cease
 * operation but will restart following a shutdown/startup sequence (if they
 * were previously configured). By contrast, eigrp bails out and deletes
 * itself.
 *
 * Two protocol vectors are defined to facilitate this. pdb->cleanup is
 * invoked via novell_router_erase() on shutdown, while pdb->restart is
 * invoked directly on startup. Process deletion/creation as well as per
 * protocol data deallocation is a side effect.
 *
 * Note that when shutdown commences, novell_shutting_down becomes TRUE,
 * while novell_running remains TRUE. When novell_shutting_down is TRUE,
 * novell_input() will pitch incoming packets, and RIP and SAP processes
 * will discard all pending request packets. Meanwhile, since novell_running
 * is still TRUE, update processes may continue to run and output poisons,
 * as appropriate. The spec says it's OK to ignore the configured RIP/SAP
 * interpacket gap when sending down poisons. This is keyed off the shutdown
 * flag. When shutdown is complete, all networks are placed in down state
 * and both booleans become FALSE.
 *
 */
void novell_address_command (boolean interactive, boolean enabling, ulong cnet,
			     uchar *chost)
{
    nidbtype *nidb, *nidbnext;
    idbtype *idb;
    hwidbtype *hwidb;
    novpdbtype *pdb, *next_pdb;
    uchar host[IEEEBYTES];
    uchar *defaddr;
    boolean sdebug = (novellnlsp_debug || novelligrp_debug ||
		      novellsap_debug || novellsap_event_debug ||
		      novellrip_debug || novellrip_event_debug);

    if (!enabling) {

	if (!novell_started || !novell_running) {
	    return;
	}
	novell_shutting_down = TRUE;

	if (sdebug) {
	    buginf("\nIPX: Shutting down ...");
	}

	/*
	 * Clear out fastswitch cache. New packets arrive at
	 * process level and will be discarded by nov_input().
	 */
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				   NOV_NETMASK_ALL);

	/*
	 * Purge the route table gracefully (send poisons):
	 */
	novrip_shutdown();

	/*
	 * Allow some time for protocols to run to completion:
	 *
	 * %%% This should probably be configurable %%%
	 */
	if (novell_nlsp_running > 0) {
	    process_sleep_for(ONESEC*10); /* %%% */
	} else {
	    process_sleep_for(ONESEC*3);  /* %%% */
	}
	/*
	 * Shutdown all processes. Purge the route table completely:
	 */
	for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb = next_pdb) {
	    next_pdb = pdb->next;
	    nov_router_erase(pdb); /* Calls pdb->cleanup */
	}

	/*
	 * Additional cleanup of non-protocol specific data structures
	 * goes here. Note that anything torn down here may need to be
	 * reallocated/initialized before we can start up again.
	 *
	 * Deallocate global data here, as appropriate:
	 */

	nov_fscache_flush(); /* Deallocate fast switch cache */

	novell_running = novell_shutting_down = FALSE;
	FOR_ALL_SWIDBS(idb)
	    novell_rxtypes_update(idb);

	FOR_ALL_HWIDBS(hwidb) {
	    nov_fastsetup(hwidb);
	}
	/* Bring down the networks (unless they are being deleted) */
	FOR_ALL_NIDBS(nidb) {
	    if (NOVELL_NIDB_CONFIGURABLE(nidb)) {
		novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	    }
	}
	/*
	 * Toss networks deleted or waiting to be deleted. This can be a
	 * network deleted, but requiring EIGRP SAPs to be sent out it.
	 */
	FOR_ALL_NIDBS_USE_SAVED_NEXT(nidb, nidbnext) {
	    if (NOVELL_NIDB_DELETED_MODE(nidb)) {
		if (IPX_NIDB_IS_PRIMARY(nidb)) {
		    IPX_CLEAR_PRIMARY_NIDB(nidb->idb);
		}
		DECREMENT_NOVELLNETS(nidb->idb);
		novell_nidb_free(nidb);
	    }
	}
	if (sdebug) {
	    buginf("\nIPX: Shut down ... complete");
	}

	/*
	 * SHUTDOWN COMPLETE: Note that all RIP/NLSP pdb's continue to exist
	 * but with pdb->running set to FALSE (EIGRP kills itself off).
	 * When IPX routing is restarted, all context required to restart
	 * each protocol must be available within each pdb. The pdb->restart
	 * vector will be called to accomplish this.
	 */
    } else {
	if (cnet) {
	    ieee_copy(chost, host);
	    defaddr = host;
	} else {
	    /*
	     * No address spec was given.  Use a default.  This
	     * default will be an IEEE interface with a valid
	     * address.  It is used only for non-IEEE (non-LAN)
	     * media.
	     */
	    defaddr = default_mac_addr;
	}
	if ((GETSHORT(&defaddr[4]) == 0 && GETSHORT(&defaddr[2]) == 0
	     && GETSHORT(&defaddr[0]) == 0)) {
	    if (interactive)
		printf(nov_badaddr,  "malformed hex address");
	    return;
	}
	ieee_copy(defaddr, nov_defaddr);
	novell_running = TRUE;	/* needed by TR reset */
	bcopy(hostname, ipx_internal_name, (strlen(hostname) + 1));
	FOR_ALL_SWIDBS(idb) {
	    novell_rxtypes_update(idb);
	    hwidb = idb->hwptr;
	    nov_fastsetup(hwidb);
	    if (!idb->novellnets){
		continue;
	    }
	    /* novell has special addrs */

	    fiddle_node_address(idb);
	}
	/*
	 * Create/initialize data structures if this is the first time.
	 */
	if (!novell_started) {
	    novell_start();
	} else {
	    /*
	     * If this is not the first time, lots of data structures
	     * may be laying around. Make sure everything is tidied up
	     * and reinitialized here.
	     */

	    /* TBS */

	    /*
	     * Restart other routing processes:
	     */
	    for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb = pdb->next)  {
		/*
		 * NLSP pdb hangs around even after 'no ipx router nlsp' is
		 * issued. Only the osi_pdb portion is freed. So don't restart 
		 * NLSP if osi_pdb is not present.
		 */
		if (pdb->proctype == NET_NLSP && !pdb->osi_pdb) 
		    continue;
		if (pdb->restart != NULL) {
		    (*pdb->restart)(pdb);
		}
	    }
	    /* Bring up the networks */
	    FOR_ALL_NIDBS(nidb) {
		if (!(nidb->administratively_shutdown ||
		      NOVELL_NIDB_DELETED_MODE(nidb))) {
		    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
		}
	    }
	    /*
	     * Make sure fastswitch acl flags are valid when restarting:
	     */
	    nov_update_global_acl_flags();
	}
    }
}

/*
 * nov_hwaddr_notify
 *
 * Used to update our node address whenever the MAC address of
 * the hwidb changes.
 */
void nov_hwaddr_notify(idbtype *swidb, uchar *hwaddr, ushort len)
{
    ushort i;

    if (novell_debug1) {
	buginf("\nhwaddr_notify - %s->", swidb->namestring);
	for (i=0;i<len-1;i++)
	    buginf("%-02x:", hwaddr[i]);
	buginf("%-02x", hwaddr[i]);
    }
    ieee_copy(hwaddr, swidb->novell_node);
    return;
}

/*
 * change_node_address - Change the IPX node address for a swidb following
 *                       a change to the underlying hw idb.
 *
 * Note: This should only be done if ok_to_change_node_address() has
 *       returned true ... otherwise, the node address may be changed
 *       too early if other nidb's exist on this swidb and are still in
 *       the process of coming down.
 */
static void change_node_address(idbtype *swidb)
{
    nidbtype *nidb;

    if (swidb->hwptr == NULL)
	return;

    FOR_ALL_NIDBS(nidb) {
	if (nidb->idb == swidb) {
	    if (nidb->node_pending) {
		/*
		 * Set the new node address as appropriate:
		 */
		fiddle_node_address(swidb);
		nidb->node_pending = FALSE;
		/*
		 * Bring nidb UP if so marked, and force periodic update:
		 */
		if (nidb->up_pending) {
		    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
		    nidb->up_pending = FALSE;
		}
	    }
	}
    }
}
/*
 * ok_to_change_node_address - Check if all nidb's on this swidb
 * that required a node change, have reached down state. If so,
 * the node address may be changed.
 */
static boolean ok_to_change_node_address(idbtype *swidb)
{
    nidbtype *nidb;

    FOR_ALL_NIDBS(nidb) {

	if (nidb->idb == NULL)
	    continue;

	if (nidb->idb == swidb) {
	    if (nidb->node_pending && (!NOVELL_NIDB_ISDOWN(nidb)))
		return(FALSE);
	}
    }
    return(TRUE);
}

/* nov_mac_address_change_swidb - Handle a change to the underlying hw
 * mac address. NOTE THAT ANY NET THAT IS UP, MUST COME DOWN!!!!!
 *
 * Therefore, any net on this swidb that is currently up (or happens
 * to be coming down) will first be brought completely down. Poisons
 * will be sent accordingly, using the old node address. Mark these
 * networks with "node_pending". Mark networks that were UP with "up_pending"
 * so we remember to bring them back up following the node address change.
 * This will happen in novell_nidb_newstate() when all networks on this
 * swidb have reached down state.
 *
 * Note that if we are here because the "mac-address" command has been
 * issued, that the hwidb has just been reset (see coerce_hw() in
 * if/network.c). Some interfaces (notably tr and fddi) will be down
 * and our best attempts to send poisons will fail.
 */
static void nov_mac_address_change_swidb(idbtype *swidb)
{
    nidbtype *nidb;
    boolean needed = FALSE;
    
    if (swidb == NULL)
	return;

    FOR_ALL_NIDBS(nidb) {
	
	if ((nidb->idb == NULL) || (nidb->idb->hwptr == NULL))
	    continue;
	
	if (nidb->idb == swidb) {

	    needed = TRUE;
	    
	    if (nidb->node_pending || nidb->up_pending) {
		continue;
	    }
	    if (NOVELL_NIDB_ISUP(nidb)) {
		/*
		 * Force the node down in order to send out poisons
		 * *before* changing the node address. Mark the
		 * nidb so as to bring it back up following the
		 * node address change.
		 */
		nidb->node_pending = nidb->up_pending = TRUE;
		novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN_PENDING);
	    } else {
		/*
		 * Mark for node address change if down or coming down:
		 */
		nidb->node_pending = TRUE;
	    }
	}
    }
    /*
     * If no reason to wait, change node address now.
     */
    if (needed && ok_to_change_node_address(swidb)) {
	change_node_address(swidb);
    }
}

/*
 * nov_mac_address_change - Handle a change to our hw mac address. The
 * novell node address in each swidb needs an update.
 */
void nov_mac_address_change(hwidbtype *hwidb)
{
    idbtype *swidb;
    
    if (hwidb == NULL)
	return;
    
    FOR_ALL_SWIDBS(swidb) {
	if ((swidb->hwptr == NULL) || (swidb->hwptr != hwidb)) {
	    continue;
	}
	nov_mac_address_change_swidb(swidb);
    }
}

/*
 * ipx_configure_for_lane
 *
 * This routine is invoked from LANE code when a LANE Client is being
 * added to an interface.  If there is already an "ipx network" on this
 * subinterface, it must be have ET_NOVELL_ETHER encap and use the
 * hardware MAC for novell_node.  If that's not true, and ipx is already
 * up, make the necesary changes.  This will only happen if ipx is
 * configured on subinterface before LANE Client.  If Client is already
 * there when "ipx network" is done, node & enctype will be correctly
 * set by the "Fiddle MAC" code in novell_network_ifcommand.
 *
 * Also set novell evenize as LANE must behave same as real ethernet.
 *
 */ 
void ipx_configure_for_lane (idbtype *swidb)
{
    nidbtype *nidb;

    nov_mac_address_change_swidb(swidb);
    swidb->novell_evenize = TRUE;
    nidb = IPX_PRIMARY_NIDB(swidb);
    if (nidb && !nidb->explicit_encap)
	ipx_set_encaps(swidb, ET_NULL);
}

void novell_command (parseinfo *csb)
{
    nidbtype *nidb;
    idbtype *swidb;
    int i;
    novndbtype *p;
    novell_sapqueuetype *sapq;
    novell_sapentrytype *entry;
    
    if (!novell_running || novell_shutting_down) {
	if (csb->nvgen || (csb->which != NOVELL_ADDRESS)) {
	    if (!csb->nvgen) {
		printf(nov_notenabled);
	    }
	    return;
	}
    }

    /*
     * No implicit encap unless "novell encap" issued
     */
    implicit_idb = NULL;
    implicit_enctype = ET_NULL;

    if (csb->nvgen) {

	switch (csb->which) {

	  case NOVELL_ALGORITHMS:
	    nv_write(!novell_algorithms, "no %s", csb->nv_command);
	    break;

	  case NOVELL_INTERNAL_NET:
	    FOR_ALL_NIDBS(nidb) {
		if (IPX_NIDB_IS_INTERNAL(nidb) &&
		    (!NOVELL_NIDB_DELETED_MODE(nidb))) {
		    nv_write(TRUE, "%s %x", csb->nv_command, nidb->novellnet);
		    break;
		}
	    }
	    break;

	  case NOVELL_SAP_USES_ROUTING:
	    nv_write(!novell_sap_uses_routing, "no %s", csb->nv_command);
	    break;

	  case NOVELL_POTENTIAL_PSEUDONODE:
	    nv_write(!novell_potential_pseudonode, "no %s", csb->nv_command);
	    break;

	  case NOVELL_ACCTLIST:
	    ipx_acctlist(csb);
	    break;

	  case NOVELL_ACCTTRANSITS:
	    nv_write((ipx_acctg_acc->count != 0) &&
		     (ipx_acctg_acc->transits_def != 0),
		     "%s %d", csb->nv_command, ipx_acctg_acc->transits_def);
	    break;

	  case NOVELL_ACCTTHRESHOLD:
	    nv_write(ipx_acctg_info.threshold != IPXACCTGDEFAULTLIMIT,
		     "%s %u", csb->nv_command, ipx_acctg_info.threshold);
	    break;

	  case NOVELL_ADDRESS:
	    nv_write(TRUE, "%s %e", csb->nv_command,
		     nov_defaddr);
	    break;

	  case NOVELL_NSQRR:
	    nv_write(nov_gns_roundrobin, "%s", csb->nv_command);
	    break;

	  case NOVELL_PER_HOST:
	    nv_write(nov_load_share_per_host, "%s", csb->nv_command);
	    break;

	  case NOVELL_TYPE20_IN:
	    nv_write(novell_type20_in_checks, "%s", csb->nv_command);
	    break;

	  case NOVELL_TYPE20_OUT:
	    nv_write(novell_type20_out_checks, "%s", csb->nv_command);
	    break;

	  case NOVELL_NETBIOS_IN:
	    nv_write(novell_netbios_in_checks, "%s", csb->nv_command);
	    break;

	  case NOVELL_RIP_DEFAULT_ROUTE:
	    nv_write(!novell_default_route, "no %s", csb->nv_command);
	    break;

	  case NOVELL_SERVER_SPLIT:
	    nv_write(ipx_server_split_on_server_paths, "%s", csb->nv_command);
	    break;

	  case NOVELL_TYPE20_HELPERED:
	    nv_write(novell_type20_helpered, "%s", csb->nv_command);
	    break;
	    
  	  case NOVELL_FASTSWITCH_BCAST:
  	    nv_write(novell_fastswitch_bcast, "%s", csb->nv_command);
  	    break;
  	    
	  case NOVELL_KLUDGE:
	    nv_write(!ipx_kludge_enabled_flag, "no %s", csb->nv_command);
	    break;
	    
	  case NOVELL_MAXIMUM_HOPS:
	    /* User enters 16 -- really means 15. We store -1, so print +1 */
	    nv_write((novell_max_hops != NOVELL_DEFAULT_MAX_HOPS),
		     "%s %u", csb->nv_command, (novell_max_hops + 1));
	    break;

	  case NOVELL_ROUTE:
	    if (!novell_table) {
		return;
	    }
	    nv_write(TRUE,"!");

	    /* Write the patricia tree */
	    rn_walktree_blocking(nov_rnhead->rnh_treetop, nv_write_patricia);

	    for (i=0; i < NOV_NETHASHLEN; i++) {
		p = (*novell_table)[i];
		for (; p; p = p->next) {
		    /* Step thru the path tables for all paths */
		    if ((p->flags & NET_STATIC) &&
			(!(p->options & IPX_NDB_OPTION_STATIC_NO_NVGEN)) &&
			(p->path_list != NULL)) {
                        if (!(p->flags & NET_FLOATING_STATIC)) {
			    nv_write(TRUE, "%s", csb->nv_command);
                            if (p->dstnet == NOV_RIP_DEFAULT_ROUTE) {
                                nv_add(TRUE, " default");
                            } else {
                                nv_add(TRUE, " %x", p->dstnet);
                            }
			    if (p->options & IPX_NDB_OPTION_INTERFACE_STATIC) {
				nv_add(TRUE, " %s",
				       p->path_list->nidb->idb->namestring);
			    } else {
				nv_add(TRUE, " %x.%e",
				       p->path_list->srcnet,
				       p->path_list->srchost);
			    }
			    if ((p->last_delay >
				 NOVELL_DEFAULT_CONNECTED_TICKS) ||
				(p->last_hopcount > NOVELL_DEFAULT_RIP_HOPS))
			      nv_add(TRUE," %u %u",
				     p->delay, p->hopcount);
                        }
                    }
                }
            }
            /* write the floating static table */
            for (i=0; i < NOV_NETHASHLEN; i++) {
                p = (*static_table)[i];
                for (; p; p = p->next) {
                    if (p->path_list != NULL) {
			nv_write(TRUE, "%s", csb->nv_command);
			if (p->dstnet == NOV_RIP_DEFAULT_ROUTE) {
			    nv_add(TRUE, " default");
			} else {
			    nv_add(TRUE, " %x", p->dstnet);
			}
			if (p->options & IPX_NDB_OPTION_INTERFACE_STATIC) {
			    nv_add(TRUE, " %s",
				   p->path_list->nidb->idb->namestring);
			} else {
			    nv_add(TRUE, " %x.%e",
				   p->path_list->srcnet,
				   p->path_list->srchost);
			}
			if ((p->last_delay > NOVELL_DEFAULT_CONNECTED_TICKS) ||
			    (p->last_hopcount > NOVELL_DEFAULT_RIP_HOPS))
			  nv_add(TRUE," %u %u", p->delay, p->hopcount);
			nv_add(TRUE, " floating-static");
		    }
		}
	    }
	    break;

	  case NOVELL_PATHS:
	    nv_write(novell_running && (novell_cfg_maxpaths != RIP_MAXPATHS),
		     "%s %u", csb->nv_command, novell_cfg_maxpaths);
	    break;

	  case NOVELL_SAPQUEUEMAX:
	    nv_write((novell_running && (novell_sapqueuemax != 0)),
		     "%s %u", csb->nv_command, novell_sapqueuemax);
	    break;

	  case NOVELL_BACKUP_SERVER_INTERVAL:
	    nv_write((novell_running &&
		      (novell_backup_server_interval !=
		       NOVELL_DEFAULT_BACKUP_SERVER_INTERVAL)),
		     "%s %u", csb->nv_command, novell_backup_server_interval);
	    break;

	  case NOVELL_GNS_RESPONSE_DELAY:
	    nv_write((novell_running && (novell_gns_response_delay !=
					 NOVELL_DEFAULT_GNS_RESPONSE_DELAY)),
		     "%s %u", csb->nv_command, novell_gns_response_delay);
	    break;

	  case NOVELL_RCACHE_MAX_SIZE:
	    nv_write((novell_running &&
		      (novcache_max_size !=
		       NOVRCACHE_DEFAULT_MAX_SIZE)),
		     "%s %u", csb->nv_command,
		     novcache_max_size);
	    break;

	  case NOVELL_RCACHE_MAX_INACTIVE_AGE:
	    nv_write((novell_running &&
		      ((novcache_max_inactive_age !=
		       NOVRCACHE_DEFAULT_MAX_INACTIVE_AGE) ||
		       (novcache_inactive_age_rate !=
			NOVRCACHE_DEFAULT_INACTIVE_AGE_RATE))),
		     "%s %u %u", csb->nv_command,
		     novcache_max_inactive_age, novcache_inactive_age_rate);
	    break;

	  case NOVELL_RCACHE_MAX_UPDATE_AGE:
	    nv_write((novell_running &&
		      ((novcache_max_update_age !=
			(NOVRCACHE_DEFAULT_MAX_UPDATE_AGE*ONEMIN)) ||
		       (novcache_update_age_rate !=
			NOVRCACHE_DEFAULT_UPDATE_AGE_RATE))),
		     "%s %u %u", csb->nv_command,
		     (novcache_max_update_age/ONEMIN),
		     novcache_update_age_rate);
	    break;
	    
	  case NOVELL_THROTTLE_UPDATE_PROC:
	    nv_write((novell_running && novell_update_proc_throttle),
		     "%s %u", csb->nv_command, novell_update_proc_throttle);
	    break;

	  case NOVELL_THROTTLE_UPDATE_SCHED:
	    nv_write((novell_running && novell_update_sched_throttle),
		     "%s %u", csb->nv_command, novell_update_sched_throttle);
	    break;

	  case NOVELL_DEFAULT_SAP_DELAY:
	    nv_write((novell_running && 
		      (novell_def_output_sap_dly != NOV_OUTPUT_DELAY_DEFAULT)),
		     "%s %u", csb->nv_command, novell_def_output_sap_dly);
	    break;

	  case NOVELL_DEFAULT_RIP_DELAY:
	    nv_write((novell_running && 
		      (novell_def_output_rip_dly != NOV_OUTPUT_DELAY_DEFAULT)),
		     "%s %u", csb->nv_command, novell_def_output_rip_dly);
	    break;

	  case NOVELL_DEFAULT_TRIG_SAP_DELAY:
	    nv_write((novell_running && 
		      (novell_def_trig_sap_dly != NOV_OUTPUT_DELAY_DEFAULT)),
		     "%s %u", csb->nv_command, novell_def_trig_sap_dly);
	    break;

	  case NOVELL_DEFAULT_TRIG_RIP_DELAY:
	    nv_write((novell_running && 
		      (novell_def_trig_rip_dly != NOV_OUTPUT_DELAY_DEFAULT)),
		     "%s %u", csb->nv_command, novell_def_trig_rip_dly);
	    break;

	  case NOVELL_SAP:
	    nv_write(TRUE,"!");
	    FOR_ALL_SAPS(entry, sapq) {
		if ((entry->flags & NET_STATIC) &&
		    (!(entry->options &
		       IPX_SERVER_OPTION_STATIC_NO_NVGEN))) {
		    nv_write(entry->flags & NET_STATIC,
			     "ipx sap %x %S %x.%e %x %u",
			     entry->server.server_type,
			     entry->server.server_name,
			     entry->server.dnet, entry->server.dhost, 
			     entry->server.dsock, entry->server.nethops);
		}
	    }
	    nv_write(TRUE,"!");
	    break;

	  case NOVELL_PING_DEFAULT:
	    nv_write((novell_running && 
		      (novell_ping_deflt != IPX_ECHO_TYPE_CISCO)),
		     "%s novell", csb->nv_command);
	    break;
	    
	  case NOVELL_SPX_SESSION_CLEAR:
	    nv_write(novell_running &&
		      (nov_spx_idle_time != SPX_DEFAULT_IDLE_TIME * ONEMIN),
		      "%s %u", csb->nv_command,
		     nov_spx_idle_time/ONEMIN);
	    break;

	  case NOVELL_SPX_TABLE_CLEAR:
	    nv_write(novell_running &&
		      (nov_spx_clear_time != SPX_DEFAULT_CLEAR_TIME * ONEHOUR),
		      "%s %u", csb->nv_command,
		     nov_spx_clear_time/ONEHOUR);
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }
    
    
    switch (csb->which) {
      case NOVELL_ADDRESS:
	novell_address_command(TRUE, csb->sense, GETOBJ(int,1),
			       GETOBJ(paddr,1)->nov_addr.host);
	break;

      case NOVELL_ALGORITHMS:
	novell_algorithms_command(csb->set_to_default || csb->sense);
	break;

      case NOVELL_INTERNAL_NET:
	novell_internal_net_command(TRUE, csb->sense, GETOBJ(int,1));
	break;

      case NOVELL_ACCTLIST:
	ipx_acctlist(csb);
	break;

      case NOVELL_ACCTTHRESHOLD:
	novell_acctthreshold_command(csb->sense, GETOBJ(int, 1));
	break;
	
      case NOVELL_ACCTTRANSITS:
	novell_accttransits_command(csb->sense, GETOBJ(int, 1));
	break;
	
      case NOVELL_SAP_USES_ROUTING:
	novell_sap_uses_routing_command(csb->set_to_default || csb->sense);
	break;

      case NOVELL_POTENTIAL_PSEUDONODE:
	novell_potential_pseudonode_command(csb->set_to_default || csb->sense);
	break;

      case NOVELL_NSQRR:
	novell_nsqrr_command(csb->sense);
	break;

      case NOVELL_PER_HOST:
	novell_perhost_command(csb->sense);
	break;

      case NOVELL_TYPE20_IN:
	novell_type20_in_command(csb->sense);
	break;

      case NOVELL_TYPE20_OUT:
	novell_type20_out_command(csb->sense);
	break;

      case NOVELL_TYPE20_HELPERED:
	novell_type20_helpered = csb->sense;
	break;
	
      case NOVELL_NETBIOS_IN:
	novell_netbios_in_command(csb->sense);
	break;

      case NOVELL_KLUDGE:
	ipx_kludge_enabled_flag = csb->set_to_default || csb->sense;
	break;
	
      case NOVELL_SERVER_SPLIT:
	ipx_server_split_on_server_paths = csb->sense;
	break;
	    
      case NOVELL_RIP_DEFAULT_ROUTE:
	novell_rip_default_route_command(csb->set_to_default || csb->sense);
	break;

      case NOVELL_MAXIMUM_HOPS:
	if (csb->sense) {
	    novell_set_max_hops(GETOBJ(int,1) - 1);
	} else {
	    novell_set_max_hops(NOVELL_DEFAULT_MAX_HOPS);
	}
	break;

      case NOVELL_FASTSWITCH_BCAST:
	if (csb->sense) {
	    FOR_ALL_SWIDBS(swidb) {
                netbios_sb_t *netb_sb = netbios_get_sb(swidb);
                if ((netb_sb) &&
                      (netb_sb->nov_netbios_h_oacf ||
                       netb_sb->nov_netbios_b_oacf ||
                       netb_sb->nov_netbios_h_iacf ||
                       netb_sb->nov_netbios_b_iacf)) {
		    printf("\n%%Remove IPX netbios access lists first");
		    return;
		}
	    }
	}
 	novell_fastswitch_bcast = csb->sense;
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON,
				   0, NOV_NETMASK_ALL);
 	break;
 	
      case NOVELL_ROUTE:
	nov_route_command(TRUE, csb->sense, GETOBJ(int,1),
			  GETOBJ(paddr,1)->nov_addr.net,
			  GETOBJ(paddr,1)->nov_addr.host,
			  GETOBJ(int,6), GETOBJ(int,7),
			  GETOBJ(int, 3), GETOBJ(idb, 1),
			  GETOBJ(int, 2), GETOBJ(int, 4), GETOBJ(int, 5),
			  IPX_STATIC_DEFINE_TYPE_NVGEN);
	break;

      case NOVELL_SAP:
	nov_sap_command(TRUE, csb->sense, GETOBJ(string,1), GETOBJ(int,1),
			GETOBJ(paddr,1)->nov_addr.net,
			GETOBJ(paddr,1)->nov_addr.host,
			GETOBJ(int, 2), GETOBJ(int, 3),
			IPX_STATIC_DEFINE_TYPE_NVGEN);
	break;

      case NOVELL_PATHS:
	novell_paths_command(TRUE, csb->sense, GETOBJ(int, 1));
	break;

      case NOVELL_SAPQUEUEMAX:
	novell_sapqueuemax_command(csb->sense, GETOBJ(int, 1));
	break;

      case NOVELL_GNS_RESPONSE_DELAY:
	novell_gns_response_delay_command(csb->sense, GETOBJ(int, 1));
	break;

      case NOVELL_RCACHE_MAX_SIZE:
	novell_rcache_max_size_command(csb->sense, GETOBJ(int, 1));
	break;

      case NOVELL_RCACHE_MAX_INACTIVE_AGE:
	novell_rcache_max_inactive_age_command(csb->sense, GETOBJ(int, 1));
	novell_rcache_inactive_age_rate_command(csb->sense, GETOBJ(int, 2));
	break;

      case NOVELL_RCACHE_MAX_UPDATE_AGE:
	novell_rcache_max_update_age_command(csb->sense, GETOBJ(int, 1));
	novell_rcache_update_age_rate_command(csb->sense, GETOBJ(int, 2));
	break;

      case NOVELL_THROTTLE_UPDATE_PROC:
	if (csb->sense) {
	    novell_update_proc_throttle = GETOBJ(int,1);
	} else {
	    novell_update_proc_throttle = 0;
	}
	break;

      case NOVELL_THROTTLE_UPDATE_SCHED:
	if (csb->sense) {
	    novell_update_sched_throttle = GETOBJ(int,1);
	} else {
	    novell_update_sched_throttle = 0;
	}
	break;

      case NOVELL_BACKUP_SERVER_INTERVAL:
	novell_backup_server_interval_command(csb->sense, GETOBJ(int, 1));
	break;

      case NOVELL_PING_DEFAULT:
	novell_ping_default_command(csb->sense, GETOBJ(int,1));
	break;

      case NOVELL_DEFAULT_SAP_DELAY:
	nov_default_sap_delay_command(csb);
	break;

      case NOVELL_DEFAULT_RIP_DELAY:
	nov_default_rip_delay_command(csb);
	break;

      case NOVELL_DEFAULT_TRIG_SAP_DELAY:
	nov_default_trig_sap_delay_command(csb);
	break;

      case NOVELL_DEFAULT_TRIG_RIP_DELAY:
	nov_default_trig_rip_delay_command(csb);
	break;

      case NOVELL_SPX_SESSION_CLEAR:
	nov_spx_session_command(csb->sense, GETOBJ(int,1));
	break;

      case NOVELL_SPX_TABLE_CLEAR:
	nov_spx_clear_command(csb->sense, GETOBJ(int,1));
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

}

static void nov_default_sap_delay_command (parseinfo *csb)
{
    idbtype *idb;
    int old_delay;

    /* If novell not running or no change to the default delay, exit */
    if (!novell_running || (GETOBJ(int,1) == novell_def_output_sap_dly))
        return;

    /* save the old delay so we may compare all configured interfaces */
    old_delay = novell_def_output_sap_dly;

    novell_def_output_sap_dly = csb->sense ? GETOBJ(int,1) :
        NOV_OUTPUT_DELAY_DEFAULT;
    FOR_ALL_SWIDBS(idb) {
      if (idb->novell_sap_delay == old_delay)
        idb->novell_sap_delay = novell_def_output_sap_dly;
    }
  }

static void nov_default_rip_delay_command (parseinfo *csb)
{
    idbtype *idb;
    int old_delay;

    /* If novell not running or no change to the default delay, exit */
    if (!novell_running || (GETOBJ(int,1) == novell_def_output_rip_dly))
        return;

    /* save the old delay so we may compare all configured interfaces */
    old_delay = novell_def_output_rip_dly;

    novell_def_output_rip_dly = csb->sense ? GETOBJ(int,1) :
        NOV_OUTPUT_DELAY_DEFAULT;
    FOR_ALL_SWIDBS(idb) {
      if (idb->novell_rip_delay == old_delay)
        idb->novell_rip_delay = novell_def_output_rip_dly;
    }
  }

static void nov_default_trig_rip_delay_command (parseinfo *csb)
{
    nidbtype *nidb;
    int old_delay;

    /* If novell not running or no change to the default delay, exit */
    if (!novell_running || (GETOBJ(int,1) == novell_def_trig_rip_dly))
        return;

    /* save the old delay so we may compare all configured interfaces */
    old_delay = novell_def_trig_rip_dly;

    novell_def_trig_rip_dly = csb->sense ? GETOBJ(int,1) :
        NOV_OUTPUT_DELAY_DEFAULT;
    FOR_ALL_NIDBS(nidb) {
      if (nidb->trig_rip_delay == old_delay)
        nidb->trig_rip_delay = novell_def_trig_rip_dly;
    }
  }

static void nov_default_trig_sap_delay_command (parseinfo *csb)
{
    nidbtype *nidb;
    int old_delay;

    /* If novell not running or no change to the default delay, exit */
    if (!novell_running || (GETOBJ(int,1) == novell_def_trig_sap_dly))
        return;

    /* save the old delay so we may compare all configured interfaces */
    old_delay = novell_def_trig_sap_dly;

    novell_def_trig_sap_dly = csb->sense ? GETOBJ(int,1) :
        NOV_OUTPUT_DELAY_DEFAULT;
    FOR_ALL_NIDBS(nidb) {
      if (nidb->trig_sap_delay == old_delay)
        nidb->trig_sap_delay = novell_def_trig_sap_dly;
    }
  }

/*
 * novell_nidb_newstate
 *
 * Set a new NIDB state. Do actions common to the setting of such states,
 * wherever they may be done, such as creating new routes, clearing SAPs,
 * etc. This is a handy function in which to place such common code.
 */
void novell_nidb_newstate (
    nidbtype *nidb,
    ulong newstate)
{
    boolean debug = (novellrip_debug || novellrip_event_debug);

    if (!nidb)
        return;

    if (debug) {
	buginf("\nIPX: Change state of [itf]:[net] %s:%x from %s",
	       nidb->idb->short_namestring, nidb->novellnet,
	       novell_nidb_print_state(nidb));
    }

    nidb->state = newstate;

    if (debug) {
	buginf(" to %s:%s", (interface_up(nidb->idb) ? "[up]" : "[down]"),
	       novell_nidb_print_state(nidb));
    }

    /*
     * Set the state 'version' of this NIDB. If new, it starts at  zero.
     * If already in existence, it increments by one. Used by the long-term
     * background functions (novell_sendupdate and novell_sendripupdate) to
     * know that they are working on an NIDB under assumptions that may no
     * longer be valid.
     */
    if (NOVELL_NIDB_ISNEW(nidb)) {
	nidb->state_version = 0;
    } else {
	nidb->state_version++;
    }

    /* Handle routing/SAP state change information for normal nidbs*/
    if (NOVELL_NIDB_ISUP(nidb)) {
	if (!novell_shutting_down) { /* Don't bother if shutting down */
	    novell_nidb_bringup(nidb);
	}
    } else if (NOVELL_NIDB_SHOULD_BRINGDOWN(nidb)) {
	if (NOVELL_NIDB_DELETED_MODE(nidb)) {
	    novrip_delete_associated_statics(nidb);
	}
	novell_nidb_bringdown(nidb);
 	nov_kill_connected_route(nidb);
	/* 
	 * If EIGRP and incremental SAP are configured, the nidb can't be 
	 * removed until the poisoned SAP services have been successfully
	 * advertised to all neighbors. In this case, we let novell_fork()
	 * clean up the nidb chain.
	 */
	if (!NOVIGRP_DOING_INCREMENTAL_SAP(nidb)) {
	    if (NOVELL_NIDB_DELETED_MODE(nidb)) {
		if (IPX_NIDB_IS_PRIMARY(nidb)) {
		    IPX_CLEAR_PRIMARY_NIDB(nidb->idb);
		}
		DECREMENT_NOVELLNETS(nidb->idb);
		novell_nidb_free(nidb);
	    }
	}
    } else if (NOVELL_NIDB_DORD_PENDING(nidb)) {
	/*
	 * Just planning to go down? Flush caches so to force packets to
	 * process level to correctly implement 'down' functionality
	 * while waiting for the ripsap_updates process to get around to
	 * downing the network.
	 */
	novcache_increment_version(NULL, NOVCACHE_NEWSTATE_DORD_PENDING, 
				   nidb->novellnet, NOV_NETMASK_ALL);
	nov_adjust_fastflags(nidb->idb);
	process_set_boolean(novell_link_down_wb, TRUE);
    }
    reg_invoke_ipx_nidb_state_change(nidb);
    
    /*
     * If nidb is now down, possibly change node address or bring it
     * back up if so marked. Note: Other nidb's on this hwidb must be
     * in a state to permit it.
     */
    if (NOVELL_NIDB_ISDOWN(nidb) && nidb->node_pending) {
 	if (ok_to_change_node_address(nidb->idb)) {
 	    change_node_address(nidb->idb);
 	}
    }
}

/*
 * novell_delete_network
 * Delete the network given by 'nidb'.
 */
void novell_delete_network (nidbtype *nidb)
{
    /*
     * Deleting network ... invalidate the route cache first thing. Then mark
     * the network as 'going down'. The RIP and SAP background process will
     * do the actual work of deletion. Remove the network and all paths
     * associated with the (virtual) interface. Make the SAP queue consistent.
     * Finally, wipe the idb primary net indicator clean if the primary
     * network was removed.
     */
    /* If it is already in a state where it is being deleted, don't bother */
    if (NOVELL_NIDB_DELETED_MODE(nidb)) {
	return;
    }
    if (novrip_exist_associated_statics(nidb)) {
	printf("\nWarning: Static routes attached to network %x are also being deleted",
	       nidb->novellnet);
    }
    nov_network_finish(&novrip_pdb, nidb->novellnet, FALSE);
    novell_nidb_newstate(nidb, NOVELL_NIDB_DELETE_PENDING);
    reg_invoke_novigrp_address_command(nidb->idb, nidb->novellnet, FALSE);
}

/*
 * novell_nidb_set_network_number
 *
 * Sets the network number in the NIDB; sets the output process names
 * based on that number.
 */
void novell_nidb_set_network_number (nidbtype *nidb, ulong net)
{
    if (nidb == NULL)
        return;
    
    nidb->novellnet = net;

}

/*
 * novell_non_default_encapsulation
 *
 * Return TRUE if this NIDB has a non-default IPX encapsulation.
 */

static boolean novell_non_default_encapsulation (nidbtype *nidb, parseinfo *csb)
{
    idbtype *swidb = nidb->idb;

    if ((idb_is_ether_like(swidb) || idb_is_bvi(swidb)) &&
	(nidb->enctype != ET_NOVELL_ETHER))
	return(TRUE);
    if (idb_is_tokenring(swidb) && (nidb->enctype != ET_SAP))
	return(TRUE);
    if (idb_is_fddi(swidb) && (nidb->enctype != ET_SNAP))
	return(TRUE);
    return(FALSE);
}

/*
 * novell_get_default_encapsulation
 *
 * Return the default_encapsulation type based on the hwidb
 */
int novell_get_default_encapsulation (idbtype *swidb)
{
    /*
     * Set default encapsulation per the media:
     */
    if (idb_is_ether_like(swidb) || idb_is_bvi(swidb))
	return(ET_NOVELL_ETHER);
    if (idb_is_tokenring(swidb))
	return(ET_SAP);
    if (idb_is_fddi(swidb))
	return(ET_SNAP);
    if (idb_is_tunnel(swidb))
	return(ET_TUNNEL);
    if (idb_is_serial(swidb))
	return(ET_HDLC);
    if (idb_is_atmhw(swidb)) {		/* ATM LANE is etherlike, above */
	return(ET_SNAP);
    }
    return(ET_NULL);
}

/*
 * novell_valid_encapsulation
 *
 * Validate a user-supplied encapsulation type.
 */

static boolean novell_valid_encapsulation (
    idbtype *swidb,
    int enctype,
    boolean explicit,
    enum RES resolvemethod)
{
    switch (enctype) {
	
      case ET_NOVELL_ETHER:
      case ET_ARPA:
	return(idb_is_ether_like(swidb) || idb_is_bvi(swidb) ||
	       /*
		* Allow all 4 encapsulation types for ATM if we are
		* reading it from non volatile memory. The interface is
		* probably configured for LANE but it has not come up yet.
		*/
	       (idb_is_atmhw(swidb) && (resolvemethod != RES_MANUAL)));
      case ET_SNAP:
	return(idb_is_lan(swidb) || idb_is_atmhw(swidb));
      case ET_NOVELL_FDDI:
	return(idb_is_fddi(swidb));
      case ET_SAP:
	return(idb_is_lan(swidb) ||
	       (idb_is_atmhw(swidb) && (resolvemethod != RES_MANUAL)));
      case ET_TUNNEL:
	return(idb_is_tunnel(swidb));
      case ET_NULL:
	return(idb_is_serial(swidb));
      case ET_HDLC:
	return (idb_is_serial(swidb) && (!explicit
					 || idb_is_hdlc(swidb)));
      case ET_SDE:
	return(is_sde_capable(swidb));
      case ET_ISL:
	return(is_isl_vlan_capable(swidb));
      default:
	return(FALSE);
    }
}

/*
 * These routines handle the various novell interface specific commands
 */
static void novell_network_ifcommand (boolean interactive, boolean enabling,
				      idbtype *idb, int int1, int int2, 
				      int int3, enum RES resolvemethod)
{
    hwidbtype *hwidb = hwidb_or_null(idb);
    nidbtype *nidb;
    boolean deleting_primary, deleted_one;
    ulong net;
    int enctype;
    boolean secondary, primary, enctype_given;
    novpdbtype *pdb;
    int i;
    novndbtype *ndb;
    ulong old_net;
    
    /* Odd.. */
    if (hwidb == NULL)
	return;
    
    if (!novell_running || novell_shutting_down)
	return;

    FOR_ALL_NIDBS(nidb) {
	if ((nidb->idb == idb) &&
	    (nov_ipxwan_on_nidb(nidb))) {
	    if (interactive)
		printf("\nIPX: Do not use the 'network' command on interfaces configured for IPXWAN");
	    return;
	}
    }
    if (enabling == FALSE) {
	if (idb->novellnets) {
	    /*
	     * Delete a specified network, or all networks if
	     * none specified. Note that if the primary interface
	     * is specified, all secondary networks are also
	     * deleted.
	     */
	    deleting_primary =
		((int1 == 0xffffffff) ||
		 (idb->primary_nidb &&
		 (int1 == idb->primary_nidb->novellnet)));

	    deleted_one = FALSE;
	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb == idb) &&
		    ((deleting_primary) || (nidb->novellnet == int1))) {
		    deleted_one = TRUE;
		    novell_delete_network(nidb);
		}
	    }
	    if (!deleted_one) {
		if (interactive)
		    printf("%%IPX network %x not present on interface %s",
			   int1, idb->namestring);
	    }
	    return;
	}
	return;			/* No networks present ... return silently */
    }

    /*
     * Add a new network. Perform necessary screening. Note
     * that we can be backwards compatible with previous
     * behavior if only a single network is defined. The network
     * and encapsulation commands may be separate. Additional
     * networks must include explicit encapsulation. The following
     * works:
     *
     *      novell network 1234
     *      novell encapsulation arpa
     *      novell network 2345 encapsulation novell-ether secondary
     */

    /*
     * Establish some programmer friendly notation.
     */
    net       = int1;
    if ((net == 0) && (!is_p2p(idb))) {
	if (interactive)
	    printf("\n%%Unnumbered on Point-to-Point (non-multi-access) interfaces only");
	return;
    }
    enctype   = int2;
    secondary = int3;
    primary   = !secondary;
    /*
     * Networks -1, and (-2 when default route enabled) are always illegal
     */
    if (net == -1L || ((net == NOV_RIP_DEFAULT_ROUTE) && novell_default_route)) {
	if (interactive)
	    printf("\n%%IPX network %x is illegal", net);
	return;
    }

    /*
     * Set target encapsulation. If not specified use implicit
     * encap from last "novell encapsulation" command.
     * If still not specified, use generic default.
     */
    if (enctype == ET_NULL) {
	if (idb->isl_vlan != NULL)
	    enctype = ET_ISL;
	else if (idb->sde_said_db != NULL)
	    enctype = ET_SDE;
        else if ((enctype = implicit_enctype) == ET_NULL)
            enctype = novell_get_default_encapsulation(idb);
        enctype_given = FALSE;
    } else {
	if (IS_VLAN_CONFIGURED_SUBINTERFACE(idb)) {
            if (interactive) {
                printf("\n\nvLAN encapsulation present, therefore"
                       " non-default encapsulation not allowed\n\n");
            }
            return;
        }
        enctype_given = TRUE;
    }

    /*
     * Validate specified encapsulation per the media.
     */
    if (enctype != ET_NULL) {
	if (!novell_valid_encapsulation(idb, enctype, enctype_given, 
					resolvemethod)) {
	    if (interactive)
		printf("\n%%IPX encapsulation %s is not allowed on %s",
		       encstring(enctype), hwidb->name);
	    return;
	}
    }
    if ((enctype == ET_NOVELL_FDDI) &&
	(hwidb->status & IDB_FDDI) &&
	rcache_cbus_sse_on_this_hwidb(hwidb)) {
	if (interactive)
	    printf("\n%%IPX autonomous/SSE switching not"
		   " supported if novell-fddi configured");
	return;
    }

    /*
     * Disallow a secondary network if no primary exists:
     */
    if ((secondary) && (IPX_PRIMARY_NIDB(idb) == NULL)) {
	if (interactive)
	    printf("\n%%Configure a primary IPX network first");
	return;
    }
    /*
     * Make sure the net doesn't already exist on another interface,
     * or that it is already being deleted at this time.
     */
    if (net != 0) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->novellnet != net)
		continue;
	    if (nidb->idb != idb) {
		if (interactive)
		    printf("%%IPX network %x already exists on interface %s",
			   nidb->novellnet, nidb->idb->namestring);
		return;
	    }
	    if (!NOVELL_NIDB_CONFIGURABLE(nidb)) {
		if (interactive)
		    printf("\nNetwork %x is being deleted, and cannot be configured at this time",
			   nidb->novellnet);
		return;
	    }
	}
    }
    /*
     * If primary, check that the network/encap is not already present
     * on secondary.
     *
     * If secondary, check that the network/encap is not already
     * present.
     */
    if (primary) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb != idb)
		continue;
	    if (IPX_PRIMARY_NIDB(idb) == nidb)
		continue;
	    if (nidb->novellnet == net) {
		if (interactive)
		    printf("%%IPX secondary network %x"
			   " already exists on this interface",
			   nidb->novellnet);
		return;
	    } else if (((enctype != ET_HDLC) ||
			(enctype != ET_ISL) ||
			(enctype != ET_SDE)) &&
		       (enctype == nidb->enctype)) {
		if (interactive)
		    printf("%%Encapsulation already in use by"
			   " IPX secondary network %x",
			   nidb->novellnet);
		return;
	    }
	}
    } else {
	/*
	 * Screen secondary network:
	 */
	/*
	 * If network number matches primary then error.
	 */
	if (IPX_PRIMARY_NIDB(idb)->novellnet == net) {
	    if (interactive)
		printf("%%IPX primary network %x"
		       " already exists on this interface", net);
		return;
	}
	if ((enctype != ET_HDLC) && (enctype != ET_SDE) &&
	    (enctype != ET_ISL)) {
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb != idb)
		    continue;
		/*
		 * If encap matches existing entry then error or NOP:
		 */
		if (nidb->enctype != enctype)
		    continue;
		if (nidb->novellnet != net)
		    if (interactive)
			printf("%%Encapsulation already in use"
			       " by IPX network %x", nidb->novellnet);
		return;
	    }
	}
    }

    /*
     * Make sure the encap doesn't already exist on this physical
     * interface. The exception being multiple vLAN encapsulating
     * or ATM subinterfaces.
     */
    if (idb_is_ieee(idb) && !is_atm(idb->hwptr) &&
	   !IS_VLAN_CONFIGURED_SUBINTERFACE(idb)) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb)
		continue;
	    if (nidb->idb->hwptr != hwidb)
		continue;
	    if (nidb->enctype != enctype)
		continue;
	    if (interactive)
		printf("%%Encapsulation already in use by "
		       "IPX network %x, on interface %s",
		       nidb->novellnet, nidb->idb->namestring);
	    return;
	}
    }
    /*
     * If a primary network exists and the new net is marked as
     * primary, change the network number and/or encapsulation
     * of the existing network. Adjust network params as appropriate.
     */
    if ((IPX_PRIMARY_NIDB(idb) != NULL) && (primary)) {
	nidb = IPX_PRIMARY_NIDB(idb);
	if ((nidb->novellnet != net) || (nidb->enctype != enctype)) {
	    reg_invoke_ipx_nidb_network_changed(nidb, IPX_ROUTE_EVENT_DELETED);
	    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	    nov_network_finish(&novrip_pdb, nidb->novellnet, FALSE);
	    old_net = nidb->novellnet;
	    novell_nidb_set_network_number(nidb, net);
	    nidb->enctype = enctype;
	    nidb->explicit_encap = enctype_given;
	    /*
	     * Remember if the primary network was given with or
	     * without explicit encapsulation. This information
	     * will be used when the configuration is written to
	     * NVM in order to keep things backwards compatible
	     * with 9.1 and earlier.
	     */
	    nidb->old_style = (primary && (int2 == ET_NULL));
	    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	    nov_network_finish(&novrip_pdb, nidb->novellnet, TRUE);
	    reg_invoke_ipx_nidb_network_changed(nidb, IPX_ROUTE_EVENT_ADD);
	    /*
	     * Now handle igrp
	     */
	    reg_invoke_novigrp_address_command(idb, net, TRUE);
	}
	return;
    } else {
	/*
	 * If an existing secondary network is specified, change
	 * encapsulation. Toss routes and SAPs learned via this
	 * (virtual) interface. They are unlikely to remain
	 * valid given the new encap.
	 */
	if (secondary) {
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb != idb)
		    continue;
		if (nidb->novellnet != net)
		    continue;
		novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
		nidb->enctype = enctype;
		nidb->explicit_encap = enctype_given;
		novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
		return;
	    }
	}
	/*
	 * The specified primary or secondary network is new.
	 * Allocate and initialize (virtual) network descriptor:
	 */
	nidb = novell_nidb_new(idb, net, enctype, primary,
			       (primary && (int2 == ET_NULL)), FALSE, TRUE,
			       enctype_given);
	if (nidb == NULL) {
	    if (interactive)
		printf("\n%%no memory to create new IPX network block");
	    return;
	}
    }
    /*
     * Fiddle MAC address each time a network is configured.
     */
    fiddle_node_address(idb);
    /*
     * If the user has configured rip not to run on any interface,
     * then don't start it up on this new network.
     */
    if (!novrip_pdb.all_nets)
	nov_network_finish(&novrip_pdb, nidb->novellnet, TRUE);
    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);

    /*
     * See if this new network has already been included
     * in an IGRP AS via the "ipx router eigrp <AS>" command.
     * If so, begin redistribution into IGRP. This is a rare
     * case, usually the "network" interface subcommand will
     * be executed prior to the "ipx router" command.
     */
    for (pdb = novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype != NET_IGRP) 
	    continue;
	for (i = 0; i < pdb->netcount; i++) {
	    if (pdb->networks[i] == net || pdb->all_nets) {
		ndb = novrip_find_rt(net, NOV_NETMASK_ALL,
				     IPX_ROUTE_MATCH_EXACT);
		nov_set_ndb_flags(ndb, pdb, TRUE);
		reg_invoke_novigrp_network_command(pdb, net, TRUE);
		nov_network_finish(pdb, net, TRUE);
	    }
	}	
    }
}

static void novell_advertise_default_route_only_ifcommand (boolean interactive,
						    boolean enabling,
						    idbtype *idb, int int1)
{
    nidbtype *nidb;
			      
    if (!novell_running || novell_shutting_down)
	return;

    FOR_ALL_NIDBS(nidb) {
	if ((nidb->idb == idb) &&
	    ((nov_ipxwan_on_nidb(nidb) ? nidb->nov_ipxwan_external_net :
	      nidb->novellnet) == int1)) {
	    if (!NOVELL_NIDB_CONFIGURABLE(nidb)) {
		if (interactive)
		    printf("\nNetwork %x is being deleted, and cannot be configured at this time",
			   nidb->novellnet);
		return;
	    }
	    nidb->adv_default_route_only = enabling;
	    break;
	}
    }
    if (!nidb)
	if (interactive)
	    printf("\nIPX: Network %x not configured on interface %s",
		   int1, idb->namestring);
}

static void novell_down_ifcommand (boolean interactive, boolean enabling,
			    idbtype *idb, int int1)
{
    nidbtype *nidb;
			      
    if (!novell_running || novell_shutting_down || !novell_default_route)
	return;

    FOR_ALL_NIDBS(nidb) {
	if ((nidb->idb == idb) &&
	    ((nov_ipxwan_on_nidb(nidb) ? nidb->nov_ipxwan_external_net :
	      nidb->novellnet) == int1)) {
	    if (!NOVELL_NIDB_CONFIGURABLE(nidb)) {
		if (interactive)
		    printf("\nNetwork %x is being deleted, and cannot be configured at this time",
			   nidb->novellnet);
		return;
	    }
	    if (enabling) {
		if (NOVELL_NIDB_ISUP(nidb)) {
		    nidb->administratively_shutdown = TRUE;
		    novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN_PENDING);
		}
	    } else {
		if (NOVELL_NIDB_ISDOWN(nidb)) {
		    if (!nidb->administratively_shutdown) {
			if (interactive)
			    printf("\nNetwork %x was not shutdown with the 'down' command",
				   int1);
			return;
		    }
		    nidb->administratively_shutdown = FALSE;
		    novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
		} else if (NOVELL_NIDB_DOWN_MODE(nidb)) {
		    if (interactive)
			printf("\nNetwork %x is in the midst of coming down. Try again later",
			       int1);
		    return;
		}
	    }
	    break;
	}
    }
    if (!nidb)
	if (interactive)
	    printf("\nIPX: Network %x not configured on interface %s",
		   int1, idb->namestring);
}			    

static void novell_delay_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    ushort old_delay;
    nidbtype *nidb;

    if (!novell_running || novell_shutting_down)
      return;
    old_delay = idb->novell_delay;
    if (enabling) {
	idb->novell_delay = int1;
    } else {
	idb->novell_delay = 0;
    }
    /* Sigh. They've changed the delay. Restart the world */
    if (old_delay != idb->novell_delay) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb) {
		novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
		novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
		reg_invoke_ipx_nidb_bw_delay_changed(nidb);
	    }
	}
    }
}

void novell_sapint_ifcommand (boolean interactive, boolean enabling,
			      idbtype *idb, int int1)
{
    nidbtype *nidb;

    if (!novell_running || novell_shutting_down) {
	return;
    }
    if (!idb->novellnets) {
	if (interactive)
	    printf("\n%%Configure an IPX network first");
  	return;
    }

    if (idb_is_lan(idb)) {
	if (interactive)
	    printf("\n%% Warning: Changing the SAP interval on LANs is dangerous!");
    }
    if (enabling) {
	idb->novell_interval = int1;
    } else {
	idb->novell_interval = 1;
    }
    /*
     * Force out an update and reschedule next to reflect new value.
     */
    FOR_ALL_NIDBS(nidb) {
 	if (nidb->idb == idb) {
 	    novell_disable_full_sap_updates(nidb);
 	    novell_enable_full_sap_updates(nidb);
 	}
    }
}

void novell_updatetime_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    nidbtype *nidb;

    if (!novell_running || novell_shutting_down)
      return;
    if (!idb->novellnets) {
	printf("\n%%Configure an IPX network first");
	return;
    }
    if (!enabling) {
	idb->nov_update_interval = NOVELL_RIP_DEFAULT;
    } else {
	idb->nov_update_interval = int1 * ONESEC;
    }

    /*
     * Force out an update and reschedule next to reflect new value.
     */
    FOR_ALL_NIDBS(nidb) {
 	if (nidb->idb == idb) {
	    novell_disable_full_rip_updates(nidb);
	    novell_enable_full_rip_updates(nidb);
 	}
    }
}

static void novell_sap_delay_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (!enabling) {
	idb->novell_sap_delay = novell_def_output_sap_dly;
    } else {
	idb->novell_sap_delay = int1;
    }
}

static void novell_rip_delay_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (!enabling) {
	idb->novell_rip_delay = novell_def_output_rip_dly;
    } else {
	idb->novell_rip_delay = int1;
    }
}

static void novell_gns_response_delay_ifcommand (boolean enabling,
					  idbtype *idb, int grd)
{
/* If no interface gns delay specified default to the global setting */
    if (!novell_running || novell_shutting_down)
	return;
    idb->nov_gns_delay = enabling ? grd :
	novell_gns_response_delay;
}

void novell_gns_reply_disable_ifcommand (boolean enabling, idbtype *idb)
{
    if (!novell_running || novell_shutting_down)
      return;
    /* Yes, the sense is reversed... */
    if (enabling) {
	idb->nov_respond_to_gns = FALSE;
    } else {
	idb->nov_respond_to_gns = TRUE;
    }
}

void novell_rip_multiplier_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (enabling) {
	idb->nov_rip_multiplier = int1;
    } else {
	idb->nov_rip_multiplier = NOVELL_RIP_INVALIDTIME;
    }
}

void novell_sap_multiplier_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (enabling) {
	idb->nov_sap_multiplier = int1;
    } else {
	idb->nov_sap_multiplier = NOVELL_RIP_INVALIDTIME;
    }
}

void novell_rip_max_packetsize_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (enabling) {
	idb->nov_rip_maxpacketsize = int1;
    } else {
	idb->nov_rip_maxpacketsize = NOVELL_RIP_MAXSIZE;
    }
}

void novell_sap_max_packetsize_ifcommand (boolean enabling, idbtype *idb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (enabling) {
	idb->nov_sap_maxpacketsize = int1;
    } else {
	idb->nov_sap_maxpacketsize = NOVELL_SAP_MAXSIZE;
    }
}
static void novell_throughput_ifcommand (boolean enabling,
				       nidbtype *nidb, int int1)
{
    ulong old_throughput;

    if (!novell_running || novell_shutting_down)
      return;

    old_throughput = nidb->novell_link_throughput;
    if (enabling) {
	nidb->novell_link_throughput = int1;
    } else {
	nidb->novell_link_throughput = NOVELL_THROUGHPUT_DEFLT;
    }
    /* Sigh. They've changed the delay. The IPXWAN world must restart */
    if (old_throughput != nidb->novell_link_throughput) {
	novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	reg_invoke_ipx_nidb_bw_delay_changed(nidb);
    }
}
static void novell_link_delay_ifcommand (boolean enabling,
					  nidbtype *nidb, int int1)
{
    ulong old_delay;

    if (!novell_running || novell_shutting_down)
      return;
    old_delay = nidb->novell_link_delay;
    if (enabling) {
	nidb->novell_link_delay = int1;
    } else {
	nidb->novell_link_delay = NOVELL_LINK_DELAY_DEFLT;
    }
    if (old_delay != nidb->novell_link_delay) {
	novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
	reg_invoke_ipx_nidb_bw_delay_changed(nidb);
    }
}
static void novell_spx_idle_time_ifcommand (boolean enabling,
					  nidbtype *nidb, int int1)
{
    if (!novell_running || novell_shutting_down)
      return;
    if (enabling) {
	nidb->novell_spxidletime = int1;
    } else {
	nidb->novell_spxidletime = NOVELL_SPX_IDLE_DEFLT;
    }
}

/*
 * nov_update_global_acl_flags
 *
 * Set or clear the global fastswitch acl flag. This flag is
 * set if any condition is known which requires access list
 * checking by the fastswitch code. Also, update per swidb
 * input and output check flags. Conditions include:
 *
 *     - input access list
 *     - output access list
 *     - accounting enabled
 */
void nov_update_global_acl_flags (void)
{
    hwidbtype *hwidb;
    idbtype *swidb;
    boolean global_checks = FALSE;
    boolean input_checks, output_checks;

    FOR_ALL_HWIDBS(hwidb) {
	hwidb->novell_input_acl_checks = FALSE;
	FOR_ALL_SWIDBS_ON_HW(hwidb, swidb) {

	    /*
	     * Qualify this check else it fails at config time:
	     */
	    if (systeminit_flag && !swidb->novellnets)
		continue;

	    input_checks = output_checks = FALSE;

	    if ((swidb->novell_accesslist != 0) ||
		(swidb->ipx_accounting != 0)) {
		output_checks = TRUE;
	    }
	    
	    if (swidb->novell_inaccesslist != 0) {
		input_checks = TRUE;
	    }
	    global_checks |= (input_checks || output_checks);
	    hwidb->novell_input_acl_checks |= input_checks;
	    swidb->novell_output_acl_checks = output_checks;
	}
    }
    novell_global_acl_checks = global_checks;
}

/*
 * update_novell_inaccesslist
 *
 * Set or clear an input access list. All changes to this parameter
 * should be done via this common routine in order to ensure the
 * consistency of fastswitch acl flags.
 */
static void update_novell_inaccesslist (idbtype *swidb, int list)
{
    swidb->novell_inaccesslist = list;
    nov_update_global_acl_flags();
}

/*
 * update_novell_accesslist
 *
 * Set or clear an output access list. All changes to this parameter
 * should be done via this common routine in order to ensure the
 * consistency of fastswitch acl flags.
 */
static void update_novell_accesslist (idbtype *swidb, int list)
{
    swidb->novell_accesslist = list;
    nov_update_global_acl_flags();
}

/*
 * update_novell_helperlist
 *
 * Set or clear a helper access list.
 */
static void update_novell_helperlist (idbtype *swidb, int list)
{
    swidb->novell_helperlist = list;
}

/*
 * novell_routecache_ifcommand
 *
 * Process the route-cache command.
 */
static void novell_routecache_ifcommand (
    parseinfo *csb,
    idbtype *idb,
    int bits)
{
    hwidbtype *hwidb;
    nidbtype *nidb;
    boolean cbus, sse;

    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL)
	return;
    cbus = (bits == NOVELL_RCACHE_CBUS);
    sse = (bits == NOVELL_RCACHE_SSE);

    /*
     * Skip check for IPX networks if starting up. This means that
     * we allow route cache commands on a primary interface, even
     * if all networks are on subinterfaces. If no networks are
     * configured, the flag has no effect and is not output via
     * nv_write(). However, after startup, we'll still notify the
     * user if no IPX networks are configured.
     */
    if (systeminit_flag)
	if (!novell_on_hwidb(hwidb)) {
	    printf("\n%%IPX not enabled");
	    printf(nov_on_itf, hwidb->hw_namestring);
	    return;
	}
    if (csb->set_to_default) {
	if (!cbus && !sse)
	    csb->sense = TRUE;
    }
    if (!csb->sense) {
	if (cbus || sse) {
	    /*
	     * Do not allow FS enable if watchdog spoofing is configured.
	     * Also don't allow it if IPX hdr compression is configured.
	     * This check is a little convoluted since FS state is kept
	     * in the hwidb, while watchdog is kept per idb, and IPX
	     * header compression is kept per nidb....
	     */
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb->hwptr != idb->hwptr)
		    continue;
		if (nidb->novell_spoof_flag) {
		    printf("\n%%IPX/SPX spoofing must be removed from %s",
			   nidb->idb->namestring);
		    return;
		}
		if (nidb->compression_enabled) {
		    printf(nov_remove_cipx, " from %s", nidb->idb->namestring);
		    return;
		}
	    }
	    if (cbus)
		hwidb->novell_routecache &= ~NOVELL_RCACHE_CBUS;
	    if (sse)
		hwidb->novell_routecache &= ~NOVELL_RCACHE_SSE;
	} else {
	    hwidb->novell_routecache = 0;
	}
    } else {
	/* cipx and spoofing should not be enabled for FS. Check in
	 * the common path.
	 */
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb->hwptr != idb->hwptr)
		continue;
	    if (nidb->novell_spoof_flag) {
		printf("\n%%IPX/SPX spoofing must be removed from %s",
		       nidb->idb->namestring);
		return;
	    }
	    if (nidb->compression_enabled) {
		printf(nov_remove_cipx, " from %s",
		       nidb->idb->namestring);
		return;
	    }
	}
	
	if (cbus || sse) {
	    if (cbus && (!novell_auto_sw_supported || 
			 !(hwidb->status & IDB_CBUS))) {
		printf(nov_no_autosw);
		printf(nov_on_itf, hwidb->hw_namestring);
		return;
	    }
	    if (cbus && (is_ppp(hwidb) ||
			 is_frame_relay(hwidb) || 
			 is_atm_dxi(hwidb) ||
			 is_smds(hwidb) ||
			 is_atm(hwidb) ||
			 is_ddr(hwidb))) {
		printf(nov_no_autosw);
		printf(nov_on_xxx_itf,
		       (is_ppp(hwidb) ? "PPP" :
			(is_atm_dxi(hwidb) ? "atm-dxi" :
			 (is_atm(hwidb) ? "atm" :
			  (is_smds(hwidb) ? "smds" :
			   (is_ddr(hwidb) ? "ddr" : "Frame Relay"))))),
		       idb->namestring);
		return;
	    }
	    if (sse && !(hwidb->status & IDB_CBUS)) {
		printf(nov_no_sse);
		printf(nov_on_itf, hwidb->hw_namestring);
		return;
	    }
	    if (sse && (is_smds(hwidb) ||
			is_ddr(hwidb))) {
		printf(nov_no_sse);
		printf(nov_on_xxx_itf,
		       (is_smds(hwidb) ? "smds" : "ddr"),
		       hwidb->hw_namestring);
		return;
	    }
	    /*
	     * Do not allow AS enable if an inaccess-group is configured
	     *  on *any* interface.
	     * Do not allow AS enable if an access-group is configured.
	     * Do not allow AS enable if watchdog spoofing is configured.
	     * Do not allow AS enable if IPX header compression is
	     *                           configured.
	     * Do not allow AS enable if fddi raw encap in use
	     * Do not allow AS enable if PPP interface
	     * This check is a little convoluted since AS state is kept
	     * in the hwidb, while accesslist are kept per swidb, and 
	     * there may be multiple networks/swidb's.
	     */
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb->novell_inaccesslist) {
		    printf("\n%%IPX input access-group must be"
			   " removed from all interfaces");
		    return;
		}
		if (nidb->idb->hwptr == idb->hwptr) {
		    if (nidb->idb->novell_accesslist) {
			printf("\n%%IPX access-group must be removed "
			       "from %s", nidb->idb->namestring);
			return;
		    }
		    if (nidb->idb->ipx_accounting) {
			printf("\n%%IPX accounting must be removed"
			       " from %s", nidb->idb->namestring);
			return;
		    }
		    if (nidb->enctype == ET_NOVELL_FDDI) {
			printf("\n%%IPX novell-fddi encap must be removed"
			       " from %s", nidb->idb->namestring);
			return;
		    }
		}
	    }
	    if (is_lex(hwidb)) {
		printf(nov_no_autosw);
		printf(nov_on_itf, idb->namestring);
		return;
	    }
	    if (cbus) {
		hwidb->novell_routecache |=
		    (NOVELL_RCACHE_CBUS | NOVELL_RCACHE);
		delete_fair_queue(hwidb);
	    }
	    if (sse) {
		if (registry_populated(REG_SSE) &&
		    reg_invoke_sse_present()) {
		    hwidb->novell_routecache |=
			(NOVELL_RCACHE_SSE | NOVELL_RCACHE);
		    delete_fair_queue(hwidb);
		} else {
		    printf("\n%%SSE not present");
		}
	    }
	} else {
	    hwidb->novell_routecache &=
		~(NOVELL_RCACHE_CBUS | NOVELL_RCACHE_SSE);
	}
	hwidb->novell_routecache |= NOVELL_RCACHE;
    }
    if (is_atm(hwidb) && if_has_multiple_mtu(hwidb)) {
	hwidb->novell_routecache &= ~(NOVELL_RCACHE_SSE);
    }
    nov_fastsetup(hwidb);
}

/*
 * novell_enctype_ifcommand
 *
 * Process the novell encapsulation command.  Encapsulations supplied
 * with the novell network command are processed with the network
 * number.
 */
static void novell_enctype_ifcommand (
    boolean enabling,
    idbtype *idb,
    int encap,
    enum RES resolvemethod)
{
    nidbtype *nidb;
    hwidbtype *hwidb = idb->hwptr;

    /*
     * Screen encapsulation by media type.
     *
     * If ISL vLAN encapsulation on subinterface don't allow
     * non-default Novell flavours.
     */
    if (IS_VLAN_CONFIGURED_SUBINTERFACE(idb) == TRUE) {
	printf("\n\nThis subinterface is already configured for vLAN "
	       "encapsulation therefore\nthe IPX encapsulation cannot "
	       "be changed from the default novell-ether.\n");
	return;
    }

    if (enabling) {
	if (!novell_valid_encapsulation(idb, encap, TRUE, resolvemethod)) {
	    printf("\n%%IPX encapsulation type %s is not allowed on %s",
		   encstring(encap), hwidb->name);
	    return;
	}
	/*
	 * Remember what was specified in case no network
	 * command has been issued.
	 */
	implicit_enctype = encap;
    } else {
	implicit_enctype = novell_get_default_encapsulation(idb);
    }

    /*
     * If no network present, exit for now.
     */
    if (idb->novellnets == 0) {
	return;
    }
    /*
     * Check that the encapsulation doesn't already exist on
     * this logical interface.
     */
    FOR_ALL_NIDBS(nidb) {
	if (nidb->idb != idb)
	    continue;
	if (implicit_enctype != nidb->enctype)
	    continue;
	if (IPX_NIDB_IS_SECONDARY(nidb)) {
	    printf("%%IPX encapsulation already exists"
		   " on secondary network %x",
		   nidb->novellnet);
	    return;
	}
    }
    /*
     * Check that the encapsulation doesn't already exist on
     * this physical interface.
     */
    if (!is_atm(idb->hwptr) && !is_vlan_capable(idb)) {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb)
		continue;
	    if (nidb->idb->hwptr != hwidb)
		continue;
	    if (implicit_enctype != nidb->enctype)
		continue;
	    printf("%%Encapsulation already in use by "
		   "IPX network %x, on interface %s",
		   nidb->novellnet, nidb->idb->namestring);
	    return;
	}
    }
    /*
     * Possibly change the encap type of our primary net. If so,
     * invalidate related cache/route/SAP data.
     */
    nidb = IPX_PRIMARY_NIDB(idb);
    if (!NOVELL_NIDB_CONFIGURABLE(nidb)) {
	printf("\nNetwork %x is being deleted, and cannot be configured at this time",
	       nidb->novellnet);
	return;
    }
    if (nidb->enctype != implicit_enctype) {
	novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	nidb->enctype = implicit_enctype;
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
    }
    nidb->explicit_encap = TRUE;
}

static void novell_hlpaddr_ifcommand (
    boolean enabling,
    idbtype *idb,
    addrtype *addr)
{
    uchar helperaddr[IEEEBYTES];
    ulong helpernet = 0;    /* Initialized to make compiler happy */
    novaddtype *ptr;
    boolean found;

    /* Get the network number and host address. */
    if (addr->type == ADDR_NOVELL) {
	helpernet = addr->nov_addr.net;
	ieee_copy(addr->nov_addr.host, helperaddr);
    }
    if (!enabling) {
	/*
	 * There was a preceding 'no'.  If we parsed an address,
	 * delete the entry, otherwise clear the entire list.
	 */
	if (addr->length != 0) {
	    for (ptr = idb->nov_helpernetQ.qhead; ptr; ptr = ptr->next) {
		if ((ptr->novell_net == helpernet) &&
		    ieee_equal(helperaddr, ptr->novell_host)) {
		    unqueue(&idb->nov_helpernetQ, ptr);
		    free(ptr);
		    break;
		}
	    }
	} else {
	    while ((ptr = dequeue(&idb->nov_helpernetQ))) {
		free(ptr);
	    }
	}
	return;
    }

    /*
     * Make sure a helper net of -1 is followed by a host address
     * of ffff.ffff.ffff, anything else is illegal.
     */
    if ((helpernet == -1L) && !ieee_equal(helperaddr, novell_allhosts)) {
	printf(nov_badaddr, "malformed address");
	return;
    }
    /*
     * Go through the current list, and see if the entry is already
     * there. If so, bail.
     */
    found = FALSE;
    for (ptr = idb->nov_helpernetQ.qhead; ptr && !found; ptr = ptr->next) {
	if ((ptr->novell_net == helpernet) &&
	    (ieee_equal(helperaddr, ptr->novell_host))) {
	    found = TRUE;
	}
    }
    if (!found) {
	/* Valid helper address; add to list */
	ptr = malloc(sizeof(novaddtype));
	if (ptr) {
	    ptr->novell_net = helpernet;
	    ieee_copy(helperaddr, ptr->novell_host);
	    enqueue(&idb->nov_helpernetQ, ptr);
	}
    }
}

/*
 * novellif_command
 * Process Novell interface keywords.
 */
void novellif_command (parseinfo *csb)
{
    idbtype *idb;
    hwidbtype *hwidb;
    nidbtype *nidb = NULL, *nnidb = NULL;
    novaddtype *ptr;
    boolean found;
    boolean all_nets_deleted;
    idbtype *target_idb;
    novell_client *client;
    int count, Qsize;

    /*
     * Parse given args if any
     */
    idb = csb->interface;
    hwidb = hwidb_or_null(idb);
    if (!novell_running || novell_shutting_down) {
	if (!csb->nvgen) {
	    printf("\n%%Must give \"ipx routing\" command first");
	}
	return;
    }
    /*
     * Forget last "novell encapsulation" if idb has changed.
     */
    if (implicit_idb != idb) {
	implicit_enctype = ET_NULL;
	implicit_idb = idb;
    }
    if (csb->nvgen) {
	/* Always bail if IPX not enabled */
	if (!novell_running || novell_shutting_down)
	    return;
	/*
	 * Immediate bail if there are no IPX networks enabled on this
	 * interface. Do this UNLESS the 'route-cache' command is given.
	 * This command must exist even if the primary interface has been
	 * de-configured for IPX, because the sub-interfaces underneath
	 * it may still be enabled for IPX.. The route-cache parameter is
	 * only configurable on the primary software interface. Note that
	 * route-cache flags are maintained on the hardware interface.
	 *
	 * Another exception is if this is the NOVELL_PPPCLIENT, since this
	 * applies to interfaces that do not have a network to call their
	 * own.
	 */
	if (!((csb->which == NOVELL_PPPCLIENT) && (idb->owner_nidb))) {
	    if (csb->which != NOVELL_ROUTEC) {
		if (!novell_on_idb(idb)) {
		    return;
		}
		/*
		 * Also bail if the network(s) associated with this interface have
		 * been deleted. Note that networks must hang around until all
		 * poisons/reverse poisons have been sent. This could be a very
		 * long time if the internet is large and/or significant output
		 * rip/sap delays have been configured. We do not want to write
		 * deleted network state to nvram while this is happening. On the
		 * other hand, only a subset of networks on an interface may be
		 * deleted. In this case we want to update nvram with everything
		 * except the deleted network(s).
		 */
		all_nets_deleted = TRUE;
		FOR_ALL_NIDBS(nidb) {
		    if ((nidb->idb == idb) && (!NOVELL_NIDB_DELETED_MODE(nidb))) {
			all_nets_deleted = FALSE;
			break;
		    }
		}
		if (all_nets_deleted)
		    return;
	    } else {
		/*
		 * route-cache commands will be processed ONLY on the primary
		 * software idb. Note however that novell may enabled on ANY
		 * software idb associated with a given hardware idb.
		 */
		if (!novell_on_hwidb(hwidb) || (idb != hwidb->firstsw)) {
		    return;
		}
	    }
	}
	
	switch (csb->which) {
	  case NOVELL_NETWORK:
	    /*
	     * Run through the network list once looking for an old-style
	     * entry. If found, write it as,
	     *
	     *     "novell network xxx"
	     *     "novell encapsulation yyy"  (UNLESS yyy == default encap)
	     *
	     * Except where a vLAN encapsulation is present.
	     */

	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb == idb) &&
		    (!nov_ipxwan_on_nidb(nidb)) &&
		    (NOVELL_NIDB_CONFIGURABLE(nidb)) &&
		    (nidb->old_style)) {
		    if (novell_non_default_encapsulation(nidb, csb) &&
			(IS_VLAN_CONFIGURED_SUBINTERFACE(idb) == FALSE))
			nv_write(TRUE, "ipx encapsulation %s",
				 encstring(nidb->enctype));
		    nv_write(TRUE, "%s ", csb->nv_command);
		    if (!NOVELL_NIDB_UNNUMBERED(nidb))
		      nv_add(TRUE, "%x", nidb->novellnet);
		    else
		      nv_add(TRUE, "unnumbered");
		    break;
		}
	    }
	    if ((nidb != NULL) && (IPX_PRIMARY_NIDB(idb) != nidb))
		printf("\n%%Warning: IPX network data base is inconsistent");
	    /*
	     * Run through the network list a second time and output anything
	     * not in the old style in the new style, i.e.,
	     *
	     *     "novell network xxx encap yyy [secondary]"
	     */
	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb == idb) && (!(nidb->old_style)) &&
		    (!nov_ipxwan_on_nidb(nidb)) &&
		    NOVELL_NIDB_CONFIGURABLE(nidb)) {
		    nv_write(TRUE, "%s ", csb->nv_command);
		    if (!NOVELL_NIDB_UNNUMBERED(nidb))
		      nv_add(TRUE, "%x", nidb->novellnet);
		    else
		      nv_add(TRUE, "unnumbered");
		    if (IPX_NIDB_IS_SECONDARY(nidb) ||
			(novell_non_default_encapsulation(nidb, csb) &&
			 (IS_VLAN_CONFIGURED_SUBINTERFACE(idb) == FALSE))) {
			nv_add(TRUE, " encapsulation %s",
			       encstring(nidb->enctype));
			nv_add(IPX_NIDB_IS_SECONDARY(nidb), " secondary");
		    }
		}
	    }
	    break;
	  case NOVELL_ADVERTISE_DEFAULT_ROUTE_ONLY:
	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb == idb) &&
		    novell_default_route &&
		    (nidb->adv_default_route_only) &&
		    NOVELL_NIDB_CONFIGURABLE(nidb)) {
		    nv_write(TRUE, "%s %x", csb->nv_command,
			     nov_ipxwan_on_nidb(nidb) ?
			     nidb->nov_ipxwan_external_net : nidb->novellnet);
		}
	    }
	    break;
	  case NOVELL_DOWN:
	    FOR_ALL_NIDBS(nidb) {
		if ((nidb->idb == idb) &&
		    NOVELL_NIDB_CONFIGURABLE(nidb) &&
		    nidb->administratively_shutdown) {
		    nv_write(TRUE, "%s %x", csb->nv_command,
			     nov_ipxwan_on_nidb(nidb) ?
			     nidb->nov_ipxwan_external_net : nidb->novellnet);
		}
	    }
	    break;
	  case NOVELL_PPPCLIENT:
	    FOR_ALL_NIDBS(nidb) {
		if ((Qsize = QUEUESIZE(&nidb->clientQ))) {
		    for (count = 0; count < Qsize; count++) {
	                client = dequeue(&nidb->clientQ);
	                enqueue(&nidb->clientQ, client);
			if (client->idb == idb) {
	    		    nv_write(idb->owner_nidb != NULL, 
					"%s %s", csb->nv_command,
					idb->owner_nidb->idb->namestring);
			}
		    }
		 }
	    }
	    break;

	  case NOVELL_ACCESS:
	    nv_write(idb->novell_inaccesslist, "%s %d in",
		     csb->nv_command, idb->novell_inaccesslist);
	    nv_write(idb->novell_accesslist_def, "%s %d out",
		     csb->nv_command, idb->novell_accesslist_def);
	    break;

	  case NOVELL_ROUTEC:
	    if (GETOBJ(int,1)) {
		if (hwidb->novell_routecache & GETOBJ(int,1))
		    nv_write(TRUE, csb->nv_command);
	    } else
		if (!(hwidb->novell_routecache & NOVELL_RCACHE))
		    nv_write(TRUE, "no %s", csb->nv_command);
	    break;
	  case NOVELL_DELAY:
	    nv_write(idb->novell_delay, "%s %u", csb->nv_command,
		     idb->novell_delay);
	    break;
	  case NOVELL_ENCTYPE:
            /* Output of encapsulation is now handled by NOVELL_NETWORK */
	    break;
	  case NOVELL_SAPINT:
	    nv_write(idb->novell_interval != 1,
		     "%s %d", csb->nv_command,
		     idb->novell_interval);
	    break;
	  case NOVELL_HLPADDR:
	    for (ptr = idb->nov_helpernetQ.qhead; ptr; ptr = ptr->next) {
		if (ptr->novell_net == -1L) {
		    nv_write(TRUE, "%s -1.%e", csb->nv_command,
			     ptr->novell_host);
		} else {
		    nv_write(TRUE, "%s %x.%e", csb->nv_command,
			     ptr->novell_net, 
			     ptr->novell_host);
		}
	    }
	    break;
	  case NOVELL_HLPLIST:
	    nv_write(idb->novell_helperlist,
		     "%s %d", csb->nv_command,
		     idb->novell_helperlist);
	    break;
	  case NOVELL_IN_FILTER:
	    nv_write(idb->nov_in_filterlist,
		     "%s %u", csb->nv_command,
		     idb->nov_in_filterlist);
	    break;
	  case NOVELL_NSQ_OUTFILTER:
	    nv_write(idb->nov_gns_outfilterlist,
		     "%s %u", csb->nv_command,
		     idb->nov_gns_outfilterlist);
	    break;
	  case NOVELL_OUT_FILTER:
	    nv_write(idb->nov_out_filterlist,
		      "%s %u", csb->nv_command,
		      idb->nov_out_filterlist);
	    break;
	  case NOVELL_ROUTER_FILTER:
	    nv_write(idb->nov_router_filterlist,
		      "%s %u", csb->nv_command,
		      idb->nov_router_filterlist);
	    break;
	  case NOVELL_SAP_INFILTER:
	    nv_write(idb->nov_sap_infilterlist,
		      "%s %u", csb->nv_command,
		      idb->nov_sap_infilterlist);
	    break;
	  case NOVELL_SAP_OUTFILTER:
	    nv_write(idb->nov_sap_outfilterlist,
		      "%s %u", csb->nv_command,
		      idb->nov_sap_outfilterlist);
	    break;
	  case NOVELL_SAP_ROUTERFILTER:
	    nv_write(idb->nov_sap_routerfilterlist,
		      "%s %u", csb->nv_command,
		      idb->nov_sap_routerfilterlist);
	    break;
	  case NOVELL_UPDATETIME:
	    nv_write(idb->nov_update_interval != NOVELL_RIP_DEFAULT,
		     "%s %u", csb->nv_command,
		     idb->nov_update_interval/ONESEC);
	    break;
	  case NOVELL_SAP_AFTER_RIP:
	    nidb = IPX_PRIMARY_NIDB(idb);
	    if (nidb != NULL) {
		nv_write(nidb->novell_sap_after_rip, "%s", csb->nv_command);
	    }
	    break;
	  case NOVELL_SAP_DELAY:
	    nv_write((idb->novell_sap_delay != novell_def_output_sap_dly),
		      "%s %u", csb->nv_command,
		      idb->novell_sap_delay);
	    break;
	  case NOVELL_RIP_DELAY:
	    nv_write((idb->novell_rip_delay != novell_def_output_rip_dly),
		      "%s %u", csb->nv_command,
		      idb->novell_rip_delay);
	    break;
	  case NOVELL_TYPE20:
	    nv_write(idb->novell_type20, "%s", csb->nv_command);
	    break;
	  case NOVELL_GNS_REPLY_DISABLE:
	    nv_write(!idb->nov_respond_to_gns, "%s", csb->nv_command);
	    break;
	  case NOVELL_RIP_MULTIPLIER:
	    nv_write(idb->nov_rip_multiplier != NOVELL_RIP_INVALIDTIME,
		     "%s %u", csb->nv_command, idb->nov_rip_multiplier);
	    break;
	  case NOVELL_SAP_MULTIPLIER:
	    nv_write(idb->nov_sap_multiplier != NOVELL_RIP_INVALIDTIME,
		     "%s %u", csb->nv_command, idb->nov_sap_multiplier);
	    break;
	  case NOVELL_RIP_MAX_PACKETSIZE:
	    nv_write(idb->nov_rip_maxpacketsize != NOVELL_RIP_MAXSIZE,
		     "%s %u", csb->nv_command, idb->nov_rip_maxpacketsize);
	    break;
	  case NOVELL_SAP_MAX_PACKETSIZE:
	    nv_write(idb->nov_sap_maxpacketsize != NOVELL_SAP_MAXSIZE,
		     "%s %u", csb->nv_command, idb->nov_sap_maxpacketsize);
	    break;
	  case NOVELL_INT_GNS_RESPONSE_DELAY:
	    nv_write(idb->nov_gns_delay != novell_gns_response_delay,
		     "%s %u", csb->nv_command, idb->nov_gns_delay);
	    break;
	  case NOVELL_EVENIZE:
	    /*
	     * Output explicit declaration iff non-default:
	     */
	    if ((hwidb->status & (IDB_ETHER|IDB_LEX)) || lec_enabled(idb))
		nv_write(!idb->novell_evenize, "no %s", csb->nv_command);
	    else if (hwidb->status & (IDB_TR | IDB_FDDI | IDB_SERIAL))
		nv_write(idb->novell_evenize, "%s", csb->nv_command);
	    break;

	  case NOVELL_WATCHDOGSPOOF:
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb == idb) {
		    nv_write((nidb->novell_spoof_flag & NOVELL_W_SPOOF),"%s",
			     csb->nv_command);
		}
	    }
	    break;

	  case NOVELL_LINKDELAY:
	    FOR_ALL_NIDBS(nidb) {
	      if (nidb->idb == idb) {
		nv_write(nidb->novell_link_delay != NOVELL_LINK_DELAY_DEFLT,
			 "%s %u", csb->nv_command,
			 nidb->novell_link_delay);
	      }
	    }
            break;
	  case NOVELL_THROUGHPUT:
	    FOR_ALL_NIDBS(nidb) {
	      if (nidb->idb == idb) {
		nv_write(nidb->novell_link_throughput != NOVELL_THROUGHPUT_DEFLT,
			 "%s %u", csb->nv_command,
			 nidb->novell_link_throughput);
	      }
	    }
	    break;

	  case NOVELL_SPXSPOOF:
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb == idb) {
		    nv_write((nidb->novell_spoof_flag & NOVELL_S_SPOOF),"%s",
			     csb->nv_command);
		}
	    }
	    break;

	  case NOVELL_SPXIDLETIME:
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb == idb) {
		    nv_write(nidb->novell_spxidletime != NOVELL_SPX_IDLE_DEFLT,
			     "%s %u", csb->nv_command,
			     nidb->novell_spxidletime);
		}
	    }
            break;

	  case NOVELL_TRIG_RIP_DELAY:
	    FOR_ALL_NIDBS(nidb) {
	      if (nidb->idb == idb && (!IPX_NIDB_IS_SECONDARY(nidb)))
		nv_write(nidb->trig_rip_delay != novell_def_trig_rip_dly,
			 "%s %u", csb->nv_command,
			 nidb->trig_rip_delay);
	    }
	    break;

	  case NOVELL_TRIG_SAP_DELAY:
	    FOR_ALL_NIDBS(nidb) {
	      if (nidb->idb == idb && (!IPX_NIDB_IS_SECONDARY(nidb)))
		nv_write(nidb->trig_sap_delay != novell_def_trig_sap_dly,
			 "%s %u", csb->nv_command,
			 nidb->trig_sap_delay);
	    }
	    break;

	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    /*
     * Note that implicit_enctype will be valid at this point
     * if a good "novell encapsulation" command has been issued.
     */

    switch (csb->which) {
      case NOVELL_PPPCLIENT:
	target_idb = GETOBJ(idb,1);
	novell_pppclient_configure(target_idb, idb, csb->sense);
	break;
      case NOVELL_ACCESS:
	/*
	 * The FS cache must be invalidated when the accesslist changes
	 * since destination net and destination host are not checked by
	 * the FS version of the filter. Note that an access-group cannot
	 * be defined while autonomous switching is enabled.
	 */
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				   NOV_NETMASK_ALL);
	if (!csb->sense) {
	    update_novell_accesslist(idb, 0);
	    idb->novell_accesslist_def = 0;
	} else {
	    if (hwidb->novell_routecache & NOVELL_RCACHE_CBUS) {
		printf("\n%%IPX autonomous switching must be disabled "
		       "on %s", hwidb->hw_namestring);
		return;
	    }
	    update_novell_accesslist(idb, GETOBJ(int,1));
	    idb->novell_accesslist_def = idb->novell_accesslist;
	}
	break;

      case NOVELL_NETWORK:
	novell_network_ifcommand(TRUE, csb->sense, idb, GETOBJ(int, 1),
				 GETOBJ(int, 2), GETOBJ(int, 3), 
				 csb->resolvemethod);
	break;
	
      case NOVELL_ADVERTISE_DEFAULT_ROUTE_ONLY:
	novell_advertise_default_route_only_ifcommand(TRUE, csb->sense, idb,
						      GETOBJ(int, 1));
	break;

      case NOVELL_DOWN:
	novell_down_ifcommand(TRUE, csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_RIP_MULTIPLIER:
	novell_rip_multiplier_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_SAP_MULTIPLIER:
	novell_sap_multiplier_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_RIP_MAX_PACKETSIZE:
	novell_rip_max_packetsize_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_SAP_MAX_PACKETSIZE:
	novell_sap_max_packetsize_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_ROUTEC:
	novell_routecache_ifcommand(csb, idb, GETOBJ(int, 1));
	break;

      case NOVELL_DELAY:
	novell_delay_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_ENCTYPE:
	novell_enctype_ifcommand(csb->sense, idb, GETOBJ(int, 1), 
				 csb->resolvemethod);
	break;

      case NOVELL_SAPINT:
	novell_sapint_ifcommand(TRUE, csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_HLPADDR:
	novell_hlpaddr_ifcommand(csb->sense, idb, GETOBJ(paddr, 1));
	break;

      case NOVELL_HLPLIST:
	if (!csb->sense) {
	    update_novell_helperlist(idb, 0);
	} else {
	    update_novell_helperlist(idb, GETOBJ(int,1));
	}
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0, 
				   NOV_NETMASK_ALL);
	break;
	
      case NOVELL_INACCESS:
	if (!csb->sense) {
	    update_novell_inaccesslist(idb, 0);
	} else {
	    if (rcache_cbus_sse_on_any_hwidb()) {
		printf("\n%%IPX autonomous/SSE switching must be disabled "
		       "on all interfaces");
		return;
	    }
	    update_novell_inaccesslist(idb, GETOBJ(int,1));
	}
	novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0, 
				   NOV_NETMASK_ALL);
	break;
	
      case NOVELL_IN_FILTER:
	if (!csb->sense) {
	    idb->nov_in_filterlist = 0;
	} else {
	    idb->nov_in_filterlist = GETOBJ(int,1);
	}
	break;

      case NOVELL_NSQ_OUTFILTER:
	if (!csb->sense) {
	    idb->nov_gns_outfilterlist = 0;
	} else {
	    idb->nov_gns_outfilterlist = GETOBJ(int,1);
	}
	break;
	
      case NOVELL_OUT_FILTER:
	if (!csb->sense) {
	    idb->nov_out_filterlist = 0;
	} else {
	    idb->nov_out_filterlist = GETOBJ(int,1);
	}
	break;

      case NOVELL_ROUTER_FILTER:
	if (!csb->sense) {
	    idb->nov_router_filterlist = 0;
	} else {
	    idb->nov_router_filterlist = GETOBJ(int,1);
	}
	break;

      case NOVELL_SAP_INFILTER:
	if (!csb->sense) {
	    if (idb->nov_sap_infilterlist != 0) {
		idb->nov_sap_infilterlist = 0;
		reg_invoke_novigrp_sap_access_list_work(0, idb);
	    }
	} else {
	    if (idb->nov_sap_infilterlist != GETOBJ(int,1)) {
		idb->nov_sap_infilterlist = GETOBJ(int,1);
		reg_invoke_novigrp_sap_access_list_work(idb->nov_sap_infilterlist, idb);
	    }
	}
	break;

      case NOVELL_SAP_OUTFILTER:
	if (!csb->sense) {
	    if (idb->nov_sap_outfilterlist != 0) {
		idb->nov_sap_outfilterlist = 0;
		reg_invoke_novigrp_sap_access_list_work(0, idb);
	    }
	} else {
	    if (idb->nov_sap_outfilterlist != GETOBJ(int,1)) {
		idb->nov_sap_outfilterlist = GETOBJ(int,1);
		reg_invoke_novigrp_sap_access_list_work(idb->nov_sap_outfilterlist, idb);
	    }
	}
	break;
	
      case NOVELL_SAP_ROUTERFILTER:
	if (!csb->sense) {
	    if (idb->nov_sap_routerfilterlist != 0) {
		idb->nov_sap_routerfilterlist = 0;
		reg_invoke_novigrp_sap_access_list_work(0, idb);
	    }
	} else {
	    if (idb->nov_sap_routerfilterlist != GETOBJ(int,1)) {
		idb->nov_sap_routerfilterlist = GETOBJ(int,1);
		reg_invoke_novigrp_sap_access_list_work(idb->nov_sap_routerfilterlist,
					     idb);
	    }
	}
	break;

      case NOVELL_UPDATETIME:
	novell_updatetime_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;


      case NOVELL_SAP_AFTER_RIP:
	nidb = IPX_PRIMARY_NIDB(idb);
	if (nidb != NULL) {
	    nidb->novell_sap_after_rip = csb->sense;
	    if (nidb->novell_sap_after_rip) {
		novell_disable_full_sap_updates(nidb);
	    } else {
		novell_enable_full_sap_updates(nidb);
	    }
	}
	break;

      case NOVELL_GNS_REPLY_DISABLE:
	novell_gns_reply_disable_ifcommand(csb->sense, idb);
	break;

      case NOVELL_TYPE20:
	idb->novell_type20 = csb->sense;
	break;

      case NOVELL_EVENIZE:
	if (csb->set_to_default)
	    idb->novell_evenize = (idb->hwptr->status & (IDB_ETHER|IDB_LEX))  ||
		lec_enabled(idb);
	else
	    idb->novell_evenize = csb->sense;
	break;

      case NOVELL_WATCHDOGSPOOF:
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb) {
		if (!csb->sense) {
		    nidb->novell_spoof_flag &= ~NOVELL_W_SPOOF;
		} else {
		    if (hwidb->novell_routecache != 0) {
			printf("\n%%IPX route-cache must be disabled first on %s",
			       hwidb->hw_namestring);
			return;
		    } else if (!(hwidb->status & IDB_SERIAL) && 
			       !(hwidb->type == IDBTYPE_LB) &&
			       !(hwidb->status & IDB_DIALER) && !is_p2p(idb)) {
			printf("\n%%IPX watchdog-spoof: %s not Serial, Dialer, or Point to Point interface",
			       hwidb->hw_namestring);
			return;
		    }
		    nidb->novell_spoof_flag |= NOVELL_W_SPOOF;
		}
	    }
	}
	break;

      case NOVELL_SAP_DELAY:
	novell_sap_delay_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_RIP_DELAY:
	novell_rip_delay_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_INT_GNS_RESPONSE_DELAY:
	novell_gns_response_delay_ifcommand(csb->sense, idb, GETOBJ(int, 1));
	break;

      case NOVELL_LINKDELAY:
	FOR_ALL_NIDBS(nidb) {
	  if (nidb->idb == idb) {
	    novell_link_delay_ifcommand(csb->sense, nidb, GETOBJ(int,1));
	  }
	}
	break;

      case NOVELL_THROUGHPUT:
	FOR_ALL_NIDBS(nidb) {
	  if (nidb->idb == idb) {
	    novell_throughput_ifcommand(csb->sense, nidb, GETOBJ(int,1));
	  }
	}
	break;

      case NOVELL_SPXSPOOF:
	if (!novell_running || novell_shutting_down)
	  return;
	FOR_ALL_NIDBS(nidb) {
	    if (nidb->idb == idb) {
		if (!csb->sense) {
		    nidb->novell_spoof_flag &= ~NOVELL_S_SPOOF;
		    idb->hwptr->novell_spxspoof = FALSE;
		    /* Due to needing to know at FS level if we are spoofing we
		     * now check if any other idb's associated with this hwidb
		     * still have spoofing enabled. If so hwptr->novell_spxspoof
		     * still needs to be set to TRUE
		     */
		    found = FALSE;
		    FOR_ALL_NIDBS(nnidb) {
		      if ((nnidb->novell_spoof_flag & NOVELL_S_SPOOF))
			found = TRUE;
		      if ((nnidb->idb->hwptr == idb->hwptr) &&
			  ((nnidb->novell_spoof_flag & NOVELL_S_SPOOF))) {
			idb->hwptr->novell_spxspoof = TRUE;
		      }
		    }
		    if (!found) novell_clear_spx_spoof();
		} else {
		    if (hwidb->novell_routecache != 0) {
			printf("\n%%IPX route-cache must be disabled first on %s",
			       hwidb->hw_namestring);
			return;
		    } else if (!(hwidb->status & IDB_SERIAL) && 
			       !(hwidb->type == IDBTYPE_LB) &&
			       !(hwidb->status & IDB_DIALER) && !is_p2p(idb)) {
			printf("\n%%IPX spx-spoof: %s not Serial, Dialer, or Point to Point interface",
			       hwidb->hw_namestring);
			return;
		    }
		    nidb->novell_spoof_flag |= NOVELL_S_SPOOF;
		    idb->hwptr->novell_spxspoof = csb->sense;
		}
	    }
	}
	break;

      case NOVELL_SPXIDLETIME:
	FOR_ALL_NIDBS(nidb) {
	  if (nidb->idb == idb) {
	    novell_spx_idle_time_ifcommand(csb->sense, nidb, GETOBJ(int,1));
	  }
	}
	break;

      case NOVELL_TRIG_RIP_DELAY:
	FOR_ALL_NIDBS(nidb) {
	  if (nidb->idb == idb)
	    if (!csb->sense) {
	      nidb->trig_rip_delay = novell_def_output_rip_dly;
	    } else {
	      nidb->trig_rip_delay = GETOBJ(int,1);
	    }
	}
	break;

      case NOVELL_TRIG_SAP_DELAY:
	FOR_ALL_NIDBS(nidb) {
	  if (nidb->idb == idb)
	    if (!csb->sense) {
	      nidb->trig_sap_delay = novell_def_output_sap_dly;
	    } else {
	      nidb->trig_sap_delay = GETOBJ(int,1);
	    }
	}
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * clear_novell_command
 *
 * Process the various 'clear novell' commands.
 */
void clear_novell_command (parseinfo *csb)
{
    nidbtype *nidb;
    int cnet, netmask;
    novpdbtype *pdb, *dist;

    if (!novell_running || novell_shutting_down) {
	printf(nov_notenabled);
	return;
    }

    switch (GETOBJ(int,10)) {
      case CLEAR_NOVELL_CACHE:
	novcache_increment_version(NULL, NOVCACHE_CLEAR, 0, NOV_NETMASK_ALL);
	nov_fscache_flush();
	break;
      case CLEAR_NOVELL_ROUTE:
	if (GETOBJ(int,2)) {
	    novrip_purgetable();
	    novrip_delete_all_backup_data(0, 0);
	    /* send RIP requests to repopulate routing table */
	    FOR_ALL_NIDBS(nidb) {
		if (interface_up(nidb->idb) && NOVELL_NIDB_ISUP(nidb)) {
		    novell_send_gen_rip_query(nidb);
		}
	    }
	    /*
	     * send SAP queries after RIP queries to give RIP table time
	     *  to be built. Can't have SAP entry without RIP entry
	     */
	    FOR_ALL_NIDBS(nidb) {
		if (interface_up(nidb->idb) && NOVELL_NIDB_ISUP(nidb)) {
		    novell_send_gen_sap_query(nidb, NOVELL_EVERYONE);
		}
	    }
            for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb= pdb->next) {
                if (pdb->proctype == NET_IGRP)
		    reg_invoke_novigrp_reload_proto_table(pdb);
            }
	    /* Redist everything back into everything else... */
	    for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb= pdb->next) {
		for (dist = (novpdbtype *)novpdbQ.qhead; dist;
		     dist= dist->next) {
		    if (dist == pdb)
			continue;
		    if (pdb->distflag & dist->mask) {
			if (pdb->redistribute)
			    (*(pdb->redistribute))(pdb, dist, TRUE);
		    }
		}
	    }
		
	} else {
	    cnet = GETOBJ(int,1);
	    netmask = GETOBJ(int,3);
	    if (netmask == 0)
		netmask = NOV_NETMASK_ALL;
	    if ((cnet & netmask) != cnet) {
		printf("\n%% IPX network %x is inconsistent with mask %x",
		       cnet, netmask);
		return;
	    }
	    novcache_increment_version(NULL, NOVCACHE_NULL_REASON, 0,
				       NOV_NETMASK_ALL);
	    novrip_del_rt(cnet, netmask, IPX_ROUTE_DELETE_REMOVE_DYNAMIC);
	}
	for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb= pdb->next) {
	    if ((pdb->proctype == NET_NLSP) && pdb->osi_pdb) {
		/*
		 * Schedule SPF if NLSP is running:
		 */
		(void) reg_invoke_ipx_nlsp_ipx_sched_spf(pdb->osi_pdb);
	    }
	}
	break;
      case CLEAR_NOVELL_SPX_SPOOF:
	/* Remove data from SPX spoof table */
	novell_clear_spx_spoof();
	break;
    default:
	bad_parser_subcommand(csb, GETOBJ(int,10));
	break;
    }
}

/*
 * novell_enqueue
 * Pass Novell packet to Novell handler for forwarding, etc.
 *
 * Note: Don't get rid of the use of the 'ints' for the queue sizes in this
 * routine!
 */
void novell_enqueue (paktype *pak)
{
    ipxhdrtype *ipx;
    novell_sappacket *sapp = (novell_sappacket *)(XNSDATASTART(pak));
    int novQlen, novbgQlen, novell_ripQlen, novell_gnsQlen;
    
    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (systeminit_flag && novell_running && !novell_shutting_down) {
      /*
       * If a queue maximum is defined, check the current queue sizes...
       */
      if (novell_sapqueuemax  && (GETSHORT(&ipx->dsock) == NOVELL_SOCK_SAP)) {
	if ((GETSHORT(&sapp->response_type) == NOVELL_SAP_GENQUERY) ||
	    (GETSHORT(&sapp->response_type) == NOVELL_SAP_NEARQUERY) ||
	    (GETSHORT(&sapp->response_type) == NOVELL_SAP_NEARNAMEQUERY) ||
	    (GETSHORT(&sapp->response_type) == NOVELL_SAP_GENNAMEQUERY)) {
	  novQlen = process_queue_size(novQ);
	  novbgQlen = (int) QUEUESIZE(&novbgQ);
	  novell_ripQlen = (int) QUEUESIZE(&novrip_pdb.inputq);
	  novell_gnsQlen = (int) QUEUESIZE(&novell_gnsQ);
	  if ((novQlen + novbgQlen + novell_ripQlen + novell_gnsQlen) >
	      novell_sapqueuemax) {
	    /*
	     * Okay... the processing queues are full. If this is a
	     * packet this router must process, (i.e., return is NOT
	     * 'routeit'
	     */
	    if (!nov_set_nidb_idb(pak))
	      return;
	    if (nov_forus(pak) == NOVELL_CONSUMEIT) {
	      if (systeminit_flag && novell_running)
		pak->flags |= PAK_ABORT;
	      novell_stat[NOVELL_THROTTLED]++;
	      datagram_done(pak);
	      return;
	    }
	  }
	}
      }
      GET_TIMESTAMP(pak->inputtime);
      process_enqueue_pak(novQ, pak);
    } else {
      protocol_discard(pak, systeminit_flag && novell_running);
    }
}  

/*
 * novell_start
 * Start Novell processing
 */
void novell_start (void)
{
    int i;
    
    if (novell_started)
       return;

    memset(&novconnected_pdb, 0, sizeof(novpdbtype));
    memset(&novstatic_pdb,    0, sizeof(novpdbtype));
    memset(&novfstatic_pdb,   0, sizeof(novpdbtype));
    memset(&novrip_pdb,       0, sizeof(novpdbtype));

    novmisc_pdbinit();
    novell_table = malloc_named(sizeof(nov_routetable), "IPX Route Table");
    if (!novell_table) {
	return;
    }
    static_table = malloc_named(sizeof(nov_statictable), "IPX Static Tbl");
    if (!static_table) {
        free(novell_table);
        novell_table = NULL;
        return;
    }
    /*
     * %%% NEED TO SQUARE THIS WITH CALL FROM ISIS_IPX.C %%%
     */
    if (!novrip_init_backup_tables())
	return;

    novell_sap_route_hashtable = malloc_named(sizeof(nov_saproutetable),
					      "IPX SAP Table");
    if (!novell_sap_route_hashtable) {
	free(novell_table);
	novell_table = NULL;
	free(static_table);
	static_table = NULL;
	return;
    }
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	(*novell_sap_route_hashtable)[i] = NULL;
        (*novell_table)[i] = NULL;
	(*static_table)[i] = NULL;
    }

    novell_started = TRUE;
    novell_inp_pid = 0;
    novell_sap_pid = 0;
    novell_spv_pid = 0;
    novell_rup_pid = 0;
    novell_sup_pid = 0;
    novell_net_pid = 0;
    novell_gns_pid = 0;
    novell_fwd_pid = 0;
    novell_ipxwan_pid = 0;
    novell_ipxwanbg_pid = 0;

    /* novrip_pdbinit MUST be executed before nov_input is started! */
    novrip_pdbinit();
    (*novrip_pdb.restart)(&novrip_pdb);

    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(novrdbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(novndbtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(backuptype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(novell_sapentrytype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(novcachetype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(novpathtype));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(nov_svc_type));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, sizeof(novell_sapentry_rdb));
    mempool_add_free_list(MEMPOOL_CLASS_LOCAL, 
			  sizeof(novell_server_per_source_list));
}

/*
 * novell_sap_poison
 *
 * Poison the current SAP entry, delete it and advertise it as poisoned
 * as well (no holddown period for Novell certification).
 */
void novell_sap_poison (novell_sapentrytype *entry)
{
    novell_sapentry_rdb *srdb;

    /* Don't do anything if the entry is already poisoned! */
    if (!entry->poisoned) {
	GET_TIMESTAMP(entry->sap_timer);
	entry->poisoned = TRUE;
	/*
	 * Router certification requires an immediate poisoning of the
	 * server, so we can accept new information from elsewhere
	 * immediately.
	 */
	if (!(entry->flags & NET_STATIC))
	    entry->server.nethops = NOVELL_MAX_HOPS_ALLOWED;
	novell_sap_check_backup_queue(entry);
	novell_mark_sap_changed(entry);
	for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) {
	    novell_mark_srdb_changed(entry, srdb, entry->flags,
				     IPX_ROUTE_EVENT_CHANGE);
	}
    }
}

/*
 * novell_sap_advertised_everywhere
 *
 * Determine if the sap has been successfully advertised to every interface.
 *
 */
static boolean novell_sap_advertised_everywhere (novell_sapentrytype *entry)
{
    nidbtype *nidb;

    FOR_ALL_NIDBS(nidb) {
	/*
	 * Skip networks running NLSP with SAP disabled (since
	 * there is no way they can be advertised via SAP).
	 */
	if (nidb->nov_lsp_enabled && !nidb->lsp_sap_on) {
	    continue;
	}
	if (NOVELL_NIDB_IDB_UP(nidb)) {

	    if (entry->reverse_poisoned) {
		if (!TIMER_EARLIER(nidb->sap_revp_lastupdate,
				   entry->sap_changed)) {
		    continue;
		}
	    } else {
		if (!TIMER_EARLIER(nidb->sap_lastupdate,
				   entry->sap_changed)) {
		    continue;
		}
	    }	    
	    /*
	     * Not advertised everywhere yet. 
	     */
	    return FALSE;
	}
    }
    return TRUE;
}

/*
 * novell_sap_delete_if_advertised
 *
 * Delete the server if it has been successfully advertised out all
 * interfaces.
 */
static void novell_sap_delete_if_advertised (novell_sapqueuetype *sapq,
				      novell_sapentrytype *entry)
{
    novell_sapqueuetype *prevq, *currentq;
    queuetype *sapque;
    int count;

    /* See if it should be deleted */
    if (!novell_sap_advertised_everywhere(entry))
	return;

    if (novellsap_debug || novellsap_event_debug)
        buginf("\nIPXSAP: server type %x named %s metric %d being deleted",
	       entry->server.server_type, entry->server.server_name,
	       entry->server.nethops);
    
    lw_remove(&entry->saps);
    if (entry->flags & NET_STATIC)
        nov_static_server_count--;
    nov_sap_free(sapq, entry, IPX_SAP_FREE_LOOKUP_BACKUPS);
    /* Is this the last entry in the hash, if so delete the queue */
    if (sapq->sap_count == 0) {
      /* Count the saps in the sapq one more time for sure */
      count = 0;
      FOR_ALL_SAPS_OF_TYPE(entry, sapq) {
	  count++;
      }
      sapq->sap_count = count;
    }
    if (sapq->sap_count == 0) {
      sapque = &(novell_sapQ);
      if (novellsap_debug || novellsap_event_debug)
	buginf("\nIPX: SAP queue-hash deleted for type %x, count %d",
	       sapq->server_type, sapque->count);
      /* sapq entry MUST be in the queue, get previous entry pointer */
      for (prevq = NULL, 
	   currentq = (novell_sapqueuetype *) novell_sapQ.qhead;
	   currentq;
	   prevq = currentq, currentq = currentq->next) {
	/* Found? */
	if (currentq->server_type == sapq->server_type) {
	  break;
	}
      }
      free(remqueue(&(novell_sapQ), sapq, prevq));
    }
}

/*
 * novell_sapentry_poisoned
 *
 * Perform common poison checks for general/nearest service queries
 * Do the poison if appropriate.
 */
static boolean novell_sapentry_poisoned (
     novell_sapqueuetype *sapq,
     novell_sapentrytype *entry, nidbtype *poison_nidb)
{
    novell_sapentry_rdb *srdb, *srdb_prev, *srdb_next;
    
    if ((!entry->poisoned)  &&
	!(entry->flags & (NET_STATIC | NET_IGRP | NET_NLSP))) {
	srdb_prev = NULL;
	for (srdb = entry->paths; srdb != NULL; srdb = srdb_next) {
	    srdb_next = srdb->next;
	    if ((srdb->nidb->idb->novell_interval &&
		!srdb->snapshot_no_age) || (srdb->nidb == poison_nidb)) {
		if ((srdb->nidb == poison_nidb) ||
		    CLOCK_OUTSIDE_INTERVAL(srdb->timer,
			      srdb->nidb->idb->nov_sap_multiplier *
			      srdb->nidb->idb->novell_interval*ONEMIN)) {
		    if (srdb_prev == NULL) {
			entry->paths = srdb_next;
		    } else {
			srdb_prev->next = srdb_next;
		    }
		    entry->pathcount--;
		    novell_mark_srdb_changed(entry, srdb, entry->flags,
					     IPX_ROUTE_EVENT_DELETED);
		    novsrdb_free(srdb);
		    continue;
		}
	    }
	    srdb_prev = srdb;
	}
    }
    if (entry->paths == NULL) {
	novell_sap_poison(entry);
    }
    return(entry->poisoned);
}

/*
 * novell_sap_adjust
 *
 * Run through the sap queue and fixup interface status of static saps.
 *
 */
static void
novell_sap_adjust (novell_sapentrytype *entry)

{
    novndbtype *rip_entry;
    uchar *first_hop;
    boolean changed;
    nidbtype *nidb_hold;
    ulong route_flags;

    /*
     * Only do this for static servers...
     */
    if (!(entry->flags & NET_STATIC)) {
	return;
    }
    rip_entry = novrip_find_rt(entry->server.dnet, NOV_NETMASK_ALL,
			       IPX_ROUTE_MATCH_FUZZ);
    /* Only care about it if the route is not poisoned! */
    if (rip_entry == NULL) {
	first_hop = NULL;
    } else {
	first_hop = novrip_first_hop(rip_entry->dstnet,
				     (uchar *) "Foo",
				     &nidb_hold, &route_flags,
				     rip_entry,
				     IPX_ROUND_ROBIN_NO,
				     IPX_DEFAULT_ROUTE_NO);
    }
    if ((rip_entry != NULL) && (first_hop != NULL)) {
 	changed = ((entry->paths->nidb != rip_entry->path_list->nidb) ||
 		   (entry->poisoned));
 	if (!changed) {
 	    if (novell_sap_uses_routing) {
 		if (entry->route_flags & NET_IGRP) {
 		    changed = (rip_entry->path_metric != entry->route_delay) ||
 			(rip_entry->ext_hop != entry->route_hops);
 		} else {
 		    changed = (rip_entry->delay != entry->route_delay) ||
 			(rip_entry->hopcount != entry->route_hops);
 		}
 	    }
 	}
	if (!changed)
	    return;
	if (entry->paths->nidb != rip_entry->path_list->nidb) {
	    entry->paths->nidb = rip_entry->path_list->nidb;
	    entry->nidb_changed = TRUE;
	}
	entry->poisoned = entry->deleted_static;
	if (!entry->poisoned) {
	    entry->reverse_poisoned = FALSE;
	}
	entry->route_delay = rip_entry->delay;
	entry->route_hops = rip_entry->hopcount;
	entry->route_flags = rip_entry->flags;
	if (entry->route_flags & NET_STATIC) {
	    entry->route_flags = NET_RIP;
	} else if (entry->route_flags & NET_IGRP) {
	    entry->route_delay = rip_entry->path_metric;
	    entry->route_hops = rip_entry->ext_hop;
	} else if (entry->route_flags & NET_RIP) {
	    if ((entry->route_hops >= novell_infinity) ||
		TIMER_RUNNING(rip_entry->holddownuntil) ||
		(entry->route_delay == NOVELL_INFDELAY)) {
		entry->route_delay = METRIC_INACCESSIBLE;
		entry->route_hops = NOVELL_MAX_HOPS_ALLOWED;
	    }
	}
	novell_mark_srdb_changed(entry, entry->paths,
				 entry->flags,
				 IPX_ROUTE_EVENT_CHANGE);
	novell_mark_sap_changed(entry);
    } else {
	if (entry->paths->nidb) {
	    /* route may have gone away */
	    entry->paths->nidb = NULL;
	    novell_mark_srdb_changed(entry, entry->paths,
				     entry->flags,
				     IPX_ROUTE_EVENT_DELETED);
	    novell_sap_poison(entry);
	}
    }
}

/*
 * novspx_ager
 * Age out Novell spx spoof table entries with no activity in one hour.
 *
 */

static void novspx_ager (void)
{
    uchar hashindex;
    nov_spx_spoof_record *spoofrecordp, *spoofrecordn, *spoofrecordprev;

    if (!nov_spx_idle_time && !nov_spx_clear_time) /* anything to do? */
	return;
    for (hashindex = 0; hashindex < SPOOF_HASH_TABLE_ENTRIES; hashindex++) {
	spoofrecordp = spoofhashtable[hashindex];
	spoofrecordprev = NULL;
	while (spoofrecordp != NULL) {
	    spoofrecordn = spoofrecordp->chain;
	    if ((nov_spx_idle_time &&
		 CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timelastchanged, 
					nov_spx_idle_time) &&
		 CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timelastspoofed,
					nov_spx_idle_time) &&
		 CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timeadded,
					nov_spx_idle_time)) ||
		(nov_spx_clear_time &&
		 CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timeadded,

					nov_spx_clear_time))) {
	      if (spoofrecordprev == NULL) {
		spoofhashtable[hashindex] = spoofhashtable[hashindex]->chain;
	      } else {
		spoofrecordprev->chain = spoofrecordp->chain;
	      }
	      novell_returnspoofrecord(spoofrecordp);
	    } else {
	      spoofrecordprev = spoofrecordp;
	    }
	    spoofrecordp = spoofrecordn;
	}
	process_may_suspend();
    } 
}

/*
 * novell_sap_ager
 *
 * Age SAP entries; about once a minute this is done. This better NEVER
 * do a suspend!
 */
static void novell_sap_ager (void)
{
    novell_sapentrytype *entry, *nextentry;
    novell_sapqueuetype *sapq;
    novell_sapentry_rdb *srdb, *last_srdb;
    nidbtype *nidb;
    boolean was_on;
    boolean sap_cmode_debug;
    int table_index;

    /*
     * Screen all NLSP auto compatibility mode circuits for activity.
     * Deactivate if stale.
     */
    sap_cmode_debug = ((novellisis_debug || novellnlsp_debug) &&
		       (novellsap_debug || novellsap_event_debug));

    for (nidb = (nidbtype *) novidbQ.qhead; nidb; nidb = nidb->next) {
	if (nidb->lsp_sap_cfg == IPX_RIPSAP_CONFIG_AUTO) {
	    if (CLOCK_OUTSIDE_INTERVAL(nidb->lsp_sap_timestamp,
				       (nidb->idb->nov_sap_multiplier *
				       nidb->idb->novell_interval * ONEMIN))) {
		was_on = nidb->lsp_sap_on;
		novell_stop_nidb_sap_auto(nidb);
		if (sap_cmode_debug && was_on)
		    buginf("\nIPX-NLSP: deactivate SAP on net %x",
			   nidb->novellnet);
	    }
	}
    }
    for (table_index = 0; table_index < NOV_NETHASHLEN; table_index++) {
	process_may_suspend();
	for (entry = (*novell_sap_route_hashtable)[table_index];
	     entry; entry = nextentry) {
	    nextentry = entry->net_hash_next;
	    sapq = entry->sapq;
	    /*
	     * Clear the nidb_changed field if the server has been properly
	     * advertised.
	     */
	    if (entry->nidb_changed) {
		if (novell_sap_advertised_everywhere(entry))
		    entry->nidb_changed = FALSE;
	    }
	    /*
	     * Adjust static entries which have exceeded the holddown
	     * period. These will continue to show as in holddown but
	     * will move to the end of the appropriate server list
	     * when displayed.
	     */
	    if (entry->flags & NET_STATIC) {
		if (entry->poisoned &&
		    (entry->route_delay != METRIC_INACCESSIBLE) &&
		    CLOCK_OUTSIDE_INTERVAL(entry->sap_timer, ONEMIN)) {
		    entry->route_delay = METRIC_INACCESSIBLE;
		    entry->route_hops = NOVELL_MAX_HOPS_ALLOWED;
		    entry->route_flags = NET_RIP;
		    entry->sapq->sorted = FALSE;
		}
		/*
		 * If a static marked as to be deleted, delete it if it has
		 * been advertised everywhere.
		 */
		if (entry->deleted_static) {
		    novell_sap_delete_if_advertised(sapq, entry);
		    continue;
		}
		/*
		 * Perform any 'moves' of the route from interface to
		 * interface, or from poisoned to not poisoned, etc...
		 */
		novell_sap_adjust(entry);
	    } else {
		/* prune the SAP entries with more than novell_maxpaths
   		 * paths. Note that we trim slower paths from end of list.
		 */
	    	if (entry->flags & NET_RIP) {
		    if (entry->pathcount > novell_maxpaths) {
		        while (entry->pathcount > novell_maxpaths) {
		            entry->pathcount--;
		            srdb = entry->paths;
   		            last_srdb = (novell_sapentry_rdb *) (&entry->paths);
		            while (srdb->next) {
   		                last_srdb = srdb;
		                srdb = srdb->next;
		            }
		            last_srdb->next = NULL;
		            novell_mark_srdb_changed(entry, srdb, entry->flags,
						     IPX_ROUTE_EVENT_DELETED);
		            novsrdb_free(srdb);
		        }
		    }
		}
		/* novell_sapentry_poisoned will age to poison, if need be */
		if (novell_sapentry_poisoned(sapq, entry, NULL) &&
		    CLOCK_OUTSIDE_INTERVAL(entry->sap_timer, ONEMIN)) {
		    novell_sap_delete_if_advertised(sapq, entry);
		}
	    }
	}
    }
}

/*
 * novellBLOCK
 * Block until something in input queue or SAP timer has expired
 */
boolean novellBLOCK (queuetype *q)
{
    if (!novell_running)
	return(TRUE);			/* never if not running */
    if (q->qhead)
	return(FALSE);
    if (novell_backup_servers_needed &&
	(!SLEEPING(novell_backup_servers_timer)))
        return(FALSE);
    if (!SLEEPING(novell_sapagertimer))
        return(FALSE);
    if (!SLEEPING(novell_freed_ndb_timer))
	return(FALSE);
    if (novell_freed_ndbq_wakeup) 	/* elements on freed ndbq */
        return(FALSE);
    return(TRUE);
}

/*
 * novell_fork
 *
 * Process/routine to handle Novell SAP packets. Also does the work of the
 * final freeing of ndbs. This may seem odd, but this is a process which is
 * guaranteed to run only when the routing table isn't "being played with"
 * *and* will never be stopped from running by someone issuing a
 * command like 'no ipx router rip', which disables the RIP process. 
 */
forktype novell_fork (void)
{
    paktype *pak;
    ipxhdrtype *ipx;
    novell_sappacket *sapp;
    int count;
    boolean full;
    boolean part;
    novpdbtype *loop_pdb;
    novndbtype *ndb;
    backuptype *bi, *binext;
    nidbtype *nidb, *nidbnext;

    edisms((blockproc *)systeminitBLOCK, (ulong)&novbgQ);
    process_set_priority(PRIO_LOW);
    full = part = FALSE;

    while (TRUE) {
	edisms((blockproc *)novellBLOCK, (long)&novbgQ);
	count = 10;
	while (TRUE) {
	    if (--count == 0) {
	       break;
	    }
	    if (!(pak = dequeue(&novbgQ))) {
	        break;
	    }
	    ipx = (ipxhdrtype *)ipxheadstart(pak);

	    if ((pak->if_input == NULL) || !interface_up(pak->if_input) ||
		novell_shutting_down) {
		retbuffer(pak);
		continue;
	    }
	    sapp = (novell_sappacket *)(XNSDATASTART(pak));
 	    /*
 	     * Discard incoming SAP packets if NLSP/ISIS is running
 	     * on input interface and an adjacency exists.
 	     */
 	    if ((!pak->nif_input->nov_lsp_enabled) ||
 		(!reg_invoke_ipx_nlsp_valid_adjacency(pak, ISIS_CIRCUIT_L1))) {
 		if ((novellisis_debug || novellnlsp_debug) &&
		    (novellsap_debug || novellsap_event_debug))
 		    buginf("\nIPXSAP: Accepting SAP packet from %x.%e (%s)",
 			   GET2WD(ipx->ssnet), ipx->shost,
 			   pak->if_input->short_namestring);
		pak->if_input->nov_sap_in_packets++;
		switch (sapp->response_type) {
		  case NOVELL_SAP_GENQUERY:
		    novell_stat[NOVELL_SAPREQIN]++;
		    novell_rcv_query(pak, (novell_querytype *)sapp);
		    break;
		  case NOVELL_SAP_NEARQUERY:
		    novell_stat[NOVELL_SAPGNSIN]++;
		    enqueue(&novell_gnsQ, pak);
		    pak = NULL;
		    break;
		  case NOVELL_SAP_GENRESPONSE:
		    /*
		     * Accept SAP updates only if RIP is running:
		     */
		    if (novell_rip_running) {
			novell_stat[NOVELL_SAPRESIN]++;
			novell_rcv_response(pak,sapp);
		    }
		    break;
		  case NOVELL_SAP_NEARRESPONSE:
		    if (novellsap_debug || novellsap_event_debug) {
			novell_printpak(pak,"Nearest service");
		    }
		    break;
		  case NOVELL_SAP_GENNAMEQUERY:
		    novell_stat[NOVELL_SAPGENNAMEIN]++;
		    if (novellsap_debug || novellsap_event_debug) {
			novell_printnamepak(pak,"I SAP General Name query");
		    }
		    novell_rcv_name_query(pak, (novell_querynametype *)sapp);
		    break;
		  case NOVELL_SAP_GENNAMERESPONSE:
		    if (novellsap_debug || novellsap_event_debug) {
			novell_printnamepak(pak,"I SAP General Name service response");
		    }
		    break;
		  case NOVELL_SAP_NEARNAMEQUERY:
		    novell_stat[NOVELL_SAPNEARNAMEIN]++;
		    if (novellsap_debug || novellsap_event_debug) {
			novell_printnamepak(pak,"I SAP Nearest Name query");
		    }
                    enqueue(&novell_gnsQ, pak);
                    pak = NULL;
		    break;
		  case NOVELL_SAP_NEARNAMERESPONSE:
		    if (novellsap_debug || novellsap_event_debug) {
			novell_printnamepak(pak,"I SAP Nearest Name service response");
		    }
		    break;
		  default:
		    if (novellsap_debug || novellsap_event_debug)
			buginf("\nIPXSAP: unknown operation type %d, %x.%e",
			       sapp->response_type, GET2WD(ipx->ssnet),
			       ipx->shost);
		    novell_stat[NOVELL_SAPFMTERROR]++;
		    break;
		} /* switch on SAP */
	    } else {
 		if ((novellisis_debug || novellnlsp_debug) &&
		    (novellsap_debug || novellsap_event_debug))
 		    buginf("\nIPX-ISIS: Adjacency exists"
 			   ", discarding SAP packet"
 			   " from %x.%e (%s)",
 			   GET2WD(ipx->ssnet), ipx->shost,
 			   pak->if_input->short_namestring);
	    }
	    if (pak != NULL) {
		retbuffer(pak);
		(void) process_may_suspend();
	    }
	} /* while packets */

	process_may_suspend();

	/* Process elements in the freed NDB queue */
	if (!SLEEPING(novell_freed_ndb_timer) || novell_freed_ndbq_wakeup) {
	    /*
	     * Do redistribution callbacks, if needed. Then free the NDB.
	     * Since items on this list are FIFO, if the top entry is 'too
	     * soon' all remaining entries must also be 'too soon'
	     */
	    while ((nov_freed_ndbQ.qhead != NULL) &&
		   (!SLEEPING(((novndbtype *)
			       (nov_freed_ndbQ.qhead))->holddownuntil))) {
		ndb = dequeue(&nov_freed_ndbQ);
		for (loop_pdb = (novpdbtype *) novpdbQ.qhead; loop_pdb;
		     loop_pdb = loop_pdb->next) {
		    /* DUAL follows special rules */
		    if (loop_pdb->proctype == NET_IGRP || 
			loop_pdb->proctype ==  NET_FLOATING_STATIC ||
			(ndb->options & IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP)) {
			if ((loop_pdb->add_route) &&
			    ((ndb->options &
			      IPX_NDB_OPTION_ALWAYS_LOOKUP_BACKUP) ||
			     (loop_pdb != nov_proctable[ndb->novpdbindex]))) {
			    (*loop_pdb->add_route)(loop_pdb, ndb->dstnet,
						   ndb->rt_mask, 0L, 0L);
			}
		    }
		}
		for (bi = ndb->backup_info; (bi != NULL); bi = binext) {
		    binext = bi->next;
		    loop_pdb = NOV_INDEX2PDB(bi->pdbindex);
		    if ((loop_pdb != NULL) && (loop_pdb->add_route)) {
			    (*loop_pdb->add_route)(loop_pdb, ndb->dstnet,
						   ndb->rt_mask,
						   bi->backup_context_info,
						   bi->route_type);
		    }
		    free(bi);
		}
		ndb->backup_info = NULL;
		free(ndb);
		process_may_suspend();
	    }
	    novell_freed_ndbq_wakeup = 0;
	    if (!SLEEPING(novell_freed_ndb_timer)) {
		TIMER_START(novell_freed_ndb_timer, 5*ONESEC);
	    }
	}

	process_may_suspend();

	/*
	 * Check out servers we need backup info on
	 */
	if (novell_backup_servers_needed &&
	    !SLEEPING(novell_backup_servers_timer)) {
	    reg_invoke_nov_get_backup_servers();
	}
	/*
	 * Clean up the nidb chain now.
	 */
	FOR_ALL_NIDBS_USE_SAVED_NEXT(nidb, nidbnext) {
	    if (NOVELL_NIDB_IS_DELETED(nidb) &&
		!NOVIGRP_DOING_INCREMENTAL_SAP(nidb)) {
		if (IPX_NIDB_IS_PRIMARY(nidb)) {
		    IPX_CLEAR_PRIMARY_NIDB(nidb->idb);
		}
		DECREMENT_NOVELLNETS(nidb->idb);
		novell_nidb_free(nidb);
	    }
	}

	if (!SLEEPING(novell_sapagertimer)) {
	    TIMER_START(novell_sapagertimer, 20*ONESEC);
	    novell_sap_ager();
	}
    }
}

/*  
 * novell_sap_split_horizon  
 *  
 * Check sap entry before sending against routing table for split horizons  
 * EIGRP rule is different if advertising: Split horizon applies only to the  
 * interface on which the service is learned, not to all the interfaces on  
 * which the route to the service is learned. This is to make it possible to  
 * remember where we have advertised information before without maintaining a  
 * list of 'service RDBs' for each service. 10.3 introduces this need for  
 * other reasons, so 10.3 and later EIGRP will behave more optimally (consider
 * parallel links to a route with multipath enabled; previous to 10.3, we will
 * flood back all services with EIGRP/SAP that we learned out all interfaces  
 * except the one the service was learned on. With 10.3, we will have a full  
 * list of interfaces on which the 'best service' is learned, so will be  
 * able to apply split horizon out the full list, thereby avoiding further  
 * any needless backwash of server information.  
 *  
 * Do the above only if split horizon for EIGRP is enabled; if not, always  
 * advertise.  
 */  
boolean novell_sap_split_horizon (novell_sapentrytype *entry, nidbtype *nidb,  
                                  enum IPX_ADVERTISING advertising)  
{  
    novndbtype *route_entry;  
    novell_sapentry_rdb *srdb;
    
    if ((advertising == IPX_ADVERTISING_YES) &&  
	NOVIGRP_DOING_INCREMENTAL_SAP(nidb)) {  
	if (!ipx_eigrp_sap_split_horizon) {
	    return(FALSE); 
	}
	for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) { 
	    if (srdb->nidb == nidb) 
	      return(TRUE); 
	} 
	return(FALSE); 
    } else if (ipx_server_split_on_server_paths) { 
	for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) { 
	    if (srdb->nidb == nidb) 
	      return(TRUE); 
	} 
	return(FALSE); 
    } else {
	/*
	 * SAP uses route info. Do an explicit lookup first ... accept
	 * route even if it's no longer valid. Not to do so may cause
	 * us to send unreachable saps back out an interface where we
	 * just received poisons! NOTE: it's OK to let a service known
	 * via an explicit match age out after it's poisoned even if
	 * a fuzzy match is present. In this case, let it be relearned
	 * via the fuzzy route.
	 */
	if ((route_entry = novrip_find_rt(entry->server.dnet, NOV_NETMASK_ALL,
					  IPX_ROUTE_MATCH_EXACT)) == NULL) 
	    if ((route_entry = novrip_find_rt(entry->server.dnet,
					      NOV_NETMASK_ALL,
					      IPX_ROUTE_MATCH_FUZZ)) == NULL) {
		return(FALSE);
	    }
	return(nov_same_nidb(nidb, route_entry->path_list));
    }
}

/*
 * novell_possibly_send_snapshot_null_updates
 *
 * Send a null SAP update out this interface, but only if SNAPSHOT is enabled
 */
static void novell_possibly_send_snapshot_null_updates (nidbtype *nidb)
{
    paktype *pak;
    ipxhdrtype *ipx;
    novell_sappacket *sap;

    if (nidb->idb == NULL)
	return;
    if (!reg_invoke_snapshot_enabled(nidb->idb))
	return;
    if (reg_invoke_snapshot_update_denied(nidb->idb, FALSE))
	return;
    pak = getbuffer(MEDDATA);
    if (pak == NULL) {
	return;
    }
    ipx = (ipxhdrtype *)ipxheadstart(pak);
    pak->nif_output = nidb;
    pak->linktype = LINK_NOVELL;
    ipx->tc = 0;
    ipx->pt = NOVELL_TYPE_SAPRESP;
    ieee_copy(baddr, ipx->dhost);
    ipx->dsock = NOVELL_SOCK_SAP;
    PUT2WD(ipx->ddnet, nidb->novellnet);
    PUT2WD(ipx->ssnet, nidb->novellnet);
    ieee_copy(nidb->idb->novell_node, ipx->shost);
    ipx->ssock = NOVELL_SOCK_SAP;
    ipx->cksum = NOVELL_NO_CHECKSUM;
    sap = (novell_sappacket *) (XNSDATASTART(pak));
    sap->response_type = NOVELL_SAP_GENRESPONSE;
    ipx->len = XNSOVERHEADBYTES + 2;
    pak->flags |= PAK_PRIORITY;
    reg_invoke_snapshot_notice_update_pak(pak, nidb->idb);
    nov_send(pak);
    if (novellsap_debug) {
	buginf("\nIPXSAP: Sending null update for Snapshot out interface %s",
	       nidb->idb->namestring);
    }
}

/*
 * nov_any_idb_up
 * Return TRUE if any IDB with this entry is UP
 */
static boolean nov_any_idb_up (novell_sapentrytype *entry)
{
    novell_sapentry_rdb *srdb;

    for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) {
	if (srdb->nidb != NULL) 
	  if (NOVELL_NIDB_IDB_UP(srdb->nidb))
	    return(TRUE);
    }
    return(FALSE);
}
static boolean nov_all_idb_linkdown_poisons (novell_sapentrytype *entry)
{
    novell_sapentry_rdb *srdb;

    for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) {
	if (srdb->nidb != NULL)
	  if (srdb->nidb->doing_linkdown_poisons == FALSE)
	    return(FALSE);
    }
    return(TRUE);
}

/*
 * novell_rip_network_enable
 *
 * Go through rip pdb's network queue to see if the nidb is configured for
 * rip. Return TRUE if found, FALSE otherwise.
 */
static boolean novell_rip_network_enable (nidbtype *nidb)
{
    boolean found;
    int i;

    /* Explicit exemption for unnumbered links */
    if (NOVELL_NIDB_UNNUMBERED(nidb)) {
	return(TRUE);
    }

    for (found = FALSE, i = 0; i < novrip_pdb.netcount; i++) {
	if (novrip_pdb.networks[i] == nidb->novellnet) {
	    found = TRUE;
	    break;
	}
    }
    return (found);
}

/*
 * ipx_distribute_sap_list_check
 */
boolean ipx_distribute_sap_list_check (novell_sapentrytype *entry, 
				       novpdbtype *pdb, nidbtype *nidb)
{
    novpdbtype *src_pdb;

    if (!(entry->flags & (NET_RIP | NET_IGRP | NET_STATIC | NET_NLSP)))
	return FALSE;

    /* In filter check */
    if (!nov_sapcheck(&entry->server, entry->server.server_type,
		      pdb->sap_ifltr_all)) 
     return FALSE;

    /* Out filter check */
    src_pdb = NOV_INDEX2PDB(entry->novpdbindex);
    if (src_pdb != NULL) {
	if (!nov_sapcheck(&entry->server, entry->server.server_type, 
			  src_pdb->sap_ofltr_all ? src_pdb->sap_ofltr_all : 
			  nidb->sap_oprotofltr[pdb->index]))
	    return FALSE;
	if (!nov_sapcheck(&entry->server, entry->server.server_type,
			  src_pdb->sap_ofltr_proto[pdb->index]))
	    return FALSE;
    }
    return TRUE;
}

/*
 * nov_sap_out_filter
 * Check if the specified service is permitted. Check both
 * distribute-sap-lists and output-sap-filter lists.
 */
static boolean nov_sap_out_filter (novell_sapentrytype *entry, idbtype *idb)
{
    novpdbtype *outpdb;

    /*
     * Check the old style output-sap-filter.
     */

    if (!nov_sapcheck(&entry->server, entry->server.server_type,
		      idb->nov_sap_outfilterlist))
	return (FALSE);

    if (!NOVIGRP_DOING_INCREMENTAL_SAP(IPX_PRIMARY_NIDB(idb)))
	outpdb = &novrip_pdb;
    else {
	for (outpdb = novpdbQ.qhead; outpdb; 
	     outpdb = outpdb->next) {
	    if (outpdb->proctype != NET_IGRP)
		continue;
	    if (reg_invoke_ipx_igrp2_exists_iidb(outpdb->ddb, idb))
		break;
	}
    }

    /*
     * Don't be fooled with a null pdb!
     */
    if (!outpdb)
	return (TRUE);

    return(ipx_distribute_sap_list_check(entry, outpdb, 
					 IPX_PRIMARY_NIDB(idb)));
}

/*
 * nov_sap_name_check
 *
 * routine to check if a service table entry matches a SAP Name Query
 * 
 * Check if the specified service is wanted.
 * return TRUE if the service should be permitted.
 *
 * Allow net 0xFFFFFFFF and mask 0xFFFFFFFF to be a wildcard combo matching all
 * allow a net 0x0 and mask 0x0 to be a wildcard
 * service type of 0x0 and 0xFFFF are not permited in NEARNAMEQUERIES.
 *   and have already been filtered before this routine.
 * Name wildcard is '*'
 * only simple wildcarding is allowed
 * '*' which matches all names
 * SERVER* which matches all service which beging with "SERVER"
 * strings with embedded '?' character which match any one character
 * combinations of * and ? are not being supported. In this implementation.
 */
static boolean nov_sap_name_check (novell_sapentrytype *entry, ushort query_type,
			    ushort servertype, char *server_name,
			    ulong dnet, ulong dmask)
{
    char *wildptr = NULL;
    boolean all_servers, end_of_string, marks_matched;
    int len, wildcard, i;
    
    len = strlen(server_name);
    marks_matched = all_servers = end_of_string = FALSE;
    wildcard = len;
    
    /* If this isn't a name query return quickly */
    if ((query_type != NOVELL_SAP_NEARNAMEQUERY) &&
	(query_type != NOVELL_SAP_GENNAMEQUERY)) {
	return(FALSE);
    }
    if ((dnet == NOV_NETMASK_ALL) && (dmask == NOV_NETMASK_ALL)) {
	/* weird allowed wildcard, make it 0 and 0 which do match anything */
	dnet = dmask = 0L;
    }
    /* check if we have a special search string */
    if ((wildptr = strchr(server_name, '*')) != NULL) {
	/* string of '*' matches all Server Names */
	all_servers = (len == 1);
	if ((len - 1) == (wildcard = ((ulong)wildptr - (ulong)server_name)))
	    /* Other simple case '*' at the end of the name */
	    end_of_string = TRUE;
    }
    
    if ((wildptr = strchr(server_name, '?')) != NULL) {
	if (len == strlen(entry->server.server_name)) {
	    marks_matched = TRUE;
	    for (i = 0; i < len; i++) {
		if (server_name[i] != '?')
		    if (server_name[i] != entry->server.server_name[i])
			marks_matched = FALSE;
	    }
	}
    }
    if ((servertype == NOVELL_EVERYONE) ||
	(servertype == entry->server.server_type))
	if ((entry->server.dnet & dmask) == dnet)
	    if (all_servers || marks_matched ||
		(end_of_string &&
		 (strncmp(server_name, entry->server.server_name, wildcard) == 0)) ||
		(strcmp(server_name, entry->server.server_name) == 0)) {
		return(TRUE);
	    }
    return(FALSE);
}

/*
 * nov_advertising_this_server
 *
 * Do we want to advertise this server?
 *
 * NO:
 * - Invalid or deleted entry
 * - A path entry is present but no nidb is known (probably a static
 *   without a valid route).
 * - An otherwise valid entry is split horizoned
 *
 * YES:
 * - Unicast update
 * - Partial update and not already advertised and reachable via
 *   at least one up network or poisoned but not previously advertised
 *   by a reverse poison.
 * - Linkdown update and the server is reachable (meaning it has previously
 *   been advertised out the interface going down.
 * - Reverse update and the service is still marked as reachable but none
 *   of the networks the service was learned on are up.
 *
 * NO:
 * - An otherwise valid entry is split horizoned
 * - An otherwise valid entry is filtered
 *
 * Additional notes:
 *
 *      servertype:  select only this server type for the update. If it is
 *                   NOVELL_EVERYONE, then select all server types.
 *
 *      query_type:  the kind of query we are doing. If GENNAMEQUERY then:
 *
 *      server_name: the Server Namem (possibly wildcarded) we are searching
 *      dnet:   network number of the Server. Possibly partial works with dmask
 *      dmask:  network mask which works with dnet to search a range of nets
 */
static boolean nov_advertising_this_server (novell_sapentrytype *entry,
					    nov_usv *usv,
					    nidbtype *nidb,
					    sys_timestamp lastupdate,
					    enum IPX_FULL fullupdate,
					    enum IPX_DESTINATION destination,
					    boolean reverse,
					    boolean linkdown,
					    boolean *eigrp_must_poison)
{
    boolean split_horizon_occured;
    boolean bcast = (destination == IPX_DESTINATION_BROADCAST);
    boolean ripsap_ucast = (destination == IPX_DESTINATION_RIPSAP_UNICAST);
    boolean full = (fullupdate == IPX_FULL_NEEDED);
    boolean part = !full;
    boolean sap_reachable =
	(entry->server.nethops+1 <= NOVELL_RIP_ADV_MAX_HOPS);
    boolean poison_update = (linkdown || reverse);
    boolean any_idb_up;
    boolean reachable_via_route =
	(novell_sap_uses_routing ?
	 (entry->route_delay != METRIC_INACCESSIBLE) :
	 (entry->server.nethops < novell_infinity));
    boolean debug = (novell_sapupd_debug && novell_verbose_debug);
    
    if (debug) {
	buginf("\n ?: %-4x %12s %-8x.%e:%04x @%d (%s) (%s)",
	       entry->server.server_type,
	       entry->server.server_name,
	       entry->server.dnet,
	       entry->server.dhost,
	       entry->server.dsock,
	       entry->server.nethops,
	       (fullupdate == IPX_FULL_NEEDED) ? "FULL" : "PART",
	       (destination == IPX_DESTINATION_BROADCAST) ?
	       "BCAST" : (destination == IPX_DESTINATION_UNICAST) ?
	       "UCAST" : "RS_UCAST");
    }
    /*
     * Immediate bail if bogus, deleted, static with no route, or wrong type:
     */
    if ((entry == NULL) ||
	((entry->paths != NULL) && (entry->paths->nidb == NULL)) ||
	((usv->servertype != NOVELL_EVERYONE) &&
	 (usv->servertype != entry->server.server_type)))
	{
	    return(FALSE);
	}

    any_idb_up = nov_any_idb_up(entry);

    /*
     * Screen by basic update characteristics ... unicast, periodic
     * broadcast, partial, linkdown, reverse poison, etc.
     */
    if ((!bcast) ||

	(part && !TIMER_LATER(lastupdate, entry->sap_changed) &&
	 ((any_idb_up && sap_reachable) ||
	  (entry->poisoned && !entry->reverse_poisoned))) ||
	
	(full && !poison_update && (sap_reachable || entry->poisoned)) ||

	(linkdown && sap_reachable) ||

	(reverse && sap_reachable && !any_idb_up)) {
	
	/*
	 * Essentially, advertise only if split horizon passes, and
	 * one interface associated with the entry->nidb is UP
	 * ..or... we are in a linkdown_poison pass for this
	 * interface...
	 * 
	 * If ipx_eigrp_sap_split_horizon is not enabled, split horizon
	 * is not applied if the interface is doing SAP incremental and
	 * we are sending out an EIGRP encapsulated packet.
	 */ 

	*eigrp_must_poison = FALSE;  

	split_horizon_occured =
	    novell_sap_split_horizon(entry, nidb,
				     (ripsap_ucast ?
				      IPX_ADVERTISING_NO :
				      IPX_ADVERTISING_YES));

	/*
	 * If running INCREMENTAL SAP, and split horizon did NOT occur,
	 * advertise as normal. If split horizon DID occur, a special
	 * check must be made to see if this server "changed interfaces"
	 * since we last poisoned it. If it did, we have to readvertise
	 * as POISON on all interfaces now split horizoned running EIGRP.
	 */
	if (ipx_eigrp_sap_split_horizon &&
	    split_horizon_occured &&
	    NOVIGRP_DOING_INCREMENTAL_SAP(nidb) &&
	    entry->nidb_changed)
	    {
		split_horizon_occured = FALSE;
		*eigrp_must_poison = TRUE;
	    }
	/*
	 * Reject if split horizon fails:
	 */
	if (!split_horizon_occured) {
	    /*
	     * These seem a bit redundant but are historical:
	     */
	    if ((entry->poisoned || (any_idb_up && reachable_via_route) ||
		 (reverse && nov_all_idb_linkdown_poisons(entry))))
		{
		    /*
		     * If name query, check for a match:
		     */
		    if ((usv->query_type != NOVELL_SAP_GENNAMEQUERY) ||
			nov_sap_name_check(entry,
					   usv->query_type,
					   usv->servertype,
					   (char *)&usv->server_name,
					   usv->dnet,
					   usv->dmask)) {
			if (nov_sap_out_filter(entry, nidb->idb)) {
			    if (debug) buginf(" YES");
			    return(TRUE);
			}
		    }
		}
	}
    }
    if (debug) buginf(" NO");
    return(FALSE);
}


/*
 * nov_valid_choice_network
 *
 * Is the network given in 'p' a valid choice to advertise out 'nidb', given
 * the values of 'fullupdate', 'lastupdate' and 'split' passed in? This
 * code used to reside in the middle of getnetwork, but it was split out
 * so others could call this section of the logic.
 *
 * This routine should possibly be split into multiple routines for clarity
 * and maintainability. Individual routines would handle a particular type
 * of update (full bcast, partial, linkdown poison, reverse poison, full
 * unicast, etc.)
 *
 * Summary of current behavior:
 *
 * - Ignore deleted nets.
 * - Include net in holddown only in broadcast (full or partial) updates.
 * - Do not advertise reachable nets if reverse poison
 * - Do not reverse poison if already done in a previous update.
 * - Do not flash unless changed since last full update.
 * - Run split horizon if requested
 * - Exclude networks known only through a down interface from full
 *   and partial updates. These are advertised via linkdown and
 *   reverse poisons instead.
 */
static boolean nov_valid_choice_network (novndbtype *p, nidbtype *nidb,
					 enum IPX_FULL fullupdate,
					 int split, sys_timestamp lastupdate,
					 enum IPX_DESTINATION destination,
					 boolean doing_reverse_poisons)
{
    int i;
    boolean inlist;
    boolean full = (fullupdate == IPX_FULL_NEEDED);
    boolean part = !full;
    boolean bcast = (destination == IPX_DESTINATION_BROADCAST);
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);
    
    /* This route is no good */
    if (p == NULL || (p->options & IPX_NDB_OPTION_DELETED)) {
	if (debug) buginf(" (DEL)");
 	return(FALSE);
    }
    
    /* Routes that are in holddown only get sent out in broadcast updates */
    if (TIMER_RUNNING(p->holddownuntil) && !bcast) {
	if (debug) buginf(" (HELD)");
        return(FALSE);
    }
    
    /*
     * Screen reverse poisons:
     */
    if (doing_reverse_poisons) {
	/*
	 * Never advertise reachable networks when doing a reverse poison:
	 */
	if ((p->path_list != NULL) && novrip_interface_up(p->path_list)) {
	    if (debug) buginf(" (PATH)");
	    return(FALSE);
	}
	/*
	 * If entry needs reverse poison but hasn't been processed by the
	 * linkdown handler yet, skip it. (We'll get it on the rebound.)
	 */
	if (!p->reverse_poisoned) {
	    if (debug) buginf(" (REVP)");
	    return(FALSE);
	}
	
	/*
	 * Also, only advertise to point of last linkdown event on this
	 * interface. All networks needing reverse poisons since then are
	 * either newer or have been recently promoted to newest by
	 * net_down_handler.
	 */
	if (TIMER_LATER(lastupdate, p->ndb_changed)) {
	    if (debug) buginf(" (AGE)");
	    return(FALSE);
	}
    }
    
    /*
     * If this is a partial (flash) update, only advertise things that
     * have changed since the last full or partial update. All networks
     * needing a flash are either newer or have been promoted to newest
     * since that time.
     */
    if (part && bcast && TIMER_LATER(lastupdate, p->ndb_changed)) {
	if (debug) buginf(" (AGE)");
	return(FALSE);
    }
    
    /*
     * Don't send this net if we are doing split horizen and this update
     * could be sent back out *any* interface that leads back to this net.
     */
    if (nov_same_nidb(nidb, p->path_list)) {
	if (split == SPLIT_TRUE) {
	    if (debug) buginf(" (SHRZ)");
	    return(FALSE);
	}
    }
    /*
     * Routes that have to go through a down interface are not
     * advertised at all if this is a full or partial update. That's
     * because these are advertised when linkdown and reverse poisons
     * are sent so this rule prevents a gratuitous flash. Static and
     * directly connected routes are an exception. (??? Not sure why ???)
     *
     * For the reason stated above, this check should NOT be performed when
     * doing poison updates. Likewise, don't try this when the route is
     * NLSP in holddown (because there is no path list).
     */
    if (!doing_reverse_poisons) {
	if (!(p->flags & (NET_CONNECTED|NET_STATIC)) &&
	    (!((p->flags & NET_NLSP) && TIMER_RUNNING(p->holddownuntil)))) {
	    if ((p->path_list == NULL) ||
		(!novrip_interface_up(p->path_list))) {
		if (debug) buginf(" (DOWN)");
		return(FALSE);
	    }
	}
    }
    
    /* 
     * Static routes don't get deleted, after awhile they
     * are marked as invalid and then we just dont say
     * anything more about them until they come back
     */
    if ((p->flags & NET_STATIC) && (!(p->options & IPX_NDB_OPTION_VALID))) {
	if (debug) buginf(" (STAT)");
	return(FALSE);
    }
    /*
     * See if we should be redistributing this route from another
     * domain. Redistribution of everything into RIP is on by
     * default.
     */
    if (!(p->flags & NET_RIP)) {
        if (!(novrip_pdb.distflag & INDEX2MASK(p->novpdbindex))) {
	    /*
	     * Not allowed, but check to see if this network is 
	     * also in the RIP domain (Connected routes that are
	     * listed in the "network" subcommand list are both
	     * part of the RIP domain and the Connected domain).
	     * If so, it must be announced in the RIP update anyway.
	     */
	    inlist = FALSE;
	    if (p->flags & NET_CONNECTED) {
	        for (i = 0; i < novrip_pdb.netcount; i++) {
		    if (novrip_pdb.networks[i] == p->dstnet) {
               	        inlist = TRUE;
		        break;
		    }
		}
	    }
	    if (!inlist) {
		if (debug) buginf(" (RIP)");
	        return(FALSE);
	    }
	}
    }
    
    /* Whew! It passed all of that. Guess it is okay... */
    return(TRUE);
}


/*
 * advertising_conn_network
 *
 * Select only connected networks for advertisement
 *
 * Usage is at link startup time only ... exact usage is TBS.
 */
/*
static boolean advertising_conn_network (novndbtype *entry,
					 nidbtype *nidb,
					 sys_timestamp lastupdate,
					 enum IPX_FULL fullupdate,
					 enum IPX_DESTINATION destination,
					 boolean reverse_poisons)
{
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);

    if (debug) {
	buginf("\n conn?: %-8x via %-8x:%s (%s:%s)",
	       entry->dstnet,
	       nidb->novellnet,
	       nidb->idb->short_namestring,
	       (fullupdate == IPX_FULL_NEEDED) ? "FULL" : "PART",
	       (destination == IPX_DESTINATION_BROADCAST) ?
	       "BCAST" : (destination == IPX_DESTINATION_UNICAST) ?
	       "UCAST" : "RS_UCAST");
    }

    if (!(entry->flags & (NET_CONNECTED|NET_INTERNAL)) ||
	!NOVELL_NIDB_IDB_UP(nidb)) {
	if (debug) buginf(" NO");
	return(FALSE);
    }
    
    if (!novrip_out_filter(entry,  entry->dstnet, nidb->idb)) {
	if (debug) buginf(" (ACL)");
	return(FALSE);
    }
    
    if (debug) buginf(" YES");

    return(TRUE);
}
*/

/*
 * nov_advertising_this_network
 *
 * Complex logic statement to determine if this route can be
 * sent. Most of the selection work is done in valid_choice_network().
 *
 * This routine screens out unreachable routes that have exceeded their
 * holddown period and those which should be filtered by an output
 * network filter.
 *
 * If all conditions are met, then this route can be advertised.
 */
static boolean nov_advertising_this_network (novndbtype *entry,
					     nov_usv *usv,
					     nidbtype *nidb,
					     sys_timestamp lastupdate,
					     enum IPX_FULL fullupdate,
					     enum IPX_DESTINATION destination,
					     boolean reverse_poisons)
{
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);

    if (debug) {
	buginf("\n ?: %-8x via %-8x:%s (%s:%s)",
	       entry->dstnet,
	       nidb->novellnet,
	       nidb->idb->short_namestring,
	       (fullupdate == IPX_FULL_NEEDED) ? "FULL" : "PART",
	       (destination == IPX_DESTINATION_BROADCAST) ?
	       "BCAST" : (destination == IPX_DESTINATION_UNICAST) ?
	       "UCAST" : "RS_UCAST");
    }
    if (!TIMER_RUNNING(entry->holddownuntil) &&
	(entry->hopcount >= NOVELL_RIP_ADV_MAX_HOPS) &&
	!reverse_poisons)
	{
	    if (debug) buginf(" (BAIL)");
	    return(FALSE);
	}

    if (!nov_valid_choice_network(entry,
				  nidb,
				  fullupdate,
				  SPLIT_TRUE,
				  lastupdate,
				  destination,
				  reverse_poisons)) {
	
	if (debug) buginf(" NO");
	return(FALSE);
    }

    if (!novrip_out_filter(entry,  entry->dstnet, nidb->idb)) {
	if (debug) buginf(" (ACL)");
	return(FALSE);
    }

    if (debug) buginf(" YES");

    return(TRUE);
}

/*
 * nov_get_nidb_usv_addr
 *
 * Return a pointer to the nidb pointer for an nidb permanent usv.
 * Return NULL for ucast updates since at the present time, there
 * is no ucast usv allocated in the nidb (they are always dynamic).
 * Finally, note that returning a pointer to the pointer to the usv
 * doesn't mean that the usv has been allocated.
 */
static nov_usv **nov_get_nidb_usv_addr (nidbtype *nidb,
					enum IPX_UPD_TYPE type,
					boolean bcast,
					boolean full,
					boolean down,
					boolean revp)
{
    boolean rip = (type == IPX_UPD_RIP);
    boolean sap = (type == IPX_UPD_SAP);

    if (rip) {
	if (bcast) {
	    if (full) {
		if (down) {
		    return((nov_usv **)&nidb->rip_down);
		} else if (revp) {
		    return((nov_usv **)&nidb->rip_revp);
		} else {
		    return((nov_usv **)&nidb->rip_full);
		}
	    } else {
		    return((nov_usv **)&nidb->rip_part);
	    }
	} else {
	    return(NULL); /* unicast */
	}
    } else if (sap) {
	if (bcast) {
	    if (full) {
		if (down) {
		    return((nov_usv **)&nidb->sap_down);
		} else if (revp) {
		    return((nov_usv **)&nidb->sap_revp);
		} else {
		    return((nov_usv **)&nidb->sap_full);
		}
	    } else {
		return((nov_usv **)&nidb->sap_part);
	    }
	} else {
	    return(NULL); /* unicast */
	}
    }
    return(NULL);
}

/*
 * debug_display_update_type
 *
 * Common routine to print some useful debug info without obfuscation
 * of the calling routine with excessive source gorp.
 */
static void debug_display_update_type (enum IPX_UPD_TYPE type,
				       boolean bcast,
				       boolean full,
				       boolean down,
				       boolean revp)
{
    if (type == IPX_UPD_RIP) {
	buginf("(RIP)");
    } else if (type == IPX_UPD_SAP) {
	buginf("(SAP)");
    } else {
	buginf("(???)");
    }

    if (bcast) {
	if (full) {
	    if (down) {
		buginf("(DOWN)");
	    } else if (revp) {
		buginf("(REVP)");
	    } else {
		buginf("(FULL)");
	    }
	} else {
	    buginf("(PART)");
	}
    } else {
	buginf("(UCAST)");
    }
}


/*
 * compute_gap
 *
 * Compute the interpacket gap for a RIP or SAP update
 *
 */
static int compute_gap (nov_usv *usv)
{
    int gap = 1;
    nidbtype *nidb = usv->nidb;
    boolean rip = usv->rip_request;
    boolean bcast = (usv->dest == IPX_DESTINATION_BROADCAST);
    boolean full = (usv->full == IPX_FULL_NEEDED);
    boolean down = usv->linkdown_poison;
    boolean revp = usv->reverse_poison;

    full = (full && (!(down || revp)));

    if (rip) {
	if (bcast && full && !novell_shutting_down) {
	    /*
	     * RIP periodic update:
	     */
	    gap = nidb->idb->novell_rip_delay;
	} else {
	    /*
	     * RIP unicast or triggered (or we are going down):
	     */
	    if (nidb->trig_rip_delay || nidb->idb->novell_rip_delay) {
		if (nidb->trig_rip_delay != NOV_OUTPUT_DELAY_DEFAULT) {
		    gap = nidb->trig_rip_delay;
		} else {
		    gap = nidb->idb->novell_rip_delay;
		}
	    }
	}
    } else {
	if (bcast && full && !novell_shutting_down) {
	    /*
	     * SAP periodic update:
	     */
	    gap = nidb->idb->novell_sap_delay;
	} else {
	    /*
	     * SAP unicast or triggered (or we are going down):
	     */
	    if (nidb->trig_sap_delay || nidb->idb->novell_sap_delay) {
		if (nidb->trig_sap_delay != NOV_OUTPUT_DELAY_DEFAULT) {
		    gap = nidb->trig_sap_delay;
		} else {
		    gap = nidb->idb->novell_sap_delay;
		}
	    }
	}
    }
    /*
     * Always introduce some delay:
     */
    if (gap == 0) {
	gap = 1;
    }
    return(gap);
}

/*
 * nov_reset_usv
 *
 * Reset an update state variable before use by the next update. Note
 * that status of the pending flag will be returned. The pending flag
 * set when a valid update is posited and the matching usv is already
 * marked as active. A usv so marked is always resubmitted as an
 * active update.
 *
 * NOTE: This routine will accept and deallocate a packet buffer
 * allocated previously and stored in the USV.
 */
static boolean nov_reset_usv (nov_usv *usv)

{

    if (usv == NULL) {
	return(FALSE);
    }

    /*
     * Delete placeholders, stop timers, recompute the interpacket gap,
     * clear counters, and capture the pending status.
     */
    nov_delete_ndb_placeholder(usv->ndb_placeholder);
    nov_delete_sap_placeholder(usv->sap_placeholder);
    usv->placeheld = FALSE;
    mgd_timer_stop(&usv->timer);
    usv->gap = compute_gap(usv);

    usv->active = usv->pending;
    usv->pending = FALSE;
    usv->rip_update_total = 0;
    usv->sap_update_total = 0;
    usv->sap_update_count = 0;

    /*
     * Remember state instance of nidb; if it ever changes, it means
     * the assumptions under which we are choosing to send data about
     * this nidb are no longer valid
     */
    usv->state_start = usv->nidb->state_version;
    usv->error_occured = FALSE;

    /*
     * We shouldn't be holding a packet buffer at this point but
     * return it if it is there. Better safe than sorry.
     */
    if (usv->update_pak != NULL) {
	retbuffer(usv->update_pak);
    }

    usv->update_pak = NULL;
    TIMER_STOP(usv->last_update_time);
    usv->sent_an_update = FALSE;

    return(usv->active);
}


/*
 * nov_deallocate_usv
 *
 * Deallocate a usv AND ESPECIALLY any resources associated with it.
 *
 * NOTE: This routine will accept and deallocate a packet buffer
 * allocated previously and stored in the USV.
 */
static void nov_deallocate_usv (nov_usv *usv)
{
    nidbtype *nidb;
    nov_usv **nidb_usv;
    boolean debug = ((novell_ripupd_debug || novell_sapupd_debug) &&
		     novell_verbose_debug);

    if (usv == NULL) {
	return;
    }

    nidb = usv->nidb;

    if (debug) {
	buginf("\nIPX: Deallocate usv for %s", nidb->idb->short_namestring);
	debug_display_update_type(usv->sap_request,
				  (usv->dest == IPX_DESTINATION_BROADCAST),
				  usv->full,
				  usv->linkdown_poison,
				  usv->reverse_poison);
    }
    /*
     * Packet buffer:
     */
    if (usv->update_pak != NULL) {
	retbuffer(usv->update_pak);
	usv->update_pak = NULL;
    }
    /*
     * Placeholders:
     */
    if (usv->ndb_placeholder != NULL) {
	nov_delete_ndb_placeholder(usv->ndb_placeholder);
	free(usv->ndb_placeholder);
    }

    if (usv->sap_placeholder != NULL) {
	nov_delete_sap_placeholder(usv->sap_placeholder);
	free(usv->sap_placeholder);
    }
    /*
     * Stop managed timer:
     */
    mgd_timer_stop(&usv->timer);

    /*
     * If this usv is known in the nidb, get it's addr
     * so that no one tries to reference it from there.
     */
    nidb_usv = nov_get_nidb_usv_addr(nidb,
				     usv->sap_request,
				     (usv->dest == IPX_DESTINATION_BROADCAST),
				     usv->full,
				     usv->linkdown_poison,
				     usv->reverse_poison);

    if (nidb_usv != NULL) {
	*nidb_usv = NULL;
    }
    
    /*
     * Free at last!
     */
    free(usv);
}


/*
 * nov_allocate_and_init_usv
 *
 * Allocate and initialize a usv and all required resources. Ensure
 * that the usv contains appropriate status information required to
 * perform a particular function.
 */

static nov_usv *nov_allocate_and_init_usv (enum IPX_UPD_TYPE type,
				    nidbtype *nidb,
				    uchar const *host,
				    ushort sock,
				    enum IPX_FULL fullupdate,
				    enum IPX_DESTINATION destination,
				    ushort servertype,
				    ushort query_type,
				    uchar *server_name, /* %%% */
				    ulong dnet,
				    ulong dmask,
				    boolean reverse,
				    boolean linkdown)
{
    nov_usv *usv;
    boolean full  = (fullupdate == IPX_FULL_NEEDED);
    boolean bcast = (destination == IPX_DESTINATION_BROADCAST);
    nov_usv **nidb_usv;
    boolean debug = ((novell_ripupd_debug || novell_sapupd_debug) &&
		     novell_verbose_debug);

    if (nidb == NULL) {
	return(NULL);
    }

    if ((type != IPX_UPD_RIP) && (type != IPX_UPD_SAP)) {
	return(NULL);
    }

    if (debug) {
	buginf("\nIPX: Allocate usv for %s ", nidb->idb->short_namestring);
	debug_display_update_type(type, bcast, full, linkdown, reverse);
    }

    /*
     * The major "static" usv types are allocated once when an nidb is
     * allocated and never deallocated. Make sure these are not duplicates.
     */
    nidb_usv = nov_get_nidb_usv_addr(nidb, type, bcast, full,
				     linkdown, reverse);

    if (nidb_usv != NULL) {
	if (*nidb_usv != NULL) {
	    return(NULL);
	}
    }

    if (debug) buginf("(ALLOC)");

    usv = malloc_named(sizeof(nov_usv), "IPX USV");

    if (usv == NULL) {
	return(NULL);
    }
    
    /*
     * Allocate a placeholder for the duration of the update. Bail
     * if not available.
     */
    usv->ndb_placeholder = nov_ndb_malloc_placeholder();
    if (usv->ndb_placeholder == NULL) {
	free(usv);
	return(NULL);
    }

    usv->sap_placeholder = nov_sap_malloc_placeholder();
    if (usv->sap_placeholder == NULL) {
	free(usv->ndb_placeholder);
	free(usv);
	return(NULL);
    }

    usv->placeheld = FALSE;

    usv->rip_request = (type == IPX_UPD_RIP);
    usv->sap_request = (type == IPX_UPD_SAP);

    /*
     * Init managed timer:
     */
    mgd_timer_init_leaf(&usv->timer,
			(usv->rip_request ?
			 &novell_rip_delay_timer : &novell_sap_delay_timer),
			IPX_TMR_GAP, usv, FALSE);
    
    /*
     * Set the correct selector routine by update type. In future,
     * a seperate selector routine could be created for each update
     * variant.
     */
    if (usv->rip_request) {
	usv->adv_rip = nov_advertising_this_network;
    } else {
	usv->adv_sap = nov_advertising_this_server;
    }
    /*
     * Remember the basic update status corresponding to the posit:
     */
    usv->nidb = nidb;
    ieee_copy(host, (uchar *)&usv->host_addr);
    usv->host = (uchar *)&usv->host_addr;
    usv->sock = sock;
    usv->full = fullupdate;
    usv->dest = destination;
    usv->servertype = servertype;
    usv->query_type = query_type;
    usv->dnet = dnet;
    usv->dmask = dmask;

    if (query_type != 0) {
	sstrncpy(usv->server_name, server_name,
		 NOVELL_SERVER_NAME_LEN);
    } else {
	usv->server_name[0] = '\0';
    }

    usv->gap = compute_gap(usv);

    /*
     * Remember state instance of nidb; if it ever changes, it means
     * the assumptions under which we are choosing to send data about
     * this nidb are no longer valid
     */
    usv->state_start = nidb->state_version;
    usv->sap_update_count = 0;
    usv->update_pak = NULL;
    TIMER_STOP(usv->last_update_time);
    usv->sent_an_update = FALSE;
    /*
     * Remember poison updates. This makes is easier to
     * distinguish poisons from full and partial updates.
     */
    usv->reverse_poison = reverse;
    usv->linkdown_poison = linkdown;

    /*
     * Fill in the appropriate update descriptor in the nidb for
     * the permanent update types. These include, full, partial,
     * linkdown poison, and reverse poison updates. These are used
     * by the posit routines to figure out if a particular update
     * is already active or not. Note that if this is a unicast
     * update, there is no permanent copy kept in the nidb.
     */
    if (nidb_usv != NULL) {
	*nidb_usv = usv;
    }

    return(usv);
}


/*
 * nov_schedule_update
 *
 * Given a particular type of update, either find the static usv
 * already allocated for this, or dynamically allocate an new usv.
 *
 */
static boolean nov_schedule_update (enum IPX_UPD_TYPE type,
				    nidbtype *nidb,
				    uchar const *host,
				    ushort sock,
				    enum IPX_FULL fullupdate,
				    enum IPX_DESTINATION destination,
				    ushort servertype,
				    ushort query_type,
				    uchar *server_name, /* %%% */
				    ulong dnet,
				    ulong dmask,
				    boolean reverse,
				    boolean linkdown)
{
    nov_usv *usv = NULL;
    boolean full  = (fullupdate == IPX_FULL_NEEDED);
    boolean bcast = (destination == IPX_DESTINATION_BROADCAST);
    boolean rip_request = (type == IPX_UPD_RIP);
    boolean sap_request = (type == IPX_UPD_SAP);
    novndbtype *ndb = NULL;
    novell_sapentrytype *sap = NULL;
    ulong holddown;
    nov_usv **nidb_usv;
    boolean rdebug = novell_ripupd_debug;
    boolean sdebug = novell_sapupd_debug;
    boolean debug = (rdebug || sdebug);

    if (nidb == NULL) {
	return(FALSE);
    }

    if ((rip_request && rdebug) || (sap_request && sdebug)) {
	buginf("\nIPX: Sched upd for %s ", nidb->idb->short_namestring);
	debug_display_update_type(type, bcast, full, linkdown, reverse);
    }

    if ((!rip_request) && (!sap_request)) {
	return(FALSE);
    }
    
    /*
     * Bail if there is nothing to update:
     */
    if (rip_request) {
	ndb = nov_get_first_ndb();
	if (ndb == NULL) {
	    if (debug) buginf(" (NONE)");
	    return(FALSE);
	}
    } else {
	sap = nov_get_first_sap();
	if (sap == NULL) {
	    if (debug) buginf(" (NONE)");
	    return(FALSE);
	}
    }

    /*
     * Locate the appropriate usv given the requested update:
     */
    nidb_usv = nov_get_nidb_usv_addr(nidb, type, bcast, full,
				     linkdown, reverse);
    if (nidb_usv != NULL) {
	usv = *nidb_usv;
    } else {
	/*
	 * All unicasts are allocated dynamically at this time:
	 */
	usv =
	    nov_allocate_and_init_usv(type, nidb, host, sock, fullupdate,
				      destination, servertype, query_type,
				      server_name, dnet, dmask,
				      reverse, linkdown);
    }

    /*
     * Return failure if we couldn't find one or couldn't allocate one:
     */
    if (usv == NULL) {
	if (debug) {
	    buginf("(NULL)");
	    if (novell_verbose_debug)
		bugtrace();
	}
	return(FALSE);
    }
    /*
     * If there is already an update in progress, remember the newer
     * request in the pending flag.
     */
    if (usv->active) {
	usv->pending = TRUE;
	if (debug) buginf("(PENDING)");
	return(FALSE);
    }
	
    /*
     * Make sure everything is initialized correctly, including
     * interpacket gap.
     */
    nov_reset_usv(usv);

    if (debug) buginf(": gap = %d", usv->gap);

    usv->active = TRUE;

    /*
     * Start partial updates with initial 55 mSec holddown, as
     * recommended by Novell. Start all other updates NOW!!!
     * Fire up a managed timer.
     */

    if (full) {
	holddown = NOV_UPDATE_HOLDDOWN;
    } else {
	holddown = NOV_OUTPUT_DELAY_DEFAULT;
    }

    if (debug) buginf(", holddown = %d", holddown);

    mgd_timer_start(&usv->timer, holddown);

    /*
     * OK, let the appropriate daemon take over to completion:
     */
    return(TRUE);
}

/*
 * nov_allocate_and_init_rip_packet
 *
 * Allocate a packet buffer of appropriate size and initialize for use.
 *
 */
static boolean nov_allocate_and_init_rip_packet (nov_usv *usv,
						 nidbtype *nidb,
						 uchar const *host,
						 ushort sock)
{
    ipxhdrtype *ipx;

    usv->rip_bytes = nidb->idb->nov_rip_maxpacketsize;

    usv->update_pak = getbuffer(usv->rip_bytes);
    if (usv->update_pak == NULL) {
	return(FALSE);
    }

    ipx = (ipxhdrtype *)ipxheadstart(usv->update_pak);

    usv->rip_bytes -= (XNSOVERHEADBYTES + NOVELL_RIP_HEADERSIZE);
    usv->update_pak->nif_output = nidb;
    usv->update_pak->if_output = nidb->idb;
    usv->update_pak->linktype = LINK_NOVELL;
    ipx->tc = 0;
    ipx->pt = NOVELL_TYPE_RIP;
    PUT2WD(ipx->ddnet, nidb->novellnet);
    ieee_copy(host, ipx->dhost);
    ipx->dsock = sock ? sock : NOVELL_SOCK_RIP;
    PUT2WD(ipx->ssnet, nidb->novellnet);
    ieee_copy(nidb->idb->novell_node, ipx->shost);
    ipx->ssock = NOVELL_SOCK_RIP;
    usv->rip =
	(novellriptype *)(ipxheadstart(usv->update_pak) +
			  XNSHEADERBYTES);
    usv->rip->cmd = RIP_REPLY;
    usv->rip_ptr = usv->rip->nets;
    return(TRUE);
}

/*
 * nov_allocate_and_init_sap_packet
 *
 * Allocate a packet buffer of appropriate size and initialize for use.
 *
 */
static boolean nov_allocate_and_init_sap_packet (nov_usv *usv,
						 nidbtype *nidb,
						 uchar const *host,
						 ushort sock)
{
    ipxhdrtype *ipx;



    usv->update_pak = getbuffer(nidb->idb->nov_sap_maxpacketsize);

    if (usv->update_pak == NULL) {
	return(FALSE);
    }

    ipx = (ipxhdrtype *)ipxheadstart(usv->update_pak);

    usv->update_pak->nif_output = nidb;
    usv->update_pak->if_output = nidb->idb;
    usv->sap_update_count = 0;
    usv->update_pak->linktype = LINK_NOVELL;
    ipx->tc = 0;
    ipx->pt = NOVELL_TYPE_SAPRESP;
    ieee_copy(host, ipx->dhost);
    ipx->dsock = sock ? sock : NOVELL_SOCK_SAP;
    PUT2WD(ipx->ddnet, nidb->novellnet);
    PUT2WD(ipx->ssnet, nidb->novellnet);
    ieee_copy(nidb->idb->novell_node, ipx->shost);
    ipx->ssock = NOVELL_SOCK_SAP;
    ipx->cksum = NOVELL_NO_CHECKSUM;
    
    usv->sap =
	(novell_sappacket *)
	    (XNSDATASTART(usv->update_pak));

    if (usv->query_type == NOVELL_SAP_GENNAMEQUERY) {
	usv->sap->response_type = NOVELL_SAP_GENNAMERESPONSE;
    } else {
	usv->sap->response_type = NOVELL_SAP_GENRESPONSE;
    }

    return(TRUE);
}


/*
 * nov_set_outbound_rip_metric
 *
 * Set an appropriate RIP metric directly in an outgoing update
 * packet. The exact metric advertised depends on the state of
 * the entry and the type of update.
 * 
 */
static void nov_set_outbound_rip_metric (novndbtype *entry,
					 nidbtype *nidb,
					 novellripinfotype *rip_ptr,
					 boolean reverse_poison,
					 boolean linkdown_poison)
{
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);
    
    if (linkdown_poison || TIMER_RUNNING(entry->holddownuntil)) {
	rip_ptr->metric = NOVELL_RIP_ADV_INFINITY;
    } else {
	nov_get_rip_metrics(entry, rip_ptr, nidb);
    }
    if ((rip_ptr->metric != NOVELL_RIP_ADV_INFINITY) && reverse_poison) {
	if ((entry->path_list == NULL) ||
	    (!novrip_interface_up(entry->path_list)))
	    {
		rip_ptr->metric = NOVELL_RIP_ADV_INFINITY;
	    }
    }
    if (!(entry->flags & NET_IGRP)) { /* Handle non-IGRP case */
	/*
	 * For RIP, set tick count correctly even if poison. Note
	 * that Novell, in their infinite wisdom, has given us
	 * lots of options here. All known specs agree that a
	 * hop count of 16 means "unreachable". The NLSP spec
	 * says that the delay field is "unspecified" (p. 7-9)
	 * if hops == 16. The IPX spec says the delay field
	 * should be set to "correct" ticks value in this case.
	 * Common sense says that a poison update should set
	 * ticks to 0xFFFF ... but if you do this the MPR will
	 * discard the poison which is REALLY BAD. Once again,
	 * common sense says you probably should NOT set it
	 * to 0 since a zero delay is clearly wrong. Let's go
	 * with the IPX spec moving forward ... we can quote
	 * it if questioned, back it up with the NLSP spec, and
	 * rest easy that the MPR won't pitch it. Whew!
	 */
	if (rip_ptr->metric == NOVELL_RIP_ADV_INFINITY) {
	    /* Set delay for poison update */
	    rip_ptr->delay =
		entry->last_delay + NOVELL_IDB_TICKS(nidb);
	} else {
	    /* Set delay for normal update */
	    rip_ptr->delay =
		entry->delay + NOVELL_IDB_TICKS(nidb);
	}
	/* Make sure we NEVER send a ZERO delay */
	if (rip_ptr->delay == 0) {
	    rip_ptr->delay = 1;
	}
    }
    if (debug) {
	buginf(" @[%-3d/", rip_ptr->delay);
	if (rip_ptr->metric == NOVELL_RIP_ADV_INFINITY) {
	    buginf("**]");
	} else {
	    buginf("%-2d]", rip_ptr->metric);
	}
    }
}

/*
 * nov_fill_rip_packet
 *
 * Attempt to build a complete update packet with entries appropriate
 * for the type of update. Return a pointer to an ndb if the current
 * update packet is full but traversal of the route table is not yet
 * complete. Return a NULL pointer if this is the last packet in an
 * update. Note that an empty update packet is recognized by the fact
 * that usv->rip_update_count is zero.
 */
static novndbtype *nov_fill_rip_packet (novndbtype *ph,
					nidbtype *nidb,
					nov_usv *usv,
					enum IPX_FULL fullupdate,
					enum IPX_DESTINATION destination)
{
    novndbtype *entry;

    /*
     * Get next candidate route. Remove the placeholder which we
     * inserted in the route list to mark our position following
     * processing of the previous packet.
     */
    entry = nov_delete_ndb_placeholder(usv->ndb_placeholder);

    /*
     * If advertising default route only then only care about
     * the default route. See if we have one.
     */
    if (nidb->adv_default_route_only) {
	entry = novell_default_ndb;
    }

    /*
     * Now step through the route list and check successive entries
     * to see if they should be advertised out this interface. The
     * adv_rip() procedure will return different results depending
     * on which route, which nidb, and which kind of update we are
     * doing. Continue until we either fill up the packet or reach
     * the end of the list.
     */
    
    while ((entry != NULL) &&
	   (usv->rip_bytes >= sizeof(novellripinfotype))) {

	if ((*usv->adv_rip)(entry,
			    usv,
			    nidb,
			    usv->lastupdate,
			    fullupdate,
			    destination,
			    usv->reverse_poison)) {
	    /*
	     * Got one to advertise. Set the metric appropriately:
	     */
	    nov_set_outbound_rip_metric(entry, nidb,
					usv->rip_ptr,
					usv->reverse_poison,
					usv->linkdown_poison);

	    PUT2WD(usv->rip_ptr->nnet, entry->dstnet);;
	    usv->rip_bytes -= sizeof(novellripinfotype);
	    usv->rip_ptr++;
	    usv->rip_update_count++;
	}

	/*
	 * Get the next candidate route (unless we are advertising
	 * default route only). Bail if no more routes.
	 */
	if (nidb->adv_default_route_only) {
	    entry = NULL;
	} else {
	    entry = nov_get_next_ndb(entry);
	    /*
	     * We may not need to continue if this is a partial or reverse
	     * poison and we have already flashed this entry. Since the
	     * list we are traversing is ordered from newest to oldest,
	     * we can safely ignore all remaining entries as well.
	     */
	    if (entry != NULL) {
		if (usv->dest == IPX_DESTINATION_BROADCAST) {
		    if (usv->reverse_poison || (nidb->rip_part == usv)) {
			if (TIMER_LATER(usv->lastupdate, entry->ndb_changed)) {
			    entry = NULL;
			}
		    }
		}
	    }
	}
    }

    /*
     * If we have not reached the end of the route list, mark our
     * position with the placeholder. After the current packet
     * is sent we may delay for quite a while.
     */
    if (entry != NULL) {
	nov_insert_ndb_placeholder(entry, usv->ndb_placeholder);
	usv->placeheld = TRUE;
	entry = usv->ndb_placeholder;
    } else {
	usv->placeheld = FALSE;
    }
    /*
     * If we return NULL then we have exhausted the route list or
     * decided to terminate for some other reason. If we return
     * non-null then the update engine will call us again later
     * to continue where we left off.
     */
    return(entry);
}

/*
 * nov_fill_sap_packet
 *
 * Build a SAP update packet ... analagous to similar RIP function.
 */
static novell_sapentrytype *nov_fill_sap_packet (novell_sapentrytype *ph,
						 nidbtype *nidb,
						 nov_usv *usv,
						 enum IPX_FULL fullupdate,
						 enum IPX_DESTINATION destination)
{
    novell_sapentrytype *entry, *next_entry;
    novell_serveridtype *data;
    boolean next_entry_valid;
    boolean eigrp_must_poison;
    volatile ushort nethops; /* CSCdi46488 */

    entry = nov_delete_sap_placeholder(usv->sap_placeholder);
    
    while ((entry != NULL) &&
	   (usv->sap_update_count <
	    NOVELL_SAP_MAXSIZE_TO_SAP_COUNT(nidb->idb->nov_sap_maxpacketsize))) {

	eigrp_must_poison = FALSE;
	next_entry = NULL;
	next_entry_valid = FALSE;

	if ((*usv->adv_sap)(entry,
			    usv,
			    nidb,
			    usv->lastupdate,
			    fullupdate,
			    destination,
			    usv->reverse_poison, usv->linkdown_poison,
			    &eigrp_must_poison)) {
	    
	    data = (novell_serveridtype *)
		&(usv->sap->server[usv->sap_update_count]);
	    bcopy((uchar *)&entry->server, (uchar *)data, 
		  sizeof(novell_serveridtype));
	    /*
	     * Set server metric. Convoluted logic is Work around
	     * for compiler bug (CSCdi46488).
	     */
	    nethops = !(entry->route_flags & NET_IGRP) ?
		data->nethops + 1 : (novrip_pdb.redist_hops) ?
		    novrip_pdb.redist_hops : entry->route_hops + 1;
	    data->nethops = nethops;

	    if (usv->linkdown_poison ||
		nov_all_idb_linkdown_poisons(entry) ||
		entry->poisoned || eigrp_must_poison)
		{
		    data->nethops = NOVELL_RIP_ADV_INFINITY;
		}
	    
	    /*
	     * Count this entry:
	     */
	    usv->sap_update_count++;
	}
	/*
	 * Continue down list:
	 */
	entry = nov_get_next_sap(entry);
	/*
	 * We may not need to continue if this is a partial or reverse
	 * poison and we have already flashed this entry. Since the
	 * list we are traversing is ordered from newest to oldest,
	 * we can safely ignore all remaining entries as well.
	 */
	if (entry != NULL) {
	    if (usv->dest == IPX_DESTINATION_BROADCAST) {
		if (usv->reverse_poison || (nidb->sap_part == usv)) {
		    if (TIMER_LATER(usv->lastupdate, entry->sap_changed)) {
			entry = NULL;
		    }
		}
	    }
	}
    }
    /*
     * If we return NULL then we have exhausted the route list or
     * decided to terminate for some other reason. If we return
     * non-null then the update engine will call us again later
     * to continue where we left off. Insert our placeholder so
     * we can continue from the right place.
     */
    if (entry != NULL) {
	nov_insert_sap_placeholder(entry, usv->sap_placeholder);
	entry = usv->sap_placeholder;
    }
    return(entry);
}

/*
 * nov_send_rip_update_packet
 *
 * Allocate a packet buffer, build an appropriate update packet,
 * and (possibly) send the packet via an appropriate sender.
 * The packet send may be aborted if snapshot is configured.
 */
static boolean nov_send_rip_update_packet (nov_usv *usv)
{
    novndbtype *entry;
    nidbtype *nidb;
    ipxhdrtype *ipx;
    boolean rdebug = novellrip_debug;
    boolean edebug = novellrip_event_debug;
    boolean debug = (rdebug || edebug);

    entry = usv->ndb_placeholder;

    if (entry == NULL)
	return(FALSE);

    nidb = usv->nidb;

    /*
     * If the nidb changed underneath us, forget it!
     */
    if ((usv->state_start != nidb->state_version) ||
	(nidb->idb == NULL) ||
	!interface_up(nidb->idb) ||
	!NOVELL_NIDB_SENDS_ALLOWED(nidb))
	{
	    if (novell_ripupd_debug) buginf("(NIDB CHANGED)");
	    return(FALSE);
	}
    
    if (!nov_allocate_and_init_rip_packet(usv,
					  nidb,
					  usv->host,
					  usv->sock)) {
	usv->error_occured = TRUE;
	return(FALSE);
    }
    
    usv->rip_update_count = 0;
    
    entry = nov_fill_rip_packet(entry, nidb, usv, usv->full, usv->dest);
    /*
     * Do we have anything to advertise?
     */
    if (usv->rip_update_count > 0) {
	/*
	 * Yes, freshen our IPX header pointer:
	 */
	ipx = (ipxhdrtype *)ipxheadstart(usv->update_pak);
	
	ipx->len =
	    nidb->idb->nov_rip_maxpacketsize - usv->rip_bytes;
	
	ipx->cksum = NOVELL_NO_CHECKSUM;
	usv->update_pak->flags |= PAK_PRIORITY;
	if (rdebug) {
	    buginf("\nIPXRIP: src=%x.%e, dst=%x.%e, packet sent",
		   nidb->novellnet, nidb->idb->novell_node,
		   nidb->novellnet, usv->host);
	    novell_printrippak(usv->update_pak);
	}
	
	if (reg_invoke_snapshot_update_denied(nidb->idb, !usv->full)) {
	    retbuffer((paktype*) usv->update_pak);
	    usv->update_pak = NULL;
	} else {
	    reg_invoke_snapshot_notice_update_pak((paktype*)usv->update_pak,
						  nidb->idb);
	    nov_send1(usv->update_pak,
		      usv->host,
		      ipx->len);
	    
	    usv->update_pak = NULL;
	    if (usv->full == IPX_FULL_NEEDED)
		novell_stat[NOVELL_RIPOUT]++;
	    else
		novell_stat[NOVELL_RIPFLASH]++;
	    nidb->idb->nov_rip_out_packets++;
	    
	    GET_TIMESTAMP(usv->last_update_time);
	    usv->sent_an_update = TRUE;
	    usv->rip_update_total++;
	}
	
	usv->rip_update_count = 0;
	return(entry != NULL);
	
    } else {
	if (debug)
	    buginf("\nIPXRIP: suppressing null update to %x.%e", 
		   nidb->novellnet, usv->host);
    }
    
    return(FALSE);
}

/*
 * nov_send_sap_update_packet
 *
 * Allocate a packet buffer, build an appropriate update packet,
 * and (possibly) send the packet via an appropriate sender.
 * The packet send may be aborted if snapshot is configured. The
 * send may be via EIGRP if incremental sap or rsup are active.
 *
 */
static boolean nov_send_sap_update_packet (nov_usv *usv)
{
    novell_sapentrytype *entry;
    nidbtype *nidb;
    ipxhdrtype *ipx;
    boolean sdebug = novellsap_debug;
    boolean edebug = novellsap_event_debug;
    boolean debug = (sdebug || edebug);
    
    entry = usv->sap_placeholder;

    if (entry == NULL) {
	return(FALSE);
    }
    nidb = usv->nidb;

    /*
     * If the nidb changed underneath us, forget it!
     */
    if ((usv->state_start != nidb->state_version) ||
	(nidb->idb == NULL) ||
	!interface_up(nidb->idb) ||
	!NOVELL_NIDB_SENDS_ALLOWED(nidb))
	{
	    if (novell_sapupd_debug) buginf("(NIDB CHANGED)");
	    return(FALSE);
	}
    
    if (!nov_allocate_and_init_sap_packet(usv,
					  nidb,
					  usv->host,
					  usv->sock)) {
	usv->error_occured = TRUE;
	return(FALSE);
    }

    usv->sap_update_count = 0;
    
    entry = nov_fill_sap_packet(entry, nidb, usv, usv->full, usv->dest);
    
    if (usv->sap_update_count > 0) {
	/*
	 * Freshen our IPX header pointer:
	 */
	ipx = (ipxhdrtype *)ipxheadstart(usv->update_pak);
	
	ipx->len = XNSOVERHEADBYTES + 2 +
	    (usv->sap_update_count * sizeof(novell_serveridtype));
	
	ipx->cksum = NOVELL_NO_CHECKSUM;
	usv->update_pak->flags |= PAK_PRIORITY;

	/*
	 * We have a SAP update ready to go. Now figure out how or if
	 * to send it. If incremental SAP is configured, use eigrp. If
	 * snapshot is configured, check the snapshot state. Maybe just
	 * deallocate the packet. If none of the above, call nov_send().
	 * Whew, life is complicated!!!
	 */

	if (NOVIGRP_DOING_INCREMENTAL_SAP(nidb) &&
	    (usv->dest != IPX_DESTINATION_RIPSAP_UNICAST)) {

	    if (!usv->error_occured) {
		
		usv->error_occured =
		    !reg_invoke_novigrp_send_sap(usv->update_pak, usv->dest, 
						 nidb);
		
		if (novellsap_debug) {
		    buginf("\nIPXEIGRP: Sending EIGRP SAP flash%s",
			   usv->error_occured ? " (ERROR)" : "");
		}
	    }
	} else {
	    if (reg_invoke_snapshot_update_denied(nidb->idb, !usv->full)) {
		retbuffer((paktype*) usv->update_pak);
		usv->update_pak = NULL;
	    } else {
		reg_invoke_snapshot_notice_update_pak
		    ((paktype*)usv->update_pak, nidb->idb);
		if (sdebug) novell_printpak(usv->update_pak, "Update");
		if (!nov_send(usv->update_pak)) {
		    if (sdebug) buginf("\nIPXSAP: cannot send SAP Update");
		} else {
		    usv->update_pak = NULL;
		    if (usv->full == IPX_FULL_NEEDED)
			novell_stat[NOVELL_SAPOUT]++;
		    else
			novell_stat[NOVELL_SAPFLASH]++;
		    nidb->idb->nov_sap_out_packets++;
		}
	    }
	    usv->sent_an_update = TRUE;
	}
	
	usv->update_pak = NULL;
	
	GET_TIMESTAMP(usv->last_update_time);

	usv->sap_update_total++;
	usv->sap_update_count = 0;
	
	return(entry != NULL);
	
    } else {
	if (debug)
	    buginf("\nIPXSAP: suppressing null update to %x.%e", 
		   nidb->novellnet, usv->host);
    }
    
    return(FALSE);
}


/*
 * nov_process_atlr
 *
 * Examine the advertise to lost route queue. If any entries are present,
 * the corresponding route is reachable, and the minimum atlr time has
 * elapsed (1/2 second), advertise this route to our peers. Entries
 * are placed on this queue when we receive poisons but continue to
 * have a valid, alternate route. Hopefully one of the recipients of
 * our flash update will be the originator of the earlier poison (lost
 * route). This is a bit of a hack in that we will flash this out
 * everywhere, not just the guy that lost it. Hey, if you don't like
 * hacks, don't run RIP.
 */
static void nov_process_atlr (void)
{
    novndbtype *ndb;

    /*
     * Now, process the routes needing advertisement to lost:
     */
    while (((ndb = novell_get_atlr_list_head()) != NULL) &&
	   (ELAPSED_TIME(ndb->atlr_time) >= NOVELL_ATLR_DELAY))
	{
	    novell_remove_from_atlr_list(ndb);
	    /* If it's deleted, don't bother... */
	    if (ndb->options & IPX_NDB_OPTION_DELETED)
		continue;
	    /* If not, horray! Advertise out to our peers! */
	    novell_mark_changed(ndb);
	}
}


/*
 * nov_super_timer_service
 *
 */
static void nov_super_timer_service (void)
{
    mgd_timer *expired_timer;
    ulong time_to_atlr;
    nidbtype *nidb;
    novndbtype *ndb;
    sys_timestamp current_time;
    
    while (mgd_timer_expired(&novell_super_root_timer)) {

	expired_timer = mgd_timer_first_expired(&novell_super_root_timer);
	mgd_timer_stop(expired_timer);
	
	switch (mgd_timer_type(expired_timer)) {
	    
	  case IPX_TMR_ATLR:

	    nov_process_atlr();
	    if ((ndb = novell_get_atlr_list_head()) != NULL) {
		GET_TIMESTAMP(current_time);
		time_to_atlr =
		    max(CLOCK_DIFF_UNSIGNED(ndb->atlr_time, current_time),
			NOVELL_ATLR_GRANULARITY);
		mgd_timer_start(&novell_atlr_timer, time_to_atlr);
	    }
	    break;
	    
	  case IPX_TMR_RIP_UPD:

	    nidb = (nidbtype *)mgd_timer_context(expired_timer);
	    if (NOVELL_NIDB_IDB_UP(nidb)) {
		novell_posit_full_rip_update(nidb);
		/*
		 * Schedule the next periodic update. However,
		 * jitter it ever so slightly so that updates
		 * on different interfaces space themselves out
		 * over the entire update period over time.
		 */
		mgd_timer_start_jittered(&nidb->rip_update_timer,
					 nidb->idb->nov_update_interval, 1);
	    } else {
		/*
		 * The timer is expired and the interface is down. This
		 * can happen when the interface is flapping.
		 * Schedule the timer to wake us up again in the next second
		 * to check it again. If the interface is really down, this
		 * timer should have been disable.
		 */
		mgd_timer_start(&nidb->rip_update_timer, ONESEC);
	    }
	    break;
	    
	  case IPX_TMR_SAP_UPD:
	    
	    nidb = (nidbtype *)mgd_timer_context(expired_timer);
	    
	    /*
	     * Only send full sap updates if configured and
	     * the network and interface are up. Note that
	     * special handling is required when "changes-only"
	     * is configured. In this mode, only linkup,
	     * link-down (poison) and partial (flash) updates
	     * are sent. The timer runs once for a full update
	     * but is then left stopped. It must be restarted
	     * explicitely as necessary.
	     *
	     * Also note that special handling is given when EIGRP
	     * SAP-INCREMENTAL is configured. In this mode the timer
	     * remains enabled (see below).
	     */
	    if (NOVELL_NIDB_IDB_UP(nidb) &&
		!nidb->novell_sap_after_rip &&
		(!NOVIGRP_DOING_INCREMENTAL_SAP(nidb)))
		{
		    novell_posit_full_sap_update(nidb);
		}
	    /*
	     * Restart the timer automatically for next time.
	     * For now, keep the timer running if the only
	     * thing holding us up is eigrp sap-incremental.
	     * Though non-optimal it's easy and safe and
	     * allows full updates to go out automatically
	     * when novigrp_have_peer(nidb) goes FALSE. Note
	     * that the timer remains stopped if the SAP update
	     * interval is 0 ("changes-only", as explained above).
	     */
	    
	    if (nidb->idb->novell_interval != 0) {
		if (NOVELL_NIDB_IDB_UP(nidb)) {
		    /*
		     * Schedule the next periodic update. However,
		     * jitter it ever so slightly so that updates
		     * on different interfaces space themselves out
		     * over the entire update period over time.
		     */
		    mgd_timer_start_jittered(&nidb->sap_update_timer,
					     nidb->idb->novell_interval*ONEMIN, 1);
		} else {
		    /*
		     * The timer is expired and the interface is down.
		     * This can happen when the interface is flapping.
		     * Schedule the timer to wake us up again in the
		     * next second to check it again. If the interface
		     * is really down, this timer should have been
		     * disabled (i.e., we should be here in that case).
		     */
		    mgd_timer_start(&nidb->sap_update_timer, ONESEC);
		}
	    }
	    break;
	    
	  default:
	    break;
	}
    }
}

/*
 * novell_supervisor
 *
 * Handle miscellaneous background functions including:
 *
 * - Scheduling of lost route advertisements
 * - Scheduling of full RIP and SAP updates
 * - Scheduling of partial RIP and SAP updates
 */
static forktype novell_supervisor (void)
{
    ulong major, minor;
    nidbtype *nidb;
    ulong wakeup_reasons;

    process_wait_on_system_init();
    process_set_priority(PRIO_LOW);

    /*
     * Build initial timer tree:
     */
    mgd_timer_init_leaf(&novell_atlr_timer, &novell_super_root_timer,
			IPX_TMR_ATLR, NULL, FALSE);
    
    process_watch_boolean(novell_rip_part_wb, ENABLE, RECURRING);
    process_watch_boolean(novell_sap_part_wb, ENABLE, RECURRING);
    process_watch_mgd_timer(&novell_super_root_timer, ENABLE);

    while (TRUE) {
	
	process_wait_for_event();

	while (process_get_wakeup(&major, &minor)) {

	    process_get_wakeup_reasons(&wakeup_reasons);

	    switch (major) {

	      case TIMER_EVENT:
		nov_super_timer_service();
		break;

	      case BOOLEAN_EVENT:
		/*
		 * Process triggered updates if pending:
		 */
		if (process_get_boolean(novell_rip_part_wb)) {
		    process_set_boolean(novell_rip_part_wb, FALSE);
		    FOR_ALL_NIDBS(nidb) {
			novell_posit_partial_rip_update(nidb);
		    }
		}
		if (process_get_boolean(novell_sap_part_wb)) {
		    process_set_boolean(novell_sap_part_wb, FALSE);
		    FOR_ALL_NIDBS(nidb) {
			novell_posit_partial_sap_update(nidb);
		    }
		}

		break;

	      default:
		break;
		
	    }
	    process_may_suspend();
	}
    }
}

static void nov_process_next_sap_update (nov_usv *usv)
{
    novell_sapentrytype *sap;
    boolean debug = (novell_sapupd_debug && novell_verbose_debug);

    if (debug) {
	buginf("\nIPXSAP: Update for %s:%x (%s)",
	       usv->nidb->idb->namestring,
	       usv->nidb->novellnet,
	       (usv->full == IPX_FULL_NEEDED) ? "FULL" : "PARTIAL");
	if (usv->linkdown_poison) buginf(" (LINKDOWN)");
	if (usv->reverse_poison)  buginf(" (REVERSE)");
    }

    if (!usv->sap_request) {
	return;
    }

    /*
     * If this is a new update that has been sitting in the timer
     * queues (e.g., during initial 55 mSec holddown), insert
     * placeholder at the beginning of the appropriate queue.
     * Also, set time of last update appropriately for usv->adv_sap().
     */
    if (!usv->placeheld) {
	if (debug) buginf(" (RESET PH)");
	sap = nov_get_first_sap();
	if (sap != NULL) {
	    nov_insert_sap_placeholder(sap, usv->sap_placeholder);
	    usv->placeheld = TRUE;
	    
	    /*
	     * Partial and reverse poison updates need advertise
	     * only to the point of the last advertised change.
	     * Record this time in usv->lastupdate.
	     */
	    if (usv->dest == IPX_DESTINATION_BROADCAST) {
		if (usv->reverse_poison) {
		    COPY_TIMESTAMP(usv->nidb->sap_revp_lastupdate,
				   usv->lastupdate);
		} else {
		    COPY_TIMESTAMP(usv->nidb->sap_lastupdate,
				   usv->lastupdate);
		}
	    }
	    /*
	     * Note the start time of this update. When the current update
	     * completes, this will become the new timestamp describing what
	     * we have advertised. Anything newer than this will again need
	     * to be advertised.
	     */
	    nov_get_timestamp(&usv->sap_start_timestamp);
	} else {
	    if (debug) buginf(" (CANCEL)");
	}
    }
    /*
     * Build and send next packet:
     */
    if (usv->placeheld && nov_send_sap_update_packet(usv)) {
	mgd_timer_start(&usv->timer, usv->gap);
    } else {

	/*
	 * Update complete ... begin termination processing.
	 */
	if (debug) buginf(", sent %d packet(s)", usv->sap_update_total);

	/*
	 * Record time of last update but ONLY if successful:
	 */
	if ((usv->error_occured == FALSE) &&
	    (usv->dest == IPX_DESTINATION_BROADCAST)) {
	    if (usv->reverse_poison) {
		COPY_TIMESTAMP(usv->sap_start_timestamp,
			       usv->nidb->sap_revp_lastupdate);
	    } else {
		COPY_TIMESTAMP(usv->sap_start_timestamp,
			       usv->nidb->sap_lastupdate);
	    }
	}

	/*
	 * If a link is going down let the supervisor process
	 * know ... further link level action may be required.
	 */
	if (usv->linkdown_poison || usv->reverse_poison) {
	    process_set_boolean(novell_sent_down_wb, TRUE);
	}

	/*
	 * Make snapshot happy:
	 */
	if (usv->sent_an_update == FALSE) {
	    novell_possibly_send_snapshot_null_updates(usv->nidb);
	}

	/*
	 * Deallocate UNICAST USV here and now. Other USV's are permanent.
	 */
	if (usv->dest != IPX_DESTINATION_BROADCAST) {
	    if (debug) buginf(" (DEALLOC UCAST)");
	    nov_deallocate_usv(usv);
	    return;
	}

	/*
	 * Reset update descriptor and check pending state. Automatically
	 * resubmit request if pending flag was set. Note that this resets
	 * the nidb->state copy to something current for the duration of
	 * the new request.
	 *
	 * Also note that reset_usv will deallocate any unused packet
	 * buffer allocated for this USV during update processing!
	 */
	if (nov_reset_usv(usv)) {
	    if (debug) buginf("(RESUBMIT)");
	    mgd_timer_start(&usv->timer, usv->gap);
	    usv->active = TRUE;
	} else {
	    if (debug) buginf("(IDLE)");
	}
    }
}


static forktype novell_sap_update_daemon (void)
{
    mgd_timer *expired_timer;
    ulong major, minor;
    nov_usv *usv = NULL;
    
    process_wait_on_system_init();
    process_set_priority(PRIO_LOW);

    process_watch_mgd_timer(&novell_sap_delay_timer, ENABLE);

    while (TRUE) {
	
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    
	    switch (major) {
	      case TIMER_EVENT:
		while (mgd_timer_expired(&novell_sap_delay_timer)) {
		    expired_timer =
			mgd_timer_first_expired(&novell_sap_delay_timer);
		    usv = (nov_usv *)mgd_timer_context(expired_timer);
		    mgd_timer_stop(expired_timer);
		    nov_process_next_sap_update(usv);
		    process_may_suspend();
		}
		break;
		
	      default:
		break;
	    }
	    process_may_suspend();
	}
    }
}

static void nov_process_next_rip_update (nov_usv *usv)
{
    novndbtype *ndb;
    boolean debug = (novell_ripupd_debug && novell_verbose_debug);

    if (debug) {
	buginf("\nIPXRIP: Update for %s:%x (%s)",
	       usv->nidb->idb->namestring,
	       usv->nidb->novellnet,
	       (usv->full == IPX_FULL_NEEDED) ? "FULL" : "PARTIAL");
	if (usv->linkdown_poison) {
	    buginf("(LINKDOWN)");
	}
	if (usv->reverse_poison) {
	    buginf("(REVERSE)");
	}
    }

    if (!usv->rip_request) {
	return;
    }

    /*
     * If this is a new update that has been sitting in the timer
     * queues (e.g., during initial 55 mSec holddown), insert
     * placeholder at the beginning of the appropriate queue.
     * Also, set time of last update appropriately.
     */
    if (!usv->placeheld) {
	if (debug) buginf("(RESET PH)");

	ndb = nov_get_first_ndb();

	if (ndb != NULL) {

	    nov_insert_ndb_placeholder(ndb, usv->ndb_placeholder);
	    usv->placeheld = TRUE;
	    
	    /*
	     * Partial and reverse poison updates need advertise
	     * only to the point of the last advertised change.
	     * Record this time in usv->lastupdate. Advertise
	     * from the beginning of the list to this point.
	     */
	    if (usv->dest == IPX_DESTINATION_BROADCAST) {
		if (usv->reverse_poison) {
		    COPY_TIMESTAMP(usv->nidb->rip_revp_lastupdate,
				   usv->lastupdate);
		} else {
		    COPY_TIMESTAMP(usv->nidb->rip_lastupdate,
				   usv->lastupdate);
		}
	    }
	    GET_TIMESTAMP(usv->rip_start_timestamp);
	} else {
	    if (debug) buginf("(CANCEL)");
	}
    }

    /*
     * Build and send next packet:
     */
    if (usv->placeheld && nov_send_rip_update_packet(usv)) {
	mgd_timer_start(&usv->timer, usv->gap);
    } else {
	/*
	 * Update complete ... begin termination processing.
	 */
	if (debug) buginf(", sent %d packet(s)", usv->rip_update_total);

	/*
	 * Record time of last update but ONLY if successful:
	 */
	if ((usv->error_occured == FALSE) &&
	    (usv->dest == IPX_DESTINATION_BROADCAST)) {
	    if (usv->reverse_poison) {
		COPY_TIMESTAMP(usv->rip_start_timestamp,
			       usv->nidb->rip_revp_lastupdate);
	    } else {
		COPY_TIMESTAMP(usv->rip_start_timestamp,
			       usv->nidb->rip_lastupdate);
	    }
	}

	/*
	 * If a link is going down let the supervisor process
	 * know ... further link level action may be required.
	 */
	if (usv->linkdown_poison || usv->reverse_poison) {
	    process_set_boolean(novell_sent_down_wb, TRUE);
	}

	/*
	 * Make snapshot happy:
	 */
	if (usv->sent_an_update == FALSE) {
	    novell_possibly_send_snapshot_null_updates(usv->nidb);
	}

	/*
	 * Deallocate UNICAST USV here and now. Other USV's are permanent.
	 */
	if (usv->dest != IPX_DESTINATION_BROADCAST) {
	    nov_deallocate_usv(usv);
	    return;
	}

	/*
	 * Fire up a periodic SAP broadcast following RIP completion,
	 * if so configured.
	 */
	if ((usv == usv->nidb->rip_full) && usv->nidb->novell_sap_after_rip) {
	    if (debug) buginf("(POSIT SAP)");
	    novell_posit_full_sap_update(usv->nidb);
	}

	/*
	 * Reset update descriptor and check pending state. Automatically
	 * resubmit request if pending flag was set. Note that this resets
	 * the nidb->state copy to something current for the duration of
	 * the new request.
	 *
	 * Also note that reset_usv will deallocate any unused packet
	 * buffer allocated for this USV during update processing!
	 */
	if (nov_reset_usv(usv)) {
	    if (debug) buginf("(RESUBMIT)");
	    mgd_timer_start(&usv->timer, usv->gap);
	    usv->active = TRUE;
	} else {
	    if (debug) buginf("(IDLE)");
	}
    }
}


static forktype novell_rip_update_daemon (void)
{
    mgd_timer *expired_timer;
    ulong major, minor;
    nov_usv *usv = NULL;
    
    process_wait_on_system_init();
    process_set_priority(PRIO_LOW);

    process_watch_mgd_timer(&novell_rip_delay_timer, ENABLE);

    while (TRUE) {
	
	process_wait_for_event();
	
	while (process_get_wakeup(&major, &minor)) {
	    
	    switch (major) {
	      case TIMER_EVENT:
		while (mgd_timer_expired(&novell_rip_delay_timer)) {
		    expired_timer =
			mgd_timer_first_expired(&novell_rip_delay_timer);
		    usv = (nov_usv *)mgd_timer_context(expired_timer);
		    mgd_timer_stop(expired_timer);
		    nov_process_next_rip_update(usv);
		    process_may_suspend();
		}
		break;
		
	      default:
		break;
	    }
	    process_may_suspend();
	}
    }
}

static void nov_net_down_handler_linkdown (void)
{
    nidbtype *nidb;
    boolean sending_poisons;
    boolean update_posited;
    novndbtype *ndb, *next_ndb;
    novell_sapentrytype *sap, *next_sap;
    novell_sapentrytype *sap_ph;
    novndbtype *ndb_ph;
    boolean debug = (novellrip_debug || novellrip_event_debug ||
		     novellsap_debug || novellsap_event_debug ||
		     novell_ripupd_debug || novell_sapupd_debug);

    /*
     * Determine whether any new networks are going down. Mark
     * them appropriately. Update nidb state to "xxx_executing" if
     * this is user directed, as opposed to just flapping down
     * spontaneously.
     */

    sending_poisons = FALSE;
    FOR_ALL_NIDBS(nidb) {
	if (!nidb->doing_linkdown_poisons) {
	    if ((NOVELL_NIDB_DORD_PENDING(nidb)) ||
		NOV_LINKDOWN_POISONS_NEEDED(nidb)) {
		sending_poisons = TRUE;
		if (debug)
		    buginf("\nIPX: Poisoning %x/%s because %s",
			   nidb->novellnet,
			   nidb->idb->namestring,
			   NOVELL_NIDB_DORD_PENDING(nidb) ?
			   "down/delete pending" : "interface down");
		nidb->doing_linkdown_poisons = TRUE;
		if (debug) {
		    buginf("\nIPX: State of itf:net %s:%x is %s:%s",
			   nidb->idb->short_namestring, nidb->novellnet,
			   (interface_up(nidb->idb) ? "[up]" : "[down]"),
			   novell_nidb_print_state(nidb));
		}
		if (nidb->state == NOVELL_NIDB_DOWN_PENDING) {
		    novell_nidb_newstate
			(nidb, NOVELL_NIDB_DOWN_EXECUTING);
		}
		if (nidb->state == NOVELL_NIDB_DELETE_PENDING) {
		    novell_nidb_newstate
			(nidb, NOVELL_NIDB_DELETE_EXECUTING);
		}
	    }
	}
    }

    /*
     * If something is going down, mark all other up networks
     * to do a reverse poison.
     */
    if (sending_poisons) {

	/*
	 * Mark networks for reverse poisons as appropriate:
	 */
	FOR_ALL_NIDBS(nidb) {
	    if (NOVELL_NIDB_UPDATES_ALLOWED(nidb) && interface_up(nidb->idb)) {
		nidb->doing_reverse_poisons = TRUE;
	    }
	}

	/*
	 * Identify all routes and services which are no longer reachable
	 * via an up interface, and which were not identified as such
	 * by a previous linkdown event. (These must be older than the
	 * last linkdown event.) Do NOT posit a partial update. Simply
	 * mark these as changed so that the reverse poison process can
	 * find them optimally. Note that any partial update which happens
	 * to follow will skip these since they are no longer reachable
	 * via an "up" interface.
	 */

	ndb = nov_get_first_ndb();
	ndb_ph = nov_ndb_malloc_placeholder();

	while (ndb != NULL) {
	    /*
	     * If we have a placeholder, and it's time to suspend,
	     * then do so, to avoid hogging the processor:
	     */
	    if ((ndb_ph != NULL) && process_time_exceeded()) {
		nov_insert_ndb_placeholder(ndb, ndb_ph);
		process_suspend();
		ndb = nov_delete_ndb_placeholder(ndb_ph);
	    }
	    /*
	     * Remember next entry in case we promote this one to
	     * to newest in order to poison it.
	     */
	    next_ndb = nov_get_next_ndb(ndb);

	    /*
	     * This entry needs to be reverse poisoned if it no longer
	     * has any up paths. To do so, set flag and promote to newest.
	     */
	    if (!ndb->reverse_poisoned &&
		((ndb->path_list != NULL) &&
		 !novrip_interface_up(ndb->path_list)))
		{
		    ndb->reverse_poisoned = TRUE;
		    novell_mark_newest(ndb);
		}
	    ndb = next_ndb;
	}

	if (ndb_ph != NULL) {
	    nov_delete_ndb_placeholder(ndb_ph);
	    free(ndb_ph);
	}

	/*
	 * Now look for services that need reverse poison:
	 */
	sap = nov_get_first_sap();
	sap_ph = nov_sap_malloc_placeholder();

	while (sap != NULL) {

	    /*
	     * If we have a placeholder, and it's time to suspend,
	     * then do so, to avoid hogging the processor:
	     */
	    if ((sap_ph != NULL) && process_time_exceeded()) {
		nov_insert_sap_placeholder(sap, sap_ph);
		process_suspend();
		sap = nov_delete_sap_placeholder(sap_ph);
	    }

	    next_sap = nov_get_next_sap(sap);

	    /*
	     * This entry needs to be reverse poisoned if it no longer
	     * has any up paths. To do so, set flag and promote to newest.
	     */
	    if (!sap->reverse_poisoned && (!nov_any_idb_up(sap)))
		{
		    sap->reverse_poisoned = TRUE;
		    nov_mark_sap_newest(sap);
		}
	    sap = next_sap;
	}

	if (sap_ph != NULL) {
	    nov_delete_sap_placeholder(sap_ph);
	    free(sap_ph);
	}

	/*
	 * Posit any and all linkdown and resulting reverse
	 * poison update that may be pending:
	 */
	update_posited = FALSE;
	FOR_ALL_NIDBS(nidb) {
	    /*
	     * Do the posit ... note that these look like full
	     * updates but are modified by the nidb flags and
	     * become either linkdown or reverse poisons.
	     */
	    if (nidb->doing_reverse_poisons) {

		update_posited |=
		    novell_posit_reverse_rip_update(nidb);

		update_posited |=
		    novell_posit_reverse_sap_update(nidb);
		
		nidb->doing_reverse_poisons = FALSE;
	    }

	    if (nidb->doing_linkdown_poisons) {

		update_posited |=
		    novell_posit_linkdown_rip_update(nidb);

		update_posited |=
		    novell_posit_linkdown_sap_update(nidb);
	    }
	}
	/*
	 * If no updates were posited (e.g., this is the last
	 * active interface), signal completion directly.
	 */
	if (!update_posited) {
	    process_set_boolean(novell_sent_down_wb, TRUE);
	}
    }
}

static void nov_net_down_handler_completion (void)
{
    nidbtype *nidb;
    boolean debug = (novellrip_debug || novellrip_event_debug ||
		     novellsap_debug || novellsap_event_debug ||
		     novell_ripupd_debug || novell_sapupd_debug);

    /*
     * For now do something simple, suboptimal, but robust ...
     * make sure that no linkdown or reverse poisons are extant:
     */
    FOR_ALL_NIDBS(nidb) {
	if (nidb->rip_down->active || nidb->rip_revp->active ||
	    nidb->sap_down->active || nidb->sap_revp->active)
	    {
		return;
	    }
    }

    /*
     * OK, at this point we know that all linkdown and reverse poisons
     * have been sent. Bring down all networks with linkdown pending.
     * Note that multiple linkdown events may have taken place in the
     * recent past. If we get here, we know that ALL of them have completed
     * and that ALL appropriate networks and servers have been poisoned.
     */
    FOR_ALL_NIDBS(nidb) {
	if (nidb->doing_linkdown_poisons) {
	    if (debug) {
		buginf("\nIPX: Bringdown %s:%x",
		       nidb->idb->namestring, nidb->novellnet);
	    }
	    nidb->doing_linkdown_poisons = FALSE;
	    if (NOV_LINKDOWN_POISONS_NEEDED(nidb)) {
		nidb->nov_sent_linkdown_poisons = TRUE;
		reg_invoke_novigrp_route_adjust(NULL, nidb->idb);
		nov_kill_connected_route(nidb);
	    }
	}
	if (NOVELL_NIDB_DORD_EXECUTING(nidb)) {	
	    nidb->idb->novell_deleted_nidb = TRUE;
	    novell_nidb_newstate
		(nidb, (nidb->state == NOVELL_NIDB_DELETE_EXECUTING) ?
		 NOVELL_NIDB_DELETED : NOVELL_NIDB_DOWN);
	    if (nidb->idb != NULL) {
		if (nidb->idb->hwptr != NULL) {
		    nov_fastsetup(nidb->idb->hwptr);
		}
		nidb->idb->novell_deleted_nidb = FALSE;
	    }
	}
    }
}


static forktype novell_net_down_handler (void)
{
    ulong major, minor;

    process_wait_on_system_init();

    process_watch_boolean(novell_link_down_wb, ENABLE, RECURRING);
    process_watch_boolean(novell_sent_down_wb, ENABLE, RECURRING);

    while (TRUE) {

	process_wait_for_event();
	process_set_priority(PRIO_LOW);
	
	while (process_get_wakeup(&major, &minor)) {
	    
	    switch (major) {
		
	      case BOOLEAN_EVENT:
		
		if (process_get_boolean(novell_link_down_wb)) {
		    process_set_boolean(novell_link_down_wb, FALSE);
		    nov_net_down_handler_linkdown();
		} else if (process_get_boolean(novell_sent_down_wb)) {
		    process_set_boolean(novell_sent_down_wb, FALSE);
		    nov_net_down_handler_completion();
		}
		break;
		
	      default:
		break;
	    }
	    process_may_suspend();
	}
    } /* while (TRUE) */
}

/*
 * novell_posit_sap_update
 *
 * Indicate the need to have a SAP update on the given NIDB.
 */
boolean novell_posit_sap_update (nidbtype *nidb,
				 ulong net,
				 uchar const *host,
				 ushort sock,
				 enum IPX_FULL fullupdate,
				 enum IPX_DESTINATION destination,
				 ushort servertype,
				 boolean reverse_poison,
				 boolean linkdown_poison)
{
    boolean vdebug = novell_verbose_debug;

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
        return(FALSE);
    }

    /*
     * Skip if SAP compatibility not enabled and general update: 
     */
    if ((servertype == NOVELL_EVERYONE) &&
	((nidb->nov_lsp_enabled && !nidb->lsp_sap_on) ||
	 (nov_ipxwan_on_nidb(nidb) && !nidb->nov_ipxwan_need_ripsap))) {
	
	if (destination == IPX_DESTINATION_BROADCAST) { 
	    nov_get_timestamp(&nidb->sap_lastupdate);
	    nidb->sap_revp_lastupdate = nidb->sap_lastupdate;
	}

	if (novellsap_debug || novellsap_event_debug) {
	    buginf("\nIPXSAP: SAP compatibility mode off,"
	           " skip positing update to %s", nidb->idb->namestring);
	}
	return(FALSE);
    }

    /*
     * Internal NIDBs should be marked as having a SAP update sent, but
     * do nothing.
     */
    if (IPX_NIDB_IS_INTERNAL(nidb)) {
	nov_get_timestamp(&nidb->sap_lastupdate);
	nidb->sap_revp_lastupdate = nidb->sap_lastupdate;
	return(FALSE);
    }

    if (novellsap_debug || novellsap_event_debug) {
	buginf("\nIPXSAP: positing update to %x.%e via %s %s %s",
	       net, host, nidb->idb->namestring,
	       (destination == IPX_DESTINATION_BROADCAST) ? "(broadcast)" : "",
	       (fullupdate == IPX_FULL_NEEDED) ? "(full)" : "(flash)");
	if (vdebug) bugtrace();
    }

    return (nov_schedule_update(IPX_UPD_SAP,
				nidb,
				host,
				sock,
				fullupdate,
				destination,
				servertype,
				0, NULL, 0, 0,
				reverse_poison, linkdown_poison));
}

/*
 * novell_posit_sap_query_response
 *
 * Handle response to SAP query
 */
boolean novell_posit_sap_query_response (nidbtype *nidb, ulong net,
					 uchar const *host, ushort sock,
					 enum IPX_FULL fullupdate,
					 enum IPX_DESTINATION destination,
					 ushort servertype,
					 ushort query_type,
					 uchar *server_name, /* %%% */
					 ulong dnet,
					 ulong dmask)
{
    boolean vdebug = novell_verbose_debug;

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
        return(FALSE);
    }

    /*
     * Note well that at the present time we can only generate a unicast
     * response. To do otherwise will require extensive changes.
     */
    if (destination != IPX_DESTINATION_UNICAST) { 
	return(FALSE);
    }

    /*
     * Skip if SAP compatibility not enabled and general query:
     */
    if ((servertype == NOVELL_EVERYONE) && (nidb->nov_lsp_enabled) && 
	(!nidb->lsp_sap_on) && (!nidb->nov_ipxwan_need_ripsap) && 
	(!NOVIGRP_DOING_INCREMENTAL_SAP(nidb))) { 
	if (destination == IPX_DESTINATION_BROADCAST) { 
	    ;
	} else { 
	    if (novellsap_debug || novellsap_event_debug) 
		buginf("\nIPXSAP: NLSP-SAP compatibility mode off, ignored)");
	} 
	return(FALSE);
    }

    /*
     * Ignore internal NIDB:
     */
    if (IPX_NIDB_IS_INTERNAL(nidb)) {
	return(FALSE);
    }

    if (novellsap_debug || novellsap_event_debug) {
	buginf("\nIPXSAP: positing query response to %x.%e via %s %s %s",
	       net, host, nidb->idb->namestring,
	       (destination == IPX_DESTINATION_BROADCAST) ? "(broadcast)" : "",
	       (fullupdate == IPX_FULL_NEEDED) ? "(full)" : "(flash)");
	if (vdebug) bugtrace();
    }

    return (nov_schedule_update(IPX_UPD_SAP,
				nidb,
				host,
				sock,
				fullupdate,
				destination,
				servertype,
				query_type,
				server_name,
				dnet,
				dmask,
				FALSE, FALSE));
}

/*
 * novell_posit_rip_update
 *
 * Screen and submit a rip update request if appropriate. Reasons why
 * a request might not be appropriate include duplicate requests, or
 * requests for updates when RIP is not configured or active.
 *
 * Note that ALL RIP update requests should be made via this routine.
 */
boolean novell_posit_rip_update (nidbtype *nidb,
				 ulong net,
				 uchar const *host,
				 ushort sock,
				 enum IPX_FULL fullupdate,
				 enum IPX_DESTINATION destination,
				 boolean reverse_poison,
				 boolean linkdown_poison)
{
    boolean vdebug = novell_verbose_debug;

    if ((nidb == NULL) || (!interface_up(nidb->idb))) {
        return(FALSE);
    }
    /*
     * Skip if RIP compatibility not enabled:
     */
    if ((nidb->nov_lsp_enabled && !nidb->lsp_rip_on) ||
	(nov_ipxwan_on_nidb(nidb) && !nidb->nov_ipxwan_need_ripsap)) {
	if (destination == IPX_DESTINATION_BROADCAST) { 
	    GET_TIMESTAMP(nidb->rip_lastupdate); 
	    GET_TIMESTAMP(nidb->rip_revp_lastupdate);
	}
	if (novellrip_debug || novellrip_event_debug) 
	    buginf("\nIPXRIP: RIP compatibility mode off,"
	           " skip positing update to %s", nidb->idb->namestring);
	return(FALSE);
    }
    /*
     * If this nidb is not listed in the router subcommand "network", then
     * do not send RIP updates out this interface. All novell nets on the
     * router should be listed in novrip_pdb.networks[] by default.
     */
    if (!novell_rip_network_enable(nidb)) {
	if (novellrip_debug || novellrip_event_debug)
	    buginf("\nIPXRIP: RIP turned off, skip positing update to %s",
		   nidb->idb->namestring);
	return(FALSE);
    }

    /*
     * Internal NIDBs should be marked as having a RIP update sent, but
     * do nothing. Same thing if the nidb has RIP processing disabled.
     * Finally, The same is true of NIDBs that only advertise
     * the default route, if this is a 'general multi-nidb loop'
     * (that is, if nidb == NULL).
     */
    if (IPX_NIDB_IS_INTERNAL(nidb) || !novell_rip_network_enable(nidb)) {
	GET_TIMESTAMP(nidb->rip_lastupdate);
	GET_TIMESTAMP(nidb->rip_revp_lastupdate);
	return(FALSE);
    }
    
    /* Build special 'default route only' table, or a full/partial table */
    if (nidb->adv_default_route_only) {
	if (novell_default_ndb == NULL) {
	    GET_TIMESTAMP(nidb->rip_lastupdate);
	    GET_TIMESTAMP(nidb->rip_revp_lastupdate);
	    return(FALSE);
	}
    }
    
    if (novellrip_debug || novellrip_event_debug) {
	buginf("\nIPXRIP: positing %s update to %x.%e via %s %s",
	       (fullupdate == IPX_FULL_NEEDED) ? "full" : "flash",
	       net, host, nidb->idb->namestring,
	       (destination == IPX_DESTINATION_BROADCAST) ?
	       "(broadcast)" : "(unicast)");
	if (vdebug) bugtrace();
    }

    return (nov_schedule_update(IPX_UPD_RIP,
				nidb,
				host,
				sock,
				fullupdate,
				destination,
				0, 0, NULL, 0, 0,
				reverse_poison, linkdown_poison));
}

/*
 * nov_good_query_packet
 *
 * Perform input packet screening common to general and nearest services query
 *
 * Note that a TRUE result indicates that the packet is acceptable. 
 * A FALSE result if the packet is malformed.
 *
 * This is also the place that puts the local network number in the packet
 * if the received network number was 0.
 *
 */
boolean nov_good_query_packet (
     paktype *pak,
     novell_querytype *query)
{
    ipxhdrtype *ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    /* traditional queries are always a specific size. The new
     * queries can be almost any length.
     */
    if ((GETSHORT(&ipx->len) != NOVELL_QUERYLENGTH) && 
	((GETSHORT(&query->query_type) == NOVELL_SAP_GENQUERY) ||
	 (GETSHORT(&query->query_type) == NOVELL_SAP_NEARQUERY))) {
	if (novellsap_debug || novellsap_event_debug)
	    buginf("\nIPXSAP: query with bad length %d from %x.%e",
		   GETSHORT(&ipx->len), GET2WD(ipx->ssnet), ipx->shost);
	novell_stat[NOVELL_SAPFMTERROR]++;
	return(FALSE);
    }

    if (GET2WD(ipx->ssnet) == 0)
        PUT2WD(ipx->ssnet, ((nidbtype *) pak->nif_input)->novellnet);
    if (novellsap_debug || novellsap_event_debug)
      buginf("\nIPX: I SAP query type %d for server type %#x from %x.%e",
	     GETSHORT(&query->query_type), GETSHORT(&query->server_type),
	     GET2WD(ipx->ssnet), ipx->shost);
    return(TRUE);
}

/*
 * novell_rcv_query
 *
 * Having received a services query, verify that the packet is good and
 * post a full sap update to the appropriate nidb with the corresponding
 * server type.
 *
 */
void novell_rcv_query (paktype *pak, novell_querytype *query)
{
    ipxhdrtype *ipx;

    if (!nov_good_query_packet(pak,query))
	return;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    novell_posit_sap_update((nidbtype *) pak->nif_input,
			    (GET2WD(ipx->ssnet) ?
			     GET2WD(ipx->ssnet) :
			     ((nidbtype *)pak->nif_input)->novellnet),
			    ipx->shost,
			    ipx->ssock,
			    IPX_FULL_NEEDED,
			    IPX_DESTINATION_UNICAST,
			    query->server_type,
			    FALSE, FALSE);
}

/*
 * novell_rcv_name_query
 *
 * Handle a request for a specific server or servers:
 *
 */
void novell_rcv_name_query (paktype *pak, novell_querynametype *query)
{
    ipxhdrtype *ipx;
    int size, i;
    nidbtype *nidb;

    if (!nov_good_query_packet(pak,(novell_querytype *) query))
 	return;

    nidb = (nidbtype *)pak->nif_input;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    size = ipx->len - (XNSOVERHEADBYTES + 2);

    for (i = 0; i < size/(sizeof(novell_servernametype)); i++) {
	novell_posit_sap_query_response(nidb,
					(GET2WD(ipx->ssnet) ?
					 GET2WD(ipx->ssnet) :
					 nidb->novellnet),
					ipx->shost,
					ipx->ssock,
					IPX_FULL_NEEDED,
					IPX_DESTINATION_UNICAST,
					query->server[i].server_type,
					query->query_type,
					(uchar*)&query->server[i].server_name,
					query->server[i].dnet,
					query->server[i].dmask);
    }
}

/* 
 * novell_sort_sap_list
 *
 * Sort a Novell SAP list. This implements 'heapsort'.
 *
 * Routing protocol administrative distances for metric tie-breakers are
 * unimportant here. Therefore, they do NOT play a part, and therefore
 * in THIS regard, the comparison functions differ from
 * nov_sap_distance_compare...
 */
novell_sapentrytype **novell_sap_heap;

inline static boolean novell_sap_compare_lt (novell_sapentrytype *a,
					     novell_sapentrytype *b)
{
    /*
     * Router certification uses 'route ticks' first, then route hops.
     */
    if (novell_sap_uses_routing) {
	/* If different protocols, don't bother to try to compare metrics */
	if ((a->route_flags & (NET_IGRP | NET_RIP)) !=
	    (b->route_flags & (NET_IGRP | NET_RIP))) {
	    if (a->route_hops < b->route_hops) {
		return(TRUE);
	    } else if (a->route_hops == b->route_hops) {
		if (a->server.nethops < b->server.nethops) {
		    return(TRUE);
		}
	    }
	} else if (a->route_delay < b->route_delay) {
	    return(TRUE);
	} else if (a->route_delay == b->route_delay) {
	    if (a->route_hops < b->route_hops) {
		return(TRUE);
	    }
	}			 
    } else {
	/* Otherwise, we just look at SAP hops */
	if (a->server.nethops < b->server.nethops) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

inline static boolean novell_sap_compare_eq (novell_sapentrytype *a,
					     novell_sapentrytype *b)
{
    if (((novell_sap_uses_routing) &&
	 ((((a->route_flags & (NET_IGRP | NET_RIP)) ==
	    (b->route_flags & (NET_IGRP | NET_RIP))) &&
	   (a->route_delay != b->route_delay)) ||
	  (a->route_hops != b->route_hops)))) {
	return(FALSE);
    }
    return(TRUE);
}
						       

inline static boolean novell_sort_sap_mustmove (int x, int size)
{
    if ((2*x) <= size) {
	if (novell_sap_compare_lt(novell_sap_heap[x],
				  novell_sap_heap[2*x]))
	  return(TRUE);
    }
    if ((2*x + 1) <= size) {
	if (novell_sap_compare_lt(novell_sap_heap[x],
				  novell_sap_heap[2*x + 1]))
	  return(TRUE);
    }
    return(FALSE);
}

inline static int novell_sort_sap_select (int x, int size)
{
    if (2*x < size) {
	if (novell_sap_compare_lt(novell_sap_heap[2*x],
				  novell_sap_heap[2*x+1]))
	  return(2*x + 1);
    }
    return(2*x);
}

inline static void novell_sort_sap_enheap (int x, int size)
{
    int y;
    novell_sapentrytype *t;

    while (novell_sort_sap_mustmove(x, size)) {
	y = novell_sort_sap_select(x, size);
	t = novell_sap_heap[x];
	novell_sap_heap[x] = novell_sap_heap[y];
	novell_sap_heap[y] = t;
	x = y;
    }
}
static void novell_sort_sap_list (novell_sapqueuetype *sapq)
{
    int z, size;
    novell_sapentrytype *entry;
    
    /* Sanity check... */
    if ((sapq == NULL) || (sapq->sorted))
      return;
    
    /* Count the saps in the sapq */
    size = 0;
    FOR_ALL_SAPS_OF_TYPE(entry, sapq) {
	size++;
    }

    /* Don't be silly... */
    if (size <= 1) {
	sapq->sorted = TRUE;
	return;
    }

    /* Start the array at 1 .. makes the HEAP code easier... */
    novell_sap_heap = malloc((size + 1) * sizeof(uchar *));
    if (novell_sap_heap == NULL) {
	return;
    }
    
    /* Okay... make an array of pointers (the original list) */
    z = 1;
    FOR_ALL_SAPS_OF_TYPE(entry, sapq) {
	novell_sap_heap[z] = entry;
	z++;
    }

    /* Now, build the novell_sap_heap... */
    for (z = size/2; z > 0; z--) {
	novell_sort_sap_enheap(z, size);
    }
    for (z = size; z > 1; z--) {
	entry = novell_sap_heap[1];
	novell_sap_heap[1] = novell_sap_heap[z];
	novell_sap_heap[z] = entry;
	novell_sort_sap_enheap(1, (z - 1));
    }

    /* Done. Now build the new list */

    /* Restart the old queue */
    sapq->saps.flink = &sapq->saps;
    sapq->saps.blink = &sapq->saps;
    entry = (novell_sapentrytype *) sapq->saps.flink;
    for (z = 1; z <= size; z++) {
 	lw_insert(&novell_sap_heap[z]->saps, entry->saps.blink);
    }
    sapq->sorted = TRUE;
    free(novell_sap_heap);
}

/*
 * novell_sort_all_saps
 *
 * Make sure all server lists are sorted ... count the total number of servers
 * as a side effect.
 */
static int novell_sort_all_saps(void)
{
    novell_sapqueuetype *sapq;

    FOR_ALL_SAP_TYPES(sapq) {
	novell_sort_sap_list(sapq);
	process_suspend();
    }
    return(novell_server_count);
}

/*
 * novell_nsokay
 *
 * Return TRUE if this server 'entry' is okay to use in a GNS reply
 * to 'pak'
 */
static boolean novell_nsokay (novell_sapqueuetype *sapq, novell_sapentrytype *entry,
		       paktype *pak, boolean *split_happened)
{
    *split_happened = FALSE;

    if (entry->paths == NULL) {
	return(FALSE);
    }
	    
    /* Make sure this entry is up-to-date: */
    if (novell_sapentry_poisoned(sapq, entry, NULL)) {
	return(FALSE);
    }

    /* Sanity check: Can we get to this server? */
    /* This is 'new' - only do this if novell_sap_uses_routing is enabled */
    if (novell_sap_uses_routing && (entry->route_delay == METRIC_INACCESSIBLE)) {
	return(FALSE);
    }

    /* Skip if poisoned or output interface is not up... */
    if ((entry->poisoned) || !nov_any_idb_up(entry)) {
	return(FALSE);
    }
	    
    /* Bail if server not within range of requestor: */
    if ((entry->server.nethops + 1) >= novell_infinity) {
	return(FALSE);
    }

    /* Do we want to say anything about this service ? */
    if (!nov_sapcheck((novell_serveridtype *)&entry->server, 
		      entry->server.server_type, 
		      pak->if_input->nov_sap_outfilterlist)) {
	return(FALSE);
    }

    /* Do we want to say anything about this "nearest" service ? */
    if (!nov_sapcheck((novell_serveridtype *)&entry->server, 
		      entry->server.server_type, 
		      pak->if_input->nov_gns_outfilterlist)) {
	return(FALSE);
    }

    /* Bail if split horizon fails: */
    if (novell_sap_split_horizon(entry, pak->nif_input,
				 IPX_ADVERTISING_NO)) {
	*split_happened = TRUE;
	return(FALSE);
    }

    return(TRUE);
}

/*
 * novell_nearnamequery
 * 
 * process a near name query packet after it has been dequeued from GNS queue
 * (this started out in life as a copy of the GNS Query handling and
 * much of this code still resembles that code).
 *
 */
static void novell_nearnamequery (paktype *pak)
{
    paktype *reply = NULL;
    ipxhdrtype *ipx;
    novell_sappacket *sap = NULL;
    novell_sapentrytype *entry, *best, *scan, *lastscan;
    novell_sapqueuetype *sapq;
    novell_serveridtype *data;
    sys_timestamp starttime;
    novell_querynametype *query;
    int size, i, count;
    boolean split_happened, result, match;
    volatile ushort nethops;

    count = 0;    /* Count of found Services which match  queries */
    ipx = (ipxhdrtype *)ipxheadstart(pak);
    query = (novell_querynametype *)(XNSDATASTART(pak));
    size = ipx->len - (XNSOVERHEADBYTES + 2);

    for (i = 0; i < size/(sizeof(novell_servernametype)); i++) {

      if (GETSHORT(&query->server[i].server_type) == 0xffff) {
	if (novellsap_debug || novellsap_event_debug) {
	  buginf("\nIPX SAP: Invalid Nearest Name Service type %x, ignoring request.",
		 GETSHORT(&query->server[i].server_type));
	  continue;
	}
      }
      sapq = novell_findsapqueue(GETSHORT(&query->server[i].server_type),
				 IPX_SAP_QUEUE_LOOKUP_MUST_EXIST);

      /* No Need to do anything if we don't have that type */
      if (!sapq) {
	continue;
      }
      /*
       * Sort the SAP list if it needs it and it hasn't been done
       * in a while...
       */
      if (!sapq->sorted && !SLEEPING(sapq->next_sorted_time)) {
	GET_TIMESTAMP(starttime);
	novell_sort_sap_list(sapq);
	if (novellsap_debug) {
	  buginf("\nIPX SAP: Sort of SAP list %x took %u msec",
		 sapq->server_type, ELAPSED_TIME(starttime));
	}
	TIMER_START(sapq->next_sorted_time, NOVELL_SAP_SORT_INTERVAL);
      }
      match = FALSE;
      split_happened = FALSE;
      best = entry = scan = lastscan = NULL;
      /* If the list is sorted, life is simple... */
      if (sapq->sorted) {
	entry = (novell_sapentrytype*) &(sapq->saps.flink);
	while (((entry = (novell_sapentrytype *)entry->saps.flink) != (novell_sapentrytype *)&sapq->saps)) {
	  result = novell_nsokay(sapq, entry, pak, &split_happened);
	  if (split_happened) {
	    break;
	  }
	  if (!result) {
	    continue;
	  }
	  
	  /*
	   * We have found a valid entry. Continue searching the
	   * valid entries of this distance, doing a 'round robin'
	   * if requested, and also checking to see if another
	   * one of equal cost is hanging around that would want
	   * us to do split horizon
	   */
	  scan = entry;
	  do {
	    lastscan = scan;
	    /* Skip if not eligible for split horizon: */
	    if ((scan->poisoned) ||
		(scan->paths == NULL) ||
		(!nov_any_idb_up(scan)))
	      continue;
	    /* Skip if access checks fail */
	    if (nov_sapcheck((novell_serveridtype *) &scan->server, 
			     scan->server.server_type, 
			     pak->if_input->nov_sap_outfilterlist) &&
		nov_sapcheck((novell_serveridtype *) &scan->server, 
			     scan->server.server_type, 
			     pak->if_input->nov_gns_outfilterlist) &&
		nov_sap_name_check(scan, query->query_type,
				   query->server[i].server_type,
				   (char *) query->server[i].server_name,
				   query->server[i].dnet,
				   query->server[i].dmask)) {
	      /* A possible service. Bail if Split Horizon */
	      if (novell_sap_split_horizon
		  (scan, pak->nif_input, IPX_ADVERTISING_NO)) {
		split_happened = TRUE;
		break;
	      } else {
		/* Everything matches this must be the one */
		match = TRUE;
		best = scan;
		break;
	      }
	    }
	  } while (((scan = (novell_sapentrytype *)scan->saps.flink) !=
		    (novell_sapentrytype *)&sapq->saps) &&
		   (!novell_sap_compare_lt(lastscan, scan)));
	  /* Do round robin, if applicable */
	  if (!split_happened && nov_gns_roundrobin &&
	      entry != lastscan) {
	    lw_remove(&entry->saps); /* Unlink old entry */
	    lw_insert(&entry->saps, scan->saps.blink); /* Hook old last to it */
	  }
	  break;
	}
	entry = best;
      } else {
	/* The list is not sorted... Grr... */
	entry = (novell_sapentrytype*) &(sapq->saps.flink);
	best = NULL;
	while (((entry = (novell_sapentrytype *)entry->saps.flink) != (novell_sapentrytype *)&sapq->saps)) {
	  result = novell_nsokay(sapq, entry, pak, &split_happened);
	  if (!result) {
	    continue;
	  }
	  if ((best == NULL) || (novell_sap_compare_lt(entry, best))) {
	    /*
	     * A new best entry is to be had. Any previous thoughts
	     * about split horizon occuring are bogus.
	     */
	    best = entry;
	    split_happened = FALSE;
	  }
	}
	/* Do round robin, if applicable */
	if ((best != NULL ) && (!split_happened && nov_gns_roundrobin)) {
	  lw_remove(&best->saps);
	  lw_insert(&best->saps, sapq->saps.blink);
	}
	entry = best;
      }
      if (match && entry && (entry != (novell_sapentrytype *)&sapq->saps) &&
	  !split_happened) {
	if (!count) /* Only get a packet once */
	  reply = getbuffer(MEDDATA);
	if (reply) {
	  sap = (novell_sappacket *)(XNSDATASTART(reply));
	  sap->response_type = NOVELL_SAP_NEARNAMERESPONSE;
	  data = (novell_serveridtype *)&sap->server[count];
	  bcopy((uchar *)&entry->server, (uchar *)&sap->server[count],
		sizeof(novell_serveridtype));
	  count++;
	  /*
	   * CSCdi46488. Patch to workaround a compiler bug for the
	   * 1000 platform.
	   */
	  nethops = !(entry->route_flags & NET_IGRP) ? 
	    data->nethops + 1 :
	      (novrip_pdb.redist_hops) ? novrip_pdb.redist_hops :
		entry->route_hops + 1;
	  data->nethops = nethops;
	}
      }
    }
    if (count)  /* we are all done, if got anything, send it */
      novell_send_response(pak, reply, TRUE, count);
}

/*
 * novell_rcv_nearquery
 *
 * Having received a nearest service query, look through the database we have
 * built and return the first matching entry (don't respond if we have heard
 * advertisements for the requested service type on the same cable we received
 * the query on).
 *
 * Note that our queue of servers is sorted as advertisements come in.
 * If the service being asked about is OLD (not updated in 3 minutes),
 * assume that it is down - don't respond, and remove it from the queue.
 */

forktype
novell_rcv_nearquery (void)
{
    paktype *pak, *reply = NULL;
    ipxhdrtype *ipx;
    novell_sappacket *sap = NULL;
    novell_sapentrytype *entry, *best, *scan, *lastscan;
    novell_sapqueuetype *sapq;
    novell_serveridtype *data;
    sys_timestamp starttime;
    novell_querytype *query;
    int count;
    boolean split_happened, result;
    volatile ushort nethops;

    edisms((blockproc *)systeminitBLOCK, (ulong)&novell_gnsQ);
    process_set_priority(PRIO_LOW);
    while (TRUE) {
	edisms((blockproc *)queueBLOCK, (long)&novell_gnsQ);
	count = 10;
	while (TRUE) {
	    /* Switch only a limited number of packets per pass */
	    if (--count <= 0) {
		break;
	    }
	    /* Dequeue a GNS datagram. Punt if none there */
	    pak = dequeue(&novell_gnsQ);
	    if (!pak)
	      break;

	    ipx = (ipxhdrtype *)ipxheadstart(pak);
	    novell_stat[NOVELL_SAPREQIN]++;
	    pak->if_input->nov_sap_in_packets++;
	    query = (novell_querytype *)(XNSDATASTART(pak));

	    if (!nov_good_query_packet(pak,query)) {
	      retbuffer(pak);
	      continue;
	    }

	    /* Handle Nearest Name Queries */
	    if (query->query_type == NOVELL_SAP_NEARNAMEQUERY) {
	      novell_nearnamequery(pak);
	      retbuffer(pak);
	      continue;
	    }
	    sapq = novell_findsapqueue(GETSHORT(&query->server_type),
				       IPX_SAP_QUEUE_LOOKUP_MUST_EXIST);
	    if (!sapq) {
		retbuffer(pak);
		continue;
	    }

	    /*
	     * Sort the SAP list if it needs it and it hasn't been done
	     * in a while...
	     */
	    if (!sapq->sorted && !SLEEPING(sapq->next_sorted_time)) {
		GET_TIMESTAMP(starttime);
		novell_sort_sap_list(sapq);
		if (novellsap_debug) {
		    buginf("\nIPX SAP: Sort of SAP list %x took %u msec",
			   sapq->server_type, ELAPSED_TIME(starttime));
		}
		TIMER_START(sapq->next_sorted_time, NOVELL_SAP_SORT_INTERVAL);
	    }
	    split_happened = FALSE;
	    best = NULL;
	    /* If the list is sorted, life is simple... */
	    if (sapq->sorted) {
		entry = (novell_sapentrytype*) &(sapq->saps.flink);
		while (((entry = (novell_sapentrytype *)entry->saps.flink) != (novell_sapentrytype *)&sapq->saps)) {
		    result = novell_nsokay(sapq, entry, pak, &split_happened);
		    if (split_happened) {
			break;
		    }
		    if (!result) {
			continue;
		    }

		    /*
		     * We have found a valid entry. Continue searching the
		     * valid entries of this distance, doing a 'round robin'
		     * if requested, and also checking to see if another
		     * one of equal cost is hanging around that would want
		     * us to do split horizon
		     */
		    scan = entry;
		    do {
			lastscan = scan;
			/* Skip if not eligible for split horizon: */
			if ((scan->poisoned) ||
			    (scan->paths == NULL) ||
			    (!nov_any_idb_up(scan)))
			  continue;
			/* Skip if access checks fail */
			if (nov_sapcheck
			    ((novell_serveridtype *)
			     &scan->server, 
			     scan->server.server_type, 
			     pak->if_input->nov_sap_outfilterlist) &&
			    nov_sapcheck
			    ((novell_serveridtype *)
			     &scan->server, 
			     scan->server.server_type, 
			     pak->if_input->nov_gns_outfilterlist)) {
			    /* A possible service. Bail if Split Horizon */
			    if (novell_sap_split_horizon
				(scan, pak->nif_input, IPX_ADVERTISING_NO)) {
				split_happened = TRUE;
				break;
			    }
			}
		    } while
			(((scan = (novell_sapentrytype *)scan->saps.flink) !=
			  (novell_sapentrytype *)&sapq->saps) &&
			 (!novell_sap_compare_lt(lastscan, scan)));
		    
		    /* Do round robin, if applicable */
		    if (!split_happened && nov_gns_roundrobin &&
			entry != lastscan) {
			lw_remove(&entry->saps); /* Unlink old entry */
			lw_insert(&entry->saps, scan->saps.blink); /* Hook old last to it */
		    }
		    break;
		}
	    } else {
		/* The list is not sorted... Grr... */
		entry = (novell_sapentrytype*) &(sapq->saps.flink);
		best = NULL;
		while (((entry = (novell_sapentrytype *)entry->saps.flink) != (novell_sapentrytype *)&sapq->saps)) {
		    result = novell_nsokay(sapq, entry, pak, &split_happened);
		    if (!result) {
			continue;
		    }
		    if ((best == NULL) ||
			(novell_sap_compare_lt(entry, best))) {
			/*
			 * A new best entry is to be had. Any previous thoughts
			 * about split horizon occuring are bogus.
			 */
			best = entry;
			split_happened = FALSE;
		    }
		}

  		/* Do round robin, if applicable */
 		if ((best != NULL ) &&
		    (!split_happened && nov_gns_roundrobin)
		    && (query->query_type == NOVELL_SAP_NEARQUERY)) {
 		    lw_remove(&best->saps);
 		    lw_insert(&best->saps, sapq->saps.blink);
  		}
  		entry = best;
  	    }
 	    if (entry && (entry != (novell_sapentrytype *)&sapq->saps) &&
		!split_happened) {
		reply = getbuffer(MEDDATA);
		if (reply) {
		    sap = (novell_sappacket *)(XNSDATASTART(reply));
		    sap->response_type = NOVELL_SAP_NEARRESPONSE;
		    data = (novell_serveridtype *)&sap->server[0];
		    bcopy((uchar *)&entry->server, (uchar *)&sap->server[0],
			  sizeof(novell_serveridtype));
		    /*
		     * CSCdi46488. Patch to workaround a compiler bug for the
		     * 1000 platform.
		     */
		    nethops = !(entry->route_flags & NET_IGRP) ? 
			data->nethops + 1 :
			    (novrip_pdb.redist_hops) ? novrip_pdb.redist_hops :
				entry->route_hops + 1;
		    data->nethops = nethops;
		    /* CSCdi46488.
                    if (entry->route_flags & NET_IGRP) {
                       if (novrip_pdb.redist_hops)
                           data->nethops = novrip_pdb.redist_hops;
                       else
                           data->nethops = entry->route_hops + 1;
                    } else {
                        data->nethops++;
                    }
		     */
		    novell_send_response(pak, reply, TRUE, 1);
		}
	    }
	    retbuffer(pak);
	    (void) process_may_suspend();
	}
    }
}

/*
 * novell_prune_nidb_sapq
 *
 * An interface has gone down. Run through the sapQ and poison all entries
 * associated with this interface. If static, null out the idb pointer.
 * If dynamic, delete the entry.
 *
 */
void
novell_prune_nidb_sapq (nidbtype *nidb)
{
    novell_sapqueuetype *sapq;
    novell_sapentrytype *entry, *nextentry;

    /* Accept no substitute for the real thing! */
    if ((nidb == NULL) || (nidb->idb == NULL))
	return;

    FOR_ALL_SAP_TYPES(sapq) {
 	FOR_ALL_SAPS_OF_TYPE_ALLOW_DELETE(entry, nextentry, sapq) {
	    novell_sapentry_poisoned(sapq, entry, nidb);
	}
    }
}

/*
 * novell_send_response
 */

void
novell_send_response (
    paktype *pak,
    paktype *reply,
    boolean neartype,
    int count)
{
    ipxhdrtype *ipx, *reply_ipx;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    reply_ipx = (ipxhdrtype *)ipxheadstart(reply);

    reply_ipx->len = XNSOVERHEADBYTES + 2 + (count * sizeof(novell_serveridtype));
    reply_ipx->tc = 0;
    reply_ipx->pt = NOVELL_TYPE_SAPREPLY;
    PUT2WD(reply_ipx->ddnet,
	   (GET2WD(ipx->ssnet) ?
	    GET2WD(ipx->ssnet) : pak->nif_input->novellnet));
    ieee_copy(ipx->shost, reply_ipx->dhost);
    reply_ipx->dsock = ipx->ssock;
    PUT2WD(reply_ipx->ssnet, pak->nif_input->novellnet ?
	   pak->nif_input->novellnet : ipx_internal_net);
    ieee_copy(pak->if_input->novell_node, reply_ipx->shost);
    reply_ipx->ssock = ipx->dsock;
    reply_ipx->cksum = NOVELL_NO_CHECKSUM;		/* no checksum supplied */
    if (novellsap_debug)
	novell_printpak(reply, "Response (out)");
    reply->linktype = LINK_NOVELL;
    /*
     * delay our response to "get nearest server" so the clients are more
     * likely to hear from local server before us.  Split horizon doesn't
     * catch all the cases.
     *
     * Delay the GNS per any interface GNS delay set. Default delay
     * is the old global novell_gns_response_delay.
     *
     * default configurable as 'novell gns-response-delay nnn'.
     * interface config same command but in interface mode.
     */
    if (neartype && (pak->if_input->nov_gns_delay != 0))
	process_sleep_for(pak->if_input->nov_gns_delay);

    reply->flags |= PAK_PRIORITY;

    /*
     * Directed responses ALWAYS go out via standard RIP. IGRP does not
     * need these, and it allows IGRP to run on nets with only clients
     * (no servers).
     */
    if (!nov_send(reply)) {
      if (novellsap_debug || novellsap_event_debug)
	buginf("\nIPXSAP: cannot send SAP REPLY");
    } else {
	pak->if_input->nov_sap_out_packets++;
        novell_stat[NOVELL_SAPREPLY]++;
    }
}

/*
 * novell_rcv_response
 * Having received a (broadcast) Services advertisement, insert the
 * services in our queue.  Put things with a lower hop count earlier
 * in the queue (we have to find if the entry is already there anyway).
 * Deqeueue the service if it is going down.
 *
 * If non NULL, call 'extrafunc' for each server, with the memory handle
 * of 'extradata' as parameter, along with each server. 
 *
 * THIS ROUTINE MUST NEVER EVER EVER EVER SUSPEND!! IT MUST RUN TO
 * COMPLETION!!!
 */
void novell_accept_sap_data (novpdbtype *pdb,
			     paktype *pak,
			     novell_sappacket *sap,
			     ulong sap_flags,
			     char *name_text,
			     novell_accept_sap_data_function extrafunc,
			     dual_peertype *extradata,
			     novpdbtype *extradata2)
{
    ipxhdrtype *ipx;
    int count, i;
    novell_serveridtype *server;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (novellsap_debug)
	novell_printpak(pak,"Response (in)");
    if (sap->response_type != NOVELL_SAP_GENRESPONSE) {
	if (novellsap_debug || novellsap_event_debug)
	   buginf("\nIPXSAP: unexpected response type %d via %s",
		sap->response_type, name_text);
	return;
    }
    count = (GETSHORT(&ipx->len) - 
	     (XNSOVERHEADBYTES+2))/sizeof(novell_serveridtype);
    for (i = 0; i < count; i++) {
	server = &sap->server[i];
	novell_addserver(pdb, &sap->server[i], pak->nif_input, ipx->shost,
			 GET2WD(ipx->ssnet), sap_flags,
			 sap->server[i].nethops, NULL, NULL, 0, 0, FALSE);
	if (extrafunc != NULL) {
	    (*extrafunc)(&sap->server[i], extradata, extradata2);
	}
    }
}


void novell_rcv_response (paktype *pak, novell_sappacket *sap)
{
    novell_accept_sap_data(&novrip_pdb, pak, sap, NET_RIP,
			   "RIP", NULL, NULL, NULL);
}
 
/*  
 * nov_same_eigrp_nidb_and_shost  
 *  
 * Return TRUE if the server learned on nidb_input and from SHOST has a route  
 * to it that was learned from the same location.  
 */  
static boolean nov_same_eigrp_nidb_and_shost (nidbtype *nidb_input, 
					      novrdbtype *rdb, uchar *shost)  
{  
    novrdbtype *pp;  
  
    if (nidb_input) {  
      for (pp = rdb; pp; pp = pp->next) {  
          if (!validmem(pp) || !validmem(pp->nidb)) {  
              return(FALSE);  
          }  
          if ((nidb_input == pp->nidb) && ieee_equal(shost, pp->srchost)) {  
              return(TRUE);  
          }  
      }  
    }  
    return(FALSE);  
}  

/*
 * novell_update_sap_route_info
 *
 * Update SAP routing info to the current value stored in the NDB. This
 * 'only' happens on routing table changes, but it makes Get Nearest
 * Server queries happen a LOT faster.
 */
void novell_update_sap_route_info (novndbtype *ndb)
{
    novell_sapentrytype *entry, *nextentry;
    uchar *first_hop;
    novndbtype *route;
    nidbtype *nidb_hold;
    ulong route_delay, route_hops, route_flags;
    ulong dstnet = ndb->dstnet;

    /*
     * If we have changed a route, it is time to see if our backup paths
     * have anything they want to tell us... EVEN IF novell_sap_uses_routing
     * IS FALSE!! THIS IS TO GET SAPS PREVIOUSLY LEARNED FOR ROUTES THAT
     * WE DID NOT HAVE, OR ALLOWS US TO ACCEPT NEW SAP INFORMATION FROM
     * INTERFACES THAT WE PREVIOUSLY IGNORED!!
     */
    novell_posit_backup_servers_needed();

    /* This is useless if the SAP table doesn't use routing information */
    if (!novell_sap_uses_routing) {
	return;
    }
    if (ndb->delay != NOVELL_INFDELAY) {
	first_hop = novrip_first_hop(ndb->dstnet, (uchar *) "Foo",
				     &nidb_hold, &route_flags, ndb,
				     IPX_ROUND_ROBIN_NO,
				     IPX_DEFAULT_ROUTE_NO);
    } else {
	route = novrip_find_rt(dstnet, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_FUZZ);
	if (route) {
	    first_hop = novrip_first_hop(dstnet, (uchar *) "Foo",
					 &nidb_hold, &route_flags, route,
					 IPX_ROUND_ROBIN_NO, 
					 IPX_DEFAULT_ROUTE_YES);
	    ndb = route;
	} else {
	    first_hop = NULL;
	}
    }
    if (first_hop) {
	route_delay = ndb->delay;
	route_hops = ndb->hopcount;
	route_flags = ndb->flags;
	if (route_flags & NET_STATIC) {
	    route_flags = NET_RIP;
	} else if (route_flags & NET_IGRP) {
	    route_delay = ndb->path_metric;
	    route_hops = ndb->ext_hop;
	} else if (route_flags & NET_RIP) {
	    if ((route_hops >= novell_infinity) ||
		TIMER_RUNNING(ndb->holddownuntil) ||
		(route_delay == NOVELL_INFDELAY)) {
		route_delay = METRIC_INACCESSIBLE;
		route_hops = NOVELL_MAX_HOPS_ALLOWED;
	    }
	}
    } else {
	route_delay = METRIC_INACCESSIBLE;
	route_hops = NOVELL_MAX_HOPS_ALLOWED;
	route_flags = NET_RIP;
    }
    for (entry = (*novell_sap_route_hashtable)[NOV_NETHASH(dstnet)];
	 entry; entry = nextentry) {
	    nextentry = entry->net_hash_next;
	    if ((entry->server.dnet & ndb->rt_mask) == ndb->dstnet) {
		boolean eigrp_must_poison = FALSE;
		novell_sapentry_rdb *srdb;
		/*
		 * The route has changed, make sure the assumption that
		 * EIGRP must learn services from the same interface
		 * and same hosts still hold.
		 */
		if (entry->flags & NET_IGRP) {
		    for (srdb = entry->paths; srdb; srdb = srdb->next) {
			if (!nov_same_eigrp_nidb_and_shost(srdb->nidb,
							   ndb->path_list,
							   srdb->srchost)) {
			    eigrp_must_poison = TRUE;
			    break;
			}
		    }
		}
		if (first_hop == NULL || eigrp_must_poison) {
		    novell_sap_poison(entry);
		} else {
		    if ((entry->route_delay != route_delay) ||
			(entry->route_hops != route_hops)) {
			entry->route_delay = route_delay;
			entry->route_hops = route_hops;
			entry->route_flags = route_flags;
			entry->sapq->sorted = FALSE;
		    }
		}
	    }
    }
}

/*
 * ipx_insert_server_backup_info
 * Place server information in the backup table. ALSO, before trying this,
 * if pdb is NOT of type 'NLSP' AND this advertisement is EQUAL or BETTER cost
 * to the one we are using, AND we are designated router on this interface,
 * THEN update this entry, if not already showing somebody else. If we are
 * the current IDB, and our cost is WORSE, then erase us, and indicate this
 * server entry has changed. 
 */
static void ipx_insert_server_backup_info (novell_sapentrytype *match,
				    backuptype *backup, novpdbtype *pdb,
				    int nethops, nidbtype *nidb_input,
				    ulong sap_flags)
{

    if (!(sap_flags & NET_NLSP)) {
	return;
    }
    
    /* Now let the normal insert routines do their stuff */
    if (nethops < novell_infinity) {
	ipx_insert_backup_info(&(match->backup_info), backup, pdb, FALSE);
    }
}

/*
 * nov_find_urd
 *
 * Find the urd for a net if it exists, otherwise return NULL.
 * If found, remove the urd from the active table.
 *
 */
static nov_urd_type *nov_find_urd (ulong dnet)
{
    nov_urd_type *urd, *last_urd;
    int i;

    if (nov_urd_table == NULL) {
	return(NULL);
    }

    i = NOV_NETHASH(dnet);
    last_urd = (nov_urd_type *) &(*nov_urd_table)[i];
    urd = (*nov_urd_table)[i];

    while (urd != NULL) {
	if (urd->dstnet == dnet) {
	    last_urd->next = urd->next;
	    urd->next = NULL;
	    return(urd);
	}
	last_urd = urd;
	urd = urd->next;
    }
    return(NULL);
}


/*
 * nov_get_urd
 *
 * Find a urd for a net if it exists, or create one. Note that we
 * may still return a NULL if the urd table has not been allocated.
 *
 */
static nov_urd_type *nov_get_urd (ulong dnet)
{
    nov_urd_type *urd;

    if (nov_urd_table == NULL) {
	return(NULL);
    }

    urd = nov_find_urd(dnet);
    if (urd == NULL) {
	urd = malloc_named(sizeof(nov_urd_type), "IPX URD");
	if (urd == NULL) {
	    return(NULL);
	}
	urd->dstnet = dnet;
    }
    return(urd);
}


/*
 * nov_insert_urd
 *
 * Insert a new descriptor for an unreachable route into the URD table.
 *
 */
void nov_insert_urd (nov_urd_type *urd)
{
    int i;
    backuptype *backup_list;

    if (urd == NULL) {
	return;
    }

    /*
     * Delete everything if no URD table could be allocated.
     */
    if ((urd->backup_info == NULL) || (nov_urd_table == NULL)) {
	backup_list = urd->backup_info;
	urd->backup_info = NULL;
	ipx_delete_backup_list(backup_list);
	urd->next = NULL;
	free(urd);
	return;
    }

    i = NOV_NETHASH(urd->dstnet);
    urd->next = (*nov_urd_table)[i];
    (*nov_urd_table)[i] = urd;

    novell_urd_insert_count++;
}

/*
 * ipx_delete_backup_info
 *
 * Clear out backup info by pdb
 *
 */
static void ipx_delete_backup_info (backuptype **backup_list, novpdbtype *pdb)
{
    backuptype *bi, *last_bi, *next_bi;

    last_bi = (backuptype *)backup_list;

    for (bi = *backup_list; bi; bi = next_bi) {
	next_bi = bi->next;
	if (bi->pdbindex == pdb->index) {
	    last_bi->next = next_bi;
	    bi->next = NULL;
	    free(bi);
	    continue;
	}
	last_bi = bi;
    }
}

/*
 * nov_clean_urd_table
 *
 * Deallocate the URD table after first deleting its contents.
 */
void nov_clean_urd_table (novpdbtype *pdb)
{
    nov_urd_type *urd, *last_urd, *next_urd;
    int i;
    int count = 0;

    if (nov_urd_table == NULL) {
	return;
    }

    /*
     * Run through the entire table, but delete backup info
     * for this routing instance only:
     */
    for (i = 0; i < NOV_NETHASHLEN; i++) {

	last_urd = (nov_urd_type *) &(*nov_urd_table)[i];
	urd = (*nov_urd_table)[i];
	
	while (urd != NULL) {
	    next_urd = urd->next;
	    ipx_delete_backup_info(&urd->backup_info, pdb);
	    if (urd->backup_info == NULL) {
		urd->next = NULL;
		free(urd);
		last_urd->next = next_urd;
		urd = next_urd;
		count++;
		continue;
	    }
	    last_urd = urd;
	    urd = next_urd;
	}
    }

    /*
     * Do not delete the URD table, once allocated. This greatly
     * simplifies the task of reallocating it when multiple NLSP
     * pdb's have been activated, deactivated, then reactivated.
     */
}

/*
 * nov_delete_urd_table
 *
 * Unconditionally delete the urd table and all contents:
 */
static void nov_delete_urd_table (void)
{
    nov_urd_type *urd, *next_urd;
    backuptype *backup_list;
    int i;
    int count = 0;

    if (nov_urd_table == NULL) {
	return;
    }

    /*
     * Run through the entire table, and delete backup info
     * for all routing instances:
     */
    for (i = 0; i < NOV_NETHASHLEN; i++) {

	urd = (*nov_urd_table)[i];
	(*nov_urd_table)[i] = NULL;
	
	while (urd != NULL) {
	    next_urd = urd->next;
	    backup_list = urd->backup_info;
	    urd->backup_info = NULL;
	    ipx_delete_backup_list(backup_list);
	    urd->next = NULL;
	    free(urd);
	    urd = next_urd;
	    count++;
	}
    }

    /*
     * Do not delete the URD table, once allocated. This greatly
     * simplifies the task of reallocating it when multiple NLSP
     * pdb's have been activated, deactivated, then reactivated.
     */
}

/*
 * nov_update_urd_list
 *
 * Allocate a descriptor for an unreachable route. Remember via the backup
 * descriptor list, protocol and context to allow required notification
 * if and when the route becomes reachable. (At the present time this
 * mechanism is used by NLSP to recover services which could not be
 * added to the SAP table for lack of a route.
 */
void nov_update_urd_list (novpdbtype *pdb, ulong dnet,
			      backuptype *binfo, boolean debug)
{
    nov_urd_type *urd;

    urd = nov_get_urd(dnet);
    if (urd == NULL) {
	return;
    }
    ipx_insert_backup_info(&(urd->backup_info), binfo, pdb, debug);
    nov_insert_urd(urd);
}

/*
 * nov_backup_to_urd_list
 *
 * Roll pessimal service info into the urd table.
 *
 */
static void nov_backup_to_urd_list (novell_sapentrytype *entry)
{
    nov_urd_type *urd;
    novpdbtype *pdb;
    backuptype *bi, *last_bi, *old_backup_info;
    boolean debug;

    if (nov_urd_table == NULL) {
	return;
    }

    pdb = NOV_INDEX2PDB(entry->novpdbindex);
    if (!pdb)
	return;

    debug = (novell_private_debug && NOVNLSP_DEBUG(entry->server.dnet, pdb));

    urd = nov_get_urd(entry->server.dnet);
    if (urd == NULL) {
	return;
    }

    /*
     * Append the complete sap backup list to the current urd backup info:
     */
    old_backup_info = entry->backup_info;
    entry->backup_info = NULL;
    last_bi = (backuptype *)&urd->backup_info;
    for (bi = urd->backup_info; bi; bi = bi->next) {
	last_bi = bi;
    }
    last_bi->next = old_backup_info;
    nov_insert_urd(urd);
}

/*
 * nov_check_urd_backup_queue
 * Check the backup queue of instances of this server that we
 * heard about before, but ignored for some reason (typically
 * for want of a route). Note that this routine deallocates
 * all backup entries.
 */
static void nov_check_urd_backup_queue (nov_urd_type *urd, boolean debug)
{
    backuptype *bi, *binext;
    novpdbtype *loop_pdb;
    
    /* Somebody may have better info for us */
    for (bi = urd->backup_info; (bi != NULL); bi = binext) {
	binext = bi->next;
	loop_pdb = NOV_INDEX2PDB(bi->pdbindex);
	if ((loop_pdb != NULL) && (loop_pdb->add_route)) {
	    if (debug) {
		buginf("\nIPX: URD add svc from net %x, pdb @ %x (%x)"
		       ", lsp {%d/%d}",
		       urd->dstnet, loop_pdb, loop_pdb->index,
		       (bi->backup_context_info >> 16),
		       (bi->backup_context_info & 0x0000FFFF));
	    }
	    (*loop_pdb->add_route)(loop_pdb, urd->dstnet, NOV_NETMASK_ALL,
				   bi->backup_context_info,
				   bi->route_type);
	}
	free(bi);
    }
    urd->backup_info = NULL;

    novell_urd_callback_count++;
}

/*
 * nov_display_urd_backup_queue
 *
 */
static void display_urd_backup_queue (backuptype *bi, int depth)
{
    novpdbtype *pdb;
    backuptype *binext;
    ushort index, version;

    depth++;

    if (bi == NULL)
	return;

    if (depth > 20) {
	printf(" ...");
	return;
    } else {
	binext = bi->next;
	display_urd_backup_queue(binext, depth);
    }

    pdb = NOV_INDEX2PDB(bi->pdbindex);

    if (pdb) {
	if (pdb->proctype == NET_NLSP) {
	    index = (bi->backup_context_info >> 16);
	    version = (bi->backup_context_info & 0x0000FFFF);
	    printf(" N:%-04x:%-04x", index, version);
	    
	    if (reg_invoke_isis_valid_lsp_version(pdb->osi_pdb,
						  index, version)) {
		printf("(OK)");
	    } else {
		printf("(??)");
	    }
	} else if (pdb->proctype == NET_IGRP) {
	    printf(" E:????");
	} else {
	    printf(" ?:???? (pdb @ %x (%x), lsp {%d/%d}",
		   pdb, pdb->proctype,
		   (bi->backup_context_info >> 16),
		   (bi->backup_context_info & 0x0000FFFF));
	}
    } else {
	printf(" NULL PDB");
    }
}

/*
 * nov_display_urd_table
 */

void nov_display_urd_table (ulong net)
{
    nov_urd_type *p, *last_p;
    int i, ii, hash, count = 0, total = 0;
    boolean invalid_list = FALSE;
    ptable *rp;

    printf("\n\nURD table:");
    if (nov_urd_table == NULL) {
	printf(" NONE");
	return;
    }
    /*
     * Run through the table once counting and locking everything:
     */
    for (i = 0; i < NOV_NETHASHLEN; i++) {
	last_p = (nov_urd_type *) &(*nov_urd_table)[i];
	for (p = (*nov_urd_table)[i]; p; p = p->next) {
	    if (validmem(p)) {
		last_p = p;
		total++;
		if ((net != 0) && (net != p->dstnet))
		    continue;
		count++;
	    } else {
		invalid_list = TRUE;
		last_p->next = NULL; /* Fix it up as best we can */
		printf("  %%Invalid urd entry @ %x", p);
		return;
	    }
	}
    }
    /*
     * Allocate an array of pointers to hold everything. If no memory, bail.
     */
    rp = malloc((count+1)*sizeof(uchar *));
    if (rp == NULL) {
	return;
    }
    /*
     * Fill in the array, locking elements:
     */
    for (i = 0, ii = 0; i < NOV_NETHASHLEN; i++) {
	for (p = (*nov_urd_table)[i]; p; p = p->next) {
	    if (ii > count) {
		break;
	    }
	    if (validmem(p)) {
		if ((net != 0) && (net != p->dstnet))
		    continue;
		mem_lock(p);
		*rp[ii++] = p;
	    }
	}
    }
    /*
     * It is now safe to suspend:
     */
    process_may_suspend();

    printf(" (%d/%d entries)", total, ii);
    if (invalid_list) printf(" *Bad*");
    printf("  %d created, %d consumed, %d invalidated",
	   novell_urd_insert_count,
	   novell_urd_callback_count,
	   novell_urd_scrub_count);
    /*
     * Do the display at our leisure:
     */
    automore_enable(NULL);

    for (i = 0; i < count; i++) {

	if (automore_quit())
	    break;

	p = *rp[i];
	hash = NOV_NETHASH(p->dstnet);

	printf("\n %-3d: Net %-08x", hash, p->dstnet);

	display_urd_backup_queue(p->backup_info, 0);

	if (automore_quit())
	    break;
    }

    automore_disable();

    /*
     * Step through the list of locked entries and free everything:
     */
    for (i = 0; i < count; i++) {
	free(*rp[i]);
    }
    free(rp);
}

/*
 * nov_up_route
 *
 */
static void nov_up_route (novpdbtype *pdb, novndbtype *ndb)
{
    nov_urd_type *urd;
    boolean debug = (novell_private_debug && NOVNLSP_DEBUG(ndb->dstnet, pdb));

    /*
     * Check if service(s) previously rejected as unreachable.
     * Scan table for Unreachable Route Descriptor (urd):
     */
    urd = nov_find_urd(ndb->dstnet);
    
    if (urd != NULL) {
	if (debug) buginf("\nIPX-NLSP: ROUTE_UP net %x, svc via net %x"
			  " was unreachable, trigger SPF",
			  ndb->dstnet, urd ? urd->dstnet : -1);
	/*
	 * If found we know that a service could not be added
	 * to the service table for want of a route. Schedule
	 * SPF for all valid backup entries and deallocate all
	 * backup entries as well as the urd.
	 */

	nov_check_urd_backup_queue(urd, debug);
	free(urd);
    }
}

/*
 * nov_modif_route
 *
 * A route has changed metric, possibly after being learned via a
 * different protocol. For any NLSP learned services dependent
 * upon this route, kick in backup (pessimal) route information. What
 * was once pessimal may now be preferred over the current route.
 *
 * Treat as ROUTE_UP as well to pickup anything that may be in the
 * urd structure. This can be important as both NLSP and EIGRP can
 * redistribute using ROUTE_MODIF when they supercede an existing
 * ndb in HOLDDOWN.
 */
static void nov_modif_route (novpdbtype *pdb, novndbtype *ndb)
{
    novell_sapentrytype *entry;
    boolean debug = (novell_private_debug && NOVNLSP_DEBUG(ndb->dstnet, pdb));

    /*
     * Use the hash table to lookup existing SAP's associated with
     * this network. If any are found, kick in any and all backup
     * info, in case anything that was pessimal is now optimal.
     */
    for (entry = (*novell_sap_route_hashtable)[NOV_NETHASH(ndb->dstnet)];
	 entry; entry = entry->net_hash_next) {
	
	if (entry->server.dnet == ndb->dstnet) {
	    if (debug) buginf("\nIPX-NLSP: ROUTE_MODIF net %x"
			      " check sap backup info",
			      ndb->dstnet);
	    novell_sap_check_backup_queue(entry);
	}
    }
    /*
     * Do up route processing as well, just in case. This takes care
     * of situations were ROUTE_MODIF was signalled because the route
     * existed, but was in holddown. If the route was *really* modified
     * (i.e., it was reachable and is now reachable via a different
     * metric) then there will be nothing for up_route to do anyway,
     * so NBD.
     */
    nov_up_route(pdb, ndb);
}

/*
 * nov_down_route
 *
 * A route has gone down. All services that depend on this route may
 * be unreachable. First, check for a less specific route. If such a
 * route exists, trigger SPF and readd the service as appropriate.
 * If no such route exists, the service is indeed unreachable. In
 * this case, place NLSP context in the urd table. A future ROUTE_UP
 * event will then resurrect this service if it is still valid.
 */
static void nov_down_route (novpdbtype *pdb, novndbtype *ndb)
{
    novell_sapentrytype *entry;
    novndbtype *alt_ndb;
    novpdbtype *sap_pdb;
    boolean debug = (novell_private_debug && NOVNLSP_DEBUG(ndb->dstnet, pdb));

    /*
     * Is there a less specific route (e.g., the default route)?
     * If not, simply save server context. If so, try to add the
     * server again, reachable this time via the less specific route.
     */
    for (entry = (*novell_sap_route_hashtable)[NOV_NETHASH(ndb->dstnet)];
	 entry; entry = entry->net_hash_next) {
	
	if ((entry->server.dnet == ndb->dstnet) &&
	    (entry->server.nethops < novell_infinity)) {

	    sap_pdb = NOV_INDEX2PDB(entry->novpdbindex);

	    /*
	     * Restrict this to NLSP for now:
	     */
	    if ((entry->flags & NET_NLSP) &&
		(sap_pdb && (sap_pdb->proctype == NET_NLSP))) {

		alt_ndb = novrip_find_rt(entry->server.dnet,
					 NOV_NETMASK_ALL,
					 IPX_ROUTE_MATCH_FUZZ);
		/*
		 * Ensure alternate route is valid:
		 */
		if (alt_ndb && !ipx_route_valid(alt_ndb)) {
		    alt_ndb = NULL;
		}
		/*
		 * Force current context info into backup list. From
		 * there it will either be rolled directly into a
		 * URD or it will be used to re-add the server.
		 */
		if (debug) {
		    buginf("\nIPX-NLSP: ROUTE_DOWN net %x,"
			   " pdb @ %x (%x), alt net %x (%s)",
			   ndb->dstnet, sap_pdb, sap_pdb->proctype,
			   alt_ndb ? alt_ndb->dstnet : 0,
			   alt_ndb ? "add" : "urd");
		}
		ipx_sap_self_backup(entry, debug);
		
		if (alt_ndb == NULL) {
		    /*
		     * No less specific route(s). Capture everything
		     * in the urd backup list for possible future ref.
		     */
		    nov_backup_to_urd_list(entry);
		} else {
		    /*
		     * A less specific route appears to exist. Try and
		     * add the service back. If this fails, addserver()
		     * will roll this info into a URD for future ref.
		     */
		    novell_sap_check_backup_queue(entry);
		}
	    }
	}
    }
}

/*
 * novell_addserver
 * Given an incoming server advertisment, add it to our database, or
 * update it's entry.  Keep the server queue doubly sorted by server
 * type, and by hop count, both in descending order.
 *
 * Note: Be careful since NLSP calls this routine *without* next hop
 * information (i.e., snet = 0 and shost is NULL). Do not try to apply
 * the sap-router-filter on this or dereference a NULL source host pointer
 * for any reason (such as saving the source net.host information in
 * the backup table).
 *
 * THIS FUNCTION MUST RUN TO COMPLETION WITHOUT ANY PROCESS BREAKS!
 *
 * IT IS SAFE TO CALL THIS FUNCTION FROM ANY PROCESS!!!
 */
void novell_addserver (novpdbtype *pdb,
		       novell_serveridtype *server,
		       nidbtype *nidb_input,
		       uchar *shost,
		       ulong snet,
		       ulong sap_flags,
		       ushort sap_ext_hops,
		       nov_cost_type *nlsp_cost,
		       isis_lsp_linkage **lsp_lh,
		       ushort lsp_version,
		       ushort lsp_index,
		       boolean via_backup_table)
{
    novpdbtype *match_pdb;
    novell_sapentry_rdb *srdb, *srdb_prev, *last_srdb, *next_srdb;
    novell_sapqueuetype *sapq;
    novell_sapentrytype *match;
    novndbtype *route;
    ulong  route_delay, new_delay, old_delay;
    ushort route_hops,  new_hops,  old_hops;
    ulong  route_flags, new_route_flags, old_route_flags;
    ulong new_sap_flags, old_sap_flags;
    uchar *first_hop;
    nidbtype *nidb_hold;
    ulong flags, dnet;
    boolean mark_changed;
    boolean cmode_debug = ((novellsap_debug || novellsap_event_debug) &&
			   (novellisis_debug || novellnlsp_debug));
    boolean sdebug = (novellsap_debug || novellsap_event_debug);
    boolean ndebug = FALSE;
    boolean pdebug = FALSE;
    boolean vdebug = FALSE;
    ushort nethops;
    backuptype backup, *old_backup_info;
    boolean was_off;
    boolean nidb_changed;

    match = NULL;
    nidb_changed = FALSE;
    old_backup_info = NULL;
    backup.pdbindex = pdb->index;
    backup.backup_context_info = ((lsp_index << 16) | lsp_version);
    backup.route_type = RTYPE_L1;

    if (server == NULL) {
 	if (sdebug)
 	    buginf("\nIPXSAP: Missing Server Name or Type received from %x.%e",
 		   snet,shost);
 	return;
    }
    if (pdb->proctype == NET_NLSP) {
 	ndebug = (NOVNLSP_DEBUG(server->dnet, pdb) && sdebug);
 	pdebug = (novell_private_debug && ndebug);
 	vdebug = (novell_verbose_debug && ndebug);
    }
    
    if (server->server_name[0] == '\0') {
	if (novellsap_debug || novellsap_event_debug) {
	    if (shost) {
		buginf("\nIPXSAP: Missing Server Name received from %x.%e",
		       snet,shost);
	    }
	    else {
		buginf("\nIPXSAP: Missing Server Name received from %x",snet);
	    }
	}
      return;
    }
    /* Fix broken service names -- the last byte should always be null */
    server->server_name[NOVELL_SERVER_NAME_LEN - 1] = '\0';

    /* See comment at start of IPX RIP update packet receiving to understand */
    if (server->nethops >= NOVELL_RIP_ADV_INFINITY) {
	server->nethops = nethops = NOVELL_MAX_HOPS_ALLOWED;
    } else {
	nethops = server->nethops;
    }

    /*
     * If various filters would apply to this server, or the 'generic'
     * server is advertised (i.e., a bogus server entry), just return.
     * Note that filter checks cannot be applied to NLSP services.
     */
    if (server->server_type == NOVELL_EVERYONE) {
	return;
    }
    if (!(sap_flags & NET_NLSP)) {
	if ((!nov_sapcheck(server, server->server_type,
			   nidb_input->idb->nov_sap_infilterlist)) ||
	    (!nov_sapcheck(server, server->server_type,
			   pdb->sap_ifltr_all ? pdb->sap_ifltr_all : 
			   nidb_input->sap_iprotofltr[pdb->index])) ||
	    (snet &&
	     !nov_saproutercheck(snet, shost, server->server_type,
				 server->server_name,
				 nidb_input->idb->nov_sap_routerfilterlist)))
	    return;
    }

    /* 
     * If RIP is turned off, accept only the locally attached services, per
     * page 7-14 in the NLSP Spec.
     */
    if ((!NOVELL_NIDB_UNNUMBERED(nidb_input) &&
	 (sap_flags & NET_RIP) &&
	 (server->dnet != nidb_input->novellnet)) &&
	(!nidb_input->lsp_sap_on)) {
	if (novellsap_debug || novellsap_event_debug)
	    buginf("\nIPXSAP: RIP turned off, rejecting SAP service: type %x server \"%s\"; %d destination net",
		   server->server_type, server->server_name, server->dnet);
	return;
    }

    /*
     * Activate ISIS/NLSP SAP compatibility mode if this is a RIP-learned
     * SAP and the incoming service is a file server:
     *
     * Note the position of this code. The spec does not require that we
     * accept the advertised file server, merely that we see that someone
     * is advertising it (reachable or otherwise). At the present time we
     * choose to ignore the sender if an active filter is present, but
     * acknowledge him if he advertises a file server, even if it is not
     * one which we will accept.
     *
     * THE ABOVE IS SUBJECT TO CHANGE AS THE INTENT OF THE SPEC BECOMES
     * MORE CLEAR.
     */
    if ((sap_flags & NET_RIP) &&
	(server->server_type == NOVELL_SAP_FILESERVER)) {
	was_off = (!nidb_input->lsp_sap_on && (nidb_input->lsp_sap_cfg ==
					       IPX_RIPSAP_CONFIG_AUTO));
	novell_start_nidb_sap_auto(nidb_input);
	novell_start_nidb_rip_auto(nidb_input);
	if (was_off) {
	    if (cmode_debug)
	      buginf("\nIPX-NLSP: activate SAP on net %x",
		     nidb_input->novellnet);
	    novell_posit_full_sap_update(nidb_input);
	}
    }
    route_delay = METRIC_INACCESSIBLE;
    route_hops = NOVELL_MAX_HOPS_ALLOWED;
    route_flags = NET_RIP;
    route = NULL;
    first_hop = NULL;
    if (novell_sap_uses_routing) {
	dnet = ((server->dnet == 0) && (nethops <= 1)) ?
	    nidb_input->novellnet : server->dnet;
	route = novrip_find_rt(dnet, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_FUZZ);
	if (route) {
	    route_delay = route->delay;
	    route_hops = route->hopcount;
	    route_flags = route->flags;
	    if (route_flags & NET_STATIC) {
		route_flags = NET_RIP;
	    } else if (route_flags & NET_IGRP) {
		route_delay = route->path_metric;
		route_hops = route->ext_hop;
	    } else if (route_flags & NET_RIP) {
		if ((route_hops >= novell_infinity) ||
		    TIMER_RUNNING(route->holddownuntil) ||
		    (route_delay == NOVELL_INFDELAY)) {
		    route_delay = METRIC_INACCESSIBLE;
		    route_hops = NOVELL_MAX_HOPS_ALLOWED;
		}
	    }
	    first_hop = novrip_first_hop(dnet, (uchar *) "Foo",
					 &nidb_hold, &flags, route,
					 IPX_ROUND_ROBIN_NO,
					 IPX_DEFAULT_ROUTE_YES);
	}
	if ((route == NULL) || (first_hop == NULL)) {
	    if (server->nethops < novell_infinity) {
 		if (sdebug) {
 		    buginf("\nIPXSAP: %s rejected, route %x not in table",
 			   server->server_name, server->dnet);
 		}
  		/*
  		 * If the SAP is from NLSP, remember what route is missing
  		 * and which LSP fragment it's in. This way we can get
  		 * it back painlessly when the route becomes available.
  		 */
  		if (pdb->proctype == NET_NLSP) {
 		    if (ndebug) buginf("\nIPXSAP: update URD for net %x",
 				       server->dnet);
  		    nov_update_urd_list(pdb, server->dnet, &backup, ndebug);
  		}
  		/*
  		 * SAP is UNREACHABLE ... DROP IT
  		 */
		return;
	    }
	    route_delay = METRIC_INACCESSIBLE;
	    route_hops = NOVELL_MAX_HOPS_ALLOWED;
	    route_flags = sap_flags;
	} else if (sap_flags & NET_IGRP) {
	    if (!nov_same_eigrp_nidb_and_shost(nidb_input, route->path_list,
					       shost)) {
                if (novellsap_debug || novellsap_event_debug)
		    buginf("\nIPXEIGRP: SAP from %x rejected, route %x"
			   " in table via different interface",
			   snet, server->dnet);
		return;
	    }
	} 
    }
    /*
     * Search through our list of services, looking for one that
     * matches this advertisement. Novell says that servers are uniquely
     * identified by the server type and server name.  Believe them.
     */
    
    /* Don't add a queue/hash block for DOWN notice for NEW service */
    if (server->nethops >= novell_infinity) {
      sapq = novell_findsapqueue(server->server_type,
				 IPX_SAP_QUEUE_LOOKUP_MUST_EXIST);
    } else {
      sapq = novell_findsapqueue(server->server_type,
				 IPX_SAP_QUEUE_LOOKUP_MAY_CREATE);
      if (!sapq) {
	return;
      }
    }
    /*
     * Now find an exactly matching entry.  Note that it may be from a
     * different place, with a different hop count.
     */
    if (sapq)
      match = nov_find_sap_name_entry(sapq, server);

    /*
     * Now we have a match, or a null pointer. Before any other machinations
     * check if the incoming advertisment is via vanilla SAP and we have
     * an existing service which was learned via NLSP. If so, perform
     * a callback via the appropriate pdb to determine if this is the best
     * external service known to the cloud thus far. If so, delete the
     * NLSP entry and add the SAP entry. (This is similar to the check
     * for the best XROUTE in an area.)
     */
    if ((match != NULL) && (sap_flags & NET_RIP) &&
	(match->flags & (NET_NLSP))) {
	match_pdb = nov_proctable[match->novpdbindex];
	if (match_pdb->xsap_callback != NULL) {
	    if ((*match_pdb->xsap_callback)
		(match_pdb, match, nethops)) {
		/*
		 * Prefer the external SAP. Delete NLSP service
		 * and add it back into table below. 
		 */
		lw_remove(&match->saps);
		if ((novellisis_debug || novellnlsp_debug) &&
		    (novellsap_debug || novellsap_event_debug))
		    buginf("\nIPXSAP: %s server %x:%s:%x.%e @ [H%d][Ext H%d]"
			   " replaced by SAP service @ [H%d]",
			   match_pdb->name, match->server.server_type,
			   match->server.server_name,
			   match->server.dnet, match->server.dhost,
			   match->server.nethops, match->ext_hop, nethops);
		/*
		 * Current entry is going away. Absorb current path info
		 * into backup list and capture before discarding entry:
		 */
		ipx_sap_self_backup(match, FALSE);
		old_backup_info = match->backup_info;
		/* See if the NIDB list changes... */
		for (srdb = match->paths; srdb != NULL; srdb = srdb->next) {
		    if (srdb->nidb != nidb_input) {
			nidb_changed = TRUE;
			break;
		    }
		}
		match->backup_info = NULL;
		nov_sap_free(sapq, match, IPX_SAP_FREE_IGNORE_BACKUPS);
		match = NULL;
	    } else {
		/*
		 * Continue to prefer the service learned via NLSP. Possibly
		 * save the new one in the potential pseudonode backup table.
		 */
		novrip_process_backup_service(pdb, nidb_input, server, snet,
					      shost, sap_flags);
		return;
	    }
	}
    }
    /*
     * Once again we have a match or a null pointer. If there is a match, check
     * whether this SAP has it closer than we did before.  If this is true,
     * we replace the old entry with the new, and treat the new SAP as new
     * data. Check to make sure we are not overwriting a static sap with
     * dynamic info.
     *
     * The complex IF statement below reads as...
     *  IF there is a match...
     *    AND it is not a static entry...
     *    AND EITHER
     *        a) This is definitely a better SAP than what we already had 
     *	         Because of lower 'distance' OR lower protocol... (IGRP over
     *           RIP, other things being equal)
     *    OR  b) This is a designated router link, and the interface we
     *           have is NOT.
     */
    if (match != NULL) {
	new_delay = route_delay;
	new_hops  = route_hops;
	new_route_flags = route_flags;
	new_sap_flags = sap_flags;
	old_delay = match->route_delay;
	old_hops = match->route_hops;
	old_route_flags = match->route_flags;
	old_sap_flags = match->flags;
	if ((new_sap_flags & NET_NLSP) || (old_sap_flags & NET_NLSP)) {
	    new_delay = old_delay = 0;
	    new_hops = sap_ext_hops;
	    old_hops = match->ext_hop;
	}
	if ((!(match->flags & NET_STATIC)) &&
	    (nov_sap_distance_compare(server, new_delay, new_hops,
				      new_route_flags, new_sap_flags,
				      &(match->server), old_delay, old_hops,
				      old_route_flags, old_sap_flags,
				      NOV_SAP_CMP_LT))) {
	    lw_remove(&match->saps);
	    if (novellsap_debug || novellsap_event_debug) 
		buginf("\nIPXSAP: type %#x server \"%s\" distance lowered;"
		       " new entry accepted [%d/%d/%d]", server->server_type,
		       server->server_name, nethops, route_hops, route_delay);
	    /*
	     * Current entry is going away. Absorb current path info
	     * into backup list and capture before discarding entry:
	     */
	    ipx_sap_self_backup(match, FALSE);
	    old_backup_info = match->backup_info;
	    match->backup_info = NULL;
	    /* See if the NIDB list changes... */
	    for (srdb = match->paths; srdb != NULL; srdb = srdb->next) {
		if (srdb->nidb != nidb_input) {
		    nidb_changed = TRUE;
		    break;
		}
	    }
	    nov_sap_free(sapq, match, IPX_SAP_FREE_IGNORE_BACKUPS);
	    match = NULL;
	}
    }
    if (match == NULL) {
	/* Never heard of this guy */
	if (nethops < novell_infinity) {
	    match = nov_allocate_sap_entry();
	    if (match == NULL) {
		ipx_delete_backup_list(old_backup_info);
		return;
	    }
	    srdb = malloc_named(sizeof(novell_sapentry_rdb), "IPX SRDB");
	    if (srdb == NULL) {
		ipx_delete_backup_list(old_backup_info);
		free(match);
		return;
	    }
	    match->nidb_changed = nidb_changed;
	    srdb->sap = match;
	    if (novellsap_debug || novellsap_event_debug)
		buginf("\nIPXSAP: new SAP entry: type %x"
		       " server \"%s\" %d hops [%d/%d]",
		       server->server_type, server->server_name,
		       nethops, route_hops, route_delay);

	    if (ndebug && (nlsp_cost != NULL)) {
		buginf("\nNLSP-SAP: OOP=%u:%u:%u:%e:%u:%e",
		       nlsp_cost->total_throughput,
		       nlsp_cost->total_delay,
		       nlsp_cost->mtu,
		       &nlsp_cost->system_id[0],
		       nlsp_cost->circuit_id,
		       &nlsp_cost->mac_addr[0]);
	    }
	    bcopy((uchar *)server, (uchar *)&match->server,
		  sizeof(novell_serveridtype));
	    srdb->nidb = nidb_input;
            /*
	     * If server is on the same network as we are, and thinks his
	     * network number is zero, help him out. Hopcount will be 1 if
	     * he is advertizing an internal network number. 
	     */
            if (match->server.dnet == 0) {
                if (match->server.nethops <= 1) {
                    match->server.dnet = nidb_input->novellnet;
                } else {
                    if (novellsap_debug || novellsap_event_debug) {
                        buginf("\nIPXSAP: Server \"%s\" dest net 0 not corrected, %d hops away",
			       server->server_name, match->server.nethops);
                    }
                }
            }
	    novell_set_timer(NULL, srdb);
	    match->novpdbindex = pdb->index;
	    match->poisoned = match->reverse_poisoned = FALSE;
	    if (shost == NULL) {
		ieee_zero(srdb->srchost);
	    } else {
		ieee_copy(shost, srdb->srchost);
	    }
	    srdb->snet = snet;
	    novell_mark_srdb_changed(match, srdb, match->flags,
				     IPX_ROUTE_EVENT_ADD);
	    match->route_delay = route_delay;
	    match->route_hops = route_hops;
	    match->route_flags = route_flags;
	    match->flags = sap_flags;
	    match->ext_hop = sap_ext_hops;
	    match->backup_info = old_backup_info;
	    old_backup_info = NULL;
	    match->pathcount = 1;
	    match->paths = srdb;
	    /*
	     * Update per-LSP info if NLSP learned service:
	     */
	    if (vdebug) buginf("\nadds: new svc");
	    if (lsp_lh != NULL) {
		bcopy(nlsp_cost, &srdb->cost, sizeof(nov_cost_type));
		srdb->lsp_linkage.lsp_version = lsp_version;
		srdb->lsp_linkage.lsp_index = lsp_index;
		reg_invoke_ipx_nlsp_insert_srdb_in_lsp_list(srdb, lsp_lh);
		if (pdebug && (srdb != NULL)) {
		    buginf("\n lsp %d/%d, lh=%x->%x"
			   ", srdb=%x (%x/%x)",
			   lsp_index, lsp_version, lsp_lh, *lsp_lh,
			   srdb, srdb->lsp_linkage.lsp_next, srdb->lsp_linkage.lsp_prev);
		}
	    }
	    if (!sapq)
	      sapq = novell_findsapqueue(server->server_type,
					 IPX_SAP_QUEUE_LOOKUP_MAY_CREATE);
	    novell_place_in_sap_list
		(sapq, match, IPX_SAP_INSERT_HASH_NEEDED,
		 !(match->flags & (NET_IGRP | NET_NLSP)));
	} else {
	    /*
	     * Unreachable ... existing sap (if any) is history. Cleanup ...
	     */
	    if (vdebug) {
		buginf("\nadds: unreachable");
	    }
	    ipx_delete_backup_list(old_backup_info);
	}
    } else if ((match->flags & NET_STATIC) ||
	       (sap_flags != match->flags) ||
	       (pdb->index != match->novpdbindex)) {
	/*
	 * We have chosen to ignore this advertisement. Put it in the
	 * backup table, possibly...(If not a poison update)
	 */
	ipx_insert_server_backup_info(match, &backup, pdb, nethops, nidb_input,
				      sap_flags);
    } else {
	/*
	 * We may want this info.
	 */
	if (vdebug) {
	    buginf("\nadds: cmp path list");
	}
	mark_changed = FALSE;
	srdb_prev = NULL;
	for (srdb = match->paths; srdb != NULL; srdb = srdb->next) {
	    if (pdebug && (srdb != NULL)) {
		buginf("\nadds: nidb %x/%x", nidb_input, srdb->nidb);
		buginf(" snet %x/%x", snet, srdb->snet);
		if (snet != 0)
		    buginf(" %e/%e", srdb->srchost, shost);
		if (srdb->sap->flags & NET_NLSP)
		    buginf(" lsp %d/%d", srdb->lsp_linkage.lsp_index, lsp_index);
	    }
	    if ((nidb_input == srdb->nidb) &&
		(snet == srdb->snet) &&
		((snet == 0) || ieee_equal(srdb->srchost, shost)) &&
		(!(srdb->sap->flags & NET_NLSP) ||
		 (srdb->lsp_linkage.lsp_index == lsp_index))) {
		/* Worse cost stuff? */
		if (vdebug) buginf(" ?GT?");
		if (nov_sap_distance_compare(server, route_delay, route_hops,
					     route_flags, sap_flags,
					     &(match->server),
					     match->route_delay,
					     match->route_hops,
					     match->route_flags, match->flags,
					     NOV_SAP_CMP_GT) ||
		    ((match->server.dnet != 
		      (server->dnet ? server->dnet : nidb_input->novellnet)) ||
		     (!ieee_equal(server->dhost, match->server.dhost)) ||
		     (match->server.dsock != server->dsock))) {
		    if (vdebug) buginf(" TRUE");
		    /* If there are other paths, toss this. */
		    /* I COULD trust pathcount, but why take the risk? */
		    if (match->paths->next != NULL) {
			if (srdb_prev == NULL) {
			    match->paths = srdb->next;
			} else {
			    srdb_prev->next = srdb->next;
			}
			novell_mark_srdb_changed(match, srdb,
						 match->flags,
						 (nethops >= novell_infinity) ?
						 IPX_ROUTE_EVENT_DELETED :
						 IPX_ROUTE_EVENT_PUSHED_OUT);
			novsrdb_free(srdb);
			match->pathcount--;
			return;
		    } else {
			/* This is the last one. Perform changes */
			if (nethops >= novell_infinity) {
			    /* Hop count is down, poison */
			    if ((novellsap_debug || novellsap_event_debug) &&
				(shost != NULL))
				buginf("\nIPXSAP: type %x server \"%s\""
				       " poison received from %x.%e",
				       server->server_type,
				       server->server_name, snet, shost);
			    match->paths = NULL;
			    match->pathcount = 0;
			    novell_mark_srdb_changed(match, srdb,
						     match->flags,
						     IPX_ROUTE_EVENT_DELETED);
			    novsrdb_free(srdb);
			    novell_sap_poison(match);
			    return;
			} else {
			    mark_changed = TRUE;
			    /* 
			     * Update the destination net.host.socket except
			     * if the new destination net is zero. 
			     */
			    if (server->dnet != 0) {
				if ((match->server.dnet != server->dnet) ||
				    (!ieee_equal(server->dhost,
						 match->server.dhost)) ||
				    (match->server.dsock != server->dsock)) {
				    if (match->server.dnet != server->dnet) {
					nov_sap_set_new_dnet(match,
							     server->dnet);
				    }
				    ieee_copy(server->dhost,
					      match->server.dhost);
				    match->server.dsock = server->dsock;
				}
			    }
			    match->route_delay = route_delay;
			    match->route_hops = route_hops;
			    match->route_flags = route_flags;
			    match->server.nethops = nethops;
			    if (vdebug) buginf("\nadds: chg svc");
			    if (lsp_lh != NULL) {
				srdb->lsp_linkage.lsp_version = lsp_version;
				if (pdebug) {
				    buginf(" lsp %d/%d, lh=%x->%x"
					   ", srdb=%x (%x/%x)",
					   lsp_index, lsp_version, lsp_lh, *lsp_lh,
					   srdb, srdb->lsp_linkage.lsp_next, srdb->lsp_linkage.lsp_prev);
				}
			    }
			    /*
			     * Just unqueue it and reinsert it. This doesn't
			     * happen that often. So don't optimize
			     */
			    lw_remove(&match->saps);
			    match->flags = sap_flags;
			    if (!sapq)
				sapq = novell_findsapqueue(server->server_type,
							   IPX_SAP_QUEUE_LOOKUP_MAY_CREATE);
			    novell_place_in_sap_list
				(sapq, match, IPX_SAP_INSERT_HASH_EXISTS, FALSE);
			}
		    }
		} else {
		    /*
		     *Same cost, place, etc, or ok changes ... refresh:
		     */
		    if (nethops < novell_infinity) {
			if (match->poisoned) {
			    match->poisoned = match->reverse_poisoned = FALSE;
			    mark_changed = TRUE;
			}
			if (!(sap_flags & NET_IGRP) || !via_backup_table)
			    novell_set_timer(NULL, srdb);
			if (vdebug) buginf("\nadds: refresh srdb");
			if (lsp_lh != NULL) {
			    srdb->lsp_linkage.lsp_version = lsp_version;
			    if (pdebug) {
				buginf(" lsp %d/%d, lh=%x->%x"
				       ", srdb=%x (%x/%x)",
				       lsp_index, lsp_version, lsp_lh, *lsp_lh,
				       srdb, srdb->lsp_linkage.lsp_next, srdb->lsp_linkage.lsp_prev);
			    }
			}
		    }
		}
		if (mark_changed) {
		    /*
		     * Find out if there is something better hidden
		     * away
		     */
		    novell_posit_backup_servers_needed();
		    /*
		     * Climb through the backup queue, if any exist
		     */
		    novell_sap_check_backup_queue(match);
		    novell_mark_srdb_changed(match, srdb,
					     match->flags,
					     IPX_ROUTE_EVENT_CHANGE);
		    novell_mark_sap_changed(match);
		}
		return;
	    }
	    srdb_prev = srdb;
	}
	/* If we are this far, the path was not found */
	/* Possible multiple if all details are the same */
	if ((nethops < novell_infinity) &&
	    nov_sap_distance_compare(server, route_delay, route_hops,
				     route_flags, sap_flags,
				     &(match->server),
				     match->route_delay,
				     match->route_hops,
				     match->route_flags, match->flags,
				     NOV_SAP_CMP_EQ) &&
	    ((match->server.dnet == server->dnet) &&
	     (ieee_equal(server->dhost, match->server.dhost)) &&
	     (match->server.dsock == server->dsock))) {
	    /* If we already have novell_maxpaths entries we can't accomodate
	     * this new information. (NLSP is an exception: all paths must
	     * be kept. This cannot be changed without saving state to either
	     * trigger a full SPF or a series of partial SPF's to recover
	     * path information which may have been discarded.)
	     */
	    if ((match->pathcount >= novell_maxpaths) &&
		(!(match->flags & NET_NLSP))) {
		return;
	    }
	    srdb = malloc_named(sizeof(novell_sapentry_rdb), "IPX SRDB");
	    if (srdb == NULL) {
		return;
	    }
	    srdb->sap = match;
	    srdb->nidb = nidb_input;
	    srdb->snet = snet;
	    if (shost == NULL) {
		ieee_zero(srdb->srchost);
	    } else {
		ieee_copy(shost, srdb->srchost);
	    }
	    if (novellsap_debug || novellsap_event_debug)
		buginf("\nIPXSAP: adding path for type %x"
		       " server \"%s\" %d hops [%d/%d], via %s:%x.%e",
		       server->server_type, server->server_name,
		       nethops, route_hops, route_delay,
		       (srdb->nidb ? srdb->nidb->idb->short_namestring : NULL),
		       srdb->snet, srdb->srchost);

	    if (ndebug && (nlsp_cost != NULL)) {
		buginf("\nNLSP-SAP: OOP=%u:%u:%u:%e:%u:%e",
		       nlsp_cost->total_throughput,
		       nlsp_cost->total_delay,
		       nlsp_cost->mtu,
		       &nlsp_cost->system_id[0],
		       nlsp_cost->circuit_id,
		       &nlsp_cost->mac_addr[0]);
	    }
	    novell_set_timer(NULL, srdb);
	    novell_mark_srdb_changed(match, srdb, match->flags,
				     IPX_ROUTE_EVENT_ADD);
	    /*
	     * Update path list ... if NLSP, sort using order of preference:
	     */
	    if (nlsp_cost == NULL) {
		srdb->next = match->paths;
		match->paths = srdb;
	    } else {
		bcopy(nlsp_cost, &srdb->cost, sizeof(nov_cost_type));
		last_srdb = (novell_sapentry_rdb *)&match->paths;
		next_srdb = match->paths;
		while ((next_srdb != NULL) &&
		       REG_INVOKE_NLSP_WORSE_COST(nlsp_cost,
						  &next_srdb->cost)) {
		    last_srdb = next_srdb;
		    next_srdb = next_srdb->next;
		}
		last_srdb->next = srdb;
		if (next_srdb != NULL) {
		    srdb->next = next_srdb;
		}
	    }
	    match->pathcount++;
	    /*
	     * Update per-LSP info if NLSP learned service:
	     */
	    if (vdebug) buginf("\nadds: new srdb");
	    if (lsp_lh != NULL) {
		srdb->lsp_linkage.lsp_version = lsp_version;
		srdb->lsp_linkage.lsp_index = lsp_index;
		reg_invoke_ipx_nlsp_insert_srdb_in_lsp_list(srdb, lsp_lh);
		if (pdebug) {
		    buginf(" lsp %d/%d, lh=%x->%x"
			   ", srdb=%x (%x/%x)",
			   lsp_index, lsp_version, lsp_lh, *lsp_lh,
			   srdb, srdb->lsp_linkage.lsp_next, srdb->lsp_linkage.lsp_prev);
		}
	    }
	} else {
	    /*
	     * We have chosen to ignore this advertisement. Put it in the
	     * backup table, possibly...(If not a poison update)
	     */
	    ipx_insert_server_backup_info(match, &backup, pdb, nethops,
					  nidb_input, sap_flags);
	}
    }
}

/*
 * nov_echo
 * Handle echo protocol
 */

void nov_echo (paktype *pak)
{
    ipxhdrtype *ipx;
    ulong tnet;
    ushort tsock;
    uchar thost[IEEEBYTES];
    nidbtype *nidb;
    ipx_stnd_ping *sp;
    ipx_cisco_ping *cp;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    nidb = pak->nif_input;
    novell_stat[NOVELL_ECHOREQIN]++;
    tnet = GET2WD(ipx->ssnet);
    /* Don't swap source/destination if this is from our internal network */
    if ((ipx_internal_net == 0) || (tnet != ipx_internal_net)) {
      ieee_copy(ipx->shost, thost);
      tsock = ipx->ssock;
      PUT2WD(ipx->ssnet,
	     (((GET2WD(ipx->ddnet) != -1L) && GET2WD(ipx->ddnet)))
	     ? GET2WD(ipx->ddnet) : nidb->novellnet ? nidb->novellnet :
	     ipx_internal_net);
      if (nov_multicast_addr(nidb->idb->hwptr, ipx->dhost[0]))
	ieee_copy(nidb->idb->novell_node, ipx->shost);
      else
	ieee_copy(ipx->dhost, ipx->shost);
      ipx->ssock = ipx->dsock;
      PUT2WD(ipx->ddnet, tnet);
      ieee_copy(thost, ipx->dhost);
      ipx->dsock = tsock;
    }
    ipx->tc = 0;
    pak->linktype = LINK_NOVELL;
    /* pak->cksum = pak->cksum; *//* Return incoming checksum */
    if (ipx->dsock == NOVELL_SOCK_ECHO) {
      ipx->pt = NOVELL_ECHO_REPLY;
      cp = (ipx_cisco_ping *)ipxdatastart(pak);
      cp->type = NOVELL_ECHO_REPLY;
    } else {
      ipx->pt = NOVELL_TYPE_ECHO_STND;
      sp = (ipx_stnd_ping *) ipxdatastart(pak);
      sp->type_byte = NOVELL_TYPE_ECHO_RPLY;
      if(GETSHORT(&ipx->len) >= NOVELL_ECHO_STD_SIZE) {
	/*
	 * WARNING: Someday when we do checksums this will mean recalculating
	 * the checksum as we are changing a data byte.
	 */
	sp->result_byte = 1;  /* found a data area */
      } else {
	sp->result_byte = 0;  /* No data area found */
      }
    }
    clear_if_input(pak);
    if (!nov_send(pak)) {
      if (novell_debug) {
	buginf("\nIPXECHO: Can't send echo");
      }
    } else {
      novell_stat[NOVELL_ECHOREPOUT]++;
    }    
}

/*
 * novell_printrippak
 * BugPrint the contents of a Novell RIP packet.
 */
void novell_printrippak (paktype *pak)
{
     novellriptype *rip = (novellriptype *) (XNSDATASTART(pak));
     ipxhdrtype *ipx;
     int size;
     int i;

     ipx = (ipxhdrtype *)ipxheadstart(pak);

     size = GETSHORT(&ipx->len) - (XNSOVERHEADBYTES + NOVELL_RIP_HEADERSIZE);
     for (i = 0; i < size/(sizeof(novellripinfotype)); i++) {
	 buginf("\n    network %x, hops %d,  delay %d",
		GET2WD(rip->nets[i].nnet), rip->nets[i].metric,
		rip->nets[i].delay);
     }
}

/*
 * novell_printpak
 * BugPrint the contents of a Novell SAP packet.
 */

void novell_printpak (paktype *pak, char *string)
{
    novell_sappacket *sap = (novell_sappacket *)(XNSDATASTART(pak));
    ipxhdrtype *ipx;
    int size;
    int i;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    size = GETSHORT(&ipx->len) - (XNSOVERHEADBYTES + 2);

    buginf("\nIPXSAP: %s type %#x len %d src:%x.%e dest:%x.%e(%x)",
	   string, sap->response_type, GETSHORT(&ipx->len), GET2WD(ipx->ssnet),
	   ipx->shost, GET2WD(ipx->ddnet),ipx->dhost,GETSHORT(&ipx->dsock));
    for (i = 0; i < size/(sizeof(novell_serveridtype)); i++) {
	buginf("\n type %#x, \"%s\", %x.%e(%x), %d hops",
	    sap->server[i].server_type,
	    sap->server[i].server_name,
	    sap->server[i].dnet, sap->server[i].dhost, sap->server[i].dsock,
	    sap->server[i].nethops);
    }
}

/*
 * novell_printnamepak
 * BugPrint the contents of a Novell SAP name query packet.
 */

void novell_printnamepak (paktype *pak, char *string)
{
    novell_querynametype *sap = (novell_querynametype *)(XNSDATASTART(pak));
    ipxhdrtype *ipx;
    int size;
    int i;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    size = ipx->len - (XNSOVERHEADBYTES + 2);

    buginf("\nIPXSAP: %s type %#x len %d src:%x.%e dest:%x.%e(%x)",
	string,
	sap->query_type, ipx->len, GET2WD(ipx->ssnet), ipx->shost,
	GET2WD(ipx->ddnet),ipx->dhost,ipx->dsock);
    for (i = 0; i < size/(sizeof(novell_servernametype)); i++) {
	buginf("\n   type %#x, \"%s\", network %x mask %x",
	    sap->server[i].server_type,
	    sap->server[i].server_name,
	    sap->server[i].dnet, sap->server[i].dmask);
    }
}

/*
 * novell_forwarder
 * Handle packets on the generic queue.
 */
forktype novell_forwarder (void)
{
    register paktype *pak;
    ipxhdrtype *ipx;
    xnsechotype *ep;
    ipx_cisco_ping *cp;
    ipx_stnd_ping *sp;
    sys_timestamp time_sent;
    int count;

    edisms((blockproc *)systeminitBLOCK, (ulong)&novell_genericQ);
    process_set_priority(PRIO_LOW);

    while (TRUE) {
        edisms((blockproc *)queueBLOCK, (long)&novell_genericQ);
	count = PAK_SWITCHCOUNT;
	/*
	 * Process a limited number of packets from the generic queue.
	 */
	while (TRUE) {
	    if (--count == 0) {
		break;
	    }
	    if (!(pak = dequeue(&novell_genericQ))) {
		break;
	    }		
	    ipx = (ipxhdrtype *)ipxheadstart(pak);

	    if ((ipx->dsock == NOVELL_SOCK_ECHO) ||
		(ipx->dsock == NOVELL_SOCK_ECHO_STND)) {
		/* handle either type of IPX echo (cisco, Novell) */
		/* Don't do echoes from network 0, on an nidb with net */
		/* equal to zero (unnumbered). We can't route back.    */
		sp = (ipx_stnd_ping *) ipxdatastart(pak);
		if (((((ipx->pt == NOVELL_ECHO_REQUEST) &&
		       (ipx->dsock == NOVELL_SOCK_ECHO))) || 
		     ((sp->type_byte == NOVELL_TYPE_ECHO_RCV) &&
		      (ipx->dsock == NOVELL_SOCK_ECHO_STND))) &&
		    (ipx->ssnet || pak->nif_input->novellnet)) {
		    nov_echo(pak);
		    continue;
		} else if (!QUEUEEMPTY(&novechoQ)) {		
		    novell_stat[NOVELL_ECHOREPIN]++;
		    for (ep = (xnsechotype *)novechoQ.qhead; ep;
			 ep = ep->next) {
			if (ep->socket == ipx->dsock) {
			    if (ipx->dsock == NOVELL_SOCK_ECHO) {
				if (GETSHORT(&ipx->len) >= XNS_MINECHOSIZE) {
				    cp = (ipx_cisco_ping *)ipxdatastart(pak);
				    PAK_GET_TS(&cp->time_sent, &time_sent);
				    ep->deltatime = ELAPSED_TIME(time_sent);
				} else {
				    ep->deltatime = 0;
				}
			    } else {
				if (GETSHORT(&ipx->len) >= NOVELL_ECHO_STD_SIZE) {
				    PAK_GET_TS(&sp->time_sent, &time_sent);
				    ep->deltatime = ELAPSED_TIME(time_sent);
				} else {
				    ep->deltatime = 0;
				}
			    }
			    novecho_testreply(pak, ep);
			    ep->active = FALSE;
			    p_unqueue(&novechoQ, ep);
			    break;
 			}
 		    }
 		}
	    } else {
 		if (!novell_forward(pak)) {
 		    if (novell_debug)
 			buginf("\nIPX: unable to forward from %x.%e(%x) to %x.%e(%x) typ %x",
 			       GET2WD(ipx->ssnet), ipx->shost, ipx->ssock,
 			       GET2WD(ipx->ddnet), ipx->dhost, ipx->dsock,
			       ipx->pt);
 		    novell_stat[NOVELL_UNKNOWN]++;
 		    retbuffer(pak);
 		}
 		continue;
 	    } /* switch on Novell type */
	    retbuffer(pak);		    
  	    (void) process_may_suspend();
	}
	(void) process_may_suspend();
    }
}

/*
 * novell_routeBLOCK
 * Block waiting for something for the novell RIP router process to do
 */

boolean novell_routeBLOCK (queuetype *q)
{
    if (!novell_running || (q == NULL))
	return(TRUE);			/* never if not running */
    if (q->qhead)
	return(FALSE);			/* input available */
    if (!SLEEPING(novell_ripagertimer)) {
	return(FALSE);
    }
    if (TIMER_RUNNING_AND_AWAKE(novell_spxagertimer)) {
	return(FALSE);                  /* spx ager timer expired */
    }
    if (!SLEEPING(novell_onemin_timer)) {
	return(FALSE);
    }
    return(TRUE);
}

/*
 * novell_router
 *
 * Novell RIP router process. Perform routing maintenance functions.
 *
 * Note that the RIP process handles RIP requests which are required
 * by end hosts even if only NLSP/EIGRP are running. Meanwhile the RIP
 * process has been modified to NOT generate periodic updates and to
 * drop incoming updates if novell_rip_running is FALSE. Breaking this
 * process up into its component parts might be a good idea.
 */

forktype novell_router (
     novpdbtype *pdb)
{
    register paktype *pak;
    ipxhdrtype *ipx;
    register novellriptype *rip;

    edisms((blockproc *)systeminitBLOCK, (ulong)&pdb->inputq);
    process_set_priority(PRIO_LOW);
    novell_routes = 0;

    while (TRUE) {

	edisms((blockproc *)novell_routeBLOCK, (long)&pdb->inputq);

	/*
	 * First empty the RIP packet queue
	 */
	while ((pak = dequeue(&pdb->inputq))) {
	    ipx = (ipxhdrtype *)ipxheadstart(pak);
	    pak->if_input->nov_rip_in_packets++;
	    rip = (novellriptype *)ipxdatastart(pak);
	    /*
	     * Pitch everything if we are shutting down:
	     */
	    if (!novell_running || novell_shutting_down) {
		retbuffer(pak);		    
		continue;
	    }
 	    /*
 	     * Discard incoming RIP packets if NLSP/ISIS is running
 	     * on input interface and an adjacency exists.
 	     */
 	    if ((!pak->nif_input->nov_lsp_enabled) ||
 		(!reg_invoke_ipx_nlsp_valid_adjacency(pak, ISIS_CIRCUIT_L1))) {
 		if ((novellisis_debug || novellnlsp_debug) &&
		    (novellrip_debug || novellrip_event_debug))
 		    buginf("\nIPXRIP: Accepting RIP packet from %x.%e (%s)",
 			   GET2WD(ipx->ssnet), ipx->shost,
 			   pak->if_input->short_namestring);
		switch (rip->cmd) {
		  case RIP_REQUEST:
		    novell_stat[NOVELL_RIPREQIN]++;
		    novell_rcvriprequest(pak, rip);
		    break;
		  case RIP_REPLY:
		    novell_stat[NOVELL_RIPRESIN]++;
		    if (novell_rip_running) {
			novell_riprcvupd(pak, rip, novellrip_debug);
		    }
		    break;
		  default:
		    if (novellrip_debug || novellrip_event_debug)
			buginf("\nIPXRIP: unknown type %d, %x.%e",
			       rip->cmd, GET2WD(ipx->ssnet),
			       ipx->shost);
		    novell_stat[NOVELL_RIPFMTERROR]++;
		    break;
		}
 	    } else {
 		if ((novellisis_debug || novellnlsp_debug) &&
		    (novellrip_debug || novellrip_event_debug))
 		    buginf("\nIPX-ISIS: Adjacency exists"
 			   ", discarding RIP packet"
 			   " from %x.%e (%s)",
 			   GET2WD(ipx->ssnet), ipx->shost,
 			   pak->if_input->short_namestring);
  	    }
	    retbuffer(pak);		    
  	    (void) process_may_suspend();
	}
	if (!SLEEPING(novell_ripagertimer)) {
	    TIMER_START(novell_ripagertimer, 30*ONESEC);
	    novrip_ager();
	    novrip_age_backup_routes();
	    process_may_suspend();
	}
	if (TIMER_RUNNING_AND_AWAKE(novell_spxagertimer)) {
	    TIMER_START(novell_spxagertimer, 15*ONEMIN);
	    novspx_ager();
	}
	if (!SLEEPING(novell_onemin_timer)) {
	    TIMER_START(novell_onemin_timer, ONEMIN);
	    nov_fscache_age();
	    process_may_suspend();
	    nov_accesslog_cache_print();
	}
    }
}

/*
 * novell_riprcvupd
 * Process a Novell RIP packet.  Note that novell rip packets contain
 * a "delay" field in addition to the hop count.  Units are IBM PC ticks.
 * Per the Novell spec, we prefer routes with a better delay, and use
 * the hop count, then the internal, cisco interface delay, to break ties.
 *
 * The NLSP spec has a very strange rule:
 *	Entries may be in the table with >= 16 hops and be valid, but on
 *      GENERAL RIP advertisement, we are to never send out those with more
 *	than 15 hops. HOWEVER, we are to retain, in our table, those which
 *      have >15 hops, for advertising in response to specific-net RIP
 *	requests. But, on RECEIVING any RIP update, we are to consider
 *	the routes learned with 16 hopcount as infinity.
 *
 *	The easiest way to implement this is to, upon reading a RIP update,
 *	convert all hopcount 16 routes to our notion of infinity before
 *	processing, and upon advertising general info, don't send any
 *	info on hopcount >15 routes that aren't in holddown.
 *
 *	Sigh.
 */
void
novell_riprcvupd (
    paktype *pak,
    novellriptype *rip,
    boolean debug)
{
    ipxhdrtype *ipx;
    novellripinfotype *p;
    nidbtype *nidb;
    int nentries;
    ulong net, sourcenet;
    boolean was_off;
    int list;
	/* make distance a global and configurable. This is just a hack */

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    nentries = (ipx->len - XNSHEADERBYTES - NOVELL_RIP_HEADERSIZE) /
	sizeof(novellripinfotype);
    nidb = pak->nif_input;

    /* Do we believe anything from this router */
    if (!nov_accesscheck(pak, nidb->idb->nov_router_filterlist)) {
      if (debug)
	buginf("\nIPXRIP: update from %x.%e filtered",
	       GET2WD(ipx->ssnet), ipx->shost);
      return;
    }
    p = (novellripinfotype *)rip->nets;
    if (debug)
	buginf("\nIPXRIP: update from %x.%e",
	       GET2WD(ipx->ssnet), ipx->shost);
    /*
     * If ISIS/NLSP circuit, check that source == destination address.
     * If so this packet will reactivate RIP compatibility mode if we
     * are currently set to AUTO.
     */
    if (nidb->nov_lsp_enabled) {
	if (GET2WD(ipx->ssnet) != GET2WD(ipx->ddnet)) {
	    if (debug && (novellisis_debug || novellnlsp_debug))
		buginf("\nIPX-%s: Discard bad rip update from %x.%e"
		       ", snet=%x, dnet=%x",
		       (nidb->nov_lsp_isis ? "ISIS" : "NLSP"),
		       GET2WD(ipx->ssnet), ipx->shost,
		       GET2WD(ipx->ssnet), GET2WD(ipx->ddnet));
	    novell_stat[NOVELL_RIPFMTERROR]++;
	    return;
	}
	was_off = (!nidb->lsp_rip_on && (nidb->lsp_rip_cfg ==
					 IPX_RIPSAP_CONFIG_AUTO));
	novell_start_nidb_rip_auto(nidb);
	novell_start_nidb_sap_auto(nidb);
	if (was_off) {
	    if (debug && (novellisis_debug || novellnlsp_debug))
	      buginf("\nIPX-%s: activate RIP on net %x",
		     (nidb->nov_lsp_isis ? "ISIS" : "NLSP"),
		     nidb->novellnet);
	    novell_posit_full_rip_update(nidb);
	}
	if (nidb->lsp_rip_cfg == IPX_RIPSAP_CONFIG_OFF) {
	    /*
	     * RIP is off ... discard update:
	     */
	    if (debug && (novellisis_debug || novellnlsp_debug))
	      buginf("\nIPXRIP: Discard rip update,"
		     " %s-RIP compatibility mode off",
		     (nidb->nov_lsp_isis ? "ISIS" : "NLSP"));
	    return;
	}
    }
    sourcenet = GET2WD(ipx->ssnet);
    /*
     * Some Novell conformance testers are known to send periodic updates
     * with a valid source network number, while sending poison updates
     * with source network 0. Though brain dead, this seems to be perfectly
     * legal per the spec. Handle this special case. Note that an unnumbered
     * link should not be affected since nidb->novellnet should always be
     * 0 in this case.
     */
    if (sourcenet == 0) {
	sourcenet = nidb->novellnet;
    }
    /*
     * If this network is not listed in the router subcommand 
     * "network", then do not listen to RIP on this interface. 
     * All novell nets on the router should be listed in 
     * novrip_pdb.networks[] by default.
     */
    if (!novell_rip_network_enable((nidbtype *)pak->nif_input)) {
	if (debug)
	    buginf("\nIPXRIP: RIP turned off, discarding RIP packet",
		   " from %x.%e (%s)", sourcenet, ipx->shost,
		   pak->if_input->short_namestring);
	return;
    }
    while (nentries-- > 0) {
	/* Do we want to hear anything about this destination net ? */
	net = GET2WD(p->nnet);
	list = novrip_pdb.ifltr_all;
	if (!list)
	    list = nidb->iprotofltr[novrip_pdb.index];
	if (nov_filtercheck(net, nidb->idb->nov_in_filterlist) && 
	    (!list || nov_filtercheck(net, list))) {
	    if (net != 0) {
		novrip_update(nidb, net, NOV_NETMASK_ALL, p->metric, sourcenet,
			      ipx->shost, p->metric, p->delay, NULL);
		if (debug)
		  buginf("\n    %x in %d hops, delay %d",
			 net, p->metric, p->delay);
	    }
	}
	p++;
    }
}

/*
 * novell_rcvriprequest
 * Handle a RIP request for routing information
 */

void
novell_rcvriprequest (paktype *pak, novellriptype *rip)
{
    ipxhdrtype *ipx, *newipx;
    paktype *newpak;
    novellriptype *newrip;
    novellripinfotype *newp;
    novellripinfotype *pakp;
    novndbtype *entry;
    int bytes;
    int length;
    boolean split; 
    nidbtype *nidb = pak->nif_input;
    ulong target_net;
    boolean debug = (novellrip_debug || novellrip_event_debug);
    boolean vdebug = (novell_verbose_debug);

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    if (GET2WD(ipx->ssnet) == 0)
	PUT2WD(ipx->ssnet, nidb->novellnet);
    if (debug) buginf("\nIPXRIP: received request from %x.%e",
		      GET2WD(ipx->ssnet), ipx->shost);
    if (ipx->len == XNSHEADERBYTES + sizeof(novellriptype) &&
	GET2WD(rip->nets[0].nnet) == -1) {
	if (debug) buginf(" for all routes");
	/*
	 * Respond to general request if not running ISIS/NLSP or
	 * if compatibility mode is auto or on. Note that receipt
	 * of a general request does not activate compatibilty mode.
	 */
	novell_posit_rip_update(pak->nif_input,
				GET2WD(ipx->ssnet),
				ipx->shost,
				ipx->ssock,
				IPX_FULL_NEEDED,
				IPX_DESTINATION_RIPSAP_UNICAST,
				FALSE, FALSE);
	return;
    }
    /*
     * Sanity check the incoming RIP packet. If it looks malformed, print a
     * warning message and bail...
     */
    if ((ipx->len > pak_get_pool_size(pak)) ||
	(((ipx->len - XNSOVERHEADBYTES) - NOVELL_RIP_HEADERSIZE) %
	 sizeof(novellripinfotype))) {
	nov_log_stats(NOVELL_RIPFMTERROR, pak);
        novell_stat[NOVELL_RIPFMTERROR]++;
	if (debug) {
	    buginf("\nIPX: malformed RIP request packet; tossing! Data follows...");
	    buginf("\nIPX: snet = %x, shost = %e, input interface = %s",
		   ipx->ssnet, ipx->shost, pak->if_input->namestring);
	    buginf("\nIPX: len from header = %d, buffer size = %d",
		   ipx->len, pak_get_pool_size(pak));
	}
	return;
    }
    newpak = pak_duplicate(pak);
    if (newpak == NULL)
      return;
    if (pak->flags & PAK_BROADCAST)
      split = TRUE;
    else
      split = FALSE;

    newipx = (ipxhdrtype *)ipxheadstart(newpak);

    newpak->if_output = pak->if_input;
    newpak->nif_output = pak->nif_input;
    newipx->tc = 0;
    PUT2WD(newipx->ssnet, pak->nif_input->novellnet);
    ieee_copy(ipx->shost, newipx->dhost);
    newipx->ssock = NOVELL_SOCK_RIP;

    ieee_copy(pak->if_input->novell_node, newipx->shost);
    PUT2WD(newipx->ddnet, GET2WD(ipx->ssnet));
    newipx->dsock = ipx->ssock;
    newrip = (novellriptype *) XNSDATASTART(newpak);
    newrip->cmd = RIP_REPLY;
    newp = newrip->nets;
    pakp = rip->nets;
    target_net = GET2WD(pakp->nnet);
    if (vdebug) buginf("\nIPXRIP:  for %x", target_net);
    bytes = newipx->len - XNSOVERHEADBYTES;
    length = 0;
    while (bytes > NOVELL_RIP_HEADERSIZE) {
	entry = novrip_find_rt(target_net, NOV_NETMASK_ALL, 
			       IPX_ROUTE_MATCH_EXACT);
	if ((entry == NULL) && (novell_default_route)) {
	    /*
	     * No route .. return anyway if default route is active/known:
	     */
	    entry = novrip_find_rt(NOV_RIP_DEFAULT_ROUTE, NOV_NETMASK_ALL,
				   IPX_ROUTE_MATCH_EXACT);
	}
	PUT2WD(newp->nnet, target_net);
	newp->metric = novell_infinity;
	newp->delay = NOVELL_INFDELAY;
	if (entry != NULL) {
	    if (vdebug) buginf(", match %x", entry->dstnet);
	    if (novrip_out_filter(entry, target_net, pak->if_input))
		if (!split ||
		    !nov_same_nidb(pak->nif_input, entry->path_list) ||
		    ((entry->flags & NET_CONNECTED) &&
		     (entry->dstnet == pak->nif_input->novellnet))) {
		    PUT2WD(newp->nnet, target_net);
		    if (!TIMER_RUNNING(entry->holddownuntil)) {
			nov_get_rip_metrics(entry, newp, nidb);
		    } else {
			newp->metric = NOVELL_RIP_ADV_INFINITY;
		    }
		    if (!(entry->flags & NET_IGRP)) 
		        newp->delay = entry->delay +
		            NOVELL_IDB_TICKS(pak->nif_input);

		    if (novellrip_debug) {
			buginf("\n    Net %x reachable in %d hops",
			       GET2WD(newp->nnet), newp->metric);
			if ((entry->dstnet == NOV_RIP_DEFAULT_ROUTE) &&
			    novell_default_route &&
			    (entry->dstnet != target_net)) {
			    buginf(", via default route");
			}
		    }
		    newp++;
		    length  += sizeof(novellripinfotype);
		} else {
		    if (vdebug) buginf(", rejected");
		}
	} else {
	    if (vdebug) buginf(", no match");
	}
	bytes -= sizeof(novellripinfotype);
	pakp++;
    }
    if (length == 0) {
	if (novellrip_debug || novellrip_event_debug)
	  buginf("\nIPXRIP: suppressing null response");
	retbuffer(newpak);
	return;
    }	

    newipx->len = XNSOVERHEADBYTES + NOVELL_RIP_HEADERSIZE + length;
    newipx->cksum = NOVELL_NO_CHECKSUM;
    pak->flags |= PAK_PRIORITY;
    if (nov_send1(newpak, ipx->shost, newipx->len)) {
      /* Received packet's interface is the output interface of response */
      pak->if_input->nov_rip_out_packets++;
      novell_stat[NOVELL_RIPREPLY]++;
      if (novellrip_debug || novellrip_event_debug)
	buginf("\nIPX: src=%x.%e, dst=%x.%e, response sent",
	       pak->nif_input->novellnet,
	       pak->if_input->novell_node,
	       GET2WD(ipx->ssnet), ipx->shost);
    }    
    /* Pak will be returned by caller */
}

/*
 * novell_show_int_brief
 *
 * Quick one-line summaries of the IPX status of an idb, or all idbs.
 *
 */
static void
novell_show_int_brief (idbtype *idb)
{
    boolean	once = FALSE;		/* TRUE if we print one idb */
    idbtype	*iidb;
    hwidbtype	*hwidb;
    nidbtype    *nidb;
    char	addrbuf[11];
    boolean     found_nidb;

    printf("\n%20s %11s %13s %22s %9s", "Interface", "IPX Network",
	"Encapsulation", "Status", "IPX State");

    iidb = (idb != NULL) ? idb : SWIDBQ_HEAD;
    for ( ; !once && iidb; iidb = IDB_NEXT(iidb)) {

        hwidb = iidb->hwptr;
	if (hwidb == NULL)
	    continue;

	/* Deleted interfaces include tunnels that have been deleted. Ugh! */
	if (hwidb->status & IDB_DELETED)
	    continue;
	

	once = (idb != NULL);
	if (iidb->novellnets) {
	    found_nidb = FALSE;
	    FOR_ALL_NIDBS(nidb) {
		if (nidb->idb == iidb) {
		    found_nidb = TRUE;
		    printf("\n%20s ", iidb->namestring);

		    if (!NOVELL_NIDB_UNNUMBERED(nidb)) {
			sprintf(addrbuf, "%x", nidb->novellnet);
		    } else {
			sprintf(addrbuf, "unnumbered");
		    }
		    printf("%11s ", addrbuf);
		    /* Pick up WAN encaps from hwidb */
		    printf("%13s ",
			   encstring(((nidb->enctype == ET_HDLC) ?
				      hwidb->enctype : nidb->enctype)));
		    printf("%22s ", print_idbstate(iidb));
		    printf("%s", novell_nidb_print_state(nidb));
		}
	    }
	    if (!found_nidb) {
		if (iidb->owner_nidb && !NOVELL_NIDB_UNNUMBERED(iidb->owner_nidb)) {
		    sprintf(addrbuf, "%x", iidb->owner_nidb->novellnet);
		} else {
		    sprintf(addrbuf, "unnumbered");
		}
		printf("\n%20s ", iidb->namestring);
		printf("%11s %13s ",addrbuf, "ppp-client    ");
		printf("%22s n/a", print_idbstate(iidb));
	    }
	} else {
	    printf("\n%20s ", iidb->namestring);
	    printf("%11s %13s ", "unassigned", "not config'd  ");
	    printf("%22s n/a", print_idbstate(iidb));
	    continue;
	}
    }
}

/*
 * show_novell
 * Display Novell Protocol information
 */

void show_novell (parseinfo *csb)
{
    if (!novell_running || novell_shutting_down) {
	printf(nov_notenabled);
	return;
    }
    switch (csb->which) {

      case SHOW_NOVELL_INTERFACE:
	automore_enable(NULL);
	if (GETOBJ(int,2)) {
	    novell_show_int_brief(GETOBJ(idb, 1));
	} else {
	    show_swidb_chain(GETOBJ(idb,1), display_novell, 1);
	}
	automore_disable();
	break;

      case SHOW_NOVELL_BACKUP:
	novrip_display_backup_routes(GETOBJ(int,1), GETOBJ(int,2));
	break;

      case SHOW_NOVELL_URD:
	nov_display_urd_table(GETOBJ(int,1));
	break;

      case SHOW_NOVELL_ROUTE:
	show_novellroute(GETOBJ(int,1), GETOBJ(int,2));
	break;

      case SHOW_NOVELL_TRAFFIC:
	automore_enable(NULL);
	show_novelltraffic();
	automore_disable();
	break;

      case SHOW_NOVELL_SERVERS:
	show_novell_servers(GETOBJ(int,1), GETOBJ(int,2), GETOBJ(string,3));
	break;

      case SHOW_NOVELL_CACHE:
	if (GETOBJ(int,1) == 1)
	    reg_invoke_auto_sw_ipx_cache_show();
	else if (GETOBJ(int,1) == 2)
	    show_nov_cache_hash_table();
	else
	    show_nov_cache();
	break;

      case SHOW_NOVELL_SPX_SPOOF:
	show_novell_spx_spoof();
	break;

      case SHOW_NOVELL_ACCESS:
	automore_enable(NULL);
	show_novaccess(GETOBJ(int,1));
	automore_disable();
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * test_novell_capacity
 * Create N static routes, and one server for each. Put them as
 * existing next hop off of the network number given. If routes > servers,
 * only the fist 'servers' routes get a server. If servers > routes,
 * the last route gets all the remaining services.
 *
 * GETOBJ(int,1) total number of routes
 * GETOBJ(int,2) network number to start with. 
 * GETOBJ(int,3) network number to attach routes to.
 * GETOBJ(int,4) total number of servers
 */
static forktype novell_capacity_fork (parseinfo *csb)
{
    int i;

    for (i = 0; ((capacity_routesleft > 0) ||
		 (capacity_serversleft > 0)); i++) {
	if ((i % 100) == 0) {
	    buginf("\nIPX CAPACITY: Done with %d items...", i);
	    process_sleep_for(ONESEC * 5);
	} else {
	    /* Force an orderly introduction of items... */ 
	    process_sleep_for(1);
	}
	if (capacity_routesleft > 0) {
	    nov_route_command(FALSE, TRUE, capacity_routesleft +
			      capacity_startroute,
			      capacity_nexthop_net, capacity_host, 
			      NOVELL_DEFAULT_CONNECTED_TICKS,
			      NOVELL_DEFAULT_CONNECTED_HOPS, 0,
			      NULL, FALSE, NOV_NETMASK_ALL, 0,
			      IPX_STATIC_DEFINE_TYPE_NO_NVGEN);
	    capacity_routesleft--;
	}
	if (capacity_serversleft > 0) {
	    sprintf(capacity_servername, "%s-CTEST%06x", capacity_ourhost,
		    capacity_startroute + capacity_serversleft);
	    nov_sap_command(FALSE, TRUE, capacity_servername, 4,
			    capacity_startroute +
			    (capacity_routesleft > 0 ?
			     capacity_routesleft : 1),
			    capacity_host, 1, 1,
			    IPX_STATIC_DEFINE_TYPE_NO_NVGEN);
	    capacity_serversleft--;
	}
    }
    capacity_fork = 0;
    process_set_analyze(FALSE);
    process_kill(THIS_PROCESS);
}

static void test_novell_capacity (parseinfo *csb)
{

    if (capacity_fork != 0) {
	printf("\nSorry, the capacity command is already running");
	return;
    }
    capacity_routesleft = GETOBJ(int,1);
    capacity_startroute = GETOBJ(int,2);
    capacity_nexthop_net = GETOBJ(int,3);
    capacity_serversleft = GETOBJ(int,4);
    sstrncpy(capacity_ourhost, hostname, 27);
    ieee_zero(capacity_host);
    capacity_host[IEEEBYTES - 1] = 0x01;
    capacity_fork = cfork((forkproc *) novell_capacity_fork, 0L, 0,
			  "IPX CAPACITY", 0);
}

/*
 * test_novell
 * Display Novell Protocol information
 */

void
test_novell (parseinfo *csb)
{
    if (!novell_running || novell_shutting_down) {
	printf("%% IPX not enabled");
	return;
    }
    switch (csb->which) {

      case TEST_NOVELL_CAPACITY:
	test_novell_capacity(csb);
	break;

      case TEST_NOVELL_GNS:
	test_novell_gns(csb);
	break;

      case TEST_NOVELL_NETBIOS_FINDNAME:
	test_novell_netbios(csb);
	break;
	
      case TEST_NOVELL_ECHO:
 	test_novell_echo(csb);
 	break;
	
      case TEST_NOVELL_DEBUG:
 	test_novell_debug(csb);
	break;
	
      case TEST_NOVELL_WATCHDOG:
 	test_novell_watchdog(csb);
	break;

      case TEST_NOVELL_RIPREQ:
	test_novell_ripreq(csb);
	break;

      case TEST_NOVELL_NAME:
	test_novell_name(csb);
	break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

/*
 * Send out a Novell SAP get nearest server request (nearest service query).
 *
 * Format of the command WAS:
 *
 *     show novell neartest <type>
 *
 * but is NOW:
 *
 *     test novell gns [<type> [<count> [<interval> [<net>]]]]
 */

static void test_novell_gns (parseinfo *csb)
{
    ushort service;
    int i,count;
    int interval;
    ulong net;
    paktype *pak, *newpak;
    ipxhdrtype *ipx, *newipx;
    novell_sappacket *sap;
    nidbtype *nidb;
    boolean no_itf;
    int col  = 0;

    service  = GETOBJ(int,1);
    count    = GETOBJ(int,2);
    interval = GETOBJ(int,3);
    net      = GETOBJ(int,4);
    no_itf   = TRUE;

    printf("\nRequest nearest service (type %d) via:", service);

    FOR_ALL_NIDBS(nidb) {
	if ((nidb != NULL) && (nidb->idb != NULL) && NOVELL_NIDB_IDB_UP(nidb) &&
	    ((net == -1) || (net == 0) || (nidb->novellnet == net))) {
	    printf("\n - net %x, interface %s",
		   nidb->novellnet, nidb->idb->namestring);
	    no_itf = FALSE;
	}
    }
    
    if (no_itf) {
	printf("\n%%No active interface matching net %x", net);
	return;
    }

    psion(stdio); /* Allow user to escape */

    for (i=0; i<count; i++) {
	if (i > 0)
	    process_sleep_for(interval);
	pak = getbuffer(MEDDATA);
	if (pak) {
	    ipx = (ipxhdrtype *)ipxheadstart(pak);

	    pak->linktype = LINK_NOVELL;
	    ipx->tc = 0;
	    ipx->pt = NOVELL_TYPE_SAPRESP;
	    ipx->len = NOVELL_QUERYLENGTH;
	    ipx->dsock = NOVELL_SOCK_SAP;
	    ipx->ssock = NOVELL_SOCK_SAP;
	    ipx->cksum = NOVELL_NO_CHECKSUM;    /* Force 0xFFFF */

	    sap = (novell_sappacket *)(XNSDATASTART(pak));
	    sap->response_type = NOVELL_SAP_NEARQUERY;
	    sap->server[0].server_type = service;

	    FOR_ALL_NIDBS(nidb) {
		if ((nidb != NULL)  && (nidb->idb != NULL) &&
		    NOVELL_NIDB_IDB_UP(nidb) &&
		    ((net == -1) || (net == 0) || (nidb->novellnet == net))) {
		    newpak = pak_duplicate(pak);
		    if (newpak == NULL)
			break;
		    newipx = (ipxhdrtype *)ipxheadstart(newpak);
		    ieee_copy(nidb->idb->novell_node, newipx->shost);
		    PUT2WD(newipx->ssnet, nidb->novellnet);
		    ieee_copy(baddr, newipx->dhost);
		    PUT2WD(newipx->ddnet, nidb->novellnet);
		    newpak->nif_output = nidb;
		    clear_if_input(newpak);
		    if (nov_send(newpak)) {
		      nidb->idb->nov_sap_out_packets++;
		      novell_stat[NOVELL_SAPOUT]++;
		    }
		    if (!(col % 70) || !col)
			printc('\n');
		    printc('.');
		    col++;
		    flush();
		}
	    }
	    retbuffer(pak);
	}
	if (psipending()) {
	    psioff(stdio);
	    return;
	}
    }
    psioff(stdio);
}

/*
 * Send out a Novell SAP request packet. parser has prompted
 * for what type of query to send.
 *
 * Format of the command:
 *
 *     test novell query [<type> <name> <network> <mask> [<count> [<interval> [<net>]]]]
 */

static void test_novell_name (parseinfo *csb)
{
    ushort service, query_type;
    int i,count;
    int interval;
    ulong net, dnet, dmask;
    paktype *pak, *newpak;
    ipxhdrtype *ipx, *newipx;
    novell_querynametype *sap;
    nidbtype *nidb;
    boolean no_itf;
    int col  = 0;
    char *server_name;
    static const char novell_sapnamedflt[] = "cisco";

    query_type = GETOBJ(int,1);
    service  = GETOBJ(int,2);
    dnet     = GETOBJ(int,3);
    dmask    = GETOBJ(int,4);
    count    = GETOBJ(int,5);
    interval = GETOBJ(int,6);
    server_name = (char *) GETOBJ(string, 1);
    net      = GETOBJ(int,7);
    no_itf   = TRUE;

    printf("\nRequest nearest service (type %d) via:", service);

    FOR_ALL_NIDBS(nidb) {
	if ((nidb != NULL) && (nidb->idb != NULL) && NOVELL_NIDB_IDB_UP(nidb) &&
	    ((net == -1) || (net == 0) || (nidb->novellnet == net))) {
	    printf("\n - net %x, interface %s",
		   nidb->novellnet, nidb->idb->namestring);
	    no_itf = FALSE;
	}
    }
    
    if (no_itf) {
	printf("\n%%No active interface matching net %x", net);
	return;
    }

    psion(stdio); /* Allow user to escape */

    for (i=0; i<count; i++) {
	if (i > 0)
	    process_sleep_for(interval);
	pak = getbuffer(MEDDATA);
	if (pak) {
	    ipx = (ipxhdrtype *)ipxheadstart(pak);

	    pak->linktype = LINK_NOVELL;
	    ipx->tc = 0;
	    ipx->pt = NOVELL_TYPE_SAPRESP;
	    ipx->len = NOVELL_NAMEQUERYLENGTH + 58;
	    ipx->dsock = NOVELL_SOCK_SAP;
	    ipx->ssock = NOVELL_SOCK_SAP;
	    ipx->cksum = NOVELL_NO_CHECKSUM;    /* Force 0xFFFF */

	    sap = (novell_querynametype *)(XNSDATASTART(pak));
	    sap->query_type = query_type;

	    sap->server[0].server_type = service;
	    if (server_name == NULL) {
	      sstrncpy(sap->server[0].server_name, novell_sapnamedflt,
		       NOVELL_SERVER_NAME_LEN);
	    } else {
	      sstrncpy(sap->server[0].server_name, server_name,
		       NOVELL_SERVER_NAME_LEN);
	    }
	    sap->server[0].dnet = dnet;
	    sap->server[0].dmask = dmask;

	    sap->server[1].server_type = service +1;
	    if (server_name == NULL) {
	      sstrncpy(sap->server[1].server_name, novell_sapnamedflt, 
			   NOVELL_SERVER_NAME_LEN);
	    } else {
	      sstrncpy(sap->server[1].server_name, server_name, 
			   NOVELL_SERVER_NAME_LEN);
	    }
	    sap->server[1].dnet = dnet;
	    sap->server[1].dmask = dmask;

	    FOR_ALL_NIDBS(nidb) {
		if ((nidb != NULL)  && (nidb->idb != NULL) &&
		    NOVELL_NIDB_IDB_UP(nidb) &&
		    ((net == -1) || (net == 0) || (nidb->novellnet == net))) {
		    newpak = pak_duplicate(pak);
		    if (newpak == NULL)
			break;
		    newipx = (ipxhdrtype *)ipxheadstart(newpak);
		    if (ipx_internal_net) {
		      PUT2WD(ipx->ssnet, ipx_internal_net);
		      ieee_copy(&ipx_internal_hostid[0], ipx->shost);
		    } else {
		      ieee_copy(nidb->idb->novell_node, newipx->shost);
		      PUT2WD(newipx->ssnet, nidb->novellnet);
		    }
		    ieee_copy(baddr, newipx->dhost);
		    PUT2WD(newipx->ddnet, nidb->novellnet);
		    newpak->nif_output = nidb;
		    clear_if_input(newpak);
		    if (nov_send(newpak)) {
		      nidb->idb->nov_sap_out_packets++;
		      novell_stat[NOVELL_SAPOUT]++;
		    }
		    if (!(col % 70) || !col)
			printc('\n');
		    printc('.');
		    col++;
		    flush();
		}
	    }
	    retbuffer(pak);
	}
	if (psipending()) {
	    psioff(stdio);
	    return;
	}
    }
    psioff(stdio);
}

static void test_novell_watchdog (parseinfo *csb)
{
    paktype *pak;
    ipxhdrtype *ipx;
    ulong novell_wd_net;
    uchar novell_wd_host[IEEEBYTES];
    nidbtype *nidb = (nidbtype *)novidbQ.qhead;

    if (nidb == NULL) {
	printf("\nIPX: Not enabled on any interfaces");
	return;
    }

    novell_wd_net = GETOBJ(paddr,1)->nov_addr.net;
    ieee_copy(GETOBJ(paddr,1)->nov_addr.host, novell_wd_host);
    pak = getbuffer(MEDDATA);
    if (pak) {
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	ipx->tc = 0;
	ipx->pt = 0;
	ipx->len = NOVELL_WATCHDOG_LEN;
	pak->linktype = LINK_NOVELL;
	ipx->dsock = 0x4052;
	ipx->ssock = 0x4052;
	ipx->cksum = NOVELL_NO_CHECKSUM;	/* no checksum supplied */
	((ipx_watchdog_packet *) pak->datagramstart)->watchdog_byte =
	    NOVELL_WATCHDOG_QUERY;
	ieee_copy(nidb->idb->novell_node, ipx->shost);
	PUT2WD(ipx->ssnet, nidb->novellnet);
	ieee_copy(novell_wd_host, ipx->dhost);
	PUT2WD(ipx->ddnet, novell_wd_net);
	nov_send(pak);
    }
    psioff(stdio);
}


/*
 * Send out a novell netbios packet.
 *
 *     test novell netbios find name string pkt-count pkt-interval output-net
 *
 *     if (debug_novell1) set use packet type specified by debug_novell1
 *     if (debug_novell2) set use destination net specified by debug_novell2
 */

void
static test_novell_netbios (parseinfo *csb)
{
    int i,j,count;
    int interval;
    unsigned int s;
    paktype *pak;
    paktype *newpak;
    ipxhdrtype *ipx, *newipx;
    novellnetbiosheader *netbios;
    nidbtype *nidb;
    ulong net;
    char *namep;
    boolean no_itf = TRUE;
    int col  = 0;
    uchar  name[NOVELL_NETBIOS_NAME_LEN+1];
    const uchar dfltname[] = "TESTPACKET";

    count    = GETOBJ(int,1);
    interval = GETOBJ(int,2);
    net      = GETOBJ(int,3);
    namep    = (char *)GETOBJ(string,1);

    for (i=0; i<NOVELL_NETBIOS_NAME_LEN; i++)
	name[i] = ' ';
    name[i] = '\0';
    s = strlen( namep );
    s = min( s, NOVELL_NETBIOS_USABLE_NAME_LEN );
    if (!s) {
	s = 10;
	namep = (char *)&dfltname;
    }
    bcopy(namep, name, s);
    
    printf("\nNETBIOS findname '%s' via:", name);
    /* If network is 0 use internal network */
    FOR_ALL_NIDBS(nidb) {
	if (NOVELL_NIDB_IDB_UP(nidb) &&
	    ((net == -1) || (net == 0) || (nidb->novellnet == net))) {
	    printf("\n - net %x, interface %s",
		   (nidb->novellnet ? nidb->novellnet : ipx_internal_net),
		   nidb->idb->namestring);
	    no_itf = FALSE;
	}
    }
    
    if (no_itf) {
	printf("\n%%No active interface matching net %x", net);
	return;
    }

    psion(stdio); /* Allow user to escape */

    for (i=0; i<count; i++) {
	if (i > 0)
	    process_sleep_for(interval);
	pak = getbuffer(MEDDATA);
	if (pak) {
	    pak->linktype = LINK_NOVELL;
	    ipx = (ipxhdrtype *)ipxheadstart(pak);
	    ipx->tc = 0;
	    if (novell_debug1)
		ipx->pt = novell_debug1;
	    else
		ipx->pt = NOVELL_TYPE_NETBIOS;
	    ipx->len = (XNSOVERHEADBYTES + sizeof(novellnetbiosheader));
	    if (novell_debug2)
		PUT2WD(ipx->ddnet, novell_debug2);
	    else
		PUT2WD(ipx->ddnet, 0);

	    ieee_copy(baddr, ipx->dhost);
	    ipx->dsock = NOVELL_SOCK_NETBIOS;
	    ipx->ssock = NOVELL_SOCK_NETBIOS;
	    ipx->cksum = NOVELL_NO_CHECKSUM;	/* no checksum supplied */

	    /* Fill in NETBIOS header: */
	    netbios = (novellnetbiosheader *)ipxdatastart(pak);
	    PUTLONG(&netbios->nov_netbios_nets[0], GETOBJ(int,4));
 	    PUTLONG(&netbios->nov_netbios_nets[1], GETOBJ(int,5));
 	    PUTLONG(&netbios->nov_netbios_nets[2], GETOBJ(int,6));
 	    PUTLONG(&netbios->nov_netbios_nets[3], GETOBJ(int,7));
 	    PUTLONG(&netbios->nov_netbios_nets[4], GETOBJ(int,8));
 	    PUTLONG(&netbios->nov_netbios_nets[5], GETOBJ(int,9));
 	    PUTLONG(&netbios->nov_netbios_nets[6], GETOBJ(int,10));
 	    PUTLONG(&netbios->nov_netbios_nets[7], GETOBJ(int,11));

	    /* Figure correct initial hop count: */
	    for (j = 0; j < NOVELL_NETBIOS_NUM_NETS; j++)
		if (GETLONG(&netbios->nov_netbios_nets[j]) == 0)
		    break;
	    ipx->tc = j;

	    netbios->nov_netbios_nametypeflag = 0;
	    netbios->nov_netbios_cmd = NOVELL_NETBIOS_FIND_NAME;
	    bcopy(name, netbios->nov_netbios_server, NOVELL_NETBIOS_NAME_LEN);
	    FOR_ALL_NIDBS(nidb) {
		if (NOVELL_NIDB_IDB_UP(nidb) &&
		    ((net == -1) || (net == 0) || (nidb->novellnet == net))) {
		    newpak = pak_duplicate(pak);
		    if (newpak == NULL)
			break;
		    newipx = (ipxhdrtype *)ipxheadstart(newpak);
		    /* If network  is 0 use internal network */
		    PUT2WD(newipx->ssnet, (nidb->novellnet ?
					   nidb->novellnet :
					   ipx_internal_net));
		    ieee_copy(nidb->idb->novell_node, (nidb->novellnet ? 
						       newipx->shost :
						       ipx_internal_hostid));
		    newpak->nif_output = nidb;
		    newpak->if_output = nidb->idb;
		    clear_if_input(newpak);
		    nov_send1(newpak, newipx->dhost, newipx->len);
		    if (!(col % 70) || !col)
			printc('\n');
		    printc('.');
		    col++;
		    flush();
		}
	    }
	    retbuffer(pak);
	}
	if (psipending()) {
	    psioff(stdio);
	    return;
	}
    }
    psioff(stdio);
}

/*
 * test_novell_ripreq
 *
 * Send a rip request for a specific network.
 *
 */
static void test_novell_ripreq(parseinfo *csb)
{
    ulong net;
    paktype *pak, *newpak;
    ipxhdrtype *ipx, *newipx;
    novellriptype *rip;
    nidbtype *nidb;

    net = GETOBJ(int,1);

    pak = getbuffer(MEDDATA);
    if (pak) {
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	pak->linktype = LINK_NOVELL;
	ipx->tc = 0;
	ipx->pt = NOVELL_TYPE_RIP;
	ipx->len = XNSHEADERBYTES + sizeof(novellriptype);
	ipx->dsock = NOVELL_SOCK_RIP;
	ipx->ssock = NOVELL_SOCK_RIP;
	ipx->cksum = NOVELL_NO_CHECKSUM;    /* Force 0xFFFF */

	rip = (novellriptype *)ipxdatastart(pak);
	rip->cmd = RIP_REQUEST;
	FOR_ALL_NIDBS(nidb) {
	    if (NOVELL_NIDB_IDB_UP(nidb)) {
	        newpak = pak_duplicate(pak);
		if (newpak == NULL)
		    break;
		newipx = (ipxhdrtype *)ipxheadstart(newpak);
		ieee_copy(nidb->idb->novell_node, newipx->shost);
		PUT2WD(newipx->ssnet, nidb->novellnet);
		ieee_copy(baddr, newipx->dhost);
		PUT2WD(newipx->ddnet, nidb->novellnet);
		newpak->nif_output = nidb;
		PUT2WD(rip->nets[0].nnet, net);
		nov_send(newpak);
	    }
	}
	retbuffer(pak);
    }
}

/*
 * Set up generic debug parameters
 *
 */
#ifdef  NOVELL_TEST_DEBUG
static void test_novell_debug_switch (int arg, boolean help_wanted);
#endif
static void test_novell_debug (parseinfo *csb)
{

    novell_debug1 = GETOBJ(int,1);
    novell_debug2 = GETOBJ(int,2);
    novell_debug3 = GETOBJ(int,3);
    novell_debug4 = GETOBJ(int,4);
    novell_debug5 = GETOBJ(int,5);
    novell_debug6 = GETOBJ(int,6);
    novell_debug7 = GETOBJ(int,7);
    novell_debug8 = GETOBJ(int,8);
    novell_debug_net1 = GETOBJ(paddr,1)->nov_addr.net;
    novell_debug_net2 = GETOBJ(paddr,2)->nov_addr.net;
    ieee_copy(GETOBJ(paddr,1)->nov_addr.host, novell_debug_host1);
    ieee_copy(GETOBJ(paddr,2)->nov_addr.host, novell_debug_host2);

/* Transient debugging code ... not for release, but DO NOT REMOVE */
#undef  NOVELL_TEST_DEBUG

#ifdef  NOVELL_TEST_DEBUG
    test_novell_debug_switch(novell_debug1, FALSE);
#endif
}

/*
 * Debug support routines:
 */
uchar *novdbg_proctype2string (ulong proctype, boolean sap_flag)
{
    switch (proctype) {

      default:
	return("????");
	break;

      case NET_STATIC:
	return("STAT");
	break;

      case NET_CONNECTED:
	return("CONN");
	break;

      case NET_RIP:
	return((sap_flag ? " SAP" : " RIP"));
	break;

      case NET_IGRP:
	return("IGRP");
	break;

      case NET_ISIS:
	return("ISIS");
	break;

      case NET_NLSP:
	return("NLSP");
	break;

    }

}
uchar *novdbg_event2string (int event)
{
    switch (event) {
      default:
	return("????");
	break;
      case ROUTE_UP:
	return("UP");
	break;
      case ROUTE_DOWN:
	return("DOWN");
	break;
      case ROUTE_MODIF:
	return("MOD");
	break;
    }
}

#if NOVELL_TEST_DEBUG
/*
 * Transient debug driver
 *
 * This routine is essentially a single SWITCH statement called
 * (if present) as part of "test novell debug <nnn>". It switches on
 * the value novell_debug1. It is broken out here so that it may
 * call itself recursively in order to implement selected combinations
 * of functionality.
 */
static void test_novell_debug_switch (int arg, boolean help_wanted)
{
    novpdbtype *pdb;
    nidbtype *nidb;
    int help_arg;

    switch(arg) {

      case 0:
      case 1:
	if (help_wanted) {
	    printf("\n - 0x%-2x: Set novell_debug variables", arg);
	    printf("\nnovell_debug: %x %x %x %x %x %x %x %x",
		   novell_debug1, novell_debug2, novell_debug3, novell_debug4, 
		   novell_debug5, novell_debug6, novell_debug7, novell_debug8);
	    return;
	}
	/*
	 * No action ... except set debug variables:
	 */
	printf("\nnovell_debug: %x %x %x %x %x %x %x %x",
	       novell_debug1, novell_debug2, novell_debug3, novell_debug4, 
	       novell_debug5, novell_debug6, novell_debug7, novell_debug8);
	break;

      case 2:
	if (help_wanted) printf("\nDebug help summary:");
	/*
	 * Print quick help:
	 */
	for (help_arg = 1; help_arg < 100; help_arg++) {
	    if (help_arg == 2) continue;
	    test_novell_debug_switch(help_arg, TRUE);
	}
	break;

      case 3:
	if (help_wanted) {
	    printf("\n - 0x%-2x: Send RIP/SAP general query on all nets", arg);
	    return;
	}
	/*
	 * Send RIP/SAP general query on all networks:
	 */
	for (nidb = (nidbtype *) novidbQ.qhead; nidb; nidb = nidb->next) {
	    if (NOVELL_NIDB_IDB_UP(nidb)) {
		novell_send_gen_rip_query(nidb);
		novell_send_gen_sap_query(nidb, NOVELL_EVERYONE);
	    }
	}
	break;

      case 4:
	if (help_wanted) {
	    printf("\n - 0x%-2x: Selected data structures size", arg);
	    return;
	}
	/*
	 * 0x4: Display size of interesting data structures:
	 *
	 */
	printf("\nsizeof(novndbtype)=%d (0x%x)",
	       sizeof(novndbtype), sizeof(novndbtype));
	printf("\nsizeof(novrdbtype)=%d (0x%x)",
	       sizeof(novrdbtype), sizeof(novrdbtype));
	printf("\nsizeof(novpdbtype)=%d (0x%x)",
	       sizeof(novpdbtype), sizeof(novpdbtype));
	printf("\nsizeof(nov_routetable)=%d (0x%x)",
	       sizeof(nov_routetable), sizeof(nov_routetable));
	printf("\nsizeof(novroutetype)=%d (0x%x)",
	       sizeof(novroutetype), sizeof(novroutetype));
	printf("\nsizeof(ipxhdrtype)=%d (0x%x)",
	       sizeof(ipxhdrtype), sizeof(ipxhdrtype));
	printf("\nsizeof(novell_serveridtype)=%d (0x%x)",
	       sizeof(novell_serveridtype), sizeof(novell_serveridtype));
	printf("\nsizeof(novell_sapentry_rdb)=%d (0x%x)",
	       sizeof(novell_sapentry_rdb), sizeof(novell_sapentry_rdb));
	printf("\nsizeof(novell_sapentrytype)=%d (0x%x)",
	       sizeof(novell_sapentrytype), sizeof(novell_sapentrytype));
	printf("\nsizeof(novcachetype)=%d (0x%x)",
	       sizeof(novcachetype), sizeof(novcachetype));
	printf("\nsizeof(novpathtype)=%d (0x%x)",
	       sizeof(novpathtype), sizeof(novpathtype));
	printf("\nsizeof(clns_pdbtype)=%d (0x%x)",
	       sizeof(clns_pdbtype), sizeof(clns_pdbtype));
	printf("\nsizeof(isis_TLV_parse)=%d (0x%x)",
	       sizeof(isis_TLV_parse), sizeof(isis_TLV_parse));
	printf("\nsizeof(isis_lspcontext)=%d (0x%x)",
	       sizeof(isis_lspcontext), sizeof(isis_lspcontext));
	printf("\nsizeof(isis_ddbtype)=%d (0x%x)",
	       sizeof(isis_ddbtype), sizeof(isis_ddbtype));
	printf("\nsizeof(isis_rdbtype)=%d (0x%x)",
	       sizeof(isis_rdbtype), sizeof(isis_rdbtype));
	printf("\nsizeof(clns_adjtype)=%d (0x%x)",
	       sizeof(clns_adjtype), sizeof(clns_adjtype));
	printf("\nsizeof(isis_lspdbtype)=%d (0x%x)",
	       sizeof(isis_lspdbtype), sizeof(isis_lspdbtype));
	break;

      case 5:
	if (help_wanted) {
	    printf("\n - 0x%-2x: Schedule SPF", arg);
	    return;
	}
	/*
	 * 0x5: Schedule SPF:
	 *
	 */
	for (pdb = (novpdbtype *)novpdbQ.qhead; pdb; pdb= pdb->next) {
	    if (pdb->proctype == NET_NLSP && pdb->osi_pdb)
		(void) reg_invoke_ipx_nlsp_ipx_sched_spf(pdb->osi_pdb);
	}
	break;

      default:
	break;
    }
}
#endif
/* Transient debugging code ... not for release, but DO NOT REMOVE */

/*
 * display_novell
 * Display NOVELL interface information
 */

void
display_novell (idbtype *idb)
{
    nidbtype *nidb;
    netbios_sb_t *netb_sb;
    novaddtype *ptr;

    /*
     * Let's not give non-information.
     */

    if (!idb->novellnets)
	return;

    netb_sb = netbios_get_sb(idb);

    printf(if_is_line_proto_is_msg, idb->namestring,
          print_idbstate(idb), interface_up(idb) ? "up" : "down");

    /*
     * Display primary network first:
     */
    nidb = IPX_PRIMARY_NIDB(idb);

    /* if there isn't a primary nidb, check for an owner_nidb */
    if (nidb == NULL) 
	nidb = idb->owner_nidb;
    if (nidb != NULL) {
	printf("\n  IPX address is %x.%e", nidb->novellnet, 
	       idb->novell_node);
	if (idb_is_lan(idb)) {
	    printf(", %s", encstring(nidb->enctype));
	}
	printf(" %s", novell_nidb_print_state(nidb));
	printf("\n  Delay of this IPX network, in ticks is %u throughput %u link delay %u",
	       NOVELL_IDB_TICKS(nidb),NOVELL_LINK_THROUGHPUT(nidb),
	       NOVELL_LINK_DELAY(nidb));
	reg_invoke_nov_ipxwan_display(nidb);
    } else {
	printf("\n%%Warning: IPX network data base is inconsistent");
	return;
    }

    FOR_ALL_NIDBS(nidb) {
	if ((nidb->idb == idb) && IPX_NIDB_IS_SECONDARY(nidb)) {
	    printf("\n  Secondary address is %x.%e",
		   nidb->novellnet, idb->novell_node);
	    if (idb_is_lan(idb))
		printf(", %s", encstring(nidb->enctype));
	    printf(" %s", novell_nidb_print_state(nidb));
	    printf("\n  Delay of this Novell network, in ticks is %u",
		   NOVELL_IDB_TICKS(nidb));
	}
    }
    printf("\n  IPX SAP update interval is %d minute(s)",idb->novell_interval);
    printf("\n  IPX type 20 propagation packet forwarding is %s",
	   (idb->novell_type20 ? "enabled" : "disabled"));
    printf("\n  Incoming access list is ");
    notset(idb->novell_inaccesslist);
    printf("\n  Outgoing access list is "); notset(idb->novell_accesslist);
    if (idb->novell_accesslist != idb->novell_accesslist_def) {
	printf(" (default is "); 
	notset(idb->novell_accesslist_def);
	printf(")"); 	
    }
    if (idb->nov_helpernetQ.qhead) {
	printf("\n  IPX helper address(es):");
        for (ptr = (novaddtype *) idb->nov_helpernetQ.qhead; ptr;
		ptr = ptr->next) {
	    printf("\n   %x.%e", ptr->novell_net, ptr->novell_host);
	}
    }
    printf("\n  IPX helper access list is "); 
    notset(idb->novell_helperlist);

    printf("\n  SAP GNS processing %sabled, delay %d ms, output filter list is ",
	   idb->nov_respond_to_gns ? "en" : "dis", idb->nov_gns_delay);
    notset(idb->nov_gns_outfilterlist);
    printf("\n  SAP Input filter list is ");
    notset(idb->nov_sap_infilterlist);
    printf("\n  SAP Output filter list is ");
    notset(idb->nov_sap_outfilterlist);
    printf("\n  SAP Router filter list is ");
    notset(idb->nov_sap_routerfilterlist);
    printf("\n  Input filter list is "); notset(idb->nov_in_filterlist);
    printf("\n  Output filter list is "); notset(idb->nov_out_filterlist);
    printf("\n  Router filter list is "); notset(idb->nov_router_filterlist);
    printf("\n  Netbios Input host access list is %s",
	   ((netb_sb && netb_sb->nov_netbios_h_iacf) ?
	    netb_sb->nov_netbios_h_iacf->name :
	    "not set"));
    printf("\n  Netbios Input bytes access list is %s",
	   ((netb_sb && netb_sb->nov_netbios_b_iacf) ?
	    netb_sb->nov_netbios_b_iacf->name :
	    "not set"));
    printf("\n  Netbios Output host access list is %s",
	   ((netb_sb && netb_sb->nov_netbios_h_oacf) ?
	    netb_sb->nov_netbios_h_oacf->name :
	    "not set"));
    printf("\n  Netbios Output bytes access list is %s",
	   ((netb_sb && netb_sb->nov_netbios_b_oacf) ?
	    netb_sb->nov_netbios_b_oacf->name :
	    "not set"));
    printf("\n  Updates each %d seconds, aging multiples RIP: %d SAP: %d ", 
	    idb->nov_update_interval/ONESEC, idb->nov_rip_multiplier,
	   idb->nov_sap_multiplier);
    nidb = IPX_PRIMARY_NIDB(idb);
    /* if there isn't a primary nidb, check for an owner_nidb */
    if (nidb == NULL) 
	nidb = idb->owner_nidb;
    printf("\n  SAP interpacket delay is %u",idb->novell_sap_delay);
    if (nidb->trig_sap_delay != NOV_OUTPUT_DELAY_DEFAULT)
      printf("/%u", nidb->trig_sap_delay);
    printf(" ms, maximum size is %d bytes", idb->nov_sap_maxpacketsize);
    printf("\n  RIP interpacket delay is %u",idb->novell_rip_delay);
    if (nidb->trig_rip_delay != NOV_OUTPUT_DELAY_DEFAULT)
      printf("/%u", nidb->trig_rip_delay);
    printf(" ms, maximum size is %d bytes", idb->nov_rip_maxpacketsize);


    /*
     * Display pad status iff non-default setting in effect:
     */
    if ((((idb->hwptr->status & (IDB_ETHER|IDB_LEX)) || lec_enabled(idb))
         && (!idb->novell_evenize)) ||
	((idb->hwptr->status & (IDB_TR | IDB_FDDI | IDB_SERIAL)) &&
	 (idb->novell_evenize))) {
	if (idb->hwptr->status & (IDB_ETHER|IDB_LEX))
	    printf("\n  Do not p");
	else
	    printf("\n  P");
	printf("ad odd-length process-switched packets on output");
    }

    FOR_ALL_NIDBS(nidb) {
        if ((nidb != NULL) && (nidb->idb == idb) &&
	    (nidb == IPX_PRIMARY_NIDB(idb)))
	    if ((idb->hwptr->status & IDB_SERIAL) ||
		(idb->hwptr->type == IDBTYPE_LB) ||
		(idb->hwptr->status & IDB_DIALER) || is_p2p(idb)) {
		printf("\n  Watchdog spoofing is %sabled, SPX spoofing is %sabled, idle time %d",
		       ((nidb->novell_spoof_flag & NOVELL_W_SPOOF) ? "en" : "dis"),
		       ((nidb->novell_spoof_flag & NOVELL_S_SPOOF) ? "en" : "dis"),
		       nidb->novell_spxidletime);
	    }
    }
    printf("\n  IPX accounting is %sabled",
	   idb->ipx_accounting ? "en" : "dis");
    if (idb->hwptr->novell_routecache) {
	printf("\n  IPX %s switching is configured (%sabled)",
	       (idb->hwptr->novell_routecache & NOVELL_RCACHE_CBUS) ?
	       "autonomous" : "fast",
	       ((idb->hwptr->novell_fastokay) ? "en" : "dis"));
	if (registry_populated(REG_SSE) && reg_invoke_sse_present()) {
	    printf("\n  IPX SSE switching is %sabled",
		   ((idb->hwptr->fast_switch_flag & (FSF_NOVELL1 | FSF_NOVELL2)) &&
		    (idb->hwptr->novell_routecache & NOVELL_RCACHE_SSE))
		   ? "en" : "dis");
	}
    } else {
	printf("\n  IPX fast switching is not configured");
    }
    printf("\n  RIP packets received %d, RIP packets sent %d",
	   idb->nov_rip_in_packets, idb->nov_rip_out_packets);
    printf("\n  SAP packets received %d, SAP packets sent %d",
	   idb->nov_sap_in_packets, idb->nov_sap_out_packets);
    /*
     * IPX ISIS/NLSP display:
     */
    if ((nidb = IPX_PRIMARY_NIDB(idb))) {
	if (nidb->nov_lsp_enabled) {
	    printf("\n  IPX %s is running on primary network %x",
		   (nidb->nov_lsp_isis ? "ISIS" : "NLSP"),
		   nidb->novellnet);
	    printf("\n    RIP compatibility mode is %s",
		   novell_name_auto_mode(nidb->lsp_rip_cfg));
	    if (nidb->lsp_rip_cfg == IPX_RIPSAP_CONFIG_AUTO) {
		printf(" (%s)", (nidb->lsp_rip_on ? "ON" : "OFF"));
		if (TIMER_RUNNING(nidb->lsp_rip_timestamp))
		    printf(", last traffic rcvd %d sec ago",
			   (ELAPSED_TIME(nidb->lsp_rip_timestamp)/1000));
	    }
	    printf("\n    SAP compatibility mode is %s",
		   novell_name_auto_mode(nidb->lsp_sap_cfg));
	    if (nidb->lsp_sap_cfg == IPX_RIPSAP_CONFIG_AUTO) {
		printf(" (%s)", (nidb->lsp_sap_on ? "ON" : "OFF"));
		if (TIMER_RUNNING(nidb->lsp_sap_timestamp))
		    printf(", last traffic rcvd %d sec ago",
			   (ELAPSED_TIME(nidb->lsp_sap_timestamp)/1000));
	    }
	}
    }
    reg_invoke_ipx_show_interface(idb);
}

static void free_srdb_list (nov_srdb_list *lh)
{
    nov_srdb_list *le, *next_le;
    
    for (le = lh; le; le = next_le) {
 	next_le = le->next;
 	free(le->srdb); /* Unlock srdb */
 	free(le);
    }
}

static const char novell_sapheader[] =
"   Type Name                       Net     Address    Port     Route Hops Itf";
#define SLEN 21 /* Length of service name window in default display */
static void
print_server (novell_sapentrytype *entry, boolean detailed)
{
    novell_sapentry_rdb *srdb;
    int pathcount = 0;
    nov_srdb_list *srdb_lh = NULL;  /* srdb display listhead */
    nov_srdb_list *le, *next_le;    /* srdb display list element */
    nov_srdb_list *last_le = NULL;
    
    if (entry == NULL) {
 	return;
    }
    /*
     * Build a list of srdb descriptors, since all kinds of mean,
     * nasty, ugly things can happen to srdb's during the display.
     * Be careful to preserve the correct order of the path list.
     */
    for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) {
 	le = malloc(sizeof(nov_srdb_list));
 	if (le == NULL) {
 	    free_srdb_list(srdb_lh);
 	    return;
 	}
 	mem_lock(srdb); /* Lock srdb */
 	le->srdb = srdb;
	if (srdb_lh == NULL) {
	    srdb_lh = le;
	}
	if (last_le != NULL) {
	    last_le->next = le;
	}
 	last_le = le;
    }
    /*
     * Remember the first srdb descriptor, if any:
     */
    le = srdb_lh;
    /*
     * Display common server info:
     */
    printf("\n%c%c%c%-4x %21S %-8x.%e:%04x ",
	   ((entry->flags & NET_STATIC) ?
	    ((entry->options & IPX_SERVER_OPTION_STATIC_NO_NVGEN) ?
	     ('s') : ('S')) :
	    (entry->flags & NET_IGRP) ? ('E') :
	    (entry->flags & NET_ISIS) ? ('I') :
	    (entry->flags & NET_NLSP) ? ('N') :
	    (entry->flags & NET_RIP) ? ('P') : ('?')),
	   (min(entry->pathcount, novell_maxpaths) > 1) ? ('+') : (' '),
	   (entry->nidb_changed ? ('*') : (' ')),
	   entry->server.server_type,
	   entry->server.server_name,
	   entry->server.dnet,
	   entry->server.dhost,
	   entry->server.dsock);

    if (entry->poisoned)
	printf("     ");
    else
	if (novell_sap_uses_routing) {
	    if (entry->route_delay == METRIC_INACCESSIBLE) {
		printf("     none");
	    } else {
	      if ((entry->flags & (NET_CONNECTED | NET_STATIC)) &&
		  ((entry->route_delay <= NOVELL_DEFAULT_CONNECTED_TICKS) &&
		   (entry->route_hops <= NOVELL_DEFAULT_RIP_HOPS))) {
		printf("     conn");
	      } else {
		printf("%-6u/%-02u", entry->route_delay,
		       entry->route_hops);
	      }
	    }
	} else {
	    if (le == NULL)
		printf("     none");
	    else {
		if (entry->server.dnet == le->srdb->nidb->novellnet)
		    printf("     conn"); 
		else
		    printf("     "); 
	    }
	}

    if (entry->poisoned)
	printf("  ");
    else {
	printf("  %-2d", entry->server.nethops);
	if (le != NULL) {
	    printf("  %s", (le->srdb->nidb ?
			    le->srdb->nidb->idb->short_namestring : NULL));
	}
	if (detailed) {
	    if (strlen(entry->server.server_name) > SLEN) {
		printf("\n%-*cFull name: '%S'", 8, ' ',
		       entry->server.server_name);
	    }
	    for (le = srdb_lh; le; le = next_le) {
		next_le = le->next;
		printf("\n%-*c-- via %s:%x.%e, %-3ds", 8, ' ',
		       (le->srdb->nidb ?
			le->srdb->nidb->idb->short_namestring : NULL),
		       le->srdb->snet, le->srdb->srchost,
		       (ELAPSED_TIME(le->srdb->timer)/1000));
		if (entry->flags & NET_NLSP) {
		    printf(", {%d/%d}",
			   le->srdb->lsp_linkage.lsp_index, le->srdb->lsp_linkage.lsp_version);
		}
		if ((++pathcount >= novell_maxpaths) &&
		    (entry->flags & NET_NLSP)) {
		    break;
		}
	    }
	}
    }
    free_srdb_list(srdb_lh);
}

/*
 * Compare two server table service name strings:
 */
static int cmp_sap_names (void *s1, void *s2)
{
    novell_sapentrytype *sap1;
    novell_sapentrytype *sap2;

    sap1 = (novell_sapentrytype *)s1;
    sap2 = (novell_sapentrytype *)s2;

    return(strcmp(sap1->server.server_name, sap2->server.server_name) < 0);
}

/*
 * Compare two server table network entries:
 */
static int cmp_sap_nets (void *s1, void *s2)
{
    novell_sapentrytype *sap1;
    novell_sapentrytype *sap2;

    sap1 = (novell_sapentrytype *)s1;
    sap2 = (novell_sapentrytype *)s2;
    
    return(sap1->server.dnet < sap2->server.dnet);
}

/*
 * Compare two server table server types:
 */
static int cmp_sap_types (void *s1, void *s2)
{
    novell_sapentrytype *sap1;
    novell_sapentrytype *sap2;

    sap1 = (novell_sapentrytype *)s1;
    sap2 = (novell_sapentrytype *)s2;
    
    return(sap1->server.server_type < sap2->server.server_type);
}

/*
 * xsort - dumb exchange sort to subsort small subsets of services.
 *
 * Sort an array of pointer to sap entry by network.
 *
 * Input:   rp:     Points to an array of pointers to item.
 *          count:  Number of elements in array.
 *          cmp:    Function to compare item
 *
 * Notes:   No error or sanity checking is performed.
 */

static void xsort (ptable *rp,         /* array of pointer to item */
		   int count,
		   boolean (*cmp)(void *, void *))
{
    int i;
    boolean sorted;
    void *tp;

    if (count <= 1)
	return;

    do {
	sorted = TRUE;
	for (i=0; (i < count-1); i++) {
	    if (cmp( *rp[i+1], *rp[i])) {
		tp = *rp[i];
		*rp[i] = *rp[i+1];
		*rp[i+1] = tp;
		sorted = FALSE;
	    }
	}
	process_suspend();
    } while (!sorted);
}

/*
 * nov_quicksort - Basic quicksort
 *
 * After Wirth - Algorithms + Data Structures = Programs
 */
/*
 * Limit recursion ... assigned empirically using a list of 3000 servers
 * This routine still gives us a performance increase of over 100 times
 * the dumb exchange sort on a list of this size.
 */


#define MAX_QSORT_DEPTH 30
#define MIN_QSORT_PARTITION 7

void nov_quicksort (ptable *rp,          /* array of pointer to item */
		    int l,
		    int r,
		    int depth,
		    boolean (*cmp)(void *, void *))
{
    int i;     /* left partition */
    int j;     /* right partition */
    void *m;   /* middle */
    void *tp;

    depth++;
#ifdef DEBUG_SORT
    if (depth > max_qsort_depth)
	max_qsort_depth = depth;
#endif

    if ((depth > MAX_QSORT_DEPTH) ||
	((r - l + 1) <= MIN_QSORT_PARTITION)) {
	xsort(&rp[l], (r-l+1), cmp);
	return;
    }

    i = l;
    j = r;
    m = *rp[(l + r) / 2];

    do {
	while ( cmp(*rp[i], m)) {
	    i++;
	}
	while ( cmp(m, *rp[j]) ) {
	    j--;
	}
	if (i <= j) {
	    /* Do the swap ... */
	    tp = *rp[i];
	    *rp[i] = *rp[j];
	    *rp[j] = tp;
	    i++;
	    j--;
	}
    } while ( i <= j );

    process_suspend();
    if (l < j)
	nov_quicksort(rp, l, j, depth, cmp);
    process_suspend();
    if (i < r)
	nov_quicksort(rp, i, r, depth, cmp);
}

/*
 * sort_by_name
 *
 * Display services sorted by name:type. Note that there is no need
 * to sort the internal data structure prior to calling this routine.
 */
static void sort_by_name (
     ptable *rp,    /* sap records */
     int count)
{
    int i,j;
#ifdef DEBUG_SORT
    sys_timestamp start_time;
    GET_TIMESTAMP(start_time);
    max_qsort_depth = 0;
#endif

    nov_quicksort(rp, 0, count-1, 0, cmp_sap_names);

    /* Subsort by type: */
    for (i=0; i < count; i++) {

	j = i++;

	while ((i < count) && (!cmp_sap_names(*rp[j], *rp[i])))
	    i++;
	/* Display one directly, display several subsorted by type: */
	if ((i-j) > 1)
	    xsort(&rp[j], (i-j), cmp_sap_types);
	--i;
    }
#ifdef DEBUG_SORT
    printf("\nMax quicksort recursion depth = %d", max_qsort_depth);
    printf("\nSorting time = %d mSec\n", ELAPSED_TIME(start_time));
#endif
}

/*
 * sort_by_net
 *
 * Display services sorted by network:name:type. Note that there is no
 * need to sort the internal data structure prior to calling this routine.
 * Also note that the exchange sort routines are called in preference to
 * quicksort to subsort since are normally few duplicate names on a given
 * server or network.
 */
static void sort_by_net (
     ptable *rp,    /* sap records */
     int count)
{
    int i,j ,ii,jj;
    ulong dnet;
#ifdef DEBUG_SORT
    sys_timestamp start_time;
    GET_TIMESTAMP(start_time);
    max_qsort_depth = 0;
#endif

    /* Sort by network: */
    nov_quicksort(rp, 0, count-1, 0, cmp_sap_nets);
    
    /* Subsort by name then by type: */
    for (i=0; i < count; i++) {

	j = i++;
	dnet = ((novell_sapentrytype*)*rp[j])->server.dnet;

	while ((i < count) &&
	       (dnet == ((novell_sapentrytype*)*rp[i])->server.dnet))
	    i++;

	/*
	 * Display one directly, display several subsorted appropriately:
	 */
	if ((i-j) > 1) {
	    /* Subsort by name */
	    nov_quicksort(rp, j, (i-1), 0, cmp_sap_names);
	    /* Subsort by type */
	    for (ii=j; ii < i; ii++) {
		jj = ii++;
		while ((ii < i) && (!cmp_sap_names(*rp[jj], *rp[ii])))
		    ii++;
		if ((ii-jj) > 1)
		    xsort(&rp[jj], (ii-jj), cmp_sap_types);
		--ii;
	    }
	}
	--i;
    }
#ifdef DEBUG_SORT
    printf("\nMax netsort recursion depth = %d", max_qsort_depth);
    printf("\nSorting time = %d mSec\n", ELAPSED_TIME(start_time));
#endif
}

/*
 * sort_by_type
 *
 * Sort sap queue entries by type:distance:name. Since the sap queue is
 * already in type:distance order (having just been sorted), this reduces
 * to simply subsorting by name. Note that, as always, we use the complex
 * form of compare to correctly sort by "distance".
 */
static void sort_by_type (
     ptable *rp,    /* sap records */
     int count)
{
    int i,j;
    ushort type;
#ifdef DEBUG_SORT
    sys_timestamp start_time;
    GET_TIMESTAMP(start_time);
    max_qsort_depth = 0;
#endif

    for (i=0; i < count; i++) {

	/* Count servers of same type at same distance: */
	type = ((novell_sapentrytype*) *rp[i])->server.server_type;
	j = i++;

	while ((i < count) &&
	       (((novell_sapentrytype*) *rp[i])->server.server_type == type) &&
	       novell_sap_compare_eq(((novell_sapentrytype*) *rp[i]),
				     ((novell_sapentrytype*) *rp[j]))) {
	    i++;
	}
	
	/* Display one directly, display several alphabetically: */
	if ((i-j) > 1)
	   nov_quicksort(rp, j, (i-1), 0, cmp_sap_names);
	--i;
    }
#ifdef DEBUG_SORT
    printf("\nMax quicksort recursion depth = %d", max_qsort_depth);
    printf("\nSorting time = %d mSec\n", ELAPSED_TIME(start_time));
#endif
}

/*
 * show_novell_servers
 * Show the user the Novell Server list
 */

void show_novell_servers (int sort, boolean detailed, uchar *pstring)
{
    novell_sapqueuetype *sapq;
    novell_sapentrytype *entry;
    int i, count;
    ptable *rp;    /* sap entry records */
    regexp *pattern;

    /* If the request is to display SAPs matching a regular expression,
     * compile the regular expression. If it is invalid, bail now.
     * Error messages are provided by regcomp().
     */
    pattern = regcomp(pstring);
    if (pstring && !pattern) {
	return;
    }

    /*
     * If the request is to display sorted by type (the default),
     * sort all internal server lists according to algorithm currently in
     * effect.
     */
    if (sort == IPXSAP_SORT_BY_TYPE) {
	novell_sort_all_saps();
    }

    /* Count/screen sapQ elements: */
    count = 0;
    FOR_ALL_SAP_TYPES(sapq) {
	entry = (novell_sapentrytype*) sapq->saps.flink;
	while (entry != (novell_sapentrytype*)&sapq->saps) {
	    if (!validmem(entry)) {
		printf("\n%%Server list invalid");
		return;
	    }
	    count++;
	    entry = (novell_sapentrytype*)entry->saps.flink;
	}
    }

    /* Fix up the global server count if there is a discrepancy. */
    if (count != novell_server_count) {
	errmsg(&msgsym(BADCOUNT, IPX), "Old server count=", 
	       novell_server_count, "New server count=", count);
	novell_server_count = count;
    }
    /* Exit if list empty: */
    if (!count)
	return;

    /*
     * During the display the sap queue may change beneath us.
     * Soooo, create a list of all entries and lock each buffer.
     * (This isn't really so bad since it facilitates sorting!)
     * At the same time, create a list of name pointers.
     */
    rp = malloc(count*sizeof(uchar *));

    if (rp == NULL) {
	return;
    }

    i = 0;
    sapq = (novell_sapqueuetype *) novell_sapQ.qhead;
    while (sapq != NULL) {
	if (i >= count)
	  break;
 	entry = (novell_sapentrytype*) sapq->saps.flink;
 	while (entry != (novell_sapentrytype*)&sapq->saps) {
	    if (i >= count)
	      break;
	    inline_mem_lock(entry);
	    *rp[i] = entry;
	    i++;
 	    entry = (novell_sapentrytype*)entry->saps.flink;
	}
	sapq = sapq->next;
    }
    /* Safety check. */
    if (i < count)
	count = i;

    /* It is now safe to suspend or be suspended. */
    (void) process_may_suspend();

    automore_enable(NULL);
    printf("\nCodes: S - Static, P - Periodic, E - EIGRP"
	   ", N - NLSP, H - Holddown, + = detail");
    printf("\n%d Total IPX Servers", count);

    if (novell_sap_uses_routing)
	printf("\n\nTable ordering is based on routing and server info\n");
    else
	printf("\n\nTable ordering is based on server info only\n");

    /* Sort the list per the user: */

    switch (sort) {
      case IPXSAP_SORT_BY_NAME:
	sort_by_name(rp, count);
	break;
      case IPXSAP_SORT_BY_NET:
	sort_by_net(rp, count);
	break;
      case IPXSAP_SORT_BY_TYPE:
	sort_by_type(rp, count);
	break;
      default:
	/* unsorted */
	break;
    }

    /* Finally, display per the sort: */
    if (count > 0) {
	automore_header(novell_sapheader);
    }
    for (i = 0; i < count; i++) {
	if (pattern) {
	    /*
	     * Only display those servers whose name matches the 
	     * regular expression.
	     */
	    entry = *rp[i];
	    if (!regexec(pattern, entry->server.server_name))
		continue;
	    print_server(*rp[i], detailed);
	    if (automore_quit())
		break;
	} else {
	    /*
	     * Dispaly all servers
	     */
	    print_server(*rp[i], detailed);
	    if (automore_quit())
		break;
	}
    }

    /* Unlock each sap buffer and return list memory: */
    for (i=0; i < count; i++) {
	inline_mem_unlock_or_free(*rp[i]);
    }
    free(rp);
    automore_disable();
}

/*
 * show_novellroute
 * Call generic RIP display routine with Novell structures as arguments.
 */

void show_novellroute (ulong net, boolean detail)
{
    show_novriproutes(net, detail);
}

/*
 * show_novelltraffic
 * Print out Novell traffic statistics
 *
 * Note: Don't get rid of the use of the 'ints' for the queue sizes in this
 * routine!
 */

void show_novelltraffic (void)
{

    int novQlen, novbgQlen, novell_ripQlen, novell_gnsQlen, nov_freed_ndbQlen;
    
    
    novQlen = process_queue_size(novQ);
    novbgQlen = (int) QUEUESIZE(&novbgQ);
    novell_ripQlen = (int) QUEUESIZE(&novrip_pdb.inputq);
    novell_gnsQlen = (int) QUEUESIZE(&novell_gnsQ);
    nov_freed_ndbQlen = (int) QUEUESIZE(&nov_freed_ndbQ);
    printf("\nSystem Traffic for %x.%e System-Name: %s", ipx_internal_net,
	   ipx_internal_hostid, ipx_internal_name);
    printf("\nRcvd:   %u total, %u format errors, %u checksum errors, %u bad hop count, ",
	    novell_stat[NOVELL_INPUT], novell_stat[NOVELL_FORMATERROR],
	    novell_stat[NOVELL_CKSUMERR], novell_stat[NOVELL_HOPCOUNT]);
    printf("\n        %u packets pitched, %u local destination, %u multicast",
	    novell_stat[NOVELL_PITCHED], 
	   novell_stat[NOVELL_LOCALDEST], novell_stat[NOVELL_INMULT]);
    printf("\nBcast:  %u received, %u sent",
	    novell_stat[NOVELL_BCASTIN], novell_stat[NOVELL_BCASTOUT]);
    printf("\nSent:   %u generated, %u forwarded",
	    novell_stat[NOVELL_OUTPUT], novell_stat[NOVELL_FORWARDED]);
    printf("\n        %u encapsulation failed, %u no route",
	    novell_stat[NOVELL_ENCAPSFAILED], novell_stat[NOVELL_NOROUTE]);
    printf("\nSAP:    %u SAP requests, %u SAP replies, %d servers", 
           novell_stat[NOVELL_SAPREQIN], novell_stat[NOVELL_SAPREPLY], 
           novell_server_count); 
    printf("\n        %u SAP Nearest Name requests, %u replies",
	   novell_stat[NOVELL_SAPNEARNAMEIN],
	   novell_stat[NOVELL_SAPNEARNAMEOUT]);
    printf("\n        %u SAP General Name requests, %u replies",
	   novell_stat[NOVELL_SAPGENNAMEIN],
	   novell_stat[NOVELL_SAPGENNAMEOUT]);
    printf("\n        %u SAP advertisements received, %u sent",
	    novell_stat[NOVELL_SAPRESIN], novell_stat[NOVELL_SAPOUT]);
    printf("\n        %u SAP flash updates sent, %u SAP format errors",
	    novell_stat[NOVELL_SAPFLASH], novell_stat[NOVELL_SAPFMTERROR]);
    if (novell_stat[NOVELL_SAPFMTERROR])
	printf(", last seen from %x.%e",
	       nov_last_bad_sap_net, nov_last_bad_sap_host);
    printf("\nRIP:    %u RIP requests, %u RIP replies, %d routes", 
           novell_stat[NOVELL_RIPREQIN], novell_stat[NOVELL_RIPREPLY], 
           novell_route_count); 
    printf("\n        %u RIP advertisements received, %u sent",
	    novell_stat[NOVELL_RIPRESIN], novell_stat[NOVELL_RIPOUT]);
    printf("\n        %u RIP flash updates sent, %u RIP format errors",
	    novell_stat[NOVELL_RIPFLASH], novell_stat[NOVELL_RIPFMTERROR]);
    if (novell_stat[NOVELL_RIPFMTERROR])
	printf(", last seen from %x.%e",
	       nov_last_bad_rip_net, nov_last_bad_rip_host);
    printf("\nEcho:   Rcvd %d requests, %d replies",
	   novell_stat[NOVELL_ECHOREQIN], novell_stat[NOVELL_ECHOREPIN]);
    printf("\n        Sent %d requests, %d replies",
	   novell_stat[NOVELL_ECHOREQOUT], novell_stat[NOVELL_ECHOREPOUT]);
    printf("\n        %u unknown: %u no socket, %u filtered, %u no helper",
	   novell_stat[NOVELL_UNKNOWN],novell_stat[NOVELL_UNKNOWN_SOCKET],
	   novell_stat[NOVELL_UNKNOWN_INFILT],
	   novell_stat[NOVELL_UNKNOWN_NOHELP]);
    printf("\n        %u SAPs throttled, freed NDB len %u" ,
	   novell_stat[NOVELL_THROTTLED], nov_freed_ndbQlen);
    printf("\nWatchdog:\n        %d packets received, %d replies spoofed",
	   novell_stat[NOVELL_WATCHDOG_IN], novell_stat[NOVELL_WATCHDOG_OUT]);
    printf("\nQueue lengths:\n        IPX input: %d, SAP %d, RIP %d, GNS %d",
	   novQlen, novbgQlen, novell_ripQlen, novell_gnsQlen);
    printf("\n        SAP throttling length: %d/",
	   (novQlen + novbgQlen + novell_ripQlen + novell_gnsQlen));
    if (novell_sapqueuemax) {
	printf("%u", novell_sapqueuemax);
    } else {
	printf("(no limit)");
    }
    printf(", %d nets pending lost route reply",
	   novell_count_atlr_list_length());
    printf("\n        Delayed process creation: %d", novell_stat[NOVELL_EXCEED_PROC]);
    printf("\nEIGRP:  Total received %d, sent %d", novell_stat[NOVELL_IGRPIN],
	novell_stat[NOVELL_IGRPOUT]);
    printf("\n        Updates received %d, sent %d", 
	novell_stat[NOVELL_UPDATE_IN], novell_stat[NOVELL_UPDATE_OUT]);
    printf("\n        Queries received %d, sent %d",
	novell_stat[NOVELL_QUERY_IN], novell_stat[NOVELL_QUERY_OUT]);
    printf("\n        Replies received %d, sent %d",
	novell_stat[NOVELL_REPLY_IN], novell_stat[NOVELL_REPLY_OUT]);
    printf("\n        SAPs received %d, sent %d", 
	novell_stat[NOVELL_IGRP_SAPIN], novell_stat[NOVELL_IGRP_SAPOUT]); 
    reg_invoke_ipx_show_nlsp_traffic();
}

/*
 * show_novell_spx_spoof
 * Print out Novell spx spoof table
 *
 */

void show_novell_spx_spoof (void)
{
    uchar hashindex;
    nov_spx_spoof_record *spoofrecordp;

    automore_enable(NULL);
    printf("\n Local SPX Network.Host:sock Cid  Remote SPX Network.Host:sock Cid Seq Ack Idle");
    for (hashindex = 0; hashindex < SPOOF_HASH_TABLE_ENTRIES; hashindex++) {
	spoofrecordp = spoofhashtable[hashindex];
	while (spoofrecordp != NULL) {
	  if ((spoofrecordp->destnet != 0) && (spoofrecordp->destsocket != 0) &&
	      TIMER_RUNNING(spoofrecordp->timelastchanged)) {
	    printf("\n%-8x.%e:%04x %04x %-8x.%e:%04x %04x %02x %04x %d",
		   spoofrecordp->destnet,
		   spoofrecordp->destnode, spoofrecordp->destsocket,
		   spoofrecordp->destconnectid,spoofrecordp->sourcenet,
		   spoofrecordp->sourcenode, spoofrecordp->sourcesocket,
		   spoofrecordp->sourceconnectid,spoofrecordp->sequencenumber,
		   spoofrecordp->acknumber,
		   (CLOCK_OUTSIDE_INTERVAL(spoofrecordp->timelastchanged, ONESEC) ?
		   (ELAPSED_TIME(spoofrecordp->timelastchanged)/ONESEC) : 0));
	  } 
	    spoofrecordp = spoofrecordp->chain;
	}
	if (automore_quit())
	    break;
    } 
    automore_disable();
}

/*
 * parse_novelladdress
 * Parse and return an XNS address 
 */

boolean parse_novelladdress (
    char *buf,
    ulong *net,
    uchar *host)
{
    char netstring[8*sizeof(ulong) + 10];
    int netindx;
    boolean zero_flag;

    buf = deblank(buf);
    zero_flag = TRUE;
    if (buf[0] == '-' && buf[1] == '1' && buf[2] == '.') {
	*net = -1L;
	buf = &buf[2];
    } else {
	for (netindx = 0;
	     (netindx < sizeof(netstring) - 2) && *buf && (*buf != '.');
	     netindx++) {
	    if (*buf != '0') zero_flag = FALSE;
	    netstring[netindx] = *buf++;
	  }
	netstring[netindx++] = '\0';
	*net = parse_hex_long(netstring);
	if (*net == 0 && !zero_flag)
	  return(FALSE);
    }
    if (*buf++ != '.')
	return(FALSE);
    if (!parse_ether_address(buf, host))
	return(FALSE);
    return(TRUE);
}
 

/*
 * novell_forward
 * see if we should forward a packet to a helper address.
 */
boolean
novell_forward (paktype *pak)
{
    novaddtype *ptr;
    novndbtype *p;
    paktype *newpak;
    ipxhdrtype *ipx, *newipx;
    nidbtype *nidb = pak->nif_input;
    idbtype *idb = nidb->idb;
    ulong snet;
    boolean type20;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    type20 = ((ipx->pt == NOVELL_TYPE_NETBIOS) &&
	      ieee_equal(ipx->dhost, novell_allhosts));

    if ((type20) && (nidb != NULL) && (nidb->idb != NULL)) {
      /* Filter according to any NetBIOS lists */
      if (!novell_netbios_acfcheck(pak,
				   IPX_ACCESS_DIRECTION_INCOMING,
				   nidb)) {
	if (novell_debug)
	  nov_netbios_buginf(pak, "inbound filtered");
	novell_stat[NOVELL_FILTER_IN]++;
	return(FALSE);
      }
    }

    snet = GET2WD(ipx->ssnet);
    if (snet == 0L) {
	snet = nidb->novellnet;
	PUT2WD(ipx->ssnet, snet);
    }
    p = novrip_find_rt(snet, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_FUZZ);

    if (p == NULL) {
	if (novell_debug)
	    buginf("\nIPXForward: no known route to src net %x.%e, dropped.",
		   snet, ipx->shost);
	return(FALSE);
    }
	
    /*
     * Bail if not broadcast. This means it was unicast to us and
     * not to a socket which got queued elsewhere.
     */
    if ((pak->flags & PAK_BROADCAST) == 0) {
       novell_stat[NOVELL_UNKNOWN_SOCKET]++;
       if (novell_debug)
               buginf("\nIPX: unknown socket %x.%e %x to %x.%e %x",
                    snet, ipx->shost, ipx->ssock,
                    GET2WD(ipx->ddnet), ipx->dhost, ipx->dsock);
       return(FALSE);
    }
    /*
     * There must be a helper address configured
     * (unless the packet is a type 20 propagation packet).
     */
    if ((!idb->nov_helpernetQ.qhead) && !type20) {
        novell_stat[NOVELL_UNKNOWN_NOHELP]++;
	return(FALSE);
    }
    /* 
     * Filter packet via helper-list on input interface!
     */
    if (!nov_accesscheck(pak, idb->novell_helperlist)) {
        novell_stat[NOVELL_UNKNOWN_INFILT]++;
	return(FALSE);
    }
    /*
     * IPX Type 20 Propagation Packet (NETBIOS) special handling:
     */
    if (type20 && !novell_type20_helpered) {
	nov_flood(pak, p, nidb, IPX_FLOOD_TYPE20);
        retbuffer(pak);
	return(TRUE);
    }
    newpak = NULL;
    for (ptr = (novaddtype *)idb->nov_helpernetQ.qhead; ptr; ptr = ptr->next) {
        if (ptr->novell_net == -1L)
	    nov_flood(pak, p, nidb, IPX_FLOOD_NORMAL);
        else {
	    /* Optimization: do not dup pak if this is last helper entry */
	    if (ptr->next != NULL)
	        newpak = pak_duplicate(pak);
	    else
	        newpak = pak;

	    /* End of optimization */
	    if (newpak == NULL)
		return(FALSE);

	    newipx = (ipxhdrtype *)ipxheadstart(newpak);

	    PUT2WD(newipx->ddnet, ptr->novell_net);
	    ieee_copy(ptr->novell_host, newipx->dhost);
	    if (novell_debug)
	       buginf("\nIPX: broadcast from %x.%e helpered to %x.%e",
		    GET2WD(ipx->ssnet), ipx->shost, 
		    GET2WD(newipx->ddnet), newipx->dhost);
	    clear_if_input(newpak);
	    nov_forward(newpak);
	    novell_stat[NOVELL_FWDBRD]++;
	}
    }
    /* Return pak UNLESS it was forwarded (as newpak) above: */
    if (pak != newpak)
        retbuffer(pak);
    return(TRUE);
}

/*
 * nov_flood
 * Flood a packet to all possible interfaces.
 * Note that "pak" is NOT deallocated!
 */
void
nov_flood (
    paktype *pak,
    novndbtype *p,
    nidbtype *ninterface,
    enum IPX_FLOOD flood_type)
{
    ipxhdrtype *ipx, *newipx;
    paktype *newpak;
    nidbtype *nidb;
    boolean type20;

    ipx = (ipxhdrtype *)ipxheadstart(pak);

    type20 = ((ipx->pt == NOVELL_TYPE_NETBIOS) &&
	      ieee_equal(ipx->dhost, novell_allhosts));

    FOR_ALL_NIDBS(nidb) {
	if (NOVELL_NIDB_IDB_UP(nidb)) {
	    /*
	     * Never flood back out incoming interface!
	     */
	    if (nidb == ninterface)
		continue;
	    /*
	     * Only flood away from source network.
	     * Apply this check to type 20 only if enabled.
	     */
	    if (((flood_type == IPX_FLOOD_TYPE20) &&
		 !novell_type20_out_checks) ||
		(!nov_same_nidb(nidb, p->path_list))) {
		/*
		 * Do not propagate type 20 unless enabled.
		 */
		if (flood_type == IPX_FLOOD_TYPE20) {
		    if (!nidb->idb->novell_type20)
			continue;
		    if (nov_type20_output_loop(pak, nidb->novellnet))
			continue;
		}
		if (type20) {
		  if (!novell_netbios_acfcheck(pak,
					       IPX_ACCESS_DIRECTION_OUTGOING,
					       nidb)) {
		    novell_stat[NOVELL_FILTER_OUT]++;
		    if (novell_debug)
		      nov_netbios_buginf(pak, "outbound filtered");
		    continue;
		  }
		}
		newpak = pak_duplicate(pak);
		if (newpak == NULL)
		    return;
		newipx = (ipxhdrtype *)ipxheadstart(newpak);
		ieee_copy(novell_allhosts, newipx->dhost);
		PUT2WD(newipx->ddnet, nidb->novellnet);
		newpak->nif_output = nidb;
		clear_if_input(newpak);
		if (novell_debug)
		    buginf("\nIPX: broadcast from %x.%e flooded to %x.%e",
			   GET2WD(ipx->ssnet), ipx->shost, 
			   GET2WD(newipx->ddnet), newipx->dhost);
		nov_forward(newpak);
		novell_stat[NOVELL_FWDBRD]++;
	    }
	}
    }
}


/*
 * Determine if a sap entry matches a sap access-list entry.
 * Return FALSE if "access" is denied.
 *
 * Note that at the present time, this routine is used for *each* of the
 * following:
 *
 *     - input-sap-filter
 *     - output-sap-filter
 *     - output-gns-filter
 */
boolean
nov_sapcheck (
    novell_serveridtype *server,
    ushort service,
    int accesslist)
{
    ulong flags;
    mac_address_type host;
    xnseacctype *item;
    char *wildptr;
    int wildcard = 0;

    if (!accesslist)
	return(TRUE);
    item = ipx_get_ext_acl(accesslist);
    if (item == NULL) 
	return(TRUE);

    do {
	flags = item->flags;
	/*
	 * A network of -1 indicates all XNS networks.
	 * A socket of 0 indicates all XNS sockets.
	 */
	if (flags & XNS_SRCNET) {
	    if ((item->srcnet != -1L) &&
		(item->srcnet != (server->dnet & item->srcnetmask)))
		continue;
	}
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	    ieee_copy((uchar *)server->dhost, (uchar *)&host);
	    if ((host.u.sword[2] & item->srchostmask.u.sword[2])
		!= item->srchost.u.sword[2])
		continue;
	    if ((host.u.lword[0] & item->srchostmask.u.lword[0])
		!= (item->srchost.u.lword[0]))
		continue;
	}
	if (flags & XNS_SERVER_TYPE) {
	    if (item->protocol && (item->protocol != service))
		continue;
	}
	if (flags & XNS_SERVER_NAME) {
	    if ((wildptr = strchr(item->server_name,'*')) == NULL) {
		/* Simple compare, no trailing wildcard */
		if (strcmp(item->server_name, server->server_name) != 0)
		    continue;
	    } else {
		/* Trailing wildcard. Accept partial match. */
		wildcard = (ulong)wildptr - (ulong)&item->server_name;
		if (strncmp(item->server_name, server->server_name,
			    wildcard) != 0)
		    continue;
	    }
	}
	return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}


/*
 * Determine if we are allowed to forward the passed datagram, based on the
 * XNS access pointer. Return FALSE if output access is denied.
 */
boolean
nov_saproutercheck (
    ulong snet,
    uchar *shost,
    ushort service,
    char *name,
    int accesslist)
{
    ulong flags;
    mac_address_type host;
    xnseacctype *item;
    
    if (!accesslist)
	return(TRUE);
    item = ipx_get_ext_acl(accesslist);
    if (item == NULL) 
	return(TRUE);

    ieee_copy(shost, (uchar *)&host);
    do {
	flags = item->flags;
	/*
	 * A network of -1 indicates all Novell networks.
	 * A socket of 0 indicates all Novell sockets.
	 */
	if (flags & XNS_SRCNET) {
	    if ((item->srcnet != -1L) &&
		(item->srcnet != (snet & item->srcnetmask)))
		continue;
	}
	if (flags & XNS_SRCHOST) {
	    /*
	     * Check for match on source host address
	     */
	    if ((host.u.lword[0] & item->srchostmask.u.lword[0])
		!= (item->srchost.u.lword[0]))
		continue;
	    if ((host.u.sword[2] & item->srchostmask.u.sword[2])
		!= item->srchost.u.sword[2])
		continue;
	}
	if (flags & XNS_SERVER_TYPE) {
	    if (item->protocol && (item->protocol != service))
		continue;
	}
	if (flags & XNS_SERVER_NAME) {
	    if (strcmp(item->server_name, name) != 0)
		continue;
	}
	return(item->grant);
    } while ((item = item->next));
    return(FALSE);
}


/*
 * novell_netbios_acfcheck
 *
 * Return true if the packet should be forwarded. Assumes pak->datagramstart
 * points to beginning of IPX header. pak->if_input and pak->if_output are
 * set correctly. 'Incoming' indicates if input or output filters should be
 * applied.
 */
boolean novell_netbios_acfcheck (
    paktype *pak,
    enum IPX_ACCESS_DIRECTION direction,
    nidbtype *nidb)
{
    ipxhdrtype *ipx;
    novellnetbiosheader *netbios;
    netbios_sb_t *netb_sb;

    ipx = (ipxhdrtype *)ipxheadstart(pak);
    netbios = (novellnetbiosheader *)ipxdatastart(pak);

    /* First check if we have netbios access lists configured */


    netb_sb = netbios_get_sb(nidb->idb);
    if (!netb_sb) {
        return(TRUE);
    }

    if (direction == IPX_ACCESS_DIRECTION_OUTGOING) {
      if ((!netb_sb->nov_netbios_h_oacf) &&
	  (!netb_sb->nov_netbios_b_oacf)) 
	return(TRUE);
    } else {
      if ((!netb_sb->nov_netbios_h_iacf) &&
	  (!netb_sb->nov_netbios_b_iacf)) 
	return(TRUE);
    }

    /* Check to make sure that it is a Novell NetBIOS frame we care about */
    if ((ipx->pt != NOVELL_TYPE_NETBIOS) ||
	(ipx->dsock != NOVELL_SOCK_NETBIOS) ||
	(netbios->nov_netbios_cmd != NOVELL_NETBIOS_FIND_NAME)) {
	return(TRUE);
    } else {
	return(netbios_acfcheck((uchar *) &(netbios->nov_netbios_nets[0]),
				(pak->datagramsize - XNSHEADERBYTES),
				nidb->idb,
				(direction == IPX_ACCESS_DIRECTION_INCOMING),
				NULL, FALSE,
				NETBIOS_IPX_CHECK));
    }
}

/*
 * novrip_redist_callback
 * Routine used to inform RIP/SAP when a route we are concerned with 
 * has changed state from another protocol. Redistribution flags are
 * checked prior to entry.
 */
static void novrip_redist_callback (
     novpdbtype *pdb,
     novndbtype *ndb,
     int event)
{
    int i;

    if (!pdb->running)
	return;

    /* RIP knows no summary route. Don't bother! */
    if (ndb->rt_mask != NOV_NETMASK_ALL)
	return;

    if (novellredist_debug)
	buginf("\nIPXRIP: Redistribute %s net %x into RIP",
	       novdbg_event2string(event), ndb->dstnet);

    /* Don't bother with Connected routes that are in the network list */

    if (ndb->flags & NET_CONNECTED) {
	for (i = 0; i < pdb->netcount; i++)
	    if (pdb->networks[i] == ndb->dstnet)
		return;
    }
    if (event == ROUTE_DOWN) {
	ndb->delay = NOVELL_INFDELAY;
	ndb->hopcount = NOVELL_MAX_HOPS_ALLOWED;
    }
    novell_mark_changed(ndb);
}

/*
 * novsap_redist_callback
 * Routine used to inform RIP/SAP when a route we are concerned with 
 * has changed state from another protocol. Redistribution flags are
 * checked prior to entry.
 */
static void novsap_redist_callback (
     novpdbtype *pdb,
     novell_sapentrytype *sap,
     int event)
{

    if (!pdb->running)
	return;

    if (novellredist_debug)
	buginf("\nIPXSAP: Redistribute service %x:%s, net %x into SAP",
	       sap->server.server_type,
	       sap->server.server_name,
	       sap->server.dnet);

    mark_sap_changed(sap);
}

/*
 * novmisc_pdb_restart
 *
 */
static void novmisc_pdb_restart (novpdbtype *pdb)
{
    pdb->running = TRUE;
}

/*
 * novmisc_pdbinit
 *
 * (Re)Initialize connected and static pdbs.
 *
 * Note: May be called multiple times.
 *
 */
void novmisc_pdbinit (void)
{
    /*
     * Allocate pdb index once and only once:
     */
    if (!novell_started) {
	(void)allocate_novpdbindex(&novconnected_pdb);
	(void)allocate_novpdbindex(&novstatic_pdb);
	(void)allocate_novpdbindex(&novfstatic_pdb);
    }

    /*
     * (Re)Init common data and mark as "running"
     */
    novconnected_pdb.proctype = NET_CONNECTED;
    novconnected_pdb.name = "connected";
    novconnected_pdb.distance = CONNECTED_DISTANCE;
    novconnected_pdb.restart = novmisc_pdb_restart;
    novconnected_pdb.running = TRUE;

    novstatic_pdb.proctype = NET_STATIC;
    novstatic_pdb.name = "static";
    novstatic_pdb.distance = STATIC_DISTANCE;
    novstatic_pdb.restart = novmisc_pdb_restart;
    novstatic_pdb.running = TRUE;

    novfstatic_pdb.proctype = NET_FLOATING_STATIC;
    novfstatic_pdb.name = "floating-static";
    novfstatic_pdb.distance = STATIC_DISTANCE;
    novfstatic_pdb.add_route = nov_add_floating;
    novfstatic_pdb.restart = novmisc_pdb_restart;
    novfstatic_pdb.running = TRUE;

    /*
     * Add to pdb list once ... these pdbs will live forever:
     */
    if (!novell_started) {
	enqueue(&novpdbQ, &novconnected_pdb);
	enqueue(&novpdbQ, &novstatic_pdb);
	enqueue(&novpdbQ, &novfstatic_pdb);
    }
}

/*
 * novell_common_restart
 *
 * Novell startup code common to all protocols. Called by pdb->restart
 * routines as appropriate. Note that at the present time, protocols
 * which need partial RIP/SAP functionality need to create at least the
 * RIP and SAP processes. For example, NLSP usually needs to respond to
 * specific RIP requests and SAP GNS even if no updates are generated.
 * The RIP/SAP processes now look at the state of novell_rip_running
 * to decide whether to generate periodic updates.
 *
 * It is a *future* project to determine how to subset RIP/SAP functionality
 * and to figure out if the other ancillary support processes need to be
 * present in all configurations. For now, whenever RIP/SAP are created,
 * everything else is thrown in to the bargain.
 *
 */
void novell_common_restart (void)
{
    if (novell_inp_pid == 0) {
	novell_inp_pid = process_create(nov_input, "IPX Input", NORMAL_STACK,
					PRIO_HIGH);
	if (novell_inp_pid == NO_PROCESS)
	    novell_inp_pid = 0;
    }
    if (novrip_pdb.pid == 0) {
	GET_TIMESTAMP(novell_ripagertimer);
	GET_TIMESTAMP(novell_onemin_timer);
	novrip_pdb.pid = cfork((forkproc *)novell_router,
			       (long)&novrip_pdb, 0, "IPX RIP In", 0);
	if (novrip_pdb.pid == NO_PROCESS)
	    novrip_pdb.pid = 0;
    }
    if (novell_sap_pid == 0) {
	GET_TIMESTAMP(novell_sapagertimer);
	GET_TIMESTAMP(novell_freed_ndb_timer);
	novell_sap_pid = cfork(novell_fork, 0L, 0, "IPX SAP In", 0);
	if (novell_sap_pid == NO_PROCESS)
	    novell_sap_pid = 0;
    }
    if (novell_spv_pid == 0) {
	novell_spv_pid = cfork(novell_supervisor,
			      0L, 0, "IPX Event Mgr", 0);
	if (novell_spv_pid == NO_PROCESS)
	    novell_spv_pid = 0;
    }
    if (novell_rup_pid == 0) {
	novell_rup_pid = cfork(novell_rip_update_daemon,
				   0L, 0, "IPX RIP Out", 0);
	if (novell_rup_pid == NO_PROCESS)
	    novell_rup_pid = 0;
    }
    if (novell_sup_pid == 0) {
	novell_sup_pid = cfork(novell_sap_update_daemon,
				   0L, 0, "IPX SAP Out", 0);
	if (novell_sup_pid == NO_PROCESS)
	    novell_sup_pid = 0;
    }
    if (novell_net_pid == 0) {
	novell_net_pid = cfork(novell_net_down_handler,
			      0L, 0, "IPX Net Mgr", 0);
	if (novell_net_pid == NO_PROCESS)
	    novell_net_pid = 0;
    }
    if (novell_gns_pid == 0) {
	novell_gns_pid = cfork(novell_rcv_nearquery,
			       0L, 0, "IPX GNS", 0);
	if (novell_gns_pid == NO_PROCESS)
	    novell_gns_pid = 0;
    }
    if (novell_fwd_pid == 0) {
	novell_fwd_pid = cfork((forkproc *)novell_forwarder,
			       0L, 0, "IPX Forwarder", 0);
	if (novell_fwd_pid == NO_PROCESS)
	    novell_fwd_pid = 0;
    }
    if (novell_ipxwan_pid == 0) {
	if (ipxwan_enabled) {
	    reg_invoke_novell_ipxwan_start();
	}
    }
    if (novell_ipxwanbg_pid == 0) {
	if (ipxwan_enabled) {
	    reg_invoke_novell_ipxwan_bg_start();
	}
    }
}

/*
 * novell_common_cleanup
 *
 * Novell cleanup code common to all protocols. Called by pdb->cleanup
 * routines as appropriate.
 *
 */
void novell_common_cleanup (void)
{

    /*
     * Only delete processes if this is a complete shutdown of IPX:
     */
    if (!novell_shutting_down) {
	return;
    }
    /*
     * Do not stop novell_fork() since it services the freed NDB queue!
     * However, here it's OK since this is a total shut down of IPX. Make sure
     * that the code which follows can only be executed under this constraint.
     */
    if (novell_inp_pid != 0) {
	process_kill(novell_inp_pid);
	novell_inp_pid = 0;
    }
    if (novell_sap_pid != 0) {
	while (!QUEUEEMPTY(&novbgQ)) {
	    retbuffer(p_dequeue(&novbgQ));
	}
	process_kill(novell_sap_pid);
	novell_sap_pid = 0;
    }
    if (novell_spv_pid != 0) {
	process_kill(novell_spv_pid);
	novell_spv_pid = 0;
    }
    if (novell_rup_pid != 0) {
	process_kill(novell_rup_pid);
	novell_rup_pid = 0;
    }
    if (novell_sup_pid != 0) {
	process_kill(novell_sup_pid);
	novell_sup_pid = 0;
    }
    if (novell_net_pid != 0) {
	process_kill(novell_net_pid);
	novell_net_pid = 0;
    }
    if (novell_gns_pid != 0) {
	while (!QUEUEEMPTY(&novell_gnsQ)) {
	    retbuffer(p_dequeue(&novell_gnsQ));
	}
	process_kill(novell_gns_pid);
	novell_gns_pid = 0;
    }
    if (novell_fwd_pid != 0) {
	while (!QUEUEEMPTY(&novell_genericQ)) {
	    retbuffer(p_dequeue(&novell_genericQ));
	}
	process_kill(novell_fwd_pid);
	novell_fwd_pid = 0;
    }
    if (novell_ipxwan_pid != 0) {
	while (!QUEUEEMPTY(&novell_ipxwanQ)) {
	    retbuffer(p_dequeue(&novell_ipxwanQ));
	}
	process_kill(novell_ipxwan_pid);
	novell_ipxwan_pid = 0;
    }
    if (novell_ipxwanbg_pid != 0) {
	process_kill(novell_ipxwanbg_pid);
	novell_ipxwanbg_pid = 0;
    }
    /*
     * Delete unreachable route data:
     */
    nov_delete_urd_table();
}

/*
 * novrip_restart
 *
 * RIP pdb startup vector
 *
 */
static void novrip_restart (novpdbtype *pdb)
{
    novpdbtype *nlsp_pdb;

    if (pdb->running) {
	return;
    }
    novell_common_restart();

    /*
     * Always redistribute RIP into NLSP.
     */
    if (novell_nlsp_enabled) {
	for (nlsp_pdb = (novpdbtype *) novpdbQ.qhead; nlsp_pdb; 
	     nlsp_pdb = nlsp_pdb->next) {
	    if (nlsp_pdb->proctype == NET_NLSP) 
		if (nlsp_pdb->running) {
		    reg_invoke_ipx_nlsp_set_default_redist(nlsp_pdb);
		}
	}
    }

    novell_rip_running = pdb->running = TRUE;
}

/*
 * novrip_cleanup
 *
 * RIP pdb shutdown vector
 *
 */
static void novrip_cleanup (novpdbtype *pdb)
{
    novell_common_cleanup();
}

/*
 * novrip_pdbinit
 *
 * Initialize a static pdb for the RIP process. Used by IGRP/ISIS/NLSP
 *
 * NOTE: For historical reasons, this routine is called from ALL OVER
 *       THE PLACE, not always in association with actual process
 *       creation. For this reason, it is up to the CALLER to set
 *       the following to TRUE if appropriate:
 *
 *       - novrip_pdb.running
 *       - novell_rip_running
 */
void novrip_pdbinit (void)
{
    novpdbtype *pdb;
    boolean new_allocation;

    /* 
     * If first time, allocate a router process flag and enqueue
     * the pdb. Also set running and redistribution flags up.
     */

    new_allocation = allocate_novpdbindex(&novrip_pdb);

    if (new_allocation) {
	enqueue(&novpdbQ, &novrip_pdb);
	queue_init(&novrip_pdb.inputq, 0);
	queue_init(&novrip_pdb.workQ, 0);
	novrip_pdb.pid = 0;
	novrip_pdb.running = FALSE;
	novrip_pdb.proctype = NET_RIP;
	novrip_pdb.name = "rip";
	novrip_pdb.pname = "IPX-RIP Router";
	novrip_pdb.process = novell_router;
	novrip_pdb.cleanup = NULL;
	novrip_pdb.distance = novrip_pdb.def_distance = RIP_DISTANCE;
	novrip_pdb.asystem = 0; 
	novrip_pdb.distflag = novrip_pdb.mask;
	/* maybe these should be novrip_pdb.default_metric? */
	novrip_pdb.redist_hops = 0;
	novrip_pdb.redist_delay = 0;
	novrip_pdb.redist_callback = novrip_redist_callback;
	novrip_pdb.sap_redist_callback = novsap_redist_callback;
	novrip_pdb.default_include_net = TRUE;
	novrip_pdb.restart = novrip_restart;
	novrip_pdb.cleanup = novrip_cleanup;
    }
    /*
     * Default behavior is to redistribute static/connected routes
     * and EIGRP via RIP. Other protocols will define their own default 
     * behavior vis-a-vis RIP when they create their own pdbs.
     * Redistributing floating static routes is very dangerous,
     * don't allow it by default.
     */
    novrip_pdb.distflag |= (novconnected_pdb.mask | novstatic_pdb.mask);
    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
	if (pdb->proctype == NET_IGRP) {
	    novrip_pdb.distflag |= pdb->mask;
	    pdb->distflag |= novrip_pdb.mask;
	}
    }
}


/* novell_send_gen_sap_query
 *
 * Send a general SAP Query. A Novell SAP General Query is a Novell SAP packet
 * marked as a query with only one SAP type asked for in the packet this SAP
 * type is NOVELL_EVERYONE 0xFFFF. We send this out when we come up so that
 * our neighbors will fill our SAP tables by sending Replies with all known
 * Services.
 */

void novell_send_gen_sap_query (nidbtype *nidb, ushort server_type)
{
    paktype *pak;
    ipxhdrtype *ipx;
    novell_querytype *sap;
    boolean vdebug = (novellsap_debug && novell_verbose_debug);

    /*
     * Bail if NLSP/ISIS SAP compatibility mode is hard off:
     */
    if (nidb->lsp_sap_cfg == IPX_RIPSAP_CONFIG_OFF) {
	return;
    }

    if (vdebug) buginf("\nIPXSAP: General query via net %x for type %x",
		       nidb->novellnet, server_type);

    pak = getbuffer(MEDDATA);
    if (pak) {
	pak->linktype = LINK_NOVELL;
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	ipx->tc = 0;
	ipx->pt = NOVELL_TYPE_SAPRESP;
	ipx->dsock = NOVELL_SOCK_SAP;
	ipx->ssock = NOVELL_SOCK_SAP;
	PUT2WD(ipx->ddnet, nidb->novellnet);
	PUT2WD(ipx->ssnet, nidb->novellnet);
	ipx->cksum = NOVELL_NO_CHECKSUM;
	ieee_copy(nidb->idb->novell_node, ipx->shost);
	ieee_copy(baddr, ipx->dhost);
	sap = (novell_querytype *)(XNSDATASTART(pak));
	sap->query_type = NOVELL_SAP_GENQUERY;
	sap->server_type = server_type;
	ipx->len = NOVELL_QUERYLENGTH;
	pak->nif_output = nidb;
	if (novellsap_debug || novellsap_event_debug)
	  buginf("\nIPXSAP: General Query src=%x.%e, dst=%x.%e, packet sent",
		 nidb->novellnet, nidb->idb->novell_node,
		 nidb->novellnet, baddr);
	pak->flags |= PAK_PRIORITY;
	if (NOVIGRP_DOING_INCREMENTAL_SAP(nidb)) {
	    reg_invoke_novigrp_send_sap(pak, IPX_DESTINATION_BROADCAST, nidb);
	    if (novellsap_debug) {
		buginf("\nIPXEIGRP: Sending EIGRP SAP general query");
	    }
	} else {
	    if(!nov_send(pak)) {
	      if (novellsap_debug || novellsap_event_debug)
		buginf("\nIPXSAP: cannot send SAP General Query");
	    } else {
	      nidb->idb->nov_sap_out_packets++;
	      novell_stat[NOVELL_SAPREQOUT]++;
	    }
	}
    }
}


/* novell_send_gen_rip_query
 *
 * Send a general RIP Query. A Novell RIP General Query is a Novell RIP packet
 * marked as a query with only one network asked for in the packet this is
 * is 0xFFFFFFFF We send this out when we come
 * up so that our neighbors will fill our RIP table by sending Replies with 
 * all known Routes (networks).
 */
 
void novell_send_gen_rip_query (nidbtype *nidb)
{
    paktype *pak;
    ipxhdrtype *ipx;
    novellripinfotype *p;
    boolean debug = novell_debug8;

    /*
     * If this nidb is not listed in the router subcommand "network", then
     * do not send RIP query out this interface. All novell nets on the
     * router should be listed in novrip_pdb.networks[] by default.
     */
    if (!novell_rip_network_enable(nidb))
        return;
    /*
     * Bail if NLSP/ISIS RIP compatibility mode is hard off:
     */
    if (nidb->lsp_rip_cfg == IPX_RIPSAP_CONFIG_OFF) {
	return;
    }

    if (debug) buginf("\nsend_gen_rip_query: via net %x", nidb->novellnet);

    pak = getbuffer(MEDDATA);
    if (pak) {
	pak->linktype = LINK_NOVELL;
	ipx = (ipxhdrtype *)ipxheadstart(pak);
	ipx->tc = 0;
	ipx->pt = NOVELL_TYPE_RIP;
	ipx->dsock = NOVELL_SOCK_RIP;
	ipx->ssock = NOVELL_SOCK_RIP;
	PUT2WD(ipx->ddnet, nidb->novellnet);
	PUT2WD(ipx->ssnet, nidb->novellnet);
	ipx->cksum = NOVELL_NO_CHECKSUM;
	ieee_copy(nidb->idb->novell_node, ipx->shost);
	ieee_copy(baddr, ipx->dhost);
	PUTSHORT((ipxheadstart(pak) + XNSOVERHEADBYTES), RIP_REQUEST);
	p = (novellripinfotype *)(XNSDATASTART(pak) + 2);
	bcopy(baddr,p->nnet,XNS_NETBYTES);
	p->metric = 0x00;
	p->delay = 0x00;
	ipx->len = XNSOVERHEADBYTES + 2 + sizeof(novellripinfotype);
	if (novellrip_debug || novellrip_event_debug) {
	  buginf("\nIPXRIP: General Query src=%x.%e, dst=%x.%e, packet sent",
		 nidb->novellnet, nidb->idb->novell_node,
		 nidb->novellnet, baddr);
	}
	nidb->idb->nov_rip_out_packets++;
	novell_stat[NOVELL_RIPREQOUT]++;
	pak->nif_output = nidb;
	pak->flags |= PAK_PRIORITY;
	nov_send(pak);
    }
}

/*
 * novell_set_timer
 * Set the timer on a route or server, and mark whether or not it should
 * age.
 */
void novell_set_timer (novrdbtype *rdb, novell_sapentry_rdb *srdb)
{
    if (rdb != NULL) {
	GET_TIMESTAMP(rdb->timer);
	rdb->snapshot_no_age =
	  reg_invoke_snapshot_aging_denied(rdb->nidb->idb);
    }
    if (srdb != NULL) {
	GET_TIMESTAMP(srdb->timer);
	srdb->snapshot_no_age =
	  reg_invoke_snapshot_aging_denied(srdb->nidb->idb);
    }
}

/*
 * novell_start_aging
 *
 * Start the aging of all routes and servers learned on the given HWIDB
 */
void novell_start_aging (idbtype *idb, paktype *pak)
{
    int i;
    novndbtype *ndb;
    novrdbtype *rdb;
    novell_sapentry_rdb *srdb;
    novell_sapqueuetype *sapq;
    novell_sapentrytype *entry;
    ipxhdrtype *ipx = (ipxhdrtype *)((pak != NULL) ? ipxheadstart(pak) : NULL);
    ulong sourcenet = (ipx != NULL) ? GET2WD(ipx->ssnet) : 0L;
    
    if (!novell_running || novell_shutting_down)
	return;

    /* Go through routes */
    for (i = 0; i < NOV_NETHASHLEN; i++) {
        for (ndb = (*novell_table)[i]; ndb != NULL; ndb = ndb->next) {
	    if (TIMER_RUNNING(ndb->holddownuntil)) {
		continue;
	    }
	    for (rdb = ndb->path_list; rdb != NULL; rdb = rdb->next) {
		if ((rdb->nidb != NULL) && (rdb->nidb->idb == idb) &&
		    (rdb->snapshot_no_age)) {
		    if ((ipx == NULL) ||
			((sourcenet == rdb->srcnet) &&
			 (ieee_equal(rdb->srchost, ipx->shost)))) {
			rdb->snapshot_no_age = FALSE;
			GET_TIMESTAMP(rdb->timer);
		    }
		}
	    }
	}
    }

    /* Do the same for the patricia tree */
    rn_walktree_blocking(nov_rnhead->rnh_treetop, patricia_start_aging, idb, 
			 pak);

    /* Go through servers */
    FOR_ALL_SAPS(entry, sapq) {
	for (srdb = entry->paths; srdb != NULL; srdb = srdb->next) {
	    if ((srdb->nidb != NULL) &&
		(srdb->snapshot_no_age) &&
		(srdb->nidb->idb == idb)) {
		if ((ipx == NULL) ||
		    ((sourcenet == srdb->snet) &&
		     (ieee_equal(srdb->srchost, ipx->shost)))) {
		    srdb->snapshot_no_age = FALSE;
		    GET_TIMESTAMP(srdb->timer);
		}
	    }
	}
    }
}

/*
 * novell_swif_erase
 * The system has told us to delete this sw idb... do it NOW
 */
void novell_swif_erase (idbtype *idb)
{
    nidbtype *nidb, *nidbnext;
    
    FOR_ALL_NIDBS_USE_SAVED_NEXT(nidb, nidbnext) {
	if (nidb->idb == idb) {
	    novell_delete_network(nidb);
	}
    }
}

/*
 * novell_set_max_hops
 * Set the maximum hops to the value given, and infinity to one 'above' that.
 */
void novell_set_max_hops (ulong value)
{
    novell_max_hops = value;
    novell_infinity = novell_max_hops + 1;

    /* Call Autonomous and SSE, tell them new max hops! */
}

/*
 * nov_get_each_route--Return each IPX route in the routing table
 *
 * The state variables are updated so we can find our way back next
 * time.
 *
 * The caller should not suspend while calling this routine--instead
 * the caller should set "suspend_allowed" TRUE when calling, and
 * this routine will suspend when it's safe.
 *
 * If the "last_ndb" pointer is NULL, we assume that we're being
 * called for the first time.
 *
 * Note that this routine returns everything in the IPX routing table,
 * including unreachable routes.  It is up to the caller to do appropriate
 * filtering.
 */
novndbtype *nov_get_each_route (novndbtype *last_ndb,
				int *table_index,
				boolean suspend_allowed)
{
    novndbtype *new_ndb;
    int local_index;

    local_index = *table_index;

    /* If first time, use the first entry. */

    if (last_ndb == NULL) {
	local_index = 0;
	new_ndb = (*novell_table)[0];
    } else {

	/* Not the first time;  advance the pointer. */

	new_ndb = last_ndb->next;
    }

    /*
     * If we're at the end of an NDB chain, possibly suspend, and then
     * pick up the next NDB chain.
     */
    while (new_ndb == NULL) {
	if (suspend_allowed) {
	    process_may_suspend();
	}
	(local_index)++;
	if (local_index >= NOV_NETHASHLEN)
	    break;
	new_ndb = (*novell_table)[local_index];
    }

    *table_index = local_index;
    return(new_ndb);
}

/*
 * nov_get_each_route_per_index -- Return each IPX route in the
 * routing table for a given table entry.
 *
 * The state variables are updated so we can find our way back next
 * time.
 *
 * If the "last_ndb" pointer is NULL, we assume that we're being
 * called for the first time.
 *
 * Note that this routine returns everything in the IPX routing table,
 * including unreachable routes.  It is up to the caller to do appropriate
 * filtering.
 */
novndbtype *nov_get_each_route_per_index (novndbtype *last_ndb,
					  int *table_index,
					  boolean suspend_allowed)
{
    novndbtype *new_ndb;
    int local_index;

    local_index = *table_index;

    if (local_index >= NOV_NETHASHLEN)
	return(NULL);

    /* If first time, use the first entry. */

    if (last_ndb == NULL) {
	new_ndb = (*novell_table)[local_index];
    } else {

	/* Not the first time;  advance the pointer. */

	new_ndb = last_ndb->next;
    }

    /*
     * If we're at the end of an NDB chain, possibly suspend before
     * returning a NULL result.
     */
    if (new_ndb == NULL) {
	if (suspend_allowed) {
	    process_may_suspend();
	}
    }

    return(new_ndb);
}

/*
 * nov_get_each_service--Return each SAP service in the SAP net hash table
 *
 * The state variables are updated so we can find our way back next
 * time.
 *
 * The caller should not suspend while calling this routine--instead
 * the caller should set "suspend_allowed" TRUE when calling, and
 * this routine will suspend when it's safe.
 *
 * If the "last_sap" pointer is NULL, we assume that we're being
 * called for the first time.
 */
novell_sapentrytype *nov_get_each_service (novell_sapentrytype *last_sap,
					   int *table_index,
					   boolean suspend_allowed)
{
    novell_sapentrytype *next_sap;
    int local_index;
    
    local_index = *table_index;

    /* If first time, use the first entry. */
    
    if (last_sap == NULL) {
	local_index = 0;
	next_sap = (*novell_sap_route_hashtable)[0];
    } else {

	/* Not the first time;  advance the pointer. */

	next_sap = last_sap->net_hash_next;
    }

    /*
     * If we're at the end of a SAP chain, possibly suspend, and then
     * pick up the next SAP chain.
     */
    while (next_sap == NULL) {
	if (suspend_allowed) {
	    process_may_suspend();
	}
	(local_index)++;
	if (local_index >= NOV_NETHASHLEN)
	    break;
	next_sap = (*novell_sap_route_hashtable)[local_index];
    }
    *table_index = local_index;
    return(next_sap);
}

/*
 * nov_get_each_service_per_index -- Return each SAP service in the
 * SAP net hash table for a particular table_index.
 *
 * The state variables are updated so we can find our way back next
 * time.
 *
 * The caller should not suspend while calling this routine--instead
 * the caller should set "suspend_allowed" TRUE when calling, and
 * this routine will suspend when it's safe.
 *
 * If the "last_sap" pointer is NULL, we assume that we're being
 * called for the first time.
 */
novell_sapentrytype
    *nov_get_each_service_per_index (novell_sapentrytype *last_sap,
				     int *table_index,
				     boolean suspend_allowed)
{
    novell_sapentrytype *next_sap;
    int local_index;
    
    local_index = *table_index;
    
    if (local_index >= NOV_NETHASHLEN)
 	return(NULL);
    
    /* If first time, use the first entry. */
    
    if (last_sap == NULL) {
 	next_sap = (*novell_sap_route_hashtable)[local_index];
    } else {
	
 	/* Not the first time;  advance the pointer. */
	
 	next_sap = last_sap->net_hash_next;
    }
    
    /*
     * If we're at the end of a SAP chain, possibly suspend before
     * returning a NULL result.
     */
    if (next_sap == NULL) {
 	if (suspend_allowed) {
	    process_may_suspend();
 	}
    }
    
    return(next_sap);
}

/*
 * ipx_insert_backup_info
 *
 * Place 'backup routes or services info' into the backup chain for this
 * route or service.
+  *
+  * NOTE: This routine is NLSP-centric at the present time.
 */
void ipx_insert_backup_info (backuptype **existingbi,
			     backuptype *newbi,
			     novpdbtype *pdb,
			     boolean debug)
{
    backuptype *backup, *last_bi, *next_bi;
    novpdbtype *old_pdb;
    boolean refresh = FALSE;
    ushort new_lsp_index = 0;
    ushort old_lsp_index = 0;
    ushort new_lsp_version = 0;
    ushort old_lsp_version = 0;

    if ((newbi == NULL) || (existingbi == NULL) || (pdb == NULL))
	return;
    
    if (pdb->malloc_backup == FALSE)
	return;

    if (debug) buginf("\nIPX: Backup context %08x from pdb @ %x (%x) ",
 		      newbi->backup_context_info, pdb, pdb->index);
    
    /*
     * Screen new info. Ignore if invalid:
     */
    if (pdb->proctype == NET_NLSP) {
 	new_lsp_index   = (newbi->backup_context_info >> 16);
 	new_lsp_version = (newbi->backup_context_info & 0x0000FFFF);
	if (!reg_invoke_isis_valid_lsp_version(pdb->osi_pdb,
					       new_lsp_index,
					       new_lsp_version)) {
	    if (debug) buginf("(invalid)");
	    return;
	}
    } else {
 	/*
 	 * TBS: Screen non-NLSP info here:
 	 */
 	if (debug) buginf("(non-NLSP)");
    }
    
    /*
     * Check old info and refresh or discard as appropriate:
     */
    last_bi = (backuptype *)existingbi;
    
    for (backup = *existingbi; backup; backup = next_bi) {
	
 	next_bi = backup->next;
	
 	old_pdb = NOV_INDEX2PDB(backup->pdbindex);
	
 	if (debug) buginf("\nIPX: Backup list context %08x from pdb @ %x (%x)",
 			  backup->backup_context_info,
			  old_pdb, old_pdb->index);
	
	if ((pdb == old_pdb) && (pdb->proctype == NET_NLSP)) {
 	    /*
 	     * If a backup route was learned via NLSP, update the information.
 	     * Discard other information if no longer valid to prevent
 	     * unrestricted growth of the list.
 	     */
  	    old_lsp_index   = (backup->backup_context_info >> 16);
  	    old_lsp_version = (backup->backup_context_info & 0x0000FFFF);
 	    
  	    if (new_lsp_index == old_lsp_index) {
 		/*
 		 * New version info ... refresh:
 		 */
 		if (debug) buginf(" NLSP: lsp {%d/%d} -> lsp {%d/%d} ",
 				  old_lsp_index, old_lsp_version,
 				  new_lsp_index, new_lsp_version);
 		backup->backup_context_info = newbi->backup_context_info;
 		backup->route_type = newbi->route_type;
 		refresh = TRUE;
 	    } else {
 		/*
 		 * Screen remaining entries to discard kruft:
 		 */
		if (reg_invoke_isis_valid_lsp_version(pdb->osi_pdb,
						       old_lsp_index,
						       old_lsp_version)) {
 		    if (debug) buginf("(valid)");
 		} else {
 		    if (debug) buginf("(stale)");
 		    last_bi->next = next_bi;
 		    free(backup);
 		    continue;
 		}
 	    }
 	} else {
 	    /*
 	     * TBS: EIGRP refresh context goes here ...
 	     */
 	    if (debug) buginf(" ????: %08x", backup->backup_context_info);
 	}
 	last_bi = backup;
    }
    
    if (refresh) {
 	return;
    }
    
    /*
     * New info ... capture:
     */
    backup = malloc_named(sizeof(backuptype), "IPX backup");
    if (backup == NULL) {
	if (debug) buginf("(no mem)");
	return;
    }
    if (debug) buginf("(new)");
    bcopy((uchar *)newbi, (uchar *)backup, sizeof(backuptype));
    backup->pdbindex = pdb->index;
    backup->next = *existingbi;
    *existingbi = backup;
}

/*
 * ipx_ndb_self_backup
 *
 */
void ipx_ndb_self_backup (novndbtype *ndb)
{
    novrdbtype *rdb;
    backuptype backup;

    if (ndb == NULL) {
	return;
    }
    rdb = ndb->path_list;
    backup.pdbindex = ndb->novpdbindex;
    backup.route_type = RTYPE_L1;
    
    while (rdb != NULL) {
	backup.backup_context_info = ((rdb->lsp_linkage.lsp_index << 16) |
				      rdb->lsp_linkage.lsp_version);
	ipx_insert_backup_info(&(ndb->backup_info), &backup,
			       NOV_INDEX2PDB(ndb->novpdbindex), FALSE);
	rdb = rdb->next;
    }
}

/*
 * ipx_sap_self_backup
 *
 */
void ipx_sap_self_backup (novell_sapentrytype *sap, boolean debug)
{
    backuptype backup;
    novell_sapentry_rdb *srdb;

    if (sap == NULL) {
	return;
    }
    srdb = sap->paths;
    backup.pdbindex = sap->novpdbindex;
    backup.route_type = RTYPE_L1;

    while (srdb != NULL) {
	backup.backup_context_info = ((srdb->lsp_linkage.lsp_index << 16) |
				      srdb->lsp_linkage.lsp_version);
	ipx_insert_backup_info(&(sap->backup_info), &backup,
			       NOV_INDEX2PDB(sap->novpdbindex), debug);
	srdb = srdb->next;
    }
}

/*
 * ipx_delete_backup_list
 *
 */
void ipx_delete_backup_list (backuptype *backup_list)
{
    backuptype *bi, *binext;

    bi = backup_list;
    while (bi != NULL) {
	binext = bi->next;
	free(bi);
	bi = binext;
    }
}


/*
 * nov_srdb_same_idb
 * If this IDB matches any idb in the path list then return TRUE.
 */
boolean
nov_srdb_same_idb (idbtype *idb, novell_sapentry_rdb *srdb)
{
    if (idb) {
	for (; srdb != NULL; srdb = srdb->next) {
	    if ((srdb->nidb != NULL) &&
		(idb == srdb->nidb->idb))
		return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * novrdb_free
 *
 * Common rdb delete routine
 */
void novrdb_free (novrdbtype *rdb)
{
    if (rdb == NULL) {
	return;
    }
    if ((rdb->ndb != NULL) && (rdb->ndb->flags & NET_NLSP)) {
	/* NLSP route */
	if ((rdb->lsp_linkage.lsp_next != NULL) && (rdb->lsp_linkage.lsp_prev == NULL)) {
	    /* Bad lsp chain */
	    crashdump(0);
	}
	reg_invoke_ipx_nlsp_delete_rdb_from_lsp_list(rdb);
    } else {
	/* Not NLSP route */
	if ((rdb->lsp_linkage.lsp_next != NULL) || (rdb->lsp_linkage.lsp_prev != NULL)) {
	    /* Bad lsp chain (on a list) or no NDB back pointer */
	    crashdump(0);
	}
    }
    if (rdb->ndb != NULL) {
	rdb->ndb->nextpath = rdb->ndb->path_list;
    }
    rdb->next = NULL;
    rdb->nidb = NULL;
    free(rdb);
}

/*
 * novsrdb_free
 *
 * Common srdb delete routine
 */
void novsrdb_free (novell_sapentry_rdb *srdb)
{
    if (srdb == NULL) {
	return;
    }
    if ((srdb->sap != NULL) && (srdb->sap->flags & NET_NLSP)) {
	/* NLSP service */
	if ((srdb->lsp_linkage.lsp_next != NULL) &&
	    (srdb->lsp_linkage.lsp_prev == NULL)) {
	    /* Bad lsp chain */
	    crashdump(0);
	}
	reg_invoke_ipx_nlsp_delete_srdb_from_lsp_list(srdb);
    } else {
	if ((srdb->lsp_linkage.lsp_next != NULL) ||
	    (srdb->lsp_linkage.lsp_prev != NULL)) {
	    crashdump(0);
	}
    }
    free(srdb);
    return;
}

/*
 * novell_return_static_counts
 * Return the count of static routes and services
 */
void novell_return_static_counts (int *route_count, int *server_count)
{
    if (route_count != NULL)
      *route_count = nov_static_route_count;
    if (server_count != NULL)
      *server_count = nov_static_server_count;
}

/***** PPP CLIENT STUFF ******/

/* 
 * novell_client_add
 *
 *  Associate a client idb with an nidb.  Basically create the client
 *   record, add it to the nidb clientQ, and point the idb->owner_nidb
 *   to the correct spot.
 */
boolean novell_client_add(nidbtype *target_nidb, idbtype *client_idb)
{
    novell_client *client;

    /* 
     * create the client record and put it on the target's clientQ 
     */
    client = malloc(sizeof(novell_client));
    if (client == NULL) {
	return(FALSE);
    }
    client->idb = client_idb;
    enqueue(&target_nidb->clientQ, client);
    client_idb->owner_nidb = target_nidb;
    return(TRUE);
}

/* 
 * novell_client_delete
 *
 *  Disassociate a client idb from an nidb.  Walk through the clientQ on
 *   the target nidb until we find the idb we're looking for, remove it
 *   from the queue and NULL out its pointer back to its owner.
 */
boolean novell_client_delete(nidbtype *target_nidb, idbtype *client_idb)
{
    novell_client *client;
    int count, Qsize;

    /* 
     *  Go through the queue one by one, looking for the client_idb.
     *    If we find it, free the memory and return TRUE.  Otherwise
     *    return FALSE.
     */
    Qsize = QUEUESIZE(&target_nidb->clientQ);
    for (count = 0; count < Qsize; count++) {
	client = dequeue(&target_nidb->clientQ);
	if (client->idb == client_idb) {
	    client_idb->owner_nidb = NULL;
	    free(client);
	    return(TRUE);
	}
	enqueue(&target_nidb->clientQ, client);
    }
    return(FALSE);
}

/*
 * novell_pppclient_configure
 *
 *  Associate a client idb with a master idb that is running Novell.  Or
 *   undo it if the sense is FALSE.   Called for the "ipx ppp-client" 
 *   interface command, and for the global vty-async as well.
 */
void novell_pppclient_configure(idbtype *target_idb, idbtype *idb, 
					boolean sense)
{    
    nidbtype *nidb = NULL, *tempnidb = NULL;
    nidb = NULL;

    FOR_ALL_NIDBS(tempnidb) {
	if (tempnidb->idb == target_idb)
	    nidb = tempnidb;
    }
    if (!nidb) {
	printf("Interface %s does not have IPX configured.\n", 
			target_idb->namestring);
	return;
    }
    if (sense) {
	/* 
	 * if interface already has nidb then bail.
	 */
	if (idb->owner_nidb) {
	    printf("Interface %s is already a ppp-client.\n", 
		    idb->namestring);
	    return;
	}
	if (novell_client_add(nidb, idb)) {
	    idb->novellnets++;
	} else {
	    printf("Failed to add client to %s.\n", target_idb->namestring);
	}
    } else {
	FOR_ALL_NIDBS(nidb) {
	    if (nidb == idb->owner_nidb) {
		if (novell_client_delete(nidb, idb)) {
		    DECREMENT_NOVELLNETS(idb);
		} else {
		    printf("Failed to delete client from %s.\n", 
				target_idb->namestring);
		}
	    }
	}
    }
}

/*
 * novell_get_owner_idb
 *
 *  return the idb pointed to by the owner_nidb
 * 
 */

idbtype *novell_get_owner_idb(idbtype *idb)
{
    if (idb->owner_nidb) {
	return(idb->owner_nidb->idb);
    }
    return(NULL);
}

/*
 * nov_network_finish
 * Common code for "network" and commands.
 */

void nov_network_finish (
    register novpdbtype *pdb,
    int address,
    boolean sense)
{
    novndbtype *ndb;
    novpdbtype *dist;
    int i, j;
    boolean first_time;

    /*
     * Lookup net.  We may get a NULL pointer if the network isn't yet
     * in the route table or has been deleted.  Callers should handle
     * NULL ndb pointers.
     */
    ndb = novrip_find_rt(address, NOV_NETMASK_ALL, IPX_ROUTE_MATCH_EXACT);
    first_time = FALSE;

    if (sense) {
        if (pdb->netcount >= MAXNETWORKS) {
            errmsg(&msgsym(TOOMANYNETS, IPX));
            return;
        }
	if (pdb->proctype == NET_RIP)
	    pdb->all_nets = FALSE;
        nov_set_ndb_flags(ndb, pdb, TRUE);
        for (i = 0; i < pdb->netcount; i++) {
            if (pdb->networks[i] == address)
                return;
        }
        pdb->networks[pdb->netcount++] = address;
	
        if (pdb->running == FALSE) {
	    if (pdb->proctype == NET_IGRP) {
            	pdb->pid = cfork ((forkproc *)pdb->process, (long)pdb, 1500,
				  pdb->pname, 0);
 		if (pdb->pid == NO_PROCESS) {
 		    pdb->pid = 0;
 		    return;
		}
	    	novell_igrp_running++;
	    }
	    if (pdb->proctype == NET_RIP) {
		novrip_pdbinit();
	        pdb->pid = cfork((forkproc *)novell_router, (long)&novrip_pdb, 
				0, "IPX RIP", 0);
 		if (pdb->pid == NO_PROCESS) {
 		    pdb->pid = 0;
 		    return;
 		}
 		novell_rip_running = novrip_pdb.running = TRUE;
	    }
            pdb->running = TRUE;
	    first_time = TRUE;
	    novrip_network_command(pdb, address, sense);
        }
    } else {
	if (pdb->proctype == NET_IGRP)
	    pdb->all_nets = FALSE;
	novrip_network_command(pdb, address, sense);
        nov_set_ndb_flags(ndb, pdb, FALSE);
        for (i = 0; i < pdb->netcount; i++) {
            if (pdb->networks[i] == address) {
                pdb->netcount--;
                for (j = i; j < pdb->netcount; j++)
                    pdb->networks[j] = pdb->networks[j+1];
                break;
            }
        }
    }
    if (pdb->proctype == NET_RIP)
	return;
    /*
     * Configure IGRP2.
     */
    reg_invoke_novigrp_network_command(pdb, address, sense);
    if (first_time) {
        for (dist = novpdbQ.qhead; dist; dist = dist->next) {
            if ((pdb == dist) || !(pdb->distflag & dist->mask))
		continue;
            reg_invoke_nov_redistribute(pdb, dist, sense);
	}
    }
}

/*
 * allocate_novpdbindex
 * Allocate a pdb index dynamically.  Sets it and returns it.  Returns -1
 * on error.
 */
boolean allocate_novpdbindex (novpdbtype *pdb)
{
    int i;

    for (i = 0; i < MAX_NOVPROCS; i++)
	if (nov_proctable[i] == pdb)
	    return(FALSE);

    for (i = 0; i < MAX_NOVPROCS; i++)
        if (!nov_proctable[i]) {
            nov_proctable[i] = pdb;
            pdb->index = i;
            pdb->mask = INDEX2MASK(i);
            return(TRUE);
        }
    printf("too many IPX routing processes");
    return(FALSE);
}

/*
 * nv_write_patricia
 */
int nv_write_patricia(rntype *rnnode, va_list ap)
{
    nov_sumroutetype *sumroute;
    novndbtype *ndb;

    sumroute = (nov_sumroutetype *)rnnode;
    ndb  = sumroute->ndb;
    if (sumroute) {
	if (ndb->path_list && (ndb->flags & NET_STATIC) &&
	    !(ndb->options & IPX_NDB_OPTION_STATIC_NO_NVGEN)) {
	    nv_write(TRUE, "%s %x %x %x.%e", "ipx route", ndb->dstnet,
		     ndb->rt_mask, ndb->path_list->srcnet,
		     ndb->path_list->srchost);
	}
	return NOV_WALK_CONTINUE;
    }
    return NOV_WALK_ABORT;
}

/*
 * ipx_validate_contiguous_mask
 *
 * A valid configuous network mask must not have any leading 0 bits or 0 bits
 * in between. 
 *
 * Some valid masks: ffff0000, ffff8000, fffc0000, ffffe000
 * Some invalid masks: 00000000, 0fff0000, ffff0fff, ffff2000, fff10000
 */
boolean ipx_validate_contiguous_mask(ulong netmask)
{
    int i;
    boolean found_zero_bit = FALSE;
    boolean curbit_on;

    for (i = 1; i <= 32; i++, netmask = netmask << 1) {
	if (netmask & 0x80000000)
	    curbit_on = TRUE;
	else {
	    found_zero_bit = TRUE;
	    curbit_on = FALSE;
	}
	if (found_zero_bit && curbit_on) {
	    return FALSE;
	}
    }
    return TRUE;
}

/*
 * patricia_paths_command
 */
int patricia_paths_command(rntype *rnnode, va_list ap)
{
    nov_sumroutetype *sumroute;
    int novell_new_maxpaths = va_arg(ap, int);
    novndbtype *ndb;
    novrdbtype *rdb;
    novpdbtype *pdb;

    sumroute = (nov_sumroutetype *)rnnode;
    ndb  = sumroute->ndb;
    if (sumroute) {
	if (ndb->flags & NET_IGRP) {
	    if (ndb->paths > novell_maxpaths) {
		while (ndb->paths > novell_new_maxpaths) {
		    ndb->paths--;
		    rdb = ndb->path_list;
		    ndb->path_list = rdb->next;
		    ndb->nextpath = ndb->path_list;
		    novell_mark_rdb_changed(ndb, rdb, ndb->flags,
					    IPX_ROUTE_EVENT_PUSHED_OUT);
		    novrdb_free(rdb);
		}
	    }
	    for (pdb = (novpdbtype *) novpdbQ.qhead; pdb; pdb = pdb->next) {
		if (pdb->add_route)
		    (*pdb->add_route)(pdb, ndb->dstnet, ndb->rt_mask, 0L, 0L);
	    }
	}
	return NOV_WALK_CONTINUE;
    }
    return NOV_WALK_ABORT;
}

/*
 * patricia_start_aging
 *
 * Start the aging of all summary routes learned on the given HWIDB
 */
int patricia_start_aging (rntype *rnnode, va_list ap)
{
    nov_sumroutetype *net;
    idbtype *idb = va_arg(ap, idbtype *);
    paktype *pak = va_arg(ap, paktype *);
    novndbtype *ndb;
    novrdbtype *rdb;
    ipxhdrtype *ipx = (ipxhdrtype *)((pak != NULL) ? ipxheadstart(pak) : NULL);
    ulong sourcenet = (ipx != NULL) ? GET2WD(ipx->ssnet) : 0L;
    
    net = (nov_sumroutetype *)rnnode;
    if (!net)
	return NOV_WALK_ABORT;

    ndb = net->ndb;

    if (TIMER_RUNNING(ndb->holddownuntil)) {
	return NOV_WALK_CONTINUE;
    }
    for (rdb = ndb->path_list; rdb != NULL; rdb = rdb->next) {
	if ((rdb->nidb != NULL) && (rdb->nidb->idb == idb) &&
	    (rdb->snapshot_no_age)) {
	    if ((ipx == NULL) ||
		((sourcenet == rdb->srcnet) &&
		 (ieee_equal(rdb->srchost, ipx->shost)))) {
		rdb->snapshot_no_age = FALSE;
		GET_TIMESTAMP(rdb->timer);
	    }
	}
    }
    return NOV_WALK_CONTINUE;
}


/*
 * nov_set_ndb_flags
 * Set or clear the pdb type flag for a network.
 */
void nov_set_ndb_flags (
    novndbtype *ndb,
    novpdbtype *pdb,
    boolean sense)
{
    if (ndb == NULL)
        return;
    if (sense == TRUE)
        ndb->procmask |= pdb->mask;
    else
        ndb->procmask &= ~pdb->mask;
}

/* nov_network_redist
 * Call the pdbs that might want to redistribute this route.  
 */
void nov_network_redist (novpdbtype *pdb, novndbtype *ndb, int event)
{
    novpdbtype *dist;

    if (novellredist_debug)
 	buginf("\nIPX-DIST: %s net %x %s",
 	       novdbg_proctype2string(pdb->proctype, FALSE),
 	       ndb->dstnet,
 	       novdbg_event2string(event));

    /*
     * NLSP protocol specific workaround for sap reachability problem.
     */
    if (event == ROUTE_DOWN) {
	nov_down_route(pdb, ndb);
    } else if (event == ROUTE_UP) {
	nov_up_route(pdb, ndb);
    } else if (event == ROUTE_MODIF) {
	nov_modif_route(pdb, ndb);
    }
    
    for (dist = novpdbQ.qhead; dist ; dist = dist->next) {
        if (pdb->mask == dist->mask)
            continue;
        if (!(dist->redist_callback))
            continue;
 	if (INDEX2MASK(ndb->novpdbindex) & dist->distflag)
 	    (*dist->redist_callback)(dist, ndb, event);
    }

    novell_update_sap_route_info(ndb);
}

/*
 * nov_invalidate_redist
 * Invalidate redistributed routes from the given routing process.
 *
 * Connected and Static routes have the property that their procmask
 * includes bits showing ALL processes that have included them with a
 * 'network' subcommand. (See the comments for procmask in iprouting/route.h).
 *
 * Therefore, we have to be very careful and not unredistribute connected
 * or static routes if we are not deleting the connected or static route
 * pdb. In these cases, we just want to remove the 'procmask' bit for
 * these routes. It is a kludge...
 */
static void nov_invalidate_redist (novpdbtype *pdb)
{
    int i;
    novndbtype *ndb;

    for (i = 0; i < NOV_NETHASHLEN; i++) {
        for (ndb = (*novell_table)[i]; ndb; ndb = ndb->next) {
            if (ndb->procmask & pdb->mask) {
		/*
		 * If this route is not owned by the process being
		 * deleted, just clear the mask bit in this route.
		 */
		if (pdb->index != ndb->novpdbindex) {
		    ndb->procmask &= ~(pdb->mask);
		} else {
		    nov_network_redist(pdb, ndb, ROUTE_DOWN);
		}
	    }
        }
    }
}

/*
 * nov_table_prune_pdb
 * Prune a process from the routing table.
 */
static void nov_table_prune_pdb (novpdbtype *pdb)
{
    novndbtype *p, *pred;
    int i;
    novrdbtype *pp, *nextpp;

    for (i = 0; i < NOV_NETHASHLEN; i++) {
        p = (*novell_table)[i];
        if (p == NULL)
            continue;
        pred = p;
        while (p) {
	    if (nov_proctable[p->novpdbindex] == pdb && 
 		!(p->flags & (NET_CONNECTED | NET_STATIC))) {
		nov_network_redist(pdb, p, ROUTE_DOWN);
		pp = p->path_list;
 		p->path_list = NULL;
		p->nextpath = NULL;
 		p->paths = 0;
		while (pp) {
 		    nextpp = pp->next;
 		    novell_mark_rdb_changed(p, pp, p->flags,
 					    IPX_ROUTE_EVENT_DELETED);
		    novrdb_free(pp);
		    pp = nextpp;
		}
 		if (novellrip_debug || novellrip_event_debug ||
 		    NOVIGRP_DTARGET(p->dstnet, ((ddbtype *)pdb->ddb)) ||
 		    NOVNLSP_DEBUG(p->dstnet, pdb)) {
 		    buginf("\nIPX: garbage collecting entry for %x",
 			   p->dstnet);
 		}
 		p->options &= ~IPX_NDB_OPTION_VALID;
 		if (p != pred)
		    pred->next = p->next;
 		else {
 		    (*novell_table)[i] = p->next;
 		    pred = NULL;
 		}
 		if (p->flags & NET_RIP)
		    novell_routes--;
 		nov_ndb_free(p, IPX_ROUTE_FREE_WAS_IN_TABLE, TRUE);
 		p = pred;
	    }
            /* Did we delete the front of the list, if so, start over... */
            if (pred == NULL) {
                pred = p = (*novell_table)[i];
            } else {
                pred = p;
                if (p != NULL) {
                    p = p->next;
                }
            }
        }
    }
}

/*
 * nov_router_erase
 * Destroy a routing process
 */

void nov_router_erase (novpdbtype *pdb)
{
    novpdbtype      *dist;
    nidbtype     *nidb;
    int          i;
    int          count;

    if ((pdb  == NULL) || !pdb->running) {
	return;
    }
    if (pdb->proctype == NET_IGRP) {
	pdb->shutdown = TRUE;
        count = pdb->netcount; 
	for (i = 0; i < count; i++) {
	    nov_network_finish(pdb, pdb->networks[i], FALSE);
	}
    }

    /*
     * Clear the redistribute bit in all other protocols, unless
     * ALL protocols are shutting down. In this case, leave them
     * intact in case IPX routing is reactivated. Exempt EIGRP,
     * which is an exception because it kills itself for good
     * when IPX routing is disabled.
     */
    nov_invalidate_redist(pdb);
    for (dist = novpdbQ.qhead; dist; dist = dist->next) {
 	if ((dist != pdb) &&
 	    (!novell_shutting_down || pdb->proctype == NET_IGRP ||
 	     pdb->proctype == NET_NLSP)) {
 	    dist->distflag &= ~pdb->mask;
 	}
    }
    
    /*
     * Shutdown process.  Flush routing table. Note that pdb's are
     * usually retained with pdb->running set to FALSE. This allows
     * them to be restarted if ipx routing is disabled then reenabled
     * or if renabled individually.
     */
    if (pdb->running) {
  	pdb->running = FALSE;
 	switch (pdb->proctype) {
 	  case NET_IGRP:
  	    novell_igrp_running--;
 	    break;
 	  case NET_NLSP:
 	    if (novell_nlsp_running > 0)
 		novell_nlsp_running--;
 	    /*
 	     * Possibly expand maxpaths since NLSP supports only 4:
 	     */
 	    novell_paths_command(FALSE, TRUE, novell_cfg_maxpaths);
 	    break;
 	  case NET_RIP:
  	    novell_rip_running = FALSE;
 	    break;
 	  case NET_STATIC:
 	  case NET_FLOATING_STATIC:
 	  case NET_CONNECTED:
 	    break;
 	  default:
 	    printf("\n%% Unknown IPX routing process");
 	    break;
 	}
 	/*
 	 * Kill the process off (except RIP if we are not shutting down
 	 * completely). This may seem odd but consider this. The RIP
 	 * process handles RIP requests which are required by end hosts
 	 * even if only NLSP/EIGRP are running. Meanwhile the RIP (and SAP)
 	 * processes have been modified to NOT generate periodic updates
 	 * and to drop incoming updates if novell_rip_running is FALSE.
 	 * OK, you say you really want RIP to go away because there are no
 	 * end nodes on any interface. That's fine but will require a
 	 * restructuring of existing process functionality. Do it here
 	 * and we break NLSP when end nodes are present.
 	 */
	if (pdb->pid) {
 	    if (novell_shutting_down || (pdb->proctype != NET_RIP)) {
 		process_kill(pdb->pid);
 		pdb->pid = 0;
 	    }
  	}
        nov_table_prune_pdb(pdb);
    }

    /*
     * Delete all elements off of the work Q.
     */
    while (!QUEUEEMPTY(&pdb->workQ)) {
	free(dequeue(&pdb->workQ));
    }

    /*
     * Retbuffer all packets on the input queue.
     */
    while (!QUEUEEMPTY(&pdb->inputq)) {
	retbuffer(p_dequeue(&pdb->inputq));
    }

    if (pdb->cleanup)
        (*pdb->cleanup)(pdb);

    FOR_ALL_NIDBS(nidb) {
      if (nov_ipxwan_on_nidb(nidb)) {
	/* Force world to know it's gone and bring IPXWAN down... */
	novell_nidb_newstate(nidb, NOVELL_NIDB_DOWN);
	novell_nidb_newstate(nidb, NOVELL_NIDB_UP);
      }
    }
}

/* nov_service_redist
 * Call the pdbs that might want to redistribute this service.
 */
void nov_service_redist (
     novpdbtype *pdb,
     novell_sapentrytype *sap,
     int event)
{
    novpdbtype *dist;

    if (novellredist_debug)
	buginf("\nIPX-DIST: %s service %s, type %x, net %x @%d hops is %s",
	       novdbg_proctype2string(pdb->proctype, TRUE),
	       sap->server.server_name,
	       sap->server.server_type,
	       sap->server.dnet,
	       sap->server.nethops,
	       novdbg_event2string(event));

    for (dist = novpdbQ.qhead; dist ; dist = dist->next) {
        if (pdb->mask == dist->mask)
            continue;
        if (!(dist->sap_redist_callback))
            continue;
        if (INDEX2MASK(sap->novpdbindex) & dist->sap_distflag)
            (*dist->sap_redist_callback)(dist, sap, event);
    }
}

/*
 * novell_redist_command
 * Set flag to cause wholesale redistribution of routes
 *
 * This is an "ipx router [rip | eigrp <as-number> | nlsp <tag-id>]" 
 * subcommand. The format is:
 * 
 * [no] redistribute [rip | eigrp <as-number> | static | connected | 
 *                    nlsp <tag-id> [access-list <800-899> | <1200-1299>] ]
 */
void novell_redist_command (parseinfo *csb)
{
    novpdbtype  *pdb;
    novpdbtype  *dist;
    boolean non_default;
    int l, number;
    novndbtype *ndb;
    nov_sumlisttype *item;
    char buffer[100]; 
    char *name;

    /*
     * csb->protocol is set to the pdb of the "ipx router" protocol 
     * in novell_router_command()
     */
    pdb = csb->protocol;

    if (pdb->proctype == NET_NLSP)
	name = GETOBJ(string,1);
    else
	name = csb->router_name.tag;
    /*
     * csb->router_name.protocol contains the "redistribute" protocol
     */
    switch (csb->router_name.protocol) {
      case PROC_IGRP2:
        dist = find_novigrp_pdb(csb->router_name.as);
	break;
      case PROC_RIP:
	dist = &novrip_pdb;
	break;
      case PROC_NLSP:
	dist = novnlsp_find_pdb(name);
	break;
      case PROC_CONNECTED:
	dist = &novconnected_pdb;
	break;
      case PROC_STATIC:
	dist = &novstatic_pdb;
	break;
      case PROC_FSTATIC:
	dist = &novfstatic_pdb;
	break;
      default:
	dist = NULL;
	break;
    }

    if (csb->nvgen) {
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {
	    if ((dist == pdb) || !dist->running)
		continue;

	    non_default = FALSE;
    	    switch (dist->proctype) {
	    case NET_IGRP:
		l = sprintf(buffer, "%s %d", dist->name, dist->asystem);
		break;
	    case NET_NLSP:
		l = sprintf(buffer, "%s", dist->name);
		if (dist->pname) 
		    l += sprintf(&buffer[l], " %s", dist->pname);
		break;
	    default:
		l = sprintf(buffer, "%s", dist->name);
	    }
	    if (pdb->sumlist[dist->index]) {
		sprintf(&buffer[l], " access-list %d",
			pdb->sumlist[dist->index]);
		non_default = TRUE;
	    }
            /*
             * Not redistributing this "dist" into this "pdb". This is
             * default behavior for EIGRP->EIGRP, and for floating->anything,
             * and EIGRP<->NLSP so don't print anything. 
             * It is non-default behavior for RIP->EIGRP and  EIGRP->RIP
             * also non-default for static->anything and conn->anything
             * so print the "no" form of the "redistribute" command.
             */
            if (!(pdb->distflag & dist->mask)) {
		if ((dist->proctype & (NET_CONNECTED|NET_STATIC)) &&
		    (pdb->proctype & (NET_RIP|NET_IGRP|NET_NLSP))) {
		    non_default = TRUE;
		} else if ((dist->proctype & NET_RIP) &&
			   (pdb->proctype & (NET_IGRP|NET_NLSP))) {
		    non_default = TRUE;
		} else if ((dist->proctype & (NET_IGRP|NET_NLSP)) &&
			   (pdb->proctype & NET_RIP)) {
		    non_default = TRUE;
		} else if (dist->proctype & NET_NLSP && 
			   pdb->proctype & NET_NLSP) {
		    non_default = TRUE;
		}
		if (non_default) {
		    nv_write(TRUE, "no %s %s", csb->nv_command, buffer);
		}
            } else {
            /*
             * Redistributing this "dist" into this "pdb". This is
             * the default case for the RIP process, static and connected
             * so don't print anything out. For EIGRP, the default
             * is to redistribute RIP but not other EIGRP processes so
             * only print the redist command if dist == NET_IGRP. This
             * is also non-default behavior for floating->anything. 
	     * Redistribute NLSPx into NLSPy is the default case unless there
	     * is an access-list attached to it.
             */
		if ((dist->proctype & NET_FLOATING_STATIC) &&
		    (pdb->proctype & (NET_RIP|NET_IGRP|NET_NLSP))) {
		    non_default = TRUE;
		} else if ((dist->proctype & NET_IGRP &&
			   pdb->proctype & NET_NLSP) || 
			   (dist->proctype & (NET_NLSP | NET_IGRP) &&
			    pdb->proctype & NET_IGRP)) {
		    non_default = TRUE;
		} else if (dist->proctype & NET_NLSP && 
			   pdb->proctype & NET_NLSP && 
			   pdb->sumlist[dist->index]) {
		    non_default = TRUE;
		}
		if (non_default) {
		    nv_write(TRUE, "%s %s", csb->nv_command, buffer);
		}
            }
	}
    } else {
	if (csb->sense) {
	    if (dist == pdb) {
		if (pdb->proctype == NET_IGRP)
		    sprintf(buffer, "%s %d", pdb->name, pdb->asystem);
		else {
		    sprintf(buffer, "%s", pdb->name);
		    if (pdb->proctype == NET_NLSP && pdb->pname)
			sprintf(buffer, " %s", dist->pname);
		}
		printf("\n%% Redistribution of \"%s\" via \"%s\" not allowed", 
			buffer, buffer);
		return;
	    }
    	    /*
     	     * If dist is NULL, the "ipx router" command for the IGRP process
     	     * that is being redistributed has not been entered yet. We need to
     	     * create that process now. Note that dist will only be NULL if
     	     * we are trying to redistribute an IGRP process.
     	     */
    	    if (!dist)
		switch (pdb->proctype) {
		case NET_NLSP:
		    dist = reg_invoke_nlsp_pdb_init(name, NULL);
		    break;
		default:
		    dist = reg_invoke_novigrp_start(csb->router_name.as);
		    break;
		}
	    if (!dist) {
		printf("\ndist pdb is null, ignore redist command");
		return;
	    }
	    /*
	     * Turn on redistribution of a protocol. pdb->distflag
	     * must be set prior to call to redistribute routine.
	     */
	    pdb->distflag |= dist->mask;
	    if (pdb->proctype == NET_NLSP) {
		/*
		 * Turn on redistribution of a protocol. 
		 */
		number = pdb->sumlist[dist->index];
		if (number) {
		    /* Remove all the drop routes. */
		    for (item = ipx_get_sum_acl(number);item;item = item->next)
			if (!item->grant && 
			    (ndb = novrip_find_rt(item->net, item->mask,
						  IPX_ROUTE_MATCH_EXACT))) {
			    if (ndb->path_list && 
				ndb->path_list->nidb == nullnidb
				&& ndb->novpdbindex == pdb->index) {
				novrip_del_rt(item->net, item->mask, 
					      IPX_ROUTE_DELETE_RETAIN_CONNECTED);
			    }
			}
		}    
		pdb->sumlist[dist->index] = GETOBJ(int,1);
		if (pdb->sumlist[dist->index] && !pdb->osi_pdb->enable_summary)
		    printf("\n%%redistribute command will not be activated until route-aggregation command is entered");
	    }
	    if (pdb->redistribute)
	        (*(pdb->redistribute))(pdb, dist, TRUE);
	} else {
            if (dist == pdb) {
                printf("\n%% Can't turn off redistribution of protocol into itself");
                return;
            }
	    /*
	     * Turn off redistribution of a protocol. pdb->distflag 
	     * will get set in novigrp_process_redistribute() for EIGRP.
	     */
	    if (!dist) {
    		if (csb->router_name.protocol == PROC_IGRP2)
		    printf("\nEIGRP Autonomous System %d unknown",
			   csb->router_name.as);
		else if (csb->router_name.protocol == PROC_NLSP)
		    printf("\nNo such NLSP routing area");
		else
		    printf("\ndist pdb is null, cannot disable redist");
		return;
	    }
	    number = pdb->sumlist[dist->index];
	    if (number) {
		/* Remove all the drop routes. */
		for (item = ipx_get_sum_acl(number); item; item = item->next)
		    if (!item->grant && 
			(ndb = novrip_find_rt(item->net, item->mask,
					      IPX_ROUTE_MATCH_EXACT))) {
			if (ndb->path_list && ndb->path_list->nidb == nullnidb
			    && ndb->novpdbindex == pdb->index) {
			    novrip_del_rt(item->net, item->mask, 
					  IPX_ROUTE_DELETE_RETAIN_CONNECTED);
			}
		    }
	    }    
	    pdb->sumlist[dist->index] = 0;	
	    if (pdb->redistribute)
	        (*(pdb->redistribute))(pdb, dist, FALSE);
	    else {
		pdb->distflag &= ~dist->mask;
	    }
	}
    }
}

/* novell_distributesaplist_command
 *
 * Set up access list to govern the contents of service updates.
 * 
 * This is a "ipx router nlsp [<tag>]" subcommand.
 * Format is: "distribute-sap-list <list> in|out [<router>|<interface>]"
 *
 */

void novell_distributesaplist_command (parseinfo *csb)
{
    idbtype *idb;
    novpdbtype *pdb, *dist;
    ddbtype *ddb;
    int     number, direction;
    int     list, i;

    pdb = (novpdbtype *) csb->protocol;

    if (csb->nvgen) {
        for (i = 0; i < MAX_NOVPROCS; i++) {
            if (!pdb->sap_ofltr_proto[i])
                continue;
            dist = nov_proctable[i];
            nv_write((boolean)dist, "distribute-sap-list %d out %s",
                        pdb->sap_ofltr_proto[i], dist->name);
            nv_add(dist && dist->asystem, " %d", dist->asystem);
        }
        FOR_ALL_SWIDBS(idb) {
	    if (IPX_PRIMARY_NIDB(idb)) {
                list = IPX_PRIMARY_NIDB(idb)->sap_oprotofltr[pdb->index];
                nv_write(list,"distribute-sap-list %d out %s", 
			list, idb->namestring);
                list = IPX_PRIMARY_NIDB(idb)->sap_iprotofltr[pdb->index];
                nv_write(list, "distribute-sap-list %d in %s", 
			list, idb->namestring);
	    }
        }
        nv_write(pdb->sap_ofltr_all,"distribute-sap-list %d out", 
		 pdb->sap_ofltr_all);
        nv_write(pdb->sap_ifltr_all,"distribute-sap-list %d in", 
		 pdb->sap_ifltr_all);
        return;
    }

    ddb = pdb->ddb;
    number = csb->sense ? GETOBJ(int,1) : 0;
    direction = GETOBJ(int,2);
    idb = GETOBJ(idb,1);
    if (!idb)
	switch (csb->router_name.protocol) {
	  case PROC_IGRP2:
             dist = find_novigrp_pdb(csb->router_name.as);
	     break;
	  case PROC_RIP:
             dist = &novrip_pdb;
             break;
          case PROC_NLSP:
	     dist = novnlsp_find_pdb(csb->router_name.tag);
	     break;
          default:
             dist = NULL;
             break;
	}
    else
        dist = NULL;
    switch (direction) {

      case IN:
        if (idb) {
	    if (IPX_PRIMARY_NIDB(idb))
	        IPX_PRIMARY_NIDB(idb)->sap_iprotofltr[pdb->index] = number;
	    else
		printf("\n%% Must first configure an IPX network number on Interface %s ", idb->namestring);
        } else {
            pdb->sap_ifltr_all = number;
        }
        break;

      case OUT:
        if (dist) {
            pdb->sap_ofltr_proto[dist->index] = number;
	    /*
	    nov_redistribute(pdb, dist, TRUE);
	    */
	} else if (idb) {
	    if (IPX_PRIMARY_NIDB(idb))
                IPX_PRIMARY_NIDB(idb)->sap_oprotofltr[pdb->index] = number;
	    else
                printf("\n%% Must first configure an IPX network number on Interface %s", idb->namestring);
        } else
            pdb->sap_ofltr_all = number;
        break;
    }

    if (direction == IN && pdb->proctype == NET_NLSP) {
	reg_invoke_ipx_mark_dr_info_change(pdb, NULL);
    } else {
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {   
	    if (dist->proctype == NET_NLSP && dist != pdb) {
		reg_invoke_ipx_mark_dr_info_change(dist, NULL);		
	    }
	}
    }

    if (ddb) {
        if (idb) {
	    reg_invoke_ipx_igrp2_take_nbrs_down(ddb, idb, FALSE, 
						"SAP filter changed");
	} else {
	    FOR_ALL_SWIDBS(idb) {
	        reg_invoke_ipx_igrp2_take_nbrs_down(ddb, idb, FALSE, 
						    "SAP filter changed");
	    }
	}
    }
}

/* novell_distributelist_command
 *
 * Set up access list to govern the contents of routing updates.
 * 
 * This is a "ipx router" subcommand.
 * Format is: "distribute-list <list> in|out [<router>|<interface>]"
 *
 */

void novell_distributelist_command (parseinfo *csb)
{
    idbtype *idb;
    novpdbtype *pdb, *dist;
    ddbtype *ddb;
    int     number, direction;
    int     list, i;

    pdb = csb->protocol;

    if (csb->nvgen) {
        for (i = 0; i < MAX_NOVPROCS; i++) {
            if (!pdb->ofltr_proto[i])
                continue;
            dist = nov_proctable[i];
            nv_write((boolean)dist, "distribute-list %d out %s",
                        pdb->ofltr_proto[i], dist->name);
            nv_add(dist && dist->asystem, " %d", dist->asystem);
        }
        FOR_ALL_SWIDBS(idb) {
	    if (IPX_PRIMARY_NIDB(idb)) {
                list = IPX_PRIMARY_NIDB(idb)->oprotofltr[pdb->index];
                nv_write(list,"distribute-list %d out %s", 
			list, idb->namestring);
                list = IPX_PRIMARY_NIDB(idb)->iprotofltr[pdb->index];
                nv_write(list, "distribute-list %d in %s", 
			list, idb->namestring);
	    }
        }
        nv_write(pdb->ofltr_all,"distribute-list %d out", pdb->ofltr_all);
        nv_write(pdb->ifltr_all,"distribute-list %d in", pdb->ifltr_all);
        return;
    }

    ddb = pdb->ddb;
    number = csb->sense ? GETOBJ(int,1) : 0;
    direction = GETOBJ(int,2);
    idb = GETOBJ(idb,1);
    if (!idb)
	switch (csb->router_name.protocol) {
	  case PROC_IGRP2:
             dist = find_novigrp_pdb(csb->router_name.as);
	     break;
	  case PROC_RIP:
             dist = &novrip_pdb;
             break;
          case PROC_NLSP:
	     dist = novnlsp_find_pdb(csb->router_name.tag);
	     break;
          default:
             dist = NULL;
             break;
	}
    else
        dist = NULL;
    switch (direction) {

      case IN:
        if (idb) {
	    if (IPX_PRIMARY_NIDB(idb))
	        IPX_PRIMARY_NIDB(idb)->iprotofltr[pdb->index] = number;
	    else
		printf("\n%% Must first configure an IPX network number on Interface %s ", idb->namestring);
        } else {
            pdb->ifltr_all = number;
        }
        break;

      case OUT:
        if (dist) {
            pdb->ofltr_proto[dist->index] = number;
	    if (pdb->redistribute)
	        (*pdb->redistribute)(pdb, dist, TRUE);
	} else if (idb) {
	    if (IPX_PRIMARY_NIDB(idb))
                IPX_PRIMARY_NIDB(idb)->oprotofltr[pdb->index] = number;
	    else
                printf("\n%% Must first configure an IPX network number on Interface %s", idb->namestring);
        } else
            pdb->ofltr_all = number;
        break;
    }

    if (direction == IN && pdb->proctype == NET_NLSP) {
	reg_invoke_ipx_mark_dr_info_change(pdb, NULL);
    } else {
	for (dist = novpdbQ.qhead; dist; dist = dist->next) {   
	    if (dist->proctype == NET_NLSP && dist != pdb) {
		reg_invoke_ipx_mark_dr_info_change(dist, NULL);		
	    }
	}
    }

    if (ddb) {
        if (idb) {
	    reg_invoke_ipx_igrp2_take_nbrs_down(ddb, idb, FALSE, 
					    "route filter changed");
        } else {
	    FOR_ALL_SWIDBS(idb) {
	        reg_invoke_ipx_igrp2_take_nbrs_down(ddb, idb, FALSE, 
						"route filter changed");
	    }
	}
    }
}

/*
 * nov_router_init
 * A process created by the "network" router subcommand that hangs until
 * the interfaces have initialized.  It then starts a router and sets up
 * the networks for which the router process is responsible.
 */

forktype nov_router_init (void)
{
    nov_spawntype *args;

    edisms((blockproc *)systeminitBLOCK, 0L);
    while (!QUEUEEMPTY(&nov_router_initQ)) {
        args = dequeue(&nov_router_initQ);
        nov_network_finish(args->protocol, args->address, args->sense);
        free(args);
    }
    nov_router_init_running = FALSE;
    process_kill(THIS_PROCESS);
}

/*
 * novigrp_net_in_pdb
 *
 * Returns TRUE if the network number is covered by our PDB.
 */
boolean novigrp_net_in_pdb (novpdbtype *pdb, ulong net)
{
    int net_ix;
    for (net_ix = 0; net_ix < pdb->netcount; net_ix++) {
	if (pdb->networks[net_ix] == net) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

/*
 * nov_network_command
 * Set up a particular routing protocol for a network.
 * Syntax: "[no] network [<net> | all]"
 */

void nov_network_command (parseinfo *csb)
{
    nov_spawntype *args;
    novpdbtype *pdb;
    nidbtype *nidb;
    ulong net;
    int i;

    if (!novell_running || novell_shutting_down) {
	return;
    }
    pdb = csb->protocol;
    if (pdb == NULL) {
	if (!csb->nvgen) {
	    printf("\n%% No routing protocol specified");
	}
	return;
    }

    if (csb->nvgen) {
	if (pdb->all_nets) {
	    if (pdb->proctype == NET_IGRP) {
		nv_write(TRUE, "%s all", csb->nv_command);
	        return;
	    } 
	    if (pdb->proctype == NET_RIP) {
		nv_write(TRUE, "no %s all", csb->nv_command);
		return;
	    }
	}
	if (pdb->default_include_net == FALSE) {
	    for (i = 0; i < pdb->netcount; i++) {
		FOR_ALL_NIDBS(nidb) {
		    /* IPXWAN interfaces never get 'network' commands */
		    if (nov_ipxwan_on_nidb(nidb)) {
			continue;
		    }
		    if (pdb->networks[i] == nidb->novellnet)
		      break;
		}
		nv_write(((nidb != NULL) &&
			  (NOVELL_NIDB_CONFIGURABLE(nidb))),
			 "%s %x", csb->nv_command, pdb->networks[i]);
	    }
	} else {
	    FOR_ALL_NIDBS(nidb) {
		/* IPXWAN interfaces never get 'network' commands */
		if (nov_ipxwan_on_nidb(nidb)) {
		    continue;
		}
		for (i = 0; i < pdb->netcount; i++) {
		    if (pdb->networks[i] == nidb->novellnet)
		      break;
		}
		if ((pdb->proctype != NET_IGRP) && (i == pdb->netcount)) {
		    nv_write(NOVELL_NIDB_CONFIGURABLE(nidb),
			     "no %s %x", csb->nv_command, nidb->novellnet);
		}
	    }
	}
	
        return;
    }

    /*
     * "all" keyword was entered
     */
    if (GETOBJ(int,2)) {
	pdb->all_nets = TRUE;
	FOR_ALL_NIDBS(nidb) {
	    /* Network is being deleted? If so, don't bother */
	    if (!NOVELL_NIDB_CONFIGURABLE(nidb))
	        continue;
	    if ((nidb != NULL) &&
		IPX_NIDB_IS_SECONDARY(nidb) &&
                (pdb->proctype == NET_IGRP) &&
		(csb->set_to_default || csb->sense)) {
		    continue;
            }

	    if (nidb->novellnet) { 
		/*
		 * Make sure the network we are removing is one that
		 * belongs to this pdb.
		 */
		if (csb->sense == FALSE) {
		    if (!novigrp_net_in_pdb(pdb, nidb->novellnet))
			return;
		}

		args = malloc(sizeof(nov_spawntype));
		if (args) {
          	    args->protocol = pdb;
        	    args->address = nidb->novellnet; 
        	    args->sense = csb->sense;
        	    enqueue(&nov_router_initQ, args);
        	    if (!nov_router_init_running) {
                        nov_router_init_running = TRUE;
            	        (void) cfork(nov_router_init, 0, 0, "IPX Router Init", 0);
        	    }
    	        }
	    }
	}
        return;
    }
    /* Verify network can be configured at this time (not being deleted) */
    FOR_ALL_NIDBS(nidb) {
	if (nidb->novellnet == GETOBJ(int,1)) {
	    if (!NOVELL_NIDB_CONFIGURABLE(nidb)) {
		printf("\n%% Network %x is being deleted, and cannot be configured at this time",
		       nidb->novellnet);
		return;
	    }
	    if ((nidb != NULL) && 
		IPX_NIDB_IS_SECONDARY(nidb) &&
		pdb->proctype == NET_IGRP && csb->sense) {
		    printf("\n%% Can't configure IPX-EIGRP on secondary interfaces");
		    return;
	    }
	}
    }
    /*
     * Make sure the network we are removing is one that belongs to this pdb.
     */
    if (csb->sense == FALSE) {
	net = GETOBJ(int, 1);
	if (!novigrp_net_in_pdb(pdb, net))
	    return;
    }
    args = malloc(sizeof(nov_spawntype));
    if (args) {
        args->protocol = pdb;
	args->address = GETOBJ(int, 1);
        args->sense = csb->sense;
        enqueue(&nov_router_initQ, args);
        if (!nov_router_init_running) {
            nov_router_init_running = TRUE;
            (void) cfork(nov_router_init, 0, 0, "IPX Router Init", 0);
        }
    }
}


