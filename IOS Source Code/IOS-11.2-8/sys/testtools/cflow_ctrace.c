/* $Id: cflow_ctrace.c,v 3.2.16.2 1996/05/13 22:25:58 skoh Exp $
 * $Source: /release/112/cvs/Xsys/testtools/cflow_ctrace.c,v $
 *------------------------------------------------------------------
 * Various functions to trace controlflow and _bb_init_func
 *
 * January 1996, Lakshmankumar Mukkavilli
 *
 * Copyright (c) 1996-1997 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: cflow_ctrace.c,v $
 * Revision 3.2.16.2  1996/05/13  22:25:58  skoh
 * CSCdi57538:  Include ciscolib.h file for cflow
 * Branch: California_branch
 *
 * Revision 3.2.16.1  1996/03/18  22:18:45  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.1.12.2  1996/03/16  07:43:47  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.1.12.1  1996/03/07  10:53:48  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.2  1996/02/14  05:49:33  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 * Revision 3.1  1996/01/31  18:24:08  lakshman
 * CSCdi47664:  Augment cflow with tracing capability
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * Among various functions in this file __bb_init_func is special.
 * __bb_init_func is called when a function is entered, if the
 * initialised flag, which is local to each module, is not set.
 * The name __bb_init_func is chosen by the compiler. Compiler
 * generates code to call this function.  bb_list_head points to
 * a linked list of bb structs which point to tables containing
 * execution counts for instrumented arcs in each module. This
 * file should NEVER be instrumented for coverage.
 *
 * For the purpose of obtaining function traces we extend the role of
 * __bb_init_func. All we need is a simple mechanism whereby we can
 * execute a set of actions at the start of every function. Since
 * __bb_init_func gets called at the start of every function (provided
 * zero_word in the bb structure that is passed is zero) we can
 * code the set of actions in __bb_init_func. We will leave zero_word 
 * to be zero and use a currently unused field (line_nums) in bb structure
 * for chaining bb structures. With this arrangement we are able to
 * obtain both coverage infomation and a controlflow trace. 
 *
 * If this feature proves to be useful we can have a new option to
 * the compiler to generate calls to a specified function at the
 * begining of every function(may be at exit as well). Then we can have
 * a controlflow trace without the overhead of test coverage(and vice
 * versa).
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
#include "../ip/ip_registry.h"
#include "../testtools/cflow.h"

#include "../os/gdb.h" /*for prototypes for breakpoint and gdb_kernel_enable*/
#include "globs.h"   /* for prototype for crashdump */

/*
 * MAXLEN  should be globally defined, but it isn't. See the c files
 * in ../tcpip
 */

#define MAXLEN 80

#define DEFAULT_CTRACE_SIZE 1024 /* Arbitrary choice */
/* Arbitrary choices of values*/
#define CTRACE_BUFFER_PREFIX1 0x8F50EC4D
#define CTRACE_BUFFER_PREFIX2 0xFF891A7B
#define CTRACE_BUFFER_SUFFIX1 0x5B18A03D
#define CTRACE_BUFFER_SUFFIX2 0xFC79BE24

/* CTRACE_SIZE indicates the number of entries(less one) in trace 
 * buffer
 */
#ifndef CTRACE_SIZE
#define CTRACE_SIZE (DEFAULT_CTRACE_SIZE)
#endif

#define CTRACE_SIZE_PLUS_ONE ((CTRACE_SIZE)+1)
#define CTRACE_SIZE_MINUS_ONE ((CTRACE_SIZE)-1)

/* Trace buffer contains addresss of the called function and the
 * caller. That is why we need twice as many slots.
 * The index of the caller is at :
 *    index for the called function + CTRACE_SIZE_PLUS_ONE
 * We leave the last slot unused. Normally this should remain unchanged.
 * It should be NULL. If it is not then there is some problem. 
 * This entry also serves as the end of buffer marker if wraparound is
 * not in use.
 */
