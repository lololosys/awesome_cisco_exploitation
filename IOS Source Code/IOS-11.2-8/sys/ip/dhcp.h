/* $Id: dhcp.h,v 3.3.10.5 1996/08/07 09:00:11 snyder Exp $
 * $Source: /release/112/cvs/Xsys/ip/dhcp.h,v $
 *------------------------------------------------------------------
 * dhcp.h - Process dhcp proxy client  Header Include file
 *
 * Mar 10, 1994, Syed Irfan Ashraf
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: dhcp.h,v $
 * Revision 3.3.10.5  1996/08/07  09:00:11  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.3.10.4  1996/07/19  22:55:11  irfan
 * CSCdi63267:  dhcp hickups
 * Branch: California_branch
 * misc. dhcp proxy client improvements:
 *  - appear to the server as a relay agent to get unicasts back correctly.
 *  - tap packets from bootp server to see if they are responses directed
 *    to the dhcp proxy client (relay-agent).
 *  - make it work for lines that don't have a logged-in user.
 *  - reduce the sleep interval in allocation attempts.
 *  - use client ID strings beginning with a byte of 0.
 *  - rename dhcp_sock.
 *  - make allocation work if prev. request is in selecting/requesting
 *    states.
 *  - make allocation work if prev. lease entry is being purged.
 *  - should use new process primitives. also block on crashes.
 *  - add more debug information.
 *
 * Revision 3.3.10.3  1996/06/22  02:07:11  irfan
 * CSCdi55476:  DHCP does not work well with larrrge lease values
 * Branch: California_branch
 * set an upper limit for max lease time.
 *
 * Revision 3.3.10.2  1996/06/21  23:48:42  irfan
 * CSCdi60979:  dhcp proxy-client should release leases before reload
 *              release all assigned leases when gracefully reloading.
 * CSCdi60953:  dhcp should be more forgiving when validating packets.
 * CSCdi52819:  dhcp proxy client should support unicast messages.
 *
 * Branch: California_branch
 *
 * Revision 3.3.10.1  1996/06/17  23:32:00  hampton
 * Implement the long sought parser "default" keyword.  [CSCdi60510]
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/22  14:34:21  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.2  1995/11/17  09:32:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:54:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/25  16:25:24  vandys
 * CSCdi42511:  DHCP fumbles if IPCP does multiple rounds of CONFREQ
 * Debounce free/alloc to avoid spinning the DHCP
 * state machine on quick-turnaround transactions.
 *
 * Revision 2.2  1995/09/18  19:35:27  fox
 * CSCdi40523:  Add IP Address Pooling support for ISDN and other sync
 * interfaces
 * Modify IP address pooling to support all Point-to-Point interfaces
 * Remove Odd/Even Address feature
 * Add 'ppp negotiate' command
 *
 * Revision 2.1  1995/06/07  20:56:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef __DHCP_H__
#define __DHCP_H__

/*
 * DHCP client process status uses these flags
 *
 */
#define DHCPC_STATUS_RUN     0x1    /* turned off, if we should to die */
#define DHCPC_STATUS_INVOKE  0x2    /* on if we need to wake up */


/*
 * Command codes used by "test DHCP" routines
 */
#define DHCP_TEST_ALLOCATE  1     /* test allocations */
#define DHCP_TEST_RELEASE   2     /* test releasing a lease */
#define DHCP_TEST_RENEW     3     /* test renewing an existing lease */


/*
 * constants, flags, structure and type for a DCHP server entry
 * this is currently a static table. This might be changed to a
 * list.
 */

#define DHCPS_DEFAULT   0x1     /* This is the limited broadcast addr
				   default */
#define DHCPS_CONFIG    0x2     /* We were told of it by administrator
				   as opposed to have discovered */
#define DHCPS_VALID     0x4     /* This entry is not valid */


#define MAX_DHCP_SERVERS   10   /* Not more than these many servers */

#define DHCPS_DEFAULT_VALID (DHCPS_DEFAULT | DHCPS_VALID)

#define DHCP_default_server 0xffffffff  /* broadcast address */

typedef struct dhcp_server_stats_ {
    uint leases;
    uint received, xmittted;
    uint offers, requests;
    uint acks, naks;
    uint declines, releases;
    uint bad_types, out_of_seq;
} dhcp_server_stats;

typedef struct dhcp_server_struct_ {
    struct dhcp_server_struct_ *next;
    ipaddrtype addr;
    uint flags;
    dhcp_server_stats stats;
} dhcp_server_type;


#define DHCP_SCAN_OK       1
#define DHCP_SCAN_BAD     -1
#define DHCP_SCAN_UNKNOWN  0

/*
 * Some timer constants
 *      (in seconds)
 */
