/* $Id: access_expr.h,v 3.2 1995/11/17 09:11:41 hampton Exp $
 * $Source: /swtal/cherf.111/ios/sys/h/access_expr.h,v $
 *------------------------------------------------------------------
 * expr.h -- compile-time values for the expression analyzer program.
 *
 * October 1991, Wayne Clark
 *
 * Copyright (c) 1991-1995 by cisco Systems, Inc.
 * All rights reserved.
 *------------------------------------------------------------------
 * $Log: access_expr.h,v $
 * Revision 3.2  1995/11/17  09:11:41  hampton
 * Remove old entries from the RCS header logs.
 *
 * Revision 3.1  1995/11/09  11:34:42  shaker
 * Bump version numbers from 2.x to 3.x.
 *
 * Revision 2.1  1995/06/07  20:34:30  hampton
 * Bump version numbers from 1.x to 2.x.
 *
 *------------------------------------------------------------------
 * $Endlog$
 */

#ifndef EXPR_H
#define EXPR_H
/******************************************************************************
*  expr.h
*
*  File Description:
*	This include file defines various compile-time values for the
*	expression analyzer program.
*
*  Structures in this file:
*	expr_state		enumerated list of Expression states
*	expr_tree_entry		entry in the ExpressionTree
*	expr_stack_entry	entry in the ExpressionStack
*	expr_operator_entry	entry in the OperatorStack
*	expr_category_entry	entry in the list of categories
*
*****************************************************************************/

/*
**   D e p e n d e n t   I n c l u d e   F i l e s
*/

/*
**   C o n s t a n t   D e f i n i t i o n s
*/
/*
**  Various Allocation Sizes:
**	MAX_EXPR_SZ		maximum number of bytes in Expression
**	MAX_EXPR_STACK_ENTRIES	maximum number of expr_stack_entry in 
**				the ExpressionStack
**	MAX_EXPR_OPER_ENTRIES
**				maximum number of expr_operator_stack entries
**				during expr_evaluate_tree(). This is the
**				maximum ExpressionTree depth - 1.
**	MAX_EXPR_SHOW_BUFFER	maximum length of Expression display buffer
**	MAX_EXPR_IDENTIFIER	maximum length of an Expression identifier
*/
#define	MAX_EXPR_SZ		100
#define	MAX_EXPR_STACK_ENTRIES	30
#define	MAX_EXPR_OPER_ENTRIES	10
#define	MAX_EXPR_SHOW_BUFFER	120
#define	MAX_EXPR_IDENTIFIER	NAMEDTHINGNAMELEN

/*
**  Floyd-Evans State Values:
*/
typedef enum expr_state_
{
    E0 = 1,	/* expression initial state		*/
    T0 = 2,	/* term initial state			*/
    P1 = 3,	/* prime reduction state		*/
    T1 = 4,	/* term reduction state			*/
    E1 = 5	/* expression reduction state		*/
} expr_state;

/*
**  expr_debug Values:
**	N.B. These are bit definitions so that they can be combined.
*/
#define	EXPR_DBG_EXPRESSION_TREE	(uchar)(0x01)
#define	EXPR_DBG_OPERATOR_STACK		(uchar)(0x02)
#define	EXPR_DBG_RETURN_CODE		(uchar)(0x04)
#define	EXPR_DBG_SYSTEM_CALLS		(uchar)(0x08)

/*
**  expr_analyze() Return Code Values:
**	EXPR_OK			no errors; keep on trucking
**	EXPR_UNKNOWN_SYMBOL	expr_get_token() encountered an unknown Symbol
**	EXPR_BAD_EXPRESSION	expr_analyze() encountered a bad
**				expression
**	EXPR_LOGIC_ERROR	heaven forbid, there has been a logic error
**				detected
**	EXPR_NO_MEMORY		malloc() failed
**	EXPR_INVALID_LIST	invalid access list specified
*/
#define	EXPR_OK			0
#define	EXPR_UNKNOWN_SYMBOL	1
#define	EXPR_BAD_EXPRESSION	2
#define	EXPR_LOGIC_ERROR	3
#define	EXPR_NO_MEMORY		4
#define	EXPR_INVALID_LIST	5

