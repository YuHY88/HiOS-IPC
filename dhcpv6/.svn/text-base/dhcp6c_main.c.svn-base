/************************************************************
   @file dhcp6c_main.c
   @brief  This file contains the init part for dhcpv6 client module.
*************************************************************/

/* Copyright (C) 2017 Huahuan, Inc. All Rights Reserved. */

#define _GNU_SOURCE
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/queue.h>
#include <errno.h>
#include <assert.h>

#include <limits.h>
#if TIME_WITH_SYS_TIME
#include <sys/time.h>
#include <time.h>
#else
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#else
#include <time.h>
#endif
#endif
#include <netinet/in.h>

#include <net/if.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <err.h>
#include <ifaddrs.h>

#include <lib/types.h>
#include <lib/thread.h>
#include <lib/log.h>
#include <lib/syslog.h>
#include <lib/pkt_buffer.h>
#include <lib/ifm_common.h>
#include <lib/module_id.h>
#include <lib/msg_ipc.h>
#include <lib/linklist.h>
#include <lib/command.h>
#include <lib/memory.h>
#include <lib/prefix.h>


#include <ftm/pkt_eth.h>
#include <ftm/pkt_ipv6.h>
#include <ftm/pkt_ip.h>
#include <ftm/pkt_udp.h>

#include <dhcp6.h>
#include <dhcpv6_if.h>
#include <common.h>
//#include <dhcpv6_timer.h>
#include <dhcp6c.h>
#include <control.h>
//#include <dhcp6c_ia.h>
//#include <prefixconf.h>
//#include <auth.h>
//#include <sched.h>
#include "../config.h"
#include "dhcp6c_cmd.h"
#include "dhcpv6_packet.h"
#include "dhcpv6_options.h"
#include "dhcpv6_msg.h"
#include "dhcpv6_if.h"


#define ZEBRA_BUG_ADDRESS "https://bugzilla.quagga.net"

#define SIGF_TERM 0x1
#define SIGF_HUP 0x2

const dhcp6_mode_t dhcp6_mode = DHCP6_MODE_CLIENT;

int sock;	/* inbound/outbound udp port */
int ctlsock = -1;		/* control TCP port */
const char *ctladdr = DEFAULT_CLIENT_CONTROL_ADDR;
const char *ctlport = DEFAULT_CLIENT_CONTROL_PORT;

#define DEFAULT_KEYFILE  "/home/bin/dhcp6cctlkey"
#define CTLSKEW 300

static const char *config_file = DHCP6C_CONF;
/* Configuration file and directory. */
//char *config_file = NULL;
/* VTY bind address. */
char *vty_addr = NULL;

static const char *pid_file = DHCP6C_PIDFILE;
extern struct duid client_duid;
extern struct keyinfo *ctlkey;

static const char *ctlkeyfile = DEFAULT_KEYFILE;

extern int ctldigestlen;

struct duid client_duid;
struct keyinfo *ctlkey = NULL;
/* common options. */
static struct option longopts[] =
{
  { "daemon",      		no_argument,       NULL, 'd'},
  { "config_file", 		required_argument, NULL, 'f'},
  { "pid_file",    		required_argument, NULL, 'i'},
  { "help",        		no_argument,       NULL, 'h'},
  { "vty_addr",    		required_argument, NULL, 'A'},
  { "vty_port",    		required_argument, NULL, 'P'},
  { "retain",      		no_argument,       NULL, 'r'},
  { "keyfile",     		required_argument, NULL, 'k'},
  { "version",     		no_argument,       NULL, 'v'},
  { 0 }
};
struct thread_master *dhcpv6_master;

#define MAX_ELAPSED_TIME 0xffff

/* Signale wrapper. */
#ifndef RETSIGTYPE
#define RETSIGTYPE void
#endif
void dhcp6c_die(void);
void client6_init(void);
void dhcpv6_init(void);
void init_header(struct dhcpv6_message *packet, char type);
void client6_recv(struct pkt_buffer *pkt);
int dhcpv6_client_fsm(uint32_t ifindex, struct pkt_buffer *pkt, enum DHCPV6_EVENT type, uint32_t state, iatype_t ia_type);

static RETSIGTYPE *signal_set (int signo, void (*func)(int))
{
	int ret;
	struct sigaction sig;
	struct sigaction osig;

	sig.sa_handler = func;
	sigemptyset (&sig.sa_mask);
	sig.sa_flags = 0;
#ifdef SA_RESTART
	sig.sa_flags |= SA_RESTART;
#endif /* SA_RESTART */

	ret = sigaction (signo, &sig, &osig);

	if (ret < 0)
		return SIG_ERR;
	else
		return osig.sa_handler;
}


/* SIGHUP handler. */
static void sighup (int sig)
{
  zlog_debug("SIGHUP received");

  /* Reload config file */
  //vty_read_config (config_file, config_default);

  /* Create VTY's socket */
  vty_serv_sock (DHCPV6_VTYSH_PATH);

  /* Try to return to normal operation. */
}


/* SIGINT handler. */
static void sigint (int sig)
{
    zlog_debug("SIGINT received\n");

    dhcp6c_die();

    exit(1);
}

/* SIGUSR1 handler. */
static void sigusr1 (int sig)
{
    zlog_debug("SIGUSR1 received\n");

	return;
}

/* SIGUSR2 handler. */
static void sigusr2 (int sig)
{
    zlog_debug("SIGUSR2 received\n");

	return;
}

