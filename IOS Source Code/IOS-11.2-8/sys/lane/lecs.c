/* $Id: lecs.c,v 3.7.8.17 1996/09/11 17:27:39 cakyol Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecs.c,v $
 *------------------------------------------------------------------
 * The lane lecs implementation
 *
 * November 8, 1994
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lecs.c,v $
 * Revision 3.7.8.17  1996/09/11  17:27:39  cakyol
 * CSCdi68348:  The LECS needs to LOCALLY maintain the calling party
 * address of conn
 * Branch: California_branch
 *
 * Revision 3.7.8.16  1996/08/28  12:55:18  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.7.8.15  1996/08/26  19:46:01  cakyol
 * CSCdi66421:  the well known LECS address needs to follow the master LECS
 * Branch: California_branch
 *
 * Revision 3.7.8.14  1996/07/25  00:16:48  rbadri
 * CSCdi63919:  LANE Client can invalidate fast cache entries too often
 * Branch: California_branch
 * New flag to facilitate better cache invalidation when VCs go down.
 *
 * Revision 3.7.8.13  1996/07/02  22:20:02  michellw
 * CSCdi61813:  LEC was not invoked when the timer expired
 * Branch: California_branch
 * Changed LEC, LES/BUS and LECS process priority to NORMAL.
 *
 * Revision 3.7.8.12  1996/06/29  06:33:56  cakyol
 * CSCdi61448:  Router crashes after changing master LECS addr in ls1010
 * table
 * Change the printf strategy in "lecs_interface_show".  The problem
 * arises when performing a "show lane config" at the same instant
 * that a connection obkject is being deleted.
 * Branch: California_branch
 *
 * Revision 3.7.8.11  1996/06/21  20:55:06  cakyol
 * CSCdi60981:  more than one LECS address for the same LECS breaks
 * redundancy
 * Branch: California_branch
 *
 * Revision 3.7.8.10  1996/06/10  07:49:07  cakyol
 * CSCdi59043:  %LANE-3-LECS_ERROR: processSetupIndication: NULL pinfo ...
 *              Much more robust handling.  Clear up BOTH the circuits:
 *              the new incoming one and the old "stuck" one.
 * Branch: California_branch
 *
 * Revision 3.7.8.9  1996/06/08  20:45:44  cakyol
 * CSCdi59043:  %LANE-3-LECS_ERROR: processSetupIndication: NULL pinfo ...
 * Branch: California_branch
 *
 * Revision 3.7.8.8  1996/06/06  04:31:25  cakyol
 * CSCdi59510:  format string in inline wastes space.
 *              offending macro obliterated.
 * Branch: California_branch
 *
 * Revision 3.7.8.7  1996/06/01  00:34:35  cakyol
 * CSCdi59229:  handle setups in the buffer just as the LECS goes down.
 *              these are normal under load.  they are just
 *              unprocessed SETUP's while the LECS is going down.
 * Branch: California_branch
 *
 * Revision 3.7.8.6  1996/05/25  09:48:16  cakyol
 * CSCdi58685:  router crashed when LS100 system call errors occured.
 *              forgot to lock/unlock connection objects while
 *              printing them out.
 * Branch: California_branch
 *
 * Revision 3.7.8.5  1996/05/21  09:54:23  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.7.8.4  1996/05/13  23:53:30  cakyol
 * CSCdi57551:  LECS does not gracefully handle non existent lane_dest_tag
 * fixed.
 * Branch: California_branch
 *
 * Revision 3.7.8.3  1996/05/09  14:33:12  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.7.8.2.2.3  1996/05/03  15:20:33  kjadams
 * CSCdi56180:  LE_CONFIGURE_RESPONSE contains an invalid OUI in the TLV
 * encoding.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 * Fix invalid oui for TLV encoding.  Pointer deref had been lost along
 * the way.  TR LANE local segment id OUI.
 *
 * Revision 3.7.8.2.2.2  1996/05/02  22:36:20  cakyol
 * - eliminate the bad packet problem which arises when the release
 *   notification races in front of the packet hence giving the
 *   impression that the packet arrived on a "dead" circuit.
 * - correct erroneous debug message.
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.8.2.2.1  1996/04/27  07:07:00  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.7.8.2  1996/04/25  23:15:01  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.8.1.8.14  1996/04/25  22:12:41  cakyol
 * - rename lecsFinderModuleInit to lecs_finder_init, similar
 *   to all the other LANE module init routines.
 * - call it from the appropriate place (lane.c: lane_init).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.13  1996/04/24  22:23:21  cakyol
 * Do not crash the router for every little thing.
 * Put out error messages and continue as normal.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.12  1996/04/19  00:42:41  cakyol
 * send the correct error back in the config response.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.11  1996/04/18  22:01:50  cakyol
 * add some performance measuring debugging.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.10  1996/04/17  22:39:19  cakyol
 * Also complain ONCE about the fact that an LECS is not
 * in the switch list when an LECS starts listening
 * on a new address.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.9  1996/04/17  22:16:54  cakyol
 * report the fact that an LECS is not in the switch list ONCE
 * only, but every time the list of LECS addresses on the switch
 * changes.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.8  1996/04/16  02:45:04  cakyol
 * goodbye to PVC support for LANE.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.7  1996/04/13  01:13:24  cakyol
 * - tidy up "byteStreamCompare".
 * - better timer handling for the ticker in lecs.
 * - some more adherence to coding conventions.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.6  1996/04/11  22:00:28  cakyol
 * looks awful now, but the boss is happy.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.5  1996/04/11  18:20:06  cakyol
 * amendments to the latest sync:
 * - take some unnecessary crashdump's out of lane server.
 * - correct possible input buffer loss at LECS input processing
 *   for token ring lan type.
 * - debugging messages needed tidying up.
 * - some but not all white space cleanup.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.4  1996/04/11  05:34:27  ddecapit
 * Branch: LE_Cal_V112_0_2_branch
 *
 * - Correct botched lecs.c sync
 * - Modify indentation to appease the pedantic
 *
 * Revision 3.7.8.1.8.3  1996/04/09  20:03:32  cakyol
 * forgot to put out the interface name on 2 debug messages.
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.2  1996/04/08  14:52:23  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.7.8.1.8.1  1996/04/08  01:57:24  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.7.8.1  1996/03/18  20:33:00  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.7.12.5  1996/04/05  03:20:38  cakyol
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
 * Revision 3.7.12.4  1996/04/02  10:09:59  cakyol
 * - perform re-verifications ONLY on completely operational clients.
 * - tidy up initializations, they were scattered all over the place.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.12.3  1996/03/29  21:24:07  cakyol
 * Clean up the LES address/Elan deletion logic in a config database.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.12.2  1996/03/27  22:58:04  cakyol
 * Added some more 11.2 backward compatibility code that I missed.
 * This involves the "name NAME server-atm-address ADDRESS" lane
 * database command.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.7.12.1  1996/03/22  22:55:25  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.2.5  1996/03/20  23:47:59  cakyol
 * The 11.2 LECS parser can now FULLY recognize pre 11.2 LECS
 * commands .  As a result of this, we also have a new systemwide
 * global uint variable named "global_nvconfig_version", which
 * reflects the nvram version number in it.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.2.4  1996/03/12  23:23:03  cakyol
 * Use more consistent function names for ALL the ilmi and
 * signalling callback routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.2.3  1996/03/06  21:55:49  cakyol
 * better debugging support for ilmi callback routines.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.2.2  1996/02/29  00:04:20  cakyol
 * handle some error conditions less drastically:
 * - incoming packet with an invalid vcd.
 * - incoming packet on a vc which has just been
 *   marked to be disconnected.
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.2.1  1996/02/27  21:10:49  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.4.10.25  1996/02/24  01:36:00  cakyol
 * - much better ordering of debug messages.
 * - use the new "atmSig_printOpcode" api function.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.24  1996/02/22  21:08:21  cakyol
 * new scheduling paradigm expects a process to watch only
 * on a single managed timer.  So, convert multiple timers
 * to work under one master timer.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.23  1996/02/22  09:05:32  cakyol
 * the new LECS config command syntax enabled provisionally
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.22  1996/02/15  00:47:51  cakyol
 * correct misleading debugging messages giving the impression
 * that they are clearing a vc, whereas what they are in fact
 * doing is acknowledging a release received from the network.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.21  1996/02/09  23:00:32  cakyol
 * call LECS_CRASH only if lecs_swif_erase is called on the
 * major interface.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.20  1996/02/08  08:29:14  cakyol
 * lecs.c:
 *     - Call "chunk_create" when needed and not at init time.
 *       This was bugid CSCdi48400 in previous releases.
 *     - add logger_flush before calling crashdump.
 * lsv_client.c:
 *     - correct misleading comment.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.19  1996/02/08  00:04:02  cakyol
 * save a lot of string space by assigning __FILE__ to a
 * static char* at the beginning of the file and use that
 * throughout instead.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.18  1996/02/07  19:11:30  cakyol
 * there is an extremely brief period when an active LES is
 * relegated to being a backup and a config request is received
 * from it just at that instant.  Do not treat this case a
 * fatal bug but a possible scenario and reject the request.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.17  1996/02/06  22:19:31  cakyol
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
 * Revision 3.4.10.16  1996/02/02  22:59:06  cakyol
 * - more robust locking while global LECS addresses are
 *   being updated
 * - better debugging messages
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.15  1996/02/02  02:16:23  cakyol
 * - much more robust procesing of calling party address on
 *   the LECS now.  If a call from an already existing caller
 *   is received, the old one is maintained, the new one is
 *   rejected.  Before, the new one bumped the old one off.
 * - More efficient lecs finder:
 *         - if noone wants to know the master LECS address,
 *           this step is completely eliminated saving cycles.
 *         - as soon as the global LECS addresses are determined,
 *           the users awaiting only for those are responded to
 *           immediately.  Before, they had to wait until the
 *           master LECS was determined as well.  This could take
 *           a long time if signalling was slow.
 *         - safety timer to protect against ILMI hangups is
 *           added for debugging only.
 *         - the "#ifdef TESTING" section is now taken out.
 * - long printf string cleanup done, concatenation used now.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.14  1996/01/30  23:56:28  cakyol
 * - last touches on the 3 additional atm sig api
 *   routines as recommended by Schrupp.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.13  1996/01/29  22:06:11  cakyol
 * - as per the signalling group's request, eliminate the
 *   direct invasion of their private data structures by
 *   using:
 *     atmSig_api_callId2CallingPartyAddress   and
 *     atmSig_api_sameCallingPartyAddress
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.12  1996/01/26  20:36:58  cakyol
 * - guard against an LES connecting from two or more different
 *   interfaces to the LECSs running on those interfaces and sharing
 *   the same config table.  This MAY happen when each interface
 *   is connected to an independent but mirror imaged ATM network.
 *   This needs to be further re-hashed in the future.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.11  1996/01/26  02:02:41  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.10  1996/01/23  23:54:37  cakyol
 * HSRP over LANE Code review changes:
 * - "interface_up" now works properly with HSRP after
 *   Badri's network.c fix.
 * - additional HSRP mac addresses implemented much
 *   faster on the LANE client.
 * - starting an already running or stopping an already
 *   stopped LANE client now behaves properly.
 * - white space cleanup
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.9  1996/01/20  02:13:07  cakyol
 * - debug message in "sendRelease" was erroneous.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.8  1996/01/19  19:37:00  cakyol
 * - lock a config table to make it accessible by only a
 *   single session at a time (this was bug CSCdi47155
 *   in 11.0 and 11.1)
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.7  1996/01/18  22:45:59  cakyol
 * - active LES may very briefly be NULL while an active and
 *   a backup LES are switched over.
 * - add comments to remind me to add a safety timer to the
 *   LECS finder, in case ILMI or signalling screws up.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.6  1996/01/17  20:26:28  cakyol
 * - Index_add now returns the inserted element or the one already
 *   present.  More convenient for handling duplicates.
 * - Tidy up the parser help strings that involve atm address
 *   template.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.5  1996/01/16  21:58:11  cakyol
 * ensure that if a NEW LES address is added to an elan in a
 * config table, the LECS clears any previous connections from
 * the same calling party which does NOT seem to be an LES, on
 * interfaces to which that specific config table is bound to.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.4  1996/01/12  22:23:29  cakyol
 * warn if a "global" LECS address happens also to be
 * an LES or LEC address.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.3  1996/01/12  01:16:11  cakyol
 * - simplify the global LECS config error message;
 *     take out the relative rank numbers, since they
 *     are not always correct.
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.2  1996/01/11  22:03:28  cakyol
 * - warn against duplicate LECS addresses from the switch
 * - update from the switch once a minute
 * - correct more out of date comments
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.10.1  1996/01/11  01:46:47  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.4.8.2  1996/01/03  22:52:47  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.4.8.1  1995/12/29  01:07:03  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.4  1995/11/24  19:39:48  rbadri
 * CSCdi36121:  Line protocol is up, while LANE client is non-operational
 * subif_line_state variable will track the swidb's line state.
 * Use subif_line_statechange() to control the operational state of
 * the sub-interface. In LECS/LES use interface_admin_down instead of
 * interface up to determine the adminstrative nature of the sub-interface.
 *
 * Revision 3.3  1995/11/17  17:51:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:51:02  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:23:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.21  1995/10/18  21:44:38  cakyol
 * CSCdi42401:  LECS does not properly send the BLLI in the
 *              connect message
 *
 * Fix: I now supress the connect blli.  This is allowed as
 *      per the LANE spec.  This was full of crap before becoz
 *      the length was not set to 0 and it was picking up
 *      trash from the stack.
 *
 * Revision 2.20  1995/09/19  18:38:09  smackie
 * Allow chunks to be created for a specified memory pool or with a
 * declared base alignment by allowing the data to be detached from
 * the chunk header and freelist. (CSCdi40637)
 *
 * Revision 2.19  1995/07/19  22:48:11  cakyol
 * CSCdi37276:  the LECS does not correctly emit the UP/DOWN transitions
 * - fixed with a more efficient use of available functions
 *
 * Revision 2.18  1995/07/18  22:03:49  cakyol
 * CSCdi37276:  the LECS does not correctly emit the UP/DOWN transitions
 * - fixed.
 *
 * Revision 2.17  1995/07/13  21:39:45  cakyol
 * CSCdi37017:  LECS should advertise its coming up and going down
 * fixed.
 *
 * CSCdi37071:  lecs should migrate to "malloc_named"
 * fixed.
 *
 * Revision 2.16  1995/07/13  01:07:08  cakyol
 * CSCdi37016:  lecs access policy should allow for restricted elans
 * - done.  The main point is that restricted elans cannot now be
 *   joined by name only.  Explicit atm address or mac address
 *   is required.
 *
 * Revision 2.15  1995/07/11  00:08:30  cakyol
 * CSCdi36888:  lecs should display admin state
 * - lecs now displays the admin state
 * - lecs uses "waitingIlmi" instead of "ilmi-"
 * - also uses "waitingListen" instead of "sig-"
 * - main lecs process name is now "LANE LECS Server"
 * - process name for config access is now "LANE CFG Access"
 *
 * Revision 2.14  1995/07/08  02:20:19  nfinn
 * CSCdi36834:  LEC not told when LES/BUS ATM address changes
 *
 * Revision 2.13  1995/07/07  22:59:15  cakyol
 * CSCdi36829:  lecs configured AFTER boot doesnt register with ilmi
 * - fixed.  Note down the fact that a prefix is received whether
 *   an lecs is configured or not on an interface.
 *
 * Revision 2.12  1995/06/29  23:15:38  cakyol
 * CSCdi36582:  lecs should register with ilmi at init stage
 * - major cleanup occured.  The lecs now registers with
 *   ilmi at init time.  Also, lecs does not attempt to
 *   register with the ilmi until at least one prefix add
 *   message from the ilmi is seen.  Lastly, if any registration
 *   or deregistration fails, the LECS will only bitch once
 *   and shut up for good afterwards.  So, it is now the
 *   user's responsibility to make sure to record offending
 *   addresses.
 *
 * Revision 2.11  1995/06/27  00:48:39  nfinn
 * CSCdi36237:  LES/BUS/LECS router hangs and crashes right after config
 * net
 *
 * Revision 2.10  1995/06/22  01:20:48  cakyol
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
 * Revision 2.9  1995/06/20  18:13:09  cakyol
 * CSCdi35998:  no error while configuring LEC on subif missing LECS NSAP
 * fixed
 *
 * Revision 2.8  1995/06/19  06:45:59  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.7  1995/06/18  06:18:49  hampton
 * Change all processes that set their priority to use the new scheduler
 * calls for this purpose. [CSCdi36039]
 *
 * Revision 2.6  1995/06/16 04:28:29  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.5  1995/06/16  01:09:28  cakyol
 * CSCdi35938:  lecs should show as down if none of its adrrs are
 * registered
 * - fixed.  To be operational now, at least one nsap address
 *   must show as fully registered both with ilmi & signalling
 *
 * Revision 2.4  1995/06/13  19:52:55  cakyol
 * CSCdi35790:  LECS should use statechange_complete registry for
 * consistency
 * LECS now uses a combination of "statechange_complete" and
 * "interface_up"
 * to process interface up/down transitions.
 *
 * Revision 2.3  1995/06/09  13:10:58  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.2  1995/06/08 20:41:49  cakyol
 * CSCdi35435:  a quick shut- no shut sequence may lock an addr
 * unregistered
 * fixed by ensuring that at every down->up transition, mark all relevant
 * addresses as unregistered with ILMI, so that they get re-registered
 * when coming up finishes
 *
 * CSCdi35366:
 * also fixed this one.
 *
 * Revision 2.1  1995/06/07  21:20:25  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

static char *ThisFileName = __FILE__;

#include "lecs.h"
#include "logger.h"
#include "../lane/msg_lane.c"		/* Not a typo, see logger.h */

/* whether to issue a Q.2931 release or not */

#define DO_RELEASE		TRUE
#define DO_NOT_RELEASE		FALSE

/* invalid index */

#define NO_INDEX	(-1)

/* timer types */

#define GENERIC_TIMER		1	/* general purpose timer */
#define LES_DISCONNECTED_TIMER	2	/* LES disconnected */

/******************************************************************
*******************************************************************
**	    		global data handles
*******************************************************************
******************************************************************/

/* lecs subsystem control variables */
static boolean G_lecsInited = FALSE;

/* lecs config server control variables */
pid_t G_lecsServerPid = NO_PROCESS;
static watched_queue *lecsInputQ = NULL;
static uint timerTicks;
static ulong lecsAddrsUpdateTime;

/* address registration retry timers etc.. */
static mgd_timer lecsMasterTimer;
static mgd_timer lecsPeriodic;

/* list of all the ACTIVE LecsInfo objects */
ListPtr G_LecsInfosList;

/* list of all the DELETED/INACTIVE LecsInfo objects */
static ListPtr G_DeletedLecsInfosList;

/* ilmi registration bookkeeping variables */
static int G_ilmiClientId = ILMI_ERROR;
static boolean G_ilmiRegisteredForResponses = FALSE;

/* list of all the config tables */
ListPtr G_ConfigTablesList;

/* storage pool for all the connection objects */
static chunk_type *G_ConnectionTable = NULL;

/* minimum acceptable configuration request message length */
static int G_minLaneConfigPakLen;

/* ascending number generator for snmp indexes */
static int G_snmpIndex;

/*
** the forum assigned LECS well known address bytes
*/
uchar FIXED_LECS_NSAP [] =
{
    /* prefix part */
    0x47, 0x00, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00,

    /* esi part */
    0x00, 0xa0, 0x3e, 0x00, 0x00, 0x01,

    /* sel byte */
    0x00
};

/******************************************************************
*******************************************************************
**	    		forward declarations
*******************************************************************
******************************************************************/

static LecsInfo *getLecsPointerFrom_HW(hwidbtype*);
static void ConnList_destroyByCalledParty(LecsInfo *pinfo, 
    LecsAddress *lecsAddr, int release);
static Connection *ConnList_searchByCallingParty(LecsInfo *pinfo,
    Connection *start, register hwaddrtype *callingParty);
static void ConnList_destroyByPtr(LecsInfo *pinfo, 
    Connection *pconn, boolean release);
ElanInfoBlock *getElanByName(ConfigTable *pCfg, 
    char *elanName, int elanNameSize);
static ElanInfoBlock *getElanByLesAddress(ConfigTable *pCfg, 
    hwaddrtype *lesAddr, ServerAtmAddress **psaa);
static void process_LES_connected(LecsInfo *pinfo, 
    Connection *pconn, ServerAtmAddress *psaa);
static void disconnect_LES(ServerAtmAddress *psaa);
static void disconnect_LECS(LecsInfo *pinfo, GlobalLecsAddress*);
static boolean noConnectedHigher_LECS (LecsInfo *pinfo, int me);
static void release_all_circuits(LecsInfo *pinfo, 
    boolean includeLecsConnexions);
static void tearDownLecsConnections(LecsInfo *pinfo);
static void register_ALL_LecsNsaps(LecsInfo *pinfo, 
    boolean registerThem);
static void startLecs(LecsInfo*);
static void stopLecs(LecsInfo*);
static void revisitLecs(LecsInfo*);
static void registerLecsNsap(LecsAddress *addr);
static void deRegisterLecsNsap(LecsAddress *addr);
static void lecsSignallingCB(void *msg);
static void getGlobalLecsAddresses(LecsInfo *pinfo);
static boolean ConfigTable_destroyElanByPtr(ConfigTable *pCfg,
    ElanInfoBlock *pElan);

#define register_all_nsaps(plecs) \
    register_ALL_LecsNsaps(plecs, TRUE)

#define deregister_all_nsaps(plecs) \
    register_ALL_LecsNsaps(plecs, FALSE)

/******************************************************************
*******************************************************************
**	    		helpers & utilities 
*******************************************************************
******************************************************************/

/*
** deliberately bring the router down but print
** the filename and line number where it occured
*/
void lecs_crash (char *filename, int lineNumber)
{
    buginf("\n");
    buginf("\n******************************************");
    buginf("\nLECS initiated deliberate crash:");
    buginf("\nfile: %s line: %d", filename, lineNumber);
    buginf("\n******************************************\n");
    buginf("\n");
    crashdump(10);
}

/*
** Note that this is ONLY an equivalence comparison and NOT a 
** lexicographic comparison.
**
** performs comparison of two byte streams taking into account
** a significance mask associated with the SECOND byte stream.
** The FIRST stream of data bytes is assumed to be ALL significant.
** The mask comes ONLY from the second byte stream.
**
** So, the routine compares ONLY the bits that are significant
** to BOTH the byte streams.  The dont cares are assumed to match
** perfectly.  The trick here is to set the common non significant
** bits to 0 so that they seem like they match.
*/
static inline boolean maskedByteStreamCompare (uchar *data1, 
    uchar *data2, 
    uchar *mask, int count)
{
    ulong maskWord;

    /* do the full words as much as you can */
    while (count >= sizeof(ulong)) {
	maskWord = GETLONG(mask);
	if ((GETLONG(data1) & maskWord) != 
	    (GETLONG(data2) & maskWord))
	    return FALSE;
	mask += sizeof(ulong);
	data1 += sizeof(ulong);
	data2 += sizeof(ulong);
	count -= sizeof(ulong);
    }

    /* do the remaining, byte by byte */
    while (count-- > 0) {
	if ((*data1++ & *mask) != (*data2++ & *mask))
	    return FALSE;
	mask++;
    }

    return TRUE;
}

static boolean invalidMaskedAddress (MaskedAddress *maddr)
{ return (maddr->type == STATION_ILLEGAL) || (maddr->length <= 0); }

/*
** performs an EXACT comparison of data bytes AND mask bytes.
** Only checks equality.  Returns TRUE for equal FALSE for
** NOT equal.  Does NOT perform lexicographic ordering
*/
static inline boolean maskedAddressCompare (MaskedAddress *m1, 
    MaskedAddress *m2)
{
    if (byteStreamCompare 
	   (m1->dataBytes, m2->dataBytes, STATIONLEN_ATMNSAP) != 0)
		return FALSE;
    return 
	byteStreamCompare 
	   (m1->maskBytes, m2->maskBytes, STATIONLEN_ATMNSAP) == 0;
}

/*
** initialize a masked address structure to be of
** a mac address type with some initial data
*/
void maskedAddress_setMacValues (MaskedAddress *maddr, uchar *bytes)
{
    int i;

    maddr->type = STATION_IEEE48;
    maddr->length = STATIONLEN_IEEE48;
    for (i=0; i<STATIONLEN_IEEE48; i++) {
	maddr->dataBytes [i] = bytes ? bytes [i] :(uchar) 0;
	maddr->maskBytes [i] = 0xFF;
    }
}

/*
** initialize a masked address structure to be of
** an atm address type with some initial data
*/
void maskedAddress_setAtmValues (MaskedAddress *maddr, 
    uchar *dataBytes, uchar *maskBytes)
{
    int i;

    maddr->type = STATION_ATMNSAP;
    maddr->length = STATIONLEN_ATMNSAP;
    for (i=0; i<STATIONLEN_ATMNSAP; i++) {
	maddr->dataBytes [i] = dataBytes ? 
	    dataBytes [i] : (uchar) 0;
	maddr->maskBytes [i] = maskBytes ? 
	    maskBytes [i] : (uchar) 0xFF;
    }
}

