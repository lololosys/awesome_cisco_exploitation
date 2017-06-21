/* $Id: vines_services.h,v 3.5.18.2 1996/08/07 09:03:58 snyder Exp $
 * $Source: /release/112/cvs/Xsys/vines/vines_services.h,v $
 *------------------------------------------------------------------
 * vines_services.h - Support for the VINES Application Layer Services.
 *
 * April 1994, David Hampton
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vines_services.h,v $
 * Revision 3.5.18.2  1996/08/07  09:03:58  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.5.18.1  1996/03/18  22:32:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.16.2  1996/03/07  11:05:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.16.1  1996/02/20  21:47:04  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/08  20:14:14  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.4  1995/12/08  04:56:28  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.3  1995/12/07  22:33:45  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.2  1995/11/17  17:58:38  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:45:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:15:08  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define VSVC_TREE_AGE_INTERVAL  (30*ONEMIN)

#define VSVC_NO_ERROR		0
#define VSVC_DEFERRED_REPLY	1
#define VSVC_ERR_BADPROCNUM	101
#define VSVC_ERR_NO_MEMORY	104
#define VSVC_ERR_INVALID	153
#define VSVC_ERR_TIMEOUT	163

#include <string.h>
#include <ciscolib.h>

extern boolean vfs_debug;
extern boolean vss_debug;
extern boolean vst_debug;
extern boolean vvs_debug;
extern boolean vnsm_debug;

extern queuetype vsvc_queue;		/* Queue of all supported services */
extern rbTree* vines_server_tree;


/**********************************************************************
 *
 *		 VINES APPLICATION LAYER DEFINITIONS
 *
 **********************************************************************/

typedef enum vsvc_send_action_ {
    VSVC_SEND_DATA   = 1,
    VSVC_SEND_NODATA = 2,
} vsvc_send_action;

typedef enum vsvc_rcv_action_ {
    VSVC_ACCEPT_DATA   = 1,
    VSVC_ACCEPT_NODATA = 2,
    VSVC_REJECT        = 3,
    VSVC_IGNORE        = 4,
    VSVC_PROXY         = 5
} vsvc_rcv_action;


typedef uchar * (*vsvc_pack)(uchar *packed, uchar *unpacked);
typedef ushort  (*vsvc_call)(vinesipcsocket *socket, uchar *call, uchar *reply);
typedef void    (*vsvc_return)(vinesipcsocket *socket, uchar *reply);
typedef void    (*vsvc_search)(vinesipcsocket *socket, ushort user_num,
			       uchar *user_string, uchar *info1,
			       uchar *info2, uchar *info3);
typedef void    (*vsvc_retadd)(vinesipcsocket *socket, ushort proto_rev,
			       ulong service_rev, vinesaddress *service_address);
typedef void    (*void_fn)(void);
typedef void    (*failure_fn)(vinesipcsocket *socket, ushort reason);
typedef boolean (*ager_fn)(serverentry *);

/*
 * Description of all procedures supported by a service.
 */
typedef struct vsvc_rtn_info_ {
    uchar		name[64];
    uchar		abbrev[32];
    ushort		number;

    vsvc_send_action	send_query_action;
    vsvc_rcv_action	rcv_reply_action;
    vsvc_return		rcv_reply_fn;

    vsvc_rcv_action	rcv_query_action;
    vsvc_send_action	send_reply_action;
    vsvc_call		rcv_query_fn;
    ushort		rcv_reject_code;

    failure_fn		abort_fn;
    failure_fn		reject_fn;

    ushort		query_len;
    vsvc_pack		query_pack;
    vsvc_pack		query_unpack;
    ushort		query_padded;

    ushort		reply_len;
    vsvc_pack		reply_pack;
    vsvc_pack		reply_unpack;
    ushort		other_len;
    vsvc_pack		other_pack;
    vsvc_pack		other_unpack;
} vsvc_rtn_info;

/*
 * Description of a service supported by the router.
 */
