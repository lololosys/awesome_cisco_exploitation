/* $Id: itcblk.h,v 3.2 1995/11/17 09:05:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/itcblk.h,v $
 *------------------------------------------------------------------
 * DSPU Interval Timer Structure.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This contains the interval timer structure.
 *------------------------------------------------------------------
 * $Log: itcblk.h,v $
 * Revision 3.2  1995/11/17  09:05:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:20  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __ITICBLK_H                         
#define __ITICBLK_H

#include "dtypes.h"      /* For boolean definition */


/* the timer is used by the following objects, declare the class pointer
* properly rather than void*
*/

#ifdef LU_TEMPLATE 
typedef Lu* Object;
#else
#ifdef PU_TEMPLATE 
typedef Pu* Object;
#else
#ifdef LS_TEMPLATE 
typedef LinkStn* Object;
#else
#ifdef SAP_TEMPLATE 
typedef Sap* Object;
#else
typedef void* Object;
#endif
#endif
#endif
#endif

typedef void (*ProcTimer)(Object ); 

 struct Itimer{                         
        struct Itimer * next;  
        ProcTimer       procTimer; /* Proc to exec on timeout.         */
        Object          objP;      /* Object assoc'd with callback.    */
        short           iTimeval;  /* Interval Timer value.            */
        boolean         freeTimer; /* Flag to indicate if itimer came  */
                                   /*   from the freeTimerList         */
        } ;

#endif /* __ITICBLK_H */
