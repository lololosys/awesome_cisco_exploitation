/* $Id: lat.h,v 3.3.32.1 1996/07/29 19:02:15 snyder Exp $
 * $Source: /release/112/cvs/Xsys/lat/lat.h,v $
 *------------------------------------------------------------------
 * lat.h -- cisco user level LAT code, interfaces to meridian portable LAT 
 *
 * November 1989, Bill Westfield
 *
 * Copyright (c) 1989-1997 by cisco Systems, Inc.
 * LAT technology licensed from Meridian Technology, Copyright 1989.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lat.h,v $
 * Revision 3.3.32.1  1996/07/29  19:02:15  snyder
 * CSCdi64402:  declare arrays in lat that should be const, const
 *              save 1084 data, 68 out of image
 * Branch: California_branch
 *
 * Revision 3.3  1996/01/10  06:49:30  syiu
 * CSCdi41302:  Concurrent LAT & bridging for HDLC and PPP
 *
 * Revision 3.2  1995/11/17  17:33:25  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:22  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:24:36  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __LAT_H__
#define __LAT_H__

#include "../lat/mtcsys.h"
#include "connect.h"
#include "termqueue.h"
#include "../lat/laths.h"
#include "ttysrv.h"

/*
 * Flags passed to lat_open_connection
 */
#define LATOPEN_QUIET 0x0001
#define LATOPEN_DEBUG 0x0002

/*
 * lat.c
 */
extern conntype *lat_open_connection(tt_soc *, byte name[SV_STR],
				     byte *, byte *, int, boolean *);
extern int lat_run(conntype *, boolean);
extern void lat_close(conntype *);
extern void lat_status(sb_ptr, nd_ptr);
extern int lat_unread(conntype *);
extern boolean lat_alive(conntype *);
extern boolean lat_on_idb(idbtype *);
extern void show_lat_sessions(int);
extern void lat_perror(void);
extern const char parity_tolat[];
extern const int parity_fromlat[];
extern boolean lat_openBLOCK(conntype *);
extern boolean lat_datab(conntype *);
extern boolean lat_report(conntype *);
extern char *lat_circuit_error(int);

/*
 * latd.c
 */
extern void lat_print_line(tt_soc *);
extern int lat_interfaces;
extern connvector lat_connvector;
extern void show_lat_nd(nd_ptr);
extern void show_lat_as(void);
extern void show_lat_groups(void);
extern void show_lat_ls(void);
extern void show_lat_traffic(void);
extern void lat_create_process(void);
extern boolean lat_getdatabuf(conntype *);
extern int lat_local_services_offered(void);

/*
 * latvty.c
 */

extern void lat_info_datab(conntype *, data_b *);
extern void lat_start_host(sb_ptr);
extern boolean lat_start_hic(hi_ptr);
extern queuevector lat_queuevector;
extern void lat_host_init(void);
extern vectortype LATvector;

/*
 * latparse.c
 */
void nv_lat_line(tt_soc *);
void nv_latif(idbtype *);
extern int create_lat_name (char *, byte *, int, char *);
conntype *lat_multiproto_open(parseinfo *, boolean *);
extern regexpacctype *lat_accesslists[];

#ifdef LATND_H

/*
 * latshow.c
 */
extern void declat_show_ls_summary(ls_ptr ls, nd_ptr nd, char *hdr);
extern void declat_show_one_ls(char *args);
extern void declat_show_ls(parseinfo *);
extern void declat_show_node_counters(nd_ptr nd, char *hdr_ptr);
extern void declat_show_node_status(nd_ptr nd, char *hdr_ptr);
extern void declat_show_node_summary(nd_ptr nd, char *hdr_ptr);
extern void declat_show_nodes(char *name, void (*funct_ptr )(nd_ptr, char *));

#endif

/*
 * latcom.c
 */

extern int lat_error;
extern int tics_uncounted;
extern of_ptr of_local[2];
extern word dst_addr[3];
extern word src_addr[3];
extern hs_ptr hs;
extern void latcom_init(void);
extern int lat_host_delay;

/*
 * ifmsg.c
 */

extern paktype *lat_currentpak;

/*
 * lat_chain.c
 */

extern void lat_parser_init(void);

/*
 * lat_setup.c
 */

extern void lat_setup_global(setup_struct *);
extern void lat_setup_int(setup_struct *);
extern void lat_setup_end(setup_struct *);

#endif __LAT_H__
