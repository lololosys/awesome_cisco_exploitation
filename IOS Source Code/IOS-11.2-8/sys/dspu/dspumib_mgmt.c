/* $Id: dspumib_mgmt.c,v 3.2.60.1 1996/05/17 10:48:23 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspumib_mgmt.c,v $
 *------------------------------------------------------------------
 * DSPU MIB Management
 *
 * September 1994, Paul Pearce
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Define methods for managing the DSPU MIB informations tables
 *------------------------------------------------------------------
 * $Log: dspumib_mgmt.c,v $
 * Revision 3.2.60.1  1996/05/17  10:48:23  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/22  18:50:42  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.2  1995/11/17  09:05:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  21:13:03  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.1  1995/06/07  20:28:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "dspumib_mgmt.h"


#define FIRST_IDX   1L      /* First Index number */


/* Definitions for DspuPoolClassTable */
/* ---------------------------------- */

/* FirstEntry of DspuPoolClassTable */
static PoolCB* dspuPoolClassTableFirstEntryP;

/* PoolCB* GetFirstEntry_DspuPoolClassTable(void) */
#define GetFirstEntry_DspuPoolClassTable()  (dspuPoolClassTableFirstEntryP)

/* void SetFirstEntry_DspuPoolClassTable(PoolCB* poolP) */
#define SetFirstEntry_DspuPoolClassTable(poolP) \
            (dspuPoolClassTableFirstEntryP = poolP)


/* Definitions for DspuPuTable        */
/* ---------------------------------- */

/* FirstEntry of DspuPuTable */
static Pu* dspuPuTableFirstEntryP;

/* Pu* GetFirstEntry_DspuPuTable(void) */
#define GetFirstEntry_DspuPuTable()  (dspuPuTableFirstEntryP)

/* void SetFirstEntry_DspuPuTable(Pu* puP) */
#define SetFirstEntry_DspuPuTable(puP)  (dspuPuTableFirstEntryP = puP)


/*=====================================================================*
* InitDspuMibTables ()
*======================================================================*
*
* Description:
*   Initializes the FirstEntry pointers for DSPU MIB tables
*
**/
void InitDspuMibTables (void)
{

  SetFirstEntry_DspuPoolClassTable(NULL);
  SetFirstEntry_DspuPuTable(NULL);
  
}  /* End InitDspuMibTables () */


/*=====================================================================*
* AddEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Adds a pool entry to the dspuPoolClassTable
*
**/
void AddEntry_DspuPoolClassTable (PoolCB* inputPoolP) 
{

  PoolCB* poolP = GetFirstEntry_DspuPoolClassTable();
  PoolCB* nextPoolP = NULL;

  ulong   poolIdx = 0L;
  ulong   nextPoolIdx = 0L;
  boolean poolFound = FALSE;


  if (inputPoolP) {

      /*
       *----------------------------------------------------------------
       * if (no entries in table)
       *   add new entry as first entry
       * else if (FIRST_IDX index number unused)
       *   add new entry as first entry
       * else 
       *   scan through table searching for next unused index number
       *   add entry into table & assign unused index number
       *----------------------------------------------------------------
       **/
      
      if (!poolP) {
          SetFirstEntry_DspuPoolClassTable(inputPoolP);
          SetPoolDspuMibPoolIdx(inputPoolP, FIRST_IDX);
          SetPoolDspuMibNextPoolP(inputPoolP, NULL);

      } else if (GetPoolDspuMibPoolIdx(poolP) != FIRST_IDX) { 
          SetFirstEntry_DspuPoolClassTable(inputPoolP);
          SetPoolDspuMibPoolIdx(inputPoolP, FIRST_IDX);
          SetPoolDspuMibNextPoolP(inputPoolP, poolP);

      } else {

          while (!poolFound) {
              poolIdx = GetPoolDspuMibPoolIdx(poolP);
              nextPoolP = GetPoolDspuMibNextPoolP(poolP);
              if (nextPoolP) {
                  nextPoolIdx = GetPoolDspuMibPoolIdx(nextPoolP);
                  if ((nextPoolIdx != (poolIdx+1)) && (nextPoolIdx > poolIdx))
                      poolFound = TRUE;
                  else 
                      poolP = nextPoolP;
              } else {
                  poolFound = TRUE;
              }

          } /* End while (!poolFound) */

          SetPoolDspuMibPoolIdx(inputPoolP, (poolIdx+1));
          SetPoolDspuMibNextPoolP(inputPoolP, nextPoolP);
          SetPoolDspuMibNextPoolP(poolP, inputPoolP);

      } /* End else - Scan for next unused index */

  } 

} /* End AddEntry_DspuPoolClassTable () */


