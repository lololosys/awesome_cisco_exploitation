
sizemem.c,1520
#define MMOFFSET 74,2415
#define S_128KB 76,2461
#define S_HALFMEG 77,2509
#define S_ONEMEG 78,2554
#define S_TWOMEG 79,2583
#define S_FOURMEG 80,2612
#define S_EIGHTMEG 81,2641
#define S_16MEG 82,2670
#define S_32MEG 83,2700
#define S_64MEG 84,2730
#define S_128MEG 85,2760
#define S_256MEG 86,2790
#define DRAM_INDEX 88,2822
#define NVRAM_INDEX 89,2844
#define UORG_TEST_DATA 91,2867
#define LORG_TEST_DATA 92,2905
#define UNEW_TEST_DATA 93,2943
#define LNEW_TEST_DATA 94,2981
#define NO_MEM_ERROR 96,3020
#define MEM_ERROR 97,3049
#define MAXBANK 98,3078
#define ENABLE_PARITY(ENABLE_PARITY100,3108
#define DISABLE_PARITY(DISABLE_PARITY107,3411
struct meminfo meminfo122,3857
int memsizing 154,4466
long sizing_sram 179,4842
long sizing_nvram(188,4942
ulong size_and_pack_mem(269,7969
#define DRAM_BANK_BANKWDTH_MASK 330,10332
#define DRAM_BANK_XKRF_MASK 331,10391
void get_gal_sim_para(333,10439
void print_mem_config(459,14567
long sizemainmem 499,16402
long sizeshmem 533,17074
long mainmemstart 551,17417
long shmemstart 559,17531
int ismainmemaddr 567,17642
int sysmemtest 590,18021
long getfreememstart 639,19308
long size_nvram 650,19469
#define PATTERN_1 661,19594
#define PATTERN_2 662,19623
int simple_mem_test(663,19652
int meminfo(716,21028
int slot0_size 810,24039
int slot1_size 815,24089
#define ODD_ONE_PATTERN 826,24216
#define EVEN_ONE_PATTERN 827,24257
void detect_parity 831,24371
void config_dram_parity 903,26451
check_dram_config(949,27833
