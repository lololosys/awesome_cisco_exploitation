/* $Id: dspuconf.h,v 3.3.60.1 1996/05/17 10:48:14 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspuconf.h,v $
 *------------------------------------------------------------------
 * DSPU Parser Call Interface.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * This exports the DSPU call interface to the Parser commands.
 *------------------------------------------------------------------
 * $Log: dspuconf.h,v $
 * Revision 3.3.60.1  1996/05/17  10:48:14  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.48.2  1996/05/03  17:18:31  schiang
 * CSCdi56627:  DSPU/NCIA interface is broken
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3.48.1  1996/03/28  08:20:39  pmorton
 * Merge VDLC fixes to IbuMod_Calif_branch
 *
 * Revision 3.3.4.1  1996/01/20  06:08:54  ppearce
 * CSCdi47227:  DLSw+ support for DSPU/SNANM via CLS/VDLC
 * Branch: Cal_Ibm_branch
 *
 * Revision 3.3  1995/11/17  09:05:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:22:48  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:28:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  22:42:08  richl
 * CSCdi38432:  dspu subsystem causing confusion, rename it
 *
 * Revision 2.2  1995/07/06  23:15:51  ppearce
 * CSCdi36790:  Add interface parm to dspu host/pu config command
 *
 * Revision 2.1  1995/06/07  20:28:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPUCONFIG_H
#define __DSPUCONFIG_H


#include "dtypes.h"

#define NoRing 0xffff   


/*=========================================================================*
* dspu_start ()
*==========================================================================*
*
* Description:
*   Called during DSPU configuration to start DSPU Main Process
*
*/
void dspu_start (void);


/*=========================================================================*
* DspuShowConfig. *
*=========================================================================*
*
* Description:
* Called by Cisco "SHOW CONFIG" module(s), to display DSPU configuration.
*
**/

void
DspuShowConfig (hword type);


/*=========================================================================*
* SnaShowConfig. *
*=========================================================================*
*
* Description:
* Called by Cisco "SHOW CONFIG" module(s), to display SNA configuration.
*
**/

void
SnaShowConfig (hword type);

void DspuEnableWrite(DspuDLCType_e dlcType, idbtype *idb, hword direction);


/*=====================================================================*
* SHOW DSPU [pu puName] [all]
*======================================================================*
*
* Description:
* Display DSPU configuration for all, or a specific resource(s).
*
**/

void
DspuShow (const char* puName,           /* Resource (pu) name. May be zero. */
          boolean all);                 /* Default is to show everything. */

/*=====================================================================*
* SHOW DSPU [pool poolName] [all]
*======================================================================*
*
* Description:
* Display DSPU pool configuration for all, or a specific resource(s).
*
**/

void
DspuShowPool (const char* poolName,           /* Resource (pu) name. May be zero. */
          boolean all);                 /* Default is to show everything. */


/*=====================================================================*
* DEBUG DSPU [puName] LEVEL [debugLevel]
*======================================================================*
*
* Description:
* Set debugging level for particular DSPU resource.
*
**/
boolean DspuDebug (const char* puName, hword debugLevel);  


/*=====================================================================*
* NO DSPU DEBUG [puName]
*======================================================================*
*
* Description:
* Reset debugging level for particular DSPU resource.
*
**/
boolean DspuNoDebug (const char* puName, hword debugLevel);


/*=====================================================================*
* DSPU ENABLE [<lsap>]
*======================================================================*
*
* Description:
* Enable DSPU on this interface.
*
**/

void
DspuKeyEnable (DspuDLCType_e dlcType,     /* DLC type                    */
               idbtype *idb,              /* Pointer to DLC idb          */
               byte localSap,             /* Local SAP number            */
               int direction);            /* Upstream or Downstream      */


/*=====================================================================*
* NO DSPU ENABLE
*======================================================================*
*
* Description:
* Disable DSPU on this interface.
*
**/

void
DspuNKeyEnable (DspuDLCType_e dlcType, idbtype *idb, byte localSap);

/*=====================================================================*
* DSPU RSRB <localRing> <bridgeNumber> <targetRing> <virtualMacAddress> 
*======================================================================*
*
* Description:
* Make RSRB known within DSPU.
*
**/

