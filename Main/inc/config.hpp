#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <opencv2/opencv.hpp>
#include "../inc/_process_com.hpp"

/*-----------------------------------------------
 *
 * ウィンドウ
 *
-----------------------------------------------*/
const cv::Size WINDOW_SIZE = cv::Size(450, 420);
const cv::Point WINDOW_POS = cv::Point(340, 35);

/*-----------------------------------------------
 *
 * モード
 *
-----------------------------------------------*/
extern void mode_test(std::vector<std::string> param, size_t branch_num);
extern void mode_wait(std::vector<std::string> param, size_t branch_num);
extern void mode_encoder(std::vector<std::string> param, size_t branch_num);
const std::vector<Mode> g_mode_dict{
    Mode(mode_wait, "wait", 0),
    Mode(mode_test, "test", 2),
    Mode(mode_encoder, "encoder", 7),
};

/*-----------------------------------------------
 *
 * キー操作で変更する値
 *
-----------------------------------------------*/
const std::vector<std::string> g_keyboard_mode{
    "chassis_speed",
    "chassis_theta",
    "chassis_spin",
    "servo1",
    "servo2",
    "servo3",
    "servo4",
    "servo5",
    "servo6",
    "servo7",
    "servo8",
};

/*-----------------------------------------------
 *
 * 通信処理
 *
-----------------------------------------------*/
using std::string;
using std::vector;
extern void com_switch_func(vector<string> paths);
extern void com_IMU_func(vector<string> paths);
extern void com_chassis_func(vector<string> paths);
extern void com_servo_func(vector<string> paths);
extern void com_odometry_func(vector<string> paths);
extern void com_limit_switch_func(vector<string> paths);
extern void com_controller_func(vector<string> paths);
extern void com_test(vector<string> paths);
const vector<FuncCom> g_func_com{
    FuncCom(com_switch_func,
             "com_switch",
             vector<string>{"switch"}),
    FuncCom(com_IMU_func,
             "com_IMU",
             vector<string>{"IMU"}),
    FuncCom(com_chassis_func,
             "com_chassis",
             vector<string>{"chassis_R", "chassis_L"}),
    FuncCom(com_servo_func,
             "com_servo",
             vector<string>{"servo_R", "servo_L"}),
    FuncCom(com_odometry_func,
             "com_odometry",
             vector<string>{"odometry"}),
    FuncCom(com_limit_switch_func,
             "com_limit_switch",
             vector<string>{"limit_switch"}),
    FuncCom(com_controller_func,
             "com_controller",
             vector<string>{"controller"}),
    FuncCom(com_test,
             "com_test",
             vector<string>{"test"}),
};

/*-----------------------------------------------
 *
 * キー
 *
-----------------------------------------------*/
#define KEY_LEFT 81
#define KEY_LEFT_KBHIT 68
#define KEY_UP 82
#define KEY_UP_KBHIT 65
#define KEY_RIGHT 83
#define KEY_RIGHT_KBHIT 67
#define KEY_DOWN 84
#define KEY_DOWN_KBHIT 66
#define KEY_PLUS 59
#define KEY_MINUS 45

#endif