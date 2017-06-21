/* $Id: cfg_int_mlp.h,v 3.1.2.1 1996/05/19 05:48:53 dblair Exp $
 * $Source: /release/112/cvs/Xsys/wan/cfg_int_mlp.h,v $
 *------------------------------------------------------------------
 * Interface configuration commands for multilink PPP.
 *
 * May 1996
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_mlp.h,v $
 * Revision 3.1.2.1  1996/05/19  05:48:53  dblair
 * CSCdi57979:  Multilink clean up from Dial1_branch
 * Branch: California_branch
 * Optimize fragmentation to remove multiply and divides from the
 * data path.
 * Xmit fragments from the mlp_holdq_enqueue.
 * update BW when links are added and removed from bundle
 * Fix byte counts and packet counts on the virtual bundle interface
 * Clean up
 *
 * Revision 3.1  1996/05/19  05:16:34  dblair
 * Placeholder for multilink interface configuration commands.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * [no] multilink max-fragments <x>
 * Configure maximum number of fragments per transmitted packet
 */
PARAMS_KEYONLY(cfg_int_mlp_max_frags, no_alt, 
 	       "max-fragments", OBJ(int,1), 1, MLP_DEFAULT_MAX_FRAGS,
 	       multilink_command,
 	       MULTILINK_MAX_FRAGS,
 	       "Specify the maximum number of fragments per transmitted packet",
 	       "Maximum fragments per packet",
 	       PRIV_CONF);
/***************************************************************
 * [no] multilink load-threshold <x> [outbound | inbound | either]
 * Configure load threshold for adding and deleting links
 */
EOLS	(cfg_int_mlp_thresh_eol, multilink_command, MULTILINK_THRESHOLD);
KEYWORD_ID(cfg_int_mlp_thresh_inbound, cfg_int_mlp_thresh_eol,
 	   cfg_int_mlp_thresh_eol,
 	   OBJ(int,2), MULTILINK_THRESH_INBOUND,
 	   "inbound", 
 	   "Threshold decision based on inbound traffic only", 
 	   PRIV_CONF);
KEYWORD_ID(cfg_int_mlp_thresh_outbound, cfg_int_mlp_thresh_eol,
 	   cfg_int_mlp_thresh_inbound,
 	   OBJ(int,2), MULTILINK_THRESH_OUTBOUND,
 	   "outbound", 
 	   "Threshold decision based on outbound traffic only", 
 	   PRIV_CONF);
KEYWORD_ID(cfg_int_mlp_thresh_either, cfg_int_mlp_thresh_eol,
 	   cfg_int_mlp_thresh_outbound,
 	   OBJ(int,2), MULTILINK_THRESH_EITHER,
 	   "either", 
 	   "Threshold decision based on max of inbound and outbound traffic", 
 	   PRIV_CONF);
NUMBER	(cfg_int_mlp_thresh_size, cfg_int_mlp_thresh_either, no_alt,
 	 OBJ(int,1), 1, 255, "Load threshold to add/delete a link");
NOPREFIX (cfg_int_mlp_no_thresh, cfg_int_mlp_thresh_size,
 	  cfg_int_mlp_thresh_eol);
NVGENS	(cfg_int_mlp_thresh_nvgen, cfg_int_mlp_no_thresh, multilink_command, 
 	 MULTILINK_THRESHOLD);
KEYWORD (cfg_int_mlp_thresh, cfg_int_mlp_thresh_nvgen, cfg_int_mlp_max_frags,
 	 "load-threshold", 
 	 "Specify threshold used to add/delete links", PRIV_CONF);
/***************************************************************
 * [no] multilink min-links <x>
 * Configure the minimum required links in a bundle
 */
PARAMS_KEYONLY(cfg_int_mlp_min_links, cfg_int_mlp_thresh, 
	       "min-links", OBJ(int,1), 0, MAXINT, multilink_command,
	       MULTILINK_MIN_LINKS,
	       "Specify lower limit of the number of links in a bundle",
	       "Number of links", PRIV_CONF);
/***************************************************************
 * [no] multilink max-links <x>
 * Configure the maximum possible links in a bundle
 */
PARAMS_KEYONLY(cfg_int_mlp_max_links, cfg_int_mlp_min_links, 
	       "max-links", OBJ(int,1), 1, MAXINT, multilink_command,
	       MULTILINK_MAX_LINKS,
	       "Specify upper limit of the number of links in a bundle",
	       "Number of links", PRIV_CONF );

KEYWORD(cfg_int_mlp, cfg_int_mlp_max_links, NONE,
 	"multilink", "Configure multilink parameters", PRIV_CONF);
ASSERT (cfg_int_mlp_test_intf, cfg_int_mlp, ALTERNATE,
	(((csb->interface->hwptr->type == IDBTYPE_VTEMPLATE) ||
	  (csb->interface->hwptr->type == IDBTYPE_VACCESS)) &&
	 (csb->interface->hwptr->enctype == ET_PPP)));

#undef	ALTERNATE
#define	ALTERNATE	cfg_int_mlp_test_intf
