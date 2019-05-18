/*
*     定义公共的错误码
*/


#ifndef HIOS_ERRCODE_H
#define HIOS_ERRCODE_H


/* 错误码定义 */
#define ERRNO_SUCCESS                0  /* 成功 */
#define ERRNO_FAIL                   1  /* 失败 */
#define ERRNO_NOT_FOUND              2  /* 没有找到 */
#define ERRNO_EXISTED                3  /* 已经存在 */
#define ERRNO_UNMATCH                4  /* 参数不匹配 */
#define ERRNO_TIMEOUT                5  /* 超时 */
#define ERRNO_OVERSIZE               6  /* 超出数量限制 */
#define ERRNO_MALLOC                 7  /* 内存分配失败 */
#define ERRNO_IPC                    8  /* IPC 发送失败 */
#define ERRNO_CLI                    9  /* 命令行错误 */
#define ERRNO_PARAM_ILLEGAL          10 /* 输入参数非法 */
#define ERRNO_HAL                    11 /* HAL 返回失败 */
#define ERRNO_FTM                    12 /* FTM 返回失败 */
#define ERRNO_INDEX_ALLOC            13 /* 索引分配失败 */
#define ERRNO_PORT_ALLOC             14 /* 端口分配失败 */
#define ERRNO_PKT_SEND               15 /* 报文发送失败 */
#define ERRNO_PKT_RECV               16 /* 报文接收失败 */
#define ERRNO_OVERLAP                17 /* 范围重叠 */
#define ERRNO_CONFIG_INCOMPLETE      18 /* 配置不完整 */


#define ERRNO_MAX                   100 /* 公共错误码最大到 100，100 以上用于各模块自定义的错误码*/


/* route 模块错误码定义 */
#define ERRNO_ROUTE					ERRNO_MAX

#define ERRNO_EXISTED_NHP			ERRNO_ROUTE+1  /* 下一跳存在 */
#define ERRNO_EXISTED_STATIC_ROUTE	ERRNO_ROUTE+2  /* 静态路由存在 */
#define ERRNO_INVALID_IP			ERRNO_ROUTE+3  /* 无效的 ip 地址 */
#define ERRNO_EXISTED_IP			ERRNO_ROUTE+4  /* ip 地址存在 */
#define ERRNO_CONFLICT_IP			ERRNO_ROUTE+5  /* ip 地址冲突 */
#define ERRNO_SUBNET_SIMILAR		ERRNO_ROUTE+6  /* ip 网段重叠 */
#define ERRNO_NOT_UPDATE			ERRNO_ROUTE+7  /* 不需更新 */
#define ERRNO_SUCCESS_UPDATE		ERRNO_ROUTE+8  /* 更新成功 */
#define ERRNO_ROUTER_ID             ERRNO_ROUTE+9  /* router-id 未配置 */


#define ERRNO_MAX_ROUTE				ERRNO_MAX+50   /* route 模块错误码最大值 */

/* mpls 模块错误码定义 */
#define ERRNO_MPLS					ERRNO_MAX_ROUTE

