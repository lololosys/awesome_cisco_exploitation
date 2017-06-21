/* $Id: dspu_debug.c,v 3.3.20.1 1996/07/25 09:43:36 snyder Exp $
 * $Source: /release/112/cvs/Xsys/dspu/dspu_debug.c,v $
 *------------------------------------------------------------------
 * Down Stream PU debug command parse chains 
 * 
 * Mar. 1994, Vivian Tseng
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dspu_debug.c,v $
 * Revision 3.3.20.1  1996/07/25  09:43:36  snyder
 * CSCdi63981:  eliminate unused code, fix bugs, make a common routine
 *              1. create a routine call generic_debug_all and
 *                 make most debugs use it
 *              2. eliminate if (*_debug_inited) return, most never set
 *              the var
 *                 to TRUE making the code useless.
 *              3. declare some constant arrays const
 *              4. fix bugs found along the way
 *              5. save 2768 bytes from image, 660 out of data section
 * Branch: California_branch
 *
 * Revision 3.3  1996/02/01  06:01:57  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.2  1995/11/17  09:04:46  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:28:07  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/09/20  17:02:48  hampton
 * Rename DSPU and SNANM source files that use generic names for specific
 * tasks.  [CSCdi40733]
 *
 * Revision 2.2  1995/09/14 06:15:10  smackie
 * Pre-emptive strike on namespace confusion. Rename DSPU source files that
 * use generic names for specific tasks. (CSCdi40350)
 *
 * Revision 2.1  1995/06/07  20:28:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#include "master.h"
#include "sys_registry.h"
#include "config.h"
#include "logger.h"
#include "../ui/debug.h"
#include "dspu.h"
#include "dspuconf.h"
#include "dspu_debug.h"
#include "dspu_debug_flags.h"
#include "dspu_ls.h"
#include "parser_defs_dspu.h"
#include "dspu_pu.h"
#include "dspu_puutil.h"
#include "dspu_error.h"

/* Declare the initialized text for the debugging array */
#define __DECLARE_DEBUG_ARR__
#include "dspu_debug_flags.h"


/*
 * DSPU Debug Commands
 */
typedef enum {
  DspuDbgCmdActivation,
  DspuDbgCmdPacket,
  DspuDbgCmdStateChg,
  DspuDbgCmdTrace
} DspuDbgCmdStringE;


/*
 * DSPU Debug Command Strings
 */
static const char* const dspuDbgCmdString[] = {
  "activation",
  "packet",
  "state change",
  "trace"
};


/*
 * dspu_debug_all()
 *
 *   Sets state of all DSPU debug flags to "on" 
 * 
 *   Registered to be called whenever "debug all" or "undebug all" commands
 *     are issued by the user.
 *
 *   Input parm "flag" = TRUE to indicate "debug all" command issued
 *   Input parm "flag" = FALSE to indicate "undebug all" command issued
 *
 */
static void dspu_debug_all (boolean flag)
{

    hword dspuDbgTrcFlags = TraceFsm | TraceFsmChange  |
                            TraceIframeTraffic | TraceActivationTraffic;

    if (flag)
        DspuDebug(NULL, dspuDbgTrcFlags);
    else 
        DspuNoDebug(NULL, dspuDbgTrcFlags);

} /* End dspu_debug_all() */


/* dspu_debug_show_pu()
 *
 * Display the values of DSPU debugging variables for an individual PU
 *
 */
