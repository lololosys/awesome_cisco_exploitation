/* $Id: dspu_alert.h,v 3.1 1995/11/09 11:27:59 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/dspu_alert.h,v $
 *------------------------------------------------------------------
 * DSPU Alert header.
 *
 * July 1994, Sandy Logie
 *
 * Copyright (c) 1993-1995 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 * 
 * Alert PassThrough header.
 *------------------------------------------------------------------
 * $Log: dspu_alert.h,v $
 * Revision 3.1  1995/11/09  11:27:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/20  17:02:44  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef alert_h
#define alert_h


#include "dtypes.h"

DataReq_t*                            /* new NMVT alert */
AlertPassThru(Pu* puP,             /* pu name */
                DataReq_t* clsiBfrP);    /* NMVT alert message */


             
#endif
