/* $Id: dspumib_access.c,v 3.3.60.4 1996/08/16 12:27:07 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspumib_access.c,v $
 *------------------------------------------------------------------
 * DSPU MIB Access 
 *
 * September 1994, Paul Pearce
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Define methods for returning DSPU MIB object information 
 *------------------------------------------------------------------
 * $Log: dspumib_access.c,v $
 * Revision 3.3.60.4  1996/08/16  12:27:07  lcox
 * CSCdi66118:  getmany inf loop for dspu over FR and FDDI
 * Branch: California_branch
 *
 * Revision 3.3.60.3  1996/08/09  01:54:07  lcox
 * CSCdi61966:  dspu - maxiframe range incorrect for default pu
 * Branch: California_branch
 *
 * Revision 3.3.60.2  1996/07/24  01:23:43  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.3.60.1  1996/05/17  10:48:20  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.5  1996/05/05  22:55:31  ppearce
 * Sync to IbuMod_Calif_baseline_960504
 *
 * Revision 3.3.48.4  1996/05/03  17:18:35  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.3  1996/04/11  02:00:17  lcox
 * CSCdi52554:  incorrect value in dspuLuOperPeerPuIndex MIB entries
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.2  1996/03/28  08:20:41  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/03/22  18:50:39  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.3.4.1  1996/01/20  06:08:56  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.3  1995/11/17  09:05:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:50  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:28:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:56:36  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/19  21:13:01  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.4  1995/09/20  17:03:23  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.3  1995/09/14 06:15:22  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.2  1995/07/20  18:37:06  rbatz
 * CSCdi37406:  SNMP get of dspuPuOperState always returns active.
 *
 * Revision 2.1  1995/06/07  20:28:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */




#include "master.h"
#include "dspumib_access.h"
#include "dspumib_mgmt.h"
#include "dtypes.h"
#include "dspu_adaptor.h"
#include "dspu.h"
#include "dspu_lu.h"
#include "dspu_ls.h"
#include "dspu_pool.h"
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_sap.h"
#include "dspu_fp.h"
#include "parser_defs_dspu.h"

/* Forward declarations for internal procedures */
/* -------------------------------------------- */
static long GetDspuPuOperFsmState(Pu* puP);
static long GetDspuLuOperFsmState(Lu* luP);
static byte GetDspuMibDlcType(DspuDLCType_e dlcType);
static DspuDLCType_e GetDspuClsDlcType(byte dlcType);


/*=====================================================================*
* GetDspuNode () 
*======================================================================*
*
* Description:
*   Returns dspuNode information 
*   
*   Returns TRUE if DSPU is configured
*
**/
boolean GetDspuNode (dspuNode_t* nodeP)
{


  /* Return FALSE if DSPU is not configured */
  /* -------------------------------------- */
  if ( !IsDspuStarted() )
    return(FALSE);


  /* Only one dspuNodeEntry exists */
  /* ----------------------------- */


  /* dspuNodeRsrb */
  /* ------------ */
  if ( GetRsrbLocalRing() ) {

      nodeP->dspuNodeRsrb = D_dspuNodeRsrb_true;

      nodeP->dspuNodeRsrbLocalVirtualRing  = GetRsrbLocalRing();
      nodeP->dspuNodeRsrbBridgeNumber      = GetRsrbBridgeNumber();
      nodeP->dspuNodeRsrbTargetVirtualRing = GetRsrbTargetRing();

      nodeP->dspuNodeRsrbVirtualMacAddress->length = IEEEBYTES;
      nodeP->dspuNodeRsrbVirtualMacAddress->octet_ptr 
          = (char *) GetRsrbVirtualMacAddr();

  } else {

      nodeP->dspuNodeRsrb = D_dspuNodeRsrb_false;
      nodeP->dspuNodeRsrbLocalVirtualRing  = 0;
      nodeP->dspuNodeRsrbBridgeNumber      = 0;
      nodeP->dspuNodeRsrbTargetVirtualRing = 0;
      nodeP->dspuNodeRsrbVirtualMacAddress->length = 0;
      nodeP->dspuNodeRsrbVirtualMacAddress->octet_ptr = NULL;
  }


  /* dspuNodeDefaultPu */
  /* ----------------- */
  if ( GetDefaultPu() ) {
      nodeP->dspuNodeDefaultPu = D_dspuNodeDefaultPu_true;
      nodeP->dspuNodeDefaultPuWindowSize = GetPuCfgWindowSize(GetDefaultPu());
      nodeP->dspuNodeDefaultPuMaxIframe = GetPuCfgMaxIFrame(GetDefaultPu());
  } else {
      nodeP->dspuNodeDefaultPu = D_dspuNodeDefaultPu_false;
      nodeP->dspuNodeDefaultPuWindowSize = DSPU_DEFAULT_WINDOW;
      nodeP->dspuNodeDefaultPuMaxIframe = DSPU_DEFAULT_MAXIFRAME;
  }
  
  nodeP->dspuNodeActivationWindow  = SapGetActWindow();
  nodeP->dspuNodeLastConfigChgTime = SNMP_ABSOLUTE_TIME(GetDspuLastConfigTime());

  return(TRUE);

} /* End GetDspuNode () */