/*
** initialize a masked address structure to be of
** a route descriptor type with some initial data
*/
void maskedAddress_setRDValues (MaskedAddress *maddr, 
    ushort *dataBytes)
{
    maddr->type = STATION_ROUTEDESC;
    maddr->length = STATIONLEN_ROUTEDESC;

    maddr->dataBytes[0] = (uchar)((0xFF00 & *dataBytes) >> 8);
    maddr->dataBytes[1] = (uchar)(0xFF & *dataBytes);
}

/*
** set the "hwaddr" to have the "fixed" LECS address
*/
void setToFixedLecsAddress (hwaddrtype *hwaddr)
{
    snpa_init(hwaddr, STATION_ATMNSAP, 
	TARGET_UNICAST, FIXED_LECS_NSAP);
}

/*
** generic routine to initialize watched queues
*/
boolean initWatchedQueue (watched_queue **wq, char *qName)
{
    /* already initialized */
    if (*wq != NULL) return TRUE;

    *wq = create_watched_queue(qName, 0, 0);
    if (*wq == NULL)
    {
	LECS_ERR("could not initialize watched queue '%s'", qName);
	return FALSE;
    }
    LECS_DBG(NULL, "watched queue '%s' initialized", qName);
    return TRUE;
}

/******************************************************************
*******************************************************************
**	    		LecsAddress object and
**			Connection Management
*******************************************************************
******************************************************************/

static LecsAddress *LecsAddress_alloc (LecsInfo *pinfo)
{
    LecsAddress *lecsAddr;

    if (! pinfo) LECS_CRASH();
    lecsAddr = malloc_named(sizeof(LecsAddress), "LecsAddress");
    if (lecsAddr != NULL) {
	/* zero everything out */
	memset(lecsAddr, 0, sizeof(LecsAddress));

	/* set the mask to all 1's */
	bcopy(ALL_ONES_MASK, lecsAddr->MASK_BYTES, 
	    STATIONLEN_ATMNSAP);

	/* which LecsInfo I belong to */
	lecsAddr->lecsInfo = pinfo;

	/* make it ready for the list */
	List_clearLinks(lecsAddr);

	lecsAddr->ilmiState = 
	    lecsAddr->sigState = DEREGISTERED;
    }
    return lecsAddr;
}

/*
** destroy an lecs address object.  Note that every connection 
** associated with it must be torn down and other appropriate 
** actions must be taken.
*/
static void LecsAddress_destroy (LecsAddress *lecsAddr)
{
    LecsInfo *pinfo;

    if (lecsAddr == NULL) LECS_CRASH();
    pinfo = lecsAddr->lecsInfo;
    
    /* if there are any connections to THIS address, release them */
    ConnList_destroyByCalledParty(pinfo, lecsAddr, DO_RELEASE);

    /* de register this address from all services */
    deRegisterLecsNsap(lecsAddr);

    /* CLEAR the list pointers just to safeguard for async events */
    List_clearLinks(lecsAddr);

    /* clear the lecsInfo pointer */
    lecsAddr->lecsInfo = NULL;

    free(lecsAddr);
}

/*
** is the lecs address deleted or not ?
** Remember to check the LecsInfo object it is hanging
** from in case it is deleted and the "lecsAddressesList"
** in case the interface is "pacified" (transferred to the
** inactive list of LecsInfos).
*/
static boolean LecsAddress_stillExists (LecsInfo *pinfo, 
    LecsAddress *lecsAddr)
{
    if (! pinfo || ! pinfo->lecsAddressesList) return FALSE;
    return 
	(List_inList(pinfo->lecsAddressesList, lecsAddr) >= 0);
}

/*
** initialize an address structure to be of
** nsap/atm address type with some initial data.  The initial
** data comes "bytes". If this is NULL, contents are set to 0. 
**
** Also, note that if the mask is all ones, the "actualValue"
** can be set now.
**
** If snmpIndex parameter is < 0, then it is set to NO_INDEX 
** (dont care about its value), if it is 0, then it is 
** automatically allocated otherwise it is just used as given.
*/
static void LecsAddress_setAtmValues (LecsAddress *lecsAddr, 
    uchar *dataBytes, uchar *maskBytes, int formationType, 
    int snmpIndex)
{
    /* initialize the masked address structure */
    maskedAddress_setAtmValues
	(&lecsAddr->maskedAddr, dataBytes, maskBytes);

    /* if no mask, set the actual value NOW */
    if (allOnesMask(maskBytes, STATIONLEN_ATMNSAP)) {
	lecsAddr->addrFlags |= ACTUAL_VALUE_VALID;
	MaskedAddress2hwaddrtype
	    (&lecsAddr->maskedAddr, &lecsAddr->actualValue);
    }

    /* how the address was formed */
    lecsAddr->formationType = formationType;

    /* dont care */
    if (snmpIndex < 0)
	lecsAddr->snmpIndex = NO_INDEX;
    else if (snmpIndex == 0)
	/* assign auto value */
	lecsAddr->snmpIndex = G_snmpIndex++;
    else
	/* take what is given */
	lecsAddr->snmpIndex = snmpIndex;
}

/*
** is this the well known lecs address ?
*/
boolean lecsAddressIsWKA (LecsAddress *lecsAddr)
{
    if (!lecsAddr) return FALSE;

    /* if explicitly entered as such, it is */
    if (lecsAddr->formationType == LANE_LECS_BIND_FIXED)
	return TRUE;

    /* check the actual bytes if there */
    if (lecsAddr->addrFlags & ACTUAL_VALUE_VALID) {
	return (byteStreamCompare(lecsAddr->actualValue.addr, 
	    FIXED_LECS_NSAP, STATIONLEN_ATMNSAP) == 0);
    }

    return FALSE;
}

/******************************************************************
*******************************************************************
**	    	   Server Atm Address Object and
**		      Server Redundancy Stuff
*******************************************************************
******************************************************************/

/*
** sends a release or release complete, depending
** on the value of parameter "releaseComplete".
** to initiate the releasing of an ALREADY EXISTING call
*/
void sendRelease (boolean releaseComplete, void *callId, 
    sig_api_cause_code cause, idbtype *swidb)
{
    sig_api_msg msg;

    msg.idb = swidb;
    if (releaseComplete) {
	msg.opcode = ATM_RELEASE_COMPLETE;
	msg.u.release_comp.call_id = callId;
	msg.u.release_comp.cause = cause;
    } else {
	msg.opcode = ATM_RELEASE;
	msg.u.release.call_id = callId;
	msg.u.release.cause = cause;
    }
    atmSig_handleUserRequest(&msg);
}

/*
** release with a debugging message
*/
static void lecsRelease (boolean releaseComplete, void *callId,
    sig_api_cause_code cause, idbtype *swidb)
{
    LECS_DBG(swidb->namestring,
	"sending RELEASE%s for call 0x%x cause %d",
	releaseComplete ? "_COMPLETE" : "",
	callId, cause);
    sendRelease(releaseComplete, callId, cause, swidb);
}

/*
** this routine should be called whenever any connection
** status changes in the LES addresses list.  Becoz any
** such event may cause an LES switchover.
**
** Note that the FIRST connected one in the list SHOULD
** be the active LES since the list is ALWAYS in the
** descending order of priority.
*/
static void reassignActiveLes (ElanInfoBlock *pElan)
{
    ServerAtmAddress *psaa, *current;
    char *ifname;

    current = pElan->activeLes;
    psaa = List_head(pElan->lesAddressesList);
    while (psaa != NULL) {
	/*
	** if connected OR its grace period timer is running, 
	** this MUST now be the current active, since a running
	** timer assumes that the LES is still connected, until
	** it expires.
	*/
	if (psaa->connPtr || mgd_timer_running(&psaa->graceTimer)) {
	    /* no change */
	    if (current == psaa) return;

	    /*
	    ** MUST be set BEFORE disconnect_LES may be called,
	    ** since disconnect_LES will, upon seeing that the
	    ** active one is being disconnected, will call
	    ** reassignActiveLes and hence cause an infinite
	    ** recursion problem.
	    */
	    pElan->activeLes = psaa;
	    
	    ifname = psaa->connPtr ? 
			psaa->connPtr->lecsInfo->name : NULL;
	    LECS_DBG(ifname, "%Ch became active LES for elan '%s'",
		&psaa->lesAddr, pElan->elanName);

	    /* if taking over from another, disconnect old one */
	    if (current) {
		ifname = current->connPtr ?
			    current->connPtr->lecsInfo->name : NULL;
		LECS_DBG(ifname, "disconnecting old active LES %Ch",
		    &current->lesAddr);
		disconnect_LES(current);
	    }

	    /* we are done */
	    return;
	} else if (current == psaa) {
	    /* oops, an UN connected LES is active, correct it */
	    pElan->activeLes = NULL;
	}
	psaa = List_next(psaa);
    }
}

/*
** This routine is called when the LECS deliberately 
** disconnects an LES.
**
** Note that there can be only a SINGLE connection from 
** any calling party to the LECS since LECS always enforces 
** this at call acceptance time.
*/
static void disconnect_LES (ServerAtmAddress *psaa)
{
    ElanInfoBlock *pElan;
    Connection *pconn;
    idbtype *swidb;

    if (psaa == NULL) return;

    /* always clean this out */
    mgd_timer_stop(&psaa->graceTimer);

    /* get the relevant elan */
    pElan = psaa->elanPtr;

    /* if this LES is connected, clean things up */
    pconn = psaa->connPtr;
    if (pconn != NULL) {
	/* clean out the cross pointers */
	pconn->lesAddress = NULL;
	psaa->connPtr = NULL;

	/* disconnect the circuit */
	swidb = pconn->lecsInfo->hwidb->firstsw;
	LECS_DBG(swidb->namestring, 
	    "elan '%s' LES %Ch callId 0x%x "
	    "deliberately being disconnected", 
	    pElan->elanName, &psaa->lesAddr, pconn->callId);
	lecsRelease(FALSE, pconn->callId, 
	    ATM_NORMAL_UNSPECIFIED, swidb);
    }

    /* if this WAS the active LES, we have to find another one now */
    if (pElan->activeLes == psaa) {
	pElan->activeLes = NULL;
	reassignActiveLes(pElan);
    }
}

/*
** actively disconnect an LECS connection.  For incoming calls,
** since I am actively disconnecting it myself as opposed to 
** the network (or the other end), I deliberately set the "index" 
** field of the corresponding "pconn" to NO_INDEX so that the 
** "Connection_dealloc" routine understands this and does not
** attempt to figure out if I should be a master or not.  This is
** basically an indication to that routine.  Similarly, for
** outgoing calls, I set the "callId" to NULL, as soon as I am
** finished with it so that when the response comes in,
** it does not cause me to do anything.
*/
static void disconnect_LECS (LecsInfo *pinfo, 
    GlobalLecsAddress *glecs)
{
    int idx;
    Connection *pconn = NULL;
    void *callId;

    /* sanity */
    if (!pinfo || !glecs) return;

    /*
    ** do the appropriate thing depending on whether
    ** the call is an incoming or outgoing call.
    */
    switch (glecs->callType) {
	case NO_CALL:
	    break;

	case INCOMING_CALL:
	    pconn = glecs->u.pconn;
	    if (! pconn) LECS_CRASH();
	    
	    /* do this BEFORE "ConnList_destroyByPtr" is called */
	    idx = pconn->index;
	    pconn->index = NO_INDEX;

	    /* now free the stuff up */
	    LECS_DBG(pinfo->name,
		"call from LECS %Ch rank %d callId 0x%x "
		"deliberately being disconnected",
		&glecs->hwaddr, idx, pconn->callId);
	    ConnList_destroyByPtr(pinfo, pconn, DO_RELEASE);
	    glecs->u.pconn = NULL;
	    glecs->callType = NO_CALL;
	    break;

	case OUTGOING_CALL:
	    if (! glecs->connected) break;
	    callId = glecs->u.callId;
	    if (! callId) LECS_CRASH();
	    glecs->u.callId = NULL;
	    glecs->callType = NO_CALL;
	    glecs->connected = FALSE;
	    LECS_DBG(pinfo->name,
		"call to LECS %Ch callId 0x%x deliberately "
		"being disconnected",
		&glecs->hwaddr, callId);
	    lecsRelease(FALSE, callId, ATM_NORMAL_UNSPECIFIED,
		pinfo->hwidb->firstsw);
	    break;
    }
}

/*
** sequentially renumber the "priorities" of LES addresses.
*/
static void reIndexLesAddresses (ElanInfoBlock *pElan)
{
    ServerAtmAddress *psaa;
    ListPtr list;
    int index;

    if (! pElan) return;
    list = pElan->lesAddressesList;
    if (! list) return;
    for (index = 0, psaa = List_head(list);
	 psaa;
	 index++, psaa = List_next(psaa))
	    psaa->priority = index;
}

/*
** destroy a server atm address object.  Note that
** if a connection FROM this address exists (which
** is an LES connected to us) and this happens to
** be the active LES, tear the connection down and
** reassign the active LES.
*/
static void serverAtmAddress_destroy (ElanInfoBlock *pElan, 
    ServerAtmAddress *psaa)
{
    ConfigTable *pCfg;
    ServerAtmAddress *deleted;

    if (! pElan || ! psaa) LECS_CRASH();
    pCfg = pElan->cfgTable;

    /*
    ** take the LES address out of the elan.
    ** Do this BEFORE "disconnect_LES" is called.
    */
    List_remove(pElan->lesAddressesList, psaa);

    /* re number the stuff again to make evrything sequential */
    reIndexLesAddresses(pElan);

    deleted = Index_remove
		(pCfg->serverAtmAddressesIndex, &psaa->lesAddr);
    if (deleted != psaa) LECS_CRASH();

    /*
    ** in case this may be connected, take care of it.
    ** MAKE SURE that this is called AFTER the address
    ** is taken off the addresses list.
    */
    disconnect_LES(psaa);

    PRINTF("server atm address %Ch for elan '%s' is deleted\n", 
	&psaa->lesAddr, pElan->elanName);

    /* ok everything cleaned up, free it now */
    mgd_timer_stop(&psaa->graceTimer);
    free(psaa);
}

/*
** compare two server atm address structures by 
** their priorities.
*/
static int compareSAAbyPrio (void *newElem, void *inList)
{
    return ((ServerAtmAddress *) newElem)->priority -
	   ((ServerAtmAddress *) inList)->priority;
}

/*
** check if an LES looks like connected as not an LES.
** This happens when an LES connects but is not yet
** in the config table.  Later, if entered into the
** config table, it does NOT show up as an LES becoz
** at the time of connection, it was not in the table
** so that the LECS could resolve it.  However, now,
** since it is being entered into the table, we will
** find all the previous connections from this address,
** and if they do NOT look like they are from an LES, we 
** will disconnect them so that they can be later connected
** properly.
*/
static void resolveLESconnexionConflict (ConfigTable *pCfg,
    hwaddrtype *lesAddr)
{
    LecsInfo *pinfo;
    Connection *pconn;

    pinfo = List_head(G_LecsInfosList);
    while (pinfo != NULL) {
	if (pinfo->cfgTable == pCfg) {
	    pconn = ConnList_searchByCallingParty(pinfo,
			NULL, lesAddr);
	    if (pconn && (pconn->callerType != CALLER_IS_LES)) {
		LECS_DBG(pinfo->name,
		    "releasing old connection from %Ch callId 0x%x",
		    lesAddr, pconn->callId);
		ConnList_destroyByPtr(pinfo, pconn, DO_RELEASE);
	    }
	}
	pinfo = List_next(pinfo);
    }
}

/*
** allocate a server atm address object and add it to an elan.
** A priority of < 0 indicates that the address is to be added
** to the END of the list, implying lowest priority.  In this case,
** the address is assigned the lowest available priority + 1.
** Otherwise insert it into the appropriate slot.
*/
static ServerAtmAddress *addServerAtmAddressToElan (
    ElanInfoBlock *pElan, hwaddrtype *hwaddr, int priority)
{
    ConfigTable *pCfg;
    ServerAtmAddress *psaa;
    LecInfoBlock *pLec;

    if (! pElan || ! hwaddr) LECS_CRASH();
    pCfg = pElan->cfgTable;

    /*
    ** try and see if the server atm address specified
    ** is also an LEC somewhere in this table.  If so,
    ** this can NOT be allowed.
    */
    pLec = Index_searchElement 
	    (pCfg->UNmaskedAtmAddressIndex, hwaddr->addr, NULL);
    if (pLec != NULL) {
	USER_ERROR("%% specified server atm address is also a ");
	USER_ERROR("client in elan '%s'\n", 
	    pLec->parentElan->elanName);
	return NULL;
    }

    /* allocate and do the rest */
    psaa = malloc_named(sizeof(ServerAtmAddress), 
		"ServerAtmAddress");
    if (psaa == NULL) {
	LECS_ERR("addServerAtmAddressToElan: malloc_named failed");
	return NULL;
    }

    /* allocation succeeded, fill it in */
	
    /* lo prio means end of list */
    if (priority <= LOWEST_PRIO)
	priority = List_size(pElan->lesAddressesList);
    psaa->priority = priority;

    /* insert it into the rite place in the list */
    List_clearLinks(psaa);
    if (! List_insertOrdered(pElan->lesAddressesList, 
	    (CMP) compareSAAbyPrio, psaa, TRUE)) {
	LECS_ERR("addServerAtmAddressToElan: "
	    "List_insertOrdered failed");
	free(psaa);
	return NULL;
    }

    /* we can fill these now */
    psaa->connPtr = NULL;
    psaa->lesAddr = *hwaddr;
    psaa->elanPtr = pElan;

    /* this SHOULD succeed */
    if (Index_add(pCfg->serverAtmAddressesIndex, 
	    hwaddr, psaa) != psaa) {
	LECS_ERR("addServerAtmAddressToElan: Index_add failed");
	List_remove(pElan->lesAddressesList, psaa);
	free(psaa);
	return NULL;
    }

    /* 
    ** if an existing connection from this address
    ** already exists AND it "looks" like it is NOT
    ** from an LES, then disconnect it.
    */
    resolveLESconnexionConflict(pCfg, hwaddr);

    /* re number the stuff again to make everything sequential */
    reIndexLesAddresses(pElan);

    /* do this last */
    mgd_timer_init_leaf(&psaa->graceTimer, &lecsMasterTimer, 
	LES_DISCONNECTED_TIMER, psaa, FALSE);

    PRINTF("added a server atm address to elan '%s'\n", 
	pElan->elanName);
    
    return psaa;
}

/*
** here is the logic:
**
** 1 - If neither the elan name nor the LES address is specified,
**     this is an error.
**
** 2 - If an elan name AND an LES address is also specified,
**     after making sure that the specified LES is indeed in the
**     specified elan, delete the LES address from that elan.
**
** 3 - If ONLY the elan name is specified, delete the elan.
**
** 4 - If ONLY the LES address is specified, delete the LES address
**     from whatever elan it belongs to.
**
** 5 - If we were only deleting an LES address from an elan, and
**     we find that after the deletion, there is no more LES
**     addresses left for this elan, then delete the elan as well.
*/
boolean deleteElan_AndOr_ServerAtmAddress (ConfigTable *pCfg, 
    char *elanName, hwaddrtype *lesAddr)
{
    ServerAtmAddress *psaa = NULL;
    ElanInfoBlock *pElan = NULL;
    ElanInfoBlock *pElanByLESAddr = NULL;
    ElanInfoBlock *elanToBeDeleted = NULL;

    if (! pCfg) LECS_CRASH();
    if (!elanName && !lesAddr) {
	USER_ERROR("%% incomplete command\n");
	return FALSE;
    }

    /* get the elan if specified */
    if (elanName) {
	pElan = getElanByName(pCfg, elanName, strlen(elanName));
	if (!pElan) {
	    USER_ERROR("%% specified elan is not in "
		"this config database\n");
	    return FALSE;
	}
    }

    /* get the LES address if specified */
    if (lesAddr) {
	pElanByLESAddr = getElanByLesAddress(pCfg, lesAddr, &psaa);
	if (!pElanByLESAddr || !psaa) {
	    USER_ERROR("%% specified LES address does not "
		"belong to any elan in this config database\n");
	    return FALSE;
	}
    }

    /* if both specified, they MUST imply the same elan */
    if (pElan && pElanByLESAddr && (pElan != pElanByLESAddr)) {
	USER_ERROR("%% specified LES address does not "
	    "belong to the specified elan");
	return FALSE;
    }

    /* if we are here, everything is consistent, so proceed */

    /*
    ** If LES address is specified, delete it from elan.
    ** Also, if after the deletion, no more LES addresses
    ** are left for the elan, then delete the elan as well.
    */
    if (pElanByLESAddr && psaa) {
	serverAtmAddress_destroy(pElanByLESAddr, psaa);
	if (List_size(pElanByLESAddr->lesAddressesList) <= 0)
	    elanToBeDeleted = pElanByLESAddr;
    } else if (pElan) {
	/* if elan deletion explicitly required, delete the elan */
	elanToBeDeleted = pElan;
    }
    
    /* delete the elan or not ? */
    if (elanToBeDeleted)
	ConfigTable_destroyElanByPtr(pCfg, elanToBeDeleted);

    return TRUE;
}

/*
** clean up ALL the LES redundancy connection info from
** the elans etc in the specified config table.  If pElan
** is specified, only that elan's info is cleared otherwise
** if it is NULL, all the elan's are cleared.
*/
static void cleanLesRedundancyInfo (ConfigTable *pCfg, 
    ElanInfoBlock *pElan)
{
    ElanInfoBlock *first, *last;
    ServerAtmAddress *psaa;

    if (pElan != NULL)
	first = last = pElan;
    else {
	first = List_head(pCfg->elanList);
	last = List_tail(pCfg->elanList);
    }

    while (first != NULL) {
	psaa = List_head(first->lesAddressesList);
	while (psaa != NULL) {
	    disconnect_LES(psaa);
	    psaa = List_next(psaa);
	}
	if (first->activeLes != NULL) LECS_CRASH();
	PRINTF("cleaned up redundancy LES conn info in elan '%s'\n",
	    first->elanName);
	if (first == last) break;
	first = List_next(first);
    }
}

/*
** Determines whether an LECS should be a master or not.
** Once the determination is done, if the LECS is a master,
** it registers the well known address if any.  Otherwise,
** if it is a slave, DE-registers the well known address
** if any AND disconnects all the non LECS connexions.
*/
static void evaluateLecsMastership (LecsInfo *pinfo)
{
    LecsAddress *lecsAddr;
    boolean oldstate, master, only_wka;
    int localRank;

    /* see if an LECS is actually running here */
    if (!pinfo) return;
    if (!pinfo->lecsRunning) {
	pinfo->thisLecsIsTheMaster = FALSE;
	return;
    }

    /* assume backup by default */
    only_wka = master = FALSE;
    oldstate = pinfo->thisLecsIsTheMaster;

    /* 
    ** if the LECS has a single address AND
    ** it is the well known LECS address,
    ** then this LECS SHOULD be the master.
    */
    if (List_size(pinfo->lecsAddressesList) == 1) {
	lecsAddr = List_head(pinfo->lecsAddressesList);
	if (lecsAddressIsWKA(lecsAddr)) {
	    only_wka = TRUE;
	    master = TRUE;
	}
    }

    /*
    ** otherwise, the only criteria that determines
    ** whether an LECS is a master or not is whether
    ** it has any vc's open to it from "superior"
    ** LECSs
    */
    if (!master) {
	localRank = myHighestRank(pinfo, NULL);
	if (noConnectedHigher_LECS(pinfo, localRank))
	    master = TRUE;
    }

    /* advertise if a real change occured */
    if (master != oldstate) {
	LECS_INFO("LECS on interface %s became %s", 
	    pinfo->name, 
	    master ? "the MASTER" : "a BACKUP");
    }

    /* always update with new state */
    pinfo->thisLecsIsTheMaster = master;

    /*
    ** if we just became a master, register
    ** any well known address lurking around.
    ** On the other hand, if we are becoming
    ** a slave, de-register any well knowns.
    */
    lecsAddr = List_head(pinfo->lecsAddressesList);
    while (lecsAddr) {
	if (lecsAddressIsWKA(lecsAddr)) {
	    if (master)
		registerLecsNsap(lecsAddr);
	    else
		deRegisterLecsNsap(lecsAddr);
	}
	lecsAddr = List_next(lecsAddr);
    }

    /*
    ** if we are the master AND that we ONLY have
    ** the single well known LECS address, tear
    ** down any existing connexions to any/all
    ** LECS's since we are NOT really participating
    ** in SSRP in the true meaning of the word.
    **
    ** This behaviour is also consistent with
    ** what would happen if we INITIALLY came up
    ** as a master with ONLY the WKA.  In that
    ** situation, we would NOT have opened any vc's
    ** to any other LECS's.
    **
    ** In any case, if we became a slave,
    ** disconnect all the NON LECS connexions 
    */
    if (master) {
	if (only_wka)
	    tearDownLecsConnections(pinfo);
    } else {
	release_all_circuits(pinfo, FALSE);
    }
}

