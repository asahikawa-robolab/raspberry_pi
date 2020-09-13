# _serial_communication
[戻る](../../README.md/#リファレンス)

# 概要
* RaspberryPi でシリアル通信を行うために必要なクラスをまとめたもの．
    * `jibiki::Com`，`jibiki::ParamCom`，`jibiki::ProcParamCom` の３つのクラスが含まれる．
* `jibiki::ParamCom` は `jibiki::Com` の機能をすべて兼ねているため，基本的に使用するのは `jibiki::ParamCom` と `jibiki::ProcParamCom` の２つ．
    * `jibiki::Com` はシリアル通信の最低限の機能を持つクラスであるため，通信のクラスを新たに作るときに派生させて使用することが想定されている．

# jibiki::Com
部内で使用している通信プロトコルに則ってシリアル通信を行うクラス．

### メンバ関数
|名前|説明|
|:-|:-|
|(constructor)|コンストラクタ|
|(destructor)|デストラクタ|
|tx|送信データを設定する|
|rx|受信データを取り出す|
|send|送信する|
|receive|受信する|
|get_rx_freq|受信周波数を取得する|

# jibiki::ParamCom
* `jibiki::Com` を継承してパラメータ設定の機能を追加したクラス．
* スレーブとの通信にはこのクラスの使用を想定している．

### [クイックスタート](quick_param_com.md)

### メンバ関数
|名前|説明|
|:-|:-|
|(constructor)|コンストラクタ|
|tx|送信データを設定する|
|rx|受信データを取り出す|
|send|送信する|
|receive|受信する|
|get_rx_freq|受信周波数を取得する|

# jibiki::ProcParamCom
* `jibiki::Com` または `jibiki::ParamCom` を使った処理を JSON ファイルで管理できるようにするクラス．
* あらかじめ通信処理を行う関数を用意しておいてオブジェクト作成時に渡すことで，自動的に別スレッドで渡された関数を実行する．

### [クイックスタート](quick_proc_param_com.md)

### メンバ関数
|名前|説明|
|:-|:-|
|(constructor)|コンストラクタ|
|(destructor)|デストラクタ|