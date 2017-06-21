
ipfast_cbus.c,1214
typedef struct cbusipcachetype_ cbusipcachetype_101,3313
} cbusipcachetype;cbusipcachetype107,3543
struct ipcache_cbtype_ ipcache_cbtype_112,3608
#define CBUSI_MAX_ENCAPS 125,3943
#define CBUSII_MAX_ENCAPS 126,3976
#define CBUSI_MAX_CE_BYTES 127,4009
#define CBUSII_MAX_CE_BYTES 128,4081
#define CBUSI_RWCACHE_ADDR 129,4155
#define CBUSII_RWCACHE_ADDR 130,4192
#define CBUSI_FREECACHE_ADDR 131,4229
#define CBUSII_FREECACHE_ADDR 132,4266
#define BOGUS_VERSION 133,4303
#define CBUS_BUCKET_SIZE 134,4340
#define CBUS_IPCACHE_ENTRIES 135,4372
#define CBUS_IPCACHE_SIZE 136,4434
#define CBUS_IPCACHE_RECENT 137,4512
static inline ushort cbus_cache_read 169,5548
static inline void cbus_cache_write 181,5839
static inline void cbus_cache_select 193,6141
static inline ipcache_cbtype *cbus_ipcache_replace 199,6265
void cbus_ipcache_update 286,8265
void cbus_ipcache_init 401,11767
void cbus_ipcache_init_callback 412,11973
#define VALID_CBENTRY(VALID_CBENTRY546,16232
void cbus_ipcache_invalidate 548,16358
void cbus_ipcache_show 613,18318
static void ip_cbus_init 702,20794
#define CBUSIP_MAJVERSION 728,21500
#define CBUSIP_MINVERSION 729,21528
#define CBUSIP_EDITVERSION 730,21556
