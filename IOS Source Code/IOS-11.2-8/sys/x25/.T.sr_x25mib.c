
sr_x25mib.c,2448
void init_x25mib 175,6068
VarBind *x25_get 215,7309
void x25_free 266,8624
static int x25_cleanup 276,8886
int x25_undo 289,9267
int x25_test 309,9978
int x25_set 378,11774
VarBind *x25AdmnEntry_get 397,12359
void x25AdmnEntry_free 618,18091
static int x25AdmnEntry_cleanup 637,18674
int x25AdmnEntry_undo 650,19091
int x25AdmnEntry_test 670,19815
int x25AdmnEntry_set 1013,29888
VarBind *x25OperEntry_get 1032,30506
VarBind *x25StatEntry_get 1262,36607
VarBind *x25ChannelEntry_get 1492,42701
void x25ChannelEntry_free 1611,45422
static int x25ChannelEntry_cleanup 1621,45720
int x25ChannelEntry_undo 1634,46149
int x25ChannelEntry_test 1654,46868
int x25ChannelEntry_set 1817,51214
VarBind *x25CircuitEntry_get 1836,51832
void x25CircuitEntry_free 2047,57757
static int x25CircuitEntry_cleanup 2072,58571
int x25CircuitEntry_undo 2085,59000
int x25CircuitEntry_test 2105,59719
int x25CircuitEntry_set 2330,66698
VarBind *x25ClearedCircuitEntry_get 2349,67323
VarBind *x25CallParmEntry_get 2501,71825
void x25CallParmEntry_free 2760,79426
static int x25CallParmEntry_cleanup 2809,80983
int x25CallParmEntry_undo 2822,81416
int x25CallParmEntry_test 2842,82138
int x25CallParmEntry_set 3322,97494
static boolean snmp_x25_idb_tester 3346,98100
static int compute_max_active_vcs 3351,98196
static int compute_num_pvcs 3384,99353
x25_t *k_x25_get 3409,99775
int k_x25_set 3425,100125
x25AdmnEntry_t *k_x25AdmnEntry_get 3432,100253
int k_x25AdmnEntry_set 3503,102935
x25OperEntry_t *k_x25OperEntry_get 3510,103071
x25StatEntry_t *k_x25StatEntry_get 3578,105661
x25ChannelEntry_t *k_x25ChannelEntry_get 3656,108741
int k_x25ChannelEntry_set(3682,109634
static boolean snmp_lci_idb_tester 3689,109832
x25CircuitEntry_t *k_x25CircuitEntry_get 3703,110139
int k_x25CircuitEntry_set 3922,117409
x25ClearedCircuitEntry_t *k_x25ClearedCircuitEntry_get 3929,117563
x25CallParmEntry_t *k_x25CallParmEntry_get 3965,118950
int k_x25CallParmEntry_set 4017,121091
#define RESTART_VBCOUNT 4039,121868
#define RESTART_TRAPNUM 4040,121894
static void generate_x25Restart_trap 4046,122025
#define RESET_VBCOUNT 4082,122920
#define RESET_TRAPNUM 4083,122944
static void generate_x25Reset_trap 4090,123132
static ifmib_err_t x25_subif_cntr32fn 4118,123730
static void x25_add_subif(4194,125460
#define MAJVERSION_x25mib 4251,126849
#define MINVERSION_x25mib 4252,126877
#define EDITVERSION_x25mib 4253,126905
