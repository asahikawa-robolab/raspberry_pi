#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/module.hpp"

int main(void)
{
    try
    {
        /* ポートを設定 */
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V9YT-if00-port0",
            1, 3, B57600, "switch", false);

        /* スイッチスレーブのデータを管理するクラス */
        SwitchData d;

        while (!jibiki::kbhit())
        {
            if (com.receive())
            {
                /* データを送り返してスイッチスレーブの LED を点灯させる */
                com.tx(0) = com.rx(0);
                com.send();

                /* 受信データを読み込ませる */
                d.set(com);

                /* 表示 */
                printf("push_l %d, push_r %d, "
                       "toggle %d, %d, %d, %d, %d, %d, %d, "
                       "tact_lu %d, ld %d, "
                       "ru %d, rd %d, slide %d\n",
                       d.push_l(), d.push_r(),
                       d.toggle(0), d.toggle(1), d.toggle(2),
                       d.toggle(3), d.toggle(4), d.toggle(5),
                       d.toggle(6),
                       d.lu(), d.ld(), d.ru(), d.rd(),
                       d.slide());
            }
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}