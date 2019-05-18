/*
 *  define IPC message queue operation
 */
#if 0

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <sys/msg.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <linux/sysctl.h>
#include "log.h"
#include "msg_ipc.h"
#include "devm_com.h"
#include "zassert.h"


int ipc_common_id = -1; /* common ipc queue for every one */
int ipc_reply_id = -1;  /* common ipc queue for reply */
int ipc_route_id = -1;  /* ipc queue for route??????·??Э?�??õ? IPC */
int ipc_ftm_id = -1;    /* ipc queue for send message to ftm */
int ipc_hal_id = -1;    /* ipc queue for send message to hal */
int ipc_syslog_id = -1; /* ipc queue for send message to syslog */
int ipc_ha_id = -1;     /* ipc queue for send message to ha */

int ipc_pktrx_vty_id = -1;  /* common ipc for vty receive packet */
int ipc_pkttx_vty_id = -1;  /* common ipc for vty send packet */

int ipc_pktrx_id = -1;  /* common ipc for app receive packet */
int ipc_pkttx_id = -1;  /* common ipc for app send packet */
int ipc_filem_id = -1;  /* filem ipc for devm or ha send packet */

int ipc_msdh_id = -1;   /* ipc for sdhmgt and snmpd, MSDH */

int ipc_halha_id = -1;   /* ipc for hal ha message send or recv */

int ipc_halack_id = -1;   /* ipc for hal btb ack message */

/* ????ϵͳ?????? IPC */
void ipc_init(void)
{
	  int id = -1;

	  /* max size of message */
	  system("sysctl -w kernel.msgmax=10000");

	  /* default max size of queue */
	  system("sysctl -w kernel.msgmnb=10000000");

	  /* ipc for receive control message from app */
	  id = ipc_create(IPC_MSG_COMMON, 4000, 100);
	  if(id < 0)
	  {
		  assert(0);
		  return;
	  }
	  else
	  {
		  ipc_common_id = id;
	  }

	  /* ipc for reply message */
	  id = ipc_create(IPC_MSG_REPLY, 4000, 100);
	  if(id < 0)
	  {
		  assert(0);
		  return;
	  }
	  else
	  {
		  ipc_reply_id = id;
	  }

	  /* ipc for receive route from app */
	  id = ipc_create(IPC_MSG_ROUTE, 4000, 100);
	  if(id < 0)
	  {
		  assert(0);
		  return;
	  }
	  else
	  {
		  ipc_route_id = id;
	  }
	  
	  /* ipc for receive ftm message from app */
	  id = ipc_create(IPC_MSG_FTM, 4000, 100);
	  if(id < 0)
	  {
		  assert(0);
		  return;
	  }
	  else
	  {
		  ipc_ftm_id = id;
	  }

	  /* ipc for receive hal message from app */
	  id = ipc_create(IPC_MSG_HAL, 4000, 100);
	  if(id < 0)
	  {
		  assert(0);
		  return;
	  }
	  else
	  {
		  ipc_hal_id = id;
	  }

	  /* ipc for receive ha message from app */
	  id = ipc_create(IPC_MSG_HA, 4000, 100);
	  if(id < 0)
	  {
		  assert(0);
		  return;
	  }
	  else
	  {
		  ipc_ha_id = id;
	  }

	  	  /* ipc for ftm receive packet from vty */
	  id = ipc_create ( IPC_PACKET_FTM_VTY, 1000, 10000 );
	  if ( id < 0 )
	  {
		 assert ( 0 );
		 return;
	  }
	  else
	  {
		 ipc_pktrx_vty_id = id;
	  }

	  /* ipc for ftm send packet to app */
	  id = ipc_create ( IPC_PACKET_SEND_VTY, 1000, 10000 );
	  if ( id < 0 )
	  {
		 assert ( 0 );
		 return;
	  }
	  else
	  {
		 ipc_pkttx_vty_id = id;
	  }
	  

	  /* ipc for ftm receive packet from app */
	  id = ipc_create ( IPC_PACKET_FTM, 1000, 10000 );
	  if ( id < 0 )
	  {
		 assert ( 0 );
		 return;
	  }
	  else
	  {
		 ipc_pktrx_id = id;
	  }

	  /* ipc for ftm send packet to app */
	  id = ipc_create ( IPC_PACKET_SEND, 1000, 10000 );
	  if ( id < 0 )
	  {
		 assert ( 0 );
		 return;
	  }
	  else
	  {
		 ipc_pkttx_id = id;
	  }

    /* ipc for send filem packet to app */
    id = ipc_create(IPC_MSG_FILEM, 100, 10000);
    if ( id < 0 )
    {
        assert ( 0 );
        return;
    }
    else
    {
        ipc_filem_id = id;
    }

    /* ipc for sdhmgt and snmpd, MSDH */
    id = ipc_create(IPC_MSG_MSDH, 1000, 1200);
    if ( id < 0 )
    {
       assert ( 0 );
       return;
    }
    else
    {
       ipc_msdh_id = id;
    }

    /* ipc for HALHA  */
    id = ipc_create(IPC_MSG_HALHA, 1000, 64);
    if ( id < 0 )
    {
       assert ( 0 );
       return;
    }
    else
    {
       ipc_halha_id = id;
    }

  /* ipc for HALHAACK  */
    id = ipc_create(IPC_MSG_HALACK, 500, 64);
    if ( id < 0 )
    {
       assert ( 0 );
       return;
    }
    else
    {
       ipc_halack_id = id;
    }    
}


/* ***********************************************
 * Function: ipc_serv_create
 * Input:
 *  	ipc_path: the name of message queue
        queue_len: length of message queue
 * Output:
 * Return:
 *		> 0: message queue id
 *      < 0: error
 ************************************************/
