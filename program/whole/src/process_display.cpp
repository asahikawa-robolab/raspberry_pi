#include <opencv2/opencv.hpp>
#include <sstream>
#include <unistd.h>
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../inc/ext_var.hpp"

const cv::Size WINDOW_SIZE(450, 500);
const cv::Scalar COLOR_WHITE(255, 255, 255);
const cv::Scalar COLOR_RED(50, 50, 255);

/*-----------------------------------------------
*
* テキストを描画
*
-----------------------------------------------*/
void my_text(cv::Mat &img, size_t line, std::string str)
{
    cv::putText(img,
                str.c_str(),
                cv::Point(7, 25 * line),
                cv::FONT_HERSHEY_SIMPLEX,
                0.6,
                COLOR_WHITE);
}

/*-----------------------------------------------
*
* current_method を描画
*
-----------------------------------------------*/
void draw_current_method(cv::Mat &img,
                         jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method,
                         size_t line)
{
    my_text(img, line, "*** current method ***");

    if (current_method.read() == jibiki::thread::OPERATE_AUTO)
        my_text(img, line + 1, "AUTO");
    else
        my_text(img, line + 1, "MANUAL");
}

/*-----------------------------------------------
*
* 足回りの情報を描画（speed, theta, spin）
*
-----------------------------------------------*/
void draw_chassis(cv::Mat &img, Chassis &chassis, size_t line)
{
    std::stringstream sstr;
    sstr << "speed : " << (int)chassis.m_speed.read()
         << ", theta : " << (int)jibiki::rad_deg(chassis.m_theta.read())
         << ", spin : " << (int)jibiki::rad_deg(chassis.m_spin.read());
    my_text(img, line, "*** chassis ***");
    my_text(img, line + 1, sstr.str());
}

/*-----------------------------------------------
*
* コントローラの情報を描画
*
-----------------------------------------------*/
void draw_controller(cv::Mat &img, Controller &controller, size_t line)
{
    std::stringstream sstr;
    sstr << "mu : " << controller.tact_mu()
         << ", ld : " << controller.tact_ld()
         << ", md : " << controller.tact_md()
         << ", rd : " << controller.tact_rd();
    my_text(img, line, "*** controller ***");
    my_text(img, line + 1, sstr.str());
}

/*-----------------------------------------------
*
* executing_order を描画
*
-----------------------------------------------*/
void draw_executing_order(cv::Mat &img,
                          jibiki::ShareVarVec<std::string> &executing_order,
                          size_t line)
{
    my_text(img, line, "*** executing_order ***");
    for (size_t i = 0; i < executing_order.size(); ++i)
        my_text(img, i + line + 1, executing_order.read(i));
}

/*-----------------------------------------------
*
* アナログスティックに関するデータを描画
*
-----------------------------------------------*/
void draw_analog_stick(cv::Mat &img,
                       Controller &controller,
                       size_t margin,
                       double scale)
{
    double ridge = 256 * scale; /* 辺の長さ */
    const cv::Point SHIFT(margin, WINDOW_SIZE.height - ridge - margin);
    /* 四角 */
    cv::rectangle(img,
                  cv::Point(0, 0) + SHIFT,
                  cv::Point(ridge, ridge) + SHIFT,
                  COLOR_WHITE);
    /* 点 */
    cv::Point center_pos =
        cv::Point(controller.l_analog_stick_h() - 127,
                  -controller.l_analog_stick_v() + 127) *
            scale +
        SHIFT + cv::Point(ridge, ridge) / 2;
    cv::circle(img, center_pos, 2, cv::Scalar(0, 0, 255), -1);
    /* 矢印 */
    cv::Point arrow_from = SHIFT + cv::Point(ridge, ridge) / 2;
    double speed = controller.speed(Controller::MODE_L, Controller::DIR_INF);
    double theta = controller.theta(Controller::MODE_L, Controller::DIR_INF);
    cv::Point arrow_to =
        (cv::Point)(cv::Point2f(cos(theta), -sin(theta)) * speed / 100 * ridge / 2) +
        arrow_from;
    cv::arrowedLine(img, arrow_from, arrow_to, COLOR_WHITE);
    /* AnalogStick */
    my_text(img, 12, "    AnalogStick");
}