/*=====================================================================*
* GetDspuPoolClassEntry () 
*======================================================================*
*
* Description:
*   Returns dspuPoolClassEntry information 
*
**/
boolean GetDspuPoolClassEntry (dspuPoolClassEntry_t* entryP, int searchType)
{

  PoolCB* poolP = NULL;
  boolean entryFound = FALSE;


  if (searchType == EXACT)
    poolP = FindEntry_DspuPoolClassTable(entryP->dspuPoolClassIndex);
  else 
    poolP = FindNextEntry_DspuPoolClassTable(entryP->dspuPoolClassIndex);
 
  
  if (poolP) {
    entryP->dspuPoolClassIndex = GetPoolDspuMibPoolIdx(poolP);
    entryP->dspuPoolClassName->length = MAX_ENTRYNAME_LN;
    entryP->dspuPoolClassName->octet_ptr = GetPoolName(poolP);
    entryP->dspuPoolClassInactivityTimeout = GetPoolInactivityTimeout(poolP);
    entryP->dspuPoolClassOperUpStreamLuDefs = GetPoolUpLuDefs(poolP);
    entryP->dspuPoolClassOperDnStreamLuDefs = GetPoolDnLuDefs(poolP);
    entryFound = TRUE;
  }

  return(entryFound);

} /* End GetDspuPoolClassEntry () */


/*=====================================================================*
* GetDspuPooledLuEntry ()
*======================================================================*
*
* Description:
*   Returns dspuPooledLuEntry information
*
**/
boolean GetDspuPooledLuEntry (dspuPooledLuEntry_t* entryP, int searchType)
{

  PoolCB*    poolP   = NULL;
  Pu*        puP     = NULL;
  Pu*        peerPuP = NULL;
  Lu*        luP     = NULL;
  PoolRange* prP     = NULL;

  long poolIndex  = entryP->dspuPoolClassIndex;
  long puIndex    = entryP->dspuPuOperIndex;
  byte luAddr     = entryP->dspuLuOperLuLocalAddress;
  byte peerLuAddr = 0;

  boolean entryFound = FALSE;


  if (searchType == EXACT) {

      /*
       * ------------------------------------------------------
       * Find correct dspuPooledLu entry using indices:
       *   poolClassIndex.puOperIndex.luOperLocalAddress
       *     (1) Find pool with poolClassIndex                
       *     (2) Find PU/LU with puOperIndex/luOperLocalAddress
       *
       *      If LU belongs to pool, pooledLU found 
       * ------------------------------------------------------ 
       */
      poolP = FindEntry_DspuPoolClassTable(poolIndex);
      if (poolP) {
        puP = FindEntry_DspuPuTable(puIndex);
        if (puP) {
            luP = FindEntry_DspuLu(puP, luAddr, &prP);
            if (luP) {
                if (poolP == GetLuPool(luP))
                  entryFound = TRUE;
                
            } else if (prP) {
                if (poolP == FindPool(GetPoolRangePoolName(prP)))
                  entryFound = TRUE;
            } 
        }
      } /* End if - poolClass found */
  } else {


      /* Yuck - I hate this algorithm...                        */
      /*   searching through all PUs/LUs for each pool is awful */
      /*     try to fix this up later  --ppearce                */
     
      /*
       *---------------------------------------------------------------
       * Search for next pooledLu entry as follows:
       * 
       *   (1) Start with the current POOL and PU
       * 
       *   (2) Search through all LUs owned by the PU
       *         for the next LU belonging to POOL 
       * 
       *   (3) If next LU not found, try the next PU 
       *         until all PU/LUs searched
       * 
       *   (4) If next LU not found, repeat PU/LU search with next pool
       *          until no more pools                                   
       *---------------------------------------------------------------
       */
     
      do {
        poolP = FindNextEntry_DspuPoolClassTable(poolIndex);
        if (poolP) {
            poolIndex = GetPoolDspuMibPoolIdx(poolP)+1;
            do {
               puP = FindNextEntry_DspuPuTable(puIndex);
               if (puP) {
                   puIndex = GetPuDspuMibPuIdx(puP)+1;
                   do {
                       luP = FindNextEntry_DspuLu(puP, &luAddr, &prP);
                       if (luP) {
                           if (poolP == GetLuPool(luP))
                             entryFound = TRUE;
                           else
                             luAddr++;
                       } else if (prP) {
                           if (poolP == FindPool(GetPoolRangePoolName(prP)))
                             entryFound = TRUE;
                           else
                             luAddr++;
                       } else {
                           luAddr = 0;
                       }
                   } while (!entryFound && (luP || prP));
               }
            } while (!entryFound && puP);


            /* 
             * 
             * If next pooled LU entry not found,
             *   reset PU/LU so all PU/LUs will be searched for next pool
             * 
             */
            if (!entryFound) {
                puIndex = 0;
                luAddr  = 0;
            }
        }
      } while (!entryFound && poolP);
  } /* End else - searchType is  NEXT */

  if (entryFound) {

    entryP->dspuPoolClassIndex = GetPoolDspuMibPoolIdx(poolP);
    entryP->dspuPuOperIndex = GetPuDspuMibPuIdx(puP);
    entryP->dspuLuOperLuLocalAddress = luAddr;
    if (luP) {

        if (GetPuType(puP) == Upstream) {
          peerPuP = GetLuDnPuP(luP);
          peerLuAddr = GetDownLuNumber(luP);
        } else {
          peerPuP = GetLuUpPuP(luP);
          peerLuAddr = GetUpLuNumber(luP);
        }

        if (peerPuP) {
            entryP->dspuPooledLuPeerPuIndex = GetPuDspuMibPuIdx(peerPuP);
            entryP->dspuPooledLuPeerLuLocalAddress = peerLuAddr;
        } else {
            entryP->dspuPooledLuPeerPuIndex = 0;
            entryP->dspuPooledLuPeerLuLocalAddress = 0;
        }

    } else {
        entryP->dspuPooledLuPeerPuIndex = 0;
        entryP->dspuPooledLuPeerLuLocalAddress = 0;
    }

  } /* End if - pooledLu entry found */

  return(entryFound);

} /* End GetDspuPooledLuEntry() */



