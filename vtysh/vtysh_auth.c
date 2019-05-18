#include <assert.h>
#include <lib/types.h>
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/msg_ipc.h>
#include <lib/aaa_common.h>
#include <lib/linklist.h>
#include "msg_ipc_n.h"
#include "vtysh.h"
#include "vtysh_auth.h"
#include "vtysh_session.h"
#include "vty.h"
#include "vtysh_user.h"
#include <string.h>
#include "vtysh_telnet.h"
#include "sshd/channels.h"


//vty后台用户登录标识
int gVtyLocalUserLogin = 0;


/* Authentication of vty */
int
vty_auth(struct vty *vty, char *buf)
{
    char *crypt(const char *, const char *);
    struct login_session *session;
    int ret = 0;

    session = (struct login_session *)vty->session;
    
	VTY_DEBUG(VD_AUTH, "node %d", vty->node);

	if(session->ul->auth_mode == AUTH_MODE_PASSWORD)
    {
		if(strcmp(session->ul->password, buf))
		{
			session->ul->pwd_err++;
		}
		else
		{
			vty->node = CONFIG_NODE;
			session->ul->pwd_err = 0;
			vty->privilege = 14;  
			session->ul->idle_cnt_en = USER_LINE_IDLE_EN;
			vtysh_user_login_trap(session, 1);
		}
		
		if(session->session_type == SESSION_CONSOLE)
		{
			vty_out(vty, "%s", VTY_NEWLINE);
		}
		
		return 0;
    }
    /* every auth we call vty_auth for two times, first time we record the username
     * user input, second time we record password user input and auth*/
    if(vty->host_auth == 0)
    {
        memset(session->name_buf, 0, sizeof(session->name_buf));

        if(strlen(buf) == 0)
        {
            vty->host_auth = 0;
            session->auth_prompt = 1;
            return 0;
        }

        if(strlen(buf) > USER_NAME_MAX)
        {
            vty_out(vty, " %%Username length <1-%d>%s",USER_NAME_MAX, VTY_NEWLINE);
            vty->host_auth = 0;
            session->auth_prompt = 1;
            return 0;
        }

        strcpy(session->name_buf, buf);
        vty->host_auth = 1;
        session->auth_prompt = 0;
        return 0;
    }

    if(strlen(buf) > USER_PASSWORD_MAX)
    {
        vty_out(vty, " %%Password length <1-63>%s", VTY_NEWLINE);
        vty->host_auth = 0;
        session->auth_prompt = 1;
        return 0;
    }

    /*password for second input*/
    memset(session->password_buf, 0, sizeof(session->password_buf));


    strcpy(session->password_buf, buf);


    zlog_debug(VD_AUTH, "session->name_buf:%s session->password_buf:%s\n", session->name_buf, session->password_buf);
    /*send user info to aaa and wait for replay*/

	if(0 == strcmp(session->name_buf, DEF_VTY_USER_NAME))
	{
		ret = vty_auth_local(session);
	}
	else
	{	
    	ret = vty_auth_remote(session);
	}

    vtysh_user_login_trap(session, 1);
    session->ul->idle_cnt_en = USER_LINE_IDLE_EN;
    
    return ret;

}

/*sucess: return 1*
 *failed: return 0*/
