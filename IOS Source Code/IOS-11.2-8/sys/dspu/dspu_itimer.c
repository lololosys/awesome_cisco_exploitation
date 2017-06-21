/* $Id: dspu_itimer.c,v 3.1.60.2 1996/08/28 12:41:58 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_itimer.c,v $
 *------------------------------------------------------------------
 * DSPU Timer Function.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This contains the interval timer function(s).
 *------------------------------------------------------------------
 * $Log: dspu_itimer.c,v $
 * Revision 3.1.60.2  1996/08/28  12:41:58  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.60.1  1996/05/17  10:47:38  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.48.2  1996/04/09  18:37:12  ppearce
 * CSCdi54114:  DSPU crashes router during deactivation of downstream PUs
 * Branch: IbuMod_Calif_branch
 *   Check for null luluTimer before invoking StopItimer()
 *
 * Revision 3.1.48.1  1996/04/03  14:02:37  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.1  1995/11/09  11:28:20  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:55  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include "dspu.h"
#include "dmalloc.h"      /* C's dynamic storage allocation           */
#include "dspuqueu.h"    /* queue manipulation functions             */
#include "itcblk.h"      /* timer control block structure            */
#include "dspu_pu.h"     /* pu timer */
#include "dspu_itimer.h"
#include "dspu_error.h"

extern void PoolCheckTimer(void);

static DspuQueueHeader freeTimerList;      
static DspuQueueHeader activeTimerList;   

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
       ) 
{
  /* Timer queues get initialised */
  DspuQueueInit(&activeTimerList);
  DspuQueueInit(&freeTimerList);
}        

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
       (Itimer * this   
       ,boolean  freeTimerFlag  
       ) 
{
  this->next      = 0;  
  this->iTimeval  = 0;  
  this->procTimer = 0; 
  this->objP      = 0;      
  this->freeTimer = freeTimerFlag;      
}        
       
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
       (hword numTimersToCreate
       )
{
  hword     i;
  Itimer * this;
  
  for(i=0; i<numTimersToCreate; i++)
  {
    this = DspuMalloc(sizeof(Itimer));
    if (this) {
        InitialiseItimer(this, TRUE);
        DspuEnqueue(&freeTimerList, this);
    }
  }  
}        
       
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
       ) 
{
  hword     i;
  Itimer * this;
  
  for(i=0; i<numTimerToDelete; i++)
  {
    this = DspuDequeue(&freeTimerList);
    DspuFree(this);
  }  
}        

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
       (Itimer  * this
       ,hword     timerInterval
       ,ProcTimer timeOutProc
       ,void    * timerHandle
       ) 
{
  this->iTimeval  = timerInterval+1;  /* +1 'cause timer is pre-decremented */
  this->procTimer = timeOutProc; 
  this->objP      = timerHandle;      
}        
       
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
Itimer *GetAndStartItimer(hword timerInterval, 
                            ProcTimer timeOutProc, void *timerHandle) 
{
  Itimer * this;
  
  if ((this = DspuDequeue(&freeTimerList)) != 0)
  {
      ReStartItimer(this, timerInterval, timeOutProc, timerHandle);
      DspuEnqueue(&activeTimerList, this);
  } else {
      DspuDebugErrorMsg(GetDebugLevel(), DspuStartTimerFailed);
  }

  return this;
}        

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
       (Itimer  * this
       ,hword     timerInterval
       ,ProcTimer timeOutProc
       ,void    * timerHandle
       ) 
{
  boolean timerNotInQueue = this->iTimeval <= 0;
  
  ReStartItimer(this, timerInterval, timeOutProc, timerHandle);
  if (timerNotInQueue)  
     DspuEnqueue(&activeTimerList, this);

}


/*=====================================================================*
* StopItimer
*======================================================================*
*
* Description:
*   This dechains the timer control block from the ActiveTimers list.
* If the freeTimer flag is set to TRUE then it chains the timer onto 
* the FreeTimers list. If the timer is not found in the ActiveTimers
* list it does nothing. It stops the external timer if this is the
* last timer on the active list.
*
**/
void StopItimer (Itimer *this) 
{
  if (!this) {
    DspuDebugErrorMsg(GetDebugLevel(), DspuStopTimerFailed);
  }
  if (DspuDeleteQueueElement(&activeTimerList, this))
  {
    this->iTimeval = 0;     /* timer is not in the queue any more */
    if (this->freeTimer)
      DspuEnqueue(&freeTimerList, this);
  }    

}        

/*=====================================================================*
* DspuTimeOut
*======================================================================*
*
* Description:
*  Note:  This timer routine is to be called EVERY SECOND.
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
       ) 
{
  Itimer * this;
  Itimer * freeThis;

  /* hook minute timer. 
   */

static minuteTimer;
  
  if (minuteTimer++ >= 60*SECONDS){    /* 1 minute  expired */
     minuteTimer = 0;
     PoolCheckTimer();         /* call pool timer    */
  }
  this = DspuGetFirstQelement(&activeTimerList);
  while (this)
  {
    if (--this->iTimeval <= 0)  
    { 
      freeThis = this;
      this = this->next;
      DspuDeleteQueueElement(&activeTimerList, freeThis);
      freeThis->procTimer(freeThis->objP);
      if (freeThis->freeTimer)
        DspuEnqueue(&freeTimerList, freeThis);
    }
    else
      this = this->next;
  }
  PuReconnectCheck();
}

