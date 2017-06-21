/* $Id: types.h,v 3.13.4.22 1996/09/14 00:52:14 ramk Exp $
 * $Source: /release/112/cvs/Xsys/h/types.h,v $
 *------------------------------------------------------------------
 * types.h - Major system types
 *
 * December 22, 1992, Kevin Paul Herbert
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This module contains the definitions for all of the major system
 * types.
 *------------------------------------------------------------------
 * $Log: types.h,v $
 * Revision 3.13.4.22  1996/09/14  00:52:14  ramk
 * CSCdi66329:  SNMP syslog traps specify wrong enterpriseOID,sysLogEvent
 * needs fix
 * Branch: California_branch
 * Save sysloghistoryentry_t data until traps can be sent from process
 * level
 * (if syslog trap notification is enabled). Create SyslogTrapProcess to
 * handle the sending of traps and freeing buffers used for saving the
 * data
 *
 * Revision 3.13.4.21  1996/08/02  22:53:09  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.13.4.20  1996/07/28  07:32:44  irfan
 * CSCdi60568:  v120 autodetect not working
 * Branch: California_branch
 * allow serial interfaces to autodetect encaps on the wire for incoming
 * connections.
 *
 * Revision 3.13.4.19  1996/07/23  13:22:07  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.13.4.18  1996/07/19  19:48:02  wmay
 * CSCdi60728:  ISDN dialer holdQ not handled correctly
 * Branch: California_branch
 *
 * Revision 3.13.4.17  1996/06/24  09:20:39  fox
 * CSCdi59550:  PPP causes router to crash
 * Branch: California_branch
 * Slim down and speed up linktype to NCP index to PPP protocol lookups.
 * Extend NCP protocheck functions to pass original IDB as well as config
 * IDB.  Change NCP names to upper case.  Display NCP name in trace of
 * PPP negotiation.
 *
 * Revision 3.13.4.16  1996/06/16  21:12:18  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.13.4.15  1996/05/29  14:02:42  bew
 * CSCdi56794:  IOS private configuration not always recovered from NVRAM
 * Branch: California_branch
 * Move the nvtype_private structure reference from config.h to types.h.
 *
 * Revision 3.13.4.14  1996/05/26  05:25:34  dkatz
 * CSCdi58637:  Managed timer manipulation is inefficient
 * Branch: California_branch
 * Search from the end of each timer list, rather than the beginning.
 *
 * Revision 3.13.4.13  1996/05/17  11:16:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.12.4.6  1996/05/14  06:18:52  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 3.12.4.5  1996/05/05  23:05:50  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.12.4.4  1996/04/26  07:53:03  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.12.4.3  1996/04/05  06:06:25  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.12.4.2  1996/04/03  14:26:22  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.12.4.1  1996/03/17  17:48:14  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.13.4.12  1996/05/10  01:37:37  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.13.4.11  1996/05/09  14:13:39  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.13.4.10  1996/05/04  00:42:38  wilber
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
 * Revision 3.13.4.9  1996/04/29  05:30:28  mdb
 * CSCdi56007:  unix images have problems with multiple size_t definitions
 * Branch: California_branch
 *
 * Revision 3.13.4.8  1996/04/27  05:47:12  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.13.4.7.2.1  1996/04/27  06:35:37  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.13.4.7  1996/04/25  23:11:24  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.7.8.9  1996/04/26  00:51:12  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.7.8.8  1996/04/23  00:31:23  suresh
 * Branch: Dial1_branch
 * Add Dialer Profiles support to Dial1_branch
 *
 * Revision 3.7.8.7  1996/04/17  01:33:15  syiu
 * Branch: Dial1_branch
 * Sync Dial1_branch to V111_1_3
 *
 * Revision 3.7.8.6  1996/03/22  05:33:39  tkolar
 * Branch: Dial1_branch
 * Reshape the way MLPVT interacts with VPN.
 *
 * Revision 3.7.8.5  1996/02/23  20:12:58  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.7.8.4  1996/01/31  18:31:56  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 3.7.8.3  1996/01/22  21:38:01  syiu
 * Branch: Dial1_branch
 * More L2F registries
 *
 * Revision 3.7.8.2  1996/01/18  04:58:47  perryl
 * Branch: Dial1_branch
 * Virtual Access interface services
 *
 * Revision 3.7.8.1  1996/01/16  15:32:29  dblair
 * CSCdi42375:  Move Multilink PPP into PPP subsystem
 * Branch: Dial1_branch
 *
 * Revision 3.13.4.6  1996/04/19  15:15:38  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commmit
 *
 * Revision 3.13.4.5  1996/04/17  13:37:42  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 * Revision 3.13.4.4  1996/04/16  18:52:47  rchandra
 * CSCdi54830:  IP features commit
 * Branch: California_branch
 *
 * Revision 3.13.4.3  1996/04/15  14:57:58  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.13.4.2  1996/04/02  05:36:18  dkatz
 * CSCdi36031:  Running out of memory running 2 or more EIGRP ASs
 * Branch: California_branch
 * EIGRP Jumbo Patch.
 *
 * Revision 3.13.4.1.8.1  1996/04/08  01:46:10  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.13.4.1  1996/03/18  19:37:50  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.13.8.1  1996/03/22  09:35:56  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.13  1996/03/09  22:54:59  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.12.8.2  1996/03/08  01:16:57  rbadri
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 * typedef for lsv_client to satisfy a registry call.
 *
 * Revision 3.12.8.1  1996/03/03  21:22:54  rlowe
 * Apply SYNALC content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.12.6.1  1996/03/02  01:18:53  rlowe
 * Non-sync sync to mainline tag V111_1_0_3.
 * Branch: Synalc3_Cal_V111_1_0_3_branch
 *
 * Revision 3.12  1996/02/08  23:08:41  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.11  1996/02/04  06:06:20  myeung
 * CSCdi48132:  Extract additional OSPF specific information out of pdbtype
 *
 * Revision 3.10  1996/01/23  11:55:34  mdb
 * CSCdi47339:  apply code review comments for CSCdi47065
 *
 * Revision 3.9  1996/01/22  09:06:31  mdb
 * CSCdi47263:  change definition of NULL to ((void *) 0) from 0
 *
 * Revision 3.8  1996/01/18  21:43:53  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 * Add a new macro IPADDR_ZERO to be used instead of NULL or 0 or 0L
 * for variables of ipaddrtype.
 * Note: This patch does not yet modify the value of the NULL macro.
 *
 * Revision 3.7.12.1  1996/02/29  07:48:16  rlowe
 * First pass non-sync sync for Synergy ATM line card to V111_0_16.
 * Branch: Synalc3_Cal_V111_0_16_branch
 *
 * Revision 3.7  1996/01/04  22:15:02  lol
 * CSCdi43469:  per-user route wont use default interface
 *
 * Revision 3.6  1995/12/12  09:02:50  dbath
 * CSCdi45422:  HSA systems forget what time it is on failover
 * Whenever we write to the battery-backed calendar hardware on
 * the master, ship the value across to the slave's calendar too.
 *
 * Revision 3.5  1995/11/30  04:41:06  mleelani
 * CSCdi44573:  IGMP doesnt process leave messages
 * o Add support for leave messages.
 * o Create idb sub-block for IGMP; use managed timers for IGMP timers.
 *
 * Revision 3.4  1995/11/21  07:28:53  lpereira
 * CSCdi44255:  APPN build is broken with the PortReady changes
 * Moved the enum HEADTAIL from globs.h to types.h so that interface.h
 * can be included without including globs.h. Also added a forward
 * reference to list_header in types.h.
 *
 * Revision 3.3  1995/11/17  09:13:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:24:43  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:36:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.15  1995/11/08  20:59:10  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.14  1995/10/23  03:56:54  dkatz
 * CSCdi42575:  Managed timers could be more robust
 * Try to avoid interpreting garbage as indicating that the managed
 * timer has been initialized.
 *
 * Revision 2.13.2.8.2.1  1996/02/03  07:06:30  rlowe
 * Non-sync sync of Synalc_catchup_branch to V111_0_5.
 * Branch: Synalc_catchup_pretal_branch
 *
 * Revision 2.13  1995/09/23  21:38:44  dkatz
 * CSCdi38306:  ISIS shouldnt run SPF on leaf route changes
 * Add an ISIS registry.  STRUCTURE_REF some structures.
 *
 * Revision 2.12  1995/09/18  19:34:43  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.11  1995/09/18  07:27:08  smackie
 * Prune buffer code into discrete and manageable sections to restore some
 * sanity and focus to buffers.c (CSCdi40529).
 *
 *   o buffers.c - buffer management code
 *   o element.c - queue element code
 *   o queue.c   - singly-linked queue support
 *
 * Revision 2.10  1995/09/15  19:22:18  lmercili
 * CSCdi34816:  add dynamic mapping for ipx/smds
 *
 * Revision 2.9  1995/09/09  00:51:27  ronnie
 * CSCdi35837:  ping works, but traceroute fails to go across ATM interface
 * Introduce a new variable type, fuzzy, for hwidb->broadcast to return
 * instead of boolean.  This allows the return of NOT_SURE.  Adjust all
 * hwidb->broadcast routines to return fuzzy instead of boolean.
 *
 * Revision 2.8  1995/08/25  14:10:49  hampton
 * Eliminate unnecessary includes of novell.h.  [CSCdi39328]
 *
 * Revision 2.7  1995/08/23 18:27:01  dkatz
 * CSCdi37901:  ISIS first SPF is unpredictable
 * Soup up ISIS SPF debugging.
 *
 * Revision 2.6.18.2  1995/10/16  23:43:47  sakumar
 * Defined structure lsv_client_t for LANE registry use.
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.6.18.1  1995/09/11  01:56:09  rlowe
 * Non-sync sync of Synalc2_branch to LE2_V110_1_0_3_branch,
 * yielding Synalc2_LE2_V110_1_0_3_branch.
 *
 * Branch: Synalc2_LE2_V110_1_0_3_branch
 *
 * Revision 2.6  1995/08/07  05:32:05  richl
 * CSCdi38315:  makefile rework
 *
 * Revision 2.5  1995/08/03  21:48:20  hampton
 * Chunk manager should record chunk usage high water mark. [CSCdi38207]
 *
 * Revision 2.4  1995/07/24  09:23:41  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.3  1995/07/19 21:01:53  pmorton
 * CSCdi25556:  dynamic mapping between x121 and qllc virtual mac
 * addresses.
 * Dynamic mapping implemented by DLSw+ over qllc.
 * Reduce qllc data fields in the idb.  Precursor to a qllc subblock.
 *
 * Revision 2.2  1995/06/27  17:07:19  tkolar
 * CSCdi35462:  DES code needs to be its own subsystem
 * Make it easier to export the source base.
 *
 * Revision 2.1  1995/06/07  20:37:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __TYPES_H__
#define __TYPES_H__

/*
 * Define another memory pointer
 *
 * This is soley in here for the use of ntp and radix code so we don't
 * have to make massive edits to those modules (it makes tracking against
 * the UNIX source easier).  Please do -not- use this in any cisco code.
 */