int ipc_create(int key, int queue_len, int msg_size)
{
	int id;
	struct msqid_ds qbuf;
	int ret = -1;

    /* create an ipc and get id */
	id = msgget(key, IPC_CREAT|ACCESS_PERMISSION);
	if (id < 0)
	{
		printf("Create ipc msg queue failed: %d\n", key);
        return -1;
	}

    /* ??ȡ???в??? */
	ret = msgctl(id, IPC_STAT, &qbuf);
	if (ret == -1)
	{
		printf("Get ipc msg queue parameter failed: %d\n", key);
		return -1;
	}

    /* ???ö??в??? */
	qbuf.msg_qnum = queue_len;  /* ???г??? */
	qbuf.msg_qbytes = queue_len * msg_size;  /* ???е??ֽ??? */
	ret = msgctl(id, IPC_SET, &qbuf);
	if (ret == -1)
	{
		printf("Set ipc msg queue parameter failed: %d\n", key);
		return -1;
	}

	printf("Create message queue %d successful\n", key);
	return id;
}


/* ***********************************************
 * Function: ipc_serv_close
 * Input:
 *   	id: the message queue id
 * Output:
 * Return:  > 0: ok
 *        < 0: error
 ************************************************/
int ipc_close(int id)
{
	int ret;

	if(id < 0)
	{
		printf("ipc_close error, ipc id = 0!\n");
		return -1;
	}

	ret = msgctl(id, IPC_RMID, NULL);
	if (ret)
	{
  		printf("IPC close failed.\n");
		return -1;
	}

 	return ret;
}


/* ***********************************************
 * Function: ipc_cli_connect
 * Input:
 *  key: the message queue id
 * Output:
 * Return: >0: the message queue id
 *       -1: error
 ************************************************/
int ipc_connect(int key)
{
	int id;

	id = msgget(key, 0);
	if (id < 0)
	{
		printf("Connect message queue failed, key:%d,id:%d\n", key, id);
		return -1;
	}

	return id;
}


/*************************************************
 *  ???? IPC ??Ϣ
 ************************************************/
int ipc_send(int id, struct ipc_msghdr *msghdr, void *data)
{
	static struct ipc_pkt mesg;
	int ret;

	if(id < 0)
	{
		return -1;
	}

    if(!msghdr)
	{
		return -1;
	}

	if(msghdr->data_len > IPC_MSG_LEN1)
	{
		return -1;
	}

    memcpy(&mesg.msghdr, msghdr, sizeof(struct ipc_msghdr));
	if(data)
    	memcpy(mesg.msg_data, data, msghdr->data_len);

	ret = msgsnd(id, (void *)(&mesg), msghdr->data_len+IPC_HEADER_LEN-4, IPC_NOWAIT);
	if(ret < 0)
	{
		zlog_debug("%s, %d %s\n",__FUNCTION__, __LINE__, strerror(errno));
	}

	return ret;
}

/*************************************************
 *  ???? IPC ??Ϣ
 ************************************************/
int ipc_send_block(int id, struct ipc_msghdr *msghdr, void *data)
{
	static struct ipc_pkt mesg;
	int ret;

	if(id < 0)
	{
		return -1;
	}

    if(!msghdr)
	{
		return -1;
	}

	if(msghdr->data_len > IPC_MSG_LEN1)
	{
		return -1;
	}

    memcpy(&mesg.msghdr, msghdr, sizeof(struct ipc_msghdr));
	if(data)
    	memcpy(mesg.msg_data, data, msghdr->data_len);

	ret = msgsnd(id, (void *)(&mesg), msghdr->data_len+IPC_HEADER_LEN-4, 0);
	if(ret < 0)
	{
		zlog_debug("%s, %d %s\n",__FUNCTION__, __LINE__, strerror(errno));
	}

	return ret;
}

/* ***********************************************
 * Return:
 *		>= 0: the clild pid, whom is the message received from
 *      -1: no message or error
 * description: ????С?? 2000 ?ֽڵı???
 ************************************************/
int ipc_recv(int id, struct ipc_mesg *pmsg, int module_id)
{
	if(id < 0)
	{
		printf("ipc_recv error, ipc id = 0!\n");
		return -1;
	}

	return msgrcv(id, (void *)pmsg, IPC_MSG_LEN+IPC_HEADER_LEN-4, module_id, IPC_NOWAIT);
}

/* ***********************************************
 * Return:
 *		>= 0: the clild pid, whom is the message received from
 *      -1: no message or error
 * description: ????С?? 2000 ?ֽڵı???
 ************************************************/
int ipc_recv_block(int id, struct ipc_mesg *pmsg, int module_id)
{
	if(id < 0)
	{
		printf("ipc_recv error, ipc id = 0!\n");
		return -1;
	}

	return msgrcv(id, (void *)pmsg, IPC_MSG_LEN+IPC_HEADER_LEN-4, module_id, 0);
}

/* ***********************************************
 * ???? 10000 ?ֽڵĴ???
 ************************************************/
int ipc_recv_pkt(int id, struct ipc_pkt *pmsg, int module_id)
{
	if(id < 0)
	{
		printf("ipc_recv error, ipc id = 0!\n");
		return -1;
	}

	return msgrcv(id, (void *)pmsg, IPC_MSG_LEN1+IPC_HEADER_LEN-4, module_id, IPC_NOWAIT);
}


/* ��?ӹ??? IPC */
int ipc_connect_common(void)
{
    int id;

    /* ipc for send control message */
	id = ipc_connect(IPC_MSG_COMMON);
	if(id < 0)
	{
	    zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);

		assert(0);
		return -1;
	}
	else
	{
	   ipc_common_id = id;
	   return 0;
	}
}