/*
** The call is from an LES.  Process it accordingly.
*/
static void process_LES_connected (LecsInfo *pinfo, 
    Connection *pconn, ServerAtmAddress *psaa)
{
    ElanInfoBlock *pElan = psaa->elanPtr;

    /* stop the timer in case this was a reconnection */
    mgd_timer_stop(&psaa->graceTimer);
    LECS_DBG(pinfo->name, "grace timer for LES %Ch stopped",
	&psaa->lesAddr);

    /* set up the cross pointers with the connection */
    psaa->connPtr = pconn;
    pconn->lesAddress = psaa;

    /* one MORE LES connexion for this elan */
    LECS_DBG(pinfo->name, "LES %Ch for elan '%s' connected",
	&psaa->lesAddr, pElan->elanName);

    /*
    ** we MAY have to reassign a new active LES 
    ** if this one is of higher priority
    */
    reassignActiveLes(pElan);
}

/*
** process the fact that an LECS is connecting to us.
** This is an INCOMING call, so store it properly.
**
** Since this is called when the connection has originated
** from a "higher" LECS, this must mean that I have to
** become a slave now.
*/
static void process_higher_LECS_connected (LecsInfo *pinfo, 
    Connection *pconn, int index)
{
    GlobalLecsAddress *glecs;

    if (!pinfo || !pconn || (index < 0) || 
	(index >= pinfo->globalLecsCount))
	    LECS_CRASH();

    /* point to the relevant global lecs address */
    glecs = &pinfo->globalLecsTable [index];

    /* set it up as an incoming call */
    glecs->callType = INCOMING_CALL;
    glecs->u.pconn = pconn;

    /* what should I be ? */
    evaluateLecsMastership(pinfo);
}

/*
** this routine returns the "rank", ordering of this
** LECS relative to all the global LECSs in the system.
** Note that since an LECS can have more than one active
** address, we want to find the highest possible rank.
** Therefore do the comparison by comparing from the top
** of the GLOBAL list first.
*/
int myHighestRank (LecsInfo *pinfo, LecsAddress **highest)
{
    LecsAddress *lecsAddr;
    int i;
    uchar *addr;

    for (i=0; i<pinfo->globalLecsCount; i++) {
	addr = pinfo->globalLecsTable [i].hwaddr.addr;
	lecsAddr = searchLecsNsapActualValues(pinfo, addr);
	if (lecsAddr) {
	    if (highest) *highest = lecsAddr;
	    return i;
	}
    }

    /* none found; assume I am the highest rank */
    if (highest) *highest = NULL;
    return 0;
}

/*
** determine if there are any LECSs above me that are connected
** to me.  If not, I ought to be a master.
*/
static boolean noConnectedHigher_LECS (LecsInfo *pinfo, int me)
{
    int i;

    for (i=0; i<pinfo->globalLecsCount; i++) {
	if (i >= me) return TRUE;
	if (pinfo->globalLecsTable [i].callType != NO_CALL) {
	    if (pinfo->globalLecsTable [i].callType != INCOMING_CALL)
		LECS_ERR("noConnectedHigher_LECS: inconsistent call");
	    return FALSE;
	}
    }
    return TRUE;
}

/******************************************************************
*******************************************************************
**	    		SVC Connection object
*******************************************************************
******************************************************************/

/*
** represents a single open connection to the LECS on
** a specific LecsInfo.
*/
static Connection *Connection_alloc (LecsInfo *pinfo)
{
    Connection *pconn;

    /*
    ** if connection table chunk is not yet
    ** allocated, allocate it here and now.
    */
    if (G_ConnectionTable == NULL) {
	G_ConnectionTable = 
	    chunk_create(sizeof(Connection), 64,
		 CHUNK_FLAGS_DYNAMIC, NULL, 0, "connTable");
	if (G_ConnectionTable == NULL)
	{
	    LECS_ERR("chunk_create for G_ConnectionTable failed");
	    return NULL;
	}
	LECS_DBG(NULL, "G_ConnectionTable allocated");
    }

    /* now allocate a connection object */
    pconn = chunk_malloc(G_ConnectionTable);
    if (pconn != NULL) {
	bzero(pconn, sizeof(Connection));
	snpa_clear(&pconn->callingParty);
	pconn->lecsInfo = pinfo;
	List_clearLinks(pconn);
	List_write(pinfo->connectionList, pconn);
    }
    return pconn;
}

/*
** destroy a SINGLE Connection object on a specific LecsInfo.  
** Also, depending on whether this is an LES or an LECS
** connection, take proper action.
*/
static void Connection_dealloc (LecsInfo *pinfo, Connection *pconn)
{
    ServerAtmAddress *psaa;
    ElanInfoBlock *pElan;
    GlobalLecsAddress *glecs;

    if (pconn == NULL) LECS_CRASH();

    /* if we are here, pinfo should NOT be NULL */
    if (pinfo == NULL) LECS_CRASH();

    /*
    ** if an LES is disconnecting, the "lesAddress" pointer
    ** should be non NULL.  If that is the case, then process
    ** it accordingly.  If however the "lesAddress" pointer
    ** is NULL, we must have cleared it before, deliberately, 
    ** so, in that case, there is nothing more to do.
    ** 
    ** If the LES disconnected itself (we did NOT disconnect it),
    ** then we must clear its cross pointers AND if this is the 
    ** active LES for this elan, we must start its grace timer.
    ** 
    ** Note that we should NOT start the timer if the lecs
    ** process is dead.  Since this is the process which
    ** deals with expired timer, the timer will get stuck
    ** as expired and will never be processed if the process
    ** is dead.
    */
    if ((pconn->callerType == CALLER_IS_LES) && pconn->lesAddress) {
	psaa = pconn->lesAddress;
	pElan = psaa->elanPtr;
	psaa->connPtr = NULL;
	pconn->lesAddress = NULL;
	if ((pElan->activeLes == psaa) && 
	    (G_lecsServerPid != NO_PROCESS)) {
	    mgd_timer_start(&psaa->graceTimer, 
		(pElan->lesTimeout * ONESEC));
	    LECS_DBG(pconn->lecsInfo->name, 
		"LES %Ch %d second disconnect grace timer started", 
		&psaa->lesAddr, pElan->lesTimeout);
	}
    }
    else if ((pconn->callerType == CALLER_IS_LECS) &&
	(pconn->index != NO_INDEX)) 
    {
	/*
	** if an LECS connection disconnected, AND it was
	** unsolicited (index != NO_INDEX), then see if our 
	** mastership status changes.
	*/

	/* clear the corresponding call */
	glecs = &pinfo->globalLecsTable [pconn->index];
	if ((glecs->callType == INCOMING_CALL) && 
	    (glecs->u.pconn == pconn)) 
	{
	    glecs->u.pconn = NULL;
	    glecs->callType = NO_CALL;
	    pconn->index = NO_INDEX;
	}
	else
	    LECS_ERR("inconsistency in Connection_dealloc");

	/*
	** since a higher LECS disconnected from us, see if
	** we are supposed to be the master.  This will be
	** the case, if there are NO other higher LECSs left
	** connected to us.  Just call evaluateLecsMastership, 
	** which will do all the work.
	*/
	evaluateLecsMastership(pinfo);
    }

    /* clean up the corresponding vcd indexed entry */
    pinfo->connexions [pconn->vcd] = NULL;

    /* destroy the actual connection object */
    List_remove(pinfo->connectionList, pconn);
    List_clearLinks(pconn);
    chunk_free(G_ConnectionTable, pconn);
}

/*
** Find a specific svc based on "callId" on a specific LecsInfo.
** If its vcd has been validly specified however, just directly 
** search on the vcd itself.
*/
static Connection *ConnList_searchByCallId (LecsInfo *pinfo, 
    register void *callId, vcd_t vcd)
{
    if (! pinfo || ! callId) LECS_CRASH();
    if (invalid_vcd(vcd)) {
	vcd = atmSig_api_callId2vcd(callId);
	if (invalid_vcd(vcd)) return NULL;
    }
    return pinfo->connexions [vcd];
}

/*
** Search a connection based on the called party address.
** this CAN match multiple times and hence the ability
** to be able to specify the starting point in the search.
** If start is NULL, search starts at the beginning of list,
** otherwise it starts from specified location.
*/
static Connection *ConnList_searchByCalledParty (LecsInfo *pinfo, 
    Connection *start, register LecsAddress *calledParty)
{
    register Connection *pconn;

    pconn = (start == NULL) ? 
	List_head(pinfo->connectionList) : start;
    while (pconn != NULL) {
	if (pconn->calledParty == calledParty) return pconn;
	pconn = List_next(pconn);
    }
    return NULL;
}

/*
** Search a connection based on the calling party address.
** this CAN match multiple times and hence the ability
** to be able to specify the starting point in the search.
**
** However, since the LECS enforces the policy of only a
** single call from the same calling party, in this code,
** this will only match once.
**
** If start is NULL, search starts at the beginning of list,
** otherwise search starts at the specified connection object.
**
** Also note that some connections may NOT have a valid
** calling party.  Exclude those from the search.  This
** is why the "lane_addr_valid" check is first made.
*/
static Connection *ConnList_searchByCallingParty (LecsInfo *pinfo, 
    Connection *start, register hwaddrtype *callingParty)
{
    register Connection *pconn;

    if (!lane_addr_valid(callingParty)) 
	return NULL;

    pconn = (start == NULL) ? 
	List_head(pinfo->connectionList) : start;
    while (pconn != NULL) {
	if (lane_addr_valid(&pconn->callingParty) &&
	    (byteStreamCompare(pconn->callingParty.addr,
		callingParty->addr, STATIONLEN_ATMNSAP) == 0))
		    return pconn;
	pconn = List_next(pconn);
    }
    return NULL;
}

/*
** destroy directly by the pointer.  If "release" is set,
** then release the SVC call before completely destroying
** the object.  Otherwise we are deleting the object as a
** result of getting a release notification from signalling.
*/
static void ConnList_destroyByPtr (LecsInfo *pinfo, 
    Connection *pconn, boolean release)
{
    if (! pconn) LECS_CRASH();
    if (release)
	lecsRelease(FALSE, pconn->callId, 
	    ATM_UNALLOCATED, pinfo->hwidb->firstsw);
    Connection_dealloc(pinfo, pconn);
}

/*
** Find a connection specified by "callId" and destroy it.
** It is expected to delete at most one.
*/
static void ConnList_destroyByCallId (LecsInfo *pinfo, void *callId, 
    vcd_t vcd, boolean release)
{
    Connection *pconn;

    pconn = ConnList_searchByCallId(pinfo, callId, vcd);
    if (pconn) ConnList_destroyByPtr(pinfo, pconn, release);
}

/*
** this CAN match more than once meaning more than one connection
** to the LECS which is perfectly valid.  This will be used when the
** administrator deletes an nsap from the lecs list of nsaps or an
** entire LecsInfo object.  
*/
static void ConnList_destroyByCalledParty (LecsInfo *pinfo, 
    LecsAddress *lecsAddr, int release)
{
    Connection *pconn, *deleter;

    pconn = NULL;
    do {
	pconn = ConnList_searchByCalledParty(pinfo, pconn, lecsAddr);
	if (pconn != NULL) {
	    deleter = pconn;
	    pconn = List_next(pconn);
	    ConnList_destroyByPtr(pinfo, deleter, release);
	}
    } while (pconn != NULL);
}

/******************************************************************
*******************************************************************
**	    		ILMI and signalling mgmt
*******************************************************************
******************************************************************/

/*
** actions to be taken when the ilmi has failed
** to perform what we wanted it to do
*/
static void ILMISetFailureCB (LecsAddress *lecsAddr, 
    int addDelete, int error, int response, uchar *nsap)
{
    char *ifname = lecsAddr ? lecsAddr->lecsInfo->name : "";

    /* always valid */
    if (lecsAddr) lecsAddr->ilmiState = DEREGISTERED;

    /*
    ** emphasize a registration problem and
    ** subdue a de-registration problem.
    */
    if (addDelete == ILMI_ADD_ADDR) {
	errmsg(&msgsym(NOREGILMI, LANE), ifname, "LECS", nsap);
    } else if (addDelete == ILMI_DELETE_ADDR) {
	LECS_DBG(ifname, "can't DE-register %Cj from ILMI (err=%d resp=%d)",
	    nsap, error, response);
    } else {
	LECS_ERR("ILMISetFailureCB");
    }
}

/*
** similar to above, these are the actions to be taken 
** if the ilmi call succeeded (whether registration OR 
** deregistration).
*/
static void ILMISetSuccessCB (LecsAddress *lecsAddr, 
    int addDelete, uchar *nsap)
{
    char *ifname = lecsAddr ? lecsAddr->lecsInfo->name : NULL;

    switch (addDelete) {
	case ILMI_ADD_ADDR:
	    if (lecsAddr) lecsAddr->ilmiState = REGISTERED;
	    LECS_DBG(ifname, "%Cj: ilmi registered", nsap);
	    break;
	case ILMI_DELETE_ADDR:
	    if (lecsAddr) lecsAddr->ilmiState = DEREGISTERED;
	    LECS_DBG(ifname, "%Cj: ilmi DE-registered", nsap);
	    break;
	default:
	    LECS_ERR("ILMISetSuccessCB");
    }
}

/*
** was the ILMI transaction erroneous ?
*/
boolean ILMI_Error (ilmiClientReq *resp)
{
    return 
	(resp->error != ilmiResponseReceived) || 
	(resp->response != ilmiRespNoErr);
}

/*
** ilmi add/delete nsap address request callback handler.
*/
static void lecsIlmiCB (void *arg)
{
    hwidbtype *hwidb;
    ilmiClientReq *resp = (ilmiClientReq*) arg;
    LecsAddress *lecsAddr;
    int addDelete;
    LecsInfo *pinfo;
    uchar nsap [STATIONLEN_ATMNSAP];
    int i;

    hwidb = hwidb_or_null_from_index(resp->intf_id);
    pinfo = getLecsPointerFrom_HW(hwidb);
    if (!pinfo) return;

    /* now sort the action out */
    switch (resp->notify_type) {

	/* response to the previous "set" */
	case ilmiResponseNotify:

	    for (i=0; i<STATIONLEN_ATMNSAP; i++) 
		nsap [i] = (uchar) resp->vblist->name->oid_ptr
		    [i+START_OF_ATM_ADDRESS_INDEX];

	    lecsAddr = (LecsAddress*) resp->transid;
	    addDelete = resp->vblist->value.sl_value;
	    if (!LecsAddress_stillExists(pinfo, lecsAddr))
		lecsAddr = NULL;
	    if (ILMI_Error(resp))
		ILMISetFailureCB(lecsAddr, addDelete, 
		    resp->error, resp->response, nsap);
	    else
		ILMISetSuccessCB(lecsAddr, addDelete, nsap);
	    break;

	default:
	    LECS_ERR("lecsIlmiCB");
    }
}

/*
** successfull processing of an atm signalling listen/cancel
** nsap request.
*/
static void atmsigListenCancelCBSuccess (LecsAddress *lecsAddr, 
    int opcode, hwaddrtype *hwaddr)
{
    char *ifname = lecsAddr ? lecsAddr->lecsInfo->name : NULL;

    switch (opcode) {

	case ATM_LISTEN_NSAP:

	    LECS_DBG(ifname, "%Ch: signalling registered", hwaddr);
	    if (lecsAddr) {
		lecsAddr->sigState = REGISTERED;
		getGlobalLecsAddresses(lecsAddr->lecsInfo);
	    }
	    break;

	case ATM_CANCEL_NSAP:

	    if (lecsAddr) lecsAddr->sigState = DEREGISTERED;
	    LECS_DBG(ifname, "%Ch: signalling DE-registered", hwaddr);
	    break;

	default:

	    LECS_ERR("atmsigListenCancelCBSuccess");
    }
}

static void atmsigListenCancelCBFailure (LecsAddress *lecsAddr, 
    int opcode, hwaddrtype *hwaddr)
{
    char *ifname = lecsAddr ? lecsAddr->lecsInfo->name : "";
    /* correct assumption */
    if (lecsAddr) lecsAddr->sigState = DEREGISTERED;

    /* emphasize a registration error */
    if (opcode == ATM_LISTEN_NSAP) {
	LECS_WARN("%s: can't register %Ch with signalling "
	    "(duplicate address ?)",
	    ifname, hwaddr);
    }
}

/*
** signalling LISTEN/CANCEL nsap handler callback
*/
static void atmsigListenCancelCB (sig_api_msg *msg)
{
    int opcode = msg->opcode;
    LecsAddress *lecsAddr;
    int result;
    LecsInfo *pinfo;
    hwaddrtype *hwaddr;

    if (msg->idb == NULL) {
	LECS_ERR("atmsigListenCancelCB 1");
	return;
    }
    if (msg->idb->lane_info == NULL) return;
    pinfo = msg->idb->lane_info->lecs_info;

    /* 
    ** this may happen, if the entire LecsInfo was deleted
    ** BEFORE the signalling called us back
    */
    if (pinfo == NULL) return;

    /* type of operation ? */
    if (opcode == ATM_LISTEN_NSAP) {
	result = msg->u.listen.result;
	hwaddr = &msg->u.listen.nsap;
	lecsAddr = (LecsAddress*) msg->u.listen.user_handle;
    } else if (opcode == ATM_CANCEL_NSAP) {
	result = msg->u.cancel.result;
	hwaddr = &msg->u.cancel.nsap;
	lecsAddr = (LecsAddress*) msg->u.cancel.user_handle;
    } else {
	LECS_ERR("atmsigListenCancelCB 2");
	return;
    }

    /* has the address disappeared in the meantime */
    if (!LecsAddress_stillExists(pinfo, lecsAddr))
	lecsAddr = NULL;

    if (result == ATM_SIGAPI_OK)
	atmsigListenCancelCBSuccess(lecsAddr, opcode, hwaddr);
    else
	atmsigListenCancelCBFailure(lecsAddr, opcode, hwaddr);

    /* final status */
    /* revisitLecs(pinfo); */
}

/*
** registers LECS with the specified service of the ilmi.
** The same callback is used all the time.
*/
static boolean registerWithIlmiService (ilmiClientNotifyType 
    serviceType)
{
    ilmiClientReq req;

    if (G_ilmiClientId == ILMI_ERROR) return FALSE;
    req.client_id = G_ilmiClientId;
    req.notifyCB = lecsIlmiCB;
    req.notify_type = serviceType;
    if (ilmi_register_service(&req) != ILMI_OK) {
	LECS_ERR("registerWithIlmiService");
	return FALSE;
    }
    LECS_DBG(NULL, "ILMI registration with service %d succeeded",
	serviceType);
    return TRUE;
}

/*
** COMPLETE SYTEM WIDE registration with the ilmi, as a client.
** NOT an individual address registration/deregistration.
** 
** If ANY of the registration phases fail, we return FALSE.
** ALL of them MUST succeed to be able to return TRUE.
*/
static boolean registerLECSwithILMI (void)
{
    boolean returnValue = TRUE;

    /* register as a client */
    if (G_ilmiClientId == ILMI_ERROR) {
	G_ilmiClientId = ilmi_register_client("LECS");
	
	/* ilmi registration failed */
	if (G_ilmiClientId == ILMI_ERROR) {
	    LECS_ERR("registerLECSwithILMI");
	    returnValue = FALSE;
	} else {
	    LECS_DBG(NULL, 
		"registered with ilmi as a client (id=%d)", 
		G_ilmiClientId);
	}
    }

    /*
    ** register for response notifications to client requests.
    ** In the LECS case, this will only be address sets.
    */
    if (! G_ilmiRegisteredForResponses) {
	if (registerWithIlmiService(ilmiResponseNotify))
	    G_ilmiRegisteredForResponses = TRUE;
	else
	    returnValue = FALSE;
    }

    return returnValue;
}

/*
** prepares the appropriate address [DE] registration 
** messages to be sent to the ilmi subsystem.
*/
static int sendSetRequestToILMI (int addDelete, idbtype *swidb, 
    LecsAddress *lecsAddr)
{
    ilmiClientReq req;
    ulong *ulptr;
    int i;

    /* fill in some basic stuff */
    req.client_id = G_ilmiClientId;
    req.intf_id = swidb->hwptr->hw_if_index;
    req.oper_type = ilmiSet;
    req.transid = (ulong) lecsAddr;

    /* now copy the atm address into the oid */
    req.vblist = &IlmiSetAddressVarBind;
    ulptr = &req.vblist->name->oid_ptr [START_OF_ATM_ADDRESS_INDEX];
    for (i=0; i<STATIONLEN_ATMNSAP; i++)
	*ulptr++ = (ulong) lecsAddr->actualValue.addr [i];

    /* add or delete operation */
    req.vblist->value.sl_value = addDelete;

    /* hand it off to the ilmi process */
    return ilmi_client_request(&req);
}

/*
** individual address DE-registration from the ilmi.
** Always assume success and can be sent multiple
** times without a complaint.
*/
static void ilmiDeRegisterAddress (idbtype *swidb, 
    LecsAddress *lecsAddr)
{
    /* if systemwide ilmi registration is NOT yet done, do so */
    if (! registerLECSwithILMI()) return;

    /* actual value MUST have been filled prior to coming here */
    if (! (lecsAddr->addrFlags & ACTUAL_VALUE_VALID)) return;

/* we can send it repeatedly, no harm done */
#if 0
    if (lecsAddr->ilmiState == DEREGISTERED) return;
    if (lecsAddr->ilmiState == DEREGISTERING) return;
#endif

    /* prepare and transmit the message to ilmi */
    if (sendSetRequestToILMI(ILMI_DELETE_ADDR, swidb, lecsAddr) == ILMI_OK) {

	/* always assume success */
	lecsAddr->ilmiState = DEREGISTERED;

	LECS_DBG(swidb->namestring,
	    "placed de-register address 0x%x (%Ch) request with ILMI",
	    lecsAddr, &lecsAddr->actualValue)
    } else {
	LECS_DBG(swidb->namestring, "ilmiDeRegisterAddress: "
	    "sendSetRequestToILMI failure; interface down ?");
    }
}

/*
** individual address registration with the ilmi.
** Can be sent multiple times without a complaint.
*/
static void ilmiRegisterAddress (idbtype *swidb, 
    LecsAddress *lecsAddr)
{
    /* if systemwide ilmi registration is NOT yet done, do so */
    if (! registerLECSwithILMI()) return;

    /* actual value MUST have been filled prior to coming here */
    if (! (lecsAddr->addrFlags & ACTUAL_VALUE_VALID)) return;

/* we can send it repeatedly, no harm done */
#if 0
    if (lecsAddr->ilmiState == REGISTERING) return;
    if (lecsAddr->ilmiState == REGISTERED) return;
#endif

    /* prepare and transmit the request to the ilmi process */
    if (sendSetRequestToILMI(ILMI_ADD_ADDR, swidb, lecsAddr) == ILMI_OK) {
	lecsAddr->ilmiState = REGISTERING;
	LECS_DBG(swidb->namestring,
	    "placed register address 0x%x (%Ch) request with ILMI",
	    lecsAddr, &lecsAddr->actualValue)
    } else {
	lecsAddr->ilmiState = DEREGISTERED;
	LECS_DBG(swidb->namestring, "ilmiRegisterAddress: "
	    "sendSetRequestToILMI failure; interface down ?");
    }
}

/*
** individual address DE-registration from atm signalling.
** Can be sent multiple times without a problem and can
** always be assumed to be successfully completed.
*/
static void signallingDeRegisterAddress (idbtype *swidb, 
    LecsAddress *lecsAddr)
{
    sig_api_msg msg;

    /* actual value MUST have been filled */
    if (! (lecsAddr->addrFlags & ACTUAL_VALUE_VALID)) return;

/* we can send it repeatedly, no harm done */
#if 0
    if (lecsAddr->sigState == DEREGISTERED) return;
    if (lecsAddr->sigState == DEREGISTERING) return;
    lecsAddr->sigState = DEREGISTERING;
#endif

    /* prepare the message */
    msg.opcode = ATM_CANCEL_NSAP;
    msg.idb = swidb;
    msg.u.cancel.nsap = lecsAddr->actualValue;
    msg.u.cancel.user_handle = lecsAddr;

    /* hand it off to the signalling process */
    atmSig_handleUserRequest(&msg);

    /* always assume success */
    lecsAddr->sigState = DEREGISTERED;
    
    LECS_DBG(swidb->namestring,
	"placed de-register address 0x%x (%Ch) request with signalling",
	lecsAddr, &lecsAddr->actualValue)
}

/*
** individual address registration with atm signalling.
** Do NOT send duplicate signalling registers requests.
*/
static void signallingRegisterAddress (idbtype *swidb, 
    LecsAddress *lecsAddr)
{
    sig_api_msg msg;

    /* actual value MUST have been filled */
    if (! (lecsAddr->addrFlags & ACTUAL_VALUE_VALID)) return;

    /*
    ** do NOT send this request in duplicate,
    ** signalling complains about this one.
    */
    if (lecsAddr->sigState == REGISTERING) return;
    if (lecsAddr->sigState == REGISTERED) return;
    lecsAddr->sigState = REGISTERING;

    /* prepare the message */
    msg.opcode = ATM_LISTEN_NSAP;
    msg.idb = swidb;
    msg.u.listen.nsap = lecsAddr->actualValue;
    msg.u.listen.sig_user_callback = lecsSignallingCB;
    msg.u.listen.user_handle = lecsAddr;

    /* hand it off to the signalling process */
    atmSig_handleUserRequest(&msg);
    
    LECS_DBG(swidb->namestring,
	"placed register address 0x%x (%Ch) request with signalling",
	lecsAddr, &lecsAddr->actualValue)
}

