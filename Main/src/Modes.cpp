#include "../../Share/inc/_Flags.hpp"
#include "../../Share/inc/_Module.hpp"
#include "../inc/Orders.hpp"
#include "../../Share/inc/_ExternalVariable.hpp"

/*-----------------------------------------------
*
* encoder
*
-----------------------------------------------*/
void ORDER::mode_encoder(char *str)
{
    const double section_width = 500; /* セクションを分割する幅 */
    const double small_dif = 100;     /* セクションを回るときの許容誤差 */

    /* パラメータ読み込み */
    cv::Point2f odometry_target = cv::Point2f(std::stod(par[0]), std::stod(par[1]));
    double permissible_error = std::stod(par[5]);
    INTER_PARAM param(std::stod(par[2]), /* min */
                      std::stod(par[3]), /* max */
                      std::stod(par[4]), /* acc */
                      0,                 /* dif_init */
                      0,                 /* dif */
                      par[6]);           /* edge */

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        odometry_init = *_Odometry.v;
        dif_init = CALC_DIS(odometry_init, odometry_target);
        check_point.clear();

        /*-----------------------------------------------
        チェックポイントを計算
        -----------------------------------------------*/
        /* 経路を section_width の刻みで分割できる場合 */
        if (dif_init > section_width * 2)
        {
            cv::Point2f tmp;
            int check_point_num = dif_init / section_width; /* チェックポイントの数を計算 */
            double theta_init = CALC_ANGLE(odometry_init, odometry_target);

            /* 一つ目 */
            tmp.x = odometry_init.x + section_width * cos(theta_init);
            tmp.y = odometry_init.y + section_width * sin(theta_init);
            check_point.emplace_back(tmp);

            /* 二つ目以降 */
            for (int i = 0; i < check_point_num - 2; i++)
            {
                tmp.x = check_point[i].x + section_width * cos(theta_init);
                tmp.y = check_point[i].y + section_width * sin(theta_init);
                check_point.emplace_back(tmp);
            }

            /* 最後 */
            check_point.emplace_back(odometry_target);

            printf("num : %d\n", check_point_num);
            for (int i = 0; i < check_point_num; i++)
            {
                printf("(%.0lf, %.0lf)\n", check_point[i].x, check_point[i].y);
            }
        }
        /* 経路を section_width の刻みで分割できない場合 */
        else
        {
            check_point.emplace_back(odometry_target);
        }
    }

    /* 偏差を計算 */
    double dif_section = CALC_DIS(cv::Point2f(_Odometry.v->x, _Odometry.v->y), check_point[0]);
    double dif_entire = CALC_DIS(cv::Point2f(_Odometry.v->x, _Odometry.v->y), odometry_target);

    /* 偏差が許容誤差を下回ったらチェックポイントを削除 */
    if ((check_point.size() > 1 && dif_section < small_dif) || (dif_section < permissible_error))
    {
        printf("erase (%.0lf, %.0lf)\n", check_point[0].x, check_point[0].y);
        check_point.erase(check_point.begin());
    }

    /* チェックポイントをすべて回ったら終了 */
    if (check_point.empty() == true)
    {
        _Chassis.stop();
        char str2[MAX_STRING_LENGTH];
        snprintf(str2, MAX_STRING_LENGTH,
                 "%+4.0lf, %+4.0lf",
                 _Odometry.v->x, _Odometry.v->y);
        finish(str2);
        return;
    }

    /* CHASSIS にセットする */
    param.dif_init = dif_init;
    param.dif = dif_entire;
    double speed = liner_inter(param);
    double theta = CALC_ANGLE(*_Odometry.v, check_point[0]);
    _Chassis.set(MOTION(speed, theta));

    /* 表示用 */
    char tmp_str[MAX_STRING_LENGTH];
    snprintf(tmp_str, MAX_STRING_LENGTH, " (%.0lf, %.0lf)",
             odometry_target.x, odometry_target.y);
    strcat(str, tmp_str);
}

/*-----------------------------------------------
*
* limit_chassis
*
-----------------------------------------------*/
void ORDER::mode_limit_chassis(void)
{
    /* パラメータ読み込み */
    double rpm = std::stod(par[0]);
    double target_theta = DEG_RAD(std::stod(par[1]));
    char *limit_name = (char *)par[2].c_str();

    const double kp = 0.015;

    /* 初回は CHASSIS の MOTION を設定 */
    if (isFirst == true)
    {
        isFirst = false;
        odometry_init = *_Odometry.v;
        theta_control = 0;
    }

    /* theta_control を計算 */
    calc_theta_control(target_theta, kp);

    /* 足回りの動作に反映 */
    _Chassis.set(MOTION(rpm, target_theta + theta_control));

    /* リミットスイッチが押されたら終了 */
    if (_Limit.read(limit_name) == true)
    {
        _Chassis.stop();
        char str[MAX_STRING_LENGTH];
        snprintf(str, MAX_STRING_LENGTH,
                 "%+4.0lf, %+4.0lf",
                 _Odometry.v->x, _Odometry.v->y);
        finish(str);
        return;
    }
}

