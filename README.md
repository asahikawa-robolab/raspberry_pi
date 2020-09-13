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
|operate_auto|自動制御を行うサンプル|
|share|RaspberryPi で利用するライブラリ|

# ライブラリについて
|ライブラリ|用途|
|:-|:-|
|[_picojson](<https://github.com/kazuho/picojson>)|JSON パーサ<br>[Kazuho Oku](https://github.com/kazuho) さんのものを利用|
|_std_func|頻繁に使用する汎用的な処理|
|_serial_communication|シリアル通信ライブラリ|
|_thread|並列処理に関連するプログラム|
|_utility|ロボットを制御する上で有用なプログラム．<br>足回りの制御や自動制御のプログラムが含まれる（予定）．|

# リファレンス
* _picojson
* [_std_func](doc/_std_func/_std_func.md)
* [_serial_communication](doc/_serial_communication/overview.md)
* [_thread](doc/_thread/overview.md)
* [_utility](doc/_utility/overview.md)

<br>

* [std::thread](doc/std_thread/overview.md)