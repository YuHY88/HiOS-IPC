
#include <lib/memory.h>
#include <lib/memtypes.h>
#include <lib/zassert.h>
#include <lib/ifm_common.h>
#include <lib/command.h>
#include <lib/prefix.h>
#include <lib/msg_ipc.h>
#include <lib/errcode.h>
#include <lib/log.h>
#include <lib/sdh_vcg_common.h>
#include <lib/crc8.h>
#include "lib/devm_com.h"
#include <ifm/ifm.h>
#include <ftm/ftm_nhp.h>

#include "sdhvcg_if.h"
#include "sdhvcg_cmd.h"

extern void vcg_binding_show(struct vty *vty, t_sdh_vcg_interface *pvcg);
extern void vcg_config_show(struct vty *vty, t_sdh_vcg_interface *pvcg);


DEFUN(physical_vcg_if,
      physical_vcg_if_cmd,
      "interface sdhvcg USP",
      CLI_INTERFACE_STR
      CLI_INTERFACE_VCG_STR
      CLI_INTERFACE_VCG_VHELP_STR)
{
    int ifindex = 0;
    char *pprompt = NULL;
    t_sdh_vcg_interface *pvcg = NULL;
	
    ifindex = ifm_get_ifindex_by_name("sdhvcg", (char *)argv[0]);

    pvcg = vcg_if_lookup(ifindex);
    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    vty->node = VCG_NODE;
    pprompt = vty->change_prompt;

    if (pprompt)
    {
        /*format the prompt*/
        snprintf(pprompt, VTY_BUFSIZ, "%%s (config-sdhvcg %d/%d/%d)#",
                 IFM_UNIT_ID_GET(ifindex),
                 IFM_SLOT_ID_GET(ifindex),
                 IFM_PORT_ID_GET(ifindex));
    }

    vty->index = (void *)ifindex;

    return CMD_SUCCESS;
}



