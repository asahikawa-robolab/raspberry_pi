#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../inc/module.hpp"

int main(void)
{
    try
    {
        /* ポートを設定 */
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579AL-if00-port0",
            1, 4, B57600, "imu", false);

        /* センサのデータを管理するクラス */
        ImuData d;

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'r':
                    /* 現在の角度を 0 [deg] とする */
                    d.write_offset(0);
                    break;
                case 'q':
                    return 0;
                }
            }

            /* 受信 */
            if (com.receive())
            {
                /* センサ出力から角度を計算 */
                double angle =
                    jibiki::deg_rad(jibiki::asbl(com.rx(0), com.rx(1)) / 10.0);
                /* 角度を読み込ませる */
                d.write_raw_data(angle);
                /* 表示 */
                printf("%.2lf [deg]\n", jibiki::rad_deg(d.read()));
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