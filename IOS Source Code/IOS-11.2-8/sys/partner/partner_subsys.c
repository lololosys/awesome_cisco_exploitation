/* $Id: partner_subsys.c,v 3.1.4.1 1996/07/06 05:55:54 thille Exp $
 * $Source: /release/112/cvs/Xsys/partner/partner_subsys.c,v $
 *------------------------------------------------------------------
 * partner_subsys.c -- Partners Engineering Subsystem
 *
 * Dec 1995, Li Jiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * The following file contains partner subsystem initialization code
 *------------------------------------------------------------------
 * $Log: partner_subsys.c,v $
 * Revision 3.1.4.1  1996/07/06  05:55:54  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.1  1996/03/09  00:12:22  ljiang
 * CSCdi50986:  Add mailbox support for  Microcom Terminator(c2511
 * derivative)
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "../ui/common_strings.h"
#include "subsys.h"
#include "../les/if_les.h"
#include "partner_subsys.h"

#define PARTNER_MAJVERSION      1
#define PARTNER_MINVERSION      0
#define PARTNER_EDITVERSION     1

SUBSYS_HEADER(partner, PARTNER_MAJVERSION, PARTNER_MINVERSION,
              PARTNER_EDITVERSION, partner_subsys_init,
              SUBSYS_CLASS_PROTOCOL, subsys_seq_iphost, NULL);

