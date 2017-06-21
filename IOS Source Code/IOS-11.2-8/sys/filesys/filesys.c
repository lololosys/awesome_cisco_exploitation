/* $Id: filesys.c,v 3.4.20.1 1996/03/18 19:35:06 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/filesys.c,v $
 *------------------------------------------------------------------
 * File system interface routines
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: filesys.c,v $
 * Revision 3.4.20.1  1996/03/18  19:35:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.2  1996/03/07  09:35:24  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  00:47:51  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/01/25  11:17:16  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.3  1995/11/17  09:07:23  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/16  23:23:18  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  11:31:28  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/11/08  20:56:54  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.2  1995/09/27  21:52:45  izhu
 * CSCdi41172:  7000 dir/show flash command not consistant w/ 7500
 *
 * Revision 2.1  1995/06/07  20:31:55  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "filesys.h"
#include "subsys.h"
#include "filesys_registry.h"
#include "../filesys/filesys_registry.regc"

long filesys_regcode;

/*----------------------------------------------------------------------------*
 * Static variables
 *----------------------------------------------------------------------------*/
static int FS_file_sys_type;
static int FS_error_code_base;

/*----------------------------------------------------------------------------*
 * Structure/Table that maps device ID to file system type
 *
 * This table is built at registration time, and has one entry per
 * device supported.  The table is terminated by an entry with at
 * file_sys_type of 0, so the actual maximum number of devices
 * supported is one less than FS_MAX_DEVICE
 *
 * dev_id -- device ID of the device.  Null terminated with NO COLON.
 * dev_id_len -- strlen of dev_id
 * file_sys_type -- the value to use when calling this file sub-system
 *   via registry.
 * first_device_of_this_type -- TRUE if this is the first device with
 *   this "file_sys_type".  This enables the list to be searched to
 *   find all the file sub-system.
 * error_code_base -- the starting error code number given the
 *   sub-system this device is part of.  This is a negative number.
 * min_error_code -- the arithmetic minimum error code for the
 *   subsystem this device is part of.  This is a negative number.
 * max_error_code -- the arithmetic maximum error code for the
 *   subsystem this device is part of.  This is a negative number.
 *
 *   If a sub-system supports 1000 error code in the range of -1000
 *   to -1999,
 *
 *     error_code_base = -1000
 *     min_error_code = -1999
 *     max_error_code = -1000
 *
 *----------------------------------------------------------------------------*/
typedef struct fs_device_rec_ {
    char     dev_id[FS_DEV_ID_MAX];
    int      dev_id_len;
    int      file_sys_type;
    boolean  first_device_of_this_type;
    int      error_code_base;
    int      min_error_code;
    int      max_error_code;
} fs_device_rec_t;

#define FS_MAX_DEVICE 16
static fs_device_rec_t fs_device_table[FS_MAX_DEVICE];

/*----------------------------------------------------------------------------*
 * Structure/Table that maps global file descriptors to file system
 *  type and local file descriptor
 *----------------------------------------------------------------------------*/
typedef struct fs_file_rec_ {
    int      state;
    int      global_fd;
    int      local_fd;
    int      file_sys_type;
    fs_device_rec_t *dp;  
} fs_file_rec_t;

#define FS_MAX_FILES 32
static fs_file_rec_t fs_file_table[FS_MAX_FILES];

#define FILE_REC_FREE   0
#define FILE_REC_IN_USE 1

/*----------------------------------------------------------------------------*
 * jack, these are temporary
 *----------------------------------------------------------------------------*/
#define FS_ERROR_TOO_MANY_FILES_OPEN -1
#define FS_ERROR_BAD_FILE_DESCRIPTOR -1
#define FS_ERROR_BUFFER_OVERFLOW     -1

/*
 * XXX need to work this out.
 */
static int get_new_global_fd(int local_fd)
{
    static global_file_descriptor = 1;

    return global_file_descriptor++;
}


/******************************************************************************
 * Name: get_new_file_sys_type
 *
 * Description: gets a new (unique) file sys type value to use
 *
 * Input: None
 *
 * Return Value:
 *   file_sys_type -- a new file sys type 
 *
 *******************************************************************************/
static int get_new_file_sys_type(void)
{
    FS_file_sys_type++;
    return(FS_file_sys_type);
}