typedef struct vsvc_info_ {
    struct vsvc_info_	*next;			/* queueing info              */
    
    char		name[64];		/* Long name for type of service */
    char		abbrev[16];		/* Abbreviation for service type */
    char		description[VST_TOTALLEN]; /* Description of this service   */
    char		svc_name[VST_TOTALLEN];	/* Streettalk name for service   */
    boolean		*debug;
    boolean		answer_search;		/* Respond to a search for this svc */

    ushort		well_known_port;	/* Statically assigned port   */
    ushort		transient_port;		/* Dynamic port for responses */
    ushort		temporary_port;		/* Port for raw IPC data      */

    char		version_string[20];	/* Version being emulated  */
    ulong		version;		/* Version being emulated  */
    ulong		min_acceptable;		/* Min version can talk to */
    ulong		max_acceptable;		/* Max version can talk to */

    void_fn		start;			/* Startup routine               */
    void_fn		stop;			/* Shutdown routine              */
    void_fn		display;		/* Data Display routine          */
    ager_fn		age_service_info;	/* Age per server info for a svc */
    void_fn		periodic;		/* Periodic function to call, if any */
    mgd_timer		timer;
    ulong		timer_period_init;
    ulong		timer_period_normal;

    /* Raw IPC info */
    vipc_upcall		*raw_ipc;		/* Routine to handle RAW data  */

    /* NetRPC info */
    vsvc_search		search_fn;		/* Handle NetRPC Search message     */
    vsvc_retadd 	retadd_fn;		/* Handle NetRPC Return Address msg */
    vsvc_rtn_info	*routines;		/* NetRPC procedures descriptions   */
    int			routine_count;		/* Number of NetRPC procedures      */
    vsvc_rcv_action	routine_default;	/* Response to unknown procedures   */
    ushort		reject_code;		/* Code to use when rejecting proc  */
    dqueue_t		queries;		/* Outstanding direct queries */
    ushort		last_broadcast;		/* Last broadcast procedure   */

    /*
     * Run Time Statistics
     */
    ulong		msgs_in;		/* Messages input */
    ulong		msgs_out;		/* Messages output */
    ulong		bytes_in;		/* Bytes input */
    ulong		bytes_out;		/* Bytes output */
} vsvc_info;

/*
 * Structure to remember an outstanding service query.  This is
 * maintained on a per service queue, so it only needs to maintain the
 * destination and procedure numbers.
 */
typedef struct vsvc_record_ {
    dqueue_t q;
    ulong  net;
    ushort host;
    ushort procedure;
} vsvc_record;

#define FOR_ALL_SERVICES(service) \
    for ((service) = vsvc_queue.qhead; (service); (service) = (service)->next)

#define VSVC_MIN_VER		30000
#define VSVC_MAX_VER		99999

#define VSVC_MAX_SLOT		15
#define VSVC_MAX_LINE		4095

/**********************************************************************
 *
 *		    CISCO specific NSM definitions
 *	    (replace when Banyan assigns values to cisco)
 *
 **********************************************************************/

/*
 * Interface number assignments
 */
#define VSVC_IF_HDLC	11	/* HDLC Protocol                */
#define VSVC_IF_BA	12	/* Banyan Block Async Protocol  */
#define VSVC_IF_X25	16	/* CCITT X.25 level 3		*/
#define VSVC_IF_ETHER	31	/* 3Com ethernet 3C503		*/
#define VSVC_IF_TOKEN	33	/* 3Com 3C603 Token-Ring	*/
#define VSVC_IF_FDDI	96	/* Undefined			*/
#define VSVC_IF_ATM	97	/* Undefined			*/
#define VSVC_IF_SMDS	98	/* Undefined			*/
#define VSVC_IF_FRAME	99	/* Undefined			*/
#define VSVC_IF_ISDN	102	/* LAPD pseudo layer */


/*
 * Server structure
 */
struct serverentry_ {
    treeLink link;		/* link data, must be first */

