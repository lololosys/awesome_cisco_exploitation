/* $Id: cfg_routemap_set.h,v 3.4.46.2 1996/08/12 16:03:37 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_routemap_set.h,v $
 *------------------------------------------------------------------
 * cfg_routemap_set.h
 *
 * Process route-map set subcommands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_routemap_set.h,v $
 * Revision 3.4.46.2  1996/08/12  16:03:37  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.46.1  1996/05/21  09:53:11  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.4  1995/12/07  17:45:13  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.3  1995/11/28  19:50:35  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:49  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:05:11  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:45  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.3  1995/06/15  15:19:14  widmer
 * CSCdi35889:  parser transition structures should be static
 *
 * Revision 2.2  1995/06/14  20:39:47  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * filesys/fslib.c            suported -> supported
 * os/msg_system.c            unititialized -> uninitialized
 * parser/cfg_routemap_set.h  substract -> subtract
 * smf/smf_debug_flags.h      missses -> misses
 *
 * Revision 2.1  1995/06/07  22:12:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * set metric <metric-value>
 * set metric-type type-1 | type-2 | internal | external
 * set tag <tag-value>
 * set automatic-tag
 * set level level-1 | level-2 | level-1-2 | stub-area | backbone
 * set interface <interface> [... <interface>]
 *
 * OBJ(int,1) = 0 for ip, 1 for clns.
 * OBJ(int,2) = metric, IGRP bandwidth, local-pref, or weight, or origin.
 * OBJ(int,3) = IGRP delay metric, or origin_as.
 * OBJ(int,4) = IGRP reliability metric.
 * OBJ(int,5) = IGRP loading metric.
 * OBJ(int,6) = IGRP mtu metric.
 * OBJ(int,2) = 0 for type-1, 1 for type-2, 2 for internal, 3 for external.
 * OBJ(int,2) = tag
 * OBJ(int,2) = 0 for level-1, 1 for level-2, 2 for level-1-2, 3 for stub-area,
 *              4 for backbone.
 * OBJ(paddr,1) = IP or OSI address.
 * OBJ(paddr,2) = If IP address in OBJ(paddr,1) than mask here.
 * OBJ(idb, 1) = interface
 */

LINK_TRANS(routemap_set_extend_here, no_alt);

EOLNS (routemap_set_eols, route_map_eols);
LINK_TRANS(routemap_set_return_here, routemap_set_eols);

/*
 * set interface <interface> ... <interface>
 */

pdecl(routemap_set_if);

FUNC 	(routemap_set_int_func,
	 routemap_set_if,
	 route_map_set_interface_command);
INTERFACE (routemap_set_if, routemap_set_int_func, routemap_set_eols,
	   OBJ(idb,1), IFTYPE_ANYSUB);

KEYWORD (routemap_set_int, routemap_set_if, routemap_set_extend_here,
	 common_str_interface, "Output interface", PRIV_CONF);

/*
 * set automatic-tag
 */
FUNC	(routemap_set_autotag_func,
	 routemap_set_eols,
	 route_map_set_autotag_command);
KEYWORD	(routemap_set_autotag, routemap_set_autotag_func,
	 routemap_set_int,
	 "automatic-tag", "Automatically compute TAG value", PRIV_CONF);

/*
 * set tag
 */
FUNC	(routemap_set_tag_func,
	 routemap_set_eols,
	 route_map_set_tag_command);
NUMBER	(routemap_set_tag_number, routemap_set_tag_func, routemap_set_eols,
	 OBJ(int,2), 0, 0xffffffff, "Tag value");
NOPREFIX(routemap_set_tag_no, routemap_set_tag_number, routemap_set_tag_func);
KEYWORD	(routemap_set_tag, routemap_set_tag_no, routemap_set_autotag,
	 "tag", "Tag value for destination routing protocol", PRIV_CONF);

/*
 * set metric
 * The following allows us to accept 'set metric +/-<metric>' or
 * 'set metric <metric> +/-<delay> <reliability> <loading> <mtu>'
 * A lot the dancing is to accept +/-.
 */
FUNC 	(routemap_set_metric_func,
	 routemap_set_eols,
	 route_map_set_metric_command);
NUMBER (routemap_set_metric_mtu, routemap_set_metric_func, no_alt,
	OBJ(int,6), 1, -1, "IGRP MTU of the path");
NUMBER (routemap_set_metric_loading, routemap_set_metric_mtu, no_alt,
	OBJ(int,5), 1, 255,
	"IGRP Effective bandwidth metric (Loading) where 255 is 100% loaded");
NUMBER (routemap_set_metric_reliability, routemap_set_metric_loading, no_alt,
	OBJ(int,4), 0, 255,
	"IGRP reliability metric where 255 is 100% reliable");
NUMBER (routemap_set_metric_delay, routemap_set_metric_reliability,
	routemap_set_metric_func,
	OBJ(int,3), 1, -1,
	"IGRP delay metric, in 10 microsecond units");
SET 	(routemap_set_minus_char2_parsed,  routemap_set_metric_delay,
	 OBJ(int,7), ROUTE_MAP_METRIC_SUB);
