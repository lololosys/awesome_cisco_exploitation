
ntp_authenticate.c,1301
#define CISCO52,1873
#define MD5_KEY_LEN 65,2117
#define DECRYPTED_PASSWORD_MAX 66,2140
#define ENCRYPTED_PASSWORD_MAX 67,2218
struct savekey savekey71,2353
#define	KEY_TRUSTED	84,2534
#define	KEY_KNOWN	85,2584
#define	KEY_DES	88,2642
#define	KEY_MD5	91,2702
#define	HASHSIZE	97,2892
#define	HASHMASK	98,2912
#define	KEYHASH(KEYHASH99,2944
#define	KEY_SCHED_SIZE	114,3210
#define	ZEROKEY_L	120,3371
#define	ZEROKEY_R	121,3426
#define	NOCRYPT_OCTETS	148,4090
#define	BLOCK_OCTETS	149,4115
#define	NOCRYPT_LONGS	151,4140
#define	BLOCK_LONGS	152,4196
#define FAKE_PACKET_SIZE 153,4248
#define LOOPCOUNT_SHIFT 157,4339
#define AUTHTIME_LOOPCOUNT 158,4392
static void auth_flush_cache 165,4557
auth_findkey(175,4747
static void auth_delete_key(192,5017
boolean ntp_delete_key 220,5562
MD5authencrypt(232,5743
MD5authdecrypt(260,6308
void MD5auth1crypt(285,6819
MD5auth2crypt(307,7265
authhavekey(325,7780
static struct savekey *auth_make_key 362,8394
authtrust(383,8759
authistrusted(425,9551
    DESauth_setkey(448,9885
void MD5auth_setkey(470,10298
auth1crypt(515,11325
auth2crypt(542,11762
authencrypt(563,12129
authdecrypt(588,12555
void ntp_nv_trust 611,12983
void ntp_nv_authkey 631,13338
static void ntp_compute_auth_delay(663,14064
init_auth(765,16485
