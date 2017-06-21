
mpilib.h,4522
typedef uchar byte;byte56,2116
typedef byte *byteptr;byteptr57,2136
typedef char *string;string58,2181
typedef ushort word16;word1659,2243
typedef ulong word32;word3260,2291
#define min(min63,2395
#define max(max64,2436
#define PORTABLE	68,2525
#define MPORTABLE	69,2585
#define UNIT16	71,2665
#define HIGHFIRST	73,2751
#define UNIT32	77,2871
#define UPTON	83,3073
#define UNIT16	89,3176
typedef unsigned char unit;unit99,3446
typedef signed char signedunit;signedunit100,3474
#define UNITSIZE 101,3506
#define uppermostbit 102,3556
#define BYTES_PER_UNIT 103,3591
#define units2bits(units2bits104,3648
#define units2bytes(units2bytes105,3713
#define bits2units(bits2units106,3740
#define bytes2units(bytes2units107,3777
typedef word16 unit;unit111,3826
typedef short signedunit;signedunit112,3847
#define UNITSIZE 113,3873
#define uppermostbit 114,3924
#define BYTES_PER_UNIT 115,3961
#define units2bits(units2bits116,4018
#define units2bytes(units2bytes117,4083
#define bits2units(bits2units118,4117
#define bytes2units(bytes2units119,4155
typedef word32 unit;unit123,4215
typedef long signedunit;signedunit124,4236
#define UNITSIZE 125,4261
#define uppermostbit 126,4312
#define BYTES_PER_UNIT 127,4354
#define units2bits(units2bits128,4411
#define units2bytes(units2bytes129,4476
#define bits2units(bits2units130,4510
#define bytes2units(bytes2units131,4548
#define power_of_2(power_of_2135,4655
#define bits2bytes(bits2bytes136,4731
typedef unit *unitptr;unitptr149,5482
#define pre_higherunit(pre_higherunit156,5638
#define pre_lowerunit(pre_lowerunit157,5672
#define post_higherunit(post_higherunit158,5705
#define post_lowerunit(post_lowerunit159,5740
#define bit_index(bit_index160,5774
#define lsbptr(lsbptr161,5833
#define make_lsbptr(make_lsbptr162,5872
#define unmake_lsbptr(unmake_lsbptr163,5923
#define msbptr(msbptr164,5975
#define make_msbptr(make_msbptr165,6003
#define rescale(rescale167,6059
#define normalize(normalize168,6119
#define pre_higherunit(pre_higherunit174,6290
#define pre_lowerunit(pre_lowerunit175,6324
#define post_higherunit(post_higherunit176,6357
#define post_lowerunit(post_lowerunit177,6392
#define bit_index(bit_index178,6426
#define lsbptr(lsbptr179,6470
#define make_lsbptr(make_lsbptr180,6498
#define unmake_lsbptr(unmake_lsbptr181,6555
#define msbptr(msbptr182,6603
#define make_msbptr(make_msbptr183,6642
#define rescale(rescale185,6692
#define normalize(normalize186,6745
#define bitptr(bitptr194,7093
#define bitmsk(bitmsk195,7134
#define mp_tstbit(mp_tstbit197,7232
#define mp_setbit(mp_setbit198,7284
#define mp_clrbit(mp_clrbit199,7336
#define msunit(msunit200,7388
#define lsunit(lsunit201,7436
#define mp_tstminus(mp_tstminus203,7545
#define MAX_BIT_PRECISION 209,7773
#define MAX_BYTE_PRECISION 210,7804
#define MAX_UNIT_PRECISION 211,7853
#define set_precision(set_precision217,8036
#define P_SETP 233,8522
#define P_ADDC 234,8544
#define P_SUBB 235,8566
#define P_ROTL 236,8588
#define set_precision(set_precision253,9146
#define mp_addc	254,9218
#define mp_subb	255,9242
#define mp_rotate_left	256,9266
#define mp_compare 263,9425
#define stage_modulus 273,9649
#define mp_modmult 274,9693
#define modmult_burn 275,9728
#define stage_modulus 281,9855
#define mp_modmult 282,9899
#define modmult_burn 283,9934
#define stage_modulus 289,10057
#define mp_modmult 290,10099
#define modmult_burn 291,10132
#define mp_shift_left(mp_shift_left296,10186
#define mp_add(mp_add299,10282
#define mp_sub(mp_sub302,10371
#define mp_abs(mp_abs305,10466
#define msub(msub307,10529
#define testeq(testeq310,10639
#define testne(testne313,10708
#define testge(testge316,10776
#define testle(testle319,10844
#define mp_square(mp_square322,10913
#define mp_modsquare(mp_modsquare325,10999
#define countbytes(countbytes328,11101
#define SLOP_BITS 341,11736
#define MERRITT_KEY	342,11767
#define SLOP_BITS 345,11916
#define SLOP_BITS 349,12049
#define sniff_bit(sniff_bit361,12379
#define init_bitsniffer(init_bitsniffer363,12432
#define bump_bitsniffer(bump_bitsniffer374,12696
#define bump_2bitsniffers(bump_2bitsniffers382,12881
#define stuff_bit(stuff_bit391,13110
#define mp_move(mp_move434,14456
#define unitfill0(unitfill0435,14541
#define mp_burn(mp_burn436,14625
#define mp_init0(mp_init0437,14701
#define mp_burn(mp_burn444,14891
#define mp_init0(mp_init0445,14954
