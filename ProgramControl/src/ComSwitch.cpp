#include "../../Share/inc/_ExternalVariable.hpp"
#include "../../Share/inc/_SerialCommunication.hpp"
#include "../../Share/inc/_Defines.hpp"
#include "../../Share/inc/_Flags.hpp"

void ProcessComSwitch(void)
{
#if ENABLE_COM_SWITCH
    /* 動作周期調整 */
    CONTROL_PERIOD ControlPeriod;

    COM ComSwitch(PATH_SWITCH, 1, 3, B57600, "Switch");

    while (1)
    {
        ComSwitch.send();

        if (ComSwitch.receive() == true)
        {
            /* 送り返すデータを用意 */
            ComSwitch.tx[0] = ComSwitch.rx[0];
            ComSwitch.tx[1] = ComSwitch.rx[1];
            ComSwitch.tx[2] = ComSwitch.rx[2];

            /* 受信したデータを共有メモリに移す */
            _Switch.d->d1 = ComSwitch.rx[0];
            _Switch.d->d2 = ComSwitch.rx[1];
            _Switch.d->d3 = ComSwitch.rx[2];
            _Flags.f[FLAG_PROGRAM] = _Switch.d->program;

            /* ComState */
            _ComState.cnt[COM_STATE_SWITCH]++;
        }

        /* 動作周期調整 */
        ControlPeriod.adjust();
    }
#endif
}