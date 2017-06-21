
as5200_nv.c,405
#define ALTERNATE 48,1414
#define NVRAM_MAGIC_OFFSET 62,1663
#define NVRAM_START_OFFSET 63,1744
long get_sys_nvsize 69,1888
uchar *get_nvbase 78,2101
ushort *get_nvmagic 87,2305
nvtype *nv_getptr 98,2518
ushort nv_readconfig 120,2832
void nv_setconfig 130,2973
boolean nv_badptr 159,3873
void nv_done 169,4011
void nv_writeenable 185,4368
void nv_writedisable 191,4468
void nv_init 201,4587
