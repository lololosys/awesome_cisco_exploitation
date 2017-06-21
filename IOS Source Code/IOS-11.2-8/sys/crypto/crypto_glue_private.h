/* $Id: crypto_glue_private.h,v 3.2.62.1 1996/04/19 14:55:44 che Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_glue_private.h,v $
 *------------------------------------------------------------------
 * crypto_glue_private.h - Cylink symbols for use within the glue layer.
 *
 * September, 1995, Brian Weis
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_glue_private.h,v $
 * Revision 3.2.62.1  1996/04/19  14:55:44  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 2.1.2.7  1995/11/28  01:46:30  bew
 * Port of newest Cylink SDU code, plus CS cleanup. Cylink files
 * are moved to match their original tree structure.
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.6  1995/10/17  01:19:35  bew
 * Branch: IosSec_branch
 * Add the simple unauthenticated public key exchange protocal. Cleaned
 * up the printing of pulic keys. Modified some show commands syntax to
 * match the functional spec. General cleanup.
 *
 * Revision 2.1.2.5  1995/10/16  04:04:18  xliu
 * Branch: IosSec_branch
 * Add crypto engine parser commands.
 *
 * Revision 2.1.2.3  1995/10/10  18:19:35  bew
 * Branch: IosSec_branch
 * Implementation of connection timeouts with managed timers & the clear
 * crypto connection command.  Introduced the concept of "current
 * connection" and "new connection" for use during connection setup.
 * Also oodles of cleanup work.
 *
 * Revision 2.1.2.2  1995/09/29  01:00:32  bew
 * Added session key timeouts. Also renamed epa_parser.c to
 * crypto_parser.c & changed the timestamp used in SDU code
 * to be the seconds from Day 0 rather than the contents of
 * msclock.
 *
 * Branch: IosSec_branch
 *
 * Revision 2.1.2.1  1995/09/27  23:20:50  bew
 * Branch: IosSec_branch
 * Addition of crypto method policy setting, separation of glue
 * headers into crypto_glue.h & crypto_glue_private.h, fast switch
 * insert to find crypto connection messages, cleanup of Cylink
 * symbols from Cisco code.
 *
 * Revision 2.1  1995/09/27  07:38:04  bew
 * Placeholder crypto files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Include some SDU files. They include others too ....
 */
#include "../crypto/sdu/include/cylink.h"
#include "../crypto/sdu/include/csdata.h"
#include "../crypto/sdu/include/csutil1.h"
#include "../crypto/sdu/include/csutil2.h"
#include "../crypto/sdu/include/ep_frm.h"
#include "../crypto/sdu/include/pa/pactl.h"
#include "../crypto/sdu/include/cy_frm.h"
#include "../crypto/sdu/include/net.h"
#include "../crypto/sdu/include/iomgr.h"
#include "../crypto/sdu/include/ep_cust.h"
#include "sdu/include/auth/authext.h"

/*
 * Miscellaneous externs not in SDU .h files
 */
extern void recv_nnc(Pa2EpFrameControl *);
extern void send_nnc_req(Pa2EpFrameControl *);

/*
 * Cisco defined stuff
 */

#define CRYPTO_NODE_ID_OFFSET 8  /* Node ID offset into NNC */
