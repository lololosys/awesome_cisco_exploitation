/* $Id: ieee.h,v 3.8.40.5 1996/05/30 23:43:31 hampton Exp $
 * $Source: /release/112/cvs/Xsys/h/ieee.h,v $
 *------------------------------------------------------------------
 * ieee.h  --  IEEE 802.x definitions
 *
 * May 1993, David R. Hampton
 * Mostly taken from modules dated October 1986 and January 1988.
 *
 * Copyright (c) 1986-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ieee.h,v $
 * Revision 3.8.40.5  1996/05/30  23:43:31  hampton
 * All commands that take access list numbers should accept either numbers
 * in the current monolithic number space, or hidden numbers in the future
 * per-protocol access list spaces.  [CSCdi59067]
 * Branch: California_branch
 *
 * Revision 3.8.40.4  1996/05/17  11:15:37  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.8.28.3  1996/05/05  23:05:04  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.8.28.2  1996/04/24  06:30:26  pmorton
 * Branch: IbuMod_Calif_branch
 * DLSw Cleanup:  Remove use of SAP 99.
 *
 * Revision 3.8.28.1  1996/03/30  00:26:55  schiang
 * CSCdi53042:  ieee_add and ieee_subtract perform warp incorrectly
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.8.40.3  1996/05/10  01:37:24  wmay
 * CPP and ppp half bridge commit
 * Branch: California_branch
 *
 * Revision 3.8.40.2  1996/05/09  14:13:07  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.8.40.1.10.1  1996/04/27  06:35:15  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.8.40.1  1996/04/03  23:18:45  dino
 * CSCdi51598:  Problem of interoperability with switches
 * Branch: California_branch
 *
 * Revision 3.8.58.1  1996/04/08  01:45:48  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.8.46.1  1996/03/22  09:35:37  rlowe
 * Non-sync Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.8.32.1  1996/03/05  06:10:19  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.8.22.1  1996/02/27  20:41:11  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.8.2.1  1996/01/11  01:44:54  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.8  1995/12/30  00:25:50  hou
 * CSCdi37413:  RTBR bridging fails for Ether->Token->Ether on vines and
 * ip.
 * - Set the default OUI for SNAP on token ring interface as 0x000000
 *   instead of 0x0000f8.
 * - Make low-end "ethernet-transit-oui" fully functional.
 *
 * Revision 3.7  1995/12/08  02:01:45  bcole
 * CSCdi45234:  NHRP: Dont refresh unused cache entries
 *
 * Keep track of which cache entries are used, and avoid refreshing unused
 * entries.  Fix holddown time for implicit cache entries.
 *
 * Use IANA defined SNAP OUI for NHRP.
 *
 * Revision 3.6  1995/12/06  20:32:07  mmcneali
 * CSCdi44534:  Define Cisco Types for Dynamic ISL and Catalyst
 *              Switching Inter-Process Communications.
 *
 * Revision 3.5.4.2  1996/01/09  00:36:06  dcheng
 * Changed the HDLC type code for VTP.
 * Branch: LE_Calif_branch
 *
 * Revision 3.5.4.1  1996/01/06  00:34:26  dcheng
 * Changed HDLC type code for VTP to 2004.
 * Branch: LE_Calif_branch
 *
 * Revision 3.5  1995/12/01  09:22:25  mmcneali
 * CSCdi44792:  Assign VTP Type Code.
 *
 * Revision 3.4  1995/11/30  17:31:54  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * remove value conflict between TYPE_HHRP and TYPE_COMPRESSED_FR
 *
 * Revision 3.3  1995/11/21  20:42:31  turadek
 * CSCdi43841:  add frame relay payload compression feature
 * syncing code to 11.0
 *
 * Revision 3.2  1995/11/17  09:12:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:16  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:58:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/10/18  05:31:22  pmorton
 * CSCdi40639:  CMNS uses wrong MAC address to open LLC2 session
 * Add prototypes for idb_is_tokenring_like, and llc_clear_rii.
 *
 * Revision 2.1  1995/06/07  20:35:17  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IEEE_H__
#define __IEEE_H__

/**********************************************************************
 *
 *			    Global Externs
 *
 **********************************************************************/

