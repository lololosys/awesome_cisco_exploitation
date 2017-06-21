/* $Id: syslogmib_history.c,v 3.1.48.7 1996/09/14 00:55:21 ramk Exp $
 * $Source: /release/112/cvs/Xsys/os/syslogmib_history.c,v $
 *------------------------------------------------------------------
 * syslogmib_history.c - Syslog History Database
 *
 * September 1995, Anke Dosedal
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: syslogmib_history.c,v $
 * Revision 3.1.48.7  1996/09/14  00:55:21  ramk
 * CSCdi66329:  SNMP syslog traps specify wrong enterpriseOID,sysLogEvent
 * needs fix
 * Branch: California_branch
 * Pass a (pointer to) sysloghistoryentry_t as a parameter to syslogEvent
 *
 * Revision 3.1.48.6  1996/09/13  18:08:20  anke
 * CSCdi68720:  errmsg too long causes router to crash
 * Increase tmp string space to the same errmsg_guts allows, make it static
 * so we don't put too much on the stack, and protect it with a semaphore.
 * Branch: California_branch
 *
 * Revision 3.1.48.5  1996/08/29  23:31:49  snyder
 * CSCdi67619:  facility obsoleted by CSCdi67083, dump it, save memory
 *              saves 404 bytes from image, 360 from data section
 *              also pre-commit hamptons error message doc macros, so
 *              between the 2 of us we only touch logger.h one time
 * Branch: California_branch
 *
 * Revision 3.1.48.4  1996/08/28  13:04:31  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.48.3  1996/06/27  19:11:34  anke
 * CSCdi58471:  Invalid memory action in syslog mib
 * Pre-allocate the memory that might be needed so we don't get caught
 * during an interupt.  Change to queue to a circular one to reduce
 * amount of pointer moving that might need to happen.
 * Branch: California_branch
 *
 * Revision 3.1.48.2  1996/05/23  00:23:39  mordock
 * CSCdi58471:  Invalid memory action in syslog mib
 * Branch: California_branch
 * temp fix: disable processing msgs in syslog mib if on interrupt stack
 * or recursing
 *
 * Revision 3.1.48.1  1996/05/21  06:36:42  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:23  anke
 * Placeholder for Syslog MIB
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include "subsys.h" 
#include "types.h"
#include "stdlib.h"
#include "logger.h"
#include "stdarg.h"
#include "config.h"
#include "parser.h"
#include "packet.h"
#include "logger_private.h"
#include "syslogmib_history.h"
#include "../snmp/snmp_api.h"
#include "../snmp/snmp_registry.h"
#include "../h/sys_registry.h"
#include "../parser/parser_defs_loghist.h"

/*
 * The syslog history data base came into being as a result of a new SNMP
 * Syslog MIB.  This history table is basically a circular fifo type of 
 * queue.  We start with a size of 1.  If the user changes this, the
 * resize routine is called and malloc's and adds or removes the
 * requested number of entries.  Every time a new syslog message is
 * logged, the tail pointer is moved to the next entry, that entry is
 * flushed, and filled in with the new information. This insures
 * a lexicographical ordering, at least until the index wraps.  When that 
 * happens, the queue will flush completely, and start fresh.
 */

/* History Scalars */

unsigned long syslog_table_size;
boolean  syslog_notification_enabled;
unsigned long max_severity;

/* History Table */
static sysloghistoryentry_t *tail_ptr;
static sysloghistoryentry_t *head_ptr;
static unsigned long hist_index;

/* Internal State */

static const char syslog_history_str[] = "Syslog History";
static boolean not_inited = TRUE;
static watched_semaphore *sp = NULL;
static unsigned long entries_flushed_count;  /* number of entries flushed */
static unsigned long ignored_count;          /* number of messages ignored */
static unsigned long syslog_drop_count;      /* number of messages dropped 
                                                because of recursion */
static unsigned long syslog_notification_count;
                                             /* number of notifications sent */


/* External Procedures */

/* syslog_history_find
 * 
 * Find the syslog history entry with a history index equal to or
 * greater than the one supplied, as controlled by an input option
 *
 */

sysloghistoryentry_t *
syslog_history_find(long want_hist_index, boolean want_equal)
{
    int i, delta;
    int smallest_index, largest_index;
    sysloghistoryentry_t *entry_ptr;

    /* make sure that the queue isn't empty, ie we were not able to malloc 
     * memory during init */
    if (not_inited || !head_ptr->valid)
        return(NULL);

    /* make sure not asking for something we don't have yet */
    largest_index = tail_ptr->hist_index;
    if (want_hist_index > largest_index)
        return(NULL);

    /* check if it is at or before the smallest index */
    smallest_index = head_ptr->hist_index;
    if (want_hist_index == smallest_index) 
        return(head_ptr);

    if (want_hist_index < smallest_index) {
        if (want_equal)
            return(NULL);
        else
            return(head_ptr);
    }

    /* must be in what we have */
    /* Perhaps checking for the end of the queue is too paranoid, 
       but want to insure that in case the queue gets shortened 
       from under us during a get, that things don't get confused */
    delta = want_hist_index - smallest_index;
    entry_ptr = head_ptr;
    for (i=0; i < delta; i++) {
	entry_ptr = entry_ptr->next;
	if (entry_ptr->hist_index == want_hist_index)
	    return(entry_ptr);
	if (entry_ptr == tail_ptr)
	    break;
    }
    return(NULL); 
}

