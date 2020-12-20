/* Last updated : 2020/10/06, 00:38 */
#ifndef _THREAD_HPP
#define _THREAD_HPP

#include <mutex>
#include <vector>
#include <iostream>
#include "_std_func.hpp"

namespace jibiki
{
    /*-----------------------------------------------
    *
    * ShareVar
    *
    -----------------------------------------------*/
    template <typename T>
    class ShareVar
    {
    private:
        std::mutex m_mtx;
        T m_data;

    private:
        void write(T val); /* 値の書き込み */

    public:
        ShareVar(void) { write(T()); }              /* コンストラクタ */
        ShareVar(T val) { write(val); }             /* 値を指定してオブジェクトを作成 */
        T read(void);                               /* 値の読み出し */
        void test_lock(size_t lock_time_us);    /* 指定時間 [us] ロックをかけるだけ */
        void operator+=(T n) { write(read() + n); } /* += */
        void operator-=(T n) { write(read() - n); } /* -= */
        void operator*=(T n) { write(read() * n); } /* *= */
        void operator/=(T n) { write(read() / n); } /* /= */
        void operator^=(T n) { write(read() ^ n); } /* ^= */
        void operator=(T n) { write(n); }           /* = */
    };

    /* 値の書き込み */
    template <typename T>
    inline void ShareVar<T>::write(T val)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data = val;
    }
    /* 値の読み出し */
    template <typename T>
    inline T ShareVar<T>::read(void)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        T ret = m_data;
        return ret;
    }
    /* 指定時間 [us] ロックをかけるだけ */
    template <typename T>
    inline void ShareVar<T>::test_lock(size_t lock_time_us)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        std::cout << "locked\n";
        usleep(lock_time_us);
        std::cout << "unlocked\n";
    }
    /*-----------------------------------------------
    *
    * ShareVarVec
    *
    -----------------------------------------------*/
    template <typename T>
    class ShareVarVec
    {
    private:
        std::mutex m_mtx;
        std::vector<T> m_data;

    public:
        void push_back(T tmp);           /* 末尾に要素を追加 */
        size_t size(void);               /* 要素数を取得 */
        T read(size_t index);            /* 値の読み出し */
        void write(size_t index, T val); /* 値の書き込み */
        void resize(size_t size);        /* 要素数の変更 */
        void clear(void);                /* クリア */
        void erase(size_t index);        /* 削除 */
    };

    /* 末尾に要素を追加 */
    template <typename T>
    inline void ShareVarVec<T>::push_back(T tmp)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.emplace_back(tmp);
    }
    /* 要素数を取得 */
    template <typename T>
    inline size_t ShareVarVec<T>::size(void)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        return m_data.size();
    }
    /* 値の読み出し */
    template <typename T>
    inline T ShareVarVec<T>::read(size_t index)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        T ret = m_data.at(index);
        return ret;
    }
    /* 値の書き込み */
    template <typename T>
    inline void ShareVarVec<T>::write(size_t index, T val)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.at(index) = val;
    }
    /* 要素数の変更 */
    template <typename T>
    inline void ShareVarVec<T>::resize(size_t size)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.resize(size);
    }
    /* クリア */
    template <typename T>
    inline void ShareVarVec<T>::clear(void)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        m_data.clear();
    }
    /* 削除 */
    template <typename T>
    inline void ShareVarVec<T>::erase(size_t index)
    {
        std::lock_guard<std::mutex> lock(m_mtx);
        // TODO:out_of_range の例外処理
        m_data.erase(m_data.begin() + index);
    }
    /*-----------------------------------------------
     *
     * thread
     *
    -----------------------------------------------*/
    namespace thread
    {
        typedef enum
        {
            OPERATE_NONE,   /* 操作とは無関係 */
            OPERATE_AUTO,   /* 自動 */
            OPERATE_MANUAL, /* 手動 */
        } OperateMethod;
        bool manage(ShareVar<bool> &exit_flag,
                    ShareVar<OperateMethod> &current_method,
                    const OperateMethod my_method);
        bool manage(ShareVar<bool> &exit_flag);
        bool enable(std::string thread_name,
                    std::string json_path = "setting.json");
    } // namespace thread
} // namespace jibiki

#endif