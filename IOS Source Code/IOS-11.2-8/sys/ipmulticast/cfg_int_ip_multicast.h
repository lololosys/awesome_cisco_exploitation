/* $Id: cfg_int_ip_multicast.h,v 3.5.20.4 1996/08/12 16:03:14 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ipmulticast/cfg_int_ip_multicast.h,v $
 *------------------------------------------------------------------
 * IP multicast interface commands
 *
 * May 1995.
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_multicast.h,v $
 * Revision 3.5.20.4  1996/08/12  16:03:14  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.5.20.3  1996/05/22  19:20:00  mleelani
 * CSCdi58181:  7000: crash at _fast_check
 * Branch: California_branch
 * Wrong check for slow acl.
 *
 * Revision 3.5.20.2  1996/05/13  20:10:17  mleelani
 * CSCdi57016:  Multicast helper-maps not functional
 * Branch: California_branch
 * Fix helper-maps.
 *
 * Revision 3.5.20.1  1996/04/05  21:55:19  dino
 * CSCdi53647:  Ambiguous command when doing <no ip multicast rate-limit
 * group-list>
 * Branch: California_branch
 *
 * Revision 3.5  1996/01/31  02:11:57  snyder
 * CSCdi47864:  spellink errors
 *              adminstratively -> administratively
 *
 * Revision 3.4  1995/12/07  17:45:08  widmer
 * CSCdi45134:  Replace occurences of Ifelse with appropriate Test* macros
 *
 * Revision 3.3  1995/12/01  18:38:32  lwei
 * CSCdi44737:  IP multicast does not support administratively scoped
 * boundaries
 *
 * Revision 3.2  1995/11/17  17:29:27  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:00:27  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:48  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/21  18:16:49  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:05:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip multicast use-functional
 */
EOLNS (ci_ip_multicast_functional_eol, ipmulticast_functional_command);
KEYWORD (ci_ip_multicast_functional, ci_ip_multicast_functional_eol, no_alt,
	 "use-functional", "Enable RFC1469 functional address", PRIV_CONF);
TEST_IDBSTATUS (ci_ip_multicast_functional_test, ci_ip_multicast_functional,
		no_alt, NONE, IDB_TR);

/************************************************************************
 *      ip multicast helper-map { broadcast | <group address> }
 *                              { <group address> | <broadcast address> }
 *     			        <acl> [ ttl <ttl> ]
 *   no ip multicast helper-map { broadcast | <group address> }
 *                              { <group address> | <broadcast address> }
 *
 * OBJ(paddr,1) = 0 for broadcast keyword | <multicast address>.
 * OBJ(paddr,2) = <multicast address> | <broadcast address>.
 * OBJ(int,1) | OBJ(string,1) = <acl>
 * OBJ(int,2) = <ttl>
 */
EOLNS (ci_ip_multicast_helper_eols, ipmulticast_helper_command);
NUMBER (ci_ip_multicast_helper_ttl, ci_ip_multicast_helper_eols, no_alt,
         OBJ(int,2), 1, 50, "TTL re-map value");
KEYWORD (ci_ip_multicast_helper_special, ci_ip_multicast_helper_ttl,
          ci_ip_multicast_helper_eols, "ttl", "TTL re-mapping", 
          PRIV_CONF|PRIV_SUBIF|PRIV_HIDDEN);
GENERAL_STRING (ci_ip_multicast_helper_nacl, ci_ip_multicast_helper_special,
		no_alt, OBJ(string,1),
		"IP Named Extended Access list", STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_multicast_helper_stest, ci_ip_multicast_helper_nacl,
		     no_alt, NONE);
NUMBER (ci_ip_multicast_helper_acl, ci_ip_multicast_helper_special, 
 	ci_ip_multicast_helper_stest, OBJ(int,1), MINSLOWACC,
  	MAXSLOWACC, "access-list");
NOPREFIX(ci_ip_multicast_helper_noprefix, ci_ip_multicast_helper_acl,
          ci_ip_multicast_helper_eols);
IPADDR(ci_ip_multicast_helper_addr2, ci_ip_multicast_helper_noprefix,
         no_alt, OBJ(paddr,2), "IP multicast address or IP broadcast address");
IPADDR(ci_ip_multicast_helper_addr1_multi, ci_ip_multicast_helper_addr2,
       no_alt, OBJ(paddr,1), "IP multicast address");
KEYWORD(ci_ip_multicast_helper_addr1_broad, ci_ip_multicast_helper_addr2,
	ci_ip_multicast_helper_addr1_multi, "broadcast", "Broadcast address",
	PRIV_CONF|PRIV_SUBIF);
NVGENNS (ci_ip_multicast_helper_nvgen, ci_ip_multicast_helper_addr1_broad, 
	ipmulticast_helper_command);
KEYWORD (ci_ip_multicast_helper, ci_ip_multicast_helper_nvgen,
	 ci_ip_multicast_functional_test,
	 "helper-map",
	 "Broadcast to Multicast map OR Multicast to Broadcast map", 
	 PRIV_CONF|PRIV_SUBIF);

/************************************************************************
 * ip multicast rate-limit [in | out] group-list <acl> source-list <acl> 
 *     [video | whiteboard] <kbps>
 * no ip multicast rate-limit [in | out] group-list <acl> source-list <acl>
 * 
 * OBJ(int,1) = TRUE if "in", FALSE if "out"
 * OBJ(int,2) | OBJ(string,2) = group-list <acl>
 * OBJ(int,3) | OBJ(string,3) = source-list <acl>
 * OBJ(int,4) = <kbps>
 */
