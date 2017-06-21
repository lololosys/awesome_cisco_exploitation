/* $Id: access.h,v 3.5.6.7 1996/08/30 01:49:45 gchristy Exp $
 * $Source: /release/112/cvs/Xsys/h/access.h,v $
 *------------------------------------------------------------------
 * access.h -- access control definitions
 *
 * 4-February-1987, Kirk Lougheed
 *
 * Copyright (c) 1987-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: access.h,v $
 * Revision 3.5.6.7  1996/08/30  01:49:45  gchristy
 * CSCdi67644:  IP NACL: inform user when access-list type is invalid
 * Branch: California_branch
 * - Complain if an existing list is an invalid type.
 *
 * Revision 3.5.6.6  1996/08/14  02:36:18  hampton
 * Use static inline routines to clean up references to the access list
 * array.  [CSCdi65925]
 * Branch: California_branch
 *
 * Revision 3.5.6.5  1996/06/03  23:22:49  ahh
 * CSCdi59119:  IP: NAT should support named access-lists
 * And extended access-lists and non-contiguous address pools.
 * Branch: California_branch
 *
 * Revision 3.5.6.4  1996/05/08  01:09:28  gchristy
 * CSCdi56944:  Compilation errors from IP named access list commit
 * Branch: California_branch
 * - Modularize new ACL code for inclusion in VIP images.
 *
 * Revision 3.5.6.3  1996/05/04  00:42:06  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - Rip out MTR transparent bridging support.
 * - New interafce type: BVI(Bridge-group Virtual Interface).
 * - New fast-switching flavor: FS_BVI.
 * - Introduce idb->tbridge_bridge_linktypes, idb->user_configured_mtu and
 *   pak->irb_rcvd_input.
 *
 * Constrained Multicast Flooding
 * - Declare tbifd_type.
 * - Define the CMF parser link points.
 * - Declare the return_one stub function.
 * - Declare the Transparent Bridging registry.
 * - Add tbridge_cmf to the fastswitch registry.
 *
 * Name Access List
 *
 * Revision 3.5.6.2  1996/05/03  01:51:51  sluong
 * Branch: California_branch
 * IPX ACL Violation Logging, Plaining English Filters, Display SAP by
 * name,
 * NLSP MIBS, Scaleable RIP/SAP, Modularity, and SAP query by name.
 *
 * Revision 3.5.6.1  1996/04/04  15:41:16  widmer
 * CSCdi53503:  long regexp in ip as-path access-list cmd crashes router
 * Branch: California_branch
 * Malloc string instead of using static size.
 *
 * Revision 3.5  1996/03/01  07:33:06  mschaef
 * CSCdi50473:  NLSP route aggregation conformance (phase 1)
 *
 * Fix assorted nits and bugs
 *
 * Revision 3.4  1996/02/24  01:55:09  pst
 * CSCdi49774:  modularize regexp-acls for bgp a bit (part 1)
 *
 * Revision 3.3  1995/11/17  09:11:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:23:49  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:34:40  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:58:21  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/18  06:45:49  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.1  1995/06/07 20:34:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Note:  protocol specific data structures should be moved to protocol
 * modules.  This has already been done with IP.  The definitions here
 * should be generic, not protocol specific.
 */

#ifndef __ACCESS_H__
#define __ACCESS_H__

#define MINFASTACC 1		/* Standard IP lists */
#define MAXFASTACC 99
#define MINSLOWACC 100		/* extended IP lists */
#define MAXSLOWACC 199
#define MINTYPEACC 200		/* Ethernet type lists */
#define MAXTYPEACC 299
#define MINDECACC  300		/* DECnet lists */
#define MAXDECACC  399
#define	MINXNSACC  400		/* XNS lists */
#define	MAXXNSACC  499
#define	MINEXNSACC 500		/* extended XNS lists */
#define	MAXEXNSACC 599
#define	MINATALKACC 600		/* AppleTalk lists */
#define	MAXATALKACC 699
#define MINMACADDRACC 700	/* MAC level address lists */
#define MAXMACADDRACC 799
#define	MINNOVACC  800		/* Novell lists */
#define	MAXNOVACC  899
#define	MINENOVACC 900		/* extended Novell lists */
#define	MAXENOVACC 999
#define	MINNOVSAPACC 1000	/* Novell server advertisement lists */
#define	MAXNOVSAPACC 1099
#define	MINXMACACC   1100	/* extended Bridging lists */
#define	MAXXMACACC   1199
#define MINNOVSUM    1200       /* Novell summary lists */
#define MAXNOVSUM    1299

#define MAXACCESSGROUP MAXNOVSUM

#define	MINIDLEACC	1	/* Minimum IDLE timeout */
#define	MAXIDLEACC	9999	/* Maximum IDLE timeout */

#define ACCESSSIZE (MAXACCESSGROUP+1)

/*
 * Regular expression based access lists are the first to use their own
 * little table of lists (currently configured via "x25 access-list" command).
 * Bad things will happen if MINREGEXPACC is not 0.
 */

#define MINREGEXPACC 0
#define MAXREGEXPACC 199

#define LIST_INCREMENT   100

