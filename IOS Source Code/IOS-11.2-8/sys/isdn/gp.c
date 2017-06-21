/* $Id: gp.c,v 3.2.60.1 1996/04/30 23:07:19 dclare Exp $
 * $Source: /release/112/cvs/Xsys/isdn/gp.c,v $
 *------------------------------------------------------------------
 * gp.c  Global Parameters Module 
 *
 * March 1992, Ronnie Kon
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: gp.c,v $
 * Revision 3.2.60.1  1996/04/30  23:07:19  dclare
 * CSCdi56247:  fix ISDN code to use proper prototyping and not use DIRTY
 * or DUSTY.
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:40:43  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:13:39  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:15:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/************************************************************************
*************************************************************************

MODULE:       gp.c                                                    
DESCRIPTION:  Global Parameters Module                                
VERSION:      3.21                                                    

*************************************************************************
****copyright_c**********************************************************

                         COPYRIGHT STATEMENT

                        Copyright (c) 1991,92
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

*************************************************************************
*************************************************************************
*/


/************************************************************************
*                 E X T E R N A L   R E F E R E N C E S                 *
*************************************************************************
*/

#include "master.h"
#include "address.h"

#include "flags.h"
/*===*/
#include "tune.h"
#include "gendef.h"
#include "gp.h"
#include "lif.h"	/*===*/
#include "ccbri.h"	/*===*/
/* """
#include <stdio.h>
""" */


/************************************************************************
*                           C O N S T A N T S                           *
*************************************************************************
*/


/************************************************************************
*               D A T A   T Y P E   D E F I N I T I O N S               *
*************************************************************************
*/


/************************************************************************
*                  D A T A   D E C L A R A T I O N S                    *
*************************************************************************
*/

/* parameters available to all tasks
*/
PUBLIC NUMBER_PARAM np;                         /* phone-number params */
PUBLIC EPID epid;                                       /* endpoint id */
PUBLIC NUMBER_PARAM_FIT np_fit[MAX_DSL] [MAX_HOSTS];	/*===*/

/************************************************************************
*                     S T U B   F U N C T I O N S                       *
*************************************************************************
*/

/***********
* gpGetLdn *
************
*
*/
PUBLIC BYTE * gpGetLdn()

{
   BYTE *ldn;

   if (np.ldn[0] == '0' && np.ldn[1] == '\0')
      ldn = NULL;
   else
      ldn = (BYTE *) np.ldn;

   return ldn;
}


/************
* gpGetEpid *
*************
*
*/
PUBLIC BOOLEAN gpGetEpid(usid, tid, epsf)
int *usid;
int *tid;
int *epsf;

{
   if (epid.valid)
      {
      *usid = epid.usid;
      *tid = epid.tid;
      *epsf = epid.epsf;
      }

   return epid.valid;
}

/*=== following routine added */
/****************
* gpBuildHostCb *
*****************
*
*/
PUBLIC void gpBuildHostCb (spid, dn, dn_routing, bearer_type, bearer_routing, 
                           host_id, ces, initializing_terminal, dsl, host_spec)

  char    *spid;
  char    *dn;
  BOOLEAN *dn_routing;
  int     *bearer_type;
  BOOLEAN *bearer_routing;
  INT16   *host_id;
  int     *ces;
  BOOLEAN *initializing_terminal;
  int     dsl;
  int     host_spec;
  
{
  strcpy(spid ,np_fit[dsl][host_spec].spid);
  strcpy(dn, np_fit[dsl][host_spec].ldn);
  *dn_routing  =         np_fit[dsl][host_spec].DN_Routing;
  *bearer_type =         np_fit[dsl][host_spec].Bearer_Type; 
  *bearer_routing =      np_fit[dsl][host_spec].Bearer_Routing;
  *host_id        =      np_fit[dsl][host_spec].Host_ID;
  *ces            =      np_fit[dsl][host_spec].Ces; 
  *initializing_terminal = np_fit[dsl][host_spec].Initializing_Terminal; 
  return;

}
