#include <thread>
#include "../inc/_process_operate_auto.hpp"
#include "../inc/external_variable.hpp"

extern void process_com(void);
extern void process_kbhit(void);
extern void process_display(void);
extern void process_operate_manual(void);
extern void process_operate_keyboard(void);

int main(void)
{
    /*-----------------------------------------------
     *
     * パラメータ読み込み
     *
    -----------------------------------------------*/
    try
    {
        using picojson::object;
        picojson::value json_value = jibiki::load_json_file("setting.json");
        object &obj = json_value.get<object>()["chassis"].get<object>();
        g_motor_FR = (size_t)obj["motor_FR"].get<double>();
        g_motor_FL = (size_t)obj["motor_FL"].get<double>();
        g_motor_BR = (size_t)obj["motor_BR"].get<double>();
        g_motor_BL = (size_t)obj["motor_BL"].get<double>();
        g_inverse_FR = obj["inverse_FR"].get<bool>();
        g_inverse_FL = obj["inverse_FL"].get<bool>();
        g_inverse_BR = obj["inverse_BR"].get<bool>();
        g_inverse_BL = obj["inverse_BL"].get<bool>();
        g_max_speed = obj["max_speed"].get<double>();
    }
    catch (std::string err)
    {
        std::cout << "*** error ***\n"
                  << err << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(EXIT_FAILURE);
    }

    /*-----------------------------------------------
     *
     * 並列処理
     *
    -----------------------------------------------*/
    std::thread thread_com(process_com);
    std::thread thread_display(process_display);
    std::thread thread_operate_auto(process_operate_auto);
    std::thread thread_operate_manual(process_operate_manual);
    std::thread thread_operate_keyboard(process_operate_keyboard);
    std::thread thread_kbhit(process_kbhit);

    thread_com.join();
    thread_display.join();
    thread_operate_auto.join();
    thread_operate_manual.join();
    thread_operate_keyboard.join();
    thread_kbhit.join();

    return 0;
}