/* $Id: radius.h,v 3.7.10.6 1996/08/02 23:44:23 billw Exp $
 * $Source: /release/112/cvs/Xsys/servers/radius.h,v $
 *------------------------------------------------------------------
 * radius.h - definitions for supporting RADIUS protocol
 *
 * July 1995, Bill Westfield
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: radius.h,v $
 * Revision 3.7.10.6  1996/08/02  23:44:23  billw
 * CSCdi60940:  RADIUS Accounting ID fields not globally unique
 * Branch: California_branch
 *
 * Revision 3.7.10.5  1996/06/24  23:29:40  billw
 * CSCdi54704:  RADIUS should be able to talk to different UDP ports
 * Branch: California_branch
 *
 * Revision 3.7.10.4  1996/05/24  01:23:19  billw
 * CSCdi56667:  RADIUS Accounting fixes
 * Branch: California_branch
 * oops.  commiting .h file changes too is a good idea.
 *
 * Revision 3.7.10.3  1996/04/24  02:06:22  billw
 * CSCdi51316:  RADIUS: implement better fallover behavior for down servers
 * Branch: California_branch
 *
 * Revision 3.7.10.2  1996/03/27  09:41:05  billw
 * CSCdi50704:  Additional RADIUS support needed
 * Branch: California_branch
 *
 * Revision 3.7.10.1  1996/03/27  07:59:37  billw
 * CSCdi50545:  RADIUS doesnt fall over to multiple servers
 * Branch: California_branch
 *
 * Revision 3.7  1996/02/22  06:08:32  billw
 * CSCdi49412:  RADIUS fails after 255 uses
 * Make saved fields in data structure the same size as the fields
 * in the actual packet.
 *
 * Revision 3.6  1996/02/09  09:03:11  billw
 * CSCdi43680:  RADIUS doesnt support magic number ip address values
 * also add authstruct->auth_idb, and change auth->tty to auth->auth_tty
 *
 * Revision 3.5  1996/01/03  19:57:31  irfan
 * CSCdi46304:  radius packets could use a predicatable source IP address
 * allow radius to use a specific (up) interface's IP address.
 *
 * Revision 3.4  1995/12/16  01:09:14  billw
 * CSCdi43681:  Some RADIUS code review comments not incorporated
 *
 * Revision 3.3  1995/11/20  10:03:27  billw
 * CSCdi42025:  radius retransmission count too big
 *
 * Revision 3.2  1995/11/17  18:51:36  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:11:55  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:24:38  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define RADIUS_UDP_PORT	1645
#define RADIUSACCT_UDP_PORT 1646

#define RADIUS_SUBSYS_MAJVERSION 1
#define RADIUS_SUBSYS_MINVERSION 0
#define RADIUS_SUBSYS_EDITVERSION 1

#define RADIUS_SECRETLENGTH 65

#define RAD_CODE_ACC_REQUEST	 1	/* Access Request */
#define RAD_CODE_ACC_ACCEPT	 2	/* Access Accept */
#define RAD_CODE_ACC_REJECT	 3	/* Access Reject */
#define RAD_CODE_ACCT_REQUEST	 4	/* Accounting Request */
#define RAD_CODE_ACCT_RESPOND	 5	/* Accounting response */
#define RAD_CODE_ACC_CHALLENGE	11	/* Access Challange */
#define RAD_CODE_STAT_SERVER	12	/* Status server (experimental) */
#define RAD_CODE_STAT_CLIENT	13	/* Status client (experimental) */
#define RAD_CODE_STAT_MAX	13 /* current maximum legal code */

typedef struct radiustype_ {
    uchar rad_code;		/* type of query */
    uchar rad_ident;		/* matchs queries and replies */
    ushort rad_length;		/* Total length */
    uchar rad_authent[16];	/* authentication, hidden password */
    uchar rad_attributes[1];	/* beginning of atrribute/value pairs */
} radiustype;

typedef struct radius_attrtype_ {
    uchar type;
    uchar length;
    union {
	uchar PACKED(str[4]);		/* String */
	ipaddrtype PACKED(addr);	/* Address */
	ulong PACKED(num);		/* Integer */
	ulong PACKED(time);		/* time value */
    } PACKED(val);
} radius_attrtype;
	

/*
 * RADIUS attribute identifiers.
 */