/*=====================================================================*
* GetDspuPuAdminEntry () 
*======================================================================*
*
* Description:
*   Returns dspuPuAdminEntry information 
*
**/
boolean GetDspuPuAdminEntry (dspuPuAdminEntry_t* entryP, int searchType)
{

  Pu* puP = NULL;
  long puIndex = entryP->dspuPuAdminIndex;
  boolean entryFound = FALSE;


  if (searchType == EXACT) {
      puP = FindEntry_DspuPuTable(puIndex);
      if (IsPuDefault(puP))
        puP = NULL;
  } else {
      do {
          puP = FindNextEntry_DspuPuTable(puIndex);
          puIndex = GetPuDspuMibPuIdx(puP)+1;
      } while ( puP && IsPuDefault(puP) );
  }

  if (puP) {

      entryP->dspuPuAdminIndex = GetPuDspuMibPuIdx(puP);

      entryP->dspuPuAdminName->length = MAX_ENTRYNAME_LN;
      entryP->dspuPuAdminName->octet_ptr = GetPuName(puP);

      if (GetPuType(puP) == Upstream)
          entryP->dspuPuAdminType = D_dspuPuAdminType_upstreamPu;
      else
          entryP->dspuPuAdminType = D_dspuPuAdminType_dnstreamPu;

      if ( (snpa_get_type(GetPuAddrP(puP)) == STATION_SDLC) ||
           (snpa_get_type(GetPuAddrP(puP)) == STATION_FR10) ||
           (snpa_get_type(GetPuAddrP(puP)) == STATION_X121) )
      {
          entryP->dspuPuAdminRemoteAddress->length = 
              snpa_get_length(GetPuAddrP(puP));
          snpa_copy_to_buffer(GetPuAddrP(puP), 
              (uchar *) entryP->dspuPuAdminRemoteAddress->octet_ptr);
          entryP->dspuPuAdminRemoteMacAddress->length = 0;
          entryP->dspuPuAdminRemoteMacAddress->octet_ptr = NULL;
      }
      else if (snpa_get_type(GetPuAddrP(puP)) == STATION_IEEE48)
      {
          entryP->dspuPuAdminRemoteMacAddress->length = 
              snpa_get_length(GetPuAddrP(puP));
          snpa_copy_to_buffer(GetPuAddrP(puP), 
              (uchar *) entryP->dspuPuAdminRemoteMacAddress->octet_ptr);
          entryP->dspuPuAdminRemoteAddress->length = 0;
          entryP->dspuPuAdminRemoteAddress->octet_ptr = NULL;
      } else {
          entryP->dspuPuAdminRemoteMacAddress->length = 0;
          entryP->dspuPuAdminRemoteMacAddress->octet_ptr = NULL;
          entryP->dspuPuAdminRemoteAddress->length = 0;
          entryP->dspuPuAdminRemoteAddress->octet_ptr = NULL;
      }

      if (GetPuRemoteSapAddr(puP) != NoSap)
          entryP->dspuPuAdminRemoteSapAddress = GetPuRemoteSapAddr(puP);
      else
          entryP->dspuPuAdminRemoteSapAddress = 0;

          
      if (GetPuLocalSapAddr(puP) != NoSap)
          entryP->dspuPuAdminLocalSapAddress  = GetPuLocalSapAddr(puP);
      else
          entryP->dspuPuAdminLocalSapAddress  = 0;

      entryP->dspuPuAdminXid   = GetPuCfgXid(puP);
      entryP->dspuPuAdminXidFmt = D_dspuPuAdminXidFmt_format3;

      entryP->dspuPuAdminWindowSize = GetPuCfgWindowSize(puP);
      entryP->dspuPuAdminMaxIframe  = GetPuCfgMaxIFrame(puP);

      entryP->dspuPuAdminLinkRetryCount   = GetPuMaxRetryCount(puP);
      entryP->dspuPuAdminLinkRetryTimeout = GetPuMaxRetryTimeout(puP);

      if ( IsPuStarted(puP) )
          entryP->dspuPuAdminStartPu = D_dspuPuAdminStartPu_true;
      else 
          entryP->dspuPuAdminStartPu = D_dspuPuAdminStartPu_false;

      entryP->dspuPuAdminDlcType = GetDspuMibDlcType(GetPuDlcType(puP));

      entryP->dspuPuAdminDlcUnit = GetPuDlcUnit(puP);
      entryP->dspuPuAdminDlcPort = GetPuDlcPort(puP);

      if ( puP == GetFocalPu() ) 
          entryP->dspuPuAdminFocalPoint = D_dspuPuAdminFocalPoint_true;
      else 
          entryP->dspuPuAdminFocalPoint = D_dspuPuAdminFocalPoint_false;

      entryP->dspuPuAdminRowStatus = D_dspuPuAdminRowStatus_active;

      entryFound = TRUE;

  } /* End if - PU entry found */

  return(entryFound);

} /* End GetDspuPuAdminEntry () */


