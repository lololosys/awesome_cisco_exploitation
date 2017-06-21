/* $Id: cfg_async-bootp.h,v 3.3.12.1 1996/08/12 16:02:44 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_async-bootp.h,v $
 *------------------------------------------------------------------
 * C F G _ A S Y N C - B O O T P . H
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cfg_async-bootp.h,v $
 * Revision 3.3.12.1  1996/08/12  16:02:44  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1996/02/14  19:36:44  asb
 * CSCdi48113:  Should deliver NetBios NS [implement rest of RFC1877]
 *
 * Revision 3.2  1995/11/17  09:32:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:53:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/25  00:56:58  vandys
 * CSCdi40595:  async-bootp time-offset needs to allow negative offsets
 * Use signed number handling
 *
 * Revision 2.1  1995/07/01  12:55:33  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:01:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Note that the spaces around the ':' leader for the hostname
 * are optional.
 */

#define BOOTP_ADRESS_HELP "An IP address"

/******************************************************************
 * no async-bootp
 * no async-bootp <tag>[:<hostname>]
 * async-bootp <tag>[:<hostname>] <data> ...
 */

EOLNS	(conf_bp_eol, asyncbootp_command);

/* IP addresses.  Subnet-mask only accepts a single address. */
IPADDR	(conf_bp_data_addr8, conf_bp_eol, conf_bp_eol,
	 OBJ(paddr,8), BOOTP_ADRESS_HELP);
IPADDR	(conf_bp_data_addr7, conf_bp_data_addr8, conf_bp_eol,
	 OBJ(paddr,7), BOOTP_ADRESS_HELP);
IPADDR	(conf_bp_data_addr6, conf_bp_data_addr7, conf_bp_eol,
	 OBJ(paddr,6), BOOTP_ADRESS_HELP);
IPADDR	(conf_bp_data_addr5, conf_bp_data_addr6, conf_bp_eol,
	 OBJ(paddr,5), BOOTP_ADRESS_HELP);
IPADDR	(conf_bp_data_addr4, conf_bp_data_addr5, conf_bp_eol,
	 OBJ(paddr,4), BOOTP_ADRESS_HELP);
IPADDR	(conf_bp_data_addr3, conf_bp_data_addr4, conf_bp_eol,
	 OBJ(paddr,3), BOOTP_ADRESS_HELP);
TESTVAR	(conf_bp_data_addr_subnet, conf_bp_eol, conf_bp_data_addr3,
	 NONE, NONE, NONE,
	 OBJ(int,1), BOOTP_SUBNETMASK);
IPADDR	(conf_bp_data_addr2, conf_bp_data_addr_subnet, no_alt,
	 OBJ(paddr,2), BOOTP_ADRESS_HELP);

/* Only one string or number datum allowed */
STRING	(conf_bp_data_str, conf_bp_eol, NONE,
	 OBJ(string,1), "A name surrounded by quotes (\")");
ASSERT	(conf_bp_data_quote, conf_bp_data_str, no_alt,
	 csb->in_help || csb->line[csb->line_index] == '"');

/* Check for tags requiring numbers.  If fail, must need an IP addr */
SIGNED_DEC(conf_bp_data_time, conf_bp_eol, no_alt,
	 OBJ(int,2), -45000, 45000, "Time offset from UTC");
TESTVAR	(conf_bp_data3, conf_bp_data_time, conf_bp_data_addr2,
	 NONE, NONE, NONE,
	 OBJ(int,1), BOOTP_TIMEOFFSET);

NUMBER	(conf_bp_data_size, conf_bp_eol, no_alt,
	 OBJ(int,2), 0, 45000, "Blocks in the boot file");
TESTVAR	(conf_bp_data2, conf_bp_data_size, conf_bp_data3,
	 NONE, NONE, NONE,
	 OBJ(int,1), BOOTP_FILESIZE);

/* Check for tags requiring string data. If fail, check for number data. */
TESTVAR	(conf_bp_data1, conf_bp_data_quote, conf_bp_data2,
	 NONE, NONE, NONE,
	 OBJ(int,1), BOOTP_HOSTNAME);
TESTVAR	(conf_bp_data, conf_bp_data_quote, conf_bp_data1,
	 NONE, NONE, NONE,
	 OBJ(int,1), BOOTP_FILE);

/* 'no async-bootp tag [:host] [<data> ...]'
 * The ASSERT is in the alternate path from CHARACTER(':') and only
 * allows the alternate if no client name is
 * specified (i.e. no ':' at this token).
 * A better command syntax would not have required this weirdness.
 */
NOPREFIX (conf_bp_host_noprefix, conf_bp_data, conf_bp_eol);
ASSERT	(conf_bp_noprefix_test, conf_bp_host_noprefix, no_alt,
	 csb->line[csb->line_index] != ':');