int vty_auth_remote(struct login_session *session)
{
    int ret, return_no;
    struct vty *vty;
    vty = session->vty;
//  struct listnode *node, *nnode;
//  struct login_session *session_loop;

	if(session->ul->iscfg == 1 && session->ul->auth_mode != AUTH_MODE_SCHEME)
	{
		return 0;
	}
	
    VTY_DEBUG(VD_AUTH,"info aaa login");
    /* send user info, and wait for replay
     * if :time_out or auth ipc err
     * else :recv finish */
    if((ret = vty_remote_aaa(session)))
    {
        vty->host_auth = 0;
        session->auth_prompt = 1;

        if(ret == ERRNO_TIMEOUT)
        {
            vty_out(vty, " %%User auth timeout, try again%s", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " %%Login incorrect.%s", VTY_NEWLINE);
        }

		vty->fail++;
        vty->host_auth = 0;
        session->auth_prompt = 1;

        if(vty->fail >= 3)
        {
            if(!vty_shell(vty))
            {
                vty_out(vty, " %%Bad passwords, too many failures!%s", VTY_NEWLINE);
                session->session_status = SESSION_CLOSE;
            }
        }

        return_no = FALSE;
    }
    else
    {
        /* if :auth success
         * else :failed*/
        AAA_LOGIN_RESPONSE *aaa_response = (AAA_LOGIN_RESPONSE *)session->aaa_replay;

//      printf("result:%d\n", aaa_response->auth_result);
//      printf("user:%s user-id:%d result:%d\n", aaa_response->username, aaa_response->user_id,aaa_response->auth_result);
        if(aaa_response->auth_result == LOGIN_SUCCESS)
        {
//          struct host_users *user;
            vty->fail = 1;
            vty->host_auth = 0;
            session->auth_prompt = 1;
            vty->node = CONFIG_NODE;    /* Success ! */
            session->user_name = XSTRDUP(MTYPE_VTY, aaa_response->username);
            session->user_id = aaa_response->user_id;
            vty->privilege = aaa_response->level;

			vtysh_execute_no_pager ("end", vty);
            VTY_DEBUG(VD_AUTH, "telnet auth:name = %s, id = %x \n", session->user_name, session->user_id);
            vty_out(vty, "%s %%User %s log in success!%s", VTY_NEWLINE, session->name_buf, VTY_NEWLINE);

            vty_out(vty, "%s", VTY_NEWLINE);

            if(vty_shell(vty))
            {
                vty_out(vty, "%s", VTY_NEWLINE);
            }

            return_no = TRUE;
        }
        else
        {
            switch(aaa_response->auth_result)
            {
                case LOGIN_FAILED:
                    vty_out(vty, " %%User auth failed%s", VTY_NEWLINE);
                    break;

                case LOGIN_TIMEOUT:
                    vty_out(vty, " %%User auth timeout, try again%s", VTY_NEWLINE);
                    break;

                case LOGIN_REPEATED:
                    vty_out(vty, " %%User auth repeated, try again%s", VTY_NEWLINE);
                    break;

                case LOGIN_MAX:
                    vty_out(vty, " %%User auth reach max num%s", VTY_NEWLINE);
                    break;

                default:
                    break;
            }

#if 0

            switch(aaa_response->auth_result)
            {
                case LOGIN_FAILED:
                    vty_out(vty, " %%User auth failed%s", VTY_NEWLINE);
                    break;

                case LOGIN_TIMEOUT:
                    vty_out(vty, " %%User auth timeout, try again%s", VTY_NEWLINE);
                    break;

                default:
                    break;
            }

#endif
            vty->fail++;
//          vty_out(vty, "Login incorrect.%s", VTY_NEWLINE);
            vty->host_auth = 0;
            session->auth_prompt = 1;

            if(vty->fail >= 3)
            {
                if(!vty_shell(vty))
                {
                    vty_out(vty, " %%Bad passwords, too many failures!%s", VTY_NEWLINE);
                    session->session_status = SESSION_CLOSE;
                }
            }
        }

        return_no = FALSE;
    }

    pthread_mutex_lock(&session->aaa_replay_lock);
	if(session->aaa_replay)
	{
    	free(session->aaa_replay);
   		session->aaa_replay = NULL;
	}
    pthread_mutex_unlock(&session->aaa_replay_lock);

    return return_no;
}


//vty 本地认证
/*sucess: return 1*
 *failed: return 0*/
