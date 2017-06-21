/* $Id: cflow_write_coverage.c,v 3.5.20.1 1996/05/13 22:26:00 skoh Exp $
 * $Source: /release/112/cvs/Xsys/testtools/cflow_write_coverage.c,v $
 * ------------------------------------------------------------------
 * Various functions to implement controlflow coverage feature(cflow)
 * 
 * June-9 1994, Lakshmankumar Mukkavilli
 * 
 * Copyright (c) 1993-1997 by cisco Systems, Inc. All rights reserved.
 * 
 * ------------------------------------------------------------------
 * $Log: cflow_write_coverage.c,v $
 * Revision 3.5.20.1  1996/05/13  22:26:00  skoh
 * CSCdi57538:  Include ciscolib.h file for cflow
 * Branch: California_branch
 *
 * Revision 3.5  1996/01/31  18:24:12  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * Revision 3.4  1996/01/24  20:20:23  jenny
 * CSCdi47046:  TCP/IP need code clean up (part 3)
 * . removed ip/tuba.c, ip/tuba_default.c and ip/tuba.h
 * . removed all includes for ip/tuba.h.
 * . removed more TUBA related code.
 *
 * Revision 3.3  1996/01/22  07:29:39  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.2  1995/11/17  18:50:32  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:37:02  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.3  1995/07/16  18:04:28  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Bad programmer, no doggie biscuit.
 *
 * Revision 2.2  1995/07/15  02:37:19  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.1  1995/06/07  23:07:23  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 * ------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Work to be done:
 * 1  Lock bb_list_head before modifying the list. __bb_init_func
 *    will need to check the lock before adding anything to the list.
 * 2  Provide a choice of protocols for transferring cflow data.
 *    Currently only TFTP is supported.
 */

#include <ciscolib.h>
#include "master.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "ttysrv.h"
#include "file.h"
#include "sys_registry.h"
#include "../ip/tftp.h"
#include "packet.h"
#include "../ip/ip.h"
#include "name.h"
#include "../os/boot.h" /* needed for TFTP_MAXDATA definition */
#include "../ip/ip_registry.h"
#include "../testtools/cflow.h"

/*
 * MAXLEN  should be globally defined, but it isn't. See the c files
 * in ../tcpip
 */

#define MAXLEN 80

static boolean lcmpzero(long *, int);

static char cflow_default_filename[((MAX_FILENAME_LENGTH)+(MAX_FILENAME_SUFFIX_LENGTH))];
char ctrace_default_filename[((MAX_FILENAME_LENGTH)+(MAX_FILENAME_SUFFIX_LENGTH))];
/* 
 * cflow_saved_data is expected to be NULL when there are no saved
 * cflow data. So after freeing saved data this variable must be
 * set to NULL. At the same time set cflow_saved_data_size to zero.
 */
char *cflow_saved_data;
uint cflow_saved_data_size;
ipaddrtype cflow_tftphost;

typedef struct {
  char *filename;
  ipaddrtype address;
} hostinfo;

/*
 * cflow_get_tftphost_and_imagenames - Get the address of TFTP host
 *               (if image was booted from the host) and the imagename
 *      This need only be called once because the information
 *      provided by this function does not change until reboot. But
 *      there is no obvious lacation from which to call this function
 *      only once. Subsystem initialization is not the right place
 *      because sysconfg is not initialized by then. 
 */

void cflow_get_tftphost_and_imagenames (void)
{
/* Lengths of suffixes should be less than MAX_FILENAME_SUFFIX_LENGTH */
  char *cflow_filename_suffix = ".cflow.dat";
  char *ctrace_filename_suffix = ".ctrace.dat";
  static boolean already_called=FALSE;

  if (already_called){
    return;
  }
  already_called=TRUE;
  if (sysconfg.proto==FILE_ACCESS_TFTP){
    strncpy(cflow_default_filename,sysconfg.filename,
	    MAX_FILENAME_LENGTH-1);
    strncpy(ctrace_default_filename,sysconfg.filename,
	    MAX_FILENAME_LENGTH-1);
    /* append suffixes to filenames */
    strncat(cflow_default_filename,cflow_filename_suffix,
	    MAX_FILENAME_SUFFIX_LENGTH);
    strncat(ctrace_default_filename,ctrace_filename_suffix,
	    MAX_FILENAME_SUFFIX_LENGTH);
    cflow_tftphost=sysconfg.ps.tuba.respondent.ip_addr;
  }
}

