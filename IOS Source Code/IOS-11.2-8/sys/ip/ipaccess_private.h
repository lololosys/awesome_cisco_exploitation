/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * ipaccess_private.h -- Internal IP Access List Definitions
 *
 * September 1996, Greg Christy
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Address and wildcard bits for extended access lists.
 */

#define ANY_HOST	0		/* 0.0.0.0 */
#define WILD_HOST     	0		/* 0.0.0.0 */
#define WILD_ANY    	((ipaddrtype) -1) /* 255.255.255.255 */

/*
 * Data structures for standard IP access lists
 */
typedef enum {
    IP_ACCESS_LOG_NONE = 0,
    IP_ACCESS_LOG_NORMAL,
    IP_ACCESS_LOG_INPUT
} ipacllogtype;


typedef struct fasthash_ {
	struct fasthash_ *next;		/* pointer to next item in chain */
	ipaddrtype address;		/* address to match exactly */
	boolean grant;			/* TRUE if permit, FALSE if deny */
} fasthash;

typedef struct fastitem_ {
        struct fastitem_ *next;		/* link to next entry */
	ipaddrtype permitmask;		/* IP address mask */
	ipaddrtype wildmask;		/* mask of wildcarded address bits */
	boolean grant;			/* TRUE if permit, FALSE if deny */
} fastitem;

typedef struct fasttype_ {
	fasthash *hashlist[NETHASHLEN]; /* list of non-wild items */
	fastitem *items;
} fasttype;

/*
 * Data structures for extended IP access lists
 */

typedef struct slowtype_ {
        struct slowtype_ *next;	/* link to next entry */
	ipaddrtype source;	/* source IP address */
	ipaddrtype srcmask;	/* mask of wildcarded address bits */
	ipaddrtype destination;	/* destination IP address */
	ipaddrtype dstmask;	/* mask of wildcarded address bits */
	ulong hits;		/* number of times ACE has matched */
	ulong si_hits;		/* number of times ACE matched at last clear */
	boolean grant;		/* TRUE if permit, FALSE if deny */
	boolean tos;		/* TRUE if the tos value is set */
	boolean precedence;	/* TRUE if the precedence value is set */
	boolean established;	/* TRUE if filtering on established */
	boolean fancy;		/* TRUE if we need to do more than usual */
	boolean	log;		/* TRUE if we need to log this ACE's use */
	ushort protocol;	/* protocol type we are filtering */
	ushort srcopr;		/* operator name */
	ushort sport;		/* [TCP, UDP] source port */
	ushort sport2;		/* [TCP, UDP] upper port in range */
	ushort dstopr;		/* operator name */
	ushort dport;		/* [TCP, UDP] destination port */
				/* ICMP message type */
	ushort dport2;		/* [TCP, UDP] upper port in range */
	uchar tos_value;	/* TOS value to filter on */
	uchar precedence_value;	/* matching precedence value */
	boolean dynamic;	/* TRUE if Dynamic/temporary entries */
	struct dynamiclist_type_ *dynacl_p; /* Ptr. to dynamic list */
	uint timeout;		/* Dynamic IDLE timer flag */
	acl_headertype *acl;	/* List we belong in */
	ulong sav_hits;		/* # of hits since last idle check */
    	mgd_timer dynacl_timer;	/* Idle time limit */
	mgd_timer dynmax_timer; /* Absolute time limit */
	sys_timestamp lastref;	/* time of last ref if dynamic list */
} slowtype;

typedef struct logcache_entry_ {
	idbtype *input;		/* input interface */
	ipaddrtype source;	/* source address */
	ipaddrtype destination;	/* destination address */
	ushort sport;		/* source port */
	ushort dport;		/* destination port */
	acl_headertype *acl; 	/* pointer to list */
	uchar protocol;		/* IP protocol type (could be uchar) */
	uchar dummy[1];		/* padding */
	unsigned age;		/* age of this entry (in packets) */
	unsigned count;		/* count of hits */
	sys_timestamp logtime;	/* last time logged to system */
	boolean grant;		/* did we pass this packet? */
	hwaddrtype src_snpa;	/* source mac addr */
} logcache_entry;


/*
 * Nested ACL structure
 */
#define	ACLNAME_LEN		64	/* Max. name length of include ACL */
typedef struct	dynamiclist_type_ {
	struct dynamiclist_type_ *next; /* Next temp. list */
	char aclname[ACLNAME_LEN]; /* Name of this Access-list */
	uint refcnt;		/* Reference count */
	uint timeout;		/* Maximum time until temp expiration */
	slowtype *item;		/* Dynamic Items. (Not NVGENed) */
} dynlisttype;

#define	LOGCACHESIZE	16		/* possible entries */
#define	LOGPRINTDELAY	(5*ONEMIN)	/* maximum reporting interval */


/*
 * Protocol codes.
 * For IP, we use the 8-bit protocol type field to indicate a specific
 * protocol.  9-bit quantities indicate other protocol types/families.
 * IP_PROT is understood to mean all IP protocols.
 */
#define IP_PROT		0x100

