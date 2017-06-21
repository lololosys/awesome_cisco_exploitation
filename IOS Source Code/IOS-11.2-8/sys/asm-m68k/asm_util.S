| $Id: asm_util.S,v 3.2 1995/11/17 08:40:05 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/asm-m68k/asm_util.S,v $
|------------------------------------------------------------------
| General purpose assembly language routines
|
| May 18, 1994, Eric B. Decker
|
| Copyright (c) 1994 by cisco Systems, Inc.
| All rights reserved.
| 
| This module contains the assembly language routines that are
| very general in use.  Not exception handling but certainly
| processor specific (its assembly language).  They should be
| kept to a minimum.
|
| Other files of interest:
|
|	asm.S		old catch all
|	asm_util.S	general purpose assembly routines
|	asm_copy.S	bcopy and friends
|	asm_kernel.S	supervisor mode only routines
|			includes exception handling
|------------------------------------------------------------------
| $Log: asm_util.S,v $
| Revision 3.2  1995/11/17  08:40:05  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  10:54:50  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  20:06:08  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|
|
| This is a placeholder for the EP branch