/*=====================================================================*
* GetDspuPuOperEntry () 
*======================================================================*
*
* Description:
*   Returns dspuPuOperEntry information 
*
**/
boolean GetDspuPuOperEntry (dspuPuOperEntry_t* entryP, int searchType)
{

  Pu*        puP = NULL;
  LinkStn*   lsP = NULL;
  AdaptorId* adaptorIdP = NULL;

  boolean   entryFound = FALSE;


  if (searchType == EXACT) 
      puP = FindEntry_DspuPuTable(entryP->dspuPuOperIndex);
  else 
      puP = FindNextEntry_DspuPuTable(entryP->dspuPuOperIndex);

  if (puP) {

      entryP->dspuPuOperIndex = GetPuDspuMibPuIdx(puP);

      entryP->dspuPuOperName->length = MAX_ENTRYNAME_LN;
      entryP->dspuPuOperName->octet_ptr = GetPuName(puP);

      if (GetPuType(puP) == Upstream)
          entryP->dspuPuOperType = D_dspuPuOperType_upstreamPu;
      else
          entryP->dspuPuOperType = D_dspuPuOperType_dnstreamPu;

      lsP = GetPuLsP(puP);
      if (lsP) {

          if ( (snpa_get_type(LsGetAddrP(lsP)) == STATION_SDLC) ||
               (snpa_get_type(LsGetAddrP(lsP)) == STATION_FR10) ||
               (snpa_get_type(LsGetAddrP(lsP)) == STATION_X121) )
          {
              entryP->dspuPuOperRemoteAddress->length = 
                  snpa_get_length(LsGetAddrP(lsP));
              snpa_copy_to_buffer(LsGetAddrP(lsP),
                  (uchar *) entryP->dspuPuOperRemoteAddress->octet_ptr);
              entryP->dspuPuOperRemoteMacAddress->length = 0;
              entryP->dspuPuOperRemoteMacAddress->octet_ptr = NULL;
          }
          else if (snpa_get_type(LsGetAddrP(lsP)) == STATION_IEEE48)
          {
              entryP->dspuPuOperRemoteMacAddress->length = 
                  snpa_get_length(LsGetAddrP(lsP));
              snpa_copy_to_buffer(LsGetAddrP(lsP),
                  (uchar *) entryP->dspuPuOperRemoteMacAddress->octet_ptr);
              entryP->dspuPuOperRemoteAddress->length = 0;
              entryP->dspuPuOperRemoteAddress->octet_ptr = NULL;
          } 
          else {
              entryP->dspuPuOperRemoteMacAddress->length = 0;
              entryP->dspuPuOperRemoteMacAddress->octet_ptr = NULL;
              entryP->dspuPuOperRemoteAddress->length = 0;
              entryP->dspuPuOperRemoteAddress->octet_ptr = NULL;
          }

          entryP->dspuPuOperRemoteSapAddress 
                      = LsGetRemoteSapAddr(GetPuLsP(puP));
          entryP->dspuPuOperLocalSapAddress 
                      = LsGetLocalSapAddr(GetPuLsP(puP));
      } else {

          if ( (snpa_get_type(GetPuAddrP(puP)) == STATION_SDLC) ||
               (snpa_get_type(GetPuAddrP(puP)) == STATION_FR10) ||
               (snpa_get_type(GetPuAddrP(puP)) == STATION_X121) )
          {
              entryP->dspuPuOperRemoteAddress->length = 
                  snpa_get_length(GetPuAddrP(puP));
              snpa_copy_to_buffer(GetPuAddrP(puP),
                  (uchar * ) entryP->dspuPuOperRemoteAddress->octet_ptr);
              entryP->dspuPuOperRemoteMacAddress->length = 0;
              entryP->dspuPuOperRemoteMacAddress->octet_ptr = NULL;
          }
          else if (snpa_get_type(GetPuAddrP(puP)) == STATION_IEEE48) 
          {
              entryP->dspuPuOperRemoteMacAddress->length = 
                  snpa_get_length(GetPuAddrP(puP));
              snpa_copy_to_buffer(GetPuAddrP(puP),
                  (uchar * ) entryP->dspuPuOperRemoteMacAddress->octet_ptr);
              entryP->dspuPuOperRemoteAddress->length = 0;
              entryP->dspuPuOperRemoteAddress->octet_ptr = NULL;
          } 
          else {
              entryP->dspuPuOperRemoteMacAddress->length = 0;
              entryP->dspuPuOperRemoteMacAddress->octet_ptr = NULL;
              entryP->dspuPuOperRemoteAddress->length = 0;
              entryP->dspuPuOperRemoteAddress->octet_ptr = NULL;
          }

          if (GetPuRemoteSapAddr(puP) != NoSap)
              entryP->dspuPuOperRemoteSapAddress = GetPuRemoteSapAddr(puP);
          else
              entryP->dspuPuOperRemoteSapAddress = 0;

          if (GetPuLocalSapAddr(puP) != NoSap)
              entryP->dspuPuOperLocalSapAddress  = GetPuLocalSapAddr(puP);
          else
              entryP->dspuPuOperLocalSapAddress  = 0;
      }  

      if (GetPuType(puP) == Upstream) {

          entryP->dspuPuOperXid   = GetPuCfgXid(puP);
          entryP->dspuPuOperXidFmt = D_dspuPuAdminXidFmt_format3;

          entryP->dspuPuOperWindowSize = GetPuCfgWindowSize(puP);
          entryP->dspuPuOperMaxIframe  = GetPuCfgMaxIFrame(puP);

      } else {

          switch ( GetPuRcvdXidFmt(puP) ) {
              case xidFmt_0:
                  entryP->dspuPuOperXidFmt = D_dspuPuOperXidFmt_format0;
                  break;
              case xidFmt_3:
                  entryP->dspuPuOperXidFmt = D_dspuPuOperXidFmt_format3;
                  break;
              default:
                  entryP->dspuPuOperXidFmt = D_dspuPuOperXidFmt_formatUnknown;
                  break;

          } /* End switch ( GetPuRcvdXidFmt(puP) ) */

          entryP->dspuPuOperXid        = GetPuRcvdXid(puP);
          entryP->dspuPuOperWindowSize = GetPuNegWindowSize(puP);
          entryP->dspuPuOperMaxIframe  = GetPuNegMaxIFrame(puP);

      }

      entryP->dspuPuOperLinkRetryCount   = GetPuMaxRetryCount(puP);
      entryP->dspuPuOperLinkRetryTimeout = GetPuMaxRetryTimeout(puP);

      if ( IsPuStarted(puP) )
          entryP->dspuPuOperStartPu = D_dspuPuOperStartPu_true;
      else 
          entryP->dspuPuOperStartPu = D_dspuPuOperStartPu_false;

      if ( GetPuLsP(puP) && LsGetSap(GetPuLsP(puP)) ) 
          adaptorIdP = SapGetAdaptorId(LsGetSap(GetPuLsP(puP)));

      if (adaptorIdP) {
          entryP->dspuPuOperDlcType 
              = GetDspuMibDlcType( GetAdaptorDlcType(adaptorIdP) );
          entryP->dspuPuOperDlcUnit = GetAdaptorDlcUnit(adaptorIdP);
          entryP->dspuPuOperDlcPort = GetAdaptorDlcPort(adaptorIdP);
      } else {
          entryP->dspuPuOperDlcType = GetDspuMibDlcType(GetPuDlcType(puP));
          entryP->dspuPuOperDlcUnit = GetPuDlcUnit(puP);
          entryP->dspuPuOperDlcPort = GetPuDlcPort(puP);
      }

      if ( puP == GetFocalPu() ) 
          entryP->dspuPuOperFocalPoint = D_dspuPuOperFocalPoint_true;
      else 
          entryP->dspuPuOperFocalPoint = D_dspuPuOperFocalPoint_false;

      if ( GetPuOperState(GetPuType(puP), GetPuSscpState(puP)) )
          entryP->dspuPuOperState = D_dspuPuOperState_active;
      else
          entryP->dspuPuOperState = D_dspuPuOperState_inactive;

      entryP->dspuPuOperFsmState = GetDspuPuOperFsmState(puP);

      entryP->dspuPuOperStartTime = SNMP_ABSOLUTE_TIME(GetPuStartTime(puP));

      entryP->dspuPuOperLastStateChgTime = SNMP_ABSOLUTE_TIME(GetPuStateChgTime(puP));

      entryFound = TRUE;

  } /* End if - PU entry found */

  return(entryFound);

} /* End GetDspuPuOperEntry () */


