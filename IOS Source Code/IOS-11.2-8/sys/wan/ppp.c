/* $Id: ppp.c,v 3.39.4.62 1996/09/15 22:20:53 fox Exp $
 * $Source: /release/112/cvs/Xsys/wan/ppp.c,v $
 *------------------------------------------------------------------
 * ppp.c -- routines for the ppp protocol.
 *
 * 2-January-1990, Vicki Ralls
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ppp.c,v $
 * Revision 3.39.4.62  1996/09/15  22:20:53  fox
 * CSCdi66416:  PPP MRU negotiation causes interoperability problems
 * Branch: California_branch
 * Make sure that PPP interface is in received for at least 1500 bytes.
 * Request peer to not send more than the configured MTU.
 * Accept NAK of our MRU as long as it fits in our buffer size.
 * Accept peer's MRU as long as it is >= the default MRU.
 *
 * Revision 3.39.4.61  1996/09/15  22:09:04  fox
 * CSCdi69056:  PPP header compression rejected on sync interfaces
 * Branch: California_branch
 * Accept PPP header compression (Address/Control and Protocol Field
 * compression) on all PPP interfaces, not just async PPP interfaces.
 * We will ignore the peer's 'ability' on sync interfaces as the RFC
 * does not require that we compress just because they say they can
 * handle it.
 *
 * Revision 3.39.4.60  1996/09/10  00:52:35  snyder
 * CSCdi68568:  more constant opportunities, save memory
 *              156 out of image,
 *              116 out of data section
 * Branch: California_branch
 *
 * Revision 3.39.4.59  1996/09/06  16:46:53  dblair
 * CSCdi68304:  Pings fail when multilink ppp is negotiated with 1 link in
 * the bundl
 * Branch: California_branch
 * Reorder mlp_raw_enqueue to handle the case where LINK_PPP is an IP
 * packet but IPCP is not up yet
 *
 * Revision 3.39.4.58  1996/09/06  03:48:58  fox
 * CSCdi67375:  ppp pap sent-username must be configured twice
 * Branch: California_branch
 * Copy chap and pap strings in copy_ppp_fields() like all other LCP
 * and authentication parameters.
 *
 * Revision 3.39.4.57  1996/09/03  23:14:39  fox
 * CSCdi67680:  PPP Code Rejects are mal-formed
 * Branch: California_branch
 * Simply prepend a Code Reject header and return the original message.
 *
 * Revision 3.39.4.56  1996/08/30  08:40:53  fox
 * CSCdi66686:  Shivas NBFCP protocol (0x8fec) is not properly rejected
 * Branch: California_branch
 * Protocol reject all unknown protocols and unsupported NCPs.  Reject
 * broken Shiva protocol #s using the full protocol number.  Fix
 * support for protocol rejecting packets with compressed protocol #s.
 *
 * Revision 3.39.4.55  1996/08/28  13:22:21  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.39.4.54  1996/08/27  20:39:17  sbelair
 * CSCdi60087:  AAA UNKNOWN for protocol 0x8027
 * Branch: California_branch
 * In fsm_do_author(), add cases for osip and deccp protocols.
 *
 * Revision 3.39.4.53  1996/08/27  18:10:14  fox
 * CSCdi58749:  LQM failure does not generate trace messages
 * Branch: California_branch
 * Add a buginf to report lost LQRs.  Cleanup a seq wrap test.
 *
 * Revision 3.39.4.52  1996/08/26  01:18:00  fox
 * CSCdi67087:  PPP gives up on all options not just those NAKd
 * Branch: California_branch
 * If we have sent too many NAKs then REJ the options that we would
 * have NAK'd.
 *
 * Revision 3.39.4.51  1996/08/25  21:40:16  dblair
 * CSCdi67072:  LCP PROTREJ not handled correctly if using multilink
 * Branch: California_branch
 * Forward Protocol Reject messages to the bundle interface idb
 *
 * Revision 3.39.4.50  1996/08/23  23:39:22  fox
 * CSCdi66321:  ACCM should be ignored on sync lines, not disallowed
 * Branch: California_branch
 * Allow the peer to negotiate an ACCM on all PPP interfaces.  It will
 * be ignored on sync/ISDN interfaces, but it will allow async/sync PPP
 * converters to operate.
 *
 * Revision 3.39.4.49  1996/08/23  20:38:06  syiu
 * CSCdi66741:  Multichassis or VPDN multilink broken
 * Branch: California_branch
 *
 * Revision 3.39.4.48  1996/08/23  04:25:43  fox
 * CSCdi66589:  ppp_periodic() dereferences null pointer - idb->lcpfsm
 * during init
 * Branch: California_branch
 * Disable interrupts in isdn_setencap(), dialer_setencap() and
 * async_setencap() before changing encapsulations.  Make sure that
 * lcpfsm pointer in idb is set before idb vectors (such as periodic)
 * are set.
 *
 * Revision 3.39.4.47  1996/08/21  19:18:03  fox
 * CSCdi57429:  Router cannot distinguish between FCS bit set/not set in
 * PPP header
 * Branch: California_branch
 * Add support for RFC 1638 bridging packet header fields.  Bounce packets
 * to process level if any fields are set.
 *
 * Revision 3.39.4.46  1996/08/18  09:08:36  fox
 * CSCdi58100:  PPP option types displayed inconsistently
 * Branch: California_branch
 * Consistently display PPP codes and types in decimal.  Convert several
 * occurrences of '0x%x' to '%#x'.
 *
 * Revision 3.39.4.45  1996/08/16  03:24:06  fox
 * CSCdi56478:  PPP sessions do not send CHAP failure on vty-async
 * Branch: California_branch
 * Delay bringing down the NCPs for a VTY that is being closed.  This
 * will give the TERM REQ a chance to get out the door.
 *
 * Revision 3.39.4.44  1996/08/15  04:56:41  fox
 * CSCdi63702:  ip route thrashs on MLP master bundle interface
 * Branch: California_branch
 * Do not mark multiple MLP interfaces UP.  Prevent NCP activity
 * unless the interface is UP.
 *
 * Revision 3.39.4.43  1996/08/14  14:19:14  dblair
 * CSCdi64968:  Implment outbound half-fastswitching for multilink PPP
 * Branch: California_branch
 * The multilink half-fastswitching performance improvement
 *
 * Revision 3.39.4.42  1996/08/14  02:28:39  fox
 * CSCdi59175:  PPP renegotiates during disconnection (TERMREQ/TERMACK)
 * Branch: California_branch
 * Delay LCP active open during LCP close process by one timeout period.
 *
 * Revision 3.39.4.41  1996/08/08  20:37:54  syiu
 * CSCdi65530:  Multichassis Multilink breaks with endpoint discr
 * Branch: California_branch
 *
 * Revision 3.39.4.40  1996/08/03  23:42:08  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.39.4.39  1996/07/30  20:18:28  dblair
 * CSCdi59050:  multilink broken when dialing Ascend with 2 phone numbers
 * Branch: California_branch
 * Adding support for PPP LCP endpoint discriminator
 *
 * Revision 3.39.4.38  1996/07/28  07:33:24  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.39.4.37  1996/07/24  16:22:57  syiu
 * CSCdi63910:  Bridging not working with VPDN
 * Branch: California_branch
 *
 * Revision 3.39.4.36  1996/07/23  13:31:55  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.39.4.35  1996/07/19  23:48:28  snyder
 * CSCdi63585:  ppp fcstab array to const
 *              512 bytes out of data space, 12 image
 * Branch: California_branch
 *
 * Revision 3.39.4.34  1996/07/08  19:01:30  fox
 * CSCdi59213:  Effects of authentication configuration are delayed
 * Branch: California_branch
 * Renegotiate LCP if PPP authentication is reconfigured.  PPP
 * authentication follows LCP negotiation.
 *
 * Revision 3.39.4.33  1996/07/01  21:27:53  syiu
 * CSCdi61806:  Multichassis MLP and VPDN should coexist
 * Branch: California_branch
 *
 * Revision 3.39.4.32  1996/07/01  20:47:29  fox
 * CSCdi61571:  Local Timeout at _ppp_getlink(0x303d730+0x1fff82)+0x160
 * Branch: California_branch
 * Virtual PPP interfaces may not be complete.  Sanity check existence
 * of NCP FSM pointer table.
 *
 * Revision 3.39.4.31  1996/06/27  09:45:26  fox
 * CSCdi61238:  linktype is an overloaded symbol
 * Branch: California_branch
 * Change idb->linktype to idb->getlink.
 *
 * Revision 3.39.4.30  1996/06/25  22:47:32  snyder
 * CSCdi61272:  fix embedded University copyrights in tcpcompress and ppp
 * Branch: California_branch
 *      change char* to static const char array, being anal I know...
 *      fixing misspellings did it to me
 *
 * Revision 3.39.4.29  1996/06/24  09:28:11  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.39.4.28  1996/06/22  07:38:31  syiu
 * CSCdi59165:  PPP Virtual-Access Int will not negotiate NCPs on
 * projected links
 * Branch: California_branch
 *
 * Revision 3.39.4.27  1996/06/21  16:06:13  wmay
 * CSCdi60209:  2500 Router Crash PPP Bridge Config - reset atalk
 * interface when ppp bridging gets set/cleared - this will set the
 * correct atalk multicast address.
 * Branch: California_branch
 *
 * Revision 3.39.4.26  1996/06/19  01:47:21  dblair
 * CSCdi58636:  DDR PPP Callback not working using async interface
 * Branch: California_branch
 *
 * Revision 3.39.4.25  1996/06/06  22:52:11  wmay
 * CSCdi58782:  down line stays stuck looped - clear inloopback field when
 * changing state
 * Branch: California_branch
 *
 * Revision 3.39.4.24  1996/05/30  18:03:46  fox
 * CSCdi57978:  calls stay up after line protocol of b channel go
 * down(under stress)
 * Branch: California_branch
 * Call lcp_close() instead of lcp_down() when max CHAP timeouts reached.
 * Save copy of CHAP Response packet for next Authentication timeout.
 * Resend if no CHAP Success or Failure packet received.
 *
 * Revision 3.39.4.23  1996/05/28  16:21:01  vandys
 * CSCdi50153:  systat does not display ISDN access users
 * Branch: California_branch
 *
 * Revision 3.39.4.22  1996/05/28  16:10:18  fox
 * CSCdi58271:  SLIP reports interface as looped
 * Branch: California_branch
 * Put inloopback change in the right function.
 *
 * Revision 3.39.4.21  1996/05/28  15:02:50  fox
 * CSCdi58271:  SLIP reports interface as looped
 * Branch: California_branch
 * Clear the inloopback flag when deencapsulating HDLC and PPP.
 *
 * Revision 3.39.4.20  1996/05/28  13:35:42  fox
 * CSCdi57337:  Remote PAP authentication broken for AAA/TACACS+ configs
 * Branch: California_branch
 * Re-enable support for AAA/TACACS+ to provide the remote PAP password.
 * If 'pap sent-username' is configured use that, else try AAA/TACACS+.
 * AAA has been blocked from getting the password from the local database.
 *
 * Fix two bugs in LCP negotiation of Shiva PAP (SPAP).  One, prevent
 * mal-formed responses by ensuring that the pointer is at the end of the
 * option.  Two, NAK with CHAP/MD5 if SPAP option is received.(CSCdi58734)
 *
 * Revision 3.39.4.19  1996/05/22  17:21:49  fox
 * CSCdi57795:  Unable to change a BRI interface from ppp to hdlc.
 * Branch: California_branch
 * Don't clear idb->enctype in ppp_deencap(), slip_deencap() or
 * bstun_deencapsulate().  Move the setting of idb->encaptype to
 * the start of the encapsulation setup functions because calls to
 * other function use this value.
 *
 * Revision 3.39.4.18  1996/05/21  10:09:46  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.39.4.17  1996/05/19  05:49:02  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.39.4.16  1996/05/17  12:16:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.35.2.4  1996/05/06  00:09:45  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.35.2.3  1996/04/26  07:59:53  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.35.2.2  1996/04/03  22:13:23  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.35.2.1  1996/03/17  18:54:44  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.39.4.15  1996/05/13  14:48:20  fred
 * CSCdi56825:  RSP box crashes when configuring a virtual interface
 *         check for cbus/mci usage before invoking cbus_mci_txlimit().
 *         Also, change name of serial_cfg_modified registry to
 *         interface_cfg_modified
 * Branch: California_branch
 *
 * Revision 3.39.4.14  1996/05/11  06:11:01  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Attach lex_ncp subsystem to lex subsystem as lex requires PPP.
 * Add registry calls to eliminate PPP's dependancy on CCP/Compress.
 *
 * Revision 3.39.4.13  1996/05/10  01:45:18  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.39.4.12  1996/05/06  17:03:31  fox
 * CSCdi49278:  Update PAP handling of outbound calls
 * Branch: California_branch
 * Three bug fixes in one!
 * Disable remote PAP authentication support by default.  Add 'ppp pap
 * sent-username xxx password yyy' to re-enable support if needed.  Warn
 * if sent-username/password matches entry in local database. (CSCdi49278)
 * Add 'ppp chap hostname xxx' command to allow multiple boxes to use the
 * same hostname in their CHAP Challenges.  (CSCdi44884)
 * Add 'ppp chap password yyy' command to allow a box to respond to many
 * unknown peers with the same password.  (CSCdi54088)
 *
 * Revision 3.39.4.11  1996/05/04  02:17:13  wilber
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
 * Revision 3.39.4.10  1996/05/01  14:33:40  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Make PPP NCPs into real subsystems.  Add them to all images where
 * PPP and the related network protocol are located.  NCPs are dependant
 * on PPP and the related network protocol subsystem but not vice versa.
 * Remove dependancies between PPP and the compression code.
 *
 * Revision 3.39.4.9  1996/04/29  09:46:14  fox
 * CSCdi55358:  PPP subsystem needs to be more modular
 * Branch: California_branch
 * Move Network Control Protocol (NCP) support from PPP subsystem to
 * individual subsystems.  Attach NCP subsystems to respective protocol
 * subsystems.  Add registry calls to network protocol init code to
 * register default NCP support for NCPs not requiring option negotiation.
 *
 * Revision 3.39.4.8  1996/04/27  06:37:23  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.39.4.7  1996/04/24  03:00:07  lol
 * CSCdi55359:  TACACS events debug output displays handle (0X0) ????
 * Branch: California_branch
 * Cosmetic changes to T+ output formats.
 *
 * Revision 3.39.4.6  1996/04/21  02:24:35  fox
 * CSCdi55198:  PPP protocol number should be displayed as hex in AAA
 * traces
 * Branch: California_branch
 *
 * Revision 3.39.4.5  1996/04/20  17:59:31  lol
 * CSCdi46005:  ISDN CLID not propagated to rem_addr field of T+ packet
 * Branch: California_branch
 *
 * Revision 3.18.2.19  1996/04/25  23:59:57  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.18.2.18  1996/04/17  17:56:58  syiu
 * Branch: Dial1_branch
 * Make ppp registry depends work correctly
 *
 * Revision 3.18.2.17  1996/04/17  00:01:54  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.18.2.16  1996/04/06  21:26:57  dblair
 * Branch: Dial1_branch
 * Fix CSCdi53768 and added restart lcp when ppp multilink command
 * processed, and fix multilink not negotiated at reload problem.
 *
 * Revision 3.18.2.15  1996/03/29  15:55:03  dblair
 * Branch: Dial1_branch
 * Fix crash when clearing virtual interface and add some multilink knobs
 *
 * Revision 3.18.2.14  1996/03/24  01:42:14  tkolar
 * Branch: Dial1_branch
 * Remove the VPN authentication incursion from PPP.
 *
 * Revision 3.18.2.13  1996/03/21  23:50:54  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 3.18.2.12  1996/03/18  18:31:29  dblair
 * Branch: Dial1_branch
 * Fix ppp timer processing.  Allow NCP timer processing and prevent lcp
 * and authentication timer processing on bundle interfaces.
 *
 * Revision 3.18.2.11  1996/03/13  17:48:37  syiu
 * Branch: Dial1_branch
 * Renable L2F-PPP authentication mechanism
 *
 * Revision 3.18.2.10  1996/03/08  17:30:37  dblair
 * Branch: Dial1_branch
 * Change for multilink virtual bundle interfaces to support ISDN BRI,
 * PRI, and Dialer interfaces
 *
 * Revision 3.18.2.9  1996/02/23  21:31:52  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.18.2.8  1996/02/17  22:58:38  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 3.18.2.7  1996/02/07  20:23:22  syiu
 * Branch: Dial1_branch
 * Add MLP Stackability calls support
 *
 * Revision 3.18.2.6  1996/02/06  20:36:54  syiu
 * Branch: Dial1_branch
 * L2F enhancements for PPP
 *
 * Revision 3.18.2.5  1996/02/04  06:29:44  syiu
 * Branch: Dial1_branch
 * Support for MLP Bundle interfaces
 *
 * Revision 3.18.2.4  1996/01/31  18:37:27  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 3.18.2.3  1996/01/21  06:08:39  syiu
 * Branch: Dial1_branch
 * More PPP L2F support
 *
 * Revision 3.18.2.2  1996/01/19  02:15:44  syiu
 * Branch: Dial1_branch
 * PPP L2F support
 *
 * Revision 3.18.2.1  1996/01/16  15:45:21  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.39.4.4  1996/04/03  02:42:39  fox
 * CSCdi42544:  PPP sessions do not terminate correctly on vty-async
 * Branch: California_branch
 * Wait one timeout after sending TERMACK before closing LCP.
 * For VTYs, delay notifying upper layer until LCP is closed.
 *
 * Revision 3.39.4.3  1996/03/28  20:22:48  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * First commit missed the initialization.
 *
 * Revision 3.39.4.2  1996/03/27  10:36:35  irfan
 * CSCdi36933:  Customer desires configurable PAP/CHAP retries
 * Branch: California_branch
 * Add PPP UI to make us accept more than one PAP/CHAP Authentication
 * attempt. Instead of hanging up the physical layer right away.
 *
 * Revision 3.39.4.1  1996/03/18  22:48:19  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.23.2.5  1996/03/16  07:56:04  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.23.2.4  1996/03/13  02:12:27  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.23.2.3  1996/03/07  11:08:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.23.2.2  1996/02/20  21:52:42  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.39  1996/03/07  19:12:10  fox
 * CSCdi50935:  LCP Identification Code is Code Rejected
 * Trace and discard the packet as it is only an advisory for debugging
 *
 * Revision 3.38  1996/03/04  10:45:49  fred
 * CSCdi47191:  MBRI cannot disable WFQ - screws up multilink
 *         disable wfq if multilink is activ
 *
 * Revision 3.37  1996/03/01  18:25:57  fox
 * CSCdi50216:  crash in localuser_do_author (no ustruct)
 * Clear ncp-reset flag when the connection is closed
 *
 * Revision 3.36  1996/03/01  18:10:17  fox
 * CSCdi48111:  Loss of PPP echoes does not bring down the connection
 * Test ppp_lineup flag before it is cleared in lcp_down
 *
 * Revision 3.35  1996/02/22  05:21:04  fox
 * CSCdi35908:  Should allow CHAP and PAP clients to same dialin pool
 * Add support for multiple acceptable inbound PPP authentication methods
 *
 * Revision 3.34  1996/02/17  07:30:58  fox
 * CSCdi49221:  PPP packet debugging can result in an endless loop
 * Use a short in case you packet is bogus AND long.
 *
 * Revision 3.33  1996/02/09  18:17:06  dblair
 * CSCdi48535:  Rcv NAK of multilink MRRU option > 1600 removes MRRU from
 * confreq.  Continue to send MRRU of 1600 in conf req
 *
 * Revision 3.32  1996/02/09  09:03:27  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.31  1996/02/07  16:17:34  widmer
 * CSCdi48285:  Remove function name from bad_parser_subcommand
 *
 * Revision 3.30  1996/02/04  15:37:38  fox
 * CSCdi48127:  First option in packet should start on its own debug line
 * Put each option of a Configuration packet on its own line
 * Fix offset to avoid bogus options being displayed
 *
 * Revision 3.29  1996/02/03  05:38:43  fox
 * CSCdi45322:  debug ppp packet looped and finally crashed router
 * The value of len was mistakenly being changed to be greater than that
 * of the packet being printed.  This caused all sorts of garbage to be
 * displayed when debug was enabled, and could cause the router to hang
 * or crash.
 *
 * Revision 3.28  1996/02/01  06:12:27  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.27  1996/01/26  20:54:08  fox
 * CSCdi47637:  Extra authorization buginf on every LCP & NCP packet rcvd
 * Remove buginf in fsm_input() main path.
 *
 * Revision 3.26  1996/01/26  03:01:43  fox
 * CSCdi47152:  fsm_do_author() attempts to dismiss with blocking disabled
 * Passive open delayed if NCP has been Protocol Rejected
 *
 * Revision 3.25  1996/01/25  07:27:38  fox
 * CSCdi47152:  fsm_do_author() attempts to dismiss with blocking disabled
 * Step 1 - Don't bother waiting for NCP authorization if someone else has
 *          already queued an authorization request for the same NCP.
 * Step 2 - When triggering an NCP Active Open on the transmit path, delay
 *          until the next PPP timeout by moving the NCP state to LISTEN
 *          and setting a tiny timeout.  This avoids blocking in a task
 *          which has blocking disabled or an interrupt routine.
 *
 * Revision 3.24  1996/01/25  06:28:55  fox
 * CSCdi47481:  Can still pass IP packets after authen before author
 * Don't convert between internal and external types until ACLs are
 * loaded (if necessary).
 *
 * Revision 3.23.2.1  1996/01/24  23:31:50  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.23  1996/01/22  07:45:56  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.22  1996/01/20  05:24:07  fox
 * CSCdi47176:  Link is reset if IPCP retransmits exceed maxconfigure
 * Don't reset the link unless LCP times out
 *
 * Revision 3.21  1996/01/18  21:08:36  dblair
 * CSCdi47131:  Multilink bundle not created if peer uses caller id
 * and other authenticated.  Check for presence of string in
 * idb->number_called OR idb->remote_name in lcp_authcomplete
 *
 * Revision 3.20  1996/01/16  09:03:09  misko
 * CSCdi46888:  PPP doesnt allow a protocol to be added later
 * Fix the case where we've received a protocol reject, but then the
 * other side starts sending what they once rejected.  We should then
 * be allowed to send those packets also.  At the same time, apply
 * CSCdi29247 to 11.0.
 *
 * Revision 3.19  1996/01/15  01:42:59  misko
 * CSCdi41085:  Need nerd-knob to configure # of PPP CONFREQ retries
 * Nerd knob already existed, but is hidden for interfaces.  CSCdi35040
 * fixed this for vty's.  But needed to call reset when the limit
 * was reached so that dialup calls, etc. would disconnect.
 *
 * Revision 3.18  1996/01/12  15:52:08  fox
 * CSCdi46814:  lcp_up can be called twice
 * Don't take the second call if we are still authenticating.
 *
 * Revision 3.17  1996/01/10  06:48:27  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.16  1996/01/10  01:20:04  sjackson
 * CSCdi46336:  lcp negotation fails when ppp callback req and ppp
 * reliable confg
 * Avoid advancing pointer past the end of the reliable link CI
 *
 * Revision 3.15  1996/01/09  17:19:44  syiu
 * CSCdi46605:  Async Callback initiate obviating Callback configs
 *
 * Revision 3.14  1996/01/03  20:35:57  wmay
 * CSCdi38318:  cant specify inbound only ppp authentication - add callin
 * option to ppp authentication command
 *
 * Revision 3.13  1995/12/17  18:41:20  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.12  1995/12/11  14:20:17  dblair
 * CSCdi45019:  Previous commit broke modularity by calling ncp_lowerup
 * from mlp.c.  Added registry to fix modularity.
 *
 * Revision 3.11  1995/12/09  17:32:39  dblair
 * CSCdi45262:  If multilink, NCPs incorrectly restart when keepalive
 * timeout
 *
 * Revision 3.10  1995/12/08  18:52:53  dblair
 * CSCdi45019:  Disconnect of initial multilink call requires NCPs
 * to renegotiate.  Added code to for the NCP's to renegotiate when the
 * initial link is disconnected.
 *
 * Revision 3.9  1995/11/29  01:59:10  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.8  1995/11/29  00:16:38  lol
 * CSCdi42385:  TACACS+ crashes router in aaa_split_AV
 *
 * Revision 3.7  1995/11/27  18:29:59  pst
 * CSCdi44460:  cosmetic cleanup of ppp / multilink information
 *
 * Revision 3.6  1995/11/19  04:50:36  fox
 * CSCdi43514:  PAP authentication is broken
 * Clean up logic used to prevent premature LCP Opens and extend the
 * logic to all possible cases.
 *
 * Revision 3.5  1995/11/18  22:39:11  fox
 * CSCdi43831:  PPP lineprotocol is marked up before authentication
 * completes
 * Set ppp_lineup whenever we change PPP phase to UP and clear it if we
 * are changing the phase to not UP.
 *
 * Revision 3.4  1995/11/18  17:26:04  fox
 * CSCdi43678:  Obsolete code exists in ppp.c
 * Remove unused and obsolete ifdef blocks and unreferenced variables.
 *
 * Revision 3.3  1995/11/17  18:03:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:50:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:50:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.37  1995/11/08  21:36:41  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.36  1995/10/23  21:02:31  fox
 * CSCdi42602:  Neighbor routes are lost if routing table is cleared
 * Move neighbor address support to the WAN code.
 * Call the WAN code if an interface's routes are being modified.
 *
 * Revision 2.35  1995/10/23  13:45:04  dblair
 * CSCdi42550:  Add multilink status and clean up lcp/ncp stats in show
 * interface
 *
 * Revision 2.34  1995/10/22  07:32:42  fox
 * CSCdi42568:  Peer IP address is not released when B-channel drops
 * Clear (ie release) the peer IP address in ppp_cstate in order to
 * catch any interfaces that do not call net_cstate().  Multiple calls
 * at interface transition time are redundant but safe.  Ideally, one
 * exit mechanism should catch all possible cases....
 *
 * Revision 2.33  1995/10/20  17:14:18  dblair
 * CSCdi40641:  Bridging over MLP ISDN PRI broken
 *         Bridge code assumed 6 byte header.  Multilink now completes
 *         header when receiving and encapsulated bridge packet
 *
 * Revision 2.32  1995/10/17  13:52:12  dblair
 * CSCdi42218:  Multilink PPP does not work with caller id verification
 *
 * Revision 2.31  1995/10/15  22:05:47  gtaylor
 * CSCdi38452:  show compress has same output for 1 min, 5 min, 10 min
 * average Problem caused by ISDN B channel idb comp_db structure not
 * being initialised with the compress_clearcounters function.
 *
 * Revision 2.30  1995/10/02  20:43:18  lol
 * CSCdi36244:  Console line failing authorization should be more forgiving
 *              Authenticated console users now bypass authorization.
 *
 * Revision 2.29  1995/10/02  01:55:07  lol
 * CSCdi41162:  xtacacsd: username no longer included in XTA_SLIPON
 *         Fake "if-needed" authentication for vty-async.
 *
 * Revision 2.28  1995/09/30  05:06:32  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.27  1995/09/25  17:28:48  wmay
 * CSCdi40802:  PPP looped message is annoying - remove it and put message
 * under debug - ppp error or ppp negotiation.
 *
 * Revision 2.26  1995/09/20  22:39:07  dblair
 * CSCdi40745:  PPP Multilink feature commit
 *
 * Revision 2.25  1995/09/18  18:13:25  gtaylor
 * CSCdi29250:  PPP compression should support the existing compress
 * command. For ppp encapsulation compression now appears without the
 * ppp prefix. The command can still be entered with the prefix.
 *
 * Revision 2.24  1995/09/16  21:44:53  carrel
 * CSCdi40334:  Bus error crash in AAA accounting code
 * Correctly set ho->address when AAA chooses the addr
 * If pooling fails to set a non-mandatory pool, fall thru to addr checks
 * Don't put ciaddr1 in cached authors if all attempts to derive an addr
 * fail.
 * Remove an arg to aaa_dup_ifneeded().
 * Make aaa accounting calls ONLY get called for aaa new-model.
 *
 * Revision 2.23  1995/09/13  01:25:35  carrel
 * CSCdi40013:  User information needs to be present even when no login
 * occurs
 * In lcp_up(), only make dummy ustruct if one doesn't exist.
 *
 * Revision 2.22  1995/09/12  18:48:01  gglick
 * CSCdi38693:  lapb disables priority queuing
 *
 * Revision 2.21  1995/09/10  23:39:04  carrel
 * CSCdi40013:  User information needs to be present even when no login
 * occurs
 * aaa_create_user() now initializes the acc_method to be
 * AAA_ACC_METH_NONE.  Make sure lcpstate->ustruct is always created.
 *
 * Revision 2.20  1995/09/09  00:54:43  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.19  1995/09/08  23:50:33  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * more ping pong b/w me and the reviewer.
 *
 * Revision 2.18  1995/09/08  19:53:03  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * Move the authorstruct cleanup to lcp_do_author.
 *
 * Revision 2.17  1995/09/07  09:36:43  irfan
 * CSCdi39879:  memory leaking as AAA AV pairs from PAP/CHAP threads
 * Plug the holes by remembering to aaa_cleanup_author() for both return
 * cases of lcp_do_author().
 *
 * Revision 2.16  1995/09/06  18:46:05  syiu
 * CSCdi37776:  PPP callback command not written to nvgen correctly
 *
 * Revision 2.15  1995/08/08  20:50:50  lol
 * CSCdi37361:  AAA should not require auth for originating PPP connections
 *
 * Revision 2.14  1995/08/08  16:50:54  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi38391]
 *
 * Revision 2.13  1995/08/07 21:52:42  lol
 * CSCdi36743:  async PPP fails to connect with /routing and tacacs+
 * authorization
 *
 * Revision 2.12  1995/08/03  21:47:06  irfan
 * CSCdi38198:  memory leak in ppp callback
 * free any previously malloc'd and saved callback-message strings. Also
 * release them when resetting lcp negotiation options and when lcp
 * closes. Changed a couple of malloc's to malloc_named.
 *
 * Revision 2.11  1995/07/14  07:18:55  misko
 * CSCdi36902:  PPP Fastswitching incorectly disabled via compression code
 * CSCdi35161 failed to take into account that if comp_db is defined,
 * that it caused fast switching to be disabled.  Since CSCdi35161 always
 * allocated comp_db when PPP was enabled, but it was never freed, this
 * causes all fast switching to be disabled for WAN encaps after PPP had
 * been configured once.  It also meant fast switching was always disabled
 * for PPP.  Make sure comp_db is only allocated when compression is
 * enabled, and free and clear it when compression is disabled or
 * encapsulation is changed.
 *
 * Revision 2.10  1995/07/05  22:17:50  gtaylor
 * CSCdi34592:  PPP Reliable Link broken. Changed lapb's attachment to PPP
 * from taking over and saving the idb->iqueue to taking over the
 * parse_packet vector instead. This also fixes bridging problems with
 * reliable link.
 *
 * Revision 2.9  1995/06/29  17:22:10  gtaylor
 * CSCdi35161:  PPP compression should support the show compression stats
 * command.
 * Support for PPP compression stats added. ISDN B channel compression
 * stats
 * display also included.
 *
 * Revision 2.8  1995/06/23  18:49:08  ahh
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase III -- remove unneeded NOMEMORY message externs.
 *
 * Revision 2.7  1995/06/22  21:09:02  syiu
 * CSCdi36300:  PPP callback message needs to freed after use
 *
 * Revision 2.6  1995/06/20  20:38:19  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.5  1995/06/20  16:06:46  syiu
 * CSCdi36154:  PPP callback allows link to go to UP state
 *
 * Revision 2.4  1995/06/15  22:58:46  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.3  1995/06/15  03:42:58  lol
 * CSCdi35713:  AAA subsystems needs more debugging output
 *              Add
 *                 aaa debug authentication
 *                 aaa debug authorization
 *                 aaa debug accounting
 *
 * Revision 2.2  1995/06/14  06:31:11  billw
 * CSCdi35380:  AAA accounting causes crashes
 * Clean up aaa_dup_user and callers, correct freeing of non-malloced
 * memory, and a couple other things.
 *
 * Revision 2.1  1995/06/07  23:19:00  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * ppp.c - Point-to-Point Protocol.
 *
 * Copyright (c) 1989 Carnegie Mellon University.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by Carnegie Mellon University.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */

#ifndef lint
static const char ppp_copyright[] = 
"Point-to-Point Protocol. Copyright (c) 1989 Carnegie Mellon University. All rights reserved. Redistribution and use in source and binary forms are permitted provided that the above copyright notice and this paragraph are duplicated in all such forms and that any documentation, advertising materials, and other materials related to such distribution and use acknowledge that the software was developed by Carnegie Mellon University.  The name of the University may not be used to endorse or promote products derived from this software without specific prior written permission. THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE. ";
#endif

#include "master.h"
#include "../ui/common_strings.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "media_registry.h"
#include "ppp_registry.h"
#include "../h/cs_registry.regh"
#include "dialer_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "ttysrv.h"
#include "packet.h"
#include "connect.h"
#include "config.h"
#include "parser.h"
#include "login.h"
#include "../ui/debug.h"
#include "parser_defs_ppp.h"
#include "../parser/parser_defs_compress.h"
#include "../parser/parser_actions.h"
#include "../if/network.h"
#include "../if/ether.h"
#include "../if/arp.h"
#include "../if/if_vtemplate.h"
#include "../srt/span.h"
#include "serial.h"
#include "../util/md5.h"
#include "serial_private.h"
#include "ppp.h"
#include "ppp_public.h"
#include "ncp.h"
#include "ccp.h"
#include "ipcp.h"
#include "ppp_auth.h"
#include "logger.h"
#include "../wan/msg_ppp.c"		/* Not a typo, see logger.h */
#include "address.h"
#include "mgd_timers.h"
#include "aaa.h"
#include "dialer.h"
#include "dialer_private.h"
#include "mlp.h"
#include "../les/isdn_bri.h"
#include "../ip/ip.h"
#include "ppp_debug.h"
#include "../ts/if_async.h"
#include "../ts/if_slip.h"
#include "../h/auth_registry.regh"
#include "../os/os_debug_flags.h"
#include "comp_sys.h"
#include "../ip/ip_registry.h"
#include "../if/priority_private.h"
#include "ppp_registry.h"
#include "../x25/lapb.h"

/*
 * Forward declarations
 */
static void fsm_clear_author(fsm *f);
static boolean fsm_author_in_progress(fsm *f);
static boolean fsm_do_author(hwidbtype *idb, fsm *f);
static boolean fsm_check_author(fsm *f);
static boolean ppp_lcpstate(hwidbtype *, int);
static boolean ppp_set_lcpstate(hwidbtype *, int);
static void ppp_continue_vpn(int, paktype *, hwidbtype *);
static boolean lcp_ackci_forced(fsm *, uchar *, int, int, int);
static void fsm_open_delayed(fsm *f);
static uchar protocol_to_authtype(ushort, uchar);
static char *authtype_name(uchar);
static void get_another_auth_method(fsm *, uchar);
static void copy_ppp_fields(hwidbtype *, hwidbtype *);
static void ppp_cstate(register hwidbtype *);
static long ppp_getlink(paktype *);
static process ppp_input(void);
static void ppp_timer(hwidbtype *);
static void ppp_init(subsystype *);
static uchar lcp_reqci(fsm *, uchar *, short *);
static int lcp_ackci(fsm *, uchar *, int);
static int lcp_cilen(fsm *);
static void fsm_rcoderej(fsm *, uchar *, int);
static void fsm_rconfack(fsm *, uchar *, uchar, int);
static void fsm_rconfnak(fsm *, uchar *, uchar, short);
static void fsm_rconfrej(fsm *, uchar *, uchar, short);
static void fsm_rechorep(fsm *, uchar *, uchar, int);
static void fsm_rprotrej(hwidbtype *, uchar *, short);
static void fsm_rtermack(fsm *);
static void fsm_rtermreq(fsm *, uchar);
static void fsm_timeout(fsm *);
static void lcp_activeopen(fsm *);
static void lcp_addci(fsm *, uchar *);
static void lcp_closed(fsm *);
static void lcp_down(fsm *);
static void lcp_init(hwidbtype *);
static void lcp_input(paktype *, hwidbtype *);
static void lcp_lowerdown(fsm *);
static void lcp_nakci(fsm *, uchar *, int);
static void lcp_passiveopen(fsm *);
static void lcp_protrej(hwidbtype *, ushort);
static void lcp_rejci(fsm *, uchar *, int);
static void lcp_resetci(fsm *);
static void lcp_up(fsm *);
static void lcp_print(paktype *);
static void lcp_slqr(hwidbtype *);
static void lqr_init(hwidbtype *);
static void lqr_protrej(hwidbtype *, ushort);
static void ppp_periodic(hwidbtype *idb);
static void lcp_rlqr(paktype *, hwidbtype *);
static boolean ppp_callback_accept(lcp_options *, uchar, uchar, uchar *);
static void ppp_deencap(hwidbtype *);
static void copy_ncp_fields(hwidbtype *);
static void ppp_interface(hwidbtype *);
static void ppp_if_prot_change(idbtype *, hwidbtype *);
static void ppp_if_state_change(hwidbtype *);
static void ppp_send_lapb_dm(paktype *, hwidbtype *);
static void ppp_lapb_event(hwidbtype *);


