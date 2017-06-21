/* $Id: msg_system.c,v 3.10.8.11 1996/09/14 00:01:05 hampton Exp $
 * $Source: /release/112/cvs/Xsys/os/msg_system.c,v $
 *------------------------------------------------------------------
 * msg_system.c - Message file for system facility
 *
 * July 17, 1990, Kevin Paul Herbert
 *
 * Copyright (c) 1990-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: msg_system.c,v $
 * Revision 3.10.8.11  1996/09/14  00:01:05  hampton
 * Incorporate the Error Messages Manual text into the source code.
 * [CSCdi69164]
 * Branch: California_branch
 *
 * Revision 3.10.8.10  1996/08/28  13:03:41  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.10.8.9  1996/08/24  00:20:17  sberl
 * CSCdi66828:  process_create_common should validate start address
 * Branch: California_branch
 * Check that process start address is not NULL and it is not greater than
 * etext.
 *
 * Revision 3.10.8.8  1996/08/06  23:34:56  ahh
 * CSCdi64520:  %SYS-2-GETBUFFFAIL: Memory allocation of 67 bytes failed
 * Error message is confusing, make it more obvious that it is a buffer
 * allocation that is failing, not malloc.
 * Branch: California_branch
 *
 * Revision 3.10.8.7  1996/08/03  23:34:49  fox
 * CSCdi63400:  high priority compatibility queue must die
 * Branch: California_branch
 * Remove high priority compatibility queue and activatehigh flag.
 * Convert high priority cfork'd processes to new-style processes with
 * watched queues, timers, and booleans.  Split Multilink PPP input
 * into separate high priority process.  Split load and busy calculations
 * into separate processes.  Speed up PPP input and scheduler processing.
 * Miscellaneous code changes.
 *
 * Revision 3.10.8.6  1996/07/23  13:27:08  thille
 * CSCdi63329: Centralize logging for getbuffer() and pak_duplicate()
 * failures
 * Branch: California_branch
 * Shamelessly rip off Mr. Katz' code from CSCdi36037.  Provide
 * centralized logging for getbuffer() and pak_duplicate() failures and
 * then rip out the 9 bazillion different messages that where in the
 * various callers to report the failure.
 *
 * Revision 3.10.8.5  1996/06/19  23:17:19  smackie
 * The chunk manager displays it's new look for 1996. (CSCdi60779)
 *
 *   o Soothe the raging masses and implement lockable chunks
 *   o Start hiding chunk structures and defines in a private header
 *   o Add a chain file for "show chunk" and remove entry from chain.c
 *   o Tidy up some redundant sections of code
 *
 * Branch: California_branch
 *
 * Revision 3.10.8.4  1996/05/08  00:06:41  hampton
 * Add better semaphore errors and better code to wait for a semaphore.
 * The routine process_wait_for_event_timed() shouldn't post a TIMER_EVENT
 * when the temporary timer expires.  [CSCdi56999]
 * Branch: California_branch
 *
 * Revision 3.10.8.3  1996/04/29  05:25:55  pst
 * CSCdi55878:  Rate limit regular expression error messages.
 * Branch: California_branch
 *
 * Revision 3.10.8.2  1996/04/04  09:37:44  smackie
 * Un-obfuscate buffer copy code for prolonged simplicity. Remove
 * misguided internal particle structure from paktype. Add a discrete
 * pak_coalesce() call for particle re-assembly. (CSCdi51223)
 * Branch: California_branch
 *
 * Revision 3.10.8.1  1996/03/27  21:52:01  hampton
 * Try to catch and report thrashing processes.  A few other minor
 * scheduler cleanups and enhancements.  [CSCdi50130]
 * Branch: California_branch
 *
 * Revision 3.10  1996/02/28  09:33:27  hampton
 * CSCdi50135:  Consolidate process creation error messages
 *
 * Revision 3.9  1996/02/21  01:20:38  hampton
 * Catch bad offsets to passive timer routines.  [CSCdi49419]
 *
 * Revision 3.8  1996/02/04  12:16:58  shaker
 * CSCdi47971:  RSP-3-NORESTART error message
 * Use portable array references instead of post incremented pointer
 * dereferences when passing arguments to errmsg(), because the order
 * of function argument evaluation varies.
 *
 * Revision 3.7  1996/01/25  23:52:29  hampton
 * Add support to the scheduler for nesting wakeup event lists.  This
 * should make it easier to convert portion of the code.  Also allow for
 * the creation of a detached list of watched events.  [CSCdi46566]
 *
 * Revision 3.6  1996/01/25  18:38:04  hampton
 * Fix per-process heap checking code.  Also convert it from conditional
 * compilation to an "internal" command.  [CSCdi47517]
 *
 * Revision 3.5  1996/01/25  11:20:29  smackie
 * Registry code gets some much-needed hot-oil massaging, free liposuction
 * and a makeover.  (CSCdi47101)
 *
 *    o Compress registry structures. Saves over 120K for all platforms
 *    o Add registry subsystems and remove feature registry initializion
 *      from registry.c to fix woeful initialization scaling problems
 *    o Remove unused and seemingly unloved registry debugging code
 *    o Add registry memory statistics to shame people into action
 *
 * Revision 3.4  1995/11/18  06:06:16  hampton
 * Don't let the entire router be dragged down by a single process with a
 * busted timer.  [CSCdi44073]
 *
 * Revision 3.3  1995/11/17  18:49:53  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:20  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:47:09  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.12  1995/09/25  08:37:11  tli
 * CSCdi40960:  ISP jumbo patch 2
 *
 * Revision 2.11  1995/09/23  05:57:46  enf
 * CSCdi39189:  100X need insurance against loading incorrect image
 *
 * Revision 2.10  1995/09/23  00:18:02  hampton
 * Allow a semaphore to be unlocked from an interrupt handler.
 * [CSCdi40930]
 *
 * Revision 2.9  1995/09/05 16:37:58  hampton
 * Fix autoinstall over frame relay.  [CSCdi38852]
 *
 * 1) Udp should only add one-shot queue watches inside of read_ipsocket.
 * 2) Allow a existing one-shot watch to be upgraded to a recurring watch,
 *    but prevent the downgrade of an existing recurring watch to a
 *    one-shot watch.
 * 3) Add some conditionally compiled debugging to the scheduler.
 *
 * Revision 2.8  1995/08/15 23:24:54  tli
 * CSCdi36981:  Need to be able to generate downward compatible
 * configurations
 *
 * Revision 2.7  1995/07/24  09:23:54  hampton
 * Requested scheduler additions.  [CSCdi37542]
 *
 * Revision 2.6  1995/07/15 02:37:06  thille
 * CSCdi37049:  Console output from SNMP WriteNet SET confuses console user
 * Add a silent_mode parameter to some routines and registries and set it
 * to TRUE when making the calls from SNMP WriteNet, netConfigSet, or
 * hostConfigSet.  Also, add an errmsg for each so the event can still get
 * logged, and make it clear what caused the event.  Fix other callers to
 * pass FALSE so current behavior will be maintained.
 *
 * Revision 2.5  1995/07/01  19:15:08  richl
 * CSCdi34435:  %SYS-3-SUPNONE: Registry 6 doesnt exist
 *              o Changed above msg so it isn't rate based
 *              o Protected call to registry with conditional statement
 *              o Resolved conflict in DRIVER/PROTOCOL class between
 *                the cbus_bridge and tbridge subsystems.
 *
 * Revision 2.4  1995/06/26  23:02:59  hampton
 * Catch and complain about calls to printf that are made while interrupts
 * are disabled.  [CSCdi36437]
 *
 * Revision 2.3  1995/06/18 06:03:37  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase I--add centralized processing for malloc failures.
 *
 * Revision 2.2  1995/06/14  20:39:42  anke
 * CSCdi35807:  Yet more spellink errors
 *
 * filesys/fslib.c            suported -> supported
 * os/msg_system.c            unititialized -> uninitialized
 * parser/cfg_routemap_set.h  substract -> subtract
 * smf/smf_debug_flags.h      missses -> misses
 *
 * Revision 2.1  1995/06/07  21:57:11  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef __MSG_SYSTEM_C__
#define __MSG_SYSTEM_C__

#include "master.h"
#define DEFINE_MESSAGES	TRUE
#include "logger.h"


facdef(SYS);				/* Define system facility */
msgdef_section("Operating system error messages");


