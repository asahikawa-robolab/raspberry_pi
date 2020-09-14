# _utility
[戻る](../../README.md/#リファレンス)

# 概要
* ロボットを制御する上で有用なプログラムをまとめたもの．
* 足回りの制御や自動制御のプログラムが含まれる（予定）.

# jibiki::ProcOperateAuto
* 自動制御を行うプログラム．
* 登録された関数を指定された順番，タイミングで呼び出す．
* 内部で `std::thread` を用いた並列処理を行う．
### [クイックスタート](quick_proc_operate_auto.md)
### JSON：:o:
### メンバ関数
|名前|説明|
|:-|:-|
|(constructor)|コンストラクタ|
|(destructor)|デストラクタ|
|manage_thread_int|[`jibiki::thread::manage`](../_thread/spec_thread.md/#jibikithread-manage) をクラス内部で呼び出す．|
|set_executing_order|`executing_order` に文字列を設定する|
|clear_executing_order|`executing_order` をクリアする|