/******************************************************************************
 * Name: get_dev
 *
 * Description: returns a pointer to the device record for the  for the device ID given
 *
 * Input: 
 *   dev_id -- a pointer to a character string that contains the
 *     device ID which is either a null-terminated string, or is part
 *     of a file ID that is of the format <dev ID>:<file name>
 *
 * Return Value:
 *   device -- a pointer to the device record for the device or a NULL
 *     if the device is not recognized
 *
 *******************************************************************************/
static fs_device_rec_t *get_dev(const char *dev_id)
{
    fs_device_rec_t *dp;
    fs_device_rec_t *device;

    device = NULL;
    for (dp = fs_device_table; dp->file_sys_type != 0; dp++) {
	if (strncmp(dev_id, dp->dev_id, dp->dev_id_len) == 0) {
	    if ((dev_id[dp->dev_id_len] == '\0') ||
		(dev_id[dp->dev_id_len] == ':')) {
		device = dp;
		break;
	    }
	}
    }
    return(device);
}


/******************************************************************************
 * Name: add_dev
 *
 * Description: adds a device to the filesys device table
 *
 * Input: 
 *   dev_id -- a pointer to a character string that contains the
 *     device ID which is a null-terminated string
 *   file_sys_type -- the file system type used when indexing the file
 *     system call registries.
 *
 * Return Value:
 *   device -- pointer to the device record that was added.  If there
 *     are no more available, a NULL is returned.
 *
 *******************************************************************************/
static fs_device_rec_t *add_dev (const char *dev_id,int file_sys_type)
{
    int dev;
    fs_device_rec_t *dp;
    fs_device_rec_t *device;

    device = NULL;
    for (dev = 0, dp = fs_device_table; dev < (FS_MAX_DEVICE - 1); dev++, dp++) {
	if (dp->file_sys_type == 0) {
	    strncpy(dp->dev_id,dev_id,sizeof(dp->dev_id));
	    dp->file_sys_type = file_sys_type;
	    dp->dev_id_len = strlen(dev_id);
	    device = dp;
	    break;
	}
    }
    return(device);
}


/******************************************************************************
 * Name: find_file_rec
 *
 * Description: finds a file record by global file descriptor
 *
 * Input: 
 *   global_fd -- global file descriptor to find
 *
 * Return Value:
 *   file_rec_ptr -- pointer to the file record for that file
 *     descriptor or NULL if none found
 *
 *******************************************************************************/
static fs_file_rec_t *find_file_rec(int global_fd)
{
    int i;
    fs_file_rec_t *fp;

    for (i = 0, fp = fs_file_table; i < FS_MAX_FILES; i++, fp++) {
	if (fp->global_fd == global_fd) {
	    return(fp);
	}
    }

    return(NULL);
}

/******************************************************************************
 * Name: alloc_file_rec
 *
 * Description: Allocates an free file record to use
 *
 * Input: None
 *
 * Return Value:
 *   file_rec_ptr -- a pointer to the newly allocated file record or a
 *     NULL if there are no free ones
 *
 *******************************************************************************/
static fs_file_rec_t *alloc_file_rec(void)
{
    int i;
    fs_file_rec_t *fp;

    for (i = 0, fp = fs_file_table; i < FS_MAX_FILES; i++, fp++) {
	if (fp->state == FILE_REC_FREE) {
	    fp->state = FILE_REC_IN_USE;
	    return(fp);
	}
    }

    return(NULL);
}


/******************************************************************************
 * Name: free_file_rec
 *
 * Description: frees the file record 
 *
 * Input: 
 *   fp -- a pointer to the file record to free
 *
 * Return Value: None
 *
 *******************************************************************************/
static void free_file_rec(fs_file_rec_t *fp)
{
    fp->state = FILE_REC_FREE;
    fp->global_fd = 0;
    fp->local_fd = 0;
    fp->file_sys_type = 0;
}


/******************************************************************************
 * Name: convert_error_code
 *
 * Description: converts an error code based on the file system
 *
 * Input: 
 *   dp -- a pointer to the device record for the device
 *   ec -- the error code to convert 
 *
 * Return Value: 
 *   ec -- the converted error code
 *
 *******************************************************************************/
static int convert_error_code (fs_device_rec_t *dp, int ec)
{
    if (ec < 0 && dp != NULL)
	ec += dp->error_code_base;
    return(ec);
}


