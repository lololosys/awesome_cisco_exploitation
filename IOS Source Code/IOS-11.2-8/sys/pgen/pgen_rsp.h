/* $Id: pgen_rsp.h,v 3.1 1995/11/09 12:59:18 shaker Exp $ 
 * $Source: /swtal/cherf.111/ios/sys/pgen/pgen_rsp.h,v $ 
 *------------------------------------------------------------------
 * Pagent RSP specific definitions
 *
 * July 1995, Brian Jackson
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen_rsp.h,v $
 * Revision 3.1  1995/11/09  12:59:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  01:52:12  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 * 
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Pagent's private buffer return pool pointer */
qa_hwq_t *pgen_qptr;

/* Pointers to the fast counting hardware queues */
qa_hwq_t *pgen_RawQ;

/* Clock tick counter used to control packet transmission rates */
static ulong pgen_delayuntil;

/* Structure used to restore packet contents after an RSP fast send */
typedef struct pgen_bufpool_t_ {
    bufhdr_t    *bufptr;
    ushort      bf_queue_ptr;
  } pgen_bufpool_t;

/*
 * Macro used by pgen_fastcount to process a single interface or all
 * interfaces.  Based on FOR_ALL_HWIDBS, this loops once for a specific hwidb
 * if one is specified; otherwise (hw == NULL), it loops through all hwidbs.
 */
#define PGEN_FOR_HWIDBS(hw) \
    int repeat, rptflag; \
    if (hw) rptflag = 0; else rptflag = 1, (hw) = (hwidbtype *) hwidbQ->qhead;\
    for (repeat = 1; (hw) && repeat; (hw) = (hw)->next, repeat = rptflag)

/* Pagent fast count input queue interrupt processing routine */
void pgen_process_rawq (void);
