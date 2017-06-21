/* $Id: address.h,v 3.3.22.3 1996/05/09 14:12:55 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/h/address.h,v $
 *------------------------------------------------------------------
 * address.h -- Generic Protocol and Hardware Address Definitions
 *
 * 1-March-1988, Kirk Lougheed, Greg Satz
 *
 * Copyright (c) 1988-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: address.h,v $
 * Revision 3.3.22.3  1996/05/09  14:12:55  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.3.22.2.2.1  1996/04/27  06:35:07  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.3.22.2  1996/04/25  23:11:02  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.3.22.1.8.2  1996/04/08  14:47:40  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.3.22.1.8.1  1996/04/08  01:45:41  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.3.22.1  1996/03/18  19:36:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.1  1996/03/22  09:35:35  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.3.12.1  1996/03/05  06:10:17  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.3.6.1  1996/02/27  20:41:09  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.3  1996/01/24  20:52:25  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 *
 * Revision 3.2.14.1  1996/01/11  01:44:52  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.2.10.1  1995/12/29  01:05:41  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.2  1995/11/17  09:11:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/23  22:49:56  ronnie
 * CSCdi38804:  InARP on ATM interfaces doesnt populate ARP table
 *         Many places were passing a complete addrtype to the hashing
 *         function, when only the first few bytes were set.  This made
 *         the hash function nondeterministic.
 *
 * Revision 2.2  1995/07/26  15:13:51  schrupp
 * CSCdi37690:  Support ATM ESI addressing with ILMI prefix
 *
 * Revision 2.1  1995/06/07  20:34:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Address family definitions
 * Address families are distinct from the protocols they
 * may run over (eg. CLNS may use either ADDR_IP or ADDR_X121).
 */

/* Don't cause trouble if we're included more than once. */
#ifndef _CSC_ADDRESS_
#define _CSC_ADDRESS_

/*
 * Note Well:
 * These address enumerations must match the CiscoNetworkProtocol
 * enumerations found in the Cisco MIB Textual Conventions
 * document, cisco-tc.my.  If you add an enumeration, please
 * update cisco-tc.my or contact someone in the snmp agent group.
 */
#define ADDR_ILLEGAL	0		/* illegal address */
#define ADDR_IP		1		/* IP address */
#define ADDR_DECNET	2		/* DECnet address */
#define	ADDR_CHAOS	4		/* Chaos address */
#define ADDR_XNS	5		/* XNS address */
#define ADDR_X121	6		/* X.121 address */
#define	ADDR_APPLE	7		/* AppleTalk address */
#define ADDR_CLNS	8		/* ISO 8473 CLNS NSAP */
#define ADDR_LAT	9		/* DEC LAT address */
#define ADDR_VINES	10		/* Banyan Vines address */
#define ADDR_CMNS	11		/* CMNS address */
#define ADDR_APOLLO	12		/* Apollo address */
#define ADDR_STUN	13		/* STUN address */
#define ADDR_NOVELL	14		/* Novell address */
#define ADDR_QLLC	15              /* qllc address */
#define ADDR_SNAPSHOT 	16              /* Snapshot routing map address */
#define ADDR_ILMI       17              /* ATM ILMI address */
#define ADDR_BSTUN      18              /* block serial tunnel */
#define ADDR_X25PVC	19		/* Logical Channel Number */
#define	ADDR_MAX	20		/* Max number of address types */

/*
 * Length in bytes
 */

#define ADDRLEN_IP	4
#define ADDRLEN_DECNET	2
#define	ADDRLEN_CHAOS	2
#define	ADDRLEN_FR   	2
#define ADDRLEN_XNS	10
#define ADDRLEN_X121	20		/* room for '\0' */
#define	ADDRLEN_APPLE	3
#define ADDRLEN_CLNS	21		/* must be consistent with
					   clns.h CLNS_MAX_NSAP_LENGTH */
#define ADDRLEN_LAT	17		/* lat hostnames don't really have
					   any address */
#define ADDRLEN_VINES	6
#define ADDRLEN_CMNS	21		/* 20 octets plus length only
					   must be consistent with
					   clns.h CLNS_MAX_NSAP_LENGTH */
#define ADDRLEN_APOLLO	6
#define ADDRLEN_STUN	8
#define ADDRLEN_NOVELL	10
#define ADDRLEN_QLLC    6
#define ADDRLEN_SNAPSHOT 1
#define ADDRLEN_ILMI    4
#define ADDRLEN_BSTUN	1
#define ADDRLEN_X25PVC	2

#define ADDRLEN_MAX	21

#include <string.h>

typedef struct {
    } bstun_address_t;			/* ibm/bstun.h BSTUN_MAX_ADDR_LEN.*/

