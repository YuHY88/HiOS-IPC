#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#define __USE_GNU
#include <sched.h>
#include <pthread.h>
#include <sys/sysinfo.h>
//#include <netinet/in.h>
#include <sys/socket.h>
//#include <arpa/inet.h>
#include <ftm/pkt_ip.h>
#include <lib/inet_ip.h>
#include <lib/vty.h>
#include <lib/command.h>

#include "boa_packet.h"
#include "../../config.h"

#include <lib/thread.h>
#include <lib/log.h>
#define WEB_TCP_TIME_OUT 30
struct web_fd_parm  g_web_fd[1024];
struct hash_table web_tcp_fd_table;   /* tcp session hash table */
pthread_mutex_t g_web_syn_lock;

int g_total_tcp_fd;
int g_cusume;

struct web_pkt_debug g_web_dbg = 
{
	0, 
	0,
	WEB_DISPLAY_CHAR,
	0
};
static char   *protostr[] = {"tcp",  "unknown"};
struct thread_master  *web_cli_master = NULL;
static struct cmd_node web_node =
{
  WEB_NODE,
  "%s(config-web)# ",
  1
};



void debug_printf(char *pbuf, int len)
{
    int      i;

//	printf("debug pbuf len : %d\n", len);
    for (i = 0; i < len ; i++)
    {
        if ((i % 128) == 0)
        {
            printf("\n");
        }

        printf("%c", pbuf[i]);
    }

    printf("\n");
}

void web_pkt_dump(u_char *data, int len, int opt, uint8_t proto, char *caller)
{
	uint32_t i = 0, taillen;
	int    flag = 0;
	char   *optstr = NULL;
	u_char *pktdata;
	char   string[64];
	char   tmp[6];
	if(NULL == data || NULL == caller)
		return;

	if(WEB_PKT_SEND == opt && proto < WEB_PROTO_MAX)
	{
		optstr = "send";
		if(WEB_TCP == proto && g_web_dbg.tcp_send)
		{
			flag = 1;
		}
	}
	else if(WEB_PKT_RECV == opt && proto < WEB_PROTO_MAX)
	{
		optstr = "recv";
		if(WEB_TCP == proto && g_web_dbg.tcp_recv)
		{
			flag = 1;
		}
	}
	else
	{
		zlog_err("%s call %s opt:%d, proto:%d error\n", caller, __FUNCTION__, opt, proto);
		return;
	}
	if(0 == flag) 
	{
		return;
	}
	zlog_debug("-----fucntion %s call fucntion %s start %s %s packet : ",caller, __FUNCTION__,optstr, protostr[proto]);

	if(WEB_DISPLAY_INT== g_web_dbg.disp_mode ||WEB_DISPLAY_BOTH== g_web_dbg.disp_mode)
	{
		for(i = 0; (i+16) < len; i += 16)
		{
			zlog_debug("%02x %02x %02x %02x %02x %02x %02x %02x  %02x %02x %02x %02x %02x %02x %02x %02x",
				data[i], data[i+1], data[i+2], data[i+3], data[i+4], data[i+5],
				data[i+6], data[i+7], data[i+8], data[i+9], data[i+10], data[i+11],
				data[i+12], data[i+13], data[i+14], data[i+15]);
		}
		taillen = len % 16;
		if(taillen != 0)
		{
			pktdata = len > 16 ? (data + i) : data;
			memset(string, 0, sizeof(string));
			memset(tmp, 0, sizeof(tmp));
			for(i = 0; i < taillen; i++)
			{
				if(i == 8)
					sprintf(tmp, " %02x ", pktdata[i]);
				else if(i == (taillen -1))
					sprintf(tmp, "%02x", pktdata[i]);
				else
					sprintf(tmp, "%02x ", pktdata[i]);
				strcat(string, tmp);
			}
			zlog_debug("%s", string);
		}
	}
	
	if(WEB_DISPLAY_CHAR == g_web_dbg.disp_mode||WEB_DISPLAY_BOTH== g_web_dbg.disp_mode)
	{
		for(i = 0; (i+16) <= len; i += 16)
		{
			zlog_debug("%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c%c",
				data[i], data[i+1], data[i+2], data[i+3], data[i+4], data[i+5],
				data[i+6], data[i+7], data[i+8], data[i+9], data[i+10], data[i+11],
				data[i+12], data[i+13], data[i+14], data[i+15]);
		}
		taillen = len % 16;
		if(taillen != 0)
		{
			pktdata = len > 16 ? (data + i) : data;
			memset(string, 0, sizeof(string));
			memset(tmp, 0, sizeof(tmp));
			for(i = 0; i < taillen; i++)
			{

				if(i == (taillen -1))
					sprintf(tmp, "%c", pktdata[i]);
				else
					sprintf(tmp, "%c", pktdata[i]);
				strcat(string, tmp);
			}
			zlog_debug("%s", string);
		}
	}
	
	
    zlog_debug("web packet data end.-----------------------------");	
}

