/*-----------------------------------------------
 *
 * Last updated : 2020/09/03, 18:31
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _THREAD_HPP
#define _THREAD_HPP

#include <mutex>
#include <vector>
#include <unistd.h>
#include <iostream>

namespace jibiki
{
    /*-----------------------------------------------
     *
     * 内部で排他制御を行うクラス
     *
    -----------------------------------------------*/
    template <typename T>
    class ShareVal
    {
    private:
        std::mutex m_mtx;
        T m_data;

    private:
        /* 値の書き込み */
        void write(T value)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data = value;
        }

    public:
        /* コンストラクタ */
        ShareVal(void)
        {
            write(T());
        }
        /* 値を指定してオブジェクトを作成 */
        ShareVal(T val)
        {
            write(val);
        }
        /* 値の読み出し */
        T read(void)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            T ret = m_data;
            return ret;
        }
        /* 指定時間ロックをかけるだけ */
        void test_lock(useconds_t lock_time)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            std::cout << "locked" << std::endl;
            usleep(lock_time);
            std::cout << "unlocked" << std::endl;
        }
        /* 演算子のオーバーロード */
        void operator+=(T n) { write(read() + n); }
        void operator-=(T n) { write(read() - n); }
        void operator=(T n) { write(n); }
    };

    template <typename T>
    class ShareValVec
    {
    private:
        std::mutex m_mtx;
        std::vector<T> m_data;

    public:
        /* 末尾に要素を追加 */
        void push_back(T tmp)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data.push_back(tmp);
        }
        /* 要素数を取得 */
        size_t size(void)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_data.size();
        }
        /* 値の読み出し */
        T read(size_t index)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            T ret = m_data[index];
            return ret;
        }
        /* 値の書き込み */
        void write(size_t index, T value)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data[index] = value;
        }
        /* 要素数の変更 */
        void resize(size_t size)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data.resize(size);
        }
        /* クリア */
        void clear(void)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data.clear();
        }
        /* 削除 */
        void erase(size_t index)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data.erase(m_data.begin() + index);
        }
    };

    /*-----------------------------------------------
     *
     * スレッドの管理
     *
    -----------------------------------------------*/
    namespace thread
    {
        typedef enum
        {
            OPERATE_NONE,     /* 操作とは無関係 */
            OPERATE_AUTO,     /* 自動 */
            OPERATE_MANUAL,   /* 手動 */
        } OperateMethod;
        bool manage(ShareVal<bool> &exit_flag,
                    ShareVal<OperateMethod> &current_method,
                    OperateMethod my_method);
        bool manage(ShareVal<bool> &exit_flag);
        bool enable(std::string thread_name,
                    std::string json_path = "setting.json");
    } // namespace thread
} // namespace jibiki

#endif