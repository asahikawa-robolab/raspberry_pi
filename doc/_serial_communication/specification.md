# 目的から探す
| 目的 | 名前 |
|:-|:-|
|経過時間を測定する|[jibiki::calc_sec](#jibikicalc_sec)|

# 一覧
### 時間関係
| 名前 | 概要 |
|:-|:-|
|[jibiki::get_time](#jibikiget_time)| 時刻を取得する |
|[jibiki::calc_sec](#jibikicalc_sec)| 引数に指定した２つの時刻の差を返す |

<br>
<br>

# jibiki::Com::コンストラクタ
### 宣言

```C++
Com(std::string path,
    size_t size_tx_raw_data,
    size_t size_rx_raw_data,
    int brate,
    std::string name);
```

### 概要
シリアルポートを開き，通信を行う準備をする．
### 引数
|引数名|説明|
|:-|:-|
|path|シリアルポートに対応するデバイスファイルを絶対パスで指定．<br>[シリアルポートの確認方法](#シリアルポートの確認方法)|
|size_tx_raw_data|送信データ数（byte）※|
|size_rx_raw_data|受信データ数（byte）※|
|brate|ボーレートを指定．<br>ボーレートの数字の先頭に `B` をつける．<br>例）B57600|
|name|シリアルポートを識別する任意の文字列．<br>他のポートと被らなければ何でもよい．|
※通信プロトコルの形式に変換する前のデータ数を指定する．<br>
例）通信プロトコルに則って 3 [byte] のデータを送信する場合
* 実際に送信するデータ数は 3 * 2 + 3 = 9 [byte]
* しかし size_tx_raw_data にはプロトコルの形式に変換する前のデータ数を指定するので，size_tx_raw_data には元データのサイズである 3 を指定する．
* 受信データ（size_rx_raw_data）に関しても同様．
### 例外
シリアルポートを開くのに失敗したら例外を投げる（`std::string` 型）．
### サンプル
### 関連
*

[一覧へ戻る](#一覧)

# jibiki::Com::デストラクタ
### 概要
シリアルポートを切断し，jibiki::Com オブジェクトを破棄する．

[一覧へ戻る](#一覧)







# jibiki::get_time
### 宣言
timespec **get_time**(void)
### 概要
時刻を取得する．
### 引数
なし
### 戻り値
この関数を呼び出した時点での時刻（UTC 時刻）を timespec 型で返す．
### 例外
なし
### コメント
* `<time.h>` で定義されている `struct timespec 型` を `_std_func.hpp` で `timespec 型` に typedef している．
* 内部的には timespec_get を呼び出している
### 関連
* [jibiki::calc_sec](#jibikicalc_sec)

[一覧へ戻る](#一覧)

# jibiki::calc_sec
### 宣言
double **calc_sec**(timespec *s*, timespec *e*)
### 概要
引数に指定した２つの時刻の差を返す．
### 引数
* time_s：古い方の時刻．s は start の略．
* time_e：新しい方の時刻．e は end の略．
### 戻り値
引数に指定した２つの時刻の差．単位は秒．
### 例外
なし
### コメント
* 計算上の精度はナノ秒単位．
* しかし実際の精度はハードウェアに依存する．
### サンプル
３秒待機するプログラム
```c++
#include "../../share/inc/_std_func.hpp"

int main(void)
{
    /* 待機開始時の時間 time_s を取得 */
    timespec time_s = jibiki::get_time();

    printf("start\n");
    while (1)
    {
        /* 現在の時間 time_e を取得 */
        timespec time_e = jibiki::get_time();

        /* time_s から time_e までの経過時間を計算 */
        double passed_time = jibiki::calc_sec(time_s, time_e);

        /* 3 秒以上経過していたらループを抜ける */
        if (passed_time >= 3)
            break;
    }

    printf("finish\n");

    return 0;
}
```
### 関連
* [jibiki::get_time](#jibikiget_time)

[一覧へ戻る](#一覧)

# シリアルポートの確認方法
USB シリアル変換等のデバイスを RaspberryPi に接続すると `/dev/serial/by-id/` 以下にシリアルポートを表すデバイスファイルが生成される．プログラムでは通信したいデバイスに対応して生成されるデバイスファイルのパスを絶対パスで指定して使用する．

RaspberryPi では以下のコマンドでシリアルポートを確認することができる．

```bash
$ cd /dev/serial/by-id
$ ls
```

また，大会で使用していた RaspberryPi に関しては以下のコマンドでデバイスファイルが表示されるように設定されているものがあるかもしれない．その場合はこっちの方法が楽．

```bash
$ dev
```

<br>
<br>

# テンプレート
### 宣言
### 概要
### 引数
### 戻り値
### 例外
### コメント
### サンプル
### 関連
*

[一覧へ戻る](#一覧)