#if 0

int web_test()
{
		int tcp_fd;
		struct web_tcp_session_parm  session_info;
		fd_set	readfds;
		struct timeval tv;
		tv.tv_sec=0;
		tv.tv_usec=500*1000;
		char tcp_buf[2000];
		char buf[1000];
		int  read_len, i;
		int  accept_fd_1;
		char tmp = 0;             
		FD_ZERO(&readfds);
        FD_SET(g_total_tcp_fd,&readfds);
		while(1)
		{
//			printf("loop readfds            : %x\n", readfds);				
//			printf("loop g_total_tcp_fd + 1 : %x\n", g_total_tcp_fd + 1);				
			if(select(g_total_tcp_fd + 1,&readfds,NULL,NULL,NULL)<0)
			{
							perror("select error");
							exit(-1);
			}
//					printf("select!!!!\n");
					
					recvfrom(g_total_tcp_fd, &tmp, 1,0,NULL, NULL);
					g_cusume = 0;
//					printf("%s %d tmp : %d\n",__FUNCTION__, __LINE__, tmp);
#if 0
			tcp_fd = web_tcp_accept(&session_info);
			printf("tcp_fd : %d\n",tcp_fd);
			if(tcp_fd >= 0)
			{
				accept_fd_1 = tcp_fd;
				printf("accept tcp_fd : %d\n",accept_fd_1);
				
				do{
					
					memset(tcp_buf, 0 , sizeof(tcp_buf));
					read_len = web_tcp_data_read(accept_fd_1,tcp_buf, 2000 );
					if(read_len > 0)
						{
					printf("1debug print len : %d\n", read_len);
					debug_printf(tcp_buf, read_len);
					printf("---------------------------------\n");
					}
				}while(read_len > 0);
			}
	
			do{
				sprintf(buf,"%s", "Content-Type: text/html<HTML> <HEAD><TITLE>thttpd is running</TITLE></HEAD><BODY BGCOLOR=\"#99cc99\" TEXT=\"#000000\" LINK=\"#2020ff\" VLINK=\"#4040cc\"><H3>thttpd is running</H3><P>Looks like you got it working.  Congrats.<P>Here's a link to the <A HREF=\"http://www.acme.com/software/thttpd/\">thttpd web pages</A>.</BODY></HTML>\n");
		//		sprintf(buf,"%s", "Content-Type: text/html<HTML> <HEAD><TITLE>thttpd is running</TITLE></HEAD><BODY BGCOLOR=\"#99cc99\" TEXT=\"#000000\" LINK=\"#2020ff\" VLINK=\"#4040cc\"><H3>thttpd is running</H3><P>Looks like you got it working.  Congrats.<P>Here's a link to the <A HREF=\"http://www.acme.com/software/thttpd/\">thttpd web pages</A>.</BODY></HTML>\n");
				web_tcp_data_write(accept_fd_1, buf, strlen(buf));
				memset(tcp_buf, 0 , sizeof(tcp_buf));
				read_len = web_tcp_data_read(accept_fd_1,tcp_buf, 2000 );
				if(read_len > 0)
					{
				printf("2debug print len : %d\n", read_len);
				debug_printf(tcp_buf, read_len);
				printf("---------------------------------\n");
					}
			}while(read_len > 0);
	
		//	usleep(10000*1000);
#endif
			
		}
	
	
		


}
#endif	
int web_get_free_fd(void)
{
	int fd;
	for(fd = 0 ; fd < WED_FD_TOTAL; fd++)
	{
		if(0 == g_web_fd[fd].is_used)
		{
			return fd;
		}
	}
	return -1;
}