/* Initialization of signal handles. */
static void signal_init (void)
{
  signal_set (SIGHUP, sighup);
  signal_set (SIGINT, sigint);
  signal_set (SIGTERM, sigint);
  signal_set (SIGPIPE, SIG_IGN);
  signal_set (SIGUSR1, sigusr1);
  signal_set (SIGUSR2, sigusr2);
}


/* Help information display. */
static void usage (char *progname, int status)
{
	if (status != 0)
		fprintf (stderr, "Try `%s --help' for more information.\n", progname);
	else
	{
		printf ("Usage : %s [OPTION...]\n\
		Daemon which manages VRRP version 1 and 2.\n\n\
		-d, --daemon       Runs in daemon mode\n\
		-f, --config_file  Set configuration file name\n\
		-i, --pid_file     Set process identifier file name\n\
		-A, --vty_addr     Set vty's bind address\n\
		-r, --retain       When program terminates, retain added route by vrrp.\n\
		-v, --version      Print program version\n\
		-h, --help         Display this help and exit\n\
		\n\
		Report bugs to %s\n", progname, ZEBRA_BUG_ADDRESS);
	}

	exit (status);
}
/*------------------------------------------------------------*/
void client6_init(void)
{
	/* get our DUID */
	if (get_duid(DUID_FILE, &client_duid)) {
		LOG(LOG_ERR, "failed to get a DUID");
		exit(1);
	}
#if 0
	if (dhcp6_ctl_authinit(ctlkeyfile, &ctlkey, &ctldigestlen) != 0) {
		LOG(LOG_ERR, "failed initialize control message authentication");
		/* run the server anyway */
	}
#endif
}


void dhcpv6_init(void)
{
	client6_init();
	dhcp6c_cmd_init();
	dhcpv6_event_register();
	dhcpv6_pkt_register();
}


void dhcp6c_die()
{
	closezlog(zlog_default);
	exit(0);
}

/* result will be a - b */
static void tv_sub(struct timeval *a, struct timeval *b, struct timeval *result)
{
	if (a->tv_sec < b->tv_sec ||
	    (a->tv_sec == b->tv_sec && a->tv_usec < b->tv_usec)) {
		result->tv_sec = 0;
		result->tv_usec = 0;

		return;
	}

	result->tv_sec = a->tv_sec - b->tv_sec;
	if (a->tv_usec < b->tv_usec) {
		result->tv_usec = a->tv_usec + 1000000 - b->tv_usec;
		result->tv_sec -= 1;
	} else
		result->tv_usec = a->tv_usec - b->tv_usec;

	return;
}


int main(int argc, char *argv[])
{
	char *p;
	int daemon_mode = 0;
	char *progname;
	struct thread thread;
	
	/* Set umask before anything for security */
	umask (0027);
	
	progname = ((p = strrchr (argv[0], '/')) ? ++p : argv[0]);
	struct sched_param param;
	
	cpu_set_t mask; 
		
	CPU_ZERO(&mask);		
	CPU_SET(1, &mask);	 /* 绑定cpu1 */ 
	sched_setaffinity(0, sizeof(mask), &mask);
	param.sched_priority = 30;
#ifndef HAVE_KERNEL_3_0		
	if (sched_setscheduler(0, SCHED_RR, &param)) 
	{   
		perror("\n  priority set: ");
	}
#endif	
	/* First of all we need logging init. */
	zlog_default = openzlog (progname, ZLOG_DHCP6C, LOG_CONS|LOG_NDELAY|LOG_PID, LOG_DAEMON); /* defined in log.h */

	while (1) {
		int opt;
		opt = getopt_long (argc, argv, "cdDfi:p:n:", longopts, 0);
		if (opt == EOF)
		  break;
		switch (opt) {
			case 0:
				break;
			case 'd':
				daemon_mode = 1;
				break;
			case 'f':
				config_file = optarg;
				break;
			case 'A':
				vty_addr = optarg;
				break;
			case 'i':
				pid_file = optarg;
				break;
			case 'k':
				ctlkeyfile = optarg;
				break;
			case 'v':
				print_version (progname);
				exit (0);
				break;
			case 'h':
				usage (progname, 0);
				break;
			default:
				usage (progname, 1);
				break;
		}
	}
	/* Prepare master thread. */
	dhcpv6_master = thread_master_create ();
	/* Library initialization. */

	signal_init ();	

	cmd_init (1);	

	vty_init (dhcpv6_master);

	memory_init();
	/* Change to the daemon program. */
	if (daemon_mode)
	{
		daemon (0, 0); 
	}
	pid_output (pid_file); /* Pid file create. */

	/* Create VTY's socket */

	vty_serv_sock (DHCPV6_VTYSH_PATH);  
	dhcpv6_init();
    /* Execute each thread. */
    while(thread_fetch(dhcpv6_master, &thread))
    {  
        thread_call(&thread);
    }
	dhcp6c_die();
}
#define DHCPV6_OPTION_SIZE 1024

void init_header(struct dhcpv6_message *packet, char type)
{
	memset(packet, 0, sizeof(struct dhcpv6_message));
	packet->msgtype = DHCP6S_INIT;	
	
	switch(type) {
		case DHCP6S_SOLICIT:
			packet->msgtype = DH6_SOLICIT;
			break;
		case DHCP6S_REQUEST:
			packet->msgtype = DH6_REQUEST;
			break;
		case DHCP6S_RENEW:
			packet->msgtype = DH6_RENEW;
			break;
		case DHCP6S_REBIND:
			packet->msgtype = DH6_REBIND;
			break;
		case DHCP6S_RELEASE:
			packet->msgtype = DH6_RELEASE;
			break;
		case DHCP6S_INFOREQ:
			packet->msgtype = DH6_INFORM_REQ;
			break;
		default:
			DHCPV6_LOG_ERR("unexpected state");
			return;	/* XXX */
	}
	
}

