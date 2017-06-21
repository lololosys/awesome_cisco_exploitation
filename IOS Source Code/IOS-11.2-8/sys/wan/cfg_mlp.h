/* $Id: cfg_mlp.h,v 3.1.6.2 1996/05/19 05:48:55 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_mlp.h,v $
 *------------------------------------------------------------------
 * MLP configuration
 *
 * March 1996, Shoou Yiu
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_mlp.h,v $
 * Revision 3.1.6.2  1996/05/19  05:48:55  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.1.6.1  1996/04/27  06:35:46  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1.2.1  1996/03/09  07:18:47  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 3.1  1996/03/09  06:44:18  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * mlp virtual-template <number>
 * no mlp virtual-template <number>
 */
EOLS	(conf_mlp_eol, ppp_mlp_command, PPP_MULTILINK_DEFAULT);

PARAMS_KEYONLY(conf_mlp_vtemplate, conf_mlp_eol,
	       "virtual-template", OBJ(int,1), 1, (VTEMPLATE_MAXVTEMPLATE-1),
	       ppp_mlp_command, PPP_MULTILINK_VTEMPLATE,
	       "virtual-template interface",
	       "virtual template number", PRIV_CONF);

KEYWORD	(conf_mlp, conf_mlp_vtemplate, ALTERNATE, 
	 "multilink", "PPP multilink global configuration", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_mlp
