/* $Id: if_isr_epad.h,v 3.1.64.1 1996/04/19 15:26:04 che Exp $
 * $Source: /release/112/cvs/Xsys/pas/if_isr_epad.h,v $
 *------------------------------------------------------------------
 * if_isr_epad.h - Encryption Port Adaptor interrupt service header
 *			file.
 *
 * August 1995, Paulina D. Tran
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: if_isr_epad.h,v $
 * Revision 3.1.64.1  1996/04/19  15:26:04  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.1.2.2  1996/02/02  23:14:34  ptran
 * Commit first release of EPA firmware.
 * Branch: IosSec_branch
 *
 * Revision 3.1.2.1  1995/12/06  21:48:57  rmontino
 * Branch: IosSec_branch
 *
 * Revision 3.1  1995/11/09  12:57:30  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/08/23  02:27:32  ptran
 * Placeholder file for new development.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IF_ISR_EPAD_H__
#define __IF_ISR_EPAD_H__
 
 
extern void epad_fastsend (hwidbtype *idb, paktype *pak);
extern void epad_interrupt (hwidbtype *);
 
 
#endif /* __IF_ISR_EPAD_H__ */

