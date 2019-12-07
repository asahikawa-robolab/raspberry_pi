#include <stdlib.h>
#include "../../Share/inc/_StdFunc.hpp"
#include "../../Share/inc/_SerialCommunication.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_Defines.hpp"
#include "../../Share/inc/_ExternalVariable.hpp"

/*-----------------------------------------------
プロトタイプ宣言
-----------------------------------------------*/
void Angle(void);
void Servo(void);
void Chassis(void);
void Emergency(void);
void GNDEncoder(void);
void IMU(void);
void Limit(void);
void Linetracer(void);
void Motor(void);

void ProcessCom(void)
{
    /* 動作周期調整 */
    CONTROL_PERIOD ControlPeriod;

    while (_Flags.p())
    {
        Angle();
        Servo();
        Chassis();
        Emergency();
        GNDEncoder();
        IMU();
        Limit();
        Linetracer();
        Motor();
        ControlPeriod.adjust();
    }
}

/*-----------------------------------------------
 *
 * 回転角制御
 *
-----------------------------------------------*/
void Angle(void)
{
#if ENABLE_COM_ANGLE
#ifdef TEAM_A
    /*-----------------------------------------------
    A チーム
    -----------------------------------------------*/
    /* デバイスファイルを開く */
    static COM ComPowerWindow(PATH_POWER_WINDOW, 6, 6, B57600, "PowerWindow");
    static COM ComElevator(PATH_ELEVATOR, 6, 6, B57600, "Elevator");

    /* T シャツ回収 */
    ComPowerWindow.tx[0] = ADDRESS;
    ComPowerWindow.tx[1] = UP(_Actuator.read("t_shirt"));
    ComPowerWindow.tx[2] = LOW(_Actuator.read("t_shirt"));
    ComPowerWindow.tx[3] = 0;
    ComPowerWindow.tx[4] = 0;
    ComPowerWindow.tx[5] = MODE_ANGLE_POWER_WINDOW;
    ComPowerWindow.send();
    ComPowerWindow.receive();

    /* 昇降 */
    ComElevator.tx[0] = ADDRESS;
    ComElevator.tx[1] = UP(_Actuator.read("elevator_low"));
    ComElevator.tx[2] = LOW(_Actuator.read("elevator_low"));
    ComElevator.tx[3] = UP(_Actuator.read("elevator_up"));
    ComElevator.tx[4] = LOW(_Actuator.read("elevator_up"));
    ComElevator.tx[5] = MODE_ANGLE_ELEVATOR_A;
    ComElevator.send();
    ComElevator.receive();

    if (ComPowerWindow.renew & ComElevator.renew)
    {
        ComPowerWindow.renew = false;
        ComElevator.renew = false;
        _Actuator.set_converge("t_shirt", ComPowerWindow.rx[0]);
        _Actuator.set_converge("elevator_low", ComElevator.rx[0]);
        _Actuator.set_converge("elevator_up", ComElevator.rx[1]);
        _ComState.cnt[COM_STATE_ANGLE]++;
    }
#endif
#ifdef TEAM_B
    /*-----------------------------------------------
    B チーム
    -----------------------------------------------*/
    static COM ComElevator(PATH_ELEVATOR, 6, 6, B57600, "Elevator");

    /* 昇降 */
    ComElevator.tx[0] = ADDRESS;
    ComElevator.tx[1] = UP(_Actuator.read("elevator_low"));
    ComElevator.tx[2] = LOW(_Actuator.read("elevator_low"));
    ComElevator.tx[5] = MODE_ANGLE_ELEVATOR_B;
    ComElevator.send();
    ComElevator.receive();
    if (ComElevator.renew)
    {
        ComElevator.renew = false;
        _Actuator.set_converge("elevator_low", ComElevator.rx[0]);
        _ComState.cnt[COM_STATE_ANGLE]++;
    }
    //printf("t_RPM %d, RPM %d\n" ,ASBL(ComElevator.rx[2],ComElevator.rx[3]),ASBL(ComElevator.rx[4],ComElevator.rx[5]));
#endif
#endif
}