void web_pkt_register(void)
{
	int ret;
	union proto_reg proto;

	memset(&proto, 0, sizeof(proto));
	proto.ipreg.protocol = IP_P_TCP;
	proto.ipreg.dport = WEB_TCP_PORT;
	ret = pkt_register(MODULE_ID_WEB, PROTO_TYPE_IPV4, &proto);
}

void web_pkt_unregister(struct web_tcp_session_parm session_info)
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(proto));
	proto.ipreg.dip= session_info.src_ip;
	proto.ipreg.dport= session_info.dst_port;
	proto.ipreg.protocol= IP_P_TCP;
	proto.ipreg.sport = session_info.src_port;
	pkt_unregister(MODULE_ID_WEB, PROTO_TYPE_IPV4, &proto);
	
}

static unsigned int web_hash_tcp_fd_compute(void *hash_key)
{
	struct web_tcp_session_parm *parm = (struct web_tcp_session_parm *)hash_key;

	return parm->src_ip + parm->dst_ip + parm->dst_port + parm->src_port;
}

static int web_hash_tcp_fd_compare(void *item, void *hash_key)
{
	struct hash_bucket *tmp = (struct hash_bucket *)item;
	struct web_tcp_session_parm *psess1 = (struct web_tcp_session_parm *)(tmp->hash_key);
	struct web_tcp_session_parm *psess2 = (struct web_tcp_session_parm *)hash_key;
		

	if((psess1->src_ip == psess2->src_ip)
		 &&(psess1->dst_ip == psess2->dst_ip)
		 && (psess1->dst_port == psess2->dst_port)
		 && (psess1->src_port == psess2->src_port))
			return 0;
		else
			return 1;
}


int web_hash_tcp_fd_lookup(struct web_tcp_session_parm *psess)
{
   int * p_fd;
   struct hash_bucket *bucket = hios_hash_find(&web_tcp_fd_table, psess);

   if(NULL == bucket) 
   {
	   return -1;
   }
   
   p_fd = (int*)(bucket->data);
   return *p_fd;
}


/* init tcp proto hash */
void web_hash_tcp_fd_hash_init(unsigned int size)
{
    hios_hash_init(&web_tcp_fd_table, size, web_hash_tcp_fd_compute, web_hash_tcp_fd_compare);
}


/* add proto to tcp_proto_hash */
int web_hash_tcp_fd_add(int fd, struct web_tcp_session_parm *psess)
{

	int *data;
	struct hash_bucket *bucket ;
	struct web_tcp_session_parm *hash_key; 

	bucket = (struct hash_bucket *)XMALLOC(MTYPE_HASH_BACKET, sizeof(struct hash_bucket));
	if(NULL == bucket)
	{
		return -1;
	}
	
	hash_key  =  (struct web_tcp_session_parm *)XMALLOC(MTYPE_WEB_ENTRY, sizeof(struct web_tcp_session_parm));
	if(NULL == hash_key)
	{
		XFREE(MTYPE_HASH_BACKET, bucket);
		return -1;
	}
	*hash_key  = *psess;

	data =(int *) XMALLOC(MTYPE_WEB_ENTRY,sizeof(int));
	if(NULL == data)
	{
		XFREE(MTYPE_HASH_BACKET, bucket);
		XFREE(MTYPE_WEB_ENTRY, hash_key);
	}
	*data = fd;
	
	bucket->hash_key = hash_key;
	bucket->data    = data;
	bucket->hashval = 0;
	bucket->next = NULL;
	bucket->prev = NULL;
	
    return hios_hash_add(&web_tcp_fd_table, bucket);
}