/* ?ӹ??? IPC ??????Ϣ*/
int ipc_recv_common(struct ipc_mesg *pmsg, int module_id)
{
	int ret = -1;

	if(ipc_common_id < 0)
		ipc_connect_common();

	ret = msgrcv(ipc_common_id, (void *)pmsg, IPC_MSG_LEN+IPC_HEADER_LEN-4, module_id, IPC_NOWAIT);
    if (ret == -1)
	{
    	return -1;
	}

	return ret;
}


/* ?ӹ??? IPC ??????Ϣ*/
int ipc_send_common(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_common_id < 0)
		ipc_connect_common();

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
		
	}

	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = 0;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

#if 0
    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
	    ret = ipc_send(ipc_common_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_common_id, &msghdr, pdata);
	
	return ret;
}


/* ?ӹ??? IPC ??????Ϣ??֧??????Ϣͷ?з??? index */
int ipc_send_common1(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_common_id < 0)
		ipc_connect_common();

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
		
	}

	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;
	
#if 0
    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
	    ret = ipc_send(ipc_common_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_common_id, &msghdr, pdata);

	return ret;
}


/* send msg and return bulk reply msg */
struct ipc_mesg * ipc_send_common_wait_reply1(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	static struct ipc_mesg msg;
	int count = IPC_RSEND_COUNT;
	int ret = -1;

	if(ipc_common_id < 0)
    {
      	ipc_connect_common();

        if (ipc_common_id < 0)
        {
            zlog_err("%s[%d]: ipc connect common failed!\n", __FILE__, __LINE__);
            return NULL;
        }
    }

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);

        if(ipc_reply_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return NULL;
        }
	}

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return NULL;
	}

	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

	/*clear old reply*/
	while(ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
	{
	}

#if 0
    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
	    ret = ipc_send(ipc_common_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_common_id, &msghdr, pdata);

	/* ??Ӧ?????? IPC_RRECIVE_COUNT ?? */
    //count = IPC_RRECIVE_COUNT;
    count = IPC_RRECIVE_COUNT_BLOCK;

wait:
	//usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT - count));//?ó? CPU 10ms
	usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT_BLOCK - count));//?ó? CPU 10ms
	if(count)
	{
		count --;
		ret = ipc_recv(ipc_reply_id, (void *)&msg, sender_id);
		if(ret == -1)
			goto wait;

		if((msg.msghdr.msg_type != msg_type)
			||(msg.msghdr.msg_subtype != subtype)
			||(msg.msghdr.msg_index != msg_index)
			||(msg.msghdr.sender_id != module_id))
			goto wait;
		if(msg.msghdr.opcode == IPC_OPCODE_NACK)
			return NULL;
		else if(msg.msghdr.opcode == IPC_OPCODE_REPLY)
			return &msg;
		else
			return NULL;
	}
	else
	{
		zlog_err ( "%s[%d] ipc reply timeout !\n", __FUNCTION__, __LINE__);
		return NULL;
	}
}


/* send msg and return reply data */
void * ipc_send_common_wait_reply(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	struct ipc_mesg * pmsg = ipc_send_common_wait_reply1(pdata, data_len, data_num, module_id, sender_id,
					 msg_type, subtype, opcode, msg_index);

    if(pmsg)
    {
		return pmsg->msg_data;
    }
	else
	{
		return NULL;
	}
}


/* send message and returen errcode */
int ipc_send_common_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	static struct ipc_mesg msg;
	int	errcode = 0;
	int count = IPC_RSEND_COUNT;
	int ret = -1;

	if(ipc_common_id < 0)
    {
     	ipc_connect_common();

        if(ipc_common_id < 0)
        {
            zlog_err("%s[%d]: ipc connect common failed!\n", __FILE__, __LINE__);
            return -1;
        }
     }

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);
        if(ipc_reply_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
	}

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}
	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

	/*clear old reply*/
	while(ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
	{
	}

#if 0
    /* ????ʧ???ش? 100 ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_common_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_common_id, &msghdr, pdata);

	/* ??Ӧ?????? IPC_RRECIVE_COUNT ?? */
	//count = IPC_RRECIVE_COUNT;
	count = IPC_RRECIVE_COUNT_BLOCK;

wait:
	//usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT - count));//?ó? CPU 10ms
	usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT_BLOCK - count));//?ó? CPU 10ms
	if(count)
	{
		count --;
		ret = ipc_recv(ipc_reply_id, (void *)&msg, sender_id);
		if(ret == -1)
			goto wait;

		if((msg.msghdr.msg_type != msg_type)
			||(msg.msghdr.msg_subtype != subtype)
			||(msg.msghdr.msg_index != msg_index)
			||(msg.msghdr.sender_id != module_id))
			goto wait;
		if(msg.msghdr.opcode == IPC_OPCODE_NACK)
		{
			memcpy(&errcode, msg.msg_data, sizeof(errcode));
			//errcode = *(uint32_t *)(msg.msg_data);
			return errcode;
		}
		else if(msg.msghdr.opcode == IPC_OPCODE_ACK)
			return 0;
		else
			return -1;
	}
	else
	{
		zlog_err ( "%s[%d] ipc ack time out \n", __FUNCTION__, __LINE__);
		return -1;
	}
}

