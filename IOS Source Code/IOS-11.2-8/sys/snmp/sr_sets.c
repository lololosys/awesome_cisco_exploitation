/************************************************************************
 *                                                                      *
 *                              NOTE WELL                               *
 * This is vendor-supplied and vendor-supported code.  Do not make any  *
 * modifications to this code without the knowledge and consent of the  *
 * SNMP agent group.                                                    *
 *                                                                      *
 ************************************************************************/
/* $Id: sr_sets.c,v 3.3 1995/12/28 22:35:48 mordock Exp $
 * $Source: /vws/aam/cherf.111/ios/sys/snmp/sr_sets.c,v $
 *------------------------------------------------------------------
 * SNMP/SNMPv2 bi-lingual agent code.
 *
 * February 1994, SNMP Research
 *
 * Copyright (c) 1994-1995 by cisco Systems, Inc.
 * All rights reserved.
 *
 *------------------------------------------------------------------
 * $Log: sr_sets.c,v $
 * Revision 3.3  1995/12/28  22:35:48  mordock
 * CSCdi46168:  improve snmp modularity
 * move some code and globals out of the kernel subsystem and back into
 * the snmp subsystem
 *
 * Revision 3.2  1995/11/17  19:01:01  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  13:17:54  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  22:50:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */
/*
 *
 * Copyright (C) 1994 by SNMP Research, Incorporated.
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license and with the
 * inclusion of the above copyright notice. This software or any other
 * copies thereof may not be provided or otherwise made available to any
 * other person. No title to and ownership of the software is hereby
 * transferred.
 *
 * The information in this software is subject to change without notice
 * and should not be construed as a commitment by SNMP Research, Incorporated.
 *
 * Restricted Rights Legend:
 *  Use, duplication, or disclosure by the Government is subject to
 *  restrictions as set forth in subparagraph (c)(1)(ii) of the Rights
 *  in Technical Data and Computer Software clause at DFARS 52.227-7013
 *  and in similar clauses in the FAR and NASA FAR Supplement.
 *
 */

/*
 *                PROPRIETARY NOTICE
 *
 * This software is an unpublished work subject to a confidentiality agreement
 * and is protected by copyright and trade secret law.  Unauthorized copying,
 * redistribution or other use of this work is prohibited.
 *
 * The above notice of copyright on this source code product does not indicate
 * any actual or intended publication of such source code.
 *
 */

#if (! ( defined(lint) ) && defined(SR_RCSID))
static char     rcsid[] = "sets.c,v 1.12 1995/02/06 18:11:38 battle Exp";
#endif				/* (! ( defined(lint) ) && defined(SR_RCSID)) */

