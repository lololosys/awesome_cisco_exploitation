/* $Id: lack2_externs.h,v 3.2 1995/11/17 18:54:25 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/srt/lack2_externs.h,v $
 *------------------------------------------------------------------
 * lack2_externs.h -- Local Acknowledgement Finite State Machine Glue
 *
 * March 4, 1993, Claude Cartee and Eric B. Decker
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 * Longer description here.
 *------------------------------------------------------------------
 * $Log: lack2_externs.h,v $
 * Revision 3.2  1995/11/17  18:54:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:31:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:00:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LACK2_EXTERNS_H
#define __LACK2_EXTERNS_H

/*
 * l a k 2 _ g e t s t a t e
 *
 * return the current state for a lak2 lack entity
 */

static inline int lak2_getstate (lak_t *lackie)
{
    return(lackie ? lackie->lak2_state : l2st_undefined);
}

extern	char	*lak2_prstate(lak_t *lackie, l2st_t state);
extern	char	*lak2_prinput(l2ma_t major, int minor);
extern	l2mi_net_t lak2_net_categorize(int opcode);
extern	void	lak2_send_netmsg(lak_t *lackie, int opcode);
extern	void	lak2_butcher_piggie(lak_t *lackie);
extern	void	lak2_ctl_llc2(lak_t *lackie, int op);
extern	void	lak2_StartAckTimer(lak_t *lackie, int retries);
extern	void	lak2_ResumeAckTimer(lak_t *lackie);
extern	void	lak2_StopAckTimer(lak_t *lackie);
extern	ushort	lak2_fsm(lak_t *, l2ma_t, int);
extern	boolean lak2_xid_ok(paktype *pak);
extern	void	lak2_process_netmsg(paktype *pak, int opcode, tr_ventrytype *vre, dlc_t type);
extern  void    lack2_init(void);

#endif __LACK2_EXTERNS_H