/* init_syslog_history
 *
 * Initialize syslog history queue and set default values
 *
 */

static void init_syslog_history(subsystype *subsys)
{
    sysloghistoryentry_t *entry_ptr;

    syslog_table_size = DEF_SYSLOG_TABLE_SIZE;      /* default is 1 */
    max_severity = -1;
    syslog_notification_enabled = FALSE;
    syslog_notification_count = 0;
    ignored_count = 0;
    entries_flushed_count = 0;
    syslog_drop_count = 0;
    hist_index = 0;

    entry_ptr = malloc_named(sizeof(sysloghistoryentry_t), syslog_history_str);
    if (entry_ptr != NULL) {
        head_ptr = tail_ptr = entry_ptr;
        entry_ptr->next = entry_ptr;
        entry_ptr->valid = FALSE;
        not_inited = FALSE;
    }

    if (!sp) {
	sp = create_watched_semaphore("SyslogHistSemaphore", 0);
	if (!sp)
	    return;
    }

    syslog_parser_init();
    reg_add_syslog_history_event(syslog_history_event, "syslog_history_event");

    return;
}


/* syslog_history_event
 *
 * Record a syslog event in the history table and send a notification.
 *
 */

void syslog_history_event(const message *msg, va_list args)
{
    sysloghistoryentry_t *entry_ptr;
    int length;
    static char msg_str[LOGDATA+ENCAPBYTES];
    leveltype status;
    static short recursion_depth = 0;

    /* if we are recursing, increment our counter and after 5 times, return */
    if (recursion_depth >= 5) {
	syslog_drop_count++;
	return;
    }
	
    /* if we were not able to allocate memory during init, don't bother */
    if (not_inited)
	return;

    /* Make sure that the user is even interested in this event.
     *
     * If the config command has not been set, check against the default,
     * else check against whatever max_severity has been set to.
     */
    if (max_severity == -1) {
        if(msg->severity > DEF_MAX_SEVERITY) {
            ignored_count++;
            return;
        }
    }
    else {
        if (msg->severity > max_severity) {
            ignored_count++;
            return;
	}
    }

    recursion_depth++;

    /* if queue is already full */
    if (tail_ptr->next == head_ptr) {
	if (tail_ptr->valid)
            entries_flushed_count++;            
	status = raise_interrupt_level(ALL_DISABLE);
	head_ptr = head_ptr->next;
        entry_ptr = tail_ptr = tail_ptr->next;
        reset_interrupt_level(status);
    } else {
        status = raise_interrupt_level(ALL_DISABLE);
        entry_ptr = tail_ptr = tail_ptr->next;
        reset_interrupt_level(status);
    }

    hist_index++;

    if (hist_index > MAX_INDEX) {
    /* Wrap the big index space, empty the queue, keeping the latest entry 
     * and start from fresh in order to keep the lexicographical ordering.
     * This won't happen very often, but paranoia can be handy.
     */
	entry_ptr = head_ptr;
	do {
	    entry_ptr->valid = FALSE;
	    entries_flushed_count++;
	    entry_ptr = entry_ptr->next;
	} while (entry_ptr != head_ptr);
	hist_index = 0;
    }

    /* fill the entry...first, check strings aren't too long, and then 
     * set entry values from event procedure call 
     */

    if (entry_ptr != NULL) {
	entry_ptr->hist_index = hist_index;

        if ((length = strlen(msg->fac)) <= MAX_FACILITY_SIZE) {
	    memcpy(entry_ptr->facility, msg->fac, length);
            entry_ptr->facility[length] = '\0';
	} else {
	    memcpy(entry_ptr->facility, msg->fac, MAX_FACILITY_SIZE - 1);
	    entry_ptr->facility[MAX_FACILITY_SIZE-1] = '*';
	    entry_ptr->facility[MAX_FACILITY_SIZE] = '\0';
        }

	entry_ptr->severity = msg->severity;

	if ((length = strlen(msg->name)) <= MAX_MESSAGE_NAME) {
	    memcpy(entry_ptr->msg_name, msg->name, length);
	    entry_ptr->msg_name[length] = '\0';
	} else {
	    memcpy(entry_ptr->msg_name, msg->name, MAX_MESSAGE_NAME - 1);
	    entry_ptr->msg_name[MAX_MESSAGE_NAME-1] = '*';
	    entry_ptr->msg_name[MAX_MESSAGE_NAME] = '\0';
        }

        if(!process_lock_semaphore(sp, MAXULONG)) {
	   syslog_drop_count++;
	   return;
	}
        length = vsprintf(msg_str, msg->format, args);
        process_unlock_semaphore(sp);
        if (length <= MAX_MESSAGE_TEXT) {
	    memcpy(entry_ptr->msg_text, msg_str, length);
	    entry_ptr->msg_text[length] = '\0';
	} else {
	    msg_str[MAX_MESSAGE_TEXT-1] = '*';
 	    msg_str[MAX_MESSAGE_TEXT] = '\0';
	    memcpy(entry_ptr->msg_text, msg_str, MAX_MESSAGE_TEXT - 1);
	}

	entry_ptr->timestamp = snmp_sysUpTime();

	entry_ptr->valid = TRUE;

	/* generate the notification, if it is wanted...*/

	if (syslog_notification_enabled == TRUE) {
	    reg_invoke_syslogEvent(entry_ptr);
	    syslog_notification_count++;
	}
    }
    recursion_depth--;
    return;
}

