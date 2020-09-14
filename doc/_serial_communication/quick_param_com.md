# jibiki::ParamCom
[戻る](overview.md/#jibikiParamCom)

# 目次
0. [デバイスファイルのパスを取得](#0-デバイスファイルのパスを取得)
1. [パラメータの準備（パラメータ設定を行う場合のみ）](#1-パラメータの準備パラメータ設定を行う場合のみ)
2. [ヘッダファイルのインクルード](#2-ヘッダファイルのインクルード)
3. [シリアルポートを開く（オブジェクトの作成）](#3-シリアルポートを開くオブジェクトの作成)
4. [送信データを設定する](#4-送信データを設定する)
5. [送信する](#5-送信する)
6. [受信する](#6-受信する)
7. [受信データを取り出す](#7-受信データを取り出す)
8. [サンプル](#8-サンプル)

# 0. デバイスファイルのパスを取得
シリアルポートを開く際に，開きたいシリアルポートに対応したデバイスファイルのパスが必要になる．

1. パスを調べたい USB シリアル変換等のデバイスを USB ケーブル等で RaspberryPi に接続する．
2. 以下のいずれか片方のコマンドを実行してデバイスファイルの名前を表示．
```bash
$ dev
```
```bash
$ cd /dev/serial/by-id/
$ ls
```
3. 2 で表示したファイル名の先頭に `/dev/serial/by-id/` を付け加えたものがシリアルポートを開く際に使用するデバイスファイルのパス．

# 1. パラメータの準備（パラメータ設定を行う場合のみ）
* パラメータ設定の機能を使用する場合はあらかじめパラメータを記述した JSON ファイルを準備する必要がある．
* rotary_encoder_slave はパラメータ設定が必要．solenoid_valve_slave や controller はパラメータ設定が不要．

1. プログラムをビルドした際に生成される実行ファイルと同じディレクトリに `param.json` を作成する．※
2. JSON のフォーマットに則って 1. で作成したファイルにパラメータを記述する．以下に示す例の `"rot"` の部分の文字列は任意に決めてよいが，他のパラメータとは被らないようにする．

例）
```JSON
{
    "rot": [
        ["enable", 1, 0],
        ["zero point pwm", 30, 30],
        ["max pwm", 80, 100],
        ["min pwm", 5, 5],
        ["stop rev", 3, 3],
        ["stop pwm", 10, 10],
        ["pwm change", 170, 170],
        ["kp", 100, 100],
        ["permissible err", 1, 1],
        ["encoder pol", 1, 1],
        ["encoder resolution", 300, 300]
    ]
}
```

※パラメータを記述した JSON ファイルの名前や位置は，シリアルポートを開く際の引数 `json_path` で変更できる．デフォルトは `param.json`．

# 2. ヘッダファイルのインクルード
```C++
#include "../../share/inc/_serial_communication.hpp"
```

# 3. シリアルポートを開く（オブジェクトの作成）
```C++
ParamCom(std::string path,
            size_t size_tx_raw_data,
            size_t size_rx_raw_data,
            int brate,
            std::string name,
            bool param_set,
            std::string json_path = "param.json");
```
|引数名|説明|
|:-|:-|
|path|0. で説明したデバイスファイルのパス|
|size_tx_raw_data|送信データ数（byte）※|
|size_rx_raw_data|受信データ数（byte）※|
|brate|ボーレートを指定．<br>ボーレートの数字の先頭に `B` をつける．<br>例）B57600|
|name|シリアルポートを識別する任意の文字列．<br>他のポートと被らなければ何でもよい．|
|param_set|パラメータ設定を行うかどうか．<br>行う：`true`<br>行わない：`false`|
|json_path|パラメータを記述した JSON ファイルのパスを指定する．<br>省略した場合は `param.json` になる．|

※通信プロトコルの形式に変換する前のデータ数を指定する．<br>
例）通信プロトコルに則って 3 [byte] のデータを送信する場合
* 実際に送信するデータ数は 3 * 2 + 3 = 9 [byte]
* しかし size_tx_raw_data にはプロトコルの形式に変換する前のデータ数を指定するので，size_tx_raw_data には元データのサイズである 3 を指定する．
* 受信データ（size_rx_raw_data）に関しても同様．

例)

```C++
jibiki::ParamCom com(
    "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LQ-if00-port0",
    5, 6, B57600, "rot", true);
```

# 4. 送信データを設定する
* `jibiki::ParamCom::tx()` を利用する．
* 以下に示す例のように何番目のデータを設定したいかを引数で指定して，代入演算子を用いて送信したいデータを設定する．
* 設定できる値の型は `uint8_t`．
* パラメータ設定の機能を有効にした場合（シリアルポートを開く際に param_set に `true` を渡した場合），0 番目のデータ `tx(0)` はパラメータ設定の処理の都合で使用できない．（`tx(0)` のデータを書き換えようとすると例外が投げられる仕様になっている）．

例）
```C++
jibiki::ParamCom com( /* 省略 */ );

com.tx(1) = 13;
```

# 5. 送信する
* `jibiki::ParamCom::send()` を使用する．
* `jibiki::ParamCom::send()` を実行すると `jibiki::ParamCom::tx()` で設定された値が送信される．
* 通信相手のバッファが溢れるのを防ぐために，最低でも `送信データ数 × 1 [ms]` のインターバルが空けられるように内部で処理が行われているため，送信周期を気にする必要はない．※

※`送信データ数 × 1 [ms]` というインターバルは経験則で定めているため，異なる環境では正常に動作しない可能性がある．

例）
```C++
jibiki::ParamCom com( /* 省略 */ );

com.send();
```

# 6. 受信する
* `jibiki::ParamCom::receive()` を使用する．
* 受信が完了したら `true` を返すため，そのときにのみ受信データを取り出すようにすること．

例）
```C++
jibiki::ParamCom com( /* 省略 */ );

if (com.receive())
{
    /* 受信データを取り出す処理 */
}
```

# 7. 受信データを取り出す
* `jibiki::ParamCom::rx()` を使用する．
* 以下に示す例のように何番目のデータを取り出したいかを引数に指定する．

例）
```C++
jibiki::ParamCom com( /* 省略 */ );

if (com.receive())
{
    std::cout << com.rx(0) << std::endl;
}
```

# 8. サンプル
これまでに説明したことを踏まえて通信の一連の処理を記述した例を示す（com_rot_control のプログラムとほぼ同じ内容）．

```C++
#include "../../share/inc/_std_func.hpp"
#include "../../share/inc/_serial_communication.hpp"

int main(void)
{
    try
    {
        jibiki::ParamCom com(
            "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LQ-if00-port0",
            5, 6, B57600, "rot", true);

        short angle = 0;

        while (1)
        {
            /* キー入力 */
            if (jibiki::kbhit())
            {
                switch (getchar())
                {
                case 'u':
                    angle += 360;
                    break;
                case 'd':
                    angle -= 360;
                    break;
                case 'q':
                    return 0;
                }
            }

            /* 送信 */
            com.tx(1) = jibiki::up(angle);
            com.tx(2) = jibiki::low(angle);
            com.send();

            /* 受信 */
            if (com.receive())
            {
                printf("target %d, curr %d, pwm %d\n",
                       angle,
                       jibiki::asbl(com.rx(0), com.rx(1)),
                       (int8_t)com.rx(4));
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