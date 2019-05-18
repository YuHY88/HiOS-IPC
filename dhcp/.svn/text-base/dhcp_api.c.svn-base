/* Copyright (C) 2012-2013 HUAHUAN, Inc. All Rights Reserved. */


//#include <linux/if_vlan.h>
#include <sys/types.h>
//#include <thread.h>
#include <pthread.h>
//#include "timeutil.h"
#include <net/if.h>
#include <sys/ioctl.h>
#include <string.h>
#include <stdlib.h>

#include "dhcp.h"
#include "dhcp_api.h"
#include "dhcp_snmp.h"
#include "dhcp_client.h"
#include "dhcp_server.h"
#include "dhcp_relay.h"

#if defined(HH_CARD_1610)
extern int bcm_mng_vlan_add(int unit, int port, int vid, int pri, int tag, int flag);
extern int bcm_mng_vlan_del(int unit, int port, int vid, int pri);
static int add_vlan_num = DHCP_ADD_VLAN_NUM_NONE;
#endif


#if defined(HH_CARD_1610)

void dhcp_add_vlan_num_set(int vlanid)
{
	add_vlan_num = vlanid;
}

int dhcp_add_vlan_num_get(void)
{
	return add_vlan_num;	
}

void dhcp_all_vlan_add(void)
{
	int vid;
	
	if(dhcp_add_vlan_num_get() != DHCP_MAX_VLAN)
	{
		for(vid = DHCP_MIN_VLAN + 1; vid <= DHCP_MAX_VLAN; vid++)
		{
			if(dhcpcm.conf->MgmtVidFlag == 1)
			{
				dhcp_all_vlan_remove(dhcpcm.conf->InBandVlanID);
				break;
			}
			
			dhcp_add_vlan_num_set (vid);
			//set fx tag tx utag
			
			/*if(dhcpflag)
			{
				bcm_mng_vlan_add(UNIT_DEFAULT, 28, vid, PRI_DEFAULT, VLAN_UNTAG, DHCP_VLAN_FLAG);
				bcm_mng_vlan_add(UNIT_DEFAULT, 29, vid, PRI_DEFAULT, VLAN_UNTAG, DHCP_VLAN_FLAG);
				bcm_mng_vlan_add(UNIT_DEFAULT, 30, vid, PRI_DEFAULT, VLAN_UNTAG, DHCP_VLAN_FLAG);
				bcm_mng_vlan_add(UNIT_DEFAULT, 31, vid, PRI_DEFAULT, VLAN_UNTAG, DHCP_VLAN_FLAG);
			}*/
			bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 27, vid, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, !DHCP_VLAN_FLAG);
			bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 32, vid, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, !DHCP_VLAN_FLAG);
			bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 33, vid, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, !DHCP_VLAN_FLAG);
			bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 34, vid, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, !DHCP_VLAN_FLAG);

			//bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, DHCP_PORT_CPU, vid, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, DHCP_VLAN_FLAG);
	    }
		//bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, DHCP_PORT_CPU, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, DHCP_VLAN_FLAG);
	}
}

void dhcp_port_defvlan_set(int tag_flag)
{

	if(dhcpcm.conf->MgmtVidFlag == 1)
	{
		dhcp_all_vlan_remove(dhcpcm.conf->InBandVlanID);
	}
	
	bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 27, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, 0);
	bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 32, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, 0);
	bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 33, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, 0);
	bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, 34, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, 0);

	bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, DHCP_PORT_CPU, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, DHCP_VLAN_FLAG);
}


