/* $Id: mgd_timers.c,v 3.5.12.4 1996/08/28 13:03:29 thille Exp $
 * $Source: /release/112/cvs/Xsys/os/mgd_timers.c,v $
 *------------------------------------------------------------------
 * Managed timer support
 *
 * December 1994, Dave Katz
 *
 * Copyright (c) 1994-1996 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: mgd_timers.c,v $
 * Revision 3.5.12.4  1996/08/28  13:03:29  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.5.12.3  1996/05/26  05:25:43  dkatz
 * CSCdi58637:  Managed timer manipulation is inefficient
 * Branch: California_branch
 * Search from the end of each timer list, rather than the beginning.
 *
 * Revision 3.5.12.2  1996/05/07  02:59:11  dkatz
 * CSCdi56755:  Managed timers can be looped by poorly timed interrupts
 * Branch: California_branch
 * Make sure that the mt_head field is picked up after interrupts are
 * excluded, since this field can be manipulated by the scheduler.
 *
 * Revision 3.5.12.1  1996/03/18  21:30:25  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.4.12.4  1996/03/16  07:20:54  gstovall
 * Branch: DeadKingOnAThrone_branch
 * Make the king aware of V111_1_3.
 *
 * Revision 3.4.12.3  1996/03/09  05:09:58  hampton
 * Slim down the kernel of the router.
 * Branch: DeadKingOnAThrone_branch
 *
 * Revision 3.4.12.2  1996/03/07  10:19:03  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.4.12.1  1996/02/20  16:40:37  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.5  1996/02/19  21:32:52  dkatz
 * CSCdi48130:  ISIS timer debugging info needed
 * Add the ability to display managed timer chains.
 *
 * Revision 3.4  1995/12/22  21:58:41  hampton
 * Pass pointers to timestamps instead of passing the complete timestamp
 * structure.  [CSCdi45966]
 *
 * Revision 3.3  1995/11/17  18:49:33  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:18:14  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  12:46:57  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/10/23  03:57:02  dkatz
 * CSCdi42575:  Managed timers could be more robust
 * Try to avoid interpreting garbage as indicating that the managed
 * timer has been initialized.
 *
 * Revision 2.4  1995/10/01  05:43:23  dkatz
 * CSCdi41343:  Managed timers could be more orderly
 * Make siblings FIFO when they have the same expiration time.
 *
 * Revision 2.3  1995/06/18  05:31:42  hampton
 * Close a small timing window in the scheduler's calls to the
 * mgd_timer_link and mgd_timer_delink routines.  [CSCdi35906]
 *
 * Revision 2.2  1995/06/17 00:52:15  dkatz
 * CSCdi36017:  Managed timer linking and delinking fully interrupt-safe
 * Move validation of head pointer within interrupt-protected zone
 *
 * Revision 2.1  1995/06/07  21:56:56  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

/*
 * This routine manages hierarchical trees of timers.  The leaves of
 * the tree are individual timers that are started, stopped, and
 * updated directly.  The nodes in the tree are timers that inherit
 * the earliest expiration of each of its leaves.
 *
 * This is done recursively, allowing a single timer to represent the
 * earliest expiration of a very large number of individual timers.
 * This is most useful for boiling a whole bunch of individual timers
 * down to a single timer to block a task on.
 *
 * The tree is established at initialization time by identifying the
 * parent timer of each leaf and node.  Once that is done, only the
 * leaf timers are directly manipulated;  the parent nodes inherit
 * the state of the child nodes.
 *
 * Any node may be examined for its running and expiration state;  this
 * is typically only interesting for the root of the tree, except
 * perhaps for diagnostic purposes.
 *
 * Timers may be manipulated (but not initialized) from the interrupt
 * context, so long as they are marked as such when they are initialized.
 * Since there are potentially lots of loosely coupled timers, some timers
 * that are never touched by interrupt routines may be linked into trees
 * containing timers that are.  We handle this as follows:
 *
 * Each timer has an "interrupt exclusion" attribute with three values:
 * NONE (zero), DIRECT, or INHERITED.  For efficiency, DIRECT and INHERITED
 * are bitfields and may be set simultaneously.  If this attribute is 
 * nonzero, a timer cannot be manipulated unless interrupts are first excluded.
 *
 * When a leaf timer is initialized, the application notes whether or
 * not that timer will be manipulated by interrupt routines.  If true,
 * the timer and all of its parents to the root of the tree are marked
 * as DIRECT.  Furthermore, all siblings of these timers are marked
 * as INHERITED.
 *
 * When a timer is started, if its parent is marked as DIRECT, the timer
 * is marked as INHERITED.
 *
 * This results in a situation where subtrees of the overall tree that
 * never are touched at interrupt time may be left unmarked (and manipulated
 * with interrupts enabled).  If a timer in such a subtree is manipulated
 * such that the change ripples upward into a part of the tree that is
 * marked for exclusion, the timer routines will disable interrupts before
 * manipulating that part of the tree.
 * 
 * See Doc/Timers for usage details.
 */
#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "mgd_timers.h"
#include "logger.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

/*
 * Define MGD_TIMER_DEBUG to enable all debugging checks.
 */

/*#define MGD_TIMER_DEBUG*/


/*
 * mgd_timer_prev_timer
 *
 * Returns a pointer to the previous timer on this chain, or NULL if looking
 * at the first element on the chain.
 */
static inline mgd_timer *mgd_timer_prev_timer (mgd_timer *timer)
{

    /* We're at the head of the chain? */

    if (timer->mt_head && (timer->mt_head->mt_union.mt_down == timer))
	return(NULL);

    /* Nope, we just point to our previous guy. */

    return(timer->mt_prev);
}

/* Make all the debugging code appear or disappear... */

#ifdef MGD_TIMER_DEBUG
/*
 * local storage
 */
