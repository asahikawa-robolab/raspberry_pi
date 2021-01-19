#ifndef EXT_VAR_HPP
#define EXT_VAR_HPP
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

extern jibiki::ShareVar<short> g_pwm_tgt[4];       /* pwm目標値 */
extern jibiki::ShareVar<short> g_rev_tgt[4];       /* 回転数目標値 */
extern jibiki::ShareVar<short> g_rev_curr[4];      /* 回転数現在地 */
extern jibiki::ShareVar<short> g_rot_tgt[4];       /* 回転角目標値 */
extern jibiki::ShareVar<short> g_rot_curr[4];      /* 回転角現在地 */
extern jibiki::ShareVar<int64_t>g_dist_tgt[2];     /* 移動距離目標値 */
extern jibiki::ShareVar<int64_t>g_dist_curr[2];    /* 移動距離 */
extern jibiki::ShareVar<bool>  g_odometry_flag[2]; /* odometry_リセットフラグ */
extern SwitchData g_switch_data;                   /* スイッチスレーブのデータ */
extern Imu g_imu;                                  /* IMU のデータ */
extern Controller g_controller;                    /* コントローラのデータ */
extern Chassis g_chassis;                          /* 足回り */

#endif