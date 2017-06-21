/* $Id: dspu_pool.c,v 3.2.6.3 1996/08/28 12:42:04 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_pool.c,v $
 *------------------------------------------------------------------
 * DSPU LU Pool Support.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Supports the Pooled LU Function in DSPU.
 *------------------------------------------------------------------
 * $Log: dspu_pool.c,v $
 * Revision 3.2.6.3  1996/08/28  12:42:04  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2.6.2  1996/07/29  07:16:43  lcox
 * CSCdi63645:  dspu inactivity timeout doesnt get cleaned up properly.
 * Branch: California_branch
 *
 * Revision 3.2.6.1  1996/03/18  19:33:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.3  1996/03/16  06:40:34  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.28.2  1996/03/07  08:45:30  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.28.1  1996/02/20  13:47:48  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1996/03/05  03:25:25  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.1  1995/11/09  11:28:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:03:04  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/09/14 06:15:13  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

 
#include "master.h"
#include "logger.h"
#include "msg_dspu.c"			/* Not a typo, see logger.h */
#include <string.h>

#include "dspu.h"
#include "dmalloc.h"    /* malloc */
#include "dspu_pool.h"
#include "dspu_lu.h"
#include "dspumib_mgmt.h"
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_ls.h"
#include "dspu_error.h"

static DspuQueueHeader luPoolRoot;
 
/*=====================================================================*
* PoolCB_nw
*======================================================================*
*
* Description:
*   This is the POOL control block constructor and it is called by
* the procedure DspuKeyPool at configuration of the POOL.
*
**/
PoolCB * PoolCB_nw
           (char * poolName,
            hword inactivityTimeout
           )
{       
  PoolCB * poolCBP;
  
  poolCBP = DspuMalloc(sizeof(PoolCB));
  if (poolCBP) {
    poolCBP->next = 0;
    memcpy(poolCBP->name, poolName, MAX_ENTRYNAME_LN); 
    poolCBP->inactivityTimeout = (byte) inactivityTimeout;
    poolCBP->upStreamLuDefs = 0;
    poolCBP->dnStreamLuDefs = 0;
    DspuQueueInit(&poolCBP->hostLuPoolHeader);
    DspuQueueInit(&poolCBP->waitQueueHeader);
    DspuEnqueue(&luPoolRoot, poolCBP);   /* save at root for poolName scanning*/

    /* Add entry into the dspuPoolClassTable of the DSPU MIB */
    /* ----------------------------------------------------- */
    AddEntry_DspuPoolClassTable(poolCBP);
  }
  return poolCBP;    
}

/*
* PoolChangeInactTimeout
*  Change the value of the inactivity timeout to the last
*  configured value.
*/
void PoolChangeInactTimeout (PoolCB * poolCBP,
                             byte inactivityTimeout)
{
    poolCBP->inactivityTimeout = inactivityTimeout;
}


/*=====================================================================*
* InitPoolRoot
*======================================================================*
*
* Description:
*   Initialises the Pool root.
*
**/
void InitPoolRoot (void)
{       
  DspuQueueInit(&luPoolRoot);
}

/*=====================================================================*
* PoolCheckTimer
*======================================================================*
*
* Description:
*   This routine is called every minute to check if the LU has 
* had any activity since the last call. If there was no activity
* then LuActivityTimeout is called which invokes the SSCP_LU FSM
* with input LuInactTimeout.
*
*/
void PoolCheckTimer (void)
{
   byte inactivityTimeout; 

   PoolCB* poolCBP = GetFirstQelement(luPoolRoot);
   while (poolCBP) {
     if ((inactivityTimeout = poolCBP->inactivityTimeout) != 0){
        Lu* luP = GetFirstQelement(poolCBP->hostLuPoolHeader);
        while(luP){
           LuActivityTimeout(luP, inactivityTimeout);
           luP = NextLu(luP);
        }
     }
     poolCBP = NextQueueElement(poolCBP);
  }
}


/*=====================================================================*
* PoolCB_dl
*======================================================================*
*
* Description:
*   This is the POOL control block destructor. The POOL control block
* must be removed from the queue headed by luPoolRoot.
*
*/
void PoolCB_dl (PoolCB* this)
{
   DspuDeleteQueueElement(&luPoolRoot, this);
   RemoveEntry_DspuPoolClassTable(this);
   DspuFree(this);
}

