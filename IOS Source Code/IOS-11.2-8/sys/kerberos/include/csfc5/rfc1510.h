/* $Id: rfc1510.h,v 3.2 1995/11/17 17:48:54 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/kerberos/include/csfc5/rfc1510.h,v $
 *------------------------------------------------------------------
 * rfc1510.h - RFC1510 definitions
 *
 * September 1995, Che-lin Ho
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rfc1510.h,v $
 * Revision 3.2  1995/11/17  17:48:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:21:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _CSFC5_RFC1510_H_
#define _CSFC5_RFC1510_H_ "%W% %G%"
/*.original proto.h 3.1 12/29/93*/

/*
 * Copyright (C) 1994 CyberSAFE Corporation.
 * All rights reserved
 */

/*
 * RFC 1510 defined constants.
 *
 * From RFC 1510, by J. Kohl and C. Neuman, September, 1993
 *
 * For the RFC-defined error definitions, see "rfc1510e.h".
 *
 * References to the RFC are denoted by "s n.n pg n" for sections and
 * pages; keywords from the RFC are also listed.
 *
 *                         PAY ATTENTION
 *
 * Changes to this file must be shown in the revision section below,
 * along with a reference to the source and reason for the revision.
 *
 * This file consists EXCLUSIVELY of constants defined in the RFC.
 * DEFINITIONS OF CONVENIENCE OR CONSENSUS DO NOT BELONG HERE.
 *
 * All definitions in this file are prefixed with "CSFC5c" to indicate
 * an RFC-defined constant.  Some of these definitions are not yet
 * official; these definition are prefixed with "CSFC5cx" AND WILL BE
 * REMOVED AT THE EARLIEST OPPORTUNITY.
 */

/*.Revision

*/

/*
 * Protocol version number
 */

#define CSFC5c_PVNO   5


/*
 * AMT -- ASN1 message types
 */

#define CSFC5c_AMT_AS_REQ	10	/* Initial authentication service request (AP->KDC) */
#define CSFC5c_AMT_AS_REP	11	/* Reply to AS_REQ (AP<-KDC) */
#define CSFC5c_AMT_TGS_REQ	12	/* Ticket granting service request (AP->KDC) */
#define CSFC5c_AMT_TGS_REP	13	/* Reply to TGS_REQ (AP<-KDC) */
#define CSFC5c_AMT_AP_REQ	14	/* Application request (AP-AP) */
#define CSFC5c_AMT_AP_REP	15	/* Reply to AP_REQ (AP-AP) */
#define CSFC5c_AMT_SAFE		20	/* Safe application message (AP-AP) */
#define CSFC5c_AMT_PRIV		21	/* Private application message (AP-AP)*/
#define CSFC5c_AMT_CRED		22	/* Credential forwarding message (AP-AP) */
#define CSFC5c_AMT_ERROR	30	/* Error response */


/*
 * LR_T -- Last request types
 *
 * This field is used to interpret the encoding of the
 * lr-value field.  Negative values indicate lr-value
 * pertains only to the responding server; non-negative
 * values pertain to all servers for the realm.
 *
 * s 5.2 pg 41
 * lr-type, LastReq
 */

#define CSFC5c_LR_T_NONE		0	/* No information. */
									/* Time of last.. */
#define CSFC5c_LR_T_TGT			1	/* request for TGT. */
#define CSFC5c_LR_T_INITIAL		2	/* initial request. */
#define CSFC5c_LR_T_TGT_ISSUED	3	/* issue of newest TGT. */
#define CSFC5c_LR_T_RENEWAL		4	/* renewal. */
#define CSFC5c_LR_T_REQ			5	/* request (of any type). */


/*
 * PADATA_TYPE -- Preauthorization data types
 *
 * s 5.4.1 pg 49, s 8.3 pg 83
 * padata-type
 */

#define CSFC5c_PADATA_T_NONE				0
#define CSFC5c_PADATA_T_TGS_REQ				1	/* Ticket in TGS request */
#define CSFC5c_PADATA_T_ENC_TIMESTAMPxxx	2	/* Encrypted timestamp */
#define CSFC5c_PADATA_T_PW_SALT				3
#define CSFC5cx_PADATA_T_ENC_UNIX_TIMESTAMP	5	/* Encrypted "Unix" timestamp */

