/* $Id: l2f.h,v 1.2.40.12 1996/09/12 04:02:11 tkolar Exp $
 * $Source: /release/112/cvs/Xsys/vpn/l2f.h,v $
 *------------------------------------------------------------------
 * l2f.h  -- declarations for the l2f protocol
 *
 * January 1996, Tim Kolar
 *
 * Copyright (c) 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: l2f.h,v $
 * Revision 1.2.40.12  1996/09/12  04:02:11  tkolar
 * CSCdi62277:  L2F tunnel authentication over Radius fails
 * Branch: California_branch
 * Allow cleartext passwords to be passed from Radius
 *
 * Revision 1.2.40.11  1996/09/11  23:35:57  tkolar
 * CSCdi63923:  L2F sequence numbers per tunnel, not MID
 * Branch: California_branch
 * Do L2F sequence numbers as per the RFC.
 *
 * Revision 1.2.40.10  1996/09/04  16:14:38  tkolar
 * CSCdi67999:  L2F_CLOSE_STR is processed incorrectly
 * Branch: California_branch
 * Fix processing of L2F_CLOSE_STR, and add our own temporary
 * outgoing L2F_CLOSE_STR so people can test against us.
 *
 * Revision 1.2.40.9  1996/08/16  22:25:54  tkolar
 * CSCdi66320:  L2F LCP0 message not handled
 * Branch: California_branch
 * Interpret L2F_REQ_LCP0 correctly, and change two
 * other defines to match the L2F draft.
 *
 * Revision 1.2.40.8  1996/08/16  22:05:14  tkolar
 * CSCdi66351:  L2F tunnel authentication should allow arbitrary length
 * challenges
 * Branch: California_branch
 * Allow them, and change the default challenge to 16 bytes.
 *
 * Revision 1.2.40.7  1996/08/07  22:51:57  tkolar
 * CSCdi62172:  VPDN dual CHAP challenge from Home-Gateway
 * Branch: California_branch
 * Merge two disparate VPDN authentication schemes.
 *
 * Revision 1.2.40.6  1996/07/12  17:29:41  tkolar
 * CSCdi62195:  L2F tunnel clearing command needed
 * Branch: California_branch
 * Make is possible to shut down an entire tunnel, rather than having
 * to do it MID by MID.
 *
 * Revision 1.2.40.5  1996/06/18  21:03:19  tkolar
 * CSCdi60718:  L2F_KEY is ignored
 * Branch: California_branch
 * Treat keys as per the RFC.
 *
 * Revision 1.2.40.4  1996/05/12  21:24:19  tkolar
 * CSCdi57473:  L2F implementation does not match RFC
 * Branch: California_branch
 * Assuage those poor fools who believe that just because we
 * wrote the RFC, our implementation should somehow interoperate
 * with it.
 *
 * Revision 1.2.40.3  1996/05/12  19:22:20  tkolar
 * CSCdi57472:  vpn is an overloaded acronym
 * Branch: California_branch
 * Change "vpn" to "vpdn", and make a few other user interface tweaks
 * while we're at it.
 *
 * Revision 1.2.40.2  1996/05/12  18:06:19  tkolar
 * CSCdi57470:  ipwrite should still not be called at interrupt
 * Branch: California_branch
 * Correct CSCdi56324 .
 *
 * Revision 1.2.40.1  1996/04/27  06:34:15  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.2.2.14  1996/04/20  20:59:39  tkolar
 * Branch: Dial1_branch
 * Bring the L2F flags into agreement with the RFC.
 *
 * Revision 1.2.2.13  1996/03/25  05:32:11  tkolar
 * Branch: Dial1_branch
 * Enable Tunnel authentication.
 *
 * Revision 1.2.2.12  1996/03/24  01:44:24  tkolar
 * Branch: Dial1_branch
 * Enable AAA forwarding of VPN sessions, and complete
 * authentication of MIDs.  Also update a few parts of the
 * protocol to match changes in the RFC.
 *
 * Revision 1.2.2.11  1996/03/22  05:34:19  tkolar
 * Branch: Dial1_branch
 * Reshape the way MLPVT interacts with VPN.
 *
 * Revision 1.2.2.10  1996/03/21  23:51:43  tkolar
 * Branch: Dial1_branch
 * Add support for multiple incoming tunnels, fix a crash in the resend
 * code, and add the beginnings of true authentication.
 *
 * Revision 1.2.2.9  1996/03/16  16:43:56  syiu
 * Branch: Dial1_branch
 * Create/Delete L2F tunnels from MLPVT. Also move code into
 * L2F create/delete tunnel routines
 *
 * Revision 1.2.2.8  1996/03/12  04:55:30  tkolar
 * Branch: Dial1_branch
 * Vastly expand L2F comments, add two new debugging variables, and make
 * wraparound sequence numbers fully understood.
 *
 * Revision 1.2.2.7  1996/03/11  22:58:33  tkolar
 * Branch: Dial1_branch
 * Clean up a small collection of nits.
 *
 * Revision 1.2.2.6  1996/03/11  05:57:06  tkolar
 * Branch: Dial1_branch
 * Provide L2F timeouts.
 *
 * Revision 1.2.2.5  1996/03/11  02:07:36  tkolar
 * Branch: Dial1_branch
 * Enable L2F management packet reliable delivery.
 *
 * Revision 1.2.2.4  1996/02/25  05:18:55  tkolar
 * Branch: Dial1_branch
 *
 * Revision 1.2.2.3  1996/02/17  22:59:01  tkolar
 * Branch: Dial1_branch
 * Checkpoint a pingable VPN.
 *
 * Revision 1.2.2.2  1996/02/13  23:07:47  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN.
 *
 * Revision 1.2.2.1  1996/02/08  23:20:02  tkolar
 * Branch: Dial1_branch
 * Checkpoint VPN integration
 *
 * Revision 1.2  1996/01/31  18:38:34  tkolar
 * Branch: Dial1_branch
 * Get all of the VPN code into the repository, with the right names
 * and in the right places.
 *
 * Revision 1.1  1996/01/29  18:09:09  tkolar
 * Populate the VPN directory.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Parser values
 */

