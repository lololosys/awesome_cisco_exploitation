/* $Id: cfg_int_iso_igrp.h,v 3.2.60.1 1996/08/12 16:00:39 widmer Exp $
 * $Source: /release/112/cvs/Xsys/clns/cfg_int_iso_igrp.h,v $
 *------------------------------------------------------------------
 * C F G _ I N T _ I S O _ I G R P . H
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_int_iso_igrp.h,v $
 * Revision 3.2.60.1  1996/08/12  16:00:39  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.2  1995/11/17  08:51:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:06:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:46:58  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:05:58  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * iso-igrp adjacency-filter <name>
 * no iso-igrp adjacency-filter
 *
 */
EOLNS	(iso_igrp_adj_filter_eol, iso_igrp_adj_filter_cmd);
STRING  (iso_igrp_adj_filter_name, iso_igrp_adj_filter_eol, no_alt,
	 OBJ(string,1), "Name of filter set or expression");
NOPREFIX(iso_igrp_adj_filter_noprefix, iso_igrp_adj_filter_name,
	 iso_igrp_adj_filter_eol);
NVGENNS  (iso_igrp_adj_filter_nvgen, iso_igrp_adj_filter_noprefix,
	 iso_igrp_adj_filter_cmd);
KEYWORD	(iso_igrp_adj_filter, iso_igrp_adj_filter_nvgen, no_alt,
	 "adjacency-filter", "Filter ISO-IGRP adjacencies", PRIV_CONF|PRIV_SUBIF);

/*********************************************************************/
KEYWORD	(iso_igrp_int, iso_igrp_adj_filter, NONE,
	 "iso-igrp", "ISO-IGRP interface subcommands", PRIV_CONF|PRIV_SUBIF);

ASSERT  (iso_igrp_subif, iso_igrp_int, ALTERNATE, 
	 main_if_or_vc(csb->interface));

#undef	ALTERNATE
#define	ALTERNATE	iso_igrp_subif
