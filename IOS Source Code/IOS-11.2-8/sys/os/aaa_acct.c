/* $Id: aaa_acct.c,v 3.13.10.10 1996/08/28 13:02:28 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/aaa_acct.c,v $
 *------------------------------------------------------------------
 * aaa_acct.c System accounting functions
 *
 * January 1995,   Bill Westfield
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: aaa_acct.c,v $
 * Revision 3.13.10.10  1996/08/28  13:02:28  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.13.10.9  1996/07/23  00:21:31  perryl
 * CSCdi60963:  %ALIGN-1-FATAL ,addr=0x1, pc=0x607945B0, ra=0x601A8010,
 * sp=0x60DA0AD
 * Branch: California_branch
 * Correct the acct_data buffer boundary check in aaa_attr_add
 * Redo aaa_acct_print() so it can handle large accounting buffer.
 *
 * Revision 3.13.10.8  1996/07/10  22:21:21  dstine
 * CSCdi62471:  Superfluous #include files in atalk, atm, os files
 * Branch: California_branch
 *
 * Revision 3.13.10.7  1996/05/28  16:00:21  vandys
 * CSCdi58491:  Require accounting event for L2F tunnel creation
 * Branch: California_branch
 *
 * Revision 3.13.10.6  1996/05/21  09:57:58  thille
 * CSCdi51599:  multiple /interface/ literals bloat code.
 * Branch: California_branch
 * Save another 392 bytes by doing away with duplicate literals.
 *
 * Revision 3.13.10.5  1996/05/07  00:51:55  billw
 * CSCdi56455:  AAA Accounting generates incorrect values of Service
 * attribute
 * Branch: California_branch
 *
 * Revision 3.13.10.4  1996/05/07  00:41:48  billw
 * CSCdi56552:  AAA System Accounting broken in 11.1
 * Branch: California_branch
 *
 * Revision 3.13.10.3  1996/04/19  15:24:19  che
 * CSCdi55138:  IosSec_branch feature commit
 * Branch: California_branch
 * IOS Security feature commit
 *
 * Revision 3.13.14.2  1996/04/10  23:07:37  lol
 * Branch: IosSec_branch
 *         Incorporate code review comments.
 *
 * Revision 3.13.14.1  1996/04/09  21:25:45  che
 * Branch: IosSec_branch
 * Sync to California_branch (960405)
 *
 * Revision 3.13.10.2  1996/03/27  08:33:00  billw
 * CSCdi49184:  AAA accounting records do not contain IP addr if
 * autoselected
 * Branch: California_branch
 *
 * Revision 3.13.10.1  1996/03/18  21:28:39  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.9.6.3  1996/03/16  07:20:07  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.9.6.2  1996/03/07  10:17:37  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.9.6.1  1996/02/20  16:36:35  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.13  1996/02/22  14:36:25  widmer
 * CSCdi49281:  make all parser_extension_request static const
 * make parser_extension_request typedef const
 * make all parser_extension_request static
 *
 * Revision 3.12  1996/02/11  09:20:59  billw
 * CSCdi47099:  not nearly enough processes call process_set_crashblock
 *
 * Revision 3.11  1996/02/09  04:17:27  che
 * CSCdi47978:  aaa acounting crashes router with vty async
 * Needed to make sure the queue head was not cleared by another
 * process in acct_cleanup().
 *
 * Revision 3.10  1996/02/01  06:07:26  hampton
 * Migrate files out of the parser directory.  [CSCdi47717]
 *   Step 2: Change all files which reference the moved parser files to
 *   point to their new locations.
 *
 * Revision 3.9  1996/01/12  02:40:56  billw
 * CSCdi44858:  use #define for AAA attribute values
 * Bring the AV pairs used up to spec level...
 *
 * Revision 3.8  1996/01/09  20:15:01  billw
 * CSCdi46519:  AAA accounting STOP records not sent via tacacs+ (regres
 * CSCdi40008)
 *
 * Revision 3.7  1995/12/17  18:34:02  thille
 * CSCdi45760: Parser needs to go on a diet.
 * Move object files that are not part of parser to os_exec or libutil.a
 * as appropriate.  Rename parser files to make it clear what is part of
 * parser and avoid other stuff accumulating.  Modify files that include
 * parser .h files to deal with new names.
 *
 * Revision 3.6  1995/12/13  02:59:35  billw
 * CSCdi42536:  aaa_acct not proper subsystem
 * Move some code around, change some calls to reg_invokes, etc.
 *
 * Revision 3.5  1995/11/29  18:53:08  syiu
 * CSCdi40008:  memory leaking in AAA ustruct when using PPP with CHAP/PAP
 *
 * Revision 3.4  1995/11/20  10:46:39  billw
 * CSCdi43781:  AAA Accounting not done for incoming tty daemon connections
 *
 * Revision 3.3  1995/11/17  18:44:10  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:16:36  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:43:31  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.22  1995/11/08  21:18:12  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.21  1995/10/16  22:57:16  snyder
 * CSCdi42258:  spellink errors
 *              unacccounted -> unaccounted
 *
 * Revision 2.20  1995/10/14  01:13:28  billw
 * CSCdi41910:  Use incrementing number for accounting task id
 * Also sneak in use of correct interface to getting timezone name
 *
 * Revision 2.19  1995/09/16  21:42:54  carrel
 * CSCdi40334:  Bus error crash in AAA accounting code
 * Removed unnecessary NULL ptr checks.
 * Added some needed NULL ptr checks!!
 * Made a call to acct_free() be safe!!
 * Removed unneeded call to aaa_dup_user().
 * Generally made the code prettier.  ;-)
 * Removed unused arg from aaa_dup_ifneeded().
 *
 * Revision 2.18  1995/08/31  01:32:58  billw
 * CSCdi39522:  AAA Accounting not portability-safe
 * Change a bunch of "int" to "long" or "ulong"
 *
 * Revision 2.17  1995/08/29  04:14:49  billw
 * CSCdi39403:  AAA Accounting: No start_time attribute
 * Include configured timezone as well.
 *
 * Revision 2.16  1995/08/11  07:58:47  billw
 * CSCdi38516:  AAA Accounting System uses bogus magic names
 * Use null username and port instead.
 *
 * Revision 2.15  1995/07/26  14:14:54  billw
 * CSCdi37036:  No AAA Accounting records generated for system events
 * generate "system" records for startup, shutdown, and change in the
 * state of "aaa accounting system" configuration.
 *
 * Revision 2.14  1995/07/21  01:47:27  billw
 * CSCdi36156:  No AAA accounting messages for ARAP or Xremote
 *
 * Revision 2.13  1995/07/18  00:56:53  billw
 * CSCdi36940:  AAA should keep cumulative statistics
 * This patch does accounting stats only...
 *
 * Revision 2.12  1995/07/14  07:02:39  hampton
 * Convert Terminal Server to fully use the passive timers macros instead
 * of referencing the system clock directly.  [CSCdi37093]
 *
 * Revision 2.11  1995/07/13 06:45:46  billw
 * CSCdi36954:  AAA accounting done improperly when exec login command is
 * used
 *
 * aaa_dup_user no longer duplicates acctQ.  New registry point for
 * changing from one username to another.
 *
 * Revision 2.10  1995/07/12  21:10:42  billw
 * CSCdi36937:  aaa accounting buginfs not formatted consistantly
 * Use AAA/ACCT: lead-in
 *
 * Revision 2.9  1995/06/30  05:21:33  hampton
 * Move externs for the random number generator to their own file.  Remove
 * redundant initialization of the random number generator.  [CSCdi36588]
 *
 * Revision 2.8  1995/06/28 09:18:44  smackie
 * Finally make subsystem sequencing robust with multiple hierarchy
 * sequences. Tidy up subsystem code into the bargain and split the
 * parser code into a new file. Make the subsystem code far noisier
 * about bogusness. (CSCdi23568)
 *
 * Revision 2.7  1995/06/24  20:33:42  billw
 * CSCdi35959:  AAA Accounting doesnt provide SERVICE avpair for all
 * records
 * Also move elapsed time to be last AV pair in packet.
 *
 * Revision 2.6  1995/06/22  18:24:05  billw
 * CSCdi35818:  AAA Accounting buginfs should be based on debug aaa
 * accounting.
 *
 * Revision 2.5  1995/06/15  22:57:45  syiu
 * CSCdi35686:  Async callback AAA authorization and accounting phase not
 * working
 *
 * Revision 2.4  1995/06/14  07:17:13  billw
 * CSCdi35096:  TACACS+ Accounting records should have elapsed time in
 * seconds
 *
 * Revision 2.3  1995/06/14  06:30:42  billw
 * CSCdi35380:  AAA accounting causes crashes
 * Clean up aaa_dup_user and callers, correct freeing of non-malloced
 * memory, and a couple other things.
 *
 * Revision 2.2  1995/06/09  13:11:56  hampton
 * Rename old scheduler calls to fit the new naming convention.
 * [CSCdi35514]
 *
 * Revision 2.1  1995/06/07 21:53:02  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "ttysrv.h"
#include "packet.h"
#include "subsys.h"
#include "sys_registry.h"
#include "interface_private.h"
#include "config.h"
#include "parser.h"
#include "../parser/parser_actions.h"
#include "connect.h"
#include "aaa.h"
#include "../if/network.h"
#include "../wan/ppp.h"
#include "../ui/debug.h"
#include "../os/os_debug_flags.h"
#include "logger.h"
#include "../os/msg_aaa.c"		/* Not a typo, see logger.h */
#include "../h/auth_registry.regh"
#include "../os/clock.h"
#include "../wan/ppp_registry.regh"

