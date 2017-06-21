/* $Id: asm_4k_vip_af.S,v 3.3.10.3 1996/03/29 04:06:16 sjacobso Exp $
 * $Source: /release/111/cvs/Xsys/asm-mips/asm_4k_vip_af.S,v $
 *------------------------------------------------------------------
 * asm_4k_vip_af.S - VIP software address filter support
 *
 * Michael Beesley, June 1995
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: asm_4k_vip_af.S,v $
 * Revision 3.3.10.3  1996/03/29  04:06:16  sjacobso
 * CSCdi52266:  4E/8E receive promiscuously, hurting performance
 * Branch: California_branch
 * Change write_address_filter code to examine address filter
 * entries for important status, and to store that status in the
 * software address filter.  The correct common routines for
 * adding MAC addresses are called.  Also, fix Predator logical
 * address filter code to work correctly with HSRP addresses.
 *
 * Revision 3.3.10.2  1996/03/29  00:46:00  sjacobso
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
 * Revision 3.3.10.1  1996/03/21  22:27:27  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.4  1996/03/18  23:50:04  mbeesley
 * Branch: ELC_branch
 * Sync to V111_1_3
 *
 * Revision 3.2.2.3  1996/03/07  17:27:37  getchell
 * CSCdi50953:  viper dfs mods
 * Branch: ELC_branch
 *
 * Revision 3.2.2.2  1996/02/16  00:06:49  getchell
 * CSCdi49143:  address filter classification cache invalidation
 * investigation
 *              Invalidate cache line before reading instead of after.
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/10  00:25:38  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.3  1996/02/23  20:01:18  feisal
 * CSCdi45142:  Support VLAN ISL for new 1FE port adapter
 *
 * Revision 3.2  1995/11/17  08:40:57  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:55:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:49:05  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/28  23:44:41  mbeesley
 * Add placeholder files for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define ASMINCLUDE

#include "asm_4k_vip.h"
#include "../src-vip/vip_af.h"

#if     defined(SVIP)
#include "../src-svip/svip_pcmap.h"
#endif

/*
 * vip_ether_filter_classify:
 * Filter and classify an ethernet packet
 */
 # ushort vip_ether_filter_classify (uchar *datagramstart,
 #				     vip_af_entry_t *ptr,
 #				     uint af_entries,
 #				     ulong *rxtype);
 #
FRAME(vip_ether_filter_classify,sp,0,ra)
	.set	noreorder

# Parameters:
#   a0:  cached address of start of packet
#   a1:  cached address of address filter
#   a2:  no. of entries in the filter
#   a3:  pointer to rxtype (unused for the moment)
#
# Return value:
#   v0:  0 if no match found, else filter bits
#   v1:  
#

#if	defined(SVIP)
	/* The delay from this cache line invalidation can be avoided */
	/* by checking rxtype under specific circumstances */	
        cache   4*4+1,0(a0)             

        cache   4*4+1,12(a0)            # Invalidate cache line before reading
#endif

#if     defined(RVIP)

	/* lock out interrupts */
	mfc0	t6,CP0_SREG
	addiu	t5,zero,0xfffe
	and	t5,t5,t6
	mtc0	t5,CP0_SREG

	/* Do an uncached read to flush the pipes */
	lui	t7,0xa000
	lw	t7,0x24c(t7)

	/* same note applies to this cache line */
        cache   4*4+1,0(a0)
        cache   4*4+1,12(a0)            # Invalidate cache line before reading

	/* restore status register */
	mtc0	t6,CP0_SREG
#endif

	move	t7,zero			# zero all of t7
	ldr	t7,5(a0)		# load MAC DA as cached read

#
# t7 has the dest mac address from packet
# a0 has a ptr to offset 12 of received packet which is 4 bytes beyond
#   a 32 byte boundary (this is different on VIP)
# a2 has count of entries to search 
# a1 points to the current entry in the address filter
#
	and	t6,zero,zero		# Zero t6

search_filter:
	ldr	t6,5(a1)		# get compare value from address filter
	lhu	t5,6(a1)		# get upper bytes of mask value into t5
	dsll	t5,t5,32		# Shift upper bytes left 32
	ldr	t5,11(a1) 		# Get rest of AF mask into t5

#	
# This is new.  Or the address filter mask into the AF match value
# before doing the comparison.  Previously, the mask must have been
# ored into the match in the system code before downloading.  Since
# this change is backwards compatible, we do not need to be concerned.
#
	or	t6,t5,t6		# Or mask into match before compare