/*
 * Local Storage
 */


static watched_queue *PPPpacketQ;
static queuetype fsm_authorQ;

struct protent {
    ushort protocol;
    void (*init)(hwidbtype *);
    void (*input)(paktype *, hwidbtype *);
    void (*protrej)(hwidbtype *, ushort);
} static const prottbl[] = {
    {TYPE_PPP_LCP, lcp_init, lcp_input, lcp_protrej },
    {TYPE_PPP_CHAP, auth_init, chap_input, auth_protrej },
    {TYPE_PPP_UPAP, auth_init, pap_input, auth_protrej },
    {TYPE_PPP_LQR, lqr_init, lcp_rlqr, lqr_protrej },
    {0,ncp_init, ncp_input, ncp_protrej }
};

static pid_t ppp_pid;			/* Process ID of PPP process */

/*
 * lcp callback routines
 */

static fsm_callbacks lcp_callbacks = {
    lcp_resetci,		/* reset our configuration information */
    lcp_cilen,			/* length of our configuration information */
    lcp_addci,			/* add our configuration information */
    lcp_ackci,			/* ack our configuration information */
    lcp_nakci,			/* nak our configuration information */
    lcp_rejci,			/* reject our configuration information */
    lcp_reqci,			/* request peer's configuration information */
    lcp_up,			/* called when fsm reaches OPEN state */
    lcp_down,			/* called when fsm leaves OPEN state */
    lcp_closed,			/* called when fsm reaches closed state */
    NULL,			/* called when protocol-reject received */
    NULL,			/* retransmission is necessary */
    NULL,			/* Reset Request */
    NULL,			/* Rest Acknowledge */
    NULL			/* Code Rejected */
};

/*
 * Text strings
 */

static const char *const lcp_optionnames[] = {
    "??",
    "MRU",			/*	1 Maximum Receive Unit */
    "ASYNCMAP",			/*2 Async Control Character Map */
    "AUTHTYPE",			/*3 Authentication Type */
    "QUALITYTYPE",		/*4 Quality Protocol Type */
    "MAGICNUMBER",		/*5 Magic Number */
    "RESERVED",			/*6 formerly Link Quality Monitoring */
    "PCOMPRESSION",		/*7 Protocol Field Compression */
    "ACCOMPRESSION",		/*8 Address/Control Field Compression */
    "32FCS",			/*9 32 bit FCS */
    "10",
    "NUMBEREDMODE",	    /*11 Number-Mode reliable transmission */
    "12", 
    "CALLBACK",                  /*13 Callback */ 
    "14",
    "15",
    "16",
    "MULTILINK_MRRU",                   /*17 Multilink request and MRRU  */
    "MUTLILINK_SHORT_SEQ",             /*18 Multilink short sequence number */
    "ENDPOINT_DISC"                     /*19 Endpoint discriminator */         
};
#define MAX_LCPOPTIONS 19

/*
 * The PPPnet subsystem header
 */

#define PPP_MAJVERSION 1
#define PPP_MINVERSION 0
#define PPP_EDITVERSION  1

SUBSYS_HEADER(ppp, PPP_MAJVERSION, PPP_MINVERSION, PPP_EDITVERSION,
	      ppp_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      "req: compress");

/*
 *  return a pointer to the ASCII printable version of the
 *    endpoint discriminator if it is available
 */

static char * ppp_get_endpoint_string (hwidbtype *idb)
{
    lcp_options *ho;

    if (idb->lcpfsm) {
	ho = idb->lcpfsm->heroptions;
	/*
	 *  If we received and endpoint discriminator option return
	 *    the string
	 */
	if (ho && ho->neg_endpoint) {
	    return(ho->endpoint_string);
	}
    }
    return(NULL);
}

/*
 *  ppp_get_remote_name
 *  find the pointer to the remote name string
 */
uchar * ppp_get_remote_name (hwidbtype *idb)
{
    if (idb->authstate && !null(idb->authstate->remote_name)) {
	return(idb->authstate->remote_name);
    }
    return(ppp_get_endpoint_string(idb));
}

/*
 * look up prottbl entry given PPP protocol type
 */
static int prottbl_lookup (ushort protocol)
{
    int i;

    for (i=0; i< sizeof(prottbl) / sizeof(struct protent); i++) {
	if (prottbl[i].protocol == protocol)
	    return(i);
	if (prottbl[i].protocol == 0) /* Default */
	    return(i);
    }

    /*  This can't happen.  Return the last entry anyhow. */

    return((sizeof(prottbl)/sizeof(struct protent)) - 1);
}

/*
 * ppp_linestate
 * Return current line protocol status
 */

static boolean ppp_linestate (hwidbtype *hwidb)
{
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(hwidb, HWIDB_SB_SERIAL);

    if (reg_invoke_dialer_spoofing_prot(hwidb)) {
	return(TRUE);
    } else if (IDBTYPE_ASYNC_GROUP == hwidb->type) {
	return(TRUE);
    } else {
	/*
	 * The interface is down if it is looped back and the
	 * interface is configured to go down on loopback
	 */
	return(hwidb->ppp_lineup &&
	       !(hwidb->inloopback && ssb->downiflooped));
    }
}

/*
 * ppp_extract_addr
 *
 * Given a PPP packet, extract the requested 'MAC' level address
 * into a hwaddrtype structure.
 */
static boolean ppp_extract_addr (
    paktype *pak,
    hwaddrtype *address,
    enum SRCDST which)
{
    if ((pak->enctype == ET_PPP) || (pak->addr_start == NULL)) {
	address->type = STATION_PPP;
	address->length = STATIONLEN_PPP;
	address->target_type = TARGET_UNICAST;
	memset(address->addr, 0, IEEEBYTES);
	return(TRUE);
    }
    return(ether_extract_addr(pak, address, which));
}

/*
 * We have to handle bridged packets on our own now.
 * ppp_parse_packet will return an iqueue pointer to
 * ppp_bridge_enqueue for all bridged packets.
 */
static void ppp_bridge_enqueue (hwidbtype *idb, paktype *pak)
{
    ulong link;
    boolean do_raw_enqueue;
    hwidbtype *master_idb;
    ushort ppp_bridge_header;

    do_raw_enqueue = FALSE;
    pak->datagramstart += PPP_ENCAPBYTES;
    PPP_GETSHORT(ppp_bridge_header, pak->datagramstart);
    pak->datagramsize -= PPP_BRIDGE_ENCAPBYTES;

    /*
     * First, check for any special processing per RFC 1638
     *
     * We want to be generous in what we accept within the rules of RFC 1638.
     * Thus we will:
     *   drop any frames that have an embedded LAN ID.
     *     (since we do not support this feature yet)  
     *   drop any non-IEEE 802.3/Ethernet packets.
     *   strip off any pad characters in the datagram.
     *   strip off any LAN FCS before processing.  CSCdi57429
     *   process Tinygram Compression even if it has not been negotiated.
     *   ignore the Reserved/MustBeZero bit even if set.
     *     (This is a potential bug if that value is ever defined and it
     *      means that we are supposed to drop the packet!)
     *
     * Optimize best case to minimize performance impact.  If not best case,
     * be thorough.
     */
    if (ppp_bridge_header != PPP_FIZC_MACTYPE) {
	/*
	 * drop any frames that have an embedded LAN ID.
	 */
	if (ppp_bridge_header & PPP_BH_LAN_ID) {
	    retbuffer(pak);
	    if (ppp_error_debug) {
		buginf("\n%s PPP: Bridge packet dropped, LAN ID present",
		       idb->hw_namestring);
	    }
	    return;
	}
	/*
	 * drop any non-IEEE 802.3/Ethernet packets.
	 */
	if ((ppp_bridge_header & PPP_BH_MAC_TYPE) != PPP_BH_802_3) {
	    retbuffer(pak);
	    if (ppp_error_debug) {
		buginf("\n%s PPP: Bridge packet dropped, unsupported MAC"
		       " type = %d", idb->hw_namestring,
		       ppp_bridge_header & PPP_BH_MAC_TYPE);
	    }
	    return;
	}
	/*
	 * strip off any pad characters in the datagram.
	 */
	if (ppp_bridge_header & PPP_BH_PADS) {
	    pak->datagramsize -= ((ppp_bridge_header & PPP_BH_PADS) >> 8);
	}
	/*
	 * strip off any LAN FCS before processing.  CSCdi57429
	 */
	if (ppp_bridge_header & PPP_BH_FCS) {
	    pak->datagramsize -= PPP_LAN_FCS_SIZE;
	}
	/*
	 * process Tinygram Compression even if it has not been negotiated.
	 *
	 * Note:  this is not the full algorithm from RFC 1638 because we
	 *	  do not support the LAN FCS and thus would have removed it
	 *	  in the test just above.  If we EVER add support for the
	 *	  LAN FCS then we need to fix up this code!
	 */
	if ((ppp_bridge_header & PPP_BH_ZEROFILL) &&
	    (pak->datagramsize < PPP_MIN_PDU_SIZE)) {
	    bzero(pak->datagramstart + pak->datagramsize,
		  PPP_MIN_PDU_SIZE - pak->datagramsize);
	    pak->datagramsize = PPP_MIN_PDU_SIZE;
	}
    }

    ether_parse_packet(idb, pak, NULL);
    link = ether_getlink(pak);

    SET_PAK_DIALER_INPUT(pak);
    if (reg_invoke_dialer_change_pak_if(idb, pak)) {
        retbuffer(pak);
        return;
    }

    /*
     * If multilink is up and there isn't a dialer configured, 
     * we want to change in input interface to originate from 
     * the Bundle interface  
     */
    if (is_multilink(idb) && !is_ddr(pak->if_input->hwptr)) {
        master_idb = mlp_get_master_idb(idb);
 		
        if (master_idb != idb) {
 	  if (!change_if_input(pak, master_idb->firstsw)) {
 	     datagram_done(pak);
 	     if (ppp_error_debug) {
 		buginf("\n%s MLP: Bridge packet dropped, can't "
 		       "change input queue", idb->hw_namestring);
 	     }
              return;
 	  }
           if (ppp_packet_debug) 
  	     buginf("\n%s MLP: Bridge input redirected to %s",
 		 idb->hw_namestring, master_idb->hw_namestring);         
 	  idb = master_idb;
        }
    }
 
    switch (link) {
    case LINK_ARP:
	do_raw_enqueue = TRUE;
	break;
    case LINK_IP:
	if (idb->ppp_bridge_ip || reg_invoke_ip_forus(pak)) {
	    do_raw_enqueue = TRUE;
	}
	break;
    case LINK_NOVELL:
	if (idb->ppp_bridge_ipx) {
	    do_raw_enqueue = TRUE;
	}
	break;
    case LINK_APPLETALK:
    case LINK_AARP:
    case LINK_ATALK_SHORT:
	if (idb->ppp_bridge_atalk) {
	    do_raw_enqueue = TRUE;
	}
	break;
    case LINK_LAT:
	/*
	 * LAT is only locally processed if LAT is enabled on 
	 * the interface (the LAT engine in turn sends "not for us" paks 
	 * to the bridging engine). This allows LAT traffic directed for the 
	 * bridge end-point to be still accessible. 
	 */
	if (reg_invoke_proto_on_swidb(LINK_LAT, idb->firstsw)) {
	    do_raw_enqueue = TRUE;
	}
	break;
    default:
	break;
    }
    if (do_raw_enqueue ||
	(global_irb_enable && is_bvi(pak->if_input->hwptr))) {
	/*
	 * If IRB(Integrated Routing and Bridging) is enabled,
	 * there is a possibility that we overwrote the input 
	 * interface to let it looks like it is coming from the BVI
	 * (Bridge-group Virtual Interface) when making bridge/route
	 * decision in tbridge fast-switching code; If these are
	 * true, raw_enqueue it to the appropriate queue instead of
	 * enqueue it to bridgeQ!
	 */
	/*
	 * Snap the frame to alignment if required
	 */
	ALIGN_DATAGRAM_CMD(pak_realign(pak, NULL));

	pak->linktype = link;

	reg_invoke_raw_enqueue(link, pak);
    } else 
	net_bridge_enqueue(pak);
}

/*
 * ppp_parse_packet
 *
 *
 * Set up a packet buffer so that the network level protocol fields
 * are initialized.
 *
 * If data is NULL, pak->datagramstart is assumed to point to the
 * start of the frame to parse. No attempt will be made to align
 * the frame to The Line.
 */

iqueue_t ppp_parse_packet (
    hwidbtype *idb,
    paktype *pak,
    uchar *data)
{
    int encsize;
    ulong protocol;
    charlong firstlong;

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

    /*
     * Grab the encapsulation type, and figure out the encapsulation size.
     */
    pak->enctype = ET_PPP;
    if (firstlong.d.sword[0] == PPP_STATION) {
	/*
	 * Packet has a leading address and control field. Look
	 * for protocol field compression in the third octet.
	 */
	if (firstlong.d.byte[2] & PPP_ENDPROTO) {
	    protocol = firstlong.d.byte[2];
	    encsize = 3;	/* Three byte encapsulation */
	} else {
	    protocol = firstlong.d.sword[1];
	    encsize = 4;	/* Four byte encapsulation */
	}
    } else {
	/*
	 * Compressed address and control field. Look for protocol
	 * field compression in the first octet.
	 */
	if (firstlong.d.byte[0] & PPP_ENDPROTO) {
	    protocol = firstlong.d.byte[0];
	    encsize = 1;	/* Single-byte encapsulation */
	} else {
	    protocol = firstlong.d.sword[0];
	    encsize = 2;	/* Two-byte encapsulation */
	}
    }
    
    if (protocol == TYPE_PPP_BRIDGE) {
	ether_hdr *ether;

	encsize += PPP_BRIDGE_EXTRABYTES;
	pak->flags |= PAK_TBRIDGE_IN;
	pak->enctype = ET_ARPA;

	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak)
		- encsize - ETHER_ARPA_ENCAPBYTES;
	}
	ether = (ether_hdr *)(pak->datagramstart + encsize);
	pak->mac_start     = (uchar *)ether;
	pak->addr_start    = pak->mac_start;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
	pak->info_start    = NULL;
	pak->network_start = ether->data;
	pak->encsize = pak->network_start - pak->datagramstart;

  
        /*
         * Normally, the iqueue is PPP bridge routine is returned. However, in 
         * the case of a NAS where the interface is Forwarded/Forwarding 
         * L2F links, the iqueue from the idb is returned. This is so that 
         * the bridged packet is forwarded via L2F.  
         */
        if (ppp_lcpstate(idb, FORWARDED) || ppp_lcpstate(idb, FORWARDING)) {
           return(idb->iqueue);
        } else {
	   return(&ppp_bridge_enqueue);
        }
    } else {
	pak->flags &= ~PAK_TBRIDGE_IN;
	/*
	 * If we're building a virgin frame, we have to set datagramstart
	 */
	if (data) {
	    pak->datagramstart = pak_center(pak) - encsize;
	}
	pak->mac_start     = pak->datagramstart;
	pak->addr_start    = NULL;
	pak->rif_start     = NULL;
	pak->riflen	   = 0;
	pak->info_start    = pak->datagramstart + encsize;
	pak->network_start = pak->datagramstart + encsize;
	/*
	 * Record just how big this encapsulation was rather than every protocol
	 * have it's own fun and wacky way to work it out.
	 */
	pak->encsize = encsize;
	return(idb->iqueue);
    }
}

/*
 * ppp_autodetect_start_interface
 *
 * attempt to start ppp on this interface. called via
 * serial autodetect registries.
 */
static boolean ppp_autodetect_start_interface (hwidbtype *hwidb)
{

    /*
     * sanity check
     */
    if (!hwidb) {
	return (FALSE);
    }
    if (hwidb->enctype == hwidb->enctype_cfg) {
	/*
	 * this should be pre-checked in autodetect.
	 * but just in case.
	 */
	return (TRUE);
    }
    /*
     * we don't implement switching into PPP mode
     * undestructively from any other encaps.
     */
    if (serial_debug) {
	buginf("\nSer-Autodetect %s: switching to ppp encaps not"
	       " supported", hwidb->hw_short_namestring);
    }
    return (FALSE);
}

/*
 * ppp_autodetect_test
 *
 * tells us if we are seeing PPP packets on
 * this B-channel/sync/async-line
 */
static boolean ppp_autodetect_test (hwidbtype *hwidb, paktype *pak)
{

    /*
     * sanity check
     */
    if ((!pak) || (!hwidb)) {
	return (FALSE);
    }

    if (ppp_packet_debug) {
	uchar *data = pak->datagramstart;
	int i;

	buginf("\nPPP: Autodetect trying to detect PPP mode on %s",
	       (hwidb) ? hwidb->hw_short_namestring : "unknown");

	buginf("\nPPP sampled pkt:  %d bytes: ", pak->datagramsize);
	for (i=0; i<((pak->datagramsize < 10)?pak->datagramsize : 10);i++)
	    buginf(" %X", *(data+i));
    }

    return ((PPP_STATION == GETSHORT(pak->datagramstart)) &&
	    (TYPE_PPP_LCP == GETSHORT(pak->datagramstart+sizeof(short))));
}

/*
 * ppp_setup
 * Set up a serial interface to use ppp
 */

boolean ppp_setup (parseinfo *csb, hwidbtype *idb)
{
    int i;
    idbtype *swidb;
    fsm *lcp_fsm;
    serialsb *ssb;

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    /*
     * First things first, make sure we have an LCP FSM.  Too many routines
     * will access this pointer without validating it and this routine may
     * dismiss before completing.  :-(  CSCdi66589
     */
    if (!idb->lcpfsm) {
    	lcp_fsm = malloc_named(sizeof(fsm), "LCP FSM");
    	if (!lcp_fsm) {
	    return(FALSE);
    	}
        idb->lcpfsm = lcp_fsm;
    }
    lcp_fsm = idb->lcpfsm;
    lcp_fsm->idb = idb;

    if (!idb->lcpstate) {
        idb->lcpstate = malloc_named(sizeof(lcp_state), "LCP STATE");
        if (!idb->lcpstate) {
	    return(FALSE);
	}
    } 

    /*
     * Initialize to the standard option set.
     */
    for (i = 0; i < sizeof(prottbl) / sizeof(struct protent); i++)
        (*prottbl[i].init)(idb);

    copy_ncp_fields(idb);

    if (!ppp_pid) {
	ppp_pid = process_create(ppp_input, "PPP manager", LARGE_STACK,
				 PRIO_NORMAL);
	if (!ppp_pid) {
	    return(FALSE);
	}
        reg_add_if_onesec(ppp_timer, "ppp_timer");
	reg_invoke_serial_autodetect_register_encaps(ET_PPP,
						     ppp_autodetect_test,
						     ppp_autodetect_start_interface);
	mlp_setup(idb);
    }

    idb->enctype = ET_PPP;
    /*
     * Set the fastswitch flavor
     */
    idb->fast_switch_type = FS_PPP;

    idb->vencap = ppp_vencapsulate;
    idb->bridge_vencap = (bridge_vencap_t)return_null;
    idb->deencap = ppp_deencap; /* Disable PPP encapsulation */
    idb->getlink = ppp_getlink;
    set_default_queueing(idb);
    idb->iqueue = raw_enqueue;
    idb->parse_packet = ppp_parse_packet;
    idb->periodic = ppp_periodic;
    idb->extract_hwaddr = invalidate_hwaddr;
    idb->extract_snpa = ppp_extract_addr;
    idb->broadcast = (broadcast_t) return_fuz_false;/* just return FUZ_FALSE */
    idb->show_support = ppp_interface;
    idb->lineaction = ppp_cstate;
    idb->linestate = ppp_linestate;
    ssb->hdlc_mineseen = 0;
    ssb->hdlc_yoursseen = 0;
    ssb->hdlc_myseq = 0;
    idb->ppp_lineup = FALSE;
    idb->ppp_lapb = FALSE;
    idb->ppp_new_vencap = NULL;
    idb->ppp_new_lineaction = NULL;
    idb->ppp_new_show_support = NULL;
    idb->ppp_save_parse_packet = NULL;
    idb->ppp_compression = NULL;
    idb->ppp_comp_history = NULL;
    idb->ppp_comp_wrkspc = NULL;
    idb->ppp_decompression = NULL;
    idb->ppp_decomp_history = NULL;
    idb->encsize = PPP_ENCAPBYTES;
    idb->max_pak_size = BIGDATA;
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	swidb->ip_enctype = ET_PPP;
	reg_invoke_media_enctype_setup(swidb, ET_PPP);
	swidb->tbridge_media = TBR_HDLC_TRANSIT;
	swidb->ip_routecache_same = swidb->ip_routecache_same_def = TRUE;
    }
    idb->inloopback = FALSE;

    /*
     * If these are ZERO then it's during system init. and we con't call
     * set_maxdgram()!
     */
    if (supports_maxdgram(idb)) {
	ushort mru;

	/*
	 * According to the specs (RFC 1661), we must be able to receive the
	 * default MRU of 1500 bytes even if we have negotiated our MRU to a
	 * smaller amount.  Thus we must make sure that we do not carve the
	 * buffers to a smaller size.
	 */
	mru = max(DEFMRU, idb->maxmtu);

	if (!(idb->status & IDB_HSSI)) {
            (*idb->set_maxdgram)(idb, mru + ETHER_BAGGAGE, 
				      mru + ETHER_BAGGAGE + HDLC_ENCAPBYTES);
        } else { /* HSSI */
	    (*idb->set_maxdgram)(idb, mru + ETHER_BAGGAGE + DS3_OVERHEAD_BYTES, 
				      mru + ETHER_BAGGAGE + DS3_BAGGAGE);
	}
    }
    idb->span_macoffset = ((idb->encsize+2) >> 1);
    idb->span_encapsize = idb->encsize + 2;

    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
        if (swidb->tbridge_on_swidb) {
            idb->ppp_bridge = TRUE;
            idb->span_bridge_process_force |= SPAN_BR_PROC_PPP_BRIDGE;
        }
    }
    (*(idb->reset))(idb);

    /*
     * bring up the lcp layer. no cp traffic can occur until lcp is up...
     */
    /*
     * This is kinda ugly.  Unfortunately we have to put in knowledge here
     * about ISDN interfaces.  This is because if the D channel is "up"
     * when the encapsulation is set to PPP, the ppp_setup routine calls
     * lcp_activeopen.  This causes config requests to be sent on the idb
     * which called ppp_setup.  While it could be argued that ppp_setup
     * shouldn't be called for the D channel, currently the way things work
     * is that it is called, and it's a significant redesign to get around
     * this.  So, we are forced here to check if this is to go out the D
     * channel, and if so just return.  If we do send it out here, not
     * only do we get garbage on the D channel, but the config fails and
     * after 10 attempts spoofing is turned off and the interface looks
     * down, down, so no packets ever get sent and no calls get made.
     *
     * isdn_cstate deals with the case where the D channel comes "up" after
     * ppp encapsulation has been sent, so this could be put into ppp_setup
     * instead of here, but it's safer to put it here.
     */
    if ((idb->state == IDBS_UP) && (!reg_invoke_dialer_spoofing_int(idb))) {
        if (!is_isdn(idb) || (idb->isdn_info->chantype != ISDN_Dchan)) {
	    lcp_lowerup(lcp_fsm);	/* xxx well, sort of... */
	    lcp_activeopen(lcp_fsm); /* start protocol in active mode */
	}

    /*
     * There is an ugly race condition here that this attempts to
     * correct.  net_cstate sets the idb->state variable so that it
     * reflects the actual state of the line at that instant.  But,
     * we may be debouncing the line, so a short flap may cause incorrect
     * behavior.  What this if does, is check to see if the carriertimer
     * is running (we are debouncing), and the old state is UP.  If
     * so we assume that we are up and forge ahead.  If we are down, the
     * cstate routine will take care of it.  Without this, if we 
     * debounce, and happen to catch it here as DOWN, PPP never starts.
     */
    } else if ((mgd_timer_running(&idb->carriertimer)) && 
            (idb->oldstate == IDBS_UP)) {
        lcp_fsm->flags |= POPENDING;
        lcp_lowerup(lcp_fsm); 	/* xxx well, sort of... */
    }
    idb->status &= ~IDB_BCAST;
    return(TRUE);
}

/*
 * ppp_link2fsm
 *
 * given a link type get ncp fsm pointer
 */

fsm *ppp_link2fsm (long link, hwidbtype *idb)
{
    return(ppp_link2fsm_inline(link, idb));
}

/*
 * ppp_serial2fsm
 * given a PPP protocol and an IDB ptr, return a pointer to the fsm table
 */

fsm *ppp_serial2fsm (long type, hwidbtype *idb)
{
    return(ppp_serial2fsm_inline(type, idb));
}

/*
 * ppp_serial2link
 * given a PPP protocol, return an appropriate link type
 */

int ppp_serial2link (long type)
{
    return(ppp_serial2link_inline(type));
}

/*
 * ppp_getlink
 * get link type for an ppp datagram.
 */
static long ppp_getlink (register paktype *pak)
{
    ushort type;
    fsm *f;
    int link;
    hwidbtype *idb;

    if (pak->flags & PAK_TBRIDGE_IN) {
	if (ppp_packet_debug) {
	    ether_hdr *ether;

	    ether = (ether_hdr *)(pak->datagramstart);
	    buginf("\nPPP %s(i): pkt type %#x, datagramsize %d", 
		   pak->if_input->namestring, 
		   GETSHORT(pak->datagramstart-4),
		   pak->datagramsize);
	    buginf("\n\tbridged     dest %e, src %e",
		   ether->daddr, ether->saddr);
            if (GETSHORT(&ether->type_or_len) > MAXETHERSIZE)
                   /* Ethernet */
                buginf(", type %2x", GETSHORT(&ether->type_or_len));
            else {
		snap_hdr *snap;
		snap = (snap_hdr *)ether->data;
                buginf(", type %2x", GETSHORT(&snap->type));
	    }
	}
        return(ether_getlink(pak));
    }

    type = ppp_get_protocol_type(pak);

    if (ppp_packet_debug) {
	buginf("\nPPP %s(i): pkt type %#x, datagramsize %d", 
	       pak->if_input->namestring, type, 
	       pak->datagramsize);
    }

    if (pak->enctype != ET_PPP) {
	return(LINK_ILLEGAL);
    }
    
    idb = pak->if_input->hwptr;

    /*
     * Now we want the Link type that is appropriate to this external type.
     * Note that this is not the same as the fsm's protocol type.
     */
    link = ppp_serial2link_inline(type);

    /*
     * Find the fsm pointer for the xxxCP that controls this link type
     */
    f = ppp_link2fsm_inline(link, idb);

    /*
     * We have a simple set of rules to follow and two pieces of information
     * (the fsm pointer, if it exists, and the matching link type).
     *
     * if it is a 'data' protocol and the fsm exists and the state is OPEN then
     *    return the looked up link type
     * else if it is LEXCP and the code is out of range
     *    return the LEX RCMD type
     * else
     *    return the common PPP link type so that ppp_input can handle it
     */
    if ((type < NCP_PROTOCOL_BASE) && f && (f->state == OPEN)) {
	return(link);	/* This is the common data protocol path */
    } else if ((type == TYPE_PPP_LEXCP) &&
	       (ppp_get_header_code(pak) > CODEREJ)) {
	/*
	 * KLUDGE Alert: TYPE_PPP_LEXCP is used to carry both the
	 * LEX ncp messages and LEX remote commands.  So we look inside
	 * the packet at the NCP code.  If it is a legal NCP code (less
	 * than or equal to a CodeReject) return LINK_PPP as this is
	 * really a LEX NCP packet, else return LINK_LEX_RCMD and let the
	 * LEX command sort it out.  Note that fsm existence or state is
	 * not checked for LEX remote commands.  Perhaps that is a bug.
	 */
	return(LINK_LEX_RCMD);
    } else {
	return(LINK_PPP);
    }
}

/*
 * Timestamp the PPP Interface transition.
 */ 
static void ppp_if_state_change (hwidbtype *hwidb)
{
   if (hwidb->lcpstate) {
       GET_TIMESTAMP(hwidb->lcpstate->link_transition);
   } 
}

/*
 * The final line protocol state indicates that the PPP interface is 
 * indeed up. From this point on, the interface is in steady state 
 * until the next interface transition (e.g. disconnected call, explicit
 * interface reset etc). Timestamp it if it is DOWN.
 */
static void ppp_if_prot_change (idbtype *swidb, hwidbtype *hwidb)
{
    if (hwidb && hwidb->lcpstate && 
        !interface_up_simple(hwidb->firstsw)) {
       GET_TIMESTAMP(hwidb->lcpstate->link_transition);
    }
}


/*
 * ppp_enqueue
 * Interrupt level packet enqueue
 */

void ppp_enqueue (paktype *pak)
{
    idbtype *swidb;
    hwidbtype *idb;
    int protocol;
    lcp_state *lcpstate;

    protocol = ppp_get_protocol_type(pak);
    if (protocol == TYPE_PPP_LQR) {
	swidb = pak->if_input;
	idb = swidb->hwptr;
	lcpstate = idb->lcpstate;
	lcpstate->rxpackets = idb_inputs(idb);
	lcpstate->rxoctets = idb_rx_cumbytes(idb) + (idb_inputs(idb) * 3);
	/*
	 * need to count 2 bytes of crc and 1 flag for every message
	 */
    }

    /*
     * Store the PPP link timestamp of the interface in the packet. 
     * Later on, inside the PPP Process dequeue, that time is checked 
     * to see if an interface transition happened between the time it is being 
     * queued here (in interrupt path) to the time the PPP process dequeues
     * this particular packet for processing. It will discard packets silently
     * if interface transition happens
     */
    if (pak->if_input->hwptr->lcpstate) {
       pak->gp_timer = pak->if_input->hwptr->lcpstate->link_transition;
    }
    process_enqueue_pak(PPPpacketQ, pak);
}

/*
 * ppp_input
 * This is the basic input routine. It pulls packets off its
 * queue till it's all done...
 */

static process ppp_input (void)
{
    hwidbtype *idb;
    fsm *f;
    int protocol, pak_count;
    paktype *pak;
    lcp_state *lcpstate;

    process_wait_on_system_init();

    /*
     * Set up this process' world.
     */
    PPPpacketQ = create_watched_queue("PPP Input", 0, 0);
    process_watch_queue(PPPpacketQ, ENABLE, RECURRING);
    reg_add_raw_enqueue(LINK_PPP, ppp_enqueue, "ppp_enqueue");

    while (TRUE) {
	process_wait_for_event();

	pak_count = PAK_SWITCHCOUNT;

	while (TRUE) {
	    /*
	     * Process only a limited number of packets per pass
	     */
	    if (pak_count-- == 0) {
		break;
	    }

	    pak = process_dequeue(PPPpacketQ);
	    if (pak == NULL) {
		break;
	    }

	    protocol = ppp_get_protocol_type(pak);
	    if (pak->if_input == NULL || pak->if_input->hwptr == NULL) {
		if (ppp_error_debug) {
		    buginf("\nPPP %s: if_input not set, protocol %#x dropped",
			   pak->if_input ? pak->if_input->namestring :
			   common_str_empty_str, protocol);
		}
		datagram_done(pak);
		break;
	    }

	    idb = pak->if_input->hwptr;

            /*
             * If interface transitioned whilst the packet was in the queue,
             * the packet is basically invalid. Discard it silently.
             */
            if (TIMERS_NOT_EQUAL(pak->gp_timer, 
                                 idb->lcpstate->link_transition)) {
               if (ppp_error_debug)
                  buginf("\nPPP %s interface transitioned, discarding packet",
                         pak->if_input->namestring); 
               datagram_done(pak);
               continue;
	    }

	    lcpstate = idb->lcpstate;
	    f = idb->lcpfsm;
	    /*
	     * If PPP-LAPB is not running, and the other side is
	     * speaking LAPB, generate a LAPB DM
	     */
	    if ((idb->ppp_lapb == FALSE) &&
		((protocol == LAPB_ADDRESSA) || (protocol == LAPB_ADDRESSB))) {
		ppp_send_lapb_dm(pak, idb);
		pak = NULL;
		break;
	    }
	    /*
	     * Toss all non-lcp packets unless lcp is OPEN.
	     */
	    if (f == NULL || (protocol != TYPE_PPP_LCP && f->state != OPEN)) {
		datagram_done(pak);
	        if (ppp_error_debug) {
	            buginf("\n%s: dropped, LCP not open.  Protocol = %#x", 
			   idb->hw_namestring, protocol);
	        }
		break;
	    }

	    /*
	     * We have to do a lot of checks before processing NCP packets.
	     * For this check, we will take advantage of the fact that all
	     * LCP and LCP-type protocols are >= 0xc021, ie TYPE_PPP_LCP.
	     *
	     * Note that this will allow LQR, Authentication, and LCP
	     * packets flow directly to the physical interface.
	     */
	    if (protocol < TYPE_PPP_LCP) {

		/*
		 * Toss all NCP packets if the interface is not UP,
		 * ie forwarding, virtualized, authenticating, etc.
		 * Except when we are authenticating on a MLP link
		 * interface. We will buffer the pending NCP packets.
		 * There are cases, e.g. when the client sends an IPCP
		 * confreq right after it receive auth ok. 
		 * If we don't buffer the IPCP confreq, the client
		 * may not send another until after a period of
		 * e.g. 9 seconds. See CSCdj05239
		 *
		 * Note that we are checking the PPP Phase of the eventual
		 * interface, ie after multilink may have switched us
		 * to the bundle interface.
		 */
		if (lcpstate->phase != UP) {
		    lcp_options *ho, *go;
		    ho = f->heroptions;
		    go = f->gotoptions;
		    if ((lcpstate->phase == AUTHENTICATING) && ho && go &&
			(ho->neg_mlp_mrru && go->neg_mlp_mrru)) {
			pak_enqueue(&mlp_pending_ncpQ, pak);
			if (multilink_detailed_debug) {
			    buginf("\n%s: Protocol = %#x buffered, "
				   "while building bundle interface",
				   idb->hw_namestring, protocol);
			}
		    }
		    else {
			datagram_done(pak);
			if (ppp_error_debug) {
			    buginf("\n%s: Protocol = %#x dropped.", 
				   idb->hw_namestring, protocol);
			}
		    }
		    break;
		}

	    }

	    (*prottbl[prottbl_lookup(protocol)].input)(pak, idb);
	}
    }
}

/* ppp_send_packet
 * Send a ppp packet. If there is no PAK present it allocates one.
 */

void ppp_send_packet (
    ushort protocol,
    uchar code,
    uchar id,
    paktype *pak,
    short len,
    hwidbtype *idb)
{
    ppp_hdr *ppp_hdrp;
    ppp_dll_hdr *dll;
    idbtype *tmpidb;
    serialsb *ssb;

    if (pak == NULL) {
	pak = getbuffer((sizeof(ppp_hdr)) + len + DLLHEADERLEN);
	if (!pak) {
	    return;
	}
    }

    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    pak->if_output = idb->firstsw;
    ssb->hdlc_mineseen = id;
    pak->linktype = LINK_PPP;
    pak->datagramsize = len + HEADERLEN + DLLHEADERLEN;
    pak->datagramstart = pak->network_start - DLLHEADERLEN;

    dll = (ppp_dll_hdr *)pak->datagramstart;
    dll->address = PPP_ADDRESS;
    dll->control = PPP_CONTROL;
    dll->type    = protocol;
    pak->enctype = ET_PPP;
    pak->encsize = DLLHEADERLEN;

    /*
     * If this is an LCP packet, set the PAK_CONTROL flag in the packet
     * flags. This tells the async PPP code to use the "conservative"
     * async control character mapping table.
     */

    if (protocol == TYPE_PPP_LCP)
      pak->flags |= PAK_CONTROL; /* Mark as control packet for async to see */

    ppp_hdrp = (ppp_hdr *)dll->data;
    ppp_hdrp->code = (uchar)code;
    ppp_hdrp->id = id;
    ppp_hdrp->plen = len + HEADERLEN; 

    /*
     * If this is an ECHOREQ or ECHOREP packet, put it at the head of
     * the queue.
     * If this is an ECHOREP packet, call clear_if_input to decrement 
     * input_qcount and clear if_input.  Clearing if_input gives this
     * packet a better chance to get queued in case of heavy traffic.
     */

    if ((code == ECHOREQ) || (code == ECHOREP)) {
	pak->flags |= PAK_PRIORITY;
	if (code == ECHOREP) {
	    clear_if_input(pak);
	}
    }

    if (ppp_packet_debug) {
        /*
         * Save off pak->if_input and Null it out so that the print routines
	 * don't display I instead of O for output packets.  This is because
	 * we are reusing a packet...
         */
	tmpidb = pak->if_input;
	pak->if_input = NULL;
	if (protocol == TYPE_PPP_LCP) {
	    lcp_print(pak);
	} else if ((protocol == TYPE_PPP_CHAP) ||
		   (protocol == TYPE_PPP_UPAP)) {
	    auth_print(pak);
	} else if (protocol > NCP_PROTOCOL_BASE) {
	    ncp_print(pak);
	}
	pak->if_input = tmpidb;
    }

    /*
     * If PPP is using an extra layer of encapsulation, ie. LAPB, then the
     * packet should be output using the idb oqueue vector.
     */
    if (idb->ppp_lapb) {
        (*idb->oqueue)(idb, pak, TAIL);
    } else {
	if (idb->board_encap) {
	    if (!(*idb->board_encap)(pak,idb)) {
		retbuffer(pak);
		return;
	    }
	}
        datagram_out(pak);
    }
}