void ORDER::calc_theta_control(double target_theta, double kp)
{
    /* 目標経路からの誤差を計算 */
    double dif_x = _Odometry.v->x - odometry_init.x;
    double dif_y = _Odometry.v->y - odometry_init.y;
    double error = fabs((dif_x * tan(target_theta) - dif_y) * cos(target_theta));

    /* +/- どちらにずれているかを判定 */
    double angle_dif = CALC_ANGLE_DIF(target_theta, CALC_ANGLE(odometry_init, *_Odometry.v));

    /* 移動方向制御量を計算 */
    theta_control = error * kp * GET_SIGNAL(angle_dif);

    /* -pi/2 ~ pi/2 に収める */
    theta_control = (theta_control > M_PI_2) ? M_PI_2 : theta_control;
    theta_control = (-M_PI_2 > theta_control) ? -M_PI_2 : theta_control;
}

/*-----------------------------------------------
 *
 * turn
 *
-----------------------------------------------*/
void ORDER::mode_turn(void)
{
    /* パラメータ読み込み */
    double target_spin = DEG_RAD(std::stod(par[0]));
    std::string spin_mode0 = par[1];

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;

        /* モード決定 */
        CALC_ANGLE_MODE spin_mode = MODE_CW;
        if (spin_mode0 == "CW")
        {
            spin_mode = MODE_CW;
        }
        else if (spin_mode0 == "CCW")
        {
            spin_mode = MODE_CCW;
        }
        else if (spin_mode0 == "SHORTEST")
        {
            spin_mode = MODE_SHORTEST;
        }
        else
        {
            PrintError("ORDER::mode_turn()",
                       "invalid spin_mode0",
                       spin_mode0.c_str());
            _Flags.e(EXIT_FAILURE);
        }

        /* 値を反映 */
        _Chassis.set_target_spin(target_spin, spin_mode);
    }

    _Chassis.set();

    /* 終了 */
    if (fabs(_Chassis.get_spin_dif()) < DEG_RAD(2))
    {
        finish((char *)"");
        return;
    }
}

/*-----------------------------------------------
*
* angle
*
-----------------------------------------------*/
void ORDER::mode_angle(char *str)
{
    /* パラメータ読み込み */
    char *actuator_name = (char *)par[0].c_str();
    int value = std::stoi(par[1]);
    char tmp_str[MAX_STRING_LENGTH];
    snprintf(tmp_str, MAX_STRING_LENGTH, "(%s)", par[1].c_str());
    strcat(str, tmp_str);

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        pre_value = CONVERGE_TRUE;
        _Actuator.set(actuator_name, value); /* 目標値をセット */
    }

    /* 収束したら終了 */
    int value_converge = _Actuator.get_converge(actuator_name);
    if ((pre_value == CONVERGE_FALSE) && (value_converge == CONVERGE_TRUE))
    {
        char str2[MAX_STRING_LENGTH];
        snprintf(str2, MAX_STRING_LENGTH,
                 "pre : %d, curr : %d,name : %s",
                 pre_value, value_converge, actuator_name);
        finish(str2);
        return;
    }

    /* 更新 */
    pre_value = value_converge;
}

/*-----------------------------------------------
*
* servo
*
-----------------------------------------------*/
void ORDER::mode_servo(void)
{
    /* パラメータ読み込み */
    char *actuator_name = (char *)par[0].c_str();
    int value = std::stoi(par[1]);
    int wait_time_sec = std::stoi(par[2]);

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        time_s = GetTime();
        _Actuator.set(actuator_name, value); /* 目標値をセット */
    }

    /* この関数を初めて呼び出してから wait_time_sec 以上経過したら終了 */
    if (CALC_SEC(time_s, GetTime()) > wait_time_sec)
    {
        finish((char *)"");
        return;
    }
}

