/* $Id: ncia_debug.h,v 3.1.2.5 1996/07/26 06:23:59 schiang Exp $
 * $Source: /release/112/cvs/Xsys/cls/ncia_debug.h,v $
 *------------------------------------------------------------------
 * ncia_debug.h - Debugging definition for NCIA
 *
 * March 1996, Steve Chiang
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ncia_debug.h,v $
 * Revision 3.1.2.5  1996/07/26  06:23:59  schiang
 * CSCdi64000:  remove unused variable ncia_cisco_debug
 * Branch: California_branch
 *
 * Revision 3.1.2.4  1996/07/03  06:35:55  jolee
 * CSCdi61974:  Fix NCIA crash if unconfig a client in active open
 * processing
 * Branch: California_branch
 *
 * Revision 3.1.2.3  1996/06/18  19:01:42  schiang
 * CSCdi60706:  move extern in .c to .h
 * Branch: California_branch
 *
 * Revision 3.1.2.2  1996/06/12  06:51:49  schiang
 * CSCdi60121:  ncia keepalive cant be turned through configuration
 * clean up code so that no line exceeds 80 characters.
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/05/17  10:43:36  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.4.1  1996/04/05  06:05:45  schiang
 * Branch: IbuMod_Calif_branch
 * merge ncia server code
 *
 * Revision 3.1  1996/03/29  07:17:55  schiang
 * CSCdi53012:  add placeholder for NCIA files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef _NCIA_DEBUG_H_
#define _NCIA_DEBUG_H_

enum ncia_show_ {
    NCIA_SHOW_ALL,
    NCIA_SHOW_SERVER,
    NCIA_SHOW_CLIENT,
    NCIA_SHOW_CIRCUIT
};

enum ncia_debug_client_ {
    NCIA_DEBUG_CLIENT_ALL,
    NCIA_DEBUG_CLIENT_MSGS,
    NCIA_DEBUG_CLIENT_EVENT,
    NCIA_DEBUG_CLIENT_ERROR
};


enum ncia_circuit_debug_ {
    NCIA_DEBUG_CIRCUIT_ALL,
    NCIA_DEBUG_CIRCUIT_FLOW,
    NCIA_DEBUG_CIRCUIT_XID,
    NCIA_DEBUG_CIRCUIT_EVENT,
    NCIA_DEBUG_CIRCUIT_STATE,
    NCIA_DEBUG_CIRCUIT_ERROR
};


/*
 * NCIA clear command constants
 */
#define NCIA_CLEAR_CIRCUIT	1
#define NCIA_CLEAR_ALL_CIRCUITS	0
#define NCIA_CLEAR_CLIENT	2
#define NCIA_CLEAR_ALL_CLIENTS	0

/*
 * NCIA debug ncia command constants
 */

#define NCIA_DEBUG_ALL_CLIENTS   0
#define NCIA_DEBUG_ALL_CIRCUITS	0
#define NCIA_DEBUG_ALL_SERVERS	-1

#define NCIA_SHOW_ALL_CLIENTS   0
#define NCIA_SHOW_ALL_CIRCUITS	0
#define NCIA_SHOW_ALL_SERVERS	-1

#define NCIA_TYPE_STRING_SIZE 36

#define NciaCircuitDebugEvent		\
        if (ncia_circuit_debug & NciaCircuitDebugEVENT) buginf
#define NciaCircuitDebugFlow		\
        if (ncia_circuit_debug & NciaCircuitDebugFLOW) buginf
#define NciaCircuitDebugError		\
        if (ncia_circuit_debug & NciaCircuitDebugERROR) buginf
#define NciaCircuitDebugState		\
        if (ncia_circuit_debug & NciaCircuitDebugSTATE) buginf

#define NciaClientDebugEvent(client)    \
        if ((client)->NciaFlags & NciaClientDebugEVENT) buginf
#define NciaClientDebugMsgs(client)     \
        if ((client)->NciaFlags & NciaClientDebugMSGS) buginf
#define NciaClientDebugError(client)    \
        if ((client)->NciaFlags & NciaClientDebugERROR) buginf

#define NciaSetClientDebug(ncia_client, flag) (ncia_client->NciaFlags |= flag)
#define NciaUnsetClientDebug(ncia_client, flag) \
        (ncia_client->NciaFlags &= ~flag)
#define NciaClientDebugOn(client)       \
        ((client)->NciaFlags & NCIA_DEBUG_CLIENT_ALL)


/*
 * The actual debugging flags are defined in ncia_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../cls/ncia_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../cls/ncia_debug_flags.h"

#define NciaServerDebugAll	if (ncia_server_debug) buginf
#define NciaClientDebugAll	if (ncia_client_debug) buginf
#define NciaCircuitDebugAll	if (ncia_circuit_debug) buginf

extern void ncia_debug_init(void);
extern void debug_ncia(parseinfo *csb);
extern void show_ncia(parseinfo *csb);
extern void clear_ncia(parseinfo *csb);
extern void ncia_exec_command(parseinfo *csb);

#endif
