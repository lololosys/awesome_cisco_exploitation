/* $Id: cfg_acclist_atalk.h,v 3.4.50.2 1996/06/11 20:28:17 hampton Exp $
 * $Source: /release/112/cvs/Xsys/atalk/cfg_acclist_atalk.h,v $
 *------------------------------------------------------------------
 * AppleTalk access list commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_acclist_atalk.h,v $
 * Revision 3.4.50.2  1996/06/11  20:28:17  hampton
 * New access-list commands should set the NONVGEN flag. [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4.50.1  1996/05/30  23:42:07  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.4  1995/12/04  11:59:46  dwong
 * CSCdi44572:  NBP ACL configuration command has wrong syntax.
 *
 * Revision 3.3  1995/11/27  21:24:34  widmer
 * CSCdi43607:  Add keyword_option macro to parse multiple keywords
 * Also create permit_deny macro using keyword_option macro to parse
 * permit/deny keywords.
 * Change "ip security multilevel" command to use keyword_option macro.
 *
 * Revision 3.2  1995/11/17  08:44:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/02  01:43:35  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize xns, atalk, vines, decnet, xremote, x25, unix, wan, tn3270,
 * clns, cdp
 *
 * Revision 2.1  1995/06/07  22:00:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*****************************************************************
 * Appletalk access list
 *
 * access-list <600-699> { permit | deny } <network>
 * no access-list <600-699> [{ permit | deny } <network>]
 *
 * All of the below network lists accept the keyword 'broadcast-permit' or
 * 'broadcast-deny' at the end of the string.
 *
 *	"[no] access-list <list> permit|deny <net>"
 *	"[no] access-list <list> permit|deny additional-zones"
 *	"[no] access-list <list> permit|deny network <net>"
 *	"[no] access-list <list> permit|deny cable-range <start>-<end>"
 *	"[no] access-list <list> permit|deny includes <start>-<end>"
 *	"[no] access-list <list> permit|deny within <start>-<end>"
 *	"[no] access-list <list> permit|deny other-access"
 *	"[no] access-list <list> permit|deny zone <zone>"
 *  <zone> == * means all zones
 *
 *      "[no] access-list <list> permit|deny nbp <sequence> object|type|zone <string>"
 *      "[no] access-list <list> permit|deny other-nbps"
 *
 *
 * OBJ(int,1) == Access list number
 * OBJ(int,2) == TRUE for permit; FALSE for deny
 * OBJ(int,3) == Access list 'type' keyword value
 * OBJ(int,4) == Network number, or low cable range
 * OBJ(int,5) == High cable range
 * OBJ(int,6) == TRUE if OBJ(int,7) is set to any 'real' value.
 * OBJ(int,7) == TRUE for permit broadcasts; FALSE for deny.
 */

EOLS	(alat_eol, atalkaccess_command, LIST_APPLE);

SET (alat_bcast_not_set, alat_eol, OBJ(int,6), FALSE);

KEYWORD_ID (alat_bcast_key_permit, alat_eol, alat_bcast_not_set,
	 OBJ(int,7), TRUE,
	 "broadcast-permit", "Specify permission of broadcasts", PRIV_CONF);
KEYWORD_ID (alat_bcast_key_deny, alat_eol, alat_bcast_key_permit,
	 OBJ(int,7), FALSE,
	 "broadcast-deny", "Specify denial of broadcasts", PRIV_CONF);

SET	(alat_broadcast, alat_bcast_key_deny, OBJ(int,6), TRUE);

/* zone <zonename> */
TEXT	(alat_zone_name, alat_bcast_not_set, no_alt,
	 OBJ(string,1), "Appletalk zone name");
KEYWORD_ID (alat_zone, alat_zone_name, no_alt,
	    OBJ(int,3), ATACC_ZONE,
	    ATACCKWD_ZONE, "Filter on appletalk zone", PRIV_CONF);

/* within <start>-<end> */
CRANGE	(alat_within_range, alat_bcast_not_set, no_alt, OBJ(int,4), OBJ(int,5));
KEYWORD_ID (alat_within, alat_within_range, alat_zone,
	    OBJ(int,3), ATACC_WITHIN,
	    ATACCKWD_WITHIN, "Filter on cable range exclusively", PRIV_CONF);

/* nbp zone */
TEXT     (nbp_zone, alat_bcast_not_set, no_alt,
          OBJ(string, 1), "NBP zone filter");
KEYWORD_ID (alat_nbp_zone, nbp_zone, no_alt,
            OBJ(int,3), ATACC_NBP_ZONE,
	    ATACCKWD_NBP_ZONE, "Filter on nbp zone", PRIV_CONF);

/* nbp type */
TEXT     (nbp_type, alat_bcast_not_set, no_alt,
          OBJ(string, 1), "NBP type filter");