msgdef_limit(MALLOCFAIL, SYS, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
	     "Memory allocation of %d bytes failed from %#x, pool %s, "
	     "alignment %d", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"The requested memory allocation is not available from the specified
	memory pool. The current system configuration, network environment, or
	possibly a software error might have exhausted or fragmented the
	router's memory.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(NOMEMORY, SYS, LOG_CRIT, 0,
	     "No memory available for %s %d", MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An operation could not be accomplished because of a low-memory
	condition.");
msgdef_recommended_action(
	"Reduce other system activity to ease memory demands. If conditions
	warrant, upgrade to a larger memory configuration.");
msgdef_ddts_component("");


msgdef_limit(GETBUFFFAIL, SYS, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
	     "%s buffer allocation (%d bytes) failed from %#x",
	     MSGDEF_LIMIT_SLOW);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOBRIDGE, SYS, LOG_INFO, 0, "Bridging software not present");
msgdef_explanation(
	"Your system is not equipped to be a bridge.");
msgdef_recommended_action(
	"Install the bridging software option.");
msgdef_ddts_component("");


msgdef(SOCKUNKN, SYS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
       "Unknown socket protocol %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(HARIKARI, SYS, LOG_ERR, 0, "Process %s top-level routine exited");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADPID, SYS, LOG_CRIT, MSG_PROCESS | MSG_TRACEBACK,
       "Bad pid %d for tty %t");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(MSGLOST, SYS, LOG_ERR, 0, 
       "%d messages lost due to queue overflow");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef_limit(REGEXP, SYS, LOG_WARNING, MSG_PROCESS | MSG_TRACEBACK, "%s", 10*ONESEC);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(RESTART, SYS, LOG_NOTICE, 0, "System restarted --\n%s");
msgdef_explanation(
	"A reload or restart was requested.");
msgdef_recommended_action(
	"Notification messages only. No action required.");
msgdef_ddts_component("");


msgdef(RELOAD, SYS, LOG_NOTICE, 0, "Reload requested");
msgdef_explanation(
	"A reload or restart was requested.");
msgdef_recommended_action(
	"Notification messages only. No action required.");
msgdef_ddts_component("");


msgdef(RELOAD_ATTEMPT, SYS, LOG_WARNING, 0, 
       "Attempt via SNMP failed, system-shutdown not configured");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef(DUAL_MESSAGE, SYS, LOG_WARNING, 0, 
       "SNMP system message request %d denied due to pending job");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CONFIG, SYS, LOG_NOTICE, 0, "Configured from %s by %s %s");
