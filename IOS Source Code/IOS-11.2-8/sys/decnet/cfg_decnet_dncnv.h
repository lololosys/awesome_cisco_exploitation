/* $Id: cfg_decnet_dncnv.h,v 3.2 1995/11/17 08:58:24 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/decnet/cfg_decnet_dncnv.h,v $
 *------------------------------------------------------------------
 * DECnet Phase V config commands
 *
 * October 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_decnet_dncnv.h,v $
 * Revision 3.2  1995/11/17  08:58:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:15:29  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:44:24  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:02:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************
 * decnet advertise <area> <hops> <cost>
 * no decnet advertise [<area>]
 *
 * OBJ(int,1) = <net>
 * OBJ(int,2) = <area>
 * OBJ(int,3) = <hops>
 * OBJ(int,4) = <cost>
 */
EOLS    (conf_decnet_adv_eol, decnet_advertise_command, DC_ADVERTISE);
NUMBER  (conf_decnet_adv_cost, conf_decnet_adv_eol, conf_decnet_adv_eol,
         OBJ(int,4), 0, DN_MAX_AREA_COST, "Cost for advertised area");
NUMBER  (conf_decnet_adv_hops, conf_decnet_adv_cost, conf_decnet_adv_eol,
         OBJ(int,3), 0, DN_MAX_AREA_HOPS, "Hop count for advertised area");
NOPREFIX (conf_decnet_adv_noprefix, conf_decnet_adv_hops, conf_decnet_adv_eol);
NUMBER  (conf_decnet_adv_number, conf_decnet_adv_noprefix, no_alt,
         OBJ(int,2), 1, DN_MAXAREAS,
         "Area to be imported into DECnet Phase IV");
NVGENS  (conf_decnet_adv_nvgen, conf_decnet_adv_number,
         decnet_advertise_command, DC_ADVERTISE);
KEYWORD (conf_decnet_advertise, conf_decnet_adv_nvgen, ALTERNATE,
         "advertise",
        "Advertise a DECnet area imported from OSI", PRIV_CONF);
/*****************************************************************
 *	decnet conversion {igrp <area-tag> | <nsap>}
 *	no decnet conversion {igrp <area-tag> | <nsap>}
 *
 * (*OBJ(paddr,1)) = <nsap>
 * OBJ(string,1) = <area-tag>
 */

EOLS	(conf_decnet_conv_eol, dn_set_conversion, DC_CONVERSION);
/* <nsap> */
NSAPADDR (conf_decnet_conv_nsap_addr, conf_decnet_conv_eol, NONE,
	  OBJ(paddr,1), "NSAP prefix");
TEST_MULTIPLE_FUNCS(conf_decnet_conv_nsap, conf_decnet_conv_nsap_addr,
		    NONE, no_alt);

/* igrp <area-tag> */
STRING	(conf_decnet_conv_tag, conf_decnet_conv_eol, no_alt,
	 OBJ(string,1), "ISO area tag");
KEYWORD_ID (conf_decnet_conv_igrp, conf_decnet_conv_tag, conf_decnet_conv_nsap,
	    OBJ(int,2), CONVERGENCE_IGRP,
	    "igrp", "ISO IGRP routing", PRIV_CONF);
NVGENS	(conf_decnet_conv_nvgen, conf_decnet_conv_igrp,
	 dn_set_conversion, DC_CONVERSION);
NOPREFIX (conf_decnet_conv_noconv, conf_decnet_conv_nvgen,
	  conf_decnet_conv_eol);
KEYWORD	(conf_decnet_conv, conf_decnet_conv_noconv, conf_decnet_advertise,
	 "conversion", "Enable Phase IV/V conversion", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_decnet_conv
