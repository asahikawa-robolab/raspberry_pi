#include <stdio.h>
#include <vector>
#include <string>
#include "../../Share/inc/_Flags.hpp"
#include "../inc/_Module.hpp"
#include "../inc/_ExternalVariable.hpp"

/*-----------------------------------------------
*
* ACTUATOR
*
-----------------------------------------------*/
ACTUATOR::ACTUATOR(void)
{
    value = (int *)shmOpen(SHM_ACTUATOR_VALUE,
                           sizeof(int) * NUM_ACTUATOR,
                           SHM_NAME_ACTUATOR_VALUE);
    converge = (int *)shmOpen(SHM_ACTUATOR_COMPLETE,
                              sizeof(int) * NUM_ACTUATOR,
                              SHM_NAME_ACTUATOR_COMPLETE);
    init();
}

ACTUATOR::~ACTUATOR(void)
{
    shmClose(SHM_ACTUATOR_VALUE);
    shmClose(SHM_ACTUATOR_COMPLETE);
}

void ACTUATOR::init(void)
{
    /* 初期化 */
    for (int i = 0; i < NUM_ACTUATOR; i++)
    {
        value[i] = _InitActuatorValue[i];
        converge[i] = CONVERGE_ZERO_POINT_ADJUSTING;
    }
}

int my_stoi(const char src[][MAX_STRING_LENGTH], int src_num, const char *name)
{
    int num = 0;
    bool isFound = false;

    /* src から一致する文字列を探す */
    for (int i = 0; i < src_num; i++)
    {
        if (strcmp(src[i], name) == 0)
        {
            num = i;
            isFound = true;
        }
    }

    /* src のどれとも一致しなかった場合 */
    if (isFound == false)
    {
        PrintError("my_stoi()", "invalid name", (char *)name);
        _Flags.e(EXIT_FAILURE);
    }

    return num;
}

/*-----------------------------------------------
*
* LINE
*
-----------------------------------------------*/
LINE::LINE(void)
{
    /* 共有メモリ */
    v = (uint8_t *)shmOpen(SHM_LINE,
                           sizeof(uint8_t) * NUM_LINE,
                           SHM_NAME_LINE);
    pre_data = (uint8_t *)shmOpen(SHM_PRE_LINE,
                                  sizeof(uint8_t) * NUM_LINE,
                                  SHM_NAME_PRE_LINE);
    init();

/* bases の設定 */
#ifdef TEAM_A
    bases.emplace_back(LINE_BASIS("front"));       /* 0 deg */
    bases.emplace_back(LINE_BASIS("left"));        /* 90 deg */
    bases.emplace_back(LINE_BASIS("front", true)); /* 180 deg */
    bases.emplace_back(LINE_BASIS("right"));       /* 270 deg */
#endif
#ifdef TEAM_B
    bases.emplace_back(LINE_BASIS("right"));       /* 0 deg */
    bases.emplace_back(LINE_BASIS("front"));       /* 90 deg */
    bases.emplace_back(LINE_BASIS("left"));        /* 180 deg */
    bases.emplace_back(LINE_BASIS("front", true)); /* 270 deg */
#endif
};

LINE::~LINE(void)
{
    shmClose(SHM_LINE);
}

bool LINE::read(const char *line_name, int bit)
{
    /* エラーチェック */
    if (BETWEEN(0, bit, 8) == false)
    {
        PrintError("LINE::read()", "invalid bit", bit);
        _Flags.e(EXIT_FAILURE);
    }

    uint8_t data = v[my_stoi(_Lines, NUM_LINE, line_name)];

    return GET_BIT(data, bit);
}

