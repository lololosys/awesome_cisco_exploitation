/* $Id: exec_show_traffic-shape.h,v 3.1.4.2 1996/08/12 16:02:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/exec_show_traffic-shape.h,v $
 *------------------------------------------------------------------
 * Interface specific configuration commands for fair queueing
 *
 * April 1996, Fred Baker
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_traffic-shape.h,v $
 * Revision 3.1.4.2  1996/08/12  16:02:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.1.4.1  1996/04/17  13:40:08  fred
 * CSCdi54910:  commit rsvp, traffic shaping, and RED
 * Branch: California_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**********************************************************************
 *      show traffic-shape [<interface>]
 *
 *      OBJ(idb,1) = interface if one is selected
 */
EOLNS(ci_show_shape_cfg_eol, show_traffic_shape_command);
SET(ci_show_shape_cfg_all, ci_show_shape_cfg_eol,
	  OBJ(idb, 1), NULL);
INTERFACE(ci_show_shape_cfg, ci_show_shape_cfg_eol,
	  ci_show_shape_cfg_all, OBJ(idb, 1), IFTYPE_ANYSUB);

/******************************************************************
 * show traffic-shape queue <interface> [<queue-number>]
 *
 * OBJ(idb,1) = <interface>
 */
EOLNS(show_shaping_queue_eols, show_shaping_queue_command);
INTERFACE(show_shaping_queue_interface, show_shaping_queue_eols,
	  show_shaping_queue_eols, OBJ(idb,1), IFTYPE_ANYSUB);
KEYWORD(show_shaping_queue, show_shaping_queue_interface, ci_show_shape_cfg,
         "queue", "Show shaping queue contents", PRIV_USER);

/**********************************************************************
 *      show traffic-shape statistics [<interface>]
 *
 *      OBJ(idb,1) = interface if one is selected
 */
EOLNS(ci_show_shape_stats_eol, show_traffic_shape_command_stats);
SET(ci_show_shape_stats_all, ci_show_shape_stats_eol,
	  OBJ(idb, 1), NULL);
INTERFACE(ci_show_shape_stats_idb, ci_show_shape_stats_eol,
	  ci_show_shape_stats_all, OBJ(idb, 1), IFTYPE_ANYSUB);
KEYWORD(ci_show_shape_stats, ci_show_shape_stats_idb, show_shaping_queue,
        "statistics", "traffic rate shaping statistics", PRIV_USER);
KEYWORD(ci_show_shape, ci_show_shape_stats, ALTERNATE,
        "traffic-shape", "traffic rate shaping configuration", PRIV_USER);

#undef ALTERNATE
#define ALTERNATE ci_show_shape
