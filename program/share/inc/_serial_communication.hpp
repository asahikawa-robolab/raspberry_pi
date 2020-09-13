/*-----------------------------------------------
 *
 * Last updated : 2020/09/06, 17:19
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
#include <thread>
#include "_picojson.hpp"
#include "_std_func.hpp"
#include "_thread.hpp"

namespace jibiki
{
  class Com
  {
  public:
    virtual uint8_t &tx(size_t index); /* 送信データを設定する */
    uint8_t rx(size_t index) const;    /* 受信データを取り出す */
    void send(void);
    bool receive(bool debug = false);
    double get_rx_freq(double sampling_period_ms);

  public:
    /* int m_brate は B9600 のように指定する */
    Com(std::string path,
        size_t size_tx_raw_data,
        size_t size_rx_raw_data,
        int brate,
        std::string name);
    virtual ~Com();

  protected:
    std::vector<uint8_t> m_rx, m_tx; /* 送信，受信データ */
    std::string m_name;              /* 通信ポートの名前 */
    size_t m_size_tx_raw;            /* もともとの送信データ数 */
    size_t m_size_rx_raw;            /* もともとの受信データ数 */

  private:
    size_t m_size_tx_prot;       /* 新通信プロトコルでの送信データ数 */
    size_t m_size_rx_prot;       /* 新通信プロトコルでの受信データ数 */
    bool m_is_refreshed;         /* 連続でリフレッシュしたかどうか */
    int m_fd;                    /* ファイルディスクリプタ */
    int m_brate;                 /* ボーレート */
    std::string m_path;          /* コンストラクタ */
    std::vector<uint8_t> m_bufs; /* 受信バッファ */
    timespec m_time_s_tx;        /* 送信周期調整 */

  private:
    timespec m_time_s_rx;     /* 受信周期測定 */
    size_t m_rx_complete_cnt; /* 受信完了したカウント */
    double m_rx_freq;         /* 受信周波数 */

  private:
    void setup(void);
    void destroy(void); /* プログラム終了時に必ずこのメソッドを呼び出さなければならない */
    bool check_write(void);
    bool serial_one_byte_read(uint8_t *data);
    void serial_write(std::vector<uint8_t> data);
    std::vector<uint8_t> protocol_tx(std::vector<uint8_t> in);
    std::vector<uint8_t> protocol_rx(std::vector<uint8_t> in);

  private:
    /* private 隠蔽 */
    Com(const Com &obj);                  /* コピーコンストラクタ */
    const Com &operator=(const Com &rhs); /* operator= のオーバーロード */
  };
  /* 送信データを取り出す */
  inline uint8_t &Com::tx(size_t index) { return m_tx[index]; }
  /* 受信データを取り出す */
  inline uint8_t Com::rx(size_t index) const { return m_rx[index]; }

  /*-----------------------------------------------
   *
   * ParamCom
   *
  -----------------------------------------------*/
  class ParamCom : public Com
  {
  public:
    ParamCom(std::string path,
             size_t size_tx_raw_data,
             size_t size_rx_raw_data,
             int brate,
             std::string name,
             bool param_set,
             std::string json_path = "param.json");
    virtual uint8_t &tx(size_t index); /* 送信データを取り出す */

  private:
    const size_t m_param_unit_num = 2; /* 同時に送信するパラメータの数 */
    std::vector<int16_t> m_param;      /* スレーブのパラメータ */
    size_t m_cur_param_index;          /* 設定中のパラメータのインデックス */
    bool m_param_set;                  /* パラメータ設定を行うかどうか */

  private:
    void init_set_param(std::string json_path);
    void set_param(void); /* スレーブにパラメータを設定する */

  private:
    /* private 隠蔽 */
    ParamCom(const ParamCom &obj);                  /* コピーコンストラクタ */
    const ParamCom &operator=(const ParamCom &rhs); /* operator= のオーバーロード */
  };

  /*-----------------------------------------------
   *
   * ProcParamCom
   *
  -----------------------------------------------*/
  class ProcParamCom
  {
  public:
    /* 関数ポインタ */
    typedef void (*ComFunc)(std::string path, std::string name);
    /* コンストラクタ */
    ProcParamCom(ShareVal<bool> &exit_flag,
                 std::vector<ComFunc> com_func,
                 std::string json_path = "setting.json");
    /* デストラクタ */
    ~ProcParamCom(void) { m_t.join(); }

  private:
    ShareVal<bool> *m_exit_flag;
    std::vector<ComFunc> m_com_func;
    std::string m_json_path;
    std::vector<std::string> m_name;
    std::vector<uint8_t> m_enable;
    std::vector<std::string> m_path;
    std::thread m_t;

  private:
    void init(ShareVal<bool> &exit_flag,
              std::vector<ComFunc> com_func,
              std::string json_path);
    void launch(void);
    void load_setting(void);
  };

} // namespace jibiki

#endif
