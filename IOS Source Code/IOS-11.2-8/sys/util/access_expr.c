/* $Id: access_expr.c,v 3.4.20.3 1996/06/28 23:31:29 hampton Exp $
 * $Source: /release/112/cvs/Xsys/util/access_expr.c,v $
 *------------------------------------------------------------------
 * access_expr.c -- Boolean Expression Analyzer
 * 
 * October 4, 1991 - Wayne Clark, from expression analyzer
 * by Karyn Weinstein
 *
 * Copyright (c) 1991-1996 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: access_expr.c,v $
 * Revision 3.4.20.3  1996/06/28  23:31:29  hampton
 * Remove gratuitous casting.  [CSCdi61655]
 * Branch: California_branch
 *
 * Revision 3.4.20.2  1996/05/17  12:15:31  ppearce
 * Merge IbuMod into Calif
 *
 * Revision 3.4.6.2  1996/04/03  22:08:19  ppearce
 * Sync to IbuMod_Calif_baseline_960402
 *
 * Revision 3.4.6.1  1996/03/16  12:10:14  ppearce
 * CSCdi50498:  IBU modularity - Implement NetBios subblock
 * Branch: IbuMod_Calif_branch
 *   Move NetBios fields from hwidbtype/idbtype structs to NetBios subblock
 *
 * Revision 3.4.20.1  1996/03/18  22:30:35  gstovall
 * Branch: California_branch
 * Elvis has left the building.  He headed out to California, and took the
 * port ready changes with him.
 *
 * Revision 3.3.26.3  1996/03/13  20:50:24  dstine
 * Branch: DeadKingOnAThrone_branch
 *         - commit IDB cleanups; changes from reviews
 *
 * Revision 3.3.26.2  1996/03/07  10:59:40  mdb
 * Branch: DeadKingOnAThrone_branch
 * cisco and ANSI/POSIX libraries.
 *
 * Revision 3.3.26.1  1996/02/20  21:44:46  dstine
 * Branch: DeadKingOnAThrone_branch
 *           Sync from DeadKingOnAThrone_baseline_960122 to
 *                     DeadKingOnAThrone_baseline_960213
 *
 * Revision 3.4  1996/02/06  03:03:21  szdravko
 * CSCdi37685:  Access-expression parsing is incosistent
 *
 * Revision 3.3  1995/11/17  17:56:02  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.2  1995/11/17  00:46:57  gstovall
 * Ladies and gentlemen, I introduce to you, the port ready commit.
 *
 * Revision 3.1  1995/11/09  13:42:58  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  23:12:41  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#define	DEBUG_EXPR	1
/******************************************************************************
*  access_expr.c
*
*  File Description:
*	This file contains the boolean expression capability for access-lists.
*
*  Functions in this file:
*    Interface functions to the router code -
*	access_expression_command()
*	    entry point into Expression analysis from the cisco EXEC
*	    Called from parse_config_text().
*	expr_evaluate_tree()
*	    evaluate the value of the Expression relative to the current
*	    datagram by processing the ExpressionTree.
*	    Called from tr_in_check() and tr_out_check().
*	show_access_expression()
*	    display the Expression as derived from the ExpressionTree.
*	    Called from show_command().
*	nv_access_expression()
*	    write the Expression represented by ExpressionTree to NV RAM.
*	    Called by nv_interface().
*	access_expr_debug_work()
*	    define access-expression debug setting.
*	    Called from debug_command().
*	show_access_expr_debug()
*	    display the current access-expression debug setting.
*	    Called from show_debug().
*	expr_update_netbios_access_list()
*	    scan all ExpressionTrees on all interfaces to either add or
*	    delete the specified NetBIOS access list from them.
*	    Called from netbios_acl().
*
*    Internal functions -
*	expr_analyze()
*	    analyze the Expression and create an ExpressionTree
*	expr_get_token()
*	    get the next Token from Expression
*	expr_reduce_stack()
*	    perform a reduction on the top of ExpressionStack
*	expr_compute_depths()
*	    compute the depths of left and right subtree for each node in
*	    ExpressionTree
*	expr_determine_symbol_value()
*	    make the appropriate call to the router system code (i.e. other
*	    native access-list definitions to determine the value of the
*	    Symbol
*	expr_free_expression_tree()
*	    free all ExpressionTree entries on the ExpressionStack
*	expr_allocate_node()
*	    allocate and initialize a node for the ExpressionTree
*	expr_free_node()
*	    free a node from the ExpressionTree
*	expr_lookup_category()
*	    look up the keyword from the Expression to locate the category
*	expr_show_expression_tree()
*	    display a node in the ExpressionTree
*	expr_update_expression_tree()
*	    update the ExpressionTree of all references to a specified NetBIOS
*	    access list.
*
*    Utilities -
*	expr_get_identifier()
*	    extract an identifier from the Expression
*	isidentifier()
*	    determine if the specified character meets the requirements
*	    for an identifier
*	expr_dump_expression_tree()
*	    For debugging purposes only.  Dump the contents of the
*	    ExpressionTree
*
*  Theory of Operation:
*	Boolean expression analysis is a two step process.  First, the
*	Expression is analyzed (via expr_analyze()) for semantic and
*	syntactic correctness.  In the process of analyzing the
*	Expression, expr_analyze() manages the ExpressionStack.  Along
*	with tokens and values, the ExpressionStack contains pointers to
*	nodes that are being used to construct the ExpressionTree. The end
*	result of analyzing the Expression is a fully-formed ExpressionTree.
*
*	The second step involves processing the ExpressionTree against the
*	packet of data to see if it meets the selection criteria.
*
*	Step 1 is done at process-level via the "access-expression" command
*	while step 2 is done at interrupt-level.  Since ExpressionTree is
*	a binary tree, it is tempting to process the tree recursively. The
*	rules that governed the devlopment of this facility is that recursion
*	is okay for process-level (i.e. in Expression analysis) but is 
*	forbidden at interrupt-level (i.e. in ExpressionTree processing).
*	The overhead of recursive function calls at interrupt-level are too
*	costly.
*
*  Vocabulary:
*    Expression
*	the set of Tokens that can be reduced by the Floyd-Evans method
*	of expression reduction to a single Value.
*
*    Token
*	a single element of the Expression that has significance to
*	expr_analyze().  Tokens are extracted from the Expression through
*	repetitive calls to expr_get_token.
*	    Token ::=  Symbol | Operator | '(' | ')' | end-of-line
*
*    Symbol
*	a Token that has represents a variable in the Expression.  
*	For arithmetic expressions, Symbol defines an arithmetic value.
*	    Symbol ::= Constant | Variable
*		where: Constant        ::= DecimalConstant | HexConstant
*		       DecimalConstant ::= {0-9}
*		       HexConstant     ::= "0x"{0-F}
*	For boolean expressions, Symbol defines a boolean value.
*	    Symbol ::= "T" | "F"
*
*    Value
*	the resultant value of the Expression.  For a boolean Expression,
*	this will be either "true" or "false" and for an arithmetic
*	Expression this will be an integer value.
*
*    Operator
*	any of the arithmetic or boolean operators permissible in arithmetic
*	or boolean expressions.
*	    Operator           :: = ArithmeticOperator | BooleanOperator
*	        where: ArithmeticOperator ::= '+' | '-' | '*' | '/'
*	    	       BooleanOperator    ::= '&' | '|' | '~'
*
*	All Operators are either binary operators or unary operators.
*	Binary operators are '+' | '-' | '*' | '/' for arithmetic and
*	'&' | '|' for boolean.  Unary operators are '+' | '-' for arithmetic
*	and '~' for boolean.
*
*    ExpressionStack
*	the stack of ExpressionStackEntries that is manipulated by
*	expr_analyze() to reduce the Expression into a Value.  ExpressionStack
*	is maintained in the global data structure, expr_stack.
*
*    CurrentState
*	a variable that is maintained by expr_analyze() which is used to
*	govern the sequence of execution of states within expr_analyze().
*	This sequence of execution of expr_analyze() states is the 
*	controlling factor in reduction of the ExpressionStack into a 
*	single Value.
*
*    StackToken
*	a token that exists in the ExpressionStack. 
*	    StackToken ::= Operator | 'E' | 'T' | 'P' | 'I'
*
*    StackValue
*	a value that exists in the ExpressionStack.  Until expr_analyze()
*	reaches the end of its reduction of Expression, all StackValues are
*	intermediate values.  Note: Since this implementation of the 
*	Expression analyzer is essential an "expression compiler", there
*	are no StackValues, only nodes for the ExpressionTree.
*
*    ExpressionTree
*	the binary tree that represents the Expression that is processed at
*	interrupt level.  The root of ExpressionTree is anchored off the
*	interface definition block, idb.
*
*****************************************************************************/

