/* $Id: exec_show_appn.h,v 3.3 1996/02/22 14:46:11 mregan Exp $
 * $Source: /release/111/cvs/Xsys/appn/exec_show_appn.h,v $
 *------------------------------------------------------------------
 * exec_show_appn.h -- Contains the APPN show commands
 *
 * April 13, 1994 - Leo Pereira
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_show_appn.h,v $
 * Revision 3.3  1996/02/22  14:46:11  mregan
 * CSCdi48499:  sho appn int filter does not work for unqualified LU names
 * corrected logic for filtering on names
 *
 * Revision 3.2  1996/02/20  15:11:52  mregan
 * CSCdi48495:  show appn dlur-lu should default to brief
 * Bad eol removed.
 *
 * Revision 3.1  1996/02/01  04:56:51  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 1: Create copies of the files to be moved in their new location.
 *
 * Revision 3.3  1995/12/05  21:57:29  mregan
 * CSCdi42175:  appn show output gets too large to be useful
 * added ability to filter output
 *
 * Revision 3.2  1995/11/17  18:46:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:52:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/26  01:56:53  mregan
 * CSCdi28879:  appn show commands have minor problems
 * update 'sh appn * brief' commands
 *
 * Revision 2.2  1995/06/21  18:17:47  widmer
 * CSCdi36227:  Fix parse chain alternates
 *
 * Revision 2.1  1995/06/07  22:22:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/******************************************************************
 * show appn connection-network
 *           class-of-service           [brief|detail]
 *           directory                  [brief|detail]
 *           intermediate-session       [brief|detail]
 *           defined-link (deleted)
 *           link-station               [brief|detail]
 *           session                    [brief|detail]
 *           mode
 *           node
 *           topology                   [brief|detail]
 *           dlus                       [brief|detail]
 *           dlur-pu [dlus <dlus-name>] [brief|detail]
 *           dlur-lu [pu <pu-name>]     [brief|detail]
 *           statistics
 */
EOLS	(show_appn_eol, appn_show_command, SHOW_APPN);
pdecl     (show_appn_dlur_lu_flags);
KEYWORD_ID(show_appn_dlur_lu_brief, show_appn_dlur_lu_flags, no_alt,
	   OBJ(int,5), FALSE,
	   "brief", "Show brief APPN DLUR LU information", PRIV_USER);
KEYWORD_ID(show_appn_dlur_lu_detail, show_appn_dlur_lu_flags, show_appn_dlur_lu_brief,
	   OBJ(int,5), TRUE,
	   "detail", "Show detailed APPN DLUR LU information", PRIV_USER);
APPN_TYPE_A_NAME_STAR(show_appn_dlur_lu_hostpuname, show_appn_dlur_lu_flags, no_alt,
	 OBJ(string, 3), "Host PU name");
KEYWORD_ID(show_appn_dlur_lu_hostpuname_flag, show_appn_dlur_lu_hostpuname, show_appn_dlur_lu_detail,
	 OBJ(int, 4), TRUE,
         "host-pu", "Show APPN DLUR LUs for a specific Host PU",PRIV_USER);
APPN_TYPE_A_NAME_STAR(show_appn_dlur_lu_luname, show_appn_dlur_lu_flags, no_alt,
	 OBJ(string, 2), "LU name");
KEYWORD_ID(show_appn_dlur_lu_luname_flag, show_appn_dlur_lu_luname, show_appn_dlur_lu_hostpuname_flag,
	 OBJ(int, 3), TRUE,
         "name", "Show APPN DLUR LUs for a specific LU",PRIV_USER);
APPN_TYPE_A_NAME_STAR(show_appn_dlur_lu_puname, show_appn_dlur_lu_flags, no_alt,
	 OBJ(string, 1), "PU name");
KEYWORD_ID(show_appn_dlur_lu_puname_flag, show_appn_dlur_lu_puname, show_appn_dlur_lu_luname_flag,
	 OBJ(int, 2), TRUE,
         "pu", "Show APPN DLUR LUs for a specific PU",PRIV_USER);
NOP       (show_appn_dlur_lu_nop, show_appn_dlur_lu_puname_flag, show_appn_eol);
HELP      (show_appn_dlur_lu_flags, show_appn_dlur_lu_nop,
           "show appn dlur-lu [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_dlur_lu, show_appn_dlur_lu_flags, no_alt,
	   OBJ(int,1), APPN_SHOW_DLUR_LU,
	   "dlur-lu", "Show APPN DLUR LU information", PRIV_USER);

pdecl     (show_appn_dlur_pu_flags);
KEYWORD_ID(show_appn_dlur_pu_brief, show_appn_dlur_pu_flags, no_alt,
	   OBJ(int,5), FALSE,
	   "brief", "Show brief APPN DLUR PU information", PRIV_USER);
