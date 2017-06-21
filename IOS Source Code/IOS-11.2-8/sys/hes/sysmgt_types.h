/* $Id: sysmgt_types.h,v 3.2.60.1 1996/05/17 11:19:30 ppearce Exp $
 * $Source: /release/112/cvs/Xsys/hes/sysmgt_types.h,v $
 *------------------------------------------------------------------
 * sysmgt_types.h
 *
 * May 1995, Mike Otto
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: sysmgt_types.h,v $
 * Revision 3.2.60.1  1996/05/17  11:19:30  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.2.48.1  1996/03/28  11:28:42  bchan
 * CSCdi52724:  merge tn3280 server code
 * Branch: IbuMod_Calif_branch
 *
 * Revision 3.2  1995/11/17  09:21:22  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:43:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  21:01:20  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/06/07  20:45:40  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#ifndef __SYSGMGT_TYPES_H__
#define __SYSGMGT_TYPES_H__

/*
 * SYSMGT classes
 */
#define SYSMGT_TCP_IP           0    /* TCP/IP tables */
#define SYSMGT_LLC            1    /* LLC tables */
#define SYSMGT_CSNA           2    /* CSNA tables */
#define SYSMGT_APPL_VERSION   3    /* CIP application version tables */
#define SYSMGT_TN3270S        4    /* TN3270 server tables */
#define SYSMGT_APPN           5    /* APPN  tables */
#define SYSMGT_DLUR           6    /* DLUR  tables */
#define SYSMGT_CHAN          10    /* Channel Traps */
#define SYSMGT_MAX_CLASS     11    /* Always keep last in table */
 
typedef uint sysmgt_class_t;
   

/*
 *   NETMGT header control values
 */
#define NETMGT_TRAP     0
#define NETMGT_REQUEST  1
#define NETMGT_RESPONSE 2

/*
 * NETMGT header operation values
 */
#define NETMGT_SET            0
#define NETMGT_GET_MATCH      1
#define NETMGT_GET_NEXT       2
#define NETMGT_MAX_OPERATIONS 3

/*
 * NETMGT header ret_code values
 */
#define NETMGT_SET_OK       0
#define NETMGT_FOUND        1
#define NETMGT_NOT_FOUND    2
#define NETMGT_NO_RESOURCES 3

typedef struct netmgt_header_ {
    struct {
        ushort control     :2;      /* control (request/response/trap) */
	ushort operation   :2;      /* operation (set/get/getnext) */
	ushort ret_code    :4;      /* return code (set ok/found/notfound) */
	ushort reserved2   :8;      /* Reserved */
    } flags;
    ushort rsrvd1;                  /* reserved for alignment */
    sysmgt_class_t class;           /* class */
    ushort slot;                    /* slot of the IP */
    ushort type;                    /* type of data within class */
    ushort index_length;            /* length of indices */
    ushort data_length;             /* length of data */
    void  *data;                    /* pointer to buffer */
} netmgt_header_t;

#define NETMGT_HDR_SIZE  (sizeof(netmgt_header_t))

#endif