/*
**   I n c l u d e   F i l e s
*/
/*  System  */
#include	"master.h"
#include	<ctype.h>
#include	<ciscolib.h>
#include	"interface_private.h"
#include	"packet.h"
#include	"config.h"
#include	"access.h"
#include        "sys_registry.h"

#include	"../if/tring.h"
#include	"../ibm/netbios.h"
#include	"../ibm/netbios_sb.h"

/*  Application  */
#include	"access_expr.h"

/*
**   C o n s t a n t   D e f i n i t i o n s
*/

/*
**   L o c a l   M a c r o s
*/


/*
**  The following macro is merely an assist for a commonly-occurring sequence
**  of instructions found in expr_analyze().  Even though I abhor the
**  use of macros like this (since they conceal the usage of variables), I 
**  nonetheless decided to use it to reduce the code size of expr_analyze().
*/
#define	GETTOKEN	\
    top--; \
    expr_rc = expr_get_token(expression, &expr_expression_index, \
	       expr_stack, top);\
    stack_count++;

#define DEBUG_OPER_STACK(push_pop,conclusion,oper) \
    if (access_expr_debug & EXPR_DBG_OPERATOR_STACK) \
    { \
	if (conclusion == 0) \
	  buginf("\nEXPR: (012) %-4s entry #%d (operator = '%c').", \
		push_pop, stack_count, oper); \
	else \
	  buginf("\nEXPR: (012) %-4s entry #%d (operator = '%c', conclusion = %s).",\
		push_pop, stack_count, oper, conclusion); \
    }

/*
**   L o c a l   D e f i n i t i o n s
*/
static	expr_stack_entry_t	expr_stack[MAX_EXPR_STACK_ENTRIES];
static	expr_operator_entry_t	expr_operator_stack[MAX_EXPR_OPER_ENTRIES];

static	int			expr_buffer_index;
static	char			expr_show_buffer[MAX_EXPR_SHOW_BUFFER+1];
static	expr_tree_entry_t	*expr_root;
static	int			expr_expression_index;
static	expr_category_entry_t	expr_category_list[] =
{
    { "lsap",		EXPR_TREE_ACCESS_LIST_LSAP },
    { "type",		EXPR_TREE_ACCESS_LIST_TYPE },
    { "smac",		EXPR_TREE_ACCESS_LIST_SMAC },
    { "dmac",		EXPR_TREE_ACCESS_LIST_DMAC },
    { "netbios-host",	EXPR_TREE_NETBIOS_HOST },
    { "netbios-bytes",	EXPR_TREE_NETBIOS_BYTES },
    { 0,		0 }
};

/*
**   E x t e r n a l   D e f i n i t i o n s
*/
extern  int access_expr_debug;

/*
**   F o r w a r d   D e c l a r a t i o n s 
*/
static void nv_access_expression(parseinfo *, hwidbtype *);


/******************************************************************************
*
* Function:	access_expression_command()
*
* Description:
*	This function acts as the interface to the EXEC command processor
*	and builds an ExpressionTree on the current IDB.  All messages that
*	emanate from the Expression Analyzer come from this function.  None
*	of the lower-level functions issue diagnostics.
*
* Call:		access_expression_command (parseinfo *)
*
* Input:
*   Arguments -
*	parseinfo	pointer to the parseinfo structure containing the
*			operand on the "access-expression" command.
*			
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*
*****************************************************************************/

void access_expression_command (parseinfo *csb)
{
    int			expr_rc;
    boolean		input_direction;
    hwidbtype		*idb;
    netbios_sb_t        *netb_sb;
    expr_tree_entry_t	*idb_tree;

    if (csb->nvgen) {
	nv_access_expression(csb, csb->interface->hwptr);
	return;
    }

    /*
    **  Get the direction:	"input" or "output"
    */
    input_direction = GETOBJ(int,1);    /* TRUE for input, FALSE for output */

    /*
    **  Make sure no other relevant access-lists have been specified for this
    **  interface.
    */
    idb = hwidb_or_null(csb->interface);
    if (idb == NULL) 
    {
      printf("\nEXPR: An interface must be specified for an access-expression.");
      return;
    }

    netb_sb = netbios_get_sb(idb->firstsw);
    if (input_direction)
    {
	if ((idb->srb_lsap_in)    ||
	    (idb->srb_type_in)    ||
	    (idb->srb_address_in) ||
            (netb_sb && 
              (netb_sb->ibm_netbios_h_iacf || netb_sb->ibm_netbios_b_iacf))) {
	    printf("\nEXPR: access-expression must be specified alone on interface");
            return;
	}
    }
    else
    {
	if ((idb->srb_lsap_out)    ||
	    (idb->srb_type_out)    ||
	    (idb->srb_address_out) ||
            (netb_sb && 
              (netb_sb->ibm_netbios_h_oacf || netb_sb->ibm_netbios_b_oacf))) {
	    printf("EXPR: access-expression must be specified alone on interface");
            return;
	}
    }

    /*
    **  If "no access-expression input|output", then free the ExpressionTree
    **  currently linked onto the idb.  If there really isn't an
    **  ExpressionTree defined, merely return without complaining.
    */
    if (!csb->sense) 
    {
	leveltype level;

	idb_tree = ((input_direction) ? 
		    idb->access_expression_in :
		    idb->access_expression_out);
	if (idb_tree == NIL_EXPR_TREE_ENTRY)
	    return;
	level = raise_interrupt_level(NETS_DISABLE);
	expr_free_node(idb_tree);
	if (input_direction)
	    idb->access_expression_in = NULL;
	else
	    idb->access_expression_out = NULL;
	reset_interrupt_level(level);
	return;
    }

    /*
    **  Analyze the Expression and build an ExpressionTree.  If the
    **  Expression is valid, place the ExpressionTree onto either the
    **  input-side or output-side of the idb.
    **
    **  If there is already an ExpressionTree on the idb, free the
    **  old one.
    */
    expr_rc = expr_analyze(GETOBJ(string,1), &expr_root);
    if (expr_rc == EXPR_OK)
    {
	if (input_direction) 
	{
	    if (idb->access_expression_in) 
		expr_free_node(idb->access_expression_in);
	    idb->access_expression_in = expr_root;
	    idb->srb_access_in = TRUE;
	} 
	else 
	{
	    if (idb->access_expression_out)
	       expr_free_node(idb->access_expression_out);
	    idb->access_expression_out = expr_root;
	    idb->srb_access_out = TRUE;
	}
#ifdef DEBUG_EXPR
	if (access_expr_debug & EXPR_DBG_EXPRESSION_TREE)
	{
	    /*
	    **  Print out a header.
	    */
	    buginf("\n  EXPR: (011) Dump of the ExpressionTree:\n");
	    buginf("    %10s  %10s  %2s  %10s  %2s  ", 
			"Address", "LChild",  "Ld", "RChild", "Rd");
	    buginf("%12s  %5s\n", 
			"Flags", "Value");

	    expr_dump_expression_tree(expr_root);
	}
    #endif

    }
    else
    {
	switch (expr_rc)
	{
	    case EXPR_UNKNOWN_SYMBOL:
	      printf("\nEXPR: Unknown Symbol encountered in Expression.");
	      break;
	    case EXPR_BAD_EXPRESSION:
	      printf("\nEXPR: Invalid Expression.");
	      break;
	    case EXPR_LOGIC_ERROR:
	      printf("\nEXPR: Expression analysis logic error.");
	      break;
	    case EXPR_NO_MEMORY:
	      printf("\nEXPR: Not enough memory.");
	      break;
	    case EXPR_INVALID_LIST:
	      printf("\nEXPR: Unknown access list.");
	      break;
	}
    }
    return;
}

/******************************************************************************
*
* Function:	show_access_expression()
*
* Description:
*	This function displays the Expression that is currently defined
*	by the ExpressionTree.
*
* Call:		show_access_expression (parseinfo *)
*
* Input:
*   Arguments -
*	parseinfo	pointer to the parseinfo structure containing the
*			operand on the "show access-expression" command.
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*
*****************************************************************************/

