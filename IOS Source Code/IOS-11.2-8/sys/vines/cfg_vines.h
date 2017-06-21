/* $Id: cfg_vines.h,v 3.3 1996/02/01 23:34:53 widmer Exp $
 * $Source: /release/111/cvs/Xsys/vines/cfg_vines.h,v $
 *------------------------------------------------------------------
 * C F G _ V I N E S . H
 *
 * Banyan Vines global configuration commands.
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_vines.h,v $
 * Revision 3.3  1996/02/01  23:34:53  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:57:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:44:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:04  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:14:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************************
 * vines streettalk
 * no vines streettalk
 */
EOLS	(c_v_streettalk_eol, vst_command, VINES_STREETTALK);
KEYWORD	(c_v_streettalk, c_v_streettalk_eol, no_alt,
	 "streettalk", "Enable Streettalk support", PRIV_CONF|PRIV_HIDDEN);

/***************************************************************************
 * vines srtp-enabled
 * no vines srtp-enabled
 */
EOLS	(c_v_srtpeol, vines_command, VINES_SRTP_ENABLED);
KEYWORD	(c_v_srtp, c_v_srtpeol, c_v_streettalk,
	 "srtp-enabled", "Enable SRTP support", PRIV_CONF);

/***************************************************************************
 * vines routing [<address>]
 * no vines routing [<address>]
 */
EOLS	(conf_vines_routingeol, vines_command, VINES_ROUTING);
/* The address, if it is there. */
SET	(conf_vines_routingadr_set, conf_vines_routingeol, OBJ(int,1), TRUE);
VINADDR	(conf_vines_routingaddr, conf_vines_routingadr_set, 
	 conf_vines_routingeol, OBJ(paddr,1), "VINES address");
KEYWORD_ID(conf_vines_routing_recompute, conf_vines_routingaddr,
	   conf_vines_routingaddr,
	   OBJ(int,2), TRUE,
	   "recompute", "recompute address", PRIV_CONF);
NOPREFIX (conf_vines_routing_noprefix, conf_vines_routing_recompute,
	  conf_vines_routingeol);

NVGENS	(conf_vines_routingnvgen, conf_vines_routing_noprefix, vines_command, 
	 VINES_ROUTING);
KEYWORD	(conf_vines_routing, conf_vines_routingnvgen, c_v_srtp,
	 "routing", "VINES routing", PRIV_CONF);

/***************************************************************************
 * vines hostname
 * no vines hostname
 */
EOLS	   (c_v_hn_eol, vines_command, VINES_HOSTNAME);
STRING     (c_v_hn_name, c_v_hn_eol, no_alt,
	    OBJ(string,1), "VINES name for this router");
NVGENS     (c_v_hn_nvgen, c_v_hn_name, vines_command, VINES_HOSTNAME);
KEYWORD_MM (conf_vines_hostname, c_v_hn_nvgen, conf_vines_routing,
	    "hostname", "VINES specific name for this router",
	    PRIV_CONF|PRIV_HIDDEN, 5);

/***************************************************************************
 * vines decimal
 * no vines decimal
 */
EOLS	(conf_vines_decimaleol, vines_command, VINES_DECIMAL);
KEYWORD	(conf_vines_decimal, conf_vines_decimaleol, conf_vines_hostname,
	 "decimal", "Display VINES addresses in decimal", PRIV_CONF);

/************************************************************************/
KEYWORD	(configure_vines, conf_vines_decimal, ALTERNATE,
	 "vines", "VINES global configuration commands", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	configure_vines