#define ERRNO_MODE_NOT_L2			ERRNO_MPLS+1   /* 接口模式不是 L2 */
#define ERRNO_EXISTED_PW_DESTIP		ERRNO_MPLS+2   /* 相同目的的 PW 已经存在 */
#define ERRNO_ALREADY_BIND_L2VPN	ERRNO_MPLS+3   /* 接口已经绑定 L2VPN */
#define ERRNO_ALREADY_BIND_PW		ERRNO_MPLS+4   /* 接口已经绑定 PW */
#define ERRNO_ALREADY_BIND_TPOAM    ERRNO_MPLS+5   /* 已经绑定 tpoam */
#define ERRNO_ALREADY_ENABLE_OAM    ERRNO_MPLS+6   /* OAM 会话已经被使能*/
#define ERRNO_MASTER_NOT_EXISTED    ERRNO_MPLS+7   /* 备依赖主，主不存在 */
#define ERRNO_PEER_IP_UNMATCH       ERRNO_MPLS+8   /* peer ip 与 tunnel dip 不匹配 */
#define ERRNO_MODE_NOT_CONFIG       ERRNO_MPLS+9   /* tdm 接口模式未配置 */
#define ERRNO_GRE_NOT_SUPPORT       ERRNO_MPLS+10  /* 不支持 gre 配置 */
#define ERRNO_PW_INGRESS_CIR        ERRNO_MPLS+11  /* pw 配置的 ingress cir 小于 tunnel 上剩余 cir */
#define ERRNO_PW_EGRESS_CIR         ERRNO_MPLS+12  /* pw 配置的 egress cir 小于 tunnel 上剩余 cir */
#define ERRNO_INTERFACE_NOT_EXIST   ERRNO_MPLS+13  /* GRE outif not exist */
#define ERRNO_OUTIF_NOT_SUPPORT     ERRNO_MPLS+14  /* GRE outif 不支持作为NNI端口 */
#define ERRNO_OUTIF_UNMATCH_TUNNEL  ERRNO_MPLS+15  /*接口的slot和tunnel 的slot不匹配 */
#define ERRNO_OUTIF_NOT_L3			ERRNO_MPLS+16  /*GRE outif not l3 interface */

#define ERRNO_MAX_MPLS				ERRNO_MAX_ROUTE+50  /* mpls 模块错误码最大值 */

/*ELPS模块错误码定义*/

#define ERRNO_ELPS         	ERRNO_MAX_MPLS

#define ELPS_LOWER_PRIO_CMD	ERRNO_ELPS+1 /* 输入低优先级指令 */
#define ELPS_IS_ALREADY_FS		ERRNO_ELPS+2 /* FS 命令重复 */
#define ELPS_IS_NOT_FS			ERRNO_ELPS+3 /* 未输入FS命令 */
#define ELPS_IS_ALREADY_MS	ERRNO_ELPS+4/* MS 命令重复 */
#define ELPS_IS_NOT_MS			ERRNO_ELPS+5 /* 未输入MS命令 */
#define ELPS_IS_ALREADY_LOP	ERRNO_ELPS+6 /* LOP 命令重复 */
#define ELPS_IS_NOT_LOP		ERRNO_ELPS+7 /* 未输入LOP 命令 */
#define ELPS_RECV_APS_ON_MASTER	ERRNO_ELPS+8 /* 在工作端口收到APS报文*/
#define ELPS_RECV_APS_ABDR_INVALID	ERRNO_ELPS+9 /* 收到的APS 报文中ABDR 无效*/
#define ELPS_RECV_APS_B_INVALID	ERRNO_ELPS+10 /* 收到的APS 报文中B 不匹配*/
#define ELPS_RECV_APS_RS_INVALID	ERRNO_ELPS+11 /* 收到的APS 报文中request signal 不匹配*/
#define ELPS_NO_APS_RECV		ERRNO_ELPS+12 /* 收到的APS 报文中request signal 不匹配*/

#define ERRNO_MAX_ELPS				ERRNO_MAX_MPLS+50  /* ELPS 模块错误码最大值 */

/*mpls aps模块错误码定义*/

#define ERRNO_MPLSAPS         	ERRNO_MAX_ELPS