/*-----------------------------------------------
*
* ロボットの状態を描画
*
-----------------------------------------------*/
cv::Point rot(cv::Point in, double theta)
{
    theta *= -1; /* ウィンドウ座標系に合わせて回転方向を反転 */
    cv::Point out;
    out.x = in.x * cos(theta) - in.y * sin(theta);
    out.y = in.x * sin(theta) + in.y * cos(theta);
    return out;
}
void draw_robot(cv::Mat img, Chassis &chassis, Imu &imu, size_t margin, double scale)
{
    double ridge = 256 * scale;                                 /* 辺の長さ */
    double wheel_offset = 40 * scale;                           /* 角から車輪までの距離 */
    double wheel_base_size = 100.0 / chassis.max_rpm() * scale; /* 車輪の基本の大きさ */

    const cv::Point SHIFT(WINDOW_SIZE.width - ridge / 2 - margin,
                          WINDOW_SIZE.height - ridge / 2 - margin);

    /*-----------------------------------------------
    座標を計算
    -----------------------------------------------*/
    /* ロボットの角 */
    cv::Point corner_fr = cv::Point(ridge / 2, -ridge / 2);
    cv::Point corner_fl = cv::Point(-ridge / 2, -ridge / 2);
    cv::Point corner_br = cv::Point(ridge / 2, ridge / 2);
    cv::Point corner_bl = cv::Point(-ridge / 2, ridge / 2);
    /* 車輪の中心位置 */
    cv::Point wheel_fr = corner_fr + cv::Point(-wheel_offset, wheel_offset);
    cv::Point wheel_fl = corner_fl + cv::Point(wheel_offset, wheel_offset);
    cv::Point wheel_br = corner_br + cv::Point(-wheel_offset, -wheel_offset);
    cv::Point wheel_bl = corner_bl + cv::Point(wheel_offset, -wheel_offset);
    /* 車輪の速度を表す矢印 */
    cv::Point wheel_fr_from = wheel_fr + cv::Point(cv::Point2f(cos(M_PI_4), sin(M_PI_4)) * chassis.raw_fr() * wheel_base_size);
    cv::Point wheel_fr_to = wheel_fr + cv::Point(cv::Point2f(-cos(M_PI_4), -sin(M_PI_4)) * chassis.raw_fr() * wheel_base_size);
    cv::Point wheel_fl_from = wheel_fl + cv::Point(cv::Point2f(-cos(M_PI_4), sin(M_PI_4)) * chassis.raw_fl() * wheel_base_size);
    cv::Point wheel_fl_to = wheel_fl + cv::Point(cv::Point2f(cos(M_PI_4), -sin(M_PI_4)) * chassis.raw_fl() * wheel_base_size);
    cv::Point wheel_br_from = wheel_br + cv::Point(cv::Point2f(-cos(M_PI_4), sin(M_PI_4)) * chassis.raw_br() * wheel_base_size);
    cv::Point wheel_br_to = wheel_br + cv::Point(cv::Point2f(cos(M_PI_4), -sin(M_PI_4)) * chassis.raw_br() * wheel_base_size);
    cv::Point wheel_bl_from = wheel_bl + cv::Point(cv::Point2f(cos(M_PI_4), sin(M_PI_4)) * chassis.raw_bl() * wheel_base_size);
    cv::Point wheel_bl_to = wheel_bl + cv::Point(cv::Point2f(-cos(M_PI_4), -sin(M_PI_4)) * chassis.raw_bl() * wheel_base_size);
    /*-----------------------------------------------
    座標変換
    -----------------------------------------------*/
    /* ロボットの角 */
    corner_fr = rot(corner_fr, imu.read()) + SHIFT;
    corner_fl = rot(corner_fl, imu.read()) + SHIFT;
    corner_br = rot(corner_br, imu.read()) + SHIFT;
    corner_bl = rot(corner_bl, imu.read()) + SHIFT;
    /* 車輪の中心位置 */
    wheel_fr = rot(wheel_fr, imu.read()) + SHIFT;
    wheel_fl = rot(wheel_fl, imu.read()) + SHIFT;
    wheel_br = rot(wheel_br, imu.read()) + SHIFT;
    wheel_bl = rot(wheel_bl, imu.read()) + SHIFT;
    /* 車輪の速度を表す矢印 */
    wheel_fr_from = rot(wheel_fr_from, imu.read()) + SHIFT;
    wheel_fr_to = rot(wheel_fr_to, imu.read()) + SHIFT;
    wheel_fl_from = rot(wheel_fl_from, imu.read()) + SHIFT;
    wheel_fl_to = rot(wheel_fl_to, imu.read()) + SHIFT;
    wheel_br_from = rot(wheel_br_from, imu.read()) + SHIFT;
    wheel_br_to = rot(wheel_br_to, imu.read()) + SHIFT;
    wheel_bl_from = rot(wheel_bl_from, imu.read()) + SHIFT;
    wheel_bl_to = rot(wheel_bl_to, imu.read()) + SHIFT;
    /*-----------------------------------------------
    描画
    -----------------------------------------------*/
    /* ロボットの角 */
    cv::line(img, corner_fl, corner_fr, COLOR_WHITE);
    cv::line(img, corner_fr, corner_br, COLOR_WHITE);
    cv::line(img, corner_br, corner_bl, COLOR_WHITE);
    cv::line(img, corner_bl, corner_fl, COLOR_WHITE);
    /* 車輪の速度を表す矢印 */
    cv::arrowedLine(img, wheel_fr_from, wheel_fr_to, COLOR_RED);
    cv::arrowedLine(img, wheel_fl_from, wheel_fl_to, COLOR_RED);
    cv::arrowedLine(img, wheel_br_from, wheel_br_to, COLOR_RED);
    cv::arrowedLine(img, wheel_bl_from, wheel_bl_to, COLOR_RED);
    /* ロボットの移動を表す矢印 */
    cv::Point motion_arrow =
        cv::Point(cv::Point2f(cos(chassis.m_theta.read()),
                              -sin(chassis.m_theta.read())) *
                  chassis.m_speed.read() * 100.0 / chassis.max_rpm());
    cv::arrowedLine(img, SHIFT, motion_arrow + SHIFT, COLOR_WHITE);
    /* Chassis */
    my_text(img, 12, "                               Chassis");
}

