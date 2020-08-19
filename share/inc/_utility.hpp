/*-----------------------------------------------
 *
 * Last updated : 2020/08/25, 21:14
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _UTILITY_HPP
#define _UTILITY_HPP

#include <vector>
#include <string>
#include "_picojson.hpp"

namespace jibiki
{
    class ProcOperateAuto
    {
    private:
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

    public:
        typedef void (*ModeFunc)(std::vector<std::string> param, size_t branch_num);
        std::vector<Order> m_list;
        std::vector<std::string> m_order_list;
        std::vector<std::string> m_mode_list;
        size_t m_index_cnt;
        std::vector<ModeFunc> m_mode_func;

    public:
        ProcOperateAuto(jibiki::ShareVal<bool> &exit_flag,
                        bool start_flag,
                        bool reset_flag,
                        OperateMethod current_method,
                        jibiki::ShareValVec<std::string> &executing_order,
                        std::vector<ModeFunc> mode_func,
                        std::string json_path = "order.json");
        void init(std::string json_path, std::vector<ModeFunc> mode_func);
        void load(void);
        void print(void);
        void execute(bool exit_flag,
                     bool reset_flag,
                     OperateMethod current_method,
                     jibiki::ShareValVec<std::string> &executing_order);

    private:
        void branch(bool exit_flag,
                    bool reset_flag,
                    uint8_t &finish_flag,
                    jibiki::ShareValVec<std::string> &executing_order,
                    size_t seq1,
                    size_t seq2);
        size_t find(size_t seq1, size_t seq2, size_t seq3);
        size_t find(size_t seq1, size_t seq2);
        size_t find(size_t seq1);
    };
} // namespace jibiki

#endif