extern uchar const baddr[];		/* broadcast address */
extern uchar const zeromac[];		/* no address */
extern uchar const zerosnap[];		/* ZERO OUI bytes for SNAP */
extern uchar const cisco_snap[];	/* ZERO OUI bytes for SNAP */
extern uchar const tb1490_snap[];		/* ZERO OUI bytes for SNAP */
extern uchar const bit_swaps[];
extern uchar const iana_snap[];

/**********************************************************************
 *
 *			  802.{3,5} definitions.
 *
 **********************************************************************/

#define IEEEBYTES 6			/* bytes in an IEEE address */
#define IEEEWORDS 3			/* words in an IEEE address */
#define BITSWAP(x) (bit_swaps[(x)])

typedef struct ieee_addrs_ {
    uchar daddr[IEEEBYTES];
    uchar saddr[IEEEBYTES];
} ieee_addrs;

/**********************************************************************
 *
 *			  802.2 definitions.
 *
 * xxxx xx0x - User defined Codes
 * xxxx xx1x - Standard LSAP Codes
 *             xxxx 001x - International standards
 *             xxxx 011x - National standards
 *             xxxx 101x - International standards
 * 	       xxxx 111x - International standards
 *
 * Strictly speaking SAP_NOVELL_ETHER is an outrageous KLUDGE!  Seems
 * Novell used a raw, non-complient, XNS encapsulation.  They wrapped
 * an XNS packet in a "raw" 802.3 packet.  That is it has a length but
 * no 802.2 layer.
 **********************************************************************/

typedef struct sap_hdr_ {
    uchar  dsap;		/* destinations service access point */
    uchar  ssap;		/* source service access point */
    uchar  control;		/* frame type */
    uchar  data[0];
} sap_hdr;

/*
 * Bit definitions
 */
#define SAP_RESPONSE	0x01		/* ssap only, command/response bit  */
#define SAP_GROUP	0x01		/* dsap only, group address         */
#define SAP_USERDEF	0x02		/* both, user defined if bit is off */
#define SAP_STANDARD	0x0C		/* both, defined by what standard   */

/*
 * (Inter)Nationally defined SAP values.
 */
#define SAP_NULL	0x00
#define SAP_SMF		0x02
#define SAP_SMF_GROUP	0x03
#define SAP_IP		0x06
#define SAP_ISA_NETMAN	0x0E
#define SAP_SPAN	0x42
#define SAP_EIA_RS511	0x4E
#define SAP_ISA_STAMAN	0x8E
#define SAP_SNAP	0xAA
#define SAP_CLNS	0xFE
#define SAP_GLOBAL	0xFF
#define SAP_SDE         0x0A       	/* 802.10 Secure Data Exchange */

/*
 * User defined SAP values.
 */
#define SAP_IBM_SNA	0x04		/* User defined - IBM */
#define SAP_ULTRA_RTE   0x20            /* User defined - Ultranet */
#define SAP_CONS        0x7E            /* User defined - CONS */
#define SAP_CMNS        SAP_CONS        /* User defined - Cisco CMNS == CONS */
#define SAP_3COM	0x80		/* User defined - 3Com */
#define SAP_NOVELL	0xE0		/* User defined - Novell */
#define SAP_VINES	0xBC		/* User defined - Banyan */
#define SAP_NETBIOS     0xF0	        /* User defined - IBM */
#define SAP_IBMNM	0xF4		/* User defined - IBM */
#define SAP_EXTENDED	0xFC		/* User defined  - HP */
#define SAP_NOVELL_ETHER 0xFF		/* totally bogus - Novell */
#define DLSW_SAP 99                     /* special bridging SAP */

/*
 * Control byte.  The format of the control byte is as follows:
 *
 * 	MMMP MM11
 *
 * where 'P' is the poll/final bit, and the 'MMMMM' bits are the
 * operation.  The only operation codes defined here are the LLC Type 1
 * operations.  All of the LLC Type 2 operation codes are defined in the
 * llc2 directory.
 */

#define LLC1_CLASS	0x03
#define LLC1_CLASS_I1	0x00
#define LLC1_CLASS_S	0x01
#define LLC1_CLASS_I2	0x00
#define LLC1_CLASS_U	0x03

