/* $Id: dspuconf.c,v 3.6.6.6 1996/09/10 06:08:42 lcox Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspuconf.c,v $
 *------------------------------------------------------------------
 * DSPU Parser Interface. 
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * This module provides a call interface to all DSPU Parser Commands.
 *------------------------------------------------------------------
 * $Log: dspuconf.c,v $
 * Revision 3.6.6.6  1996/09/10  06:08:42  lcox
 * CSCdi68591:  dspu pu matching algorithm behaviour is not clearly defined
 * Branch: California_branch
 *
 * Revision 3.6.6.5  1996/08/09  01:54:04  lcox
 * CSCdi61966:  dspu - maxiframe range incorrect for default pu
 * Branch: California_branch
 *
 * Revision 3.6.6.4  1996/07/29  07:16:40  lcox
 * CSCdi63645:  dspu inactivity timeout doesnt get cleaned up properly.
 * Branch: California_branch
 *
 * Revision 3.6.6.3  1996/07/24  01:23:40  szdravko
 * CSCdi61963:  No action and no error message on 2nd dspu default-pu
 * Branch: California_branch
 *
 * Revision 3.6.6.2  1996/05/17  10:48:11  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.5.6.6  1996/05/03  17:18:28  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.5.6.5  1996/04/26  07:35:13  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.5.6.4  1996/04/03  14:03:00  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.5.6.3  1996/03/28  08:20:37  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.5.6.2  1996/03/22  18:50:33  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 3.5.6.1  1996/03/17  17:38:41  ppearce
 * Sync IbuMod_Calif_branch to V111_1_3
 *
 * Revision 3.6.6.1  1996/03/18  19:33:37  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.20.3  1996/03/16  06:40:45  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.20.2  1996/03/07  08:45:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.20.1  1996/02/20  00:47:18  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.6  1996/03/05  03:25:41  bchan
 * CSCdi49448:  DSPU only allows 256 downstream PUs
 *
 * Revision 3.5  1996/02/01  06:02:01  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.4  1995/11/30  19:09:03  ppearce
 * CSCdi44719:  sna rsrb start command does not appear in config
 *   Separate nv_write of "sna rsrb start" and "dspu rsrb start"
 *
 * Revision 3.3  1995/11/20  16:37:35  ppearce
 * CSCdi43278:  dspu start and sna start commands disappear from config
 *
 * Revision 3.2  1995/11/17  09:05:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.10  1995/11/08  20:56:32  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.9  1995/10/19  21:12:59  ppearce
 * CSCdi42171:  SNANM/DSPU cannot be config'd on Frame Relay sub-interfaces
 *   Allow SNANM/DSPU config commands on sub-interfaces
 *
 * Revision 2.8  1995/10/11  04:20:48  ppearce
 * CSCdi38628:  DSPU RSRB statement needs configurable bridge number
 *   Add fBridgeNumber capability for CLSI/RSRB Enable.Req
 *
 * Revision 2.7  1995/09/20  17:03:17  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.6  1995/09/19 15:55:45  rbatz
 * CSCdi40476:  The command "no dspu rsrb enable-host" can cause crash
 *
 * Revision 2.5  1995/09/14  06:15:17  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.4  1995/08/28  02:16:14  ppearce
 * CSCdi38694:  DSPU sends ACTLU -rsp for dedicated LUs w/ dspu rsrb start
 *   Move nv_write of "dspu rsrb start" commands to end of DSPU config
 *
 * Revision 2.3  1995/08/27  23:35:32  ppearce
 * CSCdi38144:  Router reboot after enter no dspu rsrb xx command
 *   Prevent user from deleting SAP or RSRB adaptor while PU is active
 *
 * Revision 2.2  1995/07/06  23:15:46  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.1  1995/06/07  20:28:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_generic.h"
#include "dspu.h"
#include "dspu_pu.h"
#include "dspu_error.h"
#include "dspu_puutil.h"
#include "dspu_sap.h"
#include "dspu_adaptor.h"
#include "dspu_pool.h"
#include "dspu_lu.h"
#include "dspu_ls.h"
#include "dspuconf.h"
#include "dspu_itimer.h"
#include "dspu_util.h"
#include "dspu_fp.h"
#include "dspu_notification.h"
#include "parser_defs_dspu.h"
#include "../srt/rsrb.h"

static boolean   actWindowDefined;

static Pu*       lastPuConfigured;
static Pu* FindAllPu(const char* name);        /* Pu name to scan  */

static boolean IsPuInterfaceDefnOkay(hword dlci, byte sdlc, 
                                     const char *x25, const char *rMac,
                                     idbtype *idb);


/*=====================================================================*
* DspuKeyEnable
*======================================================================*
*
* Description:
*    Config "dspu enable-[pu host] lsap localSap"
*    Create adaptor and sap if  not existed. 
*
**/

void
DspuKeyEnable (DspuDLCType_e dlcType,     /* DLC type                    */
               idbtype *idb,              /* Pointer to DLC idb          */
               byte localSap,             /* Local SAP number            */
               int direction)             /* Upstream or Downstream      */
{

   Adaptor* adaptorP;
   AdaptorId id;
   Sap* sapP;

   lastPuConfigured = 0;    /* invalidate last PU statement */

   /* make sure we have a valid qllc subaddress. */
   if (dlcType == DSPU_DLC_QLLC) {
       if (localSap == BadQllcSubaddr) {
           DspuError0(InvalidQllcSubAddr);
	   return;
       }
   }

   if (dlcType == DSPU_DLC_UNDEFINED) { 
       DspuError0(DSPU_AdaptorNotSupported);
       return;
   } else if (dlcType == DSPU_DLC_RSRB) { 
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetRsrbAdaptorId());
       id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetRsrbAdaptorId());
       id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetRsrbAdaptorId());
       id.dlcIdb  = 0;
   } else if (dlcType == DSPU_DLC_VDLC) { 
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetVdlcAdaptorId());
       id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetVdlcAdaptorId());
       id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetVdlcAdaptorId());
       id.dlcIdb  = 0;
   } else if (dlcType == DSPU_DLC_NCIA) { 
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetNciaAdaptorId());
       id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetNciaAdaptorId());
       id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetNciaAdaptorId());
       id.dlcIdb  = 0;
   } else {
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = idb->hwptr->slot;
       id.u.adaptorPort[PortIndex] = idb->hwptr->unit;
       id.adaptorSubIfNumber       = idb->sub_number;
       id.dlcIdb  = idb;
   }


   if ((adaptorP = AdaptorGetOrCreate(&id)) != 0){
       if ((sapP = QuerySapExisted(adaptorP,localSap)) == 0){
            sapP = SapCreate (localSap, direction, GetSapChildren(adaptorP), adaptorP);
            if (!sapP)
                DspuError0 (ConfigNoMemory);
       }
       else
           DspuError1(EnableDefinedTwice, localSap);
   }
}

/*=====================================================================*
* DspuKeyStart: dspu start puName
*======================================================================*
*
* Description:
*    Find pu and request a connect out
*   
*
**/

void DspuKeyStart(const char* name,          /* PU/Host name            */
                DspuDLCType_e dlcType,       /* DLC type                */
                idbtype *idb)                /* Pointer to DLC idb      */
{
    Pu* puP;
    AdaptorId id;
    Adaptor* adaptorP;

    lastPuConfigured = 0;    /* invalidate last PU statement */
    if (dlcType == DSPU_DLC_UNDEFINED) {
       DspuError0(DSPU_AdaptorNotSupported);
       return;
    } else if (dlcType == DSPU_DLC_RSRB) {
        id.dlcType = dlcType;
        id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetRsrbAdaptorId());
        id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetRsrbAdaptorId());
        id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetRsrbAdaptorId());
        id.dlcIdb  = 0;
    } else if (dlcType == DSPU_DLC_VDLC) {
        id.dlcType = dlcType;
        id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetVdlcAdaptorId());
        id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetVdlcAdaptorId());
        id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetVdlcAdaptorId());
        id.dlcIdb  = 0;
    } else if (dlcType == DSPU_DLC_NCIA) {
        id.dlcType = dlcType;
        id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetNciaAdaptorId());
        id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetNciaAdaptorId());
        id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetNciaAdaptorId());
        id.dlcIdb  = 0;
    } else {
        id.dlcType = dlcType;
        id.u.adaptorPort[UnitIndex] = idb->hwptr->slot;
        id.u.adaptorPort[PortIndex] = idb->hwptr->unit;
        id.adaptorSubIfNumber       = idb->sub_number;
         id.dlcIdb  = idb;
    }


    puP = FindAllPu(name);
    adaptorP = QueryAllChildrenParm(&dspu.adaptorRoot, 
                                    (ProcQChild1)AdaptorFind, &id);

    if (!puP) 
       DspuErrors(DSPU_PU_NameNotFound, name);
    else if (!adaptorP)
       DspuError0(StartNoSapsEnabled);
    else if ( GetPuConfiguredIdb(puP) && (GetPuConfiguredIdb(puP) != idb) ) 
       DspuErrors(StartBadInterface, GetPuName(puP));
    else
       PuConnectOut(puP, &id);
}

