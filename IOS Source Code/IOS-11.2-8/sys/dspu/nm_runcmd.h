/* $Id: nm_runcmd.h,v 3.2.60.1 1996/05/17 10:48:57 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/nm_runcmd.h,v $
 *------------------------------------------------------------------
 * SNA Network Management IBM runcmd handler
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nm_runcmd.h,v $
 * Revision 3.2.60.1  1996/05/17  10:48:57  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/30  08:10:37  doner
 * CSCdi52657:  IBU modularity - DSPU conversion to use new IOS scheduler
 * Branch: IbuMod_Calif_branch
 * Update to new scheduler interface
 *
 * Revision 3.2  1995/11/17  09:05:58  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef mv_runcmd_h
#define  mv_runcmd_h


#define MaximumRuncmd 8

/* Instead of using an array of correlator, a buffer of 128 bytes are
 * shared. Since most of the correlator length is 2. It should never
 * overflow.
*/
#define MAX_CORRELATOR_LN 128

#undef MVReceiverGeneric
#define MVReceiverGeneric RuncmdReceiver



#include "mv_sr.h"      /* base class */

struct RuncmdCB {
   RuncmdCB* next;
   byte  readIndex;         /* current readIndex of input buffer */
   byte  writeIndex;        /* current writeIndex of input buffer */
   hword correlatorLength;
   byte* correlatorP;      /* pointer to correlator buffer */
};


struct RuncmdReceiver{
   MVReceiverBaseClass
   byte  buffer[256];    /* execute command */
   boolean interactive_mode;       /* VTY session in interactive mode. */
   byte  writeIndex;     /* current writeIndex of the buffer */
   byte  correlatorIndex; 
   byte  correlator[MAX_CORRELATOR_LN];
   RuncmdCB runcmdCB[MaximumRuncmd];
   watched_queue *batchQ;          /* Point to batch run command queue. */
   queuetype interactiveQ;         /* The interactive run command queue. */
   queuetype emptyQueue;           /* empty queue */
   boolean runcmdRunning;
   tt_soc* tty;
};

/* Get char from interative command */
int GetInteractiveChar(tt_soc* tty);
int NmInputPending(tt_soc* tty);
int Nm_startoutput(tt_soc* tty);

/* The following message indicate tty output message. */
enum TTY_overflow{
   MallocFailure = 1,
   MessageTooLong,
   MessageSentFailure
};

void
PH_Init(void);

#endif