void
DspuKeyRsrb (hword localRingNum,        /* Local ring number     */
             byte  bridgeNum,           /* Bridge number         */
             hword targetRingNum,       /* Target ring number    */
             const char* macAddr);      /* Virtual MAC address   */


/*=====================================================================*
* NO DSPU RSRB
*======================================================================*
*
* Description:
* Make unknown RSRB within DSPU.
*    
**/

void
DspuNKeyRsrb (hword localRingNum,        /* Local ring number     */
              byte  bridgeNum,           /* Bridge number         */
              hword targetRingNum,       /* Target ring number    */
              const char* macAddr);      /* Virtual MAC address   */


/*=====================================================================*
* DSPU VDLC <ringNumber> <virtualMacAddress> 
*======================================================================*
*
**/

void
DspuKeyVdlc (hword ringNum,             /* Ring number           */
             const char* macAddr);      /* Virtual MAC address   */


/*=====================================================================*
* NO DSPU VDLC
*======================================================================*
*
**/

void
DspuNKeyVdlc (hword ringNum,             /* Ring number           */
              const char* macAddr);      /* Virtual MAC address   */


/*=====================================================================*
* DSPU VDLC SERVER <number>
*======================================================================*
*
**/

void
DspuKeyNcia (int ServerNum);             /* Server number         */


/*=====================================================================*
* NO DSPU NCIA
*======================================================================*
*
**/
void
DspuNKeyNcia (int ServerNum);            /* Server number         */



/*=====================================================================*
* DSPU ACTWINDOW <window_size>
*======================================================================*
*
* Description:
* Set the number of activation buffers per sap.
* 
**/

void
DspuKeyActWindow (hword actWindow);     /* Not optional. */


/*=====================================================================*
* NO DSPU ACTWINDOW
*======================================================================*
*
* Description:
* Reset the number of activation buffers per sap.
*     
**/

void
DspuNKeyActWindow (void);


/*=====================================================================*
* HOST <hostName> RSRB XID <xid> [RMAC <remoteMac>] 
*      [RSAP <remoteSap>] [LSAP <lsap>] [FOCALPOINT] 
*      [WINDOW <window_size>] [MAXIFRAME <max_iframe>]
*      [RETRIES <retry_count>] [RETRY_TIMEOUT <retry_timeout>]
*======================================================================*
*
* Description:
*    Construct an upstream sap, link station and PU. The Sap is a child of
* RSRB adaptor.
**/

void
DspuKeyHost (const char* hostName,      /* Upstream PU name                   */
             const char* xidP,          /* XID                                */
             hword dlci,                /* DLCI number                        */
             byte  sdlc,                /* SDLC address                       */
             const char* x25,           /* X.25 address                       */
             const char* rMac,          /* Remote MAC address                 */
             byte  remoteSap,           /* Remote SAP address                 */
             byte  lsap,                /* Local SAP address                  */
             idbtype *idb,              /* Cfg'd interface (idb)              */
             hword windowSize,          /* Window size                        */
             hword maxIframe,           /* Maximimum Iframe size              */
             byte maxRetryCount,        /* Retry count                        */
             hword maxRetryTimeout,     /* Retry timeout                      */
             boolean focalPoint);       /* FocalPoint PU                      */


/*=====================================================================*
* NO DSPU HOST <hostName>
*======================================================================*
*
* Description:
*   
**/

void
DspuNKeyHost (const char* hostName);    /* upstream Pu name  */


/*=====================================================================*
* DSPU POOL poolName HOST hostName LU luStart [<luRange>]
*======================================================================*
*
* Description:
*    Construct a poolType and put it in the root queue list.  
* Construct all the LUs defined.
*
**/

void      
DspuKeyPool (char* poolName,
             const char* hostName,
             byte luStart,              /* Start LU & 0 luRange => pool of 1. */
             byte luRange,              /* 0 = none. */
             hword inactivityTimeout);  /* 0 = no timeout */

        
/*=====================================================================*
* NO DSPU POOL poolName [HOST <hostName> LU luStart [<luRange>]]
*======================================================================*
*
* Description:
* Revoke all or one specified instance(s) of POOL poolName.
*
**/

