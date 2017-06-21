/* $Id: ctrlr_cmds.h,v 3.4.8.2 1996/08/28 12:43:42 thille Exp $
 * $Source: /release/112/cvs/Xsys/hes/ctrlr_cmds.h,v $
 *------------------------------------------------------------------
 * ctrlr_cmds.h: support routines for CBUS and CTRLR command retries
 *
 * March 1995: Karen Mitchell
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: ctrlr_cmds.h,v $
 * Revision 3.4.8.2  1996/08/28  12:43:42  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.4.8.1  1996/03/18  19:40:02  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.3  1996/03/16  06:49:18  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.2.26.2  1996/02/20  00:50:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/28  22:42:24  motto
 * CSCdi49312:  CIP IPC not working after router reload
 *
 * Revision 3.3  1996/02/08  20:10:59  thille
 * CSCdi48347: format string in inlines, code size impact
 * Pass 1 - Clean up some literal strings in inlines, duplicate literals,
 * etc.  11,356 bytes saved on gs7-j-m, 9336 bytes saved on igs-j-l
 *
 * Revision 3.2.26.1  1996/01/24  21:45:05  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.2  1995/11/17  09:15:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:56  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.4  1995/11/08  20:59:40  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.3  1995/08/16  18:23:57  kmitchel
 * CSCdi38045:  Resync counter wraps resulting in ineffective timeout
 *              recovery
 *              - Also convert ctrlr_resync() from inline to function
 *
 * Revision 2.2  1995/07/13  01:20:29  fsunaval
 * CSCdi36847:  FCICMDFAILs put under debug control (debug cbus
 * cmd-timeout)
 *
 * Revision 2.1  1995/06/07  20:38:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * -----------------------------------------------------------------------------
 * Inline functions for sending commands to Interface Processors, with retry
 * -----------------------------------------------------------------------------
 *
 * Rationale:
 *  IPs must respond to cbus controller commands within 12us or a timeout error
 *  (FCI_RSP_ERR_TMOUT 0x8010) will occur.  Intelligent interface processors
 *  (i.e. those which perform much of their proccessing via software) may 
 *  occasionally miss this reponse window, typically due to the overhead
 *  involved in fetching the instructions into cache to service the request.
 *
 *  Once the instructions are in the cache, the IP is typically able to respond
 *  to the command within the required interval.  It is therefore reasonable to
 *  reissue the command if a timeout has occurred.  These inlines are designed 
 *  to handle the reattempts, including the necessary resynchronization of the
 *  IP and to (optionally) issue an errmsg if the operation were not successful.
 * 
 *  Resynchronization is required since the SP reports the timeout error to the
 *  RP, but not to the IP.  The IP may issue the acknowledgement, but it will 
 *  then be paired with a subsequent command; now the RP and IP are out of sync 
 *  with commands and responses.  When this situation occurs, the RP must 
 *  resync the IP.
 *
 * Resynchronization:
 *  Resynchronization is accomplished by sending the IP a specific command
 *  (FCI_CTRLR_CMDSYNC) with a token which must be returned.  The token is 
 *  actually a counter, initialized to a non-zero value, and incremented each 
 *  time the command is sent. When the IP receives this command, it returns 
 *  the value sent, and stores it locally (if the token of a current
 *  command matches the stored value, the command is ignored).  
 *  If the value returned by the IP does not match what was sent, this is an 
 *  indication that the response (eg. FCI_RSP_OKAY) is actually associated with
 *  a previous command; that is, the RP and the IP are out of sync.  
 *  The synchronization will continue until the IP responds with a  matching 
 *  token, or until CTRLR_RESYNC_ATTEMPTS have been made.  
 *  If the resynchronization is not successful, the original command which
 *  timed-out is not retried; the error is reported back to the caller.
 *    
 * Description: 
 *  Controller commands are sent via FCI_CMD_CTRLR command or by issuing a
 *  FCI_CMD_XXX to the cbus controller which then forwards it to the IP. 
 *  (Only certain commands are forwarded; most terminate at the SP).
 *
 *  Commands sent via FCI_CMD_CTRLR require the controller to be selected first;
 *  sometimes the interface, as well.  Interface selection is also required by 
 *  the forwarded cbus commands.
 *   
 *  Note that the selection of interface is subject to timeout. Interface
 *  selection normally works in the following manner:
 *   - The RP issues the FCI_CMD_SELECT cbus command, indicating the mci_index
 *     as the interface number (16 if interface 0 of slot 2)
 *   - The SP determines whether or not this represents a change in interface 
 *     selection for that particular controller.  
 *     - If so, it forwards the command to the interface processor, which then
 *       stores the selected interface.  Note that the command is forwarded as a
 *       3 (rather than 2), and the interface number supplied in the argument is
 *       adjusted (interface 16 is sent as interface 0).  
 *     - The interface processor responds with the type of the interface that
 *       has just been selected.  Note that the interface types are also stored
 *       in specific MEMD locations.
 *     - The SP returns this value in argreg.
 *   - If the interface selection has not changed since the SP last forwarded
 *     the request to the interface processor, then the SP returns to the RP
 *     without forwarding the request.  The value returned to the RP as the 
 *     interface type is taken from the MEMD location, not from the value 
 *     previously reported by the interface processor.
 *
 *  Note that there are several consequences of a timeout occurring on a 
 *  forwarded FCI_CMD_SELECT:
 *   - The SP believes that a particular interface has been selected
 *     (it does not unselect it if the interface processor times out on the 
 *     command).
 *   - There is a (remote) possibility that the interface processor did not 
 *     completely process the changed interface selection (it could have
 *     missed the request altogether).  Thus, the SP and IP are not in
 *     agreement with what is the selected interface.
 *   - Subsequent commmands intended for the selected interface will actually
 *     be processed for the previously selected interface.  Reselecting the 
 *     interface via FCI_CMD_SELECT will not help, since the request will not
 *     be forwarded.
 *   - There is a possibility that when the IP *does* respond to the interface 
 *     selection request, the reponse will be paired with a subsequent command
 *     (the RP and the IP have gotten out of sync).
 *   - If the RP does not check the error code (read cmdreg), it will retrieve
 *     garbage from argreg (which is supposed to specify the interface type). If
 *     the RP is interested in this information, then trouble will ensue.  
 *     Note that the cmdreg value is hardly ever (if ever) checked after the 
 *     FCI_CMD_SELECT.
 *
 *
 *  The inline functions provided here consist of a set of primitives which 
 *  handle the actual issuance of the command and recovery from timeout.  
 *  Variations exist in order to provide a full set of alternatives, so that 
 *  the most efficient inline can be used, depending on the kind of information
 *  actually needed by the command.
 *  For example, if a command does not expect data back, then the variation
 *  which does not read argreg should be used.
 *
 *  Variations:
 *   Command Type: cbus command or ctrlr command 
 *                (FCI_CMD_XXXX or FCI_CTRLR_CMD with command specified 
 *                in argreg)
 *   Response Type: none, ushort or ulong
 *   Argument Type: none, ushort or ulong
 *   Result Type  : dont care, ushort
 * 
 *   This gives (2 * 3 * 3 * 2) or 36 possible methods of issuing a command 
 *   intended for an interface processor.  One could argue that only the most
 *   generic forms are necessary (ie. always provide arg, and return result and
 *   response), but that would be inefficient.
 *    
 *   The inlines are mnemonically named to explicitly indicate the variation:
 *
 *       fci_CommandType_ResponseType_ArgType_ResultType
 *          _cbus_      _cmd_         none     none
 *          _ctrlr_     _qry_        _arg_    _rslt_
 *                      _lqry_       _larg_
 * 
 * The primitives are: 
 * fci_ctrlr_cmd_rslt    fci_ctrlr_cmd_arg_rslt       fci_ctrlr_cmd_larg_rslt
 * fci_cbus_cmd_rslt     fci_cbus_cmd_arg_rslt        fci_cbus_cmd_larg_rslt
 *   
 * The variations are: 
 * fci_ctrlr_cmd         fci_ctrlr_cmd_arg            fci_ctrlr_cmd_larg
 * fci_ctrlr_qry         fci_ctrlr_qry_arg            fci_ctrlr_qry_larg
 * fci_ctrlr_lqry        fci_ctrlr_lqry_arg           fci_ctrlr_lqry_larg
 * fci_ctrlr_qry_rslt    fci_ctrlr_qry_arg_rslt       fci_ctrlr_qry_larg_rslt
 * fci_ctrlr_lqry_rslt   fci_ctrlr_lqry_arg_rslt      fci_ctrlr_lqry_larg_rslt
 *
 * fci_cbus_cmd          fci_cbus_cmd_arg             fci_cbus_cmd_larg
 * fci_cbus_qry          fci_cbus_qry_arg             fci_cbus_qry_larg
 * fci_cbus_lqry         fci_cbus_lqry_arg            fci_cbus_lqry_larg
 * fci_cbus_qry_rslt     fci_cbus_qry_arg_rslt        fci_cbus_qry_larg_rslt
 * fci_cbus_lqry_rslt    fci_cbus_lqry_arg_rslt       fci_ctrlr_lqry_larg_rslt
 *
 * Parameters:
 *  The first two and the last two parameters are the same for all inlines:
 *    dcitype *dci       required in order to determine controller syncability
 *    mciregtype *regptr   provided for efficiency
 *    ...
 *    int attempts         number of times to retry in event of timeout
 *    boolean msg          issue a message if failure
 *
 *  The command is always supplied:
 *    uint cmd             FCI_CMD_XXXX or arg to FCI_CTRLR_CMD
 *
 *  The other parameters are supplied depending on the variation:
 *    (nothing)   ushort *sresult      address of result
 *    (nothing)   uint arg             argument (uint) even if ushort
 *    (nothing)   ushort *response  ulong *lresponse address of response 
 *
 *    
 *  The constant CTRLR_CMD_ATTEMPTS can be used for attempts
 *    It is very unlikely that the command will require more than one reattempt
 *    after synchronization.
 *
 *  The dci_from_hwidb() inline and the DCI_FROM_HWIDB() macro are provided to 
 *    easily retrieve the dci from a hwidbtype, since the dci is not usually
 *    readily available.
 *
 * Return value and effects:
 *    Returns TRUE if result is FCI_RSP_OKAY; otherwise FALSE
 *    issues errmsg if command failed (after retries have been exhausted) 
 *    AND msg is TRUE
 *    No retries are attempted if the result comes back other than
 *    FCI_RSP_ERR_TMOUT
 *
 * Assumptions: Several assumptions are made for the sake of code efficiency.
 *   - interrupts have been disabled (NETS_DISABLE)
 *   - the requisite interface or ctrlr selection has been done 
 *     (use fci_select_if() or fci_grab_if() and/or fci_select_ctrlr() for this)
 *   - attempts is non-zero. Specifying zero will not give you what you expect.
 *     Since the count is decremented and then checked, specifying zero will
 *     result in 2**32-1 attempts.
 *
 * -----------------------------------------------------------------------------
 */
