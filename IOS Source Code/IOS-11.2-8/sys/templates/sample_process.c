/* $Id$
 * $Source$
 *------------------------------------------------------------------
 * One line description here.
 *
 * Month Year, Your Name
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log$
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "logger.h"
#include "../os/msg_sched.c"		/* Not a typo, see logger.h */

enum queue_types {
    PACKETS,
    OTHER_STUFF
};

enum timer_types {
    TIMER_TYPE_FOO,
    TIMER_TYPE_BAR
};

watched_queue *packetQ, *stuffQ;
managed_timer process_root_timer;
managed_timer foo_timer, bar_timer;

/*
 * process_timers
 *
 * Process any expired timers.  This fragment shows how to process all
 * expired timers in the processes private managed timers tree.  This is
 * also where a watched simple timers would be checked if the process was
 * using simple timers.
 */
void process_timers (void)
{
    mgd_timer *expired_timer;
    private_struct *foo;
    another_struct *bar;

    while (mgd_timer_expired(&process_root_timer)) {
	expired_timer = mgd_timer_first_expired(&process_root_timer);
	switch (mgd_timer_type(expired_timer)) {
	  case TIMER_TYPE_FOO:
	    foo = mgd_timer_context(expired_timer);
	    mgd_timer_stop(expired_timer);
	    foo_action_routine();
	    break;
	    
	  case TIMER_TYPE_BAR:
	    bar = mgd_timer_context(expired_timer);
	    mgd_timer_update(expired_timer, BAR_WAKEUP_INTERVAL);
	    bar_action_routine();
	    break;

	  default:
	    errmsg(&msgsym(UNEXPECTEDTIMER,SCHED), expired_timer,
		   mgd_timer_type(expired_timer));
	    mgd_timer_stop(expired_timer);
	    break;
	}
    }
}

/*
 * process_queues
 *
 * Process the individual queue that had something added to it.  The
 * caller is told which queue had the event, so it is not necessary to
 * scan all queues looking for the right one.  It is not necessary to
 * completely drain a queue before returning.  The process_wait_for_event
 * routine is smart enough to notice the queue stil has items in it, and
 * will wake up at the next possible changes and return another queue
 * indication for the same queue.
 */
void process_queues (minor)
{
    int count;

    switch (minor) {
      case PACKETS:
	count = PAK_SWITCHCOUNT;
	while (count-- > 0) {
	    pak = process_dequeue(packetQ);
	    /* do per packet stuff */
	}
	break;
      case OTHER_STUFF:
	stuff = process_dequeue(stuffQ);
	/* do other stuff */
	break;
      default:
	errmsg(&msgsym(UNEXPECTEDQUEUE,SCHED), minor);
	break;
    }
}

/*
 * process_messages
 *
 * Handle mesages from other processes.  There is just a single MSG_EVENT
 * posted, so this routine must check the message queue to learn the
 * specific message.  It is not necessary to completely drain the message
 * queue before returning.  The process_wait_for_event routine is smart
 * enough to notice there are messages outstanding, and will wake up at
 * the next possible changes and return another message indication.
 */
void process_messages (void)
{
    ulong message, value;
    void *pointer;
    
    while (process_get_message(&message, &pointer, &value)) {
	switch (message) {
	  case FOO:
	    /* do "foo" things */
	    break;
	  case BAR:
	    /* do "bar" things */
	    break;
	  case BAZ:
	    /* do "baz" things */
	    break;
	  default:
	    errmsg(&msgsym(BADMESSAGE,SYS), message, pointer, value);
	    break;
	}
    }
}

/*
 * process_teardown 
 *
 * This process is being killed.  Remove any data structures that this
 * process set up when it started.
 */
void process_teardown (int signal, int dummy1, void *dummy2, char *dummy3)
{
    paktype *pak;

    process_watch_mgd_timer(&process_root_timer, DISABLE);
    process_watch_queue(packetQ, DISABLE, RECURRING);
    process_watch_queue(stuffQ, DISABLE, RECURRING);
    while ((pak = process_dequeue(packetQ)) != NULL)
	retbuffer(pak);
    while ((pak = process_dequeue(stuffQ)) != NULL)
	free(pak);
    delete_watched_queue(&packetQ);
    delete_watched_queue(&stuffQ);
}

/*
 * main_process
 *
 * The "main" process for a protocol, or whatever.
 */
void main_process (void)
{
    ulong major, minor;
    paktype *pak;

    /*
     * Setup process private structures
     */
    packetQ = create_watched_queue("some packets", 0, PACKETS);
    stuffQ = create_watched_queue("some stuff", 20, OTHER_STUFF);
    process_watch_queue(packetQ, ENABLE, RECURRING);
    process_watch_queue(stuffQ, ENABLE, RECURRING);
    process_watch_mgd_timer(&process_root_timer, ENABLE);
    signal_permanent(SIGEXIT, process_teardown);

    while (TRUE) {
	/*
	 * Wait for the next event to occur.
	 */
	process_wait_for_event();
	while (process_get_wakeup(&major, &minor)) {
	    switch (major) {
	      case TIMER_EVENT:
		process_timers();
		break;

	      case QUEUE_EVENT:
		process_queues(minor);
		break;

	      case MESSAGE_EVENT:
		process_messages();
		break;

	      default:
		errmsg(&msgsym(UNEXPECTEDEVENT,SCHED), major, minor);
		break;
	    }
	}
    }
}

void process_init ()
{
    mgd_timer_init_parent(&process_root_timer, NULL);
    mgd_timer_init_leaf(&foo_timer, &process_root_timer,
			TIMER_TYPE_FOO, foo_data, FALSE);
    mgd_timer_init_leaf(&bar_timer, &process_root_timer,
			TIMER_TYPE_BAR, foo_data, FALSE);
}