KEYWORD_ID(show_appn_dlur_pu_detail, show_appn_dlur_pu_flags, show_appn_dlur_pu_brief,
	   OBJ(int,5), TRUE,
	   "detail", "Show detailed APPN DLUR PU information", PRIV_USER);
APPN_TYPE_A_NAME_STAR(show_appn_dlur_pu_hostpuname, show_appn_dlur_pu_flags, no_alt,
	 OBJ(string, 3), "Host PU name");
KEYWORD_ID(show_appn_dlur_pu_hostpuname_flag, show_appn_dlur_pu_hostpuname, show_appn_dlur_pu_detail,
	 OBJ(int,4), TRUE,
         "host-pu", "Show APPN DLUR PUs for a specific host PU name", PRIV_USER);
APPN_TYPE_A_NAME_STAR(show_appn_dlur_pu_puname, show_appn_dlur_pu_flags, no_alt,
	 OBJ(string, 2), "PU name");
KEYWORD_ID(show_appn_dlur_pu_puname_flag, show_appn_dlur_pu_puname, show_appn_dlur_pu_hostpuname_flag,
	 OBJ(int,3), TRUE,
         "name", "Show APPN DLUR PUs for a specific PU name", PRIV_USER);
APPN_NAME_STAR(show_appn_dlur_pu_dlusname, show_appn_dlur_pu_flags, no_alt,
	 OBJ(string, 1), "DLUS name");
KEYWORD_ID(show_appn_dlur_pu_dlusname_flag, show_appn_dlur_pu_dlusname, show_appn_dlur_pu_puname_flag,
	 OBJ(int,2), TRUE,
         "dlus", "Show APPN DLUR PUs for a specific DLUS", PRIV_USER);
NOP       (show_appn_dlur_pu_nop, show_appn_dlur_pu_dlusname_flag, show_appn_eol);
HELP      (show_appn_dlur_pu_flags, show_appn_dlur_pu_nop,
           "show appn dlur-pu [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_dlur_pu, show_appn_dlur_pu_flags, show_appn_dlur_lu,
	   OBJ(int,1), APPN_SHOW_DLUR_PU,
	   "dlur-pu", "Show APPN DLUR PU information", PRIV_USER);