void client6_send(struct dhcpv6_if *ifp)
{
	DHCPV6_LOG_DEBUG("dhcpv6 client send....");
	struct dhcpv6_message *message = NULL;
	struct option_ia_na opia_na;
	struct option_ia opia;
	uint32_t option_start = 0;
	uint32_t option_end = 0;
	
	if(ifp == NULL)
	{
		DHCPV6_LOG_ERR("Input ifp valid is null!\n");
		return;
	}
	message = XMALLOC(MTYPE_IF, sizeof(struct dhcpv6_message));
	if(message == NULL)
	{
		DHCPV6_LOG_ERR("XMALLOC struct dhcpv6_message failed!\n");
		return;
	}
	init_header(message, ifp->event.state);

	//if (ev->timeouts == 0) {
		/*
		 * A client SHOULD generate a random number that cannot easily
		 * be guessed or predicted to use as the transaction ID for
		 * each new message it sends.
		 *
		 * A client MUST leave the transaction-ID unchanged in
		 * retransmissions of a message. [RFC3315 15.1]
		 */
		ifp->xid = random() & DH6_XIDMASK;
		DHCPV6_LOG_DEBUG("a new XID (%x) is generated", ifp->xid);
	//}
	message->xid &= ~ntohl(DH6_XIDMASK);
	message->xid |= htonl(ifp->xid);
	
	DHCPV6_LOG_DEBUG("dhcpv6 client state is %d!", ifp->event.state);

	/* server ID */
	switch (ifp->event.state) {
		case DHCP6S_REQUEST:
		case DHCP6S_RENEW:
		case DHCP6S_RELEASE:
			DHCPV6_LOG_DEBUG("copy server id from options!");
			/* 收包时获取server id */
			option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_SERVERID, ifp->server_id.duid_len, ifp->server_id.duid_id);
			option_start = option_start + option_end;
			break;
	}

	/* client ID */
	DHCPV6_LOG_DEBUG("dhcpv6 client copy duid!");
	/* 添加client id */
	option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_CLIENTID, client_duid.duid_len, client_duid.duid_id);
	option_start = option_start + option_end;
	/* rapid commit (in Solicit only) */
	if (ifp->event.state == DHCP6S_SOLICIT &&
	    (ifp->send_flags & DHCIFF_RAPID_COMMIT)) {
			ifp->rapidcommit = 1;
	}
		
	if(ifp->ia.ia_type == IATYPE_NA)
	{
		memset(&opia, 0, sizeof(struct option_ia));
		memset(&opia_na, 0, sizeof(struct option_ia_na));
		if(ifp->event.state != DHCP6S_SOLICIT)
		{
			/* 添加IANA option头部信息 */
			opia.ia_na.iaid = htonl(ifp->ia.iaid);
			opia.ia_na.t1 = ifp->ia.t1;
			opia.ia_na.t2 = ifp->ia.t2;
			opia.code = htons(DH6OPT_IAADDR);
			opia.len = htons(OPTV6_LEN_IA_AD);
			memcpy(&(opia.ia_suboption.ia_ad.ipv6_address), &(ifp->ia.ia.iana.addr), 16);
			opia.ia_suboption.ia_ad.pre_time = ifp->ia.ia.iana.pre_time;			
			opia.ia_suboption.ia_ad.valid_time = ifp->ia.ia.iana.valid_time;
			//opia.code2 = htons(DH6OPT_STATUS_CODE);
			//opia.len2 = htons(OPTV6_LEN_IANA_STATUS_CODE);			
			//opia.status_code = htons(DH6OPT_STCODE_SUCCESS);
			option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_IA_NA, OPTV6_LEN_IA_NA + OPTV6_LEN_IA_AD + OPTV6_LEN_IANA_STATUS_CODE + OPTV6_LEN *2 + OPTV6_CODE * 2, (uint8_t *)&opia);
			option_start = option_start + option_end;
	
		}else{
			opia_na.iaid = htonl(ifp->ifindex);
			option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_IA_NA, OPTV6_LEN_IA_NA, (uint8_t *)&opia_na);
			option_start = option_start + option_end;
		}
	}
	else if(ifp->ia.ia_type == IATYPE_PD)
	{
		memset(&opia, 0, sizeof(struct option_ia));
		memset(&opia_na, 0, sizeof(struct option_ia_na));
		if(ifp->event.state != DHCP6S_SOLICIT)
		{
			/* 添加IAPD option头部信息 */
			opia.ia_na.iaid = htonl(ifp->ia.iaid);
			opia.ia_na.t1 = ifp->ia.t1;
			opia.ia_na.t2 = ifp->ia.t2;
			opia.code = htons(DH6OPT_IAPREFIX);
			opia.len = htons(OPTV6_LEN_IA_PREFIX);
			memcpy(&(opia.ia_suboption.ia_pd.prefix_address), &(ifp->ia.ia.iapd.prefix_addr), 16);
			opia.ia_suboption.ia_pd.prefix_len = ifp->ia.ia.iapd.prefix_len;
			opia.ia_suboption.ia_pd.pre_time = ifp->ia.ia.iapd.pre_time;			
			opia.ia_suboption.ia_pd.valid_time = ifp->ia.ia.iapd.valid_time;
			//opia.code2 = htons(DH6OPT_STATUS_CODE);
			//opia.len2 = htons(OPTV6_LEN_IANA_STATUS_CODE);			
			//opia.status_code = htons(DH6OPT_STCODE_SUCCESS);
			option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_IA_PD, OPTV6_LEN_IA_PD + OPTV6_LEN_IA_PREFIX + OPTV6_LEN + OPTV6_CODE, (uint8_t *)&opia);
			option_start = option_start + option_end;
	
		}else{
			opia_na.iaid = htonl(ifp->ifindex);
			option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_IA_PD, OPTV6_LEN_IA_PD, (uint8_t *)&opia_na);
			option_start = option_start + option_end;
		}
	}
	
	DHCPV6_LOG_DEBUG("dhcpv6 client get time!");	
	/* elapsed time */
	if (ifp->event.timeouts == 0) {
		gettimeofday(&(ifp->event.tv_start), NULL);
		option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_ELAPSED_TIME, 2, NULL);
		option_start = option_start + option_end;
	} else {
		struct timeval now, tv_diff;
		long et;

		gettimeofday(&now, NULL);
		tv_sub(&now, &(ifp->event.tv_start), &tv_diff);

		/*
		 * The client uses the value 0xffff to represent any elapsed
		 * time values greater than the largest time value that can be
		 * represented in the Elapsed Time option.
		 * [RFC3315 22.9.]
		 */
		if (tv_diff.tv_sec >= (MAX_ELAPSED_TIME / 100) + 1) {
			/*
			 * Perhaps we are nervous too much, but without this
			 * additional check, we would see an overflow in 248
			 * days (of no responses). 
			 */
			et = MAX_ELAPSED_TIME;
		} else {
			et = tv_diff.tv_sec * 100 + tv_diff.tv_usec / 10000;
			if (et >= MAX_ELAPSED_TIME)
				et = MAX_ELAPSED_TIME;
		}
		DHCPV6_LOG_DEBUG("elapsed time is %ld!\n", et);
		option_end = dhcpv6_add_option(message->options, option_start, DH6OPT_ELAPSED_TIME, 2, (uint8_t *)&et);
		option_start = option_start + option_end;
	}
	DHCPV6_LOG_DEBUG("dhcpv6 client get elapsed time is %d!", ifp->event.timeouts);
	/* option request options */
	DHCPV6_LOG_DEBUG("if is not release packet, copy request option list from interface request option list!");
	if (ifp->event.state != DHCP6S_RELEASE ) {
		option_end = dhcpv6_add_request(message->options, option_start);	
		option_start = option_start + option_end;
	}
	/* authentication information */