union addresses {
    ipaddrtype ip_address;
    ushort decnet_address;
    ushort chaos_address;
    ushort fr_address;
    uchar snapshot_address;
    struct xns_addrtype {
	ulong net;
	uchar host[IEEEBYTES];
    } xns_address;
    struct nov_addrtype {
	ulong net;
	uchar host[IEEEBYTES];
    } nov_address;
    uchar x121_address[ADDRLEN_X121];
    struct apple_addrtype {
	ushort net;
	uchar host;
    } apple_address;
    struct apollo_addrtype {
      ulong net;
      uchar host[ADDRLEN_APOLLO];
    } apollo_address;
    uchar clns_address[ADDRLEN_CLNS];
    uchar lat_address[ADDRLEN_LAT];
    struct vines_addrtype {
	ulong net;
	ushort host;
    } vines_address;
    uchar cmns_address[ADDRLEN_CMNS];
    uchar stun_address[ADDRLEN_STUN];	/* string of bytes, should match */
					/* ibm/ibm.h STUN_MAX_ADDR_LEN.  */
    uchar qllc_address[ADDRLEN_QLLC];	/* used in qllc			 */
    ulong ilmi_address;  
    uchar bstun_address[ADDRLEN_BSTUN]; /* string of bytes, should match */
    uchar x25pvc_lcn[ADDRLEN_X25PVC];	/* Logical Channel Number        */
};

#define ip_addr		addr.ip_address
#define	decnet_addr	addr.decnet_address
#define	chaos_addr	addr.chaos_address
#define	xns_addr	addr.xns_address
#define	nov_addr	addr.nov_address
#define	x121_addr	addr.x121_address
#define	apple_addr	addr.apple_address
#define clns_addr	addr.clns_address
#define lat_addr	addr.lat_address
#define vines_addr	addr.vines_address
#define	cmns_addr	addr.cmns_address
#define apollo_addr	addr.apollo_address
#define stun_addr	addr.stun_address
#define qllc_addr	addr.qllc_address
#define snapshot_addr	addr.snapshot_address
#define ilmi_addr       addr.ilmi_address
#define bstun_addr	addr.bstun_address

struct addrtype_ {
    uchar type;
    uchar length;
    union addresses addr;
};

/*
 * address_copy(to, from)
 *
 * Copy an addrtype structure.
 */
static inline void address_copy(addrtype *to, addrtype *from)
{
    *to = *from;
}

extern char *address_string(addrtype *);
extern boolean address_equ (addrtype *, addrtype *);

/*
 * Hardware family definitions
 */

typedef enum
{
    STATION_ILLEGAL,		/* illegal address */
    STATION_IEEE48,		/* 48bit IEEE 802.X address */
    STATION_IEEE16,		/* 16bit IEEE 802.X address */
    STATION_XEROX,		/* Xerox 3MB experimental ether */
    STATION_X121,		/* CCITT X.121 address */
    STATION_CISCO_HDLC,		/* cisco HDLC framing */
    STATION_CISCO_MLAPB,	/* cisco multi-LAPB framing */
    STATION_LAPB,		/* ISO/CCITT LAPB framing */
    STATION_SMDS48,		/* SMDS w/ 48 bit addresses */
    STATION_CISCO_MSDLC,	/* cisco multi-SDLC framing */
    STATION_FR10,		/* Frame Relay with 10-bit DLCI */
    STATION_ULTRA,		/* CCCI defined Ultranet */
    STATION_CISCO_TUNNEL,	/* cisco tunnel and EON encoding */
    STATION_PPP,		/* PPP framing */
    STATION_SMDS64,		/* SMDS 64-bit addressing */
    STATION_ATMVC,		/* AIP VC no. */

    STATION_ATMNSAP,            /* ATM NSAP address */
    STATION_SDLC,		/* SDLC address */
    STATION_X25PVC,		/* X.25 Lci fpr PVC's */
    /*
    ** the two below is used ONLY to
    ** tell the parser to use a different
    ** parsing routine.  The type field will
    ** always be "STATION_ATMNSAP".
    */
    STATION_MASKED_ATMNSAP,     /* masked ATM NSAP address */
    STATION_ATMESI,             /* ATM ESI address */
    STATION_SLIP,               /* SLIP framing */

    STATION_ROUTEDESC,				/* route descriptor for TR LANE */
} SNPAType_e;


/*
 * Length in bytes
 */

#define	STATIONLEN_X121		20
#define STATIONLEN_E164         15
#define STATIONLEN_X25PVC	2
#define	STATIONLEN_IEEE48	6
#define	STATIONLEN_IEEE16	2
#define	STATIONLEN_XEROX	1
#define STATIONLEN_CISCO_HDLC	0
#define STATIONLEN_CISCO_TUNNEL	0
#define STATIONLEN_CISCO_MLAPB	0
#define STATIONLEN_LAPB		0
#define STATIONLEN_SMDS48	6
#define STATIONLEN_SMDS64	8
#define STATIONLEN_FR10		2
#define STATIONLEN_CISCO_MSDLC	0
#define STATIONLEN_ULTRA	2
#define STATIONLEN_PPP		0
#define	STATIONLEN_ATMVC	2
#define STATIONLEN_ATMNSAP      20
#define STATIONLEN_ATMESI       7
#define STATIONLEN_SDLC		1
#define STATIONLEN_ROUTEDESC  	2
#define STATIONLEN_SLIP		0

