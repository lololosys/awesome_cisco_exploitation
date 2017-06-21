/* $Id: vip_af.h,v 3.3.10.4 1996/05/31 22:34:04 feisal Exp $
 * $Source: /release/112/cvs/Xsys/src-vip/vip_af.h,v $
 *------------------------------------------------------------------
 * vip_af.h - VIP software address filter support
 *
 * Michael Beesley, June 1995
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: vip_af.h,v $
 * Revision 3.3.10.4  1996/05/31  22:34:04  feisal
 * CSCdi56695:  ISL Broken on certain card
 * Branch: California_branch
 *
 * Revision 3.3.10.3  1996/04/05  04:17:46  sjacobso
 * CSCdi52859:  New AF and new ISL code not fully integrated
 * Branch: California_branch
 * Cause dec21140 address filter write code to set the correct
 * ISL enable flags on receipt of an ISL entry.
 *
 * Revision 3.3.10.2  1996/03/29  04:06:07  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.3.10.1  1996/03/29  00:45:54  sjacobso
 * CSCdi45152:  PERFORMANCE: 1FE runs in promiscuous mode instead of
 * filtering
 * Branch: California_branch
 * Add code to allow dec21140 chip to run in perfect
 * address filtering mode whenever possible.  Modify
 * address filter write code to record information
 * about address filter entries that may affect perfect
 * filter operation.  Interface VIP-specific software
 * address filter to RVIP/SVIP/Predator common routines.
 *
 * Revision 3.3  1996/02/23  20:03:01  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.2  1995/11/17  18:51:37  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:29:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:29:33  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/28  23:44:35  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Definitions
 */
#define VIP_MAX_AF_ENTRIES             		32
#define VIP_AF_ENTRY_SIZE              		16
#define VIP_AF_BYTES		(sizeof(vip_af_entry_t) * VIP_MAX_AF_ENTRIES)
#define VIP_AF_ALIGNMENT             		32

/*
 * Defines for address filter / classifier code.
 */

/*
 * These bits appear in the filter word in the VIP address filter structure.
 */
#define AF_CLNS_PROTOCOL        0x8000
#define AF_XNS_PROTOCOL         0x4000
#define AF_DECNET_PROTOCOL      0x2000
#define AF_DOD_IP_PROTOCOL      0x1000
#define AF_HSRP_STATION_ADDR    0x0800
#define AF_BRIDGING_ADDRESS     0x0400
#define AF_MULTICAST_ADDRESS    0x0200
#define AF_STATION_ADDRESS      0x0100
#define AF_802_10		0x0004
#define AF_ISL			0x0002
#define AF_ISL_802_VALID       	0x0001
 
/*
 * Masks the MAC bits from the filter word.
 */
#define AF_FCI_MAC_BITS         0x0F07
#define AF_FCI_NO_PROTOCOL      0x0FFF
#define AF_DODIP_STATION	(AF_DOD_IP_PROTOCOL | AF_STATION_ADDRESS)

/*
 * Defines the offset of various words from the start of the
 * software address filter entry.
 */
#define AF_FILTER_OFFSET     	12
#define AF_STATUS_OFFSET     	14

/*
 * These bits appear in the status word in the VIP address filter structure.
 */
#define AF_STAT_SPECIAL_HANDLING	0x8000

#define AF_STAT_ISL_CONFIGURED		0x0200

#define AF_STAT_HSRP_CONFIGURED_SPL	0x0800
#define AF_STAT_BRIDGING_CONFIGURED	0x0400
#define AF_STAT_MULTICAST_RANGE_SPL	0x0200
#define AF_STAT_MULTICAST_OVERFLOW	0x0100

#define AF_STAT_MULTICAST_RANGE		0x0020

#define AF_STAT_HSRP_CONFIGURED		0x0008
#define AF_STAT_BROADCAST_CONFIGURED	0x0004
#define AF_STAT_MULTICAST_CONFIGURED	0x0002
#define AF_STAT_STATION_CONFIGURED	0x0001

/*
 * These should be defined in dec21140 or amdp2 header files.
 */
#define DEC21140_MULTICAST_BIT		0x0400
#define AMDP2_PHYS_MATCH_BIT		0x0040


#if !defined(ASMINCLUDE)

/*
 * VIP Address filter entry and structure
 */
typedef struct vip_af_entry_t_ {
    ushort match[3];
    ushort mask[3];
    ushort filter;
    ushort status_or_pad;
} vip_af_entry_t;

typedef struct vip_sw_af_t_ {
    vip_af_entry_t af_entry[VIP_MAX_AF_ENTRIES];
} vip_sw_af_t;

/*
 *      Invalidate cache line. Used to clear some cache entries
 *      when distributed switching (for looking at the packet header).
 */
static inline void vip_cache_invalidate_line (uchar *cline_ptr)
{
    asm volatile(".set noreorder");
    asm volatile(".set mips3");
 
    /* Invalidate cache line */
 
    asm volatile(" cache 17, 0(%0)"
		 : /* no output */
		 : "r" (cline_ptr));
 
    asm volatile(".set mips2");
    asm volatile(".set reorder");
}

/*
 * Prototypes
 */ 
extern boolean 	vip_address_filter_allocate(hwidbtype *);
extern void 	vip_address_filter_init(hwidbtype *);
extern void 	vip_address_filter_print(pas_cbtype *);
extern ushort	vip_ether_filter_classify(uchar *, vip_af_entry_t *,
                                                uint, ulong);

#endif /* ASMINCLUDE */

/* end of file */
