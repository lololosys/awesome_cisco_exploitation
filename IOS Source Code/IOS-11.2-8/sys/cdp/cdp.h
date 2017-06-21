/* $Id: cdp.h,v 3.3.44.11 1996/09/11 02:56:12 perryl Exp $
 * $Source: /release/112/cvs/Xsys/cdp/cdp.h,v $
 *------------------------------------------------------------------
 * Support for CDP (Cisco Discovery Protocol)
 *
 * October 1994, Arun Sastry
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: cdp.h,v $
 * Revision 3.3.44.11  1996/09/11  02:56:12  perryl
 * CSCdi65039:  vtemplate loses cdp enable value after reload
 * Branch: California_branch
 * nvgen "cdp enable" for vtemplate if it is cdp enable.
 *
 * Revision 3.3.44.10  1996/09/04  16:46:29  dino
 * CSCdi67769:  Add CDP_REPEATER_DEVICE capability constant
 * Branch: California_branch
 *
 * Revision 3.3.44.9  1996/07/18  23:43:10  bcole
 * CSCdi62789:  CDP is not supported on isdn and asynch interfaces
 * Branch: California_branch
 *
 * Cosmetic cleanup (real fix in CSCdi62972).
 *
 * Revision 3.3.44.8  1996/07/15  20:00:28  irfan
 * CSCdi62972:  cdp disabled on dialers causes problems for auto stub
 * routing
 * Branch: California_branch
 * cdp thinks nothing is dialer
 *
 * Revision 3.3.44.7  1996/07/08  18:39:33  tylin
 * CSCdi61348:  CDP ALWAYS sends FULL MTU size datagram (feature/bug?)
 *    Allocate data buffer with smaller of interface MTU and Huge data
 *    size.
 * Branch: California_branch
 *
 * Revision 3.3.44.6  1996/06/28  07:20:43  tylin
 * CSCdi60805:  7513 restarted by error - a SegV exception, PC 0x600EBE98
 *    Avoid sign extension problem for cdp_hash().
 * Branch: California_branch
 *
 * Revision 3.3.44.5  1996/06/17  23:02:51  irfan
 * CSCdi60567:  cdp enabled on dialers
 * Branch: California_branch
 * Put it back as it was in Arkansas.
 *
 * Revision 3.3.44.4  1996/04/30  21:29:53  dawnli
 * CSCdi53927:  Show cdp entry should accept wildcards
 * Branch: California_branch
 *
 * Revision 3.3.44.3  1996/04/27  05:45:31  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.3.10.2  1996/04/26  00:03:50  syiu
 * Branch: Dial1_branch
 * Dial1_branch to IosSec_postmerge_960419
 *
 * Revision 3.3.10.1  1996/02/23  20:11:54  lbustini
 * Branch: Dial1_branch
 * Move dialer fields out of the idb.
 *
 * Revision 3.3.44.2  1996/04/15  21:13:01  bcole
 * CSCdi54649:  IP Host Modularity, IP Automatic Stub Routing feature
 * commit
 * Branch: California_branch
 *
 * Revision 3.3.44.1  1996/03/18  19:01:48  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.2  1996/03/13  01:12:26  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups
 *
 * Revision 3.3.14.1  1996/02/20  13:39:16  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.3  1995/12/11  21:27:57  asastry
 * CSCdi35119:  Fix ATM error messages (for CDP)
 *
 * Revision 3.2  1995/11/17  08:49:39  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:05:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/17  23:15:59  hampton
 * Miscellaneous timer cleanups for CDP.  [CSCdi37215]
 *
 * Revision 2.2  1995/06/21 02:52:02  gstovall
 * CSCdi35731:  Cleanup unused cruft in idb
 * Nuke some unused vectors and variables from the hwidb.
 *
 * Revision 2.1  1995/06/07  20:14:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __CDP_H__
#define __CDP_H__

#define cdp_pabuginf if (cdp_pkt_debug) buginf
#define cdp_evbuginf if (cdp_event_debug) buginf
#define cdp_adjbuginf if (cdp_adj_debug) buginf
#define cdp_ipbuginf if (cdp_ip_debug) buginf

/*
 * Maximum/minimum values.
 */

#define CDP_MAX_XMIT_INTERVAL 15*60             /* Max 'hello' interval */
#define CDP_MIN_XMIT_INTERVAL 5                 /* Min 'hello' interval */
#define CDP_DEF_XMIT_INTERVAL 60                /* Default transmit interval */