/*-----------------------------------------------
 *
 * サーボモータ
 *
-----------------------------------------------*/
void Servo(void)
{
#if ENABLE_COM_SERVO
#ifdef TEAM_A
    /*-----------------------------------------------
    A チーム
    -----------------------------------------------*/
    /* デバイスファイルを開く */
    static COM ComServo(PATH_SERVO, 5, 0, B57600, "Servo");

    ComServo.tx[0] = ADDRESS;
    ComServo.tx[1] = _Actuator.read("bath");
    ComServo.tx[2] = 0;
    ComServo.tx[3] = _Actuator.read("sheet_root");
    ComServo.tx[4] = _Actuator.read("sheet_tip");
    ComServo.send();

    _ComState.cnt[COM_STATE_SERVO]++;
#endif
#ifdef TEAM_B
    /*-----------------------------------------------
    B チーム
    -----------------------------------------------*/
    static COM ComServo_R(PATH_SERVO_R, 5, 0, B57600, "Servo_R");
    static COM ComServo_L(PATH_SERVO_L, 5, 0, B57600, "Servo_L");

    ComServo_R.tx[0] = ADDRESS;
    ComServo_R.tx[1] = _Actuator.read("L_sheet");
    ComServo_R.tx[2] = _Actuator.read("L_clip");
    ComServo_R.tx[3] = _Actuator.read("L_bath_low");
    ComServo_R.tx[4] = _Actuator.read("L_bath_up");
    ComServo_R.send();

    ComServo_L.tx[0] = ADDRESS;
    ComServo_L.tx[1] = _Actuator.read("R_sheet");
    ComServo_L.tx[2] = _Actuator.read("R_clip");
    ComServo_L.tx[3] = _Actuator.read("R_bath_low");
    ComServo_L.tx[4] = _Actuator.read("R_bath_up");
    ComServo_L.send();

    _ComState.cnt[COM_STATE_SERVO]++;
#endif
#endif
}

/*-----------------------------------------------
 *
 * 足回り
 *
-----------------------------------------------*/
void Chassis(void)
{
#if ENABLE_COM_CHASSIS
    /* デバイスファイルを開く */
    static COM ComChassis_R(PATH_CHASSIS_R, 6, 6, B57600, "Chassis_R");
    static COM ComChassis_L(PATH_CHASSIS_L, 6, 6, B57600, "Chassis_L");

    /* 極性を反転する */
    int send_rpm[4];
    send_rpm[FR] = _Actuator.read("FR");
    send_rpm[FL] = _Actuator.read("FL");
    send_rpm[BR] = _Actuator.read("BR");
    send_rpm[BL] = _Actuator.read("BL");
    if (FR_INVERSE)
        send_rpm[FR] *= -1;
    if (BR_INVERSE)
        send_rpm[BR] *= -1;
    if (FL_INVERSE)
        send_rpm[FL] *= -1;
    if (BL_INVERSE)
        send_rpm[BL] *= -1;

    /* Chassis_R */
    ComChassis_R.tx[0] = ADDRESS;
    ComChassis_R.tx[1] = UP(send_rpm[FR]);
    ComChassis_R.tx[2] = LOW(send_rpm[FR]);
    ComChassis_R.tx[3] = UP(send_rpm[BR]);
    ComChassis_R.tx[4] = LOW(send_rpm[BR]);
    ComChassis_R.tx[5] = MODE_REV_CHASSIS;
    ComChassis_R.send();
    ComChassis_R.receive();

    /* Chassis_L */
    ComChassis_L.tx[0] = ADDRESS;
    ComChassis_L.tx[1] = UP(send_rpm[FL]);
    ComChassis_L.tx[2] = LOW(send_rpm[FL]);
    ComChassis_L.tx[3] = UP(send_rpm[BL]);
    ComChassis_L.tx[4] = LOW(send_rpm[BL]);
    ComChassis_L.tx[5] = MODE_REV_CHASSIS;
    ComChassis_L.send();
    ComChassis_L.receive();

    if (ComChassis_R.renew & ComChassis_L.renew)
    {
        ComChassis_R.renew = false;
        ComChassis_L.renew = false;
        /* 表示 */
        // printf("%d, %d, %d, %d\n",
        //        ASBL(ComChassis_R.rx[2], ComChassis_R.rx[3]),
        //        ASBL(ComChassis_R.rx[4], ComChassis_R.rx[5]),
        //        ASBL(ComChassis_L.rx[2], ComChassis_L.rx[3]),
        //        ASBL(ComChassis_L.rx[4], ComChassis_L.rx[5]));

        /* ComState */
        _ComState.cnt[COM_STATE_CHASSIS]++;
    }
#endif
}

