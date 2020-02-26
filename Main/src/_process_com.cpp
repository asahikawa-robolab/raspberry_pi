#include <stdint.h>
#include <iostream>
#include <cstdlib>
#include "../../Share/inc/_serial_communication.hpp"
#include "../../Share/inc/_std_func.hpp"
#include "../inc/external_variable.hpp"
#include "../inc/module.hpp"
#include "../inc/_utility.hpp"

/* using 宣言 */
using picojson::array;
using picojson::object;

/*-----------------------------------------------
 *
 * main
 *
-----------------------------------------------*/
void process_com(void)
{
    printf("[start com]\n");
    atexit([]() { printf("[end com]\n"); });

    try
    {
        /*-----------------------------------------------
        設定ファイル読み込み
        -----------------------------------------------*/
        picojson::value json_value =
            jibiki::load_json_file("setting.json");
        object &json_obj = json_value
                               .get<object>()["enable"]
                               .get<object>();
        /* 終了 */
        if (json_obj["process_com"].get<bool>() == false)
            return;

        /* path */
        std::vector<std::vector<std::string>> paths;
        for (auto elem1 : g_func_com)
        {
            std::vector<std::string> tmp;
            for (auto elem2 : elem1.m_paths)
            {
                tmp.emplace_back(json_value
                                     .get<object>()["com_path"]
                                     .get<object>()[elem2]
                                     .get<std::string>());
            }
            paths.emplace_back(tmp);
        }

        /*-----------------------------------------------
        通信処理
        -----------------------------------------------*/
        while (manage_process(OPERATE_NONE))
        {
            for (size_t i = 0; i < g_func_com.size(); ++i)
            {
                if (json_obj[g_func_com[i].m_name].get<bool>())
                    g_func_com[i].m_fp(paths[i]);
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