int web_hash_tcp_fd_delete(struct web_tcp_session_parm *psess)
{
	
	struct hash_bucket *pbucket = NULL;

	pbucket = hios_hash_find(&web_tcp_fd_table, psess);
	if(NULL == pbucket)
	{
		return -1;
	}
	
	XFREE(MTYPE_WEB_ENTRY, pbucket->data);
	XFREE(MTYPE_WEB_ENTRY, pbucket->hash_key);

	hios_hash_delete(&web_tcp_fd_table, pbucket);
	XFREE(MTYPE_HASH_BACKET, pbucket);
	return 0;
}


int web_tcp_queue_add_data(struct queue_data_t ** head, struct queue_data_t *item)
{
	struct queue_data_t * current;
	if(NULL == *head)
	{
		*head = item;
	}
	else
	{
		current = *head;
		while(current)
			{
				if(NULL == current->next)
				{
					current->next = item;
					item->next = NULL;
					break;
				}
				current = current->next;
			}
	}
	
	return 0;
	
}

int web_tcp_queue_del_data_all(struct queue_data_t * head)
{
	struct queue_data_t * current, *tmp;
	if(NULL == head)
	{
		return 0;
	}
	else
	{
		current = head;
		while(current)
			{
				tmp = current;
				current = current->next;
				free(tmp->data);
				free(tmp);
			}
	}
	
	return 0;
	
}

int web_tcp_close(int fd)
{
	char debug_dst_ip[100],debug_src_ip[100];
	struct ip_proto tcp;

//	printf("\n%s session closed :dst_ip : %s ,src_ip : %s, dst_port : %d, src_port : %d\n",__FUNCTION__ ,debug_dst_ip, debug_src_ip, g_web_fd[fd].session_info.dst_port,g_web_fd[fd].session_info.src_port);
//	zlog_debug("%s tcp session closed:dst_ip:%s,src_ip:%s,dst_port:%d,src_port:%d",__FUNCTION__ ,debug_dst_ip, debug_src_ip, g_web_fd[fd].session_info.dst_port,g_web_fd[fd].session_info.src_port);
	if(1 != g_web_fd[fd].is_used)
	{
		return -1;
	}
	tcp.dip   = g_web_fd[fd].session_info.src_ip;
	tcp.sip   = g_web_fd[fd].session_info.dst_ip;
	tcp.dport = g_web_fd[fd].session_info.src_port;
	tcp.sport = g_web_fd[fd].session_info.dst_port;
	tcp.protocol = IP_P_TCP;
	
	strcpy(debug_dst_ip, inet_ntoa(g_web_fd[fd].session_info.dst_ip));
	strcpy(debug_src_ip, inet_ntoa(g_web_fd[fd].session_info.src_ip));
	zlog_debug("%s  fd : %d, dst_ip:%s, src_ip:%s, dst_port:%d, src_port:%d\n",__FUNCTION__ ,fd, debug_dst_ip, debug_src_ip, tcp.dport,tcp.sport);


	pkt_close_tcp(MODULE_ID_WEB, &tcp);

//	web_pkt_unregister(g_web_fd[fd].session_info);
//	web_pkt_register();

	web_tcp_queue_del_data_all(g_web_fd[fd].read_queue_head);
	web_hash_tcp_fd_delete(&g_web_fd[fd].session_info);
	memset(&g_web_fd[fd], 0 ,sizeof(g_web_fd[fd]));
	return 0;
}

int web_tcp_accept(struct web_tcp_session_parm * session_info  )
{

	int fd;
	
	for(fd = 0 ; fd < WED_FD_TOTAL; fd++)
	{
		if(1 == g_web_fd[fd].is_new)
		{
			*session_info = g_web_fd[fd].session_info;
			g_web_fd[fd].is_new = 0;
			return fd;
		}
		else
		{
			continue;
		}
	}

	return -1;
}

int web_tcp_write_test()
{
	char buf[100];
	web_tcp_data_write(0, buf, strlen(buf));
	return 0;
}

