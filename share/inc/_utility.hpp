/*-----------------------------------------------
 *
 * Last updated : 2020/08/31, 01:12
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <vector>
#include <string>
#include "_picojson.hpp"
#include "_thread.hpp"

namespace jibiki
{
    class ProcOperateAuto
    {
    private:
        /* オーダーを表現するクラス */
        class Order
        {
        public:
            size_t m_seq[3];
            std::string m_mode;
            std::vector<std::string> m_param;
            std::string m_name;

        public:
            Order(void) {}
            Order(size_t seq1, size_t seq2, size_t seq3, std::string name);
        };
        /* スレッドを制御するデータをまとめたクラス */
        class ControlData
        {
        public:
            jibiki::ShareVal<bool> *m_exit_flag;
            jibiki::ShareVal<bool> *m_start_flag;
            jibiki::ShareVal<bool> *m_reset_flag;
            jibiki::ShareVal<OperateMethod> *m_current_method;
        };

    private:
        /* モードを実行する関数の関数ポインタ */
        typedef void (*ModeFunc)(ProcOperateAuto *data,
                                 std::vector<std::string> param,
                                 size_t seq[]);
        ControlData m_control_data;
        std::vector<Order> m_orders;                         /* 読み込んだ orders */
        std::vector<std::string> m_modes;                    /* モードの一覧 */
        std::vector<ModeFunc> m_mode_func;                   /* モードを実行する関数 */
        jibiki::ShareValVec<std::string> *m_executing_order; /* 実行中の order */
        jibiki::ShareVal<std::string> *m_execute_orders;     /* 実行する orders */
        bool m_is_print;                                     /* 実行前に orders を表示するかどうか */
        std::thread m_t;

    private:
        void init(jibiki::ShareVal<bool> &exit_flag,
                  jibiki::ShareVal<bool> &start_flag,
                  jibiki::ShareVal<bool> &reset_flag,
                  jibiki::ShareVal<OperateMethod> &current_method,
                  jibiki::ShareVal<std::string> &execute_orders,
                  jibiki::ShareValVec<std::string> &executing_order,
                  std::vector<ModeFunc> mode_func,
                  bool is_print,
                  std::string json_path);
        void load(void);
        void print(void);
        void execute(void);
        void branch(uint8_t &finish_flag,
                    size_t seq1,
                    size_t seq2);
        size_t find(size_t seq1, size_t seq2, size_t seq3);
        size_t find(size_t seq1, size_t seq2);
        size_t find(size_t seq1);
        size_t find(void);

    public:
        ProcOperateAuto(jibiki::ShareVal<bool> &exit_flag,
                        jibiki::ShareVal<bool> &start_flag,
                        jibiki::ShareVal<bool> &reset_flag,
                        jibiki::ShareVal<OperateMethod> &current_method,
                        jibiki::ShareVal<std::string> &execute_orders,
                        jibiki::ShareValVec<std::string> &executing_order,
                        std::vector<ModeFunc> mode_func,
                        bool is_print = false,
                        std::string json_path = "order.json");
        void launch(void);
        bool manage_thread_int(bool use_reset_flag = false);
        void set_executing_order(size_t seq[], std::string str);
        inline void join(void)
        {
            m_t.join();
        }
    };
} // namespace jibiki

#endif