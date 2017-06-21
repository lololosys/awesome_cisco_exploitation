/* $Id: lpd.h,v 3.4.38.1 1996/06/11 04:19:29 bjthomas Exp $
 * $Source: /release/112/cvs/Xsys/ts/lpd.h,v $
 *------------------------------------------------------------------
 * lpd.h - definitions for berkeley line printer daemon
 *
 * September,1994  Bill Westfield
 *
 * Copyright (c) 1994-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lpd.h,v $
 * Revision 3.4.38.1  1996/06/11  04:19:29  bjthomas
 * CSCdi55134:  LPD doesnt flush data of a cancelled job
 * Branch: California_branch
 *
 * Revision 3.4  1996/01/09  10:59:22  billw
 * CSCdi45881:  LPD aborts jobs identified as postscript by host
 *
 * Revision 3.3  1996/01/09  09:09:41  billw
 * CSCdi44091:  lpd should be able to output formfeed between jobs
 *
 * Revision 3.2  1995/11/17  18:55:14  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:39:34  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.2  1995/08/03  23:17:52  billw
 * CSCdi37670:  bugs in lpd cause remote server to shut down printer
 *
 * Revision 2.1  1995/06/07  23:09:45  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * cisco specific parameters
 */
#define LPD_TTYWAIT 5		/* time to wait for tty_allocate */
#define LPD_BUF_SIZE 512
#define LPD_MIN_TOP_CMD_SIZE 3	/* min top level cmd size */
#define LPD_MAX_PORT 1024

/*
 * Commands
 */
#define LPD_CMD_PRINT  1		/* Start printing */
#define LPD_CMD_RECV   2		/* Receive a print job */
#define LPD_CMD_STAT   3		/* Send queue state (short) */
#define LPD_CMD_STATL  4		/* Send queue state (long) */
#define LPD_CMD_RM     5		/* Remove job */

/*
 * Receive Job subcommands
 */
#define LPD_JOB_ABORT  1		/* Abort job */
#define LPD_JOB_CTRL   2		/* Receive control file */
#define LPD_JOB_DATA   3		/* Receive data file */

/*
 * Control file lines
 */

#define LPD_CF_CLASS  'C'		/* Class name */
#define LPD_CF_HOST   'H'		/* Host name */
#define LPD_CF_JOB    'J'		/* Job name */
#define LPD_CF_BANNER 'L'		/* Print banner page */
#define LPD_CF_USER   'P'		/* User ID*/
#define LPD_CF_TEXT   'f'		/* Plain text file */
#define LPD_CF_RAW    'l'		/* Plain text with ctrls */
#define LPD_CF_POST   'o'		/* Postscript file */
/*
 * LPD Response codes.
 * These are not defined in the RFC.  Read the berkeley code.
 * It's not clear that -1 is supposed to be a response code, but
 * it does the "right thing" because of sign extension in the unix code.
 */
#define LPD_R_OK    '\0'	/* ACK code */
#define LPD_R_FAST  -1		/* Fast retry - simulate connection close. */
#define LPD_R_RETRY  '\1'	/* Retry job */
#define LPD_R_SLOW   '\2'	/* Queue drain wait (5 minutes) */

/*
 * The following is stuff we don't care about, and will ignore.
 * Other commands not listed here will result in errors..
 */

#define LPD_CF_IGNORE "IMNSTUW1234"
#define LPD_STRSIZE 20

/* structure for each instance (rotary/line) of a named printer queue */

typedef struct printer_instance_ {

    struct printer_instance_ *next;

    struct printer_type_ *lpt;
    /*
     * These fields filled in by the lpd daemon itself.
     */
    tcbtype *tcb;		/* tcp connection data */
    char jobnum[6];		/* Job number (normally three digits) */
    char hostname[LPD_STRSIZE];	/* Host who is printing to us */
    char username[LPD_STRSIZE];	/* User who originated the job */
    char classname[LPD_STRSIZE];/* What class they are in (for banner) */
    long datasize;		/* size of data to be printed */
    long dataleft;		/* How much is left to print */
    int i_ttynum;		/* Line actually used */
    ulong i_flags;		/* Random bits - see below */

    ulong i_ncontrol;
    ulong i_ndata;
    ulong i_errors;
    ulong i_outbytes;
} printer_instance;

/*
 * Definition of bits in i_flag word of printer_instance data structure
 */

#define LPD_F_ABORT   1		/* Abort the job */

#define LPD_WHITE_SP(p) (*p==' '||*p==CH_HT||*p==CH_VT||*p==CH_FF)

/* macro which returns the next printer_instance 
 */
#define NEXT_I_LPT(n) for(n=(printer_instance*)lpt->i_lptQ.qhead;n; \
	n=n->next)

/* macro which skips to the next field of a lpd list
 */
#define NEXT_LPD_FIELD(p) for(;(*p!=' '&&*p!=CH_HT&&*p!=CH_VT&&*p!=CH_FF&& \
	*p!='\0');p++)

typedef struct printer_type_ {
    /*
     * these fields filled in by the user, saved to NVM, etc.
     */
    struct printer_type_ *next;
    NAMEDTHINGMACRO		/* this is lpt->name */
    int ttynum;			/* Line number if configed ie, none rotary*/
    char jobtypes[10];		/* Types of job we'll accept */
    char *bannerpage;		/* potential banner page string */
    int access_class;		/* Who can connect to this printer */
    int rotary;			/* line rotary group to use */

    ulong flags;
    int acq_timeout;		/* work-around for AIX and slow printers */

    queuetype i_lptQ;		/* queue of printer instance structures */

    /*
     * Since-configuration history
     */
    ulong ntcpconn;
    ulong ncontrol;
    ulong ndata;
    ulong errors;
    ulong outbytes;
} printer_type;


/*
 * Definition of bits in flag word of printer_type data structure
 */

#define LPD_F_BANNER  2		/* Print a banner page */
#define LPD_F_TEXT    4		/* discard control characters */
#define LPD_F_RAW     8		/* Raw mode */
#define LPD_F_POST   16		/* Postscript */
#define LPD_F_ACTIVE 32		/* Printer is in use */
#define LPD_F_CFIRST 64		/* lpd must receive ctrl file before data */
#define LPD_F_FFEED 128		/* Output a ^L in between jobs */
