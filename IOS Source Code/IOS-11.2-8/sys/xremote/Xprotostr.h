/* $Id: Xprotostr.h,v 3.2 1995/11/17 19:26:22 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xremote/Xprotostr.h,v $
 *------------------------------------------------------------------
 * $Log: Xprotostr.h,v $
 * Revision 3.2  1995/11/17  19:26:22  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  14:01:23  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:31:39  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* $XConsortium: Xprotostr.h,v 1.3 88/09/06 15:56:06 jim Exp $ */
#ifndef XPROTOSTRUCTS_H
#define XPROTOSTRUCTS_H

/***********************************************************
Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
#include "Xmd.h"

/* Used by PolySegment */

typedef struct _xSegment {
    INT16 x1 B16, y1 B16, x2 B16, y2 B16;
} xSegment;

/* POINT */

typedef struct _xPoint {
	INT16		x B16, y B16;
} xPoint;

typedef struct _xRectangle {
    INT16 x B16, y B16;
    CARD16  width B16, height B16;
} xRectangle;

/*  ARC  */

typedef struct _xArc {
    INT16 x B16, y B16;
    CARD16   width B16, height B16;
    INT16   angle1 B16, angle2 B16;
} xArc;

#endif /* XPROTOSTRUCTS_H */
