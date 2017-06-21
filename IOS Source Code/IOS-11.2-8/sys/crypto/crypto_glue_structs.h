/* $Id: crypto_glue_structs.h,v 3.1.68.2 1996/09/11 23:25:05 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_glue_structs.h,v $
 *------------------------------------------------------------------
 * crypto_glue_structs.h - IOS-only struct definitions referenced by IOS code.
 *
 * November 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_glue_structs.h,v $
 * Revision 3.1.68.2  1996/09/11  23:25:05  bew
 * CSCdi68438:  crypto determination of network address should be done
 * earlier
 * Branch: California_branch
 * Run the routing tables during connection message setup to determine
 * which interface the packet will go out.
 *
 * Revision 3.1.68.1  1996/04/19  14:55:47  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 3.1.2.2  1996/04/16  21:52:16  bew
 * Branch: IosSec_branch
 * General code cleanup. Implemented checking of crypto map peers. CIM now
 * addressed to the src/dst rather than pe/upe.
 *
 * Revision 3.1.2.1  1995/11/28  01:46:32  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/22  23:17:42  bew
 * Placeholders for new locations of crypto parser files
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* 
 * Cisco structure which is a part of FrameControlHeader via the
 * otherP pointer.
 * NOTE: Mixed case is Cylink's fault
 */
typedef struct cisco_FrameControlHeader_ {
    boolean	retransmit;
    /* epEntry is used when not part of Pa2EpFrameControl */
    epFrameInfo epEntry ; 
} cisco_FrameControlHeader;
