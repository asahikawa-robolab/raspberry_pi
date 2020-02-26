#include <stdint.h>
#include <iostream>
#include "../../Share/inc/_std_func.hpp"
#include "../inc/_utility.hpp"
#include "../inc/module.hpp"
#include "../inc/external_variable.hpp"

void process_kbhit(void)
{
    printf("[start kbhit]\n");
    atexit([]() { printf("[end kbhit]\n"); });

    /*-----------------------------------------------
    設定ファイル読み込み
    -----------------------------------------------*/
    try
    {
        picojson::value json_value;
        json_value =
            jibiki::load_json_file("setting.json");
        /* 終了 */
        if (json_value
                .get<picojson::object>()["enable"]
                .get<picojson::object>()["process_kbhit"]
                .get<bool>() == false)
            return;
    }
    catch (std::string err)
    {
        std::cout << "*** error ***\n"
                  << err << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(EXIT_FAILURE);
    }

    while (manage_process(OPERATE_NONE))
    {
        /*-----------------------------------------------
        キー入力を受け付ける
        -----------------------------------------------*/
        if (jibiki::kbhit())
            g_flags[FLAG_KEY] = getchar();

        /*-----------------------------------------------
        キー入力に応じた処理
        -----------------------------------------------*/
        if (g_flags[FLAG_KEY] != 0)
        {
            bool is_matched = true;
            switch (g_flags[FLAG_KEY])
            {
            /* 終了 */
            case 'q':
            {
                std::cout << "*** quit ***" << std::endl;
                g_flags[FLAG_EXIT] = true;
                exit(EXIT_SUCCESS);
                break;
            }
            /* 操作方法変更 */
            case 'c':
            {
                if (g_flags[FLAG_OPERATE] < 3)
                    ++g_flags[FLAG_OPERATE];
                else
                    g_flags[FLAG_OPERATE] = 1;
                break;
            }
            /* オドメトリ変更 */
            case 'd':
                g_odometry.x += 10;
                break;
            case 'a':
                g_odometry.x -= 10;
                break;
            case 'w':
                g_odometry.y += 10;
                break;
            case 's':
                g_odometry.y -= 10;
                break;
            /* order, 操作対象変更 */
            case 'n':
            {
                /* 自動 */
                if (g_flags[FLAG_OPERATE] == OPERATE_AUTO)
                {
                    if (g_load_order.m_index_cnt + 1 < g_load_order.m_order_list.size())
                        ++g_load_order.m_index_cnt;
                    else
                        g_load_order.m_index_cnt = 0;
                }
                /* キーボード */
                else if (g_flags[FLAG_OPERATE] == OPERATE_KEYBOARD)
                {
                    ++g_keyboard_mode_cnt;
                    if (g_keyboard_mode_cnt >= g_keyboard_mode.size())
                        g_keyboard_mode_cnt = 0;
                }
                break;
            }
            case 'p':
            {
                /* 自動 */
                if (g_flags[FLAG_OPERATE] == OPERATE_AUTO)
                {
                    if (g_load_order.m_index_cnt > 0)
                        g_load_order.m_index_cnt--;
                    else
                        g_load_order.m_index_cnt = g_load_order.m_order_list.size() - 1;
                }
                /* キーボード */
                else if (g_flags[FLAG_OPERATE] == OPERATE_KEYBOARD)
                {
                    if (g_keyboard_mode_cnt == 0)
                        g_keyboard_mode_cnt = g_keyboard_mode.size() - 1;
                    else
                        g_keyboard_mode_cnt--;
                }
                break;
            }
            /* その他 */
            default:
                is_matched = false;
            }

            /* 自動操縦開始 */
            if (g_flags[FLAG_KEY] == 'l')
            {
                g_flags[FLAG_START] = true;
                is_matched = true;
            }
            /* 自動操縦リセット */
            if (g_flags[FLAG_KEY] == 'r')
            {
                g_flags[FLAG_RESET] = true;
                is_matched = true;
            }

            /*-----------------------------------------------
            入力されたキー情報をクリア
            -----------------------------------------------*/
            if (is_matched)
                g_flags[FLAG_KEY] = 0;
        }
        else
        {
            g_flags[FLAG_START] = false;
            g_flags[FLAG_RESET] = false;
        }
    }
}