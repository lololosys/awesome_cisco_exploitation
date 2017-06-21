/* $Id: cfg_dnsix.h,v 3.2 1995/11/17 18:50:35 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/cfg_dnsix.h,v $
 *------------------------------------------------------------------
 * C F G _ D N S I X . H
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_dnsix.h,v $
 * Revision 3.2  1995/11/17  18:50:35  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  01:56:41  tli
 * CSCdi36508: Rationalize parser file placement, part I
 * Rationalize cls dev dlsw dspu filesys irix lat les lex llc2 parser pgen
 * pt servers smf smrp snmp src-36-c1000 src-4k-c4000 src-68-c3000
 * src-68-c4000 src-c4000 src-rsp srt tcl ts ui
 *
 * Revision 2.1  1995/06/07  22:02:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************
 * dnsix-nat source <address>
 *           primary <address>
 *           secondary <address>
 *           authorized-redirection <address>
 *           transmit <count>
 *           test <seconds>
 * 
 * OBJ(int,1) = Keyword
 * OBJ(int,2) = transmit count
 * OBJ(paddr,1) = ip address
 *
 *
 * dnsix-dmdp 
 */
/******************************************************************************/
EOLS	(conf_dnsix_eol, dnsix_command, DNSIX_ATM);

/* <addr1> */
IPADDR	(conf_dnsix_addr, conf_dnsix_eol, conf_dnsix_eol,
	 OBJ(paddr,1), "Ip address for the service");

NOPREFIX (conf_dnsix_noprefix, conf_dnsix_addr, conf_dnsix_eol);

NVGENS	(conf_dnsix_nvgen, conf_dnsix_noprefix, dnsix_command, DNSIX_ATM);

/* auth_redirect */
KEYWORD_ID (conf_dnsix_redirect, conf_dnsix_nvgen, no_alt,
	 OBJ(int,1), DNSIX_ATM_AUTHO_REDIRECT,
	 "authorized-redirection", "Dnsix redirect address", PRIV_CONF);

NUMBER	(conf_dnsix_num, conf_dnsix_eol, no_alt,
	 OBJ(int,2), 1, 200, "Transmit count or test frequency");

NOPREFIX (conf_dnsix_num_noprefix, conf_dnsix_num, conf_dnsix_eol);

NVGENS	(conf_dnsix_num_nvgen, conf_dnsix_num_noprefix,
	 dnsix_command, DNSIX_ATM);

/* test */
KEYWORD_ID (conf_dnsix_test, conf_dnsix_num_nvgen, conf_dnsix_redirect,
	 OBJ(int,1), DNSIX_ATM_TEST,
	 "test", "Audit Trail test", PRIV_CONF | PRIV_HIDDEN);

/* transmit count */
KEYWORD_ID (conf_dnsix_transmit, conf_dnsix_num_nvgen, conf_dnsix_test,
	 OBJ(int,1), DNSIX_ATM_TRANSMIT_COUNT,
	 "transmit-count", "Audit Trail transmit count", PRIV_CONF);

/* secondary address */
KEYWORD_ID (conf_dnsix_secondary, conf_dnsix_nvgen, conf_dnsix_transmit,
	 OBJ(int,1), DNSIX_ATM_SECONDARY,
	 "secondary", "Audit Trail secondary address", PRIV_CONF);

/* primary address */
KEYWORD_ID (conf_dnsix_primary, conf_dnsix_nvgen, conf_dnsix_secondary,
	 OBJ(int,1), DNSIX_ATM_PRIMARY,
	 "primary", "Audit Trail primary address", PRIV_CONF);

/* source address */
KEYWORD_ID (conf_dnsix_source, conf_dnsix_nvgen, conf_dnsix_primary,
	    OBJ(int,1), DNSIX_ATM_SOURCE,
	    "source", "Audit Trail source address", PRIV_CONF);

KEYWORD	(dnsix_nat, conf_dnsix_source, ALTERNATE,
	 "dnsix-nat", "Provide DNSIX service for audit trails", PRIV_CONF);

/******************************************************************
 * dnsix-dmdp retries <count>
 * 
 * OBJ(int,1) = Keyword
 * OBJ(int,2) = retries count
 *
 *
 * dnsix-dmdp 
 */
/******************************************************************/
EOLS	(conf_dmdp_eol, dmdp_command, DNSIX_DMDP);

NUMBER	(conf_dmdp_num, conf_dmdp_eol, no_alt,
	 OBJ(int,2), 0, 200, "Retries count");

NOPREFIX (conf_dmdp_noprefix, conf_dmdp_num, conf_dmdp_eol);

NVGENS	(conf_dmdp_nvgen, conf_dmdp_noprefix, dmdp_command, DNSIX_DMDP);

/* retries count */
KEYWORD_ID (conf_dmdp_retries, conf_dmdp_nvgen, no_alt,
	    OBJ(int,1), DNSIX_DMDP_RETRIES,
	    "retries", "Message retransmit count", PRIV_CONF);

KEYWORD	(dnsix_dmdp, conf_dmdp_retries, dnsix_nat,
	 "dnsix-dmdp", "Provide DMDP service for DNSIX", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	dnsix_dmdp






