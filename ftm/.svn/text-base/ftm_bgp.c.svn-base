
#include <stdio.h>
#include <string.h>
#include <lib/zassert.h>
#include <lib/inet_ip.h>
#include <lib/msg_ipc_n.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/linklist.h>
#include <lib/pkt_buffer.h>
#include <lib/pkt_type.h>
#include <lib/thread.h>
#include <lib/ether.h>
#include <lib/index.h>
#include <lib/log.h>
#include <lib/md5.h>
#include <lib/errcode.h>
#include <lib/ifm_common.h>

#include "ftm_bgp.h"


int bgp_port_swith = 0;
struct bgp_tcp_connect_with_md5 bgp_peer_entry[PEER_MAX_NUM];

int bgp_peer_entry_add(uint32_t sip,uint32_t dip,char *pass);
int bgp_peer_entry_del(uint32_t sip,uint32_t dip);
//int bgp_md5_digest_make(int flag, struct tcphdr*th, char *data,int data_len, char* digest, int dir);
void bgp_port_switch_set(int enable)
{
	bgp_port_swith = enable; 
}

int bgp_peer_entry_lookup(uint32_t sip,uint32_t dip)
{
	int i = 0;

	for( i=0; i<PEER_MAX_NUM; i++ )
	{
		if( bgp_peer_entry[i].used_flag == 0 )
			continue;
		if(( bgp_peer_entry[i].sip == sip )&&( bgp_peer_entry[i].dip == dip ))
			return i;
	}
	
	return -1;
}

int bgp_peer_entry_add(uint32_t sip,uint32_t dip,char *pass)
{
	int i = 0;
	int pass_len = strlen(pass);
	int ret = 0;

	ret = bgp_peer_entry_lookup(sip,dip);
	if(ret != -1)
	{
		memset(bgp_peer_entry[ret].passwd,0,16);
		memcpy(bgp_peer_entry[ret].passwd,pass,pass_len);
		return 0;
	}
	
	for( i=0; i<PEER_MAX_NUM; i++ )
	{
		if( bgp_peer_entry[i].used_flag != 0 )
			continue;
		
		bgp_peer_entry[i].used_flag = 1;
		bgp_peer_entry[i].sip = sip;
		bgp_peer_entry[i].dip = dip;
		memcpy(bgp_peer_entry[i].passwd,pass,pass_len);
		//bgp_peer_entry_show();
		
		return 0;
	}
	
	return -1;
}

int bgp_peer_entry_del(uint32_t sip,uint32_t dip)
{
	int ret = 0;
	
	if((ret = bgp_peer_entry_lookup(sip,dip)) == -1)
		return -1;

	bgp_peer_entry[ret].sip = 0;
	bgp_peer_entry[ret].dip = 0;
	bgp_peer_entry[ret].used_flag = 0;
	memset(bgp_peer_entry[ret].passwd,0,16);
	//bgp_peer_entry_show();
	return -1;
}

#if 0
int bgp_peer_entry_show(void)
{
	int i = 0;

	zlog(NULL,NULL,"----bgp_peer_entry_show----\n");
	for( i=0; i<PEER_MAX_NUM; i++ )
	{
		if( bgp_peer_entry[i].used_flag == 0 )
			continue;
		zlog(NULL,NULL, "peer [%d]\n", i);
		zlog(NULL,NULL, "sip: %X \n", bgp_peer_entry[i].sip);
		zlog(NULL,NULL, "dip: %X \n", bgp_peer_entry[i].dip);
		zlog(NULL,NULL, "passwd : %s\n", bgp_peer_entry[i].passwd);
		zlog(NULL,NULL, "----bgp_peer [%d] over----\n", i);
	}

	return 0;
}
#endif

/*process the msg from BGP,set the flag of BGP */
int ftm_bgp_msg_proc(void *data,int data_len,uint16_t sender_id, uint8_t msg_subtype)
{
	struct peer_entry *temp_entry;
	
	if( sender_id == MODULE_ID_BGP )
	{
		if(msg_subtype == BGP_ENABLE_MSG)
		{
			bgp_port_switch_set(1);
		}
		else if(msg_subtype == BGP_DISABLE_MSG)
		{
			bgp_port_switch_set(0);
		}
		else if(msg_subtype == BGP_SET_PASSWD_MSG)
		{
			temp_entry = (struct peer_entry *)data;
			bgp_peer_entry_add(temp_entry->sip,temp_entry->dip,temp_entry->pass);
		}
		else if(msg_subtype == BGP_UNSET_PASSWD_MSG)
		{
			temp_entry = (struct peer_entry *)data;
			bgp_peer_entry_del(temp_entry->sip,temp_entry->dip);
		}
		else
			goto drop;
		return 0;
	}
drop:
	return -1;
}

int bgp_md5_digest_make(int flag, struct tcphdr*th, char *data,int data_len, char* digest, int dir)
{
	MD5_CTX context;
	uint16_t checksum = 0;
	uint16_t seg_len = 0;
	char tcp4_pseudohdr[12] = {0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x06,0x00,0x14};
 
	if(flag < 0)
		return -1;

	seg_len = data_len + th->hlen * 4;

	if(dir == 1)
	{
		tcp4_pseudohdr[0] = (bgp_peer_entry[flag].dip >> 24) & 0xff;
		tcp4_pseudohdr[1] = (bgp_peer_entry[flag].dip >> 16) & 0xff;
		tcp4_pseudohdr[2] = (bgp_peer_entry[flag].dip >> 8) & 0xff;
		tcp4_pseudohdr[3] = bgp_peer_entry[flag].dip & 0xff;

		tcp4_pseudohdr[4] = (bgp_peer_entry[flag].sip >> 24) & 0xff;
		tcp4_pseudohdr[5] = (bgp_peer_entry[flag].sip >> 16) & 0xff;
		tcp4_pseudohdr[6] = (bgp_peer_entry[flag].sip >> 8) & 0xff;
		tcp4_pseudohdr[7] = bgp_peer_entry[flag].sip & 0xff;
	}
	else
	{
		tcp4_pseudohdr[0] = (bgp_peer_entry[flag].sip >> 24) & 0xff;
		tcp4_pseudohdr[1] = (bgp_peer_entry[flag].sip >> 16) & 0xff;
		tcp4_pseudohdr[2] = (bgp_peer_entry[flag].sip >> 8) & 0xff;
		tcp4_pseudohdr[3] = bgp_peer_entry[flag].sip & 0xff;

		tcp4_pseudohdr[4] = (bgp_peer_entry[flag].dip >> 24) & 0xff;
		tcp4_pseudohdr[5] = (bgp_peer_entry[flag].dip >> 16) & 0xff;
		tcp4_pseudohdr[6] = (bgp_peer_entry[flag].dip >> 8) & 0xff;
		tcp4_pseudohdr[7] = bgp_peer_entry[flag].dip & 0xff;
	}
	tcp4_pseudohdr[10] = seg_len >> 8 & 0xff;
	tcp4_pseudohdr[11] = seg_len & 0xff;

	checksum = th->checksum;
	th->checksum = 0;
	
    MD5Init(&context);					
    MD5Update(&context, tcp4_pseudohdr, 12);
    MD5Update(&context, th, 20);
	if(data != NULL)	
	{	
    	MD5Update(&context, data, data_len);
	}
    MD5Update(&context, bgp_peer_entry[flag].passwd, strlen(bgp_peer_entry[flag].passwd));
    MD5Final((uint8_t *)digest, &context);	
	th->checksum = checksum;
	
	return 0;
}


