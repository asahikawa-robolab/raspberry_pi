/* Last updated : 2020/10/06, 00:38 */
/* C++ */
#include <vector>
#include <stdexcept>
/* UNIX */
#include <unistd.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* jibiki */
#include "../inc/_serial_communication.hpp"

namespace jibiki
{
	/*-----------------------------------------------
	 *
	 * コンストラクタ
	 *
	-----------------------------------------------*/
	Com::Com(std::string path,
			 size_t size_tx_raw_data,
			 size_t size_rx_raw_data,
			 int brate,
			 std::string name)
		: m_name(name),
		  m_size_tx_raw(size_tx_raw_data),
		  m_size_rx_raw(size_rx_raw_data),
		  m_rx(m_size_rx_raw),
		  m_tx(m_size_tx_raw),
		  m_size_tx_prot(size_tx_raw_data * 2 + 3),
		  m_size_rx_prot(size_rx_raw_data * 2 + 3),
		  m_is_refreshed(false),
		  m_brate(brate),
		  m_path(path),
		  m_time_s_tx(get_time()),
		  m_time_s_rx(get_time()),
		  m_rx_complete_cnt(),
		  m_rx_freq()
	{
		setup();
	}

	/*-----------------------------------------------
	 *
	 * 新プロトコルでデータを送信する
	 *
	-----------------------------------------------*/
	void Com::send(void)
	{
		/* プロトコルの形式に変換 */
		std::vector<uint8_t> buf = protocol_tx(m_tx);

		/* データを送信する */
		serial_write(buf);
	}

	/*-----------------------------------------------
	 *
	 * 新プロトコルでデータを受信する
	 *
	-----------------------------------------------*/
	bool Com::receive(bool debug)
	{
		/* バッファにデータをためる */
		uint8_t tmp;
		if (serial_one_byte_read(&tmp) != true)
			return false;
		m_bufs.emplace_back(tmp);
		if (m_bufs.size() > m_size_rx_prot)
			m_bufs.erase(m_bufs.begin());

		/* デバッグ */
		if (debug)
			printf("[%s]receive buf : 0x%02X\n", m_name.c_str(), tmp);

		/* プロトコルの形式から変換 */
		std::vector<uint8_t> tmps;
		if (m_bufs.size() == m_size_rx_prot)
			tmps = protocol_rx(m_bufs);

		/* プロトコルでエラーが出なかったらデータを更新 */
		if (tmps.empty() != true)
		{
			m_rx = tmps;

			/* 受信完了したらカウントを増やす */
			++m_rx_complete_cnt;

			return true;
		}

		/* tmps.empty() == true のとき */
		return false;
	}

	/*-----------------------------------------------
	 *
	 * 受信周波数を取得する
	 *
	-----------------------------------------------*/
	double Com::get_rx_freq(double sampling_period_ms) noexcept
	{
		double passed_time_ms =
			calc_sec(m_time_s_rx, get_time()) * 1E3;
		if (passed_time_ms > sampling_period_ms)
		{
			/* 受信周波数を計算 */
			m_rx_freq = m_rx_complete_cnt / (passed_time_ms * 1E-3);
			/* 初期化 */
			m_time_s_rx = get_time();
			m_rx_complete_cnt = 0;
		}
		return m_rx_freq;
	}

	/*-----------------------------------------------
	 *
	 * 送信データをプロトコルの形に変換する
	 *
	-----------------------------------------------*/
	std::vector<uint8_t> Com::protocol_tx(std::vector<uint8_t> in) const
	{
		std::vector<uint8_t> out(m_size_tx_prot);

		/* 上位ビット */
		out[0] = 'S' & 0xF0;
		for (size_t i = 0; i < m_size_tx_raw; ++i)
		{
			out[2 * i + 1] = in[i] & 0xF0;
			out[2 * i + 2] = (in[i] << 4) & 0xF0;
		}
		out[m_size_tx_prot - 2] = ('C' << 4) & 0xF0;
		out[m_size_tx_prot - 1] = 'C' & 0xF0;

		/* 下位ビット */
		out[0] |= 'S' & 0x0F;
		out[1] |= ('S' >> 4) & 0x0F;
		for (size_t i = 0; i < m_size_tx_raw; ++i)
		{
			out[2 * i + 2] |= (~in[i] >> 4) & 0x0F;
			out[2 * i + 3] |= ~in[i] & 0x0F;
		}
		out[m_size_tx_prot - 1] |= 'C' & 0x0F;

		return out;
	}