/*
 * set_history_logging
 * Called by configurator to setup syslog history
 */

void set_history_logging (parseinfo *csb)
{
    unsigned long new_table_size;

    if (csb->nvgen) {
	if (!log_init)
	    return;

	switch (csb->which) {
          case P_LOG_HISTORY:
	    if (max_severity != -1) {
                if (max_severity == GETOBJ(int,2)) {
		    nv_write(TRUE, "%s", csb->nv_command);
                }
	    }
            break;
	  case P_LOG_HISTORY_SIZE:       
            if (syslog_table_size != DEF_SYSLOG_TABLE_SIZE) {
                nv_write(TRUE, "%s %d", csb->nv_command, syslog_table_size);
	    }
            break;

	  default:
	    printf("\nUnknown flag to set_history_logging() %d", csb->which);
	    break;
	}
	return;
    }

    switch (csb->which) {
      case P_LOG_HISTORY:
        max_severity = (csb->sense ? GETOBJ(int,2) : -1);
        break;
      case P_LOG_HISTORY_SIZE:
        new_table_size = (csb->sense ? GETOBJ(int,1) : DEF_SYSLOG_TABLE_SIZE);
        if (new_table_size != syslog_table_size)
            syslog_history_resize(new_table_size);
        break;

      default:
	bad_parser_subcommand(csb, csb->which);
	break;
    }
}

void display_log_history (parseinfo *csb)
{
    sysloghistoryentry_t *entry_ptr = head_ptr;

    automore_enable(NULL);
    printf("\nSyslog History Table:");
    printf("%d maximum table entries,", syslog_table_size);
    printf("\nsaving level %s or higher", 
           max_severity == -1 ? logger_level(DEF_MAX_SEVERITY) :
	   logger_level(max_severity));
    printf("\n %d messages ignored, %d dropped, %d recursion drops", 
           ignored_count, logger_drops(), syslog_drop_count);
    printf("\n %d table entries flushed", entries_flushed_count);
    printf("\n SNMP notifications%s enabled",
	   syslog_notification_enabled ? "" : " not");
    if (syslog_notification_enabled)
        printf(", %d notifications sent", syslog_notification_count);
    while (entry_ptr->valid) {
        printf("\n   entry number %d : %s-%d-%s",
	       entry_ptr->hist_index, entry_ptr->facility,
	       entry_ptr->severity, entry_ptr->msg_name);
        printf("\n    %s", entry_ptr->msg_text);
        printf("\n    timestamp: %d", entry_ptr->timestamp);
	if (entry_ptr != tail_ptr)
	    entry_ptr = entry_ptr->next;	
	else
	    break;
    }
    return;
}

void syslog_history_resize(int new_size)
{
    int delta, i;
    leveltype status;
    sysloghistoryentry_t *entry_ptr;

    delta = new_size - syslog_table_size;
    if (delta > 0) {                /* adding entries */
	for (i = 0; i < delta; i++) {
	    entry_ptr = malloc_named(sizeof(sysloghistoryentry_t), 
                                     syslog_history_str);
            entry_ptr->valid = FALSE;
            status = raise_interrupt_level(ALL_DISABLE);
            entry_ptr->next = tail_ptr->next;
            tail_ptr->next = entry_ptr;
            reset_interrupt_level(status);
        }
    } else {                             /* removing entries */
        for (i = 0; i < abs(delta); i++) {
	    status = raise_interrupt_level(ALL_DISABLE);
	    entry_ptr = tail_ptr->next;
	    if (tail_ptr->next == head_ptr) {
		entries_flushed_count++;
		head_ptr = head_ptr->next;
	    }
	    tail_ptr->next = entry_ptr->next;
	    reset_interrupt_level(status);
	    free(entry_ptr);
	}
    }
    syslog_table_size = new_size;
    return;
}

unsigned long get_syslog_notification_count (void)
{
    return(syslog_notification_count);
}

unsigned long get_entries_flushed_count (void)
{
    return(entries_flushed_count);
}

unsigned long get_syslog_ignored_count (void)
{
    return(ignored_count);
}


/*
 * syslog_history subsystem header
 */

#define MAJVERSION_syslog_history 1
#define MINVERSION_syslog_history 0
#define EDITVERSION_syslog_history 0

SUBSYS_HEADER(syslog_history,
              MAJVERSION_syslog_history,
              MINVERSION_syslog_history,
              EDITVERSION_syslog_history,
              init_syslog_history,
              SUBSYS_CLASS_MANAGEMENT,
              NULL,
              NULL);
      
