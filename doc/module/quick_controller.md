# Controller
[戻る](overview.md/#Controller)

# 目次
1. [JSON ファイルの準備](#1-JSON-ファイルの準備)
2. [ヘッダファイル](#2-ヘッダファイル)
3. [オブジェクトの作成](#3-オブジェクトの作成)
4. [データの準備](#4-データの準備)
5. [データの読み出し](#5-データの読み出し)
6. [アナログスティックの計算](#6-アナログスティックの計算)

# 1. JSON ファイルの準備
`Controller` のオブジェクトが作成されてから周期的に実行される処理の実行周期 `system/calc_period_ms` を `setting.json` に記述する必要がある．※
### setting.json
```JSON
{
    "system": {
        "calc_period_ms": 50
    }
}
```
※厳密には `system/calc_period_ms` で設定する値は実行周期ではなく，インターバルである．"オブジェクトが作成されてから周期的に実行される処理" は周期的に実行されるようになっているわけではなく，`Controller::speed()` または `Controller::theta()` が呼び出されたとき内部で実行されるようになっている．したがって `system/calc_period_ms` に指定した値よりも `Controller::speed()` または `Controller::theta()` が呼び出される周期が長い場合，`system/calc_period_ms` に指定した値通りには動作しない．

# 2. ヘッダファイル
```C++
#include "../../share/inc/module.hpp"
```

# 3. オブジェクトの作成
```C++
Controller(std::string json_path = "setting.json");
```
|引数名|説明|
|:-|:-|
|json_path|JSON ファイルのパスを指定する．<br>省略した場合は `setting.json` になる．|

例）
```C++
Controller controller;
```

# 4. データの準備
`Controller::set()` でコントローラから受信したデータをセットする．
`Controller::send()` でセットされた文字データを送信する．

# 5. データの読み出し
`Controller::l_cross_l()`, `Controller::l_switch_d()`, `Controller::l_analog_stick_h()` 等のメンバ関数でデータを読み出す．

# 6. 文字列のセット
`Controller::lcd_sprintf1()`,`Controller::lcd_sprintf2()`でLCDに表示する文字列をセットする(半角カタカナに対応)

# 7. アナログスティックの計算
* `Controller::speed()` と `Controller::theta()` でそれぞれアナログスティックの倒し具合と倒している向きを取得できる．
* 両方とも引数は `(Mode mode, DirNum dir_num)` 
    * `Mode` には左スティックか右スティックかを指定する．
        * `Controller::MODE_L`：左スティック
        * `Controller::MODE_R`：右スティック
    * `DirNum` にはアナログスティックを倒している向きを計算する際の分割数を指定する．
        * `Controller::DIR_INF`：全方向
        * `Controller::DIR_8`：８方向
        * `Controller::DIR_4`：４方向