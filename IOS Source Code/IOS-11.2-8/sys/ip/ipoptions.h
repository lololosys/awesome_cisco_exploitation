/* $Id: ipoptions.h,v 3.2.60.2 1996/07/12 20:09:17 widmer Exp $
 * $Source: /release/112/cvs/Xsys/ip/ipoptions.h,v $
 *------------------------------------------------------------------
 * ipoptions.h -- defines for IP level option processing
 *
 * July 1987,   By Bill Westfield
 * July 1988,      Eric B. Decker
 * April 1989,     David Hampton
 *
 * Copyright (c) 1987-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ipoptions.h,v $
 * Revision 3.2.60.2  1996/07/12  20:09:17  widmer
 * CSCdi62775:  Bogus externs in telnet.c cause 4k builds to fail
 * Branch: California_branch
 * Remove externs and move externs to header files
 *
 * Revision 3.2.60.1  1996/05/11  02:30:14  tylin
 * CSCdi56413:  Multicast ping should have a ttl option
 *              Added for extended command only.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:35:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:57:37  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/25  08:32:22  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.2  1995/07/13  23:30:15  mikel
 * CSCdi37031:  DNSIX: eso packet not being stripped properly
 * fix incorrect pointer to extended security option
 *
 * Revision 2.1  1995/06/07  20:59:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Each interface has a whole slew of security related values associated
 * with it.  The values that are directly settable are:
 *
 * 	1) xfc low security level
 * 	2) xfc high security level
 * 	3) xfc low (required) authorities
 * 	4) xfc high (optional) authorities
 * 	5) ignore authorities in incoming packets
 * 	6) allow datagrams without a label (implicit)
 * 	7) allow extended security options
 * 	6) what to do with label on output
 *
 * The other two values are:
 * 	7) xfc is multilevel
 * 	8) xfc is at default state
 *
 * By providing appropriate defaults, we fully meet RFCs 1038 and 1108.  The
 * variety of choices allows users an easy transition method.  (Its also 
 * confusing as hell.)  When an interface is set to dedicated Unclassified 
 * GENSER, implicit labelling on input is turned on, and all output 
 * modifications are turned off.  When set to anything else, input must be 
 * labelled, and output adds a label if none is present.
 *
 */

#ifndef __IPOPTIONS_H__
#define __IPOPTIONS_H__

/*
 * Flags that denote options that may be in the IP packet.
 * ipopt_inproc is responsible for setting them.
 */
#define PAKOPT_HASSEC	0x00000001	/* basic or extended security*/
#define PAKOPT_HASLSR	0x00000002	/* Loose Source Route */
#define PAKOPT_HASSSR	0x00000004	/* Strict Source Route */
#define PAKOPT_HASRRT	0x00000008	/* Record RouTe */
#define PAKOPT_HASSID	0x00000010	/* Stream ID */
#define PAKOPT_HASTSTMP	0x00000020	/* Time stamp */
#define PAKOPT_SRNOTEND	0x00000040	/* route ptr not at list end */
#define PAKOPT_SECSTRIP	0x00000080	/* ip security stripped */
#define PAKOPT_SECADD	0x00000100	/* ip security added */
#define PAKOPT_HASALERT	0x00000200	/* IP router alert option */
#define PAKOPT_HASCIPSO	0x00000400	/* Commercial security options */

#define PAKOPT_HASROUTE  (PAKOPT_HASSSR | PAKOPT_HASLSR | PAKOPT_HASRRT)