/*
 * cflow_writetobuffer - copy coverage data to a buffer
 */

static void *
cflow_writetobuffer (uint *buffersize)
{
  struct bb      *bp;
  uint            space, cnt;
  uchar          *buf, *bufp;
  uint            version = 1;
  
  /*
   * Following block of code is intended to set some counters for modules
   * that contain malloc and bcopy to positive values. This hack is needed
   * because between the time we compute the size of the buffer and 
   * actually fill the buffer we invoke these functions. 
   * This is still not entirely satisfactory because these functions
   * could invoke others based on the values of function arguments. If
   * that is an issue we will deal with that.
   */
  {
    uchar *junk;
    junk=malloc(20);
    bcopy(&version, junk, sizeof(long));
    free(junk);
    
  }
  
  /* calculate how much space we need 
   * space for version and count of instrumented modules
   */
  space = 2 * sizeof(long);
  /*
   * for each module compute space for no.of instrumented arcs +
   * filename + NULL (string terminator)+ counts for each instrumented
   * arc
   */
  
  cnt = 0;
  for (bp = bb_list_head; bp; bp = bp->next) {
    /* Skip if all the counts are zero. Counts become zero
     * as a result of 'cflow clear' (or its API counterpart).
     */ 
    if (lcmpzero(&bp->counts[0], bp->ncounts) == TRUE) {
      continue;
    }
    space += (sizeof(long)) + (strlen(bp->filename) +1) + (sizeof(long)) +
      bp->ncounts * sizeof(long);
    cnt++;
  }
  /*
   * cnt indicates the number of instrumented modules. space is amount
   * of memory required to accumulate all the counts.
   */
  
  *buffersize=space;
  buf = bufp = malloc(space);
  if (buf == NULL) {
    printf("Cannot allocate %d bytes for collecting cflow data\n", space);
    flush();
    return (NULL);
  }
  /* copy the version number */
  bcopy(&version, bufp, sizeof(long));
  bufp += sizeof(long);
  
  /* copy the count of instrumented modules */
  bcopy(&cnt, bufp, sizeof(long));
  bufp += sizeof(long);
  
  for (bp = bb_list_head; bp; bp = bp->next) {
    int           len = strlen(bp->filename);
    
    /* Skip if all the counts are zero. Counts become zero as a result
     * of 'cflow clear' (or API counterpart).
     */ 
    if (lcmpzero(&bp->counts[0], bp->ncounts) == TRUE) {
      continue;
    }      
    bcopy(&len, bufp, (sizeof(long)));
    bufp += sizeof(long);
    /* Copy filename followed by NULL */
    bcopy(bp->filename, bufp, len + 1);
    bufp += len + 1;
    
    /* Copy instrumented arc count */
    bcopy(&bp->ncounts, bufp, sizeof(long));
    bufp += sizeof(long);
    
    /* Copy counts for every instrumented arc */
    bcopy(&bp->counts[0], bufp, sizeof(long) * bp->ncounts);
    bufp += sizeof(long) * bp->ncounts;
    
  }
  /* 
     Check that while we are copying cflow data some new data did
     not get created(happens when when a module is entered for first
     time)
     */
  if (((uint)bufp - (uint)buf) != space) {
    printf("\nPossible internal error - Pl. report to cflow support\n");
    printf("   space = %x, bufp = %x, buf = %x\n",space,(uint)bufp,(uint)buf);
  }
  flush();
  
  return (buf);
}

/*
 * cflow_move_buffer_to_file  - move contents of a buffer containing
 *                              cflow data to a file
 * 
 * The coverage file to be tftp'ed contains: 1 long containing a
 * version number, 1 long saying how many entries are in the file,
 * followed by, for each module which was instrumented: 1 long
 * containing the length of the module filename as given by strlen,
 * followed by the null-terminated filename, followed by 1 long
 * saying how many counts to expect, followed by that many counts.
 */

