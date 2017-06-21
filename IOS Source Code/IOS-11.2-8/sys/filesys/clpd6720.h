/* $Id: clpd6720.h,v 3.2 1995/11/17 09:07:17 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/clpd6720.h,v $
 *------------------------------------------------------------------
 * Include file for the CLPD6720 driver code in clpd6720.c
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: clpd6720.h,v $
 * Revision 3.2  1995/11/17  09:07:17  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:19  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:31:49  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__clpd6720_h__)
#define __clpd6720_h__

/*----------------------------------------------------------------------------*
 * Typedefs
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

/*----------------------------------------------------------------------------*
 * External references to CLPD6720 functions
 *----------------------------------------------------------------------------*/

extern void clpd6720_init (void);

extern void clpd6720_mgmt_intr_handler (void);

extern void clpd6720_set_window (clpd6720_socket_t socket, 
				 clpd6720_window_t window, 
				 ulong local_addr, 
				 ulong pcmcia_addr, 
				 ulong length, 
				 clpd6720_pcmcia_type_t pcmcia_type, 
				 clpd6720_window_width_t window_width, 
				 clpd6720_write_protect_t write_protect);

extern void clpd6720_set_window_timing (clpd6720_socket_t socket, 
					clpd6720_window_t window, 
					clpd6720_timing_t timing);

extern void clpd6720_set_timing (clpd6720_socket_t socket, 
				 clpd6720_timing_t timing_set, 
				 uchar setup, 
				 uchar command, 
				 uchar recovery);

extern void clpd6720_enable_window (clpd6720_socket_t socket, clpd6720_window_t window);

extern void clpd6720_disable_window (clpd6720_socket_t socket, clpd6720_window_t window); 

extern void clpd6720_reset_socket (clpd6720_socket_t socket);

extern void clpd6720_write_enable_socket (clpd6720_socket_t socket);

extern void clpd6720_write_protect_socket (clpd6720_socket_t socket);

extern void clpd6720_write_enable_window (clpd6720_socket_t socket,
					  clpd6720_window_t window);

extern void clpd6720_write_protect_window (clpd6720_socket_t socket,
					   clpd6720_window_t window);

extern void (*clpd6720_reg_write)(clpd6720_socket_t socket, uchar reg_index, uchar value);
extern uchar (*clpd6720_reg_read)(clpd6720_socket_t socket, uchar reg_index);

#endif /* !defined(__clpd6720_h__) */