//remove vlan excpet mgmtvid and vlan 1 
void dhcp_all_vlan_remove(int expvlan)
{
	int k = DHCP_MIN_VLAN;
	int vlanend = DHCP_MAX_VLAN;
	
	vlanend = dhcp_add_vlan_num_get();/*this flag  can find if dhcp create vlan the range : 1-dhcp_addvlanflag*/
	
	for(k = DHCP_MIN_VLAN + 1; k <= vlanend; k++)
	{
		if(k == expvlan)
		{
			continue;
		}
		//bcm_mng_vlan_del(DHCP_UNIT_DEFAULT, DHCP_PORT_ALL, k, DHCP_PRI_DEFAULT);
		
		bcm_mng_vlan_del(DHCP_UNIT_DEFAULT, 27, k, DHCP_PRI_DEFAULT);
		bcm_mng_vlan_del(DHCP_UNIT_DEFAULT, 32, k, DHCP_PRI_DEFAULT);
		bcm_mng_vlan_del(DHCP_UNIT_DEFAULT, 33, k, DHCP_PRI_DEFAULT);
		bcm_mng_vlan_del(DHCP_UNIT_DEFAULT, 34, k, DHCP_PRI_DEFAULT);
		dhcp_add_vlan_num_set (k);
	}
	dhcp_add_vlan_num_set (DHCP_ADD_VLAN_NUM_NONE);

    //recovery defualt vlan 1
	//bcm_mng_vlan_add(DHCP_UNIT_DEFAULT, DHCP_PORT_ALL, DHCP_VLAN_DEFAULT, DHCP_PRI_DEFAULT, DHCP_VLAN_TAG, DHCP_VLAN_FLAG);
}

#endif

#if defined(HH_CARD_1610) || defined(HH_CARD_1604C) 
void dhcp_mgmt_vidflag_recover(void)
{
    FILE* fp;
    char mgmvidflagStr[20] = {0};
	int mgmvidflag = 0;
    char buf[128];
    
    if(access(DHCP_ROM_NODE(management-vid-flag),0) == 0)
    {
        fp = fopen(DHCP_ROM_NODE(management-vid-flag), "r");
        if(fp)
        {
            fgets(mgmvidflagStr, 20, fp);
            mgmvidflag = strtol(mgmvidflagStr, NULL, 10);
            fclose(fp);
        }
    }
    else
    {
        mgmvidflag = 0;
        sprintf(buf, "echo %d > %s",mgmvidflag,DHCP_ROM_NODE(management-vid-flag));
        system(buf);
    }
    
    dhcpcm.conf->MgmtVidFlag = mgmvidflag;
	//printf("dhcp_mgmt_vidflag_recover mgmvidflag=%d\n", mgmvidflag);
    
}

void dhcp_mgmt_vidflag_save(void)
{
    char buf[128];
    
	sprintf(buf, "echo %d > %s", dhcpcm.conf->MgmtVidFlag, DHCP_MGMT_FLAG);
    system(buf);
}

#endif

void dhcp_netif_iplongtostr(char *pIpStr, int iBufLen, const unsigned long ulIP)
{
    if(NULL != pIpStr)
    {
        snprintf(pIpStr, iBufLen, "%d.%d.%d.%d", 
                (int)((ulIP >> 24) & 0xff), (int)((ulIP >> 16) & 0xff),(int)((ulIP >> 8) & 0xff), (int)(ulIP & 0xff));
    }
}

unsigned long dhcp_netif_ipstrtolong(const char *pIpStr)
{
    int   lIndex  = 0;
    int   lStrLen = 0; 
    
    char *pTpHead = NULL;
    char *pTpFind = NULL;
    char  bStrBuf[32];
        
    unsigned long uIpAddr[4];

    if(NULL == pIpStr)
	{
		return(-1);
    }
    lStrLen  = strlen(pIpStr);
        
    if((lStrLen < 7) || (lStrLen > 15))
	{ 
		return(-1);
    }
    for(lIndex = 0; lIndex <lStrLen; lIndex++)
    {
        if(!((pIpStr[lIndex] == '.') || 
            ((pIpStr[lIndex] >= '0') && (pIpStr[lIndex] <= '9'))))
        {
            return(-1);
        }
    }

    pTpFind = bStrBuf;
        
    strcpy(bStrBuf, pIpStr);
        
    for(lIndex = 0; lIndex < 4; lIndex++)
    {
        pTpHead = pTpFind;

        if(lIndex < 3)
        {
            pTpFind = (char *)strchr(pTpHead, '.');
                        
            if(!pTpFind)
			{
				return(-1);
            }
           *pTpFind = 0;
            pTpFind ++;
        }

        lStrLen = strlen(pTpHead);
                
        if((lStrLen < 1) || (lStrLen > 3))
		{
			return(-1);
        }
        uIpAddr[lIndex] = atol(pTpHead);
                
        if(uIpAddr[lIndex] > 255)
		{ 
			return(-1);
		}
    }

    return((uIpAddr[0] << 24) | (uIpAddr[1] << 16) | (uIpAddr[2] << 8) | uIpAddr[3]);
}