#define LIST_IP1     0
#define LIST_IP2     1
#define LIST_TYPE    2
#define LIST_DECNET  3
#define LIST_XNS1    4
#define LIST_XNS2    5
#define LIST_APPLE   6
#define LIST_MAC     7
#define LIST_NOVELL1 8
#define LIST_NOVELL2 9
#define LIST_NOVELL3 10
#define LIST_NOVELL4 11
#define	LIST_XMAC    12

/*
 * Ethernet type access lists
 */

#define MAXTYPEITEMS	20

typedef struct typeitem_ {
	ushort value;
	ushort mask;
	boolean grant;
} typeitem;


typedef struct typetype_ {
	int items;			/* total used items in list */
	typeitem item[MAXTYPEITEMS];	/* array of list items */
} typetype;

/*
 * DECnet access lists
 */

typedef struct dnacctype_ {
    struct dnaccitemtype_ *normal;	/* list for normal packets */
    struct dnaccitemtype_ *connect;	/* list for connect packets */
} dnacctype;

typedef struct dnaccitemtype_ {
    struct dnaccitemtype_ *next; 
    boolean grant;
    boolean second;		 /* TRUE if destination addresses are valid */
    ushort source;
    ushort sourcemask;
    ushort dest;
    ushort destmask;
    ushort flags;			/* DNO flags */
    /* the rest of the items are only used for connect packets */
    ushort sgrp;			/* src uic */
    ushort susr;
    ushort dgrp;			/* dst uic */
    ushort dusr;
    uchar son;				/* source object number */
    uchar don;				/* dest object number */
    uchar srcop;			/* src object number operator */
    uchar dstop;			/* dest object number operator */
    char *soe;				/* source object expression */
    regexp *sopattern;			/* source object regex pattern */
    char *doe;				/* dest object expression */
    regexp *dopattern;			/* dest object regex pattern */
    char *id;				/* id expression */
    regexp *idpattern;			/* id regex pattern */
    char *pw;				/* password expression */
    regexp *pwpattern;			/* password regex pattern */
    char *ac;				/* account expression */
    regexp *acpattern;			/* account regex pattern */
} dnaccitemtype;

/*
 * Flags for DECnet object access lists
 */
#define DNO_EQ   0x0001			/* EQ|NE was EQ */
#define DNO_SON  0x0002			/* source object number present */
#define DNO_SOE  0x0004			/* source object expression present */
#define DNO_DON  0x0008			/* dest object number present */
#define DNO_DOE  0x0010			/* dest object expression present */
#define DNO_ID   0x0020			/* ID present */
#define DNO_PW   0x0040			/* password present */
#define DNO_AC   0x0080			/* account present */
#define DNO_SUIC 0x0100			/* source uic present */
#define DNO_DUIC 0x0200			/* dest uic present */
#define DNO_ANY  0x0400			/* any connect */


typedef struct mac_address_type_ {
    union {
	ushort sword[4];
	ulong lword[2];
	uchar byte[8];
    } u;
} mac_address_type;

/*
 * Take mac_address_types and convert them to uchar pointers
 */
#define MAC_ADDRESS_TYPE_TO_UCHAR(x) ((uchar *) &((x).u.byte[0]))
#define MAC_ADDRESS_TYPE_PTR_TO_UCHAR(x) ((uchar *) &((x)->u.byte[0]))

/*
 * XNS/IPX normal access lists
 */
#define XNS_SRCNET      0x0001
#define XNS_SRCHOST     0x0002	
#define XNS_SRCHOSTMASK 0x0004
#define XNS_DSTNET      0x0008
#define XNS_DSTHOST     0x0010
#define XNS_DSTHOSTMASK 0x0020
/*
 * XNS/IPX extended access lists
 */
#define XNS_PROTOCOL    0x0040
#define XNS_SRCSOCKET   0x0080
#define XNS_DSTSOCKET   0x0100
#define XNS_SERVER_TYPE 0x0200
#define XNS_PEPCLIENT   0x0400
#define XNS_SERVER_NAME 0x0800
#define XNS_SRCNETMASK  0x1000
#define XNS_DSTNETMASK  0x2000

/*
 * Common data for all types of XNS/IPX access lists. Must be first element
 * (after 'next') in all XNS/IPX access list structures.
 */
#define XNS_ACCESS_COMMON \
	boolean grant; \
	ulong flags; \
	ulong srcnet; \
	ulong srcnetmask; \
	mac_address_type srchost; \
	mac_address_type srchostmask; \
	ulong dstnet; \
	ulong dstnetmask; \
	mac_address_type dsthost; \
	mac_address_type dsthostmask;


/*
 * XNS/IPX simple access structure.
 */
typedef struct xnsnacctype_ {
        struct xnsnacctype_ *next;    
	XNS_ACCESS_COMMON
} xnsnacctype;

/*
 * Summary address data structure. 
 */
typedef struct nov_sumlisttype_ {
    struct     nov_sumlisttype_ *next;
    boolean    grant;          /* permit or deny */
    ulong      net;            /* summary network */
    ulong      mask;           /* mask for net */
    idbtype    *idb;           /* idb qualifier, if any */
    ulong      match_count;    /* number of routes which match summary */
    short      ticks;
    uchar      area_count;
} nov_sumlisttype;

