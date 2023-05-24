#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

jibiki::ShareVar<short> g_pwm_tgt[4];       /* pwm目標値 */
jibiki::ShareVar<short> g_rev_tgt[4];       /* 回転数目標値 */
jibiki::ShareVar<short> g_rev_curr[4];      /* 回転数現在地 */
jibiki::ShareVar<int16_t> g_rot_tgt[4];     /* 回転角目標値 */
jibiki::ShareVar<int16_t> g_rot_curr[4];    /* 回転角差 */
jibiki::ShareVar<int16_t> g_rot_steer_curr[4];    /* 独ステ回転角現在地 */
jibiki::ShareVar<int32_t>g_dist_tgt[2];     /* 移動距離目標値 */
jibiki::ShareVar<int32_t>g_dist_curr[2];    /* 移動距離 */
jibiki::ShareVar<bool>  g_odometry_flag[2]; /* odometry_リセットフラグ */
jibiki::ShareVar<bool>g_limit[8];           /* limitスイッチのon/off */
jibiki::ShareVar<bool> emergency_sw(0);     /* 非常停止swの状態 0:off 1:on */
jibiki::ShareVar<int8_t>g_chassis_rev[4];   /* 現在の回転数 ＊jerkで使用 0:fr,1:fl,2:br,3:bl*/
SwitchData g_switch_data;                   /* スイッチスレーブのデータ */
Imu g_imu;                                  /* IMU のデータ */
Controller g_controller;                    /* コントローラのデータ */
Chassis g_chassis(g_imu);                   /* 足回り */
SteerChassis g_steer(g_imu);                   /* 足回り */

