/* $Id: fsm.h,v 3.2.62.1 1996/07/19 22:46:55 snyder Exp $
 * $Source: /release/112/cvs/Xsys/util/fsm.h,v $
 *------------------------------------------------------------------
 * fsm.h -- General Purpose Finite State Machine Driver
 *
 *      March 4, 1992 - Wayne Clark
 *
 *      Copyright (c) 1992-1996 by cisco Systems, Inc.
 *      All Rights Reserved
 *------------------------------------------------------------------
 * $Log: fsm.h,v $
 * Revision 3.2.62.1  1996/07/19  22:46:55  snyder
 * CSCdi63560:  move fsm tables to text space from data
 *              gets 53,716 bytes of data back for run from flash platforms
 *              gets 68 bytes of image for all
 *
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  17:56:09  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:43:05  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:50  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

#ifndef FSM_H
#define FSM_H
/***********************************************************************
*
* Finite State Machine (FSM)
*
* This file contains the declaration of "structures" and "external
* variables", and the definition of "symbolic contants" that relate
* to FSM processing.
*
***********************************************************************/


/***********************************************************************
*
* Structure:	fsm_state
*
* This structure represents an element of a "state" table.  There is
* a separate "state" table for each state that a Finite State Machine
* can assume.  Within each "state" table, there is a unique element for
* every "input" that the associated Finite State Machine can handle.
*
* (*action)()	: A pointer to the procedure to be called to process the
*		  current input.  This pointer is set to FSM_NOACTION
*		  (NULL) in two cases:
*		  1) the input is legal but there is no action to be
*		  performed, and
*		  2) the input is illegal in the current state (i.e.,
*		  next_state == FSM_INVALID).
* *param_blk    : A pointer to the parameter block to pass to the
*                 action routine.  The param_blk structure is dependent
*                 on the fsm being implemented.  It is constant across
*                 a given fsm.
* next_state	: The state the the Finite State Machine is to assume
*		  after the current input has been processed by
*		  (*action)().  Each state is identified by a numeric
*		  value, which is greater that 0.  State are assigned
*		  sequential values beginning with 1.  There must be
*		  no gaps.
*
*		  See the two #defines below for special values for
*		  next_state.
*
***********************************************************************/

struct fsm_state
{
    ushort	(*action)(void *, void const *);
    void const  *param_blk;
    int		next_state;
};

#define FSM_NOARG       NULL

/*
** Special value for fsm_state.action
*/
#define  FSM_NOACTION	((ushort (*)(void *, void const *))0)
#define  FSM_ACTION_ROUTINE(rtn) ((ushort(*)(void *, void const *))rtn)

/*
** Special values for fsm_state.next_state
*/
    /* FAPL next-state indicator (-), same-state transition / remain
     * in the same state.
     *
     * FSM remains in its current state.  Return code = OK
     */
#define  FSM_NOCHANGE	(int)0

    /* FAPL next-state indicator (/), "cannot occur" condition, no
     * state change.
     *
     * The decoded input is invalid in the current state.  This
     * indicates a logic fault within the FSM caller.
     *
     * FSM remains in its current state.  RETURN CODE = FSM_FAULT.
     */
#define  FSM_INVALID	-1

    /* FAPL next-state indicator (>), error condition, no state
     * change.  There is generally an associated "action" routine.
     *
     * The decoded input indicates a protocol violation in the 
     * current state.
     *
     * FSM remains in its current state.  RETURN CODE = FSM_ERR.
     */
#define  FSM_ERROR	-2

#define  FSM_FAULT	-3

/*
**  FSM Return Code values:
*/
#define  FSM_OK			  0
#define  FSM_RC_UNKNOWN_STATE	 -1
#define  FSM_RC_UNKNOWN_INPUT	 -2
#define  FSM_RC_INVALID_INPUT	 -3
#define  FSM_RC_ERROR_INPUT	 -4
#define  FSM_RC_UNEXPECTED_INPUT -5
#define	 FSM_RC_DATA_ERROR	 -6


/***********************************************************************
*
* Structure:	finite_state_machine
*
* This structure represents an element of a "state machine" table.
* There is a separate "state machine" table for each Finite State
* Machine.  Within a "state machine" table, there is a unique element
* for each state that the Finite State Machine may assume.
*
* The order of elements in the "state machine" table is critical.  The
* first element (table[0]) represents state #1, the second element
* (table[1]) represents state #2, and so on.  That is, the numeric
* value of "current state" indicator is used as a direct index into
* the "state machine" table.
*
* *state	: A pointer to the "state" table that represents the
*		  current state.
* (*decode)()	: A pointer to the procedure that decodes input, in the
*		  current state.
*
***********************************************************************/

struct finite_state_machine
{
    const struct fsm_state	*state_tbl;
    int			(*decode)(void *, int, int);
};

#define FSM_DECODE_ROUTINE(rtn) ((int(*)(void *, int, int))rtn)

typedef struct finite_state_machine fsm_t;

/* -- In <os/fsm.c> -- */
extern  short   fsm_execute(char *, const fsm_t *, int *, int, void *, int, int, boolean);

#endif	/*FSM_H */
