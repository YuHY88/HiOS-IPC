/**
 * \page cmds_ref_ping PING
 * - \subpage modify_log_ping
 * - \subpage ping_cmd
 * - \subpage tracert_cmd
 * - \subpage ping6_cmd
 *
 */

/**
 * \page modify_log_ping Modify Log
 * \section ping-v007r004 HiOS-V007R004
 *  -#
 * \section ping-v007r003 HiOS-V007R003
 *  -#
 */
#include <zebra.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include "vtysh.h"

/**
 * \page ping_cmd ping ip A.B.C.D {-a A.B.C.D|-c <1-255>|-h <1-255>|-w <1-30>|-s <18-9600>|-f <0-1>|vpn <1-1024>}
 * - 功能说明 \n
 *   ping命令用来检查指定的IPv4地址是否可达，并输出相应的统计信息
 * - 命令格式 \n
 *   ping ip A.B.C.D {-a A.B.C.D|-c <1-255>|-h <1-255>|-w <1-30>|-s <18-9600>|-f <0-1>|vpn <1-1024>}
 * - 参数说明 \n
 *   |参数  |说明|取值        |
 *   |------|--------------|----|
 *   |A.B.C.D   |目的主机的IP地址|点分十进制形式|
 *   |-a A.B.C.D |指定发送icmp请求报文的源ip地址。如果不指定源ip地址，将采用报文出接口的ip地址作为icmp请求报文发送的源ip地址|点分十进制形式|
 *   |-c <1-255>|指定发送icmp请求报文的个数。执行ping命令每发送一个icmp请求报文，icmp请求字段中的序号sequence加1.序号从1开始，不指定-c参数情况下默认发送3个请求包|整形形式，取值范围1~255，缺省值是3|
 *   |-h <1-255>|指定TTL值。报文在转发过程中，如果TTL值减为0，报文会被丢弃，同时报文的发送者会收到icmp超时的应答，表明目标设备不可达|整形形式，取值范围1~255，缺省值是255|
 *   |-w <1-30> |指定发送完icmp请求报文后，等待icmp应答报文的超时时间。如果在超时时间内收到icmp应答，则说明目的ip可达，发送端回显时间；如果在超时时间内没有收到icmp应答，则在发送端显示超时。当网络传输速率较慢时，可以使用此参数调整报文的超时时间|整形形式，取值范围1~30，单位是秒，缺省超时时间是2秒|
 *   |-s <18-9600>|指定icmp请求报文负载的长度（不包含ip报头和icmp报头）|整数形式，取值范围是18～9600，单位是字节。缺省负载长度是18字|
 *   |-f <0-1>|设置报文发送的过程中是否分片|0:支持分片 1：不支持分片|
 *   |vpn|vpn instance ID|范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *   缺省情况下，不指定源ip地址，发送3个icmp请求报文，ttl值为255，超时时间为2s，请求报文负载为18字节，支持分片，vpn为0
 * - 命令模式 \n
 *   命令行安装在配置模式下
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     使用该命令检查网络是否连通
 *   - 前置条件 \n
 *     PING进程启动 \n
 *     FTM、IFM、HAL、VTY等有依赖的进程正常 \n
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     1、如果ping执行过程出现异常，可以使用CTRL＋C键中止ping命令 \n
 *     2、当目标主机不可达或等待应答超时时，系统显示“Ping time out” \n
 *     3、当目标主机为127.0.0.0时，系统显示“General fault!”，请求超时 \n
 *	   4、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     无
 *
 */
DEFSH (VTYSH_PING,
        ping_cmd,
        "ping {ip} A.B.C.D {-a A.B.C.D|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|-f <0-1>|vpn <1-1024>}",
        "ping\n"
        "Ping ip address\n"
        "Ip address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Set Don't Fragment flag in packet (IPv4-only)\n"
        "frag flag 0:frag 1:no frag\n"
        "Vpn\n"
        "vpn value\n"
		)