#include "logger.h"			/* Required for next include. */
#include "../hes/msg_cbus.c"		/* Not a typo, see logger.h */
#include "../hes/if_fci.h"

extern boolean cbus_cmdretry_debug;
extern boolean cbus_cmdtimeout_debug;

extern boolean ctrlr_resync(dcitype *, mciregtype *, boolean);

#define CTRLR_RESYNC_INIT_VAL 0x2815   /* Initial value for resync token */
                                        /* Arbitrary non-zero value       */

/*
 * Default number of attempts to make 
 */
#define CTRLR_CMD_ATTEMPT_ONCE 1
#define CTRLR_CMD_ATTEMPTS     4
#define CTRLR_RESYNC_ATTEMPTS 16
#define CTRLR_RESYNC_PAUSE_MS  1

/*
 * Return the dci for this interface
 */
#define DCI_FROM_HWIDB(hwidb) \
        (((mcitype *)(hwidb)->devctl)->dci[(hwidb)->cbus_ctrlr_index])


static inline dcitype *dci_from_hwidb (
    hwidbtype *hwidb) 
{  
    mcitype *cbus;
    cbus = (mcitype *) hwidb->devctl;
    return (cbus->dci[hwidb->cbus_ctrlr_index]);
}

 /* ----------------------------------------------------------------- */
