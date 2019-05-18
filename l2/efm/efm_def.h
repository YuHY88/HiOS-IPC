#ifndef HIOS_EFM_DEF_H
#define HIOS_EFM_DEF_H

typedef enum efm_bool_t
{
	EFM_FALSE = 0x00,
	EFM_TRUE,
}efm_bool;

/* Maximum OAMPDUs transmitted in a seond -57.3.2.2.1 */
#define EFM_MAX_RATE_DEFAULT                    				10

#define EFM_REM_LB_TIMEOUT_NONE             				0

#define EFM_SYMBOL_PERIOD_THRES_DEF					1 
#define EFM_SYMBOL_PERIOD_WINDOW_DEF				1000000

#define EFM_ERR_FRAME_THRES_DEF         					1
#define EFM_ERR_FRAME_WINDOW_DEF						1

#define EFM_FRAME_PERIOD_THRES_DEF      				1
#define EFM_FRAME_PERIOD_WINDOW_DEF				1000000

#define EFM_FRAME_SECOND_THRES_DEF      				1
#define EFM_FRAME_SECOND_WINDOW_DEF				60

#define EFM_FRAME_PERIOD_WINDOW_MUL         		10000
#define EFM_SYMBOL_PERIOD_MUL               				1000000

#define MAX_32BIT_NUMBER 0xFFFFFFFF

/* 57-9 */
#define EFM_TYPE			0x8809
/* 57.4 */
#define EFM_SUBTYPE		0x03

/* PDU encoding */
#define EFM_VERSION		0x01

/* Table 57-4 */
#define EFM_INFO_CODE									0x00
#define EFM_EVENT_NOTIFICATION_CODE	0x01
#define EFM_VAR_REQUEST                 			0x02
#define EFM_VAR_RESPONSE                			0x03
#define EFM_LOOPBACK_CONTROL            	0x04
#define EFM_ORG_SPECIFIC                			0xFE

enum efm_pdu
{
	EFM_INFORMATION_PDU,
	EFM_LOOPBACK_PDU,
	EFM_ERR_SYMBOL_PERIOD_EVENT_PDU,
	EFM_ERR_FRAME_EVENT_PDU,
	EFM_ERR_FRAME_PERIOD_EVENT_PDU,
	EFM_ERR_FRAME_SECONDS_SUM_EVENT_PDU,
	EFM_SPECIFIC_PDU,
	EFM_PDU_INVALID,
	EFM_PDU_MAX,
};

/* TLV's */
#define EFM_INFORMATION_TLV					0x00
#define EFM_LOCAL_INFORMATION_TLV  		0x01
#define EFM_REMOTE_INFORMATION_TLV 	0x02
#define EFM_ORG_SPECIFIC_INFO      			0xFE
#define EFM_ORG_SPEC_VALUE         				0xff // not specified
#define EFM_LOCAL_TLV_LENGTH       			0x10
#define EFM_OUI                    							0x00
#define EFM_VENDOR_INFO            				0x00

#define EFM_ERRORED_SYMBOL_PERIOD_EVENT			0x01
#define EFM_ERRORED_FRAME_EVENT            					0x02
#define EFM_ERRORED_FRAME_PERIOD_EVENT     			0x03
#define EFM_ERRORED_FRAME_SECONDS_EVENT    		0x04

#define EFM_ERR_SYMBOL_PERIOD_TLV_LEN		0x28
#define EFM_ERR_FRAME_TLV_LEN						0x1a
#define EFM_ERR_FRAME_PERIOD_TLV_LEN			0x1C 
#define EFM_ERR_FRAME_SUMMARY_TLV_LEN	0x12

#define EFM_REMOTE_LOOPBACK_ENABLE              	0x01
#define EFM_REMOTE_LOOPBACK_DISABLE             	0x02

#define EFM_GRP_ADDR_LEN             6

#define EFM_REMOTE_STABLE				(1 << 6)
#define EFM_REMOTE_EVALUATING		(1 << 5)
#define EFM_LOCAL_STABLE					(1 << 4)
#define EFM_LOCAL_EVALUATING         (1 << 3)
#define EFM_CRITICAL_EVENT           		(1 << 2)
#define EFM_DYING_GASP               		(1 << 1)
#define EFM_LINK_FAULT               			(1 << 0)

