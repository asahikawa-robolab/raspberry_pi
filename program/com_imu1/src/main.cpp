#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"

int main(void)
{
    try
    {
        /* ポートを設定 */
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579AL-if00-port0",
            1, 4, B57600, "imu", false);

        /* IMU のリセットフラグ */
        bool reset_flag = false;

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'r':
                    reset_flag ^= 1;
                    break;
                case 'q':
                    return 0;
                }
            }

            /* 送信 */
            com.tx(0) = reset_flag;
            com.send();

            /* 受信 */
            if (com.receive())
            {
                /* センサ出力から角度を計算 */
                double angle =
                    jibiki::deg_rad(jibiki::asbl(com.rx(0), com.rx(1)) / 10.0);
                /* 表示 */
                printf("reset %d, %.2lf [deg]\n",
                       reset_flag, jibiki::rad_deg(angle));
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