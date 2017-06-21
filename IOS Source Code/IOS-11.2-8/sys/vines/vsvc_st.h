/* $Id: vsvc_st.h,v 3.2 1995/11/17 17:59:58 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/vines/vsvc_st.h,v $
 *------------------------------------------------------------------
 * vsvc_st.h - Support for the `Streettalk' Service.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vsvc_st.h,v $
 * Revision 3.2  1995/11/17  17:59:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:46:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/24  22:33:09  hampton
 * Further work to parts of VINES to make it use the passive timer macros
 * for all its timers.  It no longer have any direct references to the
 * system clock.  [CSCdi37584]
 *
 * Revision 2.1  1995/06/07 23:16:21  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/**************************************************
 *
 *	        Streettalk Definitions
 *
 **************************************************/

#define VST_ERR_BADNAME		1001
#define VST_ERR_ORGLONG		1002
#define VST_ERR_DOMLONG		1003
#define VST_ERR_OBJLONG		1004
#define VST_ERR_NAMELONG	1005
#define VST_ERR_NOSUCHOBJ	1013
#define VST_ERR_INVSESHAN	1017
#define VST_ERR_UNKNOWN		1032

/* From Banyan API file STSTRUCT.H */
#define VST_OBJ_UNSPEC 		0
#define VST_OBJ_USER		1
#define VST_OBJ_SERVICE		2
#define VST_OBJ_LIST		3
#define VST_OBJ_ALIAS		4
#define VST_OBJ_DOMAIN		5

#define VST_REC_UNKNOWN 	0
#define VST_REC_USER 	 	1
#define VST_REC_SERVICE  	2
#define VST_REC_LIST 	 	3
#define VST_REC_ALIAS 	 	4
#define VST_REC_ENUM 	 	5
#define VST_REC_LSTMEMB  	6
#define VST_REC_ACLREC 	 	7
#define VST_REC_ASSOCREC 	8
#define VST_REC_DOMREC 	 	9

#define VST_CAT_SS 		1

#define MAXLSTSET		10
#define ENUMSET			15

/* End from Banyan API file STSTRUCT.H */

/*
 * Streettalk Name Syntax
 *
 * A streettalk name is always in the form <item>@<domain>@<org>.  Each of
 * the three parts can consist of any set of alphanumeric characters,
 * plus a few punctuation characters.  The definitions for the length of
 * each portion can be found in the file vines_services.h.  Other
 * definitions related to the syntax of names should go here.
 */
#define VST_CHAR_PUNCTUATION	" .,-_"
#define VST_CHAR_WILDCARD	"*"
#define VST_CHAR_SEPARATOR	"@"
#define VST_CHAR_ALLOWED	VST_CHAR_PUNCTUATION \
				VST_CHAR_WILDCARD VST_CHAR_SEPARATOR

/*
 * Associated Record Information
 */
#define VST_AREC_NONE   0
#define VST_AREC_PORTS 99

#define VST_AREC_DATASIZE 1024

/*
 * Client "Talk" ports.
 */
#define VST_MAXTPORTS 3

/*
 * Periodic events
 */
#define VST_SUMMARY_INTERVAL	( 3 * ONEHOUR)
#define VST_EXPIRATION		(96 * ONEHOUR)

#define VST_SEARCH_INTERVAL	(30 * ONESEC)	/* ms between search msgs */
#define VST_SEARCH_MAX		 5 * VST_SEARCH_INTERVAL

/*
 * Delays
 */
#define VST_XMIT_DELAY        	(37 * ONESEC)	/* max delay before xmit */
#define VST_XMIT_JITTER        	(7/37)		/* actual delay is 30-37 */
#define VST_XMIT_PACING		20		/* ms between transmits  */

#define transmit_time  gp_timer

#define VST_NEED_PACING        (5 * ONEMIN)
#define VST_NEED_DELAY         (5 * ONESEC)
#define VST_MAX_DEST		5

/*
 * Hop Count
 *
 * According to the VINES Technical Support II class, the hop count field
 * has a limit of 15, and counts down.  Some quick empirical testing
 * shows that it actually has a hop count of 20, and counts up.
 */
#define VST_MAX_HOPS	       20

/*
 * Server Priorities
 *
 * Groups can now be owned by multiple servers, and the servers indicate
 * the order of priority in and detail messages.  This value is encoded
 * into the misc info field.  If the 'primary' bit is on, the precedence
 * field is ignored.  If it is off, use the precedence field, with the
 * lowest number winning.
 */
#define VST_MISC_PRIMARY	0x0002
#define VST_MISC_PRECEDENCE	0xF000
#define VST_HIGHEST_PRECEDENCE	0
#define VST_LOWEST_PRECEDENCE	15

/*
 * Domain Sequence Numbers
 *
 * Excerpt from mail from Jim Toga (toga@banyan.com):
 *
 * This value does not have a lot of usage/meaning at this point.  The
 * value is initialized to 0x0101 (the absence of the 0x0100 bit indicates
 * that the group is on a rev3 server; no longer really an issue...).  The
 * value is incremented each time the group is modified is some manner
 * (add/del of items, etc.)
 */
#define VST_DSEQ_INIT_VALUE	0x0101

/*
 * Sizes
 */
#define VST_SUMMARY_COUNT      40
#define VST_WELCOME_COUNT      VST_SUMMARY_COUNT
#define VST_DETAIL_COUNT       25		/* max groups per server */
#define VST_NEED_COUNT         70
#define VST_COLDDUMP_MAX_GROUPS  20

/*
 * The Operate Call (16): Sub-procedure IDs
 *
 * *** These ID numbers are identical to the top level StreetTalk
 * *** procedure ID numbers.
 */