static boolean
cflow_move_buffer_to_file (hostinfo * host, char * buffer, uint buffersize)
{
  filetype        file;
  boolean         result; 
 
  memset(&file, 0, sizeof(filetype));
  file.ps.tuba.boothost.type = ADDR_IP;
  file.ps.tuba.boothost.ip_addr = host->address;
  file.proto = FILE_ACCESS_TFTP;
  file.ps.tuba.username = NULL;
  file.ps.tuba.display = FALSE;
  file.filename = host->filename;

  result = reg_invoke_file_write(file.proto, &file, buffer, buffersize);
  if (result) {
    printf("\nCopied %d bytes of cflow data to \n  %s on %i",
	   buffersize,host->filename, host->address);
    flush();
    free(buffer);
  }
  return (result);
}

/* cflow_writetofile_blockatatime - Write a block of memory to a
 *                file (one file-block at a time)
 */
/*
 * This function writes to a file using TFTP. It writes TFTP_MAXDATA
 * bytes at a time. If a block has less than TFTP_MAXDATA bytes then
 * TFTP assumes that it is the last block. That is why we keep
 * filling a filebuffer until it has TFTP_MAXDATA bytes. Unless the
 * block being processed is the last we will not write contents
 * of file buffer if it not full. It will be sent on subsequent
 * calls.
 */
static boolean cflow_writetofile_blockatatime (const void * start_address,
		      uint size, boolean last_block,ipsocktype *soc)
{
  static char file_buffer[TFTP_MAXDATA];
  static uint unused_capacity=TFTP_MAXDATA;
  static char *next_in_filebuffer=file_buffer;
  static uint block_number=1;
  char *next_in_datablock;
  uint size_yet_to_copy;
  uint size_tobe_copied;
  boolean result=TRUE;

  next_in_datablock=(void *)start_address;
  size_yet_to_copy=size;
  while (size_yet_to_copy!=0){
    size_tobe_copied=(unused_capacity >= size_yet_to_copy)
      ? size_yet_to_copy: unused_capacity;
    memcpy(next_in_filebuffer,next_in_datablock,size_tobe_copied);
    unused_capacity -= size_tobe_copied;
    next_in_filebuffer += size_tobe_copied;
    if (unused_capacity==0){
      /* We have TFTP_MAXDATA bytes in file buffer. write the block */
      result=reg_invoke_ip_tftp_sendblock(soc,block_number,file_buffer,
					  TFTP_MAXDATA,FALSE);
      flush();
      if (!result){
	printf("\n TFTP sendblock failed, blocknumber is %d",
	       block_number);	
	return (result);
      }
      unused_capacity=TFTP_MAXDATA;
      next_in_filebuffer=file_buffer;
      block_number++;
    }
    next_in_datablock+=size_tobe_copied;
    size_yet_to_copy-=size_tobe_copied;
  }
  if (last_block){ 
    /* Write the last block. Size varies from 0 to 511 */
    result=reg_invoke_ip_tftp_sendblock(soc,block_number,file_buffer,
			      (TFTP_MAXDATA-unused_capacity),FALSE);
    /* Prepare for next TFTP write opration. */
    unused_capacity=TFTP_MAXDATA;
    next_in_filebuffer=file_buffer;
    block_number=1;
  }
  return (result);
}

/*
 * cflow_scatter_gather_writetofile - Writes cflow data one
 *                       block at a time.
 */
/*
 * If there is not enough memory to first collect all the coverage data
 * in a buffer then this function can be used to write the coverage data 
 * one block at a time. This can be slow.
 */