/*
 * sets.c,v
 * Revision 1.12  1995/02/06  18:11:38  battle
 * Changes for vrtx.
 *
 * Revision 1.11  1994/11/02  18:19:38  battle
 * added an ARGSUSED
 *
 * Revision 1.10  1994/10/28  17:32:18  vaughn
 * Placed new copyright in the file
 *
 * Revision 1.9  1994/10/26  21:58:41  vaughn
 * Changed copyrights to the new copyright structure
 *
 * Revision 1.8  1994/10/06  15:26:52  pearson
 * Got rid of ctype.h, replaced debug.h with diag.h
 *
 * Revision 1.7  1994/09/30  16:11:26  battle
 * hprt changes
 *
 * Revision 1.6  1994/08/04  15:37:39  battle
 * pSOS 2.0 changes
 *
 * Revision 1.5  1994/03/03  15:42:03  vaughn
 * Changed 0's in DPRINTF to APALWAYS.
 *
 * Revision 1.4  1994/02/14  16:42:19  pearson
 * Updated copyright notices for 1994.
 *
 * AMP
 *
 * Revision 1.3  1994/02/08  00:10:08  pearson
 * Here's a brief description of the changes in this commit of the source
 * tree:
 * 	The structure of the packaged tree has changed somewhat.
 * 	Assembly language files and system-specific startup code are in
 * 	the pkg/lib/sys directory.  There is a 'serial' directory which
 * 	contains rs232 code for all of the different platforms.
 *
 * 	Uart code has a more uniform initialization call now.  For
 * 	example, init_uart() is mapped to init code for different uarts
 * 	on different systems.
 *
 * 	The national semiconductor "rib" code is in this release, along
 * 	with support for the debug/rt debugger and paradigm locate.
 * 	That created some new directories in the pkg/cmd tree.  There is
 * 	support now for a sonic driver, the ethernet repeater mib, and
 * 	sonic/ric statistics.
 *
 * 	The dos agent and utilities now configure themselves from an
 * 	ascii text file, rather than a "device driver".
 *
 * 	Various parts of the standard agent have had large stack
 * 	variables moved to malloc-ed structures to save room.
 *
 * 	Some bugs which were exposed by the marshall/levi test tool have
 * 	been fixed.
 *
 * 	Two new hello programs have been created.  Hello3 is essentially
 * 	a ping server, which allows embedded developers to test their
 * 	network stack, and hello4 allows tests of bootp and tftp.
 *
 * 	Some new routines have been added to the pkg/lib/c directory.
 * 	Dmemdump dumps memory in hex and in ascii.  32-bit versions of
 * 	memcpy, memset, etc have been created.
 *
 * Revision 1.2  1993/11/08  20:28:01  battle
 * Added code so that a GEN_ERROR error would be returned to v1 managers
 * when sets fail after all tests succeed.
 *
 *
 * Revision History:
 *
 *  2/4/89 JDC
 *  amended copyright notice
 *  changed references from "gotone" to "snmpd"
 *
 *  7/16/89 JDC
 *  worked on error messages
 *
 *  11/8/89 JDC
 *  Make it print pretty via tgrind
 *
 *  20-Sep-90
 *  Added MIB-II support.
 *
 *  27-Jan-91 JDC
 *  Drop the "global" from the name of the MIB variables in the SNMP group
 *
 *  19-May-91 JDC
 *  Finish support of setable table entries for full MIB 2 support
 *
 *  27-May-91
 *  JDC Finish modifications for TOO BIG
 *
 *  27-Jun-91 AMP
 *  Put print statements into conditional compiles.
 *
 *  27-Jun-91 JDC
 *  Fixed bug which caused error PDUs with GEN_ERROR to contain
 *    an incorrect value for errorIndex.
 *
 *  30-Jul-91 AMP
 *  Merged the MROM/PKG and UNIX trees
 *
 *  7-Feb-92 MRD
 *  Removed recursion from freeDoList().
 *  Made changes to support new test/set calling conventions.
 *  Made changes to support new find_var calling conventions.
 *
 *  30-Jun-92 MRD
 *  Changed format of do-list. dropped data->function and moved
 *  valid bit field to do-list rather than data->valid. Also
 *  changed valid bit field to be a char string (max VALIDSZ * 8
 *  objects per group).
 *
 *  27-Aug-92 MGH
 *  Modified do_sets() so community strings that appear in the party table
 *  use access control. ( See calls to CheckMIBView() )
 *
 * 9-Dec-92 DLB
 *   Lint nits.
 *
 * 18-Dec-92 MGH
 * Changed the third parameter of do_sets() to target to reflect the new
 * security drafts.
 *
 * 7-Jan-93 MGH
 * Changed the third parameter of do_sets() to context to reflect the new
 * security drafts.
 *
 * 19-Jan-93 MGH
 * Added code to increment the snmpV1BadCommunityUses when needed.
 * Changed code involving community strings and the party table.
 *
 * 20-Jan-93 MGH
 * Added code in do_sets() after pass1 to check for doListCur->state =
 * NOT_READY_STATE, CREATE_AND_GO_STATE, or NOT_IN_SERVICE_STATE.  These three
 * states have to do with the RowStatus textual convention.  If
 * doListCur->state is equal to any of these three values, do_sets returns an
 * error_pdu = INCONSISTENT_VALUE_ERROR.
 *
 * For a NOT_WRITABLE error, made do_sets check for oidstatus == READ_CREATE.
 *
 * 24-Mar-93 MGH
 * Converted internal representation of partyTDomain from an OID to an integer.
 *
 * 3-June-93 MGH
 * Changed some code in Pass 2 of do_sets.  I assign the local variable,
 * vbPos, the value of doListCur->vbPos before doListHead is freed.
 *
 * 25-June-93 MGH
 * Changed the name of asod to contextInfo.  The contents of that structure
 * have changed also.  Please see snmpd.h
 *
 * 01-Jul-1993 DLB
 * Changed name of freeDoList() to cleanupDoList().  Added code for undo
 * processing.
 *
 * 20-Jul-1993 DAR
 * use valid bitfield from snmptype.h instead of the fixed 80 bit bitfield
 *
 */

