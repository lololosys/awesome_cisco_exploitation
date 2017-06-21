/* $Id: rtt_snanm_subsys_init.c,v 3.1.2.1 1996/08/16 18:32:36 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_snanm_subsys_init.c,v $
 *------------------------------------------------------------------
 * RTT Subsystem SNANM dependent initialization.
 *
 * August 1996, Larry Metzger
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the subsystem initialization for
 *  the Round Trip Time Monitor that is specific for snanm.
 *------------------------------------------------------------------
 * $Log: rtt_snanm_subsys_init.c,v $
 * Revision 3.1.2.1  1996/08/16  18:32:36  lmetzger
 * CSCdi66255:  Add Modularity of RTR for DSPU/SNANM
 * Branch: California_branch
 *
 * Revision 3.1  1996/08/16  15:26:57  lmetzger
 * Placeholder for modularizing RTT within SNANM/DSPU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "rtt_alert.h"

/*
 * rtt_snanm_subsystem_init
 *
 * Initialize RTT specific to SNANM
 */

static void rtt_snanm_subsystem_init (subsystype * subsys)
{
    if (registry_populated(REG_SNANM)) {
       reg_add_Rtt_Resolve(rtt_Resolution, "rtt_Resolution");
       reg_add_Rtt_Alert(rtt_Alert, "rtt_Alert");
    }

}	

/*
 * RTT SNANM subsystem header
 */
#define RTT_SNANM_SUBSYS_MAJVERSION 1
#define RTT_SNANM_SUBSYS_MINVERSION 0
#define RTT_SNANM_SUBSYS_EDITVERSION  1 

SUBSYS_HEADER (rtt_snanm_dependencies, 
	       RTT_SNANM_SUBSYS_MAJVERSION, 
	       RTT_SNANM_SUBSYS_MINVERSION,
	       RTT_SNANM_SUBSYS_EDITVERSION,
	       rtt_snanm_subsystem_init, 
	       SUBSYS_CLASS_MANAGEMENT,
	       NULL, NULL);

