#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

<<<<<<< HEAD
jibiki::ShareVar<short> g_pwm[4];           /* pwm目標値 */
jibiki::ShareVar<short> g_rev[4];           /* 回転数目標値 */
jibiki::ShareVar<short> g_rev_diff[4];      /* 回転数偏差 */
jibiki::ShareVar<short> g_rot[4];           /* 回転角目標値 */
jibiki::ShareVar<short> g_rot_diff[4];      /* 回転角偏差 */
jibiki::ShareVar<int32_t>g_dist[2];         /* 移動距離 */
=======
jibiki::ShareVar<short> g_pwm_tgt[4];       /* pwm目標値 */
jibiki::ShareVar<short> g_rev_tgt[4];       /* 回転数目標値 */
jibiki::ShareVar<short> g_rev_curr[4];      /* 回転数現在地 */
jibiki::ShareVar<short> g_rot_tgt[4];       /* 回転角目標値 */
jibiki::ShareVar<short> g_rot_curr[4];      /* 回転角現在地 */
jibiki::ShareVar<int64_t>g_dist_tgt[2];     /* 移動距離目標値 */
jibiki::ShareVar<int64_t>g_dist_curr[2];    /* 移動距離 */
>>>>>>> 6aad66e... [大平]変数の調整
jibiki::ShareVar<bool>  g_odometry_flag[2]; /* odometry_リセットフラグ */
SwitchData g_switch_data;                   /* スイッチスレーブのデータ */
Imu g_imu;                                  /* IMU のデータ */
Controller g_controller;                    /* コントローラのデータ */
Chassis g_chassis(g_imu);                   /* 足回り */