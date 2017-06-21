/* $Id: laterr.h,v 3.2 1995/11/17 17:33:52 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/lat/laterr.h,v $
 *------------------------------------------------------------------
 * $Log: laterr.h,v $
 * Revision 3.2  1995/11/17  17:33:52  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  12:26:45  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  21:25:28  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*-----------------------------------------------------------------------
    laterr		LAT engine error definitions
    
    Copyright 1989,1990	Meridian Technology Corporation, St. Louis, MO
    
    This material is confidential information of Meridian Technology
    Corporation. Furnished under license. All rights reserved. 
 -----------------------------------------------------------------------*/
#ifndef LATERR_H
#define LATERR_H	0		/* Indicate file has been read	*/

/*-----------------------------------------------------------------------
    Define protocol error codes.
 -----------------------------------------------------------------------*/
#define	LAT_ERROR	0x0000		/* LAT protocol error		*/

/*-----------------------------------------------------------------------
    Define facility error codes.
 -----------------------------------------------------------------------*/
#define	AS_ERROR	0x0001		/* Advertised service error	*/
#define LS_ERROR	0x0002		/* Learned service error	*/
#define	ND_ERROR	0x0003		/* Node structure error		*/
#define OB_ERROR	0x0004		/* Object structure error	*/
#define SB_ERROR	0x0005		/* Session structure error	*/
#define CB_ERROR	0x0006		/* Circuit structure error	*/
#define HI_ERROR	0x0007		/* Host-initiated error		*/
#define QR_ERROR	0x0008		/* Queued request error		*/
#define HS_ERROR	0x000f		/* Local host error		*/

/*-----------------------------------------------------------------------
    Define generic error codes.
 -----------------------------------------------------------------------*/
#define	NO_MEM		0x0010		/* Memory allocation failure	*/
#define	NO_NAME		0x0020		/* Name not found		*/
#define	NO_NEXT		0x0030		/* Next name not found		*/
#define	NU_NAME		0x0040		/* Non-unique name		*/
#define NO_IDENT	0x0050		/* Identifier not found		*/
#define NU_IDENT	0x0060		/* Non-unique identifier	*/
#define	MAX_SPACE	0x0070		/* Reached limit of space	*/
#define	MAX_LIMIT	0x0080		/* Reached limit of struct	*/
#define MAX_SES		0x0090		/* Reached limit of sessions	*/
#define OP_DISABLED	0x00a0		/* Operation disabled		*/
#define	OP_ILLEGAL	0x00b0		/* Operation illegal		*/
#define INV_ARG		0x00c0		/* Invalid argument		*/
#define BAD_CLASS	0x00d0		/* Invalid service class	*/
#endif
