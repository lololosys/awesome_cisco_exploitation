/* $Id: fslib_print.c,v 3.2.60.1 1996/03/18 19:35:49 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/fslib_print.c,v $
 *------------------------------------------------------------------
 * Print utilites for debugging the flash file system
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fslib_print.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:49  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:36:08  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:50:13  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:08:24  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:26  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/06/16  04:26:45  dkatz
 * CSCdi35882:  Time-of-day code needs cleanup
 *
 * Revision 2.2  1995/06/14  15:18:30  anke
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
 * Revision 2.1  1995/06/07  20:32:44  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#if !defined(STANDALONE) 
#include "master.h"
#include <string.h>
#include "../os/clock.h"
#endif

#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_print.h"

/*----------------------------------------------------------------------------*
 * global debug message control variables
 *----------------------------------------------------------------------------*/
int global_indent_level = 0;
int global_print = FALSE;
int global_print_level = 1;
int global_print_mask = 0x03ffffff;


/******************************************************************************
 * Name: ulong_dump
 *
 * Description: prints a data block formatted as 32-bit values
 *
 * Input:
 *   data -- pointer to the first ulong to print (must be 32-bit aligned)
 *   length -- the length in bytes of the data to print (will be
 *     rounded up to the nearest even 32-bits)
 *
 *******************************************************************************/
void ulong_dump (ulong *data,int length)
{
    int i;

    for (i = 0; i < ((length + 3)/4); i++) {
	if ((i % 8) == 0)
	    printf("%6lx: ",i);
	printf("%8lx ",data[i]);
	if ((i % 8) == 7)
	    printf("\n");
	else if ((i % 4) == 3)
	    printf(" ");
    }
    printf("\n");
}


/******************************************************************************
 * Name: ushort_dump
 *
 * Description: prints a data block formatted as 16-bit values
 *
 * Input:
 *   data -- pointer to the first ushort to print (must be 16-bit aligned)
 *   length -- the length in bytes of the data to print (will be
 *     rounded up to the nearest even 16-bits)
 *
 *******************************************************************************/
void ushort_dump (ushort *data,int length)
{
    int i;

    for (i = 0; i < ((length + 1)/2); i++) {
	if ((i % 16) == 0)
	    printf("%8lx: ",i * 2);
	printf("%4x ",data[i]);
	if ((i % 16) == 15)
	    printf("\n");
	else if ((i % 4) == 3)
	    printf(" ");
    }
    printf("\n");
}


/******************************************************************************
 * Name: char_dump
 *
 * Description: prints a data block formatted as 8-bit values
 *
 * Input:
 *   data -- pointer to the first byte to print
 *   length -- the length in bytes of the data to print
 *
 *******************************************************************************/
void char_dump (char *data,int length)
{
    int i;

    for (i = 0; i < length; i++) {
	if ((i % 16) == 0)
	    printf("%6lx: ",i);
	printf("%2x ",data[i] & 0x000000ff);
	if ((i % 16) == 15)
	    printf("\n");
	else if ((i % 4) == 3)
	    printf(" ");
    }
    printf("\n");
}


/******************************************************************************
 * Name: print_file_hdr
 *
 * Description: prints a formatted file header to the console
 *
 * Input:
 *   fh -- pointer to a file header structure to print (in DRAM, not flash)
 *
 *******************************************************************************/
void print_file_hdr (fslib_file_hdr_t *fh)
{

    if (fh == NULL) {
	printf("NULL file header pointer");
    } else if (fh->magic != FSLIB_FILE_HDR_MAGIC) {
	ulong_dump((ulong *)fh, sizeof(fslib_file_hdr_t));
    } else {
	printf("FILE HEADER: %s\n",fh->file_name);
	printf("  hdr flags=%8lx  -- ",fh->hdr_flags);

	switch (fh->hdr_flags) {
	case FSLIB_HDR_FLAGS_IN_USE:
	    printf("IN USE \n");
	    break;
	case FSLIB_HDR_FLAGS_CREATED:
	    printf("CREATED \n");
	    break;
	case FSLIB_HDR_FLAGS_VALID:
	    printf("VALID \n");
	    break;
	case FSLIB_HDR_FLAGS_ERROR:
	    printf("ERROR \n");
	    break;
	default:
	    printf("UNKNOWN \n");
	    break;
	}

	printf("  deleted=%4x  undeleted=%4x -- ",fh->deleted,fh->undeleted);
	if ((fh->deleted ^ fh->undeleted) == 0) {
	    printf("UNDELETED\n");
	} else {
	    printf("DELETED\n");
	}

	printf("  index=%d  crc=%8lx  time=%8lx  length=%d\n",
	       fh->index,fh->crc,fh->time,fh->length);

	printf("  next=%8lx\n",fh->next);
    }

}