msgdef_explanation(
	"The router's configuration was changed.");
msgdef_recommended_action(
	"Notification messages only. No action required.");
msgdef_ddts_component("");


msgdef(CONFIG_I, SYS, LOG_NOTICE, 0, "Configured from %s by %s");
msgdef_explanation(
	"The router's configuration was changed.");
msgdef_recommended_action(
	"Notification messages only. No action required.");
msgdef_ddts_component("");


msgdef(CONFIG_NV, SYS, LOG_NOTICE, 0,
       "Non-volatile store configured from %s by %s %s");
msgdef_explanation(
	"The configuration was written successfully.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef(CONFIG_NV_ERR, SYS, LOG_ERR, 0,
       "Non-volatile store write error - configuration failed");
msgdef_explanation(
	"The write of the nonvolatile configuration failed.");
msgdef_recommended_action(
	"If this message recurs, contact your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(CONFIG_NEWER, SYS, LOG_WARNING, 0, "Configurations from "
       "version %d.%d may not be correctly understood.");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(CONFIG_NOLOCK, SYS, LOG_WARNING, 0, "The configuration could "
       "not be locked");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it "
	"to your technical support representative.");
msgdef_ddts_component("");


msgdef(CONFIG_DOWNFAIL, SYS, LOG_CRIT, 0, "Generated configuration not "
       "downward compatible");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(OUTSETUP, SYS, LOG_NOTICE, 0,
       "Configuration file %s accepted, aborting setup");