#define MPLSAPS_LOWER_PRIO_CMD	      ERRNO_MPLSAPS+1 /* 输入低优先级指令 */
#define MPLSAPS_IS_ALREADY_FS		  ERRNO_MPLSAPS+2 /* FS 命令重复 */
#define MPLSAPS_IS_NOT_FS			  ERRNO_MPLSAPS+3 /* 未输入FS命令 */
#define MPLSAPS_IS_ALREADY_MS	      ERRNO_MPLSAPS+4/* MS 命令重复 */
#define MPLSAPS_IS_NOT_MS			  ERRNO_MPLSAPS+5 /* 未输入MS命令 */
#define MPLSAPS_IS_ALREADY_LOP	      ERRNO_MPLSAPS+6 /* LOP 命令重复 */
#define MPLSAPS_IS_NOT_LOP		      ERRNO_MPLSAPS+7 /* 未输入LOP 命令 */
#define MPLSAPS_RECV_APS_ON_MASTER	  ERRNO_MPLSAPS+8 /* 在工作端口收到APS报文*/
#define MPLSAPS_RECV_APS_ABDR_INVALID ERRNO_MPLSAPS+9 /* 收到的APS 报文中ABDR 无效*/
#define MPLSAPS_RECV_APS_B_INVALID	  ERRNO_MPLSAPS+10 /* 收到的APS 报文中B 不匹配*/
#define MPLSAPS_RECV_APS_RS_INVALID	  ERRNO_MPLSAPS+11 /* 收到的APS 报文中request signal 不匹配*/
#define MPLSAPS_NO_APS_RECV		      ERRNO_MPLSAPS+12 /* 收到的APS 报文中request signal 不匹配*/

#define ERRNO_MAX_MPLSAPS		      ERRNO_MAX_MPLS+50  /* MPLSAPS 模块错误码最大值 */