extern const char Controller_d_s_cmd_d_required_d_attempts[];

static inline void msg0_if_count (
    uint unit,
    const char *cmdtype,					 
    uint cmd,
    uint count)
{
    if (cbus_cmdretry_debug && (count > 1)) {
	buginf(Controller_d_s_cmd_d_required_d_attempts,
	       unit, cmdtype, cmd, count);
    }
}
    
extern const char Controller_d_s_cmd_d_x_x_required_d_attempts[];
static inline void msg1_if_count (
    uint unit,
    const char *cmdtype,					 
    uint cmd,
    ulong arg,					 
    uint count)
{
    if (cbus_cmdretry_debug && (count > 1)) {
	buginf(Controller_d_s_cmd_d_x_x_required_d_attempts,
	       unit, cmdtype, cmd, arg, count);
    }
}

extern const char Controller_d_interface_selection_d_required_retry[];
static inline void msgSelect_retried (
    uint unit,
    ulong arg)
{
    if (cbus_cmdretry_debug) {
	buginf(Controller_d_interface_selection_d_required_retry,
	       unit, arg);
    }
}

extern const char Controller_d_interface_selection_d_failed_no_retry[];
static inline void msgSelectFailed (
    uint unit,
    ulong arg)
{
    if (cbus_cmdretry_debug) {
	buginf(Controller_d_interface_selection_d_failed_no_retry,
	       unit, arg);
    }
}


 /* ----------------------------------------------------------------- */

