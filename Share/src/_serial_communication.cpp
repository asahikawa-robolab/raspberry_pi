/*-----------------------------------------------
 *
 * Last updated : 2020/02/05, 21:17
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#include <stdlib.h>
#include <vector>
#include <sys/select.h>
#include "../inc/_serial_communication.hpp"

namespace jibiki
{

/*-----------------------------------------------
 *
 * 初期化
 *
-----------------------------------------------*/
COM::COM(std::string path,
		 size_t size_tx_raw_data,
		 size_t size_rx_raw_data,
		 int brate,
		 std::string name,
		 bool param_set)
{
	/* メンバ変数を設定 */
	m_is_refreshed = false;
	m_path = path;
	m_size_tx_raw = size_tx_raw_data;
	m_size_tx_prot = size_tx_raw_data * 2 + 3;
	m_tx.resize(m_size_tx_raw);
	m_size_rx_raw = size_rx_raw_data;
	m_size_rx_prot = size_rx_raw_data * 2 + 3;
	m_rx.resize(m_size_rx_raw);
	m_brate = brate;
	m_name = name;

	/* 初期化 */
	m_time_s_tx = get_time();
	m_time_s_rx = get_time();
	m_is_set_param_finished = !param_set;
	m_is_first_set_param = true;
	m_cur_param_index = 0;

	/* 初期化 */
	setup();

	printf("COM was constructed.(%s)\n", m_name.c_str());
}
/*-----------------------------------------------
 *
 * 終了処理
 *
-----------------------------------------------*/
COM::~COM()
{
	destroy();
}

/*-----------------------------------------------
 *
 * 新プロトコルでデータを送信する
 *
-----------------------------------------------*/
void COM::core_send(void)
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
bool COM::core_receive(bool debug)
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
double COM::get_rx_freq(void)
{
	const size_t wait_cnt = 200;

	++m_execute_cnt;

	if (m_execute_cnt >= wait_cnt)
	{
		/* 受信周波数を計算 */
		double passed_time = jibiki::CALC_SEC(m_time_s_rx, get_time());
		m_rx_freq = 1 / (passed_time / m_rx_complete_cnt);

		/* 初期化 */
		m_time_s_rx = get_time();
		m_rx_complete_cnt = 0;
		m_execute_cnt = 0;
	}
	return m_rx_freq;
}

