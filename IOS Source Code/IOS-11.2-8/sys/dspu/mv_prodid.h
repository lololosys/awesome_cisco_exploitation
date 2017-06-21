/* $Id: mv_prodid.h,v 3.2 1995/11/17 09:05:48 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/mv_prodid.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Product Set ID major vector
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mv_prodid.h,v $
 * Revision 3.2  1995/11/17  09:05:48  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:36  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:29:53  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef mv_prodid_h
#define mv_prodid_h

#include "nmtype.h"


typedef struct ProductId 
 {
  byte A_psIdLen, A_psIdKey, retired;
  byte hwLen, hw_id;
  byte hw_class;
  byte hwLen2;
  byte hw_prodid;
  byte hw_format;
  byte machineType[4];
  
  byte plant[2];
  byte sequence[7];

  byte swLen, sw_id;
  byte sw_class;
  byte commonLevelLen;
  byte commonLevelKey;
  byte version[2];
  byte release[2];
  byte modification[2];

  byte softId;
  byte softKey;
  byte softProductId[7];

  byte A_swNameLen, A_swNameKey;
  byte A_swName[0x10];
}ProductId ;


void ProductIdInit(void);

void SetSequenceNumber(const char* hostName);

extern  ProductId productSetId;


#endif