#define RAD_ATTR_USER      1	/* user-name */
#define RAD_ATTR_UPASS     2	/* User Password */
#define RAD_ATTR_CPASS     3	/* CHAP-Password */
#define RAD_ATTR_NASIP     4	/* NAS-IP-Address */
#define RAD_ATTR_NASPORT   5	/* NAS-Port */
#define RAD_ATTR_SERVICE   6	/* Service-Type */
#define   RAD_SERV_LOGIN     1	/*   Login */
#define   RAD_SERV_FRAMED    2  /* PPP/etc */
#define   RAD_SERV_CBLOG     3	/* Callback login */
#define   RAD_SERV_CBFRM     4	/* Callback framed */
#define   RAD_SERV_OUTB      5	/* Outbound user */
#define   RAD_SERV_ADMIN     6  /* Administrative user */
#define   RAD_SERV_EXEC	     7	/* Exec (NAS Prompt) user */
#define   RAD_SERV_AUTH      8	/* Authenticate only */
#define	  RAD_SERV_CBEXEC    9  /* Callback NAS Prompt  (new in 96)*/
#define RAD_ATTR_FPROT     7	/* Framed-Protocol */
#define   RAD_FPROT_PPP      1	/*   Framed protocol is PPP */
#define   RAD_FPROT_SLIP     2	/*   Framed protocol is SLIP */
#define	  RAD_FPROT_ARA	     3  /*   ARA (new for 96) */
#define RAD_ATTR_FIP	   8    /* Framed-IP-Address */
#define   RAD_FIP_NASPICK  0xFFFFFFFE
#define   RAD_FIP_USERPICK 0xFFFFFFFF
#define RAD_ATTR_FMASK     9    /* Framed-IP-Netmask */

#define RAD_ATTR_FROUTING 10	/* Framed-Routing */
#define RAD_ATTR_FILTER	  11	/* Filter-Id */
#define RAD_ATTR_FMTU     12	/* Framed-MTU */
#define RAD_ATTR_FCOMP    13	/* Framed-Compression */
#define   RAD_FCOMP_VJTCP   1	/* TCP Header compression */
#define   RAD_FCOMP_IPXHDR  2	/* IPX Header compression */
#define RAD_ATTR_LHOST    14	/* Login-IP-Host */
#define   RAD_LHOST_NASPICK 0	/* THe NAS picks a host to connect to */
#define   RAD_LHOST_USERPICK 0xFFFFFFFF /* User gets to pick address */
#define RAD_ATTR_LSERV    15	/* Login-Service */
#define   RAD_LSERV_TN      0	/*   Login using telnet */
#define   RAD_LSERV_RLOG    1   /*   Login using rlogin */
#define   RAD_LSERV_TCP     2   /*   Login using tcp stream */
#define   RAD_LSERV_LAT     4   /*   login using lat */
#define RAD_ATTR_LPORT    16	/* Login-Port */
#define RAD_ATTR_UNA1     17	/* (unassigned) */
#define RAD_ATTR_RMSG     18	/* Reply-Message */
#define RAD_ATTR_CBNUM    19    /*  Login-Callback-Number */

#define RAD_ATTR_CBNAM    20	/* Framed-Callback-Name */
#define RAD_ATTR_UNA2     21	/* (unassigned) */
#define RAD_ATTR_FROUTE   22	/* Framed-Route */
#define RAD_ATTR_FIPXN    23	/* Framed-IPX-Network */
#define  RAD_FIPX_NASPICK 0xFFFFFFFE
#define RAD_ATTR_STATE    24	/* State */
#define RAD_ATTR_CLASS    25	/* Class */
#define RAD_ATTR_VS1      26	/* Vendor-Specific */
#define RAD_ATTR_ATIMOUT  27	/* Session-Timeout */
#define RAD_ATTR_ITIMOUT  28	/* Idle-Timeout */
#define RAD_ATTR_TERMACT  29	/* Termination-Action */

#define RAD_ATTR_CDNIS    30	/* Client-Port-DNIS */
#define RAD_ATTR_CALLID   31	/* Caller-ID */
#define RAD_ATTR_NASID    32	/* NAS-Identifier */
#define RAD_ATTR_PSTATE	  33	/* client Proxy state */
#define RAD_ATTR_LLATS    34	/* Login-LAT-Service */
#define RAD_ATTR_LLATN    35	/* Login-LAT-Node */
#define RAD_ATTR_LLATG    36	/* Login-LAT-group */
#define RAD_ATTR_FATLINK  37	/* Framed-Appletalk-link */
#define RAD_ATTR_FATNET   38	/* Framed-AppleTalk-Network */
#define RAD_ATTR_FATZONE  39	/* Framed-AppleTalk-Zone */