void DspuNKeyStart(const char* name)               /* 0 if no ports on Adaptor */
{
    Pu* puP;

    if ((puP = FindAllPu(name)) == 0)
        DspuErrors(StartBadName, name);
    else
        PuNotConnectOut(puP);
}

/*=====================================================================*
* Dspu rsrb enable lsap x,  dspu rsrb start 
*======================================================================*
*
* Description:
*    The following routines are the same as normal enable and start
* except they are rsrb specific
*
**/

/* Find RSRB adaptor */

static Adaptor* RsrbAdaptor(void)
{
    return QueryAllChildrenParm(GetAdaptorRoot(), (ProcQChild1)AdaptorFind, 
                                  GetRsrbAdaptorId());
}

/* dspu rsrb enable-[host|pu] lsap lsap */
void DspuRsrbKeyEnable(byte lsap, int direction)
{


   /* 
    *
    * Don't allow SAP to be enabled 
    *   if "sna rsrb" configured (GetRsrbSnaMode() == TRUE)
    *     but command is "dspu rsrb enable" (GetSnaMode() == FALSE)
    *
    *           ...or...
    *
    *   if "dspu rsrb" configured (GetRsrbSnaMode() == FALSE)
    *     but command is "sna rsrb enable" (GetSnaMode() == TRUE)
    *
    */
   if ( GetRsrbSnaMode() && !GetSnaMode() ) {
       DspuError2(RsrbEnableBadRsrbDef, DSPU_header, SNA_header);
       return;
   } else if ( !GetRsrbSnaMode() && GetSnaMode() ) {
       DspuError2(RsrbEnableBadRsrbDef, SNA_header, DSPU_header);
       return;
   }

   /*
    * If RsrbAdaptorNumber has not been defined,
    *   Define RSRB adaptor 
    *
    */
    if (GetAdaptorDlcType(GetRsrbAdaptorId()) == DSPU_DLC_UNDEFINED) {
        dspu.rsrb.adaptorId.dlcType = DSPU_DLC_RSRB;
        dspu.rsrb.adaptorId.u.adaptorPort[UnitIndex] = 0;
        dspu.rsrb.adaptorId.u.adaptorPort[PortIndex] = 0;
        dspu.rsrb.adaptorId.adaptorSubIfNumber       = 0;
        dspu.rsrb.adaptorId.dlcIdb  = 0;
    } 
    DspuKeyEnable(DSPU_DLC_RSRB, 0, lsap, direction);
}

void DspuRsrbNKeyEnable(byte lsap)
{
    /* Make sure RSRB defined before passing NULL into DspuNKeyEnable */
    if (GetRsrbLocalRing() == 0) {
       DspuError0(DSPU_RSRBnotDefined);
       return;
    }

    DspuNKeyEnable( GetAdaptorDlcType(GetRsrbAdaptorId()), 0, lsap);
}


void DspuKeyRsrbStart(const char* name)
{

    Pu* puP = FindAllPu(name);                     /* PU name defined? */
    if (puP == 0)
        DspuErrors(DSPU_PU_NameNotFound, name);
    else if (GetRsrbLocalRing() == 0)              /* RSRB defined?     */
        DspuError0(RsrbStartFailed);
    else if (RsrbAdaptor() == NULL)                /* RSRB Sap enabled? */
        DspuError0(RsrbNotEnabled);
    else if (vrfind_ringgroup(GetRsrbTargetRing()) == 0) /* targetRing? */
        DspuError1(RsrbNoTargetRing, GetRsrbTargetRing());
    else {
        PuConnectOut(puP, GetRsrbAdaptorId()); 
    }

} /* End DspuKeyRsrbStart() */

/*=====================================================================*
* dspu vdlc
*======================================================================*
*
**/

/*
 * Find VDLC adaptor
 */
static Adaptor* VdlcAdaptor(void)
{
    return QueryAllChildrenParm(GetAdaptorRoot(), (ProcQChild1)AdaptorFind, 
                                  GetVdlcAdaptorId());
}

/* 
 * dspu vdlc enable-[host|pu] lsap <lsap>
 */
void DspuVdlcKeyEnable(byte lsap, int direction)
{


   /* 
    *
    * Don't allow SAP to be enabled 
    *   if "sna vdlc" configured (GetVdlcSnaMode() == TRUE)
    *     but command is "dspu vdlc enable" (GetSnaMode() == FALSE)
    *
    *           ...or...
    *
    *   if "dspu vdlc" configured (GetVdlcSnaMode() == FALSE)
    *     but command is "sna vdlc enable" (GetSnaMode() == TRUE)
    *
    */
   if ( GetVdlcSnaMode() && !GetSnaMode() ) {
       DspuError2(VdlcEnableBadVdlcDef, DSPU_header, SNA_header); 
       return;
   } else if ( !GetVdlcSnaMode() && GetSnaMode() ) {
       DspuError2(VdlcEnableBadVdlcDef, SNA_header, DSPU_header);
       return;
   }

   /*
    * If VdlcAdaptorNumber has not been defined,
    *   Define VDLC adaptor 
    *
    */
    if (GetAdaptorDlcType(GetVdlcAdaptorId()) == DSPU_DLC_UNDEFINED) {
        dspu.vdlc.adaptorId.dlcType = DSPU_DLC_VDLC;
        dspu.vdlc.adaptorId.u.adaptorPort[UnitIndex] = 0;
        dspu.vdlc.adaptorId.u.adaptorPort[PortIndex] = 0;
        dspu.vdlc.adaptorId.adaptorSubIfNumber       = 0;
        dspu.vdlc.adaptorId.dlcIdb  = 0;
    } 
    DspuKeyEnable(DSPU_DLC_VDLC, 0, lsap, direction);
}

void DspuVdlcNKeyEnable(byte lsap)
{
    /* Make sure VDLC defined before passing NULL into DspuNKeyEnable */
    if (GetVdlcRingNum() == 0) {
       DspuError0(DSPU_VDLCnotDefined);
       return;
    }

    DspuNKeyEnable( GetAdaptorDlcType(GetVdlcAdaptorId()), 0, lsap);
}


void DspuKeyVdlcStart(const char* name)
{

    Pu* puP = FindAllPu(name);                     /* PU name defined? */
    if (puP == 0)
        DspuErrors(DSPU_PU_NameNotFound, name);
    else if (GetVdlcRingNum() == 0)                /* VDLC defined?     */
        DspuError0(VdlcStartFailed);
    else if (VdlcAdaptor() == NULL)                /* VDLC Sap enabled? */
        DspuError0(VdlcNotEnabled);
    else {
        PuConnectOut(puP, GetVdlcAdaptorId()); 
    }

} /* End DspuKeyVdlcStart() */



/*=====================================================================*
* dspu ncia
*======================================================================*
*
**/

/*
 * Find NCIA adaptor
 */
static Adaptor* NciaAdaptor(void)
{
    return QueryAllChildrenParm(GetAdaptorRoot(), (ProcQChild1)AdaptorFind, 
                                  GetNciaAdaptorId());
}

/* 
 * dspu ncia enable-pu lsap <lsap>
 */
