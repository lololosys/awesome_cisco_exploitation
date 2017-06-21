/* $Id: snapshot.c,v 3.5.18.2 1996/08/28 13:22:46 thille Exp $
 * $Source: /release/112/cvs/Xsys/wan/snapshot.c,v $
 *------------------------------------------------------------------
 * Support for Snapshot routing
 *
 * April 1994, Joel P. Bion
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Support for Snapshot routes, which allow for a large reduction
 * of routing updates down an interface, at the cost of changes propogating
 * far more slowly to other ends of the link. For stub sites, this beats the
 * problem of static routes configured down the link to avoid routing
 * updates or to support dial on demand.
 *------------------------------------------------------------------
 * $Log: snapshot.c,v $
 * Revision 3.5.18.2  1996/08/28  13:22:46  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.18.1  1996/03/18  22:48:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  02:12:43  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.26.2  1996/03/07  11:08:28  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:53:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  20:14:54  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1996/02/01  06:12:38  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1995/11/20  17:26:46  lbustini
 * CSCdi33745:  Snapshot not working when the link is up
 * bzero addrtype before filling in the fields.
 *
 * Revision 3.2  1995/11/17  18:04:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:12  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/24  23:32:09  hampton
 * Remove the old inline s_kill_self_with_no_analysis by pushing it into
 * the routines that call it.  [CSCdi42780]
 *
 * Revision 2.4  1995/06/28  09:32:59  smackie
 * Repair widespread subsystem header braindamage. (CSCdi23568)
 *
 *   o Fix subsystem entry points to be foo(subsystype *) not foo(void)
 *   o Repair nonsensical and redundant subsystem sequences
 *   o Use NULL where no property is required
 *
 * Revision 2.3  1995/06/20  20:38:32  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.2  1995/06/09  13:25:27  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 23:19:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Snapshot routing documentation and operation notes
 * --------------------------------------------------
 * 
 * Snapshot routing provides easy solutions to two common problems:
 * 
 *   1) The need to configure static routes for Dial on Demand Routing (DDR)
 *      interfaces.
 * 
 *   2) The overhead of periodic updates for routing protocols to remote branch
 *      offices over dedicated serial lines.
 * 
 * When Snapshot routing is configured on an interface, it first allowes
 * normal routing updates to be sent across the interface for a short
 * period of time. After this user configured period of activity, the
 * routing updates are suspended, and the routes learned on the Snapshot
 * interface are 'frozen' in place, putting the interface into a 'frozen
 * period', again user configurable. During this 'frozen' time, changes
 * in network topology are typically not transmitted across the Snapshot
 * interface, although some network protocols provide the capability to
 * transmit changes.
 * 
 * Snapshot routing is not a network protocol, and no packets are
 * exchanged on an interface to support it. As such, it is fully
 * compatible with session layer protocols, such as IPXWAN.
 * 
 * The factors to consider in choosing to enable Snapshot routing to an
 * interface are:
 * 
 *   1) The importance of obtaining quick notification of changes in the
 *      network topology at a remote site.
 *   2) Whether or not you are using Dial on Demand routing from a remote
 *      site to reach a central site or using a dedicated serial link.
 * 
 * If you are using a dedicated serial link, and wish to have
 * up-to-the-minute topology information at a remote site, then Snapshot
 * routing is probably not appropriate for you. If, however, the network
 * topology changes less frequently, or if full connectivity to all
 * locations is not important to you, then Snapshot routing (with
 * possibly, a few static routes to 'key' remote locations that should
 * always be available) will probably meet your needs well.
 * 
 * If you are using dial on demand routing (DDR), then Snapshot routing
 * is almost certainly an advantage. Before Snapshot routing, all
 * 'desired' destinations needed to be configured with static routes.
 * With Snapshot routing, it is possible to learn the set of remote
 * routes dynamically, and then keep them available while the link is
 * down, avoiding the configuration of any static routes. Again, if there
 * are 'crucial' locations that must always be in the table, it may be
 * best to configure them statically, to ensure constant availability, in
 * case the desired location was temporarily down when the Snapshot route
 * sharing activity was taking place.
 * 
 * Snapshot routing is configured on a 'client/server' basis. One router
 * on the interface is chosen as the 'client' router, and it determines
 * active route-exchange and 'sleeping' times.
 * 
 * The protocols currently supporting Snapshot routing are:
 * 	AppleTalk: RTMP
 * 	IP: RIP and IGRP
 * 	IPX: RIP and SAP
 * 	VINES: RTP
 * 
 * Configuring Snapshot Routing
 * ----------------------------
 * 
 * Configuring Snapshot routing for dedicated serial lines is quite
 * simple. Only two interface commands exist, and only one is used on
 * any interface.
 * 
 *  [no] snapshot client  <active-period-length> <quiet-period-length> [opt]
 *  [no] snapshot server <active-period-length> [dialer]
 *
 * One of these two commands must be chosen for each Snapshot interface; they
 * set the interface up as either a Snapshot client or server side. Snapshot
 * clients 'control' the servers by sending updates to them. When the server
 * starts receiving updates, it knows to start sending its own updates to
 * the client. When each side starts receiving updates from the other, for
 * a given network protocol (from a given network next hop), all routes
 * in that protocol learned on that next hop start to age. Therefore, any
 * routes no longer advertised by the remote side will age out at a normal
 * rate, once the 'update exchange period' (called the 'active period') has
 * begun.
 *
 * <active-period-length> is the amount of time, in minutes, routes
 * should be exchanged between the routers.
 * 
 * <quiet-period-length> is the amount of time, in minutes, that routes
 * are retained and 'frozen' between active periods.
 * 
 * [opt] is any of the following sequences:
 * 
 *  	suppress-statechange-updates
 * 	dialer
 * 	suppress-statechange-updates dialer
 * 
 * 'suppress-statechange-updates' controls an option whereby every time an
 * interface is brought 'up', the active period is started. This allows
 * for a new exchange of routes each time the line protocol goes from
 * 'down' to 'up' or, on DDR links, each time a connection is established
 * to a remote router. This incurrs extra routing protocol overhead, but
 * allows (especially in the DDR case) to have one, fresh set, of routing
 * updates be exchanged each time the line is brought up.
 * 
 * 'dialer' informs the Snapshot code that the special DDR support for
 * Snapshot routes should be enabled on this interface.
 * 
 *
 * Using Snapshot Routing on DDR interfaces
 * ----------------------------------------
 * 
 * Snapshot routing is integrated with the Dial on Demand Routing (DDR)
 * support in the router.  With this integration, a single 'client
 * router' can call multiple 'server routers,' and use the full DDR
 * support of the router to reach those server routers.
 * 
 * To configure the DDR/Snapshot integration facility, the 'dialer'
 * keyword, as shown above, is used on the 'client' router's 'snapshot'
 * command.
 * 
 * Using the 'dialer map' command, multiple 'dialer maps' are given for
 * SNAPSHOT to dial into. For example, if the phone numbers 2134, 5423,
 * and 2024 are remote sites, the following command sequence would ensure
 * the Snapshot routing code dials all three to obtain routes:
 * 
 * 	interface serial 0
 *      dialer rotary-group 1
 *
 *      interface dialer 1
 *      encapsulation ppp
 *      ppp authentication chap
 * 	snapshot client 5 360
 * 	dialer map snapshot 1 name remote-site-1 2134
 * 	dialer map snapshot 2 name remote-site-2 5423
 * 	dialer map snapshot 3 name remote-site-3 2024
 *      ...
 *      user remote-site-1 ...
 *      user remote-site-2 ...
 *      user remote-site-3 ... 
 * 
 * ...note the first parameter, which is a sequence number -- this number
 * must be unique for each 'dialer map snapshot' command.
 *
 * IT IS REQUIRED THAT DIALER ROTARY GROUPS BE USED WITH SNAPSHOT
 * ROUTING -- THESE WILL NOT GO DOWN WHEN THE DIALER DROPS, MAKING THE
 * ROUTING BEHAVIOR HAVE THE DESIRED INTENTION...
 *
 * WHY PPP AND CHAP AUTHENTICATION IS RECOMMENDED:
 * One of Snapshot's major features is that the protocol incurrs no extra
 * packet overhead on the link to support the use of the protocol. Furthermore,
 * another (optional) feature of Snapshot is that every time an interface is
 * brought from a 'down' state to an 'up' state, the active period will be
 * reentered. However, if the following occurs:
 *		1) Server side is allowed to dial client side
 *              2) Client side dials more than one entity
 * ...there will be no way for the client side to know which server has
 * dialed it. With CHAP authentication, that information is provided, so
 * the client side can 'activate' the connection with the appropriate
 * server, and start having updates exchanged. (Remember, the server will
 * only send updates when the client requests it, so the client must be
 * the one starting the updates.)
 *
 * Note that if there is only one site that is dialed by a server, than
 * both sides can simply configure the 'dialer-string' on the interfaces,
 * and achieve the same effect, and the use of PPP with or without CHAP
 * authentication is optional.
 *
 *  Snapshot EXEC commands
 *  ======================
 * The following are the Snapshot EXEC commands:
 *
 *  debug snapshot
 *   - Turns on Snapshot debugging. When enabled, will show Snapshot state
 *     transitions, configuration changes, and when protocols start to age
 *     routes because Snapshot receives a routing update for that protocol.
 *
 *  clear snapshot quiet-time <interface>
 *   - For Snapshot CLIENT interfaces, will 'clear' the quiet time, and place
 *     the router in a state to re-enter active time within two minutes.
 *     (The hold period is maintained to ensure a quiet time of at least
 *     two minutes between active times).
 *
 *  show snapshot <interface>
 *   - Show the Snapshot parameters associated with all interfaces, or if
 *     one is specified on the command line, just the interface specified.
 *
 *  Sample output, for a client side:
 *	Router> show snapshot serial 0
 * 	 Serial1 is up, line protocol is up, snapshot client up
 *	  Options: dialer support
 *	  Length of active period:          3 minutes 
 *	  Length of quiet period:           10 minutes 
 *	  Length of retry period:           10 minutes
 *	   For dialer address 240
 *	    Current state: active, remaining time: 3 minutes
 *                         exchanging time: 2 minutes
 *          Connected dialer interfaces:
 *           Serial1 
 *          Updates received this cycle: ip, ipx, appletalk
 *	   For dialer address 1
 *	    Current state: client quiet, time until next activation: 7 minutes
 *	
 *  Fields:
 *	First line:
 *	  Standard interface up stats, plus Snapshot's last examination of
 *	  interace (not really relevent; useful for debugging)
 *	Second Line:
 *	  Options, as configured on 'snapshot' command line:
 *		dialer support: Configured with 'dialer' keyword
 *		stay asleep on carrier up: Configured with
 *					    'suppress-statechange-update'
 *					   keyword
 *	Third line: Time, in minutes, each active period lasts.
 *	Fourth line: Time, in minutes, each quiet period
 *					lasts
 *	Fifth line: Time, in minutes, a 'retry' quiet
 *				       period lasts if no routing updates
 *                                     were received on an interface.
 *    Then...
 *	For client 'dialer' interfaces, each 'dialer map snapshot' address is
 *	shown, followed by a few lines describing the state of that entry. For
 *	servers, and for non-dialer interfaces, only one block is shown.
 *	In each block, the:
 *	  First line: Shows the current state, and the time remaining in it
 *	    Queues are:
 *	 	1) active -- updates being sent out interface. For active
 *			     state, the number of minutes updates have
 *                           been exchanged is shown.
 * 		2) client quiet -- updates not being sent, sleeping as client
 *      	3) server post active -- updates not being sent, if update
 *				  received from client, it is processed, but
 *				  does not open an active window. This allows
 *				  a 'quiet' time for resynchronization of full
 *				  active window periods between the client and
 *				  server.
 *              4) client post active -- Updates not sent, any down->up of
 *				  interface will indicate updates should
 *				  be sent a few minutes from now. Will say
 *				  [->quiet] or [->active] to indicate
 *				  if a down->up transition occured, indicating
 *				  if the system moves to quiet or back to
 *				  active.
 * 		5) creation/deletion -- Temporary holding queue for new
 *				  Snapshot interfaces, and ones being deleted.
 *
 *	  When in the active state, a second line is displayed which shows
 *	  for which Snapshot-capable protocols updates have been received from
 *        the remote side during this active interval. Furthermore, for
 *        dialer interfaces, the 'real' interfaces associated currently
 *	  connected to this Snapshot dialer map address are listed.
 *	 
 *  Sample output, for a server side:
 *	Router> show snapshot serial 0
 * 		Serial0 is up, line protocol is up, snapshot server up 
 *   	          Options: dialer support
 *		  Length of active period:          3 minutes 
 *		   For novell protocol address: 9999.0000.0c01.5800 
 *		    Current state: active, remaining time: 2 minutes
 *		   For ip protocol address: 131.108.220.84 
 *		    Current state: active, remaining time: 2 minutes 
 *		   For vines protocol address: 30015800:0001 
 *		    Current state: active, remaining time: 2 minutes 
 *		    Updates received this cycle: vines 
 *		   For appletalk protocol address: 9999.124 
 *		    Current state: active, remaining time: 3 minutes 
 *
 *      First three lines: The same as in the client side.
 *	Remaining lines: For each protocol and source address sending us
 *	routing updates on the interface, shown is the address sending the
 *	updates, and the length of time left that the router is 'actively'
 *      sending updates to that address, and the protocol of the address.
 * 
 */