/*-----------------------------------------------
 *
 * モータスレーブ
 *
-----------------------------------------------*/
void Motor(void)
{
#if ENABLE_COM_MOTOR
#ifdef TEAM_B
    /*-----------------------------------------------
    B チーム
    -----------------------------------------------*/
    static COM ComMotor(PATH_MOTOR, 5, 0, B57600, "Motor");

    ComMotor.tx[0] = ADDRESS;
    ComMotor.tx[1] = _Actuator.read("motor0");
    ComMotor.tx[2] = _Actuator.read("motor1");
    ComMotor.tx[3] = 0;
    ComMotor.tx[4] = 0;
    ComMotor.send();

    _ComState.cnt[COM_STATE_MOTOR]++;

    //printf("motor0 %d, motor1 %d, ",ComMotor.tx[1],ComMotor.tx[2]);
#endif
#endif
}
/*-----------------------------------------------
 *
 * 非常停止
 *
-----------------------------------------------*/
void Emergency(void)
{
#if ENABLE_COM_EMERGENCY
    /* デバイスファイルを開く */
    static COM ComEmergency(PATH_EMERGENCY, 1, 0, B57600, "Emergency");

    ComEmergency.tx[0] = _Flags.f[FLAG_EMERGENCY];
    ComEmergency.send();
    _ComState.cnt[COM_STATE_EMERGENCY]++;
#endif
}

/*-----------------------------------------------
 *
 * 接地エンコーダ
 *
-----------------------------------------------*/
void GNDEncoder(void)
{
#if ENABLE_COM_GNDENCODER
    /* デバイスファイルを開く */
    static COM ComGNDEncoder(PATH_ENCODER, 4, 8, B57600, "GNDEncoder");

    ComGNDEncoder.tx[0] = ADDRESS;
    ComGNDEncoder.tx[1] = _Flags.f[FLAG_GND_ENCODER_RESET];
    ComGNDEncoder.tx[2] = _Flags.f[FLAG_GND_ENCODER_RESET];
    ComGNDEncoder.tx[3] = MODE_GNDENCODER;
    ComGNDEncoder.send();
    ComGNDEncoder.receive();

    /* オドメトリを計算 */
    static cv::Point2f pre_S_GND;
    static cv::Point2f S_GND;  /* x, y */
    static cv::Point2f dS_GND; /* Δx, Δy */
    static cv::Point2f dL_GND; /* ΔX, ΔY */
    if (ComGNDEncoder.renew == true)
    {
        ComGNDEncoder.renew = false;

        /* 回転回数を距離に変換 */
        const double d = 102.0; /* タイヤ直径 [mm] */

        double Spin = _Chassis.read_spin();

        S_GND.x = M_PI * d * (ASBL(ComGNDEncoder.rx[0], ComGNDEncoder.rx[1]) + (ASBL(ComGNDEncoder.rx[2], ComGNDEncoder.rx[3]) / 360.0));
        S_GND.y = M_PI * d * (ASBL(ComGNDEncoder.rx[4], ComGNDEncoder.rx[5]) + (ASBL(ComGNDEncoder.rx[6], ComGNDEncoder.rx[7]) / 360.0));

        /* 回転を考慮したオドメトリを算出 */
        dS_GND.x = S_GND.x - pre_S_GND.x;
        dS_GND.y = S_GND.y - pre_S_GND.y;
        dL_GND.x = dS_GND.x * cos(Spin) - (dS_GND.y * sin(Spin));
        dL_GND.y = dS_GND.x * sin(Spin) + (dS_GND.y * cos(Spin));
        _Odometry.v->x += dL_GND.x;
        _Odometry.v->y += dL_GND.y;
        pre_S_GND.x = S_GND.x;
        pre_S_GND.y = S_GND.y;

        /* ComState */
        _ComState.cnt[COM_STATE_GND_ENCODER]++;
    }

    /* リセット */
    if (_Flags.f[FLAG_GND_ENCODER_RESET] == true)
    {
        *_Odometry.v = cv::Point2f(0, 0);
        pre_S_GND = cv::Point2f(0, 0);
    }
#endif
}

