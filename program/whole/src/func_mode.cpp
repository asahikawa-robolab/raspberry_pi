#include <sstream>
#include "../../share/inc/_utility.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../inc/ext_var.hpp"

void test(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ読み込み */
    size_t wait_time = std::stoi(param[0]);

    printf("[ %d, %d, %d ] start %d[s]\n",
           seq[0], seq[1], seq[2], wait_time);

    /* 指定時間待機 */
    for (size_t i = 0; i < wait_time; ++i)
    {
        /* スレッドの管理 */
        if (!control->manage_thread_int())
            break;

        /* executing_order に書き込む */
        std::stringstream sstr;
        sstr << wait_time - i << " [s]";
        control->set_executing_order(seq, sstr.str());

        /* 待機 */
        jibiki::usleep(1E6);
    }

    printf("\t\t\t[ %d, %d, %d ] finish\n", seq[0], seq[1], seq[2]);

    /* executing_order をクリア */
    control->clear_executing_order(seq);
}

void pwm(jibiki::ProcOperateAuto *control,
         std::vector<std::string> param,
         size_t seq[])
{
    /* パラメータ読み込み */
    std::string name = param[0];
    short tgt_pwm = std::stoi(param[1]);

    //    printf("[ %d, %d, %d ] %s %d[s]\n",
    //       seq[0], seq[1], seq[2], name, tgt_pwm);

    if (name == "motor_1")
        g_pwm_tgt[0] = tgt_pwm;
    else if (name == "motor_2")
        g_pwm_tgt[1] = tgt_pwm;
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}

void rev(jibiki::ProcOperateAuto *control,
         std::vector<std::string> param,
         size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    short tgt_rev = std::stoi(param[1]);

    if (name == "motor_1")
    {
        g_rev_tgt[0] = tgt_rev;
        while (control->manage_thread_int())
        {
            if (!control->manage_thread_int())
                break;
            if (abs(g_rev_tgt[0].read() - g_rev_curr[0].read()) < 5)
                break;
        }
    }
    else if (name == "motor_2")
    {
        g_rev_tgt[1] = tgt_rev;
        while (control->manage_thread_int())
        {
            if (!control->manage_thread_int())
                break;
            if (abs(g_rev_tgt[1].read() - g_rev_curr[1].read()) < 5)
                break;
        }
    }
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}

void rot(jibiki::ProcOperateAuto *control,
         std::vector<std::string> param,
         size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    int16_t tgt_rot = std::stoi(param[1]);

    if (name == "motor_1")
    {
        g_rot_tgt[0] = tgt_rot;
        jibiki::usleep(0.5E6);
        while (1)
        {
            if (!control->manage_thread_int())
                break;
            if (abs(g_rot_curr[0].read()) < 5)
                break;
        }
    }
    else if (name == "motor_2")
    {
        g_rot_tgt[1] = tgt_rot;
        jibiki::usleep(0.5E6);
        while (1)
        {
            if (!control->manage_thread_int())
                break;
            if (abs(g_rot_curr[1].read()) < 5)
                break;
        }
    }
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}

/* 初期位置を0として移動する アブソリュート指令*/
void odometry(jibiki::ProcOperateAuto *control,
              std::vector<std::string> param,
              size_t seq[])
{
    /*-----------------------------------------------
    パラメータ取得
    0->どの方向に移動するか(name)
    1->どのくらい移動するか(dist)
    2->どのくらいのスピードか(speed)
    *鉛直方向は前、水平方向は左を正の方向とする
    -----------------------------------------------*/
    std::string name = param[0];
    int32_t tgt_dist = std::stoi(param[1]);
    int8_t tgt_speed = std::stoi(param[2]);

    g_odometry_flag[0] = 0;
    g_odometry_flag[1] = 0;
    /* 水平方向のオドメーター */
    if (name == "horizon")
    {
        if (tgt_dist > 0)
            g_chassis.m_theta = jibiki::deg_rad(0);
        else if (tgt_dist < 0)
            g_chassis.m_theta = jibiki::deg_rad(180);
        g_chassis.m_speed = tgt_speed;
        g_dist_tgt[0] = tgt_dist;
        while (control->manage_thread_int())
        {
            if (abs(g_dist_tgt[0].read() - g_dist_curr[0].read()) < 5)
            {
                g_chassis.stop();
                break;
            }
        }
    }
    /* 鉛直方向のオドメーター */
    else if (name == "vertical")
    {
        if (tgt_dist > 0)
            g_chassis.m_theta = jibiki::deg_rad(90);
        else if (tgt_dist < 0)
            g_chassis.m_theta = jibiki::deg_rad(270);
        g_chassis.m_speed = tgt_speed;
        g_dist_tgt[1] = tgt_dist;
        while (control->manage_thread_int())
        {
            if (abs(g_dist_tgt[1].read() - g_dist_curr[1].read()) < 5)
            {
                g_chassis.stop();
                break;
            }
        }
    }
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}