/*
 * SNAPSHOT DATA STRUCTURES AND ALGORITHMS...
 * ---------------------------------------------------------------------------
 * 
 * The Snapshot per-software-IDB data structure is dynamically allocated
 * as needed, and is NOT listed in detail in the IDB; rather a single pointer
 * to a structed called the 'snapshot_interface_info' (called SII in the
 * discussions below) is pointed to by the IDB. Snapshot also maintains a
 * separate queue of these SIIs, to speed processing time. 
 *
 * This setup provides for complete independence of the Snapshot structure from
 * the IDB structure, and allows a change in the Snapshot code to have no
 * impact on the IDB whatsoever (avoiding huge recompiles.)
 * 
 * The SII structure contains the 'global' elements configured by users on
 * the inteface: Active and quiet period lengths, flags indicating if the
 * 'dialer' or 'suppress-statechange-updates' flags are set, and information
 * on whether the interface is a Snapshot client or server.
 * 
 * Also, this structure contains a pointer to the head of a queue of
 * 'snapshot_activity_block' structures (called SABs in the discussions below.)
 * For clients, only one exists per SII, unless dialer support is enabled, and
 * in these cases, multiple SABs exist per SII: one per 'dialer map snapshot'
 * command given on the interface. For servers, an SAB is created, on a
 * per-protocal/per-source-node basis, for each source node sending route
 * advertisements to this interface. 
 * 
 * 	snapshot_interfaces-->(queue of snapshot_interface_info blocks)
 * 				a: idb ptr
 *				b: unique index number for MIB access.
 * 				c: active-period
 * 				quiet-period
 *				d: current number of SABs (for MIB access)
 * 				e: flags - SERVER, DIALER,
 *                                      SUPPRESS_STATECHANGE_UPDATES, IDB_UP
 * 				f: pointer-to SABs...
 * 				   (queue of snapshot_activity_blocks)
 * 					a: snapshot_runtime_queue_next
 * 					b: snapshot_runtime_queue_prev
 *                                      c: index number (for MIB access)
 * 					d: runtime_queue this element is in
 * 					e: pointer_to_parent_sii_block
 *					f: number of minutes routes exchanged
 * 					g: dialer map address number
 * 					h: time_to_stay_in_this_runtime_queue
 * 					i: flags of protocols_starting_to_age
 *					j: protocol address (if server) of
 *					   source host this is related to.
 *					k: list of Dialer HWIDBs attached
 *                                         to destination pointed to by dialer
 *					   map statement corresponding to this
 *					   SAB.
 *
 * Each 'snapshot-activity-block' is on one of the following runtime queues,
 * which indicate the current status of this 'activity block':
 * 	1) active -- updates being sent out interface
 * 	2) client_quiet --  updates not being sent, sleeping as client,
 *			    waiting for the next active period.
 *      3) server_post_active -- updates not being sent, if packet received
 *                               from client, it is processed, but does not
 *                               cause an 'active state' to be entered.
 *	4) client_post_active_next_quiet - updates not being sent, and
 *					   if any external event wants
 *	--BOTH STATES 4 AND		   to immediately make the state
 *      --5 ARE OF SHORT DURATION	   'active', that is not done
 *      --AND ARE MEANT AS A TRANSITION    immediately, but rather the
 *      --STATE FROM CLIENT ACTIVE TO      short 'must remain quiet' period
 *	--QUIET.		           is kept (2 minutes) before
 *                                         entering active. If there is
 *                                         no request for the active state
 *                                         after the two minutes, the
 *                                         client_quiet state is entered.
 *      5) client_post_active_next_active - as (4), above, but indicates a
 *                                          request to enter the active state
 *                                          has been recieved, so the active
 *                                          state is reentered when this
 *                                          state times out.
 * 	6) no_queue -- Holding queue for new Snapshot interfaces, and ones
 * 			being deleted.
 * 
 * The process 'snapshot_tasker' decrements the time-to-remain-in-queue
 * for each of the snapshot-activity-blocks, and if time runs out, the
 * following happens:
 * 	Current state/queue
 * 	-------------------
 * 	Client Quiet	Move to 'active' queue, reset timer to active-length
 * 
 * 	Active		Move to 'client post active quiet' or 'server post
 *                      active'  queue, as appropriate.
 *      Server Post Active
 *			Destroy activity block (servers have none by default).
 *	Client Post Active Next Quiet
 *			Move to 'quiet' queue.  Set, dynamiccally, the  
 *			quiet time based on receipt of any update packets for
 * 			desired protocols: If any received, use normal
 * 			quiet time. If none received, use retry interval.
 *			Also, if a full three minutes of routing update
 *			time had not occured, also use retry interval.
 *	Client Post Active Next Active
 *			Move to 'active' queue, reset timer to active-length
 *			
 *	
 * 
 * If snapshot_tasker finds there are no Snapshot interfaces, it kills itself
 * to free up a process slot. The configuration code will restart it if
 * the user ever configures Snapshot again.
 *
 * Since snapshot_tasker currently runs once a minute, it is possible to
 * consider it for placement on the 'per-minute task list' and not make it
 * a separate process. However, this was not done to ensure Snapshot did
 * not become 'one of many' -- the once a minute timing of this task is
 * important.
 *
 * A major portion of the Snapshot code is devoted to correctly cleaning
 * up when a user either deletes an IDB, or stops Snapshot routing on
 * an interface, or changes the parameters.
 *
 * When Snapshot is first configured, it starts out on the queue of
 * CLIENT_POST_ACTIVE_NEXT_ACTIVE on the client side, allowing a rebooted
 * router to live through any remaining POST_ACTIVE_SERVER time on its peers.
 * Also, the dialer code is called to get all 'dialer map' entries that may
 * already be configured for the interface, and build SABs for them.
 *
 * While an SAB is active, as a client with dialer support, each
 * minute it asks the dialer code to reinitiate a connection to the destination
 * string. (of course, if it is already connected to the address, the dialer
 * code will do nothing but reset the timers.) 
 *
 * Both client and servers with dialer support will ask the dialer code,
 * once a *second* to reset the dialer interface idle timers, to ensure the
 * interface stays up for a full 'active' period.
 *
 * When a 'Snapshot capable protocol' receives an update packet on a idb,
 * it calls the Snapshot code to let it know this happened. If this router is
 * a server, and it has not starting aging from this host (no activity block
 * for it), it creates an activity block, and places the source address of the
 * host connection in it.
 * 
 * If an interface goes down, all SABs on the idb are NOT moved to any of the
 * 'post active' queues - this is because this can cause a thrashing of
 * quiet/active on a flapping link.
 *
 * If an interface goes up, all SABs are moved to the active queue, unless the
 * user has configured 'suppress-statechange-updates' in which case the event
 * is ignored.
 *
 * An overriding goal of Snapshot's dialer support is that it will:
 *
 *	a) Make sure any interfaces that should be kept up stay up, indepedent
 *         of any 'dialer idle-timeout' being hit. Snapshot has greater
 *         priority.
 *  and b) Make sure that it doesn't get in the way of 'dialer idle-timeout'
 *         shutting down lines that Snapshot doesn't care about.
 *
 *  On server sides, Snapshot decides to let the 'client' decide when to drop
 *  the line. Therefore, as soon an ANY interface associated with a dialer
 *  interface comes online (is connected or is connected to), the Snapshot
 *  server code will force that line to remain up for active_period + 2
 *  'extra' minutes to allow any client that MAY be connecting in to have the
 *  line up for a full active period. If the client doesn't it want it, the
 *  client side will drop the line, but the server side won't do it first.
 *
 *  On the client side, Snapshot is informed by dialer of what interfaces
 *  are currently connected to destinations specified by each of the SABs.
 *  While a Snapshot client is active, it will not allow any of these to
 *  idle out. However, Snapshot keeps track of any changes to this list of
 *  interfaces for an SAB, so if any DO change their destinations, Snapshot
 *  no longer bothers trying to keep them up and non-idle (busy, I guess...).
 *
 *  Finally, it is very important that Snapshot control the activation of
 *  the dialer interface by update packets when it is configured, so Snapshot
 *  will have a 'bit' set in every update packet that tells the dialer code
 *  not to bother trying to bring the link up for this packet -- Snapshot will
 *  decide when the dialer interface can be enabled on behalf of update
 *  packets! Also, the Dialer interface will ask Snapshot, for any update
 *  packets being sent out Snapshot interfaces (known already because they
 *  were classified as 'uninteresting') whether or not they should be sent
 *  out a particular hwidb. Snapshot will look to see if the hwidb is attached
 *  to any 'active' SABs before allowing the frame to be sent.
 *
 * ---------------------------------------------------------------------------
 * ROUTING PROTOCOL SUPPORT OF SNAPSHOT ROUTING
 *
 * Making a protocol support Snapshot routing is very simple:
 *
 * 1) Every time the protocol is about to send an update out an interface,
 *    (the actual PACKET of the update, before calling the protocol's send
 *    routine), call:
 *	reg_invoke_snapshot_update_denied(output_idb_for_packet)
 *    If it returns TRUE, don't send the packet out the interface. Of course,
 *    an optimal implementation doesn't build the packet in the first place.
 *    Furthermore, just before commiting the 'sending' of an update packet
 *    out an interface, call:
 *      reg_invoke_snapshot_notice_update_pak(paket, output_idb_for_packet)
 *    This will allow Snapshot to set flags in the packet the Dialer code
 *    must see.
 *
 * 2) Every time a route, or structure required of a route that ages on its
 *    own, has its age reset to 'now' (NOT just 'getting older'), call:
 *	reg_invoke_snapshot_aging_denied(idb of location of entity)
 *    If it returns TRUE, set a 'bit' in this structure which indicates aging
 *    should not occur. It is easiest to make this effort a single routine
 *    that both resets the age of your entity, and does this check to turn
 *    off aging at the same time. This avoids logic errors by making sure
 *    everything that resets an age does the Snapshot no-aging check.
 *
 * 3) Do NOT age any entities that have this 'bit' set, of course.
 *
 * 4) Call on subsystem startup:
 *	reg_add_snapshot_start_aging_routes(routine, "routine");
 *    Where, 'routine' is a routine that has the following parameters:
 *	  routine(idbtype *idb, paktype *pak);
 *
 *	a) Turn off the 'no aging bit' in entities associated with this idb,
 *         first verifying if the route was learned from the source host
 *         in the packet passed into this routine (if the packet is not NULL;
 *         if the packet is NULL, *all* routes learned via idb should start
 *         aging. Typically this occurs when a user de-configures Snapshot.)
 * 	b) Reset the age of these entities to 'zero' (i.e., last update 'now')
 *	c) Not do either (a) or (b) if the route is 'poisoned' already.
 *      d) Won't do (a) or (b) if the route is not currently marked 'NO_AGE'.
 *      e) WON'T DO ANYTHING IF THE PROTOCOL HAS NOT BEEN ENABLED BY THE USER;
 *         SUCH AS REFERENCE DATA STRUCTURES THAT HAVE NOT YET BEEN MALLOCED!
 *
 * 5) Whenever an update packet is received on an interface, call:
 *      reg_invoke_snapshot_update_received(pak->if_input, pak, protocol_addr);
 *    to let the Snapshot code know an update was received on an interface.
 *    Protocol_addr is an 'addrtype' address block which contains the network
 *    level source address of the packet. MAKE SURE YOU 'memset zero' THE
 *    protocol_addr BLOCK BEFORE YOU FILL IN ANY OF ITS FIELDS, OR ELSE
 *    Snapshot WILL NOT KNOW HOW TO COMPARE THIS BLOCK TO OTHERS THAT IT
 *    HAS!
 *
 * 6) NULL UPDATES should be ALLOWED on Snapshot interfaces...
 *    This guarantees that routers age routes out an interface, even when
 *    the remote side has nothing to say. Note that it is very important
 *    that the protocols can cope with empty updates...
 *
 * 7) If the protocol wishes, it can add a registry point to:
 *		reg_add_snapshot_state_change(func, "func");
 *    Of the form: func(idb, boolean enabled)
 *	Which will get called whenever Snapshot is enabled/disabled on a
 *	idb. THIS IS NOT REQUIRED!!
 *
 * 8) You need to check to make sure your protocol has an entry for the
 *    sys registry's "build_packet_address" routine. If it does not, you
 *    will have to add one.
 * 
 * 9) The Snapshot MIB must be modified, recompiled (be careful not to lose
 *    the old information), and a retrievable flag to reference your
 *    protocol type having received updates should be added. Look at the
 *    current MIB for examples. This is pretty simple...
 *
 * --------------------------------------------------------------------------
 * 
 * INTERACTION BETWEEN THE DIALER CODE AND SNAPSHOT
 * 
 * The dialer code has had hooks added to it (and calls back Snapshot hooks).
 * It is almost better to think of dialer/Snapshot as being part of the same
 * system, as they call each other frequently, but they have been split using
 * the registry mechanism so it is possible to load dialer w/o Snapshot, or
 * vice-versa.
 * 
 * DIALER'S KNOWLEDGE OF SNAPSHOT:
 * 	1) If a packet is of type 'LINK_SNAPSHOT', dialer_classify ALWAYS
 * 		returns TRUE
 * 
 * DIALER'S NEW REGISTRY POINTS IT CALLS TO INFORM SNAPSHOT OF EVENTS:
 * 	1) dialer_map_change: 
 * 		Whenever a 'dialer map' entry is added, changed, or deleted,
 * 		a registered routine is called with the link type, the IDB,
 * 		and the protocol_address block pointer, for the map entry,
 * 		(as well as a flag indicating if it is deleted)
 *	2) dialer_number_called_changed
 *		The number 'known to be connected to/called' on an interface
 *		has changed from 'nothing' to a specific number on a particular
 *		hwidb. This lets a client know an interface is ready to move
 *              into the active state (unless suppress-statechange-updates),
 *              and lets active SABs keep the dialer interface up.
 *      3) dialer_active
 *		A dialer interface has changed state. Snapshot server routers
 *              will force this interface up for a while to allow routing
 *              protocol updates to begin in the future from a client.
 * 
 * DIALER'S NEW REGISTRY POINTS OTHERS CAN CALL THAT SNAPSHOT CALLS:
 * 	1) dialer_initiate_connection (idb, addr, link)
 * 		- Initiate a call on the idb specified, with the
 * 		  addr/linktype specified, by sending a toy 2-byte frame
 * 		  of that linktype, with that address, out of the link.
 * 	2) dialer_connected (idb, addr, link)
 * 		- Return TRUE if dialer connected to address given
 * 	3) dialer_retrieve_maps (idb, link)
 * 		- Call dialer_map_change for all maps of type 'link'
 * 		  on idb specified
 * 	4) dialer_reset_timers (idb)
 * 		- Reset the idle timer and fast idle timer on the idb
 * 		  given.
 *      5) dialer_return_connected_hwidbs (idb, idb, proto_addr, link)
 *              - On the 'idb' given (both 1st and 2nd parameters should be the
 *                same), return a list of all of the hwidbs connected to
 *                the dialer string specified by a map statement with linktype
 *                'link' and address 'proto_addr.' This list is returned by
 *                the dialer code calling 'dialer_number_called_changed'
 *                as a 'callback' back to the caller of this function.
 * 
 * SNAPSHOT REGISTRY POINTS OTHERS CALL:
 * 	1) snapshot_update_denied
 * 		- Called by Snapshot-capable routing protocols to see if
 * 		  an update should not be sent on the idb specified
 * 	2) snapshot_update_received
 * 		- Called by Snapshot-capable routing protocols to tell Snapshot
 * 		  an update has been received on the link specified
 * 	3) snapshot_aging_denied
 * 		- Called by Snapshot-capable routing protocols to see if
 * 		  aging should not happen right now on the idb given
 * 	4) snapshot_idb_information
 * 		- Retrieves Snapshot info for idb specified
 *      5) snapshot_dialer_update_denied
 *              - Called only be the dialer to see if a particular update
 *                packet should not be sent out a particular hwidb. This
 *                avoids clients, configured with multiple 'up' hwidb links
 *                out a single dialer interface to multiple destinations,
 *                from sending update packets out to non-active server, which
 *                would inadvertantly kick those servers into the active state.
 * 	
 * SNAPSHOT CALLS THE FOLLOWING SNAPSHOT-SPECIFIC REGISTRY POINTS:
 * 	1) snapshot_start_aging_routes
 * 		- 'Cased' off of linktype, one for each Snapshot-capable
 * 		  routing protcol, informing them that routes learned
 * 		  on this idb should start aging
 *	2) snapshot_state_change
 *		- A list given by others, to inform them when Snapshot
 *		  changes state.
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "sys_registry.h"
#include "dialer_registry.h"
#include "subsys.h"
#include "interface_private.h"
#include "packet.h"
#include "address.h"
#include "config.h"
#include "parser.h"
#include "logger.h"
#include "../wan/msg_snapshot.c"	/* Not a typo, see logger.h */
#include "parser_defs_snapshot.h"
#include "../ui/debug.h"
#include "snapshot.h"
#include "snapshot_debug.h"
#include "../if/network.h"
#include "../ui/parse_util.h"
#include "snapshot_private.h"
#include "sr_snapshotmib.h"
#include "dialer.h"
#include "../ui/common_strings.h"

