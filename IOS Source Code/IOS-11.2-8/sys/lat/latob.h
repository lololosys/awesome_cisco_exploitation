/* $Id: latob.h,v 3.2 1995/11/17 17:34:06 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/latob.h,v $
 *------------------------------------------------------------------
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: latob.h,v $
 * Revision 3.2  1995/11/17  17:34:06  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:59  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:52  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    latob		define lat object block structure

    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO

    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved.
 -----------------------------------------------------------------------*/
#ifndef LATOB_H
#define LATOB_H		0		/* Indicate file has been read	*/

/*----------------------------------------------------------------------
   Reason codes for ox_session.
 -----------------------------------------------------------------------*/
#define	OX_START	1		/* Set start data		*/
#define OX_CONFIG_REP	2		/* Supply local data		*/

/*----------------------------------------------------------------------
    Define the object flags.
 -----------------------------------------------------------------------*/
#define OB_LOCAL	0x1		/* Object can initiate connects	*/
#define OB_REMOTE	0x2		/* Object can accept   connects	*/
#define OB_DYNAMIC	0x4		/* Object can switch hit	*/
#define OB_ENABLED	0x8		/* Object enabled for connects	*/
#define OB_QUEUED	0x10		/* Object can queue connects	*/
#define OB_CLONED	0x20		/* Object was cloned		*/
#define OB_TEMPLATE	0x40		/* Object is template		*/
#define OB_BUSY		0x80		/* Object is busy (environment)	*/
#define OB_DEL_PEND	0x100		/* Object delete pending	*/

/*----------------------------------------------------------------------
    Define the object block.
 -----------------------------------------------------------------------*/
    typedef struct ob
    {
	struct ob OB_VIEW(ob_link[2]);	/* Link to next object		*/
	oid_type ob_oid;		/* Object identifier		*/
	int	ob_flags;		/* Object characteristics	*/
	int	ob_lcl_use;		/* Current active local ses.	*/
	int	ob_lcl_max;             /* Maximum active local ses.	*/
	int	ob_rmt_use;		/* Current active remote ses.	*/
	int	ob_rmt_max;             /* Maximum active remote ses.	*/
	data_b	ob_lcl_data;		/* Local  object configuration	*/
	data_b	ob_tmp_data;		/* Temp	  object configuration	*/
	byte	ob_idl[GROUPS];		/* Groups enabled for object	*/
	byte	ob_name[OB_STR];        /* Object name			*/
    } ob_node, OB_VIEW(ob_ptr);

extern ob_node ob_dummy;
#endif