/*=====================================================================*
* RemoveEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Removes a pool entry from the dspuPoolClassTable
*
**/
void RemoveEntry_DspuPoolClassTable (PoolCB* inputPoolP) 
{

  PoolCB* poolP = GetFirstEntry_DspuPoolClassTable();
  PoolCB* nextPoolP = NULL;


  if (poolP) {

      if (poolP == inputPoolP) {
          SetFirstEntry_DspuPoolClassTable(GetPoolDspuMibNextPoolP(poolP));

      } else {

          nextPoolP = GetPoolDspuMibNextPoolP(poolP);
          while ( nextPoolP && (nextPoolP != inputPoolP) ) {
              poolP = nextPoolP;
              nextPoolP = GetPoolDspuMibNextPoolP(nextPoolP);
          }

          if (nextPoolP) 
            SetPoolDspuMibNextPoolP(poolP, GetPoolDspuMibNextPoolP(nextPoolP));
      } 

  } 

}  /* End RemoveEntry_DspuPoolClassTable () */


/*=====================================================================*
* FindEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Find a pool entry in the dspuPoolClassTable
*
**/
PoolCB* FindEntry_DspuPoolClassTable (ulong poolIdx)
{

  PoolCB* poolP = GetFirstEntry_DspuPoolClassTable();


  while ( poolP && (poolIdx != GetPoolDspuMibPoolIdx(poolP)) )
      poolP = GetPoolDspuMibNextPoolP(poolP);

  return(poolP);

} /* End FindEntry_DspuPoolClassTable () */


/*=====================================================================*
* FindNextEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Find next pool entry in the dspuPoolClassTable
*
**/
PoolCB* FindNextEntry_DspuPoolClassTable (ulong poolIdx)
{

  PoolCB* poolP = dspuPoolClassTableFirstEntryP;

  while ( poolP && (GetPoolDspuMibPoolIdx(poolP) < poolIdx) )
      poolP = GetPoolDspuMibNextPoolP(poolP);

  return(poolP);

} /* End FindNextEntry_DspuPoolClassTable () */


/*=====================================================================*
* AddEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Adds a PU entry to the dspuPuTable
*
**/
void AddEntry_DspuPuTable (Pu* inputPuP) 
{

  Pu* puP = GetFirstEntry_DspuPuTable();
  Pu* nextPuP = NULL;

  ulong   puIdx = 0L;
  ulong   nextPuIdx = 0L;
  boolean puFound = FALSE;


  if (inputPuP) {

      /*
       *----------------------------------------------------------------
       * if (no entries in table)
       *   add new entry as first entry
       * else if (FIRST_IDX index number unused)
       *   add new entry as first entry
       * else 
       *   scan through table searching for next unused index number
       *   add entry into table & assign unused index number
       *----------------------------------------------------------------
       **/
      
      if (!puP) {
          SetFirstEntry_DspuPuTable(inputPuP);
          SetPuDspuMibPuIdx(inputPuP, FIRST_IDX);
          SetPuDspuMibNextPuP(inputPuP, NULL);

      } else if (GetPuDspuMibPuIdx(puP) != FIRST_IDX) { 
          SetFirstEntry_DspuPuTable(inputPuP);
          SetPuDspuMibPuIdx(inputPuP, FIRST_IDX);
          SetPuDspuMibNextPuP(inputPuP, puP);

      } else {

          while (!puFound) {
              puIdx = GetPuDspuMibPuIdx(puP);
              nextPuP = GetPuDspuMibNextPuP(puP);
              if (nextPuP) {
                  nextPuIdx = GetPuDspuMibPuIdx(nextPuP);
                  if ((nextPuIdx != (puIdx+1)) && (nextPuIdx > puIdx))
                      puFound = TRUE;
                  else 
                      puP = nextPuP;
              } else {
                  puFound = TRUE;
              }

          } /* End while (!puFound) */

          SetPuDspuMibPuIdx(inputPuP, (puIdx+1));
          SetPuDspuMibNextPuP(inputPuP, nextPuP);
          SetPuDspuMibNextPuP(puP, inputPuP);

      } /* End else - Scan for next unused index */

  } 

} /* End AddEntry_DspuPuTable () */


