#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/crc8.h>
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>
#include "stm_if.h"
#include <lib/index.h>
#include <lib/devm_com.h>
#include <lib/msg_ipc_n.h>
#include "ces_if.h"
#include "ces_msg.h"


struct cmd_node sdh_dxc_node =
{ 
    SDH_DXC_NODE,
    "%s(config-sdh-dxc)# ",
    1,
};

DEFUN (sdh_dxc_mode,
        sdh_dxc_mode_cmd,
        "sdhdxc",
        "sdh_dxc command mode\n")
{
        vty->node = SDH_DXC_NODE;

        return CMD_SUCCESS;
}

/**
 * @brief      : 配置物理端口环回
 * @param[in ] : 环回类型，local:接口对内环回； remote:接口对外环回
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : STM 物理接口视图
 *               接口的环回功能处于关闭状态
 */
DEFUN(stm_loopback,
    stm_loopback_cmd,
    "loopback (local | remote)",
    "stm interface loopback\n"
    "stm internal loopback\n"
    "stm external loopback\n")
{
    int ret = 0;
    struct stm_if *pif = NULL;
    uint32_t ifindex = 0;
    uint8_t loopback = STM_LB_NOLOOPBACK;
    
    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (!strncmp(argv[0], "l", 1))
    {
        //loopback = STM_LB_LOCAL;
        vty_error_out(vty, "local is not supported!%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    else if (!strncmp(argv[0], "r", 1))
    {
        loopback = STM_LB_REMOTE;
    }

    ret = stm_set_info(pif, &loopback, STM_INFO_LOOPBACK);
    if(ret != 0)
    {
        vty_error_out(vty, "set loopback failed%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

DEFUN(stm_no_loopback,
    stm_no_loopback_cmd,
    "no loopback",
    NO_STR
    "Interface loopback\n")
{
    int ret = 0;
    struct stm_if *pif = NULL;
    uint32_t ifindex = 0;
    uint8_t loopback = STM_LB_NOLOOPBACK;
    
    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    ret = stm_set_info(pif, &loopback, STM_INFO_LOOPBACK);
    if(ret != 0)
    {
        vty_error_out(vty, "no loopback failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

/**
 * @brief      : 配置SDH开销字节，SONET/SDH帧的再生段踪迹字节。 
 * @param[in ] : j0再生段踪迹字节。SDH帧格式下j0-value的取值范围为1～15个字符的字符串
 * @param[in ] : padding-zero可选，字符补齐方式，填充0，默认填充空格
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : STM 物理接口视图
                 系统使用SDH帧格式的缺省值，SDH帧格式下再生段踪迹字节J0的缺省值为“aaaaaaaaaaaaaaa”
 */
DEFUN(stm_oh_j0,
    stm_oh_j0_cmd,
    "oh j0 sdh J0-VALUE { padding-zero }",
    "Overhead\n"
    "J0\n"
    "Default frame format: SDH\n"
    "Regeneration Section Trace Message j0: 1-15 characters\n"
    "Padding character, space or zero\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint16_t length = 0;
    struct stm_if *pif = NULL;
    char str[STM_OVERHEAD_LEN+1] = "";
    char j0[STM_OVERHEAD_LEN+1] = "";
    uint8_t crc = 0x00;
    int n = 0;

    length = strlen(argv[0]);
    if(length > STM_OVERHEAD_LEN)
    {
        vty_error_out(vty, "ERROR: j0 is too long, maximum length is 15 characters! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(str, argv[0], length);
    n = STM_OVERHEAD_LEN - length;      //需要补齐的位数
    if(length < STM_OVERHEAD_LEN)
    {
        if(argv[1] != NULL)
        {
            memset(str + length, 0x00, n);   //0 补齐
            pif->stm_rs.pad_zero = 1;
        }
        else
        {
            memset(str + length, 0x20, n);   //空格补齐
        }
    }

    //获取crc并将其放在j0的第0位，输入的字符放在后15位
    crc = get_crc8((uint8_t *)str, 15) | 0x80;
    memcpy(j0 + 1, str, STM_OVERHEAD_LEN);
    j0[0] = crc;    
        
    ret = stm_set_info(pif, j0, STM_INFO_J0);
    if(ret != 0)
    {
        vty_error_out(vty, "set j0 failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

DEFUN(stm_no_oh_j0,
    stm_no_oh_j0_cmd,
    "no oh j0",
    NO_STR
    "Overhead\n"
    "J0\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct stm_if *pif = NULL;
    char str[STM_OVERHEAD_LEN+1] = "";
    char j0[STM_OVERHEAD_LEN+1] = "";
    uint8_t crc = 0x00;
    
    memset(str, 'a', 15);
    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    //获取crc并将其放在j0的第0位，输入的字符放在后15位
    crc = get_crc8((uint8_t *)str, 15) | 0x80;
    memcpy(j0 + 1, str, STM_OVERHEAD_LEN);
    j0[0] = crc;    

    //补齐标志位置位
    pif->stm_rs.pad_zero = 0;
    
    ret = stm_set_info(pif, j0, STM_INFO_J0);
    if(ret != 0)
    {
        vty_error_out(vty, "no oh j0 failed%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
        
    return CMD_SUCCESS;
}

/**
 * @brief      : 配置SONET/SDH帧的通道踪迹字节。 
 * @param[in ] : J1通道踪迹字节。SDH帧格式下取值范围为1～15个字符的字符串。
 * @param[in ] : padding-zero可选，字符补齐方式，填充0，默认填充空格
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : STM 物理接口视图
 *               系统使用SDH帧格式的缺省值，SDH帧格式下再生段踪迹字节J0的缺省值为“bbbbbbbbbbbbbbb”
 */
DEFUN(stm_oh_j1,
    stm_oh_j1_cmd,
    "oh j1 sdh J1-VALUE { padding-zero }",
    "Overhead\n"
    "J1\n"
    "Default frame format: SDH\n"
    "Higher-Order VC-N path trace byte j1: 1-15 characters\n"
    "Padding character, space or zero\n")
{
    int ret = 0;
    uint32_t        ifindex = 0;
    uint16_t        length = 0;
    uint16_t        vc4_id = 0;
    struct stm_if   *pif = NULL;
    struct stm_hp_t stm_hp;
    char str[STM_OVERHEAD_LEN+1] = "";
    char j1[STM_OVERHEAD_LEN+1] = "";
    uint8_t crc = 0x00;
    int n = 0;
    
    length = strlen(argv[0]);
    if(length > STM_OVERHEAD_LEN)
    {
        vty_error_out(vty, "ERROR: j1 is too long, maximum length is 15 characters! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    memcpy(str, argv[0], length);
    n = STM_OVERHEAD_LEN - length;      //需要补齐的位数
    if(length < STM_OVERHEAD_LEN)
    {
        if(argv[1] != NULL)
        {
            memset(str + length, 0x00, n);   //0 补齐
            pif->stm_hp_list->pad_zero = 1; //应该一个stm接口下每个vc4都有相应的标志位，暂时只支持vc4_id==1
        }
        else
        {
            memset(str + length, 0x20, n);   //空格补齐
        }
    }
    
    //获取crc并将其放在j0的第0位，输入的字符放在后15位
    crc = get_crc8((uint8_t *)str, 15) | 0x80;
    memcpy(j1 + 1, str, STM_OVERHEAD_LEN);
    j1[0] = crc;    

    vc4_id = 1;     //目前只支持stm-1，后续扩展stm-4/16/64后，可能需要vc4_id可配置
    memset(&stm_hp, 0, sizeof(struct stm_hp_t));
    stm_hp.vc4_id = vc4_id;
    memcpy(stm_hp.hp_info.j1_tx, j1, STM_OVERHEAD_LEN+1);
    
    ret = stm_set_info(pif, &stm_hp, STM_INFO_J1);
    if(ret != 0)
    {
        vty_error_out(vty, "set j1 failed  %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
        
    return CMD_SUCCESS;
}

DEFUN(stm_no_oh_j1,
    stm_no_oh_j1_cmd,
    "no oh j1",
    NO_STR
    "Overhead\n"
    "J0\n")
{
    int         ret = 0;
    uint32_t    ifindex = 0;
    uint16_t    vc4_id = 0;
    struct stm_if   *pif = NULL;
    struct stm_hp_t stm_hp;
    char    str[STM_OVERHEAD_LEN+1] = "";
    char    j1[STM_OVERHEAD_LEN+1] = "";
    uint8_t crc = 0x00;
    
    memset(str, 'b', 15);
    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    //获取crc并将其放在j0的第0位，输入的字符放在后15位
    crc = get_crc8((uint8_t *)str, 15) | 0x80;
    memcpy(j1 + 1, str, STM_OVERHEAD_LEN);
    j1[0] = crc;    
    
    vc4_id = 1;     //目前只支持stm-1，后续扩展stm-4/16/64后，可能需要vc4_id可配置
    memset(&stm_hp, 0, sizeof(struct stm_hp_t));
    stm_hp.vc4_id = vc4_id;
    memcpy(stm_hp.hp_info.j1_tx, j1, STM_OVERHEAD_LEN+1);
    
    //补齐标志位置位
    pif->stm_hp_list->pad_zero = 0;

    ret = stm_set_info(pif, &stm_hp, STM_INFO_J1);
    if(ret != 0)
    {
        vty_error_out(vty, "no oh j1 failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
        
    return CMD_SUCCESS;
}

#define STR_IS_DIGITAL(c) ((((c) >= '0') && ((c) <= '9')) || \
                                (((c) >= 'a') && ((c) <= 'f')) || \
                                (((c) >= 'A') && ((c) <= 'F')))

int c2_to_hex(char *str, uchar *hexbuf)
{
	uchar *p = NULL;
	uchar ch, cl;
    int len = strlen(str);
    
	if(!str) 
    {
		return -1;
	}

    if(len > 2 || len < 1)
    {
        return -1;
    }
    
	p = (uchar *)str;
    if(len == 1)
    {
        ch = '0';
        cl = *p;
    }
    else
    {
	    ch = *p++;
	    cl = *p++;
    }
    
    if(!STR_IS_DIGITAL(ch) || !STR_IS_DIGITAL(cl))
    {
        return -1;
    }
    
	if(!cl) 
    {
        return -1;
	}
    
	if(isupper(ch)) ch = tolower(ch);
	if(isupper(cl)) cl = tolower(cl);
    
    if((ch >= '0') && (ch <= '9')) 
    {
        ch -= '0';
    }
    else if ((ch >= 'a') && (ch <= 'f')) 
    {
        ch -= 'a' - 10;
    }
    
    if((cl >= '0') && (cl <= '9')) 
    {
        cl -= '0';
    }
    else if ((cl >= 'a') && (cl <= 'f')) 
    {
        cl -= 'a' - 10;
    }

	*hexbuf = (ch << 4) | cl;

	return 0;
}

/**
 * @brief      : 配置信号标记字节C2。
 * @param[in ] : 信号标记字节C2，整数形式，取值范围为0x00～0xFF
 * @param[out] :
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : STM 物理接口视图
 *               信号标记字节C2的值为0x02
 */
DEFUN(stm_oh_c2,
    stm_oh_c2_cmd,
    "oh c2 C2_VALUE",
    "Overhead\n"
    "Path signal label byte\n"
    "Value <00-FF>\n")
{
    int ret = 0;
    uint32_t    ifindex = 0;
    uint16_t    vc4_id = 0;
    uint8_t     c2 = 0;
    struct stm_if *pif = NULL;
    struct stm_hp_t stm_hp;

    ret = c2_to_hex((char *)argv[0], &c2);
    if(ret != 0)
    {
        vty_error_out(vty, "Input value is illegal! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vc4_id = 1;     //目前只支持stm-1，后续扩展stm-4/16/64后，可能需要vc4_id可配置
    memset(&stm_hp, 0, sizeof(struct stm_hp_t));
    stm_hp.vc4_id = vc4_id;
    stm_hp.hp_info.c2_tx = c2;

    ret = stm_set_info(pif, &stm_hp, STM_INFO_C2);
    if(ret != 0)
    {
        vty_error_out(vty, "set oh c2 failed%s", VTY_NEWLINE);
        return CMD_WARNING;
    }
        
    return CMD_SUCCESS;
}

/*default c2 is 0x02*/
DEFUN(stm_no_oh_c2,
    stm_no_oh_c2_cmd,
    "no oh c2",
    NO_STR
    "Overhead\n"
    "Path signal label byte\n")
{
    int ret = 0;
    uint32_t    ifindex = 0;
    uint16_t    vc4_id = 0;
    uint8_t     c2 = 0x02;          //default c2 is 0x02
    struct stm_if   *pif = NULL;
    struct stm_hp_t stm_hp;

    ifindex = (uint32_t)vty->index;
    pif = stm_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    vc4_id = 1;     //目前只支持stm-1，后续扩展stm-4/16/64后，可能需要vc4_id可配置
    memset(&stm_hp, 0, sizeof(struct stm_hp_t));
    stm_hp.vc4_id = vc4_id;
    stm_hp.hp_info.c2_tx = c2;

    ret = stm_set_info(pif, &stm_hp, STM_INFO_C2);
    if(ret != 0)
    {
        vty_error_out(vty, "no oh c2 failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

/**
 * @brief      : 配置STM-1/STM-4/STM-16与E1交叉映射关系。
 * @param[in ] : vc4:  高阶通道编码，范围1-16
 * @param[in ] : vc12: 低阶通道编码，范围1-63
 * @param[in ] : tdm:  E1接口，USP <0-7>/<0-31>/<1-255>
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : STM 物理接口视图
 */
DEFUN(stm_map_to_tdm,
    stm_map_to_tdm_cmd,
    "vc4 <1-16> vc12 <1-63> tdm interface USP",
    "Higher-Order path VC-4 ID\n"
    "Value of VC-4: <1-16>\n"
    "Higher-Order path VC-12 ID\n"
    "Value of VC-12: <1-63>\n"
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_VHELP_STR)
{
    return CMD_SUCCESS;
}

DEFUN(stm_no_map_to_tdm,
    stm_no_map_to_tdm_cmd,
    "no vc4 <1-16> vc12 <1-63> tdm interface USP",
    NO_STR
    "Higher-Order path VC-4 ID\n"
    "Value of VC-4: <1-16>\n"
    "Higher-Order path VC-12 ID\n"
    "Value of VC-12: <1-63>\n"
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_VHELP_STR)
{
    return CMD_SUCCESS;
}

/**
 * @brief      : 配置msp 1+1保护组，指定主端口和备份端口。
 * @param[in ] : <1-32> msp保护组编号
 * @param[in ] : master/backup interface 主接口/备份接口
 * @param[in ] : opo: One plus one保护，默认此方式，1+1保护。 
 * @param[in ] : oto: One to one保护，1:1保护
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : config 接口视图
 *               msp保护仅支持1+1(默认)保护模式
 */
DEFUN(stm_msp_group,
    stm_msp_group_cmd,
    "msp-group <1-32> master interface stm USP backup interface stm USP { mode (opo|oto) }",
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "interface role: master\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "interface role: backup\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "msp mode\n"
    "one plus one mode\n"
    "one to one mode\n")
{
    int ret = 0, i = 0;
	uint8_t tdm_port = 0;
    uint8_t m_port = 0;
    uint8_t b_port = 0;
    uint8_t  msp_index = 0;
    uint32_t m_ifindex = 0;
    uint32_t b_ifindex = 0;
    struct stm_if *pif_master = NULL;
    struct stm_if *pif_backup = NULL;
    struct stm_msp_config msp_info;
    struct stm_msp_config *pmsp = NULL;
	struct stm_dxc_config *dxc = NULL;
    struct listnode       *pnode = NULL;

    memset(&msp_info, 0, sizeof(struct stm_msp_config));
    msp_index = (uint8_t)atoi(argv[0]);
    pmsp = stm_msp_lookup_by_mspid(msp_index);
    if(pmsp != NULL)
    {
        vty_error_out(vty, "msp group %d has been configured! %s", msp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    m_ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[1] );
    pif_master = stm_if_lookup(m_ifindex);
    if (pif_master == NULL)
    {
        vty_error_out(vty, "This master interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    pmsp = stm_msp_lookup(m_ifindex);
    if(pmsp != NULL)
    {
        vty_error_out(vty, "This master interface has existed in msp group %d! %s", pmsp->msp_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
	dxc = stm_dxc_index_lookup(m_ifindex);
	if(dxc != NULL)
    {
        vty_error_out(vty, "The DXC protection group has been configured！ %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    b_ifindex = ifm_get_ifindex_by_name ( "stm", ( char * ) argv[2] );
    pif_backup = stm_if_lookup(b_ifindex);
    if (pif_backup == NULL)
    {
        vty_error_out(vty, "This backup interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    pmsp = stm_msp_lookup(b_ifindex);
    if(pmsp != NULL)
    {
        vty_error_out(vty, "This backup interface has existed in msp group %d! %s", pmsp->msp_id, VTY_NEWLINE);
        return CMD_WARNING;
    }
	dxc = stm_dxc_index_lookup(b_ifindex);
	if(dxc != NULL)
    {
        vty_error_out(vty, "The DXC protection group has been configured！ %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    if(IFM_SLOT_ID_GET(m_ifindex) != IFM_SLOT_ID_GET(b_ifindex))
    {
        vty_error_out(vty, "The MSP can not be configured with different slot! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    //保护组暂时规定为 1和2、3和4分别互为保护，不能配置类似1和3互为保护
    m_port = IFM_PORT_ID_GET(m_ifindex);
    b_port = IFM_PORT_ID_GET(b_ifindex);
    if(( m_port == 1 && b_port != 2) || ( m_port == 2 && b_port != 1))
    {
        if ( m_port == 1)
        {
            vty_error_out(vty, "The backup port number must be 2 when master port is 1! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {
            vty_error_out(vty, "The backup port number must be 1 when master port is 2! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }

    if(( m_port == 3 && b_port != 4) || ( m_port == 4 && b_port != 3))
    {
        if ( m_port == 3)
        {
            vty_error_out(vty, "The backup port number must be 4 when master port is 3! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
        else
        {
            vty_error_out(vty, "The backup port number must be 3 when master port is 4! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
	for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, dxc))
	{	
    	if(dxc != NULL)
    	{
    		if(!IFM_TYPE_IS_STM(dxc->dst_if.ifindex))
    		{
				tdm_port = IFM_PORT_ID_GET ( dxc->dst_if.ifindex);
				if((m_port==1)||(m_port==2))
				{
					for(i=tdm_port;i<(tdm_port+dxc->num);i++)
					{
						if((i>0)&&(i<64))
						{
							vty_error_out(vty, "DXC has occupied the TDM <1-63>port and is not allowed to configure MSP! %s",  VTY_NEWLINE);
		       				return CMD_WARNING;
						}	
					}
				}
				else
				{
					for(i=tdm_port;i<(tdm_port+dxc->num);i++)
					{
						if((i>=64)&&(i<=126))
						{
							vty_error_out(vty, "DXC has occupied the TDM <64-126>port and is not allowed to configure MSP! %s",  VTY_NEWLINE);
		       				return CMD_WARNING;
						}
					}
				}	
    		}
		}
	}

    //保护模式可选，默认1+1
    if(argv[3] != NULL)
    {
        if(strncmp(argv[3], "opo", 3) == 0)
        {
            msp_info.mode = MSP_MODE_OPO;
        }
        else if(strncmp(argv[3], "oto", 3) == 0)    //暂时不支持1:1模式
        {
            //msp_info.mode = MSP_MODE_OTO;
            vty_error_out(vty, "One to one mode is not supported! %s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        msp_info.set_flag = 1;
    }
    else
    {
        msp_info.mode = MSP_MODE_OPO;
    }

    msp_info.msp_id     = msp_index;
    msp_info.master_if  = m_ifindex;
    msp_info.backup_if  = b_ifindex;
    msp_info.action     = STM_MSP_ENABLE;
    msp_info.wtr        = 1;    //默认1分钟
    
    ret = stm_msp_add(&msp_info, msp_index);
    if(ret != 0)
    {
        vty_error_out(vty, "msp group add to list error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    zlog_debug(CES_DEBUG_STM, "\r\n%s:%d, msp_index=%d, master_if=%x, backup_if=%x\r\n", __FUNCTION__,__LINE__, msp_index, m_ifindex, b_ifindex);
    return CMD_SUCCESS;
}

DEFUN(stm_no_msp_group,
    stm_no_msp_group_cmd,
    "no msp-group <1-32>",
    NO_STR
    "msp gourp\n"
    "Value of msp gourp ID: <1-32>\n")
{
    int ret = 0;
    uint8_t msp_index = 0;

    msp_index = (uint8_t)atoi(argv[0]);
    ret = stm_msp_delete(msp_index);
    if(ret != 0)
    {
        vty_error_out(vty, "delete msp group error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

/**
 * @brief      : 配置msp保护组的备份回切时间。
 * @param[in ] : <1-32> msp保护组编号
 * @param[in ] : wtr 回切时间。单位为s，颗粒度为6s，实际生效的配置为对6取整后的值。
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : config 接口视图
 *               6s。
 */
DEFUN(stm_msp_group_wtr,
    stm_msp_group_wtr_cmd,
    "msp-group <1-32> backup failback wtr <1-12>",
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "interface role: backup\n"
    "switch from back to master interface\n"
    "switch time\n"
    "time <1-12>, default value is 1 min\n")
{
    int ret = 0;
    uint16_t msp_index = 0;
    uint16_t wtr = 0;
    struct stm_msp_config *pmsp = NULL;
    
    msp_index = (uint16_t)atoi(argv[0]);
    pmsp = stm_msp_lookup_by_mspid(msp_index);
    if(pmsp == NULL)
    {
        vty_error_out(vty, "msp group %d not exist! %s", msp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    wtr = (uint16_t)atoi(argv[1]);

    ret = stm_msp_wtr_set(pmsp, wtr);
    if(ret != 0)
    {
        vty_error_out(vty, "set wtr error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

DEFUN(stm_no_msp_group_wtr,
    stm_no_msp_group_wtr_cmd,
    "no msp-group <1-32> backup failback wtr",
    NO_STR
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "interface role: backup\n"
    "switch from back to master interface\n"
    "switch time\n")
{
    int ret = 0;
    uint16_t msp_index = 0;
    uint16_t wtr = 1;        //default value
    struct stm_msp_config *pmsp = NULL;
    
    msp_index = (uint16_t)atoi(argv[0]);
    pmsp = stm_msp_lookup_by_mspid(msp_index);
    if(pmsp == NULL)
    {
        vty_error_out(vty, "msp group %d not exist! %s", msp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    ret = stm_msp_wtr_set(pmsp, wtr);
    if(ret != 0)
    {
        vty_error_out(vty, "set wtr error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

/**
 * @brief      : 显示STM接口的信息，包括告警、统计、状态等信息。 
 * @param[in ] : stm 接口。
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : config 接口视图
 */
DEFUN(show_ifm_stm,
    show_ifm_stm_cmd,
    "show interface stm USP",
    SHOW_STR
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR )
{
    uint32_t ifindex = 0;
    struct stm_if *pstm_if = NULL;

    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[0]);
    pstm_if = stm_if_lookup(ifindex);
    if (pstm_if == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    stm_interface_show(vty, pstm_if);

    return CMD_SUCCESS;
}
    
DEFUN(show_ifm_all_stm, 
    show_ifm_stm_all_cmd,
    "show interface stm ",
    SHOW_STR
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR)
{
    struct stm_if *pstm_if = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;
    
    HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
    {
        pstm_if = (struct stm_if *)pbucket->data;
        if(pstm_if != NULL)
        {                
            stm_interface_show(vty, pstm_if);
        }
    }
    
    return CMD_SUCCESS;
}

/**
 * @brief      : 显示STM接口的配置信息。 
 * @param[in ] : stm 接口。
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : config 接口视图
 */
DEFUN(show_ifm_stm_config,
    show_ifm_stm_config_cmd,
    "show interface config stm USP",
    SHOW_STR
    CLI_INTERFACE_STR
    "Interface all config\n"
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR )
{
    uint32_t ifindex = 0;
    struct stm_if *pstm_if = NULL;

    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[0]);
    pstm_if = stm_if_lookup(ifindex);
    if (pstm_if == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    stm_interface_config_show(vty, pstm_if);
    
    return CMD_SUCCESS;
}

DEFUN(show_ifm_stm_all_config, 
    show_ifm_stm_all_config_cmd,
    "show interface config stm",
    SHOW_STR
    CLI_INTERFACE_STR
    "Interface all config\n"
    CLI_INTERFACE_STM_STR)
{
    struct stm_if *pstm_if = NULL;
	struct hash_bucket *pbucket = NULL;
	int cursor;
    
    HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
    {
        pstm_if = (struct stm_if *)pbucket->data;
        if(pstm_if != NULL)
        {                
            stm_interface_config_show(vty, pstm_if);
        }
    }
    
    return CMD_SUCCESS;
}


DEFUN(config_tdm_dxc1_bothway,
    config_tdm_dxc1_bothway_cmd,
	"vc12dxc interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface tdm USP num <1-63>",
    "vc12dxc\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_TDM_VHELP_STR
    "num\n"
    "tdm interface<1-63>\n"
    
    )	
 {
	uint32_t ifindex = 0, port = 0, vc4_num = 0;
	int ret = 0, num = 0, tdm_port = 0, i = 0;

	int new_vc12_backup = 0, new_vc12_base = 0;
//	int new_vc4_backup = 0, new_vc4_base = 0;

	int base_slot = 0, bk_slot = 0, dst_slot = 0;
    struct stm_dxc_config dxc_info;
    struct stm_dxc_config *dxc = NULL;
	struct stm_msp_config *msp = NULL;
	struct stm_msp_config *pmsp = NULL;
    struct listnode       *pnode = NULL;
	char vc12_name[256];

    memset(&dxc_info, 0, sizeof(struct stm_dxc_config));

	dxc_info.index = index_alloc(INDEX_TYPE_DXC);
	dxc_info.index_flag = 1;
	memset(vc12_name, 0, sizeof(vc12_name));
	sprintf(vc12_name, "%s%d", "vc12dxc", dxc_info.index);
	strcpy(dxc_info.name, vc12_name);

    dxc = stm_dxc_lookup(dxc_info.name);
    if(dxc != NULL)
    {
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
        vty_error_out(vty, "dxc %s has been configured! %s", dxc_info.name, VTY_NEWLINE);
        return CMD_WARNING;
    }

	num = (uint8_t)atoi(argv[7]);

    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[0]);  
    msp = stm_msp_lookup(ifindex);
    if(msp != NULL)
    {
        index_free(INDEX_TYPE_DXC, dxc_info.index);
        vty_error_out(vty, "base MSP protection groups have been configured! %s",  VTY_NEWLINE);
        return CMD_WARNING;
    }
    port = IFM_PORT_ID_GET ( ifindex );
    base_slot = IFM_SLOT_ID_GET ( ifindex );
 //   new_vc4_base = port - 1;

	vc4_num = (uint8_t)atoi(argv[1]);
	if(vc4_num == 1)
	{
		dxc_info.base_if.vc12_vc4 = vc4_num;
	}
	else
	{
		index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "base vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	new_vc12_base = (uint8_t)atoi(argv[2]) - 1;
	if((num+new_vc12_base)>STM_VC12_NUM_MAX)
	{
		index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "base vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}
	if ( NULL != argv[3])
	{
	    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[3]);
		msp = stm_msp_lookup(ifindex);
		if(msp != NULL)
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "bk MSP protection groups have been configured! %s",  VTY_NEWLINE);
      		return CMD_WARNING;
		}
		port = IFM_PORT_ID_GET ( ifindex );
		bk_slot = IFM_SLOT_ID_GET ( ifindex );
   //     new_vc4_backup = port - 1;

		vc4_num = (uint8_t)atoi(argv[4]);
		if(vc4_num == 1)
		{
			dxc_info.backup_if.vc12_vc4 = vc4_num;
		}
		else
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "backup vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
		    return CMD_WARNING;
		}
		new_vc12_backup = (uint8_t)atoi(argv[5]) - 1;
		if((num+new_vc12_backup)>STM_VC12_NUM_MAX)
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "backup vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
	        return CMD_WARNING;
		}
	}	
	ifindex = ifm_get_ifindex_by_name("tdm", (char *)argv[6]);	
	tdm_port = IFM_PORT_ID_GET ( ifindex );
	dst_slot = IFM_SLOT_ID_GET ( ifindex );

	for(ALL_LIST_ELEMENTS_RO(stm_msp_list, pnode, pmsp))
    {	
    	if(pmsp != NULL)
    	{
			port = IFM_PORT_ID_GET ( pmsp->master_if );
			
			if((port == 1)||(port == 2))
			{
				for(i=tdm_port;i<(tdm_port+num);i++)
				{
					if((i>0)&&(i<64))
					{
						index_free(INDEX_TYPE_DXC, dxc_info.index);
						vty_error_out(vty, "The 1-63 ports of TDM have been occupied by MSP protection! %s",  VTY_NEWLINE);
	       				return CMD_WARNING;
					}					
				}
			}
			else
			{
				for(i=tdm_port;i<(tdm_port+num);i++)
				{
					if((i>=64)&&(i<=126))
					{
						index_free(INDEX_TYPE_DXC, dxc_info.index);
						vty_error_out(vty, "The  64-126 ports of TDM have been occupied by MSP protection! %s",  VTY_NEWLINE);
	       				return CMD_WARNING;
					}					
				}
			}
		}
	}
	if(((tdm_port-1)+num)>STM_TDM_PORT_MAX)
	{
		index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "dst tdm port Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[0]);
   	dxc_info.base_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	dxc_info.base_if.dxc_vc4 = port-1;
	dxc_info.base_if.dxc_vc12 = new_vc12_base;
	dxc_info.mode 	 = STM_NO_PT_MODE; 
	if ( NULL != argv[3])
	{
		ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[3]);
	   	dxc_info.backup_if.ifindex = ifindex;
		port = IFM_PORT_ID_GET ( ifindex );
		dxc_info.backup_if.dxc_vc4 = port-1;
		dxc_info.backup_if.dxc_vc12 = new_vc12_backup;
    	dxc_info.mode = STM_PT_MODE; 
		dxc_info.failback = STM_PT_RECOVER;
		dxc_info.wtr = STM_WRT_TIME;
		
	}	
	ifindex = ifm_get_ifindex_by_name("tdm", (char *)argv[6]);	
	dxc_info.dst_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	if(port/64==0)
	{
		dxc_info.dst_if.dxc_vc4 = STM_TDM_VC4_64;
		dxc_info.dst_if.dxc_vc12 = port%64 - 1;
	}
	else if(port/64==1)
	{
		dxc_info.dst_if.dxc_vc4 = STM_TDM_VC4_128;
		dxc_info.dst_if.dxc_vc12 = port%64;
	}
    dxc_info.num 	 = (uint8_t)atoi(argv[7]);  
	dxc_info.action  = STM_DXC_ENABLE;

	if(base_slot != dst_slot)
	{
		index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
    	return CMD_WARNING;
	}
	if ( NULL != argv[3])
	{
		if((bk_slot != base_slot)||(bk_slot != dst_slot))
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
        	return CMD_WARNING;
		}
	}
	ret = stm_dxc_cross_check(&dxc_info,vty);
	if(ret != 0)
	{
		index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, " vc12 cross_check has been configured! %s",	VTY_NEWLINE);
    	return CMD_WARNING;
	}
    ret = stm_dxc_add(&dxc_info);
    if(ret != 0)
    {
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
        vty_error_out(vty, "tdm dxc NAMESTR add to list error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}   
DEFUN(config_tdm_dxc_bothway,
    config_tdm_dxc_bothway_cmd,
	"vc12dxc NAME interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface tdm USP num <1-63>",
    "vc12dxc\n"
    "name\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_TDM_STR
    CLI_INTERFACE_TDM_VHELP_STR
    "num\n"
    "tdm interface<1-63>\n"
    
    )
{
	uint32_t ifindex = 0, port = 0, vc4_num = 0;
	int ret = 0, num = 0, i = 0, tdm_port = 0;

	int new_vc12_backup = 0, new_vc12_base = 0;
//	int new_vc4_backup = 0, new_vc4_base = 0;

	int base_slot = 0, bk_slot = 0, dst_slot = 0;
    struct stm_dxc_config dxc_info;
    struct stm_dxc_config *dxc = NULL;
	struct stm_msp_config *msp = NULL;
	struct stm_msp_config *pmsp = NULL;
    struct listnode       *pnode = NULL;

    memset(&dxc_info, 0, sizeof(struct stm_dxc_config));
    strcpy(dxc_info.name, argv[0]);

    dxc = stm_dxc_lookup(dxc_info.name);
    if(dxc != NULL)
    {
        vty_error_out(vty, "dxc %s has been configured! %s", dxc_info.name, VTY_NEWLINE);
        return CMD_WARNING;
    }

	num = (uint8_t)atoi(argv[8]);

    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[1]);  
    msp = stm_msp_lookup(ifindex);
    if(msp != NULL)
    {
        vty_error_out(vty, "base MSP protection groups have been configured! %s",  VTY_NEWLINE);
        return CMD_WARNING;
    }
    port = IFM_PORT_ID_GET ( ifindex );
    base_slot = IFM_SLOT_ID_GET ( ifindex );
//    new_vc4_base = port - 1;

	vc4_num = (uint8_t)atoi(argv[2]);
	if(vc4_num == 1)
	{
		dxc_info.base_if.vc12_vc4 = vc4_num;
	}
	else
	{
		vty_error_out(vty, "base vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	new_vc12_base = (uint8_t)atoi(argv[3]) - 1;
	if((num+new_vc12_base)>STM_VC12_NUM_MAX)
	{
		vty_error_out(vty, "base vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}
	if ( NULL != argv[4])
	{
	    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[4]);
		msp = stm_msp_lookup(ifindex);
		if(msp != NULL)
		{
			vty_error_out(vty, "bk MSP protection groups have been configured! %s",  VTY_NEWLINE);
      		return CMD_WARNING;
		}
		port = IFM_PORT_ID_GET ( ifindex );
		bk_slot = IFM_SLOT_ID_GET ( ifindex );
//        new_vc4_backup = port - 1;

		vc4_num = (uint8_t)atoi(argv[5]);
		if(vc4_num == 1)
		{
			dxc_info.backup_if.vc12_vc4 = vc4_num;
		}
		else
		{
			vty_error_out(vty, "backup vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
		    return CMD_WARNING;
		}
		new_vc12_backup = (uint8_t)atoi(argv[6]) - 1;
		if((num+new_vc12_backup)>STM_VC12_NUM_MAX)
		{
			vty_error_out(vty, "backup vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
	        return CMD_WARNING;
		}
	}
	ifindex = ifm_get_ifindex_by_name("tdm", (char *)argv[7]);	
	tdm_port = IFM_PORT_ID_GET ( ifindex );
	dst_slot = IFM_SLOT_ID_GET ( ifindex );
	for(ALL_LIST_ELEMENTS_RO(stm_msp_list, pnode, pmsp))
    {	
    	if(pmsp != NULL)
    	{
			port = IFM_PORT_ID_GET ( pmsp->master_if );
			if((port == 1)||(port == 2))
			{
				for(i=tdm_port;i<(tdm_port+num);i++)
				{
					if((i>0)&&(i<64))
					{
						vty_error_out(vty, "The 1-63 ports of TDM have been occupied by MSP protection! %s",  VTY_NEWLINE);
	       				return CMD_WARNING;
					}					
				}
			}
			else
			{
				for(i=tdm_port;i<(tdm_port+num);i++)
				{
					if((i>=64)&&(i<=126))
					{
						vty_error_out(vty, "The  64-126 ports of TDM have been occupied by MSP protection! %s",  VTY_NEWLINE);
	       				return CMD_WARNING;
					}					
				}
			}
		}
	}
	if(((tdm_port-1)+num)>STM_TDM_PORT_MAX)
	{
		vty_error_out(vty, "dst tdm port Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}


	ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[1]);
   	dxc_info.base_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	dxc_info.base_if.dxc_vc4 = port-1;
	dxc_info.base_if.dxc_vc12 = new_vc12_base;
	dxc_info.mode 	 = STM_NO_PT_MODE; 
	if ( NULL != argv[4])
	{
		ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[4]);
	   	dxc_info.backup_if.ifindex = ifindex;
		port = IFM_PORT_ID_GET ( ifindex );
		dxc_info.backup_if.dxc_vc4 = port-1;
		dxc_info.backup_if.dxc_vc12 = new_vc12_backup;
    	dxc_info.mode = STM_PT_MODE; 
		dxc_info.failback = STM_PT_RECOVER;
		dxc_info.wtr = STM_WRT_TIME;
		
	}	
	ifindex = ifm_get_ifindex_by_name("tdm", (char *)argv[7]);	
	dxc_info.dst_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	if(port/64==0)
	{
		dxc_info.dst_if.dxc_vc4 = STM_TDM_VC4_64;
		dxc_info.dst_if.dxc_vc12 = port%64 - 1;
	}
	else if(port/64==1)
	{
		dxc_info.dst_if.dxc_vc4 = STM_TDM_VC4_128;
		dxc_info.dst_if.dxc_vc12 = port%64;
	}
    dxc_info.num 	 = (uint8_t)atoi(argv[8]);  
	dxc_info.action  = STM_DXC_ENABLE;

	if(base_slot != dst_slot)
	{
		vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
    	return CMD_WARNING;
	}
	if ( NULL != argv[4])
	{
		if((bk_slot != base_slot)||(bk_slot != dst_slot))
		{
			vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
        	return CMD_WARNING;
		}
	}
    ret = stm_dxc_cross_check(&dxc_info,vty);
	if(ret != 0)
	{
		vty_error_out(vty, " vc12 cross_check has been configured! %s",	VTY_NEWLINE);
    	return CMD_WARNING;
	}
    ret = stm_dxc_add(&dxc_info);
    if(ret != 0)
    {
        vty_error_out(vty, "tdm dxc NAMESTR add to list error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}

DEFUN(config_stm_dxc1_bothway ,
    config_stm_dxc1_bothway_cmd,
	"vc12dxc interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface stm USP vc4 <1-128> vc12 <1-63> num <1-63>",
    "vc12dxc\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "num\n"
    "stm interface<1-63>\n"
    
    )
{
	uint32_t ifindex = 0, port = 0, vc4_num = 0;
	int ret = 0, num = 0;

	int new_vc12_backup = 0, new_vc12_base = 0, new_vc12_dst = 0;
//	int new_vc4_backup = 0, new_vc4_base = 0, new_vc4_dst = 0;

	int base_slot = 0, bk_slot = 0, dst_slot = 0;
    struct stm_dxc_config dxc_info;
    struct stm_dxc_config *dxc = NULL;
	struct stm_msp_config *msp = NULL;
	char vc12_name[256];

    memset(&dxc_info, 0, sizeof(struct stm_dxc_config));

	dxc_info.index = index_alloc(INDEX_TYPE_DXC);
	dxc_info.index_flag = 1;
	memset(vc12_name, 0, sizeof(vc12_name));
	sprintf(vc12_name, "%s%d", "vc12dxc", dxc_info.index);
	strcpy(dxc_info.name, vc12_name);


    dxc = stm_dxc_lookup(dxc_info.name);
    if(dxc != NULL)
    { 	
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
        vty_error_out(vty, "dxc %s has been configured! %s",dxc_info.name,  VTY_NEWLINE);
        return CMD_WARNING;
    }

	num = (uint8_t)atoi(argv[9]);

    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[0]);  
    msp = stm_msp_lookup(ifindex);
    if(msp != NULL)
    {
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
        vty_error_out(vty, "base MSP protection groups have been configured! %s",  VTY_NEWLINE);
        return CMD_WARNING;
    }
    port = IFM_PORT_ID_GET ( ifindex );
    base_slot = IFM_SLOT_ID_GET ( ifindex );
 //   new_vc4_base = port - 1;

	vc4_num = (uint8_t)atoi(argv[1]);
	if(vc4_num == 1)
	{
		dxc_info.base_if.vc12_vc4 = vc4_num;
	}
	else
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "base vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	new_vc12_base = (uint8_t)atoi(argv[2]) - 1;
	if((num+new_vc12_base)>STM_VC12_NUM_MAX)
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "base vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}
	if ( NULL != argv[3])
	{
	    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[3]);
		msp = stm_msp_lookup(ifindex);
		if(msp != NULL)
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "bk MSP protection groups have been configured! %s",  VTY_NEWLINE);
      		return CMD_WARNING;
		}
		port = IFM_PORT_ID_GET ( ifindex );
		bk_slot = IFM_SLOT_ID_GET ( ifindex );
 //       new_vc4_backup = port - 1;

		vc4_num = (uint8_t)atoi(argv[4]);
		if(vc4_num == 1)
		{
			dxc_info.backup_if.vc12_vc4 = vc4_num;
		}
		else
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "backup vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
		    return CMD_WARNING;
		}
		new_vc12_backup = (uint8_t)atoi(argv[5]) - 1;
		if((num+new_vc12_backup)>STM_VC12_NUM_MAX)
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "backup vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
	        return CMD_WARNING;
		}
	}
    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[6]);
	msp = stm_msp_lookup(ifindex);
	if(msp != NULL)
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "bk MSP protection groups have been configured! %s",  VTY_NEWLINE);
  		return CMD_WARNING;
	}
	port = IFM_PORT_ID_GET ( ifindex );
	dst_slot = IFM_SLOT_ID_GET ( ifindex );
//    new_vc4_dst = port - 1;

	vc4_num = (uint8_t)atoi(argv[7]);
	if(vc4_num == 1)
	{
		dxc_info.dst_if.vc12_vc4 = vc4_num;
	}
	else
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "dst vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
	    return CMD_WARNING;
	}
	new_vc12_dst = (uint8_t)atoi(argv[8]) - 1;
	if((num+new_vc12_dst)>STM_VC12_NUM_MAX)
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "dst vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[0]);
   	dxc_info.base_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	dxc_info.base_if.dxc_vc4 = port-1;
	dxc_info.base_if.dxc_vc12 = new_vc12_base;
	dxc_info.mode 	 = STM_NO_PT_MODE; 
	if ( NULL != argv[3])
	{
		ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[3]);
	   	dxc_info.backup_if.ifindex = ifindex;
		port = IFM_PORT_ID_GET ( ifindex );
		dxc_info.backup_if.dxc_vc4 = port-1;
		dxc_info.backup_if.dxc_vc12 = new_vc12_backup;
    	dxc_info.mode = STM_PT_MODE; 
		dxc_info.failback = STM_PT_RECOVER;
		dxc_info.wtr = STM_WRT_TIME;
		
	}	
	ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[6]);
	dxc_info.dst_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	dxc_info.dst_if.dxc_vc4 = port-1;
	dxc_info.dst_if.dxc_vc12 = new_vc12_dst;
    dxc_info.num  = (uint8_t)atoi(argv[9]);
	dxc_info.action  = STM_DXC_ENABLE;

	if(base_slot != dst_slot)
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
    	return CMD_WARNING;
	}
	if ( NULL != argv[3])
	{
		if((bk_slot != base_slot)||(bk_slot != dst_slot))
		{
			index_free(INDEX_TYPE_DXC, dxc_info.index);
			vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
        	return CMD_WARNING;
		}
	}
    ret = stm_dxc_cross_check(&dxc_info,vty);
	if(ret != 0)
	{
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
		vty_error_out(vty, " vc12 cross_check has been configured! %s",	VTY_NEWLINE);
    	return CMD_WARNING;
	}
    ret = stm_dxc_add(&dxc_info);
    if(ret != 0)
    {
    	index_free(INDEX_TYPE_DXC, dxc_info.index);
        vty_error_out(vty, "stm dxc NAMESTR add to list error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}




DEFUN(config_stm_dxc_bothway ,
    config_stm_dxc_bothway_cmd,
	"vc12dxc NAME interface stm USP vc4 <1-128> vc12 <1-63> {backup interface stm USP vc4 <1-128> vc12 <1-63>} bothway interface stm USP vc4 <1-128> vc12 <1-63> num <1-63>",
    "vc12dxc\n"
    "name\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "backup interface\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "config bothway\n"
    CLI_INTERFACE_STR
    CLI_INTERFACE_STM_STR
    CLI_INTERFACE_STM_VHELP_STR
    "vc4\n"
    "vc4  index<1-128>\n"
    "vc12\n"
    "vc12 index<1-63>\n"
    "num\n"
    "stm interface<1-63>\n"
    
    )
{
	uint32_t ifindex = 0, port = 0, vc4_num = 0;
	int ret = 0, num = 0;

	int new_vc12_base = 0, new_vc12_backup = 0, new_vc12_dst = 0;
//	int new_vc4_backup = 0, new_vc4_base = 0, new_vc4_dst = 0;

	int base_slot = 0, bk_slot = 0, dst_slot = 0;
    struct stm_dxc_config dxc_info;
    struct stm_dxc_config *dxc = NULL;
	struct stm_msp_config *msp = NULL;

    memset(&dxc_info, 0, sizeof(struct stm_dxc_config));
    strcpy(dxc_info.name, argv[0]);

    dxc = stm_dxc_lookup(dxc_info.name);
    if(dxc != NULL)
    {
        vty_error_out(vty, "dxc %s has been configured! %s", dxc_info.name, VTY_NEWLINE);
        return CMD_WARNING;
    }

	num = (uint8_t)atoi(argv[10]);

    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[1]);  
    msp = stm_msp_lookup(ifindex);
    if(msp != NULL)
    {
        vty_error_out(vty, "base MSP protection groups have been configured! %s",  VTY_NEWLINE);
        return CMD_WARNING;
    }
    port = IFM_PORT_ID_GET ( ifindex );
    base_slot = IFM_SLOT_ID_GET ( ifindex );
//    new_vc4_base = port - 1;

	vc4_num = (uint8_t)atoi(argv[2]);
	if(vc4_num == 1)
	{
		dxc_info.base_if.vc12_vc4 = vc4_num;
	}
	else
	{
		vty_error_out(vty, "base vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	new_vc12_base = (uint8_t)atoi(argv[3]) - 1;
	if((num+new_vc12_base)>STM_VC12_NUM_MAX)
	{
		vty_error_out(vty, "base vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}
	if ( NULL != argv[4])
	{
	    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[4]);
		msp = stm_msp_lookup(ifindex);
		if(msp != NULL)
		{
			vty_error_out(vty, "bk MSP protection groups have been configured! %s",  VTY_NEWLINE);
      		return CMD_WARNING;
		}
		port = IFM_PORT_ID_GET ( ifindex );
		bk_slot = IFM_SLOT_ID_GET ( ifindex );
//        new_vc4_backup = port - 1;

		vc4_num = (uint8_t)atoi(argv[5]);
		if(vc4_num == 1)
		{
			dxc_info.backup_if.vc12_vc4 = vc4_num;
		}
		else
		{
			vty_error_out(vty, "backup vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
		    return CMD_WARNING;
		}
		new_vc12_backup = (uint8_t)atoi(argv[6]) - 1;
		if((num+new_vc12_backup)>STM_VC12_NUM_MAX)
		{
			vty_error_out(vty, "backup vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
	        return CMD_WARNING;
		}
	}
    ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[7]);
	msp = stm_msp_lookup(ifindex);
	if(msp != NULL)
	{
		vty_error_out(vty, "bk MSP protection groups have been configured! %s",  VTY_NEWLINE);
  		return CMD_WARNING;
	}
	port = IFM_PORT_ID_GET ( ifindex );
	dst_slot = IFM_SLOT_ID_GET ( ifindex );
//    new_vc4_dst = port - 1;

	vc4_num = (uint8_t)atoi(argv[8]);
	if(vc4_num == 1)
	{
		dxc_info.dst_if.vc12_vc4 = vc4_num;
	}
	else
	{
		vty_error_out(vty, "dst vc4 Configuration 1 are supported only %s",  VTY_NEWLINE);
	    return CMD_WARNING;
	}
	new_vc12_dst = (uint8_t)atoi(argv[9]) - 1;
	if((num+new_vc12_dst)>STM_VC12_NUM_MAX)
	{
		vty_error_out(vty, "dst vc12 Configuration range exceeds maximum! %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}

	ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[1]);
   	dxc_info.base_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	dxc_info.base_if.dxc_vc4 = port-1;
	dxc_info.base_if.dxc_vc12 = new_vc12_base;
	dxc_info.mode 	 = STM_NO_PT_MODE; 
	if ( NULL != argv[4])
	{
		ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[4]);
	   	dxc_info.backup_if.ifindex = ifindex;
		port = IFM_PORT_ID_GET ( ifindex );
		dxc_info.backup_if.dxc_vc4 = port-1;
		dxc_info.backup_if.dxc_vc12 = new_vc12_backup;
    	dxc_info.mode = STM_PT_MODE; 
		dxc_info.failback = STM_PT_RECOVER;
		dxc_info.wtr = STM_WRT_TIME;
		
	}	
	ifindex = ifm_get_ifindex_by_name("stm", (char *)argv[7]);
	dxc_info.dst_if.ifindex = ifindex;
	port = IFM_PORT_ID_GET ( ifindex );
	dxc_info.dst_if.dxc_vc4 = port-1;
	dxc_info.dst_if.dxc_vc12 = new_vc12_dst;
    dxc_info.num  = (uint8_t)atoi(argv[10]);
	dxc_info.action  = STM_DXC_ENABLE;

	if(base_slot != dst_slot)
	{
		vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
    	return CMD_WARNING;
	}
	if ( NULL != argv[4])
	{
		if((bk_slot != base_slot)||(bk_slot != dst_slot))
		{
			vty_error_out(vty, "not configured with the same slot interface! %s",  VTY_NEWLINE);
        	return CMD_WARNING;
		}
	}
    ret = stm_dxc_cross_check(&dxc_info,vty);
	if(ret != 0)
	{
		vty_error_out(vty, " vc12 cross_check has been configured! %s",	VTY_NEWLINE);
    	return CMD_WARNING;
	}
    ret = stm_dxc_add(&dxc_info);
    if(ret != 0)
    {
        vty_error_out(vty, "stm dxc NAMESTR add to list error! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    return CMD_SUCCESS;
}	
//"vc12dxc NAME backup {non-failback | failback | wtr <1-12>}",

DEFUN(config_stm_dxc_wtr ,
    config_stm_dxc_wtr_cmd,
	"vc12dxc NAME backup {failback | wtr <1-12>}",
    "vc12dxc\n"
    "name\n"
    "backup\n"
    "failback\n"
    "wtr time\n"
    "wtr time<1-12>\n"
    )	
{
    int ret = 0;
    char name_buf[256] = {0};
    struct stm_dxc_config *dxc = NULL;
	struct stm_dxc_config dxc_info;

	memset(&dxc_info, 0, sizeof(struct stm_dxc_config));
	strcpy(name_buf, argv[0]);
    dxc = stm_dxc_lookup(name_buf);
    if(dxc == NULL)
    {
        vty_error_out(vty, "dxc NAMESTR not exist! %s",  VTY_NEWLINE);
        return CMD_WARNING;
    }
	if(dxc->mode == STM_NO_PT_MODE)
	{
	    vty_error_out(vty, "dxc NAMESTR is not protect mode %s",  VTY_NEWLINE);
        return CMD_WARNING;
	}
#if 0	
	if (NULL != argv[1])
    {
        dxc_info.failback = STM_NO_PT_RECOVER;
    }
#endif	
    if (NULL != argv[1])
    {
       	dxc_info.failback = STM_PT_RECOVER;
        dxc_info.wtr      = 1;

        if (NULL != argv[2])
        {
            dxc_info.wtr = atoi(argv[2]);
        }
    }
	dxc_info.base_if.ifindex = dxc->base_if.ifindex;
	dxc_info.dst_if = dxc->dst_if;
	dxc_info.num = dxc->num;
    //ret = ipc_send_hal_wait_ack(&dxc_info, sizeof(struct stm_dxc_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_DXC_WRT, IPC_OPCODE_UPDATE, dxc->base_if.ifindex);
	ret = ces_msg_send_hal_wait_ack(&dxc_info, sizeof(struct stm_dxc_config), 1, MODULE_ID_HAL, MODULE_ID_CES, IPC_TYPE_STM, STM_INFO_DXC_WRT, IPC_OPCODE_UPDATE, dxc->base_if.ifindex);
	if (ret != 0)
    {
        zlog_err("%s:%s[%d]:ipc_send_hal_wait_ack errocode=%d\n", __FILE__, __FUNCTION__, __LINE__, ret);
        return -1;
    }
    dxc->wtr = dxc_info.wtr;
    dxc->failback = dxc_info.failback;

	return CMD_SUCCESS;
}


DEFUN(no_stm_dxc_namestr ,
    no_stm_dxc_namestr_cmd,
	"no vc12dxc NAME",
    "delete name\n"
    "vc12dxc\n"
    "name\n"
    )	
{
	int ret = 0;
    char name_buf[256] = {0};
	

	if(argv[0]==NULL)
	{
		vty_error_out(vty, "Please enter the name! %s", VTY_NEWLINE);
		return CMD_WARNING;
	}
	strcpy(name_buf, argv[0]);

	ret = stm_dxc_delete(name_buf);
	
    if(ret != 0)
    {
        vty_error_out(vty, "delete dxc group falied ! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
	return CMD_SUCCESS;
}

DEFUN(show_stm_dxc_namestr ,
    show_stm_dxc_namestr_cmd,
	"show vc12dxc [NAME]",
    "show namestr\n"
    "vc12dxc\n"
    "name\n"
    )	
{
//    int ret = 0;
    char name_buf[256] = {0};
//	uint8_t  *dxc_status = NULL;
	struct listnode *pnode = NULL;
    struct stm_dxc_config *dxc = NULL;

	if(argv[0]==NULL)
	{
		for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, dxc))
		{
			if(dxc->down_flag == LINK_DOWN)
			{
				return CMD_SUCCESS;
			}
			//dxc_status = stm_dxc_get_status(dxc);
			stm_dxc_show_status( vty, dxc);
				
		}
	}
	else
	{	
		strcpy(name_buf, argv[0]);
	    dxc = stm_dxc_lookup(name_buf);
	    if(dxc == NULL)
	    {
	        vty_error_out(vty, "dxc NAMESTR not exist! %s",  VTY_NEWLINE);
	        return CMD_WARNING;
	    }
	
		#if 0
			dxc_status = stm_dxc_get_status(dxc);
	    	if (ret != 0)
	    	{
				vty_error_out(vty, "Get status of dxc-group error! %s", VTY_NEWLINE);
	        	return -1;
	    	}
		#endif
		if(dxc->down_flag == LINK_DOWN)
		{
			return CMD_SUCCESS;
		}
		stm_dxc_show_status( vty, dxc);
			
		
	}
	return CMD_SUCCESS;
}


/**
 * @brief      : show msp-group status: working at master or backup interface 
 * @param[in ] : msp-group index。
 * @return     : return CMD_SUCCESS，or error code CMD_XXX
 * @author     : liub
 * @date       : 2018-9-10
 * @note       : config node
 */
DEFUN(show_msp_status,
    show_msp_status_cmd,
    "show msp-group <1-32> status",
    SHOW_STR
    "msp gourp\n"
    "msp gourp ID: <1-32>\n"
    "msp status:work at master interface or backup interface\n")
{
    //int ret = 0;
    uint8_t  msp_index = 0;
    uint8_t  *msp_status = NULL;
    char m_ifname[IFNET_NAMESIZE] = "";
    char b_ifname[IFNET_NAMESIZE] = "";
    struct stm_msp_config *stm_msp = NULL;


    msp_index = (uint8_t)atoi(argv[0]);
    stm_msp = stm_msp_lookup_by_mspid(msp_index);
    if(stm_msp == NULL)
    {
        vty_error_out(vty, "msp group %d not found! %s", msp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }

	if(stm_msp->down_flag == LINK_DOWN)
	{
		return CMD_SUCCESS;
	}
	
    msp_status = stm_msp_get_status(stm_msp->master_if);
    if(msp_status == NULL)
    {
        vty_error_out(vty, "Get status of msp-group %d error! %s", msp_index, VTY_NEWLINE);
        return CMD_WARNING;
    }

    ifm_get_name_by_ifindex ( stm_msp->master_if, m_ifname );
    ifm_get_name_by_ifindex ( stm_msp->backup_if, b_ifname );
    vty_out ( vty, "%-10s %-15s %-15s %-10s%s", "Group", "Master","Backup", "Status", VTY_NEWLINE );
    vty_out ( vty, "%-8d %-15s %-17s %-10s%s", stm_msp->msp_id, m_ifname, b_ifname, 
                                             *msp_status ? "backup" : "master", VTY_NEWLINE);
    
    return CMD_SUCCESS;
}


DEFUN(stm_switch_image,
    stm_switch_image_cmd,
    "switch emulation-image (e1 | stm-1) slot <1-32>",
    "switch\n"
    "Switch emulation image\n"
    "Image for E1 (Default)\n"
    "Image for STM-1\n"
    "slot number <1-32>")
{
    struct stm_if *pstm_if = NULL;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    int ret = 0;
    uint8_t p_slot = 0;
    uint32_t stm_slot = 0;
    uint32_t slot_exist = 0;
    uint32_t image_type = 0;
    struct devm_slot slot_info;
    
    if (!strncmp(argv[0], "e", 1))
    {
        image_type = STM_IMAGE_TYPE_E1;
    }
    else if (!strncmp(argv[0], "s", 1))
    {
        image_type = STM_IMAGE_TYPE_STM1;
    }

    stm_slot = atoi(argv[1]);
        
    memset(&slot_info, 0, sizeof(struct devm_slot));
    devm_comm_get_slot(1 , stm_slot, MODULE_ID_CES, &slot_info);
    if(slot_info.id != ID_SC01QE)
    {
        vty_error_out(vty, "Slot is not SC01QE! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
    {
        pstm_if = (struct stm_if *)pbucket->data;
        if(pstm_if != NULL)
        {
            p_slot = IFM_SLOT_ID_GET(pstm_if->ifindex);
            if(p_slot == stm_slot)
            {
                slot_exist = 1;
                break;
            }
        }
    }

    if(slot_exist)
    {
        ret = ces_msg_send_hal_wait_ack(&image_type, sizeof(image_type), 1, MODULE_ID_HAL, MODULE_ID_CES, 
                            IPC_TYPE_STM, STM_INFO_IMAGE_SWITCH, IPC_OPCODE_UPDATE, stm_slot);
        if (ret != 0)
        {
            vty_error_out(vty, "Switch image failed! ERR=%d %s", ret, VTY_NEWLINE);
            return CMD_WARNING;
        }
    }
    else
    {
        vty_error_out(vty, "Slot %d is not stm board! %s", stm_slot, VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    vty_out ( vty, "%%Success to switch the image to %s !%s", image_type == STM_IMAGE_TYPE_E1 ? "E1" : 
                                          (image_type == STM_IMAGE_TYPE_STM1 ? "STM-1" : "--"), VTY_NEWLINE );
    return CMD_SUCCESS;
}

int dxc_buf[SLOT_NUM_MAX][STM_DXC_VC4][STM_DXC_VC12];

int stm_dxc_cross_check(struct stm_dxc_config *pdxc_new, struct vty *vty)
{
    struct stm_dxc_config  *pdxc = NULL;
    struct listnode        *pnode = NULL;
    int                     i = 0;
	int 					slot = 0;
    int                     new_base_vc4 = 0;
    int                     new_base_vc12 = 0;	
    int                     new_backup_vc4 = 0;
    int                     new_backup_vc12 = 0;
    int                     new_dst_vc4 = 0;
    int                     new_dst_vc12 = 0;

    int                     old_base_vc4 = 0;
    int                     old_base_vc12 = 0;
    int                     old_backup_vc4 = 0;
    int                     old_backup_vc12 = 0;
    int                     old_dst_vc4 = 0;
    int                     old_dst_vc12 = 0;
    if (NULL == pdxc_new)
    {
        return -1;
    }
    memset(dxc_buf, 0, sizeof(dxc_buf));
    for(ALL_LIST_ELEMENTS_RO(stm_dxc_list, pnode, pdxc))
	{
		if(pdxc != NULL)
		{
			slot = IFM_SLOT_ID_GET ( pdxc->base_if.ifindex );
			old_base_vc4  = pdxc->base_if.dxc_vc4;
            old_base_vc12 = pdxc->base_if.dxc_vc12;

			old_dst_vc4   = pdxc->dst_if.dxc_vc4;
			old_dst_vc12  = pdxc->dst_if.dxc_vc12;
			
			old_backup_vc4	 = pdxc->backup_if.dxc_vc4;
			old_backup_vc12  = pdxc->backup_if.dxc_vc12;

            for (i = 0; i < pdxc->num; i++)
			{
			    if (old_base_vc4 < STM_DXC_VC4 && old_base_vc12 < STM_DXC_VC12)
			    {
    			    dxc_buf[slot][old_base_vc4][old_base_vc12] = 1;
    			}
			    if (old_dst_vc4 < STM_DXC_VC4 && old_dst_vc12 < STM_DXC_VC12)
			    {
        			dxc_buf[slot][old_dst_vc4][old_dst_vc12] = 1;
        	    }
                old_base_vc12++;
                if (old_base_vc12 >= 63)
                {
                   old_base_vc4++;
                   old_base_vc12 = 0;
                }
                old_dst_vc12++;
                if (old_dst_vc12 >= 63)
                {
                   old_dst_vc4++;
                   old_dst_vc12 = 0;
                }
                if (pdxc->mode == STM_PT_MODE)
                {
					if (old_backup_vc4 < STM_DXC_VC4 && old_backup_vc12 < STM_DXC_VC12)
		    		{
			    		dxc_buf[slot][old_backup_vc4][old_backup_vc12] = 1;
					}
					old_backup_vc12++;
	                if (old_backup_vc12 >= 63)
	                {
	                   old_backup_vc4++;
	                   old_backup_vc12 = 0;
	                }
                }
			}
        }
   }
   slot = IFM_SLOT_ID_GET ( pdxc_new->base_if.ifindex );
   new_base_vc4  = pdxc_new->base_if.dxc_vc4;
   new_base_vc12 = pdxc_new->base_if.dxc_vc12;

   new_dst_vc4   = pdxc_new->dst_if.dxc_vc4;
   new_dst_vc12  = pdxc_new->dst_if.dxc_vc12;
	
   new_backup_vc4 = pdxc_new->backup_if.dxc_vc4;
   new_backup_vc12  = pdxc_new->backup_if.dxc_vc12;
   for (i = 0; i < pdxc_new->num; i++)
   {
        if (new_base_vc4 < STM_DXC_VC4 && new_base_vc12 < STM_DXC_VC12)
        {
            if (dxc_buf[slot][new_base_vc4][new_base_vc12] == 1)
            {
            	vty_error_out(vty, "base_vc4=%d, base_vc12=%d, has been configured! %s",new_base_vc4,  (new_base_vc12+1), VTY_NEWLINE);
	        	return CMD_WARNING;
            }
            dxc_buf[slot][new_base_vc4][new_base_vc12] = 1;
        }
		new_base_vc12++;
        if (new_base_vc12 >= 63)
        {
           new_base_vc4++;
           new_base_vc12 = 0;
        }
   	}	
    for (i = 0; i < pdxc_new->num; i++)
    {
        if (pdxc_new->mode == STM_PT_MODE)
        {
            if (new_backup_vc4 < STM_DXC_VC4 && new_backup_vc12 < STM_DXC_VC12)
	        {
	            if (dxc_buf[slot][new_backup_vc4][new_backup_vc12] == 1)
	            {
					vty_error_out(vty, "backup_vc4=%d, backup_vc12=%d, has been configured! %s",new_backup_vc4,  (new_backup_vc12+1), VTY_NEWLINE);
		        	return CMD_WARNING;
	            }
	            dxc_buf[slot][new_backup_vc4][new_backup_vc12] = 1;
	        }  
        }
		new_backup_vc12++;
		if (new_backup_vc12 >= 63)
		{
		   new_backup_vc4++;
		   new_backup_vc12 = 0;
		}

    }	
	for (i = 0; i < pdxc_new->num; i++)
	{
	    if (new_dst_vc4 < STM_DXC_VC4 && new_dst_vc12 < STM_DXC_VC12)
        {
            if (dxc_buf[slot][new_dst_vc4][new_dst_vc12] == 1)
            {
				vty_error_out(vty, "dst_vc4=%d, dst_vc12=%d, has been configured! %s",new_dst_vc4,  (new_dst_vc12+1), VTY_NEWLINE);
	        	return CMD_WARNING;
            }
            dxc_buf[slot][new_dst_vc4][new_dst_vc12] = 1;
        } 
        new_dst_vc12++;
        if (new_dst_vc12 >= 63)
        {
           new_dst_vc4++;
           new_dst_vc12 = 0;
        }		
   }

    return 0;
}

void stm_dxc_show_status(struct vty *vty, struct stm_dxc_config *dxc)
{
	char m_ifname[IFNET_NAMESIZE] = "";
    char b_ifname[IFNET_NAMESIZE] = "";
	char d_ifname[IFNET_NAMESIZE] = "";

	ifm_get_name_by_ifindex ( dxc->base_if.ifindex, m_ifname );
	ifm_get_name_by_ifindex ( dxc->dst_if.ifindex, d_ifname );
				
	vty_out ( vty, "---------------------------------------------------------------%s", VTY_NEWLINE );	
	
	vty_out ( vty, "%-20s %-17s%s", "name", dxc->name,VTY_NEWLINE );
	vty_out ( vty, "%-20s %-s%s%d%s%d%s", "source", m_ifname,"-vc4:",dxc->base_if.vc12_vc4,"-vc12:",dxc->base_if.dxc_vc12+1,VTY_NEWLINE );
	if(dxc->mode == STM_PT_MODE)
	{
		ifm_get_name_by_ifindex ( dxc->backup_if.ifindex, b_ifname );
		vty_out ( vty, "%-20s %-s%s%d%s%d%s", "protect", b_ifname,"-vc4:",dxc->backup_if.vc12_vc4,"-vc12:",dxc->backup_if.dxc_vc12+1,VTY_NEWLINE );
	}
	else
	{
		vty_out ( vty, "%-20s %-s%s%s%s", "protect", "stm -/-/-","-vc4:-","-vc12:-",VTY_NEWLINE );
	}
	if (!IFM_TYPE_IS_STM(dxc->dst_if.ifindex))
	{	
		vty_out ( vty, "%-20s %-s%s", "destination", d_ifname,VTY_NEWLINE );
	}
	else
	{
		vty_out ( vty, "%-20s %-s%s%d%s%d%s", "destination", d_ifname,"-vc4:",dxc->dst_if.vc12_vc4,"-vc12:",dxc->dst_if.dxc_vc12+1,VTY_NEWLINE );
	}
	vty_out ( vty, "%-20s %-17d%s", "num", dxc->num,VTY_NEWLINE );
	if(dxc->mode == STM_PT_MODE)
	{
		vty_out ( vty, "%-20s %-17s%s", "work-status", dxc->status ? "protect" : "source",VTY_NEWLINE );
		if(dxc->failback == STM_PT_RECOVER)
		{
			vty_out ( vty, "%-20s %-17s%s", "failback-mode", "yes",VTY_NEWLINE );
			vty_out ( vty, "%-20s %-17d%s", "wtr", dxc->wtr,VTY_NEWLINE );			
		}
		else if(dxc->failback == STM_NO_PT_RECOVER)
		{
			vty_out ( vty, "%-20s %-17s%s", "failback-mode", "no",VTY_NEWLINE );	
			vty_out ( vty, "%-20s %-17s%s", "wtr", "--",VTY_NEWLINE );
			
		}
		else
		{
			vty_out ( vty, "%-20s %-17s%s", "failback-mode", "--",VTY_NEWLINE );	
			vty_out ( vty, "%-20s %-17s%s", "wtr", "--",VTY_NEWLINE );
		}
	}
	else
	{
		vty_out ( vty, "%-20s %-17s%s", "work-status", "--",VTY_NEWLINE );
		vty_out ( vty, "%-20s %-17s%s", "failback-mode", "--",VTY_NEWLINE );
		vty_out ( vty, "%-20s %-17s%s", "wtr", "--",VTY_NEWLINE );
	}
	vty_out ( vty, "---------------------------------------------------------------%s", VTY_NEWLINE );

}

void stm_interface_status_show(struct vty *vty, struct stm_if *pif)
{
    uint8_t slot = 0;
    uint8_t num = 1;
    struct stm_hp_t         *stm_hp = NULL;
    struct stm_oh_info_t    *oh_status = NULL;

    oh_status = stm_if_get_status(pif->ifindex);
    if(oh_status == NULL)
    {
        zlog_err("%s:%s[%d]: stm_if_get_status error\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }

    //保存获取到的再生段开销值j0
    memcpy(pif->stm_rs.j0_rx, oh_status->j0 + 1, STM_OVERHEAD_LEN);

    //保存获取到的复用段开销值k1/k2/m1
    pif->stm_ms.k1_rx = oh_status->k1;
    pif->stm_ms.k2_rx = oh_status->k2;
    pif->stm_ms.m1 = oh_status->m1;

    vty_out ( vty, "SDH status: %s", VTY_NEWLINE );
    slot = IFM_SLOT_ID_GET(pif->ifindex);
    if(g_stm_slot[slot].slot_id == ID_SC01QE)
    {
        vty_out ( vty, "    FPGA image type: %s%s", g_stm_slot[slot].image_type == 0 ? "E1" : 
                                          (g_stm_slot[slot].image_type == 1 ? "STM-1" : "--"), VTY_NEWLINE );
        if(g_stm_slot[slot].image_type == STM_IMAGE_TYPE_STM1)
        {
            return;
        }
    }
    vty_out ( vty, "    Frame-format SDH, multiplex AU-4 %s", VTY_NEWLINE );
    vty_out ( vty, "    Overhead: %s", VTY_NEWLINE );
    //Rx 的值都从hal获取，这里用本地保存的值做测试
    vty_out ( vty, "        J0 Rx: %s", VTY_NEWLINE );
    vty_out ( vty, "            String: %s%s", pif->stm_rs.j0_rx, VTY_NEWLINE );
    
    vty_out ( vty, "        J0 Tx(Expected to receive): %s", VTY_NEWLINE );
    vty_out ( vty, "            String: %s%s", pif->stm_rs.j0_tx, VTY_NEWLINE );

    //Rx 的值都从hal获取，这里用本地保存的值做测试
    /*j1 和 c2 在stm接口为stm-4/16/64 时遍历stm_hp_list，然后显示对应vc4的开销
    * 在stm 16/64时显示的内容过多，考虑在命令行加入vc4_id参数，只显示指定vc4的开销
    */

    //for(num = 1; num <= pif->type; num++)
    {
        vty_out ( vty, "VC4-%d: %s", num, VTY_NEWLINE);
        
        stm_hp = stm_hp_lookup(pif->stm_hp_list, num);
        if(stm_hp == NULL)
        {
            zlog_err("%s:%s[%d] stm_hp_lookup error!\n", __FILE__, __FUNCTION__, __LINE__);
            return;
        }

        //保存获取到的高阶通道开销值
        stm_hp->hp_info.c2_rx = oh_status->c2;
        stm_hp->hp_info.g1_rx = oh_status->g1;
        memcpy(stm_hp->hp_info.j1_rx, oh_status->j1 + 1, STM_OVERHEAD_LEN);
        
        vty_out ( vty, "        J1 Rx: %s", VTY_NEWLINE );
        vty_out ( vty, "            String: %s%s", stm_hp->hp_info.j1_rx, VTY_NEWLINE );
        
        vty_out ( vty, "        J1 Tx(Expected to receive): %s", VTY_NEWLINE );
        vty_out ( vty, "            String: %s%s", stm_hp->hp_info.j1_tx, VTY_NEWLINE );
        
        //Rx 的值都从hal获取，这里用本地保存的值做测试
        vty_out ( vty, "        C2 Rx: 0x%02x%s", stm_hp->hp_info.c2_rx, VTY_NEWLINE );
        vty_out ( vty, "        C2 Tx: 0x%02x%s", stm_hp->hp_info.c2_tx, VTY_NEWLINE );
        vty_out ( vty, "        G1 Rx: 0x%02x%s", stm_hp->hp_info.g1_rx, VTY_NEWLINE );
    }

    
    vty_out ( vty, "%s", VTY_NEWLINE);
    vty_out ( vty, "        M1 Rx: 0x%02x%s", pif->stm_ms.m1, VTY_NEWLINE );
    vty_out ( vty, "        K1 Rx: 0x%02x%s", pif->stm_ms.k1_rx, VTY_NEWLINE );
    vty_out ( vty, "        K2 Rx: 0x%02x%s", pif->stm_ms.k2_rx, VTY_NEWLINE );

}

void stm_interface_alarm_show(struct vty *vty, struct stm_if *pif)
{
    uint8_t slot = 0;
    struct stm_rs_alarm_t   *rs_alarm = NULL;
    struct stm_ms_alarm_t   *ms_alarm = NULL;
    struct stm_hp_alarm_t   *hp_alarm = NULL;
    
    vty_out ( vty, "SDH alarm:%s", VTY_NEWLINE );

    //再生段告警显示
    vty_out ( vty, "    rs alarm:%s", VTY_NEWLINE );
    
    rs_alarm = &(pif->stm_rs.rs_alarm);
    if(rs_alarm->los){
        vty_out ( vty, "        los%s", VTY_NEWLINE );
    }
    
    slot = IFM_SLOT_ID_GET(pif->ifindex);
    if(g_stm_slot[slot].slot_id == ID_SC01QE && g_stm_slot[slot].image_type == STM_IMAGE_TYPE_STM1)
    {
        return;
    }

    if(rs_alarm->lof){
        vty_out ( vty, "        lof%s", VTY_NEWLINE );
    }

    if(rs_alarm->oof){
        vty_out ( vty, "        oof%s", VTY_NEWLINE );
    }

    if(rs_alarm->rs_exc){
        vty_out ( vty, "        rs_exc%s", VTY_NEWLINE );
    }

    if(rs_alarm->rs_deg){
        vty_out ( vty, "        rs_deg%s", VTY_NEWLINE );
    }

    if(rs_alarm->rs_tim){
        vty_out ( vty, "        rs_tim%s", VTY_NEWLINE );
    }
    
    //复用段告警显示
    vty_out ( vty, "    ms alarm:%s", VTY_NEWLINE );
    
    ms_alarm = &(pif->stm_ms.ms_alarm);
    if(ms_alarm->ms_ais){
        vty_out ( vty, "        ms_ais%s", VTY_NEWLINE );
    }

    if(ms_alarm->ms_rdi){
        vty_out ( vty, "        ms_rdi%s", VTY_NEWLINE );
    }

    if(ms_alarm->ms_rei){
        vty_out ( vty, "        ms_rei%s", VTY_NEWLINE );
    }

    if(ms_alarm->ms_exc){
        vty_out ( vty, "        ms_exc%s", VTY_NEWLINE );
    }

    if(ms_alarm->ms_deg){
        vty_out ( vty, "        ms_deg%s", VTY_NEWLINE );
    }

    //高阶通道告警显示, hal层上报的告警信息暂时没有vc4_id,默认为1
    vty_out ( vty, "    hp alarm:%s", VTY_NEWLINE );
    
    hp_alarm = &(pif->stm_hp_list->hp_info.hp_alarm);
    if(hp_alarm->au_lop){
        vty_out ( vty, "        au_lop%s", VTY_NEWLINE );
    }

    if(hp_alarm->au_ais){
        vty_out ( vty, "        au_ais%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->tu_lom){
        vty_out ( vty, "        tu_lom%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_unq){
        vty_out ( vty, "        hp_unq%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_slm){
        vty_out ( vty, "        hp_slm%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_rdi){
        vty_out ( vty, "        hp_rdi%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_rei){
        vty_out ( vty, "        hp_rei%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_exc){
        vty_out ( vty, "        hp_exc%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_deg){
        vty_out ( vty, "        hp_deg%s", VTY_NEWLINE );
    }
    
    if(hp_alarm->hp_tim){
        vty_out ( vty, "        hp_tim%s", VTY_NEWLINE );
    }
    
    return;
}

void stm_interface_statis_show(struct vty *vty, struct stm_if *pif)
{
    uint8_t slot = 0;
    struct stm_count_info *stm_count = NULL;
    
    slot = IFM_SLOT_ID_GET(pif->ifindex);
    if(g_stm_slot[slot].slot_id == ID_SC01QE && g_stm_slot[slot].image_type == STM_IMAGE_TYPE_STM1)
    {
        return;
    }

    //目前只支持stm-1，所以get到的信息不包含vc4_id，默认为1
    stm_count = stm_if_get_count(pif->ifindex);
    if(stm_count == NULL)
    {
        zlog_err("%s:%s[%d]: stm_if_get_count error\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    
    pif->stm_rs.b1 = stm_count->B1;
    pif->stm_ms.b2 = stm_count->B2;
    pif->stm_hp_list->hp_info.b3 = stm_count->B3;
    
    vty_out ( vty, "SDH statistics:%s", VTY_NEWLINE );
    vty_out ( vty, "    B1: %d%s", pif->stm_rs.b1, VTY_NEWLINE );
    vty_out ( vty, "    B2: %d%s", pif->stm_ms.b2, VTY_NEWLINE );
    vty_out ( vty, "    B3: %d%s", pif->stm_hp_list->hp_info.b3, VTY_NEWLINE );
    
    return ;
}

void stm_interface_show(struct vty *vty, struct stm_if *pif)
{
    char ifname[IFNET_NAMESIZE] = "";
    
    if(pif == NULL)
    {
        zlog_err("%s:%s[%d] err, pstm is NULL!\n", __FILE__, __FUNCTION__, __LINE__);
        return;
    }
    
    ifm_get_name_by_ifindex ( pif->ifindex, ifname );
    
    vty_out ( vty, "-----------------------------------------------------------------%s", VTY_NEWLINE);
    
    stm_interface_status_show(vty, pif);
    vty_out ( vty, "-----------------------------------------------------------------%s", VTY_NEWLINE);
    
    stm_interface_alarm_show(vty, pif);
    vty_out ( vty, "-----------------------------------------------------------------%s", VTY_NEWLINE);
    
    stm_interface_statis_show(vty, pif);
    vty_out ( vty, "-----------------------------------------------------------------%s", VTY_NEWLINE);
    
}

void stm_interface_config_show(struct vty *vty, struct stm_if *pif)
{
    uint8_t num = 1;
    struct stm_hp_t         *stm_hp = NULL;
    struct stm_msp_config   *stm_msp = NULL;
    char ifname[IFNET_NAMESIZE] = "";
    char m_ifname[IFNET_NAMESIZE] = "";
    char b_ifname[IFNET_NAMESIZE] = "";
    
    ifm_get_name_by_ifindex ( pif->ifindex, ifname );
    //vty_out ( vty, "interface %s%s", ifname, VTY_NEWLINE );
    vty_out ( vty, " Frame-format SDH, multiplex AU-4 %s", VTY_NEWLINE );
    vty_out ( vty, " Loopback: %s%s", pif->loopback == 0 ? "none":
                                     (pif->loopback == 1 ? "remote":
                                     (pif->loopback == 2 ? "local":"--")), VTY_NEWLINE);
    vty_out ( vty, " SDH overhead Tx: %s", VTY_NEWLINE);
    vty_out ( vty, "    J0: %s%s", pif->stm_rs.j0_tx, VTY_NEWLINE );
    
    //与stm_interface_status_show一样，考虑在命令行加入vc4_id参数，只显示指定vc4的开销
    //for(num = 1; num <= pif->type; num++)
    {   
        vty_out ( vty, " VC4-%d: %s", num, VTY_NEWLINE);
        stm_hp = stm_hp_lookup(pif->stm_hp_list, num);
        if(stm_hp == NULL)
        {
            zlog_err("%s:%s[%d] stm_hp_lookup error!\n", __FILE__, __FUNCTION__, __LINE__);
            return;
        }
        
        vty_out ( vty, "    J1: %s%s", stm_hp->hp_info.j1_tx, VTY_NEWLINE );  
        vty_out ( vty, "    C2: %d%s", stm_hp->hp_info.c2_tx, VTY_NEWLINE );
    }

    //stm与tdm接口映射关系
    vty_out ( vty, " Cross matrix:%s", VTY_NEWLINE);
    vty_out ( vty, "        --------%s", VTY_NEWLINE);
    
    //保护组关系
    vty_out ( vty, " MSP:%s", VTY_NEWLINE);
    stm_msp = stm_msp_lookup(pif->ifindex);
    if(stm_msp != NULL)
    {
        ifm_get_name_by_ifindex ( stm_msp->master_if, m_ifname );
        ifm_get_name_by_ifindex ( stm_msp->backup_if, b_ifname );
        vty_out ( vty, " %-10s %-15s %-15s %-5s%s", "Group", "Master","Backup", "WTR", VTY_NEWLINE );
        vty_out ( vty, " %-8d %-15s %-15s %5d%s", stm_msp->msp_id, m_ifname, b_ifname, stm_msp->wtr, VTY_NEWLINE);
    }
    else
    {
        vty_out ( vty, " No msp group found!%s", VTY_NEWLINE);
    }
    
}

/* 写配置文件 */
 int stm_config_write(struct vty *vty)
{
    struct stm_if       *pif = NULL;
    struct stm_hp_t     *stm_hp = NULL;
    struct hash_bucket  *pbucket = NULL;
    struct stm_msp_config *stm_msp = NULL;
    struct listnode *node = NULL;
	struct listnode *next_node = NULL;
    char m_ifname[IFNET_NAMESIZE] = "";
    char b_ifname[IFNET_NAMESIZE] = "";
    int *pdata = NULL;
    char ifname[IFNET_NAMESIZE];
    uint32_t ifindex;
    int cursor;
    int ret = 0;
    
    HASH_BUCKET_LOOP(pbucket, cursor, stm_if_table)
    {
        pif = pbucket->data;
        if ( pif == NULL )
        {
            zlog_err ( "%s[%d] pif == NULL\n", __FUNCTION__, __LINE__ );
            return 0;
        }
        
        ifindex = pif->ifindex;
        ret = ifm_get_name_by_ifindex(ifindex, ifname);
        if (ret != 0)
        {
            zlog_err ( "%s[%d] ifm_get_name_by_ifindex err\n", __FUNCTION__, __LINE__ );
            return 0;
        }
        
        vty_out(vty,"!%s",VTY_NEWLINE);
        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        if(strncmp(pif->stm_rs.j0_tx, STM_OVERHEAD_J0, STM_OVERHEAD_LEN) != 0)
        {
            if(pif->stm_rs.pad_zero)
            {
                vty_out(vty, " oh j0 sdh %s padding-zero%s", pif->stm_rs.j0_tx, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " oh j0 sdh %s%s", pif->stm_rs.j0_tx, VTY_NEWLINE);
            }
        }

        //遍历stm_hp_list, 不同vc4_id配置 c2 和 j1
        stm_hp = pif->stm_hp_list;
        if (stm_hp->hp_info.c2_tx != STM_OVERHEAD_C2)
        {
            vty_out(vty, " oh c2 %x%s", stm_hp->hp_info.c2_tx, VTY_NEWLINE);
        }

        if(strncmp(stm_hp->hp_info.j1_tx, STM_OVERHEAD_J1, STM_OVERHEAD_LEN) != 0)
        {
            if(pif->stm_hp_list->pad_zero)
            {
                vty_out(vty, " oh j1 sdh %s padding-zero%s", stm_hp->hp_info.j1_tx, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " oh j1 sdh %s%s", stm_hp->hp_info.j1_tx, VTY_NEWLINE);
            }
        }
        
        vty_out(vty, "!%s", VTY_NEWLINE);
    }
    
    for ( ALL_LIST_ELEMENTS ( stm_msp_list, node, next_node, pdata ) )
    {
        stm_msp = (struct stm_msp_config *)pdata;
        if(stm_msp == NULL)
        {
            continue;
        }

		if(stm_msp->down_flag == LINK_DOWN)
		{
			listnode_delete(stm_msp_list, stm_msp);
			XFREE(MTYPE_IFM_ENTRY, stm_msp);
			continue;
		}
    
        ifm_get_name_by_ifindex ( stm_msp->master_if, m_ifname );
        ifm_get_name_by_ifindex ( stm_msp->backup_if, b_ifname );
        if(stm_msp->set_flag)
        {
            vty_out(vty, "msp-group %d master interface %s backup interface %s mode %s %s", 
                                stm_msp->msp_id, m_ifname, b_ifname, stm_msp->mode?"opo":"oto", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, "msp-group %d master interface %s backup interface %s %s", 
                                                    stm_msp->msp_id, m_ifname, b_ifname, VTY_NEWLINE);
        }
    }
    
    vty_out(vty,"!%s",VTY_NEWLINE);

    return 0;
}
static int stm_dxc_config_write(struct vty *vty)
{
	int flag = 0;
    //char name_buf[16] = {0};
	struct listnode *pnode = NULL;
	struct listnode *pnode_next = NULL;
    struct stm_dxc_config *dxc = NULL;
	char m_ifname[IFNET_NAMESIZE] = "";
    char b_ifname[IFNET_NAMESIZE] = "";
	char d_ifname[IFNET_NAMESIZE] = "";

	for(ALL_LIST_ELEMENTS(stm_dxc_list, pnode, pnode_next, dxc))
	{
		if(NULL == dxc)
		{
			continue;
		}
		if(flag==0)
		{
			vty_out(vty, "sdhdxc %s", VTY_NEWLINE);
			flag++;
		}		
		if(dxc->down_flag == LINK_DOWN)
		{
			//stm_dxc_delete(dxc->name);

			listnode_delete(stm_dxc_list, dxc);
            XFREE(MTYPE_IFM_ENTRY, dxc);
			continue;
		}
		
		ifm_get_name_by_ifindex ( dxc->base_if.ifindex, m_ifname );
		ifm_get_name_by_ifindex ( dxc->dst_if.ifindex, d_ifname );
		
		if(dxc->mode != STM_PT_MODE)
		{
			if (!IFM_TYPE_IS_STM(dxc->dst_if.ifindex))
			{
            	vty_out(vty, " vc12dxc %s interface %s vc4 %d vc12 %d bothway interface %s num %d%s", 
                   dxc->name, m_ifname, dxc->base_if.vc12_vc4, dxc->base_if.dxc_vc12+1, d_ifname, dxc->num, VTY_NEWLINE);
			}
			else
			{
				vty_out(vty, " vc12dxc %s interface %s vc4 %d vc12 %d bothway interface %s vc4 %d vc12 %d num %d%s", 
					   dxc->name, m_ifname, dxc->base_if.vc12_vc4, dxc->base_if.dxc_vc12+1, d_ifname, dxc->dst_if.vc12_vc4, dxc->dst_if.dxc_vc12+1, dxc->num, VTY_NEWLINE);
			}
		}
		else
		{		
			ifm_get_name_by_ifindex ( dxc->backup_if.ifindex, b_ifname );
			if (!IFM_TYPE_IS_STM(dxc->dst_if.ifindex))
			{
            	vty_out(vty, " vc12dxc %s interface %s vc4 %d vc12 %d backup interface %s vc4 %d vc12 %d bothway interface %s num %d%s", 
                   dxc->name, m_ifname, dxc->base_if.vc12_vc4, dxc->base_if.dxc_vc12+1, b_ifname, dxc->backup_if.vc12_vc4, dxc->backup_if.dxc_vc12+1, d_ifname, dxc->num, VTY_NEWLINE);
			}
			else
			{
				vty_out(vty, " vc12dxc %s interface %s vc4 %d vc12 %d backup interface %s vc4 %d vc12 %d bothway interface %s vc4 %d vc12 %d num %d%s", 
					   dxc->name, m_ifname, dxc->base_if.vc12_vc4, dxc->base_if.dxc_vc12+1, b_ifname, dxc->backup_if.vc12_vc4, dxc->backup_if.dxc_vc12+1, d_ifname, dxc->dst_if.vc12_vc4, dxc->dst_if.dxc_vc12+1, dxc->num, VTY_NEWLINE);
			}
			if(dxc->failback != STM_PT_RECOVER)
			{
				vty_out(vty, " vc12dxc %s backup non-failback %s", 
				   dxc->name,VTY_NEWLINE);
			}
			else
			{
				vty_out(vty, " vc12dxc %s backup failback wtr %d%s", 
			   	dxc->name,dxc->wtr,VTY_NEWLINE);
			}
		}	
	}
	return 0;
}
/* install all cmd */
void stm_cmd_init(void)
{
	install_node(&sdh_dxc_node, stm_dxc_config_write);
	install_default (SDH_DXC_NODE);
    ifm_stm_init(stm_config_write);
    															
    install_element(CONFIG_NODE, &sdh_dxc_mode_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &config_tdm_dxc_bothway_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &config_tdm_dxc1_bothway_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &config_stm_dxc_bothway_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &config_stm_dxc1_bothway_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &config_stm_dxc_wtr_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &no_stm_dxc_namestr_cmd, CMD_SYNC);
	install_element(SDH_DXC_NODE, &show_stm_dxc_namestr_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_loopback_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_no_loopback_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_oh_j0_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_no_oh_j0_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_oh_j1_cmd, CMD_SYNC);    
    install_element(STM_IF_NODE, &stm_no_oh_j1_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_oh_c2_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_no_oh_c2_cmd, CMD_SYNC);
    install_element(STM_IF_NODE, &stm_map_to_tdm_cmd, CMD_SYNC);    
    install_element(STM_IF_NODE, &stm_no_map_to_tdm_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &stm_msp_group_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &stm_no_msp_group_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &stm_msp_group_wtr_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &stm_no_msp_group_wtr_cmd, CMD_SYNC);

    install_element(CONFIG_NODE, &show_ifm_stm_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ifm_stm_all_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ifm_stm_config_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ifm_stm_all_config_cmd, CMD_LOCAL);
    
    install_element(CONFIG_NODE, &show_msp_status_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &stm_switch_image_cmd, CMD_LOCAL);
    
}

