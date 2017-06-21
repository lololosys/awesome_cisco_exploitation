/* $Id: crypto_util.h,v 3.2.60.2 1996/06/07 00:24:26 bew Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_util.h,v $
 *------------------------------------------------------------------
 * crypto_util.h - Defines for encryption utilitiy routines
 *
 * April, 1995 Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_util.h,v $
 * Revision 3.2.60.2  1996/06/07  00:24:26  bew
 * CSCdi59169:  Crypto cleanup work
 * Branch: California_branch
 * Various cleanups & addition of "crypto ping".
 *
 * Revision 3.2.60.1  1996/04/19  14:56:06  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.6.5  1996/03/25  18:43:44  bew
 * Branch: IosSec_branch
 * Added 40-bit DES and DES 8-bit CFB support. Implemented partial
 * challenge processing support, not yet compatible with SDU.
 * Updated the crypto methods vector per Cylink's new definitions.
 *
 * Revision 2.1.6.4  1995/11/28  01:46:43  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.6.3  1995/11/17  16:33:26  carrel
 * Branch: IosSec_branch
 * Sync to Arkansas
 *
 * Revision 2.1.6.2  1995/08/16  23:43:54  bew
 * Branch: IosSec_branch
 * Fixed a couple of small bugs.
 *
 * Revision 2.1.6.1  1995/07/01  05:31:26  carrel
 * Branch: IosSec_branch
 * Initial merge of the 10.3 IosSec_branch to Arizona.
 *
 * Revision 3.1  1995/11/09  11:13:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:21:12  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * Revision 1.1.2.2  1995/06/05  17:22:50  bew
 * Branch: IosSec_branch
 * Addition of new utility functions used by crypto subsystem.
 *
 * Revision 1.1.2.1  1995/04/27  19:23:41  bew
 * Branch: IosSec_branch
 * Adding IP level files which communicate with the EPA API, and Utility
 * files used by both the parser EPA support and the IP level files.
 *
 * Revision 1.1  1995/04/27 19:12:02  bew
 * Placeholder files for crypto IP and Utility files.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern char *crypto_xlate_status(epa_status);

extern char *crypto_xlate_algorithm_name(ushort);

extern char *crypto_xlate_message_name(ushort);