DEFUN(binding_vcg4,
      binding_vcg4_cmd,
      "binding vc4 (up|down|both) vc4 <1-128> num <1-32>",
      "binding vcg\n"
      "vc4 level\n"
      "up direction\n"
      "down direction\n"
      "both direction\n"
      "vc4 serial number\n"
      "the range of the vc4 serial number\n"
      "the numbers of binding\n"
      "the range of binding number\n")
{
    int                  ret = 0;
    int                  num = 1;
    int                  direct = SDH_VCG_BINDING_BOTH_DIRECTION;
    uint32_t             ifindex = 0;
    int                  vc4_sequence = 1;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    vcgif.config.level = SDH_VCG_LEVEL_VC4;

    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "up", 2) == 0)
        {
            direct = SDH_VCG_BINDING_UP_DIRECTION;
        }
        else if (strncmp(argv[0], "down", 4) == 0)
        {
            direct = SDH_VCG_BINDING_DOWN_DIRECTION;
        }
        else if (strncmp(argv[0], "both", 4) == 0)
        {
            direct = SDH_VCG_BINDING_BOTH_DIRECTION;
        }
    }

    VTY_GET_INTEGER_RANGE("vc4", vc4_sequence, argv[1], 1, 128);
    VTY_GET_INTEGER_RANGE("num", num, argv[2], 1, 32);

    if (direct == SDH_VCG_BINDING_UP_DIRECTION || direct == SDH_VCG_BINDING_BOTH_DIRECTION)
    {
        vcgif.config.binding_up.vc4 = vc4_sequence;
        vcgif.config.binding_up.num = num;
    }

    if (direct == SDH_VCG_BINDING_DOWN_DIRECTION || direct == SDH_VCG_BINDING_BOTH_DIRECTION)
    {
        vcgif.config.binding_down.vc4 = vc4_sequence;
        vcgif.config.binding_down.num = num;
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_BINDING);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg binding set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(binding_vcg3,
      binding_vcg3_cmd,
      "binding vc3 (up|down|both) vc4 <1-128> vc3 <1-3> num <1-96>",
      "binding vcg\n"
      "vc3 level\n"
      "up direction\n"
      "down direction\n"
      "both direction\n"
      "vc4 serial number\n"
      "the range of the vc4 serial number\n"
      "vc3 serial number\n"
      "the range of the vc3 serial number\n"
      "the numbers of binding\n"
      "the range of binding number\n")
{
    int                  ret = 0;
    int                  num = 1;
    int                  direct = SDH_VCG_BINDING_BOTH_DIRECTION;
    uint32_t             ifindex = 0;
    int                  vc4_sequence = 1;
    int                  vc3_sequence = 1;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    vcgif.config.level = SDH_VCG_LEVEL_VC3;

    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "up", 2) == 0)
        {
            direct = SDH_VCG_BINDING_UP_DIRECTION;
        }
        else if (strncmp(argv[0], "down", 4) == 0)
        {
            direct = SDH_VCG_BINDING_DOWN_DIRECTION;
        }
        else if (strncmp(argv[0], "both", 4) == 0)
        {
            direct = SDH_VCG_BINDING_BOTH_DIRECTION;
        }
    }

    VTY_GET_INTEGER_RANGE("vc4", vc4_sequence, argv[1], 1, 128);
    VTY_GET_INTEGER_RANGE("vc3", vc3_sequence, argv[2], 1, 3);
    VTY_GET_INTEGER_RANGE("num", num, argv[3], 1, 96);

    if (direct == SDH_VCG_BINDING_UP_DIRECTION || direct == SDH_VCG_BINDING_BOTH_DIRECTION)
    {
        vcgif.config.binding_up.vc4 = vc4_sequence;
        vcgif.config.binding_up.vc3_vc12 = vc3_sequence;
        vcgif.config.binding_up.num = num;
    }

    if (direct == SDH_VCG_BINDING_DOWN_DIRECTION || direct == SDH_VCG_BINDING_BOTH_DIRECTION)
    {
        vcgif.config.binding_down.vc4 = vc4_sequence;
        vcgif.config.binding_down.vc3_vc12 = vc3_sequence;
        vcgif.config.binding_down.num = num;
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_BINDING);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg binding set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(binding_vcg12,
      binding_vcg12_cmd,
      "binding vc12 (up|down|both) vc4 <1-128> vc12 <1-63> num <1-63>",
      "binding vcg\n"
      "vc12 level\n"
      "up direction\n"
      "down direction\n"
      "both direction\n"
      "vc4 serial number\n"
      "the range of the vc4 serial number\n"
      "vc12 serial number\n"
      "the range of the vc12 serial number\n"
      "the numbers of binding\n"
      "the range of binding number\n")
{
    int                  ret = 0;
    int                  num = 1;
    int                  direct = SDH_VCG_BINDING_BOTH_DIRECTION;
    uint32_t             ifindex = 0;
    int                  vc4_sequence = 1;
    int                  vc12_sequence = 1;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    vcgif.config.level = SDH_VCG_LEVEL_VC12;

    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "up", 2) == 0)
        {
            direct = SDH_VCG_BINDING_UP_DIRECTION;
        }
        else if (strncmp(argv[0], "down", 4) == 0)
        {
            direct = SDH_VCG_BINDING_DOWN_DIRECTION;
        }
        else if (strncmp(argv[0], "both", 4) == 0)
        {
            direct = SDH_VCG_BINDING_BOTH_DIRECTION;
        }
    }

    VTY_GET_INTEGER_RANGE("vc4", vc4_sequence, argv[1], 1, 128);
    VTY_GET_INTEGER_RANGE("vc12", vc12_sequence, argv[2], 1, 63);
    VTY_GET_INTEGER_RANGE("num", num, argv[3], 1, 63);

    if (direct == SDH_VCG_BINDING_UP_DIRECTION || direct == SDH_VCG_BINDING_BOTH_DIRECTION)
    {
        vcgif.config.binding_up.vc4 = vc4_sequence;
        vcgif.config.binding_up.vc3_vc12 = vc12_sequence;
        vcgif.config.binding_up.num = num;
    }

    if (direct == SDH_VCG_BINDING_DOWN_DIRECTION || direct == SDH_VCG_BINDING_BOTH_DIRECTION)
    {
        vcgif.config.binding_down.vc4 = vc4_sequence;
        vcgif.config.binding_down.vc3_vc12 = vc12_sequence;
        vcgif.config.binding_down.num = num;
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_BINDING);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg binding set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(delete_vcg_binding,
      delete_vcg_binding_cmd,
      "no binding",
      NO_STR
      "binding vcg\n")
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;
    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(&pvcg->config.binding_up, 0, sizeof(t_sdh_vcg_binding));
    memset(&pvcg->config.binding_down, 0, sizeof(t_sdh_vcg_binding));

    ret = vcg_if_info_set(pvcg, VCG_INFO_BINDING);

    if (ret != 0)
    {
        vty_error_out(vty, "delete vcg binding set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    return CMD_SUCCESS;
}

DEFUN(show_vcg_binding,
      show_vcg_binding_cmd,
      "show binding",
      SHOW_STR
      "binding vcg\n"
     )
{
    uint32_t ifindex = 0;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;
    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vcg_binding_show(vty, pvcg);

    return CMD_SUCCESS;
}


DEFUN(set_vcg_jx_oh,
      set_vcg_jx_oh_cmd,
      "oh jx (transmit|expect) DISPLAYSTRING {padding_zero}",
      "set transmit oh\n"
      "set expect receive\n"
      "displaystring\n")
{
    int ret = 0;
    int num = 0;
    uchar crc = 0x00;
    int tra_exp_flag = 0;
    uint32_t ifindex = 0;
    char string[SDH_OH_JX_LEN];
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "transmit", 8) == 0)
        {
            tra_exp_flag = SDH_VCG_TRANSMIT;
        }
        else if (strncmp(argv[0], "expect", 6) == 0)
        {
            tra_exp_flag = SDH_VCG_EXPECT;
        }
    }

    num = strlen(argv[1]);

    if (num == 0 || num >= SDH_OH_JX_LEN)
    {
        vty_error_out(vty, "length is more than 15%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memset(string, 0, SDH_OH_JX_LEN);
    snprintf(string, SDH_OH_JX_LEN, "%s", argv[1]);

    if (num + 1 != SDH_OH_JX_LEN)
    {
        if (argv[2] != NULL)
        {
            memset(string + num, 0x00, 15 - num);
        }
        else
        {
            memset(string + num, 0x20, 15 - num); // space
        }
    }

    crc = get_crc8((uint8_t *)string, 15) | 0x80;

    if (SDH_VCG_TRANSMIT == tra_exp_flag)
    {
        vcgif.config.tra_jx[0] = crc;
        memcpy(vcgif.config.tra_jx + 1, string, 15);

        ret = vcg_if_info_set(&vcgif, VCG_INFO_JX_TRANSMIT);
    }
    else
    {
        vcgif.config.exp_jx[0] = crc;
        memcpy(vcgif.config.exp_jx + 1, string, 15);

        ret = vcg_if_info_set(&vcgif, VCG_INFO_JX_EXPECT);
    }

    if (ret != 0)
    {
        vty_error_out(vty, "son/poh j0/j1/j2 set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(set_scrambling,
      set_scrambling_cmd,
      "scrambling (up|down|both|none)",
      "scrambling direction\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "up", 2) == 0)
        {
            vcgif.config.scrambling = SDH_VCG_SCRAMBLING_UP;
        }
        else if (strncmp(argv[0], "down", 4) == 0)
        {
            vcgif.config.scrambling = SDH_VCG_SCRAMBLING_DOWN;
        }
        else if (strncmp(argv[0], "both", 4) == 0)
        {
            vcgif.config.scrambling = SDH_VCG_SCRAMBLING_BOTH;
        }
        else if (strncmp(argv[0], "none", 4) == 0)
        {
            vcgif.config.scrambling = SDH_VCG_SCRAMBLING_NONE;
        }
    }
    else
    {
        vcgif.config.scrambling = SDH_VCG_SCRAMBLING_BOTH;
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_SCRAMBLING);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg scrambling set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(recover_scrambling,
      recover_scrambling_cmd,
      "no scrambling",
      NO_STR
      "default scrambling both\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    vcgif.config.scrambling = SDH_VCG_SCRAMBLING_NONE;

    ret = vcg_if_info_set(&vcgif, VCG_INFO_SCRAMBLING);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg scrambling set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(set_fcs_insert,
      set_fcs_insert_cmd,
      "fcs insert (enable|disable)",
      "enable up direction fcs insert\n"
      "disable up direction fcs insert\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "enable", 6) == 0)
        {
            vcgif.config.fcs_insert = VCG_ENABLE;
        }
        else if (strncmp(argv[0], "disable", 7) == 0)
        {
            vcgif.config.fcs_insert = VCG_DISABLE;
        }
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_FCS);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg fcs insert set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(recover_fcs_insert,
      recover_fcs_insert_cmd,
      "no fcs insert",
      NO_STR
      "default enable up direction fcs insert\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    vcgif.config.fcs_insert = VCG_ENABLE;

    ret = vcg_if_info_set(&vcgif, VCG_INFO_FCS);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg fcs insert set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}


DEFUN(set_lcas,
      set_lcas_cmd,
      "lcas (enable|disable)",
      "enable lcas\n"
      "disable lcas\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));


    if (argv[0] != NULL)
    {
        if (strncmp(argv[0], "enable", 6) == 0)
        {
            vcgif.config.lcas_enable = VCG_ENABLE;
        }
        else if (strncmp(argv[0], "disable", 7) == 0)
        {
            vcgif.config.lcas_enable = VCG_DISABLE;
        }
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_LCAS_ENA);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg lcas set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}

DEFUN(recover_lcas,
      recover_lcas_cmd,
      "no lcas",
      NO_STR
      "default enable lcas\n"
     )
{
    int ret = 0;
    uint32_t ifindex = 0;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    vcgif.config.lcas_enable = VCG_ENABLE;

    ret = vcg_if_info_set(&vcgif, VCG_INFO_LCAS_ENA);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg lcas set failure%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;

}

DEFUN(reset_lcas,
      reset_lcas_cmd,
      "lcas reset",
      "reset lcas\n"
     )
{
    uint32_t ifindex = 0;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    if (pvcg->config.lcas_enable == VCG_ENABLE)
    {
        vcg_if_info_set(pvcg, VCG_INFO_LCAS_RESET);
    }

    pvcg->config.lcas_reset = 0;

    return CMD_SUCCESS;
}


DEFUN(config_information,
      config_information_cmd,
      "show config",
      SHOW_STR
      "config info\n"
     )
{
    uint32_t ifindex = 0;
    t_sdh_vcg_interface *pvcg = NULL;
    ifindex = (uint32_t) vty->index;

    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
        return CMD_WARNING;
    }

    vcg_config_show(vty, pvcg);

    return CMD_SUCCESS;
}


DEFUN(show_vcg_interface_config,
      show_vcg_interface_config_cmd,
      "show interface config",
      SHOW_STR
      CLI_INTERFACE_STR
      "config info\n"
     )
{
    struct listnode *node;
    void *pdata = NULL;
    t_sdh_vcg_interface *pvcg = NULL;

    for (ALL_LIST_ELEMENTS_RO(g_vcg_if_list, node, pdata))
    {
        pvcg = (t_sdh_vcg_interface *)pdata;

        vcg_config_show(vty, pvcg);
    }

    return CMD_SUCCESS;
}

DEFUN(show_sdhvcg_interface_config,
      show_sdhvcg_interface_config_cmd,
      "show interface config sdhvcg [USP]",
      SHOW_STR
      CLI_INTERFACE_STR
      "config info\n"
      CLI_INTERFACE_VCG_STR
      CLI_INTERFACE_VCG_VHELP_STR
     )
{
    uint32_t ifindex = 0;
    struct listnode *node;
    void *pdata = NULL;
    t_sdh_vcg_interface *pvcg = NULL;

    if (argv[0] != NULL)
    {
        ifindex = ifm_get_ifindex_by_name("sdhvcg", (char *)argv[0]);
        pvcg = vcg_if_lookup(ifindex);

        if (pvcg == NULL)
        {
            vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);
            return CMD_WARNING;
        }

        vcg_config_show(vty, pvcg);
    }
    else
    {
        for (ALL_LIST_ELEMENTS_RO(g_vcg_if_list, node, pdata))
        {
            pvcg = (t_sdh_vcg_interface *)pdata;

            vcg_config_show(vty, pvcg);
        }
    }

    return CMD_SUCCESS;
}

DEFUN(vlan_cfg,
      vlan_cfg_cmd,
      "vlan <1-4094> pri <0-7> {tpid TPID | upvlan-keep | downvlan-keep}",
      "vlan\n"
      "the range of vlan\n"
      "pri\n"
      "the range of priority\n"
      "tpid\n"
      "tpid value\n"
      "upvlan keep\n"
      "downvlan keep\n"
     )
{
    int ret;
    uint32_t ifindex;
    uint16_t vid;
    uint8_t  cos;
    t_sdh_vcg_interface  vcgif;
    t_sdh_vcg_interface *pvcg = NULL;

    ifindex = (uint32_t) vty->index;

	
    pvcg = vcg_if_lookup(ifindex);

    if (pvcg == NULL)
    {
        vty_error_out(vty, "vcg interface no exist%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(&vcgif, pvcg, sizeof(t_sdh_vcg_interface));

    if (0 == (SDH_VCG_SUPPORT_VLAN & pvcg->config.support))
    {
        vty_error_out(vty, "vcg interface not support vlan %s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    VTY_GET_INTEGER_RANGE("vlan", vid, argv[0], 1, 4094);
    VTY_GET_INTEGER_RANGE("pri", cos, argv[1], 0, 7);
	
    vcgif.config.vid = vid;
    vcgif.config.cos = cos;

    vcgif.config.tpid = 0x8100;
    vcgif.config.upvlan_ena = 1;
    vcgif.config.downvlan_ena = 1;

    if (argv[2] != NULL)
    {
        vcgif.config.tpid = strtol(argv[2], NULL, 16);
    }

    if (argv[3] != NULL)
    {
        vcgif.config.upvlan_ena = 0;
    }

    if (argv[4] != NULL)
    {
        vcgif.config.downvlan_ena = 0;
    }

    ret = vcg_if_info_set(&vcgif, VCG_INFO_VLAN);

    if (ret != 0)
    {
        vty_error_out(vty, "vcg set vlan failure%s", VTY_NEWLINE);

        return CMD_WARNING;
    }

    memcpy(pvcg, &vcgif, sizeof(t_sdh_vcg_interface));

    return CMD_SUCCESS;
}


DEFUN(switch_image,
   switch_image_cmd,
   "switch eos-image (vc4 | vc3 | vc12) slot <1-32>",
   "switch\n"
   "the image which needs switch\n"
   "vc4 image\n"
   "vc3 image\n"
   "vc12 image\n"
   "slot number"
  )
{   
     t_sdh_vcg_interface *pvcg = NULL;
     int ret = 0;
     uint8_t p_slot = 0;
     uint32_t vx_slot = 0;
     uint32_t slot_exist = 0;
     uint32_t image_type = 0;
     struct devm_slot slot_info;
     struct listnode *node;
     void *pdata = NULL;
	 
     if (argv[0] != NULL)
     {
         if (strncmp(argv[0], "vc4", 3) == 0)
         {
             image_type = SDH_VCG_LEVEL_VC4;
         }
         else if (strncmp(argv[0], "vc3", 3) == 0)
         {
             image_type = SDH_VCG_LEVEL_VC3;
         }
		 else{

             image_type = SDH_VCG_LEVEL_VC12;
		 }
     }
 
     vx_slot = atoi(argv[1]);
         
     memset(&slot_info, 0, sizeof(struct devm_slot));
     devm_comm_get_slot(1 , vx_slot, MODULE_ID_VCG, &slot_info);
     if(slot_info.id != ID_ES08 && slot_info.id != ID_ES08S && slot_info.id != ID_ES32S)
     {
         vty_error_out(vty, "Slot is not the needed card! %s", VTY_NEWLINE);
         return CMD_WARNING;
     }
     
     for (ALL_LIST_ELEMENTS_RO(g_vcg_if_list, node, pdata))
     {
         pvcg = (t_sdh_vcg_interface *)pdata;
         if(pvcg != NULL)
         {
             p_slot = IFM_SLOT_ID_GET(pvcg->ifindex);
             if(p_slot == vx_slot)
             {   
                 pvcg->image_level = image_type;
                 slot_exist = 1;
                 break;
             }
         }
     }
 
     if(slot_exist)
     {   
         ret = vcg_if_info_set(pvcg, VCG_INFO_SWITCH_IMAGE);

         if (ret != 0)
         {
             vty_error_out(vty, "switch image failure%s", VTY_NEWLINE);
     
             return CMD_WARNING;
         }
     }
   
     vty_out ( vty, "Success to switch the image !%s", VTY_NEWLINE);
	 
	 return CMD_SUCCESS;
}

DEFUN (sdhvcg_conf_debug_fun,
	 sdhvcg_conf_debug_cmd,
	 "debug sdhvcg (enable|disable) all",
	 "Debug config\n"
	 "Sdhvcg config\n"
	 "Sdhvcg debug enable\n"
	 "Sdhvcg debug disable\n"
	 "Debug all\n"
	)
{
   int enable = 0;
   unsigned int type = 0;

   if(argv[0][0] == 'e') enable = 1;

  
   type = SDH_VCG_DEBUG_TYPE_ALL;

   zlog_debug_set(vty, type,  enable);

   return(CMD_SUCCESS);
}


void vcg_binding_show(struct vty *vty, t_sdh_vcg_interface *pvcg)
{
    int num = 1;
    int i , j;
    int vc4_sequence = 0;
    int vc3_vc12_sequence = 0;
    int up_num = 0;
    int dn_num = 0;
    struct tmp_index
    {
        uint8_t hp;
        uint8_t lp;
    }   up[128], dn[128];

    memset(up, 0, sizeof(up));
    memset(dn, 0, sizeof(dn));

    if (pvcg->config.binding_up.num == 0 && pvcg->config.binding_down.num == 0)
    {
        vty_out(vty, "vcg binding null %s", VTY_NEWLINE);
        return;
    }

    if (pvcg->config.binding_up.num != 0)
    {
        vc4_sequence = pvcg->config.binding_up.vc4;
        vc3_vc12_sequence = pvcg->config.binding_up.vc3_vc12;

        for (num = 0; num < pvcg->config.binding_up.num; num++)
        {
            if (pvcg->config.level == SDH_VCG_LEVEL_VC4)
            {
                up[up_num].hp = vc4_sequence + num;
                up[up_num].lp = 0;
                up_num++;
            }
            else if (pvcg->config.level == SDH_VCG_LEVEL_VC3)
            {
                up[up_num].hp = vc4_sequence;
                up[up_num].lp = vc3_vc12_sequence;
                up_num++;

                vc3_vc12_sequence++;

                if (vc3_vc12_sequence > 3)
                {
                    vc3_vc12_sequence = 1;
                    vc4_sequence += 1;
                }
            }
            else if (pvcg->config.level == SDH_VCG_LEVEL_VC12)
            {
                up[up_num].hp = vc4_sequence;
                up[up_num].lp = vc3_vc12_sequence;
                up_num++;

                vc3_vc12_sequence++;

                if (vc3_vc12_sequence > 63)
                {
                    vc3_vc12_sequence = 1;
                    vc4_sequence += 1;
                }
            }
        }
    }

    if (pvcg->config.binding_down.num != 0)
    {
        vc4_sequence = pvcg->config.binding_down.vc4;
        vc3_vc12_sequence = pvcg->config.binding_down.vc3_vc12;

        for (num = 0; num < pvcg->config.binding_down.num; num++)
        {
            if (pvcg->config.level == SDH_VCG_LEVEL_VC4)
            {
                dn[dn_num].hp = vc4_sequence + num;
                dn[dn_num].lp = 0;
                dn_num++;
            }
            else if (pvcg->config.level == SDH_VCG_LEVEL_VC3)
            {
                dn[dn_num].hp = vc4_sequence;
                dn[dn_num].lp = vc3_vc12_sequence;
                dn_num++;

                vc3_vc12_sequence++;

                if (vc3_vc12_sequence > 3)
                {
                    vc3_vc12_sequence = 1;
                    vc4_sequence += 1;
                }
            }
            else if (pvcg->config.level == SDH_VCG_LEVEL_VC12)
            {
                dn[dn_num].hp = vc4_sequence;
                dn[dn_num].lp = vc3_vc12_sequence;
                dn_num++;

                vc3_vc12_sequence++;

                if (vc3_vc12_sequence > 63)
                {
                    vc3_vc12_sequence = 1;
                    vc4_sequence += 1;
                }
            }
        }
    }

    vty_out(vty, "vcg binding %s", VTY_NEWLINE);
    vty_out(vty, "no  level  vc4  vc3/vc12  direct%s", VTY_NEWLINE);

    num = 1;

    for (i = 0; i < up_num; ++i)
    {
        for (j = 0; j < dn_num; ++j)
        {
            if (up[i].hp == dn[j].hp && up[i].lp == dn[j].lp)
            {
                if (pvcg->config.level == SDH_VCG_LEVEL_VC4)
                {
                    vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc4", up[i].hp, 0, "BOTH", VTY_NEWLINE);
                }
                else if (pvcg->config.level == SDH_VCG_LEVEL_VC3)
                {
                    vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc3", up[i].hp, up[i].lp, "BOTH", VTY_NEWLINE);
                }
                else
                {
                    vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc12", up[i].hp, up[i].lp, "BOTH", VTY_NEWLINE);
                }

                dn[j].hp = 0;
                dn[j].lp = 0;

                break;
            }
        }

        if (j == dn_num)
        {
            if (pvcg->config.level == SDH_VCG_LEVEL_VC4)
            {
                vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc4", up[i].hp, 0, "UP", VTY_NEWLINE);
            }
            else if (pvcg->config.level == SDH_VCG_LEVEL_VC3)
            {
                vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc3", up[i].hp, up[i].lp, "UP", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc12", up[i].hp, up[i].lp, "UP", VTY_NEWLINE);
            }
        }
    }


    for (j = 0; j < dn_num; ++j)
    {
        if (dn[j].hp != 0)
        {
            if (pvcg->config.level == SDH_VCG_LEVEL_VC4)
            {
                vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc4", dn[j].hp, 0, "DOWN", VTY_NEWLINE);
            }
            else if (pvcg->config.level == SDH_VCG_LEVEL_VC3)
            {
                vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc3", dn[j].hp, dn[j].lp, "DOWN", VTY_NEWLINE);
            }
            else
            {
                vty_out(vty, "%-4d%-7s%-5d%-10d%-6s%s", num++, "vc12", dn[j].hp, dn[j].lp, "DOWN", VTY_NEWLINE);
            }
        }
    }
}

void vcg_config_show(struct vty *vty, t_sdh_vcg_interface *pvcg)
{
    char buffer[SDH_OH_JX_LEN];

    vty_out(vty, "interface sdhvcg %d/%d/%d%s%s", IFM_UNIT_ID_GET(pvcg->ifindex),
            IFM_SLOT_ID_GET(pvcg->ifindex),
            IFM_PORT_ID_GET(pvcg->ifindex), VTY_NEWLINE, VTY_NEWLINE);

    if (pvcg->config.type == SDH_VCG_ENCAP_TYPE_EOS)
    {
        vty_out(vty, "encapsulate type     : %s%s", "EoS", VTY_NEWLINE);
    }
    else if (pvcg->config.type == SDH_VCG_ENCAP_TYPE_EOE)
    {
        vty_out(vty, "encapsulate type     : %s%s", "EoE", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "encapsulate type     : %s%s", "EoP", VTY_NEWLINE);
    }

    vty_out(vty, "encapsulate protocol : %s%s", "GFP-F", VTY_NEWLINE);

    snprintf(buffer, SDH_OH_JX_LEN, "%s", pvcg->config.tra_jx + 1);
    vty_out(vty, "jx transmit          : %s%s", buffer, VTY_NEWLINE);
    snprintf(buffer, SDH_OH_JX_LEN, "%s", pvcg->config.exp_jx + 1);
    vty_out(vty, "jx expect            : %s%s", buffer, VTY_NEWLINE);

    vty_out(vty, "lcas support         : %s%s", (pvcg->config.support & SDH_VCG_SUPPORT_LCAS) ? "support" : "no support", VTY_NEWLINE);
    vty_out(vty, "lcas enable          : %s%s", pvcg->config.lcas_enable == VCG_ENABLE ? "enable" : "disable", VTY_NEWLINE);

    if (pvcg->config.scrambling == SDH_VCG_SCRAMBLING_NONE)
    {
        vty_out(vty, "scrambling           : none%s", VTY_NEWLINE);
    }
    else if (pvcg->config.scrambling == SDH_VCG_SCRAMBLING_UP)
    {
        vty_out(vty, "scrambling           : up%s", VTY_NEWLINE);
    }
    else if (pvcg->config.scrambling == SDH_VCG_SCRAMBLING_DOWN)
    {
        vty_out(vty, "scrambling           : down%s", VTY_NEWLINE);
    }
    else
    {
        vty_out(vty, "scrambling           : both%s", VTY_NEWLINE);
    }

    vty_out(vty, "fcs insert           : %s%s", pvcg->config.fcs_insert == VCG_ENABLE ? "enable" : "disable", VTY_NEWLINE);

    if (SDH_VCG_SUPPORT_VLAN & pvcg->config.support)
    {
        vty_out(vty, "vlan id              : %d%s", pvcg->config.vid, VTY_NEWLINE);
        vty_out(vty, "vlan pri             : %d%s", pvcg->config.cos, VTY_NEWLINE);
        vty_out(vty, "tpid                 : 0x%04x%s", pvcg->config.tpid, VTY_NEWLINE);
        vty_out(vty, "up to gfp vlan       : %s%s", (pvcg->config.upvlan_ena == 1) ? "remove" : "keep", VTY_NEWLINE);
        vty_out(vty, "down to eth vlan     : %s%s", (pvcg->config.downvlan_ena == 1) ? "insert" : "keep", VTY_NEWLINE);
    }

    vcg_binding_show(vty, pvcg);

    vty_out(vty, "--------------------------------------%s%s", VTY_NEWLINE, VTY_NEWLINE);
}

static int vcg_config_write(struct vty *vty)
{
    t_sdh_vcg_interface *pvcg = NULL;
    uint32_t ifindex;
    struct listnode *node;
    void *pdata = NULL;
    char ifname[IFNET_NAMESIZE];
    char jx[SDH_OH_JX_LEN];
    int ret = 0;
    int padding_zero;

    for (ALL_LIST_ELEMENTS_RO(g_vcg_if_list, node, pdata))
    {
        pvcg = (t_sdh_vcg_interface *)pdata;
        ifindex = pvcg->ifindex;
        ret = ifm_get_name_by_ifindex(ifindex, ifname);

        if (ret < 0)
        {
            continue;
        }

        vty_out(vty, "interface %s%s", ifname, VTY_NEWLINE);

        if (pvcg->config.binding_up.num != 0)
        {
            switch (pvcg->config.level)
            {
                case SDH_VCG_LEVEL_VC4:
                    vty_out(vty, " binding vc4 up vc4 %d num %d%s", pvcg->config.binding_up.vc4, pvcg->config.binding_up.num, VTY_NEWLINE);
                    break;

                case SDH_VCG_LEVEL_VC3:
                    vty_out(vty, " binding vc3 up vc4 %d vc3 %d num %d%s", pvcg->config.binding_up.vc4, pvcg->config.binding_up.vc3_vc12, pvcg->config.binding_up.num, VTY_NEWLINE);
                    break;

                case SDH_VCG_LEVEL_VC12:
                    vty_out(vty, " binding vc12 up vc4 %d vc12 %d num %d%s", pvcg->config.binding_up.vc4, pvcg->config.binding_up.vc3_vc12, pvcg->config.binding_up.num, VTY_NEWLINE);
                    break;

                default:
                    break;
            }
        }

        if (pvcg->config.binding_down.num != 0)
        {
            switch (pvcg->config.level)
            {
                case SDH_VCG_LEVEL_VC4:
                    vty_out(vty, " binding vc4 down vc4 %d num %d%s", pvcg->config.binding_down.vc4, pvcg->config.binding_down.num, VTY_NEWLINE);
                    break;

                case SDH_VCG_LEVEL_VC3:
                    vty_out(vty, " binding vc3 down vc4 %d vc3 %d num %d%s", pvcg->config.binding_down.vc4, pvcg->config.binding_down.vc3_vc12, pvcg->config.binding_down.num, VTY_NEWLINE);
                    break;

                case SDH_VCG_LEVEL_VC12:
                    vty_out(vty, " binding vc12 down vc4 %d vc12 %d num %d%s", pvcg->config.binding_down.vc4, pvcg->config.binding_down.vc3_vc12, pvcg->config.binding_down.num, VTY_NEWLINE);
                    break;

                default:
                    break;
            }
        }

        if (0 == pvcg->config.tra_jx[SDH_OH_JX_LEN - 1])
        {
            padding_zero = 1;
        }
        else
        {
            padding_zero = 0;
        }

        snprintf(jx, SDH_OH_JX_LEN, "%s", pvcg->config.tra_jx + 1);

        if (1 == padding_zero)
        {
            vty_out(vty, " oh jx transmit %s padding_zero%s", jx, VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " oh jx transmit %s%s", jx, VTY_NEWLINE);
        }

        if (0 == pvcg->config.exp_jx[SDH_OH_JX_LEN - 1])
        {
            padding_zero = 1;
        }
        else
        {
            padding_zero = 0;
        }

        snprintf(jx, SDH_OH_JX_LEN, "%s", pvcg->config.exp_jx + 1);

        if (1 == padding_zero)
        {
            vty_out(vty, " oh jx expect %s padding_zero%s", jx, VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " oh jx expect %s%s", jx, VTY_NEWLINE);
        }


        switch (pvcg->config.scrambling)
        {
            case SDH_VCG_SCRAMBLING_NONE:
                vty_out(vty, " scrambling none%s", VTY_NEWLINE);
                break;

            case SDH_VCG_SCRAMBLING_UP:
                vty_out(vty, " scrambling up%s", VTY_NEWLINE);
                break;

            case SDH_VCG_SCRAMBLING_DOWN:
                vty_out(vty, " scrambling down%s", VTY_NEWLINE);
                break;

            case SDH_VCG_SCRAMBLING_BOTH:
                vty_out(vty, " scrambling both%s", VTY_NEWLINE);
                break;

            default:
                break;
        }

        if (pvcg->config.fcs_insert == VCG_ENABLE)
        {
            vty_out(vty, " fcs insert enable%s", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " fcs insert disable%s", VTY_NEWLINE);
        }

        if (pvcg->config.lcas_enable == VCG_ENABLE)
        {
            vty_out(vty, " lcas enable%s", VTY_NEWLINE);
        }
        else
        {
            vty_out(vty, " lcas disable%s", VTY_NEWLINE);
        }

        if (SDH_VCG_SUPPORT_VLAN & pvcg->config.support && pvcg->config.vid != 0)
        {
            if (0x8100 == pvcg->config.tpid)
            {
                if (1 == pvcg->config.upvlan_ena)
                {
                    if (1 == pvcg->config.downvlan_ena)
                    {
                        vty_out(vty, " vlan %d pri %d%s", pvcg->config.vid, pvcg->config.cos, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, " vlan %d pri %d downvlan-keep%s", pvcg->config.vid, pvcg->config.cos, VTY_NEWLINE);
                    }
                }
                else
                {
                    if (1 == pvcg->config.downvlan_ena)
                    {
                        vty_out(vty, " vlan %d pri %d upvlan-keep%s", pvcg->config.vid, pvcg->config.cos, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, " vlan %d pri %d upvlan-keep downvlan-keep%s", pvcg->config.vid, pvcg->config.cos, VTY_NEWLINE);
                    }
                }
            }
            else
            {
                if (1 == pvcg->config.upvlan_ena)
                {
                    if (1 == pvcg->config.downvlan_ena)
                    {
                        vty_out(vty, " vlan %d pri %d tpid 0x%04x%s", pvcg->config.vid, pvcg->config.cos, pvcg->config.tpid, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, " vlan %d pri %d tpid 0x%04x downvlan-keep%s", pvcg->config.vid, pvcg->config.cos, pvcg->config.tpid, VTY_NEWLINE);
                    }
                }
                else
                {
                    if (1 == pvcg->config.downvlan_ena)
                    {
                        vty_out(vty, " vlan %d pri %d tpid 0x%04x upvlan-keep%s", pvcg->config.vid, pvcg->config.cos, pvcg->config.tpid, VTY_NEWLINE);
                    }
                    else
                    {
                        vty_out(vty, " vlan %d pri %d tpid 0x%04x upvlan-keep downvlan-keep%s", pvcg->config.vid, pvcg->config.cos, pvcg->config.tpid, VTY_NEWLINE);
                    }
                }
            }
        }
    }

    vty_out(vty, "!%s", VTY_NEWLINE);

    return 0;
}

/* install all cmd */
void vcg_cmd_init(void)
{
    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    install_node(&vcgif_node, vcg_config_write);

    install_default(VCG_NODE);

    /*vcg node command*/
    install_element(VCG_NODE, &binding_vcg4_cmd, CMD_SYNC);
    install_element(VCG_NODE, &binding_vcg3_cmd, CMD_SYNC);
    install_element(VCG_NODE, &binding_vcg12_cmd, CMD_SYNC);
    install_element(VCG_NODE, &delete_vcg_binding_cmd, CMD_SYNC);
    install_element(VCG_NODE, &show_vcg_binding_cmd, CMD_LOCAL);
    install_element(VCG_NODE, &set_vcg_jx_oh_cmd, CMD_SYNC);
    install_element(VCG_NODE, &set_scrambling_cmd, CMD_SYNC);
    install_element(VCG_NODE, &recover_scrambling_cmd, CMD_SYNC);
    install_element(VCG_NODE, &set_fcs_insert_cmd, CMD_SYNC);
    install_element(VCG_NODE, &recover_fcs_insert_cmd, CMD_SYNC);
    install_element(VCG_NODE, &set_lcas_cmd, CMD_SYNC);
    install_element(VCG_NODE, &recover_lcas_cmd, CMD_SYNC);
    install_element(VCG_NODE, &reset_lcas_cmd, CMD_SYNC);
    install_element(VCG_NODE, &config_information_cmd, CMD_LOCAL);
    install_element(VCG_NODE, &vlan_cfg_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &switch_image_cmd, CMD_SYNC);
	
    /*config node command*/
    install_element(CONFIG_NODE, &physical_vcg_if_cmd, CMD_SYNC);
    install_element(CONFIG_NODE, &show_vcg_interface_config_cmd, CMD_LOCAL);
    install_element(CONFIG_NODE, &show_sdhvcg_interface_config_cmd, CMD_LOCAL);
	install_element(CONFIG_NODE, &sdhvcg_conf_debug_cmd, CMD_LOCAL);

    zlog_debug(SDH_VCG_DEBUG_TYPE_ALL,"%s[%d]: Leaving function '%s'.\n", __FILE__, __LINE__, __func__);
}