#define CTRACE_BUFFER_SIZE (2*(CTRACE_SIZE_PLUS_ONE))

/* Prototypes */
/* ctrace_enable, ctrace_enable_nowrap, ctrace_disable, 
 * and ctrace_display_status are invoked by commands.
 */
void ctrace_enable(void);
void ctrace_enable_nowrap(void);
void ctrace_disable(void);
void ctrace_display_status(void);
static void ctrace_clear_tracebuffer(void);
static boolean write_ctrace_file(char *, ipaddrtype);
boolean ctrace_writetofile(parseinfo *);

/*
 * We initialize the following array to get them allocated in
 * initialized data segment. This will help get the trace from very
 * start. If this array got allocated in uninitialized data segment
 * we will lose some trace when the area is cleared to zeros as part
 * of IOS initialization. Another reason we want it in initialized
 * data segment is that we want to bracket the buffer with some 
 * known strings. These strings will help in extracting the buffer
 * from a dump. We are making a questionable assumption that all
 * these are going to allocated contiguously. Currently this
 * assumption is valid.
 */
static const uchar *ctrace_buffer_prefix1=(uchar *)CTRACE_BUFFER_PREFIX1;
static const uchar *ctrace_buffer_prefix2=(uchar *)CTRACE_BUFFER_PREFIX2;
static uchar *ctrace_buffer[CTRACE_BUFFER_SIZE]={NULL};
static const uchar *ctrace_buffer_suffix1=(uchar *)CTRACE_BUFFER_SUFFIX1;
static const uchar *ctrace_buffer_suffix2=(uchar *)CTRACE_BUFFER_SUFFIX2;
static ulong ctrace_buffer_next=0;
static ulong ctrace_number_of_wraparounds=0;
static ulong ctrace_count_of_missed_entries=0;
static semaphore ctrace_writeto_buffer=0;

/*
 * Usually it is a bad thing to initialize global variables. But here we
 * have a special situation. If we want trace to be enabled/disabled (with
 * or without wrap around) from the very start (before ctrace subsystem 
 * is initialized) then we have to do the initialization at compile time.
 * These settings can be changed subsequently.
 */

#ifdef CTRACE_OFF
static boolean ctrace_inactive_flag=TRUE;  /* TRUE - dont trace */
#else
static boolean ctrace_inactive_flag=FALSE;  /* FALSE -  trace */
#endif

#ifdef CTRACE_NOWRAP
/* no buffer wraparound */
static boolean ctrace_buffer_dont_wraparound=TRUE;
#else
/* do buffer wraparound */
static boolean ctrace_buffer_dont_wraparound=FALSE;
#endif

/* Head of a linked list of bb structures */
struct bb       *bb_list_head=NULL;

/* 
 * __bb_init_func
 * Compiler generates a call to this function at the start of every
 * function in an instumented .c file.
 * We will use this function to save the PC of the caller and the PC
 * of the caller's caller. Actually we can add any code in this function
 * that we want executed at the start of every function(ie. code to
 * check/validate contents of selected locations in memory).
 *
 * NO NON-INLINE FUNCTION CALLS SHOULD APPEAR BEFORE "SAVE_CALLER()"
 */

