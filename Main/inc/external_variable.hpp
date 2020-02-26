#ifndef _EXTERNAL_VARIABLE_HPP
#define _EXTERNAL_VARIABLE_HPP

#include <vector>
#include <string>
#include "../inc/_process_operate_auto.hpp"
#include "../inc/_process_com.hpp"
#include "../inc/module.hpp"
#include "../inc/config.hpp"

/*-----------------------------------------------
 *
 * モジュール
 *
-----------------------------------------------*/
extern SwitchData g_switch_data;
extern uint8_t g_flags[];
extern IMU g_IMU;
extern Chassis g_chassis;
extern cv::Point2f g_odometry;
extern Controller g_controller;
extern jibiki::AssocArray<double> g_com_data;

/*-----------------------------------------------
 *
 * operate_keyboard
 *
-----------------------------------------------*/
extern const std::vector<std::string> g_keyboard_mode;
extern size_t g_keyboard_mode_cnt;

/*-----------------------------------------------
 *
 * 通信
 *
-----------------------------------------------*/
extern const std::vector<FuncCom> g_func_com;

/*-----------------------------------------------
 *
 * order
 *
-----------------------------------------------*/
extern LoadOrder g_load_order;
extern std::vector<std::string> g_executing_order;

/*-----------------------------------------------
 *
 * 足回り
 *
-----------------------------------------------*/
/* モータ入れ替え */
extern size_t g_motor_FR;
extern size_t g_motor_FL;
extern size_t g_motor_BR;
extern size_t g_motor_BL;
/* モータ極性 */
extern bool g_inverse_FR;
extern bool g_inverse_FL;
extern bool g_inverse_BR;
extern bool g_inverse_BL;
/* 最高移動速度 */
extern double g_max_speed;

#endif