typedef unsigned char *caddr_t;

/*
 * Define the queue data type and a basic enqueue structure
 */

typedef struct queuetype_ {
    void *qhead;			/* head of queue */
    void *qtail;			/* tail of queue */
    int count;				/* possible count */
    int maximum;			/* maximum entries */
} queuetype;

typedef struct nexthelper_ {
    struct nexthelper_ *next;
    unsigned char       data[0];
} nexthelper;

/*
 * Define forward references to major system types
 */

#define STRUCTURE_REF(str) typedef struct str ## _ str

STRUCTURE_REF(watched_queue);		/* os/sched.h */
STRUCTURE_REF(watched_boolean);		/* os/sched.h */
STRUCTURE_REF(watched_bitfield);	/* os/sched.h */
STRUCTURE_REF(watched_semaphore);	/* os/sched.h */
STRUCTURE_REF(neighbortype);	/* gs/neighbor.h */
STRUCTURE_REF(ndbtype);		/* gs/route.h */
STRUCTURE_REF(pdbtype);		/* gs/route.h */
STRUCTURE_REF(rdbtype);		/* gs/route.h */
STRUCTURE_REF(ddbtype);		/* iprouting/dual.h */
STRUCTURE_REF(igrp2_idblink);	/* iprouting/dual.h */
STRUCTURE_REF(route_maptype);	/* gs/route_map.h */
STRUCTURE_REF(route_map_nametype);/*iprouting/route_map.h */
STRUCTURE_REF(route_map_headtype);/*iprouting/route_map.h */
STRUCTURE_REF(addrtype);	/* h/address.h */
STRUCTURE_REF(hwaddrtype);	/* h/address.h */
STRUCTURE_REF(conntype);	/* h/connect.h */
STRUCTURE_REF(connvector);	/* h/connect.h */
STRUCTURE_REF(iabtype);		/* h/snmp_interface.h */
STRUCTURE_REF(idbtype);		/* h/interface.h */
STRUCTURE_REF(hwidbtype);       /* h/interface.h */
STRUCTURE_REF(cdbtype);		/* hes/controller.h */
STRUCTURE_REF(nametype);	/* h/name.h */
STRUCTURE_REF(araptype);        /* arap/arap.h */
STRUCTURE_REF(mnp4type);        /* arap/arap.h */
STRUCTURE_REF(paktype);		/* h/packet.h */
STRUCTURE_REF(particletype);	/* h/packet.h */
STRUCTURE_REF(sprocess);	/* h/sched.h */
STRUCTURE_REF(subsystype);	/* h/subsys.h */
STRUCTURE_REF(tt_soc);		/* h/ttysrv.h */
STRUCTURE_REF(tty_3270def);	/* h/ttysrv.h */
STRUCTURE_REF(tty_editdata);	/* h/ttysrv.h */
STRUCTURE_REF(usernametype);	/* h/login.h */
STRUCTURE_REF(expr_tree_entry_t); /* h/access_expr.h */
STRUCTURE_REF(riftype);		/* if/rif.h */
STRUCTURE_REF(mempool);	        /* os/free.h */
STRUCTURE_REF(blocktype);	/* os/free.h */
STRUCTURE_REF(pooltype);	/* os/pool.h */
STRUCTURE_REF(chunk_type);	/* os/chunk.h */
STRUCTURE_REF(regiontype);      /* os/region.h */
STRUCTURE_REF(debug_context);	/* os/gdb.h */
STRUCTURE_REF(regexp);		/* os/regexp.h */
STRUCTURE_REF(signal_table);	/* os/signal.h */
STRUCTURE_REF(timer_t);		/* os/timer.h */
STRUCTURE_REF(sysloghistoryentry_t); /* os/syslogmib_history.h */
STRUCTURE_REF(filetype);	/* h/file.h */
STRUCTURE_REF(nvtype);		/* h/config.h */
STRUCTURE_REF(nvtype_private);  /* h/config.h */
STRUCTURE_REF(parseinfo);	/* h/config.h */
STRUCTURE_REF(translatetype);	/* pt/translate.h */
STRUCTURE_REF(bootptype);	/* servers/bootp.h */
STRUCTURE_REF(btetype);		/* srt/span.h */
STRUCTURE_REF(spantype);        /* srt/span.h */
STRUCTURE_REF(ipsocktype);	/* ip/ip.h */
STRUCTURE_REF(tcptype);		/* ip/ip.h */
STRUCTURE_REF(udptype);		/* ip/ip.h */
STRUCTURE_REF(tcbtype);		/* tcp/tcp.h */
STRUCTURE_REF(fsm);		/* wan/ppp.h */       /* bad name choice */
STRUCTURE_REF(lcp_state);	/* wan/ppp.h */
STRUCTURE_REF(auth_state);	/* wan/ppp_auth.h */
STRUCTURE_REF(pppContext);	/* ts/async_ppp.h */
STRUCTURE_REF(chat_script);	/* ts/chat.h */
STRUCTURE_REF(tty_statemtype);	/* ts/ttystatem.c */
STRUCTURE_REF(isdn_bri_t);	/* les/isdn_bri.h */
STRUCTURE_REF(dialerplisttype);	/* wan/dialer.h */
STRUCTURE_REF(dialerdbtype);	/* wan/dialer.h */
STRUCTURE_REF(dialer_stat);	/* wan/dialer.h */
STRUCTURE_REF(dialer_call_tab);	/* wan/dialer_private.h */
STRUCTURE_REF(dialermaptype);	/* wan/dialer_private.h */
STRUCTURE_REF(dialergrouptype);	/* wan/dialer_private.h */
STRUCTURE_REF(dialerpooltype);  /* wan/dialer_private.h */
STRUCTURE_REF(dialerlisttype);	/* wan/dialer_private.h */
STRUCTURE_REF(dialerpaktype);	/* wan/dialer_private.h */
STRUCTURE_REF(dialer_holdq_type);/* wan/dialer_private.h */
STRUCTURE_REF(mlplinktype);	/* wan/mlp.h */
STRUCTURE_REF(mlpbundletype);	/* wan/mlp.h */
STRUCTURE_REF(vpntype);         /* vpn/vpn.h */
STRUCTURE_REF(l2f_midtype);     /* vpn/l2f.h */
STRUCTURE_REF(l2f_tunneltype);  /* vpn/l2f.h */
STRUCTURE_REF(l2f_tunnel_infotype);  /* vpn/l2f.h */
STRUCTURE_REF(xrcontext);	/* xremote/xremote.h */
STRUCTURE_REF(tunnel_info);	/* if/tunnel.h */
STRUCTURE_REF(regexpacctype);	/* h/access.h */
STRUCTURE_REF(rpoatype);	/* x25/x25.h */
STRUCTURE_REF(profiletype);	/* x25/pad.h */
STRUCTURE_REF(vinesidbtype);	/* vines/vines.h */
STRUCTURE_REF(peerentry);	/* dlsw/dlsw_peer.h */
STRUCTURE_REF(tr_vpeertype);    /* srt/rsrb.h */
STRUCTURE_REF(tr_ventrytype);	/* srt/rsrb.h */
STRUCTURE_REF(tr_vgrouptype);	/* srt/rsrb.h */
STRUCTURE_REF(VrgT);		/* cls/dlcvdlc_private.h */
STRUCTURE_REF(dlc_handle_t);	/* srt/lack_externs.h */
STRUCTURE_REF(fs_hdrtype);      /* srt/srb.h */
STRUCTURE_REF(fs_expl_que);     /* srt/srb.h */
STRUCTURE_REF(fs_explorer_cb);  /* srt/srb.h */
STRUCTURE_REF(ipcache_cbtype);  /* ip/ipfast_cbus.c */
STRUCTURE_REF(ipcache_rntype);  /* ip/ipfast.h */
STRUCTURE_REF(ipcachetype);     /* ip/ipfast_index.h */
STRUCTURE_REF(encapinfotype);   /* ip/ipfast_index.h */
STRUCTURE_REF(snmpvartype);     /* servers/snmp_data.h */
STRUCTURE_REF(flash_dev_t);     /* os/dev.h, if/g_dev.h */
STRUCTURE_REF(snmpoptype);      /* servers/snmp_data.h */
STRUCTURE_REF(stunpeertype);	/* ibm/ibm.h */
STRUCTURE_REF(sdlc_proxy_interface); /* ibm/sdlc.h */
STRUCTURE_REF(sdlc_data_block);	/* ibm/sdlc.h */
STRUCTURE_REF(ring_info);	/* ibm/lanmgr.h */
STRUCTURE_REF(crs_data);	/* ibm/ibmnm.h */
STRUCTURE_REF(netbios_acclisttype); /* ibm/netbios.h */
STRUCTURE_REF(dgtype);		/* hes/dgbus.h */
STRUCTURE_REF(mcitype);		/* if/if_mci.h */
STRUCTURE_REF(dcitype);		/* if/if_mci.h */
STRUCTURE_REF(mciregtype);	/* hes/if_mci.h */
STRUCTURE_REF(spyglass_data_t);	/* hes/if_trip.h */
STRUCTURE_REF(phytype);		/* if/fddi.h */
STRUCTURE_REF(fr_pvc_info_type);/* wan/frame_relay.h */
STRUCTURE_REF(fr_svc_info_type);/* wan/frame_relay.h */
STRUCTURE_REF(fr_svc_addr_type);/* if/static_map.h */
STRUCTURE_REF(fr_svc_nli_type);	/* wan/frame_relay.h */
STRUCTURE_REF(fr_idb_struct_type);/* wan/frame_delay.h */
STRUCTURE_REF(fr_svc_struct_type);/* wan/frame_relay.h */
STRUCTURE_REF(dxi_pvc_info_type);/* wan/frame_relay.h */
STRUCTURE_REF(dxi_idb_struct_type);/* wan/frame_delay.h */
STRUCTURE_REF(fr_bootp_stuff);	/* wan/frame_relay.h */
STRUCTURE_REF(smds_pdb_t);	/* wan/smds.h */
STRUCTURE_REF(smds_gleantype);	/* wan/smds.h */
STRUCTURE_REF(smdsaddr);	/* wan/smds.h */
STRUCTURE_REF(mopcon);		/* decnet/mop.h */
STRUCTURE_REF(setup_struct);	/* ui/setup.h */
STRUCTURE_REF(lcitype);		/* x25/x25.h */
STRUCTURE_REF(x25idbtype);	/* x25/x25.h */
STRUCTURE_REF(lapbidbtype);	/* x25/lapb.h */
STRUCTURE_REF(bootinfo_t);	/* os/boot.h */
STRUCTURE_REF(rmon_t);		/* rmon/rmon_task.h */
STRUCTURE_REF(pritype);		/* if/priority.h */
STRUCTURE_REF(plisttype);	/* if/priority.h */
STRUCTURE_REF(fair_queue_head);	/* if/priority.h */
STRUCTURE_REF(atm_db_t);	/* if/atm.h */
STRUCTURE_REF(vc_info_t);	/* if/atm.h */
STRUCTURE_REF(clns_pdbtype);	/* clns/clns_route.h */
STRUCTURE_REF(tcp_encaps);	/* tcp/tcpdriver.h */
STRUCTURE_REF(novpathtype);	/* xns/novfast.h */
STRUCTURE_REF(novcachetype);	/* xns/novfast.h */
STRUCTURE_REF(xnspathtype);	/* xns/xnsfast.h */
STRUCTURE_REF(vinesfastpathtype); /* vines/vinesfast.h */
STRUCTURE_REF(tr_mac_llc_t);	/* if/tring.h */
STRUCTURE_REF(ecshow_t);	/* hes/{ec,envm_p}.h */
STRUCTURE_REF(timeval);		/* os/clock.h */
STRUCTURE_REF(ssetype);		/* hes/sse_private.h */
STRUCTURE_REF(sse_ipinfo);	/* ip/ipfast_sse.h */
STRUCTURE_REF(sse_tbinfo);	/* hes/bridge_sse_private.h */
STRUCTURE_REF(sse_ipxinfo);	/* xns/novfast_sse.h */
STRUCTURE_REF(sse_srbinfo);	/* srt/srb_sse_private.h */
STRUCTURE_REF(sse_ipaccessinfo); /* ip/ipaccess_sse_private.h */
STRUCTURE_REF(sse_clnsinfo);	/* clns/clnsfast_sse_private.h */
STRUCTURE_REF(sse_atalkinfo);	/* atalk/at_fast_sse.h */
STRUCTURE_REF(sse_protoinfo);	/* hes/sse.h */
STRUCTURE_REF(dntype);		/* decnet/decnet.h */
STRUCTURE_REF(dnnet);		/* decnet/decnet.h */
STRUCTURE_REF(echotype);	/* ip/ip.h */
STRUCTURE_REF(tracetype);       /* ip/ip.h */
STRUCTURE_REF(iphostinfotype);	/* ip/ip.h */
STRUCTURE_REF(areatype);	/* iprouting/ospf.h *//* another bad name choice */
STRUCTURE_REF(ospf_pdb_info);
STRUCTURE_REF(list_queue);	/* iprouting/ospf.h *//* YABNC */
STRUCTURE_REF(nbrtype);		/* iprouting/ospf.h *//* YABNC */
STRUCTURE_REF(ospfbackuptype);	/* iprouting/ospf.h */
STRUCTURE_REF(ospf_idb_info);	/* iprouting/ospf.h */
STRUCTURE_REF(lsdbtype);	/* iprouting/ospf_dbase.h */
STRUCTURE_REF(ipinfotype);	/* iprouting/route.h */
STRUCTURE_REF(bmsgtype);	/* iprouting/bgp.h */
STRUCTURE_REF(dual_peertype);	/* iprouting/dual.h */
STRUCTURE_REF(compress_db);	/* compression compress.h */
STRUCTURE_REF(x25map);		/* x25/x25_address.h */
STRUCTURE_REF(clns_adjtype);	/* clns/clns_adj.h */
STRUCTURE_REF(sscopType);       /* atm/sscop_private.h */
STRUCTURE_REF(mapclass_type);   /* if/static_map.h */
STRUCTURE_REF(staticmap_type);  /* if/static_map.h */
STRUCTURE_REF(staticmap_list_type); /* if/static_map.h */
STRUCTURE_REF(maplist_type);    /* if/static_map.h */
STRUCTURE_REF(mapgroup_type);   /* if/static_map.h */
STRUCTURE_REF(fr_params_type);   /* if/static_map.h */
STRUCTURE_REF(standbyidbtype);	/* iprouting/standby.h */
STRUCTURE_REF(nidbtype);	/* xns/novell.h */
STRUCTURE_REF(novell_client);	/* xns/novell.h */
STRUCTURE_REF(novpdbtype);	/* xns/novell.h */
STRUCTURE_REF(novell_sapentry_rdb);	/* xns/novell.h */
STRUCTURE_REF(novrdbtype);	/* xns/novroute.h */
STRUCTURE_REF(apollo_acclisttype); /* xns/apollo.h */
STRUCTURE_REF(TCLSIMsg);	/* cls/clsi_msg.h */
STRUCTURE_REF(lak_t);		/* srt/lack_externs.h */
STRUCTURE_REF(lnxtype);		/* srt/lnx.h */
STRUCTURE_REF(qllctype);	/* srt/qllc.h */
STRUCTURE_REF(qllc_data_t);	/* srt/qllc.h */
STRUCTURE_REF(nciapeertype);    /* cls/ncia.h */
STRUCTURE_REF(ncia_servertype); /* cls/ncia.h */
STRUCTURE_REF(ipaddtype);	/* ip/ip.h (*NOT* ipaddrtype!) */
STRUCTURE_REF(thcdbtype);       /* tcp/tcpcompress.h */
STRUCTURE_REF(fm_receive_t);	/* sbe16/madge/if_madge.h */
STRUCTURE_REF(fm_transmit_t);	/* sbe16/madge/if_madge.h */
STRUCTURE_REF(snmp_signal_info); /* h/ttysrv.h */
STRUCTURE_REF(tbifd_type);	/* tbridge/tbridge_public.h */
STRUCTURE_REF(said_descriptor);	/* tbridge/tbridge_sde.h */
STRUCTURE_REF(smf_entry_type);	/* smf/smf_private.h */
STRUCTURE_REF(isis_ddbtype);	/* clns/isis.h */
STRUCTURE_REF(isis_tree_type);	/* clns/isis.h */
STRUCTURE_REF(isisidbtype);	/* clns/isis.h */
STRUCTURE_REF(isis_lspix_type); /* clns/isis.h */
STRUCTURE_REF(isis_lspdbtype);	/* clns/isis.h */
STRUCTURE_REF(isis_spf_log);	/* clns/isis.h */
STRUCTURE_REF(isis_backuptype); /* clns/isis.h */
STRUCTURE_REF(isis_lsp_linkage);/* clns/isis.h */
STRUCTURE_REF(isis_cleanup_queue); /* clns/isis.h */
STRUCTURE_REF(parser_mode);	/* h/config.h */
STRUCTURE_REF(parser_alias);	/* h/config.h */
STRUCTURE_REF(llctype);		/* llc2/llc2.h */
STRUCTURE_REF(cip_llctype);	/* dspu/cipllc_types.h */
STRUCTURE_REF(cip_linktype);	/* hes/sr_channel.h */
STRUCTURE_REF(arptype);		/* if/arp.h */
STRUCTURE_REF(snmpifptrtype);	/* h/snmp_interface.h */
STRUCTURE_REF(snmpidbtype);	/* h/snmp_interface.h */
STRUCTURE_REF(subiabtype);      /* h/snmp_interface.h*/
STRUCTURE_REF(himib_repeater_info_t); /* les/hub.h */
STRUCTURE_REF(himib_port_info_t); /* les/hub.h */
STRUCTURE_REF(hub_ether_t);     /* les/hub.h */
STRUCTURE_REF(hub_device_t);    /* les/hub.h */
STRUCTURE_REF(hub_t);           /* les/hub.h */
STRUCTURE_REF(hub_access_t);    /* les/hub.h */
STRUCTURE_REF(hub_mgmt_t);      /* les/hub.h */
STRUCTURE_REF(ccbtype);         /* hes/ccb_cmd.h */
STRUCTURE_REF(qa_hwq_t);	/* src-rsp/rsp_qasic.h */
STRUCTURE_REF(qa_acc_t);	/* src-rsp/rsp_qasic.h */
STRUCTURE_REF(slottype);	/* hes/slot.h */
STRUCTURE_REF(lovenotetype);	/* src-rsp/lovenote.h */
STRUCTURE_REF(bsc_t);           /* ibm/bsc.h */
STRUCTURE_REF(lane_info_t);     /* lane/lane.h */
STRUCTURE_REF(lec_info_t);  	/* lane/lec_private.h */
STRUCTURE_REF(lsv_client_t);  	/* lane/lsv_private.h */
STRUCTURE_REF(pim_nbrtype);	/* ipmulticast/pim.h */
STRUCTURE_REF(ratetype);	/* ipmulticast/mroute.h */
STRUCTURE_REF(ipc_message);     /* ipc/ipc.h */
STRUCTURE_REF(rshcontext);	/* tcp/rsh.c */
STRUCTURE_REF(pgen_context);	/* pgen/pgen_private.h */
STRUCTURE_REF(screen_type);	/* tn3270/tn3270.h */
STRUCTURE_REF(termcap_globals_struct); /* tn3270/termcapglo.h */
STRUCTURE_REF(flash_t);		/* dev/flash_dvr_spec.h */
STRUCTURE_REF(fsstat_t);	/* dev/flash_dvr_spec.h */
STRUCTURE_REF(fs_open_instance_t); /* les/flash_fs_les.h */
STRUCTURE_REF(considbtype);	/* x25/x25.h */
STRUCTURE_REF(pcbtype);		/* x25/x25.h */
STRUCTURE_REF(x25routeentrytype); /* x25/x25_switch.h */
STRUCTURE_REF(sap_registry_t);  /* h/ieee.h */ 
STRUCTURE_REF(rbTree);		/* util/tree.h */
STRUCTURE_REF(lcb_t);		/* bstun/bsc_lcb.h */
STRUCTURE_REF(userstruct);	/* h/aaa.h */
STRUCTURE_REF(authenstruct);	/* h/aaa.h */
STRUCTURE_REF(authorstruct);	/* h/aaa.h */
STRUCTURE_REF(acctstruct);	/* h/aaa.h */
STRUCTURE_REF(acctinfo);	/* h/aaa.h */
STRUCTURE_REF(per_user_info);   /* h/aaa.h */
STRUCTURE_REF(des_state_type);  /* crypto/crypto_des_public.h */
STRUCTURE_REF(async_member_info); /* ts/if_async.h */
STRUCTURE_REF(isl_vlan_descriptor); /* vlan/vlan.h */
STRUCTURE_REF(message);         /* h/logger.h */
STRUCTURE_REF(httpstate);	/* tcp/http.h */
STRUCTURE_REF(pas_cbtype);	/* pas/if_pas.h */
STRUCTURE_REF(XID3type);        /* ibm/sna_util.h */
STRUCTURE_REF(vip4r_data_t);      /* hes/if_vip.h */
STRUCTURE_REF(srbroute_t);      /* if/rif.h */
STRUCTURE_REF(netbios_sess_entry); /* ibm/netbios_as.h */
STRUCTURE_REF(netbios_lan_sess_entry); /* ibm/netbios_as.h */
STRUCTURE_REF(v120_info_type);
STRUCTURE_REF(ippeerinfotype);	/* ip/ip_addrpool.h */
STRUCTURE_REF(ip_addresspool_type); /* ip/ip_addrpool.h */
STRUCTURE_REF(itemdesc);	/* util/itemlist.h */
STRUCTURE_REF(itemlist);	/* util/itemlist.h */
STRUCTURE_REF(nhrpinfotype);	/* wan/nhrp.h */
STRUCTURE_REF(list_header);     /* os/list.h */
STRUCTURE_REF(rsvp_info_type);	/* rsvp/rsvp_database.h */
STRUCTURE_REF(red_parameters);	/* parameters for RED */
STRUCTURE_REF(igmpidbtype);	/* ipmulticast/igmp.h */
STRUCTURE_REF(ntp_refclock);	/* ntp/ntp_refclock.h */
STRUCTURE_REF(calendar_t);      /* hes/calendar_p.h */
STRUCTURE_REF(acl_headertype);	/* h/access.h */
STRUCTURE_REF(vaccess_info_block); /* if/if_vtemplate.h */
STRUCTURE_REF(traffic_shape_t);	/* if/traffic_shape.h */
STRUCTURE_REF(password_struct);	/* h/login.h */
STRUCTURE_REF(cpp_info_type);	/* wan/cpp.h */
STRUCTURE_REF(cpp_group_type);	/* wan/cpp.h */
STRUCTURE_REF(modem_mgmt_info_t); /* les/modem_mgmt.h */
STRUCTURE_REF(ser_autodetect_info); /* wan/serial_autodetect.h */

