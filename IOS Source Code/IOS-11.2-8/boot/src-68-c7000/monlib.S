| $Id: monlib.S,v 3.1 1995/11/09 09:10:22 shaker Exp $
| $Source: /swtal/cherf.111/ios/boot/src-68-c7000/monlib.S,v $
|------------------------------------------------------------------
| monlib.S -- Assembler routine for calling MONLIB 
|
| Szewei Ju, January 1995
|
| Copyright (c) 1995 by cisco Systems, Inc.
| All rights reserved.
|------------------------------------------------------------------
| $Log: monlib.S,v $
| Revision 3.1  1995/11/09  09:10:22  shaker
| Bump version numbers from 2.x to 3.x.
|
| Revision 2.1  1995/06/07  19:17:09  hampton
| Bump version numbers from 1.x to 2.x.
|
| Revision 1.2  1995/06/07 09:05:14  smackie
| Fold back Arizona_branch into mainline.
|
| Revision 1.1.4.1  1995/05/02  23:22:35  sthormod
| Branch: Arizona_branch
| Rom monitor support for RP1 flash credit card
|
| Revision 1.1  1995/04/23  00:33:52  sthormod
| Placeholder for Arizona branch.
|
|------------------------------------------------------------------
| $Endlog$
|
	.text
	.even
	.globl _call_monlib

_call_monlib:
	link 	a6,#0
	movel	a6@(20),sp@-		| pass 4th argument - debug flag
        movel	a6@(16),sp@-            | pass 3rd argument - printf ptr
	movel	a6@(12),sp@-            | pass 2nd argument - ralib ptr
        movel	a6@(8),a0               | get entry point (1st argument)
	jbsr	a0@			| invoke monlib in RAM
	addqw	#8,sp			| recover stack spaces
	addqw	#4,sp			| recover stack spaces
	unlk 	a6
	rts
