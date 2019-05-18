#ifndef _BOA_PACKET_H
#define _BOA_PACKET_H
#include <pthread.h>

#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <lib/pkt_buffer.h>
#include <lib/sockunion.h>
#include <lib/memtypes.h>
#include <lib/hash1.h>
#include <lib/memory.h>
#include <lib/syslog.h>
#include <lib/log.h>
#include <ftm/pkt_ip.h>

#define WEB_TCP_PORT  80
#define WED_FD_TOTAL 1024
#define WEB_TCP_SEND_MAX 1000

extern int g_total_tcp_fd;
extern int g_cusume;
extern pthread_mutex_t g_web_syn_lock;
extern struct web_pkt_debug g_web_dbg ;

struct web_tcp_session_parm
{
	unsigned int   dst_ip;
	unsigned int   src_ip;
	unsigned short dst_port;
	unsigned short src_port;

};

struct queue_data_t
{
	char *data;                       //数据指针
	int  data_len;                    //数据长度
	int  data_offset;                 //有效数据位置，即相对与*data的偏移
	struct queue_data_t * next;       //指向下一个数据节点
};


struct web_fd_parm
{
	int is_used;                                    //该fd是否已被占用
	int is_new;                                     //该链接是否是个新连接
	time_t time;                                    //该链接最后被访问的时间
	struct web_tcp_session_parm session_info;       //该链接对应的tcp 4要素
	struct queue_data_t * read_queue_head;          //该链接所收到的tcp数据
	
};

struct web_pkt_debug
{
	char tcp_send;
	char tcp_recv;
	char disp_mode;
	char tcp_boa_syn;
};
/* aaa protocol */
enum
{
	WEB_TCP = 0,
	WEB_PROTO_MAX
};

enum
{
	WEB_DISPLAY_CHAR = 0,
	WEB_DISPLAY_INT,
	WEB_DISPLAY_BOTH
};

enum
{
	WEB_PKT_SEND = 0,
	WEB_PKT_RECV
};


int web_packet_init();
int web_hash_tcp_fd_lookup(struct web_tcp_session_parm *psess);
void web_hash_tcp_fd_hash_init(unsigned int size);
int web_tcp_accept();
int web_tcp_data_write(int  fd, char *data, int data_len);
int web_tcp_data_read(int fd, char * data, int len);
int web_tcp_close(int fd);
int web_pltfmlog_init(int argc, char **argv);
int sys_cpu_init(void);
int web_cli_init(void);

#endif  

