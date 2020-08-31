#include <thread>
#include <exception>
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_thread.hpp"
#include "../inc/module.hpp"

/* 外部変数 */
jibiki::ShareVal<short> g_angle(0);
SwitchData g_switch_data;
ImuData g_imu_data;
ControllerData g_controller_data;

/* ProcParamCom */
extern void com_rot_control(std::string path, std::string name);
extern void com_switch(std::string path, std::string name);
extern void com_imu(std::string path, std::string name);
extern void com_controller(std::string path, std::string name);

/* thread */
extern void thread_kbhit(jibiki::ShareVal<bool> &exit_flag);

int main(void)
{
    jibiki::ShareVal<bool> exit_flag(false); /* 終了フラグ */

    try
    {
        /* 通信 */
        std::thread t_com([&exit_flag] {
            jibiki::ProcParamCom com("setting.json",
                                     std::ref(exit_flag),
                                     {com_rot_control,
                                      com_switch,
                                      com_imu,
                                      com_controller});
        });
        /* キー入力 */
        std::thread t_kbhit(thread_kbhit, std::ref(exit_flag));

        t_com.join();
        t_kbhit.join();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
        exit_flag = true;
        return;
    }

    return 0;
}