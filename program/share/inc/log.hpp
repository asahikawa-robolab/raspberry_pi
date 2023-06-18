#ifndef LOG_HPP
#define LOG_HPP
#include <fstream>
#include <mutex>
#include "_thread.hpp"

namespace jibiki
{
    class Log
    {

    private:
        std::ofstream m_file;
        std::map<std::string, double> m_data;
        std::mutex m_mtx;

        ShareVar<unsigned int> m_count;    /* 要素の更新が終わってから再確認したカウント */
        ShareVar<bool> m_has_printed;      /* 要素の書き込みを終えたかのフラグ */
        ShareVar<size_t> m_calc_period_ms; /* 動作周期 */
        ShareVar<bool> m_has_started;      /* 書き込みスタートしたかのフラグ */

    private:
        std::string m_file_name; /* コンストラクタのみで変更されるので排他的制御不要 */

    private:
        bool fileExists(const std::string &filePath);
        void write();
        void make_file();

    public:
        Log(std::string file_name = "log");
        void thread_log(jibiki::ShareVar<bool> &exit_flag);
        void set_data(std::string str, double val);
        void start(uint32_t calc_period_ms = 30); /* 書き込み開始 */
        void stop();                              /* 書き込み停止 */
        ~Log();
    };

    inline void Log::start(uint32_t calc_period_ms)
    {
        printf("log [%s] start\n", m_file_name.c_str());
        if (!m_file.is_open())
            make_file();

        m_calc_period_ms = calc_period_ms;
        m_count = 0;
        m_has_started = true;
    }
    inline void Log::stop()
    {
        printf("log [%s] stop\n,", m_file_name.c_str());
        m_has_started = false;
    }
} // namespace jibiki
#endif