/*-----------------------------------------------
 *
 * 送信データをプロトコルの形に変換する
 *
-----------------------------------------------*/
std::vector<uint8_t> COM::protocol_tx(std::vector<uint8_t> in)
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
std::vector<uint8_t> COM::protocol_rx(std::vector<uint8_t> in)
{
	/* エラーチェック */
	if (in.size() < m_size_rx_prot)
	{
		std::stringstream sstr;
		sstr << __PRETTY_FUNCTION__ << std::endl;
		sstr << "invalid vector size [" << in.size() << "]\n";
		throw sstr.str();
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
 * 例外：std::string
 *
-----------------------------------------------*/
void COM::setup(void)
{
	m_fd = open(m_path.c_str(), O_RDWR);
	if (m_fd < 0)
	{
		std::stringstream sstr;
		sstr << __PRETTY_FUNCTION__ << std::endl;
		sstr << "open err, [" << m_name << "] " << m_path << std::endl;
		throw sstr.str();
	}

	/*-----------------------------------------------
	シリアル通信設定
	-----------------------------------------------*/
	struct termios term;

	cfmakeraw(&term);		/* RAW モード */
	term.c_cflag |= CREAD;  /* 受信有効 */
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
void COM::destroy(void)
{
	close(m_fd);
}

/*-----------------------------------------------
 *
 * メンバのポートから 1 バイトのデータを受信する
 *
-----------------------------------------------*/
bool COM::serial_one_byte_read(uint8_t *data)
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
void COM::serial_write(std::vector<uint8_t> data)
{
	/* バイト数 * 1ms の間隔をあけて送信 */
	if (jibiki::CALC_SEC(m_time_s_tx, get_time()) > m_size_tx_prot * 1E-3)
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
bool COM::check_write(void)
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
 * スレーブのパラメータを設定する
 * 例外：std::string
 *
-----------------------------------------------*/
void COM::set_param(void)
{
	/* パラメータ設定の処理が終了していたら即終了 */
	if (m_is_set_param_finished)
		return;

	/* 初回のみ json ファイルからパラメータを読み込む */
	if (m_is_first_set_param)
	{
		init_set_param();
		m_is_first_set_param = false;
	}

	/* パラメータを設定する */
	m_tx[0] = m_cur_param_index + 1;
	m_tx[1] = jibiki::UP(m_param[m_cur_param_index * PARAM_UNIT_NUM]);
	m_tx[2] = jibiki::LOW(m_param[m_cur_param_index * PARAM_UNIT_NUM]);
	m_tx[3] = jibiki::UP(m_param[m_cur_param_index * PARAM_UNIT_NUM + 1]);
	m_tx[4] = jibiki::LOW(m_param[m_cur_param_index * PARAM_UNIT_NUM + 1]);
	core_send();

	/* 設定の完了を待つ */
	if (core_receive() && m_rx[0] == m_cur_param_index + 1)
	{
		printf("[%s] %d / %d\t: [%d, %d]\n",
			   m_name.c_str(), m_cur_param_index + 1, m_param.size() / 2,
			   m_param[m_cur_param_index * PARAM_UNIT_NUM],
			   m_param[m_cur_param_index * PARAM_UNIT_NUM + 1]);
		++m_cur_param_index;
	}

	/* 終了処理 */
	if (m_cur_param_index + 1 > m_param.size() / 2)
	{
		printf("set param complete.\n");
		m_tx[0] = 0xFF;
		m_is_set_param_finished = true;
	}
}

/*-----------------------------------------------
 *
 * COM::set_param の初回処理
 *
-----------------------------------------------*/
void COM::init_set_param(void)
{
	/* using 宣言 */
	using picojson::array;
	using picojson::object;

	/* エラーチェック */
	if (m_size_tx_raw < PARAM_UNIT_NUM + 2 + 1 || m_size_rx_raw < 1)
	{
		std::stringstream sstr;
		sstr << __PRETTY_FUNCTION__ << std::endl;
		sstr << "invalid size (tx : " << m_size_tx_raw
			 << ", rx : " << m_size_rx_raw << ")" << std::endl;
		sstr << "require tx >= " << PARAM_UNIT_NUM * 2 + 1 << " && rx >= 1" << std::endl;
		throw sstr.str();
	}

	/* json ファイルから読み込む */
	picojson::value json_value = jibiki::load_json_file("param.json");
	array &param_array = json_value.get<object>()[m_name]
							 .get<array>();

	/* パラメータを抽出 */
	for (size_t i = 0; i < param_array.size(); ++i)
	{
		/* エラーチェック */
		if (param_array[i].get<array>().size() != 3)
		{
			std::stringstream sstr;
			sstr << __PRETTY_FUNCTION__ << std::endl;
			sstr << "パラメータ一つにつき，３つずつ値を持たせてください．( 現在 "
				 << param_array[i].get<array>().size() << " 個 )" << std::endl;
			throw sstr.str();
		}
		m_param.push_back((int16_t)(param_array[i].get<array>()[1].get<double>()));
		m_param.push_back((int16_t)(param_array[i].get<array>()[2].get<double>()));
	}

	/* エラーチェック */
	if (m_param.size() % 2 != 0)
	{
		std::stringstream sstr;
		sstr << __PRETTY_FUNCTION__ << std::endl;
		sstr << "パラメータの個数は偶数にしてください ("
			 << m_param.size() << " 個)" << std::endl;
		throw sstr.str();
	}
}

} // namespace jibiki