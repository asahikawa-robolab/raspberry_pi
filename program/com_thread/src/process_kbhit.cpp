#include <unistd.h>
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

extern jibiki::ShareVar<short> g_angle;
extern Imu g_imu;
extern Controller g_controller;
extern SwitchData g_switch_data;

void thread_kbhit(jibiki::ShareVar<bool> &exit_flag)
{
    while (jibiki::thread::manage(exit_flag))
    {
        /* キー入力 */
        if (jibiki::kbhit())
        {
            switch (getchar())
            {
            case 'u':
                g_angle += 360;
                break;
            case 'd':
                g_angle -= 360;
                break;
            case 'r':
                g_imu.write_offset(0);
                break;
            case 'q':
                exit_flag = true;
                break;
            }
        }

        /* 表示 */
        printf("angle : %d, imu : %.2lf [deg], l_analog_stick_h : %d, push_l : %d\n",
               g_angle.read(),
               jibiki::rad_deg(g_imu.read()),
               g_controller.l_analog_stick_h(),
               g_switch_data.push_l());

        usleep(50); /* printf が表示されすぎないようにするため */
    }
}