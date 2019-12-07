#ifndef _SERIAL_COMMUNICATION_HPP
#define _SERIAL_COMMUNICATION_HPP
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <termios.h>
#include <unistd.h>
#include <time.h>
#include "_StdFunc.hpp"

#define ENCRYPTION 0
#define DECRYPTION 1

#define UP(data) ((((int16_t)data) >> 8) & 0xFF)
#define LOW(data) (((int16_t)data) & 0xFF)
#define ASBL(up, low) ((int16_t)((up) << 8) | (low)) /* Assemble */

class COM
{
public:
  std::vector<uint8_t> rx, tx; /* 送信，受信データを保管する */
  bool renew;                  /* 受信データが更新されたかどうか */
  COM(const char *path0,
      size_t SizeTxRawData0,
      size_t SizeRxRawData0,
      int brate0, const char *name0); /* int brate は B9600 のように指定する */
  ~COM();
  void send(void);
  bool receive(void);
  void destroy(void); /* プログラム終了時に必ずこのメソッドを呼び出さなければならない */

private:
  bool refreshed;               /* 連続でリフレッシュしたかどうか */
  size_t size_tx_raw,           /* もともとの送信データ数 */
      size_tx_prot,             /* 新通信プロトコルでの送信データ数 */
      size_rx_raw,              /* もともとの受信データ数 */
      size_rx_prot;             /* 新通信プロトコルでの受信データ数 */
  int fd;                       /* ファイルディスクリプタ */
  int brate;                    /* ボーレート */
  char path[MAX_STRING_LENGTH]; /* コンストラクタ */
  char name[MAX_STRING_LENGTH];
  struct timespec time_s;    /* 送信周期調整 */
  std::vector<uint8_t> bufs; /* 受信バッファ */

  bool serial_one_byte_read(uint8_t *data);
  void serial_write(std::vector<uint8_t> data0);
  bool check_write(void);
  std::vector<uint8_t> protocol_tx(std::vector<uint8_t> in);
  std::vector<uint8_t> protocol_rx(std::vector<uint8_t> in);
  void setup(void);
  /* private 隠蔽 */
  COM(const COM &obj);                  /* コピーコンストラクタ */
  const COM &operator=(const COM &rhs); /* operator= のオーバーロード */
};

#endif