#define DHCP_OFFER_WAIT_SECS 3	/* suspect lost/bad discover msg */
#define DHCP_ACK_WAIT_SECS   2	/* suspect lost/bad request msg */
#define DHCP_PURGE_WAIT_SECS 30	/* delete entry after this long */
#define DHCP_RELEASE_WAIT_SECS 1 /* delete entry after this long */
#define DHCP_HOLD_WAIT_SECS 5	/* hold entry for realloc this long */

/*
 * the following 2 values are suggested in the Nov Draft
 * to be at least 4.
 */
#define DHCP_MAX_REQUEST_RETRY     4     /* send these many requests */
                                         /*     before assuming failure */
#define DHCP_MAX_DISCOVER_RETRY    3     /* send these many discovers */
                                         /*     before assuming failure */

/*
 * the protocol asks for sending one DHCP_release message.
 * but sending more than one releases is critical to us.
 * a proper trasnmission of a release is not required
 * by the protocol, as the lease will timeout eventually
 * in a stable network. But since we are depending on the
 * Release event at the server to make this address
 * available again, we should attempt best. Specially
 * given that we always drop the first attempted transmission
 * until bcole fixes it.
 */

#define DHCP_MAX_RELEASE_RETRY   3       /* send these many releases */
#define DHCPC_MIN_LEASE_LENGTH  (3 * 60) /* 3 minutes */
#define DHCPC_DEF_RENEWAL_CONST 1 / 2    /* dafault renewal time */ 
#define DHCPC_DEF_REBIND_CONST  7 / 8    /* default rebind time */

#define DHCP_MAX_STR  50    /* We only process this max sized strings */

typedef enum DHCP_LEASE_ADDR_STATES {
    DHC_INITIAL,
    DHC_SELECTING,
    DHC_REQUESTING,
    DHC_BOUND,
    DHC_REBINDING,
    DHC_RENEWING,
    DHC_RELEASING,
    DHC_HOLDDOWN,
    DHC_PURGING
} dhc_state_type;

/*
 * the entries in the lease list
 * includes all requests as wells as granted leases
 */
typedef struct ip_lease_type_ {
    struct ip_lease_type_ *next;
    dhc_state_type state;              /* DHCP_LEASE_ADDR_STATES */
    sys_timestamp state_timer;         /* current running timer */
    ipaddrtype client_addr;            /* the address we got */
    ipaddrtype request_addr;           /* the address we requested */
    idbtype *idb;                      /* interface pointer */
    ulong my_xid;
    dhcp_server_type *lease_server;    /* the selected server --DELETE-- */
    ipaddrtype lease_server_addr;      /* the selected server's IP addr */
    ulong lease_secs, t1_secs, t2_secs;/* lease time parameters */
    sys_timestamp offer_timestamp;     /* point at which lease was offered */
    uchar client_id[DHCP_MAX_STR+1];     /* client-id string used */
    uint retry_count;                  /* discover/request/retry count */
} iplease_type;

#define MAX_IPLEASE_ADDR 200           /* Maximum size of lease Q */
#define MAX_IPLEASE_SECS 0x000FFFFF    /* Avoids roll over when multiplying */
#define DHCP_FIXED_SIZE 240            /* DHCP header w/o options */
#define MAX_DHCP_PAKS 10               /* Process this many paks at a time */
#define DHCP_SLEEP_INTERVAL (1 * ONESEC) /* sleep for this long b/w invoke */
#define DHCP_SLEEP_SMALL  (ONESEC/8)   /* Misc. DHCP short sleeps */
#define	DHCPS_PORT	67             /* DHCP server port */
#define	DHCPC_PORT	68             /* DHCP client port */

typedef struct dhcptype_ {
	uchar	dhc_op;		/* packet opcode type */
#define	DHCP_BOOTREQUEST    1   /* I don't want to include bootp.h */
#define	DHCP_BOOTREPLY      2
	uchar	dhc_htype;	/* hardware addr type */
	uchar	dhc_hlen;	/* hardware addr length */
	uchar	dhc_hops;	/* gateway hops */
	ulong	dhc_xid;	/* transaction ID */
	ushort	dhc_secs;	/* seconds since boot began */	
	ushort	dhc_flags;	/* New flags field, per rfc1542 */
	ipaddrtype dhc_ciaddr;	/* client IP address */
	ipaddrtype dhc_yiaddr;	/* 'your' IP address */
	ipaddrtype dhc_siaddr;	/* server IP address */
	ipaddrtype dhc_giaddr;	/* gateway IP address */
	uchar	dhc_chaddr[16];	/* client hardware address */
	uchar	dhc_sname[64];	/* server host name */
	uchar	dhc_file[128];	/* boot file name */
	ulong	dhc_vend_cookie;/* cookie indicating vendor specific data */
	uchar	dhc_options[0];  /* vendor-specific area */
} dhcptype;

/*
 * Flags field bits
 */
#define DHCP_FLAG_BROADCAST 0x8000

/*
 * DHCP options as per rfc1533
 */

#define DHCO_LEN_OCTET           1     /* 3 byte options. tag, len, val */
#define DHCO_LEN_SHORT           2     /* 4 byte options */
#define DHCO_LEN_LONG            4     /* 6 byte options */


