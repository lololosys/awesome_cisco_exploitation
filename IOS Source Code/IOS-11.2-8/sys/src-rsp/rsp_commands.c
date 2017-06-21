/* $Id: rsp_commands.c,v 3.3.20.4 1996/08/28 13:13:34 thille Exp $
 * $Source: /release/112/cvs/Xsys/src-rsp/rsp_commands.c,v $
 *------------------------------------------------------------------
 * Generic RP->(IP,SP,IF) command implementation for RSP.
 *
 * April 1994, David Bath
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rsp_commands.c,v $
 * Revision 3.3.20.4  1996/08/28  13:13:34  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.3.20.3  1996/04/29  10:03:10  mdb
 * The 96q1 gcc is much more particular about putting parentheses around
 * comparisons for some operands ("&", "^" and "|") [CSCdi56015].
 * Branch: California_branch
 *
 * Revision 3.3.20.2  1996/03/21  23:49:46  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.3.20.1  1996/03/18  22:01:57  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.2.2  1996/02/10  00:18:39  mbeesley
 * CSCdi48581:  Add VIP2 support to ELC_branch
 * Branch: ELC_branch
 *
 * Revision 3.2.2.1  1996/02/08  09:29:37  gstovall
 * Branch: ELC_branch
 * Sync to ELC_baseline_960205.
 *
 * Revision 3.3  1996/02/01  06:09:45  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2.26.1  1996/01/24  23:03:46  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  18:47:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:27:46  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  21:28:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/09/17  20:55:23  motto
 * CSCdi40168:  IPC does not work with a CIP in a RSP (c75xx) router
 *
 * Revision 2.2  1995/07/16  22:30:55  getchell
 * CSCdi35501:  src-rsp/ccb_cmd.h should be moved to hes
 * src-rsp/[ccb.h,ccb_cmd.h] combined and moved to hes/ccb_cmd.h.
 * rsp_command prototypes removed from ccb_cmd.h and moved to
 * src-rsp/rsp_commands.h.  Files including altered files modifed to
 * reflect new file structure.
 *
 * Revision 2.1  1995/06/07  22:57:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "logger.h"
#include "interface_private.h"
#include "../hes/if_fci.h"
#include "../hes/ucode.h"
#include "../dev/flash_dvr_spec.h"
#include "../hes/slot.h"
#include "../hes/if_cmd.h"
#include "../hes/ccb_cmd.h"
#include "../hes/if_vip.h"
#include "rsp_bufhdr.h"
#include "rsp_if.h"
#include "rsp_qasic.h"
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "msg_rsp.c"			/* Not a typo, see logger.h */


/*
 * microseconds to wait before declaring a command timed-out
 */
#define CCB_TIMEOUT_USEC	50

/*
 * There are places where our contract specifies that we should
 * return garbage.  Using a #define for this, rather than just
 * a hardcoded constant, makes it clear that we're not returning
 * anything significant.
 */
#define GARBAGE 0 

/*
 * Macros to get the CCB and command queue pointer given either a
 * hwidb, or a slot number.  This lets us easily optimise where
 * we keep these fields later.
 *
 * perhaps, but repeated dereferences are costly.
 * better to be clear about it and do it explicitly
 */
#define SLOTP_TO_CCB(s)     ((s)->ccb_ptr)
#define SLOTP_TO_CMDQ(s)    ((s)->cmd_q_ptr)
#define SLOTP_TO_NUM(s)     ((s)->slot_num)


