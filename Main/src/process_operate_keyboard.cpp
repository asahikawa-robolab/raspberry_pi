#include <iostream>
#include <cstdlib>
#include <vector>
#include <string>
#include "../inc/_utility.hpp"
#include "../inc/module.hpp"
#include "../inc/external_variable.hpp"

/* using 宣言 */
using std::string;

/*-----------------------------------------------
 *
 * main
 *
-----------------------------------------------*/
void process_operate_keyboard(void)
{
    printf("[start operate_keyboard]\n");
    atexit([]() { printf("[end operate_keyboard]\n"); });

    try
    {
        /*-----------------------------------------------
        設定ファイル読み込み
        -----------------------------------------------*/
        picojson::value json_value;
        json_value =
            jibiki::load_json_file("setting.json");
        /* 終了 */
        if (json_value
                .get<picojson::object>()["enable"]
                .get<picojson::object>()["process_operate_keyboard"]
                .get<bool>() == false)
            return;

        /*-----------------------------------------------
        メインループ
        -----------------------------------------------*/
        while (manage_process(OPERATE_KEYBOARD))
        {
            bool is_matched = true;
            if (g_flags[FLAG_KEY] != 0)
            {
                switch (g_flags[FLAG_KEY])
                {
                case 'u':
                case 'j':
                {
                    double inc_or_dec =
                        (g_flags[FLAG_KEY] == 'u') ? 1.0 : -1.0;

                    string mode_str = g_keyboard_mode[g_keyboard_mode_cnt];
                    if (mode_str == "chassis_speed")
                        g_chassis.m_speed += inc_or_dec * 10;
                    else if (mode_str == "chassis_theta")
                        g_chassis.m_theta += inc_or_dec * jibiki::DEG_RAD(10);
                    else if (mode_str == "chassis_spin")
                        g_chassis.m_spin += inc_or_dec * jibiki::DEG_RAD(10);
                    else if (mode_str == "servo1")
                        g_com_data["servo1"] += inc_or_dec * 10;
                    else if (mode_str == "servo2")
                        g_com_data["servo2"] += inc_or_dec * 10;
                    else if (mode_str == "servo3")
                        g_com_data["servo3"] += inc_or_dec * 10;
                    else if (mode_str == "servo4")
                        g_com_data["servo4"] += inc_or_dec * 10;
                    else if (mode_str == "servo5")
                        g_com_data["servo5"] += inc_or_dec * 10;
                    else if (mode_str == "servo6")
                        g_com_data["servo6"] += inc_or_dec * 10;
                    else if (mode_str == "servo7")
                        g_com_data["servo7"] += inc_or_dec * 10;
                    else if (mode_str == "servo8")
                        g_com_data["servo8"] += inc_or_dec * 10;
                    break;
                }
                default:
                    is_matched = false;
                }

                /* キー情報クリア */
                if (is_matched)
                    g_flags[FLAG_KEY] = 0;
            }
        }
    }
    catch (std::string err)
    {
        std::cout << "*** error ***\n"
                  << err << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(EXIT_FAILURE);
    }
}