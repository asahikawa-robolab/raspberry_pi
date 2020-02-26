#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include "../../Share/inc/_picojson.hpp"
#include "../../Share/inc/_std_func.hpp"
#include "../inc/_process_operate_auto.hpp"
#include "../inc/external_variable.hpp"
#include "../inc/module.hpp"
#include "../inc/_utility.hpp"

/*-----------------------------------------------
 *
 * Order
 *
-----------------------------------------------*/
Order::Order(size_t seq1, size_t seq2, size_t seq3, std::string name)
{
    m_seq[0] = seq1;
    m_seq[1] = seq2;
    m_seq[2] = seq3;
    m_name = name;
}

/*-----------------------------------------------
 *
 * Orders
 *
-----------------------------------------------*/
Orders::Orders(void)
{
    /* mode の名前とインデックスを変換するための辞書を作成 */
    for (size_t i = 0; i < g_mode_dict.size(); ++i)
        m_dict.add(g_mode_dict[i].m_name, i);
}

void Orders::load(void)
{
    /* using 宣言 */
    using picojson::array;
    using picojson::object;

    /* json ファイルを picojson で読み込む */
    picojson::value json_value =
        jibiki::load_json_file("order.json");

    /*-----------------------------------------------
    Order に読み込む
    -----------------------------------------------*/
    std::string order_name = g_load_order
                                 .m_order_list[g_load_order.m_index_cnt];
    array &root_array = json_value
                            .get<object>()[order_name.c_str()]
                            .get<array>();
    for (size_t i = 0; i < root_array.size(); ++i)
    {
        Order tmp;
        /* seq */
        for (size_t j = 0; j < 3; ++j)
            tmp.m_seq[j] = (size_t)root_array[i]
                               .get<object>()["seq"]
                               .get<array>()[j]
                               .get<double>();
        /* mode */
        tmp.m_mode = m_dict[root_array[i]
                                .get<object>()["mode"]
                                .get<std::string>()];
        /* param */
        array &param_array = root_array[i]
                                 .get<object>()["param"]
                                 .get<array>();
        for (size_t j = 0; j < param_array.size(); ++j)
            tmp.m_param.push_back(param_array[j]
                                      .get<std::string>());
        if (tmp.m_param.size() != g_mode_dict[tmp.m_mode].m_param_num)
        {
            std::stringstream sstr;
            sstr << __PRETTY_FUNCTION__ << std::endl;
            sstr << "パラメータの個数が g_mode_dict と一致しません．" << std::flush;
            sstr << "(" << tmp.m_seq[0] << ", "
                 << tmp.m_seq[1] << ", "
                 << tmp.m_seq[2] << ")" << std::endl;
            throw sstr.str();
        }
        /* name */
        tmp.m_name = root_array[i]
                         .get<object>()["name"]
                         .get<std::string>();
        /* リストに追加 */
        m_list.emplace_back(tmp);
    }
}

void Orders::print(void)
{
    std::cout << "-------------------------------" << std::endl;
    for (size_t i = 0; i < m_list.size(); ++i)
    {
        printf("%s\n%d, %d, %d : %s\n",
               m_list[i].m_name.c_str(),
               m_list[i].m_seq[0],
               m_list[i].m_seq[1],
               m_list[i].m_seq[2],
               g_mode_dict[m_list[i].m_mode].m_name.c_str());
        for (size_t j = 0; j < m_list[i].m_param.size(); ++j)
            std::cout << m_list[i].m_param[j] << ", " << std::flush;
        printf("\n\n");
    }
    std::cout << "-------------------------------" << std::endl;
}

