/* $Id: tree.c,v 3.8.12.2 1996/08/28 13:19:11 thille Exp $
 * $Source: /release/112/cvs/Xsys/util/tree.c,v $
 *------------------------------------------------------------------
 * Red/black tree structure subroutines
 *
 * September 1991, David Edwards
 *
 * Copyright (c) 1991-1997 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: tree.c,v $
 * Revision 3.8.12.2  1996/08/28  13:19:11  thille
 * CSCdi67083: Message externs need a cleanup.
 * Branch: California_branch
 * Get rid of all extern msgsym()s in .c files, rework logger.h and
 * msg_foo.c files so they are included and provide the externs.  Clean up
 * other random externs in .c files.
 *
 * Revision 3.8.12.1  1996/03/18  22:31:06  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.6.2.2  1996/03/07  11:00:20  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.6.2.1  1996/02/20  21:45:29  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.8  1996/02/13  20:49:58  hampton
 * Clean up includes of gdb.h and references to breakpoint.  [CSCdi48844]
 *
 * Revision 3.7  1996/02/08  23:11:46  jjohnson
 * CSCdi48362:  Need a consistent, global mechanism for writing plural
 * strings.  Standardize on the Plural and ArgAndPlural macros
 *
 * Revision 3.6  1996/01/22  07:30:32  mdb
 * CSCdi47065:  misuse of NULL macro in IOS sources
 *
 * Revision 3.5  1995/12/08  04:56:05  dstine
 * CSCdi45233:  Namespace breakage for RBTree
 *         Just normalize all RBTree* names.
 *
 * Revision 3.4  1995/12/07  22:33:10  hampton
 * Label the large chunks of memory allocated at system initialization
 * time.  This makes it easier to see where all the memory in router is
 * going.  [CSCdi45209]
 *
 * Revision 3.3  1995/11/17  17:56:44  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:47:08  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:43:47  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.5  1995/11/08  21:34:07  shaker
 * Merge Arkansas_branch into 11.1 mainline.
 *
 * Revision 2.4  1995/08/26  00:10:04  slin
 * CSCdi39372:  RBTrees not built correctly
 * - Wrong node used when RBTreeDeleteFixup() called
 *
 * Revision 2.3  1995/07/18  06:49:00  hampton
 * Miscellaneous changes to eliminate direct references to the system
 * clock and to convert to always using the timer macros to access timers.
 * [CSCdi37217]
 *
 * Revision 2.2  1995/06/20 20:11:15  dkatz
 * CSCdi36037:  Tons of memory wasted on no memory messages
 * Phase II--remove error messages.  Remove type coercion of malloc calls.
 *
 * Revision 2.1  1995/06/07  23:13:31  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#include "master.h"
#include <string.h>
#include <ciscolib.h>
#include "interface_private.h"
#include "packet.h"
#include "logger.h"
#include "plural.h"
#include "../if/arp.h"
#include "config.h"
#include "tree.h"
#include "../os/msg_system.c"		/* Not a typo, see logger.h */

#define SYS_ERR_TABLEERR &msgsym(TABLEERR, SYS)



/* Define TREEDEBUG to force tree validation after every modification. */
/* #define TREEDEBUG */

#ifdef TREEDEBUG
#define INLINE
#define STATIC
#else
#define INLINE inline
#define STATIC static
#endif

#define TREEASSERT(x) {}

/* NIL sentinel */
STATIC treeLink nil;

INLINE treeLink *
NIL (void)
{
    return &nil; 
}

/*
 * This function is to be used for the purposes of inline optimization only.
 *
 * There are very, very few ordinary circumstances when anyone outside
 * the tree.c module needs to know about the internal private parts of
 * tree.c -- performance is, however, a circumstance.
 */
treeLink *
RBTreeGetNIL (void)
{
    return (&nil);
}

INLINE treeLink *
RealNode (treeLink* x) 
{ 
    return ((x == NIL())?(treeLink*)NULL:x);
}

static INLINE void
UnlinkNode (treeLink *z)
{
    /* unlink from doubly linked list */
    if (z->prevl)
	z->prevl->nextl = z->nextl;
    if (z->nextl)
	z->nextl->prevl = z->prevl;
    z->nextl = z->prevl = NULL;
}

static INLINE void
LinkLeftChild (treeLink *parent, treeLink *child)
{
    child->nextl = parent;
    child->prevl = parent->prevl;
    parent->prevl = child;
    if (child->prevl)
	child->prevl->nextl = child;
}

static INLINE void
LinkRightChild (treeLink *parent, treeLink *child)
{
    child->prevl = parent;
    child->nextl = parent->nextl;
    parent->nextl = child;
    if (child->nextl)
	child->nextl->prevl = child;
}
    
/*
 * UpdateNodeInfo - Update node information
 */
STATIC void
UpdateNodeInfo (rbTree* T, treeLink* x)
{
    if ((x == NULL) || (x == NIL()))
	return;
    x->size = 1;
    x->subkey = x->key;
    if (x->left != NIL()) {
	x->size += x->left->size;
	if (T->intervalkey) {
	    x->subkey.i.high = max(x->subkey.i.high,x->left->subkey.i.high);
	    x->subkey.i.low = min(x->subkey.i.low,x->left->subkey.i.low);
	} else if (T->thirtytwobitkey) {
	    x->subkey.l = max(x->subkey.l,x->left->subkey.l);
	} else {
	    (*T->keymax)(&x->subkey,x->subkey,x->left->subkey);
	}
    }
    if (x->right != NIL()) {
	x->size += x->right->size;
	if (T->intervalkey) {
	    x->subkey.i.high = max(x->subkey.i.high,x->right->subkey.i.high);
	    x->subkey.i.low = min(x->subkey.i.low,x->right->subkey.i.low);
	} else if (T->thirtytwobitkey) {
	    x->subkey.l = max(x->subkey.l,x->right->subkey.l);
	} else {
	    (*T->keymax)(&x->subkey,x->subkey,x->right->subkey);
	}
    }
}

/*
 * IntervalsOverlap - Compare intervals and return true if they overlap
 */
static boolean
IntervalsOverlap (interval a,interval b)
{
    return !((a.high < b.low)||(b.high < a.low));
}

#ifdef NOTUSED
/*
 * IntervalsMatch - Compare intervals and return true if they are equal
 */
static boolean
IntervalsMatch (interval a,interval b)
{
    return ((a.low == b.low)&&(a.high == b.high));
}
#endif

/*
 *
 */
static void RBTreeKeyMaxDefault (treeKey *result, treeKey a, treeKey b)
{
    if (a.l > b.l)
	result->l = a.l;
    else
	result->l = b.l;
}

/*
 *
 */
static int RBTreeKeyCompareDefault (treeKey a, treeKey b)
{
    if (a.l == b.l)
	return(0);
    if (a.l < b.l)
	return(-1);
    return(1);
}

/*
 *
 */
void RBTreeKeyMaxLongLong (treeKey *result, treeKey a, treeKey b)
{
    if (a.ll > b.ll)
	result->ll = a.ll;
    else
	result->ll = b.ll;
}

