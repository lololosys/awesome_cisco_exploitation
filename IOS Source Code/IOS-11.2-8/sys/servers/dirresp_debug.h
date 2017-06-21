/* $Id: dirresp_debug.h,v 3.1 1996/02/20 22:46:01 pst Exp $
 * $Source: /release/111/cvs/Xsys/servers/dirresp_debug.h,v $
 *------------------------------------------------------------------
 * dirresp_debug.h -- Debugging declarations for director-responder
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by Cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dirresp_debug.h,v $
 * Revision 3.1  1996/02/20  22:46:01  pst
 * Initial incorporation of distributed directory agent support
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/* Function to fire up debugging */
extern void dirresp_debug_init(void);

/*
 * The actual debugging flags are defined in the ..._debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../servers/dirresp_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../servers/dirresp_debug_flags.h"
