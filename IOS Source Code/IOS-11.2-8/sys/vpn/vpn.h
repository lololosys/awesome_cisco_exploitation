/* $Id: vpn.h,v 1.2.40.2 1996/08/07 22:52:00 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/vpn.h,v $
 *------------------------------------------------------------------
 * vpn.h   --   declarations for VPN
 *
 * January 1996, Shoou Yiu
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vpn.h,v $
 * Revision 1.2.40.2  1996/08/07  22:52:00  tkolar
 * CSCdi62172:  VPDN dual CHAP challenge from Home-Gateway
 * Branch: California_branch
 * Merge two disparate VPDN authentication schemes.
 *
 * Revision 1.2.40.1  1996/04/27  06:34:24  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.6  1996/03/25  05:32:15  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.2.5  1996/03/24  01:44:27  tkolar
 * Branch: Dial1_branch
 * Enable AAA forwarding of VPN sessions, and complete
 * authentication of MIDs.  Also update a few parts of the
 * protocol to match changes in the RFC.
 *
 * Revision 1.2.2.4  1996/03/21  23:51:47  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.2.2.3  1996/02/17  22:59:03  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 1.2.2.2  1996/02/09  19:11:26  tkolar
 * Branch: Dial1_branch
 * Fix compile error.  Whups...
 *
 * Revision 1.2.2.1  1996/02/08  06:51:13  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.2  1996/01/31  18:38:40  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:12  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern boolean vpn_force_local_chap;
extern ipaddrtype vpn_source_ip_address;

/*
 * VPN handle status definitions
 */
#define VPN_PENDING 0
#define VPN_FORWARD 1
#define VPN_DONT_FORWARD 2
#define VPN_TIMEOUT 3
#define VPN_CANCEL 4
#define VPN_ABORT 5
#define VPN_ERROR 6
#define VPN_NOHANDLE 7
#define VPN_NOT_ACTIVE 8
#define VPN_HANGUP 9

#define VPN_MAX_QUEUE_ENTRIES 20

#define VPN_FWD_STATION 1
#define VPN_FWDED_STATION  2

/*
 * VPN errors
 */
#define VPN_SUCCESS         0x00000000
#define VPN_AUTHENFAIL      0x00000001
#define VPN_NORESOURCE      0x00000002
#define VPN_ADMIN	    0x00000004
#define VPN_USERQUOTA       0x00000008
#define VPN_PROTERR         0x00000010
#define VPN_NO_USER         0x00000020
#define VPN_BADPWD          0x00000040
#define VPN_BADPPPCFG       0x00000080

#define VPDN_MULTICHASSIS	0
#define VPDN_TUNNEL		1


struct vpntype_ {
    queuetype ipaddressQ;
    hwidbtype *link_hwidb;
    sys_timestamp timer;
    int authen_type;
    paktype *pak;
    int status;
    int error_code;
    char *bundle;
    l2f_tunneltype *tunnel;
};

/*
 * vpn/vpn.c
 */
hwidbtype *vpn_get_mid_interface(int *, char *, uchar, uchar, uchar, uchar *, uchar, uchar *,
ulong, short, uchar *, short, uchar *, l2f_midtype *mid);


/*
 * vpn/vpn_auth.c
 */
boolean vpn_chapstyle_auth(char *, uchar *, int, uchar *, uchar);
boolean vpn_papstyle_auth(char *, uchar *);
boolean vpn_get_md5_digest(char *, uchar, uchar *, uint, uchar *);
ulong vpn_make_tunnel_key(char *);
