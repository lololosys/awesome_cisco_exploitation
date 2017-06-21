/* $Id: pakgen.h,v 3.1 1995/11/09 12:59:04 shaker Exp $
 * $Source: /swtal/cherf.111/ios/sys/pgen/pakgen.h,v $
 *------------------------------------------------------------------
 * pakgen.h   MAC packet generator support 
 *
 * February 1992,  Anson Chen
 *
 * Copyright (c) 1992-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log $
 *------------------------------------------------------------------
 * 
$Endlog$
 */

#ifndef __PAKGEN_H__
#define __PAKGEN_H__

extern boolean pakgen_common(long *, long *, long, long, long *);
extern boolean pakhexval(char *, char *);
extern void pakgen_proto(hwidbtype *);
extern void send_tr_pak(hwidbtype *, uchar *, uchar *, int, char *, long);
extern void send_ether_pak(hwidbtype *, uchar *, char *, long);
extern void send_fddi_pak(hwidbtype *, uchar *,  char *, long);
extern void pakgen_init(subsystype *);
extern void pakgen_parser_init(void);

#endif __PAKGEN_H__
