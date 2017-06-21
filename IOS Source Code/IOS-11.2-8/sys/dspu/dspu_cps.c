/* $Id: dspu_cps.c,v 3.2.6.1 1996/08/28 12:41:57 thille Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_cps.c,v $
 *------------------------------------------------------------------
 * DSPU Child/Parent Services.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module simply maintains a 2 level index structure for the storing
 * and retrieving of addresses of core DSPU structures. For the case where
 * only one address is stored, we subvert the index levels and store the
 * (core DSPU structure) address in the location of the primary index list.
 *
 * Notes:
 * 1. During Child insertion and deletion we hold off all interrupts to
 *    ensure atomicity. This prevents concurrency problems of accessing
 *    and updating data, due to re-entrancy.
 *
 * 2. We differentiate between single and multiple siblings for speed
 *    and memory considerations only.
 *
 * 3. DoToAllChildren has two parameter passing variants. Please reduce
 *    these to one sometime soon.
 *------------------------------------------------------------------
 * $Log: dspu_cps.c,v $
 * Revision 3.2.6.1  1996/08/28  12:41:57  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.2  1996/03/05  03:25:18  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.1  1995/11/09  11:28:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:45  hampton
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
#include "dmalloc.h"

#include "dspu_cps.h"
#include "dspu_interr.h"
#include "dspu_pu.h"
#include "dspu_error.h"

/*==========================================================================*
 * static void InitSiblinIndex(SiblingIndex** sibling)
 *==========================================================================*
 *
 * Description:
 * Initialize sibling index structure.
 *
 **/

static void
InitSiblingIndex (Child_t* sibling)

{

   byte i = MAX_SIBLINGS;
   while (i-- > 0)
     *sibling++ = 0;

}  /* InitSiblingIndex. */

/*==========================================================================*
 * static void SiblingIndex_nw()
 *==========================================================================*
 *
 * Description:
 *   SiblingIndex constructor
 *
 **/
static SiblingIndex*
SiblingIndex_nw(void)
{
   SiblingIndex* this = DspuMalloc (sizeof(SiblingIndex));
   if (this) {
      this->inuse = 0;
      InitSiblingIndex(this->idxP);
   }
   return this;
}


/*==========================================================================*
 * InitChild:
 *==========================================================================*
 *
 * Description:
 * Initialize a child locator structure.
 *
 **/

boolean
InitChild (SiblingType sibtype,         /* Single/Multiple siblings. */
           Sibling *childLocP)          /* Ptr to child locator structure. */

{
   childLocP->sibtype = sibtype;
   if (sibtype == SINGLE) {
       childLocP->idxP[0] = 0;
   }
   else {
        InitSiblingIndex(childLocP->idxP);
    }
 return TRUE;

}  /* InitChild. */


/*==========================================================================*
 * GetChild:
 *==========================================================================*
 *
 * Description:
 * Get address of a specified child from child locator structure.
 *
 **/

void *
GetChild (Sibling *childLocP,           /* Ptr to child locator structure. */
          hword chindx)                 /* Index of child of interest. */

{

   SiblingIndex *idxP;
   if (childLocP->sibtype == SINGLE) {
        return childLocP->idxP[0];
   }
   else {
        if (chindx < childLocP->sibtype * MAX_SIBLINGS &&
            (idxP = childLocP->idxP[chindx / MAX_SIBLINGS]) != 0  &&
             idxP->inuse > 0)
              return idxP->idxP[chindx % MAX_SIBLINGS];
   }
   return (0);

}  /* GetChild. */


/*==========================================================================*
 * GetNextChild:
 *==========================================================================*
 *
 * Description:
 * Get address of next child structure from child locator structure.
 *
 **/

void *
GetNextChild (Sibling *childLocP,       /* Ptr to child locator structure. */
              hword chindx)             /* Index of child to start from. */

{
  hword pIdx, sIdx;
  SiblingIndex *idxP;


  if (chindx >=  childLocP->sibtype * MAX_SIBLINGS)     /* make sure index is correct */
      return (0);

   if (childLocP->sibtype == SINGLE) {
        return childLocP->idxP[0];
   }
   else {
        sIdx = (++chindx) % MAX_SIBLINGS;
        for (pIdx = chindx / MAX_SIBLINGS; pIdx < childLocP->sibtype; pIdx++){
           if ((idxP = childLocP->idxP[pIdx]) != 0){

              while (sIdx < MAX_SIBLINGS){
                 void* childObjP;
                 if ((childObjP = idxP->idxP[sIdx++]) != 0)
                    return (childObjP);
              }
           }
           sIdx = 0; /* start with sIdx = 0 for next pIdx */
        }

     }

  return (0);

}  /* GetNextChild. */


