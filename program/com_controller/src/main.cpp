#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/module.hpp"

void print(Controller &d); /* 受信データを表示 */

int main(void)
{
    try
    {
        /* ポートを設定 */
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V99N-if00-port0",
            0, 8, B57600, "controller", false);
        /* 受信データを管理するクラス */
        Controller d;

        /* キー入力があったら終了 */
        while (!jibiki::kbhit())
        {
            if (com.receive())
            {
                /* 受信データを読み込む */
                d.set(com);
                /* 表示 */
                print(d);
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

/* 受信データを表示 */
void print(Controller &d)
{
    printf("lcl %d, lcr %d, lcu %d, lcd %d, rcl %d, rcr %d, rcu %d, rcd %d\n",
           d.l_cross_l(), d.l_cross_r(), d.l_cross_u(), d.l_cross_d(),
           d.r_cross_l(), d.r_cross_r(), d.r_cross_u(), d.r_cross_d());
    printf("ld %d, lm %d, rd %d, rm %d, ll %d, lr %d, rl %d, rr %d\n",
           d.l_switch_d(), d.l_switch_m(), d.r_switch_d(), d.r_switch_m(),
           d.l_lever_l(), d.l_lever_r(), d.r_lever_l(), d.r_lever_r());
    printf("lh %d, lv %d, rh %d, rv %d\n",
           d.l_analog_stick_h(), d.l_analog_stick_v(),
           d.r_analog_stick_h(), d.r_analog_stick_v());
    printf("ls %d, rs %d, lu %d, ru %d\n",
           d.l_slide(), d.r_slide(), d.l_switch_u(), d.r_switch_u());
    printf("lu %d, mu %d, ru %d, ld %d, md %d, rd %d\n",
           d.tact_lu(), d.tact_mu(), d.tact_ru(),
           d.tact_ld(), d.tact_md(), d.tact_rd());
}