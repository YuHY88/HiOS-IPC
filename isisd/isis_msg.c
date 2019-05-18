#include <lib/thread.h>
#include <lib/module_id.h>
#include <lib/pkt_type.h>
#include <ftm/pkt_eth.h>

#include "isisd/isis_pdu.h"
#include "isisd/isis_msg.h"
#include "memory.h"


extern struct thread_master *master;


/* 注册要接收的协议报文类型 */
#if 0
void isis_pkt_register()
{
	union proto_reg proto;

	memset(&proto, 0, sizeof(union proto_reg));

	proto.ethreg.dmac[0] = 0x01;
	proto.ethreg.dmac[1] = 0x80;
	proto.ethreg.dmac[2] = 0xc2;
	proto.ethreg.dmac[3] = 0x00;
	proto.ethreg.dmac[4] = 0x00;
	proto.ethreg.dmac[5] = 0x14;
	proto.ethreg.dmac_valid = 1;
    
	pkt_register(MODULE_ID_ISIS, PROTO_TYPE_ETH, &proto);	

	proto.ethreg.dmac[0] = 0x01;
	proto.ethreg.dmac[1] = 0x80;
	proto.ethreg.dmac[2] = 0xc2;
	proto.ethreg.dmac[3] = 0x00;
	proto.ethreg.dmac[4] = 0x00;
	proto.ethreg.dmac[5] = 0x15;
	proto.ethreg.dmac_valid = 1;	

	pkt_register(MODULE_ID_ISIS,PROTO_TYPE_ETH, &proto);
	
	proto.ethreg.dmac[0] = 0x09;
	proto.ethreg.dmac[1] = 0x00;
	proto.ethreg.dmac[2] = 0x2B;
	proto.ethreg.dmac[3] = 0x00;
	proto.ethreg.dmac[4] = 0x00;
	proto.ethreg.dmac[5] = 0x05;
	proto.ethreg.dmac_valid = 1;	

	pkt_register(MODULE_ID_ISIS,PROTO_TYPE_ETH, &proto);

	proto.ethreg.dmac[0] = 0x09;
	proto.ethreg.dmac[1] = 0x00;
	proto.ethreg.dmac[2] = 0x2B;
	proto.ethreg.dmac[3] = 0x00;
	proto.ethreg.dmac[4] = 0x00;
	proto.ethreg.dmac[5] = 0x04;
	proto.ethreg.dmac_valid = 1;	

	pkt_register(MODULE_ID_ISIS,PROTO_TYPE_ETH, &proto);
}
#endif
void isis_pkt_register(void)
{
         union proto_reg proto;

         memset(&proto, 0, sizeof(union proto_reg));
         proto.ethreg.ethtype = ETH_P_ISIS;
         proto.ethreg.sub_ethtype = ETH_SUBTYPE_ISIS;
         pkt_register(MODULE_ID_ISIS,PROTO_TYPE_ETH, &proto);
}

void isis_pkt_init(void)
{
    //thread_add_event (master, isis_receive, NULL, 0);

	isis_pkt_register();
}