/*-----------------------------------------------
 *
 * Order のリストに従って実行
 *
-----------------------------------------------*/
void Orders::execute(void)
{
    std::cout << "*** execute start ***" << std::endl;
    while (manage_process(OPERATE_AUTO))
    {
        static size_t seq1 = 0;
        /*-----------------------------------------------
        枝の数を取得
        -----------------------------------------------*/
        size_t seq2_max = find(seq1); /* seq2 の最大値 */
        /* 終了 */
        if (seq2_max == std::numeric_limits<size_t>::max())
        {
            seq1 = 0; /* 静的変数初期化 */

            if (m_list.empty())
                std::cout << "*** execute reset ***" << std::endl;
            else
                std::cout << "*** execute complete ***" << std::endl;

            /* 直後にまた execute が実行されないようにするため */
            if (!m_list.empty())
                m_list.clear();

            break;
        }
        size_t branch_num = seq2_max + 1; /* 枝の数 */

        /*-----------------------------------------------
        枝の数だけ Orders::branch() を実行
        -----------------------------------------------*/
        std::vector<uint8_t> finish_flag(branch_num, 0); /* スレッドの終了フラグ */
                                                         /* bool[] を std::vector<bool> で代用することはできないので， */
                                                         /* bool 型の代わりに uint8_t 型をフラグとして使用 */
        for (size_t i = 0; i < branch_num; ++i)
        {
            std::thread t(&Orders::branch,
                          this,
                          &finish_flag[i],
                          seq1,
                          i);
            t.detach();
        }

        /*-----------------------------------------------
        全てのスレッドの処理が終了するまで待機
        -----------------------------------------------*/
        while (1)
        {
            /* 未完了のスレッドが見つからなかったら break */
            auto itr = std::find(finish_flag.begin(), finish_flag.end(), 0);
            if (itr == finish_flag.end())
                break;
        }

        /*-----------------------------------------------
        次のブロックに進む
        -----------------------------------------------*/
        ++seq1;
    }
}

/*-----------------------------------------------
 *
 * 分岐した枝毎の処理
 *
-----------------------------------------------*/
void Orders::branch(uint8_t *finish_flag, size_t seq1, size_t seq2)
{
    /*-----------------------------------------------
    要素数を取得
    -----------------------------------------------*/
    size_t seq3_max = find(seq1, seq2);
    if (seq3_max == std::numeric_limits<size_t>::max())
    {
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << std::endl
             << seq1 << ", " << seq2 << std::endl;
        throw sstr.str();
    }
    size_t element_num = seq3_max + 1;

    /*-----------------------------------------------
    Order を順次実行
    -----------------------------------------------*/
    for (size_t seq3 = 0; seq3 < element_num; ++seq3)
    {
        printf("%d %d %d\n", seq1, seq2, seq3);

        /* Order のインデックスを取得 */
        size_t order_index = find(seq1, seq2, seq3);
        if (order_index == std::numeric_limits<size_t>::max())
        {
            std::stringstream sstr;
            sstr << __PRETTY_FUNCTION__ << std::endl
                 << seq1 << ", " << seq2 << ", " << seq3 << std::endl;
            throw sstr.str();
        }

        /* executing_order 追加 */
        if (g_executing_order.size() <= seq2)
            g_executing_order.resize(seq2 + 1);
        std::stringstream sstr;
        sstr << "[" << seq1 << ", " << seq2 << ", " << seq3 << "] "
             << g_mode_dict[m_list[order_index].m_mode].m_name << std::flush;
        g_executing_order[seq2] = sstr.str();

        /* Order を実行 */
        g_mode_dict[m_list[order_index].m_mode]
            .m_fp(m_list[order_index].m_param, seq2);

        /* executing_order から削除 */
        g_executing_order[seq2] = std::string();

        /* exit_flag が立っていたら終了 */
        if (g_flags[FLAG_EXIT] | g_flags[FLAG_RESET])
        {
            if (g_flags[FLAG_RESET])
                m_list.clear();
            break;
        }
    }

    /* スレッドが終了したことを通知する */
    *finish_flag = 1;
}

