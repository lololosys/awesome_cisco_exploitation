/* $Id: partner_c4000.c,v 3.2.4.1 1996/03/18 21:34:05 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/partner/partner_c4000.c,v $
 *------------------------------------------------------------------
 * partner_c4000.c -- c4000 platform specific subsystem initialization code
 *
 * Dec 1995, Li Jiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: partner_c4000.c,v $
 * Revision 3.2.4.1  1996/03/18  21:34:05  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.6.1  1996/03/16  07:23:25  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2  1996/03/09  00:12:21  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 * Revision 3.1  1996/03/09  00:06:01  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../../boot/cpu.h"
#include "subsys.h"
#include "../les/if_les.h"
#include "partner_c4000.h"

void partner_subsys_init(subsystype *subsys)
{
     if (IS_VENDOR(VENDOR_CHIPCOM) && IS_CPU(CPU_SIERRA))
        chipcom_subsys_init(subsys);
}

