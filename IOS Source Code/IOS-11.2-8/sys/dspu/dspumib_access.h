/* $Id: dspumib_access.h,v 3.2 1995/11/17 09:05:12 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspumib_access.h,v $
 *------------------------------------------------------------------
 * DSPU MIB Access
 *
 * September 1994, Paul Pearce
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Define methods for returning DSPU MIB object information 
 *------------------------------------------------------------------
 * $Log: dspumib_access.h,v $
 * Revision 3.2  1995/11/17  09:05:12  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:28:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __DSPUMIB_ACCESS_H
#define __DSPUMIB_ACCESS_H

#include "sr_dspumib.h"


/*=====================================================================*
* GetDspuNode () 
*======================================================================*
*
* Description:
*   Returns dspuNode information 
*
**/
boolean GetDspuNode (dspuNode_t* nodeP);


/*=====================================================================*
* GetDspuPoolClassEntry ()
*======================================================================*
*
* Description:
*   Returns dspuPoolClassEntry information
*
**/
boolean GetDspuPoolClassEntry (dspuPoolClassEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuPooledLuEntry ()
*======================================================================*
*
* Description:
*   Returns dspuPooledLuEntry information
*
**/
boolean GetDspuPooledLuEntry (dspuPooledLuEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuPuAdminEntry ()
*======================================================================*
*
* Description:
*   Returns dspuPuAdminEntry information
*
**/
boolean GetDspuPuAdminEntry (dspuPuAdminEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuPuOperEntry ()
*======================================================================*
*
* Description:
*   Returns dspuPuOperEntry information
*
**/
boolean GetDspuPuOperEntry (dspuPuOperEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuPuStatsEntry ()
*======================================================================*
*
* Description:
*   Returns dspuPuStatsEntry information
*
**/
boolean GetDspuPuStatsEntry (dspuPuStatsEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuLuAdminEntry ()
*======================================================================*
*
* Description:
*   Returns dspuLuAdminEntry information
*
**/
boolean GetDspuLuAdminEntry (dspuLuAdminEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuLuOperEntry ()
*======================================================================*
*
* Description:
*   Returns dspuLuOperEntry information
*
**/
boolean GetDspuLuOperEntry (dspuLuOperEntry_t* entryP, int searchType);


/*=====================================================================*
* GetDspuSapEntry ()
*======================================================================*
*
* Description:
*   Returns dspuSapEntry information
*
**/
boolean GetDspuSapEntry (dspuSapEntry_t* entryP, int searchType);

#endif /* __DSPUMIB_ACCESS_H */