#define CDP_MAX_TTL 0xff                        /* Max TTL is 255 */
#define CDP_MIN_TTL 10                          /* Min TTL is 10 */
#define CDP_DEF_TTL 3*60                        /* Default value is 3 min. */

#define CDP_MAX_DATA_SIZE   MAXDATAGRAMSIZE	/* Largest datagram size */

#define CDP_VERSION 0x01

/* CDP Statistics */

#define CDP_INPUT                   0
#define CDP_OUTPUT                  (CDP_INPUT+1)
#define CDP_HDRSYNTAX               (CDP_OUTPUT+1)
#define CDP_CHECKSUM                (CDP_HDRSYNTAX+1)
#define CDP_ENCAPSFAILED            (CDP_CHECKSUM+1)
#define CDP_BADPACKET               (CDP_ENCAPSFAILED+1)
#define CDP_NOMEM                   (CDP_BADPACKET+1)
#define CDP_FRAG                    (CDP_NOMEM+1)

#define CDP_MAXTRAFFIC              (CDP_FRAG+1)

#define CDP_LONGTIME 70000	    /* arbitrary large number */
#define CDP_NEXT_ACTIVATION 10      /* Activate CDP every 10 sec */

/*
 * CDP packet format.
 */
typedef struct cdp_hdrtype_ {
    uchar version;            /* version */
    uchar ttl;                /* TTL */
    ushort checksum;          /* 1's complement of the 1's complement sum */
    uchar item[1];            /* TLV's start here */
} cdp_hdrtype;

#define CDP_MIN_HEADER_LENGTH 4   /* version, ttl, and checksum */
#define CDP_HEADER_OVERHEAD 4
#define cdpheadstart(ptr) ((ptr)->network_start)

/*
 * TLV - all info in CDP is passed around as type-length-value (TLV).
 */

typedef struct {
    ushort code;
    ushort length;
    uchar value[1];
} cdp_info_type;

#define CDP_TLV_OVERHEAD 4              /* 2 bytes each for type and length */
/*
 * Type codes
 */
#define CDP_DEVICE_NAME 0x0001          /* identifies the device */
#define CDP_ADDRESS_TYPE 0x0002         /* list of addresses this device has */
#define CDP_PORT_ID_TYPE 0x0003         /* port CDP packet was sent out on */
#define CDP_CAPABILITIES_TYPE 0x0004    /* funct. capabilities of this device */
#define CDP_VERSION_TYPE 0x0005         /* version */
#define CDP_PLATFORM_TYPE 0x0006        /* hardware platform of this device */
#define CDP_IPPREFIX_TYPE 0x0007	/* An IP network prefix */

/*
 * Remember to modify this macro if you add more type codes!!
 */
#define CDP_MAX_TYPE CDP_IPPREFIX_TYPE

#define CDP_TYPE_KNOWN(code) \
    ((code >= CDP_DEVICE_NAME) && (code <= CDP_MAX_TYPE))

/*
 * Value codes for all applicable types
 */

/* Addresses */

/*
  The address TLV looks as follows:

          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |                    Number of addresses                        |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |                   IDRP encoded address                        |
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	  An address is encoded in IDRP format:

	   0                   1                   2                   3
           0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |       PT      |    PT Length  |    Protocol (variable) ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
          |        Address length         |    Address (variable) ...
          +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+

	  PT: Protocol type 
	      1 = NLPID format 
              2 = 802.2 format

	  PT Length: 
	      Length of protocol field, 1 for PT = 1, and either 3 or 8 for
	      802.2 format depending if SNAP is used for PT = 2.

	      The encodings for the other protocols have the following format:

	               cisco_snap 000000 TYPE_<protocol>,

	      where "TYPE_<protocol>" is the assigned Ethernet type code for
	      the particular protocol. e.g. for DECnet the encoding is
	      AAAA03 000000 6003.

*/

#define CDP_NLPID_FORMAT 1
#define CDP_8022_FORMAT 2

#define CDP_IP_NLPID 0xCC
#define CDP_CLNS_NLPID 0x81

#define CDP_NLPID_LEN 1
#define CDP_8022_FORMAT_LEN SNAP_HDRBYTES
#define CDP_ADDR_LEN_FIELD_LEN 2                   /* Length of 'address
						      length' field */

#define CDP_NLPID_INFO_OVERHEAD 3                  /* PT + PT length +
						      Protocol value fields */
