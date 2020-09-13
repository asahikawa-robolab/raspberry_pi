#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include "../../share/inc/_utility.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"

extern void test(jibiki::ProcOperateAuto *control,
                 std::vector<std::string> param,
                 size_t seq[]);
extern void thread_kbhit(jibiki::ShareVal<bool> &exit_flag,
                         jibiki::ShareVal<bool> &start_flag,
                         jibiki::ShareVal<bool> &reset_flag,
                         jibiki::ShareVal<int> &pushed_key,
                         jibiki::ShareValVec<std::string> &executing_order);
extern void process_display(jibiki::ShareVal<bool> &exit_flag,
                            jibiki::ShareVal<int> &pushed_key,
                            jibiki::ShareValVec<std::string> &executing_order);

int main(void)
{
    jibiki::ShareVal<bool> exit_flag(false);
    jibiki::ShareVal<bool> start_flag(false);
    jibiki::ShareVal<bool> reset_flag(false);
    jibiki::ShareVal<jibiki::thread::OperateMethod>
        operate_method(jibiki::thread::OPERATE_AUTO);     /* 使用中の操作方法 */
    jibiki::ShareVal<std::string> execute_orders("test"); /* 実行する orders */
    jibiki::ShareValVec<std::string> executing_order;     /* 実行中の order に関する文字列 */

    jibiki::ShareVal<int> pushed_key(0); /* キー入力を受け付ける */

    /* ProcOperateAuto */
    jibiki::ProcOperateAuto proc_operate_auto(std::ref(exit_flag),
                                              std::ref(start_flag),
                                              std::ref(reset_flag),
                                              std::ref(operate_method),
                                              std::ref(execute_orders),
                                              std::ref(executing_order),
                                              {test});
    /* kbhit */
    std::thread t2(thread_kbhit,
                   std::ref(exit_flag),
                   std::ref(start_flag),
                   std::ref(reset_flag),
                   std::ref(pushed_key),
                   std::ref(executing_order));
    /* display */
    std::thread t3(process_display,
                   std::ref(exit_flag),
                   std::ref(pushed_key),
                   std::ref(executing_order));

    t2.join();
    t3.join();

    return 0;
}