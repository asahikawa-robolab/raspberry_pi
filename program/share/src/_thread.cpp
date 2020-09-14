#include <unistd.h>
#include "../inc/_thread.hpp"
#include "../inc/_picojson.hpp"
#include "../inc/_std_func.hpp"

namespace jibiki
{
    namespace thread
    {
        bool manage(ShareVar<bool> &exit_flag,
                    ShareVar<OperateMethod> &current_method,
                    OperateMethod my_method)
        {
            /* current_method と my_method が一致するまで待機 */
            while ((my_method != current_method.read()) & !exit_flag.read())
                usleep(10);

            /* exit_flag が立ったら処理を終了 */
            if (exit_flag.read())
                return false;

            /* 動作周期調整 */
            usleep(10);

            return true;
        }
        bool manage(ShareVar<bool> &exit_flag)
        {
            ShareVar<OperateMethod> current_method(OPERATE_NONE);
            return manage(exit_flag, current_method, OPERATE_NONE);
        }

        /* json ファイルに記載したスレッドの有効／無効（true/false）を読み出す */
        bool enable(std::string thread_name,
                    std::string json_path)
        {
            /* using 宣言 */
            using picojson::array;
            using picojson::object;

            picojson::value json_val = load_json_file(json_path);
            object &json_obj = json_val
                                   .get<object>()["thread"]
                                   .get<object>();

            return json_obj[thread_name].get<bool>();
        }
    } // namespace thread
} // namespace jibiki