/******************************************************************************
 * Name: FS_subsys_init
 *
 * Description: initialization for the file system
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
static void FS_subsys_init (subsystype* subsys)
{
    int i;
    fs_file_rec_t *fp;

    for (i = 0; i < FS_MAX_DEVICE; i++) {
	fs_device_table[i].dev_id[0] = '\0';
	fs_device_table[i].dev_id_len = 0;
	fs_device_table[i].file_sys_type = 0;
	fs_device_table[i].first_device_of_this_type = FALSE;
	fs_device_table[i].error_code_base = 0;
	fs_device_table[i].min_error_code = 0;
	fs_device_table[i].max_error_code = 0;
    }


    for (i = 0, fp = fs_file_table; i < FS_MAX_FILES; i++, fp++) {
	fp->state = FILE_REC_FREE;
	fp->global_fd = 0;
	fp->local_fd = 0;
	fp->file_sys_type = 0;
    }


    FS_file_sys_type = 0;
    
    /*
     * the first <FS_ERROR_CODE_GRANULARITY> error codes are reserved
     * for the filesys layer to use
     */
    FS_error_code_base = FS_ERROR_CODE_GRANULARITY;

    /*
     * Set up file system private function registry
     */
    filesys_regcode = registry_create(REG_UNKNOWN, SERVICE_FILESYS_MAX,
				      "File System");
    if (filesys_regcode != REG_ILLEGAL)
        create_registry_filesys();
}

/*----------------------------------------------------------------------------*
 * Subsystem header for the remote file system client
 *----------------------------------------------------------------------------*/
SUBSYS_HEADER(file_system,              /* name */
              1,                        /* major version */
              0,                        /* minor version */
              1,                        /* edit version */
              FS_subsys_init,           /* init function */
              SUBSYS_CLASS_DRIVER,      /* subsystem class */
              NULL,                     /* sequence list */
              NULL                      /* prerequisite list */
              );


/******************************************************************************
 * Name: FS_open
 *
 * Description: opens a file
 *
 *   NOTE: Do not use open as a semaphore.  This is a common practice
 *         in some Unix applications, and should not be used in this
 *         file system since opening files uses up space and may fail
 *         since only one file is allowed open at a time.
 *
 * Input: 
 *   file_id -- a file ID is made up of two parts: a device id and a file
 *     name.  They are separated by a colon (:).  Device ID's can be up to
 *     seven characters long, but the actual values accepted are not defined
 *     by this function or FSLIB in general.  They are defined by the raw
 *     access library (RALIB) and this function uses the RALIB to figure out
 *     if a valid device ID was used.  File names can be up to 31 characters
 *     long.  The entireile ID should be null terminated.  So the format
 *     looks like this:
 *
 *       <device ID>:<file name><null>
 *
 *       where:
 *         <device ID> is 1 to N characters (don't know the limit on N)
 *         <file name> is 1 to 31 characters
 *         
 *   flags -- the flags that indicate options to the open function
 *     The flags that are recognized are (they are a subset of the Unix flags)
 *       O_RDONLY -- file is opened read-only if one exists.
 *       O_WRONLY -- file is opened write-only
 *       (only one of the above flags may be specified)
 *       O_CREAT  -- if a file does not exist, one is created
 *       O_APPEND -- all writes are appended to the end of the file
 *       O_TRUNC  -- the file is truncated when it is opened
 *       O_CRC_OFF -- do not perform crc calculation
 *       O_VERBOSE_OFF -- keep quiet, do not print message to console
 *
 *     Because of limitations in the file system, only certain combinations of
 *     flags are accepted.  The action taken depends on the flags specified and
 *     whethere or no the file already exists.  The valid combinations of
 *     options are
 *
 *       O_RDONLY
 *       O_WRONLY | O_APPEND | O_TRUNC
 *       O_WRONLY | O_APPEND | O_TRUNC | O_CREAT
 *
 *     The following table shows the combinations that are accepted and the
 *     actions that they cause.
 *
 *     +--------+--------+--------+--------+--------+------------------------+
 *     | access | exists | CREAT  | APPEND | TRUNC  | action
 *     +--------+--------+--------+--------+--------+------------------------+
 *     | RDONLY |   NO   |   NO   |   NO   |   NO   | No file opened.
 *     +--------+--------+--------+--------+--------+------------------------+
 *     | RDONLY |  YES   |   X    |   NO   |   NO   | File opened read only.
 *     +--------+--------+--------+--------+--------+------------------------+
 *     | WRONLY |   NO   |   NO   |  YES   |  YES   | No file is opened.
 *     +--------+--------+--------+--------+--------+------------------------+
 *     | WRONLY |   NO   |  YES   |  YES   |  YES   | New file is created and 
 *     |        |        |        |        |        | opened for writing.
 *     +--------+--------+--------+--------+--------+------------------------+
 *     | WRONLY |  YES   |   X    |  YES   |  YES   | Existing file is marked
 *     |        |        |        |        |        | deleted,new file of same
 *     |        |        |        |        |        | name is opened (file has
 *     |        |        |        |        |        | 0 length initially)
 *     +--------+--------+--------+--------+--------+------------------------+
 *  
 *   mode -- a value to be used for the file_flags when creating a new file. 
 *     If a new file is *NOT* being created, this parameter has no funciton.
 *     More information about file flags is in the file system include file.
 *
 * Return Value:
 *   fd -- file descriptor for the file that was opened.  If the file was
 *     opened successfully, this is a non-negative number.  If there was an
 *     error opening the file, this is a negative number and it is the error
 *     code that describes the error.  This is done to replace the errno
 *     variable in the Unix environment.  The error codes are defined in the 
 *     file system include file.
 *
 *******************************************************************************/