#define LLC1_UI		0x03
#define LLC1_XID	0xAF
#define LLC1_TEST	0xE3
#define LLC1_P		0x10

#define XID_FMT_STD_LEN 3
#define XID_FMT_STD	0x81		/* 802.2 XID Format */
#define XID_CLASS_TYPE1	0x01		/* supports LLC type 1 */
#define XID_CLASS_TYPE2	0x02		/* supports LLC type 2 */
#define XID_WIN_TYPE1	0x00		/* type 1 only, then 0 window size */
#define XID_WIN_TYPE2   0xFE		/* window size of 127 for LLC Type 2 */
#define XID_LLC_TYPE	0x01
#define XID_RCV_WIN	0x00

#define XID_FMT_MASK	0xf0
#define XID_FMT_0	0x00
#define XID_FMT_1	0x10
#define XID_FMT_2	0x20
#define XID_FMT_3	0x30
#define XID_FMT_8	0x80

#define XID_TYPE_MASK	0x0f
#define XID_TYPE_1	0x01
#define XID_TYPE_2	0x02
#define XID_TYPE_4	0x04

#define XID_ERROR_CV    0x22
	/* XID negotiation error control vector */

#define XID_TYPE0_FMT2  0x02

/*
 * Miscellaneous
 */
#define SAP_HDRBYTES       3	/* 2 SAP bytes and 1 control byte */
#define SAP_LONGHDRBYTES   4	/* 2 SAP bytes and 2 control byte */
#define SAPSAP_NOVELL	((SAP_NOVELL << 8) | SAP_NOVELL) /* ISO1 DSAP+SSAP */
#define SAPSAP_CLNS	((SAP_CLNS   << 8) | SAP_CLNS)   /* ISO1 DSAP+SSAP */
#define SNAPSNAP	((SAP_SNAP << 8) | SAP_SNAP)
#define SAPSAP_SDE      ((SAP_SDE << 8) | SAP_SDE)
#define SAPSAP_NULL	((SAP_NULL << 8) | SAP_NULL)
#define SAPSAP_VINES	((SAP_VINES << 8) | SAP_VINES)
#define SAPSAP_NOVELL_ETHER ((SAP_NOVELL_ETHER << 8) | SAP_NOVELL_ETHER)
#define SAPSAP_NETBIOS	((SAP_NETBIOS << 8) | SAP_NETBIOS)
#define SAPSAP_IP	((SAP_IP << 8) | SAP_IP)

/*
 * This is the two sap bytes, ctl byte, and hi byte of snap code (zero)
 */
#define SAP_CTL_HIZER (SNAPSNAP << 16) | (LLC1_UI << 8) | (0 << 0)


/**********************************************************************
 *
 *			  SNAP definitions.
 *
 **********************************************************************/
typedef struct snap_hdr_ {
    uchar  dsap;		/* destinations service access point */
    uchar  ssap;		/* source service access point */
    uchar  control;		/* frame type */
    uchar  oui[3];		/* snap extension */
    ushort type;		/* ethernet type field */
    uchar  data[0];
} snap_hdr;

#define SNAP_HDRBYTES  8
#define SNAP_HDRWORDS  4
#define SNAP_OUIBYTES  3		/* bytes of ISO2 SNAP OUI */

/*
 * Assigned Ethernet type codes
 */

/* The following codes are public, assigned by Xerox for the stated use.
 * See below for Cisco-assigned codes
 */

