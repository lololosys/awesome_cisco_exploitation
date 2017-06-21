
connect.h,1705
#define __CONNECT_H__89,3170
#define REMOTE_UNAME_LEN 94,3234
struct connvector_ connvector_97,3365
#define CONNLEN 137,4819
#define	SUBBUFLEN 138,4879
typedef struct _conn_telnet _conn_telnet140,4933
} conn_telnet;conn_telnet162,6258
typedef struct _conn_rlogin _conn_rlogin164,6274
} conn_rlogin;conn_rlogin167,6436
#define LAT_MAXCREDITS 169,6452
typedef struct _conn_lat _conn_lat171,6514
} conn_lat;conn_lat196,7906
typedef struct _conn_pad _conn_pad198,7919
} conn_pad;conn_pad200,8036
struct conntype_ conntype_202,8049
#define SLC_LEVELBITS	232,9535
# define SLC_DEFAULT	233,9596
# define SLC_VALUE	234,9649
# define SLC_CANTCHANGE 235,9692
# define SLC_NOSUPPORT	236,9753
#define SLC_FLUSHOUT	237,9806
#define SLC_FLUSHIN	238,9850
#define SLC_ACK	239,9892
#define CONN_CURRENT 245,9948
#define CONN_NOTIFIED 246,10001
#define CONN_STREAM	247,10065
#define	CONN_SETUP	248,10132
#define	CONN_FLOW	249,10182
#define CONN_CHECKALL	250,10228
#define CONN_CHANGED	251,10292
#define CONN_KBDLOCK	252,10352
#define CONN_FLUSH	253,10405
#define CONN_INBOUND	254,10472
#define CONN_QUIET	255,10528
#define CONN_WRITEBLOCK	256,10595
#define CONN_TRANSPORT_DEFMASK 262,10703
#define LAT_USER_INPUT	268,10828
#define LAT_SEND_BREAK	270,10929
#define LAT_CONN_HIC	271,10989
#define conn_settty(conn_settty273,11055
#define conn_changed(conn_changed274,11123
#define conn_report(conn_report275,11184
#define conn_alive(conn_alive276,11243
#define conn_run(conn_run277,11321
#define conn_resume(conn_resume278,11398
#define conn_close(conn_close279,11467
#define conn_unread(conn_unread280,11524
#define conn_acctupdate(conn_acctupdate281,11583
