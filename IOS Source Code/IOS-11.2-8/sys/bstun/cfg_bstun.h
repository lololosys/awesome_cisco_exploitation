/* $Id: cfg_bstun.h,v 3.3 1995/11/21 08:52:25 jbalestr Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/cfg_bstun.h,v $
 *------------------------------------------------------------------
 * cfg_bstun.h - BSTUN Configuration commands
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995,1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: cfg_bstun.h,v $
 * Revision 3.3  1995/11/21  08:52:25  jbalestr
 * CSCdi42960:  bstun keepalive count
 * Added bstun keepalive interval and count
 *
 * Revision 3.2  1995/11/17  08:49:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:04:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/04  09:35:29  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize arap, atm, bstun
 *
 * Revision 2.2  1995/06/16  06:23:09  arothwel
 * CSCdi35957:  BSTUN/BSC cleanup.
 *
 * Revision 2.1  1995/06/07  22:02:01  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * bstun keepalive-count <count>
 * no bstun keepalive-count
 */
EOLS	(conf_bstun_keepcnt_eol, bstun_command, BSTUN_KEEPALIVE_COUNT);
NUMBER  (conf_bstun_keepcnt_value, conf_bstun_keepcnt_eol, no_alt,
	 OBJ(int,1), 2, 10, "Number of retries must be between 2 and 10");
NVGENS	(conf_bstun_keepcnt_nvgen, conf_bstun_keepcnt_value,
	 bstun_command, BSTUN_KEEPALIVE_COUNT);
KEYWORD	(conf_bstun_keepcnt, conf_bstun_keepcnt_nvgen, no_alt,
	 "keepalive-count", "Specify BSTUN peer keepalive retry count", PRIV_CONF);

/************************************************************************
 * bstun remote-peer-keepalive <seconds>
 * no bstun remote-peer-keepalive
 */
EOLS	(conf_bstun_keepalive_eol, bstun_command, BSTUN_KEEPALIVE);
NUMBER	(conf_bstun_keepalive_seconds, conf_bstun_keepalive_eol, conf_bstun_keepalive_eol,
	 OBJ(int,1), 1, 300, "Keepalive interval (default 30 seconds)");
NVGENS	(conf_bstun_keepalive_nvgen, conf_bstun_keepalive_seconds, bstun_command,
              BSTUN_KEEPALIVE);
KEYWORD_ID (conf_bstun_keepalive, conf_bstun_keepalive_nvgen, conf_bstun_keepcnt,
            OBJ(int,1), 30, "remote-peer-keepalive",
            "Enable BSTUN peer keepalive", PRIV_CONF);

/************************************************************************
 * bstun protocol-group <group-number> <protocol>
 * no bstun protocol-group
 *
 * OBJ(int,1) = protocol group number
 * OBJ(string,1) = protocol name
 */
EOLS	(conf_bstun_protoeol, bstun_command, BSTUN_PROTOCOL);
STRING	(conf_bstun_protocol, conf_bstun_protoeol, no_alt, OBJ(string,1),
	 "Protocol name (max 20 characters)");
NUMBER	(conf_bstun_protogrpnum, conf_bstun_protocol, no_alt,
	 OBJ(int,1), 1, 255, "A BSTUN protocol group number");
NVGENS	(conf_bstun_protogrpnvgen, conf_bstun_protogrpnum, bstun_command, 
	 BSTUN_PROTOCOL);
KEYWORD (conf_bstun_protogrp, conf_bstun_protogrpnvgen, conf_bstun_keepalive,
	 "protocol-group", "Assign a protocol to a protocol group", PRIV_CONF);



/************************************************************************
 * bstun peer-name <ipaddr>
 * no bstun peer-name <ipaddr>
 */
EOLS	(conf_bstun_peereol, bstun_command, BSTUN_PEER);
IPADDR	(conf_bstun_peeraddr, conf_bstun_peereol, no_alt,
	 OBJ(paddr,1), "IP address which identifies this BSTUN peer");
NOPREFIX(conf_bstun_peer_noprefix, conf_bstun_peeraddr, conf_bstun_peereol);
NVGENS	(conf_bstun_peernvgen, conf_bstun_peer_noprefix,
	 bstun_command, BSTUN_PEER);
KEYWORD	(conf_bstun_peer, conf_bstun_peernvgen, conf_bstun_protogrp,
	 "peer-name", "Enable BSTUN", PRIV_CONF);

/************************************************************************
 * bstun
 * no bstun
 */
KEYWORD	(configure_bstun, conf_bstun_peer, NONE, "bstun",
	 "BSTUN global configuration commands", PRIV_CONF);

TEST_BOOLEAN(configure_bstun_test, configure_bstun, NONE,
	     ALTERNATE, bridge_enable);

#undef  ALTERNATE
#define ALTERNATE configure_bstun_test