/******************************************************************************
 * Name: print_dib
 *
 * Description: prints a formatted device info block (DIB) to the console
 *
 * Input:
 *   dib -- pointer to a DIB to print (in DRAM, not flash)
 *
 *******************************************************************************/
void print_dib (fslib_device_info_block_t *dib)
{
    int i;

    if (dib == NULL) {
	printf("NULL device info block pointer");
    } else if (dib->magic != FSLIB_DEVICE_INFO_BLOCK_MAGIC) {
	ulong_dump((ulong *)dib, sizeof(fslib_device_info_block_t));
    } else {
	printf("DEVICE INFO BLOCK: %s\n",dib->volume_id);
	printf("  Magic Number          = %8lx  File System Vers = %8lx (%d.%d)\n",
	       dib->magic,
	       dib->file_system_version,
	       dib->file_system_version >> 16,
	       dib->file_system_version & 0x0000ffff);
	printf("  Length                = %8lx  Sector Size      = %8lx\n",
	       dib->length, dib->sector_size);
	printf("  Programming Algorithm = %8lx  Erased State     = %8lx\n",
	       dib->prog_algorithm, dib->erased_state);
	printf("  File System Offset    = %8lx  Length = %8lx\n",
	       dib->file_system_offset,dib->file_system_length);
	printf("  MONLIB Offset         = %8lx  Length = %8lx\n",
	       dib->monlib_offset,dib->monlib_length);
	printf("  Bad Sector Map Offset = %8lx  Length = %8lx\n",
	       dib->bad_sector_map_offset,dib->bad_sector_map_length);
	printf("  Squeeze Log Offset    = %8lx  Length = %8lx\n",
	       dib->squeeze_log_offset,dib->squeeze_log_length);
	printf("  Squeeze Buffer Offset = %8lx  Length = %8lx\n",
	       dib->squeeze_buffer_offset,dib->squeeze_buffer_length);
	printf("  Num Spare Sectors     = %d\n",
	       dib->num_spare_sectors);
	printf("    Spares: ");
	for (i = 0; (i < dib->num_spare_sectors) && (i < FSLIB_SPARE_SECTOR_MAX); i++) {
	    printf("%d ",dib->spare[i].spare_sector_num);
	}
	printf("\n");

    }

}


/******************************************************************************
 * Name: print_bad_sector_map
 *
 * Description: prints a formatted bad sector map
 *
 * Input:
 *   bsm -- pointer to the first byte of the bad sector map to print
 *     (in DRAM, not flash)
 *   length -- length of the bad sector map (in bytes)
 *
 *******************************************************************************/
void print_bad_sector_map(char *bsm, int length)
{

    if (bsm == NULL) {
	printf("NULL bad sector table pointer");
    } else {
	printf("BAD SECTOR MAP:\n");
	char_dump(bsm,length);
    }

}


/******************************************************************************
 * Name: print_dev
 *
 * Description: prints a formatted device structure
 *
 * Input:
 *   dev -- pointer to a device record to print
 *   flags -- indicates whether or not to print the DIB that is part
 *     of the device record.  If the LSB is set to 1, the DIB is
 *     printed, otherwise it is not.
 *
 *******************************************************************************/
void print_dev(fslib_device_t *dev,int flags)
{

    if (dev == NULL) {
	printf("NULL device record pointer");
    } else {
	printf("DEVICE RECORD: (%d) %s\n",dev->dev_num,dev->dev_id);
	printf("  valid=%d  file_open_for_write=%d  writable=%d\n",
	       dev->valid,dev->file_open_for_write,dev->writable);
	printf("  file_system_end=%8lx  last_sector=%d\n",
	       dev->file_system_end,dev->last_sector);
	printf("  next_new_file_hdr_ptr=%8lx  next_index=%d\n",
	       dev->next_new_file_hdr_ptr,dev->next_index);
	printf("  write_fn=%8lx    erase_fn=%8lx\n",
	       dev->write_fn,dev->erase_fn);
	if (flags & 1) {
	    print_dib(&dev->dib);
	}
    }
}


/******************************************************************************
 * Name: print_file_rec
 *
 * Description: prints a formatted file record
 *
 * Input:
 *   fp -- pointer to a file record to print
 *   flags -- bit significant flags that cause related data structure
 *     to be printed
 *     Bit #    Action
 *     -----    ------------------------------------------------------------
 *       0      If set, causes the related file header to be printed
 *       1      If set, causes the related device record to be printed
 *
 *******************************************************************************/
