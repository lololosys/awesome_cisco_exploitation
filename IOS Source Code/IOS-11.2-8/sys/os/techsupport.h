/* $Id: techsupport.h,v 3.1.2.1 1996/04/15 14:59:18 widmer Exp $
 * $Source: /release/112/cvs/Xsys/os/techsupport.h,v $
 *------------------------------------------------------------------
 * Tech-Support header file
 *
 * April 1996, Robert Widmer
 *
 * Copyright (c) 1996, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: techsupport.h,v $
 * Revision 3.1.2.1  1996/04/15  14:59:18  widmer
 * CSCdi47180:  No Single Command to Collect General Router Data
 * Branch: California_branch
 * Add "show tech-support" command
 * Change references to encryption types to uint
 * Change tt_soc to use password_struct
 *
 * Revision 3.1  1996/04/12  19:08:49  widmer
 * Add placeholder files
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Supporting protocol types
 */
enum {
    TECHSUPPORT_NONE = 0,
    TECHSUPPORT_IPMULTICAST,	    /* PIM, IGMP and DVMRP */
    TECHSUPPORT_IP_RSVP,	    /* RSVP */
};

extern const char *techsupport_blank_string;
extern void show_techsupport_command(parseinfo *, const char *);
