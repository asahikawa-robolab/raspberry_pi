* std::threadのコンストラクタを呼び出してスレッドを作る前に定義した変数はスレッド間で共有されるため，排他制御が必須．
* グローバル変数に関しても main 関数が実行される前にメモリが確保されるから排他制御が必要
* const 型の定数を扱う場合には排他制御が不要
    * C++11 においてデータ競合は次のように定義されている
        * (1) 同一メモリ位置に対するアクセスにおいて、
        * (2) 少なくとも一方が変更(modify)操作であり、
        * (3) 異なるスレッド上から同時に行われるとき。
    * 従って変更操作ができない定数に対してはデータ競合が起こりえないため排他制御が不要
    * [参考](https://yohhoy.hatenablog.jp/entry/2013/12/15/204116)


# thread で参照を渡す
* thread で関数の引数に参照を渡すときは std::ref() が必要

# ShareVar について
* mutex によるロックを行ってくれるクラス
* 使用可能なメソッドは read, =, +=, -=, ^=
    * 値を書き込む，変更するときは =, +=, -=, ^=
    * 値を読み出すときは read を使用する
    * =, +=, -=, ^= はオーバーロードを定義しているため，vscode においてエディタのテーマによっては通常とは違う色になる．
    * ++, -- などの演算子は定義していないため使用できない
* サンプルとして jibiki::ShareVar<cv::Point2f> を載せる
    * g_odometry.x += 10;
    * g_odometry += cv::Point2f(10, 0);
* g_imu_reset ^= 1;

# ShareVarVec について
* mutex によるロックを行ってくれるクラス
* ShareVar では使用できない std::vector で排他制御を行うクラス
* テンプレート引数には vector の型を指定する
* =, +=, -=, [] などの演算子は定義されていない

# std::thread でメンバ関数を渡す
* １つ目の引数は「&クラス名::メンバ関数名」
* ２つ目の引数は「this」
* [参考](https://qiita.com/tsuru3/items/c3e706ed77912f809cd2)

# 参考資料
* [std::atomic の参照](https://stackoverflow.com/questions/48551964/c-11-can-you-safely-pass-and-access-stdatomics-by-reference-in-different-thr)