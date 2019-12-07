#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <opencv2/opencv.hpp>
#include "../../Share/inc/_Defines.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_SharedMemory.hpp"
#include "../../Share/inc/_Class.hpp"
#include "../inc/Orders.hpp"

void ProcessMaster(void)
{
    /* 動作周期調整 */
    CONTROL_PERIOD ControlPeriod;

    ORDERS Orders;
    Orders.load();

    while (_Flags.p())
    {
        /* Order に従う */
        if (_Flags.f[FLAG_OPERATE] == OPERATE_AUTO)
        {
            Orders.execute(); /* STATE_DOING の ORDER を実行する */
            Orders.manage();  /* q の state を管理する */
        }

        /* 動作周期調整 */
        ControlPeriod.adjust();
    }
}