/* ALTM -- Alternate authentication type methods
 *
 * s 5.9.1 pg 67, s 8.3 pg 83
 * method-type, pa-data
 */

/* 0-63 reserved */
#define CSFC5c_PA_DATA_ALTM_ATT_CH_RSP		64	/* ATT challenge/response */


/*
 * NT -- Name types (principals)
 *
 * s 7.2 pg 79, s 8.3 pg 84
 */

#define CSFC5c_NT_UNKNOWN	0	/* Name type not known */
#define CSFC5c_NT_PRINCIPAL	1	/* Just the name (e.g., DCE, or for users) */
								/* Services with ... */
#define CSFC5c_NT_SRV_INST	2	/* unique instance (e.g., krbtgt) */
#define CSFC5c_NT_SRV_HST	3	/* host name instance (e.g., telnet, r-utils) */
#define CSFC5c_NT_SRV_XHST	4	/* host as remaining components */
#define CSFC5c_NT_UID		5	/* Unique ID */


/*
 * NAME -- Reserved names.
 */

/* Name of the Kerberos service
 *
 * s ???
 */
#define CSFC5cx_NAME_KERBEROS	"kerberos"

/* Principal name of the Kerberos ticket granting service.
 * I.e., the ticket-granting-ticket (tgt) service name.
 *
 * s 7.2
 */
#define CSFC5c_NAME_TGS			"krbtgt"

/* Principal service name of "system" services (e.g., telnet, r-utils).
 *
 * s 7.2
 */
#define CSFC5c_NAME_SRVHST		"host"


/*
 * NAMEC -- Name component characters
 *
 * s ???
 */
#define CSFC5c_NAMEC_REALM_SEP		'@'		/* principal@realm */
#define CSFC5c_NAMEC_COMPONENT_SEP	'/'		/* principal/instance@realm */


/*
 * TR_T -- Transited encoding types
 *
 * s 3.3.3.1 pg 29, s 5.3.1 pg 52, s 8.3 pg 83
 * tr-type, TransitedEncoding
 */

/* all others reserved */
#define CSFC5c_TR_T_DOMAIN_X500_COMPRESS	1


/*
 * AD_T -- Authzrization data types
 *
 * s 8.3 pg 83
 * ad-type
 */

/* 0-63 reserved */
#define CSFC5c_AD_T_OSF_DCE			64
#define CSFC5c_AD_T_SESAME			65


/*
 * CRS -- Crypto system definitions
 */


/* CRS_ET -- Encryption types.
 *
 * s 6.3, 8.3
 * etype
 */

/* NULL -- None */
#define CSFC5c_CRS_ET_NULL				0
#define CSFC5c_CRS_ET_NULL_blks			1	/* Block size (octets) */
#define CSFC5c_CRS_ET_NULL_pads			0	/* Minimum pad size (octets) */
#define CSFC5c_CRS_ET_NULL_cnfs			0	/* Confounder size (octets) */

/* DES_CBC_CRC -- DES cbc mode with CRC-32 (ISO 3309/Autodin-II) */
#define CSFC5c_CRS_ET_DES_CBC_CRC		1
#define CSFC5c_CRS_ET_DES_CBC_CRC_blks	8
#define CSFC5c_CRS_ET_DES_CBC_CRC_pads	4
#define CSFC5c_CRS_ET_DES_CBC_CRC_cnfs	8

/* DES_CBC_MD4 -- DES cbc mode with RSA-MD4 */
#define CSFC5c_CRS_ET_DES_CBC_MD4		2
#define CSFC5c_CRS_ET_DES_CBC_MD4_blks	8
#define CSFC5c_CRS_ET_DES_CBC_MD4_pads	0
#define CSFC5c_CRS_ET_DES_CBC_MD4_cnfs	8

/* DES_CBC_MD5 -- DES cbc mode with RSA-MD5 */
#define CSFC5c_CRS_ET_DES_CBC_MD5		3
#define CSFC5c_CRS_ET_DES_CBC_MD5_blks	8
#define CSFC5c_CRS_ET_DES_CBC_MD5_pads	0
#define CSFC5c_CRS_ET_DES_CBC_MD5_cnfs	8