/******************************************************************************
 * Name: wait_for_command_done(slot_ptr)
 *
 * Description: 
 *   Enter a CPU busy loop polling the CCB's done field.  We keep an
 *   eye on the clock, and if the IP hasn't got back in a 
 *   reasonable time, we'll return an error.  We also need to
 *   be reasonably sociable with regards accesses to MEMD, lest
 *   we block out others from using it.  Finally, we need to mimic the
 *   SP with regards processing the DONE field.  In particular,
 *   if the IP sets the DONE field to a negative value, then it is 
 *   attempting to indicate an error, and we pass this code back.
 *   If it sets it to a positive value, then it is simply attempting
 *   to indicate success, so we pass back 0.
 *
 * Inputs: 
 *   slotptr -- the slot we're dealing with
 *		contains ptr to CCB
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   msclock -- we use the system tick via the timer macros
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *    FCI_RSP_OKAY if the command was successfully executed,  otherwise
 *    an error code which may be any negative value the IP places
 *    in the DONE field, or FCI_RSP_ERR_TMOUT if the IP doesn't
 *    respond within a reasonable time.
 *
 * Restrictions/Assumptions:
 *   none
 *
 * Side effects:
 *   Uses errmsg() to report timeout errors.
 *
 ******************************************************************************
*/
static ushort wait_for_command_done (slottype* slotp)
{
    ccbtype *ccb;
    usec_timestamp timestamp;
    ushort done_field;

    /*
     * If the board is already wedged, return a timeout error now
     * so that we don't wait forever trying to bring up a crashed board!
     */
    if (slotp->flags & BOARD_WEDGED)
	return(FCI_RSP_ERR_TMOUT);

    /*
     * Set up the timestamp (the SP waits up to 12us)
     */
    USEC_TIMER_START(timestamp, CCB_TIMEOUT_USEC);

    ccb = slotp->ccb_ptr;

    /*
     * While the IP is not done ...
     */
    for (;;) {
	done_field = ccb->ccb_done;
	if (done_field)
	    break;
	/*
	 * If our timer has expired, then log an error and return
	 * a timeout error to indicate our lack of joy.
	 */
	if (USEC_AWAKE(timestamp)) {
	    ushort prev_cmnd;

	    /*
	     * check again, in case a long interrupt kept us from
	     * checking the done bit until after the timer expired
	     */
	    done_field = ccb->ccb_done;
	    if (done_field)
		break;
	    {			/* complain about cmds that take too long */
		ulong elapsed;
		usec_timestamp start;
		sys_timestamp dead;

		USEC_GET_TIMESTAMP(start);
		TIMER_START(dead, ONESEC/4);
		while (SLEEPING(dead)) {
		    done_field = ccb->ccb_done;
		    if (done_field)
			break;
		}
		elapsed = USEC_ELAPSED_TIME(start);
		if (done_field) {
		    errmsg(&msgsym(FOREVER, RSP),
			   ccb->ccb_cmd, slotp->card_name, slotp->slot_num,
			   CCB_TIMEOUT_USEC + elapsed, done_field);
		    break;
		}
	    }
	    prev_cmnd = ccb->ccb_cmd;
	    if ((slotp->flags & BOARD_WEDGED) == 0) {
		slotp->flags |= BOARD_WEDGED;
		if ((slotp->flags & BOARD_NONEXIST) == 0)
		    errmsg(&msgsym(CMDTIMEOUT, CBUS),
			   (uint) ccb, slotp->slot_num, prev_cmnd);
	    }
	    return(FCI_RSP_ERR_TMOUT);
	}
    }

    /*
     * If the high order bit is set, i.e. the DONE field is "negative"
     * then we return it untouched. Otherwise we return 0.  This
     * processing is stolen from the SP, which we're trying to mimic.
     */
    if (done_field & CMD_RSP_ERROR) {
	if (done_field == CMD_RSP_ERR_CMDQFL) {
	    errmsg(&msgsym(CMDDROPPED, CBUS),
		   (uint) ccb, slotp->slot_num, ccb->ccb_cmd);
	}
	return(done_field);
    } else {
	return (FCI_RSP_OKAY);
    }
}

/******************************************************************************
 * Name: generic_send_if_cmd (idb, cmnd, arg0, arg1, arg2, arg_count)
 *
 * Description: 
 *   Send commands to an interface. This routine is a private utility
 *   routine within this module.  All variations of the send_if_cmd()
 *   series (see below) use this routine to do the meat of the work. 
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface to which the command is
 *          to be sent
 *
 *   cmnd -- the command code to be sent to the interface.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the interface.
 *
 *   arg0 -- 16-bits worth of arg0.  Only relevant if arg_count > 0.
 *           If arg_count == 0, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *   arg1 -- 16-bits worth of arg1.  Only relevant if arg_count > 1.
 *           If arg_count <= 1, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *   arg2 -- 16-bits worth of arg2.  Only relevant if arg_count > 2.
 *           If arg_count <= 2, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface.  Note, that
 *           traditionally, there is no arg2 in the CCB structure,
 *           so this argument is passed via the CCB's res1 field.
 *
 *   arg_count -- used to indicate how many of arg0 thru' arg2 are
 *                meaningful.  It's value *must* be one of the following:
 *                  0: none of arg0, arg1 or arg2 are meaningful
 *                  1: arg0 is meaningful, arg1 and arg2 are not
 *                  2: arg0 and arg1 are meaningful, arg2 is not
 *                  3: arg0, arg1 and arg2 are all meaningful
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *   none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
extern inline void generic_send_if_cmd (hwidbtype* idb, 
				 ushort cmnd, 
				 ushort arg0,
				 ushort arg1,
				 ushort arg2,
				 ushort arg_count)
{
    ccbtype *ccb;
    qa_hwq_t *cmdq;
    slottype *slotp;

    /*
     * Next we need to make sure any previous command is completed.
     * If the previous command won't complete we'll just exit here.
     * The user will find out about it if and when they request the
     * status.
     */
    slotp = idb->slot_ptr;
    if (wait_for_command_done(slotp) == FCI_RSP_ERR_TMOUT) {
	return;
    }

    /*
     * OK, we now have the correct interface selected, and the
     * CCB is idle, so it's time to issue the command at last.
     * Note that the order in which the following writes hit the
     * bus is irrelevant as the IP will not look at any of them
     * until its attention signal is raised.
     */
    ccb = slotp->ccb_ptr;
    ccb->ccb_done = 0;                       /* Clear the done flag */

    /*
     * Depending on which of our various public interfaces is being
     * used, we set up different argument fields in the CCB.  Note
     * that we have used the res1 field of the CCB as a mechanism
     * for passing a 3rd argument to IPs.
     */
    switch (arg_count) {
    case 3:
	ccb->ccb_res1 = arg2;                /* and flow on to ... */
    case 2:
	ccb->ccb_arg1 = arg1;                /* and flow on to ... */
    case 1:
	ccb->ccb_arg0 = arg0;
    }

    ccb->ccb_cmd = cmnd;

    /*
     * CCBs live in buffer headers, and when we address CCBs, we do so
     * via an uncached page so we don't have to do anything special
     * to get the above transactions out on the bus before asserting
     * the attention signal to the IP.  The asserting of the attention
     * signal involves a write to the same memory system, and write
     * ordering is guaranteed.
     *
     * We now need to get the IPs attention.  This is done via an enqueue
     * to the hardware command queue associated with this IP.
     * No actual information is passed via the queue, but we do need
     * to obey the normal QA asic rules, in particular, no element
     * may be on more than one queue at a time.  We use the address
     * of the CCB to achieve this, as the CCB lives in a buffer
     * header.  We just need a single pulse, so we can put the CCB
     * on the hardware queue and then rip it straight off again.
     */
    cmdq = slotp->cmd_q_ptr;

    hwq_enqueue(cmdq, (bufhdr_t*)ccb);

    (void)hwq_dequeue(cmdq);

    return;
}


