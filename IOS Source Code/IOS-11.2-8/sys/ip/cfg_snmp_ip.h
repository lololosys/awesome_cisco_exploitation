/* $Id: cfg_snmp_ip.h,v 3.2.60.1 1996/07/03 17:35:55 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/ip/cfg_snmp_ip.h,v $
 *------------------------------------------------------------------
 * SNMP IP configuration commands
 *
 * November 1993, Robert Widmer
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: cfg_snmp_ip.h,v $
 * Revision 3.2.60.1  1996/07/03  17:35:55  jjohnson
 * CSCdi61958:  Remove support for undocumented snmp configuration commands
 * remove the last vestiges of support for wildcard snmp communities
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:32:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/07/01  12:55:39  tli
 * CSCdi36508:  Rationalize parser file placement, part I
 * Rationalize ip, ipmulticast, tcp
 *
 * Revision 2.2  1995/06/13  04:45:12  thille
 * CSCdi35746:  More control needed over tftp from SNMP
 * Add access-list checking to SNMP sets which cause tftp reads or writes
 * of config files.
 *
 * Revision 2.1  1995/06/07  22:13:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************
 * snmp-server tftp-server-list <list>
 * no snmp-server tftp-server-list [<list>]
 */
PARAMS_KEYONLY (conf_snmp_tftp_list, ALTERNATE,
		"tftp-server-list", OBJ(int,1), MINFASTACC, MAXFASTACC,
		snmp_ip_command, SNMP_TFTP_LIST,
		"Limit TFTP servers used via SNMP", "IP standard access list",
		PRIV_CONF);

#undef  ALTERNATE
#define ALTERNATE   conf_snmp_tftp_list

