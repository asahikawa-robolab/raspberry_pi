#include <stdlib.h>
#include <string.h>
#include <vector>
#include <sys/select.h>
#include "../inc/_SerialCommunication.hpp"
#include "../inc/_StdFunc.hpp"

/*-----------------------------------------------
 *
 * 初期化
 *
-----------------------------------------------*/
COM::COM(const char *path0, size_t SizeTxRawData0, size_t SizeRxRawData0, int brate0, const char *name0)
{
	/* メンバ変数を設定 */
	refreshed = false;
	strcpy(path, path0);
	size_tx_raw = SizeTxRawData0;
	size_tx_prot = SizeTxRawData0 * 2 + 3;
	tx.resize(size_tx_raw);
	std::fill(tx.begin(), tx.end(), 0);
	size_rx_raw = SizeRxRawData0;
	size_rx_prot = SizeRxRawData0 * 2 + 3;
	rx.resize(size_rx_raw);
	std::fill(rx.begin(), rx.end(), 0);
	brate = brate0;
	strcpy(name, name0);

	/* 送信周期調整用 */
	timespec_get(&time_s, TIME_UTC);

	/* 初期化 */
	setup();

	// printf("COM was constructed.(%s)\n", name);
}

/*-----------------------------------------------
 *
 * 終了処理
 *
-----------------------------------------------*/
COM::~COM()
{
	destroy();
	printf("\tCOM was destructed.(%s)\n", name);
}

/*-----------------------------------------------
 *
 * 新プロトコルでデータを送信する
 *
-----------------------------------------------*/
void COM::send(void)
{
	/* プロトコルの形式に変換 */
	std::vector<uint8_t> buf = protocol_tx(tx);

	/* データを送信する */
	serial_write(buf);
}

/*-----------------------------------------------
 *
 * 新プロトコルでデータを受信する
 *
-----------------------------------------------*/
bool COM::receive(void)
{
	/* バッファにデータをためる */
	uint8_t tmp;
	if (serial_one_byte_read(&tmp) != true)
	{
		return renew;
	}
	bufs.emplace_back(tmp);
	if (bufs.size() > size_rx_prot)
	{
		bufs.erase(bufs.begin());
	}

	/* プロトコルの形式から変換 */
	std::vector<uint8_t> tmps;
	if (bufs.size() == size_rx_prot)
	{
		tmps = protocol_rx(bufs);
	}

	/* プロトコルでエラーが出なかったらデータを更新 */
	if (tmps.empty() != true)
	{
		rx = tmps;
		renew = true;
	}

	return renew;
}

/*-----------------------------------------------
 *
 * 送信データをプロトコルの形に変換する
 *
-----------------------------------------------*/
std::vector<uint8_t> COM::protocol_tx(std::vector<uint8_t> in)
{
	std::vector<uint8_t> out(size_tx_prot);

	/* 上位ビット */
	out[0] = 'S' & 0xF0;
	for (size_t i = 0; i < size_tx_raw; i++)
	{
		out[2 * i + 1] = in[i] & 0xF0;
		out[2 * i + 2] = (in[i] << 4) & 0xF0;
	}
	out[size_tx_prot - 2] = ('C' << 4) & 0xF0;
	out[size_tx_prot - 1] = 'C' & 0xF0;

	/* 下位ビット */
	out[0] |= 'S' & 0x0F;
	out[1] |= ('S' >> 4) & 0x0F;
	for (size_t i = 0; i < size_tx_raw; i++)
	{
		out[2 * i + 2] |= (~in[i] >> 4) & 0x0F;
		out[2 * i + 3] |= ~in[i] & 0x0F;
	}
	out[size_tx_prot - 1] |= 'C' & 0x0F;

	return out;
}

/*-----------------------------------------------
 *
 * プロトコルの型のデータを受信データに変換する
 *
-----------------------------------------------*/
std::vector<uint8_t> COM::protocol_rx(std::vector<uint8_t> in)
{
	/* エラーチェック */
	if (in.size() < size_rx_prot)
	{
		PrintError("COM::protocol_rx", "invalid vector size", in.size());
		exit(EXIT_FAILURE);
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
	else if (((in[size_rx_prot - 2] >> 4) & 0x0F) != ('C' & 0x0F))
	{
		flag_ok = false;
	}
	else if (in[size_rx_prot - 1] != 'C')
	{
		flag_ok = false;
	}
	else
	{
		for (size_t i = 0; i < size_rx_raw * 2; i++)
		{
			if (((in[i + 1] >> 4) & 0x0F) != (~(in[i + 2]) & 0x0F))
			{
				flag_ok = false;
			}
		}
	}

	/* データを組み直す */
	std::vector<uint8_t> out;
	if (flag_ok == true)
	{
		for (size_t i = 0; i < size_rx_raw; i++)
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
void COM::setup(void)
{
	fd = open(path, O_RDWR);
	if (fd < 0)
	{
		strcat(name, ", ");
		strcat(name, path);
		PrintError("COM::setup", "OpenError", name);
		exit(EXIT_FAILURE);
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
	cfsetispeed(&term, brate); /* 入力ボーレート */
	cfsetospeed(&term, brate); /* 出力ボーレート */

	tcsetattr(fd, TCSANOW, &term); /* 設定を反映 */
}

/*-----------------------------------------------
 *
 * ポートを閉じる
 *
-----------------------------------------------*/
void COM::destroy(void)
{
	close(fd);
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
	FD_ZERO(&readfds);	/* 初期化 */
	FD_SET(fd, &readfds); /* fd を readfds に登録 */
	/* タイムアウト */
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	/*-----------------------------------------------
	read が可能か確認
	-----------------------------------------------*/
	select(fd + 1, &readfds, NULL, NULL, &tv);

	/* read 可能 */
	if (FD_ISSET(fd, &readfds) != 0)
	{
		read(fd, data, 1); /* データを読み出す */
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
void COM::serial_write(std::vector<uint8_t> data0)
{
	/* バイト数 * 1ms の間隔をあけて送信 */
	if (CALC_SEC(time_s, GetTime()) > size_tx_prot * 1E-3)
	{
		if (check_write() == true)
		{
			write(fd, data0.data(), data0.size());
			time_s = GetTime();
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
	FD_ZERO(&writefds);	/* 初期化 */
	FD_SET(fd, &writefds); /* fd を writefds に登録 */
	/* タイムアウト */
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 0;

	/*-----------------------------------------------
	write が可能か確認
	-----------------------------------------------*/
	select(fd + 1, NULL, &writefds, NULL, &tv);

	/* write 可能 */
	if (FD_ISSET(fd, &writefds) != 0)
	{
		refreshed = false;
		return true;
	}
	/* write 不可能 */
	else
	{
		setup(); /* リフレッシュ */
		if (refreshed == false)
		{
			refreshed = true;
			printf("\t\trefreshed.(%s)\n", name);
		}
		return false;
	}
}