int vty_auth_local(struct login_session *session)
{
    int ret, return_no;
    struct vty *vty;
    vty = session->vty;
//  struct listnode *node, *nnode;
//  struct login_session *session_loop;

	if(session->ul->iscfg == 1 && session->ul->auth_mode != AUTH_MODE_SCHEME)
	{
		return FALSE;
	}
	
    /* if :auth success
     * else :failed*/

//      printf("result:%d\n", aaa_response->auth_result);
//      printf("user:%s user-id:%d result:%d\n", aaa_response->username, aaa_response->user_id,aaa_response->auth_result);
    if(0 == gVtyLocalUserLogin &&\
	   0 == strcmp(session->name_buf, DEF_VTY_USER_NAME) &&\
	   0 == strcmp(session->password_buf, DEF_VTY_USER_PASSWORD))
    {
//          struct host_users *user;
        vty->fail = 1;
        vty->host_auth = 0;
        session->auth_prompt = 1;
        vty->node = CONFIG_NODE;    /* Success ! */
        session->user_name = XSTRDUP(MTYPE_VTY, session->name_buf);
        //session->user_id = aaa_response->user_id;
        vty->privilege = 15;

		vtysh_execute_no_pager ("end", vty);
		
        VTY_DEBUG(VD_AUTH, "telnet auth:name = %s, id = %x \n", session->user_name, session->user_id);
        vty_out(vty, "%s %%User %s log in success!%s", VTY_NEWLINE, session->name_buf, VTY_NEWLINE);

        vty_out(vty, "%s", VTY_NEWLINE);

        if(vty_shell(vty))
        {
            vty_out(vty, "%s", VTY_NEWLINE);
        }

		gVtyLocalUserLogin = 1;

        return_no = TRUE;
    }
    else
    {
		if(gVtyLocalUserLogin)
		{
			vty_out(vty, " %%User auth repeated, try again%s", VTY_NEWLINE);
		}
		else
		{
        	vty_out(vty, " %%User auth failed%s", VTY_NEWLINE);
		}
			
        vty->fail++;
//          vty_out(vty, "Login incorrect.%s", VTY_NEWLINE);
        vty->host_auth = 0;
        session->auth_prompt = 1;

        if(vty->fail >= 3)
        {
            if(!vty_shell(vty))
            {
                vty_out(vty, " %%Bad passwords, too many failures!%s", VTY_NEWLINE);
                session->session_status = SESSION_CLOSE;
            }
        }

		return_no = FALSE;
    }

    return return_no;
}