int ipc_recv_msdh(struct ipc_mesg *pmsg, int module_id)
{
    int ret = -1;

    if(ipc_msdh_id < 0)
    {
        ipc_msdh_id = ipc_connect(IPC_MSG_MSDH);

        if(ipc_msdh_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    ret = msgrcv(ipc_msdh_id, (void *)pmsg, IPC_MSG_LEN+IPC_HEADER_LEN-4, module_id, IPC_NOWAIT);
    if (ret == -1)
    {
        return -1;
    }

    return ret;
}

int ipc_send_msdh(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode)
{
    static struct ipc_msghdr msghdr;
    int ret = -1;
    int count = IPC_RSEND_COUNT;

    if(ipc_msdh_id < 0)
    {
        ipc_msdh_id = ipc_connect(IPC_MSG_MSDH);

        if(ipc_msdh_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if(IPC_MSG_LEN < data_len)
    {
        zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
        return -1;
    }

    msghdr.data_len = data_len;
    msghdr.module_id = module_id;
    msghdr.msg_type = msg_type;
    msghdr.msg_subtype = subtype;
    msghdr.msg_index = 0;
    msghdr.data_num = data_num;
    msghdr.opcode = opcode;
    msghdr.sender_id = sender_id;

    while((ret < 0) && (count > 0))
    {
        count--;
        ret = ipc_send(ipc_msdh_id, &msghdr, pdata);

        if(ret < 0)
        {
            usleep(IPC_RSEND_INTERVAL(count));
        }
    }

    return ret;
}


/* ????·????Ϣ */
int ipc_recv_route(struct ipc_mesg *pmsg, int module_id)
{
	int ret = -1;

	if(ipc_route_id < 0)
    {
		ipc_route_id = ipc_connect(IPC_MSG_ROUTE);
        if(ipc_route_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	ret = msgrcv(ipc_route_id, (void *)pmsg, IPC_MSG_LEN+IPC_HEADER_LEN-4, module_id, IPC_NOWAIT);
    if (ret == -1)
	{
    	return -1;
	}

	return ret;
}


/* ????·????Ϣ */
int ipc_send_route(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_route_id < 0)
    {
		ipc_route_id = ipc_connect(IPC_MSG_ROUTE);
        if (ipc_route_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}

	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = 0;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

    /* ????ʧ???ش? 100 ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_route_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }

	return ret;
}

/* 发�?? ha 消息 */
int ipc_send_ha(void *pdata, int data_len, int data_num, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_ha_id < 0)
    {
		ipc_ha_id = ipc_connect(IPC_MSG_HA);
        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	
    memset(&msghdr, 0, sizeof(struct ipc_msghdr));
	msghdr.data_len = data_len;
	msghdr.module_id = MODULE_ID_HA;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

    /* 发�?�失败重�? 100 �? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_ha_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//发�?�失败让�? CPU 10ms
		}
    }

	return ret;
}

int ipc_send_ha_1(struct ipc_pkt *pmsg, int dlen) 
{
	int ret = -1;
	int count = IPC_RSEND_COUNT;
	
	if(ipc_ha_id < 0)
    {
		ipc_ha_id = ipc_connect(IPC_MSG_HA);
        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }
	
    /* 发�?�失败重�? 100 �? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = msgsnd(ipc_ha_id, (void *)pmsg, dlen - 4, IPC_NOWAIT);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//发�?�失败让�? CPU 10ms
		}
    }

	return ret;
}


int ipc_send_partnerslot_hal_byha(void *pdata, int data_len, int data_num, int sender_id,
                                  enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret = 0;

    ret = ipc_send_partnerslot_byha(pdata, data_len, data_num, sender_id, msg_type, subtype, opcode, msg_index , IPC_MSG_HAL);

    return ret;
}


int ipc_send_partnerslot_common_byha(void *pdata, int data_len, int data_num, int sender_id,
                                     enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
    int ret = 0;

    ret = ipc_send_partnerslot_byha(pdata, data_len, data_num, sender_id, msg_type, subtype, opcode, msg_index , IPC_MSG_COMMON);

    return ret;
}


int ipc_send_partnerslot_byha(void *pdata, int data_len, int data_num, int sender_id,
                              enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char queuqe_id)
{
    int ret = 1;
    static unsigned char partner_slot = 0;

    ret = devm_comm_get_partner_slot(1, sender_id, &partner_slot);

    if (0 == ret && 0 != partner_slot)
    {
        ret = ipc_send_slot_byha(pdata, data_len, data_num, sender_id, msg_type, subtype, opcode, msg_index, partner_slot, queuqe_id);
    }
    else
    {
        zlog_err("%s[%d]: ipc send failed, ret = %d, paraslot = %d!\n", __FILE__, __LINE__, ret, partner_slot);
    }

    return ret;
}


/* ???? ha ??Ϣ */
int ipc_send_slot_byha(void *pdata, int data_len, int data_num, int sender_id,
                       enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id)
{
    static struct ha_ipc_mesg ha_msg_send;
    struct ipc_msghdr *msghdr = NULL;
    int ret = -1;
    int count = IPC_RSEND_COUNT;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (data_len > IPC_MSG_LEN1)
    {
        return -1;
    }

    if (0 == target_slot)
    {
        return -1;
    }

    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = queuqe_id;

    msghdr = &ha_msg_send.ipc_mesg_data.msghdr;
    msghdr->data_len = data_len;
    msghdr->module_id = MODULE_ID_HA;
    msghdr->msg_type = msg_type;
    msghdr->msg_subtype = subtype;
    msghdr->msg_index = msg_index;
    msghdr->data_num = data_num;
    msghdr->opcode = opcode;
    msghdr->sender_id = sender_id;

    if (pdata)
    {
        memcpy(ha_msg_send.ipc_mesg_data.msg_data, pdata, data_len);
    }

    /* ????ʧ???ش? 100 ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;

        ret = msgsnd(ipc_ha_id, (void *)(&ha_msg_send), sizeof(unsigned char) * 4 + msghdr->data_len + IPC_HEADER_LEN - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            zlog_debug("%s, %d %s\n", __FUNCTION__, __LINE__, strerror(errno));
        }

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }

	return ret;
}


/* send msg and return reply msg */
struct ipc_pkt *ipc_send_slot_byha_wait_reply1(void *pdata, int data_len, int data_num, int sender_id,
        enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id)
{
    static struct ha_ipc_mesg ha_msg_send;
    struct ipc_msghdr *msghdr = NULL;
    static struct ipc_pkt msg;
    int count = IPC_RSEND_COUNT;
    int ret = -1;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return NULL;
        }
    }

    if (ipc_reply_id < 0)
    {
        ipc_reply_id = ipc_connect(IPC_MSG_REPLY);

        if (ipc_reply_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return NULL;
        }
    }

    if (data_len > IPC_MSG_LEN1)
    {
        return NULL;
    }

    if (0 == target_slot)
    {
        return NULL;
    }

    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = queuqe_id;

    msghdr = &ha_msg_send.ipc_mesg_data.msghdr;
    msghdr->data_len = data_len;
    msghdr->module_id = MODULE_ID_HA;
    msghdr->msg_type = msg_type;
    msghdr->msg_subtype = subtype;
    msghdr->msg_index = msg_index;
    msghdr->data_num = data_num;
    msghdr->opcode = opcode;
    msghdr->sender_id = sender_id;

    if (pdata)
    {
        memcpy(ha_msg_send.ipc_mesg_data.msg_data, pdata, data_len);
    }

    /*clear old reply*/
    while (ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
    {
    }

    /* ????ʧ???ش? 100 ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;

        ret = msgsnd(ipc_ha_id, (void *)(&ha_msg_send), sizeof(unsigned char) * 4 + msghdr->data_len + IPC_HEADER_LEN - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            zlog_debug("%s, %d %s\n", __FUNCTION__, __LINE__, strerror(errno));
        }

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }

    /* ??Ӧ?????? IPC_RRECIVE_COUNT ?? */
    count = IPC_RRECIVE_COUNT;

wait:
    usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT - count));//10ms

    if (count)
    {
        count --;
        ret = ipc_recv(ipc_reply_id, (void *)&msg, sender_id);

        if (ret == -1)
        {
            goto wait;
        }

        if ((msg.msghdr.msg_type != msg_type)
                || (msg.msghdr.msg_subtype != subtype)
                || (msg.msghdr.msg_index != msg_index)
                || (msg.msghdr.sender_id != MODULE_ID_HA))
        {
            goto wait;
        }

        if (msg.msghdr.opcode == IPC_OPCODE_NACK)
        {
            return NULL;
        }
        else if (msg.msghdr.opcode == IPC_OPCODE_REPLY)
        {
            return &msg;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        zlog_err("%s[%d] ipc reply time out msg_index 0x%0x\n", __FUNCTION__, __LINE__, msg_index);
        return NULL;
    }
}


/* send msg and return reply data */
void *ipc_send_slot_byha_wait_reply(void *pdata, int data_len, int data_num, int sender_id,
                                    enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id)
{
    struct ipc_pkt *pmsg = ipc_send_slot_byha_wait_reply1(pdata, data_len, data_num, sender_id,
                           msg_type, subtype, opcode, msg_index, target_slot, queuqe_id);

    if (pmsg)
    {
        return pmsg->msg_data;
    }
    else
    {
        return NULL;
    }
}


/* send message and returen errcode */
int ipc_send_slot_byha_wait_ack(void *pdata, int data_len, int data_num, int sender_id,
                                enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index, unsigned char target_slot, unsigned char queuqe_id)
{
    static struct ha_ipc_mesg ha_msg_send;
    struct ipc_msghdr *msghdr = NULL;
    static struct ipc_pkt msg;
    int errcode = 0;
    int count = IPC_RSEND_COUNT;
    int ret = -1;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (ipc_reply_id < 0)
    {
        ipc_reply_id = ipc_connect(IPC_MSG_REPLY);

        if (ipc_reply_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (data_len > IPC_MSG_LEN1)
    {
        return -1;
    }

    if (0 == target_slot)
    {
        return -1;
    }


    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = queuqe_id;

    msghdr = &ha_msg_send.ipc_mesg_data.msghdr;
    msghdr->data_len = data_len;
    msghdr->module_id = MODULE_ID_HA;
    msghdr->msg_type = msg_type;
    msghdr->msg_subtype = subtype;
    msghdr->msg_index = msg_index;
    msghdr->data_num = data_num;
    msghdr->opcode = opcode;
    msghdr->sender_id = sender_id;

    /*clear old reply*/
    while (ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
    {
    }

    /* ????ʧ???ش? 100 ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;

        ret = msgsnd(ipc_ha_id, (void *)(&ha_msg_send), sizeof(unsigned char) * 4 + msghdr->data_len + IPC_HEADER_LEN - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            zlog_debug("%s, %d %s\n", __FUNCTION__, __LINE__, strerror(errno));
        }

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }


    /* ??Ӧ?????? IPC_RRECIVE_COUNT ?? */
    count = IPC_RRECIVE_COUNT;

wait:
    usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT - count));//10ms

    if (count)
    {
        count --;
        ret = ipc_recv(ipc_reply_id, (void *)&msg, sender_id);

        if (ret == -1)
        {
            goto wait;
        }

        if ((msg.msghdr.msg_type != msg_type)
                || (msg.msghdr.msg_subtype != subtype)
                || (msg.msghdr.msg_index != msg_index)
                || (msg.msghdr.sender_id != MODULE_ID_HA))
        {
            goto wait;
        }

        if (msg.msghdr.opcode == IPC_OPCODE_NACK)
        {
            memcpy(&errcode, msg.msg_data, sizeof(errcode));
            //errcode = *(uint32_t *)msg.msg_data;
            return errcode;
        }
        else if (msg.msghdr.opcode == IPC_OPCODE_ACK)
        {
            return 0;
        }
        else
        {
            return -1;
        }
    }
    else
    {
        zlog_err("%s[%d] ipc ack time out msg_index 0x%0x\n", __FUNCTION__, __LINE__, msg_index);
        return -1;
    }
}


int ipc_send_slot_byha_1(struct ipc_pkt *pmsg, int dlen, unsigned char target_slot, unsigned char queuqe_id)
{
    int ret = -1;
    int count = IPC_RSEND_COUNT;
    static struct ha_ipc_mesg ha_msg_send;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (0 == target_slot)
    {
        return -1;
    }

    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    memcpy(&ha_msg_send.ipc_mesg_data, pmsg, dlen);
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = queuqe_id;

    /* ????ʧ???ش? 100 ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;
        ret = msgsnd(ipc_ha_id, (void *)&ha_msg_send, sizeof(unsigned char) * 4 + dlen - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }

    return ret;
}


int ipc_send_partnerslot_byha_1(struct ipc_pkt *pmsg, int dlen, unsigned char queuqe_id)
{
    int ret = 1;
    static unsigned char partner_slot = 0;

    ret = devm_comm_get_partner_slot(1, pmsg->msghdr.sender_id, &partner_slot);

    if (0 == ret && 0 != partner_slot)
    {
        ret = ipc_send_slot_byha_1(pmsg, dlen, partner_slot, queuqe_id);
    }
    else
    {
        zlog_err("%s[%d]: ipc send failed, ret = %d, paraslot = %d!\n", __FILE__, __LINE__, ret, partner_slot);
    }

    return ret;
}


/* ???Ͷ???Ӧ????Ϣ */
int ipc_send_reply_bulk_toslot_byha(void *pdata, int data_len, int data_num, int sender_id,
                                    enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot)
{
    static struct ha_ipc_mesg ha_msg_send;
    struct ipc_msghdr *msghdr = NULL;
    int ret = -1;
    int count = IPC_RSEND_COUNT;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (data_len > IPC_MSG_LEN1)
    {
        return -1;
    }

    if (0 == target_slot)
    {
        return -1;
    }

    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = IPC_MSG_REPLY;

    msghdr = &ha_msg_send.ipc_mesg_data.msghdr;
    msghdr->data_len = data_len;
    msghdr->module_id = MODULE_ID_HA;
    msghdr->msg_type = msg_type;
    msghdr->msg_subtype = subtype;
    msghdr->msg_index = msg_index;
    msghdr->data_num = data_num;
    msghdr->opcode = IPC_OPCODE_REPLY;
    msghdr->sender_id = sender_id;

    if (pdata)
    {
        memcpy(ha_msg_send.ipc_mesg_data.msg_data, pdata, data_len);
    }

    /* ????ʧ???ش? 100 ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;

        ret = msgsnd(ipc_ha_id, (void *)(&ha_msg_send), sizeof(unsigned char) * 4 + msghdr->data_len + IPC_HEADER_LEN - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            zlog_debug("%s, %d %s\n", __FUNCTION__, __LINE__, strerror(errno));
        }

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }

    return ret;
}


/* ????һ??Ӧ????Ϣ */
int ipc_send_reply_toslot_byha(void *pdata, int data_len, int sender_id,
                               enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot)
{
    return ipc_send_reply_bulk_toslot_byha(pdata, data_len, 1, sender_id,
                                           msg_type, subtype, msg_index, target_slot);
}


/* ????ȷ?ϳɹ?????Ϣ */
int ipc_send_ack_toslot_byha(int sender_id, enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot)
{
    static struct ha_ipc_mesg ha_msg_send;
    struct ipc_msghdr *msghdr = NULL;
    int ret = -1;
    int count = IPC_RSEND_COUNT;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (0 == target_slot)
    {
        return -1;
    }

    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = IPC_MSG_REPLY;

    msghdr = &ha_msg_send.ipc_mesg_data.msghdr;
    msghdr->data_len = 0;
    msghdr->module_id = MODULE_ID_HA;
    msghdr->msg_type = msg_type;
    msghdr->msg_subtype = subtype;
    msghdr->msg_index = msg_index;
    msghdr->data_num = 1;
    msghdr->opcode = IPC_OPCODE_ACK;
    msghdr->sender_id = sender_id;

    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;

        ret = msgsnd(ipc_ha_id, (void *)(&ha_msg_send), sizeof(unsigned char) * 4 + msghdr->data_len + IPC_HEADER_LEN - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            zlog_debug("%s, %d %s\n", __FUNCTION__, __LINE__, strerror(errno));
        }

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }

    return ret;
}


/* ????ȷ??ʧ?ܵ???Ϣ????Ҫ???ʹ????? */
int ipc_send_noack_toslot_byha(uint32_t errcode, int sender_id,
                               enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index, unsigned char target_slot)
{
    static struct ha_ipc_mesg ha_msg_send;
    struct ipc_msghdr *msghdr = NULL;
    int ret = -1;
    int count = IPC_RSEND_COUNT;

    if (ipc_ha_id < 0)
    {
        ipc_ha_id = ipc_connect(IPC_MSG_HA);

        if (ipc_ha_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

    if (0 == target_slot)
    {
        return -1;
    }

    memset(&ha_msg_send, 0, sizeof(struct ha_ipc_mesg));
    ha_msg_send.target_slot = target_slot;
    ha_msg_send.target_queue_id = IPC_MSG_REPLY;

    msghdr = &ha_msg_send.ipc_mesg_data.msghdr;
    msghdr->data_len = 4;
    msghdr->module_id = MODULE_ID_HA;
    msghdr->msg_type = msg_type;
    msghdr->msg_subtype = subtype;
    msghdr->msg_index = msg_index;
    msghdr->data_num = 1;
    msghdr->opcode = IPC_OPCODE_NACK;
    msghdr->sender_id = sender_id;

    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while ((ret < 0) && (count > 0))
    {
        count --;

        ret = msgsnd(ipc_ha_id, (void *)(&ha_msg_send), sizeof(unsigned char) * 4 + msghdr->data_len + IPC_HEADER_LEN - 4, IPC_NOWAIT);

        if (ret < 0)
        {
            zlog_debug("%s, %d %s\n", __FUNCTION__, __LINE__, strerror(errno));
        }

        if (ret < 0)
        {
            sleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
        }
    }

	return ret;
}


/* ???? syslog ??Ϣ */
int ipc_send_syslog(void *pdata, int data_len, int sender_id, uint8_t subtype, enum IPC_OPCODE opcode)
{
	static struct ipc_mesg mesg;
	struct ipc_msghdr *pmsghdr;
	int ret = -1;

	if((pdata == NULL)|| (data_len > IPC_MSG_LEN))
		return 0;

	if(ipc_syslog_id < 0)
    {
		ipc_syslog_id = msgget(IPC_MSG_SYSLOG, 0);
        if (ipc_syslog_id < 0)
        {
            printf("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}

	pmsghdr = &(mesg.msghdr);
	pmsghdr->data_len = data_len;
	pmsghdr->module_id = MODULE_ID_SYSLOG;
	pmsghdr->msg_type = IPC_TYPE_SYSLOG;
	pmsghdr->msg_subtype = subtype;
	pmsghdr->msg_index = 0;
	pmsghdr->data_num = 1;
	pmsghdr->opcode = opcode;
	pmsghdr->sender_id = sender_id;

   	memcpy(mesg.msg_data, pdata, data_len);

	ret = msgsnd(ipc_syslog_id, (void *)(&mesg), data_len+IPC_HEADER_LEN-4, IPC_NOWAIT);

	return ret;
}


/* ???? ftm ??Ϣ */
int ipc_send_ftm(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_ftm_id < 0)
    {
		ipc_ftm_id = ipc_connect(IPC_MSG_FTM);
        if (ipc_ftm_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}

	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

#if 0
    /* ????ʧ???ش? 100 ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_ftm_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_ftm_id, &msghdr, pdata);

	return ret;
}


/* ???? hal ??Ϣ */
int ipc_send_hal(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_hal_id < 0)
    {
		ipc_hal_id = ipc_connect(IPC_MSG_HAL);
        if (ipc_hal_id < 0)
        {
            zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
            return -1;
        }
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}

	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

#if 0
    /* ????ʧ???ش? 100 ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_hal_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_hal_id, &msghdr, pdata);

	return ret;
}


/* send msg and return reply msg */
struct ipc_mesg * ipc_send_hal_wait_reply1(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	static struct ipc_mesg msg;
	int count = IPC_RSEND_COUNT;
	int ret = -1;

	if(ipc_hal_id < 0)
		ipc_hal_id = ipc_connect(IPC_MSG_HAL);

    if(ipc_hal_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return NULL;
    }

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);
	}

    if(ipc_reply_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return NULL;
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return NULL;
	}
	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

	/*clear old reply*/
	while(ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
	{
	}

#if 0
    /* ????ʧ???ش? 100 ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_hal_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_hal_id, &msghdr, pdata);

	/* ??Ӧ?????? IPC_RRECIVE_COUNT ?? */
	//count = IPC_RRECIVE_COUNT;
	count = IPC_RRECIVE_COUNT_BLOCK;

wait:
	//usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT - count));//10ms
	usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT_BLOCK - count));//10ms
	if(count)
	{
		count --;
		ret = ipc_recv(ipc_reply_id, (void *)&msg, sender_id);
		if(ret == -1)
			goto wait;

		if((msg.msghdr.msg_type != msg_type)
			||(msg.msghdr.msg_subtype != subtype)
			||(msg.msghdr.msg_index != msg_index)
			||(msg.msghdr.sender_id != module_id))
			goto wait;
		if(msg.msghdr.opcode == IPC_OPCODE_NACK)
			return NULL;
		else if(msg.msghdr.opcode == IPC_OPCODE_REPLY)
			return &msg;
		else
			return NULL;
	}
	else
	{
		zlog_err ( "%s[%d] ipc reply time out msg_index 0x%0x\n", __FUNCTION__, __LINE__, msg_index);
		return NULL;
	}
}


/* send msg and return reply data */
void * ipc_send_hal_wait_reply(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	struct ipc_mesg * pmsg = ipc_send_hal_wait_reply1(pdata, data_len, data_num, module_id, sender_id,
					 msg_type, subtype, opcode, msg_index);

	if(pmsg)
	{
		return pmsg->msg_data;
	}
	else
	{
		return NULL;
	}
}


/* send message and returen errcode */
int ipc_send_hal_wait_ack(void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, enum IPC_OPCODE opcode, uint32_t msg_index)
{
	static struct ipc_msghdr msghdr;
	static struct ipc_mesg msg;
	int errcode = 0;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_hal_id < 0)
		ipc_hal_id = ipc_connect(IPC_MSG_HAL);

    if(ipc_hal_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return -1;
    }

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);
	}

    if(ipc_reply_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return -1;
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}
	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = opcode;
	msghdr.sender_id = sender_id;

	/*clear old reply*/
	while(ipc_recv(ipc_reply_id, (void *)&msg, sender_id) != -1)
	{
	}

#if 0
    /* ????ʧ???ش? 100 ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_hal_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_hal_id, &msghdr, pdata);

	/* ??Ӧ?????? IPC_RRECIVE_COUNT ?? */
	//count = IPC_RRECIVE_COUNT;
	count = IPC_RRECIVE_COUNT_BLOCK;

wait:
	//usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT - count));//10ms
	usleep(IPC_RRECIVE_INTERVAL(IPC_RRECIVE_COUNT_BLOCK - count));//10ms
	if(count)
	{
		count --;
		ret = ipc_recv(ipc_reply_id, (void *)&msg, sender_id);
		if(ret == -1)
			goto wait;

		if((msg.msghdr.msg_type != msg_type)
			||(msg.msghdr.msg_subtype != subtype)
			||(msg.msghdr.msg_index != msg_index)
			||(msg.msghdr.sender_id != module_id))
			goto wait;

		if(msg.msghdr.opcode == IPC_OPCODE_NACK)
		{
			memcpy(&errcode, msg.msg_data, sizeof(errcode));
			//errcode = *(uint32_t *)msg.msg_data;
			return errcode;
		}
		else if(msg.msghdr.opcode == IPC_OPCODE_ACK)
			return 0;
		else
			return -1;
	}
	else
	{
		zlog_err ( "%s[%d] ipc ack time out msg_index 0x%0x\n", __FUNCTION__, __LINE__, msg_index);
		return -1;
	}
}


/* ???Ͷ???Ӧ????Ϣ */
int ipc_send_reply_bulk (void *pdata, int data_len, int data_num, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index)
{
    static struct ipc_msghdr msghdr;	
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);
	}

    if(ipc_reply_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return -1;
    }

	if(IPC_MSG_LEN < data_len)
	{
		zlog_err("%s[%d]: msg data_len error!\n", __FILE__, __LINE__);
		return -1;
	}
	msghdr.data_len = data_len;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = data_num;
	msghdr.opcode = IPC_OPCODE_REPLY;
	msghdr.sender_id = sender_id;

#if 0
    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_reply_id, &msghdr, pdata);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }
#endif
	ret = ipc_send_block(ipc_reply_id, &msghdr, pdata);

	return ret;
}


/* ????һ??Ӧ????Ϣ */
int ipc_send_reply (void *pdata, int data_len, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index)
{
	return ipc_send_reply_bulk(pdata, data_len, 1, module_id, sender_id,
					 msg_type, subtype, msg_index);
}


/* ????ȷ?ϳɹ?????Ϣ */
int ipc_send_ack (int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index)
{
    static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);
	}

    if (ipc_reply_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return -1;
    }

	msghdr.data_len = 0;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = 1;
	msghdr.opcode = IPC_OPCODE_ACK;
	msghdr.sender_id = sender_id;

    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_reply_id, &msghdr, NULL);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }

	return ret;
}


/* ????ȷ??ʧ?ܵ???Ϣ????Ҫ???ʹ????? */
int ipc_send_noack (uint32_t errcode, int module_id, int sender_id,
					 enum IPC_TYPE msg_type, uint8_t subtype, uint32_t msg_index)
{
    static struct ipc_msghdr msghdr;
	int ret = -1;
	int count = IPC_RSEND_COUNT;

	if(ipc_reply_id < 0)
	{
		ipc_reply_id = ipc_connect(IPC_MSG_REPLY);
	}

    if (ipc_reply_id < 0)
    {
        zlog_err("%s[%d]: ipc connect failed!\n", __FILE__, __LINE__);
        return -1;
    }

	msghdr.data_len = 4;
	msghdr.module_id = module_id;
	msghdr.msg_type = msg_type;
	msghdr.msg_subtype = subtype;
	msghdr.msg_index = msg_index;
	msghdr.data_num = 1;
	msghdr.opcode = IPC_OPCODE_NACK;
	msghdr.sender_id = sender_id;

    /* ????ʧ???ش? IPC_RSEND_COUNT ?? */
    while((ret < 0) && (count > 0))
    {
        count --;
		ret = ipc_send(ipc_reply_id, &msghdr, &errcode);
		if(ret < 0)
		{
			usleep(IPC_RSEND_INTERVAL(count));//????ʧ???ó? CPU 10ms
		}
    }

	return ret;
}

/*send for raw message data*/
                     
int ipc_rawsend(int id, void *data, int datlen, int wait)
{
    int ret = -1;
    
	if((id >= 0) && (datlen > 4) && data)
    {   
    	ret = msgsnd(id, data, (datlen - 4), (wait ? 0 :IPC_NOWAIT));

    	if(ret < 0)
    	{
    		zlog_debug("%s, %d send error %s\n",__FUNCTION__, __LINE__, strerror(errno));
    	}
    }

	return(ret);
}

/*receive for raw message data*/

int ipc_rawrecv(int id, void *data, int buflen, int module_id, int wait)
{
    int iRevlen = -1;
    
	if((id >= 0) && (buflen > 4) && data)
    {
        iRevlen = msgrcv(id, data, (buflen - 4), module_id, (wait ? 0 : IPC_NOWAIT));
        
        if(iRevlen < 0)
        {
            zlog_debug("%s, %d send error %s\n",__FUNCTION__, __LINE__, strerror(errno));
        }
    }
    return((iRevlen > 0 ) ? (iRevlen + 4) : iRevlen);
}

#endif