/*
 * FCI_CTRLR_CMD inlines
 */
extern const char ctrlr_msg[];
extern const char cbus_msg[];

static inline boolean fci_ctrlr_cmd_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,
    uint cmd,
    int attempts,
    boolean msg)
{
    int count = 0;

    do {
	regptr->argreg = cmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) ||
		    ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(CTRLRCMDFAIL0, CBUS), dci->ctrlr_unit, cmd,
		       *sresult, count);
	return FALSE;
    }
    msg0_if_count(dci->ctrlr_unit, ctrlr_msg, cmd, count);
    return TRUE;
}

static inline boolean fci_ctrlr_cmd_arg_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint arg,
    int attempts,
    boolean msg)
{
    int count = 0;
    do {
	regptr->argreg = arg;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = cmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}    		
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) ||
		    ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(CTRLRCMDFAIL1, CBUS), dci->ctrlr_unit, cmd,
		       arg, *sresult, count);
	return FALSE;
    }
    msg1_if_count(dci->ctrlr_unit, ctrlr_msg, cmd, arg, count);
    return TRUE;
}

static inline boolean fci_ctrlr_cmd_larg_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    ulong larg,
    int attempts,
    boolean msg)
{
    int count = 0;
    do {
	regptr->largreg = larg;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->argreg = cmd;
	regptr->cmdreg = FCI_CTRLR_CMD;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}	
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) ||
		    ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(CTRLRCMDFAIL1, CBUS), dci->ctrlr_unit, cmd,
		       larg, *sresult, count);
	return FALSE;
    }
    msg1_if_count(dci->ctrlr_unit, ctrlr_msg, cmd, larg, count);
    return TRUE;
}

