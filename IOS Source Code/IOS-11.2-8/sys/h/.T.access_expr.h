
access_expr.h,1728
#define EXPR_H26,843
#define	MAX_EXPR_SZ	62,2015
#define	MAX_EXPR_STACK_ENTRIES	63,2040
#define	MAX_EXPR_OPER_ENTRIES	64,2074
#define	MAX_EXPR_SHOW_BUFFER	65,2107
#define	MAX_EXPR_IDENTIFIER	66,2140
typedef enum expr_state_expr_state_71,2223
} expr_state;expr_state78,2460
#define	EXPR_DBG_EXPRESSION_TREE	84,2568
#define	EXPR_DBG_OPERATOR_STACK	85,2615
#define	EXPR_DBG_RETURN_CODE	86,2662
#define	EXPR_DBG_SYSTEM_CALLS	87,2706
#define	EXPR_OK	100,3146
#define	EXPR_UNKNOWN_SYMBOL	101,3166
#define	EXPR_BAD_EXPRESSION	102,3196
#define	EXPR_LOGIC_ERROR	103,3226
#define	EXPR_NO_MEMORY	104,3253
#define	EXPR_INVALID_LIST	105,3279
#define	EXPR_UNARY	112,3456
#define	EXPR_BINARY	113,3477
#define	EXPR_TREE_OPERATOR	118,3540
#define	EXPR_TREE_ACCESS_LIST_LSAP	119,3573
#define	EXPR_TREE_ACCESS_LIST_TYPE	120,3613
#define	EXPR_TREE_ACCESS_LIST_SMAC	121,3653
#define	EXPR_TREE_ACCESS_LIST_DMAC	122,3693
#define	EXPR_TREE_NETBIOS_HOST	123,3733
#define	EXPR_TREE_NETBIOS_BYTES	124,3770
#define	EXPR_TREE_NETBIOS_MASK 126,3809
#define	EXPR_TREE_SYMBOL_MASK	128,3848
#define	EXPR_ADD_NETBIOS_LIST	134,3983
#define	EXPR_DELETE_NETBIOS_LIST	135,4016
typedef struct expr_netbios_entryexpr_netbios_entry140,4108
} expr_netbios_entry_t;expr_netbios_entry_t144,4226
struct expr_tree_entry_t_expr_tree_entry_t_146,4251
#define	NIL_EXPR_TREE_ENTRY	161,4530
typedef struct expr_stack_entryexpr_stack_entry163,4582
} expr_stack_entry_t;expr_stack_entry_t168,4687
typedef struct expr_operator_entryexpr_operator_entry170,4710
} expr_operator_entry_t;expr_operator_entry_t176,4848
typedef struct expr_category_entryexpr_category_entry178,4874
} expr_category_entry_t;expr_category_entry_t182,4945