/**
 * \page tracert_cmd tracert ip A.B.C.D {-a A.B.C.D|-m <1-255>|-c <1-255>|-w <1-30>|-s <18-9600>|vpn <1-1024>}
 * - 功能说明 \n
 *   tracert命令主要用于查看数据包从源端到目的端的路径信息，从而检查网络连接是否可用。当网络出现故障时，用户可以使用该命令定位故障点
 * - 命令格式 \n
 *   tracert ip A.B.C.D {-a A.B.C.D|-m <1-255>|-c <1-255>|-w <1-30>|-s <18-9600>|vpn <1-1024>}
 * - 参数说明 \n
 *   |参数  |说明|取值        |
 *   |------|--------------|----|
 *   |A.B.C.D |目的主机的IP地址|点分十进制形式|
 *   |-a A.B.C.D |指定tracert发送icmp请求报文的源ip地址。如果不指定源ip地址，将采用报文出接口的ip地址作为icmp请求报文发送的源ip地址|点分十进制形式|
 *   |-m <1-255>|指定最大TTL。默认情况下，max-ttl值被设置为经过路由器的个数|整数形式，取值范围是1～255。缺省值是30|
 *   |-c <1-255>|指定每跳发送icmp探测报文的个数。网络不畅通时，可以增加发送探测报文的个数，保证探测报文能够到达目的节点|整形形式，取值范围1~255，缺省值是3|
 *   |-w <1-30> |指定发送完icmp请求报文后，等待icmp应答报文的超时时间。如果在超时时间内收到icmp应答，则说明目的ip可达；如果在超时时间内没有收到icmp应答，则在发送端显示超时。当网络传输速率较慢时，可以使用此参数调整报文的超时时间|整形形式，取值范围1~30，单位是秒，缺省超时时间是2秒|
 *   |-s <18-9600>|指定icmp请求报文负载的长度（不包含ip报头和icmp报头）|整数形式，取值范围是18～9600，单位是字节。缺省负载长度是18字|
 *   |vpn|vpn instance ID|范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *   缺省情况下，不指定源ip地址，最大TTL默认值为30，发送请求探测报文个数为3，超时时间为2s， 请求报文负载为18字节，VPN为0
 * - 命令模式 \n
 *   命令行安装在配置模式下
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     在日常的系统维护中，首先可以执行ping命令查看网络连通的情况。当网络不通时，可以使用tracert命令查看网络中出现故障的位置，为故障诊断提供依据
 *   - 前置条件 \n
 *     PING进程启动 \n
 *     FTM、IFM、HAL、VTY等有依赖的进程正常 \n
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     1、路由可达时，命令显示从左到右分别代表了“生存时间”（每途经一个路由器结点自增1）、“发送的ICMP请求包返回时间”（单位为毫秒ms）和“途经路由器的IP地址” \n
 *     2、路由不可达或超时时，使用带有星号（*）的信息表示 \n
 *     3、当目标主机为127.0.0.0时，系统显示“General fault!” \n
 *     4、命令行给定vpn范围为<1-1024>;\n
 *        实际生效可配置范围如下：HT201/HT201E <1-127>;\n
 *        HT2200 <1-128>;\n
 *        命令行实现上统一检测vpn大于128时给出错误提示。
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     无
 *
 */
DEFSH (VTYSH_PING,
        tracert_cmd,
      	"tracert {ip} A.B.C.D {-a A.B.C.D|-m <1-255>|-c <1-255>|-w <1-30>|-s <20-9500>|vpn <1-1024>}",
      	"tracert\n"
      	"Tracert ip address\n"
      	"ip address\n"
        "Set source IP address, the default is the IP address of the output interface\n"
    	"source-ip-address\n"
    	"Max time to live, the default is 30\n"
    	"max-ttl\n"
      	"Number of probe packet, the default is 3\n"
    	"count value\n"
      	"Timeout in seconds to wait for each reply, the default is 1s\n"
      	"timeout value\n"
    	"Specify the number of data bytes to be sent, the default is 18bytes\n"
    	"packet size\n"
        "Vpn\n"
        "vpn value\n"
	  	)

