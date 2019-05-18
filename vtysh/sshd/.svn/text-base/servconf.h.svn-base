#ifndef SERVCONF_H
#define SERVCONF_H

#define MAX_PORTS		256	/* Max # ports. */

#define MAX_ALLOW_USERS		256	/* Max # users on allow list. */
#define MAX_DENY_USERS		256	/* Max # users on deny list. */
#define MAX_ALLOW_GROUPS	256	/* Max # groups on allow list. */
#define MAX_DENY_GROUPS		256	/* Max # groups on deny list. */
#define MAX_SUBSYSTEMS		256	/* Max # subsystems. */
#define MAX_HOSTKEYS		256	/* Max # hostkeys. */
#define MAX_ACCEPT_ENV		256	/* Max # of env vars. */
#define MAX_MATCH_GROUPS	256	/* Max # of groups for Match. */

#define DEFAULT_AUTH_FAIL_MAX	6	/* Default for MaxAuthTries */
#define DEFAULT_SESSIONS_MAX	10	/* Default for MaxSessions */

/* Magic name for internal sftp-server */
#define INTERNAL_SFTP_NAME	"internal-sftp"

typedef struct {
	/*侦听的端口号数目*/
	u_int	num_ports;
	
	/*从进程参数中获取的port的标志位*/
	u_int	ports_from_cmdline;	
	/*侦听的端口号数组*/
	int	ports[MAX_PORTS];	/* Port number to listen on. */
	/*侦听的ip地址，默认全部*/
	char   *listen_addr;		/* Address on which the server listens. */
	int     address_family;		/* Address family used by the server. */
	
	/*所有的主机秘钥文件*/
	char   *host_key_files[MAX_HOSTKEYS];	/* Files containing host keys. */

	/*主机秘钥文件的数目*/
	int     num_host_key_files;     /* Number of files for host keys. */
	
	/*客户端验证自己的超时时间（默认为120秒）。 如果客户端超时且未能对
	用户进行身份验证，则服务器将断开连接并退出。 0表示没有限制。*/
	int     login_grace_time;	/* Disconnect if no auth in this time
					 * (sec). */
					 	
	/*指定重新生成临时协议版本1服务器密钥的频率（默认为3600秒或1小时）。*/
	int     key_regeneration_time;	/* Server key lifetime (seconds). */

	/*每个访问过的公钥都记录在./ssh/known_hosts中，下次访问相同计算机时，ssh会
	核对公钥。是否忽略known_hosts*/
	int     ignore_user_known_hosts;	/* Ignore ~/.ssh/known_hosts
						 * for RhostsRsaAuth */
	/*是否打印/etc/motd下当天的信息*/
	int     print_motd;	/* If true, print /etc/motd. */

/*所支持的SSH2密码*/
	char   *ciphers;	/* Supported SSH2 ciphers. */
	char   *macs;		/* Supported SSH2 macs. */
	int	protocol;	/* Supported protocol versions. */

	int			pubkey_authentication;
	int     password_authentication;	/* If true, permit password
						 * authentication. */
	int     compression;	/* If true, compression is allowed */
	u_int num_allow_users;
	char   *allow_users[MAX_ALLOW_USERS];
	u_int num_deny_users;
	char   *deny_users[MAX_DENY_USERS];
	u_int num_allow_groups;
	char   *allow_groups[MAX_ALLOW_GROUPS];
	u_int num_deny_groups;
	char   *deny_groups[MAX_DENY_GROUPS];

	u_int num_subsystems;
	char   *subsystem_name[MAX_SUBSYSTEMS];
	char   *subsystem_command[MAX_SUBSYSTEMS];
	char   *subsystem_args[MAX_SUBSYSTEMS];

	int	max_startups;
	int	max_authtries;
	int	max_sessions;
	char   *banner;			/* SSH-2 banner message */
	int	client_alive_count_max;	/*
					 * If the client is unresponsive
					 * for this many intervals above,
					 * disconnect the session
					 */

	char   *authorized_keys_file;	/* File containing public keys */
	char   *authorized_keys_file2;

	char   *chroot_directory;
}       ServerOptions;

void	 initialize_server_options(ServerOptions *);
void	 fill_default_server_options(ServerOptions *);

#endif				/* SERVCONF_H */