/*=====================================================================*
* GetFreeLuFromHost
*======================================================================*
*
* Description:
*   Scans through the POOL control block's hostLuPool queue to 
* find a free LU. That is an LU which is not connected to the
* downstream PU. If found return the LU otherwise return null.
*   
**/
Lu* GetFreeLuFromHost
             (PoolCB* poolCBP  /* pool control block defined by poolName */
             )
{
  /* Scan all queued LUs to find a free LU */
    
  Lu* luP = GetFirstQelement(poolCBP->hostLuPoolHeader);
  while(luP){
      if (QueryLuFree(luP))
         return luP;
      luP = NextLu(luP);
  }
  return 0;
}


/*=====================================================================*
* DeletePool
*======================================================================*
*
* Description:
*
*   This routine attempts to destruct a POOL control block. It is 
* called by the routine DspuNKeyPool at configuration time. Before
* the POOL control block can be deleted, all the LUs in the POOL must 
* be inactive and then deleted. Otherwise only the lu within the
* pool control block will be deleted.
*
*   TRUE is returned if the POOL was deleted.
*/
boolean DeletePool(PoolCB* poolCBP)
{
   Lu* luP;
   
   luP = GetFirstQelement(poolCBP->hostLuPoolHeader);
   while (luP)
   {
      Lu* tmp = NextLu(luP);
      if (!QueryLuActive(luP)){
         LuDelete(luP);
      }
      luP = tmp;
   }
   
   /* Ensure the poolCBP's hostLuPoolHeader is empty before 
    * proceeding to destruct the POOL control block.
    */
   
   if (IsEmpty(poolCBP->hostLuPoolHeader)){
      DoToAllChildrenParm2(GetInactDpuRoot(), (ProcChild2)DeletePoolRangeIfMatch, poolCBP->name);
      DoToAllChildrenParm2(GetDnlsRoot(), (ProcChild2)LsDeletePoolRangeIfMatch, poolCBP->name);
      PoolCB_dl(poolCBP);
      return TRUE;
   }
   return FALSE;
}
  
     

/*=====================================================================*
* GetNextLuWait
*======================================================================*
*
* Description:
*    
*    The algorithm for connecting an LU which is waiting to be connected
* is as follows.
*
*    The wait queue contains pool range control blocks. We take a handle
* to the first range in the wait queue. Scan through the range to find
* if there is an LU not connected. 
*
*    If found, set luWait to the lu number and connect it. Before we 
* return this LU back to the calling routine, scan through the remainder  
* of the LUs in the range to determine if there is another LU to connect. 
* If found, update the luWait. If not found then dequeue the pool range
* control block from the wait queue. But incase an LU has become 
* disconnected, scan from the start of the range to determine if there is 
* an LU waiting to be connected. If such an LU is found, then enqueue the  
* range control block and set the luWait to the lu number.
*  
*/
Lu* GetNextLuWait(PoolCB* poolCBP)
{
   Lu*    luP = 0;
   void** next;       /* pool iteration */
   

   /* Execute this loop as long as connection has not been made
    * (luP = 0) and there are range control blocks in the wait queue.
    * When a connection has been made, return the pointer to the
    * LU control block.
    */
   for(next = GetFirstQelement(poolCBP->waitQueueHeader);
       next && (luP == 0);
       next = *next)
   {
      byte lu;

      PoolRange* poolP = PoolRangePointer(next);
      byte range = poolP->luRange - (poolP->luWait - poolP->luStart);
      
      /* Starting from the luWait to the range, find an LU 
       * which is not connected. 
       */
      for (lu = poolP->luWait; range > 0; range--, lu++)
      {
         if (GetChild(GetLuChildren(poolP->puP), lu) == 0){
            
            /* This LU is not connected, get it and connect it. 
             */
            luP = GetFreeLuFromHost(poolCBP);
            if (luP)
               /* Note that the return value of LuConnect is not 
                * checked here because if it fails to connect the
                * LU to the downstream PU it will be attempted again.
                */
               LuConnect(luP, poolP->puP, lu); 
            break;    
         }
      }
      
      /* Scan through the rest of the LUs in the range to find 
       * another LU to be connected.  
       */
      while (range > 0 && GetChild(GetLuChildren(poolP->puP), lu) != 0){
          range--;
          lu++;
      }

      if (range)      
        /* We have found an LU within the range which can be  
         * connected. Thus, set the luWait to the lu number.  
         */  
         poolP->luWait = lu;  /* Start scanning from luWait */  
      else  
      {
         /* All LUs in the range are connected at the moment. 
          * But what happens if an LU in this range becomes  
          * disconnected?  Well then...
          *  
          * Dequeue this range from the wait queue. Do another 
          * scan of this range to see if there is another LU
          * which is not connected. If found, set luWait to
          * the lu number and enqueue this range back onto the
          * wait queue.
          */ 
         poolP = DspuDequeue(&poolCBP->waitQueueHeader);
         poolP = PoolRangePointer(poolP);
         range = poolP->luRange;
         
         poolP->luWait = 0;     /* assume no lu is waiting */
         for (lu = poolP->luStart; range > 0; range--, lu++)
         {
            if (GetChild(GetLuChildren(poolP->puP), lu) == 0){
               poolP->luWait = lu;  /* Start scanning from luWait */
               poolP->nextWait = 0;     /* next must be zero, otherwise enqueu fail */
               DspuEnqueue(&poolCBP->waitQueueHeader, &poolP->nextWait);
               break;
            }
         }
      }
   }
   return luP;  
}

