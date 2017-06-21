/* $Id: at_access.h,v 3.2 1995/11/17 08:41:32 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_access.h,v $
 *------------------------------------------------------------------
 * at_access.h -- definitions for Appletalk access control
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_access.h,v $
 * Revision 3.2  1995/11/17  08:41:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:56:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:07:24  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_ACCESS_H__
#define __AT_ACCESS_H__

/*
 * AppleTalk Access Parse Flags
 */
#define AT_RANGE 0x80		/* Cable range parse required */
#define AT_NETWORK 0x40		/* Network parse required */
#define AT_ZONE 0x20		/* Zone name parse required */
#define AT_NONO 0x01		/* NO APPLE ACCESS not permitted */

/*
 * AppleTalk Access Table Structure
 */
typedef struct atacctable_ {
    rbTree* networks;
    boolean inactive;
    boolean defaccess;
    boolean defbroadcastaccess;
    boolean defzoneaccess;
    boolean defzonebroadcastaccess;
    zipaccentry* zones;
    int zonecount;
    int number;
    nbpaccentry* nbpacclist;
} atacctable;

/*
 * AppleTalk Network/Cable-range Access Structure
 */
typedef struct atacctype_ {
    struct treeLink_ link;
    boolean grant;
    boolean grant_broadcasts;
    boolean exactmatch;
    boolean within;
    int keyword;
    ushort low;
    ushort high;
} atacctype;

/*
 * Operations for atacc_GetTable
 */
typedef enum createOps_ {
    createIt,
    mustExist
} createOps;

/*
 * Operations for atacc_ProtectNetwork
 */
typedef enum protectOps_ {
    protectIt,
    unprotectIt
} protectOps;


/* Prototypes */
boolean atacc_ACLUsesZone(ziptabletype*,int);

#endif __AT_ACCESS_H__