void print_file_rec(fslib_file_rec_t *fp,int flags)
{
    char *state_string;
    char *type_string;

    if (fp == NULL) {
	printf("NULL file record pointer");
    } else {
	printf("FILE RECORD: descriptor = %d\n",fp->fd);
	printf("  hdr_phys_ptr   = %8lx    start_phys_ptr = %8lx\n",
	       fp->hdr_phys_ptr,fp->start_phys_ptr);
	printf("  curr_phys_ptr  = %8lx    curr_ptr       = %8lx\n",
	       fp->curr_phys_ptr,fp->curr_ptr);
	switch (fp->state) {
	case FSLIB_FILE_STATE_CLOSED:  state_string = "CLOSED "; break;
	case FSLIB_FILE_STATE_OPENING: state_string = "OPENING"; break;   
	case FSLIB_FILE_STATE_OPEN:    state_string = "OPEN   "; break;   
	case FSLIB_FILE_STATE_ERROR:   state_string = "ERROR  "; break;   
	default:                       state_string = "UNKNOWN"; break;   
	}
	switch (fp->type) {
	case FSLIB_FILE_TYPE_NORMAL:    type_string = "NORMAL "; break;   
	case FSLIB_FILE_TYPE_DIRECTORY: type_string = "DIR    "; break;   
	default:                        type_string = "UNKNOWN"; break;   
	}
	printf("  state=%s  type=%s  writable=%s  readable=%s\n",
	       state_string,type_string,fp->writable?"YES":"NO",fp->readable?"YES":"NO");
	if (flags & 1) {
	    print_file_hdr(&fp->hdr);
	}
	if (flags & 2) {
	    print_dev(fp->dev,1);
	}
    }
}


/******************************************************************************
 * Name: print_file_recs
 *
 * Description: prints all file records formatted 
 *
 * Input:
 *   flags -- flags parameter to pass on to the print_file_rec() function
 *
 *******************************************************************************/
void print_file_recs(int flags)
{
    int i;
    fslib_file_rec_t *fp;

    for (i = 0; i < FSLIB_MAX_FILE_REC; i++) {
	fp = &FSLIB_file_rec[i];
	if (fp->fd != 0) {
	    print_file_rec(fp,flags);
	    printf("\n");
	}
    }
}


/******************************************************************************
 * Name: print_squeeze_log_rec
 *
 * Description: prints a squeeze record
 *
 * Input:
 *   sl -- pointer to the squeeze log record to print (in DRAM, not flash)
 *
 *******************************************************************************/
void print_squeeze_log_rec(fslib_squeeze_log_rec_t *sl)
{

    switch (sl->flags) {
    default:
	printf("  UNKNOWN record flags\n");
	break;
    case 0:
	printf("  UNUSED squeeze log record\n");
	break;
    case FSLIB_SQUEEZE_LOG_FLAGS_IN_USE:
    case FSLIB_SQUEEZE_LOG_FLAGS_INVALID:
	if (sl->flags == FSLIB_SQUEEZE_LOG_FLAGS_IN_USE) {
	    printf("  INCOMPLETE");
	} else {
	    printf("  INVALID");
	}
	/* falls through to the next case */
    case FSLIB_SQUEEZE_LOG_FLAGS_VALID:
	if (sl->type == FSLIB_SQUEEZE_LOG_TYPE_COPY) {
	    printf("  COPY  sector at %6lx to squeeze buffer\n",sl->s.copy.wr_ptr);
	} else if (sl->type == FSLIB_SQUEEZE_LOG_TYPE_WRITE) {
	    switch (sl->state) {
	    case FSLIB_SQUEEZE_START_OF_HDR:
		printf("  WRITE sector at %6lx (START HDR  hdr=%6lx  idx=%d)\n",
		       sl->s.start_of_hdr.wr_ptr,
		       sl->s.start_of_hdr.old_hdr_ptr,
		       sl->s.start_of_hdr.file_index);
		break;
	    case FSLIB_SQUEEZE_MIDDLE_OF_HDR:
		printf("  WRITE sector at %6lx (MIDDL HDR  hdr=%6lx  idx=%d  len=%d)\n",
		       sl->s.middle_of_hdr.wr_ptr,
		       sl->s.middle_of_hdr.old_hdr_ptr,
		       sl->s.middle_of_hdr.file_index,
		       sl->s.middle_of_hdr.bytes_left_this_hdr);
		break;
	    case FSLIB_SQUEEZE_DATA:
		printf("  WRITE sector at %6lx (DATA       hdr=%6lx  idx=%d  len=%d  rd=%6lx)\n",
		       sl->s.data.wr_ptr,
		       sl->s.data.next_old_hdr_ptr,
		       sl->s.data.file_index,
		       sl->s.data.bytes_left_this_file,
		       sl->s.data.rd_ptr);
		break;
	    case FSLIB_SQUEEZE_DONE:
		printf("  WRITE DONE\n");
		break;
	    case FSLIB_SQUEEZE_ERROR:
		printf("  WRITE ERROR\n");
		break;
	    default:
		printf("  WRITE record has unknown state %d\n",sl->state);
		break;
	    }
	} else {
	    printf("  UNKNOWN squeeze log record type %d\n",sl->type);
	}
    }
}