/*
 * ppp_vencapsulate
 * Encapsulate a datagram.  
 */

boolean ppp_vencapsulate (
    paktype *pak,
    long address)
{
    fsm *lcp_fsm, *f;
    lcp_options *ho;		/* Remote peer's options */
    ushort type;		/* PPP protocol type for packet */
    hwidbtype *idb;
    lcp_state *lcpstate;
    int pcompression, accompression;
    boolean add_ether_encap;

    idb = pak->if_output->hwptr;
    
    lcp_fsm = idb->lcpfsm;	/* Get FSM pointer */
    ho = lcp_fsm->heroptions;   /* Get peer's granted options */

    pcompression = ho->neg_pcompression;
    accompression = ho->neg_accompression;
    
    if (is_multilink(idb)) {
	/*
	 *  If this not a bundle interface something is broken
	 */
	if (!idb->firstsw->is_bundle_interface) {
	    if (multilink_events_debug)
		buginf("\nMLP %s: Forwarded to wrong interface",
		       idb->hw_namestring);
	    return(FALSE);
	}
	/*
	 *  Put the full header on so the fastswitch cache will
	 *    get updated correctly for IPX.  IPX requires
	 *    the encapsize to be at least 4 bytes in xns/novfast.c
	 */
	pcompression = 0;
	accompression = 0;
    }
   
    /*
     * before getting PPP type. check if we have to do some ugly things
     * for bridging
     */
    pak->encsize = 0;
    add_ether_encap = FALSE;
    switch (pak->linktype) {
    case LINK_IP:
    case LINK_ARP:
	if (idb->ppp_bridge_ip ||
	    (BRIDGING_IF(pak->if_output, LINK_IP) &&
	    RUNNING_IF(LINK_BRIDGE,pak->if_output))) {
	    add_ether_encap = TRUE;
	}
	break;
    case LINK_NOVELL:
	if (idb->ppp_bridge_ipx) {
	    pak->enctype = idb->ppp_bridge_ipx_enctype;
	    add_ether_encap = TRUE;
	}
	break;
    case LINK_APPLETALK:
    case LINK_AARP:
    case LINK_ATALK_SHORT:
	if (idb->ppp_bridge_atalk) {
	    add_ether_encap = TRUE;
	}
	break;
    case LINK_LAT:
    case LINK_MOP_CONSOLE:
    case LINK_MOP_BOOT:
	add_ether_encap = TRUE;
	break;
    }
    if (add_ether_encap) {
	if (ether_vencap(pak, address)) {
	    /*
	     * an ugly kludge to get NCP state for bridge
	     */
	    pak->linktype = LINK_BRIDGE;
	    pak->enctype = ET_BRIDGE_ENCAPS;
	} else {
	    if (ppp_error_debug)
		buginf("\nPPP %s: failed ether vencap", idb->hw_namestring);
	    return(FALSE);
	}
    }

    /*
     * Handle the protocol field. Look it up based on link type.
     * If the remote peer can received compressed protocol types, we
     * handle that here.
     */

    type = ppp_link2serial_inline(pak->linktype, idb);
    if (!type) {
	if (ppp_negotiation_debug)
	    buginf ("\nPPP %s: Unsupported or un-negotiated protocol. "
		    "Link = %s",
		    pak->if_output->namestring,
		    print_linktype(pak->linktype));
	/*
	 * If LCP isn't up yet, just bail out.  Otherwise do an active
	 * open on this protocol as it probably wasn't enabled when we
	 * first negotiated.
	 */
        lcpstate = idb->lcpstate;
        if (lcpstate->phase == UP) {
	    long link;

	    link = pak->linktype;
	    /*
	     * Special case of IP and bridging.
	     */
	    switch (link) {
	    case LINK_IP:
	    case LINK_ARP:
	    case LINK_COMPRESSED_TCP:
	    case LINK_UNCOMPRESSED_TCP:
		if (BRIDGING_IF(idb->firstsw, LINK_IP)) {
		    link = LINK_BRIDGE;
		}
	    }
	    f = ppp_link2fsm(link, idb);
	    if (f) {
	        if ((f->state == CLOSED) || (f->state == PPP_UNKNOWN)) {
		    if (ppp_negotiation_debug)
		        buginf ("\nPPP %s: trying to negotiate NCP for "
		        	"Link = %s",
				pak->if_output->namestring,
			        print_linktype(link));
		    /*
		     * Delay open until next timeout as we may not
		     * not be in a position to block at this time.
		     */
		    fsm_open_delayed(f);
	        }
	    }
	}
	return (FALSE);
    }

    /*
     * check if it is a bridged pkt. If so, add that extra byte
     * with the condition that we don't have pad and it is always 802.3
     * and we don't support LAN ID option, so there is no LAN ID.
     */
    
    if ((pak->linktype == LINK_BRIDGE) || (pak->linktype == LINK_LEX)) {
	pak->datagramstart -= 2;
	PUTSHORT(pak->datagramstart, PPP_FIZC_MACTYPE);
	pak->datagramsize += 2;
	pak->encsize += 2;
    }

    if (pcompression && type <= PPP_MAX_COMPRESSED_PID) {
	*--pak->datagramstart = type; /* Fill in compressed protocol code */
	pak->datagramsize += 1;	/* Count one byte added */
	pak->encsize += 1;
    } else {
	pak->datagramstart -= 2; /* Update start */
	PUTSHORT(pak->datagramstart, type);
	pak->datagramsize += 2;	/* Count two bytes added */
	pak->encsize += 2;
    }	

    /*
     * Check if we're compressing protocol datagrams and if we are
     * allowed to compress THIS datagram.  Skip compression if any
     * other code (like poor TCP, for example) is holding onto this
     * packet.
     */
    if ((pak->refcount < 2) &&
	(type != TYPE_PPP_COMPRESS) &&
	idb->ppp_compression) {
	reg_invoke_ccp_compress_outgoing_pkt(idb, pak);
    }

    /*
     * Handle the Address and Control fields. Note that if we compressed
     * the protocol field, we will be unaligned, so no fancy casts here.
     * If the address and control fields are filled in by an additional
     * layer this must NOT be done.
     */

    if (!accompression && !idb->ppp_new_vencap) {
	pak->datagramstart -= 2; /* Update start */
	PUTSHORT(pak->datagramstart, PPP_STATION); /* Now fill it in */
	pak->datagramsize += 2;	/* Count two bytes added */
	pak->encsize += 2;
    }

    if (ppp_packet_debug) {
	if (pak->linktype == LINK_BRIDGE) {
	    ether_hdr *ether;
	    ether = (ether_hdr *)(pak->datagramstart +
				  pak->if_output->hwptr->span_encapsize);
	    buginf("\nPPP %s(o): pkt type %#x, datagramsize %d", 
		   pak->if_output->namestring, type, pak->datagramsize);
            buginf("\n\tbridged     dest %e, src %e",
		   ether->daddr, ether->saddr);
            if (GETSHORT(&ether->type_or_len) > MAXETHERSIZE) {
                   /* Ethernet */
                buginf(", type %2x",
		       GETSHORT(&ether->type_or_len));
            } else {
		snap_hdr *snap;
		snap = (snap_hdr *)ether->data;
                buginf(", type %2x", GETSHORT(&snap->type));
	    }
	} else {
	    buginf("\nPPP %s(o): pkt type %#x, datagramsize %d", 
		   pak->if_output->namestring,
		   type, pak->datagramsize);
	}

    }

    pak->mac_start  = pak->datagramstart;
    pak->addr_start = NULL;
    pak->rif_start  = NULL;
    pak->riflen	    = 0;
    pak->info_start = NULL;
    pak->enctype    = ET_PPP;


    if (!is_multilink(idb)) {
	/*
	 * Pass the packet on to the additional layer of encapsulation 
	 * if there is one.
	 * Make sure that stays above the board_encap call, since lapb will do
	 * the same. Also don't want to add to fs_cache.
	 */
	if (idb->ppp_new_vencap) {
	    return((*idb->ppp_new_vencap)(pak, address));
	}
	/*
	 * Make sure routing cache is populated by transit traffic.
	 * Board encap for multilink encapsulated packets is done in mlp.c
	 */
	if (idb->board_encap) {
	    if (!(*idb->board_encap)(pak, idb)) {
		return(FALSE);
	    }
	}
	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);

    } else {

	reg_invoke_fs_cache_update(pak->linktype, pak, 0L);
    
	if (mlp_fragment(pak, idb->mlp_link->bundle)) {
	    /*
	     *  Overload pak->mlp_prior_frag with pak->if_output.
	     *  pak->if_output points to the actual output idb for the
	     *    first fragment.  We need to restore pak->if_output to
	     *    point to the virtual bundle interface so that datagram_out
	     *    will call mlp_holdq_enqueue.  We set pak->if_output =
	     *    (idbtype *)pak->mlp_prior_frag in mlp_holdq_enqueue so
	     *    the first fragment will go out over the designated interface.
	     */
	    pak->mlp_prior_frag = (paktype *)pak->if_output;
	    pak->if_output = idb->firstsw;
	} else
            return(FALSE);
    }
    
    return(TRUE);
}


/*
 * ppp_deencap
 * Take down the protocol but don't remove the encapsulation type.
 * Called whenever a new encap type is selected.
 */

static void ppp_deencap (hwidbtype *idb)
{
    fsm *f;
    idbtype *swidb;

    idb->show_support = NULL;
    idb->status |= IDB_BCAST;
    f = idb->lcpfsm;
    if (f) {
	lcp_close(f);
    }
    FOR_ALL_SWIDBS_ON_HW(idb, swidb) {
	if (swidb->tbridge_on_swidb) {
	    idb->ppp_bridge = FALSE;
	    idb->span_bridge_process_force &= ~SPAN_BR_PROC_PPP_BRIDGE;
	}
    }
    if (idb->comp_db) {
        free(idb->comp_db);
	idb->comp_db = NULL;
    }
    /*
     * Clear looped flag here.  It can not be correctly checked from any
     * serial protocol that does not support loop detection so don't
     * burden all the others with clearing the flag.
     */
    idb->inloopback = FALSE;
}

/*
 * ppp_cstate
 * Take down the protocol, the physical line has
 * gone down or bring up the protocol because the line is up.
 */

static void ppp_cstate (register hwidbtype *idb)
{
    lcp_state *lcpstate;
    dialerdbtype *ddb;
    fsm *f;

    /*
     * PPP should not run on a Group Async interface.
     */
    if (IDBTYPE_ASYNC_GROUP == idb->type) {
	return;
    }

    f = idb->lcpfsm;
    lcpstate = idb->lcpstate;

    /*
     * Verify that PPP has been setup completely
     */
    if (!f || !lcpstate) {
	return;
    }
    /*
     * Don't bring up PPP on the dialer interface or D-channel
     */
    ddb = get_dialerdb(idb);
    if (ddb && HUNTGROUP_LEADER(ddb)) {
	return;
    }
    idb->inloopback = FALSE;
    if (idb->state == IDBS_UP) {
	lcp_lowerup(f);		/* xxx well, sort of... */
	if (lcpstate->call_direction == PPP_CALL_IN) {
	    lcp_passiveopen(f);
	} else
	    lcp_activeopen(f);
    } else {
        f->flags &= ~(LOWERUP|AOPENDING|POPENDING);
	lcp_lowerdown(f);
	/*
	 * Clear IP peer address from here so that an address from a pool is
	 * returned if the line goes down.
	 * NOTE:  This could be called from ipcp_down() but that would return
	 *	  the address even if we were just re-negotiating and could
	 *	  result in a new address being allocated for the peer.
	 */
	reg_invoke_clear_ip_peer_addr(idb->firstsw);
    }

    /*
     * If PPP is using an extra layer of encapsulation, ie. LAPB, notify it.
     */
    if (idb->ppp_new_lineaction)
        (*idb->ppp_new_lineaction)(idb);
}

/*
 * ppp_timer
 * Periodic functions for PPP
 */

#define ECHOLEN 8             /* length of echo packet */

static void ppp_timer (register hwidbtype *idb)
{
    fsm *f;
    auth_state *a;


    if ((idb->enctype != ET_PPP) || (idb->lcpfsm == 0)) {
	return;
    }

    /*
     * Ignore if this link is VPN forwarding/forwarded or is
     * a Bundle Interface
     */
    if (ppp_lcpstate(idb, FORWARDED) || ppp_lcpstate(idb, FORWARDING)) {
	return;
    }

    f = idb->lcpfsm;

    /*
     * check for timeouts
     *  check lcp timeouts for non multilink bundle interfaces only
     */

    if (!idb->firstsw->is_bundle_interface) {
	if (TIMER_RUNNING_AND_AWAKE(f->timeouttime)) {
	    fsm_timeout(f);
	}
    }

    if (f->state == CLOSED) {
	idb->ppp_lineup = FALSE;
    }

    /*
     * check for Authentication re-transmits
     *  check authentication timeouts for non multilink bundle interfaces only
     */
    a = idb->authstate;

    if ((a) && !idb->firstsw->is_bundle_interface) {
        if (TIMER_RUNNING_AND_AWAKE(a->retry_timer)) {
            auth_timeout(idb);
	}
    }
   
    /*
     * Don't we have enough timers?  Check the negotiation timer that
     * keeps us from hanging in the negotiation, authentiation or NCP
     * negotiation phases.  Sortof.
     */
    if (TIMER_RUNNING_AND_AWAKE(idb->lcpstate->negtime)) {
	TIMER_STOP(idb->lcpstate->negtime);
	if (ppp_error_debug) {
	    buginf("\nPPP %s: negotiation timeout, dropping connection",
		    idb->hw_namestring);
	}
	/*
	 * We really need a hangup mechanism.  This is the most frequently
	 * used mechanism, but it is not only confusing but it isn't clear
	 * that it does the right thing.
	 */
	lcp_close(f);
    }

    /* 
     * Check on all NCP timeouts if they are started
     * NCP timeout valid only if LCP is open
     */
    if (f->state == OPEN) {
	int ncpidx;

	FOR_ALL_FSMS(idb->ncpfsmidx, f, ncpidx) {
	    if (TIMER_RUNNING_AND_AWAKE(f->timeouttime)) {
		fsm_timeout(f);
	    }
	}
    }

}

/*
 * ppp_echo
 * send echo's if keepalive is set on the interface
 *
 * Note that this is only called when LCP is Open.
 */

static void ppp_echo (hwidbtype *idb)
{
    fsm *f;
    lcp_state *l;
    paktype *pak;
    uchar *outp;

    f = idb->lcpfsm;
    l = idb->lcpstate;

    /*
     * send echo only if keepalive is set 
     */
    if (idb->nokeepalive) {
        l->echo_flag = 0;
        l->echo_cnt = 0;
        l->echo_id = 0;
	/*
	 * State is OPEN, but if keepalive was set previously and then shut
	 * off, we may need to restart lcp again since ppp_rechorep() will
	 * never be called.  The check for TERMINATING is based on the code
	 * in lcp_down().
	 */
        if (l->phase == TERMINATING) {
	    f->state = CLOSED;
	    lcp_activeopen(f);
        }
        return;
    }

    /*
     * send an echo req if not already sent one. If we have already
     * sent a request and haven't received a reply then send the next
     * request only if the MAX_CNT is not exceeded otherwise inform the
     * upper layers that the link is down
     * EXCEPTION: If we are in loopback do not increment echo_cnt.
     *            This will cause us to send ECHOREQ packets until
     *            the loopback condition clears. 
     */
    if ((!l->echo_flag) || (l->echo_flag && l->echo_cnt < MAX_ECHO_CNT)) {
	lcp_options *go;

	go = f->gotoptions;
	l->echo_flag = 1;
	if (idb->inloopback == FALSE)
	    l->echo_cnt++;

	pak = getbuffer(ECHOLEN + DLLHEADERLEN + HEADERLEN);
	if (!pak)
	    return;

	pak->datagramstart = pak->network_start - DLLHEADERLEN;
	outp = (uchar *) PACKET_DATA(pak->datagramstart);

	PPP_PUTLONG(go->neg_magicnumber ? go->magicnumber : 0L, outp);
	ppp_send_packet(TYPE_PPP_LCP, ECHOREQ, ++l->echo_id, pak, ECHOLEN, 
			f->idb);
	if (ppp_packet_debug) {
	    buginf("\nPPP %s: echo_cnt %u, sent id %u, line %s %s",
		   idb->hw_namestring, l->echo_cnt, l->echo_id,
		   idb->ppp_lineup ? "up" : "down",
		   idb->inloopback ? "(looped)" : "");
	}
    } else {
	l->echo_flag = 0;	/* reset them all */
	l->echo_cnt = 0;
	l->echo_id = 0;

	if (ppp_error_debug) {
	    buginf("\nPPP %s: exceeded max retries taking LCP down.",
		    idb->hw_namestring);
	}

	/*
	 * lcp_down clears ppp_lineup, but we want to call it before
	 * calling reset.  AFITR
	 */
	if (idb->ppp_lineup) {
	    lcp_down(f);	/* declare lcp down */
	    (*idb->reset)(idb);
	} else {
	    lcp_down(f);	/* declare lcp down */
	}
    }
}

/*
 * ppp_get_authname()
 *	Return authenticated name of port, if it's a PPP IDB
 */
static boolean
ppp_get_authname(hwidbtype *hw, char **namep)
{
    char *p;
    idbtype *sw;

    /*
     * If there's no SW idb, or it's not currently running PPP,
     * we can't help
     */
    sw = firstsw_or_null(hw);
    if (!sw || (hw->enctype != ET_PPP) || (hw->state != IDBS_UP)) {
	return(FALSE);
    }

    /*
     * Get from old versus new places
     */
    if (old_access_control) {
	p = hw->lcpstate->user;
    } else {
	p = NULL;
	aaa_get_user_p(hw->lcpstate->ustruct, &p);
    }

    /*
     * Return answer
     */
    if (p && *p) {
	*namep = p;
	return(TRUE);
    }

    p = hw->authstate->remote_name;

    /*
     * Return answer
     */
    if (p && *p) {
	*namep = p;
	return(TRUE);
    }

    return(FALSE);
}

/*
 * ppp_put_authname
 *  Insert authenticated name of port, if it's a PPP IDB
 */
static boolean ppp_put_authname (hwidbtype *hwidb, userstruct *ustruct, char *user)
{
    /*
     * If it's not currently running PPP, we can't help
     */
    
    
    if (hwidb->enctype != ET_PPP) {
	return(FALSE);
    }
    
    if (old_access_control) {
	setstring_named(&hwidb->lcpstate->user, user, "PPP user 1");
	if (hwidb->idb_tty && !hwidb->idb_tty->user)
	    setstring_named(&hwidb->idb_tty->user, user, "PPP user 2");
    } else {
	/*
	 * AAA must be supplied with a ustruct
	 */ 
	if (!ustruct)
	    return(FALSE);
	
	aaa_free_user(&hwidb->lcpstate->ustruct);
	hwidb->lcpstate->ustruct = ustruct;
	if (hwidb->idb_tty) {
	    if (aaa_acct_nested) {
		/*
		 * If we started PPP from the EXEC prompt, we want the network
		 * accounting records to be generated before the EXEC-STOP
		 * record.  This will have the side-effect that the "show user"
		 * output for this connection will show the user authenticated
		 * at EXEC-authentication and not PPP-authentication.
		 */
		if (!hwidb->idb_tty->ustruct) {
		    aaa_lock_user(ustruct);
		    hwidb->idb_tty->ustruct = ustruct;
		}
	    } else {
		/*
		 * replace whatever authentication information
		 * existed with the re-authentication we just did
		 * Doing this will also generate an EXEC-STOP record
		 * before the NETWORK-START record if PPP was started
		 * at the EXEC prompt.
		 */
		if (hwidb->idb_tty->ustruct) 
		    aaa_free_user(&hwidb->idb_tty->ustruct);
		aaa_lock_user(ustruct);
		hwidb->idb_tty->ustruct = ustruct;
	    }
	}
    }
    return(TRUE);
}

/*
 * ppp_init
 * Initialization of PPP subsystem
 */

static void ppp_init (subsystype *subsys)
{
    /*
     * Do PPP proper
     */
    ppp_parser_init();
    ppp_debug_init();
    reg_add_media_serial_setup(ET_PPP, ppp_setup, "ppp_setup");
    reg_add_media_copy_encap_fields(ET_PPP, copy_ppp_fields, "copy_ppp_fields");

    reg_add_ppp_continue_vpn(ppp_continue_vpn, "ppp_continue_vpn");
    reg_add_ppp_lcpstate(ppp_lcpstate, "ppp_lcpstate");
    reg_add_ppp_set_lcpstate(ppp_set_lcpstate, "ppp_set_lcpstate");
    reg_add_lcp_ackci_forced(lcp_ackci_forced, "lcp_ackci_forced");
    reg_add_ncp_register(ncp_register, "ncp_register");
    reg_add_ncp_register_link(ncp_register_link, "ncp_register_link");
    reg_add_ppp_link2fsm(ppp_link2fsm, "ppp_link2fsm");
    reg_add_get_authname(ppp_get_authname, "ppp_get_authname");
    reg_add_put_authname(ppp_put_authname, "ppp_put_authname");
    reg_add_lapb_ppp_event_notify(ppp_lapb_event, "ppp_lapb_event");
    reg_add_hwif_state_change(ppp_if_state_change, 
                                    "ppp_if_state_change");
    reg_add_if_statechange_complete(ppp_if_prot_change, 
                                    "ppp_if_prot_change");
    /*
     * Other members of this subsystem
     */
    async_init();
    mlp_subsys_init();

    /*
     * Other misc.
     */
    queue_init(&fsm_authorQ, 0);
}

/*
 * demuxotrej
 * Demultiplex a protocol-reject.
 */

static void demuxprotrej (hwidbtype *idb, ushort protocol)
{
    (*prottbl[prottbl_lookup(protocol)].protrej)(idb, protocol);
}

/*
 * Finite state machine code from here on
 */

/*
 * fsm_init
 * Initialize fsm state.
 */

void fsm_init (fsm *f)
{
    f->state = CLOSED;
    f->id = 0;				/* xxx start with random id? */
}

/*
 * fsm_open_delayed
 * Setup to actively open on next timeout.  Can't open now because active Open
 * may entail a task dismissal.  And because fsm_clear_author() calls free()
 * and we sometimes are called from an interrupt routine.
 * Set state to LISTEN and turn on the timer for a short interval (look at the
 * fsm_timeout() function).  Or, if WASREJECTED is set, set state to CLOSE,
 * the method is similar, but the result is a passive open during the timeout.
 */

static void fsm_open_delayed (fsm *f)
{
    f->flags &= ~(AOPENDING|POPENDING); /* clear pending flags */

    if (f->state == REQSENT ||		/* already actively open(ing)? */
	f->state == ACKRCVD ||
	f->state == ACKSENT ||
	f->state == OPEN)
	return;

    f->flags |= AOPENDING;		/* set Active Open pending flag */
    f->flags |= LOWERUP;		/* set lower up for confreq to go out */

    if (f->state == TERMSENT)		/* closing? */
	return;				/* wait for desired event */

    /*
     * We are forbidden to send frames for a particular NCP once
     * that protocol value has been rejected.  The reject will have
     * closed the state machine.  So what we want to do is a passive
     * open, so that should someone enable this protocol on the other
     * side of this connection, they will send in a packet for this
     * protocol, and then we will start up
     */
    if (!(f->flags & WASREJECTED))
	f->state = LISTEN;
    else
	f->state = CLOSED;

    TIMER_START(f->timeouttime, 1);	/* fire ASAP */
}

/*
 * fsm_activeopen
 * Actively open connection.
 * Set new state, reset desired options and send requests.
 */

void fsm_activeopen (fsm *f)
{
    f->flags &= ~(AOPENDING|POPENDING); /* clear pending flags */
    if (f->state == REQSENT ||		/* already actively open(ing)? */
	f->state == ACKRCVD ||
	f->state == ACKSENT ||
	f->state == OPEN)
	return;
    if (f->state == TERMSENT ||		/* closing or */
	!(f->flags & LOWERUP)) {	/*  lower layer down? */
	f->flags |= AOPENDING;		/* wait for desired event */
	return;
    }
    if (f->callbacks->resetci)
	(*f->callbacks->resetci)(f);	/* reset options */
    fsm_clear_author(f);
    if (!old_access_control) {
	if (f->cached_author) {
	    if (aaa_author_debug) {
	        buginf("\nAAA/AUTHOR/LCP: %s: authorized via cache",
		       f->idb->hw_namestring);
	    }
	} else {
	    if (fsm_author_in_progress(f)) {
	        return;
	    }
	    if (!fsm_do_author(f->idb, f)) {
	        fsm_lowerdown(f);
	        f->flags |= WASREJECTED;
	        return;
	    }
	}
    }
    fsm_sconfreq(f);			/* send configure-request */
    f->state = REQSENT;
    f->retransmits = 0;			/* reset retransmits count */
    f->nakloops = 0;			/* reset nakloops count */
    f->failures = 0;
}

/*
 * fsm_passiveopen
 * Passively open connection.
 * Set new state and reset desired options.
 */

void fsm_passiveopen (fsm *f)
{
    f->flags &= ~(AOPENDING|POPENDING); /* clear pending flags */
    if (f->state == LISTEN ||		/* already passively open(ing)? */
	f->state == OPEN)
	return;
    if (f->state == REQSENT ||		/* active-OPENing or */
	f->state == ACKRCVD ||
	f->state == ACKSENT ||
	f->state == TERMSENT ||		/*  closing or */
	!(f->flags & LOWERUP)) {	/*  lower layer down? */
	f->flags |= POPENDING;		/* wait for desired event */
	return;
    }
    if (f->callbacks->resetci)
	(*f->callbacks->resetci)(f);	/* reset options */
    fsm_clear_author(f);
    f->state = LISTEN;
    f->retransmits = 0;			/* reset retransmits count */
    f->nakloops = 0;			/* reset nakloops count */
    TIMER_STOP(f->timeouttime);         /* no timeouts in passive mode */
}

/*
 * fsm_close
 * Start closing connection.  The CLOSED state.
 */

void fsm_close (fsm *f)
{
    f->flags &= ~(AOPENDING|POPENDING); /* clear pending flags */
    if (f->state == CLOSED ||		/* already CLOSED or closing? */
	f->state == TERMSENT)
	return;
    if (f->state == REQSENT ||		/* timeout pending for OPEN? */
	f->state == ACKRCVD ||
	f->state == ACKSENT)
	TIMER_STOP(f->timeouttime);
    /*
     * CSCdi56478
     * Do not bring the upper layers down if this is a VTY circuit that you
     * are about to disconnect (protocol == LCP) because the VTY code is
     * dependant on the upper layers being UP in order to get the TERM REQ
     * out the pipe.  This code is called from the authentication failure
     * case as well as other spots.
     * See also CSCdi42544 and a similar change in fsm_rtermreq().
     */
    if ((f->idb->type != IDBTYPE_VTY_ASYNC) || (f->protocol != TYPE_PPP_LCP)) {
	if (f->state == OPEN &&		/* OPEN? */
	    f->callbacks->down) {
	    (*f->callbacks->down)(f);	/* inform upper layers we're down */
	}
    }
    if (f->state == ACKSENT ||		/* could peer be open? */
	f->state == OPEN) {
	ppp_send_packet(f->protocol, TERMREQ, f->reqid = ++f->id, NULL, 0, 
	        f->idb); 		/* send terminate-request */
	f->state = TERMSENT;
	f->retransmits = 0;		/* reset retransmits count */
	TIMER_START(f->timeouttime, f->restarttime);
    } else {
	f->state = CLOSED;
	if (f->callbacks->closed)
 	    (*f->callbacks->closed)(f);	/* exit/restart/etc. */
    }
}

/*
 * fsm_timeout
 * Timeout expired.
 */

static void fsm_timeout (fsm *f)
{
    if (ppp_packet_debug)
	buginf ("\nPPP %s: TIMEout: Time= %#Ta State= %s",
	        f->idb->hw_namestring, f->timeouttime, 
		ppp_linkstates[f->state]);

    TIMER_STOP(f->timeouttime);

    switch (f->state) {
    case REQSENT:
    case ACKRCVD:
    case ACKSENT:
	if (f->flags & POPENDING) { 	/* go passive? */
	    f->state = CLOSED;		/* pretend for a moment... */
	    fsm_passiveopen(f);
	    return;
	}
	if ((++f->retransmits > f->maxconfigure) && (f->state == REQSENT)) {
	    /*
	     * Rather than go passive here, what we want to do is start
	     * a timer to slow down the retransmits of CONFREQs.  Do
	     * this in LISTEN state, so that when the timer expires we
	     * try an active open again...
	     *
	     * We have randomly chosen 15 * restarttime as the length of
	     * time to wait before trying.  This will be 30-45 seconds
	     * normally, which is reasonable.
	     *
	     * But before any of this, reset the interface in case we are
	     * failing because the hardware is hung or something.  This
	     * also has the benefit of disconnecting dialup calls if they
	     * aren't responding.
	     */
	    if (f->protocol == TYPE_PPP_LCP)
		(*(f->idb->reset))(f->idb);
	    f->state = LISTEN;
	    f->retransmits = 0;
	    TIMER_START(f->timeouttime, (f->restarttime * 15));
	    return;
	}
	fsm_sconfreq(f);	/* send configure-request */
	if (f->state != ACKSENT)
	    f->state = REQSENT;
	f->nakloops = 0;
	break;
    case TERMSENT:
	if (f->flags & POPENDING) { 	/* go passive? */
	    f->state = CLOSED;		/* pretend for a moment... */
	    fsm_passiveopen(f);
	    return;
	}
	if (++f->retransmits > f->maxtermtransmits) {
 	    /*
	     * we've waited for an ack long enough.  peer probably heard us.
	     */
	    f->state = CLOSED;
	    if (f->callbacks->closed)
		(*f->callbacks->closed)(f); /* exit/restart/etc. */
	    return;
	}
	f->reqid = ++f->id;
	ppp_send_packet(f->protocol, TERMREQ, f->reqid, NULL, 0, f->idb);
	TIMER_START(f->timeouttime, f->restarttime);
	break;
      case CLOSED:
	/*
	 * Just in case, this handles where we've sent a TERMACK, but
	 * we need to restart a protocol.  Go passive and let the other
	 * side start us up.
	 */
        fsm_passiveopen(f);
	break;
      case LISTEN:
        fsm_activeopen(f);
        break;
    }
}

/*
 * fsm_lowerup
 * The lower layer is up.
 * Start active or passive open if pending.
 */

void fsm_lowerup (fsm *f)
{
    f->flags |= LOWERUP;
    if (f->flags & AOPENDING)		/* Attempting Active-Open? */
	fsm_activeopen(f);		/* Try it now */
    else if (f->flags & POPENDING)	/* Attempting Passive-Open? */
	fsm_passiveopen(f);		/* Try it now */
}

/*
 * fsm_lowerdown
 * The lower layer is down.  Inform upper layers.
 */

void fsm_lowerdown (fsm *f)
{
    f->flags &= ~(LOWERUP|AOPENDING|POPENDING);
    TIMER_STOP(f->timeouttime);		/* stop timer just in case */
    if (f->state == OPEN && (f->callbacks->down))
	(*f->callbacks->down)(f);	/* inform upper layers */
    f->state = CLOSED;
    if (f->callbacks->closed)
	(*f->callbacks->closed)(f);	/* exit/restart/etc. */
}

/*
 * is_negotiation_done
 * Stop the negotiation timer, if a useful NCP has opened up.
 */
static void is_negotiation_done (fsm* f)
{
    lcp_state *lcpstate;

    lcpstate = f->idb->lcpstate;

    /*
     * Really we should be dealing with the LINK_xxx type but amazingly
     * that is not directly reachable in this code.  :-(
     */
    if (TIMER_RUNNING(lcpstate->negtime)) {
	switch(f->protocol) {
	case TYPE_PPP_IPCP:
	case TYPE_PPP_IPXCP:
	case TYPE_PPP_OSICP:
	case TYPE_PPP_XNSCP:
	case TYPE_PPP_DECCP:
	case TYPE_PPP_ETHERTALKCP:
	case TYPE_PPP_BRIDGECP:
	case TYPE_PPP_VINESCP:
	case TYPE_PPP_NBFCP:
	case TYPE_PPP_LEXCP:
	case TYPE_PPP_APPNANRCP:
	case TYPE_PPP_LLC2CP:
	    TIMER_STOP(lcpstate->negtime);
	}
    }
}

/*
 * fsm_input
 * Input packet.
 */
const char *const ppp_packettypes[] = {
"",
"CONFREQ", /*		1	 Configuration Request */
"CONFACK", /*		2	 Configuration Ack */
"CONFNAK", /*		3	 Configuration Nak */
"CONFREJ", /*		4	 Configuration Reject */
"TERMREQ", /*		5	 Termination Request */
"TERMACK", /*		6	 Termination Ack */
"CODEREJ", /*		7	 Code Reject */
"PROTREJ", /*		8	 Protocol Reject */
"ECHOREQ", /*		9	 Echo Request */
"ECHOREP", /*		10	 Echo Reply */
"DISCREQ", /*		11	 Discard Request */
"IDENTIFY",/*		12	 Identification */
"UNKNOW",  /*		13	 Uknown */
"RESETREQ",/*		14	 Reset Request */
"RESETACK" /*		15	 Reset Ack */
};

/*
 * Link states.
 */
const char *const ppp_linkstates[] = {
"",
"Closed", /*		1	 Connection closed */
"Listen", /*		2	 Listening for a Config Request */
"REQsent", /*		3	 We've sent a Config Request */
"ACKsent", /*		4	 We've sent a Config Ack */
"ACKrcvd", /*		5	 We've received a Config Ack */
"Open", /*		6	 Connection open */
"TERMsent", /*	        7	 We've sent a Terminate Request */
"Not negotiated"/*      8        protocol not configured on the intf */
};