msgdef_explanation(
	"A configuration was accepted through the network, so the setup session
	will be aborted.");
msgdef_recommended_action(
	"No action required.");
msgdef_ddts_component("");


msgdef_limit(CPUHOG, SYS, LOG_ERR, MSG_TRACEBACK,
	     "Task ran for %d msec (%d/%d), Process = %s, PC = %x", 10*ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(WATCHDOG, SYS, LOG_CRIT, MSG_TRACEBACK,
	     "Process aborted on watchdog timeout, Process = %s", ONESEC);
msgdef_explanation(
	"The indicated process ran for such a long time without relinquishing
	the processor that the system killed it as a runaway.");
msgdef_recommended_action(
	"If this error recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef_limit(TIMERHOG, SYS, LOG_ERR, MSG_TRACEBACK,
	     "Timer callback ran long, PC = %x", 10*ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef(STACKLOW, SYS, LOG_INFO, 0, "Stack for %s %s running low, %d/%d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INPUTQ, SYS, LOG_CRIT, MSG_TRACEBACK,
       "INPUTQ set, but no idb, ptr=%x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADSHARE, SYS, LOG_CRIT, MSG_TRACEBACK,
       "Bad refcount in %s, ptr=%x, count=%x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef_limit(CFORKLEV, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "process create of %s failed (at level %d)", 30*ONESEC);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CFORKMEM, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "process create of %s failed (no memory)", 30*ONESEC);
msgdef_explanation(
	"No memory was available to create a process.");
msgdef_recommended_action(
	"If you suspect a memory leak, copy this message exactly as it appears,
	and report it to your technical support representative. If a memory
	leak does not appear to be the problem, ensure that more memory is
	available to support the additional features.");
msgdef_ddts_component("");


msgdef(CFORKBADFUNCT, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Can't create process with start address = %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADPROCESS, SYS, LOG_ERR, 0,
       "Bad process ID (%d) when %s process table");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INSCHED, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "%s within scheduler");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INTSCHED, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "'%s' at level %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(MEMDUMP, SYS, LOG_INFO, 0, "%#x: %#x %#x %#x %#x");
msgdef_explanation(
	"This message is sent in conjunction with the "%SYS-6-BLKINFO" message.
	It provides additional data.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


#define NOBLOCK_LIMIT (30*ONESEC)	/* Only send once per 30 secs */
msgdef_limit(NOBLOCK, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "%s with blocking disabled", NOBLOCK_LIMIT);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef_limit(BLOCKHUNG, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Task hung with blocking disabled, value = 0x%x", NOBLOCK_LIMIT);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(NOTDEAD, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Rekilling process %s, pid %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOPROCESS, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "No such process %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, contact your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(BADSTACK, SYS, LOG_CRIT, 0,
       "Process %s has trashed stack, old size %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


#define ADDRPC " %-8x,%8x"
msgdef(MTRACE, SYS, LOG_INFO, 0, "%s: addr,pc\n"
       ADDRPC ADDRPC ADDRPC ADDRPC "\n"
       ADDRPC ADDRPC ADDRPC ADDRPC);
msgdef_explanation(
	"The free memory pool is corrupted.");
msgdef_recommended_action(
	"If this message recurs, contact your technical support representative
	for assistance.");
msgdef_ddts_component("");
#undef ADDRPC


msgdef(BLKINFO, SYS, LOG_INFO, 0,
       "%sblk %x, words %u, alloc %x, %s, dealloc %x, rfcnt %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(PROCINFO, SYS, LOG_INFO, MSG_TRACEBACK,
       "Process %s just finished executing.");
msgdef_explanation(
	"This message is an auxiliary to the "SYS-3-OVERRUN" and
	"SYS-3-BADMAGIC" error messages. It identifies the process or process
	fragment that was executing at the time of the error.");
msgdef_recommended_action(
	"Copy this messages exactly as it appears, and report it, along with
	the "SYS-3-OVERRUN" and "SYS-3-BADMAGIC" error messages, to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(TESTINFO, SYS, LOG_INFO, 0,
       "Scheduler test %#x (for %s) just executed.");
msgdef_explanation(
	"This message is an auxiliary to the "SYS-3-OVERRUN" and
	"SYS-3-BADMAGIC" error messages. It identifies which process or process
	fragment was executing at the time of the error.");
msgdef_recommended_action(
	"Copy this message exactly as it appears, and report it, along with the
	"SYS-3-OVERRUN" and "SYS-3-BADMAGIC" error messages, to your technical
	support representative.");
msgdef_ddts_component("");


msgdef(BADBLOCK, SYS, LOG_ERR, MSG_TRACEBACK,
       "Bad block pointer %x");
msgdef_explanation(
	"A block of memory at the indicated location is corrupt.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADMAGIC, SYS, LOG_ERR, MSG_TRACEBACK,
       "Corrupt block at %x (magic %08x)");
msgdef_explanation(
	"The free memory pool is corrupted.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(OVERRUN, SYS, LOG_ERR, MSG_TRACEBACK,
       "Block overrun at %x (redzone %08x)");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, contact your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(FREEBAD, SYS, LOG_CRIT, MSG_TRACEBACK,
       "Attempted to free memory at %x, not part of buffer pool");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(FREEFREE, SYS, LOG_CRIT, MSG_TRACEBACK,
       "Attempted to free unassigned memory at %lx, alloc %lx, dealloc %lx");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INVMEMINT, SYS, LOG_ERR, MSG_TRACEBACK,
       "Invalid memory action (%s) at interrupt level");
msgdef_explanation(
	"This message indicates that a memory allocation or deallocation was
	attempted from an interrupt handler.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(INUSEFREE, SYS, LOG_ERR, MSG_TRACEBACK,
       "Block %x on free list %d in use");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NZREFCNT, SYS, LOG_ERR, MSG_TRACEBACK,
       "Block %x on free list %d with ref count %d");
msgdef_explanation(
	"A block of memory in the free pool is actually in use. The number of
	processes using the memory block is indicated.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(WRONGFREELIST, SYS, LOG_ERR, MSG_TRACEBACK,
       "Block %x, size %d, on wrong free list (%d, %d)");
msgdef_explanation(
	"A freed buffer was placed on the wrong list of free buffers.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADFREEMAGIC, SYS, LOG_ERR, MSG_TRACEBACK,
       "Corrupt free block at %x (magic %08x)");
msgdef_explanation(
	"A block of memory at the indicated location is corrupt.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(BADFREEPTRS, SYS, LOG_ERR, MSG_TRACEBACK,
       "Bad %s pointer %x at %x (%s = %x)");
msgdef_explanation(
	"A pointer in the block header at the indicated location is corrupt.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(GETBUF, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Bad getbuffer, bytes= %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(INLIST, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Buffer in list, ptr= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SHARED, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to return buffer with sharecount %d, ptr= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


#ifdef BUFRETDEBUG
msgdef(INLIST1, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Buffer in list, ptr= %x, caller= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(SHARED1, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to return buffer with sharecount %d, ptr= %x, caller= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");
#endif


msgdef(BADPOOL, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to use buffer with corrupt pool pointer, ptr= %x, pool= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADPOOLMAGIC, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to use pool pointer with corrupt header, pool= %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(WRONGPOOL, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Buffer %x found in pool %x, should be in pool %x");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef(BADBUFFER, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to use %s buffer as %s, ptr= %x, pool= %x");
msgdef_explanation(
	"A hardware or software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(QCOUNT, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Bad %s %x count %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(NOTQ, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "%s didn't find %x in queue %x");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(LINKED, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Bad %s of %x in queue %x");
msgdef_explanation(
	"The router is running short on memory.");
msgdef_recommended_action(
	"It may be necessary to free some memory by exiting a program. If this
	message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(SELFLINKED, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Buffer %x linked to itself in free pool %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(ALREADYFREE, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Buffer %x already in free pool %s");
msgdef_explanation(
	"A block of memory at the indicated location is corrupt.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(LOWMEM, SYS, LOG_CRIT, 0,
       "Low memory debug exception (limit=%u free=%u)");
msgdef_explanation(
	"The router is running short on memory.");
msgdef_recommended_action(
	"It may be necessary to free some memory by exiting a program. If this
	message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(NOMEMFRG, SYS, LOG_CRIT, 0,
       "Memory fragmentation check debug exception (fragment size %u)");
msgdef_explanation(
	"The system detected an error while checking for memory fragmentation.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(SPEC, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Trying to set unknown special character %d to %d");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(BADDISP, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Bad dispose code %x in %s");
msgdef_explanation(
	"A block of memory is corrupt at the indicated location.");
msgdef_recommended_action(
	"If this message recurs, call your technical support representative for
	assistance.");
msgdef_ddts_component("");


msgdef(NULLIDB, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Null idb in %s");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If any of these messages recur, call your technical support
	representative for assistance.");
msgdef_ddts_component("");


msgdef(DUPEXPORT, SYS, LOG_CRIT, MSG_TRACEBACK|MSG_PROCESS,
       "Duplicate export vector %s registration attempt\n");
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"If this message recurs, contact your technical support representative
	for assistance.");
msgdef_ddts_component("");


msgdef(OVERTEMP, SYS, LOG_ALERT, 0,
       "System detected OVERTEMPERATURE condition.\nPlease resolve cooling problem immediately!\n");
msgdef_explanation(
	"The environmental monitor detected a high-temperature condition.");
msgdef_recommended_action(
	"Make sure that the room temperature is not too high and that air flow
	to the card is not blocked. If this condition persists, the
	environmental monitor might shut the system down. Call your technical
	support representative for assistance, if necessary.");
msgdef_ddts_component("");


msgdef(BADPARAM, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
       "Function %s:  value %#x passed in parameter %s");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADRESID, SYS, LOG_ERR, 0,
	     "Clock hardware returned bad residual %d", 10*ONEMIN);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(BADPRINT, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
	     "Process has no associated tty (in %s).", 10*ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(INTPRINT, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
	     "Illegal printing attempt from interrupt level.", 10*ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


/*
 * Messages for generic tree code.
 */
msgdef(TABLEERR, SYS, LOG_NOTICE, 0,
       "%s table %s damaged: %s");
#define SYS_ERR_TABLEERR &msgsym(TABLEERR, SYS)
msgdef_explanation(
	"An internal table entry became corrupted.");
msgdef_recommended_action(
	"Copy the message exactly as it appears, and report it your technical
	support representative.");
msgdef_ddts_component("");


/*
 * Messages for registries and services.
 */
msgdef_limit(SUPNOT, SYS, LOG_CRIT, MSG_TRACEBACK|MSG_PROCESS,
	"Can't create Registry %s", 30 * ONESEC);
msgdef_explanation(
	"An internal software consistency check failed.");
msgdef_recommended_action(
	"Copy the message exactly as it appears and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef_limit(SUPTWICE, SYS, LOG_CRIT, MSG_TRACEBACK|MSG_PROCESS,
	"Attempt to create Registry %s again", 30 * ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(SUPNONE, SYS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
       "Registry %d doesn't exist");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef_limit(REGNONE, SYS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
	"Registry %d service %d doesn't exist - %s", 30 * ONESEC);
msgdef_explanation(
	"An internal software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(REGDFLT, SYS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
	"Attempt to add default twice to registry %d service %d", 30 * ONESEC);
msgdef_explanation(
	"An internal software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(REGTYPE, SYS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
	"Registry %d service %d is wrong type, should be %d", 30 * ONESEC);
msgdef_explanation(
	"An internal software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(REGARG, SYS, LOG_ERR, MSG_TRACEBACK|MSG_PROCESS,
	"Registry %d service %d has too many arguments", 30 * ONESEC);
msgdef_explanation(
	"An internal software consistency check failed.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(BADLIST, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Regular expression access check with bad list %d");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(SNMP0IDX, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
       "Attempt to %s snmpidb with if_index of 0.");
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the message exactly as it appears and report it your technical
	support representative.");
msgdef_ddts_component("");


msgdef(SNMP_WRITENET, SYS, LOG_WARNING, 0,
       "SNMP WriteNet request.  Writing current configuration to %i.");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(SNMP_HOSTCONFIGSET, SYS, LOG_WARNING, 0,
       "SNMP hostConfigSet request.  Loading configuration from %i.");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(SNMP_NETCONFIGSET, SYS, LOG_WARNING, 0,
       "SNMP netConfigSet request.  Loading configuration from %i.");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(INVALID_IMAGE, SYS, LOG_WARNING, 0,
	     "Running invalid image for platform!");
msgdef_explanation(
	"The software image being run is for a different hardware architecture.
	This problem can occur on a Cisco 1003, Cisco 1004, or Cisco 1005. The
	Cisco 1003 and Cisco 1004 have a different image from the Cisco
	1005.");
msgdef_recommended_action(
	"Install the correct software image.");
msgdef_ddts_component("");


/*
 * Chunk Manager error messages
 */
msgdef_limit(CHUNKBOUNDS, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
	     "Attempted to exceed freelist chunk bounds,"
	     " chunk %x, data %x", 10 * ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CHUNKFREE, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
	     "Attempted to free nonchunk memory,"
	     " chunk %x, data %x", 10 * ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CHUNKSIBLINGS, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
	     "Attempted to destroy chunk with siblings,"
	     " chunk %x", 10 * ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CHUNKPARTIAL, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
	     "Attempted to destroy partially full chunk,"
	     " chunk %x", 10 * ONESEC);
msgdef_explanation(
	"A software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef_limit(CHUNKLOCK, SYS, LOG_CRIT, MSG_TRACEBACK | MSG_PROCESS,
	     "Attempted to lock unlockable chunk data,"
	     " chunk %x data %x", 10 * ONESEC);
msgdef_explanation(
	"");
msgdef_recommended_action(
	"");
msgdef_ddts_component("");


/*
 * Passive timer error messages
 */
msgdef_limit(TIMERNEG, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
	     "Cannot start timer (%#x) with negative offset (%d).", 10 * ONESEC);
msgdef_explanation(
	"An internal software error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


/*
 * Managed timer error messages
 */
msgdef_limit(MGDTIMER, SYS, LOG_ERR, MSG_TRACEBACK | MSG_PROCESS,
	     "%s%s, timer = %x", 10 * ONESEC);
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");


msgdef(DMPMEM, SYS, LOG_ERR, 0,
       "%08x: %08x %08x %08x %08x %08x %08x");
msgdef_explanation(
	"A software or hardware error occurred.");
msgdef_recommended_action(
	"Copy the error message exactly as it appears, and report it to your
	technical support representative.");
msgdef_ddts_component("");

msgdef(BOOT_MESSAGES, SYS, LOG_INFO, 0,
       "Messages above this line are from the boot loader.\n");
msgdef_explanation(
	"At least one informational or error message was printed by the boot loader.");
msgdef_recommended_action(
      "If the system image was loaded, no action is recommended.  If the
      system image did not load as configured, send a log file and
      configuration information to your technical support representative.");
msgdef_ddts_component("");


#endif /* __MSG_SYSTEM_C__ */
