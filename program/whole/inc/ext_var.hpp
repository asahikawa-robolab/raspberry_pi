#ifndef EXT_VAR_HPP
#define EXT_VAR_HPP
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

extern jibiki::ShareVar<short> g_pwm[4]; /* pwm目標値 */
extern jibiki::ShareVar<short> g_angle;  /* 回転角制御目標値 */
extern SwitchData g_switch_data;         /* スイッチスレーブのデータ */
extern Imu g_imu;                        /* IMU のデータ */
extern Controller g_controller;          /* コントローラのデータ */
extern Chassis g_chassis;                /* 足回り */
extern jibiki::ShareVar<short> g_rev_tgt[4];/*回転数目標値*/
extern short g_pitting_rev;              /*ピッチング回転数*/
extern jibiki::ShareVar<bool> emergency_sw;/* 非常停止swの状態 0:off 1:on */

#endif