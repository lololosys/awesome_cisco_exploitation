/* $Id: tnlogscat.h,v 3.1.4.1 1996/05/17 11:19:35 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/tnlogscat.h,v $
 *------------------------------------------------------------------
 * tn3270 server log
 *
 * Jan 96, Michael Boe
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tnlogscat.h,v $
 * Revision 3.1.4.1  1996/05/17  11:19:35  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.1.2.1  1996/03/21  03:21:51  bchan
 * CSCdi52147:  Add tn3270 server new files to IbuMod_Calif_branch
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.1  1996/03/21  01:13:39  bchan
 * placeholder for tn3270 server.
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
** ---NOTICE---: this file automatically generated from CIP repository.
** generated from tnlogdef.input on Thu Oct  3 23:21:30 US/Eastern 1996
**
*/
#ifndef _tnscatlog_h
#define _tnscatlog_h

int InitScatBindingsTn (
     long          given_event_index,
     const char*   fld_sem_string,
     unsigned int  fld_sem_string_size);
/*
** TN LOG DECODE/SCATTER FUNCTIONS
** */

unsigned long APPNBindFailScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     const void* vec_ptr,
     short vec_len);

unsigned long APPNProtErrScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     const void* vec_ptr,
     short vec_len);

unsigned long BadPIUScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     unsigned char* tgNumber,
     const void* vec_ptr,
     short vec_len);

unsigned long DLUSIsntScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     const void* vec_ptr,
     short vec_len);

unsigned long DLUSXSubNetScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     const void* vec_ptr,
     short vec_len);

unsigned long NoLuSessionsScatter (
     unsigned long* ipAddrLcl,
     unsigned short* tcpPortLcl,
     const void* vec_ptr,
     short vec_len);

unsigned long SystemScatter (
     unsigned char* systemStateTransition,
     const void* vec_ptr,
     short vec_len);

unsigned long XIDRejScatter (
     unsigned char* cpNameLcl,
     unsigned char* cpNameRem,
     unsigned long* snaSense,
     unsigned char* adapter,
     const void* vec_ptr,
     short vec_len);

unsigned long XIDRejIDScatter (
     unsigned long* idBlkNum,
     unsigned long* snaSense,
     unsigned char* adapter,
     const void* vec_ptr,
     short vec_len);

#endif
