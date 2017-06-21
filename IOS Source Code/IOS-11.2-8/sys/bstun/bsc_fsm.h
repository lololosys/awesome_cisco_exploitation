/* $Id: bsc_fsm.h,v 3.2 1995/11/17 08:48:18 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/bstun/bsc_fsm.h,v $
 *------------------------------------------------------------------
 * bsc_fsm.h - BSC Generic FSM datatype
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: bsc_fsm.h,v $
 * Revision 3.2  1995/11/17  08:48:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:13:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_FSM_H
#define __BSC_FSM_H


typedef void (*fsm_function)( hwidbtype * );

typedef struct fsm_node {
        int state ;
        fsm_function action;
        } fsm_node;

#endif  /* __BSC_FSM_H. */
/*---- End include file: bsc_fsm.h. */