/* DES_CBC_RAW -- Raw DES CBC */
#define CSFC5cx_CRS_ET_DES_CBC_RAW		4
#define CSFC5cx_CRS_ET_DES_CBC_RAW_blks	xxx	/*8?*/
#define CSFC5cx_CRS_ET_DES_CBC_RAW_pads	xxx
#define CSFC5cx_CRS_ET_DES_CBC_RAW_cnfs	xxx


/* CRS_KT - Key types.
 *
 * s ???
 */
#define CSFC5c_CRS_KT_NULL		0		/* None */
#define CSFC5c_CRS_KT_DES		1		/* Data Encryption Standard, FIPS 46,81 */


/* CKS_T -- Checksum types.
 * Although these are nominally part of the crypto-system, they are
 * really separate definitions, thus the use of "CKS" instead of
 * "CRS"
 *
 * s 8.3
 * sumtype
 */

#define CSFC5c_CKS_T_CRC32				1
#define CSFC5c_CKS_T_CRC32_cks			4		/* Checksum size (octets) */

#define CSFC5c_CKS_T_RSA_MD4				2
#define CSFC5c_CKS_T_RSA_MD4_cks			16

#define CSFC5c_CKS_T_RSA_MD4_DES			3
#define CSFC5c_CKS_T_RSA_MD4_DES_cks		16	/*TBD/JNK RFC sez 24; MIT code had 16 */

#define CSFC5c_CKS_T_DES_MAC				4
#define CSFC5c_CKS_T_DES_MAC_cks			16

#define CSFC5cx_CKS_T_DESCBC				CSFC5c_CKS_T_DES_MAC		/* ??? */
#define CSFC5cx_CKS_T_DESCBC_cks			CSFC5c_CKS_T_DES_MAC_cks

#define CSFC5c_CKS_T_DES_MAC_K			5
#define CSFC5c_CKS_T_DES_MAC_K_cks		8

#define CSFC5c_CKS_T_RSA_MD4_DES_K		6
#define CSFC5c_CKS_T_RSA_MD4_DES_K_cks	16

#define CSFC5c_CKS_T_RSA_MD5				7
#define CSFC5c_CKS_T_RSA_MD5_cks			16

#define CSFC5c_CKS_T_RSA_MD5_DES			8
#define CSFC5c_CKS_T_RSA_MD5_DES_cks		16	/*TBD/JNK RFC sez 24; MIT code had 16 */

/*
 * HADDR_T -- Network/Host haddr types.
 *
 * These are defined to map directly to BSD <sys/socket.h> AF_xxx numbers.
 *
 * s 8.1
 * addr-type, HostAddress, HostAddresses
 */

/* The following are only those explicitly listed in the RFC. */
#define CSFC5c_HADDR_T_INET			2		/* Internet */
#define CSFC5c_HADDR_T_CHAOS		5		/* CHAOSnet */
#define CSFC5c_HADDR_T_ISO			7		/* ISO */
#define CSFC5c_HADDR_T_XNS			6		/* Xeros Network Services */
#define CSFC5c_HADDR_T_APPLEDDP		16		/* Appletalk Datagram Delivery Protocol */
#define CSFC5c_HADDR_T_DECNET4		12		/* DECnet Phase IV */

#define CSFC5cx_HADDR_T_PORT		0x0100
#define CSFC5cx_HADDR_T_IPPORT		0x0101


/* KDCOPT -- KDC options flags.
 * (In KDC request).
 *
 * s 5.4.1 pp 51-54
 * KDCOPtions, AS_REP, TGS_REQ
 */

#define CSFC5c_KDCOPT_FORWARDABLE_m		0x40000000
#define CSFC5c_KDCOPT_FORWARDED_m		0x20000000
#define CSFC5c_KDCOPT_PROXIABLE_m		0x10000000
#define CSFC5c_KDCOPT_PROXY_m			0x08000000
#define CSFC5c_KDCOPT_ALLOW_POSTDATE_m	0x04000000
#define CSFC5c_KDCOPT_POSTDATED_m		0x02000000
#define CSFC5c_KDCOPT_RENEWABLE_m		0x00800000
#define CSFC5c_KDCOPT_RENEWABLE_OK_m	0x00000010
#define CSFC5c_KDCOPT_ENC_TKT_IN_SKEY_m	0x00000008
#define CSFC5c_KDCOPT_RENEW_m			0x00000002
#define CSFC5c_KDCOPT_VALIDATE_m		0x00000001

