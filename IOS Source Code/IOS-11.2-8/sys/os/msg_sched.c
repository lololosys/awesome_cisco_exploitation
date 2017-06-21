/* $Id: msg_sched.c,v 3.1.2.2 1996/09/14 00:00:57 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_sched.c,v $
 *------------------------------------------------------------------
 * M S G  _ S C H E D . C
 *
 * August 1996, Nick Thille
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_sched.c,v $
 * Revision 3.1.2.2  1996/09/14  00:00:57  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.1.2.1  1996/08/28  13:03:32  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.1  1996/08/25  01:17:48  thille
 * Placeholders for cleanup of message files and externs.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SCHED);				/* Define scheduler facility */
msgdef_section("Scheduler related error messages");


msgdef_limit(NOATTR, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to %s unknown process attribute %d.", 30*ONESEC);
msgdef_explanation(
	"Each process has various attributes associated with it.  This message
	indicates an attempt to set or get a nonexistent process attribute.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(ATTRCMD, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to %s attribute %s on process %d.", 30*ONESEC);
msgdef_explanation(
	"Each process has various public and private attributes associated with
	it.  This message indicates an attempt to set or get a private
	attribute belonging to another process.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef(ATTRVALUE, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to set bad %s value (%x) on process %d.");
msgdef_explanation(
	"Each process has various attributes associated with it.  This message
	indicates an attempt to set the specified attribute to an invalid
	value.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(SEMNOTLOCKED, SCHED, LOG_CRIT, MSG_TRACEBACK,
       "%s attempted to unlock an unlocked semaphore",
       30*ONESEC);
msgdef_explanation(
	"Some sections of code use semaphores to protect critical sections of
	software.  This message indicates an attempt to unlock a semaphore that
	was not locked.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_required_info("show process");
msgdef_tac_details(
	"If this error occurs, it is likely to be the result of multiple
	processes synchronizing on a semaphore and somehow getting out of sync.
	The name of the process should allow a development engineer to
	determine which semaphore is the problem, and the 'show process'
	information should let them determine what other which other processes
	would be accessing the same semaphore.");


msgdef_limit(SEMUNLOCK, SCHED, LOG_CRIT, MSG_TRACEBACK,
       "%s attempted to unlock semaphore owned by %s",
       30*ONESEC);
msgdef_explanation(
	"Some sections of code use semaphores to protect critical sections of
	software.  This message indicates an attempt to unlock a semaphore that
	was not owned by the process attempting to unlock it.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(EDISMSCRIT, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Critical/High priority process %s may not dismiss.", 30*ONESEC);
msgdef_explanation(
	"Each process executes at a specified priority level.  Higher priority
	processes must use the new scheduler primitives, while lower priority
	processes may use primitives from either the new or old scheduler.
	This message indicates that a higher priority process attempted to use
	an old scheduler primitives.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(WATCH, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to %s uninitialized watched %s (address %x).", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates an attempt to register for an event
	without first creating the data structure for that event.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(NOTWATCHTIMER, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "%s timer %x not being watched.", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates an attempt to deregister a 
	timer expiration event that is not currently registered.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(QUEUENOTEMPTY, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to delete non-empty watched queue %s (address %x).", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates an attempt to destroy a queue that
	still contained items.  These items are lost and not returned to free
	memory.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef(SETHIGHORCRIT, SCHED, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to set %s priority on process %d (%s).");
msgdef_explanation(
	"Each process executes at a specified priority level.  Higher priority
	processes must use the new scheduler primitives, while lower priority
	processes may use primitives from either the new or old scheduler.
	This message indicates an attempt to set the specified priority too
	high for a process using old scheduler primitives.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(UNEXPECTEDEVENT, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Process received unknown event (maj %x, min %x).", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a process received an event it
	does not know how to handle.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_required_info("show process");
msgdef_tac_details(
	"If this error occurs there are several possible sources for the
	problem.  This error is most likely the result of one process directly
	waking up another process and passing it major/minor event numbers.  If
	the sending process wakes up the wrong process, the receiving process
	will not know what to do with the major/minor event number received.
	The process may perform the wrong action if it is expecting an event
	with matching major/minor event numbers, or it may print this error
	message.  The output of 'show process' would be helpful to determine
	which processes might have sent a direct wakeup to a process.  Another
	possible cause for this problem is that a programmer added the code to
	register for an event but didn't add the code to handle the event.
	Also, a subroutine called by the process could also have registered for
	a new event but not deregistered the event before exiting.");


msgdef_limit(UNEXPECTEDQUEUE, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Queue event for unknown queue (index %d).", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a process received a queuing
	event it for an unknown queue.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_tac_details(
	"If this error occurs there are several possible sources for the
	problem.  This error is most likely the result of a subroutine
	registering for a new queue but not deregistering the queue before
	exiting.  Another possible cause for this problem is that a programmer
	added the code to register for an queue but didn't add the code to
	handle the event.  It is also possible that one process directly woke
	up another process and passing it a 'Queue Event' message.  This is
	very unlikely as the sending of 'Queue Events' is built into the
	enqueueing function.  There is never a need to directly send a 'Queue
	Event' to a process.");


msgdef_limit(UNEXPECTEDTIMER, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Unknown timer expiration, timer = %x, type %d", 10 * ONESEC); 
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a process received an timer
	expiration event for an unknown timer.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_tac_details(
	"If this error occurs there are several possible sources for the
	problem.  This error is most likely the result of a subroutine
	registering for a new timer but not deregistering the timer before
	exiting.  Another possible cause for this problem is that a programmer
	added the code to register for an timer but didn't add the code to
	handle the event.  It is also possible that one process directly woke
	up another process and passing it a 'Timer Event' message.  This is
	very unlikely as the sending of 'Timer Events' is built into the
	scheduler.  There is never a need to directly send a 'Timer
	Event' to a process.");


msgdef_limit(UNEXPECTEDMESSAGE, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Unknown message %x received (ptr arg %x, num arg %x).", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a process received a message from
	another process that it does not know how to handle.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_required_info("show process");
msgdef_tac_details(
	"If this error occurs there are several possible sources for the
	problem.  This error is most likely the result of one process directly
	waking up another process and passing it a message.  If the sending
	process wakes up the wrong process, the receiving process may not know
	what to do with the received message.  The process may perform the
	wrong action if it is expecting a message with the same identifier, or
	it may print this error message.  The output of 'show process' would be
	helpful to determine which processes might have sent a message to a
	process.  Another possible cause for this problem is that a programmer
	added a registry callback to be notified by message when an event
	occurred, but didn't add the code to handle the event.");


msgdef_limit(STACKSIZE, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Illegal stack size (%d) specified for process %s.", 30*ONESEC);
msgdef_explanation(
	"Each process has its own stack space, which must be one of several
	pre-defined sizes. This message indicates an attempt to specify an
	illegal stack size.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(PRIORITY, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Illegal priority (%d) specified for process %s.", 30*ONESEC);
msgdef_explanation(
	"Each process executes at a specified priority level.  Higher priority
	processes must use the new scheduler primitives, while lower priority
	processes may use primitives from either the new or old scheduler.
	This message indicates an attempt to set the specified process to a
	priority level that is not allowed for the type (new/old) of scheduler
	primitives used.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef(PAGEZERO, SCHED, LOG_ERR, MSG_TRACEBACK,
       "Low memory modified by %s (%#x = %#x).");
msgdef_explanation(
	"In all Cisco products the first 256 bytes of memory is unused and off
	limits.  Newer platforms have hardware to immediately trap reads or
	writes to this area.  Older platforms periodically perform checks on
	this memory.  This message appears only on older platforms and
	indicates that this off limits memory area was modified.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_tac_details(
	"If this error indicates a process like 'Per-minute Jobs' 'TTY
	Background' or 'Timers', the problem will be difficult to diagnose.
	These processes call a set of routines that is registered during the
	router's execution.  The output of 'show process' will be necessary in
	this case to determine which callback routines have been registered, and
	then each of those callback routines will need to be inspected to see
	if it could be the source of the problem.");


msgdef_limit(LOSTWAKEUP, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Wakeup information for process %s lost (maj %x, min %x).", 30*ONESEC);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that an event for the specified process
	was lost and that the router may not be functioning correctly.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
msgdef_tac_details(
	"The scheduler only supports one outstanding "direct" wakeup per
	process.  If a second (or subsequent) direct wakeup is sent to a
	process before it has acted upon the first wakeup, and the major and
	minor numbers of the new wakeup are not identical to the first, the new
	wakeup information will be dropped and this error message printed.");


msgdef_limit(STUCKTMR, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Sleep w/expired timer %x, time %#Ta (%TE ago).", ONEMIN);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a registered timer is expired, and
	that its value is unchanged after the process has executed two
	successive times.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(STUCKMTMR, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Sleep w/expired mgd timer %x, time %#Ta (%TE ago).", ONEMIN);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a registered timer is expired, and
	that its value is unchanged after the process has executed two
	successive times.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef_limit(THRASHING, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Process thrashing on watched %s.", ONEMIN);
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that fifty consecutive timer that the
	process relinquished the processor there were still outstanding events
	to be processed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef(STILLWATCHING, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Process still watching %s %s.");
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a process destroyed a set of
	events without first individually removing each events in the set.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef(STILLWATCHINGT, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Process still watching %s timer %x.");
msgdef_explanation(
	"A processes can register to be notified when various events occur in the
	router.  This message indicates that a process destroyed a set of
	events without first individually removing each events in the set.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");


msgdef(CORRUPT, SCHED, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Scheduler '%s' corrupted by process %s (expected %x, found %x).");
msgdef_explanation(
	"");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it
	 to your technical support representative."); 
msgdef_ddts_component(
	"Determine the component from the process name displayed in the
	error message.");