void
show_access_expression (parseinfo *csb)
{
  hwidbtype *idb;

  FOR_ALL_HWIDBS(idb) {
    if (idb->access_expression_in || idb->access_expression_out) {
      printf("\nInterface %s:", idb->hw_namestring);
      if (idb->access_expression_in) {
	printf("\n   Input: ");
	expr_buffer_index = 0;
	expr_show_expression_tree(idb->access_expression_in);
	expr_show_buffer[expr_buffer_index++] = '\0';
	printf("%s", expr_show_buffer);
      }
      if (idb->access_expression_out) {
	printf("\n  Output: ");
	expr_buffer_index = 0;
	expr_show_expression_tree(idb->access_expression_out);
	expr_show_buffer[expr_buffer_index++] = '\0';
	printf("%s", expr_show_buffer);
      }
    }
  }
}
/******************************************************************************
*
* Function:	nv_access_expression()
*
* Description:
*	This function writes to NV RAM the Expression that is currently defined
*	by the ExpressionTree.
*
* Call:		nv_access_expression (csb, idb)
*
* Input:
*	parseinfo *csb -- parser information
*       idbtype *idb -- interface for which to give expressions 
*		   
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*
*****************************************************************************/

static void
nv_access_expression (parseinfo *csb, hwidbtype *idb)
{

  if ((GETOBJ(int,1) == TRUE) &&  idb->access_expression_in) {
    expr_buffer_index = 0;
    expr_show_expression_tree(idb->access_expression_in);
    expr_show_buffer[expr_buffer_index++] = '\0';
    nv_write(TRUE, "%s %s", csb->nv_command, expr_show_buffer);
  }
  if ((GETOBJ(int,1) == FALSE) && idb->access_expression_out) {
    expr_buffer_index = 0;
    expr_show_expression_tree(idb->access_expression_out);
    expr_show_buffer[expr_buffer_index++] = '\0';
    nv_write(TRUE, "%s %s", csb->nv_command, expr_show_buffer);
  }
}

/******************************************************************************
*
* Function:	expr_analyze()
*
* Description:
*	This function implements the Floyd-Evans method of Expression reduction.
*	The primary structure used by expr_analyze() is the ExpressionStack
*	along with variables that identify the TopOfStack and the number of
*	active entries on the ExpressionStack.
*
*	This function was orignially an arithmetic expression analyzer but was
*	modified to be a boolean expression analyzer.
*
*	expr_analyze() is implemented as a five-state finite state machine
*	which governs the reduction of the ExpressionStack.  These five states
*	are:
*	  1. E0 - expression initial state; keeps placing entries onto the
*		  ExpressionStack while a reduction is not possible.  This
*		  state in sensitive to unary Operators, open parentheses,
*		  and Symbols.
*	  2. T0 - term initial state; keeps placing entries onto the
*		  ExpressionStack while a reduction is not possible.  Works
*		  in concert with E0 to get to the end of a set of opening
*		  parentheses.
*	  3. P1 - prime reduction state; reduces ExpressionStack when a 
*		  multiplicative operation is specified for a prime and a term.
*	  4. T1 - term reduction state; reduces ExpressionStack when an
*		  additive operation is specified for a prime and a term.
*	  5. E1 - expression reduction state; detects the completion of the
*		  reduction on the Expression.
*
*	The grammar of the Expression that can be reduced by expr_analyze()
*	is:
*	    E ::= E+T  or  E-T  or  +T  or  -T  or  T  for arithmetic expr
*		  E&T  or  E|T  or  ~T  or  T          for boolean expr
*	    T ::= T*P  or  T/P  or  P
*	    P ::= I    or  (E)
*		where:
*			I .. identifier
*			E .. expression
*			T .. term
*			P .. prime
*
*	The reductions that are performed on this grammar can be summarized
*	in the following chart:
*	  Current	TopOfStack	Reduced		Scan for	Next
*	   State         Token(s)	  To	       Next Token      State
*	  -------	----------	-------	       ----------      -----
*	    E0		    I		   P		   Y		 P1
*			    (				   Y		 E0
*	      		    ~   			   Y		 T0
*	    T0		    I		   P		   Y		 P1
*			    (				   Y		 E0
*	    P1 		    P              T				 T1
*	    T1		E & or | T	   E				 E1
*			  ~ T	   	   E				 E1
*			    T		   E				 E1
*	    E1		 E & or |			   Y		 T0
*			   (E)		   P		   Y		 P1
*		        E & or | T	   E				 E1
*			  E eol		   				done
*	   
*	Note: TopOfStack Token(s) as represented in the above chart are
*	      shown in the order they would occur in Expression.  Their
*	      presence in the ExpressionStack would be in the reverse order.
*	    
*
* Call:		expr_analyze (expression, value, root)
*
* Input:
*   Arguments -
*	expression	string representing the string to analyze
*
* Output:	
*   Arguments -
*	value		value of the expression
*	root		pointer to the root of the ExpressionTree built from
*			the Expression.
*
*   Return Code -
*	returnCode	indicator of success/failure of the analysis of
*			expression
*			  == 0 .. expression was valid
*			  != 0 .. expression was invalid
*
* Notes:
*
*****************************************************************************/

int
expr_analyze (
    char		*expression,
    expr_tree_entry_t	**expr_root)
{
    int			stack_count, top;
    int			expr_rc;
    expr_state    	state;
    char		top_token;
    boolean		done;

    /*
    **  Initialze expr_analyze() variables.
    */
    top                   = MAX_EXPR_STACK_ENTRIES;
    stack_count           = 0;
    expr_expression_index = 0;
    state                 = E0;

    /*
    **  Extract the first Token from Expression to "prime" the 
    **  expr_analyze() process.
    */
    GETTOKEN;
    if (expr_rc != EXPR_OK)
	return(expr_rc);

    expr_rc  = EXPR_OK;
    done     = FALSE;
    while ((!done)  &&  (expr_rc == EXPR_OK))
    {
	switch (state)
	{

	    /*		S t a t e   E 0 		*/
	    case E0:
		top_token = expr_stack[top+0].token;
		switch (top_token)
		{
		    case 'I':
			/*  Reduce:	I --> P				*/
			expr_stack[top+0].token = 'P';
			GETTOKEN;
			state = P1;
			break;
		    case '(':
			GETTOKEN;
			state = E0;
			break;
		    case '~':
			GETTOKEN;
			state = T0;
			break;
		    default:
			expr_rc = EXPR_BAD_EXPRESSION;
		}
		break;

	    /*		S t a t e   T 0 		*/
	    case T0:
		top_token = expr_stack[top+0].token;
		switch (top_token)
		{
		    case 'I':
			/*  Reduce:	I --> P				*/
			expr_stack[top+0].token = 'P';
			GETTOKEN;
			state = P1;
			break;
		    case '(':
		    case '~':
			GETTOKEN;
			state = E0;
			break;
		    default:
			expr_rc = EXPR_BAD_EXPRESSION;
		}
		break;

	    /*		S t a t e   P 1 		*/
	    case P1:
		if (expr_stack[top+1].token == 'P')
		{
		    /*  Reduce:	P --> T					*/
		    expr_stack[top+1].token = 'T';
		    state = T1;
		}
		else
		    expr_rc = EXPR_BAD_EXPRESSION;
		break;

	    /*		S t a t e   T 1 		*/
	    case T1:
		if ((stack_count >= 4)				&&
		    (expr_stack[top+1].token == 'T')		&&
		   ((expr_stack[top+2].token == '&')      ||
		    (expr_stack[top+2].token == '|'))		&&
		    (expr_stack[top+3].token == 'E'))
		{
		    /*  
		    **  Reduce:		E & T --> E	-or
		    **			E | T --> E
		    */
		    expr_reduce_stack(expr_stack, top);
		    expr_stack[top+3].token = 'E';
		    expr_stack[top+2].token = expr_stack[top+0].token;
		    top += 2;
		    stack_count -= 2;
		    state = E1;
		}
		else
		{
		    if ((stack_count >= 3)			&&
			(expr_stack[top+1].token == 'T')    	&&
			(expr_stack[top+2].token == '~'))
		    {
			/*  Reduce:	~T --> E			*/
			expr_reduce_stack(expr_stack, top);
			expr_stack[top+2].token = 'E';
			expr_stack[top+2].node_pointer = 
				expr_stack[top+1].node_pointer;
			expr_stack[top+1].token = expr_stack[top+0].token;
			top++;
			stack_count--;
			state = E1;
		    }
		    else
		    {
			if (expr_stack[top+1].token == 'T')
			{
			    /*  Reduce:		T --> E			*/
			    expr_stack[top+1].token = 'E';
			    state = E1;
			}
			else
			{
			    expr_rc = EXPR_BAD_EXPRESSION;
			}
		    }
		}
		break;

	    /*		S t a t e   E 1 		*/
	    case E1:
		if ((stack_count >= 2)				&&
		    (expr_stack[top+1].token == 'E')     	&&
		   ((expr_stack[top+0].token == '&')     ||
		    (expr_stack[top+0].token == '|')))
		{
		    GETTOKEN;
		    state = T0;
		}
		else
		{
		    if ((stack_count >= 3)			&&
			(expr_stack[top+2].token == '(')     	&&
			(expr_stack[top+1].token == 'E')  	&&
			(expr_stack[top+0].token == ')'))
		    {
			/*  Reduce:		(E) --> P		*/
			expr_stack[top+2].token = 'P';
			expr_stack[top+2].node_pointer = 
				expr_stack[top+1].node_pointer;
			top += 2;
			stack_count -= 2;
			GETTOKEN;
			state = P1;
		    }
		    else
		    {
			if ((stack_count >= 2)				&&
			    (expr_stack[top+1].token == 'E'))
			{
			    if (((expr_stack[top+0].token == '\0')  ||
			         (expr_stack[top+0].token == '\n'))  	&&
			        (stack_count == 2))
			    {
				/*
				**  Finished with Expression.
				*/
				top++;
				stack_count--;
				*expr_root = expr_stack[top+0].node_pointer;
				expr_compute_depths(*expr_root);
				done   = TRUE;
			    }
			    else
			    {

				if ((stack_count >= 4)			&&
				    (expr_stack[top+3].token == 'E')    &&
				   ((expr_stack[top+2].token == '&')  ||
				    (expr_stack[top+2].token == '|')))
				{
				    /*  
				    **  Reduce:	E & E --> E 	-or-
				    **		E | E --> E
				    */
				    expr_reduce_stack(expr_stack, top);
				    expr_stack[top+3].token = 'E';
				    expr_stack[top+2].token 
					    = expr_stack[top+0].token;
				    top += 2;
				    stack_count -= 2;
				    state = E1;
				}
				else
				    expr_rc = EXPR_BAD_EXPRESSION;
			    }
			}
			else
			    expr_rc = EXPR_BAD_EXPRESSION;
		    }
		}
		break;
	    default:
		expr_rc = EXPR_LOGIC_ERROR;
	}
    }

    /*
    **  If there were any errors detected, free the ExpressionTree.
    */
    if (expr_rc != EXPR_OK)
	expr_free_expression_tree(expr_stack, top);

    return(expr_rc);
}