static inline boolean fci_ctrlr_cmd_2larg_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    ulong larg1,
    ulong larg2,
    int attempts,
    boolean msg)
{
    int count = 0;
    do {
	regptr->largreg = larg1;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT;
	regptr->largreg = larg2;
	regptr->cmdreg = FCI_CTRLR_ARGUMENT2;
	regptr->argreg = cmd;
	regptr->cmdreg = FCI_CTRLR_CMD_EXT;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}	
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) || ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(CTRLRCMDFAIL1, CBUS), dci->ctrlr_unit, cmd, larg1, *sresult, count);
	return FALSE;
    }
    msg1_if_count(dci->ctrlr_unit, "ctrlr", cmd, larg1, count);
    return TRUE;
}

/*
 * Cbus controller commands (FCI_CMD_xxxx) that are forwarded
 * to a controller
 */

static inline boolean fci_cbus_cmd_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,					
    uint cmd,				       
    int attempts,
    boolean msg)
{
    int count = 0;
    do {
	regptr->cmdreg = cmd;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}	
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) ||
		    ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(FCICMDFAIL0, CBUS), dci->ctrlr_unit, cmd,
		       *sresult, count);
	return FALSE;
    }
    msg0_if_count(dci->ctrlr_unit, cbus_msg, cmd, count);
    return TRUE;
}

static inline boolean fci_cbus_cmd_arg_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,					    
    uint cmd,				       
    ushort arg,
    int attempts,
    boolean msg)
{
    int count = 0;
    do {
	regptr->argreg = arg;
	regptr->cmdreg = cmd;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}	
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) ||
		    ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(FCICMDFAIL1, CBUS), dci->ctrlr_unit, cmd, arg,
		       *sresult, count);
	return FALSE;
    }
    msg1_if_count(dci->ctrlr_unit, cbus_msg, cmd, arg, count);
    return TRUE;

}

static inline boolean fci_cbus_cmd_larg_rslt (
    dcitype *dci,
    mciregtype *regptr,
    ushort *sresult,					    
    uint cmd,				       
    ulong larg,
    int attempts,
    boolean msg)
{
    int count = 0;
    do {
	regptr->largreg = larg;
	regptr->cmdreg = cmd;
	*sresult = regptr->cmdreg;
	count++;
	if (*sresult == FCI_RSP_ERR_TMOUT) {
	    if (!ctrlr_resync(dci, regptr, msg))
		break;
	}	
    }
    while ((*sresult == FCI_RSP_ERR_TMOUT) && (count < attempts));
    if (*sresult != FCI_RSP_OKAY) {
        if (msg && ((*sresult != FCI_RSP_ERR_TMOUT) ||
		    ((*sresult == FCI_RSP_ERR_TMOUT)
		&& cbus_cmdtimeout_debug && (cmd == FCI_CMD_ERRORS))))
	    	errmsg(&msgsym(FCICMDFAIL1, CBUS), dci->ctrlr_unit, cmd,
		       larg, *sresult, count);
	return FALSE;
    }
    msg1_if_count(dci->ctrlr_unit, cbus_msg, cmd, larg, count);
    return TRUE;

}



   /*
    * Variations 
    */

/* ------------------------------------------------------------------- */

static inline boolean fci_ctrlr_cmd (
    dcitype *dci,
    mciregtype *regptr,
    uint cmd,				       
    int attempts,
    boolean msg)
{
    ushort sresult;
    return (fci_ctrlr_cmd_rslt(dci, regptr, &sresult, cmd,
			       attempts, msg));
}

static inline boolean fci_ctrlr_cmd_arg (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    uint arg, 
    int attempts,
    boolean msg)
{
    ushort sresult;
    return(fci_ctrlr_cmd_arg_rslt(dci, regptr, &sresult, cmd, arg, 
				  attempts, msg));
}

static inline boolean fci_ctrlr_cmd_larg (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    uint larg, 
    int attempts,
    boolean msg)
{
    ushort sresult;
    return(fci_ctrlr_cmd_larg_rslt(dci, regptr, &sresult, cmd, larg, 
				   attempts, msg));
}