/*
**  expr_append_tree() Category Values:
**	EXPR_UNARY	a unary reduction is about to occur
**	EXPR_BINARY	a binary reduction is about to occur
*/
#define	EXPR_UNARY	1
#define	EXPR_BINARY	2

/*
**  expr_tree_entry.flags Values:
*/
#define	EXPR_TREE_OPERATOR		0x01
#define	EXPR_TREE_ACCESS_LIST_LSAP	0x02
#define	EXPR_TREE_ACCESS_LIST_TYPE	0x04
#define	EXPR_TREE_ACCESS_LIST_SMAC	0x08
#define	EXPR_TREE_ACCESS_LIST_DMAC	0x10
#define	EXPR_TREE_NETBIOS_HOST		0x20
#define	EXPR_TREE_NETBIOS_BYTES		0x40

#define	EXPR_TREE_NETBIOS_MASK 		0x60

#define	EXPR_TREE_SYMBOL_MASK		0x7E

/*
**  Operations upon the ExpressionTree performed by 
**  expr_update_netbios_access_list().
*/
#define	EXPR_ADD_NETBIOS_LIST		1
#define	EXPR_DELETE_NETBIOS_LIST	2

/*
**  T y p e d e f s   a n d   S t r u c t u r e s
*/
typedef struct expr_netbios_entry
{
    char			name[NAMEDTHINGNAMELEN];
    netbios_acclisttype	        *access_list;
} expr_netbios_entry_t;

struct expr_tree_entry_t_
{
    expr_tree_entry_t		*left_child;
    expr_tree_entry_t		*right_child;
    int				left_depth;
    int				right_depth;
    int				flags;
    union
    {
	int			access_list_number;
	expr_netbios_entry_t    netbios;
	char			operator;
    } access;
};

#define	NIL_EXPR_TREE_ENTRY	(expr_tree_entry_t *)0

typedef struct expr_stack_entry
{
    expr_tree_entry_t	*node_pointer;
    uint		value;
    char		token;
} expr_stack_entry_t;

typedef struct expr_operator_entry
{
    expr_tree_entry_t	*node_pointer;
    int			visit_count;
    boolean		result;
    char		operator;
} expr_operator_entry_t;

typedef struct expr_category_entry
{
    char	*keyword;
    int		flag;
} expr_category_entry_t;

/*
**  P r o t o t y p e   D e c l a r a t i o n s 
*/

/*
 * In <os/access_expr.c>
 */
extern	int	expr_analyze(char *, expr_tree_entry_t **);
extern	int	expr_get_token(char *, int *, expr_stack_entry_t *, int);
extern	int	expr_reduce_stack(expr_stack_entry_t *, int);
extern	int	expr_allocate_node(expr_tree_entry_t **);
extern	int	expr_compute_depths(expr_tree_entry_t *);
extern	int	expr_evaluate_tree(uchar *, ulong, int, hwidbtype *,
				    expr_tree_entry_t *, boolean *);
extern	boolean	expr_determine_symbol_value(uchar *, ulong, int,
					    expr_tree_entry_t *, boolean *);
extern void     expr_dump_expression_tree (expr_tree_entry_t *);
extern	void	expr_free_expression_tree(expr_stack_entry_t *, int);
extern	void	expr_free_node(expr_tree_entry_t *);
extern	void	expr_show_expression_tree(expr_tree_entry_t *);
extern	void	expr_update_netbios_access_list(netbios_acclisttype *, int);
extern	void	expr_update_expression_tree(expr_tree_entry_t *, 
					     netbios_acclisttype *, int);
extern	boolean	isidentifier(char);
extern	void	expr_get_identifier(char *, int *, char *, int);
extern	boolean	expr_lookup_category(char *, int *);

#endif		/* EXPR_H */
