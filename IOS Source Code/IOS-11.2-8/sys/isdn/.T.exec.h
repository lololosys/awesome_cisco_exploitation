
exec.h,2839
#define EXEC_H 88,3192
#define MAILBOX_FULL 99,3490
#define NO_MAIL_DESCRIPTORS 100,3564
#define NO_TIMERS 101,3638
#define UNBALANCED_CS 102,3712
#define UNBALANCED_NPS 103,3786
#define BAD_WAIT 104,3860
#define BAD_SWITCH 105,3934
#define NOT_DEAD 106,4008
#define SUSPEND_PRI 110,4118
#define MON_PRI 111,4192
#define TMR_PRI 112,4266
#define USER_MIN_PRI 113,4340
#define USER_MAX_PRI 114,4414
#define MUST_SEND_PRI 118,4509
#define MAX_PRI 119,4583
#define UP 125,4824
#define DOWN 126,4898
#define DEFAULT_DIR 127,4972
#define SELF 128,5046
#define ISDN_RUNNING 132,5175
#define READY 134,5277
#define BLOCKED 135,5351
#define INITIAL 136,5425
#define RESIGNED 137,5499
#define UNKNOWN_TASK 138,5573
#define F_PUSH_NEG 143,5726
#define SIZE_CONTEXT 144,5755
#define F_PUSH_NEG	148,5809
#define SIZE_CONTEXT	149,5833
#define F_PUSH_NEG 153,5884
#define SIZE_CONTEXT 154,5913
typedef unsigned long MWORD;MWORD166,6215
typedef unsigned long MWORD;MWORD170,6314
typedef unsigned MWORD;MWORD174,6413
typedef sys_timestamp U_CLOCK;U_CLOCK185,6868
typedef short int SEQNO;SEQNO190,7085
typedef struct tcbtcb207,7743
TCB;TCB219,8397
typedef struct tcb *TASK_ID;TASK_ID220,8402
typedef struct semaphoresemaphore226,8611
SEM;SEM231,8788
typedef struct semaphore *SEM_ID;SEM_ID232,8793
typedef struct ext_memory_addrext_memory_addr239,9059
EXT_MEM;EXT_MEM243,9168
typedef struct mail_descriptormail_descriptor255,9601
MAIL_DESCRIPTOR;MAIL_DESCRIPTOR262,9932
typedef struct mail_queuemail_queue274,10343
MAIL_Q;MAIL_Q280,10595
typedef struct mail_queue *MAIL_Q_ID;MAIL_Q_ID281,10603
typedef struct mailboxmailbox292,11034
MAILBOX;MAILBOX299,11357
typedef struct mailbox *MAILBOX_ID;MAILBOX_ID300,11366
typedef struct timer_blocktimer_block314,12016
TIMER_BLOCK;TIMER_BLOCK325,12639
typedef struct timer_block *TIMER_ID;TIMER_ID326,12652
#define Enable(Enable339,12983
#define Disable(Disable340,13000
#define Enable(Enable342,13024
#define Disable(Disable343,13064
#define Enable(Enable348,13137
#define Disable(Disable349,13154
#define Enable(Enable355,13240
#define Disable(Disable356,13279
#define InitTS(InitTS381,14192
#define SendMail(SendMail382,14264
#define TestMail(TestMail383,14323
#define TestMailAvail(TestMailAvail384,14376
#define TestMailLimit(TestMailLimit385,14452
#define EnterCS(EnterCS386,14528
#define EnterICS(EnterICS387,14584
#define EnterNPS(EnterNPS388,14626
#define ExitCS(ExitCS390,14682
#define ExitICS(ExitICS391,14759
#define ExitNPS(ExitNPS392,14837
#define ExitTCS(ExitTCS393,14917
#define ExitCS(ExitCS395,15004
#define ExitICS(ExitICS396,15044
#define ExitNPS(ExitNPS397,15085
#define ExitTCS(ExitTCS398,15126
#define DASSERT(DASSERT408,15489
#define DASSERT(DASSERT415,15661