void DspuNciaKeyEnable(byte lsap, int direction)
{

   /*
    * If NciaAdaptorNumber has not been defined,
    *   Define NCIA adaptor 
    *
    */
    if (GetAdaptorDlcType(GetNciaAdaptorId()) == DSPU_DLC_UNDEFINED) {
        dspu.ncia.adaptorId.dlcType = DSPU_DLC_NCIA;
        dspu.ncia.adaptorId.u.adaptorPort[UnitIndex] = 0;
        dspu.ncia.adaptorId.u.adaptorPort[PortIndex] = 0;
        dspu.ncia.adaptorId.adaptorSubIfNumber       = 0;
        dspu.ncia.adaptorId.dlcIdb  = 0;
    } 

    DspuKeyEnable(DSPU_DLC_NCIA, 0, lsap, direction);
}

void DspuNciaNKeyEnable(byte lsap)
{
    /* Make sure NCIA defined before passing NULL into DspuNKeyEnable */

    DspuNKeyEnable( GetAdaptorDlcType(GetNciaAdaptorId()), 0, lsap);
}


/* The following routines are not used. They are replaced by
 * DspuKeyStart
 */
void DspuActivate(const char* name)
{
    Pu* puP = FindAllPu(name);
    if (puP == 0)
        DspuErrors(ActivateBadName, name);
    else {
        if (snpa_get_type(GetPuAddrP(puP)) == STATION_FR10)
            PuActivateLLC(puP);
        else if (snpa_get_type(GetPuAddrP(puP)) == STATION_SDLC)
            PuActivateSDLC(puP);
        else if (snpa_get_type(GetPuAddrP(puP)) == STATION_X121)
            PuActivateSDLC(puP);
        else if (snpa_get_type(GetPuAddrP(puP)) == STATION_IEEE48)
            PuActivateLLC(puP);
        else {
            DspuError0(PuNoRemoteAddr);
            DspuNetMgmtNotification((void *) puP, PU_ACTFAIL, 
                       PuActFailRsn_ConfigError, GetPuType(puP));
        }
    }
}


void DspuDeactivate(const char* name)
{
    Pu* puP = FindAllPu(name);
    if (puP == 0)
        DspuErrors(ActivateBadName, name);
    else 
        PuDeactivate(puP);
}

/*=====================================================================*
* NO DSPU ENABLE dlcType adaptor port 
*======================================================================*
*
* Description:
* Disable DSPU on this interface.
*
**/

void
DspuNKeyEnable (DspuDLCType_e dlcType, idbtype *idb, byte lsap)
{
   Adaptor* adaptorP;
   AdaptorId id;
   Sap* sapP;
   

   lastPuConfigured = 0;    /* invalidate last PU statement */

   if (dlcType == DSPU_DLC_RSRB) { 
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetRsrbAdaptorId());
       id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetRsrbAdaptorId());
       id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetRsrbAdaptorId());
       id.dlcIdb  = 0;
   } else if (dlcType == DSPU_DLC_VDLC) { 
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetVdlcAdaptorId());
       id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetVdlcAdaptorId());
       id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetVdlcAdaptorId());
       id.dlcIdb  = 0;
   } else if (dlcType == DSPU_DLC_NCIA) { 
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = GetAdaptorDlcUnit(GetNciaAdaptorId());
       id.u.adaptorPort[PortIndex] = GetAdaptorDlcPort(GetNciaAdaptorId());
       id.adaptorSubIfNumber       = GetAdaptorDlcSubIf(GetNciaAdaptorId());
       id.dlcIdb  = 0;
   } else if ((idb == NULL) || (idb->hwptr == NULL)) {
       DspuError0(DSPU_noAdaptor);
       return;
   } else {
       id.dlcType = dlcType;
       id.u.adaptorPort[UnitIndex] = idb->hwptr->slot;
       id.u.adaptorPort[PortIndex] = idb->hwptr->unit;
       id.adaptorSubIfNumber       = idb->sub_number;
       id.dlcIdb  = idb;
   }
   
       
  /*
   * Find the DLC adaptor
   */
   if ((adaptorP = QueryAllChildrenParm(&dspu.adaptorRoot, (ProcQChild1)AdaptorFind, &id)) == 0) {
      DspuError0(DSPU_noAdaptor);
   } else if ((sapP = GetChild(GetSapChildren(adaptorP), lsap)) != 0) {
       if (QueryLsConnOutPuOnSap(sapP))
         DspuError1(SapActivePuLs, lsap);
       else
         SapRequestCloseSap(sapP);
   } else {
      DspuError1(SapNotDefined, lsap);
   }

}  /* DspuNKeyEnable. */


/*=====================================================================*
* DSPU RSRB <localRing> <bridgeNum> <targetRing> 
*               <virtualMacAddress> [LOCAL-ACK]
*======================================================================*
*
* Description:
* Make RSRB known within DSPU.   
**/

void
DspuKeyRsrb (hword localRingNum,            /* Local ring number     */
             byte  bridgeNum,               /* Bridge number         */
             hword targetRingNum,           /* Target ring number    */
             const char* macAddr)           /* Virtual Mac address   */

{

    MacAddrT zeroMacAddr;            /* Zero MAC address is invalid  */
    lastPuConfigured = 0;            /* Invalidate last PU statement */

    memset (&zeroMacAddr, 0, sizeof(MacAddrT));

    if (GetRsrbLocalRing() != 0)               /* RSRB already configured? */
        DspuError2(DSPU_RSRBalreadyDefined,
                    (GetRsrbSnaMode() ? SNA_header : DSPU_header), 0);

    else if (vrfind_ringgroup(localRingNum) != 0)  /* localRingNum avail?  */
        DspuError1(DSPU_RSRBinvalidLocalRing, localRingNum);

    else if (vrfind_ringgroup(targetRingNum) == 0) /* targetRing active?   */
        DspuError1(DSPU_RSRBinvalidTargetRing, targetRingNum);

    else if (bcmp(macAddr, (char *) &zeroMacAddr, IEEEBYTES) == 0)
        DspuError0(DSPU_RSRBinvalidMacAddr);

    else {

        memcpy(GetRsrbVirtualMacAddr(), macAddr, sizeof(MacAddrT));
        dspu.rsrb.localRing = localRingNum;
        dspu.rsrb.bridgeNum = bridgeNum;
        dspu.rsrb.targetRing = targetRingNum;
        dspu.rsrb.adaptorId.dlcType = DSPU_DLC_UNDEFINED;

        SetRsrbSnaMode(GetSnaMode());
    }

}  /* DspuKeyRsrb. */


/*=====================================================================*
* NO DSPU RSRB 
*======================================================================*
*
* Description:
* Make unknown RSRB within DSPU.
*   
**/
void
DspuNKeyRsrb (hword localRingNum,            /* Local ring number     */
              byte  bridgeNum,               /* Bridge number         */
              hword targetRingNum,           /* Target ring number    */
              const char* macAddr)           /* Virtual Mac address   */

{
    Adaptor* adaptorP;


    lastPuConfigured = 0;    /* invalidate last PU statement */
   
   if (GetRsrbLocalRing() == 0)                   /* RSRB defined?           */
       DspuError0(DSPU_RSRBnotDefined);

   else if (localRingNum != GetRsrbLocalRing())   /* Check <localRing> parm  */
       DspuError1(DSPU_RSRBinvalidLocalRing, localRingNum);

   else if (QueryLsConnOutPuOnAdaptor(GetRsrbAdaptorId()))
       DspuError0(DSPU_RSRBActivePuLs);

   else {

       memset (&dspu.rsrb.virtualMac, 0, sizeof(MacAddrT));
       dspu.rsrb.localRing = 0;
       dspu.rsrb.bridgeNum = 0;
       dspu.rsrb.targetRing = 0;

       adaptorP = RsrbAdaptor();
       if (adaptorP) {
           DoToAllChildren(GetSapChildren(adaptorP), 
                               (ProcChild)SapRequestCloseSap);
       }

       dspu.rsrb.adaptorId.dlcType = DSPU_DLC_UNDEFINED;
       dspu.rsrb.adaptorId.u.adaptornumber = 0;
       dspu.rsrb.adaptorId.adaptorSubIfNumber = 0;
       dspu.rsrb.adaptorId.dlcIdb = 0;
       SetRsrbSnaMode(FALSE);
   }

}  /* DspuNKeyRsrb. */

/*=====================================================================*
* DSPU VDLC <ringNum> <virtualMacAddress> 
*======================================================================*
*
**/

