/* $Id: mib.h,v 1.3 1996/06/07 21:40:27 jliu Exp $
 * $Source: /cscka/t1_neat/src/t1/mib.h,v $
 *------------------------------------------------------------------
 * mib.h  NEAT MIB defines
 *
 * October, 1995. Joe Liu
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mib.h,v $
 * Revision 1.3  1996/06/07 21:40:27  jliu
 *     o fixed controller was up but loss of signal was detected problem
 *     o added new sub-controller command for esf fdl exchange setting
 *
 * Revision 1.2  1996/04/29  03:35:07  jliu
 *     o clean up some code
 *     o updated loopback bit of the line status when local loopback
 *       is being performed
 *
 * Revision 1.1.1.1  1996/02/10  03:22:52  towu
 * Create
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef HAVE_MIB_H
#define HAVE_MIB_H

/* for dsx1LineType */
#define DSX1_OTHER_FRAME	1
#define DSX1_ESF		2
#define DSX1_SF			3

/* for Line Coding */
#define DSX1_B8ZS		2
#define DSX1_AMI		5
#define DSX1_OTHER_CODE		6

/* for dsx1LoopbackConfig */
#define DSX1_NO_LOOP		1
#define DSX1_PAYLOAD_LOOP	2
#define DSX1_LINE_LOOP		3
#define DSX1_OTHER_LOOP		4

/* for dsx1Fdl */
#define DSX1_FDL_ANSI           2
#define DSX1_FDL_ATT            4
#define DSX1_FDL_NONE           8 

/* for dsx1LineStatus ( these are a bitmask ) */
#define DSX1_NO_ALARM		0x0001
#define DSX1_RCV_REM_ALM	0x0002
#define DSX1_XMT_REM_ALM	0x0004
#define DSX1_RCV_AIS		0x0008
#define DSX1_XMT_AIS		0x0010
#define	DSX1_FRAME_LOSS		0x0020
#define DSX1_LOS		0x0040
#define DSX1_LOOPED		0x0080
#define DSX1_T16_AIS		0x0100
#define DSX1_RCV_REM_LOMF	0x0200
#define DSX1_XMT_REM_LOMF	0x0400
#define DSX1_RCV_TEST_CODE	0x0800
#define DSX1_OTHER_FAILURE	0x1000
#define DSX1_HAVE_ERRORS	0x1ffe

extern void SetMIBLineStatus(LineStatus *stptr, ushort mask);
extern void ClearMIBLineStatus(LineStatus *stptr, ushort mask);
extern void Cleardsx1Table(dsx1TableType *p);
extern void ClearTotalTable(dsx1TotalTableType *p);
extern void ClearMIBData(T1InfoType *t1);

#endif    /* HAVE_MIB_H */