/*-----------------------------------------------
引数で与えられた角度の向きに取り付けられたライントレーサの値を
ロボットの回転を考慮して返す
-----------------------------------------------*/
uint8_t LINE::read(double theta0, bool previous)
{
    int theta = RAD_DEG(theta0);

    /* エラーチェック */
    if (theta % 90 != 0)
    {
        PrintError("LINE::read()", "invalid theta", theta);
        _Flags.e(EXIT_FAILURE);
    }

    /* 角度を 0 [deg] ~ 360 [deg] に収める */
    while (theta < 0)
    {
        theta += 360;
    }
    while (360 < theta)
    {
        theta -= 360;
    }

    /* ロボットの回転角を４方向のいずれかに丸め込む */
    double min_dif = M_PI_2;
    int min_num = 0;                    /* 角度偏差の大きさが最小となる向き */
    double spin = _Chassis.read_spin(); /* ロボットの回転角 */
    for (int i = 0; i < 4; i++)
    {
        double dif = fabs(CALC_ANGLE_DIF(M_PI_2 * i, spin)); /* 角度偏差の大きさを計算 */
        if (min_dif > dif)
        {
            min_dif = dif;
            min_num = i;
        }
    }

    /* 値を返すライントレーサのインデックスを決定する */
    int index = theta / 90;
    index -= min_num; /* ロボットの回転を考慮する */
    while (index < 0) /* LINE::bases を使用するために 非負にする */
    {
        index += 4;
    }
    index %= 4; /* 0 ~ 3 に収める */

    return read(bases[index].line_name, bases[index].inverse, previous);
}

void LINE::calc_corr(double theta, MOTION &motion, double gain)
{
    uint8_t data = _Line.read(theta);
    uint8_t pre_data = _Line.read(theta, true);
    double corr_value = 0;

    /* ラインから外れた場合は前の値を使う */
    data = (data == 0) ? pre_data : data;

    /* 補正値を決定する */
    bool up = GET_BIT(data, 7) | GET_BIT(data, 6) | GET_BIT(data, 5);
    bool middle = GET_BIT(data, 4) | GET_BIT(data, 3);
    bool low = GET_BIT(data, 2) | GET_BIT(data, 1) | GET_BIT(data, 0);
    uint8_t condition = (up << 2) | (middle << 1) | low;

    switch (condition)
    {
    case 0b001:
        corr_value = -1;
        break;
    case 0b010:
    case 0b011:
        corr_value = 0;
        break;
    case 0b100:
    case 0b110:
        corr_value = 1;
        break;
    default:
        corr_value = 0;
        // PrintError("LINE::calc_corr()",
        //            "invalid data",
        //            (double)data);
        // _Flags.e(EXIT_FAILURE);
    }

    corr_value *= gain;
    motion.theta += DEG_RAD(corr_value);
}

/*-----------------------------------------------
*
* LIMIT
*
-----------------------------------------------*/
LIMIT::LIMIT(void)
{
    v = (bool *)shmOpen(SHM_LIMIT,
                        sizeof(uint8_t) * NUM_LIMIT,
                        SHM_NAME_LIMIT);
    init();
}

LIMIT::~LIMIT(void)
{
    shmClose(SHM_LIMIT);
}

/*-----------------------------------------------
 *
 * ODOMETRY
 *
-----------------------------------------------*/
ODOMETRY::ODOMETRY(void)
{
    v = (cv::Point2f *)shmOpen(SHM_ODOMETRY,
                               sizeof(cv::Point2f),
                               SHM_NAME_ODOMETRY);
    init();
}

ODOMETRY::~ODOMETRY(void)
{
    shmClose(SHM_ODOMETRY);
}

/*-----------------------------------------------
*
* CHASSIS
*
-----------------------------------------------*/
CHASSIS::CHASSIS(void)
{
    /* 共有メモリ */
    spin = (double *)shmOpen(SHM_SPIN,
                             sizeof(double),
                             SHM_NAME_SPIN);
    target_spin = (double *)shmOpen(SHM_TARGET_SPIN,
                                    sizeof(double),
                                    SHM_NAME_TARGET_SPIN);
    spin_offset = (double *)shmOpen(SHM_SPIN_OFFSET,
                                    sizeof(double),
                                    SHM_NAME_SPIN_OFFSET);
    motion = (MOTION *)shmOpen(SHM_MOTION,
                               sizeof(MOTION),
                               SHM_NAME_MOTION);
    init();
}

