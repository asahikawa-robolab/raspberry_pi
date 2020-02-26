#ifndef PROCESS_OPERATE_AUTO_HPP
#define PROCESS_OPERATE_AUTO_HPP

#include <vector>
#include <string>
#include "module.hpp"

void process_operate_auto(void);

/*-----------------------------------------------
 *
 * Order
 *
-----------------------------------------------*/
class Order
{
public:
    size_t m_seq[3];
    size_t m_mode;
    std::vector<std::string> m_param;
    std::string m_name;

public:
    Order(void) {}
    Order(size_t seq1, size_t seq2, size_t seq3, std::string name);
};

/*-----------------------------------------------
 *
 * Orders
 *
-----------------------------------------------*/
class Orders
{
public:
    Orders(void);
    std::vector<Order> m_list;

public:
    void load(void);
    void print(void);
    void execute(void);

private:
     jibiki::AssocArray<size_t> m_dict;
    void branch(uint8_t *finish_flag, size_t seq1, size_t seq2);
    size_t find(size_t seq1, size_t seq2, size_t seq3);
    size_t find(size_t seq1, size_t seq2);
    size_t find(size_t seq1);
};

/*-----------------------------------------------
 *
 * LoadOrder
 *
-----------------------------------------------*/
class LoadOrder
{
public:
    LoadOrder(void);
    std::vector<std::string> m_order_list;
    size_t m_index_cnt;
};

/*-----------------------------------------------
 *
 * Mode
 *
-----------------------------------------------*/
typedef void (*ModeFunc)(std::vector<std::string> param, size_t branch_num);

class Mode
{
public:
    Mode(ModeFunc fp, std::string name, size_t param_num)
    {
        m_fp = fp;
        m_name = name;
        m_param_num = param_num;
    }
    ModeFunc m_fp;
    std::string m_name;
    size_t m_param_num;
};

#endif