#define AAA_MAJVERSION 1
#define AAA_MINVERSION 0
#define AAA_EDITVERSION 1

static queuetype acct_bkg_queue;
static int acct_bkg_fork;

/*
 * data structures to tell us what sort of accounting has been turned on
 */

extern boolean novell_running;
static void show_accounting (parseinfo *);
void aaa_attr_add (acctinfo*, int, void *, int, int);
static acctinfo *acct_search_create_type(userstruct *, short, boolean);

static boolean aaa_reload_finished;	/* Has the system run the restarted registry */

static ulong aaa_acct_frees, aaa_acct_mallocs;
static ulong aaatraf_acctstarts[ACCT_TYPE_MAX],
             aaatraf_acctstops [ACCT_TYPE_MAX],
             aaatraf_acctupdate[ACCT_TYPE_MAX],
             aaatraf_acctactive[ACCT_TYPE_MAX];

static ulong task_seed = 0;


/* Accounting support */

/*
 * Macro to create boolean TRUE if we need to send a start record for
 * this type of accounting.
 */
#define SENDSTART(a) (acct_lists[a].act == AAA_ACCT_ACT_WAIT_START || \
		      acct_lists[a].act == AAA_ACCT_ACT_START_STOP)

/*
 * acct_malloc
 * get memory for an accounting queue entry, and load in some commonly
 * used fields, like NOW for the start timestamp.
 */
static acctinfo *acct_malloc (short type, char *name)
{
    acctinfo *acct = malloc_named(sizeof(acctinfo), name);
    ulong utime;

    if (acct) {
	aaa_acct_mallocs++;
	acct->type = type;
	acct->acct_dptr = acct->acct_data;
	acct->acct_end_write = NULL;
	GET_TIMESTAMP(acct->starttime);
	acct->task_id = ++task_seed;
	aaa_attr_add(acct, AAA_ATTR_TASK, &acct->task_id,
		     AAA_TYPE_INT, sizeof(int));
	if (clock_is_probably_valid()) {
	    char *tzname = clock_timezone_name();
	    utime = unix_time();
	    aaa_attr_add(acct, AAA_ATTR_STARTTIME, &utime,
			 AAA_TYPE_INT, sizeof(int));
	    aaa_attr_add(acct, AAA_ATTR_TIMEZONE, tzname,
			 AAA_TYPE_STR, strlen(tzname));
	}
    }
    return(acct);
}

static void acct_free (acctinfo *acct)
{
    if (mem_refcount(acct) == 1) {
	/*
	 * Really freeing the structure - free other strings too!
	 */
	aaa_acct_frees++;
    }
    free(acct);			/* Free or unlock */
}

/*
 * aaa_attr_add
 * Store aaa attribute/value pairs in a protocol independent way, putting
 * the results in a nice bytestream.  tacacs+, radius, or whatever will
 * later decode this into protocol specific formats.
 */

