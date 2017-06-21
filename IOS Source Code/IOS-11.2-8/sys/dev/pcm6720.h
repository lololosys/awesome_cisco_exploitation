/* $Id: pcm6720.h,v 3.2 1995/11/17 09:01:34 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/dev/pcm6720.h,v $
 *------------------------------------------------------------------
 * PCMCIA controller header file: structures and externs
 *
 * Jan   95    Mick Henniger (stolen from Jack Jenney)
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: pcm6720.h,v $
 * Revision 3.2  1995/11/17  09:01:34  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:20:44  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:25:26  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/******************************************************************************
 * Name:
 *
 * Description:
 *
 *
 ******************************************************************************/

#if !defined(__CLPD6720_H__)
#define __CLPD6720_H__

/*----------------------------------------------------------------------------*
 * 
 *----------------------------------------------------------------------------*/
typedef enum clpd6720_socket_t {
                 CLPD6720_SOCKET_0,
                 CLPD6720_SOCKET_1
} clpd6720_socket_t;


typedef enum clpd6720_window_t {
                 CLPD6720_WINDOW_0,
                 CLPD6720_WINDOW_1,
                 CLPD6720_WINDOW_2,
                 CLPD6720_WINDOW_3,
                 CLPD6720_WINDOW_4,
} clpd6720_window_t;

typedef enum clpd6720_pcmcia_type_t {
                 CLPD6720_COMMON_MEM,
                 CLPD6720_ATTRIB_MEM
} clpd6720_pcmcia_type_t;

typedef enum clpd6720_window_width_t {
                 CLPD6720_WIDTH_16_BIT,
                 CLPD6720_WIDTH_8_BIT,
} clpd6720_window_width_t;

typedef enum clpd6720_write_protect_t {
                 CLPD6720_READ_ONLY,
                 CLPD6720_READ_WRITE
} clpd6720_write_protect_t;

typedef enum clpd6720_timing_t {
                 CLPD6720_TIMING_0,
                 CLPD6720_TIMING_1
} clpd6720_timing_t;

typedef enum clpd6720_devtype_t {
		 PCM_DEV_UNKNOWN,
		 PCM_DEV_FLASH,
		 PCM_DEV_SRAM,
		 PCM_DEV_NO_ATTR
} clpd6720_devtype_t;

/*****************************************************************************
 * External references to CLPD6720 functions
 *****************************************************************************/

extern boolean pcmcia_card_present(dev_info_t *dev);
extern uchar pcmcia_status_change(dev_info_t *dev);
extern void pcmcia_init (dev_info_t *dev);
extern uchar pcm_device_type(dev_info_t *dev);
extern ulong pcm_flash_size(dev_info_t *dev);
extern ushort pcm_flash_chips(dev_info_t *dev);


extern void clpd6720_reg_write (clpd6720_socket_t, uchar, uchar);
extern uchar clpd6720_reg_read (clpd6720_socket_t, uchar );

#endif /* !defined(__CLPD6720_H__) */



