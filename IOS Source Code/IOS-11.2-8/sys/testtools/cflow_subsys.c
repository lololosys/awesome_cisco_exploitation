/* $Id: cflow_subsys.c,v 3.3.20.1 1996/07/06 05:57:18 thille Exp $
 * $Source: /release/112/cvs/Xsys/testtools/cflow_subsys.c,v $
 * ------------------------------------------------------------------
 * Declare cflow subsystem and perform initialization 
 * 
 * June-9 1994, Lakshmankumar Mukkavilli
 * 
 * Copyright (c) 1993-1997 by cisco Systems, Inc. All rights reserved.
 * 
 * ------------------------------------------------------------------
 * $Log: cflow_subsys.c,v $
 * Revision 3.3.20.1  1996/07/06  05:57:18  thille
 * CSCdi62136:  seq: iphost and req: iphost duplicated and waste memory
 * Branch: California_branch
 * Put copies of the strings in common_strings.c/h and reference them
 * instead of having multiple copies of the literals.  Save 660 bytes.
 *
 * Revision 3.3  1996/01/31  18:24:09  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * Revision 3.2  1995/11/17  18:50:30  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:01  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:07:22  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "interface_private.h"
#include "subsys.h"
#include "../testtools/cflow.h"

/*
 * cflow subsystem initialization 
 */

void cflow_init (subsystype *subsys)
{
    cflow_parser_init();
    ctrace_parser_init();
}


/*
 * cflow subsystem header
 */

#define CFLOW_MAJVERSION 1
#define CFLOW_MINVERSION 0
#define CFLOW_EDITVERSION  1

SUBSYS_HEADER(cflow, CFLOW_MAJVERSION, CFLOW_MINVERSION, CFLOW_EDITVERSION,
	      cflow_init, SUBSYS_CLASS_LIBRARY,
	      "req: tftp",
	      NULL);


