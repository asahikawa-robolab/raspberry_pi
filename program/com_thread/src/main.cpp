#include <thread>
#include <exception>
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/module.hpp"

/* 外部変数 */
jibiki::ShareVar<short> g_angle(0);
SwitchData g_switch_data;
Imu g_imu;
Controller g_controller;

/* ProcParamCom */
void com_rot_control(std::string path, std::string name);
void com_switch(std::string path, std::string name);
void com_imu(std::string path, std::string name);
void com_controller(std::string path, std::string name);

/* thread */
void thread_kbhit(jibiki::ShareVar<bool> &exit_flag);

int main(void)
{
    jibiki::ShareVar<bool> exit_flag(false); /* 終了フラグ */

    try
    {
        /* 通信 */
        jibiki::ProcParamCom com(std::ref(exit_flag),
                                 {com_rot_control,
                                  com_switch,
                                  com_imu,
                                  com_controller});
        /* キー入力 */
        std::thread t_kbhit(thread_kbhit, std::ref(exit_flag));

        t_kbhit.join();
    }
    catch (std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit_flag = true;
        return 1;
    }

    return 0;
}