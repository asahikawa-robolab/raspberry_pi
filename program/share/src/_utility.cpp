/*-----------------------------------------------
 *
* Last updated : 2020/09/03, 03:15
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

    /* 初期化 */
    void ProcOperateAuto::
        init(ShareVal<bool> &exit_flag,
             ShareVal<bool> &start_flag,
             ShareVal<bool> &reset_flag,
             ShareVal<thread::OperateMethod> &current_method,
             ShareVal<std::string> &execute_orders,
             ShareValVec<std::string> &executing_order,
             std::vector<ModeFunc> mode_func,
             bool is_print,
             std::string json_path)
    {
        /* using 宣言 */
        using picojson::array;
        using picojson::object;

        /* 初期化 */
        m_control_data.m_exit_flag = &exit_flag;
        m_control_data.m_start_flag = &start_flag;
        m_control_data.m_reset_flag = &reset_flag;
        m_control_data.m_current_method = &current_method;
        m_execute_orders = &execute_orders;
        m_executing_order = &executing_order;
        m_mode_func = mode_func;
        m_is_print = is_print;

        /* m_modes に値を設定 */
        picojson::value json_value = load_json_file(json_path);
        array &mode_list_array = json_value
                                     .get<object>()["mode_list"]
                                     .get<array>();
        for (size_t i = 0; i < mode_list_array.size(); ++i)
            m_modes.push_back(mode_list_array[i].get<std::string>());
    }

    /* orders を読み込む */
    void ProcOperateAuto::load(void)
    {
        /* using 宣言 */
        using picojson::array;
        using picojson::object;

        /* json ファイルを picojson で読み込む */
        picojson::value json_value =
            load_json_file("order.json");

        /*-----------------------------------------------
        orders を読み込む
        -----------------------------------------------*/
        array &root_array = json_value
                                .get<object>()[m_execute_orders->read().c_str()]
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

            /* name */
            tmp.m_name = root_array[i]
                             .get<object>()["name"]
                             .get<std::string>();
            /* リストに追加 */
            m_orders.emplace_back(tmp);
        }
    }

    /* order を表示 */
    void ProcOperateAuto::print(void)
    {
        std::cout << "-------------------------------" << std::endl;
        for (size_t i = 0; i < m_orders.size(); ++i)
        {
            printf("%s\n%d, %d, %d : %s\n",
                   m_orders[i].m_name.c_str(),
                   m_orders[i].m_seq[0],
                   m_orders[i].m_seq[1],
                   m_orders[i].m_seq[2],
                   m_orders[i].m_mode.c_str());
            for (size_t j = 0; j < m_orders[i].m_param.size(); ++j)
                std::cout << m_orders[i].m_param[j] << ", " << std::flush;
            printf("\n\n");
        }
        std::cout << "-------------------------------" << std::endl;
    }

    /*-----------------------------------------------
     *
     * Order のリストに従って実行
     *
    -----------------------------------------------*/
    void ProcOperateAuto::execute(void)
    {
        std::cout << "*** orders start ["
                  << m_execute_orders->read() << "] ***" << std::endl;

        for (size_t i = 0; i <= find(); ++i)
        {
            if (!manage_thread_int())
                break;

            size_t branch_num = find(i) + 1;       /* 分岐の数 */
            m_executing_order->resize(branch_num); /* executing_order を準備 */

            /*-----------------------------------------------
            枝の数だけ branch() を実行
            -----------------------------------------------*/
            std::vector<uint8_t> finish_flag(branch_num, 0); /* スレッドの終了フラグ */
            for (size_t j = 0; j < branch_num; ++j)
            {
                if (!manage_thread_int())
                    break;
                std::thread t(&ProcOperateAuto::branch, this,
                              std::ref(finish_flag[j]), i, j);
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
        }

        /* 終了処理 */
        m_executing_order->clear();
        if (m_control_data.m_reset_flag->read())
            std::cout << "*** orders reset ["
                      << m_execute_orders->read() << "] ***" << std::endl;
        else if (m_control_data.m_exit_flag->read())
            std::cout << "*** orders quit ["
                      << m_execute_orders->read() << "] ***" << std::endl;
        else
            std::cout << "*** orders complete ["
                      << m_execute_orders->read() << "] ***" << std::endl;
        m_orders.clear();
    }

    /*-----------------------------------------------
     *
     * 分岐した枝毎の処理
     *
    -----------------------------------------------*/
    void ProcOperateAuto::branch(uint8_t &finish_flag, size_t seq1, size_t seq2)
    {
        try
        {
            /* 要素数を取得 */
            size_t element_num = find(seq1, seq2) + 1;

            /*-----------------------------------------------
            Order を順次実行
            -----------------------------------------------*/
            for (size_t seq3 = 0; seq3 < element_num; ++seq3)
            {
                /* スレッドの終了 */
                if (!(manage_thread_int()))
                    break;

                /* Order のインデックスを取得 */
                size_t order_index = find(seq1, seq2, seq3);

                /* order の mode の index を探す */
                auto itr = std::find(m_modes.begin(),
                                     m_modes.end(),
                                     m_orders[order_index].m_mode);
                if (itr == m_modes.end())
                {
                    std::stringstream sstr;
                    sstr << __PRETTY_FUNCTION__ << std::endl;
                    sstr << "存在しないモード [ " << m_orders[order_index].m_mode
                         << " ] が指定されています" << std::endl;
                    throw sstr.str();
                }
                size_t mode_index = itr - m_modes.begin();

                /* order を実行 */
                size_t seq[] = {seq1, seq2, seq3};
                m_mode_func[mode_index](this,
                                        m_orders[order_index].m_param,
                                        seq);
            }

            /* スレッドが終了したことを通知する */
            finish_flag = 1;
        }
        catch (std::string err)
        {
            std::cout << "*** error ***\n"
                      << err << std::endl;
            *m_control_data.m_exit_flag = true;
            return;
        }
        catch (std::exception &e)
        {
            std::cout << "*** error ***\n"
                      << __PRETTY_FUNCTION__ << "\n"
                      << e.what() << std::endl;
            *m_control_data.m_exit_flag = true;
            return;
        }
    }

    /* 引数に指定した seq と一致する list のインデックスを返す */
    size_t ProcOperateAuto::find(size_t seq1, size_t seq2, size_t seq3)
    {
        for (size_t i = 0; i < m_orders.size(); ++i)
        {
            bool _1 = (m_orders[i].m_seq[0] == seq1);
            bool _2 = (m_orders[i].m_seq[1] == seq2);
            bool _3 = (m_orders[i].m_seq[2] == seq3);
            if (_1 & _2 & _3)
                return i;
        }

        /* １つも見つからなかった場合 */
        std::stringstream sstr;
        sstr << __PRETTY_FUNCTION__ << std::endl
             << "seq1 : " << seq1 << ", seq2 : " << seq2
             << "seq3 : " << seq3 << std::endl;
        throw sstr.str();
    }

    /* 引数で指定した seq において，seq3 の最大値を返す */
    size_t ProcOperateAuto::find(size_t seq1, size_t seq2)
    {
        bool is_found = false;
        size_t max = 0;
        for (size_t i = 0; i < m_orders.size(); ++i)
        {
            bool _1 = (m_orders[i].m_seq[0] == seq1);
            bool _2 = (m_orders[i].m_seq[1] == seq2);
            bool _3 = (m_orders[i].m_seq[2] >= max);
            if (_1 & _2 & _3)
            {
                max = m_orders[i].m_seq[2];
                is_found = true;
            }
        }
        /* １つも見つからなかった場合 */
        if (!is_found)
        {
            std::stringstream sstr;
            sstr << __PRETTY_FUNCTION__ << std::endl
                 << "seq1 : " << seq1 << ", seq2 : " << seq2 << std::endl;
            throw sstr.str();
        }
        return max;
    }

    /* 引数で指定した seq において，seq2 の最大値を返す */
    size_t ProcOperateAuto::find(size_t seq1)
    {
        bool is_found = false;
        size_t max = 0;
        for (size_t i = 0; i < m_orders.size(); ++i)
        {
            bool _1 = (m_orders[i].m_seq[0] == seq1);
            bool _2 = (m_orders[i].m_seq[1] >= max);
            if (_1 & _2)
            {
                max = m_orders[i].m_seq[1];
                is_found = true;
            }
        }
        /* １つも見つからなかった場合 */
        if (!is_found)
        {
            std::stringstream sstr;
            sstr << __PRETTY_FUNCTION__ << std::endl
                 << "seq1 : " << seq1 << std::endl;
            throw sstr.str();
        }

        return max;
    }

    /* seq1 の最大値を取得 */
    size_t ProcOperateAuto::find(void)
    {
        size_t max = 0;
        for (auto i : m_orders)
            if (i.m_seq[0] >= max)
                max = i.m_seq[0];
        return max;
    }

    ProcOperateAuto::
        ProcOperateAuto(ShareVal<bool> &exit_flag,
                        ShareVal<bool> &start_flag,
                        ShareVal<bool> &reset_flag,
                        ShareVal<thread::OperateMethod> &current_method,
                        ShareVal<std::string> &execute_orders,
                        ShareValVec<std::string> &executing_order,
                        std::vector<ModeFunc> mode_func,
                        bool is_print,
                        std::string json_path)
    {
        init(exit_flag, start_flag, reset_flag, current_method,
             execute_orders, executing_order, mode_func, is_print, json_path);

        std::thread t([this] {
            this->launch();
        });
        m_t = std::move(t);
    }

    void ProcOperateAuto::launch(void)
    {
        try
        {
            /* スレッドの実行の管理 */
            if (!thread::enable("operate_auto"))
                return;

            while (manage_thread_int(false))
            {
                if (m_control_data.m_start_flag->read() &
                    !m_control_data.m_reset_flag->read())
                {
                    load();
                    if (m_is_print)
                        print();
                }
                if (!m_orders.empty())
                    execute();
            }
        }
        catch (std::string err)
        {
            std::cout << "*** error ***\n"
                      << err << std::endl;
            *m_control_data.m_exit_flag = true;
            return;
        }
        catch (std::exception &e)
        {
            std::cout << "*** error ***\n"
                      << __PRETTY_FUNCTION__ << "\n"
                      << e.what() << std::endl;
            *m_control_data.m_exit_flag = true;
            return;
        }
    }

    bool ProcOperateAuto::manage_thread_int(bool use_reset_flag)
    {
        bool f1 = thread::manage(*m_control_data.m_exit_flag,
                                 *m_control_data.m_current_method,
                                 thread::OPERATE_AUTO);
        bool f2 = !m_control_data.m_reset_flag->read();

        if (use_reset_flag)
            return f1 & f2;
        else
            return f1;
    }

    void ProcOperateAuto::set_executing_order(size_t seq[], std::string str)
    {
        std::stringstream sstr;
        sstr << "[ " << seq[0] << ", "
             << seq[1] << ", " << seq[2] << " ] " << str;
        m_executing_order->write(seq[1], sstr.str());
    }
} // namespace jibiki