int
vty_remote_aaa(struct login_session *session)
{
//  static struct ipc_mesg mesg;
//      struct ipc_msghdr *phdr = NULL;
//  AAA_LOGIN_RESPONSE *pinfo;
    static AAA_LOGIN_REQUEST auth_info;
//  int ret;
    time_t last_time;
    struct vty *vty = session->vty;

    static uint32_t user_id_buf = 0;
    struct ipc_mesg_n *pmsg = NULL;
    AAA_VERIFY_RESPONSE *pmsg_recv;
    user_id_buf++;
VTY_DEBUG(VD_AUTH, "name %s, %s", session->name_buf, session->password_buf);
    memset(&auth_info, 0, sizeof(auth_info));
    //auth_info.mode = aaa_authmode;
    memcpy(auth_info.username, session->name_buf, USER_NAME_MAX);
    memcpy(auth_info.password, session->password_buf, USER_PASSWORD_MAX);
    session->user_id_buf = user_id_buf;

    /*need to add*/
    memcpy(auth_info.ttyname, session->address, strlen(session->address));  //where user connect from (char *)
    auth_info.port = session->client_port;  //port
    auth_info.ip = session->client_ip;      //ip
    auth_info.user_id = 0;
    auth_info.user_id_buf = user_id_buf;

    zlog_debug(VD_AUTH, "auth_info.ttyname:%s\n", auth_info.ttyname);
    auth_info.type = LOGIN_REQUEST;


//    AAA_VERIFY_RESPONSE *pmsg_recv = (AAA_VERIFY_RESPONSE *)ipc_send_common_wait_reply(&auth_info, sizeof(AAA_LOGIN_REQUEST), 1,
    pmsg = ipc_sync_send_n2(&auth_info, sizeof(AAA_LOGIN_REQUEST), 1,
                                     MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_VTY, AAA_MSG_AUTH_REQUEST, 0, 0, 2000);

    if(NULL != pmsg)
    {
        pmsg_recv = (AAA_VERIFY_RESPONSE *)pmsg->msg_data;
        if((pmsg_recv->user_id != auth_info.user_id)
                || (0 != strcmp(pmsg_recv->username, auth_info.username))
                || (AAA_MSG_AUTH_REQUEST != pmsg_recv->msg_type))
        {
            if(session->session_type != SESSION_SSH)
            {
                vty_out(vty, "%%User auth-msg send failed, try again%s", VTY_NEWLINE);
            }
        }
        mem_share_free(pmsg, MODULE_ID_VTY);
    }
    else
    {
        VTY_DEBUG(VD_AUTH, "aaa msg null \n");
        if(session->session_type != SESSION_SSH)
        {
            vty_out(vty, "%%User auth-msg send failed, try again%s", VTY_NEWLINE);
        }
    }

    last_time = time(NULL);

    /*wait for aaa replay, and recv. we are recving in an other thread*/
    while(1)
    {
        if(!session->aaa_replay)
        {
            usleep(100000);

            time_t time_curr = time(NULL);

            if(time_cmp_sec(last_time, time_curr, 15))
            {
                if(session->session_type != SESSION_SSH)
                {
                    vty_out(vty, "%%User auth timeout, try again%s", VTY_NEWLINE);
                }

                return ERRNO_TIMEOUT;
            }

            continue;
        }

        /* wait here untile vtysh_handle_aaa_login_response relese
         * the lock*/
        pthread_mutex_lock(&session->aaa_replay_lock);
        pthread_mutex_unlock(&session->aaa_replay_lock);

        return ERRNO_SUCCESS;
    }
}

int vtysh_auth_remote_ssh(struct login_session *session)
{
    int ret = -1;

    if(session->session_type != SESSION_SSH)
    {
        zlog_err("vtysh_auth_remote_ssh not session_ssh\n");
        return 0;
    }

	if(session->ul->iscfg == 1 && session->ul->auth_mode != AUTH_MODE_SCHEME)
	{
		return 0;
	}
	
    VTY_DEBUG(VD_AUTH,"info aaa %s login \n", session->user_name);
    
    ret = vty_remote_aaa(session);
    if(ret)
    {
        return 0;
    }
    else
    {
        /*Success*/
        AAA_LOGIN_RESPONSE *aaa_response = (AAA_LOGIN_RESPONSE *)session->aaa_replay;

        if(aaa_response->auth_result == LOGIN_SUCCESS)
        {
            session->user_name = XSTRDUP(MTYPE_VTY, aaa_response->username);
            session->user_id = aaa_response->user_id;
            VTY_DEBUG(VD_AUTH, "name = %s, id = %x \n", session->user_name, session->user_id);
            session->privilege = aaa_response->level;
            host.login_cnt++;
            session->ul->islogin = USER_LINE_LOGIN_IN;
         	if(session->ul->auth_mode == AUTH_MODE_NONE)
         	{
        		vtysh_user_login_trap(session, 1);
        		session->ul->idle_cnt_en = USER_LINE_IDLE_EN;
         	}
			pthread_mutex_lock(&session->aaa_replay_lock);
			if(session->aaa_replay)
			{
				free(session->aaa_replay);
				session->aaa_replay = NULL;
			}
			pthread_mutex_unlock(&session->aaa_replay_lock);
            return 1;
        }
        else
        {
			pthread_mutex_lock(&session->aaa_replay_lock);
			if(session->aaa_replay)
			{
				free(session->aaa_replay);
				session->aaa_replay = NULL;
			}
			pthread_mutex_unlock(&session->aaa_replay_lock);

			return 0;
        }
    }
}