/* ------------------------------------------------------------------- */

static inline boolean fci_ctrlr_qry_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_rslt(dci, regptr, sresult, cmd, 
			    attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}

static inline boolean fci_ctrlr_qry_arg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint arg, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_arg_rslt(dci, regptr, sresult, cmd, arg, 
				attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}

static inline boolean fci_ctrlr_qry_larg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint larg, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_larg_rslt(dci, regptr, sresult, cmd, larg, 
				 attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}


/* ------------------------------------------------------------------- */

static inline boolean fci_ctrlr_lqry_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    ulong *lresult,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_rslt(dci, regptr, sresult, cmd, 
			    attempts, msg))
	return FALSE;
    *lresult = regptr->largreg;
    return TRUE;
}

static inline boolean fci_ctrlr_lqry_arg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint arg, 
    ulong *lresult,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_arg_rslt(dci, regptr, sresult, cmd, arg, 
				attempts, msg))
	return FALSE;
    *lresult = regptr->largreg;
    return TRUE;
}

static inline boolean fci_ctrlr_lqry_larg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint larg, 
    ulong *lresult,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_larg_rslt(dci, regptr, sresult, cmd, larg, 
				 attempts, msg))
	return FALSE;
    *lresult = regptr->largreg;
    return TRUE;
}

/* ------------------------------------------------------------------- */

static inline boolean fci_ctrlr_qry (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd(dci, regptr, cmd, attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}

static inline boolean fci_ctrlr_qry_arg (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    uint arg, 
    ushort *response,
    int attempts,
    boolean msg)
{
    ushort sresult;
    return (fci_ctrlr_qry_arg_rslt(dci, regptr, &sresult, cmd, arg, 
				   response, attempts, msg));
}

static inline boolean fci_ctrlr_qry_larg (
    dcitype *dci,
    mciregtype *regptr,
    uint cmd, 
    uint larg, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_larg(dci, regptr, cmd, larg, attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}

/* ------------------------------------------------------------------- */

static inline boolean fci_ctrlr_lqry (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    ulong *lresponse,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd(dci, regptr, cmd, attempts, msg))
	return FALSE;
    *lresponse = regptr->largreg;
    return TRUE;
}

static inline boolean fci_ctrlr_lqry_arg (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    uint arg, 
    ulong *lresponse,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_arg(dci, regptr, cmd, arg, attempts, msg))
	return FALSE;
    *lresponse = regptr->largreg;
    return TRUE;
}

static inline boolean fci_ctrlr_lqry_larg (
    dcitype *dci,
    mciregtype *regptr, 
    uint cmd, 
    uint larg, 
    ulong *lresponse,
    int attempts,
    boolean msg)
{
    if (!fci_ctrlr_cmd_larg(dci, regptr, cmd, larg, attempts, msg))
	return FALSE;
    *lresponse = regptr->largreg;
    return TRUE;
}



/* ------------------------------------------------------------------- */

static inline boolean fci_cbus_cmd (
    dcitype *dci,
    mciregtype *regptr,
    uint cmd,				       
    int attempts,
    boolean msg)
{
    ushort sresult;
    return (fci_cbus_cmd_rslt(dci, regptr, &sresult, cmd,
			      attempts, msg));
}

static inline boolean fci_cbus_cmd_arg (
    dcitype *dci,
    mciregtype *regptr,
    uint cmd,				       
    ushort arg,
    int attempts,
    boolean msg)
{
    ushort sresult;
    return (fci_cbus_cmd_arg_rslt(dci, regptr, &sresult, cmd,
				  arg, attempts, msg));
}

static inline boolean fci_cbus_cmd_larg (
    dcitype *dci, 
    mciregtype *regptr,
    uint cmd, 
    uint larg, 
    int attempts,
    boolean msg)
{
    ushort sresult;
    return(fci_cbus_cmd_larg_rslt(dci, regptr, &sresult, cmd, larg, 
				   attempts, msg));
}

