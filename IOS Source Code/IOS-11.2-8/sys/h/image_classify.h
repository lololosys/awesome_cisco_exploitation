/* $Id: image_classify.h,v 3.2 1995/11/17 09:12:26 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/image_classify.h,v $
 *------------------------------------------------------------------
 * header file to support identification of image types
 *
 * Oct 1994, Eric Fronberg
 *
 * Copyright (c) 1994 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: image_classify.h,v $
 * Revision 3.2  1995/11/17  09:12:26  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:35:18  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:35:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__IMAGE_CLASSIFY_H__)
#define __IMAGE_CLASSIFY_H__
/*
 * defines used for image validations, either during download to
 * flash, or while booting from flash.
 */
#define AOUT_MAGIC 0x00020107
#define MOP_MAGIC  0xA8003000
#define COMPRESSED_MAGIC_1 0x1F9D
#define COMPRESSED_MAGIC_2 0x1F9E

#define UNKNOWN_IMAGE_OR_DATA_FILE 0x00
#define UNCOMPRESSED_68K_RFR_IMAGE 0x01
#define UNCOMPRESSED_68K_RFF_IMAGE 0x02
#define UNCOMPRESSED_MOP_RFR_IMAGE 0x03
#define UNCOMPRESSED_MOP_RFF_IMAGE 0x04
#define COMPRESSED_68K_IMAGE       0x05
#define COMPRESSED_68K_RFR_IMAGE   0x06
#define COMPRESSED_68K_RFF_IMAGE   0x07
#define COMPRESSED_MOP_RFR_IMAGE   0x08
#define COMPRESSED_MOP_RFF_IMAGE   0x09
#define UNCOMPRESSED_68K_REL_IMAGE 0x12
#define INVALID_REL_IMAGE          0xffffffff
#endif /* !defined(__IMAGE_CLASSIFY_H__) */
