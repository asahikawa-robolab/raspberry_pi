#ifndef EXT_VAR_HPP
#define EXT_VAR_HPP
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

extern jibiki::ShareVar<short> g_angle;  /* 回転角制御目標値 */
extern SwitchData g_switch_data;         /* スイッチスレーブのデータ */
extern Imu g_imu;                        /* IMU のデータ */
extern Controller g_controller;          /* コントローラのデータ */
extern Chassis g_chassis;                /* 足回り */

#endif