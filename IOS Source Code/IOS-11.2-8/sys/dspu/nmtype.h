/* $Id: nmtype.h,v 3.2.60.1 1996/05/17 10:49:01 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/nmtype.h,v $
 *------------------------------------------------------------------
 * SNA Network Management data type
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nmtype.h,v $
 * Revision 3.2.60.1  1996/05/17  10:49:01  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/02/28  21:23:56  ppearce
 * CSCdi50230:  IBU modularity - Remove unnecessary header file includes
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  09:06:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:52  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef nmtype_h
#define nmtype_h

#include "../ibm/ibm_types.h"
#include "snanm.h"
/* network management nmvt vector */

typedef struct SV SV;
typedef struct MVReceiver MVReceiver;       /* Vector receiver client */
typedef struct DataHeader DataHeader;
typedef struct RuncmdReceiver RuncmdReceiver;
typedef struct RuncmdCB RuncmdCB;


#endif

