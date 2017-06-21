/* $Id: pgen.c,v 3.2.20.1 1996/03/18 21:35:16 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/pgen/pgen.c,v $
 *------------------------------------------------------------------
 * pgen.c -- Packet generator 
 *
 * February 1995, Biren Fondekar
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pgen.c,v $
 * Revision 3.2.20.1  1996/03/18  21:35:16  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.28.2  1996/03/07  10:41:02  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.1.28.1  1996/02/20  16:58:19  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1996/01/25  11:21:12  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.1  1995/11/09  12:59:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/09/14  01:52:04  biren
 * CSCdi40312:  Move pakgen and pgen (Pagent 2.2) to seperate subsystems
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <ctype.h>
#include <stdarg.h>
#include "config.h"
#include "file.h"
#include "interface_private.h"
#include "subsys.h"
#include "packet.h"
#include "ttysrv.h"
#include "parser.h"
#include "capture_private.h"
#include "pgen_private.h"
#include "pgen_registry.h"
#include "../pgen/pgen_registry.regc"


/*
 * Definitions
 */
boolean pgen_ok = FALSE;
long pgen_regcode;                      /* pgen service registration
                                           code */

/*
 * Pgen subsystem header
 */

#define PGEN_MAJVERSION 1
#define PGEN_MINVERSION 0
#define PGEN_EDITVERSION  1

SUBSYS_HEADER(pgen, PGEN_MAJVERSION, PGEN_MINVERSION, PGEN_EDITVERSION,
	      pgen_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL);


/*
 * pgen_init
 */

void pgen_init (subsystype *subsys)
{
    if (!pgen_ok)
	pgen_ok = TRUE;
    pgen_parser_init();
    capture_parser_init();
    /* Create the Pagent registry */
    pgen_regcode = registry_create(REG_UNKNOWN, SERVICE_PGEN_MAX, "Pagent");
    if (pgen_regcode != REG_ILLEGAL) {
	create_registry_pgen();
    
	/* Register the default services */
	reg_add_pgen_send(pgen_send, "pgen_send");
	reg_add_pgen_count(pgen_count, "pgen_count");
    }
}

