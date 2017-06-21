/* $Id: fslib.c,v 3.4.42.4 1996/08/30 02:10:28 rolsen Exp $
 * $Source: /release/112/cvs/Xsys/filesys/fslib.c,v $
 *------------------------------------------------------------------
 * File system library -- implements the flash file system
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib.c,v $
 * Revision 3.4.42.4  1996/08/30  02:10:28  rolsen
 * CSCdi47580:  micro reload prints from interrupt level (SYS-3-INTPRINT)
 * Branch: California_branch
 *              make check of interrupt level before doing printf
 *
 * Revision 3.4.42.3  1996/08/07  08:58:24  snyder
 * CSCdi65343:  more things that should be declared const
 *              char * [] -> char *const []
 *              char *    -> const char []
 *              savings 17,496 data space, 1,852 image space
 * Branch: California_branch
 *
 * Revision 3.4.42.2  1996/05/06  20:19:52  ssangiah
 * CSCdi56328:  The bootload process is a CPU hog.
 * Branch: California_branch
 *
 * Revision 3.4.42.1  1996/03/18  19:35:29  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.2  1996/03/07  09:35:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  13:49:50  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1995/12/18  07:09:01  schaefer
 * CSCdi45771: include slave devices in 75xx flash mib
 *
 * Revision 3.3  1995/11/20  22:26:53  snyder
 * CSCdi44202:  spellink errors
 *              inconsistant -> inconsistent
 *
 * Revision 3.2  1995/11/17  09:08:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:08  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.6  1995/11/08  20:57:16  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.5  1995/10/16  22:53:12  snyder
 * CSCdi42258:  spellink errors
 *              alread -> already
 *
 * Revision 2.4  1995/07/07  22:57:03  yanke
 * Detect device write protect switch, update it in device structure.
 * Check the device for protection in all write-related operations.
 * Reduce the time to wait for vpp on valid bit.
 *
 * CSCdi36427:  write protecting flash credit card hangs the router
 *
 * Revision 2.3  1995/06/14  20:39:22  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * filesys/fslib.c            suported -> supported
 * os/msg_system.c            unititialized -> uninitialized
 * parser/cfg_routemap_set.h  substract -> subtract
 * smf/smf_debug_flags.h      missses -> misses
 *
 * Revision 2.2  1995/06/14  15:18:28  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * cls/dlcqllc.c:         Unexepcted -> Unexpected
 * ipc/msg_ipc.c:         availble -> available
 * exec_debug_channel.h:  debuggeries -> debugging
 * filesys/fslib.c:       inconsistant -> inconsistent
 * atm/sscop.c:           initalize -> initialize
 * filesys/fslib_print.c: progresss -> progress
 * ts/callback.c:         unnexpected -> unexpecte
 *
 * Revision 2.1  1995/06/07  20:32:19  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/******************************************************************************

   FILE SYSTEM DESIGN OVERVIEW
   ---------------------------

     This file system is designed to be used on sector-erasable flash
     devices that may be removable (i.e. PCMCIA flash cards).

     NOTE:  IT IS NOT INTENDED TO BE USE ON A ROTATING MAGNETIC DISK
            (LIKE A PCMCIA-ATA DISK DRIVE).  ALTHOUGH IT WILL PROBABLY
            WORK ON THIS KIND OF DEVICE, IT WOULD BE AN ESPECIALLY BAD
            CHOICE SINCE IT HAS SOME SEVERE LIMITATIONS DUE TO THE
	    FACT IT WAS DESIGNED TO BE USED ON SECTOR-ERASABLE FLASH
            DEVICES.

            WHY?

            1. It assumes that sectors are generally large compared to
	    the total size of the device (the design point is less
	    than 1000 sectors).

            2. It assumes that it is difficult to erase sectors, and
	    that they can only be written once between erasures.

            3. It keeps the file header and all of its data contiguous
	    in device address space, and has an explicit garbage
	    collection operation (squeeze) that removes all the erased
	    files.  A read/write disk drive should be able to do
	    garbage colection on the fly, and keep files in
	    discontiguous disk sectors.

            4. It supports a very limited set of flags to the open
	    call that 
              - prevent a file from being opened for writing without
                also being truncated (i.e. existing files cannot be
		appended)
              - prevent a file from being opened read/write (all open
                files are either readable or writable)

            SO PLEASE DON'T USE THIS FILE SYSTEM ON A ROTATING DISK.


   READ, WRITE AND ERASE ALGORITHMS
   --------------------------

     This file system is designed to support multiple flash devices
     including PCMCIA flash cards and/or internal flash banks.  Since
     the algorithms used to write to these devices are different, the
     file system uses two functions that contain the device dependent
     stuff about programming and erasing flash.  Each device type has
     its own implementation of those three functions.  The functions
     are the "read function" the "write function" and the "erase 
     function".  They are defined below.

       int read_fn(int dev_num, fslib_ptr src, char *dst, int length)
           dev_num = device number for the device 
           src = offset into the file system from which to start reading
           dst = pointer to a buffer inwhich to put the data read
           length = number of bytes to read
           return(int) actual = actual number of bytes read
          
           The read_fn will never be asked to read more than one 
           sector at a time.  All the bytes read (i.e. pointed to
	   by src) will reside in the same sector.
           
       int write_fn(int dev_num, char *src, fslib_ptr dst, int length)
           dev_num = device number for the device 
           src = pointer to the data to write to the device
           dst = offset into the file system at which to start writing
           length = number of bytes to write
           return(int) actual = actual number of bytes written
          
           The write_fn will never be asked to write more than one 
           sector at a time.  All the bytes written (i.e. pointed to
	   by dst) will reside in the same sector.
           
           
       fslib_error erase_fn(int dev_num, fslib_ptr sector_ptr, int length)
           dev_num = device number of the device
           sector_ptr = pointer to the start of the first sector to
	     erase (note: this will always point to the beginning of a 
	     sector) 
           length = number of bytes to erase (note: this will always
             be an even multiple of sectors)
           return(rs_error) rc = FSLIB_OK if all sectors erased ok,
             othewise an error code.

     When a device is formatted, the device type is written in the
     DIB.  This device type is a Cisco-specific number that is used to
     find the correct write_fn and erase_fn for the device.  The
     FSLIB_prog_table[] array contains device types, coupled with
     pointers to the correct write_fn and erase_fn.  When the device
     is initialized init_device() scans the FSLIB_prog_table for the
     matching device type, and fills in the pointers to the erase and
     write functions.  If the device is not one that the file system
     recognizes (perhaps because it was formatted by a newer release
     of the file system code), it marks the device read only, and puts
     NULL pointers in for the write_fn and the erase_fn.


   SQUEEZE
   -------

     The squeeze operation is done to remove all the deleted files
     from the file system and recover the space that they occupied.
     It is a sort of garbage collection process.  All the files that
     are not deleted are kept and are written to the beginning of the
     flash memory space for the file system, and the rest of flash is
     left in the erased state (i.e. ready to be written).  New files
     can then be added to the end of the file system.

     The process is greatly complicated by two requirements

       1. the need for the squeeze process to be interruptable -- the
          data in the files must not be lost if power goes down while
          a squeeze is going on.

       2. the need to discover and handle bad sectors while they are
          being erased -- this is the normal failure case for flash:
          sectors don't erase all the way.

     Making Squeeze Interruptable
     ----------------------------
     The squeeze process is made interruptable by reserving the last 2
     sectors of flash for buffering file system data, and squeeze
     state information.  In describing the squeeze operation, we refer
     to the "old file system" as the file system before the squeeze,
     and the "new file system" as the file system after the squeeze.

     The last sector is called the "squeeze buffer" and is used to
     contain a sector of the old file system before that sector is
     erased.  The squeeze buffer is NOT alway written for every sector
     -- the algorithm is detailed later.

     The next-to-last sector is called the "squeeze log" and is used
     to keep a running history of the squeeze operation.  Each time a
     permanent (i.e. irreversible) change is made to the file system,
     a record of that change is written in the squeeze log.  If the
     squeeze process is interrupted, the recovery software searches
     through the squeeze log to find out where the process was
     interrupted, and recovers the state of the squeeze operation so
     it can be continued.

     The Squeeze Algorithm
     ---------------------
     The outline of the squeeze operation is fairly simple:

       while (there is still data in the old file system)
           build a sector of the new file system in a RAM buffer
           if (the new fs sector was built using old fs data from the sector to be erased)
	       copy the sector to be erased to the squeeze buffer
	       write a record to the squeeze log saying what sector is in the buffer
	   erase the sector
           write the new sector (in the RAM buffer) to flash
	   write a record to the squeeze log saying where we are in the
                old and new file systems
       end while

       erase all the rest of the flash sectors to the end of the file system area
       erase the squeeze buffer
       erase the squeeze log


     Details of the Squeeze Log
     --------------------------
     The squeeze log is comprised of a list of 24-byte records that
     contain a description of the last step completed.  The records
     are written in a 3-phase process that ensures that the data in
     the squeeze log records is either good, or can be detected as
     bad.  The first byte in the squeeze log record is a "flags" byte
     that contains 3 one-bit flags:

       - the "in use" flag
       - the "valid" flag
       - the "invalid" flag

     The process for writing a squeeze log record goes as follows:

       Phase I:  The squeeze log record in RAM is initialized to the
         erased state of flash (as defined in the DIB).  The "in use"
         bit is changed to the other state to indicate that this flash
         log record is being used, but the data cannot yet be trusted.

       Phase II: The squeeze log record in RAM is filled in with the
         necessary state information -- the flags byte still only has
         the "in use" flag set.

       Phase III: The flags byte in the squeeze log record in RAM is
         changed to include the "valid" bit.  The record is complete.
         Since the last write only changes one bit in the record, it
	 is impossible to get inconsistent data.

      
       Possible squeeze log "flags" values and their meanings
       +---------+---------+---------+------------------------------+
       |  in use |  valid  | invalid | Description                  |
       +---------+---------+---------+------------------------------+
       |    0    |    0    |    0    | UNUSED                       |
       |    1    |    0    |    0    | IN USE -- data not valid yet |
       |    1    |    1    |    0    | VALID -- data OK             |
       |    1    |    1    |    1    | INVALID -- data *NOT* OK     |
       +---------+---------+---------+------------------------------+
       | ALL OTHER COMBINATIONS      | software error               |
       +---------+---------+---------+------------------------------+
       |This table assumes that the erased state of flash is 0, and |
       |that the bits are changed to 1 when they are written. If the|
       |erased state of flash is 1, all the values are inverted. The|
       |software handles this by XOR'ing the "flags" field with the |
       |erased state of flash defined in the DIB.                   |
       +------------------------------------------------------------+

       When the squeeze recovery software encounteres squeeze log
       records with these "flags" values the following assumptions and
       actions are taken

         UNUSED -- the end of the squeeze log has been reached, this
           record is usable for subsequent squeeze log entries
	 IN USE -- the end of the squeeze log has been reached, this
           record is changed to INVALID, and the next squeeze record
           can be used for subsequent squeeze log entries
	 VALID -- this is a valid squeeze log record -- if it is the
           last valid entry in the squeeze log, it is used to recover
           the state of the squeeze operation
	 INVALID -- this is an invalid squeeze log record -- it is
           skipped. 
	 
     There are 2 types of squeeze log records: 

       - "copy" records, that describe a sector of the old file
         system flash being copied to the squeeze buffer -- there is
         only one format for these records
       - "write" records, that describe a sector of the new file
         system flash being written -- there are three different
         formats for "write" records depending on the state of the
         squeeze operation when the end of the sector for the new file
	 system was reached.

     COPY records 
       wr_ptr -- a pointer to the sector that was copied to the
          squeeze buffer 

     WRITE records
       state -- defines the state of the squeze operation when the end
         of the new file system sector was reached.  

	 state == START OF HDR -- the first thing in the next sector
           of the new file system is the start of a file header.
	   wr_ptr -- pointer to the next sector of the new file system
	   old_hdr_ptr -- pointer to the file header in the old file
             system that should be used to continue the squeeze.
	   file_index -- the index of the next file in the new file
             system.

	 state == MIDDLE of HDR -- the end of the last sector in the
           new file system was in the middle of a file header -- the
           first thing in the next sector new file system is the rest
           of that file header.
	   wr_ptr -- pointer to the next sector of the new file system
	   old_hdr_ptr -- pointer to the file header in the old file
             system that should be used to continue the squeeze.
	   file_index -- the index of the next file in the new file
             system.
	   bytes_left_this_hdr -- the number of bytes of this file
             header needed to complete the file header

	 state == DATA -- the end of the last sector in the new file
	   system was in the middle of file data.  The rest of the
           file data is the first thing in the next sector of the new
           file system.
	   wr_ptr -- pointer to the next sector of the new file system
	   next_old_hdr_ptr -- pointer to the file header in the old file
             system that should be used to continue the squeeze after
             the data for the current file is completed.
	   file_index -- the index of the next file in the new file
             system.
	   bytes_left_this_file -- the number of bytes of this file
             data needed to complete the file.  This is rounded up to
             an even multiple of 32-bits since all files are written
             on even 32-bit boundaries -- any pad bytes are copied.

     Recovering an Interrupted Squeeze
     ---------------------------------
     The recovery process is fairly simple, and is implemented in the
     function get_squeeze_state().  The squeeze log is scanned and the
     last valid "write" and "copy" records are found (if any).  If
     there are no valid records in the squeeze log, get_squeeze_state
     sets the state to the beginning of a squeeze process.  There are
     two cases to consider:

       - the last record in the squeeze log is a "write" record 
       - the last record in the squeeze log is a "copy" record 

     If the last record is a "write" record, the data in the write
     record can be used to recover the state of the squeeze.  If the
     last record is a "copy" record, the sector in the squeeze buffer
     is copied back to its original place, and the previous "write"
     record is used to recover the state of the squeeze.


     Detailed Squeeze Algorithm
     --------------------------
     The more detailed description of the squeeze algorithm includes
     the possible states that the squeeze operation can be in when it
     is interrupted, the information necessary to describe that state,
     and the recover procedure for recovering when that state is the
     last state completed.  The best way to really understand this is
     to read the code (no kidding!).


   SPARE SECTORS
   -------------

   When a sector goes bad it is marked bad in the Bad Sector Map, and
   a spare sector is allocated if available.  Sectors that have been
   spared are marked bad in the Bad Sector Map.


   OVERVIEW OF DEVICE LAYOUT
   -------------------------

   There are several sections/features to a flash device.  

   1. Device Info Block (DIB) -- always starts at address 0 in the device
   2. Bad Sector Map (BSM) -- usually follows the DIB, but can be anywhere
   3. MONLIB -- usually at the beginning of the device after the DIB and the
      BSM.  The MONLIB is optional.
   4. File System -- usually follows the MONLIB, and is the largest section of
      the device.
   5. Spare Sectors -- up to 16 sectors that are identified as spare.  They
      can be anywhere, but are usually found after the file system area. They 
      do not need to be adjacent to each other, but usually are.  The Spare
      Sectors are optional.
   6. Squeeze Log -- usually the next to last sector on the device, it can be
      located anywhere, and can be more than one sector long.  The length of
      the squeeze log depends on the number of sectors in the file system.
   7. Squeeze Buffer -- a single sector used to buffer file system sectors
      during the squeeze operation.  It is usually the last sector on the
      device but it can be anywhere.

      +--------------------------------+
      | DIB                            |
      +--------------------------------+
      | BSM                            |
      +--------------------------------+
      | MONLIB (optional)              |
      +--------------------------------+
      |                                |
      | File System                    |
      |                                |
      +--------------------------------+
      | Spare Sectors (optional)       |
      +--------------------------------+
      | Squeeze Log                    |
      +--------------------------------+
      | Squeeze Buffer                 |
      +--------------------------------+


 ******************************************************************************/

#if defined(STANDALONE)
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#else
#include "master.h"
#endif

#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib.h"
#include "fslib_internals.h"
#include "fslib_print.h"
#include <ciscolib.h>
#include <string.h>
#include "ttysrv.h"

#if !defined(STANDALONE)
#if !defined(FSLIB)
#define strncpy sstrncpy
#endif
#endif

#if defined(DEBUG)
#define STATIC
#else
#define STATIC static
#endif

#if defined(C7000)
#include "rp_fc.h"
#endif


/*----------------------------------------------------------------------------*
 * static variables used internally
 *----------------------------------------------------------------------------*/
STATIC int FSLIB_file_semaphore;
STATIC int FSLIB_file_descriptor;
STATIC const char file_system_character_set[] = 
  "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ01234567890!@#$%^&*()-_+=[]{}<>,./\\";
STATIC int fslib_print_enable = TRUE;

/*----------------------------------------------------------------------------*
 * File system version definitions
 *----------------------------------------------------------------------------*/
#define FSLIB_MIN_SUPPORTED_VERSION 0x00010000
#define FSLIB_MAX_SUPPORTED_VERSION 0x00010000

/*----------------------------------------------------------------------------*
 * global variables
 *----------------------------------------------------------------------------*/
ralib_vector_table_t *FSLIB_ralib;
fslib_vector_table_t  FSLIB_vector_table;
fslib_device_t   FSLIB_device_table[FSLIB_MAX_DEVICE];
fslib_file_rec_t FSLIB_file_rec[FSLIB_MAX_FILE_REC];

/*----------------------------------------------------------------------------*
 * Some useful things for debug messages
 *----------------------------------------------------------------------------*/
static char *const torf[2] = {"FALSE","TRUE"};

/*----------------------------------------------------------------------------*
 * Forward references to internal functions
 *----------------------------------------------------------------------------*/
STATIC fslib_error       allocate_squeeze_buffer (fslib_device_t *dev, char **ram_buffer);
STATIC fslib_error       get_busy_info (const char *dev, int *busy_p);
STATIC fslib_error       build_ram_sector (fslib_squeeze_state_rec_t *ss);
STATIC ulong             calc_crc (fslib_file_rec_t *fp, fslib_error *error);
STATIC fslib_error       check_crc (fslib_file_rec_t *fp);
STATIC fslib_error       check_sector_erase (fslib_device_t *dev, fslib_ptr sector_ptr);
STATIC void              convert_file_hdr_to_dirent (fslib_file_hdr_t *fh, dirent_t *de);
STATIC void              convert_file_hdr_to_stat (fslib_file_hdr_t *fh, stat_t *status);
       fslib_error       convert_ralib_error_code (int ralib_error_code);
STATIC fslib_error       copy_sector_to_sector (fslib_device_t *dev, fslib_ptr src, fslib_ptr dst);
STATIC fslib_error       create_new_file (fslib_device_t *dev, fslib_file_rec_t *fp, const char *file_name, ulong mode);
STATIC ulong             dev_read (fslib_device_t *dev, fslib_ptr src, char *dst, ulong length, fslib_ptr *next_ptr);
STATIC void              dev_seek (fslib_device_t *dev, fslib_ptr src, ulong offset, fslib_ptr *next_ptr);
STATIC ulong             dev_write (fslib_device_t *dev, char *src, fslib_ptr dst, ulong length, fslib_ptr *next_ptr);
STATIC fslib_error       erase_sector (fslib_device_t *dev, fslib_ptr sector_ptr);
STATIC fslib_ptr         find_file_hdr_by_index (fslib_device_t *dev,int index,fslib_file_hdr_t *fh,fslib_error *error);
STATIC fslib_ptr         find_file_hdr_by_name (fslib_device_t *dev,const char *file_name,fslib_file_hdr_t *fh,fslib_error *error);
STATIC fslib_error       find_file_hdr_by_phys_ptr (fslib_device_t *dev,fslib_ptr fh_ptr,fslib_file_hdr_t *fh);
STATIC fslib_ptr         find_next_new_file_hdr_ptr (fslib_device_t *dev,fslib_error *error);
STATIC fslib_error       find_squeeze_log_rec (fslib_squeeze_state_rec_t *ss, fslib_squeeze_log_rec_t *slw);
STATIC void              fix_created_file_hdr (fslib_device_t *dev,fslib_file_hdr_t *fh,fslib_ptr fh_ptr,fslib_error *error);
STATIC void              fix_in_use_file_hdr (fslib_device_t *dev,fslib_file_hdr_t *fh,fslib_ptr fh_ptr,fslib_error *error);
STATIC void              free_squeeze_buffer (char *ram_buffer);
STATIC ulong             get_bytes (fslib_device_t *dev, fslib_ptr start_ptr, fslib_ptr end_ptr);
STATIC ulong             get_bytes_left_this_sector (fslib_device_t *dev, fslib_ptr ptr);
STATIC fslib_ptr         get_end_of_region (fslib_device_t *dev, fslib_ptr ptr);
STATIC fslib_file_rec_t *get_file_rec (int fd);
STATIC fslib_device_t   *get_new_dev (void);
STATIC fslib_file_rec_t *get_new_file_rec (void);
STATIC int               get_region (fslib_device_t *dev, fslib_ptr ptr);
STATIC fslib_ptr         get_sector_ptr (fslib_device_t *dev, ulong sector_num);
STATIC ulong             get_sector_num (fslib_device_t *dev, fslib_ptr ptr);
STATIC fslib_error       get_squeeze_state (fslib_squeeze_state_rec_t *ss);
STATIC int               good_sector (fslib_device_t *dev, ulong sector_num);
static fslib_device_t   *init_device (const char *dev_id, boolean force, fslib_error *error);
STATIC fslib_error       mark_file_deleted (fslib_device_t *dev, fslib_file_hdr_t *fh, fslib_ptr fh_ptr);
STATIC fslib_error       mark_file_undeleted (fslib_device_t *dev, fslib_file_hdr_t *fh, fslib_ptr fh_ptr);
STATIC void              print_status_update (const char *status, uint quiet);
STATIC fslib_error       read_file_hdr (fslib_device_t *dev, fslib_ptr fh_ptr, fslib_file_hdr_t *fh, fslib_ptr *next_ptr);
STATIC void              remove_dev (fslib_device_t *dev);
STATIC fslib_ptr         skip_bad_sectors (fslib_device_t *dev, fslib_ptr ptr);
STATIC fslib_error       switch_to_spare_sector (fslib_device_t *dev, fslib_ptr sector_ptr);
STATIC boolean           valid_dev_id (const char *dev_id, fslib_error *error);
static boolean           valid_dib (fslib_device_info_block_t *dib, fslib_error *error);
STATIC boolean           valid_file_flags (int flags, fslib_error *error);
STATIC boolean           valid_file_hdr (fslib_device_t *dev, fslib_file_hdr_t *fh, fslib_error *error);
STATIC boolean           valid_file_id (const char *file_id, fslib_error *error);
STATIC boolean           valid_file_name (const char *file_name, fslib_error *error);
STATIC boolean           valid_whence (int whence, fslib_error *error);
STATIC fslib_error       write_file_hdr (fslib_device_t *dev, fslib_file_hdr_t *fh, fslib_ptr fh_ptr);
STATIC fslib_error       write_ram_sector_to_flash (fslib_squeeze_state_rec_t *ss, int flag);
STATIC fslib_error       write_squeeze_log (fslib_squeeze_state_rec_t *ss,int type);
STATIC boolean           device_being_format (int fd);
STATIC int               format_device (const char *dev_id, format_parm_t *fmp);
STATIC void              print_squeeze_status_update (const char *status, uint quiet,int *longest_squeeze_status,char psector,int sector_val);

#if !defined(FSLIB)
STATIC char 		*copy_monlib_to_buffer(const char *, int *, int *);
STATIC fslib_prog_rec_t *find_prog_table_entry (int alg_num);
#include "msg_filesys.c"

/*
 * note that this is an extern in a .c file.
 * it is used here because putting it in a fslib_internals.h 
 * would require a bunch of ifdefs and other badness.
 *
 * the underlying reason is the different types for the 
 * monlib bundle in RP files compared to RSP files.
 */
extern const uchar monlib_bundle[];
                                        
#endif



/******************************************************************************
 * Name: FSLIB_init
 *
 * Description: initialization for the file system
 *
 * Input: 
 *   ralib_table -- pointer to the RALIB vector table to use.
 *
 * Return Value:
 *   fslib_table -- pointer to the FSLIB vector table that the caller
 *     can use to access the file system.
 *
 *******************************************************************************/
struct fslib_vector_table_t *FSLIB_init (ralib_vector_table_t *ralib_table)
{
    fslib_vector_table_t *fslib_vector_table;
    int i;
 
    FSLIB_ralib = ralib_table;
    FSLIB_file_descriptor = 0;
    FSLIB_file_semaphore = 0;

    for (i = 0; i < FSLIB_MAX_FILE_REC; i++) {
        FSLIB_file_rec[i].state = FSLIB_FILE_STATE_CLOSED;
    }

    for (i = 0; i < FSLIB_MAX_DEVICE; i++) {
        FSLIB_device_table[i].valid = FALSE;
    }

    /*
     * Set up the fslib vector table to point at the functions.
     */
    fslib_vector_table = &FSLIB_vector_table;

    fslib_vector_table->version = 0x00010000;
    fslib_vector_table->min_fslib_version = 0x00010000;
    fslib_vector_table->max_fslib_version = 0x00010000;
    fslib_vector_table->init = FSLIB_init;
    fslib_vector_table->open = FSLIB_open;
    fslib_vector_table->close = FSLIB_close;
    fslib_vector_table->read = FSLIB_read;
    fslib_vector_table->write = FSLIB_write;
    fslib_vector_table->lseek = FSLIB_lseek;
    fslib_vector_table->delete = FSLIB_delete;
    fslib_vector_table->undelete = FSLIB_undelete;
    fslib_vector_table->squeeze = FSLIB_squeeze;
    fslib_vector_table->stat = FSLIB_stat;
    fslib_vector_table->fstat = FSLIB_fstat;
    fslib_vector_table->istat = FSLIB_istat;
    fslib_vector_table->fsstat = FSLIB_fsstat;
    fslib_vector_table->getdents = FSLIB_getdents;

    /*
     * Initialize the device drivers for the various
     * file system devices.
     */
    FSLIB_dev_init();
    return(fslib_vector_table);
}


/******************************************************************************
 * Name: FSLIB_open
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
int FSLIB_open (const char *file_id, int flags, ulong mode)
{
    fslib_file_rec_t  *fp;
    char           *separator;
    char           *dev_id;
    char           *file_name;
    char            temp_file_id[FSLIB_FILE_NAME_MAX + 1];
    fslib_device_t *dev;
    fslib_ptr       fh_ptr;
    fslib_error     error;
    fslib_error     rc;
    int             fd;

    PRT(P0E,("open(%s,%lx,%d)\n",file_id,flags,mode));
    
    fp = NULL;
    error = FSLIB_OK;
    /*
     * Do some basic parameter checking
     */
    if (!valid_file_id(file_id,&error))
        fd = error;
    else if (!valid_file_flags(flags,&error))
	fd = error;
    else {
	/* 
	 * All the parameters checked out OK
	 *
         * The device ID is part of a file ID (and therefore ends with a
         * colon), make it a null=terminated string by replacing the
	 * separating colon with a null.
         */
	strncpy(temp_file_id,file_id,sizeof(temp_file_id));
        separator = strchr(temp_file_id,':');
	*separator = '\0';
	file_name = separator + 1;
        dev_id = temp_file_id;
	
        /*
         * Need to get a file record for this file
         */
        fp = get_new_file_rec();
        if (fp == NULL) {
	    fd = FSLIB_ERROR_NO_MORE_FILE_RECS;
        } else {
	    /*
	     *	Remember verbose/crc flags.
	     */
	    if ((flags & O_CRC_MASK) == O_CRC_OFF)
		fp->flag |= FSLIB_NO_CRC;
	    else
		fp->flag &= ~FSLIB_NO_CRC;
	    
	    if ((flags & O_VERBOSE_MASK) == O_VERBOSE_OFF)
		fp->flag |= FSLIB_NO_VERBOSE;
	    else
		fp->flag &= ~FSLIB_NO_VERBOSE;
	    /*
	     *	flags no longer needed.
	     */
	    flags &= ~(O_CRC_MASK | O_VERBOSE_MASK);
	    
	    /*
	     * Need to check to see if the device is OK.  We already know that
             * the device ID is OK, but the device may be removed (like PCMCIA
             * cards) 
	     */
	    dev = init_device(dev_id,FALSE,&error);
	    if (dev == NULL) {
		fd = error;
	    } else {
		/*
		 * Use a semaphore to ensure that dev is not already open
		 */
		dev->open_semaphore++;
		while (dev->open_semaphore > 1) {
		    dev->open_semaphore--;
		    FSLIB_ralib->yield(1);
		    dev->open_semaphore++;
		}
    
		if (dev->format_semaphore) {
		    fd = FSLIB_ERROR_BEING_FORMAT;
		} else {
		    /*
		     * We have found the device and it is OK
		     * we have found a file record for the file.
		     * Things are looking good.
		     * Now there is a special case we have to handle.  If the file
		     * name is "." this is the directory they are trying to open.
		     * Since the directory file does not really exist like it does
		     * in Unix, we have to fake it here.
		     */
		    if (strcmp(file_name,".") == 0) {
			if (flags & O_RDONLY) {
			    /* 
			     * OPEN the directory
			     */
			    fp->dev            = dev;
			    fp->hdr_phys_ptr   = 0;
			    fp->start_phys_ptr = dev->dib.file_system_offset;
			    fp->curr_phys_ptr  = dev->dib.file_system_offset;
			    fp->curr_ptr       = 0;
			    fp->state          = FSLIB_FILE_STATE_OPEN;
			    fp->type           = FSLIB_FILE_TYPE_DIRECTORY;
			    fp->writable       = FALSE;
			    fp->readable       = TRUE;
			    fd = fp->fd;
			} else {
			    fd = FSLIB_ERROR_FILE_IS_READ_ONLY;
			}
		    } else {
			/*
			 * This is not the directory they are looking for, so
			 * find the file and open it if possible.
			 * Scan through the device to find the file header for the
			 * file indicated.
			 */
			fh_ptr = find_file_hdr_by_name(dev,file_name,&fp->hdr,&error);
			if (fh_ptr == -1) {
			    fd = error;               
			} else if (fh_ptr == 0) {
			    /*
			     * There is NO file in the system that matches
			     * the file name given.  Now the flags matter.  
			     * If the flags say to create a file, we create a new
			     * (empty) file and return successfully, otherwise we
			     * fail. 
			     */
			    if (flags != (O_WRONLY | O_APPEND | O_TRUNC | O_CREAT)) {
				fd = FSLIB_ERROR_FILE_NOT_FOUND;
			    } else {
				/* 
				 * If there is already a file open for writing
				 * on this device, we fail.
				 */
				if (dev->file_open_for_write == TRUE) {
				    fd = FSLIB_ERROR_CANNOT_OPEN_ANOTHER_FILE_FOR_WRITE;
				} else {
				    /* 
				     * OPEN the new file for writing
				     * Now create the header and write it to the file system.
				     */
				    fp->old_file_hdr_ptr = 0;
				    rc = create_new_file(dev,fp,file_name,mode);
				    if (rc != FSLIB_OK) {
					fd = rc;
				    } else {
					fd = fp->fd;
				    }
				}
			    }                        
			} else {
			    /*
			     * We have found the file in the file system.
			     * Now flags matter.
			     * If the flags say RDONLY, we return successfully
			     * If the flags say WRONLY | APPEND | TRUNC, 
			     *               or WRONLY | APPEND | TRUNC | CREAT, we
			     *   check the permissions to see if the file is read-only.
			     *   If the file is read only, we fail.  Otherwise we
			     *   delete the existing file and create a new (empty) file
			     *   and return successfully.
			     */
			    if (flags & O_WRONLY) {
				if (fp->hdr.file_flags & FSLIB_FILE_FLAGS_READ_ONLY) {
				    fd = FSLIB_ERROR_FILE_IS_READ_ONLY;
				} else {
				    /* 
				     * If there is already a file open for writing
				     * on this device, we fail.
				     */
				    if (dev->file_open_for_write == TRUE) {
					fd = FSLIB_ERROR_CANNOT_OPEN_ANOTHER_FILE_FOR_WRITE;
				    } else {
					/* 
					 * OPEN the existing file for writing (truncated)
					 * Now create the header and write it to
					 * the file system. 
					 * Remember where the header for the file
					 * of the same name is so we can delete it
					 * when we close this file
					 */
					fp->old_file_hdr_ptr = fh_ptr;
					rc = create_new_file(dev,fp,file_name,mode);
					if (rc != FSLIB_OK) {
					    fd = rc;
					} else {
					    fd = fp->fd;
					}
				    }
				} 
			    } else {
				/* 
				 * Check the CRC for the file and if it is OK,
				 * OPEN the existing file for reading
				 */
				fp->dev            = dev;
				fp->hdr_phys_ptr   = fh_ptr;
				dev_seek(dev,fh_ptr,sizeof(fslib_file_hdr_t),&fp->start_phys_ptr);
				fp->curr_phys_ptr  = fp->start_phys_ptr;
				fp->curr_ptr       = 0;
				fp->type           = FSLIB_FILE_TYPE_NORMAL;
				fp->writable       = FALSE;
				fp->readable       = TRUE;
				rc = check_crc(fp);
				if (rc == FSLIB_OK) {
				    fp->state = FSLIB_FILE_STATE_OPEN;
				    fd = fp->fd;
				} else {
				    fd = rc;
				}
			    }
			}
		    }      
		}
		/*
		 * Release the semaphore
		 */
		dev->open_semaphore--;
	    }
	}
    }
    
    /* 
     * Clean up if the open did not go successfully.
     */
    if ((fp != NULL) && (fp->state != FSLIB_FILE_STATE_OPEN)) {
	fp->state = FSLIB_FILE_STATE_CLOSED;
    }
    
    PRT(P0X,("open(%s,%lx,%d) = %d\n",file_id,flags,mode,fd));
    
    return(fd);
}


