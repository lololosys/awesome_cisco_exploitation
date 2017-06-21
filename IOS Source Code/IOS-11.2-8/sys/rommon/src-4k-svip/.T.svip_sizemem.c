
svip_sizemem.c,1104
typedef unsigned long long u64;u6438,1231
#define DIM(DIM49,1468
#define S_128KB 51,1515
#define S_256KB 52,1562
#define S_HALFMEG 53,1609
#define S_ONEMEG 54,1653
#define S_TWOMEG 55,1682
#define S_FOURMEG 56,1711
#define S_EIGHTMEG 57,1740
#define S_16MEG 58,1769
#define S_32MEG 59,1799
#define S_64MEG 60,1829
#define S_128MEG 61,1859
#define S_256MEG 62,1889
#define DRAM_INDEX 64,1921
#define SRAM_INDEX 65,1943
#define NVRAM_INDEX 66,1965
#define ORG_TEST_DATA 68,1988
#define NEW_TEST_DATA 69,2025
#define MEM_GOOD 71,2063
#define NO_MEM_ERROR 72,2092
#define MEM_ERROR 73,2121
struct meminfo meminfo81,2326
#define BASE_ADDR	102,2746
struct sramtestdata sramtestdata104,2782
move64 139,3982
int memsizing(153,4207
void svip_setup_PMA_registers 177,4586
sizing_memd(219,5700
long sizing_nvram(228,5797
sizemainmem(303,8252
#define KSEG0_MON_FREE_MEM 313,8546
sizesram(328,9017
sizeshmem(427,12666
mainmemstart(448,13073
shmemstart(454,13164
ismainmemaddr(460,13251
sysmemtest(483,13615
getfreememstart(532,15042
size_nvram(547,15403
meminfo(564,15570