/*
 * 	Can't use STRUCTURE_REF for transition because
 * it is a constant.
 */
typedef const struct transition_ transition;	/* h/config.h */

/*
 * These don't follow cisco convention because they're imported
 * from vendor code.
 */
typedef struct radix_node	rntype;
typedef struct radix_node_head	rnhtype;

/*
 * Define major system types
 */

/*
 * Type Definitions  -- note that these are really compiler specific 
 */

#if !defined(_SIZE_T) && !defined(_GCC_SIZE_T)
#define _SIZE_T

#ifdef __GNUC__
#define _GCC_SIZE_T
typedef __typeof__ (sizeof (0)) size_t;
#else
typedef unsigned int size_t;
#endif /* __GNUC__ */

#endif /* _SIZE_T */

typedef long long longlong;
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned int uint;
typedef unsigned long ulong;
typedef unsigned long long ulonglong;

/*
 * Size specific defines
 */
typedef signed char        int8;
typedef signed short       int16;
typedef signed long        int32;
typedef signed long long   int64;
typedef unsigned char      uint8;
typedef unsigned short     uint16;
typedef unsigned long      uint32;
typedef unsigned long long uint64;

typedef ulong ipaddrtype;

typedef char *va_list;		/* h/stdarg.h */
typedef int boolean;
typedef unsigned char tinybool;
typedef void forktype;
typedef int pid_t;