/******************************************************************************
 * Name: FSLIB_read
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
int FSLIB_read (int fd, void *buffer, int byte_count)
{
    int               bytes_left_this_file;
    int               read_length;
    int               actual;
    fslib_file_rec_t *fp;

    PRT(P0E,("read(%d,%lx,%d)\n",fd,buffer,byte_count));

    /*
     *	Make sure card is not being formatted.
     */
    if (device_being_format(fd) == TRUE)
	return FSLIB_ERROR_BEING_FORMAT;

/* jack need to add reading for the directory ? */
    /*
     * get the file record for the open file
     */
    fp = get_file_rec(fd);
    if (fp == NULL) {
	actual = FSLIB_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	if (fp->state == FSLIB_FILE_STATE_ERROR) {
	    actual = FSLIB_ERROR_NO_DEVICE;
	} else {
	    /* 
	     * Check to see if the file is open for reading
	     */
	    if (!(fp->readable)) {
		actual = FSLIB_ERROR_FILE_NOT_OPEN_FOR_READ;
	    } else {
		/*
		 * read from the device.  Have dev_read update the current
		 * physical pointer, and then update the current pointer
		 * based on the actual value (if not an error code)
		 */
		bytes_left_this_file = fp->hdr.length - fp->curr_ptr;
		if (bytes_left_this_file < byte_count) {
		    read_length = bytes_left_this_file;
		} else {
		    read_length = byte_count;
		}
		actual = dev_read(fp->dev,
                                  fp->curr_phys_ptr,
                                  buffer,
                                  read_length,
                                  &(fp->curr_phys_ptr));
		if (actual > 0)
		    fp->curr_ptr += actual;
	    }
	}
    }

    PRT(P0X,("read(%d,%lx,%d) = %d\n",fd,buffer,byte_count,actual));

    return(actual);
}


/******************************************************************************
 * Name: FSLIB_close
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
int FSLIB_close (int fd)
{
    fslib_error       rc;
    fslib_error       error;
    fslib_file_rec_t *fp;
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    
    PRT(P0E,("close(%d)\n",fd));

    /*
     *	Make sure card is not being formatted.
     */
    if (device_being_format(fd) == TRUE)
	return FSLIB_ERROR_BEING_FORMAT;

    fh = &file_hdr;
    /*
     * get the file record for the open file
     */
    fp = get_file_rec(fd);
    if (fp == NULL) {
	rc = FSLIB_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	if (fp->state == FSLIB_FILE_STATE_ERROR) {
	    rc = FSLIB_ERROR_NO_DEVICE;
	    fp->state = FSLIB_FILE_STATE_CLOSED;
	} else {
	    if (fp->readable) {
		/* 
		 * close the file that was open for reading
		 */
		fp->state = FSLIB_FILE_STATE_CLOSED;
		rc = FSLIB_OK;
	    } else {
		/* 
		 * close the file that was open for writing
		 * - fill out the rest of the file header
		 * - write the file header to the file system
		 * - change the header flags to valid
		 * - write the header flags to the file system
		 * - update the fields in the device structure
		 */
		fp->hdr.length = fp->curr_ptr;
		fp->hdr.next = ROUND_UP_TO_4_BYTES(fp->curr_phys_ptr);
		fp->hdr.crc = calc_crc(fp,&error);
		if (fp->hdr.crc == 0) {
		    rc = error;
		} else {
		    rc = write_file_hdr(fp->dev,&fp->hdr,fp->hdr_phys_ptr);
		    if (rc == FSLIB_OK) {
			if ((fp->flag & FSLIB_FILE_ERR) == 0) {
			    /*
			     * Just before we mark this file valid, we need to
			     * delete the existing file of the same name (if one
			     * exists).  This information was found and kept when
			     * the file was opened.
			     */
			    if (fp->old_file_hdr_ptr != 0) {
				rc = read_file_hdr(fp->dev,fp->old_file_hdr_ptr,fh,NULL);
				if (rc == FSLIB_OK) {
				    rc = mark_file_deleted(fp->dev,fh,fp->old_file_hdr_ptr);
				}
			    }
			    /*
			     * Now mark the new file valid
			     */
			    if (rc == FSLIB_OK) {
				fp->hdr.hdr_flags = FSLIB_HDR_FLAGS_VALID;
				rc = write_file_hdr(fp->dev,&fp->hdr,fp->hdr_phys_ptr);
			    }
			} else {
			    /*
			     *	Mark the file invalid.
			     */
			    fp->hdr.hdr_flags = FSLIB_HDR_FLAGS_ERROR;
			    rc = write_file_hdr(fp->dev,&fp->hdr,fp->hdr_phys_ptr);
			    fp->flag &= ~FSLIB_FILE_ERR;
			}
		    }
		}
		if (fp->hdr.next <= fp->dev->file_system_end) 
		    fp->dev->next_new_file_hdr_ptr = fp->hdr.next;
		else
		    fp->dev->next_new_file_hdr_ptr = 0;
		fp->state = FSLIB_FILE_STATE_CLOSED;
		fp->dev->file_open_for_write = FALSE;
	    }
	}
    }
    
    PRT(P0X,("close(%d) = %d\n",fd,rc));
    return((int)rc);
}


/******************************************************************************
 * Name: FSLIB_lseek
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
int FSLIB_lseek (int fd, int offset, int whence)
{
    int               curr_ptr;
    fslib_error       error;
    fslib_error       rc;
    fslib_file_rec_t *fp;
    
    PRT(P0E,("lseek(%d,%d,%d)\n",fd,offset,whence));

    /*
     *	Make sure card is not being formatted.
     */
    if (device_being_format(fd) == TRUE)
	return FSLIB_ERROR_BEING_FORMAT;

    /*
     * Do some basic parameter checking
     */
    if (!valid_whence(whence,&error)) {
	curr_ptr = error;
    } else {
	/*
	 * get the file record for the open file
	 */
	fp = get_file_rec(fd);
	if (fp == NULL) {
	    curr_ptr = FSLIB_ERROR_BAD_FILE_DESCRIPTOR;
	} else {
	    if (fp->state == FSLIB_FILE_STATE_ERROR) {
		curr_ptr = FSLIB_ERROR_NO_DEVICE;
	    } else {
		if (fp->type == FSLIB_FILE_TYPE_DIRECTORY) {
		    /* 
		     * Seek for a directory
		     */
		    if (whence != SEEK_SET) {
			curr_ptr = FSLIB_ERROR_INVALID_WHENCE;
		    } else {
			rc = find_file_hdr_by_phys_ptr(fp->dev,offset,&fp->hdr);
			if (rc != FSLIB_OK) {
			    curr_ptr = rc;
			} else {
			    fp->curr_phys_ptr = offset;
			    curr_ptr = offset;
			}
		    }
		} else {
		    /* 
		     * Seek for a normal file
		     * One can only seek in a file that is opened for reading.
		     * (jack, should seeking a writable file cause an error?)
		     * Set the curr_ptr depending on the value of whence
		     */
		    if (fp->readable != TRUE) {
			curr_ptr = fp->curr_ptr;
		    } else {
			switch (whence) {
			default: /* to keep the complier quiet */
			case SEEK_SET:
			    curr_ptr = offset;
			    break;
			case SEEK_CUR:
			    curr_ptr = fp->curr_ptr + offset;
			    break;
			case SEEK_END:
			    curr_ptr = fp->hdr.length + offset;
			    break;
			}
			/* 
			 * now make sure that we have a reasonable curr_ptr
			 */
			if (curr_ptr > fp->hdr.length)
			    curr_ptr = fp->hdr.length;
			else if (curr_ptr < 0)
			    curr_ptr = 0;
			/* 
			 * now set the physical address to match the logical one
			 * and update the curr_ptr in the file record.
			 */
			dev_seek(fp->dev,fp->start_phys_ptr,curr_ptr,&fp->curr_phys_ptr);
			fp->curr_ptr = curr_ptr;
		    }
		}
	    }
	}
    }
    PRT(P0X,("lseek(%d,%d,%d) = %d\n",fd,offset,whence,curr_ptr));
    return(curr_ptr);
}


/******************************************************************************
 * Name: FSLIB_write
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
int FSLIB_write (int fd, void *buffer, int byte_count)
{
    int               actual;
    fslib_file_rec_t *fp;
    fslib_ptr         next;
    
    PRT(P0E,("write(%d,%lx,%d)\n",fd,buffer,byte_count));

    /*
     *	Make sure card is not being formatted.
     */
    if (device_being_format(fd) == TRUE)
	return FSLIB_ERROR_BEING_FORMAT;

    /*
     * get the file record for the open file
     */
    fp = get_file_rec(fd);
    if (fp == NULL) {
	actual = FSLIB_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	if (fp->state == FSLIB_FILE_STATE_ERROR) {
	    actual = FSLIB_ERROR_NO_DEVICE;
	} else {
	    if (fp->writable != TRUE) {
		actual = FSLIB_ERROR_FILE_NOT_OPEN_FOR_WRITE;
	    } else {
		/* 
		 * write the file
		 */
		actual = dev_write(fp->dev,
				   (char *)buffer,
				   fp->curr_phys_ptr,
				   byte_count,
				   &next);
		if (actual > 0) {
		    fp->curr_ptr += actual;
		    fp->curr_phys_ptr = next;
		}
		if (actual != byte_count) {
		    /*
		     *	Flash write is unsuccessful or
		     *	incomplete. Either case, we mark
		     *	the error and let FSLIB_close()
		     *	to mark the file as invalid.
		     */
		    fp->flag |= FSLIB_FILE_ERR;
		}
	    }
	}
    }
    
    PRT(P0X,("write(%d,%lx,%d) = %d\n",fd,buffer,byte_count,actual));
    return(actual);
}


/******************************************************************************
 * Name: FSLIB_fstat
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
int FSLIB_fstat (int fd, stat_t *status)
{
    fslib_error       rc;
    fslib_file_rec_t *fp;
    
    
    PRT(P0E,("fstat(%d,%lx)\n",fd,status));

    /*
     *	Make sure card is not being formatted.
     */
    if (device_being_format(fd) == TRUE)
	return FSLIB_ERROR_BEING_FORMAT;

    /*
     * get the file record for the open file
     */
    fp = get_file_rec(fd);
    if (fp == NULL) {
	rc = FSLIB_ERROR_BAD_FILE_DESCRIPTOR;
    } else {
	if (fp->state == FSLIB_FILE_STATE_ERROR) {
	    rc = FSLIB_ERROR_NO_DEVICE;
	} else {
	    /* 
	     * write the information to the status buffer
	     */
	    convert_file_hdr_to_stat(&fp->hdr,status);
	    rc = FSLIB_OK;
	}
    }            

    PRT(P0X,("fstat(%d,%lx) = %d\n",fd,status,rc));
    return((int)rc);
}


/******************************************************************************
 * Name: FSLIB_stat
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
int FSLIB_stat (const char *file_id, stat_t *status)
{
    fslib_error  rc;
    int          fd;
    
    PRT(P0E,("stat(%s,%lx)\n",file_id,status));

    /*
     * Open the file for reading
     */
    fd = FSLIB_open(file_id, O_RDONLY | O_CRC_OFF, 0);
    if (fd < 0) {
	rc = fd;
    } else {
	/*
	 * get the status using the file descriptor
	 */
	rc = FSLIB_fstat(fd,status);
	(void)FSLIB_close(fd);
    }

    PRT(P0X,("stat(%s,%lx) = %d\n",file_id,status,rc));
    return((int)rc);
}


/******************************************************************************
 * Name: FSLIB_istat
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
int FSLIB_istat (const char *dev_id, int index, stat_t *status)
{
    fslib_error       error;
    char              temp_dev_id[FSLIB_DEV_ID_MAX + 1];
    char             *separator;
    fslib_device_t   *dev;
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    fslib_ptr         fh_ptr;
    
    PRT(P0E,("istat(%s,%d,%lx)\n",dev_id,index,status));

    error = FSLIB_OK;
    fh = &file_hdr;
    /*
     * Do some basic parameter checking
     */
    if (valid_dev_id(dev_id,&error)) {
	/*
	 * If the device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  We must have
	 * a copy of the dev_id if we are going to write a null at the end.
	 */
	strncpy(temp_dev_id,dev_id,sizeof(temp_dev_id));
	separator = strchr(temp_dev_id,':');
	if (separator != NULL) 
	    *separator = '\0';
	else
	    temp_dev_id[FSLIB_DEV_ID_MAX] = '\0';
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards) 
	 */
	dev = init_device(temp_dev_id,FALSE,&error);
	if (dev != NULL) {
	    /*
	     * We have found the device and it is OK
	     * Scan through the device to find the file header for the
	     * file indicated.
	     */
	    fh_ptr = find_file_hdr_by_index(dev,index,fh,&error);
	    
	    if (fh_ptr != -1) {
		if (fh_ptr == 0) {
		    error = FSLIB_ERROR_FILE_NOT_FOUND;
		} else {
		    convert_file_hdr_to_stat(fh,status);
		    error = FSLIB_OK;
		}
	    }
	}
    }
    
    PRT(P0X,("istat(%s,%d,%lx) = %d\n",dev_id,index,status,error));
    return((int)error);
}


#if !defined(FSLIB)
/* 
 * check if monlib on device dev_id 
 * does not match the monlib bundled in this image.
 * logs warning message if log_warning is TRUE.
 * returns TRUE if mismatch.
 * 
 */ 
#define MONLIB_END_NAME_OFFSET   64    /* ELF header starts at byte 64 */
#define MONLIB_START_NAME_OFFSET 16    /* first byte of name */
boolean check_monlib_mismatch (const char *dev_id, boolean log_warning)
{
    char *bundled_monlib = (uchar *)&monlib_bundle[0];
    char *device_monlib,
	  *device_monlib_name,
	  device_id_with_colon[FSLIB_DEV_ID_MAX+1];
    char *bundled_monlib_name;
    int size = MONLIB_END_NAME_OFFSET,
	error;

#if defined(C7000)
    bundled_monlib = copy_bndl_monlib_to_buffer(&size);
    if ( !bundled_monlib ) 
	return(FALSE);
#endif
    bundled_monlib_name = (uchar *)(bundled_monlib + MONLIB_START_NAME_OFFSET);
    strcpy(device_id_with_colon,dev_id);	
    strcat(device_id_with_colon,":");
    device_monlib = copy_monlib_to_buffer(device_id_with_colon,&size,&error);
    if ( !device_monlib ) {
#if defined(C7000)
	free(bundled_monlib);
#endif
	return(FALSE);
    }
    device_monlib_name = device_monlib + MONLIB_START_NAME_OFFSET;

    if (strncmp(bundled_monlib_name,
		device_monlib_name,
		MONLIB_END_NAME_OFFSET-MONLIB_START_NAME_OFFSET)) {
	if (log_warning) {
	    errmsg(&msgsym(DEV,FILESYS),dev_id,
		   "format is from a different router type");
#if defined(C7000)
	    errmsg(&msgsym(DEV,FILESYS),dev_id,
		   "reformat is required");
#else
	    errmsg(&msgsym(DEV,FILESYS),dev_id,
		   "reformat is recommended");
#endif
	}
	return(TRUE);
    }
    return(FALSE);
}
#endif !defined(FSLIB)


/******************************************************************************
 * Name: FSLIB_fsstat
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
int FSLIB_fsstat (const char *dev_id, file_sys_stat_t *status)
{
    fslib_error       rc;
    char             *separator;
    char              temp_dev_id[FSLIB_DEV_ID_MAX + 1];
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    fslib_ptr         ptr;
    fslib_ptr         old_ptr;
    fslib_device_t   *dev;
    int               first_sector;
    int               last_sector;
    int               sector;
    
    PRT(P0E,("fsstat(%s,%lx)\n",dev_id,status));

    fh = &file_hdr;

    /*
     * Do some basic parameter checking
     */
    rc = FSLIB_OK;
    if (valid_dev_id(dev_id,&rc)) {
	/*
	 * The device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  
	 */
	strncpy(temp_dev_id,dev_id,sizeof(temp_dev_id));
	separator = strchr(temp_dev_id,':');
	if (separator != NULL) 
	    *separator = '\0';
	else
	    temp_dev_id[FSLIB_DEV_ID_MAX] = '\0';
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards) 
	 */
	dev = init_device(temp_dev_id,FALSE,&rc);
	if (dev != NULL) {
	    /*
	     *	Make sure card is not being formatted.
	     */
	    if (dev->format_semaphore) {
		rc = FSLIB_ERROR_BEING_FORMAT;
	    } else {
		/*
		 * We have found the device and it is OK
		 * copy all the status stuff from the device structure,
		 * and the device stuff from the dib in the device structure
		 */
		status->writable = dev->writable;
		status->file_open_for_write = dev->file_open_for_write;
		status->incomplete_stats = FALSE;
		status->unrecovered_errors = FALSE;
		if (dev->is_busy && dev->busy_reason == FS_BUSY_SQUEEZE)
		    status->squeeze_in_progress = TRUE;
		else
		    status->squeeze_in_progress = FALSE;
		memcpy(&(status->dib), &(dev->dib), sizeof(dev->dib));
		status->dev_num = dev->dev_num;
		status->file_hdr_size = sizeof(fslib_file_hdr_t);
		
		/*
		 * Initialize all the usage info counts
		 */
		status->files_ok = 0;
		status->files_ok_bytes = 0;
		status->files_deleted = 0;
		status->files_deleted_bytes = 0;
		status->files_with_errors = 0;
		status->files_with_errors_bytes = 0;
#if !defined(FSLIB)
		check_monlib_mismatch(dev_id,TRUE);	
#endif
		/* 
		 * now go through the file system and gather all the usage info
		 */
		old_ptr = 0;
		ptr = dev->dib.file_system_offset;
		while (ptr <= dev->file_system_end) {
		    /* 
		     * Keep track of the previous value of the pointer and
		     * use it to make sure we don't have file system
		     * problems like loops that would cause problems.
		     * The new file pointer must always be greater than
		     * the old one.
		     */
		    if (ptr <= old_ptr) {
			rc = FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT;
			break;
		    }
		    old_ptr = ptr;
		    
		    /*
		     * Let other processes have a chance to run
		     */
		    ralib_yield(0);
		    
		    /* 
		     * read in the file header 
		     * If we can't read the entire file header, we assume
		     * all is OK, we have just run into the end of the file system
		     */
		    rc = read_file_hdr(dev,ptr,fh,NULL);
		    if (rc == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
			rc = FSLIB_OK;
			break;
		    }
		    if (rc != FSLIB_OK) {
			break;
		    }
		    
		    /*
		     * if we reach an unused file header, we are done --> end of file system
		     */
		    if (fh->hdr_flags == 0) {
			break;
		    }
		    
		    /*
		     * if the file header has an "error", skip over it and continue
		     */
		    if (fh->hdr_flags == FSLIB_HDR_FLAGS_ERROR) {
			status->files_with_errors++;
			status->files_with_errors_bytes += ROUND_UP_TO_4_BYTES(fh->length);
			ptr = fh->next;
			continue;
		    }
		    
		    /* 
		     * if the file header is "in use" only, and the device
		     * structure says that we have a file open for
		     * writing, then we have come to the end of the file
		     * system If the device structure does not say we have
		     * a file open for writing, then this file header is
		     * an error that needs fixing -- fix the file header
		     * and continue.  		 
		     */
		    if (fh->hdr_flags == FSLIB_HDR_FLAGS_IN_USE) {
			if (dev->file_open_for_write == TRUE) {
			    status->incomplete_stats = TRUE;
			    break;
			} else {
			    fix_in_use_file_hdr(dev,fh,ptr,&rc);
			    if (fh->next == 0) {
				status->incomplete_stats = TRUE;
				status->unrecovered_errors = TRUE;
				break;
			    } else {
				status->files_with_errors++;
				/* there are no bytes to count for IN USE headers */
				ptr = fh->next;
				continue;
			    }
			}
		    }
		    
		    /* 
		     * If the file header has been "created", but is not
		     * marked valid, and the device structure says that we
		     * have a file open for writing, then we have come to
		     * the end of the file system. If the device structure
		     * does not say we have a file open for writing, then
		     * this file header is an error that needs fixing --
		     * fix up the file system, and continue.
		     */
		    
		    if (fh->hdr_flags == FSLIB_HDR_FLAGS_CREATED) {
			if (dev->file_open_for_write == TRUE) {
			    status->incomplete_stats = TRUE;
			    break;
			} else {
			    fix_created_file_hdr(dev,fh,ptr,&rc);
			    if (fh->next == 0) {
				status->incomplete_stats = TRUE;
				status->unrecovered_errors = TRUE;
				break;
			    } else {
				status->files_with_errors++;
				status->files_with_errors_bytes += ROUND_UP_TO_4_BYTES(fh->length);
				ptr = fh->next;
				continue;
			    }
			}
		    }
		    
		    /*
		     * if the file header is valid we need to check the
		     * has been deleted or not and decide what stat to
		     * increment 
		     */
		    if (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID) {
			if ((fh->deleted ^ fh->undeleted) == 0) {
			    status->files_ok++;
			    status->files_ok_bytes += ROUND_UP_TO_4_BYTES(fh->length);
			} else {
			    status->files_deleted++;
			    status->files_deleted_bytes += ROUND_UP_TO_4_BYTES(fh->length);
			}
			ptr = fh->next;
			continue;
		    }

		    /*
		     * We come here because file header contains
		     * unknown flag. Assume file header is corrupted.
		     * Stop the search here.
		     */
		    status->incomplete_stats = TRUE;
		    status->unrecovered_errors = TRUE;
		    break;
		}
		/*
		 * Now calculate the total usage based on the offset of
		 * the last file header found (this will normally be the
		 * place where the next new file hdr would start
		 */
		status->bytes_used = get_bytes(dev,dev->dib.file_system_offset,ptr);
		status->bytes_available = get_bytes(dev,ptr,dev->file_system_end) + 1;
		
		/*
		 * count the bad sectors
		 */
		status->bad_sectors = 0;
		status->spared_sectors = 0;
		first_sector = get_sector_num(dev,dev->dib.file_system_offset);
		last_sector = get_sector_num(dev,dev->file_system_end);
		for (sector = first_sector; sector <= last_sector; sector++) {
		    
		    /*
		     * Let other processes have a chance to run
		     */
		    ralib_yield(0);
		    
		    switch (good_sector(dev,sector)) {
		    default:
		    case FSLIB_SECTOR_GOOD:
			break;
		    case FSLIB_SECTOR_BAD:
			status->bad_sectors++;
			break;
		    case FSLIB_SECTOR_SPARED:
			status->spared_sectors++;
			break;
		    }
		}
	    }
	}    
    }
    
    PRT(P0X,("fsstat(%s,%lx) = %d\n",dev_id,status,rc));
    return((int)rc);
}


/******************************************************************************
 * Name: FSLIB_delete
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
int FSLIB_delete (const char *file_id)
{
    int               error;
    char              temp_file_id[FSLIB_FILE_ID_MAX + 1];
    char             *separator;
    char             *dev_id;
    char             *file_name;
    fslib_device_t   *dev;
    fslib_ptr         fh_ptr;
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    
    PRT(P0E,("delete(%s)\n",file_id));

    error = FSLIB_OK;
    fh = &file_hdr;
    /*
     * Do some basic parameter checking
     */
    if (valid_file_id(file_id,&error)) {
	/*
	 * The device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  
	 */
	strncpy(temp_file_id,file_id,sizeof(temp_file_id));
	separator = strchr(temp_file_id,':');
	*separator = '\0';
	file_name = separator + 1;
	dev_id = temp_file_id;
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards) 
	 */
	dev = init_device(dev_id,FALSE,&error);
	if (dev != NULL) {
	    if (dev->format_semaphore == 0) {
		if (dev->writable == TRUE) {
		    /*
		     * We have found the device and it is OK
		     * Scan through the device to find the file header for the
		     * file indicated.
		     */
		    fh_ptr = find_file_hdr_by_name(dev,file_name,fh,&error);
		    if (fh_ptr != -1) {
			if (fh_ptr == 0) {
			    error = FSLIB_ERROR_FILE_NOT_FOUND;
			} else {
			    error = mark_file_deleted(dev,fh,fh_ptr);
			}
		    }
		} else {
		    error = FSLIB_ERROR_DEVICE_IS_READ_ONLY;
		}
	    } else {
		error = FSLIB_ERROR_BEING_FORMAT;
	    }
	}
    }
    
    PRT(P0X,("delete(%s) = %d\n",file_id,error));
    return((int)error);
}