/*=====================================================================*
* RemoveEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Removes a PU entry from the dspuPuTable
*
**/
void RemoveEntry_DspuPuTable (Pu* inputPuP) 
{

  Pu* puP = GetFirstEntry_DspuPuTable();
  Pu* nextPuP = NULL;


  if (puP) {

      if (puP == inputPuP) {
          SetFirstEntry_DspuPuTable(GetPuDspuMibNextPuP(puP));

      } else {

          nextPuP = GetPuDspuMibNextPuP(puP);
          while ( nextPuP && (nextPuP != inputPuP) ) {
              puP = nextPuP;
              nextPuP = GetPuDspuMibNextPuP(puP);
          }

          if (nextPuP) 
            SetPuDspuMibNextPuP(puP, GetPuDspuMibNextPuP(nextPuP));
      } 

  } 

}  /* End RemoveEntry_DspuPuTable () */


/*=====================================================================*
* FindEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Find a PU entry in the dspuPuTable
*
**/
Pu* FindEntry_DspuPuTable (ulong puIdx)
{

  Pu* puP = GetFirstEntry_DspuPuTable();


  while ( puP && (puIdx != GetPuDspuMibPuIdx(puP)) )
      puP = GetPuDspuMibNextPuP(puP);

  return(puP);

} /* End FindEntry_DspuPuTable () */


/*=====================================================================*
* FindNextEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Find next PU entry in the dspuPuTable
*
**/
Pu* FindNextEntry_DspuPuTable (ulong puIdx)
{

  Pu* puP = GetFirstEntry_DspuPuTable();


  while ( puP && (GetPuDspuMibPuIdx(puP) < puIdx) )
      puP = GetPuDspuMibNextPuP(puP);

  return(puP);

} /* End FindNextEntry_DspuPuTable () */


/*=====================================================================*
* FindEntry_DspuLu ()
*======================================================================*
*
* Description:
*   Find the specified DSPU LU entry owned by the given PU
*
**/
Lu* FindEntry_DspuLu (Pu* puP, byte luAddr, PoolRange **prP)
{

  Lu* luP = NULL;


  if (puP) {

      luP = GetChild(GetLuChildren(puP), luAddr);
      if ( !luP && (GetPuType(puP) == Downstream) ) 
          *prP = FindPoolRangeLu(PuGetPoolRangeHeader(puP), luAddr);

  } /* End if - puP is not Null */

  if (QueryLuApiLu(luP))   /* filter out LUAPI LUs */
      luP = NULL;

  return(luP);

} /* End FindEntry_DspuLu () */