/*=====================================================================*
* PutLuWait
*======================================================================*
*
* Description:
*   An LU has been disconnected from the downstream PU. So this LU
* is available to be connected to a downstream PU. This LU is made
* available to a downstream PU by putting the LU (or rather its
* pool range) onto a wait queue. 
*
*/
void PutLuWait(PoolCB* poolCBP,     /* lu's pool control block */
               Pu*     puP, 
               byte    lunumber)         
{
   PoolRange* poolRgeP = FindPoolRange(puP, lunumber);

   if (poolRgeP && poolRgeP->luWait == 0){    /* is this range waiting ? */

       poolRgeP->luWait = lunumber;           /* put this lu in */
       poolRgeP->nextWait = 0;        /* otherwise enque will fail */
       DspuEnqueue(&poolCBP->waitQueueHeader, &poolRgeP->nextWait);
   }
   
}

/*=====================================================================*
* FindPoolRange
*======================================================================*
*
* Description:
*    Find the pool range control block from a given LU connected to
* upstream PU.
*/
               
PoolRange* FindPoolRange(Pu* puP, byte lu)
{
   PoolRange* poolRgeP = GetFirstQueue(PuGetPoolRangeHeader(puP));

   while (poolRgeP){

      /* make sure pool name existed. Note that pool can be deleted
       * and to be redefined later. So we check the poolname here
       */
      if (FindPool(poolRgeP->poolName) != 0 && poolRgeP->luStart <= lu &&  
                       lu< poolRgeP->luStart+poolRgeP->luRange)
         return poolRgeP;
      poolRgeP = poolRgeP->next;
   }
   return 0;       /* cannot find the range */
}

/*=====================================================================*
* RemoveAllLuFromWaitList
*======================================================================*
*
* Description:
*
*   Remove all the LU Range defined in the PU from the waiting list. 
*
*   Take the first pool control block in the pool root. The pool control
* block has a wait queue. Get the first element in the wait queue and
* call PoolRangePointer to get the pool range pointer. Using the pool
* range pointer compare its PU pointer to the given PU pointer. If
* the PUs are the same then we delete this element from the wait queue.
* This is done for each element in the wait queue and for each pool
* in the luPoolRoot.
*  
* This is called when the downstream PU is changed to inactive.
*
**/
void RemoveAllLuFromWaitList(Pu* puP)  /* Downstream PU */
{
   PoolRange* poolRgeP;
 
   PoolCB* poolCBP = GetFirstQelement(luPoolRoot);

   while (poolCBP) {
     
     void* next;       /* pool iteration */
     next = GetFirstQelement(poolCBP->waitQueueHeader);
     
     while(next)
     {
        poolRgeP = PoolRangePointer(next);
        if (poolRgeP->puP == puP){     /* Is this the pu ?*/
           void* tmp;
           tmp = NextQueueElement(next);/* get next pointer before deleted */
           DspuDeleteQueueElement(&poolCBP->waitQueueHeader, next);
           next = tmp;
        }
        else
           next = NextQueueElement(next);
      }
      poolCBP = NextQueueElement(poolCBP);
   }
}



