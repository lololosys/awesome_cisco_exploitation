/* $Id: snanm_subsys_init.c,v 3.2 1995/11/17 09:06:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dspu/snanm_subsys_init.c,v $
 *------------------------------------------------------------------
 * SNA Network Management Initialization sub system
 *
 * Jan 95, Barry Chan
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: snanm_subsys_init.c,v $
 * Revision 3.2  1995/11/17  09:06:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:30:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/09  13:16:19  ppearce
 * CSCdi38432:  dspu subsystem causing confusion, rename it
 *   Change dspu subsystem name in subsytems that require dspu
 *
 * Revision 2.1  1995/06/07  20:30:47  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "dspu_debug.h"

extern void snanm_init(void);

/*
 * snanm_subsys_init()
 *   Initialization point for the SNANM subsystem
 *
 */

static void snanm_subsys_init (subsystype *subsys)
{
    snanm_init();
}


/*
 * SNANM subsystem header
 */

#define SNANM_MAJVERSION 1
#define SNANM_MINVERSION 0
#define SNANM_EDITVERSION  1

SUBSYS_HEADER(snanm, SNANM_MAJVERSION, SNANM_MINVERSION, SNANM_EDITVERSION,
	      snanm_subsys_init, SUBSYS_CLASS_PROTOCOL,
	      "seq: sna_pu",
	      "req: sna_pu");