/*
 *
 */
int RBTreeKeyCompareLongLong (treeKey a, treeKey b)
{
    if (a.ll == b.ll)
	return(0);
    if (a.ll < b.ll)
	return(-1);
    return(1);
}

/*
 * InitRBTrees - Initialize data used by this module
 */
STATIC void
InitRBTrees (void)
{
    memset(&nil, 0,sizeof(treeLink));
    nil.left = NIL();
    nil.right = NIL();
    nil.parent = NIL();
    nil.freeList = NIL();
    nil.color = BlackNode;
}

/*
 * RBTreeCreate - Create a new empty tree and return it
 */
rbTree*
RBTreeCreate (char* protocol, char *abbrev, char* name, treeKeyPrint printfn,
	      boolean *debug_flag)
{
    rbTree* iTree;
    char buffer[80], *full_name;

    InitRBTrees();
    sprintf(buffer, "%s RBTree", abbrev, name);
    full_name = strdup(buffer);
    iTree = malloc_named(sizeof(rbTree), full_name ? full_name : name);
    if (iTree == NULL) {
	return (NULL);
    }
    memset(iTree, 0, sizeof(rbTree)); /* Zero all counters */
    iTree->root = NIL();	/* Initialize the head and free ptrs */
    iTree->freeList = NIL();
    iTree->protoName = protocol;
    iTree->protoAbbrev = abbrev;
    iTree->fullName = full_name;
    iTree->tableName = name;
    iTree->lastErr = NULL;
    TIMER_STOP(iTree->errTimer);
    iTree->view = 0;
    iTree->thirtytwobitkey = TRUE;
    iTree->intervalkey = FALSE;
    iTree->keymax = RBTreeKeyMaxDefault;
    iTree->keycompare = RBTreeKeyCompareDefault;
    if (printfn)
	iTree->print_key = printfn;
    else
	iTree->print_key = RBTreePrintKeyLong;
    iTree->debug_flag = debug_flag;
    return (iTree);
}

/*
 *
 */
void
RBTreeNon32Bit (rbTree* T, treeKeyMax keymax, treeKeyCompare keycompare)
{
    if (!T)
	return;
    T->thirtytwobitkey = FALSE;
    T->keymax = keymax;
    T->keycompare = keycompare;
}


/*
 * RBTreeDestroy - Delete and free all unlocked nodes in the tree.  If
 * all nodes are returned to the free memory pool, also return the memory
 * for the tree structure itself.
 */
rbTree*
RBTreeDestroy (rbTree* T, boolean *debug_flag)
{
    treeLink *x, *nxt;
    
    if (!T)
	return(NULL);

    x = RBTreeFirstNode(T);
    while (x) {
	nxt = RBTreeNextNode(x);
	RBTreeDelete(T, x);
	x = nxt;
    }
    if (RBTreeTrimFreeList(T)) {
	if (*debug_flag)
	    buginf("\n%s: (%s) RBTreeDestroy failed - %d nodes in use",
		   T->protoAbbrev, T->tableName, RBTreeReleasedNodeCount(T));
	return(T);
    }
    if (T->fullName)
	free(T->fullName);
    free(T);
    return(NULL);
}

/*
 * SubRBTreeMinimum - Get the node with the smallest key from the subtree
 */
static INLINE treeLink*
SubRBTreeMinimum (treeLink* node)
{
    register treeLink* x = node;
    while (x->left != NIL())
	x = x->left;
    return (RealNode(x));
}

/*
 * SubRBTreeMaximum - Get the node with the largest key from the subtree
 */
static INLINE treeLink*
SubRBTreeMaximum (treeLink* node)
{
    register treeLink* x = node;
    while (x->right != NIL())
	x = x->right;
    return RealNode(x);
}

/*
 * SubRBTreeSuccessor - Get the next logical node from tree
 *
 * Returns the node with the smallest key greater than the current node.
 */
STATIC treeLink*
SubRBTreeSuccessor (treeLink* node)
{
    register treeLink* x = node;
    treeLink* y;
    if (x->right != NIL())
	return SubRBTreeMinimum(x->right);
    y = x->parent;
    while ((y != NIL()) && (x == y->right)) {
	x = y;
	y = y->parent;
    }
    return RealNode(y);
}

/*
 * RBTreesError - Check module globals for validity.
 */
STATIC char*
RBTreesError (void)
{
    if (nil.right != NIL()) return "Right pointer invalid";
    if (nil.left != NIL()) return "Left pointer invalid";
    if (nil.parent != NIL()) return "Parent pointer invalid";
    if (nil.freeList != NIL()) return "FreeList pointer invalid";
    if (nil.key.ll != 0) return "Key value invalid";
    if (nil.subkey.ll != 0) return "Key summary value invalid";
    if (nil.size != 0) return "Size value invalid";
    if (nil.color != BlackNode) return "Color is invalid";
    return NULL;
}

STATIC char*
RBTreeError (rbTree* T)
{
    if (T == NULL) return "Pointer is NULL";
    if (!validmem(T)) return "Pointer is invalid";
    if (!validmem(T->root) && (T->root != NIL()))
	return "Root pointer is invalid";
    if (!validmem(T->freeList) && (T->freeList != NIL()))
	return "Free list pointer is invalid";
    return NULL;
}

STATIC char*
RBRootError (rbTree* T)
{
    if (T->activeNodes == 0) {
	if (T->root != NIL()) return "Tree empty but root not NIL";
    } else {
	if (T->root->size != T->activeNodes) 
	    return "Root size differs from active count";
	if (T->root->parent != NIL())
	    return "Root node has non-NIL parent";
    }
    return NULL;
}

STATIC char*
RBNodeVerifyError (rbTree* T, treeLink* x)
{
    if ((x->left->size + x->right->size + 1) != x->size)
	return "Size value is wrong";
    if ((x->parent != NIL()) && (x->parent->size <= x->size))
	return "Parent size is wrong";
    if (x->left != NIL()) {
	if (x->left->parent != x)
	    return "Left child has bad parent ptr";
	if (T->intervalkey) {
	    if (x->left->key.i.low > x->key.i.low)
		return "Left child has larger key";
	    if (x->left->subkey.i.low != x->subkey.i.low)
		return "Left child min subkey differs";
	}
    } else {
	if ((T->intervalkey)&&(x->subkey.i.low != x->key.i.low))
	    return "Min subkey not equal to low key";
    }
    if (x->right != NIL()) {
	if (x->right->parent != x)
	    return "Right child has bad parent ptr";
	if ((T->intervalkey)&&(x->right->key.i.low < x->key.i.low))
	    return "Right child has smaller key";
	if (T->intervalkey) {
	    if (x->right->subkey.i.high != x->subkey.i.high)
		return "Right child max subkey differs";
	} else if (T->thirtytwobitkey) {
	    if (x->right->subkey.l != x->subkey.l)
		return "Right child max subkey differs";
	} else {
	    if ((*T->keycompare)(x->right->subkey, x->subkey) != 0)
		return "Right child max subkey differs";
	}
    } else {
	if (T->intervalkey) {
	    if (x->key.i.high != x->subkey.i.high)
		return "Max subkey not equal to high key";
	} else if (T->thirtytwobitkey) {
	    if (x->key.l != x->subkey.l)
		return "Max subkey not equal to high key";
	} else {
	    if ((*T->keycompare)(x->key, x->subkey) != 0)
		return "Max subkey not equal to high key";
	}
    }
    if (x->color == RedNode) {
	if ((x->left->color == RedNode)||(x->right->color == RedNode)) 
	    return "Red node and red child (violates #3)";
	if (x->parent->color == RedNode)
	    return "Red node and red parent (violates #3)";
    }
    if (x->parent != NIL())
	if ((x->parent->right != x)&&(x->parent->left != x))
	    return "Node is not a sibling of the parent";
    if (x->freeList != NIL())
	return "Node has an invalid freelist pointer";
    if (x->size == 1)
	if ((x->left != NIL()) || (x->right != NIL()))
	    return "Leaf node has child(ren)";
    if (x->size < 1)
	return "Node has an invalid size (less than 1)";
    if (x->nextl)
	if (x->nextl->prevl != x)
	    return "Previous link of next node doesn't point to us";
    if (x->prevl)
	if (x->prevl->nextl != x)
	    return "Next link of previous node doesn't point to us";
    return NULL;
}

