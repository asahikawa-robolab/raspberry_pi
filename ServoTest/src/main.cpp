#include "../../Share/inc/_SerialCommunication.hpp"
#include "../../Share/inc/_StdFunc.hpp"

int main(void)
{
    COM com("/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI057CF8-if00-port0", 5, 0, B57600, "test");
    uint8_t angle[4] = {0, 0, 0, 0};
    const int base = 10;

    while (1)
    {
        /* 送信 */
        com.tx[0] = 0;
        com.tx[1] = angle[0];
        com.tx[2] = angle[1];
        com.tx[3] = angle[2];
        com.tx[4] = angle[3];
        com.send();
        // printf("%d, %d, %d, %d\n", angle[0], angle[1], angle[2], angle[3]);

        /* キー入力 */
        if (kbhit())
        {
            switch (getchar())
            {
            case 'q':
                if (angle[0] <= 180 - base)
                {
                    angle[0] += base;
                }
                break;
            case 'a':
                if (angle[0] >= base)
                {
                    angle[0] -= base;
                }
                break;
            case 'w':
                if (angle[1] <= 180 - base)
                {
                    angle[1] += base;
                }
                break;
            case 's':
                if (angle[1] >= base)
                {
                    angle[1] -= base;
                }
                break;
            case 'e':
                if (angle[2] <= 180 - base)
                {
                    angle[2] += base;
                }
                break;
            case 'd':
                if (angle[2] >= base)
                {
                    angle[2] -= base;
                }
                break;
            case 'r':
                if (angle[3] <= 180 - base)
                {
                    angle[3] += base;
                }
                break;
            case 'f':
                if (angle[3] >= base)
                {
                    angle[3] -= base;
                }
                break;
            case 'c':
                return 0;
                break;
            }
        }
    }

    return 0;
}