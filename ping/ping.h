#ifndef HIOS_PING_H
#define HIOS_PING_H

#include <lib/linklist.h>
#include <lib/pkt_buffer.h>
#include <stdio.h>
#include <ftm/pkt_icmp.h>
#include <ftm/pkt_udp.h>


#define PING_DBG_ALL      		0


#define PING_CMD   1
#define TRACE_CMD  2
#define TRACE_UDP_CMD 3

#define VPN_SUPPORT 128
#define TRACE_SPORT 30005
#define TRACE_BASE_DPORT 33434

/*echo str max len*/
#define ECHO_MAX_STR_LEN(count)\
    (10 + (count) * 10 + 20)


/* Master of threads. */
struct thread_master *ping_master;

/* icmp request 报文链表*/
struct list *preq_list;  

/* 等待reply 的定时器*/
TIMERID ptimer; 

TIMERID ptimer; 
//struct thread * ptimer; 

struct ping_info{
	struct inet_addr srcip;
	struct inet_addr destip;
	uint32_t fragflag;
	uint32_t time;
	uint32_t waittime;
	uint32_t size;
	uint16_t vpn;
	uint16_t len;
	uint16_t ttl;
	uint16_t type;
    uint32_t count;//发包数量
    uint32_t ifindex;
    void *data;//报文指针
    void *pvty;//vty  终端
    struct timeval sendsec;//发包时间
    struct ping_cnt *stats;//收发包统计
};

struct ping_cnt
{
    uint32_t sendcnt;
    uint32_t recvcnt;
	uint32_t lastrecv;
    uint32_t ip;//trace 回显ip
    uint8_t *buffer;//存储每一跳的trace 结果
};


/*DEBUG 宏*/
#define PING_ZLOG_DEBUG(format,...)\
    zlog_debug(PING_DBG_ALL,"%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);

#define PING_ZLOG_ERR(format,...)\
    zlog_err("%s[%d]:In function '%s',"format,__FILE__,__LINE__,__func__,##__VA_ARGS__);


/*trace 每跳结束回应vty*/
#define ICMP_TRACE_VTYOUT(icmp_pkt,pingInfo)\
    if(ntohs(icmp_pkt->sequence) == pingInfo->count)\
    {                                         \
        vty_out(pingInfo->pvty,"%s\r\n",pingInfo->stats->buffer);\
        memset(pingInfo->stats->buffer,0, ECHO_MAX_STR_LEN(pingInfo->count));\
    }

/*trace 每跳打印ttl */
#define ICMP_TRACE_SPRINT_TTL(icmp_pkt,pingInfo,tmp)\
    if(1 == ntohs(icmp_pkt->sequence))\
    {                           \
        if(1 == pingInfo->count)            \
            sprintf(tmp,"%2d ",(pingInfo->stats->sendcnt)/pingInfo->count);\
        else                        \
            sprintf(tmp,"%2d ",(pingInfo->stats->sendcnt)/pingInfo->count + 1);\
        strcat((char *)(pingInfo->stats->buffer),(char *)tmp);\
        memset(tmp,0,10);\
    }

/*trace 回显ip*/
#define ICMP_TRACE_SPRINT_IP(icmp_pkt,pingInfo,addr)\
    if((pingInfo->count == ntohs(icmp_pkt->sequence)) &&\
       (inet_valid_ipv4(pingInfo->stats->ip)))\
    {                                           \
        inet_ipv4tostr(pingInfo->stats->ip,addr);\
        strcat((char *)(pingInfo->stats->buffer),addr);\
    }

/*trace udp每跳打印ttl; sendcnt字段需每跳结束清0*/
#define TRACE_SET_SPRINT_TTL(ttl, pinfInfo, tmp)\
	memset(tmp,0,10);                \
	if (pingInfo->stats->sendcnt == 1)\
	    sprintf((char *)tmp,"%2d ", ttl);\
    strcat((char *)(pingInfo->stats->buffer),(char *)tmp);\
    memset(tmp,0,10);       


/*trace udp打印回显ip,并输出每跳回显*/
#define TRACE_SET_SPRINT_IP(sip,pingInfo,addr)\
	if ((pingInfo->stats->sendcnt == pingInfo->count) && \
		(inet_valid_ipv4(sip)))\
	{                                            \
		inet_ipv4tostr(sip,(char *)addr);\
        strcat((char *)(pingInfo->stats->buffer),(char *)addr);\
	}                                                    \
	vty_out(pingInfo->pvty,"%s\r\n",(char *)(pingInfo->stats->buffer));\
	memset(pingInfo->stats->buffer,0,ECHO_MAX_STR_LEN(pingInfo->count));


int ndp_ipv6_str_check(const char *src);
void icmp_ping_set_ipcb(struct ip_control *ipcb,struct ping_info *pingInfo);
int  icmp_ping_set_icmphdr(struct icmphdr *icmp_pkt,struct ping_info *pingInfo);
void icmp_ping_recv_reply(struct icmphdr *icmp_pkt,struct ip_control *ipcb,uint32_t datalen);
void icmp_ping_send_next(struct ping_info *pingInfo);
int icmp_ping_send_request(struct ping_info *pingInfo);
void icmp_trace_send_next(struct ping_info *pingInfo);
void trace_udp_send_next(struct ping_info *pingInfo);
void trace_udp_rcv_reply(struct icmphdr *icmp_pkt,struct ip_control *ipcb);
void trace_udp_rcv_localip(uchar *udp_pkt, struct ip_control *ipcb);
void trace_set_udphdr(uint8_t *udp_pkt,struct ping_info *pingInfo);
int trace_udp_send(struct ping_info *pingInfo);
int icmp_ping_timer_thread(void *thread);
//int  icmp_ping_msg_rcv(struct thread *thread);
int icmp_ping_msg_rcv_n(struct ipc_mesg_n *pmesg, int imlen);
void icmp_ping_pkt_register(void);
void icmp_ping_init (void);


#endif
