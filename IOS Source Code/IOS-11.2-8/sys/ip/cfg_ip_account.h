/* $Id: cfg_ip_account.h,v 3.2 1995/11/17 09:32:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/ip/cfg_ip_account.h,v $
 *------------------------------------------------------------------
 * IP accounting config commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_ip_account.h,v $
 * Revision 3.2  1995/11/17  09:32:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:37  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:08:14  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 *	ip accounting-transits <count>
 *	no ip accounting-transits [<count>]
 *
 * OBJ(int,1) = num of transit records to store
 */
PARAMS_KEYONLY(cip_accounting_transits, ALTERNATE,
	       "accounting-transits",
	       OBJ(int,1), 0, 32000,
	       ip_accttransits, IP_ACCTTRANSITS,
	       "Sets the maximum number of transit entries",
	       "Max transit entries", PRIV_CONF);

/******************************************************************************
 *	ip accounting-list <ipaddr> <mask>
 *	no ip accounting-list [<ipaddr> <mask>]
 *
 * OBJ(int,1) =	TRUE if an accounting address and mask given
 *		FALSE for 'no ip accounting-list'
 *
 * (*OBJ(paddr,1)) = ipaddr
 * (*OBJ(paddr,2)) = mask
 *
 */
EOLS	(cip_acctlist_eol, ip_acctlist, IP_ACCTLIST);
SET	(cip_acctlist_set, cip_acctlist_eol, OBJ(int,1), TRUE);
IPADDR	(cip_acctlist_get_mask, cip_acctlist_set, no_alt,
	 OBJ(paddr,2), "IP address mask");

/* Check for an IP address.
 * If there was no IP address, then check for a 'no' prefix and look for EOL.
 */
TESTVAR	(cip_acctlist_noprefix, cip_acctlist_eol, no_alt,
	 NONE, NONE, NONE, sense, FALSE);
IPADDR	(cip_acctlist_get_addr, cip_acctlist_get_mask, cip_acctlist_noprefix,
	 OBJ(paddr,1), "IP address of host");
NVGENS	(cip_acctlist_nvgen, cip_acctlist_get_addr, ip_acctlist, IP_ACCTLIST);
KEYWORD	(cip_acctlist, cip_acctlist_nvgen, cip_accounting_transits,
	 "accounting-list",
	 "Select hosts for which IP accounting information is kept",
	 PRIV_CONF);

/******************************************************************************
 *	ip accounting-threshold <threshold>
 *	no ip accounting-threshold [<threshold>]
 *
 * OBJ(int,1) = max accounting entries
 */
PARAMS_KEYONLY(cip_accounting_threshold, cip_acctlist,
	       "accounting-threshold",
	       OBJ(int,1), 0, -1,
	       ip_acctthreshold, IP_ACCTTHRESHOLD,
	       "Sets the maximum number of accounting entries",
	       "Max acct entries", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	cip_accounting_threshold