int web_tcp_data_write(int  fd, char *data, int data_len)
{
#if 1
	int data_len_tmp;
	char *p_data;
	int tx_len;
	union pkt_control pkt_ctrl;
	int ret;
	char debug_dst_ip[100],debug_src_ip[100];
	if(NULL == data)
	{
		return -1;
	}
	
	if(1 != g_web_fd[fd].is_used)
	{
		return -1;
	}
	memset(&pkt_ctrl, 0, sizeof(pkt_ctrl));
	pkt_ctrl.ipcb.dip      =  g_web_fd[fd].session_info.src_ip;
	pkt_ctrl.ipcb.sip      =  g_web_fd[fd].session_info.dst_ip;
	pkt_ctrl.ipcb.dport    =  g_web_fd[fd].session_info.src_port;
	pkt_ctrl.ipcb.sport    =  g_web_fd[fd].session_info.dst_port;
	
	pkt_ctrl.ipcb.protocol = IP_P_TCP;
	pkt_ctrl.ipcb.is_changed = 1;
	strcpy(debug_dst_ip, inet_ntoa(pkt_ctrl.ipcb.dip));
	strcpy(debug_src_ip, inet_ntoa(pkt_ctrl.ipcb.sip));
//	printf("\n%s  fd : %d, dst_ip : %x, src_ip : %x, dst_port : %d, src_port : %d\n",__FUNCTION__ ,fd, pkt_ctrl.ipcb.dip, pkt_ctrl.ipcb.sip, pkt_ctrl.ipcb.dport,pkt_ctrl.ipcb.sport);
	zlog_debug("%s fd : %d, dst_ip:%s,src_ip:%s,dst_port:%d,src_port:%d",__FUNCTION__ ,fd,debug_dst_ip, debug_src_ip, pkt_ctrl.ipcb.dport,pkt_ctrl.ipcb.sport);
//	debug_printf(data, data_len);
	web_pkt_dump(data, data_len,WEB_PKT_SEND,WEB_TCP,__FUNCTION__);

	p_data = data;
	data_len_tmp = data_len;
	
	while(data_len_tmp > 0)
		{
			if(data_len_tmp >= WEB_TCP_SEND_MAX)
			{
				tx_len = WEB_TCP_SEND_MAX;
			}
			else
			{
				tx_len = data_len_tmp;
			}
			ret = pkt_send(PKT_TYPE_TCP, &pkt_ctrl, p_data, tx_len);
			p_data = p_data + tx_len;
			data_len_tmp = data_len_tmp - tx_len;

		}
	usleep(50*1000);
	return data_len;
#endif
	
}

