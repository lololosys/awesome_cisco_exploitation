/* $Id: crypto_debug_flags.h,v 3.1.60.2 1996/06/07 00:24:00 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_debug_flags.h,v $
 *------------------------------------------------------------------
 * crypto_debug_flags.h - Definition of debug array for debug facility
 *
 * June 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_debug_flags.h,v $
 * Revision 3.1.60.2  1996/06/07  00:24:00  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.1.60.1  1996/04/19  14:55:36  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.7  1995/12/06  23:29:03  bew
 * Branch: IosSec_branch
 * Removed the CRYPTO_DEBUG_FLAGS_H_ symbol and added a warning explaining
 * that the header file may by design it will be included multiple times.
 *
 * Revision 2.1.2.6  1995/12/06  00:39:39  xliu
 * Branch: IosSec_branch
 * Add crypto engine vip debug flag.
 *
 * Revision 2.1.2.5  1995/11/28  01:46:24  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.4  1995/11/17  16:33:17  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.2.3  1995/10/17  01:19:32  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.2.2  1995/10/13  08:30:49  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser command. Add debug parser command.
 *
 * Revision 2.1.2.1  1995/07/01  05:31:20  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 1.1.2.1  1995/06/07  00:00:28  bew
 * Branch: IosSec_branch
 * Definition of crypto debug array.
 *
 * Revision 3.1  1995/11/09  11:13:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/29  22:59:53  carrel
 * Placeholders for IosSecurity.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * WARNING! Do not implement CRYPTO_DEBUG_FLAGS_H_ here, as it is intentionally
 *	    multiply included with DEBUG_FLAG defined differently!
 */

#include "../ui/debug_macros.h"

/*
 * Debug flags added to the debug facility.
 */
DEBUG_ARRDECL(crypto_debug_arr)
DEBUG_FLAG(crypto_sm_debug,DEBUG_CRYPTO_SM,"Crypto Session Management")
DEBUG_FLAG(crypto_engine_debug,DEBUG_CRYPTO_ENGINE,"Crypto Engine")
DEBUG_FLAG(crypto_engine_vip_debug, DEBUG_CRYPTO_ENGINE_VIP, "Crypto Engine VIP")
DEBUG_FLAG(crypto_ke_debug,DEBUG_CRYPTO_EXCHANGE,"Crypto Key Exchange")
DEBUG_ARRDONE
