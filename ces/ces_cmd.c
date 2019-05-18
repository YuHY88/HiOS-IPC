/*
 * =====================================================================================
 *
 *       Filename:  ces_cmd.c
 *
 *    Description:
 *
 *        Version:  1.0
 *        Created:  06/28/2017 09:09:18 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (),
 *   Organization:
 *
 * =====================================================================================
 */
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>
#include <ces/ces_if.h>
#include <ces/ces_cmd.h>
#include <lib/crc8.h>

//配置端口的封装格式
DEFUN(port_encap,
      port_encap_cmd,
      "encapsulate (satop | cesopsn)",
      "Interface encap format\n"
      "Satop format\n"
      "Cesopsn format\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int encap = CES_ENCAP_SATOP;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);

    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (!strncmp(argv[0], "s", 1))
    {
        encap = CES_ENCAP_SATOP;
    }
    else if (!strncmp(argv[0], "c", 1))
    {
        encap = CES_ENCAP_CESOPSN;
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_NOSUPPORT_CESOPSN), VTY_NEWLINE);
        return CES_ERR_NOSUPPORT_CESOPSN;
    }

    ret = ces_if_info_set(pif, (void *)&encap, CES_INFO_ENCAP);

    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//恢复默认端口的封装格式
DEFUN(no_port_encap,
      no_port_encap_cmd,
      "no encapsulate",
      NO_STR
      "Interface encap format\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int encap = CES_ENCAP_SATOP;

    ifindex = (uint32_t) vty->index;
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    ret = ces_if_info_set(pif, (void *)&encap, CES_INFO_ENCAP);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//配置端口的成帧模式
DEFUN(port_mode,
      port_mode_cmd,
      "mode (unframed | framed | multiframed)",
      "Interface frame mode\n"
      "Unframed mode\n"
      "Framed mode\n"
      "Multiframed mode\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    int mode = CES_MODE_UNFRAMED;
    struct ces_if *pif = NULL;

    ifindex = (uint32_t) vty->index;
    /*ces信息是否已经创建，未创建进行创建*/
    pif = ces_if_create(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_CREATE), VTY_NEWLINE);
        return CES_ERR_CREATE;
    }

    if (!strncmp(argv[0], "f", 1))
    {
        mode = CES_MODE_FRAMED;
    }
    else if (!strncmp(argv[0], "u", 1))
    {
        mode = CES_MODE_UNFRAMED;
    }
    else if (!strncmp(argv[0], "m", 1))
    {
        mode = CES_MODE_MULTIFRAMED;
    }

    ret = ces_if_info_set(pif, (void *)&mode, CES_INFO_MODE);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED),  VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//恢复默认端口的成帧模式
DEFUN(no_port_mode,
      no_port_mode_cmd,
      "no mode",
      NO_STR
      "Interface frame mode\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;

    ifindex = (uint32_t) vty->index;
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    ret = ces_if_delete(ifindex);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_DELETE), VTY_NEWLINE);
        return CES_ERR_DELETE;
    }

    return CMD_SUCCESS;
}


//配置端口的帧格式
DEFUN(frame_format,
      frame_format_cmd,
      "frame (pcm30|pcm31) {crc}",
      "Interface frame format\n"
      "Pcm30 format\n"
      "Pcm31 format\n"
      "Crc\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    int frame = CES_FRAME_PCM31;
    struct ces_if *pif = NULL;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (CES_MODE_UNFRAMED == pif->ces.mode)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_UNFRAM_NO_SET), VTY_NEWLINE);
        return CES_ERR_UNFRAM_NO_SET;
    }

    if (NULL == argv[1])
    {
        if (!strncmp(argv[0], "pcm30", 5))
        {
            frame = CES_FRAME_PCM30;
        }
        else if (!strncmp(argv[0], "pcm31", 5))
        {
            frame = CES_FRAME_PCM31;
        }
    }
    else
    {
        if (!strncmp(argv[0], "pcm30", 5))
        {
            frame = CES_FRAME_PCM30_CRC;
        }
        else if (!strncmp(argv[0], "pcm31", 5))
        {
            frame = CES_FRAME_PCM31_CRC;
        }

    }

    ret = ces_if_info_set(pif, (void *)&frame, CES_INFO_FRAME);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

