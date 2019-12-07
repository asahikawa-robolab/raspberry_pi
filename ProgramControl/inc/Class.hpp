#ifndef CLASS_HPP
#define CLASS_HPP

#include "../../Share/inc/_Module.hpp"

class MANUAL
{
public:
    MOTION motion;
    int motor[2], angle[3], servo[8];

    MANUAL(void);
    void read(void);
    void write(void);
    void print(void);
};

/*-----------------------------------------------
 *
 * コンストラクタ
 *
-----------------------------------------------*/
inline MANUAL::MANUAL(void)
{
    read();
    motion = MOTION(0, 0);
}

/*-----------------------------------------------
 *
 * アクチュエータ指令値読み込み
 *
-----------------------------------------------*/
inline void MANUAL::read(void)
{
#ifdef TEAM_A
    /*-----------------------------------------------
    A チーム
    -----------------------------------------------*/
    servo[0] = _Actuator.read("bath");
    servo[1] = 0;
    servo[2] = _Actuator.read("sheet_root");
    servo[3] = _Actuator.read("sheet_tip");
    angle[0] = _Actuator.read("elevator_low");
    angle[1] = _Actuator.read("elevator_up");
    angle[2] = _Actuator.read("t_shirt");
#endif
#ifdef TEAM_B
    /*-----------------------------------------------
    B チーム
    -----------------------------------------------*/
    servo[0] = _Actuator.read("R_bath_up");
    servo[1] = _Actuator.read("R_bath_low");
    servo[2] = _Actuator.read("R_sheet");
    servo[3] = _Actuator.read("R_clip");
    servo[4] = _Actuator.read("L_bath_up");
    servo[5] = _Actuator.read("L_bath_low");
    servo[6] = _Actuator.read("L_sheet");
    servo[7] = _Actuator.read("L_clip");
    angle[0] = _Actuator.read("elevator_low");

    motor[0] = _Actuator.read("motor0");
    motor[1] = _Actuator.read("motor1");

#endif
}

/*-----------------------------------------------
 *
 * アクチュエータ指令値書き込み
 *
-----------------------------------------------*/
inline void MANUAL::write(void)
{
#ifdef TEAM_A
    /*-----------------------------------------------
    A チーム
    -----------------------------------------------*/
    _Chassis.set(motion);
    _Actuator.set("bath", servo[0]);
    _Actuator.set("sheet_root", servo[2]);
    _Actuator.set("sheet_tip", servo[3]);
    _Actuator.set("elevator_low", angle[0]);
    _Actuator.set("elevator_up", angle[1]);
    _Actuator.set("t_shirt", angle[2]);
#endif
#ifdef TEAM_B
    /*-----------------------------------------------
    B チーム
    -----------------------------------------------*/
    _Chassis.set(motion);
    _Actuator.set("R_bath_up", servo[0]);
    _Actuator.set("R_bath_low", servo[1]);
    _Actuator.set("R_sheet", servo[2]);
    _Actuator.set("R_clip", servo[3]);
    _Actuator.set("L_bath_up", servo[4]);
    _Actuator.set("L_bath_low", servo[5]);
    _Actuator.set("L_sheet", servo[6]);
    _Actuator.set("L_clip", servo[7]);
    _Actuator.set("elevator_low", angle[0]);
    _Actuator.set("motor0", motor[0]);
    _Actuator.set("motor1", motor[1]);
#endif
}

/*-----------------------------------------------
 *
 * アクチュエータ指令値表示
 *
-----------------------------------------------*/
inline void MANUAL::print(void)
{
#ifdef TEAM_A
    /*-----------------------------------------------
    A チーム
    -----------------------------------------------*/
    printf("up : %d, low : %d, tip : %d, root : %d\n",
           _Actuator.read("elevator_up"),
           _Actuator.read("elevator_low"),
           _Actuator.read("sheet_root"),
           _Actuator.read("sheet_tip"));
    // printf("%d\n", _Limit.read("bath"));
    // _Limit.print();
    // printf("front %d, right %d, left %d, bath %d\n",
    //        _Limit.read("front"),
    //        _Limit.read("right"),
    //        _Limit.read("left"),
    //        _Limit.read("bath"));
#endif
#ifdef TEAM_B
    /*-----------------------------------------------
    B チーム
    -----------------------------------------------*/
    // printf("R [BU %d, BL %d, S %d, C %d], L [BU %d, BL %d, S %d, C %d]\n",
    //        _Actuator.read("R_bath_up"),
    //        _Actuator.read("R_bath_low"),
    //        _Actuator.read("R_sheet"),
    //        _Actuator.read("R_clip"),
    //        _Actuator.read("L_bath_up"),
    //        _Actuator.read("L_bath_low"),
    //        _Actuator.read("L_sheet"),
    //        _Actuator.read("L_clip"));
    // printf("low %d\n",
    //        _Actuator.read("elevator_low"));
    // printf("motor0 %d, motor1 %d\n",
    //        _Actuator.read("motor0"),
    //        _Actuator.read("motor1"));
#endif
}

#endif