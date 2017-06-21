/* $Id: cfg_int_ip_tcpcompress.h,v 3.3.56.1 1996/08/12 16:08:03 widmer Exp $
 * $Source: /release/112/cvs/Xsys/tcp/cfg_int_ip_tcpcompress.h,v $
 *------------------------------------------------------------------
 * TCP/IP header compression interface commands
 *
 * May 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_int_ip_tcpcompress.h,v $
 * Revision 3.3.56.1  1996/08/12  16:08:03  widmer
 * CSCdi65725:  Create Eolns macro that defaults csb->which to 0
 * Branch: California_branch
 * Add Eolns and Nvgenns macro that default csb->which to 0
 * Remove hidden keyword help strings
 *
 * Revision 3.3  1995/11/20  23:22:08  ahh
 * CSCdi41919:  PT: cannot specify source interface for TCP connections
 * Add source-interface option to outgoing TCP translation and telnet.
 * Also convert some parser defines to enums.
 *
 * Revision 3.2  1995/11/17  18:47:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:35:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:56:30  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.1  1995/06/07  22:05:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/************************************************************************
 * ip tcp compression-connections <number>
 * no ip tcp compression-connections [<number>]
 *
 * OBJ(int,1) = number for 'compression-connections'
 */

/* compression-connections <number> */
PARAMS_KEYONLY(ci_ip_compconn, no_alt, "compression-connections",
	       OBJ(int,1), MIN_STATES, MAX_STATES,
	       ip_compression_connections_command, 0,
	       "Maximum number of compressed connections",
	       "Number of connections", PRIV_CONF);

/***********************************************************************
 * ip tcp header-compression [passive]
 * no ip tcp header-compression [passive]
 *
 * OBJ(int,1) = TCP_COMPRESS_PASSIVE if 'passive' for 'header-compression'
 *		TCP_COMPRESS_ON
 *		TCP_COMPRESS_OFF
 *
 * Possible commands are:
 *
 *     'ip tcp header-compression'            - turns header compression on
 *     'ip tcp header-compression passive'    - turns passive header comp on 
 *     'no ip tcp header-compression'         - turns header compression off
 *
 * Note that obsolete keywords "on" and "off" are still supported, but
 * are hidden.
 *
 */
/* header-compression [passive] */
EOLNS	(ci_ip_hdrcomp_eol, ip_header_compression_command);
SET	(ci_ip_hdrcomp_set, ci_ip_hdrcomp_eol, OBJ(int,1), TCP_COMPRESS_ON);
KEYWORD_ID (ci_ip_hdrcomp_off, ci_ip_hdrcomp_eol, ci_ip_hdrcomp_set,
	    OBJ(int,1), TCP_COMPRESS_OFF,
	    "off", NULL, PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_ip_hdrcomp_on, ci_ip_hdrcomp_eol, ci_ip_hdrcomp_off,
	    OBJ(int,1), TCP_COMPRESS_ON,
	    "on", NULL,
	    PRIV_CONF|PRIV_HIDDEN);
KEYWORD_ID (ci_ip_hdrcomp_passive, ci_ip_hdrcomp_eol, ci_ip_hdrcomp_on,
	    OBJ(int,1), TCP_COMPRESS_PASSIVE,
	    "passive",
	    "Compress only for destinations which send compressed headers", 
	    PRIV_CONF);
NOPREFIX (ci_ip_hdrcomp_noprefix, ci_ip_hdrcomp_passive, ci_ip_hdrcomp_eol);
NVGENNS	(ci_ip_hdrcomp_nvgen, ci_ip_hdrcomp_noprefix,
	 ip_header_compression_command);
KEYWORD	(ci_ip_hdrcomp, ci_ip_hdrcomp_nvgen, ci_ip_compconn,
	 "header-compression", "Enable TCP header compression", PRIV_CONF);

/************************************************************************/
/* tcp
 * This keyword introduces both "ip tcp compression-connections"
 * and "ip tcp header-compression".
 */
KEYWORD	(ci_ip_tcp, ci_ip_hdrcomp, ALTERNATE,
	 "tcp", "TCP header compression parameters", PRIV_CONF);

#undef	ALTERNATE
#define	ALTERNATE	ci_ip_tcp
