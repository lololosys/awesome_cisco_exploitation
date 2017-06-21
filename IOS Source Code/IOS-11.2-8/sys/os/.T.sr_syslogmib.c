
sr_syslogmib.c,1223
#define SYSLOG_TRAP_NUMBER 94,3286
#define TRAP_VARBIND_COUNT 95,3315
#define FACILITY 97,3345
#define SEVERITY 98,3365
#define MSGNAME 99,3385
#define MSGTEXT 100,3405
#define TIMESTAMP 101,3425
#define SYSLOG_TRAP_SWITCHCOUNT 104,3496
#define MAX_SYSLOG_QUEUE_LENGTH 105,3532
clogBasic_get(148,4685
clogBasic_free(244,7080
clogBasic_cleanup(256,7371
clogBasic_test(277,8101
clogBasic_set(392,11466
clogHistory_get(413,12102
clogHistory_free(491,14063
clogHistory_cleanup(503,14360
clogHistory_test(524,15096
clogHistory_set(624,18120
clogHistoryEntry_get(645,18767
k_clogBasic_get(763,21473
k_clogBasic_test(795,22487
k_clogBasic_ready(806,22679
k_clogBasic_set(818,22890
clogBasic_undo(843,23658
k_clogHistory_get(854,23893
k_clogHistory_test(868,24236
k_clogHistory_ready(879,24430
k_clogHistory_set(891,24643
clogHistory_undo(907,24966
k_clogHistoryEntry_get(918,25210
static void do_syslog_trap 962,26426
static void process_SyslogTrapQueue 1063,29926
static process SyslogTrapProcess 1090,30775
static void syslogEvent 1107,31195
init_syslogmib 1156,32747
#define MAJVERSION_syslogmib 1174,33185
#define MINVERSION_syslogmib 1175,33216
#define EDITVERSION_syslogmib 1176,33247