/*-----------------------------------------------
*
* main
*
-----------------------------------------------*/
void thread_display(jibiki::ShareVar<bool> &exit_flag,
                    jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method,
                    jibiki::ShareVarVec<std::string> &executing_order,
                    jibiki::ShareVar<int> &pushed_key)
{
    try
    {
        if (!jibiki::thread::enable("display"))
            return;

        cv::Mat img = cv::Mat::zeros(WINDOW_SIZE, CV_8UC3); /* 画像を作成 */
        cv::namedWindow("window");                          /* ウィンドウを作成 */
        cv::moveWindow("window", 400, 40);                  /* ウィンドウを移動 */

        while (jibiki::thread::manage(exit_flag))
        {
            /* 重くならないように間隔を空ける */
            usleep(50E3);

            /* 描画データの用意 */
            draw_current_method(img, current_method, 1);
            draw_chassis(img, g_chassis, 3);
            draw_controller(img, g_controller, 5);
            draw_executing_order(img, executing_order, 7);
            draw_analog_stick(img, g_controller, 40, 0.5);
            draw_robot(img, g_chassis, g_imu, 40, 0.5);

            /* 描画 */
            cv::imshow("window", img);                  /* ウィンドウに描画 */
            img = cv::Mat::zeros(WINDOW_SIZE, CV_8UC3); /* 画像をクリア */
            pushed_key = cv::waitKey(1);                /* キー入力 */
        }
    }
    catch (const std::exception &e)
    {
        jibiki::print_err(__PRETTY_FUNCTION__);
        exit_flag = true;
        return; /* 最上部 */
    }
}