void __bb_init_func (struct bb * blocks)
{
  
    /* Attach the bb block to the list of bb blocks first time this routine
     * is entered. Head of the list is at bb_list_head.
     */
    if (!blocks->line_nums) {
	blocks->next = bb_list_head;
	bb_list_head = blocks;
	blocks->line_nums = (long *)1;
    }
    
    /*
     * If ctrace is active then place the PC of the caller  and the PC
     * of the caller's caller in the trace buffer. 
     * Increment the index to next entry in the buffer.
     * Since the tracebuffer is a circular buffer we need to wraparound
     * the value of index. We move NULL to next location in the buffer
     * because the program(on the host) that will process the contents of
     * the buffer will know the location of the oldest entry.
     */
    if (!ctrace_inactive_flag){
	/*
	 * If someone else is in critical section then just return. This may
	 * mean we will skip an entry. We are relying on the fact that 
	 * lock_semaphore() is an inline function.
	 */
    
	if (!lock_semaphore(&ctrace_writeto_buffer))
	    {
		ctrace_count_of_missed_entries++; 
		return;
	    };
    
	/* We will treat m68k objects differently because we can avoid some
	 * checks and we can get trace from the very start.
	 */
#ifdef OBJECT_68
	{
	    ulong *frame1, *frame2; 
	    frame1 = caller_frame();
	    frame2 = (ulong *) (*frame1);
	    ctrace_buffer[ctrace_buffer_next] = (uchar *) *(frame1+1);
	    ctrace_buffer[ctrace_buffer_next+CTRACE_SIZE_PLUS_ONE]
		= (uchar *) *(frame2+1);
	}
	/* Following chunk of code won't work if kernel (specifically code
	 * that is executed at interrupt level) is instrumented on 4k
	 * platforms.
	 */
#elif defined CTRACE_NOKERNEL
	{
	    ulong *frame1, *frame2, *frame3, *pc; 
	    pc = (ulong *)__bb_init_func;
	    frame1 = caller_frame();
	    frame2 = trace_caller(frame1, &pc);
	    if (frame2) {
		ctrace_buffer[ctrace_buffer_next] = (uchar *)pc;
		frame3 = trace_caller(frame2, &pc);
		if (frame3) {
		    ctrace_buffer[ctrace_buffer_next+CTRACE_SIZE_PLUS_ONE] = (uchar *)pc;
		} else {
		    ctrace_buffer[ctrace_buffer_next+CTRACE_SIZE_PLUS_ONE] = NULL;
		}
	    } else {
		/* This can happen only at the very begining, before memory  
		 * region is initialized. These NULL entries provide a count of
		 * function calls before region initialization. It is possible
		 * this count is useless. We will wait and see.
		 */
		ctrace_buffer[ctrace_buffer_next] = NULL;
		ctrace_buffer[ctrace_buffer_next+CTRACE_SIZE_PLUS_ONE] = NULL;
	    }
	}
#else
	{
	    SAVE_CALLER();
	    ctrace_buffer[ctrace_buffer_next] = (uchar *)caller();
	    /* We need to develop an efficient means of obtaining callers caller
	     * information for 4k platforms. This function should be a leaf
	     * function. Perhaps it needs to be in assembler. Until then
	     * we cant get caller's caller for 4k platforms if kernel is 
	     * instrumented.
	     */
      
	}
#endif
	
	ctrace_buffer_next++;
	/*
	 * We can add code here to do any processing
	 * that we want to do at the start of every function.
         * This should be short.
	 */
        /*
         * An example: Qhead was getting corrupted after running for 
         * several hours. We used the following code segment to break
         * soon after corruption happened. We saved the correct value of
         * Qhead in Qhead_correct_value and enabled ctrace. We could have
         * used a call to crashdump(0) if we wanted a dump instead
         * of a GDB beakpoint.

          if (Qhead != Qhead_correct_value) {
                ctrace_inactive_flag=TRUE;
                ctrace_buffer[ctrace_buffer_next] = NULL;
		ctrace_fastcopy();
		gdb_kernel_enable();
		(void) breakpoint();
          }
         */

	if (ctrace_buffer_next >= CTRACE_SIZE) {
	    if (ctrace_buffer_dont_wraparound) {
		ctrace_inactive_flag = TRUE;
	        unlock_semaphore(&ctrace_writeto_buffer);
		return;
	    }
	    ctrace_buffer_next = 0;
	    ctrace_number_of_wraparounds++;
	}
	unlock_semaphore(&ctrace_writeto_buffer);
    }
    return;
}

/*
 * ctrace_write_entry - Place an entry in ctrace buffer.
 * __bb_init_func places an entry automatically when an
 * instrumented function is entered (and ctrace is enabled)
 * ctrace_write_entry can be called from anywhere to place
 * an entry in the trace buffer.
 */