/*
** complete address registration procedure
*/
static void registerLecsNsap (LecsAddress *lecsAddr)
{
    idbtype *firstsw;
    lane_ilmi_compback_t rc;
    LecsInfo *pinfo;

    pinfo = lecsAddr->lecsInfo;
    firstsw = firstsw_or_null(pinfo->hwidb);
    if (firstsw == NULL) LECS_CRASH();

    /* always re-compute the actual address */
    rc = lane_ilmi_compute_atmsap 
	   (firstsw, LANE_ILMI_CONFIG_SERVER,
	     &lecsAddr->maskedAddr, &lecsAddr->actualValue);
    if (rc != LANE_ILMI_COMPUTE_SUCCESS) {
	LECS_DBG(pinfo->name,
	    "actualValue for %Cw could NOT be set: "
	    "lane_ilmi_compute_atmsap error %s",
	    &lecsAddr->maskedAddr, lane_ilmi_error_text(rc));

	/*
	** do NOT invalidate what is already in the actual value,
	** otherwise we may get into a situation where we think
	** the address is NOT registered with signalling.  This
	** happens if the switch power cycles and stays down for
	** more than a cycle period.
	*/
	return;
    }

    /* set at least once */
    lecsAddr->addrFlags |= ACTUAL_VALUE_VALID;

    /* special processing for the WKA */
    if (lecsAddressIsWKA(lecsAddr)) {

	/* it gets registered ONLY if this LECS is master */
	if (pinfo->thisLecsIsTheMaster) {

	    /* 
	    ** This is to supress printing multiple times.
	    ** We use the sigState instead of the ilmiState
	    ** since it is MUCH MORE reliable than the ILMI.
	    */
	    if (lecsAddr->sigState == DEREGISTERED) {
		LECS_INFO("%s: starting to listen on "
		    "the well known LECS address",
		    pinfo->name);
	    }

	/* the LECS is NOT the master, skip it */
	} else {
	    LECS_DBG(pinfo->name,
		"skipping well known address registration");
	    return;
	}
    }

    /* send messages to both subsystems */
    signallingRegisterAddress(firstsw, lecsAddr);
    ilmiRegisterAddress(firstsw, lecsAddr);
}

/*
** complete address DE-registration procedure
*/
static void deRegisterLecsNsap (LecsAddress *lecsAddr)
{
    idbtype *firstsw;

    firstsw = firstsw_or_null(lecsAddr->lecsInfo->hwidb);
    if (firstsw == NULL) LECS_CRASH();
    if (! (lecsAddr->addrFlags & ACTUAL_VALUE_VALID)) {
	LECS_DBG(lecsAddr->lecsInfo->name, 
	     "deRegisterLecsNsap: actual address was NEVER set");
	return;
    }

    /* just a notification */
    if (lecsAddressIsWKA(lecsAddr)) {
	if (lecsAddr->sigState != DEREGISTERED) {
	    LECS_INFO("%s: stopped listening on "
		"the well known LECS address",
		lecsAddr->lecsInfo->name);
	}
    }

    signallingDeRegisterAddress(firstsw, lecsAddr);
    ilmiDeRegisterAddress(firstsw, lecsAddr);
}

/*
** this is called when the LECS is notified by
** the signalling that a specific call has been
** completely released.  This event was beyond
** our control.
*/
static void cleanUpSvcResources (idbtype *swidb, void *callId, 
    vcd_t vcd)
{
    LecsInfo *pinfo;
    
    pinfo = getLecsPointer(swidb);
    if (pinfo == NULL) {
	LECS_ERR("cleanUpSvcResources callId 0x%x: "
	    "getLecsPointer failed", callId);
	return;
    }
    ConnList_destroyByCallId(pinfo, callId, vcd, DO_NOT_RELEASE);
    LECS_DBG(swidb->namestring, "call 0x%x cleaned up", callId);
}

/*
** this routine finds if there is at least one LECS address
** that is both registered with signalling and has its actual
** value set properly, in the list of all the LECS addresses.
*/
static boolean LecsAddress_atLeastOneSearchable (LecsInfo *pinfo)
{
    register LecsAddress *paddr;

    paddr = List_head(pinfo->lecsAddressesList);
    while (paddr) {
	if ((paddr->sigState == REGISTERED) &&
	    (paddr->addrFlags & ACTUAL_VALUE_VALID))
		return TRUE;
	paddr = List_next(paddr);
    }
    return FALSE;
}

/*
** this routine tries to find the called party address (destination)
** in the list of lecs nsap addresses which are currently registered
** with the signalling subsystem.
**
** Note that ONLY the addresses registered with signalling should
** be searched.  And note that what is being searched is the CURRENT
** value, composed of the CURRENT prefix, mac address etc.
*/
LecsAddress *searchLecsNsapActualValues (LecsInfo *pinfo, 
    uchar *addrDataBytes)
{
    register LecsAddress *paddr;

    paddr = List_head(pinfo->lecsAddressesList);
    while (paddr != NULL) {
	/*
	** check only addresses that are registered with signalling 
	** AND whose current actual values are set
	*/
	if ((paddr->sigState == REGISTERED) &&
	   (paddr->addrFlags & ACTUAL_VALUE_VALID)) {
	    if (byteStreamCompare
		(addrDataBytes, paddr->actualValue.addr, 
		 STATIONLEN_ATMNSAP) == 0) {
		return paddr;
	    }
	}
	paddr = List_next(paddr);
    }
    return NULL;
}

/*
** return the rank of "any" specified LECS address.
** if NOT found, return NO_INDEX.
*/
static int indexOfLecs (LecsInfo *pinfo, hwaddrtype *hwaddr)
{
    int i;

    for (i=0; i<pinfo->globalLecsCount; i++) {
	if (byteStreamCompare
		(pinfo->globalLecsTable [i].hwaddr.addr,
		 hwaddr->addr,
		 STATIONLEN_ATMNSAP) == 0)
		    return i;
    }
    return NO_INDEX;
}

/*
** This is used when we are completely going down,
** tear up all the LECS connections we may have.  
** These can be either incoming or outgoing calls.
*/
static void tearDownLecsConnections (LecsInfo *pinfo)
{
    int i;

    LECS_DBG(pinfo->name,
	"All calls to/from LECSs are being released");
    for (i=0; i<pinfo->globalLecsCount; i++)
	disconnect_LECS(pinfo, &pinfo->globalLecsTable [i]);
}

/*
** This routine first checks if another call from the
** same party already exists and tears it down if so,
** and secondly, determines the type of the calling 
** party in terms of whether it is an LECS, LES or another 
** entity.  It returns that type.  If the caller was an LES, then
** the routine sets the "ppsaa" to the corresponding server 
** atm address.  Alternatively, if the caller is an LECS, then 
** it sets the "index" to the rank of the LECS which is 
** originating the call.  This routine also returns the
** calling party address in the passed parameter "callingParty".
*/
static CallOriginatorType 
processCallingPartyAddress (LecsInfo *pInfo,
    sig_api_setup *setupInd, ServerAtmAddress **ppsaa, int *index,
    hwaddrtype *callingParty)
{
    char *ifname = pInfo->name;
    int idx;
    Connection *pconn = NULL;

    /* initialize */
    *ppsaa = NULL;
    *index = NO_INDEX;
    
    /* get the calling party address from the setup indication */
    atmSig_api_callId2CallingPartyAddress(setupInd->call_id, 
	callingParty);

    LECS_DBG(ifname,
	"calling party addr %Ch (type=%u len=%u)",
	callingParty, callingParty->type, callingParty->length);

    /*
    ** if no calling party address is specified,
    ** assume that an LEC is calling.
    */
    if (!lane_addr_valid(callingParty)) {
	LECS_DBG(ifname,
	    "invalid/missing calling party IE; assuming an LEC");
	snpa_clear(callingParty);
	return CALLER_IS_LEC;
    }

    /*
    ** if a call from the same calling party already exists,
    ** do NOT accept the new call.  This maintains consistency.
    */
    pconn = ConnList_searchByCallingParty(pInfo, NULL, callingParty);
    if (pconn) {
	LECS_DBG(ifname, 
	    "a call from the same caller already exists; "
	    "new call rejected");
	return NO_CALLER;
    }

    /* see if an LECS is calling */
    idx = indexOfLecs(pInfo, callingParty);
    if (idx != NO_INDEX) {
	LECS_DBG(ifname, "caller is an LECS of rank %d", idx);
	*index = idx;
	return CALLER_IS_LECS;
    }

    /* is an LES calling ? */
    if (getElanByLesAddress(pInfo->cfgTable, callingParty, ppsaa)) {
	LECS_DBG(ifname, "caller is an LES");

	/*
	** if this LES is already connected, reject the new call.
	** This situation is not catched above in the duplicate
	** connection catching code.  This is becoz the above
	** method searches only in the connections local to the
	** single specific interface.  However, this situation 
	** may arise if a config table is bound to 2 or more 
	** interfaces (LECSs) and somehow the two networks are
	** mirror images.  Very unlikely, but NOT impossible.
	** When we have some time later, we may have to re-think
	** this whole scheme again.
	*/
	if ((*ppsaa)->connPtr) {
	    LECS_DBG(ifname, 
		"this LES is already connected from port %s",
		(*ppsaa)->connPtr->lecsInfo->name);
	    return NO_CALLER;
	}

	return CALLER_IS_LES;
    }

    /* if we are here, we dont know the caller; assume a LEC */
    LECS_DBG(ifname, "calling party is probably an LEC");
    return CALLER_IS_LEC;
}

/*
** process an incoming call connection request.
** check everything, if all is well then
** store the call information and acknowledge
** acceptance to the signalling.  Otherwise,
** reject the call.
*/
static void processSetupIndication (sig_api_setup *setupInd, 
    idbtype *idb)
{
    sig_api_msg msg;
    Connection *pconn = NULL;
    Connection *oldConn;
    LecsInfo *pinfo;
    LecsAddress *pCalledParty;
    char *ifname = idb->namestring;
    CallOriginatorType callerType;
    ServerAtmAddress *psaa = NULL;
    vcd_t vcd = setupInd->vcd;
    int index = NO_INDEX;
    int localRank;
    void *callId = setupInd->call_id;
    hwaddrtype callingPartyAddress;
    ushort sdu_size;


    /* obtain the "LecsInfo" object to which this call applies */
    pinfo = getLecsPointer(idb);

    /* this should NOT be NULL */
    if (!pinfo) {
	LECS_ERR("processSetupIndication: NULL pinfo");
	lecsRelease(FALSE, callId, ATM_TEMP_FAILURE, idb);
	return;
    }

    /* vcd validity check */
    if (invalid_vcd(vcd)) {
	LECS_ERR("processSetupIndication: invalid vcd %u", vcd);
	lecsRelease(FALSE, callId, ATM_TEMP_FAILURE, idb);
	return;
    }
    
    /* check that the called party ie is indeed present */
    if (setupInd->called_party.length != STATIONLEN_ATMNSAP) {
	LECS_ERR("processSetupIndication: "
	    "invalid called party address length %d",
	    setupInd->called_party.length);
	lecsRelease(FALSE, callId, ATM_TEMP_FAILURE, idb);
	return;
    }
    
    /* make sure that the call id is not already used */
    oldConn = ConnList_searchByCallId(pinfo, callId, vcd);
    if (oldConn) {
	/* 
	** if we are here something is badly screwed either with 
	** LECS or signalling.  Clean things up a little in the
	** hope of recovering.
	*/
	LECS_ERR("processSetupIndication: "
	    "duplicate call id 0x%x at vcd %u", callId, vcd);

	/* release the new call explicitly if they are different */
	if (oldConn->callId != callId)
	    lecsRelease(FALSE, callId, ATM_TEMP_FAILURE, idb);

	/* now release the old call "stuck" in the LECS */
	ConnList_destroyByPtr(pinfo, oldConn, DO_RELEASE);
	return;
    }

    /*
    ** this COULD be NULL if we just disassociated the config
    ** table from the interface while there were still pending
    ** incoming calls to the LECS.
    */
    if (!pinfo->cfgTable) {
	LECS_DBG(ifname,
	    "just disassociated the config table");
	lecsRelease(FALSE, callId, ATM_DEST_OUT_OF_ORDER, idb);
	return;
    }

    /* should NOT accept a multipoint call */
    if (setupInd->multipoint) {
	LECS_DBG(ifname, "multipoint connection request 0x%x", 
	    callId);
	lecsRelease(FALSE, callId, ATM_INCOMPAT_DEST, idb);
	return;
    }

    /* check that the blli is acceptable */
    if (! lane_sig_blli_match 
	   (ATM_MAX_BLLI_NUM, &setupInd->blli [0], LANE_CONFIGURE)) {
	LECS_DBG(ifname, "incorrect blli on callid 0x%x", callId);
	lecsRelease(FALSE, callId, ATM_INCOMPAT_DEST, idb);
	return;
    }
    sdu_size = LANE_SDU_SIZE_1516;
    /*
     * reject call SETUP, if SDU size is not 1516, if so configured.
     */
    if ((sdu_size != setupInd->fwd_sdu_size) ||
	(sdu_size != setupInd->bwd_sdu_size)) {
	if (idb->hwptr->atm_db->mtu_reject_call) {
	    LECS_ERR("Invalid SDU size in incoming setup"
		     "\nCall from ATM NSAP: %Ch" 
		     "\nreceived forward SDU size is %u"
		     "\nreceived backward SDU size is %u" 
		     "\nSDU size on the interface %s is %d"
		     "\nCall Rejected",
		     &setupInd->calling_party,
		     setupInd->fwd_sdu_size,
		     setupInd->bwd_sdu_size,
		     idb->namestring, sdu_size);
	    lecsRelease(TRUE, callId, ATM_AAL_PARAMS_NOT_SUPP, idb);
	    return;
 
	}
    }

    /* find the lecs nsap address this call is intended for */
    pCalledParty = searchLecsNsapActualValues(pinfo, 
			setupInd->called_party.addr);
    if (pCalledParty == NULL) {
	LECS_ERR("processSetupIndication: "
	    "call received for an UN registered address");
	lecsRelease(FALSE, callId, ATM_INVALID_ENDPOINT, idb);
	return;
    }

    /* 
    ** now try and categorize the caller by 
    ** analyzing the calling party address
    ** information element.
    */
    callerType = processCallingPartyAddress(pinfo, setupInd, 
		    &psaa, &index, &callingPartyAddress);

    /*
    ** currently this can happen only if more than one
    ** LECS (interface) shares the same config table and
    ** an LES appears to be trying to connect from multiple 
    ** different ports.  In this case, we reject the call.
    */
    if (callerType == NO_CALLER) {
	lecsRelease(FALSE, callId, ATM_USER_BUSY, idb);
	return;
    }

    /* if the caller is an LECS, check its relative "rank" */
    if (callerType == CALLER_IS_LECS) {
	localRank = myHighestRank(pinfo, NULL);
	if (index >= localRank) {
	    LECS_WARN("interface %s: inferior LECS %Ch "
		"is trying to connect up to me; "
		"global LECS addresses configuration error",
		pinfo->name, &setupInd->calling_party);
	    lecsRelease(FALSE, callId, ATM_INVALID_ENDPOINT, idb);
	    return;
	}
	LECS_DBG(pinfo->name,
	    "higher LECS %Ch (rank %d) connecting to me (%d)",
	    &setupInd->calling_party, index, localRank);
    } else if (! pinfo->thisLecsIsTheMaster) {
	/* 
	** someone OTHER than an LECS is trying to connect
	** to me.  I can allow that ONLY if I am the master.
	*/
	LECS_DBG(ifname,
	    "NON LECS connecting to a slave LECS, "
	    "call 0x%x being rejected",
	    callId);
	lecsRelease(FALSE, callId, ATM_RES_UNAVAIL, idb);
	return;
    }

    /* if we made it here, this is a valid connection request */

    /* store the connexion details in a "Connection" object. */
    pconn = Connection_alloc(pinfo);
    if (pconn == NULL) {
	LECS_ERR("processSetupIndication: Connection_alloc failed;"
	    " call rejected");
	lecsRelease(FALSE, callId, ATM_RES_UNAVAIL, idb);
	return;
    }
    pconn->callId = callId;
    pconn->vcd = vcd;
    pconn->calledParty = pCalledParty;
    pconn->callingParty = callingPartyAddress;
    pconn->callerType = callerType;
    pconn->lesAddress = psaa;
    pconn->index = index;

    /* record this call in the connexions [] array entry */
    pinfo->connexions [vcd] = pconn;

    /* now accept the connection: send an ATM_CONNECT back */
    msg.opcode = ATM_CONNECT;
    msg.u.connect.call_id = callId;
    msg.u.connect.vcd = vcd;
    msg.u.connect.flags = (ATM_VC_AAL5LANE | VC_FLAG_NOIDLETIME |
			   VC_FLAG_NOBUMP_CACHE);
    msg.u.connect.user_vc_type = LANE_CONFIGURE;
    msg.u.connect.blli.length = 0;
    bzero(msg.u.connect.blli.string, ATM_BBAND_LOLAYER_MAXSIZE);
    atmSig_handleUserRequest(&msg);
    LECS_DBG(ifname, 
	"call ACCEPTED and sent a CONNECT to 0x%x", callId);

    /* additional special case stuff we have to do */
    switch (callerType) {
	case CALLER_IS_LES:
	    process_LES_connected(pinfo, pconn, psaa);
	    break;
	case CALLER_IS_LECS:
	    process_higher_LECS_connected(pinfo, pconn, index);
	    break;
	default:;
    }
}

/*
** OVERALL signalling callback handler
*/
static void lecsSignallingCB (void *voidp)
{
    sig_api_msg *msg = (sig_api_msg*) voidp;
    idbtype *idb = msg->idb;
    char *ifname = idb->namestring;
    char *opcodeStr = atmSig_printOpcode(msg->opcode);

    if (msg == NULL) {
	LECS_ERR("lecsSignallingCB: atm signalling subsystem "
	    "error: NULL message passed up from signalling");
	return;
    }
    switch (msg->opcode) {
	case ATM_LISTEN_NSAP:
	case ATM_CANCEL_NSAP:
	    atmsigListenCancelCB(msg);
	    break;

	/* the OTHER end wants to establish a connection to us */
	case ATM_SETUP:
	    LECS_DBG(ifname, "%s received: callid 0x%x",
		opcodeStr, msg->u.setup.call_id);

	    /* 
	    ** analyze everything and accept if ok, or reject if not.
	    ** If we accepted, all the data structures are created.
	    ** This may seem a bit premature since we really should
	    ** wait for the connect ack to come back, but the extra
	    ** work needed to salt away all the information and deal 
	    ** with these intermediate states is not worth the effort.
	    ** Besides, it is extremely unlikely that a connection
	    ** will NOT succeed after we have accepted it anyway.  
	    ** Even if it does occur, the conclusive RELEASE will 
	    ** be received and everything will be cleared up anyway.
	    */
	    processSetupIndication(&msg->u.setup, msg->idb);
	    break;

	/*
	** we must have caused this by accepting a setup request 
	** (by sending a connect back), and as a result, the connect 
	** ack came back to us as the connection confirmation.  
	** But since we allocated all the stuff during the setup 
	** indication phase, no need to do anything with this one.
	*/
	case ATM_CONNECT_ACK:
	    LECS_DBG(ifname, "%s received: callid 0x%x",
		opcodeStr, msg->u.connect_ack.call_id);
	    break;

	/* the OTHER end is tearing down an ESTABLIHED connection */
	case ATM_RELEASE:
	    LECS_DBG(ifname, "%s received: callid 0x%x cause %d",
		opcodeStr, msg->u.release.call_id, 
		msg->u.release.cause);
	    
	    /* send a release_complete to acknowledge */
	    lecsRelease(TRUE, msg->u.release.call_id, 
		ATM_NORMAL_UNSPECIFIED, idb);

	    /* clean up our local data structures */
	    cleanUpSvcResources(msg->idb, 
		msg->u.release.call_id, IGNORE_VCD);

	    break;

	/*
	** either we broke a connection before and this is the
	** confirmation OR a fault occured in the network.
	** In any case, just clear the data structures.  In the case
	** where we previously already cleared the data structures,
	** this causes no harm anyway.
	*/
	case ATM_RELEASE_COMPLETE:
	    LECS_DBG(ifname,
		"%s received: callId 0x%x cause %d",
		opcodeStr, msg->u.release_comp.call_id,
		msg->u.release_comp.cause);

	    /* clean up our local data structures */
	    cleanUpSvcResources(msg->idb, 
		msg->u.release_comp.call_id, IGNORE_VCD);
	    break;

	/* nothing else is acceptable */
	default:
	    LECS_ERR("lecsSignallingCB: unexpected message %d (%s)"
		"from signalling subsystem", 
		msg->opcode, opcodeStr);
    }
}

/******************************************************************
*******************************************************************
**                      ConfigTable object
*******************************************************************
******************************************************************/

/*
** used in searching an elan by name in the config table
*/
static int compareElanNames (void *key, void *elemptr)
{ 
    return 
	strncmp ((char*) key, ((ElanInfoBlock*) elemptr)->elanName, 
	    NAMESIZE); 
}

/*
** used in searching a server atm address (for an elan)
** by the fully specified atm address, in the config table.
*/
static int compareServerAtmAddresses (void *key, void *elemptr)
{
    return
	byteStreamCompare
	    (((hwaddrtype*) key)->addr,
	     ((ServerAtmAddress*)elemptr)->lesAddr.addr,
	     STATIONLEN_ATMNSAP);
}

/*
** Used from parser AND CONFIG REQUESTS.
**
** Compare MAC addresses; key is a masked address.
*/
static int compareLECsByMacAddress (void *key, void *elemptr)
{
    return
	byteStreamCompare 
	   ((uchar*) key, 
	    ((LecInfoBlock*) elemptr)->address.dataBytes,
	     STATIONLEN_IEEE48);
}

/*
** Used from parser AND CONFIG REQUESTS for UNMASKED nsaps only.
**
** Compare ATM addresses with data ONLY
*/
static int compareLECsByAtmAddress (void *key, void *elemptr)
{
    return
	byteStreamCompare 
	   ((uchar*) key,
	    ((LecInfoBlock*) elemptr)->address.dataBytes, 
	     STATIONLEN_ATMNSAP);
}

/*
** Used from parser AND CONFIG REQUESTS.
**
** Compare Route Descriptors: key is the pointer to the index
*/
static int compareLECsByRouteDesc (void *key, void *elemptr)
{
    uchar *key_ptr = (uchar*) key;
    MaskedAddress *new_addr = &((LecInfoBlock*)elemptr)->address;

    /* first byte */
    if (*key_ptr == new_addr->dataBytes [0]) {  
	/* second byte */
	if (*(key_ptr+1) == new_addr->dataBytes [1])
	    return 0;
	else if (*(key_ptr+1) < new_addr->dataBytes [1])
	    return (-1);
	else
	    return 1;
    } else {
	if (*key_ptr < new_addr->dataBytes [0])
	    return (-1);
	else
	    return 1;
    }
}

/*
** Used from parser only.
**
** Compare LEC's precisely to match BOTH in data and mask EXACTLY.
** Used in linear searches in the masked atm address index, to find
** duplicate entries while typing them in from the parser.
*/
static boolean absolutelyIdenticalLECnsaps (void *key, void *elemPtr)
{
    return 
	maskedAddressCompare 
	   ((MaskedAddress*) key, 
	    &((LecInfoBlock*) elemPtr)->address);
}

/*
** Used for CONFIG REQUESTS with the MASKED index.
**
** similar to above, but for atm addresses.  It uses
** a mask of all ones for the FIRST atm address's data
** bytes(specified by "key").
*/
static boolean compareLECsUsingMask (void *key, void *elemptr)
{
    return 
	maskedByteStreamCompare
	   ((uchar*) key, 
	    ((LecInfoBlock*) elemptr)->address.dataBytes, 
	    ((LecInfoBlock*) elemptr)->address.maskBytes, 
	     STATIONLEN_ATMNSAP);
}

/*
** delete a LEC element given the LEC info pointer.
** take it off all the indexes, and reclaim its space.
** Note that if the LEC was represented by a masked
** address, it was NOT in an index but was in the
** linear array.  So delete it from there.
*/
static boolean ConfigTable_destroyLecByPtr (ConfigTable *pCfg, 
    LecInfoBlock *pLec)
{
    if (pLec == NULL) LECS_CRASH();

    /* delete reference to it first */
    switch (pLec->address.type) {
	case STATION_ATMNSAP:
	    if (allOnesMask(pLec->address.maskBytes, 
		STATIONLEN_ATMNSAP))
		    Index_remove 
			(pCfg->UNmaskedAtmAddressIndex, 
			 pLec->address.dataBytes);
	    else
		Index_remove(pCfg->maskedAtmAddressIndex, pLec);
	    break;

	case STATION_IEEE48:
	    Index_remove(pCfg->macAddressIndex, 
		pLec->address.dataBytes);
	    break;

	case STATION_ROUTEDESC:
	    Index_remove(pCfg->rdIndex, pLec->address.dataBytes);
	    break;

	default:
	    LECS_CRASH();
    }
    PRINTF("LEC %Cw in elan '%s' is deleted\n", 
	&pLec->address, pLec->parentElan->elanName);

    /* now remove its storage */
    chunk_free(pCfg->lecTable, pLec);

    return TRUE;
}