#define L2F_ENABLE	0
#define L2F_INCOMING	1
#define L2F_OUTGOING	2
#define L2F_CLEAR_TUNNEL	3
#define L2F_CLEAR_ACCOUNT	4
#define L2F_FORCE_LOCAL_CHAP	5
#define L2F_IGNORE_MID_SEQUENCE	6
#define L2F_SOURCE_IP_ADDRESS 7
/* 
 * L2F protocol values 
 */

#define MAX_VPN_SMALL_PARM 256
#define MAX_VPN_LARGE_PARM 256*256

#define L2F_ANY_TYPE	-1

#define L2F_MANAGEMENT_MID	0

#define L2F_ILLEGAL	0x0  /* Illegal  */
#define L2F_PROTO	0x1  /* L2F management packets */
#define L2F_PPP		0x2  /* PPP tunneled inside L2F */
#define L2F_SLIP	0x3  /* SLIP tunneled inside L2F */

#define  L2F_INVALID	0x00  /* Invalid message			*/
#define  L2F_CONF	0x01  /* Request configuration			*/
#define   L2F_CONF_NAME	0x02  /* Name of peer sending LPTP_CONF		*/
#define   L2F_CONF_CHAL	0x03  /* Random number peer challenges with	*/
#define   L2F_CONF_CLID	0x04  /* CLID that the other side should use    */
#define  L2F_OPEN	0x02  /* Accept configuration			*/
#define   L2F_OPEN_NAME	0x01  /* CHAP name received from client		*/
#define   L2F_OPEN_CHAL	0x02  /* Challenge CHAP client received		*/
#define   L2F_OPEN_RESP	0x03  /* CHAP challenge response from client	*/
#define   L2F_ACK_LCP1	0x04  /* LCP CONFACK accepted from client	*/
#define   L2F_ACK_LCP2	0x05  /* LCP CONFACK sent to client		*/
#define   L2F_OPEN_TYPE	0x06  /* Type of authentication used		*/
#define   L2F_OPEN_ID	0x07  /* CHAP Identification number             */
#define   L2F_REQ_LCP0  0x08  /*  First LCP CONFREQ from client */
#define  L2F_CLOSE	0x03  /* Request disconnect			*/
#define   L2F_CLOSE_WHY	0x01  /* Reason code for close			*/
#define   L2F_CLOSE_STR	0x02  /* ASCII string description		*/
#define  L2F_ECHO	0x04  /* Verify presence of peer		*/
#define  L2F_ECHO_RESP	0x05  /* Respond to LPTP_ECHO			*/

