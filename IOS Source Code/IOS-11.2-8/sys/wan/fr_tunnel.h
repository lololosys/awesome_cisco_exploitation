/* $Id: fr_tunnel.h,v 3.1.22.1 1996/04/25 23:24:02 ronnie Exp $
 * $Source: /release/112/cvs/Xsys/wan/fr_tunnel.h,v $
 *------------------------------------------------------------------
 * Frame Relay IP Tunneling
 *
 * January 1996, Charles Poe
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fr_tunnel.h,v $
 * Revision 3.1.22.1  1996/04/25  23:24:02  ronnie
 * CSCdi55766:  Hey!  This ATM and Frame Relay is no different than in 11.1
 * Feature commit for ATM and Frame Relay into 11.2
 * Branch: California_branch
 *
 * Revision 3.1.30.1  1996/04/24  04:11:40  shankar
 * Branch: ATM_Cal_branch
 * Patch FR_SVC_branch changes
 *
 * Revision 3.1.2.2  1996/04/16  22:10:11  ccpoe
 * Code review comments from Shankar, Rene, and Paulina
 * Branch: FR_SVC_branch
 *
 * Revision 3.1.2.1  1996/02/22  01:12:54  ccpoe
 * Frame Relay IP tunnel
 * Branch: FR_SVC_branch
 *
 * Revision 3.1  1996/02/13  23:41:45  ccpoe
 * Frame Relay IP Tunneling
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
# define FR_TUNNEL 0x0
# define FR_TUNNEL_DLCI 0x00
# define FR_TUNNEL_TIMER 0x0
# define FR_TUNNEL_ADD 0x0
# define FR_TUNNEL_REMOVE 0x1
# define FR_TUNNEL_FIND 0x2
# define TUNNEL_LMI_INTERVAL 20

extern void set_tunnel_pvc_status (void);
extern int get_tunnel_pvcs (hwidbtype *);
extern void fr_init_tunnel_stats (hwidbtype *);
extern void fr_switch_tunnel_init (subsystype *);



