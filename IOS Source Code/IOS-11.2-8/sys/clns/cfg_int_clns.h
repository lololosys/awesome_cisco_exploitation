/* $Id: cfg_int_clns.h,v 3.2 1995/11/17 08:51:23 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/clns/cfg_int_clns.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ C L N S . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_clns.h,v $
 * Revision 3.2  1995/11/17  08:51:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:06:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:56  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:04:10  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/***************************************************************
 * clns esct-time <value>
 * no clns esct-time
 *
 * OBJ(int,1) = value in seconds
 */
EOLS (clns_esct_time_eols, clns_intfc_command, CLNS_ESCT_TIME);
NUMBER	(clns_esct_time_value, clns_esct_time_eols, no_alt,
	 OBJ(int,1), 0, 0xffff, "Configuration time in seconds");
NOPREFIX(clns_esct_time_noprefix, clns_esct_time_value, clns_esct_time_eols);
NVGENS (clns_esct_time_nvgen, clns_esct_time_noprefix, clns_intfc_command,
	CLNS_ESCT_TIME);
KEYWORD (clns_esct_time, clns_esct_time_nvgen, no_alt, "esct-time",
	 "Suggested ES configuration time", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * clns adjacency-filter {es | is} <name>
 * no clns adjacency-filter {es | is}
 *
 * OBJ(int,1) = 0 for ES, 1 for IS
 * OBJ(string,1) = filter name
 */
EOLS    (clns_adj_fltr_eol, clns_intfc_command, CLNS_ADJ_FILTER);
STRING  (clns_adj_fltr_name, clns_adj_fltr_eol, no_alt, OBJ(string,1),
	 "Name of filter expression or set");
NOPREFIX(clns_adj_fltr_no, clns_adj_fltr_name, clns_adj_fltr_eol);
KEYWORD_ID(clns_adj_fltr_is, clns_adj_fltr_no, no_alt, OBJ(int,1),
	   1, "IS", "Filter IS adjacencies", PRIV_CONF|PRIV_SUBIF);
KEYWORD (clns_adj_fltr_es, clns_adj_fltr_no, clns_adj_fltr_is,
	 "ES", "Filter ES adjacencies", PRIV_CONF|PRIV_SUBIF);
NVGENS  (clns_adj_fltr_nvgen, clns_adj_fltr_es, clns_intfc_command,
	 CLNS_ADJ_FILTER);
KEYWORD (clns_adj_fltr, clns_adj_fltr_nvgen, clns_esct_time, 
	 "adjacency-filter", "Filter adjacencies", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no]  clns split-horizon
 *
 */
EOLS (clns_int_split_eols, clns_intfc_command, CLNS_SPLIT_CMD);
KEYWORD (clns_int_split, clns_int_split_eols, clns_adj_fltr, 
	 "split-horizon", "Enable ISO-IGRP split horizon",
	 PRIV_CONF|PRIV_SUBIF);	

/***************************************************************
 * clns access-group gname {in|[out]}
 * no clns access-group {in|[out]}
 *
 * OBJ(string,1) = group name
 * OBJ(int,1) = TRUE if "in", FALSE if "out" (default)
 *
 */
EOLS    (clns_int_accgrp_eol, clns_intfc_command, CLNS_ACCESS_GROUP);
KEYWORD (clns_int_accgrp_out, clns_int_accgrp_eol, clns_int_accgrp_eol, "out",
	 "Filter output PDUs", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_int_accgrp_in, clns_int_accgrp_eol, clns_int_accgrp_out, 
	   OBJ(int,1), TRUE, "in", "Filter input PDUs", PRIV_CONF|PRIV_SUBIF);
STRING  (clns_int_accgrp_gname, clns_int_accgrp_in, no_alt, OBJ(string,1),
	 "Name of filter expression or set");
STRING  (no_clns_int_accgrp_gname, clns_int_accgrp_in, clns_int_accgrp_eol, 
	 OBJ(string,1), "Name of filter expression or set");
TEST_MULTIPLE_FUNCS(clns_int_accgrp_ambig, no_clns_int_accgrp_gname,
		    clns_int_accgrp_eol, NONE);
KEYWORD (no_clns_int_accgrp_out, clns_int_accgrp_eol, clns_int_accgrp_ambig,
	 "out", "Filter output PDUs", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(no_clns_int_accgrp_in, clns_int_accgrp_eol, no_clns_int_accgrp_out, 
	   OBJ(int,1), TRUE, "in", "Filter input PDUs", PRIV_CONF|PRIV_SUBIF);
TESTVAR (clns_int_accgrp_noprefix, clns_int_accgrp_gname, 
	 no_clns_int_accgrp_in, NONE, NONE, NONE, sense, TRUE);
NVGENS  (clns_int_accgrp_nvgen, clns_int_accgrp_noprefix, clns_intfc_command,
	 CLNS_ACCESS_GROUP);
KEYWORD (clns_int_accgrp, clns_int_accgrp_nvgen, clns_int_split, 
	 "access-group", "Filter inbound or outbound CLNS traffic", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns send-rdpdu
 *
 */
EOLS	(clns_int_send_rdpdu_eol, clns_intfc_command, CLNS_SEND_RDPDU);
KEYWORD	(clns_int_send_rdpdu, clns_int_send_rdpdu_eol, clns_int_accgrp,
	 "send-rdpdu", "Send redirect PDUs to optimize routing",
	 PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns send-erpdu
 *
 */
EOLS	(clns_int_send_erpdu_eol, clns_intfc_command, CLNS_SEND_ERPDU);
KEYWORD	(clns_int_send_erpdu, clns_int_send_erpdu_eol, clns_int_send_rdpdu,
	 "send-erpdu",
	 "Send error PDUs when an error is detected in a data PDU",
	 PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns route-cache
 * [no] clns route-cache sse
 *
 * OBJ(int,1) = 0 by default
 * 		CLNS_SSE_ROUTECACHE if 'sse' was specified
 */
EOLS	(clns_int_route_cache_eol, clns_intfc_command, CLNS_ROUTEC);
KEYWORD_ID (clns_int_route_cache_sse, clns_int_route_cache_eol,
	    clns_int_route_cache_eol, OBJ(int,1), CLNS_SSE_ROUTECACHE,
	    "sse", "Enable SSE fast-switching cache", PRIV_CONF);
KEYWORD	(clns_int_route_cache, clns_int_route_cache_sse, clns_int_send_erpdu,
	 "route-cache", "Enable fast switching through the cache", PRIV_CONF);

/***************************************************************
 * clns router isis <name>
 * clns router { igrp | iso-irgp | static } <name> [level 2]
 *
 */
EOLS	(clns_int_router_eol, clns_intfc_command, CLNS_ROUTER);

KEYWORD_ID(clns_int_router_level2, clns_int_router_eol, no_alt,
	   OBJ(int,2), 2,
	   "2", "Advertise Level 2 information only", PRIV_CONF|PRIV_SUBIF);
KEYWORD(clns_int_router_level, clns_int_router_level2, NONE,
	"level", "Advertise Level 2 information only", PRIV_CONF|PRIV_SUBIF);
TESTVAR	(clns_int_router_level_test, NONE, clns_int_router_level,
	 NONE, NONE, clns_int_router_eol, OBJ(int,1), PROTOCOL_ISIS);

STRING	(clns_int_router_name, clns_int_router_level_test,
	 clns_int_router_level_test,
	 OBJ(string,1), "Routing process tag");

KEYWORD_ID(clns_int_router_isis, clns_int_router_name, no_alt,
	   OBJ(int,1), PROTOCOL_ISIS,
	   "isis", "ISO IS-IS routing protocol", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_int_router_isoigrp, clns_int_router_name, clns_int_router_isis,
	   OBJ(int,1), PROTOCOL_ISO_IGRP,
	   "iso-igrp", "ISO-IGRP routing protocol", PRIV_CONF|PRIV_SUBIF);
NVGENS	(clns_int_router_nvgen, clns_int_router_isoigrp,
	 clns_intfc_command, CLNS_ROUTER);
KEYWORD	(clns_int_router, clns_int_router_nvgen, clns_int_route_cache,
	 "router", "Enable CLNS routing protocol on interface", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns rdpdu-mask
 *
 * Temporarily disabled until the functionality is implemented.
 *
EOLS	(clns_int_rpdu_mask_eol, clns_intfc_command, CLNS_RDPDU_MASK);
KEYWORD	(clns_int_rpdu_mask, clns_int_rpdu_mask_eol, clns_int_router,
	 "rdpdu-mask", "Enable the address mask for Redirect PDUs", PRIV_CONF|PRIV_SUBIF);
*/

/***************************************************************
 * clns rdpdu-interval <num>
 * no clns rdpdu-interval
 */
PARAMS_KEYONLY(clns_int_rpdu_interval, clns_int_router, "rdpdu-interval",
	       OBJ(int,1), 0, -1,
	       clns_intfc_command, CLNS_RDPDU_INTRVL,
	       "Define the minimum interval between Redirect PDUs",
	       "Minimum interval between Redirect PDUs in milliseconds",
	       PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * clns mtu <num>
 * no clns mtu
 */
PARAMS_KEYONLY(clns_int_mtu, clns_int_rpdu_interval, "mtu",
	       OBJ(int,1), CLNS_MINMTU, -1,
	       clns_intfc_command, CLNS_MTU,
	       "Set MTU packet size for interface",
	       "CLNS MTU packet size for interface", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * clns { is-neighbor | es-neighbor } <clnsaddr> <addr> [--- options ---]
 * no clns { is-neighbor | es-neighbor } <clnsaddr>
 *
 *
 */
EOLS	(clns_int_neighbor_eol, clns_intfc_command, CLNS_ES_NEIGHBOR);

pdecl(clns_opts);

NUMBER	(clns_opts_window_out, clns_opts, no_alt,
	 OBJ(int,9), 1, -1, "Output window size");
NUMBER	(clns_opts_window_in, clns_opts_window_out, no_alt,
	 OBJ(int,8), 1, -1, "Input window size");
KEYWORD_OR(clns_opts_window, clns_opts_window_in, clns_int_neighbor_eol,
	   OBJ(int,2), FLAG_WINDOW,
	   "windowsize", "Specify window sizes", PRIV_CONF|PRIV_SUBIF);

NUMBER	(clns_opts_transit_num, clns_opts, no_alt,
	 OBJ(int,10), 0, 65534, "Transit delay");
KEYWORD_OR(clns_opts_transit, clns_opts_transit_num, clns_opts_window,
	   OBJ(int,2), FLAG_TDELAY,
	   "transit-delay", "Specify transit delay", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(clns_opts_throughput_num2_48000, clns_opts, no_alt,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_48000,
	   "48000", "48000 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_19200, clns_opts,
	   clns_opts_throughput_num2_48000,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_19200,
	   "19200", "19200 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_9600, clns_opts,
	   clns_opts_throughput_num2_19200,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_9600,
	   "9600", "9600 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_4800, clns_opts,
	   clns_opts_throughput_num2_9600,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_4800,
	   "4800", "4800 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_2400, clns_opts,
	   clns_opts_throughput_num2_4800,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_2400,
	   "2400", "2400 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_1200, clns_opts,
	   clns_opts_throughput_num2_2400,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_1200,
	   "1200", "1200 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_600, clns_opts,
	   clns_opts_throughput_num2_1200,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_600,
	   "600", "600 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_300, clns_opts,
	   clns_opts_throughput_num2_600,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_300,
	   "300", "300 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_150, clns_opts,
	   clns_opts_throughput_num2_300,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_150,
	   "150", "150 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num2_75, clns_opts,
	   clns_opts_throughput_num2_150,
	   OBJ(int,12), X25_FACILITY_THROUGHPUT_75,
	   "75", "75 bps output rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_48000, clns_opts_throughput_num2_75,
	   no_alt, OBJ(int,11), X25_FACILITY_THROUGHPUT_48000,
	   "48000", "48000 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_19200, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_48000,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_19200,
	   "19200", "19200 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_9600, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_19200,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_9600,
	   "9600", "9600 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_4800, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_9600,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_4800,
	   "4800", "4800 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_2400, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_4800,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_2400,
	   "2400", "2400 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_1200, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_2400,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_1200,
	   "1200", "1200 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_600, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_1200,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_600,
	   "600", "600 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_300, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_600,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_300,
	   "300", "300 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_150, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_300,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_150,
	   "150", "150 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_throughput_num1_75, clns_opts_throughput_num2_75,
	   clns_opts_throughput_num1_150,
	   OBJ(int,11), X25_FACILITY_THROUGHPUT_75,
	   "75", "75 bps input rate", PRIV_CONF|PRIV_SUBIF);
KEYWORD_OR(clns_opts_throughput, clns_opts_throughput_num1_75, clns_opts_transit,
	   OBJ(int,2), FLAG_THROUGHPUT,
	   "throughput", "Request bandwidth in X.25 network", PRIV_CONF|PRIV_SUBIF);

STRING	(clns_opts_rpoa_name, clns_opts, no_alt,
	 OBJ(string,3), "RPOA string");
KEYWORD_OR(clns_opts_rpoa, clns_opts_rpoa_name, clns_opts_throughput,
	   OBJ(int,2), FLAG_RPOA,
	   "rpoa", "Specify RPOA", PRIV_CONF|PRIV_SUBIF);

KEYWORD_OR(clns_opts_reverse, clns_opts, clns_opts_rpoa,
	   OBJ(int,2), FLAG_OREVERSE,
	   "reverse", "Use reverse charging on outgoing calls", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(clns_opts_psout_num4096, clns_opts, no_alt,
	   OBJ(int,7), 12, "4096", "4096 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num2048, clns_opts, clns_opts_psout_num4096,
	   OBJ(int,7), 11, "2048", "2048 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num1024, clns_opts, clns_opts_psout_num2048,
	   OBJ(int,7), 10, "1024", "1024 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num512, clns_opts, clns_opts_psout_num1024,
	   OBJ(int,7), 9, "512", "512 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num256, clns_opts, clns_opts_psout_num512,
	   OBJ(int,7), 8, "256", "256 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num128, clns_opts, clns_opts_psout_num256,
	   OBJ(int,7), 7, "128", "128 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num64, clns_opts, clns_opts_psout_num128,
	   OBJ(int,7), 6, "64", "64 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num32, clns_opts, clns_opts_psout_num64,
	   OBJ(int,7), 5, "32", "32 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psout_num16, clns_opts, clns_opts_psout_num32,
	   OBJ(int,7), 4, "16", "16 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num4096, clns_opts_psout_num16, no_alt,
	   OBJ(int,6), 12, "4096", "4096 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num2048, clns_opts_psout_num16,
	   clns_opts_psin_num4096,
	   OBJ(int,6), 11, "2048", "2048 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num1024, clns_opts_psout_num16,
	   clns_opts_psin_num2048,
	   OBJ(int,6), 10, "1024", "1024 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num512, clns_opts_psout_num16,
	   clns_opts_psin_num1024,
	   OBJ(int,6), 9, "512", "512 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num256, clns_opts_psout_num16, clns_opts_psin_num512,
	   OBJ(int,6), 8, "256", "256 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num128, clns_opts_psout_num16, clns_opts_psin_num256,
	   OBJ(int,6), 7, "128", "128 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num64, clns_opts_psout_num16, clns_opts_psin_num128,
	   OBJ(int,6), 6, "64", "64 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num32, clns_opts_psout_num16, clns_opts_psin_num64,
	   OBJ(int,6), 5, "32", "32 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_psin_num16, clns_opts_psout_num16, clns_opts_psin_num32,
	   OBJ(int,6), 4, "16", "16 bit packet size", PRIV_CONF|PRIV_SUBIF);
KEYWORD_OR(clns_opts_packetsize, clns_opts_psin_num16, clns_opts_reverse,
	   OBJ(int,2), FLAG_PACKET,
	   "packetsize", "Specify maximum packet sizes", PRIV_CONF|PRIV_SUBIF);

NUMBER	(clns_opts_nvc_num, clns_opts, no_alt,
	 OBJ(int,5), 1, 8, "XX Need some help here");
KEYWORD_OR(clns_opts_nvc, clns_opts_nvc_num,  clns_opts_packetsize,
	   OBJ(int,2), FLAG_NVC,
	   "nvc", "Set number of virtual circuits for this map", PRIV_CONF|PRIV_SUBIF);

KEYWORD_OR(clns_opts_nopbp, clns_opts, clns_opts_nvc,
	   OBJ(int,2), FLAG_NOPBP,
	   "no-compress", "Specify No Packet By Packet Compression",
	   PRIV_CONF|PRIV_SUBIF);

KEYWORD_OR(clns_opts_pbp, clns_opts, clns_opts_nopbp,
	   OBJ(int,2), FLAG_PBP,
	   "compress", "Specify Packet By Packet Compression", PRIV_CONF|PRIV_SUBIF);

STRING	(clns_opts_nudata_data, clns_opts, no_alt,
	 OBJ(string,1), "User formatted network user ID");
KEYWORD_OR(clns_opts_nudata, clns_opts_nudata_data, clns_opts_pbp,
	   OBJ(int,2), FLAG_NUDATA,
	   "nudata", "Specify user formatted network user ID", PRIV_CONF|PRIV_SUBIF);

STRING	(clns_opts_nuid_pw, clns_opts, no_alt,
	 OBJ(string,2), "Password");
STRING	(clns_opts_nuid_user, clns_opts_nuid_pw, no_alt,
	 OBJ(string,1), "Username");
KEYWORD_OR(clns_opts_nuid, clns_opts_nuid_user, clns_opts_nudata,
	   OBJ(int,2), FLAG_NUID,
	   "nuid", "Specify Cisco formatted network user ID", PRIV_CONF|PRIV_SUBIF);

KEYWORD_ID(clns_opts_modulo_num2, clns_opts, no_alt,
	   OBJ(int,4), 128, "128", "Extended sequencing", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_opts_modulo_num1, clns_opts, clns_opts_modulo_num2,
	   OBJ(int,4), 8, "8", "Normal sequencing", PRIV_CONF|PRIV_SUBIF);
KEYWORD_OR(clns_opts_modulo, clns_opts_modulo_num1, clns_opts_nuid,
	   OBJ(int,2), FLAG_MODULO,
	   "modulo", "Specify maximum window size", PRIV_CONF|PRIV_SUBIF);

NUMBER	(clns_opts_cug_num, clns_opts, no_alt,
	 OBJ(int,3), 1, 100, "CUG number");
KEYWORD_OR(clns_opts_cug, clns_opts_cug_num, clns_opts_modulo,
	   OBJ(int,2), FLAG_CUG,
	   "cug", "Specify a Closed User Group number", PRIV_CONF|PRIV_SUBIF);

KEYWORD_OR(clns_opts_broadcast, clns_opts, clns_opts_cug,
	   OBJ(int,2), FLAG_BROAD,
	   "broadcast", "Output broadcasts to this X.121 address", PRIV_CONF|PRIV_SUBIF);

KEYWORD_OR(clns_opts_acc, clns_opts, clns_opts_broadcast,
	   OBJ(int,2), FLAG_IREVERSE,
	   "accept-reverse", "Accepting incoming reverse-charged calls", 
	   PRIV_CONF|PRIV_SUBIF);
NOP (clns_opts, clns_opts_acc, NONE);

SNPAADDR(clns_int_neighbor_addr3, clns_opts, no_alt,
	 OBJ(hwaddr,1), "Subnetwork (SNPA) address");

IFELSE (clns_int_neighbor_addr2, clns_int_neighbor_addr3, clns_int_neighbor_eol,
	!is_p2p(GETOBJ(idb, 1)));

EVAL	(clns_int_neighbor_eval, clns_int_neighbor_addr2,
	 (GETOBJ(idb,1) = GETOBJ(idb,current)));
NOPREFIX(clns_int_neighbor_noprefix, clns_int_neighbor_eval,
	 clns_int_neighbor_eol);
NSAPADDR(clns_int_neighbor_addr1, clns_int_neighbor_noprefix, no_alt,
	 OBJ(paddr,1), "CLNS address or host name");
NVGENS	(clns_int_neighbor_nvgen, clns_int_neighbor_addr1,
	 clns_intfc_command, CLNS_ES_NEIGHBOR);
KEYWORD_ID(clns_int_is, clns_int_neighbor_nvgen, clns_int_mtu,
	   OBJ(int,1), CLNS_IS_NEIGHBOR,
	   "is-neighbor",
	   "Intermediate system SNPA static mapping configuration",
	   PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID(clns_int_es, clns_int_neighbor_nvgen, clns_int_is,
	   OBJ(int,1), CLNS_ES_NEIGHBOR,
	   "es-neighbor",
	   "End system SNPA static mapping configuration",
	   PRIV_CONF|PRIV_SUBIF);


/***************************************************************
 * clns erpdu-interval <num>
 * no clns erpdu-interval
 */
PARAMS_KEYONLY (clns_int_erpdu, clns_int_es, "erpdu-interval",
		OBJ(int,1), 0, -1,
		clns_intfc_command, CLNS_ERPDU_INTRVL,
		"Minimum interval between ERPDUs",
		"Number of milliseconds between ERPDUs", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns dec-compatible
 *
 */
EOLS    (clns_int_dec_eol, clns_intfc_command, CLNS_DEC_COMPAT);
KEYWORD	(clns_int_dec, clns_int_dec_eol, clns_int_erpdu,
	 "dec-compatible ", "Received NSAPs do not contain N-selector octet", 
	 PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns enable
 *
 */
EOLS	(clns_int_enable_eol, clns_intfc_command, CLNS_ENABLE);
KEYWORD	(clns_int_enable, clns_int_enable_eol, clns_int_dec,
	 "enable", "Enable CLNS and ES-IS routing on interface", PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * clns congestion-threshold <num>
 * no clns congestion-threshold
 */
PARAMS_KEYONLY(clns_int_congestion, clns_int_enable, "congestion-threshold",
	       OBJ(int,1), 0, -1,
	       clns_intfc_command, CLNS_CNGST_THRESH,
	       "Set the congestion experience bit if the output queue"
	       " contains more than the specified number of packets",
	       "Number of packets before setting the congestion"
	       " experience bit",
	       PRIV_CONF);

/***************************************************************
 * [no] clns cluster-alias
 *
 */
EOLS	(clns_int_cluster_eol, clns_intfc_command, CLNS_CLUSTER_CMD);
KEYWORD	(clns_int_cluster, clns_int_cluster_eol, clns_int_congestion,
	 "cluster-alias", "Enable DECnet Phase V cluster aliasing", 
	 PRIV_CONF|PRIV_SUBIF);

/***************************************************************
 * [no] clns checksum
 */
EOLS	(clns_int_checksum_eol, clns_intfc_command, CLNS_CHECKSUM_CMD);
KEYWORD	(clns_int_checksum, clns_int_checksum_eol, clns_int_cluster,
	 "checksum", "Enable checksum generation", PRIV_CONF|PRIV_SUBIF);

/**************************************************************************
 * clns net <addr>
 * no clns net 
 *
 * Note that there is both a global and interface version 
 * of this command.
 *
 * (*OBJ(paddr,1)) = <addr>
 */

EOLS	(clns_int_net_eol, clns_intfc_command, CLNS_NSAP);
NSAPADDR(clns_int_net_addr, clns_int_net_eol, no_alt,
	 OBJ(paddr,1), "A CLNS address or host name");
NVGENS	(clns_int_net_nvgen, clns_int_net_addr, clns_commands, CLNS_NSAP);
KEYWORD	(clns_int_net, clns_int_net_nvgen, clns_int_checksum,
	 "net", "Set the CLNS address for this interface", PRIV_CONF|PRIV_SUBIF);

/*********************************************************************/
KEYWORD	(clns_int, clns_int_net, NONE,
	 "clns", "CLNS interface subcommands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (clns_int_subif, clns_int, ALTERNATE, main_if_or_vc(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	clns_int_subif