#define VST_OP_LOOKUP		   0
#define VST_OP_UPDATE_OBJ	   1
#define VST_OP_REMOVE_OBJ	   2
#define VST_OP_ADD_OBJ		   3
#define VST_OP_CREATE_DOMAIN	   4
#define VST_OP_CRE_DEL_DOMAIN	   5
#define VST_OP_ENUMERATE	   6
#define VST_OP_ADD_MEMBER	   7
#define VST_OP_DELETE_MEMBER	   8
#define VST_OP_MEMBERSHIP	   9
#define VST_OP_LIST_MEMBERS	  10
#define VST_OP_SET_PASSWORD	  11
#define VST_OP_LOGIN		  13
#define VST_OP_LOGOUT		  14
#define VST_OP_NICENAME		  15

/*
 * The "New Income" Call: Sub-procedure IDs
 */
#define VST_NI_SUMMARY             1
#define VST_NI_DETAIL              2
#define VST_NI_NEED                3
#define VST_NI_WELCOME             4
#define VST_NI_MAX                 VST_NI_WELCOME

enum vst_freshness {
    /*
     * The information is owned by this router and should never be
     * accepted.
     */
    VST_LOCAL,

    /*
     * Do nothing with this new information until the ten minute holddown
     * period has passed.
     */
    VST_MOVED,

    /*
     * Something at the group level has changed on the server.  The
     * router needs to get new information for this server.
     */
    VST_CHANGED,

    /*
     * A new periodic broadcast.  No information has changed.
     */
    VST_PERIODIC,

    /*
     * A duplicate broadcast.
     */
    VST_DUPLICATE,

    /*
     * Old information.
     */
    VST_OLD
};

/*
 * Domain@Org structure.
 *
 * In all existing VINES releases, a group should only exist on one
 * server.  It is possible for a group to exist in multiple places
 * because of things like network failures while moving a group, etc.
 * It is not expected to be a lasting condition.  A forthcoming VINES
 * release (6.0?), however, will support maintaining backup group
 * information (read-only) at two alternate sites.  Keeping multiple
 * entries for the same group seems the easiest way to handle both of
 * these situations.  It will also continue to work should Banyan ever
 * decide that two backup sites is not enough.
 */
typedef struct vst_groupentry_ {
    treeLink link;		/* link data, must be first */

    /*
     * General Information
     */
    treeKey namehash;

    /*
     * Group Specific Information
     */
    char   org[VST_ORGLEN];
    char   domain[VST_DOMAINLEN];
    ulong  netgroup_id;
    ulong  localgroup_id;
    ushort domain_seq;
    sys_timestamp expires;

    /*
     * Server Specific Information.  The priority field indicates the
     * owner/backup information.
     */
    ulong  serverid;
    ulong  change_id;
    ushort priority;	/* 0 = owner, 1 = first backup, etc. */

    /*
     * Miscellanea
     */
    boolean is_servername;
    dqueue_t servername_thread;
    dqueue_t groupname_thread;
} vst_groupentry;


/*
 * Local structures
 */
typedef struct vst_msg_dest_ {
    sys_timestamp when;
    boolean broadcast;
    int     count;
    ulong   dest[VST_MAX_DEST];
} vst_msg_dest;

typedef struct vst_aid_ {
    vinesipcsocket *socket;
    vsvc_rtn_info *routine;
    void *data;

    ulong  *checksum;
    ushort *count;
    uchar  *addr;

    boolean sent;
    ulong   server;
} vst_aid;

typedef struct vst_handle_ {
    dqueue_t q;
    sys_timestamp timer;
    ulong session_handle;
    boolean valid;
    ulong net;
    ushort host;
    ushort port;
    vinesnonce nonce;
    char *username;
} vst_handle;

#define VST_HANDLE_SHORT   (1*ONEMIN)	/* must hear from VS or nuked */
#define VST_HANDLE_NORMAL  (1*ONEHOUR)	/* normal age */

typedef struct vst_operation_ {
    dqueue_t q;
    sys_timestamp timer;
    ulong  handle;	/* Client that sent request */
    ulong  server;	/* Server processing request */
    ushort type;	/* Type of operation requested */
    ushort caller;	/* Original ST function that caused op call */
    char   name[VST_TOTALLEN]; /* Request Data */
} vst_operation;

#define VST_OPERATION_TIME  (1*ONEMIN)

/**************************************************
 *
 *	      Inlines
 *
 **************************************************/

static inline char *vst_group2name (
    vst_groupentry *g,
    char *group_name)
{
    sprintf(group_name, "%s@%s", g->domain, g->org);
    return(group_name);
}

static inline char *vst_make_group_name (
    char *group_name,
    char *domain_name,
    char *org_name)
{
    sprintf(group_name, "%s@%s", domain_name, org_name);
    return(group_name);
}


/**************************************************
 *
 *	      Externs
 *
 **************************************************/

extern streettalk_enabled;
extern vst_msg_dest	vst_dest_welcome;
extern vst_msg_dest	vst_dest_detail;
extern vst_msg_dest	vst_dest_need;

extern void vst_send_ni_welcome(void);
extern void vst_send_ni_detail(void);
extern void vst_send_ni_need(void);
extern void vst_send_ni_summary(void);
extern void vst_set_gen_dest(vst_msg_dest *, ulong);
extern void vst_cold_dump_send_call(ulong, char *);
extern void vst_create_local_group(char *, char *, ulong, ushort, boolean);
extern void vst_delete_local_group(char *, char *, ulong, boolean);
extern void vst_user_validated(ushort, char *);
extern boolean vines_is_st_welcome(paktype *);
extern void vst_send_search(boolean);
