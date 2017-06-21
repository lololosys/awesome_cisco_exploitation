
clock.h,2006
#define _CLOCK_H_88,3080
#define	JAN	93,3156
#define	FEB	94,3171
#define	FEBLEAP	95,3186
#define	MAR	96,3205
#define	APR	97,3220
#define	MAY	98,3235
#define	JUN	99,3250
#define	JUL	100,3265
#define	AUG	101,3280
#define	SEP	102,3295
#define	OCT	103,3310
#define	NOV	104,3325
#define	DEC	105,3340
#define MILLISECSPERSEC 112,3529
#define	SECSPERMIN	113,3593
#define	MINSPERHR	114,3645
#define	HRSPERDAY	115,3694
#define DAYSPERWEEK	116,3740
#define	DAYSPERYEAR	117,3787
#define DAYSPERLEAPYEAR 118,3836
#define YEARSPERCENTURY 119,3894
#define SECSPERHR 121,3952
#define	SECSPERDAY	122,4001
#define MILLISECSPERDAY 123,4057
#define SECSPERYEAR	124,4111
#define	SECSPERLEAPYEAR	125,4177
#define SECONDS_PER_MINUTE 127,4249
#define MINUTES_PER_HOUR 128,4306
#define SECONDS_PER_HOUR 129,4354
#define	MAR1900	131,4404
#define	DAYSPERCYCLE	132,4473
#define	SECSPERCYCLE	133,4579
#define	YEARSPERCYCLE	134,4626
#define	TIMES60(TIMES60140,4774
#define	TIMES24(TIMES24141,4836
#define	TIMESDPERC(TIMESDPERC142,4898
#define	CYCLE23	149,5069
#define	STARTCYCLE23	150,5092
#define JAN11900DOW	151,5136
#define	JANFEBNOLEAP	156,5262
#define	JANFEBLEAP	157,5308
#define MAR1968	159,5357
#define MAR1970	160,5422
#define JAN1970	161,5485
#define MAR1992 162,5543
#define MAR1993	163,5592
#define DEFAULT_TZ_OFFSET 167,5691
#define DEFAULT_TZ_NAME 168,5740
#define DEFAULT_DST_OFFSET 169,5815
#define CLOCK_MIN_YEAR	177,6016
#define CLOCK_MAX_YEAR	178,6044
struct timeval_ timeval_184,6119
typedef struct secs_and_nsecs_ secs_and_nsecs_201,6714
} secs_and_nsecs;secs_and_nsecs204,6863
} clock_epoch;clock_epoch220,7195
#define epoch_secs	222,7211
#define	epoch_frac	223,7265
typedef enum clock_source_ clock_source_228,7353
} clock_source;clock_source235,7689
} summer_time_selection_type;summer_time_selection_type244,7961
typedef struct recur_ recur_250,8057
} recur;recur256,8270
#define MONTHS_IN_YEAR 258,8280
#define DAYS_IN_WEEK 259,8306