void fsm_input (fsm *f, paktype *inpacket)
{
    ppp_hdr *ppp_hdrp;
    uchar code, id, *inp;
    short len, l;
    hwidbtype *idb;
    lcp_options *go;
    ulong magic;
    serialsb *ssb;

    if (!(f->flags & LOWERUP)) {
        if (ppp_error_debug)
	    buginf("\nPPP %s: dropping packet.  Lower layer not up",
	            f->idb->hw_namestring);
        goto freepacket;	/* blech, but follow the code flow */
    }

    l = inpacket->datagramsize -
	(inpacket->network_start - inpacket->datagramstart);

    /*
     * parse header (code, id and length).
     * if packet too short, drop it.
     */
    ppp_hdrp = (ppp_hdr *)inpacket->network_start;
    inp = ((uchar *) ppp_hdrp) + HEADERLEN;
    if (l < HEADERLEN) {
	if (ppp_error_debug)
	    buginf("\nPPP %s: rcvd short header for %s.",
		   print_linktype(f->protocol));
	goto freepacket;
    }
    code = ppp_hdrp->code;
    id = ppp_hdrp->id;
    len = ppp_hdrp->plen;
    idb = f->idb;
    ssb = idb_get_hwsb_inline(idb, HWIDB_SB_SERIAL);
    ssb->hdlc_yoursseen = id;
    if (len < HEADERLEN) {
	if (ppp_error_debug)
	    buginf("\nPPP %s: rcvd illegal length for %s.",
		  f->idb->hw_namestring, print_linktype(f->protocol));
	goto freepacket;
    }
    if (len > l) {
	if (ppp_error_debug)
	    buginf("\nPPP %s: rcvd short packet. len %d > %d for %s",
		  f->idb->hw_namestring, len, l, print_linktype(f->protocol));
	goto freepacket;
    }
    len -= HEADERLEN;
    if (ppp_packet_debug)
	buginf("\nPPP %s: input(%#x) state = %s code = %s(%d) id = %d len = %d", 
	       idb->hw_namestring, ppp_get_protocol_type(inpacket),
	       ppp_linkstates[f->state],
	       code > MAX_LCPTYPES ? "UNKNOWN" : ppp_packettypes[code],
	       code, id, ppp_hdrp->plen);

    /*
     * AUTHORIZATION:
     * Do simple authorization here for NCPs.  Just check for yes/no.
     * More complicated author is done below OR by specific NCPs.
     * Don't do it if we did it already.
     */
    if (!old_access_control) {
	if (!f->cached_author) {
	    if (fsm_author_in_progress(f)) {
	        goto freepacket;
	    }
	    if (!fsm_do_author(f->idb, f)) {
		fsm_sprotrej(f->protocol, inpacket->network_start,
			     inpacket->datagramsize - inpacket->encsize,
			     idb->lcpfsm);
	        fsm_lowerdown(f);
	        f->flags |= WASREJECTED;
	        goto freepacket;
	    }
	}
    }

    /*
     * Action depends on code.
     */
    switch (code) {
      case CONFREQ:
	code = CONFACK; /* Assume success */
	if (f->state == TERMSENT) {
            if (ppp_error_debug) {
                buginf("\nPPP %s: dropping packet, state is TERMSENT",
                        idb->hw_namestring);
            }
	    goto freepacket;
	}
	if (f->state == CLOSED) {
	    ppp_send_packet(f->protocol, TERMACK, id, inpacket, 0, idb);
            if (ppp_error_debug) {
                buginf("\nPPP %s: dropping packet, state is CLOSED",
                        idb->hw_namestring);
            }
	    return;
	}
	/*
	 * If we are open and we receive a config request, shut down this
	 * layer, and send a config request.
	 */
	if (f->state == OPEN && f->callbacks->down)
	    (*f->callbacks->down)(f);	/* inform upper layers */
	/*
	 * Also, if we are in passive open (which should be removed), we
	 * should send a config request when we hear one to start back up.
	 */
	if (f->state == OPEN || f->state == LISTEN || f->state == PPP_UNKNOWN) {
	    if (f->state == LISTEN)
	        TIMER_STOP(f->timeouttime);
	    /* xxx possibly need hold-down on open? */
            if (f->callbacks->resetci)
	        (*f->callbacks->resetci)(f); /* reset options */
	    fsm_sconfreq(f);	/* send configure-request */
	}

	if (f->callbacks->reqci)	/* check ci */
	    code = (*f->callbacks->reqci)(f, inp, &len);
	else if (len)
	    code = CONFREJ;		/* reject all ci */

	/*
	 * AUTHORIZATION:
	 * Check to see if NCP did smart authorization.
	 * If so, bail if NCP says so.
	 * If not, the we must bail here if any attributes are mandatory.
	 */
	if (!old_access_control) {
	    if (((f->ncp_did_author) && (code == TERMREQ)) ||
		((!f->ncp_did_author) && (!fsm_check_author(f)))) {
		fsm_sprotrej(f->protocol, inpacket->network_start,
			     inpacket->datagramsize - inpacket->encsize,
			     idb->lcpfsm);
		fsm_lowerdown(f);
		f->flags |= WASREJECTED;
		goto freepacket;
	    }
	}

	/*
	 * count up the number of naks we've sent.  If we send more than
	 * maxfailure, start sending configure-reject instead.
	 */
        if (code == CONFNAK) {
	    /*
	     * If we've exceeded maxfailures, then we've sent too many
	     * NAKs, so send a CONFREJ instead.
	     */
	    if (++f->failures > f->maxfailure) {
		code = CONFREJ;
	    }
        } else
	    f->failures = 0;

	ppp_send_packet(f->protocol, code, id, inpacket, len, idb);

	if (code == CONFACK) {
	    if (f->state == ACKRCVD) {
		TIMER_STOP(f->timeouttime);
		f->state = OPEN;
		/*
		 * If we are opened, clear the WASREJECTED flag, as
		 * this means we know they can accept these packets 
		 */
		f->flags &= ~(WASREJECTED);
		if (f->callbacks->up)
		    (*f->callbacks->up)(f); /* inform upper layers */
		is_negotiation_done(f);
	    } else
		f->state = ACKSENT;
	} else {
	    if (f->state != ACKRCVD)
		f->state = REQSENT;
	}
	return;
    case CONFACK:
	fsm_rconfack(f, inp, id, len);
	break;
    case CONFNAK:
	fsm_rconfnak(f, inp, id, len);
	break;
    case CONFREJ:
	fsm_rconfrej(f, inp, id, len);
	break;
    case TERMREQ:
	fsm_rtermreq(f, id);
	break;
    case TERMACK:
	fsm_rtermack(f);
	break;
    case CODEREJ:
	fsm_rcoderej(f, inp, len);
	break;
    case PROTREJ:
	fsm_rprotrej(idb, inp, len);
	break;
    case ECHOREQ:
	switch (f->state) {
	case OPEN:
	    magic = 0;	
	    go = f->gotoptions;
    	    if (go->neg_magicnumber) {
    		PPP_GETLONG(magic, inp);
		/* NOTE: If this net is in loopback we will not respond
		 *       to the ECHOREQ with an ECHOREP. The first ECHOREP
		 *       that comes in matching our id will clear the
		 *       loopback condition.
		 */
        	if (magic == go->magicnumber) { 
		    idb->inloopback = TRUE; /* we are in loopback now */
	            goto freepacket;
		}
		DECPTR(sizeof(long), inp);
		PPP_PUTLONG(go->magicnumber, inp);
	    }	
	    ppp_send_packet(f->protocol, ECHOREP, id, inpacket, len, f->idb);
	    return;
	}
	break;
    case ECHOREP:
	if (idb->inloopback == TRUE)
	    idb->inloopback = FALSE;    /* loopback condition has cleared */
	fsm_rechorep(f, inp, id, len);  /* validate echo resps */
        break;
    case DISCREQ:
    case IDENTIFY:
    break;
    case RESETREQ:
        if((f->state == OPEN) && (*f->callbacks->resetrequest)(f, inp, len)){
            /*
             * It's a good reset-req, so we must send a reset-ack reply.
             */
            ppp_send_packet(f->protocol, RESETACK, id, inpacket, len, idb);
            return;
        }
    break;
    case RESETACK:
        if(f->state == OPEN)
            (*f->callbacks->resetack)(f, id, inp, len);
    break;
    default:
	/*
	 * we receive FF 03 C0 21 0C 03 00 17 00 00 13 ......
	 * we send    FF 03 C0 21 07 52 00 1B 0C 03 00 17 00 00 13 ...
	 *                       ^           ^
	 * So sizes go up by the size of the xCP header and pointers move
	 * to the left to accomodate slamming in the Code Reject header.
	 */
	inpacket->datagramstart -= HEADERLEN;
	inpacket->network_start -= HEADERLEN;
	inpacket->datagramsize += HEADERLEN;
	len += HEADERLEN;
	ppp_send_packet(f->protocol, CODEREJ, ++f->id, inpacket, len, f->idb);
	return;
    }

    /* 
     * Return the packet
     */
freepacket:
    datagram_done(inpacket);
}

/*
 * fsm_rconfack
 * Receive configure-ack.
 */

static void fsm_rconfack (fsm *f, uchar *inp, uchar id, int len)
{
    hwidbtype *idb;

	idb = f->idb;
    if (ppp_negotiation_debug)
	buginf("\nPPP %s: state = %s fsm_rconfack(%#x): rcvd id %d", 
	    idb->hw_namestring, ppp_linkstates[f->state], f->protocol, id);
    switch (f->state) {
    case LISTEN:
    case CLOSED:
	ppp_send_packet(f->protocol, TERMACK, id, NULL, 0, f->idb);
	break;
    case ACKRCVD:
    case REQSENT:
	/*
 	 * Expected ID?
	 */
	if (id != f->reqid) {
	    if (ppp_error_debug) {
	        buginf("\nPPP %s: dropping packet, id %d didn't match %d", 
		       idb->hw_namestring, id, f->reqid);
	    }
	    break;		/* nope, toss... */
	}

	/* 
	 * Good ack?
	 */
	if (f->callbacks->ackci && (*f->callbacks->ackci)(f, inp, len))  
	    f->state = ACKRCVD;
	else
	    f->state = REQSENT;	/* wait for timeout to retransmit */
	break;

    case ACKSENT:
	if (id != f->reqid) {	/* expected id? */
	    if (ppp_error_debug) {
	        buginf("\nPPP %s: dropping packet, id %d didn't match %d", 
		       idb->hw_namestring, id, f->reqid);
	    }
	    break;		/* nope, toss... */
	}
	if (f->callbacks->ackci &&
	    (*f->callbacks->ackci)(f, inp, len)) { /* good ack? */
	    TIMER_STOP(f->timeouttime);
	    f->state = OPEN;
	    /*
	     * If we are opened, clear the WASREJECTED flag, as
	     * this means we know they can accept these packets 
	     */
	    f->flags &= ~(WASREJECTED);
	    if (f->callbacks->up)
		(*f->callbacks->up)(f);	/* inform upper layers */
	    is_negotiation_done(f);
	}
	else
	    f->state = REQSENT;		/* wait for timeout to retransmit */
	break;
    case OPEN:
	if (f->callbacks->down)
	    (*f->callbacks->down)(f); 	/* inform upper layers */
	f->state = CLOSED;		/* only for a moment... */
	fsm_activeopen(f);		/* restart */
	break;
    }
}

/*
 * fsm_rconfnak
 * Receive configure-nak.
 */

static void fsm_rconfnak (fsm *f, uchar *inp, uchar id, short len)
{
    hwidbtype *idb;

    idb = f->idb;
    if (ppp_negotiation_debug)
	buginf("\nPPP %s: state = %s fsm_rconfnck(%#x): rcvd id %d", 
	       idb->hw_namestring, ppp_linkstates[f->state], f->protocol, id);
    switch (f->state) {
    case LISTEN:
    case CLOSED:
	ppp_send_packet(f->protocol, TERMACK, id, NULL, 0, f->idb);
	break;
    case REQSENT:
    case ACKSENT:
	if (id != f->reqid) {	/* expected id? */
	    if (ppp_error_debug) {
	        buginf("\nPPP %s: dropping packet, id %d didn't match %d", 
		       idb->hw_namestring, id, f->reqid);
	    }
	    break;		/* nope, toss... */
	}
	if (++f->nakloops > f->maxnakloops) {
	    errmsg(&msgsym(CONFNAK, PPP), f->protocol);
	    break;		/* break the loop */
	}
	TIMER_STOP(f->timeouttime);
	if (f->callbacks->nakci)
	    (*f->callbacks->nakci)(f, inp, len);
	fsm_sconfreq(f);	/* send configure-request */
	++f->retransmits;
	break;
    case ACKRCVD:
	f->state = REQSENT;	/* wait for timeout to retransmit */
	break;
    case OPEN:
	if (f->callbacks->down)
	    (*f->callbacks->down)(f); /* inform upper layers */
	f->state = CLOSED;	/* only for a moment... */
	fsm_activeopen(f);	/* restart */
	break;
    }
}

/*
 * fsm_rconfrej
 * Receive configure-rej.
 */

static void fsm_rconfrej (fsm *f, uchar *inp, uchar id, short len)
{
    switch (f->state) {
    case LISTEN:
    case CLOSED:
	ppp_send_packet(f->protocol, TERMACK, id, NULL, 0, f->idb);
	break;
    case REQSENT:
    case ACKSENT:
	if (id != f->reqid) {	/* expected id? */
	    if (ppp_error_debug) {
	        buginf("\nPPP %s: dropping packet, id %d didn't match %d", 
		       f->idb->hw_namestring, id, f->reqid);
	    }
	    break;		/* nope, toss... */
	}
	if (++f->nakloops > f->maxnakloops)
	    break;		/* break the loop */
	TIMER_STOP(f->timeouttime);
	if (f->callbacks->rejci)
	    (*f->callbacks->rejci)(f, inp, len);
	fsm_sconfreq(f);	/* send configure-request */
	++f->retransmits;
	break;
    case ACKRCVD:
	f->state = REQSENT;	/* wait for timeout to retransmit */
	break;
    case OPEN:
	f->state = CLOSED;	/* only for a moment... */
	fsm_activeopen(f);	/* restart */
	break;
    }
}

/*
 * fsm_rtermreq
 * Receive terminate-req.
 */

static void fsm_rtermreq (fsm *f, uchar id)
{
    ppp_send_packet(f->protocol, TERMACK, id, NULL, 0, f->idb);
    switch (f->state) {
    case ACKRCVD:
    case ACKSENT:
	f->state = REQSENT;	/* start over but keep trying */
	/*
	 *  Start over with the original set of options
	 */
	if (f->callbacks->resetci)
	    (*f->callbacks->resetci)(f); /* reset options */
	break;
    case OPEN:
	/*
	 * If this is LCP, make sure that the TERMACK makes it out the
	 * door before we close it.  See fsm_timeout() in order to
	 * understand how the changes below will result in the proper
	 * delay.  Obviously, if that code is changed the code below
	 * should be as well!
	 */
	if (f->protocol == TYPE_PPP_LCP) {
	    /*
	     * CSCdi42544
	     * Don't shut down the upper layers or mark ppp_lineup FALSE as
	     * a VTY needs everything still working to get the TERMACK out.
	     */
	    if (f->idb->type != IDBTYPE_VTY_ASYNC) {
	        if (f->callbacks->down)
	            (*f->callbacks->down)(f); /* inform upper layers */
	    }
	    f->retransmits = f->maxtermtransmits;
	    f->state = TERMSENT;
	} else {
	    if (f->callbacks->down)
	        (*f->callbacks->down)(f); /* inform upper layers */
	    f->state = CLOSED;
	    if (f->callbacks->closed)
	        (*f->callbacks->closed)(f);	/* exit/restart/etc. */
	}
	TIMER_START(f->timeouttime, f->restarttime);
	break;
    }
}

/*
 * fsm_rtermack
 * Receive terminate-ack.
 */

static void fsm_rtermack (fsm *f)
{
    switch (f->state) {
    case OPEN:
	if (f->callbacks->down)
	    (*f->callbacks->down)(f); /* inform upper layers */
	f->state = CLOSED;
	if (f->callbacks->closed)
	    (*f->callbacks->closed)(f);	/* exit/restart/etc. */
	break;
    case TERMSENT:
	TIMER_STOP(f->timeouttime);
	f->state = CLOSED;
	if (f->callbacks->closed)
	    (*f->callbacks->closed)(f);	/* exit/restart/etc. */
	break;
    }
}

/*
 * fsm_rcoderej
 * Receive an code-reject.
 */
static void fsm_rcoderej (fsm *f, uchar *inp, int len)
{
    if (len < sizeof(uchar)) {
	if (ppp_error_debug)
	    buginf("\nPPP: rcvd short code-reject packet!");
	return;
    }
    if (ppp_negotiation_debug)
	buginf("\nPPP: rcvd code-reject for protocol 0x%x", f->protocol);

	if((f->state == OPEN) && (f->callbacks->coderejected))
		    (*f->callbacks->coderejected)(f);
}

/*
 * fsm_rprotrej
 * Receive an protocol-reject.
 * Figure out which protocol is rejected and inform it.
 */

static void fsm_rprotrej (hwidbtype *idb, uchar *inp, short len)
{
    ushort protocol;

    /*
     *  if this is a multilink interface, send the prot rej to
     *     the virtual bundle interface.
     */
    if (is_multilink(idb))
	idb = mlp_get_master_idb(idb);

    PPP_GETSHORT(protocol, inp);
    if (ppp_negotiation_debug)
        buginf("\nPPP %s: protocol reject received for protocol = %#x", 
	        idb->hw_namestring, protocol);
    demuxprotrej(idb, protocol);
}

/*
 * fsm_sconfreq
 * Send a configure-request.
 */

void fsm_sconfreq (fsm *f)
{
    paktype *pak;
    uchar *outp;
    short outlen;

    outlen = (f->callbacks->cilen ? (*f->callbacks->cilen)(f) : 0);

    /*
     * set time out time
     */
    TIMER_START(f->timeouttime, f->restarttime);
    pak = getbuffer(outlen + DLLHEADERLEN + HEADERLEN);
    if (!pak)
	return;
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    outp = (uchar *) PACKET_DATA(pak->datagramstart);
    if (f->callbacks->cilen && f->callbacks->addci)
	(*f->callbacks->addci)(f, outp);
    ppp_send_packet(f->protocol, CONFREQ, f->reqid = ++f->id, pak, outlen, 
		    f->idb);
}

/*
 * fsm_rechorep
 *
 * receive echo replies and validate them and set my own flags 
 */

static void fsm_rechorep (fsm *f, uchar *inp, uchar id, int len)
{
    hwidbtype *idb;
    lcp_state *l;
    ulong rcvmagic;        /* received magic number */
    lcp_options *go;

    idb = f->idb;
    l = idb->lcpstate;
    go = f->gotoptions;

    if (!l)
       return;              /* just make sure that the ptr is valid */

    if (ppp_packet_debug) {
	buginf("\nPPP %s: rcvd id %u, sent id %u, line %s %s",
	       idb->hw_namestring, id, l->echo_id,
	       idb->ppp_lineup ? "up" : "down",
	       idb->inloopback ? "(looped)" : "");
    }

    /* check for magic number too  */

    if (id != l->echo_id)
       return;              /* wrong id, forget it */

    PPP_GETLONG(rcvmagic, inp);

    if ((!go->neg_magicnumber) && (rcvmagic != 0))
        return;

    if (!idb->ppp_lineup && (l->phase == TERMINATING) && (f->state == OPEN)) {
	/*
	 * We need to understand how we got here because it is not in the
	 * least obvious.  We need to think out the cases where PPP Keepalives
	 * are being exchanged but the ppp_lineup flag is not set.  There are
	 * three possible ways this can happen.
	 *
	 * 1) We are still authenticating.  In this case, we do not want to
	 *    call lcp_up() or any other code.  An earlier fix involved adding
	 *    a check to the front of lcp_up() to keep it from being executed
	 *    if the phase is AUTHENTICATING.  With this change, that change
	 *    is no longer necessary and has been ripped out.
	 *
	 * 2) We are terminating.  In this case, we do not want to call
	 *    lcp_up() or any other code as we want the link to go down after
	 *    the TERMREQ/TERMACK exchange.
	 *
	 * 3) We took the NCPs down in ppp_echo() by calling lcp_down().  We
	 *    did this because of too many missed keepalives.  lcp_down() is
	 *    called elsewhere, but only as part of a termination sequence.
	 *    In all of the other cases, the state is set CLOSED.  So we
	 *    should get here if we receive an Echo Reply after resetting
	 *    the interface and it is still connected and Open at both ends.
	 *    The line did not drop so this is likely a leased line.  We can't
	 *    simply do an lcp_up() at this point because bidirectional
	 *    authentication may not complete.  So force LCP renegotiation.
	 *
	 *  CSCdi70254
	 */
	f->state = CLOSED;
        lcp_activeopen(f);
    } else {
        l->echo_flag = 0;
        l->echo_cnt = 0;
    }
}

/*
 * fsm_sprotrej
 * send a protocol reject.  Copy the given data to the reject packet.
 */
void fsm_sprotrej (ushort protocol, uchar *data, int datalen, fsm *f)
{
    paktype *pak;
    uchar *outp;
    int protrejlen;
   
    protrejlen = datalen + sizeof(ushort);
    pak = getbuffer(protrejlen + DLLHEADERLEN + HEADERLEN);
    if (!pak)
        return;
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    outp = PACKET_DATA(pak->datagramstart);
    PPP_PUTSHORT(protocol, outp); /* protocol fills a fixed-length field */
    bcopy(data, outp, datalen);   /* information field follows protocol */
    ppp_send_packet(TYPE_PPP_LCP, PROTREJ, ++f->id, pak, protrejlen, f->idb);
}

static void ppp_cleanup_options (lcp_options *options)
{

    if (options->callback_message) {
	free(options->callback_message);
	options->callback_message = NULL;
    }

    if (options->endpoint) {
	free(options->endpoint);
	options->endpoint = NULL;
    }

    if (options->endpoint_string) {
	free(options->endpoint_string);
	options->endpoint_string = NULL;
    }

}



/*
 * lcp_init
 * Initialize lcp.
 */

static void lcp_init (register hwidbtype *idb)
{
    fsm *f;
    lcp_options *wo;
    lcp_options *ao;
    lcp_options *go;
    lcp_state *lcpstate;

    f = idb->lcpfsm;

    if (!f->wantoptions) {
        f->wantoptions = malloc_named(sizeof(struct lcp_options),
				      "LCP WANT");
	if (!f->wantoptions)
	    return;
    } else {
	ppp_cleanup_options(f->wantoptions);
	memset(f->wantoptions, 0, sizeof(struct lcp_options));
    }

    if (!f->allowoptions) {
        f->allowoptions = malloc_named(sizeof(struct lcp_options),
				       "LCP ALLOW");
	if (!f->allowoptions)
	    return;
    } else {
	ppp_cleanup_options(f->allowoptions);
	memset(f->allowoptions, 0, sizeof(struct lcp_options));
    }

    if (!f->ouroptions) {
        f->ouroptions = malloc_named(sizeof(struct lcp_options),
				     "LCP OUR");
	if (!f->ouroptions)
	    return;
    } else {
	ppp_cleanup_options(f->ouroptions);
	memset(f->ouroptions, 0, sizeof(struct lcp_options));
    }

    if (!f->gotoptions) {
        f->gotoptions = malloc_named(sizeof(struct lcp_options),
				     "LCP GOT");
	if (!f->gotoptions)
	    return;
    } else {
	ppp_cleanup_options(f->gotoptions);
	memset(f->gotoptions, 0, sizeof(struct lcp_options));
    }

    if (!f->heroptions) {
        f->heroptions = malloc_named(sizeof(struct lcp_options),
				     "LCP HER");
	if (!f->heroptions)
	    return;
    } else {
	ppp_cleanup_options(f->heroptions);
	memset(f->heroptions, 0, sizeof(struct lcp_options));
    }

    wo = f->wantoptions;
    ao = f->allowoptions;
    go = f->gotoptions;
    f->protocol = TYPE_PPP_LCP;
    f->retransmits = 0;
    TIMER_STOP(f->timeouttime);
    f->failures = 0;
    /*
     * Timer and retry values should only be set the first time
     * through here. This will allow values that have been configured
     * to remain on this interface even if the encapsulation is
     * changed from PPP and back.
     */
    if (f->timerinitfl == FALSE) {
    	f->restarttime = DEFTIMEOUT;
    	f->maxconfigure = DEFMAXCONFIGURE;
    	f->maxtermtransmits = DEFMAXTERMTRANSMITS;
    	f->maxnakloops = DEFMAXNAKLOOPS;
    	f->maxfailure = DEFMAXFAILURE;
	f->timerinitfl = TRUE;
	f->max_bad_auths = DEFMAXBADAUTHS;
    }
    f->callbacks = &lcp_callbacks;
    f->flags = 0;

    /*
     * If this is an async link, set the async map to quote only ^S and ^Q,
     * since our input is binary, but some device in the way may not be
     * transparent to ^S and ^Q.
     */

    if (idb->status & IDB_ASYNC) {
	wo->neg_asyncmap = 1;		/* Negotiate async map */
	wo->asyncmap = 0x000A0000;	/* Just send ^S and ^Q */
	wo->neg_accompression = 1;	/* Negotiate A/C compression */
	wo->neg_pcompression = 1;	/* Negot. protocol field compression */
    } else {
	wo->neg_asyncmap = 0;
	wo->asyncmap = 0;
	wo->neg_accompression = 0;	/* No A/C compression */
	wo->neg_pcompression = 0;	/* No protocol field compression */
    }

    /*
     * Set up the requested authentication
     */

    wo->neg_auth = 0;		/* Authentication disabled by default */
    wo->neg_magicnumber = 1;
    wo->neg_quality = 0;
    idb->ppp_lapb = FALSE;	/* LAPB should be down before lcp negotiation */
    wo->neg_numbered_mode = 0; /* Default to not negotiating numbered mode */
    wo->num_mode_window = 7; /* Max window size we will support */
    wo->num_mode_address = 0; /* Let the other side choose our address */
    ao->neg_mru = 1;		/* Allow peer to set its MRU */

    /*
     * Set up callback (disallow request and accept) 
     */
    ao->neg_callback = 0;
    ao->neg_callback = CALLBACK_PPP_NOLOCATION;
    wo->neg_callback = 0;

    /*
     * Setup multilink options
     */

    wo->neg_mlp_mrru = 0;
    ao->neg_mlp_mrru = 0;
    wo->mlp_mrru = DEF_MULTILINK_MRRU;
    ao->mlp_mrru = DEF_MULTILINK_MRRU;

    wo->neg_endpoint = 0;
    ao->neg_endpoint = 0;
    wo->endpoint = NULL;
    ao->endpoint = NULL;

    /*
     * Per RFC 1331 we will always allow the peer to negotiate ACCM,
     * regardless of whether it is an async or sync line.  This allows
     * a sync interface to connect to an async to sync converter.
     */
    ao->neg_asyncmap = 1;	/* Allow async maps on all PPP lines */

    /*
     * No harm done if the peer is willing to accept PPP header compression.
     * If we are sync, we will simply ignore it at the right time.
     */
    ao->neg_pcompression = 1; /* Allow protocol field compression */
    ao->neg_accompression = 1; /* Allow A/C field compression */

    ao->neg_auth = 1;
    ao->neg_quality = 1;		/* Allow Link Quality Monitoring */
    ao->neg_magicnumber = 1;
    ao->neg_numbered_mode = wo->neg_numbered_mode;
    ao->num_mode_window = wo->num_mode_window;
    ao->num_mode_address = wo->num_mode_address;

    lcpstate = idb->lcpstate;
    lcpstate->idb = idb;
    lcpstate->phase = DOWN;
    lcpstate->ppp_quality_threshold = 0;
    lcpstate->mlp_max_links = MAXINT;
    lcpstate->mlp_max_frags = MLP_DEFAULT_MAX_FRAGS;
    lcpstate->mlp_min_links = 0;
    lcpstate->mlp_threshold = 0;
    lcpstate->mlp_threshold_confg = FALSE;

    wo->reportingperiod = 0;
    go->reportingperiod = 0;
    fsm_init(f);
}

/*
 * lcp_activeopen
 * Actively open lcp.
 */

static void lcp_activeopen (fsm *f)
{
    lcp_state *lcpstate;


    lcpstate = f->idb->lcpstate;
    lcpstate->phase = ESTABLISHING;
    fsm_activeopen(f);
}

/*
 * lcp_passiveopen
 * Passively open lcp.
 */

static void lcp_passiveopen (fsm *f)
{
    lcp_state *lcpstate;


    lcpstate = f->idb->lcpstate;
    lcpstate->phase = ESTABLISHING;
    fsm_passiveopen(f);
    /*
     * if fsm_passiveopen succeeded in doing the passive open, then start
     * the timer.  We don't want to sit in passive open state for more than
     * 30 seconds to give the other side to send their first request.  When
     * the timer expires, we try an active open.  
     */
    if (f->state == LISTEN) {
        TIMER_START(f->timeouttime, (30 * ONESEC));
    }
}

/*
 * lcp_close
 * Close lcp.
 */

void lcp_close (fsm *f)
{
    fsm_close(f);
}

/*
 * lcp_lowerup
 * The lower layer is up.
 */

void lcp_lowerup (fsm *f)
{
    hwidbtype *idb;
    lcp_state *lcpstate;
    dialerdbtype *ddb;
    char *direction;

    idb = f->idb;
    lcpstate = idb->lcpstate;
    ddb = get_dialerdb(idb);

    if (lcpstate->cfg_direction) {
	lcpstate->call_direction = lcpstate->cfg_direction;
	if (ppp_chap_debug || ppp_negotiation_debug) {
	    buginf("\nPPP %s: using configured call direction",
		   idb->hw_namestring);
	}
    } else {
	if (ddb) {
	    if (reg_invoke_dialer_is_answer(ddb->dialer, ddb)) {
		lcpstate->call_direction = PPP_CALL_IN;
		direction = "callin";
	    } else {
		lcpstate->call_direction = PPP_CALL_OUT;
		direction = "callout";
	    }
	} else {
	    lcpstate->call_direction = PPP_NO_CALL;
	    direction = "dedicated line";
	}
	if (ppp_chap_debug || ppp_negotiation_debug) {
	    buginf("\nPPP %s: treating connection as a %s",
		   idb->hw_namestring, direction);
	}
    }

    /*
     * if a negotiation timeout is configured, now is the time to start it
     */
    if (lcpstate->neg_timeout) {
        TIMER_START(lcpstate->negtime, lcpstate->neg_timeout);
    }

    fsm_lowerup(f);
}

/*
 * lcp_lowerdown
 * The lower layer is down.
 */

static void lcp_lowerdown (fsm *f)
{
    /*
     * Stop the negotiation timer.
     */
    TIMER_STOP(f->idb->lcpstate->negtime);

    fsm_lowerdown(f);
}

/*
 * lcp_input
 * Input lcp packet.
 */

static void lcp_input (register paktype *pak, register hwidbtype *idb)
{
    if (ppp_packet_debug)
	lcp_print(pak);
    fsm_input(idb->lcpfsm, pak);
}

/*
 * lcp_protrej
 * A protocol-reject was received.  Can't reject lcp!
 * This routine is called from the standard table and so has 
 * the standard aurgument defined. Although it is not used
 * in this case.
 */

static void lcp_protrej (hwidbtype *idb, ushort protocol)
{

 /* This is a serious problem. LCP must NEVER be rejected */
    if (ppp_error_debug)
	buginf("\nPPP %s: received protocol-reject for lcp!", 
	       idb->hw_namestring);
}

/*
 * lcp_resetci
 * Reset our ci.
 */

static void lcp_resetci (fsm *f)
{
    lcp_options *go, *wo, *ao, *ho;
    tt_soc *tty;
    boolean ask_authen = TRUE;
    lcp_state *lcpstate;
    char *hostname_ptr;

    tty = f->idb->idb_tty;
    lcpstate = f->idb->lcpstate;

    if (old_access_control) {
	if (tty && tty->user && *(tty->user) && tty->saved_passwd[0] &&
	    (f->localauthen & (PPP_LAUTHEN_IFNEED|PPP_PT_LAUTHEN_IFNEED))) {
	    /* 
	     * PPP_PT_LAUTHEN_IFNEED signals that this is a vty-async
	     * connection for which a login was required.  We
	     * propagate the login user's name here so that any slipons
	     * we send will contain it. 
	     */
	    ask_authen = FALSE;
	    /* Save username from tty in lcpstate */
	    setstring_named(&lcpstate->user, tty->user, "PPP user 3");
	} else {
	    /* Free username and clear pointer in lcpstate */
	    setstring(&lcpstate->user, NULL);
	}
    } else { /* ! old_access_control */
	/*
	 * lcp_resetci() is called from many places.  So there is a chance
	 * that we will check for aaa_dup_ifneeded() more than once.  If
	 * we are, the ustruct pointers in lcpstate and the tty will
	 * be the same.  So instead of freeing the ustruct in lcpstate
	 * (and generating a NETWORK-STOP record before we may really
	 * want it (see bug pertaining to aaa_acct_nested), we will
	 * decrement the ref count and set lcpstate->ustruct to NULL.
	 * Now we can check aaa_dup_ifneeded() safely for a second time.
	 */
	if (lcpstate->ustruct == f->idb->idb_tty->ustruct) {
	    aaa_unlock_user(&lcpstate->ustruct);
	    lcpstate->ustruct = NULL;
	} else {
	    aaa_free_user(&lcpstate->ustruct);	/* Checks for NULL */
	}
	/* Get authen info from tty if its there and if-needed is configured */
	if (aaa_dup_ifneeded(tty, lcpstate->acc_list_name, &lcpstate->ustruct,
			     AAA_SVC_PPP)) {

	    if (aaa_authen_debug) {
		buginf("\nAAA/AUTHEN: Method=IF-NEEDED: "
		       "no authentication is needed");
	    }

	    ask_authen = FALSE;
	}
    }

    if ((ask_authen) &&
	(f->negotiate_callin_only) &&
	(lcpstate->call_direction == PPP_CALL_OUT)) {
	if (ppp_negotiation_debug)
	    buginf("\nPPP %s: No remote authentication for call-out",
		   f->idb->hw_namestring);
	ask_authen = FALSE;
    }
    
    wo = f->wantoptions;
    /*
     * Check that the lapb subsystem is actually present before negotiating
     * its use.
     */
    if (!reg_used_lapb_setup_encap(ET_PPP))
        wo->neg_numbered_mode = FALSE;

    /*
     * Negotiate MRU iff interface MTU is not equal to PPP default MRU
     */
    wo->mru = f->idb->maxmtu;
    wo->neg_mru = (wo->mru != DEFMRU) ? 1 : 0;

    /*
     *  We are just about to wipe out gotoptions
     *  Clean up any strings that may be malloc'ed to avoid memory
     *    leaks.
     */

    go = f->gotoptions;
    
    if (go->endpoint) {
	free(go->endpoint);
	go->endpoint = NULL;
    }
    
    if (go->endpoint_string) {
	free(go->endpoint_string);
	go->endpoint_string = NULL;
    }

    bcopy(f->wantoptions, f->gotoptions, sizeof(lcp_options));
    ao = f->allowoptions;
    go->magicnumber = create_unique_number();
    if (ask_authen == FALSE) {
	go->neg_auth = 0;
    }
    go->reportingperiod = f->idb->keep_period * 100;
    wo->reportingperiod = f->idb->keep_period * 100;

    /*
     * We need to allow numbered mode in our direction if someone has
     * negotiated it.
     */
    ao->neg_numbered_mode = go->neg_numbered_mode;
    /*
     * Address and Control field compression is not allowed when numbered mode
     * is being negotiated.
     */
    if (wo->neg_numbered_mode)
    {
	ao->neg_accompression = FALSE;
        go->neg_accompression = FALSE;
    }

    /*
     * On an async link, reset the map to the default
     */

    if (f->idb->status & IDB_ASYNC)
      async_setmap(f->idb, 0xFFFFFFFF);	/* Set the default map */

    if (lcpstate->call_direction == PPP_CALL_IN)
      go->neg_callback = 0;

    /*
     * free any callback resources, we have
     */
    ho = f->heroptions;
    if (ho && ho->callback_message) {
	free(ho->callback_message);
	ho->callback_message = NULL;
    }

    /*
     *  Get endpoint discriminator stuff ready for negotiation
     */

    if (ho && ho->endpoint) {
	free(ho->endpoint);
	ho->endpoint = NULL;
    }
	
    if (go->neg_endpoint) {
	hostname_ptr = ppp_hostname(f->idb);
	if (hostname_ptr && (hostname_ptr[0] != '\0')) {
	    go->endpoint_size = min(PPP_MAX_ENDPOINT_LENGTH,
				    strlen(hostname_ptr));
	    go->endpoint = malloc_named(go->endpoint_size,
					"PPP Endpoint go" );
	    if (!go->endpoint) {
		go->neg_endpoint = 0;
	    } else {
		bcopy(hostname_ptr, go->endpoint, go->endpoint_size);
	    }
	} else {
	    /*
	     *  Hostname does not exist on the router so don't send
	     *    the endpoint in config request.
	     */
	    go->neg_endpoint = 0;
	}
    }
}

/*
 * lcp_cilen
 * Return length of our ci.
 */

static int lcp_cilen (fsm *f)
{
    lcp_options *go;
    lcp_options *wo;

    go = f->gotoptions;
    wo = f->wantoptions;
    return(LENCISHORT(go->neg_mru) +
           LENCICHAR(go->neg_callback) +
	   LENCISHORT(go->neg_mlp_mrru) +
	   LENCIENDPOINT(go->neg_endpoint, go->endpoint_size) +
	   LENCILONG(go->neg_asyncmap) +
	   LENCIAUTH(go->neg_auth, go->authtypes[0]) +
	   LENCIQUAL(go->neg_quality) +
	   LENCILONG(go->neg_magicnumber) +
	   LENCIVOID(go->neg_pcompression) +
	   LENCIVOID(go->neg_accompression) +
       LENCINMODE(go->neg_numbered_mode));
}

/*
 * lcp_addci
 * Add our desired cis to a packet.
 */

static void lcp_addci (fsm *f, uchar *ucp)
{
    lcp_options *go, *wo;
    hwidbtype* idb;

    go = f->gotoptions;
    wo = f->wantoptions;
    idb = f->idb;

    ADDCISHORT(CI_MRU, go->neg_mru, go->mru)
    ADDCILONG(CI_ASYNCMAP, go->neg_asyncmap, go->asyncmap)
    ADDCIAUTH(CI_AUTHTYPE, go->neg_auth, go->authtypes[0])
    ADDCIQUAL(CI_QUALITYTYPE, go->neg_quality, go->qualitytype, 
				go->reportingperiod)
    ADDCILONG(CI_MAGICNUMBER, go->neg_magicnumber, go->magicnumber)
    if (go->neg_pcompression) {
	PUTCHAR(CI_PCOMPRESSION, ucp); 
	PUTCHAR(2, ucp);
    }
    if (go->neg_accompression) {
	PUTCHAR(CI_ACCOMPRESSION, ucp); 
	PUTCHAR(2, ucp);
    }
    ADDCINUMBEREDMODE(CI_NUMBEREDMODE, go->neg_numbered_mode,
                      go->num_mode_window, go->num_mode_address)
    ADDCICHAR(CI_CALLBACK, go->neg_callback, go->callback)
    ADDCISHORT(CI_MULTILINK_MRRU, go->neg_mlp_mrru, go->mlp_mrru)
    ADDCIENDPOINT(CI_ENDPOINT_DISC, go->neg_endpoint, go->endpoint,
		  go->endpoint_size)
}

/*
 * lcp_ackci
 * Ack our cis.
 * Returns: FALSE - ack was bad, TRUE - ack was good.
 */