void
DspuKeyVdlc (hword ringNum,                 /* Ring number           */
             const char* macAddr)           /* Virtual Mac address   */

{

    MacAddrT zeroMacAddr;            /* Zero MAC address is invalid  */
    lastPuConfigured = 0;            /* Invalidate last PU statement */

    memset (&zeroMacAddr, 0, sizeof(MacAddrT));

    if (GetVdlcRingNum() != 0)                  /* VDLC already configured? */
        DspuError2(DSPU_VDLCalreadyDefined,
                    (GetRsrbSnaMode() ? SNA_header : DSPU_header), 0);

    else if (vrfind_ringgroup(ringNum) == 0)    /* SRB ringNum exists?      */
        DspuError1(DSPU_VDLCinvalidRingNum, ringNum);

    else if (bcmp(macAddr, (char *) &zeroMacAddr, IEEEBYTES) == 0)
        DspuError0(DSPU_VDLCinvalidMacAddr);

    else {

        memcpy(GetVdlcVirtualMacAddr(), macAddr, sizeof(MacAddrT));
        dspu.vdlc.ringNum = ringNum;
        dspu.vdlc.adaptorId.dlcType = DSPU_DLC_UNDEFINED;

        SetVdlcSnaMode(GetSnaMode());
    }

}  /* DspuKeyVdlc() */


/*=====================================================================*
* NO DSPU VDLC 
*======================================================================*
*
**/
void
DspuNKeyVdlc (hword ringNum,                 /* Ring number     */
              const char* macAddr)           /* Virtual Mac address   */

{
    Adaptor* adaptorP;


    lastPuConfigured = 0;    /* invalidate last PU statement */
   
   if (GetVdlcRingNum() == 0)                     /* VDLC defined?           */
       DspuError0(DSPU_VDLCnotDefined);

   else if (ringNum != GetVdlcRingNum())          /* Check <ringNum> parm    */
       DspuError1(DSPU_VDLCinvalidRingNum, ringNum);

   else if (QueryLsConnOutPuOnAdaptor(GetVdlcAdaptorId()))
       DspuError0(DSPU_VDLCActivePuLs);

   else {

       memset (&dspu.vdlc.virtualMac, 0, sizeof(MacAddrT));
       dspu.vdlc.ringNum = 0;

       adaptorP = VdlcAdaptor();
       if (adaptorP) {
           DoToAllChildren(GetSapChildren(adaptorP), 
                               (ProcChild)SapRequestCloseSap);
       }

       dspu.vdlc.adaptorId.dlcType = DSPU_DLC_UNDEFINED;
       dspu.vdlc.adaptorId.u.adaptornumber = 0;
       dspu.vdlc.adaptorId.adaptorSubIfNumber = 0;
       dspu.vdlc.adaptorId.dlcIdb = 0;
       SetVdlcSnaMode(FALSE);
   }

}  /* DspuNKeyVdlc. */




/*=====================================================================*
* DSPU NCIA Server <number>
*======================================================================*
*
**/

void
DspuKeyNcia (int ServerNum)                 /* Server number         */

{

    lastPuConfigured = 0;            /* Invalidate last PU statement */

    /* there is no way to validate the server number unless we do
       enable port */
    dspu.ncia.serverNum = ServerNum;
    
    dspu.ncia.adaptorId.dlcType = DSPU_DLC_UNDEFINED;

    SetNciaSnaMode(GetSnaMode());

}  /* DspuKeyNcia() */


/*=====================================================================*
* NO DSPU NCIA 
*======================================================================*
*
**/
void
DspuNKeyNcia (int ServerNum)                 /* Server number         */

{
    Adaptor* adaptorP;

    lastPuConfigured = 0;    /* invalidate last PU statement */
   
    dspu.ncia.serverNum = ServerNum;

    adaptorP = NciaAdaptor();
    if (adaptorP) {
        DoToAllChildren(GetSapChildren(adaptorP), 
                            (ProcChild)SapRequestCloseSap);
    }

    dspu.ncia.adaptorId.dlcType = DSPU_DLC_UNDEFINED;
    dspu.ncia.adaptorId.u.adaptornumber = 0;
    dspu.ncia.adaptorId.adaptorSubIfNumber = 0;
    dspu.ncia.adaptorId.dlcIdb = 0;
    dspu.ncia.serverNum = 0;
    SetNciaSnaMode(FALSE);

}  /* DspuNKeyNcia. */



#define FindInactivePu(name,root)\
   QueryAllChildrenParm(root, (ProcQChild1)QueryMatchPuName, (void*)name)


/*=====================================================================*
* FindPu
*======================================================================*
*
* Description:
*    Given the link type, find a pu with a pu name given a link type.
**/
static Pu* FindPu(const char* name,   /* Downstream pu  name to scan  */
                  LinkType puType)
{

   Pu* puP;
   if (puType == Upstream){
      if ((puP = FindActiveUpPu(name)) == 0)
          puP = FindInactiveUpPu(name);
   }
   else {
      if ((puP = FindActiveDownPu(name)) == 0)
          puP = FindInactiveDownPu(name);
   }
   return puP;
}


/*=====================================================================*
* FindAllPu
*======================================================================*
*
* Description:
*    Scan through all pu to find a pu with a pu name
**/
static Pu* FindAllPu(
     const char* name)        /* Pu name to scan  */
{
   Pu* puP; 
   if ((puP = FindActiveUpPu(name))  == 0 &&
       (puP = FindInactiveUpPu(name)) == 0 &&
       (puP = FindActiveDownPu(name)) == 0)
          puP = FindInactiveDownPu(name);
   return puP;
}



/*=====================================================================*
* DSPU ACTWINDOW windowSize
*======================================================================*
*
* Description:
*   Updated the actwindow value.
**/

void
DspuKeyActWindow (hword actWindow)

{

   lastPuConfigured = 0;                 /* invalidate last PU statement */
   if (actWindowDefined)
      DspuError0(ActWindowDefinedTwice);
   else 
   {
      actWindowDefined = TRUE;
      SapSetActWindow(actWindow);        /* change to new value */
   }
}


/*=====================================================================*
* DSPU NO ACTWINDOW
*======================================================================*
*
* Description:
*   Set the actwindow value to the default value of 10.
**/

void
DspuNKeyActWindow (void)

{

   lastPuConfigured = 0;         /* invalidate last PU statement */
   if (!actWindowDefined)
      DspuError0(ActWindowNotDefined);
   else 
   {
      actWindowDefined = FALSE;
      SapSetActWindow(DEFAULT_ACTWINDOW);   /* Reset to default value  */
      
   }
}


/*=====================================================================*
* MakeXidFromId
*======================================================================*
*
* Description:
*   Make xid out of char* id
*
**/
static word MakeXidFromId(const char* id)
{
    Xid xid;
    if (id == 0)
       xid.xid = NoXid;
    else
       HexAsciiToByte (id, &xid.blk_num[0], 4);
    return xid.xid;
}



/*=====================================================================*
* DSPU HOST hostName XID id [RMAC remoteMac RSAP remoteSap]
*           [LSAP lsap] [FOCALPOINT] [WINDOW windowSize] 
*           [MAXIFRAME maxIframe] [RETRIES retryCount] 
*           [RETRY TIMEOUT retryTimeout]
*======================================================================*
*
* Description:
*    Construct an upstream sap, link station and PU. The Sap is a 
* child of RSRB adaptor.
**/