void aaa_attr_add (acctinfo* acct, int type, void *value, int fmt, int len)
{
    /*
     * this code is accounting protocol independent, and should get used
     * eventually.
     */
    uchar *p;

    if (!acct || !acct->acct_dptr)
	return;

    if (acct->ustruct && acct->ustruct->no_acct) {
	return;
    }

    if (fmt == AAA_TYPE_STR)
	len += 1;		/* Include terminating null w strings */
    p = acct->acct_dptr;

    /*
     * Check the write ptr against the end of the acct_data buffer
     * Typically, the buffer size, AAA_ACCT_DATASIZE is enough for
     * all accounting records
     */
    if ((p + len + 4) >= 
	((uchar*)&(acct->acct_data) + AAA_ACCT_DATASIZE)) {
	errmsg(&msgsym(BADSTR, AAAA), "too many attributes");
	return;
    }
    *p++ = len;
    *p++ = type;
    *p++ = fmt;
    bcopy(value, p, len);
    p += len;
    *p = 0;
    acct->acct_dptr = p;
}

/*
 * aaa_acct_print 
 * Prints the accounting records into the static buffer  acctprt_buf. 
 * The buffer size is ACCTPRT_LEN long. It is 100 off
 * from MEDDATA, so the buffer can be printed within a buginf, 
 * which uses median size buffer, i.e. MEDDATA. 
 */ 
#define ACCTPRT_LEN (MEDDATA-100)
static char acctprt_buf[ACCTPRT_LEN+100];
static char *aaa_acct_print (acctinfo *acct)
{
    /* this code prints the protocol-independent AV lists that we aren't using
     * yet.  Someday, we should use it.
     */
    char *w=acctprt_buf;
    struct acct_attr_ *a;
    uchar *p;

    memset(w, 0, ACCTPRT_LEN+100);

    p = acct->acct_data;
    while (p < acct->acct_dptr) {
	a = (struct acct_attr_ *) p;
	if (a->attr_ident >= AAA_ATTR_MAX || 
	    ((p + a->value_length + 4) >= 
	     ((uchar*)&(acct->acct_data) + AAA_ACCT_DATASIZE))) {
	    errmsg(&msgsym(BADSTR, AAAA), "illegal attributes");
	    break;
	}
	w += sprintf(w, "%s=", aaa_attr_names[a->attr_ident]);
	switch (a->value_fmt) {
	  case AAA_TYPE_BYTE:
	    w += sprintf(w, "%d ", a->attr_value[1]);
	    break;
	  case AAA_TYPE_INT:
	    w += sprintf(w, "%d ", GETLONG(a->attr_value));
	    break;
	  case AAA_TYPE_STR:
	    w += ansi_sprintf(w, "%.*s ", a->value_length, a->attr_value);
	    break;
	  case AAA_TYPE_IP:
	    w += sprintf(w, "%i ", GETLONG(a->attr_value));
	    break;
	  case AAA_TYPE_IPX:
	    w += sprintf(w, "%x ", GETLONG(a->attr_value));
	    break;
	  case AAA_TYPE_ETHER:
	    w += sprintf(w, "%e ", a->attr_value);
	    break;
	  default:
	    w += sprintf(w,"BADFMT");
	    break;
	}
	p = (p + sizeof(struct acct_attr_) + a->value_length)-1;
	/*
	 * if the printout has already exceed the size of the buffer
	 * truncate it and overwrite the last 5 bytes with "..."
	 */
	if ((w - acctprt_buf) > ACCTPRT_LEN) {
	    sprintf(acctprt_buf+ACCTPRT_LEN-5,"...");
	    break;
	}
	    
    }
    return(acctprt_buf);
}


/*
 * Separate fork to write accounting records with possible high delays
 * and blocking, looping over a queue of records to be written.
 */

static forktype aaa_acct_writer (void)
{
    acctinfo *acct;
    acctstruct a;
    userstruct *u;
    boolean result;

    do {
	edisms(queueBLOCK, (ulong)&acct_bkg_queue);
	acct = data_dequeue(&acct_bkg_queue);
	if (!acct)
	    continue;
#ifdef VERBOSEDEBUGGING
    if (aaa_acct_debug)
	buginf("\nAAA/ACCT: user %s, acct type %d",
	       acct->ustruct->user, acct->type);
#endif
	u = acct->ustruct;
	if (acct->ustruct->no_acct) {
	    aaa_unlock_user(&u);
	    acct->acct_end_write = NULL;
	    acct_free(acct);                    /* unlock */
	    acct = NULL;
	    continue;
	}
	memset(&a, 0, sizeof(acctstruct));
	a.info = acct;
	aaa_randomize_id(&a.session_id);
	a.seq_num = 1;
	a.flags = acct->rec_flags;
	reg_invoke_do_acct(acct->method, &result, acct->ustruct, &a);
	aaa_unlock_user(&u);
	acct->acct_end_write = NULL;
	acct_free(acct);			/* unlock */
    } while (TRUE);
}

/*
 * aaa_acctsend
 * do the dirty work of sending the collected accounting info off to a server.
 */
static void aaa_acctsend (acctinfo *acct, userstruct *user, int flags)
{
    acctstruct a;
    boolean result;

    aaa_lock_user(user);		/* Lock in user */
    mem_lock(acct);		/* Lock this account structure, */

    /* If we have a start-packet, fix the amount of data we write
     * so that data added to the record later (address negotiation parameters
     * for example) don't get written this time
     */
    if (flags == ACCT_FLAG_START) {
	acct->acct_end_write = acct->acct_dptr;
    }
    
    if (!stdio || stdio->ttynum == 0) {
	/*
	 * This process is controlled by the console, which means it's more
	 * than likely that it ISN'T the process owned by the user (ie, it's
	 * some system process like linewatch or ppp doing something)
	 * In this case, we don't want to block in the tcp code, so go start
	 * another fork to do the dirty work.  This will happen on user
	 * processes on the console too, but that's OK.
	 */
	acct->rec_flags = flags;
	acct->ustruct = user;
	if (acct_bkg_fork == NO_PROCESS) {
	   acct_bkg_fork = cfork((forkproc*)aaa_acct_writer, (long) acct, 0,
				 "AAA Accounting", 0);
	    if (acct_bkg_fork != NO_PROCESS)
		process_set_crashblock(acct_bkg_fork, TRUE);
	}
	data_enqueue(&acct_bkg_queue, acct);
	return;
    }
    memset(&a, 0, sizeof(acctstruct));
    a.info = acct;
    aaa_randomize_id(&a.session_id);
    a.seq_num = 1;
    a.flags = flags;
    if (!user->no_acct) {
	reg_invoke_do_acct(acct->method, &result, user, &a);
    }
    aaa_unlock_user(&user);
    acct->acct_end_write = NULL;
    acct_free(acct);		/* unlock */
}    

