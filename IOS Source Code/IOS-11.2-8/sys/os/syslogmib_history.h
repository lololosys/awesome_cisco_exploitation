/* $Id: syslogmib_history.h,v 3.1.48.4 1996/09/14 00:55:22 ramk Exp $
 * $Source: /release/112/cvs/Xsys/os/syslogmib_history.h,v $
 *------------------------------------------------------------------
 * syslogmib_history.h - Syslog History Database
 *
 * September 1995, Anke Dosedal
 *
 * Copyright (c) 1995-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: syslogmib_history.h,v $
 * Revision 3.1.48.4  1996/09/14  00:55:22  ramk
 * CSCdi66329:  SNMP syslog traps specify wrong enterpriseOID,sysLogEvent
 * needs fix
 * Branch: California_branch
 * Pass a (pointer to) sysloghistoryentry_t as a parameter to syslogEvent
 *
 * Revision 3.1.48.3  1996/08/28  13:04:33  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1.48.2  1996/06/27  19:11:35  anke
 * CSCdi58471:  Invalid memory action in syslog mib
 * Pre-allocate the memory that might be needed so we don't get caught
 * during an interupt.  Change to queue to a circular one to reduce
 * amount of pointer moving that might need to happen.
 * Branch: California_branch
 *
 * Revision 3.1.48.1  1996/05/21  06:36:43  mordock
 * Branch: California_branch
 * Improve snmp modularity via creating services to register interfaces
 * and chassis cards.
 * Add syslog mib.
 *
 * Revision 3.1  1996/01/19  03:24:24  anke
 * Placeholder for Syslog MIB
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef _syslogmib_history_h_
#define _syslogmib_history_h_


/* Parameters */

#define DEF_SYSLOG_TABLE_SIZE 1 /* default number of entries in syslog table */
#define DEF_MAX_SEVERITY 4      /* default maximum severity */
#define MAX_INDEX 0x7FFFFFFF    /* maximum event index number */
#define MAX_FACILITY_SIZE 20   /* maximum size of syslog facility string */
#define MAX_MESSAGE_NAME 30    /* maximum size of syslog message name string */
#define MAX_MESSAGE_TEXT 255   /* maximum size of syslog message text string */
#define MAX_SYSLOG_TABLE_SIZE 500 /* maximum number of entries in table */

/* Type Definitions */

struct sysloghistoryentry_t_ {
        struct sysloghistoryentry_t_  *next;
	boolean                       valid;
	long                          hist_index;
        char                          facility[MAX_FACILITY_SIZE+1];
        long                          severity;
        char                          msg_name[MAX_MESSAGE_NAME+1];
        char                          msg_text[MAX_MESSAGE_TEXT+1];
        unsigned long                 timestamp;
};


/* Globals */

extern unsigned long syslog_table_size; /* user defined history table size */
extern boolean  syslog_notification_enabled; /* notification enabled flag */
extern unsigned long max_severity;    /* user defined maximum severity level */

/* Procedure Prototypes */

extern long logger_drops(void);

sysloghistoryentry_t *syslog_history_find(long, boolean);
void syslog_history_event(const message *msg, va_list args);
void syslog_history_resize(int new_size);
unsigned long get_syslog_notification_count(void);
unsigned long get_entries_flushed_count(void);
unsigned long get_syslog_ignored_count(void);

/*
 * loghist_chain.c
 */
extern void syslog_parser_init(void);

#endif _syslogmib_history_h_