/*恢复默认帧格式*/
DEFUN(no_frame_format,
      no_frame_format_cmd,
      "no frame",
      NO_STR
      "Interface frame format\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    int frame = CES_FRAME_PCM31;
    struct ces_if *pif = NULL;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (CES_MODE_UNFRAMED == pif->ces.mode)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_UNFRAM_NO_SET), VTY_NEWLINE);
        return CES_ERR_UNFRAM_NO_SET;
    }

    ret = ces_if_info_set(pif, (void *)&frame, CES_INFO_FRAME);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//配置接口或子接口的时隙
DEFUN(port_time_slot,
      port_time_slot_cmd,
      "time-slot <1-31> {to <1-31>}",
      "Interface time slot\n"
      "The value of time slot\n"
      "The end time slot\n"
      "The end value of time slot\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    uint32_t timeslot = 0;
    uint32_t start_timeslot = 0;
    uint32_t end_timeslot = 0;
    uint32_t i;

    ifindex = (uint32_t) vty->index;
    VTY_GET_INTEGER_RANGE("timeslot", start_timeslot, argv[0], 1, 31);

    if (NULL == argv[1])
    {
        timeslot |= (1 << start_timeslot);
    }
    else
    {
        VTY_GET_INTEGER_RANGE("timeslot", end_timeslot, argv[1], 1, 31);

        if (start_timeslot >= end_timeslot)
        {
            vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_TIMESLOT), VTY_NEWLINE);
            return CES_ERR_TIMESLOT;
        }

        for (i = start_timeslot; i <= end_timeslot; i++)
        {
            timeslot |= (1 << i);
        }
    }

    /*时隙是否已经被其他子端口设置*/
    ret = ces_if_timeslot_valid(ifindex, timeslot);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_TIMESLOT_USED), VTY_NEWLINE);
        return CES_ERR_TIMESLOT_USED;
    }

    /*只支持子接口*/
    pif = ces_if_create(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_CREATE), VTY_NEWLINE);
        return CES_ERR_CREATE;
    }

    ces_if_timeslot_set(ifindex, timeslot);

    ret = ces_if_info_set(pif, (void *)&timeslot, CES_INFO_TIMESLOT);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//恢复接口或子接口的默认时隙
DEFUN(no_time_slot,
      no_time_slot_cmd,
      "no time-slot",
      NO_STR
      "Interface time slot\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;

    ifindex = (uint32_t) vty->index;
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        return CMD_SUCCESS;
    }

    ret = ces_if_delete(ifindex);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_DELETE), VTY_NEWLINE);
        return CES_ERR_DELETE;
    }

    return CMD_SUCCESS;
}


