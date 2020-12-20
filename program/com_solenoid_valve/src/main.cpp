#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_thread.hpp"

int main(void)
{
    try
    {
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579S8-if00-port0",
            2, 0, B57600, "solenoid_valve", false);

        jibiki::ShareVar<int8_t> valve_val[2];
        valve_val[0] = 0;
        valve_val[1] = 0;

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'u':
                    valve_val[0] = 0b01111111;
                    valve_val[1] = 0b01111111;
                    break;
                case 'd':
                    valve_val[0] = 0;
                    valve_val[1] = 0;
                    break;
                case 'q':
                    return 0;
                }
                printf("%u, %u\n",
                       valve_val[0].read(),
                       valve_val[1].read());
            }

            /* 送信 */
            com.tx(0) = valve_val[0].read();
            com.tx(1) = valve_val[1].read();
            com.send();
        }
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
        return 1;
    }
    return 0;
}