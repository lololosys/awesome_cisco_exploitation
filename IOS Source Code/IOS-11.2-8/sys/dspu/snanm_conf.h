/* $Id: snanm_conf.h,v 3.2 1995/11/17 09:06:14 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/snanm_conf.h,v $
 *------------------------------------------------------------------
 * SNA Network Management Configuration 
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm_conf.h,v $
 * Revision 3.2  1995/11/17  09:06:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:00  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:42  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef snanm_conf_h
#define snanm_conf_h


void Fp_endNode(char* netId, char* cpName);
void Fp_showEndNode(void);
void Fp_NoEndNode(void);


void Fp_location(char* location);
void Fp_NoLocation(void);
void Fp_showLocation(void);

const char* GetFpLocation (void);

#endif
