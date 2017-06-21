
parser_actions.c,5817
typedef struct iftype_ iftype_395,14294
} iftype;iftype406,14670
#define	CHECK_RANGE_BOUNDS(CHECK_RANGE_BOUNDS409,14682
#define	CHECK_RANGE_BOUNDS_ALIST(CHECK_RANGE_BOUNDS_ALIST420,14972
#define PA_HELP_STRING	440,15540
static boolean terminator_check 514,18319
boolean eol_check 519,18428
boolean help_check 524,18520
static boolean long_help_check 529,18627
static boolean short_help_check 535,18779
static boolean priv_check 540,18887
static void keyword_flag_modify 599,20428
#define	PARSER_HTTP_FORM_UNKOWN	616,21025
#define	PARSER_HTTP_FORM_NUMBER	617,21060
#define	PARSER_HTTP_FORM_ADDRESS	618,21095
#define	PARSER_HTTP_FORM_LINE	619,21130
#define	PARSER_HTTP_FORM_WORD	620,21163
static void parser_http_form 622,21197
boolean match_char 661,22117
boolean match_whitespace2 674,22369
boolean get_name 688,22639
void NONE_action 715,23123
void link_point_action 721,23228
void no_alt_action 757,24765
void NOP_action 957,29411
void keyword_action 969,29683
void keyword_optws_action 978,29952
void keyword_nows_action 987,30245
void keyword_mm_action 996,30520
void keyword_id_action 1005,30803
void keyword_id_mm_action 1014,31085
void keyword_orset_action 1023,31385
void keyword_ortest_action 1032,31687
void keyword_trans_action 1041,32005
void general_keyword_action 1052,32307
void keyword_option_action 1171,35633
#define BEFORE_HW_INTS 1260,38049
#define HW_INT 1261,38075
#define AFTER_HW_INTS 1262,38101
#define NEVER_DISPLAY 1263,38127
static hwidbtype *find_hwidb_by_name 1319,40640
static idbtype *find_interface 1374,41842
static void do_interface_parse 1451,43665
static void interface_parse 1490,44671
static void hw_interface_parse 1535,45991
void interface_action 1564,46624
void whitespace_action 1848,55489
void print_action 1875,56151
void eol_action 1896,56697
void eols_action 1973,58754
void eolns_action 1980,58909
void alt_help_action 1990,59129
void alt_helps_action 2007,59519
void general_number_short_help 2014,59696
void general_number_short_help_http 2020,59856
void general_number_action 2101,62419
void signed_number_action 2153,63541
void number_func_action 2196,64460
void number_help_func_action 2214,64943
void help_action 2236,65477
void display_action 2265,66539
typedef struct line_keyword_ line_keyword_2277,66897
} line_keyword;line_keyword2282,67004
mark_range 2303,67712
void ttyline_action 2372,69031
void hub_action 2517,72436
void char_action 2559,73393
void char_number_action 2595,74185
void func_action 2666,76264
void set_action 2683,76743
void test_action 2694,77064
void test_boolean_action 2710,77447
void test_int_action 2721,77685
void test_func_action 2732,77922
void test_line_action 2743,78161
void test_idbstatus_action 2782,78912
void general_string_action 2800,79384
void nvgen_action 2855,80758
void nvgens_action 2875,81122
void nvgenns_action 2882,81285
void nvgen_token 2898,81789
void noprefix_action 2936,82687
void params_action 2957,83128
void params_action_alist 3084,86172
boolean match_minusone 3224,89655
boolean match_decimal 3236,89867
boolean match_octal 3274,90528
boolean match_hexadecimal 3310,91153
boolean match_ulong_hexadecimal 3344,91777
boolean match_ulong_octal 3381,92436
boolean match_ulong_decimal 3421,93230
static boolean match_signed_dec 3479,94583
void snpaaddr_action 3537,95690
void router_action 3681,99369
static boolean process_partial_protocol_action 3763,101704
boolean match_partial_keyword 3911,105240
static void string_escape 3988,107455
uint copy_varstring 4065,108865
void grouplist_action 4118,109940
static boolean match_grouplist_item 4155,110848
static boolean match_grouplist 4237,112867
static boolean match_spaces 4261,113324
static int match_hex_digits 4284,113792
static char *hex_to_byte 4307,114290
void hexdata_action 4329,114606
void literal_char_action 4379,115854
void help_or_error 4398,116305
static void set_error_index 4411,116633
static void match_number_hex 4418,116772
static void match_number_oct 4509,119059
static void match_number_dec 4592,121019
boolean match_number 4648,122406
#define	MATCH_NUMBER_SET(MATCH_NUMBER_SET4696,123842
#define	MAX3(MAX34701,123942
void generic_addr_noflag_action 4866,128602
static boolean match_general_addr 4874,128830
void generic_addr_action 4925,130126
static boolean match_one_spec 4954,130920
static boolean match_link 4978,131767
static int match_protocol 5001,132529
void protoname_action 5069,134358
void protoaddr_action 5090,134801
void multiple_funcs_action 5133,135788
void save_ambig_string 5144,136048
static void save_ambig_nstring 5149,136178
static void save_ambig_int 5174,136625
static void save_ambig_uint 5182,136799
static void save_ambig_string_flag 5190,136975
static void save_help_msg 5239,138182
static void save_help_short 5246,138339
void encode_str 5256,138658
void save_help_long 5276,139025
static int match_transport 5313,140153
void transport_action 5368,141370
static boolean match_subinterface 5402,142243
static boolean idb_name_match 5449,143539
static boolean match_interface_unit 5467,143903
static boolean match_subcard_in_slot 5603,148349
static void parser_bail_out 5627,148781
void print_tokenQ 5642,149160
void priv_action 5661,149752
void mode_action 5678,150170
void month_action 5696,150575
void day_action 5726,151193
void timesec_action 5755,151796
void bgp_community_action 5804,153018
void comment_action 5839,153900
void testexpr_action 5847,154062
static boolean match_type_a_char 5872,154632
static boolean match_pcid 5882,154855
void appn_fqname_action 5894,155094
void appn_name_star_action 5973,157086
void appn_type_a_name_action 6135,161214
void appn_type_a_name_star_action 6191,162575
void appn_pcid_action 6252,164039