static int lcp_ackci (fsm *f, uchar *p, int len)
{
    lcp_options *go, *wo;
    uchar cilen, citype;
    uchar cichar;
    ushort cishort;
    ulong cilong;
    int save_len = len;
    uchar *save_p = p;

    go = f->gotoptions;
    wo = f->wantoptions;

    /*
     * cis must be in exactly the same order that we sent.
     * Check packet length and ci length at each step.
     * If we find any deviations, then this packet is bad.
     */
    ACKCISHORT(CI_MRU, go->neg_mru, go->mru)
    ACKCILONG(CI_ASYNCMAP, go->neg_asyncmap, go->asyncmap)
    ACKCIAUTH(CI_AUTHTYPE, go->neg_auth, go->authtypes[0])
    ACKCIQUAL(CI_QUALITYTYPE, go->neg_quality, go->qualitytype,
				go->reportingperiod)
    ACKCILONG(CI_MAGICNUMBER, go->neg_magicnumber, go->magicnumber)
    ACKCIVOID(CI_PCOMPRESSION, go->neg_pcompression)
    ACKCIVOID(CI_ACCOMPRESSION, go->neg_accompression)
    ACKCINUMBEREDMODE(CI_NUMBEREDMODE, go->neg_numbered_mode,
                      go->num_mode_window, go->num_mode_address)
    ACKCICHAR(CI_CALLBACK, go->neg_callback, go->callback)
    ACKCISHORT(CI_MULTILINK_MRRU, go->neg_mlp_mrru, go->mlp_mrru)
    ACKCIENDPOINT(CI_ENDPOINT_DISC, go->neg_endpoint)

    /*
     * If there are any remaining cis, then this packet is bad.
     */
    if (len != 0) {
bad:
	if (ppp_error_debug)
	    buginf("\nPPP: %s received bad configuration ACK!",
	            f->idb->hw_namestring);
	return(FALSE);
    }

    if (f->confack_rcv)
       free(f->confack_rcv);
    f->confack_rcv = malloc_named(save_len, "VPN CONFACK Rcv");
    if (f->confack_rcv) {
       bcopy(save_p, f->confack_rcv, save_len);
       f->confack_rcv_len = save_len;
    }
    f->idb->inloopback = FALSE;
    return(TRUE);
}


/*
 * lcp_ackci_forced
 * Accept the options of last CONFACK (sent by client) received by the 
 * remote node (aka NAS) as if it these options were sent by _this_ node's 
 * CONFREQ.
 * This mechanism is used to make the terminating end of the L2F Tunnel
 * (aka this node, the Home-Gateway) assume the negotiated CONFREQ that the 
 * the NAS sent and was eventually agreed upon by the client (last CONFACK). 
 * This means setting the gotoptions accordingly.
 * Returns: FALSE - ack was bad, TRUE - ack was good.
 * If PPP_TEST_ASYNC/PPP_TEST_MLP is TRUE, this routine is only used to 
 * test whether the original (remote) interface is an async/negotiated 
 * MLP options. There is no setting of gotoptions.
 */