void web_pkt_rcv()
{
	char debug_dst_ip[100],debug_src_ip[100];
	int fd;
	int data_len;
	time_t cur_time;
	struct pkt_buffer	*pkt = NULL;	
	enum PKT_TYPE       cb_type;
	struct ip_control *ipcb;
	struct web_tcp_session_parm session_info;
	struct queue_data_t  *data_item;
	pkt = pkt_rcv(MODULE_ID_WEB);
	if(pkt == NULL)
	{
		return;
	}

	cb_type = pkt->cb_type;
	ipcb  = (struct ip_control *)(&(pkt->cb));		
	strcpy(debug_dst_ip, inet_ntoa(ipcb->dip));
	strcpy(debug_src_ip, inet_ntoa(ipcb->sip));
//	printf("%s dst_ip:%x,src_ip:%x,dst_port:%d,src_port:%d\n",__FUNCTION__ ,ipcb->dip, ipcb->sip, ipcb->dport,ipcb->sport);
//	printf("%s dst_ip:%s,src_ip:%s,dst_port:%d,src_port:%d",__FUNCTION__ ,debug_dst_ip, debug_src_ip, ipcb->dport,ipcb->sport);
	zlog_debug("%s dst_ip:%s,src_ip:%s,dst_port:%d,src_port:%d",__FUNCTION__ ,debug_dst_ip, debug_src_ip, ipcb->dport,ipcb->sport);
//	debug_printf(pkt->data, pkt->data_len);
	web_pkt_dump(pkt->data, pkt->data_len,WEB_PKT_RECV,WEB_TCP,__FUNCTION__);

	
	data_len = pkt->data_len; 
	if(/*(cb_type != PKT_TYPE_TCP) ||*/ (ipcb->dport != WEB_TCP_PORT))
	{
		return ;
	}

	session_info.dst_ip    = ipcb->dip;
	session_info.src_ip    = ipcb->sip;
	session_info.dst_port  = ipcb->dport;
	session_info.src_port  = ipcb->sport;
	
	data_item = malloc(sizeof(struct queue_data_t));
	if(NULL == data_item)
	{
		return ;
	}
	memset(data_item, 0 ,sizeof(struct queue_data_t));
	data_item->data = malloc(data_len);
	if(NULL == data_item)
	{
		free(data_item);
		return ;
	}

	memset(data_item->data, 0 ,data_len);
	memcpy(data_item->data,pkt->data, data_len);
	data_item->data_len = data_len;

	fd = web_hash_tcp_fd_lookup(&session_info);
	if(fd < 0)
	{
		fd  = web_get_free_fd();
		if(fd < 0)
		{
			free(data_item->data);
			free(data_item);
			return ;
		}
		zlog_debug("%s  add fd : %d, dst_ip:%s,src_ip:%s,dst_port:%d,src_port:%d",__FUNCTION__ ,fd,debug_dst_ip, debug_src_ip,  ipcb->dport,ipcb->sport);
		g_web_fd[fd].is_new  = 1;
		g_web_fd[fd].is_used = 1;
		g_web_fd[fd].session_info = session_info;
		g_web_fd[fd].read_queue_head = data_item;
		
		web_hash_tcp_fd_add(fd, &session_info);
	}
	else
	{
		web_tcp_queue_add_data(&g_web_fd[fd].read_queue_head, data_item);
	}
	
	time(&cur_time);
	g_web_fd[fd].time = cur_time;

	return;
}


int web_tcp_data_read(int fd, char * data, int len)
{

	int real_len, tmp_len;
	struct queue_data_t  *head = NULL ;
	struct queue_data_t  *tmp = NULL ;
	head =  g_web_fd[fd].read_queue_head;
	tmp_len = len ;
	if(NULL == head)
	{
		return -1;
	}
	
	if(1 != g_web_fd[fd].is_used)
	{
		return -1;
	}
	
	real_len = 0;

	while(head)
	{
		if(tmp_len < head->data_len)
		{
			memcpy(data + real_len, head->data + head->data_offset, tmp_len);
			head->data_offset = head->data_offset + tmp_len;
			head->data_len = head->data_len - tmp_len;
			real_len = real_len + tmp_len;
			break;
		}
		else
		{
			memcpy(data + real_len, head->data + head->data_offset, head->data_len);
			head->data_offset = head->data_offset + head->data_len;
			real_len = real_len + head->data_len;
			tmp_len = tmp_len - head->data_len;
			tmp = head ;
			head = head->next;
			free(tmp->data);
			free(tmp);
		}
	}
	
	g_web_fd[fd].read_queue_head = head;

	return real_len;

}


void web_tcp_session_time_out(void)
{
	int fd; 
	char debug_dst_ip[100],debug_src_ip[100];
	time_t cur_time;
	for(fd = 0 ; fd < WED_FD_TOTAL; fd++)
	{
		if(1 == g_web_fd[fd].is_used)
		{
			time(&cur_time);
			if(cur_time - g_web_fd[fd].time > WEB_TCP_TIME_OUT)
			{
				strcpy(debug_dst_ip, inet_ntoa(g_web_fd[fd].session_info.dst_ip));
				strcpy(debug_src_ip, inet_ntoa(g_web_fd[fd].session_info.src_ip));
				//printf("\n%s session timeout : fd : %d ,dst_ip : %s src_ip : %s, dst_port : %d, src_port : %d\n",__FUNCTION__ ,fd, debug_dst_ip, debug_src_ip, g_web_fd[fd].session_info.dst_port,g_web_fd[fd].session_info.src_port);
				zlog_debug("%s tcp session timeout:dst_ip:%s,src_ip:%s,dst_port:%d,src_port:%d\n",__FUNCTION__ ,debug_dst_ip, debug_src_ip, g_web_fd[fd].session_info.dst_port,g_web_fd[fd].session_info.src_port);
				web_tcp_close(fd);
			}
		}
		
	}

	return ;

}