	/*-----------------------------------------------
	 *
	 * プロトコルの型のデータを受信データに変換する
	 * 例外：std::string
	 *
	-----------------------------------------------*/
	std::vector<uint8_t> Com::protocol_rx(std::vector<uint8_t> in) const
	{
		/* エラーチェック */
		if (in.size() != m_size_rx_prot)
		{
			std::stringstream sstr;
			sstr << "引数の要素数が不適切です [" << in.size() << "]";
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}

		/* プロトコルのエラーチェック */
		/* 一か所でもデータがおかしかったら flag_ok を false にする */
		bool flag_ok = true;
		if (in[0] != 'S')
		{
			flag_ok = false;
		}
		else if ((in[1] & 0x0F) != (('S' >> 4) & 0x0F))
		{
			flag_ok = false;
		}
		else if (((in[m_size_rx_prot - 2] >> 4) & 0x0F) != ('C' & 0x0F))
		{
			flag_ok = false;
		}
		else if (in[m_size_rx_prot - 1] != 'C')
		{
			flag_ok = false;
		}
		else
		{
			for (size_t i = 0; i < m_size_rx_raw * 2; ++i)
			{
				if (((in[i + 1] >> 4) & 0x0F) != (~(in[i + 2]) & 0x0F))
					flag_ok = false;
			}
		}

		/* データを組み直す */
		std::vector<uint8_t> out;
		if (flag_ok == true)
		{
			for (size_t i = 0; i < m_size_rx_raw; ++i)
			{
				uint8_t tmp = (in[2 * i + 1] & 0xF0) | ((in[2 * i + 2] >> 4) & 0x0F);
				out.emplace_back(tmp);
			}
		}
		else
		{
			out.clear();
		}
		return out;
	}

	/*-----------------------------------------------
	 *
	 * ポートを開く
	 *
	-----------------------------------------------*/
	void Com::setup(void)
	{
		m_fd = open(m_path.c_str(), O_RDWR);
		if (m_fd < 0)
		{
			std::stringstream sstr;
			sstr << "open err, [" << m_name << "] " << m_path;
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}

		/*-----------------------------------------------
		シリアル通信設定
		-----------------------------------------------*/
		struct termios term;

		cfmakeraw(&term);		/* RAW モード */
		term.c_cflag |= CREAD;	/* 受信有効 */
		term.c_cflag |= CLOCAL; /* 接続が確立していなくてもブロックしない */

		/* 1byte あたりの bit 数を 8bit に */
		term.c_cflag &= ~CSIZE;
		term.c_cflag |= CS8;

		/* ボーレートを設定 */
		cfsetispeed(&term, m_brate); /* 入力ボーレート */
		cfsetospeed(&term, m_brate); /* 出力ボーレート */

		tcsetattr(m_fd, TCSANOW, &term); /* 設定を反映 */
	}

	/*-----------------------------------------------
	 *
	 * ポートを閉じる
	 *
	-----------------------------------------------*/
	void Com::destroy(void) const noexcept
	{
		close(m_fd);
	}

	/*-----------------------------------------------
	 *
	 * メンバのポートから 1 バイトのデータを受信する
	 *
	-----------------------------------------------*/
	bool Com::serial_one_byte_read(uint8_t *data) const noexcept
	{
		/*-----------------------------------------------
		select に必要な変数を用意
		-----------------------------------------------*/
		/* 記述子セット */
		fd_set readfds;
		FD_ZERO(&readfds);		/* 初期化 */
		FD_SET(m_fd, &readfds); /* m_fd を readfds に登録 */
		/* タイムアウト */
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		/*-----------------------------------------------
		read が可能か確認
		-----------------------------------------------*/
		select(m_fd + 1, &readfds, NULL, NULL, &tv);

		/* read 可能 */
		if (FD_ISSET(m_fd, &readfds) != 0)
		{
			read(m_fd, data, 1); /* データを読み出す */
			return true;
		}
		/* read 不可能 */
		else
		{
			return false;
		}
	}

