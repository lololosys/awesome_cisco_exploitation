
remote_tty.h,1272
#define RTTY_OUTPUT_MAX	19,653
#define RTTYC_INPUT_MAX	20,684
#define RTTY_SERVER_MASTER_PORT_NAME	22,727
#define RTTY_CLIENT_SLAVE_PORT_NAME	23,789
#define OUTPUT_TIMEOUT	25,862
typedef enum rtty_msg_type_ rtty_msg_type_31,1134
} rtty_msg_type;rtty_msg_type41,1337
typedef enum rtty_error_type_ rtty_error_type_43,1355
} rtty_error_type;rtty_error_type48,1489
typedef struct rtty_output_req_ rtty_output_req_58,1857
} rtty_output_req_t;rtty_output_req_t61,1948
typedef struct rtty_output_rsp_ rtty_output_rsp_63,1971
} rtty_output_rsp_t;rtty_output_rsp_t65,2026
typedef struct rttyc_attach_req_ rttyc_attach_req_69,2070
} rttyc_attach_req_t;rttyc_attach_req_t73,2180
typedef struct rttyc_attach_rsp_ rttyc_attach_rsp_75,2204
} rttyc_attach_rsp_t;rttyc_attach_rsp_t77,2260
typedef struct rttyc_detach_req_ rttyc_detach_req_79,2283
} rttyc_detach_req_t;rttyc_detach_req_t81,2341
typedef struct rttyc_detach_rsp_ rttyc_detach_rsp_83,2365
} rttyc_detach_rsp_t;rttyc_detach_rsp_t85,2421
typedef struct rttyc_input_req_ rttyc_input_req_87,2444
} rttyc_input_req_t;rttyc_input_req_t90,2535
typedef struct rttyc_input_rsp_ rttyc_input_rsp_92,2558
} rttyc_input_rsp_t;rttyc_input_rsp_t94,2613
static inline tt_soc *number2tt 104,2864
