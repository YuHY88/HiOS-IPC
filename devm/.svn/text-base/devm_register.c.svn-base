#include <lib/linklist.h>
#include "lib/memshare.h"
#include "lib/msg_ipc.h"
#include "lib/msg_ipc_n.h"
#include <lib/module_id.h>
#include <lib/zassert.h>
#include <lib/devm_com.h>
#include <lib/log.h>
#include "devm.h"

struct list *devm_event_list[DEV_EVENT_MAX];


int devm_register_msg(int module_id, uint8_t subtype , uint8_t queue_id)
{
    struct list *plist;
    unsigned int index = 0;

    if (subtype >= DEV_EVENT_MAX)
    {
        assert(0);
        return -1;
    }

    index = 0;
    index |= (module_id << 0);
    index |= (queue_id << 8);

    plist = devm_event_list[subtype];

    if (plist == NULL)
    {
        assert(0);
        return -1;
    }

    if (listnode_lookup(plist, (void *)index) == NULL)
    {
        listnode_add(plist, (void *)index);
        zlog_debug(DEVM_DBG_REG, "%s[%d]: leave '%s': sucessfully add module(%d) in devm event list, subtype(%d), queue id = %d.\n", __FILE__, __LINE__, __func__, module_id, subtype, queue_id);
    }

    return 0;
}



void devm_event_list_init(void)
{
    zlog_debug(DEVM_DBG_REG, "%s[%d]: Entering function '%s'.\n", __FILE__, __LINE__, __func__);

    for (int i = 0; i < DEV_EVENT_MAX; ++i)
    {
        devm_event_list[i] = list_new();
    }

    return;
}


int devm_event_notify(enum DEV_EVENT type, struct devm_com *pdev)
{
    struct list *plist;
    struct listnode *node;
    int *pdata = NULL;
    int module_id = 0;
    uint8_t queue_id = 0;
    unsigned int index = 0;

    plist = devm_event_list[type];

    for (ALL_LIST_ELEMENTS_RO(plist, node, pdata))
    {
        module_id = 0;
        queue_id = 0;
        index = (int)pdata;
        module_id |= (index & 0xff);
        queue_id |= ((index & 0xff00) >> 8);

        ipc_send_msg_n2(pdev, sizeof(struct devm_com), 1, module_id, MODULE_ID_DEVM,
                        IPC_TYPE_DEVM, type, IPC_OPCODE_EVENT, 0);

        zlog_debug(DEVM_DBG_REG, "%s[%d]:%s:devm_event_notify, module=%d,type=%d,u=%d,s=%d,p=%d\n", __FILE__, __LINE__, __func__, module_id, type, pdev->unit, pdev->slot, pdev->port);
    }

    return 0;
}




