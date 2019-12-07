#include <opencv2/opencv.hpp>
#include <unistd.h>
#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../../Share/inc/_ExternalVariable.hpp"
#include "../../Share/inc/_Class.hpp"
#include "../inc/Orders.hpp"

#define C_WHITE CV_RGB(255, 255, 255)
#define C_BLACK CV_RGB(0, 0, 0)
#define C_RED CV_RGB(255, 0, 0)

#define RECT_SIZE 120
#define RRID_SIZE 40
#define RECT_CENTER cv::Point(150, 275)
#define TEXT_POS -15

/* ウィンドウサイズ，位置 */
#define WINDOW_SIZE cv::Size(450, 420)
#define WINDOW_POS cv::Point(340, 35)

void myText(cv::Mat &img, const char *str, int pos, cv::Scalar Color);
void Draw(const char *window, cv::Mat &img);
void DrawStr(cv::Mat &img, const int indent1, const int indent2, double spin, MOTION &motion);
void DrawStr_A(cv::Mat &img, const int indent1, const int indent2);
void DrawStr_B(cv::Mat &img, const int indent1, const int indent2);
void DrawComState(cv::Mat &img, int indent1, int indent2, const char *name, int pos, int com_state_num);
void DrawRobotState(cv::Mat &img, double spin, MOTION &motion);

char ExeStr[MAX_STRING_LENGTH];
char OrderPathStr[MAX_STRING_LENGTH];

void ProcessDisplay(void)
{
#if ENABLE_DISPLAY

    /* ウィンドウ */
    cv::Mat img = cv::Mat::zeros(WINDOW_SIZE, CV_8UC3);
    cv::namedWindow("info");
    cv::moveWindow("info", WINDOW_POS.x, WINDOW_POS.y);

    /* 表示データ */
    ORDERS Orders;

    /* インデント */
    const int indent1 = 33, indent2 = 13;

    /* ウィンドウの表示タイミングを調整 */
    usleep(100E3);

    while (_Flags.p())
    {
        double spin = _Chassis.read_spin();
        MOTION motion = _Chassis.read_motion();
        Orders.read_exe(ExeStr);
        Orders.read_order_path(OrderPathStr);

        /* 文字列を描画 */
        DrawStr(img, indent1, indent2, spin, motion);
        DrawStr_A(img, indent1, indent2);
        DrawStr_B(img, indent1, indent2);

        /* ロボットの状態を描画 */
        DrawRobotState(img, spin, motion);

        /* 描画する */
        Draw("info", img);

        /*-----------------------------------------------
        キー入力受付
        -----------------------------------------------*/
        int key = cv::waitKey(1);
        if (key > 0)
        {
            _Flags.f[FLAG_KEY] = key;
        }
    }

#endif
}

void myText(cv::Mat &img, const char *str, int pos, cv::Scalar Color)
{
    cv::putText(img, str, cv::Point(10, 20 * pos), cv::FONT_HERSHEY_SIMPLEX, 0.5, Color, 1);
}

void Draw(const char *window, cv::Mat &img)
{
    cv::imshow(window, img);
    img = cv::Mat::zeros(WINDOW_SIZE, CV_8UC3);
}

