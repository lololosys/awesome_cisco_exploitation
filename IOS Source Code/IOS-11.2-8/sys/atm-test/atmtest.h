/* $Id: atmtest.h,v 3.1 1995/11/09 11:02:58 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/atm-test/atmtest.h,v $
 *------------------------------------------------------------------
 * atmtest.h --  Atm test support
 *
 * August 1995,  Biren Fondekar
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log $
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __ATMTEST_H__
#define __ATMTEST_H__

/*
 * The Tcl interpreter
 */
extern Tcl_Interp *interp;

extern void atmtest_init(subsystype *);
extern void atmtest_create_commands(Tcl_Interp *);

#endif __ATMTEST_H__
