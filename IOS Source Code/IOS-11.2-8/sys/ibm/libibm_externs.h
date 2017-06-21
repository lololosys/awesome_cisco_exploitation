/* $Id: libibm_externs.h,v 3.2.42.1 1996/05/17 11:20:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/ibm/libibm_externs.h,v $
 *------------------------------------------------------------------
 * libibm_externs.h - external declarations for functions defined in 
 *                    libibm_util.a
 * 
 * May 1995, Richard Livingston
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: libibm_externs.h,v $
 * Revision 3.2.42.1  1996/05/17  11:20:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.30.2  1996/04/11  14:37:46  fbordona
 * CSCdi51341:  miscellaneous SDLC cleanup for CLSI compliance
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2.30.1  1996/04/03  14:33:57  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.2  1995/12/23  00:54:25  kpatel
 * CSCdi46079:  DLSw+ does not support non-activation XID
 *
 * Revision 3.1  1995/11/09  11:45:15  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/10/19  08:09:19  bchan
 * CSCdi27730:  SNA Fragmentation break if no buffers available
 *
 * Revision 2.1  1995/08/07  05:11:02  richl
 * CSCdi38315:  makefile rework
 *              add new files for libraries
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * libibm.a consists of:
 *    ibm/peer_util.c
 *    ibm/sna_util.c
 */

/*
 * ibm/peer_util.c - peer utility routines for DLSw and RSRB
 */

#ifndef __LIBIBM_EXTERNS_H__
#define __LIBIBM_EXTERNS_H__
extern char *priority_peername (int priority_num);
extern char *tcpuser_open_fail_str (int stat);

/* 
 * ibm/sna_util.c - SNA BIU fragmentation routine
 */
extern boolean sna_th_frag(paktype *pak, ushort max_frag, queuetype* q);

typedef void (* SendFragProc)(void*, paktype*);
extern void sendqueue(queuetype* q, void (* sendfrag)(void *controlblock, paktype *pak), 
                      void* controlBlock);

extern boolean valid_XID(char *buffs, uchar *xid, int xid_size);
extern boolean is_xid3(paktype *pak);
extern boolean is_valid_xid(paktype *pak);
extern void xid3_glean_maxbtu( XID3type *xid, uchar *stash);
extern uchar xid_get_format(const uchar* ptr);
extern uchar xid_get_node_type(const uchar* ptr);
extern char xid_get_length(const uchar* ptr);
extern void xid3_set_abm(uchar* ptr, boolean abm);
extern uchar xid3_get_maxiframe(const uchar* ptr);
#ifdef NOBODY_CALLS_ME
extern void xid3_spoof_abm(paktype *pak, uchar *from_abm, uchar *to_abm);
extern boolean xid3_get_abm(const uchar* ptr);
extern void xid3_set_maxiframe(uchar* ptr, uchar maxiframe);
extern ushort xid3_get_maxbtu(const uchar* ptr);
extern void xid3_set_maxbtu(uchar* ptr, ushort maxbtu);
#endif /* NOBODY_CALLS_ME */
extern boolean xid_is_nonactivation(const uchar *ptr);
extern boolean xid3_is_nonactivation(const uchar *ptr);
#endif


