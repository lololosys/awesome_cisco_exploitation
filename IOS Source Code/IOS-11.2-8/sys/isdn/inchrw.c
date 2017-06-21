/* $Id: inchrw.c,v 3.2.60.1 1996/04/30 23:07:21 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/inchrw.c,v $
 *------------------------------------------------------------------
 * inchrw.c
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: inchrw.c,v $
 * Revision 3.2.60.1  1996/04/30  23:07:21  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:40:47  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:43  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:34  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/* ==
.Vt,08000 INCHRW.C 28-Jan-92,19:52:24,`ELO' initial
.I 51
=== */
/************************************************************************
*************************************************************************
   Module: INCHRW.C
*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                         Copyright (c) 1991
                          by Telenetworks
                        Petaluma, CA   94952
                           (707) 778-8737

All rights reserved. Copying, compilation, modification, distribution
or any other use whatsoever of this material is strictly prohibited
except in accordance with a Software License Agreement with
Telenetworks.

****copyright_c**********************************************************
****history**************************************************************

****history**************************************************************
*************************************************************************


GENERAL NOTES
                                                                
char *INCHRW()                                                  
                                                                
inchrw read in one char from port 2                             
       use IFA * 2 to direct the input                          
                                                                

*************************************************************************
*************************************************************************
*/

#if 0
static char INCHRW()
{
char *input_port;
      input_port = (char *) 0x00000002;
      return(*input_port);
}
#endif
char INCHRW(void)
{
      return(0);
}
