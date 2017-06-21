/* $Id: chat_debug.c,v 3.1.62.4 1996/09/13 17:27:49 irfan Exp $
 * $Source: /release/112/cvs/Xsys/ts/chat_debug.c,v $
 *------------------------------------------------------------------
 * Chat scripts debugging support
 *
 * May'94, Syed Irfan Ashraf
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *
 *------------------------------------------------------------------
 * $Log: chat_debug.c,v $
 * Revision 3.1.62.4  1996/09/13  17:27:49  irfan
 * CSCdi69081:  chat_debug.c is missing log, endlog directives
 * Branch: California_branch
 * fix the headers.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "sys_registry.h"
#include "../ui/debug.h"
#include "chat_debug.h"
#include "ttysrv.h"
#include "config.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "chat_debug_flags.h"

/* "chatineno" is used to restrict the output of Chat debugging debugging */

int chatlineno = 0;

/* Ideally this flag should be configurable independantly */
/* For now we will make it follow chat_lineno */

int chat_debug_granular = 0;

/*
 * chat_debug_command
 * This is the routine the parser calls for Chat debug commands. It
 * handles the special case of a line number on "debug chat line",
 * and punts everything else to the generic handler.
 */

void chat_debug_command (parseinfo *csb)
{

    /*
     * When turning on chat script debugging for a line, make sure that
     * the line is a physical line (con or aux or tty)
     * Another way to check this is to compare input line number
     * against (nconlines+nauxlines+nttylines)
     */

    if (csb->sense  && GETOBJ(int,1) &&
	((NULL == MODEMS[GETOBJ(int,1)]) ||
	 (MODEMS[GETOBJ(int,1)]->type == CTY_LINE) ||
	 (MODEMS[GETOBJ(int,1)]->type == VTY_LINE) )){
	printf("\nCannot debug chat scripts on this line");
	return;
    }

    chatlineno = GETOBJ(int,1);

    (const) SETOBJ(pdb,1) = chat_debug_arr;

    /* Everything else is generic */
    debug_command(csb);

    if (csb->sense){
	if(chatlineno)
	    printf(" for line number %d", chatlineno);
    }
    else
	chatlineno = 0;

    chat_debug_granular = chatlineno;

}

/*
 * chat_debug_all is registered to be called whenever anybody issues
 * a "debug all" or "undebug all" command... or whenever you want to
 * set the state of all the chat debug flags at once. The argument is
 * TRUE for "debug all", FALSE for "undebug all".
 */

void chat_debug_all (boolean flag)
{

    chatlineno = chat_debug_granular = 0;

    generic_debug_all(chat_debug_arr, flag);
}

/*
 * chat_debug_show is called to display the values of all the chat
 * debugging variables.
 */

void chat_debug_show (void)
{
    debug_show_flags(&(chat_debug_arr[0]), "Chat Scripts");
    if (chat_debug && chatlineno)
	printf(" for line number %d", chatlineno);
}

/*
 * chat_debug_init is called at chat subsystem startup. It registers
 * the routines to maintain and display the chat debug flags, and
 * makes sure the flags are set to the appropriate values depending on
 * whether "debug all" is in effect when chat is started.
 */

void chat_debug_init (void)
{

    /* Register for "debug all" and "show debug" events. A Real Programmer
     * would be prepared to do something with the error code returned
     * from registry_add_list. Oh, well...
     */
    reg_add_debug_all(chat_debug_all, "chat_debug_all");
    reg_add_debug_show(chat_debug_show, "chat_debug_show");

    /* Make sure the debug flags are right at startup. If "debug all"
     * is in effect when chat is initialized, we want to turn on
     * all the chat debugging right now.
     */
    chat_debug_all(debug_all_p());
}
