/* $Id: simpfsio.c,v 3.2.58.2 1996/06/16 21:18:27 vnguyen Exp $
 * $Source: /release/112/cvs/Xsys/rommon/src/simpfsio.c,v $
 *------------------------------------------------------------------
 * simpfsio.c
 *
 * February 1994, Rob Clevenger
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 * Routines for I/O on simple file system.
 *------------------------------------------------------------------
 * $Log: simpfsio.c,v $
 * Revision 3.2.58.2  1996/06/16  21:18:27  vnguyen
 * Finally committing Brasil to California_branch
 *
 * Revision 3.2.58.1  1996/03/21  23:27:59  gstovall
 * Branch: California_branch
 * The ELC_branch now knows the joy of California dreaming.
 *
 * Revision 3.2.2.1  1995/12/01  04:21:17  mbeesley
 * Branch: ELC_branch
 * Reorganize Rom monitor header files to make it easier to port
 * to new platforms.
 *
 * Revision 3.2  1995/11/17  18:43:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:06:49  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:38:32  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */


/*
** These routines are needed to do I/O on a Simple File System
** These are the default routines in the Monitor
**
** Note: this code gets compiled in 2 different environments, the
** monitor and monlib.
**
** Note: There should never be any information about physical devices
** in this module except for devnum and devsiz.  In fact scrutinize
** carefully if/when it is determined that more include files need
** to be added.
*/

#include "monitor.h"
#include "getdents.h"
#include "fstat.h"
#include "fsio.h"
#include "simpfsio.h"
#include "console.h"

struct simp_fsio sfsiotbl[MAXFSFD];
/* forward declaration */
unsigned short sfs_fchksum(int devnum, char *logfptr, int size);

/*
** The simple flash file system open routine
*/
int
sfs_open(char *path, int mode, int devnum, int offset, long devsiz)
{
    struct simp_fsio *sfsiop;
    struct flash_file_hdr ffh;
    struct flash_file_hdr *ffhptr = &ffh;
    int i, fsfd;
    char *phys_fptr;
    int size;

    if(*path == '\0') {  /* no file name specified - raw open */
	printf("sfs_open: no file name specified\n");
	return(-1);
    }
    sfsiop = (struct simp_fsio *)0;
    for(fsfd=0; fsfd<MAXFSFD; fsfd++) {

#ifdef DEBUG
printf("looking for empty file slot in sfs_open\n");
printf("fs[%d].flags = %#x, .devnum is %d, .start is %#x\n",
       fsfd,sfsiotbl[fsfd].flags,sfsiotbl[fsfd].devnum,sfsiotbl[fsfd].start); 
#endif

	if(sfsiotbl[fsfd].flags == SFS_UNCLAIMED) {  /* find an unused slot */
	    sfsiop = &sfsiotbl[fsfd];
	    sfsiop->flags = SFS_UNKNOWN;  /* claim this slot */
	    break;
	}
    }
    if(!sfsiop) {
	printf("sfs_open: no space in file table\n");
	return(-1);
    }
    
    sfsiop->devnum = devnum;
    sfsiop->start = (char *)offset;
    sfsiop->fptr = sfsiop->start;
    sfsiop->size = devsiz;
    sfsiop->end = sfsiop->start + sfsiop->size;
    sfsiop->mode = mode;
    sfsiop->fsfd = fsfd;

#ifdef DEBUG
printf("sfs_open: devnum is: %d, start is: %#x, fptr is: %#x, size is: %#x,\n",
       sfsiop->devnum, sfsiop->start, sfsiop->fptr, sfsiop->size);
printf("end is: %#x, fsfd is: %d\n", sfsiop->end, sfsiop->fsfd);
#endif 

    if(strcmp(path, ".") == 0) return(fsfd);  /* open the file system */

    for(i=0; ; i++) {

	/*
	** This assumes that the device itself starts on
	** a 4 byte boundary
	*/
	if((long)sfsiop->fptr & 3) {
	    printf("open: simple flash file header not on 4 byte boundary\n");
#ifdef DEBUG
	    printf("file pointer = %#x\n", sfsiop->fptr);
#endif
	    return(-1);
	}

	if(sfs_read(fsfd, (unsigned char *)ffhptr,
		    sizeof(ffh)) != sizeof(ffh)) {
	    printf("open: trouble reading simple flash file header\n");
badopen:
	    sfsiop->flags = SFS_UNCLAIMED;  /* release this slot */
	    return(-1);
	}
	if(ffhptr->ffh_magic != FLASH_FHDR_MAGIC) {  /* check if magic */
	    if(i == 0) {  /* bad first record - not simple file */
printf("open: bad first simple flash file magic number (%#.4x) at (logicl) location %#x\n",
		       ffhptr->ffh_magic, sfsiop->start);
		goto badopen;
	    }

	    /*
	    ** This could mean the end of the file system
	    */
	    if(ffhptr->ffh_magic == 0xffffffff && ffhptr->ffh_len == 0xffffffff) {
#ifdef DEBUG
		printf("sfs_open: end of file system reached\n");
#endif
		printf("open: file \"%s\" not found\n", path);
		goto badopen;
	    }
	    /*
	    ** Should never get here
	    */
	    printf("open: bad flash file magic number (%#.4x)\n", ffhptr->ffh_magic);
	    printf("file size: %#x, flags: %#x\n", ffhptr->ffh_len, ffhptr->ffh_flags);
	    printf("possible end of file system reached\n");
	    goto badopen;
	}

#ifdef DEBUG
	printf("got flash file header...");
	printf("name is %s ...", ffhptr->ffh_name);
	printf("file size is %#x\n", ffhptr->ffh_len);
#endif
	if(strcmp(ffhptr->ffh_name, path) == 0 &&
	   (ffhptr->ffh_flags & FFH_FLAGS_DELETED)) {  /* found our file */

	    if(ffhptr->ffh_checksum == sfs_fchksum(devnum, sfsiop->fptr, ffhptr->ffh_len))
		break;  /* we are already at the first byte of the file */
	    /*
	    ** We should continue on here, there may be other files of
	    ** the same name in the file system with a good checksum (gack).
	    */
	}

	/* seek over file contents (round up to the next 4 byte boundary) */
	if(sfs_lseek(sfsiop->fsfd, (long)(ffhptr->ffh_len + 3) & ~3, SEEK_CUR) < 0) {
	    printf("open: lseek error\n");
	    return(-1);
	}
    }
    sfsiop->start = sfsiop->fptr;    /* set the start of the file */
    sfsiop->size = ffhptr->ffh_len;  /* pick up file size from ffh */
    sfsiop->end = sfsiop->start + sfsiop->size;
    sfsiop->flags = SFS_SIMP;
    return(fsfd);
}