#define CSFC5c_KDCOPT_m ( \
	CSFC5c_KDCOPT_FORWARDABLE_m | \
	CSFC5c_KDCOPT_FORWARDED_m | \
	CSFC5c_KDCOPT_PROXIABLE_m | \
	CSFC5c_KDCOPT_PROXY_m | \
	CSFC5c_KDCOPT_ALLOW_POSTDATE_m | \
	CSFC5c_KDCOPT_POSTDATED_m | \
	CSFC5c_KDCOPT_RENEWABLE_m | \
	CSFC5c_KDCOPT_RENEWABLE_OK_m | \
	CSFC5c_KDCOPT_ENC_TKT_IN_SKEY_m | \
	CSFC5c_KDCOPT_RENEW_m | \
	CSFC5c_KDCOPT_VALIDATE_m \
)

#define CSFC5c_KDCOPT_RESERVED_m (~(CSFC5c_KDCOPT_m))


/* TKTFLG -- Ticket flags in issued ticket.
 * (In KDC reply.)
 *
 * s 5.4.1 pp 51-54
 * TicketFlags, AS_REP, AP_REP
 */

#define CSFC5c_TKTFLG_FORWARDABLE_m		0x40000000
#define CSFC5c_TKTFLG_FORWARDED_m		0x20000000
#define CSFC5c_TKTFLG_PROXIABLE_m		0x10000000
#define CSFC5c_TKTFLG_PROXY_m			0x08000000
#define CSFC5c_TKTFLG_MAY_POSTDATE_m	0x04000000
#define CSFC5c_TKTFLG_POSTDATED_m		0x02000000
#define CSFC5c_TKTFLG_INVALID_m			0x01000000
#define CSFC5c_TKTFLG_RENEWABLE_m		0x00800000
#define CSFC5c_TKTFLG_INITIAL_m			0x00400000
#define CSFC5c_TKTFLG_PRE_AUTHENT_m		0x00200000
#define CSFC5c_TKTFLG_HW_AUTHENT_m		0x00100000

#define CSFC5c_TKTFLG_m ( \
	CSFC5c_TKTFLG_FORWARDABLE_m | \
	CSFC5c_TKTFLG_FORWARDED_m | \
	CSFC5c_TKTFLG_PROXIABLE_m | \
	CSFC5c_TKTFLG_PROXY_m | \
	CSFC5c_TKTFLG_MAY_POSTDATE_m | \
	CSFC5c_TKTFLG_POSTDATED_m | \
	CSFC5c_TKTFLG_INVALID_m | \
	CSFC5c_TKTFLG_RENEWABLE_m | \
	CSFC5c_TKTFLG_INITIAL_m | \
	CSFC5c_TKTFLG_PRE_AUTHENT_m | \
	CSFC5c_TKTFLG_HW_AUTHENT_m \
)

#define CSFC5c_TKTFLG_RESERVED_m (~(CSFC5c_TKTFLG_m))


/* KDCTKT_COMMON - Flags common between KDC options ticket.
 * (I.e., same for KDC request and issued ticket.)
 */
#define CSFC5cx_KDCTKT_COMMON_m ( \
	CSFC5c_TKTFLG_FORWARDABLE_m | \
	CSFC5c_TKTFLG_FORWARDED_m | \
	CSFC5c_TKTFLG_PROXIABLE_m | \
	CSFC5c_TKTFLG_PROXY_m | \
	CSFC5c_TKTFLG_MAY_POSTDATE_m | \
	CSFC5c_TKTFLG_POSTDATED_m | \
	CSFC5c_TKTFLG_INVALID_m | \
	CSFC5c_TKTFLG_RENEWABLE_m \
)


/* APOPTS -- Application options flags.
 * (In AP request and reply.)
 *
 * s 5.5.1 pg 59
 * APOptions, AP_REQ
 */

#define CSFC5c_APOPTS_USE_SESSION_KEY_m	0x40000000
#define CSFC5c_APOPTS_MUTUAL_REQUIRED_m	0x20000000

#define CSFC5c_APOPTS_m ( \
	CSFC5c_APOPTS_USE_SESSION_KEY_m | \
	CSFC5c_APOPTS_MUTUAL_REQUIRED_m \
)

#define CSFC5c_APOPTS_RESERVED_m (~(CSFC5c_APOPTS_m))

#endif /* _CSFC5_RFC1510_H_ */
