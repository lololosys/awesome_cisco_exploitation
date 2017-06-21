/* $Id: bsc_output.h,v 3.2.60.2 1996/06/20 01:43:39 jbalestr Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_output.h,v $
 *------------------------------------------------------------------
 * bsc_output.h - BSC output encapsulation definitions
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * set of functions to encapsulate the I/O stuff by providing
 * an extra level of indirection to ensure consistent output
 *
 *------------------------------------------------------------------
 * $Log: bsc_output.h,v $
 * Revision 3.2.60.2  1996/06/20  01:43:39  jbalestr
 * CSCdi52599:  bsc does not update interface error counters
 * Branch: California_branch
 *
 * Revision 3.2.60.1  1996/06/17  08:30:00  jhernand
 * Commit Modular Cobra to California_branch
 *
 * Revision 3.2  1995/11/17  08:48:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:14:06  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_OUTPUT_H
#define __BSC_OUTPUT_H

typedef enum BSC_event {
  BSCevent_BAD_NS,
  BSCevent_BAD_ADDR,
  BSCevent_ADDR_NO_MAP,
  BSCevent_NEW_ADDR,
  BSCevent_PENDING_TIMEOUT,
  BSCevent_BAD_HARD_OPCODE,
  BSCevent_UNKNOWN_FRAME_TYPE
} BSC_event ;

extern void
bsc_output_event(hwidbtype *idb , BSC_event );

extern void
bsc_output_state(
  const char * machine ,
  const char * in ,
  const char * oldstate ,
  const char * newstate ,
  hwidbtype  * idb
  );

extern void
bsc_output_frame(
  const char * ,
  const unsigned char * frame ,
  int size ,
  hwidbtype * idb
  );
     
#endif  /* __BSC_OUTPUT_H. */
/*---- End include file: bsc_output.h. */