/******************************************************************************
 * Name: send_if_cmd(idb, cmnd)
 *
 * Description: 
 *   Send commands to an interface without any arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_if_cmd() above.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface to which the command is
 *          to be sent
 *
 *   cmnd -- the command code to be sent to the interface.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the interface.
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_if_cmd (hwidbtype* idb, ushort cmnd)
{
    generic_send_if_cmd(idb, cmnd, 0, 0, 0, 0);
}


/******************************************************************************
 * Name: send_if_cmd_1arg(idb, cmnd, arg0)
 *
 * Description: 
 *   Send commands to an interface with one argument.  All the
 *   real work is done by a common routine, namely
 *   generic_send_if_cmd() above.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface to which the command is
 *          to be sent
 *
 *   cmnd -- the command code to be sent to the interface.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the interface.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_if_cmd_1arg (hwidbtype* idb, ushort cmnd, ushort arg0)
{
    generic_send_if_cmd (idb, cmnd, arg0, 0, 0, 1);
}


/******************************************************************************
 * Name: send_if_cmd_2arg(idb, cmnd, arg0, arg1)
 *
 * Description: 
 *   Send commands to an interface with two arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_if_cmd() above.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface to which the command is
 *          to be sent
 *
 *   cmnd -- the command code to be sent to the interface.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the interface.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *   arg1 -- 16-bits worth of arg1.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_if_cmd_2arg (hwidbtype* idb, 
		       ushort cmnd, 
		       ushort arg0,
		       ushort arg1)
{
    generic_send_if_cmd (idb, cmnd, arg0, arg1, 0, 2);
}


/******************************************************************************
 * Name: send_if_cmd_3arg(idb, cmnd, arg0, arg1, arg2)
 *
 * Description: 
 *   Send commands to an interface with three arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_if_cmd() above.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface to which the command is
 *          to be sent
 *
 *   cmnd -- the command code to be sent to the interface.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the interface.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *   arg1 -- 16-bits worth of arg1.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface
 *
 *   arg2 -- 16-bits worth of arg2.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface.  Since the CCB
 *           doesn't have an arg2 field, this argument is passed via
 *           the CCB's res1 field.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_if_cmd_3arg (hwidbtype* idb, 
		       ushort cmnd, 
		       ushort arg0,
		       ushort arg1,
		       ushort arg2)
{
    generic_send_if_cmd (idb, cmnd, arg0, arg1, arg2, 3);
}


/******************************************************************************
 * Name: send_if_cmd_larg(idb, cmnd, larg)
 *
 * Description: 
 *   Send commands to an interface with one long argument.  All the
 *   real work is done by a common routine, namely
 *   generic_send_if_cmd() above.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface to which the command is
 *          to be sent
 *
 *   cmnd -- the command code to be sent to the interface.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the interface.
 *
 *   larg -- 32-bits worth of arg.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the interface.  It puts the
 *           low order 16-bits in arg0 of the CCB, and the high
 *           order 16-bits in arg1 of the CCB.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_if_cmd_larg (hwidbtype* idb, ushort cmnd, ulong larg)
{
    generic_send_if_cmd (idb, cmnd, larg & 0xFFFF, larg >> 16, 0, 2);
}


/******************************************************************************
 * Name: get_if_status(idb)
 *
 * Description: 
 *   Get the status reply from a previously issued command.
 *   (This is the information that currently comes back in the cmdreg in
 *   the existing hes implementations).  Note that this call will 
 *   stall the CPU awaiting a reply from the IF.  On some architectures
 *   (for example the RP/SP) this stall is implemented by the hardware
 *   stalling the read cycle to the SP register.  On other architectures
 *   (for example, the RSP) this routine will sit in a busy CPU 
 *   loop awaiting a reply from the IF.  After some suitably
 *   long time, if the IF still hasn't responded, this routine will 
 *   return a TIMEOUT indication.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface from which we want the
 *          status
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  status
 *   0 if the command completed successfully            OR
 *   the value returned from the IF in the DONE field,  OR
 *   FCI_RSP_ERR_TMOUT if the IF didn't complete the command in a 
 *     reasonable time limit.
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ushort get_if_status (hwidbtype* idb)
{
    /*
     * We need to wait until the IP has completed executing
     * the command, but we wont wait forever.  If it hasn't completed
     * within a reasonable period, we'll return a TIMEOUT indication.
     * All of this logic is handled by wait_for_command_done().
     */
    return (wait_for_command_done(idb->slot_ptr));
}