/*==========================================================================*
 * GetFirstChild:
 *==========================================================================*
 *
 * Description:
 * Get address of first child structure from child locator structure.
 *
 **/

void *
GetFirstChild (Sibling *childLocP)      /* Ptr to child locator structure. */

{

   hword  pIdx, sIdx;
   SiblingIndex *idxP;

   if (childLocP->sibtype == SINGLE) {
        return childLocP->idxP[0];
   }
   else {
        sIdx = 0;
        for (pIdx = 0; pIdx < childLocP->sibtype; pIdx++){
           
           if ((idxP = childLocP->idxP[pIdx]) != 0){

              for (sIdx = 0; sIdx < MAX_SIBLINGS; sIdx++){
                 void* childObjP;
                 if ((childObjP = idxP->idxP[sIdx]) != 0)
                    return (childObjP);
              }
           }
        }
     }

    return (0);

}  /* GetFirstChild. */


/*==========================================================================*
 * GetFirstFreeChild:
 *==========================================================================*
 *
 * Description:
 * Get index of first free child structure from child locator structure.
 *
 **/

hword
AddChildFirstFree (Sibling *childLocP,  /* Ptr to child locator structure. */
                   void *childObjP,     /* Addr of child struct to add. */
                   Parent_t parentP,     /* Address of parent of child. */
                   Parent_t* addrParentP)  /* Address of above argument. */

{

   SiblingIndex *idxP;
   boolean foundFree;
   hword pIdx, sIdx;
   hword sibIndex;

   foundFree = FALSE;
   sibIndex = BAD_SIBLING_INDEX;


   DISABLE_INTERRUPT                /* Don't allow interruptions. */

   *addrParentP = parentP;              /* ie childObjP->parentP = parentP; */
   if (childLocP->sibtype == SINGLE) {
      if (childLocP->idxP[0] == 0) {
           childLocP->idxP[0]  = childObjP;
           foundFree = TRUE;        /* find a free slot */
           sibIndex = 0;
      }
   }
   else {
         sibIndex = 0;
         for (pIdx = 0; pIdx < childLocP->sibtype && !foundFree; pIdx++){
            
            if ((idxP = childLocP->idxP[pIdx]) == 0){
                idxP = SiblingIndex_nw();
                if (idxP) {
                   childLocP->idxP[pIdx] = idxP;
                }
                else{
                   sibIndex = BAD_SIBLING_INDEX;
                   foundFree = TRUE;      /* break the outer loop */
                   break;
                }
            }
            for (sIdx = 0; sIdx < MAX_SIBLINGS; sIdx++){
                if (idxP->idxP[sIdx] == 0){
                   idxP->idxP[sIdx] = childObjP;
                   idxP->inuse++;
                   foundFree = TRUE;
                   break;
                }
                sibIndex++;      /* increment the child index */
            }
         }

      }

   ENABLE_INTERRUPT                 /* Allow interruptions. */
    /* 
     * Return BAD_SIBLING_INDEX 
     *   if no free slots available to add child
     *
     */
    if (!foundFree)
      sibIndex = BAD_SIBLING_INDEX;
 
   return (sibIndex);

}  /* AddChildFirstFree. */


/*==========================================================================*
 * AddChild:
 *==========================================================================*
 *
 * Description:
 * Add address of specified structure to child locator structure.
 *
 **/

boolean
AddChild (Sibling *childLocP,           /* Ptr to child locator structure. */
          hword chindx,                 /* The actual child. */
          void *childObjP,              /* Address of child structure to add. */
          Parent_t parentP,              /* Address of parent of child. */
          Parent_t* addrParentP)           /* Address of above argument. */

{

   boolean added;
   hword pIdx, sIdx;
   SiblingIndex* idxP;


   added = FALSE;

   DISABLE_INTERRUPT                /* Don't allow interruptions. */

   *addrParentP = parentP;              /* ie childObjP->parentP = parentP; */
   if (childLocP->sibtype == SINGLE) {
      if (childLocP->idxP[0] == 0) {
           childLocP->idxP[0]  = childObjP;
           added = TRUE;
      }
   }
   else {
         pIdx = chindx / MAX_SIBLINGS;
         sIdx = chindx % MAX_SIBLINGS;
         if ((idxP = childLocP->idxP[pIdx]) != 0){
            idxP->idxP[sIdx] = childObjP;
            idxP->inuse++;
            added = TRUE;
            }
         else {
            if ((idxP = SiblingIndex_nw()) != 0) {
               childLocP->idxP[pIdx] = idxP;
               idxP->idxP[sIdx] = childObjP;
               idxP->inuse = 1;
               added = TRUE;
               }
            }
    
      }

   ENABLE_INTERRUPT                 /* Allow interruptions. */

   return (added);

}  /* AddChild. */