    /* General Information */
    ulong serverid;		/* The unique server serial number */
    sys_timestamp started;	/* when entry was created */

    /* StreetTalk Information */
    boolean	  st_info_valid;
    vinesaddress  st_port;
    ulong         st_database_id;
    ulong         st_change_id;
    ulong         st_reinforce_id;
    boolean       st_need_info;
    sys_timestamp st_last_update;
};


/***********************************************************************
 *
 *			         Inlines
 *
 ***********************************************************************/

static inline void vsvc_make_name (
    char *full_name,
    char *item,
    char *group,
    char *org)
{
    sprintf(full_name, "%s@%s@%s", item, group, org);
}

extern char* vsvc_hostname(void);
static inline void vsvc_insert_hostname (char *string)
{
    sstrncpy(string, vsvc_hostname(), VST_DOMAINLEN);
}

static inline void vsvc_insert_description (char *dest, char *src)
{
    sstrncpy(dest, src, VST_DESCRIPTION);
}

/*
 * The following inlines are needed because Banyan packs its data
 * structures before transmitting them.  These routines handle insertion
 * and extraction of the data fields without cluttering up the C routines
 * so that they are ureadable.
 */

/*
 * vines_insert_stringz
 *
 * Insert a null terminated string.  This string will not be preceeded by
 * a length indicator, nor will it be padded to a word boundary.  It
 * simply is guaranteed to be NULL terminated.
 */
static inline uchar *vines_insert_stringz (
    uchar *packed,
    char *data,
    int maxlength)
{
    int length;

    length = strlen(data);
    if (length >= maxlength)
	length = maxlength-1;
    sstrncpy(packed, data, length+1); /* bec strncpy counts terminator */
    packed += length;
    *packed++ = '\0';		      /* force the null terminator */
    return(packed);
}

/*
 * vines_insert_string
 *
 * Insert a string that is preceeded by a length indicator.  This string
 * will always be padded to a word boundary, though the pad is not
 * included in the length.  The string is not NULL terminated because it
 * includes a length.
 */
static inline uchar *vines_insert_string (
    uchar *packed,
    char *data,
    int maxlength)
{
    vinesstring *string;
    int length;

    length = strlen(data);
    if (length >= maxlength)
	length = maxlength-1;
    string = (vinesstring *)packed;
    PUTSHORT(&string->length, length);
    sstrncpy(string->data, data, length+1); /* bec strncpy counts terminator */
    if (length & 1)
	length++;
    return(packed + 2 + length);
}

static inline uchar *vines_extract_string (
    uchar *packed,
    char *data,
    int maxlength)
{
    vinesstring *string;
    int length;

    string = (vinesstring *)packed;
    length = GETSHORT(&string->length);
    if (length >= maxlength)
	buginf(vines_invalid_string_len_msg, vsvc_str);
    sstrncpy(data, string->data, length+1); /* bec strncpy counts terminator */
    if (length & 1)
	length++;
    return(packed + 2 + length);
}

static inline uchar *vines_insert_uchar (uchar *packed, uchar value)
{
    *packed++ = value;
    return(packed);
}

static inline uchar *vines_extract_uchar (uchar *packed, uchar *value)
{
    *value = *packed++;
    return(packed);
}

static inline uchar *vines_insert_ushort (uchar *packed, ushort value)
{
    ushort *dest = (ushort *)packed;

    PUTSHORT(dest++, value);
    return((uchar *)dest);
}

static inline uchar *vines_extract_ushort (uchar *packed, ushort *value)
{
    ushort *source = (ushort *)packed;

    *value = GETSHORT(source++);
    return((uchar *)source);
}

static inline uchar *vines_insert_ulong (uchar *packed, ulong value)
{
    ulong *dest = (ulong *)packed;

    PUTLONG(dest++, value);
    return((uchar *)dest);
}