/* ------------------- SNAPSHOT GLOBAL INFORMATION ------------------------- */

/* All snapshot_interface_info blocks, in the order they were created */
queuetype snapshot_interfaces;

/* The 'run' queues for the snapshot_interface_info blocks */
snapshot_activity_block *snapshot_run_queues[SNAPSHOT_QUEUE_COUNT];

/* Incrementing value for each Snapshot index. Helps SNMP retrieval */
int snapshot_interface_info_index;

/* Used by SNMP to track items */
int snapshot_last_forced_if_index = 0;

/* All Snapshot free activity blocks, to avoid memory fragmentation */
snapshot_activity_block *snapshot_free_activity_block_queue;

/* All Snapshot free active dialer hardware blocks ('sadhb') */
snapshot_active_dialer_hardware_block
  *snapshot_free_active_dialer_hardware_block_queue;

snapshot_active_dialer_hardware_block *snapshot_active_dialer_hwidbs;

/* ----- SNAPSHOT ACTIVE DIALER HARDWARE BLOCK MEMORY POOL MAINTENANCE ---- */

/*
 * snapshot_init_active_dialer_hardware_block_queue
 *
 * Initialize the Snapshot active dialer hardware block memory pool system.
 */
static void snapshot_init_active_dialer_hardware_block_queue (void)
{
    snapshot_free_active_dialer_hardware_block_queue = NULL;
    snapshot_active_dialer_hwidbs = NULL;
}

/*
 * snapshot_malloc_active_dialer_hardware_block
 *
 * Return an active dialer hardware block to the caller. Grab (and initialize)
 * one off of the internal list if there is one. Ask the system for one if
 * there is not one available.
 */
static snapshot_active_dialer_hardware_block
  *snapshot_malloc_active_dialer_hardware_block (void)
{
    snapshot_active_dialer_hardware_block *sadhb;

    if (snapshot_free_active_dialer_hardware_block_queue != NULL) {
	sadhb = snapshot_free_active_dialer_hardware_block_queue;
	snapshot_free_active_dialer_hardware_block_queue = sadhb->next;
	memset(sadhb, 0, sizeof(snapshot_active_dialer_hardware_block));
    } else {
	sadhb = malloc(sizeof(snapshot_active_dialer_hardware_block));
    }
    return(sadhb);
}

/*
 * snapshot_free_active_dialer_hardware_block
 *
 * Put an active dialer hardware block back on the internal free queue, for
 * later use. This helps avoid memory fragmentation.
 */
static void snapshot_free_active_dialer_hardware_block
  (snapshot_active_dialer_hardware_block *sadhb)
{
    if (sadhb == NULL)
      return;

    sadhb->next = snapshot_free_active_dialer_hardware_block_queue;
    snapshot_free_active_dialer_hardware_block_queue = sadhb;
}

/*
 * snapshot_delete_active_dialer_hardware_block
 *
 * Delete a single active dialer hardware block. This is a separate routine
 * from the 'free' function to provide for additional logic to be done upon
 * deletion that is separate from the low level 'free' function, which is
 * purely reserved for memory management.
 */
static void snapshot_delete_active_dialer_hardware_block
  (snapshot_active_dialer_hardware_block *sadhb)
{
    if (sadhb != NULL) { 
	/* Keep hwidbs consistent. */
	if (sadhb->hwidb != NULL) {
	    sadhb->hwidb->snapshot_adhb = NULL;
	}
	/* Remove from active dialer hardware list, if there... */
	if (sadhb->prev != NULL) {
	    sadhb->prev->next = sadhb->next;
	} else if (snapshot_active_dialer_hwidbs == sadhb) {
	    snapshot_active_dialer_hwidbs = sadhb->next;
	}
	if (sadhb->next != NULL) {
	    sadhb->next->prev = sadhb->prev;
	}
	snapshot_free_active_dialer_hardware_block(sadhb);
    }
}

/*
 * snapshot_destroy_active_dialer_hardware_block_queue
 *
 * Return free active dialer hardware block queue elements to the system
 * memory pool. Before that, move all currently used active dialer hardware
 * blocks to the free pool.
 */
static void snapshot_destroy_active_dialer_hardware_block_queue (void)
{
    snapshot_active_dialer_hardware_block *sadhb, *sadhbnext;

    SNAPSHOT_FOR_ALL_SADHB_SAVE_NEXT(sadhb, sadhbnext) {
	snapshot_delete_active_dialer_hardware_block(sadhb);
    }

    for (sadhb = snapshot_free_active_dialer_hardware_block_queue;
	 sadhb != NULL;
	 sadhb = sadhbnext) {
	sadhbnext = sadhb->next;
	free(sadhb);
    }
    snapshot_free_active_dialer_hardware_block_queue = NULL;
}