#define DHCP_MAGIC_COOKIE 0x63825363   /* magic cookie for DHCP */

#define DHCO_PAD               0
#define DHCO_END             255
#define DHCO_IPADDR           50
#define DHCO_LEASE            51
#define DHCO_OVERLOAD         52
#define DHCO_MSG_TYPE         53
#define DHCO_SRVR_ID          54
#define DHCO_PARAM_REQ_LIST   55
#define DHCO_MESSAGE          56
#define DHCO_MAX_MSG          57
#define DHCO_RENEWAL_TIME     58
#define DHCO_REBIND_TIME      59
#define DHCO_CLASS_ID         60
#define DHCO_CLIENT_ID        61

/*
 * We use Client ID type 0. Not a real HW address.
 */
#define DHCO_CLIENT_ID_TYPE   0x0

/*
 * some suboptions
 */
#define DHCO_OVERFLOW_FILE      1
#define DHCO_OVERFLOW_SNAME     2
#define DHCO_OVERFLOW_ALLFLAGS  (DHCO_OVERFLOW_FILE | DHCO_OVERFLOW_SNAME)

#define DHCP_MIN_OPTION_FIELD       60
#define DHCP_MIN_MSG_SIZE_OPTION   576
#define DHCP_MAX_MSG_SIZE_OPTION  1024

/*
 *  DHCP Message types
 */

#define DHCPM_DISCOVER         1
#define DHCPM_OFFER            2
#define DHCPM_REQUEST          3
#define DHCPM_DECLINE          4
#define DHCPM_ACK              5
#define DHCPM_NAK              6
#define DHCPM_RELEASE          7

/*
 * return codes for various dhcp routines
 */
#define DHCP_RET_OK            1
#define DHCP_RET_FAIL          0

/*
 * Operation codes for dhcp_send_pack
 */
#define DHCP_SEND_BROADCAST    1
#define DHCP_SEND_UNICAST      2
 
/*
 * we use the following structure to save option values.
 * this is bcaz they can be at any position (and any order)
 * in the DHCP packet.
 * e.g. we look up c_msg_type for DHCP message type instead
 * of scanning through the packet every time.
 */
typedef struct dhcp_values_ {
    int c_msg_type;
    ipaddrtype c_source, c_destination, c_req_ipaddr, c_offer_ipaddr;
    ulong c_lease, c_renewal, c_rebind;
    uchar c_class_id[DHCP_MAX_STR+1];
    uchar c_client_id[DHCP_MAX_STR+1];
    uchar c_reason[DHCP_MAX_STR+1];
    uchar c_file_sname_ov, c_fill_char1;
    ushort c_max_msg;
    ulong c_srvr_id, c_xid;
} dhcp_values;


#define DHCP_GETLONG(l, cp) { \
	(l) = *(cp)++ << 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; (l) <<= 8; \
	(l) |= *(cp)++; \
}

#define DHCP_GETSHORT(s, cp) { \
	(s) = *(cp)++ << 8; \
	(s) |= *(cp)++; \
}
#define DHCP_PUTSHORT(s, cp) { \
	*(cp)++ = (s) >> 8; \
	*(cp)++ = (s) & 0xff; \
}

#define DHCP_PUTLONG(l, cp) { \
	*(cp)++ = (l) >> 24; \
	*(cp)++ = (l) >> 16; \
	*(cp)++ = (l) >> 8; \
	*(cp)++ = (l); \
}

/*
 *  some data values used by dhcp
 */

extern const char *const DHCPM_strings[];
extern const char *const DHCPC_strings[];
extern dhcp_server_type dhcps_table[MAX_DHCP_SERVERS];
extern ipsocktype *dhcp_client_soc;
extern uint total_dhcpservers;
extern queuetype ip_leaseQ;



/*
 * some dhcp functions
 */
extern void init_dhcps_table(void);

/*
 * dhcp_io.c
 */
extern uint dhcp_send_discover(iplease_type *);
extern uint dhcp_send_request(iplease_type *);
extern uint dhcp_send_decline(dhcp_values *, iplease_type *);
extern uint dhcp_send_release(iplease_type *);

/*
 * dhcp_parse.c
 */
extern void dhcpsrv_command(parseinfo *);
extern void show_dhcp_servers(void);
extern void show_dhcp(parseinfo *);
extern void print_lease_entry(boolean console, iplease_type *);
extern void show_dhcpc_leases(parseinfo *);
extern void dump_dhcp_pkt(udptype *udp, dhcptype *dhcp, dhcp_values *dhc);
extern void dhcp_parser_init(void);

extern dhcp_server_type *find_dhcps_entry(ipaddrtype);
extern dhcp_server_type *add_dhcps_entry(ipaddrtype);

extern void test_dhcp_command(parseinfo *);


/*
 * that's all to it
 */

#endif __DHCP_H__