/*
** The simple flash file system close routine
*/
int
sfs_close(int fsfd)
{
    if(fsfd >= MAXFSFD ||
       sfsiotbl[fsfd].flags == SFS_UNCLAIMED)
	return(-1);
    sfsiotbl[fsfd].flags = SFS_UNCLAIMED;
    return(0);
}


/*
** The simple flash file system read routine
*/
int
sfs_read(int fsfd, unsigned char *bufptr, int numbytes)
{
    struct simp_fsio *sfsiop = &sfsiotbl[fsfd];

#ifdef DEBUG
printf("Entering sfs_read: fsfd: %d, bufptr: %#x, numbytes: %d, flag: %#x\n",
       fsfd,bufptr,numbytes,sfsiop->flags);
#endif

    if(fsfd >= MAXFSFD || !sfsiop->flags) {
	printf("sfs_read: read on invalid file descriptor\n");
	return(-1);
    }

#ifdef DEBUG
printf("sfs_read: bufptr = %#x, numbytes = %d\n", bufptr, numbytes);
printf("sfsiop->fptr = %#x, sfsiop->end = %#x\n", sfsiop->fptr, sfsiop->end);
#endif

    /*
    ** Make sure the user is not trying to read
    ** past the end of the file
    ** Adjust if necessary
    */
    if(sfsiop->fptr + numbytes > sfsiop->end) 
	numbytes = sfsiop->end - sfsiop->fptr;

    numbytes = raw_read(sfsiop->devnum, sfsiop->fptr, (char *)bufptr, numbytes);
    if(numbytes > 0) sfsiop->fptr += numbytes;  /* bump the file pointer */
    return(numbytes);
}


/*
** The simple flash file system write routine
*/
int
sfs_write(int fsfd, unsigned char *bufptr, int numbytes)
{
    return(-1);  /* not supported at this time */
}


/*
** The simple flash file system lseek routine
*/
int
sfs_lseek(int fsfd, long offset, int whence)
{
    char *ptr;
    struct simp_fsio *sfsiop = &sfsiotbl[fsfd];

    if(fsfd >= MAXFSFD || !sfsiop->flags) {
	printf("lseek on invalid file descriptor\n");
    } else {
	switch(whence) {
	case SEEK_CUR:
	    ptr = sfsiop->fptr + offset;
	    break;
	case SEEK_SET:
	    ptr = sfsiop->start + offset;
	    break;
	case SEEK_END:
	    ptr = sfsiop->end + offset;
	    break;
	default:
	    printf("lseek operation not supported\n");
	    return(-1);
	}
	if(ptr > sfsiop->end || ptr < sfsiop->start) {
	    printf("lseek out of bounds\n");
#ifdef DEBUG
printf("sfs_lseek: seeked file pointer = %#x, ", ptr);
printf("file start = %#x, file end = %#x\n", sfsiop->start, sfsiop->end);
#endif
	} else {
	    sfsiop->fptr = ptr;
	    /* return the current location in file */
	    return(ptr - sfsiop->start);
	}
    }
    return(-1);
}