/* ------------- DIALER SUPPORT INTERFACE, PART 1 -------------------------- */

/*
 * snapshot_set_dialer_stays_up
 *
 * Set the dialer interface specified to 'stay up' for a short duration.
 *
 * Done carefully on the server sides -- whenever a connection comes up.
 */
static void snapshot_set_dialer_stays_up (hwidbtype *hwidb, int time_to_live)
{
    snapshot_active_dialer_hardware_block *sadhb;

    if (hwidb == NULL)
      return;
    sadhb = hwidb->snapshot_adhb;
    if (sadhb == NULL) {
	/* Why bother to do more if not giving any time? */ 
	if (time_to_live == 0)
	  return;
	sadhb = snapshot_malloc_active_dialer_hardware_block();
	if (sadhb == NULL)
	  return;
	sadhb->hwidb = hwidb;
	sadhb->hwidb->snapshot_adhb = sadhb;
	sadhb->next = snapshot_active_dialer_hwidbs;
	if (sadhb->next != NULL) {
	    sadhb->next->prev = sadhb;
	}
	sadhb->prev = NULL;
	snapshot_active_dialer_hwidbs = sadhb;
    }
    sadhb->dialer_startup_remaining = time_to_live;
}


/*
 * snapshot_dialer_hwidbs_on_sab_statechange
 *
 * A dialer IDB connected string has been set or cleared on an interface.
 * If set, add IDB to list of IDBs being 'watched' by this SAB. If cleared,
 * remove IDB from list of IDBs being 'watched' by this SAB.
 */
static void snapshot_dialer_hwidbs_on_sab_statechange (snapshot_activity_block *sab,
						hwidbtype *hwidb, boolean set)
{
    int i;
    
    /* Check to see if this SA would exceed maximum # of interfaces */
    if (set) {
	for (i = 0; i < sab->count_dialer_connected_hwidbs; i++)
	    if (sab->dialer_connected_hwidbs[i] == hwidb)
		return;

	if (sab->count_dialer_connected_hwidbs >=
	    SNAPSHOT_MAX_DIALER_CONNECTED_HWIDBS) {
	    errmsg(&msgsym(TOOMANYDIALERS, SNAPSHOT), hwidb->hw_namestring);
	    return;
	}
	sab->dialer_connected_hwidbs[sab->count_dialer_connected_hwidbs++] =
	    hwidb;
    } else {
	for (i = 0; i < sab->count_dialer_connected_hwidbs; i++) {
	    if (sab->dialer_connected_hwidbs[i] == hwidb) {
		/*
		 * Decrement total, and copy final one, if any, into here.
		 * This is the quickest way to 'repack' the array, because the
		 * order doesn't mean squat.
		 */
		if (--sab->count_dialer_connected_hwidbs > 0) {
		    sab->dialer_connected_hwidbs[i] =
			sab->dialer_connected_hwidbs
			    [sab->count_dialer_connected_hwidbs];
		} else {
		    /* Last one. Possibly clear exchange period info */
		    if (sab->active_exchange_period <
			SNAPSHOT_SUCCESSFULL_EXCHANGE_MINUTES(sab->sii)) {
			sab->active_exchange_period = 0;
		    }
		}
		return;
	    }
	}
    }
}

/*
 * snapshot_kick_dialer_into_gear
 *
 * Once a minute, while active, force the dialer to initiate the call,
 * if it is not already connected.
 */
static void snapshot_kick_dialer_into_gear (snapshot_activity_block *sab)
{
    if (SNAPSHOT_IS_DIALER(sab->sii)) {
	if (SNAPSHOT_IS_CLIENT(sab->sii)) {
	    reg_invoke_dialer_initiate_connection
	      (hwidb_or_null(sab->sii->idb), (long) sab->address,
	       LINK_SNAPSHOT);
	} else {
	    reg_invoke_dialer_reset_timers(hwidb_or_null(sab->sii->idb));
	}
    }
}

/*
 * snapshot_dialer_timer_reset
 *
 * Reset dialer timers on all Snapshot interfaces that are active
 */
static void snapshot_dialer_timer_reset (void)
{
    snapshot_activity_block *sab;
    int i;
    snapshot_active_dialer_hardware_block *sadhb;

    /* All hwidbs associated with active queues are reset */
    SNAPSHOT_FOR_ALL_SAB_IN_QUEUE(sab, SNAPSHOT_ACTIVE_Q) {
	for (i = 0; i < sab->count_dialer_connected_hwidbs; i++) {
	    reg_invoke_dialer_reset_timers
		(sab->dialer_connected_hwidbs[i]);
	}
    }

    /* All interfaces with the startup remaining set are reset... */
    SNAPSHOT_FOR_ALL_SADHB(sadhb) {
	if (sadhb->dialer_startup_remaining > 0) {
	    reg_invoke_dialer_reset_timers(sadhb->hwidb);
	}
    }
}

/*
 * snapshot_find_current_connected_hwidbs
 *
 * For the snapshot_activity_block given, find all of the currently connected
 * dialer hwidbs. This just invokes a dialer registry, which will go off and
 * call us back again, claiming that these have 'come up' - this is done
 * for all interfaces associated with the hunt-group/whatever of the
 * sab->sii->idb.
 */
static void snapshot_find_current_connected_hwidbs (snapshot_activity_block *sab)
{
    hwidbtype *hwidb;
    addrtype addr;

    memset(&addr, 0, sizeof(addrtype));
    addr.snapshot_addr = sab->address;
    addr.type = ADDR_SNAPSHOT;
    addr.length = ADDRLEN_SNAPSHOT;
    
    hwidb = hwidb_or_null(sab->sii->idb);
    if (hwidb != NULL) {
	reg_invoke_dialer_return_connected_hwidbs(hwidb, hwidb, &addr,
							LINK_SNAPSHOT);
    }
}

/* -------------------- SNAPSHOT RUN QUEUE MAINTENANCE --------------------- */

/*
 * snapshot_run_queue_name
 *
 * Return the name of a Snapshot queue
 */
static char *snapshot_run_queue_name (int queueoffset)
{
    switch (queueoffset) {
      case SNAPSHOT_NO_Q:
	  return("creation/deletion");
      case SNAPSHOT_ACTIVE_Q:
	  return("active");
      case SNAPSHOT_CLIENT_QUIET_Q:
	  return("quiet");
      case SNAPSHOT_SERVER_POST_ACTIVE_Q:
	  return("server post active");
      case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q:
	  return("client post active->quiet");
      case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q:
	  return("client post active->active");
      default:
	  return("-Unknown State-");
    }
}

/*
 * snapshot_move_to_new_run_queue
 *
 * Move an SAB off its current state queue, if any, and onto a new queue.
 * New queue = SNAPSHOT_NO_Q if it should not move onto any new queue.
 */
static void snapshot_move_to_new_run_queue (snapshot_activity_block *sab,
				     int new_queue, ulong time_in_queue)
{
    if (snapshot_debug)
	buginf("\nSNAPSHOT: %s[%d]: moving to %s queue",
	       sab->sii->idb->namestring, sab->address,
	       snapshot_run_queue_name(new_queue));

    /* Remove from current state queue, if any */
    if (!SNAPSHOT_IS_IN_NO_Q(sab)) {
	if (sab->queue_prev != NULL) {
	    sab->queue_prev->queue_next = sab->queue_next;
	} else {
	    snapshot_run_queues[sab->runtime_queue] = sab->queue_next;
	}
	if (sab->queue_next != NULL) {
	    sab->queue_next->queue_prev = sab->queue_prev;
	}
    }

    /* Put on head of new queue, if any */
    sab->queue_prev = NULL;
    sab->runtime_queue = new_queue;

    if (SNAPSHOT_IS_IN_NO_Q(sab)) {
	sab->queue_next = NULL;
    } else {
	sab->queue_next = snapshot_run_queues[new_queue];
	if (sab->queue_next != NULL) {
	    sab->queue_next->queue_prev = sab;
	}
	snapshot_run_queues[new_queue] = sab;
    }

    sab->time_left_in_this_queue = time_in_queue;
    sab->count_dialer_connected_hwidbs = 0;
}

/* ------------ SNAPSHOT ACTIVITY BLOCK MEMORY POOL MAINTENANCE ------------ */

/*
 * snapshot_init_activity_block_queue
 *
 * Initialize the Snapshot activity block memory pool system.
 */
static void snapshot_init_activity_block_queue (void)
{
    snapshot_free_activity_block_queue = NULL;
}

/*
 * snapshot_malloc_activity_block
 *
 * Return an activity_block to the caller. Grab (and initialize) one off of
 * the internal list if there is one. Ask the system for one if there is not
 * one available.
 */
static snapshot_activity_block *snapshot_malloc_activity_block (void)
{
    snapshot_activity_block *sab;

    if (snapshot_free_activity_block_queue != NULL) {
	sab = snapshot_free_activity_block_queue;
	snapshot_free_activity_block_queue = sab->next;
	memset(sab, 0, sizeof(snapshot_activity_block));
    } else {
	sab = malloc(sizeof(snapshot_activity_block));
    }
    return(sab);
}

/*
 * snapshot_free_activity_block
 *
 * Put an activity block back on the internal free queue, for later use.
 * This helps avoid memory fragmentation.
 */
static void snapshot_free_activity_block (snapshot_activity_block *sab)
{
    if (sab == NULL)
      return;

    snapshot_move_to_new_run_queue(sab, SNAPSHOT_NO_Q, 0L);
    sab->next = snapshot_free_activity_block_queue;
    snapshot_free_activity_block_queue = sab;
}

/*
 * snapshot_destroy_activity_block_queue
 *
 * Return free activity block queue elements to the system memory pool.
 */
static void snapshot_destroy_activity_block_queue (void)
{
    snapshot_activity_block *sab, *sabnext;

    for (sab = snapshot_free_activity_block_queue;
	 sab != NULL;
	 sab = sabnext) {
	sabnext = sab->next;
	free(sab);
    }
    snapshot_free_activity_block_queue = NULL;
}

/*
 * snapshot_delete_activity_block
 *
 * Delete a single activity block.
 */
static void snapshot_delete_activity_block (snapshot_activity_block *sab)
{
    snapshot_activity_block *sabp;

    /* Set previous's next to our next */
    if (sab->sii->activity_blocks != sab) {
	SNAPSHOT_FOR_ALL_SAB_ON_SII(sabp, sab->sii) {
	    if (sabp->next == sab)
		break;
	}
	if (sabp != NULL) {
	    sabp->next = sab->next;
	}
    } else {
	sab->sii->activity_blocks = sab->next;
    }
    snapshot_free_activity_block(sab);
}

/* ------------- SNAPSHOT ACTIVITY BLOCK STATE CHANGE SUPPORT -------------- */

/*
 * snapshot_make_more_quiet
 *
 * Move the Snapshot activity block given to the appropriate 'quieter' queue.
 * NOTE: FOR SERVER INTERFACES, THE ACTIVITY BLOCK PASSED IN MAY BE FREED!!
 *
 * Returns TRUE if the activity block was freed.
 */
