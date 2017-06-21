/* $Id: fscreate.c,v 3.2.60.1 1996/03/18 19:35:24 gstovall Exp $
 * $Source: /release/111/cvs/Xsys/filesys/fscreate.c,v $
 *------------------------------------------------------------------
 * Utility for creating file system test files on a Unix machine
 *
 * January 1995, Jack Jenney
 *
 * Copyright (c) 1995-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: fscreate.c,v $
 * Revision 3.2.60.1  1996/03/18  19:35:24  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.2.26.2  1996/03/07  09:35:39  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2.26.1  1996/02/20  13:49:47  dstine
 * Branch: DeadKingOnAThrone_branch
 *         Sync from DeadKingOnAThrone_baseline_960122 to
 *                   DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.2  1995/11/17  09:07:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:32:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:32:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include <stdio.h>
#include <string.h>
#include "filesys.h"
#include "fslib_internals.h"
#include "fslib_dev.h"

/*
 * if this is TRUE -- only the first few sectors that contain the dib
 * bad sector map and the monlib are made.
 * if this is FALSE -- the while device is initialized.
 */
static int make_dib_sector_only = FALSE;


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
		      char *volume_id) {

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
 



/******************************************************************************
 * Name: ralib_unix_create_flash_file
 *
 * Description: initializes the Unix file system simulator
 *
 *   WARNING: this function does its best to create a file that can be used to
 *            emulate flash, but it does not do very much sanity checking of
 *            the parameters, so specifying fewer than 4 sectors or a
 *            rediculously small sector size will cause problems.
 *
 *            See the recommended values for the paramters below.
 *
 * Input: 
 *   name -- a pointer to the Unix path name of the file to create
 *   sector_size -- the sector size of the device to create (should be in the
 *     general range 256 - 256K)
 *   num_sectors -- the number of sectors in the device being created (should
 *     be in the general range 16 - 1024)
 *   erased_state -- the erased state of flash 0==>0x00000000, !0==>0xffffffff
 *   num_spare_sectors -- the number of sectors to be reserved to be spare sectors
 *
 * Return Value:
 *   boolean -- TRUE = sucessfully created, FALSE = not created or not complete
 *
 *******************************************************************************/
int ralib_unix_create_flash_file(char *name, 
				 char *monlib_name,
				 ulong sector_size, 
				 ulong num_sectors, 
				 ulong num_spare_sectors, 
				 ulong prog_algorithm,
				 ulong erased_state,
				 int make_dib_sector_only)
{
    int rc;
    int rc2;
    int i;
    int actual;
    int num_squeeze_log_sectors;
    ushort spare_sector_num;
    FILE *stream;
    FILE *monlib_stream;
    fslib_device_info_block_t  working_dib;
    fslib_device_info_block_t  *dib;
    char *file_name;
    char monlib_buffer[120 * 1024];
    ulong monlib_length;
    ulong init_length;

    rc = TRUE;
    dib = &working_dib;

    monlib_stream = fopen(monlib_name,"r");
    if (monlib_stream == NULL) {
	monlib_length = 0;
    } else {
	actual = fread(monlib_buffer,1,sizeof(monlib_buffer),monlib_stream);
	if (actual == sizeof(monlib_buffer)) {
	    printf("MONLIB file %s is too big (larger than %d bytes)\n",monlib_name,sizeof(monlib_buffer));
	    rc = FALSE;
	} else {
	    monlib_length = actual;
	}
    }

printf("MONLIB = %s, monlib_length = %d\n",monlib_name,monlib_length);

    if (rc == TRUE) {
	/* 
	 * open the file for the file system
	 */
	stream = fopen(name,"w+");
	if (stream == NULL) {
	    printf("**ERROR** Error opening file \"%s\"\n",name);
	    rc = FALSE;
	} else {
	    if (erased_state != 0)
		erased_state = 0xffffffff;
	    /*
	     * Set the whole DIB to the erased state so unused parts of
	     * the DIB are set to the erased state when we are done
	     */
	    memset(dib,(int)erased_state,sizeof(fslib_device_info_block_t));
	    
	    /*
	     * Get the file name part from the path name
	     */
	    file_name = strrchr(name,'/');
	    if (file_name == NULL) {
		file_name = name;
	    }
	    
	    /*
	     * build the DIB
	     */
	    rc = FSLIB_build_dib(dib,
				 sector_size,
				 num_sectors,
				 num_spare_sectors,
				 monlib_length,
				 prog_algorithm,
				 erased_state,
				 file_name);

	    print_dib(dib);
	    /*
	     * Now write the DIB to the file you just opened
	     */
	    actual = fwrite(dib,1,sizeof(fslib_device_info_block_t),stream);
	    if (actual != sizeof(fslib_device_info_block_t)) {
		printf("**ERROR** Error writing DIB to file \"%s\"\n",name);
		rc = FALSE;
	    } else {
		if (make_dib_sector_only) {
		    init_length = dib->file_system_offset;
		} else {
		    init_length = dib->length;
		}
		for (i = sizeof(fslib_device_info_block_t); i < init_length; i++) {
		    actual = fwrite(&dib->erased_state,1,1,stream);
		    if (actual != 1) {
			printf("**ERROR** Error initializing file \"%s\" (byte %6.6lx)\n",name,i);
			rc = FALSE;
			break;
		    }
		}
		if (rc == TRUE) {
		    if (monlib_length != 0) {
			/*
			 * write the monlib to the file.
			 * Seek to the place,
			 * start writing from the monlib_buffer
			 */
			rc2 = fseek(stream,dib->monlib_offset,SEEK_SET);
			if (rc2 != 0) {
			    printf("**ERROR** Error seeking in file to write monlib\n");
			    rc = FALSE;
			} else {
			    actual = fwrite(monlib_buffer,1,dib->monlib_length,stream);
			    if (actual != dib->monlib_length) {
				printf("**ERROR** Error writing monlib.  monlib_length=%d, actual = %d\n",dib->monlib_length,actual);
			    }
			}
		    }
		    fclose(stream);
		}
	    }
	}
    return(rc);
    }
}


/******************************************************************************
 * Name: main
 *
 * Description: main for creating flash simulation files
 *
 * Input: 
 *   argc and argv
 *     argv[1] = file_name -- name of the file to create
 *     argv[2] = num_sectors -- number of total sectors on device (decimal)
 *     argv[3] = sector_size -- number of bytes per sector (decimal)
 *     argv[4] = erased_state -- 0 = 0x00000000, else erased state is 0xffffffff
 *     argv[5] = num_spare_sectors -- up to 16
 *     argv[6] = monlib_name -- name of the file containing the MONLIB
 *
 * Return Value: 
 *   exits with 0 if successful, otherwise exits with -1
 *
 *******************************************************************************/
int main(int argc, char *argv[])
{
    int interactive_mode;
    char buffer[128];
    char monlib_name_buffer[128];
    char *monlib_name;
    char file_name[128];
    char *newline;
    ulong num_sectors;
    ulong sector_size;
    ulong erased_state;
    ulong num_spare_sectors;
    int make_dib_sector_only;
    int converted;
    int rc;


    monlib_name = monlib_name_buffer;
    /* 
     * convert the parameters that were given
     */
    interactive_mode = TRUE;
    if (argc >= 2) {
	if (strlen(argv[1]) > sizeof(file_name)) {
	    argc = 1;
	} else {
	    strcpy(file_name,argv[1]);
	}
    }

    if (argc >= 3) {
	num_sectors = 0;
	converted = sscanf(argv[2],"%d",&num_sectors);
	if (converted != 1) {
	    argc = 2;
	}	
    }

    if (argc >= 4) {
	sector_size = 0;
	converted = sscanf(argv[3],"%d",&sector_size);
	if (converted != 1) {
	    argc = 3;
	}	
    }
    
    if (argc >= 5) {
	erased_state = 0;
	converted = sscanf(argv[4],"%d",&erased_state);
	if (converted != 1) {
	    argc = 4;
	}
    }

    if (argc >= 6) {
	num_spare_sectors = 0;
	converted = sscanf(argv[5],"%d",&num_spare_sectors);
	if (converted != 1) {
	    argc = 5;
	}
    }

    if (argc >= 7) {
	monlib_name = argv[6];
    }

    if (argc >= 8) {
	make_dib_sector_only = 0;
	converted = sscanf(argv[7],"%d",&make_dib_sector_only);
	if (converted != 1) {
	    argc = 7;
	}
    }

    if (argc >= 9) {
	printf("\nToo many parameters\n");
	exit(-1);
    }

    /*
     * now go get the parameters that we need
     */
    switch (argc) {
    case 1: 
	printf("\nName of file to create: ");
	fgets(file_name,sizeof(file_name),stdin);
	file_name[sizeof(file_name) - 1] = '\0';
	newline = strchr(file_name,'\n');
	if (newline != NULL) {
	    *newline = '\0';
	}
	/* drops through to the next case */
    case 2:
        do {
	    printf("\nNumber of sectors (16 to 1024 decimal): ");
	    fgets(buffer,sizeof(buffer),stdin);
	    num_sectors = 0;
	    converted = sscanf(buffer,"%d",&num_sectors);
	} while (converted != 1);
	/* drops through to the next case */
    case 3:
        do {
	    printf("\nSector size (256 to 262144 decimal): ");
	    fgets(buffer,sizeof(buffer),stdin);
	    sector_size = 0;
	    converted = sscanf(buffer,"%d",&sector_size);
	} while (converted != 1);
	/* drops through to the next case */
    case 4:
        do {
	    printf("\nErased state of flash (0 or 1): ");
	    fgets(buffer,sizeof(buffer),stdin);
	    erased_state = 0;
	    converted = sscanf(buffer,"%d",&erased_state);
	} while (converted != 1);
	if (erased_state) {
	    erased_state = 1;
	}
	/* drops through to the next case */
    case 5:
        do {
	    printf("\nNumber of Spare Sectors (0 to %d): ",FSLIB_SPARE_SECTOR_MAX);
	    fgets(buffer,sizeof(buffer),stdin);
	    num_spare_sectors = 0;
	    converted = sscanf(buffer,"%d",&num_spare_sectors);
	    if (converted == 1) {
		if (num_spare_sectors > FSLIB_SPARE_SECTOR_MAX) {
		    printf("\n***ERROR*** Number of spare sectors must be less than %d\n",FSLIB_SPARE_SECTOR_MAX);
		    converted = 0;
		}
	    }
	} while (converted != 1);
	/* drops through to the next case */
    case 6:
	printf("\nFile name of MONLIB: ");
	fgets(monlib_name_buffer,sizeof(monlib_name_buffer),stdin);
	monlib_name_buffer[strlen(monlib_name) - 1] = '\0';
	/* drops through to the next case */
    case 7:
	while (1) {
	    printf("\nMake option:\n");
	    printf("  0 = Make entire file system\n");
	    printf("  1 = Make DIB sector only\n");
	    printf("Enter make option: ");
	    fgets(buffer,sizeof(buffer),stdin);
	    if (buffer[0] == '0') {
		make_dib_sector_only = FALSE;
		break;
	    }
	    if (buffer[0] == '1') {
		make_dib_sector_only = TRUE;
		break;
	    }
	}
	break;

    case 8:
	interactive_mode = FALSE;
	break;
    }
    
    if (interactive_mode) {
	printf("\nCreate %s, %d sectors, %d bytes per sector, erased state = %d, MONLIB = %s? (y/n) ",
	       file_name,num_sectors,sector_size,erased_state,monlib_name);
	fgets(buffer,sizeof(buffer),stdin);
	if ((buffer[0] != 'Y') && (buffer[0] != 'y')) {
	    exit(-1);
	}
    }

    rc = ralib_unix_create_flash_file(file_name,
				      monlib_name,
				      sector_size,
				      num_sectors,
				      num_spare_sectors,
				      FSLIB_PROG_INTEL_SERIES_2PLUS_CARD,
				      erased_state,
				      make_dib_sector_only);
    if (rc == TRUE) {
	if (interactive_mode)
	    printf("\n\n%s created successfully\n",file_name);
	exit(0);
    } else {
	if (interactive_mode)
	    printf("\n\nERROR creating %s\n",file_name);
	exit(-1);
    }



}