STATIC char*
RBNodesVerifyError (rbTree* T)
{
    treeKey lastkey;
    char* err = NULL;
    treeLink* x = SubRBTreeMinimum(T->root);
    if (x != NULL) {
	if (T->intervalkey)
	    if (x->key.i.low != T->root->subkey.i.low)
		return "Smallest key is not root min subkey";
	lastkey = x->key;
    }
    while (x != NULL) {
	if ((err = RBNodeVerifyError(T, x)) != NULL)
	    return err;
	if (T->intervalkey) {
	    if (lastkey.i.low > x->key.i.low)
		return "Nodes are out of order";
	} else if (T->thirtytwobitkey) {
	    if (lastkey.l > x->key.l)
		return "Nodes are out of order";
	} else {
	    if ((*T->keycompare)(lastkey, x->key) > 0)
		return "Nodes are out of order";
	}
	lastkey = x->key;
	x = SubRBTreeSuccessor(x);
    }
    x = SubRBTreeMaximum(T->root);
    if (x != NULL) {
	if (T->intervalkey) {
	    if (x->key.i.high != T->root->subkey.i.high)
		return "Largest key is not root max subkey";
	} else if (T->thirtytwobitkey) {
	    if (x->key.l != T->root->subkey.l)
		return "Largest key is not root max subkey";
	} else {
	    if ((*T->keycompare)(x->key, T->root->subkey) != 0)
		return "Largest key is not root max subkey";
	}
    }
    return NULL;
}


STATIC char*
RBFreeListError (rbTree* T)
{
    if (T->freeNodes == 0) {
	if (T->freeList != NIL())
	    return "Free list empty but not NIL";
    } else {
	treeLink* x = T->freeList;
	int cnt = 0;
	while (x != NIL()) {
	    cnt++;
	    if (!validmem(x))
		return "Free list is corrupt";
	    if ((x->left != NIL())||(x->right != NIL())||(x->parent != NIL()))
		return "Non-nil links for free node";
	    x = x->freeList;
	}
	if (cnt != T->freeNodes)
	    return "Free node count wrong";
    }
    return NULL;
}

/*
 * RBTreeValid - Validate the tree structure
 */
boolean
RBTreeValid (rbTree* T, boolean quiet, char const * title)
{
    char* err;
    T->lastErr = NULL;
    err = RBTreesError();
    if (err != NULL) {
	if (!quiet)
	    buginf("\n%s: %s(%s) TreeValid failed - NIL error: %s",
		   T->protoAbbrev,title,T->tableName,err);
	T->lastErr = err;
	return FALSE;
    }
    err = RBTreeError(T);
    if (err != NULL) {
	if (!quiet)
	    buginf("\n%s: %s(%s) TreeValid failed - %s",
		   T->protoAbbrev,title,T->tableName,err);
	T->lastErr = err;
	return FALSE;
    }
    err = RBRootError(T);
    if (err != NULL) {
	if (!quiet)
	    buginf("\n%s: %s(%s) TreeValid failed - Root error: %s",
		   T->protoAbbrev,title,T->tableName,err);
	T->lastErr = err;
	return FALSE;
    }
    err = RBNodesVerifyError(T);
    if (err != NULL) {
	if (!quiet)
	    buginf("\n%s: %s(%s) TreeValid failed - Node error: %s",
		   T->protoAbbrev,title,T->tableName,err);
	T->lastErr = err;
	return FALSE;
    }
    err = RBFreeListError(T);
    if (err != NULL) {
	if (!quiet)
	    buginf("\n%s: %s(%s) TreeValid failed - Free error: %s",
		   T->protoAbbrev,title,T->tableName,err);
	T->lastErr = err;
	return FALSE;
    }
    return TRUE;
}

void
RBTreeStats (rbTree* T, char* name)
{
    int busycnt;
    printf("\n%s Tree Statistics:",name);
    if (T == NULL) {
	printf("\n	Tree is not allocated.");
	return;
    }
    if (!RBTreeValid(T,TRUE,"RBTreeStats")) 
	printf("\n	Tree structure is INVALID! (%s)",T->lastErr);
    else if (T->lastErr != NULL)
	printf("\n	Last tree structure error: %s",T->lastErr);
    printf("\n	Tree contains %d active node%s",
	   ArgAndPlural(T->activeNodes,"","s"));
    if (T->freeNodes != 0) {
	printf(" and %d free node%s",
	       ArgAndPlural(T->freeNodes,"","s"));
	busycnt = T->freeNodes - RBTreeReleasedNodeCount(T);
	if (busycnt != 0)
	    printf(" but %d %s busy",ArgAndPlural(busycnt,"is","are"));
    }
    printf(".\n	Tree has had %d node%s inserted and %d node%s deleted.",
	   ArgAndPlural(T->addedNodes,"","s"),
	   ArgAndPlural(T->deletedNodes,"","s"));
    printf("\n	Tree has had %d node%s returned to the free memory pool.",
	   ArgAndPlural(T->trimmedNodes,"","s"));
    printf("\n	Tree required %d node rotation%s and %d splice%s.",
	   ArgAndPlural(T->rotatedNodes,"","s"),
	   ArgAndPlural(T->splicedNodes,"","s"));
    printf("\n	Tree searches required accessing a maximum of %d node%s.",
	   ArgAndPlural(T->maxNodeDepth,"","s"));
    if ((T->root != NIL()) && (T->root != NULL))
	printf("\n	Subtree depth of root[left] is %d, and root[right] is %d.",
	       T->root->left->size, T->root->right->size);
    printf("\n  Tree search statistics:");
    if (T->findRequests != 0) {
	printf("\n	Keyed searches accessed %d nodes for %d requests"
	       "(%d ave).",
	       T->findAccessed, T->findRequests, 
	       T->findAccessed / T->findRequests);
        printf("\n	Keyed searches accessed a maximum of %d nodes.",
	       T->findDeepest);
    }
    if (T->findIRequests != 0) {
	printf("\n	Interval searches accessed %d nodes for %d "
	       "requests (%d ave).",
	       T->findIAccessed, T->findIRequests, 
	       T->findIAccessed / T->findIRequests);
        printf("\n	Interval searches accessed a maximum %d nodes.",
	       T->findIDeepest);
    }
}