void
DspuKeyHost (const char* hostName,      /* Upstream PU name                   */
             const char* xidP,          /* XID                                */
             hword dlci,                /* DLCI number                        */
             byte  sdlc,                /* SDLC address                       */
             const char* x25,           /* X.25 address                       */
             const char* rMac,          /* Remote MAC address                 */
             byte  rSap,                /* Remote SAP address                 */
             byte  lSap,                /* Local SAP address                  */
             idbtype *idb,              /* Cfg'd interface (idb)              */
             hword windowSize,          /* Window size                        */
             hword maxIframe,           /* Maximimum Iframe size              */
             byte maxRetryCount,        /* Retry count                        */
             hword maxRetryTimeout,     /* Retry timeout                      */
             boolean focalPoint)        /* FocalPoint PU                      */
{

   Pu* hostPuP;


   lastPuConfigured = 0;    /* invalidate last PU statement */

   /* 
    * If this PU is being defined for SNA Network Mgmt,
    *   set focalPoint = TRUE 
    *     without regard to whether focalPoint was explicitly confiugred
    *
    */
   if (GetSnaMode())
     focalPoint = TRUE;

   /*---- Set retry timeout period. -1 => infinite. */
   if (maxRetryTimeout == INFINITE_MAX_RETRY_TIMEOUT)
      maxRetryTimeout = 0;

   /* make sure we have a valid qllc subaddress */
   if (x25 && (lSap == BadQllcSubaddr)) {
           DspuError0(InvalidQllcSubAddr);
           return;
   }

   /* scan both upstream and downstream to ensure pu is not defined */
   
   if (FindAllPu(hostName)) {
       DspuErrors(DSPU_PU_Duplicated, hostName);
       return;
   }

   /*
    * Validate host name and issue error msg if invalid. 
    */
   if (!(DspuIsValidPuName(hostName))) {
       DspuErrors(DSPU_HOST_InvalidName,hostName);
       return;
   }

   if (focalPoint && IsFocalPointExisted()) {
       DspuError0(MultipleFOCALPOINT);
       return;
   }

   
   if (!IsPuInterfaceDefnOkay(dlci, sdlc, x25, rMac, idb)) {
       DspuError0(DSPU_PU_InterfaceConflict);
       return;
   } 

   if (strlen(xidP) > 8) {
       DspuError0(InvalidXID);
       return;
   }

   hostPuP = Pu_nw(Upstream, hostName, dlci, sdlc, x25, rMac, 
                   rSap, lSap, idb, (byte) windowSize, maxIframe, 
                   MakeXidFromId(xidP), maxRetryCount, maxRetryTimeout);
                     
   if (!hostPuP) {        
       DspuError0(ConfigNoMemory); 
       return;
   }
            
   if (focalPoint)
       Fp_new(hostPuP);   /* construct focal point */

} /* End DspuKeyHost() */


/*=====================================================================*
* DSPU NO HOST hostName 
*======================================================================*
*
* Description:
*   Delete the upstream pu with hostname.
**/

void
DspuNKeyHost (const char* name)        /* upstream Pu name  */

{
   Pu* puP;
   
   lastPuConfigured = 0;
   puP = FindPu(name,Upstream);
   if (puP == 0)
      DspuErrors(DSPU_PU_NameNotFound, name);
   else
      Pu_dl(puP);
}      


/*=====================================================================*
* DSPU POOL poolName HOST hostName LU luStart luEnd inactivityTimeout
*======================================================================*
*
* Description:
*    Construct a poolType and put it in the root queue list.  
* Construct all the LUs defined.
**/
void      
DspuKeyPool (char* poolName,
             const char* hostName,
             byte  luStart,
             byte  luEnd,  
             hword inactivityTimeout)
{

   PoolCB* poolP;
   Lu*     luP;
   hword   lu;
   byte    luRange = 1;                   /* Default luRange to 1 */
   Pu* puP = FindPu(hostName,Upstream);   /* Find Upstream PU     */
   
   lastPuConfigured = 0;    /* invalidate last PU statement */

   if ( (luEnd > 0) && (luEnd < luStart) ) {   /* Range check luEnd addr */
       DspuError0(DSPU_LuEndBeforeLuStart);
       return;
   }

   if (luEnd != 0)
       luRange = (luEnd-luStart)+1;            /* Calculate luRange      */

   if (puP == 0) {
       DspuErrors(DSPU_POOL_HostNameInvalid,hostName );
   } else if (QueryLuConflict(puP, luStart, luRange)) {
       DspuError0(DSPU_POOL_ConflictHostLuDef);
   } else {

      if ((poolP = FindPool(poolName)) == 0)
          poolP = PoolCB_nw(poolName, inactivityTimeout);
                          /* create a pool control block */
      else
          PoolChangeInactTimeout(poolP, inactivityTimeout);
              /* update inactivityTimeout to be last cfgd value */
          
      if (poolP){
      
         for (lu = 0; lu < luRange; lu++){
            luP = Lu_nw(puP, luStart+lu, poolP);
            if (luP)
               DspuEnqueue(&poolP->hostLuPoolHeader, luP);
            else      
               /* Lu not created */
               DspuError1(ConfigNoMemoryForLu, luStart+lu);
                  
         }
      }
      else
        DspuError0(ConfigNoMemory);  /* poolP not created */
   }
}

      
/*=====================================================================*
* NO DSPU POOL poolName 
*======================================================================*
*
* Description:
* Revoke all or one specified instance(s) of POOL poolName.
*
**/

void      
DspuNKeyPool (const char* poolName)

{

   PoolCB* poolP;
   if ((poolP = FindPool(poolName)) == 0){
      DspuErrors(DSPU_POOL_InvalidPoolName, poolName);
      return;
   
   }
   if (!DeletePool(poolP))
       DspuError0(DSPU_POOL_PoolActiveMember);
}
   
   
/*=====================================================================*
* DSPU PU puName [RMAC rmac] [RSAP rSap] [XID id]
*         [LSAP lsap] [WINDOW windowSize] [MAXIFRAME maxIframe]
*         [RETRIES retryCount] [RETRY_TIMEOUT retryTimeout]
*======================================================================*
*
* Description:
*    Construct a pu.
**/

void
DspuKeyPu (const char* puName,        /* Dnstream PU name                   */
           const char* xidP,          /* XID                                */
           hword dlci,                /* DLCI number                        */
           byte  sdlc,                /* SDLC address                       */
           const char* x25,           /* X.25 address                       */
           const char* rMac,          /* Remote MAC address                 */
           byte  rSap,                /* Remote SAP address                 */ 
           byte  lSap,                /* Local SAP address                  */
           idbtype *idb,              /* Cfg'd interface (idb)              */
           hword windowSize,          /* Window size                        */
           hword maxIframe,           /* Maximimum Iframe size              */
           byte maxRetryCount,        /* Retry count                        */
           hword maxRetryTimeout)     /* Retry timeout                      */
{

  Pu* puP;
  Xid xid;

  /*---- Set retry timeout period. -1 => infinite. */
  if (maxRetryTimeout == INFINITE_MAX_RETRY_TIMEOUT)
    maxRetryTimeout = 0;

  /* make sure the qllc subaddress is valid. */
  if (x25 && (lSap == BadQllcSubaddr)) {
          DspuError0(InvalidQllcSubAddr);
	  return;
  }

  /* scan through both upstream and downstream that pu name is not defined */

  if ((puP = FindAllPu(puName)) != 0) {
      DspuErrors(DSPU_PU_Duplicated, puName);
      return;
  }

   if (strlen(xidP) > 8) {
       DspuError0(InvalidXID);
       return;
   }

  xid.xid = MakeXidFromId(xidP);
  if ( (dlci == NO_DLCI) && (sdlc == NO_SDLC) && 
       (x25 == NO_X25) && (rMac == NULL) && (xid.xid == NoXid) ) {

      DspuError0(DSPU_PU_NoRemoteAddressOrXid);
      return;
  }

  /* print warning for pu with xid-rcv parm _and_ address parm */
  if ( (xid.xid != NoXid) && 
       ((dlci != NO_DLCI) || (sdlc != NO_SDLC) ||
        (x25 != NO_X25) || (rMac != NULL)) ) {

      DspuError0(DSPU_PU_RemoteAddressAndXid);
  }

  if (!IsPuInterfaceDefnOkay(dlci, sdlc, x25, rMac, idb)) {
      DspuError0(DSPU_PU_InterfaceConflict);
      return;

  }

  puP = Pu_nw ( Downstream, puName, dlci, sdlc, x25, rMac, 
                rSap, lSap, idb, (byte)windowSize, maxIframe, 
                xid.xid, maxRetryCount, maxRetryTimeout);
  if (puP)
     lastPuConfigured = puP;
  else   
     DspuError0(ConfigNoMemory); 
      
}  /* End DspuKeyPu() */


/*=====================================================================*
* DSPU NO PU puName 
*======================================================================*
*
* Description:
*   Destruct a pu.   
**/