/******************************************************************************
 * Name: get_if_val(idb)
 *
 * Description: 
 *   Get the 16-bit "data" from a previously issued command.
 *   (this is the information that currently comes back in the argreg in
 *   the existing hes implementations). Note that this call will 
 *   stall the CPU awaiting a reply from the IF.  On some architectures
 *   (for example the RP/SP) this stall is implemented by the hardware
 *   stalling the read cycle to the SP register.  On other architectures
 *   (for example, the RSP) this routine will sit in a busy CPU 
 *   loop awaiting a reply from the IF.  If the caller has just previously
 *   returned from a call to get_if_status() then the CPU loop in 
 *   get_if_val() will effectively be a no-op as the necessary waiting has
 *   already been performed by get_if_status().
 *   If after some suitably long time
 *   the IF still hasn't responded, this routine will return garbage
 *   and an appropriate error message will be logged.
 *   The implication, therefore, is that callers who are "expecting"
 *   possible timeouts should always call get_if_status() first.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface from which we want the
 *          data
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  val
 *   the 16-bit data returned from the IF in res0,  OR
 *   garbage if the IF didn't complete the command in a reasonable 
 *      time limit.
 *      (as mentioned above in Description, the caller of this routine
 *      has no way of knowing which, use get_if_status() instead)
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ushort get_if_val (hwidbtype* idb)
{
    ccbtype *ccb;
    slottype *slotp;

    /*
     * First we need to wait until the IP has completed executing
     * the command, but we wont wait forever.  If it hasn't completed
     * within a reasonable period we'll log an error and return garbage.
     */
    slotp = idb->slot_ptr;
    if (wait_for_command_done(slotp) == FCI_RSP_ERR_TMOUT ) {
	return (GARBAGE);
    }

    /*
     * Note that the CCB pointer accesses the CCB via an uncached
     * page, so the following access will cause a real read to MEMD.
     */
    ccb = slotp->ccb_ptr;
    return(ccb->ccb_res0);
}


/******************************************************************************
 * Name: get_if_lval(idb)
 *
 * Description: 
 *   Get the 32-bit "data" from a previously issued command.
 *   (this is the information that currently comes back in the largreg in
 *   the existing hes implementations). Note that this call will 
 *   stall the CPU awaiting a reply from the IF.  On some architectures
 *   (for example the RP/SP) this stall is implemented by the hardware
 *   stalling the read cycle to the SP register.  On other architectures
 *   (for example, the RSP) this routine will sit in a busy CPU 
 *   loop awaiting a reply from the IF.  If the caller has just previously
 *   returned from a call to get_if_status() then the CPU loop in 
 *   get_if_val() will effectively be a no-op as the necessary waiting has
 *   already been performed by get_if_status().
 *   If after some suitably long time
 *   the IF still hasn't responded, this routine will return garbage
 *   and an appropriate error message will be logged.
 *   The implication, therefore, is that callers who are "expecting"
 *   possible timeouts should always call get_if_status() first.
 *
 * Inputs: 
 *   idb -- the hwidb indicating the interface from which we want the
 *          data
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  lval
 *   the 32-bit data returned from the IF in res1/res0,  OR
 *   garbage if the IF didn't complete the command in a reasonable 
 *      time limit.
 *      (as mentioned above in Description, the caller of this routine
 *      has no way of knowing which, use get_if_status() instead)
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ulong get_if_lval (hwidbtype* idb)
{
    ccbtype *ccb;
    slottype *slotp;

    /*
     * First we need to wait until the IP has completed executing
     * the command, but we wont wait forever.  If it hasn't completed
     * within a reasonable period we'll log an error and return garbage.
     */
    slotp = idb->slot_ptr;
    if (wait_for_command_done(slotp) == FCI_RSP_ERR_TMOUT) {
	return (GARBAGE);
    }

    /*
     * Note that the CCB pointer accesses the CCB via an uncached
     * page, so the following access will cause a real read to MEMD.
     */
    ccb = slotp->ccb_ptr;
    return((ccb->ccb_res1 << 16) | ccb->ccb_res0);
}