static boolean lcp_ackci_forced (fsm *f, uchar *inp, int len, int authtype, 
  				 int option_test)
{
    uchar *cip;		/* pointer to current ci */
    uchar cilen, citype;	/* parsed len, type, char value */
    uchar cichar;		/* parsed byte value */
    ushort cishort;		/* parsed short value */
    ulong cilong;		/* parse long value */
    int rc = CONFACK;		/* final packet return code */
    int orc;			/* individual option return code */
    uchar *p = inp;		/* pointer to next char to parse */
    uchar *ucp = inp;		/* pointer to current output char */
    int l = len;		/* length left */
    lcp_options *go;
    hwidbtype *idb;
    uchar *putchar, *getchar, *endpoint_ptr;
    int i, j;
    ulong temp_ulong;
    int size;

    idb = f->idb;
    go = f->gotoptions;

    /*
     * process all her options.
     */
    while (l) {
	orc = CONFACK;			/* assume success */
	cip = p;			/* remember begining of ci */
	if (l < 2 ||		/* not enough data for ci header or */
	    p[1] < 2 ||		/*  ci length too small or */
	    p[1] > l) {		/*  ci length too big? */
	    if (ppp_error_debug)
		buginf("\nPPP %s: bad CI length = %d, %d!", 
		        idb->hw_namestring, l, p[1]);
	    orc = CONFREJ;	/* reject bad ci */
	    cilen = l;		/* reject till end of packet */
	    l = 0;		/* don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* parse ci type */
	GETCHAR(cilen, p);		/* parse ci length */
	l -= cilen;			/* adjust remaining length */
	cilen -= 2;			/* adjust cilen to just data */
        if (ppp_negotiation_debug)
	    buginf("\nPPP %s: Forced local config for type = %d (%s)", 
	           idb->hw_namestring, citype,
		   citype > MAX_LCPOPTIONS ? "UNKNOWN" :
		   lcp_optionnames[citype]);
	switch (citype) {	/* check ci type */
	case CI_MRU:
	    if (cilen != sizeof(short)) { /* Check CI length */
		INCPTR(cilen, p); /* Skip rest of CI */
		orc = CONFREJ;	/* Reject CI */
		break;
	    }
	    PPP_GETSHORT(cishort, p); /* Parse MRU */
	    if (ppp_negotiation_debug)
		buginf(" value = %d", cishort);

	    /*
	     * He must be able to receive at least our minimum.
	     * No need to check a maximum.  If he sends a large number,
	     * we'll just ignore it.
	     */
	    if (cishort < MINMRU) {
		orc = CONFREJ;	/* Nak CI */
		break;
	    }
            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_mru = 1;	/* Remember he sent an MRU */
	    go->mru = cishort;	/* And remember value */
	    break;

	case CI_MULTILINK_MRRU:
	    if (cilen != sizeof(short)) { /* Check CI length */
		INCPTR(cilen, p); /* Skip rest of CI */
		orc = CONFREJ;	/* Reject CI */
		break;
	    }
	    PPP_GETSHORT(cishort, p); /* Parse MRRU */
	    if (ppp_negotiation_debug)
		buginf(" value = %d", cishort);

	    /*
	     * He must be able to receive at least our minimum.
	     * No need to check a maximum.  If he sends a large number,
	     * we'll just ignore it.
	     */
	    if (cishort < MINMRU) {
		orc = CONFREJ;	/* Nak CI */
		break;
	    }
            
            if (option_test == PPP_TEST_MLP)
               return(TRUE);  
            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_mlp_mrru = 1;	/* Remember he sent an MRU */
	    go->mlp_mrru = cishort;	/* And remember value */
	    break;

        case CI_ENDPOINT_DISC:
	    if ((cilen < 1) || 
		(cilen > (20 + 1))) { /* Check CI length */
		INCPTR(cilen, p); /* Skip rest of CI */
		orc = CONFREJ;	/* Reject CI */
		break;
	    }

	    GETCHAR(go->endpoint_class, p);
	    endpoint_ptr = p;
	    INCPTR((cilen - 1), p);

	    go->neg_endpoint = 1;	
	    go->endpoint_size = cilen - 1;

	    if (go->endpoint) {
		free(go->endpoint);
		go->endpoint = NULL;
	    }
	    if (go->endpoint_string) {
		free(go->endpoint_string);
		go->endpoint_string = NULL;
	    }
	    if ((go->endpoint_size > 0) &&
		(go->endpoint_size <= PPP_MAX_ENDPOINT_LENGTH)) {
		go->endpoint = malloc_named((go->endpoint_size+1), 
				      "PPP Endpoint go");
		if (!go->endpoint) {
		    go->neg_endpoint = 0;
		    break;
		}
		/*
		 * (2 * PPP_MAX_ENDPOINT_LENGTH) + 1
		 *  2 * ... needed because endpoint disc.  may have to 
		 *    be converted from hex to ascii.  This is 2 bytes
		 *    ascii for every one byte of hex.
		 *  ... + 1 needed to terminate string with NULL.
		 */
		go->endpoint_string =
		    malloc_named(((2*PPP_MAX_ENDPOINT_LENGTH)+1),
				 "PPP Endpoint str");
		if (!go->endpoint_string) {
		    go->neg_endpoint = 0;
		    free(go->endpoint);
		    go->endpoint = NULL;
		    break;
		}
		bcopy(endpoint_ptr, go->endpoint, go->endpoint_size);
		
		/* add a null termination for safety */
		go->endpoint[go->endpoint_size] = '\0';  

		/*
		 *  Put the Ascii representation of the endpoint descriminator
		 *   in dialerdb->remote_name.  This will get overwritten in
		 *   dialer_remote_name if authenticating over a ddr
		 *   interface
		 */
		switch (go->endpoint_class) {
		  case ENDPOINT_NULL:
		    strcpy(go->endpoint_string, "Null");
		    break;
		    
		  case ENDPOINT_LOCAL:
		  case ENDPOINT_PHONE_NUMBER:
		    if (is_printable_string(go->endpoint, go->endpoint_size)) {
			/*
			 *  Endpoint Disc. is a printable ASCII string so use
			 *   it as is.
			 */
			strcpy(go->endpoint_string, go->endpoint);
		    } else {
			/*
			 *  Convert the ED to ascii
			 */
			j = 0;
			for(i = 0;i < go->endpoint_size; i++) {
			    go->endpoint_string[j++] =
				tohexchar(go->endpoint[i] >> 4);
			    go->endpoint_string[j++] = 
				tohexchar(go->endpoint[i] & 0x0F);
			}
			/* make sure string is null terminated */
			go->endpoint_string[j] = '\0';
		    }
		    break;
		    
		  case ENDPOINT_IP_ADDR:
		    sprintf(go->endpoint_string, "%i", GETLONG(go->endpoint));
		    break;
		    
		  case ENDPOINT_IEEE_802:
		    sprintf(go->endpoint_string, "%e", go->endpoint);
		    break;
		    
		  case ENDPOINT_MAGIC:
		    /*
		     *  create endpoint string of form:
		     *    magic1.magic2.magic3....
		     *  option may have 5 magic numbers.
		     */
		    putchar = go->endpoint_string;
		    getchar = go->endpoint;
		    size = go->endpoint_size;
		    if (size >= sizeof(ulong)) {
			PPP_GETLONG(temp_ulong, getchar);
			putchar += sprintf(putchar, "%x", temp_ulong);
			size -= sizeof(ulong);
			while (size >= sizeof(ulong)) {
			    PPP_GETLONG(temp_ulong, getchar);
			    putchar += sprintf(putchar, "%x", temp_ulong);
			    size -= sizeof(ulong);
			}
		    }
		    break;
		    
		}
		if (ppp_negotiation_debug)
		    buginf(" value = %s", go->endpoint_string);
		
		if (is_ddr(idb) && idb->dialerdb->remote_name) {
		    strcpy(idb->dialerdb->remote_name, go->endpoint_string);
		}
	    } 
	    
	    break;


	case CI_ASYNCMAP:
	    if (cilen != sizeof(long)) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    PPP_GETLONG(cilong, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %#x", cilong);

            if (option_test == PPP_TEST_ASYNC) 
                return(TRUE);
            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_asyncmap = 1; /* Remember we got an async map */
	    go->asyncmap = cilong; /* And remember the map itself */
	    break;

	case CI_AUTHTYPE:
	    if (cilen < sizeof(short)) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    PPP_GETSHORT(cishort, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %#x", cishort);

	    /*
	     * Authtype could be CHAP, UPAP or TAC.
	     */
	    if ((cishort != TYPE_PPP_CHAP) && (cishort != TYPE_PPP_UPAP)) {
		orc = CONFREJ;
		INCPTR(cilen - sizeof(ushort), p);
		break;
	    }
	    /*
	     * If Authtype is UPAP we have all the config data.
	     */
	    if (cishort == TYPE_PPP_UPAP) {
		go->neg_auth = 1;
		go->authtypes[0] = PPP_AUTH_PAP;
		break;
	    }
            go->authtypes[0] = 0;
	    GETCHAR(cichar, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %d", cichar);
	    /*
	     * Digest must be MD5
	     */
	    if (cichar != DIGEST_MD5) {
		orc = CONFREJ;
		INCPTR(cilen - (sizeof(ushort) + sizeof(uchar)), p);
		break;
	    }

	    INCPTR(cilen - (sizeof(ushort)+sizeof(uchar)), p);

            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_auth = 1;
	    go->authtypes[0] = PPP_AUTH_CHAP;
	    break;

	case CI_QUALITYTYPE:
	    if (cilen < (sizeof(short) + sizeof(long))) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    PPP_GETSHORT(cishort, p); /* quality protocol must be 0xC025 */
	    if (cishort != TYPE_PPP_LQR) {
		orc = CONFREJ;
                break;
	    }
	    PPP_GETLONG(cilong, p); /* reporting period if 0 NAK */ 
	    if (cilong == 0 && !idb->nokeepalive) {
		orc = CONFREJ;
		break;
	    }
	    /*
	     * They are requesting a reporting period that is less than
	     * our keepperiod. NAK them.
	     */
	    if ((cilong != 0) && (cilong < (idb->keep_period * 100))) {
		orc = CONFREJ;
		break;
	    }

            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_quality = 1;
	    break;

	case CI_MAGICNUMBER:
	    if (cilen != sizeof(long)) {
		INCPTR(cilen, p);
		orc = CONFREJ;

		break;
	    }
	    PPP_GETLONG(cilong, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %#x", cilong);

            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_magicnumber = 1;
	    go->magicnumber = cilong;
	    break;

	case CI_RESERVED:
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;

	case CI_PCOMPRESSION:
	    if (cilen != 0) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	   
            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_pcompression = 1;
	    break;

	case CI_ACCOMPRESSION:
	    if (cilen != 0) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }

            if (option_test != PPP_TEST_NONE)
                break;

	    go->neg_accompression = 1;
	    break;

	case CI_32FCS:

	    INCPTR(cilen, p);
            orc = CONFREJ;
	    
            if (option_test != PPP_TEST_NONE)
                break;

	    break;

        case CI_CALLBACK:
            if (cilen < sizeof(uchar)) {
 		INCPTR(cilen, p);
 		orc = CONFREJ;
 		break;
 	    }
            GETCHAR(cichar, p);   /* Parse Callback operation */

            if (option_test != PPP_TEST_NONE)
                break;

             /*
             * NB: Callbacks are not supported in VPN.
             * So don't remember the the callback option
             */           
             
 	    if (ppp_negotiation_debug) {
 		buginf(" value = %#x", cichar);
                buginf("\n CALLBACK option is ignored");
	    }
            break;
 
        case CI_NUMBEREDMODE:
            if (cilen < 2) {
               INCPTR(cilen, p);
               orc = CONFREJ;
               break;
            }
            /*
             * First check her requested window size
             */
            GETCHAR(cichar, p);
            if (ppp_negotiation_debug)
               buginf(" window = %d", cichar);

            if (cichar == 0)
            {
               orc = CONFREJ;
               INCPTR(cilen - sizeof(uchar), p);
               break;
            }
            else
               go->num_mode_window = cichar;

            /*
             * Now check her address choice
             */
            GETCHAR(cichar, p);
            if (ppp_negotiation_debug)
               buginf(" address = %d", cichar);

           if (cichar == 0) {
              buginf("\n Value is illegal");
              orc = CONFREJ;
              break;  
           }

           if (option_test != PPP_TEST_NONE)
              break;

           go->num_mode_address = cichar;
           go->neg_numbered_mode = 1;
           break;

	default:
	    if (ppp_negotiation_debug)
		buginf("\nPPP %s: rcvd unknown option %d", 
		        idb->hw_namestring, citype);
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;
	}
	cilen += 2;			/* adjust cilen whole ci */

endswitch:
	if (ppp_negotiation_debug && 
            (option_test == PPP_TEST_NONE)) {
	    if (orc == CONFACK)
		buginf(" acked");
	    if (orc == CONFREJ)
		buginf(" rejected");
	    if (orc == CONFNAK)
		buginf(" nacked");
 	}

	if (orc == CONFACK &&		/* good ci */
	    rc != CONFACK)		/*  but prior ci wasnt? */
	    continue;			/* don't send this one */

	if (orc == CONFNAK) {		/* nak this ci? */
	    if (rc == CONFREJ)		/* rejecting prior ci? */
		continue;		/* don't send this one */
	    if (rc == CONFACK) {	/* ack'd all prior cis? */
		rc = CONFNAK;		/* not anymore... */
		ucp = inp;		/* backup */
	    }
	}
	if (orc == CONFREJ &&		/* reject this ci */
	    rc != CONFREJ) {		/*  but no prior ones? */
	    rc = CONFREJ;
	    ucp = inp;			/* backup */
	}
	if (ucp != cip)                 /* need to move ci? */ 
	    bcopy(cip, ucp, cilen);	/* move it */
	INCPTR(cilen, ucp);		/* update output pointer */
    }

    /*
     * xxx if we wanted to send additional naks (for unsent cis), the
     * code would go here.  this must be done with care since it might
     * require a longer packet than we received.
     */

   len = ucp - inp;			/* compute output length */

   /*
    * If this were a test for LCP options, no options were 
    * found.
    */
   if (option_test != PPP_TEST_NONE)
      return(FALSE);

   if (rc == CONFACK)
      return(TRUE);
   else
      return(FALSE);
} 


/*
 * lcp_nakci
 * Nak some of our cis.
 */

static void lcp_nakci (fsm *f, uchar *p, int len)
{
    ushort cishort;
    ulong cilong;
    uchar cichar;
    uchar cichar1;
    uchar cichar2;
    uchar authtype;

    lcp_options *go;
    lcp_options *wo;

    go = f->gotoptions;
    wo = f->wantoptions;

    /*
     * we don't care if they want to send us smaller packets than
     * we want.  therefore, accept any mru less than what we asked for.
     * if they send us a bigger mru than what we asked, just use the
     * default mru.
     */
    NAKCISHORT(CI_MRU, go->neg_mru,
	       if ((cishort <= MAX(wo->mru, DEFMRU)) && (cishort >= MINMRU)) {
		    go->mru = cishort;
	       } else {

		   /*
		    * The remote peer sent us a MRU which is larger than
		    * the one that we offered. Since we can not come
		    * to an agreement, don't attempt to negotiate the
		    * option.
		    */

		   go->neg_mru = 0;
	       }
	       )

    NAKCILONG(CI_ASYNCMAP, go->neg_asyncmap,

	      /*
	       * If they sent us a NAK with a different mask, then
	       * OR that mask into the requested mask, and try again.
	       * This represents a compromise between what we offered
	       * and what they offered.
	       *
	       * However, if they sent us a NULL mask, then we should
	       * simply use that. This will prevent negotiation loops.
	       */

	      if (cilong)
		go->asyncmap |= cilong;
	      else {
		  if (go->asyncmap)
		    go->asyncmap = 0; /* Try a null mask */
		  else
		    go->neg_asyncmap = 0; /* Null didn't work */
	      }	      
	      )
    /*
     * If they NAK'd our authentication request, see if we have configured
     * to allow the method they propose.  If so, change to that method for
     * the next try.  If not, and we have another method to try, change to
     * that method.  Else, ignore the NAK, and send the first method in the
     * list.
     * NOTE:  It is VERY VERY important that the NAKCIAUTH block of code
     *		can never set the "go->neg_auth" argument to zero.  If it
     *		does, then security can be compromised.
     */
    NAKCIAUTH(CI_AUTHTYPE, go->neg_auth, go->authtypes[0],
	      authtype = protocol_to_authtype(cishort, cichar1);
	      if (authtype != go->authtypes[0]) {
		  if (ppp_negotiation_debug)
		    buginf("\nPPP %s: NAK'd Authentication, wanted %s"
			   " got %s(%#x/%d)",
			   f->idb->hw_namestring,
			   authtype_name(go->authtypes[0]),
			   authtype_name(authtype), cishort, cichar1);
		  get_another_auth_method(f, authtype);
		  /*
		   * Whether we have another method or not, give him a
		   * limited number of NAKs/REJs to jerk us around before
		   * we hang up.  This counter only affects NAKs/REJs as
		   * the timer has its own counter.  This should happen
		   * rapidly if we are not going to converge so the extra
		   * cycles should not hurt and we have not opened the door
		   * for them yet so there should be no security hole.
		   * This is no worse than the peer ignoring all of our
		   * configuration attempts.
		   */
		  if (go->neg_auth > 1)
		      go->neg_auth--;
		  if (go->neg_auth == 1) {
		      if (ppp_negotiation_debug) {
			  buginf("\nppp: %s closing connection because "
				 "remote won't authenticate.",
				 f->idb->hw_namestring);
		      }
		      lcp_close(f);        /* declare lcp down */
		  }
	      }
	      )
    /*
     * If they NAK'ed our reporting period and the period they are
     * requesting is larger than our reporting period accept their
     * value but don't change our reporting period. This means
     * we will be transmitting faster than they expect which is
     * not in violation of RFC-1333.
     */
    NAKCIQUAL(CI_QUALITYTYPE, go->neg_quality,
		  if (cilong > wo->reportingperiod)
		      go->reportingperiod = cilong;
		  
	      )
    NAKCILONG(CI_MAGICNUMBER, go->neg_magicnumber,
	      go->magicnumber = create_unique_number(); 
	      if (ppp_error_debug || ppp_negotiation_debug)
		  buginf("\n%%%s appears to be looped back.", 
			 f->idb->hw_namestring);
	      f->idb->inloopback = TRUE;
	      ) 
    NAKCIVOID(CI_PCOMPRESSION, go->neg_pcompression,
	       go->neg_pcompression = 0;
	      )
    NAKCIVOID(CI_ACCOMPRESSION, go->neg_accompression,
	       go->neg_accompression = 0;
	      )
    NAKCINUMBEREDMODE(CI_NUMBEREDMODE, go->neg_numbered_mode,
                      if (cichar1 && (cichar1 <= wo->num_mode_window))
                          go->num_mode_window = cichar1;
                      else
                          go->neg_numbered_mode = 0;
                      if (cichar2 == 1 || cichar2 == 3)
                          go->num_mode_address = cichar2;
                      else
                          go->neg_numbered_mode = 0;
                     )
    NAKCICHAR(CI_CALLBACK, go->neg_callback,
 	       go->neg_callback = 0;
 	      )

    NAKCISHORT(CI_MULTILINK_MRRU, go->neg_mlp_mrru,
	       if ((cishort <= wo->mlp_mrru) && (cishort >= MINMRU))
		    go->mlp_mrru = cishort;
	       else {

		   /*
		    * The remote peer sent us a MRU which is larger than
		    * the one that we offered. Since we can not come
		    * to an agreement, don't attempt to negotiate the
		    * option.
		    */

		   go->neg_mlp_mrru = 0;
	       }
	       )

    NAKCIENDPOINT(CI_ENDPOINT_DISC, go->neg_endpoint,
		  go->neg_endpoint = 0;
		  )

   
    /*
     * This is where we check for additional options specified.
     */
    while (len >= 2) {

	/*
	 * we should only be here, if our MRU is set to the default so we
	 * were not negotiating.
	 * we don't care if they want to send us smaller packets than
	 * we want.  therefore, accept any mru less than the PPP default MRU.
	 * if they send us a bigger mru than the default mru, just use the
	 * default mru.
	 */
	NAKCISHORT(CI_MRU, !go->neg_mru,
		   if ((cishort <= MAX(wo->mru, DEFMRU)) &&
		       (cishort >= MINMRU)) {
			go->mru = cishort;
			if (cishort != DEFMRU) {
			    go->neg_mru = 1;
			}
		   }
		   )

	else {
	    int optlen;

	    if (ppp_negotiation_debug || ppp_error_debug)
		buginf("\nPPP %s: ignoring NAK for type = %d (%s) len = %d",
		       f->idb->hw_namestring, p[0],
		       p[0] > MAX_LCPOPTIONS ? "UNKNOWN" :
		       lcp_optionnames[p[0]], p[1]);
	    optlen = p[1];
	    if ((optlen > len) || (optlen < 2))
		break;	/* we should end up going thru the buginf below */
	    p += optlen;
	    len -= optlen;
	}
    }

    if (len) {
	if (ppp_negotiation_debug || ppp_error_debug)
	    buginf("\nPPP %s: received bad configuration NAK!",
		    f->idb->hw_namestring);
    }
}

/*
 * lcp_rejci
 * Reject some of our cis.
 */

static void lcp_rejci (fsm *f, uchar *p, int len)
{
    ushort cishort;
    ulong cilong;
    uchar cichar;
    uchar cichar2;
    lcp_options *go, *wo;

    go = f->gotoptions;
    wo = f->wantoptions;
    REJCISHORT(CI_MRU, go->neg_mru, go->mru)
    REJCILONG(CI_ASYNCMAP, go->neg_asyncmap, go->asyncmap)
    /*
     * NOTE:  It is VERY VERY important that the REJCIAUTH block of code
     *		can never set the "neg" argument to zero.  If it does, 
     *		then security can be compromised.
     *
     * NOTE:  This code was moved here from the REJCIAUTH macro definition
     *		in ppp.h because it was too confusing to pass in necessary
     *		pointers.
     */
    if (go->neg_auth &&
	p[0] == CI_AUTHTYPE &&
	p[1] <= len &&
	p[1] >= (2 + sizeof(short))) {
	int optlen = p[1];
	len -= optlen;
	INCPTR(2, p);
	PPP_GETSHORT(cishort, p);
	/* check rejected value. */
	if (go->authtypes[0] == PPP_AUTH_CHAP) {
	    if (optlen != 2 + sizeof(short) + sizeof(char))
	        goto bad;
	    if (cishort != TYPE_PPP_CHAP)
	        goto bad;
	    GETCHAR(cichar, p);
	    if (cichar != DIGEST_MD5)
	        goto bad;
	} else {
	    if (optlen != 2 + sizeof(short))
	        goto bad;
	    if (cishort != TYPE_PPP_UPAP)
	        goto bad;
	    cichar = 0;
	}
	if (ppp_negotiation_debug)
	    buginf("\nppp: config REJ received, type = %d (CI_AUTHTYPE),"
		   " value = %#x/%d",
		   CI_AUTHTYPE, cishort, cichar);
	get_another_auth_method(f, 0);
	/*
	 * Whether we have another method or not, give him a
	 * limited number of NAKs/REJs to jerk us around before
	 * we hang up.  This counter only affects NAKs/REJs as
	 * the timer has its own counter.  This should happen
	 * rapidly if we are not going to converge so the extra
	 * cycles should not hurt and we have not opened the door
	 * for them yet so there should be no security hole.
	 * This is no worse than the peer ignoring all of our
	 * configuration attempts.
	 */
	if (go->neg_auth > 1)
	    go->neg_auth--;
	if (go->neg_auth == 1) {
	    if (ppp_negotiation_debug) {
		buginf("\nppp: %s closing connection because remote "
		       "won't authenticate.", f->idb->hw_namestring);
	    }
	    lcp_close(f);        /* declare lcp down */
	}
    }

    REJCIQUAL(CI_QUALITYTYPE, go->neg_quality, go->qualitytype,
				go->reportingperiod)
    REJCILONG(CI_MAGICNUMBER, go->neg_magicnumber, go->magicnumber)
    REJCIVOID(CI_PCOMPRESSION, go->neg_pcompression)
    REJCIVOID(CI_ACCOMPRESSION, go->neg_accompression)
    REJCINUMBEREDMODE(CI_NUMBEREDMODE, go->neg_numbered_mode,
                      go->num_mode_window, go->num_mode_address)
    REJCICHAR(CI_CALLBACK, go->neg_callback, go->callback)
    REJCISHORT(CI_MULTILINK_MRRU, go->neg_mlp_mrru, go->mlp_mrru)
    REJCIENDPOINT(CI_ENDPOINT_DISC, go->neg_endpoint)

    /*
     * If there are any remaining cis, then this packet is bad.
     */
    if (len == 0)
	return;
bad:
    if (ppp_error_debug)
	buginf("\nPPP %s: received bad configuration reject!",
	        f->idb->hw_namestring);
}

/*
 * lcp_reqci
 * Check the peer's requested cis and send appropriate response.
 * Returns: confack, confnak or confrej and input packet modified
 * appropriately.
 *
 *                      ****NOTE**** 
 * new CI_xxxx must also be added to lcp_ackci_forced for Link Level 2
 * Forwarding to work.
 */


static uchar lcp_reqci (fsm *f, uchar *inp, short *len)
{
    uchar *cip;		/* pointer to current ci */
    uchar cilen, citype;	/* parsed len, type, char value */
    uchar cichar;		/* parsed byte value */
    ushort cishort;		/* parsed short value */
    ulong cilong;		/* parse long value */
    int rc = CONFACK;		/* final packet return code */
    int orc;			/* individual option return code */
    uchar *p = inp;		/* pointer to next char to parse */
    uchar *ucp = inp;		/* pointer to current output char */
    int l = *len;		/* length left */
    int skip;			/* number of bytes to skip at end of option */
    lcp_options *go;
    lcp_options *ho;
    lcp_options *ao;
    lcp_options *wo;
    hwidbtype *idb;
    lcp_state *lcpstate;
    int i, j;                      /* counter */
    uchar *putchar, *getchar, *endpoint_ptr;
    ulong temp_ulong;
    int size;
    hwidbtype *lead;
    boolean will_do_chap;	/* TRUE if we will do CHAP */

    idb = f->idb;
    go = f->gotoptions;
    ho = f->heroptions;
    ao = f->allowoptions;
    wo = f->wantoptions;
    lcpstate = idb->lcpstate;
    lead = get_netidb(idb);

    /*
     * reset all her options.
     */
    ho->neg_mru = 0;
    ho->neg_mlp_mrru = 0;
    ho->neg_endpoint = 0;
    ho->neg_asyncmap = 0;
    ho->neg_auth = 0;
    ho->neg_quality = 0;
    ho->neg_magicnumber = 0;
    ho->neg_pcompression = 0;
    ho->neg_accompression = 0;
    ho->neg_numbered_mode = 0;
    ho->neg_callback = 0;
    ho->num_mode_window = 0;
    ho->num_mode_address = 0;

    /*
     * If ppp callback initiate is configured, heroptions for callback
     * are always enabled. 
     */ 
    if ((ao->neg_callback) && (ao->callback == CALLBACK_PPP_NONEG))
       ho->neg_callback = 1;

    /*
     * process all her options.
     */
    while (l) {
	orc = CONFACK;			/* assume success */
	cip = p;			/* remember begining of ci */
	if (l < 2 ||		/* not enough data for ci header or */
	    p[1] < 2 ||		/*  ci length too small or */
	    p[1] > l) {		/*  ci length too big? */
	    if (ppp_error_debug)
		buginf("\nPPP %s: bad CI length = %d, %d!", 
		        idb->hw_namestring, l, p[1]);
	    orc = CONFREJ;	/* reject bad ci */
	    cilen = l;		/* reject till end of packet */
	    l = 0;		/* don't loop again */
	    goto endswitch;
	}
	GETCHAR(citype, p);		/* parse ci type */
	GETCHAR(cilen, p);		/* parse ci length */
	l -= cilen;			/* adjust remaining length */
	cilen -= 2;			/* adjust cilen to just data */
        if (ppp_negotiation_debug)
	    buginf("\nPPP %s: received config for type = %d (%s)", 
	           idb->hw_namestring, citype,
		   citype > MAX_LCPOPTIONS ? "UNKNOWN" :
		   lcp_optionnames[citype]);
	switch (citype) {	/* check ci type */
	case CI_MRU:
	    if (cilen != sizeof(short)) { /* Check CI length */
		INCPTR(cilen, p); /* Skip rest of CI */
		orc = CONFREJ;	/* Reject CI */
		break;
	    }
	    PPP_GETSHORT(cishort, p); /* Parse MRU */
	    if (ppp_negotiation_debug)
		buginf(" value = %d", cishort);

	    if (!ao->neg_mru) {		/* Allow option? */
		/*
		 * If the option is not allowed, still accept an MRU
		 * negotiation for the default value.  So many implementations
		 * send this that we have interoperability problems otherwise
		 */
		if (cishort != DEFMRU) {
		    orc = CONFREJ;
		    break;
		}
	    }			
	    /*
	     * He must be able to receive at least our MTU.
	     * No need to check a maximum.  If he sends a large number,
	     * we'll just accept it here and ignore it later on.
	     */
	    if (cishort < idb->maxmtu) {
		orc = CONFNAK;	/* Nak CI */
		DECPTR(sizeof(short), p); /* Backup */
		PPP_PUTSHORT(idb->maxmtu, p); /* Give him a hint */
		break;
	    }
	    ho->neg_mru = 1;	/* Remember he sent an MRU */
	    ho->mru = cishort;	/* And remember value */
	    break;

	case CI_MULTILINK_MRRU:
	    if (!ao->neg_mlp_mrru ||
		cilen != sizeof(short)) { /* Check CI length */
		INCPTR(cilen, p); /* Skip rest of CI */
		orc = CONFREJ;	/* Reject CI */
		break;
	    }
	    PPP_GETSHORT(cishort, p); /* Parse MRRU */
	    if (ppp_negotiation_debug)
		buginf(" value = %d", cishort);

	    /*
	     * He must be able to receive at least our MTU.
	     * No need to check a maximum.  If he sends a large number,
	     * we'll just ignore it.
	     */
	    if (cishort < idb->maxmtu) {
		orc = CONFNAK;	/* Nak CI */
		DECPTR(sizeof(short), p); /* Backup */
		PPP_PUTSHORT(idb->maxmtu, p); /* Give him a hint */
		break;
	    }
	    ho->neg_mlp_mrru = 1;	/* Remember he sent an MRU */
	    ho->mlp_mrru = cishort;	/* And remember value */
	    break;

	case CI_ENDPOINT_DISC:
	    if (!ao->neg_endpoint ||
		(cilen < 1) || 
		(cilen > (20 + 1))) { /* Check CI length */
		INCPTR(cilen, p); /* Skip rest of CI */
		orc = CONFREJ;	/* Reject CI */
		break;
	    }

	    GETCHAR(ho->endpoint_class, p);
	    endpoint_ptr = p;
	    INCPTR((cilen - 1), p);

	    ho->neg_endpoint = 1;	
	    ho->endpoint_size = cilen - 1;

	    if (ho->endpoint) {
		free(ho->endpoint);
		ho->endpoint = NULL;
	    }
	    if (ho->endpoint_string) {
		free(ho->endpoint_string);
		ho->endpoint_string = NULL;
	    }
	    if ((ho->endpoint_size > 0) &&
		(ho->endpoint_size <= PPP_MAX_ENDPOINT_LENGTH)) {
		ho->endpoint = malloc_named((ho->endpoint_size+1), 
				      "PPP Endpoint ho");
		if (!ho->endpoint) {
		    ho->neg_endpoint = 0;
		    break;
		}
		/*
		 * (2 * PPP_MAX_ENDPOINT_LENGTH) + 1
		 *  2 * ... needed because endpoint disc.  may have to 
		 *    be converted from hex to ascii.  This is 2 bytes
		 *    ascii for every one byte of hex.
		 *  ... + 1 needed to terminate string with NULL.
		 */
		ho->endpoint_string =
		    malloc_named(((2*PPP_MAX_ENDPOINT_LENGTH)+1),
				 "PPP Endpoint str");
		if (!ho->endpoint_string) {
		    ho->neg_endpoint = 0;
		    free(ho->endpoint);
		    ho->endpoint = NULL;
		    break;
		}
		bcopy(endpoint_ptr, ho->endpoint, ho->endpoint_size);
		
		/* add a null termination for safety */
		ho->endpoint[ho->endpoint_size] = '\0';  

		/*
		 *  Put the Ascii representation of the endpoint descriminator
		 *   in dialerdb->remote_name.  This will get overwritten in
		 *   dialer_remote_name if authenticating over a ddr
		 *   interface
		 */
		switch (ho->endpoint_class) {
		  case ENDPOINT_NULL:
		    strcpy(ho->endpoint_string, "Null");
		    break;
		    
		  case ENDPOINT_LOCAL:
		  case ENDPOINT_PHONE_NUMBER:
		    if (is_printable_string(ho->endpoint, ho->endpoint_size)) {
			/*
			 *  Endpoint Disc. is a printable ASCII string so use
			 *   it as is.
			 */
			strcpy(ho->endpoint_string, ho->endpoint);
		    } else {
			/*
			 *  Convert the ED to ascii
			 */
			j = 0;
			for(i = 0;i < ho->endpoint_size; i++) {
			    ho->endpoint_string[j++] =
				tohexchar(ho->endpoint[i] >> 4);
			    ho->endpoint_string[j++] = 
				tohexchar(ho->endpoint[i] & 0x0F);
			}
			/* make sure string is null terminated */
			ho->endpoint_string[j] = '\0';
		    }
		    break;
		    
		  case ENDPOINT_IP_ADDR:
		    sprintf(ho->endpoint_string, "%i", GETLONG(ho->endpoint));
		    break;
		    
		  case ENDPOINT_IEEE_802:
		    sprintf(ho->endpoint_string, "%e", ho->endpoint);
		    break;
		    
		  case ENDPOINT_MAGIC:
		    /*
		     *  create endpoint string of form:
		     *    magic1.magic2.magic3....
		     *  option may have 5 magic numbers.
		     */
		    putchar = ho->endpoint_string;
		    getchar = ho->endpoint;
		    size = ho->endpoint_size;
		    if (size >= sizeof(ulong)) {
			PPP_GETLONG(temp_ulong, getchar);
			putchar += sprintf(putchar, "%x", temp_ulong);
			size -= sizeof(ulong);
			while (size >= sizeof(ulong)) {
			    PPP_GETLONG(temp_ulong, getchar);
			    putchar += sprintf(putchar, "%x", temp_ulong);
			    size -= sizeof(ulong);
			}
		    }
		    break;
		    
		}
		if (ppp_negotiation_debug)
		    buginf(" value = %s", ho->endpoint_string);
		
		if (is_ddr(idb) && idb->dialerdb->remote_name) {
		    strcpy(idb->dialerdb->remote_name, ho->endpoint_string);
		}
	    } 
	    
	    break;

	case CI_ASYNCMAP:
	    if (!ao->neg_asyncmap ||
		cilen != sizeof(long)) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    PPP_GETLONG(cilong, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %#x", cilong);
	    ho->neg_asyncmap = 1; /* Remember we got an async map */
	    ho->asyncmap = cilong; /* And remember the map itself */
	    break;

	/*
	 * Responding to the Authentication option is difficult for a couple
	 * of reasons.  First, it is not a fixed length and the NAK logic
	 * currently does not support expanding an option.  Second, outbound
	 * PAP is a security hole, so NAKing with PAP is probably not a good
	 * idea, even if we haven't completely disabled outbound PAP.  So we
	 * will establish a few simple rules and see if we can get the code
	 * to match.  :-)
	 *  Rule 1 - Ack if the format is OK and the protocol is supported.
	 *           In the case of PAP, only ACK if the PAP sent name has
	 *           been configured or AAA is enabled, otherwise outbound
	 *           PAP is disabled.
	 *  Rule 2 - Nak with CHAP/MD5 if offered CHAP/DES and format is OK.
	 *  Rule 3 - Nak with CHAP/MD5 if field is 5 characters or longer.
	 *  Rule 4 - Nak with PAP if field is 4 characters long.
	 *           Only NAK if the PAP sent name has been configured or AAA
	 *           is enabled, otherwise outbound PAP is disabled.
	 *  Rule 5 - Reject all other cases.
	 *
	 * To close the CHAP security hole, we will amend the logic above to
	 * refuse CHAP and not suggest CHAP if so configured.
	 */
	case CI_AUTHTYPE:
	    if (!ao->neg_auth ||
		cilen < sizeof(short)) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    PPP_GETSHORT(cishort, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %#x", cishort);

	    will_do_chap = TRUE;
	    if (lcpstate->chap_refuse) {
		if ((lcpstate->chap_refuse == PPP_REFUSE_CHAP) ||
		    (lcpstate->call_direction == PPP_CALL_IN)) {
		    will_do_chap = FALSE;
		}
	    }
	    /*
	     * Authtype must be a supported protocol (CHAP or PAP).
	     * And if the Authtype is CHAP, there must be space for the digest.
	     */
	    if (((cishort != TYPE_PPP_CHAP) && (cishort != TYPE_PPP_UPAP)) ||
		((cishort == TYPE_PPP_CHAP) &&
		 (!will_do_chap || (cilen < PPP_CHAP_OPT_SIZE)))) {
		if (will_do_chap && (cilen >= PPP_CHAP_OPT_SIZE)) {
		    /*
		     * Rule 3 - Nak with CHAP/MD5 if field is 5 characters or
		     *		longer and outbound CHAP was not turned off.
		     */
		    orc = CONFNAK;
		    DECPTR(sizeof(short), p);
		    PPP_PUTSHORT(TYPE_PPP_CHAP, p);
		    PUTCHAR(DIGEST_MD5, p);
		    INCPTR(cilen - PPP_CHAP_OPT_SIZE, p);
		} else if ((cilen == PPP_PAP_OPT_SIZE) &&
			   (lcpstate->pap_sent_name ||
			    (lead->lcpstate && lead->lcpstate->pap_sent_name) ||
			     !old_access_control)) {
		    /*
		     * Rule 4 - Nak with PAP if field is 4 characters long.
		     *		Check that the PAP sentname has been configured
		     *		or that we are running AAA.
		     */
		    orc = CONFNAK;
		    DECPTR(sizeof(short), p);
		    PPP_PUTSHORT(TYPE_PPP_UPAP, p);
		} else {
		    /*
		     * Rule 5 - Reject all other cases.
		     */
		    orc = CONFREJ;
		    INCPTR(cilen - sizeof(short), p);
		}
		break;
	    }
	    /*
	     * If Authtype is UPAP we have all the config data.
	     */
	    if (cishort == TYPE_PPP_UPAP) {
		/*
		 * Rule 1 - Ack if the format is OK and the protocol is
		 *  supported.  Note that we are a little generous in what
		 *  we accept as we are not concerned if the option contains
		 *  trailing garbage.
		 * Before accepting PAP, make sure it is enabled or that we
		 * are running AAA.  Note that we cannot, at this point, NAK
		 * with CHAP, because the option is too small and we don't
		 * know if we can expand the response packet.
		 */
		if (lcpstate->pap_sent_name ||
		    (lead->lcpstate && lead->lcpstate->pap_sent_name) ||
		     !old_access_control) {
		    ho->neg_auth = 1;
		    ho->authtypes[0] = PPP_AUTH_PAP;
		} else {
		    orc = CONFREJ;
		}
		skip =  cilen - sizeof(short);
	    } else {
	        /*
	         * Only case left is CHAP.  We have already verified that there
	         * is space for the digest.  Now verify that it is MD5 (5).
	         */
	        GETCHAR(cichar, p);
	        if (ppp_negotiation_debug)
		    buginf(" digest = %d", cichar);
	        if (cichar == DIGEST_MD5) {
		    /*
		     * Rule 1 - Ack if the format is OK and the protocol is
		     *  supported.  Note that we are a little generous in what
		     *  we accept as we are not concerned if the option contains
		     *  trailing garbage.
		     */
	            ho->neg_auth = 1;
	            ho->authtypes[0] = PPP_AUTH_CHAP;
	        } else {
		    /*
		     * Rule 3 - Nak with CHAP/MD5 if offered CHAP/DES and format
		     *  is OK.  Note that we are a little generous in what we
		     *  accept as we are not concerned if the option contains
		     *  trailing garbage.
		     */
		    orc = CONFNAK;
		    DECPTR(sizeof(uchar), p);
		    PUTCHAR(DIGEST_MD5, p);
	        }
		skip =  cilen - (sizeof(short) + sizeof(char));
	    }
	    if (skip) {
		INCPTR(skip, p);
		if (ppp_negotiation_debug)
		    buginf("\nPPP %s: Ignoring %d extra characters in the"
			   " option.", idb->hw_namestring, skip);
	    }
	    break;

	case CI_QUALITYTYPE:
	    if (!ao->neg_quality ||
		cilen < (sizeof(short) + sizeof(long))) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    PPP_GETSHORT(cishort, p); /* quality protocol must be 0xC025 */
	    if (cishort != TYPE_PPP_LQR) {
		orc = CONFNAK;
		DECPTR(sizeof(short), p);
		cishort = TYPE_PPP_LQR;
		PPP_PUTSHORT(cishort, p);
		cilong = idb->keep_period * 100;
		PPP_PUTLONG(cilong, p);
		INCPTR(cilen - (sizeof(short) + sizeof(long)), p);
		break;
	    }
	    PPP_GETLONG(cilong, p); /* reporting period */ 
	    /*
	     * NAK if they request a reporting period of 0 and keepalives
	     * are enabled, or if they request a reporting period that is
	     * less than our keepalive period.
	     */
	    if ((cilong == 0 && !idb->nokeepalive) ||
		((cilong != 0) && (cilong < (idb->keep_period * 100)))) {
		orc = CONFNAK;
		DECPTR(sizeof(long), p);
		cilong = idb->keep_period * 100;
		PPP_PUTLONG(cilong, p);
	    } else {
		ho->neg_quality = 1;
	    }
	    /*
	     * ignore trash at end of option
	     */
	    INCPTR(cilen - (sizeof(short) + sizeof(long)), p);
	    break;

	case CI_MAGICNUMBER:
	    if (!ao->neg_magicnumber ||
		cilen != sizeof(long)) {
		INCPTR(cilen, p);
		orc = CONFREJ;

		break;
	    }
	    PPP_GETLONG(cilong, p);
	    if (ppp_negotiation_debug)
		buginf(" value = %#x", cilong);
	    /*
	     * She must have a different magic number.
	     */
	    if (go->neg_magicnumber &&
		cilong == go->magicnumber) {
		orc = CONFNAK;
		DECPTR(sizeof(long), p);
		cilong = create_unique_number();
		/* don't put create_unique_number() inside macro! */
		PPP_PUTLONG(cilong, p);
		break;
	    }
	    ho->neg_magicnumber = 1;
	    ho->magicnumber = cilong;
	    f->idb->inloopback = FALSE;
	    break;

	case CI_PCOMPRESSION:
	    if (!ao->neg_pcompression ||
		cilen != 0) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    ho->neg_pcompression = 1;
	    break;

	case CI_ACCOMPRESSION:
	    if (!ao->neg_accompression ||
		cilen != 0) {
		INCPTR(cilen, p);
		orc = CONFREJ;
		break;
	    }
	    ho->neg_accompression = 1;
	    break;

	case CI_RESERVED:
	case CI_32FCS:
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;

        case CI_CALLBACK:
            if (!ao->neg_callback ||
                 cilen < sizeof(uchar)) {
 		INCPTR(cilen, p);
 		orc = CONFREJ;
 		break;
 	    }
            GETCHAR(cichar, p);   /* Parse Callback operation */
             
 	    if (ppp_negotiation_debug)
 		buginf(" value = %#x", cichar);
 
	    /*
	     * Mustn't update the pointer 'p' until we are done with the
	     * option.  Nor change the value of cilen!!!!
	     */
            if (!ppp_callback_accept(ho, cichar, (cilen - sizeof(uchar)), p)) {
		orc = CONFREJ;
            } else {
		/*
		 * remember the other side's (her) callback option
		 */
		ho->neg_callback = 1;
		ho->callback = cichar; 
	    }
	    INCPTR((cilen - sizeof(uchar)), p);
            break;
 
    case CI_NUMBEREDMODE:
        if (!ao->neg_numbered_mode || cilen < 2) {
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;
        }
        /*
         * First check her requested window size
         */
        GETCHAR(cichar, p);
        if (ppp_negotiation_debug)
            buginf(" window = %d", cichar);

        if (cichar == 0 || (cichar > ao->num_mode_window))
        {
            DECPTR(sizeof(uchar), p);
            orc = CONFNAK;
            PUTCHAR(wo->num_mode_window, p);
            INCPTR(cilen - sizeof(uchar), p);
            break;
        }
        else
            ho->num_mode_window = cichar;

        /*
         * Now check her address choice
         */
        GETCHAR(cichar, p);
        if (ppp_negotiation_debug)
            buginf(" address = %d", cichar);
        /*
         * Check if she wants to be told what address to use.
         * Also, if she hasn't choosen a valid address value suggest one.
         */
        if (cichar != 1 && cichar != 3) /* This covers (cichar == 0) */
        {
            /*
             * If she has address 0, she is asking us to choose. It's possible
             * from the above if () that she gave us an invalid address. We
             * will suggest something sensible if so. We assume in that case
             * that its the equivalent of her address being 0.
             */
            cichar = 0;

            /*
             * 1st check: Did we choose our own address?
             */
            if (go->num_mode_address != 0)
                cichar = (go->num_mode_address == 1) ? 3 : 1;
            else
            {
                /*
                 * 2nd check: Who has the largest window size?
                 */
                if (ho->num_mode_window &&
                    (go->num_mode_window != ho->num_mode_window))
                    cichar = (go->num_mode_window > ho->num_mode_window) ?
                                1 : 3;
                else
                {
                    /*
                     * 3rd and final chance: Who's got the largest
                     * magic number?
                     */
                    if (go->magicnumber && ho->magicnumber)
                        cichar = (go->magicnumber > ho->magicnumber) ?
                                    1 : 3;
                }
            }
            /*
             * Are we still clueless as to what to suggest?
             */
            if (cichar == 0)
            {
                /*
                 * Your are now entering the twilight zone... DoDeDoDo..etc.
                 * We'll pick either address randomly.
                 */
                cichar = (create_unique_number() & 0x01) ? 1 : 3;
            }

            ho->num_mode_address = cichar;
            DECPTR(sizeof(uchar), p);
            orc = CONFNAK;
            PUTCHAR(ho->num_mode_address, p);
            break;            
        }

        /*
         * Check that she didn't choose the same address as we did.
         */
        if (cichar == go->num_mode_address)
        {
            /*
             * Need to break the dead-lock. She should have realised that we
             * have both chosen the same number when she gets our confreq.
             * It's up to one of us to back down and say OK you can have this
             * address and I'll wait for your NAK and take the other address
             * value. But this backing-off decision should be a random 50/50
             * chance. 2 Cisco boxes or the same box in loopback could end up
             * getting no where in deciding.
             */
            if (create_unique_number() & 0x01)
            {
                /*
                 * We've thrown the dice and decided to tell her to take the
                 * other address.
                 */
                cichar = (cichar == 3) ? 1 : 3;
                ho->num_mode_address = cichar;
                DECPTR(sizeof(uchar), p);
                orc = CONFNAK;
                PUTCHAR(ho->num_mode_address, p);
                break;
            }
        }

        ho->num_mode_address = cichar;
        ho->neg_numbered_mode = 1;
        break;

	default:
	    if (ppp_negotiation_debug)
		buginf("\nPPP %s: rcvd unknown option %d", 
		        idb->hw_namestring, citype);
	    INCPTR(cilen, p);
	    orc = CONFREJ;
	    break;
	}
	cilen += 2;			/* adjust cilen whole ci */

endswitch:
	if (ppp_negotiation_debug) {
	    if (orc == CONFACK)
		buginf(" acked");
	    if (orc == CONFREJ)
		buginf(" rejected");
	    if (orc == CONFNAK)
		buginf(" nacked");
 	}

	if (orc == CONFACK &&		/* good ci */
	    rc != CONFACK)		/*  but prior ci wasnt? */
	    continue;			/* don't send this one */

	if (orc == CONFNAK) {		/* nak this ci? */
	    if (rc == CONFREJ)		/* rejecting prior ci? */
		continue;		/* don't send this one */
	    if (rc == CONFACK) {	/* ack'd all prior cis? */
		rc = CONFNAK;		/* not anymore... */
		ucp = inp;		/* backup */
	    }
	}
	if (orc == CONFREJ &&		/* reject this ci */
	    rc != CONFREJ) {		/*  but no prior ones? */
	    rc = CONFREJ;
	    ucp = inp;			/* backup */
	}
	if (ucp != cip)			/* need to move ci? */
	    bcopy(cip, ucp, cilen);	/* move it */
	INCPTR(cilen, ucp);		/* update output pointer */
    }

      /*
       * If we are to ack this CONFREQ, save the CONFACK info for possible
       * use in VPN
       */
      if (rc == CONFACK) {
        if (f->confack_sent)
           free(f->confack_sent);
        f->confack_sent = malloc_named(*len, "VPN CONFACK Sent");
        if (f->confack_sent) {
           bcopy(inp, f->confack_sent, *len);
           f->confack_sent_len = *len;
        }
      }
 
    /*
     * xxx if we wanted to send additional naks (for unsent cis), the
     * code would go here.  this must be done with care since it might
     * require a longer packet than we received.
     */

    *len = ucp - inp;			/* compute output length */
    return(rc);			/* return final code */
}

/*
 * lcp_up
 * lcp has come up. Start ipcp, etc.
 */

static void lcp_up (fsm *f)
{
    hwidbtype *idb;
    lcp_state *lcpstate;
    lcp_options *go;
    lcp_options *wo;
    lcp_options *ho;
    int maxhdr;

    idb = f->idb;
    lcpstate = idb->lcpstate;
    go = f->gotoptions;
    wo = f->wantoptions;
    ho = f->heroptions;

    /*
     * Start LAPB, if negotiated. (Both sides must have agreed)
     */
    if (go->neg_numbered_mode && ho->neg_numbered_mode) {
	maxhdr = PPP_MAX_HDR_SIZE; /* Fixed value for now */
        reg_invoke_lapb_setup_encap(ET_PPP, idb, go->num_mode_address,
                                    go->num_mode_window, maxhdr);
    }

    lcpstate->passive = 0;
    lcpstate->restart = 1;    /* set to 1 in kernels or multi-line versions */

    lcpstate->echo_flag = 0;
    lcpstate->echo_id = 0;

    lcpstate->poorquality = 0;		/* Assume good Link Quality    */
    lcpstate->loopedback = 0;		/* Assume link not looped back */
    lcpstate->LastOutLQRs = 0;		/* No LQRs have been received  */
    lcpstate->LastOutPackets = 0; 	/* No LQRs have been received  */
    lcpstate->LastOutOctets = 0;	/* No LQRs have been received  */
    lcpstate->SaveInPackets = 0;
    lcpstate->SaveInOctets = 0;
    lcpstate->OutLQRs = 0;	        /* Init LQR xmt seq number     */
    lcpstate->InLQRs = 0;		/* Init LQR rcv seq number     */
    lcpstate->InGoodOctets = 0;		/* Init good octet count       */
    lcpstate->InDiscards = 0;		/* Init discard count          */
    lcpstate->validlqrrcvd = 0;		/* clear the valid flag        */ 
    lcpstate->cLastOutLQRs = 0;		/* clear saved count */

    memset(lcpstate->in.history, 0xFF, LQM_HISTORY_SIZE);
    lcpstate->in.index = 0;	/* Start at beginning of history */
    lcpstate->in.successes = lcpstate->in.n; /* No failures yet */
    memset(lcpstate->out.history, 0xFF, LQM_HISTORY_SIZE);
    lcpstate->out.index = 0;	/* Start at beginning of history */
    lcpstate->out.successes = lcpstate->out.n; /* No failures yet */

    if (idb->status & IDB_ASYNC) {
	/*
	 * If we have an async control character map to set, do so now
	 */
	if (ho->neg_asyncmap)
	    async_setmap(idb, ho->asyncmap);
    } else {
	ho->neg_accompression = 0;	/* No A/C compression */
	ho->neg_pcompression = 0;	/* No protocol field compression */
    }

    /*
     * If we are supposed to do any sort of authentication, do so now
     *
     * Note that if we wanted to negotiate authentication, but failed,
     * we will do the authentication anyway. This allows us to avoid
     * getting stuck in LCP negotiation.
     *
     * Note that if any authentication was agreed to then we enter the
     * authenticating phase until both sides (if necessary) are happy.
     * No NCP packets will be exchanged until authentication is complete.
     */
    auth_init(idb);		/* start authentication fresh */

    /*
     * If this were a Virtual Access Interface projected via L2F, 
     * the remote_name field in the idb is NULL. Call VPDN to full in 
     * that field
     */
    reg_invoke_vpn_get_remote_name(idb);
    
    /*
     * Did we agree to do any authentication?
     */
    if (go->neg_auth || ho->neg_auth) {
        lcpstate->phase = AUTHENTICATING;
	auth_start(idb);
    }

    /*
     * AUTHORIZATION
     * If not authenticating, then do LCP authorization here.
     */
    if ((!go->neg_auth) && (!old_access_control) ) {
	authorstruct author;
	if (!lcpstate->ustruct) {
	    char *rem = NULL;

	    /* Create a ustruct so authorization can tell that NO
	     * authentication was done.
	     */
	    if (idb->idb_tty) {
		rem = tty_get_remote_addr(idb->idb_tty);
	    }
	    if (is_isdn(idb) && 
		(idb->isdn_info->remote_src == ISDN_SRC_ISDN)) {
		rem = idb->isdn_info->remote_number;
	    }

	    /*
	     * We need a ustruct in order to do authorization.  However, 
	     * CSCdi77152 frees the tty->ustruct and copies the 
	     * lcpstate->ustruct to tty->ustruct after PPP authentication.
	     * This causes accounting records to be generated in the following
	     * sequence:  EXEC-START/EXEC-STOP/NET-START/NET-STOP. So we need
	     * to be consistent here.  If tty->ustruct exists, we will dup
	     * it, then free it.  This generates the EXEC-STOP before
	     * the NET-START.  Then we copy our dup'd ustruct into tty->ustruct
	     * and lcpstate->ustruct.(Yes I know this is ugly)  If tty->ustruct
	     * doesn't exist or if aaa_acct_nested is TRUE, we follow our 
	     * original idea and call aaa_create_user().
	     */

	    if (idb->idb_tty && idb->idb_tty->ustruct && !aaa_acct_nested) {
		if (aaa_dup_user(&idb->idb_tty->ustruct, &lcpstate->ustruct, 
		    AAA_SVC_PPP, PRIV_USER)) {

		    aaa_free_user(&idb->idb_tty->ustruct);
		    aaa_lock_user(lcpstate->ustruct);
		    idb->idb_tty->ustruct = lcpstate->ustruct;
		}
	    }

	    if (lcpstate->ustruct == NULL) {
		aaa_create_user(&lcpstate->ustruct, NULL, NULL, 
				idb->hw_namestring, rem, AAA_AUTHEN_TYPE_NONE, 
				AAA_SVC_PPP, PRIV_USER);
	    }
	}

	if (!lcp_do_author(lcpstate->ustruct, &author, idb, f, NULL)) {
	    lcp_close(f); /* will clear lcpstate->ustruct */
	}
	aaa_cleanup_author(&author);
    }

    if (lcpstate->phase == ESTABLISHING) {
	if (!old_access_control) {
	    reg_invoke_aaa_acct_net(lcpstate->ustruct, NULL, idb->firstsw);
	}

	idb->ppp_lineup = TRUE;

	if (lcpstate->ustruct && !null(lcpstate->ustruct->user)) {
	    reg_invoke_dialer_remote_name(idb, lcpstate->ustruct->user);
	}

	/* check for mlp negotiated */
	if ((ho->neg_mlp_mrru && go->neg_mlp_mrru)) {
	    if (multilink_up(idb)) {
		lcpstate->phase = VIRTUALIZED;
		TIMER_STOP(lcpstate->negtime);
		return;
	    } else {
	        if (multilink_events_debug)
	           buginf("\nMLP %s: Multilink join failed",
			   idb->hw_namestring);
	        lcp_close(idb->lcpfsm);
	        return;
	    }
	}
	
	/*
	 * lcpstate->phase should not be set to UP before this point in order
	 * to avoid race conditions with the virtualizing process.  We will
	 * set the phase to UP if and only if the interface will negotiate
	 * NCPs.  Note that multilink member interfaces and forwarded
	 * interfaces do not negotiate NCPs.
	 */
	lcpstate->phase = UP;
	ncp_lowerup(idb->ncpfsmidx);
    }

}

/*
 * lcp_authcomplete
 * Authentication has completed successfully, so start up the NCP's.
 */

void lcp_authcomplete (hwidbtype *idb, usernametype *info)
{
    lcp_state *lcpstate;
    fsm *f;
    lcp_options *ho, *go, *wo, *ao;
    boolean temp_bool;
    dialerdbtype *ddb;

    auth_cleanup(idb);

    f = idb->lcpfsm;

    lcpstate = idb->lcpstate;

    /*
     * Make sure that the line didn't drop while we were waiting for a AAA
     * request to complete.  We should come here once and only in the
     * authentication phase.
     */
    if (lcpstate->phase != AUTHENTICATING) {
	return;
    }

    if (!old_access_control) {
	reg_invoke_aaa_acct_net(lcpstate->ustruct, NULL, idb->firstsw);
    }

    /*
     * If there is Callback to the caller and it is an async device, 
     * initiate callback here, if necessary.
     * reg_invoke_callback_ppp_start return codes need not be checked.
     * If return is FALSE
     *    callback is disabled, user does not have callback or registry 
     *    is not registered.
     *    PPP proceeds as if no callback were in effect. 
     * else if return is TRUE 
     *    Callback is in effect (the selected line for callback may
     *    be unavailable but it will fail in the callback process).
     *    Don't allow link to go to UP state
     */  
    ho = f->heroptions;
    go = f->gotoptions;
    ao = f->allowoptions;
    wo = f->wantoptions;
    if (ho->neg_callback && (idb->type == IDBTYPE_ASYNC) && idb->idb_tty &&
	!is_ddr(idb)) {
       if (reg_invoke_callback_ppp_start(idb->idb_tty,info,
					 ho->callback_message)) {
          /*
           * Free the callback message now that we won't be using 
           * it anymore
           */
          if (ho->callback_message) {
             free(ho->callback_message);
             ho->callback_message = 0;
	  }
          return;
       }
    } else if (ao->neg_callback || wo->neg_callback) {
      temp_bool = reg_invoke_dialer_callback_start(idb, info,
						   ( ho->neg_callback || 
						    go->neg_callback ),
						   ho->callback_message);

      if (ho->neg_callback)
	if (ho->callback_message) {
	  free(ho->callback_message);
	  ho->callback_message = 0;
	}
      if (temp_bool)
	return;
    }

    /*
     * Just in case callback_message was malloced but for whatever reason 
     * callback was not initiated, free the callback_message. 
     */
    if (ho->callback_message) {
       free(ho->callback_message);
       ho->callback_message = 0;
    }

    idb->ppp_lineup = TRUE;

    /* check for mlp negotiated */
    if ((ho->neg_mlp_mrru && go->neg_mlp_mrru)) {
	/*
	 *  If answering, multilink requires authentication or
	 *   caller id.  If not true, take link down.  Not capable
	 *   of doing multilink over this link.
	 */
	if (lcpstate->call_direction == PPP_CALL_IN) {
	    ddb = get_dialerdb(idb);
	    if (!(ppp_get_remote_name(idb) || (ddb->number_called[0]))) {
		if (multilink_events_debug)
		    buginf("\nMLP %s: Authentication or Caller ID required.",
			   idb->hw_namestring);
		lcp_close(idb->lcpfsm);
		return;
	    }
	}
	
	/*
	 * Do not negotiated NCP's if this is not the first link
	 * in a bundle or if this is first link, and 
         * a Master Bundle interface was created for it and other 
         * future links, then do not negotiate NCPs on any of the links 
	 */
	if (multilink_up(idb)) {
	    lcpstate->phase = VIRTUALIZED;
	    TIMER_STOP(lcpstate->negtime);
	    return;
	} else {
	    if (multilink_events_debug)
	       buginf("\nMLP %s: Multilink join failed",
			   idb->hw_namestring);
	    lcp_close(idb->lcpfsm);
	    return;
	}
    }
    
    /*
     * lcpstate->phase should not be set to UP before this point in order
     * to avoid race conditions with the virtualizing process.  We will
     * set the phase to UP if and only if the interface will negotiate
     * NCPs.  Note that multilink member interfaces and forwarded
     * interfaces do not negotiate NCPs.
     */
    lcpstate->phase = UP;
    ncp_lowerup(idb->ncpfsmidx);
}

/*
 * lcp_down
 * lcp has gone down.  Alert other protocols.
 */

static void lcp_down (fsm *f)
{
    hwidbtype *idb;
    lcp_state *l;

    idb = f->idb;
    l = idb->lcpstate;
    l->inlqrvalid = 0;
    ncp_lowerdown(idb->ncpfsmidx);
    idb->ppp_lineup = FALSE;
    l->phase = TERMINATING;
    multilink_down(idb);
    /* Free any pending NCP packet that are buffered on this interface */
    mlp_process_pending_ncps(idb, FALSE);
    auth_cleanup(idb);
    /*
     * If lcp_closed() does not get called due to a race condition that
     * brings the line protocol down (i.e. ppp_deencap), we need to
     * make sure and close reliable link so we can return to normal
     * PPP operation for future calls.
     */
    if (idb->ppp_lapb)
        reg_invoke_lapb_remove_encap(ET_PPP, idb);
}

/*
 * lcp_closed
 * lcp has closed.  Alert other protocols.
 */

static void lcp_closed (fsm *f)
{
    lcp_state *l;
    hwidbtype *idb;
    lcp_options *ho;
    dialerdbtype *ddb;

    idb = f->idb;
    l = idb->lcpstate;
    l->phase = DOWN;
    idb->ppp_lineup = FALSE;
    auth_cleanup(idb);
    reg_invoke_lcp_closed_cleanup(idb);
    multilink_down(idb);
    /* Free any pending NCP packet that are buffered on this interface */
    mlp_process_pending_ncps(idb, FALSE);
    GET_TIMESTAMP(l->link_transition);
    /* reset the hardware to disconnect if a dialup link */
    (*(idb->reset))(idb);
    if (!old_access_control) {
	aaa_free_user(&l->ustruct);
    }
    if (f && f->idb && f->idb->lcpfsm) {
	ho = f->idb->lcpfsm->heroptions;
	if (ho && ho->callback_message) {
	    free(ho->callback_message);
	    ho->callback_message = NULL;
	}
        /*
         * Free up the CONFACK sent and received buffers 
         */
        if (f->confack_rcv) {
            free(f->confack_rcv);
            f->confack_rcv = NULL;
	}
        if (f->confack_sent) {
            free(f->confack_sent);
            f->confack_sent = NULL;
	}
    }

    /*
     * It's possible that we got here because we tried and tried to bring
     * up LCP, but we couldn't.  So, if we are a dialer, and we are still
     * spoofing, we had better turn off spoofing so that we start to
     * send unreachables, etc.  Best to reflect reality finally after PPP
     * has given up trying to bring line protocol up.
     */
    ddb = get_dialerdb(idb);
    if ((ddb != NULL) && IS_SPOOFING_PROT(ddb)) {
	ddb->dialer_spoofing_prot = FALSE;
    }

    /*
     * If we want to attempt a restart, make sure the lower layer (physical
     * layer) is up.  If not then don't bother with the open.  However,
     * it is very likely that LOWERUP has not been cleared by ppp_cstate()
     * yet, so we need to be avoid doing an immediate lcp_activeopen().
     * Instead we will set up for a delayed active open and wait one full
     * restart time.  If the peer starts negotiating before we timeout and
     * start, that is fine as we will immediately begin negotiating.  With
     * any luck, any pending disconnect will occur before the timeout and
     * we will not send an unwelcome Config Request.  CSCdi59175
     */
    if (l->restart && (f->flags & LOWERUP)) {
	/* Note we will *always* try to restart the protocol */
	/* This will avoid some CI-CI timing problems */
	lcp_passiveopen(f);	/* start protocol in passive mode */
	if (!l->passive) {
	    /*
	     * Here we change the standard script.  We want an immediate
	     * Active Open if the state logic sees the interface come up.
	     * And we want to fire in 2-3 seconds instead of 30-45 seconds
	     * as lcp_passiveopen() set up.
	     */
	    f->flags |= AOPENDING;	/* set Active Open pending flag */
	    TIMER_START(f->timeouttime, f->restarttime);
	}
	/*
	 * Not sure if this is right, but this should ensure that a
	 * connection does not just bounce once.
	 */
	if (f->idb->lcpstate->neg_timeout) {
	    TIMER_START(l->negtime, l->neg_timeout);
	}
    }

    /*
     * If we used LAPB to provide a reliable link, we need to close it down and
     * return to normal PPP operative mode for future calls, since numbered
     * mode will need to be renegotiated.
     */
    if (idb->ppp_lapb)
        reg_invoke_lapb_remove_encap(ET_PPP, idb);
}

/*
 * ppp_copy_fields
 * copy ppp fields from one idb to another.  Used to propagate info from
 * dialer hunt groups to interfaces.
 */
static void copy_ppp_fields (hwidbtype *idb1, hwidbtype *idb2)
{
    fsm *f1, *f2;
    lcp_options *lcp_wo1;
    lcp_options *lcp_wo2;
    lcp_options *lcp_ao1;
    lcp_options *lcp_ao2;
    lcp_options *lcp_ho1;
    lcp_options *lcp_ho2;
    ccp_options *ccp_wo1;
    ccp_options *ccp_wo2;
    boolean auth_changed;
    lcp_state *lcpstate1;
    lcp_state *lcpstate2;

    auth_changed = FALSE;

    f1 = idb1->lcpfsm;
    f2 = idb2->lcpfsm;

    if ((f1 == NULL) || (f2 == NULL))
        return;

    lcp_wo1 = f1->wantoptions;
    lcp_ao1 = f1->allowoptions;
    lcp_ho1 = f1->heroptions;
    lcp_wo2 = f2->wantoptions;
    lcp_ao2 = f2->allowoptions;
    lcp_ho2 = f2->heroptions;

    lcpstate1 = idb1->lcpstate;
    lcpstate2 = idb2->lcpstate;

    if ((lcpstate1 == NULL) || (lcpstate2 == NULL))
        return;

    if (lcp_wo2->neg_auth != lcp_wo1->neg_auth) {
	auth_changed = TRUE;
	lcp_wo2->neg_auth = lcp_wo1->neg_auth;
    }

    if (bcmp(lcp_wo1->authtypes, lcp_wo2->authtypes, PPP_AUTH_NUM)) {
	auth_changed = TRUE;
	bcopy(lcp_wo1->authtypes, lcp_wo2->authtypes, PPP_AUTH_NUM);
    }

    if (bcmp(lcpstate1->acc_list_name, lcpstate2->acc_list_name,
	     sizeof(lcpstate1->acc_list_name))) {
	auth_changed = TRUE;
	bcopy(lcpstate1->acc_list_name, lcpstate2->acc_list_name,
	      sizeof(lcpstate1->acc_list_name));
    }

    if (strcmp(lcpstate1->chap_name, lcpstate2->chap_name)) {
	auth_changed = TRUE;
	setstring_named(&lcpstate2->chap_name, lcpstate1->chap_name,
			"PPP chap 1");
    }

    if (strcmp(lcpstate1->chap_pw, lcpstate2->chap_pw)) {
	auth_changed = TRUE;
	setstring_named(&lcpstate2->chap_pw, lcpstate1->chap_pw,
			"PPP chap 2");
    }

    if (strcmp(lcpstate1->pap_sent_name, lcpstate2->pap_sent_name)) {
	auth_changed = TRUE;
	setstring_named(&lcpstate2->pap_sent_name, lcpstate1->pap_sent_name,
			"PPP pap 1");
    }

    if (strcmp(lcpstate1->pap_sent_pw, lcpstate2->pap_sent_pw)) {
	auth_changed = TRUE;
	setstring_named(&lcpstate2->pap_sent_pw, lcpstate1->pap_sent_pw,
			"PPP pap 2");
    }

    if (lcpstate1->cfg_direction != lcpstate2->cfg_direction) {
	auth_changed = TRUE;
	lcpstate2->cfg_direction = lcpstate1->cfg_direction;
    }
    lcpstate2->call_direction = lcpstate1->call_direction;

    if (lcpstate1->chap_refuse != lcpstate2->chap_refuse) {
	auth_changed = TRUE;
	lcpstate2->chap_refuse = lcpstate1->chap_refuse;
    }

    if (lcpstate1->chap_nowait != lcpstate2->chap_nowait) {
	auth_changed = TRUE;
	lcpstate2->chap_nowait = lcpstate1->chap_nowait;
    }

    if (lcpstate1->chap_noignoreus != lcpstate2->chap_noignoreus) {
	auth_changed = TRUE;
	lcpstate2->chap_noignoreus = lcpstate1->chap_noignoreus;
    }

#ifdef NOTDONE
    if (lcpstate1->chap_auto_secrets != lcpstate2->chap_auto_secrets) {
	auth_changed = TRUE;
	lcpstate2->chap_auto_secrets = lcpstate1->chap_auto_secrets;
    }
#endif

    if (lcpstate1->chap_split_names != lcpstate2->chap_split_names) {
	auth_changed = TRUE;
	lcpstate2->chap_split_names = lcpstate1->chap_split_names;
    }

    lcp_wo2->mru = lcp_wo1->mru;
    lcp_wo2->neg_mru = lcp_wo1->neg_mru;
    
    lcp_ao2->neg_callback = lcp_ao1->neg_callback;
    lcp_ho2->neg_callback = lcp_ho1->neg_callback;  
    lcp_wo2->neg_callback = lcp_wo1->neg_callback;  
    lcp_wo2->neg_numbered_mode = lcp_wo1->neg_numbered_mode;

    lcp_wo2->neg_mlp_mrru = lcp_wo1->neg_mlp_mrru;
    lcp_wo2->mlp_mrru = lcp_wo1->mlp_mrru;
    lcp_ao2->neg_mlp_mrru = lcp_ao1->neg_mlp_mrru;
    lcp_ao2->mlp_mrru = lcp_ao1->mlp_mrru;
    
    lcp_wo2->neg_endpoint = lcp_wo1->neg_endpoint;
    lcp_ao2->neg_endpoint = lcp_ao1->neg_endpoint;
 
    if (f2->localauthen != f1->localauthen) {
	auth_changed = TRUE;
	f2->localauthen = f1->localauthen;
    }
    f2->restarttime = f1->restarttime;
    f2->maxconfigure = f1->maxconfigure;
    f2->maxtermtransmits = f1->maxtermtransmits;
    f2->maxfailure = f1->maxfailure;
    f2->max_bad_auths = f1->max_bad_auths;
    f2->maxnakloops = f1->maxnakloops;
    lcpstate2->neg_timeout = lcpstate1->neg_timeout;
    if (f2->negotiate_callin_only != f1->negotiate_callin_only) {
	auth_changed = TRUE;
	f2->negotiate_callin_only = f1->negotiate_callin_only;
    }
    copy_ncp_fields(idb2);

    lcp_ao2 = f2->allowoptions;
    lcp_ao2->neg_numbered_mode = lcp_wo1->neg_numbered_mode;

    /*
     * Also need to copy options for CCP
     */
    f1 = ppp_serial2fsm(TYPE_PPP_CCP, idb1);
    f2 = ppp_serial2fsm(TYPE_PPP_CCP, idb2);
    if (f1 && f2) {
	ccp_wo1 = f1->wantoptions;
	ccp_wo2 = f2->wantoptions;
	ccp_wo2->negotiate_compression = ccp_wo1->negotiate_compression;
	ccp_wo2->user_preference = ccp_wo1->user_preference;
    }

    /*
     * deal with compression
     * If the primary has compression enabled, allocate the space on
     * on the secondary if it doesn't exist.  If the primary doesn't
     * have it, then if the secondary does, free it.
     */
    idb2->ppp_bridge_ip = idb1->ppp_bridge_ip;
    idb2->ppp_bridge_ipx = idb1->ppp_bridge_ipx;
    idb2->ppp_bridge_atalk = idb1->ppp_bridge_atalk;
    idb2->ppp_bridge_ipx_enctype = idb1->ppp_bridge_ipx_enctype;
    if (idb1->comp_db) {
	if (!idb2->comp_db){
	    idb2->comp_db = malloc_named(sizeof(struct compress_db_),
					 "PPP Comp DB");
	    reg_invoke_compress_clearcounters(idb2);
	}
	if (idb2->comp_db)
	    idb2->comp_db->comp_configed = idb1->comp_db->comp_configed;
    } else {
	if (idb2->comp_db) {
	    free(idb2->comp_db);
	    idb2->comp_db = NULL;
	}
    }
    /*
     * if authentication was changed significantly, restart LCP negotiation
     */
    if (auth_changed && (idb2->lcpfsm->state != CLOSED)) {
	lcpstate2->restart = 1;
	lcp_close(idb2->lcpfsm);
    }
}

/*
 * ppp_command
 * handle ppp quality % command and other sub commands
 */
void ppp_command (parseinfo *csb)
{
    hwidbtype *idb;
    lcp_state *lcpstate;
    fsm *f;
    lcp_options *wo, *go, *ao, *ho;
    int old_value;
    uint encrypt_type;
    char password[PASSWORDLEN_ENCRYPTED+1];
    char *pw;

    idb = csb->interface->hwptr;
    if (reg_invoke_dialer_huntgroup_member(idb)) {
	if (!csb->nvgen) {
	    printf("\nCannot change ppp values of hunt group member");
	}
	return;
    }

    /* On an async interface, the configured encapsulation may be
     * different from current encapsulation.  (user slip command
     * changes encapsulation to slip).  We still need to nvgen the
     * configured commands, regardless of user affected current state.
     */
    if ((idb->enctype != ET_PPP) &&
	!((idb->type == IDBTYPE_ASYNC) && (idb->enctype_cfg == ET_PPP))) {
	if (!csb->nvgen) {
	    printf("\nMust set encapsulation to PPP "
		   "before using PPP subcommands");
	}
	return;
    }

    lcpstate = idb->lcpstate;
    if (csb->nvgen) {
	f = idb->lcpfsm;
	wo = f->wantoptions;
	go = f->gotoptions;
	ao = f->allowoptions;
        ho = f->heroptions;

	switch (csb->which) {
	case PPP_LINK_QUALITY:
	    nv_write(lcpstate->ppp_quality_threshold != 0,
		     "%s %d", csb->nv_command,
		     lcpstate->ppp_quality_threshold);
	    break;
	    
	case PPP_MAX_TERMINATE:
	    nv_write(f->maxtermtransmits != DEFMAXTERMTRANSMITS,
		     "%s %d", csb->nv_command, f->maxtermtransmits);
	    break;
	    
	case PPP_MAX_CONFIGURE:
	    nv_write(f->maxconfigure != DEFMAXCONFIGURE,
		     "%s %d", csb->nv_command, f->maxconfigure);
	    break;
	    
	case PPP_MAX_FAILURE:
	    nv_write(f->maxfailure != DEFMAXFAILURE,
		     "%s %d", csb->nv_command, f->maxfailure);
	    break;
	    
	case PPP_RESTART_TIMER:
	    nv_write(f->restarttime != DEFTIMEOUT,
		     "%s %d", csb->nv_command, f->restarttime / ONESEC);
	    break;
	    
	case PPP_AUTHENTICATION:
	    nv_write(wo->neg_auth, csb->nv_command);
	    if (wo->neg_auth) {
		int i;
		for (i = 0; (i < PPP_AUTH_NUM) && wo->authtypes[i]; i++) {
		    nv_add((wo->authtypes[i] == PPP_AUTH_CHAP), " chap");
		    nv_add((wo->authtypes[i] == PPP_AUTH_PAP), " pap");
		}
		nv_add((f->negotiate_callin_only), " callin");
		if (old_access_control) {
		  nv_add((f->localauthen & PPP_LAUTHEN_IFNEED), " if-needed");
		} else {
		  nv_add(lcpstate->acc_list_name[0], " %s",
			 lcpstate->acc_list_name);
		  nv_add(f->localauthen & PPP_LAUTHEN_SINGLELINE, " one-time");
		}
	    }
	    break;
	    
	case PPP_USE_TACACS:
	    /* 
	     * If single-line is set in new-model it can be TRUE here
	     * even in PPP_LAUTHEN_TACACS is false.
	     */
	    if (f->localauthen & PPP_LAUTHEN_TACACS) {
		nv_write(TRUE, csb->nv_command);
		nv_add(f->localauthen & PPP_LAUTHEN_SINGLELINE, " single-line");
	    }
	    break;

	case PPP_CALLBACK:
            if (ao->neg_callback) {
               if (ao->callback == CALLBACK_PPP_NONEG) {
                  nv_write(TRUE, csb->nv_command);
                  nv_add(TRUE, " initiate");
               } else {
                  nv_write(TRUE, csb->nv_command);
                  nv_add(TRUE, " accept");
               }             
            } else if (wo->neg_callback) {
               nv_write(TRUE, csb->nv_command);
               nv_add(TRUE, " request");
            }
            break;

	case PPP_MULTILINK:
	    nv_write(ao->neg_mlp_mrru || wo->neg_mlp_mrru, csb->nv_command);
	    break;

	case PPP_RELIABLE_LINK:
	    nv_write(wo->neg_numbered_mode, csb->nv_command);
	    break;

	case PPP_MAX_BAD_AUTHS:
	    nv_write(f->max_bad_auths != DEFMAXBADAUTHS,
		     "%s %d", csb->nv_command, f->max_bad_auths);
	    break;
	    
	case PPP_CHAP_NAME:
	    nv_write(lcpstate->chap_name != NULL,
		     "%s %s", csb->nv_command, lcpstate->chap_name);
	    break;

	case PPP_PASSWORD:
	    nv_write(lcpstate->chap_pw != NULL,
		     "%s %d %s", csb->nv_command, ENCRYPT_TEMP,
		     lcpstate->chap_pw);
	    break;

	case PPP_CALL_DIRECTION:
	    if (lcpstate->cfg_direction) {
		nv_write(TRUE, csb->nv_command);
		nv_add(lcpstate->cfg_direction == PPP_CALL_IN, " callin");
		nv_add(lcpstate->cfg_direction == PPP_CALL_OUT, " callout");
		nv_add(lcpstate->cfg_direction == PPP_NO_CALL, " dedicated");
	    }
	    break;

	case PPP_REFUSE_CHAP:
	    if (lcpstate->chap_refuse) {
		nv_write(TRUE, csb->nv_command);
		nv_add(lcpstate->chap_refuse == PPP_REFUSE_CHAP_CALLIN,
		       " callin");
	    }
	    break;

	case PPP_CHAP_WAIT:
	    nv_write(lcpstate->chap_nowait, "no %s", csb->nv_command);
	    break;

	case PPP_CHAP_IGNOREUS:
	    nv_write(lcpstate->chap_noignoreus, "no %s", csb->nv_command);
	    break;

#ifdef NOTDONE
	case PPP_CHAP_DUAL_SECRETS:
	case PPP_CHAP_QUAD_SECRETS:
	    nv_write(lcpstate->chap_auto_secrets == csb->which,
		     csb->nv_command);
	    break;
#endif

	case PPP_CHAP_SPLIT_NAMES:
	    nv_write(lcpstate->chap_split_names, csb->nv_command);
	    break;

	case PPP_PAP_SENT_NAME:
	    nv_write(lcpstate->pap_sent_name != NULL,
		     "%s %s password %d %s", csb->nv_command,
		     lcpstate->pap_sent_name, ENCRYPT_TEMP,
		     lcpstate->pap_sent_pw);
	    break;

	case PPP_BRIDGE:
	    nv_write(idb->ppp_bridge_ip, "ppp bridge ip");
	    nv_write(idb->ppp_bridge_ipx, "ppp bridge ipx");
	    nv_add(idb->ppp_bridge_ipx &&
		   idb->ppp_bridge_ipx_enctype != ET_NOVELL_ETHER,
		   " %s", encstring(idb->ppp_bridge_ipx_enctype));
	    nv_write(idb->ppp_bridge_atalk, "ppp bridge appletalk");
	    break;

	case PPP_NEG_TIMEOUT:
	    nv_write(lcpstate->neg_timeout != 0,
		     "%s %d", csb->nv_command, lcpstate->neg_timeout / ONESEC);
	    break;

	default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
	}
	return;
    }

    f = idb->lcpfsm;

    wo = f->wantoptions;
    go = f->gotoptions;
    ao = f->allowoptions;
    ho = f->heroptions;
    
    switch (csb->which) {
    case PPP_LINK_QUALITY:
	if (csb->sense) {
	    lcpstate->ppp_quality_threshold = GETOBJ(int,1);
	    wo->neg_quality = 1; 
	    go->qualitytype = TYPE_PPP_LQR;
	    wo->qualitytype = TYPE_PPP_LQR;
	    wo->reportingperiod = idb->keep_period * 100;
	    go->reportingperiod = wo->reportingperiod;
	    /*
	     * If the line is OPEN and we are not running LQM
             * then renegotiate for LQM as long as keep_alive is set.
	     * If nokeepalive is set then set all the flags but don't
	     * renegotiate for it. When keepalive is set we will just
	     * start sending LQRs.
	     */
	    if (f->state == OPEN && !go->neg_quality && !idb->nokeepalive) {
		go->neg_quality = 1;
		fsm_sconfreq(f);
	    }
	    go->neg_quality = 1;
	} else {
	    lcpstate->ppp_quality_threshold = 0;
	    wo->neg_quality = 0; 
	    go->neg_quality = 0; 
	    return;
	}

	/*
	 * We do K out of N checking where N is the number of periods we check,
	 * and K is the number of successes we must have had during that period
	 * K is the threshold percentage of N (e.g. 80% of N).
	 */
	
	if (lcpstate->ppp_quality_threshold) {
	    lcpstate->in.n = CHECK_PERIOD;  /* # of periods to check quality */
	    lcpstate->out.n = CHECK_PERIOD;
	    lcpstate->in.k = (lcpstate->in.n * 
			      lcpstate->ppp_quality_threshold) / 100;
	    lcpstate->out.k = (lcpstate->out.n * 
			       lcpstate->ppp_quality_threshold) / 100;
	    memset(lcpstate->in.history, 0, LQM_HISTORY_SIZE);
	    memset(lcpstate->out.history, 0, LQM_HISTORY_SIZE);
	}
	break;
	
    case PPP_MAX_TERMINATE:
	if (csb->sense) {
	    f->maxtermtransmits = GETOBJ(int,1);
	} else {
	    f->maxtermtransmits = DEFMAXTERMTRANSMITS;
	}
	copy_ncp_fields(idb);
	break;
	
    case PPP_MAX_CONFIGURE:
	if (csb->sense) {
	    f->maxconfigure = GETOBJ(int,1);
	} else {
	    f->maxconfigure = DEFMAXCONFIGURE;
	}
	copy_ncp_fields(idb);
	break;
	
    case PPP_MAX_FAILURE:
	if (csb->sense) {
	    f->maxfailure = GETOBJ(int,1);
	} else {
	    f->maxfailure = DEFMAXFAILURE;
	}
	copy_ncp_fields(idb);
	break;
	
    case PPP_RESTART_TIMER:
	if (csb->sense) {
	    f->restarttime = (GETOBJ(int,1)*ONESEC);
	} else {
	    f->restarttime = DEFTIMEOUT;
	}
	copy_ncp_fields(idb);
	break;
	
    case PPP_AUTHENTICATION:
	if (csb->sense) {
	    int i;
	    boolean restart;
	    f->negotiate_callin_only = (boolean)(GETOBJ(int, 3));
	    restart = (wo->neg_auth == 0);
	    wo->neg_auth = NUM_AUTH_INSISTS;
	    for (i = 0; i < PPP_AUTH_NUM; i++)
		 wo->authtypes[i] = csb->uiarray[i];
	    if (old_access_control) {
		f->localauthen |= GETOBJ(int,2) ? PPP_LAUTHEN_IFNEED : 0;
	    } else {
		char *list_name;
		switch (GETOBJ(int,2)) {
		case 0:
		case 1:
		    lcpstate->acc_list_name[0] = '\0';
		    list_name = "default";
		    break;
		case 2:
		    list_name = GETOBJ(string,1);
		    sstrncpy(lcpstate->acc_list_name, list_name,
			     sizeof(lcpstate->acc_list_name));
		    /* 
		     * We should add a check here to make sure the list_name
		     * is not the same as a AAA method.
		     */
		    break;
		default:
		    buginf("\n%% PPP: AAA: bad value in authentication list.");
		    list_name = NULL;
		    break;
		}
		if (GETOBJ(int,4)) {
		    f->localauthen |= PPP_LAUTHEN_SINGLELINE;
		}
		if ((csb->resolvemethod == RES_MANUAL) && list_name &&
		    !aaa_is_list_defined(list_name, AAA_SVC_PPP)) {
		    printf("AAA: Warning authentication list \"%s\" is not "
			   "defined for PPP.\n", list_name);
		}
	    }
	    /* Restart LCP negotiation if LCP is not CLOSED and if PPP auth
	     * was disabled before and is being enabled now (which is how we
	     * got here).
	     */
	    if (restart && (f->state != CLOSED)) {
		lcpstate->restart = 1;
		lcp_close(f);
	    }
	} else {
	    wo->neg_auth = 0;
	    bzero(wo->authtypes, PPP_AUTH_NUM);
	    f->negotiate_callin_only = FALSE;
	    f->localauthen &= ~PPP_LAUTHEN_IFNEED;
	    f->localauthen &= ~PPP_LAUTHEN_SINGLELINE;
	    lcpstate->acc_list_name[0] = '\0';
	}
	break;
	
    case PPP_USE_TACACS:
	if (csb->sense) {
	    f->localauthen |= PPP_LAUTHEN_TACACS;
	    if (GETOBJ(int,1)) {
		f->localauthen |= PPP_LAUTHEN_SINGLELINE;
		if ((csb->resolvemethod == RES_MANUAL) &&
		    !auth_method_supported(f, PPP_AUTH_CHAP)) {
		    printf("NOTE: \"single-line\" has no effect unless CHAP "
			   "authentication is specified.\n");
		}
	    } else {
		f->localauthen &= ~PPP_LAUTHEN_SINGLELINE;
	    }
	} else {
	    f->localauthen &= ~(PPP_LAUTHEN_TACACS | PPP_LAUTHEN_SINGLELINE);
	}
        break;

    case PPP_MULTILINK:
	old_value = wo->neg_mlp_mrru;

	if (csb->sense) {
	  ao->neg_mlp_mrru = 1;
	  wo->neg_mlp_mrru = 1;
	  wo->neg_endpoint = 1;
	  ao->neg_endpoint = 1;
	  delete_fair_queue(idb);	/* until fair queuing is supported */
	} else {
	  ao->neg_mlp_mrru = 0;
	  wo->neg_mlp_mrru = 0;
	  wo->neg_endpoint = 0;
	  ao->neg_endpoint = 0;
	}

	if (old_value != wo->neg_mlp_mrru) {
	    /*
	     * Restart only if the config changed.
	     */
	    if (!is_ddr(idb) && !is_async(idb)) {
		/*
		 *  If the link is leased/direct, close lcp and restart to
		 *    allow multilink to renegotiate.
		 */
		lcpstate->restart = 1;
		lcp_close(f);
	    }
	}
	
	break;

    case PPP_CALLBACK:
        /*
         * Set up callback to be allowed, requested or initiated
         */
 	if (csb->sense) {
 	    switch (GETOBJ(int,1)) {
 	      case PPP_CALLBACK_ACCEPT:
                 /*
                  * Accept a callback request. Callback is initiated
                  * if the remote end requests a callback and the user
                  * is designated to callback.
                  */
 		 ao->neg_callback = 1;
                 wo->neg_callback = 0;
                 ho->callback = CALLBACK_PPP_NOLOCATION;
                 ao->callback = CALLBACK_PPP_NOLOCATION;
 		 break;
 
 	       case PPP_CALLBACK_INITIATE:
                 /*
                  * Superset of PPP_CALLBACK_ACCEPT.
                  * May initiate callback without actual negotiation.
                  * If remote end actually negotiates callback, we will
                  * still honor it. However, this provides callback for 
                  * all those PPP Clients out there which do not have the 
                  * ability to negotiate callback, the user is still 
                  * designated to callback and we should callback anyway.
                  */
                  ho->neg_callback = 1;
                  ao->neg_callback = 1;
                  ao->callback = CALLBACK_PPP_NONEG;
                  wo->neg_callback = 0;
                  break;

 	      case PPP_CALLBACK_REQUEST:
                 /*
                  * Request a callback.
                  * We support only this callback option 
                  */
                 wo->neg_callback = 1;
                 wo->callback = CALLBACK_PPP_NOLOCATION;   
                 ao->neg_callback = 0;
                 ao->callback = CALLBACK_PPP_NOLOCATION;
                 break;
	     }
 	} else {
             ao->neg_callback = 0;
             wo->neg_callback = 0;
             ho->neg_callback = 0;
        }
        break;
 
    case PPP_RELIABLE_LINK:
        if (csb->sense) {
	    if (!reg_used_lapb_setup_encap(ET_PPP)) {
		printf("\n%% LAPB is unavailable on this system, "
		       "configuration remains unchanged");
	    } else if (wo->neg_numbered_mode == FALSE) {
		/*
		 * Don't renogotiate numbered mode if we are already
		 * in numbered mode
		 */
		 wo->neg_numbered_mode = TRUE;
	        /*
	         * If LCP is already in the OPEN state or has started
		 * negotiating then renegotiate for reliable link.
	         */
	        if (f->state == OPEN || f->state == ACKRCVD ||
		    f->state == ACKSENT || f->state == REQSENT) {
		    go->neg_numbered_mode = TRUE;
		    ao->neg_numbered_mode = TRUE;
		    /*
		     * Address and Control field compression is not allowed
		     * when numbered mode is being negotiated.
		     */
		    ao->neg_accompression = FALSE;
		    go->neg_accompression = FALSE;
		    fsm_sconfreq(f);
		    f->state = REQSENT;
	        }
	    }
        } else if (wo->neg_numbered_mode) {
	    /*
	     * Revert to non-LAPB operation
	     */
	    wo->neg_numbered_mode = FALSE;
	    lcpstate->restart = 1;
	    lcp_close(f);
	} 
        break;

    case PPP_MAX_BAD_AUTHS:
	if ((!csb->sense) || !GETOBJ(int,1)) {
	    GETOBJ(int,1) = DEFMAXBADAUTHS;
	}
	f->max_bad_auths = GETOBJ(int,1);
	break;

    case PPP_CHAP_NAME:
	/*
	 * Free old PPP CHAP hostname.  All done, if we're unsetting it
	 */
	if (lcpstate->chap_name) {
	    free(lcpstate->chap_name);
	    lcpstate->chap_name = NULL;
	}
	if (!csb->sense) {
	    break;
	}
	/*
	 * strdup() the new one.
	 */
	lcpstate->chap_name = strdup_named(GETOBJ(string,1), "PPP chapname");
	break;

    case PPP_CALL_DIRECTION:
	lcpstate->cfg_direction = csb->sense ? GETOBJ(int,1) : 0;
	break;

    case PPP_REFUSE_CHAP:
    case PPP_REFUSE_CHAP_CALLIN:
	lcpstate->chap_refuse = csb->sense ? csb->which : 0;
	break;

    case PPP_CHAP_WAIT:
	lcpstate->chap_nowait = !csb->sense;
	break;

    case PPP_CHAP_IGNOREUS:
	lcpstate->chap_noignoreus = !csb->sense;
	break;

#ifdef NOTDONE
    case PPP_CHAP_DUAL_SECRETS:
    case PPP_CHAP_QUAD_SECRETS:
	lcpstate->chap_auto_secrets = csb->sense ? csb->which : 0;
	break;
#endif

    case PPP_CHAP_SPLIT_NAMES:
	lcpstate->chap_split_names = csb->sense;
	break;

    case PPP_PASSWORD:
	/*
	 * Verify that we know how to decrypt this password before proceeding
	 */
	pw = GETOBJ(string,1);
	encrypt_type = GETOBJ(int,1);
	if (encrypt_type && (encrypt_type != 7)) {
		printf("\nEncryption type %d not supported.  Use 7 or 0.",
		       encrypt_type);
		return;
	}
	/*
	 * Free old PPP password.  All done, if we're unsetting it
	 */
	if (lcpstate->chap_pw) {
	    free(lcpstate->chap_pw);
	    lcpstate->chap_pw = NULL;
	}
	if (!csb->sense) {
	    break;
	}
	/*
	 * Verify the length of the password and truncate if necessary
	 */
	if (!encrypt_type && strlen(pw) > PASSWORDLEN) {
	    printf("\n%% Overly long password truncated after %d characters",
		   PASSWORDLEN);
	    pw[PASSWORDLEN] = 0;
	}
	/*
	 * The following code always encrypts the password rather than
	 * checking the flag 'password_encryption_enable'.  Thus it will be
	 * inconsistent with the documentation, but consistent with the
	 * implementation of the global command 'username xx password yy'.
	 * This inconsistency should be resolved eventually.
	 */
	if (encrypt_type) {
	    lcpstate->chap_pw = strdup_named(pw, "PPP chappw 1");
	} else {
	    encrypt(pw, password, NULL, PASSWORDLEN_ENCRYPTED);
	    lcpstate->chap_pw = strdup_named(password, "PPP chappw 2");
	}
	break;

    case PPP_PAP_SENT_NAME:
	/*
	 * Verify that we know how to decrypt this password before proceeding
	 */
	pw = GETOBJ(string,2);
	encrypt_type = GETOBJ(int,1);
	if (encrypt_type && (encrypt_type != ENCRYPT_TEMP)) {
		printf("\nEncryption type %d not supported.  Use 0 or %d.",
		       encrypt_type, ENCRYPT_TEMP);
		return;
	}
	/*
	 * Free old PPP PAP sent name and password.  All done, if we're
	 * unsetting it
	 */
	if (lcpstate->pap_sent_name) {
	    free(lcpstate->pap_sent_name);
	    lcpstate->pap_sent_name = NULL;
	}
	if (lcpstate->pap_sent_pw) {
	    free(lcpstate->pap_sent_pw);
	    lcpstate->pap_sent_pw = NULL;
	}
	if (!csb->sense) {
	    break;
	}
	/*
	 * Verify the length of the password and truncate if necessary
	 */
	if (!encrypt_type && strlen(pw) > PASSWORDLEN) {
	    printf("\n%% Overly long password truncated after %d characters",
		   PASSWORDLEN);
	    pw[PASSWORDLEN] = 0;
	}
	/*
	 * The following code always encrypts the password rather than
	 * checking the flag 'password_encryption_enable'.  Thus it will be
	 * inconsistent with the documentation, but consistent with the
	 * implementation of the global command 'username xx password yy'.
	 * This inconsistency should be resolved eventually.
	 */
	if (encrypt_type) {
	    lcpstate->pap_sent_pw = strdup_named(pw, "PPP pappw 1");
	} else {
	    encrypt(pw, password, NULL, PASSWORDLEN_ENCRYPTED);
	    lcpstate->pap_sent_pw = strdup_named(password, "PPP pappw 2");
	}
	if (!lcpstate->pap_sent_pw)
	    return;		/* make sure both strings are clear */
	lcpstate->pap_sent_name = strdup_named(GETOBJ(string,1),
					       "PPP PAP sent");
	/*
	 * get the secret for the given name.
	 */
	if (find_passwd(lcpstate->pap_sent_name, password)) {
	    char clear[PASSWORDLEN+1];

	    decrypt(lcpstate->pap_sent_pw, clear, PASSWORDLEN);
	    if (!bcmp(clear, password, strlen(clear) + 1)) {
		printf("\nPPP: Warning:  You have chosen a username/password "
		       "combination that");
		printf("\n               is valid for CHAP.  This is a "
		       "potential security hole.");
	    }
	}
	break;

    case PPP_NEG_TIMEOUT:
	if (csb->sense) {
	    lcpstate->neg_timeout = (GETOBJ(int,1)*ONESEC);
	} else {
	    lcpstate->neg_timeout = 0;
	}
	break;

    case PPP_BRIDGE:
	switch (GETOBJ(int, 1)) {
	case LINK_IP:
	    idb->ppp_bridge_ip = csb->sense;
	    if (idb->ppp_bridge_ip) {
		idb->firstsw->arp_arpa = TRUE;
		idb->firstsw->arp_arpa_def = TRUE;
		idb->firstsw->arp_timeout = ARP_MAXAGE;
	    } else {
		idb->firstsw->arp_arpa = FALSE;
		idb->firstsw->arp_arpa_def = FALSE;
		idb->firstsw->arp_timeout = ARP_MAXAGE;
	    }
	    break;
	case LINK_NOVELL:
	    idb->ppp_bridge_ipx = csb->sense;
	    idb->ppp_bridge_ipx_enctype = GETOBJ(int,2);
	    break;
	case LINK_APPLETALK:
	    idb->ppp_bridge_atalk = csb->sense;
	    reg_invoke_reset_if(idb->firstsw);
	    break;
	}
	break;

    default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }

    reg_invoke_interface_cfg_modified(idb);
}

/*
 * ppp_ncp_command
 * handle ppp ncp related commands and other sub commands
 * The function above this one "ppp_command", is biased heavily
 * towards lcp's. It's easier and less messy to separate the ncp's
 * configuration parser and do it here instead.
 */
void ppp_ncp_command (parseinfo *csb)
{
    hwidbtype *idb;

    idb = csb->interface->hwptr;

    if (reg_invoke_dialer_huntgroup_member(idb))
    {
        if (!csb->nvgen)
            printf("\nCannot change ppp values of hunt group member");
        return;
    }

    if (idb->enctype != ET_PPP)
    {
        if (!csb->nvgen)
            printf("\nMust set encapsulation to PPP before using PPP subcommands");
        return;
    }

    if (csb->nvgen)
    {
        switch (csb->which)
        {
	  case PPP_COMPRESSION:
                /*
                 * Config no longer appears as "ppp compression <stac/pred>",
                 * but instead appears as the traditional
                 * "compression <stac/pred>"
                 */
	    break;
        
	  default:
	    bad_parser_subcommand(csb, csb->which);
	    break;
        }
        return;
    }

    switch (csb->which)
    {
        case PPP_COMPRESSION:
        {
            int type = COMP_STAC;

            if (csb->sense) {
                switch (GETOBJ(int,1)){
                    case PPP_COMP_PREDICTOR:
                        type = COMP_RAND;
                    break;
                    case PPP_COMP_STACKER:
                        type = COMP_STAC;
                    break;
                    default:
                        printf("\n%%Unsupported compression type");
                        return;
                    break;
                }
            }
            reg_invoke_ppp_compression_config(idb, csb->sense, type);
        }        
        break;

        default:
            bad_parser_subcommand(csb, csb->which);
        break;
    }
}

/*
 * copy_ncp_fields
 * copy lcp timer and retry values to all the ncps. 
 */
static void copy_ncp_fields (hwidbtype *idb)
{
    fsm *lf, *f;
    int ncpidx;

    lf = idb->lcpfsm;

    FOR_ALL_FSMS(idb->ncpfsmidx, f, ncpidx) {
	f->restarttime = lf->restarttime;
	f->maxconfigure = lf->maxconfigure;
	f->maxtermtransmits = lf->maxtermtransmits;
	f->maxfailure = lf->maxfailure;
	f->maxnakloops = lf->maxnakloops;
    }
}


/*
 * ppp_interface - display information specific to ppp for the 
 * 'show interface' command.
 */
static void ppp_interface (hwidbtype *idb)
{
    fsm *lcpfsm;
    int mlp_state;

    mlp_state = PPP_UNKNOWN;

    if (reg_invoke_dialer_huntgroup_leader(idb))
      return;

    if (is_isdn(idb) && idb->isdn_info->chantype == ISDN_Dchan)
      return;

    lcpfsm = idb->lcpfsm;
    if (!lcpfsm)
        return;

    printf("\n  LCP %s", ppp_linkstates[lcpfsm->state]);

    if (is_multilink(idb)) {
	mlp_state = OPEN;
    } else if (is_mlp_enabled(idb)) {
	mlp_state = CLOSED;
    }

    if (mlp_state != PPP_UNKNOWN) {
	printf (", multilink %s", ppp_linkstates[mlp_state]);
    }

    if (idb->lcpstate->phase != VIRTUALIZED) {
	show_ncp_on_idb(idb);
    }

    if (idb->ppp_new_show_support)
        (*idb->ppp_new_show_support)(idb);
}

static void ppp_periodic (hwidbtype *idb)
{
    lcp_options *ho, *go, *wo, *ao;
    fsm *f;
    lcp_state *l;

    if (idb->firstsw->is_bundle_interface)
       return;

    f = idb->lcpfsm;
    l = idb->lcpstate;
    go = f->gotoptions;
    ho = f->heroptions;
    wo = f->wantoptions;
    ao = f->allowoptions;

    /*
     * Ignore if this link is VPN forwarding/forwarded or
     * is a Bundle Interface
     */
    if (ppp_lcpstate(idb, FORWARDED) ||
        ppp_lcpstate(idb, FORWARDING)|| 
        idb->firstsw->is_bundle_interface) {
      return;
    }
    if ((wo->neg_numbered_mode == FALSE) && idb->ppp_lapb) {
	wo->neg_numbered_mode = TRUE;
	(*(f->idb->reset))(f->idb);
    }
    /*
     * Check if any of the protocols are not configured anymore
     */
    check_ncp_status(idb);

    /*
     * If we are up and running, we may want to send a keepalive or LQR
     */
    if (f->state == OPEN) {
	if (go->neg_quality && !idb->nokeepalive) {
	    /*
	     * If the other side wants us to send LQRs
	     * and we haven't disabled keepalive, then send an LQR.
	     */
	    if (idb->keep_period > (wo->reportingperiod / 100)) {
		/*
		 * The user has changed the keep_period with the keepalive
		 * command to greater than the negotiated period.  We need to
		 * renegotiate. If we don't renegotiate, the peer will start
		 * timing out.
		 */
		wo->reportingperiod = idb->keep_period * 100;
		go->reportingperiod = wo->reportingperiod;
		fsm_sconfreq(f);
	    }
	    lcp_slqr(idb);
	} else {
	    /*
	     * else call ppp_echo() even if nokeepalive is true.  We do this
	     * because we may need to recover from the case where we missed
	     * too many keepalives and brought the NCPs down.  If we simply
	     * stopped sending keepalives then there would be no way for the
	     * link to come back up.
	     */
	    ppp_echo(idb);
	}
    }
}

#define SLQRLEN	(12)*sizeof(ulong)	/* LQR length on transmission */
#define MAX_LOST_LQRS	4		/* point at which we close down */
#define MIN_LOST_LQRS	1		/* point at which we report lost */

/*
 * lcp_slqr - Send a Link-Quality-Report.
 */
static void lcp_slqr (hwidbtype *idb)
{
    paktype *pak;
    uchar *outp;
    int outlen;
    lcp_options *go;
    lcp_options *wo;
    lcp_state *l;
    fsm *f;
    ppp_dll_hdr *dll;
    ulong lostLQRs;

    f = idb->lcpfsm;
    l = idb->lcpstate;
    go = f->gotoptions;
    wo = f->wantoptions;

    /*
     * Before we send an LQR, check to see if the other side has stopped
     * responding.  If we send too many without hearing from the other side,
     * declare everything down.
     *
     * Circular arithmetic is the bane of many programmers and is thus
     * often avoided which thus results in overly complicated checks for
     * wraps.  Of course, one could argue that we will all be retired
     * before a router lasts long enough to ship 4 billion LQRs (at one
     * per second that would be 126+ years) but lets get it right anyway.
     */

    lostLQRs = l->OutLQRs - l->cLastOutLQRs;
    if (lostLQRs > MIN_LOST_LQRS) {
	if (ppp_packet_debug || ppp_error_debug) {
	    buginf("\nPPP %s: lcp_slqr() %d lost LQRs",
		   idb->hw_namestring, lostLQRs);
	}
	if (lostLQRs > MAX_LOST_LQRS) {
	    if (f->callbacks->down)
		(*f->callbacks->down)(f); /* inform upper layers */
	    f->state = CLOSED;
	    if (f->callbacks->closed)
		(*f->callbacks->closed)(f); /* exit/restart/etc. */
	    idb->ppp_lineup = FALSE;
	    return;	/* bail out since lcp now down */
	}
    }

    outlen = SLQRLEN;
    pak = getbuffer(outlen + DLLHEADERLEN);
    if (!pak)
	return;

    pak->if_output = idb->firstsw;
    pak->linktype = LINK_PPP;
    pak->datagramsize = outlen + DLLHEADERLEN;
    pak->datagramstart = pak->network_start - DLLHEADERLEN;
    dll = (ppp_dll_hdr *)pak->datagramstart;
    dll->address = PPP_ADDRESS;
    dll->control = PPP_CONTROL;
    dll->type    = TYPE_PPP_LQR;
    pak->flags |= PAK_PRIORITY;
    pak->enctype = ET_PPP;
    outp = dll->data;

    l->OutLQRs++;	/* increment LQR send seq number */

    PPP_PUTLONG(go->neg_magicnumber ? go->magicnumber : 0L, outp);
    PPP_PUTLONG(l->LastOutLQRs, outp);	  /* copy of last PeerOutLQRs */
    PPP_PUTLONG(l->LastOutPackets, outp); /* copy of last PeerOutPackets */
    PPP_PUTLONG(l->LastOutOctets, outp);  /* copy of last PeerOutOctets */
    PPP_PUTLONG(l->InLQRs, outp);         /* number of rcvd LQRs */
    PPP_PUTLONG(l->SaveInPackets, outp);  /* number of packets received */
    PPP_PUTLONG(l->SaveInDiscards, outp); /* number of discards received */
    PPP_PUTLONG(l->SaveInErrors, outp);   /* number of errors received */
    PPP_PUTLONG(l->SaveInOctets, outp); 
					  /* number of rcv octets */
    PPP_PUTLONG(l->OutLQRs, outp);        /* number of xmit LQRs */
    PPP_PUTLONG(idb_outputs(idb) + 1, outp); /* number of xmit packets */
    PPP_PUTLONG(idb_tx_cumbytes(idb) + pak->datagramsize + 2 + 1 +
				(idb_outputs(idb) * 3), outp);  
    /* number of xmit octets including 2 bytes of crc and 1 flag for
     * this and every previous message.
     */

    /*
     * If debug is on let people know what we are sending.
     */

    if (ppp_packet_debug)
        buginf("\nPPP %s(o): lcp_slqr() state = %s magic = %#x, len = %d",
	       idb->hw_namestring, ppp_linkstates[f->state], 
	       go->magicnumber, outlen);

    if (idb->board_encap)
	if (!(*idb->board_encap)(pak,idb)) {
	    retbuffer(pak);
	    return;
	}
    datagram_out(pak);
}

/*
 * lcp_poorquality - Link Quality Monitoring has detected poor quality.
 *
 * Close Network-Layer Protocols, bring down the link, and start
 * link testing.
 */
static void lcp_poorquality (fsm *f)
{
    lcp_state *l;
    hwidbtype *idb;

    l = f->idb->lcpstate;
    idb = f->idb;

    if (l->out.successes < l->out.k && l->in.successes < l->in.k) {
	if (ppp_error_debug)
	    buginf("\nPPP %s: The serial link is down.", idb->hw_namestring);
    } else if (l->out.successes < l->out.k) {
	if (ppp_error_debug)
	    buginf("\nPPP %s: The outbound link is down.", 
		   idb->hw_namestring);
    } else if (l->in.successes < l->in.k) {
	if (ppp_error_debug)
	    buginf("\nPPP %s: The inbound link is down.", 
		   idb->hw_namestring);
    }

    l->poorquality = 1;
    /*
     * clear the history.  This way when the line comes back up, we can
     * start counting successes until we reach our desired threshold
     */
    memset(l->in.history, 0, LQM_HISTORY_SIZE);
    l->in.index = 0;	 /* Start at beginning of history */
    l->in.successes = 0; /* No successes yet */
    memset(l->out.history, 0, LQM_HISTORY_SIZE);
    l->out.index = 0;	/* Start at beginning of history */
    l->out.successes = 0; /* No successes yet */

    ncp_close(idb->ncpfsmidx, NULL);
    ncp_lowerdown(idb->ncpfsmidx);

    idb->ppp_lineup = FALSE;	/* indicate that the protocol is down */
}


/*
 * lcp_goodquality - Link Quality Monitoring has detected good quality.
 *
 * Bring up the link, (re)start Network-Layer Protocols, and halt
 * link testing.
 */
static void lcp_goodquality (fsm *f)
{
    hwidbtype *idb;

    idb = f->idb;
    if (ppp_error_debug) {
    	buginf("\nPPP %s: The serial link is up.",idb->hw_namestring);
    }
    f->idb->lcpstate->poorquality = 0;
    idb->ppp_lineup = TRUE;	/* indicate that PPP is back up */
    if (f->idb->lcpstate->phase == UP) {
	ncp_lowerup(idb->ncpfsmidx);
	ncp_activeopen(idb->ncpfsmidx, NULL);
    }
}


/*
 * lcp_loopedback - Link Quality Monitoring has detected a looped-back link.
 *
 * Close Network-Layer Protocols, bring down the link, and start
 * link testing.
 */
static void lcp_loopedback (fsm *f)
{
    lcp_state *l;
    hwidbtype *idb;

    idb = f->idb;
    if (ppp_error_debug)
    	buginf("\nPPP %s: The link appears to be looped back.", 
	       idb->hw_namestring);
    l = f->idb->lcpstate;
    l->loopedback = 1;
}

/*
 * lcp_notloopedback - Link Quality Monitoring has detected that the loopback
 * has disappeared.
 *
 * Bring up the link, (re)start Network-Layer Protocols.
 */
static void lcp_notloopedback (fsm *f)
{
    lcp_state *l;
    hwidbtype *idb;

    idb = f->idb;
    if (ppp_error_debug)
    	buginf("\nPPP %s: The link appears to be no longer looped back.",
	       idb->hw_namestring);
    l = f->idb->lcpstate;
    l->loopedback = 0;
}

/*
 * We have been sending LQR and received a protrej. Stop sending them.
 * Same code as "no ppp quality".
 */
static void lqr_protrej (hwidbtype *idb, ushort protocol)
{
    lcp_state *lcpstate;
    lcp_options *wo, *go;
    fsm *f;

    f = idb->lcpfsm;
    wo = f->wantoptions;
    go = f->gotoptions;
    lcpstate = idb->lcpstate;

    if (ppp_error_debug)
    	buginf("\nPPP %s(i): lqr_protrej() Stop sending LQRs.",
	       idb->hw_namestring);
    lcpstate->ppp_quality_threshold = 0;
    wo->neg_quality = 0;
    go->neg_quality = 0;
    wo->reportingperiod = 0;
    go->reportingperiod = 0;
}


static void lqr_init (hwidbtype *idb)
{
}

/*
 * lcp_rlqr - Receive Link-Quality-Report.
 *
 * Check Magic-Number (for loopback) and link quality.
 */

static void lcp_rlqr (paktype *inpacket, hwidbtype *idb)
{
    uchar *inp;
    ulong magic, LastOutLQRs, LastOutPackets, LastOutOctets;
    ulong PeerInLQRs, PeerInPackets, PeerInDiscards, PeerInErrors;
    ulong PeerInOctets, PeerOutLQRs, PeerOutPackets, PeerOutOctets;
    ulong peerrcvdiffp, peerxmitdiffp, myxmitdiffp, myrcvdiffp;
    ulong peerrcvdiffo, myxmitdiffo, myrcvdiffo, peerxmitdiffo;
    ulong rxpackets, rxoctets;
    lcp_state *l;
    lcp_options *go;
    lcp_options *ho;
    lcp_options *wo;
    fsm *f;
    int len, sendflag;

    sendflag = 0;
    len = inpacket->datagramsize -
	(inpacket->network_start - inpacket->datagramstart);
    inp = inpacket->network_start;
    f = idb->lcpfsm;
    l = idb->lcpstate;

    go = f->gotoptions;
    ho = f->heroptions;
    wo = f->wantoptions;

    if (len < SLQRLEN) {
	if (ppp_error_debug)
	    buginf("\nPPP %s(i): lcp_rlqr() Rcvd short LQR.",
		   idb->hw_namestring);
        goto rlqrdone; 
    }

    /*
     * We need to reply to a received LQM whether or not we
     * negotiated for it. Don't do any calculations.
     */
    
    if (!go->neg_quality && !ho->neg_quality)
	sendflag = 1;
    
    /*
     * If we have LQM enabled but nokeepalive we need to respond.
     */
    
    if (go->neg_quality && idb->nokeepalive)
	sendflag = 1;

   /*
    * Make sure we aren't talking to ourselves first
    */

    PPP_GETLONG(magic, inp);

    /*
     * If debug is on let people know what we received.
     */

    if (ppp_packet_debug)
        buginf("\nPPP %s(i): lcp_rlqr() state = %s magic = %#x, len = %d",
	       idb->hw_namestring, ppp_linkstates[f->state], magic, len);

    /*
     * Check for loopback.
     */
    if (go->neg_magicnumber) {
	if (magic == go->magicnumber) {
	    lcp_loopedback(f);
            goto rlqrdone; 
	}
    }

    if (l->loopedback)
	lcp_notloopedback(f);

    /*
     * Parse the rest of the fields.
     */

    PPP_GETLONG(LastOutLQRs, inp);
    PPP_GETLONG(LastOutPackets, inp);
    PPP_GETLONG(LastOutOctets, inp);
    PPP_GETLONG(PeerInLQRs, inp);
    PPP_GETLONG(PeerInPackets, inp);
    PPP_GETLONG(PeerInDiscards, inp);
    PPP_GETLONG(PeerInErrors, inp);
    PPP_GETLONG(PeerInOctets, inp);
    PPP_GETLONG(PeerOutLQRs, inp);
    PPP_GETLONG(PeerOutPackets, inp);
    PPP_GETLONG(PeerOutOctets, inp);

   /*
    * Calculate delta values before compares.
    * Calculated by the difference between the new fields and the old ones.
    */

    rxpackets = l->rxpackets;           /* saved in ppp_enqueue */
    rxoctets = l->rxoctets;             /* saved in ppp_enqueue */ 
    peerrcvdiffp = PeerInPackets - l->cPeerInPackets;
    peerrcvdiffo = PeerInOctets - l->cPeerInOctets;
    myxmitdiffp = LastOutPackets - l->cLastOutPackets; 
    myxmitdiffo = LastOutOctets - l->cLastOutOctets;
    peerxmitdiffp = PeerOutPackets - l->cPeerOutPackets;
    peerxmitdiffo = PeerOutOctets - l->cPeerOutOctets;
    myrcvdiffp = rxpackets - l->SaveInPackets;
    myrcvdiffo = rxoctets - l->SaveInOctets;

   /*
    * We have to be able to tell the difference between the first
    * packet and a wrapped counter. So just set the validlqrrcvd
    * flag when we receive the first LQR and use that to test with.
    */

    if ((PeerOutLQRs != 0) && (!l->validlqrrcvd) && (PeerInLQRs !=0))
        l->validlqrrcvd = 1;

    /*
     * Implement outbound link-quality policy, but only if inbound
     * section of LQR is new and valid. If PeerInLQRs is zero then
     * the LastOutXxx fields are indeterminate and the PeerInXxx
     * contain the initial values.
     */

    if ((l->validlqrrcvd) &&  
        (LastOutLQRs != l->cLastOutLQRs) && /*  is it a new one? */
	(go->neg_quality)) {

	int i = l->out.index;

	/*
	 * Forget oldest result in history.
	 */
	if (l->out.history[i/BITSPERCHAR] & (1 << (i % BITSPERCHAR)))
	    l->out.successes--;	/* One less success */
	l->out.history[i/BITSPERCHAR] &= ~(1 << (i % BITSPERCHAR));

	/*
	 * Determine if link quality is ok.
	 * The link Packet Error Rate and Octet Error Rate must be above
	 * their respective bounds, as determined by the policy.
	 * Also, our peer must have heard a recent outbound LQR.
	 * If the last Identifier he heard is two or more behind the
	 * last one we sent, then at least one must have been lost
	 * (possibly two if we are perfectly synchronized).
	 * Policy: Count up one failure on the outbound link.
	 */

        /*
         * Check if the number of packets we've transmitted vs. the
         * number of packets they've received is within the threshold
         * that has been given.
         *
         *      100 *  ( my xmit - his rcv) 
         *      ---------------------------- 	< 20% (def)
         *        			my xmit 
         */
	if ((myxmitdiffp == 0 ||
	     (100 * (myxmitdiffp - peerrcvdiffp)) / myxmitdiffp < 
	     100 - l->ppp_quality_threshold) &&
	    (myxmitdiffo == 0 ||
	     (100 * (myxmitdiffo - peerrcvdiffo)) / myxmitdiffo < 
	     100 - l->ppp_quality_threshold) &&
	    (l->OutLQRs - LastOutLQRs <= 1)) { 
	    l->out.successes++;	/* One more success */
	    l->out.history[i/BITSPERCHAR] |= (1 << (i % BITSPERCHAR));
	} else {
	    if (ppp_error_debug) {
		buginf("\nPPP %s(i): rlqr transmit failure. successes = %d ", 
		       idb->hw_namestring, l->out.successes);
                buginf("\nPPP: myxmitdiffp = %d peerrcvdiffp = %d", 
		       myxmitdiffp, peerrcvdiffp);
                buginf("\nPPP: myxmitdiffo = %d peerrcvdiffo = %d", 
		       myxmitdiffo, peerrcvdiffo);
                buginf("\nPPP: l->OutLQRs = %d LastOutLQRs = %d", 
		       l->OutLQRs, LastOutLQRs);
                buginf("\nPPP: threshold = %d",
		       100 - l->ppp_quality_threshold);
	    }
	}
	l->out.index++;		/* Increment history index */

	if (l->out.index >= l->out.n)
	    l->out.index = 0;
    }

    /*
     * If this is not the first LQR received, then the inbound report is
     * valid.  Implement inbound/outbound link-quality policy.
     */
    if ((l->validlqrrcvd) && (go->neg_quality)) {
	int i = l->in.index;

	/*
	 * Forget oldest result in history.
	 */

	if (l->in.history[i/BITSPERCHAR] & 1 << i % BITSPERCHAR)
	    l->in.successes--;	/* One less success */
	l->in.history[i/BITSPERCHAR] &= ~(1 << i % BITSPERCHAR);

       	/*
         * Determine if link quality is ok.
         * The link Packet Error Rate and Octet Error Rate must be above
         * their respective bounds, as determined by the policy.
         *
         *      100 * (his xmit - my recv)
         *      --------------------------		< 20% (def) 
         *         		his xmit
         */

	if ((peerxmitdiffp == 0 ||
	     (100 * (peerxmitdiffp - myrcvdiffp)) /  peerxmitdiffp < 
	     100 - l->ppp_quality_threshold) &&
	    (peerxmitdiffo == 0 ||
	     (100 * (peerxmitdiffo - myrcvdiffo)) / peerxmitdiffo < 
	     100 - l->ppp_quality_threshold)) {
	    l->in.successes++;	/* One more success */
	    l->in.history[i/BITSPERCHAR] |= 1 << i % BITSPERCHAR;
	} else {
	    if (ppp_error_debug) {
		buginf("\nPPP %s(i): rlqr receive failure.  successes = %d",
		       idb->hw_namestring, l->in.successes);
                buginf("\nPPP: myrcvdiffp = %d peerxmitdiffp = %d", 
		       myrcvdiffp, peerxmitdiffp);
                buginf("\nPPP: myrcvdiffo = %d peerxmitdiffo = %d", 
		       myrcvdiffo, peerxmitdiffo);
                buginf("\nPPP: threshold = %d",
		       100 - l->ppp_quality_threshold);
            }
	}
	l->in.index++;		/* Increment history index */

	if (l->in.index >= l->in.n)
	    l->in.index = 0;
    }

    l->lqrrcvd = 1;		/* We've received an LQR now */
    l->lqrrcvdthisperiod = 1;

    /*
     * Finally, make a judgement on the link quality.
     */
    if ((!l->poorquality) && (go->neg_quality)) {
	if (l->out.successes < l->out.k || l->in.successes < l->in.k) {
	    if ((l->out.successes < l->out.k) && (ppp_error_debug)) {
		buginf("\nPPP %s(i): rlqr OUT fail. myxmitdiffp = %d"
		       " peerrcvdiffp = %d",
		       idb->hw_namestring, myxmitdiffp, 
		       peerrcvdiffp);
		buginf("\nPPP: rlqr OUT fail. myxmitdiffo = %d "
		       "peerrcvdiffo = %d",
		       myxmitdiffo, peerrcvdiffo);
		buginf("\nPPP: rlqr OUT fail. PeerOutLQRs = %d "
		       "l->LastOutLQRs = %d",
		       PeerOutLQRs, l->LastOutLQRs);
	    } 
	    if ((l->in.successes < l->in.k) && (ppp_error_debug)) {
		buginf("\nPPP %s(i): rlqr IN fail. myrcvdiffp = %d "
		       "peerxmitdiffp = %d",
		       idb->hw_namestring, myrcvdiffp, 
		       peerxmitdiffp);
		buginf("\nPPP: rlqr IN fail. myrcvdiffo = %d "
		       "peerxmitdiffo = %d",
		       myrcvdiffo, peerxmitdiffo);
	    }
	    lcp_poorquality(f);
        }
    } else {
	if ((l->out.successes >= l->out.k && l->in.successes >= l->in.k) &&
	    (go->neg_quality))
	    lcp_goodquality(f);
    }

    /*
     * Is this the same PeerInLQRs as the last time ?
     * If so we need to send another LQR.
     */

    if (PeerInLQRs == l->cPeerInLQRs)
	sendflag = 1;

   /*
    * Let's make a copy of this message for next time
    */

    l->cLastOutLQRs = LastOutLQRs;
    l->cLastOutPackets = LastOutPackets;
    l->cLastOutOctets = LastOutOctets;
    l->cPeerInLQRs = PeerInLQRs;
    l->cPeerInPackets = PeerInPackets;
    l->cPeerInDiscards = PeerInDiscards;
    l->cPeerInErrors = PeerInErrors;
    l->cPeerInOctets = PeerInOctets;
    l->cPeerOutLQRs = PeerOutLQRs;
    l->cPeerOutPackets = PeerOutPackets;
    l->cPeerOutOctets = PeerOutOctets;

    /* Update fields from this packet */

    l->LastOutLQRs = PeerOutLQRs;
    l->LastOutPackets = PeerOutPackets;
    l->LastOutOctets = PeerOutOctets;
    l->SaveInLQRs = PeerInLQRs;
    l->SaveInPackets = rxpackets;
    l->SaveInDiscards = idb->counters.output_nobuffers;
    l->SaveInErrors = idb->counters.input_error;
    l->SaveInOctets = rxoctets;
    l->InLQRs++;                        /* increment rcv LQR cnt */

rlqrdone:
    datagram_done(inpacket);

    /*
     * If we have not negotiated quality but he has we
     * need to respond to an incoming LQR with an
     * outgoing LQR or if we have received 2 LQR with
     * the same PeerInLQRs we need to send an LQR.
     */

    if ((!go->neg_quality) || (sendflag))
	lcp_slqr(idb);
}


/*
 * debugging: print out an entire Link Control packet
 */

static void lcp_print (paktype *pak)
{
    char protocol[100];
    uchar *p = (uchar *)(pak->network_start);
    idbtype *idb = pak->if_input;
    uchar code;
    char *s;

    if (!idb)
	idb = pak->if_output;

    code = *p;

    s = protocol;
    s += sprintf(protocol, "PPP %s: %c LCP %s(%d) id %d len %d",
  	    idb ? idb->namestring : "?", pak->if_input ? 'I' : 'O',
  	    *p > MAX_LCPTYPES ? "UNKNOWN" : ppp_packettypes[*p],
  	    *p,
  	    *(p+1),
  	    GETSHORT(p+2));

    /*
     * ppp_dataprint is better suited for display configuration option
     * information. If ECHOREQ or ECHOREP frames are sent to ppp_dataprint
     * the magic number is decoded into lcp_optionnames and can lead
     * to a very misleading trace.  Additionally, if it is a protocol
     * reject, it is also misleading.
     */

    if ((code == ECHOREQ) || (code == ECHOREP)) {
        sprintf(s, " magic %#x", GETLONG(p+4));
	buginf("\n%s", protocol);

    } else if (code == PROTREJ) {
        sprintf(s, " protocol = %#x", GETSHORT(p+4));
        buginf("\n%s", protocol);

    } else {
        p = (uchar *)(pak->network_start);
        ppp_dataprint(protocol, p+4, 
  		  (*(short *) (p+2))-4, lcp_optionnames,MAX_LCPOPTIONS);
    }
}

/*
 * ppp_dataprint
 * Print out a bunch of ppp style data fields. (type, length, data).
 * if we get passed a table of names, use those for "type"
 * Put each of the data fields on its own line
 */

#define DEBUGLEN 100
char pppdebugbuffer[DEBUGLEN];

void ppp_dataprint (
    char *prefix,
    uchar *p,
    short len,
    const char *const names[],
    int nnames)
{
    uchar type;
    short optlen;
    int i;
    char *s;

    if (prefix && *prefix)
	buginf("\n%s", prefix);
    while (len > 0) {
	s = pppdebugbuffer;
	type = *p++;
	optlen = *p++;
	if ((type <= nnames) && names)
	    s += sprintf(s, "\n   %s (%d)", names[type], optlen);
	else
	    s += sprintf(s, "\n   Type %d len %d", type, optlen);
	if (optlen > len || optlen < 2)
	    optlen = len;
	for (i=0; i < optlen-2; i++) {
	    /* just to make sure there is space in the array for this output */
	    if (s < pppdebugbuffer + (DEBUGLEN - 7))
	        s += sprintf(s, " %#x", p[i]);
	    else
	        break;
	}
	len -= optlen;
	p += optlen-2;
	buginf("%s", pppdebugbuffer);
    }
}


/*
 * If callback option is supported, extract the callback variable message 
 * contents and return TRUE. Otherwise return FALSE.
 */
static boolean ppp_callback_accept (lcp_options *o,
				    uchar option,
				    uchar len,
				    uchar *ptr)
{
   switch (option){
     case CALLBACK_PPP_NOLOCATION:
        if (len == 0)
           return(TRUE);
        else
           return(FALSE);
        break;
     case CALLBACK_PPP_DIALSTRING:          
     case CALLBACK_PPP_E164:      
        /*
         * Store the variable portion of message in option 
	 * after freeing previous message.
         */
	if (o->callback_message) {
	    free(o->callback_message);
	    o->callback_message = NULL;
	}
        if ((len > 0) && (len < MAXSTR) && 
            (o->callback_message = malloc_named(len+1,
						"PPP C-Back message"))){
           /*
            * Warning - o->callback_message does have len+1, i.e. one
            * greater than than the lcp packet since we know intimately
            * that the pak's length is far greater than the lcp packet. 
            * All this to make up for sstrncpy always terminating with a 
            * NULL on the last byte in destination string
            */
           sstrncpy(o->callback_message, ptr, len+1);
           if (ppp_negotiation_debug)
              buginf("\n PPP Callback string allocated %s", 
                      o->callback_message);
           return(TRUE);
	}
        return(FALSE);
        break;
     case CALLBACK_PPP_DN:
        if (ppp_negotiation_debug)
           buginf("\n PPP Callback Distinguished Name not supported");
        break;
     case CALLBACK_PPP_LOCATION:
        if (ppp_negotiation_debug)
           buginf("\n PPP Callback Location not supported");
        break;
     default:
        break; 
     }
   return(FALSE);
}

/*
 * This is taken directly from the IETF's rfc1331.  It generates a CRC16
 * FCS check. 
 */

static const u16 fcstab[256] = {
0x0000,0x1189,0x2312,0x329b,0x4624,0x57ad,0x6536,0x74bf,
0x8c48,0x9dc1,0xaf5a,0xbed3,0xca6c,0xdbe5,0xe97e,0xf8f7,
0x1081,0x0108,0x3393,0x221a,0x56a5,0x472c,0x75b7,0x643e,
0x9cc9,0x8d40,0xbfdb,0xae52,0xdaed,0xcb64,0xf9ff,0xe876,
0x2102,0x308b,0x0210,0x1399,0x6726,0x76af,0x4434,0x55bd,
0xad4a,0xbcc3,0x8e58,0x9fd1,0xeb6e,0xfae7,0xc87c,0xd9f5,
0x3183,0x200a,0x1291,0x0318,0x77a7,0x662e,0x54b5,0x453c,
0xbdcb,0xac42,0x9ed9,0x8f50,0xfbef,0xea66,0xd8fd,0xc974,
0x4204,0x538d,0x6116,0x709f,0x0420,0x15a9,0x2732,0x36bb,
0xce4c,0xdfc5,0xed5e,0xfcd7,0x8868,0x99e1,0xab7a,0xbaf3,
0x5285,0x430c,0x7197,0x601e,0x14a1,0x0528,0x37b3,0x263a,
0xdecd,0xcf44,0xfddf,0xec56,0x98e9,0x8960,0xbbfb,0xaa72,
0x6306,0x728f,0x4014,0x519d,0x2522,0x34ab,0x0630,0x17b9,
0xef4e,0xfec7,0xcc5c,0xddd5,0xa96a,0xb8e3,0x8a78,0x9bf1,
0x7387,0x620e,0x5095,0x411c,0x35a3,0x242a,0x16b1,0x0738,
0xffcf,0xee46,0xdcdd,0xcd54,0xb9eb,0xa862,0x9af9,0x8b70,
0x8408,0x9581,0xa71a,0xb693,0xc22c,0xd3a5,0xe13e,0xf0b7,
0x0840,0x19c9,0x2b52,0x3adb,0x4e64,0x5fed,0x6d76,0x7cff,
0x9489,0x8500,0xb79b,0xa612,0xd2ad,0xc324,0xf1bf,0xe036,
0x18c1,0x0948,0x3bd3,0x2a5a,0x5ee5,0x4f6c,0x7df7,0x6c7e,
0xa50a,0xb483,0x8618,0x9791,0xe32e,0xf2a7,0xc03c,0xd1b5,
0x2942,0x38cb,0x0a50,0x1bd9,0x6f66,0x7eef,0x4c74,0x5dfd,
0xb58b,0xa402,0x9699,0x8710,0xf3af,0xe226,0xd0bd,0xc134,
0x39c3,0x284a,0x1ad1,0x0b58,0x7fe7,0x6e6e,0x5cf5,0x4d7c,
0xc60c,0xd785,0xe51e,0xf497,0x8028,0x91a1,0xa33a,0xb2b3,
0x4a44,0x5bcd,0x6956,0x78df,0x0c60,0x1de9,0x2f72,0x3efb,
0xd68d,0xc704,0xf59f,0xe416,0x90a9,0x8120,0xb3bb,0xa232,
0x5ac5,0x4b4c,0x79d7,0x685e,0x1ce1,0x0d68,0x3ff3,0x2e7a,
0xe70e,0xf687,0xc41c,0xd595,0xa12a,0xb0a3,0x8238,0x93b1,
0x6b46,0x7acf,0x4854,0x59dd,0x2d62,0x3ceb,0x0e70,0x1ff9,
0xf78f,0xe606,0xd49d,0xc514,0xb1ab,0xa022,0x92b9,0x8330,
0x7bc7,0x6a4e,0x58d5,0x495c,0x3de3,0x2c6a,0x1ef1,0x0f78
};


/*
 * Calculate a new fcs given the current fcs and the new data.
 */
u16 pppfcs (register u16 fcs, register unsigned char *cp, register int len)
{
	while (len--)
		fcs = (fcs >> 8) ^ fcstab[(fcs ^ *cp++) & 0xff];
	return (fcs);
}


/*
 * All authorization routines are here.
 */

static void fsm_clear_author (fsm *f)
{
    aaa_clear_attrs(&f->cached_author);
    f->ncp_did_author = FALSE;
}

typedef struct author_queuetype_ {
    struct author_queuetype_ *next;
    fsm *fsm;
} author_queuetype;

static boolean fsm_author_in_progress (fsm *f)
{
    author_queuetype *fp;

    /* If already doing author for same fsm, then wait for it and return. */
    for (fp = (author_queuetype *)fsm_authorQ.qhead; fp; fp = fp->next) {
	if (fp->fsm == f) {
	    return(TRUE);
	}
    }
    return(FALSE);
}

static boolean fsm_do_author (hwidbtype *idb, fsm *f)
{
    authorstruct author;
    boolean ret;
    int i, avail = 0;
    char *attr, *val;
    enum AV_TYPE type;
    author_queuetype *fp;
    char *description = "UNKNOWN";
    char *attrval;

    fp = malloc_named(sizeof(author_queuetype), "PPP Authorization");
    if (fp) {
	fp->fsm = f;
	enqueue(&fsm_authorQ, fp);
    }

    memset(&author, 0, sizeof(authorstruct));
    author.auth_idb = idb;
    author.auth_tty = idb->idb_tty;

    /* Build minimal AV list */
    ret = aaa_add_AV(&author.out_args, &avail, AAA_ATTR_service,
		     AAA_VAL_SVC_ppp, AAA_AV_MAND);

    if (!aaa_author_needed(AAA_AUTHOR_TYPE_NET, 0, idb, idb->idb_tty)) {
	f->cached_author = aaa_copy_attrs(author.out_args);	/* Dummy */
	aaa_cleanup_author(&author);
	if (fp) {
	    unqueue(&fsm_authorQ, fp);
	    free(fp);
	}
	return(TRUE);		/* Authorization not required */
    }

    if (ret) {
	switch (f->protocol) {
	case TYPE_PPP_LCP:
	    description = "LCP";
	    /*
	     * Do nothing and return TRUE, because if we are in LCP
	     * then authentication has not been done yet.
	     * f->cached_author will hold dummy args.
	     */
	    ret = aaa_add_AV(&author.out_args, &avail, AAA_ATTR_protocol,
			     AAA_VAL_PROT_lcp, AAA_AV_MAND);
	    if (ret) {
		aaa_clear_attrs(&f->cached_author);
		f->cached_author = author.out_args;
		author.out_args = NULL;
	    }
	    aaa_cleanup_author(&author);
	    if (fp) {
		unqueue(&fsm_authorQ, fp);
		free(fp);
	    }
	    return(TRUE);
	    break;
	case TYPE_PPP_IPCP:
	    description = "IPCP";
	    attrval = AAA_VAL_PROT_ip;
	    break;
	case TYPE_PPP_IPXCP:
	    description = "IPXCP";
	    attrval = AAA_VAL_PROT_ipx; 
	    break;
	case TYPE_PPP_VINESCP:
	    description = "VINESCP";
	    attrval = AAA_VAL_PROT_vines;
	    break;
	case TYPE_PPP_ETHERTALKCP:
	    description = "ATCP";
	    attrval = AAA_VAL_PROT_atalk;
	    break;
	case TYPE_PPP_NBFCP:
	    description = "NBFCP";
	    attrval = AAA_VAL_PROT_nbfcp;
	    break;
	case TYPE_PPP_OSICP:
	    description = "OSICP";
	    attrval = AAA_VAL_PROT_osicp;
	    break;
	case TYPE_PPP_DECCP:
	    description = "DECCP";
	    attrval = AAA_VAL_PROT_deccp;
	    break;
	default:
	    attrval = AAA_VAL_PROT_unknown;
	    break;
	}
	ret = aaa_add_AV(&author.out_args, &avail, AAA_ATTR_protocol,
			 			attrval, AAA_AV_MAND);
    }
	
    if (ret) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/FSM (%u): can we start %s?", 
		   author.session_id, description);
	}

	ret = aaa_do_author(idb->lcpstate->ustruct, &author, 
			    AAA_AUTHOR_TYPE_NET, 0);

	if (ret) {
	    switch(author.status) {
	    case AAA_AUTHOR_STATUS_ERROR:
	    case AAA_AUTHOR_STATUS_FAIL:
		ret = FALSE;
		break;
	    case AAA_AUTHOR_STATUS_PASS_ADD:
		if (!author.in_args) {
		    aaa_clear_attrs(&f->cached_author);
		    f->cached_author = aaa_copy_attrs(author.out_args);
		    break;
		}
		aaa_merge_attrs(author.out_args, author.in_args,
				&author.in_args);
		/* FALLTHRU */
	    case AAA_AUTHOR_STATUS_PASS_REPL:
		if (!author.in_args) {
		    aaa_clear_attrs(&f->cached_author);
		    break;
		}
		for (i = 0; ret && author.in_args[i]; i++) {
		    if (aaa_split_AV(author.in_args[i], &attr, &val, &type)) {
			if (!strcmp(attr, AAA_ATTR_service)) {
			    if (strcmp(val, AAA_VAL_SVC_ppp)) {
				if (type == AAA_AV_MAND) {
				    ret = FALSE;
				} else {
				    if (aaa_author_debug)
					buginf("\nAAA/AUTHOR/FSM/%s: "
					       "tried to change \"service\".  "
					       "Ignoring.", description);
				}
			    }
			    continue;
			}
			if (!strcmp(attr, AAA_ATTR_protocol)) {
			    /* Ignore this */
			    continue;
			}
		    }
		}
		if (ret) {
		    aaa_clear_attrs(&f->cached_author);
		    f->cached_author = aaa_copy_attrs(author.in_args);
		}
		break;
	    }
	}
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/FSM: we %s start %s for protocol %#x",
	       ret ? "can" : "cannot", 
	       description, 
	       (int)f->protocol);
    }
    aaa_cleanup_author(&author);
    if (fp) {
	unqueue(&fsm_authorQ, fp);
	free(fp);
    }
    return(ret);
}


static boolean fsm_check_author (fsm *f)
{
    int i;
    boolean ret = TRUE;
    char **attrs = f->cached_author;
    char *attr, *val;
    enum AV_TYPE type;

    /* Don't do LCP check here */
    if (f->protocol == TYPE_PPP_LCP)
	return(TRUE);

    if (!attrs) {
	/* Shouldn't happen */
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/NCP: %s: Denied -- No AV pairs",
		   f->idb->hw_namestring);
	}
	return(FALSE);
    }

    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/FSM: Check for unauthorized mandatory AV's");
    }

    for (i = 0; ret && attrs[i]; i++) {
	if (aaa_author_debug) {
	    buginf("\nAAA/AUTHOR/FSM: Processing AV %s", 
		   attrs[i]);
	}

	if (aaa_split_AV(attrs[i], &attr, &val, &type)) {
	    if (!strcmp(attr, AAA_ATTR_service) ||
		!strcmp(attr, AAA_ATTR_protocol))
		continue;
	    if (type == AAA_AV_MAND) {
		if (aaa_author_debug) {
		    buginf("\nAAA/AUTHOR/FSM: Received mandatory "
			   "attribute \"%s\" for PPP protocol %#x.  Failing.",
			   attr, f->protocol);
		}
		ret = FALSE;
	    }
	}
    }
    if (aaa_author_debug) {
	buginf("\nAAA/AUTHOR/FSM: %s", ret ? "succeeded" : "denied");
    }
    return(ret);
}


