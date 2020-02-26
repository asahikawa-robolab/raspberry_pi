#include <iostream>
#include <cstdlib>
#include "../inc/external_variable.hpp"
#include "../inc/_utility.hpp"
#include "../inc/config.hpp"

typedef enum
{
    DIRECTION_8,
    DIRECTION_4,
    DIRECTION_NONE,
} AnalogStickDirection;

/*-----------------------------------------------
 *
 * アナログスティックをベクトルとみなしたときの
 * 大きさと向き（角度）を返す
 * double *theta には静的変数のポインタを渡す
 *
-----------------------------------------------*/
void calc_analog_stick(double *size,
                       double *theta,
                       uint8_t data_horizontal,
                       uint8_t data_vertical,
                       AnalogStickDirection analog_stick_direction)
{
    /*-----------------------------------------------
    0 を中心にする
    -----------------------------------------------*/
    double x = (double)data_horizontal - 255 / 2.0;
    double y = (double)data_vertical - 255 / 2.0;

    /*-----------------------------------------------
    大きさを計算
    -----------------------------------------------*/
    const double insensible_field_size = 30; /* 不感領域の大きさ（原点中心の正方形の辺長 ÷ 2） */
    double tmp_size = sqrt(x * x + y * y);   /* 大きさ（原点からの距離）を求める */
    if (fabs(x) < insensible_field_size &&
        fabs(y) < insensible_field_size) /* 不感領域にいる場合 0 にする */
        tmp_size = 0;
    tmp_size *= 100 / 127.5;                   /* 百分率に変換 */
    *size = (tmp_size > 100) ? 100 : tmp_size; /* 100 以下に収める */

    /*-----------------------------------------------
    向き（角度）を計算
    -----------------------------------------------*/
    double tmp_theta = atan2(y, x);
    if (tmp_size != 0)
    {
        /* ８方向に丸める */
        if (analog_stick_direction == DIRECTION_8)
        {
            for (int i = -4; i <= 4; ++i)
                if (jibiki::BETWEEN(M_PI_4 * i + -M_PI_4 / 2, tmp_theta, M_PI_4 * i + M_PI_4 / 2))
                    tmp_theta = M_PI_4 * i;
        }
        /* ４方向 */
        else if (analog_stick_direction == DIRECTION_4)
        {
            for (int i = -2; i <= 2; ++i)
                if (jibiki::BETWEEN(M_PI_2 * i + -M_PI_2 / 2, tmp_theta, M_PI_2 * i + M_PI_2 / 2))
                    tmp_theta = M_PI_2 * i;
        }
        *theta = tmp_theta;
    }
}

/*-----------------------------------------------
 *
 * main
 *
-----------------------------------------------*/
void process_operate_manual(void)
{
    printf("[start operate_keyboard]\n");
    atexit([]() { printf("[end operate_keyboard]\n"); });

    double l_stick_size = 0;
    double l_stick_theta = jibiki::DEG_RAD(90);

    /*-----------------------------------------------
    設定ファイル読み込み
    -----------------------------------------------*/
    try
    {
        picojson::value json_value;
        json_value =
            jibiki::load_json_file("setting.json");
        /* 終了 */
        if (json_value
                .get<picojson::object>()["enable"]
                .get<picojson::object>()["process_operate_manual"]
                .get<bool>() == false)
            return;
    }
    catch (std::string err)
    {
        std::cout << "*** error ***\n"
                  << err << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(EXIT_FAILURE);
    }

    /*-----------------------------------------------
    メインループ
    -----------------------------------------------*/
    while (manage_process(OPERATE_MANUAL))
    {

        /* アナログスティックの値を計算する */
        calc_analog_stick(&l_stick_size,
                          &l_stick_theta,
                          g_controller.l_analog_stick_h,
                          g_controller.l_analog_stick_v,
                          DIRECTION_8);

        /* 通信が止まったら安全のために足回りを停止する */
        if (g_com_data["com_state_controller"] == 0)
        {
            g_chassis.stop();
        }
        else
        {
            g_chassis.m_speed = l_stick_size;
            g_chassis.m_theta = l_stick_theta;
        }

        /* spin */
        if (g_controller.tact_ld)
            g_chassis.m_spin = jibiki::DEG_RAD(90);
        else if (g_controller.tact_mu)
            g_chassis.m_spin = jibiki::DEG_RAD(0);
        else if (g_controller.tact_rd)
            g_chassis.m_spin = jibiki::DEG_RAD(-90);
        else if (g_controller.tact_md)
            g_chassis.m_spin = jibiki::DEG_RAD(180);
    }
}