/*
** finds a lec in the config table, either from the unmasked
** unique address index OR the masked storage area.
*/
static LecInfoBlock *ConfigTable_findLECByAtmAddr (ConfigTable *pCfg, 
    MaskedAddress *maddr)
{
    LecInfoBlock *pLec;

    if (allOnesMask(maddr->maskBytes, STATIONLEN_ATMNSAP))
    {
	return 
	    Index_searchElement 
		(pCfg->UNmaskedAtmAddressIndex, 
		 maddr->dataBytes, NULL);
    } else {
	Index_linearSearch 
	   (pCfg->maskedAtmAddressIndex, 0, maddr, 
	    (CMP) absolutelyIdenticalLECnsaps, &pLec);
	return pLec;
    }
}

/*
** destroy an LEC by its atm address
*/
void ConfigTable_destroyLecByAtmAddr (ConfigTable *pCfg, 
    MaskedAddress *atmAddrPtr)
{
    LecInfoBlock *pLec;

    pLec = ConfigTable_findLECByAtmAddr(pCfg, atmAddrPtr);
    if (pLec == NULL) {
	USER_ERROR("%% no such LANE Client exists\n");
	return;
    }
    ConfigTable_destroyLecByPtr(pCfg, pLec);
}

/*
** destroy an LEC by its mac address
*/
void ConfigTable_destroyLecByMacAddr (ConfigTable *pCfg, 
    MaskedAddress *macAddrPtr)
{
    LecInfoBlock *pLec;

    pLec = Index_searchElement 
		(pCfg->macAddressIndex, macAddrPtr->dataBytes, NULL);
    if (pLec == NULL) {
	USER_ERROR("%% no such LANE Client exists\n");
	return;
    }
    ConfigTable_destroyLecByPtr(pCfg, pLec);
}

/*
** destroy an LEC by its route descriptor in a given ELAN
*/
void ConfigTable_destroyLecByRouteDesc (ConfigTable *pCfg,
    char *elanName, MaskedAddress *rd_ptr)
{
    LecInfoBlock *pLec;
    ElanInfoBlock *pElan;

    pLec = Index_searchElement(pCfg->rdIndex, rd_ptr->dataBytes, 
	    NULL);
    if (pLec == NULL) {
	USER_ERROR("%% no such LANE Client exists\n");
	return;
    }
    pElan = pLec->parentElan;
    if (byteStreamCompare(elanName, pElan->elanName, 
	pElan->elanNameSize) == 0) {
	    ConfigTable_destroyLecByPtr(pCfg, pLec);
    } else {
	USER_ERROR("%% no such LANE Client exists in elan '%s'\n", 
	    elanName);
    }
}

/*
** deletes all the LEC's that belong to the specified elan.  If
** the elan pointer is NULL, then deletes ALL the LEC's in the
** config table.  The first form is used when a particular elan
** in a config table is deleted, whereas the second form is useful 
** when an entire config table is being deleted.
*/
static void ConfigTable_destroyLECsInElan (ConfigTable *pCfg, 
    ElanInfoBlock *pElan)
{
    int n;
    LecInfoBlock *pLec;
    
    /*
    ** go thru each LEC and delete the ones whose 
    ** "parentElan" field matches the specified elan.
    */

    /*
    ** in searching thru the indexes, perform a reverse
    ** traversal since we are deleting from the index which
    ** changes the size as it is being deleted.  The loop
    ** basically always obtains the last element in the index.
    ** This is faster AND does NOT confuse the count.
    */

/* go thru the LECs indexed by the UNMASKED nsap address */
    n = Index_numberOfElements(pCfg->UNmaskedAtmAddressIndex) - 1;
    while ((pLec = Index_getElementPointer
		(pCfg->UNmaskedAtmAddressIndex, n)) != NULL) {
	if ((pElan == NULL) || (pLec->parentElan == pElan))
	    ConfigTable_destroyLecByPtr(pCfg, pLec);
	n--;
    }

/* go thru the LECs indexed by the MASKED nsap address */
    n = Index_numberOfElements(pCfg->maskedAtmAddressIndex) - 1;
    while ((pLec = Index_getElementPointer
		(pCfg->maskedAtmAddressIndex, n)) != NULL) {
	if ((pElan == NULL) || (pLec->parentElan == pElan))
	    ConfigTable_destroyLecByPtr(pCfg, pLec);
	n--;
    }

/* now go thru the LECs indexed by the mac address */
    n = Index_numberOfElements(pCfg->macAddressIndex) - 1;
    while ((pLec = Index_getElementPointer
		(pCfg->macAddressIndex, n)) != NULL) {
	if ((pElan == NULL) || (pLec->parentElan == pElan))
	    ConfigTable_destroyLecByPtr(pCfg, pLec);
	n--;
    }

/* now go thru the LECs indexed by the route descriptors */
    n = Index_numberOfElements(pCfg->rdIndex) - 1;
    while ((pLec = Index_getElementPointer
		(pCfg->rdIndex, n)) != NULL) {
	if ((pElan == NULL) || (pLec->parentElan == pElan))
	    ConfigTable_destroyLecByPtr(pCfg, pLec);
	n--;
    }

}

/*
** lock an lecs interface, while we are getting
** the LECS addresses from the ILMI.
*/
static void lecsLockInterface (LecsInfo *pinfo)
{
    if (pinfo->locked) return;
    pinfo->locked = TRUE;
    LECS_DBG(pinfo->name, "lecs interface locked");
}

/*
** opposite of the above
*/
static void lecsUnlockInterface (LecsInfo *pinfo)
{
    if (! pinfo->locked) return;
    pinfo->locked = FALSE;
    LECS_DBG(pinfo->name, "lecs interface un-locked");
}

/*
** destroy an elan.  This causes:
**
**	a) All LEC members of this particular ELAN to be destroyed.
**	b) Elan itself gets destroyed
*/
static boolean ConfigTable_destroyElanByPtr (ConfigTable *pCfg, 
    ElanInfoBlock *pElan)
{
    ServerAtmAddress *psaa, *deleter;

    if (pElan == NULL) LECS_CRASH();

    /* destroy all LEC's in this elan */
    ConfigTable_destroyLECsInElan(pCfg, pElan);

    /* if this elan was the default, clear the default */
    if (pElan == pCfg->defaultElan) {
	PRINTF("elan '%s' was default, hence default deleted\n",
		    pElan->elanName);
	pCfg->defaultElan = NULL;
    }

    /*
    ** destroy all the server atm addresses and 
    ** their container list object from this elan.
    */
    psaa = List_head(pElan->lesAddressesList);
    while (psaa != NULL) {
	deleter = psaa;
	psaa = List_next(psaa);
	serverAtmAddress_destroy(pElan, deleter);
    }
    if (List_size(pElan->lesAddressesList) != 0) LECS_CRASH();
    List_destroy(pElan->lesAddressesList);

    /* take its name off the elan name index */
    if (Index_remove(pCfg->elanIndexByName, pElan->elanName) != pElan)
	LECS_CRASH();

    /* clean up */
    List_remove(pCfg->elanList, pElan);
    PRINTF("elan '%s' is deleted\n", pElan->elanName);
    free(pElan);

    return TRUE;
}

ConfigTable *ConfigTable_alloc (char *name)
{
    ConfigTable *pCfg;

    if ((name == NULL) || (name [0] == 0)) LECS_CRASH();
    pCfg = malloc_named(sizeof(ConfigTable), "ConfigTable");
    if (pCfg != NULL) {
	/* allocate lec storage area, and indexes */
	pCfg->lecTable = chunk_create(sizeof(LecInfoBlock), 64, 
				      CHUNK_FLAGS_DYNAMIC,
				      NULL, 0,
				      "lecTable");
	if (pCfg->lecTable == NULL) {
	    LECS_ERR("ConfigTable_alloc: lecTable failed");
	    free(pCfg);
	    return NULL;
	}
	pCfg->UNmaskedAtmAddressIndex =
	    Index_alloc((CMP) compareLECsByAtmAddress, 0);
	if (pCfg->UNmaskedAtmAddressIndex == NULL) {
	    LECS_ERR("ConfigTable_alloc: "
		"UNmaskedAtmAddressIndex failed");
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}
	pCfg->macAddressIndex =
	    Index_alloc((CMP) compareLECsByMacAddress, 0);
	if (pCfg->macAddressIndex == NULL) {
	    LECS_ERR("ConfigTable_alloc: macAddressIndex failed");
	    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}
	pCfg->maskedAtmAddressIndex = Ulong_Index_alloc();
	if (pCfg->maskedAtmAddressIndex == NULL) {
	    LECS_ERR("ConfigTable_alloc: "
		"maskedAtmAddressIndex failed");
	    Index_destroy(pCfg->macAddressIndex);
	    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}
	pCfg->rdIndex = Index_alloc((CMP) compareLECsByRouteDesc, 0);
	if (pCfg->rdIndex == NULL) {
	    LECS_ERR("ConfigTable_alloc: rdIndex failed");
	    Index_destroy(pCfg->maskedAtmAddressIndex);
	    Index_destroy(pCfg->macAddressIndex);
	    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}
	pCfg->elanList = List_alloc();
	if (pCfg->elanList == NULL) {
	    LECS_ERR("ConfigTable_alloc: elanList failed");
            Index_destroy(pCfg->rdIndex);
	    Index_destroy(pCfg->maskedAtmAddressIndex);
	    Index_destroy(pCfg->macAddressIndex);
	    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}
	pCfg->elanIndexByName = 
	    Index_alloc((CMP) compareElanNames, 0);
	if (pCfg->elanIndexByName == NULL) {
	    LECS_ERR("ConfigTable_alloc: elanIndexByName failed");
	    List_destroy(pCfg->elanList);
            Index_destroy(pCfg->rdIndex);
	    Index_destroy(pCfg->maskedAtmAddressIndex);
	    Index_destroy(pCfg->macAddressIndex);
	    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}
	pCfg->serverAtmAddressesIndex =
	    Index_alloc((CMP) compareServerAtmAddresses, 0);
	if (pCfg->serverAtmAddressesIndex == NULL) {
	    LECS_ERR("ConfigTable_alloc: "
		"serverAtmAddressesIndex failed");
	    Index_destroy(pCfg->elanIndexByName);
	    List_destroy(pCfg->elanList);
            Index_destroy(pCfg->rdIndex);
	    Index_destroy(pCfg->maskedAtmAddressIndex);
	    Index_destroy(pCfg->macAddressIndex);
	    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
	    chunk_destroy(pCfg->lecTable);
	    free(pCfg);
	    return NULL;
	}

	/* everything is ok, so fill everything else in */
	List_clearLinks(pCfg);
	pCfg->bindCount = 0;
	pCfg->defaultElan = NULL;
	pCfg->locked = FALSE;
	pCfg->tableName [0] = 0;
	sstrncpy(pCfg->tableName, name, NAMESIZE);

	/* enter it into the global list */
	List_write(G_ConfigTablesList, pCfg);
    }
    return pCfg;
}

/*
** deallocate a config table object.
*/
static void ConfigTable_dealloc (ConfigTable *pCfg)
{
    ElanInfoBlock *pElan;

    if (pCfg == NULL) return;

    /*
    ** destroy ALL the LEC's in the config table.
    ** the NULL elan pointer ensures that
    */
    ConfigTable_destroyLECsInElan(pCfg, NULL);

    /* 
    ** now destroy the elans.  At this point 
    ** there should be NO LEC's left 
    */
    while ((pElan = List_head(pCfg->elanList)) != NULL)
	ConfigTable_destroyElanByPtr(pCfg, pElan);
    List_destroy(pCfg->elanList);

    /* all the indexes must have been emptied out */
    if (Index_numberOfElements(pCfg->rdIndex) != 0)
	LECS_CRASH();
    if (Index_numberOfElements(pCfg->UNmaskedAtmAddressIndex) != 0)
	LECS_CRASH();
    if (Index_numberOfElements(pCfg->macAddressIndex) != 0)
	LECS_CRASH();
    if (Index_numberOfElements(pCfg->maskedAtmAddressIndex) != 0)
	LECS_CRASH();
    if (Index_numberOfElements(pCfg->elanIndexByName) != 0)
	LECS_CRASH();
    if (Index_numberOfElements(pCfg->serverAtmAddressesIndex) != 0)
	LECS_CRASH();

    /* destroy the lec table & indexes */
    chunk_destroy(pCfg->lecTable);
    Index_destroy(pCfg->UNmaskedAtmAddressIndex);
    Index_destroy(pCfg->macAddressIndex);
    Index_destroy(pCfg->maskedAtmAddressIndex);
    Index_destroy(pCfg->elanIndexByName);
    Index_destroy(pCfg->serverAtmAddressesIndex);
    Index_destroy(pCfg->rdIndex); 

    /* take it off the global list */
    List_remove(G_ConfigTablesList, pCfg);
    List_clearLinks(pCfg);

    /* now delete the object itself */
    free(pCfg);
}

/*
** destroy a ConfigTable object by pointer
*/
boolean ConfigTable_destroyByPtr (ConfigTable *pCfg)
{
    LecsInfo *pinfo;

    if (! pCfg) LECS_CRASH();

    /*
    ** any LecsInfo object referencing this config
    ** table should have its config table pointer cleared.
    */
    pinfo = List_head(G_LecsInfosList);
    while (pinfo != NULL) {

	if (pinfo->cfgTable == pCfg) pinfo->cfgTable = NULL;
	pinfo = List_next(pinfo);
    }
    ConfigTable_dealloc(pCfg);

    /* since many LECSs may have been affected, check them ALL */
    revisitLecs(NULL);

    return TRUE;
}

/*
** this is a linear search since there are NOT likely to
** be too many config database tables in the router
*/
ConfigTable *getConfigTableByName (char *name)
{
    ConfigTable *pCfg;

    if (! name || (name [0] == 0)) LECS_CRASH();
    pCfg = List_head(G_ConfigTablesList);
    while (pCfg != NULL) {
	if (strncmp(name, pCfg->tableName, NAMESIZE-1) == 0) 
	    return pCfg;
	pCfg = List_next(pCfg);
    }
    return NULL;
}

/*
** search for a particular elan by name.  This is linear since
** it is very unlikely to have a lot of these in one config table.
*/
ElanInfoBlock *getElanByName (ConfigTable *pCfg, 
    char *elanName, int elanNameSize)
{
    if (! pCfg || ! elanName || (elanNameSize <= 0))
	LECS_CRASH();

    /*
    ** do this so that the search from a config request works
    ** properly.  The elan name in a config request is NOT
    ** NULL terminated.
    */
    elanName [elanNameSize] = 0;
    return 
	(ElanInfoBlock*)
	    Index_searchElement
		(pCfg->elanIndexByName, elanName, NULL);
}

/*
** search the elan based on its LES nsap address.  
** Returns NULL if no elan is associated
** with this address.  Note that, if found, psaa is also set to
** the server atm address.
*/
static ElanInfoBlock *getElanByLesAddress (ConfigTable *pCfg, 
    hwaddrtype *lesAddr, ServerAtmAddress **psaa)
{
    ServerAtmAddress *saa;

    /* note that "psaa" CAN be NULL */
    if (! pCfg || ! lesAddr) LECS_CRASH();

    /* search the server atm address */
    saa = (ServerAtmAddress*)
	Index_searchElement
	    (pCfg->serverAtmAddressesIndex, lesAddr, NULL);

    /* not found an elan with this server atm address */
    if (saa == NULL) return NULL;

    /* if we are here, it is found */
    if (psaa) *psaa = saa;
    return saa->elanPtr;
}

/*
** convert the tri-valued parser variable into
** a boolean that can be used with "elan->restricted" field
*/
static boolean restricted2bool (ElanRestrictedStatus elanStatus)
{
    switch (elanStatus) {
	case ELAN_NOT_RESTRICTED: return FALSE;
	case ELAN_RESTRICTED: return TRUE;
	default: LECS_CRASH();
    }
    return TRUE;
}

/*
** set an elan's restricted status.
** Check and make sure that the default elan can NOT
** be made restricted.  This can be called either 
** from the parser (with the name) or internally
** (with the elan pointer) for flexibility.
*/
void setElanRestrictedStatus (ConfigTable *pCfg, char *name, 
    ElanInfoBlock *pElan, ElanRestrictedStatus elanStatus)
{
    boolean restricted;

    if (name != NULL)
	pElan = getElanByName(pCfg, name, strlen(name));

    if (pElan == NULL) {
	USER_ERROR("%% specified elan does not exist\n");
	return;
    }

    if (elanStatus == ELAN_NO_CHANGE) return;
    restricted = restricted2bool(elanStatus);
    
    /* no real change */
    if (pElan->restricted == restricted) return;

    /* default elan can NOT be made restricted */
    if ((pCfg->defaultElan == pElan) && restricted) {
	USER_ERROR("%% this is the default elan ");
	USER_ERROR("and cannot be made restricted\n");
	return;
    }

    pElan->restricted = restricted;
    PRINTF("elan '%s' is made %srestricted\n", 
	pElan->elanName, restricted ? "" : "UN-");
}

/*
** set the elan les disconnect grace timeout period
*/
void elanSetLesTimeout (ConfigTable *pCfg, char *name, 
    int lesTimeout)
{
    ElanInfoBlock *pElan;

    pElan = getElanByName(pCfg, name, strlen(name));
    if (pElan == NULL) {
	USER_ERROR("%% specified elan does not exist\n");
	return;
    }
    if (lesTimeout < 0) {
	USER_ERROR("%% negative timeout value is defaulted to 0\n");
	lesTimeout = 0;
    }
    pElan->lesTimeout = lesTimeout;
    PRINTF("elan '%s' les timeout set to %d seconds\n",
	name, lesTimeout);
}

/*
** renames an existing elan.  Note that
** if an elan is renamed, the LES connection
** serving that elan MUST be broken, since it
** is now a "different" elan, by virtue of the
** fact that its name is changed.
*/
void renameElan (ConfigTable *pCfg, char *oldName, char *newName)
{
    ElanInfoBlock *pElan, *exists;
    int oldNameSize, newNameSize;

    /* no real change */
    if (oldName == newName) return;
    if (strncmp(oldName, newName, NAMESIZE) == 0) return;

    /* make sure that another one with the same name doesnt exist */
    newNameSize = strlen(newName);
    exists = getElanByName(pCfg, newName, newNameSize);
    if (exists != NULL) {
	USER_ERROR("another elan named '%s' already exists\n", 
	    newName);
	return;
    }

    /* make sure that this one exists */
    oldNameSize = strlen(oldName);
    pElan = getElanByName(pCfg, oldName, oldNameSize);
    if (pElan == NULL) {
	USER_ERROR("elan '%s' does not exist\n", oldName);
	return;
    }

    /* remove the OLD name from index */
    if (Index_remove(pCfg->elanIndexByName, oldName) != pElan)
	LECS_CRASH();

    /* copy the new name into the elan info block */
    sstrncpy(pElan->elanName, newName, NAMESIZE);
    pElan->elanNameSize = newNameSize;

    /* add the NEW name to index */
    if (Index_add(pCfg->elanIndexByName, newName, pElan) != pElan)
	LECS_ERR("renameElan: Index_add newName failed");

    /* elan name change should cause all LESs to be disconnected */
    cleanLesRedundancyInfo (pCfg, pElan);

    PRINTF("renamed elan '%s' to '%s'\n", oldName, newName);
}

/*
** is the user trying to change the priority
** of an existing server atm address ?
*/
static boolean differentPriorities (ElanInfoBlock *pElan,
    ServerAtmAddress *psaa, int priority)
{
    int lastPrio;

    lastPrio = List_size(pElan->lesAddressesList);
    
    /* adjust new priority */
    if ((priority <= LOWEST_PRIO) ||
	(priority > lastPrio))
	    priority = lastPrio;
    return psaa->priority != priority;
}

/*
** ensures that an LES address is not also a global
** LECS address, on all running interfaces to which
** this config table is bound to.  If there is indeed
** a conflict, it returns the rank of the conflicted
** LECS address, otherwise returns NO_INDEX.
*/
static int LECS_LES_conflict (ConfigTable *pCfg,
    hwaddrtype *lesAddr, LecsInfo **returned)
{
    LecsInfo *pinfo;
    int rank;

    pinfo = List_head(G_LecsInfosList);
    while (pinfo != NULL) {
	if (pinfo->cfgTable == pCfg) {
	    rank = indexOfLecs(pinfo, lesAddr);
	    if (rank >= 0) {
		*returned = pinfo;
		return rank;
	    }
	}
	pinfo = List_next(pinfo);
    }
    return NO_INDEX;
}

/*
** create/modify an ELAN.  If the elan does not exist, create
** one with the given address.  If it already exists, then
** add the server address to that elan.  
** 
** FOR BACKWARD COMPATIBILITY ONLY: In every operation,
** check whether to set/change the elan restricted status.
*/
ElanInfoBlock *ConfigTable_elanCreate (ConfigTable *pCfg, 
    char *elanName, hwaddrtype *lesAddr, int priority,
    ElanRestrictedStatus elanStatus)
{
    ElanInfoBlock *byName, *byLesAddress;
    int elanNameSize;
    ServerAtmAddress *psaa;
    int rank;
    LecsInfo *pinfo;

    if (elanName == NULL) LECS_CRASH();
    if (elanName [0] == 0) LECS_CRASH();
    if (lesAddr == NULL) LECS_CRASH();
    if (lesAddr->type != STATION_ATMNSAP) LECS_CRASH();

    /*
    ** if this config table is bound to any interfaces,
    ** then the LES address being entered should NOT
    ** also be a global LECS address on any of those
    ** interfaces.
    */
    rank = LECS_LES_conflict(pCfg, lesAddr, &pinfo);
    if (rank >= 0) {
	USER_ERROR("%% server atm address is also a "
	    "global LECS address of rank %d on interface %s", 
	    rank, pinfo->name);
	return NULL;
    }

    elanNameSize = strlen(elanName);
    byName = getElanByName(pCfg, elanName, elanNameSize);
    byLesAddress = getElanByLesAddress(pCfg, lesAddr, &psaa);

    /*
    ** exists by name AND by LES address.  Better imply the 
    ** same elan or else.  If same, attempt to change the 
    ** restricted status of the elan.
    */
    if (byName && byLesAddress) {
	if (byName != byLesAddress) {
	    USER_ERROR("%% name and address imply different elans");
	    return NULL;
	}

	/* attempting to change priority ? */
	if (differentPriorities(byName, psaa, priority)) {
	    PRINTF("changing address prio to %d\n", priority);

	    /* delete the old one */
	    serverAtmAddress_destroy(byName, psaa);

	    /* add it as new */
	    addServerAtmAddressToElan(byName, lesAddr, priority);
	}

	setElanRestrictedStatus(pCfg, NULL, byName, elanStatus);
	return byName;
    }

    /*
    ** exists by name only.  Add the LES address and
    ** attempt to change the restricted status of elan.
    */
    if (byName && (byLesAddress == NULL)) {
	addServerAtmAddressToElan(byName, lesAddr, priority);
	setElanRestrictedStatus(pCfg, NULL, byName, elanStatus);
	return byName;
    }

    /*
    ** create a new elan altogether
    */
    if ((byName == NULL) && (byLesAddress == NULL)) {
	/* create the new elan */
	byName = malloc_named(sizeof(ElanInfoBlock), "ElanInfoBlock");
	if (byName == NULL) {
	    LECS_ERR("could NOT create elan '%s'", elanName);
	    return NULL;
	}
	byName->lesAddressesList = List_alloc();
	if (byName->lesAddressesList == NULL) {
	    LECS_ERR("could NOT create lesAddressesList in elan '%s'",
		elanName);
	    free(byName);
	    return NULL;
	}
	GET_TIMESTAMP(byName->lastAccessTime);
	sstrncpy(byName->elanName, elanName, NAMESIZE);
	byName->elanNameSize = elanNameSize;
	byName->restricted = FALSE;
	byName->activeLes = NULL;
	byName->cfgTable = pCfg;
	byName->lesTimeout = LES_TIMEOUT_DFLT;
        byName->local_segment_id = 0;

	if (Index_add(pCfg->elanIndexByName, 
		elanName, byName) != byName)
	    LECS_ERR("ConfigTable_elanCreate: Index_add failed");
	PRINTF("created elan '%s'\n", elanName);
	setElanRestrictedStatus(pCfg, NULL, byName, elanStatus);

	/*
	** add the server atm address to the elan.
	** This is the first server atm address
	*/
	if (addServerAtmAddressToElan(byName, lesAddr, priority) == 
		NULL) {
	    List_destroy(byName->lesAddressesList);
	    free(byName);
	    return NULL;
	}

	/* write into global elan list */
	List_clearLinks(byName);
	List_write(pCfg->elanList, byName);

	return byName;
    }

    /*
    ** if we are here, address exists but elan does NOT.
    ** In the previous release of the software, this was
    ** used to rename an elan, but we are trying to discourage
    ** this now. Use the "new-name" command instead for this.
    */
    USER_ERROR("%% meaningless elan-address combination");
    return NULL;
}

