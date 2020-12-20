/* Last updated : 2020/10/05, 19:36 */
#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <vector>
#include <string>
#include <thread>
#include "_thread.hpp"

namespace jibiki
{
    /*-----------------------------------------------
    *
    * ProcOperateAuto
    *
    -----------------------------------------------*/
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
            Order(void) noexcept {}
            Order(size_t seq1,
                  size_t seq2,
                  size_t seq3,
                  std::string name) noexcept
                : m_seq{seq1, seq2, seq3}, m_name(name) {}
        };
        /* スレッドを制御するデータをまとめたクラス */
        class ControlData
        {
        public: /* TODO : 参照で書き直す */
            ShareVar<bool> *m_exit_flag;
            ShareVar<bool> *m_start_flag;
            ShareVar<bool> *m_reset_flag;
            ShareVar<thread::OperateMethod> *m_current_method;

        public:
            ControlData(ShareVar<bool> *exit_flag,
                        ShareVar<bool> *start_flag,
                        ShareVar<bool> *reset_flag,
                        ShareVar<thread::OperateMethod> *current_method) noexcept
                : m_exit_flag(exit_flag),
                  m_start_flag(start_flag),
                  m_reset_flag(reset_flag),
                  m_current_method(current_method) {}
        };

    private:
        /* モードを実行する関数の関数ポインタ */
        using ModeFunc = void (*)(ProcOperateAuto *control,
                                  std::vector<std::string> param,
                                  size_t seq[]);
        ControlData m_control_data;
        std::vector<Order> m_orders;                 /* 読み込んだ orders */
        std::vector<std::string> m_modes;            /* モードの一覧 */
        std::vector<ModeFunc> m_mode_func;           /* モードを実行する関数 */
        ShareVarVec<std::string> *m_executing_order; /* 実行中の order */
        ShareVar<std::string> *m_execute_orders;     /* 実行する orders */
        bool m_is_print;                             /* 実行前に orders を表示するかどうか */
        std::string m_json_path;                     /* 読み込む JSON ファイルのパス */
        std::thread m_t;

    private:
        void load_json(void);            /* JSON ファイルを読み込む */
        void launch(void);               /* 並列処理で実行する処理 */
        void load_orders(void);          /* orders を読み込む */
        void check_duplication(void);    /* seq の重複を確認 */
        void check_ordering(void);       /* seq の順序性を確認 */
        void print(void) const noexcept; /* 読み込んだ orders を表示する */
        void execute(void);
        void branch(uint8_t &finish_flag, size_t seq1, size_t seq2);
        size_t find(size_t seq1, size_t seq2, size_t seq3) const;
        size_t find(size_t seq1, size_t seq2) const;
        size_t find(size_t seq1) const;
        size_t find(void) const noexcept;

    public:
        ProcOperateAuto(ShareVar<bool> &exit_flag,
                        ShareVar<bool> &start_flag,
                        ShareVar<bool> &reset_flag,
                        ShareVar<thread::OperateMethod> &current_method,
                        ShareVar<std::string> &execute_orders,
                        ShareVarVec<std::string> &executing_order,
                        std::vector<ModeFunc> mode_func,
                        bool is_print = false,
                        std::string json_path = "order.json");
        ~ProcOperateAuto(void) { m_t.join(); }
        bool manage_thread_int(bool use_reset_flag = true);
        void set_executing_order(size_t seq[], std::string str);
        void clear_executing_order(size_t seq[]);
    };
} // namespace jibiki

#endif