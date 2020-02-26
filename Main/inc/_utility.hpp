#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include "../../Share/inc/_picojson.hpp"

typedef enum
{
    OPERATE_NONE,     /* 操作とは無関係 */
    OPERATE_AUTO,     /* 自動 */
    OPERATE_MANUAL,   /* 手動 */
    OPERATE_KEYBOARD, /* キーボード */
} OperateMethod;

typedef enum
{
    TURN_CW,       /* 時計回り */
    TURN_CCW,      /* 反時計回り */
    TURN_SHORTEST, /* 最短方向 */
} TurnMode;

bool manage_process(OperateMethod operate_method);
double calc_angle_diff(double subed, double sub, TurnMode turn_mode = TURN_SHORTEST);

#endif