/* $Id: lecs_finder.h,v 3.1.68.1 1996/05/09 14:33:19 rbadri Exp $
 * $Source: /release/112/cvs/Xsys/lane/lecs_finder.h,v $
 *------------------------------------------------------------------
 *
 * the lecs finder header file
 *
 * Nov 22, 1995 C. M. Akyol (gee)
 *
 * Copyright (c) 1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: lecs_finder.h,v $
 * Revision 3.1.68.1  1996/05/09  14:33:19  rbadri
 * Branch: California_branch
 * LANE, UNI3.1 features
 *
 * Revision 3.1.94.1  1996/04/27  07:07:09  cakyol
 * non sync sync from V112_0_2 to ....
 * Branch: LE_Cal_ATM_FR_California_Postsync_960425_branch
 *
 * Revision 3.1.84.2  1996/04/25  22:12:51  cakyol
 * - rename lecsFinderModuleInit to lecs_finder_init, similar
 *   to all the other LANE module init routines.
 * - call it from the appropriate place (lane.c: lane_init).
 * Branch: LE_Cal_V112_0_2_branch
 *
 * Revision 3.1.84.1  1996/04/08  01:57:32  bbenson
 * Branch: LE_Cal_V112_0_2_branch
 * Sync of LE_Cal to new V112_0_2 sync point (post Port Ready).
 *
 * Revision 3.1.74.1  1996/03/22  22:55:36  rlowe
 * Non-sync of Synalc3_LE_Cal_V111_1_0_3_merge_branch to V111_1_3
 * yielding LE_Syn_Cal_V111_1_3_branch.
 * Branch: LE_Syn_Cal_V111_1_3_branch
 *
 * Revision 3.1.58.1  1996/03/05  06:43:40  rlowe
 * Apply LANE content to merge branch.
 * Branch: Synalc3_LE_Cal_V111_1_0_3_merge_branch
 *
 * Revision 3.1.46.1  1996/02/27  21:10:57  cakyol
 * non sync sync of LE_Cal_V111_0_16_branch to V111_1_0_3 yielding
 * LE_Cal_V111_1_0_3_branch
 * Branch: LE_Cal_V111_1_0_3_branch
 *
 * Revision 3.1.20.2  1996/01/26  02:02:50  cakyol
 * more code review mods (still more to come):
 *
 * - "show lane config" now shows all the details of the
 *   connected LESs, such as elan name, relative priority
 *   and whether active or not.
 * - in the lecs finder, protect against getnext going past
 *   the end of the lecs addresses table, by checking the
 *   incoming oid every time.
 * - in the lecs finder, add a parameter to the user interface
 *   limiting the number of lecs addresses the caller is
 *   interested in.
 * - use a static global buffer for debugging messages; saves
 *   stack space.
 * - make "les-timeout" and "lane global-lecs-address" commands
 *   hidden.
 * - prepend "atmSig_" to the 2 new atm signalling api function
 *   names.
 * - prepend "lsv_" to some LES functions for consistency.
 * - more white space, if...else formatting cleanup in some
 *   of the files.
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.20.1  1996/01/11  01:46:55  cakyol
 * non sync sync of LE_Calif_branch to V111_0_16 yielding
 *     LE_Cal_V111_0_16_branch
 *
 * Branch: LE_Cal_V111_0_16_branch
 *
 * Revision 3.1.8.2  1996/01/03  22:52:58  cakyol
 * - divided up debug categories to: all, events and packets
 *   for the LECS.  There were too many debugging messages
 *   for a single debug flag.
 *
 * - updated some of the outdated comments.
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.1.8.1  1995/12/29  01:07:09  cakyol
 * Initial commits for:
 *
 * LANE Fault Tolerant LECS Servers
 * LANE Fault Tolerant LES Servers
 * HSRP over LANE Client
 *
 * Branch: LE_Calif_branch
 *
 * Revision 3.1  1995/11/22  21:02:23  cakyol
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef lecs_finder_h
#define lecs_finder_h

/*
** This api is used to:
**
**	- Get you the list of all LECS addresses in the system.
**	  This is obtained from the ATM switch using ILMI.
**
**	- Get you the address of the master LECS address.
**
** It is the well known, call and callback scheme.  The user
** calls an api routine, which places the request on the
** service provider's q.  When the service is completed,
** the user is probed with the callback he/she provided.
**
** parameters of the callback routine:
**
**	swidb:		need I say more on this ?
**
**	transactionId:	is the SAME value, that was returned
**			by the api routine, when the request
**			was initially issued.
**
**	userContext:	This is a pointer the user supplied
**			when the api routine was called.  It
** 			is faithfully passed in to the user
**			callback.  Similar to the managed
**			timers context pointer.
**
**	count:		If this value is >= 0, it represents
**			the answer to the user query.  For
**			example, if the user request was to
**			get the master LECS address, this
**			value is expected to be 1.  If however,
**			the user request was to get the list of
**			global LECS addresses from the ILMI.
**			this value could be anywhere between 0 
**			and N.  0 means that there are NO 
**			configured LECS addresses on the switch.
**
**			If this value is negative, it indicates
**			that an error occured while trying to
**			process the user request.  The -ve
**			values are specified below.
**
**	lecs_addresses:	These are the return values to the
**			user query.  If count was 1, this value
**			represents a single LECS address.
**			If count was > 1, then it represents 
**			an ARRAY of LECS address values.  If 
**			the count value was <= 0, then this 
**			pointer should NOT be dereferenced.
**
**		********** IMPORTANT **********
**
** Note that the data returned thru the "lecs_addresses" pointer
** of the callback routine is very volatile.  It is expected that
** the user will immmediately take a copy of the data as soon as
** the callback is entered.  The data will NOT persist past the 
** context of the callback routine.
**
*/
typedef void (*FinderCallback) 
    (idbtype *swidb, uint transactionId, void *userContext,
     int count, hwaddrtype *lecs_addresses);