/******************************************************************************
 * Name: generic_send_ip_cmd (slotp, cmnd, arg0, arg1, arg2, arg_count)
 *
 * Description: 
 *   Send commands to an IP. This routine is a private utility
 *   routine within this module.  All variations of the send_ip_cmd()
 *   series (see below) use this routine to do the meat of the work. 
 *   
 *
 * Inputs: 
 *   slotp  -- the slotp indicating the IP to which the command is
 *             to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   arg0 -- 16-bits worth of arg0.  Only relevant if arg_count > 0.
 *           If arg_count == 0, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *   arg1 -- 16-bits worth of arg1.  Only relevant if arg_count > 1.
 *           If arg_count <= 1, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *   arg2 -- 16-bits worth of arg2.  Only relevant if arg_count > 2.
 *           If arg_count <= 2, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  Note, that
 *           traditionally, there is no arg2 in the CCB structure,
 *           so this argument is passed via the CCB's res1 field.
 *
 *   arg3 -- 16-bits worth of arg3.  Only relevant if arg_count > 3.
 *           If arg_count <= 3, this argument is ignored.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  Note, that
 *           traditionally, there is no arg3 in the CCB structure,
 *           so this argument is passed via the CCB's res0 field.
 *
 *   arg_count -- used to indicate how many of arg0 thru' arg2 are
 *                meaningful.  It's value *must* be one of the following:
 *                  0: none of arg0, arg1, arg2 or arg3 are meaningful
 *                  1: arg0 is meaningful, arg1 and arg2 and arg3 are not
 *                  2: arg0 and arg1 are meaningful, arg2 and arg3 are not
 *                  3: arg0, arg1 and arg2 are meaningful, arg3 is not
 *                  4: arg0, arg1 and arg2 and arg3 are all meaningful
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *   none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
extern inline void generic_send_ip_cmd (slottype* slotp, 
				 ushort cmnd, 
				 ushort arg0,
				 ushort arg1,
				 ushort arg2,
				 ushort arg3,
				 ushort arg_count)
{
    ccbtype *ccb = SLOTP_TO_CCB(slotp);     /* Locate CCB and.... */
    qa_hwq_t *cmdq = SLOTP_TO_CMDQ(slotp); /* the command queue */

    /*
     * Next we need to make sure any previous command is completed.
     * If the previous command won't complete we'll just exit here.
     * The user will find out about it if and when they request the
     * status.
     */
    if (wait_for_command_done(slotp) == FCI_RSP_ERR_TMOUT) {
	return;
    }

    /*
     * CCB is idle, so it's time to issue the command at last.
     * Note that the order in which the following writes hit the
     * bus is irrelevant as the IP will not look at any of them
     * until its attention signal is raised.
     */
    ccb->ccb_done = 0;                       /* Clear the done flag */

    /*
     * Depending on which of our various public interfaces is being
     * used, we set up different argument fields in the CCB.  Note
     * that we have used the res1 field of the CCB as a mechanism
     * for passing a 3rd argument to IPs.
     */
    switch (arg_count) {
    case 4:
	ccb->ccb_res0 = arg3;                /* and flow on to ... */
    case 3:
	ccb->ccb_res1 = arg2;                /* and flow on to ... */
    case 2:
	ccb->ccb_arg1 = arg1;                /* and flow on to ... */
    case 1:
	ccb->ccb_arg0 = arg0;
    }

    ccb->ccb_cmd = cmnd;

    /*
     * CCBs live in buffer headers, and when we address CCBs, we do so
     * via an uncached page so we don't have to do anything special
     * to get the above transactions out on the bus before asserting
     * the attention signal to the IP.  The asserting of the attention
     * signal involves a write to the same memory system, and write
     * ordering is guaranteed.
     *
     * We now need to get the IPs attention.  This is done via an enqueue
     * to the hardware command queue associated with this IP.
     * No actual information is passed via the queue, but we do need
     * to obey the normal QA asic rules, in particular, no element
     * may be on more than one queue at a time.  We use the address
     * of the CCB to achieve this, as the CCB lives in a buffer
     * header.  We just need a single pulse, so we can put the CCB
     * on the hardware queue and then rip it straight off again.
     */
    hwq_enqueue(cmdq, (bufhdr_t*)ccb);

    (void)hwq_dequeue(cmdq);

    return;
}


