/* $Id: stac_comp.h,v 3.2 1995/11/17 18:05:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/wan/stac_comp.h,v $
 *------------------------------------------------------------------
 * COMP_STAC.H
 *
 * Feburary 11, 1994 Bill Thomas
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All Rights Reserved
 *------------------------------------------------------------------
 * $Log: stac_comp.h,v $
 * Revision 3.2  1995/11/17  18:05:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:51:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:20:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************************
*
* Copyright 1988-92 Stac Electronics, Carlsbad, California.
* All rights reserved.  This code is Stac confidential property, and the
* algorithms and methods used herein may not be disclosed to any party without
* express written consent from Stac Electronics.
*
******************************************************************************
*
* Contents:		Symbolic Constants
*				Performance Tuning Declarations
*				Processor Specific Macros
*				Function Prototypes
*
****************************************************************************/

#ifndef __COMP_STAC_H__
#define __COMP_STAC_H__

/* to improve porting...we're just going to directly include stac's headers
 * from now on
 */

#include "../wan/LZS.H"

/* define a few stac forgot */
#define		LZS_NO_RESET		0
#define		LZS_RESET		1

#endif __COMP_STAC_H__

/************************/
/* End File comp_stac.h */
/************************/

