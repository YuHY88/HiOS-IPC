#ifndef HIOS_PING6_H
#define HIOS_PING6_H

#include <lib/linklist.h>
#include <lib/pkt_buffer.h>
#include <stdio.h>

#include <ftm/pkt_icmp.h>
#include "ping.h"



#define PING6_CMD   1
#define TRACE6_CMD  2

#define ICMPV6_PING_HDR_SIZE 8 //ICMPV6请求报文头长度

/* icmp request 报文链表*/
struct list *preq6_list;  

/* 等待reply 的定时器*/
TIMERID p6timer; 

struct icmpv6_req
{
    struct icmpv6_hdr hdr;
    uint16_t id;
    uint16_t sequence;
    uint8_t  data[0];
};



int  icmpv6_ping_msg_rcv(struct thread *thread);
void icmpv6_ping_set_ipcb(struct ip_control *pktcontrol,struct ping_info *pingInfo);
int  icmpv6_ping_set_icmphdr(struct icmpv6_hdr *icmp_pkt,struct ping_info *pingInfo);
void  icmpv6_ping_recv_request(struct pkt_buffer *pkt);
void icmpv6_ping_recv_reply(struct icmpv6_req *icmpv6_reply,struct ip_control *ip6cb,uint32_t datalen);
int icmpv6_ping_timer_thread(struct thread *thread);
void icmpv6_ping_send_next(struct ping_info *pingInfo);
int icmpv6_ping_send_request(struct ping_info *pingInfo);

void icmpv6_ping_pkt_register(void);
void icmpv6_ping_init (void);


#endif