boolean ctrace_write_entry (uchar *caller, uchar *callers_caller)
{
    /*
     * Whether or not ctrace is active place the parameters in the 
     * trace buffer. Increment the index to next entry in the buffer.
     * Since the tracebuffer is a circular buffer we need to wraparound
     * the value of index.
     */
    /*
     * If someone else is in critical section then just return. This may
     * mean we will skip an entry. 
     */
    if (!lock_semaphore(&ctrace_writeto_buffer))
	{
	    ctrace_count_of_missed_entries++; 
	    return (FALSE);
	}
  
    ctrace_buffer[ctrace_buffer_next] = caller;
    ctrace_buffer[ctrace_buffer_next+CTRACE_SIZE_PLUS_ONE] = callers_caller; 
    ctrace_buffer_next++;
    if (ctrace_buffer_next >= CTRACE_SIZE) {
	if (ctrace_buffer_dont_wraparound) {
	    ctrace_inactive_flag = TRUE;
            unlock_semaphore(&ctrace_writeto_buffer);
	    return (TRUE);
	}
	ctrace_buffer_next = 0;
	ctrace_number_of_wraparounds++;
    }
    unlock_semaphore(&ctrace_writeto_buffer);
    return (TRUE);
}

/*
 * ctrace_enable
 */
void ctrace_enable (void)
{
    ctrace_start();
    ctrace_display_status();
}

/*
 * ctrace_start
 */
void ctrace_start (void)
{
  ctrace_clear_tracebuffer();
  ctrace_buffer_dont_wraparound = FALSE;
  ctrace_inactive_flag = FALSE;
}

/*
 * ctrace_enable_nowrap - Enable tracing without buffer wraparound
 */
void ctrace_enable_nowrap (void)
{
  ctrace_start_nowrap();
  ctrace_display_status();
}

/*
 * ctrace_start_nowrap - Enable tracing without buffer wraparound
 */
void ctrace_start_nowrap (void)
{
  ctrace_clear_tracebuffer();
  ctrace_buffer_dont_wraparound = TRUE;
  ctrace_inactive_flag = FALSE;
}

/*
 * ctrace_disable 
 */
void ctrace_disable (void)
{
  ctrace_stop();
  ctrace_display_status();
}

/*
 * ctrace_stop - Stop ctracing.
 */
void ctrace_stop (void)
{
  ctrace_inactive_flag = TRUE;
}

/*
 * ctrace_dont_wraparound - Disable wraparound of ctrace buffer
 */
void ctrace_dont_wraparound (void)
{
  ctrace_buffer_dont_wraparound = TRUE;
}

/*
 * ctrace_clear_tracebuffer 
 */
static void ctrace_clear_tracebuffer (void)
{
  ulong index;
  
  for (index = 0; index <= CTRACE_SIZE; index++)
    {
      ctrace_buffer[index] = NULL;
      ctrace_buffer[index+CTRACE_SIZE_PLUS_ONE] = NULL;
    }
  ctrace_buffer_next = 0;
}

/*
 * write_ctrace_file - Transfer ctrace buffer to a file on a host machine
 */
static boolean write_ctrace_file (char *filename, ipaddrtype dumphost)
{
  uint            buffersize_inbytes; 
  boolean         result;
  boolean         save_ctrace_inactive_flag;
  filetype        file;
  
  /* Disable tracing till write is complete. */
  save_ctrace_inactive_flag = ctrace_inactive_flag;
  ctrace_inactive_flag = TRUE;

  memset(&file, 0, sizeof(filetype));
  file.ps.tuba.boothost.type = ADDR_IP;
  file.ps.tuba.boothost.ip_addr = dumphost;
  buffersize_inbytes = CTRACE_BUFFER_SIZE * (sizeof (uchar *));
  printf("\nctrace file size is %u bytes",buffersize_inbytes);
  /* To let the program that processes the trace file locate the
   * oldest entry we will insert a null.    
   */
  ctrace_buffer[ctrace_buffer_next] = NULL;

  file.proto = FILE_ACCESS_TFTP;
  file.ps.tuba.username = NULL;
  file.ps.tuba.display = FALSE;
  file.filename = filename;
  result = reg_invoke_file_write(file.proto, &file, (char *) ctrace_buffer,
				 buffersize_inbytes);

  ctrace_inactive_flag = save_ctrace_inactive_flag;
  return (result);
}

