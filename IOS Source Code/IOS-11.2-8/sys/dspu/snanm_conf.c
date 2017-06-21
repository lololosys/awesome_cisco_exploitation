/* $Id: snanm_conf.c,v 3.2.60.1 1996/03/18 19:34:13 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/dspu/snanm_conf.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Configuration 
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm_conf.c,v $
 * Revision 3.2.60.1  1996/03/18  19:34:13  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:11  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:48:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:06:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:29:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "snanm_conf.h"

/*=====================================================================*
*  parser interface
*======================================================================*
*
* Description:
*   For Network management, we had configuration interface:
*     sna endnode netId cp name
*     location location_string
*/


void Fp_endNode (char* netId, char* cpName)
{
    printf("sna endnode %s %s not implemented yet\n", netId, cpName);
}

void Fp_showEndNode (void)
{
}

void Fp_NoEndNode (void)
{
}

#define MaxLocation 50
static char snaLocation[MaxLocation+2]= {0};

void Fp_location (char* location)
{
   bcopy(location, snaLocation, MaxLocation);
}

void Fp_NoLocation (void)
{
   snaLocation[0] = 0;
}

void Fp_showLocation (void)
{
  nv_write(snaLocation[0] != 0,"location %s", snaLocation);
}

const char* GetFpLocation (void)
{
   return snaLocation;
}

