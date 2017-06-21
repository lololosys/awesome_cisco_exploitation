/* $Id: mv_sent.h,v 3.2 1995/11/17 09:05:51 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/mv_sent.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Major Vector sent to focal point
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_sent.h,v $
 * Revision 3.2  1995/11/17  09:05:51  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:38  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef mv_sent_h
#define  mv_sent_h

#include "dtypes.h"
#include "nmtype.h"
MV_SV_RC
MV_VectorSent(DataHeader* vector, char* correlator, hword corrleatorLen);

#endif