#define EFM_CFG_VAR_RESPONSE_SUPPORT	(1 << 4)
#define EFM_CFG_LINK_EVENT_SUPPORT			(1 << 3)
#define EFM_CFG_REM_LB_SUPPORT         		(1 << 2)
#define EFM_CFG_UNI_DIR_SUPPORT        		(1 << 1)
#define EFM_CFG_OAM_MODE               				(1 << 0)

#define EFM_CFG_COMPARE_MASK	(0xFE)

#define EFM_OUI_LENGTH	3
#define EFM_VSI_LENGTH	4

#define EFM_WIN_LENGTH							8
#define EFM_THRES_LENGTH						8
#define EFM_ERR_LENGTH							8
#define EFM_ERR_RUN_TOT_LENGTH			8
#define EFM_EVENT_RUN_TOT_LENGTH	4

#define EFM_MAX_OAMPDU_SIZE            1518

#define EFM_OAM_LINK_FAULT	1
#define EFM_OAM_LINK_UP		0

#define EFM_UNI_DIR_ENABLE 1
#define EFM_UNI_DIR_DISABLE 0
enum efm_admin_state 
{
	EFM_ENABLE = 1,
	EFM_DISABLE = 2,
};

enum efm_mode
{
	EFM_MODE_PASSIVE = 0,
	EFM_MODE_ACTIVE = 1,
	EFM_MODE_UNKNOWN = 2,
};

enum efm_oper_status
{
	OPER_DISABLED=1,
	OPER_LINK_FAULT,
	OPER_PASSIVE_WAIT,
	OPER_ACTIVE_SEND_LOCAL,
	OPER_SEND_LOCAL_REMOTE,
	OPER_SEND_LOCAL_REMOTE_OK,
	OPER_PEERING_LOCALLY_REJECTED,
	OPER_PEERING_REMOTELY_REJECTED,
	OPER_OPERATIONAL,
	OPER_NONOPER_HALF_DUPLEX,
};

struct efm_info_pdu
{
	uint8_t info_type;
	uint8_t info_length;
	uint8_t oam_version;
	uint16_t revision;
	uint8_t state;
	uint8_t oam_config;
	uint16_t oam_pdu_config;
	uint8_t oui [EFM_OUI_LENGTH];
	uint8_t vend_spec_info [EFM_VSI_LENGTH];

	uint8_t rem_info_type;
	uint8_t rem_info_length;
	uint8_t rem_oam_version;
	uint16_t rem_revision;
	uint8_t rem_state;
	uint8_t rem_oam_config;
	uint16_t rem_oam_pdu_config;
	uint8_t rem_oui [EFM_OUI_LENGTH];
	uint8_t rem_vend_spec_info [EFM_VSI_LENGTH];
};

struct efm_sym_period_event_pdu
{
	uint16_t err_sym_period_event_ts;
	uint64_t err_sym_window;
	uint64_t err_sym_threshold;
	uint64_t err_symbols;
	uint64_t err_sym_total;
	uint32_t err_sym_event_total;
};

struct efm_frame_event_pdu
{
	uint16_t err_frame_event_ts;
	uint16_t err_frame_window;
	uint32_t err_frame_thres;
	uint32_t err_frames;
	uint64_t err_frame_total;
	uint32_t err_frame_event_total; 
};

struct efm_frame_period_event_pdu
{
	uint16_t err_frame_period_event_ts;
	uint32_t err_frame_period_window;
	uint32_t err_frame_period_thres;
	uint32_t err_frame_period_frames;
	uint64_t err_frame_period_error_total;
	uint32_t err_frame_period_event_total;
};

struct efm_frame_seconds_pdu
{
	uint16_t err_frame_sec_event_ts;
	uint16_t err_frame_sec_window;
	uint16_t err_frame_sec_thres;
	uint16_t err_frame_sec_error;
	uint32_t err_frame_sec_error_total;
	uint32_t err_frame_sec_event_total;
};

struct efm_event_pdu
{
#define EFM_SYM_PERIOD_EVENT_RCVD			(1 << 0)
#define EFM_FRAME_EVENT_RCVD             			(1 << 1)
#define EFM_FRAME_PERIOD_EVENT_RCVD		(1 << 2)
#define EFM_FRAME_SEC_SUM_EVENT_RCVD	(1 << 3)

