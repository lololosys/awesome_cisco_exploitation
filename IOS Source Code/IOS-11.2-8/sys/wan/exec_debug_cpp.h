/* $Id: exec_debug_cpp.h,v 3.1.2.1 1996/05/10 01:44:47 wmay Exp $
 * $Source: /release/112/cvs/Xsys/wan/exec_debug_cpp.h,v $
 *------------------------------------------------------------------
 * E X E C _ D E B U G _ P P P . H
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: exec_debug_cpp.h,v $
 * Revision 3.1.2.1  1996/05/10  01:44:47  wmay
 * CPP and PPP half-bridge commit
 * Branch: California_branch
 *
 * Revision 3.1  1996/05/09  17:24:39  wmay
 * Placeholder for California branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * debug cpp error
 */

EOLS    (debug_cpp_event_eol, debug_command, DEBUG_CPP_EVENT);
KEYWORD (debug_cpp_event, debug_cpp_event_eol, no_alt,
	 "event", "Protocol events", PRIV_OPR);

/*
 * debug cpp negotiation
 */

EOLS    (debug_cpp_negotiation_eol, debug_command, 
	 DEBUG_CPP_NEGOTIATION);
KEYWORD (debug_cpp_negotiation, debug_cpp_negotiation_eol, 
	 debug_cpp_event, "negotiation", 
	 "Protocol parameter negotiation", PRIV_OPR);

/*
 * debug cpp packet
 */

EOLS    (debug_cpp_packet_eol, debug_command, DEBUG_CPP_PACKET);
KEYWORD (debug_cpp_packet, debug_cpp_packet_eol, 
	 debug_cpp_negotiation, "packet", 
	 "Low-level cpp packet dump", PRIV_OPR);

/******************************************************************
 * debug cpp
 */

KEYWORD_DEBUG(debug_cpp, debug_cpp_packet, ALTERNATE,
	      OBJ(pdb,1), cpp_debug_arr,
	      "cpp", "Cpp information", PRIV_OPR);

#undef	ALTERNATE
#define	ALTERNATE	debug_cpp
