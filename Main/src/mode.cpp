#include <iostream>
#include <sstream>
#include "../../Share/inc/_std_func.hpp"
#include "../inc/_utility.hpp"
#include "../inc/module.hpp"
#include "../inc/external_variable.hpp"

void mode_test(std::vector<std::string> param, size_t branch_num)
{
    printf("start mode_test\n");

    struct timespec time_s = jibiki::get_time();
    while (manage_process(OPERATE_AUTO) & !g_flags[FLAG_RESET])
    {
        if (jibiki::CALC_SEC(time_s, jibiki::get_time()) > 3)
            break;
    }

    printf("finish mode_test\n");
}

void mode_wait(std::vector<std::string> param, size_t branch_num)
{
    printf("start mode_wait\n");

    struct timespec time_s = jibiki::get_time();
    while (manage_process(OPERATE_AUTO) & !g_flags[FLAG_RESET])
    {
        if (jibiki::CALC_SEC(time_s, jibiki::get_time()) > 5)
            break;
    }

    printf("finish mode_wait\n");
}

void mode_encoder(std::vector<std::string> param, size_t branch_num)
{
    /* パラメータ読み込み */
    const cv::Point2f target_odometry(std::stod(param[0]), std::stod(param[1]));
    const double min_rpm = std::stod(param[2]);
    // const double max_rpm = std::stod(param[3]);
    // const double acc = std::stod(param[4]);
    const double permissible_err = std::stod(param[5]);
    // const std::string edge = param[6];

    /* 元の文字列 */
    std::string init_str = g_executing_order[branch_num];

    while (manage_process(OPERATE_AUTO) & !g_flags[FLAG_RESET])
    {
        /* オドメトリを表示 */
        std::stringstream sstr;
        sstr << " (" << g_odometry.x << ", " << g_odometry.y << ") ->"
             << " (" << target_odometry.x << ", " << target_odometry.y << ")" << std::flush;
        g_executing_order[branch_num] = init_str + sstr.str();

        /* 移動パラメータを計算 */
        g_chassis.m_speed = min_rpm;
        g_chassis.m_theta = jibiki::CALC_ANGLE(g_odometry, target_odometry);

        if (jibiki::CALC_DIS(g_odometry, target_odometry) < permissible_err)
            break;
    }
}