# define TYPE_PUP	   0x0200	/* PUP */
# define TYPE_XEROX_ARP	   0x0201	/* Xerox ARP on 10MB nets */
# define TYPE_XNS	   0x0600	/* XNS */
# define TYPE_IP10MB       0x0800	/* IP on 10MB net */
# define TYPE_CHAOS	   0x0804	/* Chaos on 10MB net */
# define TYPE_RFC826_ARP   0x0806	/* IP ARP */
# define TYPE_FR_ARP       0x0808	/* Frame Relay ARP */
# define TYPE_VINES	   0x0BAD	/* Vines IP */
# define TYPE_VINES_LOOP   0x0BAE	/* Vines Loppback Protocol */
# define TYPE_VINES_ECHO   0x0BAF	/* Vines Echo */
# define TYPE_MOP_BOOT	   0x6001	/* DEC MOP booting protocol */
# define TYPE_MOP_CONSOLE  0x6002	/* DEC MOP console protocol */
# define TYPE_DECNET	   0x6003	/* DECnet phase IV on Ethernet */
# define TYPE_LAT	   0x6004	/* DEC LAT on ethernet */
# define TYPE_HP_PROBE	   0x8005	/* HP Probe */
# define TYPE_REVERSE_ARP  0x8035	/* Reverse ARP */
# define TYPE_DEC_SPANNING 0x8038	/* DEC spanning tree */
# define TYPE_ETHERTALK    0x809b	/* Apple EtherTalk */
# define TYPE_AARP	   0x80f3	/* Appletalk ARP */
# define TYPE_APOLLO	   0x8019	/* Apollo domain */
# define TYPE_VINES2	   0x80C4	/* Vines IP (New - Being Phased In) */
# define TYPE_VINES_ECHO2  0x80C5	/* Vines Echo (New, Being Phased In) */
# define TYPE_NOVELL1	   0x8137	/* Novell IPX */
# define TYPE_CPP	   0x8731	/* Combinet negotiation packets */
# define TYPE_LOOP	   0x9000	/* Ethernet loopback packet */

/*
 * cisco Ethernet protocol types
 *
 * These codes are from a small pool of types assigned by Xerox to cisco
 * for our own administration.
 *
 * PLEASE NOTE:  If you need a new HDLC type code, do NOT assign it youself.
 *               Send your request to cana@cisco.com and say what protocol
 *               you need it for.  CANA will assign and commit the number.
 *
 * Cisco's range of numbers is 0x883D through 0x8843.
 *
 */
# define TYPE_ETHER_ESMP       0x883D   /* Even Simpler Management Protocol */
/* New Cisco Ethernet protocol type assignments go here */

/*
 * cisco HDLC SNAP type codes - not assigned Ethernet codes
 * 
 * PLEASE NOTE:  If you need a new HDCL type code, do NOT assign it youself.
 *               Send your request to cana@cisco.com and say what protocol
 *               you need it for.  CANA will assign and commit the number.
 *
 */
# define TYPE_HDLC_ESMP        0x0100   /* Even Simpler Management Protocol */
# define TYPE_HDLC_TAGSWITCH   0x0101   /* Tag switching */
# define TYPE_HDLC_DRIP        0x0102   /* Duplicate Ring Protocol */
# define TYPE_HDLC_VSI         0x0103   /* Virtual Switch Interface */
# define TYPE_HDLC_PAGP        0x0104   /* Port Aggregation Protocol */
/* New HDLC type assignments go here */
# define TYPE_NOVELL2          0x1A58   /* Novell IPX, standard form */
# define TYPE_CLNS             0xFEFE   /* CLNS */
# define TYPE_ESIS             0xEFEF   /* ES-IS */
# define TYPE_IEEE_SPANNING    0x4242   /* use dsap ssap values */
# define TYPE_DLSW             0x1995   /* DLSw Raw */
# define TYPE_RSRB             0x1996   /* RSRB raw */
# define TYPE_STUN             0x1997   /* Serial Tunnel */
# define TYPE_BSTUN            0x1994   /* Block Serial Tunnel */
# define TYPE_UNCOMPRESSED_TCP 0x1998   /* Uncompressed TCP (NOT normal TCP/IP!) */ 
# define TYPE_COMPRESSED_TCP   0x1999   /* Compressed TCP */
/*
 * The following definitions should be the same as those in file
 * syn_core/nmp/nmpflash/h/snap_types.h in the Synergy (Catalyst)
 * source repository (/wbu-sw/dev/synergy) 
 */
# define TYPE_CDP              0x2000   /* Cisco Discovery Protocol */
# define TYPE_CGMP             0x2001   /* Cisco Group Management Protocol */
# define TYPE_COMPRESSED_FR    0x2002	/* payload-compressed frame-relay */
# define TYPE_VTP	       0x2003	/* VLAN Trunking Protocol */
# define TYPE_DISL             0x2004   /* Dynamic Inter Switch Link (ISL) */
# define TYPE_CATALYST_IPC     0x2005   /* Inter Card communications
                                           on the Catalyst Switches */
