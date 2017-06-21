
avl.h,636
typedef enum balance_type_ balance_type_65,1874
} balance_type;balance_type69,1963
typedef struct avl_node_type_ avl_node_type_71,1980
} avl_node_type;avl_node_type75,2170
enum avl_compare avl_compare77,2188
enum avl_compare {AVL_LT,77,2188
enum avl_compare {AVL_LT, AVL_EQ,77,2188
enum avl_compare {AVL_LT, AVL_EQ, AVL_GT}77,2188
avl_get_quickest 87,2448
typedef boolean (*avl_walker_type)avl_walker_type96,2608
typedef boolean (*avl_walker_type)(avl_node_type *avl_node, va_list ap)ap96,2608
typedef enum avl_compare (*avl_compare_type)avl_compare_type104,2930
					     avl_node_type *node_two)node_two105,3000