/*
 * RBTreeLexiNode
 * Search for the node equal to or greater than <look>
 */
treeLink*
RBTreeLexiNode (rbTree* T,treeKey look)
{
    register treeLink *walk, *best;
    ulong cnt = 0;

    walk = T->root;
    best = NIL();
    while (walk != NIL()) {
	cnt++;
	if (T->thirtytwobitkey) {
	    if (walk->key.l == look.l) {
		best = walk;
		break;
	    } else if (look.l < walk->key.l) {
		best = walk;		/* our key less than current node */
		walk = walk->left;	/* nodes less than are on the left */
	    } else {
		walk = walk->right;	/* nodes greater than are on the right */
	    }
	} else {
	    if ((*T->keycompare)(walk->key, look) == 0) {
		best = walk;
		break;
	    } else if ((*T->keycompare)(look, walk->key) < 0) {
		best = walk;		/* our key less than current node */
		walk = walk->left;	/* nodes less than are on the left */
	    } else {
		walk = walk->right;	/* nodes greater than are on the right */
	    }
	}
    }
    T->findAccessed += cnt;
    if (cnt > T->findDeepest) 
	T->findDeepest = cnt;
    if (cnt > T->maxNodeDepth)
	T->maxNodeDepth = cnt;

    return RealNode(best);
}

/*
 * RBTreeBestNode
 * Search for the maximal node just less than or equal to <look>
 */
treeLink*
RBTreeBestNode (rbTree* T,treeKey look)
{
    register treeLink *walk, *best;
    ulong cnt = 0;

    walk = T->root;
    best = NIL();
    while (walk != NIL()) {
	cnt++;
	if (T->thirtytwobitkey) {
	    if (walk->key.l == look.l) {
		best = walk;
		break;
	    } else if (look.l < walk->key.l)
		walk = walk->left;	/* nodes less than are on the left */
	    else {
		best = walk;		/* our key exceeds current node */
		walk = walk->right;	/* nodes greater than are on the right */
	    }
	} else {
	    if ((*T->keycompare)(walk->key, look) == 0) {
		best = walk;
		break;
	    } else if ((*T->keycompare)(look, walk->key) < 0)
		walk = walk->left;	/* nodes less than are on the left */
	    else {
		best = walk;		/* our key exceeds current node */
		walk = walk->right;	/* nodes greater than are on the right */
	    }
	}
    }
    T->findAccessed += cnt;
    if (cnt > T->findDeepest) 
	T->findDeepest = cnt;
    if (cnt > T->maxNodeDepth)
	T->maxNodeDepth = cnt;

    return RealNode(best);
}

/*
 * RBTreeNearBestNode
 *
 * Search for a node with the largest key less than or equal to this key
 */
treeLink *
RBTreeNearBestNode (rbTree *T, treeKey key)
{
    register treeLink *x = T->root;
    treeLink *best = NIL();
    ulong cnt = 0;
    
    while (x != NIL()) {
	cnt++;
	if (T->thirtytwobitkey) {
	    if (x->key.l == key.l) {
		best = x;
		break;
	    }
	    if (x->key.l < key.l)
		if ((best == NIL()) || (best->key.l < x->key.l))
		    best = x;
	    if ((x->right != NIL()) && (x->right->key.l <= key.l))
		x = x->right;
	    else
		x = x->left;
	} else {
	    if ((*T->keycompare)(x->key, key) == 0) {
		best = x;
		break;
	    }
	    if ((*T->keycompare)(x->key, key) < 0)
		if ((best == NIL()) || ((*T->keycompare)(best->key, x->key) < 0))
		    best = x;
	    if ((x->right != NIL()) && ((*T->keycompare)(x->right->key, key) <= 0))
		x = x->right;
	    else
		x = x->left;
	}
    }
    T->findAccessed += cnt;
    if (cnt > T->findDeepest)
	T->findDeepest = cnt;
    if (cnt > T->maxNodeDepth) T->maxNodeDepth = cnt;
    return RealNode(best);
}

/*
 * IntRBTreeNearBestNode
 *
 * Search for a node with the largest key less than or equal to this key
 */
treeLink *
RBTreeIntNearBestNode (rbTree *T, treeKey key)
{
    register treeLink *x = T->root;
    treeLink *best = NIL();
    ulong cnt = 0;
    
    while (x != NIL()) {
	cnt++;
	if (x->key.i.low == key.i.low) {
	    best = x;
	    break;
	}
	if (x->key.i.low < key.i.low)
	    if ((best == NIL()) || (best->key.i.low < x->key.i.low))
		best = x;
	if ((x->right != NIL()) && (x->right->key.i.low <= key.i.low))
	    x = x->right;
	else
	    x = x->left;
    }
    T->findAccessed += cnt;
    if (cnt > T->findDeepest)
	T->findDeepest = cnt;
    if (cnt > T->maxNodeDepth) T->maxNodeDepth = cnt;
    return RealNode(best);
}

/*
 * RBTreeSearch - Search for a specific key in the tree
 */
treeLink*
RBTreeSearch (rbTree* T,treeKey key)
{
    register treeLink* x = T->root;
    ulong cnt = 0;
    T->findRequests++;
    while (x != NIL()) {
	cnt++;
	if (T->thirtytwobitkey) {
	    if (x->key.l == key.l) {
		/*
		 * Keys match, so find first so GetNext will iterate
		 */
		if ((x->left != NIL()) && (x->left->key.l == key.l))
		    x = SubRBTreeMaximum(x->left);
		else if ((x->right != NIL()) && (x->right->key.l == key.l)) 
		    x = SubRBTreeMinimum(x->right);
		break;
	    }
	    if (key.l < x->key.l)
		x = x->left;
	    else
		x = x->right;
	} else {
	    if ((*T->keycompare)(x->key, key) == 0) {
		/*
		 * Keys match, so find first so GetNext will iterate
		 */
		if ((x->left != NIL()) && 
		    ((*T->keycompare)(x->left->key, key) == 0))
		    x = SubRBTreeMaximum(x->left);
		else if ((x->right != NIL()) && 
			 ((*T->keycompare)(x->right->key, key) == 0))
		    x = SubRBTreeMinimum(x->right);
		break;
	    }
	    if ((*T->keycompare)(key, x->key) < 0)
		x = x->left;
	    else
		x = x->right;
	}
    }
    T->findAccessed += cnt;
    if (cnt > T->findDeepest) 
	T->findDeepest = cnt;
    if (cnt > T->maxNodeDepth) T->maxNodeDepth = cnt;
    return RealNode(x);
}

/*
 * RBTreeIntSearch - Search for an overlapping interval in a tree
 */