/*
 * aaa_acctstop
 * Send an AAA accounting stop record to the appropriate protocols.
 * This uses a passed acctinfo to supply most of the useful info.
 */

static void aaa_acctstop (userstruct *user, int stat, acctinfo *acct,
				 ulong bytesin, ulong bytesout, ulong paksin,
				 ulong paksout)
{
    char buff[30];
    ulong elapsed, bi, bo, pi, po;
    char *type = "Unknown";

    if (!acct)
	return;

    if (user->no_acct) {
	return;
    }

    if (!user) {
	errmsg(&msgsym(NULUSR, AAAA));
	acct_free(acct);
	return;
    }
    unqueue(&user->acctQ, acct);

    elapsed = ELAPSED_TIME(acct->starttime);
    sprint_dhms(buff, acct->starttime);

    switch (acct->type) {
      case ACCT_TYPE_EXEC:
	/*
	 * End of an exec.
	 */
	type = "Exec";
	/*
	 * there ought not be any other accounting records left!
	 */
	if (user->acctQ.qhead != NULL) {
	    errmsg(&msgsym(BADSTR, AAAA), "records remain");
	}
	aaatraf_acctstops[acct->type]++;
	break;
      case ACCT_TYPE_CONN:
	/*
	 * End of an active connection
	 */
	pi = paksin - acct->packets_in;
	bi =  bytesin - acct->bytes_in;
	po = paksout - acct->packets_in;
	bo = bytesout - acct->bytes_out;
	aaa_attr_add(acct, AAA_ATTR_BYTESIN, &bi, AAA_TYPE_INT, sizeof(long));
	aaa_attr_add(acct, AAA_ATTR_BYTESOUT, &bo, AAA_TYPE_INT, sizeof(long));
	aaa_attr_add(acct, AAA_ATTR_PAKSIN, &pi, AAA_TYPE_INT, sizeof(long));
	aaa_attr_add(acct, AAA_ATTR_PAKSOUT, &po, AAA_TYPE_INT, sizeof(long));
	aaatraf_acctstops[acct->type]++;
	type = "Connection";
	break;
      case ACCT_TYPE_NET:
	/*
	 * End of a network service connection (slip, ppp, arap, etc)
	 */
	
	if (acct->idb != NULL) { /* don't try for ARA */
	    bi =  bytesin - acct->bytes_in;
	    bo = bytesout - acct->bytes_out;
	    aaa_attr_add(acct, AAA_ATTR_BYTESIN, &bi,
			 AAA_TYPE_INT, sizeof(long));
	    aaa_attr_add(acct, AAA_ATTR_BYTESOUT, &bo,
			 AAA_TYPE_INT, sizeof(long));
	}
	pi = paksin - acct->packets_in;
	po = paksout - acct->packets_out;
	aaa_attr_add(acct, AAA_ATTR_PAKSIN, &pi, AAA_TYPE_INT, sizeof(long));
	aaa_attr_add(acct, AAA_ATTR_PAKSOUT, &po, AAA_TYPE_INT, sizeof(long));
	type = "Network";
	aaatraf_acctstops[acct->type]++;
	break;
      default:
	errmsg(&msgsym(BADSTR, AAAA), "Unknown type");
	break;
    } /* Switch */

    elapsed = (elapsed + (ONESEC/2)) / ONESEC;
    aaa_attr_add(acct, AAA_ATTR_ELAPSTIME, &elapsed, 
		 AAA_TYPE_INT, sizeof(ulong));
    if (aaa_acct_debug) {
	buginf("\nAAA/ACCT: %s acct stop.  User %s, Port %s:\n         %s",
	       type, user->user, user->port, aaa_acct_print(acct));
    }
    aaa_acctsend(acct, user, ACCT_FLAG_STOP);

    acct_free(acct);	    
}

/*
 * aaa_acct_cmdstart
 * Possibly send an account start record for an arbitrary command
 * (via "aaa accounting level N")  In any case, initialize an acctinfo
 * and put it on the users accounting event queue.
 */

static acctinfo *aaa_acct_cmdstart (userstruct *user, parseinfo *csb)
{
    acctinfo *acct;
    int priv, method;
    char *cmd;

    if (!user)
	return(NULL);

    if (user->no_acct) {
	return(NULL);
    }

    if (!csb) {
	if (!(method = aaa_acct_needed(AAA_AUTHOR_TYPE_SHELL, 0)))
	    return(NULL);
	/*
	 * EXEC accounting start
	 */
	if (aaa_acct_debug)
	    buginf("\nAAA/ACCT: EXEC acct start. User %s, port %s",
		   user->user, user->port);
	acct = user->acctQ.qtail;
	if (!acct || acct->type != ACCT_TYPE_EXEC) {
	    acct = acct_malloc(ACCT_TYPE_EXEC, "AAA acct 1");
	    if (!acct)
		return(NULL);
	    acct->method = method;
	    enqueue(&user->acctQ, acct);
	}
	aaa_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_shell,
		     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_SVC_shell));
	if (SENDSTART(AAA_AUTHOR_TYPE_SHELL)) {
	    aaatraf_acctstarts[acct->type]++;
	    aaa_acctsend(acct, user, ACCT_FLAG_START);
	}
	return(acct);
    }
    
    priv = csb->last_priv;
    if (!(method = aaa_acct_needed(AAA_AUTHOR_TYPE_CMD, priv))) {
	priv = csb->highest_priv;
	if (!(method = aaa_acct_needed(AAA_AUTHOR_TYPE_CMD, priv)))
	    return(NULL);
    }
    if (csb->flags & COMMAND_AUTHORIZE)
	cmd = csb->nv_command;
    else
	cmd = csb->line;
    if (aaa_acct_debug)
	buginf("\nAAA/ACCT: Command Accounting. User %s, Port %s, Priv %d:\n"
	       "         \"%s\"", user->user, user->port, priv, cmd);
    acct = acct_malloc(ACCT_TYPE_CMD, "AAA acct 2");
    if (!acct)
	return(NULL);
    acct->method = method;
    aaa_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_shell,
		 AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_SVC_shell));
    aaa_attr_add(acct, AAA_ATTR_PRIV, &priv,
		 AAA_TYPE_INT, sizeof(int));
    aaa_attr_add(acct, AAA_ATTR_CMD, cmd,
		 AAA_TYPE_STR, strlen(cmd));
    aaa_acctsend(acct, user, ACCT_FLAG_STOP);
    acct_free(acct);
    return(NULL);
}