/**
 * \page ping6_cmd ping ipv6 X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <18-9600>|-f <0-1>|vpn <1-1024>}
 * - 功能说明 \n
 *   ping命令用来检查指定的IPv6地址是否可达，并输出相应的统计信息
 * - 命令格式 \n
 *   ping ipv6 X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <18-9600>|-f <0-1>|vpn <1-1024>}
 * - 参数说明 \n
 *   |参数  |说明|取值        |
 *   |------|--------------|----|
 *   |X:X:X:X:X:X:X:X |目的主机的IPv6地址|十六进制形式|
 *   |-a X:X:X:X:X:X:X:X |指定发送icmpv6请求报文的源ipv6地址。如果不指定源ipv6地址，将采用报文出接口的ipv6地址作为icmpv6请求报文发送的源ipv6地址|十六进制形式|
 *   |-c <1-255>|指定发送icmpv6请求报文的个数。执行ping ipv6命令每发送一个icmpv6请求报文，icmpv6请求字段中的序号sequence加1.序号从1开始，不指定-c参数情况下默认发送3个请求包|整数形式，取值范围是1～255。缺省值是3|
 *   |-h <1-255>|指定TTL值。报文在转发过程中，如果TTL值减为0，报文会被丢弃，同时报文的发送者会收到icmpv6超时的应答，表明目标设备不可达|整形形式，取值范围1~255，缺省值是255|
 *   |-w <1-30> |指定发送完icmpv6请求报文后，等待icmpv6应答报文的超时时间。如果在超时时间内收到icmpv6应答，则说明目的主机可达，发送端回显时间；如果在超时时间内没有收到icmpv6应答，则在发送端显示超时。当网络传输速率较慢时，可以使用此参数调整报文的超时时间|整形形式，取值范围1~30，单位是秒，缺省超时时间是2秒|
 *   |-s <18-9600>|指定icmpv6请求报文负载的长度（不包含ip报头和icmpv6报头）|整数形式，取值范围是18～9600，单位是字节。缺省负载长度是18字节|
 *   |vpn|vpn instance ID|范围：1-1024，缺省值为 0|
 *
 * - 缺省情况 \n
 *   缺省情况下，不指定源ipv6地址，最大TTL默认值为30，发送请求探测报文个数为3，超时时间为1s， 请求报文负载为18字节，VPN为0
 * - 命令模式 \n
 *   命令行安装在配置模式下
 * - 用户等级 \n
 *   11
 * - 使用指南 \n
 *   - 应用场景 \n
 *     在日常的系统维护中，首先可以执行ping ipv6命令查看网络连通的情况。当网络不通时，可以使用tracert命令查看网络中出现故障的位置，为故障诊断提供依据
 *   - 前置条件 \n
 *     PING进程启动 \n
 *     FTM、IFM、HAL、VTY等有依赖的进程正常 \n
 *   - 后续任务 \n
 *     无
 *   - 注意事项 \n
 *     无
 *   - 相关命令 \n
 *     无
 * - 使用举例 \n
 *     Hios-v7.3(config)# ping ipv6 1000::3 \n
 *     Ping 1000::3: 18 data bytes,press CTRL_C to break \n
 *     Reply from 1000::3: bytes=26 Sequence=1 ttl=127 time:29ms \n
 *     Reply from 1000::3: bytes=26 Sequence=2 ttl=127 time:32ms \n
 *     Reply from 1000::3: bytes=26 Sequence=3 ttl=127 time:28ms \n
 *     ---PING6 statistics---- \n
 *     3 packets transmitted, 3 received , 0% lost \n
 *
 */

DEFSH (VTYSH_PING,
        ping6_eth_cmd,
        "ping ipv6 X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>} interface ethernet USP",
        "ping\n"
        "Ping ipv6 address\n"
        "Ipv6 address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value <1-128>\n"    
        INTERFACE_STR
        CLI_INTERFACE_ETHERNET_STR
        CLI_INTERFACE_ETHERNET_VHELP_STR
    )


DEFSH (VTYSH_PING,
        ping6_ge_cmd,
        "ping ipv6  X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>} interface gigabitethernet USP",
        "ping\n"
        "Ping ipv6 address\n"
        "Ipv6 address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value <1-128>\n"    
        INTERFACE_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_STR
        CLI_INTERFACE_GIGABIT_ETHERNET_VHELP_STR
    )