treeLink*
RBTreeIntSearch (rbTree* T,treeKey key)
{
    register treeLink* x = T->root;
    ulong cnt = 0;
    T->findIRequests++;
    while (x != NIL()) {
	cnt++;
	if (IntervalsOverlap(x->key.i,key.i)) break;
	if (!IntervalsOverlap(x->subkey.i,key.i)) return NULL; 
	if ((x->left != NIL()) && (x->left->subkey.i.high >= key.i.low))
	    x = x->left;
	else
	    x = x->right;
    }
    T->findIAccessed += cnt;
    if (cnt > T->findIDeepest) 
	T->findIDeepest = cnt;
    if (cnt > T->maxNodeDepth) T->maxNodeDepth = cnt;
    return RealNode(x);
}

STATIC void
RBTreeLeftRotate (rbTree* T, treeLink* x)
{
    treeLink* y = x->right;
#ifndef TREEDEBUG
    if (*T->debug_flag)
#endif
	TREEASSERT(y != NIL());
    x->right = y->left;
    if (y->left != NIL())
	y->left->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL()) {
	T->root = y;
    } else {
	if (x == x->parent->left)
	    x->parent->left = y;
	else
	    x->parent->right = y;
    }
    y->left = x;
    x->parent = y;
    UpdateNodeInfo(T, x);
    UpdateNodeInfo(T, y);
    T->rotatedNodes++;
}

STATIC void
RBTreeRightRotate (rbTree* T, treeLink* x)
{
    treeLink* y = x->left;
#ifndef TREEDEBUG
    if (*T->debug_flag)
#endif
	TREEASSERT(y != NIL());
    x->left = y->right;
    if (y->right != NIL())
	y->right->parent = x;
    y->parent = x->parent;
    if (x->parent == NIL()) {
	T->root = y;
    } else {
	if (x == x->parent->right)
	    x->parent->right = y;
	else
	    x->parent->left = y;
    }
    y->right = x;
    x->parent = y;
    UpdateNodeInfo(T, x);
    UpdateNodeInfo(T, y);
    T->rotatedNodes++;
}

/*
 * RBTreeInsert - Insert a node into the tree
 */
treeLink*
RBTreeInsert (treeKey key, rbTree* T, treeLink* node)
{
    treeLink* y = NIL();
    treeLink* z = node;
    treeLink* x = T->root;

    T->intervalkey = FALSE;
    z->key = key;
    z->subkey = z->key;
    z->size = 1;
    z->busy = 0;
    z->left = NIL();
    z->right = NIL();
    z->freeList = NIL();

    while (x != NIL()) {
	y = x;
	x->size++;
	if (T->thirtytwobitkey) {
	    if (z->key.l > x->subkey.l)
		x->subkey.l = z->key.l;
	    if (z->key.l < x->key.l)
		x = x->left;
	    else
		x = x->right;
	} else {
	    if ((*T->keycompare)(z->key, x->subkey) > 0)
		x->subkey = z->key;
	    if ((*T->keycompare)(z->key, x->key) < 0)
		x = x->left;
	    else
		x = x->right;
	}
    }
    if (y == z) {
	errmsg(SYS_ERR_TABLEERR,T->protoName,T->tableName,"node added twice?");
	return z;
    }

    z->parent = y;

    T->activeNodes++;
    T->addedNodes++;

    if (y == NIL()) {
	T->root = z;
	z->nextl = z->prevl = NULL;
    } else if ((*T->keycompare)(z->key, y->key) < 0) {
	y->left = z;			/* z becomes y's left child */
	/* now, link the node in */
	LinkLeftChild(y, z);
    } else {
	y->right = z;			/* z becomes y's right child */
	/* now, link the node in */
	LinkRightChild(y, z);
    }
    x = z;			/* Switch to x to match algorithm */
    x->color = RedNode;
    while ((x != T->root) && (x->parent->color == RedNode)) {
	if (x->parent == (x->parent)->parent->left) {
	    y = x->parent->parent->right;
	    if (y->color == RedNode) {
		x->parent->color = BlackNode;
 		y->color = BlackNode;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(x->parent->parent != NIL());
		x->parent->parent->color = RedNode;
		x = x->parent->parent;
	    } else {
		if (x == x->parent->right) {
		    x = x->parent;
		    RBTreeLeftRotate(T,x);
		}
		x->parent->color = BlackNode;
		x->parent->parent->color = RedNode;
		RBTreeRightRotate(T,x->parent->parent);
	    }
	} else {
	    y = x->parent->parent->left;
	    if (y->color == RedNode) {
		x->parent->color = BlackNode;
		y->color = BlackNode;
		x->parent->parent->color = RedNode;
 		x = x->parent->parent;
	    } else {
		if (x == x->parent->left) {
		    x = x->parent;
		    RBTreeRightRotate(T,x);
		}
		x->parent->color = BlackNode;
 		x->parent->parent->color = RedNode;
		RBTreeLeftRotate(T,x->parent->parent);
 	    }
	}
    }
    T->root->color = BlackNode;
#ifndef TREEDEBUG	     
    if (*T->debug_flag)
#endif
	RBTreeValid(T,FALSE,"RBTreeInsert");
    return node;
}    
  

/*
 * RBTreeIntInsert - Insert a node into the tree
 */
treeLink*
RBTreeIntInsert (ushort low, ushort high, rbTree* T, treeLink* node)
{
    treeLink* y = NIL();
    treeLink* z = node;
    treeLink* x = T->root;

#ifndef TREEDEBUG	     
    if (*T->debug_flag)
#endif
	RBTreeValid(T,FALSE,"start of RBTreeIntInsert");
    T->intervalkey = TRUE;
    z->key.i.low = low;
    z->key.i.high = ((high == 0) ? low : high);
    z->subkey = z->key;
    z->size = 1;
    z->busy = 0;
    z->left = NIL();
    z->right = NIL();
    z->parent = NIL();
    z->freeList = NIL();

    while (x != NIL()) {
	y = x;
	x->size++;
	if (z->key.i.low < x->subkey.i.low)
	    x->subkey.i.low = z->key.i.low;
	if (z->key.i.high > x->subkey.i.high)
	    x->subkey.i.high = z->key.i.high;
	if (z->key.i.low < x->key.i.low)
	    x = x->left;
	else
	    x = x->right;
    }
    if (y == z) {
	errmsg(SYS_ERR_TABLEERR,T->protoName,T->tableName,"node added twice?");
	return z;
    }
	    
    z->parent = y;

    T->activeNodes++;
    T->addedNodes++;

    if (y == NIL())
	T->root = z;
    else if (z->key.i.low < y->key.i.low) {
	y->left = z;
	LinkLeftChild(y, z);
    } else {
	y->right = z;
	LinkRightChild(y, z);
    }
    x = z;			/* Switch to x to match algorithm */
    x->color = RedNode;
    while ((x != T->root) && (x->parent->color == RedNode)) {
	if (x->parent == (x->parent)->parent->left) {
	    y = x->parent->parent->right;
	    if (y->color == RedNode) {
		x->parent->color = BlackNode;
 		y->color = BlackNode;
		x->parent->parent->color = RedNode;
		x = x->parent->parent;
	    } else {
		if (x == x->parent->right) {
		    x = x->parent;
		    RBTreeLeftRotate(T,x);
		}
		x->parent->color = BlackNode;
		x->parent->parent->color = RedNode;
		RBTreeRightRotate(T,x->parent->parent);
	    }
	} else {
	    y = x->parent->parent->left;
	    if (y->color == RedNode) {
		x->parent->color = BlackNode;
		y->color = BlackNode;
		x->parent->parent->color = RedNode;
 		x = x->parent->parent;
	    } else {
		if (x == x->parent->left) {
		    x = x->parent;
		    RBTreeRightRotate(T,x);
		}
		x->parent->color = BlackNode;
 		x->parent->parent->color = RedNode;
		RBTreeLeftRotate(T,x->parent->parent);
 	    }
	}
    }
    T->root->color = BlackNode;
#ifndef TREEDEBUG	     
    if (*T->debug_flag)
#endif
	RBTreeValid(T,FALSE,"end of RBTreeIntInsert");
    return node;
}    
  