/*
 * acct_execinfo.
 * Record additional info about a command style accounting record.  This is
 * usually additional info for an exec accounting record such as callback
 * data.  The execinfo can be registered before or after the cmdstart.
 * If BEFORE, the latest acct entry on the acctQ will not be a command or
 * exec entry, and the info is saved for future reference.
 * If AFTER, it is saved, and an addition accounting record (with the same
 * session_id) is sent to the server.
 */
static void acct_execinfo (userstruct *user, int type, void *value,
			   int fmt, int len)
{
    boolean dosend = FALSE;
    acctinfo *acct;

    if (!user || !aaa_acct_needed(AAA_AUTHOR_TYPE_SHELL, 0))
	return;

    if (user->no_acct) {
	return;
    }

    /*
     * Check whether it looks like this data should be added to the exec
     * record. Don't allow called routine to create one if not found.
     */
    acct = acct_search_create_type(user, ACCT_TYPE_CMD, FALSE);
    if (!acct)
        acct = acct_search_create_type(user, ACCT_TYPE_EXEC, FALSE);

    /*
     * Create an exec type accounting entry if no exec or command type
     * was found. Otherwise, flag that the START accounting mesage be 
     * sent
     */
    if (!acct) {
	acct = acct_malloc(ACCT_TYPE_EXEC, "AAA acct 3");
    } else {
	dosend = TRUE;
    }

    if (acct) {
	aaa_attr_add(acct, type, value, fmt, len);
	if (dosend)  {
	    aaatraf_acctupdate[acct->type]++;
	    if (SENDSTART(AAA_AUTHOR_TYPE_SHELL))
		aaa_acctsend(acct, user, ACCT_FLAG_WATCHDOG);
	} else
	    enqueue(&user->acctQ, acct);
    }
}

/*
 * aaa_acct_netstart
 * Possibly send an accounting start record for a network even such at
 * PPP/SLIP/ARAP startup, or an NCP activation.  In any case, initialize an
 * acctinfo with data from the idb, and put it in the user acctQ.
 */

static acctinfo *aaa_acct_netstart (userstruct *user, parseinfo *csb,
				    idbtype *idb)
{
    acctinfo *acct;
    int method;

    if (!user || !(method = aaa_acct_needed(AAA_AUTHOR_TYPE_NET, 0)))
	return(NULL);

    if (user->no_acct) {
	return(NULL);
    }

    if (aaa_acct_debug) {
	char *p = NULL;		/* OK to _doprnt() a NULL string */
	if (csb)
	    p = csb->line;
	else if (idb)
	    p = idb->namestring;
	else
	    p = "ARAP";

	buginf("\nAAA/ACCT: NET acct start. User %s, Port %s: %s",
	       user->user, user->port, p);
    }
    /*
     * Search for an existing network record created by netinfo calls.
     * If not found, create one and queue it to the user
     */
    acct = acct_search_create_type(user, ACCT_TYPE_NET, TRUE);

    if (!acct) 
	return(NULL);

    acct->method = method;
    if (idb) {
	hwidbtype *hwidb = idb->hwptr;
	char *svc;
	acct->idb = idb;

	switch (hwidb->enctype) {
	  case ET_PPP:
	    if (reg_invoke_ppp_lcpstate(hwidb, FORWARDING) ||
		    reg_invoke_ppp_lcpstate(hwidb, FORWARDED)) {
		svc = AAA_VAL_SVC_vpdn;
	    } else {
		svc = AAA_VAL_SVC_ppp;
	    }
	    break;
	  case ET_SLIP:
	    svc = AAA_VAL_SVC_slip;
	    break;
	  default:
	    svc = (char *) encstring(hwidb->enctype);
	    break;
	}

	aaa_attr_add(acct, AAA_ATTR_SERVICE, svc, AAA_TYPE_STR, strlen(svc));

	acct->bytes_in = idb_rx_cumbytes(hwidb);
	acct->packets_in = idb_inputs(hwidb);
	acct->bytes_out = idb_tx_cumbytes(hwidb);
	acct->packets_out = idb_outputs(hwidb);
    } else {
	aaa_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_arap,
		     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_SVC_arap));
    }
    if (csb)
	aaa_attr_add(acct, AAA_ATTR_CMD, csb->line,
		     AAA_TYPE_STR, strlen(csb->line));
    if (SENDSTART(AAA_AUTHOR_TYPE_NET)) {
	aaatraf_acctstarts[acct->type]++;
	aaa_acctsend(acct, user, ACCT_FLAG_START);
    }

    return(acct);
}


/*
 * acct_netinfo.
 * Record additional info about a network style accounting record.  This
 * is usually something like the network address, which can get figured out
 * much later than the network start in the case of PPP, for example.  netinfo
 * can be registered before or after the netstart.  If BEFORE, it is saved.
 * If AFTER, it is saved, and an addition accounting record (with the same
 * session_id) is sent to the server iff the send boolean is true.
 */
static void acct_netinfo (userstruct *user, int type, void *value,
			  int fmt, int len, boolean send)
{
    boolean dosend = FALSE;
    acctinfo *acct;

    if (!user || !(aaa_acct_needed(AAA_AUTHOR_TYPE_NET, 0)))
	return;

    if (user->no_acct) {
	return;
    }

    /*
     * Search for an existing network record created by netinfo calls.
     * Don't allow called routine to create one if not found.
     */
    acct = acct_search_create_type(user, ACCT_TYPE_NET, FALSE);   

    if (!acct) {
	acct = acct_malloc(ACCT_TYPE_NET, "AAA acct 4");
    } else {
	dosend = TRUE;
    }

    if (acct) {
	aaa_attr_add(acct, type, value, fmt, len);
	if (dosend) {
	    aaatraf_acctupdate[acct->type]++;
	    if (send && SENDSTART(AAA_AUTHOR_TYPE_NET))
		aaa_acctsend(acct, user, ACCT_FLAG_WATCHDOG);
	} else
	    enqueue(&user->acctQ, acct);
    }
}

/*
 * aaa_acct_connstart
 * collect accounting info for a network connection.
 * uses a vector in the conn structure to get actual statistics.
 */