/*=====================================================================*
* GetDspuPuStatsEntry () 
*======================================================================*
*
* Description:
*   Returns dspuPuStatsEntry information 
*
**/
boolean GetDspuPuStatsEntry (dspuPuStatsEntry_t* entryP, int searchType)
{

  Pu* puP = NULL;
  boolean entryFound = FALSE;


  if (searchType == EXACT) 
      puP = FindEntry_DspuPuTable(entryP->dspuPuOperIndex);
  else 
      puP = FindNextEntry_DspuPuTable(entryP->dspuPuOperIndex);

  if (puP) {

      entryP->dspuPuOperIndex = GetPuDspuMibPuIdx(puP);

      entryP->dspuPuStatsSentBytes  = GetPuByteSents(puP);
      entryP->dspuPuStatsRcvdBytes  = GetPuByteReceives(puP);
      entryP->dspuPuStatsSentFrames = GetPuFrameSents(puP);
      entryP->dspuPuStatsRcvdFrames = GetPuFrameReceives(puP);

      entryP->dspuPuStatsSentNegativeRsps = GetPuNegativeRspSents(puP);
      entryP->dspuPuStatsRcvdNegativeRsps = GetPuNegativeRspReceives(puP);

      entryP->dspuPuStatsActiveLus   = GetPuActiveSscpLuSessions(puP);
      entryP->dspuPuStatsInactiveLus = GetPuInactiveSscpLuSessions(puP);
      entryP->dspuPuStatsBindLus     = GetPuActiveLuLuSessions(puP);

      entryP->dspuPuStatsActivationFailures = GetPuActFailCnt(puP);

     switch ( GetPuLastActFailRsn(puP) ) {

          case PuActFailRsn_None:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_noError;
              break;

          case PuActFailRsn_Other:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_otherError;
              break;

          case PuActFailRsn_InternalError:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_internalError;
              break;

          case PuActFailRsn_ConfigError:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_configurationError;
              break;

          case  PuActFailRsn_NegativeRsp:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_puNegativeResponse;
              break;

          case PuActFailRsn_AlreadyActive:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_puAlreadyActive;
              break;

          default:
              entryP->dspuPuStatsLastActivationFailureReason 
                = D_dspuPuStatsLastActivationFailureReason_otherError;
              break;

      } /* End switch( GetPuLastActFailRsn(puP) ) */

      entryFound = TRUE;

  }

  return(entryFound);

} /* End GetDspuPuStatsEntry () */


