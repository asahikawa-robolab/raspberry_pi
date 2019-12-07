#include <unistd.h>
#include <signal.h>
#include "../../Share/inc/_SharedMemory.hpp"
#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_Config.hpp"
#include "../../Share/inc/_SerialCommunication.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../../Share/inc/_ExternalVariable.hpp"
#include "../../Share/inc/_Class.hpp"
#include "../inc/Class.hpp"

void shmInit(void);
void ProcessComSwitch(void);
void ControlKey(pid_t pid, MANUAL &Manual);
void ControlKey_Common(pid_t pid, MANUAL &Manual);
void ControlKey_A(MANUAL &Manual);
void ControlKey_B(MANUAL &Manual);
void ResetProcess(void);
void Switch(void);
inline bool read_bit(unsigned char data, int num)
{
    return (data & (int)pow(2, num)) / (int)pow(2, num);
}

int main(void)
{
    /* 共有メモリ初期化 */
    shmInit();

    /* プロセスを作成 */
    pid_t pid = fork();

    /* エラーチェック */
    if (pid == -1)
    {
        PrintError("ProgramControl/main()", "fork failed.");
        return 1;
    }
    /*-----------------------------------------------
    子プロセス
    -----------------------------------------------*/
    else if (pid == 0)
    {
        ProcessComSwitch();
        return 0;
    }
    /*-----------------------------------------------
    親プロセス
    -----------------------------------------------*/
    else
    {
        /* 動作周期調整 */
        CONTROL_PERIOD ControlPeriod;

        MANUAL Manual;
        while (1)
        {
            usleep(10);              /* CPU の使用率を抑えるために実行周期を下げる */
            Manual.read();           /* アクチュエータ指令値を読み込む */
            ControlKey(pid, Manual); /* キー制御 */
            if (_Flags.f[FLAG_OPERATE] == OPERATE_MANUAL)
            {
                Manual.write(); /* アクチュエータ指令値を書き込む */
            }
            // Manual.print(); /* アクチュエータ指令値を表示 */

            ResetProcess(); /* リセット */
            Switch();       /* Switch のデータに応じた処理 */
            // _Switch.print();  /* 受信データを表示 */

            /* 動作周期調整 */
            ControlPeriod.adjust();
        }
    }
    return 0;
}

/*-----------------------------------------------
 *
 * キー制御
 *
-----------------------------------------------*/
void ControlKey(pid_t pid, MANUAL &Manual)
{
    /* ProgramControl からのキー入力も反映させる */
    if (kbhit())
    {
        _Flags.f[FLAG_KEY] = getchar();
    }

    /* 入力されたキーに応じた処理 */
    ControlKey_Common(pid, Manual);
    ControlKey_A(Manual);
    ControlKey_B(Manual);

    /* 入力されたキー情報をクリア */
    _Flags.f[FLAG_KEY] = 0;
}

