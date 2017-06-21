/* $Id: sscop_public.h,v 3.2.56.1 1996/05/21 21:06:16 jwjang Exp $
 * $Source: /release/112/cvs/Xsys/atm/sscop_public.h,v $
 *----------------------------------------------------------------------
 *
 * S S C O P _ P U B L I C . H
 *
 * Service Specific Connection Oriented Protocol public header file.
 *
 * November 1993     Percy P. Khabardar
 *
 * Copyright (c) 1993-1997 by cisco Systems, Inc.
 * All rights reserved.
 *---------------------------------------------------------------------
 * $Log: sscop_public.h,v $
 * Revision 3.2.56.1  1996/05/21  21:06:16  jwjang
 * CSCdi58013:  sscop should send end pdu when shut atm interface
 * Branch: California_branch
 *
 * Revision 3.2  1995/11/17  08:47:28  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:01:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/08/08  01:06:27  percyk
 * CSCdi38366:  move processes to atmsig
 *
 * Revision 2.2  1995/08/07  19:14:33  percyk
 * CSCdi38338:  q messages on the correct output process.
 *
 * Revision 2.1  1995/06/07  20:13:07  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *---------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __SSCOP_PUBLIC_H__
#define __SSCOP_PUBLIC_H__


#define SSCOP_HEADER_SIZE         4

/*
 * Open return codes
 */
typedef enum _sscop_openCode {
   SSCOP_OPEN_OK,
   SSCOP_OPEN_NOMEMORY,
   SSCOP_OPEN_INPROGRESS,
   SSCOP_OPEN_INVALIDPARAM,
} sscop_openCode;


/*
 * E x t e r n s
 *
 */

extern pid_t sscop_output_pid; 
extern pid_t atmsig_timer_pid; 
extern sscop_openCode sscop_open(idbtype *);
extern void sscop_close(hwidbtype *);
extern boolean sscop_write(paktype *);
extern void sscop_showSscop(void);
extern void sscop_globalResetStats(void);
extern void sscop_interfaceResetStats(hwidbtype *);
extern void sscop_enableSscop(void);
extern void sscop_disableSscop(void);
extern void saal_enqueue(paktype *, vc_info_t *);
extern void sscop_enqueue(paktype *, boolean);

#endif __SSCOP_PUBLIC_H__