/******************************************************************************
 * Name: print_dirent_header
 *
 * Description: prints a dirent (directory entry)
 *
 * Input: None
 *   
 *******************************************************************************/
void print_dirent_header (void)
{

    printf("-#- ED --type-- --crc--- -seek-- nlen -length- -----date/time------ name\n");

}

void print_dirent_header_short (void)
{
    printf("-#- -length- -----date/time------ name\n");
}


/******************************************************************************
 * Name: print_dirent
 *
 * Description: prints a dirent (directory entry)
 *
 * Input:
 *   de -- pointer to the directory entry to print
 *
 *******************************************************************************/
void print_dirent (dirent_t *de)
{
    char date_time_string[UNIX_TIME_STRING_2_MINBUF];

#if (defined(STANDALONE) || defined(FSLIB))
    strcpy(date_time_string,"--- no date/time ---");
#else
    unix_time_string_2(date_time_string, de->time);
#endif

    printf("%3d %s%s %s %-08lx %-7lx %-4d %-8ld %s %s\n",
	   de->d_fileno,
	   de->valid   ? ".":"E",
	   de->deleted ? "D":".",
	   (de->type == FILE_TYPE_SYS_CONFIG)   ? "config  " :
	   ((de->type == FILE_TYPE_SYS_IMAGE)   ? "image   " : "unknown " ),
	   de->crc,
	   de->d_off,
	   de->d_namlen,
	   de->size,
	   date_time_string,
	   de->d_name);

}

void print_dirent_short (dirent_t *de)
{
    char date_time_string[UNIX_TIME_STRING_2_MINBUF];

#if (defined(STANDALONE) || defined(FSLIB))
    strcpy(date_time_string,"--- no date/time ---");
#else
    unix_time_string_2(date_time_string, de->time);
#endif

    printf("%3d %-8ld %s %s\n",
	   de->d_fileno,
	   de->size,
	   date_time_string,
	   de->d_name);
}


/******************************************************************************
 * Name: print_stat
 *
 * Description: prints a stat_t (file status)
 *
 * Input:
 *   st -- pointer to the status structure to print
 *
 *******************************************************************************/
void print_stat(stat_t *st)
{

    printf("File Status: %s\n",st->name);
    printf("  st_ino   = %d\n",st->st_ino);
    printf("  st_size  = %d\n",st->st_size);
    printf("  time   a = %8lx    m = %8lx    c = %8lx\n",
	   st->st_atime,st->st_mtime,st->st_ctime);
    printf("  namlen   = %d\n",st->namlen);
    printf("  type     = %8lx\n",st->type);
    printf("  flags    = %8lx\n",st->flags);
    printf("  crc      = %8lx\n",st->crc);
    printf("  valid    = %s\n",st->valid ? "valid" : "NOT VALID");
    printf("  deleted  = %s\n",st->deleted ? "DELETED" : "undeleted");

}


/******************************************************************************
 * Name: print_file_sys_stat
 *
 * Description: prints a file system status structure (file_sys_stat_t)
 *
 * Input:
 *   fss -- pointer to the file system status structure to print
 *
 *******************************************************************************/
void print_file_sys_stat(file_sys_stat_t *fss)
{
    fslib_device_info_block_t *dib;

    dib = &fss->dib;
    printf("-------- F I L E   S Y S T E M   S T A T U S --------\n");
    printf("  Device Number = %d\n",fss->dev_num);
    print_dib(dib);
    printf("STATUS INFO:\n");
    printf("  %sWritable\n",fss->writable?"":"NOT ");
    printf("  %sFile Open for Write\n",fss->file_open_for_write?"":"NO ");
    printf("  %sStats\n",fss->incomplete_stats?"Incomplete ":"Complete ");
    printf("  %sUnrecovered Errors\n",fss->unrecovered_errors?"":"No ");
    printf("  %sSqueeze in progress\n",fss->squeeze_in_progress?"":"No ");

    printf("USAGE INFO:\n");
    printf("  Bytes Used     = %6lx  Bytes Available = %6lx\n",
	   fss->bytes_used,fss->bytes_available);
    printf("  Bad Sectors    = %3d     Spared Sectors  = %d\n",
	   fss->bad_sectors,fss->spared_sectors);
    printf("  OK Files       = %3d     Bytes = %6lx\n",
	   fss->files_ok,fss->files_ok_bytes);
    printf("  Deleted Files  = %3d     Bytes = %6lx\n",
	   fss->files_deleted,fss->files_deleted_bytes);
    printf("  Files w/Errors = %3d     Bytes = %6lx\n",
	   fss->files_with_errors,fss->files_with_errors_bytes);

}
