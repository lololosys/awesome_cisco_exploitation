
clns_parse3.c,2170
#define ISHEXDIGIT(ISHEXDIGIT59,2015
#define CVHTOB(CVHTOB60,2096
#define NOTE_ADDR_ERR_POSN(NOTE_ADDR_ERR_POSN61,2172
#define NOTE_PFX_ERR_POSN(NOTE_PFX_ERR_POSN62,2244
#define ST_START 88,3260
#define ST_HEXDIGITS 89,3298
#define ST_BINDIGITS 90,3350
#define ST_DOTS 91,3405
#define NUMSTATES 93,3447
#define EQ_01 98,3514
#define EQ_29 99,3546
#define EQ_AF 100,3582
#define EQ_STAR 101,3618
#define EQ_DOT 102,3652
#define EQ_LPAR 103,3683
#define EQ_RPAR 104,3725
#define EQ_END 105,3768
#define EQ_BAD 106,3806
#define NUMEQCLASSES 108,3849
struct state_table_entry state_table_entry140,4745
struct working_vars working_vars268,8250
#define curstate 304,9810
#define exit 305,9841
#define lastchar 306,9864
#define endptr 307,9896
#define errorfound 308,9924
#define addr_err_posn 309,9959
#define pfx_err_posn 310,10000
#define someleft 311,10039
#define someright 312,10070
#define founddots 313,10103
#define dotcount 314,10136
#define inbindig 315,10167
#define bincount 316,10198
#define starseen 317,10229
#define embeddedstar 318,10260
#define leftlen 319,10299
#define leftposition 320,10328
#define rightposition 321,10367
#define leftside 322,10408
#define leftmask 323,10439
#define rightside 324,10470
#define rightmask 325,10503
#define getnibble(getnibble327,10537
#define putbit(putbit328,10632
#define putnibble(putnibble329,10735
static void parse_nsap_string 337,11001
static int geteqclass 417,12815
static void enter_start 465,13596
static void enter_hex 476,13888
static void enter_bin 557,15610
static void enter_dots 656,17750
static void startdot 676,18132
static void enddots 686,18302
static void endbinstring 736,19266
static void done 749,19510
static void error 764,19749
static void cerror 770,19819
static boolean is_nsap 779,19943
static boolean is_nsap_pfx 802,20439
static boolean is_masked_nsap 827,20989
static void extract_nsap 837,21184
static void extract_nsap_pfx 848,21425
static void extract_masked_nsap 859,21680
boolean get_nsap_addr 916,23294
boolean get_nsap_pfx 945,23857
boolean get_masked_nsap 973,24350
int get_nsap_structure 1010,25511