typedef enum {
    FUZ_FALSE,			/* definitely false */
    FUZ_TRUE,			/* definitely true */
    FUZ_NOTSURE,		/* unsure of the truth or falsity */
} fuzzy;

typedef void process;
typedef process (process_t)(void);
typedef forktype (forkproc)(void /* void */);
typedef boolean (blockproc)(queuetype *);
typedef void (void_dontcare)(void *, ...);
typedef boolean (boolean_dontcare)(void *, ...);
typedef void (*timer_func)(void *, void *, void *);
#if defined(CSC2) || defined(RP1)
typedef boolean (*fastswitch_type)(hwidbtype *);
#else
typedef boolean (*fastswitch_type)(paktype *);
#endif /* CSC2 || RP1 */

/*
 * Force the definition of the NULL pointer
 */
#ifdef NULL
#undef NULL
#endif /* NULL */
#define NULL ((void *) 0)

/*
 * Define FALSE and TRUE
 */

#define TRUE 1
#define FALSE 0

/*
 * Define the PACKED macro
 */
#if defined(__mips) || defined(__mc68000)
#define PACKED(item) item __attribute__ ((packed))
#else
#define PACKED(item) item
#endif

/*
 * Define the ALIGN macros.
 * These are used to align elements of a struct or a variable
 * in stand-alone allocation.
 *
 * The MIPS chipset is rather finicky about its alignment where performance
 * is concerned. Consult a MIPS cache wizard if you don't know what the
 * results of changing the default alignment generated by the compiler.
 *
 * ALIGN_2	will align on a word boundary
 * ALIGN_4	will align on a longword boundary
 * ALIGN_8	will align on a quadword boundary
 * ALIGN_16	will align on a octaword boundary
 * ALIGN_32	will align on a 32-byte (16-word) boundary
 */
