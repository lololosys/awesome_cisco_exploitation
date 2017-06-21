/* $Id: flash_filesys.c,v 3.2.60.1 1996/03/18 19:35:20 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/flash_filesys.c,v $
 *------------------------------------------------------------------
 * Flash file system sub-system and initialization code
 *
 * July 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: flash_filesys.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:20  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:35  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:38  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:07:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:31:51  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/11/08  20:57:06  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.1  1995/08/23  01:18:23  yanke
 * Placeholder file.
 *
 * CSCdi39151:  modulized local/remote file systems
 *
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib.h"
#include "filesys_registry.h"


/******************************************************************************
 * Name: FSLIB_subsys_init
 *
 * Description: initialization for the remote file system client
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
static void FSLIB_subsys_init (subsystype* subsys)
{
    int file_sys_type;
    int error;
    char device_name[FS_DEV_NAMES_MAX];
    ralib_vector_table_t *ralib_vector_table;
    fslib_vector_table_t *fslib_vector_table;

    /*------------------------------------------------------------------------*
     * First we get the lower layer stuff running -- get the flash
     * file system initialized.
     *------------------------------------------------------------------------*/
    /*
     * Initialize the raw access library and the flash file system library
     */
    ralib_vector_table = ralib_init();
    fslib_vector_table = FSLIB_init(ralib_vector_table);
    
    /*------------------------------------------------------------------------*
     * Now we can initialize all the upper layer stuff -- the file
     * system registration etc.
     *------------------------------------------------------------------------*/
    /*
     * register with the file system layer and get the file_sys_type
     * to use.
     */
    error = (int)FSLIB_getdevicenames(device_name, FS_DEV_NAMES_MAX);
    file_sys_type = FS_register(device_name, FS_STD_ERROR_CODE_ALLOCATION);

    /*
     * add all the client file system routines to the registries
     */
    reg_add_open(file_sys_type,FSLIB_open,"FSLIB_open");
    reg_add_close(file_sys_type,FSLIB_close,"FSLIB_close");
    reg_add_read(file_sys_type,FSLIB_read,"FSLIB_read");
    reg_add_write(file_sys_type,FSLIB_write,"FSLIB_write");
    reg_add_lseek(file_sys_type,FSLIB_lseek,"FSLIB_lseek");
    reg_add_stat(file_sys_type,FSLIB_stat,"FSLIB_stat");
    reg_add_fstat(file_sys_type,FSLIB_fstat,"FSLIB_fstat");
    reg_add_istat(file_sys_type,FSLIB_istat,"FSLIB_istat");
    reg_add_fsstat(file_sys_type,FSLIB_fsstat,"FSLIB_fsstat");
    reg_add_delete(file_sys_type,FSLIB_delete,"FSLIB_delete");
    reg_add_undelete(file_sys_type,FSLIB_undelete,"FSLIB_undelete");
    reg_add_squeeze(file_sys_type,FSLIB_squeeze,"FSLIB_squeeze");
    reg_add_settype(file_sys_type,FSLIB_settype,"FSLIB_settype");
    reg_add_getdents(file_sys_type,FSLIB_getdents,"FSLIB_getdents");
/*    reg_add_format(file_sys_type,FSLIB_format,"FSLIB_format");*/
    reg_add_geterrortext(file_sys_type,FSLIB_geterrortext,"FSLIB_geterrortext");
    reg_add_getdevicenames(file_sys_type,FSLIB_getdevicenames,"FSLIB_get_device_names");
    reg_add_ioctl(file_sys_type, FSLIB_ioctl, "FSLIB_ioctl");
    reg_add_fioctl(file_sys_type, FSLIB_fioctl, "FSLIB_fioctl");
}

/*----------------------------------------------------------------------------*
 * Subsystem header for the flash file system
 *----------------------------------------------------------------------------*/
SUBSYS_HEADER(flash_file_system,        /* name */
              1,                        /* major version */
              0,                        /* minor version */
              1,                        /* edit version */
              FSLIB_subsys_init,        /* init function */
              SUBSYS_CLASS_DRIVER,      /* subsystem class */
              "seq: file_system",       /* sequence list */
              "req: file_system"        /* prerequisite list */
              );
