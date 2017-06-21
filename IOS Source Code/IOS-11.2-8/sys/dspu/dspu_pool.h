/* $Id: dspu_pool.h,v 3.1.62.1 1996/07/29 07:16:37 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_pool.h,v $
 *------------------------------------------------------------------
 * DSPU LU Pool Support.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * LU Pool Support header.  
 *------------------------------------------------------------------
 * $Log: dspu_pool.h,v $
 * Revision 3.1.62.1  1996/07/29  07:16:37  lcox
 * CSCdi63645:  dspu inactivity timeout doesnt get cleaned up properly.
 * Branch: California_branch
 *
 * Revision 3.1  1995/11/09  11:28:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/09/20  17:03:05  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.1  1995/09/14 06:15:14  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPU_POOL_H__
#define __DSPU_POOL_H__

#include "dtypes.h"
#include "dspu_const.h"
#include "dspuqueu.h"
#include "dspu_pu.h"


/*=====================================================================*
* PoolCB
*======================================================================*
*
* Description: 
*   This is constructed at configuration time for a new poolName:
*   POOL poolName HOST hostName LU luStart luRange
*
*   where poolName is defined here and hostName and lu are chained headed
* by hostLuPoolHeader
*
**/
struct PoolCB{
        struct PoolCB* next;            
        char   name[MAX_ENTRYNAME_LN+1];   /* Name of this pool */
        byte   inactivityTimeout;          /* timeout on inactivity */
        DspuQueueHeader hostLuPoolHeader;  /* HostLuPool lu chain header */
        DspuQueueHeader waitQueueHeader;   /* Pool range waiting to connect */
        ulong           dspuMibPoolIdx;    /* Index into dspuPoolClassTable */
        long            upStreamLuDefs;    /* Num of upstream LUs in pool   */
        long            dnStreamLuDefs;    /* Num of downstream LUs in pool */
        struct PoolCB*  dspuMibNextPoolP;  /* Next entry in PoolClassTable  */
        } ;

PoolCB * PoolCB_nw                      /* constructor */
           (char* poolName,
            hword inactivityTimeout
           );

void PoolChangeInactTimeout (PoolCB * poolCBP,
                             byte inactivityTimeout);

void PoolCB_dl(PoolCB* this);           /* destructor */

/*=====================================================================*
* InitPoolRoot
*======================================================================*
*
* Description:
*   Initialises the Pool root.
*
**/
void InitPoolRoot
           (void
           );

/*=====================================================================*
* PoolRange
*======================================================================*
*
* Description:
*    This is constructed at configuration Pool or Lu statement. This
* structure is headed by the PU poolRangeHeader. The lu can either be part
* of the pool or can be dedicated.
*
**/

struct PoolRange{               /* Pool Range structure. */
        struct PoolRange*  next;    /* Pu  chaining ptr */
        void*  nextWait;            /* waitQueueHeader chain */
        Pu*    puP;                 /* owner of this pool range */
        byte   luStart;             /* The LU "extent": beginning LU. */
        byte   luRange;             /* Number of LUs in extent. */
        byte   luWait;              /* lu waiting for this pool */
        
        char   poolName[MAX_ENTRYNAME_LN+1]; /* Pool control block    */
        };

/* The waitQueueHeader points to the nextWait which also points to the 
* nextWait of next structure. Such structure allows Queue primitive to 
* work properly. The following defined moves the pointer back to PoolRange.
*/

#define PoolRangePointer(next) \
  (PoolRange *)(((char*)next) - offsetof(PoolRange,nextWait))

PoolRange* PoolRange_nw              /* constructor */
             (Pu*  puP,              /* owner of this pool range */
             byte luStart,
             byte luRange,
             const char* name      /* pool control block */
             );
void PoolRange_dl(PoolRange* this);
        

/*=====================================================================*
* PoolCB* FindPool(const char* poolName)
*======================================================================*
*
* Description:
*    Scan the pool list using pool name 
**/
PoolCB* FindPool(const char* poolNameP);


/*=====================================================================*
* PoolCheckTimer
*======================================================================*
*
* Description:
*   This routine is called every minute to check if lu has any activity
* during the last call
*
*/
void PoolCheckTimer(void);


       
/*=====================================================================*
* FindPoolRange
*======================================================================*
*
* Description:
*    Find the pool range control block from a given lu of pu
*/
               
PoolRange* FindPoolRange(Pu* puP, byte lu);
       

       
/*=====================================================================*
* PoolRange_free
*======================================================================*
*
* Description:
*   This is called at config 
* NO DSPU LU luStart [luRange] POOL poolName
*
* To make implementation simpler, luStart and luRange must match with the
* configured luStart and luRange
*   
*/
PoolRange*                   /* 0 if pool range not found  */
  PoolRange_free
       (const char* poolName,  /* poolName */
        Pu*  puP,         /* Downstream Pu owner of this range */
        byte luStart,
        byte luRange      /* 0 -> not defined  */
       );


   
/*=====================================================================*
* DeletePool
*======================================================================*
*
* Description:
*   This routine is called to delete a pool control block at:
*   NO POOL poolName [HOST hostName LU luStart [luRange]].
*/
boolean                       /* FALSE if delete fail */
   DeletePool(PoolCB*      poolCBP);

   
/*=====================================================================*
* GetFreeLuFromHost
*======================================================================*
*
* Description:
*   Scans through the pool control block to find a free lu. 
*   
**/
Lu* GetFreeLuFromHost
             (PoolCB* poolCBP    /* pool control block defined by poolName */
             );