/*-----------------------------------------------
*
* motor
*
-----------------------------------------------*/
void ORDER::mode_motor(void)
{
    struct timespec time_e;
    char *motor_name = (char *)par[0].c_str();
    int duty = std::stoi(par[1]);
    int wait_time_sec = std::stoi(par[2]);

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        timespec_get(&time_s, TIME_UTC);
        _Actuator.set(motor_name, duty); /* 目標値をセット */
    }

    /* 経過時間を計算するため */
    timespec_get(&time_e, TIME_UTC);

    /* この関数を初めて呼び出してから wait_time_sec 以上経過したら終了 */
    if (CALC_SEC(time_s, time_e) > wait_time_sec)
    {
        finish((char *)"");
        return;
    }
}

/*-----------------------------------------------
*
* set_odometry
*
-----------------------------------------------*/
void ORDER::mode_set_odometry(void)
{
    const double wait_time_sec = 0.5;

    /* 初回 */
    if (isFirst == true)
    {
        _Chassis.stop();
        isFirst = false;
        timespec_get(&time_s, TIME_UTC);

        /* エラーチェック */
        if (par[0][0] != 'x' || par[1][0] != 'y')
        {
            PrintError("ORDER::mode_set_odometry()",
                       "invalid parameter",
                       par[0].c_str());
            _Flags.e(EXIT_FAILURE);
        }
    }

    /* wait_time_sec [秒]経過したら終了 */
    if (CALC_SEC(time_s, GetTime()) > wait_time_sec)
    {
        /* 値を反映 */
        /* x */
        if (par[0].size() > 1)
        {
            par[0].erase(par[0].begin());
            _Odometry.v->x = std::stod(par[0]);
        }
        /* y */
        if (par[1].size() > 1)
        {
            par[1].erase(par[1].begin());
            _Odometry.v->y = std::stod(par[1]);
        }

        /* メッセージ */
        char str[MAX_STRING_LENGTH];
        snprintf(str, MAX_STRING_LENGTH,
                 "%+4.0lf, %+4.0lf",
                 _Odometry.v->x, _Odometry.v->y);
        finish(str);
    }
}

/*-----------------------------------------------
 *
 * set_spin
 *
-----------------------------------------------*/
void ORDER::mode_set_spin(void)
{
    const double wait_time_sec = 0.5;

    /* パラメータ読み込み */
    double spin_offset = std::stod(par[0]);

    /* 初回 */
    if (isFirst == true)
    {
        _Chassis.stop();
        isFirst = false;
        timespec_get(&time_s, TIME_UTC);
    }

    /* wait_time_sec [秒]経過したら終了 */
    if (CALC_SEC(time_s, GetTime()) > wait_time_sec)
    {
        /* spin_offset に値を反映 */
        _Chassis.write_spin_offset(DEG_RAD(spin_offset));
        _Chassis.set_target_spin(DEG_RAD(spin_offset));

        /* メッセージ */
        char str[MAX_STRING_LENGTH];
        snprintf(str, MAX_STRING_LENGTH,
                 "%.2lf", spin_offset);
        finish(str);
    }
}

/*-----------------------------------------------
*
* along_line
*
-----------------------------------------------*/
void ORDER::mode_along_line(char *str)
{
    const int permissible_err = 50;
    double dif = 0;

    /* パラメータ読み込み */
    double target_odometry = std::stod(par[0]);
    double theta = std::stod(par[4]);
    double gain = std::stod(par[5]);
    INTER_PARAM param(std::stod(par[1]), /* min */
                      std::stod(par[2]), /* max */
                      std::stod(par[3]), /* acc */
                      0,                 /* dif_init */
                      0,                 /* dif */
                      par[6]);           /* edge */

    /* オドメトリから偏差を計算 */
    char tmp_str[MAX_STRING_LENGTH]; /* 表示用 */
    if (theta == 0 || theta == 180)
    {
        dif = fabs(target_odometry - _Odometry.v->x);
        snprintf(tmp_str, MAX_STRING_LENGTH, "(x = %.0lf)", target_odometry);
    }
    else
    {
        dif = fabs(target_odometry - _Odometry.v->y);
        snprintf(tmp_str, MAX_STRING_LENGTH, "(y = %.0lf)", target_odometry);
    }
    strcat(str, tmp_str);

    /* 偏差が許容誤差を下回ったら終了 */
    if (dif < permissible_err)
    {
        _Chassis.stop();
        finish((char *)"");
        return;
    }

    if (isFirst == true)
    {
        isFirst = false;
        dif_init = dif;
        /* エラーチェック */
        if ((int)theta % 90 != 0)
        {
            PrintError("ORDER::mode_along_line()", "invalid theta", theta);
            _Flags.e(EXIT_FAILURE);
        }
    }

    /* CHASSIS にセットする */
    param.dif_init = dif_init;
    param.dif = dif;
    double speed = liner_inter(param);
    MOTION motion = MOTION(speed, DEG_RAD(theta));
    _Line.calc_corr(DEG_RAD(theta), motion, gain);
    _Chassis.set(motion);
}

