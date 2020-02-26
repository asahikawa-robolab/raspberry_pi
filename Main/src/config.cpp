#include <vector>
#include <string>
#include "../../Share/inc/_std_func.hpp"
#include "../inc/_process_com.hpp"

/*-----------------------------------------------
 *
 * 通信データ
 *
-----------------------------------------------*/
jibiki::AssocArray<double> g_com_data({
    "servo1",
    "servo2",
    "servo3",
    "servo4",
    "servo5",
    "servo6",
    "servo7",
    "servo8",
    "limit_switch1",
    "limit_switch2",
    "limit_switch3",
    "limit_switch4",
    "limit_switch5",
    "limit_switch6",
    "limit_switch7",
    "limit_switch8",
    "com_state_switch",
    "com_state_IMU",
    "com_state_chassis_R",
    "com_state_chassis_L",
    "com_state_servo_R",
    "com_state_servo_L",
    "com_state_odometry",
    "com_state_limit_switch",
    "com_state_controller",
});