EOLNS (ci_ip_multicast_rate_eols, ip_ratelimit_command);
NUMBER (ci_ip_multicast_rate_kbps, ci_ip_multicast_rate_eols, 
	ci_ip_multicast_rate_eols, OBJ(int,4), 0, MAXUINT,
	"Rate in kilobits per second");
SET (ci_ip_multicast_rate_media_def, ci_ip_multicast_rate_kbps, OBJ(int,5),
     SD_MEDIA_UNDEFINED);
KEYWORD_ID (ci_ip_multicast_rate_media_wb, ci_ip_multicast_rate_kbps,
	    ci_ip_multicast_rate_media_def, OBJ(int,5), SD_MEDIA_WB,
	    "whiteboard", "Rate limit whiteboard only", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ip_multicast_rate_media_video, ci_ip_multicast_rate_kbps,
	    ci_ip_multicast_rate_media_wb, OBJ(int,5), SD_MEDIA_VIDEO,
	    "video", "Rate limit video only", PRIV_CONF|PRIV_SUBIF);
NOPREFIX(ci_ip_multicast_rate_noprefix, ci_ip_multicast_rate_media_video,
	 ci_ip_multicast_rate_eols);
TEST_MULTIPLE_FUNCS(ci_ip_multicast_rate_ambig, ci_ip_multicast_rate_noprefix,
		    ci_ip_multicast_rate_kbps, no_alt);
GENERAL_STRING (ci_ip_multicast_rate_source_nacl, ci_ip_multicast_rate_ambig,
		no_alt, OBJ(string,3), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_multicast_rate_source_stest2,
		      ci_ip_multicast_rate_source_nacl, no_alt, NONE);
NUMBER (ci_ip_multicast_rate_source_num, ci_ip_multicast_rate_ambig,
	ci_ip_multicast_rate_source_stest2,
	OBJ(int,3), MINFASTACC, MAXFASTACC,
	"Multicast source access-list reference");
KEYWORD (ci_ip_multicast_rate_source, ci_ip_multicast_rate_source_num, 
	 ci_ip_multicast_rate_ambig, "source-list", 
	 "Rate limit for sources", PRIV_CONF|PRIV_SUBIF);
GENERAL_STRING (ci_ip_multicast_rate_group_nacl, ci_ip_multicast_rate_source,
		no_alt, OBJ(string,2), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_multicast_rate_group_stest,
		     ci_ip_multicast_rate_group_nacl, no_alt, NONE);
NUMBER (ci_ip_multicast_rate_group_num, ci_ip_multicast_rate_source,
	ci_ip_multicast_rate_group_stest,
	OBJ(int,2), MINFASTACC, MAXFASTACC,
	"Multicast group access-list reference");
KEYWORD (ci_ip_multicast_rate_group, ci_ip_multicast_rate_group_num, 
	 ci_ip_multicast_rate_source, 
	 "group-list", "Rate limit for groups", PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ip_multicast_rate_out, ci_ip_multicast_rate_group, no_alt,
	    OBJ(int,1), FALSE, "out", "Rate limit outgoing packets", 
	    PRIV_CONF|PRIV_SUBIF);
KEYWORD_ID (ci_ip_multicast_rate_in, ci_ip_multicast_rate_group, 
	    ci_ip_multicast_rate_out, OBJ(int,1), TRUE, "in", 
	    "Rate limit incoming packets", PRIV_CONF|PRIV_SUBIF);
NVGENNS (ci_ip_multicast_rate_nvgen, ci_ip_multicast_rate_in, 
	ip_ratelimit_command);
KEYWORD (ci_ip_multicast_rate, ci_ip_multicast_rate_nvgen,
	 ci_ip_multicast_helper,
	 "rate-limit", "Rate limit multicast data packets", 
	 PRIV_CONF|PRIV_SUBIF);


/************************************************************************
 * [no] ip multicast boundary <access-list> 
 *
 * OBJ(int, 1) = <access-list>
 */
EOLNS (ci_ip_multicast_boundary_eols, ip_multicast_boundary_command);
GENERAL_STRING (ci_ip_multicast_boundary_nacl, ci_ip_multicast_boundary_eols,
		no_alt, OBJ(string,1), "IP Named Standard Access list",
		STRING_HELP_CHECK);
TEST_MULTIPLE_FUNCS (ci_ip_multicast_boundary_stest,
		     ci_ip_multicast_boundary_nacl, no_alt, NONE);
NUMBER (ci_ip_multicast_boundary_number, ci_ip_multicast_boundary_eols,
	ci_ip_multicast_boundary_stest, OBJ(int,1), MINFASTACC, MAXFASTACC,
	"Access-list number");
NOPREFIX (ci_ip_multicast_boundary_no, ci_ip_multicast_boundary_number,
	  ci_ip_multicast_boundary_eols);
NVGENNS (ci_ip_multicast_boundary_nvgen, ci_ip_multicast_boundary_no,
	 ip_multicast_boundary_command);
KEYWORD (ci_ip_multicast_boundary, ci_ip_multicast_boundary_nvgen,
	 ci_ip_multicast_rate, "boundary",
	 "Boundary for administratively scoped multicast addresses",
	 PRIV_CONF|PRIV_SUBIF);
/************************************************************************
 * [no] ip multicast ttl-threshold <ttl-value>
 * 
 * OBJ(int,1) = <ttl-value>
 */
PARAMS_KEYONLY(ci_ip_multicast_ttl, ci_ip_multicast_boundary, "ttl-threshold",
	       OBJ(int,1), 0, TTL_255,
	       ipmulticast_ttl_command, 0,
	       "TTL threshold for multicast packets",
	       "TTL threshold", PRIV_CONF|PRIV_SUBIF);

KEYWORD (ci_ip_multicast, ci_ip_multicast_ttl, ALTERNATE,
	 "multicast", "IP multicast interface commands", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_multicast
