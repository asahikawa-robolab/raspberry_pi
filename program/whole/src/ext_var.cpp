#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

jibiki::ShareVar<short> g_pwm[4];   /* pwm目標値 */
jibiki::ShareVar<short> g_angle(0); /* 回転角制御目標値 */
SwitchData g_switch_data;           /* スイッチスレーブのデータ */
Imu g_imu;                          /* IMU のデータ */
Controller g_controller;            /* コントローラのデータ */
Chassis g_chassis(g_imu);           /* 足回り */
jibiki::ShareVar<short> g_rev_tgt[4];/*回転数目標値*/
short g_pitting_rev;              /*ピッチング回転数*/
jibiki::ShareVar<bool> emergency_sw(0);/* 非常停止swの状態 0:off 1:on */