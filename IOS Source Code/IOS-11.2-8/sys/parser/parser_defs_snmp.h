/*
 *	P A R S E R _ D E F S _ S N M P . H
 * $Id: parser_defs_snmp.h,v 3.6.40.6 1996/07/03 17:36:44 jjohnson Exp $
 * $Source: /release/112/cvs/Xsys/parser/parser_defs_snmp.h,v $
 * 
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: parser_defs_snmp.h,v $
 * Revision 3.6.40.6  1996/07/03  17:36:44  jjohnson
 * CSCdi61958:  Remove support for undocumented snmp configuration commands
 * remove the last vestiges of support for wildcard snmp communities
 * Branch: California_branch
 *
 * Revision 3.6.40.5  1996/05/21  06:37:46  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.6.40.4  1996/05/17  11:39:45  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.6.26.3  1996/05/05  23:41:58  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.6.26.2  1996/04/03  21:13:04  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.6.26.1  1996/03/22  19:06:22  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.6.40.3  1996/05/09  14:40:22  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.6.40.2.6.1  1996/04/27  07:17:31  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.6.40.2  1996/04/15  03:05:06  jjohnson
 * CSCdi53343:  Remove support for the SNMPv2 Simplified Security
 * Conventions
 * Branch: California_branch
 *
 * Revision 3.6.40.1.4.1  1996/04/08  02:00:14  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.6.40.1  1996/03/28  16:46:29  jjohnson
 * CSCdi50399:  SNMP can drive CPU utilization to over 90%
 * Make SNMP processes default to low priority, and add a hidden config
 * command to allow customers to change it
 * Branch: California_branch
 *
 * Revision 3.6.44.1  1996/03/22  09:41:27  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.6.30.1  1996/03/05  05:50:50  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.6.24.1  1996/02/27  21:11:38  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.6.4.1  1996/01/24  01:37:40  cyoung
 * VTP MIB support and dependant auto-config base. Part II
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.6  1995/12/16  00:15:16  sdurham
 * CSCdi44645:  MAC illegal address trap needs snmp-server host and enable
 * cli support.
 *    moved all parser commands related to mib to appropriate mib
 *    subsystem.
 *
 * Revision 3.5  1995/11/21  00:24:34  thille
 * CSCdi44228:  Remove stale definitions used by old snmp stack
 *
 * Revision 3.4  1995/11/18  00:59:16  thille
 * CSCdi38736:  BGP snmp trap responses getting lost
 * Fix and modularize the bgp trap support.
 *
 * Revision 3.3  1995/11/18  00:19:14  jjohnson
 * CSCdi43548:  need mechanism to enable multiple traps for a subsystem
 *
 * Revision 3.2  1995/11/17  18:52:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:56:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:20:49  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/09  19:07:29  rbatz
 * CSCdi38250:  CIP CSNA SNMP traps do not work
 *
 * Revision 2.3  1995/07/03  22:25:47  jjohnson
 * CSCdi35401:  Parser help for snmp-server packetsize is incorrect
 * Differentiate between the minimum, maximum, and default packetsize
 *
 * Revision 2.2  1995/06/13  04:45:14  thille
 * CSCdi35746:  More control needed over tftp from SNMP
 * Add access-list checking to SNMP sets which cause tftp reads or writes
 * of config files.
 *
 * Revision 2.1  1995/06/07  22:31:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

enum {
    SNMP_NOLINK,
    SNMP_TRAP_INTERFACE,
    SNMP_COMMUNITY,
    SNMP_PACKET,
    SNMP_HOST,
    SNMP_ENABLE,
    SNMP_AUTH,
    SNMP_TIMEOUT,
    SNMP_TFTP_LIST,
    SNMP_QLEN,
    SNMP_LOCATION,
    SNMP_CONTACT,
    SNMP_SHUTDOWN,
    SNMP_DISABLE,
    SNMP_HUB,
    SNMP_CHASSIS,
    SNMP_VIEW,
    SNMP_CONTEXT,
    SNMP_PARTY,
    SNMP_ACCESS_POLICY,
    SNMP_NOSPARSE,
    SNMP_PRIORITY,
/*
 * WARNING - the below enum is ONLY for the mac security trap, and
 * should not be used anywhere else without prior approval from the
 * mib police!!
 */
    SNMP_TRAP_FREQUENCY
};

/*
 * enumerate the various trap subsystems.  These enumerations are used
 * by the "snmp-server host" and "snmp-server enable" commands
 */
enum {
    TRAP_SNMP,
    TRAP_TTY,
    TRAP_X25,
    TRAP_FRAMERELAY,
    TRAP_ISDN,
    TRAP_SDLC,
    TRAP_REPEATER,
    TRAP_ENVMON,
    TRAP_RSRB,
    TRAP_SDLLC,
    TRAP_STUN,
    TRAP_LLC2,
    TRAP_CONFIG,
    TRAP_BGP,
    TRAP_DSPU,
    TRAP_RTT,
    TRAP_VTP,
    TRAP_SYSLOG,
    TRAP_CHAN_LINK_FAIL,  
    TRAP_LAST		/* keep this last in the list */
};
#define TRAPS_ALL_SUBSYS_ENABLED      ((1 << TRAP_LAST) - 1) 
#define TRAPS_ALL_TYPES_ENABLED               (0xffffffff)

enum {
    SNMP_ENABLE_TRAPS,
    SNMP_ENABLE_INFORMS
};

enum {
    TRAP_SNMP_AUTHENTICATION
};

/*
 * WARNING - the below enum is ONLY for the mac security trap, and
 * should not be used anywhere else without prior approval from the
 * mib police!!
 */
 
enum {
    SNMP_TRAP_FREQ_DECAY    
};

enum {
    SNMP_PARTY_LOCAL = 1,
    SNMP_PARTY_REMOTE,
};

enum {
    SNMP_PARTY_PROTO_UDP = 1,
    SNMP_PARTY_PROTO_IPX,
    SNMP_PARTY_PROTO_ATALK,
};

enum {
    SNMP_PARTY_AUTH_MD5 = 1,
    SNMP_PARTY_AUTH_SNMPV1,
};

#define SNMP_AUTH_SNMPV1	1
#define SNMP_AUTH_SNMPV2	2

enum {
    SNMP_PARTY_PRIV_DES = 1,
};

/*
 * show snmp choices
 */
enum {
    SNMP_SHOW_GLOBALS = 1,
    SNMP_SHOW_MIB,
    SNMP_SHOW_ACL,
    SNMP_SHOW_CONTEXT,
    SNMP_SHOW_PARTY,
    SNMP_SHOW_VIEW,
};

