/* $Id: outchr.c,v 3.2.60.1 1996/04/30 23:08:02 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/outchr.c,v $
 *------------------------------------------------------------------
 * outchr.c
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: outchr.c,v $
 * Revision 3.2.60.1  1996/04/30  23:08:02  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:42:05  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:14:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:17:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/* ===
.Vt,08000 OUTCHR.C 11-Nov-91,15:12:54,`ELO' initial
.I 29
=== */
/********************************************************************
*                                                                  *
*  char *OUTCHR(c)                                                 *
*                                                                  *
*   outchr output one char to port 3                               *
*          use OFA * 2 to direct the output                        *
*                                                                  *
********************************************************************/
void _OUTCHR(char);

#if 0
_OUTCHR(c)
char c;
{
char *output_port;
      output_port = (char *) 0x00000003;
     *output_port = c;
}
#endif

char OutBuf[1024];
int OutPtr = 0;

void _OUTCHR (c)
char c;
{
    OutBuf[OutPtr] = c;
    OutPtr++;
    if (OutPtr >= 1024)
        OutPtr = 0;
}