STATIC void
RBTreeDeleteFixup (rbTree* T,treeLink* x)
{
    treeLink* w;

    while ((x != T->root) && (x->color == BlackNode)) {
	if (x == x->parent->left) {
	    w = x->parent->right;
	    if (w->color == RedNode) { /* Case 1 */
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w != NIL());
		w->color = BlackNode;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(x->parent != NIL());
		x->parent->color = RedNode;
		RBTreeLeftRotate(T,x->parent);
		w = x->parent->right;

/*
 * For some reason, a valid tree causes this ASSERT to go off. For
 * now (911030) this is commented out.
 */
#if 0
		TREEASSERT(w!=NIL())
#endif
	    }
	    if (w == NIL()) {
		x = x->parent;
	    } else if ((w->left->color == BlackNode) && 
		(w->right->color == BlackNode)) { /* case 2b */
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w != NIL());
		w->color = RedNode;
		x = x->parent;
	    } else {
		if (w->right->color == BlackNode) { /* case 3 */
		    w->left->color = BlackNode;
#ifndef TREEDEBUG
		    if (*T->debug_flag)
#endif
			TREEASSERT(w != NIL());
		    w->color = RedNode;
		    RBTreeRightRotate(T,w);
		    w = x->parent->right;
#ifndef TREEDEBUG
		    if (*T->debug_flag)
#endif
			TREEASSERT(w != NIL());
		}
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w != NIL()); 			/* case 4 */
		w->color = x->parent->color;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(x->parent != NIL());
		x->parent->color = BlackNode;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w->right != NIL());
		w->right->color = BlackNode;
		RBTreeLeftRotate(T,x->parent);
		x = T->root;
	    }
	} else {
	    w = x->parent->left;
	    if (w->color == RedNode) {
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w != NIL());
		w->color = BlackNode;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(x->parent != NIL());
		x->parent->color = RedNode;
		RBTreeRightRotate(T,x->parent);
		w = x->parent->left;

/*
 * For some reason, a valid tree causes this ASSERT to go off. For
 * now (911030) this is commented out.
 */
#if 0
		TREEASSERT(w != NIL());
#endif
		/*
		 * We have fixed the tree, do case 2a or 2b and quit
		 */
	    }
	    if (w == NIL()) {
		/*
		 * Case 2a, siblings are both black but don't change nil
		 */
		x = x->parent;
	    } else if ((w->right->color == BlackNode) && 
		(w->left->color == BlackNode)) {
		/*
		 * Case 2b, delete a black node from subtree w
		 */
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w != NIL());
		w->color = RedNode;
		x = x->parent;
	    } else {
		if (w->left->color == BlackNode) { /* Case 3 */
		    w->right->color = BlackNode;
#ifndef TREEDEBUG
		    if (*T->debug_flag)
#endif
			TREEASSERT(w != NIL());
		    w->color = RedNode;
		    RBTreeLeftRotate(T,w);
		    w = x->parent->left;
#ifndef TREEDEBUG
		    if (*T->debug_flag)
#endif
			TREEASSERT(w != NIL());
		}
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w != NIL()); 		  /* Case 4 */
		w->color = x->parent->color;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(x->parent != NIL());
		x->parent->color = BlackNode;
#ifndef TREEDEBUG
		if (*T->debug_flag)
#endif
		    TREEASSERT(w->left != NIL());
		w->left->color = BlackNode;
		RBTreeRightRotate(T,x->parent);
		x = T->root;
	    }
	}
    }
    UpdateNodeInfo(T, x);
    x->color = BlackNode;
}

/*
 * RBTreeDelete - Delete a node from the tree
 */
