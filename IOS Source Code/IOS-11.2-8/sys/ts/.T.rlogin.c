
rlogin.c,1009
#define	MAXBYTES 217,7517
#define FLUSHREAD	219,7576
#define	FLUSHWRITE	220,7599
#define	NOSTOP	221,7623
#define	DOSTOP	222,7644
#define	WINDOW	223,7665
struct windowupdate windowupdate225,7687
static boolean rlogin_supported 262,8518
static void put_window 270,8591
static int rlogin_oob 289,9137
static int rlogin 358,10881
static conntype *rlogin_open_connection 619,18539
static void rlogin_connect_command 804,22966
static conntype *rlogin_multiproto_open 927,26041
static char *rlogin_get_lname_local 1009,28180
static char *rlogin_get_lname_tacacs 1022,28382
static char *rlogin_get_lname_radius 1049,28929
static void rlogin_get_remote_name_local 1065,29320
static void rlogin_trusted_host_missing 1108,30242
static char *rlogin_get_remote_username 1119,30434
void rlogin_trust_remoteuser_command 1150,31210
static char *rlogin_get_local_username 1185,32245
void rlogin_trusted_localuser_source_command 1214,32946
void rlogin_init 1243,33578
void rlogin_conn_acct 1266,34365
