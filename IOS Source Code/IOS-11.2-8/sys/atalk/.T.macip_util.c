
macip_util.c,2140
#define DEBUGSWITCH 79,2521
#define MAXDEBUG 82,2548
str160init 99,2865
str32init 127,3455
str32ninit 154,4033
str32cpy 177,4406
str32len 194,4667
str32ptr 207,4848
str32match 219,4993
typedef uchar bitword;bitword239,5316
#define ELEM_SIZE 240,5339
#define ALL_ONES 241,5393
} t_bitarray;t_bitarray248,5703
#define BITMOD_VALID 250,5718
#define BITVAL(BITVAL264,6039
#define BITSET(BITSET267,6131
#define BITCLR(BITCLR270,6211
bit_validate 281,6432
bit_new 306,6921
bit_free 345,7657
bit_nclr 364,7945
bit_nset 384,8284
bit_nbits 404,8629
bit_clrall 419,8854
bit_val 436,9151
bit_set 457,9535
bit_clr 478,9881
bit_next 504,10411
bit_last 564,11851
} t_iddesc;t_iddesc633,13617
#define ID_PATTERN 635,13630
#define ZADJUST(ZADJUST638,13720
#define RANGE_ADJUST(RANGE_ADJUST641,13819
#define INCR_CURSOR(INCR_CURSOR646,13966
#define ADJUST_CURSOR(ADJUST_CURSOR660,14363
id_validate 681,14841
id_delid 704,15289
id_free 752,16354
id_getid 772,16666
id_new 814,17682
id_reset 858,18505
id_first 881,18971
id_next 901,19411
id_last 943,20365
typedef struct symbol_entry_ symbol_entry_963,20648
} symbol_entry;symbol_entry970,20815
#define SYM_HASH_SIZE 972,20832
typedef struct t_symtab_ t_symtab_975,20900
} t_symtab;t_symtab980,21132
#define FIRST_SYM 983,21204
#define LAST_SYM 984,21235
#define MAX_SYM_LEN 988,21344
def_canFunc 1000,21616
sy_hash 1013,21792
mk_canonical 1023,21991
str_lookup 1038,22292
sym_lookup 1070,23054
new_symbol 1097,23637
free_symbol(1163,25121
sy_append 1181,25464
sy_append32 1203,25911
sy_delete 1223,26376
sy_symbol 1270,27508
sy_symbol32 1289,27850
sy_string 1308,28249
sy_init 1326,28563
char *sy_resultStr(1378,29844
sy_destroy 1394,30134
struct dbkey dbkey1430,30805
typedef struct dbkey dbkey;dbkey1437,31016
#define NILDBKEY 1438,31044
#define TABLESIZE 1440,31082
#define ABSURD_CHAIN 1444,31186
hash 1455,31360
db_append 1493,32231
db_retrieve 1564,34082
db_delete 1632,35663
db_replace 1700,37024
db_mkkey 1762,38438
db_init 1790,38963
db_check 1821,39536
db_keyStr 1864,40321
db_resultStr 1888,40717