treeLink*
RBTreeDelete (rbTree* T,treeLink* z)
{
    treeLink *y, *x, *oldp;
    nodeColor oldcolor;

#ifndef TREEDEBUG	     
    if (*T->debug_flag)
#endif
	RBTreeValid(T,FALSE,"pre-RBTreeDelete");
#ifndef TREEDEBUG
    if (*T->debug_flag)
#endif
	TREEASSERT(z != NIL());

#if 0
    /* If the node smells like an already deleted node, squawk & return. */
    if ((z->parent == NIL()) && (z->left == NIL()) && (z->right == NIL())) {
	if (*T->debug_flag)
	    buginf("\n%s: attempt to delete RB node %08x twice", T->protoAbbrev, z);
	return (NULL);
    }
#endif
    UnlinkNode(z);			/* unlink from linked list */

    if ((z->left == NIL()) || (z->right == NIL()))
	y = z;
    else
	y = SubRBTreeSuccessor(z);
#ifndef TREEDEBUG
    if (*T->debug_flag)
#endif
	TREEASSERT(y != NIL());
    if (y->left != NIL())
	x = y->left;
    else
	x = y->right;
    /*
     * Special, we are deleting the successors parent
     */
    if (y->parent == z) {
	oldcolor = y->color;
	y->parent = z->parent;	/* Change parent */
	/*
	 * If y is left, copy the right pointer, clear left
	 */
	if (z->left == y) {
#ifndef TREEDEBUG
	    if (*T->debug_flag)
#endif
		TREEASSERT(y->right == NIL());
	    y->right = z->right;
	    if (z->right != NIL()) z->right->parent = y;
	} else {
	    /*
	     * y is right, copy left and clear right
	     */
#ifndef TREEDEBUG
	    if (*T->debug_flag)
#endif
		TREEASSERT(y->left == NIL());
	    y->left = z->left;
	    if (z->left != NIL()) z->left->parent = y;
	}
	if (z->parent == NIL())		/* Now, fixup the parent */
	    T->root = y;
	else 
	    if (y->parent->left == z)
		y->parent->left = y;
	    else
		y->parent->right = y;
	z->parent = y;
	y->color = z->color;
	z->color = oldcolor;
	UpdateNodeInfo(T, y);
	UpdateNodeInfo(T, y->parent);
	x->parent = y;
	y = z;
    } else {
	x->parent = y->parent;
	if (y->parent == NIL())
	    T->root = x;
	else {
	    if (y == y->parent->left)
		y->parent->left = x;
	    else y->parent->right = x;
	    UpdateNodeInfo(T, y->parent);
	}
    }
    /*
     * At this point, y has been unlinked from the tree and x points
     * at the next node below y or NIL().  If the original node to be
     * deleted is different from y, swap z and y in the tree.  There
     * are not any special cases here because we took care of it already.
     * (Otherwise we must undo/modify what was done above...)
     */
    if (y != z) {
	T->splicedNodes++;
#ifndef TREEDEBUG
	if (*T->debug_flag)
#endif
	    TREEASSERT((y != NIL())&&(z != NIL()));
	y->left = z->left;
	if (y->left != NIL()) y->left->parent = y;
	y->right = z->right;
	if (y->right != NIL()) y->right->parent = y;
	oldp = y->parent;		/* Save the old parent */
	/*
	 * Cannot happen since y is a non-nil successor of z
	 */
#ifndef TREEDEBUG
	if (*T->debug_flag)
#endif
	    TREEASSERT(z->parent != y); 
	  /*
	   * The assert looks out for an infinite parent loop here.
	   */
	y->parent = z->parent;
	/*
	 * Restore the parent of the deleted node, for cleanup
	 */
	z->parent = oldp;
	if (y->parent == NIL())
	    T->root = y;
	else {
	    if (y->parent->left == z)
		y->parent->left = y;
	    else
		y->parent->right = y;
	}
#ifndef TREEDEBUG
	if (*T->debug_flag) {
#endif
	    TREEASSERT(y != NIL());
	    TREEASSERT(z != NIL());
#ifndef TREEDEBUG
	}
#endif
	oldcolor = y->color;
	y->color = z->color;
	z->color = oldcolor;
	UpdateNodeInfo(T, y);
	UpdateNodeInfo(T, y->parent);
	y = z;			
    }
    /*
     * At this point, y contains the node which is being deleted and s/b z.
     * x is a node in the subtree of z or nil and x->parent is the first
     * node which needs updating due to the delete (either z or the
     * successor of z).  
     * WARNING!!  nil->parent is not guarrenteed to be nil()!!
     */
    if (T->root != NIL()) {
	z = y->parent;		/* Save the first node to update pointer */
#ifndef TREEDEBUG
	if (*T->debug_flag)
#endif
	    TREEASSERT((x != y) && (x->parent != y));
	UpdateNodeInfo(T, x);
	/* If the black node height changed, rebalance */
	if (y->color == BlackNode) {
	    RBTreeDeleteFixup(T,x);
        }
	if (x == NIL())
	    x->parent = NIL();
	while (z != NIL()) {
	    UpdateNodeInfo(T, z);
	    z = z->parent;
	}
    } else {
      if (x == NIL())
	x->parent = NIL();
    }
    T->activeNodes--;
    T->deletedNodes++;
    y->parent = NIL();
    y->right = NIL();
    y->left = NIL();
    y->size = 0;
    y->subkey.ll = 0;
    y->freeList = T->freeList;
    T->freeList = y;
    T->freeNodes++;
#ifndef TREEDEBUG	     
    if (*T->debug_flag)
#endif
	RBTreeValid(T,FALSE,"RBTreeDelete");
    return RealNode(y);
}

boolean
RBTreeForEachNode (treeProc proc, void* pdata, rbTree* T, boolean protectIt)
{
    treeLink* x;
    treeLink* nxt;

    if ((T == NULL) || (T->root == NIL()))
	return(TRUE);
    x = SubRBTreeMinimum(T->root);
    if (x != NULL && protectIt)
	RBTreeNodeProtect(x, TRUE);
    while (x != NULL) {
	nxt = SubRBTreeSuccessor(x);
	if (nxt != NULL && protectIt)
	    RBTreeNodeProtect(nxt, TRUE);
	(*proc)(x,pdata);
	if (protectIt)
	    RBTreeNodeProtect(x, FALSE);
	x = nxt;
    }
    return(TRUE);
}

boolean
RBTreeForEachNodeTilFalse (treeProc proc, void* pdata, rbTree* T,
			   treeLink* start, boolean protectIt)
{
    treeLink* x;
    treeLink* nxt;
    boolean result;

    if ((T == NULL) || (T->root == NIL()))
	return(TRUE);
    if (start) {
	x = start;
    } else {
	x = SubRBTreeMinimum(T->root);
    }
    if (x != NULL && protectIt)
	RBTreeNodeProtect(x, TRUE);
    while (x != NULL) {
	nxt = SubRBTreeSuccessor(x);
	if (nxt != NULL && protectIt)
	    RBTreeNodeProtect(nxt, TRUE);
	result = (*proc)(x,pdata);
	if (!result) {
	    if (protectIt) {
		RBTreeNodeProtect(x, FALSE);
		RBTreeNodeProtect(nxt, FALSE);
	    }
	    return(FALSE);
	}
	if (protectIt)
	    RBTreeNodeProtect(x, FALSE);
	x = nxt;
    }
    return(TRUE);
}

#ifdef NO_TREE_INLINES

boolean
RBTreeNodeProtect (treeLink* x,boolean lockIt)
{
    if (lockIt)
	x->busy++;
    else {
#ifndef TREEDEBUG
	if (*T->debug_flag)
#endif
	    TREEASSERT(x->busy > 0);
	
	if (x->busy > 0)
	    x->busy--;
    }
    if (x->busy == 0)
	return FALSE;
    return TRUE;
}

#endif

boolean
RBTreeNodeProtected (treeLink* x)
{
    if (x->busy > 0)
	return TRUE;
    return FALSE;
}

treeLink*
RBTreeFirstNode (rbTree* T)
{
    if ((T == NULL) || (T->root == NIL()))
	return NULL;
    return SubRBTreeMinimum(T->root);
}
treeLink *
RBTreeNextNode (treeLink *x)
{
    if ((x == NULL) || (x == NIL()))
	return NULL;
    if (x->nextl == NULL)		/* no successor */
	return (NULL);	/* we could return NULL here */
#ifdef TREEDEBUG
    if (x->nextl != SubRBTreeSuccessor(x))
	buginf("\nDifference in RBTreeNextNode");
#endif
    return (x->nextl);
}

/*
 * RBTreeGetFreeNode
 *
 * Returns the first non-busy node from the specified tree's freelist
 * after unlinking it from the single-linked list.
 */
treeLink*
RBTreeGetFreeNode (rbTree* T)
{
    treeLink *y, *x;
    if ((T == NULL) || (T->freeList == NIL())) 
	return NULL;
    y = NIL();
    x = T->freeList;
    while ((x != NIL()) && (x->busy > 0)) {
	y = x;
	x = x->freeList;
    }
    if (x != NIL()) {
	if (y == NIL())
	    T->freeList = x->freeList;
	else
	    y->freeList = x->freeList;
	x->freeList = NIL();
	T->freeNodes--;
    }
    return (RealNode(x));
}

/*
 * RBScavengeFreeNode
 *
 * Returns the specified node from the specified tree's freelist
 * after unlinking it from the single-linked list. The busy/non-busy
 * state of the returned node is the caller's problem.
 */
