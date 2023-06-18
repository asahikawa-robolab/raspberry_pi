#include <iostream>
#include <sys/stat.h>
#include <sstream>
// /* UNIX */
#include <fcntl.h>
#include <unistd.h>
/* jibiki */
#include "../inc/log.hpp"

namespace jibiki
{
    /* コンストラクタ */
    Log::Log(std::string file_name)
        : m_count(0),
          m_has_printed(false),
          m_calc_period_ms(30),
          m_file_name(file_name)
    {
        mkdir("../../log", 0777); // フォルダ作成
    }
    /* デストラクタ */
    Log::~Log()
    {
        m_file.close();
    }
    /* ファイルの存在を確認 */
    bool Log::fileExists(const std::string &filePath)
    {
        std::ifstream file(filePath);
        return file.good();
    }
    /* ファイルを作成 */
    void Log::make_file()
    {
        int i = 1;
        while (fileExists("../../log/" + m_file_name + std::to_string(i) + ".csv"))
            i++;
        m_file.open("../../log/" + m_file_name + std::to_string(i) + ".csv", std::ios::out);
    }
    /* データをセット */
    void Log::set_data(std::string str, double val)
    {
        std::lock_guard<std::mutex> lock(m_mtx); // スレッドセーフのロック

        // エラーチェック

        if (str.find(",") != std::string::npos)
        {
            std::stringstream sstr;
            sstr << "set_data 中の str の値が不適切です．\n"
                 << "カンマを含まないでください．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(""); /* エラー発生 */
        }
        /* 番号と文字の分離（ex: 001value ⇒ 001,value） */
        std::string num = "0";
        for (const auto &pair : m_data)
        {
            std::string key = pair.first;
            std::string n, s;
            n = key.substr(0, 3);
            s = key.substr(3);
            if (s == str)
                num = n;
        }
        if (num != "0") // 既にkeyが存在するなら
        {
            m_count += 1;
            str = num + str;
        }
        else
        {
            m_has_printed = false;
            m_count = 0;
            std::string S = std::to_string(m_data.size() + 1);
            str = std::string(std::max(0, 3 - (int)S.size()), '0') + S + str; // 新しく要素を追加
        }
        /* 要素の値を更新 */
        m_data[str] = val;
    }

    /* 書き込み */
    void Log::write()
    {
        std::lock_guard<std::mutex> lock(m_mtx); // スレッドセーフのロック

        if (m_count.read() != 0 && m_count.read() == m_data.size() && !m_has_printed.read()) /* 先頭に要素名を書き込み */
        {
            for (const auto &pair : m_data)
            {
                std::string key = pair.first;
                m_file << key.substr(3) << ",";
            }
            m_file << std::endl;

            m_has_printed = true;
        }
        if (m_has_printed.read()) /* 値を書き込み */
        {
            for (const auto &pair : m_data)
            {
                double value = pair.second;
                m_file << std::to_string(value) << ",";
            }
            m_file << std::endl;
        }
    }
    /* スレッド処理 */
    void Log::thread_log(jibiki::ShareVar<bool> &exit_flag)
    {
        while (jibiki::thread::manage(exit_flag))
        {
            if (m_has_started.read())
            {
                write();
                usleep(m_calc_period_ms.read() * 1E3); // 待機
            }
        }
    }
}