/*
** set an already existing elan to be the
** default elan for this config table.  Note that setting
** to NULL means UN-defining the default elan, BUT
** not being able to find the elan specified by the
** name is an error.
**
** Note that a restricted elan cannot be made the default
** and vice versa.
*/
boolean ConfigTable_setDefaultElan (ConfigTable *pCfg, 
    char *elanName)
{
    ElanInfoBlock *pElan;

    /* is it being UN-defined ? */
    if (elanName == NULL) {
	if (lane_lecs_debug && (pCfg->defaultElan != NULL))
	    printf("default elan cleared\n");
	pCfg->defaultElan = NULL;
    } else {
	pElan = getElanByName(pCfg, elanName, strlen(elanName));
	if (pElan == NULL) {
	    USER_ERROR("%% no such elan exists in "
		"this config table\n");
	    return FALSE;
	}

	/* a restricted elan cannot be made default */
	if (pElan->restricted) {
	    USER_ERROR("%% elan is restricted and therefore ");
	    USER_ERROR("cannot be made the default\n");
	    return FALSE;
	}

	pCfg->defaultElan = pElan;
	PRINTF("the default elan is set to '%s'\n", elanName);
    }
    return TRUE;
}

boolean ConfigTable_destroyElanByName (ConfigTable *pCfg, 
    char *elanName)
{
    ElanInfoBlock *pElan;

    pElan = getElanByName(pCfg, elanName, strlen(elanName));
    if (pElan == NULL) {
	USER_ERROR("%% no such elan in this config table\n");
	return FALSE;
    }
    return ConfigTable_destroyElanByPtr(pCfg, pElan);
}

/*
** add a LEC to an Elan.  This implies membership.  
** Possible outcomes:
**
**	Method 1
**	--------
**
**		- lec is ALREADY in another/same elan.
**			remove it from that elan
**			enter it into the new elan
**
**		- lec is not there at all.
**			create a new lec object
**			enter it into the elan
**
**	Method 2
**	--------
**
**		- lec is ALREADY in another/same elan.
**			reject the request
**
**		- lec is not there at all.
**			create a new lec object
**			enter it into the elan
**
** In this routine, we will adopt method 2.  If a LEC is 
** deliberately to be moved from one to another elan, it 
** needs to be explicitly removed from the first elan and 
** then added to the new elan afterwards.  This does NOT 
** take too much work, hence method 2 is justified.
*/
LecInfoBlock *ConfigTable_addLec (ConfigTable *pCfg, char *elanName, 
    MaskedAddress *lecAddr)
{
    ElanInfoBlock *pElan, *elanAlready;
    LecInfoBlock *pLec, *pAlready;
    hwaddrtype hwaddr;

    /* sanity checks */
    if (lecAddr == NULL) LECS_CRASH();
    if (elanName == NULL) LECS_CRASH();
    if ((lecAddr->type != STATION_ATMNSAP) &&
	(lecAddr->type != STATION_IEEE48) && 
        (lecAddr->type != STATION_ROUTEDESC))
	    LECS_CRASH();

    if ((lecAddr->type == STATION_ATMNSAP) &&
	allZeroMask(lecAddr->maskBytes, STATIONLEN_ATMNSAP)) 
    {
	USER_ERROR("%% LANE Client nsap should NOT "
	    "be totally insignificant\n");
	return NULL;
    }

    /* now do the work */

    pElan = getElanByName(pCfg, elanName, strlen(elanName));
    if (pElan == NULL) {
	USER_ERROR("%% elan '%s' does not exist\n", elanName);
	return NULL;
    }

    /* ok so far so good, enter the LEC into the table */
    pLec = chunk_malloc(pCfg->lecTable);
    if (pLec == NULL) return NULL;

    /* store the particulars in the Lec object */
    pLec->address = *lecAddr;
    pLec->parentElan = pElan;
    GET_TIMESTAMP(pLec->lastAccessTime);

    /*
    ** insert into index by ATM address; 
    ** note that this is where duplication
    ** is caught.
    */
    if (lecAddr->type == STATION_ATMNSAP) {
	void *retVal;

	/* see if lec is already there */
	pAlready = ConfigTable_findLECByAtmAddr(pCfg, lecAddr);
	if (pAlready != NULL) {
	    /* back it out of the table */
	    chunk_free(pCfg->lecTable, pLec);

	    /* report the error */
	    USER_ERROR("%% LEC %Cw is already in elan '%s'\n",
		lecAddr, pAlready->parentElan->elanName);
	    return NULL;
	}

	/* not there so enter it into the appropriate list */
	if (allOnesMask(lecAddr->maskBytes, STATIONLEN_ATMNSAP)) {
	    /*
	    ** see if LEC is also a server atm address.
	    ** This should NOT be allowed
	    */
	    MaskedAddress2hwaddrtype(lecAddr, &hwaddr);
	    elanAlready = getElanByLesAddress(pCfg, &hwaddr, NULL);
	    if (elanAlready != NULL) {
		/* back it out of the table */
		chunk_free(pCfg->lecTable, pLec);

		USER_ERROR("%% specified client address is also a ");
		USER_ERROR("server atm address for elan '%s'\n", 
		    elanAlready->elanName);
		return NULL;
	    }
	    retVal = Index_add(pCfg->UNmaskedAtmAddressIndex, 
			lecAddr->dataBytes, pLec);
	} else {
	    retVal = Index_add(pCfg->maskedAtmAddressIndex, pLec, 
			pLec);
	}

	/* this should NOT happen */
	if (retVal != pLec) LECS_CRASH();

    } else if (lecAddr->type == STATION_IEEE48) {
	/*
	** insert into index by MAC address;
	** again, this catches duplication
	*/
	if (Index_add(pCfg->macAddressIndex, 
		lecAddr->dataBytes, pLec) != pLec) {
	    /* back it out of the table */
	    chunk_free(pCfg->lecTable, pLec);

	    pAlready = Index_searchElement 
		(pCfg->macAddressIndex, lecAddr->dataBytes, NULL);
	    USER_ERROR("%% LEC %Cw is already in elan '%s'\n",
		lecAddr, pAlready->parentElan->elanName);
	    return NULL;
	}
    } else if (lecAddr->type == STATION_ROUTEDESC) {
	/*
	** insert into index by route descriptor;
	** again, this catches duplication
	*/
	if (Index_add(pCfg->rdIndex, 
		lecAddr->dataBytes, pLec) != pLec) {
	    /* back it out of the table */
	    chunk_free(pCfg->lecTable, pLec);

	    pAlready = Index_searchElement
		(pCfg->rdIndex, lecAddr->dataBytes, NULL);
	    USER_ERROR("%% LEC %Cw is already in elan '%s'\n",
		lecAddr, pAlready->parentElan->elanName);
	    return NULL;
	}
    } else {
	/* 
	** unknown address type, BUT we checked this above already
	** and everything was ok then (paranoid programming)
	*/
	LECS_CRASH();
    }

    PRINTF("lec added to elan '%s'\n", pElan->elanName);
    return pLec;
}

/*
** bind or unbind a config table to or from an interface.
** If the pCfg parameter is NULL, this indicates an un-binding
** operation.  
**
** Note that when an un-binding occurs, all the LES redundancy
** stuff must be cleaned out from the config table.
*/
void ConfigTable_bindToLecsInfo (ConfigTable *pCfg, LecsInfo *pinfo)
{
    ConfigTable *oldTable;

    /*
    ** every time a config table is un-bound from the LAST
    ** interface, ie its bindCount reaches zero, then all
    ** the info in the elans that is monitoring the LES 
    ** redundancy design should be cleared out.
    */
    if (pCfg == NULL) {
	oldTable = pinfo->cfgTable;
	pinfo->cfgTable = NULL;
	if (oldTable) {
	    oldTable->bindCount--;
	    PRINTF("config table '%s' bind count is now: %d\n",
		oldTable->tableName, oldTable->bindCount);

	    /*
	    ** this table is NOT bound to any interfaces any more.
	    ** Clear out its LES redundancy information
	    */
	    if (oldTable->bindCount <= 0) {
		oldTable->bindCount = 0;
		cleanLesRedundancyInfo(oldTable, NULL);
	    }
	}
    } else {
	pinfo->cfgTable = pCfg;
	pCfg->bindCount++;
	PRINTF("config table '%s' bind count is now: %d\n",
	    pCfg->tableName, pCfg->bindCount);
    }

    /* this affects this lecs */
    revisitLecs(pinfo);
}

/******************************************************************
*******************************************************************
**                      LecsInfo object
**			(one per hw idb)
*******************************************************************
******************************************************************/

static LecsInfo *LecsInfo_alloc (hwidbtype *hwidb)
{
    idbtype *firstsw;
    LecsInfo *pinfo;

    firstsw = firstsw_or_null(hwidb);
    if (hwidb == NULL) LECS_CRASH();
    if (! is_atm(hwidb)) LECS_CRASH();
    if (firstsw == NULL) LECS_CRASH();

    /* first, see if in the inactive/deleted list */
    pinfo = List_head(G_DeletedLecsInfosList);
    while (pinfo != NULL) {
	/* search by name in the inactive list */
	if (strncmp(hwidb->hw_namestring, pinfo->name, 
			NAMESIZE-1) == 0) {
	    /* remove it from the deleted list */
	    List_remove(G_DeletedLecsInfosList, pinfo);

	    /* found what we were looking for */
	    break;
	}
	pinfo = List_next(pinfo);
    }

    /* if not found, create one from scratch */
    if (pinfo == NULL) {
	pinfo = malloc_named(sizeof(LecsInfo), "LecsInfo");
	if (pinfo == NULL) return NULL;
    }

    /* start filling its fields */
    memset(pinfo, 0, sizeof(LecsInfo));

    /* allocate the lecs nsap addresses list */
    if ((pinfo->lecsAddressesList = List_alloc()) == NULL) {
	LECS_ERR("LecsInfo_alloc: "
	    "List_alloc lecsAddressesList failed");
	free(pinfo);
	return NULL;
    }

    /* allocate the svc connection list */
    if ((pinfo->connectionList = List_alloc()) == NULL) {
	LECS_ERR("LecsInfo_alloc: "
	    "List_alloc connectionList failed");
	List_destroy(pinfo->lecsAddressesList);
	free(pinfo);
	return NULL;
    }

    /* no global LECSs exist at startup */
    pinfo->globalLecsCount = 0;

    /* get this from the system */
    pinfo->shutDown = interface_admin_down(firstsw);

    /* always start as master */
    pinfo->thisLecsIsTheMaster = TRUE;

    /* other bookkeeping */
    pinfo->lecsRunning = FALSE;
    GET_TIMESTAMP(pinfo->lecsUpTime);
    pinfo->hwidb = hwidb;
    sstrncpy(pinfo->name, hwidb->hw_namestring, NAMESIZE);
    List_write(G_LecsInfosList, pinfo);
    
    return pinfo;
}

/*
** destroy a SINGLE LecsInfo object.  Note that this routine
** does NOT really destroy the object.  It pacifies it first,
** and then puts it into the list of deleted LecsInfos.
*/
void LecsInfo_destroy (LecsInfo *pinfo)
{
    LecsAddress *addr;

    /* if it gets called in subinterface, this may be NULL */
    if (pinfo == NULL) return;

    /* shut it down first */
    stopLecs(pinfo);

    /* take this LecsInfo out of the active list */
    List_remove(G_LecsInfosList, pinfo);

    /* 
    ** delete all the lecs nsap addresses AND their list.
    ** as a side effect, this also releases and frees all the 
    ** relevant SVC connections to this LecsInfo.
    */
    while ((addr = List_read(pinfo->lecsAddressesList)) != NULL)
	LecsAddress_destroy(addr);
    List_destroy(pinfo->lecsAddressesList);
    pinfo->lecsAddressesList = NULL;

    /* destroy the connection lists themselves */
    List_destroy(pinfo->connectionList);
    pinfo->connectionList = NULL;

    /*
    ** make sure that the lane info pointing to 
    ** THIS interface knows that we are gone
    */
    pinfo->lane_info->lecs_info = NULL;

    /*
    ** ******* IMPORTANT *******
    **
    ** do NOT mess with the "name" field, since this
    ** will later be used to resurrect this LecsInfo object
    */

    /* now insert it into the dormant list, do NOT actually free it */
    List_write(G_DeletedLecsInfosList, pinfo);
}

/*
** lecs related cleanups just before
** a lane_info is cleared up.  This
** should really not happen since
** subinterface 0 cannot be deleted.
**
** Can it happen thru the SNMP agent ?
*/
void lecs_swif_erase (lane_info_t *lane_info)
{
    if (! lane_info) return;
    if (! lane_info->lecs_info) return;

    /*
    ** if we are here, somehow a major interface is
    ** being deleted.  This should NOT happen.
    */
    LECS_CRASH();

    /* clear the common_str_interface */
    LecsInfo_destroy(lane_info->lecs_info);
    lane_info->lecs_info = NULL;

    /* clear lecs finder stuff */
    clearLecsFinderModule(lane_info);
    lane_info->lecsFinderContext = NULL;

    lane_info_destroy(lane_info);
}

/*
** the method to be registered for interface up/down transitions.
** this is important only if there is an Lecs on this interface.
*/
void lecs_interface_updown (idbtype *swidb, 
    hwidbtype *hwidb)
{
    boolean down;
    LecsInfo *pinfo;

    if (!hwidb) return;
    if (!is_atm(hwidb)) return;
    pinfo = getLecsPointerFrom_HW(hwidb);
    if (!pinfo) return;
    down = interface_admin_down(hwidb->firstsw);

    /* has a real change taken place ? */
    if (pinfo->shutDown == down) return;

    LECS_DBG(pinfo->name, "processing interface %s transition", 
	down ? "down" : "up");
    pinfo->shutDown = down;
    revisitLecs(pinfo);
}

/*
** generic routine called if/when a possibility
** of changing any of the nsaps of the LECS
** does exist.  This is typical when a prefix
** event occurs or when a mac address changes.
*/
void lecs_addr_recycle (hwidbtype *hwidb, 
    lane_addr_recycle_reason reason)
{
    LecsInfo *pinfo;
    char *str;

    pinfo = getLecsPointerFrom_HW(hwidb);
    if (!pinfo) return;
    if (!pinfo->lecsRunning) return;
    str = lane_pr_addr_recycle_reason(reason);
    LECS_INFO("%s: recycling addresses due to %s event",
	hwidb->hw_namestring, str);
    deregister_all_nsaps(pinfo);
    release_all_circuits(pinfo, TRUE);
    revisitLecs(pinfo);
}

/*
** search an nsap address in the list of nsap addresses
** on an LecsInfo.  Return NULL if not found.
**
** The search can be performed EITHER by nsap OR by the snmp index.
** If the snmpIndex <= 0, the nsap is used, otherwise the index
** is used.  Since the index within an LecsInfo is guranteed to
** be unique, this will work.
**
** The rest of this comment applies ONLY if the snmpIndex <= 0.
**
** Note that the "formationType" is used as a hint during the search.
** The "fixed" and "auto" addresses will be already flagged in
** the list AND there will only always be ONE of these
** in the list.  Therefore, on these two special cases
** we can find them more quickly without having
** to perform a full address comparison.
**
** This routine will search for an EXACT match of BOTH the data
** and the mask bytes.  In other words, it will NOT perform a
** wildcard search.
*/
LecsAddress *searchLecsAddress (LecsInfo *pinfo, 
    MaskedAddress *maddr, int formationType, int snmpIndex)
{
    LecsAddress *paddr;

    paddr = List_head(pinfo->lecsAddressesList);
    while (paddr != NULL) {
	if (snmpIndex <= 0) {
	    /* check "fixed" and "auto" first */
	    if ((formationType != LANE_LECS_BIND_ADDR) &&
		(formationType == paddr->formationType))
		    return paddr;

	    /* manually searched, check the long hard way */
	    if (maskedAddressCompare(maddr, &paddr->maskedAddr))
		return paddr;
	} else { 
	    if (paddr->snmpIndex == snmpIndex) return paddr;
	}

	paddr = List_next(paddr);
    }
    return NULL;
}

/*
** add a new lecs nsap address to the LecsInfo address list.
** Be careful when mucking about with the well known LECS address.
** If the SSR has explicitly requested it, mark it as such.
*/
int addLecsAddress (idbtype *swidb, MaskedAddress *maddr,
     int formationType, int snmpIndex)
{
    LecsInfo *pinfo;
    LecsAddress *lecsAddr;

    pinfo = buildLecsPointer(swidb);
    if (pinfo == NULL) {
	LECS_ERR("addLecsAddress: buildLecsPointer failed");
	return NULL_IF;
    }

    /* check that it is not already there */
    lecsAddr = searchLecsAddress
		(pinfo, maddr, formationType, snmpIndex);
    if (lecsAddr != NULL) {
	USER_ERROR("%% this address is already bound "
	    "to this interface\n");
	return ALREADY_BOUND;
    }

    /* allocate address and set its values */
    lecsAddr = LecsAddress_alloc(pinfo);
    if (lecsAddr == NULL) {
	LECS_ERR("addLecsAddress: Address_alloc failed");
	return MALLOC_FAILURE;
    }
    LecsAddress_setAtmValues 
	(lecsAddr, maddr->dataBytes, maddr->maskBytes, 
	 formationType, snmpIndex);

    /* enter it into the list */
    List_write(pinfo->lecsAddressesList, lecsAddr);

    PRINTF("lecs nsap address added to interface\n");

    /* does the lecs need starting ? */
    revisitLecs(pinfo);

    return NO_ERROR;
}

/*
** delete (disassociate) an LECS nsap address 
** from an LecsInfo.  The address parameter
** is what describes one of the LECS addresses
** and is hence is matched to the called party.
*/
int deleteLecsAddress (idbtype *swidb, MaskedAddress *masked,
     int formationType, int snmpIndex)
{
    LecsInfo *pinfo;
    LecsAddress *lecsAddr;

    pinfo = getLecsPointer(swidb);
    if (pinfo == NULL) {
	USER_ERROR("%% LANE Config Server is NOT "
	    "configured on this interface\n");
	return NULL_IF;
    }

    /* find it first */
    lecsAddr = searchLecsAddress(pinfo, masked, formationType, 
		    snmpIndex);
    if (lecsAddr == NULL) {
	return NOT_BOUND;
    }

    List_remove(pinfo->lecsAddressesList, lecsAddr);

    /* This will DE-register address from all services */
    LecsAddress_destroy(lecsAddr);

    PRINTF("Address un-bound from interface\n"); 

    /* does the lecs need shutting down ? */
    revisitLecs(pinfo);

    return NO_ERROR;
}

/*
** expose the specified lecs nsap address at the sub interface
** level for NON LECS entities.
*/
void exposeThisLecsAddress (idbtype *swidb, MaskedAddress *maddr, 
    int formationType)
{
    lane_info_t *linfo;
    MaskedAddress *exposed;

    linfo = lane_init_idb(swidb);
    if (linfo == NULL) {
	LECS_ERR("exposeThisLecsAddress: lane_init_idb failed\n");
	return;
    }
    exposed = &linfo->lecs_nsap;

    /* if address is already exposed, get out */
    if (! invalidMaskedAddress(exposed)) {
	if (maskedAddressCompare(exposed, maddr)) {
	    PRINTF("specified address already exposed\n");
	    return;
	}
    }

    /* fill in the ** manually ** specified address */
    linfo->lecs_nsap_type = formationType;

    PRINTF("%Cw is exposed as the lecs address\n", maddr);

    /* tell LES/BUS and client about this */
    lsv_new_nsap(linfo, TRUE, maddr, exposed, "Config Server");

    /* "lsv_new_nsap" above takes care of this */
    /* linfo->lecs_nsap = *maddr; */
}

/*
** clear the exposure of the lecs nsap address 
*/
static void clearExposedLecsAddress (idbtype *swidb)
{
    lane_info_t *linfo;
    MaskedAddress *exposed;

    linfo = swidb->lane_info;
    if (linfo == NULL) {
	USER_ERROR("%% LANE is NOT configured on this interface\n");
	return;
    }
    exposed = &linfo->lecs_nsap;

    /* if already clear, get out */
    if (invalidMaskedAddress(exposed)) {
	PRINTF("LECS %s: exposed address is already clear\n", 
		    swidb->namestring);
	return;
    }

    PRINTF("LECS %s: exposed lecs address is cleared\n", 
	swidb->namestring);

    /* tell LES/BUS and LEC about this */
    lsv_new_nsap(linfo, FALSE, NULL, exposed, "Config Server");

    /* "lsv_new_nsap" above takes care of this */
    /* clearMaskedAddress(exposed); */
}

/*
** clear the exposure of the lecs nsap address ONLY if the
** specified address happens to match the exposed address
*/
void unExposeThisLecsAddress (idbtype *swidb, MaskedAddress *masked)
{
    MaskedAddress *exposed;
    lane_info_t *linfo;

    linfo = swidb->lane_info;
    if (linfo == NULL) {
	USER_ERROR("%% LANE is NOT configured on this interface\n");
	return;
    }
    exposed = &linfo->lecs_nsap;
    if (invalidMaskedAddress(exposed)) {
	PRINTF("LECS %s: exposed address is already clear\n",
	    swidb->namestring);
	return;
    }
    if (maskedAddressCompare(exposed, masked))
	clearExposedLecsAddress(swidb);
    else
	USER_ERROR("%% specified address does NOT "
	    "match the currently exposed address\n");
}

/******************************************************************
*******************************************************************
**			global initializations
*******************************************************************
*******************************************************************/

/*
** helper to traverse to the LECS pointer from
** a software idb.
*/
LecsInfo *getLecsPointer (idbtype *swidb)
{
    hwidbtype *hwidb;

    if (!swidb) return NULL;
    hwidb = hwidb_or_null(swidb);
    if (!hwidb) return NULL;
    swidb = firstsw_or_null(hwidb);
    if (!swidb) return NULL;
    if (!swidb->lane_info) return NULL;
    return swidb->lane_info->lecs_info;
}

/*
** variation of the above for the hw idb.
*/
LecsInfo *getLecsPointerFrom_HW (hwidbtype *hwidb)
{
    if (!hwidb) return NULL;
    return getLecsPointer(hwidb->firstsw);
}

/*
** builds the pointer chain at the swidb for the
** lane info.  It first builds the "lane_info" if
** NULL, followed by the "lecs_info" if NULL.  Returns
** "lecs_info" or NULL if any errors.
**
** This routine also starts the lecs server if not already
** running.
**
** Note that as far as the lecs is concerned, it is only
** interested in the interface and NOT the SUB-interface.
** Therefore, this routine sets the "lecs_info" field
** of the "lane_info" structure ONLY for the FIRST swidb.
** All subsequent sw idb's do NOT have an lecs pointer set.
*/
LecsInfo *buildLecsPointer (idbtype *swidb)
{
    hwidbtype *hwidb;
    LecsInfo *pinfo;

    if (swidb == NULL) LECS_CRASH();
    hwidb = hwidb_or_null(swidb);
    if (hwidb == NULL) LECS_CRASH();
    
    /*
    ** now get the FIRST sw idb.  lecs info ALWAYS hangs off 
    ** the interface (NOT the sub-interface) hence the FIRST
    ** software idb
    */
    swidb = firstsw_or_null(hwidb);
    if (swidb == NULL) LECS_CRASH();

    /* build the "lane_info" structure */
    if (swidb->lane_info == NULL) {
	swidb->lane_info = lane_init_idb(swidb);
	if (swidb->lane_info == NULL) {
	    LECS_ERR("buildLecsPointer: lane_init_idb failed");
	    return NULL;
	}
    }

    /* now build the "LecsInfo" structure for the lecs */
    pinfo = swidb->lane_info->lecs_info;
    if (pinfo == NULL) {
	pinfo = swidb->lane_info->lecs_info = LecsInfo_alloc(hwidb);
	if (pinfo == NULL) {
	    LECS_ERR("buildLecsPointer: LecsInfo_alloc failed");
	    return NULL;
	}

	/* point back to the lane_info struct */
	pinfo->lane_info = swidb->lane_info;
    }

    return pinfo;
}