/*=====================================================================*
* GetDspuLuAdminEntry () 
*======================================================================*
*
* Description:
*   Returns dspuLuAdminEntry information 
*
**/
boolean GetDspuLuAdminEntry (dspuLuAdminEntry_t* entryP, int searchType)
{

  Pu*        puP = NULL;
  Pu*        peerPuP = NULL;
  Lu*        luP = NULL;
  PoolRange* prP = NULL;

  long puIndex    = entryP->dspuPuAdminIndex;
  byte luAddr  = entryP->dspuLuAdminLuLocalAddress;
  byte peerLuAddr = 0;

  boolean entryFound = FALSE;

  
  if (searchType == EXACT) { 

      if (entryP->dspuLuAdminLuLocalAddress < 256) {
          puP = FindEntry_DspuPuTable(puIndex);
          if (!IsPuDefault(puP)) {
              luP = FindEntry_DspuLu(puP, luAddr, &prP);
              if (luP || prP)
                  entryFound = TRUE;
          }
      } /* End if - (luAddr < 256) */

  } else {

      if (entryP->dspuLuAdminLuLocalAddress > 255) {
          puIndex++;
          luAddr = 0;
      } 

      do {

          do {
              puP = FindNextEntry_DspuPuTable(puIndex);
              puIndex = GetPuDspuMibPuIdx(puP)+1;
          } while ( puP && IsPuDefault(puP) );

          if (puP) {
              luP = FindNextEntry_DspuLu(puP, &luAddr, &prP);
              if (luP || prP)
                  entryFound = TRUE;
              else 
                  luAddr = 0;
          }

      } while (!entryFound && puP);
  }

  if (entryFound) {

    entryP->dspuPuAdminIndex = GetPuDspuMibPuIdx(puP);
    entryP->dspuLuAdminLuLocalAddress = luAddr;

    if (luP) {

      if ( GetLuPool(luP) ) {

          entryP->dspuLuAdminType = D_dspuLuAdminType_pooled;

          entryP->dspuLuAdminPoolClassName->length = MAX_ENTRYNAME_LN;
          entryP->dspuLuAdminPoolClassName->octet_ptr 
              = GetPoolName(GetLuPool(luP));

            entryP->dspuLuAdminPeerPuIndex = 0;
            entryP->dspuLuAdminPeerLuLocalAddress = 0;

      } else {

          entryP->dspuLuAdminType = D_dspuLuAdminType_dedicated;

          entryP->dspuLuAdminPoolClassName->length = 0;
          entryP->dspuLuAdminPoolClassName->octet_ptr = NULL;

          if (GetPuType(puP) == Upstream) {
              peerPuP = GetLuDnPuP(luP);
              peerLuAddr = GetDownLuNumber(luP);
          } else {
              peerPuP = GetLuUpPuP(luP);
              peerLuAddr = GetUpLuNumber(luP);
          }

          if (peerPuP) {
              entryP->dspuLuAdminPeerPuIndex = GetPuDspuMibPuIdx(peerPuP);
              entryP->dspuLuAdminPeerLuLocalAddress = peerLuAddr;
          } else {
              entryP->dspuLuAdminPeerPuIndex = 0;
              entryP->dspuLuAdminPeerLuLocalAddress = 0;
          }

      }
    } else if (prP) {

        entryP->dspuLuAdminType = D_dspuLuAdminType_pooled;

        entryP->dspuLuAdminPoolClassName->length = MAX_ENTRYNAME_LN;
        entryP->dspuLuAdminPoolClassName->octet_ptr 
            = GetPoolRangePoolName(prP);

        entryP->dspuLuAdminPeerPuIndex = 0;
        entryP->dspuLuAdminPeerLuLocalAddress = 0;

    } 

    entryP->dspuLuAdminRowStatus = D_dspuLuAdminRowStatus_active;
    
  } /* End if - LU entry found */

  return(entryFound);

} /* End GetDspuLuAdminEntry () */