/******************************************************************************
*
* Function:	expr_get_token
*
* Description:
*	This function is repetitively called by expr_analyze() to
*	retrieve the next Token in the Expression.  The input to
*	expr_get_token () is a pointer to the start of Expression and an
	index into for the next character position in Expression.
*	Maintenance of this index is the total responsibility of
*	expr_get_token().  The output from expr_get_token() is a new
*	entry into ExpressionStack at the current TopOfStack location.
*
*	Since Token can be a single character of multiple character
*	entity, expr_get_token() must take care in the order which
*	Tokens are extracted from the Expression.  expr_get_token() is
*	sensitive to the single character Tokens first.  The single
*	character Tokens which expr_get_token() will extract are the
*	following:
*	    1. Operators
*	       a. arithmetic	'+' '-' '*' '/'
*	       b. boolean	'&' '|' '~'
*	    2. parentheses	'(' ')'
*	    3. end-of-line	'\0' or '\n'
*
*	If expr_get_token() encounters one of these single character
*	Tokens at the start of a particular invocation, this Token will
*	become the StackToken in the TopOfStack location of
*	ExpressionStack.  Since there is really no value associated
*	With these Tokens, the StackValue will be 0.
*
*	If expr_get_token() determines that the next Token is not one of
*	the above single character Tokens, then a Symbol is extracted
*	from Expression.  (Note: expr_get_token() treats whitespace
*	characters of blank and tab (' ' and '\t') as insignificant
*	characters except as proper terminators of Symbols.)  The end
*	of the Symbol is determined by thbe next occurrence of (a) one
*	of the above single-character expressions, (b) a whitespace
*	character, or (c) the of the Expression.  A Symbol will be
*	returned on the TopOfStack location of ExpressionStack with a
*	StackToken of 'I' and the appropriate value of StackValue.
*
* Call:		expr_get_token (expression, index, stack, top)
*
* Input:
*   Arguments -
*	expression	string representing the expression to be analyzed
*	index		index into expression of where begin in extracting
*			next Token
*	stack		pointer to the ExpressionStack
*	top		index to the current top of the ExpressionStack
*
* Output:	
*   Arguments -
*	index		index into expression of Token extraction ended
*
*   Return Code -
*	returnCode	indicator of success/failure of the Token retrieval
*			  EXPR_OK  
*				a valid Token was extracted
*			  EXPR_UNKNOWN_SYMBOL
*				a parsing error was detected
*
* Notes:
*   (1) 
*
*****************************************************************************/

int
expr_get_token (
    char		*expression,
    int			*index,
    expr_stack_entry_t	*stack,
    int			top)
{
    int			i_expr, token_rc;
    expr_tree_entry_t	*new_node;
    char		identifier[MAX_EXPR_IDENTIFIER+1];
    char		character;
    int			category;
    ulong		number;
    netbios_acclisttype	*netbios_list;
    boolean             parsed;

    /*
    **  Scan past any initial whitespace.
    */
    i_expr    = *index;
    while (((expression[i_expr] == ' ')     ||
            (expression[i_expr] == '\t'))  	&&  
	    (expression[i_expr] != '\0')    	&&
	    (expression[i_expr] != '\n'))
	i_expr++;

    token_rc = EXPR_OK;
    character = expression[i_expr];
    switch (character)
    {
	case '(':
        case ')':
        case '&':
        case '|':
        case '~':
	case '\0':
	case '\n':
	    stack[top].token        = character;
	    stack[top].node_pointer = NIL_EXPR_TREE_ENTRY;
	    i_expr++;
	    break;
	default:
	    /*
	    **  Allocate a new node for the ExpressionTree and place it
	    **  onto the top of the ExpressionStack.  Also mark this
	    **  ExpressionStack entry to be of type 'I'.
	    **
	    **  N.B. If there are any errors detected in the processing of the
	    **	     Expression, expr_analyze() will free any ExpressionTree
	    **	     nodes that live on the ExpressionStack.  No need to
	    **	    do it here.
	    */
	    token_rc = expr_allocate_node(&new_node);
	    if (token_rc != EXPR_OK)
		return(token_rc);
	    new_node->left_child    = NIL_EXPR_TREE_ENTRY;
	    new_node->right_child   = NIL_EXPR_TREE_ENTRY;
	    stack[top].token        = 'I';
	    stack[top].node_pointer = new_node;

	    /*
	    **  Extract the category designator from the command line buffer
	    **  and make sure it is one of the known values.  Set up the
	    **  ExpressionTree node that has already been placed onto the
	    **  top of the ExpressionStack.
	    **
	    **  This subexpression must be one of the following:
	    **	    lsap(2nn)		type(2nn)
	    **	    smac(7nn)		dmac(7nn)
	    **	    netbios-host(name)	netbios-bytes(name)
	    */
	    expr_get_identifier(expression, &i_expr, identifier, 
		MAX_EXPR_IDENTIFIER);
	    if (!(expr_lookup_category (identifier, &category)))
		return(EXPR_UNKNOWN_SYMBOL);
	    if (expression[i_expr++] != '(')
		return(EXPR_BAD_EXPRESSION);
	    expr_get_identifier(expression, &i_expr, identifier, 
		MAX_EXPR_IDENTIFIER);
	    new_node->flags = category;
	    switch (category) {
		case EXPR_TREE_ACCESS_LIST_LSAP:
		case EXPR_TREE_ACCESS_LIST_TYPE:
		    number = parse_unsigned(identifier, &parsed);
		    if (!parsed)
			return(EXPR_INVALID_LIST);
		    if ((number < MINTYPEACC)	|| (number > MAXTYPEACC))
			return(EXPR_INVALID_LIST);
		    new_node->access.access_list_number = (int) number;
		    break;
		case EXPR_TREE_ACCESS_LIST_SMAC:
		case EXPR_TREE_ACCESS_LIST_DMAC:
		    number = parse_unsigned(identifier, &parsed);
		    if (!parsed)
			return(EXPR_INVALID_LIST);
		    if ((number < MINMACADDRACC) || (number > MAXMACADDRACC))
			return(EXPR_INVALID_LIST);
		    new_node->access.access_list_number = (int) number;
		    break;
		case EXPR_TREE_NETBIOS_HOST:
		case EXPR_TREE_NETBIOS_BYTES:
		    netbios_list = reg_invoke_netbios_acl_get(category,
							      identifier);
		    if (netbios_list == NULL)
			return(EXPR_INVALID_LIST);
		    strcpy(new_node->access.netbios.name, identifier);
		    new_node->access.netbios.access_list = netbios_list;
		    break;
		default:
		    return(EXPR_LOGIC_ERROR);
	    }
	    if (expression[i_expr++] != ')')
		return(EXPR_BAD_EXPRESSION);
    }
    *index = i_expr;
    return(token_rc);
}

