/* $Id: nv_msgid.h,v 3.2 1995/11/17 09:13:04 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/nv_msgid.h,v $
*------------------------------------------------------------------
 * SNA Network Management Message To Operator Message IDs
 *
 * Semptember 95, Bob Batz
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: nv_msgid.h,v $
 * Revision 3.2  1995/11/17  09:13:04  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:50  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:58:53  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/09/21  15:14:42  rbatz
 * Placeholder file for Ark_Ibm_branch
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef NV_MSGID_H
#define NV_MSGID_H

#define NV_OPER "OPRCISCO"

/* An enum would have been nice here, but it makes the
 * formatting somewhat awkward.  If a number is used, use
 * a define here or at least a comment telling where it is used.
 */

#define NVMSG_NONE                "NSP000I "
#define NVMSG_DspuPuDownActiv     "NSP001I  DSPU - ROUTER"
#define NVMSG_DspuPuDownInact     "NSP002I  DSPU - ROUTER"
#define NVMSG_DspuPuFailAct1      "NSP003I  DSPU -"
#define NVMSG_DspuPuFailAct2      "NSP004I  DSPU - ROUTER"
#define NVMSG_DspuPuFailActRsn1   "NSP005I  DSPU - REASON:"
#define NVMSG_DspuPuFailActRsn2   "NSP006I  DSPU - REASON:"
#define NVMSG_DspuPuFailActRsn3   "NSP007I  DSPU - REASON:"
#define NVMSG_DspuPuFailActRsn4   "NSP008I  DSPU - REASON:"
#define NVMSG_DspuPuFailActRsn5   "NSP009I  DSPU - REASON:"
#define NVMSG_DspuLuFailAct1      "NSP010I  DSPU -"
#define NVMSG_DspuLuFailAct2      "NSP011I  DSPU - ROUTER"
#define NVMSG_DspuLuFailActRsn1   "NSP012I  DSPU - REASON:"
#define NVMSG_DspuLuFailActRsn2   "NSP013I  DSPU - REASON:"

#endif





