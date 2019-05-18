/**
 * @file      : port_isolate.c
 * @brief     : use for EP-Tree or EVP-Tree
 * @details   : 
 * @author    : geqian
 * @date      : 2018/10/18  15:38:50
 * @version   : 
 * @copyright : Copyright (C), 2018-2023, HuaHuan Co., Ltd.
 * @note      : according to OTN test of China Telecom
 */


#ifndef HIOS_PORT_ISOLATE_H
#define HIOS_PORT_ISOLATE_H

#include <lib/types.h>
#include <lib/hash1.h>

#define ISLT_GRP_NUM	1024
#define ISLT_PORT_NUM	32

#define ISLT_GRP_ROOT  	1

#define ISLT_GROUP_MIN 1
#define ISLT_GROUP_MAX 65535

#define   ISLT_MAX_PORT_NUM		32

enum ISLT_TYPE
{
	EP_TREE, 
	EVP_TREE    	
};

enum ISLT_ROLE
{
	LEAF, 
	ROOT    	
};


enum ISLT_STATE
{
	ACTIVE, 
	INACTIVE    	
};

enum ISLT_ACT
{
	ISLT_ADD,
	ISLT_DELETE
};

struct isolate_group
{
	uint32_t  	id;       //hash key:groupid or vlan id

	uint32_t	root;     //root port

	struct list *leaf;    //leaf ports
};

struct islt_key
{
	enum ISLT_TYPE  type;

	uint32_t  		ifindex;
	uint32_t  		id;
};

struct isolate_port
{
	struct islt_key key;

	enum ISLT_ROLE   role;
	enum ISLT_STATE  state;
	
	uint32_t		 root_port;
};

struct port_islt_msg
{
	uint8_t        opt;
	uint8_t        type;
	uint16_t 	   vlanid;
	uint32_t       rootid;
	uint32_t	   leafid[ISLT_MAX_PORT_NUM];
};

struct port_islt
{
	uint32_t 		id;
	enum ISLT_TYPE  type;
	enum ISLT_ROLE  role;
};

struct port_islt_snmp
{
	enum ISLT_TYPE type;
	uint32_t ifindex;
	uint32_t id;

	enum ISLT_ROLE role;
	enum ISLT_STATE state;
};

void port_isolate_init(void);
void l2if_reply_isolate_info_bulk(struct ipc_msghdr_n *phdr, void *pdata);

#endif

