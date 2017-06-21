
c7100_envm.c,2887
} env_reason_t;env_reason_t155,4789
} env_state_t;env_state_t175,5212
typedef char env_name_buf[env_name_buf184,5357
typedef struct temptab_ temptab_198,5752
} temptab_t;temptab_t208,5984
typedef uint voltage_format_t(voltage_format_t251,7040
typedef int voltage_convert_t(voltage_convert_t252,7094
typedef struct volttab_ volttab_257,7176
} volttab_t;volttab_t273,7555
} pstype_t;pstype_t337,8890
typedef struct pstab_ pstab_342,8943
} pstab_t;pstab_t350,9109
typedef struct fantab_ fantab_377,9645
} fantab_t;fantab_t385,9820
static int ds1620_2_celcius 425,10740
static uint c7100_temp_format 446,11050
static int c7100_p3v_convert 461,11314
static uint c7100_p3v_format 473,11508
static int c7100_p5v_convert 487,11768
static uint c7100_p5v_format 499,11962
static int c7100_p12v_convert 512,12224
static int c7100_n12v_convert 523,12392
static uint c7100_p12v_format 535,12595
static uint c7100_n12v_format 550,12893
static void write_ds1620_bit 567,13254
static uint read_ds1620_bits 580,13524
static void write_ds1620_byte 596,13817
static void write_ds1620_cmd 631,14538
static void read_ds1620_byte 660,15228
static void select_mux 697,16090
static uint read_a2d 710,16318
static void save_last_envm_stats 738,16968
static void monitor_shutdown 780,18003
static void get_new_ds1620_temps 820,18911
static void check_ds1620_temps 840,19345
static void measure_deltas 915,21567
static void print_fan_table(930,21936
static void check_fan_status 951,22485
static void get_new_voltage 990,23453
static void check_voltages 1014,23944
static pstype_t get_ps_type 1084,26105
static env_state_t get_ps_state 1102,26475
static void check_ps_id 1119,26789
static boolean get_last_envm_stats 1258,30590
boolean c7100_envmon_interrupt 1310,31824
forktype c7100_envmon_daemon 1341,32570
static void c7100_envmon_midplane_test 1404,34094
static void c7100_envmon_init 1426,34564
#define ENVMON_MAJVERSION 1574,39095
#define ENVMON_MINVERSION 1575,39123
#define ENVMON_EDITVERSION 1576,39151
show_env_all 1595,39555
show_env_default 1702,42029
show_env_table 1779,43621
show_env_last 1817,44866
void show_env 1848,45570
boolean c7100_volt_valid 1880,46080
char* c7100_volt_descr 1889,46276
int c7100_volt_value 1894,46365
uint c7100_volt_lowthresh 1902,46517
uint c7100_volt_highthresh 1910,46669
uint c7100_volt_lastvalue 1918,46823
uint c7100_volt_state 1926,46975
boolean c7100_temp_valid 1938,47361
char* c7100_temp_descr 1947,47557
uint c7100_temp_value 1952,47646
uint c7100_temp_thresh 1960,47791
uint c7100_temp_lastvalue 1968,47935
uint c7100_temp_state 1976,48081
boolean c7100_ps_valid 1989,48483
char* c7100_ps_descr 1995,48634
uint c7100_ps_state 2003,48837
uint c7100_ps_source 2014,49221
boolean c7100_fan_valid 2028,49599
char* c7100_fan_descr 2038,49775
uint c7100_fan_state 2043,49862