/******************************************************************************
 * Name: send_ip_cmd(slotp, cmnd)
 *
 * Description: 
 *   Send commands to an IP without any arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd (slottype* slotp, ushort cmnd)
{
    generic_send_ip_cmd (slotp, cmnd, 0, 0, 0, 0, 0);
}


/******************************************************************************
 * Name: send_ip_cmd_1arg(slotp, cmnd, arg0)
 *
 * Description: 
 *   Send commands to an IP with one argument.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd_1arg (slottype* slotp, ushort cmnd, ushort arg0)
{
    generic_send_ip_cmd (slotp, cmnd, arg0, 0, 0, 0, 1);
}


/******************************************************************************
 * Name: send_ip_cmd_2arg(slotp, cmnd, arg0, arg1)
 *
 * Description: 
 *   Send commands to an IP with two arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP
 *
 *   arg1 -- 16-bits worth of arg1.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd_2arg (slottype* slotp, 
		       ushort cmnd, 
		       ushort arg0,
		       ushort arg1)
{
    generic_send_ip_cmd (slotp, cmnd, arg0, arg1, 0, 0, 2);
}


/******************************************************************************
 * Name: send_ip_cmd_3arg(slotp, cmnd, arg0, arg1, arg2)
 *
 * Description: 
 *   Send commands to an IP with three arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *   arg1 -- 16-bits worth of arg1.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *   arg2 -- 16-bits worth of arg2.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  Since the CCB
 *           doesn't have an arg2 field, this argument is passed via
 *           the CCB's res1 field.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd_3arg (slottype* slotp, 
		       ushort cmnd, 
		       ushort arg0,
		       ushort arg1,
		       ushort arg2)
{
    generic_send_ip_cmd (slotp, cmnd, arg0, arg1, arg2, 0, 3);
}


/******************************************************************************
 * Name: send_ip_cmd_4arg(slotp, cmnd, arg0, arg1, arg2, arg3)
 *
 * Description: 
 *   Send commands to an IP with three arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   arg0 -- 16-bits worth of arg0.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *   arg1 -- 16-bits worth of arg1.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.
 *
 *   arg2 -- 16-bits worth of arg2.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  Since the CCB
 *           doesn't have an arg2 field, this argument is passed via
 *           the CCB's res1 field.
 *
 *   arg3 -- 16-bits worth of arg3.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  Since the CCB
 *           doesn't have an arg3 field, this argument is passed via
 *           the CCB's res0 field.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd_4arg (slottype* slotp, 
		       ushort cmnd, 
		       ushort arg0,
		       ushort arg1,
		       ushort arg2,
		       ushort arg3)
{
    generic_send_ip_cmd (slotp, cmnd, arg0, arg1, arg2, arg3, 4);
}


/******************************************************************************
 * Name: send_ip_cmd_larg(slotp, cmnd, larg)
 *
 * Description: 
 *   Send commands to an IP with one long argument.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   larg -- 32-bits worth of arg.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  It puts the
 *           low order 16-bits in arg0 of the CCB, and the high
 *           order 16-bits in arg1 of the CCB.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd_larg (slottype* slotp, ushort cmnd, ulong larg)
{
    generic_send_ip_cmd (slotp, cmnd, larg & 0xFFFF, larg >> 16, 0, 0, 2);
}


/******************************************************************************
 * Name: send_ip_cmd_2larg(slotp, cmnd, larg0, larg1)
 *
 * Description: 
 *   Send commands to an IP with two long arguments.  All the
 *   real work is done by a common routine, namely
 *   generic_send_ip_cmd() above.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent
 *
 *   cmnd -- the command code to be sent to the IP.  This
 *           routine makes no interpretation of this code, but simply
 *           passes it on to the IP.
 *
 *   larg0 -- 32-bits worth of arg.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  It puts the
 *           low order 16-bits in arg0 of the CCB, and the high
 *           order 16-bits in arg1 of the CCB.
 *
 *   larg1 -- 32-bits worth of arg.  This
 *           routine makes no interpretation of this value, but
 *           simply passes it on to the IP.  It puts the
 *           low order 16-bits in res0 of the CCB, and the high
 *           order 16-bits in res1 of the CCB.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_cmd_2larg (slottype* slotp, ushort cmnd, ulong larg0, ulong larg1)
{
    generic_send_ip_cmd(slotp, cmnd, larg0 & 0xFFFF, larg0 >> 16,
	larg1 >> 16, larg1 & 0xFFFF, 4);
}


/******************************************************************************
 * Name: get_ip_status(slotp)
 *
 * Description: 
 *   Get the status reply from a previously issued command.
 *   (This is the information that currently comes back in the cmdreg in
 *   the existing hes implementations).  Note that this call will 
 *   stall the CPU awaiting a reply from the IP.  On some architectures
 *   (for example the RP/SP) this stall is implemented by the hardware
 *   stalling the read cycle to the SP register.  On other architectures
 *   (for example, the RSP) this routine will sit in a busy CPU 
 *   loop awaiting a reply from the IP.  After some suitably
 *   long time, if the IP still hasn't responded, this routine will 
 *   return a TIMEOUT indication.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP from which we want the
 *            status
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  status
 *   0 if the command completed successfully            OR
 *   the value returned from the IP in the DONE field,  OR
 *   FCI_RSP_ERR_TMOUT if the IP didn't complete the command in a 
 *     reasonable time limit.
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ushort get_ip_status (slottype* slotp)
{

    /*
     * We need to wait until the IP has completed executing
     * the command, but we wont wait forever.  If it hasn't completed
     * within a reasonable period, we'll return a TIMEOUT indication.
     * All of this logic is handled by wait_for_command_done().
     */
    return (wait_for_command_done(slotp));
}


/******************************************************************************
 * Name: get_ip_val(slotp)
 *
 * Description: 
 *   Get the 16-bit "data" from a previously issued command.
 *   (this is the information that currently comes back in the argreg in
 *   the existing hes implementations). Note that this call will 
 *   stall the CPU awaiting a reply from the IP.  On some architectures
 *   (for example the RP/SP) this stall is implemented by the hardware
 *   stalling the read cycle to the SP register.  On other architectures
 *   (for example, the RSP) this routine will sit in a busy CPU 
 *   loop awaiting a reply from the IP.  If the caller has just previously
 *   returned from a call to get_ip_status() then the CPU loop in 
 *   get_ip_val() will effectively be a no-op as the necessary waiting has
 *   already been performed by get_ip_status().
 *   If after some suitably long time
 *   the IP still hasn't responded, this routine will return garbage
 *   and an appropriate error message will be logged.
 *   The implication, therefore, is that callers who are "expecting"
 *   possible timeouts should always call get_ip_status() first.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP from which we want the
 *            data
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  val
 *   the 16-bit data returned from the IP in res0,  OR
 *   garbage if the IP didn't complete the command in a reasonable 
 *      time limit.
 *      (as mentioned above in Description, the caller of this routine
 *      has no way of knowing which, use get_ip_status() instead)
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ushort get_ip_val (slottype* slotp)
{
    ccbtype *ccb = SLOTP_TO_CCB(slotp);        /* Locate CCB */

    /*
     * First we need to wait until the IP has completed executing
     * the command, but we wont wait forever.  If it hasn't completed
     * within a reasonable period we'll log an error and return garbage.
     */
    if (wait_for_command_done(slotp) == FCI_RSP_ERR_TMOUT) {
	return (GARBAGE);
    }

    /*
     * Note that the CCB pointer accesses the CCB via an uncached
     * page, so the following access will cause a real read to MEMD.
     */
    return(ccb->ccb_res0);
}