CHARACTER (routemap_set_minus_char2, routemap_set_minus_char2_parsed,
	   routemap_set_metric_delay, '-');
SET 	(routemap_set_plus_char2_parsed,  routemap_set_metric_delay,
	 OBJ(int,7), ROUTE_MAP_METRIC_ADD);
CHARACTER (routemap_set_plus_char2, routemap_set_plus_char2_parsed,
	   routemap_set_minus_char2, '+');
HELP 	(routemap_set_char_help2, routemap_set_plus_char2,
	 "  +/-<delay>	Add or subtract delay\n");
TESTVAR	(routemap_set_char2_check, routemap_set_char_help2,
	 routemap_set_metric_func, NONE, NONE, NONE, OBJ(int,7), 0);
NUMBER	(routemap_set_metric_number, routemap_set_char2_check,
	 routemap_set_eols, OBJ(int,2), 0, 0xffffffff,
	 "Metric value or IGRP bandwidth in Kbits per second");
SET 	(routemap_set_minus_char1_parsed,  routemap_set_metric_number,
	 OBJ(int,7), ROUTE_MAP_METRIC_SUB);
CHARACTER (routemap_set_minus_char1, routemap_set_minus_char1_parsed,
	   routemap_set_metric_number, '-');
SET 	(routemap_set_plus_char1_parsed,  routemap_set_metric_number,
	 OBJ(int,7), ROUTE_MAP_METRIC_ADD);
CHARACTER (routemap_set_plus_char1, routemap_set_plus_char1_parsed,
	   routemap_set_minus_char1, '+');
HELP 	(routemap_set_char_help1, routemap_set_plus_char1,
	 "  +/-<metric>	Add or subtract metric\n");
NOPREFIX(routemap_set_metric_no, routemap_set_char_help1,
	 routemap_set_metric_func);
KEYWORD	(routemap_set_metric, routemap_set_metric_no, routemap_set_tag,
	 "metric", "Metric value for destination routing protocol", PRIV_CONF);

/*
 * set metric-type
 */
FUNC 	(routemap_set_metrictype_func,
	 routemap_set_eols,
	 route_map_set_metrictype_command);
KEYWORD_ID (routemap_set_metrictype_ext, routemap_set_metrictype_func,
	    routemap_set_eols,
	    OBJ(int,2), 3,
	    "external", "IS-IS external metric", PRIV_CONF);
KEYWORD_ID (routemap_set_metrictype_int, routemap_set_metrictype_func,
	    routemap_set_metrictype_ext,
	    OBJ(int,2), 2,
	    "internal", "IS-IS internal metric", PRIV_CONF);
KEYWORD_ID (routemap_set_metrictype_type2, routemap_set_metrictype_func,
	    routemap_set_metrictype_int,
	    OBJ(int,2), 1,
	    "type-2", "OSPF external type 2 metric", PRIV_CONF);
KEYWORD_ID (routemap_set_metrictype_type1, routemap_set_metrictype_func,
	    routemap_set_metrictype_type2,
	    OBJ(int,2), 0,
	    "type-1", "OSPF external type 1 metric", PRIV_CONF);
NOPREFIX (routemap_set_metrictype_no, routemap_set_metrictype_type1,
	  routemap_set_metrictype_func);
KEYWORD_MM (routemap_set_metrictype, routemap_set_metrictype_no,
	    routemap_set_metric,
	    "metric-type",
	    "Type of metric for destination routing protocol", PRIV_CONF, 7);

/*
 * set level
 */
FUNC	(routemap_set_level_func,
	 routemap_set_eols,
	 route_map_set_level_command);
KEYWORD_ID (routemap_set_backbone, routemap_set_level_func, routemap_set_eols,
	    OBJ(int,2), 4,
	    "backbone", "Import into OSPF backbone area", PRIV_CONF);
KEYWORD_ID (routemap_set_stub, routemap_set_level_func, routemap_set_backbone,
	    OBJ(int,2), 3,
	    "stub-area", "Import into OSPF NSSA area", PRIV_CONF);
KEYWORD_ID_MM(routemap_set_level12, routemap_set_level_func, routemap_set_stub,
	      OBJ(int,2), 2,
	      "level-1-2", "Import into level-1 and level-2", PRIV_CONF, 9);
KEYWORD_ID (routemap_set_level2, routemap_set_level_func, routemap_set_level12,
	    OBJ(int,2), 1,
	    "level-2", "Import into level-2 sub-domain", PRIV_CONF);
KEYWORD_ID (routemap_set_level1, routemap_set_level_func, routemap_set_level2,
	    OBJ(int,2), 0,
	    "level-1", "Import into a level-1 area", PRIV_CONF);
NOPREFIX (routemap_set_level_no, routemap_set_level1, routemap_set_level_func);
KEYWORD	(routemap_set_level, routemap_set_level_no, routemap_set_metrictype,
	 "level", "Where to import route", PRIV_CONF);

NVGENNS (routemap_set_nvgen, routemap_set_level, route_map_set_nvgen);
KEYWORD	(routemap_set, routemap_set_nvgen, ALTERNATE,
	 "set", "Set values in destination routing protocol", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	routemap_set