/* ------------------------------------------------------------------- */

static inline boolean fci_cbus_qry_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_cbus_cmd_rslt(dci, regptr, sresult, cmd, 
			    attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}

static inline boolean fci_cbus_qry_arg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint arg, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_cbus_cmd_arg_rslt(dci, regptr, sresult, cmd, arg, 
				attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}

static inline boolean fci_cbus_qry_larg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint larg, 
    ushort *response,
    int attempts,
    boolean msg)
{
    if (!fci_cbus_cmd_larg_rslt(dci, regptr, sresult, cmd, larg, 
				 attempts, msg))
	return FALSE;
    *response = regptr->argreg;
    return TRUE;
}


/* ------------------------------------------------------------------- */

static inline boolean fci_cbus_lqry_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    ulong *lresponse,
    int attempts,
    boolean msg)
{
    if (!fci_cbus_cmd_rslt(dci, regptr, sresult, cmd, 
			    attempts, msg))
	return FALSE;
    *lresponse = regptr->largreg;
    return TRUE;
}

static inline boolean fci_cbus_lqry_arg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint arg, 
    ulong *lresponse,
    int attempts,
    boolean msg)
{
    if (!fci_cbus_cmd_arg_rslt(dci, regptr, sresult, cmd, arg, 
				attempts, msg))
	return FALSE;
    *lresponse = regptr->largreg;
    return TRUE;
}

static inline boolean fci_cbus_lqry_larg_rslt (
    dcitype *dci, 
    mciregtype *regptr,
    ushort *sresult,					      
    uint cmd, 
    uint larg, 
    ulong *lresponse,
    int attempts,
    boolean msg)
{
    if (!fci_cbus_cmd_larg_rslt(dci, regptr, sresult, cmd, larg, 
				 attempts, msg))
	return FALSE;
    *lresponse = regptr->largreg;
    return TRUE;
}

/* ------------------------------------------------------------------- */

/* ------------------------------------------------------------------- */
/*
 * Controller and interface selection inlines
 */
static inline boolean fci_grab_if (
    dcitype *dci,				     
    mciregtype *regptr,
    ushort *sresult,				   
    ushort if_index,
    ushort *if_type)
{
    /*
     * First select the interface via the SP
     */
    regptr->argreg = if_index;
    regptr->cmdreg = FCI_CMD_SELECT;
    *sresult = regptr->cmdreg;

    if (*sresult == FCI_RSP_OKAY) {
	*if_type = regptr->argreg;
	return TRUE;
    }    

    if (*sresult == FCI_RSP_ERR_TMOUT) {
	if (ctrlr_resync(dci, regptr, TRUE)) {
	    int slot, slotunit;
	    cbus_unit2slotunit(if_index, &slot, &slotunit);
	    msgSelect_retried(dci->ctrlr_unit, if_index);
	    /* If the resync worked, tunnel select through SP */
	    return fci_ctrlr_qry_arg_rslt(dci, regptr, sresult,
					  FCI_CTRLR_SELECT_INT,
					  slotunit, if_type,
					  CTRLR_CMD_ATTEMPTS, TRUE); 
	} else {
	    msgSelectFailed(dci->ctrlr_unit, if_index);
	}
    }
    return FALSE;
}

static inline boolean fci_select_if (
    hwidbtype *hwidb,
    mciregtype *regptr)
{
    ushort sresult;				   
    ushort if_type;

    return fci_grab_if(DCI_FROM_HWIDB(hwidb), regptr, &sresult,
		       hwidb->mci_index, &if_type);
}

static inline boolean fci_select_ctrlr (
    hwidbtype *hwidb,
    mciregtype *regptr)
{
    regptr->argreg = hwidb->cbus_ctrlr_index;
    regptr->cmdreg = FCI_CMD_CTRLR_SELECT;
    return TRUE;
}


