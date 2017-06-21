/* $Id: rtt_dspu_subsys_init.c,v 3.1.2.1 1996/08/16 18:32:34 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/dspu/rtt_dspu_subsys_init.c,v $
 *------------------------------------------------------------------
 *  RTT Subsystem DSPU dependent initialization.
 *
 * August 1996, Larry Metzger
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the subsystem initialization for
 *  the Round Trip Time Monitor that is specific for dspu.
 *------------------------------------------------------------------
 * $Log: rtt_dspu_subsys_init.c,v $
 * Revision 3.1.2.1  1996/08/16  18:32:34  lmetzger
 * CSCdi66255:  Add Modularity of RTR for DSPU/SNANM
 * Branch: California_branch
 *
 * Revision 3.1  1996/08/16  15:27:18  lmetzger
 * Placeholder for modularizing RTT within SNANM/DSPU
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "../dspu/rtt_dependluecho.h"
#include "../dspu/rtt_sscpruecho.h"

/*
 * rtt_dspu_subsystem_init
 *
 * Initialize RTT specific to DSPU
 */

static void rtt_dspu_subsystem_init (subsystype * subsys)
{
    if (registry_populated(REG_RTT)) {
	reg_add_rtt_echo_exists(D_rttMonEchoAdminProtocol_snaLU0EchoAppl,
				rtt_lu0_echo_exists,
				"rtt_lu0_echo_exists");
	reg_add_rtt_echo_validate(D_rttMonEchoAdminProtocol_snaLU0EchoAppl,
				  rtt_lu0_echo_validate,
				  "rtt_lu0_echo_validate");
	reg_add_rtt_echo_activate(D_rttMonEchoAdminProtocol_snaLU0EchoAppl,
				  rtt_lu0_echo_activate,
				  "rtt_lu0_echo_activate");
	reg_add_rtt_echo_exists(D_rttMonEchoAdminProtocol_snaLU2EchoAppl,
				rtt_lu2_echo_exists,
				"rtt_lu2_echo_exists");
	reg_add_rtt_echo_validate(D_rttMonEchoAdminProtocol_snaLU2EchoAppl,
				  rtt_lu2_echo_validate,
				  "rtt_lu2_echo_validate");
	reg_add_rtt_echo_activate(D_rttMonEchoAdminProtocol_snaLU2EchoAppl,
				  rtt_lu2_echo_activate,
				  "rtt_lu2_echo_activate");
	reg_add_rtt_echo_exists(D_rttMonEchoAdminProtocol_snaRUEcho,
				rtt_sscp_ru_echo_exists,
				"rtt_sscp_ru_echo_exists");
	reg_add_rtt_echo_validate(D_rttMonEchoAdminProtocol_snaRUEcho,
				  rtt_sscp_ru_echo_validate,
				  "rtt_sscp_ru_echo_validate");
	reg_add_rtt_echo_activate(D_rttMonEchoAdminProtocol_snaRUEcho,
				  rtt_sscp_ru_echo_activate,
				  "rtt_sscp_ru_echo_activate");
	reg_add_rtt_depend_lu_split_address(rtt_depend_lu_split_address,
					    "rtt_depend_lu_split_address");
    }

}	

/*
 * RTT DSPU subsystem header
 */
#define RTT_DSPU_SUBSYS_MAJVERSION 1
#define RTT_DSPU_SUBSYS_MINVERSION 0
#define RTT_DSPU_SUBSYS_EDITVERSION  1 

SUBSYS_HEADER (rtt_dspu_dependencies, 
	       RTT_DSPU_SUBSYS_MAJVERSION, 
	       RTT_DSPU_SUBSYS_MINVERSION,
	       RTT_DSPU_SUBSYS_EDITVERSION,
	       rtt_dspu_subsystem_init, 
	       SUBSYS_CLASS_MANAGEMENT,
	       NULL, NULL);