	/*-----------------------------------------------
	 *
	 * メンバのポートに引数で指定されたデータを送信する
	 *
	-----------------------------------------------*/
	void Com::serial_write(std::vector<uint8_t> data)
	{
		/* バイト数 * 1ms の間隔をあけて送信 */
		if (calc_sec(m_time_s_tx, get_time()) > m_size_tx_prot * 1E-3)
		{
			if (check_write() == true)
			{
				write(m_fd, data.data(), data.size());
				m_time_s_tx = get_time();
			}
		}
	}

	/*-----------------------------------------------
	 *
	 * write() が可能か select を用いて判断する
	 *
	-----------------------------------------------*/
	bool Com::check_write(void)
	{
		/*-----------------------------------------------
		select に必要な変数を用意
		-----------------------------------------------*/
		/* 記述子セット */
		fd_set writefds;
		FD_ZERO(&writefds);		 /* 初期化 */
		FD_SET(m_fd, &writefds); /* m_fd を writefds に登録 */
		/* タイムアウト */
		struct timeval tv;
		tv.tv_sec = 0;
		tv.tv_usec = 0;

		/*-----------------------------------------------
		write が可能か確認
		-----------------------------------------------*/
		select(m_fd + 1, NULL, &writefds, NULL, &tv);

		/* write 可能 */
		if (FD_ISSET(m_fd, &writefds) != 0)
		{
			m_is_refreshed = false;
			return true;
		}
		/* write 不可能 */
		else
		{
			setup(); /* リフレッシュ */
			if (m_is_refreshed == false)
			{
				m_is_refreshed = true;
				printf("\t\trefreshed.(%s)\n", m_name.c_str());
			}
			return false;
		}
	}
	/*-----------------------------------------------
	*
	* コンストラクタ
	*
	-----------------------------------------------*/
	ParamCom::ParamCom(std::string path,
					   size_t size_tx_raw_data,
					   size_t size_rx_raw_data,
					   int brate,
					   std::string name,
					   bool param_set,
					   std::string json_path)
		: Com(path,
			  size_tx_raw_data,
			  size_rx_raw_data,
			  brate,
			  name),
		  m_cur_param_index(0),
		  m_param_set(param_set)

	{
		/* パラメータ設定 */
		if (param_set)
		{
			load_json(json_path);
			set_param();
		}
		printf("ParamCom(%s) was constructed.\n", m_name.c_str());
	}
	/*-----------------------------------------------
	 *
	 * JSON ファイルからパラメータを読み込む
	 *
	-----------------------------------------------*/
	void ParamCom::load_json(std::string json_path)
	{
		/* using 宣言 */
		using picojson::array;
		using picojson::object;

		/* エラーチェック */
		if (m_size_tx_raw < m_PARAM_UNIT_NUM * 2 + 1 || m_size_rx_raw < 1)
		{
			std::stringstream sstr;
			sstr << "引数の値が不適切です． (tx : " << m_size_tx_raw
				 << ", rx : " << m_size_rx_raw << ")\n"
				 << "tx >= " << m_PARAM_UNIT_NUM * 2 + 1
				 << " && rx >= 1 を満たさなければならない．";
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}

		try
		{
			/* json ファイルを読み込む準備 */
			picojson::value json_val = load_json_file(json_path);
			array &param_array = json_val.get<object>()[m_name]
									 .get<array>();

			for (const auto &i : param_array)
			{
				/* エラーチェック */
				if (i.get<array>().size() != 3)
				{
					std::stringstream sstr;
					sstr << json_path << " の書式が不適切です．";
					throw sstr.str(); /* エラー発生 */
				}
				/* パラメータを読み込む */
				m_param.emplace_back((int16_t)(i.get<array>()[1].get<double>()));
				m_param.emplace_back((int16_t)(i.get<array>()[2].get<double>()));
			}
		}
		catch (const std::exception &e)
		{
			/* param_array の初期化時のエラー */
			std::stringstream sstr;
			sstr << json_path << " に " << m_name
				 << "のパラメータの項目がありません．";
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}
		catch (std::string e)
		{
			jibiki::print_err(__PRETTY_FUNCTION__, e);
			throw std::runtime_error(""); /* エラー発生 */
		}
	}
	/*-----------------------------------------------
	 *
	 * スレーブのパラメータを設定する
	 *
	-----------------------------------------------*/
	void ParamCom::set_param(void)
	{
		while (1)
		{
			/* 送信データにパラメータをセットする */
			m_tx[0] = m_cur_param_index + 1;
			m_tx[1] = up(m_param[m_cur_param_index * m_PARAM_UNIT_NUM]);
			m_tx[2] = low(m_param[m_cur_param_index * m_PARAM_UNIT_NUM]);
			m_tx[3] = up(m_param[m_cur_param_index * m_PARAM_UNIT_NUM + 1]);
			m_tx[4] = low(m_param[m_cur_param_index * m_PARAM_UNIT_NUM + 1]);
			send();

			/* 設定の完了を待つ */
			if (receive() && m_rx[0] == m_cur_param_index + 1)
			{
				printf("[%s] %d / %d\t: [%d, %d]\n",
					   m_name.c_str(), m_cur_param_index + 1, m_param.size() / 2,
					   m_param[m_cur_param_index * m_PARAM_UNIT_NUM],
					   m_param[m_cur_param_index * m_PARAM_UNIT_NUM + 1]);
				++m_cur_param_index;
			}

			/* 終了処理 */
			if (m_param.size() / 2 <= m_cur_param_index)
			{
				printf("set param complete.\n");
				m_tx[0] = 0xFF;
				break;
			}
		}
	}
	/*-----------------------------------------------
	 *
	 * 送信データを設定する
	 *
	-----------------------------------------------*/
	uint8_t &ParamCom::tx(size_t index)
	{
		if (index == 0 && m_param_set == true)
		{
			std::stringstream sstr;
			sstr << "\"ParamCom::tx(0)\" は変更しないでください";
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}
		return m_tx.at(index);
	}

