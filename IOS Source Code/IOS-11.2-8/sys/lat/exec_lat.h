/* $Id: exec_lat.h,v 3.2.62.1 1996/08/12 16:04:22 widmer Exp $
 * $Source: /release/112/cvs/Xsys/lat/exec_lat.h,v $
 *------------------------------------------------------------------
 * E X E C _ L A T . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: exec_lat.h,v $
 * Revision 3.2.62.1  1996/08/12  16:04:22  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  17:32:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:25:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:53:47  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:21:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	LAT_NONE_SET	0
#define	LAT_NODE_SET	1<<1
#define	LAT_PORT_SET	1<<2
#define	LAT_DEBUG_SET	1<<3
#define	LAT_EOL_SET	1<<4

/******************************************************************
 * lat <name> [node <nodename>] [port <portname>] [/debug]
 *
 * <name>	= OBJ(string,1)
 * <nodename>	= OBJ(string,2)
 * <portname>	= OBJ(string,3)
 *        OBJ(int,1) =  TRUE if /debug is set
 *        	  FALSE if /debug is not set
 */
pdecl(exec_lat_options);

EOLNS	(exec_lat_eol, lat_connect_command);

SET	(exec_lat_debug_set, exec_lat_options, OBJ(int,1), TRUE);
KEYWORD_ORTEST(exec_lat_debug, exec_lat_debug_set, exec_lat_eol,
	       OBJ(int,2), LAT_DEBUG_SET,
	       "/debug", "Print parameter changes and special messages",
	       PRIV_USER);


STRING	(exec_lat_port_name, exec_lat_options, no_alt,
	 OBJ(string,3), "Destination LAT port name");
KEYWORD_ORTEST(exec_lat_port, exec_lat_port_name, exec_lat_debug,
	       OBJ(int,2), LAT_PORT_SET,
	       "port", "Destination LAT port name", PRIV_USER);


STRING	(exec_lat_node_name, exec_lat_options, no_alt,
	 OBJ(string,2), "Name of a specific LAT node");
KEYWORD_ORTEST(exec_lat_node, exec_lat_node_name, exec_lat_port,
	       OBJ(int,2), LAT_NODE_SET,
	       "node", "Connect to a specific LAT node", PRIV_USER);


NOP	(exec_lat_options, exec_lat_node, NONE);


STRING	(exec_lat_name, exec_lat_options, no_alt,
	 OBJ(string,1), "Name of a remote system");
KEYWORD	(exec_lat_command, exec_lat_name, ALTERNATE,
	 "lat", "Open a lat connection", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	exec_lat_command