/******************************************************************************
 * Name: get_ip_lval(slotp)
 *
 * Description: 
 *   Get the 32-bit "data" from a previously issued command.
 *   (this is the information that currently comes back in the largreg in
 *   the existing hes implementations). Note that this call will 
 *   stall the CPU awaiting a reply from the IP.  On some architectures
 *   (for example the RP/SP) this stall is implemented by the hardware
 *   stalling the read cycle to the SP register.  On other architectures
 *   (for example, the RSP) this routine will sit in a busy CPU 
 *   loop awaiting a reply from the IP.  If the caller has just previously
 *   returned from a call to get_ip_status() then the CPU loop in 
 *   get_ip_val() will effectively be a no-op as the necessary waiting has
 *   already been performed by get_ip_status().
 *   If after some suitably long time
 *   the IP still hasn't responded, this routine will return garbage
 *   and an appropriate error message will be logged.
 *   The implication, therefore, is that callers who are "expecting"
 *   possible timeouts should always call get_ip_status() first.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP from which we want the
 *            data
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  lval
 *   the 32-bit data returned from the IP in res1/res0,  OR
 *   garbage if the IP didn't complete the command in a reasonable 
 *      time limit.
 *      (as mentioned above in Description, the caller of this routine
 *      has no way of knowing which, use get_ip_status() instead)
 *
 * Restrictions/Assumptions:
 *    none
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ulong get_ip_lval (slottype* slotp)
{
    ccbtype *ccb = SLOTP_TO_CCB(slotp);        /* Locate CCB */

    /*
     * First we need to wait until the IP has completed executing
     * the command, but we wont wait forever.  If it hasn't completed
     * within a reasonable period we'll log an error and return garbage.
     */
    if (wait_for_command_done(slotp) == FCI_RSP_ERR_TMOUT) {
	return (GARBAGE);
    }

    /*
     * Note that the CCB pointer accesses the CCB via an uncached
     * page, so the following access will cause a real read to MEMD.
     */
    return((ccb->ccb_res1 << 16) | ccb->ccb_res0);
}

/******************************************************************************
 * Name: get_ip_diag0(slotp)
 *
 * Description: 
 *   Return the diag0 value from the CCB associated with this slot.
 *   Some IPs will put diagnostic information in here just before
 *   crashing (for example, the TRIP).
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP from which we want the
 *            diag
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  val
 *   the 16-bit data returned from the IP in diag0
 *
 * Restrictions/Assumptions:
 *    We do not handshake with the DONE bit.  The diag words are
 *    typically used during emergencies, just before the ucode
 *    issues a "jmp .", so normal DONE bit testing is invalid.
 *    The caller of this routine should already have determined
 *    via other means (such as an 8010 return) that the card may
 *    have placed something of use in the diag words.
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ushort get_ip_diag0 (slottype* slotp)
{
    ccbtype *ccb = SLOTP_TO_CCB(slotp);        /* Locate CCB */

    /*
     * Note that the CCB pointer accesses the CCB via an uncached
     * page, so the following access will cause a real read to MEMD.
     */
    return(ccb->ccb_diag0);
}


/******************************************************************************
 * Name: get_ip_diag1(slotp)
 *
 * Description: 
 *   Return the diag1 value from the CCB associated with this slot.
 *   Some IPs will put diagnostic information in here just before
 *   crashing (for example, the TRIP).
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP from which we want the
 *            diag
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  val
 *   the 16-bit data returned from the IP in diag1
 *
 * Restrictions/Assumptions:
 *    We do not handshake with the DONE bit.  The diag words are
 *    typically used during emergencies, just before the ucode
 *    issues a "jmp .", so normal DONE bit testing is invalid.
 *    The caller of this routine should already have determined
 *    via other means (such as an 8010 return) that the card may
 *    have placed something of use in the diag words.
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
ushort get_ip_diag1 (slottype* slotp)
{
    ccbtype *ccb = SLOTP_TO_CCB(slotp);        /* Locate CCB */

    /*
     * Note that the CCB pointer accesses the CCB via an uncached
     * page, so the following access will cause a real read to MEMD.
     */
    return(ccb->ccb_diag1);
}


