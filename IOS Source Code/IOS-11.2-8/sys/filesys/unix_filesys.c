/* $Id: unix_filesys.c,v 3.2 1995/11/17 09:09:13 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/filesys/unix_filesys.c,v $
 *------------------------------------------------------------------
 * File system interface for the standalone Unix file system test image
 *
 * February 1995, Jack Jenney
 *
 * Copyright (c) 1994-1995, 1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: unix_filesys.c,v $
 * Revision 3.2  1995/11/17  09:09:13  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:33:06  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:33:27  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib.h"
#include "fslib_internals.h"



/******************************************************************************
 * Name: FS_init
 *
 * Description: initialization for the file system
 *
 * Input: 
 *
 * Return Value:
 *
 *******************************************************************************/
void FS_init (void)
{
    ralib_vector_table_t *ralib_vector_table;
    fslib_vector_table_t *fslib_vector_table;

    ralib_vector_table = ralib_init();
    fslib_vector_table = FSLIB_init(ralib_vector_table);

}


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
 *     long.  The entire file ID should be null terminated.  So the format
 *     looks like this:
 *
 *       <device ID>:<file name><null>
 *
 *       where:
 *         <device ID> is 1 to 7 characters
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
 *     If a new file is *NOT* bring created, this parameter has no funciton.
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
    int fd;

    fd = FSLIB_open(file_id,flags,mode);
    return(fd);
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

    actual = FSLIB_read(fd,buffer,byte_count);
    return(actual);
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

    rc = FSLIB_close(fd);
    return(rc);
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

    curr_ptr = FSLIB_lseek(fd,offset,whence);
    return(curr_ptr);
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

    actual = FSLIB_write(fd,buffer,byte_count);
    return(actual);
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

    rc = FSLIB_fstat(fd,status);
    return(rc);
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
    
    rc = FSLIB_stat(file_id,status);
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
    int error;
    
    error = FSLIB_istat(dev_id,index,status);
    return(error);
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

    rc = FSLIB_fsstat(dev_id,status);
    return((int)rc);
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
    int error;
    
    error = FSLIB_delete(file_id);
    return(error);
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
    int error;

    error = FSLIB_undelete(dev_id,index,mode);
    return(error);
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
    int error;
    
    error = FSLIB_squeeze(dev_id,flag);
    return(error);
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

    rc = FSLIB_settype(file_id,file_type);
    
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
    int            byte_count;

    byte_count = FSLIB_getdents(fd, buffer, buffer_length);

    return(byte_count);
}