#define L2F_AUTH_NONE	0x00
#define L2F_AUTH_TEXT	0x01
#define L2F_AUTH_CHAP	0x02
#define L2F_AUTH_PAP	0x03

/* version_and_flags field values */
#define VERSIONBIT1	0x1
#define VERSIONBIT2	0x2
#define VERSIONBIT3	0x4
#define L2F_CHECKSUM	0x8
#define UNUSED1		0x10
#define UNUSED2		0x20
#define UNUSED3		0x40
#define UNUSED4		0x80
#define UNUSED5		0x100
#define UNUSED6		0x200
#define UNUSED7		0x400
#define UNUSED8		0x800
#define L2F_SEQUENCED	0x1000
#define L2F_PRIORITY	0x2000
#define L2F_KEY		0x4000
#define L2F_OFFSET	0x8000

/* L2F VERSION (must be between 0 and 7) */

#define L2F_VERSION 1

/*
 * L2F states 
 */

#define L2FTUNNEL_CLOSED	0
#define L2FTUNNEL_OPENING	1
#define L2FTUNNEL_OPEN		2
#define L2FTUNNEL_CLOSING	3

#define L2FMID_CLOSED		0
#define L2FMID_WAITING_FOR_TUNNEL	1
#define L2FMID_OPENING		2
#define L2FMID_OPEN		3
#define L2FMID_CLOSING		4

/*
 *  Largest size for a management packet
 */
#define MAX_L2FMANAGE_SIZE 600

#define L2F_RESEND_TIME	2*ONESEC	/* send a resend after this */

#define L2F_IDLE_TIME 3*ONESEC		/* send an ECHO after this */

#define INITIAL_SEND 0

#define L2FMANAGE_QUEUE	1
#define L2FDATA_QUEUE	2

#define PACKETS_PER_INVOCATION	20

#define MAX_CHALLENGE_LEN 128
#define VPN_CHALLENGE_SIZE 16

#define L2F_FAREWELL	"BYE BYE"

#define L2F_RETRY_RESEND 6

/*
 * Debugging variables 
 */
extern int l2f_event_debug;
extern int l2f_packet_debug;
extern int l2f_error_debug;

/*
 * Is l2f running?  
 */
extern uchar l2f_enable;

/*
 * Structures
 */
struct l2f_midtype_ {
    struct l2f_midtype_ *next;	/* we'll be keeping this on a linked list */
    int mid;			/* number associated with MID */
    int state;			/* current state */
    sys_timestamp state_time;   /* when did we move to this state? */
    char *name;			/* user name for MID */
    int challenge_len;		/* length of authentication challenge */
    uchar *challenge;		/* the challenge itself */
    int response_len;		/* length of authentication response */
    uchar *response; 	   	/* the response itself */
    int rcv_ack_len;		/* length of LCP_ACK received by local PPP */
    uchar *rcv_ack;		/* last LCP_ACK received by local PPP */
    int snt_ack_len;		/* length of LCP_ACK sent by local PPP */
    uchar *snt_ack;		/* last LCP_ACK sent by local PPP */
    l2f_tunneltype *owner;      /* tunnel this MID is associated with */
    hwidbtype *idb;		/* IDB this MID is associated with */
    uchar authtype;		/* type of authentication used */
    uchar chap_id;		/* CHAP identification number */
    uchar outgoing_sequence;	/* Current outgoing sequence number */
    uchar incoming_sequence;	/* Cur. expected incoming sequence number */
    long packets_out;		/* Number of packets sent over tunnel */
    long bytes_out;		/* Number of bytes sent over tunnel */
    long packets_in;		/* Number of packets received over tunnel */
    long bytes_in;		/* Number of bytes received over tunnel */
};
 
struct l2f_mid_elem_type_ {
    struct l2f_mid_elem_type_ *next;
    struct l2f_midtype_ *mid;  
};