KEYWORD_ID (alat_nbp_type, nbp_type, alat_nbp_zone,
            OBJ(int,3), ATACC_NBP_TYPE,
	    ATACCKWD_NBP_TYPE, "Filter on nbp type", PRIV_CONF);


/* nbp object */
TEXT     (nbp_object, alat_bcast_not_set, no_alt,
          OBJ(string, 1), "NBP object filter");
KEYWORD_ID (alat_nbp_object, nbp_object, alat_nbp_type,
            OBJ(int,3), ATACC_NBP_OBJ,
	    ATACCKWD_NBP_OBJ, "Filter on nbp object", PRIV_CONF);

/* nbp sequence */
NUMBER   (alat_seq, alat_nbp_object, no_alt,
          OBJ(int,4), 1, 65536, "nbp sequence number");

/* nbp */
KEYWORD (alat_nbp, alat_seq, alat_within,
	 ATACCKWD_NBP, "Specify nbp filter", PRIV_CONF);

/* other-nbps */
KEYWORD_ID (alat_other_nbps, alat_bcast_not_set, alat_nbp,
            OBJ(int,3), ATACC_NBP_OTHERS,
	    ATACCKWD_NBP_OTHERS, "Default filter action for nbp", PRIV_CONF);

/* other-access */
KEYWORD_ID (alat_other, alat_broadcast, alat_other_nbps,
            OBJ(int,3), ATACC_OTHERS,
	    ATACCKWD_OTHER_ACCESS, "Default filter action", PRIV_CONF);

/* includes <start>-<end> */
CRANGE	(alat_incl_range, alat_broadcast, no_alt, OBJ(int,4), OBJ(int,5));
KEYWORD_ID (alat_incl, alat_incl_range, alat_other,
            OBJ(int,3), ATACC_INCLUDES,
	    ATACCKWD_INCLUDES, "Filter on cable range inclusively", PRIV_CONF);

/* cable-range <start>-<end> */
CRANGE	(alat_cr_range, alat_broadcast, no_alt, OBJ(int,4), OBJ(int,5));
KEYWORD_ID (alat_cr, alat_cr_range, alat_incl,
            OBJ(int,3), ATACC_CABLE,
	    ATACCKWD_CABLE_RANGE, "Filter on cable range", PRIV_CONF);

/* network <net> */
NUMBER	(alat_net_addr, alat_broadcast, no_alt,
	 OBJ(int,4), 1, 65279, "Appletalk network number");
KEYWORD_ID (alat_net, alat_net_addr, alat_cr,
	    OBJ(int,3), ATACC_NETWORK,
	    ATACCKWD_NETWORK, "Filter an appletalk network", PRIV_CONF);

/* additional-zones */
KEYWORD_ID (alat_addl, alat_bcast_not_set, alat_net,
	    OBJ(int,3), ATACC_ANYZONES,
	    ATACCKWD_ADDITIONAL_ZONES,
	    "Default filter action for unspecified zones",
	    PRIV_CONF);

/* <net> */
SET	(alat_addr_set, alat_broadcast, OBJ(int,3), ATACC_NETWORK);
NUMBER	(alat_addr, alat_addr_set, alat_addl, OBJ(int,4), 1, 65279,
	 "Appletalk network number");

/* {permit | deny} or EOL for 'no access-list <list>' */
SET	(alat_noprefix_set, alat_broadcast, OBJ(int,3), ATACC_DELETE);
TESTVAR	(alat_noprefix, alat_noprefix_set, no_alt,
	 NONE, NONE, NONE,
	 sense, FALSE);
PERMIT_DENY(alat_permit_deny, alat_addr, alat_noprefix,
	    OBJ(int,2), PRIV_CONF);
/*
 * New style "[no] apple access-list ..."
 */
EVAL	(alat_modify, alat_permit_deny,
	 GETOBJ(int,1) = GETOBJ(int,1) - AT_MIN_ACL + MINATALKACC);
NUMBER	(alat_num, alat_modify, NONE,
	 OBJ(int,1), AT_MIN_ACL, AT_MAX_ACL, "Appletalk access list");
NVGENS	(alat_nvgen, alat_num, atalkaccess_command, LIST_APPLE);
KEYWORD (access_list_appletalk, alat_nvgen, ALTERNATE,
	 "access-list", "Appletalk Access lists",
	 PRIV_CONF|PRIV_HIDDEN|PRIV_NONVGEN);

#undef	ALTERNATE
#define	ALTERNATE	access_list_appletalk

/*
 * Old style "[no] access-list ..."
 *
 * These remaining lines should be deleted in a future release.
 */
NUMBER	(oalat_num, alat_permit_deny, NONE,
	 OBJ(int,1), MINATALKACC, MAXATALKACC, "Appletalk access list");
NVGENS	(oalat_nvgen, oalat_num, atalkaccess_command, LIST_APPLE);
NOP	(oaccess_list_appletalk, oalat_nvgen, OALTERNATE);

#undef	OALTERNATE
#define	OALTERNATE	oaccess_list_appletalk