static acctinfo *aaa_acct_connstart (userstruct * user, parseinfo *csb,
				     conntype *conn)
{
    acctinfo *acct;
    int method = aaa_acct_needed(AAA_AUTHOR_TYPE_CONN, 0);

    if (!user || !method)
	return(NULL);

    if (user->no_acct) {
	return(NULL);
    }

    acct = acct_malloc(ACCT_TYPE_CONN, "AAA acct 5");
    if (!acct)
	return(NULL);
    acct->method = method;
    if (conn && (stdio->conntop == conn))
	aaa_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_ttydaemon,
		     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_SVC_ttydaemon));
    else
	aaa_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_connect,
		     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_SVC_connect));
    if (conn) {
	conn->acct_info = acct;
	conn_acctupdate(conn, acct);
    } else if (stdio->xrcb)
	aaa_attr_add(acct, AAA_ATTR_PROTO, AAA_VAL_PROT_xremote,
		     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_PROT_xremote));
    if (csb)
	aaa_attr_add(acct, AAA_ATTR_CMD, csb->line,
		     AAA_TYPE_STR, strlen(csb->line));

    enqueue(&user->acctQ, acct);
    if (SENDSTART(AAA_AUTHOR_TYPE_CONN)) {
	aaatraf_acctstarts[acct->type]++;
	aaa_acctsend(acct, user, ACCT_FLAG_START);
    }
    if (aaa_acct_debug)
	buginf("\nAAA/ACCT: Connect start. User %s, Port %s, %s",
	       user->user, user->port,
	       csb ? csb->line : (conn ? conn->hostname : ""));
    return(acct);
}

/*
 * aaa_acct_system
 * Send a system accounting message.  This is a moderately generic
 * pair of attributes: "event", and "reason".  Typically, event is
 * whether aaa accounting is being turned on or off, and reason is why.
 * Since there is no user associated with system events, we create a
 * dummy null user.  start-stop is ignored for system accounting.
 */
static void
aaa_acct_system (char *event, char *reason, int flags)
{
    userstruct *user;
    acctinfo *acct;
    int method;
    /*
     * See if accounting is needed.  Some care is required to call us
     * AFTER this is set correctly for the event that occured.
     * It doesn't do to say that accounting is being turned on if the
     * data structures still say its off and we immediately return.
     */
    if (!(method = aaa_acct_needed(AAA_AUTHOR_TYPE_SYSTEM, 0)))
	return;

    if (aaa_create_user(&user, NULL, NULL, NULL, NULL,
			AAA_AUTHEN_TYPE_NONE, AAA_SVC_NONE, PRIV_MAX)) {
	acct = acct_malloc(ACCT_TYPE_SYS, "AAA acct 6");
	if (acct) {
	    acct->method = method;
	    aaa_attr_add(acct, AAA_ATTR_SERVICE, AAA_VAL_SVC_system,
		     AAA_TYPE_STR, AAA_STRSIZE(AAA_VAL_SVC_system));
	    aaa_attr_add(acct, AAA_ATTR_EVENT, event,
			 AAA_TYPE_STR, strlen(event));
	    aaa_attr_add(acct, AAA_ATTR_REASON, reason,
			 AAA_TYPE_STR, strlen(reason));
	    if (aaa_acct_debug) {
		buginf("\nAAA/ACCT: System Event: %s", aaa_acct_print(acct));
	    }
	    aaa_acctsend(acct, user, flags);
	    acct_free(acct);	/* clean up after ourselves, being */
	}
	aaa_free_user(&user);	/*  careful to observe sharecounts, etc */
    }
}


/*
 * aaa_acct_startup
 * send an accounting record when the system first starts up
 * This is just a wrapper for aaa_acct_system()
 */
static void
aaa_acct_startup (void)
{
    /*
     * we remember that the system has passed through the reload state.  We 
     * don't want to try to send accounting info in aaa_acct_cmd() until after
     * the system is well up and ready to do complicated things.
     */
    aaa_reload_finished = TRUE;

    aaa_acct_system("sys_acct", "reload", ACCT_FLAG_START);
}


/*
 * aaa_acct_shutdown
 * send an accounting record when the system is about to be reloaded
 * (assumes a gracefull shutdown, obviously).  Another wrapper.
 */
static void
aaa_acct_shutdown (void)
{
    aaa_acct_system("sys_acct", "shutdown", ACCT_FLAG_STOP);
}


/*
 * aaa_acct_cmd
 * if system accounting is turned on or off other than during system
 * startup, we should send system accounting messages as well.
 * Our caller is responsible for detecting that the state has actually
 * changed - we'll always send a mesasge reflecting the csb indicated state.
 * We avoid sending a message when the system is first booting and reading
 * the config from NVM, because aaa_reload_finished won't be set.
 */
static void
aaa_acct_cmd (parseinfo *csb)
{
    char *event;

    if (aaa_reload_finished) {
	switch(GETOBJ(int, 2)) {
	  case AAA_AUTHOR_TYPE_SYSTEM:
	    event = "sys_acct";
	    break;
	  case AAA_AUTHOR_TYPE_CONN:
	    event = "conn_acct";
	    break;
	  case AAA_AUTHOR_TYPE_SHELL:
	    event = "shell_acct";
	    break;
	  case AAA_AUTHOR_TYPE_CMD:
	    event = "cmd_acct";
	    break;
	  case AAA_AUTHOR_TYPE_NET:
	    event = "net_acct";
	    break;
	  default:
	    event = "unknown";
	}

	if (csb->sense)
	    aaa_acct_system(event, "reconfigure", ACCT_FLAG_START);
	else 
	    aaa_acct_system(event, "reconfigure", ACCT_FLAG_STOP);
    }
}

/*
 * acct_cleanup
 * when a user logs out or the userstruct is being deleted, clean up all
 * the outstanding accounting events for the user by sending appropriate
 * stop records.  Send the stop for the initial "login" last...
 */
static void acct_cleanup (userstruct *user)
{
    acctinfo *next;
    acctinfo *p;
    idbtype *idb;
    long bi, bo, pi, po;

    if (user == NULL)
	return;

    if (user->no_acct) {
	return;
    }

    p = user->acctQ.qhead;

    if (p) {
	/*
	 * The first element in the queue should be our exec or top level
	 * login accounting record.  Skip it now and save till last to delete!
	 */
	next = p->next;
	while (next) {
	    if (!validlink(next)) {
		errmsg(&msgsym(BADSTR, AAAA), "bad pointer");
		break;
	    }
	    p = next;
	    next = p->next;
	    if (p->idb) {
		idb = p->idb;
		bi = idb_rx_cumbytes(idb->hwptr);
		pi = idb_inputs(idb->hwptr);
		bo = idb_tx_cumbytes(idb->hwptr);
		po = idb_outputs(idb->hwptr);
	    } else
		bi = bo = pi = po = 0;
	    aaa_acctstop(user, OK, p, bi, bo, pi, po);
	}
	p = user->acctQ.qhead;
	if ((p) && (p->idb)) {
	    idb = p->idb;
	    bi = idb_rx_cumbytes(idb->hwptr);
	    pi = idb_inputs(idb->hwptr);
	    bo = idb_tx_cumbytes(idb->hwptr);
	    po = idb_outputs(idb->hwptr);
	} else
	    bi = bo = pi = po = 0;
	aaa_acctstop(user, OK, p, bi, bo, pi, po);
    }	
}