#if defined(__mips) || defined(__mc68000)
#define ALIGN_2(item) item __attribute__ ((aligned (2)))
#define ALIGN_4(item) item __attribute__ ((aligned (4)))
#define ALIGN_8(item) item __attribute__ ((aligned (8)))
#define ALIGN_16(item) item __attribute__ ((aligned (16)))
#define ALIGN_32(item) item __attribute__ ((aligned (32)))
#else
#define ALIGN_2(item) item
#define ALIGN_4(item) item
#define ALIGN_8(item) item
#define ALIGN_16(item) item
#define ALIGN_32(item) item
#endif

/*
 * Define the PRINTF_FUNCTION macro
 */
#define PRINTF_FUNCTION(func, format_string, args) func \
    __attribute__ ((format (printf, (format_string), (args))))


/*
 * charlong data structure
 * Used for manipulating longs, shorts, and bytes.
 */

typedef struct charlong_ {
    union {
	uchar	byte[4];
	ushort	sword[2];
	ulong	lword;
    } d;
} charlong;

/*
 * charlonglong data structure
 * Used for manipulating longs, shorts, and bytes.
 */

typedef struct charlonglong_ {
    union {
	uchar		byte[8];
	ushort		sword[4];
	ulong		lword[2];
	ulonglong	llword;
    } d;
} charlonglong;

