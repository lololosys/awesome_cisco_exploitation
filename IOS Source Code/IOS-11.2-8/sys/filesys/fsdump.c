/* $Id: fsdump.c,v 3.2.60.1 1996/03/18 19:35:26 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/fsdump.c,v $
 *------------------------------------------------------------------
 * Utility for dumping file system test files on a Unix machine
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fsdump.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:26  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:42  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:49  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:07:54  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:03  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:13  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


#if defined(STANDALONE)
#include <stdio.h>
#include <string.h>
#else
#include "master.h"
#endif

#include "types.h"
#include "filesys.h"
#include "ralib.h"
#include "fslib_internals.h"
#include "fslib_print.h"


/******************************************************************************
 * Name: printhelp
 *
 *
 *
 *******************************************************************************/
static void printhelp(void)
{
    printf("\n H E L P    I N F O R M A T I O N  ----  F S D U M P\n\n");
    printf("  This is a utility to dump flash file system file headers\n\n");
    printf("  syntax:  fsdump <filename>\n\n");
}

/******************************************************************************
 * Name: main
 *
 *
 *
 *******************************************************************************/
int main(int argc, char *argv[])
{
    int i;
    int actual;
    int read_length;
    FILE *stream;
    fslib_device_info_block_t dev_info_block;
    fslib_device_info_block_t *dib;
    fslib_squeeze_log_rec_t *sl;
    fslib_file_hdr_t file_hdr;
    fslib_file_hdr_t *fh;
    ulong buffer[65536];
    ulong fslib_ptr;

    fh = &file_hdr;
    dib = &dev_info_block;

    if (argc == 1) {
	printhelp();
    } else {
	stream = fopen(argv[1],"r");
	if (stream == NULL) {
	    printf("Could not open file \"%s\"\n",argv[1]);
	} else {
	    printf("Dumping file system from file \"%s\"\n\n",argv[1]);
	    
	    /* 
	     * print the DIB
	     */
	    actual = fread(dib,sizeof(fslib_device_info_block_t),1,stream);
	    if (actual != 1) {
		printf("Error reading DIB\n");
	    } else {
		print_dib(dib);
		printf("\n");
		/* 
		 * print the bad sector map
		 */
		fseek(stream,dib->bad_sector_map_offset,0);
		if (dib->bad_sector_map_length > sizeof(buffer))
		    read_length = sizeof(buffer);
		else 
		    read_length = dib->bad_sector_map_length;
		actual = fread(buffer,1,read_length,stream);
		if (actual != read_length) {
		    printf("Error reading Bad Sector Map\n");
		} else {
		    print_bad_sector_map((char *)buffer,read_length);

		    /* 
		     * print the squeeze log if not empty 
		     */
		    fseek(stream,dib->squeeze_log_offset,0);
		    if (dib->squeeze_log_length > sizeof(buffer))
			read_length = sizeof(buffer);
		    else 
			read_length = dib->squeeze_log_length;
		    actual = fread(buffer,1,read_length,stream);
		    if (actual != read_length) {
			printf("Error reading Squeeze Log\n");
		    } else {
			sl = (fslib_squeeze_log_rec_t *)buffer;
			sl->flags ^= (uchar)dib->erased_state;
			while (sl->flags != 0) {
			    if ((ulong *)sl == buffer) {
				printf("Squeeze Log:\n");
			    }
			    print_squeeze_log_rec(sl);
			    sl++;
			    sl->flags ^= (uchar)dib->erased_state;
			}
			if ((ulong *)sl != buffer) {
			    printf("\n");
			}

			/* 
			 * print the file system file headers
			 */
			fslib_ptr = dib->file_system_offset;
			while ((fslib_ptr != 0) && (fslib_ptr != 0xffffffff)) {
			    fseek(stream,fslib_ptr,0);
			    if (ftell(stream) != fslib_ptr) {
				printf("Error reading file header -- could not seek to %8.8lx\n",fslib_ptr);
				break;
			    } else {
				actual = fread(fh,sizeof(fslib_file_hdr_t),1,stream);
				if (actual != 1) {
				    printf("Error reading file header\n");
				    break;
				} else {
				    fh->file_flags ^= dib->erased_state;
				    fh->hdr_flags  ^= dib->erased_state;
				    fh->deleted    ^= (ushort)(dib->erased_state);
				    fh->undeleted  ^= (ushort)(dib->erased_state);
				    print_file_hdr(fh);
				    if (fh->magic != FSLIB_FILE_HDR_MAGIC) {
					fslib_ptr = 0;
				    } else if (fh->next == 0) {
					fslib_ptr += sizeof(fslib_file_hdr_t);
				    } else {
					fslib_ptr = fh->next;
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

