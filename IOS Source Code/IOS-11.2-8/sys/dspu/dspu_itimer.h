/* $Id: dspu_itimer.h,v 3.1 1995/11/09 11:28:21 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_itimer.h,v $
 *------------------------------------------------------------------
 * DSPU Interval Timer Exports.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This contains procedure definitions for the interval timer function(s).
 *------------------------------------------------------------------
 * $Log: dspu_itimer.h,v $
 * Revision 3.1  1995/11/09  11:28:21  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:56  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __ITIMER_H                         
#define __ITIMER_H

#include "dtypes.h"
#include "itcblk.h"  /* For the timer control block structure */

#define SECONDS 1

/*=====================================================================*
* InitialiseItimerQueues
*======================================================================*
*
* Description:
*   This initialises the freeTimerList and the activeTimerList.
*
**/
void InitialiseItimerQueues
       (void
       ); 

/*=====================================================================*
* CreateGlobalItimer
*======================================================================*
*
* Description:
*   This creates the number of timer control blocks, initialises them,
* and chains them onto the global FreeTimer list.
*
**/
void CreateGlobalItimer
       (hword numTimerToCreate
       );
       
/*=====================================================================*
* DeleteGlobalItimer
*======================================================================*
*
* Description:
*   This dequeues the number of timers from the global FreeTimer list
* and deallocates them.
*
**/
void DeleteGlobalItimer
       (hword numTimerToDelete
       );
       
/*=====================================================================*
* InitialiseItimer
*======================================================================*
*
* Description:
*   This initialises the timer control block to its initial value.
* The freeTimerFlag is used to distinguish if the Itimer control
* block should be put back onto the freeTimerList. i.e. calls from
* the link station control block do not use the freeTimerList.
*
**/
void InitialiseItimer
       (Itimer * iTimerP
       ,boolean  freeTimerFlag  
       );
       
/*=====================================================================*
* GetAndStartItimer
*======================================================================*
*
* Description:
*   This gets a timer control block from the FreeTimer list, fills in
* the parameters and chains the control block onto the ActiveTimers 
* list, and returns the pointer to the timer control block. It calls 
* the external timer to get DspuTimeout driven if this is the first
* timer started.
*
**/
Itimer * GetAndStartItimer 
           (hword     timerInterval
           ,ProcTimer timeOutProc
           ,void    * timerHandle
           );

/*=====================================================================*
* StartItimer
*======================================================================*
*
* Description:
*   This fills in the parameters into the control block and chains the 
* control block onto the ActiveTimers list. It calls the external
* timer to get DspuTimeout driven if this is the first timer started.
*
**/
void StartItimer 
       (Itimer  * iTimerP
       ,hword     timerInterval
       ,ProcTimer timeOutProc
       ,void    * timerHandle
       );

/*=====================================================================*
* ReStartItimer
*======================================================================*
*
* Description:
*   This assumes that the timer control block is currently chained on  
* the ActiveTimers list. It simply fills in the new parameters into the  
* control block. 
*
**/
void ReStartItimer 
       (Itimer  * iTimerP
       ,hword     timerInterval
       ,ProcTimer timeOutProc
       ,void    * timerHandle
       );

/*=====================================================================*
* StopItimer
*======================================================================*
*
* Description:
*   This dechains the timer control block from the ActiveTimers list. 
* If the timer is not found in the ActiveTimers list it does nothing.
* It stops the external timer if this is the last timer on the active list.
*
**/
void StopItimer
       (Itimer * iTimerP
       );

/*=====================================================================*
* DspuTimeOut
*======================================================================*
*
* Description:
*   This is the entry point driven by the external timer service. It 
* runs the active list of timers, decrementing all the timers, and
* drives the timeout routine if the count goes to zero.
*   When the timeout routine is called, if it returns FREE_TIMER, then  
* the timer control block is put back on the FreeTimers list; if it  
* returns DO_NOT_FREE_TIMER then the timer is assumed to be "owned" 
* by someone else.
*
**/
void DspuTimeOut
       (void
       );

#endif  /* __ITIMER_H */









