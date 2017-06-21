/* $Id: name.h,v 3.2.60.1 1996/04/15 21:13:34 bcole Exp $
 * $Source: /release/112/cvs/Xsys/h/name.h,v $
 *------------------------------------------------------------------
 * name.h -- definitions for host name and address lookup routines
 *
 * 8-December-1985, Kirk Lougheed
 *
 * Copyright (c) 1985-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: name.h,v $
 * Revision 3.2.60.1  1996/04/15  21:13:34  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:13:03  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/23  21:49:22  vandys
 * CSCdi41005:  ip host command writes port number above 32767 as negative
 * to nvram
 * Fix short/ushort handling
 *
 * Revision 2.1  1995/06/07  20:36:05  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __NAME_H__
#define __NAME_H__

#include "address.h"
/*
 * A few global parameters
 */

#define MAXDOMAINSERVERS 6	/* the domain servers */
#define MAXIPADDRESSES 8	/* up to eight addresses per name */
#define MAX121ADDRESSES 2	/* up to two X.121 addresses per name */
#define MAXCLNSADDRESSES 8      /* up to eight CLNS addresses per name */
#define MAXVINESADDRESSES 1	/* one address per name */

#define NAM_UNK_CACHETIMEOUT (3 * ONESEC) /* Cache timeout is 3 seconds */

/*
 *  IP cache variant structure.
 */
typedef struct ipcachevariant_ {
    addrtype adr[MAXIPADDRESSES];
    ushort port;
} ipcachevariant;

/*
 * X121 cache variant structure.
 */
typedef struct padcachevariant_ {
    addrtype adr[MAX121ADDRESSES];
    uchar *facilities;		/* NULL if none */
    uchar *userdata;
} padcachevariant;

/*
 * CLNS cache variant structure.
 *  ??? should there be more content in this structure ???
 */
typedef struct clnscachevariant_ {
    addrtype adr[MAXCLNSADDRESSES];
} clnscachevariant;

/*
 * DECnet cache variant structure.
 */
typedef struct decnetcachevariant_ {
    addrtype adr;
} decnetcachevariant;

/*
 * DEC LAT cache variant structure
 * Note that we essentially have our own copy of an advertised lat SERVICE,
 * which might resolve into multiple hosts down in the depths of LAT.
 */
typedef struct latcachevariant_ {
    addrtype adr;
} latcachevariant;

/*
 *  Vines IP cache variant structure.
 */
typedef struct vinescachevariant_ {
    addrtype adr[MAXVINESADDRESSES];
} vinescachevariant;

/*
 * IPX cache variant structure.
 */
typedef struct ipxcachevariant_ {
    addrtype adr;
} ipxcachevariant;

/*
 *  Name cache data structure
 */

struct nametype_ {
    nametype *next;		/* pointer to next cache entry */
    int sharecount;		/* people using it? */
    sys_timestamp lasttime;	/* msclock of last use */
    sys_timestamp expiretime;	/* When the name expires */
    char *busystring;		/* pointer to possible "host busy" message */
    char *loginstring;		/* pointer to possible auto login sequence */
    char *resumestring;		/* pointer to possible auto resume sequence */
    int flags;                  /* status flags */
    union {
	ipcachevariant ip;       /* unique to TCP/IP */
	padcachevariant x121;    /* unique to PAD (x.29) */
	clnscachevariant clns;   /* unique to CLNS */
	decnetcachevariant decnet; /* unique to DECnet */
	latcachevariant lat;	 /* unique to LAT */
	vinescachevariant vines; /* unique to Banyan Vines */
	ipxcachevariant ipx;	/* unique to IPX */
    } v;
    int namecount;		/* number of name strings */
    char name[1];		/* start of name string */
};

/*
 * Flag bits per cache entry
 */

#define NAM_PERMANENT   0x1	/* never delete this entry from cache */
#define NAM_REVALIDATE  0x2	/* entry must be revalidated if possible */
#define NAM_IP          0x4	/* IP type address */
#define NAM_PAD         0x8     /* X.121 type address -- for PAD use */
#define NAM_CLNS	0x10	/* CLNS type address */
#define NAM_LAT		0x20	/* LAT type address (name, actually) */
#define NAM_RLOGIN	0x40	/* Use RLogin to connect here */
#define NAM_ADDRVALID	0x80	/* The addresses are valid. */
#define NAM_VINES	0x100	/* Vines type address */
#define NAM_HP		0x200   /* HP type name */
#define NAM_DECNET	0x400	/* DECnet type address */
#define NAM_UNKNOWN	0x800	/* Cached name lookup failure */
#define NAM_IPX		0x1000	/* IPX name (for NLSP) */

/* match on any/all name types */

#define NAM_ANY  (NAM_IP + NAM_PAD + NAM_CLNS + NAM_LAT + NAM_VINES + \
		  NAM_HP + NAM_DECNET + NAM_IPX)

#define NAM_USES_DOMAIN (NAM_IP + NAM_CLNS)

typedef struct domainlisttype_ {
    struct domainlisttype_ *next;
    char name[1];
} domainlisttype;

/*
 * Prototypes and Externs
 */
extern queuetype nameQ;  	/* name cache queue */
extern char *default_domain;	/* default domain name */
extern queuetype domain_listQ;	/* names to use when resolving */
extern ipaddrtype domain_server[]; /* address of the domain server */

extern boolean name_verify(char *);
extern nametype *name_lookup(char *, int *, int);
extern nametype *name_cache_lookup(char *, addrtype *, int);
extern void name_valid(nametype *, int);
extern void name_show(parseinfo *);
extern void name_clear_host(parseinfo *);
extern void name_delete(char *, int);
extern void name_free(nametype *);
extern void name_init(void);
extern char *name_addr2numeric(addrtype *);
extern char *name_addr2string(addrtype *);
extern void name_update_entry (nametype *, nametype *);

#endif __NAME_H__