#define	STATIONLEN_MAX		20

/*
 * target types
 */

typedef enum
{
    TARGET_UNICAST,
    TARGET_BROADCAST,
    TARGET_HP_PROBE,			/* HP Probe multicast */
    TARGET_DN_3,			/* Decnet */
    TARGET_DN_4,			/* Decnet */
    TARGET_OSI_ALL_ES,			/* OSI All End Systems */
    TARGET_OSI_ALL_IS,			/* OSI All Intermediate Systems */
    TARGET_OSI_ALL_L1,			/* OSI All L1 Routers */
    TARGET_OSI_ALL_L2,			/* OSI All L2 Routers */
    TARGET_OSI_ALL_IGRP_L1,		/* ISO-IGRP level 1 routers */
    TARGET_OSI_ALL_IGRP_L2		/* ISO-IGRP level 2 routers */
} SNPATarget_e;

struct hwaddrtype_ {
    uchar type;
    uchar length;
    ushort target_type;
    /* please ensure that "addr" stays aligned to sizeof (long) for speed */
    uchar addr[STATIONLEN_MAX];
};

extern void snpa_init(hwaddrtype* snpa, SNPAType_e type, SNPATarget_e target_type, uchar* addr);
extern boolean snpa_equ(hwaddrtype *snpa1, hwaddrtype *snpa2);

/*
 * snpa_clear
 *
 * Make an snpa invalid.
 */
static inline void snpa_clear (hwaddrtype *snpa)
{
    snpa->type = STATION_ILLEGAL;
    snpa->length = 0;
    snpa->addr[0] = (uchar) 0;
}

/*
 * snpa_is_valid
 *
 * Return true if the address is valid.
 * Return false if the address is not valid.
 */
static inline boolean snpa_is_valid(hwaddrtype *snpa)
{
    if (snpa->type != STATION_ILLEGAL)
	return(TRUE);
    else
	return(FALSE);
}

/*
 * snpa_is_invalid
 *
 * Return true if the address is not valid.
 * Return false if the address is valid.
 */
static inline boolean snpa_is_invalid(hwaddrtype *snpa)
{
    return(!snpa_is_valid(snpa));
}

/*
 * snpa_copy
 * Copy a snpa
 */
static inline void snpa_copy (hwaddrtype *src, hwaddrtype *dst)
{
    if (src) {
	memcpy(dst, src, sizeof(hwaddrtype));
    }
}

static inline SNPAType_e snpa_get_type(hwaddrtype *snpa)
{
    return(snpa->type);
}

static inline SNPATarget_e snpa_get_target_type(hwaddrtype *snpa)
{
    return(snpa->target_type);
}

static inline ushort snpa_get_length(hwaddrtype *snpa)
{
    return(snpa->length);
}

static inline uchar* snpa_copy_to_buffer(hwaddrtype *snpa, uchar *buffer)
{
    memcpy(buffer, snpa->addr, snpa->length);
    return(buffer);
}

/*
** this structure is for representing MASKED addresses.  
** Each significant bit in the data is represented by
** a "1" bit in the corresponding position in the mask.
** Conversely, each dont care bit in the data is represented
** by a "0" bit in the mask.
**
** This object is used heavily in the ATM subsystem, especially
** in the Lan Emulation section.
*/
typedef struct maskedAddress
{
    uchar type;
    uchar length;
    long aligner1 [0];	/* to align the NEXT field to sizeof (long) */
    uchar dataBytes [STATIONLEN_MAX];
    long aligner2 [0];	/* to align the NEXT field to sizeof (long) */
    uchar maskBytes [STATIONLEN_MAX];

} MaskedAddress, *pMaskedAddress;

static inline
void clearMaskedAddress (MaskedAddress *ma)
{
    register int i;

    ma->type = STATION_ILLEGAL;
    ma->length = 0;
    for (i=0; i<STATIONLEN_MAX; i++) {
	ma->dataBytes [i] = 0;
	ma->maskBytes [i] = 0xFF;
    }
}

static inline 
void MaskedAddress2hwaddrtype (MaskedAddress *ma, hwaddrtype *hw)
{
    register int i;

    hw->type = ma->type;
    hw->length = ma->length;
    for (i=0; i<STATIONLEN_MAX; i++)
	hw->addr [i] = ma->dataBytes [i];
}

static inline 
void hwaddrtype2MaskedAddress (hwaddrtype *hw, MaskedAddress *ma)
{
    register int i;

    ma->type = hw->type;
    ma->length = hw->length;
    for (i=0; i<STATIONLEN_MAX; i++)
    {
	ma->dataBytes [i] = hw->addr [i];
	ma->maskBytes [i] = 0xFF;
    }
}

#endif _CSC_ADDRESS_

