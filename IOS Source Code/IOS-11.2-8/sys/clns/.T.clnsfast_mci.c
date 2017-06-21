
clnsfast_mci.c,1312
#define CLNS_BUFFER_SIZE 98,3212
#define clns_head 103,3421
#define fast_head 104,3467
#define CLNS_COPY_OFFSET 110,3690
#define	ETHER_OFFSET 111,3739
#define SERIAL_OFFSET	113,3847
#define FDDI_OFFSET 114,3890
#define	ATM_OFFSET(ATM_OFFSET116,3992
enum if_type if_type119,4108
enum if_type { IF_ETHER,119,4108
enum if_type { IF_ETHER, IF_FDDI,119,4108
enum if_type { IF_ETHER, IF_FDDI, IF_SERIAL,119,4108
enum if_type { IF_ETHER, IF_FDDI, IF_SERIAL, IF_ATM 119,4108
#define	CLNS_TTL_OFFSET	121,4164
#define CLNS_SAPSAP 126,4274
#define CLNS_CTLNLPID 131,4405
static inline void write_emac 140,4514
static inline void write_fmac_and_start 158,5054
static inline void clns_write_hdlc_header(191,6234
static inline void clns_write_atm_header(202,6578
static boolean clns_mci2mci 220,7081
static inline boolean fast_path 441,15100
#define CLEARWORD(CLEARWORD765,32296
#define CLEARWORD(CLEARWORD771,32408
#define CKSUMWORD(CKSUMWORD777,32480
#define CKSUMWORD(CKSUMWORD789,32706
boolean clns_fastswitch 799,32871
static boolean clns_fast_fragmentation 1171,44213
static void clns_mci_fs_register_drivers 1215,45274
static void clns_mci_fs_init 1257,46539
#define CLNSMCIFS_MAJVERSION 1267,46730
#define CLNSMCIFS_MINVERSION 1268,46761
#define CLNSMCIFS_EDITVERSION 1269,46792