static boolean snapshot_make_more_quiet (snapshot_activity_block *sab)
{
    ulong new_queue = SNAPSHOT_NO_Q;
    ulong new_time = 0;
    boolean avoiding_state_change = FALSE;
    boolean delete_it = FALSE;

    if (SNAPSHOT_IS_SERVER(sab->sii)) {
	switch (sab->runtime_queue) {
	case SNAPSHOT_ACTIVE_Q:
	    new_queue = SNAPSHOT_SERVER_POST_ACTIVE_Q;
	    new_time = SNAPSHOT_GUARANTEED_QUIET_MINUTES;
	    break;
	case SNAPSHOT_SERVER_POST_ACTIVE_Q:
	    if (sab->time_left_in_this_queue > 0)
		avoiding_state_change = TRUE;
	    else
		delete_it = TRUE;
	    break;
	default:
	    errmsg(&msgsym(BADSTATE, SNAPSHOT), sab->sii->idb->namestring,
		   sab->address, 1);
	    delete_it = TRUE;
	    break;
	}
    } else {
	switch (sab->runtime_queue) {
	case SNAPSHOT_CLIENT_QUIET_Q:
	case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q:
	    avoiding_state_change = TRUE;
	    break;
	case SNAPSHOT_ACTIVE_Q:
	    new_queue = SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q;
	    new_time = SNAPSHOT_GUARANTEED_QUIET_MINUTES;
	    break;
	case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q:
	    if (sab->time_left_in_this_queue > 0) {
		avoiding_state_change = TRUE;
	    } else {
		new_queue = SNAPSHOT_CLIENT_QUIET_Q;
		/* Retry interval if no updates exchanged; else full time */
		if (!SNAPSHOT_CHECK_ANY_PROTOCOL_UPDATES_EXCHANGED(sab)) {
		    new_time = SNAPSHOT_RETRY_INTERVAL(sab->sii->active_time);
		    if (snapshot_debug)
			buginf("\nSNAPSHOT: %s[%d]: retrying; no updates exchanged",
			       sab->sii->idb->namestring, sab->address);
		} else if (sab->active_exchange_period <
			   SNAPSHOT_SUCCESSFULL_EXCHANGE_MINUTES(sab->sii)) {
		    if (snapshot_debug)
			buginf("\nSNAPSHOT: %s[%d]: retrying; want %d, got %d xchg minutes",
			       sab->sii->idb->namestring, sab->address,
			       SNAPSHOT_SUCCESSFULL_EXCHANGE_MINUTES(sab->sii),
			       sab->active_exchange_period);
		} else {
		    new_time = sab->sii->quiet_time;
		}
	    }
	    break;
	default:
	    errmsg(&msgsym(BADSTATE, SNAPSHOT), sab->sii->idb->namestring,
		   sab->address, 2);
	    new_queue = SNAPSHOT_CLIENT_QUIET_Q;
	    new_time = SNAPSHOT_RETRY_INTERVAL(sab->sii->active_time);
	    break;
	}
    }

    if (delete_it) {
	snapshot_delete_activity_block(sab);
    } else if (avoiding_state_change) {
	if (snapshot_debug)
	    buginf("\nSNAPSHOT: %s[%d]: Avoiding quiet: in %s queue",
		   sab->sii->idb->namestring, sab->address,
		   snapshot_run_queue_name(sab->runtime_queue));
    } else {
	snapshot_move_to_new_run_queue(sab, new_queue, new_time);
    }
    return(delete_it);
}

/*
 * snapshot_make_more_active
 *
 * Move the Snapshot activity block given to a more 'active' queue.
 */
static void snapshot_make_more_active (snapshot_activity_block *sab, char *reason)
{
    ulong new_queue = SNAPSHOT_NO_Q;
    ulong new_time = 0;
    boolean avoiding_state_change = FALSE;
    boolean choose_active_state = FALSE;
    
    switch (sab->runtime_queue) {
    case SNAPSHOT_NO_Q:
	/* This is how new SABs get initialized into the proper queue */
	if (SNAPSHOT_IS_CLIENT(sab->sii)) {
	    new_queue = SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q;
	    new_time = SNAPSHOT_GUARANTEED_QUIET_MINUTES;
	} else {
	    choose_active_state = TRUE;
	}
	break;
    case SNAPSHOT_ACTIVE_Q:
    case SNAPSHOT_SERVER_POST_ACTIVE_Q:
	avoiding_state_change = TRUE;
	break;
    case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q:
	new_queue = SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q;
	new_time = sab->time_left_in_this_queue;
	break;
    case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q:
	if (sab->time_left_in_this_queue > 0)
	    avoiding_state_change = TRUE;
	else
	    choose_active_state = TRUE;
	break;
    default:
	choose_active_state = TRUE;
	break;
    }

    /* Real, full active state chosen? */
    if (choose_active_state) {
	/* No protocols have started to age... */
	SNAPSHOT_CLEAR_PROTOCOL_UPDATES_EXCHANGED(sab);
	sab->active_exchange_period = 0;
	snapshot_kick_dialer_into_gear(sab);
	if (snapshot_debug)
	    buginf("\nSNAPSHOT: %s[%d]: Move to active queue (%s)",
		   sab->sii->idb->namestring, sab->address, reason);
	new_queue = SNAPSHOT_ACTIVE_Q;
	new_time = sab->sii->active_time;
    }

    if (avoiding_state_change) {
	if (snapshot_debug)
	    buginf("\nSNAPSHOT: %s[%d]: Avoiding active: in %s queue (%s)",
		   sab->sii->idb->namestring, sab->address,
		   snapshot_run_queue_name(sab->runtime_queue), reason);
    } else {
	snapshot_move_to_new_run_queue(sab, new_queue, new_time);
	if (SNAPSHOT_IS_IN_ACTIVE_Q(sab) && SNAPSHOT_IS_DIALER(sab->sii) &&
	    SNAPSHOT_IS_CLIENT(sab->sii)) {
	    snapshot_find_current_connected_hwidbs(sab);
	}
    }
}

/* ----------------- SNAPSHOT ACTIVITY BLOCK CONTROL ----------------------- */

/*
 * snapshot_delete_all_activity_blocks
 *
 * Delete activity blocks associated with this Snapshot entry
 */
void snapshot_delete_all_activity_blocks (snapshot_interface_info *sii)
{
    snapshot_activity_block *sab, *sabnext;

    SNAPSHOT_FOR_ALL_SAB_ON_SII_SAVE_NEXT(sab, sii, sabnext) {
	snapshot_delete_activity_block(sab);
    }
    sii->activity_blocks = NULL;
}

/*
 * snapshot_find_activity_block
 *
 * Find an activity block structure given an address
 */
static snapshot_activity_block *snapshot_find_activity_block (snapshot_interface_info
						       *sii, ulong addr)
{
    snapshot_activity_block *sab;

    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	if (addr == sab->address) {
	    return(sab);
	}
    }
    return(NULL);
}

/*
 * snapshot_create_activity_block
 *
 * Create a single activity block
 */
static snapshot_activity_block *snapshot_create_activity_block
  (snapshot_interface_info *sii, ulong addr,
   addrtype *protocol_address)
{
    snapshot_activity_block *sab, *sabp = NULL;

    /* Find the last activity block */
    if (sii->activity_blocks != NULL) {
	for (sabp = sii->activity_blocks; sabp->next != NULL;
	     sabp = sabp->next)
	  ;
    }

    sab = snapshot_malloc_activity_block();
    if (sab == NULL)
       return(NULL);

    /* Perform initial setup for activity block */
    sab->data_integrity_check = SNAPSHOT_ACTIVITY_BLOCK_DATA_INTEGRITY;
    sab->count_dialer_connected_hwidbs = 0;
    if (protocol_address != NULL) {
	bcopy((uchar *) protocol_address, (uchar *) &(sab->protocol_address),
	      sizeof(addrtype));
    } else {
	memset(&(sab->protocol_address), 0, sizeof(addrtype));
    }
    sab->address = addr;
    sab->runtime_queue = SNAPSHOT_NO_Q;
    sab->queue_next = sab->queue_prev = NULL;
    sab->sii = sii;
    sab->index = sii->activity_index++;
    /* Attach the new activity block to the end of the list */
    if (sii->activity_blocks == NULL) {
	sii->activity_blocks = sab;
    } else {
	sabp->next = sab;
    }
    
    snapshot_make_more_active(sab, "Snapshot activity block created");
    return(sab);
}

/*
 * snapshot_create_initial_activity_blocks
 *
 * Create the initial activity blocks. Servers have none. Clients that
 * are not dialers, get one. Dialer clients get one for each dialer map
 * entry, of course there may be none of these at the current time...
 */
void snapshot_create_initial_activity_blocks (snapshot_interface_info *sii)
{
    if (SNAPSHOT_IS_CLIENT(sii)) {
	if (SNAPSHOT_IS_DIALER(sii)) {
	    reg_invoke_dialer_retrieve_maps(hwidb_or_null(sii->idb),
						  LINK_SNAPSHOT);
	} else {
	    (void) snapshot_create_activity_block(sii, SNAPSHOT_NO_DIALER_ADDR,
						  NULL);
	}
    }
}

/* ----------------------- SNAPSHOT CLEAN SHUTDOWN ------------------------- */
/*
 * tini_tohspans();
 *
 * The reverse of snapshot_init();
 */
static void tini_tohspans (void)
{
    snapshot_destroy_activity_block_queue();
    snapshot_destroy_active_dialer_hardware_block_queue();
}

/* ----------------------- SNAPSHOT RUNTIME PROCESS ------------------------ */

/*
 * snapshot_tasker
 *
 * Perform all Snapshot tasks, once a minute.
 */
forktype snapshot_tasker (void)
{
    snapshot_activity_block *sab, *next_in_queue;
    snapshot_active_dialer_hardware_block *sadhb, *sadhb_next_in_queue;

    while (TRUE) {

	/* Do work on items in list. If list is NULL, die. */
	if (QUEUEEMPTY(&snapshot_interfaces)) {
	    tini_tohspans();
	    process_set_analyze(FALSE);
	    process_kill(THIS_PROCESS);
	}

	/*
	 * Climb through all of the hardware interfaces, and reduce the
	 * 'time active' count.
	 */
	SNAPSHOT_FOR_ALL_SADHB_SAVE_NEXT(sadhb, sadhb_next_in_queue) {
	    if (sadhb->dialer_startup_remaining > 0) {
		sadhb->dialer_startup_remaining--;
	    } else {
		snapshot_delete_active_dialer_hardware_block(sadhb);
	    }
	}
	
	/*
	 * Climb through the server post active queue, bringing any timed
	 * out SABs to a quieter state.
	 */
	SNAPSHOT_FOR_ALL_SAB_IN_QUEUE_SAVE_NEXT(sab,
						SNAPSHOT_SERVER_POST_ACTIVE_Q,
						next_in_queue) {
	    /* Move the item to the sleeping queue if done */
	    if (sab->time_left_in_this_queue-- <= 0) {
		(void) snapshot_make_more_quiet(sab);
	    }
	}

	/*
	 * Climb through the client post active queue, bringing any timed
	 * out SABs to a quieter state.
	 */
	SNAPSHOT_FOR_ALL_SAB_IN_QUEUE_SAVE_NEXT
	    (sab, SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q, next_in_queue) {
	    /* Move the item to the sleeping queue if done */
	    if (sab->time_left_in_this_queue-- <= 0) {
		(void) snapshot_make_more_quiet(sab);
	    }
	}


	/*
	 * Climb through the active queue, bringing any timed out SABs
	 * to a quieter state. Also, increment exchange period if the
	 * interface is up (non-dialer) or there are connected dialer
	 * IDBs (dialer).
	 */
	SNAPSHOT_FOR_ALL_SAB_IN_QUEUE_SAVE_NEXT(sab, SNAPSHOT_ACTIVE_Q,
						next_in_queue) {
	    /* Non-zero means the timer has started... */
	    if (sab->active_exchange_period > 0) {
		if (SNAPSHOT_IS_DIALER(sab->sii)) {
		    if (sab->count_dialer_connected_hwidbs > 0)
			sab->active_exchange_period++;
		} else {
		    if (interface_up(sab->sii->idb))
			sab->active_exchange_period++;
		}
	    }
	    /* Move the item to a quieter queue if timed out. */
	    if (sab->time_left_in_this_queue-- <= 0) {
		(void) snapshot_make_more_quiet(sab);
	    } else {
		/* Force the dialer interface awake (if failed first time) */
		snapshot_kick_dialer_into_gear(sab);
	    }
	}

	/*
	 * Climb through the client post active->active queue, bringing any
	 * timed out SABs back to the active state.
	 */
	SNAPSHOT_FOR_ALL_SAB_IN_QUEUE_SAVE_NEXT
	    (sab, SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q, next_in_queue) {
	    /* Move the item to the sleeping queue if done */
	    if (sab->time_left_in_this_queue-- <= 0) {
		(void) snapshot_make_more_active(sab, "Post active timeout");
	    }
	}
	
	/*
	 * Go through client quiet queue. Move now active items to the active
	 * queue.
	 */
	SNAPSHOT_FOR_ALL_SAB_IN_QUEUE_SAVE_NEXT(sab, SNAPSHOT_CLIENT_QUIET_Q,
						next_in_queue) {
	    if (sab->time_left_in_this_queue-- <= 0) {
		snapshot_make_more_active(sab, "Quiet timer expired");
	    }
	}
	process_sleep_for(ONEMIN);
    }    
}

