/* $Id: exec_show_rttmon.h,v 1.1.4.2 1996/06/07 22:25:49 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/exec_show_rttmon.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain header file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the definitions for
 * the RTT Monitor show parse chain.
 *------------------------------------------------------------------
 * $Log: exec_show_rttmon.h,v $
 * Revision 1.1.4.2  1996/06/07  22:25:49  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.3  1996/05/14  06:20:12  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Response Time Reporter code review updates
 *
 * Revision 1.1.2.2  1996/04/19  02:41:16  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:29  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:22  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

LINK_TRANS(show_rttmon_extend_here, NONE);

/******************************************************************
 * show rtr application [ tabular | full ]
 *
 */
EOLS(show_rttmon_appl_raw_eol,
     rttmon_show_appl_raw_command, 0);

EOLS(show_rttmon_appl_full_eol,
     rttmon_show_appl_full_command, 0);

EOLS(show_rttmon_appl_tabular_eol,
     rttmon_show_appl_tabular_command, 0);

EOLS(show_rttmon_appl_def_eol,
     rttmon_show_appl_def_command, 0);

KEYWORD(show_rttmon_appl_tabular,
	show_rttmon_appl_tabular_eol,
	show_rttmon_appl_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_appl_full,
	show_rttmon_appl_full_eol,
	show_rttmon_appl_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_appl_raw,
	show_rttmon_appl_raw_eol,
	show_rttmon_appl_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

KEYWORD(show_rttmon_appl, show_rttmon_appl_raw,
	show_rttmon_extend_here,
	D_rttMonAppl_string,
	"RTR Application", PRIV_USER);

/******************************************************************
 * show rtr reaction-trigger [ <entry-number> ] [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number
 */
EOLS(show_rttmon_react_entry_raw_eol,
     rttmon_show_react_trigger_raw_command, 0);

EOLS(show_rttmon_react_entry_full_eol,
     rttmon_show_react_trigger_full_command, 0);

EOLS(show_rttmon_react_entry_tabular_eol,
     rttmon_show_react_trigger_tabular_command, 0);

EOLS(show_rttmon_react_entry_def_eol,
     rttmon_show_react_trigger_def_command, 0);

KEYWORD(show_rttmon_react_entry_tabular,
	show_rttmon_react_entry_tabular_eol,
	show_rttmon_react_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_react_entry_full,
	show_rttmon_react_entry_full_eol,
	show_rttmon_react_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_react_entry_raw,
	show_rttmon_react_entry_raw_eol,
	show_rttmon_react_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_react_entry_number,
    show_rttmon_react_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_react_entry_no_number,
    show_rttmon_react_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_react_entry,
       show_rttmon_react_entry_number,
       show_rttmon_react_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_react, show_rttmon_react_entry,
	show_rttmon_appl,
	D_rttMonReactTriggerAdmin_string,
	"RTR Reaction Trigger", PRIV_USER);

/******************************************************************
 * show rtr history [ <entry-number> ] [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number
 */
EOLS(show_rttmon_history_entry_raw_eol,
     rttmon_show_hist_raw_command, 0);

EOLS(show_rttmon_history_entry_full_eol,
     rttmon_show_hist_full_command, 0);

EOLS(show_rttmon_history_entry_tabular_eol,
     rttmon_show_hist_tabular_command, 0);

EOLS(show_rttmon_history_entry_def_eol,
     rttmon_show_hist_def_command, 0);

KEYWORD(show_rttmon_history_entry_tabular,
	show_rttmon_history_entry_tabular_eol,
	show_rttmon_history_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_history_entry_full,
	show_rttmon_history_entry_full_eol,
	show_rttmon_history_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_history_entry_raw,
	show_rttmon_history_entry_raw_eol,
	show_rttmon_history_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_history_entry_number,
    show_rttmon_history_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_history_entry_no_number,
    show_rttmon_history_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_history_entry,
       show_rttmon_history_entry_number,
       show_rttmon_history_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_history, show_rttmon_history_entry,
	show_rttmon_react,
	D_rttMonHistoryAdmin_string, "RTR History",
	PRIV_USER);

/******************************************************************
 * show rtr distribution-statistics [ <entry-number> ] 
 *                                  [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number
 */
EOLS(show_rttmon_stat_dist_entry_raw_eol,
     rttmon_show_stat_dist_raw_command, 0);

EOLS(show_rttmon_stat_dist_entry_full_eol,
     rttmon_show_stat_dist_full_command, 0);

EOLS(show_rttmon_stat_dist_entry_tabular_eol,
     rttmon_show_stat_dist_tabular_command, 0);

EOLS(show_rttmon_stat_dist_entry_def_eol,
     rttmon_show_stat_dist_def_command, 0);

KEYWORD(show_rttmon_stat_dist_entry_tabular,
	show_rttmon_stat_dist_entry_tabular_eol,
	show_rttmon_stat_dist_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_stat_dist_entry_full,
	show_rttmon_stat_dist_entry_full_eol,
	show_rttmon_stat_dist_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_stat_dist_entry_raw,
	show_rttmon_stat_dist_entry_raw_eol,
	show_rttmon_stat_dist_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_stat_dist_entry_number,
    show_rttmon_stat_dist_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_stat_dist_entry_no_number,
    show_rttmon_stat_dist_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_stat_dist_entry,
       show_rttmon_stat_dist_entry_number,
       show_rttmon_stat_dist_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_stat_dist, show_rttmon_stat_dist_entry,
	show_rttmon_history,
	D_rttMonStatisticsAdminDistStats_string,
	"RTR Statistic Distributions", PRIV_USER);

/******************************************************************
 * show rtr collection-statistics [ <entry-number> ] [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number
 */
EOLS(show_rttmon_stat_coll_entry_raw_eol,
     rttmon_show_stat_collect_raw_command, 0);

EOLS(show_rttmon_stat_coll_entry_full_eol,
     rttmon_show_stat_collect_full_command, 0);

EOLS(show_rttmon_stat_coll_entry_tabular_eol,
     rttmon_show_stat_collect_tabular_command, 0);

EOLS(show_rttmon_stat_coll_entry_def_eol,
     rttmon_show_stat_collect_def_command, 0);

KEYWORD(show_rttmon_stat_coll_entry_tabular,
	show_rttmon_stat_coll_entry_tabular_eol,
	show_rttmon_stat_coll_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_stat_coll_entry_full,
	show_rttmon_stat_coll_entry_full_eol,
	show_rttmon_stat_coll_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_stat_coll_entry_raw,
	show_rttmon_stat_coll_entry_raw_eol,
	show_rttmon_stat_coll_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_stat_coll_entry_number,
    show_rttmon_stat_coll_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_stat_coll_entry_no_number,
    show_rttmon_stat_coll_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_stat_coll_entry,
       show_rttmon_stat_coll_entry_number,
       show_rttmon_stat_coll_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_stat_coll, show_rttmon_stat_coll_entry,
	show_rttmon_stat_dist,
	D_rttMonStatisticsAdminCollStats_string,
	"RTR Statistic Collections", PRIV_USER);

/******************************************************************
 * show rtr totals-statistics [ <entry-number> ] [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number 
 */
EOLS(show_rttmon_stat_totals_entry_raw_eol,
     rttmon_show_stat_totals_raw_command, 0);

EOLS(show_rttmon_stat_totals_entry_full_eol,
     rttmon_show_stat_totals_full_command, 0);

EOLS(show_rttmon_stat_totals_entry_tabular_eol,
     rttmon_show_stat_totals_tabular_command, 0);

EOLS(show_rttmon_stat_totals_entry_def_eol,
     rttmon_show_stat_totals_def_command, 0);

KEYWORD(show_rttmon_stat_totals_entry_tabular,
	show_rttmon_stat_totals_entry_tabular_eol,
	show_rttmon_stat_totals_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_stat_totals_entry_full,
	show_rttmon_stat_totals_entry_full_eol,
	show_rttmon_stat_totals_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_stat_totals_entry_raw,
	show_rttmon_stat_totals_entry_raw_eol,
	show_rttmon_stat_totals_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_stat_totals_entry_number,
    show_rttmon_stat_totals_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_stat_totals_entry_no_number,
    show_rttmon_stat_totals_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_stat_totals_entry,
       show_rttmon_stat_totals_entry_number,
       show_rttmon_stat_totals_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_stat_totals, show_rttmon_stat_totals_entry,
	show_rttmon_stat_coll,
	D_rttMonStatisticsAdminTotalsStats_string,
	"RTR Statistics Totals", PRIV_USER);

/******************************************************************
 * show rtr operational-state [ <entry-number> ] [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number
 */
EOLS(show_rttmon_oper_entry_raw_eol,
     rttmon_show_oper_raw_command, 0);

EOLS(show_rttmon_oper_entry_full_eol,
     rttmon_show_oper_full_command, 0);

EOLS(show_rttmon_oper_entry_tabular_eol,
     rttmon_show_oper_tabular_command, 0);

EOLS(show_rttmon_oper_entry_def_eol,
     rttmon_show_oper_def_command, 0);

KEYWORD(show_rttmon_oper_entry_tabular,
	show_rttmon_oper_entry_tabular_eol,
	show_rttmon_oper_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_oper_entry_full,
	show_rttmon_oper_entry_full_eol,
	show_rttmon_oper_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_oper_entry_raw,
	show_rttmon_oper_entry_raw_eol,
	show_rttmon_oper_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_oper_entry_number,
    show_rttmon_oper_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_oper_entry_no_number,
    show_rttmon_oper_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_oper_entry, show_rttmon_oper_entry_number,
       show_rttmon_oper_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_oper, show_rttmon_oper_entry,
	show_rttmon_stat_totals,
	D_rttMonCtrlOper_string,
	"RTR Operational State", PRIV_USER);


/******************************************************************
 * show rtr configuration [ <entry-number> ] [ tabular | full ]
 *
 * OBJ(int,1) = <entry-number>
 * OBJ(int,2) = 1 = entry number
 *              0 = no entry number
 */
EOLS(show_rttmon_cfg_entry_raw_eol,
     rttmon_show_cfg_raw_command, 0);

EOLS(show_rttmon_cfg_entry_full_eol,
     rttmon_show_cfg_full_command, 0);

EOLS(show_rttmon_cfg_entry_tabular_eol,
     rttmon_show_cfg_tabular_command, 0);

EOLS(show_rttmon_cfg_entry_def_eol,
     rttmon_show_cfg_def_command, 0);

KEYWORD(show_rttmon_cfg_entry_tabular,
	show_rttmon_cfg_entry_tabular_eol,
	show_rttmon_cfg_entry_def_eol,
	D_rttMonShowTabular_string,
	"Compact Output", PRIV_USER);

KEYWORD(show_rttmon_cfg_entry_full,
	show_rttmon_cfg_entry_full_eol,
	show_rttmon_cfg_entry_tabular,
	D_rttMonShowFull_string,
	"Listed Output", PRIV_USER);

KEYWORD(show_rttmon_cfg_entry_raw,
	show_rttmon_cfg_entry_raw_eol,
	show_rttmon_cfg_entry_full,
	D_rttMonShowRaw_string,
	"Without header and more", PRIV_HIDDEN);

SET(show_rttmon_cfg_entry_number,
    show_rttmon_cfg_entry_raw, OBJ(int, 2), 1);

SET(show_rttmon_cfg_entry_no_number,
    show_rttmon_cfg_entry_raw, OBJ(int, 2), 0);

NUMBER(show_rttmon_cfg_entry, show_rttmon_cfg_entry_number,
       show_rttmon_cfg_entry_no_number, OBJ(int, 1), 1,
       MAX_rttMonCtrlAdminIdx, "Entry Number");

KEYWORD(show_rttmon_cfg, show_rttmon_cfg_entry, show_rttmon_oper,
	D_rttMonCtrlAdmin_string,
	"RTR Configuration", PRIV_USER);

/******************************************************************
 * (show) rtr ...
 */
KEYWORD(show_rttmon, show_rttmon_cfg, ALTERNATE,
	D_rttMon_string, "Response Time Reporter (RTR)",
	PRIV_USER);

#undef	ALTERNATE
#define	ALTERNATE	show_rttmon