/*=====================================================================*
* PrintRange
*======================================================================*
*
* Description:
*   This routine writes lu for a given PU, all the defined POOL names
* and its start and range. This is done by scanning through each pool 
* range element in the poolRangeHeader list. Note that a pool can be
* deleted. We ensure that the poolRange does not print out the
* pool.
*   
**/
void PrintRange(Pu* puP)
{
   PoolRange* poolP;
   
   poolP = GetFirstQelement(puP->poolRangeHeader);
   
   while (poolP){
      nv_write(TRUE, "dspu lu %d %d pool %s", poolP->luStart, 
          (poolP->luStart+poolP->luRange-1), poolP->poolName);
      poolP = poolP->next;
   }
}


/*=====================================================================*
* PoolRange_nw
*======================================================================*
*
* Description:
*   This is the pool range control block constructor. It is called at 
* configuration time when the POOL LU(s) are connected to the downstream
* PU. i.e.
*
*   DSPU LU luStart [luRange] POOL poolName [PU puName] 
*
* Also it is called when a default PU is created. 
*   Once the pool range control block is constructed, it is enqueued on 
* the poolRangeHeader queue. 
*   
*/
PoolRange* PoolRange_nw
             (Pu*  puP          /* PU owner of this range */
             ,byte luStart
             ,byte luRange
             ,const char* name  /* poolName */
             )
{

  PoolCB*     poolCBP;
  PoolRange * poolRange;
  
  poolRange = DspuMalloc(sizeof(PoolRange));
  if (poolRange) {
    poolRange->next    = 0;
    poolRange->nextWait= 0;
    poolRange->luWait  = 0;          /* no lu is now waiting */
    poolRange->luStart = luStart;
    poolRange->puP     = puP;       /* owner of this pool range */
    poolRange->luRange = luRange;
    strcpy(poolRange->poolName,name);

    poolCBP = FindPool(poolRange->poolName);
    if (poolCBP) 
        IncrPoolDnLuDefs(poolCBP, poolRange->luRange);

    DspuEnqueue(PuGetPoolRangeHeader(puP), poolRange);
  }
  return poolRange;    
}

/*=====================================================================*
* PoolRange_dl
*======================================================================*
*
* Description:
*   This is the pool range destructor.
*/
void PoolRange_dl(PoolRange* this)
{
    PoolCB *poolCBP;

    poolCBP = FindPool(this->poolName);
    if (poolCBP) 
        DecrPoolDnLuDefs(poolCBP, this->luRange);
   DspuFree(this);
}

/*=====================================================================*
* PoolRange_free
*======================================================================*
*
* Description:
*   This subroutine finds the pool range control block for a given range
* parameters. This is called at configuration time when the POOL LU(s) are 
* disconnected from the downstream PU. i.e.  
*
*   DSPU NO LU luStart [luRange] POOL poolName [PU puName]
*
* This involves deleting the pool range control block.
*
* To make implementation simpler, luStart and luRange must match with the
* configured luStart and luRange.
*   
*/
PoolRange*                   /* 0 if poolRange not found  */
PoolRange_free
       (const char* poolName,  /* poolName */
        Pu*  puP,              /* Downstream Pu owner of this range */
        byte luStart,
        byte luRange           /* 0 -> not defined  */
       )
{
   PoolRange* poolP;
   
   poolP = GetFirstQelement(puP->poolRangeHeader);
   
   /* Scan through the poolRangeHeader list to find a pool range control 
    * which matches the given poolName, luStart and luRange. 
    */
   while (poolP)
   {
      if (strcmp(poolP->poolName, poolName) == 0 && 
          poolP->luStart == luStart && poolP->luRange == luRange) {
         return poolP;
      }
      poolP = poolP->next;
   }
   return 0;
}             
             


/*=====================================================================*
*  FindPool
*======================================================================*
*
* Description:
*   Scan through the luPoolRoot to find the pool with the given 
* pool name.
**/
PoolCB* FindPool(const char* poolName)
{
   PoolCB* poolP = GetFirstQelement(luPoolRoot);
   while (poolP) {

      if (strcmp(poolP->name, poolName) == 0)
         return poolP;
      poolP = poolP->next;
   }
   return 0;
}


/*=====================================================================*
* FindPooledHostLu()
*======================================================================*
*
* Description:
*   Scans HostLuPool queue for LU matching inputs:
*     luType = upstream/downstream
*     lu     = Lu local address
*
**/
Lu* FindPooledHostLu(PoolCB* poolCBP, LinkType luType, byte lu)
{

  boolean luFound = FALSE;


  Lu* luP = GetFirstQelement(GetPoolHostLuQhdr(poolCBP));
  while(luP && !luFound) {
    if ( (luType == Upstream) && (lu == GetUpLuNumber(luP)) )
      luFound = TRUE;
    else if ( (luType == Downstream) && (lu == GetDownLuNumber(luP)) )
      luFound = TRUE;
    else
      luP = NextLu(luP);
  }

  return(luP);

} /* End FindPooledHostLu() */



