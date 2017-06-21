/* $Id: chat_debug.h,v 3.1.62.2 1996/09/13 17:27:51 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/chat_debug.h,v $
 *------------------------------------------------------------------
 * Header file for chat scripts debugging support
 *
 * May'94, Syed Irfan Ashraf
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * 
 *------------------------------------------------------------------
 * $Log: chat_debug.h,v $
 * Revision 3.1.62.2  1996/09/13  17:27:51  irfan
 * CSCdi69081:  chat_debug.c is missing log, endlog directives
 * Branch: California_branch
 * fix the headers.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

extern void chat_debug_command (parseinfo *);
extern void chat_debug_all (boolean);
extern void chat_debug_show (void);
extern void chat_debug_init(void);

/* Line number to restrict Chat debugging to */
extern int chatlineno;

/* Should we report every char received ? */
extern int chat_debug_granular;

/*
 * The actual debugging flags are defined in chat_debug_flags.h.
 * We include that file twice, once to define the flags themselves
 * and once to define the indices that the parser uses.
 */
#include "../ts/chat_debug_flags.h"
#define __DECLARE_DEBUG_NUMS__
#include "../ts/chat_debug_flags.h"
