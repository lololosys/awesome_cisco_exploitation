/* $Id: dialer_profile.h,v 3.1.2.1 1996/04/27 06:36:19 syiu Exp $
 * $Source: /release/112/cvs/Xsys/wan/dialer_profile.h,v $
 *------------------------------------------------------------------
 * Dialer profiles definitions.
 *
 * March 1996 - Lionel Bustini
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: dialer_profile.h,v $
 * Revision 3.1.2.1  1996/04/27  06:36:19  syiu
 * Branch: California_branch
 * Commit Dial1_branch
 *
 * Revision 3.1  1996/04/22  22:52:35  syiu
 * Placeholder for Dial1_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * dialer_profile.c
 */
extern static dialerdbtype *dialer_fsm_idle(*ddb, *dpak);
extern static dialerdbtype *dialer_fsm_pending(*ddb, *dpak);