#if 0
	if (set_auth(ev, &optinfo)) {
		LOG(LOG_ERR, "failed to set authentication option");
		goto end;
	}
	/* set options in the message */
	/* 把选项结构转换成输出缓冲区中的线格式，并返回选项长度，放入缓冲区 */
	if ((optlen = dhcp6_set_options(dh6->dh6_msgtype,
	    (struct dhcp6opt *)(dh6 + 1),
	    (struct dhcp6opt *)(option_string + sizeof(option_string)), &optinfo)) < 0) {
		LOG(LOG_ERR, "failed to construct options");
		goto end;
	}
	DHCPV6_LOG_DEBUG("dhcpv6 client buf len is %d, optlen is %d!", len , optlen);
	DHCPV6_LOG_DEBUG("dhcpv6 client struct dhcp6 len is %d, struct dhcp6_optinfo len is %d!", sizeof(struct dhcp6) , sizeof(struct dhcp6_optinfo));
	len += optlen;

	/* calculate MAC if necessary, and put it to the message */
	if (event->authparam != NULL) {
		switch (ev->authparam->authproto) {
			DHCPV6_LOG_DEBUG("dhcpv6 client ev->authparam->authproto is %d!", ev->authparam->authproto);
			case DHCP6_AUTHPROTO_DELAYED:
				if (ev->authparam->key == NULL) {
					DHCPV6_LOG_DEBUG("dhcpv6 client ev->authparam->key is null!");
					break;
				}
				DHCPV6_LOG_DEBUG("dhcpv6 calc mac!");
				if (dhcp6_calc_mac((char *)dh6, len,
				    optinfo.authproto, optinfo.authalgorithm,
				    optinfo.delayedauth_offset + sizeof(*dh6),
				    ev->authparam->key)) {
					LOG(LOG_ERR, "failed to calculate MAC");
					goto end;
				}
				break;
			default:
				break;	/* do nothing */
		}
	} else {
		DHCPV6_LOG_DEBUG("dhcpv6 client ev->authparam is null!");
	}

	/*
	 * Unless otherwise specified in this document or in a document that
	 * describes how IPv6 is carried over a specific type of link (for link
	 * types that do not support multicast), a client sends DHCP messages
	 * to the All_DHCP_Relay_Agents_and_Servers.
	 * [RFC3315 Section 13.]
	 */
    ifindex = ev->ifp->ifid;
	
#endif
	//DHCPV6_LOG_DEBUG("get ifindex is 0x%x, len is %d\n", ifp->ifindex, sizeof(msg));
	uint8_t *msg = NULL;
	msg = XMALLOC(MTYPE_IF, 256);
	if(msg == NULL)
	{	
		DHCPV6_LOG_DEBUG("XMALLOC is failed!\n");
		return;
	}
	memset(msg, 0, 256);
	msg[0] = message->msgtype;

	/* xid 类型为 uint32_t, 大小为4btye, 但实质上在包中xid只占3btype, 所以取xid后三位放入包中 */
	msg[1] = ((uint8_t *)&(message->xid))[1];
	msg[2] = ((uint8_t *)&(message->xid))[2];	
	msg[3] = ((uint8_t *)&(message->xid))[3];
	memcpy(msg + 5, message->options, 160);
	if (send_multicast_packet(msg, ifp->ifindex, option_start + 4)) {
		DHCPV6_LOG_DEBUG("ifindex 0x%x send multicast packet failed!\n", ifp->ifindex);
		return;
	}
	if(ifp->event.state == DHCP6S_RELEASE)
	{
		if(ifp->ifindex && ifp->ia.ia.iapd.prefix_addr && ifp->ia.ia.iapd.prefix_len)
		{
			dhcpv6_ip_address_set(ifp->ifindex, ifp->ia.ia.iapd.prefix_addr, ifp->ia.ia.iapd.prefix_len, TRUE, TRUE);
		}
	}
	DHCPV6_LOG_DEBUG("ifindex 0x%x send multicast packet successed!\n", ifp->ifindex);
	return;
}

