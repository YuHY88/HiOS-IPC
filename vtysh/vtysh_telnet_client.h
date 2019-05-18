
#ifndef VTYSH_TELNET_CLIENT_H
#define VTYSH_TELNET_CLIENT_H



#define TELENT_CONNECT_WAIT_TIME    5     /* 发送数据给对端后，等待应答超时时间*/

#define SERVER_PKT_LEN              1500   /* server端发来的最大包长 */
#define CMD_BUF_LEN                 512
#define TELNET_PORT                 23


#define TELNET_CLIENT_MAX    (USER_LOGIN_MAX + 1)    /* telnet client 登录最大个数*/

/*telnet 到server端 的状态*/
#define TELNET_SERVER_START     0
#define TELNET_SERVER_SUCCESS   1
#define TELNET_SERVER_FAIL      -1

/*CLIENT 登录通道是否占用， 一共 TELNET_CLIENT_MAX 个 */
#define CLIENT_OCCUPY      1
#define CLIENT_IDLE         0

/* 对端登录状态*/
#define SERVER_NONE                 0
#define SERVER_CONNECT_EXIT         1

struct telnet_client
{
    int32_t     state;
    int32_t     server_state;   //作为server端被登录，server状态，是否退出
    int32_t     connect;
    uint32_t    sip;
    uint32_t    dip;
    uint32_t    sport;
    uint32_t    dport;    
    uint16_t    vpn;
    uint16_t    n;
    uint32_t    len;
    uint32_t    user_input;
    uint32_t    from_server;
    uint8_t *recv_buf; 
    struct login_session *ss; // server session
};


void telnelc_debug_show(struct vty *vty);
struct telnet_client *telnetc_match(uint32_t sip, uint32_t dip,  uint32_t sport);
void telnetc_init(void);
int32_t telnetc_telnet_host(struct vty *vty, uint32_t sip, uint32_t dip, uint32_t dport, uint16_t vpn);
int32_t telnetc_recv_from_server(struct login_session *session, const uint8_t *buf, uint32_t len);
int32_t telnetc_send_ctrlc(struct telnet_client *tc);
void telnetc_server_exit(uint32_t sip, uint32_t dip, uint32_t sport);
int32_t telnetc_send_server(struct telnet_client *tc, uint8_t *buf, uint32_t len);
int32_t telnelc_debug_console_exit(void);




#endif



