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
        g_pwm[0] = tgt_pwm;
    else if(name == "motor_2")
        g_pwm[1] = tgt_pwm;
    else
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << "name が一致しません";
        throw sstr.str();
    }
}