/* $Id: mlpvt.c,v 1.1.42.2 1996/05/06 18:08:56 syiu Exp $
 * $Source: /release/112/cvs/Xsys/mlpvt/mlpvt.c,v $
 *------------------------------------------------------------------
 * mlpvt.c -- Multilink PPP virtual termination (MLPVT) subsystem
 *
 * January 1996, Andrew G. Harvey
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: mlpvt.c,v $
 * Revision 1.1.42.2  1996/05/06  18:08:56  syiu
 * CSCdi56520:  Show ppp multilink not showing member links
 * Branch: California_branch
 *
 * Revision 1.1.42.1  1996/04/27  06:34:19  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 1.1.2.5  1996/03/09  07:18:05  syiu
 * Branch: Dial1_branch
 * Move virtual-template from mlpvt to mlp
 *
 * Revision 1.1.2.4  1996/03/05  02:46:52  agh
 * Branch: Dial1_branch
 * Much cleanup of unwanted code, also added comments.
 *
 * Revision 1.1.2.3  1996/02/08  23:47:14  syiu
 * Branch: Dial1_branch
 * Support for CHAP challenge by stackname if locally defined
 *
 * Revision 1.1.2.2  1996/02/08  06:51:40  syiu
 * Branch: Dial1_branch
 * Tie together MLPVT/MLP/VPN registries, add vtemplate
 * support in MLPVT
 *
 * Revision 1.1.2.1  1996/01/26  20:28:37  agh
 * Branch: Dial1_branch
 * Initial Code Commit for MLPVT.
 *
 * Revision 1.1  1996/01/26  19:22:32  abaindur
 * Initial revision
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "../ip/ip.h"
#include "../tcp/tcp.h"
#include "../ip/ip_registry.h"
#include "logger.h"
#include "login.h"

#include "../mlpvt/mlpvt.h"
#include "../mlpvt/mlpvt_chain.h"

static ulong mlpvt_state;
extern void mlpvt_if_state_change(idbtype *, hwidbtype *);

/*
 * =================================================================
 * Name: mlpvt_getpackage_state
 *
 * Description:	Wrapper that returns the state of the mlpvt package.
 *
 * Parameters:	void - None.
 *
 * Returns:	ulong - mlpvt package state.
 *
 * =================================================================
 */

ulong
mlpvt_getpackage_state (void)
{
    return(mlpvt_state);
}

/*
 * =================================================================
 * Name: mlpvt_setpackage_state
 *
 * Description: Wrapper that sets the state of the mlpvt package.
 *
 * Parameters:	ulong - new state of mlpvt package.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_setpackage_state (ulong state)
{
    mlpvt_state = state;
}

/*
 * =================================================================
 * Name: mlpvt_process_shutdown
 *
 * Description:	Kill the processes required to support the pipes
 *		used by the upper protocols.
 *
 * Parameters:	void - None.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_process_shutdown (void)
{
    mlpvt_mq_shutdown();
    mlpvt_hello_shutdown();

    mlpvt_setpackage_state(MLPVT_IDLE);
}

/*
 * =================================================================
 * Name: mlpvt_process_init
 *
 * Description:	Start the hello and connection server.
 *		Initalise parse chains.
 *
 * Parameters:	void - None.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

void
mlpvt_process_init (void)
{
    mlpvt_hello_startup();
    mlpvt_init_groups();
    mlpvt_mq_init();

    mlpvt_setpackage_state(MLPVT_ACTIVE);
}

/*
 * =================================================================
 * Name: mlpvt_subsys_init
 *
 * Description: Set the package state to idle (this allows a check
 *		to see if the package is installed).
 *
 * Parameters:	subsystype * - pointer to subsys.
 *
 * Returns:	void - None.
 *
 * =================================================================
 */

static void
mlpvt_subsys_init (subsystype *subsys)
{

    /*
     * Add registries
     */
    reg_add_mlpvt_find_stackname(mlpvt_find_stackname, 
                                 "mlpvt_find_stackname");
    reg_add_mlpvt_find_membername(mlpvt_find_membername,
                                  "mlpvt_find_membername");
    reg_add_if_statechange_complete(mlpvt_if_state_change, 
                                  "mlpvt_if_state_change");

    mlpvt_setpackage_state(MLPVT_IDLE);
    
    mlpvt_parser_init();
}

SUBSYS_HEADER(mlpvt, MLPVT_MAJVERSION, MLPVT_MINVERSION, MLPVT_EDITVERSION,
	      mlpvt_subsys_init, SUBSYS_CLASS_PROTOCOL,
	      NULL,
	      NULL)