/* call this fuc in the main thread timer every 5 sec*/
//int vtysh_check_idle_user(struct thread *thread)
int vtysh_check_idle_user(void *para)
{
    struct login_session *session_loop;
    struct listnode *node, *nnode;
    struct vty *vty;
    AAA_ECHO_REQUEST echo_info;
    
    memset(&echo_info, 0, sizeof(AAA_ECHO_REQUEST));

    pthread_mutex_lock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session_loop))
    {
        memset(&echo_info, 0, sizeof(AAA_ECHO_REQUEST));

        if(session_loop->session_type != SESSION_TELNET
                && session_loop->session_type != SESSION_SSH
                && session_loop->session_type != SESSION_CONSOLE)
        {
            continue;
        }
/*  
1. HH命令模式，用户名为NULL(可能登录后又退出)不处理
2. H3命令，scheme认证模式，用户名为NULL(可能登录后又退出)不处理	
3. H3命令，密码认证模式，用户名为NULL(因为没有发给aaa登录信息)， 这时要处理，判断超时，不发给aaa时间信息
*/
        if(!session_loop->user_name && (session_loop->ul->iscfg == -1)) 
        {
            continue;
        }

        vty = session_loop->vty;

        if(vty == NULL)
        {
            zlog_debug(VD_AUTH, "vtysh_check_idle_user vty NULL\n");
            continue;
        }

        session_loop->timer_count++;
        echo_info.idle_time = session_loop->timer_count * CHECK_IDLE_TIME;
        zlog_debug(VD_AUTH, "session_type %d, user_id %d linecfg %d, idle:%d, idleset %d", session_loop->session_type,
                session_loop->user_id, session_loop->ul->iscfg, 
                session_loop->timer_count * CHECK_IDLE_TIME, session_loop->ul->idle_time_set);
        /*busy status, idle_time is 0*/
        if(session_loop->session_status == SESSION_BUSY)
        {
            zlog_debug(VD_AUTH, "vtysh_check_idle_user SESSION_BUSY\n");
            echo_info.idle_time = 0;
        }

		if(session_loop->ul->iscfg == 1) /* H3命令，配置过line命令 */
		{
			if(session_loop->ul->idle_cnt_en == USER_LINE_IDLE_EN && session_loop->ul->idle_time_set != 0 
					&& echo_info.idle_time > session_loop->ul->idle_time_set)
    		{
	            if(session_loop->session_type == SESSION_CONSOLE)
        		{
	                vty_out(vty, "%sUser Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

	                vty_consol_close(session_loop);
        		}
        		else
        		{
                    VTY_DEBUG(VD_AUTH, "idle:%ld, idleset %d",echo_info.idle_time, session_loop->ul->idle_time_set);        		
        		    vty_out(vty, "%s%% User idle and exit%s", VTY_NEWLINE, VTY_NEWLINE);
	                session_loop->session_status = SESSION_CLOSE;
	            }
            } 
            echo_info.idle_time = 0;
        }

        if(session_loop->user_name)
        {
            /* send message to aaa */
            memcpy(echo_info.username, session_loop->user_name,
                   strlen(session_loop->user_name));
            echo_info.user_id = session_loop->user_id;

			if(echo_info.idle_time >= host.idle_max)
			{
				if(0 == strcmp(session_loop->user_name, DEF_VTY_USER_NAME))
				{
					gVtyLocalUserLogin = 0;
				}
				else
				{
					if(ERRNO_SUCCESS != vty_remote_aaa_logout(session_loop))
	                {
	                    vty_out(vty, "User(%s:%x) idle exit failed!!!%s%s", session_loop->user_name, session_loop->user_id, VTY_NEWLINE, VTY_NEWLINE);
	                }
				}

				vty_out(vty, "%s%% User idle and exit%s", VTY_NEWLINE, VTY_NEWLINE);

				if(session_loop->session_type == SESSION_CONSOLE)
	            {
	                //vty_out(vty, "%%User timeout from consol%s%s", VTY_NEWLINE, VTY_NEWLINE);
	                vty_out(vty, "%sUser Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

	                vty_consol_close(session_loop);
	            }
	            else
	            {
	                /*concle口不需要没有close状态*/
	                session_loop->session_status = SESSION_CLOSE;
	            }
			}
			else
			{
				if(0 != strcmp(session_loop->user_name, DEF_VTY_USER_NAME))
				{
					/*send */
		        	zlog_debug(VD_AUTH, "send echo_info.username:%s echo_info.idle_time:%d\n", echo_info.username, (int)echo_info.idle_time);
		//            ipc_send_common(&echo_info, sizeof(echo_info), 1, MODULE_ID_AAA,
		//                            MODULE_ID_VTY, IPC_TYPE_VTY, AAA_MSG_ECHO, 0);
		            if(ipc_send_msg_n2(&echo_info, sizeof(echo_info), 1, MODULE_ID_AAA,
		                            MODULE_ID_VTY, IPC_TYPE_VTY, AAA_MSG_ECHO, 0, 0) < 0)
		            {
		                zlog_err("%s-%d:send msg to aaa fail\n", __FUNCTION__, __LINE__);
		            }
				}
			}

        }                
    }

    pthread_mutex_unlock(&session_delete_lock);

    /* after send, add timer again */

    return 0;
}

#if 0
time_t vty_get_thread_idle_time(struct login_session *session)
{
    struct vty *vty;
    struct thread *timer_thread;
    time_t sec;

    vty = session->vty;
    timer_thread = vty->t_timeout;
//  printf("timer_thread:%x\n", timer_thread);

    time_get_relative_time(NULL);

    zlog_debug(VD_AUTH, "\nrelative_time.tv_sec:%d timer_thread->u.sands.tv_sec:%d vty->v_timeout:%d\n",
               (int)relative_time.tv_sec, (int)timer_thread->u.sands.tv_sec, (int)vty->v_timeout);
    sec = relative_time.tv_sec - (timer_thread->u.sands.tv_sec - (time_t)vty->v_timeout);
//  printf("sec:%d\n", sec);
    return sec;

}
#endif
/* tell aaa if user exit from terminal*/
int vty_remote_aaa_logout(struct login_session *session)
{
//  static struct ipc_mesg mesg;
//  struct ipc_msghdr *phdr = NULL;
    static AAA_LOGIN_REQUEST auth_info;
//  int ret;
//  time_t time_curr, last_time;
    uint8_t subtype = AAA_MSG_AUTH_REQUEST;
    struct vty *vty = session->vty;
    struct ipc_mesg_n *pmsg;
    AAA_VERIFY_RESPONSE *pmsg_recv;    
    int ret = -1;

    if(vty == NULL)
    {
        zlog_err(" %s:vty = NULL, type %d \n", __FUNCTION__, session->session_type);
        return -1;
    }

	if(session->ul->iscfg == 1 && session->ul->auth_mode != AUTH_MODE_SCHEME)
	{
		return 0;
	}

    VTY_DEBUG(VD_AUTH,"info aaa %s logout \n", session->user_name);

    zlog_debug(VD_AUTH, "vty_remote_aaa_logout\n");
    memset(&auth_info, 0, sizeof(auth_info));
    //auth_info.mode = aaa_authmode;
    memcpy(auth_info.username, session->user_name, USER_NAME_MAX);
    memcpy(auth_info.ttyname, vty->ttyname, 20);
    auth_info.type = LOGIN_EXIT;
    auth_info.user_id = session->user_id;


//    AAA_VERIFY_RESPONSE *pmsg_recv = (AAA_VERIFY_RESPONSE *)ipc_send_common_wait_reply(&auth_info, sizeof(AAA_LOGIN_REQUEST), 1,
    pmsg = ipc_sync_send_n2(&auth_info, sizeof(AAA_LOGIN_REQUEST), 1,
                                     MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_VTY, subtype, 0, 0, 1000);
    if(pmsg != NULL)
    {
        pmsg_recv = (AAA_VERIFY_RESPONSE *)pmsg->msg_data; 
        if((pmsg_recv->user_id == auth_info.user_id)
                && (0 == strcmp(pmsg_recv->username, auth_info.username))
                && (subtype == pmsg_recv->msg_type))
        {
            ret = ERRNO_SUCCESS;
        }
        else
        {
            ret = ERRNO_FAIL;
        }
        mem_share_free(pmsg, MODULE_ID_VTY);
    }
    else
    {
        zlog_debug(VD_AUTH, "%s:aaa msg null \n",__FUNCTION__);
        ret = ERRNO_FAIL;
    }
    return ret;
}

/*  */
void vtysh_handle_aaa_max_idle_time(AAA_MAX_IDLE_MSG *pdata)
{
	if(0 == pdata->idle_time)
	{
		host.idle_max = 30*60;
	}
	else
	{
		host.idle_max = pdata->idle_time;
	}
}

/* aaa logout a user and tell vtysh, vtysh end user session here*/
void vtysh_handle_aaa_force_exit(AAA_FORCE_EXIT *pmsg)
{
    struct vty *vty;
    int found = 0;

    zlog_debug(VD_AUTH, "vtysh_handle_aaa_force_exit\n");
    struct login_session *session;
//  struct login_session *session_loop;
    struct listnode *node, *nnode;

    AAA_VERIFY_RESPONSE msg;

    pthread_mutex_lock(&session_delete_lock);

    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session))
    {

//  printf("session->user_id:%d pmsg->user_id:%d\n", session->user_id, pmsg->user_id);
//  printf("session->user_name:%s pmsg->username:%s\n", session->user_name, pmsg->username);
        if(session
                && session->user_name != NULL
                && strcmp(pmsg->username, session->user_name) == 0
                && session->user_id == pmsg->user_id)
        {
            found++;
            vty = session->vty;

            zlog_debug(VD_AUTH, "%s[%d] : user[%s][%x] force-exit\n",
                       __func__, __LINE__, session->user_name, session->user_id);

            switch(pmsg->reason)
            {
                case EXIT_IDLE:
                    vty_out(vty, "%s%% User idle and exit%s", VTY_NEWLINE, VTY_NEWLINE);
                    break;

                case EXIT_ECHO_TIMEOUT:
                    vty_out(vty, "%s%% User echo timeout and exit%s", VTY_NEWLINE, VTY_NEWLINE);
                    break;

                case EXIT_ACCT_FAIL:
                    vty_out(vty, "%s%% User account failed and exit%s", VTY_NEWLINE, VTY_NEWLINE);
                    break;

                default:
                    break;
            }

            memset(&msg, 0, sizeof(AAA_VERIFY_RESPONSE));
            msg.user_id = session->user_id;
            msg.msg_type = AAA_MSG_FORCE_EXIT;

            if(NULL != session->user_name)
            {
                memcpy(msg.username, session->user_name, strlen(session->user_name));
            }

            zlog_debug(VD_AUTH, "%s[%d] : send force-exit verify to aaa\n", __func__, __LINE__);
//            ipc_send_reply(&msg, sizeof(AAA_VERIFY_RESPONSE),
//                           MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_FORCE_EXIT, 0);

            ipc_send_reply_n2(&msg, sizeof(AAA_VERIFY_RESPONSE), 1, 
                           MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_FORCE_EXIT, 0, 0, IPC_OPCODE_REPLY);           
            if(session->session_type == SESSION_CONSOLE)
            {
                //vty_out(vty, "%%User timeout from consol%s%s", VTY_NEWLINE, VTY_NEWLINE);
                vty_out(vty, "%sUser Access Verification%s%s", VTY_NEWLINE, VTY_NEWLINE, VTY_NEWLINE);

                vty_consol_close(session);
            }
            else
            {
                /*concle口不需要没有close状态*/
                session->session_status = SESSION_CLOSE;
            }

            break;
            /*user session going down, do we need to tell aaa?*/
        }
    }

    pthread_mutex_unlock(&session_delete_lock);

    if(!found)
    {
        zlog_debug(VD_AUTH, "%s[%d] : send force-exit verify to aaa\n", __func__, __LINE__);
//        ipc_send_noack(0, MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_FORCE_EXIT, 0);
        ipc_send_reply_n2((void *)0, 4, 1, 
                     MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_FORCE_EXIT, 0, 0, IPC_OPCODE_NACK);        
        zlog_err("%% User:%s id:%d not exist! No user logout.\n",
                 pmsg->username, pmsg->user_id);
    }
}