#	
# t6 has AF entry compare value | AF entry mask
# t5 has AF entry mask 
#
	or	t3,t7,t5		# Or mask with packet MAC DA
	bne	t3,t6,chk_for_more_entries
	and	v0,zero,zero		# Zero v0, the filter bits return

#
# We have a match - MAC DA matches AF entry compare value | AF entry mask.
#
	lhu	t6,AF_FILTER_OFFSET(a1)	# Read filter bits from address filter

#
# t6 contains the filter bits from the matching address filter entry
#
# Now do the additional filtering / packet clasification.
# filtering pulled directly from if_hes_ether
#
# t2 will contain four bytes from the frame starting at ethertype.
#
# IP Classifier
#
	andi	v0,t6,AF_DOD_IP_PROTOCOL # Test to see if IP is configured
	beqz	v0,not_an_ip_packet	# Pretend Not IP if IP not configured
        lwu     t2,0+12(a0)             # get etype, ivhl and TOS
        lui     t0,0x0800
        ori     t0,t0,0x45FF            # only accept 0x080045xx
        ori     t3,t2,0x00FF            # Or packet TOS to 0xFF
        bne     t0,t3,not_an_ip_packet
	lbu	t3,10+12(a0)		# get the ttl
	addiu	t3,t3,0xffff		# for ttl 0 and 1 dont mark as ip
	blez	t3,bad_hdr
	lbu	t3,18+12(a0)		# Ignore addr 255.xx.xx.xx
	addiu	t3,t3,0xff01
	beqz	t3,bad_hdr
#
exit_search_mac_match:
	andi	t6,t6,AF_FCI_MAC_BITS	# Mask off all but MAC match bits
exit_search:

	j	ra			# Return
	or	v0,t6,v0

chk_for_more_entries:
	addiu	a2,a2,-1

	bnez	a2,search_filter
	addiu	a1,a1,16		# skip to next entry
#
# the search failed
#
	j	ra			# Return
	and	v0,zero,zero		# indicate no match

bad_hdr:
	j	exit_search_mac_match
	and	v0,zero,zero

#
# XNS Classifier
#
# t2 contains four bytes from the frame starting at ethertype.
# t1 will contain the two bytes of ethertype from the frame.
#
not_an_ip_packet:
	andi	v0,t6,AF_XNS_PROTOCOL	# perform xns filtering
	beqz	v0,not_an_xns_packet
	srl	t1,t2,16		# Shift etype into low two bytes
	li	t0,0x0600
	bne	t0,t1,not_an_xns_packet
	lbu	t4,6+12(a0)		# exclude hop counts > 0xf
	addiu	t4,t4,0xfff1
	bgez	t4,bad_hdr
	lbu	t4,12+12(a0)
	addiu	t4,t3,0xff01		# Ignore addr 255.xx.xx.xx.xx.xx
	beqz	t4,bad_hdr
	nop
	j	exit_search_mac_match	# xns filtering match
	
#
# DECnet Classifier
#
not_an_xns_packet:
	andi	v0,t6,AF_DECNET_PROTOCOL # perform decnet filtering
	beqz	v0,not_a_dec_packet
	li	t0,0x0603
	bne	t0,t3,not_a_dec_packet
	andi	t3,t2,0xFFFF		# Mask off all but 2 bytes past etype
	li	t0,0x0081
	bne	t0,t3,bad_hdr
	nop
	j	exit_search_mac_match	# a match for decnet filtering

#
# CLNS Classifier
#
not_a_dec_packet:
	andi	v0,t6,AF_CLNS_PROTOCOL	# perform clns filtering
	beqz	v0,not_clns_packet
	li	t0,0xfefe
	andi	t3,t2,0xFFFF		# Mask off all but 2 bytes past etype
	bne	t0,t3,bad_hdr
	nop
	j	exit_search_mac_match	# a match for clns filtering
	
#
# ISL Classifier
#
not_clns_packet:
	andi	v0,t6,AF_ISL
	beqz	v0,not_isl_packet
	ori	v0,v0,AF_ISL_802_VALID 
	j	exit_search_mac_match	# a match for isl filtering
	nop
#
# 802_10 Classifier
#
not_isl_packet:
	and	v0,t6,AF_802_10
	beqz	v0,not_802_10
	nop

not_802_10:
	j	exit_search_mac_match
	and	v0,zero,zero




	.set	reorder
ENDFRAME(vip_ether_filter_classify)

/* end of file */