static inline uchar *vines_extract_ulong (uchar *packed, ulong *value)
{
    ulong *source = (ulong *)packed;

    *value = GETLONG(source++);
    return((uchar *)source);
}

static inline uchar *vines_insert_char (uchar *packed, char value)
{
    return(vines_insert_uchar(packed, (uchar)value));
}
static inline uchar *vines_extract_char (uchar *packed, char *value)
{
    return(vines_extract_uchar(packed, (uchar *)value));
}
static inline uchar *vines_insert_short (uchar *packed, short value)
{
    return(vines_insert_ushort(packed, (ushort)value));
}
static inline uchar *vines_extract_short (uchar *packed, short *value)
{
    return(vines_extract_ushort(packed, (ushort *)value));
}
static inline uchar *vines_insert_long (uchar *packed, long value)
{
    return(vines_insert_ulong(packed, (ulong)value));
}
static inline uchar *vines_extract_long (uchar *packed, long *value)
{
    return(vines_extract_ulong(packed, (ulong *)value));
}
static inline uchar *vines_insert_boolean (uchar *packed, short value)
{
    return(vines_insert_ushort(packed, (ushort)value));
}
static inline uchar *vines_extract_boolean (uchar *packed, short *value)
{
    return(vines_extract_ushort(packed, (ushort *)value));
}

static inline uchar *vines_insert_address (uchar *packed, vinesaddress *value)
{
    ulong *source, *dest;

    source = (ulong *)value;
    dest = (ulong *)packed;
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    return((uchar *)dest);
}

static inline uchar *vines_extract_address (uchar *packed, vinesaddress *value)
{
    ulong *source, *dest;

    source = (ulong *)packed;
    dest = (ulong *)value;
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    return((uchar *)source);
}

static inline uchar *vines_insert_macaddress (uchar *packed, vinesmacaddress *mac)
{
    ulong *source, *dest;
    ushort *source_s, *dest_s;

    source = (ulong *)mac;
    dest = (ulong *)packed;
    PUTLONG(dest++,  GETLONG(source++));

    source_s = (ushort *)source;
    dest_s = (ushort *)dest;
    PUTSHORT(dest_s++,  GETSHORT(source_s++));
    return((uchar *)dest_s);
}

static inline uchar *vines_extract_macaddress (uchar *packed, vinesmacaddress *mac)
{
    ulong *source, *dest;
    ushort *source_s, *dest_s;

    source = (ulong *)packed;
    dest = (ulong *)mac;
    PUTLONG(dest++,  GETLONG(source++));

    source_s = (ushort *)source;
    dest_s = (ushort *)dest;
    PUTSHORT(dest_s++,  GETSHORT(source_s++));
    return((uchar *)source_s);
}

static inline uchar *vines_insert_nonce (uchar *packed, vinesnonce *nonce)
{
    ulong *source, *dest;
    ushort *source_s, *dest_s;

    source = (ulong *)nonce;
    dest = (ulong *)packed;
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));

    source_s = (ushort *)source;
    dest_s = (ushort *)dest;
    PUTSHORT(dest_s++,  GETSHORT(source_s++));
    return((uchar *)dest_s);
}

static inline uchar *vines_extract_nonce (uchar *packed, vinesnonce *nonce)
{
    ulong *source, *dest;
    ushort *source_s, *dest_s;

    source = (ulong *)packed;
    dest = (ulong *)nonce;
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));
    PUTLONG(dest++,  GETLONG(source++));

    source_s = (ushort *)source;
    dest_s = (ushort *)dest;
    PUTSHORT(dest_s++,  GETSHORT(source_s++));
    return((uchar *)source_s);
}

/**********************************************************************
 *
 *		   Server Tree Interface Routines
 *
 **********************************************************************/

/*
 * vsvc_ServerKey
 * Create a treeKey for a neighbor which is always unique.
 */
static inline treeKey
vsvc_ServerKey (ulong serverid)
{
    treeKey key;
    key.l = serverid;
    return key;
}

