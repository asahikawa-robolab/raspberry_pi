#include <sstream>
#include "../../Share/inc/_std_func.hpp"
#include "../inc/module.hpp"
#include "../inc/_utility.hpp"
#include "../inc/external_variable.hpp"

/*-----------------------------------------------
 *
 * Chassis
 *
-----------------------------------------------*/
/* コンストラクタ */
Chassis::Chassis(void)
{
    m_speed = 0;
    m_theta = jibiki::DEG_RAD(90);
    m_spin = 0;
    m_turn_mode = TURN_SHORTEST;
}

/* 足回りを停止する */
void Chassis::stop(void)
{
    m_speed = 0;
    m_spin = g_IMU.read_spin();
}

/* 各モータの回転数目標値を計算 */
/* speed の値が異常だったら例外を投げる */
void Chassis::calc(double target_rpm[])
{
    /* エラーチェック */
    if (!jibiki::BETWEEN(0.0, m_speed, g_max_speed + 1))
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "\n"
             << "無効な speed\t" << m_speed << std::endl;
        throw sstr.str();
    }

    double rotate = calc_rotate();           /* 回転量を計算 */
    double current_spin = g_IMU.read_spin(); /* 現在の回転角を取得 */

    /* 目標回転数を計算 */
    double raw_rpm[4];
    raw_rpm[g_motor_FR] = m_speed * sin(m_theta - current_spin - M_PI_4) + rotate;
    raw_rpm[g_motor_FL] = m_speed * sin(m_theta - current_spin + M_PI_4) - rotate;
    raw_rpm[g_motor_BR] = m_speed * sin(m_theta - current_spin + M_PI_4) + rotate;
    raw_rpm[g_motor_BL] = m_speed * sin(m_theta - current_spin - M_PI_4) - rotate;

    /* g_motor_x の値が入れ替わったら */
    /* 回転数の目標値を入れ替える */
    target_rpm[g_motor_FR] = raw_rpm[0];
    target_rpm[g_motor_FL] = raw_rpm[1];
    target_rpm[g_motor_BR] = raw_rpm[2];
    target_rpm[g_motor_BL] = raw_rpm[3];

    /* 極性を反転 */
    if (g_inverse_FR)
        target_rpm[g_motor_FR] *= -1;
    if (g_inverse_FL)
        target_rpm[g_motor_FL] *= -1;
    if (g_inverse_BR)
        target_rpm[g_motor_BR] *= -1;
    if (g_inverse_BL)
        target_rpm[g_motor_BL] *= -1;
}

/* 回転量を計算 */
double Chassis::calc_rotate(void)
{
    /* パラメータ */
    const double MinRPM = 20;
    const double MaxRPM = 50;
    const double Kp = 1;

    /* 角度偏差を計算 */
    double diff = calc_angle_diff(m_spin, g_IMU.read_spin(), m_turn_mode);

    /* 偏差が小さくなったら最短モードに変更 */
    /* （オーバーシュートしたときにもう一回転してしまうから） */
    if (fabs(diff) < jibiki::DEG_RAD(90))
        m_turn_mode = TURN_SHORTEST;

    double rotate;
    /* 偏差に応じて rotate を決定 */
    if (fabs(diff) < jibiki::DEG_RAD(2))
    {
        rotate = 0;
    }
    else
    {
        rotate = diff * Kp;
        if (fabs(rotate) < MinRPM)
            rotate = MinRPM * jibiki::GET_SIGNAL(rotate);
        else if (MaxRPM < fabs(rotate))
            rotate = MaxRPM * jibiki::GET_SIGNAL(rotate);
    }

    return rotate;
}