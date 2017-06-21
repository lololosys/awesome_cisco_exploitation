/* $Id: dirresp.h,v 3.1.4.1 1996/08/15 06:49:46 raj Exp $
 * $Source: /release/112/cvs/Xsys/servers/dirresp.h,v $
 *------------------------------------------------------------------
 * Director-Responder protocol definitions
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dirresp.h,v $
 * Revision 3.1.4.1  1996/08/15  06:49:46  raj
 * CSCdi61197:  Part of  CSCdi49395 patches missing from 11.2
 * Branch: California_branch
 * Update Director Responder Agent to the latest version.
 *
 * Revision 3.1  1996/03/19  00:20:58  pst
 * CSCdi49395:  sync director responder code to current working base
 *              don't pass down ip->ident parameter in ip_udp_send
 *              (radius.c)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef	__DIRRESP_H__
#define	__DIRRESP_H__
/*
 * Protocol versions supported
 */
#define	DRP_MIN_VER	1
#define	DRP_MAX_VER	1
#define	DRP_VERSION	1

/*
 * Format of DRP_OP_FAILURE entries
 */
typedef struct drfailtype_ {
    uchar	mandatory : 1;		/* generic part */
    uchar	op	  : 7;
    uchar	len;

    uchar	reason;			/* failure reason */
    uchar	reserved;		/* must be zero */
    uchar	info[8];		/* failure info */
} drfailtype;

/* failure reason */
#define	DRP_FAIL_NOFAIL  0		/* no failure */
#define	DRP_FAIL_FORMAT  1		/* badly formatted message */
#define	DRP_FAIL_VERSION 2		/* unsupported version */
#define	DRP_FAIL_COMMAND 3		/* invalid command */
#define	DRP_FAIL_FAMILY  4		/* unsupported address family */
#define	DRP_FAIL_AUTH	 5		/* authentication failure */

/*
 * Format of DRP_OP_AUTH entries
 */
#define	DRP_AUTH_DIGEST_LEN	16
typedef struct drauthtype_ {
    uchar	mandatory : 1;		/* generic part */
    uchar	op	  : 7;
    uchar	len;

    uchar	mode;			/* authentication method used */
    uchar	key_id;
    uchar	info[DRP_AUTH_DIGEST_LEN];
} drauthtype;

/* authentication modes */
#define	DRP_AUTH_KMD5	0		/* keyed MD5 */

/*
 * Universal "bad metric"
 */
#define DRP_UNREACHABLE METRIC_INACCESSIBLE

/*
 * Format of IPv4 query entries
 */
typedef struct drquerytype_ {
    uchar	mandatory : 1;		/* generic part */
    uchar	op	  : 7;
    uchar	len;
    ushort	family;			/* address family */
    uchar	data[0];		/* ...family specific */
} drquerytype;

#define	DRP_FAM_UNKNOWN	0		/* unspecified address family */
#define DRP_FAM_IPV4	0x0800		/* IP version 4 */

typedef struct dripv4qtype_ {
    ipaddrtype	address;		/* first address... */
    ulong	int_metric;		/* interior metric... */
    ulong	ext_metric;		/* exterior metric... */
    uchar	int_type;		/* interior metric type */
    uchar	ext_type;		/* exterior metric type */
    uchar	pad1;
    uchar	pad2;
} dripv4qtype;

enum drmettype {
    DRP_METTYPE_UNKNOWN =	0,	/* unknown metric type */
    DRP_METTYPE_IGRP,			/* IGRP composite metric */
    DRP_METTYPE_HOPCOUNT,		/* OSPF? I-ISIS? RIP? all same? */
    DRP_METTYPE_ASHOPCOUNT,		/* BGP autonomous systems */
    DRP_METTYPE_ISIS,                   /* ISIS metric */
    DRP_METTYPE_OSPF,                   /* OSPF metric */
    DRP_METTYPE_RIP,                    /* RIP metric */
    DRP_METTYPE_IGRP2,                  /* EIGRP composite metric */
    DRP_METTYPE_STATIC,                 /* Static metric */
    DRP_METTYPE_CONNECTED,              /* Directly connected metric 0 */
    DRP_METTYPE_MOBILE,                 /* Mobile metric */
    DRP_METTYPE_BGP,                    /* BGP autonomous systems */
    DRP_METTYPE_MAX
};

/*
 * Format of message entry part, generic template.
 */
typedef struct drentrytype_ {
    uchar	mandatory : 1;		/* must be same for all messages */
    uchar	op	  : 7;
    uchar	len;
    ushort	pad;

    uchar	data[0];		/* generic data... */
} drentrytype;

/*
 * Valid message opcodes
 */
#define	DRP_OP_FAILURE	0
#define	DRP_OP_AUTH	1
#define	DRP_OP_RTQUERY	2
#define	DRP_OP_RTRESP	3

/*
 * Director message format
 * This is just a collection of TLV operands
 */
typedef struct drtype_ {
    ushort	length;
    uchar	version;
    uchar	flags;
    drentrytype	entry[0];
} drtype;

extern boolean dirresp_append_auth(drtype *msg, int querylength, int id);
extern int dirresp_op_authenticate (drauthtype *entry, char *data, int length,
				    boolean *authenticated);
extern void dirresp_send_failure (paktype *old_pak, void *data, int reason);

extern int dirresp_acl;

extern char *dirresp_keychain;

#ifdef	DEBUG
extern void dirresp_print_drp(drtype *msg);
#endif

#define DRP_FLAG_AUTH	1

#endif	__DIRRESP_H__
