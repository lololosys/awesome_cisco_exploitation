/* $Id: dspu_fsmstr.h,v 3.1.60.1 1996/05/17 10:47:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_fsmstr.h,v $
 *------------------------------------------------------------------
 * DSPU FSM Structure.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * FSM structure  
 *------------------------------------------------------------------
 * $Log: dspu_fsmstr.h,v $
 * Revision 3.1.60.1  1996/05/17  10:47:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.48.1  1996/03/22  18:50:19  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.1  1995/11/09  11:28:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:52  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __FSMSTR_H                         
#define __FSMSTR_H

typedef struct FsmStruct {
   byte  state;
   byte  action;
}FsmStruct;


typedef enum Action {a0,a1,a2,a3,a4,a5,a6,a7,a8,a9,a10,a11,a12,a13,a14,a15,
            a16,a17,a18,a19,a20,a21,a22,a23,a24,a25,a26,a27,a28,a29,a30,
            a31,a32,a33,a34,a35,a36,a37,a38,a39,a40,a41,a42
} Action;

#endif