/*
** initialize all global data structures
*/
boolean lecs_init (void)
{
    lane_cfg_pdu_t cfg;

    /* turn debug off by default */
    lane_lecs_debug = FALSE;
    lane_lecsPacket_debug = FALSE;

    /* start with 1 */
    G_snmpIndex = 1;

    /* ILMI related stuff */
    G_lecsServerPid = NO_PROCESS;
    G_ilmiClientId = ILMI_ERROR;
    G_ilmiRegisteredForResponses = FALSE;

    /* minimum acceptable config request packet length */
    G_minLaneConfigPakLen = ((int) (&cfg.tlvs [0]) - (int) &cfg);

    LECS_DBG(NULL, "lecs_init activated");
    if (G_lecsInited) {
	LECS_ERR("lecs already initialized");
	return TRUE;
    }
    G_LecsInfosList = List_alloc();
    if (G_LecsInfosList == NULL) {
	LECS_ERR("List_alloc for G_LecsInfosList failed");
	return FALSE;
    }
    LECS_DBG(NULL, "G_LecsInfosList allocated");
    G_DeletedLecsInfosList = List_alloc();
    if (G_DeletedLecsInfosList == NULL) {
	LECS_ERR("List_alloc for G_DeletedLecsInfosList failed");
	List_destroy(G_LecsInfosList);
	return FALSE;
    }
    LECS_DBG(NULL, "G_LecsInfosList allocated");
    G_ConfigTablesList = List_alloc();
    if (G_ConfigTablesList == NULL) {
	LECS_ERR("List_alloc for G_ConfigTablesList failed");
	List_destroy(G_LecsInfosList);
	List_destroy(G_DeletedLecsInfosList);
	return FALSE;
    }
    LECS_DBG(NULL, "G_ConfigTablesList allocated");

    /* allocate this when needed not now */
    G_ConnectionTable = NULL;

    /* initialize the input packet queue */
    initWatchedQueue(&lecsInputQ, "lecsInputQ");

    /* initialize static timers */
    mgd_timer_init_parent(&lecsMasterTimer, NULL);
    mgd_timer_init_leaf(&lecsPeriodic, &lecsMasterTimer,
	GENERIC_TIMER, NULL, FALSE);
    LECS_DBG(NULL, "lecsMasterTimer mgd_timer initialized");

    /* register LECS with ILMI services */
    registerLECSwithILMI();

    /* what to use to enq packets to the LECS */
    reg_add_lecs_enqueue(lecs_enqueue, "lecs_enqueue");

    G_lecsInited = TRUE;
    LECS_DBG(NULL, "successfully initialized");

    return TRUE;
}

/*
** lecs_enqueue
**
** This function is called from fast path code to deliver 
** packets to the LECS.  Just stuff the packet into the 
** lecs server input queue.
**
** We're always returning TRUE, so that the registry can 
** be checked to see if LANE is linked in.
*/
boolean lecs_enqueue (paktype *pak)
{
    /* queue the packets up ONLY if the server is running */
    if (G_lecsServerPid == NO_PROCESS) {
	/* discard packet */
	pak->flags |= PAK_ABORT;
	netinput_enqueue(pak);
	LECSP_DBG(pak->if_input->namestring,
	    "packet received and discarded (no LECS server)");
    } else {
	process_enqueue_pak(lecsInputQ, pak);
	LECSP_DBG(pak->if_input->namestring,
	    "packet received and enqueued");
    }
    return TRUE;
}

/*
** send the packet out.
** Note that by the time we call this, the "if_output"
** on the packet MUST already have been set to the correct
** interface.  Also note that the "vcd" is already on the
** packet itself.
*/
static void transmitPacket (paktype *pak, char *ifname)
{
    if (lane_control_encap(pak, pak->vcd)) {
	datagram_out(pak);
	LECSP_DBG(ifname, "packet transmitted");
    } else {
	LECS_ERR("transmitPacket: lane_control_encap on %s failed", 
	    ifname);
	datagram_done(pak);
    }
}

/*
** creates a config response with an error status.
** clears all variables out to 0
*/
static void sendErrorResponse (paktype *pak, LecsInfo *pinfo, 
    lane_cfg_pdu_t *cfg, int error)
{
    /* update error stuff */
    pinfo->lecsOutConfigFails++;
    pinfo->causeOfLastFailure = error;
    bcopy(cfg->src_atm_addr, pinfo->lastOffender, 
	STATIONLEN_ATMNSAP);

    /* prepare the return message */
    cfg->hdr.status = error;
    cfg->name_size = 0;
    bzero(cfg->target_atm_addr, STATIONLEN_ATMNSAP);
    bzero(cfg->elan_name, MAX_ELAN_NAME_LENGTH);
    LECSP_DBG(pinfo->name, 
	"config response error %d pak assembled", error);

    /* send it out */
    transmitPacket(pak, pinfo->name);
}

/* 
** for debugging the config request packet, dump out some
** important fields
*/
static void dumpReceivedPacket (char *ifname, lane_cfg_pdu_t *cfg)
{
    int i;

    buginf("\n--- LECS PACKET %s: dump of input packet ---\n", 
	ifname);
    buginf("opcode: 0x%x\n", cfg->hdr.opcode);
    buginf("transaction_id: %d\n", cfg->hdr.transaction_id);
    buginf("src_lan_dest: ");
    switch (cfg->src_lan_dest.tag) {
	case LANE_TAG_NOT_PRESENT:
	    buginf("NOT PRESENT");
	    break;
	case LANE_TAG_MAC_ADDRESS:
	    buginf("%e", cfg->src_lan_dest.addr.mac);
	    break;
	case LANE_TAG_ROUTE_DESC:
       buginf("%d %d", cfg->src_lan_dest.addr.rd.val >> 4,
                       cfg->src_lan_dest.addr.rd.val & 0xF);
	    break;
	default:
	    buginf("invalid tag value %d", cfg->src_lan_dest.tag);
	    break;
    }
    buginf("\n");
    buginf("lan_type: %d\n", cfg->lan_type);
    buginf("max_frame_size: %d\n", cfg->max_frame_size);
    buginf("num_tlvs: %d\n", cfg->num_tlvs);
    buginf("src_atm_addr: %Cj\n", cfg->src_atm_addr);
    buginf("name_size: %d\n", cfg->name_size);
    if (cfg->name_size > 0) {
	buginf("elan_name: ");
	for (i=0; i<cfg->name_size; i++) 
	    buginf("%c", cfg->elan_name [i]);
	buginf("\n");
    }
    buginf("------------------------------------------");
}

/*
** search an LEC specified by a mac address in the config
** request in the configuration table and return the elan
** pointer if found
*/
static ElanInfoBlock *searchLEC_InMacAddrList (ConfigTable *pCfg, 
    uchar *macAddr, char *ifname)
{
    LecInfoBlock *pLec;
    ElanInfoBlock *pElan = NULL;

    pLec = Index_searchElement
	    (pCfg->macAddressIndex, macAddr, NULL);
    if (pLec != NULL) {
	pElan = pLec->parentElan;
	LECSP_DBG(ifname,
	    "matched LEC mac addr %Cw elan name '%s'",
	    &pLec->address, pElan->elanName);
    }
    return pElan;
}

/*
** search an LEC specified by an atm nsap address in the config
** request in the configuration table and return the elan
** pointer if found.  This search is conducted in the UNmasked
** index only.
*/
static ElanInfoBlock *searchLEC_InUnmaskedList (ConfigTable *pCfg, 
    uchar *atmAddr, char *ifname)
{
    LecInfoBlock *pLec;
    ElanInfoBlock *pElan = NULL;

    pLec = Index_searchElement 
		(pCfg->UNmaskedAtmAddressIndex, atmAddr, NULL);
    if (pLec != NULL) {
	pElan = pLec->parentElan;
	LECSP_DBG(ifname,
	    "matched LEC unmasked nsap %Cw elan name '%s'",
	    &pLec->address, pElan->elanName);
    }
    return pElan;
}

/*
** search a LEC specified by an atm nsap address in the config
** request in the configuration table and return the elan
** pointer if found.  This search is conducted in the masked
** index given the starting index value.  It is called until
** no more matches are found.
*/
static ElanInfoBlock *searchLEC_InMaskedList (ConfigTable *pCfg, 
    uchar *atmAddr, int *index, char *ifname)
{
    LecInfoBlock *pLec;
    ElanInfoBlock *pElan = NULL;
    int idx = *index;

    idx = Index_linearSearch 
	   (pCfg->maskedAtmAddressIndex, idx, atmAddr,
	    (CMP) compareLECsUsingMask, &pLec);
    *index = idx;
    if (idx == NO_INDEX) return NULL;
    pElan = pLec->parentElan;
    LECSP_DBG(ifname,
	"matched LEC masked nsap %Cw elan name '%s'",
	&pLec->address, pElan->elanName);
    return pElan;
}

/*
** search an LEC specified by a route descriptor in the config
** request in the configuration table and return the elan
** pointer if found
*/
static ElanInfoBlock *searchLEC_InRDList (ConfigTable *pCfg,
    uchar *rd_ptr, char *ifname)
{
    LecInfoBlock *pLec;
    ElanInfoBlock *pElan = NULL;

    pLec = Index_searchElement(pCfg->rdIndex, rd_ptr, NULL);
    if (pLec != NULL) {
	pElan = pLec->parentElan;
	LECSP_DBG(ifname, 
	    "matched LEC route descriptor %Cw elan name '%s'",
	    &pLec->address, pElan->elanName);
    }
    return pElan;
}

/*
** check whether the elan found so far from previous searches
** matches the elan found just recently and print an error message
** if an inconsistency is detected.
*/
static boolean inconsistentElans (ElanInfoBlock *elanSoFar, 
    ElanInfoBlock *currentElan, char *ifname)
{
    if (elanSoFar && (elanSoFar != currentElan)) {
	LECSP_DBG(ifname, 
	    "config request elan conflict: '%s' != '%s'",
	    elanSoFar->elanName, currentElan->elanName);
	return TRUE;
    }
    return FALSE;
}

/*
** check the lan type field in the config request.
*/
static inline boolean badLanType (int lanType)
{
    return ((lanType < LANE_TYPE_UNSPECIFIED) ||
	    (lanType > LANE_TYPE_802_5));
}

/*
** check the frame size field in the config request.
*/
static inline boolean badFrameSize (int frameSize)
{
    return ((frameSize > LANE_MAX_FRAME_18190) ||
	    (frameSize < LANE_MAX_FRAME_UNSPECIFIED));
}

/* token ring TLV */
static const uchar local_seg_tlv [5] = 
{
    0x00, 0xA0, 0x3E,	/* ATM  Forum OUI */
    0x0B		/* C23 local segment id */
};

/*
** processes a single config request.  Note that since
** this guy uses the input message itself directly to
** compose the output message, it does NOT need to
** allocate or de-allocate any buffers.  This is possible
** since the output message size is exactly the same as the
** input message size (becoz we do NOT add any tlv's).
*/
static void lecsProcessSinglePacket (paktype *pak)
{
    register lane_ctl_hdr_t *hdr;
    register lane_cfg_pdu_t *cfg;
    ElanInfoBlock *pElanTemp;
    ElanInfoBlock *pElan;
    LecsInfo *pinfo;
    ConfigTable *pCfg;
    hwidbtype *hwidb;
    idbtype *swidb;
    int index;
    char *ifname;
    boolean foundByNameOnly, success;
    ServerAtmAddress *psaa, *callingLes = NULL;
    int vcd = pak->vcd;
    CallOriginatorType callerType = CALLER_IS_LEC;
    Connection *pconn;
    paktype *rsp_packet;
    register lane_cfg_pdu_t *tr_cfg;

    /* set the outgoing interface to be the same */
    swidb = pak->if_output = pak->if_input;
    if (swidb == NULL) LECS_CRASH();
    ifname = swidb->namestring;
    if (invalid_vcd(vcd)) {
	LECS_ERR("interface %s: lecsProcessSinglePacket "
	    "received a packet with invalid vcd %u",
	    ifname, vcd);
	datagram_done(pak);
	return;
    }
    hwidb = hwidb_or_null(swidb);
    if (hwidb == NULL) LECS_CRASH();
    swidb = firstsw_or_null(hwidb);
    if (swidb == NULL) LECS_CRASH();
    if (swidb->lane_info == NULL) {
	LECSP_DBG(ifname, 
	    "lecsProcessSinglePacket: lane_info is NULL");
	datagram_done(pak);
	return;
    }
    pinfo = swidb->lane_info->lecs_info;
    if (pinfo == NULL) {
	LECSP_DBG(ifname, "pinfo (lecs_info) is NULL");
	datagram_done(pak);
	return;
    }
    if (pinfo->shutDown) {
	LECSP_DBG(ifname, "interface is shut down");
	datagram_done(pak);
	return;
    }
    pCfg = pinfo->cfgTable;
    if (pCfg == NULL) {
	LECSP_DBG(ifname, "pinfo->cfgTable is NULL");
	datagram_done(pak);
	return;
    }

    /*
    ** What entity is sending this packet ? 
    ** We need to know this in advance to demux the stuff
    ** properly.  Note that there is a very small chance
    ** that a packet may already have come in just as the
    ** LECS is disconnecting that connexion.  So, handle
    ** this specific case gracefully.
    */
    pconn = pinfo->connexions [vcd];
    if (pconn == NULL) {
	datagram_done(pak);
	return;
    }
    callerType = pconn->callerType;

    if (lane_lecsPacket_debug) {
	buginf("\nLECS PACKET %s: a packet is received from an ",
	    pinfo->name);
	switch (callerType) {
	    case NO_CALLER: buginf("BAD CIRCUIT\n"); break;
	    case CALLER_IS_LEC: buginf("LEC\n"); break;
	    case CALLER_IS_LES: buginf("LES\n"); break;
	    case CALLER_IS_LECS: buginf("LECS\n"); break;
	}
    }

    /* should not happen but.... */
    if (callerType == NO_CALLER) {
	datagram_done(pak);
	return;
    }

    /* 
    ** analyze packet contents sanity.  Packet gets dropped 
    ** and NO response is generated if errors are encountered
    */

    cfg = (lane_cfg_pdu_t*) pak->datagramstart;
    if (pak->datagramsize < G_minLaneConfigPakLen) {
	LECSP_DBG(ifname, "incorrect config req packet size %d",
	    pak->datagramsize);
	pinfo->lecsInConfigErrors++;
	datagram_done(pak);
	return;
    }
    hdr = &cfg->hdr;
    if (hdr->marker != LANE_MARKER) {
	LECSP_DBG(ifname, 
	    "bad config req packet marker 0x%x", hdr->marker);
	pinfo->lecsInConfigErrors++;
	datagram_done(pak);
	return;
    }
    if (hdr->protocol != LANE_PROTOCOL_ID) {
	LECSP_DBG(ifname, "bad config req packet protocol id 0x%x", 
	    hdr->protocol);
	pinfo->lecsInConfigErrors++;
	datagram_done(pak);
	return;
    }
    if (hdr->version != LANE_PROTOCOL_VERSION) {
	LECSP_DBG(ifname, 
	    "bad config req packet protocol version 0x%x", 
	    hdr->version);
	pinfo->lecsInConfigErrors++;
	datagram_done(pak);
	return;
    }

    /* debugging dump */
    if (lane_lecsPacket_debug) 
	dumpReceivedPacket(pinfo->name, cfg);

    /* check the opcode validity */
    if (hdr->opcode != LANE_CONFIG_REQ) {
	LECSP_DBG(ifname, "bad config req packet opcode 0x%x", 
	    hdr->opcode);
	pinfo->lecsInConfigErrors++;
	datagram_done(pak);
	return;
    }

    /* now convert the query frame to a response frame */
    hdr->opcode = LANE_CONFIG_RSP;

    /*
    ** assume things will be ok from now on.  Any errors 
    ** encountered at this stage WILL be responded to, but 
    ** with an error status in the config response frame.  
    ** Note that however, if any response frame is received, 
    ** drop it without a response since it may generate
    ** perpetual response packets in the network.
    */
    hdr->status = LANE_STATUS_SUCCESS;

    /* update stats */
    pinfo->lecsInConfigReqs++;

    /* check lan type */
    if (badLanType(cfg->lan_type)) {
	LECSP_DBG(ifname, "bad lan type %d in config request",
	    cfg->lan_type);
	sendErrorResponse(pak, pinfo, cfg, 
	    LANE_STATUS_INVALID_LAN_DEST);
    }

    /* check frame size */
    if (badFrameSize(cfg->max_frame_size)) {
	LECSP_DBG(ifname, "bad frame size %d in config request",
	    cfg->max_frame_size);
	sendErrorResponse(pak, pinfo, cfg, 
	    LANE_STATUS_INVALID_REQ_PARMS);
    }

    /* some more initializations */
    cfg->num_tlvs = 0;
    pak->datagramsize = G_minLaneConfigPakLen;

    /*
    ** This should not really happen, but....
    **
    ** if the caller is an lecs finder, this is part of the
    ** LECS mastership determination procedure.  Answer it
    ** back very quickly with a NO_CONFIG error.  The fact
    ** that we are returning an error is not relevant.  The
    ** important thing is that we respond one way or another,
    ** within the timeout period.  It is just the fact that 
    ** we have responded in time is important.
    */
    if (callerType == CALLER_IS_LECS) {
	sendErrorResponse(pak, pinfo, cfg, 
	    LANE_STATUS_NO_CONFIG);
	return;
    } else if (callerType == CALLER_IS_LES) {
	/*
	** if the LECS knows that this request is from an LES AND 
	** that this LES is NOT the active one for its elan, 
	** it should respond with an error immediately without 
	** having to do anything else.
	*/
	callingLes = pconn->lesAddress;

	/* 
	** if this is NULL, the calling LES is just
	** being relegated to a secondary position
	** at this instant, so reject the request.
	*/
	if (callingLes == NULL) {
	    LECSP_DBG(ifname, "calling LES is just being "
		"relegated to a secondary position");
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_NO_CONFIG);
	    return;
	}
	pElan = callingLes->elanPtr;

	/* if not running in redundant mode, it does not matter */
	if (List_size(pElan->lesAddressesList) > 1) {
	    if (callingLes != pElan->activeLes) { 
		LECSP_DBG(ifname, 
		    "request is from a NON active LES of elan '%s'",
		    pElan->elanName);
		sendErrorResponse(pak, pinfo, cfg, 
		    LANE_STATUS_NO_CONFIG);
		return;
	    }
	}
    }

    /* nothing known so far */
    pElan = pElanTemp = NULL;
    foundByNameOnly = TRUE;

    /* search by tag type if specified */
    switch (cfg->src_lan_dest.tag) {

	case LANE_TAG_NOT_PRESENT:
	    break;

	case LANE_TAG_MAC_ADDRESS:
	    pElan = searchLEC_InMacAddrList(pCfg, 
			cfg->src_lan_dest.addr.mac, ifname);
	    if (pElan) foundByNameOnly = FALSE;
	    break;

	case LANE_TAG_ROUTE_DESC:
	    pElan = searchLEC_InRDList(pCfg,
			(uchar*) &(cfg->src_lan_dest.addr.rd.val), 
			ifname);
	    if (pElan) foundByNameOnly = FALSE;
	    break;

	default:
	    LECSP_DBG(ifname, "bad lan_dest tag %d in cfg request",
		cfg->src_lan_dest.tag);
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_INVALID_LAN_DEST);
	    return;
    }

    /* search in unmasked nsap list */
    pElanTemp = searchLEC_InUnmaskedList(pCfg, 
		    cfg->src_atm_addr, ifname);
    if (pElanTemp != NULL) {
	foundByNameOnly = FALSE;
	if (inconsistentElans(pElan, pElanTemp, ifname)) {
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_LE_CONFIG_ERROR);
	    return;
	} else {
	    pElan = pElanTemp;
	}
    }

    /* search in masked nsap list as long as matches occur */
    index = 0;
    while (TRUE) {
	pElanTemp = searchLEC_InMaskedList(pCfg, 
			cfg->src_atm_addr, &index, ifname);
	if (pElanTemp == NULL) break;
	foundByNameOnly = FALSE;
	if (inconsistentElans(pElan, pElanTemp, ifname)) {
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_LE_CONFIG_ERROR);
	    return;
	} else {
	    pElan = pElanTemp;
	}
	index++;
    }

    /*
    ** lastly, search on elan name IF SPECIFIED.  Note however,
    ** if the elan name is explicitly specified, and NOT found
    ** by the lecs, regardless of whatever else has matched, reject
    ** the config request.
    */
    pElanTemp = NULL;
    if ((cfg->name_size > 0) && 
	(cfg->name_size <= MAX_ELAN_NAME_LENGTH)) {
	pElanTemp = getElanByName(pCfg, cfg->elan_name, 
			cfg->name_size);
	if (pElanTemp == NULL) {
	    LECSP_DBG(ifname, "elan '%s' inquired by name not found",
		cfg->elan_name);
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_NO_CONFIG);
	    return;
	} else {
	    LECSP_DBG(ifname, "found elan '%s' by name", 
		pElanTemp->elanName);
	    if (inconsistentElans(pElan, pElanTemp, ifname)) {
		sendErrorResponse(pak, pinfo, cfg, 
		    LANE_STATUS_LE_CONFIG_ERROR);
		return;
	    }

	    /* 
	    ** if this is a "restricted" elan, and we found it
	    ** only by asking the elan name, this should not
	    ** be allowed.
	    */
	    if (pElanTemp->restricted && foundByNameOnly) {
		LECSP_DBG(ifname, 
		    "access denied to restricted elan found "
		    "by name only");
		sendErrorResponse(pak, pinfo, cfg, 
		    LANE_STATUS_ACCESS_DENIED);
		return;
	    }

	    pElan = pElanTemp;
	}
    }

    /* if still nothing found, use the default one (if exists) */
    if (pElan == NULL) {
	pElan = pCfg->defaultElan;
	LECSP_DBG(ifname, 
	    "using default elan '%s' for config response", 
	    pElan ? pElan->elanName : "NULL");
    }

    /* if still no elan, there is no config for this guy */
    if (pElan == NULL)
	sendErrorResponse(pak, pinfo, cfg, LANE_STATUS_NO_CONFIG);
    else {
	/*
	** we may have success here; check for redundancy rules
	*/

	/*
	** if the request originated from an LES, then it
	** should NOT ask for an elan other than the one 
	** that it is supposed to be serving.  Just warn 
	** but continue.
	*/
	if (callingLes && (pElan != callingLes->elanPtr)) {
	    LECS_WARN("%s: elan '%s' LES asking for elan '%s'",
		ifname, callingLes->elanPtr->elanName, 
		pElan->elanName);
/*
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_NO_CONFIG);
	    return;
*/
	}

	psaa = NULL;
	success = FALSE;

	/* time stamp the found elan */
	GET_TIMESTAMP(pElan->lastAccessTime);

	/*
	** if only a single server atm address exists for this elan,
	** then revert back to the classic operation mode; ie, give 
	** out the LES address (which should always be at the head 
	** of the server atm addresses list), whether the LES is 
	** connected or not.
	*/
	if (List_size(pElan->lesAddressesList) <= 1) {
	    success = TRUE;
	    psaa = List_head(pElan->lesAddressesList);
	} else if (pElan->activeLes != NULL) {
	    /* 
	    ** if we are here, we MUST have an active connected LES
	    ** for this elan before we can successfully give out the
	    ** LES address.
	    */
	    success = TRUE;
	    psaa = pElan->activeLes;
	} else {
	    LECSP_DBG(ifname, 
		"no active/connected LES exists for elan '%s'",
		pElan->elanName);
	}

	if (success) {
	    LECSP_DBG(ifname, 
		"config request for elan '%s' succeeded",
		pElan->elanName);

	    /* time stamp the found elan */
	    GET_TIMESTAMP(pElan->lastAccessTime);

	    /* copy elan name to the config response */
	    cfg->name_size = strlen(pElan->elanName);
	    bcopy(pElan->elanName, cfg->elan_name, cfg->name_size);

	    /* copy the les address to the config response */
	    bcopy(psaa->lesAddr.addr, cfg->target_atm_addr, 
	       STATIONLEN_ATMNSAP);

	    /* 
	    ** for token ring add a TLV to the response 
	    ** to return the local segment id.
	    */
	    if (cfg->lan_type == LANE_TYPE_802_5) {
		if (pElan->local_segment_id == 0) {
		    LECSP_DBG(ifname, 
			"but elan has no local segment id");
		    sendErrorResponse(pak, pinfo, cfg,
			LANE_STATUS_INSUFF_RESOURCE);
		    return;
		}

		/* try and add the tlv */
		rsp_packet = pak_grow(pak, pak->datagramsize,
		    pak->datagramsize + LANE_LOCAL_SEG_TLV_SIZE);
		if (rsp_packet == NULL) {
		    LECSP_DBG(ifname, "but pak_grow failed");
		    sendErrorResponse(pak, pinfo, cfg, 
		       LANE_STATUS_INSUFF_RESOURCE);
		    return;
		}
			
		rsp_packet->datagramsize += LANE_LOCAL_SEG_TLV_SIZE;
		tr_cfg = (lane_cfg_pdu_t*) rsp_packet->datagramstart;

		/* this is the 4 byte type for local segment id */
		PUTLONG((ulong*) &tr_cfg->tlvs[0],
		    *((ulong*)&local_seg_tlv[0]));
 
		/*
		** this is the length (1 byte), segment id 
		** (2 bytes) in the high order of the ulong.  
		** Note the shift to the high order.
		*/
		PUTLONG((ulong*) &tr_cfg->tlvs [LANE_TLV_LEN_OFFSET],
		    (ulong) ((LANE_LOCAL_SEG_LENGTH << 24) |
		    (pElan->local_segment_id << 8)));
		tr_cfg->num_tlvs = 1;
		pak = rsp_packet;
	    }

	    /* send out the success */
	    transmitPacket(pak, ifname);
	} else {
	    sendErrorResponse(pak, pinfo, cfg, 
		LANE_STATUS_NO_CONFIG);
	}
    }
}

/*
** for every possible LECS in the system,
** check if any higher LECs is connected
** to it or not and declare mastership if
** no higher LECS is connected.
*/
static void checkMastership (void)
{
    LecsInfo *pinfo;

    FOR_ALL_LECSS(pinfo) {
	evaluateLecsMastership(pinfo);
    }
}