int FS_open (const char *file_id, int flags, ulong mode)
{
    int local_fd;
    int global_fd;
    fs_device_rec_t *dp;
    fs_file_rec_t *fp;
    
    /* 
     * Get the file system type from the device name
     */
    dp = get_dev(file_id);
    if (dp == NULL) {
	global_fd = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	/* 
	 * Get a file record and global file descriptor
	 */
	fp = alloc_file_rec();
	if (fp == NULL) {
	    global_fd = FS_ERROR_TOO_MANY_FILES_OPEN;
	} else {
	    /* 
	     * Make the registry call to open
	     */
	    local_fd = reg_invoke_open(dp->file_sys_type,file_id,flags,mode);
	    if (local_fd < 0) {
		free_file_rec(fp);
		global_fd = convert_error_code(dp, local_fd);
	    } else {
		/* 
		 * If things went well, translate the file descriptor
		 * and keep track of the type for future reference
		 */
		fp->global_fd = get_new_global_fd(local_fd);
		fp->local_fd = local_fd;
		fp->file_sys_type = dp->file_sys_type;
		fp->dp = dp;
		return fp->global_fd;
	    }
	}
    }
    return(global_fd);
}
 

/******************************************************************************
 * Name: FS_close
 *
 * Description: closes an open file
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *
 * Return Value:
 *   error_code -- 0 if successful, othewise an error code.
 *
 *******************************************************************************/