DEFSH (VTYSH_PING,
        ping6_xge_cmd,
        "ping ipv6  X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>} interface xgigabitethernet USP",
        "ping\n"
        "Ping ipv6 address\n"
        "Ipv6 address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value <1-128>\n"    
        INTERFACE_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_STR
        CLI_INTERFACE_XGIGABIT_ETHERNET_VHELP_STR
    )

DEFSH (VTYSH_PING,
    ping6_trunk_cmd,
    "ping ipv6  X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>} interface trunk TRUNK",
    "ping\n"
    "Ping ipv6 address\n"
    "Ipv6 address\n"
    "Select source IP address, the default is the IP address of the output interface\n"
    "source-ip-address\n"       
    "Specify the number of echo requests to be sent, the default is 5\n"
    "count value\n"
    "Specify TTL value for echo requests to be sent, the default is 255\n"
    "ttl value\n"   
    "Timeout in seconds to wait for each reply, the default is 1s\n"
    "time-out value\n"
    "Specify the bytes of payload to be sent, the default is 18bytes\n"
    "payload size\n"
    "Vpn\n"
    "vpn value <1-128>\n"    
    INTERFACE_STR
    CLI_INTERFACE_TRUNK_STR
    CLI_INTERFACE_TRUNK_VHELP_STR
)

DEFSH (VTYSH_PING,
        ping6_vlanif_cmd,
        "ping ipv6  X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>} interface vlanif <1-4094>",
        "ping\n"
        "Ping ipv6 address\n"
        "Ipv6 address\n"
        "Select source IP address, the default is the IP address of the output interface\n"
        "source-ip-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value <1-128>\n"    
        INTERFACE_STR
        CLI_INTERFACE_VLANIF_STR
        CLI_INTERFACE_VLANIF_VHELP_STR
    )






DEFSH(VTYSH_PING,
        ping6_cmd,
        "ping ipv6 X:X:X:X:X:X:X:X {-a X:X:X:X:X:X:X:X|-c <1-255>|-h <1-255>|-w <1-30>|-s <20-9500>|vpn <1-128>}",
        "ping\n"
        "Ping ipv6\n"
        "Ipv6 address\n"
        "Select source Ipv6 address, the default is the Ipv6 address of the output interface\n"
        "source-ipv6-address\n"       
        "Specify the number of echo requests to be sent, the default is 5\n"
        "count value\n"
        "Specify TTL value for echo requests to be sent, the default is 255\n"
        "ttl value\n"   
        "Timeout in seconds to wait for each reply, the default is 1s\n"
        "time-out value\n"
        "Specify the bytes of payload to be sent, the default is 18bytes\n"
        "payload size\n"
        "Vpn\n"
        "vpn value <1-128>\n"    
        )


DEFSH(VTYSH_PING,
      h3c_ping_cmd,
      "ping {-a A.B.C.D|-c <1-4294967295>|-f |-h <1-255> |-m <1-65535> |-p <0-FFFFFFFF> |-q |-r |-s <20-8100> |-t <0-65535> |-tos <0-255> |-vpn-instance STRING<1-31>} A.B.C.D",
      "Ping function\n"
      "Specify the source IP address\n"
      "Source IP address\n"
      "Specify the number of echo requests\n"
      "Number of echo requests\n"
      "Specify packets not to be fragmented\n"
      "Specify the TTL value\n"
      "TTL value\n"
      "Specify the interval for sending echo requests\n"
      "No more than 8 ""pad"" hexadecimal characters to fill out the sent packet. For example, -p f2 will fill the sent packet with 000000f2 repeatedly\n"
      "Display only summary\n"
      "Record route. Include the RECORD_ROUTE option in the ECHO_REQUEST packets and display the route\n"
      "Specify the payload length\n"
      "Payload length in bytes\n"
      "Specify the wait time for each reply\n"
      "Wait time in milliseconds"
      "Specify the TOS value\n"
      "Specify a VPN instance\n"
      "VPN instance name\n"
      "IP address\n"
     )