/*=====================================================================*
* GetDspuLuOperEntry () 
*======================================================================*
*
* Description:
*   Returns dspuLuOperEntry information 
*
**/
boolean GetDspuLuOperEntry (dspuLuOperEntry_t* entryP, int searchType)
{

  Pu*        puP = NULL;
  Pu*        peerPuP = NULL;
  Lu*        luP = NULL;
  PoolRange* prP = NULL;

  long puIndex = entryP->dspuPuOperIndex;
  byte luAddr  = entryP->dspuLuOperLuLocalAddress;
  byte peerLuAddr = 0;

  boolean entryFound = FALSE;


  if (searchType == EXACT) { 

      if (entryP->dspuLuOperLuLocalAddress < 256) {
          puP = FindEntry_DspuPuTable(puIndex);
          if (puP) {
              luP = FindEntry_DspuLu(puP, luAddr, &prP);
              if (luP || prP)
                  entryFound = TRUE;
          }
      } /* End if - (luAddr < 256) */

  } else {

      if (entryP->dspuLuOperLuLocalAddress > 255) {
          puIndex++;
          luAddr = 0;
      } 

      do {

              puP = FindNextEntry_DspuPuTable(puIndex);
          if (puP) {
              puIndex = GetPuDspuMibPuIdx(puP)+1;
              luP = FindNextEntry_DspuLu(puP, &luAddr, &prP);
              if (luP || prP)
                  entryFound = TRUE;
              else 
                  luAddr = 0;
          }

      } while (!entryFound && puP);
  }

  if (entryFound) {

    entryP->dspuPuOperIndex = GetPuDspuMibPuIdx(puP);
    entryP->dspuLuOperLuLocalAddress = luAddr;

    if (luP) {

        /* Pooled or Dedicated */
        /* ------------------- */
        if ( GetLuPool(luP) ) {
            entryP->dspuLuOperType = D_dspuLuOperType_pooled;
            entryP->dspuLuOperPoolClassName->length = MAX_ENTRYNAME_LN;
            entryP->dspuLuOperPoolClassName->octet_ptr
                = GetPoolName(GetLuPool(luP));
        } else {
            entryP->dspuLuOperType = D_dspuLuOperType_dedicated;
            entryP->dspuLuOperPoolClassName->length = 0;
            entryP->dspuLuOperPoolClassName->octet_ptr = NULL;
        }

        /* Upstream or Downstream */
        /* ---------------------- */
        if (GetPuType(puP) == Upstream) {
            peerPuP = GetLuDnPuP(luP);
            peerLuAddr = GetDownLuNumber(luP);
        } else {
            peerPuP = GetLuUpPuP(luP);
            peerLuAddr = GetUpLuNumber(luP);
        }

        if (peerPuP) {
            entryP->dspuLuOperPeerPuIndex = GetPuDspuMibPuIdx(peerPuP);
            entryP->dspuLuOperPeerLuLocalAddress = peerLuAddr;
        } else {
            entryP->dspuLuOperPeerPuIndex = 0;
            entryP->dspuLuOperPeerLuLocalAddress = 0;
        }

        if ( QueryLuActiveUpOrDown(luP, GetPuType(puP)) )
          entryP->dspuLuOperState = D_dspuLuOperState_active;
        else 
          entryP->dspuLuOperState = D_dspuLuOperState_inactive;

        entryP->dspuLuOperFsmState = GetDspuLuOperFsmState(luP);

        if ( QueryLuBound(luP) )
          entryP->dspuLuOperSessionState = D_dspuLuOperSessionState_bound;
        else 
          entryP->dspuLuOperSessionState = D_dspuLuOperSessionState_unbound; 

        switch (GetLuLastActFailRsn(luP)) {
            case LuActFailRsn_None:
                 entryP->dspuLuOperLastActivationFailureReason 
                     = D_dspuLuOperLastActivationFailureReason_noError;
                 break;
            case LuActFailRsn_NegativeRsp:
                 entryP->dspuLuOperLastActivationFailureReason 
                     = D_dspuLuOperLastActivationFailureReason_luNegativeResponse;
                 break;
            case LuActFailRsn_Other:
            default:
                 entryP->dspuLuOperLastActivationFailureReason 
                     = D_dspuLuOperLastActivationFailureReason_otherError;
                 break;
        }

    } else if (prP) {
  
        entryP->dspuLuOperType = D_dspuLuOperType_pooled;

        entryP->dspuLuOperPoolClassName->length = MAX_ENTRYNAME_LN;
        entryP->dspuLuOperPoolClassName->octet_ptr
            = GetPoolRangePoolName(prP);

        entryP->dspuLuOperPeerPuIndex = 0;
        entryP->dspuLuOperPeerLuLocalAddress = 0;

        entryP->dspuLuOperState = D_dspuLuOperState_inactive;
        entryP->dspuLuOperFsmState = D_dspuLuOperFsmState_reset;
        entryP->dspuLuOperSessionState = D_dspuLuOperSessionState_unbound; 

        entryP->dspuLuOperLastActivationFailureReason 
            = D_dspuLuOperLastActivationFailureReason_noError;
    }

  } /* End if - LU entry found */

  return(entryFound);

} /* End GetDspuLuOperEntry () */


/*=====================================================================*
* GetDspuSapEntry ()
*======================================================================*
*
* Description:
*   Returns dspuSapEntry information
*
**/
boolean GetDspuSapEntry (dspuSapEntry_t* entryP, int searchType)
{

  Sap *sapP;
  AdaptorId adaptorId;

  boolean entryFound = FALSE;


  adaptorId.dlcType = GetDspuClsDlcType(entryP->dspuSapDlcType);
  adaptorId.u.adaptorPort[UnitIndex] = entryP->dspuSapDlcUnit;
  adaptorId.u.adaptorPort[PortIndex] = entryP->dspuSapDlcPort;
  adaptorId.adaptorSubIfNumber       = 0; /* No entry in DSPU MIB --ppearce*/

  if (searchType == EXACT) {
      sapP = FindEntry_DspuSapTable(entryP->dspuSapAddress, 
                                        &adaptorId);
  } else {
      sapP = FindNextEntry_DspuSapTable(entryP->dspuSapAddress, 
                                            &adaptorId);
  }


  if (sapP) {

      entryP->dspuSapAddress = SapGetSapNum(sapP);

      if (SapQueryDirection(sapP) == Upstream) 
          entryP->dspuSapType = D_dspuSapType_upstreamSap;
      else 
          entryP->dspuSapType = D_dspuSapType_dnstreamSap;

      entryP->dspuSapDlcType 
          = GetDspuMibDlcType( GetAdaptorDlcType(SapGetAdaptorId(sapP)) );

      entryP->dspuSapDlcUnit = GetAdaptorDlcUnit(SapGetAdaptorId(sapP));
      entryP->dspuSapDlcPort = GetAdaptorDlcPort(SapGetAdaptorId(sapP));

      entryP->dspuSapRowStatus = D_dspuSapRowStatus_active;

      entryFound = TRUE;
  }

  return(entryFound);

} /* End GetDspuSapEntry() */



/*
 * Internal procedures 
 */