/*-----------------------------------------------
 *
 * キー制御（共通）
 *
-----------------------------------------------*/
void ControlKey_Common(pid_t pid, MANUAL &Manual)
{
    static double target_spin = _Chassis.get_target_spin();

    switch (_Flags.f[FLAG_KEY])
    {
    /* オドメトリ */
    case 'd':
        _Odometry.v->x += 10;
        break;
    case 'a':
        _Odometry.v->x -= 10;
        break;
    case 'w':
        _Odometry.v->y += 10;
        break;
    case 's':
        _Odometry.v->y -= 10;
        break;
    /* 非常停止 */
    case 'e':
        _Flags.f[FLAG_EMERGENCY] ^= 1;
        break;
    /* ProgramControl の終了 */
    case 'q':
        if (_Flags.f[FLAG_PROGRAM] == false)
        {
            kill(pid, SIGTERM); /* 子プロセスを終了 */
            printf("*** ProgramControl end. ***\n");
            exit(EXIT_SUCCESS);
        }
        break;
    /* マニュアル操作 */
    case 'c':
        _Flags.f[FLAG_OPERATE] ^= 1;
        break;
    /* 足回り */
    case ';':
        if (Manual.motion.speed <= SPEED_MAX - 10)
            Manual.motion.speed += 10;
        break;
    case '.':
        if (Manual.motion.speed >= 10)
            Manual.motion.speed -= 10;
        break;
    case ':':
        Manual.motion.theta += DEG_RAD(5);
        break;
    case '/':
        Manual.motion.theta -= DEG_RAD(5);
        break;
    case ']':
        target_spin += DEG_RAD(5);
        _Chassis.set_target_spin(target_spin);
        break;
    case '\\':
        target_spin -= DEG_RAD(5);
        _Chassis.set_target_spin(target_spin);
        break;
    case KEY_UP:
    case KEY_UP_KBHIT:
        Manual.motion.theta = DEG_RAD(90);
        break;
    case KEY_DOWN:
    case KEY_DOWN_KBHIT:
        Manual.motion.theta = DEG_RAD(-90);
        break;
    case KEY_RIGHT:
    case KEY_RIGHT_KBHIT:
        Manual.motion.theta = DEG_RAD(0);
        break;
    case KEY_LEFT:
    case KEY_LEFT_KBHIT:
        Manual.motion.theta = DEG_RAD(180);
        break;
    }
}

/*-----------------------------------------------
 *
 * キー制御（A チーム）
 *
-----------------------------------------------*/
void ControlKey_A(MANUAL &Manual)
{
#ifdef TEAM_A
    const int base_servo = 5, base_elevator = 180, base_t_shirt = 10;

    switch (_Flags.f[FLAG_KEY])
    {
    /* 昇降 */
    case '5':
        if (Manual.angle[0] >= -5000 + base_elevator)
            Manual.angle[0] -= base_elevator;
        break;
    case 't':
        if (Manual.angle[0] <= -base_elevator)
            Manual.angle[0] += base_elevator;
        break;
    case 'g':
        Manual.angle[0] = -5000;
        break;
    case 'b':
        Manual.angle[0] = 0;
        break;
    case '6':
        if (Manual.angle[1] >= -4250 + base_elevator)
            Manual.angle[1] -= base_elevator;
        break;
    case 'y':
        if (Manual.angle[1] <= -base_elevator)
            Manual.angle[1] += base_elevator;
        break;
    case 'h':
        Manual.angle[1] = -4250;
        break;
    case 'n':
        Manual.angle[1] = 0;
        break;
    /* t シャツ */
    case '7':
        if (Manual.angle[2] <= 130 - base_t_shirt)
            Manual.angle[2] += base_t_shirt;
        break;
    case 'u':
        if (Manual.angle[2] >= base_t_shirt)
            Manual.angle[2] -= base_t_shirt;
        break;
    /* サーボモータ */
    case '8':
        if (Manual.servo[0] <= 180 - base_servo)
            Manual.servo[0] += base_servo;
        break;
    case 'i':
        if (Manual.servo[0] >= base_servo)
            Manual.servo[0] -= base_servo;
        break;
    case '9':
        if (Manual.servo[1] <= 180 - base_servo)
            Manual.servo[1] += base_servo;
        break;
    case 'o':
        if (Manual.servo[1] >= base_servo)
            Manual.servo[1] -= base_servo;
        break;
    case '0':
        if (Manual.servo[2] <= 180 - base_servo)
            Manual.servo[2] += base_servo;
        break;
    case 'p':
        if (Manual.servo[2] >= base_servo)
            Manual.servo[2] -= base_servo;
        break;
    case '-':
        if (Manual.servo[3] <= 180 - base_servo)
            Manual.servo[3] += base_servo;
        break;
    case '@':
        if (Manual.servo[3] >= base_servo)
            Manual.servo[3] -= base_servo;
        break;
    }
#endif
}

