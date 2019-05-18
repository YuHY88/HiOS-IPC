#ifndef HIOS_EFM_LINK_MONITOR_H
#define HIOS_EFM_LINK_MONITOR_H

#include <time.h>
#include <sys/time.h>
#include <linux/param.h> 
#include <lib/command.h>


#include "efm_def.h"

/* HZ = frequency of ticks per second. */
#define WRAPAROUND_VALUE (0xffffffffUL / HZ + 1) 

#define EFM_LINK_MONITOR_PERIOD_FRAME_TIMER		500//500ms
#define EFM_LINK_MONITOR_FRAME_SEC_INTERVAL     1// 1s
#define NILMSGHDR_LEN 1024
#define MAX_DATA_LEN 10
enum ev_notify_enable
{
	EFM_NOTIFY_ON = 1, 
	EFM_NOTIFY_OFF,
};

enum efm_link_event_type
{        
         EFM_ERR_NO_TYPE_EVENT,
	EFM_ERR_SYMBOL_PERIOD_EVENT = 1,
	EFM_ERR_FRAME_EVENT,
	EFM_ERR_FRAME_PERIOD_EVENT,
	EFM_ERR_FRAME_SECONDS_SUM_EVENT,
	EFM_ORG_SPEC_EVENT =0xFE,
	EFM_LINK_FAULT_EVENT = 256,
	EFM_DYING_GASP_EVENT = 257,
	EFM_CRITICAL_LINK_EVENT = 258,
	EFM_UNKNOWN_EVENT = 300,
};

enum efm_event_location
{
	EFM_EVENT_LOCAL = 1,
	EFM_EVENT_REMOTE,
};

struct efm_link_event_info
{
	uint16_t    seq_no;

	struct timeval 	last_symbol_period_event_ts;
	uint16_t	err_symbol_period_event_ts;
	uint64_t	err_symbol_window;
	uint64_t	err_symbol_threshold;
	uint64_t	err_symbols;
	uint64_t	err_symbols_total;
	uint32_t	err_sym_event_total;
	enum ev_notify_enable   err_symbol_ev_notify_enable;

	struct timeval 	last_frame_event_ts;
	uint16_t	err_frame_event_ts;
	uint16_t   err_frame_window;
	uint32_t   err_frame_threshold;
	uint32_t   err_frames;
	uint64_t   err_frame_total;
	uint32_t   err_frame_event_total;
	enum ev_notify_enable  err_frame_ev_notify_enable;
  
	struct timeval 	last_frame_period_event_ts;
	uint16_t	err_frame_period_event_ts;
	uint32_t   err_frame_period_window;
	uint32_t   err_frame_period_threshold;
	uint32_t   err_frame_period_frames;
	uint64_t   err_frame_period_error_total;
	uint32_t   err_frame_period_event_total;
	enum ev_notify_enable  err_frame_period_ev_notify_enable;

	struct timeval 	last_frame_sec_sum_event_ts;
	uint16_t	err_frame_sec_sum_event_ts;
	uint16_t   err_frame_sec_sum_window;
	uint16_t   err_frame_sec_sum_threshold;
	uint16_t   err_frame_sec_error;
	uint32_t   err_frame_sec_error_total;
	uint32_t   err_frame_sec_event_total;
	enum ev_notify_enable  err_frame_sec_ev_notify_enable;

	enum ev_notify_enable  dying_gasp_enable;
	enum ev_notify_enable  critical_event_enable;
};

struct efm_link_monitor_run
{
	uint8_t link_monitor_on;

	efm_bool symbol_period_threshold_set;
	efm_bool symbol_period_window_set;
	
	efm_bool frame_threshold_set;
	efm_bool frame_window_set;

	efm_bool frame_period_threshold_set;
	efm_bool frame_period_window_set;

	efm_bool frame_seconds_threshold_set;
	efm_bool frame_seconds_window_set;
};


struct efm_event_log_entry
{
	uint32_t	event_log_Index;
	uint64_t	event_log_timestamp;
	uint8_t		event_log_oui[3];
	uint32_t	event_log_type;
	s_int32_t	event_log_location;
	uint64_t	event_log_window;
	uint64_t	event_log_threshold;
	uint64_t	event_log_value;
	uint64_t	event_log_running_total;
	uint32_t	event_log_event_total;
};

#include "efm.h"

int
efm_system_uptime (struct timeval *tv);
time_t
efm_time_current (time_t *tp);

uint16_t
efm_link_event_get_ts (struct timeval *last_event_ts);


s_int32_t
efm_link_monitor_support_set (struct efm_if *pefm, u_int8_t enable);
int
efm_link_monitoring_data_init(struct efm_if *pefm);
s_int32_t
efm_link_monitor_err_symbol_window_expiry (struct thread *thread);

s_int32_t
efm_link_monitor_err_frame_window_expiry(struct thread *thread);

s_int32_t
efm_link_monirot_err_frame_period_window_expiry (struct thread *thread);
s_int32_t
efm_link_monitor_err_frame_secs_expiry (struct thread *thread);
s_int32_t
efm_link_monitor_err_frame_secs_window_expiry (struct thread *thread);
s_int32_t
efm_link_monitor_on_set (struct efm_if *pefm, u_int8_t enable);
int
efm_process_local_link_events (struct efm_if *pefm, 
										uint64_t errors, uint16_t event_type);


s_int32_t 
efm_link_event_log_add (struct efm_if *pefm, 
                            struct efm_link_event_info *event_info,    
                            enum efm_link_event_type event_type, 
                            enum efm_event_location ev_location, u_int8_t *oui);

s_int32_t 
efm_event_log_size_set (struct efm_if *pefm, uint32_t size);
uint32_t
efm_clear_event_log (struct efm_if *pefm);


int
efm_process_local_event (struct efm_if *pefm, 
							uint16_t local_event, uint8_t enable);

void
efm_recv_local_event (uint16_t local_event, uint8_t enable);

int
efm_send_link_event_to_alarm(struct efm_if *pefm,int event_location,uint16_t  event_type); 

int
efm_send_link_monitor_data_to_hal(uint32_t index,uint8_t err_type, uint64_t window,uint64_t threshold);
int 
efm_link_monitor_event_occur(struct ipc_msghdr_n *phdr,uint64_t *pdata);


#endif

