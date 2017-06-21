/* $Id: crypto_debug.h,v 3.1.58.1 1996/04/19 14:55:35 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_debug.h,v $
 *------------------------------------------------------------------
 * crypto_debug.h - Debugging declarations for the Crypto subsystem
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_debug.h,v $
 * Revision 3.1.58.1  1996/04/19  14:55:35  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.3  1995/11/28  01:46:24  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.2  1995/11/17  16:33:16  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.2.1  1995/07/01  05:31:20  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 1.1.2.1  1995/06/06  22:36:19  bew
 * Branch: IosSec_branch
 * Addition of debug flags used by the debug facility. These flags
 * supercede the previously defined debug flags.
 *
 * Revision 3.1  1995/11/09  11:13:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/29  22:59:52  carrel
 * Placeholders for IosSecurity.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up crypto debugging */
extern void crypto_debug_init(void);

/*
 * The actual debugging flags are defined in crypto_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../crypto/crypto_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../crypto/crypto_debug_flags.h"
