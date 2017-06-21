/* $Id: cfg_translate_lat_out.h,v 3.2 1995/11/17 18:58:53 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/pt/cfg_translate_lat_out.h,v $
 *------------------------------------------------------------------
 * LAT translate out command
 *
 * December 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_translate_lat_out.h,v $
 * Revision 3.2  1995/11/17  18:58:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:00:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:26  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:14:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_EXIT(trans_out_lat_return, no_alt);

/****************************************************************
 *  lat and options
 *
 * translate lat <string> [ node <string> | port <string> | unadvertised ]
 *
 * lat <string> = OBJ(string,4)
 * node <string> = OBJ(string,5)
 * port <string> = OBJ(string,6)
 * unadvertised = OBJ(int,7)
 */
pdecl(trans_out_lat_opts);
KEYWORD_ID(trans_out_lat_unadv, trans_out_lat_opts, NONE,
	   OBJ(int,7), TRUE,
	   "unadvertised",
	  "Prevent service advertisements from being broadcast to the network",
	   PRIV_CONF);
STRING	(trans_out_lat_port_name, trans_out_lat_opts, no_alt,
	 OBJ(string,6), "LAT port name");
KEYWORD	(trans_out_lat_port, trans_out_lat_port_name, trans_out_lat_unadv,
	 "port", "LAT port name", PRIV_CONF);
STRING	(trans_out_lat_node_name, trans_out_lat_opts, no_alt,
	 OBJ(string,5), "LAT node name");
KEYWORD	(trans_out_lat_node, trans_out_lat_node_name, trans_out_lat_port,
	 "node", "LAT node name", PRIV_CONF);
NOP	(trans_out_lat_opts, trans_out_lat_node, trans_out_lat_return);

/*************************************************************/
STRING	(trans_out_lat_name, trans_out_lat_opts, no_alt,
	 OBJ(string,4), "LAT service name");
KEYWORD_ID(trans_out_lat, trans_out_lat_name, ALTERNATE,
	   OBJ(int,6), TRANS_PROT_LAT,
	   "lat", "DEC LAT protocol", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	trans_out_lat