/*
 * From RFC1700 (current assigned numbers)
 *
 *
 * IP OPTION NUMBERS
 * 
 * The Internet Protocol (IP) has provision for optional header fields
 * identified by an option type field.  Options 0 and 1 are exactly one
 * octet which is their type field.  All other options have their one
 * octet type field, followed by a one octet length field, followed by
 * length-2 octets of option data.  The option type field is sub-divided
 * into a one bit copied flag, a two bit class field, and a five bit
 * option number.  These taken together form an eight bit value for the
 * option type field.  IP options are commonly refered to by this value.
 * 
 * 
 * Copy Class Number Value Name                            Reference
 * ---- ----- ------ ----- ------------------------------- ---------
 *    0     0      0     0 EOOL   - End of Options List    [RFC791,JBP]
 *    0     0      1     1 NOP    - No Operation           [RFC791,JBP]
 *    1     0      2   130 SEC    - Security                  [RFC1108]
 *    1     0      3   131 LSR    - Loose Source Route     [RFC791,JBP]
 *    0     2      4    68 TS     - Time Stamp             [RFC791,JBP]
 *    1     0      5   133 E-SEC  - Extended Security         [RFC1108]
 *    1     0      6   134 CIPSO  - Commercial Security           [???]
 *    0     0      7     7 RR     - Record Route           [RFC791,JBP]
 *    1     0      8   136 SID    - Stream ID              [RFC791,JBP]
 *    1     0      9   137 SSR    - Strict Source Route    [RFC791,JBP]
 *    0     0     10    10 ZSU    - Experimental Measurement      [ZSu]
 *    0     0     11    11 MTUP   - MTU Probe                 [RFC1191]
 *    0     0     12    12 MTUR   - MTU Reply                 [RFC1191]
 *    1     2     13   205 FINN   - Experimental Flow Control    [Finn]
 *    1     0     14   142 VISA   - Expermental Access Control [Estrin]
 *    0     0     15    15 ENCODE - ???                      [VerSteeg]
 *    1     0     16   144 IMITD  - IMI Traffic Descriptor        [Lee]
 *    1     0     17   145 EIP    - ???                       [RFC1358]
 *    0     2     18    82 TR     - Traceroute                [RFC1393]
 *    1     0     19   147 ADDEXT - Address Extension    [Ullmann IPv7]
 */

#define	IPOPT_FRAGCOPY		0x80		/* copy on fragmentation */
#define	IPOPT_CLASS		0x60		/* class field of option */
#define	  IPOPT_CLASS_CONTROL	0x00
#define	  IPOPT_CLASS_RESERVED	0x20
#define	  IPOPT_CLASS_DEBUG	0x40
#define	  IPOPT_CLASS_RESERVED2	0x60
#define	IPOPT_NUMBER		0x1f		/* option number */

/*
 * All IP options are of the form "type-length-value" except
 * for END and NOP.
 */
#define	IPOPT_TLV(opt)	(((uchar) opt) > IPOPT_NOP)

#define IPOPT_END		0x00		/* end of options */
#define IPOPT_NOP		0x01		/* no-op */
#define IPOPT_BASSEC		0x82		/* basic security */
#define IPOPT_LSR		0x83		/* loose source route */
#define IPOPT_TSTMP		0x44		/* Internet Timestamp */
#define   IPOPT_TSTMP_TYPE0	 0		/*   timestamps only */
#define   IPOPT_TSTMP_TYPE1	 1		/*   timestamps and hosts */
#define   IPOPT_TSTMP_TYPE2	 2		/*   times and hosts */
#define IPOPT_EXTSEC		0x85		/* Extended Security */
#define	IPOPT_CIPSO		0x86		/* Commercial Security */
#define IPOPT_RRT		0x07		/* Record Route */
#define IPOPT_SID		0x88		/* Stream ID */
#define IPOPT_SSR		0x89		/* Strict Source Route */
#define	IPOPT_ZSU		0x0a		/* Experimental measure */
#define	IPOPT_MTUP		0x0b		/* MTU probe (RFC1191) */
#define	IPOPT_MTUR		0x0c		/* MTU probe (RFC1191) */
#define	IPOPT_FINN		0xcd		/* exp. flow control */
#define	IPOPT_VISA		0x8e		/* exp. access control */
#define	IPOPT_ENCODE		0x0f		/* ??? Versteeg */
#define	IPOPT_IMITD		0x90		/* IMI traffic desc */
#define	IPOPT_EIP		0x91		/* RFC1358 */
#define	IPOPT_TR		0x52		/* Traceroute */
#define	IPOPT_ADDEXT		0x93		/* address extension */
#define IPOPT_ALERT		0x94		/* Router alert */

#define PADDING			0

typedef struct ipopt_routetype_ {
    uchar code;				/* option type code */
    uchar length;			/* total length in bytes */
    uchar pointer;			/* pointer to current hop */
    uchar hops[1][4];			/* internet addresses of each host */
} ipopt_routetype;

#define IPOPT_ROUTEHEADERSIZE 3
#define IPOPT_ROUTEMINPTR 4
#define IPOPT_ROUTEMINLEN (IPOPT_ROUTEHEADERSIZE + sizeof(ipaddrtype))

#define SECURITY_SIZE sizeof(ipopt_bassectype)

