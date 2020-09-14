# Imu
[戻る](overview.md/#Imu)

# 目次
1. [ヘッダファイル](#1-ヘッダファイル)
2. [データの準備](#2-データの準備)
3. [データの読みだし](#3-データの読みだし)
4. [オフセットの設定](#4-オフセットの設定)
5. [サンプル](#5-サンプル)

# 1. ヘッダファイル
```C++
#include "../../share/inc/module.hpp"
```

# 2. データの準備
`Imu::write_raw_data()` で IMU スレーブから受信したデータをセットする．

# 3. データの読みだし
`Imu::read()` でデータを読み出す．

# 4. オフセットの設定
`Imu::write_offset()` でオフセットを設定して現在値を変更できる．

# 5. サンプル
```C++
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/module.hpp"

int main(void)
{
    try
    {
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579AL-if00-port0",
            1, 4, B57600, "imu", false);

        Imu imu;

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'r':
                    /* 現在の角度を 0 [deg] とする */
                    imu.write_offset(0);
                    break;
                case 'q':
                    return 0;
                }
            }

            /* 受信 */
            if (com.receive())
            {
                /* センサ出力から角度を計算 */
                double angle =
                    jibiki::deg_rad(jibiki::asbl(com.rx(0), com.rx(1)) / 10.0);
                /* 角度を読み込ませる */
                imu.write_raw_data(angle);
                /* 表示 */
                printf("%.2lf [deg]\n", jibiki::rad_deg(imu.read()));
            }
        }
    }
    catch (std::string err)
    {
        std::cout << err << std::endl;
        return 1;
    }
    return 0;
}
```
キーボードの `r` を入力した位置を `0 [deg]` にするサンプル．