#define CDP_8022_INFO_OVERHEAD (SNAP_HDRBYTES+2)   /* PT + PT length +
						      Protocol value fields */

/* Port ID */

/* 
 * Allow max port ID string length to be 20. It's not worth mallocing
 * space to hold a string that might usually be 10 bytes long.
 */
#define CDP_MAX_PORT_ID_STR_LEN 20

/*
 * Capabilities.
 *
 * Please update "CDP_MAX_CAPABILITIES" if you modify this
 * list of capabilities!! Thank you.
 */

#define CDP_ROUTER_DEVICE 0x01
#define CDP_TB_DEVICE 0x02
#define CDP_SRB_DEVICE 0x04
#define CDP_SWITCH_DEVICE 0x08
#define CDP_HOST_DEVICE 0x10
#define CDP_IGMP_DEVICE 0x20
#define CDP_REPEATER_DEVICE 0x40

#define CDP_MAX_CAPABILITIES 7

/* Platform */

/* 
 * Allow max platform string length to be 40. It's not worth mallocing
 * space to hold a string that might usually be 10 bytes long.
 */
#define CDP_MAX_PLATFORM_STR_LEN 40

/*
 * Address info consists of a count of the # of addresses, followed by
 * an "array" of addresses of type 'cdp_address_structure' (which looks like
 * 'struct addrtype_')
 */
typedef struct cdp_addr_info_ {
    ulong num_addr;                      /* number of addresses */
    uchar data[0];    
} cdp_addr_info; 

#define CDP_ADDRESS_OVERHEAD 4           /* (sizeof(ulong)) */

typedef struct cdp_addr_structure_ {
    uchar type;
    uchar length;
    uchar PACKED(address[ADDRLEN_MAX]);
} cdp_addr_structure;

#define CDP_ADDRSTRUCT_OVERHEAD 2	/* sizeof(Type + len) */

#define CDP_CAPAB_FIELD_LEN  4           /* (sizeof(ulong)) */
/*
 * Cache entry structure
 */

typedef struct cdp_cache_type_ {
    struct cdp_cache_type_ *next;       /* pointer to next entry */
    sys_timestamp expiration_time;      /* when we forget this entry */
    int entry_name_len;			/* length of entry name */
    uchar *entry_name;                  /* identifies this entry */
    cdp_addr_info *addr;                /* buffer for address info */
    uchar port_id[CDP_MAX_PORT_ID_STR_LEN]; /* neighbor's outgoing interface */
    ulong capabilities;                 /* functional capabilities */
    uchar *version;                     /* version information */
    uchar platform[CDP_MAX_PLATFORM_STR_LEN];  /* platform information */
    idbtype *idb;                       /* which intrface this neighbr is on */
    ulong device_number;	        /* device number of this device */
} cdp_cache_type;

typedef struct cdp_entry_link_ {
    struct cdp_entry_link_	*next;
    cdp_cache_type		*node;
} cdp_entry_link;


/*
 * Hash table to store entries
 */
typedef unsigned char	cdp_hash_type;	/* Cache table index from 0 to 255 */
cdp_cache_type *(cdp_table[NETHASHLEN]);

/*
 * On some CDP supported interfaces, e.g.  virtual template and virtual
 * access interfaces, the default is to have CDP disabled. Virtual template 
 * and virtual access interfaces are mostly used for remote access, and 
 * running CDP over these interfaces is not typical.
 *
 * For such interfaces, CDP should be disabled by default, and if the user
 * does turn on CDP, we should allow it. Furthermore, we should nvgen the
 * (non-default) configuration i.e. "cdp enable" for this particular case.
 *
 * Therefore on the next reload, we can figure out whether CDP should be
 * enabled or disabled on this interface.
 *
 * CDP is set to disable by default for FR p2mp now.
 */
#define CDP_DEFAULT_DISABLE_INT(idb)		    \
    (((idb)->hwptr->status & IDB_VTEMPLATE) ||	    \
     (is_frame_relay((idb)->hwptr) && !is_p2p(idb)))

#define	CDP_DEFAULT_ENABLE_INT(idb)		    \
    (cdp_supported_int(idb) && !CDP_DEFAULT_DISABLE_INT(idb))
				      

typedef boolean (*cdp_add_func_t)(cdp_info_type *, idbtype *, ushort *);
typedef void (*cdp_decode_func_t)(cdp_info_type *, cdp_cache_type *,
				     boolean);

#endif  __CDP_H__

