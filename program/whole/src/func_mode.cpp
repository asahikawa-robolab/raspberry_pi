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
    
    if(name == "motor_1")
        g_pwm_tgt[0] = tgt_pwm;
    else if(name == "motor_2")
        g_pwm_tgt[1] = tgt_pwm;
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}

/* 回転数 */
void rev(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    short tgt_rev = std::stoi(param[1]);


    if(name == "motor_1")
    {
        g_rev_tgt[0] = tgt_rev;
        while(control->manage_thread_int())
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_rev_tgt[0].read() - g_rev_curr[0].read()) < 5)
                break;
        }
    }
    else if(name == "motor_2")
    {
        g_rev_tgt[1] = tgt_rev;
        while(control->manage_thread_int())
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_rev_tgt[1].read() - g_rev_curr[1].read()) < 5)
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

/* 回転角 */
void rot(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    int16_t tgt_rot = std::stoi(param[1]);


    if(name == "motor_1")
    {
        g_rot_tgt[0] = tgt_rot;
        jibiki::usleep(0.5E6);
        while(1)
        {
            if(!control->manage_thread_int())
                break;  
            if(abs(g_rot_curr[0].read()) < 5)
                break;
        }
    }
    else if(name == "motor_2")
    {
        g_rot_tgt[1] = tgt_rot;
        jibiki::usleep(0.5E6);
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_rot_curr[1].read()) < 5)
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

/* 初期位置を0として移動する */
void odometry(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /*-----------------------------------------------
    パラメータ取得
    0->どの方向に移動するか(name)
    1->どのくらい移動するか(dist)
    2->どのくらいのスピードか(speed)
    *鉛直方向は前、水平方向は右を正の方向とする
    -----------------------------------------------*/
    std::string name = param[0];
    int64_t tgt_dist = std::stoi(param[1]);
    int8_t tgt_speed = std::stoi(param[2]);

    g_odometry_flag[0] = 1;
    g_odometry_flag[1] = 1;
    /* 鉛直方向のオドメーター */
    if(name == "vertical")
    {
        if(tgt_dist > 0)
            g_chassis.m_theta = 0;
        g_chassis.m_speed = tgt_speed;
        g_dist_tgt[0] = tgt_dist;
        while(1)
        {
            if(!control->manage_thread_int())
                break;  
            if(abs(g_dist_tgt[0].read() - g_dist_curr[0].read()) < 5)
                break;
        }
    }
    /* 水平方向のオドメーター */
    else if(name == "horizon")
    {
        if(tgt_dist > 0)
            g_chassis.m_theta = 270;
        g_chassis.m_speed = tgt_speed;
        g_dist_tgt[1] = tgt_dist;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_dist_tgt[1].read() - g_dist_tgt[1].read()) < 5)
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

/* 現在地を0として移動する */
void set_odometry(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /*-----------------------------------------------
    パラメータ取得
    0->どの方向に移動するか(name)
    1->どのくらい移動するか(dist)
    2->どのくらいのスピードか(speed)
    *鉛直方向は前、水平方向は右を正の方向とする
    -----------------------------------------------*/
    std::string name = param[0];
    int64_t tgt_dist = std::stoi(param[1]);
    int8_t tgt_speed = std::stoi(param[2]);

    g_odometry_flag[0] = 1;
    g_odometry_flag[1] = 1;
    /* 鉛直方向のオドメーター */
    if(name == "vertical")
    {
        if(tgt_dist > 0)
            g_chassis.m_theta = 0;
        g_chassis.m_speed = tgt_speed;
        /* odometryのリセット */
        g_dist_tgt[0] = tgt_dist;
        g_odometry_flag[0] = 0;
        while(1)
        {
            if(!control->manage_thread_int())
                break;  
            if(abs(g_dist_tgt[0].read() - g_dist_curr[0].read()) < 5)
                break;
        }
    }
    /* 水平方向のオドメーター */
    else if(name == "horizon")
    {
        if(tgt_dist > 0)
            g_chassis.m_theta = 270;
        g_chassis.m_speed = tgt_speed;
        /* odometryのリセット */
        g_dist_tgt[1] = tgt_dist;
        g_odometry_flag[1] = 0;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_dist_tgt[1].read() - g_dist_tgt[1].read()) < 5)
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

void limit(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];

    while(control->manage_thread_int())
        if(name == "limit_1")
        {
                printf("%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\n"
                ,g_limit[0].read(), g_limit[1].read(), g_limit[2].read() ,g_limit[3].read()
                ,g_limit[4].read(), g_limit[5].read(), g_limit[6].read(), g_limit[7].read());
            if(g_limit[0].read() == true)
                break;
        }
        else
        {
            std::stringstream sstr;
            sstr << __PRETTY_FUNCTION__ << "name が一致しません";
            throw sstr.str();
        }
}
