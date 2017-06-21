/* $Id: domain_private.h,v 3.1 1995/11/29 22:09:57 pst Exp $
 * $Source: /swtal/cherf.111/ios/sys/servers/domain_private.h,v $
 *------------------------------------------------------------------
 * domain_private.h -- domain service definitions.  RFC 882, 883, etc.
 *
 * 27-Oct-1985, Kirk Lougheed
 * 29-Nov-1995, Paul Traina
 *
 * Copyright (c) 1985-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: domain_private.h,v $
 * Revision 3.1  1995/11/29  22:09:57  pst
 * CSCdi44659:  clean up internals of domain code (make an interface API)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __DOMAIN_PRIVATE_H__
#define __DOMAIN_PRIVATE_H__

#define INDIRECT_MASK 0xC0	/* for handling compressed domain names */ 

/*
 * Header format
 */

#define domaintype struct domaintype_

domaintype {
    ushort id;			/* request id */
#ifdef BIGENDIAN
    unsigned qr:1;		/* 0 if query, 1 if response */
    unsigned opcode:4;		/* opcode (see below) */
    unsigned aa:1;		/* 1 if authoritative answer */
    unsigned tc:1;		/* 1 if message truncated */
    unsigned rd:1;		/* 1 if recursion desired */
    unsigned ra:1;		/* 1 if recursion available */
    unsigned reserved:3;	/* not defined */
    unsigned rcode:4;		/* response code (see below) */
#endif
#ifdef LITTLEENDIAN
    unsigned char rd:1;		/* 1 if recursion desired */
    unsigned char tc:1;		/* 1 if message truncated */
    unsigned char aa:1;		/* 1 if authoritative answer */
    unsigned char opcode:4;	/* opcode (see below) */
    unsigned char qr:1;		/* 0 if query, 1 if response */

    unsigned char rcode:4;	/* response code (see below) */
    unsigned char reserved:3;	/* not defined */
    unsigned char ra:1;		/* 1 if recursion available */
#endif
    ushort qdcount;		/* count of questions */
    ushort ancount;		/* count of answers */
    ushort nscount;		/* count of authority records */
    ushort arcount;		/* count of additional records */
};

/*
 * opcode field values 
 */

#define OPCODE_QUERY 0		/* standard query */
#define OPCODE_IQUERY 1		/* inverse query */

/*
 * response code field values
 */

#define RCODE_OKAY 0		/* no errors */
#define RCODE_FORMAT 1		/* format error */
#define RCODE_SERVER 2		/* server error */
#define RCODE_NAME 3		/* no such name -- AA must be set! */
#define RCODE_NYI 4		/* not implemented */
#define RCODE_REFUSED 5		/* request refused */

/*
 * Notes on question fields
 *
 *  QNAME - variable length - name string in compressed format
 *  QTYPE - 2 octets - type of query (see below for opcodes)
 *  QCLASS - 2 octets - class of query (e.g. internet, csnet) (see below)
 */

#define QUERYFIXEDBYTES 4		/* always at least four bytes */

/*
 * Notes on resource records (RR's)
 *
 *  NAME - variable length - name string in compressed format
 *  TYPE - 2 octets - type of RR (see below)
 *  CLASS - 2 octets - class of data in RR (see below)
 *  TTL - 4 octets - time in seconds to expire data
 *  RDLENGTH - 2 octets - octets in RDATA field
 *  RDATA - variable - data for RR
 */

/*
 *    TYPE fields are used in resource records.  Note that these types
 *    are not the same as the QTYPE fields used in queries, although the
 *    functions are often similar.
 */

#define TYPE_A      1  /* a host address */
#define TYPE_NS     2  /* an authoritative name server */
#define TYPE_MD     3  /* a mail destination */
#define TYPE_MF     4  /* a mail forwarder */
#define TYPE_CNAME  5  /* the canonical name for an alias */
#define TYPE_SOA    6  /* marks the start of a zone of authority */
#define TYPE_MB     7  /* a mailbox domain name */
#define TYPE_MG     8  /* a mail group member */
#define TYPE_MR     9  /* a mail rename domain name */
#define TYPE_NULL  10  /* a null RR */
#define TYPE_WKS   11  /* a well known service description */
#define TYPE_PTR   12  /* a domain name pointer */
#define TYPE_HINFO 13  /* host information */
#define TYPE_MINFO 14  /* mailbox or mail list information */
#define	TYPE_NSAP  22  /* RFC1348 and successor NSAP entry */
#define	TYPE_NSAP_PTR 23	/* RFC1348 CLNS version of PTR */
#define TYPE_NB    32	/* RFC1002 NetBIOS name service */
#define TYPE_NBSTAT 33	/* RFC1002 NetBIOS node status */
#define	TYPE_ANY   255	/* any record matching this object */

 /*
  *   QTYPE fields appear in the question part of a query.  They include
  *   the values of TYPE with the following additions:
  */

#define TYPE_AXFR  252 /* request for a transfer of zone of authority */
#define TYPE_MAILB 253 /* request for mailbox-related records (MB, MG or MR)*/
#define TYPE_MAILA 254 /* request for mail agent RRs (MD and MF) */
#define TYPE_WILD  255 /* request for all records */

 /*
  *   CLASS fields appear in resource records
  */

#define CLASS_IN 1 		 /* the ARPA Internet */
#define CLASS_CS 2		 /* the computer science network (CSNET) */

  /*
   *  QCLASS fields appear in the question section of a query.  They
   *  include the values of CLASS with the following additions:
   */

#define CLASS_WILD 255		 /* any class */

#endif __DOMAIN_H__