/******************************************************************************
 * Name: FSLIB_undelete
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
int FSLIB_undelete (const char *dev_id, int index, int mode)
{
    fslib_error       error;
    char              temp_dev_id[FSLIB_DEV_ID_MAX + 1];
    char             *separator;
    fslib_device_t   *dev;
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    fslib_ptr         fh_ptr;
    fslib_file_hdr_t  exist_file_hdr;
    fslib_file_hdr_t *efh;
    fslib_ptr         efh_ptr;
    
    PRT(P0E,("undelete(%s,%d,%d)\n",dev_id,index,mode));

    error = FSLIB_OK;
    fh = &file_hdr;
    efh = &exist_file_hdr;
    /*
     * Do some basic parameter checking
     */
    if (valid_dev_id(dev_id,&error)) {
	/*
	 * The device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  
	 */
	strncpy(temp_dev_id,dev_id,sizeof(temp_dev_id));
	separator = strchr(temp_dev_id,':');
	if (separator != NULL) 
	    *separator = '\0';
	else
	    temp_dev_id[FSLIB_DEV_ID_MAX] = '\0';
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards) 
	 */
	dev = init_device(temp_dev_id,FALSE,&error);
	if (dev != NULL) {
	    if (dev->format_semaphore) {
		error = FSLIB_ERROR_BEING_FORMAT;
	    } else {
		if (dev->writable == FALSE)
		    error = FSLIB_ERROR_DEVICE_IS_READ_ONLY;
		else {
		    /*
		     * We have found the device and it is OK
		     * Scan through the device to find the file header for the
		     * file indicated.
		     */
		    fh_ptr = find_file_hdr_by_index(dev,index,fh,&error);
		    if (fh_ptr != -1) {
			if (fh_ptr == 0) {
			    error = FSLIB_ERROR_FILE_NOT_FOUND;
			} else {
			    if (fh->hdr_flags != FSLIB_HDR_FLAGS_VALID) {
				error = FSLIB_ERROR_FILE_NOT_FOUND;
			    } else {
				/*
				 * find out if there is an undeleted file with the
				 * same name as the file we are undeleting
				 */
				efh_ptr = find_file_hdr_by_name(dev,fh->file_name,efh,&error);
				if (efh_ptr != -1) {
				    if (efh_ptr == 0) {
					/*
					 * we DID NOT find another file with the same name
					 */
					error = mark_file_undeleted(dev,fh,fh_ptr);
				    } else {
					/*
					 * We DID find another file with the same name -- we
					 * need to check to see if we should delete it.
					 */
					if (mode == UNDELETE_UNIQUE) {
					    error = FSLIB_ERROR_FILE_NOT_UNIQUE;
					} else {
					    error = mark_file_deleted(dev,efh,efh_ptr);
					    if (error == FSLIB_OK) {
						error = mark_file_undeleted(dev,fh,fh_ptr);
					    }
					}
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }
    
    PRT(P0X,("undelete(%s,%d,%d) = %d\n",dev_id,index,mode,error));
    return((int)error);
}


/******************************************************************************
 * Name: FSLIB_squeeze
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
int FSLIB_squeeze (const char *dev_id, int flag)
{
    fslib_error                error;
    char                       temp_dev_id[FSLIB_DEV_ID_MAX + 1];
    char                      *separator;
    fslib_squeeze_state_rec_t  squeeze_state;
    fslib_squeeze_state_rec_t *ss;
    int                        bytes_left_this_sector;
    fslib_ptr                  ptr;
        
    PRT(P0E,("squeeze(%s)\n",dev_id));

    error = FSLIB_ERROR;
    ss = &squeeze_state;
    ss->last_status_length = 0;
    /*
     * Do some basic parameter checking
     */
    if (valid_dev_id(dev_id,&error)) {
	/*
	 * The device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  
	 */
	strncpy(temp_dev_id,dev_id,sizeof(temp_dev_id));
	separator = strchr(temp_dev_id,':');
	if (separator != NULL) 
	    *separator = '\0';
	else
	    temp_dev_id[FSLIB_DEV_ID_MAX] = '\0';
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards) 
	 */
	ss->dev = init_device(temp_dev_id,FALSE,&error);
	if (ss->dev != NULL) {
 	    /*
 	     * Use a semaphore to ensure that dev is not being opened/squeezed.
 	     * Setting squeeze in progress first means that "dir" may (rarely)
 	     * print "squeeze in progress" when the semaphore has not yet
 	     * been obtained for the squeeze - but since squeeze is about to
 	     * happen anyway, it is not worth disabling interrupts.
 	     */
 	
 	    ss->dev->busy_reason = FS_BUSY_SQUEEZE;
 	    ss->dev->is_busy++;

 	    ss->dev->open_semaphore++;
 	    while (ss->dev->open_semaphore > 1) {
 		ss->dev->open_semaphore--;
 		FSLIB_ralib->yield(1);
 		ss->dev->open_semaphore++;
 	    }

	    if (ss->dev->format_semaphore) {
		error = FSLIB_ERROR_BEING_FORMAT;
	    } else {
		if (ss->dev->writable == FALSE) {
		    error = FSLIB_ERROR_DEVICE_IS_READ_ONLY;
		} else {
		    /*
		     * We have found the device and it is OK
		     * If there is a file open for write, we cannot squeeze the file
		     * system (jack, should we do this, or allow the squeezer to force
		     * the file to close).
		     */
		    if (ss->dev->file_open_for_write) {
			error = FSLIB_ERROR_FILE_OPEN_FOR_WRITE;
		    } else {
			error = allocate_squeeze_buffer(ss->dev,&ss->ram_buffer);
			if (error == FSLIB_OK) {
			    error = get_squeeze_state(ss);
			    if (error == FSLIB_OK) {
				/*
				 * While the state structure say we are not done, we loop
				 * building a sector in ram and writing it to flash
				 */
				while ((ss->done == FSLIB_NOT_DONE) && (error == FSLIB_OK)) {
				
				    /*
				     * Let other processes have a chance to run
				     */
				    ralib_yield(0);
				
				    error = build_ram_sector(ss);
				    if (error == FSLIB_OK) {
				    
					/*
					 * Let other processes have a chance to run
					 */
					ralib_yield(0);
				    
					error = write_ram_sector_to_flash(ss,flag);
					if (error == FSLIB_OK) {
					    switch (ss->state) {
					    case FSLIB_SQUEEZE_START_OF_HDR:
					    case FSLIB_SQUEEZE_MIDDLE_OF_HDR:
						ss->rd_ptr_save = ss->old_hdr_ptr;
						break;
					    case FSLIB_SQUEEZE_DATA:
					    default:
						ss->rd_ptr_save = ss->rd_ptr;
						break;
					    }
					}
				    }
				} /* end while */
				/*
				 * now we need to erase all the rest of the sectors at
				 * the end of the file system.  First move the write
				 * pointer to point to the next sector unless it is
				 * pointing to the beginning of a sector already;
				 */
				if (error == FSLIB_OK) {
				    bytes_left_this_sector = get_bytes_left_this_sector(ss->dev,ss->wr_ptr);
				    if (bytes_left_this_sector != ss->dev->dib.sector_size) {
					dev_seek(ss->dev,ss->wr_ptr,bytes_left_this_sector,&ss->wr_ptr);
				    }
				    while (ss->wr_ptr < ss->dev->file_system_end) {
				    
					/*
					 * Let other processes have a chance to run
					 */
					ralib_yield(0);
				    
					error = erase_sector(ss->dev,ss->wr_ptr);
					if (error != FSLIB_OK) {
					    break;
					} else {
					    dev_seek(ss->dev,ss->wr_ptr,ss->dev->dib.sector_size,&ss->wr_ptr);
					}
				    }
				}
				/*
				 * now erase the squeeze log
				 */
				if (error == FSLIB_OK) {
				    ptr = ss->dev->dib.squeeze_log_offset;
				    while (ptr < ss->dev->squeeze_log_end) {
				    
					/*
					 * Let other processes have a chance to run
					 */
					ralib_yield(0);
				    
					error = erase_sector(ss->dev,ptr);
					print_squeeze_status_update("Erasing squeeze log", flag & SQUEEZE_QUIET,
								    &ss->last_status_length,0,0);
					if (error != FSLIB_OK) {
					    break;
					} else {
					    dev_seek(ss->dev,ptr,ss->dev->dib.sector_size,&ptr);
					}
				    }
				}
				/*
				 * now erase the squeeze buffer
				 */
				if (error == FSLIB_OK) {
				
				    /*
				     * Let other processes have a chance to run
				     */
				    ralib_yield(0);
				
				    error = erase_sector(ss->dev,ss->dev->dib.squeeze_buffer_offset);
				    if (error == FSLIB_OK) {
					
					(void) init_device(temp_dev_id,TRUE,&error);
				    }
				}
			    }
			    free_squeeze_buffer(ss->ram_buffer);
			}
		    }
		}
	    }
	    /*
	     * Release the semaphore we obtained.  (That device block may
	     * have been marked invalid by this time, but we still must release
	     * its semaphore).   Again, no need to disable interrupts.
	     */

	    ss->dev->is_busy--;
	    ss->dev->open_semaphore-- ;
	}
    }
    
    PRT(P0X,("squeeze(%s) = %d\n",dev_id,error));
    
    return((int)error);
}


/*****************************************************************************
 * Name: get_busy_info
 *
 * Description: Sets passed in variable to:
 *                      FS_BUSY_IDLE if the device appears idle
 *                      FS_BUSY_SQUEEZE if squeeze is in progress.
 *                      FS_BUSY_FORMAT if format is in progress.
 *		Returns error if device is not found.
 *
 * Input: 
 *   dev_id -- the device ID of the device containing the file to undelete
 *   busy_p -- pointer to squeeze in progress variable
 *
 * Output:
 *   *busy_p --  location which will contain the answer, providing that
 *               we return FSLIB_OK.
 *
 * Return Value:
 *   int -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 ****************************************************************************/
STATIC fslib_error get_busy_info (const char *dev_id, int *busy_p)
{
    fslib_error                error;
    char                       temp_dev_id[FSLIB_DEV_ID_MAX + 1];
    char                      *separator;
    fslib_device_t            *dev;
        
    PRT(P0E,("get_busy_info(%s)\n",dev_id));

    error = FSLIB_OK;
    /*
     * Do some basic parameter checking
     */
    if (valid_dev_id(dev_id,&error)) {
	/*
	 * The device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  
	 */
	strncpy(temp_dev_id,dev_id,sizeof(temp_dev_id));
	separator = strchr(temp_dev_id,':');
	if (separator != NULL) 
	    *separator = '\0';
	else
	    temp_dev_id[FSLIB_DEV_ID_MAX] = '\0';
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards).  If it is OK, find out whether device is busy.
	 */
	dev = init_device(temp_dev_id,FALSE,&error);
	if (dev != NULL) {
	    if (dev->is_busy)
		*busy_p = dev->busy_reason;
	    else
		*busy_p = FS_BUSY_IDLE;
	}
	
    }
    PRT(P0X,("get_busy_info(%s) = %d\n",dev_id,error));
    return (error);
}


/******************************************************************************
 * Name: allocate_squeeze_buffer
 *
 * Description: calls malloc to get a temporary squeeze buffer for a sector.
 *
 * Input: 
 *   dev - pointer to the device structure for the device being squeezed
 *   ram_buffer_ptr -- pointer to the pointer variable inwhich to put
 *       a pointer to the squeeze buffer
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error allocate_squeeze_buffer (fslib_device_t *dev, char **ram_buffer_ptr)
{
    int rc;

#ifdef FSLIB

    *ram_buffer_ptr = NULL;
    rc = FSLIB_ERROR_COULD_NOT_ALLOC_SQUEEZE_BUFFER;

#else
    
    *ram_buffer_ptr = malloc(dev->dib.sector_size);
    if (*ram_buffer_ptr == NULL) {
	rc = FSLIB_ERROR_COULD_NOT_ALLOC_SQUEEZE_BUFFER;
    } else {
	rc = FSLIB_OK;
    }
    PRT(P2,("allocate_squeeze_buffer(%8x,%8x) = %d(%8x)\n",dev,ram_buffer_ptr,rc,*ram_buffer_ptr));

#endif

    return(rc);
}


/******************************************************************************
 * Name: free_squeeze_buffer
 *
 * Description: calls free to return a squeeze buffer
 *
 * Input: 
 *   ram_buffer -- pointer to the pointer squeeze buffer to free
 *
 * Return Value: None
 *
 *******************************************************************************/
STATIC void free_squeeze_buffer (char *ram_buffer)
{
#ifdef FSLIB

    /* do nothing, since malloc/free are not allowed in fslib */

#else
    
    free(ram_buffer);
    PRT(P2,("free_squeeze_buffer(%8x)\n",ram_buffer));
    
#endif
}


/******************************************************************************
 * Name: get_squeeze_state
 *
 * Description: Gets the squeeze state from the squeeze log (if any).
 *   If there are no records in the squeeze log, this function initializes the
 *   squeeze state to the right values for a fresh squeeze.
 *
 * Input: 
 *   ss -- a pointer to the squeeze state structure to fill out.
 *       The following fields must be valid when this function is called:
 *
 *       ram_buffer -- points to a RAM buffer big enough to hold a sector
 *
 *   dev - pointer to the device structure for the device being squeezed
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error get_squeeze_state (fslib_squeeze_state_rec_t *ss)
{
    int rc;
    fslib_squeeze_log_rec_t  squeeze_log;
    fslib_squeeze_log_rec_t *sl;

    PRT(P1E,("get_squeeze_state()\n"));

    sl = &squeeze_log;
    
    /*
     * get the last squeeze record from the squeeze log (if a squeeze was
     * going on).
     */
    rc = find_squeeze_log_rec(ss,sl);

    if (rc == FSLIB_OK) {
	/*
	 * regardless of what we are doing, we can set these parts of the squeeze
	 * state to these values.
	 */
	ss->done = FSLIB_NOT_DONE;
	ss->ram_ptr    = ss->ram_buffer;
	ss->bytes_left_this_sector = ss->dev->dib.sector_size;
	/*
	 * find out if there was a squeeze going on.  If there was NOT a
	 * squeeze going on the last write log will be marked invalid, so we
	 * just start from the beginning of the file system
	 */
	if (sl->flags != FSLIB_SQUEEZE_LOG_FLAGS_VALID) {
	    /* 
	     * sector was all erased -- no squeeze going on.  Set up
	     * erything to squeeze from the beginning of the file system.
	     */
	    ss->state = FSLIB_SQUEEZE_START_OF_HDR;
	    ss->file_index = 1;
	    ss->old_hdr_ptr = ss->dev->dib.file_system_offset;
	    ss->rd_ptr      = ss->dev->dib.file_system_offset;
	    ss->rd_ptr_save = ss->dev->dib.file_system_offset;
	    ss->wr_ptr      = ss->dev->dib.file_system_offset;
	} else {
	    /*
	     * There was a squeeze going on, and the squeeze log has the
	     * information about the squeeze.  We can set the state of the
	     * squeeze based on the squeeze log.
	     */
	    ss->state = sl->state;
	
	    switch (ss->state) {
	    case FSLIB_SQUEEZE_START_OF_HDR:
		ss->file_index           = sl->s.start_of_hdr.file_index;
		ss->old_hdr_ptr          = sl->s.start_of_hdr.old_hdr_ptr;
		ss->rd_ptr_save          = sl->s.start_of_hdr.old_hdr_ptr;
		ss->wr_ptr               = sl->s.start_of_hdr.wr_ptr;
		break;
	    case FSLIB_SQUEEZE_MIDDLE_OF_HDR:
		ss->file_index           = sl->s.middle_of_hdr.file_index;
		ss->old_hdr_ptr          = sl->s.middle_of_hdr.old_hdr_ptr;
		ss->rd_ptr_save          = sl->s.middle_of_hdr.old_hdr_ptr;
		ss->wr_ptr               = sl->s.middle_of_hdr.wr_ptr;
		ss->bytes_left_this_hdr  = sl->s.middle_of_hdr.bytes_left_this_hdr;
		break;
	    case FSLIB_SQUEEZE_DATA:
		ss->file_index           = sl->s.data.file_index;
		ss->next_old_hdr_ptr     = sl->s.data.next_old_hdr_ptr;
		ss->wr_ptr               = sl->s.data.wr_ptr;
		ss->rd_ptr               = sl->s.data.rd_ptr;
		ss->rd_ptr_save          = sl->s.data.rd_ptr;
		ss->bytes_left_this_file = sl->s.data.bytes_left_this_file;
		break;
	    case FSLIB_SQUEEZE_DONE:
		ss->done = FSLIB_DONE_END_OF_FS;
		break;
	    case FSLIB_SQUEEZE_ERROR:
		ss->done = FSLIB_DONE_ERROR;
		break;
	    }
	}
    }
    
    PRT(P1X,("get_squeeze_state() = %d\n",rc));
    return(rc);
}


/******************************************************************************
 * Name: find_squeeze_log_rec
 *
 * Description: This function checks to see if the squeeze log has information
 *   in it (i.e. a squeeze was in progress on this device).  If it was, it
 *   searches through the squeeze log entries and finds three things:
 *     
 *     1. the last valid squeeze log WRITE record -- this is copied to the
 *        record pointed to by the  "sl"  parameter
 *     2. the last valid squeeze log COPY record -- this is used inside this
 *        function (description later)
 *     3. a pointer to the next available record in the squeeze log -- this is
 *        put in ss->log_ptr
 *
 *   Having found all these things, it determines if the last operation was a
 *   COPY or a WRITE, and if it was a COPY, it copies the sector back from the
 *   squeeze buffer to its place in the flash file system (erasing the
 *   original sector if necessary before copying).
 *
 * Input: 
 *   ss -- a pointer to the squeeze state structure that has everything needed
 *     dev - pointer to the device structure for the device being squeezed
 *   slw -- a pointer to the squeeze log record in which to put the last valid 
 *     WRITE record written to the squeeze log.  If none were written, the flags
 *     field in this structure is set to INVALID.
 *
 * Output:
 *   ss->log_ptr -- the correct value of the next valid space to write in the
 *     log is put in this field.
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error find_squeeze_log_rec (fslib_squeeze_state_rec_t *ss, fslib_squeeze_log_rec_t *slw)
{
    int rc;
    int actual;
    uchar erased_state;
    fslib_ptr end_of_log;
    fslib_squeeze_log_rec_t  squeeze_log_copy;
    fslib_squeeze_log_rec_t *slc;
    fslib_squeeze_log_rec_t  squeeze_log;
    fslib_squeeze_log_rec_t *sl;
    fslib_ptr slw_ptr;
    fslib_ptr slc_ptr;

    PRT(P1E,("find_squeeze_log_rec()\n"));

    slc = &squeeze_log_copy;
    sl = &squeeze_log;
    slc_ptr = 0;
    slw_ptr = 0;

    erased_state = (uchar)(ss->dev->dib.erased_state);

    slw->flags = FSLIB_SQUEEZE_LOG_FLAGS_INVALID;
    /* 
     * Start at the beginning of the log, and read records until
     *  1. an error is found,
     *  2. an unused record is found (end of log)
     *  3. the end of the log is reached (log is full)
     */
    ss->log_ptr = ss->dev->dib.squeeze_log_offset;
    end_of_log = ss->dev->dib.squeeze_log_offset + ss->dev->dib.squeeze_log_length;
    rc = FSLIB_OK;
    while ((rc == FSLIB_OK) && (ss->log_ptr < end_of_log)) {
	/*
	 * read the squeeze log record
	 */
	actual = dev_read(ss->dev,ss->log_ptr,(char *)sl,sizeof(fslib_squeeze_log_rec_t),NULL);
	if (actual != sizeof(fslib_squeeze_log_rec_t)) {
	    if (actual < 0) {
		rc = actual;
	    } else {
		rc = FSLIB_ERROR_READING_SQUEEZE_LOG;
	    }
	    break;
	}
	/*
	 * convert the flags to the positive sense
	 */
	sl->flags ^= erased_state;

	/* 
	 * if the flags are 0 (unused entry) we are done.
	 */
	if (sl->flags == 0) {
	    break;
	}

	/* 
	 * if the flags are IN USE, we have to fix them to be INVALID
	 * (this entry was being written when the squeeze was aborted)
	 */
	if (sl->flags == FSLIB_SQUEEZE_LOG_FLAGS_IN_USE) {
	    sl->flags = FSLIB_SQUEEZE_LOG_FLAGS_INVALID ^ erased_state;
	    actual = dev_write(ss->dev,(char *)sl,ss->log_ptr,1,NULL);
	    if (actual != 1) {
		rc = FSLIB_ERROR_FIXING_SQUEEZE_LOG;
	    } else {
		ss->log_ptr += sizeof(fslib_squeeze_log_rec_t);
	    }
	    continue;
	}

	/* 
	 * if the flags are INVALID, just skip them
	 */
	if (sl->flags == FSLIB_SQUEEZE_LOG_FLAGS_INVALID) {
	    ss->log_ptr += sizeof(fslib_squeeze_log_rec_t);
	    continue;
	}

	/* 
	 * if the flags are VALID, find out what kind of record it is, and
         * copy it to the right place.  WRITE records go to the "slw" since
	 * the caller wants the last valid WRITE record.  COPY records go to 
	 * "slc" for us to use later.
	 */
	if (sl->flags == FSLIB_SQUEEZE_LOG_FLAGS_VALID) {
	    if (sl->type == FSLIB_SQUEEZE_LOG_TYPE_COPY) {
		slc_ptr = ss->log_ptr;
		memcpy(slc,sl,sizeof(fslib_squeeze_log_rec_t));
	    } else if (sl->type == FSLIB_SQUEEZE_LOG_TYPE_WRITE) {
		slw_ptr = ss->log_ptr;
		memcpy(slw,sl,sizeof(fslib_squeeze_log_rec_t));
	    } else {
		rc = FSLIB_ERROR_INVALID_SQUEEZE_LOG_ENTRY;
	    }
	    ss->log_ptr += sizeof(fslib_squeeze_log_rec_t);
	    continue;
	}
    } /* end of while */
    
    /* 
     * now check if the last record written was a COPY record, and 
     * if it was, copy the sector back to where it was originally.
     * The process is
     *   1. erase the original sector
     *   2. copy the data from the squeeze buffer back to the original sector
     */
    if ((rc == FSLIB_OK) && (slc_ptr > slw_ptr)) {
	rc = erase_sector(ss->dev,slc->s.copy.wr_ptr);
	if (rc == FSLIB_OK) {
	    rc = copy_sector_to_sector(ss->dev,
				       ss->dev->dib.squeeze_buffer_offset,
				       slc->s.copy.wr_ptr);
	}
    }

    PRT(P1X,("find_squeeze_log_rec() = %d\n",rc));

    return(rc);
}


/******************************************************************************
 * Name: build_ram_sector
 *
 * Description: build a sector of the squeezed file system in a ram buffer. 
 *
 * Input: 
 *   ss -- a pointer to the squeeze state structure that has everything needed
 *     The actual fields that need to be filled out are dependent on the state
 *     of the squeeze operation (in the ss->state field).
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error build_ram_sector (fslib_squeeze_state_rec_t *ss)
{
    int rc;
    int read_length;
    int seek_length;
    fslib_ptr seek_start;
    char *read_start;
    int actual;
    static fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    
    PRT(P1E,("build_ram_sector(%d,%lx,%lx,%lx)\n",
	    ss->state,ss->old_hdr_ptr,ss->rd_ptr,ss->wr_ptr));

    rc = FSLIB_OK;
    fh = &file_hdr;

    /*
     * we can always assume that we are trying to build a whole sector
     */
    ss->bytes_left_this_sector = ss->dev->dib.sector_size;
    ss->ram_ptr = ss->ram_buffer;
    ss->files_removed = FALSE;

    /*
     * Create a sector in the RAM buffer
     */
    while ((ss->done == FSLIB_NOT_DONE) && (ss->bytes_left_this_sector > 0)) {

	switch (ss->state) {
	case FSLIB_SQUEEZE_START_OF_HDR:
	    ss->bytes_left_this_hdr = sizeof(fslib_file_hdr_t);
	    /* drops through to the next case */
	case FSLIB_SQUEEZE_MIDDLE_OF_HDR:

	    if (ss->state == FSLIB_SQUEEZE_START_OF_HDR) {
		PRT(PCOM,("SQUEEZE_START_OF_HDR\n"));
	    } else {
		PRT(PCOM,("SQUEEZE_MIDDLE_OF_HDR\n"));
	    }
	    
	    /*
	     * read the file header -- if we cannot read the entire file
	     * header, we assume all is OK, we have just run into the end of
	     * the file system.
	     */
	    rc = read_file_hdr(ss->dev,ss->old_hdr_ptr,fh,&ss->rd_ptr);
	    if (rc == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
		rc = FSLIB_OK;
		ss->state = FSLIB_SQUEEZE_DONE;
		ss->done = FSLIB_DONE_END_OF_FS;
	    } else if (rc != FSLIB_OK) {
		ss->state = FSLIB_SQUEEZE_ERROR;
		ss->done = FSLIB_DONE_ERROR;
	    } else {
		/*
		 * if we run into the end of the file system or
		 * we run into a partial file header -- we are done
		 */
		if (fh->hdr_flags == 0) {
		    ss->state = FSLIB_SQUEEZE_DONE;
		    ss->done = FSLIB_DONE_END_OF_FS;
		} else if (   (fh->hdr_flags != FSLIB_HDR_FLAGS_VALID) 
			   && (fh->hdr_flags != FSLIB_HDR_FLAGS_ERROR) ) {
		    ss->state = FSLIB_SQUEEZE_DONE;
		    ss->done = FSLIB_DONE_PARTIAL_FILE_HDR;
		    ss->files_removed = TRUE;
		} else {
		    /*
		     * This file header is valid or an error, but in
		     * either case there may be files after it, and
		     * the next field is OK
		     * if it is an error, or if is it valid but is
		     * deleted, skip to the next file header
		     */
		    if (   (fh->hdr_flags == FSLIB_HDR_FLAGS_ERROR)
			|| ((fh->deleted ^ fh->undeleted) != 0)  ) {
			ss->state = FSLIB_SQUEEZE_START_OF_HDR;
			ss->old_hdr_ptr = fh->next;
			ss->files_removed = TRUE;
		    } else {
			/*
			 * this file is not deleted, so we have to
			 * update the fields in the header and then copy
			 * it to the ram buffer
			 */
			ss->bytes_left_this_file = ROUND_UP_TO_4_BYTES(fh->length);
			ss->next_old_hdr_ptr = fh->next;
			fh->index = ss->file_index;
			fh->deleted = ss->dev->dib.erased_state;
			fh->undeleted = ss->dev->dib.erased_state;
			fh->hdr_flags = FSLIB_HDR_FLAGS_VALID ^ ss->dev->dib.erased_state;
			seek_length = ss->bytes_left_this_hdr + ss->bytes_left_this_file;
			seek_start = ss->wr_ptr + (ss->ram_ptr - ss->ram_buffer);
			dev_seek(ss->dev,seek_start,seek_length,&fh->next);

			/*
			 * now copy the file header to the ram buffer
			 * (as much as is necessary)
			 */
			read_start = (char *)fh 
			    + (sizeof(fslib_file_hdr_t) - ss->bytes_left_this_hdr);
			if (ss->bytes_left_this_sector < ss->bytes_left_this_hdr) {
			    ss->state = FSLIB_SQUEEZE_MIDDLE_OF_HDR;
			    read_length = ss->bytes_left_this_sector;
			} else {
			    ss->state = FSLIB_SQUEEZE_DATA;
			    read_length = ss->bytes_left_this_hdr;
			    ss->file_index++;
			}
			memcpy(ss->ram_ptr,read_start,read_length);
			ss->ram_ptr += read_length;
			ss->bytes_left_this_sector -= read_length;
			ss->bytes_left_this_hdr -= read_length;
		    }
		}
	    }
	    break;
	    
	case FSLIB_SQUEEZE_DATA:

	    PRT(PCOM,("SQUEEZE_DATA\n"));

	    if (ss->bytes_left_this_sector < ss->bytes_left_this_file) {
		ss->state = FSLIB_SQUEEZE_DATA;
		read_length = ss->bytes_left_this_sector;
	    } else {
		ss->state = FSLIB_SQUEEZE_START_OF_HDR;
		read_length = ss->bytes_left_this_file;
		ss->old_hdr_ptr = ss->next_old_hdr_ptr;
	    }
	    
	    actual = dev_read(ss->dev,ss->rd_ptr,ss->ram_ptr,read_length,&ss->rd_ptr);
	    if (actual != read_length) {
		ss->state = FSLIB_SQUEEZE_ERROR;
		ss->done = FSLIB_DONE_ERROR;
		if (actual < 0)
		    rc = actual;
		else
		    rc = FSLIB_ERROR_READING_DATA_DURING_SQUEEZE;
	    } else {
		ss->bytes_left_this_sector -= read_length;
		ss->bytes_left_this_file -= read_length;
		ss->ram_ptr += read_length;
                /* ss->rd_ptr is updated by dev_read() */
	    }		
	    break;
	    
	case FSLIB_SQUEEZE_DONE:
	    PRT(PCOM,("SQUEEZE_DONE\n"));
	    break;
	    
	case FSLIB_SQUEEZE_ERROR:
	default:
	    PRT(PCOM,("SQUEEZE_ERROR\n"));
	    break;
	    
	} /* end of switch */
    } /* end of while */
	    
    PRT(P1X,("build_ram_sector(%d,%lx,%lx,%lx)\n",
	    ss->state,ss->old_hdr_ptr,ss->rd_ptr,ss->wr_ptr));
    
    return((int)rc);
}


/******************************************************************************
 * Name: write_ram_sector_to_flash
 *
 * Description: write a sector of the new (squeezed) file system that is in
 *   the RAM buffer to the sector pointed to by the ss->wr_ptr field.
 *   This function also handles copying the sector that is about to be erased
 *   to the squeeze buffer, erasing the sector, and writing the ram buffer to
 *   the flash sector.  It also updates the squeeze log while doing this.
 *
 * Input: 
 *   ss -- a pointer to the squeeze state structure that has everything needed
 *     dev -- points to the device structure for the device being squeezed.
 *     ram_buffer -- points to the ram buffer containing the new sector.
 *     wr_ptr -- points to the sector to which to write the ram buffer.
 *     rd_ptr_save -- points to the first piece of data used to create the new
 *       flash sector in the ram buffer.
 *   flag -- flag that indicates whether the squeeze log should be used or not.
 *     Not using the squeeze log could speed up the squeeze in some cases, but
 *     it means that if power is lost or the card is removed before the
 *     squeeze completes, the data on the flash will be lost, and the device
 *     will have to be reformatted.
 *     SQUEEZE_NORMAL -- squeeze log *WILL* be used
 *     SQUEEZE_NO_LOG -- squeeze log *WILL NOT* be used
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error write_ram_sector_to_flash (fslib_squeeze_state_rec_t *ss, int flag)
{
    fslib_error rc;
    int write_length;
    int actual;
    char status_string[80];
    unsigned int sector_val = 0;
    status_string[0] = 0;

    PRT(P1E,("write_ram_sector_to_flash(%lx)\n",ss->wr_ptr));

    rc = FSLIB_OK;

    /*
     * if the rd_ptr_save is the same as the write pointer, the new sector
     * started at the beginning of the sector we are writing.  If no files
     * were removed from the file system, then the new sector is the same as
     * the sector we are about to erase, so there is no reason to write this
     * sector to flash -- we can just move the wr_ptr by seeking.
     */
    
    if ((ss->wr_ptr == ss->rd_ptr_save) && (ss->files_removed == FALSE)) {
	dev_seek(ss->dev,ss->wr_ptr,ss->dev->dib.sector_size,&ss->wr_ptr);
    } else {
	/*
	 * If the data we are writing over is some of the data we used to build
	 * this sector, we need to save the buffer to the extra flash sector we 
	 * use at the end of the file system (unless the "no log" flag is used)
	 * 
	 * check the copy sector -- erase if necessary
	 * copy the sector at the write_ptr to the copy sector
	 * write squeeze log -- sector N is in the copy sector
	 */
	if ((flag & SQUEEZE_NO_LOG) == 0) {
	    if (ss->rd_ptr_save < (ss->wr_ptr + ss->dev->dib.sector_size)) {
		rc = erase_sector(ss->dev,ss->dev->dib.squeeze_buffer_offset);
 		print_squeeze_status_update("Erasing squeeze buffer", flag & SQUEEZE_QUIET,
					     &ss->last_status_length,0,0);
		if (rc == FSLIB_OK) {
		    rc = copy_sector_to_sector(ss->dev,ss->wr_ptr,ss->dev->dib.squeeze_buffer_offset);
 		    print_squeeze_status_update("Writing squeeze buffer", flag & SQUEEZE_QUIET,
						 &ss->last_status_length,0,0);
		    if (rc == FSLIB_OK) {
			rc = write_squeeze_log(ss,FSLIB_SQUEEZE_LOG_TYPE_COPY);
		    }
		}
	    }
	}
	
	/* 
	 * erase the sector at the write_ptr
	 * write the ram sector to the sector at the write_ptr
	 * update the write pointer
	 * write status -- state, hdr, rd, wr, len
	 */
	if (rc == FSLIB_OK) {

	    sector_val = get_sector_num(ss->dev,ss->wr_ptr);
	    rc = erase_sector(ss->dev,ss->wr_ptr);
	    
 	    print_squeeze_status_update("Erasing sector", flag & SQUEEZE_QUIET,&ss->last_status_length,1,sector_val);
 	    if (rc == FSLIB_OK) {
		write_length = ss->dev->dib.sector_size - ss->bytes_left_this_sector;
		actual = dev_write(ss->dev,ss->ram_buffer,ss->wr_ptr,write_length,&ss->wr_ptr);
                sector_val = get_sector_num(ss->dev,ss->wr_ptr);
                print_squeeze_status_update("Writing sector", flag & SQUEEZE_QUIET,&ss->last_status_length,1,sector_val);
 		if (actual != write_length) {
		    rc = FSLIB_ERROR_WRITING_SECTOR_DURING_SQUEEZE;
		} else {
		    if ((flag & SQUEEZE_NO_LOG) == 0) {
			rc = write_squeeze_log(ss,FSLIB_SQUEEZE_LOG_TYPE_WRITE);
		    }
		}
	    }
	}
    }
			
	    
    PRT(P1X,("write_ram_sector_to_flash(%lx) = %d\n",ss->wr_ptr,rc));
    
    return(rc);
}


