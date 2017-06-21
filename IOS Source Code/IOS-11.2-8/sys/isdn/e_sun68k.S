| $Id: e_sun68k.S,v 3.2 1995/11/17 17:40:27 hampton Exp $
| $Source: /swtal/cherf.111/ios/sys/isdn/e_sun68k.S,v $
|------------------------------------------------------------------
| e_sun68k.S  Low level support routines for MTEX on 68K suns
|
| March 1992, Ronnie Kon
|
| Copyright (c) 1992-1994 by cisco Systems, Inc.
| All rights reserved.
| 
|------------------------------------------------------------------
| $Log: e_sun68k.S,v $
| Revision 3.2  1995/11/17  17:40:27  hampton
| Remove old entries from the RCS header logs.
|
| Revision 3.1  1995/11/09  12:13:22  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  21:15:03  hampton
| Bump version numbers from 1.x to 2.x.
|
|------------------------------------------------------------------
| $Endlog$
|
/* Renamed to e_sun68k.S so make dependencies works.  === */
|*************************************************************************
|*************************************************************************
|
| MODULE: e_sun68k.s
| DESCRIPTION: Low level support routines for MTEX on 68K suns
| VERSION: 3.21
|
|*************************************************************************
|***copyright_s***********************************************************
|
|                          COPYRIGHT STATEMENT
|
|                         Copyright (c) 1991,92
|                           by Telenetworks
|                         Petaluma, CA   94952
|                            (707) 778-8737
|
| All rights reserved. Copying, compilation, modification, distribution
| or any other use whatsoever of this material is strictly prohibited
| except in accordance with a Software License Agreement with
| Telenetworks.
|
|***copyright_s***********************************************************
|***history***************************************************************
|***history***************************************************************
|*************************************************************************
|
| GENERAL NOTES
|
| This file contains low level routines for running MTEX on a
| Motorola 68K based sun system.
|
|*************************************************************************
|*************************************************************************


|*************************************************************************
| void _swstack (old, new, stack)
| unsigned *old;
| unsigned *new;
| unsigned *stack;
|

	.globl	__swstack
	
__swstack:

|	save registers on stack -
|	just save them all (D0-D7 and A0-A6)

	moveml	d0-d7/a0-a6,sp@-

|	 save current stack pointer in 'old'

	movl		sp@(64),a0
	movl		sp,a0@

|	 report the stack change to the scheduler.
	
	movl		sp@(72),a0
	movl		_forkx,a1
	movl		a0,a1@

|	 switch from 'new' to 'old' stack
	
	movl		sp@(68),a0   
	movl		a0@,sp

|	 restore the registers

	moveml   sp@+,d0-d7/a0-a6

	rts
   
| eof