void
DspuNKeyPu(const char* puName)   /* dnstream PU name */

{
   
   Pu* puP;
   
   lastPuConfigured = 0;
   puP = FindPu(puName,Downstream);
   if (puP == 0)
      DspuErrors(DSPU_PU_NameNotFound, puName);
   else if (QueryAllChildren(GetLuChildren(puP), (ProcQChild)QueryLuActiveAndDedicated))
      DspuError0(DSPU_PU_NotInactive);
   else
      Pu_dl(puP);
}
  
   
/*=====================================================================*
* DSPU LU luStart [luEnd] POOL poolName HOST hostName 
*         [PU puName] hostLuStart
*======================================================================*
*
* Description:
*   if pool: save luStart and luRange at the pool headed by PU.
*   if host: construct LU control block by connecting Upstream to Dnstream
*  Note that either the pool OR the host can be configured.
**/

void
DspuKeyLu(byte  luStart, 
          byte  luEnd, 
          const char* poolName,
          const char* hostName,
          const char* puName,
          byte  hostLuStart)

{

   hword lu;
   Pu* puP;
   byte   range = 1;            /* Default luRange to 1 */
   
   if (puName)
   {
      if ((puP = FindPu(puName, Downstream)) == 0){
          DspuErrors(DSPU_LU_PuNameInvalid, puName);
          return;
      }
      lastPuConfigured = 0;
   }
   else
      puP = lastPuConfigured;

   if ( (luEnd > 0) && (luEnd < luStart) ) {   /* Range check luEnd addr */
       DspuError0(DSPU_LuEndBeforeLuStart);
       return;
   }


   if (luEnd != 0)
       range = (luEnd-luStart)+1;              /* Calculate luRange      */


   if (puP == 0) {
       DspuError0(DSPU_LU_CommNotFollowsPUComm);
   } else if (poolName && hostName) {
       DspuError0(DSPU_LU_InvalidPOOL_HOSTPara);
   } else if (hostName) {

      /* Lu are defined to connect both upstream and dnstream PU, check
       * if upstream pu is defined and both upstream/dnstream lu were
       * not used before.
       */
       
      Pu* hostPuP = FindPu(hostName,Upstream);

      if (hostPuP == 0)
         DspuErrors(DSPU_LU_HostNameInvalid, hostName);

      else if (QueryLuConflict(hostPuP,hostLuStart, range))
         DspuError0(DSPU_LU_ConflictHostLuDef);  /* upstream lu conflict */
      
      else if (QueryLuConflict(puP,luStart, range))
         DspuError0(DSPU_LU_ConflictPuLuDef);  /* dnstream lu conflict */

      else{
      
         /* all lu checking is now OK. Construct the LU control block.
          */
         
         for (lu = 0; lu < range; lu++){
             if (LuHost_nw(hostPuP, lu+hostLuStart, puP, luStart+lu) == 0)
                DspuError1(ConfigNoMemoryForDwLu, luStart+lu);  
                
         }
      }
   }
   else if (poolName) {
   
      /* pool name is defined. Find the pool from the pool name and
       * verify that the lu range is ok
       */
      if (QueryLuConflict(puP,luStart, range))
          DspuError0(DSPU_LU_ConflictPoolLuDef);

      else {
          PoolCB* poolCBP;             /* pool control block */
          
          PoolRange* poolRgeP = PoolRange_nw(puP, luStart, range, poolName);
          
          if (poolRgeP) {
      
            if ((poolCBP = FindPool(poolName)) == 0)
               /* Send a warning message */
               DspuErrors(DSPU_LU_PoolNameInvalid,poolName);
          
            else if (PuQueryActive(puP)) {
               ConnectStatus connectStatus;

               connectStatus = ConnectPoolRangeLusToPu(puP, poolCBP, poolRgeP);

               if (connectStatus == NoMemory)
                  DspuError0(ConfigNoMemToConnAllLus);

               else if (connectStatus == NoLusInPool)
                  DspuError0(InsufficientLusInPool);
            }   
          }
          else  /* poolRgeP not created */ 
            DspuError0(ConfigNoMemory);
      }
   }
   else 
      DspuError0(DSPU_LU_NoName);
      
}

/*=====================================================================*
* QueryLuActiveInRange           
*======================================================================*
*
* Description:
*   This function searchs all the LU children defined in the given PU  
* parent within the range (luStart) to (luStart+range), to determine 
* if any LUs are active. The LU is defined to be in an INACTIVE state
* if it's sscpLuState is Reset or dLuStrt.
*   TRUE is returned if there is at least one LU which is ACTIVE and an 
* error message is displayed. FALSE is returned if ALL LUs are INACTIVE.
*  
**/
static boolean QueryLuActiveInRange(Pu* puP, byte luStart, byte range)
{
   byte lu;
   boolean luActive = FALSE;

   for (lu = 0; lu < range; lu++){
      Lu* luP = GetChild(GetLuChildren(puP), lu + luStart);
      if (luP != 0 && QueryLuActive(luP)){     /* Lu active ? */
         DspuError1(DSPU_LU_PoolActiveMember, lu + luStart);
         luActive = TRUE;
      }
   }
   return luActive;
}


/*=====================================================================*
* DSPU NO LU luStart luEnd POOL poolName HOST hostName hostLuStart
*            [PU puName] hostLuStart
*======================================================================*
*
* Description:
*   if pool: destruct range control block if lu not active
*   if host: destruct LU control block if lu in reset
**/
void
DspuNKeyLu(byte  luStart, 
           byte  luEnd, 
           const char* poolName,
           const char* hostName,
           const char* puName,
           byte  hostLuStart
          )
{

   hword lu;
   byte  range = 1;              /* Default luRange to 1 */

   
   Pu* puP;
   if (puName)
   {
      if ((puP = FindPu(puName, Downstream)) == 0)
         DspuErrors(DSPU_LU_PuNameInvalid, puName);
      lastPuConfigured = 0;
   }
   else if ((puP = lastPuConfigured) == 0)
       DspuError0(DSPU_LU_CommNotFollowsPUComm);

   if (luEnd != 0)
       range = (luEnd-luStart)+1;              /* Calculate luRange       */

   if (puP == 0)
      return;
   
   else if (poolName){

      PoolRange* poolP;

      if ((poolP = PoolRange_free(poolName, puP,luStart,range)) == 0)
         DspuError0(DSPU_LU_PoolNotMatched);

      /* The pool range is found, now check if any LU is active. 
       */
      else if (!QueryLuActiveInRange(puP, luStart, range)){
     
      /* No LU is active, so disconnect the LUs from the PU parent
       * and delete the pool range. 
       */
         for (lu = 0; lu < range; lu++){
            Lu* luP = GetChild(GetLuChildren(puP), lu + luStart);
            if (luP != 0) {
               LuDisconnect(luP);
            }
            else if (FindPoolRange(puP,(byte)lu + luStart) == 0) 
               DspuError1(LuNotConfigured,lu + luStart); 
         }
         DspuDeleteQueueElement(PuGetPoolRangeHeader(puP), poolP);
         PoolRange_dl(poolP);
      }
   }
   else
   {
      /* If ALL the LUs are inactive then proceed to delete the LUs 
       * from the puP parent.
       */
      if (!QueryLuActiveInRange(puP, luStart, range)){
         for (lu = 0; lu < range; lu++){
            Lu* luP = GetChild(GetLuChildren(puP), lu + luStart);
            if (luP != 0)
               LuDelete(luP);
            else
               DspuError1(LuNotConfigured,lu + luStart); 
         }
      }
   }
}


/*=====================================================================*
* DSPU DEFAULT_PU [WINDOW windowSize] [MAXIFRAME maxIframe]
*======================================================================*
*
* Description:
*    Construct a default PU template.  If the default window size is
* configured, then the windowSize parameter passed is zero.  Similarly for
* maxiframe.  The reason for this is to distinguish between 
* "dspu default-pu" and "dspu default-pu window 7 maxiframe 1472"
* in the case when a default pu template is already configured.  
* i.e.   
* "dspu default-pu" is configured and then 
* "dspu default-pu window 7 maxiframe 1472" is entered,  
* the result is an error message - default pu configured twice.
* 
**/

void
DspuKeyDefaultPu (hword windowSize,
              hword maxIframe)

