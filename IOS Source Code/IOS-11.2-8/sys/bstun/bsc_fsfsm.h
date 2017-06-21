/* $Id: bsc_fsfsm.h,v 3.3.30.2 1996/08/23 23:42:19 jtsao Exp $
 * $Source: /release/112/cvs/Xsys/bstun/bsc_fsfsm.h,v $
 *------------------------------------------------------------------
 * bsc_fsfsm.h - BSC Frame Sequencing FSM definition.
 *
 * August 1994, James Balestriere
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * Prepared by Metaplex.
 * All rights reserved.
 *
 * Implementation of FSM to ensure correct frame sequence at the 
 * BSC protocol level.
 * 
 *------------------------------------------------------------------
 * $Log: bsc_fsfsm.h,v $
 * Revision 3.3.30.2  1996/08/23  23:42:19  jtsao
 * CSCdi52178:  BSC passthru resets NS counts on every poll sequence
 * Branch: California_branch
 *
 * Revision 3.3.30.1  1996/05/17  10:40:18  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.3.18.1  1996/04/22  08:57:35  jbalestr
 * CSCdi54541:  system gets stuck in BSC-3-BADLINESTATE
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.3  1996/01/11  18:26:47  jbalestr
 * CSCdi46666:  BSC multidrop, controller never gets deselected
 * Change primary fsm to discard before passing packet as fsm input.
 *
 * Revision 3.2  1995/11/17  08:48:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:03:35  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/26  06:17:41  arothwel
 * CSCdi37680:  Fix BSC BADSHARE problem. Fix BSC FDX cfg prob. Fix BSC
 * dbg/sh cmd.
 *
 * Revision 2.1  1995/06/07  20:13:43  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __BSC_FSFSM_H
#define __BSC_FSFSM_H

typedef enum FS_frame {
        LOCAL_PS_ENQ ,
        LOCAL_ENQ    , REMOTE_ENQ ,
        LOCAL_EOT    , REMOTE_EOT ,
        LOCAL_FRAME  , REMOTE_FRAME ,
        LOCAL_HANGUP , REMOTE_HANGUP,
        LINK_DOWN    , LINK_UP, STATE_RESET
        } FS_frame;

extern char*
bsc_map_fs_state(int role, int state);

extern void
pass_to_frame_sequencer(FS_frame, hwidbtype *idb, int transmitting);

#endif  /* __BSC_FSFSM_H. */
/*---- end include file: bsc_fsfsm.h. */