KEYWORD_ID(show_appn_dlus_brief, show_appn_eol, show_appn_eol,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN DLUS information", PRIV_USER);
KEYWORD_ID(show_appn_dlus_detail, show_appn_eol, show_appn_dlus_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN DLUS information", PRIV_USER);
KEYWORD_ID(show_appn_dlus, show_appn_dlus_detail, show_appn_dlur_pu,
	   OBJ(int,1), APPN_SHOW_DLUS,
	   "dlus", "Show APPN DLUS information", PRIV_USER);

pdecl     (show_appn_trs_flags);
KEYWORD_ID(show_appn_trs_brief, show_appn_trs_flags, no_alt,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN topology information", PRIV_USER);
KEYWORD_ID(show_appn_trs_detail, show_appn_trs_flags, show_appn_trs_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN topology information", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_trs_detail

APPN_NAME_STAR (show_appn_trs_name, show_appn_trs_flags, no_alt,
	   OBJ(string, 1), "Show specific APPN topology resources by name");
KEYWORD_ID(show_appn_trs_name_flag, show_appn_trs_name, NEXT_FILTER,
	   OBJ(int,3), TRUE,
           "name", "Show specific APPN topology resources by CP name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_trs_name_flag

NOP       (show_appn_trs_nop, NEXT_FILTER, show_appn_eol);
HELP      (show_appn_trs_flags, show_appn_trs_nop,
           "show appn topology [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_trs, show_appn_trs_flags, show_appn_dlus,
	   OBJ(int,1), APPN_SHOW_TRS,
	   "topology", "Show APPN topology database information", PRIV_USER);
KEYWORD_ID(show_appn_node, show_appn_eol, show_appn_trs,
	   OBJ(int,1), APPN_SHOW_NODE,
	   "node", "Show APPN local node information", PRIV_USER);
KEYWORD_ID(show_appn_stats, show_appn_eol, show_appn_node,
	   OBJ(int,1), APPN_SHOW_STATS,
	   "statistics", "Show APPN statistics", PRIV_USER);

KEYWORD_ID(show_appn_mode, show_appn_eol, show_appn_stats,
	   OBJ(int,1), APPN_SHOW_MODE,
	   "mode", "Show APPN mode information", PRIV_USER);

pdecl     (show_appn_sess_flags);
KEYWORD_ID(show_appn_sess_brief, show_appn_sess_flags, no_alt, 
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN LU6.2 endpoint session information", PRIV_USER);
KEYWORD_ID(show_appn_sess_detail, show_appn_sess_flags, show_appn_sess_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN LU6.2 endpoint session information", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_sess_detail

APPN_NAME_STAR (show_appn_sess_name, show_appn_sess_flags, no_alt,
	   OBJ(string, 2), "Show specific APPN sessions by fully-qualified LU name");
KEYWORD_ID(show_appn_sess_name_flag, show_appn_sess_name, NEXT_FILTER,
	   OBJ(int,4), TRUE,
           "name", "Show specific APPN sessions by local or remote LU name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_sess_name_flag

APPN_PCID (show_appn_sess_pcid, show_appn_sess_flags, no_alt,
	   OBJ(string, 1), "Show specific APPN sessions by PCID");
KEYWORD_ID(show_appn_sess_pcid_flag, show_appn_sess_pcid, NEXT_FILTER,
	   OBJ(int,3), TRUE,
           "pcid", "Show specific APPN sessions by PCID", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_sess_pcid_flag

NOP       (show_appn_sess_nop, NEXT_FILTER, show_appn_eol);
HELP      (show_appn_sess_flags, show_appn_sess_nop,
           "show appn sesion [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_sess, show_appn_sess_flags, show_appn_mode,
	   OBJ(int,1), APPN_SHOW_SESS,
	   "session", "Show APPN LU6.2 endpoint session information", PRIV_USER);

pdecl     (show_appn_port_flags);
KEYWORD_ID(show_appn_port_brief, show_appn_port_flags, no_alt,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN port information", PRIV_USER);
KEYWORD_ID(show_appn_port_detail, show_appn_port_flags, show_appn_port_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN port information", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_port_detail

APPN_TYPE_A_NAME_STAR (show_appn_port_name, show_appn_port_flags, no_alt,
	   OBJ(string, 1), "Show specific APPN ports by name");
KEYWORD_ID(show_appn_port_name_flag, show_appn_port_name, NEXT_FILTER,
	   OBJ(int,3), TRUE,
           "name", "Show specific APPN ports by port name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_port_name_flag

NOP       (show_appn_port_nop, NEXT_FILTER, show_appn_eol);
HELP      (show_appn_port_flags, show_appn_port_nop,
           "show appn port [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_port, show_appn_port_flags, show_appn_sess,
	   OBJ(int,1), APPN_SHOW_PORT,
	   "port", "Show APPN port information", PRIV_USER);

pdecl     (show_appn_link_flags);
KEYWORD_ID(show_appn_link_brief, show_appn_link_flags, no_alt,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN link station information", PRIV_USER);
KEYWORD_ID(show_appn_link_detail, show_appn_link_flags, show_appn_link_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN link station information", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_link_detail

/*
 * cpname netid.cpname
 */
APPN_NAME_STAR(show_appn_link_cpname, show_appn_link_flags, no_alt,
            OBJ(string,4),"Show specific appn link stations by CP name" );
KEYWORD_ID(show_appn_link_cpname_flag, show_appn_link_cpname, NEXT_FILTER,
           OBJ(int, 7), TRUE,
           "cpname",
           "Show specific appn link stations by CP name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_link_cpname_flag

/*
 *     xid nodeid (idnum/idblk )
*/
HEXDIGIT  (show_appn_link_xid_xid, show_appn_link_flags, no_alt,
           OBJ(int,6), 0, 0xFFFFFFFF, APPN_CP_XID_VAL_HELP);
NOPREFIX  (show_appn_link_xid_noprefix, show_appn_link_xid_xid, no_alt);
NVGENS    (show_appn_link_xid_nvgen, show_appn_link_xid_noprefix,
           appn_show_command, APPN_CP_XID);
KEYWORD_ID(show_appn_link_xid, show_appn_link_xid_nvgen, NEXT_FILTER,
           OBJ(int, 5), TRUE,
           "xid", "Show specific APPN link stations by idnum/idblk",
           PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_link_xid

APPN_TYPE_A_NAME_STAR (show_appn_link_portname, show_appn_link_flags, no_alt,
	   OBJ(string, 2), "Show specific APPN link stations by port name");
KEYWORD_ID(show_appn_link_portname_flag, show_appn_link_portname, NEXT_FILTER, 
	   OBJ(int,4), TRUE,
           "port", "Show specific APPN link stations by port name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_link_portname_flag

APPN_TYPE_A_NAME_STAR (show_appn_link_linkname, show_appn_link_flags, no_alt,
	   OBJ(string, 1), "Show specific APPN link stations by link name");
KEYWORD_ID(show_appn_link_linkname_flag, show_appn_link_linkname, NEXT_FILTER,
	   OBJ(int,3), TRUE,
           "name", "Show specific APPN link stations by link name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_link_linkname_flag

NOP       (show_appn_link_nop, NEXT_FILTER, show_appn_eol);
HELP      (show_appn_link_flags, show_appn_link_nop,
           "show appn link-station [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_link, show_appn_link_flags, show_appn_port,
	   OBJ(int,1), APPN_SHOW_LINK,
	   "link-station", "Show APPN link station information", PRIV_USER);

pdecl     (show_appn_isr_flags);
KEYWORD_ID(show_appn_isr_brief, show_appn_isr_flags, no_alt, 
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN LU6.2 intermediate session information", PRIV_USER);
KEYWORD_ID(show_appn_isr_detail, show_appn_isr_flags, show_appn_isr_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN LU6.2 intermediate session information", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_isr_detail

APPN_NAME_STAR (show_appn_isr_name, show_appn_isr_flags, no_alt,
	   OBJ(string, 2), "Show specific APPN intermediate sessions by LU name");
KEYWORD_ID(show_appn_isr_name_flag, show_appn_isr_name, NEXT_FILTER,
	   OBJ(int,4), TRUE,
           "name", "Show specific APPN intermediate sessions by LU name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_isr_name_flag

APPN_PCID (show_appn_isr_pcid, show_appn_isr_flags, no_alt,
	   OBJ(string, 1), "Show specific APPN intermediate-sessions by PCID");
KEYWORD_ID(show_appn_isr_pcid_flag, show_appn_isr_pcid, NEXT_FILTER,
	   OBJ(int,3), TRUE,
           "pcid", "Show specific APPN intermediate-sessions by PCID", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_isr_pcid_flag

NOP       (show_appn_isr_nop, NEXT_FILTER, show_appn_eol);
HELP      (show_appn_isr_flags, show_appn_isr_nop,
           "show appn intermediate-session [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_isr, show_appn_isr_flags, show_appn_link,
	   OBJ(int,1), APPN_SHOW_ISR,
	   "intermediate-session", "Show APPN intermediate session information", PRIV_USER);

pdecl     (show_appn_dir_flags);
KEYWORD_ID(show_appn_dir_brief, show_appn_dir_flags, no_alt,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN directory information", PRIV_USER);
KEYWORD_ID(show_appn_dir_detail, show_appn_dir_flags, show_appn_dir_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN directory information", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_dir_detail

APPN_NAME_STAR (show_appn_dir_name, show_appn_dir_flags, no_alt,
	   OBJ(string, 1), "Show specific APPN directory resources by name");
KEYWORD_ID(show_appn_dir_name_flag, show_appn_dir_name, NEXT_FILTER,
	   OBJ(int,3), TRUE,
           "name", "Show specific APPN directory resources by name", PRIV_USER);
#undef  NEXT_FILTER
#define NEXT_FILTER show_appn_dir_name_flag

NOP       (show_appn_dir_nop, NEXT_FILTER, show_appn_eol);
HELP      (show_appn_dir_flags, show_appn_dir_nop,
           "show appn directory [filters] [brief|detail]\n");
KEYWORD_ID(show_appn_dir, show_appn_dir_flags, show_appn_isr,
	   OBJ(int,1), APPN_SHOW_DIR,
	   "directory", "Show APPN directory database information", PRIV_USER);

KEYWORD_ID(show_appn_cos_brief, show_appn_eol, show_appn_eol,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN class of service information", PRIV_USER);
KEYWORD_ID(show_appn_cos_detail, show_appn_eol, show_appn_cos_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN class of service information", PRIV_USER);
KEYWORD_ID(show_appn_cos, show_appn_cos_detail, show_appn_dir,
	   OBJ(int,1), APPN_SHOW_COS,
	   "class-of-service", "Show APPN class of service information", PRIV_USER);
KEYWORD_ID(show_appn_cn_brief, show_appn_eol, show_appn_eol,
	   OBJ(int,2), FALSE,
	   "brief", "Show brief APPN connection network information", PRIV_USER);
KEYWORD_ID(show_appn_cn_detail, show_appn_eol, show_appn_cn_brief,
	   OBJ(int,2), TRUE,
	   "detail", "Show detailed APPN connection network information", PRIV_USER);
KEYWORD_ID(show_appn_cn, show_appn_cn_detail, show_appn_cos,
   OBJ(int,1), APPN_SHOW_CN,
	   "connection-network", "Show APPN connection network information", PRIV_USER);
KEYWORD (show_appn, show_appn_cn, ALTERNATE,
	 "appn", "APPN show commands", PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_appn
