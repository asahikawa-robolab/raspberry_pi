/* Last updated : 2020/10/05, 19:36 */
#include <sstream>
#include <iostream>
#include <thread>
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_thread.hpp"
#include "../../share/inc/_utility.hpp"

namespace jibiki
{
    /*-----------------------------------------------
    *
    * ProcOperateAuto コンストラクタ
    *
    -----------------------------------------------*/
    ProcOperateAuto::
        ProcOperateAuto(ShareVar<bool> &exit_flag,
                        ShareVar<bool> &start_flag,
                        ShareVar<bool> &reset_flag,
                        ShareVar<thread::OperateMethod> &current_method,
                        ShareVar<std::string> &execute_orders,
                        ShareVarVec<std::string> &executing_order,
                        std::vector<ModeFunc> mode_func,
                        bool is_print,
                        std::string json_path)
        : m_control_data{&exit_flag,
                         &start_flag,
                         &reset_flag,
                         &current_method},
          m_mode_func(mode_func),
          m_executing_order(&executing_order),
          m_execute_orders(&execute_orders),
          m_is_print(is_print),
          m_json_path(json_path)
    {
        try
        {
            load_json();
            std::thread t([this] {
                this->launch();
            });
            m_t = std::move(t);
        }
        catch (const std::exception &e)
        {
            jibiki::print_err(__PRETTY_FUNCTION__);
            throw; /* 仲介 */
        }
    }
    /*-----------------------------------------------
    *
    * 初期化
    *
    -----------------------------------------------*/
    void ProcOperateAuto::load_json(void)
    {
        /* using 宣言 */
        using picojson::array;
        using picojson::object;

        /*-----------------------------------------------
        値を読み込む
        -----------------------------------------------*/
        try
        {
            picojson::value json_val = load_json_file(m_json_path);
            array &mode_list_array = json_val
                                         .get<object>()["mode_list"]
                                         .get<array>();
            for (const auto &i : mode_list_array)
                m_modes.emplace_back(i.get<std::string>());
        }
        catch (const std::exception &e)
        {
            std::stringstream sstr;
            sstr << m_json_path << " 中の mode_list の書式が不適切です．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(""); /* エラー発生 */
        }

        /*-----------------------------------------------
		mode_func の要素数の確認
		-----------------------------------------------*/
        if (m_mode_func.size() != m_modes.size())
        {
            std::stringstream sstr;
            sstr << "jibiki::ProcOperateAuto::ProcOperateAuto() "
                 << "の引数である mode_func の要素数と\n"
                 << m_json_path << " 中の mode_list の要素数が一致しません．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(""); /* エラー発生 */
        }
    }
    /*-----------------------------------------------
    *
    * スレッド内部で実行する処理
    *
    -----------------------------------------------*/
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
                    load_orders();
                    if (m_is_print)
                        print();
                }
                if (!m_orders.empty())
                    execute();
            }
        }
        catch (const std::exception &e)
        {
            jibiki::print_err(__PRETTY_FUNCTION__);
            *m_control_data.m_exit_flag = true;
            return; /* 最上部 */
        }
    }
    /*-----------------------------------------------
    *
    * orders を読み込む
    *
    -----------------------------------------------*/
    void ProcOperateAuto::load_orders(void)
    {
        /* using 宣言 */
        using picojson::array;
        using picojson::object;
        /*-----------------------------------------------
        orders を読み込む
        -----------------------------------------------*/
        try
        {
            picojson::value json_val = load_json_file(m_json_path);
            array &root_array =
                json_val.get<object>()[m_execute_orders->read().c_str()].get<array>();

            for (auto &i : root_array)
            {
                Order tmp;
                /* seq */
                for (size_t j = 0; j < 3; ++j)
                    tmp.m_seq[j] = (size_t)i.get<object>()["seq"]
                                       .get<array>()[j]
                                       .get<double>();
                /* mode */
                tmp.m_mode = i.get<object>()["mode"].get<std::string>();
                /* param */
                array &param_array = i.get<object>()["param"].get<array>();
                for (const auto &j : param_array)
                    tmp.m_param.emplace_back(j.get<std::string>());
                /* name */
                tmp.m_name = i.get<object>()["name"].get<std::string>();
                /* リストに追加 */
                m_orders.emplace_back(tmp);
            }
        }
        catch (const std::exception &e)
        {
            std::stringstream sstr;
            sstr << m_json_path << " の書式が不適切です．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(""); /* エラー発生 */
        }
        /*-----------------------------------------------
        seq のエラーチェック
        -----------------------------------------------*/
        check_duplication(); /* 重複を確認 */
        check_ordering();    /* 順序性を確認 */
    }
    /*-----------------------------------------------
    *
    * 重複を確認
    *
    -----------------------------------------------*/
    void ProcOperateAuto::check_duplication(void)
    {
        for (size_t i = 0; i < m_orders.size(); ++i)
        {
            for (size_t j = i + 1; j < m_orders.size(); ++j)
            {
                bool c1 = m_orders[i].m_seq[0] == m_orders[j].m_seq[0];
                bool c2 = m_orders[i].m_seq[1] == m_orders[j].m_seq[1];
                bool c3 = m_orders[i].m_seq[2] == m_orders[j].m_seq[2];
                if (c1 & c2 & c3)
                {
                    std::stringstream sstr;
                    sstr << m_json_path << " 中の " << m_execute_orders->read()
                         << " で seq が重複しています．[ "
                         << m_orders[i].m_seq[0] << ", "
                         << m_orders[i].m_seq[1] << ", "
                         << m_orders[i].m_seq[2] << " ]";
                    jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
                    throw std::runtime_error(""); /* エラー発生 */
                }
            }
        }
    }
    /*-----------------------------------------------
    *
    * 順序性を確認
    *
    -----------------------------------------------*/
    void ProcOperateAuto::check_ordering(void)
    {
        try
        {
            for (size_t seq1 = 0; seq1 <= find(); ++seq1)
            {
                size_t branch_num = find(seq1) + 1; /* 分岐の数 */
                for (size_t seq2 = 0; seq2 < branch_num; ++seq2)
                {
                    size_t element_num = find(seq1, seq2) + 1;
                    for (size_t seq3 = 0; seq3 < element_num; ++seq3)
                        find(seq1, seq2, seq3);
                }
            }
        }
        catch (const std::exception &e)
        {
            std::stringstream sstr;
            sstr << m_json_path << " 中の " << m_execute_orders->read()
                 << " で抜けている seq があります．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(""); /* エラー発生 */
        }
    }
    /*-----------------------------------------------
    *
    * order を表示
    *
    -----------------------------------------------*/
    void ProcOperateAuto::print(void) const noexcept
    {
        std::cout << "-------------------------------\n";
        for (const auto &i : m_orders)
        {
            printf("%s\n%d, %d, %d : %s\n",
                   i.m_name.c_str(),
                   i.m_seq[0], i.m_seq[1], i.m_seq[2],
                   i.m_mode.c_str());
            for (const auto &j : i.m_param)
                std::cout << j << ", " << std::flush;
            printf("\n\n");
        }
        std::cout << "-------------------------------\n";
    }
    /*-----------------------------------------------
    *
    * Order のリストに従って実行
    *
    -----------------------------------------------*/
    void ProcOperateAuto::execute(void)
    {
        std::cout << "orders start [" << m_execute_orders->read() << "]\n";
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
                std::thread t(&ProcOperateAuto::branch,
                              this,
                              std::ref(finish_flag[j]),
                              i,
                              j);
                t.detach();
            }
            /*-----------------------------------------------
            全てのスレッドの処理が終了するまで待機
            -----------------------------------------------*/
            while (manage_thread_int())
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
            std::cout << "orders reset [" << m_execute_orders->read() << "]\n";
        else if (m_control_data.m_exit_flag->read())
            std::cout << "orders quit [" << m_execute_orders->read() << "]\n";
        else
            std::cout << "orders complete [" << m_execute_orders->read() << "]\n";
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
                    sstr << m_json_path << " の mode_list に"
                         << "存在しないモードが指定されています．\n"
                         << "seq : [ " << seq1 << ", " << seq2 << ", "
                         << seq3 << " ], mode : "
                         << m_orders[order_index].m_mode;
                    jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
                    throw std::runtime_error(""); /* エラー発生 */
                }

                /*-----------------------------------------------
                order を実行
                -----------------------------------------------*/
                size_t seq[] = {seq1, seq2, seq3};
                try
                {
                    /* 開始 */
                    std::cout << "[ " << seq1 << ", " << seq2 << ", " << seq3
                              << " ] start" << std::endl;

                    /* 実行 */
                    size_t mode_index = itr - m_modes.begin();
                    m_mode_func[mode_index](this,
                                            m_orders[order_index].m_param,
                                            seq);

                    /* 終了 */
                    std::cout << "\t\t\t[ " << seq1 << ", " << seq2 << ", "
                              << seq3 << " ] finish" << std::endl;
                }
                catch (const std::exception &e)
                {
                    std::stringstream sstr;
                    sstr << "ModeFunc 中で例外が発生．"
                         << "seq : [ " << seq[0] << ", " << seq[1] << ", "
                         << seq[2] << " ]";
                    jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
                    throw std::runtime_error(""); /* エラー発生 */
                }
            }
            /* スレッドが終了したことを通知する */
            finish_flag = 1;
        }
        catch (const std::exception &e)
        {
            jibiki::print_err(__PRETTY_FUNCTION__);
            *m_control_data.m_exit_flag = true;
            return; /* 最上部 */
        }
    }
    /*-----------------------------------------------
    *
    * 引数に指定した seq と一致する list のインデックスを返す
    *
    -----------------------------------------------*/
    size_t ProcOperateAuto::find(size_t seq1, size_t seq2, size_t seq3) const
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
        sstr << "seq [ " << seq1 << ", " << seq2 << ", " << seq3 << " ] "
             << "が見つかりませんでした．";
        jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
        throw std::runtime_error(sstr.str()); /* エラー発生 */
        return 0;                             /* warning 対策 */
    }
    /*-----------------------------------------------
    *
    * 引数で指定した seq において，seq3 の最大値を返す
    *
    -----------------------------------------------*/
    size_t ProcOperateAuto::find(size_t seq1, size_t seq2) const
    {
        bool is_found = false;
        size_t max = 0;
        for (const auto &i : m_orders)
        {
            bool _1 = (i.m_seq[0] == seq1);
            bool _2 = (i.m_seq[1] == seq2);
            bool _3 = (i.m_seq[2] >= max);
            if (_1 & _2 & _3)
            {
                max = i.m_seq[2];
                is_found = true;
            }
        }
        /* １つも見つからなかった場合 */
        if (!is_found)
        {
            std::stringstream sstr;
            sstr << "seq [ " << seq1 << ", " << seq2 << ", * ] "
                 << "が見つかりませんでした．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(sstr.str()); /* エラー発生 */
        }
        return max;
    }
    /*-----------------------------------------------
    *
    * 引数で指定した seq において，seq2 の最大値を返す
    *
    -----------------------------------------------*/
    size_t ProcOperateAuto::find(size_t seq1) const
    {
        bool is_found = false;
        size_t max = 0;

        for (const auto &i : m_orders)
        {
            bool _1 = (i.m_seq[0] == seq1);
            bool _2 = (i.m_seq[1] >= max);
            if (_1 & _2)
            {
                max = i.m_seq[1];
                is_found = true;
            }
        }
        /* １つも見つからなかった場合 */
        if (!is_found)
        {
            std::stringstream sstr;
            sstr << "seq [ " << seq1 << ", *, * ] が見つかりませんでした．";
            jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
            throw std::runtime_error(sstr.str()); /* エラー発生 */
        }
        return max;
    }
    /*-----------------------------------------------
    *
    * seq1 の最大値を取得
    *
    -----------------------------------------------*/
    size_t ProcOperateAuto::find(void) const noexcept
    {
        size_t max = 0;
        for (const auto &i : m_orders)
            if (i.m_seq[0] >= max)
                max = i.m_seq[0];
        return max;
    }
    /*-----------------------------------------------
    *
    * jibiki::thread::manage を内部で呼び出す
    *
    -----------------------------------------------*/
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
    /*-----------------------------------------------
    *
    * executing_order に文字列を設定する
    *
    -----------------------------------------------*/
    void ProcOperateAuto::set_executing_order(size_t seq[], std::string str)
    {
        std::stringstream sstr;
        sstr << "[ " << seq[0] << ", "
             << seq[1] << ", " << seq[2] << " ] " << str;
        m_executing_order->write(seq[1], sstr.str());
    }
    /*-----------------------------------------------
    *
    * executing_order をクリア（初期化）する
    *
    -----------------------------------------------*/
    void ProcOperateAuto::clear_executing_order(size_t seq[])
    {
        m_executing_order->write(seq[1], "");
    }
} // namespace jibiki