/* $Id: interface_generic.h,v 3.1.2.1 1996/05/17 11:15:47 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/h/interface_generic.h,v $
 *------------------------------------------------------------------
 * interface_generic.h.  A stable subset of interface_private.h
 *
 * April 1996, Gordon Cox
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: interface_generic.h,v $
 * Revision 3.1.2.1  1996/05/17  11:15:47  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.3  1996/05/07  05:14:33  gcox
 * Branch: IbuMod_Calif_branch
 * Improve compile error when tangled includes; Improve comments
 *
 * Revision 3.1.4.2  1996/04/29  21:54:31  gcox
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1.4.1  1996/04/26  07:52:50  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.1  1996/04/26  05:28:12  dstine
 * Placeholder for the IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __INTERFACE_GENERIC_H__
#define __INTERFACE_GENERIC_H__

#ifndef __INTERFACE_PRIVATE_H__

#include "interface_types.h"  /* supports generic stuff generation */


struct hwidbtype_ {          /*******************************/
                             /*                             */ 
  HWIDB_FIELDS_GENERIC       /*      These statements       */ 
                             /*        must be kept         */ 
  HWIDB_FIELDS_IBU           /*          together           */ 
                             /*                             */ 
};                           /*******************************/ 



struct idbtype_ {            /*******************************/
                             /*                             */ 
  SWIDB_FIELDS_GENERIC       /*      These statements       */ 
                             /*        must be kept         */ 
  SWIDB_FIELDS_IBU           /*          together           */ 
                             /*                             */ 
};                           /*******************************/ 


INTERFACE_GENERIC_INLINES

INTERFACE_IBU_INLINES


#else

struct hwidbtype_ { int dummy; } deliberate_compile_error;
/*
 * If you see an error here, the real cause of the problem is that 
 * you have included both interface_generic.h and interface_private.h.
 * These are incompatible, and only one can be included.
 * Therefore .h files which include one cannot be included 
 * in .c files which include the other or which include a .h file  
 * which includes the other.
 */

#endif __INTERFACE_PRIVATE_H__
#endif __INTERFACE_GENERIC_H__