/******************************************************************************
 * Name: send_ip_ccb_ptr(slotp)
 *
 * Description: 
 *   Send a CCB pntr to an IP.
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP to which the command is
 *            to be sent.  The following fields are of particularr
 *            interest to this routine, and must be setup before
 *            calling:
 *
 *       ccb_ptr -- contains the full 32-bit address of the ccb_ptr
 *                  in uncached space.  This routine assumes that
 *                  this address space view is consistent in the bottom
 *                  16-bits with all other views of MEMD, and
 *                  therefore this address can be passed as is to
 *                  hwq_enqueue.
 *
 *       cmd_q_ptr -- contains the queue to which we should send the
 *                    command.
 *
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:
 *   none
 *
 * Restrictions/Assumptions:
 *    see Input description of ccb_ptr above, for asssumptions about
 *    address space mapping.
 *
 * Side effects:
 *   none
 *
 ******************************************************************************
*/
void send_ip_ccb_ptr (slottype* slotp)
{
    ccbtype *ccb = SLOTP_TO_CCB(slotp);     /* Locate CCB and.... */
    qa_hwq_t *cmdq = SLOTP_TO_CMDQ(slotp); /* the command queue */
	
    /*
     * We now write the CCB pointer to the hardware queue and leave
     * it there.  It is the IP's responsibility to read this off
     * the queue (they all poll this queue when starting).
     */
    hwq_enqueue(cmdq, (bufhdr_t*)ccb);
}

/******************************************************************************
 * Name: get_ip_ccb_status(slotp)
 *
 * Description: 
 *   Get the status reply from a previous call to send_ip_ccb_ptr()
 *   This routine will wait (within limits) until the IP has read
 *   its CCB pointer out of the hardware queue, *and* executed
 *   the harmless command placed in the CCB by send_ip_ccb_ptr().
 *
 * Inputs: 
 *   slotp -- the slot indicating the IP from which we want the
 *            status.  Of particular interest are the following
 *            fields:
 *
 *       ccb_ptr -- contains the full 32-bit address of the ccb_ptr
 *                  in uncached space.  This routine assumes that
 *                  this address space view is consistent in the bottom
 *                  16-bits with all other views of MEMD, and
 *                  therefore this address can be passed as is to
 *                  hwq_peek.
 *
 *       cmd_q_ptr -- contains the queue to which the command was
 *                    previously set.  We now sniff this queue to
 *                    determine if/when the IP has removed the pntr.
 *
 *       slot_num -- used for error reporting in the case where the
 *                   IP won't remove it's CCB pntr, or won't process
 *                   the first command.
 *
 * Outputs: 
 *   none
 *
 * Implicit inputs: 
 *   none
 *
 * Implicit outputs: 
 *   none
 *
 * Return Value:  status
 *   0 if the the pntr was fetched, and the cmnd executed  OR
 *   the value returned from the IP in the DONE field,     OR
 *   FCI_RSP_ERR_TMOUT if the IP didn't complete the command in a 
 *     reasonable time limit, or didn't remove the CCB pntr from
 *     the queue.
 *
 * Restrictions/Assumptions:
 *   This routine must be called.  Even if the caller of
 *   send_ip_ccb_ptr() is not particularly interested in the outcome,
 *   this routine must be called to allow the RP to re-sync with the
 *   IP.  Failure to do so may result in spurious command timeouts later.
 *
 * Side effects:
 *   Before returning FCI_RSP_ERR_TMOUT, it will log a useful error
 *   with the errmsg routine.
 *
 ******************************************************************************
*/
ushort get_ip_ccb_status (slottype* slotp)
{
    sys_timestamp timestamp;
    ccbtype *ccb = SLOTP_TO_CCB(slotp);     /* Locate CCB and.... */
    qa_hwq_t *cmdq = SLOTP_TO_CMDQ(slotp); /* the command queue */
	
    /*
     * Set up the timestamp x secs into the future
     */
    /* FIXME */
    if (vip_controller(slotp->ctrlr_type)){
	TIMER_START(timestamp, 10000);                /* 10.0 seconds */
    } else {
	TIMER_START(timestamp, 500);                  /* .5 seconds */
    }

    /*
     * Next we wait until the queue goes empty.  This may take a long
     * time as the card completes its initialisation.  We'll give it
     * 0.5 seconds before we get stroppy.
     */
    while (hwq_peek(cmdq)) {
	/*
	 * If our 5.0 secs has expired, then log an error and return
	 * FCI_RSP_ERR_TMOUT to indicate our lack of joy.  We also
	 * remove the CCB pntr from the hardware queue, to prevent
	 * possible future QA errors due to queueing the same
	 * element twice.
	 */
	if (!SLEEPING(timestamp)) {
	    bufhdr_t *ret;

	    /* check one last time after timer expired */
	    ret = hwq_dequeue(cmdq);
	    if (ret == NULL)
		break;
	    /* something bad happened, might as well check that
	     * the right thing came back from the queue */
	    if (((ulong) ret & 0xffff) != ((ulong) ccb & 0xffff))
		ccb = (ccbtype*) ret;
	    slotp->flags |= BOARD_WEDGED;
	    if ((slotp->flags & BOARD_NONEXIST) == 0)
		errmsg(&msgsym(CCBPTIMEOUT, CBUS), ccb, SLOTP_TO_NUM(slotp));
	    return(FCI_RSP_ERR_TMOUT);
	}
    }
    ccb->ccb_done = CCB_DONE_OK;
    return FCI_RSP_OKAY;
}