/*
 * Search for accounting type for specified user.
 * Optionally, allow for creation of accounting type and queue to the user.
 */
static acctinfo *acct_search_create_type (userstruct *user, short type,
					  boolean create)
{
    acctinfo *acct;

    if (!user)
       return(NULL);

    if (user->no_acct) {
	return(NULL);
    }

    /*
     * Search for an existing network record.
     * Note that for a given user data structure, there can only be 
     * ONE accounting type accounting record active at any given time!
     */
    for (acct = user->acctQ.qhead; acct; acct = acct->next) {
	if (acct->type == type)
	    break;
    }

    /*
     * If caller wants to create one when no accounting type is found,
     * do so and queue it to the user
     */
    if (create && !acct) {
      acct = acct_malloc(type, "AAA acct 7");
      if (acct)
         enqueue(&user->acctQ, acct); 
    }
    return(acct);
}

#if 0
char *acctwhen (boolean start, boolean wait)
{
    if (start && wait)
	return("wait-start");
    else if (start)
	return("start-stop");
    else
	return("stop-only");
}

void accounting_command (parseinfo *csb)
{
    int i;
    boolean wait = FALSE, start = FALSE;


    if (csb->nvgen) {
	acct_cfgtype which;

	switch (GETOBJ(int,2)) {
	  case AAA_AUTHOR_TYPE_SHELL:
	    which = aaa_acct_exec;
	    break;
	  case AAA_AUTHOR_TYPE_NET:
	    which = aaa_acct_net;
	    break;
	  case AAA_AUTHOR_TYPE_CONN:
	    which = aaa_acct_conn;
	    break;
	  case AAA_AUTHOR_TYPE_CMD:
	    which.doit = FALSE;
	    for (i=0; i<=PRIV_MAX; i++) {
		nv_write(aaa_acct_priv[i].doit, "%s %d %s %s",
			 csb->nv_command, i, 
			 acctwhen(aaa_acct_priv[i].start,
				  aaa_acct_priv[i].wait), "tacacs+");
	    }
	    break;
	}
	nv_write(which.doit, "%s %s %s", csb->nv_command,
		 acctwhen(which.start, which.wait), "tacacs+");
    }


    switch (GETOBJ(int,4)) {	/* Load up details of accounting */
      case AAA_ACCT_ACT_WAIT_START:
	wait = TRUE;
	/* Fall through */
      case AAA_ACCT_ACT_START_STOP:
	start = TRUE;
	break;
    }    

    switch (GETOBJ(int,2)) {
      case AAA_AUTHOR_TYPE_SHELL:
	aaa_acct_exec.doit = csb->sense;
	aaa_acct_exec.wait = wait;
	aaa_acct_exec.start = start;
	break;

      case AAA_AUTHOR_TYPE_CMD:
	i = GETOBJ(int,3);
	aaa_acct_priv[i].doit = csb->sense;
	aaa_acct_priv[i].wait = wait;
	aaa_acct_priv[i].start = start;
	break;

      case AAA_AUTHOR_TYPE_NET:
	aaa_acct_net.doit = csb->sense;
	aaa_acct_net.wait = wait;
	aaa_acct_net.start = start;
	break;

      case AAA_AUTHOR_TYPE_CONN:
	aaa_acct_conn.doit = csb->sense;
	aaa_acct_conn.wait = wait;
	aaa_acct_conn.start = start;
	break;
    }
}
#endif /* 0 */

/*
 * aaa_relogin_fixup
 * When a user logs in (using the login command) under a new username,
 * we have to adjust the accounting records in reasonable ways.
 * send a logout record for the old username, a login record for the
 * new username, and transfer any open connection acct records to the
 * new username (not quite ideal, but not too bad.)
 */
static void aaa_relogin_fixup (userstruct *newuser, userstruct *olduser)
{
    queuetype oldQ;
    acctinfo *acct;

    if (olduser->no_acct) {
	return;
    }

    /*
     * Sanity check
     */
    if (newuser->acctQ.qhead ||	newuser == olduser)
	errmsg(&msgsym(RELOGIN, AAAA), olduser->user, newuser->user);

    reg_invoke_aaa_acct_cmd(newuser, NULL); /* log in new user */

    if (olduser && olduser->acctQ.qhead) {
	/*
	 * transfer old accounting information
	 */
	oldQ = olduser->acctQ;	/* temporary queue of acct entries */
	queue_init(&olduser->acctQ, 0); /* reset old queue */
	while ((acct = dequeue(&oldQ))) {
	    if (acct->type == ACCT_TYPE_EXEC) {
		/*
		 * Put "login" entry back on the old ustruct, so we can
		 * do a clean "logout" for it.
		 */
		enqueue(&olduser->acctQ, acct);
	    } else {
		enqueue(&newuser->acctQ, acct);
		if (aaa_acct_debug)
		    buginf("\nAAA/ACCT: transferring type %d acct record"
			   " from %s to %s", acct->type, olduser->user,
			   newuser->user);
	    }
	}
	/*
	 * log out the old user.  Note that this doesn't delete the
	 * user structure, our caller does that (probably)
	 */
	aaa_acctstop(olduser, 0, olduser->acctQ.qhead, 0,0,0,0);
    }
}

/*
 * print_acctdata
 * Given a userstruct, print out any accounting records contained therein.
 */