/* -------------- SNAPSHOT INTERFACE INFO (SII) SUPPORT -------------------- */

/*
 * snapshot_find_idb
 *
 * Return the snapshot_interface_info block associated with the given
 * IDB, if any.
 */
inline snapshot_interface_info *snapshot_find_idb (idbtype *idb)
{
    snapshot_interface_info *sii;

    if (idb == NULL) {
	return(NULL);
    }
    sii = idb->sii; 

    if ((sii != NULL) && (sii->idb != idb))
	return(NULL);
    else
	return(sii);
}

/*
 * snapshot_delete_usage
 *
 * Delete usage of a particular snapshot_interface_info block
 */
void snapshot_delete_usage (snapshot_interface_info *sii)
{
    int i;

    if (snapshot_debug)
	buginf("\nSNAPSHOT: %s: Deleting support on interface",
	       sii->idb->namestring);
    snapshot_delete_all_activity_blocks(sii);
    unqueue(&snapshot_interfaces, sii);
    /* Registry inform all protocols that they are losing snapshot */
    for (i = 0; i < LINK_MAXLINKTYPE; i++)
      reg_invoke_snapshot_start_aging_routes(i, sii->idb, NULL); 
    /* Now let everyone know snapshot has been removed from the interface */
    reg_invoke_snapshot_state_change(sii->idb, FALSE);
    sii->idb->sii = NULL;
    free(sii);
}

/* ------------- DIALER SUPPORT INTERFACE, PART 2 -------------------------- */

/*
 * snapshot_dialer_map_change
 *
 * Registry function - typically called by dialer when a new map is
 * added of linktype LINK_SNAPSHOT
 */
static void snapshot_dialer_map_change (hwidbtype *hwidb, addrtype *addr,
				 boolean deleting)
{
    idbtype *idb;
    snapshot_interface_info *sii;
    snapshot_activity_block *sab;

    if (hwidb == NULL)
	return;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {    
	sii = snapshot_find_idb(idb);
	if ((sii == NULL) || (!SNAPSHOT_IS_DIALER(sii)) ||
	    SNAPSHOT_IS_SERVER(sii))
	    continue;
	sab = snapshot_find_activity_block(sii, addr->snapshot_addr);
	if (deleting) {
	    if (sab != NULL) {
		snapshot_delete_activity_block(sab);
	    }
	} else {
	    if (sab == NULL) {
		(void) snapshot_create_activity_block(sii, addr->snapshot_addr,
						     NULL);
	    }
	}
    }
}

/*
 * snapshot_dialer_number_called_change
 *
 * Registry function - typically called by dialer when either an incoming
 * call has been established, or an outgoing call is being started. If
 * supress-statechange-updates is not set on the inteface, bring the
 * appropriate Snapshot activity block into the active state. This is done
 * only on the client side, as the server side is always made active by
 * client intervention. On the server side, if the number is cleared (set
 * is FALSE), we do drop the interface associated with it from and and all 
 * SABs on this 'master_idb' that may reference it. This tells us that
 * those SABs no longer want to send updates out that interface, as the
 * client is 'no longer there' (if and when it is again, it will send the
 * server another update.)
 *
 * If passed in addr == SNAPSHOT_SERVER_DIALER_ADDR, this is dialer telling
 * us the interface went down. This is what should be used by server sides,
 * and the non-SNAPSHOT_SERVER_DIALER_ADDR entries should NOT be used for
 * servers.
 */
static void snapshot_dialer_number_called_change (hwidbtype *real_idb,
					   hwidbtype *master_idb,
					   addrtype *addr, boolean set)
{
    idbtype *idb;
    snapshot_interface_info *sii;
    snapshot_activity_block *sab;

    FOR_ALL_SWIDBS_ON_HW(master_idb, idb) {    
	sii = snapshot_find_idb(idb);
	if ((sii != NULL) && SNAPSHOT_IS_DIALER(sii)) {
	    if (SNAPSHOT_IS_CLIENT(sii)) {
		/* Ignore this if no dialer addr -- indicates server news */
		if (addr->snapshot_addr == SNAPSHOT_SERVER_DIALER_ADDR) {
		    return;
		}
		sab = snapshot_find_activity_block(sii, addr->snapshot_addr);
		if (sab != NULL) {
		    if (set && !SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii))
		      snapshot_make_more_active(sab, "Dial connection set");
		    if (SNAPSHOT_IS_IN_ACTIVE_Q(sab))
		      snapshot_dialer_hwidbs_on_sab_statechange(sab, real_idb,
								set);
		}
	    } else {
		/*
		 * Server side. If 'not set', remove from all lists. 'set' will
		 * be done when protocol updates are exchanged.
		 */
		if ((!set) && (addr->snapshot_addr ==
			       SNAPSHOT_SERVER_DIALER_ADDR)) {
		    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
			snapshot_dialer_hwidbs_on_sab_statechange(sab,
								  real_idb,
								  FALSE);
		    }
		}
	    }
	}
    }
}

/*
 * snapshot_dialer_active
 *
 * Registry function - typically called by dialer when an interface is
 * changing state to 'up'.  This will force the server side interface to
 * STAY up for a full number of active minutes to allow the client to
 * control the actual connected time.
 */
static void snapshot_dialer_active (hwidbtype *idb_changed,
			     hwidbtype *hunt_group_master, boolean up)
{
    hwidbtype *hwidb = (hunt_group_master != NULL ?
		      hunt_group_master : idb_changed);
    idbtype *idb;
    snapshot_interface_info *sii;
    
    if ((hwidb == NULL) || (!up))
	return;

    FOR_ALL_SWIDBS_ON_HW(hwidb, idb) {    
	sii = snapshot_find_idb(idb);
	if ((sii != NULL) && SNAPSHOT_IS_DIALER(sii) &&
	    SNAPSHOT_IS_SERVER(sii)) {
	    /*
	     * Keep it running for a while. Only done on server side, as
	     * client side will set these flags up only when it has the dialer
	     * connected strings set up (allows client to fully control the
	     * duration of the link being up; the server will NOT get in the
	     * way.) Once this is done once, no need to do it anymore, so
	     * just stop...
	     */
	    snapshot_set_dialer_stays_up
	      (idb_changed, SNAPSHOT_DIALER_EXTRA_BREATHING_ROOM_MINUTES);
	    return;
	}
    }
}

/* ----------------- SNAPSHOT COMMAND PARSING SUPPORT ---------------------- */

/*
 * snapshot_command
 *
 * Configure global Snapshot commands. Currently, there are none, but
 * if you don't build one of these functions, some automatically generated
 * parser code goes wacko with undefined references to this function.
 */
void snapshot_command (parseinfo *csb)
{
    return;
}

/*
 * snapshotif_command
 *
 * Parse Snapshot interface specific information.
 */
void snapshotif_command (parseinfo *csb)
{
    idbtype *idb;
    snapshot_interface_info *sii;
    boolean added;
    
    idb = csb->interface;
    sii = snapshot_find_idb(idb);

    if (csb->nvgen) {
	if (sii != NULL) {
	    nv_write(TRUE, "%s", csb->nv_command);
	    if (SNAPSHOT_IS_SERVER(sii)) {
		nv_add(TRUE, " %s %d", SNAPSHOT_KEYWORD_SERVER,
		       sii->active_time);
	    } else {
		nv_add(TRUE, " %s %d %d", SNAPSHOT_KEYWORD_CLIENT,
		       sii->active_time, sii->quiet_time);
		nv_add(SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii), " %s",
		       SNAPSHOT_KEYWORD_SUPPRESS_STATECHANGE_UPDATES);
	    }
	    nv_add(SNAPSHOT_IS_DIALER(sii), " %s", SNAPSHOT_KEYWORD_DIALER);
	}
    } else if (!csb->sense) {

	if (sii == NULL)
	    return;

	switch (GETOBJ(int, 1)) {
	  case SNAPSHOT_CLIENT:
	    if (SNAPSHOT_IS_SERVER(sii)) {
		printf("\nInterface currently configured for server, not client, Snapshot support");
		return;
	    }
	    snapshot_delete_usage(sii);
	    break;
	  case SNAPSHOT_SERVER:
	    if (SNAPSHOT_IS_CLIENT(sii)) {
		printf("\nInterface currently configured for client, not server, Snapshot support");
		return;
	    }
	    snapshot_delete_usage(sii);
	    break;
	}
    } else {
	switch (GETOBJ(int, 1)) {
	  case SNAPSHOT_CLIENT:
	    /*
	     * The quiet interval must never be allowed to be less than
	     * the retry interval, which is the minimum period of time
	     * to allow the server/client to both time out to fully
	     * 'quiet' in any communication mishap between client and
	     * server.
	     */
	    if (GETOBJ(int, 3) < SNAPSHOT_RETRY_INTERVAL(GETOBJ(int, 2))) {
		printf("\n% Cannot configure quiet time less than %d for active time of %d",
		       SNAPSHOT_RETRY_INTERVAL(GETOBJ(int,2)), GETOBJ(int, 2));
		return;
	    }
	    /* FALL THROUGH */
	  case SNAPSHOT_SERVER:
	    if (sii == NULL) {
		sii = malloc(sizeof(snapshot_interface_info));
		if (sii == NULL) {
		    printf(nomemory);
		    return;
		}
		if (QUEUEEMPTY(&snapshot_interfaces)) {
		    cfork(snapshot_tasker, 0L, 0, "Snapshot", 0);
		}
		sii->data_integrity_check =
		  SNAPSHOT_INTERFACE_INFO_DATA_INTEGRITY;
		sii->index = snapshot_interface_info_index++;
		sii->activity_index = 0;
		sii->activity_blocks = NULL;
		sii->idb = idb;
		idb->sii = sii;
		if (interface_up(idb)) {
		    SNAPSHOT_SET_IDB_UP(sii);
		} else {
		    SNAPSHOT_SET_IDB_DOWN(sii);
		}
		enqueue(&snapshot_interfaces, sii);
		added = TRUE;
	    } else {
		added = FALSE;
		if ((SNAPSHOT_IS_CLIENT(sii) ?
		     (sii->quiet_time == GETOBJ(int, 3)) : TRUE) &&
		       (sii->active_time == GETOBJ(int, 2)) &&
		       (SNAPSHOT_IS_CLIENT(sii) ?
			(GETOBJ(int, 1) == SNAPSHOT_CLIENT) :
			(GETOBJ(int, 1) == SNAPSHOT_SERVER)) &&
		       (SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii) ?
			GETOBJ(int, 4) : (!GETOBJ(int, 4))) &&
		       (SNAPSHOT_IS_DIALER(sii) ?
			GETOBJ(int, 5) : (!GETOBJ(int, 5)))) {
		    /* No change! */
		    return;
		}
	    }

	    SNAPSHOT_CLEAR_FLAGS(sii);
	    if (GETOBJ(int, 1) == SNAPSHOT_SERVER) {
		SNAPSHOT_SET_SERVER(sii);
	    }
	    sii->active_time = GETOBJ(int, 2);
	    sii->quiet_time = GETOBJ(int, 3);
	    if (GETOBJ(int, 4)) {
		SNAPSHOT_SET_SUPPRESS_STATECHANGE_UPDATES(sii);
	    }
	    if (GETOBJ(int, 5)) {
		SNAPSHOT_SET_DIALER(sii);
	    }
	    snapshot_delete_all_activity_blocks(sii);
	    snapshot_create_initial_activity_blocks(sii);
	    if (snapshot_debug)
		buginf("\nSNAPSHOT: %s: Changing support on interface",
		       sii->idb->namestring);
	    if (added)
	      reg_invoke_snapshot_state_change(sii->idb, TRUE);
	    break;
	}
    }
}

