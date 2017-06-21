
ip_localpool.c,1636
typedef struct ippool_pooltype_ ippool_pooltype_135,5086
} pool_type;pool_type141,5373
typedef struct pooladdr_type_ pooladdr_type_149,5576
} pooladdr_type;pooladdr_type155,5925
#define FOR_ALL_IPLOCALPOOL_ADDR(FOR_ALL_IPLOCALPOOL_ADDR185,7108
#define FOR_ALL_POOLS(FOR_ALL_POOLS190,7258
void ip_localpool_command 211,7703
static void purge_pool_addresses 334,10173
static inline void purge_pool_free_addresses 345,10373
static inline void purge_pool_used_addresses 365,10846
static pooladdr_type *find_localpool_addr_addr 381,11147
static pooladdr_type *find_localpool_addr_idb 399,11448
static pooladdr_type *find_localpool_addr_ustring 421,11928
static boolean ippool_add 439,12273
static void ippool_remove 520,13890
static int nmember 533,14103
static void print_pool_state 555,14480
static void show_all_pools 565,14695
static void show_ip_localp 587,15080
static void mark_pooladdr_used 609,15498
static void mark_pooladdr_free 625,15924
static void ip_localpool_chown 649,16387
static void ip_localpool_free 662,16636
static int ip_localpool_allocate 690,17444
static void print_ip_localp_pool 834,21491
static void print_ip_localpool_addr 869,22158
static pool_type *find_pool_name 881,22452
ip_pool_exists 894,22656
static void ip_localpool_shutdown 902,22763
static void ip_localpool_startup 909,22840
#define	ALTERNATE	916,22937
#define	ALTERNATE	924,23103
#define	ALTERNATE	932,23277
#define	ALTERNATE	940,23456
static void ip_localpool_init 961,24105
#define IP_LOCALPOOL_MAJVERSION 973,24460
#define IP_LOCALPOOL_MINVERSION 974,24494
#define IP_LOCALPOOL_EDITVERSION 975,24528
