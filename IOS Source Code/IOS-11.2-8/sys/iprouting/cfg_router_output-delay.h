/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * C F G _ R O U T E R _ O U T P U T - D E L A Y . H
 *
 * router RIP inter-frame gap subcommand
 *
 * Aug 1996, Dawn Li
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */


/*****************************************************************
 * output-delay <delay>
 * no output-delay [<delay>]
 */

PARAMS_KEYONLY(router_outdelay_kw, NONE,
	       "output-delay", OBJ(int,1), MINOUTDELAY, MAXOUTDELAY,
	       outdelay_command, 0,
	       "Interpacket delay for RIP updates",
	       "Delay in milliseconds", PRIV_CONF);

ASSERT	(router_outdelay, router_outdelay_kw, ALTERNATE,
	 ((pdbtype *) csb->protocol)->proctype & PROC_RIP);

#undef	ALTERNATE
#define ALTERNATE	router_outdelay
