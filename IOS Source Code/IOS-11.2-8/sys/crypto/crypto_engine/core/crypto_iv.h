/* $Id: crypto_iv.h,v 1.1.66.4 1996/08/07 09:21:44 xliu Exp $
 * $Source: /release/112/cvs/Xsys/crypto/crypto_engine/core/crypto_iv.h,v $
 *------------------------------------------------------------------
 * crypto_engine_iv.h
 * 
 * This file defines the Cylink iv gen table. 
 *
 * 11, 1995, xliu 
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: crypto_iv.h,v $
 * Revision 1.1.66.4  1996/08/07  09:21:44  xliu
 * CSCdi63274:  Wrong crypto engine name a show cr eng conf
 * Branch: California_branch
 *
 * Revision 1.1.66.3  1996/07/15  10:06:00  xliu
 * CSCdi62312:  IP packets without layer 4 headers dont encrypt properly
 * Branch: California_branch
 *
 * Revision 1.1.66.2  1996/05/12  23:12:46  xliu
 * CSCdi56023:  Fixes made after integrating with turbo IPC, and cached
 * packet mem.
 * Branch: California_branch
 *
 * Revision 1.1.66.1  1996/04/19  14:56:35  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commits
 *
 * Revision 1.1.2.1  1995/11/30  04:59:46  xliu
 * Branch: IosSec_branch
 *
 * Moved the crypto engine core files into dir CORE.
 *
 * Revision 1.1  1995/11/30  04:20:51  xliu
 * Place holder files.
 *
 * Revision 1.7  1995/06/07 12:03:05  smackie
 * Fold back Arizona_branch into mainline.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
#ifndef CRYPTO_IV_H_
#define CRYPTO_IV_H_

#include "master.h"

/* Cryptographic initialization values.  Note that the offsets (1st and 3rd */
/* columns) are relative to the Layer 3 header.  ALSO, A RESTRICTION ON THIS */
/* TABLE is that the size be a root value of 8 (e.g., 1,2,4,8). */

/*

typedef struct {            
    ushort  ivgen_ofs;      
    ushort  ivgen_siz;      
    ushort  skip;          
}   cylink_cyConfig[NUM_PROTOCOLS];

*/

cylink_cyConfig  crypto_iv_gen_tbl  =   /* init values per protocol stack */
{
    {   0,  0,   0  },          /* #0   undefined */
    {   0,  0,   0  },          /* #1   undefined */
    {   0,  0,   0  },          /* #2   undefined */
    {   0,  0,   0  },          /* #3   undefined */
    {   0,  0,   0  },          /* #4   undefined */
    {   0,  0,   0  },          /* #5   undefined */
    {   0,  0,   0  },          /* #6   undefined */
    {   0,  0,   0  },          /* #7   undefined */
    {   0,  0,   0  },          /* #8   undefined */
    {   0,  0,   0  },          /* #9   undefined */
    {   0,  0,   0  },          /* #10  undefined */
    {   0,  0,   0  },          /* #11  undefined */
    {   0,  0,   0  },          /* #12  undefined */
    {   0,  0,   0  },          /* #13  NTBS */
    {   0,  0,   0  },          /* #14  VIP */
    {   0,  0,   0  },          /* #15  VIP:VSPP */
    {   0,  0,   0  },          /* #16  VIP:VIPC */
    {   0,  0,   0  },          /* #17  DRP */
    {   0,  0,   0  },          /* #18  DRP:NSP */
    {   6,  1,   7  },          /* #19  DDP */
    {   7,  4,   11 },          /* #20  DDP:ADSP */
    {   6,  1,   7  },          /* #21  DDP:ATP */
    {   6,  2,   8  },          /* #22  IDP */
    {   6,  2,   8  },          /* #23  IDP:PEP */
    {   6,  4,   10 },          /* #24  IDP:SPP */
    {   0,  1,   1  },          /* #25  IPX */
    {   3,  1,   4  },          /* #26  IPX:NCP */
    {   0,  4,   4  },          /* #27  IPX:PEP */
    {   6,  4,   10 },          /* #28  IPX:SPX */
    {   6,  2,   14  },         /* #29  IP */
    {   6,  2,   8  },          /* #30  IP:UDP */
    {   4,  8,   14 },          /* #31  IP:TCP */
    {   0,  2,   0  },          /* #32  CISCO default iv. only ivgen_siz 
                                   is useful in this defintion.
                                 */
};


#endif