/* 現在地を0として移動する インクリメント指令*/
void set_odometry(jibiki::ProcOperateAuto *control,
                  std::vector<std::string> param,
                  size_t seq[])
{
    /*-----------------------------------------------
    パラメータ取得
    0->どの方向に移動するか(name)
    1->どのくらい移動するか(dist)
    2->どのくらいのスピードか(speed)
    *鉛直方向は前、水平方向は左を正の方向とする
    -----------------------------------------------*/
    std::string name = param[0];
    int32_t tgt_dist = std::stoi(param[1]);
    int8_t tgt_speed = std::stoi(param[2]);

    g_odometry_flag[0] = 1;
    g_odometry_flag[1] = 1;
    /* 水平方向のオドメーター */
    if (name == "horizon")
    {
        if (tgt_dist > 0)
            g_chassis.m_theta = jibiki::deg_rad(0);
        else if (tgt_dist < 0)
            g_chassis.m_theta = jibiki::deg_rad(180);
        g_dist_tgt[0] = tgt_dist;
        g_odometry_flag[0] = 0;
        g_chassis.m_speed = tgt_speed;
        while (control->manage_thread_int())
        {
            if (abs(g_dist_tgt[0].read() - g_dist_curr[0].read()) < 5)
            {
                g_chassis.stop();
                break;
            }
        }
    }
    /* 鉛直方向のオドメーター */
    else if (name == "vertical")
    {
        if (tgt_dist > 0)
            g_chassis.m_theta = jibiki::deg_rad(90);
        else if (tgt_dist < 0)
            g_chassis.m_theta = jibiki::deg_rad(270);
        g_dist_tgt[1] = tgt_dist;
        g_odometry_flag[1] = 0;
        g_chassis.m_speed = tgt_speed;
        while (control->manage_thread_int())
        {
            if (abs(g_dist_tgt[1].read() - g_dist_curr[1].read()) < 5)
            {
                g_chassis.stop();
                break;
            }
        }
    }
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}

void limit(jibiki::ProcOperateAuto *control,
           std::vector<std::string> param,
           size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];

    while (control->manage_thread_int())
        if (name == "limit_1")
        {
            printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n", 
            g_limit[0].read(), g_limit[1].read(), g_limit[2].read(), g_limit[3].read(), 
            g_limit[4].read(), g_limit[5].read(), g_limit[6].read(), g_limit[7].read());
            if (g_limit[0].read() == true)
                break;
        }
        else
        {
            std::stringstream sstr;
            sstr << __PRETTY_FUNCTION__ << "name が一致しません";
            throw sstr.str();
        }
}
void turn(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    double tgt_angle = std::stod(param[0]);

    g_chassis.m_spin = jibiki::deg_rad(tgt_angle);
    while (control->manage_thread_int())
    {
        if (abs(jibiki::rad_deg(g_imu.read()) - tgt_angle) < 5)
        {
            g_chassis.stop();
            break;
        }
    }
}

void jerk(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{

    /*-----------------------------------------------
    躍度最小軌道

    p(t)->目標座標,　p0->初期座標, pf->最終座標, t->現在時間, tf->終了時間
    p(t) = p0 + (pf - p0) * (6(t / tf)^5 -15(t / tf)^4 + 10(t / tf)^3)
    -----------------------------------------------*/
    int32_t f_utc[2]; //pf [0] -> x, [1] -> y
    int32_t s_utc[2]; //p0
    int32_t tgt_utc[2];

    f_utc[0] = std::stoi(param[0]);
    f_utc[1] = std::stoi(param[1]);
    double f_time = std::stod(param[2]); //tf

    s_utc[0] = g_dist_curr[0].read();
    s_utc[1] = g_dist_curr[1].read();

    const double ave_speed = std::hypot((f_utc[0] - s_utc[0]), (f_utc[1] - s_utc[1])) / f_time;//平均速度
    auto s_time = jibiki::get_time();

    while (control->manage_thread_int())
    {
        if (!(abs(f_utc[0] - g_dist_curr[0].read()) < 5) && (abs(f_utc[1] - g_dist_curr[1].read()) < 5))//目標地点に到達したら足回りを止める
        {
            g_chassis.stop();
            break;
        }
        //すべての足回りが停止した(非常停止が押された)なら
        if((g_chassis_rev[0].read() < 0) && (g_chassis_rev[1].read() < 0) && (g_chassis_rev[2].read() < 0) && (g_chassis_rev[3].read() < 0))
        {
            //始点を設定しなおす
            s_utc[0] = g_dist_curr[0].read();
            s_utc[1] = g_dist_curr[1].read();
            //終了時間(f_time)を設定しなおす
            f_time = abs(f_utc[0] - g_dist_curr[0].read()) / ave_speed;
            if(f_time <= abs(f_utc[1] - g_dist_curr[1].read()) / ave_speed)
                f_time = abs(f_utc[1] - g_dist_curr[1].read()) / ave_speed;
            //開始時間を設定しなおす
            auto s_time = jibiki::get_time();
            continue;
        }
        else
        {

            auto curr_time = jibiki::calc_sec(s_time, jibiki::get_time());
            if(curr_time < f_time)
                for (int i = 0; i < 2; i++)
                    tgt_utc[i] = s_utc[i] + (f_utc[i] - s_utc[i]) * (6 * pow((curr_time / f_time), 5)
                                - 15 * pow((curr_time / f_time), 4) + 10 * pow((curr_time / f_time), 3));

            g_chassis.m_speed = abs(std::hypot(tgt_utc[0], tgt_utc[1]) - std::hypot(g_dist_curr[0].read(), g_dist_curr[1].read())) * g_chassis.rotate_kp() * 0.01; //
            g_chassis.m_theta = std::atan2(tgt_utc[1], tgt_utc[0]);//進行方向を指定

            printf("speed:%lf, theta:%lf, X:%d, Y:%d, time:%lf\n%d  %d\n", 
            g_chassis.m_speed.read(), g_chassis.m_theta.read(),tgt_utc[0], 
            tgt_utc[1], curr_time, g_dist_curr[0].read(), g_dist_curr[1].read());
        }

    }
}