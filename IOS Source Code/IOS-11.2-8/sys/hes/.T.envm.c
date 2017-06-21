
envm.c,1282
#define ENV_CTL 129,4612
#define ENV_STS 130,4659
#define ENV_NVRAM 131,4706
static void envm_init 219,7388
static ecshow_t *env_showp_getptr 269,8791
static void env_nv_done 297,9628
static void write_env_nvram 311,10173
static void mark_env_hdr_dirty 323,10573
ecshow_t *read_env_nvram 338,11147
static void handle_env_powerfail 374,12340
static void processor_shutdown 422,14245
static void update_env_stats 448,15092
process env_watch_process 579,19248
int get_ps_bits 614,20223
char *get_power_supply_str 635,20622
static void convert_volts 655,21091
int convert_millivolts 677,21751
static void resolve_temp_warning 702,22485
static void resolve_voltage_warning 780,25444
static void resolve_fan_warning 833,27284
void env_warning_check 885,29021
void show_env 973,31612
void show_power_supply 1100,35178
void show_voltage 1135,36003
void show_temperature 1175,37168
void show_who_shutdown 1192,37550
void show_env_table 1224,38390
ecshow_t *env_get_show 1316,41836
void env_show_str_init 1344,42467
uint env_temp_bounds 1364,43577
int getenvVoltThreshold 1395,44114
int getenvSupplyState 1409,44373
int getenvSupplySource 1432,44940
#define ENVM_MAJVERSION 1469,45763
#define ENVM_MINVERSION 1470,45789
#define ENVM_EDITVERSION 1471,45815