/*=====================================================================*
* GetNextLuWait
*======================================================================*
*
* Description:
*    A host lu is now free for connection, scanning through the pool range
* list to find a lu that is waiting to connect.
*   
**/
Lu* GetNextLuWait(PoolCB* poolCBP);

/*=====================================================================*
* PutLuWait
*======================================================================*
*
* Description:
*    A downstream lu had been disconnected. Put it in the wait queue list
*
*/
void PutLuWait(PoolCB* poolCBP,     /* lu's pool control block */
               Pu* puP, 
               byte    lu);         /* lu's number */

/*=====================================================================*
* RemoveAllLuFromWaitList
*======================================================================*
*
* Description:
*   Remove all lu range defined in the pu from the waiting list. This is 
* called when the Pu is changed to inactive
*
*/
void RemoveAllLuFromWaitList(Pu* pu);


void PrintRange(Pu* puP);

/* DspuQueueHeader GetLuPoolRoot() */
#define GetLuPoolRoot()  (luPoolRoot)

#define GetPoolName(p) (p->name)
/* const char* GetPoolName(PoolCB* p) {return p->name;} */

/* byte GetPoolInactivityTimeout(PoolCB* poolCBP) */
#define GetPoolInactivityTimeout(poolCBP)  (poolCBP->inactivityTimeout)

/* DspuQueueHeader GetPoolHostLuQhdr(PoolCB* poolCBP) */
#define GetPoolHostLuQhdr(poolCBP)  (poolCBP->hostLuPoolHeader)

/* DspuQueueHeader GetPoolWaitLuQhdr(PoolCB* poolCBP) */
#define GetPoolWaitLuQhdr(poolCBP)  (poolCBP->waitQueueHeader)

/* Pu* GetPoolRangeDnPuName(PoolRange* prP) */
#define GetPoolRangeDnPuName(prP)  (GetPuName(prP->puP))

/* byte GetPoolRangeDnLuStart(PoolRange* prP) */
#define GetPoolRangeDnLuStart(prP)  (prP->luStart)

/* byte GetPoolRangeDnLuEnd(PoolRange* prP) */
#define GetPoolRangeDnLuEnd(prP)  (prP->luStart + prP->luRange - 1)

/* char* GetPoolRangePoolName(PoolRange* prp) */
#define GetPoolRangePoolName(prP)  (prP->poolName)

/* long GetPoolUpLuDefs(PoolCB* p) */
#define GetPoolUpLuDefs(p)  (p->upStreamLuDefs)

/* void IncrPoolUpLuDefs(PoolCB* p) */
#define IncrPoolUpLuDefs(p)  (p->upStreamLuDefs++)

/* void DecrPoolUpLuDefs(PoolCB* p) */
#define DecrPoolUpLuDefs(p)  (p->upStreamLuDefs--)

/* long GetPoolDnLuDefs(PoolCB* p) */
#define GetPoolDnLuDefs(p)  (p->dnStreamLuDefs)

/* void IncrPoolDnLuDefs(PoolCB* p, byte n) */
#define IncrPoolDnLuDefs(p,n)  (p->dnStreamLuDefs = p->dnStreamLuDefs+n)

/* void DecrPoolDnLuDefs(PoolCB* p, n) */
#define DecrPoolDnLuDefs(p,n)  (p->dnStreamLuDefs = p->dnStreamLuDefs-n)

/* PoolCB* GetPoolDspuMibNextPoolP(PoolCB* p) */
#define GetPoolDspuMibNextPoolP(p)  (p->dspuMibNextPoolP) 

/* void SetPoolDspuMibNextPoolP(PoolCB* p, PoolCB* nextP) */
#define SetPoolDspuMibNextPoolP(p, nextP)  (p->dspuMibNextPoolP = nextP)

/* ulong GetPoolDspuMibPoolIdx(PoolCB* p) */
#define GetPoolDspuMibPoolIdx(p)  (p->dspuMibPoolIdx)

/* void SetPoolDspuMibPoolIdx(PoolCB* p, ulong idx) */
#define SetPoolDspuMibPoolIdx(p, idx)  (p->dspuMibPoolIdx = idx)


/*=====================================================================*
* FindPoolRangeLu()
*======================================================================*
*
* Description:
*   Scans queue for PoolRange containing input LU local address
*
**/
PoolRange* FindPoolRangeLu(DspuQueueHeader* poolRangeQ, byte lu);


/*=====================================================================*
* FindNextPoolRangeLu()
*======================================================================*
*
* Description:
*   Scans queue for PoolRange containing next LU local address
*
**/
PoolRange* FindNextPoolRangeLu(DspuQueueHeader* poolRangeQ, byte *lu);


/*=====================================================================*
* FindPooledHostLu()
*======================================================================*
*
* Description:
*   Scans HostLuPool queue for LU matching input LU local address
*
**/
Lu* FindPooledHostLu(PoolCB* poolCBP, LinkType luType, byte lu);


/*=====================================================================*
* FindWaitingLuPoolRange()
*======================================================================*
*
* Description:
*   Scans LuPoolWait queue for PoolRange matching input LU local address
*
**/
PoolRange* FindWaitingLuPoolRange(PoolCB* poolCBP, byte luAddr);

void ShowPool(PoolCB* poolP, boolean all);
void ShowAllPool(void);


#endif  /* __DSPU_POOL_H__ */
