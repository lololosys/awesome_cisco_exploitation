/* $Id: snanm_parse.c,v 3.4.20.2 1996/05/17 10:49:32 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/dspu/snanm_parse.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Parser command
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm_parse.c,v $
 * Revision 3.4.20.2  1996/05/17  10:49:32  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/04/26  07:35:28  dstine
 * Branch: IbuMod_Calif_branch
 * - commit of IBU modularity work, phase 1.
 *
 * Revision 3.4.6.1  1996/04/03  14:03:55  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.20.1  1996/03/18  19:34:14  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  08:46:12  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  00:47:34  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/01  06:02:04  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.3  1996/01/29  07:27:25  hampton
 * Remove unnecessary includes of parser definition files.  [CSCdi47717]
 *
 * Revision 3.2  1995/11/17  09:06:15  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:30:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ciscolib.h>
#include "interface_generic.h"
#include "config.h"
#include "parser.h"
#include "parser_defs_dspu.h"
#include "snanm_conf.h"
#include "dspu.h"



void dspu_command (parseinfo *csb);
void dspuif_command (parseinfo *csb);
void dspu_show_command (parseinfo *csb);
void dspu_debug_command (parseinfo *csb);

void sna_command (parseinfo *csb)
{
    switch (csb->which) {
       case SNA_ENDNODE:
        if (csb->nvgen)
           Fp_showEndNode();
        else if (csb->sense)
           Fp_endNode(GETOBJ(string, 1), GETOBJ(string, 2));
        else
           Fp_NoEndNode();
        break;
      
      case SNA_LOCATION:
        if (csb->nvgen)
           Fp_showLocation();
        else if (csb->sense)
           Fp_location(GETOBJ(string, 1));
        else
           Fp_NoLocation();
        break;

      default:
         dspu_command(csb);
   }
}
void snaif_command (parseinfo *csb)
{
    switch (csb->which) {
       case LAN_NAME:
         if (csb->nvgen){
            char  lanName[9];
            lanName[8] = 0;     /* ensure null termination */
            bcopy(csb->interface->lanName, lanName, 8);  /* copy 8 bytes of lan name */
            nv_write(lanName[0] != 0, "lan-name %s",lanName);
         }
         else if (csb->sense)
            bcopy(GETOBJ(string, 1), csb->interface->lanName, 8);
         else
            csb->interface->lanName[0] = 0;
            
         
         break;

       default:
          dspuif_command(csb);
  }
}

void snanm_show_command (parseinfo *csb)
{
    SetSnaMode(TRUE);
    dspu_show_command(csb);
    SetSnaMode(FALSE);
}

void sna_debug_command (parseinfo *csb)
{
   SetSnaMode(TRUE);
   dspu_debug_command(csb);
   SetSnaMode(FALSE);
}