treeLink*
RBTreeScavengeFreeNode (rbTree* T, treeLink *node)
{
    treeLink *y, *x;
    if ((T == NULL) || (T->freeList == NIL())) 
	return (NULL);
    y = NIL();
    x = T->freeList;
    while ((x != NIL()) && (x != node)) {
	y = x;
	x = x->freeList;
    }
    if (x != NIL()) {
	if (y == NIL())
	    T->freeList = x->freeList;
	else
	    y->freeList = x->freeList;
	x->freeList = NIL();
	T->freeNodes--;
    }
    return (RealNode(x));
}

/*
 * RBAddToFreeList
 *
 * Add a RBTree's node to the freelist.
 * The node must not be linked into the tree and further, the
 * linkages must appear as they would after a RBTreeDelete() call
 * has completed.
 */
boolean
RBTreeAddToFreeList (rbTree* T, treeLink* node)
{
    if (!(T && node))
	return (FALSE);

    if (!(node->parent == NIL() && node->left == NIL() && node->right == NIL()))
	return (FALSE);
    node->freeList = T->freeList;
    T->freeList = node;
    T->freeNodes++;
    return (TRUE);
}

boolean
RBTreeTrimFreeList (rbTree* T)
{
    treeLink *y, *x;
    if ((T == NULL) || (T->freeList == NIL())) 
	return FALSE;
    y = NIL();
    x = T->freeList;
    while (x != NIL()) {
	if (x->busy) {
	    y = x;
	    x = x->freeList;
	    continue;
	}
	T->trimmedNodes++;
	T->freeNodes--;
	if (y != NIL()) {
	    y->freeList = x->freeList;
	    free(x);
	    x = y->freeList;
	} else {
	    T->freeList = x->freeList;
	    free(x);
	    x = T->freeList;
	}
    }
    return (!T->freeList);
}

/*
 * RBTreeReleasedNodeCount - Returns number of free nodes which are not busy
 */
int
RBTreeReleasedNodeCount (rbTree* T)
{
    int cnt = 0;
    treeLink* x;

    if ((T == NULL) || (T->freeList == NIL())) 
	return (FALSE);
    x = T->freeList;
    while (x != NIL()) {
	if (x->busy <= 0) cnt++;
	x = x->freeList;
    }
    return (cnt);
}

/*
 * RBTreeFreeNodeCount - Returns the number of free nodes 
 */
int
RBTreeFreeNodeCount (rbTree* T)
{
    return (T->freeNodes);
}

/*
 * TreeNodeDeleted - Check if node has been deleted
 * Don't do anything silly during this call, like dismiss.
 */
boolean
RBTreeNodeDeleted (rbTree* head, treeLink* l)
{
    treeLink	*tl;
    ushort	deleted_nodes = head->freeNodes;

    tl = head->freeList;
    while (deleted_nodes > 0 && (tl != NIL() && tl != l)) {
	tl = tl->freeList;
	--deleted_nodes;
    }
    return (tl == l);
}

/*
 *
 */
void
RBTreePrintKeyLong (char *text, treeKey key)
{
    sprintf(text, "#%d, %08x", key.l, key.l);
}

void
RBTreePrintKeyLongLong (char *text, treeKey key)
{
    sprintf(text, "#%ld, %16x", key.ll, key.ll);
}

/*
 * PrintTreeNode - Actually print info about a node. The 'more' mechanism
 * 		   in TreePrint() assumes that this routine prints only
 *		   one line of information. See TreePrint, where the
 *		   display_done() routine is called.
 */
static void
RBPrintTreeNode (treeLink* x, ulong depth, treeKeyPrint fn)
{
    char text[20];
				/* The busy check must remember we busy it */
    if (fn) {
	(fn)(text, x->key);
	printf("\n%*ckey,(%s) %s, %s%d ref%s",(depth*4),' ', text,
	       ((x->color == RedNode)?"Red":"Black"),
	       ((x->freeList != NIL())?"deleted, ":""),
	       x->busy, (x->busy!=1)?"s":"");
    }
}

/*
 * TreePrint - Print each node in tree rooted at x, for debugging
 */
void
RBTreePrint (treeLink* x, ulong depth, rbTree* head)
{
    static ulong 	mindepth,maxdepth,nodecnt,redcnt,blkcnt;
    static treeKey	lastkey;
    char text1[20], text2[20];

    if (depth == 0) {
	if (head->print_key) {
	    (*head->print_key)(text1, head->root->key);
	    printf("\nInternal Tree Structure:		(Root [%s])", text1);
	} else {
	    printf("\nInternal Tree Structure: don't know how to decode key");
	}
	mindepth = 0;
	maxdepth = 0;
	redcnt = 0;
	blkcnt = 0;
	nodecnt = 0;
	lastkey.l = 0;
	RBTreePrint(x, depth+1, head);
	printf("\n  Tree summary: %3d nodes, max depth %d , min depth %d",
	       nodecnt, maxdepth, mindepth);
	printf("\n                %3d red nodes, %3d black nodes",
	       redcnt, blkcnt);
	return;
    } else if (x != NIL()) {
	x->busy++;		/* Protect this node */
	if (RBTreeNodeDeleted(head, x)) {
	    (*head->print_key)(text1, head->root->key);
	    printf("\n%s: Tree FAULT! deleted node found=%s",
		   head->protoAbbrev,text1);	    
	}
	if ((*head->keycompare)(x->key, lastkey) <= 0) {
	    (*head->print_key)(text1, x->key);
	    printf("\n%s: Tree FAULT! invalid key found node=%s",
		   head->protoAbbrev,text1);
	}
	if (x->color == RedNode) {
	    if (x->left->color == RedNode) {
		(*head->print_key)(text1, x->key);
		(*head->print_key)(text2, x->left->key);
		printf("\n%s: Tree FAULT! depth=%d,node=%s node->l=%s",
		       head->protoAbbrev,depth,text1,text2);
	    }
	    if (x->right->color == RedNode) {
		(*head->print_key)(text1, x->key);
		(*head->print_key)(text2, x->right->key);
		printf("\n%s: Tree FAULT! depth=%d,node=%d node->r=%d",
		       head->protoAbbrev,depth,text1,text2);
	    }
	}
	if ((x->right == x->left) && (x->right == NIL())) {
	    if ((depth < mindepth) || (mindepth == 0))
		mindepth = depth;
	    if (depth > maxdepth)
		maxdepth = depth;
	}
	RBTreePrint(x->left,depth+1,head);/* Handle left chain */

	if (head == (rbTree *)x)
	    printf("\n%s: Tree FAULT! Head node found within tree",
		   head->protoAbbrev);
	lastkey = x->key;
	if (x->color == RedNode)
	    redcnt++;
	else
	    blkcnt++;
	nodecnt++;
	RBPrintTreeNode(x,depth,head->print_key);	/* Print each node */
	if (!validmem(x)) {
	    printf("\n%% Lost data synchronization - reenter command");
	    return;
	}
	RBTreePrint(x->right,depth+1,head);/* ...handle right chain */
	x->busy--;			/* Unprotect this node */
    }
}
