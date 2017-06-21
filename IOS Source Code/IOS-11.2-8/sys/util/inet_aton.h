/* $Id: inet_aton.h,v 3.2 1996/02/21 07:41:51 pst Exp $
 * $Source: /release/111/cvs/Xsys/util/inet_aton.h,v $
 *------------------------------------------------------------------
 * inet_aton.h - Almost-Berkeley-Compatible inet_aton routine
 *
 * Feburary 1996, Paul Traina
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: inet_aton.h,v $
 * Revision 3.2  1996/02/21  07:41:51  pst
 * CSCdi49395:  b.i.g.: forgot to remove extra includes from .h file
 *
 * Revision 3.1  1996/02/21  07:39:42  pst
 * Add cisco_inet_aton() to library
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Check whether "cp" is a valid ascii representation
 * of an Internet address and convert to a binary address.
 * Returns 1 if the address is valid, 0 if not.
 * This replaces inet_addr, the return value from which
 * cannot distinguish between failure and a local broadcast address.
 */
int cisco_inet_aton(const char *cp, ipaddrtype *addr);