/* ------------------- SNAPSHOT SHOW COMMAND SUPPORT ----------------------- */

/* 
 * minute_string
 *
 * Return singular/plural minute string
 */
static char *minute_string (int value)
{
    return((value == 1) ? "minute" : "minutes");
}

/*
 * snapshot_show_first_protocol_update
 *
 * Show the first protocol update that has been received on this link
 * (for the server side).
 */
static void snapshot_show_first_protocol_update (snapshot_activity_block *sab)
{
    ulong i;

    for (i = 0; i < LINK_MAXLINKTYPE; i++) {
	if (SNAPSHOT_CHECK_PROTOCOL_UPDATES_EXCHANGED(sab, i)) {
	    printf("%s", print_linktype(i));
	    break;
	}
    }
}

/*
 * snapshot_show_protocol_updates
 *
 * Show the protocols that have received on this link.
 */
static void snapshot_show_protocol_updates (snapshot_activity_block *sab)
{
    boolean once = FALSE;
    ulong i;

    /* Nothing interesting to print for servers.. */
    if (SNAPSHOT_IS_SERVER(sab->sii))
	return;
    for (i = 0; i < LINK_MAXLINKTYPE; i++) {
	if (SNAPSHOT_CHECK_PROTOCOL_UPDATES_EXCHANGED(sab, i)) {
	    if (once) {
		printf(", ");
	    } else  {
		once = TRUE;
		printf("\n    Updates received this cycle: ");
	    }
	    printf("%s", print_linktype(i));
	}
    }
}

/*
 * show_snapshot_info
 *
 * Show the details about Snapshot information for the given info block
 */
static void show_snapshot_info (snapshot_interface_info *sii)
{
    snapshot_activity_block *sab;
    boolean once;
    boolean client;
    char addr_string[1000];
    int i;

    if (sii->data_integrity_check != SNAPSHOT_INTERFACE_INFO_DATA_INTEGRITY) {
        printf("\nSNAPSHOT: Interface information integrity suspect at address 0x%x", 
	       sii);
	return;
    }

    client = SNAPSHOT_IS_CLIENT(sii);
    
    printf(if_is_line_proto_is_msg, sii->idb->namestring,
	   print_idbstate(sii->idb), interface_up(sii->idb) ? "up" : "down");
    printf("Snapshot %s", client ? "client" : "server");
    if (!SNAPSHOT_IS_DIALER(sii)) {
	printf(" line state %s", SNAPSHOT_IS_IDB_UP(sii) ? "up" : "down");
    }
    if (SNAPSHOT_IS_DIALER(sii) ||
	SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii)) {
	printf("\n  Options:");
	once = FALSE;
	if (SNAPSHOT_IS_DIALER(sii)) {
	    printf(" dialer support");
	    once = TRUE;
	}
	if (SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii)) {
	    if (once) printf(",");
	    printf(" stay asleep on carrier up");
	}
    }
    printf("\n  Length of active period:          %d %s",
	   sii->active_time, minute_string(sii->active_time));
    if (SNAPSHOT_IS_CLIENT(sii)) {
	printf("\n  Length of quiet period:           %d %s",
	       sii->quiet_time, minute_string(sii->quiet_time));
	printf("\n  Length of retry period:           %d %s",
	       SNAPSHOT_RETRY_INTERVAL(sii->active_time),
	       minute_string(sii->quiet_time));
    }
    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	if (sab->data_integrity_check !=
	    SNAPSHOT_ACTIVITY_BLOCK_DATA_INTEGRITY) {
	    printf("\nSNAPSHOT: Dialer string integrity suspect at address 0x%x", 
		   sab);
	    break;
	}
	if (sab->address != SNAPSHOT_NO_DIALER_ADDR) {
	    printf("\n   For dialer address %d", sab->address);
	} else if (SNAPSHOT_IS_SERVER(sii)) {
	    addr_string[0] = '\0';
	    reg_invoke_sprintf_address_addr(sab->protocol_address.type,
					    addr_string,
					    &sab->protocol_address);
	    printf("\n   For ");
	    snapshot_show_first_protocol_update(sab);
	    printf(" address: %s", addr_string);
	}
	printf("\n    Current state: %s",
	       snapshot_run_queue_name(sab->runtime_queue));
	switch (sab->runtime_queue) {
	case SNAPSHOT_ACTIVE_Q: 
	    if (SNAPSHOT_IS_CLIENT(sii)) {
		printf(", remaining/exchange time: %d/%d minutes",
		       sab->time_left_in_this_queue,
		       sab->active_exchange_period);
	    } else {
		printf(", remaining time: %d %s",
		       sab->time_left_in_this_queue,
		       minute_string(sab->time_left_in_this_queue));
	    }
	    if (SNAPSHOT_IS_DIALER(sii)) {
		for (i = 0; i < sab->count_dialer_connected_hwidbs; i++) {
		    if (i == 0) {
			printf("\n    Connected dialer interface%s ",
			       (sab->count_dialer_connected_hwidbs > 1) ?
			       "s:" : ":");
		    }
		    if ((i % 5) == 0)
		        printf("\n       ");
		    else
		        printf(", ");
		    printf("%s",
			   sab->dialer_connected_hwidbs[i]->hw_namestring);
		}
	    }
	    snapshot_show_protocol_updates(sab);
	    break;
	case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_QUIET_Q:
	case SNAPSHOT_CLIENT_POST_ACTIVE_NEXT_ACTIVE_Q:
	case SNAPSHOT_SERVER_POST_ACTIVE_Q:
	    printf(", remaining time: %d %s",
		   sab->time_left_in_this_queue,
		   minute_string(sab->time_left_in_this_queue));
	    snapshot_show_protocol_updates(sab);
	    break;
	case SNAPSHOT_CLIENT_QUIET_Q:
	    printf(", remaining: %d %s",
		   sab->time_left_in_this_queue,
		   minute_string(sab->time_left_in_this_queue));
	    break;
	default:
	    break;
	}
    }
}

/*
 * show_snapshot
 *
 * Show Snapshot information about one or all interfaces.
 */
void show_snapshot (parseinfo *csb)
{
    idbtype *idb;
    boolean onceflag;
    snapshot_interface_info *sii;
    snapshot_active_dialer_hardware_block *sadhb;
    snapshot_activity_block *sab;
    int i;
    
    automore_enable(NULL);
    if (GETOBJ(int, 1)) {
	/* Private keyword was set */
	printf("\nSnapshot private information:");
	sadhb = snapshot_free_active_dialer_hardware_block_queue;
	i = 0;
	while (sadhb != NULL) {
	    i++;
	    sadhb = sadhb->next;
	}
	printf("\n  Length of active dialer hardware free queue: %d", i);
	sab = snapshot_free_activity_block_queue;
	i = 0;
	while (sab != NULL) {
	    i++;
	    sab = sab->next;
	}
	printf("\n  Length of activity block free queue: %d", i);
	printf("\n  Interfaces that are set for dialer timeout avoidance:");
	if (snapshot_active_dialer_hwidbs == NULL) {
	    printf("\n    None");
	} else {
	    i = 0;
	    SNAPSHOT_FOR_ALL_SADHB(sadhb) {
		if (sadhb->hwidb) {
		    printf((i == 0) ? "\n    " : ", ");
		    i = (i + 1) % 3;
		    printf("%s[%d]", sadhb->hwidb->hw_namestring,
			   sadhb->dialer_startup_remaining);
		}
	    }
	}
    } else {
	if (GETOBJ(idb, 1)) {
	    idb = GETOBJ(idb, 1);
	    sii = snapshot_find_idb(idb);
	    if (sii != NULL) {
		show_snapshot_info(sii);
	    }
	} else {
	    onceflag = FALSE;
	    SNAPSHOT_FOR_ALL_SII(sii) {
		if (onceflag) {
		    automore_conditional(0);
		} else {
		    onceflag = TRUE;
		}
		show_snapshot_info(sii);
	    }
	}
    }
    automore_disable();
}

/* -------------------- SNAPSHOT CLEAR COMMAND SUPPORT --------------------- */

/*
 * snapshot_force_idb_active
 *
 * Force the specified interface to active, if it is configured for Snapshot,
 * and it is a client interface.
 */
void snapshot_force_idb_active (idbtype *idb, boolean print_errors)
{
    snapshot_interface_info *sii = snapshot_find_idb(idb);
    snapshot_activity_block *sab;

    if (sii == NULL) {
	if (print_errors)
	    printf("\nSNAPSHOT: Not configured on interface, or interface not given");
        return;
    }

    if (SNAPSHOT_IS_CLIENT(sii)) {
	snapshot_last_forced_if_index = sii->idb->hwptr->hw_if_index;
	SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	    snapshot_make_more_active(sab, "Operator intervention");
	}
    } else {
	if (print_errors)
	    printf("\nSNAPSHOT: Interface is not the client side.");
    }
}

/*
 * snapshot_clear_quiet_period
 *
 * Interactive parser hook into snapshot_force_idb_active
 */
void snapshot_clear_quiet_period (parseinfo *csb)
{
    snapshot_force_idb_active(GETOBJ(idb, 1), TRUE);
}

/* -------------------- INTERFACE REGISTRY CALL POINTS --------------------- */

/*
 * snapshot_idb_statechange
 *
 * Called when a IDB changes state, to bring up non-dialer Snapshot
 * interfaces that are not configured for 'suppress-statechange-updates'
 * support.
 */
static void snapshot_idb_statechange (idbtype *idb)
{
    hwidbtype *hwidb;
    snapshot_interface_info *sii;
    snapshot_activity_block *sab;
    
    hwidb = hwidb_or_null(idb);
    if (hwidb == NULL)
	return;

    /*
     * Snapshot don't care about dial-on-demand virtual interfaces changing.
     * It is their REAL interfaces behind them that are interesting. For
     * example: BRI ISDN interfaces for dial on demand. Of course, with
     * 'interface dialer xx' interfaces, Snapshot is configured on those
     * interfaces, so this code will really have no effect in those
     * circumstances. Snapshot relies on the dialer callbacks, coded above, to
     * tell us when dialer connections are coming and going.
     */
    if (hwidb->status & IDB_DIALER) {
	return;
    }

    sii = snapshot_find_idb(idb);

    /* Not using it, or dialer support, which is handled elsewhere */
    if ((sii == NULL) || SNAPSHOT_IS_DIALER(sii))
	return;

    if (!interface_up(idb)) {
	SNAPSHOT_SET_IDB_DOWN(sii);
	SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	    if (sab->active_exchange_period <
		SNAPSHOT_SUCCESSFULL_EXCHANGE_MINUTES(sii)) {
		sab->active_exchange_period = 0;
	    }
	}
    } else {
	if (!SNAPSHOT_IS_IDB_UP(sii)) {
	    SNAPSHOT_SET_IDB_UP(sii);
	    /*
	     * If surpressing statechange updates, don't move to active.
	     * If server, don't move to active.
	     */
	    if (!SNAPSHOT_IS_SUPPRESS_STATECHANGE_UPDATES(sii)) {
		if (SNAPSHOT_IS_CLIENT(sii)) {
		    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
			snapshot_make_more_active(sab, "Interface up");
		    }
		}
	    }
	}
    }
}

