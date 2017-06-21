
radix.h,1421
#define	_RADIX_H_130,5464
#define	__P(__P133,5499
#define	__P(__P135,5522
#define	RN_MAX_KEYLEN	144,5718
struct radix_node radix_node150,5791
#define RNF_NORMAL	156,6060
#define RNF_ROOT	157,6115
#define RNF_ACTIVE	158,6168
#define rn_dupedkey 179,6745
#define rn_key 180,6790
#define rn_mask 181,6825
#define rn_off 182,6862
#define rn_l 183,6897
#define rn_r 184,6928
extern struct radix_mask radix_mask190,7041
#define rm_mask 202,7431
#define rm_leaf 203,7463
#define MKGet(MKGet205,7535
#define MKFree(MKFree212,7702
typedef int (*rn_walk_function)rn_walk_function214,7777
typedef int (*rn_walk_function)(struct radix_node *, va_list ap)ap214,7777
typedef struct radix_node * (*rn_succ_function)rn_succ_function215,7843
typedef struct radix_node * (*rn_succ_function)(struct radix_node *, caddr_t)caddr_t215,7843
typedef struct radix_node * (*rn_succ_ver_function)rn_succ_ver_function216,7922
						    u_long, caddr_t)caddr_t217,7995
#define	RN_TREE_AREA	218,8023
struct radix_node_head radix_node_head220,8064
#define Bcmp(Bcmp258,9766
#define Bcopy(Bcopy259,9828
#define Bzero(Bzero260,9902
#define R_Malloc(R_Malloc261,9948
#define Free(Free262,10010
#define Bcmp(Bcmp264,10041
#define Bcopy(Bcopy265,10115
#define Bzero(Bzero266,10191
#define R_Malloc(R_Malloc267,10242
#define Free(Free268,10327
rn_nextleaf 304,11687
rn_nextleaf_version 323,12140