/*-----------------------------------------------
*
* detect_line
*
-----------------------------------------------*/
void ORDER::mode_detect_line(void)
{
    /* パラメータ読み込み */
    double speed = std::stod(par[0]);
    double theta = DEG_RAD(std::stod(par[1]));
    std::string x_or_y = par[2];

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        time_s = GetTime();
    }

    /* ライントレーサが反応したら complete_cnt を加算 */
    if (x_or_y == "x" && _Line.read(0.) != 0 && _Line.read(M_PI) != 0)
    {
        _Chassis.stop();
    }
    else if (x_or_y == "y" && _Line.read(M_PI_2) != 0 && _Line.read(3 * M_PI_2) != 0)
    {
        _Chassis.stop();
    }
    else
    {
        _Chassis.set(MOTION(speed, theta));
        time_s = GetTime();
    }

    /* ライントレーサが一定回数以上連続で反応したら終了 */
    if (CALC_SEC(time_s, GetTime()) > 1)
    {
        finish((char *)"");
        return;
    }
}

/*-----------------------------------------------
 *
 * positioning_line
 *
-----------------------------------------------*/
void ORDER::mode_positioning_line(void)
{
    /* パラメータ読み込み */
    double speed = std::stod(par[0]);
    std::string x_or_y = par[1];
    double odometry_value = std::stod(par[2]);

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        time_s = GetTime();

        /* エラーチェック */
        if (x_or_y != "x" && x_or_y != "y")
        {
            PrintError("ORDER::mode_positioning_line()",
                       "invalid x_or_y",
                       x_or_y.c_str());
            _Flags.e(EXIT_FAILURE);
        }
    }

    /* 使用するライントレーサを決定 */
    double line = (x_or_y == "x") ? 0 : 3 * M_PI_2;
    uint8_t data = _Line.read(line);
    uint8_t pre_data = _Line.read(line, true);

    /* エラーチェック */
    if (data == 0)
    {
        // PrintError("ORDER::mode_positioning_line()",
        //            "invalid line value",
        //            (double)data);
        // _Flags.e(EXIT_FAILURE);
        data = pre_data;
    }

    /* 位置が合ったらモータを停止 */
    if (data == 0b00011000)
    {
        _Chassis.stop();
    }
    /* 合っていなかったら修正する */
    else
    {
        int direction = 0;
        double theta = 0;

        /* 中央の二個が付いていた場合 */
        if (GET_BIT(data, 3) & GET_BIT(data, 4))
        {
            direction = GET_BIT(data, 5) ? 1 : -1;
        }
        /* 中央の二個が付いていない場合 */
        else
        {
            direction = GET_UP(data) ? 1 : -1;
        }

        theta = (x_or_y == "x") ? direction * M_PI_2 : (1 - direction) * M_PI_2;
        _Chassis.set(MOTION(speed, theta));
        time_s = GetTime();
    }

    /* 位置合わせ完了 */
    if (CALC_SEC(time_s, GetTime()) > 1)
    {
        /* オドメトリを上書き */
        if (x_or_y == "x")
        {
            _Odometry.v->y = odometry_value;
        }
        else
        {
            _Odometry.v->x = odometry_value;
        }

        /* 終了 */
        finish((char *)"");
        return;
    }
}

/*-----------------------------------------------
 *
 * test
 *
-----------------------------------------------*/
void ORDER::mode_test(void)
{
    struct timespec time_e;

    /* パラメータ読み込み */
    int wait_time = std::stod(par[0]);

    /* 初回 */
    if (isFirst == true)
    {
        isFirst = false;
        timespec_get(&time_s, TIME_UTC);
    }

    /* 時間が経過したら終了 */
    timespec_get(&time_e, TIME_UTC);
    if (CALC_SEC(time_s, time_e) > wait_time)
    {
        char str[MAX_STRING_LENGTH];
        snprintf(str, MAX_STRING_LENGTH, "%ds", wait_time);
        finish(str);
        return;
    }
}

/*-----------------------------------------------
*
* wait
*
-----------------------------------------------*/
void ORDER::mode_wait(void)
{
    if (isFirst == true)
    {
        isFirst = false;
    }

    if (_Flags.f[FLAG_START] == true)
    {
        finish((char *)"");
        return;
    }
}

/*-----------------------------------------------
 *
 * その他
 *
-----------------------------------------------*/