# jibiki::ProcParamCom
[戻る](overview.md/#jibikiProcParamCom)

# 目次
1. [JSON ファイルの準備](#1-json-ファイルの準備)
2. [ヘッダファイル](#2-ヘッダファイル)
3. [通信処理を行う関数の準備](#3-通信処理を行う関数の準備)
4. [オブジェクトの作成](#4-オブジェクトの作成)
5. [サンプル](#5-サンプル)

# 1. JSON ファイルの準備
* 以下の値を記述した JSON ファイルを用意する必要がある．
    * `jibiki::thread::enable` 用の値
        * スレッドを実行するかどうか（`true` / `false`）
    * `jibiki::ProcParamCom` 用の値
        * 通信を行うかどうか（`true` / `false`）
        * 通信するシリアルポートのデバイスファイルのパス
* プログラムをビルドした際に生成される実行ファイルと同じディレクトリに `setting.json` を作成する．※

例）
```JSON
{
    "thread":
    {
        "com": true
    },
    "com": [
        ["com_test1", true, "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LQ-if00-port0"],
        ["com_test2", true, "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V99N-if00-port0"]
    ]
}
```

※パラメータを記述した JSON ファイルの名前や位置は，`jibiki::ProcOperateAuto` のオブジェクトを作成する際の引数 `json_path` で変更できる．デフォルトは `setting.json`．

# 2. ヘッダファイル
```C++
#include "../../share/inc/_serial_communication.hpp"
```

# 3. 通信処理を行う関数の準備
* オブジェクトを作成する前に通信処理を行う関数（以降 `ComFunc` と呼ぶ）を定義する．
* `ComFunc` の型は必ず `void func(std::string path, std::string name)` にする（`func` は任意）．
* `jibiki::ParamCom` のオブジェクトを作成するときは必ず
    * `static` をつけて，
    * `patn`，`name` に `ComFunc` の仮引数を使う．

例）
```C++
void com_rot_control(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, true);

    /* 送信 */
    com.tx(1) = jibiki::up(g_angle.read());
    com.tx(2) = jibiki::low(g_angle.read());
    com.send();

    /* 受信 */
    if (com.receive())
    {
        printf("target %d, curr %d, pwm %d\n",
               g_angle.read(),
               jibiki::asbl(com.rx(0), com.rx(1)),
               (int8_t)com.rx(4));
    }
}
```

# 4. オブジェクトの作成
```C++
ProcParamCom(ShareVal<bool> &exit_flag,
                std::vector<ComFunc> com_func,
                std::string json_path = "setting.json");
```

|引数名|説明|
|:-|:-|
|exit_flag|終了フラグ．<br>終了する：`true`<br>終了しない：`false`|
|com_func|実行したい通信処理を行う関数を指定する．<br>関数の型は必ず `void func(std::string paths, std::string name)` にする．|
|json_path|パラメータを記述した JSON ファイルのパスを指定する．<br>省略した場合は `setting.json` になる．|

例）
```C++
jibiki::ProcParamCom com(std::ref(exit_flag),
                            {com_test1,
                            com_test2});
```

# 5. サンプル
* これまでに説明したことを踏まえて `jibiki::ProcParamCom` を使用する一連のプログラムを示す．
* 実際に通信を行うサンプルは本リポジトリの `program/com_thread` 以下にあるため参考にされたい．
### main.cpp
```C++
#include <iostream>
#include <thread>
#include "../../share/inc/_std_func.hpp" /* キー入力 */
#include "../../share/inc/_serial_communication.hpp"
#include "../../share/inc/_thread.hpp"

void com_test1(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 5, 6, B57600, name, false);

    /* 送信 */
    /* 送信データを設定する */
    com.send();

    /* 受信 */
    if (com.receive())
    {
        /* 受信データを取り出す */
    }

    std::cout << "com_test1\n" << std::flush;
}

void com_test2(std::string path, std::string name)
{
    static jibiki::ParamCom com(path, 0, 8, B57600, name, false);

    /* 送信 */
    /* 送信データを設定する */
    com.send();

    /* 受信 */
    if (com.receive())
    {
        /* 受信データを取り出す */
    }

    std::cout << "\t\tcom_test2\n" << std::flush;
}

void process_kbhit(jibiki::ShareVal<bool> &exit_flag)
{
    if (!jibiki::thread::enable("kbhit"))
        return;

    while (jibiki::thread::manage(exit_flag))
    {
        /* キー入力 */
        if (jibiki::kbhit())
        {
            exit_flag = true;
            break;
        }
    }
}

int main(void)
{
    jibiki::ShareVal<bool> exit_flag; /* 終了フラグ */

    jibiki::ProcParamCom com(std::ref(exit_flag),
                             {com_test1,
                              com_test2});
    std::thread t(process_kbhit,
                  std::ref(exit_flag));
    t.join();

    return 0;
}
```
### setting.json
```C++
{
    "thread":
    {
        "com": true,
        "kbhit": true
    },
    "com": [
        ["com_test1", true, "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_AI0579LQ-if00-port0"],
        ["com_test2", true, "/dev/serial/by-id/usb-FTDI_FT232R_USB_UART_A906V99N-if00-port0"]
    ]
}
```

* プログラムの開始とともに別スレッドで `com_test1`，`com_test2` を実行する．
* キーボードの `q` を入力するとすべてのスレッドが終了し，プログラムも終了する．
* このサンプルプログラムを実際に実行する場合は `setting.json` のデバイスファイルのパスを適切なものに変更する必要がある．
* サンプルプログラムの動作確認はしてないです．