/*-----------------------------------------------
 *
 * 文字列を描画
 *
-----------------------------------------------*/
void DrawStr(cv::Mat &img, const int indent1, const int indent2, double spin, MOTION &motion)
{
    char str[MAX_STRING_LENGTH];
    /* -------------------------------------------------------------------------- */
    myText(img, ExeStr, 1, C_WHITE);
    /* -------------------------------------------------------------------------- */
    myText(img, OrderPathStr, 2, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "Motion : (%+3.0lf, %+3.0lf)",
             motion.speed, RAD_DEG(motion.theta));
    myText(img, str, 3, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str,
             MAX_STRING_LENGTH,
             "spin %+.2lf -> %+.2lf",
             RAD_DEG(spin),
             RAD_DEG(_Chassis.get_target_spin()));
    myText(img, str, 4, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "odometry : (%+4.0lf, %+4.0lf)",
             _Odometry.v->x, _Odometry.v->y);
    myText(img, str, 5, C_WHITE);
    /* -------------------------------------------------------------------------- */
    if (_Flags.f[FLAG_OPERATE] == OPERATE_AUTO)
    {
        snprintf(str, MAX_STRING_LENGTH, "%*sOPERATE : AUTO", indent1, "");
    }
    else
    {
        snprintf(str, MAX_STRING_LENGTH, "%*sOPERATE : MANUAL", indent1, "");
    }
    myText(img, str, 14, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%*sKEY : %c (%d)",
             indent1, "", _Flags.f[FLAG_KEY], (int)_Flags.f[FLAG_KEY]);
    myText(img, str, 15, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%*sSTART : %d", indent1, "", _Flags.f[FLAG_START]);
    myText(img, str, 16, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%*sEMERGENCY : %d", indent1, "", _Flags.f[FLAG_EMERGENCY]);
    myText(img, str, 17, C_WHITE);
    /* -------------------------------------------------------------------------- */
}

void DrawStr_A(cv::Mat &img, const int indent1, const int indent2)
{
#ifdef TEAM_A
    char str[MAX_STRING_LENGTH];
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d %d : %d : front",
             _Line.read("front", 7), _Line.read("front", 6),
             _Line.read("front", 5), _Line.read("front", 4),
             _Line.read("front", 3), _Line.read("front", 2),
             _Line.read("front", 1), _Line.read("front", 0),
             _Limit.read("front"));
    myText(img, str, 6, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d %d : %d : right",
             _Line.read("right", 7), _Line.read("right", 6),
             _Line.read("right", 5), _Line.read("right", 4),
             _Line.read("right", 3), _Line.read("right", 2),
             _Line.read("right", 1), _Line.read("right", 0),
             _Limit.read("right"));
    myText(img, str, 7, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d %d : %d : left",
             _Line.read("left", 7), _Line.read("left", 6),
             _Line.read("left", 5), _Line.read("left", 4),
             _Line.read("left", 3), _Line.read("left", 2),
             _Line.read("left", 1), _Line.read("left", 0),
             _Limit.read("left"));
    myText(img, str, 8, C_WHITE);
    /* -------------------------------------------------------------------------- */

    /* ComState */
    DrawComState(img, indent1, indent2, "SERVO", 2, COM_STATE_SERVO);
    DrawComState(img, indent1, indent2, "EMERGENCY", 3, COM_STATE_EMERGENCY);
    DrawComState(img, indent1, indent2, "SWITCH", 4, COM_STATE_SWITCH);
    DrawComState(img, indent1, indent2, "ANGLE", 5, COM_STATE_ANGLE);
    DrawComState(img, indent1, indent2, "CHASSIS", 6, COM_STATE_CHASSIS);
    DrawComState(img, indent1, indent2, "GND", 7, COM_STATE_GND_ENCODER);
    DrawComState(img, indent1, indent2, "IMU", 8, COM_STATE_IMU);
    DrawComState(img, indent1, indent2, "LIMIT", 9, COM_STATE_LIMIT);
    DrawComState(img, indent1, indent2, "LINE", 10, COM_STATE_LINE_TRACER);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%*sbath1 %d, bath2 %d",
             indent1, "",
             _Limit.read("bath1"), _Limit.read("bath2"));
    myText(img, str, 12, C_WHITE);
    /* -------------------------------------------------------------------------- */
#endif
}

void DrawStr_B(cv::Mat &img, const int indent1, const int indent2)
{
#ifdef TEAM_B
    char str[MAX_STRING_LENGTH];
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d %d : front",
             _Line.read("front", 7), _Line.read("front", 6),
             _Line.read("front", 5), _Line.read("front", 4),
             _Line.read("front", 3), _Line.read("front", 2),
             _Line.read("front", 1), _Line.read("front", 0));
    myText(img, str, 6, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d %d : right",
             _Line.read("right", 7), _Line.read("right", 6),
             _Line.read("right", 5), _Line.read("right", 4),
             _Line.read("right", 3), _Line.read("right", 2),
             _Line.read("right", 1), _Line.read("right", 0));
    myText(img, str, 7, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d %d : left",
             _Line.read("left", 7), _Line.read("left", 6),
             _Line.read("left", 5), _Line.read("left", 4),
             _Line.read("left", 3), _Line.read("left", 2),
             _Line.read("left", 1), _Line.read("left", 0));
    myText(img, str, 8, C_WHITE);
    /* -------------------------------------------------------------------------- */
    snprintf(str, MAX_STRING_LENGTH, "%d %d %d %d %d %d %d  : limit",
            _Limit.read("lim_L_S"),_Limit.read("lim_right"),
            _Limit.read("lim_left"),_Limit.read("lim_R_S"),
            _Limit.read("lim_R_B_low"),_Limit.read("lim_R_B_up"),
            _Limit.read("lim_front"));
    myText(img, str, 9, C_WHITE);
    /* -------------------------------------------------------------------------- */

    DrawComState(img, indent1, indent2, "SERVO", 2, COM_STATE_SERVO);
    DrawComState(img, indent1, indent2, "EMERGENCY", 3, COM_STATE_EMERGENCY);
    DrawComState(img, indent1, indent2, "SWITCH", 4, COM_STATE_SWITCH);
    DrawComState(img, indent1, indent2, "ANGLE", 5, COM_STATE_ANGLE);
    DrawComState(img, indent1, indent2, "CHASSIS", 6, COM_STATE_CHASSIS);
    DrawComState(img, indent1, indent2, "GND", 7, COM_STATE_GND_ENCODER);
    DrawComState(img, indent1, indent2, "IMU", 8, COM_STATE_IMU);
    DrawComState(img, indent1, indent2, "LIMIT", 9, COM_STATE_LIMIT);
    DrawComState(img, indent1, indent2, "LINE", 10, COM_STATE_LINE_TRACER);
    DrawComState(img, indent1, indent2, "MOTOR", 11, COM_STATE_MOTOR);
