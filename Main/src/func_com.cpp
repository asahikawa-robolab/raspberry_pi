#include "../../Share/inc/_serial_communication.hpp"
#include "../../Share/inc/_picojson.hpp"
#include "../inc/external_variable.hpp"
#include "../inc/module.hpp"
#include "../inc/config.hpp"

/* using 宣言 */
using picojson::object;

/* プロトタイプ宣言 */
static void calc_odometry(cv::Point2f r);

/*-----------------------------------------------
 *
 * スイッチスレーブ
 *
-----------------------------------------------*/
void com_switch_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_switch(paths[0], 1, 3, B57600, "switch");

    /* 送信 */
    com_switch.send();

    /* 受信 */
    if (com_switch.receive())
    {
        /* 送り返すデータを用意 */
        com_switch.m_tx[0] = com_switch.m_rx[0];
        com_switch.m_tx[1] = com_switch.m_rx[1];
        com_switch.m_tx[2] = com_switch.m_rx[2];

        /* 受信したデータを移す */
        g_switch_data.d1 = com_switch.m_rx[0];
        g_switch_data.d2 = com_switch.m_rx[1];
        g_switch_data.d3 = com_switch.m_rx[2];
        g_flags[FLAG_START] = g_switch_data.start;
        g_flags[FLAG_RESET] = g_switch_data.reset;
    }

    /* ComState */
    g_com_data["com_state_switch"] = com_switch.get_rx_freq();
}

/*-----------------------------------------------
 *
 * IMU
 *
-----------------------------------------------*/
void com_IMU_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_IMU(paths[0], 1, 4, B57600, "IMU");

    /* 送信 */
    com_IMU.m_tx[0] = 0;
    com_IMU.send();

    /* 受信 */
    if (com_IMU.receive())
    {
        /* センサの値を g_IMU に代入 */
        double angle = jibiki::DEG_RAD(jibiki::ASBL(com_IMU.m_rx[0], com_IMU.m_rx[1]) / 10.0);
        g_IMU.write_raw_data(angle);
    }

    /* ComState */
    g_com_data["com_state_IMU"] = com_IMU.get_rx_freq();
}

/*-----------------------------------------------
 *
 * Chassis
 *
-----------------------------------------------*/
void com_chassis_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_chassis_R(paths[0], 6, 6, B57600, "chassis_R");
    static jibiki::COM com_chassis_L(paths[1], 6, 6, B57600, "chassis_L");

    /* 回転数目標値を計算 */
    double target_rpm[4];
    g_chassis.calc(target_rpm);

    /* 送信 */
    com_chassis_R.m_tx[0] = 0;
    com_chassis_R.m_tx[1] = jibiki::UP(target_rpm[g_motor_FR]);
    com_chassis_R.m_tx[2] = jibiki::LOW(target_rpm[g_motor_FR]);
    com_chassis_R.m_tx[3] = jibiki::UP(target_rpm[g_motor_BR]);
    com_chassis_R.m_tx[4] = jibiki::LOW(target_rpm[g_motor_BR]);
    com_chassis_R.m_tx[5] = 1;
    com_chassis_R.send();

    com_chassis_L.m_tx[0] = 0;
    com_chassis_L.m_tx[1] = jibiki::UP(target_rpm[g_motor_FL]);
    com_chassis_L.m_tx[2] = jibiki::LOW(target_rpm[g_motor_FL]);
    com_chassis_L.m_tx[3] = jibiki::UP(target_rpm[g_motor_BL]);
    com_chassis_L.m_tx[4] = jibiki::LOW(target_rpm[g_motor_BL]);
    com_chassis_L.m_tx[5] = 1;
    com_chassis_L.send();

    /* 受信 */
    com_chassis_R.receive();
    com_chassis_L.receive();

    /* ComState */
    g_com_data["com_state_chassis_R"] = com_chassis_R.get_rx_freq();
    g_com_data["com_state_chassis_L"] = com_chassis_L.get_rx_freq();

    /* デバッグ */
    // printf("%.0lf, %.0lf, %.0lf, %.0lf\n",
    //        target_rpm[g_motor_FR],
    //        target_rpm[g_motor_FL],
    //        target_rpm[g_motor_BR],
    //        target_rpm[g_motor_BL]);
}

