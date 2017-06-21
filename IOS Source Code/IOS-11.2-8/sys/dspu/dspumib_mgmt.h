/* $Id: dspumib_mgmt.h,v 3.2 1995/11/17 09:05:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspumib_mgmt.h,v $
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
 * $Log: dspumib_mgmt.h,v $
 * Revision 3.2  1995/11/17  09:05:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:03:24  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/09/14 06:15:24  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.1  1995/06/07  20:28:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPUMIB_MGMT_H
#define __DSPUMIB_MGMT_H
 
#include "dspu.h"
#include "dspu_adaptor.h"
#include "dspu_lu.h"
#include "dspu_pool.h"


/*=====================================================================*
* InitDspuMibTables ()
*======================================================================*
*
* Description:
*   Initializes the FirstEntry pointers for DSPU MIB tables
*
**/
void InitDspuMibTables (void);


/*=====================================================================*
* AddEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Adds a pool entry to the dspuPoolClassTable
*
**/
void AddEntry_DspuPoolClassTable (PoolCB* inputPoolP);


/*=====================================================================*
* RemoveEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Removes a pool entry from the dspuPoolClassTable
*
**/
void RemoveEntry_DspuPoolClassTable (PoolCB* inputPoolP);


/*=====================================================================*
* FindEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Find a pool entry in the dspuPoolClassTable
*
**/
PoolCB* FindEntry_DspuPoolClassTable (ulong poolIdx);


/*=====================================================================*
* FindNextEntry_DspuPoolClassTable ()
*======================================================================*
*
* Description:
*   Find next pool entry in the dspuPoolClassTable
*
**/
PoolCB* FindNextEntry_DspuPoolClassTable (ulong poolIdx);

/*=====================================================================*
* GetDspuPuTableFirstEntryP ()
*======================================================================*
*
* Description:
*   Returns first entry of the dspuPuTable
*
**/
Pu* GetDspuPuTableFirstEntryP (void);


/*=====================================================================*
* GetDspuPuTableLastEntryP ()
*======================================================================*
*
* Description:
*   Returns last entry of the dspuPuTable
*
**/
Pu* GetDspuPuTableLastEntryP (void);


/*=====================================================================*
* AddEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Adds a PU entry to the dspuPuTable
*
**/
void AddEntry_DspuPuTable (Pu* inputPuP);


/*=====================================================================*
* RemoveEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Removes a pool entry from the poolClass table
*
**/
void RemoveEntry_DspuPuTable (Pu* inputPuP);


/*=====================================================================*
* FindEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Find a PU entry in the dspuPuTable 
*
**/
Pu* FindEntry_DspuPuTable (ulong puIdx);


/*=====================================================================*
* FindNextEntry_DspuPuTable ()
*======================================================================*
*
* Description:
*   Find next PU entry in the dspuPuTable 
*
**/
Pu* FindNextEntry_DspuPuTable (ulong puIdx);


/*=====================================================================*
* FindEntry_DspuLu ()
*======================================================================*
*
* Description:
*   Find the specified LU entry owned by the given PU
*
**/
Lu* FindEntry_DspuLu (Pu* puP, byte luAddr, PoolRange **prP);


/*=====================================================================*
* FindNextEntry_DspuLu ()
*======================================================================*
*
* Description:
*   Find the next LU entry owned by the given PU
*
**/
Lu* FindNextEntry_DspuLu (Pu* puP, byte *luAddr, PoolRange **prP);


/*=====================================================================*
* FindEntry_DspuSapTable ()
*======================================================================*
*
* Description:
*   Find the specified SAP entry owned by the given adaptor
*
**/
Sap* FindEntry_DspuSapTable (byte sapAddr, AdaptorId *adaptorIdP);


/*=====================================================================*
* FindNextEntry_DspuSapTable ()
*======================================================================*
*
* Description:
*   Find the next SAP entry owned by the given adaptor
*
**/
Sap* FindNextEntry_DspuSapTable (byte sapAddr, AdaptorId *adaptorIdP);

#endif /* __DSPUMIB_MGMT_H */