/*ifm 模块错误码定义*/          		     
#define ERRNO_IFM                       	 ERRNO_MAX_MPLSAPS
#if 0
#define IFM_SUCCESS                          ERRNO_IFM + 1 /*IFM接口存在*/
#define IFM_ERR_GENERAL                      ERRNO_IFM + 2 /*接口基本信息错误*/
#define IFM_ERR_VLAN_NOT_FOUND               ERRNO_IFM + 3 /*没有封装vlan*/
#define IFM_ERR_UNIT_NOT_FOUND               ERRNO_IFM + 4 /*获取机箱号失败*/
#define IFM_ERR_SLOT_NOT_FOUND               ERRNO_IFM + 5 /*获取槽位号失败*/
#define IFM_ERR_PORT_NOT_FOUND               ERRNO_IFM + 6 /*获取端口号失败*/
#define IFM_ERR_EXISTS                       ERRNO_IFM + 7 /*返回错误值*/
#define IFM_ERR_NOT_FOUND                    ERRNO_IFM + 8 /*接口查找失败*/
#define IFM_ERR_NOT_BOUND                    ERRNO_IFM + 9 /*预留*/
#define IFM_ERR_ALREADY_BOUND                ERRNO_IFM + 10 /*预留*/
#define IFM_ERR_MISMATCH                     ERRNO_IFM + 11 /*接口类型无效*/
#define IFM_ERR_MEM                          ERRNO_IFM + 12 /*内存越界*/
#define IFM_ERR_HAL                          ERRNO_IFM + 13 /*向HAL发送信息失败*/
#define IFM_ERR_PARAM                        ERRNO_IFM + 14 /*指针参数为空*/
#define IFM_ERR_NOTIFY                       ERRNO_IFM + 15 /*向FTM发送消息失败*/
#define IFM_ERR_XMALLOC                      ERRNO_IFM + 16 /*申请内存失败*/
#define IFM_ERR_HASH_ADD                     ERRNO_IFM + 17 /*向HASH表增加数据失败*/
#define IFM_ERR_ENCAP_UNTAG                  ERRNO_IFM + 18 /*封装类型为UNTAG*/
#define IFM_ERR_ENCAP_DOT1Q                  ERRNO_IFM + 19 /*封装类型为DOT1Q*/
#define IFM_ERR_ENCAP_QINQ                   ERRNO_IFM + 20 /*封装类型为QINQ*/
#define IFM_ERR_ENCAP_RANGE                  ERRNO_IFM + 21 /*VLAN值起始范围错误*/
#define IFM_ERR_ENCAP_RANGE_L3               ERRNO_IFM + 22 /*L3模式下不能配置VLAN值起始范围*/
#define IFM_ERR_PARNT_NOT_FOUND              ERRNO_IFM + 23 /*父接口不存在*/
#define IFM_ERR_ENCAP_UNTAG_L3               ERRNO_IFM + 24 /*L3模式下不能配置UNTAG*/
#define IFM_ERR_PARNT_NO_L3                  ERRNO_IFM + 25 /*接口模式不是L3*/
#define IFM_ERR_PARAM_MAC_MC                 ERRNO_IFM + 26 /*设置组播MAC*/
#define IFM_ERR_PARAM_MAC_BC                 ERRNO_IFM + 27 /*设置单播MAC*/
#define IFM_ERR_PARAM_MAC_ZERO               ERRNO_IFM + 28 /*MAC值为全0*/
#define IFM_ERR_PARAM_SPEED_AUTO             ERRNO_IFM + 29 /*速率自协商模式*/
#define IFM_ERR_PARAM_NOSUPPORT              ERRNO_IFM + 30 /*端口不支持配置*/
#define IFM_ERR_PARAM_E1_NO_FRAMED           ERRNO_IFM + 31 /*E1接口非成帧模式*/
#define IFM_ERR_ENCAP_VLANT_MODE_ERRO        ERRNO_IFM + 32 /*封装VALN的模式错误*/
#define IFM_ERR_WRONG_QOS_TYPE               ERRNO_IFM + 33 /*QOS映射类型错误*/
#define IFM_ERR_ENCAP_XLVLAN_SAME_ERRO       ERRNO_IFM + 34 /*设置VLAN为VLAN本身*/
#define IFM_ERR_BINDED_TRUNK_ERRO            ERRNO_IFM + 35 /*接口因绑定TRUNK而设置失败*/
#define IFM_ERR_INTERFACE_OUTOFNUM_ERRO      ERRNO_IFM + 36 /*接口数量超出最大值*/
#define IFM_ERR_ENCAP_EXIST                  ERRNO_IFM + 37 /*接口封装已设置*/
#define IFM_ERR_PARAM_TRUNK_RANGE_32         ERRNO_IFM + 38 /*超出TRUNK口数值范围*/
#define IFM_ERR_PARAM_SUPPORT_ONE            ERRNO_IFM + 39 /*只支持一个接口*/
#define IFM_ERR_PARAM_FORMAT                 ERRNO_IFM + 40 /*接口索引值为0*/
#define IFM_ERR_PORT_NOSUPPORT_DELETE        ERRNO_IFM + 41 /*物理口不能删除*/
#define IFM_ERR_STRING_OUT_RANGE             ERRNO_IFM + 42 /*字符串越界*/
#define IFM_ERR_PARAM_MAC_INVALID            ERRNO_IFM + 43 /*无效MAC*/
#define IFM_ERR_PARAM_IP_INVALID             ERRNO_IFM + 44 /*无效IP*/
#define IFM_ERR_LOOPBACK_NO_SET              ERRNO_IFM + 45 /*环回未配置*/
#define IFM_ERR_SLA_IS_SETTED                ERRNO_IFM + 46 /*SLA会话使能*/
#define IFM_ERR_TRUNK_NO_EXIST               ERRNO_IFM + 47 /*trunk口不存在*/
#define IFM_ERR_LOOPBACK_EX_SET              ERRNO_IFM + 48 /*设置外环回*/
#define IFM_ERR_LOOPBACK_IN_SET              ERRNO_IFM + 49 /*设置内环回*/
#define IFM_ERR_DCN_ENABLE                   ERRNO_IFM + 50 /*dcn已经配置*/
#define IFM_ERR_PARAM_TUNNEL_RANGE_128		 ERRNO_IFM + 51 /*HT201设备TUNNEL口的范围*/
#define IFM_ERR_PARAM_TUNNEL_RANGE_2000		 ERRNO_IFM + 52 /*HT2200设备TUNNEL口的范围*/
#define IFM_ERR_TUNNEL_UNIT                  ERRNO_IFM + 53 /*tunnel口的UNIT不为1*/
#endif