/*
 * XNS/IPX extended access structure.
 */
typedef struct xnseacctype_ {
        struct xnseacctype_ *next;
	XNS_ACCESS_COMMON
/***** Add fields *unique* to this type below  *****/
	ushort pep_clienttype;
        ushort protocol;
	ushort srcsocket;
        ushort dstsocket;
	char server_name[48];
	boolean log;
} xnseacctype;

/*
 * MAC-level address access lists -- used in bridging operation, 
 * to filter incoming and outgoing frames.
 */


typedef struct macaddracctype_ {
        struct macaddracctype_ *next;
	mac_address_type addr;
	mac_address_type mask;
	boolean grant;
} macaddracctype;



/*
 * regular expression based access lists - these can be used by anything
 * that has text like values (eg X.25 addresses, Novell SAP types, and so on.
 * You only get one regular expression per access list, though.  I can't
 * think of anything offhand that requires more.
 */

struct regexpacctype_ {
    struct regexpacctype_ *next;
    char *pattern;
    regexp *comppattern;
    boolean grant;
};

/*
 * The access list array data structure.
 * Allows us to have different access list types in one structure.
 */

typedef struct accessptr_ {
	union {
	    typetype *type;
	    dnacctype *dn;
	    xnsnacctype *xnsn;
	    xnseacctype *xnse;
	    macaddracctype *mac;
	    regexpacctype *regexp;
	} type;
} accessptr;

extern accessptr *accessarray[];

static inline typetype *type_get_acl (int number)
{
    return((typetype *)accessarray[number]);
}

static inline void type_set_acl (int number, typetype *acl)
{
    accessarray[number] = (accessptr *)acl;
}

static inline macaddracctype *mac_get_std_acl (int number)
{
    return((macaddracctype *)accessarray[number]);
}

static inline void mac_set_std_acl (int number, macaddracctype *acl)
{
    accessarray[number] = (accessptr *)acl;
}

/*
 * Set whenever an access-list entry is parsed, for any reason for the list
 * given. Code can do with this as it will, but it would probably be a good
 * idea (read: you SHOULD do this) to have it read and cleared promptly by
 * some background process.
 */
/* An access list entry was changed */
extern sys_timestamp access_entry_changed_time;
extern sys_timestamp access_changed_times[];

/* A structure for callback functions to call if an access list is changed */
typedef void (*access_callbackfunction)(ulong);
typedef struct _access_callback {
    struct _access_callback *next;
    ulong low;
    ulong high;
    access_callbackfunction callback_function;
} access_callback;

typedef enum {
    ACL_UNDEFINED,
    ACL_IP_SIMPLE,
    ACL_IP_EXTENDED,
    ACL_IP_NAMED_SIMPLE,
    ACL_IP_NAMED_EXTENDED
} acltype;

#define MAXNACLS 2000
#define MAXACLNAME 255

/*
 * This structure is used to hold general information about a list.
 * A pointer to this structure is maintained in the idbs which are
 * configured to use access lists. This was done to speed up the list
 * lookup during fast-switching.
 */

struct acl_headertype_ {
    char *name;			/* Name */
    acltype type;		/* ACL type */
    boolean tp_header_required;  /* Does this list require access to
				   the transport header? */
    accessptr *list;		/* Entry list */
};

extern acl_headertype *acl_array[];
extern list_header nacl_list;
/*
 * Returns true if NACL name begins with an alphabetic character.
 */

extern boolean is_valid_nacl(const char *);

extern const char nacl_name_error[];

/*
 * Prototypes and stuff
 */

/*
 * regexp_access.c
 */
extern regexpacctype *x29_accesslists[];
extern regexpacctype *lat_accesslists[];

extern boolean regexp_accesscheck(uchar *, int, regexpacctype *[]);
extern void regexpaccess_init(void);
extern void show_regexpaccess(int);
extern void show_regexpacl(regexpacctype **, int, char *);
extern int regexpaccess_command(parseinfo *, regexpacctype *[]);

/*
 * access.c
 */
extern void access_init(void);
extern const char *access_type2string(acltype);
extern void access_print_name (acl_headertype *);
extern access_check_type(ulong, ushort);
extern access_check_mac(int, uchar *);
extern boolean access_create_callback(access_callbackfunction, ulong, ulong);
extern boolean access_destroy_callback(ulong, ulong);
extern void access_mark_list_changed(ulong);

/*
 * access_chain.c
 */
extern void access_parser_init(void);


/*
 * access_common.c
 */
extern void acl_init(void);
extern acl_headertype *access_find_or_create_acl(ulong);
extern void access_delete_acl(ulong);
extern acl_headertype *access_find_acl(ulong);
extern acl_headertype *access_find_or_create_nacl(const char *, acltype); 
extern acl_headertype *access_create_nacl(const char *, acltype);
extern acl_headertype *access_find_nacl(const char *);
extern void access_delete_nacl(acl_headertype *);
extern acl_headertype *access_parse_acl(int, char *, boolean, acltype);

#endif __ACCESS_H__