/*-----------------------------------------------
 *
 * キー制御（B チーム）
 *
-----------------------------------------------*/
void ControlKey_B(MANUAL &Manual)
{
#ifdef TEAM_B
    const int base_elevator = 90, base_motor = 1;
    switch (_Flags.f[FLAG_KEY])
    {
        /* servo */
    case '5':
        Manual.servo[0] = 0;
        Manual.servo[4] = 180;
        break;
    case 't':
        Manual.servo[0] = 180;
        Manual.servo[4] = 0;
        break;
    case '6':
        Manual.servo[1] = 0;
        Manual.servo[5] = 180;
        break;
    case 'y':
        Manual.servo[1] = 180;
        Manual.servo[5] = 0;
        break;
    case '7':
        Manual.servo[2] = 150;
        break;
    case 'u':
        Manual.servo[2] = 0;
        break;
    case '8':
        Manual.servo[3] = 80;
        break;
    case 'i':
        Manual.servo[3] = 180;
        break;
    case '9':
        Manual.servo[4] = 100;
        break;
    case 'o':
        Manual.servo[4] = 0;
        break;
    case '-':
        Manual.servo[6] = 140;
        break;
    case '@':
        Manual.servo[6] = 0;
        break;
    case '^':
        Manual.servo[7] = 80;
        break;
    case '[':
        Manual.servo[7] = 0;
        break;
    /* elevetor */
    case '1':
        Manual.angle[0] -= base_elevator;
        break;
    case '2':
        Manual.angle[0] += base_elevator;
        break;
    case '3':
        Manual.angle[0] = -360;
        break;
    case '4':
        Manual.angle[0] = 0;
        break;
    /* motor */
    case 'Q':
        Manual.motor[0] += base_motor;
        break;
    case 'A':
        Manual.motor[0] -= base_motor;
        break;
    case 'Z':
        Manual.motor[0] = 0;
        break;
    case 'W':
        Manual.motor[1] += base_motor;
        break;
    case 'S':
        Manual.motor[1] -= base_motor;
        break;
    case 'X':
        Manual.motor[1] = 0;
        break;
    }
#endif
}

void ResetProcess(void)
{
    static bool isFirst = true;
    static struct timespec time_s;

    if (_Flags.f[FLAG_GND_ENCODER_RESET] | _Flags.f[FLAG_IMU_RESET])
    {
        if (isFirst == true)
        {
            isFirst = false;
            time_s = GetTime();
        }

        if (CALC_SEC(time_s, GetTime()) > 0.1)
        {
            _Flags.f[FLAG_GND_ENCODER_RESET] = false;
            _Flags.f[FLAG_IMU_RESET] = false;
            isFirst = true;
        }
    }
}

/*-----------------------------------------------
 *
 * スイッチの入力に応じた処理
 *
-----------------------------------------------*/
void Switch(void)
{
    /*-----------------------------------------------
    start
    -----------------------------------------------*/
    if (_Switch.once(ONCE_START, _Switch.d->start) == true)
    {
        _Flags.f[FLAG_START] = true;
        _Flags.f[FLAG_EMERGENCY] = START;
    }
    /*-----------------------------------------------
    reset
    -----------------------------------------------*/
    if (_Switch.once(ONCE_RESET, _Switch.d->reset) == true)
    {
        if (_Switch.d->program == false)
        {
            _Flags.f[FLAG_START] = false;
            _Flags.f[FLAG_MAIN_END] = true;
        }
        /* ロボット非動作中 */
        else
        {
            _Flags.f[FLAG_GND_ENCODER_RESET] = true;
            _Flags.f[FLAG_IMU_RESET] = true;
        }
    }

    /*-----------------------------------------------
    program
    -----------------------------------------------*/
    static bool pre_program = false;
    if ((pre_program ^ _Switch.d->program) == true)
    {
        if (_Switch.d->program == true)
        {
            printf("開始\n");
            system("sh ../Script/autostart.sh");
        }
        else
        {
            printf("終了\n");
            _Flags.k();
        }
        pre_program = _Switch.d->program;
    }
}

/*-----------------------------------------------
 *
 * 共有メモリ初期化
 *
-----------------------------------------------*/
void shmInit(void)
{
    _Switch.init();

    printf("*** shm was initialized. ***\n\n");
}