static boolean mgd_timer_debug;

#define mgd_timer_buginf(parms...) \
  if (mgd_timer_debug) \
    (*kernel_buginf)( ## parms)

#else
#define mgd_timer_buginf(parms...)
#endif

#ifdef MGD_TIMER_DEBUG

#define DEPTH_LIMIT 10000		/* Way more than necessary */

/*
 * mgd_timer_validate_one_timer
 *
 * Validate a single timer.
 */
static void mgd_timer_validate_one_timer (mgd_timer *timer)
{
    if (timer->mt_initialized != MT_INITIALIZED_MAGIC)
	crashdump(20);			/* Not initialized */
    if (!timer->mt_head) {		/* At the very top */
	if (timer->mt_prev || timer->mt_next) {
	    crashdump(21);		/* Should have no linkage */
	}
    }
    if (TIMER_RUNNING(timer->mt_exptime)) {
	if (!timer->mt_prev && timer->mt_head)
	    crashdump(22);		/* Running but not linked */
    } else {
	if (timer->mt_prev || timer->mt_next)
	    crashdump(23);		/* Linked but not running */
    }
}

/*
 * mgd_timer_validate_one_level
 *
 * Validate this timer and each of its siblings.
 */
static void mgd_timer_validate_one_level (mgd_timer *timer)
{
    mgd_timer *current, *previous;
    ulong depth_limit;
    
    /* Start by going left from the timer in question until we hit the end. */

    current = timer;
    previous = NULL;
    depth_limit = DEPTH_LIMIT;

    /*
     * Remember, "previous" is really the subsequent timer; we're going
     * backwards.
     */
    while (current && depth_limit) {
	if (previous && TIMER_LATER(current->mt_exptime, previous->mt_exptime))
	    crashdump(10);		/* Bad expiration time */
	if (previous && current->mt_next != previous)
	    crashdump(11);		/* Bad linkage */
	mgd_timer_validate_one_timer(current);
	previous = current;
	current = mgd_timer_prev_timer(current);
	depth_limit--;
    }
    if (depth_limit == 0)
	crashdump(12);			/* Things went circular! */

    /* OK, now go forward. */

    current = timer->mt_next;
    previous = timer;
    depth_limit = DEPTH_LIMIT;
    while (current && depth_limit) {
	if (TIMER_LATER(previous->mt_exptime, current->mt_exptime))
	    crashdump(13);		/* Bad expiration time */
	if (current->mt_prev != previous)
	    crashdump(14);		/* Bad linkage */
	mgd_timer_validate_one_timer(current);
	previous = current;
	current = current->mt_next;
	depth_limit--;
    }
}

/*
 * mgd_timer_validate
 *
 * Validate the timer tree.  Given a timer, it goes to the top of the
 * tree and works its way back down, validating all active timers on
 * the tree.
 */
static void mgd_timer_validate (mgd_timer *timer)
{
    ulong depth_limit;
    mgd_timer *current;

    /* First work our way upward toward the root. */

    current = timer;
    depth_limit = DEPTH_LIMIT;
    do {
	mgd_timer_validate_one_level(current);
	current = current->mt_head;
	depth_limit--;
    } while (current && depth_limit);
    if (depth_limit == 0)
	crashdump(0);			/* Things went circular! */

    /* Now work our way downward toward the leaves. */

    current = timer;
    depth_limit = DEPTH_LIMIT;
    while (!current->mt_leaf && depth_limit) {
	current = current->mt_union.mt_down;
	if (!current)
	    break;
	mgd_timer_validate_one_level(current);
	depth_limit--;
    }
    if (depth_limit == 0)
	crashdump(1);			/* Things went circular! */
}
#else
static inline void mgd_timer_validate (mgd_timer *timer)
{}
#endif /* MGD_TIMER_DEBUG */

/*
 * mgd_timer_complain_running
 *
 * File a complaint about a running timer
 */
static void mgd_timer_complain_running (mgd_timer *timer, char *complaint)
{
    (*kernel_errmsg)(&msgsym(MGDTIMER, SYS), "Running timer, ", complaint,
		     timer);
}

/*
 * mgd_timer_complain_uninit
 *
 * File a complaint about an uninitialized timer
 */
static void mgd_timer_complain_uninit (mgd_timer *timer, char *complaint)
{
    (*kernel_errmsg)(&msgsym(MGDTIMER, SYS), "Uninitialized timer, ",
		     complaint, timer);
}

/*
 * mgd_timer_complain_notleaf
 *
 * File a complaint about a timer that isn't a leaf
 */
static void mgd_timer_complain_notleaf (mgd_timer *timer, char *complaint)
{
    (*kernel_errmsg)(&msgsym(MGDTIMER, SYS), "Timer not a leaf, ",
		     complaint, timer);
}

/*
 * mgd_timer_complain_parent_is_leaf
 *
 * File a complaint about a parent timer that is a leaf
 */
static void mgd_timer_complain_parent_is_leaf (mgd_timer *timer,
					       char *complaint)
{
    (*kernel_errmsg)(&msgsym(MGDTIMER, SYS), "Parent is a leaf, ",
		     complaint, timer);
}

/*
 * mgd_timer_init_common
 *
 * common timer initialization for parents and leaves
 */
static void mgd_timer_init_common (mgd_timer *timer, mgd_timer *master)
{

    /* Complain if the master isn't initialized yet. */

    if (master && !mgd_timer_initialized(master)) {
	mgd_timer_complain_uninit(master, "init with uninitialized master");
	return;
    }

    /* Complain if the master isn't a parent timer. */

    if (master && master->mt_leaf) {
	mgd_timer_complain_parent_is_leaf(timer, "timer init");
	return;
    }

    /* Complain if we're already initialized and running. */

    if (mgd_timer_initialized(timer) && mgd_timer_running(timer)) {
	mgd_timer_complain_running(timer, "init");
	return;
    }

    /* Set up all the appropriate stuff. */

    memset(timer, 0, sizeof(*timer));
    timer->mt_initialized = MT_INITIALIZED_MAGIC;
    timer->mt_head = master;
}

/*
 * mgd_timer_mark_intrpt_exclusion
 *
 * Mark the parents and all siblings of this timer as being
 * interrupt excluded.
 */
static void mgd_timer_mark_intrpt_exclusion (mgd_timer *timer)
{
    ushort status;
    mgd_timer *cur_parent, *master, *cur_child;

    status = raise_interrupt_level(ALL_DISABLE);
    master = timer->mt_head;
    cur_parent = master;
    while (cur_parent) {

	/* Bail from the loop if the parent is already marked. */

	if (cur_parent->mt_intrpt_env & MT_INTRPT_EXCLUDE_DIRECT)
	    break;

	/* Mark the parent. */
	
	cur_parent->mt_intrpt_env |= MT_INTRPT_EXCLUDE_DIRECT;
	
	/* Mark the siblings. */
	
	cur_child = cur_parent->mt_union.mt_down;
	while (cur_child) {
	    cur_child->mt_intrpt_env |= MT_INTRPT_EXCLUDE_INHERITED;
	    cur_child = cur_child->mt_next;
	}
	cur_parent = cur_parent->mt_head;
    }
    reset_interrupt_level(status);
}

/*
 * mgd_timer_init_parent
 *
 * Initialize a parent timer
 */
void mgd_timer_init_parent (mgd_timer *timer, mgd_timer *master)
{
    if (!timer)
	return;

    /* Do common initialization. */

    mgd_timer_init_common(timer, master);

    mgd_timer_buginf("\nInit parent %x, master %x", timer, timer->mt_head);
}

/*
 * mgd_timer_init_leaf
 *
 * Initialize a leaf timer
 */
void mgd_timer_init_leaf (mgd_timer *timer, mgd_timer *master, ushort type,
			  void *context, boolean interrupt_environ)
{
    if (!timer)
	return;

    /* Do common initialization. */

    mgd_timer_init_common(timer, master);

    /* Set up all the appropriate stuff. */

    timer->mt_leaf = TRUE;
    timer->mt_union.mt_context = context;
    timer->mt_type = type;

    /*
     * If the "interrupt environment" flag is set, recurse up the tree,
     * setting the flag in all the parents, as well as the INHERITED
     * bit in the children of all those parents (siblings of the marked
     * timers).
     */

    if (interrupt_environ) {
	timer->mt_intrpt_env = MT_INTRPT_EXCLUDE_DIRECT;
	mgd_timer_mark_intrpt_exclusion(timer);
    }

    mgd_timer_buginf("\nInit leaf %x, master %x, type %d, context %x",
		     timer, master, timer->mt_type,
		     timer->mt_union.mt_context);
}

/*
 * mgd_timer_set_exptime_internal
 *
 * Set the expiration time of a timer.  This involves potentially
 * delinking it, and then linking it into the proper place in the chain.
 * If we were (or become) the first in the chain, we must propagate
 * the changes up through the master.
 *
 * This is the internal version, which allows the setting of non-leaf
 * times.  This is done by mgd_timer_stop.
 */
static void mgd_timer_set_exptime_internal (mgd_timer *timer,
					    sys_timestamp *exptime_ptr)
{
    sys_timestamp exptime;		/* Our working time value */
    sys_timestamp old_exptime;		/* Our old time */
    mgd_timer *master;			/* Our master */
    mgd_timer *current;			/* Current comparison */
    mgd_timer *successor;		/* Next guy */
    mgd_timer *predecessor;		/* Previous guy */
    boolean update_master;		/* True if we must recurse */
    boolean was_first;			/* True if we were first in the list */
    boolean was_running;		/* True if we were running */
    boolean interrupts_disabled;
    ushort status = 0;

    mgd_timer_validate(timer);

    exptime = *exptime_ptr;
    if (!TIMER_RUNNING(exptime)) {	/* Bad bad bad */
	(*kernel_errmsg)(&msgsym(MGDTIMER, SYS),
			 "Setting zero expiration time", "", timer);
	return;
    }

    if (!mgd_timer_initialized(timer))	{	/* Equally bad bad bad */
	mgd_timer_complain_uninit(timer, "set_exptime_internal");
	return;
    }

    interrupts_disabled = FALSE;
    while (TRUE) {			/* Poor man's recursion */
	master = timer->mt_head;

	/*
	 * Do interrupt exclusion magic.  If the parent has the DIRECT
	 * bit set, set the INHERITED bit locally.  If the local or
	 * our master has exclusion set, disable interrupts.
	 */
	if (master && (master->mt_intrpt_env & MT_INTRPT_EXCLUDE_DIRECT))
	    timer->mt_intrpt_env |= MT_INTRPT_EXCLUDE_INHERITED;

	if ((timer->mt_intrpt_env || (master && master->mt_intrpt_env)) &&
	    !interrupts_disabled) {
	    status = raise_interrupt_level(ALL_DISABLE);
	    interrupts_disabled = TRUE;
	    master = timer->mt_head;	/* Reload it with exclusion. */
	}

	/* Complain if timer and linkage are inconsistent. */

	mgd_timer_buginf("\nSet %x, time %#Ta", timer, exptime);
	was_running = TIMER_RUNNING(timer->mt_exptime);
	if (!was_running && timer->mt_prev) { /* Uh oh */
	    (*kernel_errmsg)(&msgsym(MGDTIMER, SYS),
			     "NZ prev pointer but not running", "", timer);
	    break;
	}

	/* Note the old expiry time.  Update our time. */
	
	old_exptime = timer->mt_exptime;
	timer->mt_exptime = exptime;
	mgd_timer_buginf(", old %#Ta", old_exptime);

	/*
	 * If there is no master, it means that we're the head of all
	 * timer chains, so get out now.
	 */
	mgd_timer_buginf(", master %x", master);
	if (!master)
	    break;

	/* Complain if the master is a leaf. */

	if (master->mt_leaf) {
	    mgd_timer_complain_parent_is_leaf(timer, "set_exptime_internal");
	    break;
	}
	was_first = (master->mt_union.mt_down == timer);
	if (was_first) {
	    mgd_timer_buginf(", first");
	} else {
	    mgd_timer_buginf(", not first");
	}
	update_master = was_first;	/* Must update if we're first */

	/*
	 * If we were running before, we're not the head of the list, and
	 * our position won't change, we're done.
	 */
	if (was_running && !was_first) {

	    /* Previous guy is no later than us, and */
	    if ((!TIMER_LATER((timer->mt_prev)->mt_exptime, exptime)) &&

		/* We're last, or we're earlier than the next guy */
		((!timer->mt_next) ||
		 TIMER_EARLIER(exptime, timer->mt_next->mt_exptime))) {
		mgd_timer_buginf(", didn't move");
		break;			/* That was easy. */
	    }
	}

	/* If we weren't running before, start our search at the tail. */

	if (!was_running) {
	    if (master->mt_union.mt_down) {
		current = master->mt_union.mt_down->mt_prev; /* The tail... */
	    } else {
		current = NULL;
	    }
	    successor = NULL;

	} else {

	    /*
	     * We were running.  Delink ourselves.  Note the subtle
	     * difference between "predecessor" and timer->mt_prev.  If
	     * we're the head of the list, the former will be NULL but the
	     * latter will point to the list tail.
	     */
	    successor = timer->mt_next;
	    predecessor = mgd_timer_prev_timer(timer);
	    if (predecessor) {
		predecessor->mt_next = successor;
	    } else {			/* Must have been first */
		master->mt_union.mt_down = successor; /* Update master */
	    }
	    if (successor) {
		successor->mt_prev = timer->mt_prev; /* Prev or tail pointer */
	    } else {			/* We were tail */
		if (master->mt_union.mt_down) { /* Update tail. */
		    master->mt_union.mt_down->mt_prev = predecessor;
		}
	    }
	    current = predecessor;

	    /*
	     * If our new time is later than our old, start the search
	     * at the tail;  otherwise, start where we delinked.
	     */
	    if (TIMER_LATER(exptime, old_exptime)) {
		if (master->mt_union.mt_down) {
		    current = master->mt_union.mt_down->mt_prev; /* Tail */
		} else {
		    current = NULL;
		}
		successor = NULL;
		mgd_timer_buginf(", new later");
	    }
	}

	/*
	 * Now move backward until we find an entry that is not later
	 * than us (or we hit the head of the list) and link in at that point.
	 */
	while (current) {
	    mgd_timer_buginf(", cur %x (%#Ta)", current, current->mt_exptime);
	    if (!TIMER_LATER(current->mt_exptime, exptime))
		break;
	    successor = current;
	    current = mgd_timer_prev_timer(current);
	}

	/* Found our spot.  We link between current and successor. */

	timer->mt_next = successor;
	timer->mt_prev = current;
	if (successor) {

	    /* If we're the new head, pick up the tail from our successor. */

	    if (!current) {
		timer->mt_prev = successor->mt_prev;
	    }
	    successor->mt_prev = timer;

	} else {

	    /* We're the tail.  Update the tail pointer. */

	    if (master->mt_union.mt_down)
		master->mt_union.mt_down->mt_prev = timer;
	}

	if (current) {
	    current->mt_next = timer;
	} else {			/* We're the head */
	    mgd_timer_buginf(", at head");
	    if (!successor) {		/* Only timer in the chain... */
		timer->mt_prev = timer;	/* We're the tail! */
	    }
	    master->mt_union.mt_down = timer;
	    update_master = TRUE;	/* Gotta recurse. */
	}

	/* If we must update the master, recurse.  Otherwise, we're done. */

	if (!update_master)
	    break;
	timer = master;
	exptime = master->mt_union.mt_down->mt_exptime;
	mgd_timer_buginf(", recurse");
    }					/* end while(TRUE) */

    mgd_timer_validate(timer);

    if (interrupts_disabled)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_link
 *
 * Link a subtree into another tree.  The "head" pointer of the current
 * root of the subtree must be NULL.  The timer may be running.  The
 * link point must be a parent timer.  The timer and the parent must be
 * initialized.
 *
 * Interrupt exclusion information is propagated appropriately.
 */
void mgd_timer_link (mgd_timer *timer, mgd_timer *master, mgd_timer **shadow,
		     boolean interrupt_environ)
{
    boolean interrupts_disabled;
    sys_timestamp exptime;
    ushort status = 0;

    /* Sanity. */

    *shadow = NULL;
    if (!timer || !master)
	return;
    mgd_timer_validate(timer);

    /* Timer must be initialized. */

    if (!mgd_timer_initialized(timer)) {
	mgd_timer_complain_uninit(timer, "root of linked subtree");
	return;
    }

    /* Master must be initialized. */

    if (!mgd_timer_initialized(master)) {
	mgd_timer_complain_uninit(master, "link with uninitialized master");
	return;
    }

    /* Complain if the master isn't a parent timer. */

    if (master->mt_leaf) {
	mgd_timer_complain_parent_is_leaf(timer, "timer link");
	return;
    }

    mgd_timer_validate(timer);

    interrupts_disabled = FALSE;
    if (timer->mt_intrpt_env) {
	status = raise_interrupt_level(ALL_DISABLE);
	interrupts_disabled = TRUE;
    }

    /* There better not be a current parent. */

    if (timer->mt_head) {
	(*kernel_errmsg)(&msgsym(MGDTIMER, SYS),
			 "Timer has parent, timer link", "", timer);
	goto exit;
    }

    /* Link in the parent. */

    timer->mt_head = master;
    *shadow = timer;

    /* Propagate interrupt exclusion information. */

    if (interrupt_environ)
	timer->mt_intrpt_env |= MT_INTRPT_EXCLUDE_DIRECT;
    if (timer->mt_intrpt_env & MT_INTRPT_EXCLUDE_DIRECT)
	mgd_timer_mark_intrpt_exclusion(timer);

    /* If the timer isn't running, we're done. */

    if (!mgd_timer_running(timer))
	goto exit;

    /* Link it in.  Cheap hack! */

    exptime = timer->mt_exptime;
    TIMER_STOP(timer->mt_exptime);	/* It's not linked in yet! */
    mgd_timer_set_exptime_internal(timer, &exptime);

  exit:
    mgd_timer_validate(timer);
    if (interrupts_disabled)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_set_exptime
 *
 * Set the expiration time of a timer.  This involves potentially
 * delinking it, and then linking it into the proper place in the chain.
 * If we were (or become) the first in the chain, we must propagate
 * the changes up through the master.
 *
 * This version sanity checks to enforce leaf-level calls only.
 */
void mgd_timer_set_exptime (mgd_timer *timer, sys_timestamp *exptime)
{
    if (!timer)
	return;

    if (!timer->mt_leaf) {		/* This is bad too! */
	mgd_timer_complain_notleaf(timer, "set_exptime");
	return;
    }

    mgd_timer_set_exptime_internal(timer, exptime);
}

/*
 * mgd_timer_set_soonest
 *
 * Set a timer to the soonest of its current time and the specified
 * timestamp.  If the timer isn't currently running, the timer is set
 * to the specified timestamp.
 */
void mgd_timer_set_soonest (mgd_timer *timer, sys_timestamp time)
{
    sys_timestamp exptime;
    boolean interrupt_environment;
    ushort status = 0;

    if (!timer)
	return;

    /* Disable interrupts if necessary. */

    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);

    /*
     * If not currently running, use the given time.  Otherwise, use 
     * the soonest.
     */
    if (!TIMER_RUNNING(timer->mt_exptime)) {
	exptime = time;
    } else {
	exptime = TIMER_SOONEST(time, timer->mt_exptime);
    }
    mgd_timer_set_exptime(timer, &exptime);
    if (interrupt_environment)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_start
 *
 * Start a timer with the specified delay.
 */
void mgd_timer_start (mgd_timer *timer, ulong delay)
{
    sys_timestamp exptime;

    TIMER_START(exptime, delay);
    mgd_timer_set_exptime(timer, &exptime);
}

/*
 * mgd_timer_start_jittered
 *
 * Start a timer with the specified delay and jitter.
 */
void mgd_timer_start_jittered (mgd_timer *timer, ulong delay, ulong jitter)
{
    sys_timestamp exptime;

    TIMER_START_JITTERED(exptime, delay, jitter);
    mgd_timer_set_exptime(timer, &exptime);
}

/*
 * mgd_timer_update
 *
 * Bump the timer's current expiration time with the new delay.
 *
 * Does nothing if the timer isn't currently running.
 */
void mgd_timer_update (mgd_timer *timer, ulong delay)
{
    sys_timestamp exptime;
    ushort status = 0;
    boolean interrupt_environment;

    if (!timer)
	return;

    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);
    if (mgd_timer_running(timer)) {
	exptime = mgd_timer_exp_time(timer);
	TIMER_UPDATE(exptime, delay);
	mgd_timer_set_exptime(timer, &exptime);
    }
    if (interrupt_environment)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_update_jittered
 *
 * Bump the timer's current expiration time with the new delay and jitter.
 *
 * Does nothing if the timer isn't currently running.
 */
void mgd_timer_update_jittered (mgd_timer *timer, ulong delay, ulong jitter)
{
    sys_timestamp exptime;
    ushort status = 0;
    boolean interrupt_environment;

    if (!timer)
	return;

    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);
    if (mgd_timer_running(timer)) {
	exptime = mgd_timer_exp_time(timer);
	TIMER_UPDATE_JITTERED(exptime, delay, jitter);
	mgd_timer_set_exptime(timer, &exptime);
    }
    if (interrupt_environment)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_next_running
 *
 * Returns a pointer to the next sibling of this timer, or NULL if there
 * are none.
 */
mgd_timer *mgd_timer_next_running (mgd_timer *timer)
{
    ushort status = 0;
    boolean interrupt_environment;

    if (!timer)
	return(NULL);

    mgd_timer_validate(timer);

    /*
     * Disable interrupts if necessary.  Note that if the timer is not
     * marked for exclusion, we know that none of the siblings are.
     */
    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);

    timer = timer->mt_next;		/* Get the sibling */

    if (interrupt_environment)
	reset_interrupt_level(status);

    mgd_timer_buginf("\nnext running %x, time %#Ta", timer, timer->mt_exptime);
    return(timer);
}

/*
 * mgd_timer_walk_down_tree
 *
 * Returns a pointer to the first leaf of this timer tree that is
 * running, or NULL if there are none.
 */
typedef enum {FIND_RUNNING, FIND_FENCED} mgd_timer_walk_type;

static mgd_timer *mgd_timer_walk_down_tree (mgd_timer *timer,
					    mgd_timer_walk_type walk_type)
{
    ushort status = 0;
    boolean interrupt_environment;

    if (!timer)
	return(NULL);

    mgd_timer_validate(timer);

    /*
     * Disable interrupts if necessary.  Note that if the parent is not
     * marked for exclusion, we know that none of the children are.
     */
    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);

    /* If the master is not running, there are no subordinates. */

    if (TIMER_RUNNING(timer->mt_exptime)) {

	/*
	 * Recurse down to the bottom of the tree.  By definition this
	 * is the first running entry.
	 */
	while ((!timer->mt_leaf) && timer->mt_union.mt_down) {
	    switch (walk_type) {
	      case FIND_RUNNING:
		break;
	      case FIND_FENCED:
		if (timer->mt_fence)
		    goto found;
		break;
	    }
	    timer = timer->mt_union.mt_down;
	}
	if (timer && !timer->mt_leaf)
	    timer = NULL;
    } else {
	timer = NULL;
    }
  found:
    switch (walk_type) {
      case FIND_RUNNING:
	break;
      case FIND_FENCED:
	if (timer && !timer->mt_fence)
	    timer = NULL;
	break;
    }
    if (interrupt_environment)
	reset_interrupt_level(status);

    mgd_timer_buginf("\nwalk down %x, time %#Ta", timer, timer->mt_exptime);
    return(timer);
}

/*
 * mgd_timer_first_child
 *
 * Returns a pointer to this timer's first child, or NULL
 */
mgd_timer *mgd_timer_first_child (mgd_timer *timer)
{
    if (!timer)
	return(NULL);
    mgd_timer_validate(timer);
    if (timer->mt_leaf)
	return(NULL);
    return(timer->mt_union.mt_down);
}

/*
 * mgd_timer_set_fenced
 *
 * Set the "fenced" flag on the timer to the provided value.
 */
void mgd_timer_set_fenced (mgd_timer *timer, boolean state)
{
    if (!timer)
	return;
    mgd_timer_validate(timer);
    timer->mt_fence = (state != FALSE);	/* It's a bitfield */
}

/*
 * mgd_timer_first_fenced
 *
 * Walks down the left-hand side of the timer tree, stopping at an entry
 * marked as "fenced".  If an entry is not found, return NULL.
 *
 * Note that all timers at a fence level must be marked as fenced in
 * order for this to be reliable, since it only walks the lefthand
 * side.
 */
mgd_timer *mgd_timer_first_fenced (mgd_timer *timer)
{
    return (mgd_timer_walk_down_tree(timer, FIND_FENCED));
}

/*
 * mgd_timer_first_running
 *
 * Returns a pointer to the first leaf of this timer tree that is
 * running, or NULL if there are none.
 */
mgd_timer *mgd_timer_first_running (mgd_timer *timer)
{
    return (mgd_timer_walk_down_tree(timer, FIND_RUNNING));
}

/*
 * mgd_timer_first_expired
 *
 * Returns a pointer to the first leaf of this timer tree that is
 * expired, or NULL if there are none.
 */
mgd_timer *mgd_timer_first_expired (mgd_timer *timer)
{
    if (!timer)
	return(NULL);

    mgd_timer_validate(timer);

    /* If the master is not expired, none of the subordinates are. */

    if (!TIMER_RUNNING_AND_AWAKE(timer->mt_exptime))
	return(NULL);
    return(mgd_timer_first_running(timer));
}

/*
 * mgd_timer_delink_internal
 *
 * Delink a timer subtree from the timer tree, stopping the timer.
 */
static void mgd_timer_delink_internal (mgd_timer *timer)
{
    boolean was_first;
    mgd_timer *master;
    boolean interrupts_disabled;
    ushort status = 0;			/* Stupid compiler. */
    
    interrupts_disabled = FALSE;

    mgd_timer_validate(timer);
    
    while (TRUE) {			/* Poor man's recursion */

	/*
	 * Do interrupt exclusion.  Note that we don't have to check
	 * the master, since if we were not excluded, it means that
	 * none of our siblings are, and it's safe to manipulate our
	 * master's down pointer.
	 */
	if (timer->mt_intrpt_env && !interrupts_disabled) { /* Uhoh */
	    status = raise_interrupt_level(ALL_DISABLE);
	    interrupts_disabled = TRUE;
	}

	/* Kill the timer. */
	
	TIMER_STOP(timer->mt_exptime);	/* Kill the timer */

	/*
	 * Bail if there's no "prev" or master pointer;  either we're a
	 * master, or we weren't running to begin with.
	 */
	master = timer->mt_head;
	mgd_timer_buginf("\nDelink timer %x", timer);
	mgd_timer_buginf(", head %x, prev %x, next %x", master, timer->mt_prev,
			 timer->mt_next);
	if (!timer->mt_prev || !master)
	    break;
	
	/* Complain if the master is a leaf. */

	if (master->mt_leaf) {
	    mgd_timer_complain_parent_is_leaf(timer, "timer stop");
	    break;
	}

	/* Note if we were first on the chain. */

	was_first = (master->mt_union.mt_down == timer);
	if (was_first) {
	    mgd_timer_buginf(", first");
	} else {
	    mgd_timer_buginf(", not first");
	}

	/* Delink ourselves. */

	if (was_first) {
	    master->mt_union.mt_down = timer->mt_next; /* Link down */
	} else {
	    timer->mt_prev->mt_next = timer->mt_next; /* Link forward */
	}
	if (timer->mt_next) {
	    timer->mt_next->mt_prev = timer->mt_prev; /* Link backward */
	} else {			/* We were tail */
	    if (master->mt_union.mt_down) { /* Update tail. */
		master->mt_union.mt_down->mt_prev = timer->mt_prev;
	    }
	}
	timer->mt_prev = NULL;
	timer->mt_next = NULL;

	/* If we weren't the first, we're done. */

	if (!was_first)
	    break;

	/*
	 * If we were the first, we must work recursively upward, since
	 * we are impacting our master timer as well.  If there are other
	 * timers in the chain, update the master's time to the new first
	 * guy in the chain;  otherwise, stop it.  Note that we
	 * will never actually recurse.
	 */
	if (master->mt_union.mt_down) {	   /* There's somebody else there */
	    mgd_timer_buginf(", down %x");
	    mgd_timer_set_exptime_internal(master,
				   &master->mt_union.mt_down->mt_exptime);
	    break;
	}

	timer = master;			/* Nobody there, "recurse." */
	mgd_timer_buginf(", recurse %x", timer);
    }

    mgd_timer_validate(timer);

    if (interrupts_disabled)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_delink
 *
 * Delink a timer subtree.  We do not stop any of the timers in the
 * subtree.  We may stop one or more parent timers if we were the
 * last child timer.
 *
 * The timer must have a non-null parent pointer.  We clear the parent
 * pointer after delinking.
 */
void mgd_timer_delink (mgd_timer **timer_ptr)
{
    mgd_timer *timer;
    sys_timestamp exptime;
    boolean interrupt_environment;
    ushort status = 0;		/* Avoid compiler complaints. */

    /* Sanity check. */

    if ((timer_ptr == NULL) || (*timer_ptr == NULL))
	return;
    timer = *timer_ptr;

    mgd_timer_validate(timer);

    /* Must be initialized. */

    if (!mgd_timer_initialized(timer))	{	/* Don't ever do this! */
	mgd_timer_complain_uninit(timer, "timer delink");
	return;
    }

    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);

    /* Must be linked to something. */

    if (!timer->mt_head) {
	(*kernel_errmsg)(&msgsym(MGDTIMER, SYS), "No parent, ", "delink",
			 timer);
	goto exit;
    }

    /*
     * If this timer is running, delink it from the parents.  We need
     * to save the expiration time across the delink call, since the
     * delinker zeroes it out (this is done for overall consistency and
     * is checked by the validator).
     */
    if (mgd_timer_running(timer)) {
	exptime = timer->mt_exptime;	/* Save it */
	mgd_timer_delink_internal(timer);
	timer->mt_exptime = exptime;	/* Put it back */
    }
    
    /* Null out the parent pointer. */
       
    timer->mt_head = NULL;
    
    /* Null out the pointer to the child timer. */
       
    *timer_ptr = NULL;

  exit:
    mgd_timer_validate(timer);
    if (interrupt_environment)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_stop
 *
 * Stop a timer.  This can be a leaf or a parent;  if it's a parent,
 * we stop each of its children, which will in turn stop the parent.
 */
void mgd_timer_stop (mgd_timer *timer)
{
    mgd_timer *child;
    boolean interrupt_environment;
    ushort status = 0;		/* Avoid compiler complaints. */

    /* Sanity check. */

    if (!timer)
	return;
    mgd_timer_validate(timer);

    if (!mgd_timer_initialized(timer))	{	/* Don't ever do this! */
	mgd_timer_complain_uninit(timer, "timer stop");
	return;
    }

    /* If this timer is already stopped, we're done. */

    if (!mgd_timer_running(timer))
	return;

    /*
     * If this is a leaf timer, stop it directly.
     */

    if (timer->mt_leaf) {
	mgd_timer_delink_internal(timer);
	return;
    }

    /*
     * Not a leaf timer.  Recurse down to the bottom to find the
     * next running leaf timer and stop that.  When we succeed in
     * turning off all of the leaves, our timer will be stopped,
     * at which time we can boogie out of here.
     *
     * Note that if we are not marked for interrupt exclusion, none of
     * the children will be either.
     */
    interrupt_environment = timer->mt_intrpt_env;
    if (interrupt_environment)
	status = raise_interrupt_level(ALL_DISABLE);
    while (mgd_timer_running(timer)) {
	child = timer->mt_union.mt_down;
	while (TRUE) {
	    if (!child) {		/* Can't happen! */
		(*kernel_errmsg)(&msgsym(MGDTIMER, SYS),
				 "Running parent with no child", "", timer);
		goto exit;
	    }
	    if (child->mt_leaf) {	/* We hit bottom */
		mgd_timer_delink_internal(child); /* Stop it */
		break;			/* Exit to main loop */
	    }
	    child = child->mt_union.mt_down; /* Continue downward. */
	}
    }
  exit:
    mgd_timer_validate(timer);
    if (interrupt_environment)
	reset_interrupt_level(status);
}

/*
 * mgd_timer_set_context
 *
 * Change the context of an existing timer
 */
void mgd_timer_set_context (mgd_timer *timer, void *context)
{
    mgd_timer_validate(timer);
    if (!mgd_timer_initialized(timer)) {
	mgd_timer_complain_uninit(timer, "set context");
    } else if (!timer->mt_leaf) {
	mgd_timer_complain_notleaf(timer, "set context");
    } else {
	timer->mt_union.mt_context = context;
    }
}

/*
 * mgd_timer_set_type
 *
 * Change the type of an existing timer.  The timer can be a parent or
 * a leaf.  The timer must have already been initialized.
 */
void mgd_timer_set_type (mgd_timer *timer, ushort type)
{
    mgd_timer_validate(timer);
    if (!mgd_timer_initialized(timer)) {
	mgd_timer_complain_uninit(timer, "set type");
    } else {
	timer->mt_type = type;
    }
}

/*
 * mgd_timer_change_to_leaf
 *
 * Change a parent timer into a leaf timer.  The timer must be 
 * initialized, and must not be running.  It is up to the caller
 * to ensure that any previous child timers have been disowned (via a
 * mgd_timer_delink).
 */
void mgd_timer_change_to_leaf (mgd_timer *timer)
{
    mgd_timer_validate(timer);

    if (!mgd_timer_initialized(timer)) {
	mgd_timer_complain_uninit(timer, "make leaf");
	return;
    }

    if (mgd_timer_running(timer)) {
	mgd_timer_complain_running(timer, "make leaf");
	return;
    }

    if (timer->mt_leaf) {
	return;				/* Nothing to do */
    }

    if (timer->mt_union.mt_down) { /* Can't happen! */
	(*kernel_errmsg)(&msgsym(MGDTIMER, SYS),
			 "NZ downlink, change leaf", "", timer);
	return;
    }

    timer->mt_leaf = TRUE;
}

/*
 * mgd_timer_change_to_parent
 *
 * Change a leaf timer into a parent timer.  The timer must be
 * initialized, and must not be running.
 */
void mgd_timer_change_to_parent (mgd_timer *timer)
{
    mgd_timer_validate(timer);
    if (!mgd_timer_initialized(timer)) {
	mgd_timer_complain_uninit(timer, "make parent");
	return;
    }
    if (mgd_timer_running(timer)) {
	mgd_timer_complain_running(timer, "make parent");
	return;
    }

    if (!timer->mt_leaf) {
	return;				/* Nothing to do */
    }

    timer->mt_leaf = FALSE;
    timer->mt_union.mt_down = NULL;
}


/*
 * Structure for holding managed timer info
 */
typedef struct mgd_timer_info_ {
    mgd_timer *timer;
    sys_timestamp exptime;
    ushort depth;
    ushort type;
} mgd_timer_info;

#define MAX_DISPLAY_MGD_TIMERS 10000	/* Can't do more than this */

/*
 * mgd_timer_copy_tree
 *
 * Copies the structure of a tree into a linear array
 *
 * Returns the array offset of the next element to go into the array.
 *
 * This routine recurses!
 */
static ulong mgd_timer_copy_tree (mgd_timer *timer, mgd_timer_info *info_array,
				  ulong current_offset, ulong depth)
{
    mgd_timer_info *cur_info;
    mgd_timer *cur_child;

    /* First add the current timer. */

    if (current_offset >= MAX_DISPLAY_MGD_TIMERS)
	return(current_offset);
    cur_info = &info_array[current_offset];
    cur_info->timer = timer;
    cur_info->exptime = mgd_timer_exp_time(timer);
    cur_info->depth = depth;
    cur_info->type = mgd_timer_type(timer);
    current_offset++;

    /* Now recurse for each of the children of this timer. */

    if (mgd_timer_initialized(timer)) {
	if (!timer->mt_leaf) {
	    cur_child = timer->mt_union.mt_down;
	    depth++;
	    while (cur_child) {
		if (current_offset >= MAX_DISPLAY_MGD_TIMERS)
		    return(current_offset);
		current_offset = mgd_timer_copy_tree(cur_child, info_array,
						     current_offset, depth);
		cur_child = cur_child->mt_next;
	    }
	}
    }
    return(current_offset);
}

/*
 * mgd_timer_display_tree
 *
 * Display the contents of a managed timer subtree
 *
 * This routine walks a timer tree, calling a supplied callback routine
 * to format any annotated text.
 *
 * The callback routine may either format text into the buffer supplied to
 * it by this routine, or it may return its own character pointer (useful
 * if the formatted text is a constant).
 *
 * ***NOTE***  The managed timer pointer passed to the callback routine
 * may not be valid by the time the callback routine is called, due to
 * the fact that things may have suspended in the mean time.  Any use
 * of the timer pointer itself must be done very carefully, as the
 * value may point to something entirely different.
 *
 * The entire tree is copied into an auxiliary structure so that the
 * tree itself stays consistent.
 */
void mgd_timer_display_tree(mgd_timer *timer,
			    char *(*callback_func)(mgd_timer *timer,
						   ushort type, char *buf))
{
    mgd_timer_info *info_array, *info_ptr;
    ulong entry_count;
    sys_timestamp curtime;
    ulong blank_ix;
    int hours, minutes, seconds, milliseconds;
    char buf[MGD_TIMER_DISPLAY_BUFMAX + 1];
    char *return_buf;
    boolean printing;

    /* First malloc the info array. */

    info_array = malloc(sizeof(*info_array) * MAX_DISPLAY_MGD_TIMERS);
    if (!info_array) {
	printf(nomemory);
	return;
    }

    /* Now suck the tree into the array. */

    GET_TIMESTAMP(curtime);
    entry_count = mgd_timer_copy_tree(timer, info_array, 0, 0);

    /* Now walk the array, displaying each entry. */

    info_ptr = info_array;
    while (entry_count--) {
	printf("\n");
	for (blank_ix = 0; blank_ix < info_ptr->depth; blank_ix++) {
	    printf("  ");
	}
	printf("|");
	milliseconds = CLOCK_DIFF_SIGNED(curtime, info_ptr->exptime);
	if (milliseconds < 0)
	    milliseconds = 0;
	seconds = (milliseconds / ONESEC) % 60;
	minutes = (milliseconds / ONEMIN) % 60;
	hours = milliseconds / ONEHOUR;
	milliseconds %= ONESEC;
	printing = FALSE;
	if (hours) {
	    printf("%-3d:", hours);
	    printing = TRUE;
	} else {
	    printf("    ");
	}
	if (minutes || printing) {
	    if (printing) {
		printf("%02d:", minutes);
	    } else {
		printf("%-2d:", minutes);
		printing = TRUE;
	    }
	} else {
	    printf("   ");
	}
	if (printing) {
	    printf("%02d.%03d  ", seconds, milliseconds);
	} else {
	    printf("%-2d.%03d  ", seconds, milliseconds);
	}
	buf[0] = '\0';
	return_buf = (*callback_func)(info_ptr->timer, info_ptr->type, buf);
	printf(return_buf);
	info_ptr++;
    }
    free(info_array);
}