#define ERRNO_MAX_IFM                        ERRNO_IFM + 100  /*IFM错误码最大值*/
/* FTM_IFM错误码定义 */
#define ERRNO_FTM_IFM                        ERRNO_MAX_IFM  
#if 0
#define FTM_IFM_SUCCESS                      ERRNO_FTM_IFM + 1 /*FTM接口表项中接口存在*/

#define FTM_IFM_ERR_GENERAL                  ERRNO_FTM_IFM + 2 /*FTM接口表项中接口基本信息错误*/
#define FTM_IFM_ERR_MEM                      ERRNO_FTM_IFM + 3 /*FTM中接口申请内存失败*/
#define FTM_IFM_ERR_HAL                      ERRNO_FTM_IFM + 4 /*FTM向HAL发送消息失败*/
#define FTM_IFM_ERR_PARAM                    ERRNO_FTM_IFM + 5 /*FTM指针参数为空*/
#define FTM_IFM_ERR_NOTIFY                   ERRNO_FTM_IFM + 6 /*预留*/
#define FTM_IFM_ERR_MISMATCH                 ERRNO_FTM_IFM + 7 /*FTM_IFM接口类型不匹配*/
#define FTM_IFM_ERR_NOT_FOUND                ERRNO_FTM_IFM + 8 /*FTM_IFM查找接口失败*/
#endif
#define ERRNO_MAX_FTM_IFM                    ERRNO_FTM_IFM +50 /*FTM_IFM错误码最大值*/

/*CES 模块错误码定义*/
#define ERRNO_CES                          	ERRNO_MAX_FTM_IFM 
#define CES_ERR_MODE_NO_SET               	ERRNO_CES + 1 /*成帧模式未配置*/
#define CES_ERR_EXE_FAILED                 	ERRNO_CES + 2 /*命令执行失败*/
#define CES_ERR_UNFRAM_NO_SET              	ERRNO_CES + 3 /*非成帧下不可配*/
#define CES_ERR_NOSUPPORT_CESOPSN          	ERRNO_CES + 4 /*cesopsn不支持*/
#define CES_ERR_CREATE                     	ERRNO_CES + 5 /*创建失败*/
#define CES_ERR_DELETE                 	   	ERRNO_CES + 6 /*删除失败*/
#define CES_ERR_TIMESLOT                   	ERRNO_CES + 7 /*时隙设置错误*/
#define CES_ERR_TIMESLOT_USED              	ERRNO_CES + 8 /*时隙已被使用*/
#define CES_ERR_PRBS_USED              	  	ERRNO_CES + 9 /*prbs已被使用*/

#define ERRNO_MAX_CES                      	ERRNO_CES + 50 /*CES错误码最大值*/

/*EFM 模块错误码定义*/
#define ERRNO_EFM_BASE                 			ERRNO_MAX_CES
#define EFM_ERR_IF_NOT_FOUND			        (ERRNO_EFM_BASE + 1) /*接口不存在*/
#define EFM_ERR_NOT_ENABLED          	        (ERRNO_EFM_BASE + 2) /*EFM没有使能*/
#define EFM_ERR_ENABLED              		    (ERRNO_EFM_BASE + 3) /*EFM已经使能*/
#define EFM_ERR_MEMORY               		    (ERRNO_EFM_BASE + 4) /*EFM没有创建*/
#define EFM_ERR_REM_LB_NOT_SUPP	                (ERRNO_EFM_BASE + 5) /*不支持远端环回*/
#define EFM_ERR_DISCOVERY            	        (ERRNO_EFM_BASE + 6) /*EFM发现状态不是send_any*/
#define EFM_ERR_RB_MODE            		        (ERRNO_EFM_BASE + 7) /*EFM正处于远端环回模式*/
#define EFM_ERR_SET_UNIDIREC      		        (ERRNO_EFM_BASE + 8) /*EFM配置支持单纤强制发包模式失败*/
#define EFM_ERR_LOCALSIDE_MAKE_REMOTESIDE_STARTED       (ERRNO_EFM_BASE + 9) /*EFM已经使能远端环回了*/ 
#define EFM_ERR_REMOTESIDE_MAKE_LOCALSIDE_STARTED       (ERRNO_EFM_BASE + 10) /*EFM已经处于本端环回了*/
#define EFM_ERR_MODE_PASSIVE  		                    (ERRNO_EFM_BASE + 11) /*EFM处于被动模式*/