/*
** The simple flash file system fstat routine
*/
int
sfs_fstat(int fsfd, struct stat_ *statptr)
{
    struct flash_file_hdr ffh;
    struct simp_fsio *sfsiop = &sfsiotbl[fsfd];
    char *phys_fptr;
    int size;

    if(!sfsiop->flags) {
	printf("fstat on invalid file descriptor\n");
	return(-1);
    }

    /* read in the header */
    if(raw_read(sfsiop->devnum, sfsiop->start - sizeof(ffh),
		(char *)&ffh, sizeof(ffh)) != sizeof(ffh)) {
	printf("fstat: trouble reading simple file header\n");
	return(-1);
    }
    statptr->devnum = sfsiop->devnum;
    statptr->st_size = ffh.ffh_len;
    statptr->crc = ffh.ffh_checksum;

    return(0);
}

/*
** The simple flash file system getdirent routine
** This routine assumes that the current file position
** (sfsiop->fptr) is pointing to the beginning of a file
** header (like right after the open of a raw device).
** It will leave the file pointer pointing to the next
** directory entry - ready for the next invokation.
** This will also ckeck the checksum of the file
*/
int
sfs_getdents(int fsfd, struct dirent_ *dptr, int dirent_len)
{
    struct flash_file_hdr ffh;
    unsigned char *start, *fptr;
    struct simp_fsio *sfsiop = &sfsiotbl[fsfd];
    char *phys_fptr;
    int size;

    if(fsfd >= MAXFSFD || !sfsiop->flags) {
	printf("getdirent on invalid file descriptor\n");
	return(0);
    }
    if(sfs_read(sfsiop->fsfd, (unsigned char *)&ffh,
		sizeof(ffh)) != sizeof(ffh)) {
	printf("getdirent: trouble reading simple flash file header\n");
	return(0);
    }

    if(ffh.ffh_magic == 0xffffffff && ffh.ffh_len == 0xffffffff)
	return(0);  /* the end of the file system */

    if(ffh.ffh_magic != FLASH_FHDR_MAGIC) {  /* check if magic */
	printf("getdirent: bad file magic number, possibly out of sync\n");
	return(0);
    }

    /* fill in the dirent structure */
    dptr->size = ffh.ffh_len;
    dptr->crc = ffh.ffh_checksum;
    dptr->fileno = -1;
    strcpy(dptr->d_name, ffh.ffh_name);
    dptr->flags = ffh.ffh_flags;
    dptr->filetype = SIMPLE_FS_TYPE;

    /*
    ** checksum the file
    */
    dptr->achecksum = sfs_fchksum(sfsiop->devnum, sfsiop->fptr, ffh.ffh_len);

    /* seek over file contents */
    if(sfs_lseek(sfsiop->fsfd, (long)(ffh.ffh_len + 3) & ~3, SEEK_CUR) < 0) {
	printf("getdirent: lseek error\n");
	return(0);
    }
    return(1);
}


/*
** A file checksum routine that runs fast.
** This was written for the Sierra Monitor because executing from
** EPROM is very slow.
** WARNING: kludgy stuff here!
** If the size is greater than FCHK_THRESHOLD, copy the routine to the 
** stack and execute it on the stack (through kseg0 (cached) if the
** stack is so configured).
** NOTE: fchksum_body must only be called with an even size
*/

#define FCHK_THRESHOLD 	0x10000
#define BUFFER_SIZE 	0x90000

unsigned short
fchksum_body(unsigned long chksum, unsigned short *ptr, int size)
{
    unsigned short *end;

    end = (unsigned short *)((int)ptr + size);
    /* we deal with even sizes only */
    while(ptr < end) {   /* calculate the checksum */
	chksum += *ptr++;
	if(chksum & ~0xffff)
	    chksum =  ++chksum & 0xFFFF;  /* end-around carry */
    }
    return(chksum);
}


/*
** The simple file checksum routine.
** Returns the checksum (unsigned short).
*/
unsigned short
sfs_fchksum(int devnum, char *logfptr, int size)
{
    char buffer[((int)&sfs_fchksum - (int)&fchksum_body)+4];
    unsigned short (*routine)(unsigned long, unsigned short *, int);
    unsigned long chksum;
    unsigned char *chksum_buf_ptr;
    int chunk;

    chksum = 0;
    chksum_buf_ptr = (unsigned char *)getfreememstart();

    routine = &fchksum_body;  /* init */

    if(size > FCHK_THRESHOLD) {
	movword(&fchksum_body, buffer, sizeof(buffer));
	cache_flush(0, -1);
	routine = (unsigned short(*)(unsigned long, unsigned short *, int))buffer;
    }

    while(size) {
	if(size >= BUFFER_SIZE) chunk = BUFFER_SIZE;
	else chunk = size;

	raw_read(devnum, logfptr, (char *)chksum_buf_ptr, chunk);
	chksum = (*routine)(chksum, (unsigned short *)chksum_buf_ptr, (chunk & ~1));
	if(chunk & 1) {    /* odd sized chunk */
	    chksum += (*(chksum_buf_ptr + (chunk - 1)) << 8);
	    if(chksum & ~0xffff)
		chksum =  ++chksum & 0xFFFF;  /* end-around carry */   
	}
	logfptr += chunk;
	size -= chunk;
    }
    return(~chksum & 0xFFFF);
}

/* end of module */





