#include <sstream>
#include "../../share/inc/_utility.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../inc/ext_var.hpp"

#include <inttypes.h>

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
            if(abs(g_rev_tgt[0].read() - g_rev_curr[0].read()) < 5)
                break;
        }
    }
    else if(name == "motor_2")
    {
        g_rev_tgt[1] = tgt_rev;
        while(control->manage_thread_int())
        {
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

void rot(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    short tgt_rot = std::stoi(param[1]);


    if(name == "motor_1")
    {
        g_rot_tgt[0] = tgt_rot;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_rev_tgt[0].read() - g_rot_curr[0].read()) < 5)
                printf("a");
                // break;
        }
    }
    else if(name == "motor_2")
    {
        g_rot_tgt[1] = tgt_rot;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(g_rev_tgt[1].read() - g_rot_curr[1].read()) < 5)
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

void odometry(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    int64_t tgt_dist = std::stoi(param[1]);

    g_odometry_flag[0] = 1;
    g_odometry_flag[1] = 1;
    printf("%"PRIx64"\n", tgt_dist);
    /* 鉛直方向のオドメーター */
    if(name == "odometry_1")
    {
        /* odometryのリセット */
        g_dist_tgt[0] = tgt_dist;
        g_odometry_flag[0] = 0;
        while(1)
        {
            printf("%"PRIx64"\n", tgt_dist - g_dist_curr[0].read());
            if(!control->manage_thread_int())
                break;  
            if(abs(tgt_dist - g_dist_curr[0].read()) < 5)
            {
                printf("match\n");
                // break;
            }   
        }
    }
    /* 水平方向のオドメーター */
    else if(name == "odometry_2")
    {
        /* odometryのリセット */
        g_dist_tgt[1] = tgt_dist;
        g_odometry_flag[1] = 0;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(tgt_dist - g_dist_tgt[1].read()) < 5)
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

void rev(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    short tgt_rev = std::stoi(param[1]);


    if(name == "motor_1")
    {
        g_rev[0] = tgt_rev;
        while(control->manage_thread_int())
        {
            if(abs(g_rev_diff[0].read()) < 5)
                break;
        }
    }
    else if(name == "motor_2")
    {
        g_rev[1] = tgt_rev;
        while(control->manage_thread_int())
        {
            if(abs(g_rev_diff[1].read()) < 5)
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
    short tgt_rot = std::stoi(param[1]);


    if(name == "motor_1")
    {
        g_rot[0] = tgt_rot;
        while(control->manage_thread_int())
        {
            if(abs(g_rot_diff[0].read()) < 5)/* 偏差になってないし */
                break;
        }
    }
    else if(name == "motor_2")
    {
        g_rot[1] = tgt_rot;
        while(control->manage_thread_int())
        {
            if(abs(g_rot_diff[1].read()) < 5)
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

void odometry(jibiki::ProcOperateAuto *control,
          std::vector<std::string> param,
          size_t seq[])
{
    /* パラメータ取得 */
    std::string name = param[0];
    int32_t tgt_dist = std::stoi(param[1]);

    g_odometry_flag[0] = 1;
    g_odometry_flag[1] = 1;
    printf("%lf\n", tgt_dist);
    /* 鉛直方向のオドメーター */
    if(name == "odometry_1")
    {
        /* odometryのリセット */
        g_dist[0] = tgt_dist;
        g_odometry_flag[0] = 0;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(tgt_dist - g_dist[0].read()) < 5)
                break;           
        }
    }
    /* 水平方向のオドメーター */
    else if(name == "odometry_2")
    {
        /* odometryのリセット */
        g_dist[1] = tgt_dist;
        g_odometry_flag[1] = 0;
        while(1)
        {
            if(!control->manage_thread_int())
                break;
            if(abs(tgt_dist - g_dist[1].read()) < 5)
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