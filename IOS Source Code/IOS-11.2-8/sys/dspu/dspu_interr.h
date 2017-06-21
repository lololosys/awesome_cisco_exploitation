/* $Id: dspu_interr.h,v 3.1 1995/11/09 11:28:19 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_interr.h,v $
 *------------------------------------------------------------------
 * DSPU Interrupt locking facility.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This module contains interrupt enable and disable. Currently the DSPU
 * does not have any reentrancy data to be protected. So we define both 
 * interrupt and disable interrupt to null
 *------------------------------------------------------------------
 * $Log: dspu_interr.h,v $
 * Revision 3.1  1995/11/09  11:28:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:53  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __INTERR_h
#define __INTERR_h

#ifdef AtomicOperationRequired
void InhibitInterrupts (void);
void EnableInterrupts(void);

#define DISABLE_INTERRUPT InhibitInterrupts();
#define ENABLE_INTERRUPT  EnableInterrupts();
#else
#define DISABLE_INTERRUPT
#define ENABLE_INTERRUPT
#endif

#endif