struct l2f_tunneltype_ {
    struct l2f_tunneltype_ *next;  /* pointer for linked list */
    char nas_name[MAX_VPN_SMALL_PARM];
    char gw_name[MAX_VPN_SMALL_PARM];
    char gw_domain[MAX_VPN_SMALL_PARM];
    char nas_password[MAX_VPN_SMALL_PARM];
    char gw_password[MAX_VPN_SMALL_PARM];
    int type;
    int state;
    boolean local_config;	/* this was configured from NVRAM */
    sys_timestamp state_time;   /* when did we move to this state? */
    ipsocktype *sock;
    /* frame relay output */
    ipaddrtype ip_address;
    ipaddrtype source_ip;
    l2f_midtype *mids;
    uchar nas_response[MD5_LEN];
    char nas_challenge[MAX_CHALLENGE_LEN];
    uint nas_challenge_length;
    uint nas_key;		/* An XOR of the 4 sets of 4 bytes in the 
					response */
    char gw_challenge[MAX_CHALLENGE_LEN];
    uint gw_challenge_length;
    uchar gw_response[MD5_LEN];
    uint gw_key;		/* An XOR of the 4 sets of 4 bytes in the 
					response */
    boolean offset_exists;
    boolean key_exists;
    boolean do_checksum;   /* boolean */
    ushort version_and_flags;
    uchar outgoing_sequence;
    uchar incoming_sequence;
    ushort nas_clid;
    ushort gw_clid;
    int next_mid;
    boolean instigator;  /* Did we start this? (Are we the NAS)*/
    int midcount;   /* current number of MIDs on the tunnel */
    int virt_temp;  /* virtual template to use */
    sys_timestamp last_traffic; /* last time we heard from remote */
    boolean echo_outstanding;
    uchar protocol;
    long packets_out;		/* Number of packets sent over MID */
    long bytes_out;		/* Number of bytes sent over MID */
    long packets_in;		/* Number of packets received over MID */
    long bytes_in;		/* Number of bytes received over MID */
    queuetype tunnel_mid_elemQ;
};

struct l2f_headertype_ {
    ushort version_and_flags;
    uchar protocol;
    uchar sequence;
    ushort mid;
    ushort clid;
    ushort length;
    ushort offset;
    ulong key;
};

/* header to be used when there is no offset */
struct l2f_shortheadertype_ {
    ushort version_and_flags;
    uchar protocol;
    uchar sequence;
    ushort mid;
    ushort clid;
    ushort length;
    ushort offset;
    ulong key;
};

struct open_info {
    uchar chapname[MAX_VPN_SMALL_PARM];
    uchar chapchal[MAX_VPN_SMALL_PARM];
    uchar chapresp[MAX_VPN_SMALL_PARM];
    uchar rcv_ack[MAX_VPN_LARGE_PARM];
    uchar snt_ack[MAX_VPN_LARGE_PARM];
};

struct l2f_resend_elem_ {
    void *next;
    l2f_tunneltype *tunnel;
    int times_resent;
    sys_timestamp last_sent;
    int datalen;
    char *data;
}; 

struct l2f_tunnel_infotype_ {
    struct l2f_tunnel_infotype_ *next;  /* pointer for linked list */
    char nas_name[MAX_VPN_SMALL_PARM];
    char gw_name[MAX_VPN_SMALL_PARM];
    char gw_domain[MAX_VPN_SMALL_PARM];
    char nas_password[MAX_VPN_SMALL_PARM];
    char gw_password[MAX_VPN_SMALL_PARM];
    int type;
    ipaddrtype ip_address;
    ipaddrtype source_ip;
    int virt_temp;  /* virtual template to use */
    int direction;  /* incoming our outgoing */
};

struct l2f_midlock_info {
    l2f_midtype *mid;
    l2f_tunneltype *owner;
    char *name;
};

STRUCTURE_REF(l2f_shortheadertype);
STRUCTURE_REF(l2f_headertype);
STRUCTURE_REF(l2f_resend_elem);
STRUCTURE_REF(l2f_mid_elem_type);


/*
 * l2f.c
 */
int l2f_create_mid(hwidbtype *, l2f_tunneltype *, char *,
			  uchar, uchar, uchar, uchar *, uchar,
			  uchar *, int, uchar *, int, uchar *);