	/*-----------------------------------------------
	 *
	 * コンストラクタ
	 *
	-----------------------------------------------*/
	ProcParamCom::ProcParamCom(ShareVar<bool> &exit_flag,
							   std::vector<ProcParamCom::ComFunc> com_func,
							   std::string json_path)
		: m_exit_flag(&exit_flag),
		  m_com_func(com_func),
		  m_json_path(json_path)
	{
		std::thread t([this] {
			this->launch();
		});
		m_t = std::move(t);
	}

	void ProcParamCom::launch(void)
	{
		try
		{
			/* スレッドの実行の管理 */
			if (!thread::enable("com"))
				return;

			/* 設定ファイル読み込み */
			load_json();

			/* ComFunc を順番に実行する */
			while (thread::manage(*m_exit_flag))
				for (size_t i = 0; i < m_com_func.size(); ++i)
					if (m_enable[i])
						m_com_func[i](m_path[i], m_name[i]);
		}
		catch (const std::exception &e)
		{
			print_err(__PRETTY_FUNCTION__);
			*m_exit_flag = true;
			return; /* 最上部 */
		}
	}

	/* 設定ファイル読み込み */
	void ProcParamCom::load_json(void)
	{
		/* using 宣言 */
		using picojson::array;
		using picojson::object;

		size_t com_num; /* Com の個数 */

		/*-----------------------------------------------
		値を読み込む
		-----------------------------------------------*/
		try
		{
			/* JSON ファイルを開く */
			picojson::value json_val = load_json_file(m_json_path);
			array &root_array = json_val
									.get<object>()["com"]
									.get<array>();
			com_num = root_array.size();

			for (const auto &i : root_array)
			{
				array com_array = i.get<array>();
				/* エラーチェック */
				if (com_array.size() != 3)
					throw std::runtime_error(""); /* エラー発生 */
				/* name, enable, path を読み込む */
				m_name.emplace_back(com_array[0].get<std::string>());
				m_enable.emplace_back(com_array[1].get<bool>());
				m_path.emplace_back(com_array[2].get<std::string>());
			}
		}
		catch (const std::exception &e)
		{
			std::stringstream sstr;
			sstr << m_json_path << " 中の com の書式が不適切です．";
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}

		/*-----------------------------------------------
		com_func の要素数の確認
		-----------------------------------------------*/
		/* TODO : 動作確認 */
		if (com_num != m_com_func.size())
		{
			std::stringstream sstr;
			sstr << "jibiki::ProcParamCom::ProcParamCom() "
				 << "の引数である com_func の要素数と\n"
				 << m_json_path << " 中の com の要素数が一致しません．";
			jibiki::print_err(__PRETTY_FUNCTION__, sstr.str());
			throw std::runtime_error(""); /* エラー発生 */
		}
	}

} // namespace jibiki