/******************************************************************************
 * Name: write_squeeze_log
 *
 * Description: Write a record to the squeeze log at the log_ptr location
 *
 * Input: 
 *   ss -- a pointer to the squeeze state structure that has everything needed
 *   type -- the type of log record to write (WRITE or COPY)
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error write_squeeze_log (fslib_squeeze_state_rec_t *ss,int type)
{
    fslib_error rc;
    fslib_squeeze_log_rec_t  squeeze_log;
    fslib_squeeze_log_rec_t *sl;
    int actual;
    fslib_ptr end_of_log;
    uchar erased_state;
    
    PRT(P1E,("write_squeeze_log(%lx(%lx),%x)\n",ss->log_ptr,ss->wr_ptr,type));

    /*
     * Check to make sure there is enough room to write another record to the
     * squeeze log -- if not, return an error
     */
    end_of_log = ss->dev->dib.squeeze_log_offset + ss->dev->dib.squeeze_log_length;
    if (ss->log_ptr > (end_of_log - sizeof(fslib_squeeze_log_rec_t))) {
	rc = FSLIB_ERROR_SQUEEZE_LOG_FULL;
    } else {
	/*
	 * there is enough room to write the record -- let's get to it.
	 */
	sl = &squeeze_log;
	erased_state = (char)ss->dev->dib.erased_state;
	
	/*
	 * Phase 1: set up the squeeze write log to have all the erased state,
	 * and then just write the IN USE bit.
	 */
	memset(sl,erased_state,sizeof(fslib_squeeze_log_rec_t));
	sl->flags = FSLIB_SQUEEZE_LOG_FLAGS_IN_USE ^ erased_state;
	actual = dev_write(ss->dev,(char *)sl,ss->log_ptr,1,NULL);
	if (actual != 1) {
	    rc = FSLIB_ERROR_WRITING_SQUEEZE_LOG_1;
	} else {
	    /*
	     * Phase 2a: OK, the IN USE bit is set, now fill in the rest of the entry
	     */
	    if (type == FSLIB_SQUEEZE_LOG_TYPE_COPY) {
		sl->type   = FSLIB_SQUEEZE_LOG_TYPE_COPY;
		sl->s.copy.wr_ptr = ss->wr_ptr;
	    } else {
		sl->type  = FSLIB_SQUEEZE_LOG_TYPE_WRITE;
		sl->state = ss->state;
		switch (ss->state) {
		case FSLIB_SQUEEZE_START_OF_HDR:
		    sl->s.start_of_hdr.file_index           = ss->file_index;
		    sl->s.start_of_hdr.old_hdr_ptr          = ss->old_hdr_ptr;
		    sl->s.start_of_hdr.wr_ptr               = ss->wr_ptr;
		    break;
		case FSLIB_SQUEEZE_MIDDLE_OF_HDR:
		    sl->s.middle_of_hdr.file_index          = ss->file_index;
		    sl->s.middle_of_hdr.old_hdr_ptr         = ss->old_hdr_ptr;
		    sl->s.middle_of_hdr.wr_ptr              = ss->wr_ptr;
		    sl->s.middle_of_hdr.bytes_left_this_hdr = ss->bytes_left_this_hdr;
		    break;
		case FSLIB_SQUEEZE_DATA:
		    sl->s.data.file_index                   = ss->file_index;
		    sl->s.data.next_old_hdr_ptr             = ss->next_old_hdr_ptr;
		    sl->s.data.wr_ptr                       = ss->wr_ptr;
		    sl->s.data.rd_ptr                       = ss->rd_ptr;
		    sl->s.data.bytes_left_this_file         = ss->bytes_left_this_file;
		    break;
		}
	    }
	    /*
	     * Phase 2b: Now the entry is filled in, write it to the log
	     */
	    actual = dev_write(ss->dev,(char *)sl,ss->log_ptr,sizeof(fslib_squeeze_log_rec_t),NULL);
	    
	    if (actual != sizeof(fslib_squeeze_log_rec_t)) {
		rc = FSLIB_ERROR_WRITING_SQUEEZE_LOG_2;
	    } else {
		/*
		 * Phase 3: Now write the valid bit in the flags
		 */
		sl->flags = FSLIB_SQUEEZE_LOG_FLAGS_VALID ^ erased_state;
		actual = dev_write(ss->dev,(char *)sl,ss->log_ptr,1,NULL);
		if (actual != 1) {
		    rc = FSLIB_ERROR_WRITING_SQUEEZE_LOG_3;
		} else {
		    ss->log_ptr += sizeof(fslib_squeeze_log_rec_t);
		    rc = FSLIB_OK;
		}
	    }
	}
    }

    PRT(P1X,("write_squeeze_log(%lx(%lx),%x) = %d\n",ss->log_ptr,ss->wr_ptr,type,rc));
	    
    return(rc);
}


/******************************************************************************
 * Name: copy_sector_to_sector
 *
 * Description: copy one sector to another sector
 *
 * Input: 
 *   dev -- a pointer to the device structure
 *   src -- the start of the sector to use as the source of the copy
 *   dst -- the start of the sector to use as the destination of the copy.
 *     This sector must be either erased, or must contain data that is
 *     compatible with the data in the source sector.  The destination 
 *     sector is *NOT* erased as part of this funciton.
 *
 * Return Value:
 *   fslib_error -- return code: FSLIB_OK if successful, otherwise an error code.
 *
 *******************************************************************************/
STATIC fslib_error copy_sector_to_sector (fslib_device_t *dev, fslib_ptr src, fslib_ptr dst)
{
    fslib_error rc;
    int actual;
    static ulong buffer[256 * 16];
    int bytes_left_this_sector;
    int copy_length;
    
    PRT(P1E,("copy_sector_to_sector(%s,%lx,%lx)\n",dev->dev_id,src,dst));
    
    /* 
     * some basic parameter checking.  Both src and dst must point to the
     * start of sectors.
     */
    if (get_sector_ptr(dev,get_sector_num(dev,src)) != src) {
	rc = FSLIB_ERROR_SECTOR_COPY_SRC;
    } else if (get_sector_ptr(dev,get_sector_num(dev,dst)) != dst) {
	rc = FSLIB_ERROR_SECTOR_COPY_DST;
    } else {
	/*
	 * read from src to the buffer then write from the buffer dst
	 * until the whole sector is copied or there is an error.
	 */
	rc = FSLIB_OK;
	bytes_left_this_sector = dev->dib.sector_size;
	while ((bytes_left_this_sector > 0) && (rc == FSLIB_OK)) {
	    if (bytes_left_this_sector > sizeof(buffer)) {
		copy_length = sizeof(buffer);
	    } else {
		copy_length = bytes_left_this_sector;
	    }
	    actual = dev_read(dev,src,(char *)buffer,copy_length,NULL);
	    if (actual != copy_length) {
		rc = FSLIB_ERROR_SECTOR_COPY_READ;
	    } else {
		actual = dev_write(dev,(char *)buffer,dst,copy_length,NULL);
		if (actual != copy_length) {
		    rc = FSLIB_ERROR_SECTOR_COPY_WRITE;
		} else {
		    src += copy_length;
		    dst += copy_length;
		    bytes_left_this_sector -= copy_length;
		}
	    }
	}
    }

    PRT(P1X,("copy_sector_to_sector(%s,%lx,%lx) = %d (\n",dev->dev_id,src,dst,rc));
	    
    return(rc);
}


/******************************************************************************
 * Name: FSLIB_getdents
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
int FSLIB_getdents (int fd, char *buffer, int buffer_length)
{
    fslib_error       error;
    fslib_file_hdr_t *fh;
    fslib_file_rec_t *fp;
    int               byte_count;
    fslib_ptr         fh_ptr;
    
    PRT(P0E,("getdents(%d,%lx,%d)\n",fd,buffer,buffer_length));

    /*
     *	Make sure card is not being formatted.
     */
    if (device_being_format(fd) == TRUE)
	return FSLIB_ERROR_BEING_FORMAT;

    error = FSLIB_OK;
    byte_count = 0;
    /*
     * get the file record for the open file
     */
    fp = get_file_rec(fd);
    if (fp == NULL) {
	error = FSLIB_ERROR_BAD_FILE_DESCRIPTOR;
    } else if (fp->state == FSLIB_FILE_STATE_ERROR) {
	error = FSLIB_ERROR_NO_DEVICE;
    } else if (fp->type != FSLIB_FILE_TYPE_DIRECTORY) {
	error = FSLIB_ERROR_FILE_NOT_DIRECTORY;
    } else {
	/* 
	 * Loop through the file system getting a file header and converting
	 */
	fh = &fp->hdr;
	while (buffer_length >= sizeof(dirent_t)) {

	    /*
	     *	Make sure the currently physical location
	     *	is within valid range.
	     */
	    if ((char *)fp->curr_phys_ptr > (char *)fp->dev->file_system_end)
		break;
	    
	    /*
	     * Get the file header -- if we cannot read the entire file header
	     * we assume that all is OK, we have just run into the end of the
	     * file system.
	     */
	    error = read_file_hdr(fp->dev,fp->curr_phys_ptr,&fp->hdr,NULL);
	    if (error == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
		error = FSLIB_OK;
		break;
	    }
	    if (error != FSLIB_OK) {
		break;
	    }

	    /*
	     *	Check if we have a valid file header.
	     */
	    if (fp->dev == NULL) {
		break;
	    }
	    if (valid_file_hdr(fp->dev, fh, &error) == FALSE) {
		break;
	    }

	    /*
	     * We also make sure the next ptr is strictly
	     * increasing. This can avoid being trapped
	     * in a infinite loop if flash is corrupted.
	     */
	    fh_ptr = fp->curr_phys_ptr + sizeof(fslib_file_hdr_t);
	    if ((char *)fh->next < (char *)fh_ptr) {
		/*
		 * Bail out.
		 */
		break;
	    }

	    /*
	     * if we reach an unused file header, we are done --> end of file system
	     */
	    if (  (fh->hdr_flags == 0)
		|| (fh->hdr_flags == FSLIB_HDR_FLAGS_IN_USE)
		|| (fh->hdr_flags == FSLIB_HDR_FLAGS_CREATED)  ) {
		error = FSLIB_OK;
		break;
	    }

	    /*
	     * Convert the file to the dirent in the buffer provided
	     */
	    convert_file_hdr_to_dirent(fh,(dirent_t *)buffer);
	    
	    /*
	     * adjust the pointer and length
	     */
	    byte_count += sizeof(dirent_t);
	    buffer += sizeof(dirent_t);
	    buffer_length -= sizeof(dirent_t);
	    fp->curr_phys_ptr = fh->next;
	}
	if (error != FSLIB_OK)
	    byte_count = (int)error;
    }
    
    PRT(P0X,("getdents(%d,%lx,%d) = %d\n",fd,buffer,buffer_length,byte_count));
    return(byte_count);
}


/******************************************************************************
 * Name: FSLIB_settype
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
int FSLIB_settype (const char *file_id, ulong file_type)
{
    fslib_error       error;
    char              temp_file_id[FSLIB_FILE_ID_MAX + 1];
    char             *separator;
    char             *dev_id;
    char             *file_name;
    fslib_device_t   *dev;
    fslib_ptr         fh_ptr;
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    ulong             type;
    
    PRT(P0E,("settype(%s,%d)\n",file_id,file_type));

    error = FSLIB_OK;
    fh = &file_hdr;
    /*
     * Do some basic parameter checking
     */
    if (valid_file_id(file_id,&error)) {
	/*
	 * The device ID ends with a colon, make it a null-terminated
	 * string by replacing the separating colon with a null.  
	 */
	strncpy(temp_file_id,file_id,sizeof(temp_file_id));
	separator = strchr(temp_file_id,':');
	*separator = '\0';
	file_name = separator + 1;
	dev_id = temp_file_id;
	
	/*
	 * Need to check to see if the device is OK.  We already know that
	 * the device ID is OK, but the device may be removed (like PCMCIA
	 * cards) 
	 */
	dev = init_device(dev_id,FALSE,&error);
	if (dev != NULL) {
	    /*
	     * We have found the device and it is OK
	     * Scan through the device to find the file header for the
	     * file indicated.
	     */
	    fh_ptr = find_file_hdr_by_name(dev,file_name,&file_hdr,&error);
	    if (fh_ptr != -1) {
		if (fh_ptr == 0) {
		    error = FSLIB_ERROR_FILE_NOT_FOUND;
		} else {
		    type = fh->type ^ dev->dib.erased_state;
		    if (type != 0) {
			if (type == file_type) {
			    error = FSLIB_ERROR_FILE_TYPE_SET_THE_SAME;
			} else {
			    error = FSLIB_ERROR_FILE_TYPE_ALREADY_SET;
			}
		    } else {
			fh->type = file_type;
			error = write_file_hdr(dev,fh,fh_ptr);
		    }
		}
	    }
	}
    }
    
    /*
     * Do some basic parameter checking
     */
    PRT(P0X,("settype(%s,%d) = %d\n",file_id,file_type,error));
    return((int)error);
}


/******************************************************************************
 * Name: FSLIB_geterrortext
 *
 * Description: returns a pointer to a text string for the give error code
 *
 * Input: 
 *   error -- error code

 * Return Value:
 *   pointer to a text string describing the error
 *
 *******************************************************************************/
