
mlp.h,2174
#define __MLP_H__130,4757
#define MLP_SEQ_MASK_24BITS 133,4810
#define MLP_SEQ_RANGE_24BITS 134,4850
#define MLP_BEGBIT_MASK 135,4904
#define MLP_ENDBIT_MASK 136,4939
#define MAX_MLP_BUNDLES 138,4975
#define MLP_LOST_FRAG_TIME 140,5003
#define MLP_UPDATE_DIALER_IDLE_INTERVAL 142,5043
#define MLP_MAX_IDLE_TIMEOUT 143,5097
#define MLP_DEFAULT_LOAD_ANALYZE_TIME 144,5153
#define MLP_DEFAULT_WAIT_FOR_ADD_TIME 145,5206
#define MLP_DEFAULT_WAIT_BEFORE_DISCONNECT_TIME 146,5259
#define MLP_WAIT_BUNDLE_APPOINTMENT 147,5320
#define MLP_LOST_FRAG_TIMER 153,5426
#define MLP_LOST_FRAG_RFC 154,5464
#define MLP_LOST_FRAG_UNA_GT_LINKS 155,5502
#define MLP_LOST_FRAG_MAX_BUFFERS 156,5540
#define MLP_LOST_FRAG_EXP 162,5721
#define MLP_DEF_MIN_UNA_SIZE 163,5759
#define MLP_SEQ_WINDOW_SIZE_24BITS 166,5800
#define MLP_DEFAULT_MIN_FRAG_SIZE 168,5844
#define MLP_INIT_SEQ 170,5884
#define DEF_MULTILINK_MRRU 172,5912
#define MLP_24BITSEQ_HDR_SIZE 173,5945
enum MLP_BUNDLE_STATE MLP_BUNDLE_STATE175,6022
    MLP_BUNDLE_DOWN,176,6046
    MLP_BUNDLE_ACTIVE,177,6111
    MLP_BUNDLE_REMOVE,178,6180
    MLP_BUNDLE_CREATING,179,6247
    MLP_BUNDLE_APPOINTED181,6347
struct mlplinktype_ mlplinktype_184,6376
struct mlpbundletype_ mlpbundletype_199,6859
inline static ulong mlp_get_pak_seq 247,9446
inline static boolean mlp_get_beg_bit 252,9565
inline static boolean mlp_get_end_bit 259,9687
inline static void mlp_set_beg_and_end_bits 266,9809
inline static void mlp_set_beg_bit 273,9941
inline static void mlp_set_end_bit 280,10044
inline static ulong mlp_inc_sequence 287,10147
inline static ulong mlp_add_sequence 294,10266
inline static void mlp_put_frag_seq 305,10484
inline static ushort mlp_get_seq_size 317,10751
#define FOR_ALL_BUNDLES(FOR_ALL_BUNDLES327,10907
#define FOR_ALL_LINKS_IN_BUNDLE(FOR_ALL_LINKS_IN_BUNDLE335,11069
#define FOR_ALL_FRAGMENTS_IN_PACKET(FOR_ALL_FRAGMENTS_IN_PACKET342,11239
static inline boolean is_mlp_links_greater_than_one 352,11492
static inline hwidbtype * mlp_get_master_idb 364,11748
inline static ulong mlp_frag_distance 373,11947
inline static boolean mlp_is_frag_outside_expected_seq 396,12569