#define ERRNO_MAX_EFM                      	ERRNO_EFM_BASE + 50 /*EFM错误码最大值*/

/*TRUNK 模块错误码定义*/								
#define ERRNO_L2_TRUNK						ERRNO_MAX_EFM
#define L2_TRUNK_ERR_GET_L2IF               ERRNO_L2_TRUNK + 1  /*获取l2if失败*/
#define L2_TRUNK_ERR_NOT_FOUND              ERRNO_L2_TRUNK + 2  /*trunk接口查找失败*/
#define L2_TRUNK_ERR_MEMBER_OUTOFNUM        ERRNO_L2_TRUNK + 3  /*成员数超出最大范围*/
#define L2_TRUNK_ERR_HAL                    ERRNO_L2_TRUNK + 4  /*发送至hal失败*/
#define L2_TRUNK_ERR_NOFAILBACK_NO_SET      ERRNO_L2_TRUNK + 5  /*非回切不可设置*/
#define L2_TRUNK_ERR_NOSUPPORT_OUTBAND      ERRNO_L2_TRUNK + 6  /*管理口不支持*/
#define L2_TRUNK_ERR_MEMBER_USED            ERRNO_L2_TRUNK + 7  /*已作为其他聚合口成员*/
#define L2_TRUNK_ERR_LACP_DISABLE           ERRNO_L2_TRUNK + 8  /*lacp未使能*/
#define L2_TRUNK_ERR_MANUAL_ECMP_NO_SET     ERRNO_L2_TRUNK + 9  /*手工聚合负载分担模式不可配置*/
#define L2_TRUNK_ERR_NOT_TRUNK_MEMBER       ERRNO_L2_TRUNK + 10 /*端口非聚合组成员*/
#define L2_TRUNK_ERR_ECMP_NO_SET            ERRNO_L2_TRUNK + 11 /*负载分担方式不可配置*/
#define L2_TRUNK_ERR_BACKUP_NO_SET          ERRNO_L2_TRUNK + 12 /*主备方式不可配置*/
#define L2_TRUNK_ERR_IF_NO_EXISTED          ERRNO_L2_TRUNK + 13 /*trunk口不存在*/
#define L2_TRUNK_ERR_MEMBER_ADD             ERRNO_L2_TRUNK + 14 /*添加成员失败*/
#define L2_TRUNK_ERR_MEMBER_DEL             ERRNO_L2_TRUNK + 15 /*删除成员失败*/
#define L2_TRUNK_ERR_MEMBER_INIT            ERRNO_L2_TRUNK + 16 /*初始化成员失败*/
#define L2_TRUNK_ERR_TRUNK_DEL              ERRNO_L2_TRUNK + 17 /*trunk接口删除*/
#define L2_TRUNK_ERR_TRUNK_UN_CONFIGURABLE  ERRNO_L2_TRUNK + 18

#define ERRNO_MAX_L2_TRUNK                  ERRNO_L2_TRUNK + 50 /*trunk模块错误码最大值*/

#define BIT(X) (0X1 << X)

extern const char *errcode_get_string (int errcode);


#endif




