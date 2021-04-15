# 概要
RaspberryPi をマスターとしてスレーブを制御するためのライブラリとサンプルプログラムをまとめたリポジトリ
|プログラム|内容|
|:-|:-|
|com_controller|コントローラと通信するサンプル|
|com_imu1|IMU と通信するサンプル|
|com_imu2|IMU と通信するサンプル（リセット方法が com_imu1 と異なる）|
|com_odometry|rotary_encoder_slave（odometry）と通信するサンプル|
|com_pwm_control|rotary_encoder_slave（pwm_control）と通信するサンプル|
|com_rev_control|rotary_encoder_slave（revolution_control）と通信するサンプル|
|com_rot_control|rotary_encoder_slave（rotation_angle_control）と通信するサンプル|
|com_solenoid_slave|solenoid_valve_slave と通信するサンプル|
|com_switch|switch_slave と通信するサンプル|
|com_thread|スレッドを使用して通信するサンプル|
|dead_lock|std::mutex でデッドロックが発生するサンプル|
|operate_auto|自動制御を行うサンプル|
|whole|自動制御と手動制御の両方を行うサンプル|
|share|RaspberryPi で利用するライブラリ|

# ライブラリについて
|ライブラリ|用途|
|:-|:-|
|[_picojson](<https://github.com/kazuho/picojson>)|JSON パーサ<br>[Kazuho Oku](https://github.com/kazuho) さんのものを利用|
|_std_func|頻繁に使用する汎用的な処理|
|_serial_communication|シリアル通信ライブラリ|
|_thread|並列処理に関連するプログラム|
|_utility|ロボットを制御する上で有用なプログラム．<br>今のところ自動制御のプログラムのみ．|
|module|ロボットに関する処理の中で汎用性の高いプログラムをまとめたもの|

# リファレンス
* _picojson
* [_std_func](doc/_std_func/_std_func.md)
* [_serial_communication](doc/_serial_communication/overview.md)
* [_thread](doc/_thread/overview.md)
* [_utility](doc/_utility/overview.md)
* [module](doc/module/overview.md)

<br>

* [std::thread](doc/std_thread/overview.md)

# トラブルシューティング
### 1. `rotary_encoder_slave` が動作しない
* `parameter.json` で設定しているパラメータが使用しているスレーブに対応しているか確認する．
    * 例えば `revolution_control` に対して `rotation_angle_control` のパラメータを設定していないか
* スレーブの LED の細かい点滅は通信ができていることを表しており，そこからパラメータが正常に設定されているかを判断することはできない（１秒周期の点滅は何らかのエラー（主に極性）を表す）．
* パラメータ設定が完了するとマスター側で `set param complete.` と表示されるが，これはあくまで `param.json` で指定されたパラメータをすべて設定し終わったことを表しているだけであり，設定されたパラメータがスレーブにとって適切なものかは考慮されない点に注意．