void      
DspuNKeyPool (const char* poolName);


/*=====================================================================*
* DSPU PU <puName> [RMAC <rmac>] [RSAP <rSap>] [XID <xid>]
*         [TOKENRING <unit>/<port>] [WINDOW <window_size>]
*         [MAXIFRAME <max_iframe>] [RETRIES <retry_count>]
*         [RETRY_TIMEOUT <retry_timeout>]
*         [WINDOW windowSize][MAXIFRAME maxIframe]
*
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
           byte  remoteSap,           /* Remote SAP address                 */
           byte  lsap,                /* Local SAP address                  */
           idbtype *idb,              /* Cfg'd interface (idb)              */
           hword windowSize,          /* Window size                        */
           hword maxIframe,           /* Maximimum Iframe size              */
           byte maxRetryCount,        /* Retry count                        */
           hword maxRetryTimeout);    /* Retry timeout                      */
      
/*=====================================================================*
* NO PU puName 
*======================================================================*
*
* Description:
*    
**/

void
DspuNKeyPu (const char* puName);        /* Downstream PU name. */


/*=====================================================================*
* LU luStart [luRange] [PU puName] [POOL poolName] 
*    [HOST hostName hostLuStart]
*======================================================================*
*
* Description:
*   if pool: save luStart and luRange at the pool headed by PU.
*   if host: construct LU control block by connecting Upstream to 
*            Downstream.
**/

void
DspuKeyLu (byte  luStart,               /* Must be specified. */
           byte  range,                 /* May be 0. Implies range of 1. */
           const char* poolName,        /* Pool name. Null for LU HOST */
           const char* hostName,        /* Host name. Null for LU POOL */
           const char* puName,          /* 0 => use last cfg'd PU. */ 
           byte  hostLuStart);          /* Only valid for LU HOST */


/*=====================================================================*
* NO LU luStart luRange [PU puName] [POOL poolName] [HOST hostName hostLuStart]
*======================================================================*
*
* Description:
*   if pool: destruct range control block if lu not active
*   if host: destruct LU control block if lu in reset
**/

void
DspuNKeyLu(byte  luStart,               /* Must be specified. */
           byte  range,                 /* May be 0. Implies range of 1. */
           const char* poolName,        /* Must be specified. */
           const char* hostName,        /* Must be specified. */
           const char* puName,          /* 0 => use last cfg'd PU. */ 
           byte  hostLuStart);          /* Only valid for LU HOST */


/*=====================================================================*
* DSPU DEFAULT_PU [WINDOW <windowSize>] [MAXIFRAME <maxIframe>]
*======================================================================*
*
* Description:
*    Construct a default PU template
*
**/

void
DspuKeyDefaultPu (hword windowSize,     /* May be 0. */
                  hword maxIframe);     /* May be 0. */


/*=====================================================================*
* NO DSPU DEFAULT_PU
*======================================================================*
*
* Description:
*    
**/

void
DspuNKeyDefaultPu (void);

void DspuKeyStart(const char* name,          /* PU/Host name            */
                DspuDLCType_e dlcType,       /* DLC type                */
                idbtype *idb);               /* Pointer to DLC idb      */

void DspuNKeyStart(const char* name);

void DspuKeyRsrbStart(const char* name);
void DspuKeyVdlcStart(const char* name);

void DspuActivate(const char* name);

void DspuDeactivate(const char* name);

void DspuRsrbKeyEnable(byte lsap, int direction);
void DspuVdlcKeyEnable(byte lsap, int direction);
void DspuNciaKeyEnable(byte lsap, int direction);

void DspuRsrbNKeyEnable(byte lsap);
void DspuVdlcNKeyEnable(byte lsap);
void DspuNciaNKeyEnable(byte lsap);


/*=====================================================================*
* DspuIsValidPuName
*======================================================================*
*
* Description:
*    
* Validates passed string is a valid VTAM name.
* Returns TRUE if valid, FALSE if not.
**/

boolean DspuIsValidPuName(const char*);

#endif
/*---- End include file: config.h. */
