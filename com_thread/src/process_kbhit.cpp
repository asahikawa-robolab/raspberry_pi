#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_thread.hpp"
#include "../inc/module.hpp"

extern jibiki::ShareVal<short> g_angle;
extern ImuData g_imu_data;
extern ControllerData g_controller_data;
extern SwitchData g_switch_data;

void thread_kbhit(jibiki::ShareVal<bool> &exit_flag)
{
    while (jibiki::manage_thread(exit_flag.read()))
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
                g_imu_data.write_offset(0);
                break;
            case 'q':
                exit_flag = true;
                break;
            }
        }

        /* 表示 */
        printf("angle : %d, imu : %.2lf [deg], l_analog_stick_h : %d, push_l : %d\n",
               g_angle.read(),
               jibiki::rad_deg(g_imu_data.read()),
               g_controller_data.m_l_analog_stick_h.read(),
               g_switch_data.m_push_l.read());
    }
}