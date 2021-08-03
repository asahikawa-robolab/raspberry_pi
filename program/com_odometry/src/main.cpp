#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_thread.hpp"

int main(void)
{
    try
    {
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AH061WJN-if00-port0",
            5, 8, B57600, "odometry", true);

        jibiki::ShareVar<bool> reset(false);
        jibiki::ShareVar<int32_t> odometry[2];
        odometry[0] = 0;
        odometry[1] = 0;

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'r':
                    reset ^= 1;
                    break;
                case 'q':
                    return 0;
                }
                printf("reset %d\n", reset.read());
            }

            /* 送信 */
            com.tx(1) = reset.read();
            com.tx(2) = reset.read();
            com.send();

            /* 受信 */
            if (com.receive())
            {
                odometry[0] = com.rx(0);
                odometry[0] = odometry[0].read() | (com.rx(1) << 8);
                odometry[0] = odometry[0].read() | (com.rx(2) << 16);
                odometry[0] = odometry[0].read() | (com.rx(3) << 24);
                odometry[1] = com.rx(4);
                odometry[1] = odometry[1].read() | (com.rx(5) << 8);
                odometry[1] = odometry[1].read() | (com.rx(6) << 16);
                odometry[1] = odometry[1].read() | (com.rx(7) << 24);
                printf("%d, %d\n", odometry[0].read(), odometry[1].read());
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