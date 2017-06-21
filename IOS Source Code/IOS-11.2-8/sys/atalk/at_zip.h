/* $Id: at_zip.h,v 3.2.60.1 1996/03/23 01:28:02 slin Exp $
 * $Source: /release/111/cvs/Xsys/atalk/at_zip.h,v $
 *------------------------------------------------------------------
 * at_zip.h -- definitions for Appletalk ZIP protocol
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_zip.h,v $
 * Revision 3.2.60.1  1996/03/23  01:28:02  slin
 * Branch: California_branch
 *
 * AppleTalk makes its exodus from the idb.
 *
 * Revision 3.2  1995/11/17  08:44:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:58:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:50:44  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/07/13  06:50:18  hampton
 * Convert Appletalk to fully use the passive timers macros instead of
 * referencing the system clock directly.  [CSCdi37037]
 *
 * Revision 2.1  1995/06/07 20:09:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_ZIP_H__
#define __AT_ZIP_H__


extern ziptabletype *zip_table[];

/*
 * AppleTalk private prototypes
 */

boolean 	atzip_SendGetNetInfo(atalkidbtype*);
void 		atzip_SendQuery(atalk_rdb_t*);
ziptabletype *  atzip_CreateZone(uchar*,uint);
ziptabletype *  atzip_AddZone(atalk_rdb_t*,uchar*,uint);
ziptabletype * 	atzip_FindZone(uchar*,uint);

void 		atzip_Initialize(void);
void 		atzip_StatusReport(void);
void 		atzip_WriteNV(void);
ulong		atzip_ZoneCount(void);
void 		atzip_NeedsGC(void);
void 		atalk_zip(void);
void 		zip_notify(paktype*);
void 		zip_ans_getnetinfo(paktype*);
void 		zip_validate_netinfo(paktype*);
void 		zip_ans_query(paktype*);
void		zip_FinishMerge(atalk_rdb_t *, sys_timestamp);
ziptabletype *  zip_GetNextZone(atalkidbtype *, ziptabletype *, int *);

#endif __AT_ZIP_H__
