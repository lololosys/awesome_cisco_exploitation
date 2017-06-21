/* $Id: ralib.h,v 3.2 1995/11/17 09:08:31 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/ralib.h,v $
 *------------------------------------------------------------------
 * Include file for the RALIB (Raw Access Library) for flash file system
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: ralib.h,v $
 * Revision 3.2  1995/11/17  09:08:31  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:33  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/07/07  22:57:09  yanke
 * Detect device write protect switch, update it in device structure.
 * Check the device for protection in all write-related operations.
 * Reduce the time to wait for vpp on valid bit.
 *
 * CSCdi36427:  write protecting flash credit card hangs the router
 *
 * Revision 2.1  1995/06/07  20:32:51  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(__ralib_h__)
#define __ralib_h__

/*----------------------------------------------------------------------------*
 * constants for use in the "mode" parameter to the ralib_control() function
 *----------------------------------------------------------------------------*/
#define RALIB_RESET_DEVICE  0
#define RALIB_VPGM_OFF      1
#define RALIB_VPGM_VCC      2
#define RALIB_VPGM_12V      3
#define RALIB_WRITE_PROTECT 4

/*----------------------------------------------------------------------------*
 * RALIB error codes
 *----------------------------------------------------------------------------*/
#define RALIB_DEVICE_FIRST_OK               3
#define RALIB_DEVICE_STILL_OK               2
#define RALIB_BUSY                          1
#define RALIB_OK                            0
#define RALIB_ERROR                        -1
#define RALIB_ERROR_INVALID_DEVICE_ID      -2
#define RALIB_ERROR_INVALID_FLASH_PTR      -3
#define RALIB_ERROR_INVALID_DEV_NUM        -4
#define RALIB_ERROR_INVALID_MODE           -5
#define RALIB_ERROR_NO_DEVICE              -6
#define RALIB_ERROR_DEVICE_NEEDS_INIT      -7
#define RALIB_ERROR_DEVICE_NOT_READY       -8
#define RALIB_ERROR_DEVICE_NOT_WRITABLE    -9
#define RALIB_ERROR_VPGM_NOT_SUPPORTED     -10

/*----------------------------------------------------------------------------*
 * RALIB vector table
 *----------------------------------------------------------------------------*/
typedef struct ralib_vector_table_t {
    int   (*read)(int dev_num, ulong flash_ptr, void *buffer_ptr, int length);
    int   (*write)(int dev_num, void *buffer_ptr, ulong flash_ptr, int length);
    int   (*status)(int dev_num);
    int   (*control)(int dev_num, int mode);
    void  (*yield)(int microseconds);
    ulong (*time)(void);
    int   (*device_init)(int dev_num);
    int   (*device_reset)(int dev_num);
    const char *(*get_device_names)(void);
    int   (*get_dev_num)(const char *device_id);
    int   (*attrib_read)(int dev_num, ulong flash_ptr, void *buffer_ptr, int length);
    int   (*attrib_write)(int dev_num, void *buffer_ptr, ulong flash_ptr, int length);
} ralib_vector_table_t;

/*----------------------------------------------------------------------------*
 * External prototypes of the RALIB functions.
 *   These functions are in the RALIB vector table.
 *----------------------------------------------------------------------------*/
extern  ralib_vector_table_t  *ralib_init(void);
extern  int   ralib_read(int dev_num, ulong flash_ptr, void *buffer_ptr, int length);
extern  int   ralib_write(int dev_num, void *buffer_ptr, ulong flash_ptr, int length);
extern  int   ralib_status(int dev_num);
extern  int   ralib_control(int dev_num, int mode);
extern  void  ralib_yield(int microseconds);
extern  ulong ralib_time(void);
extern  int   ralib_device_init(int dev_num);
extern  int   ralib_device_reset(int dev_num);
extern  int   ralib_get_dev_num(const char *device_id);
extern  int   ralib_attrib_read(int dev_num, ulong flash_ptr, void *buffer_ptr, int length);
extern  int   ralib_attrib_write(int dev_num, void *buffer_ptr, ulong flash_ptr, int length);
extern  const char *ralib_get_device_names(void);

extern ushort rsp_disable_io_write_buffer(void);
extern void rsp_restore_io_write_buffer(ushort dcl_asic_reg);

/*----------------------------------------------------------------------------*
 * External prototypes of the RALIB functions that are testability hooks.
 *   None of these functions are in the RALIB vector table.
 *   All the functions that begin "ralib_unix_" are only available on the 
 *   Unix version of the RALIB.
 *----------------------------------------------------------------------------*/
/*-------- testability hooks --------*/
extern  void  ralib_remove_device(int dev_num);
extern  void  ralib_insert_device(int dev_num);
/*-------- testability hooks only available on the Unix RALIB --------*/
extern  void  ralib_unix_set_file_name(int dev_num, char *file_name);
extern  void  ralib_unix_make_sector_bad(int dev_num, int sector_num);

#endif /* !defined(__ralib_h__) */

