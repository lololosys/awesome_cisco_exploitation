/* $Id: cfg_ip_community-list.h,v 3.4.46.2 1996/08/12 16:03:30 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_ip_community-list.h,v $
 *------------------------------------------------------------------
 * cfg_ip_community-list.h -- ip community-list parse chain
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_ip_community-list.h,v $
 * Revision 3.4.46.2  1996/08/12  16:03:30  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.4.46.1  1996/07/29  21:33:09  rchandra
 * CSCdi64437:  BGP: community display is not user-friendly
 * Branch: California_branch
 * - add user friendly community display
 * - add update throttling code (CSCdi64451)
 * - allow 'set community <additive>' during redistribution into BGP
 *
 * Revision 3.4  1995/12/11  07:43:05  rchandra
 * CSCdi45046:  BGP: prefer external route over confederation external
 * route
 * - prefer external route over the confederation external route
 * - do not drop routes with 'no-export' community in the sub-AS boundary
 * - add 'local-AS' special community
 *
 * Revision 3.3  1995/11/28  19:50:29  widmer
 * CSCdi44251:  Replace Ifelse(csb->in_help)/Eval with Func
 *
 * Revision 3.2  1995/11/17  17:31:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:38  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:08:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/****************************************************************
 * ip community-list <1-99> {permit | deny} <max of 16 communities>
 *
 *
 * This parse chain is weird in many ways because we could have different
 * set of communities for the same community list. Further, we want the
 * maximum number of communities allowed to be flexible to increase it.
 *
 * If <no> ip community-list <1-99> is straight forward.
 *
 * In general for 'ip commmunity-list' we have 3 stages and in each stage
 * we call community_command. (OBJ,4) is set to different values to
 * the different stages.
 *
 * 1) Once the permit/deny is parsed, we call community_command with init
 *    to initialize the static structure.
 * 2) After each number is parser, call community_command (with OBJ(int, 4)
 *    ENTRY) to add each entry to the static structure.
 * 3) If a eol is detected, call community_command (with OBJ(int, 4) set to
 *    END) to move the info in the static structure to the global structure.
 *
 * The community lists has been extended to include the regular expression
 * based lists. The list <1-99> will now be called as standard and <100-199>
 * as extended.
 */

pdecl(clist_community);

EOLNS	(clist_eol, ip_community_command);
SET	(clist_eol_std, clist_eol, OBJ(int, 4), PARSE_COMMUNITYLIST_END);

FUNC	(clist_func, clist_community, ip_community_command);
SET	(clist_func_set, clist_func, OBJ(int, 4), PARSE_COMMUNITYLIST_ENTRY);

KEYWORD_ID (clist_localas, clist_func_set, clist_eol_std,
	    OBJ(int, 3), COMMUNITY_LOCALAS, 
 	    "local-AS",
 	    "Do not send outside local AS (well-known community)", PRIV_CONF);
KEYWORD_ID (clist_noadv, clist_func_set, clist_localas,
	    OBJ(int, 3), COMMUNITY_NOADV,
	    "no-advertise",
	    "Do not advertise to any peer (well-known community)", PRIV_CONF);
KEYWORD_ID (clist_noexport, clist_func_set, clist_noadv,
	    OBJ(int, 3), COMMUNITY_NOEXPORT,
	    "no-export",
	    "Do not export to next AS (well-known community)", PRIV_CONF);
KEYWORD_ID (clist_internet, clist_func_set, clist_noexport,
	    OBJ(int, 3), COMMUNITY_INTERNET,
	    "internet",
	    "Internet (well-known community)", PRIV_CONF);

NUMBER	(clist_number, clist_func_set, clist_internet,
	 OBJ(int,3), 1, COMMUNITY_MAX, "community number");

BGP_COMMUNITY (clist_community, clist_func_set, clist_number,
	       OBJ(int,5), OBJ(int,6), "community number");

FUNC	(clist_init, clist_community, ip_community_command);
SET	(clist_init_std, clist_init, OBJ(int, 4), PARSE_COMMUNITYLIST_INIT);

SET	(clist_eol_ext, clist_eol, OBJ(int,4), PARSE_COMMUNITYLIST_REXP);

TEXT	(clist_extexp, clist_eol_ext, no_alt,
	 OBJ(string, 1), "An ordered list as a regular-expression");
IFELSE	(clist_branch, clist_init_std, clist_extexp,
	 (GETOBJ(int,1) <= MAX_STD_COMLIST));
KEYWORD_ID (clist_permit, clist_branch, no_alt, OBJ(int,2), TRUE,
	    "permit", "Specify community to accept", PRIV_CONF);
KEYWORD_ID (clist_deny, clist_branch, clist_permit, OBJ(int,2), FALSE,
	    "deny", "Specify community to reject", PRIV_CONF);

IFELSE	(clist_del, clist_eol_std, clist_eol_ext,
	 (GETOBJ(int,1) <= MAX_STD_COMLIST));
NOPREFIX(clist_noprefix, clist_deny, clist_del);
NUMBER	(clist_extnum, clist_noprefix, NONE,
	 OBJ(int,1), MAX_STD_COMLIST+1, MAX_EXT_COMLIST,
	 "Community list number (extended)");
NUMBER 	(clist_stdnum, clist_noprefix, clist_extnum,
	 OBJ(int,1), 1, MAX_STD_COMLIST, "Community list number (standard)");
NVGENNS	(clist_nvgen, clist_stdnum, ip_community_nvgen);
KEYWORD (clist_kwd, clist_nvgen, ALTERNATE,
	 "community-list", "Add a community list entry", PRIV_CONF);


/*
 * ip bgp-community new-format
 */
EOLNS	(community_eol, ip_community_format);
KEYWORD (community_format, community_eol, no_alt,
	 "new-format", "select aa:nn format for BGP community", PRIV_CONF);
NVGENNS	(community_nvgen, community_format, ip_community_format);
KEYWORD (community_kwd, community_nvgen, clist_kwd,
	 "bgp-community", "format for BGP community", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	community_kwd
