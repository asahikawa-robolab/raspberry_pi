#include <iostream>
#include <sstream>
#include <thread>
#include <unistd.h>
#include "../../share/inc/_utility.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/module.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../inc/func_com.hpp"
#include "../inc/func_mode.hpp"

void thread_manual(jibiki::ShareVar<bool> &exit_flag,
                   jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method);
void thread_kbhit(jibiki::ShareVar<bool> &exit_flag,
                  jibiki::ShareVar<bool> &start_flag,
                  jibiki::ShareVar<bool> &reset_flag,
                  jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method,
                  jibiki::ShareVar<int> &pushed_key);
void thread_display(jibiki::ShareVar<bool> &exit_flag,
                    jibiki::ShareVar<jibiki::thread::OperateMethod> &current_method,
                    jibiki::ShareVarVec<std::string> &executing_order,
                    jibiki::ShareVar<int> &pushed_key);

int main(void)
{
    /*-----------------------------------------------
    共有する変数
    -----------------------------------------------*/
    jibiki::ShareVar<bool> exit_flag(false);
    jibiki::ShareVar<bool> start_flag(false);
    jibiki::ShareVar<bool> reset_flag(false);
    jibiki::ShareVar<jibiki::thread::OperateMethod>
        current_method(jibiki::thread::OPERATE_AUTO);     /* 使用中の操作方法 */
    jibiki::ShareVar<std::string> execute_orders("test"); /* 実行する orders */
    jibiki::ShareVarVec<std::string> executing_order;     /* 実行中の order に関する文字列 */
    jibiki::ShareVar<int> pushed_key(-1);                 /* キー入力で受け取った値 */
    /*-----------------------------------------------
    通信
    -----------------------------------------------*/
    jibiki::ProcParamCom com(std::ref(exit_flag),
                             {com_pwm_control,
                              com_rot_control,
                              com_chassis_f,
                              com_chassis_b,
                              com_switch,
                              com_imu,
                              com_controller});
    /*-----------------------------------------------
    自動制御
    -----------------------------------------------*/
    jibiki::ProcOperateAuto proc_operate_auto(std::ref(exit_flag),
                                              std::ref(start_flag),
                                              std::ref(reset_flag),
                                              std::ref(current_method),
                                              std::ref(execute_orders),
                                              std::ref(executing_order),
                                              {test, pwm});
    /*-----------------------------------------------
    手動制御
    -----------------------------------------------*/
    std::thread t_manual(thread_manual,
                         std::ref(exit_flag),
                         std::ref(current_method));
    /*-----------------------------------------------
    キー入力
    -----------------------------------------------*/
    std::thread t_kbhit(thread_kbhit,
                        std::ref(exit_flag),
                        std::ref(start_flag),
                        std::ref(reset_flag),
                        std::ref(current_method),
                        std::ref(pushed_key));
    /*-----------------------------------------------
    opencv で描画
    -----------------------------------------------*/
    std::thread t_display(thread_display,
                          std::ref(exit_flag),
                          std::ref(current_method),
                          std::ref(executing_order),
                          std::ref(pushed_key));
    /*-----------------------------------------------
    スレッドが終了するまで待機
    -----------------------------------------------*/
    t_manual.join();
    t_kbhit.join();
    t_display.join();

    return 0;
}