/*
 * PPP continuation from VPN (when it is not forwarded)
 */
static void ppp_continue_vpn (int protocol, paktype *pak, hwidbtype *idb)
{
   (*prottbl[prottbl_lookup(protocol)].input)(pak, idb);
}

/*
 * Check the LCP state
 */
static boolean ppp_lcpstate (hwidbtype *link_hwidb, int state)
{
   if (link_hwidb && link_hwidb->lcpstate && 
       (link_hwidb->lcpstate->phase == state))
      return(TRUE);
   return(FALSE);
}

/*
 * Set the LCP state
 */
static boolean ppp_set_lcpstate (hwidbtype *link_hwidb, int state)
{
   if (link_hwidb && link_hwidb->lcpstate) {
      link_hwidb->lcpstate->phase = state;
      return(TRUE);
   }
   return(FALSE);
}

/*
 * protocol_to_authtype
 *
 * Return internal authentication type (from parser defs) of PPP authentication
 *  protocol number and digest value.
 */

static uchar protocol_to_authtype (ushort protocol, uchar digest)
{
    switch (protocol) {
    case TYPE_PPP_CHAP:
	switch (digest) {
	case DIGEST_MD5:	return PPP_AUTH_CHAP;
	case DIGEST_DES:	return PPP_AUTH_MSCHAP;
	default:		return 0;
	}

    case TYPE_PPP_UPAP:
	return PPP_AUTH_PAP;

    default:
	return 0;
    }
}


