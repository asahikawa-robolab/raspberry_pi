# _thread
[戻る](../../README.md/#リファレンス)

# 概要
* `std::thread` を用いて行う並列処理を補助するクラスをまとめたもの．
    * ２つのクラス `jibiki::ShareVal`，`jibiki::ShareValVec` と１つの名前空間 `jibiki::thread` が含まれる．
* `jibiki::ShareVal` と `jibiki::ShareValVec` は[排他制御](#排他制御)を提供するクラス．
* `jibiki::ShareVal` は将来的に `std::atomic` に置き換える予定．

# jibiki::ShareVal
* 内部で[排他制御](#排他制御)を行ってデータを管理し，スレッドセーフを保証する（データ競合が発生しない）クラス．
* **異なるスレッド間で変数を共有するときには必須．**
* POD 型※や列挙型，`std::string` などが使用できる．
* STL のコンテナクラス（`std::vector` や `std::map` など）は使用できない．
* `std::vector` を使用したい場合は疑似的に `std::vector` を再現したクラスである [`jibiki::ShareValVel`](#jibikiShareValVec) を使用する．

※POD（Plain Old Data）型：`int`，`float` 等のいわゆるプレーンな古い型

### [クイックスタート](quick_share_val.md)

### メンバ関数
|名前|説明|
|:-|:-|
|(constructor)|コンストラクタ|
|operator=|代入演算子|
|operator+=|複合代入演算子（加算）|
|operator-=|複合代入演算子（減算）|
|read|値を読み出す|
|test_lock|指定時間ロックをかける|

# jibiki::ShareValVec
* 内部で[排他制御](#排他制御)を行ってデータを管理し，スレッドセーフを保証する（データ競合が発生しない）クラス．
* `jibiki::ShareVal` で使用できない `std::vector` を疑似的に再現したクラス．
* 基本的な使い方は `std::vector` と同様．

### [クイックスタート](quick_share_val_vec.md)

### メンバ関数
|名前|説明|
|:-|:-|
|read|値を読み出す|
|write|値を書き込む|
|push_back|末尾に要素を追加する|
|erase|指定した要素を削除する|
|clear|すべての要素を削除する|
|size|要素数を取得する|
|resize|要素数を変更する|

# jibiki::thread
スレッドの管理を補助する関数群．

### メンバ関数
|名前|説明|
|:-|:-|
|[manage](spec_thread.md/#jibikithread-manage)|スレッドの終了，一時停止を管理する|
|[enable](spec_thread.md/#jibikithread-enable)|スレッドを実行するかどうかを JSON ファイルで管理できるようにする|

### メンバ型
|名前|説明|
|:-|:-|
|[OperateMethod](spec_thread.md/#jibikithread-OperateMethod)|操作方法を表す|

# 排他制御
* `std::thread` 等を利用して並列処理（マルチスレッド処理）を行う際に **データ競合（data race）** が発生すると**未定義動作（undefined behavior）** を引き起こしてしまう．
* C++11 における**データ競合**は以下の３つの条件を満たすときに発生する．
    * 同一メモリ位置に対するアクセスにおいて，
    * 少なくとも一方が変更（modify）操作であり，
    * 異なるスレッド上から同時に行われるとき．
* 並列処理を行う際にはデータ競合を回避するために，同一変数へのアクセスが同時に生じないように制御（**排他制御**）する必要がある．




参考：[スレッドセーフという幻想と現実 - yohhoyの日記（別館）](https://yohhoy.hatenablog.jp/entry/2013/12/15/204116)