/*
 * snapshot_delete_swidb
 *
 * Called when a SW IDB is about to be deleted.
 */
static void snapshot_delete_swidb (idbtype *idb)
{
    snapshot_interface_info *sii;
    
    sii = snapshot_find_idb(idb);

    /* Not using it; who cares ?!? */
    if (sii == NULL)
      return;

    /* Found. Chuck it. */
    snapshot_delete_usage(sii);
}

/*
 * snapshot_hw_idb_init
 *
 * Initialize Snapshot fields in the hw idb
 */
static void snapshot_hw_idb_init (hwidbtype *hwidb)
{
    hwidb->snapshot_adhb = NULL;
}

/*
 * snapshot_sw_idb_init
 *
 * Initialize Snapshot fields in the sw idb
 */
static void snapshot_sw_idb_init (idbtype *idb, boolean setup_hwidb)
{
    hwidbtype *hwidb;

    idb->sii = NULL;
    if (setup_hwidb) {
	hwidb = hwidb_or_null(idb);
	if (hwidb != NULL)
	    snapshot_hw_idb_init(hwidb);
    }
}

/* ----------------- SNAPSHOT DEFINED REGISTRY CALL POINTS ----------------- */

/*
 * snapshot_notice_update_pak
 *
 * Set the SNAPSHOT_PAK_IS_UPDATE flag on this packet, so the dialer code
 * will not allow this packet to trigger the dialer. Snapshot should be allowed
 * to control the triggering of the dialer interface, as far as update
 * packets are concerned, if it is enabled.
 */
void snapshot_notice_update_pak (paktype *pak, idbtype *idb)
{
    snapshot_interface_info *sii;

    sii = snapshot_find_idb(idb);

    if (sii == NULL)
	return;
    pak->snapshot_flags |= SNAPSHOT_PAK_IS_UPDATE;
}

/*
 * snapshot_update_denied
 *
 * Return TRUE if updates are denied on this idb at this time.
 * Updates are allowed if ANY of the queues are currently active on this
 * idb, because at the 'dialer interface' level, we have to build a full
 * 'broadcast' update if ANY of the SABs are active, as the network layer
 * doesn't see the different 'dialer' connections. The call made by dialer
 * to 'snapshot_dialer_update_denied' will keep update packets off the
 * wrong interfaces.
 *
 * 'flashupdate' is for future support of sending flash updates out, but
 * not periodic ones (a possibly bad idea...).
 */
boolean snapshot_update_denied (idbtype *idb, boolean flashupdate)
{
    snapshot_interface_info *sii = snapshot_find_idb(idb);
    snapshot_activity_block *sab;

    /* Not using it; who cares ?!? */
    if (sii == NULL) {
	return(FALSE);
    }
    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	if (SNAPSHOT_IS_IN_ACTIVE_Q(sab)) {
	    return(FALSE);
	}
    }
    return(TRUE);
}

/*
 * snapshot_dialer_update_denied
 *
 * Before sending a routing protocol update packet (this is a packet with the
 * SNAPSHOT_PAK_IS_UPDATE bit set) out of a physical interface associated with
 * a Snapshot interface, the Dialer code will call this routine to ensure that
 * the packet should go out this physical interface. Snapshot will verify this
 * physical interface is in its list of 'known' physical interfaces that are
 * attached to an SAB in the active state. This guarantees that because, one
 * 'dialer remote site' is ACTIVE with Snapshot, that the router doesn't try
 * to send packets out to other remote sites that are not active, but are
 * members of the same dialer hunt group.
 */
boolean snapshot_dialer_update_denied (hwidbtype *hwidb)
{
    hwidbtype *master_hwidb;
    snapshot_interface_info *sii;
    snapshot_activity_block *sab;
    int i;

    if (hwidb == NULL)
        return(FALSE);

    /* Get hunt group master, which may be this interface itself */
    master_hwidb = get_netidb(hwidb);
	 
    sii = snapshot_find_idb(firstsw_or_null(master_hwidb));

    /* If Snapshot not used, or this is not a dialer interface, ignore */
    if ((sii == NULL) || (!SNAPSHOT_IS_DIALER(sii)))
        return(FALSE);

    /* Examine only active SABs.. */
    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	if (SNAPSHOT_IS_IN_ACTIVE_Q(sab)) {
	    for (i = 0; i < sab->count_dialer_connected_hwidbs; i++) {
		/* Is this HWIDB one of the ones active with this SAB? */
		if (sab->dialer_connected_hwidbs[i] == hwidb)
		    return(FALSE);
	    }
	}
    }

    /* No decent interface found. Sorry.. */
    return(TRUE);
    
    
}

/*
 * snapshot_enabled
 *
 * Return TRUE if Snapshot enabled on a idb
 */
boolean snapshot_enabled (idbtype *idb)
{
    return((snapshot_find_idb(idb) != NULL) ? TRUE : FALSE);
}

/*
 * snapshot_aging_denied
 *
 * Return TRUE if aging routes learned on this idb is not to be done.
 */
boolean snapshot_aging_denied (idbtype *idb)
{
    /* Don't age if there is Snapshot configured at all */
    return(snapshot_enabled(idb));
}

/*
 * snapshot_update_received
 *
 * A routing protocol update was received on this interface.
 */
void snapshot_update_received (idbtype *idb, paktype *pak)
{
    addrtype protocol_address;
    snapshot_interface_info *sii = snapshot_find_idb(idb);
    snapshot_activity_block *sab;
    int i;
    boolean found;

    /* Not using it; who cares ?!? */
    if (sii == NULL) {
	return;
    }
    
    sab = NULL;
    if (SNAPSHOT_IS_SERVER(sii)) {
	/*
	 * Okay. Find the protocol address that matches this packet. If
	 * found, work with this entry. If not found, create a new one,
	 * and put it in the active queue.
	 */
	reg_invoke_build_packet_address(pak->linktype, pak, &protocol_address,
					SOURCE);
	SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
	    if (!bcmp((uchar *) &protocol_address,
		      (uchar *) &(sab->protocol_address),
		      sizeof(addrtype)))
	      break;
	}
	if (sab == NULL) {
	    sab = snapshot_create_activity_block(sii,
						 SNAPSHOT_SERVER_DIALER_ADDR,
						 &protocol_address);
	}
    } else {
	if (SNAPSHOT_IS_DIALER(sii)) {
	    found = FALSE;
	    /* Find currently connected address from dialer... */
	    SNAPSHOT_FOR_ALL_SAB_ON_SII(sab, sii) {
		for (i = 0; i < sab->count_dialer_connected_hwidbs; i++) {
		    /* Is this HWIDB one of the ones active with this SAB? */
		    if (sab->dialer_connected_hwidbs[i] == pak->dialer_input) {
			found = TRUE;
			break;
		    }
		}
		if (found)
		  break;
	    }
	    if (!found)
	      sab = NULL;
	} else {
	    sab = snapshot_find_activity_block(sii, SNAPSHOT_NO_DIALER_ADDR);
	}
    }
    if (sab == NULL)
      return;

    if (SNAPSHOT_IS_IN_ACTIVE_Q(sab)) {
	if (SNAPSHOT_IS_CLIENT(sii)) {
	    if (sab->active_exchange_period == 0) {
		/*
		 * Three minute warning! The active period should be
		 * 'productive' for at least three minutes, or half the
		 * active period, whichever is greater.
		 */
		sab->active_exchange_period = 1;
	    }
	} else if (SNAPSHOT_IS_DIALER(sii)) {
	    /* Possibly add this hwidb to list of ones this SAB is using... */
	    if (pak->dialer_input != NULL) {
		if (!SNAPSHOT_CHECK_PROTOCOL_UPDATES_EXCHANGED
		    (sab, pak->linktype)) {
		    /*
		     * Servers have the timeout period set to the active time
		     * upon receipt of the first update.
		     */
		    snapshot_set_dialer_stays_up(pak->dialer_input,
						 sab->sii->active_time);
		}
		snapshot_dialer_hwidbs_on_sab_statechange(sab,
							  pak->dialer_input,
							  TRUE);
	    }
	}
	/* Age this protocol, from this source... */
	if (!SNAPSHOT_CHECK_PROTOCOL_UPDATES_EXCHANGED(sab, pak->linktype)) {
	    reg_invoke_snapshot_start_aging_routes(pak->linktype, sii->idb,
						   pak);
	    SNAPSHOT_SET_PROTOCOL_UPDATES_EXCHANGED(sab, pak->linktype);
	    if (snapshot_debug)
	      buginf("\nSNAPSHOT: %s[%d]: Starting aging of %s protocol", 
		     sab->sii->idb->namestring, sab->address,
		     print_linktype(pak->linktype));
	}
    }
}

/* -------------------- INITIALIZE SNAPSHOT SUPPORT ------------------------ */

/*
 * snapshot_init
 *
 * Initialize Snapshot support.
 */
void snapshot_init (subsystype *subsys)
{
    int i;

    snapshot_interface_info_index = 0;
    snapshot_debug_init();
    
    queue_init(&snapshot_interfaces, 0);
    snapshot_init_activity_block_queue();
    snapshot_init_active_dialer_hardware_block_queue();
    for (i = 0; i < SNAPSHOT_QUEUE_COUNT; i++) {
	snapshot_run_queues[i] = NULL;
    }
    reg_add_swif_erase(snapshot_delete_swidb, "snapshot_delete_swidb");
    reg_add_route_adjust(snapshot_idb_statechange, "snapshot_idb_statechange");
    reg_add_snapshot_notice_update_pak(snapshot_notice_update_pak,
				       "snapshot_notice_update_pak");
    reg_add_snapshot_update_denied(snapshot_update_denied,
					    "snapshot_update_denied");
    reg_add_snapshot_dialer_update_denied(snapshot_dialer_update_denied,
					  "snapshot_dialer_update_denied");
    reg_add_snapshot_update_received(snapshot_update_received,
				     "snapshot_update_received");
    reg_add_snapshot_aging_denied(snapshot_aging_denied,
				  "snapshot_aging_denied");
    reg_add_snapshot_enabled(snapshot_enabled, "snapshot_enabled");
    reg_add_dialer_map_change(LINK_SNAPSHOT, snapshot_dialer_map_change,
			      "snapshot_dialer_map_change");
    reg_add_dialer_number_called_changed
      (LINK_SNAPSHOT, snapshot_dialer_number_called_change,
       "snapshot_dialer_number_called_change");
    reg_add_dialer_active(snapshot_dialer_active, "snapshot_dialer_active");
    reg_add_hw_idb_init(snapshot_hw_idb_init, "snapshot_hw_idb_init");
    reg_add_sw_idb_init(snapshot_sw_idb_init, "snapshot_sw_idb_init");
    reg_add_onesec(snapshot_dialer_timer_reset, "snapshot_dialer_timer_reset");
    snapshot_parser_init();
    snapshot_media_registry();

    /*
     * remove temporarily while updated snapshot mib is being implemented
     *    init_snapshotmib();
     */
}

/* ----------------------- SNAPSHOT SUBSYSTEM HEADER ----------------------- */

#define SNAPSHOT_MAJVERSION 1
#define SNAPSHOT_MINVERSION 1
#define SNAPSHOT_EDITVERSION  2

SUBSYS_HEADER(snapshot, SNAPSHOT_MAJVERSION,
	      SNAPSHOT_MINVERSION, SNAPSHOT_EDITVERSION,
	      snapshot_init, SUBSYS_CLASS_LIBRARY,
	      NULL,
	      NULL);