#endif
}

void DrawComState(cv::Mat &img, int indent1, int indent2, const char *name, int pos, int com_state_num)
{
    char str[MAX_STRING_LENGTH];
    snprintf(str, MAX_STRING_LENGTH, "%*s%s", indent1, "", name);
    myText(img, str, pos, C_WHITE);
    snprintf(str, MAX_STRING_LENGTH, "%*s", indent1 + indent2, "");
    for (int i = 0; i < _ComState.cnt[com_state_num]; i++)
    {
        strcat(str, "*");
        if (i > 5)
        {
            break;
        }
    }
    _ComState.cnt[com_state_num] = 0;
    myText(img, str, pos, C_WHITE);
}

/*-----------------------------------------------
 *
 * ロボットの状態を描画する
 *
-----------------------------------------------*/
void DrawRobotState(cv::Mat &img, double spin, MOTION &motion)
{
    /*"F","B"の表示*/
    char str[MAX_STRING_LENGTH];
    cv::Point Text_F(RECT_CENTER.x - 8 + (TEXT_POS - 10 + RECT_SIZE) * cos(-(spin + (M_PI / 2))), RECT_CENTER.y + (TEXT_POS - 10 + RECT_SIZE) * sin(-(spin + (M_PI / 2))));
    sprintf(str, "F");
    cv::putText(img, str, Text_F, cv::FONT_HERSHEY_SIMPLEX, 0.5, C_WHITE, 1);

    cv::Point Text_B(RECT_CENTER.x - 8 + (TEXT_POS + RECT_SIZE) * cos(-(spin + (3 * M_PI / 2))), RECT_CENTER.y + (TEXT_POS + RECT_SIZE) * sin(-(spin + (3 * M_PI / 2))));
    sprintf(str, "B");
    cv::putText(img, str, Text_B, cv::FONT_HERSHEY_SIMPLEX, 0.5, C_WHITE, 1);

    /*進行方向の矢印 */
    cv::Point RRID(RECT_CENTER.x + RRID_SIZE * cos(-((motion.theta))), RECT_CENTER.y + RRID_SIZE * sin(-((motion.theta))));
    cv::arrowedLine(img, RECT_CENTER, RRID, C_RED, 1, 8, 0);

    /*四角を描く */
    cv::Point Squa_FR(RECT_CENTER.x + RECT_SIZE * cos(-(spin + (M_PI / 4))), RECT_CENTER.y + RECT_SIZE * sin(-(spin + (M_PI / 4))));
    cv::Point Squa_FL(RECT_CENTER.x + RECT_SIZE * cos(-(spin + (3 * M_PI / 4))), RECT_CENTER.y + RECT_SIZE * sin(-(spin + (3 * M_PI / 4))));
    cv::Point Squa_BL(RECT_CENTER.x + RECT_SIZE * cos(-(spin + (5 * M_PI / 4))), RECT_CENTER.y + RECT_SIZE * sin(-(spin + (5 * M_PI / 4))));
    cv::Point Squa_BR(RECT_CENTER.x + RECT_SIZE * cos(-(spin + (7 * M_PI / 4))), RECT_CENTER.y + RECT_SIZE * sin(-(spin + (7 * M_PI / 4))));

    cv::line(img, Squa_FR, Squa_FL, C_WHITE, 1, 8, 0);
    cv::line(img, Squa_FL, Squa_BL, C_WHITE, 1, 8, 0);
    cv::line(img, Squa_BL, Squa_BR, C_WHITE, 1, 8, 0);
    cv::line(img, Squa_BR, Squa_FR, C_WHITE, 1, 8, 0);

    /*矢印を描く */
    //ARROW_SIZE:0の時の線の大きさ
    //cv::Point 矢印のポイント((ARROW_SIZE/* +_Chassis.rpm[0]*/)*cos(3*M_PI/4)+ARROW_FR.x,(ARROW_SIZE/* +_Chassis.rpm[0]*/)*sin(3*M_PI/4)+ARROW_FR.y);
    double Arrow_FR_SIZE = 50 * _Actuator.read("FR") * 3 / SPEED_MAX;
    double Arrow_FL_SIZE = 50 * _Actuator.read("FL") * 3 / SPEED_MAX;
    double Arrow_BR_SIZE = 50 * _Actuator.read("BR") * 3 / SPEED_MAX;
    double Arrow_BL_SIZE = 50 * _Actuator.read("BL") * 3 / SPEED_MAX;

    cv::Point Arrow_FR(RECT_CENTER.x + RECT_SIZE * 0.5 * cos(-(spin + (M_PI / 4))), RECT_CENTER.y + RECT_SIZE * 0.5 * sin(-(spin + (M_PI / 4))));
    cv::Point Arrow_FL(RECT_CENTER.x + RECT_SIZE * 0.5 * cos(-(spin + (3 * M_PI / 4))), RECT_CENTER.y + RECT_SIZE * 0.5 * sin(-(spin + (3 * M_PI / 4))));
    cv::Point Arrow_BL(RECT_CENTER.x + RECT_SIZE * 0.5 * cos(-(spin + (5 * M_PI / 4))), RECT_CENTER.y + RECT_SIZE * 0.5 * sin(-(spin + (5 * M_PI / 4))));
    cv::Point Arrow_BR(RECT_CENTER.x + RECT_SIZE * 0.5 * cos(-(spin + (7 * M_PI / 4))), RECT_CENTER.y + RECT_SIZE * 0.5 * sin(-(spin + (7 * M_PI / 4))));

    cv::Point Arrow_FR1((Arrow_FR_SIZE)*cos(-(3 * M_PI / 4 + spin)) + Arrow_FR.x, (Arrow_FR_SIZE)*sin(-(3 * M_PI / 4 + spin)) + Arrow_FR.y);
    cv::Point Arrow_FR2((Arrow_FR_SIZE)*cos(-(7 * M_PI / 4 + spin)) + Arrow_FR.x, (Arrow_FR_SIZE)*sin(-(7 * M_PI / 4 + spin)) + Arrow_FR.y);
    cv::Point Arrow_FL1((Arrow_FL_SIZE)*cos(-(5 * M_PI / 4 + spin)) + Arrow_FL.x, (Arrow_FL_SIZE)*sin(-(5 * M_PI / 4 + spin)) + Arrow_FL.y);
    cv::Point Arrow_FL2((Arrow_FL_SIZE)*cos(-(1 * M_PI / 4 + spin)) + Arrow_FL.x, (Arrow_FL_SIZE)*sin(-(1 * M_PI / 4 + spin)) + Arrow_FL.y);
    cv::Point Arrow_BR1((Arrow_BR_SIZE)*cos(-(1 * M_PI / 4 + spin)) + Arrow_BR.x, (Arrow_BR_SIZE)*sin(-(1 * M_PI / 4 + spin)) + Arrow_BR.y);
    cv::Point Arrow_BR2((Arrow_BR_SIZE)*cos(-(5 * M_PI / 4 + spin)) + Arrow_BR.x, (Arrow_BR_SIZE)*sin(-(5 * M_PI / 4 + spin)) + Arrow_BR.y);
    cv::Point Arrow_BL1((Arrow_BL_SIZE)*cos(-(7 * M_PI / 4 + spin)) + Arrow_BL.x, (Arrow_BL_SIZE)*sin(-(7 * M_PI / 4 + spin)) + Arrow_BL.y);
    cv::Point Arrow_BL2((Arrow_BL_SIZE)*cos(-(3 * M_PI / 4 + spin)) + Arrow_BL.x, (Arrow_BL_SIZE)*sin(-(3 * M_PI / 4 + spin)) + Arrow_BL.y);

    cv::arrowedLine(img, Arrow_FR2, Arrow_FR1, C_WHITE, 1, 8, 0);
    cv::arrowedLine(img, Arrow_FL1, Arrow_FL2, C_WHITE, 1, 8, 0);
    cv::arrowedLine(img, Arrow_BR2, Arrow_BR1, C_WHITE, 1, 8, 0);
    cv::arrowedLine(img, Arrow_BL1, Arrow_BL2, C_WHITE, 1, 8, 0);
}