/******************************************************************************
*
* Function:	expr_reduce_stack()
*
* Description:
*	This function is called by expr_analyze() whenever it
*	determines that the top of ExpressionStack is in a state where
*	it can be simplified by performing the indicated boolean
*	computations on the entries of the ExpressionStack.  The
*	reduction is not performed on the absolute top of the
*	ExpressionStack, but on locations 1 and below.
*
*	This function processes either binary or unary operators.  For
*	binary operators, the ExpressionStack entries that are of
*	interest are at locations 1, 2, and 3.  For unary operators,
*	the ExpressionStack entries that are of interest are at
*	locations 1 and 2.
*
*	The two values in the ExpressionStack at these locations are
*	combined in the manner indicated in location 2 and the result
*	is placed bath in the ExpressionStack at location 3.
*
* Call:		expr_reduce_stack (stack, top)
*
* Input:
*   Arguments -
*	stack		pointer to the ExpressionStack
*	top		index to the current top of the ExpressionStack
*
* Output:	
*   Arguments -
*	stack		contents are altered according to the above
*			description
*   Return Code -
*	returnCode	indicator of success/failure of the reduction
*			  EXPR_OK  
*				the ExpressionStack was successfully reduced
*			  EXPR_LOGIC_ERROR
*				the ExpressionStack is hosed
*			  EXPR_NO_MEMORY
*				expr_allocate_node() couldn't get a node
*
* Notes:
*   (1) Operands for Unary Operators.
*	Since unary operators have a single operand, the subtree representing
*	that operand is hung, by convention, off the left subtree.
*
*****************************************************************************/

int
expr_reduce_stack (
    expr_stack_entry_t	*stack,
    int			top)
{
    int			reduce_rc;
    expr_tree_entry_t	*new_node;
    char		operator;

    reduce_rc = expr_allocate_node(&new_node);
    if (reduce_rc != EXPR_OK)
	return(reduce_rc);
    new_node->flags = EXPR_TREE_OPERATOR;
    operator  = stack[top+2].token;
    switch (operator)
    {
	case '&':
	    new_node->access.operator = operator;
	    new_node->left_child      = stack[top+1].node_pointer;
	    new_node->right_child     = stack[top+3].node_pointer;
	    stack[top+3].node_pointer = new_node;
	    break;
	case '|':
	    new_node->access.operator = operator;
	    new_node->left_child      = stack[top+1].node_pointer;
	    new_node->right_child     = stack[top+3].node_pointer;
	    stack[top+3].node_pointer = new_node;
	    break;
	case '~':
	    new_node->access.operator = operator;
	    new_node->left_child      = stack[top+1].node_pointer;
	    new_node->right_child     = NIL_EXPR_TREE_ENTRY;
	    stack[top+1].node_pointer = new_node;
	    break;
	default:
	    reduce_rc = EXPR_LOGIC_ERROR;
    }
    return(reduce_rc);
}

/******************************************************************************
*
* Function:	expr_compute_depths()
*
* Description:
*	This function walks down an ExpressionTree and computes the
*	depths of the left and right subtrees for each node.
*
*	NOTE: This function is recursive.
*
* Call:		expr_compute_depths (root)
*
* Input:
*   Arguments -
*	root		first time: root of the ExpressionTree
*			other times: root of a subtree of ExpressionTree
*			
* Output:	
*   Return Code -
*	depth		the depth of the subtree beneath this node
*
* Notes:
*
*****************************************************************************/

int
expr_compute_depths (expr_tree_entry_t *root)
{
    int		left_side, right_side;

    /*
    **  Compute the depth of the left subtree.
    */
    if (root->left_child  == NIL_EXPR_TREE_ENTRY)
    {
	left_side        = 0;
	root->left_depth = left_side;
    }
    else
    {
	left_side        = 1 + expr_compute_depths(root->left_child);
	root->left_depth = left_side;
    }

    /*
    **  Compute the depth of the right subtree.
    */
    if (root->right_child  == NIL_EXPR_TREE_ENTRY)
    {
	right_side        = 0;
	root->right_depth = right_side;
    }
    else
    {
	right_side        = 1 + expr_compute_depths(root->right_child);
	root->right_depth = right_side;
    }

    /*
    **  Return the larger of the depths of the left and right subtrees.
    */
    if (left_side >= right_side)
	return(left_side);
    else
	return(right_side);
}

/******************************************************************************
*
* Function:	expr_evaluate_tree()
*
* Description:
*	This function walks down the ExpressionTree and determines the boolean
*	result of that tree.  The properties of the ExpressionTree that are
*	exploited by this function are:
*	  1. all leaf nodes are boolean variables, and
*	  2. all intermediate nodes are boolean operators.
*
*	This function optimizes the search of the ExpressionTree in an attempt
*	to reach a boolean result by examining the fewest number of nodes
*	in ExpressionTree.  This is done by examining the shallowest subtree
*	of an intermediate node first. 
*
* Call:		expr_evaluate_tree (dgstart, dgsize, riflen, root, value)
*
* Input:
*   Arguments -
*	dgstart		pointer to the start of the packet
*	dgsize 		size (in bytes) of the packet
*	riflen		size (in bytes) of the RIF
*	idb		pointer to the Interface Description Block
*	root		pointer to the ExpressionTree to evaluate
*			
* Output:	
*   Arguments -
*	value		boolean result of passing the packet through
*			the ExpressionTree
*   Return Code -
*	return_code	an indicator of the success/failure of processing
*			the packet through the ExpressionTree
*			  EXPR_OK          .. successful
*			  EXPR_LOGIC_ERROR .. a logic error has occurred
*
* Notes:
*   (1)	When processing the ExpressionTree only intermediate nodes 
*	(i.e. Operators) are maintained on the OperatorStack.
*   (2) visit_count is a count of the number of subtrees that have been
*       visited for a particular operator node.
*       For binary operators, the following rules apply:
*               visit_count     Meaning
*               -----------     -------
*                   0           examine the shallowest subtree
*                   1           examine the other subtree
*                   2           done with this node
*       For unary operators, the following rules apply:
*               visit_count     Meaning
*               -----------     -------
*                   0           examine the left subtree
*                   1           done with this node 
*                   2           ** impossible ** 
*       After a subtree is visited, the visit_count is incremented.
*
*****************************************************************************/