#include "master.h"




#include "sr_snmpd.h"
#include "sr_snmpmib2.h"

#ifdef SETS



/*----------------------------------------------------------------------
 * The do-list is a linked list with an allocated field. Traverse
 * the list, free the allocated element and the list itself.
 *----------------------------------------------------------------------*/
static int cleanupDoList SR_PROTOTYPE((doList_t *doList));

static int
cleanupDoList(doList)
    doList_t       *doList;
{
    doList_t       *p = doList;
    doList_t       *trash;
    int             cc;
    int             error_status = NO_ERROR;

    while (p != NULL) {
	trash = p;

	p = p->next;

	if (trash->data != NULL && trash->cleanupMethod != NULL) {

	    cc = (trash->cleanupMethod) (trash);

	    if (error_status == NO_ERROR && cc != NO_ERROR) {
		error_status = cc;
	    }
	}

	free((char *) trash);
    }
    return error_status;
}				/* cleanupDoList */


/*----------------------------------------------------------------------
 * The do_sets function is divided into 3 passes.
 *
 * Pass 1: Perform simple error tests while building a do-list. Call
 *         the "test" method for the given object.
 *
 * Pass 2: Examine the results of testing all object sets. Some
 *         may not be valid if all "related" objects have not been
 *         set. (For example some tabular objects require several
 *         columns of a row to be specified)
 *
 * Pass 3: Perform the set method on each do-list item. If the set
 *         method fails there is problem with the test method.
 *
 *----------------------------------------------------------------------*/

/*

  Here is a thumbnail sketch of how do_sets has changed to incorporate undo:

  A new method type has been added to each doList element; the undoMethod.  The
  undoMethod has the same calling interface as the setMethod.  A new field has
  also been added to each doList element; the undodata field in analogy the
  the existing data field.

  It is the responsibility of the undoMethod, if called, to restore all mib
  variables which were affected by the set on the doList element which it
  is passed to the state they were in before the set operation.  If an
  undoMethod is unsuccessful in affecting this change, or if it is unable
  to assess whether or not it was successful, it should return undoFailed,
  or some other non 0 error code.  If it succeeds it should return noError.

  The undoMethod and undodata will be filled in by the test method, just as
  for the setMethod and data.  The undoMethod may be NULL, and if it is
  this indicates that the setMethod should not fail if the test method
  succeeds.  If undoMethod is not NULL, undodata should be filled in by the
  test method with a copy of what the row or scalar group looked like before
  the set began, stored in the same format as the data field.

  The freeMethod's name has been changed to cleanupMethod and its semantics
  have been changed as follows:

      1) it now returns an error code instead of void
      2) it has the added responsibility of finalizing any deletions made
         to groups/rows other than the one represented in undodata.  This
         is to allow the setMethod and undoMethod to cooperate by
         merely marking (or unmarking) rows in other groups for deletion.

  A further semantic change is made in pass 3 of do_sets in that doList
  elements which have undoMethods are set first so that if any of them fail,
  it will not be necessary to undo changes to rows/groups which lack
  undoMethods.

  If a set fails even though the cooresponding test succeeded, then further
  sets will be foregone and the undoMethod (if it exists) will be called for
  each doList item which had been successfully set.  If all the undo calls
  succeed then the error commitFailed is returned.  If one of the undo calls
  fails or does not exist, then further undo calls are foregone and the error
  undoFailed is returned.

  After the dust settles, either because a test failed, a set failed and undo's
  completed, an undo failed, or everything worked fine, the cleanupMethod for
  for each doList element is called.  If one of these fails, the others are
  still called, but an undoFailed error is returned.  It's a very bad thing
  for a cleanupMethod to fail.

  -David L. Battle
  July 1, 1993
 */