int dhcpv6_ip_address_set(uint32_t ifindex, uint8_t *ip_addr, uint32_t mask, uchar pd_flag, uchar del_flag)
{
	struct ifm_l3 l3;	
	enum IFNET_EVENT event;
	int ret=0;
	LOG(LOG_DEBUG,"dhcpv6_ip_address_set, ifindex:0x%x ,mask:%08X,del_flag:%d\n",ifindex,mask,del_flag);

	if (ip_addr != NULL) {
		DHCPV6_LOG_DEBUG("ip_addr is %s, u16 is %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x", 
		ip_addr, ip_addr[0],ip_addr[1],ip_addr[2],ip_addr[3],
		ip_addr[4],ip_addr[5],ip_addr[6],ip_addr[7],
		ip_addr[8],ip_addr[9],ip_addr[10],ip_addr[11],
		ip_addr[12],ip_addr[13],ip_addr[14],ip_addr[15]);
	}
	memset(&l3,0,sizeof(struct ifm_l3));
	if(!del_flag)
	{
		if (ip_addr != NULL) {	
			memcpy(l3.ipv6[0].addr, ip_addr, sizeof(l3.ipv6[0].addr));
		}
		DHCPV6_LOG_DEBUG("ip_addr is  %x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x:%x%x", 
		l3.ipv6[0].addr[0],l3.ipv6[0].addr[1],l3.ipv6[0].addr[2],l3.ipv6[0].addr[3],
		l3.ipv6[0].addr[4],l3.ipv6[0].addr[5],l3.ipv6[0].addr[6],l3.ipv6[0].addr[7],
		l3.ipv6[0].addr[8],l3.ipv6[0].addr[9],l3.ipv6[0].addr[10],l3.ipv6[0].addr[11],
		l3.ipv6[0].addr[12],l3.ipv6[0].addr[13],l3.ipv6[0].addr[14],l3.ipv6[0].addr[15]);
		l3.ipv6[0].prefixlen = mask;
		l3.vpn = 0;
		if(pd_flag)
		{
			l3.ipv6_flag = IP_TYPE_DHCP_PD;
		}else
		{
			l3.ipv6_flag = IP_TYPE_DHCP;
		}
		event = IFNET_EVENT_IP_ADD;
		
		ret = ipc_send_common_wait_ack(&l3 ,sizeof(struct ifm_l3),1,MODULE_ID_ROUTE,MODULE_ID_DHCPV6,
			IPC_TYPE_L3IF,event,IPC_OPCODE_EVENT,ifindex);
	}else{
		event = IFNET_EVENT_IP_DELETE;
		
		ret = ipc_send_common1(&l3 ,sizeof(struct ifm_l3),1,MODULE_ID_ROUTE,MODULE_ID_DHCPV6,
			IPC_TYPE_L3IF,event,IPC_OPCODE_EVENT,ifindex);
	}

	if(ret)
		LOG(LOG_DEBUG,"dhcpv6_ip_address_set error!! ifindex:%d ,mask:%08X,del_flag:%d, ret: %d\n",ifindex ,mask,del_flag,ret);
	return ret;
}

