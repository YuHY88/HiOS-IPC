
//#include "includes.h"
#include <sys/types.h>
//#include <sys/socket.h>

//#include <netdb.h>
//#include <pwd.h>
//#include <stdio.h>
//#include <stdlib.h>
//#include <string.h>
//#include <signal.h>
#include <unistd.h>
//#include <stdarg.h>
//#include <errno.h>

//#include "ssh.h"
//#include "buffer.h"
#include "servconf.h"
//#include "compat.h"
#include "pathnames.h"
//#include "misc.h"
//#include "cipher.h"
#include "key.h"
#include "kex.h"
//#include "mac.h"
//#include "match.h"
//#include "channels.h"
#include "ssh_version.h"
#include <ftm/pkt_tcp.h>
#include <string.h>

void
initialize_server_options(ServerOptions *options)
{
	memset(options, 0, sizeof(*options));

	/* Standard Options */
	options->num_ports = 0;
	options->num_host_key_files = 0;
	options->login_grace_time = -1;
	options->key_regeneration_time = -1;
	options->print_motd = -1;
	options->pubkey_authentication = -1;
	options->password_authentication = -1;
	options->compression = -1;
	options->num_allow_users = 0;
	options->num_deny_users = 0;
	options->num_allow_groups = 0;
	options->num_deny_groups = 0;
	options->ciphers = NULL;
	options->macs = NULL;
	options->protocol = SSH_PROTO_UNKNOWN;
	options->max_startups = -1;
	options->max_authtries = -1;
	options->max_sessions = -1;
	options->banner = NULL;
	options->client_alive_count_max = -1;
	options->authorized_keys_file = NULL;
	options->authorized_keys_file2 = NULL;
	options->chroot_directory = NULL;
}

void
fill_default_server_options(ServerOptions *options)
{
	/* Standard Options */
	if (options->protocol == SSH_PROTO_UNKNOWN)
		options->protocol = SSH_PROTO_2;
	if (options->num_host_key_files == 0) {
		/* fill default hostkeys for protocols */
		if (options->protocol & SSH_PROTO_2) {
			options->host_key_files[options->num_host_key_files++] =
			    _PATH_HOST_RSA_KEY_FILE;
			options->host_key_files[options->num_host_key_files++] =
			    _PATH_HOST_DSA_KEY_FILE;
//printf("DEFULT:host_key_files:%s\n\t%s\n",options->host_key_files[options->num_host_key_files++],
//options->host_key_files[options->num_host_key_files++]);
		}
	}
	if (options->num_ports == 0)
		options->ports[options->num_ports++] = TCP_PORT_SSH;
	if (options->login_grace_time == -1)
		options->login_grace_time = 120;
	if (options->key_regeneration_time == -1)
		options->key_regeneration_time = 3600;
	if (options->ignore_user_known_hosts == -1)
		options->ignore_user_known_hosts = 0;
	if (options->print_motd == -1)
		options->print_motd = 1;
	if (options->pubkey_authentication == -1)
		options->pubkey_authentication = 1;
	if (options->password_authentication == -1)
		options->password_authentication = 1;
	if (options->compression == -1)
		options->compression = COMP_DELAYED;
	if (options->max_startups == -1)
		options->max_startups = 10;
	if (options->max_authtries == -1)
		options->max_authtries = DEFAULT_AUTH_FAIL_MAX;
	if (options->max_sessions == -1)
		options->max_sessions = DEFAULT_SESSIONS_MAX;
	if (options->client_alive_count_max == -1)
		options->client_alive_count_max = 3;
	if (options->authorized_keys_file2 == NULL) {
		/* authorized_keys_file2 falls back to authorized_keys_file */
		if (options->authorized_keys_file != NULL)
			options->authorized_keys_file2 = options->authorized_keys_file;
		else
			options->authorized_keys_file2 = _PATH_SSH_USER_PERMITTED_KEYS2;
	}
	if (options->authorized_keys_file == NULL)
		options->authorized_keys_file = _PATH_SSH_USER_PERMITTED_KEYS;
}