{

   if (dspu.defaultPuP){
      if ((windowSize == 0) && (maxIframe == 0))
          /*
           * Default pu template already exists and 
           * "dspu default-pu" is entered - this 
           * refers to the already configured default 
           * pu template.  Since the default pu template
           * doesn't have a name, this gives a handle to 
           * to the default-pu template to allow LUs to
           * be configured.
           */ 
          lastPuConfigured = dspu.defaultPuP;
      else
          /*
           * Default pu template already exists and 
           * "dspu default-pu window X maxiframe Y" 
           * configured - this is an attempt to reconfigure
           * the default pu template, display error message.
           */
          DspuError0(DSPU_DEFAULT_twice);
   }
   else{
      /*
       * Default-pu template doesn't exist.  Set up the default
       * windowSize and/or maxIframe values if default values
       * were configured.
       */
      if (windowSize == 0) 
          windowSize = DSPU_DEFAULT_WINDOW;
      if (maxIframe == 0) 
          maxIframe = DSPU_DEFAULT_MAXIFRAME;

      dspu.defaultPuP = Pu_nw(Downstream, 0, 0, 0, 0, 0, 0, 0, 0,
                                (byte) windowSize, maxIframe, NoXid, 0, 0);
      if (dspu.defaultPuP == 0)
        DspuError0(ConfigNoMemory);
      else                             
        lastPuConfigured = dspu.defaultPuP;
   }
}



/*=====================================================================*
* DspuEnableWrite
*======================================================================*
*
* Description:
*    nv ram write for "dspu enable-[host pu]"
*
**/

void DspuEnableWrite(DspuDLCType_e dlcType, idbtype *idb, hword direction)
{  
   AdaptorId id;
   Adaptor* adaptorP;

   id.dlcType = dlcType;
   id.u.adaptorPort[UnitIndex] = idb->hwptr->slot;
   id.u.adaptorPort[PortIndex] = idb->hwptr->unit;
   id.adaptorSubIfNumber       = idb->sub_number;
   id.dlcIdb = idb;
   
   /* Check adaptor defined */
   
   if ((adaptorP = QueryAllChildrenParm(
              &dspu.adaptorRoot, (ProcQChild1)AdaptorFind, &id)) != 0){
      AdaptorPrintDLC(adaptorP, direction);
   
      if (direction == Downstream){
         DoToAllChildrenParm2(GetDnlsRoot(), (ProcChild2)LsWriteOut, &id);
         DoToAllChildrenParm2(GetInactDpuRoot(), (ProcChild2)PuWriteOut, &id);
      }
      else{
         DoToAllChildrenParm2(&dspu.upstrLsRoot, (ProcChild2)LsWriteOut, &id);
         DoToAllChildrenParm2(&dspu.upstrPuRoot, (ProcChild2)PuWriteOut, &id);
      }
    }
}


/*=====================================================================*
* NO DSPU DEFAULT_PU
*======================================================================*
*
* Description:
*    Destruct a default PU template.
**/

void
DspuNKeyDefaultPu (void)

{

   lastPuConfigured = 0;
   if (dspu.defaultPuP){
      Pu_dl(dspu.defaultPuP);
      dspu.defaultPuP = 0;
   }
   else
      DspuError0(DSPU_DEFAULT_NotConfigured);
}


/*============================================================================*
* DspuShowConfig. 
*============================================================================*
*
* Description:
* Called by parse tree to write the nv ram. Since the pool and lu statements
* are controlled by the PU/host statment. The order that appear in the nv
* ram is as follows:
* 1. host
* 2. pool of the host, repeat 1,2
* 3. pu
* 4. lu range in pu, repeat 3,4
* 5. lu range in host
**/

void
DspuShowConfig (hword writeType)
{
   Adaptor* adaptorP;
   int  level;
   char tBuf[12];  

   switch (writeType){

      case DSPU_RSRB:
          if ( !GetRsrbSnaMode() ) {

              nv_write( GetRsrbLocalRing() != 0, "!\ndspu rsrb %d %d %d %e",
                  GetRsrbLocalRing(), GetRsrbBridgeNumber(),
                  GetRsrbTargetRing(), GetRsrbVirtualMacAddr());

          }
          if (GetRsrbLocalRing() != 0 && (adaptorP = RsrbAdaptor()) != 0) {
              AdaptorPrintRsrb(adaptorP);  
          }
          break;

      case DSPU_VDLC:
          if ( !GetVdlcSnaMode() ) {
              nv_write( GetVdlcRingNum() != 0, "!\ndspu vdlc %d %e",
                  GetVdlcRingNum(), GetVdlcVirtualMacAddr());
          }
          if (GetVdlcRingNum() != 0 && (adaptorP = VdlcAdaptor()) != 0) {
              AdaptorPrintVdlc(adaptorP);  
          }
          break;

      case DSPU_NCIA:
          if ( !GetNciaSnaMode() && GetNciaServerNum() != 0) {
              nv_write( TRUE, "!\ndspu ncia %d", GetNciaServerNum());
          }
          if (GetNciaServerNum() != 0 && (adaptorP = NciaAdaptor()) != 0) {
              AdaptorPrintNcia(adaptorP);  
          }
          break;

      case DSPU_HOST:
         DoToAllChildren(&dspu.upstrLsRoot, (ProcChild)LsWrite);
         DoToAllChildren(&dspu.upstrPuRoot, (ProcChild)PuWrite);
         break;
         
      case DSPU_ACTWINDOW:
         if (actWindowDefined) {
            nv_write(TRUE, "!\ndspu activation-window %d", 
                       SapGetActWindow());
         }
         break;
         
      case DSPU_PU:
         DoToAllChildren(GetDnlsRoot(), (ProcChild)LsWrite);
         DoToAllChildren(GetInactDpuRoot(), (ProcChild)PuWrite);

         break;
         
      case DSPU_DEFAULT_PU:
        if (dspu.defaultPuP)
            DefaultPuWrite(dspu.defaultPuP);

        /*
         * Write "dspu rsrb start" commands at end of DSPU config
         *
         */
        nv_write(RsrbAdaptor() != NULL, "!");
        DoToAllChildren(&dspu.upstrLsRoot, (ProcChild)LsWriteRsrbStart);
        DoToAllChildren(&dspu.upstrPuRoot, (ProcChild)DspuPuWriteRsrbStart);
        DoToAllChildren(GetDnlsRoot(), (ProcChild)LsWriteRsrbStart);
        DoToAllChildren(GetInactDpuRoot(), (ProcChild)DspuPuWriteRsrbStart);


        /*
         * Write "dspu vdlc start" commands at end of DSPU config
         *
         */
        nv_write(VdlcAdaptor() != NULL, "!");
        DoToAllChildren(&dspu.upstrLsRoot, (ProcChild)LsWriteVdlcStart);
        DoToAllChildren(&dspu.upstrPuRoot, (ProcChild)DspuPuWriteVdlcStart);
        DoToAllChildren(GetDnlsRoot(), (ProcChild)LsWriteVdlcStart);
        DoToAllChildren(GetInactDpuRoot(), (ProcChild)DspuPuWriteVdlcStart);

        break;

      case DSPU_NOTIFICATION:
        level = GetDspuNotificationLevel();
        if (level == DSPU_NOTIFY_LEVEL_DEFAULT)
            return;

        switch (GetDspuNotificationLevel()) {
          case DSPU_NOTIFY_LEVEL_OFF:
               sprintf(tBuf, "off");
               break;
          case DSPU_NOTIFY_LEVEL_LOW:
               sprintf(tBuf, "low");
               break;
          case DSPU_NOTIFY_LEVEL_MEDIUM:
               sprintf(tBuf, "medium");
               break;
          case DSPU_NOTIFY_LEVEL_HI:
               sprintf(tBuf, "high");
               break;
          default:
               sprintf(tBuf, "undefined");
          }
        nv_write(TRUE, "!\ndspu notification-level %s", tBuf);
        break;
        
     }   /* LU and POOL is done at DSPU_HOST */
}


/*============================================================================*
* SnaShowConfig
*=============================================================================*
*
* Description:
* Called by parse tree to write the nv ram. 
**/

