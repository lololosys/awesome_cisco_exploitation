/* $Id: tcl_init.c,v 3.4.4.1 1996/03/18 22:13:03 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/tcl/tcl_init.c,v $
 *------------------------------------------------------------------
 * tcl_init.c -- To make tcl a subsystem.
 * April 1994, Satish Gannu
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tcl_init.c,v $
 * Revision 3.4.4.1  1996/03/18  22:13:03  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.20.3  1996/03/16  07:40:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.3.20.2  1996/03/07  10:52:36  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.20.1  1996/02/20  21:20:09  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/03/09  22:57:32  widmer
 * CSCdi47890:  HTTP server doesnt work
 * Add HTTP server
 *
 * Revision 3.3  1995/12/01  23:14:11  widmer
 * CSCdi44724:  Remove nv_top argument to parser_add_mode()
 * Forgot a call to parser_add_mode()
 *
 * Revision 3.2  1995/11/17  18:47:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:34:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/25  00:47:18  biren
 * Move the TCL-based LANE/ATM signalling test tool to a seperate subsytem
 * (atmtest).
 *
 * CSCdi39304:  Move TCL-based LANE/ATM test tool to separate subsystem
 *
 * Revision 2.2  1995/07/12  18:59:09  cparrott
 * CSCdi35565:  TCL image doesnt compile.
 *
 * Revision 2.1  1995/06/07  23:04:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <stdarg.h>
#include "sys_registry.h"
#include "subsys.h"
#include "ttysrv.h"
#include "config.h"
#include "parser.h"
#include "../xns/xnstypes.h"
#include "../tcl/tcl.h"
#include "../tcl/tcl_init.h"
#include "../tcl/tcl_new_commands.h"

/*
 * tcl subsystem header
 */
#define TCL_MAJVERSION 1
#define TCL_MINVERSION 0
#define TCL_EDITVERSION  1

SUBSYS_HEADER(tcl, TCL_MAJVERSION, TCL_MINVERSION, TCL_EDITVERSION,
	      tcl_init, SUBSYS_CLASS_LIBRARY,
	      NULL,NULL);

static Tcl_DString command;     /* Used to buffer incomplete commands being
				 * read from stdin. */

/*
 * The Tcl interpreter
 */
Tcl_Interp *interp = NULL;

/*
 * tcl_init
 * Define the subsystem main function. Add some registrys.
 */
void tcl_init (subsystype *subsys)
{
    /*
     * Add all the registries involved with this subsystem.
     */
    reg_add_parse_cli_cmd(PARSER_CLI_TCL, tcl_parse_cmd, "tcl_parse_cmd");
    reg_add_parse_cli_quit(tcl_quit, "tcl_quit");
    parser_add_mode("tcl", "tcl", "Tcl mode",
		    TRUE, TRUE, NULL, NULL, NULL,
		    get_mode_top(exec_mode), NULL);
    
    /*
     * Initialize the parse to add the tclsh and tclquit command.
     */
    tcl_parser_init();
}

/*
 * tcl_putc
 * This procedure caputures all the characters and puts in a buffer.
 */
void tcl_putc (tt_soc *tty, int c)
{
    char ch[2];
    
    ch[0] = c;
    ch[1] = 0;
    Tcl_DStringAppend(&interp->Tcl_input, ch, 1);
    
}

/*
 * tclsh_command
 * This implements the parser's tcl command and sets things up for
 * the tcl interpreter.
 */
void tclsh_command (parseinfo *csb)
{

    /*
     * check if tcl shell is not invoked 
     */
    if (stdio->parse_cli_type != PARSER_CLI_TCL) {
	
	/*
	 * Initialize the tclbuf_vector and let the rest of the values
	 * be default as we are only interested in the putc and will not
	 * be doing any output, so initialize vputc to tcl_putc which is
	 * used to buffer the output.
	 */
	vector_init(&tclbuf_vector);
	tclbuf_vector.vputc = tcl_putc;
	
	/*
	 * tcl is the current shell.
	 * Change the prompt to tcl.
	 * Set the  break_chars to include .
	 */
	stdio->parse_cli_type = PARSER_CLI_TCL;
	csb->break_chars = TCL_BREAKSET;
	set_mode_byname(&csb->mode, "tcl", MODE_VERBOSE);
	
	/*
	 * If a Tcl interpreter does not already exist, create
	 * and initialize one.
	 */
	if (interp == NULL)
	   interp = Tcl_CreateInterp();
	
	Tcl_DStringInit(&interp->Tcl_input);


	/*
	 * Add New commands into tcl here.
	 */
	Tcl_CreateCommand(interp, "loadscripts", Tcl_LoadScriptsCmd, 
			  (ClientData) NULL, NULL);

	interp->csb = csb;
	Tcl_SetVar(interp, "tcl_interactive", "1", TCL_GLOBAL_ONLY);
	
	/*
	 * Init the tcl string type.
	 */
	Tcl_DStringInit(&command);
    }
}

/*
 * tcl_quit
 * This implements the parser's tclquit command. tcl shell is reset
 * using this routine and normal parsing is resumed.
 */
void tcl_quit (void)
{
    /*
     * if not in tcl shell do nothing. 
     */
    if (stdio->parse_cli_type !=  PARSER_CLI_DEFAULT) {
	stdio->parse_cli_type = PARSER_CLI_DEFAULT;
	
	interp->csb = NULL;
	/* 
	 * Free the input buffer in tcl and the Tcl_input 
	 */
	Tcl_DStringFree(&interp->Tcl_input);

	Tcl_DeleteInterp(interp);
    }
}

/*
 * tcl_parse_cmd
 * This is routine is executed after tcl becomes the shell.
 */
boolean  tcl_parse_cmd (parseinfo *csb)
{
    
    char *cmd;
    int tcl_result;
    
    /* Append the inputed line on to the tcl buffer and
     * append it with the \n because it is removed above and
     * tcl requires it as a delimiter.
     * Invoke the tcl interpreter which inside it calls
     * parse_cmd. The return value of the parse_cmd is
     * interp_>parse_cmd_error.
     */
    
    if (csb->lastchar == CH_SUB) {
	csb->line_buffer[0] = csb->lastchar;
	csb->line_buffer[1] = 0;
    }
    cmd = Tcl_DStringAppend(&command, csb->line, -1);
    cmd = Tcl_DStringAppend(&command, "\n", -1);
    
    if (!Tcl_CommandComplete(cmd)) {
	return PARSE_TCL_INCOMPLETE_COMMAND;
    } else {
	interp->parse_cmd_error = 0;
	tcl_result = Tcl_RecordAndEval(interp, cmd, 0 );
	
	/* 
	 * Print the error message of the tcl command evaluated 
	 */
	if ((tcl_result != TCL_OK) && (*interp->result != 0 )) {
	    printf("%s\n",interp->result);
	}
	if (stdio->parse_cli_type == PARSER_CLI_TCL) {
	    
	    /* 
	     * Free the input buffer in tcl and the Tcl_input 
	     */
	    Tcl_DStringFree(&command);
	    Tcl_DStringFree(&interp->Tcl_input);
	}
	return interp->parse_cmd_error;
    }
}

/*
 * tclquit_command
 * This routine calls the tcl_quit. It is done this way because
 * a parser command default passes csb.
 */
void tclquit_command (parseinfo *csb)
{
    csb->mode = exec_mode;
    tcl_quit();
}