static void dspu_debug_show_pu (Pu* puP, boolean *showDbgHdr)
{

    hword globalDbgLevel = GetDebugLevel();
    hword puDbgLevel = GetLuDebugLevel(puP);

    char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);

    boolean showPuDbgHdr = TRUE;

    /*
     *
     * Don't write PU if SNA command (GetSnaMode() == TRUE),
     *   but PU is not SNA PU (IsPuSna() == FALSE)
     *
     * Don't write PU if DSPU command (GetSnaMode() == FALSE),
     *   but PU is not DSPU PU (IsPuSna() == TRUE)
     *
     */
    if ( (GetSnaMode() && !IsPuSna(puP)) || (!GetSnaMode() && IsPuSna(puP)) )
      return;


    puDbgLevel = GetLuDebugLevel(puP);


    /* Display DSPU tracing header */
    /* --------------------------- */ 
    if ( puDbgLevel && (*showDbgHdr) ) {
        DspuDisplayMsg1(DspuDbgHeader, dbgHeader);
        *showDbgHdr = FALSE;
    }


    /* Display DSPU activation tracing */
    /* ------------------------------- */
    if ( (puDbgLevel & TraceActivationTraffic) &&
        !(globalDbgLevel & TraceActivationTraffic) ) {

        if (showPuDbgHdr) {
            DspuDisplayMsg1(DspuDbgPuHeader, GetPuName(puP));
            showPuDbgHdr = FALSE;
        }
        DspuDisplayMsg2(DspuDbgPuOn2, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdActivation]);
    }


    /* Display DSPU packet tracing */
    /* --------------------------- */
    if (  (puDbgLevel & TraceIframeTraffic) &&
         !(globalDbgLevel & TraceIframeTraffic) ) {

        if (showPuDbgHdr) {
            DspuDisplayMsg1(DspuDbgPuHeader, GetPuName(puP));
            showPuDbgHdr = FALSE;
        }
        DspuDisplayMsg2(DspuDbgPuOn2, dbgHeader, 
                          dspuDbgCmdString[DspuDbgCmdPacket]);
    }

    /* Display DSPU FSM state change tracing */
    /* ------------------------------------- */
    if (  (puDbgLevel & TraceFsmChange) &&
         !(globalDbgLevel & TraceFsmChange) ) {
 
        if (showPuDbgHdr) {
            DspuDisplayMsg1(DspuDbgPuHeader, GetPuName(puP));
              showPuDbgHdr = FALSE;
        }
        DspuDisplayMsg2(DspuDbgPuOn2, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdStateChg]);
    }

    /* Display DSPU FSM tracing */
    /* ------------------------ */
    if (  (puDbgLevel & TraceFsm) &&
         !(globalDbgLevel & TraceFsm) ) {

        if (showPuDbgHdr) {
            DspuDisplayMsg1(DspuDbgPuHeader, GetPuName(puP));
            showPuDbgHdr = FALSE;
        }
        DspuDisplayMsg2(DspuDbgPuOn2, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdTrace]);
  }

} /* End dspu_debug_show_pu() */


/* dspu_debug_show_ls()
 *
 * Display the values of DSPU debugging variables for an individual PU
 *   (access PU ptr from LinkStn ptr)
 *
 */
static void dspu_debug_show_ls (LinkStn* lsP, boolean *showDbgHdr)
{

    Pu* puP;

    puP = (lsP) ? LsChild(lsP) : NULL;
    if (puP)
        dspu_debug_show_pu(puP, showDbgHdr);

} /* End dspu_debug_show_ls() */


/* 
 * dspu_debug_show
 *
 *   Display the values of DSPU debugging variables
 */
static void dspu_debug_show (void)
{

    hword globalDbgLevel = 0;
    char *dbgHeader = (GetSnaMode() ? SNA_header : DSPU_header);
    boolean showDspuDbgHeader = TRUE;


    /* Show global debugging levels (i.e debugging level for all PUs) */
    /* -------------------------------------------------------------- */ 
    globalDbgLevel |= GetDebugLevel();

    if (globalDbgLevel) {
        DspuDisplayMsg1(DspuDbgHeader, dbgHeader);
        showDspuDbgHeader = FALSE;
    }

    if (globalDbgLevel & TraceActivationTraffic) 
        DspuDisplayMsg2(DspuDbgAllOn, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdActivation]);

    if (globalDbgLevel & TraceIframeTraffic)
        DspuDisplayMsg2(DspuDbgAllOn, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdPacket]);

    if (globalDbgLevel & TraceFsmChange)
        DspuDisplayMsg2(DspuDbgAllOn, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdStateChg]);

    if (globalDbgLevel & TraceFsm)
        DspuDisplayMsg2(DspuDbgAllOn, dbgHeader,
                          dspuDbgCmdString[DspuDbgCmdTrace]);


    /* Show debugging levels for each individual PU */
    /* -------------------------------------------- */

    DoToAllChildrenParm2(GetUplsRoot(), (ProcChild2) dspu_debug_show_ls, 
                           &showDspuDbgHeader);

    DoToAllChildrenParm2(GetDnlsRoot(), (ProcChild2) dspu_debug_show_ls, 
                           &showDspuDbgHeader);
 
    DoToAllChildrenParm2(GetInactUpuRoot(), (ProcChild2) dspu_debug_show_pu, 
                           &showDspuDbgHeader);

    DoToAllChildrenParm2(GetInactDpuRoot(), (ProcChild2) dspu_debug_show_pu, 
                           &showDspuDbgHeader);

} /* End dspu_debug_show() */


/* 
 * snanm_debug_show
 *
 *   Display the values of SNA debugging variables
 */
static void snanm_debug_show (void)
{

  SetSnaMode(TRUE);
  dspu_debug_show();
  SetSnaMode(FALSE);

} /* End sna_debug_show() */

/*
 * dspu_debug_init() 
 *
 *   Called at DSPU subsystem startup
 *     Registers routines to maintain & display DSPU debug flags
 *     Ensures flags are set to appropriate values if "debug all" 
 *       is in effect when DSPU is started.
 *   
 */