int FS_close (int fd)
{
    int rc;
    fs_file_rec_t *fp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    fp = find_file_rec(fd);
    if (fp == NULL) {
        rc = FS_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	rc = reg_invoke_close(fp->file_sys_type,fp->local_fd);
	rc = convert_error_code(fp->dp, rc);
	free_file_rec(fp);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_read
 *
 * Description: reads from an open file
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   buffer -- a pointer to the buffer inwhich to put the data
 *   byte_count -- the number of bytes to read
 *
 * Return Value:
 *   actual -- the number of bytes actually read.  If there was an error while
 *     reading the file, an error code is returned.  All error codes are
 *     negative, and all valid values for actual are positive.  Note that
 *     reading from a file that has no more bytes left in it is NOT an error.
 *     In this case a 0 is returned.
 *
 *******************************************************************************/
int FS_read (int fd, void *buffer, int byte_count)
{
    int actual;
    fs_file_rec_t *fp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    fp = find_file_rec(fd);
    if (fp == NULL) {
	actual = FS_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	actual = reg_invoke_read(fp->file_sys_type,fp->local_fd,buffer,byte_count);
	actual = convert_error_code(fp->dp,actual);
    }
    return(actual);
}


/******************************************************************************
 * Name: FS_write
 *
 * Description: writes data to a file that has been opened for writing
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   buffer -- a pointer to the data to write
 *   byte_count -- the number of bytes to write
 *
 * Return Value:
 *   actual -- the number of bytes actually written.  If there was an error,
 *     an error code is returned.  All error codes are negative.
 *
 *******************************************************************************/
int FS_write (int fd, void *buffer, int byte_count)
{
    int actual;
    fs_file_rec_t *fp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    fp = find_file_rec(fd);
    if (fp == NULL) {
	actual = FS_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	actual = reg_invoke_write(fp->file_sys_type,fp->local_fd,buffer,byte_count);
	actual = convert_error_code(fp->dp,actual);
    }
    return(actual);
}


/******************************************************************************
 * Name: FS_lseek
 *
 * Description: sets the current pointer associated with the open file
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   offset -- the offset to seek in the file 
 *   whence -- determines from whence the seek originates. Possible values:
 *     SEEK_SET -- set the pointer to the offset.  If the offset is longer
 *       than the file, the pointer is set one byte past the end of the file.
 *     SEEK_CUR -- increment the pointer by offset.  If pointer + offset is
 *       longer than the file, the pointer is set one byte past the end of
 *       the file.
 *     SEEK_END -- set the pointer to the size of the file plus offset. 
 *       If offset is positive, the pointer is set to the size of the file
 *       (i.e. one byte past the end of the file).  If size + offset is 
 *       negative, the pointer is set to 0 (i.e. the beginning of the file).
 *
 * Return Value:
 *   curr_ptr -- the new value of the file pointer.  If an error occurred, an
 *     error code is returned.  All error codes are negative, and all valid 
 *     file pointers are positive.
 *
 *******************************************************************************/
int FS_lseek (int fd, int offset, int whence)
{
    int curr_ptr;
    fs_file_rec_t *fp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    fp = find_file_rec(fd);
    if (fp == NULL) {
	curr_ptr = FS_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	curr_ptr = reg_invoke_lseek(fp->file_sys_type,fp->local_fd,offset,whence);
	curr_ptr = convert_error_code(fp->dp,curr_ptr);
    }
    return(curr_ptr);
}


/******************************************************************************
 * Name: FS_stat
 *
 * Description: returns information about a file (by file name)
 *
 * Input: 
 *   file_id -- the file ID of the file (including device ID and file name)
 *     for which to return status
 *   status -- a pointer to a buffer inwhich to write the stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_stat (const char *file_id, stat_t *status)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(file_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_stat(dp->file_sys_type,file_id,status);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_fstat
 *
 * Description: returns information about an open file (by file descriptor)
 *
 * Input: 
 *   fd -- the file descriptor for the open file (returned from open())
 *   status -- a pointer to a buffer inwhich to write the stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_fstat (int fd, stat_t *status)
{
    int rc;
    fs_file_rec_t *fp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    fp = find_file_rec(fd);
    if (fp == NULL) {
	rc = FS_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	rc = reg_invoke_fstat(fp->file_sys_type,fp->local_fd,status);
	rc = convert_error_code(fp->dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_istat
 *
 * Description: returns information about a file (by file index)
 *
 * Input: 
 *   dev_id -- a pointer to a character string that is the device ID for the
 *     device containing the file.  The device ID can be terminated with a 
 *     null or a colon (:).
 *   index -- the index of the file for which to return status
 *   status -- a pointer to a buffer inwhich to write the stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_istat (const char *dev_id, int index, stat_t *status)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(dev_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_istat(dp->file_sys_type,dev_id,index,status);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_fsstat
 *
 * Description: returns information about a file system
 *
 * Input: 
 *   dev_id -- a pointer to a character string that is the device ID for the
 *     device for which to return status information
 *   status -- a pointer to a buffer inwhich to write the file_sys_stat_t structure.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(dev_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_fsstat(dp->file_sys_type,dev_id,status);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_delete
 *
 * Description: deletes a file by file name
 *
 * Input: 
 *   file_id -- the file ID (including the device ID and the file name) of the
 *     file to delete
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_delete (const char *file_id)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(file_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_delete(dp->file_sys_type,file_id);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_undelete
 *
 * Description: undeletes a file by device ID and file index
 *
 * Input: 
 *   dev_id -- the device ID of the device containing the file to undelete
 *   index -- the index of the file to undelete
 *   mode -- indicates what to do if the file to undelete has the same name
 *     as a file that already exists.  
 *     FSLIB_UNDELETE_AUTO -- automatically delete another file of the same name
 *     FSLIB_UNDELETE_UNIQUE -- only undelete the file if the file name in unique
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_undelete (const char *dev_id, int index, int mode)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(dev_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_undelete(dp->file_sys_type,dev_id,index,mode);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_squeeze
 *
 * Description: squeezes al the unused space out of a file system
 *   This function may take a long time to execute (perhaps minutes)
 *   since it may have to erase all of flash.  The process will leave a file
 *   system with all of the files that are valid at the beginning of the file
 *   system memory, and all of the unused space at the end of the file system
 *   memory will be erased.  After this function is executed, none of the
 *   deleted files can be successfully "undeleted".  Before a squeeze, files
 *   that have been deleted remain in the file system and are just marked
 *   deleted.  When the squeeze is executed, the deleted files are actually
 *   erased. 
 *
 * Input: 
 *   dev_id -- the device ID of the device containing the file to undelete
 *   flag -- flag that indicates whether the squeeze log should be used or not.
 *     Not using the squeeze log could speed up the squeeze in some cases, but
 *     it means that if power is lost or the card is removed before the
 *     squeeze completes, the data on the flash will be lost, and the device
 *     will have to be reformatted.
 *     SQUEEZE_NORMAL -- squeeze log *WILL* be used
 *     SQUEEZE_NO_LOG -- squeeze log *WILL NOT* be used
 *
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
int FS_squeeze (const char *dev_id, int flag)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(dev_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_squeeze(dp->file_sys_type,dev_id,flag);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_settype
 *
 * Description: Sets the file type of the specified file.
 *   If the file type has already been set, it is not changed.  If the current 
 *   value of the file type is the same as the value being set, this program
 *   returns FSLIB_ERROR_FILE_TYPE_SET_THE_SAME.  If the current value of the
 *   file type is different than the one being set, this program returns
 *   FSLIB_ERROR_FILE_TYPE_ALREADY_SET.
 *
 * Input: 
 *   file_id -- the file ID of the file (including device ID and file name)
 *     for which to set the type
 *   file_type -- the value to set the file type
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, FSLIB_ERROR_FILE_TYPE_SET_THE_SAME
 *     if the file type has already been set and is being set to the same
 *     value again, FSLIB_ERROR_FILE_TYPE_ALREADY_SET if the file type has
 *     already been set and is being set to a different value.
 *
 *******************************************************************************/
int FS_settype (const char *file_id, ulong file_type)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(file_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_settype(dp->file_sys_type,file_id,file_type);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


int FS_ioctl (const char *file_id, int func, void *arg)
{
    int rc;
    fs_device_rec_t *dp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    dp = get_dev(file_id);
    if (dp == NULL) {
	rc = FS_ERROR_UNKNOWN_DEVICE;
    } else {
	rc = reg_invoke_ioctl(dp->file_sys_type, file_id, func, arg);
	rc = convert_error_code(dp,rc);
    }
    return(rc);
}


/******************************************************************************
 * Name: FS_getdents
 *
 * Description: gets directory entries in a file system independent format.
 *   The directory entries returned include all files in the file system
 *   including files that have been deleted.
 *
 * Input: 
 *   fd -- the file descriptor for an open directory (the directory file to
 *     open for the directory is <device ID>:. (where the file name is ".")
 *   buffer -- a pointer to a buffer to hold the directory entries.
 *   buffer_length -- the length of the buffer pointed to by "buffer"
 *
 * Return Value:
 *   byte_count -- the number of bytes written to the buffer.  If there was 
 *     an error an error code is returned.  All error codes are negative.
 *
 *******************************************************************************/
int FS_getdents (int fd, char *buffer, int buffer_length)
{
    int rc;
    fs_file_rec_t *fp;
    
    /* 
     * Demux to different devices according to file descriptor.
     */
    fp = find_file_rec(fd);
    if (fp == NULL) {
	rc = FS_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	rc = reg_invoke_getdents(fp->file_sys_type,fp->local_fd,buffer,buffer_length);
	rc = convert_error_code(fp->dp,rc);
    }
    return(rc);
}

/******************************************************************************
 * Name: FS_geterrortext
 *
 * Description: returns a pointer to a text string for the give error code
 *   This function will always return a null-terminated string in the
 *   buffer even if the buffer it shorter than the error text string.
 *   In this case a null will just be tacked on the end and the error
 *   text string will be truncated.
 *
 * Input: 
 *   error_code -- error code for which to get the text string
 *   buffer -- a pointer to the buffer in which to put the text string
 *   buffer_len -- the length of the buffer (must be > 0)
 *
 * Return Value: None
 *
 *******************************************************************************/
void FS_geterrortext (int error_code, char *buffer, int buffer_len)
{
    fs_device_rec_t *dp;
    int local_error_code;

    for (dp = fs_device_table; dp->file_sys_type != 0; dp++) {
	if ((error_code > dp->min_error_code) && (error_code < dp->max_error_code)) {
	    local_error_code = error_code - dp->error_code_base;
	    reg_invoke_geterrortext(dp->file_sys_type,local_error_code,buffer,buffer_len);
	    break;
	}
    }
}


/******************************************************************************
 * Name: FS_getdevicenames
 *
 * Description: returns a pointer to a text string of the devices supported
 *
 * Input: character buffer ptr & length
 *
 * Return Value:
 *   device_name_ptr -- a pointer to a character string that contains
 *     the names of the supported device names (i.e the name of the
 *     devices that cause get_dev_num() to return a valid device
 *     number).  The names do not contain the colon (:) and are
 *     concatenated with separating spaces and null terminated.
 *     The device names are in no specific order. 
 *     The character string is a const, and must not be modified.
 *
 *     Example of return value.
 *       "slot0 slot1 bootflash"
 *
 *******************************************************************************/
int FS_getdevicenames (char *buffer, int buffer_len)
{
    fs_device_rec_t *dp;
    int rc;
    char *original_buffer;
    char *buffer_end;

    /*
     * leave room at the end for a NUL
     */
    original_buffer = buffer;
    buffer_end = &buffer[buffer_len - 1];

    /*
     * Copy all the device names to the string
     */
    rc = FS_OK;
    for (dp = fs_device_table; dp->file_sys_type != 0; dp++) {
	if ((dp->file_sys_type != 0) && dp->first_device_of_this_type) {
	    rc = reg_invoke_getdevicenames(dp->file_sys_type,
					   buffer, buffer_end - buffer);
	    if (rc != FS_OK) {
		rc = convert_error_code(dp,rc);
		break;
	    }
	    while (buffer < buffer_end) {
		if (*buffer == '\0')
		    break;
		++buffer;
	    }
	    if (*buffer != '\0') {
		*buffer = '\0';
		rc = FS_ERROR_TRUNCATED;
		break;
	    }
	    *buffer++ = ' ';
	    *buffer   = '\0';
	}
    }
    
    /*
     * the last char we put on was a space, so we back up and
     * put a NUL there unless we didn't copy any characters to
     * the buffer in which case we don't back up.
     */
    if ((buffer > original_buffer) &&
	(buffer[-1] == ' '))
	--buffer;
    *buffer = '\0';

    return(rc);
}


/******************************************************************************
 * Name: FS_register
 *
 * Description: Used during initialization to register a device
 *   sub-system (that supports one or more devices).  It returns the
 *   file system type number underwhich this sub-system should
 *   register its file system routines (open, read, write, close, etc.)
 *
 * Input:
 *   device_name_ptr -- a pointer to a character string that contains
 *     the names of the supported device names.  The names do not
 *     contain the colon (:) and are concatenated with separating
 *     spaces and null terminated.
 *     The device names are in no specific order. 
 *     The character string is a const, and must not be modified.
 *
 *     Example of return value.
 *       "slot0 slot1 bootflash"
 *
 *   error_code_range -- the number of error codes to reserve for this
 *     sub-system of the file system.  It can be a negative or a
 *     positive number.
 *
 * Return Value:
 *   file_sys_type -- the file system type under which the file system
 *     functions should be registered.
 *
 *******************************************************************************/
int FS_register (const char *device_name_ptr, int error_code_range)
{
    int file_sys_type;
    boolean first;
    char dev_id[FS_DEV_ID_MAX + 1];
    int i;
    char *dev_id_ptr;
    int error_code_base = 0;
    int max_error_code = 0;
    int min_error_code = 0;
    fs_device_rec_t *dp;

    if (device_name_ptr == NULL) {
	/* XXX - post an error */
	file_sys_type = -1;
    } else {
	/*
	 * get a new (unique) file sys type for the caller
	 */
	file_sys_type = get_new_file_sys_type();
	
	/*
	 * scan through the device_name_ptr to find the devices and
         * add them to the device table.
	 */
	first = TRUE;
	while (*device_name_ptr != '\0') {
	    /*
	     * copy the device name to the dev_id variable and null
	     * terminate it.
	     */
	    dev_id_ptr = dev_id;
	    for (i = 0; i < FS_DEV_ID_MAX; i++) {
		if (*device_name_ptr == '\0') {
		    *dev_id_ptr = *device_name_ptr;
		    break;
		} else if (*device_name_ptr == ' ') {
		    *dev_id_ptr = '\0';
		    /*** *device_name_ptr++; ***/
		    device_name_ptr++;	/* XXX - we want this? */
		    break;
		} else {
		    *dev_id_ptr++ = *device_name_ptr++;
		}
	    }
	    
	    /*
	     * add the device
	     */
	    dp = add_dev(dev_id,file_sys_type);
	    if (dp != NULL) {
		if (first) {
		    first = FALSE;
		    /*
		     * we do all this error code calculations in the
                     * positive and then flip to negative numbers at
                     * the end
		     */
		    if (error_code_range < 0) {
			error_code_range = -error_code_range;
		    }
		    error_code_base = -FS_error_code_base;
		    max_error_code = -FS_error_code_base;
		    FS_error_code_base += ((((error_code_range) 
					     / FS_ERROR_CODE_GRANULARITY) + 1) 
					   * FS_ERROR_CODE_GRANULARITY);
		    min_error_code = -FS_error_code_base;
		    dp->first_device_of_this_type = TRUE;
		}

		dp->error_code_base = error_code_base;
		dp->max_error_code = max_error_code;
		dp->min_error_code = min_error_code;
	    }
	}
    }
    return(file_sys_type);
}

/*****************************************************************************
 *
 * Common utility routines.
 *
 *****************************************************************************/

/*
 * The following routine will be replaced by fileid2filename()
 */
const char *get_file_name_by_file_id (const char *file_id)
{
    char *ptr;

    ptr = strchr(file_id, ':');
    if (ptr == NULL) {
	/*
	 * ":" is missing, entire file_id is file_name.
	 */
	return file_id;
    } else if (ptr == file_id) {
	/*
	 * First is ":", so device id is missing.
	 */
	return NULL;
    } else {
	ptr++;		/* skip the ":"*/
	if (*ptr == 0)
	    return NULL;
	else
	    return ptr;
    }
}

/*
 * The following routine will be replaced by fileid2devid()
 */
const char *get_dev_id_by_file_id (const char *file_id)
{
    char *ptr;
    static char dev_id_part[FSLIB_DEV_ID_MAX + 1];
    char *ptr2;

    memset(dev_id_part, 0, FSLIB_DEV_ID_MAX + 1);

    ptr = strchr(file_id, ':');
    if (ptr == NULL || ptr == file_id)
	return NULL;	/* device id must be followed by ":" */

    /*
     * Analyze non-empty string before ":".
     */
    *ptr = 0;
    ptr2 = strchr(file_id, ',');
    if (ptr2) {
	*ptr2 = 0;
	sstrncpy(dev_id_part, file_id, FSLIB_DEV_ID_MAX);
	*ptr2 = ',';
    } else
	sstrncpy(dev_id_part, file_id, FSLIB_DEV_ID_MAX);
    *ptr = ':';

    if (dev_id_part[0] == 0)
	return NULL;
    else
	return &dev_id_part[0];
}

void fileid2filename (const char *file_id, char *file_name, int len)
{
    const char *name;

    name = get_file_name_by_file_id(file_id);
    if (name) {
	strncpy(file_name, name, len);
	file_name[len] = 0;
    } else
	*file_name = 0;
}

void fileid2devid (const char *file_id, char *dev_id, int len)
{
    const char *name;

    name = get_dev_id_by_file_id(file_id);
    if (name) {
	strncpy(dev_id, name, len);
	dev_id[len] = 0;
    } else
	*dev_id = 0;
}
