#include <vector>
#include <string>
#include "../inc/_process_operate_auto.hpp"
#include "../inc/module.hpp"
#include "../inc/config.hpp"

/*-----------------------------------------------
 *
 * モジュール
 *
-----------------------------------------------*/
SwitchData g_switch_data;
uint8_t g_flags[NUM_FLAGS];
IMU g_IMU;
Chassis g_chassis;
cv::Point2f g_odometry;
Controller g_controller;

/*-----------------------------------------------
 *
 * operate_keyboard
 *
-----------------------------------------------*/
size_t g_keyboard_mode_cnt;

/*-----------------------------------------------
 *
 * order
 *
-----------------------------------------------*/
LoadOrder g_load_order;
std::vector<std::string> g_executing_order;

/*-----------------------------------------------
 *
 * 足回り
 *
-----------------------------------------------*/
/* モータ入れ替え */
size_t g_motor_FR;
size_t g_motor_FL;
size_t g_motor_BR;
size_t g_motor_BL;
/* モータ極性 */
bool g_inverse_FR;
bool g_inverse_FL;
bool g_inverse_BR;
bool g_inverse_BL;
/* 最高移動速度 */
double g_max_speed;