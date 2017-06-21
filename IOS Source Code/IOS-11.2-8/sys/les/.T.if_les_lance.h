
if_les_lance.h,4133
typedef struct lance_instance lance_instance_t;lance_instance_t55,2100
typedef struct lance_initblock lance_initblock_t;lance_initblock_t56,2149
typedef struct lance_rx_ring lance_rx_ring_t;lance_rx_ring_t57,2199
typedef struct lance_rx_descriptor lance_rx_descriptor_t;lance_rx_descriptor_t58,2245
typedef struct lance_tx_descriptor lance_tx_descriptor_t;lance_tx_descriptor_t59,2303
typedef struct lance_tx_ring lance_tx_ring_t;lance_tx_ring_t60,2361
typedef void (* get_regaddr_t)get_regaddr_t65,2451
typedef void (* get_regaddr_t) (hwidbtype hwidbtype65,2451
typedef void (* dev_reset_t)dev_reset_t66,2497
typedef void (* dev_reset_t) (hwidbtype hwidbtype66,2497
typedef void (* set_vectors_t)set_vectors_t67,2541
typedef void (* set_vectors_t) (hwidbtype hwidbtype67,2541
typedef void (* show_structure_info_t)show_structure_info_t68,2587
typedef void (* show_structure_info_t) (hwidbtype hwidbtype68,2587
typedef void (* pre_init_controller_t)pre_init_controller_t69,2641
typedef void (* pre_init_controller_t) (hwidbtype hwidbtype69,2641
typedef void (* post_init_controller_t)post_init_controller_t70,2695
typedef void (* post_init_controller_t) (hwidbtype hwidbtype70,2695
typedef void (* delay_vector_t)delay_vector_t71,2750
#define LANCE_CSR0 88,3093
#define	LANCE_CSR0_ERR	89,3114
#define	LANCE_CSR0_BABL	90,3174
#define	LANCE_CSR0_HEART	91,3235
#define	LANCE_CSR0_MISS	92,3300
#define	LANCE_CSR0_MERR	93,3361
#define LANCE_CSR0_RINT	94,3412
#define	LANCE_CSR0_TINT	95,3469
#define	LANCE_CSR0_IDON	96,3529
#define	LANCE_CSR0_INTR	97,3587
#define	LANCE_CSR0_INEA	98,3640
#define LANCE_CSR0_RXON	99,3695
#define LANCE_CSR0_TXON	100,3757
#define LANCE_CSR0_TDMD	101,3822
#define	LANCE_CSR0_STOP	102,3878
#define	LANCE_CSR0_STRT	103,3943
#define	LANCE_CSR0_INIT	104,3992
#define LANCE_CSR0_ERRBITS	105,4041
#define LANCE_CSR0_NORMAL	106,4150
#define LANCE_CSR1 117,4580
#define LANCE_CSR2 118,4601
#define LANCE_CSR3 124,4707
#define	LANCE_CSR3_BSWP	125,4728
#define	LANCE_CSR3_ACON	126,4776
#define	LANCE_CSR3_BCON	127,4826
#define LANCE_CSR3_NORMAL	129,4878
struct lance_rx_descriptor lance_rx_descriptor135,4983
struct lance_rx_ring lance_rx_ring143,5305
#define	LANCE_RMD_OWN	147,5380
#define LANCE_RMD_ERR	148,5439
#define LANCE_RMD_FRAM	149,5484
#define LANCE_RMD_OFLO	150,5533
#define LANCE_RMD_CRC	151,5586
#define LANCE_RMD_BUFF	152,5630
#define LANCE_RMD_STP	153,5684
#define LANCE_RMD_ENP	154,5750
#define LANCE_RMD_ERRBITS 155,5815
struct lance_tx_descriptor lance_tx_descriptor157,5894
struct lance_tx_ring lance_tx_ring165,6193
#define	LANCE_TMD_OWN	169,6268
#define LANCE_TMD_ERR	170,6326
#define LANCE_TMD_MORE	172,6401
#define LANCE_TMD_ONE	173,6468
#define LANCE_TMD_DEF	174,6511
#define LANCE_TMD_STP	175,6557
#define LANCE_TMD_ENP	176,6622
#define LANCE_TMD3_BUFF	178,6687
#define LANCE_TMD3_UFLO	179,6740
#define LANCE_TMD3_LCOL	181,6810
#define LANCE_TMD3_LCAR	182,6862
#define LANCE_TMD3_RTRY	183,6915
#define LANCE_TMD3_TDR	184,6973
#define LANCE_TMD3_ERRBITS 185,7019
#define LANCE_MODE_PROM	191,7159
#define LANCE_MODE_INTL	192,7214
#define LANCE_MODE_DRTY	193,7275
#define LANCE_MODE_COLL	194,7327
#define LANCE_MODE_DTCR	195,7381
#define LANCE_MODE_LOOP	196,7434
#define LANCE_MODE_DTX	197,7498
#define LANCE_MODE_DRX	198,7559
struct lance_initblock lance_initblock204,7666
typedef struct lance_vectors lance_vectors_t;lance_vectors_t224,8461
struct lance_vectors lance_vectors225,8507
#define LANCE_BUFF_LO(LANCE_BUFF_LO240,8934
#define LANCE_BUFF_HI(LANCE_BUFF_HI241,8996
#define LANCE_GET_HI(LANCE_GET_HI243,9065
#define LANCE_GET_NEG_W(LANCE_GET_NEG_W244,9118
#define LANCE_DS_SIZE 246,9171
#define LANCE_IB_SIZE 247,9224
#define LANCE_RX_SIZE 248,9274
#define LANCE_TX_SIZE 249,9346
#define LANCE_MAKE_PTR(LANCE_MAKE_PTR251,9419
#define MAX_LANCE 253,9478
#define LANCE_CRC_SIZE 254,9501
#define LANCE_MAX_SPIN 256,9527
#define LANCE_16M_PAGE(LANCE_16M_PAGE262,9604
#define LANCE_WRITE_RMD(LANCE_WRITE_RMD267,9716
#define LANCE_READ_RMD(LANCE_READ_RMD273,9973
