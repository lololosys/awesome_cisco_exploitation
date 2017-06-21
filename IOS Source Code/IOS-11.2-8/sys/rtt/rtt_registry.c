/* $Id: rtt_registry.c,v 1.1.4.3 1996/07/31 21:41:23 lmetzger Exp $
 * $Source: /release/112/cvs/Xsys/rtt/rtt_registry.c,v $
 *------------------------------------------------------------------
 * RTT Registry initialization.
 *
 * March 1996, Larry Metzger 
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * This file contains the registry subsystem initialization for
 *  the Round Trip Time Monitor Applicaiton.
 *------------------------------------------------------------------
 * $Log: rtt_registry.c,v $
 * Revision 1.1.4.3  1996/07/31  21:41:23  lmetzger
 * CSCdi64744:  Move Registry Initialization to the RTT subsystem
 * Branch: California_branch
 *
 * Revision 1.1.4.2  1996/06/07  22:26:19  lmetzger
 * CSCdi59781:  Update to Coding Standards, No Code Changed, Just a
 * reformat
 * Branch: California_branch
 *
 * Revision 1.1.4.1  1996/05/17  11:41:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 1.1.2.1  1996/03/22  18:41:43  lmetzger
 * Initial Version of Response Time Reporter
 *
 * Revision 1.1  1996/03/20  17:34:31  lmetzger
 * Placeholder for IbuMod_Calif_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "interface_private.h"
#include "registry.h"
#include "../rtt/rtt_registry.h"
#include "../rtt/rtt_registry.regc"

/* 
 * These flags are needed by 
 *   the trace and debug macros
 *   located in each probe.  They
 *   need to be optimal (not in a 
 *   registry because they get checked
 *   often.  They are in this file
 *   because this registry is required in
 *   all builds, that have a probe 
 *   for a supported protocol, which IP
 *   is one of them.
 */
unsigned long RTT_ERROR_FLAG;
unsigned long RTT_TRACE_FLAG;

/*
 * rtt_registry_init
 *
 * Initialize RTT Registry
 */

static void rtt_registry_init (subsystype * subsys)
{
    registry_create(REG_RTT, SERVICE_RTT_MAX, "RTT");
    create_registry_rtt();
}	

/*
 * RTT Registry subsystem header
 */
#define RTT_REGISTRY_MAJVERSION 1
#define RTT_REGISTRY_MINVERSION 0
#define RTT_REGISTRY_EDITVERSION  1 

SUBSYS_HEADER (rtt_registry, 
	       RTT_REGISTRY_MAJVERSION, 
	       RTT_REGISTRY_MINVERSION,
	       RTT_REGISTRY_EDITVERSION,
	       rtt_registry_init, 
	       SUBSYS_CLASS_REGISTRY,
	       NULL, NULL);
