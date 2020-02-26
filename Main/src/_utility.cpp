#include <stdint.h>
#include <unistd.h>
#include <sstream>
#include <fstream>
#include "../inc/_utility.hpp"
#include "../inc/module.hpp"
#include "../inc/external_variable.hpp"

/*-----------------------------------------------
 *
 * スレッドの管理
 *
-----------------------------------------------*/
bool manage_process(OperateMethod operate_method)
{
    /* exit_flag が立ったら処理を終了 */
    if (g_flags[FLAG_EXIT])
        return false;

    /* operate_flag と operate_method が一致するまで待機 */
    while (operate_method != OPERATE_NONE && g_flags[FLAG_OPERATE] != operate_method)
        usleep(50);

    /* 動作周期調整 */
    usleep(50);

    return true;
}

/*-----------------------------------------------
 *
 * -pi ~ pi の不連続部に影響されない角度偏差（subed - sub）を返す
 *
-----------------------------------------------*/
double calc_angle_diff(double subed, double sub, TurnMode turn_mode)
{
    /* 角度を -pi ~ pi にする */
    double subed_limited = jibiki::LIMIT_ANGLE(subed);
    double sub_limited = jibiki::LIMIT_ANGLE(sub);
    double angle_diff = 0;

    switch (turn_mode)
    {
    case TURN_CCW:
        if (subed_limited - sub_limited >= 0)
            angle_diff = subed_limited - sub_limited;
        else
            angle_diff = subed_limited - sub_limited + 2 * M_PI;
        break;
    case TURN_CW:
        if (subed_limited - sub_limited > 0)
            angle_diff = subed_limited - sub_limited - 2 * M_PI;
        else
            angle_diff = subed_limited - sub_limited;
        break;
    case TURN_SHORTEST:
        if (subed_limited - sub_limited > M_PI)
            angle_diff = (subed_limited - sub_limited) - 2 * M_PI;
        else if (subed_limited - sub_limited < -M_PI)
            angle_diff = (subed_limited - sub_limited) + 2 * M_PI;
        else
            angle_diff = subed_limited - sub_limited;
        break;
    }

    return angle_diff;
}