CHASSIS::~CHASSIS(void)
{
    shmClose(SHM_SPIN);
    shmClose(SHM_TARGET_SPIN);
    shmClose(SHM_SPIN_OFFSET);
    shmClose(SHM_MOTION);
}

void CHASSIS::set(MOTION motion0)
{
    /* エラーチェック */
    if (BETWEEN(0, motion0.speed, SPEED_MAX + 1) == false)
    {
        PrintError("CHASSIS::set()", "invalid speed", motion0.speed);
        _Flags.e(EXIT_FAILURE);
    }

    /* spin を t_spin に近づけるために rotate を計算 */
    int rotate = calc_rotate();

    /* rpm を計算 */
    int rpm[4];
    rpm[FR] = (int)(motion0.speed * sin(motion0.theta - *spin - M_PI_4) + rotate);
    rpm[FL] = (int)(motion0.speed * sin(motion0.theta - *spin + M_PI_4) - rotate);
    rpm[BR] = (int)(motion0.speed * sin(motion0.theta - *spin + M_PI_4) + rotate);
    rpm[BL] = (int)(motion0.speed * sin(motion0.theta - *spin - M_PI_4) - rotate);

    /* ACTUATOR に反映 */
    _Actuator.set("FR", rpm[0]);
    _Actuator.set("FL", rpm[1]);
    _Actuator.set("BR", rpm[2]);
    _Actuator.set("BL", rpm[3]);

    /* motion に反映 */
    *motion = motion0;
}

void CHASSIS::set(void)
{
    /* spin を t_spin に近づけるために rotate を計算 */
    int rotate = calc_rotate();

    /* rpm を計算 */
    int rpm[4];
    rpm[FR] = (int)(motion->speed * sin(motion->theta - *spin - M_PI_4) + rotate);
    rpm[FL] = (int)(motion->speed * sin(motion->theta - *spin + M_PI_4) - rotate);
    rpm[BR] = (int)(motion->speed * sin(motion->theta - *spin + M_PI_4) + rotate);
    rpm[BL] = (int)(motion->speed * sin(motion->theta - *spin - M_PI_4) - rotate);

    /* ACTUATOR に反映 */
    _Actuator.set("FR", rpm[0]);
    _Actuator.set("FL", rpm[1]);
    _Actuator.set("BR", rpm[2]);
    _Actuator.set("BL", rpm[3]);
}

void CHASSIS::stop(void)
{
    set(MOTION(0, 0));
    _Actuator.set("FR", 0);
    _Actuator.set("FL", 0);
    _Actuator.set("BR", 0);
    _Actuator.set("BL", 0);
}

int CHASSIS::calc_rotate(void)
{
    /* パラメータ */
    const double MinRPM = 20;
    const double MaxRPM = 50;
    const double Kp = 1;

    int rotate;
    double dif = RAD_DEG(get_spin_dif());

    /* 偏差が小さくなったら最短モードに変更 */
    if (fabs(dif) < 90 && spin_mode != MODE_SHORTEST)
    {
        spin_mode = MODE_SHORTEST;
    }

    /* 偏差に応じて rotate を決定 */
    if (fabs(dif) < 2)
    {
        rotate = 0;
    }
    else
    {
        rotate = dif * Kp;
        if (fabs(rotate) < MinRPM)
        {
            rotate = MinRPM * GET_SIGNAL(rotate);
        }
        else if (MaxRPM < fabs(rotate))
        {
            rotate = MaxRPM * GET_SIGNAL(rotate);
        }
    }

    return rotate;
}

/*-----------------------------------------------
 *
 * Switch
 *
-----------------------------------------------*/
bool SWITCH::once(SWITCH_ONCE name, bool data)
{
    /* 押された瞬間かどうかを判定する */
    if (data == true)
    {
        button_cnt[name]++;
    }
    else
    {
        button_cnt[name] = 0;
    }
    if (button_cnt[name] == 1)
    {
        return true;
    }
    return false;
}