void dspu_debug_init (void)
{

    /* Register for "debug all" and "show debug" events */
    /* ------------------------------------------------ */
    reg_add_debug_all(dspu_debug_all, "dspu_debug_all");
    reg_add_debug_show(dspu_debug_show, "dspu_debug_show");

    /* Ensure flags are set if "debug all" in effect when DSPU is started */
    /* ------------------------------------------------------------------ */
    dspu_debug_all(debug_all_p());

} /* End dspu_debug_init() */


/*
 * snanm_debug_init() 
 *
 *   Called at SNA subsystem startup
 *     Registers routines to maintain & display SNA debug flags
 *     Ensures flags are set to appropriate values if "debug all" 
 *       is in effect when SNA is started.
 *   
 */
void snanm_debug_init (void)
{

    /* Register for "debug all" and "show debug" events */
    /* ------------------------------------------------ */
    reg_add_debug_all(dspu_debug_all, "dspu_debug_all");
    reg_add_debug_show(snanm_debug_show, "sna_debug_show");

    /* Ensure flags are set if "debug all" in effect when SNA is started */
    /* ----------------------------------------------------------------- */
    dspu_debug_all(debug_all_p());

} /* End snanm_debug_init() */


/*
 * dspu_debug_command()
 *
 *   Handles debug commands related to DSPU/SNA
 *
 */
void dspu_debug_command (parseinfo *csb)
{


   boolean dspuDbgCmd = TRUE;      /* FALSE if not a defined "dspu/sna" cmd */

   DspuDbgCmdStringE dspuDbgTrcCmd = 0; /* dspu debug trace command         */
   hword dspuDbgTrcFlags = 0;           /* dspu debug trace flags           */ 
   char* dspuDbgPuNameP;                /* PU name parm from "debug dspu"   */  

   char* dbgHeader = GetSnaMode() ? SNA_header : DSPU_header;


    /* Set trace flags corresponding to "debug command" */
    /* ------------------------------------------------ */
    switch (csb->which) {

        case DEBUG_SNA_STATE:
        case DEBUG_DSPU_STATE:
            dspuDbgTrcCmd   = DspuDbgCmdStateChg;
            dspuDbgTrcFlags = TraceFsmChange;
            break;

        case DEBUG_SNA_TRACE:
        case DEBUG_DSPU_TRACE:
            dspuDbgTrcCmd   = DspuDbgCmdTrace;
            dspuDbgTrcFlags = TraceFsm;
            break;

        case DEBUG_SNA_PACKET:
        case DEBUG_DSPU_PACKET:
            dspuDbgTrcCmd   = DspuDbgCmdPacket;
            dspuDbgTrcFlags = TraceIframeTraffic;
            break;

        case DEBUG_SNA_ACTIVATION:
        case DEBUG_DSPU_ACTIVATION:
            dspuDbgTrcCmd   = DspuDbgCmdActivation;
            dspuDbgTrcFlags = TraceActivationTraffic;
            break;

        default:
            dspuDbgCmd = FALSE;   /* If not a defined "dspu debug" command */
            debug_command(csb);   /*   use generic debug handler           */
            break;
    }

    /* dspuDbgCmd == TRUE if defined "dspu debug" command was issued */
    /* ------------------------------------------------------------- */
    if (dspuDbgCmd) {         

        dspuDbgPuNameP = GETOBJ(int, 1) ? GETOBJ(string, 1) : NULL;

   	if (csb->sense) {
            if ( DspuDebug(dspuDbgPuNameP, dspuDbgTrcFlags) ) {
                if (dspuDbgPuNameP) {
                    DspuDisplayMsg3(DspuDbgPuOn1, dbgHeader,
                      dspuDbgCmdString[dspuDbgTrcCmd], dspuDbgPuNameP);
                } else {
                    DspuDisplayMsg2(DspuDbgAllOn, dbgHeader,
                      dspuDbgCmdString[dspuDbgTrcCmd]);
                }
            } /* End if - DspuDebug returns TRUE - TraceFlags set okay */
        } else {
	    if ( DspuNoDebug(dspuDbgPuNameP, dspuDbgTrcFlags) ) {
                if (dspuDbgPuNameP) {
                    DspuDisplayMsg3(DspuDbgPuOff, dbgHeader,
                      dspuDbgCmdString[dspuDbgTrcCmd], dspuDbgPuNameP);
                } else {
                    DspuDisplayMsg2(DspuDbgAllOff, dbgHeader,
                      dspuDbgCmdString[dspuDbgTrcCmd]);
                }
            }  /* End if - DspuNoDebug returns TRUE - TraceFlags set okay */
        } /* End else (csb->sense == 0) */

    } /* End if "dspu debug" cmd issued */

} /* End dspu_debug_command() */