/* Check if any children existed */

boolean QueryAnyChild (Sibling* this)
{
    hword pIdx;
    for (pIdx = 0; pIdx < this->sibtype; pIdx++){
        if (this->idxP[pIdx] != 0)
           return TRUE;
    }
    return FALSE;
}

/*==========================================================================*
 * DelChild:
 *==========================================================================*
 *
 * Description:
 * Remove specified child from child locator structure.
 *
 **/

boolean
DelChild (Sibling *childLocP,           /* Ptr to child locator structure. */
          hword chindx,                 /* The child to delete. */
          Parent_t* addrParentP)           /* Address of ptr to parent in child. */

{

   boolean deleted;
   hword pIdx, sIdx;
   SiblingIndex *idxP;


   deleted = FALSE;

   DISABLE_INTERRUPT                /* Don't allow interruptions. */

   *addrParentP = NULL;                 /* ie childObjP->parentP = NULL; */
   if (childLocP->sibtype == SINGLE) {
      childLocP->idxP[0] = 0;
      deleted = TRUE;
   }
   else {
         pIdx = chindx / MAX_SIBLINGS;
         sIdx = chindx % MAX_SIBLINGS;
         if ((idxP = childLocP->idxP[pIdx]) != 0) {
            idxP->idxP[sIdx] = NULL;
            if (--idxP->inuse == 0) {
               DspuFree (idxP);
               childLocP->idxP[pIdx] = NULL;
               }
            deleted = TRUE;
            }

      }

   ENABLE_INTERRUPT                 /* Allow interruptions. */

   return (deleted);

}  /* DelChild. */


/*==========================================================================*
 * Children iteration:
 *==========================================================================*
 *
 * Description:
 *    Macro definition for the execution of children loop to be used in:
 * ChildLoopBegin
 *    childprocedure
 * ChildLoopEnd
 *
 **/

#define ChildLoopBegin                                \
   hword pIdx, sIdx;                                  \
   SiblingIndex *idxP;                                \
   void* childObj;                                    \
   for (pIdx = 0; pIdx < this->sibtype;pIdx++) {       \
      for (sIdx = 0; sIdx < MAX_SIBLINGS; sIdx++) {   \
         if ((idxP = this->idxP[pIdx]) == 0)          \
            break;                                    \
         else if ((childObj = idxP->idxP[sIdx]) != 0)

#define ChildLoopEnd }}


/*==========================================================================*
 * DoToAllChildrenParm:
 *==========================================================================*
 *
 * Description:
 * Execute procedure iteratively over children until children-set end reached.
 *
 **/

void
DoToAllChildren (Sibling   *this,
                 ProcChild proc)

{
  ChildLoopBegin
             proc(childObj);
  ChildLoopEnd

}  /* DoToAllChildren. */


/*==========================================================================*
 * DoToAllChildrenParm:
 *==========================================================================*
 *
 * Description:
 * Execute procedure iteratively over children until children-set end reached.
 * Also, pass parameter (hword) to procedure being executed.
 *
 **/

void
DoToAllChildrenParm (Sibling   *this,
                     ProcChild1 proc,
                     hword     param)

{

  ChildLoopBegin
            proc(childObj, param);
  ChildLoopEnd

}  /* DoToAllChildrenParm. */
    

/*==========================================================================*
 * DoToAllChildrenParm2:
 *==========================================================================*
 *
 * Description:
 * Execute procedure iteratively over children until children-set end reached.
 * Also, pass parameter (pointer) to procedure being executed.
 *
 **/

void
DoToAllChildrenParm2 (Sibling   *this,
                      ProcChild2 proc,
                      void *parmP)

{

  ChildLoopBegin
         proc(childObj, parmP);
  ChildLoopEnd

}  /* DoToAllChildrenParm2. */
    

/*==========================================================================*
 * QueryAllChildren:
 *==========================================================================*
 *
 * Description:
 * Execute procedure iteratively over children until procedure returns
 * true or children-set end reached.
 *
 **/

void*
QueryAllChildren (Sibling   *this,
                  ProcQChild proc)

{

   ChildLoopBegin{
        if (proc(childObj))
           return childObj;
   }
   ChildLoopEnd
   return 0;

}  /* QueryAllChildren. */


/*==========================================================================*
 * QueryAllChildrenParm:
 *==========================================================================*
 *
 * Description:
 * Execute procedure iteratively over children until procedure returns
 * true or children-set end reached.
 *
 **/

void*
QueryAllChildrenParm (Sibling   *this,
                  ProcQChild1 proc,
                  void * parmP)

{
   ChildLoopBegin{
        if (proc(childObj, parmP))
             return childObj;
   }
   ChildLoopEnd
   return 0;

}  /* QueryAllChildrenParm. */