int
expr_evaluate_tree (
    uchar		*dgstart,
    ulong		dgsize,
    int			riflen,
    hwidbtype		*idb,
    expr_tree_entry_t	*root,
    boolean		*value)
{
    expr_tree_entry_t	*tree_node;
    int			visit_count, top, stack_count;
    boolean		done, leaf_result, subtree_result, ignored;

    /*
    **  Initialize OperatorStack.
    */
    top         = MAX_EXPR_OPER_ENTRIES;
    stack_count = 0;

    /*
    **  Initialize ExpressionTree evaluation loop variables.  The 
    **  initialization of all these variables isn't really necessary unless
    **  you want -Wall to shut up.
    */
    tree_node        = root;
    done             = FALSE;
    visit_count      = 0;
    leaf_result      = FALSE;
    subtree_result   = FALSE;
    while (!done)
    {
	if (tree_node->flags & EXPR_TREE_OPERATOR)
	{
	    switch (visit_count)
	    {
		case 0:
		    /*
		    **  New intermediate (i.e. operator) node.  Push it
		    **  onto the top of the OperatorStack and determine
		    **  which subtree to go down first.
		    */
		    top--;
		    stack_count++;
		    DEBUG_OPER_STACK("Push", 0, tree_node->access.operator);
		    expr_operator_stack[top].node_pointer = tree_node;
		    expr_operator_stack[top].operator = 
			tree_node->access.operator;
		    expr_operator_stack[top].visit_count = 1;
		    expr_operator_stack[top].result = 0;
		    if (tree_node->right_depth == 0)
			/*  Special case for unary operator. */
			tree_node = tree_node->left_child;
		    else if (tree_node->left_depth <= tree_node->right_depth)
			tree_node = tree_node->left_child;
		    else
			tree_node = tree_node->right_child;
		    if (tree_node->flags & EXPR_TREE_OPERATOR)
			visit_count = 0;
		    break;
		case 1:
		    /*
		    **  An intermediate node that has had 1 subtree
		    **  examined with no final result.  
		    **
		    **  For binary operators, we have to go down the other
		    **  subtree.  
		    **
		    **  For unary operators, we're finished with this node
		    **  so pop it off the stack.
		    */
		    switch (expr_operator_stack[top].operator)
		    {
			case '&':
			case '|':
			    expr_operator_stack[top].result = subtree_result;
			    expr_operator_stack[top].visit_count = 2;
			    if (tree_node->left_depth > tree_node->right_depth)
				tree_node = tree_node->left_child;
			    else
				tree_node = tree_node->right_child;
			    if (tree_node->flags & EXPR_TREE_OPERATOR)
				visit_count = 0;
			    break;
			case '~':
			    if (ignored)
			      subtree_result = TRUE;
			    else
			      subtree_result = (subtree_result) ? FALSE : TRUE;
			    expr_operator_stack[top].result = subtree_result;
			    DEBUG_OPER_STACK("Pop","normal",
				expr_operator_stack[top].operator);
			    top++;
			    stack_count--;
			    if (stack_count == 0)
				done = TRUE;
			    else
			    {
				tree_node   = 
					expr_operator_stack[top].node_pointer;
				visit_count = 
					expr_operator_stack[top].visit_count;
			    }
			    break;
			default:
			    return(EXPR_LOGIC_ERROR);
		    }
		    break;
		case 2:
		    /*
		    **  An intermediate node that has had both subtrees
		    **  examined (or we know the result) .  We are finished
		    **  with this node so pop it off the stack.
		    */
		    switch (expr_operator_stack[top].operator)
		    {
			case '&':
			    subtree_result &= expr_operator_stack[top].result;
			    break;
			case '|':
			    subtree_result |= expr_operator_stack[top].result;
			    break;
		    }
		    DEBUG_OPER_STACK("Pop","normal",
			expr_operator_stack[top].operator);
		    top++;
		    stack_count--;
		    if (stack_count == 0)
			done = TRUE;
		    else
		    {
			tree_node   = expr_operator_stack[top].node_pointer;
			visit_count = expr_operator_stack[top].visit_count;
		    }
		    break;
		default:
		    return(EXPR_LOGIC_ERROR);
	    }
	}
	else
	{
	    /*
	    **  We're at a leaf node (i.e. a Symbol) in ExpressionTree.  
	    **  Determine the boolean result of this Symbol.
	    */
	    leaf_result = expr_determine_symbol_value(dgstart, dgsize, riflen,
						      tree_node, &ignored);

	    if (stack_count > 0)
	    {
		/*
		**  Reduce this subtree based upon the value of Operator
		**  and Symbol.
		*/
		switch (expr_operator_stack[top].visit_count)
		{
		    case 1:
			/*
			**  First subtree examination. We can only reach a 
			**  conclusion on the 2st subtree examination in the
			**  following cases:
			**    1. Operator is '~',
			**    2. Operator is '&' and Symbol is false,
			**    3. Operator is '|' and Symbol is true.
			*/
			switch (expr_operator_stack[top].operator)
			{
			    case '~':
				/*  Don't negate it - it's done above. */
				subtree_result = leaf_result;
				break;
			    case '&':
				if (leaf_result == FALSE)
				{
				    subtree_result = leaf_result;
				    DEBUG_OPER_STACK("Pop","early",
					expr_operator_stack[top].operator);
				    top++;
				    stack_count--;
				    if (stack_count == 0)
					done = TRUE;
				}
				else
				    subtree_result = TRUE;
				break;
			    case '|':
				if (leaf_result == TRUE)
				{
				    DEBUG_OPER_STACK("Pop","early",
					expr_operator_stack[top].operator);
				    subtree_result = leaf_result;
				    top++;
				    stack_count--;
				    if (stack_count == 0)
					done = TRUE;
				}
				else
				    subtree_result = FALSE;
				break;
			    default:
				return(EXPR_LOGIC_ERROR);
			}
			break;
		    case 2:
			/*
			**  Second subtree examination. The conclusion of the
			**  subtree is the value of the Symbol.
			*/
			if (expr_operator_stack[top].operator == '~')
			    return(EXPR_LOGIC_ERROR);
			else
			{
			    DEBUG_OPER_STACK("Pop","normal",
				expr_operator_stack[top].operator);
			    subtree_result = leaf_result;
			    top++;
			    stack_count--;
			    if (stack_count == 0)
				done = TRUE;
			}
			break;
		    default:
			return(EXPR_LOGIC_ERROR);
		}
	    }
	    else
	    {
		/*
		**  Degenerate case where the ExpressionTree is a single 
		**  Symbol node.
		*/
		subtree_result = leaf_result;
		done = TRUE;
	    }
	    if (stack_count > 0)
	    {
		tree_node   = expr_operator_stack[top].node_pointer;
		visit_count = expr_operator_stack[top].visit_count;
	    }
	}
    }
    *value = subtree_result;
    return(EXPR_OK);
}

/******************************************************************************
*
* Function:	expr_determine_symbol_value()
*
* Description:
*	This function walks down the ExpressionTree and determines the boolean
*	result of that tree.  The properties of the ExpressionTree that are
*	exploited by this function are:
*	  1. all leaf nodes are boolean variables, and
*	  2. all intermediate nodes are boolean operators.
*
*	This function optimizes the search of the ExpressionTree in an attempt
*	to reach a boolean result by examining the fewest number of nodes
*	in ExpressionTree.  This is done by examining the shallowest subtree
*	of an intermediate node first. 
*
* Call:		expr_determine_symbol_value (dgstart, dgsize, riflen, idb, node)
*
* Input:
*   Arguments -
*	dgstart		pointer to the start of the packet
*	dgsize 		size (in bytes) of the packet
*	riflen		size (in bytes) of the RIF
*	node		pointer to the ExpressionTree Symbol node whose
*			value is to be determined
*			
* Output:	
*   Arguments -
*	ignored		boolean indicator that identifies whether or not the
*			packet was handed to the real access-list logic.
*			See Note (1) below.
*   Return Code -
*	return_code	an indicator of the premit/deny result of this Symbol
*			node.
*			  TRUE  .. this Symbol says permit the packet
*			  FALSE .. this Symbol says deny the packet
*
* Notes:
*   (1)	The 'ignored' Boolean
*
*	The boolean indicator 'ignored' is used primarily for the sake of
*	NetBIOS packets.  Packets that are known not to meet the requirements
*	for NetBIOS are allowed through the interface (i.e. return_code is
*	TRUE) but the 'ignored' boolean is also set to TRUE in this case.
*	This permits the higher level routine, namely expr_evaluate_tree(),
*	to handle this case correctly in order to effectively ignore the
*	NetBIOS term in the expression.
*
*	All non-NetBIOS Symbols (lsap, type, dmac, and smac) are always
*	processed and therefore ignored is FALSE.
*
*****************************************************************************/

