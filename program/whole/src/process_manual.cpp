#include <iostream>
#include <unistd.h>
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"
#include "../inc/ext_var.hpp"

void thread_manual(jibiki::ShareVar<bool> &exit_flag,
                   jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method)
{
    try
    {

        if (!jibiki::thread::enable("manual"))
            return;

        while (jibiki::thread::manage(exit_flag,
                                      current_method,
                                      jibiki::thread::OPERATE_MANUAL))
        {

            /* 足回りに反映 */
            double speed = g_controller.speed(Controller::MODE_L, Controller::DIR_INF);
            double theta = g_controller.theta(Controller::MODE_L, Controller::DIR_INF);
            // g_chassis.m_speed = speed * 1E-2 * g_chassis.max_rpm();
            // g_chassis.m_theta = theta;

            g_steer.m_speed = speed * 1E-2 * g_chassis.max_rpm();
            g_steer.m_theta = theta;
            g_steer.calc();
            // printf("%lf,%lf,%lf,%lf\n", g_steer.bl_ang(), g_steer.br_ang(), g_steer.fl_ang(), g_steer.fr_ang());
            /* 旋回 */
            if (g_controller.l_cross_r())
                g_steer.m_spin = jibiki::deg_rad(0);
            if (g_controller.l_cross_u())
                g_steer.m_spin = jibiki::deg_rad(90);
            if (g_controller.l_cross_l())
                g_steer.m_spin = jibiki::deg_rad(180);
            if (g_controller.l_cross_d())
                g_steer.m_spin = jibiki::deg_rad(270);

            // 昇降
            if (g_controller.l_switch_u())
                g_rev_tgt[0] = 30;
            if (g_controller.l_switch_m())
                g_rev_tgt[0] = 0;
            if (g_controller.l_switch_d())
                g_rev_tgt[0] = 30;
        }
    }
    catch (const std::exception &e)
    {
        jibiki::print_err(__PRETTY_FUNCTION__);
        exit_flag = true;
        return; /* 最上部 */
    }
}