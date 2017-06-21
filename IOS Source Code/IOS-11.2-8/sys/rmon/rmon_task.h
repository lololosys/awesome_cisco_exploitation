/* $Id: rmon_task.h,v 3.3 1996/02/09 07:57:21 jjohnson Exp $
 * $Source: /release/111/cvs/Xsys/rmon/rmon_task.h,v $
 *------------------------------------------------------------------
 * rmon_task.h:  Remote Monitoring MIB Packet Processing Task
 *
 * August 1995, Jeffrey T. Johnson
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: rmon_task.h,v $
 * Revision 3.3  1996/02/09  07:57:21  jjohnson
 * CSCdi48524:  Need mechanism to display RMON tables
 * parse chains are in place.  checkpoint work so that anke can take over.
 *
 * Revision 3.2  1995/11/17  18:39:18  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:03:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

struct rmon_t_ {
    ulong	rmon_mode;
    ulong	rmon_errors;
    ulong	rmon_collisions;
    ulong	rmon_drops;
    ulong	rmon_serialnum;
};

extern void rmon_parser_init (void);
extern void rmon_global_command (parseinfo *csb);
extern void rmon_int_command (parseinfo *csb);

extern void show_rmon_etherstats (void);
extern void show_rmon_history (void);
extern void show_rmon_hosts (void);
extern void show_rmon_topn (void);
extern void show_rmon_matrix (void);
extern void show_rmon_filters (void);
extern void show_rmon_capture (void);