static int client6_recvadvert(struct dhcpv6_if *ifp, struct dhcpv6_message *message)
{
	DHCPV6_LOG_DEBUG("dhcpv6 client advertise packet!");
	if(ifp == NULL)
	{
		DHCPV6_LOG_ERR("ifp is null!\n");
		return -1;
	}

	
	/* validate authentication */
	/* 验证以后处理
	authparam0 = *ev->authparam;
	if (process_auth(&authparam0, dh6, len, optinfo)) {
		LOG(LOG_ERR, "failed to process authentication");
		return -1;
	}
	
	*/
	/*
	 * The requesting router MUST ignore any Advertise message that
	 * includes a Status Code option containing the value NoPrefixAvail
	 * [RFC3633 Section 11.1].
	 * Likewise, the client MUST ignore any Advertise message that includes
	 * a Status Code option containing the value NoAddrsAvail. 
	 * [RFC3315 Section 17.1.3].
	 * We only apply this when we are going to request an address or
	 * a prefix.
	 */

	if((ifp->ia.ia_type == IATYPE_NA)&&(ifp->ia.ia.iana.status_code.code == DH6OPT_STCODE_NOADDRSAVAIL))
	{
		DHCPV6_LOG_ERR("advertise contains NoAddrsAvail status!\n");
		return -1;
	}else if((ifp->ia.ia_type == IATYPE_PD)&&(ifp->ia.ia.iapd.status_code.code == DH6OPT_STCODE_NOPREFIXAVAIL))
	{
		DHCPV6_LOG_ERR("advertise contains NoPrefixAvail status!\n");
		return -1;
	}
	if (ifp->event.state != DHCP6S_SOLICIT ||
	    (ifp->send_flags & DHCIFF_RAPID_COMMIT)) {
		/*
		 * We expected a reply message, but do actually receive an
		 * Advertise message.  The server should be configured not to
		 * allow the Rapid Commit option.
		 * We process the message as if we expected the Advertise.
		 * [RFC3315 Section 17.1.4]
		 */
		LOG(LOG_ERR, "unexpected advertise");
		/* proceed anyway */
	}
	/* ignore the server if it is known */
	/*后续处理
	if (find_server(ev, &optinfo->serverID)) {
		LOG(LOG_ERR, "duplicated server (ID: %s)",
		    duidstr(&optinfo->serverID));
		return -1;
	}
	*/
	/* keep the server */
	#if 0
	/* 与process_auth一同处理 */
	if ((newserver = malloc(sizeof(*newserver))) == NULL) {
		LOG(LOG_ERR, "memory allocation failed for server");
		return -1;
	}
	memset(newserver, 0, sizeof(*newserver));

	/* remember authentication parameters */
	newserver->authparam = ev->authparam;
	newserver->authparam->flags = authparam0.flags;
	newserver->authparam->prevrd = authparam0.prevrd;
	newserver->authparam->key = authparam0.key;
	/* allocate new authentication parameter for the soliciting event */
	if ((authparam = new_authparam(ev->authparam->authproto,
	    ev->authparam->authalgorithm, ev->authparam->authrdm)) == NULL) {
		LOG(LOG_ERR, "memory allocation failed for authentication parameters");
		free(newserver);
		return -1;
	}
	ev->authparam = authparam;
	/* copy options */
	dhcp6_init_options(&newserver->optinfo);
	if (dhcp6_copy_options(&newserver->optinfo, optinfo)) {
		LOG(LOG_ERR, "failed to copy options");
		if (newserver->authparam != NULL)
			free(newserver->authparam);
		free(newserver);
		return -1;
	}
	if (optinfo->pref != DH6OPT_PREF_UNDEF)
		newserver->pref = optinfo->pref;
	newserver->active = 1;
	for (sp = &ev->servers; *sp; sp = &(*sp)->next) {
		if ((*sp)->pref != DH6OPT_PREF_MAX &&
		    (*sp)->pref < newserver->pref) {
			break;
		}
	}
	newserver->next = *sp;
	*sp = newserver;

	if (newserver->pref == DH6OPT_PREF_MAX) {
		/*
		 * If the client receives an Advertise message that includes a
		 * Preference option with a preference value of 255, the client
		 * immediately begins a client-initiated message exchange.
		 * [RFC3315 Section 17.1.2]
		 */
		ev->current_server = newserver;
		if (duidcpy(&ev->serverid,
		    &ev->current_server->optinfo.serverID)) {
			LOG(LOG_ERR, "can't copy server ID");
			return (-1); /* XXX: better recovery? */
		}
		if (construct_reqdata(ifp, &ev->current_server->optinfo, ev)) {
			LOG(LOG_ERR, "failed to construct request data");
			return (-1); /* XXX */
		}

		ifp->event->timeouts = 0;
		ifp->event->state = DHCP6S_REQUEST;

		//free(ev->authparam);
		//ev->authparam = newserver->authparam;
		//newserver->authparam = NULL;

		client6_send(ifp);

		//dhcp6_set_timeoparam(ev);
		//dhcp6_reset_timer(ev);
	} else if (ev->servers->next == NULL) {
		struct timeval *rest, elapsed, tv_rt, tv_irt, timo;

		/*
		 * If this is the first advertise, adjust the timer so that
		 * the client can collect other servers until IRT elapses.
		 * XXX: we did not want to do such "low level" timer
		 *      calculation here.
		 */
		rest = dhcp6_timer_rest(ev->timer);
		tv_rt.tv_sec = (ev->retrans * 1000) / 1000000;
		tv_rt.tv_usec = (ev->retrans * 1000) % 1000000;
		tv_irt.tv_sec = (ev->init_retrans * 1000) / 1000000;
		tv_irt.tv_usec = (ev->init_retrans * 1000) % 1000000;
		timeval_sub(&tv_rt, rest, &elapsed);
		if (TIMEVALE_LEQ(elapsed, tv_irt))
			timeval_sub(&tv_irt, &elapsed, &timo);
		else
			timo.tv_sec = timo.tv_usec = 0;

		LOG(LOG_ERR, "reset timer for %s to %d.%06d",
		    ifp->ifname, (int)timo.tv_sec, (int)timo.tv_usec);

		dhcp6_set_timer(&timo, ev->timer);
	}
	#endif
	ifp->event.timeouts = 0;
	ifp->event.state = DHCP6S_REQUEST;
	
	//free(ev->authparam);
	//ev->authparam = newserver->authparam;
	//newserver->authparam = NULL;
	
	client6_send(ifp);

	return 0;
}