/*-----------------------------------------------
 *
 * Servo
 *
-----------------------------------------------*/
void com_servo_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_servo_R(paths[0], 5, 0, B57600, "servo_R");
    static jibiki::COM com_servo_L(paths[1], 5, 0, B57600, "servo_L");

    /* 送信 */
    com_servo_R.m_tx[0] = 0;
    com_servo_R.m_tx[1] = g_com_data["servo1"];
    com_servo_R.m_tx[2] = g_com_data["servo2"];
    com_servo_R.m_tx[3] = g_com_data["servo3"];
    com_servo_R.m_tx[4] = g_com_data["servo4"];
    com_servo_R.send();

    com_servo_L.m_tx[0] = 0;
    com_servo_L.m_tx[1] = g_com_data["servo5"];
    com_servo_L.m_tx[2] = g_com_data["servo6"];
    com_servo_L.m_tx[3] = g_com_data["servo7"];
    com_servo_L.m_tx[4] = g_com_data["servo8"];
    com_servo_L.send();
}

/*-----------------------------------------------
 *
 * odometry
 *
-----------------------------------------------*/
void com_odometry_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_odometry(paths[0], 4, 8, B57600, "odometry");

    /* 送信 */
    com_odometry.m_tx[0] = 0;
    com_odometry.m_tx[1] = 0;
    com_odometry.m_tx[2] = 0;
    com_odometry.m_tx[3] = 1;
    com_odometry.send();

    /* 受信 */
    if (com_odometry.receive())
    {
        /* タイヤ直径 [mm] */
        const double d = 102.0;

        /* 回転回数を距離に変換 */
        cv::Point2f r;
        r.x = M_PI * d * (jibiki::ASBL(com_odometry.m_rx[0], com_odometry.m_rx[1]) + (jibiki::ASBL(com_odometry.m_rx[2], com_odometry.m_rx[3]) / 360.0));
        r.y = M_PI * d * (jibiki::ASBL(com_odometry.m_rx[4], com_odometry.m_rx[5]) + (jibiki::ASBL(com_odometry.m_rx[6], com_odometry.m_rx[7]) / 360.0));

        /* オドメトリを計算 */
        calc_odometry(r);
    }

    /* ComState */
    g_com_data["com_state_odometry"] = com_odometry.get_rx_freq();
}

static void calc_odometry(cv::Point2f r)
{
    /* r_pre */
    static cv::Point2f r_pre = r;

    /* Δr */
    cv::Point2f dr = r - r_pre;

    /* ΔR */
    cv::Point2f dR;
    double spin = g_IMU.read_spin();
    dR.x = dr.x * cos(spin) - dr.y * sin(spin);
    dR.y = dr.x * sin(spin) + dr.y * cos(spin);

    /* ΣR */
    g_odometry += dR;

    /* 更新 */
    r_pre = r;
}

/*-----------------------------------------------
 *
 * LimitSwitch
 *
-----------------------------------------------*/
void com_limit_switch_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_limit_switch(paths[0], 0, 1, B57600, "limit_switch");

    /* 受信 */
    if (com_limit_switch.receive())
    {
        /* 値を g_com_data に代入 */
        for (size_t i = 0; i < 8; ++i)
        {
            std::stringstream sstr;
            sstr << "limit_switch" << i + 1;
            g_com_data[sstr.str()] =
                (com_limit_switch.m_rx[0] >> i) & 1;
        }
    }

    /* ComState */
    g_com_data["com_state_limit_switch"] = com_limit_switch.get_rx_freq();
}

/*-----------------------------------------------
 *
 * Controller
 *
-----------------------------------------------*/
void com_controller_func(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_controller(paths[0], 0, 8, B57600, "controller");

    /* 受信 */
    if (com_controller.receive())
    {
        g_controller.d1 = com_controller.m_rx[0];
        g_controller.d2 = com_controller.m_rx[1];
        g_controller.d3 = com_controller.m_rx[2];
        g_controller.d4 = com_controller.m_rx[3];
        g_controller.d5 = com_controller.m_rx[4];
        g_controller.d6 = com_controller.m_rx[5];
        g_controller.d7 = com_controller.m_rx[6];
        g_controller.d8 = com_controller.m_rx[7];
    }

    /* ComState */
    g_com_data["com_state_controller"] = com_controller.get_rx_freq();
}

/*-----------------------------------------------
 *
 * テスト
 *
-----------------------------------------------*/
void com_test(std::vector<std::string> paths)
{
    /* デバイスファイルを開く */
    static jibiki::COM com_test(paths[0], 0, 8, B57600, "test");

    /* 受信 */
    if (com_test.receive())
    {
        g_controller.d1 = com_test.m_rx[0];
        g_controller.d2 = com_test.m_rx[1];
        g_controller.d3 = com_test.m_rx[2];
        g_controller.d4 = com_test.m_rx[3];
        g_controller.d5 = com_test.m_rx[4];
        g_controller.d6 = com_test.m_rx[5];
        g_controller.d7 = com_test.m_rx[6];
        g_controller.d8 = com_test.m_rx[7];
    }
}