/*
 * vsvc_ServerCast
 * Do data type conversion between links and servers.
 */
static inline serverentry*
vsvc_ServerCast (treeLink* link) { return(serverentry*)link; }

/*
 * vsvc_UnprotectServer
 * Allow a server to be freed or reused.
 */
static inline void
vsvc_UnprotectServer (serverentry* s) 
{ 
    RBTreeNodeProtect(&s->link,FALSE); 
}

/*
 * vsvc_ProtectServer
 * Protect a server from being freed.
 */
static inline void
vsvc_ProtectServer (serverentry* s) 
{ 
    RBTreeNodeProtect(&s->link,TRUE); 
}

/*
 * vsvc_ServerProtected
 * Return protection status of server treeLink entry.
 */
static inline boolean
vsvc_ServerProtected (serverentry* s)
{
    return RBTreeNodeProtected(&s->link);
}

/*
 * GetNextServer - Search for the next server and return it w/o protection
 */
static inline serverentry*
vsvc_GetNextServer (serverentry* server)
{
    serverentry* s = server;

    if (vines_server_tree == NULL)
	return NULL;
    if (s == NULL) {
	s = vsvc_ServerCast(RBTreeFirstNode(vines_server_tree));
    } else {
	s = vsvc_ServerCast(RBTreeNextNode(&s->link));
    }
    return s;
}

/*
 * vsvc_GetFirstServer
 * Get the first server from the tree.
 */
static inline serverentry*
vsvc_GetFirstServer (void)
{ 
    return vsvc_GetNextServer(NULL); 
}

/*
 * vsvc_GetBestServer
 * Return the specified server OR the next greater server.
 */
static inline serverentry*
vsvc_GetBestServer (treeKey key)
{
    return vsvc_ServerCast(RBTreeBestNode(vines_server_tree,key));
}

/*
 * vsvc_InsertServer
 * Add a server to our table.
 */
static inline void
vsvc_InsertServer (serverentry* s) 
{
    treeKey key = vsvc_ServerKey(s->serverid);
    if (vines_server_tree != NULL)
	RBTreeInsert(key,vines_server_tree,&s->link);
}

/*
 * vsvc_DeleteServer
 * Delete a server from our table.
 */
static inline void
vsvc_DeleteServer (serverentry* s)
{
    if ((vines_server_tree != NULL)&&(s!=NULL))
	RBTreeDelete(vines_server_tree,&s->link);
}

/*
 * vsvc_ReinitServer
 * Release any resources in use for this server
 */
static inline void
vsvc_ReinitServer (serverentry* s)
{
				/* Currently no special resources */
}

/*
 * vsvc_CreateServer
 * initialize a new server
 */
static inline serverentry*
vsvc_CreateServer (ulong serverid)
{
    serverentry* s;

    if (vines_server_tree == NULL)
	return NULL;

    s = (serverentry*) RBTreeGetFreeNode(vines_server_tree);
    if (s != NULL)
	vsvc_ReinitServer(s);
    else
	s = malloc_named(sizeof(struct serverentry_), "Vines Server");
    if (s == NULL)
	return NULL;
    memset(s, 0, sizeof(struct serverentry_)); /* clear node */
    s->serverid = serverid;
    GET_TIMESTAMP(s->started);
    vsvc_InsertServer(s);
    return s;
}

/*
 * vsvc_FindServer
 *
 * Find a server for the serverid. No NULL parameters allowed.
 */
static inline serverentry*
vsvc_FindServer (ulong serverid)
{
    serverentry* s;
    treeKey key;

    key = vsvc_ServerKey(serverid);
    if (vines_server_tree == NULL)
	return (NULL);
    s = vsvc_ServerCast(RBTreeSearch(vines_server_tree,key));
    return (s);
}

/*
 * vsvc_WalkServerTree
 *
 * Call the specified function for each node in the tree.
 */
static inline boolean
vsvc_WalkServerTree (treeProc proc, void *data)
{
    return(RBTreeForEachNode(proc, data, vines_server_tree, FALSE));
}

