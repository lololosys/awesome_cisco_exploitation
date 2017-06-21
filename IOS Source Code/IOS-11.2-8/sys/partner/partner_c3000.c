/* $Id: partner_c3000.c,v 3.1.4.1 1996/03/18 21:34:04 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/partner/partner_c3000.c,v $
 *------------------------------------------------------------------
 * partner_c3000.c -- c3000 platform specific subsystem initialization code
 *
 * Dec 1995, Li Jiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: partner_c3000.c,v $
 * Revision 3.1.4.1  1996/03/18  21:34:04  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.6.1  1996/03/16  07:23:24  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1  1996/03/09  00:12:19  ljiang
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
#include "partner_c3000.h"

void partner_subsys_init(subsystype *subsys)
{
     if (IS_VENDOR(VENDOR_CHIPCOM) && IS_CPU(CPU_CANCUN))
        chipcom_subsys_init(subsys);
     else if(IS_VENDOR(VENDOR_MICROCOM) && IS_CPU(CPU_CANCUN))
        microcom_subsys_init(subsys);
}

