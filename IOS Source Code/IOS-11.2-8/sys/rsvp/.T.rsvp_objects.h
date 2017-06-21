
rsvp_objects.h,5173
#define __RSVP_OBJECT__48,1615
#define RSVP_FRAGMENTATION49,1639
#define RSVP_BUGINF(RSVP_BUGINF52,1722
#define RSVP_RESV_BUGINF(RSVP_RESV_BUGINF56,1835
#define RSVP_PATH_BUGINF(RSVP_PATH_BUGINF63,2055
#define RSVP_DUMP_OBJECT(RSVP_DUMP_OBJECT70,2274
struct rsvp_object_ rsvp_object_84,2667
#define RSVP_VERSION 91,2891
enum rsvp_message_types_ rsvp_message_types_94,2923
rsvp_message_types;rsvp_message_types98,3075
struct rsvp_header_ rsvp_header_101,3125
#define RSVP_MORE_FRAGS	115,3754
#define RSVP_FRAGMENT_OFFSET 116,3822
#define RSVP_CLASS_FORWARD_MASK 132,4425
#define RSVP_CLASS_NULL 133,4462
#define RSVP_CLASS_SESSION 134,4488
#define RSVP_CLASS_SESSION_GROUP 135,4517
#define RSVP_CLASS_HOP 136,4552
#define RSVP_CLASS_INTEGRITY 137,4577
#define RSVP_CLASS_TIME_VALUES 138,4608
#define RSVP_CLASS_ERROR_SPEC 139,4641
#define RSVP_CLASS_SCOPE 140,4673
#define RSVP_CLASS_STYLE 141,4700
#define RSVP_CLASS_FLOWSPEC 142,4727
#define RSVP_CLASS_FILTER_SPEC 143,4757
#define RSVP_CLASS_SENDER_TEMPLATE 144,4791
#define RSVP_CLASS_SENDER_TSPEC 145,4829
#define RSVP_CLASS_ADSPEC 146,4864
#define RSVP_CLASS_POLICY_DATA 147,4893
#define RSVP_CLASS_RESV_CONFIRM 148,4927
#define RSVP_CLASS_MAX 149,4962
#define RSVP_TYPE_IPV4 151,4989
#define RSVP_TYPE_IPV6 152,5014
struct rsvp_null_ rsvp_null_160,5322
struct rsvp_session_ipv4_ rsvp_session_ipv4_170,5609
#define RSVP_SESSION_POLICE 175,5760
struct rsvp_hop_ipv4_ rsvp_hop_ipv4_186,6120
#define RSVP_REFRESH_MS 195,6336
#define RSVP_CLOCK_TICK 196,6370
struct rsvp_refresh_ rsvp_refresh_198,6442
enum rsvp_error_types rsvp_error_types206,6601
    RSVP_ERROR_ADMISSION 207,6625
    RSVP_ERROR_ADMINISTRATIVE,208,6680
    RSVP_ERROR_NO_PATH_INFORMATION,209,6732
    RSVP_ERROR_NO_SENDER_INFORMATION,210,6787
    RSVP_ERROR_AMBIGUOUS_PATH,211,6847
    RSVP_ERROR_AMBIGUOUS_FILTER_SPECIFICATION,212,6900
    RSVP_ERROR_INCOMPATIBLE_STYLE,213,6964
    RSVP_ERROR_MISSING_OBJECT 214,7023
    RSVP_ERROR_UNKNOWN_OBJECT,215,7080
    RSVP_ERROR_UNKNOWN_CLASS_TYPE,216,7143
    RSVP_OBJECT_ERROR,217,7209
    RSVP_TRAFFIC_ERROR 218,7262
    RSVP_SYSTEM_ERROR,219,7322
    RSVP_ERROR_CONFIRM	220,7377
