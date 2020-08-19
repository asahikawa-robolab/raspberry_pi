/*-----------------------------------------------
 *
 * Last updated : 2020/08/25, 02:08
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _THREAD_HPP
#define _THREAD_HPP

#include <mutex>
#include <vector>

namespace jibiki
{
    /*-----------------------------------------------
     *
     * 内部で排他処理を行うクラス
     *
    -----------------------------------------------*/
    template <typename T>
    class ShareVal
    {
    private:
        std::mutex m_mtx;
        T m_data;

    public:
        /* コンストラクタ */
        inline ShareVal(void)
        {
            write(T());
        }
        /* 値を指定してオブジェクトを作成 */
        inline ShareVal(T val)
        {
            write(val);
        }
        /* 値の書き込み */
        inline void write(T value)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data = value;
        }
        /* 値の読み出し */
        inline T read(void)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            T ret = m_data;
            return ret;
        }
        /* 演算子のオーバーロード */
        inline void operator+=(T n) { write(read() + n); }
        inline void operator-=(T n) { write(read() - n); }
        inline void operator=(T n) { write(n); }
    };

    template <typename T>
    class ShareValVec
    {
    private:
        std::mutex m_mtx;
        std::vector<T> m_data;

    public:
        /* 要素を追加 */
        inline void push_back(T tmp)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data.push_back(tmp);
        }
        /* 要素数を取得 */
        inline size_t size(void)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            return m_data.size();
        }
        /* 値の読み出し */
        inline T read(size_t index)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            T ret = m_data[index];
            return ret;
        }
        /* 値の書き込み */
        inline void write(size_t index, T value)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data[index] = value;
        }
        /* 要素数の変更 */
        inline void resize(size_t size)
        {
            std::lock_guard<std::mutex> lock(m_mtx);
            m_data.resize(size);
        }
    };

    /*-----------------------------------------------
     *
     * スレッドの管理
     *
    -----------------------------------------------*/
    typedef enum
    {
        OPERATE_NONE,     /* 操作とは無関係 */
        OPERATE_AUTO,     /* 自動 */
        OPERATE_MANUAL,   /* 手動 */
        OPERATE_KEYBOARD, /* キーボード */
    } OperateMethod;
    bool manage_thread(bool exit_flag,
                       OperateMethod my_method = OPERATE_NONE,
                       OperateMethod current_method = OPERATE_NONE);
    bool enable_thread(std::string thread_name,
                       std::string json_path = "setting.json");
} // namespace jibiki

#endif