//配置端口的 jitter-buffer
DEFUN(port_jitter_buffer,
      port_jitter_buffer_cmd,
      "jitter-buffer <4-255>",
      "Interface jitter buffer\n"
      "The value of jitter buffer\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    uint8_t jitbuffer = 4;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    /*判断输入参数是否满足*/
    VTY_GET_INTEGER_RANGE("jitbuffer", jitbuffer, argv[0], 4, 255);

    ret = ces_if_info_set(pif, (void *)&jitbuffer, CES_INFO_JITBUFFER);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

/*恢复默认端口jitter buffer*/
DEFUN(no_port_jitter_buffer,
      no_port_jitter_buffer_cmd,
      "no jitter-buffer",
      NO_STR
      "Interface jitter buffer\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    uint8_t jitbuffer = 4;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    ret = ces_if_info_set(pif, (void *)&jitbuffer, CES_INFO_JITBUFFER);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}


//配置端口 rtp 使能和禁止
DEFUN(port_rtp,
      port_rtp_cmd,
      "rtp (enable|disable)",
      "Interface rtp\n"
      "Enable rtp of the interface\n"
      "Disable rtp of the interface\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    uint8_t rtp = CES_DISABLE;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (!strncmp(argv[0], "e", 1))
    {
        rtp = CES_ENABLE;
    }
    else if (!strncmp(argv[0], "d", 1))
    {
        rtp = CES_DISABLE;
    }

    ret = ces_if_info_set(pif, (void *)&rtp, CES_INFO_RTP);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//配置端口环回
DEFUN(port_loopback,
      port_loopback_cmd,
      "loopback (internal|external)",
      "Interface loopback\n"
      "Internal loopback\n"
      "External loopback\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int loopback = CES_LB_NOLOOPBACK;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (!strncmp(argv[0], "i", 1))
    {
        loopback = CES_LB_INTERBAL;
    }
    else if (!strncmp(argv[0], "e", 1))
    {
        loopback = CES_LB_EXTERNAL;
    }

    ret = ces_if_info_set(pif, (void *)&loopback, CES_INFO_LOOPBACK);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//配置端口不环回
DEFUN(no_port_loopback,
      no_port_loopback_cmd,
      "no loopback",
      NO_STR
      "Interface loopback\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int loopback = CES_LB_NOLOOPBACK;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    ret = ces_if_info_set(pif, (void *)&loopback, CES_INFO_LOOPBACK);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//配置端口时钟模式
DEFUN(port_clock,
      port_clock_cmd,
      "clock-send (loopback|local|acr|dcr|phy)",
      "Interface send clock\n"
      "Loopback clock\n"
      "Local clock\n"
      "Acr clock\n"
      "Dcr clock\n"
      "Phy clock\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int clock = CES_CLK_ACR;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (!strncmp(argv[0], "loo", 3))
    {
        clock = CES_CLK_LOOPBACK;
    }
    else if (!strncmp(argv[0], "loc", 3))
    {
        clock = CES_CLK_LOCAL;
    }
    else if (!strncmp(argv[0], "a", 1))
    {
        clock = CES_CLK_ACR;
    }
    else if (!strncmp(argv[0], "d", 1))
    {
        clock = CES_CLK_DCR;
    }
    else if (!strncmp(argv[0], "p", 1))
    {
        clock = CES_CLK_PHY;
    }

    ret = ces_if_info_set(pif, (void *)&clock, CES_INFO_SENDCLOCK);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//恢复默认发送时钟
DEFUN(no_port_clock,
      no_port_clock_cmd,
      "no clock-send",
      NO_STR
      "Interface send clock\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int clock = CES_CLK_ACR;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    ret = ces_if_info_set(pif, (void *)&clock, CES_INFO_SENDCLOCK);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//配置端口时钟模式
DEFUN(port_recvclock,
      port_recvclock_cmd,
      "clock-receive (line|external-sync|local)",
      "Interface receive clock\n"
      "Line clock\n"
      "External synchronization line clock\n"
      "Local clock\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int clock = CES_RECEIVE_CLK_LINE;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (!strncmp(argv[0], "li", 2))
    {
        clock = CES_RECEIVE_CLK_LINE;
    }
    else if (!strncmp(argv[0], "e", 1))
    {
        clock = CES_RECEIVE_CLK_EXTERNAL;
    }
    else if (!strncmp(argv[0], "lo", 1))
    {
        clock = CES_RECEIVE_CLK_LOCAL;
    }

    ret = ces_if_info_set(pif, (void *)&clock, CES_INFO_RECVCLOCK);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}

//恢复默认接收时钟
DEFUN(no_port_recvclock,
      no_port_recvclock_cmd,
      "no clock-receive",
      NO_STR
      "Interface receive clock\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    int clock = CES_RECEIVE_CLK_LINE;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    ret = ces_if_info_set(pif, (void *)&clock, CES_INFO_RECVCLOCK);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}


//配置端口 prbs
DEFUN(port_prbs,
      port_prbs_cmd,
      "prbs (enable|disable)",
      "Interface prbs\n"
      "Enable prbs of the interface\n"
      "Disable prbs of the interface\n"
     )
{
    int ret = 0;
    struct ces_if *pif = NULL;
    uint32_t ifindex = 0;
    uint8_t prbs = CES_DISABLE;
    struct hash_bucket *pbucket = NULL;
    int cursor;
    struct ces_if *pif_tmp = NULL;

    ifindex = (uint32_t) vty->index;
    /*需先设置mode命令，查找不到，提示设置mode*/
    pif = ces_if_lookup(ifindex);
    if (NULL == pif)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_MODE_NO_SET), VTY_NEWLINE);
        return CES_ERR_MODE_NO_SET;
    }

    if (!strncmp(argv[0], "e", 1))
    {
        prbs = CES_ENABLE;
    }
    else if (!strncmp(argv[0], "d", 1))
    {
        prbs = CES_DISABLE;
    }

    /*其它端口是否已经使能prbs,已经使能不允许再设置其它口*/
    if (CES_ENABLE == prbs)
    {
        HASH_BUCKET_LOOP(pbucket, cursor, ces_if_table)
        {
            pif_tmp = pbucket->data;

            if (CES_ENABLE == pif_tmp->ces.prbs)
            {
                if (pif_tmp->ifindex != ifindex)
                {
                    vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_PRBS_USED), VTY_NEWLINE);
                    return CES_ERR_PRBS_USED;
                }
            }
        }
    }

    ret = ces_if_info_set(pif, (void *)&prbs, CES_INFO_PRBS);
    if (ret != 0)
    {
        vty_error_out(vty, "%s %s", errcode_get_string(CES_ERR_EXE_FAILED), VTY_NEWLINE);
        return CES_ERR_EXE_FAILED;
    }

    return CMD_SUCCESS;
}
     
/**
 * @brief      : 配置E1通道开销，低阶通道踪迹字节J2。 
 * @param[in ] : j2低阶通道踪迹字节。SDH帧格式下j2-value的取值范围为1～15个字符的字符串
 * @param[in ] : padding-zero可选，字符补齐方式，填充0，默认填充空格
 * @return     : 成功返回 CMD_SUCCESS，否则返回错误码 CMD_XXX
 * @author     : liub
 * @date       : 2018-6-25
 * @note       : TDM 物理接口视图
                 系统使用SDH帧格式的缺省值，SDH帧格式下再生段踪迹字节J0的缺省值为“cccccccccccccc”
 */
DEFUN(ces_oh_j2,
    ces_oh_j2_cmd,
    "oh j2 sdh J2-VALUE { padding-zero }",
    "Overhead\n"
    "J2\n"
    "Default frame format: SDH\n"
    "Lower-Order VC-N path trace byte j2: 1-15 characters\n"
    "Padding character, space or zero\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    uint16_t length = 0;
    struct ces_if *pif = NULL;
    char str[STM_OVERHEAD_LEN+1] = "";
    char j2[STM_OVERHEAD_LEN+1] = "";
    uint8_t crc = 0x00;
    int n = 0;
    
    length = strlen(argv[0]);
    if(length > STM_OVERHEAD_LEN)
    {
        vty_error_out(vty, "ERROR: j2 is too long, maximum length is 15 characters! %s", VTY_NEWLINE);
        return -1;
    }

    ifindex = (uint32_t)vty->index;

    pif = ces_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out ( vty, "Please first set mode%s", VTY_NEWLINE );
        return CMD_WARNING;
    }
    
    memcpy(str, argv[0], length);
    n = STM_OVERHEAD_LEN - length;      //需要补齐的位数

    if(length < STM_OVERHEAD_LEN)
    {
        if(argv[1] != NULL)
        {
            memset(str + length, 0x00, n);   //0 补齐
            pif->j2_pad = 1;
        }
        else
        {
            memset(str + length, 0x20, n);   //空格补齐
        }
    }

    //获取crc并将其放在j2的第0位，输入的字符放在后15位
    crc = get_crc8((uint8_t *)str, 15) | 0x80;
    j2[0] = crc;
    memcpy(j2 + 1, str, STM_OVERHEAD_LEN);

    ret = ces_if_info_set(pif, j2, CES_INFO_J2);
    if(ret != 0)
    {
        vty_error_out(vty, "set oh j2 failed %s", VTY_NEWLINE);
        return CMD_WARNING;
    }
        
    return CMD_SUCCESS;
}

/*default j2 is fifteen 'c': 'ccccccccccccccc'*/
DEFUN(ces_no_oh_j2,
    ces_no_oh_j2_cmd,
    "no oh j2",
    NO_STR
    "Overhead\n"
    "J2\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    struct ces_if *pif = NULL;
    char str[STM_OVERHEAD_LEN+1] = "";
    char j2[STM_OVERHEAD_LEN+1] = "";
    uint8_t crc = 0x00;
    
    memset(str, 'c', 15);
    ifindex = (uint32_t)vty->index;
    pif = ces_if_lookup(ifindex);
    if (pif == NULL)
    {
        vty_error_out(vty, "This interface not exist! %s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    //获取crc并将其放在j2的第0位，输入的字符放在后15位
    crc = get_crc8((uint8_t *)str, 15) | 0x80;
    j2[0] = crc;
    memcpy(j2 + 1, str, STM_OVERHEAD_LEN);

    ret = ces_if_info_set(pif, j2, CES_INFO_J2);
    if(ret != 0)
    {
        vty_error_out(vty, "no oh j2 failed %s", ret, VTY_NEWLINE);
        return CMD_WARNING;
    }
    
    pif->j2_pad = 0;
        
    return CMD_SUCCESS;
}

/*显示特定接口的告警信息*/
DEFUN(show_ifm_tdm, show_ifm_tdm_cmd,
      "show interface tdm USP",
      SHOW_STR
      CLI_INTERFACE_STR
      CLI_INTERFACE_TDM_STR
      CLI_INTERFACE_TDM_VHELP_STR)
{
    uint32_t ifindex = 0;
    struct ces_if *pces = NULL;

    ifindex = ifm_get_ifindex_by_name("tdm", (char *) argv[0]);
    pces = ces_if_lookup(ifindex);

    if (!pces)
    {
        return CMD_WARNING;
    }
    
    ces_alarm_info_show(vty, pces);
    
    return CMD_SUCCESS;
}

/*显示所有tdm接口的告警信息*/
DEFUN(show_ifm_all_tdm, show_ifm_tdm_all_cmd,
      "show interface tdm ",
      SHOW_STR
      CLI_INTERFACE_STR
      CLI_INTERFACE_TDM_STR)
{
    return CMD_SUCCESS;
}

/*显示特定接口的配置信息*/
DEFUN(show_ifm_tdm_config, show_ifm_tdm_config_cmd,
      "show interface config tdm USP",
      SHOW_STR
      CLI_INTERFACE_STR
      "Interface all config\n"
      CLI_INTERFACE_TDM_STR
      CLI_INTERFACE_TDM_VHELP_STR)
{
    uint32_t ifindex = 0;
    struct ces_if *pces = NULL;

    ifindex = ifm_get_ifindex_by_name("tdm", (char *) argv[0]);
    pces = ces_if_lookup(ifindex);
    if (!pces)
    {
        return CMD_WARNING;
    }
    
    ces_config_info_show(vty, pces);
    return CMD_SUCCESS;
}

/*显示所有tdm接口的配置信息*/
DEFUN(show_ifm_tdm_all_config, show_ifm_tdm_all_config_cmd,
      "show interface config tdm",
      SHOW_STR
      CLI_INTERFACE_STR
      "Interface all config\n"
      CLI_INTERFACE_TDM_STR)
{
    return CMD_SUCCESS;
}

DEFUN (ces_conf_debug,
       ces_conf_debug_cmd,
       "debug ces (enable|disable) (all|tdm|stm|other)",
       "Debuv config\n"
       "Ces config\n"
       "Ces debug enable\n"
       "Ces debug disable\n"
       "Ces debug type all\n"
       "Ces debug type tdm\n"
       "Ces debug type stm\n"
       "Ces debug type other\n")
{
    int enable = 0;
    unsigned int type = 0;

    if(argv[0][0] == 'e') enable = 1;

    if(strcmp(argv[1],"other") == 0)
        type = CES_DEBUG_OTHER;
    else if(strcmp(argv[1],"tdm") == 0)
        type = CES_DEBUG_TDM;
    else if(strcmp(argv[1],"stm") == 0)
        type = CES_DEBUG_STM;
    else type = CES_DEBUG_ALL;

    zlog_debug_set(vty, type,  enable);

    return(CMD_SUCCESS);
}

/* 写配置文件 */
static int ces_config_write(struct vty *vty)
{
    struct ces_if *pif = NULL;
    struct hash_bucket *pbucket = NULL;
    char ifname[IFNET_NAMESIZE];
    uint32_t ifindex;
    int cursor;
    int i, ret = 0;
    int flag = 0, start_timeslot = 0, end_timeslot = 0;

    HASH_BUCKET_LOOP(pbucket, cursor, ces_if_table)
    {
        pif = pbucket->data;
        ifindex = pif->ifindex;
        ret = ifm_get_name_by_ifindex(ifindex, ifname);
        if (ret < 0)
        {
            //assert(0);
            continue;
        }

        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        if (IFM_IS_SUBPORT(ifindex))
        {
            if (pif->ces.timeslot != 0)
            {
                flag = 0;

                for (i = 1; i <= 31; i++)
                {
                    if (pif->ces.timeslot & (1 << i))
                    {
                        if (!flag)
                        {
                            start_timeslot = i;
                            flag = 1;
                        }

                        end_timeslot = i;
                    }
                }

                if (start_timeslot == end_timeslot)
                {
                    vty_out(vty, " time-slot %d%s", start_timeslot, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " time-slot %d to %d%s", start_timeslot, end_timeslot, VTY_NEWLINE);
                }
            }

        }
        else
        {
            if (pif->ces.mode != CES_MODE_UNFRAMED)
            {
                if (CES_MODE_FRAMED == pif->ces.mode)
                {
                    vty_out(vty, " mode framed%s", VTY_NEWLINE);
                }
                else if (CES_MODE_MULTIFRAMED == pif->ces.mode)
                {
                    vty_out(vty, " mode multiframed%s", VTY_NEWLINE);
                }

                if (CES_FRAME_PCM30 == pif->ces.frame)
                {
                    vty_out(vty, " frame pcm30%s", VTY_NEWLINE);
                }
                else if (CES_FRAME_PCM30_CRC == pif->ces.frame)
                {
                    vty_out(vty, " frame pcm30 crc%s", VTY_NEWLINE);
                }
                else if (CES_FRAME_PCM31_CRC == pif->ces.frame)
                {
                    vty_out(vty, " frame pcm31 crc%s", VTY_NEWLINE);
                }
            }
            else
            {
                vty_out(vty, " mode unframed%s", VTY_NEWLINE);
            }

            if (CES_ENCAP_CESOPSN == pif->ces.encap)
            {
                vty_out(vty, " encapsulate cesopsn%s", VTY_NEWLINE);
            }

            if (CES_CLK_LOCAL == pif->ces.clock)
            {
                vty_out(vty, " clock-send local%s", VTY_NEWLINE);
            }
            else if (CES_CLK_LOOPBACK == pif->ces.clock)
            {
                vty_out(vty, " clock-send loopback%s", VTY_NEWLINE);
            }
            else if (CES_CLK_DCR == pif->ces.clock)
            {
                vty_out(vty, " clock-send dcr%s", VTY_NEWLINE);
            }
            else if (CES_CLK_PHY == pif->ces.clock)
            {
                vty_out(vty, " clock-send phy%s", VTY_NEWLINE);
            }

            if (CES_RECEIVE_CLK_EXTERNAL == pif->ces.recvclock)
            {
                vty_out(vty, " clock-receive external-sync%s", VTY_NEWLINE);
            }
            else if (CES_RECEIVE_CLK_LOCAL == pif->ces.recvclock)
            {
                vty_out(vty, " clock-receive local%s", VTY_NEWLINE);
            }

            if (pif->ces.jitbuffer > 4)
            {
                vty_out(vty, " jitter-buffer %d%s", pif->ces.jitbuffer, VTY_NEWLINE);
            }

            if (CES_ENABLE == pif->ces.rtp)
            {
                vty_out(vty, " rtp enable%s", VTY_NEWLINE);
            }
            
            if(strncmp(pif->ces.j2, STM_OVERHEAD_J2, STM_OVERHEAD_LEN))
            {
                if(pif->j2_pad)
                {
                    vty_out(vty, " oh j2 sdh %s padding-zero%s", pif->ces.j2, VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, " oh j2 sdh %s%s", pif->ces.j2, VTY_NEWLINE);
                }
            }
        }

        vty_out(vty, "!%s", VTY_NEWLINE);
    }

    return 0;
}

void ces_alarm_info_show(struct vty *vty,  struct ces_if *pif)
{
    uint8_t slot = 0;
    uint8_t port = 0;
    struct ces_count_info *ces_count = NULL;

    if (NULL == pif)
    {
        return;
    }
    
    /*show tdm port 1~4 only while image type is stm1*/
    slot = IFM_SLOT_ID_GET(pif->ifindex);
    port = IFM_PORT_ID_GET(pif->ifindex);
    if(g_stm_slot[slot].slot_id == ID_SC01QE 
        && g_stm_slot[slot].image_type == STM_IMAGE_TYPE_STM1
        && port > 4)
    {
        return;
    }

    /*告警信息获取并显示*/
    vty_out(vty, " E1 alarm:");

    if (0 == pif->ces.ces_alarm.los_ais_local)
    {
        vty_out(vty, " Los");
    }
    else if (3 == pif->ces.ces_alarm.los_ais_local)
    {
        vty_out(vty, " Ais");
    }

    if (pif->ces.ces_alarm.los_ais_local != 0)
    {
        /*成帧模式下显示*/
        if (pif->ces.mode == CES_MODE_FRAMED)
        {
            if (1 == pif->ces.ces_alarm.e1_lof)
            {
                vty_out(vty, " Lof");
            }
        }

        /*成复帧模式下显示，不支持*/
        if (pif->ces.mode == CES_MODE_MULTIFRAMED)
        {
            if (1 == pif->ces.ces_alarm.e1_lomf)
            {
                vty_out(vty, " Lofm");
            }

            if (1 == pif->ces.ces_alarm.e1_crcerr)
            {
                vty_out(vty, " Crc");
            }
        }
    }

    if (1 == pif->ces.ces_alarm.pck_fulllos)
    {
        vty_out(vty, " Nopacket");
    }

    if (1 == pif->ces.ces_alarm.pck_los)
    {
        vty_out(vty, " PacketLoss");
    }

    if (1 == pif->ces.ces_alarm.pck_fulllos_r)
    {
        vty_out(vty, " Remote-PacketLoss");
    }

    if (1 == pif->ces.ces_alarm.e1_los_r)
    {
        vty_out(vty, " RemoteErr");
    }

    if (1 == pif->ces.ces_alarm.e1_hdbe_err)
    {
        vty_out(vty, " Hdb3/B8zs");
    }

    vty_out(vty, "%s", VTY_NEWLINE);

    ces_count = ces_if_get_count(pif->ifindex);

    if (ces_count != NULL)
    {
        if (CES_ENABLE == pif->ces.prbs)
        {
            vty_out(vty, " Prbs count:%u%s", ces_count->prbs_count, VTY_NEWLINE);
        }

        vty_out(vty, " Hdb3 error:%u%s", ces_count->hdb3_count, VTY_NEWLINE);
        vty_out(vty, " Es:%u%s", ces_count->es_count, VTY_NEWLINE);
        vty_out(vty, " Ses:%u%s", ces_count->ses_count, VTY_NEWLINE);
        vty_out(vty, " Uas:%u%s", ces_count->uas_count, VTY_NEWLINE);

        if (ces_count->clock_sync_count <= 20)
        {
            vty_out(vty, " Sync-clock:holdover%s", VTY_NEWLINE);
        }
        else if (ces_count->clock_sync_count <= 80)
        {
            vty_out(vty, " Sync-clock:locked%s", VTY_NEWLINE);

        }
        else
        {
            vty_out(vty, " Sync-clock:free-run%s", VTY_NEWLINE);
        }
    }

    return;
}


void ces_config_info_show(struct vty *vty,  struct ces_if *pif)
{
    int i;
    int flag = 0, start_timeslot = 0, end_timeslot = 0;
    uint8_t slot = 0;
    uint8_t port = 0;

    if (!pif)
    {
        return;
    }
    
    /*show tdm port 1~4 only while image type is stm1*/
    slot = IFM_SLOT_ID_GET(pif->ifindex);
    port = IFM_PORT_ID_GET(pif->ifindex);
    if(g_stm_slot[slot].slot_id == ID_SC01QE 
        && g_stm_slot[slot].image_type == STM_IMAGE_TYPE_STM1
        && port > 4)
    {
        return;
    }

    if (IFM_IS_SUBPORT(pif->ifindex))
    {
        /*时隙*/
        if (pif->ces.timeslot != 0)
        {
            flag = 0;

            for (i = 1; i <= 31; i++)
            {
                if (pif->ces.timeslot & (1 << i))
                {
                    if (!flag)
                    {
                        start_timeslot = i;
                        flag = 1;
                    }

                    end_timeslot = i;
                }
            }

            if (start_timeslot == end_timeslot)
            {
                vty_out(vty, " time slot : %d%s", start_timeslot, VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, " time slot : %d-%d%s", start_timeslot, end_timeslot, VTY_NEWLINE);
            }
        }

        return ;
    }

    if (pif->ces.mode != CES_MODE_UNFRAMED)
    {
        if (CES_MODE_FRAMED == pif->ces.mode)
        {
            vty_out(vty, " mode : framed%s", VTY_NEWLINE);
        }
        else if (CES_MODE_MULTIFRAMED == pif->ces.mode)
        {
            vty_out(vty, " mode : multiframed%s", VTY_NEWLINE);
        }

        /*成帧格式*/
        if (CES_FRAME_PCM30 == pif->ces.frame)
        {
            vty_out(vty, " frame : pcm30%s", VTY_NEWLINE);
        }
        else if (CES_FRAME_PCM30_CRC == pif->ces.frame)
        {
            vty_out(vty, " frame : pcm30 crc%s", VTY_NEWLINE);
        }
        else if (CES_FRAME_PCM31 == pif->ces.frame)
        {
            vty_out(vty, " frame : pcm31%s", VTY_NEWLINE);
        }
        else if (CES_FRAME_PCM31_CRC == pif->ces.frame)
        {
            vty_out(vty, " frame : pcm31 crc%s", VTY_NEWLINE);
        }
    }
    else
    {
        vty_out(vty, " mode : unframed%s", VTY_NEWLINE);
    }

    if (CES_ENCAP_SATOP == pif->ces.encap)
    {
        vty_out(vty, " encapsulate : satop%s", VTY_NEWLINE);
    }
    else if (CES_ENCAP_CESOPSN == pif->ces.encap)
    {
        vty_out(vty, " encapsulate : cesopsn%s", VTY_NEWLINE);
    }

    if (CES_CLK_LOCAL == pif->ces.clock)
    {
        vty_out(vty, " send clock : local%s", VTY_NEWLINE);
    }
    else if (CES_CLK_LOOPBACK == pif->ces.clock)
    {
        vty_out(vty, " send clock : loopback%s", VTY_NEWLINE);
    }
    else if (CES_CLK_ACR == pif->ces.clock)
    {
        vty_out(vty, " send clock : acr%s", VTY_NEWLINE);
    }
    else if (CES_CLK_DCR == pif->ces.clock)
    {
        vty_out(vty, " send clock : dcr%s", VTY_NEWLINE);
    }
    else if (CES_CLK_PHY == pif->ces.clock)
    {
        vty_out(vty, " send clock : phy%s", VTY_NEWLINE);
    }

    if (CES_RECEIVE_CLK_LINE == pif->ces.recvclock)
    {
        vty_out(vty, " receive clock : line%s", VTY_NEWLINE);
    }
    else if (CES_RECEIVE_CLK_EXTERNAL == pif->ces.recvclock)
    {
        vty_out(vty, " receive clock : external-sync%s", VTY_NEWLINE);
    }
    else if (CES_RECEIVE_CLK_LOCAL == pif->ces.recvclock)
    {
        vty_out(vty, " receive clock : local%s", VTY_NEWLINE);
    }

    if (pif->ces.jitbuffer >= 4)
    {
        vty_out(vty, " jitter buffer : %d%s", pif->ces.jitbuffer, VTY_NEWLINE);
    }

    if (CES_ENABLE == pif->ces.rtp)
    {
        vty_out(vty, " rtp : enable%s", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, " rtp : disable%s", VTY_NEWLINE);
    }

    if (CES_ENABLE == pif->ces.prbs)
    {
        vty_out(vty, " prbs : enable%s", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, " prbs : disable%s", VTY_NEWLINE);
    }

    if (CES_LB_INTERBAL == pif->ces.loopback)
    {
        vty_out(vty, " loopback : internal%s", VTY_NEWLINE);
    }
    else if (CES_LB_EXTERNAL == pif->ces.loopback)
    {
        vty_out(vty, " loopback : external%s", VTY_NEWLINE);
    }

    return;
}

/* install all cmd */
void ces_cmd_init(void)
{
    zlog_debug(CES_DEBUG_OTHER, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    ifm_ces_init(ces_config_write);

    //ces特有命令进行注册
    install_element(TDM_IF_NODE, &port_encap_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_port_encap_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_mode_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_port_mode_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &frame_format_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_frame_format_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_jitter_buffer_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_port_jitter_buffer_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_rtp_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_loopback_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_port_loopback_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_clock_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_port_clock_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_recvclock_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &no_port_recvclock_cmd, CMD_SYNC);
    install_element(TDM_IF_NODE, &port_prbs_cmd, CMD_SYNC);
    install_element (TDM_IF_NODE, &ces_oh_j2_cmd, CMD_SYNC);    
    install_element (TDM_IF_NODE, &ces_no_oh_j2_cmd, CMD_SYNC);

    install_element(TDM_SUBIF_NODE, &port_time_slot_cmd, CMD_SYNC);
    install_element(TDM_SUBIF_NODE, &no_time_slot_cmd, CMD_SYNC);

    //公共命令进行注册
    install_element(CONFIG_NODE, &show_ifm_tdm_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ifm_tdm_config_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ifm_tdm_all_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_ifm_tdm_all_config_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &ces_conf_debug_cmd, CMD_LOCAL);

    zlog_debug(CES_DEBUG_OTHER, "%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
}







