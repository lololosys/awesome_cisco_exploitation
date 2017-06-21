/* $Id: at_nbp.h,v 3.2 1995/11/17 08:43:16 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/atalk/at_nbp.h,v $
 *------------------------------------------------------------------
 * at_nbp.h -- definitions for Appletalk NBP protocol
 *
 * September 1991, David Edwards
 * 
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: at_nbp.h,v $
 * Revision 3.2  1995/11/17  08:43:16  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  10:57:25  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:50:15  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:08:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __AT_NBP_H__
#define __AT_NBP_H__
#include "../atalk/macip_util.h"
#include "../atalk/atalk_nbp.h"

/*
 * AppleTalk NBP Test command functions.
 */
enum {
    ATNBP_TEST_ADDCLIENT=1,
    ATNBP_TEST_CONFIRM,
    ATNBP_TEST_DELCLIENT,
    ATNBP_TEST_LOOKUP,
    ATNBP_TEST_PARAMETERS,
    ATNBP_TEST_POLL,
    ATNBP_TEST_REGISTER,
    ATNBP_TEST_STATISTICS,
    ATNBP_TEST_UNREGISTER,
    ATNBP_TEST_HELP
};
    

/*
 * AppleTalk NBP private structures
 */
typedef struct _NBPOps {
    int		opcode;
    ataddress	entity;
    short	nbpId;
    short       zhash;
    short	nbpEnum;
    struct { 
	uchar	*p;
	ulong	len;
    } name, type, zone;
} nbpOps;

/*
 * AppleTalk private prototypes
 */
boolean atnbp_DecodeOps(paktype*, nbpOps*);
boolean atnbp_DecodeTuple(uchar*, nbpOps*);

void nbp_init(void);
nbp_result nbp_find(nbpEntity *nve, ataddress *src, nbptimer *tm,
		    queuetype *replyQ);
char *atnbp_resultStr(nbp_result ii);
void atnbp_mkname(nbpEntity *nve, char *cname, char *ctype, char *czone);
char *atnbp_entityStr(nbpEntity *nve);
char *ddpAddrStr(ataddress *ddp);
void nbptest_init(void);

#endif __AT_NBP_H__
