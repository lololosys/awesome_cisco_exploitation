/* $Id: cfg_rttmon_entry_exit.h,v 1.1.4.2 1996/06/07 22:25:47 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/cfg_rttmon_entry_exit.h,v $
 *------------------------------------------------------------------
 * Round Trip Time Monitor parse chain rtr entry mode exit file.
 *
 * Dec 1995, Larry Metzger
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * This header file contains the definitions for
 * the RTT Monitor configuration entry mode exit parse chain.
 *------------------------------------------------------------------
 * $Log: cfg_rttmon_entry_exit.h,v $
 * Revision 1.1.4.2  1996/06/07  22:25:47  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:40:43  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.2  1996/04/19  02:41:14  lmetzger
 * Branch: IbuMod_Calif_branch
 *
 * Updating Initial Version of Response Time Reporter
 *
 * Revision 1.1.2.1  1996/03/22  18:41:27  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:20  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/***************************************************************
 * exit - exit from rtr entry configuration command
 * 
 */

EOLS(rttmon_exit_eol, rttmon_exit_config_submode,
     RTR_ENTRY_CFG_EXIT);

EOLS(rttmon_end_eol, rttmon_exit_config_submode,
     RTR_ENTRY_CFG_END);

KEYWORD(end_configure, rttmon_end_eol, NONE, "end",
	"Exit from configure mode", PRIV_MIN);

KEYWORD(rttmon_exit_kwd, rttmon_exit_eol, end_configure,
	"exit",
	"Exit from response time reporter entry configuration submode",
	PRIV_MIN);

TESTVAR(rttmon_exit, rttmon_exit_kwd, NONE, NONE, NONE,
	ALTERNATE, sense, TRUE);

#undef  ALTERNATE
#define ALTERNATE       rttmon_exit
