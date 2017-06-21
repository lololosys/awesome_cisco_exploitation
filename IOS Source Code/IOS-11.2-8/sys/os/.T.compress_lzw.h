
compress_lzw.h,1456
#define BIT_MASK	47,1638
#define BLOCK_MASK	48,1660
#define BITS 50,1685
#define HSIZE	51,1713
#define INIT_BITS 53,1765
typedef struct compress_private_def compress_private_def56,1884
} compress_private;compress_private87,3128
#define MAGIC_1 91,3179
#define MAGIC_2 92,3200
#define CHECK_GAP 94,3223
#define DUMP_ALL 95,3272
#define n_bits 98,3385
#define last_output_nbits 99,3429
#define need_output_clear 100,3484
#define maxbits 101,3539
#define maxcode 102,3584
#define maxmaxcode 103,3629
#define htab 104,3677
#define codetab 105,3719
#define free_ent 106,3764
#define save_finchar 107,3810
#define save_oldcode 108,3860
#define exit_stat 109,3910
#define block_compress 110,3957
#define clear_flg 111,4009
#define rmask 112,4056
#define lmask 113,4099
#define offset 114,4142
#define size 115,4186
#define buf 116,4228
#define inputbuf 117,4269
#define inputbufend 118,4315
#define outputbuf 119,4364
#define outputbufend 120,4411
#define outputcount 121,4461
#define in_count 122,4510
#define bytes_out 123,4556
#define out_count 124,4603
#define ratio 125,4650
#define checkpoint 126,4693
#define hsize 127,4741
#define MAXCODE(MAXCODE129,4785
#define EOF 131,4835
#define htabof(htabof142,5290
#define codetabof(codetabof143,5316
#define tab_prefixof(tab_prefixof144,5348
#define tab_suffixof(tab_suffixof145,5385
#define de_stack	146,5430
#define FIRST	152,5609
#define	CLEAR	153,5650