#define FNAME "do_sets"
Pdu            *
do_sets(inPdu, contextInfo, context, domain)
    Pdu            *inPdu;
    ContextInfo    *contextInfo;
    OID            *context;
    int             domain;
{
    doList_t       *doListHead = NULL;
    doList_t       *doListCur = NULL;
    doList_t       *temp = NULL;
    int             leftOver = 0;
    long            req = inPdu->u.normpdu.request_id;
    int             error_index;
    int             cc;
    VarBind        *curVb;
    ObjectInfo     *curObj;
    int             i;
    int             error_status;

#ifdef SR_SNMPv2
    long            viewIndex;
    int             NonUndoablesStarted = 0;
#endif				/* SR_SNMPv2 */


    /*
     * Pass 1: Scan the entire variable binding list. On errors, construct an
     * error pdu and return it.  On success NULL is returned.
     */
    error_index = 0;

#ifdef SR_SNMPv2
    viewIndex = GetViewIndex(context);
#endif				/* SR_SNMPv2 */

    for (curVb = inPdu->var_bind_list; curVb != NULL; curVb = curVb->next_var) {
	error_index++;

#ifdef SR_SNMPv2
	/*
	 * (1)  If the variable binding's name specifies a variable which is
	 * not accessible by this request, then the value of the
	 * Response-PDU's error-status field is set to `noAccess', and the
	 * value of its error-index field is set to the index of the failed
	 * variable binding.
	 */
	if (CheckMIBView(curVb->name, viewIndex) < 0) {
	    DPRINTF((APTRACE, "%s: variable not in mib view.\n", FNAME));
	    error_status = NO_ACCESS_ERROR;
	    goto fail;
	}
#endif				/* SR_SNMPv2 */


	/*
	 * (2)  Otherwise, if the variable binding's name specifies a
	 * variable which does not exist and could not ever be created, then
	 * the value of the Response-PDU's error- status field is set to
	 * `noCreation', and the value of its error-index field is set to the
	 * index of the failed variable binding.
	 */
	if ((i = FindObjectClass(curVb->name, EXACT, domain)) == -1) {
	    DPRINTF((APTRACE, "%s: can't find object\n", FNAME));
	    error_status = NO_CREATION_ERROR;
	    goto fail;
	}
	curObj = GetOidElem(i);

	/*
	 * (3)  Otherwise, if the variable binding's name specifies a
	 * variable which exists but can not be modified no matter what new
	 * value is specified, then the value of the Response-PDU's
	 * error-status field is set to `notWritable', and the value of its
	 * error-index field is set to the index of the failed variable
	 * binding.
	 */

	if (curObj->oidstatus != SR_READ_WRITE

#ifdef SR_SNMPv2
	    && curObj->oidstatus != SR_READ_CREATE
#endif				/* SR_SNMPv2 */

	    ) {

	    DPRINTF((APTRACE, "%s: Attempt to set read-only object\n", FNAME));

	    error_status = NO_ACCESS_ERROR;

#ifdef SR_SNMPv2
	    if (domain == SNMPv2_DOMAIN) {
		error_status = NOT_WRITABLE_ERROR;
	    }
#endif				/* SR_SNMPv2 */

	    goto fail;
	}

	/*
	 * (4)  Otherwise, if the variable binding's value field specifies,
	 * according to the ASN.1 language, a type which is inconsistent with
	 * that required for the variable, then the value of the
	 * Response-PDU's error-status field is set to `wrongType', and the
	 * value of its error-index field is set to the index of the failed
	 * variable binding.
	 */

	if (curObj->oidtype != curVb->value.type) {
	    DPRINTF((APTRACE, "%s: value wrong type for object being set\n",
		     FNAME));

	    error_status = WRONG_TYPE_ERROR;
	    goto fail;
	}


	/*
	 * This variable binding has passed preliminary tests. Add it to the
	 * do-list. "leftOver" indicates the last element of the do-list is
	 * actually empty and left over from the previous iteration. (See the
	 * test method call below for details)
	 */
	if (!leftOver) {
	    temp = NULL;
	    if ((temp = (doList_t *) malloc(sizeof(doList_t))) == NULL) {
		DPRINTF((APTRACE, "%s: malloc failed\n", FNAME));

		if (temp != NULL)
		    free((char *) temp);

		error_status = RESOURCE_UNAVAILABLE_ERROR;
		goto fail;
	    }

	    memset(temp, 0, sizeof(doList_t));

	    if (doListHead == NULL) {
		doListHead = temp;
	    }
	    else {
		doListCur->next = temp;
	    }

	    doListCur = temp;
	}

	doListCur->setMethod = NULL;
	doListCur->cleanupMethod = NULL;
	doListCur->state = UNUSED;
	doListCur->data = (void *) NULL;
	doListCur->next = NULL;
	doListCur->vbPos = error_index;

#ifdef SR_SNMPv2
	doListCur->undoMethod = NULL;
	doListCur->undodata = (void *) NULL;
#endif				/* SR_SNMPv2 */


	/*
	 * Call the test method for this object. This method verifies the
	 * object instance, the object type (class), and its value. The test
	 * method may combine this object with a prior, "related" object in
	 * the do-list. If so, "doListCur->setMethod" will be NULL,
	 * indicating the set method need not be called for this do-list
	 * item. If this happens, the variable "leftOver" is set to indicate
	 * that during the next iteration of this loop another do-list
	 * element does not need to be allocated. (Note: this may leave a
	 * single, unused element in the do-list. This will not cause an
	 * error.)
	 */

	cc = (*curObj->test) (curVb->name, curObj, &curVb->value,
			      doListHead, doListCur, contextInfo);

	leftOver = (doListCur->state == UNUSED);

	if (cc != NO_ERROR) {
	    DPRINTF((APTRACE, "%s: test method returned error\n", FNAME));
	    error_status = cc;
	    goto fail;
	}
    }

    DPRINTF((APTRACE, "%s: pass 1 of set request processing done\n", FNAME));



    /*
     * Pass 2: Some objects are members of relations, and as such must be set
     * atomicly. For example columns in a row must be set at once. So, scan
     * the do-list and determine if all nodes in the list are either empty
     * (can occur if related items are combined into a single do-list
     * element), ADD_MODIFY,  or DELETE.
     */
    for (doListCur = doListHead; doListCur != NULL; doListCur = doListCur->next) {
	if (doListCur->state != UNUSED && doListCur->state != ADD_MODIFY &&
	    doListCur->state != DELETE) {

	    DPRINTF((APTRACE, "%s: All related objects not set\n", FNAME));

	    error_status = INCONSISTENT_VALUE_ERROR;
	    error_index = doListCur->vbPos;
	    goto fail;
	}
    }

    DPRINTF((APTRACE, "%s: pass 2 of set request processing done\n", FNAME));

    /*
     * Pass 3: Now actually do the set. For each element of the do-list in
     * which the set_func is non-NULL, call that set method. This should
     * never fail since all tests should be done in the test method.
     */

#ifdef SR_SNMPv2
    if (domain == SNMPv2_DOMAIN) {
	/*
	 * First do just the sets that have undo methods to maximize the
	 * probability that we'll be able to undo everything if one of them
	 * fails.
	 */

	for (doListCur = doListHead; doListCur != NULL; doListCur = doListCur->next) {
	    if (doListCur->setMethod != NULL && doListCur->undoMethod != NULL) {

		cc = (*doListCur->setMethod) (doListHead, doListCur, contextInfo);

		if (cc != NO_ERROR) {
		    goto undo;
		}
	    }
	}
    }
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv2
    /*
     * Do the sets that don't have undo methods (or all of them for domain ==
     * SNMPv1_DOMAIN).
     */

    for (doListCur = doListHead; doListCur != NULL; doListCur = doListCur->next) {
	if ((doListCur->setMethod != NULL && doListCur->undoMethod == NULL)
	    || (domain == SNMPv1_DOMAIN && doListCur->setMethod != NULL)) {

	    cc = (*doListCur->setMethod) (doListHead, doListCur, contextInfo);

	    if (cc != NO_ERROR) {
		if (domain == SNMPv2_DOMAIN) {
		    goto undo;
		}
		else {
		    error_status = GEN_ERROR;
		    error_index = doListCur->vbPos;
		    goto fail;
		}
	    }
	    NonUndoablesStarted = 1;
	}
    }
#else				/* SR_SNMPv2 */
    /* do the set calls for v1only */

    for (doListCur = doListHead; doListCur != NULL; doListCur = doListCur->next) {
	if (doListCur->setMethod != NULL) {

	    cc = (*doListCur->setMethod) (doListHead, doListCur, contextInfo);
	    if (cc != NO_ERROR) {
		error_status = GEN_ERROR;
		error_index = doListCur->vbPos;
		goto fail;
	    }
	}
    }
#endif				/* SR_SNMPv2 */

#ifdef SR_SNMPv2
undo:
    if (domain == SNMPv2_DOMAIN && doListCur != NULL) {
	/*
	 * If any of these assignments fail (even after all the previous
	 * validations), then all other assignments are undone, and the
	 * Response-PDU is modified to have the value of its error-status
	 * field set to `commitFailed', and the value of its error-index
	 * field set to the index of the failed variable binding.
	 */
	DPRINTF((APTRACE, "%s: set failed, undoing\n", FNAME));
	for (temp = doListHead; temp != doListCur; temp = temp->next) {
	    if (temp->undoMethod == NULL) {
		if (NonUndoablesStarted) {
		    break;
		}
		continue;
	    }
	    cc = (*temp->undoMethod) (doListHead, temp, contextInfo);
	    if (cc != NO_ERROR) {
		break;
	    }
	}
	if (temp != doListCur) {
	    /*
	     * If and only if it is not possible to undo all the assignments,
	     * then the Response-PDU is modified to have the value of its
	     * error-status field set to `undoFailed', and the value of its
	     * error-index field is set to zero.  Note that implementations
	     * are strongly encouraged to take all possible measures to avoid
	     * use of either `commitFailed' or `undoFailed' - these two
	     * error-status codes are not to be taken as license to take the
	     * easy way out in an implementation.
	     */
	    DPRINTF((APTRACE, "%s: undo failed\n", FNAME));
	    error_status = UNDO_FAILED_ERROR;
	    error_index = 0;
	}
	else {
	    error_status = COMMIT_FAILED_ERROR;
	    error_index = doListCur->vbPos;
	}
	goto fail;
    }
#endif				/* SR_SNMPv2 */

    DPRINTF((APTRACE, "%s: pass 3 of set request processing done\n", FNAME));

    if (cleanupDoList(doListHead) != NO_ERROR) {
	error_status = UNDO_FAILED_ERROR;
	error_index = 0;
	goto fail;
    }

    snmpData.snmpInTotalSetVars += error_index;

    return (NULL);

fail:
    if (doListHead != NULL) {
	if (cleanupDoList(doListHead) != NO_ERROR) {
	    error_status = UNDO_FAILED_ERROR;
	}
    }

    return (make_error_pdu(GET_RESPONSE_TYPE, req, error_status,
			   error_index, inPdu, domain));

}				/* do_sets() */

#undef FNAME
/*----------------------------------------------------------------------
 * This function should never be called. If it is, "do_sets()" or the
 * object data base has been corrupted.
 *----------------------------------------------------------------------*/
#define FNAME "readOnly_test"
int
readOnly_test (
    OID            *incoming,
    ObjectInfo     *object,
    ObjectSyntax   *value,
    doList_t       *doHead,
    doList_t       *doCur,
    ContextInfo    *auth)
{
    DPRINTF((APALWAYS, "%s: Internal error; attempted to set a read-only variable.\n",
	     FNAME));
    return (NOT_WRITABLE_ERROR);
}				/* readOnly_test() */

#undef FNAME
#endif				/* SETS */