/*
 * Define a semaphore.  This should only be locked with the
 * lock_semaphore routine to guarantee an atomic lock.
 */
typedef volatile ulong semaphore;


/*
 * sys_timestamp--a system timestamp
 */
typedef longlong sys_deltatime;
typedef struct sys_timestamp_ {
    union {
	ulonglong value;
	struct {
	    ulong high;
	    ulong low;
	} p;
    } u;
} sys_timestamp;
#define timer_no_time (const sys_timestamp) { { 0 } }
#define TS_VALUE(x) ((x).u.value)
#define TS_VALUE32(x) ((x).u.p.low)

/*
 * Managed timer structure
 *
 * Note--these fields should remain opaque!  Do not refer to them directly!
 * Use wrappers defined in mgd_timers.h!
 *
 * Note that mt_down and mt_context are in a union because they are
 * mutually exclusive;  only leaf timers have a context, and only
 * parent timers have a down pointer.
 *
 * All timers have a pointer to their parent (mt_head) which stays there
 * always.  All active timers are dually linked in expiration order with
 * the other children of their common parent (mt_next, mt_prev).  All
 * parents of active timers have a down pointer (mt_down) to the first
 * active child (head of the list of active children).  The mt_prev entry
 * for the head of a chain points to the tail of the chain.  The mt_next
 * entry for the tail of a chain is NULL.
 *
 * The mt_intrpt_env field contains two bits pertaining to the interrupt
 * environment.  If the DIRECT bit is set, this timer has been initialized
 * to note that it will be modified in an interrupt environment, or else
 * this timer is a parent of one that has been so initialized.  If the
 * INHERITED bit is set, this timer is a child of a timer that has the
 * DIRECT attribute.  If either bit is set, interrupts must be excluded
 * while manipulating the timer.
 *
 * The mt_fence attribute is set so that a timer tree can be descended to
 * a fence point (as opposed to going all the way down to a leaf).  This
 * is useful for situations where subtrees are to be delinked;  the fence
 * can be set in the root of the subtree to be delinked.
 *
 * Managed timers can carry additional context information by appending
 * it onto the context block.  The MGD_TIMER_EXTENDED macro should be
 * used to allocate the space.
 */