static inline serverentry *
vsvc_FindSelf (boolean debug, char *tag)
{
    return(vsvc_FindServer(vines_network));
}


/***********************************************************************
 *
 *			EXTERNS and PROTOTYPES
 *
 ***********************************************************************/

extern const char vsvc_nomem[];
extern mgd_timer vsvc_tree_timer;

extern serverentry *vsvc_CreateServer(ulong);
extern serverentry *vsvc_FindServer(ulong);
extern void vsvc_install_svc(vsvc_info *);
extern void vsvc_send_call(vinesipcsocket *, vsvc_info *, vsvc_rtn_info *, uchar *);
extern void vsvc_send_reply(vinesipcsocket *, vsvc_info *, vsvc_rtn_info *, uchar *);
extern void vsvc_send_search(vinesipcsocket *, vsvc_info *, ushort, uchar *, uchar *, uchar *, uchar *, uchar);
extern void vsvc_send_returnaddress(vinesipcsocket *, vsvc_info *);
extern void vsvc_send_other(vinesaddress *, vsvc_info *, vsvc_rtn_info *, uchar *);
extern vsvc_rtn_info *vsvc_find_routine(vsvc_info *, ushort);
extern void vsvc_oops_buginf(char *, char *, char *);
extern char *vsvc_print_nonce(vinesnonce *nonce);
extern void vsvc_break_nonce(vinesnonce *, ulong *, ushort *);
#ifdef NOBODY_CALLS_ME
extern void vsvc_break_port(vinesaddress *, ulong *, ushort *, ushort *);
#endif /* NOBODY_CALLS_ME */
extern vinesaddress *vsvc_create_address(vinesaddress *, ushort);
extern vinesaddress *vsvc_socket_to_address(vinesaddress *, vinesipcsocket *);
extern ushort vsvc_validate_nonce(vsvc_info *, vinesnonce *);
extern void vsvc_report_error(vinesipcsocket *);
extern void *vsvc_unpack_other(vinesipcsocket *, vsvc_info *, vsvc_rtn_info *, uchar *, int);
extern vsvc_info *vsvc_find_service(ushort port);
extern boolean vsvc_dispatch_call(vinesipcsocket *, vsvc_info *, vrpc_call *, uchar *, int);
extern void vsvc_dispatch_reject(vinesipcsocket *, vsvc_info *, ushort);
extern void vsvc_dispatch_abort(vinesipcsocket *, vsvc_info *, ushort);
extern boolean vsvc_dispatch_return(vinesipcsocket *, vsvc_info *, uchar *, int);
extern boolean vsvc_dispatch_search(vinesipcsocket *, vsvc_info *, ushort, char *, char *, char *, char *); 
extern void vsvc_dispatch_returnaddress(vinesipcsocket *, vsvc_info *, ushort, ulong, vinesaddress *);
extern char *vsvc_sprintf_version(char *, ulong);
extern char *vsvc_sprintf_address(char *, vinesaddress *);
extern void vsvc_dump (vsvc_info *, char *, uchar *, int);
extern void vsvc_hostname_changed(boolean);
extern ushort vsvc_break_name(char *, char *, char *, char *);
extern void vsvc_create_nonce(vinesnonce *);
extern inline ushort vsvc_c2b_iftype(vinesidbtype *);
extern inline ushort vsvc_c2b_wantype(vinesidbtype *);

extern void vfs_init(void);
extern void vnsm_init(void);
extern void vss_init(void);
extern void vst_init(void);
extern void vvs_init(void);
extern void vst_new_neighbor(ulong, vinesidbtype *);
extern void vvs_new_neighbor(ulong, vinesidbtype *);
extern void vst_display_group_tree(void);
extern void vst_hostname_changed(void);
extern char *vst_find_server_by_address(ulong);
extern ulong vst_find_server_by_name(char *);
extern void vnsm_report_error(vinesipcsocket *);