static int client6_recvreply(struct dhcpv6_if *ifp, struct dhcpv6_message *message)
{
	//struct dhcpv6_event *event = NULL;
	uint32_t state = 0;
	
	state = ifp->event.state;
	if (state != DHCP6S_INFOREQ &&
	    state != DHCP6S_REQUEST &&
	    state != DHCP6S_RENEW &&
	    state != DHCP6S_REBIND &&
	    state != DHCP6S_RELEASE &&
	    (state != DHCP6S_SOLICIT ||
	     !(ifp->send_flags & DHCIFF_RAPID_COMMIT))) {
		DHCPV6_LOG_ERR("unexpected reply");
		return -1;
	}

	/* A Reply message must contain a Server ID option */
	if (ifp->server_id.duid_len == 0) {
		DHCPV6_LOG_ERR("no server ID option");
		return -1;
	}

	/*
	 * DUID in the Client ID option (which must be contained for our
	 * client implementation) must match ours.
	 */
	if (ifp->client_id.duid_len == 0) {
		DHCPV6_LOG_ERR("no client ID option");
		return -1;
	}
	if (duidcmp(&(ifp->client_id), &client_duid)) {
		DHCPV6_LOG_ERR("client DUID mismatch");
		return -1;
	}
	/* validate authentication */
	/* 后续处理
	if (process_auth(ev->authparam, dh6, len, optinfo)) {
		LOG(LOG_ERR, "failed to process authentication");
		return -1;
	}
	*/
	/*
	 * If the client included a Rapid Commit option in the Solicit message,
	 * the client discards any Reply messages it receives that do not
	 * include a Rapid Commit option.
	 * (should we keep the server otherwise?)
	 * [RFC3315 Section 17.1.4]
	 */
	if (state == DHCP6S_SOLICIT &&
	    (ifp->send_flags & DHCIFF_RAPID_COMMIT) &&
	    !ifp->rapidcommit) {
		DHCPV6_LOG_ERR("no rapid commit");
		return -1;
	}

	/*
	 * The client MAY choose to report any status code or message from the
	 * status code option in the Reply message.
	 * [RFC3315 Section 18.1.8]
	 */
	if(ifp->ia.ia.iana.status_code.code)
	{
		DHCPV6_LOG_DEBUG("iana address status code: %s", dhcp6_stcodestr(ifp->ia.ia.iana.status_code.code));
	}
	/* nameserver dnsname ntpserver SIPserver SIPDomainName
	if (!TAILQ_EMPTY(&optinfo->dns_list)) {
		struct dhcp6_listval *d;
		int i = 0;

		for (d = TAILQ_FIRST(&optinfo->dns_list); d;
		     d = TAILQ_NEXT(d, link), i++) {
			LOG(LOG_DEBUG, "nameserver[%d] %s", i, in6addr2str(&d->val_addr6, 0));
		}
	}

	if (!TAILQ_EMPTY(&optinfo->dnsname_list)) {
		struct dhcp6_listval *d;
		int i = 0;

		for (d = TAILQ_FIRST(&optinfo->dnsname_list); d;
		     d = TAILQ_NEXT(d, link), i++) {
			LOG(LOG_DEBUG, "Domain search list[%d] %s", i, d->val_vbuf.dv_buf);
		}
	}

	if (!TAILQ_EMPTY(&optinfo->ntp_list)) {
		struct dhcp6_listval *d;
		int i = 0;

		for (d = TAILQ_FIRST(&optinfo->ntp_list); d;
		     d = TAILQ_NEXT(d, link), i++) {
			LOG(LOG_DEBUG, "NTP server[%d] %s",i, in6addr2str(&d->val_addr6, 0));
		}
	}

	if (!TAILQ_EMPTY(&optinfo->sip_list)) {
		struct dhcp6_listval *d;
		int i = 0;

		for (d = TAILQ_FIRST(&optinfo->sip_list); d;
		     d = TAILQ_NEXT(d, link), i++) {
			LOG(LOG_DEBUG, "SIP server address[%d] %s", i, in6addr2str(&d->val_addr6, 0));
		}
	}

	if (!TAILQ_EMPTY(&optinfo->sipname_list)) {
		struct dhcp6_listval *d;
		int i = 0;

		for (d = TAILQ_FIRST(&optinfo->sipname_list); d;
		     d = TAILQ_NEXT(d, link), i++) {
			LOG(LOG_DEBUG, "SIP domain name[%d] %s",i, d->val_vbuf.dv_buf);
		}
	}
*/
	
	/*
	 * Set refresh timer for configuration information specified in
	 * information-request.  If the timer value is specified by the server
	 * in an information refresh time option, use it; use the protocol
	 * default otherwise.
	 */
	#if 0
	option 更新时间 以后处理
	if (state == DHCP6S_INFOREQ) {
		int64_t refreshtime = DHCP6_IRT_DEFAULT;

		if (ifp->refreshtime != DH6OPT_REFRESHTIME_UNDEF)
			refreshtime = ifp->refreshtime;

		ifp->timer = dhcp6_add_timer(client6_expire_refreshtime, ifp);
		if (ifp->timer == NULL) {
			LOG(LOG_DEBUG, "failed to add timer for refresh time");
		} else {
			struct timeval tv;

			tv.tv_sec = (long)refreshtime;
			tv.tv_usec = 0;

			if (tv.tv_sec < 0) {
				/*
				 * XXX: tv_sec can overflow for an
				 * unsigned 32bit value.
				 */
				LOG(LOG_DEBUG, "refresh time is too large: %llu",
				    (u_int64_t)refreshtime);
				tv.tv_sec = 0x7fffffff;	/* XXX */
			}

			dhcp6_set_timer(&tv, ifp->timer);
		}
	} else if (optinfo->refreshtime != DH6OPT_REFRESHTIME_UNDEF) {
		/*
		 * draft-ietf-dhc-lifetime-02 clarifies that refresh time
		 * is only used for information-request and reply exchanges.
		 */
		LOG(LOG_DEBUG, "unexpected information refresh time option (ignored)");
	}
	#endif
	/* update stateful configuration information */
	/*
	if (state != DHCP6S_RELEASE) {
		update_ia(IATYPE_PD, &optinfo->iapd_list, ifp,
		    &optinfo->serverID, ev->authparam);
		update_ia(IATYPE_NA, &optinfo->iana_list, ifp,
		    &optinfo->serverID, ev->authparam);
	}
	
	dhcp6_remove_event(ev);
	*/

	if (state == DHCP6S_RELEASE) {
		/*
		 * When the client receives a valid Reply message in response
		 * to a Release message, the client considers the Release event
		 * completed, regardless of the Status Code option(s) returned
		 * by the server.
		 * [RFC3315 Section 18.1.8]
		 */
		//check_exit();
	}

	LOG(LOG_DEBUG, "got an expected reply, sleeping.");
	if(ifp->ia.ia_type == IATYPE_NA)
	{
		dhcpv6_ip_address_set(ifp->ifindex, ifp->ia.ia.iana.addr, ifp->ia.ia.iana.mask, FALSE, FALSE);
	}else if(ifp->ia.ia_type == IATYPE_PD)
	{
		dhcpv6_ip_address_set(ifp->ifindex, ifp->ia.ia.iapd.prefix_addr, ifp->ia.ia.iapd.prefix_len, TRUE, FALSE);
	}
	return 0;
}