/*=====================================================================*
* GetDspuPuOperFsmState()
*======================================================================*
*
* Description:
*   Returns correct dspuPuOperFsmState
*
**/
static long GetDspuPuOperFsmState(Pu* puP)
{

  long fsmState;


  long T2PuOperFsmStateTable[T2Max] =
         { D_dspuPuOperFsmState_linkReset,              /* T2Reset         */
           D_dspuPuOperFsmState_puActive,               /* T2Active        */
           D_dspuPuOperFsmState_puBusy                  /* T2Busy          */
         };

  long T5PuOperFsmStateTable[T5Max] =
         { D_dspuPuOperFsmState_linkReset,              /* T5Reset         */
           D_dspuPuOperFsmState_puPendAct,              /* T5PendingActive */
           D_dspuPuOperFsmState_puBusy,                 /* T5Busy          */
           D_dspuPuOperFsmState_puActive,               /* T5Max           */
           D_dspuPuOperFsmState_puPendInact             /* T5PendingInact  */
         };

  long LsPuOperFsmStateTable[LsS_MaxState] =
         { D_dspuPuOperFsmState_linkReset,             /* LsS_Reset       */
           D_dspuPuOperFsmState_linkPendConnOut,       /* LsS_PendOut     */
           D_dspuPuOperFsmState_linkPendConnIn,        /* LsS_PendIn      */
           D_dspuPuOperFsmState_linkPendConnIn,        /* LsS_PendInPu    */
           D_dspuPuOperFsmState_linkPendXid,           /* LsS_PendXid     */
           D_dspuPuOperFsmState_linkXidNeg,            /* LsS_Xid         */
           D_dspuPuOperFsmState_linkConnOut,           /* LsS_ConnOut     */
           D_dspuPuOperFsmState_linkConnIn,            /* LsS_ConnIn      */
           D_dspuPuOperFsmState_linkConnected,         /* LsS_Conn        */
           D_dspuPuOperFsmState_linkPendDisc,          /* LsS_PendDisc    */
           D_dspuPuOperFsmState_linkPendClose          /* LsS_PendClose   */
         };


 
  if (GetPuType(puP) == Upstream) 
    fsmState = T2PuOperFsmStateTable[GetPuSscpState(puP)];
  else
    fsmState = T5PuOperFsmStateTable[GetPuSscpState(puP)];

  if (!fsmState) 
    fsmState = LsPuOperFsmStateTable[GetPuLsState(puP)];

  return(fsmState);
  
} /* End GetDspuPuOperFsmState() */


/*=====================================================================*
* GetDspuLuOperFsmState()
*======================================================================*
*
* Description:
*   Returns correct dspuLuOperFsmState
*
**/
static long GetDspuLuOperFsmState(Lu* luP)
{

  long LuOperFsmStateTable[] =
         { D_dspuLuOperFsmState_reset,                 /* Reset           */
           D_dspuLuOperFsmState_dnLuStarted,           /* dLuStrt         */
           D_dspuLuOperFsmState_upLuActive,            /* uLuAct          */
           D_dspuLuOperFsmState_dnLuPendAct,           /* dLuPendAct      */
           D_dspuLuOperFsmState_dnLuActUnav,           /* dLuActUnav      */
           D_dspuLuOperFsmState_upLuPendAvail,         /* uLuPendAvail    */
           D_dspuLuOperFsmState_bothAvail,             /* BothAvail       */
           D_dspuLuOperFsmState_dnLuPendInact,         /* dLuPendInact    */
           D_dspuLuOperFsmState_upLuPendInact,         /* uLuPendInact    */
           D_dspuLuOperFsmState_luInactivityTimeout,   /* InactTimeout    */
           D_dspuLuOperFsmState_dnInactivityPendInact  /* dluPendTimeout  */
         };

  return(LuOperFsmStateTable[GetLuSscpState(luP)]);
  
} /* End GetDspuLuOperFsmState() */


/*=====================================================================*
* GetDspuMibDlcType()
*======================================================================*
*
* Description:
*   Given a DSPU CLS dlcType - returns DSPU MIB dlcType
*
**/
static byte GetDspuMibDlcType(DspuDLCType_e dlcType)
{

  switch(dlcType) {

    case DSPU_DLC_UNDEFINED:
        return(D_dspumibDlcType_undefined);

    case DSPU_DLC_SDLC:
        return(D_dspumibDlcType_sdlc);
       
    case DSPU_DLC_QLLC:
        return(D_dspumibDlcType_qllc);
       
    case DSPU_DLC_ETHERNET:
        return(D_dspumibDlcType_ethernet);
       
    case DSPU_DLC_TOKEN_RING:
    case DSPU_DLC_CHANNEL:
    case DSPU_DLC_VDLC:
    case DSPU_DLC_NCIA:
        return(D_dspumibDlcType_tokenRing);

    case DSPU_DLC_RSRB:
        return(D_dspumibDlcType_rsrb);

    case DSPU_DLC_FRAME_RELAY:
        return(D_dspumibDlcType_framerelay);

    case DSPU_DLC_FDDI:
        return(D_dspumibDlcType_fddi);

    default:
        return(D_dspumibDlcType_undefined);

  } /* End switch(dlcType) */

} /* End GetDspuMibDlcType() */


/*=====================================================================*
* GetDspuClsDlcType()
*======================================================================*
*
* Description:
*   Given a DSPU MIB dlcType - returns DSPU CLS dlcType
*
**/
static DspuDLCType_e GetDspuClsDlcType(byte dlcType)
{

  switch(dlcType) {

    case D_dspumibDlcType_undefined: 
        return(DSPU_DLC_UNDEFINED);

    case D_dspumibDlcType_sdlc:
        return(DSPU_DLC_SDLC);

    case D_dspumibDlcType_qllc:
        return(DSPU_DLC_QLLC);

    case D_dspumibDlcType_ethernet:
        return(DSPU_DLC_ETHERNET);

    case D_dspumibDlcType_tokenRing:
        return(DSPU_DLC_TOKEN_RING);

    case D_dspumibDlcType_rsrb:
        return(DSPU_DLC_RSRB);

    case D_dspumibDlcType_framerelay:
        return(DSPU_DLC_FRAME_RELAY);

    case D_dspumibDlcType_fddi:
        return(DSPU_DLC_FDDI);

    default:
        return(DSPU_DLC_UNDEFINED);

  } /* End switch(dlcType) */

} /* End GetDspuClsDlcType() */
