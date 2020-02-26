/*-----------------------------------------------
 *
 * Last updated : 2020/02/05, 21:17
 * Author       : Takuto Jibiki
 *
-----------------------------------------------*/
#ifndef _SERIAL_COMMUNICATION_HPP
#define _SERIAL_COMMUNICATION_HPP
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include <stdint.h>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>
#include "_picojson.hpp"
#include "_std_func.hpp"

namespace jibiki
{

#define PARAM_UNIT_NUM 2 /* 同時に送信するパラメータの数 */

class COM
{
public:
  std::vector<uint8_t> m_rx, m_tx; /* 送信，受信データ */
  bool m_is_set_param_finished;    /* パラメータ設定が終了したか */
  void send(void);
  bool receive(bool debug = false);
  double get_rx_freq(void);
  void set_param(void);

public:
  /* int m_brate は B9600 のように指定する */
  COM(std::string path,
      size_t size_tx_raw_data,
      size_t size_rx_raw_data,
      int brate,
      std::string name,
      bool param_set = false);
  ~COM();

private:
  size_t m_size_tx_raw;        /* もともとの送信データ数 */
  size_t m_size_tx_prot;       /* 新通信プロトコルでの送信データ数 */
  size_t m_size_rx_raw;        /* もともとの受信データ数 */
  size_t m_size_rx_prot;       /* 新通信プロトコルでの受信データ数 */
  bool m_is_refreshed;         /* 連続でリフレッシュしたかどうか */
  int m_fd;                    /* ファイルディスクリプタ */
  int m_brate;                 /* ボーレート */
  std::string m_path;          /* コンストラクタ */
  std::string m_name;          /* 通信ポートの名前 */
  std::vector<uint8_t> m_bufs; /* 受信バッファ */
  struct timespec m_time_s_tx; /* 送信周期調整 */

private:
  struct timespec m_time_s_rx; /* 受信周期測定 */
  size_t m_rx_complete_cnt;    /* 受信完了したカウント */
  size_t m_execute_cnt;        /* get_rx_freq の実行された回数 */
  double m_rx_freq;            /* 受信周波数 */
  /* set_param */
  bool m_is_first_set_param;    /* set_param() が初めて呼ばれたか */
  std::vector<int16_t> m_param; /* スレーブのパラメータ */
  size_t m_cur_param_index;     /* 設定中のパラメータのインデックス */

private:
  void setup(void);
  void destroy(void); /* プログラム終了時に必ずこのメソッドを呼び出さなければならない */
  void core_send(void);
  bool core_receive(bool debug = false);
  bool check_write(void);
  bool serial_one_byte_read(uint8_t *data);
  void serial_write(std::vector<uint8_t> data);
  std::vector<uint8_t> protocol_tx(std::vector<uint8_t> in);
  std::vector<uint8_t> protocol_rx(std::vector<uint8_t> in);
  void init_set_param(void);

private:
  /* private 隠蔽 */
  COM(const COM &obj);                  /* コピーコンストラクタ */
  const COM &operator=(const COM &rhs); /* operator= のオーバーロード */
};

inline void COM::send(void)
{
  if (m_is_set_param_finished)
    core_send();
}
inline bool COM::receive(bool debug)
{
  if (m_is_set_param_finished)
    return core_receive(debug);
  return false;
}

} // namespace jibiki

#endif
