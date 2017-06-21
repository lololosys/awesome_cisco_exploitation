/* $Id: cfg_int_ip_gdp.h,v 3.3.20.1 1996/08/12 16:03:25 widmer Exp $
 * $Source: /release/112/cvs/Xsys/iprouting/cfg_int_ip_gdp.h,v $
 *------------------------------------------------------------------
 * GDP interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_gdp.h,v $
 * Revision 3.3.20.1  1996/08/12  16:03:25  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/01  23:33:52  widmer
 * CSCdi45325:  Change Keyword/Set to use Keyword_Id macro
 *
 * Revision 3.2  1995/11/17  17:31:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:04:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:15:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize iprouting
 *
 * Revision 2.1  1995/06/07  22:05:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/************************************************************************
 * ip gdp [{ holdtime <seconds> | priority <number> | reporttime <seconds> }]
 * no ip gdp [{ default-route |
 *		holdtime <seconds> | 
 *		priority <number> | 
 *		reporttime <seconds> }]
 *
 * OBJ(int,1) = GDP_CMD_ADDRESS, GDP_CMD_HOLDTIME, GDP_CMD_PRIORITY,
 *	        GDP_CMD_REPORTTIME, GDP_CMD_DEFAULTROUTE. GDP_CMD_ENABLE
 * OBJ(int,2) = <priority>, <seconds>, or <number>
 * OBJ(int,3) = <holdtime> (only for 'ip gdp address')
 * (*OBJ(paddr,1)) = ipaddr (only for 'ip gdp address')
 * 
 * Note that NVGENS and EOLS are only called once for all interface specific 
 * gdp commands.  This is because we have to generate 'ip gdp' before any
 * of the other commands so that default values for some gdp variables
 * are initialized.
 *
 * The command 'ip gdp address <ipaddr> <priority> <holdtime>' was
 * for GDP proxy stuff and should be ignored/flushed, according
 * to Greg Satz (Feb 26, 92).
 */

EOLNS	(ci_ip_gdp_eol, ip_gdp_command);

/* ip gdp */
SET	(ci_ip_gdp_enable, ci_ip_gdp_eol, OBJ(int,1), GDP_CMD_ENABLE);

/* reporttime <seconds> */
NUMBER	(ci_ip_gdp_rpttime_val, ci_ip_gdp_eol, no_alt,
	 OBJ(int,2), 0, -1, "Report time");
NOPREFIX(ci_ip_gdp_rpttime_noprefix, ci_ip_gdp_rpttime_val, ci_ip_gdp_eol);
KEYWORD_ID(ci_ip_gdp_rpttime, ci_ip_gdp_rpttime_noprefix, ci_ip_gdp_enable,
	   OBJ(int,1), GDP_CMD_REPORTTIME,
	   "reporttime", "Reporting interval", PRIV_CONF|PRIV_SUBIF);

/* priority <seconds> */
NUMBER	(ci_ip_gdp_prival, ci_ip_gdp_eol, no_alt,
	 OBJ(int,2), 0, 0xffff, "Priority");
NOPREFIX (ci_ip_gdp_pri_noprefix, ci_ip_gdp_prival, ci_ip_gdp_eol);
KEYWORD_ID(ci_ip_gdp_priority, ci_ip_gdp_pri_noprefix, ci_ip_gdp_rpttime,
	   OBJ(int,1), GDP_CMD_PRIORITY,
	   "priority",
	   "Router priority (default 100; larger is higher priority)",
	   PRIV_CONF|PRIV_SUBIF);

/* holdtime <seconds> */
NUMBER	(ci_ip_gdp_hold, ci_ip_gdp_eol, no_alt,
	 OBJ(int,2), 0, 0xffff, "Holdtime (default 15 seconds)");
NOPREFIX (ci_ip_gdp_holdtime_noprefix, ci_ip_gdp_hold, ci_ip_gdp_eol);
KEYWORD_ID(ci_ip_gdp_holdtime, ci_ip_gdp_holdtime_noprefix, ci_ip_gdp_priority,
	   OBJ(int,1), GDP_CMD_HOLDTIME,
	   "holdtime", "How long a receiver should believe the information",
	   PRIV_CONF|PRIV_SUBIF);

/* default-route */
KEYWORD_ID(ci_ip_gdp_defroute, ci_ip_gdp_eol, ci_ip_gdp_holdtime,
	   OBJ(int,1), GDP_CMD_DEFAULTROUTE,
	   "default-route", "Propagate the default route metric",
	   PRIV_CONF|PRIV_SUBIF);

NVGENNS	(ci_ip_gdp_nvgen, ci_ip_gdp_defroute, ip_gdp_command);

KEYWORD	(ci_ip_gdp, ci_ip_gdp_nvgen, ALTERNATE,
	 "gdp", "Gateway Discovery Protocol", PRIV_CONF|PRIV_SUBIF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_gdp