boolean
expr_determine_symbol_value (uchar *dgstart, ulong dgsize, int riflen,
			     expr_tree_entry_t *symbol_node,
			     boolean *ignored)
{
    ushort lsap, typecode;
    boolean leaf_result;
    int what;

    /*
    **  We're at a leaf node (i.e. a Symbol) in ExpressionTree.  
    **  Determine the boolean result of this Symbol.
    **
    **  N.B. "Symbols" are, in reality, calls to the original
    **	     access-list processing routines as follows:
    **	       LSAP			access_check_type()
    **	       Type			access_check_type()
    **	       MAC Address 		access_check_mac()
    **	       NetBIOS Filter		netbios_acfcheck()
    */
    leaf_result = FALSE;
    *ignored    = FALSE;
    what = (symbol_node->flags & EXPR_TREE_SYMBOL_MASK);
    switch (what) {
	case EXPR_TREE_ACCESS_LIST_LSAP:
	    lsap = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen));
	    leaf_result = 
		access_check_type(symbol_node->access.access_list_number,lsap);
	    break;

	case EXPR_TREE_ACCESS_LIST_TYPE:
	    typecode = GETSHORT((dgstart+TRING_ENCAPBYTES+riflen
				  +LLC_LSAP_OFFSET));
	    leaf_result = 
		access_check_type(symbol_node->access.access_list_number,
				 typecode);
	    break;

	case EXPR_TREE_ACCESS_LIST_SMAC:
	    leaf_result = 
		access_check_mac(symbol_node->access.access_list_number,
				 dgstart + TR_SA_OFFSET);
	    break;

	case EXPR_TREE_ACCESS_LIST_DMAC:
	    leaf_result = 
		access_check_mac(symbol_node->access.access_list_number,
				 dgstart + TR_DA_OFFSET);
	    break;

	case EXPR_TREE_NETBIOS_HOST:
	case EXPR_TREE_NETBIOS_BYTES:
	    leaf_result = reg_invoke_netbios_acl_check(what, dgstart, dgsize,
						       riflen,  symbol_node,
						       ignored);
	    break;
    }

    /*
     *  Return the value derived from calling the access list routine.
     */
    return(leaf_result);
}

/******************************************************************************
*
* Function:	expr_free_expression_tree()
*
* Description:
*	This function walks down the ExpressionStack and frees all 
*	ExpressionTree entries that are being built.
*
* Call:		expr_free_expression_tree (stack, top)
*
* Input:
*   Arguments -
*	stack		pointer to the ExpressionStack
*	top		index to the current TopOfStack
*			
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*
*****************************************************************************/

void
expr_free_expression_tree (
    expr_stack_entry_t	*stack,
    int			top)
{
    expr_tree_entry_t	*node_ptr;
    int			i_stack;

    /*
    **  For each entry in ExpressionStack that contains a pointer to a node
    **  for the ExpressionTree, free the ExpressionTree entry.  Note that
    **  the pointer addressed by the ExpressionStack entry might be a pointer
    **  to a subtree rather than just a node. expr_free_node() is recursive
    **  and takes care of this situation.
    */
    for (i_stack = top; i_stack < MAX_EXPR_STACK_ENTRIES; i_stack++) {
	node_ptr = stack[i_stack].node_pointer;
	if (node_ptr != NIL_EXPR_TREE_ENTRY)
	    expr_free_node(node_ptr);
    }
}

/******************************************************************************
*
* Function:	expr_allocate_node()
*
* Description:
*	This function is allocates enough memory for an entry in the
*	ExpressionTree and performs some meager initialization functions.
*
* Call:		expr_allocate_node (node)
*
* Input:
*   Arguments -
*	node		pointer to the pointer of the ExpressionTree entry
*			
*
* Output:	
*   Return Code -
*	returnCode	indicator of success/failure of the memory allocation
*			  EXPR_OK  
*				memory was allocated and the ExpressionTree
*				node was initialized
*			  EXPR_NO_MEMORY
*				malloc() failed
*
* Notes:
*   (1) 
*
*****************************************************************************/

int
expr_allocate_node (expr_tree_entry_t **node_ptr)
{
    expr_tree_entry_t	*new_node;

    /*
    **  Allocate a node for the ExpressionTree.
    */
    new_node = malloc(sizeof(expr_tree_entry_t));
    if (new_node == NIL_EXPR_TREE_ENTRY)
	return(EXPR_NO_MEMORY);
    
    /*
    **  Perform rudimentary initialization of ExpressionTree node.
    */
    new_node->left_child  = NIL_EXPR_TREE_ENTRY;
    new_node->right_child = NIL_EXPR_TREE_ENTRY;
    *node_ptr             = new_node;
    return(EXPR_OK);
}

/******************************************************************************
*
* Function:	expr_free_node()
*
* Description:
*	This function frees the left and right subtrees of the specified
*	node and then frees this node iteslf.
*
*	NOTE: This function is recursive.
*
* Call:		expr_free_node (node)
*
* Input:
*   Arguments -
*	node		pointer to the node in ExpressionTree that is
*			to be freed
*			
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*
*****************************************************************************/

void
expr_free_node (expr_tree_entry_t *node_ptr)
{
    /*
    **  Free the the ExpressionTree nodes down the left subtree.
    */
    if (node_ptr->left_child != NIL_EXPR_TREE_ENTRY)
    {
	expr_free_node(node_ptr->left_child);
	node_ptr->left_child = NIL_EXPR_TREE_ENTRY;
    }

    /*
    **  Free the the ExpressionTree nodes down the right subtree.
    */
    if (node_ptr->right_child != NIL_EXPR_TREE_ENTRY)
    {
	expr_free_node(node_ptr->right_child);
	node_ptr->right_child = NIL_EXPR_TREE_ENTRY;
    }

    /*
    **  Free this ExpressionTree node.
    */
    free(node_ptr);
    return;
}

/******************************************************************************
*
* Function:	expr_lookup_category()
*
* Description:
*	This function looks up the specified string in the list of valid
*	keywords and returns an indicator of the type of keyword that was
*	found.
*
* Call:		expr_lookup_category (string, category)
*
* Input:
*   Arguments -
*	string		pointer to the keyword string
*			
* Output:	
*   Arguments -
*	category	the values to be assigned to the 'flags' field of
*			the ExpressionTree node.
*   Return Code -
*	returnCode	boolean indicator of the success of the keyword search
*			  TRUE  .. 'string' matches a valid keyword
*			  FALSE .. 'string' is not a valid keyword
*
* Notes:
*
*****************************************************************************/

boolean
expr_lookup_category (char *string, int *category)
{
    int		i;

    for (i = 0; (expr_category_list[i].keyword != 0); i++)
    {
	if  (strcmp (expr_category_list[i].keyword, string) == 0)
	{
	    *category = expr_category_list[i].flag;
	    return(TRUE);
	}
    }
    return(FALSE);
}

#ifdef DEBUG_EXPR
/*
**  Function:	expr_dump_expression_tree()
**
**  Description:
**	Dump the contents of the ExpressionTree.
**
**  Note:
**   (1) This function is recursive.  Since it is recursive, ExpressionTree
**	 entries are printed out in a bottom-up manner.
**
*/
void
expr_dump_expression_tree (expr_tree_entry_t *root)
{
    int		i;

    /*
    **  Recursively go down the left and right subtrees of this node.
    */
    if (root->left_child != NIL_EXPR_TREE_ENTRY)
	expr_dump_expression_tree(root->left_child);
    if (root->right_child != NIL_EXPR_TREE_ENTRY)
	expr_dump_expression_tree(root->right_child);

    /*
    **  Print out the contents of this node.
    */
    buginf("    0x%08lx  ", root);
    buginf("0x%08lx  ", root->left_child);
    buginf("%2d  ", root->left_depth);
    buginf("0x%08lx  ", root->right_child);
    buginf("%2d  ", root->right_depth);
    switch (root->flags)
    {
	case EXPR_TREE_OPERATOR:
	    buginf("%12s  %c", "Operator", root->access.operator);
	    break;
        case EXPR_TREE_ACCESS_LIST_LSAP:
            buginf("%12s  %5d", "Lsap", root->access.access_list_number);
            break;
        case EXPR_TREE_ACCESS_LIST_TYPE:
            buginf("%12s  %5d", "Type", root->access.access_list_number);
            break;
        case EXPR_TREE_ACCESS_LIST_SMAC:
            buginf("%12s  %5d", "Smac", root->access.access_list_number);
            break;
        case EXPR_TREE_ACCESS_LIST_DMAC:
            buginf("%12s  %5d", "Dmac", root->access.access_list_number);
            break;
        case EXPR_TREE_NETBIOS_HOST:
            buginf("%12s  ", "NetBIOS Host");
	    for (i = 0; 
		((i < 8) && 
		 (root->access.netbios.name[i] != '\0'));
		 i++)
	    {
		buginf("%c", root->access.netbios.name[i]);
	    }
            break;
        case EXPR_TREE_NETBIOS_BYTES:
            buginf("%12s  ", "NetBIOS Bytes");
            for (i = 0; 
		((i < 8) && 
		 (root->access.netbios.name[i] != '\0'));
		 i++)
	    {
		buginf("%c", root->access.netbios.name[i]);
	    }
	    break;
    }
    buginf("\n");
}
#endif