static boolean cflow_scatter_gather_writetofile (hostinfo * host)
{
  ipsocktype     *soc;
  boolean         result;
  addrtype        faddr;
  struct bb      *bp;
  uint            cnt,file_count,space;
  uint            version = 1;

  /*
   * Prepare TFTP connection for writing cflow data.
   * It is important that this block of code should appear before
   * we count (in cnt) the number of files. This placement ensures
   * that TFTP related source files are included in the count.
   */
  faddr.type = ADDR_IP;
  faddr.ip_addr = host->address;
  soc = reg_invoke_ip_tftp_open_socket(&faddr, SOCK_ANYSRC);
  if (!soc) {
    printf("\nCant open socket to dump host %i", host->address);
    flush();
    return (FALSE);
  }
  printf("\nWriting %s ",host->filename);  
  flush();
  result= reg_invoke_ip_tftp_WRQ(soc,host->filename,"octet",FALSE);
  if (!result) {
    printf("\nCant send WRQ to %s", host->filename);
    flush();
    return (FALSE);
  } 

  /*
   * space contains the size of cflow data and cnt is the
   * number of files for which cflow data are being written.
   */
  space = 2 * sizeof(long);
  cnt = 0;
  for (bp = bb_list_head; bp; bp = bp->next) {
    /* Skip if all the counts are zero. Counts become zero
     * as a result of 'cflow clear' (or its API counterpart).
     */ 
    if (lcmpzero(&bp->counts[0], bp->ncounts) == TRUE) {
      continue;
    }
    cnt++;
    space += (sizeof(long)) + (strlen(bp->filename) +1) + (sizeof(long)) +
      bp->ncounts * sizeof(long);
  }
  
  /* write the version number */
  result=cflow_writetofile_blockatatime(&version,sizeof(long),
					FALSE,soc);
  if (!result) return (result);
  
  /* write the count of instrumented modules */
  result=cflow_writetofile_blockatatime(&cnt, sizeof(long),
					FALSE,soc);
  if (!result) return (result);

  file_count=0;
  for (bp = bb_list_head; bp; bp = bp->next) {
    int len = strlen(bp->filename);
    
    /* Skip if all the counts are zero. Counts become zero as a result
       of 'cflow clear' (or API counterpart).
       */ 
    if (lcmpzero(&bp->counts[0], bp->ncounts) == TRUE) {
      continue;
    }      
    result=cflow_writetofile_blockatatime(&len,(sizeof(long)),
					  FALSE,soc);
    if (!result) return (result);
    /* write filename followed by NULL */
    result=cflow_writetofile_blockatatime(bp->filename,len + 1,
					  FALSE,soc);      
    if (!result) return (result);
    /* write instrumented arc count */
    result=cflow_writetofile_blockatatime(&bp->ncounts,sizeof(long),
					  FALSE,soc);
    if (!result) return (result);
    /* write counts for every instrumented arc */
    result=cflow_writetofile_blockatatime(&bp->counts[0],
			   sizeof(long) * bp->ncounts,FALSE,soc);
    if (!result) return (result);  
    file_count++;      
  }
  /* Now indicate end of file */
  result=cflow_writetofile_blockatatime(NULL,0,
					  TRUE,soc);
  if (!result) return (result);      
  /* 
   *  Check that while we are writing cflow data some new data did
   *  not get created(happens when when a module is entered for first
   *  time)
   */
  if (cnt != file_count) {
    printf("\nPossible internal error - Pl. report to cflow support\n");
    printf("   cnt = %d, file_count = %d\n",cnt,file_count);
    printf(" In most cases this is not a serious error.");
    printf(" Please continue with your cflow work.");
  }
  flush();
  close_ipsocket(soc);
  printf("\nCopied %d bytes of cflow data(for %d files) to\n  %s on %i",
	 space,cnt,host->filename, host->address);
  flush();
  return (TRUE);
}

/*
 * cflow_parse_commandline - Read hostname(addr) and filenames from
 *                            user input(s)
 */

boolean
cflow_parse_commandline (parseinfo * csb, hostinfo * host)
{ 
  ipaddrtype      dumphost;
  int             parsed;
  char           *filename = "coverage.dat";
  
  cflow_get_tftphost_and_imagenames();
  if (GETOBJ(paddr, 1)->type == ADDR_IP) {
    dumphost = GETOBJ(paddr, 1)->ip_addr;
    parsed = TRUE;
  } else {
    if (cflow_tftphost!=0) {
      dumphost=cflow_tftphost;
      printf("\nRemote host[%i]? ", dumphost);
    } else {
      printf("\nRemote host?");
    }
    if ((parsed =  rdtty(GETOBJ(string, 2), MAXLEN))) {
      /* 
       * We are assuming that if parse_hostname cant parse the string
       * it wont disturb dumphost.
       */
      parsed = parse_hostname(GETOBJ(string, 2), &dumphost);
      /* If user hit only the <CR> and there is a default host use it */
      if ((!strlen(deblank(GETOBJ(string,2)))) && (cflow_tftphost!=0)) {
	parsed=TRUE;
      }
    }
  }
  if ((!parsed) || martian(dumphost, TRUE)) {
    printf("\n?Bad IP address or host name");
    return (FALSE);
  }
  /* Get coverage file name */
  
  if (*GETOBJ(string, 1) != '\0') {
    filename = GETOBJ(string, 1);
    parsed=TRUE;
  } else {
    if (cflow_default_filename[0]  != '\0') {
      filename=cflow_default_filename;
    }
    printf("\nName of coverage file to write [%s]? ", filename);
    if ((parsed = rdtty(GETOBJ(string, 1), MAXLEN))) {
      char  *cp;
      
      cp = deblank(GETOBJ(string, 1));
      if (cp && *cp)
	filename = cp;
    }
  }
  if (!parsed) {
    printf("%%Bad file name");
    return (FALSE);
  }
  printf("\nWrite file %s on host %i? ", filename, dumphost);
  if (!confirm(NULL))
    return (FALSE);
  host->filename=filename;
  host->address=dumphost;
  return (TRUE);
} 


