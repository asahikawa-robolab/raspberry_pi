#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../inc/ext_var.hpp"

void thread_kbhit(jibiki::ShareVar<bool> &exit_flag,
                  jibiki::ShareVar<bool> &start_flag,
                  jibiki::ShareVar<bool> &reset_flag,
                  jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method,
                  jibiki::ShareVar<int> &pushed_key)
{
    try
    {
        if (!jibiki::thread::enable("kbhit"))
            return;

        while (jibiki::thread::manage(exit_flag))
        {
            if (jibiki::kbhit())
                pushed_key = getchar();

            switch (pushed_key.read() & 0xFF)
            {
            /*-----------------------------------------------
            フラグ
            -----------------------------------------------*/
            case 's': /* start */
                start_flag ^= 1;
                break;
            case 'r': /* reset */
                reset_flag ^= 1;
                break;
            case 'q': /* quit */
                exit_flag = true;
                break;
            case 't': /* toggle */
                current_method = current_method.read() == jibiki::thread::OPERATE_AUTO
                                     ? jibiki::thread::OPERATE_MANUAL
                                     : jibiki::thread::OPERATE_AUTO;
                break;
            /*-----------------------------------------------
            IMU
            -----------------------------------------------*/
            case 'l':
                g_imu.write_raw_data(g_imu.read() + jibiki::deg_rad(10));
                break;
            case ',':
                g_imu.write_raw_data(g_imu.read() - jibiki::deg_rad(10));
                break;
            /*-----------------------------------------------
            足回り
            -----------------------------------------------*/
            case ';':
                g_chassis.m_speed += 10;
                break;
            case '.':
                g_chassis.m_speed -= 10;
                break;
            case ':':
                g_chassis.m_theta += jibiki::deg_rad(10);
                break;
            case '/':
                g_chassis.m_theta -= jibiki::deg_rad(10);
                break;
            case ']':
                g_chassis.m_spin += jibiki::deg_rad(10);
                break;
            case '\\':
                g_chassis.m_spin -= jibiki::deg_rad(10);
                break;
            }

            if (pushed_key.read() != -1)
                pushed_key = -1;
        }
    }
    catch (const std::exception &e)
    {
        jibiki::print_err(__PRETTY_FUNCTION__);
        exit_flag = true;
        return; /* 最上部 */
    }
}