#ifndef EXT_VAR_HPP
#define EXT_VAR_HPP
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

extern jibiki::ShareVar<short> g_pwm[4];           /* pwm目標値 */
extern jibiki::ShareVar<short> g_rev[4];           /* rev目標値 */
extern jibiki::ShareVar<short> g_rev_diff[4];      /* rev偏差 */
extern jibiki::ShareVar<short> g_rot[4];           /* 回転角目標値 */
extern jibiki::ShareVar<short> g_rot_diff[4];      /* 回転角偏差 */
extern jibiki::ShareVar<int32_t>g_dist[2];         /* 移動距離 */
extern jibiki::ShareVar<bool>  g_odometry_flag[2]; /* odometry_リセットフラグ */
extern SwitchData g_switch_data;                   /* スイッチスレーブのデータ */
extern Imu g_imu;                                  /* IMU のデータ */
extern Controller g_controller;                    /* コントローラのデータ */
extern Chassis g_chassis;                          /* 足回り */

#endif