/* $Id: smf_private.h,v 3.3.4.1 1996/08/28 13:09:13 thille Exp $
 * $Source: /release/112/cvs/Xsys/smf/smf_private.h,v $
 *------------------------------------------------------------------
 * Software MAC Address Filter (SMF) private definitions.
 *
 * May 1995, Tony Speakman
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The SMF is intended for use when an interface is listening
 * promiscuously to match and dispose of received packets based
 * upon their destination MAC addresses in an efficient way.
 * In particular, it provides a facility that may be used on a
 * per-interface basis in support of Concurrent Routing and
 * Bridging (CRB) and eventually Integrated Routing and Bridging (IRB).
 *
 *------------------------------------------------------------------
 * $Log: smf_private.h,v $
 * Revision 3.3.4.1  1996/08/28  13:09:13  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3  1996/03/07  23:31:37  speakman
 * CSCdi48222:  SMF creates chunk storage even when unconfigured
 * Replace SMF chunk memory with managed memory and fix the original
 * problem with a simple check for onintstack.
 *
 * Revision 3.2  1995/11/17  18:52:29  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:12:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/10/26  00:28:59  speakman
 * CSCdi41837:  SMF calls malloc and free at interrupt (SYS-3-INVMEMINT)
 * Convert SMF to use a chunk instead of malloc() and free().
 *
 * Revision 2.2  1995/06/20  07:17:56  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  22:44:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This hash function on the LSByte of the OUI and the LSByte
 * avoids clustering addresses that have the same value in their
 * two low-order bytes, an attribute characteristic of the kinds
 * of addresses typically found in this filter.
 */

#define SMF_HASH(mac_addr) ((mac_addr[IEEEBYTES-4]) ^ (mac_addr[IEEEBYTES-1]))

struct smf_entry_type_ {
    struct smf_entry_type_ *smf_entry_next;
    union {
	uchar byte[8];
	ushort sword[4];
	ulong lword[2];
    } smf_entry_mac_addr;	/* 48-bit IEEE MAC address */
    ulong smf_entry_result;	/* type of address & disposition of packets */
    ulong smf_entry_hits;	/* count the times a match is found */
};
