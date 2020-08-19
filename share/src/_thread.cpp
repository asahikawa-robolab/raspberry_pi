/*-----------------------------------------------
 *
 * Last updated : 2020/08/25, 05:31
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#include <unistd.h>
#include "../inc/_thread.hpp"
#include "../inc/_picojson.hpp"
#include "../inc/_std_func.hpp"

namespace jibiki
{
    bool manage_thread(bool exit_flag,
                               OperateMethod my_method,
                               OperateMethod current_method)
    {
        /* exit_flag が立ったら処理を終了 */
        if (exit_flag)
            return false;

        /* operate_flag と operate_method が一致するまで待機 */
        while (my_method != current_method)
            usleep(10);

        /* 動作周期調整 */
        usleep(10);

        return true;
    }

    /*-----------------------------------------------
    *
    * json ファイルに記載したスレッドの ON / OFF を読み出す
    *
    -----------------------------------------------*/
    bool enable_thread(std::string thread_name,
                               std::string json_path)
    {
        /* using 宣言 */
        using picojson::array;
        using picojson::object;

        picojson::value json_value = jibiki::load_json_file(json_path);
        object &json_obj = json_value
                               .get<object>()["thread"]
                               .get<object>();

        return json_obj[thread_name].get<bool>();
    }
} // namespace jibiki