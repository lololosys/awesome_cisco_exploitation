
arap_smartbuf.h,997
#define OUR_ARAP_SENDBUFFERSIZE	26,784
#define OUR_ARAP_RECVBUFFERSIZE	27,830
#define SFLAG_FIXUP	30,918
#define SFLAG_PKTDATA	31,958
#define SFLAG_TOKENIZED	32,1017
#define SFLAG_LASTGROUP	33,1068
#define SFLAG_RANGE	34,1123
#define SFLAG_WANTSEQD	35,1170
#define SFLAG_WANTPKTSEQD	36,1227
#define SFLAG_RESERVED	37,1287
    SMARTBUF_INPUT,40,1338
    SMARTBUF_OUTPUT41,1358
typedef struct sb_s sb_s47,1458
#define GET_PTR(GET_PTR50,1565
#define PUT_PTR(PUT_PTR51,1622
#define GET_PTR(GET_PTR54,1752
#define PUT_PTR(PUT_PTR55,1775
} sb_t;sb_t60,1951
typedef struct ring_s ring_s62,1960
} ring_t;ring_t84,2563
#define get_compact_num(get_compact_num86,2574
#define put_compact_num(put_compact_num90,2732
#define put_seq_num(put_seq_num94,2882
#define ring_size_pad(ring_size_pad100,2998
#define ring_size_pad(ring_size_pad103,3094
#define ring_size_pad(ring_size_pad108,3182
#define advance_ring_ptr(advance_ring_ptr111,3256
} config_t;config_t117,3401