/* 引数に指定した seq と一致する list のインデックスを返す */
/* 見つからなかったら size_t の最大値を返す */
size_t Orders::find(size_t seq1, size_t seq2, size_t seq3)
{
    for (size_t i = 0; i < m_list.size(); ++i)
    {
        bool _1 = (m_list[i].m_seq[0] == seq1);
        bool _2 = (m_list[i].m_seq[1] == seq2);
        bool _3 = (m_list[i].m_seq[2] == seq3);
        if (_1 & _2 & _3)
            return i;
    }
    return std::numeric_limits<size_t>::max();
}

/* 引数で指定した seq において，seq3 の最大値を返す */
/* 見つからなかったら size_t の最大値を返す */
size_t Orders::find(size_t seq1, size_t seq2)
{
    bool is_found = false;
    size_t max = 0;
    for (size_t i = 0; i < m_list.size(); ++i)
    {
        bool _1 = (m_list[i].m_seq[0] == seq1);
        bool _2 = (m_list[i].m_seq[1] == seq2);
        bool _3 = (m_list[i].m_seq[2] >= max);
        if (_1 & _2 & _3)
        {
            max = m_list[i].m_seq[2];
            is_found = true;
        }
    }

    /* １つも見つからなかった場合 */
    if (!is_found)
        return std::numeric_limits<size_t>::max();

    return max;
}

/* 引数で指定した seq において，seq2 の最大値を返す */
/* 見つからなかったら size_t の最大値を返す */
size_t Orders::find(size_t seq1)
{
    bool is_found = false;
    size_t max = 0;
    for (size_t i = 0; i < m_list.size(); ++i)
    {
        bool _1 = (m_list[i].m_seq[0] == seq1);
        bool _2 = (m_list[i].m_seq[1] >= max);
        if (_1 & _2)
        {
            max = m_list[i].m_seq[1];
            is_found = true;
        }
    }

    /* １つも見つからなかった場合 */
    if (!is_found)
        return std::numeric_limits<size_t>::max();

    return max;
}

/*-----------------------------------------------
 *
 * LoadOrder
 *
-----------------------------------------------*/
LoadOrder::LoadOrder(void)
{
    /* 初期化 */
    m_index_cnt = 0;

    /* json ファイルを読み込む */
    picojson::value json_value =
        jibiki::load_json_file("order.json");

    /* order_list を作成 */
    picojson::array &order_list_array =
        json_value
            .get<picojson::object>()["order_list"]
            .get<picojson::array>();
    for (size_t i = 0; i < order_list_array.size(); ++i)
    {
        m_order_list.push_back(
            order_list_array[i]
                .get<std::string>());
    }
}

/*-----------------------------------------------
 *
 * main
 *
-----------------------------------------------*/
void process_operate_auto(void)
{
    printf("[start operate_auto]\n");
    atexit([]() { printf("[end operate_auto]\n"); });

    g_flags[FLAG_OPERATE] = OPERATE_AUTO;
    Orders orders;

    try
    {
        /*-----------------------------------------------
        設定ファイル読み込み
        -----------------------------------------------*/
        picojson::value json_value;
        json_value =
            jibiki::load_json_file("setting.json");
        /* 終了 */
        if (json_value
                .get<picojson::object>()["enable"]
                .get<picojson::object>()["process_operate_auto"]
                .get<bool>() == false)
            return;

        /*-----------------------------------------------
        メインループ
        -----------------------------------------------*/
        while (manage_process(OPERATE_AUTO))
        {
            if (g_flags[FLAG_START])
                orders.load();
            if (!orders.m_list.empty())
                orders.execute();
        }
    }
    catch (std::string err)
    {
        std::cout << "*** error ***\n"
                  << err << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(EXIT_FAILURE);
    }
    catch (int exit_code)
    {
        std::cout << "*** exit_code : " << exit_code << " ***" << std::endl;
        g_flags[FLAG_EXIT] = true;
        exit(exit_code);
    }
}