# define TYPE_CATALYST_SYNC    0x2006   /* "sync" protocol used between
                                           Catalyst switches */

# define TYPE_BRIDGE           0x6558   /* serial line bridging */ 

/* NOTE:  All new HDLC types will be in the non-Ethernet range, above. */

/*
 * IANA assigned type cod/wbu-sw/dev/synergyes (SNAP PID)
 */
# define TYPE_NHRP	       0x0003	/* NHRP */

/*
 * Local LNM feature Type code
 */
# define TYPE_NMP              0x003E   /* NMP */

/*
 * RFC1490/FRF.3 defined type codes for LLC2/8802 routed over Frame Relay
 */
# define TYPE_L2_8022_PAD      0x4C80	/* 8022 code + PAD byte, LLC2 over FR */

/*    
 * Xerox assigned ethertypes for Tagswitching. Cisco contact is Bruce Davie.
 * Xerox contact is Neil Sembower.
 */
# define TYPE_TAG             0x8847
# define TYPE_MTAG            0x8848

#define CISCOOUI 0x0300000C	/* used in encapsulation bridging */
#define ETHEROUI 0x030000F8	/* magic cookie - frame originally ether V2 */
#define OLD_ETHEROUI 0x03000000
#define SNAP_CTL_OUI 0x03000000 /* the real name for the above */
#define OUI_MASK 0xffffff00     /* org-id mask bits in ieee address */
#define DEFAULT_OUI SNAP_CTL_OUI


/**********************************************************************
 *
 *			  Deprecated definitions.
 *
 **********************************************************************/
/*
 * LLC Header
 * This is the format for LLC-1 header, with SNAP extension.  This somewhat 
 * duplicates the information in the defines for iso2dsap, etc., in ether.h, 
 * but its more convenient for TR to have it this way.
 *
 * This works for both ISO1 and ISO2 styles.  Iso 1 uses just dsap, ssap, and
 * control.
 */

#define LLC_DSAP_OFFSET 0
#define LLC_SSAP_OFFSET 1
#define LLC_CONTROL_OFFSET 2
#define LLC_ORG_OFFSET 3
#define LLC_TYPE_OFFSET 6
#define LLC_LSAP_OFFSET LLC_TYPE_OFFSET
#define LLC_DATA_OFFSET 8

typedef struct _llc1_hdr_t {
    uchar dsap;			/* destinations service access point */
    uchar ssap;			/* source service access point */
    uchar control;		/* frame type */
    uchar oui[3];		/* snap extension */
    ushort type;		/* ethernet type field */
    uchar data[0];
} llc1_hdr_t;

#define TYPECODE_MIN_ACL      1
#define TYPECODE_MAX_ACL    100
#define MACADDR_MIN_STD_ACL 101
#define MACADDR_MAX_STD_ACL 200
#define MACADDR_MIN_EXT_ACL 201
#define MACADDR_MAX_EXT_ACL 300

/**********************************************************************
 *
 *			   Inline Functions
 *
 **********************************************************************/

/*
 * ieee_copy
 */
static inline void ieee_copy (uchar const *src, uchar *dst)
{
    PUTSHORT(dst+0, GETSHORT(src+0));
    PUTLONG(dst+2,  GETLONG(src+2));
}

/*
 * ieee_move
 *
 * Safely move an address in a packet
 */
static inline void ieee_move (uchar const *src, uchar *dst)
{
    ushort a;
    ulong b;
    
    a = GETSHORT(src+0);
    b = GETLONG(src+2);
    PUTSHORT(dst+0, a);
    PUTLONG(dst+2, b);
}

/*
 * ieee_copy_sans_rii
 */
static inline void ieee_copy_sans_rii (uchar const *src, uchar *dst)
{
    PUTSHORT(dst+0, GETSHORT(src+0)&0x7FFF);
    PUTLONG(dst+2,  GETLONG(src+2));
}

/*
 * ieee_zero
 */
