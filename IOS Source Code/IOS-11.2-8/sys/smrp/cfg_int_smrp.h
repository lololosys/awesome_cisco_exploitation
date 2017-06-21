/* $Id: cfg_int_smrp.h,v 3.2 1995/11/17 18:52:47 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/smrp/cfg_int_smrp.h,v $
 *------------------------------------------------------------------
 * Interface configuration commands for SMRP.
 *
 * April 1995, Steven Lin
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_int_smrp.h,v $
 * Revision 3.2  1995/11/17  18:52:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  21:24:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/06/21  18:18:26  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:44:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(ci_smrp_extend_here, no_alt);

/******************************************************************
 * smrp mroute-cache protocol appletalk
 * no smrp mroute-cache protocol appletalk
 *
 * OBJ(int, 1) = protocol type (uses linktype)
 */
EOLS	(ci_smrp_mrcache_eol, smrpif_command, SMRP_MCROUTEC);
NVGENS (ci_smrp_mrcache_nvgen, ci_smrp_mrcache_eol, smrpif_command, SMRP_MCROUTEC);

KEYWORD_ID (ci_smrp_mrcache_atalk, ci_smrp_mrcache_nvgen, no_alt,
	    OBJ(int,1), LINK_APPLETALK,
	    "appletalk", "Enable AppleTalk datagram service",
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD (ci_smrp_mrcache_proto, ci_smrp_mrcache_atalk, no_alt,
	 "protocol", "Specify network layer service protocol",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD	(ci_smrp_mrcache, ci_smrp_mrcache_proto, ci_smrp_extend_here,
	"mroute-cache", "Enable SMRP route cache", PRIV_CONF|PRIV_SUBIF);

/*******************************************************************
 * smrp protocol { appletalk } [netrange <start>-<end>] 
 * no smrp protocol appletalk
 * OBJ(int, 1) = start number
 * OBJ(int, 2) = end number
 * OBJ(int, 3) = protocol type (uses linktype)
 *******************************************************************/

EOLS (ci_smrp_proto_eol, smrpif_command, SMRP_PROTO);
SMRPRANGE (ci_smrp_proto_srange, ci_smrp_proto_eol, ci_smrp_proto_eol,
	   OBJ(int,1), OBJ(int,2));
KEYWORD (ci_smrp_proto_net, ci_smrp_proto_srange, ci_smrp_proto_eol,
	"network-range", "SMRP Network Range", PRIV_CONF|PRIV_SUBIF);
NOPREFIX (ci_smrp_proto_no, ci_smrp_proto_net, ci_smrp_proto_eol);
KEYWORD_ID (ci_smrp_proto_type, ci_smrp_proto_no, no_alt,
	    OBJ(int,3), LINK_APPLETALK,
	    "appletalk", "Use AppleTalk for datagram service",
	    PRIV_CONF|PRIV_SUBIF);
NVGENS (ci_smrp_proto_nvgen, ci_smrp_proto_type, smrpif_command, SMRP_PROTO);

KEYWORD (ci_smrp_proto, ci_smrp_proto_nvgen, ci_smrp_mrcache,
	 "protocol", "Specify network layer service protocol",
	 PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_smrp, ci_smrp_proto, ALTERNATE,
	"smrp", "Simple Multicast Routing Protocol interface subcommands",
	PRIV_CONF|PRIV_SUBIF);

#undef ALTERNATE
#define ALTERNATE ci_smrp