/*-----------------------------------------------
 *
 * IMU
 *
-----------------------------------------------*/
void IMU(void)
{
#if ENABLE_COM_IMU
    /* デバイスファイルを開く */
    static COM ComIMU(PATH_IMU, 1, 4, B57600, "IMU");

    ComIMU.tx[0] = _Flags.f[FLAG_IMU_RESET];
    ComIMU.send();
    ComIMU.receive();

    if (ComIMU.renew == true)
    {
        ComIMU.renew = false;

        /* spin */
        double Spin = DEG_RAD(ASBL(ComIMU.rx[0], ComIMU.rx[1]) / 10.0);
        _Chassis.write_spin(Spin);

        /* ComState */
        _ComState.cnt[COM_STATE_IMU]++;
    }
#endif
}

/*-----------------------------------------------
 *
 * リミットスイッチ
 *
-----------------------------------------------*/
void Limit(void)
{
#if ENABLE_COM_LIMIT
#ifdef TEAM_A
    /* デバイスファイルを開く */
    static COM ComLimit(PATH_LIMIT, 0, 1, B57600, "Limit");
    ComLimit.receive();
    if (ComLimit.renew == true)
    {
        ComLimit.renew = false;

        _Limit.write("front", ComLimit.rx[0], 0);
        _Limit.write("right", ComLimit.rx[0], 1);
        _Limit.write("bath2", ComLimit.rx[0], 5);
        _Limit.write("bath1", ComLimit.rx[0], 6);
        _Limit.write("left", ComLimit.rx[0], 7);

        /* ComState */
        _ComState.cnt[COM_STATE_LIMIT]++;
    }
#endif
#ifdef TEAM_B
    static COM ComLimit(PATH_LIMIT, 0, 1, B57600, "Limit");
    ComLimit.receive();
    if (ComLimit.renew == true)
    {
        ComLimit.renew = false;

        _Limit.write("lim_front", ComLimit.rx[0], 1);
        _Limit.write("lim_L_S", ComLimit.rx[0], 2);
        _Limit.write("lim_right", ComLimit.rx[0], 3);
        _Limit.write("lim_left", ComLimit.rx[0], 4);
        _Limit.write("lim_R_S", ComLimit.rx[0], 5);
        _Limit.write("lim_R_B_low", ComLimit.rx[0], 6);
        _Limit.write("lim_R_B_up", ComLimit.rx[0], 7);

        /* ComState */
        _ComState.cnt[COM_STATE_LIMIT]++;
    }
#endif
#endif
}

/*-----------------------------------------------
 *
 * ライントレーサ
 *
-----------------------------------------------*/
void Linetracer(void)
{
#if ENABLE_COM_LINETRACER
    /* デバイスファイルを開く */
    static COM ComLineFront(PATH_LINE_FRONT, 0, 2, B57600, "LineFront");
    //static COM ComLineRight(PATH_LINE_RIGHT, 0, 2, B57600, "LineRight");
    //static COM ComLineLeft(PATH_LINE_LEFT, 0, 2, B57600, "LineLeft");

    ComLineFront.receive();
    //ComLineRight.receive();
    //ComLineLeft.receive();

    //if (ComLineFront.renew & ComLineRight.renew & ComLineLeft.renew)
    if (ComLineFront.renew)
    {
        ComLineFront.renew = false;
        //ComLineRight.renew = false;
        //ComLineLeft.renew = false;

        _Line.write("front", ComLineFront.rx[0]);
        //_Line.write("right", ComLineRight.rx[0]);
        //_Line.write("left", ComLineLeft.rx[0]);

        _ComState.cnt[COM_STATE_LINE_TRACER]++;
    }
#endif
}