/*
 * write_coverage_file - copy coverage data to host
 *              This function will be deleted soon.
 */

static boolean
write_coverage_file (char *filename, ipaddrtype dumphost)
{
  struct bb      *bp;
  uint            space, cnt;
  uchar          *buf, *bufp;
  ipsocktype     *soc;
  boolean         result;
  uint            version = 1;
  addrtype	    faddr;
    
  /* calculate how much space we need 
   * space for version and count of instrumented modules
   */
  space = 2 * sizeof(long);
  /*
   * for each module compute space for no.of instrumented arcs +
   * filename + NULL (string terminator)+ counts for each instrumented
   * arc
   */
  cnt = 0;
  for (bp = bb_list_head; bp; bp = bp->next) {
    space += (sizeof(long)) + (strlen(bp->filename) +1) + (sizeof(long)) +
      bp->ncounts * sizeof(long);
    cnt++;
  }
  /*
   * cnt indicates the number of instrumented modules. space is amount
   * of memory required to accumulate all the counts.
   */
    
  bufp = buf = malloc(space);
  if (bufp == NULL) {
    printf("Write coverage: cannot allocate %d bytes\n", space);
    flush();
    return (FALSE);
  }
  /* copy the version number */
  bcopy(&version, bufp, sizeof(long));
  bufp += sizeof(long);
  
  /* copy the count of instrumented modules */
  bcopy(&cnt, bufp, sizeof(long));
  bufp += sizeof(long);
  
  for (bp = bb_list_head; bp; bp = bp->next) {
    int           len = strlen(bp->filename);
    
    bcopy(&len, bufp, (sizeof(long)));
    bufp += sizeof(long);
    /* Copy filename followed by NULL */
    bcopy(bp->filename, bufp, len + 1);
    bufp += len + 1;
    
    /* Copy instrumented arc count */
    bcopy(&bp->ncounts, bufp, sizeof(long));
    bufp += sizeof(long);
    
    /* Copy counts for every instrumented arc */
    bcopy(&bp->counts[0], bufp, sizeof(long) * bp->ncounts);
    bufp += sizeof(long) * bp->ncounts;
    
  }
  
  faddr.type = ADDR_IP;
  faddr.ip_addr = dumphost;
  soc = reg_invoke_ip_tftp_open_socket(&faddr, SOCK_ANYSRC);
  if (!soc) {
    printf("\nCan't open socket to dump host %i", dumphost);
    free(buf);
    return (FALSE);
  }
  result = tftp_write_socket(soc, filename, (uchar *) buf,
			     "octet", space);
  close_ipsocket(soc);
  free(buf);
  return (result);
}


/*
 * cflow_writetofile - Write out a coverage file
 *                 This function will be deleted soon
 */

void
cflow_writetofile (parseinfo * csb)
{
    ipaddrtype      dumphost;
    int             parsed;
    char           *filename = "coverage.dat";
    
    if (GETOBJ(paddr, 1)->type == ADDR_IP) {
	dumphost = GETOBJ(paddr, 1)->ip_addr;
	parsed = TRUE;
    } else {
	printf("Remote host? ");
	if ((parsed =  rdtty(GETOBJ(string, 2), MAXLEN))) {
	    parsed = parse_hostname(GETOBJ(string, 2), &dumphost);
	}
    }
    if (!parsed || martian(dumphost, TRUE)) {
	printf("\n?Bad IP address or host name");
	return;
    }
    /* Get coverage file name */
    
    if (*GETOBJ(string, 1) != '\0') {
	filename = GETOBJ(string, 1);
	parsed++;
    } else {
	printf("\nName of coverage file to write to[%s]? ", filename);
	if ((parsed = rdtty(GETOBJ(string, 1), MAXLEN))) {
	    char  *cp;
	    
	    cp = deblank(GETOBJ(string, 1));
	    if (cp && *cp)
	      filename = cp;
	}
    }
    if (!parsed) {
	printf("%%Bad file name");
	return;
    }
    printf("\nWrite file %s on host %i? ", filename, dumphost);
    if (!confirm(NULL))
        return;
    
    if (write_coverage_file(filename, dumphost))
        printf(" [DONE]");
    else
        printf(" [Failed]");
}

