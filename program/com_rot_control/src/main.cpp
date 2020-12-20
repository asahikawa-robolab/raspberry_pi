#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_thread.hpp"

int main(void)
{
    try
    {
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LQ-if00-port0",
            5, 6, B57600, "rot", true);

        jibiki::ShareVar<short> angle(0);

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'u':
                    angle += 360;
                    break;
                case 'd':
                    angle -= 360;
                    break;
                case 'q':
                    return 0;
                }
            }

            /* 送信 */
            com.tx(1) = jibiki::up(angle.read());
            com.tx(2) = jibiki::low(angle.read());
            com.send();

            /* 受信 */
            if (com.receive())
            {
                printf("target %d, curr %d, pwm %d\n",
                       angle.read(),
                       jibiki::asbl(com.rx(0), com.rx(1)),
                       (int8_t)com.rx(4));
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