#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include "../../share/inc/_utility.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../inc/mode_func.hpp"

void thread_kbhit(jibiki::ShareVar<bool> &exit_flag,
                  jibiki::ShareVar<bool> &start_flag,
                  jibiki::ShareVar<bool> &reset_flag,
                  jibiki::ShareVar<int> &pushed_key);
void process_display(jibiki::ShareVar<bool> &exit_flag,
                     jibiki::ShareVar<int> &pushed_key,
                     jibiki::ShareVarVec<std::string> &executing_order);

int main(void)
{
    jibiki::ShareVar<bool> exit_flag(false);
    jibiki::ShareVar<bool> start_flag(false);
    jibiki::ShareVar<bool> reset_flag(false);
    jibiki::ShareVar<jibiki::thread::OperateMethod>
        current_method(jibiki::thread::OPERATE_AUTO);     /* 使用中の操作方法 */
    jibiki::ShareVar<std::string> execute_orders("test"); /* 実行する orders */
    jibiki::ShareVarVec<std::string> executing_order;     /* 実行中の order に関する文字列 */

    jibiki::ShareVar<int> pushed_key(-1); /* キー入力を受け付ける */

    /* ProcOperateAuto */
    jibiki::ProcOperateAuto proc_operate_auto(std::ref(exit_flag),
                                              std::ref(start_flag),
                                              std::ref(reset_flag),
                                              std::ref(current_method),
                                              std::ref(execute_orders),
                                              std::ref(executing_order),
                                              {test});
    /* kbhit */
    std::thread t2(thread_kbhit,
                   std::ref(exit_flag),
                   std::ref(start_flag),
                   std::ref(reset_flag),
                   std::ref(pushed_key));
    /* display */
    std::thread t3(process_display,
                   std::ref(exit_flag),
                   std::ref(pushed_key),
                   std::ref(executing_order));

    t2.join();
    t3.join();

    return 0;
}