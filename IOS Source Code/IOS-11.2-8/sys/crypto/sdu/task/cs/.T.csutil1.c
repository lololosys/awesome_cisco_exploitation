
csutil1.c,1765
#define CSUTIL1_C302,10975
void swap_l2_da_sa(455,16917
void build_cs_frm_l2_addr(479,17704
static void check_to_swap_llc(498,18368
void swap_l3_da_sa(524,19115
void build_cs_frm_l3_addr(585,20690
short alloc_pconn(640,22526
void free_pconn(731,25586
#define LONGEST_STR_LENGTH	813,27599
#define CS_STR_ERR_NO_TX_BUF	814,27630
#define CS_STR_ERR_NO_PCONN_ENTRY	815,27685
#define CS_STR_ERR_PCONN_TIMEOUT	816,27749
#define CS_STR_ERR_NNC_EXP	817,27818
#define CS_STR_ERR_NNC_SIG	818,27881
#define CS_STR_ERR_SU_SIG	819,27937
#define CS_STR_ERR_PCONN_INDEX_LOST	820,28007
#define CS_STR_ERR_CHALLENGE_ODD	821,28075
#define CS_STR_ERR_CHALLENGE_CMP	822,28146
#define CS_STR_ERR_NNC_GRPS	823,28217
#define CS_STR_ERR_GEN_CHL	824,28290
#define CS_STR_ERR_GEN_DH	825,28350
#define CS_STR_ERR_GEN_SIG	826,28419
#define CS_STR_ERR_SOK_MASK_RNG	827,28479
#define CS_STR_ERR_VERIFY_SIG	828,28557
#define CS_STR_ERR_NNC_FRM_SIZE	829,28629
#define CS_STR_ERR_TRANSMIT	830,28702
#define CS_STR_ERR_NNC_SIZE	831,28769
#define CS_STR_ERR_UNKNOWN	832,28818
static void Print_auth_error 840,29136
void Error_proc(923,31677
void init_l3_l4_ip(1010,34070
void build_ip_header(1051,35531
void init_l3_l4_idp_ipx(1080,36520
void init_l3_l4_ddp(1112,37467
void build_cs_msg_req(1152,38809
void build_cs_msg_rpy(1256,42153
void build_cs_msg_cfm(1495,48651
short srch_pconn_index(1569,50988
ushort get_ip_checksum(1602,51983
uchar handle_nnc_req_revisions(1632,53020
int bad_nnc_frm_size(1748,56744
void  cs_transmit_frm(1824,58634
ushort	get_nnc_size(1872,59561
void post_err_iomgr(1909,60581
static void set_cs_frm_nw_addr(1943,61461
void genDHshareXorChallenge(1982,62365
void xor_challenge(2064,64743
uchar *umSduGetDssPublic(2118,66094
