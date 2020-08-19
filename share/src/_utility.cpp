/*-----------------------------------------------
 *
 * Last updated : 2020/08/25, 21:14
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
#include "../../share/inc/_picojson.hpp"
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_utility.hpp"

namespace jibiki
{
    /*-----------------------------------------------
     *
     * ProcOperateAuto::Order
     *
    -----------------------------------------------*/
    ProcOperateAuto::Order::Order(size_t seq1,
                                  size_t seq2,
                                  size_t seq3,
                                  std::string name)
    {
        m_seq[0] = seq1;
        m_seq[1] = seq2;
        m_seq[2] = seq3;
        m_name = name;
    }

    /*-----------------------------------------------
     *
     * ProcOperateAuto
     *
    -----------------------------------------------*/
    ProcOperateAuto::ProcOperateAuto(jibiki::ShareVal<bool> &exit_flag,
                                     bool start_flag,
                                     bool reset_flag,
                                     OperateMethod current_method,
                                     jibiki::ShareValVec<std::string> &executing_order,
                                     std::vector<ProcOperateAuto::ModeFunc> mode_func,
                                     std::string json_path)
    {
        init(json_path, mode_func);

        try
        {
            /* スレッドの実行の管理 */
            if (!enable_thread("operate_auto"))
                return;

            while (manage_thread(exit_flag.read(),
                                 OPERATE_AUTO,
                                 current_method))
            {
                if (start_flag)
                    load();
                if (!m_list.empty())
                    execute(exit_flag.read(),
                            reset_flag,
                            current_method,
                            executing_order);
            }
        }
        catch (std::string err)
        {
            std::cout << "*** error ***\n"
                      << err << std::endl;
            exit_flag = true;
            return;
        }
        catch (int exit_code)
        {
            std::cout << "*** exit_code : " << exit_code << " ***" << std::endl;
            exit_flag = true;
            return;
        }
    }

    /* 初期化 */
    void ProcOperateAuto::init(std::string json_path, std::vector<ModeFunc> mode_func)
    {
        /* using 宣言 */
        using picojson::array;
        using picojson::object;

        /* 初期化 */
        m_index_cnt = 0;
        m_mode_func = mode_func;

        /* json ファイルを読み込む */
        picojson::value json_value =
            jibiki::load_json_file(json_path);

        /* order_list に値を設定 */
        array &order_list_array =
            json_value
                .get<object>()["order_list"]
                .get<array>();
        for (size_t i = 0; i < order_list_array.size(); ++i)
        {
            m_order_list.push_back(
                order_list_array[i]
                    .get<std::string>());
        }

        /* mode_list に値を設定 */
        array &mode_list_array =
            json_value
                .get<object>()["mode_list"]
                .get<array>();
        for (size_t i = 0; i < mode_list_array.size(); ++i)
        {
            m_mode_list.push_back(
                mode_list_array[i]
                    .get<std::string>());
        }
    }

    /* order を読み込む */
    void ProcOperateAuto::load(void)
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
        /* g_load_order：複数のオーダーを管理するクラス */
        /* m_order_list：オーダーのリスト */
        /* m_index_cnt：実行するオーダーの番号 */
        /* 実行するオーダーの文字列を取得 */
        std::string order_name = m_order_list[m_index_cnt];
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
            tmp.m_mode = root_array[i]
                             .get<object>()["mode"]
                             .get<std::string>();
            /* param */
            array &param_array = root_array[i]
                                     .get<object>()["param"]
                                     .get<array>();
            for (size_t j = 0; j < param_array.size(); ++j)
                tmp.m_param.push_back(param_array[j]
                                          .get<std::string>());
            // if (tmp.m_param.size() != g_mode_dict[tmp.m_mode].m_param_num)
            // {
            //     std::stringstream sstr;
            //     sstr << __PRETTY_FUNCTION__ << std::endl;
            //     sstr << "パラメータの個数が g_mode_dict と一致しません．" << std::flush;
            //     sstr << "(" << tmp.m_seq[0] << ", "
            //          << tmp.m_seq[1] << ", "
            //          << tmp.m_seq[2] << ")" << std::endl;
            //     throw sstr.str();
            // }
            /* name */
            tmp.m_name = root_array[i]
                             .get<object>()["name"]
                             .get<std::string>();
            /* リストに追加 */
            m_list.emplace_back(tmp);
        }
    }

    /* order を表示 */
    void ProcOperateAuto::print(void)
    {
        std::cout << "-------------------------------" << std::endl;
        for (size_t i = 0; i < m_list.size(); ++i)
        {
            printf("%s\n%d, %d, %d : %s\n",
                   m_list[i].m_name.c_str(),
                   m_list[i].m_seq[0],
                   m_list[i].m_seq[1],
                   m_list[i].m_seq[2],
                   m_list[i].m_mode.c_str());
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
    void ProcOperateAuto::
        execute(bool exit_flag,
                bool reset_flag,
                OperateMethod current_method,
                jibiki::ShareValVec<std::string> &executing_order)
    {
        std::cout << "*** execute start ***" << std::endl;
        while (manage_thread(exit_flag, OPERATE_AUTO, current_method))
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
            枝の数だけ branch() を実行
            -----------------------------------------------*/
            std::vector<uint8_t> finish_flag(branch_num, 0); /* スレッドの終了フラグ */
                                                             /* bool[] を std::vector<bool> で代用することはできないので， */
                                                             /* bool 型の代わりに uint8_t 型をフラグとして使用 */
            for (size_t i = 0; i < branch_num; ++i)
            {
                std::thread t(&ProcOperateAuto::branch,
                              this,
                              exit_flag,
                              reset_flag,
                              std::ref(finish_flag[i]),
                              std::ref(executing_order),
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
    void ProcOperateAuto::
        branch(bool exit_flag,
               bool reset_flag,
               uint8_t &finish_flag,
               jibiki::ShareValVec<std::string> &executing_order,
               size_t seq1,
               size_t seq2)
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
            if (executing_order.size() <= seq2)
                executing_order.resize(seq2 + 1);
            std::stringstream sstr;
            sstr << "[" << seq1 << ", " << seq2 << ", " << seq3 << "] "
                 << m_list[order_index].m_mode << std::flush;
            executing_order.write(seq2, sstr.str());

            /* Order を実行 */
            auto itr = std::find(m_mode_list.begin(),
                                 m_mode_list.end(),
                                 m_list[order_index].m_mode);
            if (itr == m_mode_list.end())
            {
                std::stringstream sstr;
                sstr << __PRETTY_FUNCTION__ << std::endl;
                sstr << "存在しないモード [ " << m_list[order_index].m_mode
                     << " ] が指定されています" << std::endl;
                throw sstr.str();
            }
            size_t mode_index = itr - m_mode_list.begin();
            m_mode_func[mode_index](m_list[order_index].m_param, seq2);

            /* executing_order から削除 */
            executing_order.write(seq2, std::string());

            /* exit_flag が立っていたら終了 */
            if (exit_flag | reset_flag)
            {
                if (reset_flag)
                    m_list.clear();
                break;
            }
        }

        /* スレッドが終了したことを通知する */
        finish_flag = 1;
    }

    /* 引数に指定した seq と一致する list のインデックスを返す */
    /* 見つからなかったら size_t の最大値を返す */
    size_t ProcOperateAuto::find(size_t seq1, size_t seq2, size_t seq3)
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
    size_t ProcOperateAuto::find(size_t seq1, size_t seq2)
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
    size_t ProcOperateAuto::find(size_t seq1)
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
} // namespace jibiki