/*=====================================================================*
* FindWaitingLuPoolRange()
*======================================================================*
*
* Description:
*   Scans LuPoolWait queue for PoolRange matching input LU local address
*
**/
PoolRange* FindWaitingLuPoolRange(PoolCB* poolCBP, byte luAddr)
{

  return( FindPoolRangeLu(&GetPoolWaitLuQhdr(poolCBP), luAddr) );

} /* End FindWaitingLuPoolRange() */


/*=====================================================================*
* FindPoolRangeLu()
*======================================================================*
*
* Description:
*   Scans queue for PoolRange containing input LU local address
*
**/
PoolRange* FindPoolRangeLu(DspuQueueHeader* poolRangeQ, byte lu)
{

  PoolRange* prP = NULL;

  boolean luRangeFound = FALSE;

  prP = GetFirstQueue(poolRangeQ);
  while(prP && !luRangeFound) {
    if ( lu >= GetPoolRangeDnLuStart(prP) && lu <= GetPoolRangeDnLuEnd(prP) )
      luRangeFound = TRUE;
    else
      prP = NextQueueElement(prP);
  } /* End while - scan through poolRangeQ */

  return(prP);

} /* End FindPoolRangeLu() */

/*=====================================================================*
* FindNextPoolRangeLu()
*======================================================================*
*
* Description:
*   Scans queue for PoolRange containing next LU local address
*
**/
PoolRange* FindNextPoolRangeLu(DspuQueueHeader* poolRangeQ, byte *lu)
{

  PoolRange* prP = NULL;
  PoolRange* index_prP = NULL;

  byte inputLu = *lu;


  /* Try finding next LU in defined within an existing poolRange */
  /* ----------------------------------------------------------- */
  prP = FindPoolRangeLu(poolRangeQ, *lu);
  if (!prP) {

      /* Scan through poolRangeQ searching for next LU */
      /* --------------------------------------------- */
      
      *lu = 255;
      index_prP = GetFirstQueue(poolRangeQ);
      while (index_prP) {
          if ( inputLu < GetPoolRangeDnLuStart(index_prP) && 
               *lu > GetPoolRangeDnLuStart(index_prP) ) {
            *lu = GetPoolRangeDnLuStart(index_prP);
            prP = index_prP;
          }
          index_prP = NextQueueElement(index_prP);
      }
  }

  return(prP);

} /* End FindNextPoolRangeLu() */





void ShowPool(PoolCB* poolP, boolean all)
{
  Lu* luP = GetFirstQelement(GetPoolHostLuQhdr(poolP));
  const char* hostName;
  byte luStart;
  byte luEnd;
  byte inact = poolP->inactivityTimeout;

  if (luP) {
     while(luP) {
         luStart = GetUpLuNumber(luP);
         hostName = GetHostName(luP);
         luEnd = luStart;
         luP = NextLu(luP);
         while (luP && ((luEnd + 1) == GetUpLuNumber(luP)) && 
                 (strcmp(hostName,GetHostName(luP)) == 0)) {
             luEnd++;
             luP = NextLu(luP);
         }
         printf("dspu pool %s host %s lu %d %d",
                poolP->name, hostName, luStart, luEnd);
         if (inact) {
             printf(" inactivity-timeout %d",poolP->inactivityTimeout);
             inact = 0;
         }
         printf("\n");
     }

     if (all){
       luP = GetFirstQelement(GetPoolHostLuQhdr(poolP));
       while (luP){
         printf("    lu %d host %s ", GetUpLuNumber(luP), GetHostName(luP));
         if (!QueryLuFree(luP))
             printf("peer lu %d pu %s", GetDownLuNumber(luP), GetDownPuName(luP));
         printf(" status %s\n",LuStatus(luP));
         luP = NextLu(luP);
      }
    }
  }
  else
     printf("dspu pool %s -- no lu defined\n",poolP->name);
}

void ShowAllPool()
{
   PoolCB* poolP = GetFirstQelement(luPoolRoot);
   while (poolP) {
      ShowPool(poolP, FALSE);
      poolP = poolP->next;
   }
}