void FSLIB_geterrortext (int error, char *buffer, int buffer_len)
{
    switch (error) {
    case FSLIB_OK:
	strncpy(buffer,"OK",buffer_len);
	break;
    case FSLIB_ERROR:
	strncpy(buffer,"unspecified error",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FILE_ID:
	strncpy(buffer,"invalid file ID",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_DEVICE_ID:
	strncpy(buffer,"invalid device ID",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FILE_NAME:
	strncpy(buffer,"invalid file name",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FLAGS:
	strncpy(buffer,"invalid flags",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_WHENCE:
	strncpy(buffer,"invalid whence",buffer_len);
	break;
    case FSLIB_ERROR_UNKNOWN_DEVICE:
	strncpy(buffer,"unknown device",buffer_len);
	break;
    case FSLIB_ERROR_NO_DEVICE:
	strncpy(buffer,"no device",buffer_len);
	break;
    case FSLIB_ERROR_FILE_NOT_FOUND:
	strncpy(buffer,"file not found",buffer_len);
	break;
    case FSLIB_ERROR_FILE_NOT_UNIQUE:
	strncpy(buffer,"file not unique",buffer_len);
	break;
    case FSLIB_ERROR_FILE_NOT_UNDELETABLE:
	strncpy(buffer,"file not undeletable",buffer_len);
	break;
    case FSLIB_ERROR_DEVICE_IS_READ_ONLY:
	strncpy(buffer,"device is read-only",buffer_len);
	break;
    case FSLIB_ERROR_FILE_IS_READ_ONLY:
	strncpy(buffer,"file is read-only",buffer_len);
	break;
    case FSLIB_ERROR_NO_MORE_FILE_RECS:
	strncpy(buffer,"no more file records",buffer_len);
	break;
    case FSLIB_ERROR_BAD_FILE_DESCRIPTOR:
	strncpy(buffer,"bad file descriptor",buffer_len);
	break;
    case FSLIB_ERROR_FILE_NOT_OPEN_FOR_READ:
	strncpy(buffer,"file not open for read",buffer_len);
	break;
    case FSLIB_ERROR_FILE_NOT_OPEN_FOR_WRITE:
	strncpy(buffer,"file not open for write",buffer_len);
	break;
    case FSLIB_ERROR_BUFFER_NOT_BIG_ENOUGH:
	strncpy(buffer,"buffer not big enough",buffer_len);
	break;
    case FSLIB_ERROR_FILE_TYPE_ALREADY_SET:
	strncpy(buffer,"file type already set",buffer_len);
	break;
    case FSLIB_ERROR_FILE_TYPE_SET_THE_SAME:
	strncpy(buffer,"file type set the same",buffer_len);
	break;
    case FSLIB_ERROR_FILE_SYSTEM_FULL:
	strncpy(buffer,"file system full",buffer_len);
	break;
    case FSLIB_ERROR_CANNOT_OPEN_ANOTHER_FILE_FOR_WRITE:
	strncpy(buffer,"cannot open another file for write",buffer_len);
	break;
    case FSLIB_ERROR_FILE_NOT_DIRECTORY:
	strncpy(buffer,"file not a directory",buffer_len);
	break;
    case FSLIB_ERROR_BAD_DIB:
	strncpy(buffer,"bad device info block",buffer_len);
	break;
    case FSLIB_ERROR_SECTOR_NOT_ERASED:
	strncpy(buffer,"sector not erased",buffer_len);
	break;
    case FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR:
	strncpy(buffer,"could not read entire file header",buffer_len);
	break;
    case FSLIB_ERROR_FIXING_CREATED_FILE_HDR:
	strncpy(buffer,"error fixing created file header",buffer_len);
	break;
    case FSLIB_ERROR_NULL_FILE_NAME_PTR:
	strncpy(buffer,"null file name pointer",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_IN_USE_FILE_HDR:
	strncpy(buffer,"invalid in-use file header",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_CREATED_FILE_HDR:
	strncpy(buffer,"invalid created file header",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_CREATED_FILE_HDR_NAME:
	strncpy(buffer,"invalid created file header name",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_CREATED_FILE_HDR_MAGIC:
	strncpy(buffer,"invalid created file header magic number",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_CREATED_FILE_HDR_DELETE:
	strncpy(buffer,"invalid created file header delete field",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FILE_HDR_CRC:
	strncpy(buffer,"invalid file header CRC",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FILE_HDR_NAME:
	strncpy(buffer,"invalid file header name",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FILE_HDR_MAGIC:
	strncpy(buffer,"invalid file header magic number",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_FILE_HDR_NEXT:
	strncpy(buffer,"invalid file header next field",buffer_len);
	break;
    case FSLIB_ERROR_READING_FILE_FOR_CRC:
	strncpy(buffer,"error reading file for CRC",buffer_len);
	break;
    case FSLIB_ERROR_RALIB_FLASH_PTR:
	strncpy(buffer,"invalid Raw Access Library flash pointer",buffer_len);
	break;
    case FSLIB_ERROR_RALIB_DEV_NUM:
	strncpy(buffer,"invalid Raw Access Library device number",buffer_len);
	break;
    case FSLIB_ERROR_RALIB_MODE:
	strncpy(buffer,"invalid Raw Access Library mode",buffer_len);
	break;
    case FSLIB_ERROR_RALIB_UNKNOWN:
	strncpy(buffer,"unknown Raw Access Library error code",buffer_len);
	break;
    case FSLIB_ERROR_READING_FOR_ERASE_CHECK:
	strncpy(buffer,"error reading for erase check",buffer_len);
	break;
    case FSLIB_ERROR_FILE_OPEN_FOR_WRITE:
	strncpy(buffer,"file not open for write",buffer_len);
	break;
    case FSLIB_ERROR_READING_SQUEEZE_LOG:
	strncpy(buffer,"error reading squeeze log",buffer_len);
	break;
    case FSLIB_ERROR_FIXING_SQUEEZE_LOG:
	strncpy(buffer,"error fixing squeeze log",buffer_len);
	break;
    case FSLIB_ERROR_INVALID_SQUEEZE_LOG_ENTRY:
	strncpy(buffer,"invalid squeeze log entry",buffer_len);
	break;
    case FSLIB_ERROR_READING_DATA_DURING_SQUEEZE:
	strncpy(buffer,"error reading data during squeeze",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_SECTOR_DURING_SQUEEZE:
	strncpy(buffer,"error writing sector during squeeze",buffer_len);
	break;
    case FSLIB_ERROR_SQUEEZE_LOG_FULL:
	strncpy(buffer,"squeeze log full",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_SQUEEZE_LOG_1:
	strncpy(buffer,"error writing squeeze log 1",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_SQUEEZE_LOG_2:
	strncpy(buffer,"error writing squeeze log 2",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_SQUEEZE_LOG_3:
	strncpy(buffer,"error writing squeeze log 3",buffer_len);
	break;
    case FSLIB_ERROR_SECTOR_COPY_SRC:
	strncpy(buffer,"invalid sector copy src",buffer_len);
	break;
    case FSLIB_ERROR_SECTOR_COPY_DST:
	strncpy(buffer,"invalid sector copy dst",buffer_len);
	break;
    case FSLIB_ERROR_SECTOR_COPY_READ:
	strncpy(buffer,"error sector copy read",buffer_len);
	break;
    case FSLIB_ERROR_SECTOR_COPY_WRITE:
	strncpy(buffer,"error sector copy write",buffer_len);
	break;
    case FSLIB_ERROR_INIT_DEVICE_NO_DEVS:
	strncpy(buffer,"error init device no devices",buffer_len);
	break;
    case FSLIB_ERROR_READING_DIB:
	strncpy(buffer,"error reading Device Info Block",buffer_len);
	break;
    case FSLIB_ERROR_REMOVING_SPARE_SECTOR:
	strncpy(buffer,"error removing spare sector",buffer_len);
	break;
    case FSLIB_ERROR_SETTING_SPARE_SECTOR:
	strncpy(buffer,"error setting spare sector",buffer_len);
	break;
    case FSLIB_ERROR_READING_BAD_SECTOR_MAP:
	strncpy(buffer,"error reading bad sector map",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_BAD_SECTOR_MAP:
	strncpy(buffer,"error writing bad sector map",buffer_len);
	break;
    case FSLIB_ERROR_COULD_NOT_ALLOC_SQUEEZE_BUFFER:
	strncpy(buffer,"could not allocate squeeze buffer",buffer_len);
	break;
    case FSLIB_ERROR_NO_READ_FUNCTION:
	strncpy(buffer,"no device driver read function",buffer_len);
	break;
    case FSLIB_ERROR_NO_WRITE_FUNCTION:
	strncpy(buffer,"no device driver write function",buffer_len);
	break;
    case FSLIB_ERROR_NO_ERASE_FUNCTION:
	strncpy(buffer,"no device driver erase function",buffer_len);
	break;
    case FSLIB_ERROR_ERASING_SECTOR:
	strncpy(buffer,"error erasing sector",buffer_len);
	break;
    case FSLIB_ERROR_DEVICE_NOT_READY:
	strncpy(buffer,"device not ready",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_DEVICE:
	strncpy(buffer,"device driver error writing device",buffer_len);
	break;
    case FSLIB_ERROR_ERASING_DEVICE:
	strncpy(buffer,"device driver error erasing device",buffer_len);
	break;
    case FSLIB_ERROR_VPP_LOW:
        strncpy(buffer,"programming voltage low",buffer_len);
        break;
    case FSLIB_ERROR_BEING_FORMAT:
	strncpy(buffer,"device being formatted",buffer_len);
	break;
    case FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT:
	strncpy(buffer,"file system is in an inconsistent state",buffer_len);
	break;
    case FSLIB_ERROR_DEVICE_NAME_BUFFER_TOO_SMALL:
	strncpy(buffer,"buffer too small to hold all the device names",buffer_len);
	break;
    case FSLIB_ERROR_NO_MONLIB_ON_DEV:
	strncpy(buffer,"no monlib on device",buffer_len);
	break;
    case FSLIB_ERROR_NO_MEMORY_FOR_MONLIB_BUF:
	strncpy(buffer,"no more memory for monlib buffer",buffer_len);
	break;
    case FSLIB_ERROR_NO_MEMORY_FOR_FLASH_READ:
	strncpy(buffer,"no more memory to read flash",buffer_len);
	break;
    case FSLIB_ERROR_NO_MEMORY_FOR_BAD_SECTOR_MAP:
	strncpy(buffer,"no more memory to read bad sector map",buffer_len);
	break;
    case FSLIB_ERROR_READING_MONLIB:
	strncpy(buffer,"read monlib error",buffer_len);
	break;
    case FSLIB_ERROR_MONLIB_SIZE:
	strncpy(buffer,"monlib has incorrect size",buffer_len);
	break;
    case FSLIB_ERROR_BAD_SECTOR_NOT_ALLOWED:
	strncpy(buffer,"bad sector not allowed in region",buffer_len);
	break;
    case FSLIB_ERROR_COPY_MONLIB_TO_DEV:
	strncpy(buffer,"copy monlib to device error",buffer_len);
	break;
    case FSLIB_ERROR_DEVICE_BUSY:
	strncpy(buffer,"device is busy",buffer_len);
	break;
    case FSLIB_ERROR_NO_ALGORITHM:
	strncpy(buffer,"can not find flash algorithm",buffer_len);
	break;
    case FSLIB_ERROR_WRITING_DIB:
	strncpy(buffer,"write dib error",buffer_len);
	break;
    case FSLIB_ERROR_BAD_SECTOR_DEVICE_SIZE:
	strncpy(buffer,"sector/device size incorrect",buffer_len);
	break;
    default:	
	strncpy(buffer,"UNKNOWN ERROR CODE",buffer_len);
	break;
    }
    buffer[buffer_len - 1] = '\0';
}


/******************************************************************************
 * Name: FSLIB_getdevicenames
 *
 * Description: gets a string that contains all the valid device names
 *   This function just calls ralib_get_device_names() and returns
 *   that value.
 *
 * Input:
 *   buffer -- a pointer to the buffer in which to put the string of
 *     device names.  Upon successful completion, this buffer will
 *     contain the names of the devices supported.  The names do 
 *     NOT contain the colon (:) and are concatenated with separating 
 *     spaces.  The entire string is null terminated.
 *     The device names are in no specific order. 
 *
 *     Example of return value.
 *       "slot0 slot1 bootflash"
 *
 *   buffer_len -- length of the buffer
 *
 * Return Value:
 *   return_code -- FSLIB_OK if successful, otherwise an error coce
 *
 *******************************************************************************/
int FSLIB_getdevicenames(char *buffer, int buffer_len)
{
    int rc;
    const char *names;

    names = ralib_get_device_names();
    if (strlen(names) >= buffer_len) {
	rc = FSLIB_ERROR_DEVICE_NAME_BUFFER_TOO_SMALL;
    } else {
	strcpy(buffer, (char *)names);
	rc = FSLIB_OK;
    }
    return(rc);
}

/*
 * Hardcoded device names.
 */
#define	BFLASH_DEV	"bootflash"

static int print_chip_info (const char *dev_id)
{
#if !defined(FSLIB)
    int devnum;
    format_dev_parm_t fp_rec;
    format_dev_parm_t *fpp;
    fslib_prog_rec_t *ptblp;
    fslib_show_info_fn_t fshowp;
    int retcode;

    devnum = ralib_get_dev_num(dev_id);
    if (devnum < 0)
	return -1;

    /*
     * Get flash prog algorithm number by analyzing
     * the flash devices.
     */
    fpp = &fp_rec;
    memset(fpp, 0, sizeof(format_dev_parm_t));
    fpp->devnum = devnum;
    /*
     * Check if we understand the type of card/flash.
     * And update format param structures.
     */
    retcode = analyze_flash_device(fpp);
    if (retcode < 0)
	return -1;

    /*
     * Find the right prog table entry by algorithm number.
     */
    ptblp = find_prog_table_entry(fpp->alg_num);
    if (ptblp == NULL)
	return -1;

    /*
     * Make sure we have read/erase functions available.
     */
    fshowp = ptblp->show_info_fn;
    if (fshowp == NULL)
	return -1;

    if (*fshowp) {
	(*fshowp)(devnum);
	printf("\n");
    }
#endif
    return FSLIB_OK;
}


int FSLIB_ioctl (const char *file_id, int func, void *arg)
{
    int retcode = FSLIB_OK;
    uint dev_type = IOCTL_DEV_TYPE_LOCAL;

    switch (func) {
	case IOCTL_FORMAT_FLASH:
	    retcode = format_device(file_id, (format_parm_t *)arg);
	    break;

	case IOCTL_GET_DEV_TYPE:
	    dev_type |= IOCTL_DEV_TYPE_FLASH;
	    if (strncmp(file_id, BFLASH_DEV, strlen(BFLASH_DEV)) != 0)
		dev_type |= IOCTL_DEV_TYPE_REMOVABLE;
	    *(uint *)arg = dev_type;
	    break;

	case IOCTL_PRINT_CHIP:
	    print_chip_info(file_id);
	    break;

	case IOCTL_SET_VERBOSE:
	    fslib_print_enable = (int)arg;
	    break;

        case IOCTL_GET_BUSY_INFO:
	    retcode = get_busy_info(file_id, (int *)arg);
	    break;
	    
	default:
	    retcode = FSLIB_ERROR;
    }
    return retcode;
}


int FSLIB_fioctl (int fd, int func, void *arg)
{
    return FSLIB_ERROR;
}


/******************************************************************************
 * Name: init_device()
 *
 * Description: initializes the file system data structures for a device.
 *   This function only needs to be called once for any given device.  This
 *   means that, for fixed flash devices (like internal flash), this function
 *   only gets called once each time the router is initialized.  For removable
 *   devices (like PCMCIA cards) this function must be called each time a
 *   device is plugged in the PCMCIA slot before any of the files on that
 *   device can be accessed.
 *
 * Input: 
 *   dev_id -- a pointer to the device ID.  It must contain the device
 *     ID as a null-terminated string.
 *   force -- if set to TRUE, forces reinitialization even if the device
 *     state returned from ralib_device_init is STILL_OK. 
 *   error -- a pointer to the error variable to set if there is an error
 *
 * Return Value:
 *   dev -- a pointer to the device structure for this device.  If an error
 *     occurred during the init process, a NULL is returened.
 *
 *******************************************************************************/
fslib_device_t *init_device (const char *dev_id, boolean force, fslib_error *error)
{
    int i;
    int state;
    int actual;
    int dev_num;
    fslib_device_t *dev;
    
    PRT(P1E,("init_device(%s)\n",dev_id));

    dev = get_dev(dev_id);
    dev_num = FSLIB_ralib->get_dev_num(dev_id);
    if (dev_num < 0) {
	*error = convert_ralib_error_code(dev_num);
    } else {
	state = FSLIB_ralib->device_init(dev_num);
	if ((force) && (state == RALIB_DEVICE_STILL_OK)) {
	    state = RALIB_DEVICE_FIRST_OK;
	}
	switch(state) {
	case RALIB_ERROR_NO_DEVICE:
	    remove_dev(dev);
	    *error = FSLIB_ERROR_NO_DEVICE;
	    break;
	case RALIB_DEVICE_STILL_OK:
	    /*
	     * if the device is marked valid, just get out of here
	     */
	    if ((dev != NULL) && (dev->valid)) {
		/*
		 * Check if the device is write protected or not,
		 * and record the result.
		 */
		actual = FSLIB_ralib->control(dev_num, RALIB_WRITE_PROTECT);
		if (actual)
		    dev->writable = FALSE;
		else
		    dev->writable = TRUE;
		break;
	    }
	    /*
	     * if device is not valid, fall through and reinitialize
	     * the device
             */
	case RALIB_DEVICE_FIRST_OK:
	    /*
	     * the device has either never been initialized, or has changed
	     *  state since the last time it was initialized (if it has just
	     *  been inserted in a PCMCIA slot, for example).  Now we must 
	     *  initialize the device
	     */
	    /*
	     * Put the open files for this device in the error state.
	     * Anything that was open before on this device is now invalid
	     * since this is the "first OK" for this device.
	     * Get rid of any old device structure that was being used for this
	     * device and get a new device structure to initialize
	     */
	    remove_dev(dev);
	    dev = get_new_dev();
	    if (dev == NULL) {
		*error = FSLIB_ERROR_INIT_DEVICE_NO_DEVS;
		PRT(PERR,("init_device(%s): no free device structures\n",dev_id));
	    } else {
		/*
		 * reset the device (all devices come up in read mode)
		 * copy the DIB from the device to the device data structure.
		 * check the DIB to make sure it makes sense.
		 * check the DIB to make sure we can support this file system.
		 * fill out the rest of the fields in dev
		 */
		FSLIB_ralib->control(dev_num,RALIB_RESET_DEVICE);

		/*
		 * Check if the device is write protected or not,
		 * and record the result.
		 */
		actual = FSLIB_ralib->control(dev_num, RALIB_WRITE_PROTECT);
		if (actual)
		    dev->writable = FALSE;
		else
		    dev->writable = TRUE;

		actual = FSLIB_ralib->read(dev_num,0,&dev->dib,sizeof(fslib_device_info_block_t));
		if (actual != sizeof(fslib_device_info_block_t)) {
		    if (actual < 0)
			*error = convert_ralib_error_code(actual);
		    else
			*error = FSLIB_ERROR_READING_DIB;
		    PRT(PERR,("init_device(%s): error reading DIB (%d != %d)\n",
			    dev_id,actual,sizeof(fslib_device_info_block_t)));
		    dev = NULL;
		} else {
		    if (!valid_dib(&dev->dib,error)) {
			PRT(PERR,("init_device(%s): DIB error\n",dev_id));
			dev = NULL;
		    } else {
			/* 
			 * everything looks OK, so lets fill in the device structure
			 */
			strcpy(dev->dev_id,dev_id);
			dev->dev_num = dev_num;
			dev->file_open_for_write = FALSE;
			dev->file_system_end = dev->dib.file_system_offset
			    + dev->dib.file_system_length - 1;
			dev->monlib_end = dev->dib.monlib_offset
			    + dev->dib.monlib_length - 1;
			dev->bad_sector_map_end = dev->dib.bad_sector_map_offset
			    + dev->dib.bad_sector_map_length - 1;
			dev->squeeze_log_end = dev->dib.squeeze_log_offset
			    + dev->dib.squeeze_log_length - 1;
			dev->squeeze_buffer_end = dev->dib.squeeze_buffer_offset
			    + dev->dib.squeeze_buffer_length - 1;
			dev->last_sector = get_sector_num(dev,dev->file_system_end);
			dev->next_new_file_hdr_ptr = 0;
			/*
			 * check if the programming algorithm is supported
			 */
			dev->read_fn = NULL;
			dev->write_fn = NULL;
			dev->erase_fn = NULL;
			dev->writable = FALSE;
			for (i = 0; FSLIB_prog_table[i].algorithm != FSLIB_PROG_LIST_END; i++) {
			    if (FSLIB_prog_table[i].algorithm == dev->dib.prog_algorithm) {
				dev->read_fn = FSLIB_prog_table[i].read_fn;
				dev->write_fn = FSLIB_prog_table[i].write_fn;
				dev->erase_fn = FSLIB_prog_table[i].erase_fn;
				dev->writable = TRUE;
				break;
			    }
			}
			dev->valid = TRUE;
		    }
		}
	    }
	    break;
	}    
    }
    
    PRT(P1X,("init_device(%s) = %lx\n",dev_id,dev));
    return(dev);
}


/******************************************************************************
 * Name: get_dev
 *
 * Description: finds a device structure based on the device ID
 *
 * Input:
 *   dev_id -- a pointer to the device ID of the device to find (null terminated)
 *
 * Return Value:
 *   dev -- pointer to the device ID for the device
 *
 *******************************************************************************/
fslib_device_t *get_dev (const char *dev_id)
{
    fslib_device_t *dev;
    int i;
    
    PRT(P3E,("get_dev(%s)\n",dev_id));

    dev = NULL;
    
    /*
     * check all the device entries for a matching device ID
     */
    for (i = 0; i < FSLIB_MAX_DEVICE; i++) {
	if (FSLIB_device_table[i].valid == TRUE) {
	    if (strcmp(dev_id,FSLIB_device_table[i].dev_id) == 0) {
		dev = &FSLIB_device_table[i];
		break;
	    }
	}
    }
    
    PRT(P3X,("get_dev(%s) = %lx\n",dev_id,dev));
    
    return(dev);
}

/******************************************************************************
 * Name: get_new_dev
 *
 * Description: finds an unused device structure
 *
 * Return Value:
 *   dev -- pointer to an unused fslib_device_t structure, or NULL if none available
 *
 *******************************************************************************/
STATIC fslib_device_t *get_new_dev (void)
{
    fslib_device_t *dev;
    int i;
    
    PRT(P3E,("get_new_dev()\n"));

    dev = NULL;
    
    /*
     * check all the device entries for a matching device ID
     */
    for (i = 0; i < FSLIB_MAX_DEVICE; i++) {
	if (FSLIB_device_table[i].valid == FALSE) {
	    dev = &FSLIB_device_table[i];
	    break;
	}
    }
    
    PRT(P3X,("get_new_dev() = %lx\n",dev));
    
    return(dev);
}


/******************************************************************************
 * Name: remove_dev
 *
 * Description: finds a structure that matches the name and makes it invalid
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device that was removed
 *
 * Return Value: none
 *
 *******************************************************************************/
STATIC void remove_dev (fslib_device_t *dev)
{
    int i;
    fslib_file_rec_t *fp;
    
    PRT(P3E,("remove_dev(%lx)\n",dev));

    if (dev != NULL) {
	/*
	 * Go through all the file records and put any open files on this
         * device in the error state.
	 */
	for (i = 0; i < FSLIB_MAX_FILE_REC; i++) {
	    fp = &FSLIB_file_rec[i];
	    if (fp->dev == dev) {
		fp->state = FSLIB_FILE_STATE_ERROR;
	    }
	}
	/*
	 * mark the device record invalid
	 */
	dev->valid = FALSE;
    }

    PRT(P3X,("remove_dev(%lx)\n",dev));
}


/******************************************************************************
 * Name: find_next_new_file_hdr_ptr()
 *
 * Description: searches through a file system for the end of the file system.
 *   This returns a pointer to the next place in file system memory that a
 *   file header can go.  If a file is already open for writing, this function
 *   returns 0.
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   error -- a pointer to the error code variable.  If the return value
 *     is -1, an error code is written to this variable.
 *
 * Output:
 *   dev->next_new_file_hdr_ptr -- gets the address in file system memory of
 *     the place the next file header goes.
 *   dev->next_index -- gets the index of the next file
 *
 * Return Value:
 *   fh_ptr -- a pointer to the place in file system memory for the new file
 *     header.  If there is not enough room, a 0 is returned.  If there was an
 *     error while searching for the file, a -1 is returned and the error code
 *     is written to the error variable.
 *
 *******************************************************************************/
STATIC fslib_ptr find_next_new_file_hdr_ptr (fslib_device_t *dev,fslib_error *error)
{
    fslib_file_hdr_t  file_hdr;
    fslib_file_hdr_t *fh;
    fslib_ptr         ptr;
    fslib_ptr         old_ptr;
    fslib_ptr         fh_ptr;
    ulong             index;
    fslib_error       rc;
    
    PRT(P3E,("find_next_new_file_hdr_ptr(%lx)\n",dev));

    fh = &file_hdr;
    ptr = dev->dib.file_system_offset;
    old_ptr = 0;
    fh_ptr = 0;
    index = 0;
    while (ptr <= dev->file_system_end) {
	/* 
	 * Keep track of the previous value of the pointer and
	 * use it to make sure we don't have file system
	 * problems like loops that would cause problems.
	 * The new file pointer must always be greater than
	 * the old one.
	 */
	if (ptr <= old_ptr) {
	    *error = FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT;
	    fh_ptr = -1;
	    break;
	}
	old_ptr = ptr;

	/* 
	 * read in the file header -- if we cannot read the entire file header
         * we assume that all is OK, we have just run into the end of the file
         * system. 
	 */
	rc = read_file_hdr(dev,ptr,fh,NULL);
	if (rc == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
	    rc = FSLIB_OK;
	    break;
	}
	if (rc != FSLIB_OK) {
	    *error = rc;
	    fh_ptr = -1;
	    break;
	}
	
	/*
	 * if we reach an unused file header, we are done --> end of file system
	 */
	if (fh->hdr_flags == 0) {
	    fh_ptr = ptr;
	    break;
	}
	
	/*
	 * if the file header is valid, count it and skip over it and continue
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID) {
	    index = fh->index;
	    ptr = fh->next;
	    continue;
	}

	/*
	 * if the file header has an "error", skip over it and continue
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_ERROR) {
	    if (fh->index == dev->dib.erased_state) {
		index++;
	    } else {
		index = fh->index;
	    }
	    ptr = fh->next;
	    continue;
	}

	/*
	 * if the file header is "in use" only, fix it up, and continue.
	 * We know that there is never an open file on this device when we
	 * call this function, so we can just go ahead and fix the file system.
	 * if the file system has an error during the fix up, fh->next will
	 * be 0, error will be set and the search is over.
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_IN_USE) {
	    fix_in_use_file_hdr(dev,fh,ptr,error);
	    index++;
	    if (fh->next == 0) {
		fh_ptr = -1;
		break;
	    } else {
		ptr = fh->next;
		continue;
	    }
	}
	
	/*
	 * if the file header has been "created", but is not marked valid, there
	 * was a failure while this file was being written.  Fix up the file
	 * system, and continue.  
	 * We know that there is never an open file on this device when we
	 * call this function, so we can just go ahead and fix the file system.
         * If the file system has an error during the 
	 * fix up, fh->next will be 0, error will be set  and the search is over.
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_CREATED) {
	    fix_created_file_hdr(dev,fh,ptr,error);
	    index = fh->index;
	    if (fh->next == 0) {
		fh_ptr = -1;
		break;
	    } else {
		ptr = fh->next;
		continue;
	    }
	}
    }
    
    /*
     * Write the output information to the device structure.
     */
    if (fh_ptr <= dev->file_system_end)
	dev->next_new_file_hdr_ptr = fh_ptr;
    else 
	dev->next_new_file_hdr_ptr = 0;
    dev->next_index = index + 1;
    
    PRT(P3X,("find_next_new_file_hdr_ptr(%lx) = %lx (%d)\n",dev->dev_id,fh_ptr,*error));
    return(fh_ptr);
}


/******************************************************************************
 * Name: find_file_hdr_by_name()
 *
 * Description: searches through a file system for a file header by file name
 *   This only finds files that are valid, and have *NOT* been deleted.
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   file_name -- a pointer to the null-terminated file name to search for
 *   fh -- a pointer to the buffer inwhich to write the file header
 *   error -- a pointer to the error code variable.  If the return value
 *     is -1, an error code is written to this variable.
 *
 * Return Value:
 *   fh_ptr -- a pointer to the file header for the file in file system
 *     memory.  If the file was not found, a 0 is returned.  If there was an
 *     error while searching for the file, a -1 is returned and the error code
 *     is written to the error variable.
 *
 *******************************************************************************/
STATIC fslib_ptr find_file_hdr_by_name (fslib_device_t *dev,const char *file_name,fslib_file_hdr_t *fh,fslib_error *error)
{
    fslib_ptr       ptr;
    fslib_ptr       old_ptr;
    fslib_ptr       fh_ptr;
    fslib_error     rc;
        
    PRT(P3E,("find_file_hdr_by_name(%lx,%s,%lx)\n",dev,file_name,fh));

    ptr = dev->dib.file_system_offset;
    old_ptr = 0;
    fh_ptr = 0;
    while (ptr <= dev->file_system_end) {
	/* 
	 * Keep track of the previous value of the pointer and
	 * use it to make sure we don't have file system
	 * problems like loops that would cause problems.
	 * The new file pointer must always be greater than
	 * the old one.
	 */
	if (ptr <= old_ptr) {
	    *error = FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT;
	    fh_ptr = -1;
	    break;
	}
	old_ptr = ptr;

	/* 
	 * read in the file header -- if we cannot read the entire file header
         * we assume that all is OK, we have just run into the end of the file
         * system. 
	 */
	rc = read_file_hdr(dev,ptr,fh,NULL);
	if (rc == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
	    rc = FSLIB_OK;
	    break;
	}
	if (rc != FSLIB_OK) {
	    *error = rc;
	    fh_ptr = -1;
	    break;
	}
	
	/*
	 * if we reach an unused file header, we are done --> end of file system
	 */
	if (fh->hdr_flags == 0) {
	    break;
	}
	
	/*
	 * if the file header has an "error", skip over it and continue
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_ERROR) {
	    ptr = fh->next;
	    continue;
	}
	
	/*
	 * if the file header is "in use" only, and the device structure says
	 * that we have a file open for writing, then we have come to the end
	 * of the file system and have not found the file -- break out of this
	 * loop and we will return.  If the device structure does not say we
	 * have a file open for writing, then this file header is an error
	 * that needs fixing -- fix the file header and continue.
	 * if the file system has an error during the fix up, fh->next will
	 * be 0, error will be set and the search is over.
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_IN_USE) {
	    if (dev->file_open_for_write == TRUE) {
		break;
	    } else {
		fix_in_use_file_hdr(dev,fh,ptr,error);
		if (fh->next == 0) {
		    fh_ptr = -1;
		    break;
		} else {
		    ptr = fh->next;
		    continue;
		}
	    }
	}

	/* 
	 * if the file header has been "created", but is not marked valid, and
	 * the device structure says that we have a file open for writing,
	 * then we have come to the end of the file system and have not found
	 * the file -- break out of this loop and we will return.  If the
	 * device structure does not say we have a file open for writing, then
	 * this file header is an error that needs fixing -- fix up the file
	 * system, and continue.  If the file system has an error during the
	 * fix up, fh->next will be 0, error will be set and the search is
	 * over.
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_CREATED) {
	    if (dev->file_open_for_write == TRUE) {
		break;
	    } else {
		fix_created_file_hdr(dev,fh,ptr,error);
		if (fh->next == 0) {
		    fh_ptr = -1;
		    break;
		} else {
		    ptr = fh->next;
		    continue;
		}
	    }
	}
	
	/*
	 * if the file header is valid we need to check the name to see if
	 * it matches
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID) {
	    /*
	     * if the file name matches, and the file is not marked
	     * deleted, we are done --> found the file!
	     * otherwise skip over the file and keep checking
	     */
	    if (  (strcmp(file_name,fh->file_name) == 0)
		&& ((fh->deleted ^ fh->undeleted) == 0)  ) {
		fh_ptr = ptr;
		break;
	    } else {
		ptr = fh->next;
		continue;
	    }
	}
    }
    
    PRT(P3X,("find_file_hdr_by_name(%lx,%s,%lx) = %lx (%d)\n",dev,file_name,fh,fh_ptr,*error));
    return(fh_ptr);
}


/******************************************************************************
 * Name: find_file_hdr_by_index()
 *
 * Description: searches through a file system for a file header by file index
 *   This will find file headers that are either
 *     - valid
 *     - created but are not marked valid
 *     - marked with some sort of error
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   index -- the index of the file to find
 *   fh -- a pointer to the buffer inwhich to write the file header
 *   error -- a pointer to the error code variable.  If the return value
 *     is -1, an error code is written to this variable.
 *
 * Return Value:
 *   fh_ptr -- a pointer to the file header for the file in file system
 *     memory.  If the file was not found, a 0 is returned.  If there was an
 *     error while searching for the file, a -1 is returned and the error code
 *     is written to the error variable.
 *
 *******************************************************************************/
STATIC fslib_ptr find_file_hdr_by_index (fslib_device_t *dev,int index,fslib_file_hdr_t *fh,fslib_error *error)
{
    fslib_ptr       ptr;
    fslib_ptr       old_ptr;
    fslib_ptr       fh_ptr;
    fslib_error     rc;
    
    PRT(P3E,("find_file_hdr_by_index(%lx,%d,%lx)\n",dev,index,fh));

    ptr = dev->dib.file_system_offset;
    old_ptr = 0;
    fh_ptr = 0;
    while (ptr <= dev->file_system_end) {
	/* 
	 * Keep track of the previous value of the pointer and
	 * use it to make sure we don't have file system
	 * problems like loops that would cause problems.
	 * The new file pointer must always be greater than
	 * the old one.
	 */
	if (ptr <= old_ptr) {
	    *error = FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT;
	    fh_ptr = -1;
	    break;
	}
	old_ptr = ptr;

	/* 
	 * read in the file header -- if we cannot read the entire file header
         * we assume that all is OK, we have just run into the end of the file
         * system. 
	 */
	rc = read_file_hdr(dev,ptr,fh,NULL);
	if (rc == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
	    rc = FSLIB_OK;
	    break;
	}
	if (rc != FSLIB_OK) {
	    *error = rc;
	    fh_ptr = -1;
	    break;
	}
	
	/*
	 * if we reach an unused file header, we are done --> end of file system
	 */
	if (fh->hdr_flags == 0) {
	    break;
	}
	
	/*
	 * if the file header is "in use" only, and the device structure says
	 * that we have a file open for writing, then we have come to the end
	 * of the file system and have not found the file -- break out of this
	 * loop and we will return.  If the device structure does not say we
	 * have a file open for writing, then this file header is an error
	 * that needs fixing -- fix the file header and continue.
	 * if the file system has an error during the fix up, fh->next will
	 * be 0, error will be set and the search is over.
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_IN_USE) {
	    if (dev->file_open_for_write == TRUE) {
		break;
	    } else {
		fix_in_use_file_hdr(dev,fh,ptr,error);
		if (fh->next == 0) {
		    fh_ptr = -1;
		    break;
		} else {
		    ptr = fh->next;
		    continue;
		}
	    }
	}
	
	/* 
	 * if the file header has been "created", but is not marked valid, and
	 * the device structure says that we have a file open for writing,
	 * then we have come to the end of the file system and have not found
	 * the file -- break out of this loop and we will return.  If the
	 * device structure does not say we have a file open for writing, then
	 * this file header is an error that needs fixing -- fix up the file
	 * system, and continue.  If the file system has an error during the
	 * fix up, fh->next will be 0, error will be set and the search is
	 * over.  There is still the possibility that this is the file index
	 * we are looking for (index is filled out when the file header is
	 * "created") 
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_CREATED) {
	    if (dev->file_open_for_write == TRUE) {
		break;
	    } else {
		fix_created_file_hdr(dev,fh,ptr,error);
		if (fh->next == 0) {
		    fh_ptr = -1;
		    break;
		} else {
		    if (fh->index == index) {
			fh_ptr = ptr;
			break;
		    } else {
			ptr = fh->next;
			continue;
		    }
		}
	    }
	}
	
	/*
	 * if the file header is valid we need to check the name to see if
	 * it matches
	 */
	if (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID) {
	    /*
	     * if the file name matches, we are done --> found the file!
	     * otherwise skip over the file and keep checking
	     */
	    if (fh->index == index) {
		fh_ptr = ptr;
		break;
	    } else {
		ptr = fh->next;
		continue;
	    }
	}
    }
    
    PRT(P3X,("find_file_hdr_by_index(%lx,%d,%lx) = %lx (%d)\n",dev,index,fh,fh_ptr,*error));
    return(fh_ptr);
}


/******************************************************************************
 * Name: find_file_hdr_by_phys_ptr()
 *
 * Description: searches through a file system for a file header by physical
 *   pointer into the file system.  This is used when seeking through the 
 *   directory file.  lseek() will not allow the directory curr_phys_ptr to
 *   be set to a value that does not point to a file header.
 *   This will find file headers that are either
 *     - valid
 *     - created but are not marked valid
 *     - marked with some sort of error
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   fh_ptr -- the physical pointer to the file header to find
 *   fh -- a pointer to the buffer inwhich to write the file header
 *
 * Return Value:
 *   return code -- FSLIB_OK if successful, otherwise an error code
 *
 *******************************************************************************/
STATIC fslib_error find_file_hdr_by_phys_ptr (fslib_device_t *dev,fslib_ptr fh_ptr,fslib_file_hdr_t *fh)
{
    fslib_ptr       ptr;
    fslib_ptr       old_ptr;
    fslib_error     rc;
    fslib_error     rc1;
    
    PRT(P3E,("find_file_hdr_by_phys_ptr(%lx,%lx,%lx)\n",dev,fh_ptr,fh));

    rc = FSLIB_ERROR_FILE_NOT_FOUND;
    ptr = dev->dib.file_system_offset;
    old_ptr = 0;
    while ((ptr <= dev->file_system_end) && (ptr <= fh_ptr)) {
	/* 
	 * Keep track of the previous value of the pointer and
	 * use it to make sure we don't have file system
	 * problems like loops that would cause problems.
	 * The new file pointer must always be greater than
	 * the old one.
	 */
	if (ptr <= old_ptr) {
	    rc = FSLIB_ERROR_FILE_SYSTEM_INCONSISTANT;
	    break;
	}
	old_ptr = ptr;

	/* 
	 * read in the file header -- if we cannot read the entire file header
         * we assume that all is OK, we have just run into the end of the file
         * system. 
	 */
	rc1 = read_file_hdr(dev,ptr,fh,NULL);
	if (rc1 == FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR) {
	    rc1 = FSLIB_OK;
	    break;
	}
	if (rc1 != FSLIB_OK) {
	    rc = rc1;
	    break;
	}
	
	/*
	 * if we reach an unused file header, we are done --> end of file system
	 */
	if (fh->hdr_flags == 0) {
	    rc = FSLIB_ERROR_FILE_NOT_FOUND;
	    break;
	}
	
	/*
	 * if this file header has the phys ptr we are looking for, we are done
	 */
	if (ptr == fh_ptr) {
	    rc = FSLIB_OK;
	    break;
	}

	/*
	 * if we didn't find the file header, move on to the next one
	 */
	ptr = fh->next;

    }
    
    PRT(P3X,("find_file_hdr_by_phys_ptr(%lx,%lx,%lx) = %d\n",dev,fh_ptr,fh,rc));
    return(rc);
}


/******************************************************************************
 * Name: read_file_hdr
 *
 * Description: reads a file header and does some validity checking
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fh_ptr -- a pointer to the file header in file system memory
 *   fh -- a pointer to a buffer into which to read the file header
 *   next_ptr -- a pointer to the variable inwhich to put the next physical
 *     address in file system memory after the file header that was read.
 *     May be a NULL pointer if the caller does not want this info.
 *
 * Return Value: 
 *   return code -- FSLIB_OK = file header read and OK, otherwise and error code
 *
 *******************************************************************************/
STATIC fslib_error read_file_hdr (fslib_device_t *dev, fslib_ptr fh_ptr, fslib_file_hdr_t *fh, fslib_ptr *next_ptr)
{
    int actual;
    fslib_error error;
    fslib_error rc;
    int all_erased;
    int i;
    
    PRT(P5E,("read_file_hdr(%lx,%lx,%lx,%lx)\n",dev,fh_ptr,fh,next_ptr));

    /*
     * read the file header from the file system
     */
    actual = dev_read(dev,fh_ptr,(char *)fh,sizeof(fslib_file_hdr_t),next_ptr);
    
    /*
     * if there was an error or the whole file header was not read, 
     * we are done --> error
     */
    if (actual < 0) {
	rc = actual;
    } else if (actual != sizeof(fslib_file_hdr_t)) {
	rc = FSLIB_ERROR_COULD_NOT_READ_ENTIRE_FILE_HDR;
    } else {
	
	/*     
	 * If the header consists entirely of "erased" bytes, the file header
         * should not (most not) be validity checked.  It is basically OK for
	 * This function to return successfully when it has read an
	 * uninitialized file header.
	 */
	all_erased = TRUE;
	for (i = 0; i < sizeof(fslib_file_hdr_t); i += sizeof(dev->dib.erased_state)) {
	    if (*(ulong *)((uchar *)fh + i) != dev->dib.erased_state) {
		all_erased = FALSE;
	    }
	}

	/*     
	 * Convert the header fields that depend on the device erased state
	 */
	fh->file_flags ^= dev->dib.erased_state;
        fh->hdr_flags  ^= dev->dib.erased_state;
	fh->deleted    ^= (ushort)(dev->dib.erased_state);
	fh->undeleted  ^= (ushort)(dev->dib.erased_state);
	
	/*     
	 * if the header is not just an bunch of erased bytes, 
	 * check if the file header is valid
	 * This check is not whether the file header has the "valid"
	 * bit set, but whether the fields in the header make sense.
	 * If the file header is uninitialized memory, it still appears
	 * valid.
	 */
	if (all_erased) {
	    rc = FSLIB_OK;
	} else {
	    error = FSLIB_OK;
	    if (!valid_file_hdr(dev,fh,&error)) {
		rc = error;
	    } else {
		rc = FSLIB_OK;
	    }
	}
    }
    PRT(P5X,("read_file_hdr(%lx,%lx,%lx,%lx) = %d\n",dev,fh_ptr,fh,next_ptr,rc));
    return(rc);
}


/******************************************************************************
 * Name: write_file_hdr
 *
 * Description: writes a file header
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fh_ptr -- a pointer to the file header in file system memory
 *   fh -- a pointer to a buffer into which to read the file header
 *
 * Return Value: 
 *   return code -- FSLIB_OK = file header read and OK, otherwise and error code
 *
 *******************************************************************************/
STATIC fslib_error write_file_hdr (fslib_device_t *dev, fslib_file_hdr_t *fh, fslib_ptr fh_ptr)
{
    int actual;
    fslib_error rc;
    
    PRT(P6E,("write_file_hdr(%lx,%lx,%lx)\n",dev,fh,fh_ptr));
    /*
     * convert the file header fields that depend on the erased state of flash
     * (we will convert these back after)
     */
    fh->file_flags ^= dev->dib.erased_state;
    fh->hdr_flags  ^= dev->dib.erased_state;
    fh->deleted    ^= (ushort)(dev->dib.erased_state);
    fh->undeleted  ^= (ushort)(dev->dib.erased_state);

    /* 
     * write the header to the device
     */
    actual = dev_write(dev,(char *)fh,fh_ptr,sizeof(fslib_file_hdr_t),NULL);
    
    /*
     * if there was an error or the whole file header was not written,
     * we are done --> error
     */
    if (actual == sizeof(fslib_file_hdr_t)) {
	rc = FSLIB_OK;
    } else if (actual < 0) {
	rc = actual;
    } else {
	rc = FSLIB_ERROR_FILE_SYSTEM_FULL;
    }

    /*
     * convert the file header fields back to their original state
     */
    fh->file_flags ^= dev->dib.erased_state;
    fh->hdr_flags  ^= dev->dib.erased_state;
    fh->deleted    ^= (ushort)(dev->dib.erased_state);
    fh->undeleted  ^= (ushort)(dev->dib.erased_state);

    PRT(P6X,("write_file_hdr(%lx,%lx,%lx) = %d\n",dev,fh,fh_ptr,rc));
    return(rc);
}


/******************************************************************************
 * Name: fix_in_use_file_hdr
 *
 * Description: fixes a file header that was corrupted during creation
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fh -- a pointer to the buffer header in memory
 *   fh_ptr -- a pointer to the file header in file system memory
 *   error -- a pointer to the error variable to which to write an error code
 *     if there is an error
 *
 * Output: the fields in the file header (in local memory, not file
 *   system memory) are modified.  If the operation is unsuccessful, the
 *   next field of the file header is set to 0, and an error code is written
 *   to the error variable.
 *
 * Return Value: None
 *
 *******************************************************************************/
STATIC void fix_in_use_file_hdr (fslib_device_t *dev,fslib_file_hdr_t *fh,fslib_ptr fh_ptr,fslib_error *error)
{
    fslib_error rc;
    
    PRT(P4E,("fix_in_use_file_hdr(%lx,%lx,%lx)\n",dev,fh,fh_ptr));

    /*
     * Fill out some fields just to get them set to reasonable values
     */
    fh->magic = FSLIB_FILE_HDR_MAGIC;
    fh->index = 0;
    fh->file_name[0] = '\0';
    fh->type = 0;
    fh->time = 0;
    fh->file_flags = 0;
    fh->deleted = 0;
    fh->undeleted = 0;

    /*
     * figure out what the next pointer will be
     */
    dev_seek(dev,fh_ptr,sizeof(fslib_file_hdr_t),&fh->next);

    /*
     * write the new file header to the file system 
     */
    rc = write_file_hdr(dev,fh,fh_ptr);
    if (rc != FSLIB_OK) {
	fh->next = 0;
	*error = rc;
    } else {
	/*
	 * modify the header flags field so the file header is marked bad.
	 */
	fh->hdr_flags = FSLIB_HDR_FLAGS_ERROR;
	rc = write_file_hdr(dev,fh,fh_ptr);
	if (rc != FSLIB_OK) {
	    fh->next = 0;
	    *error = rc;
	}
    }

    PRT(P4X,("fix_in_use_file_hdr(%lx,%lx,%lx) = done\n",dev,fh,fh_ptr));

}


/******************************************************************************
 * Name: fix_created_file_hdr
 *
 * Description: fixes a file header that was corrupted during writing
 *   This function is used to "fix" a file header that has the created bit
 *   set, but does not have the valid bit set.  This kind of file can occur if
 *   power is lost during the writing of a file.  The header is written, but
 *   the file has not been closed.  Any number of bytes could have been
 *   written to this file, and they must now be skipped and ignored.
 *
 *   This function starts searching backwards through the file system memory
 *   to find the highest address that has been written (i.e. is not in the
 *   erased state), and adjusts the pointer in the file header to point past
 *   the data that was written for the file.  Once it has found the place to
 *   point the file header, it writes the file header "next" field, and then
 *   writes the file header "hdr_flags" field to indicate this file has an
 *   error. 
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fh -- a pointer to the buffer header in memory
 *   fh_ptr -- a pointer to the file header in file system memory
 *   error -- a pointer to the error variable to which to write an error code
 *     if there is an error
 *
 * Output: the fields in the file header (in local memory, not file
 *   system memory) are modified.  If the operation is unsuccessful, the
 *   next field of the file header is set to 0, and an error code is written
 *   to the error variable.
 *
 * Return Value: None
 *
 *******************************************************************************/
STATIC void fix_created_file_hdr (fslib_device_t *dev,fslib_file_hdr_t *fh,fslib_ptr fh_ptr,fslib_error *error)
{
    fslib_error rc;
    int actual;
    ulong status_length;
    int found;
    int sector;
    int i;
    fslib_ptr free_ptr;
    fslib_ptr end_ptr;
    static ulong buffer[256];
    fslib_ptr buffer_base_ptr;
    
    PRT(P4E,("fix_created_file_hdr(%lx,%lx,%lx)\n",dev,fh,fh_ptr));

    /*
     * figure out what the next pointer will be
     * It will be somewhere between fh->next and the 
     * end of the file system.
     */
    dev_seek(dev,fh_ptr,sizeof(fslib_file_hdr_t),&fh->next);
    buffer_base_ptr = dev->file_system_end + 1 - sizeof(buffer);
    free_ptr = dev->file_system_end + 1;
    end_ptr = fh_ptr + sizeof(fslib_file_hdr_t);
    status_length = 0;
    found = FALSE;
    while (!found) {
	/*
	 * read a buffer from the file system
	 */
	actual = dev_read(dev,buffer_base_ptr,(char *)buffer,sizeof(buffer),NULL);
	if (actual != sizeof(buffer)) {
	    if (actual < 0)
		*error = actual;
	    else
		*error = FSLIB_ERROR_FIXING_CREATED_FILE_HDR;
	    break;
	} else {
	    /* 
	     * check the buffer data
	     * If we find an unerased word, we need to make sure it is in a
	     *  good sector before we say we have found the spot.
	     */
	    i = (sizeof(buffer)/sizeof(ulong)) - 1;
	    while ((i >= 0) && (!found)) {
		if (buffer[i] != dev->dib.erased_state) {
		    sector = get_sector_num(dev,free_ptr);
		    if (good_sector(dev,sector) != FSLIB_SECTOR_BAD) {
			found = TRUE;
			break;
		    }
		}
		/* 
		 * back up the pointer and counter (we are going backwards
		 * through the buffer (and file system memory)
		 */
		free_ptr -= sizeof(ulong);
		i--;
		/* 
		 * if we have gotten all the way back to the end of the
		 * file header we are looking at, so we just use that ptr 
		 */
		if (free_ptr <= end_ptr) {
		    if (free_ptr < end_ptr)
			free_ptr += sizeof(ulong); /* restore previous */
		    found = TRUE;
		    break;
		}
	    }
	    buffer_base_ptr -= sizeof(buffer);
	    status_length += sizeof(buffer);
	    if (status_length >= FSLIB_FIX_CREATED_FILE_HDR_STATUS_LENGTH_LIMIT) {
		print_status_update("X", FALSE /* not quiet */);
		status_length = 0;
	    }
	}
    }

    /* 
     * If we have found the right place in memory for the next file header to
     * start, we have to change the current file header to point there, and
     * write the changed file header to the file system
     */
    if (found) {
	/*
	 * fill in the next field in the file header now that we have figured
	 * out what it is supposed to be.
	 */
	fh->next = free_ptr;

	/*
	 * calculate the data length of the error file by calculating
         * the number of good bytes from the header to the start of
	 * free memory, and subtracting the size of the file hdr.
	 */
	fh->length = get_bytes(dev,fh_ptr,free_ptr);
	fh->length -= sizeof(fslib_file_hdr_t);
	if ((int)fh->length < 0) {
	    fh->length = 0;
	}

	/*
	 * write the new file header to the file system 
	 */
	rc = write_file_hdr(dev,fh,fh_ptr);
	if (rc != FSLIB_OK) {
	    fh->next = 0;
	    *error = rc;
	} else {
	    /*
	     * modify the header flags field so the file header is marked bad.
	     */
	    fh->hdr_flags = FSLIB_HDR_FLAGS_ERROR;
	    rc = write_file_hdr(dev,fh,fh_ptr);
	    if (rc != FSLIB_OK) {
		fh->next = 0;
		*error = rc;
	    }
	}
    } else {
	fh->next = 0;	/* indicate no next file header is found */
	*error = FSLIB_ERROR_FIXING_CREATED_FILE_HDR;
    }

    PRT(P4X,("fix_created_file_hdr(%lx,%lx,%lx) = done\n",dev,fh,fh_ptr));

}


/******************************************************************************
 * Name: create_new_file
 *
 * Description: creates a new file by writing a file header to the file system
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fp -- a pointer to the fslib_file_rec_t to set
 *   file_name -- pointer to the name of the file to create
 *   mode -- pointer to the mode variable passed to open 
 *     (jack, not sure what to do with this now)
 *   error -- pointer to the error variable to set if there is an error.
 *
 * Return Value: 
 *   return code -- FSLIB_OK = new file created, otherwise error code
 *
 *******************************************************************************/
STATIC fslib_error create_new_file (fslib_device_t *dev, fslib_file_rec_t *fp, const char *file_name, ulong mode)
{
    fslib_error        rc;
    fslib_error        error;
    fslib_file_hdr_t  *fh;
    fslib_ptr          fh_ptr;
    fslib_ptr          next;
    
    PRT(P4E,("create_new_file(%lx,%lx,%s,%d)\n",dev,fp,file_name,mode));

    rc = FSLIB_OK;

    if (dev->writable == FALSE) {
	rc = FSLIB_ERROR_DEVICE_IS_READ_ONLY;
    } else if (dev->file_open_for_write) {
	rc = FSLIB_ERROR_CANNOT_OPEN_ANOTHER_FILE_FOR_WRITE;
    } else {
	/*
	 * initialize the file header to the erased state of flash, and
	 * then set the IN USE bit
	 */
	fh = &fp->hdr;
	memset(fh,(uchar)dev->dib.erased_state,sizeof(fslib_file_hdr_t));
	fh->hdr_flags  = FSLIB_HDR_FLAGS_IN_USE;
	fh->file_flags = 0;
	fh->deleted    = 0;
	fh->undeleted  = 0;
	
	/*
	 * find the next place to start a file if one is not already set.
	 */
	if (dev->next_new_file_hdr_ptr == 0) {
	    dev->next_new_file_hdr_ptr = find_next_new_file_hdr_ptr(dev,&error);
	    if (dev->next_new_file_hdr_ptr == -1) {
		dev->next_new_file_hdr_ptr = 0;		
		rc = error;
	    } else if (dev->next_new_file_hdr_ptr == 0) {
		rc = FSLIB_ERROR_FILE_SYSTEM_FULL;
	    }
	}

	/*
	 * write the header flags IN USE
	 */
	if (rc == FSLIB_OK) {
	    fh_ptr = dev->next_new_file_hdr_ptr;
	    rc = write_file_hdr(dev,fh,fh_ptr);
	    if (rc == FSLIB_OK) {
		/*
		 * Now write the new file header in its entirety
		 * create the new file header in the file record 
		 */
		fh = &fp->hdr;
		fh->magic        = FSLIB_FILE_HDR_MAGIC;
		fh->index        = dev->next_index;
		strcpy(fh->file_name,file_name);
		fh->time         = FSLIB_ralib->time();
		fh->file_flags   = mode;
		
		rc = write_file_hdr(dev,fh,fh_ptr);
		if (rc == FSLIB_OK) {
		    /*
		     * Now write the header flags CREATED
		     */
		    fh->hdr_flags = FSLIB_HDR_FLAGS_CREATED;
		    rc = write_file_hdr(dev,fh,fh_ptr);
		    if (rc == FSLIB_OK) {
			/* 
			 * Set up all the fields in the file record
			 */
			dev->file_open_for_write = TRUE;
			dev->next_index++;
			dev_seek(dev, fh_ptr, sizeof(fslib_file_hdr_t), &next);
			fp->dev            = dev;
			fp->hdr_phys_ptr   = dev->next_new_file_hdr_ptr;
			fp->start_phys_ptr = next;
			fp->curr_phys_ptr  = next;
			fp->curr_ptr       = 0;
			fp->state          = FSLIB_FILE_STATE_OPEN;
			fp->type           = FSLIB_FILE_TYPE_NORMAL;
			fp->writable       = TRUE;
			fp->readable       = FALSE;
		    }
		}
	    }
	}
    }
    
    PRT(P4X,("create_new_file(%lx,%lx,%s,%d) = %d\n",dev,fp,file_name,mode,rc));
    return(rc);
}


/******************************************************************************
 * Name: mark_file_deleted
 *
 * Description: marks an existing file deleted by changing the deleted flags.
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fh -- a pointer to a copy of the file header for the file in local memory
 *   fh_ptr -- the pointer to the file header in file system memory
 *
 * Return Value: 
 *   return code -- FSLIB_OK = file deleted, FSLIB_ERROR = file not deleted,
 *     error variable set.
 *
 *******************************************************************************/
STATIC fslib_error mark_file_deleted (fslib_device_t *dev,fslib_file_hdr_t *fh,fslib_ptr fh_ptr)
{
    fslib_error rc;
    
    PRT(P4E,("mark_file_deleted(%lx,%lx,%lx)\n",dev,fh_ptr,fh));

    /*
     * create the new deleted mask for the file
     */
    for (; fh->deleted != 0xffff; fh->deleted = (fh->deleted << 1) | 1) {
	if ((fh->deleted ^ fh->undeleted) != 0)
	    break;
    }
    
    /*
     * write the new deleted vector to the file header
     */
    rc = write_file_hdr(dev,fh,fh_ptr);
    
    PRT(P4X,("mark_file_deleted(%lx,%lx,%lx) = %d\n",dev,fh_ptr,fh,rc));
    return(rc);
}


/******************************************************************************
 * Name: mark_file_undeleted
 *
 * Description: marks an existing file undeleted by changing the undeleted flags.
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   fh -- a pointer to a copy of the file header for the file in local memory
 *   fh_ptr -- the pointer to the file header in file system memory
 *
 * Return Value: 
 *   return code -- FSLIB_OK = file undeleted, otherwise error code
 *
 *******************************************************************************/
STATIC fslib_error mark_file_undeleted (fslib_device_t *dev,fslib_file_hdr_t *fh,fslib_ptr fh_ptr)
{
    fslib_error rc;
    
    PRT(P4E,("mark_file_undeleted(%lx,%lx,%lx)\n",dev,fh_ptr,fh));

    /*
     * create the new undeleted mask for the file
     */
    fh->undeleted = fh->deleted & 0x7fff;
    if (fh->undeleted != fh->deleted) {
	rc = FSLIB_ERROR_FILE_NOT_UNDELETABLE;
    } else {
	/*
	 * write the new undeleted vector to the file header
	 */
	rc = write_file_hdr(dev,fh,fh_ptr);
    }
    
    PRT(P4X,("mark_file_undeleted(%lx,%lx,%lx) = %d\n",dev,fh_ptr,fh,rc));
    return(rc);
}


/******************************************************************************
 * Name: valid_file_id
 *
 * Description: checks to see if a given file id is valid
 *
 * Input: 
 *   file_id -- a pointer to the file id to validate.
 *   error -- a pointer to the error code variable.  If the return value
 *     is FALSE, an error code is written to this variable.
 *
 * Return Value:
 *   boolean: TRUE = file ID is valid, FALSE = file ID is NOT valid
 *
 *******************************************************************************/
STATIC boolean valid_file_id (const char *file_id, fslib_error *error)
{
    boolean rc;
    char *file_name;
    
    PRT(P1E,("valid_file_id(%s)\n",file_id));
    rc = FALSE;
    /*
     * check that the file name is made up of <valid dev id>:<valid file name>
     */
    file_name = strchr(file_id,':');
    if (file_name == NULL) {
	*error = FSLIB_ERROR_INVALID_FILE_ID;
	PRT(PERR,("valid_file_id(%s) -- no \":\" in file id\n",file_id));
    } else {
	/* 
	 * the file name starts immediately after the ":"
	 */
	file_name++;
	/* 
	 * if the device id and the file name are OK, the file ID is OK
	 */
	if (valid_dev_id(file_id,error)) {
	    if (valid_file_name(file_name,error)) {
		rc = TRUE;
	    }
	}
    }
    
    PRT(P1X,("valid_file_id(%s) = %s(%d)\n",file_id,torf[rc],*error));
    return(rc);
}


/******************************************************************************
 * Name: valid_dev_id
 *
 * Description: checks to see if a given device id is valid.
 *
 * Input: 
 *   dev_id -- a pointer to the device id to validate.
 *   error -- a pointer to the error code variable.  If the return value
 *     is FALSE, an error code is written to this variable
 *
 * Return Value:
 *   boolean: TRUE = dev ID is valid, FALSE = dev ID is NOT valid
 *
 *******************************************************************************/
STATIC boolean valid_dev_id (const char *dev_id, fslib_error *error)
{
    boolean rc;
    char temp_dev_id[FSLIB_FILE_ID_MAX + 1];
    char *separator;
    int  dev_id_len;
    int  span_len;
    int  dev_num;
    
    rc = FALSE;
    PRT(P1E,("valid_dev_id(%s)\n",dev_id));
    
    /*
     * if the device ID is part of a file ID (and therefore is terminated with
     * a colon) we need a copy of the device ID that is NULL terminated
     */
    strncpy(temp_dev_id,dev_id,sizeof(temp_dev_id));
    separator = strchr(temp_dev_id,':');
    if (separator != NULL)
	*separator = '\0';
    else
	temp_dev_id[FSLIB_FILE_ID_MAX] = '\0';
    
    /*
     * check the length of the device ID
     */
    dev_id_len = strlen(temp_dev_id);
    if (dev_id_len > FSLIB_DEV_ID_MAX) {
	*error = FSLIB_ERROR_INVALID_DEVICE_ID;
	PRT(PERR,("device ID too long (%d > %d)\n",dev_id_len,FSLIB_DEV_ID_MAX));
    } else {
	/*
	 * check the characters
	 */
	span_len = strspn(temp_dev_id,file_system_character_set);
	if (span_len != dev_id_len) {
	    *error = FSLIB_ERROR_INVALID_DEVICE_ID;
	    PRT(PERR,("invalid char \"%c\"at index %d\n",temp_dev_id[span_len],span_len));
	} else {
	    dev_num = FSLIB_ralib->get_dev_num(temp_dev_id);
	    if (dev_num < 0) {
		*error = FSLIB_ERROR_UNKNOWN_DEVICE;
		PRT(PERR,("device unrecognized by RALIB (rc = %d)\n",dev_num));
	    } else {
		/*
		 * everything is OK
		 */
		rc = TRUE;
	    }
	}
    }        
    PRT(P1X,("valid_dev_id(%s) = %s\n",dev_id,torf[rc]));
    return(rc);
}


/******************************************************************************
 * Name: valid_file_name
 *
 * Description: checks to see if a given file name is valid.
 *
 * Input: 
 *   file_name -- a pointer to the file name to validate.
 *   error -- a pointer to the error code variable.  If the return value
 *     is FALSE, an error code is written to this variable
 *
 * Return Value:
 *   int -- boolean: TRUE = file name is valid, FALSE = file name is NOT valid
 *
 *******************************************************************************/
STATIC boolean valid_file_name (const char *file_name, fslib_error *error)
{
    boolean rc;
    int file_name_len;
    int span_len;
    
    rc = FALSE;
    PRT(P1E,("valid_file_name(%s)\n",file_name));
    
    /*
     * check that the file name pointer is not null
     */
    if (file_name == NULL) {
	*error = FSLIB_ERROR_NULL_FILE_NAME_PTR;
	PRT(PERR,("null file name pointer\n"));
    } else {
	/*
	 * check that the file name is not null
	 */
	file_name_len = strlen(file_name);
	if (file_name_len == 0) {
	    *error = FSLIB_ERROR_INVALID_FILE_NAME;
	    PRT(PERR,("file name length is 0\n"));
	} else {
	    /*
	     * check that the file name is not too long
	     */
	    if (file_name_len > FSLIB_FILE_NAME_MAX) {
		*error = FSLIB_ERROR_INVALID_FILE_NAME;
		PRT(PERR,("too long (%d > %d)\n",file_name_len,FSLIB_FILE_NAME_MAX));
	    } else {
		/*
		 * check that the file name is made up of valid chars
		 */
		span_len = strspn(file_name,file_system_character_set);
		if (span_len != file_name_len) {
		    *error = FSLIB_ERROR_INVALID_FILE_NAME;
		    PRT(PERR,("invalid char \"%c\" at index %d\n",file_name[span_len],span_len));
		} else {
		    /*
		     * everything is OK
		     */
		    rc = TRUE;
		}
	    }
	}
    }
    PRT(P1X,("valid_file_name(%s) = %s\n",file_name,torf[rc]));
    return(rc);
}


/******************************************************************************
 * Name: valid_file_flags
 *
 * Description: checks to see if a given file flags is valid for the open function
 *
 * Input: 
 *   flags -- flags to validate.
 *   error -- a pointer to the error code variable.  If the return value
 *     is FALSE, an error code is written to this variable.
 *
 * Return Value:
 *   int -- boolean: TRUE = file mode is valid, FALSE = file mode is invalid
 *
 *******************************************************************************/
STATIC boolean valid_file_flags (int flags, fslib_error *error)
{
    boolean rc;
    
    /*
     * check that the file mode is one of the valid modes
     */
    flags &= O_RW_MASK;
    if (  (flags == O_RDONLY)
	|| (flags == (O_WRONLY | O_APPEND | O_TRUNC))
	|| (flags == (O_WRONLY | O_APPEND | O_TRUNC | O_CREAT)) ) {
	rc = TRUE;
    } else {
	*error = FSLIB_ERROR_INVALID_FLAGS;
	rc = FALSE;
    }
    
    PRT(P1,("valid_file_flags(%lx) = %s\n",flags,torf[rc]));
    return(rc);
}


/******************************************************************************
 * Name: valid_whence
 *
 * Description: checks that the whence parameter to seek is valid
 *
 * Input: 
 *   whence -- whence parameter to the seek function
 *   error -- a pointer to the error code variable.  If the return value
 *     is FALSE, an error code is written to this variable.
 *
 * Return Value:
 *   int -- boolean: TRUE = whence is valid, FALSE = whence is invalid (error set)
 *
 *******************************************************************************/
STATIC boolean valid_whence (int whence, fslib_error *error)
{
    boolean rc;
    
    /*
     * check that the file mode is one of the valid modes
     */
    if (  (whence == SEEK_CUR)
	|| (whence == SEEK_SET)
	|| (whence == SEEK_END) ) {
	rc = TRUE;
    } else {
	*error = FSLIB_ERROR_INVALID_WHENCE;
	rc = FALSE;
    }
    
    PRT(P1,("valid_whence(%d) = %s\n",whence,torf[rc]));
    return(rc);
}


/******************************************************************************
 * Name: valid_file_hdr
 *
 * Description: checks fields in file header for general sanity
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   fh -- a pointer to the file header to check
 *   error -- a pointer to the error code variable.  If the return value
 *     is FALSE, an error code is written to this variable.
 *
 * Return Value:
 *   int -- boolean: TRUE = file header is valid, FALSE = header is invalid.
 *
 *******************************************************************************/
STATIC boolean valid_file_hdr (fslib_device_t *dev, fslib_file_hdr_t *fh, fslib_error *error)
{
    boolean rc;
    
    PRT(P3E,("valid_file_hdr(%lx,%lx)\n",dev,fh));

    /*
     * This function should never get passed a file header that is all erased
     * bytes, so if the header flags are 0, this is a bad file header.
     */
    if (fh->hdr_flags == 0) {
	rc = FALSE;
    }

    /*
     * if the header flags say the file is in use, we can only assume
     * it is roughly OK -- it will be fixed up later.
     */
    else if (fh->hdr_flags == FSLIB_HDR_FLAGS_IN_USE) {
	rc = FALSE;
    }

    /*
     * if the header flags say the file is created, we can check the magic
     * number, the file name, and the deleted/undeleted status as well as
     * the things we checked for IN USE
     */
    else if (fh->hdr_flags == FSLIB_HDR_FLAGS_CREATED) {
	if (   (fh->length != dev->dib.erased_state)
	    || (fh->crc    != dev->dib.erased_state)
	    || (fh->next   != dev->dib.erased_state) ) {
	    *error = FSLIB_ERROR_INVALID_CREATED_FILE_HDR;
	    rc = FALSE;

	} else if (!valid_file_name(fh->file_name,error)) {
	    *error = FSLIB_ERROR_INVALID_CREATED_FILE_HDR_NAME;
	    rc = FALSE;

	} else if (fh->magic != FSLIB_FILE_HDR_MAGIC) {
	    *error = FSLIB_ERROR_INVALID_CREATED_FILE_HDR_MAGIC;
	    rc = FALSE;

	} else if ( (fh->deleted   != 0) || (fh->undeleted != 0) ) {
	    *error = FSLIB_ERROR_INVALID_CREATED_FILE_HDR_DELETE;
	    rc = FALSE;

	} else {
	    rc = TRUE;
	}
    }

    /*
     * if the header flags say the file is valid, we can check the magic
     * number, the file name, and the the crc and the next fields
     */
    else if (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID) {
	if (   (fh->crc == 0)
	    || (fh->crc == 0xffffffff) ) {
	    *error = FSLIB_ERROR_INVALID_FILE_HDR_CRC;
	    rc = FALSE;

	} else if (!valid_file_name(fh->file_name,error)) {
	    *error = FSLIB_ERROR_INVALID_FILE_HDR_NAME;
	    rc = FALSE;

	} else if (fh->magic != FSLIB_FILE_HDR_MAGIC) {
	    *error = FSLIB_ERROR_INVALID_FILE_HDR_MAGIC;
	    rc = FALSE;

	} else if (fh->next == dev->dib.erased_state) {
	    *error = FSLIB_ERROR_INVALID_FILE_HDR_NEXT;
	    rc = FALSE;

	} else {
	    rc = TRUE;
	}
    }

    /*
     * if the header flags say the file header has an error that is ok, no
     * checking needs to be done.
     */
    else if (fh->hdr_flags == FSLIB_HDR_FLAGS_ERROR) {
	rc = TRUE;
    }

    /*
     * any other value of the header flags is invalid
     */
    else {
	rc = FALSE;
    }

    PRT(P3X,("valid_file_hdr(%lx,%lx) = %d (%d)\n",dev,fh,rc,*error));
    return(rc);    
}


/******************************************************************************
 * Name: valid_dib
 *
 * Description: checks fields in the device info block for general sanity.
 *
 * Input: 
 *   dib -- a pointer to the device info block
 *
 * Return Value:
 *   int -- boolean: TRUE = dib is valid, FALSE = dib is invalid.
 *
 *******************************************************************************/
boolean valid_dib (fslib_device_info_block_t *dib, fslib_error *error)
{
    boolean rc;
    int i;
    ulong bad_sector_map_coverage;
    
    PRT(P1E,("valid_dib(%lx)\n",dib));

    rc = TRUE;
    
    /*
     * check that the DIB has the correct magic number
     */
    if (dib->magic != FSLIB_DEVICE_INFO_BLOCK_MAGIC) {
	PRT(PERR,("bad magic number %lx (should be %lx)\n",
		dib->magic,FSLIB_DEVICE_INFO_BLOCK_MAGIC));
	*error = FSLIB_ERROR_BAD_DIB;
	rc = FALSE;
    }
    
    /*
     * check that the length of the DIB is correct
     */
    if (dib->length < sizeof(fslib_device_info_block_t)) {
	PRT(PERR,("bad DIB length %d (mus be greater than %d)\n",
		dib->length,sizeof(fslib_device_info_block_t)));
	*error = FSLIB_ERROR_BAD_DIB;
	rc = FALSE;
    }
    
    /*
     * check the file system version number is supported
     */
    if (  (dib->file_system_version < FSLIB_MIN_SUPPORTED_VERSION) 
	|| (dib->file_system_version > FSLIB_MAX_SUPPORTED_VERSION)) {
	PRT(PERR,("version %d not supported (ver %d thru %d are supported)\n",
		dib->file_system_version,FSLIB_MIN_SUPPORTED_VERSION,FSLIB_MAX_SUPPORTED_VERSION));
	*error = FSLIB_ERROR_BAD_DIB;
	rc = FALSE;
    }
    
    /*
     * check if the programming algorithm is supported
     */
    for (i = 0; FSLIB_prog_table[i].algorithm != 0; i++) {
	if (FSLIB_prog_table[i].algorithm == dib->prog_algorithm)
	    break;
    }
    if (FSLIB_prog_table[i].algorithm == 0) {
	PRT(PERR,("programming algorithm %d not supported -- read only\n",
		dib->prog_algorithm));
	*error = FSLIB_ERROR_BAD_DIB;
	rc = FALSE;
    }
    
    /*
     * check if the bad sector map overlaps the file system area
     */
    if ((dib->bad_sector_map_offset + dib->bad_sector_map_length) > dib->file_system_offset) {
	PRT(PERR,("bad sector map overlaps file system area (%d + %d > %d)\n",
		dib->bad_sector_map_offset,dib->bad_sector_map_length,dib->file_system_offset));
	*error = FSLIB_ERROR_BAD_DIB;
	rc = FALSE;
    }
    
    /*
     * check if the bad sector map covers all of the file system area
     */
    bad_sector_map_coverage = dib->bad_sector_map_length * 8 * dib->sector_size;
    if (dib->length > bad_sector_map_coverage) {
	PRT(PERR,("bad sector map does not cover entire device (%d > %d)\n",
		dib->length,bad_sector_map_coverage));
	*error = FSLIB_ERROR_BAD_DIB;
	rc = FALSE;
    }
    
    PRT(P1X,("valid_dib(%lx) = %s\n",dib,torf[rc]));
    return(rc);
    
}


/******************************************************************************
 * Name: dev_read
 *
 * Description: reads bytes from a device and handles the bad sector stuff 
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   src -- a pointer into device memory from which to start reading
 *   dst -- a pointer to local memory in which to put the data
 *   length -- the number of bytes to read from the device
 *   next -- a pointer to a pointer where the address of the next byte in the
 *     device is.  In the case of bad sectors, this may not be the same as
 *     (src + bytes).  If this information is not needed this can be a NULL
 *     pointer.
 *
 * Return Value:
 *   actual -- the actual number of bytes copied from the device.  If an error
 *     occurred, an error code is returned.  All error codes are negative.
 *
 ******************************************************************************/
STATIC ulong dev_read (fslib_device_t *dev, fslib_ptr src, char *dst, ulong length, ulong *next_ptr)
{
    int             i;
    ulong           actual;
    ulong           this_sector;
    ulong           this_sector_length;
    int             this_sector_actual;
    fslib_ptr       end_of_region;
    fslib_read_fn_t read_fn;
    fslib_ptr       read_src;
    
    PRT(P7E,("dev_read(%lx,%lx,%lx,%d)\n",dev,src,dst,length));
    
    /*
     * skip bad blocks if any
     */
    actual = 0;
    src = skip_bad_sectors(dev,src);
    
    /*
     * while there are still bytes to copy (read), and the end of the file
     * system data area has not been reached
     * 
     * copy the bytes in this sector, 
     * increment all the pointers and counts,
     * skip any bad sector(s).
     *
     * Since this function is used to read all parts of the device
     * we can't limit it to reading to the file system area, 
     * but we can make sure that we don't cross any boundaries from 
     * one part of the device to another.
     */
    end_of_region = get_end_of_region(dev,src);
    read_fn = dev->read_fn;
    if (read_fn == NULL) {
	actual = FSLIB_ERROR_NO_READ_FUNCTION;
    } else {
	
	while ((length > 0) && (src <= end_of_region)) {
	    this_sector_length = get_bytes_left_this_sector(dev,src);
	    if (this_sector_length > length)
		this_sector_length = length;
	    /*
	     * Just before we actually read the device, we need to check if this
	     * sector has been spared, and adjust the address to the spare sector 
	     * if necessary 
	     */
	    this_sector = get_sector_num(dev,src);
	    read_src = src;
	    for (i = 0; i < dev->dib.num_spare_sectors; i++) {
		if (this_sector == dev->dib.spare[i].bad_sector_num) {
		    read_src = get_sector_ptr(dev,dev->dib.spare[i].spare_sector_num)
			+ (src % dev->dib.sector_size);
		    break;
		}
	    }
	    /* 
	     * Now do the actual read using the read function vector set
	     * up in the device structure -- it will call the RALIB.
	     */
	    this_sector_actual = read_fn(dev->dev_num,read_src,dst,this_sector_length);
	    /*
	     * If there was an error, convert the error code, and if the device
	     * has been removed, clean up all the data structures
	     */
	    if (this_sector_actual < 0) {
		actual = this_sector_actual;
		if (actual == FSLIB_ERROR_NO_DEVICE) {
		    remove_dev(dev);
		}
		break;
	    }
	    length -= this_sector_actual;
	    actual += this_sector_actual;
	    dst    += this_sector_actual;
	    src    += this_sector_actual;
	    src = skip_bad_sectors(dev,src);
	    if (this_sector_actual != this_sector_length)
		break;
            /*
             * Give other processes a chance to run.
             */
            ralib_yield(0);
	}
    }
    /*
     * if the caller wants to know where the next byte of the file system
     * is, copy the present value of src to next.  
     */
    if (next_ptr != NULL) {
	*next_ptr = src;
    }
    
    PRT(P7X,("dev_read(%lx,%lx,%lx,%d) = %d (%lx)\n",dev,src,dst,length,actual,src));
    
    return(actual);
}
    

/******************************************************************************
 * Name: dev_seek
 *
 * Description: seeks from a given physical address in the device plus an offset
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   src -- a pointer into device memory from which to start reading
 *   offset -- the number of bytes to seek into the device
 *   next_ptr -- a pointer to a pointer where the address of the next byte in
 *     the device is.  In the case of bad sectors, this may not be the same as
 *     (src + offset).  This may be a NULL pointer, but it is the output of
 *     the function, so it might not make any sense to pass a NULL pointer.
 *
 * Return Value: None
 *
 ******************************************************************************/
STATIC void dev_seek (fslib_device_t *dev, fslib_ptr src, ulong offset, ulong *next_ptr)
{
    ulong       this_sector_length;
    fslib_ptr   end_of_region;
    
    PRT(P7E,("dev_seek(%lx,%lx,%d)\n",dev,src,offset));

    /*
     * there are bad blocks, read block by block skipping the bad blocks
     */
    src = skip_bad_sectors(dev,src);
    
    /*
     * while there are still bytes to seek, and the end of the file
     * system data area has not been reached, skip the bytes in this sector
     *
     * Since this function is used to seek all parts of the device
     * we can't limit it to seeking to the file system area, 
     * but we can make sure that we don't cross any boundaries from 
     * one part of the device to another.
     */
    end_of_region = get_end_of_region(dev,src);
    while ((offset > 0) && (src <= end_of_region)) {
	this_sector_length = get_bytes_left_this_sector(dev,src);
	if (this_sector_length > offset)
	    this_sector_length = offset;
	offset -= this_sector_length;
	src    += this_sector_length;
	src = skip_bad_sectors(dev,src);
    }
    
    /*
     * copy the present value of src to next.  
     */
    if (next_ptr != NULL) {
	*next_ptr = src;
    }

    PRT(P7X,("dev_seek(%lx,%lx,%d) = (%lx)\n",dev,src,offset,src));

}


/******************************************************************************
 * Name: dev_write
 *
 * Description: writes bytes to a device and handles the bad sector stuff 
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   src -- a pointer to local memory buffer that contains data to write
 *   dst -- a pointer into device memory at which to start writing
 *   length -- the number of bytes to write to the device
 *   next -- a pointer to a pointer where the address of the next byte in the
 *     device is.  In the case of bad sectors, this may not be the same as
 *     (src + bytes).  If this information is not needed this can be a NULL
 *     pointer.
 *
 * Return Value:
 *   actual -- the actual number of bytes written to the device.  If an error
 *     occurred, an error code is returned.  All error codes are negative.
 *
 ******************************************************************************/

STATIC ulong dev_write (fslib_device_t *dev, char *src, fslib_ptr dst, ulong length, ulong *next_ptr)
{
    int              i;
    ulong            actual;
    ulong            this_sector;
    ulong            this_sector_length;
    int              this_sector_actual;
    fslib_write_fn_t write_fn;
    fslib_ptr        end_of_region;
    fslib_ptr        write_dst;
    
    PRT(P8E,("dev_write(%lx,%lx,%lx,%d)\n",dev,src,dst,length));

    /*
     * we always write sector at a time regardless of whether there are
     * bad sectors or not.
     */
    actual = 0;
    dst = skip_bad_sectors(dev,dst);
    
    /*
     * while there are still bytes to copy (write), and the end of the file
     * system data area has not been reached
     * 
     * write the bytes to this sector, 
     * increment all the pointers and counts,
     * skip any bad sector(s).
     * 
     * Since this function is used to write the squeeze log and the
     * squeeze buffer and the file system area, we can't limit it to
     * writing to the file system area, but we can make sure that we
     * don't cross any boundaries from the file system into the
     * squeeze log, or from the squeeze log into the squeeze buffer.
     */
    end_of_region = get_end_of_region(dev,dst);
    write_fn = dev->write_fn;
    if (write_fn == NULL) {
	actual = FSLIB_ERROR_NO_WRITE_FUNCTION;
    } else {

	while ((length > 0) && (dst <= end_of_region)) {
	    this_sector_length = get_bytes_left_this_sector(dev,dst);
	    if (this_sector_length > length)
		this_sector_length = length;
	    /*
	     * Just before we actually write the device, we need to check if this
	     * sector has been spared, and adjust the address to the spare sector 
	     * if necessary
	     */
	    this_sector = get_sector_num(dev,dst);
	    write_dst = dst;
	    for (i = 0; i < dev->dib.num_spare_sectors; i++) {
		if (this_sector == dev->dib.spare[i].bad_sector_num) {
		    write_dst = get_sector_ptr(dev,dev->dib.spare[i].spare_sector_num)
			+ (dst % dev->dib.sector_size);
		    break;
		}
	    }
	    /*
	     * Now actually write the device using the write function
	     */
	    this_sector_actual = write_fn(dev->dev_num,src,dst,this_sector_length);
	    /*
	     * If there was an error, convert the error code, and if the device
	     * has been removed, clean up all the data structures
	     */
	    if (this_sector_actual < 0) {
		actual = this_sector_actual;
		if (actual == FSLIB_ERROR_NO_DEVICE) {
		    remove_dev(dev);
		}
		break;
	    }
	    length -= this_sector_actual;
	    actual += this_sector_actual;
	    dst    += this_sector_actual;
	    src    += this_sector_actual;
	    dst = skip_bad_sectors(dev,dst);
	    if (this_sector_actual != this_sector_length)
		break;
	}
    }
    
    /*
     * if the caller wants to know where the next byte of the file system
     * is, copy the present value of dst to next.  
     */
    if (next_ptr != NULL) {
	*next_ptr = dst;
    }

    PRT(P8X,("dev_write(%lx,%lx,%lx,%d) = %d (%lx)\n",dev,src,dst,length,actual,dst));

    return(actual);
}


/******************************************************************************
 * Name: skip_bad_sectors
 *
 * Description: given a pointer, returns a pointer to valid data area
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   ptr -- a pointer into device memory
 *
 * Return Value:
 *   fslib_ptr -- pointer to a valid area in device memory.  If the ptr already
 *     points to a byte that is part of a valid sector, the ptr is returned
 *     unchanged.  If there are no more good bytes in the file system, a
 *     pointer to one byte past the end of the file system is returned.
 *
 ******************************************************************************/
STATIC fslib_ptr skip_bad_sectors (fslib_device_t *dev, fslib_ptr ptr)
{
    ulong    last_sector;
    ulong    original_sector_num;
    ulong    sector_num;
    
    PRT(P5E,("skip_bad_sectors(%lx,%lx)\n",dev,ptr));

    /*
     * Skip over all the bad sectors that src points to
     */
    last_sector = dev->last_sector;
    original_sector_num  = get_sector_num(dev,ptr);
    sector_num = original_sector_num;
    while ((sector_num <= last_sector) && (good_sector(dev,sector_num) == FSLIB_SECTOR_BAD)) {
	sector_num++;
    }
    if (sector_num != original_sector_num) {
	ptr = get_sector_ptr(dev,sector_num);
    }

    PRT(P5X,("skip_bad_sectors(%lx,%lx) = %lx\n",dev,ptr,ptr));

    return(ptr);
}


/******************************************************************************
 * Name: get_region
 *
 * Description: given a pointer, returns the region of the device it points to
 *
 * Input:
 *   dev -- a pointer to the device structure for the device
 *   ptr -- a pointer into device memory
 *
 * Return Value:
 *   region -- one of the region constants that define the region
 *
 ******************************************************************************/
STATIC int get_region (fslib_device_t *dev, fslib_ptr ptr)
{
    int region;
    fslib_device_info_block_t *dib;

    dib = &dev->dib;
    if (ptr < sizeof(fslib_device_info_block_t)) {
        region = FSLIB_REGION_DIB;
    } else if (   (ptr >= dib->file_system_offset)
               && (ptr <  dib->file_system_offset + dib->file_system_length)) {
        region = FSLIB_REGION_FILE_SYSTEM;
    } else if (   (ptr >= dib->bad_sector_map_offset)
               && (ptr <  dib->bad_sector_map_offset + dib->bad_sector_map_length)) {
        region = FSLIB_REGION_BAD_SECTOR_MAP;
    } else if (   (ptr >= dib->squeeze_log_offset)
               && (ptr <  dib->squeeze_log_offset + dib->squeeze_log_length)) {
        region = FSLIB_REGION_SQUEEZE_LOG;
    } else if (   (ptr >= dib->squeeze_buffer_offset)
               && (ptr <  dib->squeeze_buffer_offset + dib->squeeze_buffer_length)) {
        region = FSLIB_REGION_SQUEEZE_BUFFER;
    } else if (   (ptr >= dib->monlib_offset)
               && (ptr <  dib->monlib_offset + dib->monlib_length)) {
        region = FSLIB_REGION_MONLIB;
    } else {
        region = FSLIB_REGION_UNKNOWN;
    }

    PRT(P5,("get_region(%lx,%lx) = %d\n",dev,ptr,region));

    return(region);
}


/******************************************************************************
 * Name: get_end_of_region
 *
 * Description: given an fslib_ptr, returns an fslib_ptr to the end of the region
 *   The device is devided up into several regions, and a read/write/seek is
 *   not allowed to cross that boundary.
 *
 * Input: 
 *   dev -- a pointer to the device structure for the device
 *   ptr -- a pointer into device memory
 *
 * Return Value:
 *   end_of_region -- an fslib_ptr to the last byte in the region
 *
 ******************************************************************************/
STATIC fslib_ptr get_end_of_region (fslib_device_t *dev, fslib_ptr ptr)
{
    fslib_ptr end_of_region;

    PRT(P5E,("get_end_of_region(%lx,%lx)\n",dev,ptr));

    switch(get_region(dev,ptr)) {
	
    case FSLIB_REGION_DIB:
	end_of_region = (fslib_ptr)sizeof(fslib_device_info_block_t) - 1;
	break;
    case FSLIB_REGION_FILE_SYSTEM:
	end_of_region = dev->file_system_end;
	break;
    case FSLIB_REGION_BAD_SECTOR_MAP:
	end_of_region = dev->bad_sector_map_end;
	break;
    case FSLIB_REGION_SQUEEZE_LOG:
	end_of_region = dev->squeeze_log_end;
	break;
    case FSLIB_REGION_SQUEEZE_BUFFER:
	end_of_region = dev->squeeze_buffer_end;
	break;
    case FSLIB_REGION_MONLIB:
	end_of_region = dev->monlib_end;
	break;
    case FSLIB_REGION_UNKNOWN:
	end_of_region = dev->dib.length - 1;
	break;
    default: /* to keep the compiler quiet */
	end_of_region = 0;
	break;
    }

    PRT(P5X,("get_end_of_region(%lx,%lx) = %lx\n",dev,ptr,end_of_region));

    return(end_of_region);
}


/******************************************************************************
 * Name: good_sector
 *
 * Description: determines whether a given sector is good or not
 *
 * Input: 
 *   dev -- pointer to the device structure for the device
 *   sector_num -- the sector number of the sector to check
 *
 * Return Value:
 *   sector status = 
 *       FSLIB_SECTOR_GOOD   -- sector is good
 *       FSLIB_SECTOR_SPARED -- sector is marked bad, but there is a spare sector
 *                              that is being used instead
 *       FSLIB_SECTOR_BAD    -- sector is marked bad, and is not spared
 *
 ******************************************************************************/
STATIC int good_sector (fslib_device_t *dev, ulong sector_num)
{
    int rc;
    int actual;
    int i;
    uchar sector_byte;
    uchar sector_bit;
    ulong sector_byte_addr;
    ulong num_spare_sectors;
    
    PRT(P5E,("good_sector(%lx,%d)\n",dev,sector_num));

    sector_byte_addr = dev->dib.bad_sector_map_offset + (sector_num / 8);
    actual = FSLIB_ralib->read(dev->dev_num,sector_byte_addr,&sector_byte,1);
    if (actual != 1) {
	rc = FSLIB_SECTOR_BAD;
    } else {
	sector_byte ^= (uchar)dev->dib.erased_state;
	sector_bit = 1 << (sector_num % 8);
	if ((sector_byte & sector_bit) == 0) {
	    rc = FSLIB_SECTOR_GOOD;
	} else {
	    /*
	     * If the sector is marked bad, we should search the spare 
	     * sector list in the DIB to see if this sector has been spared
	     */
	    rc = FSLIB_SECTOR_BAD;
	    if (dev->dib.num_spare_sectors > FSLIB_SPARE_SECTOR_MAX) {
		num_spare_sectors = 0;
	    } else {
		num_spare_sectors = dev->dib.num_spare_sectors;
	    }
	    for (i = 0; i < num_spare_sectors; i++) {
		if (dev->dib.spare[i].bad_sector_num == sector_num) {
		    rc = FSLIB_SECTOR_SPARED;
		    break;
		}
	    }
	}
    }

    PRT(P5X,("good_sector(%lx,%d) = %d\n",dev,sector_num,rc));
    return(rc);
}


/******************************************************************************
 * Name: convert_file_hdr_to_dirent
 *
 * Description: Converts a file header to a directory entry (dirent_t).  
 *   The file header checked for validity, and then, if it is valid, it is 
 *   converted.
 *
 * Input: 
 *   fh -- pointer to the file header to be converted
 *   de -- pointer to a buffer inwhich to create the directory entry
 *
 * Return Value: None
 *
 ******************************************************************************/
STATIC void convert_file_hdr_to_dirent (fslib_file_hdr_t *fh, dirent_t *de)
{
    
    de->d_off     = fh->next;
    de->d_fileno  = fh->index;
    de->d_reclen  = sizeof(dirent_t);
    strncpy(de->d_name,fh->file_name,sizeof(de->d_name));
    de->d_namlen  = strlen(de->d_name);
    
    de->size      = fh->length;
    de->type      = fh->type;
    de->valid     = (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID);
    de->deleted   = (fh->deleted != fh->undeleted);
    de->time      = fh->time;
    de->crc       = fh->crc;

    de->filetype = MONLIB_TYPE_FS;  /* added for ROMMON to distinguish */
				    /*   between simple fs and new fs */
}


/******************************************************************************
 * Name: convert_file_hdr_to_stat
 *
 * Description: Converts a file header to a file status structure (stat_t).  
 *
 * Input: 
 *   fh -- pointer to the file header to be converted
 *   status -- pointer to a file status buffer (stat_t)
 *
 * Return Value: None
 *
 ******************************************************************************/
STATIC void convert_file_hdr_to_stat (fslib_file_hdr_t *fh, stat_t *status)
{
    
    /* standard Unix fields */
    status->st_ino = fh->index;
    status->st_mode = 0;
    status->st_nlink = 1;
    status->st_uid = 0;
    status->st_gid = 0;
    status->st_size = fh->length;
    status->st_atime = fh->time;
    status->st_mtime = fh->time;
    status->st_ctime = fh->time;
    status->st_blksize = 0;
    status->st_blocks = 0;

    /* special cisco fields */
    status->type    = fh->type;
    status->flags   = fh->file_flags;
    status->crc     = fh->crc;
    status->valid   = (fh->hdr_flags == FSLIB_HDR_FLAGS_VALID);
    status->deleted = (fh->deleted != fh->undeleted);
    status->namlen  = strlen(fh->file_name);
    strncpy(status->name, fh->file_name, sizeof(status->name));
}


/******************************************************************************
 * Name: get_sector_num
 *
 * Description: returns the sector number for the address in the device
 *   specified.
 *
 * Input: 
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   ptr -- address to be converted to sector number
 *
 * Return Value: 
 *   ulong -- sector number.
 *
 ******************************************************************************/
STATIC ulong get_sector_num (fslib_device_t *dev, fslib_ptr ptr)
{
    ulong sector_num;
    
    sector_num = ptr / dev->dib.sector_size;
    
    PRT(P5,("get_sector_num(%lx,%lx) = %d\n",dev,ptr,sector_num));
    
    return(sector_num);
}

/******************************************************************************
 * Name: get_sector_ptr
 *
 * Description: returns the address of the start of the sector 
 *
 * Input: 
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   sector_num -- sector number to be converted to an address
 *
 * Return Value: 
 *   fslib_ptr -- sector address
 *
 ******************************************************************************/
STATIC fslib_ptr get_sector_ptr (fslib_device_t *dev, ulong sector_num)
{
    fslib_ptr sector_ptr;
    
    sector_ptr = sector_num * dev->dib.sector_size;
    
    PRT(P5,("get_sector_ptr(%lx,%d) = %lx\n",dev,sector_num,sector_ptr));
    
    return(sector_ptr);
}

/******************************************************************************
 * Name: get_bytes_left_this_sector
 *
 * Description: returns the number of bytes left in this sector
 *
 * Input: 
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   ptr -- address to be converted to sector number
 *
 * Return Value: Sector number.
 *
 ******************************************************************************/
STATIC ulong get_bytes_left_this_sector (fslib_device_t *dev, fslib_ptr ptr)
{
    ulong bytes_left;
    ulong sector_size;
    
    sector_size = dev->dib.sector_size;
    bytes_left  = sector_size - (ptr % sector_size);
    
    PRT(P5,("get_bytes_left_this_sector(%lx,%lx) = %d\n",dev,ptr,bytes_left));
    
    return(bytes_left);
}


/******************************************************************************
 * Name: get_bytes
 *
 * Description: returns the number of good bytes from start to end
 *
 * Input: 
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   start_ptr -- starting pointer in device memory
 *   end_ptr -- ending pointer in device memory
 *
 * Return Value:
 *   byte_count -- number of good bytes (i.e. bytes in good sectors)
 *     from start_ptr to end_ptr
 *
 ******************************************************************************/
STATIC ulong get_bytes (fslib_device_t *dev, fslib_ptr start_ptr, fslib_ptr end_ptr)
{
    int byte_count;
    int start_sector;
    int end_sector;
    int sector;

    byte_count = 0;
    if (start_ptr < end_ptr) {
	start_sector = get_sector_num(dev,start_ptr);
	end_sector = get_sector_num(dev,end_ptr);
	if (good_sector(dev,start_sector) != FSLIB_SECTOR_BAD) {
	    byte_count = get_bytes_left_this_sector(dev,start_ptr);
	}
	for (sector = start_sector; sector < end_sector; sector++) {
	    if (good_sector(dev,sector) != FSLIB_SECTOR_BAD) {
		byte_count += dev->dib.sector_size;
	    }
	}
	if (good_sector(dev,end_sector) != FSLIB_SECTOR_BAD) {
	    byte_count -= get_bytes_left_this_sector(dev,end_ptr);
	} else {
	    byte_count -= dev->dib.sector_size;
	}
    }
    
    PRT(P5,("get_bytes(%s,%lx,%lx) = %d\n",dev->dev_id,start_ptr,end_ptr,byte_count));
    
    return(byte_count);
}


/******************************************************************************
 * Name: check_crc
 *
 * Description: checks the crc for a file, given a ptr to the file descriptor
 *
 * Input: 
 *   fp -- pointer to the FSLIB_FILE for the file to check
 *
 * Return Value:
 *   return code: FSLIB_OK if CRC is OK (matches header CRC), otherwise an error code
 *
 ******************************************************************************/
STATIC fslib_error check_crc (fslib_file_rec_t *fp)
{
    ulong crc;
    fslib_error error;
    
    if (fp->flag & FSLIB_NO_CRC)
	return FSLIB_OK;

    crc = calc_crc(fp,&error);
    PRT(P1E,("check_crc(%lx)",fp));
    if (crc == fp->hdr.crc) {
	PRT(P1X,("OK (crc=%lx)\n",crc));
	return(FSLIB_OK);
    } else {
	PRT(P1X,("FAILED (hdr.crc=%lx,crc=%lx)\n",fp->hdr.crc,crc));
	PRT(PERR,("CRC FAILURE (hdr.crc=%lx,crc=%lx)\n",fp->hdr.crc,crc));
	return(error);
    }
    
}


/******************************************************************************
 * Name: calc_crc
 *
 * Description: calculates the crc for a file, given a ptr to the file descriptor
 *
 * Input: 
 *   fp -- pointer to the FSLIB_FILE for the file for which to calculate
 *     the crc.
 *   error -- a pointer to the error variable to set if there is an error
 *
 * Return Value:
 *   crc -- the crc calculated.  If there was an error, the crc is 0
 *     (this is an invalid crc) and the error variable is set
 *
 ******************************************************************************/
STATIC ulong calc_crc (fslib_file_rec_t *fp, fslib_error *error)
{
    ulong crc;
    static ulong buffer[256 * 16];
    ulong length;
    fslib_ptr file_ptr;
    ulong actual;
    ulong read_length;
    int   error_flag;
    int   i;
    int   status_length;
    
    PRT(P1E,("calc_crc(%lx)\n",fp));

    crc = 0;
    error_flag = FALSE;

    /*
     * start from the beginning of the file.
     */
    status_length = 0;
    length = fp->hdr.length;
    file_ptr = fp->start_phys_ptr;
    
    /*
     * while there is more data in the file, read the data, accumulate the CRC 
     */
    while (length > 0) {
	/*
	 * read as much as fits in the buffer now
	 * if there is less than a buffer's worth of data to read, we pad
	 * the last 32-bit word with 0's so the 32-bit checksum is calculated
	 * using the 0's in the odd bytes (if the file is not an even
	 * multiple of 32-bit words long)
	 */
	if (length < sizeof(buffer)) {
	    read_length = length;
	    buffer[length >> 2] = 0;
	} else {
	    read_length = sizeof(buffer);
	}
	
	/*
	 * read from the device (note that the file_ptr will be updated
	 * by the dev_read() function itself)
	 */
	actual = dev_read(fp->dev,file_ptr,(char *)buffer,read_length,&file_ptr);
	
	/*
	 * if the actual did not match the read_length, there was some sort
	 * of an error, so abort now and return a 0 CRC. (0 is invalid)
	 */
	if (actual != read_length) {
	    PRT(PERR,("calc_crc(%lx) ERROR: actual(%d) != read_length(%d)\n",
		    fp,actual,read_length));
	    error_flag = TRUE;
	    if (actual > 0)
		*error = FSLIB_ERROR_READING_FILE_FOR_CRC;
	    else
		*error = actual;
	    
	    break;
	}
	
	/*
	 * accumulate the CRC on the bytes read in the buffer
	 * The algorithm is to rotate the current sum and then add the 
	 * next 32-bit word (one's complement).  This is slightly better
	 * than a simple one's complement sum.
         * The length includes the entire 32-bit word that contains
	 * the last 1,2,3 or 4 bytes.  The word has been zeroed if it
         * is only going to contain 1,2 or 3 bytes so the CRC will be
         * computed on the trailing zeroes too.
	 */
	for (i = 0; i < ((read_length + (sizeof(long) - 1))/sizeof(ulong)); i++) {
	    /* 
	     * rotate the crc
	     */
	    if (crc & 0x80000000) {
		crc <<= 1;
		crc++;
	    } else {
		crc <<= 1;
	    }
	    /*
	     * add in the next 32-bit word
	     */
	    crc += buffer[i];
	    if (crc < buffer[i]) {
		crc++;
	    }
	}
	
	/*
	 * keep counting down the length until the whole file has been CRC'ed
	 */
	length -= read_length;
	status_length += read_length;
	if (status_length >= FSLIB_CALC_CRC_STATUS_LENGTH_LIMIT) {
	    if ((fp->flag & FSLIB_NO_CRC) == 0)
		print_status_update("C", fp->flag & FSLIB_NO_VERBOSE );		
	    status_length = 0;
	}
    }
    
    /*
     * If there was an error, return a 0 CRC, otherwise we don't allow
     * crc's of 0 or 0xffffffff -- we map them to other numbers
     */
    if (error_flag == TRUE) {
	crc = 0;
    } else {
	if (crc == 0)
	    crc = 1;
	else if (crc == 0xffffffff)
	    crc = 0xfffffffe; 
    }

    PRT(P1X,("calc_crc(%lx) = %lx(%d)\n",fp,crc,*error));

    return(crc);
}


/******************************************************************************
 * Name: get_file_rec
 *
 * Description: finds a fslib_file_rec_t for an open file given the file descriptor
 *   It will also return file descriptor to files that have been put to the
 *   error state because of a device being removed or having some sort of
 *   error. 
 *
 * Input:
 *   fd -- the file descriptor of the file to find
 *
 * Return Value:
 *   fp -- a pointer to the fslib_file_rec_t structure for the open file.
 *
 ******************************************************************************/
STATIC fslib_file_rec_t *get_file_rec (int fd)
{
    int i;
    
    PRT(P3E,("get_file_rec(%d) -- ",fd));

    for (i = 0; i < FSLIB_MAX_FILE_REC; i++) {
        if (  (  (FSLIB_file_rec[i].state == FSLIB_FILE_STATE_OPEN)
	       || (FSLIB_file_rec[i].state == FSLIB_FILE_STATE_ERROR)  )
	    && (FSLIB_file_rec[i].fd == fd)  ) {
	    PRT(P3X,("found (%d) (%lx)\n",i,&FSLIB_file_rec[i]));

            return(&FSLIB_file_rec[i]);
	}
    }

    PRT(P3X,("not found\n"));
    
    return(NULL);
}


/******************************************************************************
 * Name: get_new_file_rec
 *
 * Description: gets an unused fslib_file_rec_t, and gets a new file descriptor
 *
 * Input: None
 *
 * Return Value:
 *   fp -- a pointer to the new fslib_file_rec_t structure or NULL if none available
 *
 ******************************************************************************/
STATIC fslib_file_rec_t *get_new_file_rec (void)
{
    int i;
    fslib_file_rec_t *fp;

    PRT(P3E,("get_new_file_rec()\n"));

    /*
     * Use a global semaphore to ensure that the available file record is
     * only obtained by one user, and that the file descriptor is unique.
     */
    FSLIB_file_semaphore++;
    while (FSLIB_file_semaphore > 1) {
	FSLIB_file_semaphore--;
	FSLIB_ralib->yield(1);
	FSLIB_file_semaphore++;
    }

    /*
     * look for a file record that is available (state == CLOSED)
     * or is in the error state (can be used if nothing else available)
     */
    fp = NULL;
    for (i = 0; i < FSLIB_MAX_FILE_REC; i++) {
        if (FSLIB_file_rec[i].state == FSLIB_FILE_STATE_ERROR) {
	    fp = &FSLIB_file_rec[i];
	} else if (FSLIB_file_rec[i].state == FSLIB_FILE_STATE_CLOSED) {
	    fp = &FSLIB_file_rec[i];
	    break;
	}
    }

    /*
     * if we found a record to use, change the state to opening so no
     * one else takes the file record, and get file descriptor 
     */
    if (fp != NULL) {
	fp->state = FSLIB_FILE_STATE_OPENING;
	FSLIB_file_descriptor++;
	if (FSLIB_file_descriptor <= 0)
	    FSLIB_file_descriptor = 1;
	fp->fd = FSLIB_file_descriptor;
    }

    /* Release the global file record and file descriptor semaphore */

    FSLIB_file_semaphore--;

    PRT(P3X,("get_new_file_rec() = %lx\n",fp));

    return(fp);
}


/******************************************************************************
 * Name: convert_ralib_error_code
 *
 * Description: converts an RALIB error code to a FSLIB (or FS) error code
 *   This function converts the output of the RALIB functions to appropriate
 *   FSLIB error codes.  All RALIB error codes are negative, so if the input
 *   value is positive, it is assumed to NOT be an error code, and is just
 *   returned as is.  This is useful in the case of ralib_read and ralib_write
 *   when the actual number of bytes read or written is returned when the 
 *   function completes successfully.  This function should NOT be used to 
 *   convert the return value of ralib_device_init().
 *
 * Input:
 *   ralib_error_code -- an RALIB error code returned from an RALIB function. 
 *     If this number is positive, it is assumed to NOT be an error code, and
 *     the number is just returned. 
 *
 * Return Value:
 *   fslib_error_code -- the FSLIB error code that corresponds to the RALIB 
 *     error code that was input
 *
 ******************************************************************************/
int convert_ralib_error_code (int ralib_error_code)
{
    fslib_error fslib_error_code;
    
    if (ralib_error_code >= 0)
	fslib_error_code = ralib_error_code;
    else {
	switch (ralib_error_code) {
	case RALIB_OK:                      fslib_error_code = FSLIB_OK; break;
	case RALIB_ERROR_INVALID_DEVICE_ID: fslib_error_code = FSLIB_ERROR_INVALID_DEVICE_ID; break;
	case RALIB_ERROR_INVALID_FLASH_PTR: fslib_error_code = FSLIB_ERROR_RALIB_FLASH_PTR; break;
	case RALIB_ERROR_INVALID_DEV_NUM:   fslib_error_code = FSLIB_ERROR_RALIB_DEV_NUM; break;
	case RALIB_ERROR_INVALID_MODE:      fslib_error_code = FSLIB_ERROR_RALIB_MODE; break;
	case RALIB_ERROR_NO_DEVICE:         fslib_error_code = FSLIB_ERROR_NO_DEVICE; break;
	case RALIB_ERROR_DEVICE_NEEDS_INIT: fslib_error_code = FSLIB_ERROR_NO_DEVICE; break;
	case RALIB_ERROR_DEVICE_NOT_WRITABLE: fslib_error_code = FSLIB_ERROR_DEVICE_IS_READ_ONLY; break;
	case RALIB_ERROR_DEVICE_NOT_READY:  fslib_error_code = FSLIB_ERROR_DEVICE_NOT_READY; break;
        default:                            fslib_error_code = FSLIB_ERROR_RALIB_UNKNOWN; break;
	}
    }

    PRT(P3,("convert_ralib_error_code(%d) = %d\n",ralib_error_code,fslib_error_code));
    return(fslib_error_code);
}

/******************************************************************************
 * Name: print_squeeze_status_update
 *
 * Description: Prints a status update message to the console, 
 *              overwriting the previous status message.
 *
 * Input parameters:
 *   status -- a pointer to the string to print to the console
 *   quiet  -- if nonzero, do not print the status
 *   longest_sqeeze_status - length of longest string printed on previous invokations
 *   psector -- flag, non-zero print out sector field
 *   sector  -- sector value, if psector non-zero
 * Return Value: None
 *
 ******************************************************************************/

#define ADJUST_END_OF_STRING_OFFSET 4

STATIC void print_squeeze_status_update (const char *status, uint quiet,
					 int *longest_squeeze_status,char psector,int sector)
{
    /* 
     * length longest squeeze status string printed since boot,
     * across all vtys.
     */

   
    int status_string_len = strlen(status);
     
    if (fslib_print_enable == TRUE) {
	if (!quiet) {
            int i;
            /* 
             * move cursor to the beginning of the line, and
             * print status strings
             */


            for (i = 0; i < *longest_squeeze_status; i++)
              printf("%c",CH_BS);

            if (psector)
	    {
	      printf("%s: %d",status,sector);
	      status_string_len += ADJUST_END_OF_STRING_OFFSET;
            }
	    else
	    {
              printf("%s",status);
	    }
	    if (status_string_len < *longest_squeeze_status) { 

                /* print spaces over the tail end of last printed status */
		for (i = status_string_len; i <= *longest_squeeze_status; i++)
		    printf(" ");
	        /* Backspace to last printable character in present string */	
                for (i = status_string_len; i <= *longest_squeeze_status; i++)
                   printf("%c",CH_BS);
            }
	    else
	    {
                *longest_squeeze_status = status_string_len;
	    }
	    flush();



	}
    }
    /*
     * Let other processes have a chance to run
     */
    ralib_yield(0);
}

/******************************************************************************
 * Name: print_status_update
 *
 * Description: Prints a status update message to the console if applicable.
 *
 * Input:
 *   status -- a pointer to the string to print to the console
 *
 * Return Value: None
 *
 ******************************************************************************/
STATIC void print_status_update (const char *status, uint quiet)
{
    if (fslib_print_enable == TRUE) {
	if (!quiet) {
	    printf("%s",status);
	    flush();
	}
    }

    /*
     * Let other processes have a chance to run
     */
    ralib_yield(0);
}


/******************************************************************************
 * Name: check_sector_erase
 *
 * Description: checks to see if the sector indicated is erased
 * 
 * Input:
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   sector_ptr -- a pointer to the start of the sector
 *
 * Return Value:
 *   return code -- FSLIB_OK = sector is all erased, FSLIB_ERROR = error while read
 *     sector, FSLIB_ERROR_SECTOR_NOT_ERASED = sector not completely erased
 *
 ******************************************************************************/
STATIC fslib_error check_sector_erase (fslib_device_t *dev, fslib_ptr sector_ptr)
{
    fslib_error rc;
    static ulong buffer[256 * 16];
    fslib_ptr s_ptr;
    int actual;
    int length;
    int sector_size;
    int i;

    PRT(P3E,("check_sector_erase(%s,%lx)\n",dev->dev_id,sector_ptr));

    s_ptr = get_sector_ptr(dev,get_sector_num(dev,sector_ptr));

    length = 0;
    rc = FSLIB_OK;
    sector_size = dev->dib.sector_size;
    while ((length < sector_size) && (rc == FSLIB_OK)) {
	/* 
	 * read the sector a buffer-worth at a time
	 */
	actual = dev_read(dev,s_ptr,(char *)buffer,sizeof(buffer),NULL);
	if (actual != sizeof(buffer)) {
	    if (actual < 0) {
		rc = actual;
	    } else {
		rc = FSLIB_ERROR_READING_FOR_ERASE_CHECK;
	    }
	} else {
	    /* 
	     * we read the sector ok, so check the buffer to see if it is all
	     * erased 
	     */
	    for (i = 0; i < (sizeof(buffer)/sizeof(ulong)); i++) {
		if (buffer[i] != dev->dib.erased_state) {
		    rc = FSLIB_ERROR_SECTOR_NOT_ERASED;
		}
	    }
	    /*
	     * adjust the pointers and counts to move through the sector
	     */
	    s_ptr += sizeof(buffer);
	    length += sizeof(buffer);
	}
    }

    PRT(P3X,("check_sector_erase(%s,%lx) = %d\n",dev->dev_id,sector_ptr,rc));
    return(rc);
}


/******************************************************************************
 * Name: erase_sector
 *
 * Description: erases the sector if it needs it 
 * 
 * Input:
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   sector_ptr -- a pointer to the start of the sector
 *
 * Return Value:
 *   return code -- FSLIB_OK = sector is all erased, FSLIB_ERROR = error while read
 *     sector, FSLIB_ERROR_SECTOR_NOT_ERASED = sector not completely erased
 *
 ******************************************************************************/
STATIC fslib_error erase_sector (fslib_device_t *dev, fslib_ptr sector_ptr)
{
    fslib_error      rc;
    fslib_ptr        s_ptr;
    int              i;
    int              this_sector;
    fslib_erase_fn_t erase_fn;

    PRT(P9E,("erase_sector(%s,%lx)\n",dev->dev_id,sector_ptr));

    /*
     * Just before we actually erase the device, we need to check if this
     * sector has been spared, and adjust the address to the spare sector 
     * if necessary 
     */
    erase_fn = dev->erase_fn;
    if (erase_fn == NULL) {
	rc = FSLIB_ERROR_NO_ERASE_FUNCTION;
    } else {
	this_sector = get_sector_num(dev,sector_ptr);
	for (i = 0; i < dev->dib.num_spare_sectors; i++) {
	    if (this_sector == dev->dib.spare[i].bad_sector_num) {
		this_sector = dev->dib.spare[i].spare_sector_num;
		break;
	    }
	}
	/*
	 * Now get the pointer to the sector and call the erase function
	 */
	s_ptr = get_sector_ptr(dev,this_sector);
	rc = check_sector_erase(dev,s_ptr);
	if (rc == FSLIB_ERROR_SECTOR_NOT_ERASED) {
	    rc = erase_fn(dev->dev_num,s_ptr,dev->dib.sector_size);
	    if (rc == FSLIB_OK) {
		/*
		 * Now check to see if the sector has been erased completely.  If the
		 * erase was unsuccessful, try to find a spare sector to use instead.
		 */
		rc = check_sector_erase(dev,s_ptr);
		if (rc == FSLIB_ERROR_SECTOR_NOT_ERASED) {
		    rc = switch_to_spare_sector(dev,s_ptr);
		}
	    }
	}
    }

    PRT(P9X,("erase_sector(%s,%lx) = %d\n",dev->dev_id,sector_ptr,rc));
    return(rc);
}


/******************************************************************************
 * Name: switch_to_spare_sector
 *
 * Description: switches to an alternate sector if one is available
 * 
 * Input:
 *   dev -- pointer to the fslib_device_t entry for the device being used
 *   sector_ptr -- a pointer to the start of the sector
 *
 * Return Value:
 *   return code -- FSLIB_OK = success, spare sector found, otherwise and error
 *       code is returned
 *
 ******************************************************************************/
STATIC fslib_error switch_to_spare_sector (fslib_device_t *dev, fslib_ptr sector_ptr)
{
    fslib_error rc;
    int      i;
    int      actual;
    fslib_ptr   write_ptr;
    int      write_len;
    ushort   unerased_state;
    fslib_ptr   sector_byte_ptr;
    uchar    sector_byte;
    uchar    sector_bit;
    int      bad_sector_num;
    int      phys_bad_sector_num;

    PRT(P5E,("switch_to_spare_sector(%s,%lx)\n",dev->dev_id,sector_ptr));

    rc = FSLIB_OK;

    /*
     * See if the sector is presently being spared, and if it is change that
     * spare entry to the *UNERASED* state of flash (to indicate that it is no
     * longer available).  This would happen if a spare sector failed.
     * Remember to update both the DIB on the device and the DIB in the dev
     * structure. 
     */
    bad_sector_num = get_sector_num(dev,sector_ptr);
    phys_bad_sector_num = bad_sector_num;
    for (i = 0; i < dev->dib.num_spare_sectors; i++) {
	if (bad_sector_num == dev->dib.spare[i].bad_sector_num) {
	    phys_bad_sector_num = dev->dib.spare[i].spare_sector_num;
	    unerased_state = (ushort)~dev->dib.erased_state;
	    write_ptr = (fslib_ptr)((char *)&dev->dib.spare[i].bad_sector_num - (char *)&dev->dib);
	    write_len = sizeof(dev->dib.spare[i].bad_sector_num);
	    actual = dev_write(dev,(char *)&unerased_state,write_ptr,write_len,NULL);
	    if (actual != write_len) {
		rc = (actual < 0) ? actual : FSLIB_ERROR_REMOVING_SPARE_SECTOR;
		break;
	    }
	    dev->dib.spare[i].bad_sector_num = unerased_state;
	}
    }

    /*
     * See if there are any spare sectors to use, if so,
     * 1. write the bad sector number to the DIB
     * 2. update the DIB in the dev structure
     * 3. read the bad sector map
     * 4. change the bit in the bad sector map to mark the sector bad
     * 5. write the value back to the bad sector map
     */
    if (rc == FSLIB_OK) {
	rc = FSLIB_ERROR;
	for (i = 0; i < dev->dib.num_spare_sectors; i++) {
	    if (dev->dib.spare[i].bad_sector_num == (ushort)dev->dib.erased_state) {
		/*
		 * write the bad sector number to the spare section of the DIB
		 */
		write_ptr = (fslib_ptr)((char *)&dev->dib.spare[i].bad_sector_num - (char *)&dev->dib);
		write_len = sizeof(dev->dib.spare[i].bad_sector_num);
		actual = dev_write(dev,(char *)&bad_sector_num,write_ptr,write_len,NULL);
		if (actual != write_len) {
		    rc = (actual < 0) ? actual : FSLIB_ERROR_SETTING_SPARE_SECTOR;
		} else {
		    /*
		     * update the DIB in the dev structure
		     */
		    dev->dib.spare[i].bad_sector_num = bad_sector_num;
		    /*
		     * read the bad sector map
		     */
		    sector_byte_ptr = dev->dib.bad_sector_map_offset + (phys_bad_sector_num / 8);
		    actual = dev_read(dev,sector_byte_ptr,(char *)&sector_byte,1,NULL);
		    if (actual != 1) {
			rc = (actual < 0) ? actual : FSLIB_ERROR_READING_BAD_SECTOR_MAP;
		    } else {
			/*
			 * change the bit in the bad sector map
			 */
			sector_byte ^= (uchar)dev->dib.erased_state;
			sector_bit = 1 << (phys_bad_sector_num % 8);
			sector_byte |= sector_bit;
			sector_byte ^= (uchar)dev->dib.erased_state;
			/*
			 * write the value back to the bad sector map
			 */
			actual = dev_write(dev,(char *)&sector_byte,sector_byte_ptr,1,NULL);
			if (actual != 1) {
			    rc = (actual < 0) ? actual : FSLIB_ERROR_WRITING_BAD_SECTOR_MAP;
			} else {
			    rc = FSLIB_OK;
			}
		    }
		}
		break;
	    }
	}
    }

    PRT(P5X,("switch_to_spare_sector(%s,%lx) = %d (new sector=%d)\n",dev->dev_id,sector_ptr,rc));
    return(rc);
}

	
/******************************************************************************
 * Name: FSLIB_build_dib
 *
 * Description: builds a DIB based on the paramters passed.  This
 *   function builds the DIB in a RAM buffer -- NOT IN FLASH.  The
 *   caller must copy the DIB to the device.
 *
 * Input:
 *   dib -- pointer to an area in which to build the DIB (static or
 *      malloc'ed RAM buffer).  This is NOT a pointer into the device
 *      where the DIB should go.
 *   sector_size -- the size (in bytes) of one sector on the flash
 *     card.  This is not necessarily the same as the sector size of
 *     the flash chips in the card.  For example, Intel Series 2+
 *     flash cards use chips that have 64K sector size, but the card
 *     is built 2 chips wide, so the effective sector size is 128K.
 *   num_sectors -- total number of sectors on the device that are to
 *     be used for the file system.  Normally this is all the sectors
 *     on the device (includes DIB sector, spare sectors, squeeze
 *     buffer sector, squeeze log sector, etc. -- the whole enchilada).
 *   num_spare_sectors -- the number of sectors to reserve for use as
 *     spare sectors.
 *   monlib_length -- the amount of room to reserve for the MONLIB (in
 *     bytes).
 *   prog_algorithm -- the programming algorithm for the device.
 *   erased_state -- the erased state of the device.  This should be
 *     either 0 or 0xffffffff.  If it is not 0 it will be set to
 *     0xffffffff.
 *   volume_id -- a pointer to the volume ID to put in the DIB.  It is
 *     a pointer to a null-terminated character string that is up to
 *     32 characters total (31 plus a null).
 *
 * Output:
 *   The Device Info Block pointed to by the dib parameter will be
 *     filled out completely.  If there is an error in the parameters
 *     given to this function, parts of the DIB may be written before
 *     the error is discovered.  In any case, if the return code
 *     indicates an error, this DIB should be considered invalid.
 *
 * Return Value:
 *   boolean return code: TRUE = DIB built successfully, FALSE = some
 *     error or inconsistency detected -- DIB is invalid.
 * 
 *******************************************************************************/
int FSLIB_build_dib(fslib_device_info_block_t *dib,
		      ulong sector_size,
		      ulong num_sectors,
		      ulong num_spare_sectors,
		      ulong monlib_length,
		      ulong prog_algorithm,
		      ulong erased_state,
		      const char *volume_id) {

    ulong bad_sector_map_length;
    ulong dib_monlib_length;
    ulong squeeze_log_length;
    ulong num_dib_monlib_sectors;
    ulong num_squeeze_log_sectors;
    ulong total_overhead_sectors;
    ulong spare_sector_num;
    int i;

    /*
     * let's start counting up sectors to figure out if we have
     * enough, or if the caller is crazy.
     *
     * - first section contains the monlib, dib and bad sector map 
     * - number of spare sectors is a parameter to this function
     * - squeeze log big enough for 2 squeeze log records for each sector,
     *   then rounded up to an even sector boundary 
     * - the squeeze buffer is always 1 sector
     */
    bad_sector_map_length = (num_sectors + 7) / 8;
    dib_monlib_length = sizeof(fslib_device_info_block_t) + monlib_length + bad_sector_map_length;
    num_dib_monlib_sectors = (dib_monlib_length / sector_size) + 1;
    squeeze_log_length = sizeof(fslib_squeeze_log_rec_t) * 2 * num_sectors;
    num_squeeze_log_sectors = (squeeze_log_length / sector_size) + 1;
    total_overhead_sectors = num_dib_monlib_sectors + num_spare_sectors + num_squeeze_log_sectors + 1;
    /*
     * now do the check.  We must have at least one sector left over
     * for the file system after all the overhead has been allocated
     */
    if (total_overhead_sectors >= num_sectors) {
	return(FALSE);
    } else {
	/*
	 * things look generally OK.  start filling out the DIB
	 */
	dib->magic = FSLIB_DEVICE_INFO_BLOCK_MAGIC;
	dib->file_system_version = 0x00010000;
	dib->prog_algorithm = prog_algorithm;
	dib->erased_state = erased_state ? 0xffffffff : 0;
	dib->sector_size = sector_size;
	dib->length = sector_size * num_sectors;
	dib->monlib_offset = sizeof(fslib_device_info_block_t);
	dib->monlib_length = monlib_length;
	dib->bad_sector_map_length = bad_sector_map_length;
	dib->bad_sector_map_offset = (num_dib_monlib_sectors * sector_size) - bad_sector_map_length;

	/*
	 * fill out the spare sector map in the DIB
	 * the spare sectors come after the file system and before the
         * squeeze log and squeeze buffer
         */
	dib->num_spare_sectors = num_spare_sectors;
	spare_sector_num = (num_sectors - (num_squeeze_log_sectors + 1)) - num_spare_sectors;
	for (i = 0; i < num_spare_sectors; i++) {
	    dib->spare[i].spare_sector_num = spare_sector_num;
	    spare_sector_num++;
	}
	dib->squeeze_buffer_length = sector_size;
	dib->squeeze_buffer_offset = (num_sectors - 1) * sector_size;
	dib->squeeze_log_length = num_squeeze_log_sectors * sector_size;
	dib->squeeze_log_offset = (num_sectors - (num_squeeze_log_sectors + 1)) * sector_size;
	dib->file_system_offset = num_dib_monlib_sectors * sector_size;
	dib->file_system_length = (num_sectors - total_overhead_sectors) * sector_size;
	strncpy(dib->volume_id,volume_id,FSLIB_VOLUME_ID_MAX);
	for (i = 0; i < 22; i++) {
	    dib->unused[i] = dib->erased_state;
	}
	return(TRUE);
    }
}


/*
 * Return TRUE if device containting the indicated
 * file is currently being formated
 */
STATIC boolean device_being_format (int fd)
{
    fslib_file_rec_t *fp;
    fslib_device_t *dev;

    fp = get_file_rec(fd);
    if (fp && fp->dev) {
	dev = fp->dev;
	if (dev->format_semaphore)
	    return TRUE;
    }
    return FALSE;
}

#if !defined(FSLIB)

/*
 * Read device info block into block memory and return
 * pointer to it.
 */
STATIC int get_device_info_block (const char *dev_id, fslib_device_info_block_t *dibp)
{
    int devnum;
    char *bufp;
    int bufsize;
    int value;

    devnum = FSLIB_ralib->get_dev_num(dev_id);
    if (devnum < 0)
	return convert_ralib_error_code(devnum);

    bufp = (char *)dibp;
    bufsize = sizeof(fslib_device_info_block_t);
    value = FSLIB_ralib->read(devnum, 0, bufp, bufsize);

    if (value != bufsize) {
	if (value < 0)
	    return convert_ralib_error_code(value);
	else
	    return FSLIB_ERROR_READING_DIB;
    }

    valid_dib(dibp, &value);
    return value;
}

/*
 * Copy monlib to a buffer.
 * If ok, return pointer to buffer and size.
 * Otherwise, return NULL.
 */
STATIC char *copy_monlib_to_buffer (const char *monlib, int *size, int *error)
{
    fslib_device_info_block_t dib_rec;
    fslib_device_info_block_t *dibp;
    int bufsize;
    char *bufp;
    const char *monlib_dev;
    const char *monlib_file;
    int devnum;
    int fd;
    int value;
    stat_t status;

    monlib_dev = get_dev_id_by_file_id(monlib);
    if (monlib_dev == NULL) {
	*error = FSLIB_ERROR_INVALID_DEVICE_ID;
	return NULL;
    }

    monlib_file = get_file_name_by_file_id(monlib);
    if (monlib_file == NULL) {
	/*
	 * Get monlib from device's device header area.
	 */
	devnum = FSLIB_ralib->get_dev_num(monlib_dev);
	if (devnum < 0) {
	    *error = convert_ralib_error_code(devnum);
	    return NULL;
	}
	dibp = &dib_rec;
	value = get_device_info_block(monlib_dev, dibp);
	if (value < 0) {
	    *error = value;
	    return NULL;
	}
	/*
	 * We allocate single buffer for monlib hoping
	 * monlib is not huge.
	 */
	bufsize = dibp->monlib_length;
	if (bufsize == 0) {
	    *error = FSLIB_ERROR_NO_MONLIB_ON_DEV;
	    return NULL;
	}
	bufp = malloc(bufsize);
	if (bufp == NULL) {
	    *error = FSLIB_ERROR_NO_MEMORY_FOR_MONLIB_BUF;
	    return NULL;
	}

	/*
	 * Read monlib from device and write it to
	 * the formated device.
	 */
	*size = FSLIB_ralib->read(devnum, dibp->monlib_offset, bufp, bufsize);
	if (*size != bufsize) {
	    if (*size < 0)
		*error = convert_ralib_error_code(*size);
	    else
		*error = FSLIB_ERROR_READING_MONLIB;
	    free(bufp);
	    return NULL;
	}
    } else {
	/*
	 * Copy monlib as a file on device.
	 */
	fd = FSLIB_open(monlib, O_RDONLY, 0);
	if (fd < 0) {
	    *error = -1;	/* need to define error code */
	    return NULL;
	}
	value = FSLIB_fstat(fd, &status);
	if (value < 0) {
	    FSLIB_close(fd);
	    *error = value;
	    return NULL;
	}
	bufsize = status.st_size;
	if (bufsize) {
	    bufp = malloc(bufsize);
	    if (bufp == NULL) {
		FSLIB_close(fd);
		*error = FSLIB_ERROR_NO_MEMORY_FOR_MONLIB_BUF;
		return NULL;
	    }
	    *size = FSLIB_read(fd, bufp, bufsize);
	} else {
	    FSLIB_close(fd);
	    *error = FSLIB_ERROR_MONLIB_SIZE;
	    return NULL;
	}

	if (bufsize != *size) {
	    free(bufp);
	    FSLIB_close(fd);
	    if (*size < 0)
		*error = *size;
	    else
		*error = FSLIB_ERROR_READING_MONLIB;
	    return NULL;
	}
    }
    return bufp;
}

/*
 * Return pointer to the prog table entry with matching
 * algorithm number.
 */
STATIC fslib_prog_rec_t *find_prog_table_entry (int alg_num)
{
    fslib_prog_rec_t *ptblp;

    /*
     * Search FSLIB_prog_table by prog_number to find entry
     * with functions for read, write, erase.
     */
    ptblp = &FSLIB_prog_table[0];
    while (ptblp->algorithm != FSLIB_PROG_LIST_END) {
	if (ptblp->algorithm == alg_num)
	    return ptblp;
	ptblp++;
    }
    return NULL;
}

/*
 * Verify the buffer content match the erase value.
 */
STATIC boolean check_block_erased (char *bufp, int bufsize, uint erase_state)
{
    char erase_value;

    erase_value = erase_state & 0xff;
    while (bufsize--) {
	if (*bufp++ != erase_value)
	    return FALSE;
	if (bufsize && (bufsize & 0xfff) == 0)
	    process_suspend();	/* give other processes a chance to run */
    }
    return TRUE;
}

/*
 * Check if it's ok to have this bad sector 
 */
STATIC boolean check_bad_sector (fslib_device_info_block_t *dibp, uint offset)
{
    if (offset >= dibp->file_system_offset &&
	offset < dibp->file_system_offset + dibp->file_system_length)
    {
	return TRUE;
    }
    return FALSE;
}

STATIC boolean read_flash_to_buf (fslib_read_fn_t freadp, int devnum,
    int offset, char *bufp, int bufsize)
{
    int block_size;
    int size;
    int value;

    block_size = 4*1024;
    while (bufsize) {
	size = (bufsize >= block_size) ? block_size : bufsize;
	value = (*freadp)(devnum, offset, bufp, size);
	/*
	 * Make sure the sector is read correctly.
	 */
	if (value != size)
	    return FALSE;
	offset += size;
	bufp += size;
	bufsize -= size;
	if (bufsize)
	    process_suspend();
    }
    return TRUE;
}

/*
 * Erase all sectors of the device, construct bad
 * sector map along the way, allow bad sectors only
 * outside of device header area.
 */
STATIC char *erase_all_sectors (format_dev_parm_t *fpp, int *error)
{
    fslib_device_info_block_t *dibp;
    fslib_prog_rec_t *ptblp;
    fslib_init_fn_t finitp;
    fslib_erase_fn_t ferasep;
    fslib_read_fn_t freadp;
    int bufsize;
    char *bufp;
    char *smap;
    int offset;
    int sector_num;
    int value;
    int total_sectors;
    char display_buf[64];
    int len;
    boolean retcode;

    dibp = fpp->dibp;
    if (dibp == NULL) {
	*error = FSLIB_ERROR_BAD_DIB;
	return NULL;
    }

    /*
     * Make sure we have read/erase functions available.
     */
    ptblp = (fslib_prog_rec_t *)fpp->ptblp;
    finitp = ptblp->init_fn;
    freadp = ptblp->read_fn;
    ferasep = ptblp->erase_fn;
    if (finitp == NULL || freadp == NULL || ferasep == NULL) {
	*error = FSLIB_ERROR_NO_ERASE_FUNCTION;
	return NULL;
    }

    /*
     * Allocate buffer for read/bad sector map.
     */
    bufsize = fpp->sector_size;
    total_sectors = fpp->device_size / bufsize;
    bufp = malloc(bufsize);
    if (bufp == NULL) {
	*error = FSLIB_ERROR_NO_MEMORY_FOR_FLASH_READ;
	return NULL;
    }

    smap = malloc(dibp->bad_sector_map_length);
    if (smap == NULL) {
	free(bufp);
	*error = FSLIB_ERROR_NO_MEMORY_FOR_BAD_SECTOR_MAP;
	return NULL;
    }
    memset(smap, 0, dibp->bad_sector_map_length);

#if !defined(C7000)     /* access attribute memory is not allowed on 7000 */
    /*
     * Make sure card is not locked.
     */
    (*finitp)(fpp->devnum);
#endif

    /*
     * Erase device one sector at a time.
     */
    display_buf[0] = 0;
    if (fslib_print_enable == TRUE)
    	printf("\n");
    for (offset = (total_sectors-1)*bufsize, sector_num = 0; offset >= 0;
	offset -= bufsize, sector_num++)
    {
	value = (*ferasep)(fpp->devnum, offset, bufsize);
	if (value != FSLIB_OK) {
	    *error = value;
	    free(smap);
	    free(bufp);
	    return NULL;
	}
	/*
	 * Read it back and verify.
	 */
	retcode = read_flash_to_buf(freadp, fpp->devnum,
	    offset, bufp, bufsize);
	if (retcode == FALSE) {
	    *error = FSLIB_ERROR_READING_FOR_ERASE_CHECK;
	    free(smap);
	    free(bufp);
	    return NULL;
	}
	if (check_block_erased(bufp, bufsize, fpp->erase_state) == FALSE) {
	    /*
	     * This is a bad sector.
	     * Check if it's ok to have it.
	     */
	    if (check_bad_sector(dibp, offset) == FALSE) {
		*error = FSLIB_ERROR_BAD_SECTOR_NOT_ALLOWED;
		free(smap);
		free(bufp);
		return NULL;
	    }
	    /*
	     * Record this bad sector in map.
	     */
	    *(smap + (sector_num / 8)) |= 1 << (sector_num % 8);
	}
	/*
	 * Show some information.
	 */
	if (fslib_print_enable == TRUE) {
	    len = strlen(display_buf);
	    while (len--)
		printc(8);
	    sprintf(display_buf, "Formatting sector %d  ",
		total_sectors-sector_num);
	    printf("%s", display_buf);
	    flush();
	}
	process_suspend();
    } /* for */

    free(bufp);
    return smap;
}

/*
 * Copy the monlib from buffer to the device being formated.
 */
STATIC int copy_monlib_to_dev (format_dev_parm_t *fpp, char *bufp, int size)
{
    fslib_device_info_block_t *dibp;
    fslib_prog_rec_t *ptblp;
    fslib_write_fn_t fwritep;
    uint offset;
    int value;

    dibp = fpp->dibp;
    if (dibp == NULL)
	return FSLIB_ERROR_BAD_DIB;

    ptblp = (fslib_prog_rec_t *)fpp->ptblp;
    fwritep = ptblp->write_fn;
    if (fwritep == NULL)
	return FSLIB_ERROR_NO_WRITE_FUNCTION;

    offset = dibp->monlib_offset;
    value = (*fwritep)(fpp->devnum, bufp, offset, size);
    if (value != size) {
	if (value >= 0)
	    value = FSLIB_ERROR_COPY_MONLIB_TO_DEV;
    } else
	value = FSLIB_OK;
    return value;
}

/*
 * Copy bad sector map to device at offset given by dib.
 */
STATIC int copy_bad_sector_map_to_dev (format_dev_parm_t *fpp, char *smap)
{
    fslib_device_info_block_t *dibp;
    fslib_prog_rec_t *ptblp;
    fslib_write_fn_t fwritep;
    char erase_value;
    char *ptr;
    int map_size;
    uint offset;
    int value;

    dibp = fpp->dibp;
    if (dibp == NULL)
	return FSLIB_ERROR_BAD_DIB;

    ptblp = (fslib_prog_rec_t *)fpp->ptblp;
    fwritep = ptblp->write_fn;
    if (fwritep == NULL)
	return FSLIB_ERROR_NO_WRITE_FUNCTION;

    /*
     * Xor bad sector map with erase state
     * to make sure good sector corresponds to erase state
     * (was originally set to 0).
     */
    erase_value = fpp->erase_state & 0xff;
    map_size = dibp->bad_sector_map_length;
    ptr = smap;
    while (map_size--) {
	*ptr ^= erase_value;
	ptr++;
    }

    /*
     * Write bad sector map to the device pointed by dib.
     */
    map_size = dibp->bad_sector_map_length;
    offset = dibp->bad_sector_map_offset;
    value = (*fwritep)(fpp->devnum, smap, offset, map_size);
    if (value != map_size) {
	if (value >= 0)
	    value = FSLIB_ERROR_WRITING_BAD_SECTOR_MAP;
	return value;
    }
    return FSLIB_OK;
}
#endif

STATIC int format_device (const char *dev_id, format_parm_t *fmp)
{
#if !defined(FSLIB)
    char *monlib;
    int monlib_size;
    char *bufp;
    int devnum;
    fslib_device_t *dev;
    int value;
    format_dev_parm_t fp_rec;
    format_dev_parm_t *fpp;
    fslib_device_info_block_t dib_rec;
    fslib_device_info_block_t *dibp;
    fslib_prog_rec_t *ptblp;
    char *smap;
    fslib_write_fn_t fwritep;
    int len;

    /*
     * Copy monlib to buffer.
     */
    monlib = fmp->monlib;
    if (monlib == NULL || *monlib == 0) {
	/*
	 * monlib not specified, use the bundled monlib
	 */
#if defined(C7000)
	bufp = copy_bndl_monlib_to_buffer(&monlib_size);
	if (bufp == NULL) 
	    return FSLIB_ERROR_NO_MEMORY_FOR_MONLIB_BUF;
#else
	monlib_size = fmp->monlib_bndl_size;
	bufp = malloc(monlib_size);
	if (bufp == NULL)
	    return FSLIB_ERROR_NO_MEMORY_FOR_MONLIB_BUF;
	memcpy(bufp, fmp->monlib_bndl, monlib_size);
#endif
    } else {
    	bufp = copy_monlib_to_buffer(monlib, &monlib_size, &value);
	if (bufp == NULL)
	    return value;
    }

    /*
     * We need the device number for various operations.
     */
    devnum = FSLIB_ralib->get_dev_num(dev_id);
    if (devnum < 0) {
	free(bufp);
	return convert_ralib_error_code(devnum);
    }

    /*
     * Check if device is being formated,
     * if not, lock it so we keep it for ourselves.
     */
    dev = get_dev(dev_id);
    if (dev == NULL) {
	dev = init_device(dev_id, FALSE, &value);
	if (dev == NULL) {
	    if (value != FSLIB_ERROR_BAD_DIB) {
		free(bufp);
		return -1;	
	    }
	}
    } else {
	/*
	 * Make sure device is not being opened or squeezed.
	 */	
	if (dev->open_semaphore) {
	    free(bufp);
	    return FSLIB_ERROR_DEVICE_BUSY;
	}
	

	if (dev->format_semaphore) {
	    free(bufp);
	    return FSLIB_ERROR_BEING_FORMAT;
	}
    }
    if (dev)
	dev->format_semaphore++;

    /*
     * Reset file system open file pointer.
     * Make sure device is initialized,
     * ok to init again if has already done so.
     */
    value = FSLIB_ralib->device_init(devnum);
    if (value < 0) {
	if (dev)
	    dev->format_semaphore--;
	free(bufp);
	return value;
    }

    /*
     * Clean up and init internal format parameter block.
     */
    fpp = &fp_rec;
    memset(fpp, 0, sizeof(format_dev_parm_t));
    fpp->devnum = devnum;

    /*
     * Check if we understand the type of card/flash.
     * And update info to format param block.
     */
    value = analyze_flash_device(fpp);
    if (value < 0) {
	if (bufp)
	    free(bufp);
	if (dev)
	    dev->format_semaphore--;
	return -1;
    }

    /*
     * Figure out the number of sectors.
     */
    fpp->sector_num = fpp->device_size / fpp->sector_size;
    if (fpp->sector_num * fpp->sector_size != fpp->device_size) {
	if (bufp)
	    free(bufp);
	if (dev)
	    dev->format_semaphore--;
	return FSLIB_ERROR_BAD_SECTOR_DEVICE_SIZE;
    }

    /*
     * Pass collected parameters to build_dib() to build dib
     * Later on user may specify monlib size,
     * if the given monlib size is smaller than actual monlib size,
     * then return error, otherwise,
     * pass the user monlib size to build_dib but overwrite
     * it with actual size later.
     */
    dibp = &dib_rec;
    memset(dibp, 0, sizeof(fslib_device_info_block_t));
    FSLIB_build_dib(dibp, fpp->sector_size, fpp->sector_num, fmp->spare,
	monlib_size, fpp->alg_num, fpp->erase_state, fmp->volume_id);
    fpp->dibp = dibp;

    /*
     * Find the right prog table entry by algorithm number.
     */
    ptblp = find_prog_table_entry(fpp->alg_num);
    if (ptblp == NULL) {
	if (bufp)
	    free(bufp);
	if (dev)
	    dev->format_semaphore--;
	return FSLIB_ERROR_NO_ALGORITHM;
    }
    fpp->ptblp = (void *)ptblp;

    /*
     * verify if device is erased, and if not, erase with confirmation.
     * construct bad sector map along the way (0-ok, 1-bad)
     */
    smap = erase_all_sectors(fpp, &value);
    if (smap == NULL) {
	if (bufp)
	    free(bufp);
	if (dev)
	    dev->format_semaphore--;
	return value;
    }

    /*
     * Make sure the write function is available.
     */
    fwritep = ptblp->write_fn;
    if (fwritep == NULL) {
	free(smap);
	if (bufp)
	    free(bufp);
	if (dev)
	    dev->format_semaphore--;
	return FSLIB_ERROR_NO_WRITE_FUNCTION;
    }

    /*
     * Write device info block into device offset 0.
     */
    len = sizeof(fslib_device_info_block_t);
    value = (*fwritep)(fpp->devnum, (char *)dibp, 0, len);
    if (value != len) {
	free(smap);
	if (bufp)
	    free(bufp);
	if (dev)
	    dev->format_semaphore--;
	return FSLIB_ERROR_WRITING_DIB;
    }

    /*
     * Now the dib is available on device, we should
     * lock the semaphore if hasn't done so.
     */
    if (dev == NULL) {
	dev = init_device(dev_id, FALSE, &value);
	if (dev == NULL) {
	    free(smap);
	    if (bufp)
	    	free(bufp);
	    return -1;	
	}
	dev->format_semaphore++;
    }
	
    /*
     * Write monlib to the device at offset pointed by dib
     * if there is a monlib.
     */
    value = copy_monlib_to_dev(fpp, bufp, monlib_size);
    if (value != FSLIB_OK) {
	free(smap);
	free(bufp);
	dev->format_semaphore--;
	return value;
    }

    if (bufp)	/* don't need monlib anymore */
	free(bufp);

    /*
     * Write bad sector map to device at offset given by dib.
     */
    value = copy_bad_sector_map_to_dev(fpp, smap);
    if (value != FSLIB_OK) {
	free(smap);
	dev->format_semaphore--;
	return value;
    }
    free(smap);
    if (fslib_print_enable == TRUE)
	printf("\nFormat device %s completed", dev_id);
    FSLIB_ralib->device_reset(devnum);
    init_device(dev_id, FALSE, &value);
    dev->format_semaphore--;
#endif
    return FSLIB_OK;
}
