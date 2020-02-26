#include <opencv2/opencv.hpp>
#include "../inc/external_variable.hpp"
#include "../inc/_process_operate_auto.hpp"
#include "../inc/config.hpp"

typedef enum
{
    TEXT_RIGHT,
    TEXT_LEFT,
} TextPos;

/* プロトタイプ宣言 */
static void my_text(cv::Mat &img, const char *str, double line, TextPos text_pos, cv::Scalar color);
static void draw_partition(cv::Mat &img);
static void draw_arrow(cv::Mat &img);
static void add_com_state_marker(double freq, char *str);
static void draw_up(cv::Mat &img);
static void draw_middle(cv::Mat &img);
static void draw_low(cv::Mat &img);

/* 色 */
const cv::Scalar C_WHITE = CV_RGB(255, 255, 255);
const cv::Scalar C_BLACK = CV_RGB(0, 0, 0);
const cv::Scalar C_RED = CV_RGB(255, 0, 0);

/*-----------------------------------------------
 *
 * main
 *
-----------------------------------------------*/
void func_draw_main(cv::Mat &img)
{
    draw_up(img);        /* 上段に文字列を描画 */
    draw_middle(img);    /* 中段に文字列を描画 */
    draw_low(img);       /* 下段に文字列を描画 */
    draw_partition(img); /* 仕切りを描画 */
    draw_arrow(img);     /* 矢印を描画 */
}

/*-----------------------------------------------
 *
 * 位置を指定して文字列を描画
 *
-----------------------------------------------*/
static void my_text(cv::Mat &img, const char *str, double line, TextPos text_pos, cv::Scalar color)
{
    if (text_pos == TEXT_LEFT)
        cv::putText(img, str,
                    cv::Point(7, 16 * line),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 1);
    else if (text_pos == TEXT_RIGHT)
        cv::putText(img, str,
                    cv::Point(7 + WINDOW_SIZE.width / 2, 16 * line),
                    cv::FONT_HERSHEY_SIMPLEX, 0.4, color, 1);
}

/*-----------------------------------------------
 *
 * 仕切りを描画
 *
-----------------------------------------------*/
static void draw_partition(cv::Mat &img)
{
    /* 縦線を引く */
    cv::line(img,
             cv::Point(WINDOW_SIZE.width / 2, 0),
             cv::Point(WINDOW_SIZE.width / 2, WINDOW_SIZE.height - 100),
             C_WHITE);
    /* 横線を引く */
    cv::line(img,
             cv::Point(0, WINDOW_SIZE.height / 2 - 80),
             cv::Point(WINDOW_SIZE.width, WINDOW_SIZE.height / 2 - 80),
             C_WHITE);
    cv::line(img,
             cv::Point(0, WINDOW_SIZE.height - 100),
             cv::Point(WINDOW_SIZE.width, WINDOW_SIZE.height - 100),
             C_WHITE);
}

/*-----------------------------------------------
 *
 * ロボットの動きを表す矢印を描画
 *
-----------------------------------------------*/
static void draw_arrow(cv::Mat &img)
{
    const cv::Point center_pos(WINDOW_SIZE.width / 4,
                               WINDOW_SIZE.height - 190);
    const double max_size = 150;

    double size = g_chassis.m_speed / g_max_speed * max_size;
    double theta = g_chassis.m_theta;
    cv::Point arrow_tip =
        cv::Point(size * cos(theta), size * sin(-theta)) +
        center_pos;

    cv::arrowedLine(img, center_pos, arrow_tip, C_WHITE);
}

/*-----------------------------------------------
 *
 * ComState のマーカー（*）を追加する
 *
-----------------------------------------------*/
static void add_com_state_marker(double freq, char *str)
{
    /* パラメータ */
    const double max_freq = 150;
    const size_t max_name_len = 15;

    /* マーカーの位置を揃える */
    sprintf(str, "%*s", max_name_len, "");

    /* マーカーを追加 */
    size_t marker_num = (size_t)((freq / max_freq) * 8);
    for (size_t i = 0; i < marker_num; ++i)
        strcat(str, "*");
}