static void print_acctdata (tt_soc *tty, hwidbtype *idb, userstruct *user)
{
    acctinfo *acct;
    char buff[30];
    char *type;

    if (user->acctQ.qhead) {
	printf("\n\nActive Accounted actions on ");
	if (tty)
	    printf("tty%t", tty->ttynum);
	if (idb)
	    printf("Interface %s", idb->hw_namestring);
	if (!validlink(user->user))
	    return;
	printf(", User %s Priv %d", user->user ? user->user : "(not logged in)", user->priv_lvl);
	for (acct = user->acctQ.qhead; acct; acct = acct->next) {
	    if (!validlink(user->acctQ.qhead))
		break;
	    sprint_dhms(buff, acct->starttime);
	    switch (acct->type) {
	      case ACCT_TYPE_EXEC:
		type = "EXEC";
		aaatraf_acctactive[acct->type]++;
		break;
	      case ACCT_TYPE_CONN:
		type = "Connection";
		aaatraf_acctactive[acct->type]++;
		break;
	      case ACCT_TYPE_NET:
		type = "Network";
		aaatraf_acctactive[acct->type]++;
		break;
	      default:
		type = "Unknown";
		aaatraf_acctactive[acct->type]++;
		break;
	    }
	    printf("\n Task ID %d, %s Accounting record, %s Elapsed",
		   acct->task_id, type, buff);
	    printf("\n %s", aaa_acct_print(acct));
	    if ((!validlink(acct->next)))
		break;
	}
    }
}

static void show_acct_idbs (hwidbtype *idb)
{
    if (idb->lcpstate && idb->lcpstate->ustruct)
	if (!(idb->idb_tty &&
	      (idb->idb_tty->ustruct == idb->lcpstate->ustruct)))
	    print_acctdata(NULL, idb, idb->lcpstate->ustruct);
}

/*
 * show_accounting
 * Step through various lines and print out the stored accounting records.
 */
static void show_accounting (parseinfo *csb)
{
    tt_soc *tty;
    int line, i;

    for (i=0; i < ACCT_TYPE_MAX; i++)
	aaatraf_acctactive[i] = 0;

    automore_enable(NULL);
    /*
     * show accounting data for tty lines
     */
    for (line=0; line < ALLlines; line++) {
	tty = MODEMS[line];
	if (tty == NULL)
	    break;
	if (tty->ustruct)
	    print_acctdata(tty, NULL, tty->ustruct);
    }
    /*
     * Show accounting for interfaces
     */
    show_hwidb_chain(NULL, show_acct_idbs);

    if (aaa_acct_debug) {
	printf("\n"
	       "\n     Overall Accounting Traffic"
	       "\n          Starts   Stops  Updates  Active"
	       "\nExec      %-6d   %-5d   %-6d  %-6d"
	       "\nNetwork   %-6d   %-5d   %-6d  %-6d",
	       aaatraf_acctstarts[ACCT_TYPE_EXEC],
	       aaatraf_acctstops[ACCT_TYPE_EXEC],
	       aaatraf_acctupdate[ACCT_TYPE_EXEC],
	       aaatraf_acctactive[ACCT_TYPE_EXEC],

	       aaatraf_acctstarts[ACCT_TYPE_NET],
	       aaatraf_acctstops[ACCT_TYPE_NET],
	       aaatraf_acctupdate[ACCT_TYPE_NET],
	       aaatraf_acctactive[ACCT_TYPE_NET]);
	printf("\nConnect   %-6d   %-5d   %-6d  %-6d"
	       "\nCommand   %-6d   %-5d   %-6d  %-6d",
	       aaatraf_acctstarts[ACCT_TYPE_CONN],
	       aaatraf_acctstops[ACCT_TYPE_CONN],
	       aaatraf_acctupdate[ACCT_TYPE_CONN],
	       aaatraf_acctactive[ACCT_TYPE_CONN],

	       aaatraf_acctstarts[ACCT_TYPE_CMD],
	       aaatraf_acctstops[ACCT_TYPE_CMD],
	       aaatraf_acctupdate[ACCT_TYPE_CMD],
	       aaatraf_acctactive[ACCT_TYPE_CMD]);
	printf("\n\nUser creates:%d, frees:%d, Acctinfo mallocs:%d, frees:%d"
	       "\nUsers freed with accounting unaccounted for: %d",
	       aaa_user_create, aaa_user_frees,
	       aaa_acct_mallocs, aaa_acct_frees, aaa_user_acctleft);
    }

    automore_disable();
}

/*
 * Parse chains for "show accounting" commands
 */
#include "../parser/macros.h"

#define ALTERNATE       NONE
#include "exec_show_accounting.h"
LINK_POINT(acct_show_commands, ALTERNATE);
#undef	ALTERNATE

static parser_extension_request acct_chain_init_table[] = {
    { PARSE_ADD_SHOW_CMD, &pname(acct_show_commands) },
    { PARSE_LIST_END, NULL }
};

static void aaa_acct_init (subsystype *subsys)
{
    int i;

    parser_add_command_list(acct_chain_init_table, "Accounting");

    reg_add_aaa_acct_cmd(aaa_acct_cmdstart, "aaa_acct_cmdstart");
    reg_add_aaa_acct_conn(aaa_acct_connstart, "aaa_acct_connstart");
    reg_add_aaa_acct_net(aaa_acct_netstart, "aaa_acct_netstart");
    reg_add_aaa_acct_end(aaa_acctstop, "aaa_acctstop");
    reg_add_aaa_acct_cleanup(acct_cleanup, "acct_cleanup");
    reg_add_aaa_acct_netinfo(acct_netinfo, "acct_netinfo");
    reg_add_aaa_acct_execinfo(acct_execinfo, "acct_cmdinfo");
    reg_add_aaa_acct_search_create_type(acct_search_create_type, "acct_search_create_type");
    reg_add_aaa_acct_attr_add(aaa_attr_add, "aaa_attr_add");
    reg_add_aaa_acct_relogin_fixup(aaa_relogin_fixup, "aaa_relogin_fixup");
    reg_add_restarted(aaa_acct_startup, "aaa_acct_startup");
    reg_add_will_reload(aaa_acct_shutdown, "aaa_acct_shutdown");
    reg_add_aaa_acct_cmdparse(aaa_acct_cmd, "aaa_acct_cmd");

    queue_init(&acct_bkg_queue, 0);

    acct_bkg_fork = NO_PROCESS;
    aaa_acct_frees = aaa_acct_mallocs = 0;
    for (i=0; i < ACCT_TYPE_MAX; i++) {
	aaatraf_acctstarts[i] = 0;
	aaatraf_acctstops [i] = 0;
	aaatraf_acctupdate[i] = 0;
    }	
}

SUBSYS_HEADER(aaa_accounting, AAA_MAJVERSION, AAA_MINVERSION, AAA_EDITVERSION,
	      aaa_acct_init, SUBSYS_CLASS_PROTOCOL,
	      NULL, NULL);