void
SnaShowConfig (hword writeType)
{
   Adaptor* adaptorP;
   switch (writeType){

      case SNA_RSRB:
          if ( GetRsrbSnaMode() ) {

              nv_write( GetRsrbLocalRing() != 0, "!\nsna rsrb %d %d %d %e",
                  GetRsrbLocalRing(), GetRsrbBridgeNumber(),
                  GetRsrbTargetRing(), GetRsrbVirtualMacAddr());
              if (GetRsrbLocalRing() != 0 && (adaptorP = RsrbAdaptor()) != 0){
                  AdaptorPrintRsrb(adaptorP);  
              }

          }
          break;

      case SNA_VDLC:
          if ( GetVdlcSnaMode() ) {

              nv_write( GetVdlcRingNum() != 0, "!\nsna vdlc %d %e",
                  GetVdlcRingNum(), GetVdlcVirtualMacAddr());
              if (GetVdlcRingNum() != 0 && (adaptorP = VdlcAdaptor()) != 0){
                  AdaptorPrintVdlc(adaptorP);  
              }
          }
          break;

      case SNA_HOST:
         DoToAllChildren(&dspu.upstrLsRoot, (ProcChild)LsWrite);
         DoToAllChildren(&dspu.upstrPuRoot, (ProcChild)PuWrite);
         break;
         
     }  /* End switch(writeType) */

} /* End SnaShowConfig() */
      

/*=====================================================================*
* SHOW DSPU [PU puName] [all]
*======================================================================*
*
* Description:
*    show command
**/

void
DspuShow(const char* puName,
         boolean all)
{
   Pu* puP;
   if (puName) {

      if ((puP = FindAllPu(puName)) == 0){
          DspuErrors(DSPU_PU_NameNotFound, puName);
      }
      else{
         automore_enable(NULL);
         ShowPu(puP, all? ShowAllPu+ShowAllLu: ShowAllPu);
         automore_disable();
     }

   }
   else{
   
      
   /* DSPU HOST hostName INTERFACE ..  refer PU.C */
      
      automore_enable(NULL);
      DoToAllChildrenParm(&dspu.upstrLsRoot, (ProcChild1)ShowLs, 0);
      DoToAllChildrenParm(&dspu.upstrPuRoot, (ProcChild1)ShowPu, 0);
   
   /* DSPU PU puName INTERFACE ..  refer PU.C */
      
      DoToAllChildrenParm(GetDnlsRoot(), (ProcChild1)ShowLs, 0);
      DoToAllChildrenParm(GetInactDpuRoot(), (ProcChild1)ShowPu, 0);
      
      automore_disable();
      
   }
}


/*=====================================================================*
* SHOW DSPU [POOL puName] [all]
*======================================================================*
*
* Description:
*    show command
**/

void DspuShowPool (const char* poolName, boolean all)
{
   PoolCB* poolP;
   if (poolName) {

      if ((poolP = FindPool(poolName)) == 0){
          DspuErrors(DSPU_POOL_NameNotFound, poolName);
      }
      else{
         automore_enable(NULL);
         ShowPool(poolP, all);
         automore_disable();
      }
   }
   else{
   
      automore_enable(NULL);
      ShowAllPool();
      automore_disable();
      
   }
}

/*=====================================================================*
* DEBUG DSPU [puName] [debugLevel]
*======================================================================*
*
* Description:
* Enable debugging for a specific (or all) resources.
*
**/
boolean DspuDebug (const char* puName, hword debugLevel)
{

    Pu* puP;
    boolean debugFlagsSet = TRUE;

    if (puName) {
        puP = FindAllPu(puName);
        if (!puP) { 
            DspuErrors(DSPU_LU_PuNameInvalid, puName);
            debugFlagsSet = FALSE;
        } else {
            debugLevel |= GetLuDebugLevel(puP);
            DebugPuWithName(puP, debugLevel);
        }
    } else {
        debugLevel |= GetDebugLevel();
        SetDebugLevel(debugLevel);
        DoToAllChildrenParm(&dspu.upstrLsRoot, (ProcChild1)DebugLs, 
                              debugLevel);
        DoToAllChildrenParm(&dspu.upstrPuRoot, (ProcChild1)DebugPu,
                              debugLevel);
        DoToAllChildrenParm(GetDnlsRoot(), (ProcChild1)DebugLs,
                              debugLevel);
        DoToAllChildrenParm(GetInactDpuRoot(), (ProcChild1)DebugPu,
                              debugLevel);
    }

    return(debugFlagsSet);

}  /* End DspuDebug() */


/*============================================================================*
* NO DSPU DEBUG
*============================================================================*
*
* Description:
* Disable debugging for one or all resources.
*
**/

boolean DspuNoDebug (const char *puName, hword debugLevel)
{

    Pu* puP;
    boolean debugFlagsSet = TRUE;

    if (puName)
    {
        puP = FindAllPu(puName);
        if (!puP) {
            DspuErrors(DSPU_LU_PuNameInvalid, puName);
            debugFlagsSet = FALSE;
        } else {
           if (debugLevel & GetDebugLevel()) {
               DspuErrors(DspuDbgNotReset, GetPuName(puP));
               debugFlagsSet = FALSE;
           } else {
               NoDebugPuWithName(puP, debugLevel);
           }
        }
     }
     else {
         SetDebugLevel( (~debugLevel) & GetDebugLevel() );
         DoToAllChildrenParm(&dspu.upstrLsRoot, (ProcChild1)NoDebugLs, 
                               debugLevel);
         DoToAllChildrenParm(&dspu.upstrPuRoot, (ProcChild1)NoDebugPu, 
                               debugLevel);
         DoToAllChildrenParm(GetDnlsRoot(), (ProcChild1)NoDebugLs,
                               debugLevel);
         DoToAllChildrenParm(GetInactDpuRoot(), (ProcChild1)NoDebugPu,
                               debugLevel);
     }

    return(debugFlagsSet);

}  /* End DspuNoDebug() */


/*------------------------------------------------------------------------*/


/* 
 * IsPuInterfaceDefnOkay() 
 *   Verify PU addressing definition matches PU interface specification
 *
 */
static boolean IsPuInterfaceDefnOkay(hword dlci, byte sdlc, 
                                     const char *x25, const char *rMac,
                                     idbtype *idb) 
{

   if (!idb)
     return(TRUE);

   /* 
    * Ensure that the user has not configured "interface serial" 
    *   on a PU definition using MAC addressing
    *
    */
   if ( (rMac) && ((idb->hwptr->status & IDB_SERIAL) != 0) ) {
     return(FALSE);
   }

   /*
    * Ensure that user has configured "interface serial" 
    *   on a PU definition using SDLC/DLCI/X25 addressing
    *
    */
   if ( ((dlci != NO_DLCI) || (sdlc != NO_SDLC) || (x25 != NO_X25)) &&
        ((idb->hwptr->status & IDB_SERIAL) == 0) ) {
     return(FALSE);
   }

   return(TRUE);

} /* End IsPuIntefaceDefnOkay() */



/*============================================================================*
* DspuIsValidPuName
*============================================================================*
*
* Description:
* Validates that the passed string is a valid VTAM name.
*
* Returns TRUE if valid, FALSE if not.
**/

boolean DspuIsValidPuName (const char* hostName)
{
  int nameLength = 0;
  int index = 0;
  char c;

  nameLength = strlen(hostName);

  /* validate incoming argument */
  if ((!hostName) ||
      (nameLength == 0) ||
      (nameLength > 8)) {
    return (FALSE);
  } /* endif */

  /* 
   * Make sure host/pu name begins with an ALPHA character
   * and contains only A-Z, 0-9, or '%$#'
   * If not, return false.
   */

  for (index = 0; index < nameLength; index++) {
    c = toupper(hostName[index]);

    /* check 1st character for ALPHA */
    if (index == 0) {
      if (! ((c >= 'A') &&  (c <= 'Z')) ) {
	return (FALSE);
      } /* endif */
    } /* endif */

    if (!( ((c >= '0') &&  (c <= '9')) ||
	   ((c >= 'A') &&  (c <= 'Z')) ||
	   (c == '$')                  ||
	   (c == '#')                  ||
	   (c == '@')) ) {
      return (FALSE);
    } /* endif */

  }
  return (TRUE);
}