/*-----------------------------------------------
 *
 * 文字列を描画
 *
-----------------------------------------------*/
/* 上段 */
static void draw_up(cv::Mat &img)
{
    char str[100];

    switch (g_flags[FLAG_OPERATE])
    {
    case OPERATE_NONE:
        snprintf(str, 100, "OPERATE : NONE");
        break;
    case OPERATE_AUTO:
        snprintf(str, 100, "OPERATE : AUTO");
        break;
    case OPERATE_MANUAL:
        snprintf(str, 100, "OPERATE : MANUAL");
        break;
    case OPERATE_KEYBOARD:
        snprintf(str, 100, "OPERATE : KEYBOARD");
        break;
    }
    my_text(img, str, 1, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    snprintf(str, 100, "START : %d",
             g_flags[FLAG_START]);
    my_text(img, str, 2, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    snprintf(str, 100, "RESET : %d",
             g_flags[FLAG_RESET]);
    my_text(img, str, 3, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    snprintf(str, 100, "mode keyboard :");
    my_text(img, str, 4, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    snprintf(str, 100, " %s",
             g_keyboard_mode[g_keyboard_mode_cnt].c_str());
    my_text(img, str, 5, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    /* ------------------------------------------- */
    my_text(img, "odometry", 1, TEXT_RIGHT, C_WHITE);
    snprintf(str, 100, " %.0lf, %.0lf",
             g_odometry.x, g_odometry.y);
    my_text(img, str, 2, TEXT_RIGHT, C_WHITE);
    /* ------------------------------------------- */
    my_text(img, "limit switch", 3, TEXT_RIGHT, C_WHITE);
    snprintf(str, 100, " %.0lf %.0lf %.0lf %.0lf %.0lf %.0lf %.0lf %.0lf",
             g_com_data["limit_switch1"],
             g_com_data["limit_switch2"],
             g_com_data["limit_switch3"],
             g_com_data["limit_switch4"],
             g_com_data["limit_switch5"],
             g_com_data["limit_switch6"],
             g_com_data["limit_switch7"],
             g_com_data["limit_switch8"]);
    my_text(img, str, 4, TEXT_RIGHT, C_WHITE);
    /* ------------------------------------------- */
    snprintf(str, 100, "servo : %.0lf",
             g_com_data["servo1"]);
    my_text(img, str, 5, TEXT_RIGHT, C_WHITE);
}
/* 中段 */
static void draw_middle(cv::Mat &img)
{
    char str[100];

    snprintf(str, 100, "%+4.0lf %+4.0lf %+4.0lf/%+4.0f",
             g_chassis.m_speed,
             jibiki::RAD_DEG(g_chassis.m_theta),
             jibiki::RAD_DEG(g_chassis.m_spin),
             jibiki::RAD_DEG(g_IMU.read_spin()));
    my_text(img, str, 19.5, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    /* ------------------------------------------- */
    my_text(img, "switch", 9, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_switch"], str);
    my_text(img, str, 9, TEXT_RIGHT, C_WHITE);
    my_text(img, "IMU", 10, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_IMU"], str);
    my_text(img, str, 10, TEXT_RIGHT, C_WHITE);
    my_text(img, "chassis R", 11, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_chassis_R"], str);
    my_text(img, str, 11, TEXT_RIGHT, C_WHITE);
    my_text(img, "chassis L", 12, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_chassis_L"], str);
    my_text(img, str, 12, TEXT_RIGHT, C_WHITE);
    my_text(img, "odometry", 13, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_odometry"], str);
    my_text(img, str, 13, TEXT_RIGHT, C_WHITE);
    my_text(img, "limit switch", 14, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_limit_switch"], str);
    my_text(img, str, 14, TEXT_RIGHT, C_WHITE);
    my_text(img, "controller", 15, TEXT_RIGHT, C_WHITE);
    add_com_state_marker(g_com_data["com_state_controller"], str);
    my_text(img, str, 15, TEXT_RIGHT, C_WHITE);
    /* ------------------------------------------- */
}
/* 下段 */
static void draw_low(cv::Mat &img)
{
    char str[100];

    snprintf(str, 100, "load order : %s",
             g_load_order
                 .m_order_list[g_load_order.m_index_cnt]
                 .c_str());
    my_text(img, str, 21, TEXT_LEFT, C_WHITE);
    /* ------------------------------------------- */
    for (size_t i = 0; i < g_executing_order.size(); ++i)
    {
        snprintf(str, 100, g_executing_order[i].c_str());
        my_text(img, str, 22 + i, TEXT_LEFT, C_WHITE);
    }
}