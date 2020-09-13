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