	uint16_t  seq_no;
	uint8_t   pdu_list;
	uint8_t   event_type;
	uint8_t   event_length;

	struct
	{
		struct efm_sym_period_event_pdu 	sym_period_pdu;
		struct efm_frame_event_pdu 			frame_pdu;
		struct efm_frame_period_event_pdu	frame_period_pdu;
		struct efm_frame_seconds_pdu 		frame_sec_pdu;
	} data;
};

struct efm_rem_loopback_pdu
{
	uint8_t enable;
};


extern struct thread_master	*l2_master;
#if 0
#define EFM_TIMER_ON(T,F,S,V) \
	do { \
		if (! (T)) \
		(T) = thread_add_timer (l2_master, (F), (S), (V)); \
	} while (0)

#define EFM_TIMER_MSEC_ON(T,F,S,V) \
	do { \
		if (! (T)) \
		(T) = thread_add_timer_msec (l2_master, (F), (S), (V)); \
	} while (0)


#define EFM_TIMER_OFF(T) \
	do { \
		if (T) \
		{ \
			thread_cancel (T); \
			(T) = NULL; \
		} \
	} while (0)
#endif
#define EFM_TIMER_ON_LOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("efm_timer",LIB_TIMER_TYPE_LOOP, (F), (S), (V)*1000); \
			} while (0)
		
#define EFM_TIMER_ON_UNLOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("efm_timer",LIB_TIMER_TYPE_NOLOOP, (F), (S), (V)*1000); \
			   } while (0)

#define EFM_TIMER_MSEC_ON_LOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("efm_timer",LIB_TIMER_TYPE_LOOP, (F), (S), (V)); \
			} while (0)
		

#define EFM_TIMER_MSEC_ON_UNLOOP(T,F,S,V) \
			do { \
				if (! (T)) \
				(T) = high_pre_timer_add ("efm_timer",LIB_TIMER_TYPE_NOLOOP, (F), (S), (V)); \
			} while (0)
				
#define EFM_TIMER_OFF(T) \
			do { \
				if (T) \
				{ \
					 high_pre_timer_delete (T); \
					(T) = 0; \
				} \
			} while (0)

typedef union {
	uint8_t	c[8];
	uint16_t s[4];
	uint32_t l[2];
	uint64_t ll;
} efm_uint64_t;  /* 64 bit unsigned integer */


#define EFM_MUL_32_UINT(A,B,RESULT)                                    \
	do {                                                                \
		uint32_t _a, _b, _c, _d;                                           \
		uint32_t _x, _y;                                                   \
		efm_uint64_t *_res = (efm_uint64_t*)&RESULT;                                         \
		\
		_a = ((A) >> 16) & 0xffff;                                          \
		_b = (A) & 0xffff;                                                  \
		_c = ((B) >> 16) & 0xffff;                                          \
		_d = (B) & 0xffff;                                                  \
		\
		_res->l[0] = _b * _d;                                               \
		_x = _a * _d + _c * _b;                                             \
		_y = (((_res->l[0]) >> 16) & 0xffff) + _x;                          \
		\
		_res->l[0] = (((_res->l[0]) & 0xffff)  | ((_y & 0xffff) << 16));    \
		_res->l[1] = ((_y >> 16) & 0xffff);                                 \
		\
		_res->l[1] += (_a * _c);                                            \
	} while (0)


struct efm_oam_pdu 
{
	u_int8_t	subtype;
	uint16_t efm_flags;
	uint16_t code;

	union
	{
		struct efm_rem_loopback_pdu 	lb_pdu;
		struct efm_info_pdu 					info_pdu;
		struct efm_event_pdu				event_pdu;
	} data;
};


#define sym_period_pdu		event_pdu.data.sym_period_pdu
#define frame_pdu               	event_pdu.data.frame_pdu
#define frame_period_pdu	event_pdu.data.frame_period_pdu
#define frame_sec_pdu		event_pdu.data.frame_sec_pdu
#define info_pdu					data.info_pdu
#define event_pdu				data.event_pdu 
#define lb_pdu						data.lb_pdu 

#endif

