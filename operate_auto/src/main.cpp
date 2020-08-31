#include <iostream>
#include <sstream>
#include <thread>
#include <random> /* 乱数 */
#include <unistd.h>
#include "../../share/inc/_utility.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"

void mode_wait(jibiki::ProcOperateAuto *data,
               std::vector<std::string> param,
               size_t seq[])
{
    std::string order_name = param[0];
    size_t wait_times = std::stoi(param[1]);

    /* 乱数生成器 */
    static std::mt19937_64 mt64(0);

    for (size_t i = 0; i < wait_times; ++i)
    {
        if (!data->manage_thread_int(true))
            break;

        /* 乱数 */
        std::uniform_int_distribution<uint64_t> get_rand_uni_int(0, 3E3);
        int rand_wait_time = get_rand_uni_int(mt64);

        /* executing_order */
        std::stringstream sstr;
        sstr << rand_wait_time * 1E-3 << "[s]";
        data->set_executing_order(seq, sstr.str());

        /* 待機 */
        usleep(rand_wait_time * 1E3);

        /* 表示 */
        std::cout << order_name << " " << i + 1
                  << "/ " << wait_times << std::endl;
    }
}

void thread_kbhit(jibiki::ShareVal<bool> &exit_flag,
                  jibiki::ShareVal<bool> &start_flag,
                  jibiki::ShareVal<bool> &reset_flag,
                  jibiki::ShareValVec<std::string> &executing_order)
{
    if (!jibiki::enable_thread("kbhit"))
        return;

    while (jibiki::manage_thread(exit_flag.read()))
    {
        if (jibiki::kbhit())
            switch (getchar())
            {
            case 's':
                start_flag = start_flag.read() ^ 1;
                // std::cout << "start : " << start_flag.read() << std::endl;
                break;
            case 'r':
                reset_flag = reset_flag.read() ^ 1;
                // std::cout << "reset : " << reset_flag.read() << std::endl;
                break;
            case 'p':
                for (size_t i = 0; i < executing_order.size(); ++i)
                    std::cout << "\t\t" << executing_order.read(i) << std::endl;
                break;
            case 'q':
                exit_flag = true;
                break;
            }
    }
}

int main(void)
{
    jibiki::ShareVal<bool> exit_flag(false);
    jibiki::ShareVal<bool> start_flag(false);
    jibiki::ShareVal<bool> reset_flag(false);
    jibiki::ShareVal<jibiki::OperateMethod> operate_method(jibiki::OPERATE_AUTO);
    jibiki::ShareVal<std::string> execute_orders("test");
    jibiki::ShareValVec<std::string> executing_order;

    /* ProcOperateAuto */
    jibiki::ProcOperateAuto proc_operate_auto(std::ref(exit_flag),
                                              std::ref(start_flag),
                                              std::ref(reset_flag),
                                              std::ref(operate_method),
                                              std::ref(execute_orders),
                                              std::ref(executing_order),
                                              {mode_wait});
    /* kbhit */
    std::thread t2(thread_kbhit,
                   std::ref(exit_flag),
                   std::ref(start_flag),
                   std::ref(reset_flag),
                   std::ref(executing_order));

    proc_operate_auto.join();
    t2.join();

    return 0;
}