/*
 * cflow_save - Save cflow data in a buffer
 */
void cflow_save (void)
{
    char *buffer;
    uint buffersize;

    if (cflow_saved_data != NULL) {
	free(cflow_saved_data);
	cflow_saved_data=NULL;
	cflow_saved_data_size=0;
	printf("\nPreviouly saved cflow data are being discarded");
	flush();
    }
    buffer=cflow_writetobuffer(&buffersize);
    if (buffer==NULL) {
	printf (" ['cflow save' FAILED]");
    } else {
	cflow_saved_data=buffer;
	cflow_saved_data_size=buffersize;
	printf(" ['cflow save' DONE]");
    }
    flush();
  }

/*
 * cflow_clear - Clear all cflow counts to zeros
 * It is assumed that each count is of type 'long'
 */
void cflow_clear (void)
{
    struct bb      *bp;
    for (bp = bb_list_head; bp; bp = bp->next) {
      /* Zero counts for every instrumented arc */
      memset(&bp->counts[0], 0, sizeof(long) * bp->ncounts); 
    }
    printf(" ['cflow clear' DONE]");
    flush();
  }

/*
 * cflow_killsaved - Free the buffer containing saved cflow data
 */
void cflow_killsaved (void)
{
    if (cflow_saved_data == NULL) {
	printf("\nThere are no saved data to kill [FAILED]");
    } else {
	free(cflow_saved_data);
	cflow_saved_data=NULL;
	cflow_saved_data_size=0;
	printf(" ['cflow killsaved' DONE]");
    }
    flush();
  }

/*
 * cflow_listfiles
 * Lists all the instrumented files that have been entered and
 * atleast one count is non-zero
 */
void cflow_listfiles (void)
{
    struct bb      *bp;
    for (bp = bb_list_head; bp; bp = bp->next) {
        if (lcmpzero(&bp->counts[0], bp->ncounts) != TRUE) {
        printf("\n%s",bp->filename);
        }
    }
    printf("\n [DONE]");
    flush();
  }

/*
 * cflow_listfiles_all
 * Lists all the instrumented files that have been entered
 */
void cflow_listfiles_all (void)
{
    struct bb      *bp;
    for (bp = bb_list_head; bp; bp = bp->next) {
        printf("\n%s",bp->filename);
        if (lcmpzero(&bp->counts[0], bp->ncounts) == TRUE) {
        	printf(" [zeros]");
        }
    }
    printf("\n [DONE]");
    flush();
  }

/*
 * cflow_display_status - Print various data about cflow
 */
void cflow_display_status (void)
{
    struct bb      *bp;
    uint number_of_files=0;
    uint number_of_files_with_zeros=0;
    boolean fastcopy_possible; 
    
    fastcopy_possible=TRUE;
    cflow_get_tftphost_and_imagenames();
    for (bp = bb_list_head; bp; bp = bp->next) {
	number_of_files++;
	if (lcmpzero(&bp->counts[0], bp->ncounts) == TRUE) {
	    number_of_files_with_zeros++;
	} 
    }
    printf("\nNumber of instrumented files touched so far = %d",
	   number_of_files);
    if (number_of_files_with_zeros!=0) {
        printf("\n Of these, execution counts for %d files are all zeros",
           number_of_files_with_zeros);
    }
    if (cflow_saved_data==NULL) {
	printf("\nThere are no saved cflow data.");
    } else {
	printf("\nThere are cflow data saved");
	printf("\n  The size of the saved cflow data is %d bytes.",
	       cflow_saved_data_size);
    }
    if (cflow_tftphost!=0) {
      printf("\nDefault tftphost for cflow data is %i",cflow_tftphost);
    } else {
      fastcopy_possible=FALSE;
    }
    if (cflow_default_filename[0] != '\0') {
      printf("\nThe default filename for cflow data is %s",
                        cflow_default_filename);
    } else {
      fastcopy_possible=FALSE;
    }
    if (!fastcopy_possible) {
      printf("\nFastcopy is not possible");
    }
    flush();
  }