static inline void ieee_zero (uchar *addr)
{
    PUTLONG(addr, 0L);
    PUTSHORT(addr+4, 0);
}

/*
 * ieee_copy_oui
 */
static inline void ieee_copy_oui (uchar const *src, uchar *dst)
{
    PUTSHORT(dst+0, GETSHORT(src+0));
    dst[2] = src[2];
}

/*
 * ieee_swap
 */
static inline void ieee_swap (uchar const *src, uchar *dst)
{
    int i;
    
    for (i = 0; i < IEEEBYTES; i++)
	dst[i] = bit_swaps[src[i]];
}

/*
 * ieee_equal
 *
 * Given two 48 bit IEEE addresses, return equality predicate.
 */
static inline boolean ieee_equal (uchar const *addr1, uchar const *addr2)
{
    if ((addr1 == NULL) || (addr2 == NULL))
	return (FALSE);
    if ((GETLONG(&addr1[2])  != GETLONG(&addr2[2])) ||
	(GETSHORT(&addr1[0]) != GETSHORT(&addr2[0])))
	return(FALSE);
    return(TRUE);
}

/*
 * ieee_equal_sans_rii
 *
 * Given two 48 bit IEEE addresses, return equality predicate.  Ignore
 * the Token Ring RII bit in the comparison.
 */
static inline boolean ieee_equal_sans_rii (uchar const *addr1,
					   uchar const *addr2)
{
    if ((addr1 == NULL) || (addr2 == NULL))
	return (FALSE);
    if ((GETLONG(&addr1[2])         != GETLONG(&addr2[2])) ||
	((GETSHORT(&addr1[0])&0x7FFF) != (GETSHORT(&addr2[0])&0x7FFF)))
	return(FALSE);
    return(TRUE);
}

/*
 * ieee_equal_oui
 *
 * Given two 32 bit SNAP OUI fields, return equality predicate.
 */
static inline int ieee_equal_oui (uchar const *addr1, uchar const *addr2)
{
    if (addr1 == addr2)
	return(TRUE);

    return((GETLONG(addr1)&0xFFFFFF00) == (GETLONG(addr2)&0xFFFFFF00));
}

/*
 * ieee_compare
 *
 * Given two 48 bit IEEE addresses, return equality predicate.
 */
static inline int ieee_compare (uchar const *addr1, uchar const *addr2)
{
    ulong diff;

    if (addr1 == addr2)
	return(0);

    diff = GETSHORT(&addr1[0]) - GETSHORT(&addr2[0]);
    if (diff)
	return(diff);
    diff = GETLONG(&addr1[2]) - GETLONG(&addr2[2]);
    return(diff);
}

/*
 * is_ieee_zero
 */
static inline boolean is_ieee_zero (uchar const *addr)
{
   return(ieee_equal(addr, zeromac));
}

/*
 * is_ieee_bcast
 */
static inline boolean is_ieee_bcast (uchar const *addr)
{
   return(ieee_equal(addr, baddr));
}

/*
 * 'arithmetic' operations for ranges of virtual mac addresses
 *
 * Adding and subtracting positive integers only
 */
 
/*
 * ieee_add
 */

static inline void ieee_add (uchar *addr, uint sum, uchar *result)
{
    ushort a;
    ulong b;
    
    a = GETSHORT(addr+0);
    b = GETLONG(addr+2);
    
    b += sum;
    if (b < GETLONG(addr+2)) {  /* wrapped */
    	a++;
    }
    PUTSHORT(result+0, a);
    PUTLONG(result+2, b);
}
 
/*
 * ieee_subtract
 */

static inline void ieee_subtract (uchar *addr, uint diff, uchar *result)
{
    ushort a;
    ulong b;
    
    a = GETSHORT(addr+0);
    b = GETLONG(addr+2);
    
    b -= diff;
    if (b > GETLONG(addr+2)) {  /* wrapped */
    	a--;
    }
    PUTSHORT(result+0, a);
    PUTLONG(result+2, b);
}

/*
 * ieee_mask
 * *addr |= *mask;
 */
static inline void ieee_mask (uchar *addr, uchar *mask)
{
    ushort a;
    ulong b;
    
    a = GETSHORT(addr+0);
    b = GETLONG(addr+2);
    
    a |= GETSHORT(mask+0);
    b |= GETLONG(mask+2);

    PUTSHORT(addr+0, a);
    PUTLONG(addr+2, b);
}