typedef struct ipopt_bassectype_ {
    uchar code;
    uchar length;
    uchar classification;
    uchar authority[1];
} ipopt_bassectype;

/*
 * The following define the external values used in the Basic Security
 * security level field.  And how they should be compared.
 */
#define RESERVED4               0x01
#define TOPSECRET               0x3d
#define SECRET                  0x5a
#define CONFIDENTIAL            0x96
#define RESERVED3               0x66
#define RESERVED2               0xcc
#define UNCLASSIFIED            0xab
#define RESERVED1               0xf1
#define SECLVL_RESERVED4        8
#define SECLVL_TOPSECRET        7
#define SECLVL_SECRET           6
#define SECLVL_CONFIDENTIAL     5
#define SECLVL_RESERVED3        4
#define SECLVL_RESERVED2        3
#define SECLVL_UNCLASSIFIED     2
#define SECLVL_RESERVED1        1
#define SECINSERT                 0xff
#define SECNONE                   0

/* Authority Bits */

#define AUTH_GENSER     0x80
#define AUTH_SIOP_ESI   0x40
#define AUTH_SCI        0x20
#define AUTH_NSA        0x10
#define	AUTH_DOE	0x08
#define AUTH_CONT       0x01
#define AUTH_NONE       0x00
#define AUTH_UNSET      0xfe
#define AUTH_MASK       (AUTH_GENSER|AUTH_SIOP_ESI|AUTH_SCI|AUTH_NSA|AUTH_DOE)
#define MAX_AUTH_STRING 80

#define inclass(idb, class) ((idb->ip_sec_low <= class) && (idb->ip_sec_high >= class))
#define inauthl(idb, auth)  (( idb->ip_sec_auth_low  & auth) == idb->ip_sec_auth_low)
#define inauthh(idb, auth)  ((~idb->ip_sec_auth_high & auth) == 0)

/* Interface security style.   idb->ip_secure_(in|out)style */

#define SECSTYLE_NONE           0x00
#define SECSTYLE_ADD            0x01
#define SECSTYLE_STRIP          0x02
#define SECSTYLE_FIRST          0x04
#define SECSTYLE_IGNOREAUTH	0x10
#define SECSTYLE_EXTSECOK	0x20

/*
 *  What does security dictate that we do with this packet?
 */
#define SECRESP_BLACKHOLE 0
#define SECRESP_PROHIB    1
#define SECRESP_REQUIRED  2
#define SECRESP_REQNOROOM 3
#define SECRESP_ERROR     4


typedef struct ipopt_extsectype_ {
    uchar code;
    uchar length;
    uchar source;
    /*
     * Variable length auxiliary info goes here
     */
    uchar info[1];
} ipopt_extsectype;

typedef struct ipopt_cipsotype_ 
{
    uchar code;
    uchar length;
    /*
     * Variable length auxiliary info goes here
     */
    uchar info[1];
} ipopt_cipsotype;

typedef struct ipopt_alerttype_ {
    uchar code;
    uchar length;
    uchar value[2];
} ipopt_alerttype;

#define IPOPT_ALERTSIZE 4

typedef struct ipopt_sidtype_ {
    uchar code;
    uchar length;
    uchar streamid[2];
} ipopt_sidtype;

#define IPOPT_SIDSIZE 4

typedef struct ipopt_tstmptype_ {
    uchar code;
    uchar length;
    uchar pointer;		/* pointer into list of timestamps */
#ifdef BIGENDIAN
    uchar oflw:4;		/* count of overflows of timestamp list */
    uchar flg:4;		/* flag indicating type of list */
#endif
#ifdef LITTLEENDIAN
    uchar flg:4;		/* flag indicating type of list */
    uchar oflw:4;		/* count of overflows of timestamp list */
#endif
    uchar tsdata[1][4];		/* time stamps and perhaps ip addresses */
} ipopt_tstmptype;

#define IPOPT_TSTMPHEADERSIZE 4

/*
 * Extended IPSO support
 */
#define MAX_NLESO 16		/* Max of 16 different NLESO sources
				   configurable */
#define MAX_COMPARTMENT_BYTES 16
/*
 * How should extended IPSO be processed?
 */
#define IP_ESO_DENY	0	/* Disallow packets containing extended IPSO */
#define IP_ESO_IGNORE	1	/* Pass through and ignore extended IPSO */
#define IP_ESO_CHECK	2	/* Do DNSIX checks on extended IPSO */

/*
 * How should commercial IPSO be processed?
 */