void vtysh_handle_aaa_login_response(AAA_LOGIN_RESPONSE *pmsg)
{
    struct login_session *session = NULL;
    char *name = pmsg->username;
    uint32_t user_id_buf = pmsg->user_id_buf;
    struct listnode *node, *nnode;
    AAA_VERIFY_RESPONSE msg;

    zlog_debug(VD_AUTH, "vtysh_handle_aaa_login_response, %s\n", pmsg->username);

//  pthread_mutex_lock(&session_delete_lock);
    for(ALL_LIST_ELEMENTS(user_session_list, node, nnode, session))
    {
        zlog_debug(VD_AUTH, "%s[%d] : name[%s <--> %s], user_id_buf[%d <--> %d]\n",
                   __func__, __LINE__, name, session->name_buf, user_id_buf, session->user_id_buf);

        if((strcmp(name, session->name_buf) == 0) && (user_id_buf == session->user_id_buf))
        {
            pthread_mutex_lock(&session->aaa_replay_lock);

            memset(&msg, 0, sizeof(AAA_VERIFY_RESPONSE));
            msg.user_id = pmsg->user_id;
            msg.user_id_buf = session->user_id_buf;
            msg.msg_type = AAA_MSG_AUTH_RESPONSE;

            if(NULL != session->name_buf)
            {
                memcpy(msg.username, session->name_buf, strlen(session->name_buf));
            }

            zlog_debug(VD_AUTH, "%s[%d] : send login-response verify to aaa\n", __func__, __LINE__);
//            ipc_send_reply(&msg, sizeof(AAA_VERIFY_RESPONSE),
//                           MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_AUTH_RESPONSE, 0);

            ipc_send_reply_n2(&msg, sizeof(AAA_VERIFY_RESPONSE), 1, 
                           MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_AUTH_RESPONSE, 0, 0, IPC_OPCODE_REPLY);
            if(!session->aaa_replay)
            {
                session->aaa_replay =
                    (AAA_LOGIN_RESPONSE *)malloc(sizeof(AAA_LOGIN_RESPONSE));
                memset(session->aaa_replay, 0, sizeof(AAA_LOGIN_RESPONSE));
                memcpy(session->aaa_replay, pmsg, sizeof(AAA_LOGIN_RESPONSE));
            }
            else
            {
                zlog_err("aaa_replay is not null\n");
            }

            AAA_LOGIN_RESPONSE *tmp = session->aaa_replay;
            zlog_debug(VD_AUTH, "username:%s user_id:%d result:%d\n", tmp->username, tmp->user_id, tmp->auth_result);
            pthread_mutex_unlock(&session->aaa_replay_lock);
            return;
        }
    }

//  pthread_mutex_unlock(&session_delete_lock);

//    ipc_send_noack(0, MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA,
//                   AAA_MSG_AUTH_RESPONSE, 0);
    ipc_send_reply_n2((void *)0, 4, 1, 
                     MODULE_ID_AAA, MODULE_ID_VTY, IPC_TYPE_AAA, AAA_MSG_AUTH_RESPONSE, 0, 0, IPC_OPCODE_NACK);                   
}