/*
 * ctrace_fastcopy - Copy ctrace data without user intervention
 */
void ctrace_fastcopy (void)
{
    cflow_get_tftphost_and_imagenames();
    if (cflow_tftphost==0) {
	printf("\nThere is no TFTPHOST - fastcopy can't be done");
	return;
    }
    if (ctrace_default_filename[0] == '\0') {
	printf("\nThe image was not netbootd via TFTP - fastcopy can't be done");
	return;
    }
    if (write_ctrace_file(ctrace_default_filename,
	     cflow_tftphost)) {
	printf("\n ['ctrace fastcopy' DONE]");
    } else {
	printf("\n ['ctrace fastcopy' FAILED]");
    }
    flush();
}

/*
 * ctrace_writetofile - Write ctrace buffer to a file
 */

boolean
  ctrace_writetofile (parseinfo * csb)
{
  ipaddrtype      dumphost;
  int             parsed;
  char           *filename = "ctrace.dat";
    
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

  /* Get ctrace file name */
  
  if (*GETOBJ(string, 1) != '\0') {
    filename = GETOBJ(string, 1);
    parsed=TRUE;
  } else {
    if (ctrace_default_filename[0]  != '\0') {
      filename=ctrace_default_filename;
    }
    printf("\nName of ctrace file to write to[%s]? ", filename);
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

  if (write_ctrace_file(filename, dumphost)) {
    printf(" [DONE]");
    return (TRUE);
  }  else {
    printf(" [FAILED]");
    return (FALSE);
  }
}

/*
 * ctrace_display_status - Display ctrace status
 */
void ctrace_display_status (void)
{
    boolean fastcopy_possible; 
    
    fastcopy_possible=TRUE;
    cflow_get_tftphost_and_imagenames();
    if (ctrace_inactive_flag) {
      printf("\nctrace is not active");
    } else {
      printf("\nctrace is active");
      if (ctrace_buffer_dont_wraparound) {
	printf("\nBuffer wraparound is not in effect");
      } else {
	printf("\nBuffer wraparound is in effect");
      }
    }
    printf("\nTrace buffer has room for %u entries, next entry will be at %u",
	   CTRACE_SIZE, ctrace_buffer_next);
    printf("\nEntry in next slot %x", ctrace_buffer[ctrace_buffer_next]);
    printf("\nNumber of wraparounds = %u", ctrace_number_of_wraparounds);
    printf("\nNumber of missed entries = %u", ctrace_count_of_missed_entries);
    if ((ctrace_buffer_prefix1 != (uchar *)CTRACE_BUFFER_PREFIX1) ||
	(ctrace_buffer_prefix2 != (uchar *)CTRACE_BUFFER_PREFIX2)) { 
      printf("\nTracebuffer prefix is corrupted");
    }
    if ((ctrace_buffer_suffix1 != (uchar *)CTRACE_BUFFER_SUFFIX1) ||
	(ctrace_buffer_suffix2 != (uchar *)CTRACE_BUFFER_SUFFIX2)) { 
      printf("\nTracebuffer suffix is corrupted");
    }
    if (cflow_tftphost!=0) {
      printf("\nDefault tftphost for ctrace data is %i",cflow_tftphost);
    } else {
      fastcopy_possible=FALSE;
    }
    if (ctrace_default_filename[0] != '\0') {
      printf("\nThe default filename for ctrace is %s",
	     ctrace_default_filename);
    } else {
      fastcopy_possible=FALSE;
    }
    if (!fastcopy_possible) {
      printf("\nFastcopy is not possible");
    }
  }