/*
** have the LECS addresses on the switch changed
** with respect to what we have previously stored
** locally in the LECS ?
*/
static boolean sameGlobalLecsAddresses (LecsInfo *pinfo, int count,
    hwaddrtype *addresses)
{
    int i;
    GlobalLecsAddress *glecs;

    if (pinfo->globalLecsCount != count) return FALSE;
    for (i=0; i<count; i++) {
	glecs = &pinfo->globalLecsTable [i];
	if (byteStreamCompare
		(glecs->hwaddr.addr, addresses [i].addr,
		 STATIONLEN_ATMNSAP) != 0)
		    return FALSE;
    }
    return TRUE;
}

/*
** signalling callback function which deals with
** connections to the LOWER LECSs.
*/
static void lowerLecssSignallingCB (void *sigmsg)
{
    sig_api_msg *msg = (sig_api_msg*) sigmsg;
    char *ifname = msg->idb->namestring;
    boolean success = TRUE;
    void *callId = NULL;
    LecsInfo *pinfo = NULL;
    GlobalLecsAddress *glecs = NULL;
    boolean found = FALSE;
    int i;

    switch (msg->opcode) {
	/* our setup request just got accepted */
	case ATM_CONNECT:
	    callId = msg->u.connect.call_id; 
	    break;
	case ATM_CONNECT_ACK:
	    callId = msg->u.connect_ack.call_id; 
	    break;
	case ATM_RELEASE:
	    success = FALSE;
	    callId = msg->u.release.call_id;
	    break;
	case ATM_RELEASE_COMPLETE:
	    success = FALSE;
	    callId = msg->u.release_comp.call_id; 
	    break;
	default:
	    LECS_ERR("lowerLecssSignallingCB: "
		"unexpected opcode %s",
		atmSig_printOpcode(msg->opcode));
	    return;
    }
    LECS_DBG(ifname,
	"lowerLecssSignallingCB activated "
	"with opcode %s callId 0x%x",
	atmSig_printOpcode(msg->opcode), callId);
    
    /* always acknowledge this */
    if (msg->opcode == ATM_RELEASE)
	lecsRelease(TRUE, callId, ATM_NORMAL_UNSPECIFIED, 
	    msg->idb);

    /* find corresponding LECS address */
    pinfo = getLecsPointer(msg->idb);
    if (pinfo == NULL) return;
    glecs = NULL;
    for (i=0; i<pinfo->globalLecsCount; i++) {
	glecs = &pinfo->globalLecsTable [i];
	if ((glecs->callType == OUTGOING_CALL) &&
	    (glecs->u.callId == callId)) 
	{
	    found = TRUE;
	    break;
	}
    }

    /*
    ** if we are getting a connect to a setup we issued
    ** a lllooong time ago such that we cannot find its 
    ** call id now, just release it now.
    */
    if (! found) {
	LECS_DBG(ifname, 
	    "sig message %s with callId 0x%x is UN-related", 
	    atmSig_printOpcode(msg->opcode), callId);
	if (success)
	    lecsRelease(FALSE, callId, ATM_NORMAL_UNSPECIFIED,
		msg->idb);
	return;
    }

    /* record it */
    if (success) {
	LECS_DBG(ifname,
	    "connected to lower LECS %Ch", &glecs->hwaddr);
	glecs->connected = TRUE;
    } else {
	LECS_DBG(ifname,
	    "release from lower LECS %Ch", &glecs->hwaddr);
	glecs->callType = NO_CALL;
	glecs->u.callId = NULL;
	glecs->connected = FALSE;
    }
}

/*
** every LECS should connect to lower than itself
** LECSs, if not already connected.  So ensure that.
** If the "pinfo" is NULL, then the operation is performed
** on EVERY lecs, otherwise only on the specified
** lecs.
**
** Make sure that this LECS does NOT connect to itself
** if it has multiple addresses.
*/
static void connectToLowerLecss (LecsInfo *pinfo)
{
    LecsInfo *first, *last;
    GlobalLecsAddress *glecs;
    int localRank, i;
    LecsAddress *highest;

    if (pinfo == NULL) {
	first = List_head(G_LecsInfosList);
	last = List_tail(G_LecsInfosList);
    } else {
	first = last = pinfo;
    }

    /* now do it */
    while (first) {

	/* it must be running and not locked */
	if (! first->lecsRunning || first->locked) {
	    LECS_DBG(first->name,
		"can't connect to lower lecss: "
		"not running or interface locked");
	    goto NEXT_LECS;
	}

	localRank = myHighestRank(first, &highest);

	/*
	** note that at startup, until the ILMI prefix is available,
	** this value may not be set correctly.  If so, bom out now.
	*/
	if (highest == NULL) goto NEXT_LECS;

	/* connect to everyone lower than me */
	for (i=localRank+1; i<first->globalLecsCount; i++) {
	    glecs = &first->globalLecsTable [i];

	    /* if already connect-ing/ed move on */
	    if (glecs->callType != NO_CALL) continue;

	    /* if this is another one of MY addresses, move on */
	    if (searchLecsNsapActualValues(first, glecs->hwaddr.addr)) {
		LECS_DBG(first->name,
		    "cannot connect to myself %Cj",
		    glecs->hwaddr.addr);
		continue;
	    }
	    
	    /* ok connect to this one */
	    LECS_DBG(first->name,
		"connecting to lower LECS %Ch", &glecs->hwaddr);
	    glecs->callType = OUTGOING_CALL;
	    glecs->u.callId = 
		connect_to_target(first->lane_info->swidb,
		    &highest->actualValue, 
		    &glecs->hwaddr, LANE_CONFIGURE,
		    lowerLecssSignallingCB);

	    /* did it succeed ? */
	    if (glecs->u.callId == NULL) {
		LECS_ERR("connectToLowerLecss: "
		    "connect_to_target %Ch failed", &glecs->hwaddr);
		glecs->callType = NO_CALL;
	    }
	}
    NEXT_LECS:
	if (first == last) break;
	first = List_next(first);
    }
}

/*
** callback which is invoked when the finder request
** for getting the LECS addresses from the ILMI completes.
*/
static void getLecsAddressesCB (idbtype *swidb, uint tid,
    void *context, int count, hwaddrtype *lecs_addresses)
{
    int i;
    LecsInfo *pinfo;
    GlobalLecsAddress *glecs;
    LecsAddress *lecs;
    ElanInfoBlock *pElan;
    Connection *pconn;

    if (!context) LECS_CRASH();
    pinfo = (LecsInfo*) context;
    if (! pinfo->lecsRunning) { 
	lecsUnlockInterface(pinfo);
	return;
    }

    /* see if an error occured in "getting" the LECS addresses */
    if (count < 0) {
	switch (count) {
	    case INTERNAL_ERROR:
		LECS_WARN("%s: internal error "
		    "getting LECS addresses", pinfo->name);
		break;
	    case ILMI_PROBLEM:
		LECS_DBG(pinfo->name,
		    "ILMI timeout getting LECS addresses");
		break;
	    default:
		LECS_ERR("getLecsAddressesCB: "
		    "unknown error %d", count);
	}
	lecsUnlockInterface(pinfo);
	return;
    }

    /* has the LECS addresses on the switch really changed ? */
    if (sameGlobalLecsAddresses(pinfo, count, lecs_addresses)) {
	LECS_DBG(pinfo->name,
	    "no change in global LECS addresses");
	lecsUnlockInterface(pinfo);
	goto CONNECT_TO_LOWER;
    }

    /*
    ** if we are here, then global lecs addresses on the 
    ** switch must have changed.  therefore, we have to 
    ** tear down all the so far existing LECS connections,
    ** store the new addresses in and reconnect to the
    ** lower ones immediately.
    */

    LECS_DBG(pinfo->name,
	"global LECS addresses on the switch have changed");

    /* tear down ALL existing (below and above) connections */
    tearDownLecsConnections(pinfo);

    /* 
    ** copy in the new global LECS addresses.
    **
    ** As we do this however, check these:
    **   - ensure that this is not also an LES address.
    **   - ensure that this is not also an LEC address.
    **   - ensure that an incoming call from that address
    **	   is actually torn down, if it does not look like
    **	   it is already from an LECS.
    */
    pinfo->globalLecsCount = count;
    for (i=0; i<count; i++) {
	glecs = &pinfo->globalLecsTable [i];
	glecs->hwaddr = lecs_addresses [i];

	/* check that this is NOT also an LES address */
	pElan = getElanByLesAddress(pinfo->cfgTable, 
		    &glecs->hwaddr, NULL);
	if (pElan != NULL) {
	    LECS_WARN("%s: global LECS address %Ch "
		"is also an LES address for elan '%s'", 
		pinfo->name, &glecs->hwaddr, pElan->elanName);
	}

	/* check that this is NOT also an LEC address */
	if (pinfo->cfgTable) {
	    pElan = searchLEC_InUnmaskedList(pinfo->cfgTable,
			glecs->hwaddr.addr, pinfo->name);
	    if (pElan != NULL)
		LECS_WARN("%s: global LECS address %Ch "
		    "is also an LEC address for elan '%s'", 
		    pinfo->name, &glecs->hwaddr, pElan->elanName);
	}

	/*
	** Check that we do NOT already have an incoming
	** call from this guy, which looks like NOT an LECS.
	** This happens at startup when different routers
	** get different LECS addresses from the ILMI, if
	** and when the switches are not powered up yet, etc.
	** or ILMI gave us incorrect addresses, which has been
	** known to happen.  If this happens, tear that call
	** down.
	*/
	pconn = ConnList_searchByCallingParty(pinfo, NULL,
		    &glecs->hwaddr);
	if (pconn && (pconn->callerType != CALLER_IS_LECS))
	    ConnList_destroyByPtr(pinfo, pconn, DO_RELEASE);

	glecs->callType = NO_CALL;
	glecs->connected = FALSE;
	glecs->u.callId = glecs->u.pconn = NULL;
	LECS_DBG(pinfo->name,
	    "new global LECS: %Ch", &glecs->hwaddr);
    }
    lecsUnlockInterface(pinfo);

    /* try and catch config errors if you can */
    if ((count > 0) && LecsAddress_atLeastOneSearchable(pinfo)) {
	myHighestRank(pinfo, &lecs);
	if (!lecs)
	    LECS_WARN("%s: LECS here NOT in "
		"the switch list", pinfo->name);
    }

CONNECT_TO_LOWER:

    /* now connect to all below me */
    connectToLowerLecss(pinfo);
}

/*
** this routine returns if there are any outstanding
** UN-finished signalling requests available, to the
** lower LECSs.  If so, while they are ongoing, we should
** not update the table.
*/
static boolean pendindDownwardConnections (LecsInfo *pinfo)
{
    int i;
    GlobalLecsAddress *glecs;

    glecs = &pinfo->globalLecsTable [0];
    for (i=0; i<pinfo->globalLecsCount; i++, glecs++) {
	if ((glecs->callType == OUTGOING_CALL) && 
	    !glecs->connected)
		return TRUE;
    }
    return FALSE;
}

/*
** use the ILMI and get the global LECS addresses
** from the system.  This routine just puts out
** the request to the LECS Finder to get the 
** addresses.  It is the callback routine that
** actually does the work.
*/
static void getGlobalLecsAddresses (LecsInfo *pinfo)
{
    LecsInfo *first, *last;

    /* record the time of the last update */
    get_timestamp32(&lecsAddrsUpdateTime);

    if (pinfo) {
	first = last = pinfo;
    } else {
	first = List_head(G_LecsInfosList);
	last = List_tail(G_LecsInfosList);
    }
    while (first) {
	if (first->lecsRunning) {
	    /*
	    ** if there are still uncomplete signalling
	    ** transactions to the lower LECSs, do not
	    ** upset them, get the LECS addresses NEXT time
	    */
	    if (pendindDownwardConnections(first)) {
		LECS_DBG(first->name,
		    "cannot get global LECS addresses from ILMI; "
		    "un-finished downward sig activity remains");
	    } else {
		LECS_DBG(first->name,
		    "getting global LECS addresses from ILMI");
		lecsLockInterface(first);
		lecsFinderRequest(first->lane_info->swidb,
		    GET_ALL_GLOBAL_LECSS, first, 
		    getLecsAddressesCB,
		    MAX_GLOBAL_LECS_COUNT);
	    }
	}
	if (first == last) break;
	first = List_next(first);
    }
}

/* smallest granularity */
#define BASE_TIME		5

/*
** every so many specified seconds, try and connect to all the
** LECSs lower than myself, if I haven't already connected to 
** them so far.  Make sure that this number is EXACTLY divisible
** by BASE_TIME.
*/
#define CHECK_RECONNECT_DOWN	5 

/* is it time to try and reconnect to lower LECSs ? */
static boolean timeForConnectingToLowerLecss (void)
{
    return
	(timerTicks % (CHECK_RECONNECT_DOWN / BASE_TIME) == 0);
}

/*
** every so many specified seconds, try and re-register any LECS
** addresses of mine, which have not yet been registered with 
** either the signalling or the ILMI module.  Make sure that 
** this number is EXACTLY divisible by BASE_TIME.
*/
#define CHECK_REREGISTER	30 

/* is it time to try and reregister unregistered addresses ? */
static boolean timeToReregisterAddresses (void)
{
    return
	(timerTicks % (CHECK_REREGISTER / BASE_TIME) == 0);
}

/*
** every so many specified seconds, go out to the ILMI and
** get the list of global LECS addresses to be used in 
** mastership determination process.  Make sure that 
** this number is EXACTLY divisible by BASE_TIME.
*/
#define CHECK_ILMI_LECS		60

/* is it time to go out to the ILMI for global LECS addresses ? */
static boolean timeForGettingLecsAddresses (void)
{
    return
	(timerTicks % (CHECK_ILMI_LECS / BASE_TIME) == 0);
}

/*
** how long remaining to update the global LECS addresses
** from the ILMI and the switch again.
*/
int timeRemainingToUpdateGlobalLecsAddresses (void)
{
    ulong timeNow;
    ulong msecsleft;

    get_timestamp32(&timeNow);
    msecsleft = lecsAddrsUpdateTime + (CHECK_ILMI_LECS*ONESEC) - 
		    timeNow;
    return msecsleft/ONESEC;
}

/*
** this is the main LECS process
*/

static ulong totalRequests = 0;
static ulong totalTime = 0;

static process lecsServer (void)
{
    paktype *pak;
    int count;
    mgd_timer *expired;
    ServerAtmAddress *psaa;

    /* wait until system comes up */
    process_wait_on_system_config();

    /* set triggers */
    process_watch_queue(lecsInputQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&lecsMasterTimer, ENABLE);

    /* start the periodic timer */
    mgd_timer_start(&lecsPeriodic, BASE_TIME*ONESEC);
    timerTicks = 0;

    /* main loop */
    for (;;) {
	process_wait_for_event();

	/* process expired timers */
	count = PAK_SWITCHCOUNT;
	expired = mgd_timer_first_expired(&lecsMasterTimer);
	while (expired && (--count >= 0)) {
	    mgd_timer_stop(expired);
	    switch (mgd_timer_type(expired)) {
		
		/* an LES disconnected */
		case LES_DISCONNECTED_TIMER:
		    psaa = mgd_timer_context(expired);
		    LECS_DBG(NULL, "LES %Ch grace timer expired", 
			&psaa->lesAddr);
		    disconnect_LES(psaa);
		    break;

		/* periodic things to do */
		case GENERIC_TIMER:
		    /* restart it again for next time */
		    mgd_timer_start(&lecsPeriodic, BASE_TIME*ONESEC);
		    
		    /* do this every time */
		    checkMastership();

		    /*
		    ** NEVER do these together becoz we do NOT
		    ** want to change addresses while we are trying
		    ** to connect down or vice versa.  It is
		    ** either one or the other.
		    */
		    if (timeForGettingLecsAddresses())
			getGlobalLecsAddresses(NULL);
		    else if (timeForConnectingToLowerLecss())
			connectToLowerLecss(NULL);

		    if (timeToReregisterAddresses())
			revisitLecs(NULL);

		    /* update the seconds */
		    timerTicks++;

		    /* dont forget this one */
		    break;

		default:
		    LECS_CRASH();
	    }
	    expired = mgd_timer_first_expired(&lecsMasterTimer);
	}
	
	/* be courteous */
	process_may_suspend();

	/* now process input config request packets */
	for (count=0; count<PAK_SWITCHCOUNT; count++) {

	    /* for performance measurement */
	    ulong start = 0, end = 0;

	    pak = process_dequeue(lecsInputQ);
	    if (pak == NULL) break;
	    get_timestamp32(&start);
	    lecsProcessSinglePacket(pak);
	    get_timestamp32(&end);
	    totalRequests++;
	    totalTime += (end-start);
	    LECSP_DBG(NULL, 
		"time units used: %u, "
		"total requests: %u, "
		"total time units: %u",
		(end-start), totalRequests, totalTime);
	}
    }
}

/*
** this routine tears down EVERY SVC for a specified LecsInfo.
** If specified LecsInfo is NULL, then EVERY LecsInfo will be done.
** It is called just before the lecs server is brought down.
** Note that ONLY the SVCs are brought down, NONE of the
** actual nsap addresses are touched in any other way.
**
** If "includeLecsConnexions" is set, the connections 
** to/from other LECSs are also torn down as well.
*/
static void release_all_circuits (LecsInfo *pinfo, 
    boolean includeLecsConnexions)
{
    LecsInfo *first, *last;
    Connection *pconn, *deleter;
    int idx;

    if (pinfo == NULL) {
	first = List_head(G_LecsInfosList);
	last = List_tail(G_LecsInfosList);
    } else {
	first = last = pinfo;
    }

    while (first != NULL) {
	LECS_DBG(first->name,
	    "tearing down all %sconnexions",
	    includeLecsConnexions ? "" : "NON LECS ");

	/* release appropriate connections */
	pconn = List_head(first->connectionList);
	while (pconn) {
	    deleter = pconn;
	    pconn = List_next(pconn);

	    /*
	    ** note that for an LES or an LECS, we must call
	    ** the appropriate routines to tear their connections
	    ** since they take care of proper bookkeeping.
	    */
	    switch (deleter->callerType) {
		case CALLER_IS_LES:
		    if (deleter->lesAddress) 
			disconnect_LES(deleter->lesAddress);
		    break;

		/* incoming LECS calls are taken care of here */
		case CALLER_IS_LECS:
		    if (includeLecsConnexions) {
			idx = deleter->index;
			if ((idx >= 0) &&
			    (idx < first->globalLecsCount)) {
			    disconnect_LECS(first, 
				&first->globalLecsTable [idx]);
			}
		    }
		    break;

		default:
		    lecsRelease(FALSE, deleter->callId,
			ATM_INVALID_ENDPOINT, first->hwidb->firstsw);
	    }
	}

	/* the outgoing LECS calls are taken care of here */
	if (includeLecsConnexions)
	    tearDownLecsConnections(first);

	/* next interface ? */
	if (first == last) break;
	first = List_next(first);
    }
}

/*
** register or deregister all the appropriate 
** lecs nsaps on a specified LecsInfo.  If the specified
** LecsInfo is NULL, then ALL the LecsInfos will be done.
** If "registerThem" is TRUE, they are all registered, otherwise 
** deregistered.
*/
static void register_ALL_LecsNsaps (LecsInfo *pinfo, 
    boolean registerThem)
{
    LecsInfo *first, *last;
    LecsAddress *lecsAddr;

    if (pinfo == NULL) {
	first = List_head(G_LecsInfosList);
	last = List_tail(G_LecsInfosList);
    } else {
	first = last = pinfo;
    }

    while (first != NULL) {
	lecsAddr = List_head(first->lecsAddressesList);
	while (lecsAddr != NULL) {
	    if (registerThem)
		registerLecsNsap(lecsAddr);
	    else
		deRegisterLecsNsap(lecsAddr);
	    lecsAddr = List_next(lecsAddr);
	}
	if (first == last) break;
	first = List_next(first);
    }
}

/*
** is there at least a "hint" of an lecs on this interface ?
** This is so if there is EITHER a config table OR at least 
** one lecs nsap address bound to the interface.
*/
boolean hintOfLecs (LecsInfo *pinfo)
{

    if (pinfo == NULL) return FALSE;
    return
	(pinfo->cfgTable != NULL) ||
	(List_size(pinfo->lecsAddressesList) > 0);
}

/*
** is lecs reachable from the outside ?
** To be so, it must have at least one fully registered
** address.
*/
boolean lecsIsReachable (LecsInfo *pinfo)
{
    LecsAddress *lecsAddr;
    
    if (pinfo == NULL) return FALSE;
    lecsAddr = List_head(pinfo->lecsAddressesList);
    while (lecsAddr != NULL) {
	if ((lecsAddr->ilmiState == REGISTERED) &&
	    (lecsAddr->sigState == REGISTERED)) return TRUE;
	lecsAddr = List_next(lecsAddr);
    }
    return FALSE;
}

/*
** should the LECS be started on this interface ?
** This is so if the interface is up AND a config table
** AND at least one lecs nsap address is available (NOT 
** necessarily registered) on the interface.  
*/
static boolean lecsShouldRunOnThisInterface (LecsInfo *pinfo)
{
    if (!pinfo) return FALSE;
    if (pinfo->shutDown) return FALSE;
    if (!pinfo->cfgTable) return FALSE;
    return (List_size(pinfo->lecsAddressesList) > 0);
}

/*
** start the LECS IOS process if NOT already running
*/
static void startLecsServerProcess (void)
{
    if (G_lecsServerPid == NO_PROCESS) {
	G_lecsServerPid = process_create(lecsServer, "LECS Server",
	    LARGE_STACK, PRIO_NORMAL);
	if (G_lecsServerPid != NO_PROCESS)
	    LECS_DBG(NULL, "LANE config server started (pid=%d)", 
		G_lecsServerPid);
    }
}

/*
** kill the LECS IOS process if running
*/
static void killLecsServerProcess (void)
{
    paktype *pak;
    pid_t pid;

    /* always do this */
    mgd_timer_stop(&lecsMasterTimer);

    /* if the IOS process is running, kill it */
    if (G_lecsServerPid != NO_PROCESS) {

	/*
	** do these BEFORE the process_kill,
	** in case we are comitting suicide
	*/
	pid = G_lecsServerPid;
	G_lecsServerPid = NO_PROCESS;

	/* 
	** Discard all awaiting packets.
	** Do this AFTER setting the server
	** pid to NO_PROCESS, so that any
	** packets in the limbo are assured
	** NOT to be permanently stuck in the
	** process queue
	*/
	pak = process_dequeue(lecsInputQ);
	while (pak != NULL) {
	    datagram_done(pak);
	    pak = process_dequeue(lecsInputQ);
	}

	/* now kill it */
	LECS_DBG(NULL, "config server %d is killed", pid);
	process_kill(pid);
    }
}

/*
** start a *single* LECS entity
*/
static void startLecs (LecsInfo *pinfo)
{
    /* always start the IOS process in case it is not running */
    startLecsServerProcess();

    /* a real change ? */
    if (!pinfo->lecsRunning) {

	/* notify that we are up */
	errmsg(&msgsym(UPDOWN, LANE), pinfo->name, "database",
	    pinfo->cfgTable->tableName,
	    "LE Config Server (LECS)", 
	    "up");

	/* clear these out */
	pinfo->globalLecsCount = 0;
    }
    pinfo->lecsRunning = TRUE;

    /* register all nsaps */
    register_all_nsaps(pinfo);

    /* re-evaluate our mastership status */
    evaluateLecsMastership(pinfo);
}

/*
** stop a *single* LECS entity
*/
static void stopLecs (LecsInfo *pinfo)
{
    char *dbasename = "?";

    /* a real change ? */
    if (pinfo->lecsRunning) {
	
	/* notify that we are down */
	if (pinfo->cfgTable)
	    dbasename = pinfo->cfgTable->tableName;
	errmsg(&msgsym(UPDOWN, LANE), pinfo->name, "database",
	    dbasename,
	    "LE Config Server (LECS)", 
	    "down");
    }

    /* set these flags correctly */
    pinfo->lecsRunning = FALSE;
    pinfo->thisLecsIsTheMaster = FALSE;

    /* deregister all the LECS nsaps */
    deregister_all_nsaps(pinfo);

    /* release all vc's INLCUDING ones from other LECSs */
    release_all_circuits(pinfo, TRUE);

    /* This must be done LAST */
    pinfo->globalLecsCount = 0;
}

/*
** decide the fate of an LECS or ALL LECSs in the system.
** If an LECS is specified, then ONLY that one is checked,
** otherwise if NULL is specified, ALL LECSs are checked.
*/
static void revisitLecs (LecsInfo *pinfo)
{
    LecsInfo *first, *last;
    boolean lecsServerShouldDie;

    if (pinfo) {
	first = last = pinfo;
	lecsServerShouldDie = FALSE;
    } else {
	first = List_head(G_LecsInfosList);
	last = List_tail(G_LecsInfosList);
	lecsServerShouldDie = TRUE;
    }

    while (first) {
	if (lecsShouldRunOnThisInterface(first)) {
	    lecsServerShouldDie = FALSE;
	    startLecs(first);
	} else {
	    stopLecs(first);
	}
	if (first == last) break;
	first = List_next(first);
    }

    /* do we need to kill the lecs server ? */
    if (lecsServerShouldDie) {
	killLecsServerProcess();
    }
}



