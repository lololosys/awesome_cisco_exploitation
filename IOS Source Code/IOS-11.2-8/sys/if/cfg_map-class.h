/* $Id: cfg_map-class.h,v 3.3.52.2 1996/08/12 16:02:19 widmer Exp $
 * $Source: /release/112/cvs/Xsys/if/cfg_map-class.h,v $
 *------------------------------------------------------------------
 * map-class command
 *
 * October 1993, Lionel Bustini
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_map-class.h,v $
 * Revision 3.3.52.2  1996/08/12  16:02:19  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3.52.1  1996/04/25  23:13:10  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.56.1  1996/04/24  04:06:40  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.3  1995/11/29  00:13:12  dblair
 * CSCdi41333:  DDR Callback and Multilink PPP clean up
 *
 * Revision 3.2  1995/11/17  09:26:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:48:17  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/30  05:03:17  dblair
 * CSCdi41333:  DDR Callback initial commit
 * Adding Callback feature for DDR interfaces.
 *
 * Revision 2.1  1995/06/07  22:11:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] map-class encap <name> 
 * 
 * encap = atm, (frame-relay, smds, x25, dialer)
 */

EOLNS	(cfg_mapclass_eol, mapclass_command);

STRING	(cfg_mapclass_name, cfg_mapclass_eol, no_alt,
	 OBJ(string, 1), "Static map class name");

KEYWORD_ID (cfg_mapclass_dialer, cfg_mapclass_name, no_alt, 
	    OBJ(int,1), ET_DIALER,
	    "dialer", "Dial on Demand", PRIV_CONF);

KEYWORD_ID (cfg_mapclass_atm, cfg_mapclass_name, cfg_mapclass_dialer, 
	    OBJ(int,1), ET_ATM,
	    "atm", "Asynchronous transfer mode", PRIV_CONF);

KEYWORD_ID (cfg_mapclass_frame, cfg_mapclass_name, cfg_mapclass_atm,
	OBJ(int,1), ET_FRAME_RELAY,
	"frame-relay", "Frame Relay", PRIV_CONF);

NVGENNS  (cfg_mapclass_nvgens, cfg_mapclass_frame, mapclass_command);

KEYWORD_ID(cfg_mapclass, cfg_mapclass_nvgens, ALTERNATE,
           leave_subcommand, TRUE,
           "map-class", "Configure static map class", PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE       cfg_mapclass
