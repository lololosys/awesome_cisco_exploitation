/* $Id: smterror.h,v 3.2 1995/11/17 19:19:48 gstovall Exp $
 * $Source: /swtal/cherf.111/ios/sys/xdi/smterror.h,v $
 *------------------------------------------------------------------
 * XDI include file.
 *
 * March, 1994  Walter R. Friedrich
 *
 * Copyright (c) 1993-1994 by cisco Systems, Inc.
 * All rights reserved.
 * 
 *------------------------------------------------------------------
 * $Log: smterror.h,v $
 * Revision 3.2  1995/11/17  19:19:48  gstovall
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:56:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:26:03  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
@(#) xm7_smterror.h xm7_v3.1.1:cset.0173:9:9 Fri Jan 21 10:51:29 1994
Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
*/
/*********************************************************************
		SMT Header File
		
		SMT Error File
			
		File:			xm7_smterror.h
		Created:		12/01/89

		Version:		xm7_v3.1.1		Fri Jan 21 10:51:29 1994
		Last Modified:	cset.0173		01/14/94
		
		Copyright 1990-1994 XLNT Designs, Inc., ALL RIGHTS RESERVED
		
		This header file is used by all SMT modules. This file contains
		error code definitions for used by all SMT modules.

		Modification History:
		*** SMT 7.3 ***
*********************************************************************/

/*********************************************************************
	System Error Definitions
*********************************************************************/

/*
*	Error codes are divided into sections based upon the process which
*	encounters the error. There are five sections: general errors, FSP
*	errors, CSP errors, MSP errors and interface errors.
*/
#define	GENERAL_ERROR			0x100
#define	FBM_ERROR				0x200
#define	CSP_ERROR				0x300
#define	MIB_ERROR				0x400
#define	IF_ERROR				0x500


/*
*	General error definitions
*/

/*
*	Frame service process error definitions
*/
 /* no frame buffers available */
#define	EFBM_NO_FRAME_BUF_MEM				(FBM_ERROR | 0x01)
 /* bad frame class requested in send frame message */
#define	EFBM_BAD_FRAME_CLASS				(FBM_ERROR | 0x03)
 /* bad frame type requested in send frame message */
#define	EFBM_BAD_FRAME_TYPE					(FBM_ERROR | 0x04)
 /* bad frame requested in send frame message */
#define	EFBM_BAD_FRAME_REQUESTED			(FBM_ERROR | 0x05)
 /* no messages available for FBM */
#define	EFBM_NO_MESSAGE						(FBM_ERROR | 0x06)
 /* message send failed from FBM */
#define	EFBM_MSG_FAILED						(FBM_ERROR | 0x07)
 /* no frames available for FBM */
#define	EFBM_NO_FRAMES						(FBM_ERROR | 0x08)
 /* FSP action queue empty */
#define	EFBM_Q_MT							(FBM_ERROR | 0x09)
 /* FSP action queue full */
#define	EFBM_Q_FULL							(FBM_ERROR | 0x0A)
 /* SBA invalid typeInfo command */
#define	EFBM_SBA_BAD_COMMAND				(FBM_ERROR | 0x0B)
 /* invalid MAC index for frame xmit */
#define	EFBM_BAD_MAC_INDEX					(FBM_ERROR | 0x0C)
 /* MAC not available for xmit */
#define	EFBM_MAC_NOT_AVAILABLE				(FBM_ERROR | 0x0D)
 /* SBA not supported on this path */
#define	EFBM_SBA_NOT_AVAILABLE				(FBM_ERROR | 0x0E)
 /* RAF not supported */
#define	EFBM_RAF_NOT_SUPPORTED				(FBM_ERROR | 0x0F)
 /* ESF not supported */
#define	EFBM_ESF_NOT_SUPPORTED				(FBM_ERROR | 0x10)


/*
*	Connection service process error definitions
*/
 /* signal queue is empty */
#define	ECSP_SIG_Q_MT						(CSP_ERROR | 0x01)
 /* signal queue filled */
#define	ECSP_SIG_Q_FULL						(CSP_ERROR | 0x02)
 /* no messages availabe for CSP */
#define	ECSP_NO_MESSAGE						(CSP_ERROR | 0x06)
 /* message send failed from CSP */
#define	ECSP_MSG_FAILED						(CSP_ERROR | 0x07)

/* XLNT Entry support */
 /* graceful insertion port queue empty */
#define	ECSP_GIPORT_Q_MT					(CSP_ERROR | 0x08)
 /* graceful insertion port not in queue */
#define	ECSP_GIPORT_NO_PORT					(CSP_ERROR | 0x09)
 /* error initializing graceful insertion */
#define	ECSP_GI_INIT						(CSP_ERROR | 0x0a)


/*
*	MIB service process error definitions
*/
 /* MIB memory could not be allocated */
#define	EMIB_INIT_MIB						(MIB_ERROR | 0x01)
 /* no messages availabe for MIB */
#define	EMIB_NO_MESSAGE						(MIB_ERROR | 0x02)
 /* message send failed from MIB */
#define	EMIB_MSG_FAILED						(MIB_ERROR | 0x03)
 /* resource table full */
#define	EMIB_TABLE_FULL						(MIB_ERROR | 0x04)
 /* resource table empty */
#define	EMIB_TABLE_MT						(MIB_ERROR | 0x05)
 /* duplicate resource ID */
#define	EMIB_DUP_ID							(MIB_ERROR | 0x06)
 /* resource ID not found */
#define	EMIB_ID_NOT_FOUND					(MIB_ERROR | 0x07)
 /* duplicate port type present */
#define	EMIB_DUP_PORT_TYPE					(MIB_ERROR | 0x08)
 /* invalid port type */
#define	EMIB_INVALID_PORT_TYPE				(MIB_ERROR | 0x09)
 /* invalid resource ID */
#define	EMIB_INVALID_ID						(MIB_ERROR | 0x0A)