/*
 * authtype_name
 *
 * Return pointer to fixed string for adding to buginfs.
 */

static char *authtype_name (uchar authtype)
{
    switch (authtype) {
    case PPP_AUTH_CHAP:		return "CHAP";
    case PPP_AUTH_MSCHAP:	return "MS CHAP";
    case PPP_AUTH_PAP:		return "PAP";
    default:			return "Unknown";
    }
}


/*
 * get_another_auth_method
 *
 * Return TRUE and adjust go->authtypes[0] if another method available.
 *  Use suggested authtype (zero if this is an option reject) if possible
 *  else use the next protocol in the list.
 */

static void get_another_auth_method (fsm *f, uchar authtype)
{
    int i;
    lcp_options *go;
    lcp_options *wo;

    go = f->gotoptions;
    wo = f->wantoptions;

    /*
     * First, see if there is a suggestion and if the suggestion is in our
     * list.  That is what we are here for.  Maybe it will be that simple.
     */
    if (authtype && auth_method_supported(f, authtype)) {
	go->authtypes[0] = authtype;	/* use their suggestion next times */
        return;
    }

    /*
     * Suggestion was worthless, so see if we have another method to hand
     * out.  At end of list, wrap back to the beginning.
     */
    for (i = 0; (i < PPP_AUTH_NUM) && wo->authtypes[i]; i++) {
	if (wo->authtypes[i] == go->authtypes[0]) {
	    /*
	     * Found the one we tried the last time (which the peer evidently
	     * didn't like).  See if there is another one to handout.
	     */
	    if ((++i < PPP_AUTH_NUM) && wo->authtypes[i]) {
		go->authtypes[0] = wo->authtypes[i];
		return;		/* Another one to hand out */
	    }
	    break;
	}
    }
    /*
     * End of list reached.  Start over with the first choice.
     */
    go->authtypes[0] = wo->authtypes[0];
    return;
}


/*
 * auth_method_supported
 *
 * Return TRUE if authentication method is in a list of supported methods.
 */

boolean auth_method_supported (fsm *f, uchar authtype)
{
    int i;
    lcp_options *wo;

    wo = f->wantoptions;

    for (i = 0; (i < PPP_AUTH_NUM) && wo->authtypes[i]; i++) {
	if (wo->authtypes[i] == authtype)
	    return TRUE;
    }
    return FALSE;
}

/*
 * ppp_lapb_event
 * Called by LAPB to notify PPP of link reset or invalid address
 * Reset the interface and let PPP renegotiate numbered or unnumbered mode. 
 */
static void ppp_lapb_event (hwidbtype *hwidb)
{
    fsm *f;
    lcp_options *wo;
    
    if ((hwidb == NULL) || (hwidb->lcpfsm == NULL))
	return;
    f = hwidb->lcpfsm;
    wo = f->wantoptions;
    if (wo->neg_numbered_mode) {
	/*
	 * Since we are invoked at interrupt level, we can not reset
	 * the interface here.  Defere reseting the interface to
	 * net_periodic() which runs at process level.
	 */
	wo->neg_numbered_mode = FALSE;
    }
}

/*
 * ppp_send_lapb_dm
 * Generate and send a Disconnect Mode (DM) frame.
 * The DM frame is transmitted as a response and is used to inform the
 * primary that the secondary is in a logically disconnected state.
 *
 * Per RFC1663, send a LAPB DM frame when PPP (running in non-LAPB mode)
 * detects a LAPB frame.
 */
static void ppp_send_lapb_dm (paktype *pak, hwidbtype *hwidb)
{
    pak->datagramsize = LAPB_OVERHEAD_8;
    pak->datagramstart[LAPB_CONTROL_INX] =
	(pak->datagramstart[LAPB_CONTROL_INX] & ~LAPB_U_CONTROL_M) | LAPB_DM;
    if ((*hwidb->oqueue)(hwidb, pak, TAIL)) {
	(*hwidb->soutput)(hwidb);
    }
}
