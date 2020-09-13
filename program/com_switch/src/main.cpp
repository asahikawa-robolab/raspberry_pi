#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../inc/module.hpp"

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
                d.set(com.rx(0), com.rx(1), com.rx(2));

                /* 表示 */
                printf("push_l %d, push_r %d, "
                       "toggle %d, %d, %d, %d, %d, %d, %d, "
                       "tact_lu %d, ld %d, "
                       "ru %d, rd %d, slide %d\n",
                       d.m_push_l.read(), d.m_push_r.read(),
                       d.m_toggle[0].read(), d.m_toggle[1].read(), d.m_toggle[2].read(),
                       d.m_toggle[3].read(), d.m_toggle[4].read(), d.m_toggle[5].read(),
                       d.m_toggle[6].read(),
                       d.m_lu.read(), d.m_ld.read(), d.m_ru.read(), d.m_rd.read(),
                       d.m_slide.read());
            }
        }
    }
    catch (std::string err)
    {
        std::cout << err << std::endl;
        return 1;
    }
    return 0;
}