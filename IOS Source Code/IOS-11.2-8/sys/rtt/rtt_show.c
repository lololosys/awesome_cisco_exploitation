/* $Id: rtt_show.c,v 1.1.4.2 1996/06/07 22:26:24 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_show.c,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor Show Command Utilities.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This file contains the RTT Monitor Parser Show Command utilities.
 *------------------------------------------------------------------
 * $Log: rtt_show.c,v $
 * Revision 1.1.4.2  1996/06/07  22:26:24  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:40  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:53  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:48  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:35  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "rtt_util_ext.h"
#include "../rtt/rtt_show.h"
#include "rtt_defaults.h"

void rttmon_show_appl_def_command (parseinfo * csb)
{
    if (DEF_show_appl_type == rtt_output_full) {
	rttMonOutputAppl(rtt_output_full);
    } else {
	rttMonOutputAppl(rtt_output_brief);
    }
} 

void rttmon_show_appl_full_command (parseinfo * csb)
{
    rttMonOutputAppl(rtt_output_full);
} 

void rttmon_show_appl_raw_command (parseinfo * csb)
{
    rttMonOutputAppl(rtt_output_raw);
} 

void rttmon_show_appl_tabular_command (parseinfo * csb)
{
    rttMonOutputAppl(rtt_output_brief);
} 

void rttmon_show_cfg_def_command (parseinfo * csb)
{
    if (DEF_show_cfg_type == rtt_output_full) {
	rttMonOutputCtrlTable(GETOBJ(int, 1),
			      GETOBJ(int, 2),
			      rtt_output_full);
    } else {
	rttMonOutputCtrlTable(GETOBJ(int, 1),
			      GETOBJ(int, 2),
			      rtt_output_brief);
    }
} 

void rttmon_show_cfg_full_command (parseinfo * csb)
{
    rttMonOutputCtrlTable(GETOBJ(int, 1),
			  GETOBJ(int, 2),
			  rtt_output_full);
} 

void rttmon_show_cfg_raw_command (parseinfo * csb)
{
    rttMonOutputCtrlTable(GETOBJ(int, 1),
			  GETOBJ(int, 2),
			  rtt_output_raw);
} 

void rttmon_show_cfg_tabular_command (parseinfo * csb)
{
    rttMonOutputCtrlTable(GETOBJ(int, 1),
			  GETOBJ(int, 2),
			  rtt_output_brief);
} 

void rttmon_show_oper_def_command (parseinfo * csb)
{
    if (DEF_show_oper_type == rtt_output_full) {
	rttMonOutputOperTable(GETOBJ(int, 1),
			      GETOBJ(int, 2),
			      rtt_output_full);
    } else {
	rttMonOutputOperTable(GETOBJ(int, 1),
			      GETOBJ(int, 2),
			      rtt_output_brief);
    }
} 

void rttmon_show_oper_full_command (parseinfo * csb)
{
    rttMonOutputOperTable(GETOBJ(int, 1),
			  GETOBJ(int, 2),
			  rtt_output_full);
} 

void rttmon_show_oper_raw_command (parseinfo * csb)
{
    rttMonOutputOperTable(GETOBJ(int, 1),
			  GETOBJ(int, 2),
			  rtt_output_raw);
} 

void rttmon_show_oper_tabular_command (parseinfo * csb)
{
    rttMonOutputOperTable(GETOBJ(int, 1),
			  GETOBJ(int, 2),
			  rtt_output_brief);
} 

void rttmon_show_stat_totals_def_command (parseinfo * csb)
{
    if (DEF_show_stat_totals_type == rtt_output_full) {
	rttMonOutputStatsTotalsTable(GETOBJ(int, 1),
				     GETOBJ(int, 2),
				     rtt_output_full);
    } else {
	rttMonOutputStatsTotalsTable(GETOBJ(int, 1),
				     GETOBJ(int, 2),
				     rtt_output_brief);
    }
} 

void rttmon_show_stat_totals_full_command (parseinfo * csb)
{
    rttMonOutputStatsTotalsTable(GETOBJ(int, 1),
				 GETOBJ(int, 2),
				 rtt_output_full);
} 

void rttmon_show_stat_totals_raw_command (parseinfo * csb)
{
    rttMonOutputStatsTotalsTable(GETOBJ(int, 1),
				 GETOBJ(int, 2),
				 rtt_output_raw);
} 

void rttmon_show_stat_totals_tabular_command (parseinfo * csb)
{
    rttMonOutputStatsTotalsTable(GETOBJ(int, 1),
				 GETOBJ(int, 2),
				 rtt_output_brief);
} 

void rttmon_show_stat_collect_def_command (parseinfo * csb)
{
    if (DEF_show_stat_collect_type == rtt_output_full) {
	rttMonOutputStatsCollTable(GETOBJ(int, 1),
				   GETOBJ(int, 2),
				   rtt_output_full);
    } else {
	rttMonOutputStatsCollTable(GETOBJ(int, 1),
				   GETOBJ(int, 2),
				   rtt_output_brief);
    }
} 

void rttmon_show_stat_collect_full_command (parseinfo * csb)
{
    rttMonOutputStatsCollTable(GETOBJ(int, 1),
			       GETOBJ(int, 2),
			       rtt_output_full);
} 

void rttmon_show_stat_collect_raw_command (parseinfo * csb)
{
    rttMonOutputStatsCollTable(GETOBJ(int, 1),
			       GETOBJ(int, 2),
			       rtt_output_raw);
} 

void rttmon_show_stat_collect_tabular_command (parseinfo * csb)
{
    rttMonOutputStatsCollTable(GETOBJ(int, 1),
			       GETOBJ(int, 2),
			       rtt_output_brief);
} 

void rttmon_show_stat_dist_def_command (parseinfo * csb)
{
    if (DEF_show_stat_dist_type == rtt_output_full) {
	rttMonOutputStatsDistTable(GETOBJ(int, 1),
				   GETOBJ(int, 2),
				   rtt_output_full);
    } else {
	rttMonOutputStatsDistTable(GETOBJ(int, 1),
				   GETOBJ(int, 2),
				   rtt_output_brief);
    }
} 

void rttmon_show_stat_dist_full_command (parseinfo * csb)
{
    rttMonOutputStatsDistTable(GETOBJ(int, 1),
			       GETOBJ(int, 2),
			       rtt_output_full);
} 

void rttmon_show_stat_dist_raw_command (parseinfo * csb)
{
    rttMonOutputStatsDistTable(GETOBJ(int, 1),
			       GETOBJ(int, 2),
			       rtt_output_raw);
} 

void rttmon_show_stat_dist_tabular_command (parseinfo * csb)
{
    rttMonOutputStatsDistTable(GETOBJ(int, 1),
			       GETOBJ(int, 2),
			       rtt_output_brief);
} 

void rttmon_show_hist_def_command (parseinfo * csb)
{
    if (DEF_show_hist_type == rtt_output_full) {
	rttMonOutputHistoryTable(GETOBJ(int, 1),
				 GETOBJ(int, 2),
				 rtt_output_full);
    } else {
	rttMonOutputHistoryTable(GETOBJ(int, 1),
				 GETOBJ(int, 2),
				 rtt_output_brief);
    }
} 

void rttmon_show_hist_full_command (parseinfo * csb)
{
    rttMonOutputHistoryTable(GETOBJ(int, 1),
			     GETOBJ(int, 2),
			     rtt_output_full);
} 

void rttmon_show_hist_raw_command (parseinfo * csb)
{
    rttMonOutputHistoryTable(GETOBJ(int, 1),
			     GETOBJ(int, 2),
			     rtt_output_raw);
} 

void rttmon_show_hist_tabular_command (parseinfo * csb)
{
    rttMonOutputHistoryTable(GETOBJ(int, 1),
			     GETOBJ(int, 2),
			     rtt_output_brief);
} 

void rttmon_show_react_trigger_def_command (parseinfo * csb)
{
    if (DEF_show_react_trigger_type == rtt_output_full) {
	rttMonOutputReactTriggerTable(GETOBJ(int, 1),
				      GETOBJ(int, 2),
				      rtt_output_full);
    } else {
	rttMonOutputReactTriggerTable(GETOBJ(int, 1),
				      GETOBJ(int, 2),
				      rtt_output_brief);
    }
} 

void rttmon_show_react_trigger_full_command (parseinfo * csb)
{
    rttMonOutputReactTriggerTable(GETOBJ(int, 1),
				  GETOBJ(int, 2),
				  rtt_output_full);
} 

void rttmon_show_react_trigger_raw_command (parseinfo * csb)
{
    rttMonOutputReactTriggerTable(GETOBJ(int, 1),
				  GETOBJ(int, 2),
				  rtt_output_raw);
} 

void rttmon_show_react_trigger_tabular_command (parseinfo * csb)
{
    rttMonOutputReactTriggerTable(GETOBJ(int, 1),
				  GETOBJ(int, 2),
				  rtt_output_brief);
}