/* [:hostname] */
IPADDR_NAME (conf_bp_colon_host, conf_bp_host_noprefix, no_alt,
	     OBJ(paddr,1), "Specific host to which this entry applies");
CHARACTER   (conf_bp_colon_char, conf_bp_colon_host, conf_bp_noprefix_test, ':');
HELP	    (conf_bp_colon, conf_bp_colon_char,
	     "  :Host or :A.B.C.D\tColon and name/address of a specific client\n");

/* Only allow "bootfile-size" if we've not already matched "bootfile" */
KEYWORD_ID(conf_bp_bfilesizekwd, conf_bp_colon, NONE,
	   OBJ(int,1), BOOTP_FILESIZE,
	   "bootfile-size", "Set the boot file size", PRIV_CONF);
TEST_MULTIPLE_FUNCS(conf_bp_bfilesize, conf_bp_bfilesizekwd, NONE, no_alt);

KEYWORD_ID(conf_bp_hostname, conf_bp_colon, conf_bp_bfilesize,
	   OBJ(int,1), BOOTP_HOSTNAME,
	   "hostname", "Set client hostname", PRIV_CONF);

KEYWORD_ID(conf_bp_rlpsrv, conf_bp_colon, conf_bp_hostname,
	   OBJ(int,1), BOOTP_RLP,
	   "rlp-server", "Set Resource Location Protocol servers", PRIV_CONF);

KEYWORD_ID(conf_bp_impresssrv, conf_bp_colon, conf_bp_rlpsrv,
	   OBJ(int,1), BOOTP_IMPRESS,
	   "impress-server", "Set Impress print servers", PRIV_CONF);

KEYWORD_ID(conf_bp_lprsrv, conf_bp_colon, conf_bp_impresssrv,
	   OBJ(int,1), BOOTP_LPR,
	   "lpr-server", "Set Berkeley lpr servers", PRIV_CONF);

KEYWORD_ID(conf_bp_quotesrv, conf_bp_colon, conf_bp_lprsrv,
	   OBJ(int,1), BOOTP_QUOTE,
	   "quote-server", "Set quote server addresses", PRIV_CONF);

KEYWORD_ID(conf_bp_logsrv, conf_bp_colon, conf_bp_quotesrv,
	   OBJ(int,1), BOOTP_LOG,
	   "log-server", "Set MIT-LCS UDP log servers", PRIV_CONF);

KEYWORD_ID(conf_bp_dns, conf_bp_colon, conf_bp_logsrv,
	   OBJ(int,1), BOOTP_DNS,
	   "dns-server", "Set DNS nameservers", PRIV_CONF);

KEYWORD_ID(conf_bp_nbns, conf_bp_colon, conf_bp_dns,
	   OBJ(int,1), BOOTP_NBNS,
	   "nbns-server", "Set NBNS nameservers", PRIV_CONF);

KEYWORD_ID(conf_bp_timesrv, conf_bp_colon, conf_bp_nbns,
	   OBJ(int,1), BOOTP_TIME,
	   "time-server", "Set time server addresses", PRIV_CONF);

KEYWORD_ID(conf_bp_gateway, conf_bp_colon, conf_bp_timesrv,
	   OBJ(int,1), BOOTP_GATEWAY,
	   "gateway", "Set gateway addresses, primary first ", PRIV_CONF);

KEYWORD_ID(conf_bp_timeoffset, conf_bp_colon, conf_bp_gateway,
	   OBJ(int,1), BOOTP_TIMEOFFSET,
	   "time-offset", "Set time offset in seconds from UTC", PRIV_CONF);

KEYWORD_ID(conf_bp_submask, conf_bp_colon, conf_bp_timeoffset,
	   OBJ(int,1), BOOTP_SUBNETMASK,
	   "subnet-mask", "Set the subnet mask", PRIV_CONF);

KEYWORD_ID(conf_bp_bootfile, conf_bp_colon, conf_bp_submask,
	   OBJ(int,1), BOOTP_FILE,
	   "bootfile", "Set a boot file name", PRIV_CONF);

/* special case:  'no async-bootp' */
SET	(conf_bp_noprefix1, conf_bp_eol, OBJ(int,1), BOOTP_DELETE);
TESTVAR	(conf_bp_noprefix, conf_bp_noprefix1, NONE,
	 NONE, NONE, conf_bp_bootfile,
	 sense, FALSE);
NVGENNS	(conf_bp_nvgen, conf_bp_noprefix, asyncbootp_command);

/******************************************************************/
KEYWORD	(conf_bootp, conf_bp_nvgen, ALTERNATE,
	 "async-bootp", "Modify system bootp parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	conf_bootp