/******************************************************************************
*
* Function:	expr_show_expression_tree()
*
* Description:
*	This function builds parts of the Expression into a buffer.
*
*	NOTE: This function is recursive.
*
*
* Call:		expr_show_expression_tree(node_pointer)
*
* Input:
*   Arguments -
*	node_pointer		pointer to a node in the ExpressionTree
*			
* Output:	
*   Return Code -
*	(None)
*
* Notes:
*
*****************************************************************************/

void
expr_show_expression_tree (expr_tree_entry_t *node_pointer)
{
    char	subexpression[MAX_EXPR_SZ];
    char	leaf_char;

    leaf_char = ' ';
    if (node_pointer->flags & EXPR_TREE_OPERATOR)
    {
	/*
	**  This is an Operator node. 
	**  Print: 	"(left operator right)" for binary operators.
	**  Print: 	"~left" 	        for unary operators.
	*/
	if (node_pointer->access.operator == '~')
	{
	    expr_show_buffer[expr_buffer_index++] = 
		    node_pointer->access.operator;
	    expr_show_expression_tree(node_pointer->left_child);
	}
	else
	{
	    expr_show_buffer[expr_buffer_index++] = '(';
	    if (node_pointer->left_child != NIL_EXPR_TREE_ENTRY)
		expr_show_expression_tree(node_pointer->left_child);
	    expr_show_buffer[expr_buffer_index++] = ' ';
	    expr_show_buffer[expr_buffer_index++] = 
		    node_pointer->access.operator;
	    expr_show_buffer[expr_buffer_index++] = ' ';
	    if (node_pointer->right_child != NIL_EXPR_TREE_ENTRY)
		expr_show_expression_tree(node_pointer->right_child);
	    expr_show_buffer[expr_buffer_index++] = ')';
	}
    }
    else
    {
	/*
	**  This is a Symbol node. 
	**  Print: 	"Symbol""
	**
	**  N.B. It is possible for netbios-host or netbios-bytes access 
	**       lists to be deleted and the access-expression still contains
	**       the name of the access-list.  In this case, an asterisk '*'
	**       is printed in front of the deleted access-list.
	**
	**       For exaample, if the following commands were entered:
	**          no netbios access-list host accept
	**
	**       a sample 'show access-expression' would display:
	**         Input: (smac(701) & *netbios-host(accept))
	*/
	switch (node_pointer->flags & EXPR_TREE_SYMBOL_MASK)
	{
	    case EXPR_TREE_ACCESS_LIST_LSAP:
		sprintf(subexpression, "lsap(%03d)", 
			node_pointer->access.access_list_number);
		break;
	    case EXPR_TREE_ACCESS_LIST_TYPE:
		sprintf(subexpression, "type(%03d)", 
			node_pointer->access.access_list_number);
		break;
	    case EXPR_TREE_ACCESS_LIST_SMAC:
		sprintf(subexpression, "smac(%03d)", 
			node_pointer->access.access_list_number);
		break;
	    case EXPR_TREE_ACCESS_LIST_DMAC:
		sprintf(subexpression, "dmac(%03d)", 
			node_pointer->access.access_list_number);
		break;
	    case EXPR_TREE_NETBIOS_HOST:
		sprintf(subexpression, "%snetbios-host(%s)", 
			((node_pointer->access.netbios.access_list ==
			  NULL) ? "*" : ""),
			node_pointer->access.netbios.name);
		break;
	    case EXPR_TREE_NETBIOS_BYTES:
		sprintf(subexpression, "%snetbios-bytes(%s)", 
			((node_pointer->access.netbios.access_list ==
			  NULL) ? "*" : ""),
			node_pointer->access.netbios.name);
		break;
	}
	sprintf(&(expr_show_buffer[expr_buffer_index]), "%s", subexpression);
	expr_buffer_index += strlen(subexpression);
    }
    return;
}

/******************************************************************************
*
* Function:     expr_update_expression_tree()
*
* Description:
*       This function walks down the left and right subtrees of the
*	specified node looking for the specified NetBIOS access list.
*       Note that only leaf nodes (i.e. no left or right subtrees) can
*	define a Symbol and thus contain references to the NetBIOS 
*	access list.
*	
*       NOTE: This function is recursive.
*
* Call:         expr_update_expression_tree (node, netbios_list, operation)
*
* Input:
*   Arguments -
*       node            pointer to the node in ExpressionTree that is
*                       to be examined
*	netbios_list	pointer to the NetBIOS access list that is being
*			deleted or added.
*	operation	indicator of the operation being performed on this
*			NetBIOS access list
*			  EXPR_ADD_NETBIOS_LIST    .. adding a list
*			  EXPR_DELETE_NETBIOS_LIST .. deleting a list
*
* Output:
*   Return Code -
*       (None)
*
* Notes:
*
*****************************************************************************/

void
expr_update_expression_tree (
    expr_tree_entry_t   *node_ptr,
    netbios_acclisttype	*netbios_list,
    int			operation)
{
    /*
    **  If there is a left subtree for this mode, walk down it looking
    **  to get to the leaf node.
    */
    if (node_ptr->left_child != NIL_EXPR_TREE_ENTRY)
        expr_update_expression_tree(node_ptr->left_child, netbios_list,
				    operation);

    /*
    **  Free the the ExpressionTree nodes down the right subtree.
    */
    if (node_ptr->right_child != NIL_EXPR_TREE_ENTRY)
        expr_update_expression_tree(node_ptr->right_child, netbios_list,
				    operation);

    /*
    **  We're at a leaf node.  See if this Symbol defines a NetBIOS
    **  Access List that is to be purged.  If it is, then wipe
    **  out the pointer to the access list.
    */
    if (node_ptr->flags & EXPR_TREE_NETBIOS_MASK)
    {
	if (!strcasecmp(node_ptr->access.netbios.name, netbios_list->name))
	{
	    switch (operation)
	    {
		case EXPR_ADD_NETBIOS_LIST:
		    node_ptr->access.netbios.access_list = netbios_list;
		    break;
		case EXPR_DELETE_NETBIOS_LIST:
		    node_ptr->access.netbios.access_list = NULL;
		    break;
    }
	}
    }

    return;
}


/*
**  i s i d e n t i f i e r ( ) 
**
**  Function:
**	Determine if the specified character is to be considered an
**	identifier character.
*/
boolean
isidentifier (char character)
{
    if (isalnum(character)  ||  (character == '_')  ||  (character == '-'))
	return(TRUE);
    else
	return(FALSE);
}

/*
**  e x p r _ g e t _ i d e n t i f i e r ( ) 
**
**  Function:
**	Extract an identifier from the specified character string.
*/
void
expr_get_identifier (
    char	*string,
    int		*index,
    char	*identifier,
    int		maximum)
{
    int		i_string, i_id;

    /*
    **  Scan past any initial whitespace.
    */
    i_string    = *index;
    while (((string[i_string] == ' ')	  ||
            (string[i_string] == '\t'))  	&&  
	    (string[i_string] != '\0')    	&&
	    (string[i_string] != '\n'))
	i_string++;

    if ((string[i_string] == '\0')  ||  (string[i_string] == '\n'))
    {
	identifier[0] = '\0';
	return;
    }

    /*
    **  Copy all characters that match the requirements for identifiers from
    **  'string' into 'identifier'.
    */
    i_id = 0;
    while ((isidentifier(string[i_string]))  &&  (i_id < maximum))
    {
	identifier[i_id++] = string[i_string++];
    }
    identifier[i_id] = '\0';
    *index = i_string;
    return;
}