void client6_recv(struct pkt_buffer *pkt)
{
	struct dhcpv6_if *ifp = NULL;
	struct dhcpv6_message *message = NULL;
	if (pkt != NULL) {
		ifp = dhcpv6_if_lookup(pkt->in_ifindex);
		if(ifp == NULL)
		{
			DHCPV6_LOG_ERR("dhcpv6 find by ifindex is failed!\n");
			return;
		}
		DHCPV6_LOG_DEBUG(" pkt->data_len %d",  pkt->data_len);
		message = XMALLOC(MTYPE_IF, pkt->data_len);
		if(message == NULL)
		{
			DHCPV6_LOG_ERR("XMALLOC struct dhcpv6_message is failed!\n");
			return;
		}
		message->msgtype = ((unsigned char *)(pkt->data))[0];
		memcpy(&(message->xid), ((unsigned char *)(pkt->data)) + 1, 3);
		message->xid = ntohl(message->xid);
		LOG(LOG_DEBUG, "receive type %s, xid is %x\n", dhcp6msgstr(message->msgtype), message->xid);
		dhcpv6_parse_options(ifp, ((unsigned char *)(pkt->data)) + 4);
		switch(message->msgtype) {
			case DH6_ADVERTISE:
				(void)client6_recvadvert(ifp, message);
				break;
			case DH6_REPLY:
				(void)client6_recvreply(ifp, message);
				break;
			default:
				LOG(LOG_ERR, "received an unexpected message (%s) ", dhcp6msgstr(message->msgtype));
				break;
		}

	}
	return;
}



int dhcpv6_client_fsm(uint32_t ifindex, struct pkt_buffer *pkt, enum DHCPV6_EVENT type, uint32_t state, iatype_t ia_type )
{
	
	DHCPV6_LOG_DEBUG("Enter client6_timo function!");

	struct dhcpv6_if *ifp = NULL;
	/* ??send部分是否可以精简?? */
	if (pkt != NULL && pkt->data != NULL) {
		client6_recv(pkt);
	} else {
		/* 目前dhcpv6只满足地址获取，不满足前缀获取 */
		if((ia_type != IATYPE_NA) && (ia_type != IATYPE_PD))
		{
			DHCPV6_LOG_DEBUG("Ia type is error!\n");
			return -1;
		}
		
		ifp = dhcpv6_if_lookup(ifindex);
		if(ifp == NULL)
		{
			DHCPV6_LOG_DEBUG("find dhcpv6 ifp is failed!\n");
			return -1;
		}
		
		switch(ifp->event.state) {
			case DHCP6S_INIT:
				//ev->timeouts = 0; /* indicate to generate a new XID. */
				if ((ifp->send_flags & DHCIFF_INFO_ONLY))
					ifp->event.state = DHCP6S_INFOREQ;
				else {
					ifp->event.state = DHCP6S_SOLICIT;
				}
				/* fall through */
			case DHCP6S_REQUEST:
			case DHCP6S_RELEASE:
			case DHCP6S_INFOREQ:
				client6_send(ifp);
				break;
			case DHCP6S_RENEW:
			case DHCP6S_REBIND:
				client6_send(ifp);
				break;
			case DHCP6S_SOLICIT:
				
#if 0
				if (ev->servers) {
					/*
					 * Send a Request to the best server.
					 * Note that when we set Rapid-commit in Solicit,
					 * but a direct Reply has been delayed (very much),
					 * the transition to DHCP6S_REQUEST (and the change of
					 * transaction ID) will invalidate the reply even if it
					 * ever arrives.
					 */
					ev->current_server = select_server(ev);
					if (ev->current_server == NULL) {
						/* this should not happen! */
						LOG(LOG_ERR, "can't find a server");
						exit(1); /* XXX */
					}
					if (duidcpy(&ev->serverid,
						&ev->current_server->optinfo.serverID)) {
						LOG(LOG_ERR, "can't copy server ID");
						return NULL; /* XXX: better recovery? */
					}
					ev->timeouts = 0;
					ev->state = DHCP6S_REQUEST;
					dhcp6_set_timeoparam(ev);

					if (ev->authparam != NULL)
						free(ev->authparam);
					ev->authparam = ev->current_server->authparam;
					ev->current_server->authparam = NULL;
					
					if (construct_reqdata(ifp,
						&ev->current_server->optinfo, ev)) {
						LOG(LOG_DEBUG, "failed to construct request data");
						break;
					}
				}
				
#endif
				client6_send(ifp);
				break;
				default:
					DHCPV6_LOG_DEBUG("dhcpv6 ifp event  state is error!\n");
					return -1;
		}
	}
	//dhcp6_reset_timer(ev);
	
	return 0;
}

