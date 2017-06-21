
refclock_trimble.c,3027
#define CISCO32,1102
#define	PRECISION	60,1787
#define	REFID	61,1848
#define	DESCRIPTION	62,1891
typedef enum {SYNC_STATE, SYNC_DLE_STATE, RCV_STATE, RCV_DLE_STATE} rcv_state;rcv_state66,1980
#define TRIMBLE_ID_GPS_TIME 70,2084
typedef struct trimble_gps_time_ trimble_gps_time_71,2117
} trimble_gps_time;trimble_gps_time76,2368
#define TRIMBLE_GPS_TIME_LENGTH 77,2388
#define TRIMBLE_ID_XYZ_FIX 82,2462
typedef struct trimble_xyz_fix_ trimble_xyz_fix_83,2494
} trimble_xyz_fix;trimble_xyz_fix89,2786
#define TRIMBLE_XYZ_FIX_LENGTH 90,2805
#define RADIANS_TO_CENTISECS_MANT 91,2839
#define RADIANS_TO_CENTISECS_EXP 92,2917
#define TRIMBLE_ID_XYZ_VELOCITY 97,3019
typedef struct trimble_xyz_velocity_ trimble_xyz_velocity_98,3056
} trimble_xyz_velocity;trimble_xyz_velocity105,3389
#define TRIMBLE_XYZ_VELOCITY_LENGTH 106,3413
#define TRIMBLE_ID_RECEIVER_HEALTH 111,3477
typedef struct trimble_receiver_health_ trimble_receiver_health_112,3517
} trimble_receiver_health;trimble_receiver_health116,3672
#define TRIMBLE_RECEIVER_HEALTH_LENGTH 117,3699
#define RECEIVER_STATUS_OK 119,3741
#define RECEIVER_STATUS_NO_GPS_TIME 120,3792
#define RECEIVER_STATUS_HIGH_PDOP 121,3855
#define RECEIVER_STATUS_NO_SATELLITE 122,3918
#define RECEIVER_STATUS_1_SATELLITE 123,3987
#define RECEIVER_STATUS_2_SATELLITE 124,4057
#define RECEIVER_STATUS_3_SATELLITE 125,4119
#define TRIMBLE_ID_SYSTEM_MESSAGE 130,4207
#define TRIMBLE_ID_LLA_FIX 135,4272
typedef struct trimble_lla_fix_ trimble_lla_fix_136,4304
} trimble_lla_fix;trimble_lla_fix143,4615
#define TRIMBLE_LLA_FIX_LENGTH 144,4634
#define TRIMBLE_ID_DOUBLE_LLA_FIX 149,4711
typedef struct trimble_double_lla_fix_ trimble_double_lla_fix_150,4750
} trimble_double_lla_fix;trimble_double_lla_fix157,5068
#define TRIMBLE_DOUBLE_LLA_FIX_LENGTH 158,5094
#define TRIMBLE_ID_UTC_PARAMS 163,5159
typedef struct trimble_utc_params_ trimble_utc_params_164,5194
} trimble_utc_params;trimble_utc_params174,5466
#define TRIMBLE_UTC_PARAMS_LENGTH 175,5488
struct trimbleunit trimbleunit182,5561
#define TRIMBLE_DLE	189,5806
#define TRIMBLE_ETX	190,5831
static int trimble_float_to_int 197,5940
static void trimble_float_to_l_fp 213,6271
static void trimble_float_multiply 233,6786
static void trimble_convert_single 297,8658
static void trimble_convert_double 321,9266
static void trimble_flush_reass_queue 347,9845
static void trimble_shutdown 360,10062
static void trimble_process_location 377,10358
static void trimble_process_gps_time 439,12598
static void trimble_process_message 516,15105
static struct recvbuf *trimble_receive 640,18417
void clock_send_str_exec 757,21016
static void trimble_send(791,21624
static void trimble_poll(811,22034
static int trimble_start 835,22550
static void trimble_setup 869,23251
static void ntp_refclock_trimble_init 926,24659
#define NTP_REFCLOCK_TRIMBLE_MAJVERSION 936,24836
#define NTP_REFCLOCK_TRIMBLE_MINVERSION 937,24878
#define NTP_REFCLOCK_TRIMBLE_EDITVERSION 938,24920
