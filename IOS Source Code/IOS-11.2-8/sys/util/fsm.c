/* $Id: fsm.c,v 3.2.62.1 1996/07/19 22:46:52 snyder Exp $
 * $Source: /release/112/cvs/Xsys/util/fsm.c,v $
 *------------------------------------------------------------------
 * fsm.c -- General Purpose Finite State Machine Driver
 *
 * March 4, 1992 - Wayne Clark (Wayne's World. Wayne's World.. Party Out)
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fsm.c,v $
 * Revision 3.2.62.1  1996/07/19  22:46:52  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:56:07  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:04  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:48  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
*  fsm.c
*
*  File Description:
*	This file contains a general-purpose Finite State Machine driver.
*	It is patterned after IBM's matrix definition of Finite State Machines
*	as found in most SNA documents.  Blow chunks if you like ... it works.
*
*	The Finite State Machine is manifested through the following
*	components:
*	  1. A table representing the FSM quintuple.  
*	     The academic notion of a Finite State Machine is a quintuple
*	     consisting of:
*	       a. set of states
*	       b. set of inputs
*	       c. set of functions that perform state transitions,
*	       d. set of outputs, and
*	       e. set of functions that produce an output based upon
*	          the current state and the input.
*	  2. An input decoder for each possible state in the FSM,
*	  3. An executer of the FSM.
*
*	1a. (the set of states) is represented in a 2-level hierarchy
*	definition of structures: 'finite_state_machine' and 'fsm_state'.
*	There is an entry in finite_state_machine for each state in the
*	FSM.  Each entry contains a pointer to the fsm_state for the
*	state and a pointer to a function that serves as the decoder
*	of input for the state.  There is an entry in fsm_state for
*	each input in a state.  Each entry in fsm_state is indexed by
*	the specific input ranging from 0 through n-1, where n is the
*	number of inputs.
*
*	1b. (the set of inputs) come in two varieties: raw input and
*	decoded input.  Raw input are input values known by the application
*	using this FSM driver and are passed to the application-supplied
*	FSM decoder specified in the finite_state_machine entry for the
*	current state.  Decoded input values are the result of processing
*	raw input values by the decoder function.  Decoded input values
*	range from 0 through n-1, where n is the number of inputs.  Decoded
*	input values are used as indices into the fsm_state structure
*	for the current state.
*
*	1c. (the set of functions that perform state transitions) are
*	contained within this FSM driver.  This function, fsm_execute(),
*	operates according to the specification of the relevant entry
*	in fsm_state.  (Relevant in this context means according to the
*	fsm_state entry indexed by the decoded input value.)
*
*	Both 1d. (the set of output) and 1e. (the set of functions) are
*	provided by the application using this FSM driver.  The outputs are
*	produced by the action routines.
*
*	2. (the input decoder) is supplied by the application and converts
*	Raw Input known only by the application into a Decoded Input value
*	that can be used by the FSM driver.  This Decoded Input is merely an
*	index into the fsm_state structure for the current state.
*
*	3. (the executer) is merely the application that calls this FSM
*	driver.
*
*  Functions in this file:
*	fsm_execute()
*	    Execute the specified Finite State Machine
*
*****************************************************************************/

/*
**   I n c l u d e   F i l e s
*/
#include	"master.h"
#include	"config.h"
#include	"../util/fsm.h"

/*
**   C o n s t a n t   D e f i n i t i o n s
*/

/*
**   L o c a l   D e f i n i t i o n s
*/

/*
**   G l o b a l   D e f i n i t i o n s
*/

/*
**   E x t e r n a l   D e f i n i t i o n s
*/


/*
**   F o r w a r d   D e c l a r a t i o n s 
*/


/******************************************************************************
*
* Function:	fsm_execute()
*
* Description:
*	This function is a Finite State Machine driver.  The operation of
*	this driver is documented inthe file header above.
*
* Call:		fsm_execute (fsm_name, machine_table, state, max_state, 
*			     structure, major, minor)
*
* Input:
*   Arguments -
*	fsm_name	ASCII name of the finite state machine
*	machine_table	the table representation of the finite state machine
*	state		the variable that represents the current value of
*			the state machine
*	max_state	the maximum value that 'state' can assume
*	structure	a user-defined structure that is passed to the decoder
*			and action routines
*	major		a user-defined major category input to the FSM
*	minor		a user-defined minor category input to the FSM
*       debug           TRUE if FSM should display primative state debugging.
*
* Output:	
*   Arguments -
*	state		the variable that represents the value of the next state
*			of the state machine
*
*   Return Code -
*	returnCode	either an indicator of the success or failure of the
*			finite state machine driver or a return value from the
*			action routines for this finite state machine
*			  FSM_OK		.. no error was found
*			  FSM_RC_UNKNOWN_STATE  .. 'state' is invalid
*			  FSM_RC_UNKNOWN_INPUT  .. 'major'/'minor' is invalid
*			  FSM_RC_INVALID_INPUT  .. 'major'/'minor' is invalid
*						   in the current state
*			  FSM_RC_ERROR_INPUT  	.. 'major'/'minor' causes an
*						   error in the current state
*				- or-
*			  value from action routine
*
* Notes:
*   (1) FSM Raw Input
*	The combination of 'major' and 'minor' comprise the Raw Input to the
*	finite state machine.  These values are interpreted by the FSM decoder
*	and result in a Decoded Input for the finite state machine which is
*	an index into the current state table.
*
*   (2)	Return Code Values
*	Since the return code from fsm_execute() takes on dual purposes (i.e.
*	to indicate the success of the finite state machine execution and
*	as a return value for the action routines, the action routines must
*	be careful not to have one of the FSM_xxx values as their return
*	value.  This includes all numbers less than or equal to 0.
*
*****************************************************************************/
     
short
fsm_execute (
    char				*fsm_name,
    const struct finite_state_machine	*machine_table,
    int					*state,
    int					max_state,
    void				*structure,
    int					major,
    int					minor,
    boolean                    		debug)
{
    int   index;
    const struct fsm_state  *transition;

    /* Verify that current "state" indicator is valid */
    if ((*state < 1) || (*state > max_state))
	return (FSM_RC_UNKNOWN_STATE);

    /* 
    **  Call appropriate input decoder to get the "index" into the
    **  current state table.
    */
    index = (*(machine_table[*state-1].decode))(structure, major, minor);
    if (index == FSM_FAULT)
	return (FSM_RC_UNKNOWN_INPUT);

    /* 
    **  Check if the decoded input is valid in the current state.
    **  If not, return an error indication.
    */
    transition = &machine_table[*state-1].state_tbl[index];
    if (transition->next_state == FSM_INVALID)
	return (FSM_RC_INVALID_INPUT);

    /* Update the "current state" indicator */
    if ((transition->next_state != FSM_NOCHANGE)  &&
	(transition->next_state != FSM_ERROR))
    {
	*state = transition->next_state;
    }

    /* If there is an action routine for this state transition,
    ** call it.
    */
    if (transition->action != FSM_NOACTION)
    {
        return((*(transition->action))(structure, transition->param_blk));
    }

    if (transition->next_state == FSM_ERROR)
	return (FSM_RC_ERROR_INPUT);

    return (FSM_OK);
}