#define IP_CIPSO_DENY	0	/* Disallow packets containing CIPSO */
#define IP_CIPSO_IGNORE	1	/* Pass through and ignore CIPSO */

typedef struct ip_sec_compartment_info_ {
    ushort max_bytes;
    uchar bit_default;
    uchar source_id;
} ip_sec_compartment_info;

enum NLESO_RANGE {
    NLESO_MIN,
    NLESO_MAX
};

typedef struct ip_sec_eso_source_ {
    struct ip_sec_eso_source_ *next;
    uchar type;				/* eso-min, eso-max or aeso info */
    uchar source_id;
    uchar compartment_bytes;
    uchar compartment_info[MAX_COMPARTMENT_BYTES];
} ip_sec_eso_source;

extern int ip_sec_info_entries;
extern ip_sec_compartment_info ip_sec_info[];
extern boolean ip_srcroute;		/* true if source routing allowed */

/*
 * Prototypes definitions for
 *    ipoptions.c
 *    ipoptparse.c
 */

/*
 * ipoptions.c
 */
extern boolean add_ipoption(ipoptiontype *, ipoptiontype *);
extern boolean ipopt_addtstmp(ipopt_tstmptype *, ipaddrtype);
extern boolean ipopt_checktcb(ipoptiontype **, paktype *);
extern boolean ipopt_copytcb(ipoptiontype *, paktype *);
extern boolean ip_bso_fixstyle(paktype *, boolean, uchar **, boolean *, 
			       uchar **);
extern boolean ip_eso_fixstyle(paktype *, boolean, uchar *, uchar *, int,
			       boolean *);
extern boolean ipopt_inproc(paktype *);
extern boolean ipopt_postroute(paktype *, ipaddrtype);
extern boolean parse_ipoptrecord(ipoptiontype *, char *, int);
extern boolean parse_ipopttstmp(ipoptiontype *, char *, int);
extern char *authority_string(uchar *);
extern char *security_string(uchar);
extern int map_security(int);
extern int optionbytesleft(int, ipoptiontype *);
extern boolean print_ipoptbassec(ipopt_bassectype *);
extern boolean print_ipoptextsec(ipopt_extsectype *);
extern boolean print_ipoptroute(ipopt_routetype *);
extern boolean print_ipoptsid(ipopt_sidtype *);
extern boolean print_ipoptalert(ipopt_alerttype *);
extern boolean print_ipopttstmp(ipopt_tstmptype *);
extern boolean print_ipoptcipso(ipopt_cipsotype *);
extern inv_map_security(int);
extern ipaddrtype ipopt_srin(paktype *, ipopt_routetype *);
extern ipopt_routetype *ipopt_findopt(uchar *, int, int);
extern void abort_for_security(paktype *, uint, uchar, uchar, char *);
extern void abort_opt(paktype *, uchar *, char *);
extern void extract_ipoptions(iphdrtype *, ipoptiontype *);
extern void ipopt_fixfrag(paktype *, int);
extern void ipopt_reversepak(paktype *);
extern void ipopt_reverseroute(ipopt_routetype *);
extern void print_ipoptions(ipoptiontype *);
extern void print_security(idbtype *);
extern void send_parameter2(paktype *, int, int, uchar, uchar);
extern void send_unreachable2(paktype *, uchar, uchar, uchar);
extern boolean parse_ipoptroute(ipoptiontype *, parseinfo *);
extern void ip_add_bso_opt(ipoptiontype *);
extern void ip_add_eso_opt(ipoptiontype *, ipaddrtype);
extern void ip_eso_remove_source(uchar);
extern void ip_eso_disable(idbtype *);
extern boolean ip_eso_valid_max(ushort);
extern boolean ip_sec_post_checks (paktype *, boolean, uchar *,
				   boolean, uchar *, int, boolean *);

/*
 * ipoptparse.c
 */
extern const char noroom[];
extern int parse_ipoptions(char *);
extern ulong get_options(ipoptiontype *, ipoptiontype *, char buff[],
			boolean *, ipaddrtype); 
extern void get_ping_options(char *, ushort *, ipoptiontype **,
			     ipoptiontype *, int *, boolean *, boolean *,
			     ipaddrtype *, ipaddrtype, int *, idbtype **,
			     boolean *, boolean *, ushort *);
extern void gipopt_prompt(boolean, ulong);

#endif __IPOPTIONS_H__