/*=====================================================================*
* FindNextEntry_DspuLu ()
*======================================================================*
*
* Description:
*   Find the next DSPU LU entry owned by the given PU
*
**/
Lu* FindNextEntry_DspuLu (Pu* puP, byte *luAddr, PoolRange **prP)
{

  Lu* luP = NULL;

  byte    tmpAddr = *luAddr;
  boolean luFound = FALSE;


  if (puP) {

      /* Try finding LU on active-LU-list */
      /* -------------------------------- */
      luP = GetChild(GetLuChildren(puP), *luAddr);
      if (luP)
          luFound = TRUE;

      /* Try finding DownstreamLu on waiting-pool-queue */
      /* ---------------------------------------------- */
      if ( !luFound && (GetPuType(puP) == Downstream) ) {
          *prP = FindPoolRangeLu(PuGetPoolRangeHeader(puP), *luAddr);
          if(*prP)
              luFound = TRUE; 
      }   

      /* Find next LU on active-LU-list                  */
      /*     and next DownstreamLu on waiting-pool-queue */
      /* ----------------------------------------------- */
      if (!luFound) {
      
          luP = GetNextChild(GetLuChildren(puP), *luAddr);
          if (GetPuType(puP) == Upstream) {

              /* filter out LUAPI LUs */
              while (luP && QueryLuApiLu(luP))
                  luP = GetNextChild(GetLuChildren(puP),
                                     GetUpLuNumber(luP));

              if (luP)
                  *luAddr = GetUpLuNumber(luP);

          } else {

              *prP = FindNextPoolRangeLu(PuGetPoolRangeHeader(puP), &tmpAddr);
              if (luP && !prP) {
                  *luAddr = GetDownLuNumber(luP);
              } else if (!luP && prP) {
                  *luAddr = tmpAddr;
              } else if (luP && prP) {
                  if (tmpAddr < GetDownLuNumber(luP)) {
                      luP = NULL;
                      *luAddr = tmpAddr;
                  } else {
                      *prP = NULL;
                      *luAddr = GetDownLuNumber(luP);
                  }
              } 
            
          } /* End else - DownstreamLu */

      } /* End if - LU not found */
      
  } /* End if - puP is not Null */

  return(luP);

} /* End FindNextEntry_DspuLuTable () */


/*=====================================================================*
* FindEntry_DspuSapTable ()
*======================================================================*
*
* Description:
*   Find the specified SAP entry owned by the given adaptor
*
**/
Sap* FindEntry_DspuSapTable (byte sapAddr, AdaptorId *adaptorIdP)
{

  Adaptor* adaptorP = NULL;
  Sap* sapP = NULL;

  adaptorP = QueryAllChildrenParm( GetAdaptorRoot(),
                                   (ProcQChild1) AdaptorFind, adaptorIdP);

  if (adaptorP) {
      sapP = GetChild(GetSapChildren(adaptorP), sapAddr);
  }

  return(sapP);

} /* End FindEntry_DspuSapTable() */


/*=====================================================================*
* FindNextEntry_DspuSapTable ()
*======================================================================*
*
* Description:
*   Find the next SAP entry owned by the given adaptor
*
**/
Sap* FindNextEntry_DspuSapTable (byte sapAddr, AdaptorId *adaptorIdP)
{

  AdaptorId   tmpAdaptorId;
  Adaptor*    adaptorP = NULL;

  Sap* sapP = NULL;
  boolean sapFound = FALSE;


  /* If adaptorId does not identify a "real" adaptor             */
  /*   Find next adaptor using lexicographical ordering criteria */
  /* ----------------------------------------------------------- */
  tmpAdaptorId.dlcType            = GetAdaptorDlcType(adaptorIdP);
  tmpAdaptorId.u.adaptornumber    = GetAdaptorDlcNumber(adaptorIdP);
  tmpAdaptorId.adaptorSubIfNumber = GetAdaptorDlcSubIf(adaptorIdP);
  adaptorP = QueryAllChildrenParm(GetAdaptorRoot(),
                                  (ProcQChild1) AdaptorFind, &tmpAdaptorId);

  if (!adaptorP) {
      sapAddr = 0;
      adaptorP = FindNextAdaptor(&tmpAdaptorId);
  }

  /* Search for next Sap defined on the adaptor */
  /*                                            */
  /* If Sap not found,                          */
  /*   continue search for Sap on next adaptor  */
  /* ------------------------------------------ */
  while (adaptorP && !sapFound) {

      sapP = GetChild(GetSapChildren(adaptorP), sapAddr);
      if (sapP)
        sapFound = TRUE;

      if (!sapFound) {
          sapP = GetNextChild(GetSapChildren(adaptorP), sapAddr);
          if (sapP)
            sapFound = TRUE;
      }

      if (!sapFound) {
          sapAddr = 0;
          adaptorP = FindNextAdaptor(&tmpAdaptorId);
      }

  } /* End while (adaptorP && !SapFound) */

  return(sapP);

} /* End FindNextEntry_DspuSapTable() */