DEFSH(VTYSH_PING,
      h3c_ping_sb_ip_cmd,
      "ping ip {-a A.B.C.D|-c <1-4294967295>|-f |-h <1-255> |-m <1-65535> |-p <0-FFFFFFFF> |-q |-r |-s <20-8100> |-t <0-65535> |-tos <0-255> |-vpn-instance STRING<1-31>} A.B.C.D",
      "Ping function\n"
      "IP information\n"
      "Specify the source IP address\n"
      "Source IP address\n"
      "Specify the number of echo requests\n"
      "Number of echo requests\n"
      "Specify packets not to be fragmented\n"
      "Specify the TTL value\n"
      "TTL value\n"
      "Specify the interval for sending echo requests\n"
      "No more than 8 ""pad"" hexadecimal characters to fill out the sent packet. For example, -p f2 will fill the sent packet with 000000f2 repeatedly\n"
      "Display only summary\n"
      "Record route. Include the RECORD_ROUTE option in the ECHO_REQUEST packets and display the route\n"
      "Specify the payload length\n"
      "Payload length in bytes\n"
      "Specify the wait time for each reply\n"
      "Wait time in milliseconds"
      "Specify the TOS value\n"
      "Specify a VPN instance\n"
      "VPN instance name\n"
      "IP address\n"
     )

DEFSH(VTYSH_PING,
      h3c_trace_cmd,
      //"tracert {-a A.B.C.D |-f <1-255> |-m <1-255> |-p <1-65535> |-q <1-65535> |-t <0-255> |-w <1-65535>|-vpn-instance STRING<1-31>[-resolve-as (global|none|vpn)]} A.B.C.D",
      "tracert {-a A.B.C.D |-f <1-255> |-m <1-255> |-p <1-65535> |-q <1-65535> |-t <0-255> |-w <1-65535>|-vpn-instance STRING<1-31>} A.B.C.D",
      "Tracert function\n"
      "Specify the source IP address used by TRACERT\n"
      "Source IP address\n"
      "Specify the TTL value for the first packet\n"
      "TTL value for the first packet\n"
      "Specify the maximum TTL value\n"
      "Maximum TTL value\n"
      "Specify the destination UDP port number\n"
      "Destination UDP port number\n"
      "Specify the number of probe packets sent each time\n"
      "Number of probe packets\n"
      "Set the Type of Service (ToS) value\n"
      "ToS value\n"
      "Set the timeout to wait for each reply\n"
      "Timeout in milliseconds\n"
      "IP address or hostname of the destination device\n"
    )

/* debug ÈÕÖ¾·¢ËÍµ½ syslog Ê¹ÄÜ×´Ì¬ÉèÖÃ */
DEFSH (VTYSH_PING,pingd_log_level_ctl_cmd_vtysh, "debug pingd(enable | disable)",
	"Output log of debug level\n""Program name\n""Enable\n""Disable\n")

/* debug ÈÕÖ¾·¢ËÍµ½ syslog Ê¹ÄÜ×´Ì¬ÏÔÊ¾ */
DEFSH (VTYSH_PING,pingd_show_log_level_ctl_cmd_vtysh,	"show debug pingd",
	SHOW_STR"Output log of debug level\n""Program name\n")



void
vtysh_init_ping_cmd ()
{
	install_element_level (CONFIG_NODE, &ping_cmd, VISIT_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &tracert_cmd, VISIT_LEVE, CMD_LOCAL);

    install_element_level (CONFIG_NODE, &ping6_cmd, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping6_eth_cmd, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping6_ge_cmd, VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &ping6_xge_cmd, VISIT_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ping6_trunk_cmd, VISIT_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &ping6_vlanif_cmd, VISIT_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &pingd_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);
	install_element_level (CONFIG_NODE, &pingd_show_log_level_ctl_cmd_vtysh, MANAGE_LEVE, CMD_LOCAL);

#if 0
    install_element_level (CONFIG_NODE, &h3c_ping_cmd,VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &h3c_ping_sb_ip_cmd,VISIT_LEVE, CMD_LOCAL);
    install_element_level (CONFIG_NODE, &h3c_trace_cmd,VISIT_LEVE, CMD_LOCAL);
#endif
	
}