/*
 * cflow_writetofile_saved - Move saved cflow data to a file
 */
void cflow_writetofile_saved (parseinfo *csb)
{
  hostinfo host;
  if (cflow_saved_data==NULL) {
    printf("\nThere are no saved cflow data [FAILED]");
    return;
  }
  if (cflow_parse_commandline(csb,&host)==FALSE){
    printf(" [FAILED]");
    return;
  }
  if (cflow_move_buffer_to_file(&host,cflow_saved_data,cflow_saved_data_size)) {
    cflow_saved_data=NULL;
    cflow_saved_data_size=0;
    printf(" [DONE]");
  } else {
    printf(" [FAILED]");
  } 

}

/*
 * cflow_copy - Copy cflow data to a file
 */
void cflow_copy (parseinfo *csb)
{
  hostinfo host;
  uint buffersize;
  uchar *buffer;
  
  if (cflow_parse_commandline(csb,&host)==FALSE) {
    return;
  }
  /*
   * First try to copy all cflow data to a buffer, if it does not
   * work then try a slower method that writes a block at a time
   */
  if ((buffer=cflow_writetobuffer(&buffersize))) {
    if (cflow_move_buffer_to_file(&host,
				 buffer,buffersize)) {
      printf(" [DONE]");
    } else {
      free(buffer);
      printf(" [FAILED]");
    }
  } else {
    printf("\nWe will try to write without using additional memory");
    if (cflow_scatter_gather_writetofile(&host)) {
      printf(" [DONE]");
    } else {
      printf(" [FAILED]");
    }
  }
  flush();
}

/*
 * cflow_fastcopy - Copy cflow data without user intervention
 */
void cflow_fastcopy (void)
{
  hostinfo host;
  
  cflow_get_tftphost_and_imagenames();
  if (cflow_tftphost==0) {
    printf("\nThere is no TFTPHOST - fastcopy can't be done");
    return;
  }
  host.address=cflow_tftphost;
  if (cflow_default_filename[0] == '\0') {
    printf("\nThe image was not netbootd via TFTP - fastcopy can't be done");
    return;
  }
  host.filename=cflow_default_filename;
  /*
   * We do not want to try to allocate a buffer and copy cflow data
   * there because this function could be called when heap is corrupted
   * (and hence malloc may not work.)
   */
  if (cflow_scatter_gather_writetofile(&host)) {
    printf("['cflow fastcopy' DONE]");
  } else {
    printf("['cflow fastcopy' FAILED]");
  }
  flush();
}

/*
 * cflow_fastcopy_saved - Copy saved cflow data without user intervention
 */
void cflow_fastcopy_saved (void)
{
    hostinfo host;

    cflow_get_tftphost_and_imagenames();
    if (cflow_tftphost==0) {
      printf("\nThere is no TFTPHOST - fastcopy can't be done");
      return;
    }
    host.address=cflow_tftphost;
    if (cflow_default_filename[0] == '\0') {
      printf("\nThe image was not netbootd via TFTP - fastcopy can't be done");
      return;
    }
    host.filename=cflow_default_filename;
    if (cflow_saved_data==NULL) {
      printf("\nThere are no saved cflow data [FAILED]");
      return;
    }
    if (cflow_move_buffer_to_file(&host,
             cflow_saved_data,cflow_saved_data_size)) {
      cflow_saved_data=NULL;
      cflow_saved_data_size=0;
      printf(" ['cflow fastcopy saved' DONE]");
    } else {
      printf(" ['cflow fastcopy saved' FAILED]");
    }
    flush();
  }

/*
 * lcmpzero
 * Check if a sequence of longs is all zeros.  Returns TRUE if all zeros,
 *              FALSE otherwise.
 */

static boolean lcmpzero(long *p, int longs)
{
  while (longs--) {
    if (*p++ != 0)
      return (FALSE);
  }
  return (TRUE);
}
