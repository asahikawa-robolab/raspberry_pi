# Chassis
[戻る](overview.md/#Chassis)

# 目次
1. [JSON ファイルの準備](#1-JSON-ファイルの準備)
2. [ヘッダファイル](#2-ヘッダファイル)
3. [オブジェクトの作成](#3-オブジェクトの作成)
4. [動きを指定する](#4-動きを指定する)
5. [足回りモータの回転数目標値を計算，取得](#5-足回りモータの回転数目標値を計算取得)
6. [足回りの停止](#6-足回りの停止)
7. [`chassis/max_rpm` の取得](#7-chassismax_rpm-の取得)


# 1. JSON ファイルの準備
`setting.json` に以下の内容を記載すること．値は適宜変更すること．
### setting.json
```JSON
{
    "chassis": {
        "max_rpm": 300,
        "channel": {
            "fr" : 0,
            "fl" : 1,
            "br" : 2,
            "bl" : 3
        },
        "inverse": {
            "fr": false,
            "fl": false,
            "br": false,
            "bl": false            
        },
        "rotate": {
            "min": 20,
            "max": 50,
            "kp": 60
        }
    },
    "system": {
        "calc_period_ms": 50
    }
}
```

* `fr`，`fl`，`br`，`bl` はそれぞれ右前，左前，右後，左後を表す．
* `max_rpm` は足回りモータの回転数目標値を表し，これよりも大きな値は出力されない．
* `channel` の `fr`，`fl` 等に指定されている `0 ~ 3` の値を入れ替えることによって値を出力するモータを入れ替えることができる．
* `inverse` の `fr`，`fl` 等の値に `true` を指定すると回転方向が反転する．
* `rotate` では旋回処理で用いられている比例制御に関するパラメータを設定する．
    * `rotate/min`：旋回時の回転数の最小値
    * `rotate/max`：旋回時の回転数の最大値
    * `rotate/kp`：旋回時の回転数を決定する際の比例ゲイン
* `system/calc_period_ms` には `Chassis` のオブジェクトが作成されてから周期的に実行される処理の実行周期を `[ms]` 単位で指定．

# 2. ヘッダファイル
```C++
#include "../../share/inc/module.hpp"
```

# 3. オブジェクトの作成
```C++
Chassis(Imu &imu, std::string json_path = "setting.json");
```
|引数名|説明|
|:-|:-|
|imu|Imu のオブジェクトを指定する．|
|json_path|JSON ファイルのパスを指定する．<br>省略した場合は `setting.json` になる．|

例）
```C++
Imu g_imu;                          /* IMU のデータ */
Chassis g_chassis(g_imu);           /* 足回り */
```

# 4. 動きを指定する
`Chassis` のメンバ変数に値を書き込むことで動きを指定できる．

|メンバ変数名|説明|
|:-|:-|
|Chassis::m_speed|移動する速さ|
|Chassis::m_theta|移動する向き|
|Chassis::m_spin|ロボットの向き|
|Chassis::m_turn_mode|旋回方法|

# 5. 足回りモータの回転数目標値を計算，取得
* `Chassis::fr()`，`Chassis::fl()`，`Chassis::br()`，`Chassis::bl()` を呼び出すと内部で計算が行われ，それぞれ右前，左前，右後，左後のモータの回転数目標値を返す．
    * この値は `setting.json` における `chassis/channel`，`chassis/inverse` が反映されたもの．
    * `setting.json` における `chassis/channel`，`chassis/inverse` の影響を受けない値を取得したい場合※は `Chassis::raw_fr()`，`Chassis::raw_fl()`，`Chassis::raw_br()`，`Chassis::raw_bl()` を使う．

※例えばモータの回転数目標値をデバッグのために表示するとき

# 6. 足回りの停止
`Chassis::stop()` を呼び出すと `Chassis::fr()` 等の関数によって `0` が戻り値として返されるようになる．

# 7. `chassis/max_rpm` の取得
JSON ファイルに記載した `chassis/max_rpm` の値は `Chassis::max_rpm()` で取得できる．移動する速さを決定する時に使用することが想定される．