/* error conditions for < 0 "count" value */

#define INTERNAL_ERROR	(-1)	/* malloc and or consistency */
#define ILMI_PROBLEM	(-2)	/* ilmi problems, timeout etc */

/*
** these are the types of the requests that the user can ask
** the Finder module to perform for it.
*/
typedef enum _finderRequestType
{
    /* get the definitive current master LECS address */
    GET_MASTER_LECS_ADDRESS,

    /* get the list of ALL the LECS addresses in the system */
    GET_ALL_GLOBAL_LECSS,

} FinderRequestType;

/*
** This is the user api to be used to request something from
** the LECS Finder service.  The return value is a transaction
** id/key that should be used to track the request.  That same 
** value will be passed in to the user callback as the transaction
** id.  The return value is >= 0 for a valid trans id.  
** INTERNAL_ERROR (-1) is returned if there was a problem.
** 
** The "reqType" indicates which service is required.
** This can be one of the FinderRequestType defined above.
**
** "max" is for limiting the return values.  the callback
** will NOT deliver more than the specified value of
** lecs addresses, whatever the command.  If max is 0,
** then callback will not return ANY LECS addresses.
*/
extern uint lecsFinderRequest (idbtype *swidb,
    FinderRequestType reqType, void *userContext,
    FinderCallback callback, uint max);

/*
** some other externally useful routines.
** These are NOT part of the user api. So,
** do NOT use them.
*/

extern void clearLecsFinderModule
    (lane_info_t *lane_info);
extern void switchLecsAddress_add 
    (idbtype *swidb, boolean manual, hwaddrtype *hwaddr);
extern void switchLecsAddress_delete
    (idbtype *swidb, hwaddrtype *hwaddr);
extern void globalLecsAddresses_regenerate 
    (idbtype *swidb, char *command);

#endif lecs_finder_h

