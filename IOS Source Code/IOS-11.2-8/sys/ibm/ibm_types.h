/* $Id: ibm_types.h,v 3.2.60.1 1996/08/02 22:53:38 smackie Exp $
 * $Source: /release/112/cvs/Xsys/ibm/ibm_types.h,v $
 *------------------------------------------------------------------
 * ibm_types.h - common base types fpr ibm software.
 *
 * March, 1995,		Phil  Morton
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ibm_types.h,v $
 * Revision 3.2.60.1  1996/08/02  22:53:38  smackie
 * Attempt to kill two types.h birds with one stone. (CSCdi64993)
 *
 *    o Add typedefs for size-oriented intX and uintX types
 *    o Rename dev_t to flash_dev_t to avoid near-future conflicts
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  09:22:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:44:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:46:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __IBM_TYPES_H__
#define __IBM_TYPES_H__
      
#ifndef CSWEXDEF_H_INCLUDED		/* This is for APPN compatibility */

/*
 * Definitions when the natural int size is 4 bytes
 */

typedef unsigned char byte;             /* portable definition for an 8 bit  */
typedef unsigned short hword;           /* 16 bit field                      */
typedef unsigned int word;              /* 32 bit field                      */

#endif /* CSWEXDEF_H_INCLUDED */

#endif /* __IBM_TYPES_H__ */