#define RAD_ACCT_STATTYPE 40    /* Acct-Status-Type */
#define   RADACCT_STAT_START 1	/*   Start record */
#define   RADACCT_STAT_STOP  2  /*   Stop record */
#define   RADACCT_STAT_WATCH 3  /*   Watchdog record */
#define RAD_ACCT_DELAY    41    /* Acct-Delay-Time */
#define RAD_ACCT_INBYTES  42    /* Acct-Input-Octets */
#define RAD_ACCT_OUTBYTES 43    /* Acct-Output-Octets */
#define RAD_ACCT_SESSID   44    /* Acct-Session-Id */
#define RAD_ACCT_AUTHEN   45    /* Acct-Authentic */
#define RADACCT_AUTHEN_NONE 0	/*   No authentication */
#define RADACCT_AUTHEN_RADIUS 1	/*   authenticated using radius */
#define RADACCT_AUTHEN_LOCAL 2	/*   some form of Local authentication */
#define RADACCT_AUTHEN_REMOTE 3	/*   Some form of remote authentication */
#define RAD_ACCT_STIME    46    /* Acct-Session-Time */
#define RAD_ACCT_INPAKS   47    /* Acct-Input-Packets */
#define RAD_ACCT_OUTPAKS  48    /* Acct-Output-Packets */

/*  New in 1996 */
#define RAD_ATTR_CHAPCHAL 60	/* CHAP-Challenge */
#define RAD_ATTR_NASPTYPE 61	/* NAS-Port-Type */
#define   RAD_PTYPE_ILLEGAL -1
#define   RAD_PTYPE_ASYNC 0
#define   RAD_PTYPE_SYNC  1
#define   RAD_PTYPE_SISDN 2	/* Sync ISDN */
#define   RAD_PTYPE_V120  3	/* ASync ISDN V.120 */
#define   RAD_PTYPE_V110  4	/* ASync ISDN V.110 */
#define   RAD_PTYPE_VTY   5	/* Virtual */

#define RAD_ATTR_PLIMIT   62	/* Port-Limit */
#define RAD_ATTR_LLATP	  63	/* Login-LAT-Port */


#define OID_CISCO 9		/* SNMP OID for cisco, used in VS attributes */
#define RAD_CISCO_AVPAIR 1	/* cisco's vendors specific option */

typedef struct radius_context_ {
    struct radius_context_ *next;
    uchar authent[16];		/* authenticator we used */
    uchar ident;		/* Radius session ID */
    uchar type;			/* Type of request */
    ushort udpport;		/* destination udp port */
    ipaddrtype firstserver;	/* First server we tried */
    ipaddrtype server;		/* Which server are we talking to now */
    paktype *pak;		/* packet to or from us */
    watched_boolean *done;	/* scheduler interface */
    mgd_timer rxtimer;		/* retransmittion timer */
    sys_timestamp expiretime;	/* End of retransmissions */
    userstruct *user;		/* AAA interafce */
    authenstruct *authen;	/* AAA interface */
    tt_soc *tty;		/* TTY (if any) that we're using */
    idbtype *idb;		/* interface (if any) in use */
} radius_ctx;

/*
 * queue of radius servers
 */
typedef struct radius_servertype_ {
    struct radius_servertype_ *next;
    ipaddrtype addr;
    ushort authport;
    ushort acctport;
    char secret [RADIUS_SECRETLENGTH];
    sys_timestamp deaduntil;	/* Time to avoid retrying */
} radius_servertype;


/*
 * Saved athorization data
 */


/*
 * Parser defs for radius
 */

enum radius_parse_vals {
    RADIUS_HOST,
    RADIUS_RETRANSMIT,
    RADIUS_TIMEOUTVALUE,
    RADIUS_DEADTVALUE,
    RADIUS_PASSWORDOPT,
    RADIUS_DIRECTED_REQUEST,
    RADIUS_SECRET,
    RADIUS_SOURCE_INTERFACE,
    RADIUS_UNIQUEID,
    RADIUS_EXTENDED_PORTNAMES
};

#define RADIUS_RETRANS_DEFAULT 3
#define RADIUS_TIMEOUT_DEFAULT 5*ONESEC
#define RADIUS_DEADTIME_DEFAULT 0