/**********************************************************************
 *
 *			   Externs
 *
 **********************************************************************/

extern long sap2link(uchar);
extern long type2link(ushort);
extern ushort link2snaptype(long);
extern void llc_input(paktype* pak);
boolean is_well_known_sap(uchar sap);
extern void llc_clear_rii(hwaddrtype *src, hwaddrtype *dest, idbtype *idb);

/* -----------------------------------------------------------------------
 *
 *				SAP REGISTRY
 *
 * Constants and structures
 */

/* 
 * Values for sap_flags 
 */

/* 
 * If these bits are set the dlc can reply automatically to the
 * Commands
 */

#define SAP_TEST_AUTORESPONSE		0x0001
#define SAP_IEEE_XID_AUTORESPONSE	0x0002
#define SAP_SNA_XID_AUTORESPONSE	0x0004

/* 
 * If these bits are set the dlc will pass Commands and Responses up to cls.
 */

#define SAP_TEST_FORWARD		0x0010
#define SAP_IEEE_XID_FORWARD		0x0020
#define SAP_SNA_XID_FORWARD		0x0040

#define SAP_CLS_ACT_SAP                 0x0100 /* activate.sap on this entry */
#define SAP_PRE_REG_SAP                 0x0200 /* 'well-known' for legacy code */

#define MAX_SAP_VALUE	256
#define MAX_SAP_ENTRIES	(MAX_SAP_VALUE/2)

/* different handlers for each class of input */

typedef void (*sap_handle_test_t)(idbtype *idb, paktype *pak, boolean usesa);
typedef void (*sap_handle_ieee_xid_t)
    (idbtype *idb, paktype *pak, boolean usesa);
typedef void (*sap_handle_sna_xid_t)(idbtype *idb, paktype *pak, 
	void *usapid, boolean isresponse);

typedef struct sap_entry_t_
{
    ulong	sap_flags;		/* what the handling is 	*/
    void*	usapid;			/* who to pass sap info to 	*/
    llctype*	llc_list;		/* list of llcs on this sap     */

    /* routines to handle XID and TEST packets */

    sap_handle_test_t		sap_handle_test;
    sap_handle_ieee_xid_t	sap_handle_ieee_xid;
    sap_handle_sna_xid_t	sap_handle_sna_xid;

    /*
     * When responding to IEEE XID requests, these values control the
     * contents of bytes 1 and 2. They are set by the activateSap.req
     * CLSI primitive.
     *
     * First byte of IEEE XID is always XID_FMT_STD (0x81).
     */
    
    uchar	xid_class;		/* XID_CLASS_TYPE1 or _TYPE2 */
    uchar	xid_window;		/* type 2 receive window size */
} sap_entry_t;

struct sap_registry_t_
{
    sap_entry_t*	bridging_sap;	/* This SAP entry is used for bridging SAPs
					 * In particular, this is used by DLSw
					 */
    sap_entry_t*	dlc_saps[MAX_SAP_ENTRIES]; /* A NULL pointer */
						   /* in this array */
						   /* indicates that */
						   /* the sap is not active. */
    llctype		**llchashtablep;/* pointer to hash table - only 
					 * allocate space on req open station
					 * on a per device basis
					 */
};

sap_registry_t *sap_create_registry(idbtype *idb);
sap_registry_t *sap_get_registry(idbtype *idb);
int sap_register(sap_registry_t *sap_registry, uchar sap,
			  sap_entry_t* sap_entry);
sap_entry_t *sap_deregister(sap_registry_t *sap_registry, uchar sap);
sap_entry_t *sap_get_entry(idbtype *idb, uchar sap);
void *sap_get_usap_id(sap_registry_t *sap_registry, uchar sap);

int sap_register_bridging(sap_registry_t *sap_registry, 
			  sap_entry_t *sap_entry);
sap_entry_t *sap_deregister_bridging(sap_registry_t *sap_registry);
sap_entry_t *sap_get_bridging_entry(idbtype *idb);

#endif  __IEEE_H__