void web_pkt_thread(void)
{
	int fd;
	int is_send;
	char tmp = 0;

	struct sockaddr_in server_addr;
	bzero(&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	server_addr.sin_port = htons(55555);
	
	int client_socket_fd = socket(AF_INET, SOCK_DGRAM, 0);
	if(client_socket_fd < 0)
	{
			perror("Create Socket Failed:");
			exit(1);
	}
	while(1)
	{
		usleep(10*1000);
		pthread_mutex_lock(&g_web_syn_lock);
		web_pkt_rcv();
#if 1
		if(0 == g_cusume)
		{
			is_send = 0;
			for(fd = 0 ; fd < WED_FD_TOTAL; fd++)
			{
				if(NULL != g_web_fd[fd].read_queue_head)
				{
					is_send = 1;
				}
				else
				{
					continue;
				}
			}
			if(1 == is_send)
			{   tmp ++;
				g_cusume = 1;
				sendto(client_socket_fd, &tmp, 1,0,(struct sockaddr*)&server_addr,sizeof(server_addr));
			}
		}
		web_tcp_session_time_out();

		pthread_mutex_unlock(&g_web_syn_lock);
#endif		
	}
}

void web_cli_thread(void)
{
    struct thread thread;
	
    while(1)
    {
        if(thread_fetch(web_cli_master, &thread))
        {
            thread_call(&thread); 			
        }
    }
}


DEFUN (web_mode_enable,
		web_mode_enable_cmd,
		"web",
		"web command node\n")
{
	vty->node = WEB_NODE;
	return CMD_SUCCESS;	
}


DEFUN(web_version_config_fun,
	web_version_config_cmd ,
	"web version <1-3>",
	"web\n"
	"version of web\n"
	"version number\n")
{

	printf("web_version_config_cmd\n");
	
	return CMD_SUCCESS;
}

DEFUN(debug_web_packet_enable_fun,
       debug_web_packet_enable_cmd,
       "debug web packet (receive|send)  enable",
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "disable\n")
{
	if(strncmp(argv[0], "receive", strlen("receive")) == 0)
	{
		g_web_dbg.tcp_recv = 1;
	}
	else if(strncmp(argv[0], "send", strlen("send")) == 0)
	{
		g_web_dbg.tcp_send = 1;
	}
	else
	{
		vty_out(vty, "Error: para(%s) invalid!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty_out(vty, "Enable debug %s  packet!%s", argv[0],  VTY_NEWLINE);

	return CMD_SUCCESS;
}

DEFUN(debug_web_packet_disable_fun,
       debug_web_packet_disable_cmd,
       "debug web packet (receive|send)  disable",
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "receive\n"
	   "send\n"
	   "disable\n")
{
	if(strncmp(argv[0], "receive", strlen("receive")) == 0)
	{
		g_web_dbg.tcp_recv = 0;
	}
	else if(strncmp(argv[0], "send", strlen("send")) == 0)
	{
		g_web_dbg.tcp_send = 0;
	}
	else
	{
		vty_out(vty, "Error: para(%s) invalid!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}
	vty_out(vty, "Disable debug %s  packet!%s", argv[0],  VTY_NEWLINE);

	return CMD_SUCCESS;
}


DEFUN(debug_web_packet_display_fun,
       debug_web_packet_display_cmd,
       "debug web packet  mode (char | int | both)",
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "display mode\n"
	   "display format char \n"
	   "display format int  \n"
	   "display format both \n")
{
	if(strncmp(argv[0], "char", strlen("char")) == 0)
	{
		g_web_dbg.disp_mode = WEB_DISPLAY_CHAR;
	}
	else if(strncmp(argv[0], "int", strlen("int")) == 0)
	{
		g_web_dbg.disp_mode = WEB_DISPLAY_INT;
	}
	else if(strncmp(argv[0], "both", strlen("both")) == 0)
	{
		g_web_dbg.disp_mode = WEB_DISPLAY_BOTH;
	}
	else
	{
		vty_out(vty, "Error: para(%s) invalid!%s", argv[0], VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

DEFUN(show_debug_web_packet_display_fun,
       show_debug_web_packet_display_cmd,
       "show debug web packet",
	   "Debug show\n"
	   "Debug control\n"
	   "web module\n"
	   "packet debug\n"
	   "display mode\n")
{

	if(1 == g_web_dbg.tcp_send)
	{
		vty_out(vty, "packet send enable%s", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "packet send disable%s", VTY_NEWLINE);
	}
	
	if(1 == g_web_dbg.tcp_recv)
	{
		vty_out(vty, "packet receive enable%s", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "packet receive disable%s", VTY_NEWLINE);
	}
	
	if(WEB_DISPLAY_CHAR == g_web_dbg.disp_mode)
	{
		vty_out(vty, "debug mode char %s", VTY_NEWLINE);
	}
	else if(WEB_DISPLAY_INT== g_web_dbg.disp_mode)
	{
		vty_out(vty, "debug mode int %s", VTY_NEWLINE);
	}
	else if(WEB_DISPLAY_BOTH== g_web_dbg.disp_mode)
	{
		vty_out(vty, "debug mode both %s", VTY_NEWLINE);
	}
	else
	{
		vty_out(vty, "Error: invalid!%s", VTY_NEWLINE);
		return CMD_WARNING;
	}

	return CMD_SUCCESS;
}

int web_pltfmlog_init(int argc, char **argv)
{
	char *p;
	char *progname;
	/* Get program name. */
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
	
	/* First of all we need logging init. */
	zlog_default = openzlog(progname, ZLOG_WEB,LOG_CONS | LOG_NDELAY | LOG_PID, LOG_DAEMON);
	return 0;

}

int web_cmd_init (void)
{
	install_node(&web_node, NULL);
	install_default(WEB_NODE);
	
	install_element(CONFIG_NODE, &web_mode_enable_cmd, CMD_SYNC);	
	install_element(WEB_NODE, &web_version_config_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &debug_web_packet_enable_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &debug_web_packet_disable_cmd, CMD_SYNC);
	install_element(CONFIG_NODE, &debug_web_packet_display_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &show_debug_web_packet_display_cmd, CMD_LOCAL);
	return 0;
}	

int sys_cpu_init(void)
{
	struct sched_param param;

	cpu_set_t mask;

	CPU_ZERO ( &mask );
	CPU_SET ( 1, &mask );
	sched_setaffinity ( 0, sizeof ( mask ), &mask );

	param.sched_priority = 60;
#ifndef HAVE_KERNEL_3_0		
	if ( sched_setscheduler ( 0, SCHED_RR, &param ) )
	{
		perror ( "\n  priority set: " );
	}
#endif	
	return 0;
}


int web_cli_init(void)
{
	pthread_t thread_id;

	web_cli_master = thread_master_create ();
	cmd_init ( 1 );
	vty_init ( web_cli_master );
	memory_init();
	web_cmd_init();
	
	vty_serv_sock (  WEB_VTYSH_PATH );
	
	pthread_create(&thread_id, NULL, web_cli_thread, NULL) ;
	pthread_detach(thread_id);

	return 0;
}

			
int web_packet_init()
{
	pthread_t   thread_id;
	pthread_mutex_init(&g_web_syn_lock, NULL);
	web_hash_tcp_fd_hash_init(HASHTAB_SIZE);
	web_pkt_register();
	pthread_create(&thread_id, NULL, (void *)web_pkt_thread, NULL);
	pthread_detach(thread_id);
	return 0;
}