#define RSVP_ERROR_INTEGRITY 223,7444
struct rsvp_error_ipv4_ rsvp_error_ipv4_226,7554
#define RSVP_LUB_USED 230,7646
struct rsvp_confirm_ipv4_ rsvp_confirm_ipv4_239,7841
struct rsvp_scope_ipv4_ rsvp_scope_ipv4_248,8047
#define RSVP_DISTINCT 257,8288
#define RSVP_SHARED 258,8319
#define RSVP_SHARING_MASK 259,8348
#define RSVP_SCOPE_WILD 261,8405
#define RSVP_SCOPE_EXPLICIT 262,8431
#define RSVP_SCOPE_MASK 263,8461
#define RSVP_WF_OPTION 265,8491
#define RSVP_FF_OPTION 266,8546
#define RSVP_SE_OPTION 267,8607
struct rsvp_style_ rsvp_style_270,8695
enum    qos_service_type qos_service_type278,8822
    QOS_CONTROLLED_LOAD=279,8849
    QOS_GUARANTEED 280,8911
    QOS_PREDICTIVE 281,8968
    QOS_CONTROLLED_DELAY 282,9025
    QOS_TSPEC 283,9088
enum rsvp_spec_types rsvp_spec_types286,9155
enum rsvp_spec_types { INTSRV 286,9155
enum intsrv_services intsrv_services288,9193
	INTSRV_GENERAL 289,9216
	INTSRV_GTD 290,9237
	INTSRV_CLOAD 291,9254
#define INTSRV_MAX_DELAY 294,9276
enum intsrv_field_name intsrv_field_name295,9322
	INTSRV_NON_IS_HOPS 296,9347
	INTSRV_NON_IS_HOPS = 1, INTSRV_COMPOSED_NON_IS_HOPS,296,9347
	INTSRV_IS_HOPS,297,9401
	INTSRV_IS_HOPS, INTSRV_COMPOSED_IS_HOPS,297,9401
	INTSRV_PATH_BANDWIDTH,298,9443
	INTSRV_PATH_BANDWIDTH, INTSRV_MIN_PATH_BANDWIDTH,298,9443
	INTSRV_IF_LATENCY,299,9494
	INTSRV_IF_LATENCY, INTSRV_PATH_LATENCY,299,9494
	INTSRV_MTU,300,9535
	INTSRV_MTU, INTSRV_COMPOSED_MTU,300,9535
	INTSRV_TOKEN_BUCKET_TSPEC 302,9570
	INTSRV_GTD_RSPEC 303,9604
    	INTSRV_DELAY 305,9630
	INTSRV_MAX_JITTER,306,9703
    	INTSRV_E2E_DELAY,307,9758
	INTSRV_E2E_MAX_JITTER,308,9806
    	INTSRV_SHP_DELAY,309,9855
	INTSRV_SHP_MAX_JITTER	310,9903
struct intsrv_tspec_ intsrv_tspec_314,9984
struct intsrv_cload_flowspec_ intsrv_cload_flowspec_335,10627
struct intsrv_real_object_ intsrv_real_object_356,11344
struct intsrv_integer_object_ intsrv_integer_object_363,11497
rsvp_valid_integer_object(370,11632
rsvp_build_integer_object(380,11867
rsvp_valid_real_object(390,12108
rsvp_build_real_object(400,12335
struct intsrv_adspec_general_ intsrv_adspec_general_412,12589
#define RSVP_ADSPEC_BREAK 414,12665
struct intsrv_gtd_adspec_ intsrv_gtd_adspec_425,13098
#define RSVP_MIN_GTD_SPEC_LENGTH 442,13670
struct intsrv_cload_adspec_ intsrv_cload_adspec_445,13742
struct intsrv_template_adspec_ intsrv_template_adspec_453,14072
struct intsrv_gtd_flowspec_ intsrv_gtd_flowspec_464,14411
struct isi_flowspec_ isi_flowspec_491,15282
struct isi_tspec_ isi_tspec_499,15605
struct rsvp_flowspec_ rsvp_flowspec_505,15713
struct rsvp_filter_ipv4_ rsvp_filter_ipv4_519,16132
struct rsvp_template_ipv4_ rsvp_template_ipv4_532,16479
struct rsvp_tspec_ rsvp_tspec_544,16774
struct rsvp_adspec_ rsvp_adspec_557,17087
struct rsvp_policy_ rsvp_policy_571,17488
#define RSVP_TYPE_CRYPTO_INTEGRITY 579,17673
struct rsvp_integrity_ rsvp_integrity_581,17742
