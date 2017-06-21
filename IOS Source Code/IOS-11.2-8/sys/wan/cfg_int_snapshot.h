/* $Id: cfg_int_snapshot.h,v 3.2.62.1 1996/08/12 16:09:32 widmer Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_snapshot.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ S N A P S H O T . H
 *
 * Support for snapshot routes
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_snapshot.h,v $
 * Revision 3.2.62.1  1996/08/12  16:09:32  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  18:01:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:48:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:45:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:07:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/***************************************************************
 *	snapshot client <active-time> <quiet-time>
 *		[suppress-statechange-update] [dialer]
 *	no snapshot client
 *
 *	snapshot server <active-time> [dialer]
 *      no snapshot server
 *
 *	OBJ(int,1) = keyword choice
 *	OBJ(int,2) = active time
 *	OBJ(int,3) = quiet time
 *	OBJ(int,4) = TRUE if suppressing state change updates
 *	OBJ(int,5) = TRUE if dialer interface
 */
EOLNS	(snapshot_eol, snapshotif_command);

/* Support for server */
KEYWORD_ID (snapshot_dialer, snapshot_eol,
	    snapshot_eol, OBJ(int,5), TRUE,
	    SNAPSHOT_KEYWORD_DIALER,
	    "Use Snapshot dialer maps for connectivity information",
	    PRIV_CONF|PRIV_SUBIF);

NUMBER	(snapshot_server_active_time, snapshot_dialer, no_alt, OBJ(int,2),
	 SNAPSHOT_ACTIVE_INTERVAL_MIN, SNAPSHOT_ACTIVE_INTERVAL_MAX,
	 "duration, in minutes, of each active period");

NOPREFIX (snapshot_server_noprefix, snapshot_server_active_time,
	  snapshot_eol);

KEYWORD_ID (snapshot_keyword_server, snapshot_server_noprefix, no_alt,
	    OBJ(int, 1), SNAPSHOT_SERVER, SNAPSHOT_KEYWORD_SERVER,
	    "Send routing updates out this link when updates are received",
	    PRIV_CONF|PRIV_SUBIF);

/* Support for client */

KEYWORD_ID (snapshot_suppress_statechange_updates, snapshot_dialer,
	    snapshot_dialer, OBJ(int,4), TRUE,
	    SNAPSHOT_KEYWORD_SUPPRESS_STATECHANGE_UPDATES,
	    "Suppress start of active interval every time line protocol goes up",
	    PRIV_CONF|PRIV_SUBIF);

/*
 * The minimum allowed quiet time should be well above the value of
 * SNAPSHOT_SERVER_EXTRA_MINUTES
 */
NUMBER	(snapshot_client_quiet_time,
	 snapshot_suppress_statechange_updates, no_alt, OBJ(int,3),
	 SNAPSHOT_QUIET_INTERVAL_MIN, SNAPSHOT_QUIET_INTERVAL_MAX,
	 "duration, in minutes, of quiet time between active periods");

NUMBER	(snapshot_client_active_time, snapshot_client_quiet_time, no_alt,
	 OBJ(int,2), SNAPSHOT_ACTIVE_INTERVAL_MIN, SNAPSHOT_ACTIVE_INTERVAL_MAX,
	 "duration, in minutes, of each active period");

NOPREFIX (snapshot_client_noprefix, snapshot_client_active_time, snapshot_eol);

KEYWORD_ID (snapshot_keyword_client, snapshot_client_noprefix,
	    snapshot_keyword_server, OBJ(int, 1), SNAPSHOT_CLIENT,
	    SNAPSHOT_KEYWORD_CLIENT,
	    "Enable client control of Snapshot routing",
	    PRIV_CONF|PRIV_SUBIF);

NVGENNS	(snapshot_nvgen, snapshot_keyword_client, snapshotif_command);

KEYWORD_MM (snapshot_main_command, snapshot_nvgen, ALTERNATE,
			SNAPSHOT_KEYWORD, SNAPSHOT_INTERFACE_HELP,
			PRIV_CONF|PRIV_SUBIF, SNAPSHOT_UNIQ);

#undef	ALTERNATE
#define	ALTERNATE	snapshot_main_command