boolean l2f_send(l2f_tunneltype *, uchar *, int, int);
l2f_tunnel_infotype *l2f_find_tunnel_by_name(char *);
void l2f_activate_waiting_mids(l2f_tunneltype *);
l2f_midtype *l2f_make_mid_structure(l2f_tunneltype *);
void l2f_tunnel_move_to_state(l2f_tunneltype *, int);
void l2f_mid_move_to_state(l2f_midtype *, int);
void l2f_destroy_mid_structure(l2f_midtype *, boolean);
void l2f_init(subsystype *);
void l2f_destroy_mid(l2f_midtype *);
void l2f_packet_send(hwidbtype *, paktype *);
char *l2f_tunnel_state_string(int);
boolean l2f_clean_resendQ(l2f_tunneltype *, int, int);
void l2f_clear_tunnel(l2f_tunneltype *);
l2f_tunneltype *l2f_make_tunnel_structure(l2f_tunnel_infotype *);
l2f_tunneltype *l2f_find_active_tunnel_by_address(ipaddrtype);
void l2f_initiate_tunnel_shutdown(l2f_tunneltype *);


/* 
 * l2f_db.c 
 */
l2f_midtype *l2f_get_mid(int, int);
l2f_midtype *l2f_get_mid_by_idb (hwidbtype *);
boolean l2f_store_mid(int, int, l2f_midtype *);
void l2f_remove_mid(int, int);
void l2f_init_mid_storage(void);
boolean l2f_tunnel_exists (l2f_tunneltype *);
boolean l2f_store_tunnel (l2f_tunneltype *);
l2f_tunneltype *l2f_find_tunnel_by_clid(int);
l2f_tunneltype *l2f_find_active_tunnel_by_name(char *);
l2f_tunneltype *l2f_find_active_tunnel_by_names(char *, char *);
l2f_tunneltype *l2f_find_active_tunnel_by_address(ipaddrtype);
boolean l2f_destroy_tunnel_structure(l2f_tunneltype *);
l2f_tunneltype *l2f_getnext_tunnel(void);
boolean l2f_tunnel_mid_elem_exists(l2f_tunneltype *, l2f_midtype *);
boolean l2f_store_tunnel_mid_elem(l2f_tunneltype *, l2f_midtype *);
l2f_midtype *l2f_getnext_mid_from_tunnel(l2f_tunneltype *);
boolean l2f_remove_mid_elem_from_tunnel(l2f_tunneltype *, l2f_midtype *);
void l2f_remove_mids_from_tunnel(l2f_tunneltype *);
struct l2f_midlock_info *l2f_lock_mids_in_tunnel(l2f_tunneltype *, int *);
void l2f_unlock_mids_in_tunnel(l2f_tunneltype *, struct l2f_midlock_info *,
    int);
 
extern queuetype *l2f_hashtable_ptr;

/*
 * l2f_packets.c
 */
int l2f_get_management_header(l2f_tunneltype *, uchar *, boolean);
int l2f_nas2gw_tunnel0(l2f_tunneltype *, uchar *, int);
int l2f_gw2nas_tunnel0(l2f_tunneltype *, uchar *, int);
int l2f_nas2gw_tunnel1(l2f_tunneltype *, uchar *, int);
int l2f_gw2nas_tunnel1(l2f_tunneltype *, uchar *, int);
int l2f_gw2nas_mid0(l2f_midtype *, uchar *, int);
int l2f_nas2gw_mid0(l2f_midtype *, uchar *, int);
int l2f_echo(l2f_tunneltype *, uchar *, int);
int l2f_echo_resp(l2f_tunneltype *, uchar *, int);
void l2f_receive_l2f_conf(l2f_headertype *, uchar *, int, ipaddrtype);
void l2f_receive_l2f_open(l2f_headertype *, uchar *, int);
void l2f_receive_l2f_close(l2f_headertype *, uchar *, int);
void l2f_receive_l2f_echo(l2f_headertype *, uchar *, int);
int l2f_pkt_close_mid(l2f_midtype *, uchar *, int, int);
int l2f_pkt_close_tunnel(l2f_tunneltype *, uchar *, int);
