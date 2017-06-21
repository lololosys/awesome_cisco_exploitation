/* $Id: tbridge_registry.c,v 3.1.8.1 1996/05/04 05:09:15 wilber Exp $
 * $Source: /release/112/cvs/Xsys/tbridge/tbridge_registry.c,v $
 *------------------------------------------------------------------
 * tbridge/tbridge_registry.c
 *
 * April 1996, Tony Speakman
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Transparent bridging registry
 *------------------------------------------------------------------
 * $Log: tbridge_registry.c,v $
 * Revision 3.1.8.1  1996/05/04  05:09:15  wilber
 * Branch: California_branch
 *
 * Integrated Routing and Bridging
 * - New file(tbridge_bvi.c) for Bridge-group Virtual Interface
 * functions.
 * - Obsolete tbridge_crb_inline.h with tbridge_irb_inline.h.
 * - Bridge/route decision(tbridge_irb_inline.h).
 * - Learn routable packets(DLLI_IRBPKT) in bridge table.
 * - Move locate_bte_inline() and tbridge_get_arpa_type_inline() to
 *   tbridge_common_inline.h.
 * - Rip out MTR transparent bridging support.
 *
 * Constrained Multicast Flooding
 * - Add the tbridge monitor.
 * - Add the tbridge registry.
 * - Add the entire CMF subsystem.
 * - Convert tbridge flood array to a linked list of tbifd's.
 * - Add the call outs to CMF from the LES, RSP, and PAS forwarding
 *   functions.
 *
 * Revision 3.1  1996/04/21  23:06:49  speakman
 * Placeholders for the CMF subsystem and eventual partition of the
 * bridging registries.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "address.h"
#include "interface_private.h"
#include "logger.h"

#include "subsys.h"

#include "../srt/span.h"

#include "../tbridge/tbridge_public.h"
#include "../tbridge/tbridge_monitor.h"
#include "../tbridge/tbridge_registry.h"
#include "../tbridge/tbridge_registry.regc"

/*
 * tbridge_registry_init
 *
 * Initialize TBRIDGE Registry
 */

static void tbridge_registry_init (subsystype *subsys)
{
    registry_create(REG_TBRIDGE, SERVICE_TBRIDGE_MAX, "TBRIDGE");
    create_registry_tbridge();
}

/*
 * TBRIDGE Registry subsystem header
 */

#define TBRIDGE_REGISTRY_MAJVERSION 1
#define TBRIDGE_REGISTRY_MINVERSION 0
#define TBRIDGE_REGISTRY_EDITVERSION  1

SUBSYS_HEADER(tbridge_registry,
              TBRIDGE_REGISTRY_MAJVERSION, TBRIDGE_REGISTRY_MINVERSION,
              TBRIDGE_REGISTRY_EDITVERSION,
              tbridge_registry_init, SUBSYS_CLASS_REGISTRY,
              NULL, NULL);
