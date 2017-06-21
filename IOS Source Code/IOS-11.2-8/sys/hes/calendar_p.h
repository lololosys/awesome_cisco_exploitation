/* $Id: calendar_p.h,v 3.3.44.1 1996/03/18 19:39:39 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/hes/calendar_p.h,v $
 *------------------------------------------------------------------
 * calendar_p.h - Real-Time Calendar public definitions and prototypes
 *
 * Copyright (c) 1992-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: calendar_p.h,v $
 * Revision 3.3.44.1  1996/03/18  19:39:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.14.1  1996/01/24  21:44:53  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Nuke piles of externs, start hiding driver specific stuff from the rest
 * of the system.
 *
 * Revision 3.3  1995/12/12  09:06:00  dbath
 * CSCdi45422:  HSA systems forget what time it is on failover
 * Whenever we write to the battery-backed calendar hardware on
 * the master, ship the value across to the slave's calendar too.
 *
 * Revision 3.2  1995/11/17  09:14:40  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:37:24  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/06/16  04:27:28  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.1  1995/06/07  20:37:54  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * $Endlog$
 */

/*
 * Calendar chip defines and structs
 */

struct calendar_t_ {
    unsigned    hund_seconds:8;
    unsigned    zeroes0:1;
    unsigned    seconds:7;
    unsigned    zeroes1:1;
    unsigned    minutes:7;
    unsigned    mode_12_24:1;
    unsigned    zeroes2:1;
    unsigned    hours:6;
    unsigned    zeroes4:2;
    unsigned    oscillator:1;
    unsigned    reset:1;
    unsigned    zeroes3:1;
    unsigned    day:3;
    unsigned    zeroes5:2;
    unsigned    date:6;
    unsigned    zeroes6:3;
    unsigned    month:5;
    unsigned    year:8;
};

extern void write_calendar(calendar_t *);
