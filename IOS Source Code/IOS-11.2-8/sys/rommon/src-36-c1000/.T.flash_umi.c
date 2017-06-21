
flash_umi.c,680
#define MAXFLASHERASE 52,1552
#define ERASECMD 54,1580
#define ERASECONFIRM 56,1663
#define ERASESTAT 59,1785
#define ERASEVER 62,1892
#define WRSTATRDY 65,2006
#define BYTEWRSTAT 68,2117
#define CLRSTATREG 71,2233
#define READMEM 74,2357
#define RDSTATCMD 77,2466
#define PROGCMD 80,2585
#define PROGVER 82,2675
#define RESETCMD 85,2796
#define VPPSTAT 87,2879
#define MAXFLIDS 112,3681
check_flash_end(115,3754
get_man_dev(143,4471
int sizeflash(165,4993
get_flash_info(184,5473
poll_flash_stat(308,9144
program_flash(343,9858
get_fdi_struct(460,13483
flash_cleanup(477,13805
erase_flash(493,14117
flash_test(721,20760
ftest(778,22156
flash(869,25244
