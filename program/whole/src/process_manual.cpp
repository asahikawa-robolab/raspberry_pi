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
            g_chassis.m_speed = speed * 1E-2 * g_chassis.max_rpm();
            g_chassis.m_theta = theta;
            /* 旋回 */
            if (g_controller.tact_mu())
                g_chassis.m_spin = jibiki::deg_rad(0);
            if (g_controller.tact_ld())
                g_chassis.m_spin = jibiki::deg_rad(90);
            if (g_controller.tact_md())
                g_chassis.m_spin = jibiki::deg_rad(180);
            if (g_controller.tact_rd())
                g_chassis.m_spin = jibiki::deg_rad(-90);
        }
    }
    catch (const std::exception &e)
    {
        jibiki::print_err(__PRETTY_FUNCTION__);
        exit_flag = true;
        return; /* 最上部 */
    }
}