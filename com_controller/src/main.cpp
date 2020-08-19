#include "../../share/inc/_serial_communication.hpp"
#include "../inc/module.hpp"

void print(ControllerData &d); /* 受信データを表示 */

int main(void)
{
    try
    {
        /* ポートを設定 */
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V99N-if00-port0",
            0, 8, B57600, "controller", false);
        /* 受信データを管理するクラス */
        ControllerData d;

        /* キー入力があったら終了 */
        while (!jibiki::kbhit())
        {
            if (com.receive())
            {
                /* 受信データを読み込む */
                d.set(com.rx(0), com.rx(1), com.rx(2), com.rx(3),
                      com.rx(4), com.rx(5), com.rx(6), com.rx(7));
                /* 表示 */
                print(d);
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

/* 受信データを表示 */
void print(ControllerData &d)
{
    printf("lcl %d, lcr %d, lcu %d, lcd %d, rcl %d, rcr %d, rcu %d, rcd %d\n",
           d.m_l_cross_l.read(), d.m_l_cross_r.read(),
           d.m_l_cross_u.read(), d.m_l_cross_d.read(),
           d.m_r_cross_l.read(), d.m_r_cross_r.read(),
           d.m_r_cross_u.read(), d.m_r_cross_d.read());
    printf("ld %d, lm %d, rd %d, rm %d, ll %d, lr %d, rl %d, rr %d\n",
           d.m_l_switch_d.read(),
           d.m_l_switch_m.read(),
           d.m_r_switch_d.read(),
           d.m_r_switch_m.read(),
           d.m_l_lever_l.read(),
           d.m_l_lever_r.read(),
           d.m_r_lever_l.read(),
           d.m_r_lever_r.read());
    printf("lh %d, lv %d, rh %d, rv %d\n",
           d.m_l_analog_stick_h.read(),
           d.m_l_analog_stick_v.read(),
           d.m_r_analog_stick_h.read(),
           d.m_r_analog_stick_v.read());
    printf("ls %d, rs %d, lu %d, ru %d\n",
           d.m_l_slide.read(),
           d.m_r_slide.read(),
           d.m_l_switch_u.read(),
           d.m_r_switch_u.read());
    printf("lu %d, mu %d, ru %d, ld %d, md %d, rd %d\n",
           d.m_tact_lu.read(),
           d.m_tact_mu.read(),
           d.m_tact_ru.read(),
           d.m_tact_ld.read(),
           d.m_tact_md.read(),
           d.m_tact_rd.read());
}