typedef struct mgd_timer_ {
    struct mgd_timer_ *mt_next;		/* Next entry in this chain */
    struct mgd_timer_ *mt_prev;		/* Previous entry in chain */
    struct mgd_timer_ *mt_head;		/* Master timer for this chain */
    union {
	struct mgd_timer_ *mt_down;	/* Child chain of this timer */
	void *mt_context;		/* Opaque context */
    } mt_union;
    sys_timestamp mt_exptime;		/* Expiration time */
    ushort mt_type;			/* Opaque type */
    uchar mt_initialized;		/* Set to magic # if initialized */
    uchar mt_fence:1;			/* True if a fence is set here */
    uchar mt_leaf:1;			/* True if a leaf timer */
    uchar mt_intrpt_env:2;		/* MT_INTRPT_xx */
    void *mt_additional_context[0];	/* Additional context information */
} mgd_timer;

#define MT_INTRPT_EXCLUDE_DIRECT 0x1	/* Interrupts excluded directly */
#define MT_INTRPT_EXCLUDE_INHERITED 0x2	/* Interrupt exclusion inherited */
#define MT_INITIALIZED_MAGIC 0x42	/* Magic number for mt_initialized */

#define MGD_TIMER_EXTENDED(fieldname, size) \
    mgd_timer fieldname;\
    void *_##fieldname##_extension[size]

/* IPC specific types - needed for sys_registry.reg */
typedef enum ipc_error_code_ {
    IPC_OK,
    IPC_INTERNAL,
    IPC_NO_SUCH_SEAT,
    IPC_SEAT_IN_USE,
    IPC_NO_MEMORY,
    IPC_PORT_IN_USE,
    IPC_TIMEOUT,
    IPC_NO_SUCH_PORT,
    IPC_PROCESS_LEVEL_ONLY,
} ipc_error_code; 

typedef void (*ipc_callback)(ipc_message *, void *, ipc_error_code);
typedef uint ipc_port_id;
typedef ushort ipc_port;
typedef uint ipc_seat;
typedef ushort ipc_sequence;
typedef ushort ipc_message_type;
typedef ushort ipc_size;
typedef ushort ipc_flags;

enum HEADTAIL {
    HEAD,
    TAIL
};

#define	MIN(a,b)	(((a) < (b)) ? (a) : (b))
#define	MAX(a,b)	(((a) > (b)) ? (a) : (b))

#endif __TYPES_H__

